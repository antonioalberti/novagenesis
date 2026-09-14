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

from local_provenance import capture_git_state, sanitize_config, sanitize_evidence_tree, validate_build_linkage
from ng_remote_executor import build_parser, register_local_process, run_local_trial, write_evidence_manifest


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
    roles = {name: {"path": f"/build/{name}", "sha256": ("abcdef0123456789"[index] * 64)} for index, name in enumerate(("PGCS", "NRNCS", "Repository", "Source"))}
    return {
        "schema_version": 1,
        "source_head": source["head"],
        "source_snapshot": source,
        "recipe": {"commands": [["cmake", "--build", "build"]], "working_directory": "/src"},
        "toolchain": {"compiler": "/usr/bin/g++", "version": "13.2.0", "status": "ok"},
        "options": {"variant": "normal", "jobs": 1, "configure_only": False},
        "runtime_library_identity": {"method": "ldd", "binaries": {role: {"status": "ok", "output_sha256": "a" * 64, "libraries": ["libc.so"]} for role in roles}},
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


def test_quoted_assignment_values_are_redacted():
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
    assert safe["ok"] is True
    for path in tmp_path.rglob("*"):
        if path.is_file():
            assert secret not in path.read_text(encoding="utf-8")

    binary = tmp_path / "roles" / "opaque.bin"
    binary.write_bytes(b"prefix\0" + secret.encode() + b"\0suffix")
    unsafe = sanitize_evidence_tree(tmp_path, {secret})
    assert unsafe["ok"] is False
    assert any("protected-input" in item for item in unsafe["blockers"])
    assert not binary.exists()


def test_identity_failure_retains_uncertain_record_and_descendant_failure(tmp_path: Path):
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
    args = build_parser().parse_args(["run", "--plan", "plan.json", "--scenario", "L0"])
    assert args.mode == "remote"


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
        (repo / "input.txt").write_text("fixture\n", encoding="utf-8")
        subprocess.run(["git", "init", "--quiet", str(repo)], check=True)
        subprocess.run(["git", "-C", str(repo), "add", "input.txt"], check=True)
        subprocess.run(["git", "-C", str(repo), "-c", "user.name=t", "-c", "user.email=t@e", "commit", "--quiet", "-m", "fixture"], check=True)
        state = capture_git_state(repo, snapshot_dir=root / "source-snapshot")
        executable = Path(sys.executable).resolve()
        executable_hash = hashlib.sha256(executable.read_bytes()).hexdigest()
        roles = ["PGCS", "NRNCS", "Repository", "Source"]
        plan_roles = [{
            "name": role,
            "vm": "local",
            "command": [str(executable), "-c", "import time; print('READY', flush=True); time.sleep(1)"],
            "cwd": str(build),
            "readiness": [{"id": "ready", "pattern": "READY"}],
        } for role in roles]
        manifest = {
            "schema_version": 1,
            "source_head": state["head"],
            "source_snapshot": state,
            "recipe": {"commands": [["cmake", "--build", str(build)]], "working_directory": str(repo)},
            "toolchain": {"compiler": str(executable), "version": "fixture", "status": "ok"},
            "options": {"variant": "normal", "jobs": 1, "configure_only": False},
            "runtime_library_identity": {"method": "fixture", "binaries": {role: {"status": "ok", "sha256": executable_hash, "libraries": ["fixture-runtime"]} for role in roles}},
            "binaries": {role: {"path": str(executable), "size": executable.stat().st_size, "sha256": executable_hash} for role in roles},
        }
        manifest_path = root / "build-manifest.json"
        manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
        plan_path = root / "plan.json"
        plan_path.write_text(json.dumps({
            "schema_version": 1,
            "roles": plan_roles,
            "timeouts": {"readiness": 2, "observation": 0.1, "total": 5},
            "diagnostic_only": True,
        }), encoding="utf-8")
        env = {
            "NG_LOCAL_REPO_PATH": str(repo), "NG_LOCAL_BUILD_PATH": str(build),
            "NG_LOCAL_IO_PATH": str(io), "NG_LOCAL_EVIDENCE_PATH": str(evidence),
            "NG_LOCAL_BUILD_MANIFEST": str(manifest_path),
        }
        args = Namespace(plan=str(plan_path), scenario="local-intra-os", debug_profile="obs-normal", trial="r09-four-role", env=env)
        assert run_local_trial(args) == 11
        events = [(json.loads(line))["event"] for line in (evidence / "r09-four-role" / "controller-events.jsonl").read_text().splitlines()]
        assert events.count("launch") == 4
        assert events.count("readiness") == 4
    finally:
        shutil.rmtree(root, ignore_errors=True)
