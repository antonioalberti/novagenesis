"""Closed, controller-only validation for SPEC-060 payload evidence plans."""
from __future__ import annotations

import copy
import hashlib
import json
import math
import os
import re
import stat
import time
from collections.abc import Mapping
from pathlib import Path
from pathlib import PurePosixPath
from typing import Any


class PayloadContractError(ValueError):
    """Raised when a normal payload-evidence contract is not fail-closed."""


_SCHEMA_VERSION = 1
_ACCEPTANCE_PROFILE = "normal-payload"
_ORIGIN_NAMES = ("source", "nrncs", "repository")
_EXPECTED_KEYS = frozenset(("count", "pattern", "hash", "map"))
_ORIGIN_KEYS = frozenset(("host", "role", "path", "service_config_ref"))
_BOUNDS_KEYS = frozenset(
    (
        "global_timeout_s",
        "per_origin_timeout_s",
        "max_files_per_origin",
        "max_bytes_per_origin",
        "max_bytes_total",
    )
)
_OWNERSHIP_KEYS = frozenset(("trial_id", "owner", "cleanup_scope"))
_NRNCS_BINDING_KEYS = frozenset(("role", "service_config_ref", "observation_ref"))
_PAYLOAD_KEYS = frozenset(("schema_version", "expected", "origins", "bounds", "ownership", "nrncs_binding"))
_NAME_RE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_.-]*\.jpg$")
_DIGEST_RE = re.compile(r"^[0-9a-f]{64}$")
_SAFE_TEXT_RE = re.compile(r"^[A-Za-z0-9_.:-]+$")


def _reject_duplicate_pairs(pairs: list[tuple[str, Any]]) -> dict[str, Any]:
    result: dict[str, Any] = {}
    for key, value in pairs:
        if key in result:
            raise PayloadContractError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def loads_rejecting_duplicate_keys(text: str) -> Any:
    """Load JSON without silently accepting duplicate object keys."""
    return json.loads(text, object_pairs_hook=_reject_duplicate_pairs)


def _object(value: Any, label: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise PayloadContractError(f"{label} must be an object")
    return value


def _exact_keys(value: Mapping[str, Any], expected: frozenset[str], label: str) -> None:
    actual = set(value)
    missing = sorted(expected - actual)
    extra = sorted(actual - expected)
    if missing or extra:
        detail = []
        if missing:
            detail.append("missing=" + ",".join(missing))
        if extra:
            detail.append("unknown=" + ",".join(extra))
        raise PayloadContractError(f"{label} keys invalid ({'; '.join(detail)})")


def _text(value: Any, label: str, *, pattern: re.Pattern[str] | None = None) -> str:
    if not isinstance(value, str) or not value or "\x00" in value:
        raise PayloadContractError(f"{label} must be non-empty text")
    if pattern is not None and not pattern.fullmatch(value):
        raise PayloadContractError(f"{label} has unsafe format")
    return value


def _positive_int(value: Any, label: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or value <= 0:
        raise PayloadContractError(f"{label} must be a positive integer")
    return value


def _absolute_path(value: Any, label: str) -> str:
    path = _text(value, label)
    raw_parts = path.split("/")
    if not path.startswith("/") or any(part in {"", ".", ".."} for part in raw_parts[1:]):
        raise PayloadContractError(f"{label} must be an absolute, normalized path")
    parsed = PurePosixPath(path)
    if not parsed.is_absolute() or parsed.as_posix() != path:
        raise PayloadContractError(f"{label} must be an absolute, normalized path")
    return path


def _validate_payload_block(block: Any) -> dict[str, Any]:
    payload = _object(block, "payload_evidence")
    _exact_keys(payload, _PAYLOAD_KEYS, "payload_evidence")
    if isinstance(payload["schema_version"], bool) or not isinstance(payload["schema_version"], int) or payload["schema_version"] != _SCHEMA_VERSION:
        raise PayloadContractError("payload_evidence.schema_version must be 1")

    expected = _object(payload["expected"], "payload_evidence.expected")
    _exact_keys(expected, _EXPECTED_KEYS, "payload_evidence.expected")
    if isinstance(expected["count"], bool) or not isinstance(expected["count"], int) or expected["count"] != 5:
        raise PayloadContractError("payload_evidence.expected.count must be 5")
    if expected["pattern"] != "*.jpg":
        raise PayloadContractError("payload_evidence.expected.pattern must be *.jpg")
    if expected["hash"] != "sha256":
        raise PayloadContractError("payload_evidence.expected.hash must be sha256")
    expected_map = _object(expected["map"], "payload_evidence.expected.map")
    if len(expected_map) != expected["count"]:
        raise PayloadContractError("payload_evidence.expected.map cardinality must equal count")
    normalized_map: dict[str, dict[str, Any]] = {}
    for name, item in sorted(expected_map.items()):
        if not isinstance(name, str) or not _NAME_RE.fullmatch(name) or "/" in name or "\\" in name:
            raise PayloadContractError("payload_evidence.expected.map contains an unsafe filename")
        record = _object(item, f"payload_evidence.expected.map[{name}]")
        _exact_keys(record, frozenset(("size", "sha256")), f"payload_evidence.expected.map[{name}]")
        size = _positive_int(record["size"], f"payload_evidence.expected.map[{name}].size")
        digest = _text(record["sha256"], f"payload_evidence.expected.map[{name}].sha256", pattern=_DIGEST_RE)
        normalized_map[name] = {"size": size, "sha256": digest}

    origins = _object(payload["origins"], "payload_evidence.origins")
    _exact_keys(origins, frozenset(_ORIGIN_NAMES), "payload_evidence.origins")
    normalized_origins: dict[str, dict[str, str]] = {}
    for name in _ORIGIN_NAMES:
        origin = _object(origins[name], f"payload_evidence.origins.{name}")
        _exact_keys(origin, _ORIGIN_KEYS, f"payload_evidence.origins.{name}")
        host = _text(origin["host"], f"payload_evidence.origins.{name}.host", pattern=_SAFE_TEXT_RE)
        role = _text(origin["role"], f"payload_evidence.origins.{name}.role", pattern=_SAFE_TEXT_RE)
        if role != name:
            raise PayloadContractError(f"payload_evidence.origins.{name}.role must be {name}")
        normalized_origins[name] = {
            "host": host,
            "role": role,
            "path": _absolute_path(origin["path"], f"payload_evidence.origins.{name}.path"),
            "service_config_ref": _text(origin["service_config_ref"], f"payload_evidence.origins.{name}.service_config_ref", pattern=_SAFE_TEXT_RE),
        }

    bounds = _object(payload["bounds"], "payload_evidence.bounds")
    _exact_keys(bounds, _BOUNDS_KEYS, "payload_evidence.bounds")
    normalized_bounds = {key: _positive_int(bounds[key], f"payload_evidence.bounds.{key}") for key in sorted(_BOUNDS_KEYS)}
    if normalized_bounds["global_timeout_s"] < normalized_bounds["per_origin_timeout_s"]:
        raise PayloadContractError("global timeout must cover per-origin timeout")
    if normalized_bounds["max_files_per_origin"] < expected["count"]:
        raise PayloadContractError("max_files_per_origin must cover expected count")
    if normalized_bounds["max_bytes_total"] < 3 * normalized_bounds["max_bytes_per_origin"]:
        raise PayloadContractError("max_bytes_total must cover three origins")

    ownership = _object(payload["ownership"], "payload_evidence.ownership")
    _exact_keys(ownership, _OWNERSHIP_KEYS, "payload_evidence.ownership")
    normalized_ownership = {
        "trial_id": _text(ownership["trial_id"], "payload_evidence.ownership.trial_id", pattern=_SAFE_TEXT_RE),
        "owner": _text(ownership["owner"], "payload_evidence.ownership.owner", pattern=_SAFE_TEXT_RE),
        "cleanup_scope": _text(ownership["cleanup_scope"], "payload_evidence.ownership.cleanup_scope", pattern=_SAFE_TEXT_RE),
    }
    if normalized_ownership["cleanup_scope"] != "declared-roots-only":
        raise PayloadContractError("cleanup_scope must be declared-roots-only")

    binding = _object(payload["nrncs_binding"], "payload_evidence.nrncs_binding")
    _exact_keys(binding, _NRNCS_BINDING_KEYS, "payload_evidence.nrncs_binding")
    if binding["role"] != "nrncs":
        raise PayloadContractError("nrncs_binding.role must be nrncs")
    normalized_binding = {
        "role": "nrncs",
        "service_config_ref": _text(binding["service_config_ref"], "payload_evidence.nrncs_binding.service_config_ref", pattern=_SAFE_TEXT_RE),
        "observation_ref": _text(binding["observation_ref"], "payload_evidence.nrncs_binding.observation_ref", pattern=_SAFE_TEXT_RE),
    }
    if normalized_binding["service_config_ref"] != normalized_origins["nrncs"]["service_config_ref"]:
        raise PayloadContractError("nrncs binding does not match the nrncs origin")

    return {
        "schema_version": _SCHEMA_VERSION,
        "expected": {
            "count": 5,
            "pattern": "*.jpg",
            "hash": "sha256",
            "map": normalized_map,
        },
        "origins": normalized_origins,
        "bounds": normalized_bounds,
        "ownership": normalized_ownership,
        "nrncs_binding": normalized_binding,
    }


def validate_payload_plan(plan: Mapping[str, Any]) -> dict[str, Any]:
    """Validate and normalize a normal-payload SPEC-060 plan fragment."""
    if not isinstance(plan, Mapping):
        raise PayloadContractError("plan must be an object")
    if plan.get("diagnostic_only") is not False:
        raise PayloadContractError("normal payload plan requires diagnostic_only=false")
    if plan.get("acceptance_profile") != _ACCEPTANCE_PROFILE:
        raise PayloadContractError("normal payload plan requires acceptance_profile=normal-payload")
    normalized = _validate_payload_block(plan.get("payload_evidence"))
    return copy.deepcopy(normalized | {"acceptance_profile": _ACCEPTANCE_PROFILE})


def payload_acceptance_eligible(plan: Mapping[str, Any]) -> bool:
    """Return whether a plan is explicitly eligible for normal payload acceptance."""
    try:
        validate_payload_plan(plan)
    except (PayloadContractError, TypeError):
        return False
    return True


def compare_payload_maps(expected: Mapping[str, Any], observed: Mapping[str, Any]) -> dict[str, Any]:
    """Compare name/size/digest maps without treating absence as success."""
    expected_map = _validate_map_records(expected, "expected payload map", allow_empty=False)
    observed_map = _validate_map_records(observed, "observed payload map", allow_empty=True)
    expected_names = set(expected_map)
    observed_names = set(observed_map)
    missing = sorted(str(name) for name in expected_names - observed_names)
    extra = sorted(str(name) for name in observed_names - expected_names)
    mismatched = sorted(
        str(name)
        for name in expected_names & observed_names
        if expected_map[name] != observed_map[name]
    )
    accepted = not missing and not extra and not mismatched
    return {
        "status": "PASS" if accepted else "FAIL",
        "missing": missing,
        "extra": extra,
        "mismatched": mismatched,
        "accepted": accepted,
    }


def _validate_map_records(value: Mapping[str, Any], label: str, *, allow_empty: bool) -> dict[str, dict[str, int | str]]:
    if not isinstance(value, Mapping):
        raise PayloadContractError(f"{label} must be an object")
    if not value and not allow_empty:
        raise PayloadContractError(f"{label} must not be empty")
    normalized: dict[str, dict[str, int | str]] = {}
    for name, record in value.items():
        if not isinstance(name, str) or not _NAME_RE.fullmatch(name) or "/" in name or "\\" in name:
            raise PayloadContractError(f"{label} contains an unsafe filename")
        item = _object(record, f"{label}[{name}]")
        _exact_keys(item, frozenset(("size", "sha256")), f"{label}[{name}]")
        size = _positive_int(item["size"], f"{label}[{name}].size")
        digest = _text(item["sha256"], f"{label}[{name}].sha256", pattern=_DIGEST_RE)
        normalized[name] = {"size": size, "sha256": digest}
    return dict(sorted(normalized.items()))


def _read_bounded_file(path: Path, budget: int, deadline: float | None) -> tuple[bytes, str | None]:
    if budget <= 0:
        return b"", "byte-bound-exceeded"
    if deadline is not None and time.monotonic() >= deadline:
        return b"", "deadline-expired"
    flags = os.O_RDONLY | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0)
    try:
        fd = os.open(path, flags)
    except OSError as exc:
        return b"", f"read-failed:{type(exc).__name__}"
    try:
        before = os.fstat(fd)
        if not stat.S_ISREG(before.st_mode):
            return b"", "unsafe-source-entry"
        chunks: list[bytes] = []
        total = 0
        while True:
            if deadline is not None and time.monotonic() >= deadline:
                return b"", "deadline-expired"
            chunk = os.read(fd, min(65536, budget - total + 1))
            if not chunk:
                break
            chunks.append(chunk)
            total += len(chunk)
            if total > budget:
                return b"", "byte-bound-exceeded"
        after = os.fstat(fd)
        if (before.st_dev, before.st_ino, before.st_size, before.st_mtime_ns) != (after.st_dev, after.st_ino, after.st_size, after.st_mtime_ns) or total != after.st_size:
            return b"", "read-race"
        return b"".join(chunks), None
    except OSError as exc:
        return b"", f"read-failed:{type(exc).__name__}"
    finally:
        os.close(fd)


def inventory_payload_origin(root: str | Path, *, max_files: int, max_bytes: int, timeout_s: float | None = None) -> dict[str, Any]:
    """Hash one exclusive payload root with bounded, fail-closed reads."""
    if isinstance(max_files, bool) or not isinstance(max_files, int) or max_files <= 0:
        raise PayloadContractError("max_files must be a positive integer")
    if isinstance(max_bytes, bool) or not isinstance(max_bytes, int) or max_bytes <= 0:
        raise PayloadContractError("max_bytes must be a positive integer")
    if timeout_s is not None and (isinstance(timeout_s, bool) or not isinstance(timeout_s, (int, float)) or timeout_s < 0 or not math.isfinite(timeout_s)):
        raise PayloadContractError("timeout_s must be a finite non-negative number")
    deadline = None if timeout_s is None else time.monotonic() + float(timeout_s)
    root_path = Path(root)
    errors: list[str] = []
    payload_map: dict[str, dict[str, Any]] = {}
    total_bytes = 0
    try:
        root_stat = root_path.lstat()
    except OSError as exc:
        return {
            "status": "INCOMPLETE",
            "map": {},
            "identity": None,
            "bytes": 0,
            "errors": [f"root-unavailable:{type(exc).__name__}"],
        }
    if root_path.is_symlink() or not root_path.is_dir():
        return {
            "status": "INCOMPLETE",
            "map": {},
            "identity": None,
            "bytes": 0,
            "errors": ["root-is-not-exclusive-directory"],
        }

    children: list[Path] = []
    try:
        with os.scandir(root_path) as iterator:
            for entry in iterator:
                children.append(Path(entry.path))
                if len(children) > max_files:
                    errors.append("file-count-bound-exceeded")
                    break
    except OSError as exc:
        errors.append(f"enumeration-failed:{type(exc).__name__}")
    children.sort(key=lambda item: item.name)
    for child in children[:max_files]:
        if deadline is not None and time.monotonic() >= deadline:
            errors.append("deadline-expired")
            break
        if child.is_symlink() or not child.is_file() or not _NAME_RE.fullmatch(child.name):
            errors.append(f"unsafe-or-unexpected-entry:{child.name}")
            continue
        remaining = max_bytes - total_bytes
        data, error = _read_bounded_file(child, remaining, deadline)
        if error is not None:
            errors.append(f"{error}:{child.name}" if error not in {"byte-bound-exceeded", "deadline-expired", "read-race"} else error)
            if error in {"byte-bound-exceeded", "deadline-expired"}:
                total_bytes = max_bytes if error == "byte-bound-exceeded" else total_bytes
                break
            continue
        total_bytes += len(data)
        payload_map[child.name] = {"size": len(data), "sha256": hashlib.sha256(data).hexdigest()}
    return {
        "status": "COMPLETE" if not errors else "INCOMPLETE",
        "map": dict(sorted(payload_map.items())),
        "identity": {"st_dev": root_stat.st_dev, "st_ino": root_stat.st_ino},
        "bytes": total_bytes,
        "errors": sorted(set(errors)),
    }


def _write_json(path: Path, value: Mapping[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, sort_keys=True, indent=2) + "\n", encoding="utf-8")


def _ensure_directory_tree_safe(path: Path) -> None:
    current = path
    while True:
        if os.path.lexists(current):
            if current.is_symlink() or not current.is_dir():
                raise PayloadContractError(f"unsafe preservation directory: {current.name}")
        if current == current.parent:
            return
        current = current.parent


def _copy_bounded_file(source: Path, destination: Path, *, budget: int, deadline: float | None) -> tuple[bool, str | None, int]:
    data, error = _read_bounded_file(source, budget, deadline)
    if error is not None:
        return False, error, min(len(data), budget)
    try:
        _ensure_directory_tree_safe(destination.parent)
        if os.path.lexists(destination):
            return False, "unsafe-or-stale-destination", 0
        flags = os.O_WRONLY | os.O_CREAT | os.O_EXCL | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0)
        fd = os.open(destination, flags, 0o600)
        try:
            written = 0
            while written < len(data):
                if deadline is not None and time.monotonic() >= deadline:
                    return False, "deadline-expired", written
                written += os.write(fd, data[written:])
            os.fsync(fd)
        finally:
            os.close(fd)
        return True, None, len(data)
    except OSError as exc:
        try:
            if os.path.lexists(destination) and not destination.is_symlink():
                destination.unlink()
        except OSError:
            pass
        return False, f"copy-failed:{type(exc).__name__}", 0


def preserve_payload_origins(
    bundle: str | Path,
    roots: Mapping[str, str | Path],
    *,
    expected: Mapping[str, Any],
    max_files: int,
    max_bytes: int,
    timeout_s: float | None = None,
) -> dict[str, Any]:
    """Preserve three fake/fixture origins under the SPEC-060 canonical layout."""
    if set(roots) != set(_ORIGIN_NAMES):
        raise PayloadContractError("payload origins must be exactly source, nrncs, repository")
    expected = _validate_map_records(expected, "expected payload map", allow_empty=False)
    if timeout_s is not None and (isinstance(timeout_s, bool) or not isinstance(timeout_s, (int, float)) or timeout_s < 0 or not math.isfinite(timeout_s)):
        raise PayloadContractError("timeout_s must be a finite non-negative number")
    deadline = None if timeout_s is None else time.monotonic() + float(timeout_s)
    bundle_path = Path(bundle)
    payload_root = bundle_path / "payload"
    origins_root = payload_root / "origins"
    maps_root = payload_root / "maps"
    metadata_root = payload_root / "metadata"
    for directory in (origins_root, maps_root, metadata_root):
        _ensure_directory_tree_safe(directory)
        directory.mkdir(parents=True, exist_ok=True)
    _write_json(payload_root / "expected.json", {"count": len(expected), "hash": "sha256", "map": dict(sorted(expected.items()))})

    origin_results: dict[str, dict[str, Any]] = {}
    for name in _ORIGIN_NAMES:
        source_root = Path(roots[name])
        remaining_timeout = None if deadline is None else max(0.0, deadline - time.monotonic())
        source_result = inventory_payload_origin(source_root, max_files=max_files, max_bytes=max_bytes, timeout_s=remaining_timeout)
        comparison = compare_payload_maps(expected, source_result["map"])
        destination = origins_root / name / "files"
        _ensure_directory_tree_safe(destination)
        destination.mkdir(parents=True, exist_ok=True)
        errors = list(source_result["errors"])
        copied_bytes = 0
        for filename in sorted(source_result["map"]):
            source_file = source_root / filename
            destination_file = destination / filename
            copied, copy_error, written = _copy_bounded_file(source_file, destination_file, budget=max_bytes - copied_bytes, deadline=deadline)
            copied_bytes += written
            if not copied:
                errors.append(f"preservation-failed:{copy_error}")
                break
        remaining_timeout = None if deadline is None else max(0.0, deadline - time.monotonic())
        preserved_result = inventory_payload_origin(destination, max_files=max_files, max_bytes=max_bytes, timeout_s=remaining_timeout)
        preserved_comparison = compare_payload_maps(expected, preserved_result["map"])
        if preserved_result["status"] != "COMPLETE":
            errors.extend(preserved_result["errors"])
        complete = not errors and source_result["status"] == "COMPLETE" and preserved_result["status"] == "COMPLETE"
        status = "COMPLETE" if complete else "INCOMPLETE"
        _write_json(maps_root / f"{name}.json", preserved_result["map"])
        _write_json(
            metadata_root / f"{name}.json",
            {
                "origin": name,
                "source_root": str(source_root),
                "source_identity": source_result["identity"],
                "source_status": source_result["status"],
                "preserved_status": preserved_result["status"],
                "comparison": comparison,
                "preserved_comparison": preserved_comparison,
                "errors": sorted(set(errors)),
                "status": status,
            },
        )
        origin_results[name] = {
            "status": status,
            "source": source_result,
            "preserved": preserved_result,
            "comparison": comparison,
            "preserved_comparison": preserved_comparison,
            "errors": sorted(set(errors)),
        }

    accepted = all(
        result["status"] == "COMPLETE"
        and result["comparison"]["accepted"]
        and result["preserved_comparison"]["accepted"]
        for result in origin_results.values()
    )
    complete = all(result["status"] == "COMPLETE" for result in origin_results.values())
    checkpoint = {
        "schema_version": 1,
        "kind": "payload-preservation-checkpoint",
        "complete": complete,
        "origins": {name: result["status"] for name, result in origin_results.items()},
    }
    checkpoint_path = payload_root / "preservation-checkpoint.json"
    _write_json(checkpoint_path, checkpoint)
    return {
        "status": "COMPLETE" if complete else "INCOMPLETE",
        "accepted": accepted,
        "origins": origin_results,
        "checkpoint": str(checkpoint_path.relative_to(bundle_path)),
    }


def evaluate_payload_result(origins: Mapping[str, Mapping[str, Any]], *, prior_runtime: str | None = None) -> dict[str, Any]:
    """Apply the SPEC-060 runtime/evidence/acceptance precedence matrix."""
    if set(origins) != set(_ORIGIN_NAMES):
        return {
            "runtime": "FAIL" if prior_runtime == "FAIL" else "INCONCLUSIVE",
            "evidence": "INCOMPLETE",
            "accepted": False,
        }
    incomplete = False
    divergent = False
    for name in _ORIGIN_NAMES:
        item = origins[name]
        if item.get("status") != "COMPLETE":
            incomplete = True
        comparison = item.get("comparison")
        if not isinstance(comparison, Mapping) or comparison.get("accepted") is not True:
            divergent = True
    if prior_runtime == "FAIL":
        return {"runtime": "FAIL", "evidence": "INCOMPLETE" if incomplete else "COMPLETE", "accepted": False}
    if incomplete:
        return {"runtime": "INCONCLUSIVE", "evidence": "INCOMPLETE", "accepted": False}
    if divergent:
        return {"runtime": "FAIL", "evidence": "COMPLETE", "accepted": False}
    return {"runtime": "PASS", "evidence": "COMPLETE", "accepted": True}
