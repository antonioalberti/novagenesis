#!/usr/bin/env python3
"""Fail-closed local plan and payload-oracle helpers for the AIOPT5 photo profile.

This module does not connect to guests, build binaries, launch workloads, or
change VM power state. It validates the owner-fixed profile boundary and checks
an already captured payload snapshot. Runtime launch remains gated on an
approved concrete plan and read-only guest preflight.
"""
from __future__ import annotations

import argparse
import errno
import hashlib
import json
import math
import os
import re
import stat
import sys
import time
from pathlib import Path, PurePosixPath
from typing import Any, Mapping, Sequence

PROFILE = "contentapp-source-to-repository-1000-photos"
SOURCE_HEAD = "1af604dad66148618cc0e578c28adb4a9829e607"
SPEC027_TEXT_BLOB = "546e4131064beaa82992a78749be67c68ceb6cf6"
SPEC027_TEXT_SHA256 = "c481af00535c189dd1b24cce71096ab4a7e13f34e771175df251290713d17ad5"
CONTENTAPP_DEADLINE_S = 600
GRACEFUL_SHUTDOWN_S_PER_GUEST = 120
PHOTO_EXTENSIONS = {".jpg", ".jpeg"}


class PlanError(ValueError):
    """The fixed AIOPT5 profile contract is incomplete or contradictory."""


class SnapshotError(ValueError):
    """A payload root or manifest cannot be safely inventoried."""


class RunBlocked(RuntimeError):
    """The local controller cannot establish a required run condition."""


def validate_plan(plan: Mapping[str, Any]) -> None:
    """Check the owner-fixed profile invariants; unknown runtime values stay explicit."""
    if not isinstance(plan, Mapping):
        raise PlanError("plan must be a JSON object")
    if plan.get("profile") != PROFILE:
        raise PlanError(f"profile must be {PROFILE!r}")
    run_id = plan.get("run_id")
    if not isinstance(run_id, str) or not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]{0,127}", run_id):
        raise PlanError("run_id must be a non-empty safe identifier")
    participants = plan.get("participants")
    if participants != {"controller": 100, "source": 102, "repository": 101}:
        raise PlanError("participants must map controller/source/repository to VM 100/102/101")
    if plan.get("source_head") != SOURCE_HEAD:
        raise PlanError("source_head must identify the pinned AIOPT5 candidate")
    if plan.get("spec027_text_blob") != SPEC027_TEXT_BLOB:
        raise PlanError("SPEC-027 approved text blob identity is incorrect")
    if plan.get("spec027_text_sha256") != SPEC027_TEXT_SHA256:
        raise PlanError("SPEC-027 approved text SHA-256 identity is incorrect")
    if plan.get("photo_count") != 1000:
        raise PlanError("photo_count must be exactly 1000")
    if plan.get("contentapp_deadline_s") != CONTENTAPP_DEADLINE_S:
        raise PlanError("the single Repository-origin ContentApp deadline must be 600 seconds")
    if plan.get("graceful_shutdown_s_per_guest") != GRACEFUL_SHUTDOWN_S_PER_GUEST:
        raise PlanError("graceful shutdown limit must be 120 seconds per guest")
    if plan.get("deadline_origin") != "repository_contentapp_launch":
        raise PlanError("deadline_origin must be repository_contentapp_launch")
    if plan.get("rate_window_start") != "immediately_before_source_contentapp_launch":
        raise PlanError("rate_window_start must precede Source ContentApp launch")
    if plan.get("completion_oracle") != "stable_repository_snapshot":
        raise PlanError("completion_oracle must be stable_repository_snapshot")
    max_total_bytes = plan.get("max_total_bytes")
    if not isinstance(max_total_bytes, int) or isinstance(max_total_bytes, bool) or max_total_bytes <= 0:
        raise PlanError("max_total_bytes must be an explicit positive bound")
    manifest_digest = plan.get("workload_manifest_sha256")
    if not isinstance(manifest_digest, str) or not re.fullmatch(r"[0-9a-f]{64}", manifest_digest):
        raise PlanError("workload_manifest_sha256 must pin the immutable Source manifest")
    evidence_root = plan.get("evidence_root")
    if not isinstance(evidence_root, str) or not Path(evidence_root).is_absolute():
        raise PlanError("evidence_root must be an explicit absolute local path")
    if "vm_power_actions" not in plan or plan["vm_power_actions"] != []:
        raise PlanError("VM power actions are forbidden in this run profile")


def _safe_relative_path(raw: Any) -> PurePosixPath:
    if not isinstance(raw, str) or not raw or "\\" in raw or "\x00" in raw:
        raise SnapshotError("manifest path is invalid")
    path = PurePosixPath(raw)
    if path.is_absolute() or any(part in ("", ".", "..") for part in path.parts):
        raise SnapshotError(f"manifest path escapes or is not canonical: {raw!r}")
    if path.as_posix() != raw:
        raise SnapshotError(f"manifest path is not canonical: {raw!r}")
    if any(ord(ch) < 32 for ch in raw):
        raise SnapshotError("manifest path contains a control character")
    return path


def _normalize_manifest(manifest: Sequence[Mapping[str, Any]], expected_count: int) -> dict[str, dict[str, Any]]:
    if not isinstance(manifest, Sequence) or isinstance(manifest, (str, bytes)):
        raise SnapshotError("manifest must be a list of file entries")
    if len(manifest) != expected_count:
        raise SnapshotError(f"manifest must contain exactly {expected_count} files")
    normalized: dict[str, dict[str, Any]] = {}
    for item in manifest:
        if not isinstance(item, Mapping):
            raise SnapshotError("manifest entries must be objects")
        path = _safe_relative_path(item.get("path")).as_posix()
        size = item.get("size")
        digest = item.get("sha256")
        if not isinstance(size, int) or isinstance(size, bool) or size < 0:
            raise SnapshotError(f"invalid size for {path!r}")
        if not isinstance(digest, str) or not re.fullmatch(r"[0-9a-f]{64}", digest):
            raise SnapshotError(f"invalid SHA-256 for {path!r}")
        if Path(path).suffix.lower() not in PHOTO_EXTENSIONS:
            raise SnapshotError(f"manifest path is not a JPEG photo: {path!r}")
        if path in normalized:
            raise SnapshotError(f"duplicate manifest path: {path!r}")
        normalized[path] = {"path": path, "size": size, "sha256": digest}
    return normalized


def _file_signature(path: Path) -> tuple[int, int, int, int, int]:
    info = path.lstat()
    if stat.S_ISLNK(info.st_mode):
        raise SnapshotError(f"symlink is not allowed in payload root: {path}")
    if not stat.S_ISREG(info.st_mode):
        raise SnapshotError(f"non-regular file is not allowed in payload root: {path}")
    return (info.st_dev, info.st_ino, info.st_size, info.st_mtime_ns, info.st_ctime_ns)


def _inventory_payload_files(root: Path) -> dict[str, Path]:
    found: dict[str, Path] = {}
    pending = [root]
    while pending:
        directory = pending.pop()
        try:
            with os.scandir(directory) as scan:
                entries = sorted(scan, key=lambda entry: entry.name)
        except OSError as exc:
            raise SnapshotError(f"cannot inventory {directory}: {exc}") from exc
        for entry in entries:
            path = Path(entry.path)
            try:
                mode = entry.stat(follow_symlinks=False).st_mode
            except OSError as exc:
                raise SnapshotError(f"cannot stat {path}: {exc}") from exc
            if stat.S_ISLNK(mode):
                raise SnapshotError(f"symlink is not allowed in payload root: {path}")
            if stat.S_ISDIR(mode):
                pending.append(path)
            elif stat.S_ISREG(mode):
                relative = path.relative_to(root).as_posix()
                found[relative] = path
            else:
                raise SnapshotError(f"non-regular filesystem entry is not allowed: {path}")
    return found


def _hash_stable_file(path: Path) -> dict[str, Any]:
    before = _file_signature(path)
    flags = os.O_RDONLY | getattr(os, "O_NOFOLLOW", 0) | getattr(os, "O_NONBLOCK", 0)
    try:
        fd = os.open(path, flags)
    except OSError as exc:
        raise SnapshotError(f"cannot open regular payload file {path}: {exc}") from exc
    digest = hashlib.sha256()
    try:
        with os.fdopen(fd, "rb", closefd=True) as stream:
            opened = os.fstat(stream.fileno())
            opened_signature = (opened.st_dev, opened.st_ino, opened.st_size, opened.st_mtime_ns, opened.st_ctime_ns)
            if opened_signature != before:
                raise SnapshotError(f"file changed before hashing: {path}")
            while True:
                chunk = stream.read(1024 * 1024)
                if not chunk:
                    break
                digest.update(chunk)
            after_fd = os.fstat(stream.fileno())
            after_signature = (after_fd.st_dev, after_fd.st_ino, after_fd.st_size, after_fd.st_mtime_ns, after_fd.st_ctime_ns)
    except OSError as exc:
        raise SnapshotError(f"cannot hash {path}: {exc}") from exc
    after_path = _file_signature(path)
    if before != after_signature or before != after_path:
        raise SnapshotError(f"file changed during hashing: {path}")
    return {"size": before[2], "sha256": digest.hexdigest()}


def verify_exact_snapshot(
    root: str | os.PathLike[str],
    manifest: Sequence[Mapping[str, Any]],
    *,
    expected_count: int = 1000,
    max_total_bytes: int,
    max_actual_files: int | None = None,
) -> dict[str, Any]:
    """Compare a stable payload-only root snapshot to an explicit JPEG manifest.

    Every regular file in the isolated payload root is included, so non-JPEG
    extras cannot disappear from the exact-set comparison. This verifies names,
    sizes and bytes, not JPEG decodability; the caller must record a decoder.
    """
    if not isinstance(expected_count, int) or isinstance(expected_count, bool) or expected_count <= 0:
        raise SnapshotError("expected_count must be a positive integer")
    if not isinstance(max_total_bytes, int) or isinstance(max_total_bytes, bool) or max_total_bytes <= 0:
        raise SnapshotError("max_total_bytes must be a positive integer")
    manifest_by_path = _normalize_manifest(manifest, expected_count)
    expected_bytes = sum(item["size"] for item in manifest_by_path.values())
    if expected_bytes > max_total_bytes:
        raise SnapshotError("manifest exceeds the total byte limit")

    root_path = Path(root)
    if root_path.is_symlink():
        raise SnapshotError("payload root must not be a symlink")
    try:
        root_path = root_path.resolve(strict=True)
    except OSError as exc:
        raise SnapshotError(f"payload root is unavailable: {exc}") from exc
    if not root_path.is_dir():
        raise SnapshotError("payload root must be a directory")

    before_files = _inventory_payload_files(root_path)
    limit = max_actual_files if max_actual_files is not None else max(expected_count + 100, 1024)
    if not isinstance(limit, int) or isinstance(limit, bool) or limit < expected_count:
        raise SnapshotError("max_actual_files must be an integer at least expected_count")
    if len(before_files) > limit:
        raise SnapshotError("actual photo count exceeds the bounded inventory limit")
    before_signatures = {name: _file_signature(path) for name, path in before_files.items()}
    received_bytes = sum(signature[2] for signature in before_signatures.values())
    if received_bytes > max_total_bytes:
        raise SnapshotError("actual payload exceeds the total byte limit")

    hashed = {name: _hash_stable_file(path) for name, path in sorted(before_files.items())}
    after_files = _inventory_payload_files(root_path)
    after_signatures = {name: _file_signature(path) for name, path in after_files.items()}
    if before_signatures != after_signatures:
        raise SnapshotError("payload root changed during snapshot hashing")

    expected_paths = set(manifest_by_path)
    actual_paths = set(hashed)
    missing = sorted(expected_paths - actual_paths)
    extra = sorted(actual_paths - expected_paths)
    mismatched = sorted(
        path for path in expected_paths & actual_paths
        if hashed[path]["size"] != manifest_by_path[path]["size"]
        or hashed[path]["sha256"] != manifest_by_path[path]["sha256"]
    )
    files = [{"path": path, **hashed[path]} for path in sorted(hashed)]
    canonical_expected = [manifest_by_path[path] for path in sorted(manifest_by_path)]
    expected_entries_digest = hashlib.sha256(
        json.dumps(canonical_expected, sort_keys=True, separators=(",", ":")).encode("utf-8")
    ).hexdigest()
    canonical_actual = [{"path": item["path"], "size": item["size"], "sha256": item["sha256"]} for item in files]
    actual_entries_digest = hashlib.sha256(
        json.dumps(canonical_actual, sort_keys=True, separators=(",", ":")).encode("utf-8")
    ).hexdigest()
    exact = not (missing or extra or mismatched) and len(files) == expected_count
    return {
        "status": "MATCH" if exact else "MISMATCH",
        "expected_count": expected_count,
        "received_count": len(files),
        "matched": expected_count - len(missing) - len(mismatched),
        "expected_bytes": expected_bytes,
        "received_bytes": received_bytes,
        "entries_sha256": actual_entries_digest,
        "expected_entries_sha256": expected_entries_digest,
        "missing": missing,
        "mismatched": mismatched,
        "extra": extra,
        "files": files,
    }


def validate_spec027_record(record: Mapping[str, Any]) -> None:
    """Validate record identity/timestamp envelope, not full SPEC-027 acceptance.

    All remaining §8.5 values still require captured evidence or explicit
    UNKNOWN reasons; this structural check cannot turn unknowns into passes.
    """
    if not isinstance(record, Mapping):
        raise PlanError("run record must be an object")
    if record.get("record_version") != 1:
        raise PlanError("run record must declare record_version 1")
    identity = record.get("run_identity")
    required_identity = {
        "run_id", "spec_revision", "spec_blob", "phase", "requirements_exercised",
        "classification", "repetition", "comparison_arm",
    }
    if not isinstance(identity, Mapping) or not required_identity.issubset(identity):
        raise PlanError("run record is missing required §8.5 identity fields")
    if not isinstance(identity["run_id"], str) or not identity["run_id"].strip():
        raise PlanError("run record requires a unique non-empty run_id")
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9._-]{0,127}", identity["run_id"]):
        raise PlanError("run record run_id must be a safe identifier")
    if identity["classification"] not in {"diagnostic", "correctness", "performance"}:
        raise PlanError("invalid run classification")
    if not isinstance(identity["repetition"], int) or isinstance(identity["repetition"], bool) or identity["repetition"] < 1:
        raise PlanError("repetition must be a positive integer")
    if not isinstance(identity["requirements_exercised"], list) or not identity["requirements_exercised"]:
        raise PlanError("requirements_exercised must be a non-empty list")
    for key in (
        "source_and_executable_identity", "effective_configuration",
        "measurement_window", "counter_semantics_and_stage_outcomes",
        "failures_and_resources", "evidence_linkage",
    ):
        if not isinstance(record.get(key), Mapping):
            raise PlanError(f"run record is missing §8.5 section {key}")
    window = record["measurement_window"]
    start = window.get("start_monotonic_ns")
    end = window.get("end_monotonic_ns")
    if (
        not isinstance(start, int) or isinstance(start, bool)
        or not isinstance(end, int) or isinstance(end, bool)
        or start < 0 or end <= start
    ):
        raise PlanError("measurement timestamps must be monotonic and ordered")
    evidence = record["evidence_linkage"].get("artifacts")
    if not isinstance(evidence, list):
        raise PlanError("evidence_linkage.artifacts must be a list")


def _event_time(event: Mapping[str, Any], key: str, deadline: float, label: str) -> float:
    value = event.get(key)
    if not isinstance(value, (int, float)) or isinstance(value, bool) or not math.isfinite(value):
        raise RunBlocked(f"{label} has no valid monotonic timestamp")
    value = float(value)
    if value > deadline:
        raise RunBlocked(f"{label} occurred after the fixed 600-second ContentApp deadline")
    return value


def _require_observed_ready(event: Any, deadline: float, label: str) -> float:
    if not isinstance(event, Mapping) or event.get("status") != "READY":
        raise RunBlocked(f"{label} readiness is UNKNOWN or not READY")
    evidence = event.get("evidence")
    if not isinstance(evidence, str) or not evidence.strip():
        raise RunBlocked(f"{label} readiness has no evidence reference")
    return _event_time(event, "observed_at", deadline, label)


def run_profile(plan: Mapping[str, Any], adapter: Any, *, clock=time.monotonic) -> dict[str, Any]:
    """Run the fixed lifecycle against an injected adapter (tests use fakes).

    No SSH/guest adapter is shipped in this bounded local increment. An adapter
    must implement read_only_preflight, role launch/observation, and
    identity-scoped stop_owned_role/build_spec027_record methods. VM power
    operations are not part of this interface.
    """
    validate_plan(plan)
    if not callable(getattr(adapter, "build_spec027_record", None)):
        raise RunBlocked("adapter cannot create the required SPEC-027 run record")
    if not callable(getattr(adapter, "stop_owned_role", None)):
        raise RunBlocked("adapter cannot perform identity-scoped process teardown")
    record_root = Path(plan["evidence_root"])
    if record_root.is_symlink() or not record_root.is_dir():
        raise RunBlocked("evidence_root must exist as a non-symlink directory before launch")
    record_target = record_root / f"{plan['run_id']}.spec027-run-record.v1.json"
    if record_target.exists() or record_target.is_symlink():
        raise RunBlocked("a run record already exists for this run_id")

    preflight = adapter.read_only_preflight(plan)
    if not isinstance(preflight, Mapping) or preflight.get("status") != "READY":
        raise RunBlocked("read-only preflight is UNKNOWN or not READY")
    checks = preflight.get("checks")
    required_checks = ("pgcs_source", "pgcs_repository", "nrncs_source")
    if not isinstance(checks, Mapping):
        raise RunBlocked("readiness evidence is UNKNOWN")
    for name in required_checks:
        check = checks.get(name)
        if (
            not isinstance(check, Mapping)
            or check.get("status") != "READY"
            or not isinstance(check.get("evidence"), str)
            or not check["evidence"].strip()
        ):
            raise RunBlocked(f"{name} readiness is UNKNOWN or lacks evidence")

    source_root = preflight.get("source_root")
    source_manifest = preflight.get("source_manifest")
    source_manifest_sha256 = preflight.get("source_manifest_sha256")
    if not isinstance(source_root, (str, os.PathLike)):
        raise RunBlocked("Source staging root is UNKNOWN")
    if not isinstance(source_manifest, Sequence) or isinstance(source_manifest, (str, bytes)):
        raise RunBlocked("Source publish-time manifest entries are UNKNOWN")
    if source_manifest_sha256 != plan["workload_manifest_sha256"]:
        raise RunBlocked("Source publish-time manifest hash differs from the run plan")
    source_decoder = preflight.get("jpeg_validation")
    if (
        not isinstance(source_decoder, Mapping)
        or source_decoder.get("status") != "PASS"
        or not isinstance(source_decoder.get("identity"), str)
        or not source_decoder["identity"].strip()
        or not isinstance(source_decoder.get("evidence"), str)
        or not source_decoder["evidence"].strip()
    ):
        raise RunBlocked("Source JPEG decodability is UNKNOWN; launch is blocked")
    source_oracle = verify_exact_snapshot(
        source_root,
        source_manifest,
        expected_count=1000,
        max_total_bytes=plan["max_total_bytes"],
    )
    if source_oracle["status"] != "MATCH":
        raise RunBlocked("Source staging does not match its frozen 1000-photo manifest")

    handles: list[Any] = []
    repository_handle = None
    source_handle = None
    repository_start: float | None = None
    deadline: float | None = None
    rate_window_start: float | None = None
    repository_launch_attempted = False
    outcome: dict[str, Any] | None = None
    run_error: Exception | None = None
    teardown: list[dict[str, Any]] = []
    try:
        repository_start = float(clock())
        if not math.isfinite(repository_start):
            raise RunBlocked("repository launch clock is invalid")
        deadline = repository_start + CONTENTAPP_DEADLINE_S
        repository_launch_attempted = True
        repository_handle = adapter.launch_repository(plan)
        if repository_handle is None:
            raise RunBlocked("Repository ContentApp launch returned no owned handle")
        handles.append(repository_handle)
        if float(clock()) > deadline:
            raise RunBlocked("Repository ContentApp launch exceeded the fixed 600-second deadline")

        repository_ready = adapter.await_repository_readiness(repository_handle, deadline)
        repository_ready_at = _require_observed_ready(repository_ready, deadline, "Repository ContentApp")
        if repository_ready_at < repository_start:
            raise RunBlocked("Repository readiness timestamp precedes Repository launch")
        if float(clock()) > deadline:
            raise RunBlocked("Repository readiness exceeded the fixed 600-second ContentApp deadline")

        rate_window_start = float(clock())
        if not math.isfinite(rate_window_start) or rate_window_start > deadline:
            raise RunBlocked("Source rate-window start is outside the ContentApp deadline")
        source_handle = adapter.launch_source(plan)
        if source_handle is None:
            raise RunBlocked("Source ContentApp launch returned no owned handle")
        handles.append(source_handle)
        if float(clock()) > deadline:
            raise RunBlocked("Source ContentApp launch exceeded the fixed 600-second deadline")

        offers = adapter.await_source_offers(source_handle, 1000, deadline)
        if not isinstance(offers, Mapping) or offers.get("count") != 1000:
            raise RunBlocked("Source offer count is UNKNOWN or differs from 1000")
        offers_at = _event_time(offers, "observed_at", deadline, "Source offered-set observation")
        if offers_at < rate_window_start:
            raise RunBlocked("Source offered-set timestamp precedes the Source rate-window start")
        if not isinstance(offers.get("evidence"), str) or not offers["evidence"].strip():
            raise RunBlocked("Source offered-set observation has no evidence reference")
        if float(clock()) > deadline:
            raise RunBlocked("Source offered-set observation exceeded the fixed deadline")

        source_stop = adapter.stop_source(source_handle, deadline)
        if not isinstance(source_stop, Mapping) or source_stop.get("status") != "STOPPED":
            raise RunBlocked("Source process stop/drain is incomplete")
        source_stopped_at = _event_time(source_stop, "completed_at", deadline, "Source stop")
        if source_stopped_at < offers_at:
            raise RunBlocked("Source stop timestamp precedes the offered-set observation")
        if float(clock()) > deadline:
            raise RunBlocked("Source stop exceeded the fixed 600-second ContentApp deadline")

        drain = adapter.await_drain(repository_handle, deadline)
        if not isinstance(drain, Mapping) or drain.get("status") != "DRAINED":
            raise RunBlocked("Repository drain is incomplete or UNKNOWN")
        if not isinstance(drain.get("evidence"), str) or not drain["evidence"].strip():
            raise RunBlocked("Repository drain has no evidence reference")
        drained_at = _event_time(drain, "completed_at", deadline, "Repository drain")
        if drained_at < source_stopped_at:
            raise RunBlocked("Repository drain timestamp precedes Source stop")
        if float(clock()) > deadline:
            raise RunBlocked("Repository drain exceeded the fixed 600-second ContentApp deadline")

        snapshot = adapter.await_stable_snapshot(repository_handle, plan, deadline)
        if not isinstance(snapshot, Mapping) or snapshot.get("stable") is not True:
            raise RunBlocked("Repository completion snapshot is UNKNOWN or unstable")
        if snapshot.get("immutable_snapshot") is not True:
            raise RunBlocked("Repository snapshot is not proven immutable")
        snapshot_id = snapshot.get("snapshot_id")
        if not isinstance(snapshot_id, str) or not re.fullmatch(r"[0-9a-f]{64}", snapshot_id):
            raise RunBlocked("Repository immutable snapshot identity is UNKNOWN")
        snapshot_evidence = snapshot.get("snapshot_evidence")
        if not isinstance(snapshot_evidence, str) or not snapshot_evidence.strip():
            raise RunBlocked("Repository immutable snapshot has no evidence reference")
        completion_at = _event_time(snapshot, "observed_at", deadline, "Repository stable snapshot")
        if completion_at < drained_at:
            raise RunBlocked("Repository stable snapshot precedes completion of drain")
        if float(clock()) > deadline:
            raise RunBlocked("Repository stable snapshot was not returned within the fixed deadline")
        decoder = snapshot.get("jpeg_validation")
        if (
            not isinstance(decoder, Mapping)
            or decoder.get("status") != "PASS"
            or not isinstance(decoder.get("identity"), str)
            or not decoder["identity"].strip()
            or not isinstance(decoder.get("evidence"), str)
            or not decoder["evidence"].strip()
        ):
            raise RunBlocked("JPEG decodability is UNKNOWN; delivery cannot pass")

        snapshot_root = snapshot.get("root")
        if not isinstance(snapshot_root, (str, os.PathLike)):
            raise RunBlocked("Repository snapshot path is UNKNOWN")
        snapshot_path = Path(snapshot_root)
        if snapshot_path.is_symlink():
            raise RunBlocked("Repository snapshot path must not be a symlink")
        try:
            resolved_snapshot = snapshot_path.resolve(strict=True)
            resolved_evidence = record_root.resolve(strict=True)
            resolved_snapshot.relative_to(resolved_evidence)
        except (OSError, ValueError) as exc:
            raise RunBlocked("Repository snapshot must be confined to the approved evidence root") from exc
        oracle = verify_exact_snapshot(
            resolved_snapshot,
            source_manifest,
            expected_count=1000,
            max_total_bytes=plan["max_total_bytes"],
        )
        if oracle["entries_sha256"] != snapshot_id:
            raise RunBlocked("Repository snapshot identity does not match the hashed snapshot")
        source_after = verify_exact_snapshot(
            source_root,
            source_manifest,
            expected_count=1000,
            max_total_bytes=plan["max_total_bytes"],
        )
        if source_after["status"] != "MATCH":
            raise RunBlocked("Source workload changed during the ContentApp run")
        elapsed = completion_at - rate_window_start
        if elapsed <= 0:
            raise RunBlocked("Source-launch-to-completion measurement interval is not positive")
        outcome = {
            "status": "PHOTO_TRANSFER_PASS" if oracle["status"] == "MATCH" else "PHOTO_TRANSFER_FAIL",
            "run_id": plan["run_id"],
            "workload_manifest_sha256": plan["workload_manifest_sha256"],
            "repository_launch_at": repository_start,
            "repository_deadline_at": deadline,
            "rate_window_start": rate_window_start,
            "completion_at": completion_at,
            "attempt_end_at": float(clock()),
            "observed_elapsed_s": elapsed,
            "observed_photos_per_s": oracle["matched"] / elapsed,
            "snapshot_id": snapshot_id,
            "snapshot_evidence": snapshot_evidence,
            "oracle": oracle,
            "jpeg_validation": dict(decoder),
        }
    except Exception as exc:
        run_error = exc
        if repository_launch_attempted:
            outcome = {
                "status": "INCONCLUSIVE",
                "run_id": plan["run_id"],
                "workload_manifest_sha256": plan["workload_manifest_sha256"],
                "repository_launch_at": repository_start,
                "repository_deadline_at": deadline,
                "rate_window_start": rate_window_start,
                "completion_at": None,
                "attempt_end_at": float(clock()),
                "reason": f"{type(exc).__name__}: {exc}",
            }
    finally:
        for handle in reversed(handles):
            try:
                stop_result = adapter.stop_owned_role(handle)
                if isinstance(stop_result, Mapping):
                    teardown.append(dict(stop_result))
                else:
                    teardown.append({"status": "UNKNOWN", "reason": "missing teardown result"})
            except Exception as exc:
                teardown.append({"status": "BLOCKED", "reason": f"{type(exc).__name__}: {exc}"})

    if repository_launch_attempted:
        if outcome is None:
            outcome = {
                "status": "INCONCLUSIVE",
                "run_id": plan["run_id"],
                "reason": "controller ended without a classified outcome",
                "attempt_end_at": float(clock()),
            }
        outcome["teardown"] = teardown
        outcome["teardown_status"] = (
            "PASS"
            if len(teardown) == len(handles)
            and all(item.get("status") == "STOPPED" and item.get("evidence") for item in teardown)
            else "BLOCKED"
        )
        record = adapter.build_spec027_record(plan, outcome)
        validate_spec027_record(record)
        if record["run_identity"]["run_id"] != plan["run_id"]:
            raise RunBlocked("SPEC-027 record run_id does not match the executed run")
        record_path = write_spec027_record_once(record_root, record)
        outcome["run_record_path"] = str(record_path)

    if run_error is not None:
        if isinstance(run_error, RunBlocked):
            raise run_error
        raise RunBlocked(f"controller adapter failed: {type(run_error).__name__}: {run_error}") from run_error
    if outcome is None:
        raise RunBlocked("controller produced no run outcome")
    return outcome


def write_spec027_record_once(root: str | os.PathLike[str], record: Mapping[str, Any]) -> Path:
    """Persist one versioned record without overwriting an existing run ID."""
    validate_spec027_record(record)
    record_root = Path(root)
    if record_root.is_symlink() or not record_root.is_dir():
        raise PlanError("record root must be an existing non-symlink directory")
    run_id = record["run_identity"]["run_id"]
    target = record_root / f"{run_id}.spec027-run-record.v1.json"
    payload = (json.dumps(record, sort_keys=True, indent=2) + "\n").encode("utf-8")
    flags = os.O_WRONLY | os.O_CREAT | os.O_EXCL | getattr(os, "O_NOFOLLOW", 0)
    try:
        fd = os.open(target, flags, 0o600)
    except FileExistsError as exc:
        raise PlanError(f"run record already exists for run_id {run_id}") from exc
    except OSError as exc:
        if exc.errno == errno.ELOOP:
            raise PlanError("record target must not be a symlink") from exc
        raise PlanError(f"cannot create run record: {exc}") from exc
    try:
        with os.fdopen(fd, "wb", closefd=True) as stream:
            stream.write(payload)
            stream.flush()
            os.fsync(stream.fileno())
    except OSError as exc:
        raise PlanError(f"cannot durably write run record: {exc}") from exc
    return target


def _load_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise PlanError(f"cannot read JSON file {path}: {exc}") from exc


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="action", required=True)
    plan_parser = subparsers.add_parser("validate-plan", help="validate the fixed AIOPT5 profile invariants")
    plan_parser.add_argument("plan", type=Path)
    snapshot_parser = subparsers.add_parser("verify-snapshot", help="compare a captured payload root to its manifest")
    snapshot_parser.add_argument("root", type=Path)
    snapshot_parser.add_argument("manifest", type=Path, help="JSON list of {path,size,sha256} entries")
    snapshot_parser.add_argument("--expected-count", type=int, default=1000)
    snapshot_parser.add_argument("--max-total-bytes", type=int, required=True)
    record_parser = subparsers.add_parser("validate-record", help="check §8.5 identity/timestamp envelope")
    record_parser.add_argument("record", type=Path)
    args = parser.parse_args(argv)
    try:
        if args.action == "validate-plan":
            validate_plan(_load_json(args.plan))
            print("PLAN_VALID")
            return 0
        if args.action == "verify-snapshot":
            result = verify_exact_snapshot(
                args.root,
                _load_json(args.manifest),
                expected_count=args.expected_count,
                max_total_bytes=args.max_total_bytes,
            )
            print(json.dumps(result, sort_keys=True))
            return 0 if result["status"] == "MATCH" else 1
        validate_spec027_record(_load_json(args.record))
        print("RECORD_ENVELOPE_VALID")
        return 0
    except (PlanError, SnapshotError) as exc:
        print(f"BLOCKED: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
