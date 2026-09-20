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


def _normalized_absolute_path(path: str | Path, label: str) -> str:
    raw = os.fspath(path)
    if not isinstance(raw, str) or "\x00" in raw or not raw.startswith("/") or os.path.normpath(raw) != raw:
        raise PayloadContractError(f"{label} must be an absolute, normalized path")
    return raw


def _deadline_expired(deadline: float | None) -> bool:
    return deadline is not None and time.monotonic() >= deadline


def _check_deadline(deadline: float | None, label: str) -> None:
    if _deadline_expired(deadline):
        raise TimeoutError(f"deadline-expired:{label}")


def _open_absolute_directory(path: str | Path, *, create: bool, deadline: float | None) -> int:
    """Open every directory component with O_NOFOLLOW and retain its fd."""
    raw = _normalized_absolute_path(path, "directory")
    directory_flags = os.O_RDONLY | getattr(os, "O_DIRECTORY", 0) | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0)
    current = os.open("/", directory_flags)
    try:
        for component in raw.split("/")[1:]:
            _check_deadline(deadline, f"open-directory:{component}")
            try:
                child = os.open(component, directory_flags, dir_fd=current)
            except FileNotFoundError:
                if not create:
                    raise
                os.mkdir(component, 0o700, dir_fd=current)
                _check_deadline(deadline, f"open-directory:{component}")
                child = os.open(component, directory_flags, dir_fd=current)
            os.close(current)
            current = child
        return current
    except TimeoutError:
        os.close(current)
        raise
    except OSError as exc:
        os.close(current)
        raise PayloadContractError(f"unsafe directory boundary {raw}: {exc}") from exc


def _open_parent_directory(path: str | Path, *, create: bool, deadline: float | None) -> tuple[int, str]:
    raw = _normalized_absolute_path(path, "file")
    file_name = Path(raw).name
    if not file_name or file_name in {".", ".."}:
        raise PayloadContractError("file destination has an unsafe basename")
    parent = str(Path(raw).parent)
    return _open_absolute_directory(parent, create=create, deadline=deadline), file_name


def _read_bounded_file(path: Path, budget: int, deadline: float | None) -> tuple[bytes, str | None, int]:
    if budget <= 0:
        return b"", "byte-bound-exceeded", 0
    chunks: list[bytes] = []
    consumed = 0
    try:
        _check_deadline(deadline, "open-source")
        flags = os.O_RDONLY | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0) | getattr(os, "O_NONBLOCK", 0)
        fd = os.open(_normalized_absolute_path(path, "source file"), flags)
    except TimeoutError:
        return b"", "deadline-expired", 0
    except (OSError, PayloadContractError) as exc:
        return b"", f"read-failed:{type(exc).__name__}", 0
    try:
        _check_deadline(deadline, "stat-source")
        before = os.fstat(fd)
        if not stat.S_ISREG(before.st_mode):
            return b"", "unsafe-source-entry", 0
        if before.st_size > budget:
            return b"", "byte-bound-exceeded", 0
        while True:
            _check_deadline(deadline, "read-source")
            if consumed >= budget:
                current = os.fstat(fd)
                if current.st_size > consumed:
                    return b"".join(chunks), "byte-bound-exceeded", consumed
                break
            chunk = os.read(fd, min(65536, budget - consumed))
            if not chunk:
                break
            chunks.append(chunk)
            consumed += len(chunk)
            if consumed > budget:
                return b"".join(chunks), "byte-bound-exceeded", consumed
        _check_deadline(deadline, "restat-source")
        after = os.fstat(fd)
        before_identity = (before.st_dev, before.st_ino, before.st_size, before.st_mtime_ns)
        after_identity = (after.st_dev, after.st_ino, after.st_size, after.st_mtime_ns)
        if before_identity != after_identity or consumed != after.st_size:
            return b"".join(chunks), "read-race", consumed
        return b"".join(chunks), None, consumed
    except TimeoutError:
        return b"".join(chunks), "deadline-expired", consumed
    except OSError as exc:
        return b"".join(chunks), f"read-failed:{type(exc).__name__}", consumed
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
    root_path = Path(_normalized_absolute_path(root, "payload root"))
    errors: list[str] = []
    payload_map: dict[str, dict[str, Any]] = {}
    total_bytes = 0
    try:
        _check_deadline(deadline, "stat-root")
        root_stat = root_path.lstat()
    except TimeoutError:
        return {"status": "INCOMPLETE", "map": {}, "identity": None, "bytes": 0, "errors": ["deadline-expired"]}
    except OSError as exc:
        return {
            "status": "INCOMPLETE",
            "map": {},
            "identity": None,
            "bytes": 0,
            "errors": [f"root-unavailable:{type(exc).__name__}"],
        }
    if stat.S_ISLNK(root_stat.st_mode) or not stat.S_ISDIR(root_stat.st_mode):
        return {
            "status": "INCOMPLETE",
            "map": {},
            "identity": None,
            "bytes": 0,
            "errors": ["root-is-not-exclusive-directory"],
        }

    if _deadline_expired(deadline):
        return {"status": "INCOMPLETE", "map": {}, "identity": {"st_dev": root_stat.st_dev, "st_ino": root_stat.st_ino}, "bytes": 0, "errors": ["deadline-expired"]}
    children: list[Path] = []
    try:
        with os.scandir(root_path) as iterator:
            while True:
                _check_deadline(deadline, "enumerate-root")
                try:
                    entry = next(iterator)
                except StopIteration:
                    break
                children.append(Path(entry.path))
                if len(children) > max_files:
                    errors.append("file-count-bound-exceeded")
                    break
    except TimeoutError:
        errors.append("deadline-expired")
    except OSError as exc:
        errors.append(f"enumeration-failed:{type(exc).__name__}")
    try:
        root_after_enumeration = root_path.lstat()
        if (root_stat.st_dev, root_stat.st_ino, root_stat.st_mode) != (root_after_enumeration.st_dev, root_after_enumeration.st_ino, root_after_enumeration.st_mode):
            errors.append("root-identity-changed")
    except OSError as exc:
        errors.append(f"root-restat-failed:{type(exc).__name__}")
    children.sort(key=lambda item: item.name)
    for child in children[:max_files]:
        if _deadline_expired(deadline):
            errors.append("deadline-expired")
            break
        try:
            child_stat = child.lstat()
        except OSError as exc:
            errors.append(f"stat-failed:{child.name}:{type(exc).__name__}")
            continue
        if stat.S_ISLNK(child_stat.st_mode) or not stat.S_ISREG(child_stat.st_mode) or not _NAME_RE.fullmatch(child.name):
            errors.append(f"unsafe-or-unexpected-entry:{child.name}")
            continue
        remaining = max_bytes - total_bytes
        data, error, consumed = _read_bounded_file(child, remaining, deadline)
        total_bytes += consumed
        if error is not None:
            errors.append(f"{error}:{child.name}" if error not in {"byte-bound-exceeded", "deadline-expired", "read-race"} else error)
            if error in {"byte-bound-exceeded", "deadline-expired"}:
                break
            continue
        if not data:
            errors.append(f"invalid-observed-size:{child.name}")
            continue
        payload_map[child.name] = {"size": len(data), "sha256": hashlib.sha256(data).hexdigest()}
    try:
        root_after_reads = root_path.lstat()
        if (root_stat.st_dev, root_stat.st_ino, root_stat.st_mode) != (root_after_reads.st_dev, root_after_reads.st_ino, root_after_reads.st_mode):
            errors.append("root-identity-changed")
    except OSError as exc:
        errors.append(f"root-restat-failed:{type(exc).__name__}")
    return {
        "status": "COMPLETE" if not errors else "INCOMPLETE",
        "map": dict(sorted(payload_map.items())),
        "identity": {"st_dev": root_stat.st_dev, "st_ino": root_stat.st_ino},
        "bytes": total_bytes,
        "errors": sorted(set(errors)),
    }


def _write_json(path: Path, value: Mapping[str, Any], *, deadline: float | None = None) -> None:
    """Write one new JSON object without following stale path components."""
    _check_deadline(deadline, "serialize-json")
    data = (json.dumps(value, ensure_ascii=False, sort_keys=True, indent=2) + "\n").encode("utf-8")
    parent_fd, file_name = _open_parent_directory(path, create=True, deadline=deadline)
    file_fd: int | None = None
    created = False
    success = False
    try:
        _check_deadline(deadline, "open-json")
        flags = os.O_WRONLY | os.O_CREAT | os.O_EXCL | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0)
        try:
            file_fd = os.open(file_name, flags, 0o600, dir_fd=parent_fd)
            created = True
        except FileExistsError as exc:
            raise PayloadContractError(f"refusing stale or symlink destination: {path}") from exc
        try:
            written = 0
            while written < len(data):
                _check_deadline(deadline, "write-json")
                written += os.write(file_fd, data[written:])
            _check_deadline(deadline, "fsync-json")
            os.fsync(file_fd)
        except TimeoutError:
            raise
        finally:
            os.close(file_fd)
            file_fd = None
        _check_deadline(deadline, "fsync-json-directory")
        os.fsync(parent_fd)
        success = True
    except TimeoutError:
        raise
    except PayloadContractError:
        raise
    except OSError as exc:
        raise PayloadContractError(f"unsafe JSON destination {path}: {exc}") from exc
    finally:
        if file_fd is not None:
            os.close(file_fd)
        if created and not success:
            try:
                os.unlink(file_name, dir_fd=parent_fd)
            except OSError:
                pass
        os.close(parent_fd)


def _ensure_directory_tree_safe(path: Path, *, deadline: float | None = None) -> None:
    fd = _open_absolute_directory(path, create=True, deadline=deadline)
    os.close(fd)


def _copy_bounded_file(source: Path, destination: Path, *, budget: int, deadline: float | None) -> tuple[bool, str | None, int]:
    data, error, _consumed = _read_bounded_file(source, budget, deadline)
    if error is not None:
        return False, error, 0
    parent_fd: int | None = None
    file_fd: int | None = None
    file_name: str | None = None
    created = False
    success = False
    try:
        parent_fd, file_name = _open_parent_directory(destination, create=True, deadline=deadline)
        _check_deadline(deadline, "open-preserved-file")
        flags = os.O_WRONLY | os.O_CREAT | os.O_EXCL | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0)
        try:
            file_fd = os.open(file_name, flags, 0o600, dir_fd=parent_fd)
            created = True
        except FileExistsError:
            return False, "unsafe-or-stale-destination", 0
        written = 0
        while written < len(data):
            _check_deadline(deadline, "write-preserved-file")
            written += os.write(file_fd, data[written:])
        _check_deadline(deadline, "fsync-preserved-file")
        os.fsync(file_fd)
        os.close(file_fd)
        file_fd = None
        _check_deadline(deadline, "fsync-preserved-directory")
        os.fsync(parent_fd)
        success = True
        return True, None, len(data)
    except TimeoutError:
        return False, "deadline-expired", 0
    except PayloadContractError:
        return False, "unsafe-or-stale-destination", 0
    except OSError as exc:
        return False, f"copy-failed:{type(exc).__name__}", 0
    finally:
        if file_fd is not None:
            os.close(file_fd)
        if created and not success and parent_fd is not None and file_name is not None:
            try:
                os.unlink(file_name, dir_fd=parent_fd)
            except OSError:
                pass
        if parent_fd is not None:
            os.close(parent_fd)


def preserve_payload_origins(
    bundle: str | Path,
    roots: Mapping[str, str | Path],
    *,
    expected: Mapping[str, Any],
    max_files: int,
    max_bytes: int,
    max_bytes_total: int | None = None,
    timeout_s: float | None = None,
) -> dict[str, Any]:
    """Preserve three fake/fixture origins under the SPEC-060 canonical layout."""
    if set(roots) != set(_ORIGIN_NAMES):
        raise PayloadContractError("payload origins must be exactly source, nrncs, repository")
    expected = _validate_map_records(expected, "expected payload map", allow_empty=False)
    if max_bytes_total is None:
        max_bytes_total = 3 * max_bytes
    if isinstance(max_bytes_total, bool) or not isinstance(max_bytes_total, int) or max_bytes_total <= 0:
        raise PayloadContractError("max_bytes_total must be a positive integer")
    if timeout_s is not None and (isinstance(timeout_s, bool) or not isinstance(timeout_s, (int, float)) or timeout_s < 0 or not math.isfinite(timeout_s)):
        raise PayloadContractError("timeout_s must be a finite non-negative number")
    deadline = None if timeout_s is None else time.monotonic() + float(timeout_s)
    bundle_path = Path(_normalized_absolute_path(bundle, "payload bundle"))
    payload_root = bundle_path / "payload"
    origins_root = payload_root / "origins"
    maps_root = payload_root / "maps"
    metadata_root = payload_root / "metadata"
    for directory in (origins_root, maps_root, metadata_root):
        _ensure_directory_tree_safe(directory, deadline=deadline)
    _write_json(payload_root / "expected.json", {"count": len(expected), "hash": "sha256", "map": dict(sorted(expected.items()))}, deadline=deadline)

    origin_results: dict[str, dict[str, Any]] = {}
    aggregate_bytes = 0
    expected_bytes = sum(int(record["size"]) for record in expected.values())
    for name in _ORIGIN_NAMES:
        source_root = Path(roots[name])
        remaining_timeout = None if deadline is None else max(0.0, deadline - time.monotonic())
        remaining_total = max_bytes_total - aggregate_bytes
        inventory_budget = min(max_bytes, max(1, remaining_total))
        source_result = inventory_payload_origin(source_root, max_files=max_files, max_bytes=inventory_budget, timeout_s=remaining_timeout)
        comparison = compare_payload_maps(expected, source_result["map"])
        destination = origins_root / name / "files"
        _ensure_directory_tree_safe(destination, deadline=deadline)
        errors = list(source_result["errors"])
        source_bytes = int(source_result.get("bytes", 0))
        if aggregate_bytes + expected_bytes > max_bytes_total:
            errors.append("aggregate-byte-bound-exceeded")
        aggregate_bytes += source_bytes
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
        _write_json(maps_root / f"{name}.json", preserved_result["map"], deadline=deadline)
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
            deadline=deadline,
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
        "aggregate_bytes": aggregate_bytes,
        "max_bytes_total": max_bytes_total,
        "origins": {name: result["status"] for name, result in origin_results.items()},
    }
    checkpoint_path = payload_root / "preservation-checkpoint.json"
    _write_json(checkpoint_path, checkpoint, deadline=deadline)
    return {
        "status": "COMPLETE" if complete else "INCOMPLETE",
        "accepted": accepted,
        "aggregate_bytes": aggregate_bytes,
        "max_bytes_total": max_bytes_total,
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
