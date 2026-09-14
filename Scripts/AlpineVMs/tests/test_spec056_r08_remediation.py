"""SPEC-056 R08 lifecycle regressions; no VM, SSH, or candidate trial."""
from __future__ import annotations

import hashlib
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any
from unittest.mock import patch

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from local_provenance import capture_git_state, sanitize_config
from ng_remote_executor import (
    _source_identity_matches,
    prepare_local_executable,
    register_local_process,
    write_evidence_manifest,
)


def _git(repo: Path, *args: str) -> None:
    subprocess.run(["git", "-C", str(repo), *args], check=True, capture_output=True)


def _provenance(root: Path) -> tuple[dict[str, Any], Path]:
    repo = root / "repo"
    repo.mkdir()
    source = repo / "source.txt"
    source.write_text("base\n", encoding="utf-8")
    _git(repo, "init", "--quiet")
    _git(repo, "add", "source.txt")
    subprocess.run(["git", "-C", str(repo), "-c", "user.name=t", "-c", "user.email=t@e", "commit", "--quiet", "-m", "initial"], check=True)
    evidence = root / "evidence"
    evidence.mkdir()
    state = capture_git_state(repo, snapshot_dir=evidence / "provenance" / "source-snapshot")
    executable = root / "checked-executable"
    shutil.copy2("/bin/true", executable)
    executable.chmod(0o755)
    manifest = root / "build-manifest.json"
    manifest.write_text("manifest\n", encoding="utf-8")
    preserved = evidence / "provenance" / "build-manifest.json"
    preserved.parent.mkdir(parents=True)
    shutil.copy2(manifest, preserved)
    manifest_identity = {
        "path": str(manifest),
        "size": manifest.stat().st_size,
        "sha256": hashlib.sha256(manifest.read_bytes()).hexdigest(),
        "preserved_path": "provenance/build-manifest.json",
        "preserved_size": preserved.stat().st_size,
        "preserved_sha256": hashlib.sha256(preserved.read_bytes()).hexdigest(),
    }
    binary = {
        "resolved_path": str(executable.resolve()),
        "path": str(executable.resolve()),
        "size": executable.stat().st_size,
        "sha256": hashlib.sha256(executable.read_bytes()).hexdigest(),
    }
    return {
        "build_linkage": True,
        "repo_path": str(repo),
        "evidence_dir": str(evidence),
        "source": state,
        "build_manifest": manifest_identity,
        "binaries": {"Role": binary},
    }, executable


def test_unchanged_source_launches_sealed_image_and_records_identity(tmp_path: Path):
    provenance, executable = _provenance(tmp_path)
    identity = prepare_local_executable(provenance, "Role", str(executable))
    assert identity["representation"] == "sealed-memfd"
    assert identity["sha256"] == provenance["binaries"]["Role"]["sha256"]
    assert identity["proc_path"].startswith("/proc/self/fd/")
    try:
        child = subprocess.Popen([identity["proc_path"]], pass_fds=(identity["fd"],))
        assert child.wait(timeout=5) == 0
    finally:
        os.close(identity["fd"])


def test_executable_replacement_fails_closed_before_launch(tmp_path: Path):
    provenance, executable = _provenance(tmp_path)
    executable.write_bytes(b"replacement")
    executable.chmod(0o755)
    with pytest.raises(ValueError, match="drift|immutable"):
        prepare_local_executable(provenance, "Role", str(executable))


def test_actual_index_only_drift_is_detected(tmp_path: Path):
    provenance, _ = _provenance(tmp_path)
    repo = Path(provenance["repo_path"])
    source = repo / "source.txt"
    source.write_text("staged\n", encoding="utf-8")
    _git(repo, "add", "source.txt")
    source.write_text("worktree\n", encoding="utf-8")
    provenance["source"] = capture_git_state(repo, snapshot_dir=Path(provenance["evidence_dir"]) / "provenance" / "source-snapshot")
    source.write_text("other-staged\n", encoding="utf-8")
    _git(repo, "add", "source.txt")
    source.write_text("worktree\n", encoding="utf-8")
    matched, reason = _source_identity_matches(provenance)
    assert matched is False
    assert "index" in (reason or "") and "drift" in (reason or "")


def test_url_source_assignment_and_expanded_argv_secrets_are_absent():
    secret = "R08-URL-SECRET"
    safe = sanitize_config({
        "source": f"SOURCE_URL=https://user:{secret}@example.invalid/input",
        "argv": ["tool", "--token", secret, "https://user:{0}@example.invalid/?x=1".format(secret)],
        "command": f"SOURCE_TOKEN={secret} tool --token={secret}",
    })
    assert secret not in json.dumps(safe, sort_keys=True)
    assert "<redacted>" in json.dumps(safe)


def test_process_identity_failure_rolls_back_registered_child(tmp_path: Path):
    stdout = (tmp_path / "stdout").open("w")
    stderr = (tmp_path / "stderr").open("w")
    child = subprocess.Popen([sys.executable, "-c", "import time; time.sleep(30)"], stdout=stdout, stderr=stderr)
    processes: dict[str, dict[str, object]] = {}
    with patch("ng_remote_executor.os.getpgid", side_effect=PermissionError("identity denied")):
        with pytest.raises(RuntimeError, match="registration failed"):
            register_local_process(processes, "Role", child, ["child"], stdout, stderr, tmp_path / "stdout", tmp_path / "stderr", {"representation": "sealed-memfd"})
    assert processes == {}
    assert child.poll() is not None


def test_remote_schema_v1_manifest_contract_is_preserved(tmp_path: Path):
    (tmp_path / "payload.txt").write_text("payload", encoding="utf-8")
    manifest = write_evidence_manifest(tmp_path, {"mode": "remote", "trial_id": "r"})
    data = json.loads(manifest.read_text(encoding="utf-8"))
    assert data["schema_version"] == 1
    assert (tmp_path / "manifest.sha256").is_file()
    assert not (tmp_path / "terminal-seal.json").exists()
