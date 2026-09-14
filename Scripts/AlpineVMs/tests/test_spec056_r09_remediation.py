"""SPEC-056 R09 bounded remediation regressions."""
from __future__ import annotations

import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile
from argparse import Namespace
from pathlib import Path
from unittest.mock import patch

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import local_provenance
from local_provenance import capture_git_state, sanitize_config, sanitize_evidence_tree, validate_build_linkage
from ng_remote_executor import build_parser, register_local_process, run_local_trial, validate_plan, write_evidence_manifest
from evidence_verifier import verify_bundle


def _evidence() -> dict[str, object]:
    return {
        "path": "/build-manifest.json",
        "size": 10,
        "sha256": "a" * 64,
        "preserved_path": "provenance/build-manifest.json",
        "preserved_size": 10,
        "preserved_sha256": "b" * 64,
    }


def _complete_manifest() -> dict[str, object]:
    compiler = str(Path(shutil.which("g++") or shutil.which("c++")).resolve())
    version = subprocess.run([compiler, "--version"], capture_output=True, text=True, check=True).stdout.splitlines()[0]
    binary = Path("/bin/true")
    binary_hash = hashlib.sha256(binary.read_bytes()).hexdigest()
    ldd_run = subprocess.run(["ldd", str(binary)], capture_output=True, text=True, check=True)
    ldd_output = ldd_run.stdout + ldd_run.stderr
    stable_output = local_provenance._normalise_loader_output(ldd_output)
    ldd_hash = hashlib.sha256(stable_output.encode()).hexdigest()
    libraries = sorted(line.strip() for line in stable_output.splitlines() if line.strip())
    index = {
        "captured": True,
        "sha256": "d" * 64,
        "entries": [{"path": "input.txt", "mode": "100644", "blob_id": "c" * 40, "stage": 0}],
        "errors": [],
    }
    source = {
        "head": "a" * 40,
        "status": [],
        "tree_sha256": "e" * 64,
        "submodules": [],
        "submodules_complete": True,
        "index": index,
        "index_sha256": index["sha256"],
        "index_entries": index["entries"],
        "content_snapshot": {"captured": True, "files": [], "entries": [], "errors": []},
    }
    roles = {name: {"path": "/bin/true", "sha256": binary_hash} for name in ("PGCS", "NRNCS", "Repository", "Source")}
    return {
        "schema_version": 1,
        "source_head": source["head"],
        "source_snapshot": source,
        "recipe": {"commands": [["cmake", "--build", "build"]], "working_directory": "/src", "executed": True, "returncodes": [0]},
        "toolchain": {"compiler": compiler, "version": version, "version_sha256": hashlib.sha256(version.encode()).hexdigest(), "status": "ok"},
        "options": {"variant": "normal", "jobs": 1, "configure_only": False},
        "runtime_library_identity": {"method": "ldd", "binaries": {role: {"status": "ok", "output_sha256": ldd_hash, "binary_sha256": binary_hash, "libraries": libraries} for role in roles}},
        "binaries": roles,
    }


def test_empty_and_placeholder_identity_structures_fail_closed():
    manifest = _complete_manifest()
    for field, replacement in (
        ("toolchain", {"compiler": "", "version": "13"}),
        ("options", {"cmake_args": []}),
        ("runtime_library_identity", {"binaries": {"PGCS": {}}}),
    ):
        candidate = dict(manifest)
        candidate[field] = replacement
        with pytest.raises(ValueError):
            validate_build_linkage(candidate, "a" * 40, candidate["binaries"], manifest_evidence=_evidence())

    candidate = dict(manifest)
    candidate["toolchain"] = {"compiler": "unknown", "version": "13"}
    with pytest.raises(ValueError, match="placeholder|toolchain"):
        validate_build_linkage(candidate, "a" * 40, candidate["binaries"], manifest_evidence=_evidence())


def test_complete_four_role_manifest_requires_and_accepts_per_role_runtime_identity():
    manifest = _complete_manifest()
    executables = manifest["binaries"]
    assert validate_build_linkage(manifest, "a" * 40, executables, source_state=manifest["source_snapshot"], manifest_evidence=_evidence()) is True

    incomplete = json.loads(json.dumps(manifest))
    del incomplete["runtime_library_identity"]["binaries"]["Source"]
    with pytest.raises(ValueError, match="coverage.*Source"):
        validate_build_linkage(incomplete, "a" * 40, executables, source_state=manifest["source_snapshot"], manifest_evidence=_evidence())


def test_manifest_source_index_linkage_is_explicit():
    manifest = _complete_manifest()
    broken = json.loads(json.dumps(manifest))
    broken["source_snapshot"]["index_sha256"] = "x" * 64
    with pytest.raises(ValueError, match="index"):
        validate_build_linkage(broken, "a" * 40, broken["binaries"], source_state=broken["source_snapshot"], manifest_evidence=_evidence())


def test_manifest_identity_rejects_numeric_boolean_nested_values_and_unlinked_index():
    manifest = _complete_manifest()
    for field, replacement in (
        ("toolchain", {"compiler": "/usr/bin/g++", "version": 13, "status": "ok"}),
        ("options", {"variant": "normal", "jobs": False, "configure_only": False}),
        ("runtime_library_identity", {"method": "ldd", "binaries": {role: {"status": "ok", "sha256": "a" * 64, "libraries": [False]} for role in ("PGCS", "NRNCS", "Repository", "Source")}}),
    ):
        candidate = json.loads(json.dumps(manifest))
        candidate[field] = replacement
        with pytest.raises(ValueError):
            validate_build_linkage(candidate, "a" * 40, candidate["binaries"], source_state=candidate["source_snapshot"], manifest_evidence=_evidence())

    candidate = json.loads(json.dumps(manifest))
    candidate["source_snapshot"]["index"]["entries"][0]["stage"] = True
    with pytest.raises(ValueError, match="index"):
        validate_build_linkage(candidate, "a" * 40, candidate["binaries"], source_state=candidate["source_snapshot"], manifest_evidence=_evidence())

    candidate = json.loads(json.dumps(manifest))
    independent = json.loads(json.dumps(candidate["source_snapshot"]))
    independent["index_sha256"] = "f" * 64
    with pytest.raises(ValueError, match="index"):
        validate_build_linkage(candidate, "a" * 40, candidate["binaries"], source_state=independent, manifest_evidence=_evidence())


    secret = "R09-quoted-secret"
    rendered = sanitize_config({
        "command": f'SOURCE_TOKEN="{secret}" tool --token="{secret}"',
        "argv": [f'SOURCE_TOKEN="{secret}"'],
    })
    text = json.dumps(rendered)
    assert secret not in text
    assert "<redacted>" in text


def test_local_bundle_scan_redacts_logs_and_reports_unpreservable_input(tmp_path: Path):
    secret = "R09-bundle-secret"
    (tmp_path / "roles").mkdir()
    log = tmp_path / "roles" / "stdout.log"
    log.write_text(f'SOURCE_TOKEN="{secret}"\n', encoding="utf-8")
    (tmp_path / "roles" / "launch.json").write_text(json.dumps({"argv": [f'SOURCE_TOKEN="{secret}"']}), encoding="utf-8")
    (tmp_path / "controller-events.jsonl").write_text(json.dumps({"event": "error", "error": f"SOURCE_TOKEN=\"{secret}\""}) + "\n", encoding="utf-8")
    safe = sanitize_evidence_tree(tmp_path, {secret})
    assert safe["ok"] is False
    assert any("rewritten" in item for item in safe["blockers"])
    for path in tmp_path.rglob("*"):
        if path.is_file():
            assert secret not in path.read_text(encoding="utf-8")

    binary = tmp_path / "roles" / "opaque.bin"
    binary.write_bytes(b"prefix\0" + secret.encode() + b"\0suffix")
    unsafe = sanitize_evidence_tree(tmp_path, {secret})
    assert unsafe["ok"] is False
    assert any("protected-input" in item for item in unsafe["blockers"])
    assert not binary.exists()


def test_bundle_scan_rejects_secret_bearing_filename_and_rewrite_failure(tmp_path: Path):
    secret_path = tmp_path / "roles" / "argv-token.log"
    secret_path.parent.mkdir()
    secret_path.write_text("ordinary text", encoding="utf-8")
    unsafe = sanitize_evidence_tree(tmp_path, set())
    assert unsafe["ok"] is False
    assert any("filename" in item for item in unsafe["blockers"])
    assert not secret_path.exists()

    rewrite = tmp_path / "roles" / "stdout.log"
    rewrite.write_text('TOKEN="rewrite-secret"\n', encoding="utf-8")
    with patch("local_provenance.Path.write_text", side_effect=OSError("read-only evidence")):
        failed = sanitize_evidence_tree(tmp_path, {"rewrite-secret"})
    assert failed["ok"] is False
    assert any("rewrite" in item or "quarantine" in item for item in failed["blockers"])
    assert not rewrite.exists()


    stdout = (tmp_path / "stdout").open("w")
    stderr = (tmp_path / "stderr").open("w")
    child = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"], stdout=stdout, stderr=stderr)
    processes: dict[str, dict[str, object]] = {}
    try:
        with patch("ng_remote_executor.os.getpgid", side_effect=PermissionError("identity denied")), patch("ng_remote_executor.subprocess.Popen.kill", side_effect=PermissionError("kill denied")):
            with pytest.raises(RuntimeError, match="registration failed"):
                register_local_process(processes, "Role", child, ["child"], stdout, stderr, tmp_path / "stdout", tmp_path / "stderr", {"representation": "sealed-memfd"})
        record = processes["Role"]
        assert record["rollback_uncertain"] is True
        assert record["unresolved"] is True
        assert "tracked_descendants" in record
    finally:
        try:
            child.kill()
        except ProcessLookupError:
            pass
        child.wait(timeout=5)
        stdout.close()
        stderr.close()


def test_remote_cli_defaults_and_schema_v1_manifest_remain_unchanged(tmp_path: Path):
    (tmp_path / "payload.txt").write_text("payload", encoding="utf-8")
    manifest = write_evidence_manifest(tmp_path, {"mode": "remote", "trial_id": "r"})
    assert json.loads(manifest.read_text(encoding="utf-8"))["schema_version"] == 1
    assert (tmp_path / "manifest.sha256").is_file()
    assert not (tmp_path / "terminal-seal.json").exists()
    args = build_parser().parse_args(["run", "--mode", "remote", "--plan", "plan.json", "--scenario", "L0"])
    assert args.mode == "remote"
    remote_plan = {
        "schema_version": 1,
        "roles": [{"name": "PGCS", "vm": "source", "command": ["/bin/true"], "readiness": [{"id": "ready", "pattern": "READY"}]}],
        "timeouts": {"readiness": 1, "observation": 0, "total": 1},
        "runtime_oracle": {"type": "markers", "required": [{"role": "PGCS", "id": "ready", "pattern": "READY"}]},
    }
    validate_plan(remote_plan, mode="remote")
    with pytest.raises(ValueError, match="shell"):
        validate_plan({**remote_plan, "roles": [{**remote_plan["roles"][0], "shell": "echo READY"}]}, mode="remote")


def test_four_role_lifecycle_fixture_reaches_each_readiness_gate():
    root = Path(tempfile.mkdtemp(prefix="ng-elc-r09-four-role-", dir=Path.home()))
    try:
        repo = root / "repo"
        build = root / "build"
        io = root / "io"
        evidence = root / "evidence"
        repo.mkdir()
        build.mkdir()
        io.mkdir()
        source = root / "fixture.c"
        executable = build / "fixture-role"
        source.write_text("#include <stdio.h>\n#include <unistd.h>\nint main(void) { puts(\"READY\"); fflush(stdout); sleep(1); puts(\"OBSERVE\"); fflush(stdout); sleep(5); return 0; }\n", encoding="utf-8")
        compiler = shutil.which("cc") or shutil.which("gcc")
        if compiler is None:
            pytest.fail("positive control cannot run: no C compiler is available to build its real four-role fixture")
        subprocess.run([compiler, str(source), "-O0", "-o", str(executable)], check=True)
        (repo / "input.txt").write_text("fixture\n", encoding="utf-8")
        subprocess.run(["git", "init", "--quiet", str(repo)], check=True)
        subprocess.run(["git", "-C", str(repo), "add", "input.txt"], check=True)
        subprocess.run(["git", "-C", str(repo), "-c", "user.name=t", "-c", "user.email=t@e", "commit", "--quiet", "-m", "fixture"], check=True)
        state = capture_git_state(repo, snapshot_dir=root / "source-snapshot")
        executable_hash = hashlib.sha256(executable.read_bytes()).hexdigest()
        compiler = str(Path(compiler).resolve())
        compiler_version = subprocess.run([compiler, "--version"], capture_output=True, text=True, check=True).stdout.splitlines()[0]
        ldd_output = subprocess.run(["ldd", str(executable)], capture_output=True, text=True, check=True)
        ldd_text = ldd_output.stdout + ldd_output.stderr
        stable_ldd = local_provenance._normalise_loader_output(ldd_text)
        ldd_hash = hashlib.sha256(stable_ldd.encode()).hexdigest()
        roles = ["PGCS", "NRNCS", "Repository", "Source"]
        plan_roles = [{
            "name": role,
            "vm": "local",
            "command": [str(executable)],
            "cwd": str(build),
            "readiness": [{"id": "ready", "pattern": "READY"}],
        } for role in roles]
        receipt_commands = []
        build_commands = [["cmake", "-S", str(repo), "-B", str(build)], ["cmake", "--build", str(build)]]
        for command in build_commands:
            captured = {"argv": command, "cwd": str(repo), "returncode": 0, "stdout_sha256": hashlib.sha256(b"").hexdigest(), "stderr_sha256": hashlib.sha256(b"").hexdigest()}
            captured["command_sha256"] = local_provenance._receipt_command_digest(captured)
            receipt_commands.append(captured)
        receipt = {"schema_version": 1, "working_directory": str(repo), "output_directory": str(build), "commands": receipt_commands, "outputs": {"PGCS": {"path": str(executable), "size": executable.stat().st_size, "sha256": executable_hash}, "NRNCS": {"path": str(executable), "size": executable.stat().st_size, "sha256": executable_hash}, "Repository": {"path": str(executable), "size": executable.stat().st_size, "sha256": executable_hash}, "Source": {"path": str(executable), "size": executable.stat().st_size, "sha256": executable_hash}}}
        receipt["receipt_sha256"] = hashlib.sha256(json.dumps({key: value for key, value in receipt.items() if key != "receipt_sha256"}, sort_keys=True, separators=(",", ":")).encode()).hexdigest()
        manifest = {
            "schema_version": 1,
            "source": str(repo),
            "output": str(build),
            "source_head": state["head"],
            "source_snapshot": state,
            "recipe": {"commands": build_commands, "working_directory": str(repo), "executed": True, "returncodes": [0, 0]},
            "build_receipt": receipt,
            "toolchain": {"compiler": compiler, "version": compiler_version, "version_sha256": hashlib.sha256(compiler_version.encode()).hexdigest(), "status": "ok"},
            "options": {"variant": "normal", "jobs": 1, "configure_only": False},
            "runtime_library_identity": {"method": "ldd", "binaries": {role: {"status": "ok", "output_sha256": ldd_hash, "binary_sha256": executable_hash, "libraries": [line.strip() for line in stable_ldd.splitlines() if line.strip()]} for role in roles}},
            "binaries": {role: {"path": str(executable), "size": executable.stat().st_size, "sha256": executable_hash} for role in roles},
        }
        manifest_path = root / "build-manifest.json"
        manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
        plan_path = root / "plan.json"
        plan_path.write_text(json.dumps({
            "schema_version": 1,
            "roles": plan_roles,
            "workload": {
                "schema_version": 1,
                "generator": {"id": "synthetic-jpeg", "version": "1.0", "seed": 56010, "parameters": {"count": 5, "size_bytes": 32, "prefix": "positive-control"}},
                "names": [f"positive-control-{index}.jpg" for index in range(1, 6)],
            },
            "runtime_oracle": {"type": "markers", "required": [{"role": role, "id": "observed", "pattern": "OBSERVE"} for role in roles]},
            "timeouts": {"readiness": 2, "observation": 2, "total": 15},
            "diagnostic_only": False,
        }), encoding="utf-8")
        env = {
            "NG_LOCAL_REPO_PATH": str(repo), "NG_LOCAL_BUILD_PATH": str(build),
            "NG_LOCAL_IO_PATH": str(io), "NG_LOCAL_EVIDENCE_PATH": str(evidence),
            "NG_LOCAL_BUILD_MANIFEST": str(manifest_path),
        }
        args = Namespace(plan=str(plan_path), scenario="local-intra-os", debug_profile="obs-normal", trial="r10-four-role", env=env)
        rc = run_local_trial(args)
        assert rc == 0
        bundle = evidence / "r10-four-role"
        records = [json.loads(line) for line in (bundle / "controller-events.jsonl").read_text().splitlines()]
        events = [record["event"] for record in records]
        assert events.count("launch") == 4
        assert events.count("readiness") == 4
        assert events.count("runtime-marker") >= 4
        assert [record["role"] for record in records if record["event"] == "stop"] == list(reversed(roles))
        result = json.loads((bundle / "result.json").read_text(encoding="utf-8"))
        assert result["runtime_result"] == "PASS"
        assert result["teardown_result"] == "PASS"
        assert result["evidence_result"] == "COMPLETE"
        assert result["offline_verification"]["accepted"] is True
        sealed_report = verify_bundle(bundle)
        assert sealed_report["code"] == "NGELC-VALID"
        assert sealed_report["accepted"] is True
        assert not any("token" in path.name.lower() or "secret" in path.name.lower() for path in bundle.rglob("*") if path.is_file())
        assert json.loads((bundle / "terminal-seal.json").read_text(encoding="utf-8"))["sealed"] is True
    finally:
        shutil.rmtree(root, ignore_errors=True)
