import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from validate_spec061_contract import (
    POLICY,
    directory_observation,
    load,
    validate_directory_observation_contract,
)


CONTRACT = {
    "exclusions_by_role": {
        "SOURCE_ROOT": ["Docs"],
        "PAYLOAD_ROOT": [],
    },
    "generated_artifact_patterns_by_role": {
        "SOURCE_ROOT": ["**/__pycache__/**", "**/*.pyc", "**/*.pyo", "**/.pytest_cache/**"],
        "PAYLOAD_ROOT": [],
    },
}


def test_payload_root_enumeration_includes_docs_and_generated_names(tmp_path):
    root = tmp_path / "payload"
    (root / "Docs").mkdir(parents=True)
    (root / "Docs" / "payload.txt").write_text("payload", encoding="utf-8")
    (root / ".pytest_cache").mkdir()
    (root / ".pytest_cache" / "state").write_text("cache", encoding="utf-8")
    (root / "__pycache__").mkdir()
    (root / "__pycache__" / "module.pyc").write_bytes(b"pyc")
    (root / "root.pyc").write_bytes(b"pyc")
    (root / "keep.txt").write_text("keep", encoding="utf-8")

    _, count, _ = directory_observation(root, CONTRACT, "PAYLOAD_ROOT")

    assert count == 8


def test_source_root_excludes_docs_and_generated_artifacts_only_by_role(tmp_path):
    root = tmp_path / "source"
    (root / "Docs").mkdir(parents=True)
    (root / "Docs" / "source.txt").write_text("docs", encoding="utf-8")
    (root / ".pytest_cache").mkdir()
    (root / ".pytest_cache" / "state").write_text("cache", encoding="utf-8")
    (root / "__pycache__").mkdir()
    (root / "__pycache__" / "module.pyc").write_bytes(b"pyc")
    (root / "root.pyc").write_bytes(b"pyc")
    (root / "keep.txt").write_text("keep", encoding="utf-8")

    _, count, _ = directory_observation(root, CONTRACT, "SOURCE_ROOT")

    assert count == 1


def test_policy_directory_observation_uses_role_scoped_exclusions():
    contract = load(POLICY)["directory_observation_contract"]
    assert validate_directory_observation_contract(contract) == []
    assert "Docs" in contract["exclusions_by_role"]["SOURCE_ROOT"]
    assert contract["exclusions_by_role"]["PAYLOAD_ROOT"] == []
    assert contract["generated_artifact_patterns_by_role"]["PAYLOAD_ROOT"] == []


def test_directory_observation_contract_forbids_payload_exclusions():
    assert validate_directory_observation_contract(CONTRACT) == []

    unsafe = {
        **CONTRACT,
        "exclusions_by_role": {
            "SOURCE_ROOT": ["Docs"],
            "PAYLOAD_ROOT": ["Docs"],
        },
    }
    assert any("PAYLOAD_ROOT" in error for error in validate_directory_observation_contract(unsafe))


def test_directory_observation_contract_rejects_non_object():
    assert validate_directory_observation_contract(None) == ["directory observation contract must be an object"]


def test_directory_observation_rejects_unknown_role(tmp_path):
    with pytest.raises(ValueError, match="directory role"):
        directory_observation(tmp_path, CONTRACT, "UNDECLARED_ROOT")
