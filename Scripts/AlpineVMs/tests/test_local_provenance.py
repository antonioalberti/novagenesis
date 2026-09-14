import subprocess
from pathlib import Path

import pytest

from local_provenance import (
    capture_code_identity,
    capture_git_state,
    file_identity,
    snapshot_selected_files,
    sanitize_config,
    validate_build_linkage,
)


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


def test_capture_git_state_contains_only_head_and_status(tmp_path):
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

    assert set(state) == {"head", "status", "clean"}
    assert len(state["head"]) == 40
    assert state["status"] == ["?? untracked.txt"]
    assert state["clean"] is False


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
    return {
        "recipe": {"command": ["cmake", "--build", "build"], "options": ["RelWithDebInfo"]},
        "source_head": "abc123",
        "source_snapshot": [
            {"path": "src/main.cpp", "size": 12, "sha256": "a" * 64},
        ],
        "binaries": {
            "Source": {"path": "/build/bin/Source", "sha256": "b" * 64},
        },
    }


def test_valid_build_linkage_is_accepted():
    manifest = valid_manifest()
    executables = {
        "Source": {"path": "/build/bin/Source", "sha256": "b" * 64},
    }

    assert validate_build_linkage(manifest, "abc123", executables) is True


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
        validate_build_linkage(manifest, "abc123", {"Source": {"path": "/build/bin/Source", "sha256": "b" * 64}})


def test_hash_divergence_is_rejected():
    manifest = valid_manifest()
    executables = {"Source": {"path": "/build/bin/Source", "sha256": "c" * 64}}

    with pytest.raises(ValueError, match="hash"):
        validate_build_linkage(manifest, "abc123", executables)


def test_path_divergence_is_rejected():
    manifest = valid_manifest()
    executables = {"Source": {"path": "/other/bin/Source", "sha256": "b" * 64}}

    with pytest.raises(ValueError, match="path"):
        validate_build_linkage(manifest, "abc123", executables)
