from __future__ import annotations

import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path

import pytest

ALPINEVM_DIR = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ALPINEVM_DIR))

from ng_elc_contentapp_runner import (  # noqa: E402
    PlanError,
    RunBlocked,
    SnapshotError,
    run_profile,
    validate_plan,
    verify_exact_snapshot,
    validate_spec027_record,
    write_spec027_record_once,
)
import ng_elc_contentapp_runner as runner_module  # noqa: E402


def valid_plan() -> dict:
    return {
        "profile": "contentapp-source-to-repository-1000-photos",
        "run_id": "ng064-20260928-r01",
        "participants": {"controller": 100, "source": 102, "repository": 101},
        "source_head": "1af604dad66148618cc0e578c28adb4a9829e607",
        "spec027_text_blob": "546e4131064beaa82992a78749be67c68ceb6cf6",
        "spec027_text_sha256": "c481af00535c189dd1b24cce71096ab4a7e13f34e771175df251290713d17ad5",
        "photo_count": 1000,
        "contentapp_deadline_s": 600,
        "graceful_shutdown_s_per_guest": 120,
        "deadline_origin": "repository_contentapp_launch",
        "rate_window_start": "immediately_before_source_contentapp_launch",
        "completion_oracle": "stable_repository_snapshot",
        "max_total_bytes": 100_000_000,
        "workload_manifest_sha256": "0" * 64,
        "evidence_root": "/tmp/ng064-evidence",
        "vm_power_actions": [],
    }


def manifest_entry(path: str, data: bytes) -> dict:
    return {"path": path, "size": len(data), "sha256": hashlib.sha256(data).hexdigest()}


def test_accepts_only_the_owner_fixed_profile_and_timing_contract():
    validate_plan(valid_plan())

    plan = valid_plan()
    plan["contentapp_deadline_s"] = 601
    with pytest.raises(PlanError, match="600"):
        validate_plan(plan)


def test_rejects_wrong_vm_roles_and_any_power_operation():
    plan = valid_plan()
    plan["participants"]["source"] = 101
    with pytest.raises(PlanError, match="participants"):
        validate_plan(plan)

    plan = valid_plan()
    plan["vm_power_actions"] = ["qm stop 101"]
    with pytest.raises(PlanError, match="power"):
        validate_plan(plan)

    plan = valid_plan()
    del plan["vm_power_actions"]
    with pytest.raises(PlanError, match="power"):
        validate_plan(plan)


def test_exact_snapshot_matches_manifest_and_is_deterministic(tmp_path: Path):
    (tmp_path / "b.jpg").write_bytes(b"photo-b")
    (tmp_path / "a.jpg").write_bytes(b"photo-a")
    manifest = [manifest_entry("b.jpg", b"photo-b"), manifest_entry("a.jpg", b"photo-a")]

    result = verify_exact_snapshot(tmp_path, manifest, expected_count=2, max_total_bytes=100)

    assert result["status"] == "MATCH"
    assert result["matched"] == 2
    assert result["missing"] == []
    assert result["mismatched"] == []
    assert result["extra"] == []
    assert [entry["path"] for entry in result["files"]] == ["a.jpg", "b.jpg"]


def test_exact_snapshot_rejects_missing_extra_and_corrupt_files(tmp_path: Path):
    (tmp_path / "kept.jpg").write_bytes(b"changed")
    (tmp_path / "extra.jpg").write_bytes(b"extra")
    manifest = [manifest_entry("kept.jpg", b"expected"), manifest_entry("missing.jpg", b"missing")]

    result = verify_exact_snapshot(tmp_path, manifest, expected_count=2, max_total_bytes=100)

    assert result["status"] == "MISMATCH"
    assert result["missing"] == ["missing.jpg"]
    assert result["mismatched"] == ["kept.jpg"]
    assert result["extra"] == ["extra.jpg"]


def test_exact_snapshot_counts_non_jpeg_files_as_extras(tmp_path: Path):
    (tmp_path / "expected.jpg").write_bytes(b"photo")
    (tmp_path / "unexpected.png").write_bytes(b"image")
    manifest = [manifest_entry("expected.jpg", b"photo")]

    result = verify_exact_snapshot(tmp_path, manifest, expected_count=1, max_total_bytes=100)

    assert result["status"] == "MISMATCH"
    assert result["extra"] == ["unexpected.png"]


def test_exact_snapshot_rejects_out_of_root_and_symlink_paths(tmp_path: Path):
    outside = tmp_path.parent / "outside.jpg"
    outside.write_bytes(b"outside")
    root = tmp_path / "payload"
    root.mkdir()
    (root / "linked.jpg").symlink_to(outside)

    with pytest.raises(SnapshotError, match="symlink"):
        verify_exact_snapshot(root, [manifest_entry("linked.jpg", b"outside")], expected_count=1, max_total_bytes=100)

    with pytest.raises(SnapshotError, match="path"):
        verify_exact_snapshot(root, [manifest_entry("../outside.jpg", b"outside")], expected_count=1, max_total_bytes=100)


def test_spec027_record_requires_identity_evidence_and_ordered_timestamps():
    record = {
        "record_version": 1,
        "run_identity": {
            "run_id": "ng064-20260928-r01",
            "spec_revision": "SPEC-027",
            "spec_blob": "546e4131064beaa82992a78749be67c68ceb6cf6",
            "phase": "contentapp-oracle",
            "requirements_exercised": ["SPEC-062-6.1"],
            "classification": "diagnostic",
            "repetition": 1,
            "comparison_arm": "baseline",
        },
        "source_and_executable_identity": {"participants": {"source": "UNKNOWN", "repository": "UNKNOWN"}},
        "effective_configuration": {"unknown_fields": "UNKNOWN: no guest preflight yet"},
        "measurement_window": {"start_monotonic_ns": 100, "end_monotonic_ns": 200},
        "counter_semantics_and_stage_outcomes": {"unknown_fields": "UNKNOWN: no runtime"},
        "failures_and_resources": {"unknown_fields": "UNKNOWN: no runtime"},
        "evidence_linkage": {"artifacts": []},
    }
    validate_spec027_record(record)

    record["measurement_window"]["end_monotonic_ns"] = 100
    with pytest.raises(PlanError, match="timestamp"):
        validate_spec027_record(record)


class FakeClock:
    def __init__(self):
        self.value = 100.0

    def __call__(self) -> float:
        return self.value

    def tick(self, seconds: float = 1.0) -> float:
        self.value += seconds
        return self.value


class FakeAdapter:
    def __init__(
        self,
        root: Path,
        manifest: list[dict],
        clock: FakeClock,
        source_root: Path | None = None,
        record_root: Path | None = None,
    ):
        self.root = root
        self.manifest = manifest
        self.source_root = source_root or root
        self.record_root = record_root or root.parent / f"{root.name}-evidence"
        self.record_root.mkdir(parents=True, exist_ok=True)
        self.source_manifest = manifest
        self.source_manifest_sha256 = hashlib.sha256(
            json.dumps(manifest, sort_keys=True, separators=(",", ":")).encode()
        ).hexdigest()
        self.clock = clock
        self.calls: list[str] = []
        self.stopped: list[str] = []
        self.readiness_status = "READY"
        self.decoder_status = "PASS"
        self.source_decoder_status = "PASS"
        self.snapshot_observed_at: float | None = None
        self.drain_completed_at: float | None = None
        self.snapshot_immutable = True
        self.snapshot_id_override: str | None = None

    def read_only_preflight(self, plan):
        self.calls.append("preflight")
        checks = {
            name: {"status": self.readiness_status, "evidence": "fixture evidence"}
            for name in ("pgcs_source", "pgcs_repository", "nrncs_source")
        }
        return {
            "status": "READY",
            "checks": checks,
            "source_root": self.source_root,
            "source_manifest": self.source_manifest,
            "source_manifest_sha256": self.source_manifest_sha256,
            "jpeg_validation": {
                "status": self.source_decoder_status,
                "identity": "unit-fixture decoder",
                "evidence": "fixture source decode",
            },
        }

    def launch_repository(self, plan):
        self.calls.append("launch_repository")
        self.clock.tick()
        return "repository-process-handle"

    def await_repository_readiness(self, handle, deadline):
        self.calls.append("repository_readiness")
        now = self.clock.tick()
        return {"status": "READY", "observed_at": now, "evidence": "fixture readiness"}

    def launch_source(self, plan):
        self.calls.append("launch_source")
        self.source_launch_at = self.clock.value
        self.clock.tick()
        return "source-process-handle"

    def await_source_offers(self, handle, expected_count, deadline):
        self.calls.append("source_offers")
        now = self.clock.tick()
        return {"count": expected_count, "observed_at": now, "evidence": "fixture offer count"}

    def stop_source(self, handle, deadline):
        self.calls.append("stop_source")
        now = self.clock.tick()
        return {"status": "STOPPED", "completed_at": now}

    def await_drain(self, handle, deadline):
        self.calls.append("drain")
        now = self.drain_completed_at if self.drain_completed_at is not None else self.clock.tick()
        return {"status": "DRAINED", "completed_at": now, "evidence": "fixture drain evidence"}

    def await_stable_snapshot(self, handle, plan, deadline):
        self.calls.append("stable_snapshot")
        now = self.snapshot_observed_at if self.snapshot_observed_at is not None else self.clock.tick()
        snapshot_root = self.record_root / "receiver-snapshot"
        shutil.copytree(self.root, snapshot_root)
        snapshot_id = self.snapshot_id_override or hashlib.sha256(
            json.dumps(self.manifest, sort_keys=True, separators=(",", ":")).encode()
        ).hexdigest()
        return {
            "stable": True,
            "observed_at": now,
            "root": snapshot_root,
            "immutable_snapshot": self.snapshot_immutable,
            "snapshot_id": snapshot_id,
            "snapshot_evidence": "fixture immutable-snapshot evidence",
            "manifest": self.manifest,
            "jpeg_validation": {
                "status": self.decoder_status,
                "identity": "unit-fixture decoder",
                "evidence": "unit-fixture decoder result",
            },
        }

    def stop_owned_role(self, handle):
        self.stopped.append(handle)
        return {"status": "STOPPED", "evidence": f"fixture teardown {handle}"}

    def build_spec027_record(self, plan, outcome):
        start_s = outcome.get("rate_window_start", 100.0)
        end_s = outcome.get("completion_at") or outcome.get("attempt_end_at")
        if end_s is None or end_s <= start_s:
            end_s = start_s + 1.0
        return {
            "record_version": 1,
            "run_identity": {
                "run_id": plan["run_id"],
                "spec_revision": "SPEC-027",
                "spec_blob": plan["spec027_text_blob"],
                "phase": "contentapp-oracle",
                "requirements_exercised": ["SPEC-062-6.1"],
                "classification": "diagnostic",
                "repetition": 1,
                "comparison_arm": "baseline",
            },
            "source_and_executable_identity": {"participants": {"source": "UNKNOWN", "repository": "UNKNOWN"}},
            "effective_configuration": {"unknown": "UNKNOWN: unit adapter only"},
            "measurement_window": {
                "start_monotonic_ns": int(start_s * 1_000_000_000),
                "end_monotonic_ns": int(end_s * 1_000_000_000),
            },
            "counter_semantics_and_stage_outcomes": {"unknown": "UNKNOWN: unit adapter only"},
            "failures_and_resources": {"unknown": "UNKNOWN: unit adapter only"},
            "evidence_linkage": {"artifacts": []},
            "run_outcome": outcome,
        }


def fixture_payload(root: Path) -> list[dict]:
    entries = []
    for index in range(1000):
        data = f"fixture-photo-{index}".encode()
        name = f"photo-{index:04}.jpg"
        (root / name).write_bytes(data)
        entries.append(manifest_entry(name, data))
    return entries


def plan_for(adapter: FakeAdapter) -> dict:
    plan = valid_plan()
    plan["workload_manifest_sha256"] = adapter.source_manifest_sha256
    plan["run_id"] = f"ng064-{adapter.root.parent.name}"
    plan["evidence_root"] = str(adapter.record_root)
    return plan


def run_fixture(tmp_path: Path):
    source_root = tmp_path / "source"
    repository_root = tmp_path / "repository"
    evidence_root = tmp_path / "evidence"
    source_root.mkdir()
    repository_root.mkdir()
    evidence_root.mkdir()
    manifest = fixture_payload(source_root)
    fixture_payload(repository_root)
    clock = FakeClock()
    adapter = FakeAdapter(repository_root, manifest, clock, source_root, evidence_root)
    return adapter, clock


def test_local_runner_requires_observable_readiness_before_launch(tmp_path: Path):
    root = tmp_path / "repository"
    root.mkdir()
    clock = FakeClock()
    adapter = FakeAdapter(root, [], clock)
    adapter.readiness_status = "UNKNOWN"

    with pytest.raises(RunBlocked, match="readiness"):
        run_profile(plan_for(adapter), adapter, clock=clock)

    assert adapter.calls == ["preflight"]
    assert adapter.stopped == []
    assert list(adapter.record_root.glob("*.spec027-run-record.v1.json")) == []


def test_local_runner_blocks_unknown_source_jpeg_validation_before_launch(tmp_path: Path):
    adapter, clock = run_fixture(tmp_path)
    adapter.source_decoder_status = "UNKNOWN"

    with pytest.raises(RunBlocked, match="Source JPEG decodability is UNKNOWN"):
        run_profile(plan_for(adapter), adapter, clock=clock)

    assert adapter.calls == ["preflight"]
    assert adapter.stopped == []


def test_local_runner_uses_one_repository_deadline_and_nested_source_rate_window(tmp_path: Path):
    adapter, clock = run_fixture(tmp_path)

    result = run_profile(plan_for(adapter), adapter, clock=clock)

    assert result["status"] == "PHOTO_TRANSFER_PASS"
    assert result["teardown_status"] == "PASS"
    assert result["repository_deadline_at"] == 700.0
    assert result["rate_window_start"] == adapter.source_launch_at
    assert result["snapshot_id"]
    assert result["snapshot_evidence"] == "fixture immutable-snapshot evidence"
    assert adapter.calls == [
        "preflight", "launch_repository", "repository_readiness", "launch_source",
        "source_offers", "stop_source", "drain", "stable_snapshot",
    ]
    assert adapter.stopped == ["source-process-handle", "repository-process-handle"]
    assert Path(result["run_record_path"]).exists()
    assert len(list(adapter.record_root.glob("*.spec027-run-record.v1.json"))) == 1
    saved = json.loads(Path(result["run_record_path"]).read_text())
    assert saved["run_identity"]["run_id"] == result["run_id"]

    calls_before_reuse = list(adapter.calls)
    with pytest.raises(RunBlocked, match="already exists"):
        run_profile(plan_for(adapter), adapter, clock=clock)
    assert adapter.calls == calls_before_reuse


def test_local_runner_rejects_completion_observed_after_the_fixed_deadline(tmp_path: Path):
    adapter, clock = run_fixture(tmp_path)
    adapter.snapshot_observed_at = 701.0

    with pytest.raises(RunBlocked, match="600-second"):
        run_profile(plan_for(adapter), adapter, clock=clock)

    assert "launch_repository" in adapter.calls
    assert "launch_source" in adapter.calls
    assert adapter.stopped == ["source-process-handle", "repository-process-handle"]
    records = list(adapter.record_root.glob("*.spec027-run-record.v1.json"))
    assert len(records) == 1
    assert json.loads(records[0].read_text())["run_outcome"]["status"] == "INCONCLUSIVE"


def test_local_runner_rejects_drain_that_finishes_after_the_deadline(tmp_path: Path):
    adapter, clock = run_fixture(tmp_path)
    adapter.drain_completed_at = 700.1

    with pytest.raises(RunBlocked, match="600-second"):
        run_profile(plan_for(adapter), adapter, clock=clock)


def test_local_runner_does_not_pass_when_decoder_evidence_is_unknown(tmp_path: Path):
    adapter, clock = run_fixture(tmp_path)
    adapter.decoder_status = "UNKNOWN"

    with pytest.raises(RunBlocked, match="decodability is UNKNOWN"):
        run_profile(plan_for(adapter), adapter, clock=clock)


def test_local_runner_rejects_a_mutable_receiver_snapshot(tmp_path: Path):
    adapter, clock = run_fixture(tmp_path)
    adapter.snapshot_immutable = False

    with pytest.raises(RunBlocked, match="immutable"):
        run_profile(plan_for(adapter), adapter, clock=clock)


def test_local_runner_rejects_receiver_snapshot_identity_drift(tmp_path: Path):
    adapter, clock = run_fixture(tmp_path)
    adapter.snapshot_id_override = "f" * 64

    with pytest.raises(RunBlocked, match="snapshot identity"):
        run_profile(plan_for(adapter), adapter, clock=clock)


def test_repository_snapshot_cannot_supply_its_own_expected_manifest(tmp_path: Path):
    source_root = tmp_path / "source"
    repository_root = tmp_path / "repository"
    source_root.mkdir()
    repository_root.mkdir()
    source_manifest = fixture_payload(source_root)
    fixture_payload(repository_root)
    (repository_root / "photo-0000.jpg").write_bytes(b"receiver-corruption")
    receiver_manifest = [
        manifest_entry(entry["path"], (repository_root / entry["path"]).read_bytes())
        for entry in source_manifest
    ]
    source_digest = hashlib.sha256(
        json.dumps(source_manifest, sort_keys=True, separators=(",", ":")).encode()
    ).hexdigest()
    plan = valid_plan()
    plan["workload_manifest_sha256"] = source_digest

    class IndependentManifestAdapter(FakeAdapter):
        def read_only_preflight(self, plan):
            result = super().read_only_preflight(plan)
            result["source_root"] = source_root
            result["source_manifest"] = source_manifest
            result["source_manifest_sha256"] = source_digest
            return result

    clock = FakeClock()
    adapter = IndependentManifestAdapter(
        repository_root, receiver_manifest, clock, source_root, tmp_path / "evidence"
    )
    adapter.record_root.mkdir(parents=True, exist_ok=True)
    plan["evidence_root"] = str(adapter.record_root)
    plan["run_id"] = f"ng064-{tmp_path.name}"
    result = run_profile(plan, adapter, clock=clock)

    assert result["status"] == "PHOTO_TRANSFER_FAIL"
    assert result["oracle"]["mismatched"] == ["photo-0000.jpg"]


def test_snapshot_rejects_a_file_changed_during_hashing(tmp_path: Path, monkeypatch):
    data = b"original"
    file_path = tmp_path / "one.jpg"
    file_path.write_bytes(data)
    manifest = [manifest_entry("one.jpg", data)]
    original_signature = runner_module._file_signature
    calls = 0

    def mutate_after_first_signature(path):
        nonlocal calls
        signature = original_signature(path)
        calls += 1
        if calls == 1:
            path.write_bytes(b"mutated")
        return signature

    monkeypatch.setattr(runner_module, "_file_signature", mutate_after_first_signature)
    with pytest.raises(SnapshotError, match="changed"):
        verify_exact_snapshot(tmp_path, manifest, expected_count=1, max_total_bytes=100)


def test_spec027_record_writer_rejects_duplicate_run_ids(tmp_path: Path):
    record = {
        "record_version": 1,
        "run_identity": {
            "run_id": "ng064-20260928-r01", "spec_revision": "SPEC-027",
            "spec_blob": "546e4131064beaa82992a78749be67c68ceb6cf6",
            "phase": "contentapp-oracle", "requirements_exercised": ["SPEC-062-6.1"],
            "classification": "diagnostic", "repetition": 1, "comparison_arm": "baseline",
        },
        "source_and_executable_identity": {"participants": {"source": "UNKNOWN", "repository": "UNKNOWN"}},
        "effective_configuration": {"unknown_fields": "UNKNOWN: no guest preflight yet"},
        "measurement_window": {"start_monotonic_ns": 100, "end_monotonic_ns": 200},
        "counter_semantics_and_stage_outcomes": {"unknown_fields": "UNKNOWN: no runtime"},
        "failures_and_resources": {"unknown_fields": "UNKNOWN: no runtime"},
        "evidence_linkage": {"artifacts": []},
    }
    first = write_spec027_record_once(tmp_path, record)

    assert first.exists()
    with pytest.raises(PlanError, match="already exists"):
        write_spec027_record_once(tmp_path, record)


def test_cli_validate_plan_uses_the_real_entrypoint(tmp_path: Path):
    plan_path = tmp_path / "plan.json"
    plan_path.write_text(json.dumps(valid_plan()), encoding="utf-8")

    result = subprocess.run(
        [sys.executable, str(ALPINEVM_DIR / "ng_elc_contentapp_runner.py"), "validate-plan", str(plan_path)],
        capture_output=True,
        text=True,
        check=False,
    )

    assert result.returncode == 0
    assert result.stdout.strip() == "PLAN_VALID"


def test_cli_snapshot_verifier_uses_the_real_entrypoint(tmp_path: Path):
    root = tmp_path / "payload"
    root.mkdir()
    (root / "a.jpg").write_bytes(b"jpeg-a")
    (root / "b.jpg").write_bytes(b"jpeg-b")
    manifest = [manifest_entry("a.jpg", b"jpeg-a"), manifest_entry("b.jpg", b"jpeg-b")]
    manifest_path = tmp_path / "manifest.json"
    manifest_path.write_text(json.dumps(manifest), encoding="utf-8")

    result = subprocess.run(
        [
            sys.executable,
            str(ALPINEVM_DIR / "ng_elc_contentapp_runner.py"),
            "verify-snapshot",
            str(root),
            str(manifest_path),
            "--expected-count",
            "2",
            "--max-total-bytes",
            "100",
        ],
        capture_output=True,
        text=True,
        check=False,
    )

    assert result.returncode == 0
    assert json.loads(result.stdout)["status"] == "MATCH"
