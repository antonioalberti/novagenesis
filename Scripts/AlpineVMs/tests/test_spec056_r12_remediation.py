"""SPEC-056 R12 bounded fail-closed regressions."""
from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path
from unittest.mock import patch

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import local_provenance
import ng_remote_executor as executor
from evidence_verifier import verify_bundle


def _minimal_local_bundle(root: Path) -> None:
    required = {
        "result.json": {
            "schema_version": 2,
            "runtime_result": "INCONCLUSIVE",
            "teardown_result": "FAIL",
            "evidence_result": "INCOMPLETE",
            "local_acceptance_eligible": False,
            "acceptance_blockers": ["test"],
            "protected_input_blockers": [],
            "exit_code": 21,
        },
        "provenance.json": {"schema_version": 2, "capture_complete": False},
        "workload.json": {"schema_version": 2, "verified": False},
        "oracle.json": {"schema_version": 2, "preservation_verified": False},
        "ownership.json": {"schema_version": 2, "anchor": {"verified": True}},
        "cleanup.json": {"schema_version": 2, "result": "FAIL"},
        "preservation.json": {"schema_version": 2, "verified": False},
        "controller-events.jsonl": "{\"event\":\"test\"}\n",
    }
    for relative, value in required.items():
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        if isinstance(value, str):
            path.write_text(value, encoding="utf-8")
        else:
            path.write_text(json.dumps(value) + "\n", encoding="utf-8")
    for relative in ("plan/effective.json", "artifacts/source/_not-captured.json", "artifacts/repository/_not-captured.json", "roles/none/exit.json", "inventory/final.json"):
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("{}\n", encoding="utf-8")


def test_quarantine_failure_aborts_manifest_and_terminal_seal(tmp_path: Path):
    unsafe = tmp_path / ".env.local"
    unsafe.write_text("TOKEN=must-not-publish\n", encoding="utf-8")
    with patch.object(Path, "unlink", side_effect=PermissionError("quarantine denied")):
        with pytest.raises(OSError, match="publication|protected-input"):
            executor.write_evidence_manifest(tmp_path, {"mode": "local", "trial_id": "r12"})
    assert unsafe.exists()
    assert not (tmp_path / "manifest.json").exists()
    assert not (tmp_path / "terminal-seal.json").exists()


def test_successful_local_manifest_is_atomic_durable_and_reopened(tmp_path: Path):
    _minimal_local_bundle(tmp_path)
    manifest = executor.write_evidence_manifest(tmp_path, {"mode": "local", "trial_id": "r12"})
    seal = tmp_path / "terminal-seal.json"
    assert manifest.is_file() and seal.is_file()
    reopened = json.loads(manifest.read_text(encoding="utf-8"))
    seal_data = json.loads(seal.read_text(encoding="utf-8"))
    assert seal_data["sealed"] is True
    assert seal_data["manifest_sha256"] == __import__("hashlib").sha256(manifest.read_bytes()).hexdigest()
    assert verify_bundle(tmp_path)["integrity"] is True
    assert reopened["schema_version"] == 2


def test_serialized_quoted_argv_and_url_secrets_are_redacted_without_known_values(tmp_path: Path):
    log = tmp_path / "argv.log"
    log.write_text(
        '["--password", "r12-argv-secret"] curl "https://user:r12-url-secret@example.invalid/?token=r12-query-secret"\n',
        encoding="utf-8",
    )
    report = local_provenance.sanitize_evidence_tree(tmp_path)
    text = log.read_text(encoding="utf-8")
    assert report["ok"] is False
    assert "r12-argv-secret" not in text
    assert "r12-url-secret" not in text
    assert "r12-query-secret" not in text
    assert "<redacted>" in text


def test_dotenv_variants_are_protected(tmp_path: Path):
    for name in (".env", ".env.production", "service.env", ".envrc"):
        (tmp_path / name).write_text("protected", encoding="utf-8")
    report = local_provenance.sanitize_evidence_tree(tmp_path)
    assert report["ok"] is False
    assert len(report["quarantined"]) == 4
    assert all(not (tmp_path / name).exists() for name in (".env", ".env.production", "service.env", ".envrc"))


def test_late_detached_descendant_is_owned_by_persistent_anchor():
    anchor = executor.local_ownership_anchor("r12-late")
    assert anchor["verified"] is True
    code = "import os,time; child=os.fork(); (time.sleep(0.1) if child else (os.setsid(), time.sleep(4)))"
    env = os.environ.copy()
    env.update({"NG_ELC_TRIAL_ID": "r12-late", "NG_ELC_TRIAL_ROLE": "Role"})
    leader = subprocess.Popen([sys.executable, "-c", code], env=env, start_new_session=True)
    try:
        assert leader.wait(timeout=3) == 0
        report = executor.local_descendant_reconciliation(
            leader.pid, {}, leader.pid, scans=4, owner_pid=os.getpid(), trial_id="r12-late", role="Role"
        )
        assert report["ok"] is True
        assert report["residual"] == []
        assert report["unknown"] == []
    finally:
        for item in (report.get("tracked", []) if "report" in locals() else []):
            try:
                os.kill(item["pid"], 9)
            except (ProcessLookupError, KeyError):
                pass


def test_build_receipt_rejects_recipe_not_matching_actual_output(tmp_path: Path):
    output = tmp_path / "build"
    output.mkdir()
    manifest = {
        "source": str(tmp_path),
        "output": str(output),
        "options": {"configure_only": False},
        "recipe": {
            "commands": [["cmake", "-S", str(tmp_path), "-B", str(output)], ["cmake", "--build", str(output)]],
            "working_directory": str(tmp_path),
            "executed": True,
            "returncodes": [0, 0],
        },
        "build_receipt": {
            "schema_version": 1,
            "working_directory": str(tmp_path),
            "output_directory": str(output),
            "commands": [{"argv": ["echo", "synthetic"], "cwd": str(tmp_path), "returncode": 0}],
            "outputs": {},
        },
    }
    with pytest.raises(ValueError, match="linked|diverges|receipt"):
        local_provenance.validate_build_receipt(manifest, {})


def test_remote_omitted_mode_keeps_schema_v1_defaults():
    args = executor.build_parser().parse_args(["run", "--plan", "plan.json", "--scenario", "L0"])
    assert args.mode == "remote"
    plan = {
        "schema_version": 1,
        "roles": [{"name": "PGCS", "vm": "source", "command": ["/bin/true"], "readiness": [{"id": "ready", "pattern": "READY"}]}],
        "timeouts": {"readiness": 1, "observation": 0, "total": 1},
        "runtime_oracle": {"type": "markers", "required": [{"role": "PGCS", "id": "ready", "pattern": "READY"}]},
    }
    executor.validate_plan(plan)
