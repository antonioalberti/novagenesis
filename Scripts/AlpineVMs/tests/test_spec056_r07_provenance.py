"""SPEC-056 R07 Astra remediation regressions.

These tests are helper/lifecycle-level only: no VM, SSH, or candidate trial.
"""
from __future__ import annotations

import hashlib
import json
import subprocess
from pathlib import Path
import pytest

import local_provenance
from local_provenance import (
    _capture_content_snapshot,
    capture_git_state,
    sanitize_config,
    validate_build_linkage,
)
from ng_remote_executor import local_executable_linkage, write_evidence_manifest


def _complete_manifest(tmp_path: Path) -> tuple[dict[str, object], dict[str, object]]:
    compiler = str(Path(subprocess.run(["which", "g++"], capture_output=True, text=True, check=True).stdout.strip()).resolve())
    version = subprocess.run([compiler, "--version"], capture_output=True, text=True, check=True).stdout.splitlines()[0]
    binary = Path("/bin/true")
    binary_hash = hashlib.sha256(binary.read_bytes()).hexdigest()
    ldd_run = subprocess.run(["ldd", str(binary)], capture_output=True, text=True, check=True)
    ldd_output = ldd_run.stdout + ldd_run.stderr
    stable_output = local_provenance._normalise_loader_output(ldd_output)
    ldd_hash = hashlib.sha256(stable_output.encode()).hexdigest()
    libraries = sorted(line.strip() for line in stable_output.splitlines() if line.strip())
    manifest_path = tmp_path / "build-manifest.json"
    manifest_path.write_text("manifest bytes\n", encoding="utf-8")
    evidence = {"path": str(manifest_path), "size": manifest_path.stat().st_size,
                "sha256": hashlib.sha256(manifest_path.read_bytes()).hexdigest()}
    manifest = {
        "schema_version": 1,
        "source_head": "abc123",
        "source_snapshot": {
            "head": "abc123", "status": [], "tree_sha256": "b" * 64, "submodules": [], "submodules_complete": True,
            "index": {"captured": True, "sha256": "a" * 64, "entries": [{"path": "input.txt", "mode": "100644", "blob_id": "c" * 40, "stage": 0}]},
            "index_sha256": "a" * 64, "index_entries": [{"path": "input.txt", "mode": "100644", "blob_id": "c" * 40, "stage": 0}],
            "content_snapshot": {"captured": True, "files": [], "entries": [], "errors": []},
        },
        "recipe": {"commands": [["cmake", "--build", "build"]], "working_directory": "/src", "executed": True, "returncodes": [0]},
        "toolchain": {"compiler": compiler, "version": version, "version_sha256": hashlib.sha256(version.encode()).hexdigest(), "status": "ok"},
        "options": {"build_type": "RelWithDebInfo"},
        "runtime_library_identity": {"method": "ldd", "binaries": {"Source": {"status": "ok", "ldd_sha256": ldd_hash, "binary_sha256": binary_hash, "libraries": libraries}}},
        "binaries": {"Source": {"path": "/bin/true", "sha256": binary_hash}},
    }
    evidence.update({"preserved_path": "provenance/build-manifest.json", "preserved_size": evidence["size"], "preserved_sha256": evidence["sha256"]})
    return manifest, {"manifest": evidence}


def test_complete_manifest_control_is_accepted(tmp_path: Path):
    manifest, preserved = _complete_manifest(tmp_path)
    assert validate_build_linkage(
        manifest, "abc123", {"Source": {"path": "/bin/true", "sha256": manifest["binaries"]["Source"]["sha256"]}},
        source_state=manifest["source_snapshot"], manifest_evidence=preserved["manifest"],
    ) is True


@pytest.mark.parametrize("field", ["source_snapshot", "toolchain", "options", "runtime_library_identity"])
def test_linkage_rejects_missing_required_identity(field: str, tmp_path: Path):
    manifest, preserved = _complete_manifest(tmp_path)
    manifest.pop(field)
    with pytest.raises(ValueError, match="identity|snapshot|toolchain|options"):
        validate_build_linkage(
            manifest, "abc123", {"Source": {"path": "/bin/true", "sha256": manifest["binaries"]["Source"]["sha256"]}},
            source_state=manifest.get("source_snapshot"), manifest_evidence=preserved["manifest"],
        )


def test_linkage_rejects_missing_preserved_manifest_evidence(tmp_path: Path):
    manifest, _ = _complete_manifest(tmp_path)
    with pytest.raises(ValueError, match="manifest evidence"):
        validate_build_linkage(manifest, "abc123", {"Source": {"path": "/bin/true", "sha256": manifest["binaries"]["Source"]["sha256"]}}, source_state=manifest["source_snapshot"])


def test_linkage_rejects_distinct_same_basename_executables():
    manifest = {
        "source_head": "abc123",
        "source_snapshot": {"head": "abc123", "status": [], "tree_sha256": "b" * 64, "submodules": [], "submodules_complete": True,
                            "content_snapshot": {"captured": True, "files": [], "errors": []}},
        "recipe": {"commands": [["build"]]}, "toolchain": {"id": "tc"}, "options": {"id": "opts"},
        "runtime_library_identity": {"id": "rt"},
        "binaries": {
            "left": {"path": "/left/tool", "sha256": "c" * 64},
            "right": {"path": "/right/tool", "sha256": "d" * 64},
        },
    }
    evidence = {"path": "/manifest", "size": 1, "sha256": "e" * 64, "preserved_path": "provenance/build-manifest.json", "preserved_size": 1, "preserved_sha256": "e" * 64}
    with pytest.raises(ValueError, match="collision|ambiguous"):
        validate_build_linkage(
            manifest, "abc123",
            {"RoleA": {"path": "/left/tool", "sha256": "c" * 64, "manifest_name": "tool"},
             "RoleB": {"path": "/right/tool", "sha256": "d" * 64, "manifest_name": "tool"}},
            source_state=manifest["source_snapshot"], manifest_evidence=evidence,
        )


def test_dirty_snapshot_hashes_copy_and_rehashes_independently(tmp_path: Path):
    repo = tmp_path / "repo"
    repo.mkdir()
    source = repo / "dirty.txt"
    source.write_bytes(b"stable bytes")
    destination = tmp_path / "snapshot"
    snapshot = _capture_content_snapshot(repo, [" M dirty.txt"], destination)
    item = snapshot["files"][0]
    copy = destination / "dirty.txt"
    assert item["sha256"] == hashlib.sha256(copy.read_bytes()).hexdigest()
    assert item["rehash_sha256"] == item["sha256"]
    assert snapshot["captured"] is True


def test_dirty_snapshot_blocks_racing_source(monkeypatch, tmp_path: Path):
    repo = tmp_path / "repo"
    repo.mkdir()
    source = repo / "dirty.txt"
    source.write_bytes(b"bytes")
    destination = tmp_path / "snapshot"
    monkeypatch.setattr(local_provenance, "_copy_stable_bytes", lambda *_args: (_ for _ in ()).throw(ValueError("source race while capturing")))
    snapshot = _capture_content_snapshot(repo, [" M dirty.txt"], destination)
    assert snapshot["captured"] is False
    assert any("race" in error for error in snapshot["errors"])


def test_dirty_snapshot_represents_deletion_without_claiming_complete(tmp_path: Path):
    repo = tmp_path / "repo"
    repo.mkdir()
    snapshot = _capture_content_snapshot(repo, [" D deleted.txt"], tmp_path / "snapshot")
    assert snapshot["captured"] is False
    assert snapshot["entries"][0]["state"] == "deleted"


def test_dirty_snapshot_preserves_staged_and_worktree_versions(tmp_path: Path):
    repo = tmp_path / "repo"
    repo.mkdir()
    source = repo / "input.txt"
    source.write_text("base\n", encoding="utf-8")
    subprocess.run(["git", "init", "--quiet", str(repo)], check=True)
    subprocess.run(["git", "-C", str(repo), "add", "input.txt"], check=True)
    subprocess.run(["git", "-C", str(repo), "-c", "user.name=t", "-c", "user.email=t@e", "commit", "--quiet", "-m", "initial"], check=True)
    source.write_text("staged\n", encoding="utf-8")
    subprocess.run(["git", "-C", str(repo), "add", "input.txt"], check=True)
    source.write_text("worktree\n", encoding="utf-8")
    destination = tmp_path / "snapshot"
    snapshot = capture_git_state(repo, snapshot_dir=destination)
    item = snapshot["content_snapshot"]["files"][0]
    assert item["staged_sha256"] == hashlib.sha256((destination / "staged" / "input.txt").read_bytes()).hexdigest()
    assert item["sha256"] == hashlib.sha256((destination / "input.txt").read_bytes()).hexdigest()
    assert snapshot["content_snapshot"]["captured"] is True


def test_dirty_snapshot_represents_submodule_as_unreconstructable(tmp_path: Path):
    repo = tmp_path / "repo"
    repo.mkdir()
    submodule = repo / "nested"
    submodule.mkdir()
    (submodule / ".git").write_text("gitdir: ../.git/modules/nested\n", encoding="utf-8")
    snapshot = _capture_content_snapshot(repo, [" M nested"], tmp_path / "snapshot")
    assert snapshot["captured"] is False
    assert snapshot["entries"][0]["state"] == "submodule"


def test_secret_safe_capture_redacts_argv_urls_and_command_strings():
    result = sanitize_config({
        "argv": ["tool", "--token", "super-secret", "https://example.invalid/api?password=super-secret&ok=1"],
        "command": "curl -H 'Authorization: Bearer super-secret' https://example.invalid/?token=super-secret",
        "API_TOKEN": "super-secret",
    })
    published = json.dumps(result, sort_keys=True)
    assert "super-secret" not in published
    assert "<redacted>" in published


def test_secret_bearing_dirty_source_is_protected_not_published(tmp_path: Path):
    repo = tmp_path / "repo"
    repo.mkdir()
    source = repo / "config.env"
    source.write_bytes(b"API_TOKEN=super-secret\n")
    snapshot = _capture_content_snapshot(repo, ["?? config.env"], tmp_path / "snapshot")
    assert snapshot["captured"] is False
    assert snapshot["entries"][0]["state"] == "protected-input"
    assert not (tmp_path / "snapshot" / "config.env").exists()


def test_missing_build_linkage_is_not_launch_eligible(tmp_path: Path):
    executable = tmp_path / "tool"
    executable.write_bytes(b"tool")
    linked, reason = local_executable_linkage({"build_linkage": False}, "Role", str(executable))
    assert linked is False
    assert "missing" in (reason or "")


def test_launch_recheck_rejects_source_drift(tmp_path: Path):
    repo = tmp_path / "repo"
    repo.mkdir()
    source = repo / "input.txt"
    source.write_text("before\n", encoding="utf-8")
    subprocess.run(["git", "init", "--quiet", str(repo)], check=True)
    subprocess.run(["git", "-C", str(repo), "add", "input.txt"], check=True)
    subprocess.run(["git", "-C", str(repo), "-c", "user.name=t", "-c", "user.email=t@e", "commit", "--quiet", "-m", "initial"], check=True)
    evidence = tmp_path / "evidence"
    (evidence / "provenance").mkdir(parents=True)
    manifest_path = tmp_path / "build-manifest.json"
    manifest_path.write_bytes(b"manifest")
    preserved = evidence / "provenance" / "build-manifest.json"
    preserved.write_bytes(b"safe manifest")
    executable = tmp_path / "tool"
    executable.write_bytes(b"tool")
    state = capture_git_state(repo, snapshot_dir=evidence / "provenance" / "source-snapshot")
    expected = {"path": str(executable), "resolved_path": str(executable), "sha256": hashlib.sha256(b"tool").hexdigest()}
    provenance = {
        "build_linkage": True, "repo_path": str(repo), "evidence_dir": str(evidence), "source": state,
        "build_manifest": {"path": str(manifest_path), "size": 8, "sha256": hashlib.sha256(b"manifest").hexdigest(),
                           "preserved_path": "provenance/build-manifest.json", "preserved_size": 13,
                           "preserved_sha256": hashlib.sha256(b"safe manifest").hexdigest()},
        "binaries": {"Role": expected},
    }
    source.write_text("after\n", encoding="utf-8")
    linked, reason = local_executable_linkage(provenance, "Role", str(executable), checked_path=str(executable))
    assert linked is False
    assert "source" in (reason or "") and "drift" in (reason or "")


def test_remote_manifest_writer_keeps_schema_v1(tmp_path: Path):
    (tmp_path / "payload.txt").write_text("payload", encoding="utf-8")
    path = write_evidence_manifest(tmp_path, {"mode": "remote", "trial_id": "r"})
    data = json.loads(path.read_text(encoding="utf-8"))
    assert data["schema_version"] == 1
    assert (tmp_path / "manifest.sha256").is_file()
    assert not (tmp_path / "terminal-seal.json").exists()
