"""SPEC-056 R13 corrective regressions."""
from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
from pathlib import Path
from unittest.mock import patch

import pytest

import local_provenance
import ng_remote_executor as executor


def _receipt_manifest(root: Path, *, output_path: Path) -> tuple[dict, dict]:
    source = root / "source"
    output = root / "build"
    source.mkdir()
    output.mkdir()
    output_path.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2("/bin/true", output_path)
    output_path.chmod(0o755)
    digest = hashlib.sha256(output_path.read_bytes()).hexdigest()
    commands = [
        ["cmake", "-S", str(source), "-B", str(output)],
        ["cmake", "--build", str(output)],
    ]
    captured = []
    for argv in commands:
        record = {
            "argv": argv,
            "cwd": str(source),
            "returncode": 0,
            "stdout_sha256": hashlib.sha256(b"").hexdigest(),
            "stderr_sha256": hashlib.sha256(b"").hexdigest(),
        }
        record["command_sha256"] = local_provenance._receipt_command_digest(record)
        captured.append(record)
    receipt = {
        "schema_version": 1,
        "working_directory": str(source),
        "output_directory": str(output),
        "commands": captured,
        "outputs": {"Role": {"path": str(output_path), "size": output_path.stat().st_size, "sha256": digest}},
    }
    receipt["receipt_sha256"] = hashlib.sha256(
        json.dumps({key: value for key, value in receipt.items() if key != "receipt_sha256"}, sort_keys=True, separators=(",", ":")).encode()
    ).hexdigest()
    manifest = {
        "source": str(source),
        "output": str(output),
        "options": {"configure_only": False},
        "recipe": {"commands": commands, "working_directory": str(source), "executed": True, "returncodes": [0, 0]},
        "build_receipt": receipt,
    }
    return manifest, {"Role": {"path": str(output_path), "size": output_path.stat().st_size, "sha256": digest}}


def _minimal_local_bundle(root: Path) -> None:
    required = {
        "result.json": {"schema_version": 2, "runtime_result": "INCONCLUSIVE", "teardown_result": "FAIL", "evidence_result": "INCOMPLETE", "local_acceptance_eligible": False, "acceptance_blockers": ["test"], "protected_input_blockers": [], "exit_code": 21},
        "provenance.json": {"schema_version": 2},
        "workload.json": {"schema_version": 2},
        "oracle.json": {"schema_version": 2},
        "ownership.json": {"schema_version": 2},
        "cleanup.json": {"schema_version": 2},
        "preservation.json": {"schema_version": 2},
        "controller-events.jsonl": "{\"event\":\"test\"}\n",
    }
    for relative, value in required.items():
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(value if isinstance(value, str) else json.dumps(value) + "\n", encoding="utf-8")
    for relative in ("plan/effective.json", "artifacts/source/_not-captured.json", "artifacts/repository/_not-captured.json", "roles/none/exit.json", "inventory/final.json"):
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("{}\n", encoding="utf-8")


def test_receipt_accepts_executable_directly_under_output_directory(tmp_path: Path):
    manifest, executables = _receipt_manifest(tmp_path, output_path=tmp_path / "build" / "Role")
    assert local_provenance.validate_build_receipt(manifest, executables) is True


def test_receipt_rejects_executable_that_escapes_output_directory(tmp_path: Path):
    manifest, executables = _receipt_manifest(tmp_path, output_path=tmp_path / "escaped" / "Role")
    with pytest.raises(ValueError, match="escapes output directory"):
        local_provenance.validate_build_receipt(manifest, executables)


def test_local_linkage_requires_receipt_before_legacy_fallback():
    compiler = str(Path(shutil.which("g++") or shutil.which("c++")).resolve())
    version = subprocess.run([compiler, "--version"], capture_output=True, text=True, check=True).stdout.splitlines()[0]
    manifest = {
        "mode": "local",
        "recipe": {"commands": [["cmake", "--build", "/build"]], "working_directory": "/src", "executed": True, "returncodes": [0]},
        "toolchain": {"compiler": compiler, "version": version, "version_sha256": hashlib.sha256(version.encode()).hexdigest(), "status": "ok"},
        "options": {"variant": "normal", "jobs": 1, "configure_only": False},
    }
    with patch.object(local_provenance, "validate_build_receipt", side_effect=ValueError("receipt required")) as validate_receipt:
        with pytest.raises(ValueError, match="receipt required"):
            local_provenance.validate_build_linkage(manifest, "head", {})
    validate_receipt.assert_called_once_with(manifest, {})


@pytest.mark.parametrize("fault", ["fsync", "fsync-after-replace", "manifest-reopen", "seal-write"])
def test_publication_faults_remove_all_publication_artifacts_before_failure(tmp_path: Path, fault: str):
    _minimal_local_bundle(tmp_path)
    if fault == "fsync":
        context = patch.object(executor.os, "fsync", side_effect=OSError("fsync fault"))
    elif fault == "fsync-after-replace":
        calls = 0

        def fail_directory_fsync(fd):
            nonlocal calls
            calls += 1
            if calls == 2:
                raise OSError("directory fsync fault")

        context = patch.object(executor.os, "fsync", side_effect=fail_directory_fsync)
    elif fault == "manifest-reopen":
        original_open = Path.open

        def failing_open(path: Path, *args, **kwargs):
            if path.name == "manifest.json" and (args and args[0] == "rb" or kwargs.get("mode") == "rb"):
                raise OSError("reopen fault")
            return original_open(path, *args, **kwargs)

        context = patch.object(Path, "open", failing_open)
    else:
        original_write = executor._durable_atomic_write

        def failing_write(path: Path, data: bytes, **kwargs):
            if path.name == "terminal-seal.json":
                path.write_bytes(data)
                raise OSError("seal write fault")
            return original_write(path, data, **kwargs)

        context = patch.object(executor, "_durable_atomic_write", failing_write)
    with context:
        with pytest.raises(OSError):
            executor.write_evidence_manifest(tmp_path, {"mode": "local", "trial_id": "r13"})
    assert not (tmp_path / "manifest.json").exists()
    assert not (tmp_path / "manifest.sha256").exists()
    assert not (tmp_path / "terminal-seal.json").exists()
    assert not list(tmp_path.glob(".*.tmp"))


def test_abort_records_failure_only_after_publication_cleanup(tmp_path: Path):
    _minimal_local_bundle(tmp_path)
    original_read_bytes = Path.read_bytes

    def tamper_seal(path: Path):
        if path.name == "terminal-seal.json":
            return b"tampered"
        return original_read_bytes(path)

    with patch.object(Path, "read_bytes", tamper_seal):
        with pytest.raises(OSError, match="publication aborted"):
            executor.write_evidence_manifest(tmp_path, {"mode": "local", "trial_id": "r13-abort"})
    assert (tmp_path / "result.json").is_file()
    assert not (tmp_path / "manifest.json").exists()
    assert not (tmp_path / "manifest.sha256").exists()
    assert not (tmp_path / "terminal-seal.json").exists()
    result = json.loads((tmp_path / "result.json").read_text(encoding="utf-8"))
    assert result["evidence_result"] == "INCOMPLETE"
    assert any("terminal seal" in item for item in result["acceptance_blockers"])


def test_quoted_argv_corpus_redacts_unknown_values_in_shell_json_and_python(tmp_path: Path):
    cases = {
        "shell.log": r'''tool --token "shell value with spaces\, escaped\; delimiter and \"quote\""''',
        "argv.json": r'''["--password", "json value with spaces\, escaped\; delimiter and \"quote\""]''',
        "argv.py": r'''("--api-key", 'python value with spaces\, escaped\; delimiter and \'quote\'')''',
    }
    for name, text in cases.items():
        (tmp_path / name).write_text(text, encoding="utf-8")
    report = local_provenance.sanitize_evidence_tree(tmp_path)
    assert report["ok"] is False
    for name, raw in cases.items():
        scrubbed = (tmp_path / name).read_text(encoding="utf-8")
        assert raw.split(maxsplit=2)[-1] not in scrubbed
        assert "<redacted>" in scrubbed
