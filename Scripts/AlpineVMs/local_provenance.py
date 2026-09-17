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


def _git_argv(repository: os.PathLike[str] | str, *args: str) -> list[str]:
    """Build a Git argv safe for execution as root on a user-owned repo."""
    root = str(Path(repository).resolve())
    return ["git", "-c", f"safe.directory={root}", "-C", root, *args]


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


def _tree_sha256(repository: Path, excluded_root: Path | None = None) -> str:
    """Match the build helper's content identity without reading ``.git``."""

    excluded = {".git", "build", "cmake-build-debug", "cmake-build-sanitizer", "cmake-build-relwithdebinfo", "__pycache__"}
    excluded_path = excluded_root.resolve() if excluded_root is not None else None
    digest = hashlib.sha256()
    for path in sorted(repository.rglob("*")):
        if not path.is_file() or path.is_symlink() or any(part in excluded for part in path.relative_to(repository).parts):
            continue
        if excluded_path is not None:
            try:
                path.resolve(strict=False).relative_to(excluded_path)
            except ValueError:
                pass
            else:
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


def _source_status(repository: Path, status_run: subprocess.CompletedProcess[str], excluded_root: Path | None) -> list[str]:
    if status_run.returncode != 0:
        return ["git status unavailable"]
    status = [line for line in status_run.stdout.splitlines() if line]
    if excluded_root is None:
        return status
    excluded = excluded_root.resolve()
    filtered: list[str] = []
    for line in status:
        relative = _status_path(line)
        if not relative:
            filtered.append(line)
            continue
        candidate = (repository / relative).resolve(strict=False)
        try:
            candidate.relative_to(excluded)
        except ValueError:
            filtered.append(line)
    return filtered


def _status_xy(line: str) -> tuple[str, str]:
    return (line[0], line[1]) if len(line) >= 2 else ("?", "?")


_SECRET_SOURCE_RE = re.compile(
    rb"(?:^|[\s\"'])(?:export\s+)?(?:[A-Za-z_][A-Za-z0-9.-]*(?:password|passwd|secret|token|credential|private[_-]?key|api[_-]?key|ssh[_-]?key)|password|passwd|secret|token|api[_-]?key|private[_-]?key|credential)\s*[:=]",
    re.IGNORECASE | re.MULTILINE,
)
_URL_USERINFO_RE = re.compile(rb"[A-Za-z][A-Za-z0-9+.-]*://[^/\s:@]+:[^/@\s]+@", re.IGNORECASE)


def _source_contains_secret(path: str, data: bytes) -> bool:
    name = Path(path).name.lower()
    if any(word in name for word in ("secret", "credential", "password", ".env")):
        return True
    return bool(_SECRET_SOURCE_RE.search(data) or _URL_USERINFO_RE.search(data))


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
            _git_argv(repository, "cat-file", "blob", f":{relative}"),
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


def _capture_index_identity(repository: Path) -> dict[str, Any]:
    """Capture the index entries, including the exact staged blob ids."""

    try:
        run = subprocess.run(
            _git_argv(repository, "ls-files", "--stage", "-z"),
            capture_output=True,
            timeout=10,
            check=False,
        )
    except (OSError, subprocess.SubprocessError) as exc:
        return {"captured": False, "entries": [], "errors": [f"git index unavailable: {exc}"]}
    if run.returncode != 0:
        return {"captured": False, "entries": [], "errors": ["git index unavailable"]}
    entries: list[dict[str, Any]] = []
    canonical: list[str] = []
    for raw in run.stdout.split(b"\0"):
        if not raw:
            continue
        try:
            header, relative_bytes = raw.split(b"\t", 1)
            mode, blob_id, stage = header.decode("ascii").split()
            relative = relative_bytes.decode("utf-8")
        except (UnicodeDecodeError, ValueError):
            return {"captured": False, "entries": [], "errors": ["git index contains an invalid staged entry"]}
        if not re.fullmatch(r"[0-7]{6}", mode) or not re.fullmatch(r"[0-9a-fA-F]{40,64}", blob_id) or stage not in {"0", "1", "2", "3"}:
            return {"captured": False, "entries": [], "errors": [f"git index entry is invalid: {relative}"]}
        item = {"path": Path(relative).as_posix(), "mode": mode, "blob_id": blob_id.lower(), "stage": int(stage)}
        entries.append(item)
        canonical.append(f"{mode} {blob_id.lower()} {stage}\t{item['path']}\n")
    canonical_bytes = "".join(sorted(canonical)).encode("utf-8")
    return {
        "captured": True,
        "entries": sorted(entries, key=lambda item: (item["path"], item["stage"])),
        "sha256": hashlib.sha256(canonical_bytes).hexdigest(),
        "errors": [],
    }


def _current_index_blob(repository: Path, relative: str) -> dict[str, Any] | None:
    """Read the current index blob bytes without consulting working-tree bytes."""

    try:
        run = subprocess.run(
            _git_argv(repository, "cat-file", "blob", f":{relative}"),
            capture_output=True,
            timeout=10,
            check=False,
        )
    except (OSError, subprocess.SubprocessError):
        return None
    if run.returncode != 0:
        return None
    return {"sha256": hashlib.sha256(run.stdout).hexdigest(), "size": len(run.stdout)}


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
        _git_argv(repository, "rev-parse", "HEAD"),
        capture_output=True,
        text=True,
        check=False,
        timeout=10,
    )
    status_run = subprocess.run(
        _git_argv(repository, "status", "--porcelain=v1", "--untracked-files=all"),
        capture_output=True,
        text=True,
        check=False,
        timeout=10,
    )
    head = head_run.stdout.strip() if head_run.returncode == 0 else None
    status = _source_status(repository, status_run, Path(excluded_root).resolve() if excluded_root is not None else None)
    result: dict[str, Any] = {
        "head": head or None,
        "status": status,
        "clean": status_run.returncode == 0 and not status,
    }
    index_identity = _capture_index_identity(repository)
    result.update({
        "index": index_identity,
        "index_sha256": index_identity.get("sha256"),
        "index_entries": index_identity.get("entries", []),
    })
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
        result["tree_sha256"] = _tree_sha256(repository, excluded_root=Path(excluded_root) if excluded_root is not None else None)
    if snapshot_dir is not None or include_tree:
        submodule_run = subprocess.run(
            _git_argv(repository, "submodule", "status", "--recursive"),
            capture_output=True,
            text=True,
            check=False,
            timeout=10,
        )
        result["submodules"] = submodule_run.stdout.splitlines() if submodule_run.returncode == 0 else ["git submodule status unavailable"]
        result["submodules_complete"] = submodule_run.returncode == 0
    if include_tree:
        result["tree_sha256"] = _tree_sha256(repository, excluded_root=Path(excluded_root) if excluded_root is not None else None)
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
_SECRET_ASSIGN_RE = re.compile(
    r"((?:--?|/)?(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key)(?:=|\s+))"
    r"(?:(['\"])(.*?)\2|([^\s'\"&;]+))",
    re.IGNORECASE,
)
_SECRET_QUERY_RE = re.compile(r"([?&](?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key)=)(?P<query_value>[^&#\s]+)", re.IGNORECASE)
_AUTH_RE = re.compile(r"(\b(?:authorization\s*:\s*bearer|bearer)\s+)(?P<auth_value>[^\s'\"]+)", re.IGNORECASE)
_URL_USERINFO_TEXT_RE = re.compile(r"(\b[A-Za-z][A-Za-z0-9+.-]*://)([^/@\s:]+):(?P<url_password>[^/@\s]+)@", re.IGNORECASE)
_SECRET_SOURCE_ASSIGN_TEXT_RE = re.compile(
    r"(\b(?:export\s+)?[A-Za-z_][A-Za-z0-9.-]*(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key|ssh[-_]?key)\s*[:=]\s*)"
    r"(?:(['\"])(.*?)\2|([^\s'\";&]+))",
    re.IGNORECASE,
)
# Transport syntax can carry secrets without a structured secret key.  Keep
# these patterns shared by collection and evidence-tree sanitisation.
_SECRET_ARGV_TEXT_RE = re.compile(
    r"(?:^|[\s\[,(])(?P<flag>--?|/)(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key|ssh[-_]?key)"
    r"(?:=|[\s,]+)(?P<quote>['\"]?)(?P<value>[^'\"\s,\])]+)(?P=quote)",
    re.IGNORECASE,
)
_SERIALIZED_ARGV_TEXT_RE = re.compile(
    r"(?P<flag_quote>['\"])(?P<flag>--?(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key|ssh[-_]?key))"
    r"(?P=flag_quote)\s*,\s*(?P<value_quote>['\"])(?P<serialized_value>.*?)(?P=value_quote)",
    re.IGNORECASE,
)
_SECRET_ARGV_FLAG_RE = re.compile(
    r"(?<![A-Za-z0-9_])(?:--?|/)(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key|ssh[-_]?key)(?![A-Za-z0-9_-])",
    re.IGNORECASE,
)
_SECRET_TEXT_PATTERNS = (
    _SECRET_QUERY_RE,
    _AUTH_RE,
    _URL_USERINFO_TEXT_RE,
    _SECRET_SOURCE_ASSIGN_TEXT_RE,
    _SECRET_ASSIGN_RE,
    _SECRET_ARGV_TEXT_RE,
    _SERIALIZED_ARGV_TEXT_RE,
)


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
        redact_next = False
        for item in value:
            if redact_next and isinstance(item, (str, int, float)):
                values.add(str(item))
            values.update(_collect_secret_values(item))
            item_text = item if isinstance(item, str) else ""
            redact_next = _is_secret_argv_flag(item_text)
        return values
    if isinstance(value, str):
        values: set[str] = set()
        for pattern in _SECRET_TEXT_PATTERNS:
            for match in pattern.finditer(value):
                groups = match.groupdict()
                candidate = (
                    groups.get("value")
                    or groups.get("quoted_value")
                    or groups.get("bare_value")
                    or groups.get("query_value")
                    or groups.get("auth_value")
                    or groups.get("url_password")
                    or groups.get("serialized_value")
                )
                if not candidate:
                    candidate = next((item for item in reversed(match.groups()) if item), None)
                if candidate and candidate != "<redacted>":
                    values.add(candidate)
        return values
    return set()


def _redact_string(value: str, secrets: set[str]) -> str:
    def redact_assignment(match: re.Match[str]) -> str:
        prefix, quote, quoted_value, bare_value = match.groups()
        if quote:
            return f"{prefix}{quote}<redacted>{quote}"
        return f"{prefix}<redacted>"

    result = _redact_argv_forms(value)
    for secret in sorted((item for item in secrets if item), key=len, reverse=True):
        result = result.replace(secret, "<redacted>")
    result = _SECRET_QUERY_RE.sub(r"\1<redacted>", result)
    result = _SECRET_ASSIGN_RE.sub(redact_assignment, result)
    result = _AUTH_RE.sub(r"\1<redacted>", result)
    result = _URL_USERINFO_TEXT_RE.sub(r"\1<redacted>@", result)
    result = _SECRET_SOURCE_ASSIGN_TEXT_RE.sub(redact_assignment, result)
    result = _SECRET_ARGV_TEXT_RE.sub(
        lambda match: match.group(0)[:match.start("value") - match.start()] + "<redacted>" + (match.group("quote") or ""),
        result,
    )
    result = _SERIALIZED_ARGV_TEXT_RE.sub(
        lambda match: (
            f"{match.group('flag_quote')}{match.group('flag')}{match.group('flag_quote')}, "
            f"{match.group('value_quote')}<redacted>{match.group('value_quote')}"
        ),
        result,
    )
    return result


def _is_secret_argv_flag(value: str) -> bool:
    """Return whether one argv item is an exact protected flag token."""
    return bool(_SECRET_ARGV_FLAG_RE.fullmatch(value.strip()))


def _redact_argv_forms(value: str) -> str:
    """Fail closed for arbitrary text containing a protected argv flag.

    Captured logs are not guaranteed to be valid shell, JSON, or Python
    syntax.  A partial lexer cannot prove where an escaped, concatenated, or
    unterminated value ends, so retaining any part of that record could leak a
    value or suffix.  Replace the complete record instead.  The evidence-tree
    caller records this replacement as a protected-input blocker.
    """
    if _SECRET_ARGV_FLAG_RE.search(value):
        return "<redacted>"
    return value


def _has_argv_secret_form(value: str) -> bool:
    """Return whether a sensitive argv flag has a value to protect."""
    return _redact_argv_forms(value) != value


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
                redact_next = _is_secret_argv_flag(child_text)
            return result
        return item

    if key and _SECRET_KEY_RE.search(key):
        return "<redacted>"
    return sanitize(value, key)


def collect_secret_values(value: Any) -> set[str]:
    """Collect secret values for the controller's in-memory scrub boundary."""
    return _collect_secret_values(value)


_SECRET_FILENAME_RE = re.compile(
    r"(?:^\.env(?:\..*)?$|(?:^|[._-])(?:secret|credential|password|passwd|token|api[-_]?key|private[-_]?key|ssh[-_]?key)(?:$|[._-]))",
    re.IGNORECASE,
)


def _is_secret_filename(part: str) -> bool:
    """Recognise dotenv names and secret-labelled path components exactly."""
    lowered = part.lower()
    return bool(
        _SECRET_FILENAME_RE.search(part)
        or lowered == ".envrc"
        or lowered.endswith(".env")
        or lowered.startswith(".env.")
        or lowered.startswith(".env-")
    )


def _remove_protected_path(path: Path, relative: str, blockers: list[str], quarantined: list[str], reason: str) -> None:
    """Remove an unsafe evidence input when possible, but always retain a blocker."""
    try:
        path.unlink()
        quarantined.append(relative)
    except (OSError, RuntimeError) as exc:
        blockers.append(f"protected-input quarantine failed: {relative}: {type(exc).__name__}")
        return
    blockers.append(f"protected-input {reason}: {relative}")


def sanitize_evidence_tree(
    evidence_dir: os.PathLike[str] | str,
    known_secrets: Iterable[str] = (),
) -> dict[str, Any]:
    """Scan every evidence file/name and fail closed on unsafe publication.

    Text containing quoted assignments, URL credentials, query credentials, or
    argv-style secrets is rewritten only when the rewrite succeeds and the
    result is demonstrably clean.  Binary payloads remain allowed when no known
    secret is present; opaque/unreadable or secret-bearing paths are rejected
    and removed when possible.  A blocker is returned even after quarantine so
    callers cannot publish an apparently complete bundle.
    """
    root = Path(evidence_dir).resolve()
    if not root.is_dir():
        return {"ok": False, "scanned": [], "quarantined": [], "blockers": [f"protected-input evidence root is unavailable: {root}"]}
    secrets = {item for item in known_secrets if isinstance(item, str) and item}
    blockers: list[str] = []
    scanned: list[str] = []
    quarantined: list[str] = []
    rewritten_paths: list[str] = []
    for path in sorted(root.rglob("*")):
        relative = path.relative_to(root).as_posix()
        if path.is_symlink():
            _remove_protected_path(path, relative, blockers, quarantined, "unsafe evidence link")
            continue
        try:
            is_file = path.is_file()
        except (OSError, RuntimeError) as exc:
            blockers.append(f"protected-input evidence stat failed: {relative}: {type(exc).__name__}")
            continue
        if not is_file:
            continue
        scanned.append(relative)
        if any(_is_secret_filename(part) for part in Path(relative).parts):
            _remove_protected_path(path, relative, blockers, quarantined, "secret-bearing filename")
            continue
        try:
            data = path.read_bytes()
        except (OSError, RuntimeError) as exc:
            _remove_protected_path(path, relative, blockers, quarantined, f"evidence unreadable ({type(exc).__name__})")
            continue
        if b"\0" in data and any(secret.encode("utf-8") in data for secret in secrets):
            _remove_protected_path(path, relative, blockers, quarantined, "cannot safely preserve opaque evidence")
            continue
        try:
            text = data.decode("utf-8")
        except UnicodeDecodeError:
            if any(secret.encode("utf-8") in data for secret in secrets):
                _remove_protected_path(path, relative, blockers, quarantined, "cannot safely preserve opaque evidence")
            continue
        scrubbed = _redact_string(text, secrets)
        if scrubbed != text:
            rewritten_paths.append(relative)
            try:
                path.write_text(scrubbed, encoding="utf-8")
                # Re-open the rewritten path so a partial or intercepted write
                # cannot be mistaken for successful redaction.
                rewritten_text = path.read_text(encoding="utf-8")
            except (OSError, RuntimeError, UnicodeError) as exc:
                _remove_protected_path(path, relative, blockers, quarantined, f"cannot safely rewrite evidence ({type(exc).__name__})")
                continue
            if rewritten_text != scrubbed or any(secret in rewritten_text for secret in secrets):
                _remove_protected_path(path, relative, blockers, quarantined, "secret remains in evidence")
                continue
            # Rewriting proves only that the published copy is scrubbed.  It
            # does not prove that the original execution input was safe.
            blockers.append(f"protected-input rewritten: {relative}")
        elif any(secret in text for secret in secrets):
            _remove_protected_path(path, relative, blockers, quarantined, "secret remains in evidence")
    return {"ok": not blockers, "scanned": scanned, "rewritten": rewritten_paths, "quarantined": quarantined, "blockers": blockers}


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
        "evidence_dir": str(evidence_root.resolve()),
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


_PLACEHOLDER_IDENTITY_RE = re.compile(
    r"^(?:$|<[^>]+>|unknown|unavailable|not[-_ ]?available|n/?a|none|null|placeholder|todo|tbd|dummy|example)$",
    re.IGNORECASE,
)
_RUNTIME_TYPED_NUMERIC_FIELDS = frozenset({"ldd_returncode", "library_count"})


def _require_meaningful_identity(
    value: Any,
    field: str,
    *,
    typed_numeric_fields: frozenset[str] = frozenset(),
    typed_empty_fields: frozenset[str] = frozenset(),
) -> None:
    """Reject empty, placeholder, and untyped identity structures.

    JSON numbers and booleans are valid *options* only when their containing
    record gives them an explicit type/meaning.  They are never meaningful
    identity values by themselves; accepting them lets ``{"id": false}`` or
    ``{"version": 1}`` masquerade as provenance.  A caller may allow a
    closed set of known, integer-valued fields for a typed identity schema.
    """
    if value is None or isinstance(value, bool) or isinstance(value, (int, float)):
        raise ValueError(f"{field} identity is missing or has an invalid type")
    if isinstance(value, str):
        if _PLACEHOLDER_IDENTITY_RE.fullmatch(value.strip()):
            raise ValueError(f"{field} identity is a placeholder or unavailable")
        return
    if isinstance(value, Mapping):
        if not value:
            raise ValueError(f"{field} identity is missing")
        for key, item in value.items():
            if not isinstance(key, str) or not key.strip():
                raise ValueError(f"{field} contains an invalid key")
            if isinstance(item, (Mapping, list, tuple)):
                if key in typed_empty_fields and isinstance(item, (list, tuple)) and not item:
                    continue
                _require_meaningful_identity(
                    item,
                    f"{field}.{key}",
                    typed_numeric_fields=typed_numeric_fields,
                    typed_empty_fields=typed_empty_fields,
                )
            elif key in typed_numeric_fields and isinstance(item, int) and not isinstance(item, bool):
                continue
            elif item is None or isinstance(item, (bool, int, float)):
                raise ValueError(f"{field}.{key} identity has an invalid type")
            elif isinstance(item, str) and _PLACEHOLDER_IDENTITY_RE.fullmatch(item.strip()):
                raise ValueError(f"{field}.{key} identity is a placeholder or unavailable")
            elif not isinstance(item, str):
                raise ValueError(f"{field}.{key} identity has an invalid type")
        return
    if isinstance(value, (list, tuple)):
        if not value:
            raise ValueError(f"{field} identity is missing")
        for number, item in enumerate(value):
            _require_meaningful_identity(
                item,
                f"{field}[{number}]",
                typed_numeric_fields=typed_numeric_fields,
                typed_empty_fields=typed_empty_fields,
            )
        return
    raise ValueError(f"{field} identity has an invalid type")


def _require_text(value: Any, field: str, *, absolute: bool = False) -> str:
    if not isinstance(value, str) or not value.strip() or _PLACEHOLDER_IDENTITY_RE.fullmatch(value.strip()):
        raise ValueError(f"{field} is missing, placeholder, or invalid")
    if absolute and not Path(value).is_absolute():
        raise ValueError(f"{field} must be an absolute path")
    return value


def _require_nonempty_string_list(value: Any, field: str) -> list[str]:
    if not isinstance(value, (list, tuple)) or not value:
        raise ValueError(f"{field} must be a non-empty string array")
    result = []
    for index, item in enumerate(value):
        result.append(_require_text(item, f"{field}[{index}]"))
    return result


def _require_index_identity(snapshot: Mapping[str, Any], field: str) -> tuple[str, list[dict[str, Any]]]:
    index = snapshot.get("index")
    if not isinstance(index, Mapping) or index.get("captured") is not True:
        raise ValueError(f"{field} index identity is incomplete")
    entries = index.get("entries")
    if not isinstance(entries, list) or not entries:
        raise ValueError(f"{field} index entries are required")
    validated: list[dict[str, Any]] = []
    for number, entry in enumerate(entries):
        if not isinstance(entry, Mapping):
            raise ValueError(f"{field} index entry {number} is invalid")
        path = _require_text(entry.get("path"), f"{field} index entry {number} path")
        mode = entry.get("mode")
        blob_id = entry.get("blob_id")
        stage = entry.get("stage")
        if not re.fullmatch(r"[0-7]{6}", mode or ""):
            raise ValueError(f"{field} index entry {number} mode is invalid")
        if not isinstance(blob_id, str) or not re.fullmatch(r"[0-9a-fA-F]{40,64}", blob_id):
            raise ValueError(f"{field} index entry {number} blob_id is invalid")
        if isinstance(stage, bool) or not isinstance(stage, int) or stage not in {0, 1, 2, 3}:
            raise ValueError(f"{field} index entry {number} stage is invalid")
        validated.append({"path": path, "mode": mode, "blob_id": blob_id.lower(), "stage": stage})
    index_hash = _normalise_hash(index.get("sha256"), f"{field} index sha256")
    if snapshot.get("index_sha256") != index_hash:
        raise ValueError(f"{field} index identity is inconsistent")
    if snapshot.get("index_entries") != entries:
        raise ValueError(f"{field} index entries are inconsistent")
    return index_hash, validated


def _source_index_identity(snapshot: Mapping[str, Any], field: str) -> tuple[str, list[Any]]:
    """Return a validated, independently comparable source/index identity."""
    return _require_index_identity(snapshot, field)


def _execution_input_blockers(value: Any, location: str = "input") -> list[str]:
    """Return stable locations of secrets before they enter evidence output."""
    blockers: list[str] = []
    if isinstance(value, Mapping):
        for key, item in value.items():
            key_text = str(key)
            child_location = f"{location}.{key_text}"
            if _SECRET_KEY_RE.search(key_text) and isinstance(item, (str, int, float)) and str(item):
                blockers.append(f"protected-input execution secret: {child_location}")
            blockers.extend(_execution_input_blockers(item, child_location))
        return blockers
    if isinstance(value, (list, tuple)):
        secret_flag = False
        for index, item in enumerate(value):
            item_location = f"{location}[{index}]"
            if secret_flag and isinstance(item, (str, int, float)) and str(item):
                blockers.append(f"protected-input argv secret: {item_location}")
            blockers.extend(_execution_input_blockers(item, item_location))
            item_text = item if isinstance(item, str) else ""
            secret_flag = bool(_SECRET_KEY_RE.search(item_text.lstrip("-/"))) or bool(
                re.search(r"(?:password|passwd|secret|token|credential|private[-_]?key|api[-_]?key|ssh[-_]?key)=?$", item_text, re.IGNORECASE)
            )
        return blockers
    if isinstance(value, str):
        if any(_is_secret_filename(part) for part in Path(value).parts):
            blockers.append(f"protected-input secret-bearing filename: {location}")
        if any(pattern.search(value) for pattern in _SECRET_TEXT_PATTERNS) or _has_argv_secret_form(value):
            blockers.append(f"protected-input secret-bearing execution text: {location}")
    return blockers


def _validate_build_options(options: Mapping[str, Any]) -> None:
    """Validate typed build options; falsey IDs are not identities."""
    if not isinstance(options, Mapping) or not options:
        raise ValueError("complete build options identity is required")
    if "id" in options:
        _require_meaningful_identity(options["id"], "build options.id")
    variant = options.get("variant", options.get("build_type"))
    _require_text(variant, "build options variant")
    if "jobs" in options and (isinstance(options["jobs"], bool) or not isinstance(options["jobs"], int) or options["jobs"] <= 0):
        raise ValueError("build options jobs must be a positive integer")
    if "configure_only" in options and not isinstance(options["configure_only"], bool):
        raise ValueError("build options configure_only must be boolean")
    if "cmake_args" in options:
        _require_nonempty_string_list(options["cmake_args"], "build options cmake_args")


def _normalise_loader_output(output: str) -> str:
    """Remove ASLR addresses while retaining the loader's concrete mapping."""
    return re.sub(r"0x[0-9a-fA-F]+", "0xADDR", output)


def _runtime_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    try:
        return subprocess.run(command, capture_output=True, text=True, timeout=10, check=False)
    except (OSError, subprocess.SubprocessError) as exc:
        raise ValueError(f"runtime identity command unavailable: {exc}") from exc


def _runtime_output(run: subprocess.CompletedProcess[str]) -> str:
    return (run.stdout or "") + (run.stderr or "")


def _is_static_ldd_output(returncode: int, output: str) -> bool:
    return returncode != 0 and output.strip().lower().endswith("not a dynamic executable")


def _validate_runtime_record(
    record: Mapping[str, Any],
    field: str,
    expected_binary_hash: str | None = None,
    expected_binary_path: str | None = None,
) -> None:
    """Validate dynamic ``ldd`` or independently proven static ELF identity."""
    if not isinstance(record, Mapping) or not record:
        raise ValueError(f"{field} identity is missing")
    if record.get("status") != "ok":
        raise ValueError(f"{field} is unavailable or incomplete")
    hashes = [name for name in ("output_sha256", "ldd_sha256", "sha256") if name in record]
    if len(hashes) != 1:
        raise ValueError(f"{field} must carry exactly one runtime output hash")
    _normalise_hash(record[hashes[0]], f"{field} {hashes[0]}")
    binary_hash = record.get("binary_sha256")
    if not isinstance(binary_hash, str):
        raise ValueError(f"{field} must carry the launched binary hash")
    binary_hash = _normalise_hash(binary_hash, f"{field} binary_sha256")
    if expected_binary_hash is not None and binary_hash != expected_binary_hash:
        raise ValueError(f"{field} binary hash diverges from launched executable")
    linkage = record.get("linkage", "dynamic")
    if linkage not in {"dynamic", "static"}:
        raise ValueError(f"{field} linkage mode is ambiguous")
    expected_path = str(Path(expected_binary_path).resolve(strict=False)) if expected_binary_path is not None else None
    recorded_path = record.get("binary_path")
    if linkage == "static":
        if not isinstance(recorded_path, str) or not recorded_path:
            raise ValueError(f"{field} static executable path identity is missing")
    if recorded_path is not None:
        if not isinstance(recorded_path, str) or not recorded_path:
            raise ValueError(f"{field} executable path identity is invalid")
        if expected_path is not None and str(Path(recorded_path).resolve(strict=False)) != expected_path:
            raise ValueError(f"{field} executable path diverges from launched executable")
    libraries = record.get("libraries")
    if linkage == "static":
        if libraries != [] or record.get("library_count") != 0:
            raise ValueError(f"{field} static identity contains library dependencies")
        if expected_path is None:
            raise ValueError(f"{field} static executable path is required")
        launched_path = expected_path
        try:
            observed_binary = file_identity(launched_path)
        except (FileNotFoundError, OSError, ValueError) as exc:
            raise ValueError(f"{field} launched executable is unavailable: {exc}") from exc
        if observed_binary["sha256"] != binary_hash:
            raise ValueError(f"{field} binary hash diverges from launched executable")
        file_output = record.get("file_output")
        header_output = record.get("elf_header_output")
        program_output = record.get("elf_program_headers_output")
        if not all(isinstance(value, str) and value for value in (file_output, header_output, program_output)):
            raise ValueError(f"{field} static ELF identity is incomplete")
        for label, output, digest in (
            ("file", file_output, "file_output_sha256"),
            ("ELF header", header_output, "elf_header_sha256"),
            ("ELF program headers", program_output, "elf_program_headers_sha256"),
        ):
            if _normalise_hash(record.get(digest), f"{field} {digest}") != hashlib.sha256(output.encode()).hexdigest():
                raise ValueError(f"{field} {label} identity hash diverges")
        observed_file = _runtime_output(_runtime_command(["file", "-b", launched_path]))
        observed_header_run = _runtime_command(["readelf", "-h", launched_path])
        observed_header = _runtime_output(observed_header_run)
        observed_program_run = _runtime_command(["readelf", "-l", launched_path])
        observed_program = _runtime_output(observed_program_run)
        if observed_file != file_output or observed_header != header_output or observed_program != program_output:
            raise ValueError(f"{field} static ELF identity diverges")
        if observed_header_run.returncode != 0 or observed_program_run.returncode != 0 or not observed_file.lstrip().startswith("ELF"):
            raise ValueError(f"{field} static ELF identity is unavailable")
        if "statically linked" not in observed_file.lower() and "static-pie linked" not in observed_file.lower():
            raise ValueError(f"{field} static file identity is contradictory")
        if "INTERP" in observed_program:
            raise ValueError(f"{field} static ELF identity is contradictory")
        ldd_run = _runtime_command(["ldd", launched_path])
        ldd_output = _runtime_output(ldd_run)
        if ldd_run.returncode != record.get("ldd_returncode") or ldd_output != record.get("ldd_output"):
            raise ValueError(f"{field} static ldd identity diverges")
        if not _is_static_ldd_output(ldd_run.returncode, ldd_output):
            raise ValueError(f"{field} static ldd evidence is contradictory")
        if record[hashes[0]] != hashlib.sha256(ldd_output.encode()).hexdigest():
            raise ValueError(f"{field} static ldd output hash diverges")
        return
    if not isinstance(libraries, list) or not libraries:
        raise ValueError(f"{field} library list is empty or invalid")
    for number, library in enumerate(libraries):
        _require_text(library, f"{field}.libraries[{number}]")
        if _PLACEHOLDER_IDENTITY_RE.fullmatch(library.strip()):
            raise ValueError(f"{field}.libraries[{number}] is synthetic or unavailable")
    if expected_binary_path is not None:
        try:
            observed_binary = file_identity(expected_binary_path)
        except (FileNotFoundError, OSError, ValueError) as exc:
            raise ValueError(f"{field} launched executable is unavailable: {exc}") from exc
        if observed_binary["sha256"] != binary_hash:
            raise ValueError(f"{field} binary hash diverges from launched executable")
        observed_run = _runtime_command(["ldd", expected_binary_path])
        observed_output = _runtime_output(observed_run)
        if observed_run.returncode != 0 or not observed_output.strip():
            raise ValueError(f"{field} ldd evidence is unavailable")
        stable_output = _normalise_loader_output(observed_output)
        observed_hash = hashlib.sha256(stable_output.encode()).hexdigest()
        if record[hashes[0]] != observed_hash:
            raise ValueError(f"{field} ldd output hash diverges")
        observed_libraries = sorted(line.strip() for line in stable_output.splitlines() if line.strip())
        if "library_count" in record:
            if record.get("library_count") != len(observed_libraries) or libraries != observed_libraries:
                raise ValueError(f"{field} library evidence is incomplete or diverges")
        elif not set(libraries) <= set(observed_libraries):
            raise ValueError(f"{field} library evidence diverges")


def _receipt_records(value: Any, field: str) -> dict[str, Mapping[str, Any]]:
    if isinstance(value, Mapping):
        result: dict[str, Mapping[str, Any]] = {}
        for name, record in value.items():
            if not isinstance(record, Mapping):
                raise ValueError(f"{field} entries must be mappings")
            result[str(name)] = record
        return result
    if isinstance(value, list):
        result = {}
        for number, record in enumerate(value):
            if not isinstance(record, Mapping) or not isinstance(record.get("name"), str):
                raise ValueError(f"{field}[{number}] requires a name")
            result[record["name"]] = record
        return result
    raise ValueError(f"{field} must be a mapping or array")


def _receipt_command_digest(command: Mapping[str, Any]) -> str:
    payload = {
        "argv": command.get("argv"),
        "cwd": command.get("cwd"),
        "returncode": command.get("returncode"),
        "stdout_sha256": command.get("stdout_sha256"),
        "stderr_sha256": command.get("stderr_sha256"),
    }
    return hashlib.sha256(json.dumps(payload, sort_keys=True, separators=(",", ":")).encode("utf-8")).hexdigest()


def validate_build_receipt(manifest: Mapping[str, Any], executables: Mapping[str, Any] | Sequence[Mapping[str, Any]]) -> bool:
    """Require a receipt that binds commands, output paths, and output hashes.

    A recipe's ``executed`` flag is only a claim.  The receipt is the captured
    command/output ledger produced by ``ng_observability.build_variant``; every
    launched executable must be one of its captured outputs and every command
    must carry a successful, self-consistent execution record.
    """
    recipe = manifest.get("recipe", manifest.get("build_recipe"))
    receipt = manifest.get("build_receipt")
    if not isinstance(recipe, Mapping) or not isinstance(receipt, Mapping):
        raise ValueError("build recipe receipt is required")
    commands = recipe.get("commands", recipe.get("command"))
    returncodes = recipe.get("returncodes")
    receipt_commands = receipt.get("commands")
    if not isinstance(commands, list) or not commands or not isinstance(returncodes, list) or len(returncodes) != len(commands):
        raise ValueError("build recipe commands/returncodes are incomplete")
    if not isinstance(receipt_commands, list) or len(receipt_commands) != len(commands):
        raise ValueError("build receipt command coverage is incomplete")
    working_directory = _require_text(recipe.get("working_directory"), "build recipe working_directory", absolute=True)
    if receipt.get("schema_version") != 1 or receipt.get("working_directory") != working_directory:
        raise ValueError("build receipt identity is incomplete")
    output_directory = _require_text(receipt.get("output_directory"), "build receipt output_directory", absolute=True)
    output_root = Path(output_directory).resolve()
    if not output_root.is_dir():
        raise ValueError("build receipt output_directory is not a directory")
    if manifest.get("output") is not None and str(Path(manifest["output"]).resolve()) != str(output_root):
        raise ValueError("build receipt output directory diverges")
    source_directory = manifest.get("source")
    if source_directory is not None:
        source_directory = _require_text(source_directory, "build source", absolute=True)
        if str(Path(source_directory).resolve()) != str(Path(working_directory).resolve()):
            raise ValueError("build recipe working directory is unrelated to source")
        command_argv = [item.get("argv") for item in receipt_commands if isinstance(item, Mapping)]
        configured = any(
            isinstance(argv, list) and "cmake" in {str(argv[0])} and "-S" in argv and source_directory in argv and "-B" in argv and output_directory in argv
            for argv in command_argv
        )
        building = any(
            isinstance(argv, list) and "cmake" in {str(argv[0])} and "--build" in argv and output_directory in argv
            for argv in command_argv
        )
        if not configured or (manifest.get("options", {}).get("configure_only") is not True and not building):
            raise ValueError("build recipe commands are not linked to the captured source/output")
    for number, (recipe_command, returncode, captured) in enumerate(zip(commands, returncodes, receipt_commands)):
        if not isinstance(captured, Mapping) or captured.get("argv") != recipe_command or captured.get("cwd") != working_directory:
            raise ValueError(f"build receipt command {number} diverges from recipe")
        if captured.get("returncode") != returncode or returncode != 0:
            raise ValueError(f"build receipt command {number} did not complete successfully")
        _normalise_hash(captured.get("stdout_sha256"), f"build receipt command {number} stdout_sha256")
        _normalise_hash(captured.get("stderr_sha256"), f"build receipt command {number} stderr_sha256")
        command_hash = _normalise_hash(captured.get("command_sha256"), f"build receipt command {number} command_sha256")
        if command_hash != _receipt_command_digest(captured):
            raise ValueError(f"build receipt command {number} hash diverges")
    receipt_hash = receipt.get("receipt_sha256")
    if receipt_hash is not None:
        canonical = dict(receipt)
        canonical.pop("receipt_sha256", None)
        expected = hashlib.sha256(json.dumps(canonical, sort_keys=True, separators=(",", ":")).encode("utf-8")).hexdigest()
        if _normalise_hash(receipt_hash, "build receipt receipt_sha256") != expected:
            raise ValueError("build receipt hash diverges")

    actual = _binary_records(executables, "executables") if executables else {}
    outputs = _receipt_records(receipt.get("outputs"), "build receipt outputs")
    if not outputs:
        raise ValueError("build receipt output coverage is empty")
    for role, record in actual.items():
        output = outputs.get(role)
        if output is None:
            raise ValueError(f"build receipt output coverage is missing for {role}")
        expected_path, expected_hash = _binary_identity(role, record, "executables")
        if str(Path(output.get("path", "")).resolve(strict=False)) != expected_path:
            raise ValueError(f"build receipt output path diverges for {role}")
        if _normalise_hash(output.get("sha256"), f"build receipt output {role} sha256") != expected_hash:
            raise ValueError(f"build receipt output hash diverges for {role}")
        if isinstance(record.get("size"), int) and output.get("size") != record["size"]:
            raise ValueError(f"build receipt output size diverges for {role}")
        if not Path(expected_path).is_file() or _sha256(Path(expected_path)) != expected_hash:
            raise ValueError(f"build receipt output is not the captured file for {role}")
        resolved_output = Path(expected_path).resolve()
        try:
            resolved_output.relative_to(output_root)
        except ValueError as exc:
            raise ValueError(f"build receipt output escapes output directory for {role}") from exc
    return True


def validate_build_linkage(
    manifest: Mapping[str, Any],
    source_head: str | None,
    executables: Mapping[str, Any] | Sequence[Mapping[str, Any]],
    source_state: Mapping[str, Any] | None = None,
    manifest_evidence: Mapping[str, Any] | None = None,
    require_receipt: bool | None = None,
) -> bool:
    """Validate that a build manifest links the supplied source and binaries.

    A valid manifest must carry the complete source/build identity and a
    separately preserved manifest record.  Every launched role is matched to
    one stable path+hash identity; basename-only ambiguity is rejected.
    """

    if not isinstance(manifest, Mapping) or not manifest:
        raise ValueError("manifest is required and must not be empty")
    if require_receipt is None:
        require_receipt = manifest.get("mode") == "local"
    if not isinstance(require_receipt, bool):
        raise ValueError("require_receipt must be boolean")
    preliminary_value = manifest.get("binaries")
    if preliminary_value:
        preliminary = _binary_records(preliminary_value, "manifest")
        seen_basenames: dict[str, tuple[str, str]] = {}
        for name, record in preliminary.items():
            try:
                identity = _binary_identity(name, record, "manifest")
            except ValueError:
                continue
            basename = Path(identity[0]).name
            prior = seen_basenames.get(basename)
            if prior is not None and prior != identity:
                raise ValueError(f"executable basename/path collision for {basename}")
            seen_basenames[basename] = identity
    schema_version = manifest.get("schema_version")
    if schema_version is not None and (isinstance(schema_version, bool) or not isinstance(schema_version, int) or schema_version < 1):
        raise ValueError("build manifest schema_version is invalid")

    recipe = manifest.get("recipe", manifest.get("build_recipe"))
    if not isinstance(recipe, Mapping) or not recipe:
        raise ValueError("complete build recipe identity is required")
    recipe_commands = recipe.get("commands", recipe.get("command"))
    if not isinstance(recipe_commands, (list, tuple)) or not recipe_commands:
        raise ValueError("complete build recipe commands are required")
    if all(isinstance(item, str) for item in recipe_commands):
        _require_nonempty_string_list(recipe_commands, "build recipe command")
    else:
        for number, command in enumerate(recipe_commands):
            _require_nonempty_string_list(command, f"build recipe command {number}")
    if recipe.get("executed") is not True:
        raise ValueError("build recipe execution evidence is required")
    if "commands" in manifest and manifest["commands"] != recipe_commands:
        raise ValueError("manifest recipe commands diverge")
    returncodes = recipe.get("returncodes")
    if not isinstance(returncodes, list) or len(returncodes) != len(recipe_commands) or any(
        isinstance(code, bool) or not isinstance(code, int) or code != 0 for code in returncodes
    ):
        raise ValueError("build recipe returncode evidence is incomplete")
    _require_text(recipe.get("working_directory"), "build recipe working_directory", absolute=True)

    toolchain = manifest.get("toolchain", manifest.get("toolchain_identity"))
    options = manifest.get("options", manifest.get("build_options"))
    if not isinstance(toolchain, Mapping) or not toolchain:
        raise ValueError("complete compiler/toolchain identity is required")
    _require_meaningful_identity(toolchain, "toolchain")
    if toolchain.get("status") != "ok":
        raise ValueError("toolchain identity is unavailable or incomplete")
    compiler = _require_text(toolchain.get("compiler"), "toolchain.compiler", absolute=True)
    compiler_path = Path(compiler)
    if not compiler_path.is_file() or not os.access(compiler_path, os.X_OK):
        raise ValueError("toolchain compiler is not an executable file")
    version = _require_text(toolchain.get("version"), "toolchain.version")
    version_hash = _normalise_hash(toolchain.get("version_sha256"), "toolchain.version_sha256")
    if version_hash != hashlib.sha256(version.encode()).hexdigest():
        raise ValueError("toolchain version evidence hash diverges")
    try:
        version_run = subprocess.run([str(compiler_path), "--version"], capture_output=True, text=True, timeout=10, check=False)
    except (OSError, subprocess.SubprocessError) as exc:
        raise ValueError(f"toolchain version evidence unavailable: {exc}") from exc
    observed_version = (version_run.stdout or version_run.stderr).splitlines()
    if version_run.returncode != 0 or not observed_version or observed_version[0] != version:
        raise ValueError("toolchain version evidence does not match compiler")
    if not isinstance(options, Mapping) or not options:
        raise ValueError("complete build options identity is required")
    _validate_build_options(options)

    if require_receipt or "build_receipt" in manifest or "source" in manifest or "output" in manifest:
        validate_build_receipt(manifest, executables)
    else:
        # Keep old schema-1 callers readable, but do not let an arbitrary
        # fixture command masquerade as a NovaGenesis build recipe.
        recipe_commands = manifest.get("recipe", {}).get("commands", manifest.get("recipe", {}).get("command", []))
        command_tokens = [str(item) for item in recipe_commands] if isinstance(recipe_commands, list) else []
        if "cmake" not in command_tokens or "--build" not in command_tokens:
            nested = [token for command in recipe_commands if isinstance(command, (list, tuple)) for token in command] if isinstance(recipe_commands, list) else []
            if "cmake" not in nested or "--build" not in nested:
                raise ValueError("build recipe commands are not linked to a captured build")

    runtime_identity = manifest.get("runtime_library_identity")
    if runtime_identity is None:
        runtime_identity = manifest.get("runtime_libraries") or manifest.get("runtime_library")
    if not isinstance(runtime_identity, Mapping) or not runtime_identity:
        raise ValueError("runtime-library identity is required")
    _require_meaningful_identity(
        runtime_identity,
        "runtime-library",
        typed_numeric_fields=_RUNTIME_TYPED_NUMERIC_FIELDS,
        typed_empty_fields=frozenset({"libraries"}),
    )
    method = runtime_identity.get("method")
    _require_text(method, "runtime-library.method")
    if method not in {"ldd", "static"}:
        raise ValueError("runtime-library method is ambiguous or unsupported")
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
    source_manifest_path = Path(evidence_path)
    if source_manifest_path.is_symlink() or (source_manifest_path.exists() and not source_manifest_path.is_file()):
        raise ValueError("build manifest evidence is not a regular file")
    if source_manifest_path.is_file():
        current_manifest = file_identity(source_manifest_path)
        if current_manifest["size"] != evidence_size or current_manifest["sha256"] != manifest_evidence["sha256"]:
            raise ValueError("build manifest identity drift")
    evidence_root = manifest_evidence.get("evidence_dir")
    if isinstance(evidence_root, str) and evidence_root:
        preserved_candidate = Path(preserved_path) if Path(preserved_path).is_absolute() else Path(evidence_root) / preserved_path
        if not preserved_candidate.is_file() or preserved_candidate.is_symlink():
            raise ValueError("preserved manifest evidence is unavailable")
        current_preserved = file_identity(preserved_candidate)
        if current_preserved["size"] != preserved_size or current_preserved["sha256"] != manifest_evidence["preserved_sha256"]:
            raise ValueError("preserved manifest evidence drift")
        if require_receipt:
            try:
                preserved_manifest = json.loads(preserved_candidate.read_text(encoding="utf-8"))
            except (OSError, UnicodeError, json.JSONDecodeError) as exc:
                raise ValueError("preserved build receipt is unavailable") from exc
            if not isinstance(preserved_manifest, Mapping) or not isinstance(preserved_manifest.get("build_receipt"), Mapping):
                raise ValueError("preserved build receipt is required")
            validate_build_receipt(preserved_manifest, executables)
    elif require_receipt:
        raise ValueError("preserved build receipt evidence is required")

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
    if status and (content_snapshot.get("captured") is not True or not content_snapshot.get("entries")):
        raise ValueError("dirty source status has an empty or partial content snapshot")
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
        if source_state.get("head") and snapshot.get("head") and source_state["head"] != snapshot["head"]:
            raise ValueError("source HEAD divergence")
        if source_state.get("tree_sha256") and snapshot["tree_sha256"] != source_state["tree_sha256"]:
            raise ValueError("source content divergence")
        if source_state.get("status") is not None and source_state.get("status") != snapshot.get("status"):
            raise ValueError("source status divergence")
        if source_state.get("submodules") is not None and source_state.get("submodules") != snapshot.get("submodules"):
            raise ValueError("source submodule divergence")
        if source_state.get("submodules_complete") is not None and source_state.get("submodules_complete") != snapshot.get("submodules_complete"):
            raise ValueError("source submodule completeness divergence")

    expected_value = manifest.get("binaries")
    if not expected_value:
        raise ValueError("binaries are required")
    expected = _binary_records(expected_value, "manifest")
    actual = _binary_records(executables, "executables") if executables else {}
    if not actual:
        raise ValueError("executables are required")
    snapshot_index_hash, snapshot_index_entries = _source_index_identity(snapshot, "manifest source")
    if not isinstance(source_state, Mapping):
        raise ValueError("independent captured source/index identity is required")
    state_index_hash, state_index_entries = _source_index_identity(source_state, "captured source")
    if state_index_hash != snapshot_index_hash or state_index_entries != snapshot_index_entries:
        raise ValueError("manifest source/index identity divergence")
    if source_state.get("head") != snapshot.get("head"):
        raise ValueError("manifest source HEAD identity divergence")
    if source_state.get("tree_sha256") != snapshot.get("tree_sha256"):
        raise ValueError("manifest source tree identity divergence")

    expected_identities = {
        name: _binary_identity(name, record, "manifest")
        for name, record in expected.items()
    }
    runtime_records = runtime_identity.get("binaries")
    if not isinstance(runtime_records, Mapping) or not runtime_records:
        raise ValueError("runtime-library coverage is incomplete")
    for role, record in actual.items():
        coverage = runtime_records.get(role)
        if coverage is None:
            raise ValueError(f"runtime-library coverage is missing for {role}")
        if not isinstance(coverage, Mapping):
            raise ValueError(f"runtime-library coverage is invalid for {role}")
        if method == "static" and coverage.get("linkage") != "static":
            raise ValueError(f"runtime-library coverage is not static for {role}")
        if method == "ldd" and coverage.get("linkage") == "static":
            raise ValueError(f"runtime-library coverage contradicts dynamic mode for {role}")
        expected_record = expected_identities.get(role)
        expected_hash = expected_record[1] if expected_record else None
        expected_path = expected_record[0] if expected_record else None
        _validate_runtime_record(coverage, f"runtime-library.{role}", expected_hash, expected_path)
        if _identity_unavailable(coverage):
            raise ValueError(f"runtime-library coverage is unavailable for {role}")
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
    execution_blockers = _execution_input_blockers(
        {"plan": _expand_value(plan, variables), "config": config},
        "execution",
    )
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
                require_receipt=True,
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
    protected_inputs = execution_blockers + (list(source["content_snapshot"].get("errors", [])) if source["content_snapshot"].get("errors") else [])
    protected_input_blockers = list(execution_blockers)
    protected_input_blockers.extend(
        item for item in protected_inputs
        if str(item).startswith("protected-input") and item not in protected_input_blockers
    )
    if plan_record.get("error"):
        protected_inputs.append(f"protected-input plan snapshot unavailable: {plan_record['error']}")
    if manifest_error:
        protected_inputs.append(f"protected-input build manifest unavailable: {manifest_error}")
    for item in protected_inputs:
        if item not in protected_input_blockers and str(item).startswith("protected-input") and "unavailable" not in str(item):
            # Publication blockers represent secret-bearing inputs, not every
            # ordinary provenance incompleteness (which remains an eligibility blocker).
            if "execution" in str(item) or "dirty source content" in str(item) or "staged source content" in str(item):
                protected_input_blockers.append(item)
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
        "protected_inputs": protected_inputs,
        "protected_input_blockers": protected_input_blockers,
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
    "collect_secret_values",
    "file_identity",
    "sanitize_config",
    "sanitize_evidence_tree",
    "snapshot_files",
    "snapshot_selected_files",
    "validate_build_linkage",
    "validate_build_receipt",
]
