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
        if not path.is_file() or path.is_symlink() or any(part in excluded for part in path.relative_to(repository).parts):
            continue
        relative = path.relative_to(repository).as_posix().encode("utf-8")
        before = path.stat()
        data = path.read_bytes()
        after = path.stat()
        if (before.st_size, before.st_mtime_ns, before.st_ino) != (after.st_size, after.st_mtime_ns, after.st_ino):
            raise ValueError(f"source tree race while hashing: {path.relative_to(repository)}")
        digest.update(relative + b"\0" + data + b"\0")
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


def _status_xy(line: str) -> tuple[str, str]:
    return (line[0], line[1]) if len(line) >= 2 else ("?", "?")


_SECRET_SOURCE_RE = re.compile(
    rb"(?:^|[\s\"'])(?:password|passwd|secret|token|api[_-]?key|private[_-]?key|credential)\s*[:=]",
    re.IGNORECASE | re.MULTILINE,
)


def _source_contains_secret(path: str, data: bytes) -> bool:
    name = Path(path).name.lower()
    if any(word in name for word in ("secret", "credential", "password", ".env")):
        return True
    return bool(_SECRET_SOURCE_RE.search(data))


def _copy_stable_bytes(source: Path, target: Path) -> tuple[int, str, str]:
    """Copy first, hash only the preserved bytes, and reject source races."""

    before = source.stat()
    target.parent.mkdir(parents=True, exist_ok=True)
    temporary = target.with_name(target.name + ".tmp")
    try:
        with source.open("rb") as input_file, temporary.open("wb") as output_file:
            shutil.copyfileobj(input_file, output_file, length=1024 * 1024)
            output_file.flush()
            os.fsync(output_file.fileno())
        after = source.stat()
        signature_before = (before.st_size, before.st_mtime_ns, before.st_ino)
        signature_after = (after.st_size, after.st_mtime_ns, after.st_ino)
        if signature_before != signature_after:
            raise ValueError(f"source race while capturing {source}")
        os.replace(temporary, target)
        preserved_hash = _sha256(target)
        source_hash = _sha256(source)
        rehashed = _sha256(target)
        if source_hash != preserved_hash:
            raise ValueError(f"source race while hashing {source}")
        if preserved_hash != rehashed or target.stat().st_size != before.st_size:
            raise ValueError(f"preserved snapshot is unstable: {source}")
        return target.stat().st_size, preserved_hash, rehashed
    except Exception:
        try:
            temporary.unlink()
        except FileNotFoundError:
            pass
        try:
            target.unlink()
        except FileNotFoundError:
            pass
        raise


def _write_snapshot_bytes(data: bytes, target: Path) -> tuple[int, str, str]:
    """Persist staged bytes before hashing the resulting evidence file."""
    target.parent.mkdir(parents=True, exist_ok=True)
    temporary = target.with_name(target.name + ".tmp")
    try:
        with temporary.open("wb") as output_file:
            output_file.write(data)
            output_file.flush()
            os.fsync(output_file.fileno())
        os.replace(temporary, target)
        first = _sha256(target)
        second = _sha256(target)
        if first != second or target.stat().st_size != len(data):
            raise ValueError(f"staged snapshot is unstable: {target}")
        return target.stat().st_size, first, second
    except Exception:
        try:
            temporary.unlink()
        except FileNotFoundError:
            pass
        try:
            target.unlink()
        except FileNotFoundError:
            pass
        raise


def _capture_staged_snapshot(repository: Path, relative: str, destination: Path) -> dict[str, Any]:
    try:
        run = subprocess.run(
            ["git", "-C", str(repository), "cat-file", "blob", f":{relative}"],
            capture_output=True,
            timeout=10,
            check=False,
        )
    except (OSError, subprocess.SubprocessError) as exc:
        return {"state": "unavailable", "error": f"staged input unavailable: {relative}: {exc}"}
    if run.returncode != 0:
        return {"state": "unavailable", "error": f"staged input unavailable: {relative}"}
    data = run.stdout
    if _source_contains_secret(relative, data):
        return {"state": "protected-input", "protected_input": True, "error": f"protected-input prevents publishing staged source content: {relative}"}
    target = destination / "staged" / Path(relative)
    try:
        size, digest, rehashed = _write_snapshot_bytes(data, target)
    except (OSError, ValueError) as exc:
        return {"state": "race", "error": str(exc)}
    return {
        "state": "captured",
        "snapshot_path": (Path("provenance") / "source-snapshot" / "staged" / Path(relative)).as_posix(),
        "size": size,
        "sha256": digest,
        "rehash_sha256": rehashed,
    }


def _capture_content_snapshot(
    repository: Path,
    status: Sequence[str],
    destination: Path,
    excluded_root: Path | None = None,
) -> dict[str, Any]:
    """Copy dirty inputs and record hashes, rather than recording status only."""

    files: list[dict[str, Any]] = []
    entries: list[dict[str, Any]] = []
    errors: list[str] = []
    destination = destination.resolve()
    excluded = excluded_root.resolve() if excluded_root is not None else None
    for line in status:
        relative = _status_path(line)
        if not relative:
            errors.append(f"unparseable git status entry: {line}")
            continue
        index_status, worktree_status = _status_xy(line)
        raw_candidate = repository / relative
        candidate = raw_candidate.resolve(strict=False)
        entry: dict[str, Any] = {"path": Path(relative).as_posix(), "status": line, "index": index_status, "worktree": worktree_status}
        entries.append(entry)
        if index_status == "D" or worktree_status == "D":
            entry["state"] = "deleted"
            entry["reconstructable"] = False
            errors.append(f"deleted dirty input is not reconstructable from working-tree bytes: {relative}")
            continue
        try:
            candidate.relative_to(repository)
        except ValueError:
            entry.update({"state": "escaped", "reconstructable": False})
            errors.append(f"dirty input escapes repository: {relative}")
            continue
        if excluded is not None:
            try:
                candidate.relative_to(excluded)
                entry.update({"state": "excluded", "reconstructable": False})
                errors.append(f"dirty input is inside excluded provenance root: {relative}")
                continue
            except ValueError:
                pass
        if raw_candidate.is_symlink() or not candidate.is_file():
            if candidate.is_dir() and (candidate / ".git").exists():
                entry.update({"state": "submodule", "reconstructable": False})
                errors.append(f"submodule dirty input requires protected recursive capture: {relative}")
            else:
                entry.update({"state": "unavailable", "reconstructable": False})
                errors.append(f"dirty input is not a regular file: {relative}")
            continue
        try:
            data = candidate.read_bytes()
        except OSError as exc:
            entry.update({"state": "unavailable", "reconstructable": False})
            errors.append(f"dirty input unreadable: {relative}: {exc}")
            continue
        if _source_contains_secret(relative, data):
            entry.update({"state": "protected-input", "protected_input": True, "reconstructable": False})
            errors.append(f"protected-input prevents publishing dirty source content: {relative}")
            continue
        target = destination / Path(relative)
        try:
            size, preserved_hash, rehashed = _copy_stable_bytes(candidate, target)
        except (OSError, ValueError) as exc:
            entry.update({"state": "race", "reconstructable": False})
            errors.append(str(exc))
            continue
        entry.update({"state": "captured", "reconstructable": True, "snapshot_path": (Path("provenance") / "source-snapshot" / Path(relative)).as_posix()})
        record = {
            "path": Path(relative).as_posix(),
            "size": size,
            "sha256": preserved_hash,
            "snapshot_sha256": preserved_hash,
            "rehash_sha256": rehashed,
            "snapshot_path": entry["snapshot_path"],
        }
        files.append(record)
        if index_status not in {"?", " "}:
            staged = _capture_staged_snapshot(repository, relative, destination)
            entry["staged_identity"] = staged
            if staged.get("state") != "captured":
                errors.append(str(staged.get("error") or f"staged input is not reconstructable: {relative}"))
            else:
                record["staged_snapshot_path"] = staged["snapshot_path"]
                record["staged_sha256"] = staged["sha256"]
                record["staged_size"] = staged["size"]
        else:
            entry["staged_identity"] = {"status": "not-staged"}
    return {
        "captured": not errors and all(item.get("reconstructable", False) for item in entries),
        "files": sorted(files, key=lambda item: item["path"]),
        "entries": sorted(entries, key=lambda item: item["path"]),
        "errors": errors,
        "root": str(repository),
    }


def capture_git_state(
    repo: os.PathLike[str] | str,
    snapshot_dir: os.PathLike[str] | str | None = None,
    excluded_root: os.PathLike[str] | str | None = None,
    include_tree: bool = False,
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
        result["submodules_complete"] = submodule_run.returncode == 0
    if include_tree:
        result["tree_sha256"] = _tree_sha256(repository)
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
_SECRET_ASSIGN_RE = re.compile(r"((?:--?|/)?(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key)(?:=|\s+))([^\s'\"&]+)", re.IGNORECASE)
_SECRET_QUERY_RE = re.compile(r"([?&](?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key)=)[^&#\s]+", re.IGNORECASE)
_AUTH_RE = re.compile(r"(\b(?:authorization\s*:\s*bearer|bearer)\s+)[^\s'\"]+", re.IGNORECASE)


def _collect_secret_values(value: Any, key: str | None = None) -> set[str]:
    if isinstance(value, Mapping):
        values: set[str] = set()
        for name, item in value.items():
            if _SECRET_KEY_RE.search(str(name)) and isinstance(item, (str, int, float)):
                values.add(str(item))
            values.update(_collect_secret_values(item, str(name)))
        return values
    if isinstance(value, (list, tuple)):
        values: set[str] = set()
        for item in value:
            values.update(_collect_secret_values(item))
        return values
    return set()


def _redact_string(value: str, secrets: set[str]) -> str:
    result = value
    for secret in sorted((item for item in secrets if item), key=len, reverse=True):
        result = result.replace(secret, "<redacted>")
    result = _SECRET_QUERY_RE.sub(r"\1<redacted>", result)
    result = _SECRET_ASSIGN_RE.sub(r"\1<redacted>", result)
    result = _AUTH_RE.sub(r"\1<redacted>", result)
    return result


def sanitize_config(value: Any, key: str | None = None, _known_secrets: set[str] | None = None) -> Any:
    """Redact secret-looking values in config, argv, URLs, and commands."""

    secrets = set(_known_secrets or ()) | _collect_secret_values(value, key)

    def sanitize(item: Any, item_key: str | None = None, sensitive_next: bool = False) -> Any:
        if item_key and _SECRET_KEY_RE.search(item_key):
            return "<redacted>"
        if sensitive_next and isinstance(item, str):
            return "<redacted>"
        if isinstance(item, str):
            return _redact_string(item, secrets)
        if isinstance(item, Mapping):
            return {str(name): sanitize(child, str(name)) for name, child in sorted(item.items(), key=lambda pair: str(pair[0]))}
        if isinstance(item, (list, tuple)):
            result: list[Any] = []
            redact_next = False
            for child in item:
                child_text = child if isinstance(child, str) else ""
                result.append(sanitize(child, sensitive_next=redact_next))
                redact_next = bool(_SECRET_KEY_RE.search(child_text.lstrip("-/"))) or bool(re.search(r"(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key)=?$", child_text, re.IGNORECASE))
            return result
        return item

    if key and _SECRET_KEY_RE.search(key):
        return "<redacted>"
    return sanitize(value, key)


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


def _preserve_manifest(path: Path, evidence_root: Path, manifest: Mapping[str, Any]) -> dict[str, Any]:
    """Keep a secret-safe manifest copy and identities for both sides."""

    source_stat = path.stat()
    target = evidence_root / "provenance" / "build-manifest.json"
    target.parent.mkdir(parents=True, exist_ok=True)
    safe = sanitize_config(manifest)
    target.write_text(json.dumps(safe, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    after = path.stat()
    if (source_stat.st_size, source_stat.st_mtime_ns, source_stat.st_ino) != (after.st_size, after.st_mtime_ns, after.st_ino):
        raise ValueError("build manifest raced during preservation")
    preserved = _evidence_file_record(target, "provenance/build-manifest.json")
    return {
        "path": str(path.resolve()),
        "size": source_stat.st_size,
        "sha256": _sha256(path),
        "preserved_path": preserved["path"],
        "preserved_size": preserved["size"],
        "preserved_sha256": preserved["sha256"],
    }


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
    known_secrets = _collect_secret_values({"plan": plan, "expanded": expanded, "contract": contract, "config": config})
    records: dict[str, dict[str, Any]] = {}
    payloads = {
        "plan/original.json": sanitize_config(plan, _known_secrets=known_secrets),
        "plan/expanded.json": sanitize_config(expanded, _known_secrets=known_secrets),
        "plan/scenario.json": sanitize_config(contract.get("scenario", {}), _known_secrets=known_secrets),
        "plan/observability.json": sanitize_config(contract.get("profile", {}), _known_secrets=known_secrets),
        "plan/effective-config.json": sanitize_config({str(key): str(value) for key, value in config.items()}, _known_secrets=known_secrets),
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
    path = str(Path(str(raw_path)).resolve(strict=False))
    if not Path(path).is_absolute():
        raise ValueError(f"{field} binary {name!r} path must be absolute")
    raw_hash = record.get("sha256")
    if raw_hash is None:
        candidate = Path(str(raw_path))
        if not candidate.is_file():
            raise ValueError(f"{field} binary {name!r} requires sha256")
        raw_hash = _sha256(candidate)
    return path, _normalise_hash(raw_hash, f"{field} binary {name!r} sha256")


def _identity_unavailable(value: Any) -> bool:
    if isinstance(value, Mapping):
        return value.get("status") == "unavailable" or any(_identity_unavailable(item) for item in value.values())
    if isinstance(value, Sequence) and not isinstance(value, (str, bytes)):
        return any(_identity_unavailable(item) for item in value)
    return False


def validate_build_linkage(
    manifest: Mapping[str, Any],
    source_head: str | None,
    executables: Mapping[str, Any] | Sequence[Mapping[str, Any]],
    source_state: Mapping[str, Any] | None = None,
    manifest_evidence: Mapping[str, Any] | None = None,
) -> bool:
    """Validate that a build manifest links the supplied source and binaries.

    A valid manifest must carry the complete source/build identity and a
    separately preserved manifest record.  Every launched role is matched to
    one stable path+hash identity; basename-only ambiguity is rejected.
    """

    if not isinstance(manifest, Mapping) or not manifest:
        raise ValueError("manifest is required and must not be empty")

    recipe = manifest.get("recipe", manifest.get("build_recipe"))
    if not isinstance(recipe, Mapping) or not recipe:
        raise ValueError("complete build recipe identity is required")
    recipe_commands = recipe.get("commands", recipe.get("command"))
    if not recipe_commands:
        raise ValueError("complete build recipe commands are required")
    toolchain = manifest.get("toolchain", manifest.get("toolchain_identity"))
    options = manifest.get("options", manifest.get("build_options"))
    for field, value in (("toolchain", toolchain), ("options", options)):
        if not value:
            raise ValueError(f"complete build {field} identity is required")
        if isinstance(value, Mapping) and value.get("status") == "unavailable":
            raise ValueError(f"build {field} identity is unavailable")
    runtime_identity = manifest.get("runtime_library_identity")
    if runtime_identity is None:
        runtime_identity = manifest.get("runtime_libraries") or manifest.get("runtime_library")
    if not runtime_identity:
        raise ValueError("runtime-library identity is required")
    if _identity_unavailable(runtime_identity):
        raise ValueError("runtime-library identity is unavailable or incomplete")
    if manifest_evidence is None and isinstance(manifest.get("manifest_evidence"), Mapping):
        manifest_evidence = manifest["manifest_evidence"]
    if not isinstance(manifest_evidence, Mapping):
        raise ValueError("preserved manifest evidence is required")
    evidence_path = manifest_evidence.get("path")
    evidence_size = manifest_evidence.get("size")
    _normalise_hash(manifest_evidence.get("sha256"), "preserved manifest evidence sha256")
    _normalise_hash(manifest_evidence.get("preserved_sha256"), "preserved manifest evidence copy sha256")
    preserved_size = manifest_evidence.get("preserved_size")
    preserved_path = manifest_evidence.get("preserved_path")
    if not isinstance(evidence_path, str) or not evidence_path or not isinstance(evidence_size, int) or evidence_size < 0 or not isinstance(preserved_path, str) or not preserved_path or not isinstance(preserved_size, int) or preserved_size < 0:
        raise ValueError("preserved manifest evidence is incomplete")

    snapshot = manifest.get("source_snapshot")
    if snapshot is None and isinstance(manifest.get("source"), Mapping):
        source = manifest["source"]
        snapshot = source.get("snapshot") or source
    if not isinstance(snapshot, Mapping):
        raise ValueError("complete source snapshot identity is required")
    snapshot_head = snapshot.get("head", manifest.get("source_head"))
    if not isinstance(snapshot_head, str) or not snapshot_head:
        raise ValueError("source snapshot HEAD is required")
    status = snapshot.get("status")
    if not isinstance(status, list) or any(not isinstance(item, str) for item in status):
        raise ValueError("source snapshot status is required")
    submodules = snapshot.get("submodules")
    if not isinstance(submodules, list) or snapshot.get("submodules_complete") is not True:
        raise ValueError("source submodule identity is incomplete")
    tree_hash = snapshot.get("tree_sha256")
    _normalise_hash(tree_hash, "source snapshot tree_sha256")
    content_snapshot = snapshot.get("content_snapshot")
    if not isinstance(content_snapshot, Mapping) or content_snapshot.get("captured") is not True or content_snapshot.get("errors"):
        raise ValueError("source content snapshot is incomplete")
    if any(entry.get("state") != "captured" for entry in content_snapshot.get("entries", []) if isinstance(entry, Mapping)):
        raise ValueError("source content snapshot contains unreconstructable entries")
    _snapshot_records(content_snapshot.get("files", [])) if content_snapshot.get("files") else []

    if not isinstance(source_head, str) or not source_head:
        raise ValueError("source_head is required")
    manifest_head = manifest.get("source_head")
    if manifest_head is None and isinstance(manifest.get("source"), Mapping):
        manifest_head = manifest["source"].get("head")
    if manifest_head is None and isinstance(snapshot, Mapping):
        manifest_head = snapshot.get("head")
    if not isinstance(manifest_head, str) or not manifest_head:
        raise ValueError("manifest source head is required")
    if manifest_head != source_head or manifest_head != snapshot_head:
        raise ValueError("source head divergence")
    if source_state is not None and isinstance(snapshot, Mapping):
        if source_state.get("tree_sha256") and snapshot["tree_sha256"] != source_state["tree_sha256"]:
            raise ValueError("source content divergence")
        if source_state.get("status") is not None and source_state.get("status") != snapshot.get("status"):
            raise ValueError("source status divergence")

    expected_value = manifest.get("binaries")
    if not expected_value:
        raise ValueError("binaries are required")
    expected = _binary_records(expected_value, "manifest")
    actual = _binary_records(executables, "executables") if executables else {}
    if not actual:
        raise ValueError("executables are required")
    expected_identities = {
        name: _binary_identity(name, record, "manifest")
        for name, record in expected.items()
    }
    actual_names: dict[str, tuple[str, str]] = {}
    for role, record in actual.items():
        identity = _binary_identity(role, record, "executables")
        basename = Path(identity[0]).name
        if basename in actual_names and actual_names[basename] != identity:
            raise ValueError(f"executable basename/path collision for {basename}")
        actual_names[basename] = identity
    matched: dict[str, tuple[str, str]] = {}
    for role, record in actual.items():
        actual_path, actual_hash = _binary_identity(role, record, "executables")
        manifest_name = str(record.get("manifest_name") or Path(actual_path).name) if isinstance(record, Mapping) else Path(actual_path).name
        candidates = [role] if role in expected else [name for name in expected if name == manifest_name]
        if not candidates:
            basename_candidates = [name for name, identity in expected_identities.items() if Path(identity[0]).name == manifest_name]
            if len(basename_candidates) > 1 and len({expected_identities[name] for name in basename_candidates}) > 1:
                raise ValueError(f"executable basename/path collision for {manifest_name}")
            candidates = basename_candidates
        if not candidates:
            raise ValueError(f"binaries do not correspond to launched executable {role}")
        if len(candidates) != 1:
            raise ValueError(f"ambiguous executable mapping for {role}")
        name = candidates[0]
        previous = matched.get(name)
        if previous is not None and previous != (actual_path, actual_hash):
            raise ValueError(f"executable basename/path collision for {manifest_name}")
        matched[name] = (actual_path, actual_hash)
        expected_path, expected_hash = expected_identities[name]
        if expected_path != actual_path:
            raise ValueError(f"binary path divergence for {name}")
        if expected_hash != actual_hash:
            raise ValueError(f"binary hash divergence for {name}")
    if not matched:
        raise ValueError("executables are required")
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
    except (OSError, RuntimeError, ValueError, subprocess.SubprocessError) as exc:
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
    executable_names: dict[str, tuple[str, str | None]] = {}
    executable_collision_error: str | None = None
    for role in plan.get("roles", []):
        name = role.get("name")
        argv = role.get("command", [])
        if not isinstance(name, str) or not isinstance(argv, list) or not argv:
            continue
        expanded = _expand_value(argv, variables)
        argv0 = expanded[0]
        candidate = Path(argv0) if Path(argv0).is_absolute() else Path(shutil.which(argv0) or argv0)
        resolved_path = str(candidate.resolve(strict=False))
        record: dict[str, Any] = {"argv0": argv0, "resolved_path": resolved_path, "path": resolved_path}
        resolved_candidate = Path(resolved_path)
        if resolved_candidate.is_file():
            record.update({"size": resolved_candidate.stat().st_size, "sha256": _sha256(resolved_candidate)})
        else:
            record.update({"size": None, "sha256": None})
        binaries[name] = record
        manifest_name = candidate.name
        manifest_record = {"path": record["resolved_path"], "sha256": record["sha256"], "manifest_name": manifest_name}
        prior = executable_names.get(manifest_name)
        current_identity = (resolved_path, record["sha256"])
        if prior is not None and prior != current_identity:
            executable_collision_error = f"executable basename/path collision for {manifest_name}"
        executable_names[manifest_name] = current_identity
        # Keep one record per launched role. Never collapse roles by basename.
        manifest_executables[name] = manifest_record

    manifest_path = Path(config["NG_LOCAL_BUILD_MANIFEST"]).resolve() if config.get("NG_LOCAL_BUILD_MANIFEST") else None
    manifest_data: dict[str, Any] | None = None
    manifest_error: str | None = None
    manifest_record: dict[str, Any] | None = None
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
            if root is None:
                raise ValueError("evidence root is required to preserve build manifest")
            manifest_record = _preserve_manifest(manifest_path, root, loaded)
        except (OSError, json.JSONDecodeError, ValueError) as exc:
            manifest_error = f"build manifest unreadable: {exc}"

    build_linkage = False
    linkage_error = manifest_error
    if executable_collision_error:
        linkage_error = executable_collision_error
    elif manifest_data is not None:
        try:
            build_linkage = validate_build_linkage(
                manifest_data,
                git_state.get("head"),
                manifest_executables,
                source_state=git_state,
                manifest_evidence=manifest_record,
            )
            linkage_error = None
        except (TypeError, ValueError) as exc:
            linkage_error = str(exc)

    if manifest_path is not None and manifest_path.is_file() and not manifest_path.is_symlink():
        if manifest_record is None:
            manifest_record = {"path": str(manifest_path), "sha256": _sha256(manifest_path), "valid": False}
        manifest_record["valid"] = build_linkage

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
        "protected_inputs": list(source["content_snapshot"].get("errors", [])) if source["content_snapshot"].get("errors") else [],
        "git_head": git_state.get("head"),
        "git_status": git_state.get("status", []),
        "git_clean": bool(git_state.get("clean", False)),
        "repo_path": str(Path(repo).resolve()),
        "evidence_dir": str(root) if root is not None else None,
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
            "runtime_library_identity": (manifest_data or {}).get("runtime_library_identity", (manifest_data or {}).get("runtime_libraries")),
            "manifest_evidence": manifest_record,
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
