import hashlib
import subprocess
import shutil
from pathlib import Path

import pytest

import local_provenance
from local_provenance import (
    _git_argv,
    _tree_sha256,
    capture_code_identity,
    capture_git_state,
    file_identity,
    snapshot_selected_files,
    sanitize_config,
    validate_build_linkage,
)


def test_git_commands_mark_repository_safe_for_root_execution(tmp_path):
    assert _git_argv(tmp_path, "status", "--porcelain") == [
        "git", "-c", f"safe.directory={tmp_path.resolve()}", "-C", str(tmp_path.resolve()), "status", "--porcelain"
    ]



def test_capture_git_state_excludes_evidence_root_from_source_status(tmp_path):
    subprocess.run(["git", "init", "--quiet", str(tmp_path)], check=True)
    tracked = tmp_path / "tracked.txt"
    tracked.write_text("tracked\n", encoding="utf-8")
    subprocess.run(["git", "-C", str(tmp_path), "add", "tracked.txt"], check=True)
    subprocess.run([
        "git", "-C", str(tmp_path), "-c", "user.name=Test", "-c", "user.email=test@example.invalid",
        "commit", "--quiet", "-m", "initial"
    ], check=True)
    evidence = tmp_path / "evidence"
    (evidence / "run").mkdir(parents=True)
    (evidence / "run" / "result.json").write_text("{}\n", encoding="utf-8")
    state = capture_git_state(tmp_path, excluded_root=evidence, snapshot_dir=tmp_path / "snapshot")
    assert state["clean"] is True
    assert state["status"] == []
    assert state["tree_sha256"] == _tree_sha256(tmp_path, excluded_root=evidence)


def test_snapshot_selected_files_is_content_addressed(tmp_path):
    source = tmp_path / "src" / "main.cpp"
    source.parent.mkdir()
    source.write_bytes(b"int main() {}\n")

    snapshot = snapshot_selected_files([source], root=tmp_path)

    assert snapshot == [
        {
            "path": "src/main.cpp",
            "size": source.stat().st_size,
            "sha256": "bc8bb8e433bf65214540115414c821c904b2a30d60a3ac0424bf9b77a00024b7",
        }
    ]


def test_capture_git_state_contains_source_and_index_identity(tmp_path):
    subprocess.run(["git", "init", "--quiet", str(tmp_path)], check=True)
    tracked = tmp_path / "tracked.txt"
    tracked.write_text("tracked\n", encoding="utf-8")
    subprocess.run(["git", "-C", str(tmp_path), "add", "tracked.txt"], check=True)
    subprocess.run(
        ["git", "-C", str(tmp_path), "-c", "user.name=Test", "-c", "user.email=test@example.invalid", "commit", "--quiet", "-m", "initial"],
        check=True,
    )
    (tmp_path / "untracked.txt").write_text("untracked\n", encoding="utf-8")

    state = capture_git_state(tmp_path)

    assert {"head", "status", "clean", "index", "index_sha256", "index_entries"} <= set(state)
    assert len(state["head"]) == 40
    assert state["status"] == ["?? untracked.txt"]
    assert state["clean"] is False
    assert state["index"]["captured"] is True
    assert len(state["index_sha256"]) == 64
    assert state["index_entries"][0]["path"] == "tracked.txt"


def test_capture_code_identity_records_controller_and_helpers(tmp_path):
    controller = tmp_path / "controller.py"
    helper = tmp_path / "helper.py"
    controller.write_bytes(b"controller")
    helper.write_bytes(b"helper")

    identity = capture_code_identity(controller, {"helper": helper})

    assert identity["controller"] == file_identity(controller)
    assert identity["helpers"]["helper"] == file_identity(helper)


def test_sanitize_config_redacts_secret_values_without_redacting_paths():
    result = sanitize_config({"NG_LOCAL_IO_PATH": "/var/ng/io", "API_TOKEN": "do-not-publish"})

    assert result == {"API_TOKEN": "<redacted>", "NG_LOCAL_IO_PATH": "/var/ng/io"}


def test_empty_manifest_is_rejected():
    with pytest.raises(ValueError, match="manifest"):
        validate_build_linkage({}, "abc123", {})


def valid_manifest():
    compiler = str(Path(shutil.which("g++") or shutil.which("c++")).resolve())
    version = subprocess.run([compiler, "--version"], capture_output=True, text=True, check=True).stdout.splitlines()[0]
    binary = Path("/bin/true")
    binary_hash = hashlib.sha256(binary.read_bytes()).hexdigest()
    ldd_run = subprocess.run(["ldd", str(binary)], capture_output=True, text=True, check=True)
    ldd_output = ldd_run.stdout + ldd_run.stderr
    stable_output = local_provenance._normalise_loader_output(ldd_output)
    ldd_hash = hashlib.sha256(stable_output.encode()).hexdigest()
    libraries = sorted(line.strip() for line in stable_output.splitlines() if line.strip())
    return {
        "recipe": {"command": ["cmake", "--build", "build"], "working_directory": "/src", "executed": True, "returncodes": [0, 0, 0]},
        "toolchain": {"compiler": compiler, "version": version, "version_sha256": hashlib.sha256(version.encode()).hexdigest(), "status": "ok"},
        "options": {"build_type": "RelWithDebInfo"},
        "runtime_library_identity": {"method": "ldd", "binaries": {"Source": {"status": "ok", "ldd_sha256": ldd_hash, "binary_sha256": binary_hash, "libraries": libraries}}},
        "source_head": "abc123",
        "source_snapshot": {"head": "abc123", "status": [], "tree_sha256": "e" * 64, "submodules": [], "submodules_complete": True,
                             "index": {"captured": True, "sha256": "a" * 64, "entries": [{"path": "tracked.txt", "mode": "100644", "blob_id": "b" * 40, "stage": 0}]},
                             "index_sha256": "a" * 64, "index_entries": [{"path": "tracked.txt", "mode": "100644", "blob_id": "b" * 40, "stage": 0}],
                             "content_snapshot": {"captured": True, "files": [], "errors": []}},
        "binaries": {
            "Source": {"path": "/bin/true", "sha256": binary_hash},
        },
    }


def test_valid_build_linkage_is_accepted():
    manifest = valid_manifest()
    executables = {
        "Source": {"path": "/bin/true", "sha256": manifest["binaries"]["Source"]["sha256"]},
    }

    assert validate_build_linkage(manifest, "abc123", executables, source_state=manifest["source_snapshot"], manifest_evidence={"path": "manifest.json", "size": 1, "sha256": "f" * 64, "preserved_path": "provenance/build-manifest.json", "preserved_size": 1, "preserved_sha256": "f" * 64}) is True


@pytest.mark.parametrize(
    ("field", "replacement", "message"),
    [
        ("recipe", {}, "recipe"),
        ("source_snapshot", [], "source snapshot"),
        ("binaries", {}, "binaries"),
    ],
)
def test_required_linkage_sections_are_rejected_when_empty(field, replacement, message):
    manifest = valid_manifest()
    manifest[field] = replacement

    with pytest.raises(ValueError, match=message):
        validate_build_linkage(manifest, "abc123", {"Source": {"path": "/bin/true", "sha256": "b" * 64}}, source_state=manifest["source_snapshot"], manifest_evidence={"path": "manifest.json", "size": 1, "sha256": "f" * 64, "preserved_path": "provenance/build-manifest.json", "preserved_size": 1, "preserved_sha256": "f" * 64})


def test_hash_divergence_is_rejected():
    manifest = valid_manifest()
    executables = {"Source": {"path": "/bin/true", "sha256": "c" * 64}}

    with pytest.raises(ValueError, match="hash"):
        validate_build_linkage(manifest, "abc123", executables, source_state=manifest["source_snapshot"], manifest_evidence={"path": "manifest.json", "size": 1, "sha256": "f" * 64, "preserved_path": "provenance/build-manifest.json", "preserved_size": 1, "preserved_sha256": "f" * 64})


def test_path_divergence_is_rejected():
    manifest = valid_manifest()
    executables = {"Source": {"path": "/other/bin/Source", "sha256": "b" * 64}}

    with pytest.raises(ValueError, match="path"):
        validate_build_linkage(manifest, "abc123", executables, source_state=manifest["source_snapshot"], manifest_evidence={"path": "manifest.json", "size": 1, "sha256": "f" * 64, "preserved_path": "provenance/build-manifest.json", "preserved_size": 1, "preserved_sha256": "f" * 64})
