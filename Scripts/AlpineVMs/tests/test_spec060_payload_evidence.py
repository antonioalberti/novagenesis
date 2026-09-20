"""SPEC-060 first controller-only RED slice."""
from __future__ import annotations

import sys
import errno
import hashlib
import json
from pathlib import Path
from types import SimpleNamespace

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import payload_evidence as payload_module
from payload_evidence import PayloadContractError, _copy_bounded_file, _open_absolute_directory, _write_json, compare_payload_maps, evaluate_payload_result, inventory_payload_origin, loads_rejecting_duplicate_keys, payload_acceptance_eligible, preserve_payload_origins, validate_payload_plan
import ng_remote_executor as executor


def _valid_normal_plan() -> dict[str, object]:
    zero = "0" * 64
    return {
        "diagnostic_only": False,
        "acceptance_profile": "normal-payload",
        "payload_evidence": {
            "schema_version": 1,
            "expected": {
                "count": 5,
                "pattern": "*.jpg",
                "hash": "sha256",
                "map": {
                    f"photo-{number:02d}.jpg": {"size": 4096, "sha256": zero}
                    for number in range(1, 6)
                },
            },
            "origins": {
                "source": {"host": "source-vm", "role": "source", "path": "/var/ng/source", "service_config_ref": "source.service"},
                "nrncs": {"host": "source-vm", "role": "nrncs", "path": "/var/ng/nrncs", "service_config_ref": "nrncs.service"},
                "repository": {"host": "repository-vm", "role": "repository", "path": "/var/ng/repository", "service_config_ref": "repository.service"},
            },
            "bounds": {
                "global_timeout_s": 300,
                "per_origin_timeout_s": 60,
                "max_files_per_origin": 100,
                "max_bytes_per_origin": 10485760,
                "max_bytes_total": 31457280,
            },
            "ownership": {
                "trial_id": "TRIAL-EXACT-SHA",
                "owner": "ng-elc-controller",
                "cleanup_scope": "declared-roots-only",
            },
            "nrncs_binding": {
                "role": "nrncs",
                "service_config_ref": "nrncs.service",
                "observation_ref": "subscription-readback",
            },
        },
    }


def test_closed_normal_payload_plan_is_accepted():
    normalized = validate_payload_plan(_valid_normal_plan())

    assert normalized["schema_version"] == 1
    assert normalized["acceptance_profile"] == "normal-payload"
    assert normalized["expected"]["count"] == 5
    assert set(normalized["origins"]) == {"source", "nrncs", "repository"}
    assert len(normalized["expected"]["map"]) == 5


def _remote_marker_plan() -> dict[str, object]:
    return {
        "schema_version": 1,
        "roles": [
            {
                "name": "PGCS",
                "vm": "source",
                "command": ["/bin/true"],
                "readiness": [{"id": "ready", "pattern": "READY"}],
            }
        ],
        "timeouts": {"readiness": 1, "observation": 0, "total": 1},
        "runtime_oracle": {
            "type": "markers",
            "required": [{"role": "PGCS", "id": "ready", "pattern": "READY"}],
        },
    }


def test_normal_remote_plan_without_payload_contract_is_rejected_before_launch():
    plan = _remote_marker_plan()
    plan.update({"diagnostic_only": False, "acceptance_profile": "normal-payload"})

    with pytest.raises((PayloadContractError, ValueError), match="payload_evidence"):
        executor.validate_plan(plan)


def test_legacy_marker_only_plan_remains_valid_but_is_not_payload_acceptance():
    plan = _remote_marker_plan()

    executor.validate_plan(plan)


def test_explicit_non_diagnostic_plan_without_profile_is_rejected():
    plan = _remote_marker_plan()
    plan["diagnostic_only"] = False

    with pytest.raises(ValueError, match="payload acceptance profile"):
        executor.validate_plan(plan)


def test_duplicate_json_keys_are_rejected_before_payload_validation():
    with pytest.raises(PayloadContractError, match="duplicate JSON key"):
        loads_rejecting_duplicate_keys('{"payload_evidence":{"schema_version":1,"schema_version":1}}')


def test_payload_contract_rejects_bool_float_and_non_normalized_paths():
    plan = _valid_normal_plan()
    plan["payload_evidence"]["schema_version"] = True
    with pytest.raises(PayloadContractError, match="schema_version"):
        validate_payload_plan(plan)

    plan = _valid_normal_plan()
    plan["payload_evidence"]["expected"]["count"] = 5.0
    with pytest.raises(PayloadContractError, match="count"):
        validate_payload_plan(plan)

    plan = _valid_normal_plan()
    plan["payload_evidence"]["origins"]["source"]["path"] = "/var/ng/./source"
    with pytest.raises(PayloadContractError, match="absolute, normalized"):
        validate_payload_plan(plan)


def test_legacy_marker_plan_is_explicitly_not_payload_acceptance_eligible():
    assert payload_acceptance_eligible(_remote_marker_plan()) is False
    assert payload_acceptance_eligible(_valid_normal_plan()) is True


def test_identical_payload_maps_are_complete_and_accepted():
    expected = _valid_normal_plan()["payload_evidence"]["expected"]["map"]

    result = compare_payload_maps(expected, dict(expected))

    assert result == {
        "status": "PASS",
        "missing": [],
        "extra": [],
        "mismatched": [],
        "accepted": True,
    }


def test_compare_payload_maps_rejects_empty_expected_map():
    with pytest.raises(PayloadContractError, match="expected payload map"):
        compare_payload_maps({}, {})


def test_compare_payload_maps_rejects_malformed_observed_record():
    expected = _valid_normal_plan()["payload_evidence"]["expected"]["map"]
    observed = dict(expected)
    observed["photo-01.jpg"] = {"size": 0, "sha256": "not-a-digest"}

    with pytest.raises(PayloadContractError, match="payload map"):
        compare_payload_maps(expected, observed)


def test_payload_origin_inventory_hashes_a_bounded_jpeg_root(tmp_path: Path):
    root = tmp_path / "source"
    root.mkdir()
    expected: dict[str, dict[str, object]] = {}
    for number in range(1, 6):
        data = f"synthetic-jpeg-{number}".encode("ascii")
        name = f"photo-{number:02d}.jpg"
        (root / name).write_bytes(data)
        expected[name] = {"size": len(data), "sha256": hashlib.sha256(data).hexdigest()}

    result = inventory_payload_origin(root, max_files=10, max_bytes=1024)

    assert result["status"] == "COMPLETE"
    assert result["map"] == expected
    assert result["identity"]["st_dev"] == root.stat().st_dev
    assert result["identity"]["st_ino"] == root.stat().st_ino


def test_payload_origin_inventory_honors_explicit_deadline(tmp_path: Path):
    root = tmp_path / "source"
    root.mkdir()
    (root / "photo-01.jpg").write_bytes(b"bytes")

    result = inventory_payload_origin(root, max_files=10, max_bytes=1024, timeout_s=0)

    assert result["status"] == "INCOMPLETE"
    assert "deadline-expired" in result["errors"]


def test_empty_origin_honors_expired_deadline_before_enumeration(tmp_path: Path):
    root = tmp_path / "empty"
    root.mkdir()

    result = inventory_payload_origin(root, max_files=10, max_bytes=1024, timeout_s=0)

    assert result["status"] == "INCOMPLETE"
    assert "deadline-expired" in result["errors"]


def test_zero_length_observed_file_is_incomplete_not_unexpected_exception(tmp_path: Path):
    root = tmp_path / "source"
    root.mkdir()
    (root / "photo-01.jpg").write_bytes(b"")

    result = inventory_payload_origin(root, max_files=10, max_bytes=1024)

    assert result["status"] == "INCOMPLETE"
    assert result["map"] == {}
    assert "invalid-observed-size:photo-01.jpg" in result["errors"]


def test_inventory_rejects_symlinked_source_ancestor(tmp_path: Path):
    real_parent = tmp_path / "real-parent"
    real_root = real_parent / "source"
    real_root.mkdir(parents=True)
    (real_root / "photo-01.jpg").write_bytes(b"payload")
    linked_parent = tmp_path / "linked-parent"
    linked_parent.symlink_to(real_parent, target_is_directory=True)

    result = inventory_payload_origin(linked_parent / "source", max_files=10, max_bytes=1024)

    assert result["status"] == "INCOMPLETE"
    assert result["map"] == {}
    assert any("root-unavailable" in error for error in result["errors"])


def test_open_absolute_directory_accepts_root_directory():
    fd = _open_absolute_directory(Path("/"), create=False, deadline=None)
    try:
        assert fd >= 0
    finally:
        import os

        os.close(fd)


def test_open_absolute_directory_rechecks_deadline_after_root_open(monkeypatch: pytest.MonkeyPatch):
    clock = [0.0]
    opened: list[int] = []
    real_open = payload_module.os.open

    def advancing_open(path: str, flags: int, *args: object, **kwargs: object) -> int:
        fd = real_open(path, flags, *args, **kwargs)
        opened.append(fd)
        if path == "/":
            clock[0] = 2.0
        return fd

    monkeypatch.setattr(payload_module.time, "monotonic", lambda: clock[0])
    monkeypatch.setattr(payload_module.os, "open", advancing_open)

    try:
        with pytest.raises(TimeoutError, match="deadline-expired"):
            _open_absolute_directory(Path("/"), create=False, deadline=1.0)
    finally:
        for fd in opened:
            try:
                payload_module.os.close(fd)
            except OSError:
                pass


@pytest.mark.parametrize("create", [False, True])
def test_open_absolute_directory_closes_child_on_post_open_deadline(monkeypatch: pytest.MonkeyPatch, tmp_path: Path, create: bool):
    target = tmp_path / ("existing" if not create else "created")
    if not create:
        target.mkdir()
    clock = [0.0]
    child_fd: list[int | None] = [None]
    parent_fd: list[int | None] = [None]
    real_open = payload_module.os.open
    real_close = payload_module.os.close

    def advancing_open(path: str, flags: int, *args: object, **kwargs: object) -> int:
        fd = real_open(path, flags, *args, **kwargs)
        dir_fd = kwargs.get("dir_fd")
        if isinstance(dir_fd, int) and path in {"existing", "created"}:
            child_fd[0] = fd
            parent_fd[0] = dir_fd
            clock[0] = 2.0
        return fd

    monkeypatch.setattr(payload_module.time, "monotonic", lambda: clock[0])
    monkeypatch.setattr(payload_module.os, "open", advancing_open)

    with pytest.raises(TimeoutError, match="deadline-expired"):
        _open_absolute_directory(target, create=create, deadline=1.0)

    assert child_fd[0] is not None
    assert parent_fd[0] is not None
    for fd in (child_fd[0], parent_fd[0]):
        try:
            payload_module.os.fstat(fd)
        except OSError as exc:
            assert exc.errno == errno.EBADF
        else:
            real_close(fd)
            pytest.fail(f"descriptor {fd} remained open")


def test_partial_read_error_reports_consumed_bytes(tmp_path: Path, monkeypatch: pytest.MonkeyPatch):
    source = tmp_path / "photo-01.jpg"
    source.write_bytes(b"abcdef")
    real_read = payload_module.os.read
    calls = 0

    def flaky_read(fd: int, size: int) -> bytes:
        nonlocal calls
        calls += 1
        if calls == 1:
            return real_read(fd, min(size, 3))
        raise OSError("synthetic read failure")

    monkeypatch.setattr(payload_module.os, "read", flaky_read)

    data, error, consumed = payload_module._read_bounded_file(source, budget=1024, deadline=None)

    assert data == b"abc"
    assert error == "read-failed:OSError"
    assert consumed == 3


def test_inventory_marks_root_mutation_during_collection_incomplete(tmp_path: Path, monkeypatch: pytest.MonkeyPatch):
    root = tmp_path / "source"
    root.mkdir()
    (root / "photo-01.jpg").write_bytes(b"payload")
    real_fstat = payload_module.os.fstat
    root_calls = 0

    def mutating_fstat(fd: int):
        nonlocal root_calls
        result = real_fstat(fd)
        if payload_module.stat.S_ISDIR(result.st_mode):
            root_calls += 1
            if root_calls == 2:
                return SimpleNamespace(
                    st_dev=result.st_dev,
                    st_ino=result.st_ino,
                    st_mode=result.st_mode,
                    st_mtime_ns=result.st_mtime_ns + 1,
                    st_ctime_ns=result.st_ctime_ns,
                )
        return result

    monkeypatch.setattr(payload_module.os, "fstat", mutating_fstat)

    result = inventory_payload_origin(root, max_files=10, max_bytes=1024)

    assert result["status"] == "INCOMPLETE"
    assert "root-contents-changed" in result["errors"]


def test_json_write_deadline_after_fsync_is_not_success(tmp_path: Path, monkeypatch: pytest.MonkeyPatch):
    clock = [0.0]
    real_fsync = payload_module.os.fsync

    def expiring_fsync(fd: int) -> None:
        real_fsync(fd)
        clock[0] = 2.0

    monkeypatch.setattr(payload_module.time, "monotonic", lambda: clock[0])
    monkeypatch.setattr(payload_module.os, "fsync", expiring_fsync)
    destination = tmp_path / "bundle" / "payload.json"

    with pytest.raises(TimeoutError, match="deadline-expired"):
        _write_json(destination, {"safe": True}, deadline=1.0)

    assert destination.exists() is False


def test_json_write_closes_fd_if_deadline_expires_after_open(tmp_path: Path, monkeypatch: pytest.MonkeyPatch):
    clock = [0.0]
    target_fd: list[int | None] = [None]
    real_open = payload_module.os.open
    real_close = payload_module.os.close

    def advancing_open(path: str, flags: int, *args: object, **kwargs: object) -> int:
        fd = real_open(path, flags, *args, **kwargs)
        if flags & payload_module.os.O_WRONLY:
            target_fd[0] = fd
            clock[0] = 2.0
        return fd

    monkeypatch.setattr(payload_module.time, "monotonic", lambda: clock[0])
    monkeypatch.setattr(payload_module.os, "open", advancing_open)

    with pytest.raises(TimeoutError, match="deadline-expired"):
        _write_json(tmp_path / "bundle" / "payload.json", {"safe": True}, deadline=1.0)

    assert target_fd[0] is not None
    try:
        payload_module.os.fstat(target_fd[0])
    except OSError as exc:
        assert exc.errno == errno.EBADF
    else:
        real_close(target_fd[0])
        pytest.fail(f"descriptor {target_fd[0]} remained open")


def test_copy_closes_fd_if_deadline_expires_after_destination_open(tmp_path: Path, monkeypatch: pytest.MonkeyPatch):
    source = tmp_path / "source.jpg"
    destination = tmp_path / "bundle" / "destination.jpg"
    source.write_bytes(b"payload")
    clock = [0.0]
    target_fd: list[int | None] = [None]
    real_open = payload_module.os.open
    real_close = payload_module.os.close

    def advancing_open(path: str, flags: int, *args: object, **kwargs: object) -> int:
        fd = real_open(path, flags, *args, **kwargs)
        if flags & payload_module.os.O_WRONLY:
            target_fd[0] = fd
            clock[0] = 2.0
        return fd

    monkeypatch.setattr(payload_module.time, "monotonic", lambda: clock[0])
    monkeypatch.setattr(payload_module.os, "open", advancing_open)

    copied, error, written = _copy_bounded_file(source, destination, budget=1024, deadline=1.0)

    assert copied is False
    assert error == "deadline-expired"
    assert written == 0
    assert target_fd[0] is not None
    try:
        payload_module.os.fstat(target_fd[0])
    except OSError as exc:
        assert exc.errno == errno.EBADF
    else:
        real_close(target_fd[0])
        pytest.fail(f"descriptor {target_fd[0]} remained open")


@pytest.mark.parametrize(
    "relative",
    (
        "payload/expected.json",
        "payload/maps/source.json",
        "payload/metadata/source.json",
        "payload/preservation-checkpoint.json",
    ),
)
def test_json_preservation_refuses_final_symlink(tmp_path: Path, relative: str):
    outside = tmp_path / "outside.json"
    outside.write_text("sentinel\n", encoding="utf-8")
    target = tmp_path / "bundle" / relative
    target.parent.mkdir(parents=True)
    target.symlink_to(outside)

    with pytest.raises((PayloadContractError, OSError), match="symlink|stale|destination|unsafe|exists"):
        _write_json(target, {"safe": True})

    assert outside.read_text(encoding="utf-8") == "sentinel\n"


def test_json_preservation_refuses_symlinked_ancestor(tmp_path: Path):
    outside = tmp_path / "outside"
    outside.mkdir()
    bundle = tmp_path / "bundle"
    bundle.mkdir()
    (bundle / "payload").symlink_to(outside, target_is_directory=True)

    with pytest.raises((PayloadContractError, OSError), match="symlink|unsafe|exists"):
        _write_json(bundle / "payload" / "expected.json", {"safe": True})

    assert not (outside / "expected.json").exists()


def test_bounded_copy_refuses_to_write_past_byte_budget(tmp_path: Path):
    source = tmp_path / "source.jpg"
    destination = tmp_path / "destination.jpg"
    source.write_bytes(b"x" * 32)

    copied, error, written = _copy_bounded_file(source, destination, budget=8, deadline=None)

    assert copied is False
    assert error == "byte-bound-exceeded"
    assert written <= 8
    assert destination.exists() is False


def test_preservation_enforces_aggregate_origin_quota(tmp_path: Path):
    roots: dict[str, Path] = {}
    expected: dict[str, dict[str, object]] = {}
    for origin in ("source", "nrncs", "repository"):
        root = tmp_path / f"{origin}-root"
        root.mkdir()
        roots[origin] = root
        for number in range(1, 6):
            (root / f"photo-{number:02d}.jpg").write_bytes(b"data")
        if origin == "source":
            expected = inventory_payload_origin(root, max_files=10, max_bytes=1024)["map"]

    result = preserve_payload_origins(
        tmp_path / "bundle",
        roots,
        expected=expected,
        max_files=10,
        max_bytes=1024,
        max_bytes_total=59,
    )

    assert result["status"] == "INCOMPLETE"
    assert result["accepted"] is False
    assert any("aggregate-byte-bound-exceeded" in error for error in result["origins"]["repository"]["errors"])


def test_preservation_does_not_read_after_aggregate_quota_is_exhausted(tmp_path: Path):
    roots: dict[str, Path] = {}
    expected = {"photo-01.jpg": {"size": 1, "sha256": hashlib.sha256(b"x").hexdigest()}}
    for origin in ("source", "nrncs", "repository"):
        root = tmp_path / f"{origin}-root"
        root.mkdir()
        roots[origin] = root
        (root / "photo-01.jpg").write_bytes(b"x")

    result = preserve_payload_origins(
        tmp_path / "bundle",
        roots,
        expected=expected,
        max_files=10,
        max_bytes=1024,
        max_bytes_total=1,
    )

    assert result["aggregate_bytes"] == 1
    assert result["origins"]["source"]["source"]["bytes"] == 1
    assert result["origins"]["nrncs"]["source"]["bytes"] == 0
    assert result["origins"]["repository"]["source"]["bytes"] == 0
    assert result["accepted"] is False


def test_remote_marker_result_never_grants_normal_payload_acceptance():
    code, eligibility = executor.evaluate_remote_acceptance(
        _remote_marker_plan(),
        runtime="PASS",
        teardown="PASS",
        evidence="COMPLETE",
    )

    assert code == 0
    assert eligibility["eligible"] is False
    assert "marker-only" in eligibility["blockers"][0]


def _verified_payload_result() -> dict[str, object]:
    return {
        "status": "COMPLETE",
        "accepted": True,
        "offline_verification": {
            "code": "NGELC-VALID",
            "integrity": True,
            "accepted": True,
            "schema_version": 2,
        },
    }


def test_normal_payload_acceptance_requires_complete_payload_result():
    plan = _valid_normal_plan()

    code, eligibility = executor.evaluate_remote_acceptance(
        plan,
        runtime="PASS",
        teardown="PASS",
        evidence="COMPLETE",
    )

    assert code != 0
    assert eligibility["eligible"] is False
    assert "not collected" in eligibility["blockers"][0]

    code, eligibility = executor.evaluate_remote_acceptance(
        plan,
        runtime="PASS",
        teardown="PASS",
        evidence="COMPLETE",
        payload_result={"status": "COMPLETE", "accepted": True},
    )

    assert code == 21
    assert eligibility["eligible"] is False
    assert eligibility["blockers"] == ["payload evidence offline verification is missing or invalid"]

    code, eligibility = executor.evaluate_remote_acceptance(
        plan,
        runtime="PASS",
        teardown="PASS",
        evidence="COMPLETE",
        payload_result=_verified_payload_result(),
    )

    assert code == 0
    assert eligibility == {"eligible": True, "blockers": []}


@pytest.mark.parametrize(
    ("payload_result", "expected_blocker"),
    (
        (None, "payload evidence not collected"),
        ([], "payload evidence attestation is invalid"),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": []},
            "payload evidence offline verification is missing or invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": True, "accepted": True}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"integrity": True, "accepted": True, "schema_version": 2}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-TAMPERED", "integrity": True, "accepted": True, "schema_version": 2}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": 1, "accepted": True, "schema_version": 2}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": False, "accepted": True, "schema_version": 2}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": True, "accepted": 1, "schema_version": 2}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": True, "accepted": False, "schema_version": 2}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": True, "accepted": True, "schema_version": "2"}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": True, "accepted": True, "schema_version": True}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": True, "accepted": True, "schema_version": 2.0}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": True, "offline_verification": {"code": "NGELC-VALID", "integrity": True, "accepted": True, "schema_version": 3}},
            "payload evidence offline verification is invalid",
        ),
        (
            {"status": "COMPLETE", "accepted": 1, "offline_verification": _verified_payload_result()["offline_verification"]},
            "payload evidence is incomplete or not accepted",
        ),
        (
            {"status": "INCOMPLETE", "accepted": False, "offline_verification": _verified_payload_result()["offline_verification"]},
            "payload evidence is incomplete or not accepted",
        ),
    ),
)
def test_normal_payload_rejects_invalid_offline_attestation(payload_result: object, expected_blocker: str):
    code, eligibility = executor.evaluate_remote_acceptance(
        _valid_normal_plan(),
        runtime="PASS",
        teardown="PASS",
        evidence="COMPLETE",
        payload_result=payload_result,
    )

    assert code == 21
    assert eligibility == {"eligible": False, "blockers": [expected_blocker]}


def test_invalid_attestation_does_not_mask_nonzero_runtime_code():
    code, eligibility = executor.evaluate_remote_acceptance(
        _valid_normal_plan(),
        runtime="FAIL",
        teardown="PASS",
        evidence="COMPLETE",
        payload_result={"status": "COMPLETE", "accepted": True},
    )

    assert code == 10
    assert eligibility == {
        "eligible": False,
        "blockers": ["payload evidence offline verification is missing or invalid"],
    }


def test_valid_attestation_does_not_mask_nonzero_runtime_code():
    code, eligibility = executor.evaluate_remote_acceptance(
        _valid_normal_plan(),
        runtime="FAIL",
        teardown="PASS",
        evidence="COMPLETE",
        payload_result=_verified_payload_result(),
    )

    assert code == 10
    assert eligibility == {
        "eligible": False,
        "blockers": ["runtime, teardown, or evidence is not acceptance-eligible"],
    }


@pytest.mark.parametrize(
    ("runtime", "teardown", "evidence", "expected_code"),
    (
        ("PASS", "FAIL", "COMPLETE", 20),
        ("PASS", "PASS", "INCOMPLETE", 20),
        ("INCONCLUSIVE", "PASS", "COMPLETE", 11),
    ),
)
def test_valid_attestation_cannot_override_runtime_teardown_or_evidence(
    runtime: str,
    teardown: str,
    evidence: str,
    expected_code: int,
):
    code, eligibility = executor.evaluate_remote_acceptance(
        _valid_normal_plan(),
        runtime=runtime,
        teardown=teardown,
        evidence=evidence,
        payload_result=_verified_payload_result(),
    )

    assert code == expected_code
    assert eligibility == {
        "eligible": False,
        "blockers": ["runtime, teardown, or evidence is not acceptance-eligible"],
    }


def test_three_origins_are_preserved_in_canonical_namespaces_and_checkpointed(tmp_path: Path):
    roots: dict[str, Path] = {}
    expected: dict[str, dict[str, object]] = {}
    for origin in ("source", "nrncs", "repository"):
        root = tmp_path / f"{origin}-root"
        root.mkdir()
        roots[origin] = root
        for number in range(1, 6):
            data = f"synthetic-jpeg-{number}".encode("ascii")
            (root / f"photo-{number:02d}.jpg").write_bytes(data)
        expected = inventory_payload_origin(root, max_files=10, max_bytes=4096)["map"]

    bundle = tmp_path / "bundle"
    result = preserve_payload_origins(bundle, roots, expected=expected, max_files=10, max_bytes=4096)

    assert result["status"] == "COMPLETE"
    assert result["accepted"] is True
    assert (bundle / "payload" / "preservation-checkpoint.json").is_file()
    for origin in roots:
        assert (bundle / "payload" / "origins" / origin / "files" / "photo-01.jpg").is_file()
        assert (bundle / "payload" / "maps" / f"{origin}.json").is_file()
        assert (bundle / "payload" / "metadata" / f"{origin}.json").is_file()


def test_complete_divergence_is_fail_complete_not_inconclusive(tmp_path: Path):
    roots: dict[str, Path] = {}
    expected: dict[str, dict[str, object]] = {}
    for origin in ("source", "nrncs", "repository"):
        root = tmp_path / f"{origin}-root"
        root.mkdir()
        roots[origin] = root
        for number in range(1, 6):
            data = f"synthetic-jpeg-{number}".encode("ascii")
            if origin == "repository" and number == 3:
                data = b"divergent-bytes"
            (root / f"photo-{number:02d}.jpg").write_bytes(data)
        if origin == "source":
            expected = inventory_payload_origin(root, max_files=10, max_bytes=4096)["map"]

    result = preserve_payload_origins(bundle := tmp_path / "bundle", roots, expected=expected, max_files=10, max_bytes=4096)
    evaluated = evaluate_payload_result(result["origins"])

    assert result["status"] == "COMPLETE"
    assert result["accepted"] is False
    assert result["origins"]["repository"]["status"] == "COMPLETE"
    assert evaluated == {"runtime": "FAIL", "evidence": "COMPLETE", "accepted": False}


def test_preservation_does_not_follow_pending_destination_symlink(tmp_path: Path):
    roots: dict[str, Path] = {}
    expected: dict[str, dict[str, object]] = {}
    for origin in ("source", "nrncs", "repository"):
        root = tmp_path / f"{origin}-root"
        root.mkdir()
        roots[origin] = root
        for number in range(1, 6):
            data = f"synthetic-jpeg-{number}".encode("ascii")
            (root / f"photo-{number:02d}.jpg").write_bytes(data)
        if origin == "source":
            expected = inventory_payload_origin(root, max_files=10, max_bytes=4096)["map"]

    bundle = tmp_path / "bundle"
    destination = bundle / "payload" / "origins" / "source" / "files"
    destination.mkdir(parents=True)
    outside = tmp_path / "outside"
    outside.mkdir()
    escape = outside / "escaped.jpg"
    (destination / "photo-01.jpg").symlink_to(escape)

    result = preserve_payload_origins(bundle, roots, expected=expected, max_files=10, max_bytes=4096)

    assert escape.exists() is False
    assert result["accepted"] is False


def test_incomplete_origin_is_inconclusive_and_never_accepted():
    origins = {
        "source": {"status": "COMPLETE", "comparison": {"accepted": True}},
        "nrncs": {"status": "INCOMPLETE", "comparison": {"accepted": False}},
        "repository": {"status": "COMPLETE", "comparison": {"accepted": True}},
    }

    result = evaluate_payload_result(origins)

    assert result == {
        "runtime": "INCONCLUSIVE",
        "evidence": "INCOMPLETE",
        "accepted": False,
    }


def _minimal_v2_bundle_without_payload(root: Path) -> None:
    json_files = {
        "result.json": {
            "schema_version": 2,
            "runtime_result": "PASS",
            "teardown_result": "PASS",
            "evidence_result": "COMPLETE",
            "local_acceptance_eligible": True,
            "acceptance_blockers": [],
            "protected_input_blockers": [],
            "exit_code": 0,
        },
        "provenance.json": {"schema_version": 2, "capture_complete": True},
        "workload.json": {"schema_version": 2, "verified": True},
        "oracle.json": {"schema_version": 2, "preservation_verified": True},
        "ownership.json": {"schema_version": 2, "anchor": {"verified": True}},
        "cleanup.json": {"schema_version": 2, "result": "PASS"},
        "preservation.json": {"schema_version": 2, "verified": True},
        "plan/effective.json": {"schema_version": 1},
        "inventory/final.json": {"processes": [], "ipc": {}},
        "roles/none/exit.json": {"exit_code": 0},
    }
    for relative, value in json_files.items():
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(value) + "\n", encoding="utf-8")
    for relative in ("artifacts/source/_not-captured.json", "artifacts/repository/_not-captured.json"):
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("{}\n", encoding="utf-8")
    (root / "controller-events.jsonl").write_text("{\"event\":\"test\"}\n", encoding="utf-8")


def test_payload_contract_manifest_requires_canonical_payload_areas(tmp_path: Path):
    _minimal_v2_bundle_without_payload(tmp_path)

    with pytest.raises(OSError, match="payload"):
        executor.write_evidence_manifest(
            tmp_path,
            {
                "mode": "remote",
                "trial_id": "TRIAL-EXACT-SHA",
                "acceptance_profile": "normal-payload",
                "payload_evidence_required": True,
            },
            schema_version=2,
        )

    assert not (tmp_path / "terminal-seal.json").exists()


def test_verifier_rejects_flagged_payload_bundle_without_payload_areas(tmp_path: Path):
    _minimal_v2_bundle_without_payload(tmp_path)
    manifest_path = executor.write_evidence_manifest(
        tmp_path,
        {"mode": "local", "trial_id": "TRIAL-EXACT-SHA"},
        schema_version=2,
    )
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    manifest["payload_evidence_required"] = True
    manifest_bytes = (json.dumps(manifest, ensure_ascii=False, indent=2) + "\n").encode("utf-8")
    manifest_path.write_bytes(manifest_bytes)
    seal = json.loads((tmp_path / "terminal-seal.json").read_text(encoding="utf-8"))
    seal["manifest_sha256"] = hashlib.sha256(manifest_bytes).hexdigest()
    (tmp_path / "terminal-seal.json").write_text(json.dumps(seal, indent=2) + "\n", encoding="utf-8")

    report = executor.verify_bundle(tmp_path)

    assert report["integrity"] is False
    assert any("payload" in error.lower() for error in report["errors"])
