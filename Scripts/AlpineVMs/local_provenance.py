"""Canonical, side-effect-limited provenance capture for local NG-ELC runs.

The lifecycle imports this module rather than maintaining a second provenance
implementation.  Captures are serialisable, content-addressed, and fail closed
when required source, controller, plan, or build identity cannot be verified.
"""

from __future__ import annotations

import hashlib
import json
import os
import re
import shutil
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


def _tree_sha256(repository: Path) -> str:
    """Match the build helper's content identity without reading ``.git``."""

    excluded = {".git", "build", "cmake-build-debug", "cmake-build-sanitizer", "cmake-build-relwithdebinfo"}
    digest = hashlib.sha256()
    for path in sorted(repository.rglob("*")):
        if not path.is_file() or any(part in excluded for part in path.relative_to(repository).parts):
            continue
        relative = path.relative_to(repository).as_posix().encode("utf-8")
        digest.update(relative + b"\0" + path.read_bytes() + b"\0")
    return digest.hexdigest()


def _status_path(line: str) -> str | None:
    if len(line) < 4:
        return None
    value = line[3:].strip()
    if " -> " in value:
        value = value.rsplit(" -> ", 1)[-1]
    if value.startswith('"') and value.endswith('"'):
        try:
            value = json.loads(value)
        except json.JSONDecodeError:
            return None
    return value or None


def _capture_content_snapshot(
    repository: Path,
    status: Sequence[str],
    destination: Path,
    excluded_root: Path | None = None,
) -> dict[str, Any]:
    """Copy dirty inputs and record hashes, rather than recording status only."""

    files: list[dict[str, Any]] = []
    errors: list[str] = []
    destination = destination.resolve()
    excluded = excluded_root.resolve() if excluded_root is not None else None
    for line in status:
        relative = _status_path(line)
        if not relative:
            errors.append(f"unparseable git status entry: {line}")
            continue
        raw_candidate = repository / relative
        candidate = raw_candidate.resolve(strict=False)
        try:
            candidate.relative_to(repository)
        except ValueError:
            errors.append(f"dirty input escapes repository: {relative}")
            continue
        if excluded is not None:
            try:
                candidate.relative_to(excluded)
                continue
            except ValueError:
                pass
        if raw_candidate.is_symlink() or not candidate.is_file():
            errors.append(f"dirty input is not a regular file: {relative}")
            continue
        target = destination / Path(relative)
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(candidate, target)
        files.append({
            "path": Path(relative).as_posix(),
            "size": candidate.stat().st_size,
            "sha256": _sha256(candidate),
            "snapshot_path": (Path("provenance") / "source-snapshot" / Path(relative)).as_posix(),
        })
    return {
        "captured": not errors,
        "files": sorted(files, key=lambda item: item["path"]),
        "errors": errors,
        "root": str(repository),
    }


def capture_git_state(
    repo: os.PathLike[str] | str,
    snapshot_dir: os.PathLike[str] | str | None = None,
    excluded_root: os.PathLike[str] | str | None = None,
) -> dict[str, Any]:
    """Capture HEAD/status and, when requested, reconstructable dirty inputs."""

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
    head = head_run.stdout.strip() if head_run.returncode == 0 else None
    status = [line for line in status_run.stdout.splitlines() if line]
    if status_run.returncode != 0:
        status = ["git status unavailable"]
    result: dict[str, Any] = {"head": head or None, "status": status, "clean": status_run.returncode == 0 and not status}
    if snapshot_dir is not None:
        content_snapshot = _capture_content_snapshot(
            repository,
            [] if status == ["git status unavailable"] else status,
            Path(snapshot_dir),
            Path(excluded_root) if excluded_root is not None else None,
        )
        if not status:
            content_snapshot["captured"] = True
            content_snapshot["reason"] = "working tree clean"
        result["content_snapshot"] = content_snapshot
        result["tree_sha256"] = _tree_sha256(repository)
        submodule_run = subprocess.run(
            ["git", "-C", str(repository), "submodule", "status", "--recursive"],
            capture_output=True,
            text=True,
            check=False,
            timeout=10,
        )
        result["submodules"] = submodule_run.stdout.splitlines() if submodule_run.returncode == 0 else ["git submodule status unavailable"]
    return result


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


_SECRET_KEY_RE = re.compile(r"(?:password|passwd|secret|token|credential|private[_-]?key|api[_-]?key|ssh[_-]?key|key$)", re.IGNORECASE)
_VARIABLE_RE = re.compile(r"\$\{([A-Za-z_][A-Za-z0-9_]*)\}")


def sanitize_config(value: Any, key: str | None = None) -> Any:
    """Redact secret-looking configuration values while retaining structure."""

    if key and _SECRET_KEY_RE.search(key):
        return "<redacted>"
    if isinstance(value, Mapping):
        return {str(name): sanitize_config(item, str(name)) for name, item in sorted(value.items(), key=lambda item: str(item[0]))}
    if isinstance(value, list):
        return [sanitize_config(item) for item in value]
    if isinstance(value, tuple):
        return [sanitize_config(item) for item in value]
    return value


def _expand_value(value: Any, variables: Mapping[str, str]) -> Any:
    if isinstance(value, str):
        return _VARIABLE_RE.sub(lambda match: variables.get(match.group(1), match.group(0)), value)
    if isinstance(value, Mapping):
        return {str(key): _expand_value(item, variables) for key, item in value.items()}
    if isinstance(value, list):
        return [_expand_value(item, variables) for item in value]
    return value


def _evidence_file_record(path: Path, relative: str) -> dict[str, Any]:
    return {"path": relative, "size": path.stat().st_size, "sha256": _sha256(path)}


def capture_plan_snapshot(
    evidence_dir: os.PathLike[str] | str,
    plan: Mapping[str, Any],
    contract: Mapping[str, Any],
    config: Mapping[str, Any],
    variables: Mapping[str, str],
) -> dict[str, Any]:
    """Persist original/expanded plan and a secret-free effective configuration."""

    root = Path(evidence_dir)
    plan_dir = root / "plan"
    expanded = _expand_value(json.loads(json.dumps(plan)), variables)
    records: dict[str, dict[str, Any]] = {}
    payloads = {
        "plan/original.json": sanitize_config(plan),
        "plan/expanded.json": sanitize_config(expanded),
        "plan/scenario.json": sanitize_config(contract.get("scenario", {})),
        "plan/observability.json": sanitize_config(contract.get("profile", {})),
        "plan/effective-config.json": sanitize_config({str(key): str(value) for key, value in config.items()}),
    }
    for relative, payload in payloads.items():
        path = root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        records[relative] = _evidence_file_record(path, relative)
    return {"complete": len(records) == len(payloads), "files": records}


def _snapshot_records(value: Any) -> list[Mapping[str, Any]]:
    if isinstance(value, Mapping):
        if "files" in value:
            value = value.get("files")
        elif "head" in value:
            # Build manifests produced by ng_observability use a git identity
            # object rather than a file list. The caller validates its HEAD;
            # this branch keeps that format lossless and fail-closed.
            if not isinstance(value.get("head"), str) or not value.get("head"):
                raise ValueError("source snapshot head is required")
            if value.get("tree_sha256") is not None:
                _normalise_hash(value["tree_sha256"], "source snapshot tree_sha256")
            return []
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
    source_head: str | None,
    executables: Mapping[str, Any] | Sequence[Mapping[str, Any]],
    source_state: Mapping[str, Any] | None = None,
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
    if recipe is None:
        recipe = manifest.get("commands")
    if not recipe:
        raise ValueError("recipe is required")

    snapshot = manifest.get("source_snapshot")
    if snapshot is None and isinstance(manifest.get("source"), Mapping):
        source = manifest["source"]
        snapshot = source.get("snapshot") or source
    _snapshot_records(snapshot)

    if not isinstance(source_head, str) or not source_head:
        raise ValueError("source_head is required")
    manifest_head = manifest.get("source_head")
    if manifest_head is None and isinstance(manifest.get("source"), Mapping):
        manifest_head = manifest["source"].get("head")
    if manifest_head is None and isinstance(snapshot, Mapping):
        manifest_head = snapshot.get("head")
    if not isinstance(manifest_head, str) or not manifest_head:
        raise ValueError("manifest source head is required")
    if manifest_head != source_head:
        raise ValueError("source head divergence")
    if source_state is not None and isinstance(snapshot, Mapping):
        if snapshot.get("tree_sha256") and source_state.get("tree_sha256") and snapshot["tree_sha256"] != source_state["tree_sha256"]:
            raise ValueError("source content divergence")
        if "status" in snapshot and source_state.get("status") != snapshot.get("status"):
            raise ValueError("source status divergence")

    expected_value = manifest.get("binaries")
    if not expected_value:
        raise ValueError("binaries are required")
    expected = _binary_records(expected_value, "manifest")
    actual = _binary_records(executables, "executables") if executables else {}
    if not actual:
        raise ValueError("executables are required")
    manifest_names = {
        str(record.get("manifest_name")): record
        for record in actual.values()
        if isinstance(record, Mapping) and record.get("manifest_name")
    }
    if manifest_names:
        actual = manifest_names
    if not set(actual) <= set(expected):
        raise ValueError("binaries do not correspond to executables")

    expected_identities = {
        name: _binary_identity(name, record, "manifest")
        for name, record in expected.items()
    }
    actual_identities = {
        name: _binary_identity(name, record, "executables")
        for name, record in actual.items()
    }
    for name in sorted(actual):
        expected_path, expected_hash = expected_identities[name]
        actual_path, actual_hash = actual_identities[name]
        if expected_path != actual_path:
            raise ValueError(f"binary path divergence for {name}")
        if expected_hash != actual_hash:
            raise ValueError(f"binary hash divergence for {name}")
    return True


def capture_local_provenance(
    repo: os.PathLike[str] | str,
    controller: os.PathLike[str] | str,
    helpers: Mapping[str, os.PathLike[str] | str] | None = None,
    plan: Mapping[str, Any] | None = None,
    variables: Mapping[str, str] | None = None,
    config: Mapping[str, Any] | None = None,
    evidence_dir: os.PathLike[str] | str | None = None,
    contract: Mapping[str, Any] | None = None,
) -> dict[str, Any]:
    """Capture the complete local R06 record used by the lifecycle."""

    variables = dict(variables or {})
    config = dict(config or {})
    plan = plan or {}
    contract = contract or {}
    root = Path(evidence_dir).resolve() if evidence_dir is not None else None
    source_snapshot_dir = root / "provenance" / "source-snapshot" if root else None
    try:
        git_state = capture_git_state(
            repo,
            snapshot_dir=source_snapshot_dir,
            excluded_root=root,
        )
        git_capture = True
    except (OSError, RuntimeError, subprocess.SubprocessError) as exc:
        git_state = {
            "head": None,
            "status": [f"git capture unavailable: {exc}"],
            "clean": False,
            "content_snapshot": {"captured": False, "files": [], "errors": [str(exc)]},
        }
        git_capture = False

    try:
        code_identity = capture_code_identity(controller, helpers or {})
        code_capture = True
    except (OSError, ValueError) as exc:
        code_identity = {"controller": None, "helpers": {}, "error": str(exc)}
        code_capture = False

    plan_record: dict[str, Any] = {"complete": False, "files": {}}
    if root is not None:
        try:
            plan_record = capture_plan_snapshot(root, plan, contract, config, variables)
        except (OSError, TypeError, ValueError) as exc:
            plan_record = {"complete": False, "files": {}, "error": str(exc)}

    binaries: dict[str, dict[str, Any]] = {}
    manifest_executables: dict[str, dict[str, Any]] = {}
    for role in plan.get("roles", []):
        name = role.get("name")
        argv = role.get("command", [])
        if not isinstance(name, str) or not isinstance(argv, list) or not argv:
            continue
        expanded = _expand_value(argv, variables)
        argv0 = expanded[0]
        candidate = Path(argv0) if Path(argv0).is_absolute() else Path(shutil.which(argv0) or argv0)
        record: dict[str, Any] = {"argv0": argv0, "resolved_path": str(candidate.resolve(strict=False)), "path": str(candidate.resolve(strict=False))}
        if candidate.is_file() and not candidate.is_symlink():
            record.update({"size": candidate.stat().st_size, "sha256": _sha256(candidate)})
        else:
            record.update({"size": None, "sha256": None})
        binaries[name] = record
        manifest_name = candidate.name
        manifest_record = {"path": record["resolved_path"], "sha256": record["sha256"], "manifest_name": manifest_name}
        if manifest_name not in manifest_executables or manifest_executables[manifest_name]["sha256"] == manifest_record["sha256"]:
            manifest_executables[manifest_name] = manifest_record

    manifest_path = Path(config["NG_LOCAL_BUILD_MANIFEST"]).resolve() if config.get("NG_LOCAL_BUILD_MANIFEST") else None
    manifest_data: dict[str, Any] | None = None
    manifest_error: str | None = None
    if manifest_path is None:
        manifest_error = "build manifest was not supplied"
    elif not manifest_path.is_file() or manifest_path.is_symlink():
        manifest_error = "build manifest is missing or unsafe"
    else:
        try:
            loaded = json.loads(manifest_path.read_text(encoding="utf-8"))
            if not isinstance(loaded, dict):
                raise ValueError("build manifest must be an object")
            manifest_data = loaded
        except (OSError, json.JSONDecodeError, ValueError) as exc:
            manifest_error = f"build manifest unreadable: {exc}"

    build_linkage = False
    linkage_error = manifest_error
    if manifest_data is not None:
        try:
            build_linkage = validate_build_linkage(
                manifest_data,
                git_state.get("head"),
                manifest_executables,
                source_state=git_state,
            )
            linkage_error = None
        except (TypeError, ValueError) as exc:
            linkage_error = str(exc)

    manifest_record = None
    if manifest_path is not None and manifest_path.is_file() and not manifest_path.is_symlink():
        manifest_record = {
            "path": str(manifest_path),
            "sha256": _sha256(manifest_path),
            "valid": build_linkage,
        }

    source = {
        **git_state,
        "content_snapshot": git_state.get("content_snapshot", {"captured": False, "files": [], "errors": ["not requested"]}),
    }
    controller_record = code_identity.get("controller") if isinstance(code_identity, Mapping) else None
    helper_records = code_identity.get("helpers", {}) if isinstance(code_identity, Mapping) else {}
    return {
        "schema_version": 2,
        "mode": "local",
        "source": source,
        "source_head": git_state.get("head"),
        "source_status": git_state.get("status", []),
        "source_content_snapshot": source["content_snapshot"],
        "source_snapshot_complete": bool(source["content_snapshot"].get("captured", False)),
        "git_head": git_state.get("head"),
        "git_status": git_state.get("status", []),
        "git_clean": bool(git_state.get("clean", False)),
        "repo_path": str(Path(repo).resolve()),
        "build_path": str(Path(config["NG_LOCAL_BUILD_PATH"]).resolve()) if config.get("NG_LOCAL_BUILD_PATH") else None,
        "io_path": str(Path(config["NG_LOCAL_IO_PATH"]).resolve()) if config.get("NG_LOCAL_IO_PATH") else None,
        "controller_identity": code_capture and bool(controller_record),
        "controller": controller_record,
        "controller_hash": controller_record.get("sha256") if isinstance(controller_record, Mapping) else None,
        "helpers": helper_records,
        "helper_hashes": {name: record.get("sha256") for name, record in helper_records.items()} if isinstance(helper_records, Mapping) else {},
        "controller_helpers": code_identity,
        "controller_identity_error": code_identity.get("error"),
        "plan_snapshot": bool(plan_record.get("complete")),
        "plan": plan_record,
        "expanded_plan": plan_record.get("files", {}).get("plan/expanded.json"),
        "effective_config": sanitize_config(config),
        "sanitized_config": sanitize_config(config),
        "build_linkage": build_linkage,
        "build_linkage_reason": linkage_error,
        "build_manifest": manifest_record,
        "build": sanitize_config({
            "recipe": (manifest_data or {}).get("recipe", (manifest_data or {}).get("build_recipe")),
            "commands": (manifest_data or {}).get("commands"),
            "source_snapshot": (manifest_data or {}).get("source_snapshot"),
            "toolchain": (manifest_data or {}).get("toolchain"),
            "options": (manifest_data or {}).get("options"),
        }),
        "binaries": binaries,
        "capture_complete": git_capture and code_capture and bool(source["content_snapshot"].get("captured", False)) and bool(plan_record.get("complete")) and build_linkage,
    }


__all__ = [
    "capture_code_identity",
    "capture_controller_helpers",
    "capture_git_provenance",
    "capture_git_state",
    "capture_local_provenance",
    "capture_plan_snapshot",
    "file_identity",
    "sanitize_config",
    "snapshot_files",
    "snapshot_selected_files",
    "validate_build_linkage",
]
