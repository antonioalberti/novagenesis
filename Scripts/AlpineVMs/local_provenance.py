"""Small, side-effect-limited provenance helpers for local NG-ELC runs.

The module deliberately does not wire itself into the controller.  It provides
serialisable records that a controller can include in an evidence bundle after
review.  Git capture records only the commit id and porcelain status; it never
captures diffs, environment values, or command output other than those fields.
"""

from __future__ import annotations

import hashlib
import os
import subprocess
from collections.abc import Iterable, Mapping, Sequence
from pathlib import Path
from typing import Any


_SHA256_LENGTH = 64


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _require_regular_file(path: Path) -> Path:
    if path.is_symlink():
        raise ValueError(f"provenance input must not be a symlink: {path}")
    if not path.is_file():
        raise FileNotFoundError(path)
    return path


def _normalise_hash(value: Any, field: str) -> str:
    if not isinstance(value, str) or len(value) != _SHA256_LENGTH:
        raise ValueError(f"{field} must be a SHA-256 hex digest")
    result = value.lower()
    if any(character not in "0123456789abcdef" for character in result):
        raise ValueError(f"{field} must be a SHA-256 hex digest")
    return result


def file_identity(path: os.PathLike[str] | str) -> dict[str, Any]:
    """Return the resolved path, byte size, and SHA-256 for one regular file."""

    candidate = _require_regular_file(Path(path))
    resolved = candidate.resolve()
    return {
        "path": str(resolved),
        "size": resolved.stat().st_size,
        "sha256": _sha256(resolved),
    }


def snapshot_selected_files(
    paths: Iterable[os.PathLike[str] | str],
    root: os.PathLike[str] | str | None = None,
) -> list[dict[str, Any]]:
    """Create a deterministic content-addressed snapshot of selected files.

    Each record contains only ``path``, ``size`` and ``sha256``.  When ``root``
    is supplied, paths are recorded relative to that root so the snapshot is
    portable between evidence directories.  Files outside the root and
    duplicate recorded paths are rejected rather than silently normalised.
    """

    root_path = Path(root).resolve() if root is not None else None
    records: list[dict[str, Any]] = []
    seen: set[str] = set()
    for raw_path in paths:
        candidate = _require_regular_file(Path(raw_path))
        resolved = candidate.resolve()
        if root_path is None:
            recorded_path = str(resolved)
        else:
            try:
                recorded_path = resolved.relative_to(root_path).as_posix()
            except ValueError as exc:
                raise ValueError(f"snapshot path is outside root: {resolved}") from exc
        if recorded_path in seen:
            raise ValueError(f"duplicate snapshot path: {recorded_path}")
        seen.add(recorded_path)
        records.append(
            {
                "path": recorded_path,
                "size": resolved.stat().st_size,
                "sha256": _sha256(resolved),
            }
        )
    return sorted(records, key=lambda record: record["path"])


# The shorter name is convenient for callers and keeps the API explicit.
snapshot_files = snapshot_selected_files


def capture_git_state(repo: os.PathLike[str] | str) -> dict[str, Any]:
    """Capture safe local git identity without exposing diffs or secrets."""

    repository = Path(repo).resolve()
    if not repository.is_dir():
        raise FileNotFoundError(repository)

    head_run = subprocess.run(
        ["git", "-C", str(repository), "rev-parse", "HEAD"],
        capture_output=True,
        text=True,
        check=False,
        timeout=10,
    )
    status_run = subprocess.run(
        ["git", "-C", str(repository), "status", "--porcelain=v1", "--untracked-files=all"],
        capture_output=True,
        text=True,
        check=False,
        timeout=10,
    )
    if status_run.returncode != 0:
        raise RuntimeError("git status failed")
    head = head_run.stdout.strip() if head_run.returncode == 0 else None
    status = [line for line in status_run.stdout.splitlines() if line]
    return {"head": head or None, "status": status, "clean": not status}


# Explicit alias for callers that use the R06 terminology.
capture_git_provenance = capture_git_state


def capture_code_identity(
    controller: os.PathLike[str] | str,
    helpers: Mapping[str, os.PathLike[str] | str] | Iterable[os.PathLike[str] | str] = (),
) -> dict[str, Any]:
    """Capture controller and imported-helper file identities."""

    if isinstance(helpers, Mapping):
        helper_records: Any = {
            str(name): file_identity(path)
            for name, path in sorted(helpers.items(), key=lambda item: str(item[0]))
        }
    else:
        helper_records = [file_identity(path) for path in helpers]
        helper_records.sort(key=lambda record: record["path"])
    return {"controller": file_identity(controller), "helpers": helper_records}


# Name used by some provenance callers.
capture_controller_helpers = capture_code_identity


def _snapshot_records(value: Any) -> list[Mapping[str, Any]]:
    if isinstance(value, Mapping):
        if "files" in value:
            value = value.get("files")
        else:
            value = [
                dict(record, path=path) if isinstance(record, Mapping) and "path" not in record else record
                for path, record in value.items()
            ]
    if isinstance(value, (str, bytes)) or not isinstance(value, Sequence):
        raise ValueError("source snapshot must be a non-empty sequence")
    if not value:
        raise ValueError("source snapshot is required")
    records: list[Mapping[str, Any]] = []
    for record in value:
        if not isinstance(record, Mapping):
            raise ValueError("source snapshot entries must be mappings")
        path = record.get("path")
        size = record.get("size")
        if not isinstance(path, str) or not path or not isinstance(size, int) or size < 0:
            raise ValueError("source snapshot entries require path and non-negative size")
        _normalise_hash(record.get("sha256"), "source snapshot sha256")
        records.append(record)
    return records


def _binary_records(value: Any, field: str) -> dict[str, Mapping[str, Any]]:
    if isinstance(value, Mapping):
        if "path" in value or "resolved_path" in value:
            name = str(value.get("name") or value.get("role") or "default")
            return {name: value}
        records: dict[str, Mapping[str, Any]] = {}
        for name, record in value.items():
            if not isinstance(record, Mapping) and not isinstance(record, (str, os.PathLike)):
                raise ValueError(f"{field} entries must be mappings or paths")
            records[str(name)] = record if isinstance(record, Mapping) else {"path": str(record)}
        return records
    if isinstance(value, (str, bytes)) or not isinstance(value, Sequence):
        raise ValueError(f"{field} must be a non-empty mapping or sequence")
    records = {}
    for record in value:
        if not isinstance(record, Mapping):
            raise ValueError(f"{field} sequence entries must be mappings")
        name = record.get("name") or record.get("role")
        if not isinstance(name, str) or not name:
            raise ValueError(f"{field} entries require name or role")
        records[name] = record
    return records


def _binary_identity(name: str, record: Mapping[str, Any], field: str) -> tuple[str, str]:
    raw_path = record.get("path", record.get("resolved_path"))
    if not isinstance(raw_path, (str, os.PathLike)) or not str(raw_path):
        raise ValueError(f"{field} binary {name!r} requires path")
    path = os.path.normpath(str(raw_path))
    raw_hash = record.get("sha256")
    if raw_hash is None:
        candidate = Path(str(raw_path))
        if not candidate.is_file():
            raise ValueError(f"{field} binary {name!r} requires sha256")
        raw_hash = _sha256(candidate)
    return path, _normalise_hash(raw_hash, f"{field} binary {name!r} sha256")


def validate_build_linkage(
    manifest: Mapping[str, Any],
    source_head: str,
    executables: Mapping[str, Any] | Sequence[Mapping[str, Any]],
) -> bool:
    """Validate that a build manifest links the supplied source and binaries.

    A valid manifest must have a non-empty recipe, source snapshot, source
    head, and binary records.  Every manifest binary must match the launched
    executable with the same role, normalised path, and SHA-256.  ``ValueError``
    is raised for every incomplete or divergent linkage; ``True`` is returned
    only for a complete match.
    """

    if not isinstance(manifest, Mapping) or not manifest:
        raise ValueError("manifest is required and must not be empty")

    recipe = manifest.get("recipe", manifest.get("build_recipe"))
    if not recipe:
        raise ValueError("recipe is required")

    snapshot = manifest.get("source_snapshot")
    if snapshot is None and isinstance(manifest.get("source"), Mapping):
        snapshot = manifest["source"].get("snapshot")
    _snapshot_records(snapshot)

    if not isinstance(source_head, str) or not source_head:
        raise ValueError("source_head is required")
    manifest_head = manifest.get("source_head")
    if manifest_head is None and isinstance(manifest.get("source"), Mapping):
        manifest_head = manifest["source"].get("head")
    if not isinstance(manifest_head, str) or not manifest_head:
        raise ValueError("manifest source head is required")
    if manifest_head != source_head:
        raise ValueError("source head divergence")

    expected_value = manifest.get("binaries")
    if not expected_value:
        raise ValueError("binaries are required")
    expected = _binary_records(expected_value, "manifest")
    actual = _binary_records(executables, "executables") if executables else {}
    if not actual:
        raise ValueError("executables are required")
    if set(expected) != set(actual):
        raise ValueError("binaries do not correspond to executables")

    expected_identities = {
        name: _binary_identity(name, record, "manifest")
        for name, record in expected.items()
    }
    actual_identities = {
        name: _binary_identity(name, record, "executables")
        for name, record in actual.items()
    }
    for name in sorted(expected):
        expected_path, expected_hash = expected_identities[name]
        actual_path, actual_hash = actual_identities[name]
        if expected_path != actual_path:
            raise ValueError(f"binary path divergence for {name}")
        if expected_hash != actual_hash:
            raise ValueError(f"binary hash divergence for {name}")
    return True


__all__ = [
    "capture_code_identity",
    "capture_controller_helpers",
    "capture_git_provenance",
    "capture_git_state",
    "file_identity",
    "snapshot_files",
    "snapshot_selected_files",
    "validate_build_linkage",
]
