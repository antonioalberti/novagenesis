"""SPEC-056 §12 static/dynamic runtime identity regressions.

The fixtures use the host compiler and the real ``file``, ``readelf`` and
``ldd`` commands.  They do not launch VMs or mock subprocess observations.
"""

from __future__ import annotations

import hashlib
import shutil
import subprocess
from pathlib import Path

import pytest

import local_provenance
from ng_observability import runtime_library_identity


def _sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _compile_fixture(tmp_path: Path, *, static: bool) -> Path:
    compiler = shutil.which("cc") or shutil.which("gcc")
    if compiler is None:
        pytest.skip("a C compiler is required for the ELF fixture")
    source = tmp_path / ("static.c" if static else "dynamic.c")
    binary = tmp_path / ("static-fixture" if static else "dynamic-fixture")
    source.write_text("int main(void) { return 0; }\n", encoding="utf-8")
    command = [compiler, str(source), "-O0", "-o", str(binary)]
    if static:
        command.insert(1, "-static")
    result = subprocess.run(command, capture_output=True, text=True, check=False)
    if result.returncode != 0:
        pytest.skip(f"host compiler cannot produce the requested ELF fixture: {result.stderr}")
    return binary


def test_static_elf_identity_is_truthful_and_accepted(tmp_path: Path):
    binary = _compile_fixture(tmp_path, static=True)
    identity = runtime_library_identity({"Static": {"path": str(binary), "sha256": _sha256(binary)}})
    record = identity["binaries"]["Static"]

    assert identity["method"] == "static"
    assert record["status"] == "ok"
    assert record["linkage"] == "static"
    assert record["binary_path"] == str(binary.resolve())
    assert record["binary_sha256"] == _sha256(binary)
    assert record["libraries"] == []
    assert record["library_count"] == 0
    assert record["ldd_returncode"] != 0
    assert "not a dynamic executable" in record["ldd_output"].lower()
    assert record["ldd_sha256"] == hashlib.sha256(record["ldd_output"].encode()).hexdigest()
    assert "ELF" in record["file_output"]
    assert record["file_output_sha256"] == hashlib.sha256(record["file_output"].encode()).hexdigest()
    assert record["elf_header_sha256"]

    local_provenance._validate_runtime_record(
        record,
        "runtime-library.Static",
        expected_binary_hash=_sha256(binary),
        expected_binary_path=str(binary),
    )


def test_dynamic_ldd_identity_remains_accepted(tmp_path: Path):
    binary = _compile_fixture(tmp_path, static=False)
    identity = runtime_library_identity({"Dynamic": {"path": str(binary), "sha256": _sha256(binary)}})
    record = identity["binaries"]["Dynamic"]

    assert identity["method"] == "ldd"
    assert record["status"] == "ok"
    assert record["libraries"]
    local_provenance._validate_runtime_record(
        record,
        "runtime-library.Dynamic",
        expected_binary_hash=_sha256(binary),
        expected_binary_path=str(binary),
    )


def test_static_identity_rejects_launched_path_drift(tmp_path: Path):
    binary = _compile_fixture(tmp_path, static=True)
    other_binary = tmp_path / "other-static-fixture"
    shutil.copy2(binary, other_binary)
    identity = runtime_library_identity({"Static": {"path": str(binary), "sha256": _sha256(binary)}})
    record = identity["binaries"]["Static"]

    with pytest.raises(ValueError, match="path"):
        local_provenance._validate_runtime_record(
            record,
            "runtime-library.Static",
            expected_binary_hash=_sha256(binary),
            expected_binary_path=str(other_binary),
        )


def test_static_identity_rejects_binary_hash_drift(tmp_path: Path):
    binary = _compile_fixture(tmp_path, static=True)
    identity = runtime_library_identity({"Static": {"path": str(binary), "sha256": _sha256(binary)}})
    record = dict(identity["binaries"]["Static"])
    record["binary_sha256"] = "0" * 64

    with pytest.raises(ValueError, match="binary hash"):
        local_provenance._validate_runtime_record(
            record,
            "runtime-library.Static",
            expected_binary_hash=_sha256(binary),
            expected_binary_path=str(binary),
        )


def test_dynamic_identity_rejects_ldd_hash_drift(tmp_path: Path):
    binary = _compile_fixture(tmp_path, static=False)
    identity = runtime_library_identity({"Dynamic": {"path": str(binary), "sha256": _sha256(binary)}})
    record = dict(identity["binaries"]["Dynamic"])
    record["output_sha256"] = "0" * 64

    with pytest.raises(ValueError, match="ldd output hash"):
        local_provenance._validate_runtime_record(
            record,
            "runtime-library.Dynamic",
            expected_binary_hash=_sha256(binary),
            expected_binary_path=str(binary),
        )
