#!/usr/bin/env python3
"""Read-only, offline verifier for NG-ELC evidence bundles (schema v2).

The verifier never invokes the controller, accesses build/IO inputs, contacts a
network, or writes below the bundle.  ``accepted`` is deliberately derived
separately from cryptographic/inventory integrity: an intact bundle can still
contain a non-eligible trial result.
"""
from __future__ import annotations

import hashlib
import json
import os
import stat
from pathlib import Path, PurePosixPath
from typing import Any, Iterable


SCHEMA_VERSION = 2
CODE_VALID = "NGELC-VALID"
CODE_BUNDLE_MISSING = "NGELC-BUNDLE-MISSING"
CODE_TAMPERED = "NGELC-TAMPERED"
CODE_SCHEMA_OLD = "NGELC-SCHEMA-OLD"
CODE_TERMINAL_SEAL_MISSING = "NGELC-TERMINAL-SEAL-MISSING"

_MANIFEST_NAME = "manifest.json"
_TERMINAL_SEAL_NAME = "terminal-seal.json"
_SHA256_HEX_LENGTH = hashlib.sha256().digest_size * 2
_REQUIRED_FILES = frozenset(
    {
        "result.json",
        "controller-events.jsonl",
        "provenance.json",
        "workload.json",
        "oracle.json",
        "ownership.json",
        "cleanup.json",
        "preservation.json",
    }
)
_REQUIRED_PREFIXES = (
    "plan/",
    "artifacts/source/",
    "artifacts/repository/",
    "roles/",
    "inventory/",
)


class _VerificationError(Exception):
    """An expected bundle verification failure with stable human detail."""


def _report(
    code: str,
    *,
    integrity: bool = False,
    accepted: bool = False,
    bundle: Path,
    schema_version: int | None = None,
    errors: Iterable[str] = (),
    checked_files: int = 0,
) -> dict[str, Any]:
    report: dict[str, Any] = {
        "code": code,
        "integrity": integrity,
        "accepted": accepted,
        "bundle": str(bundle),
        "checked_files": checked_files,
        "errors": list(errors),
    }
    if schema_version is not None:
        report["schema_version"] = schema_version
    return report


def _safe_relative_path(value: Any) -> bool:
    """Return whether *value* is an unambiguous safe POSIX relative path."""
    if not isinstance(value, str) or not value or "\x00" in value or "\\" in value:
        return False
    path = PurePosixPath(value)
    if path.is_absolute() or path.as_posix() != value:
        return False
    return all(part not in {"", ".", ".."} for part in path.parts)


def _scan_tree(bundle: Path) -> tuple[set[str], list[str]]:
    """List regular files without following symlinks, deterministically."""
    files: set[str] = set()
    errors: list[str] = []
    pending = [bundle]
    while pending:
        directory = pending.pop()
        try:
            entries = sorted(os.scandir(directory), key=lambda entry: entry.name)
        except OSError as exc:
            errors.append(f"cannot read directory {directory.relative_to(bundle).as_posix() or '.'}: {exc}")
            continue
        for entry in entries:
            relative = Path(entry.path).relative_to(bundle).as_posix()
            try:
                entry_stat = entry.stat(follow_symlinks=False)
            except OSError as exc:
                errors.append(f"cannot stat {relative}: {exc}")
                continue
            mode = entry_stat.st_mode
            if stat.S_ISLNK(mode):
                errors.append(f"symlink is not permitted: {relative}")
            elif stat.S_ISDIR(mode):
                pending.append(Path(entry.path))
            elif stat.S_ISREG(mode):
                if not _safe_relative_path(relative):
                    errors.append(f"unsafe file path: {relative}")
                else:
                    files.add(relative)
            else:
                errors.append(f"non-regular evidence entry: {relative}")
    return files, sorted(errors)


def _read_stable(path: Path) -> bytes:
    """Read a regular file through an O_NOFOLLOW descriptor and re-stat it."""
    flags = os.O_RDONLY | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0)
    try:
        fd = os.open(path, flags)
    except OSError as exc:
        raise _VerificationError(f"cannot open {path.name}: {exc}") from exc
    try:
        before = os.fstat(fd)
        if not stat.S_ISREG(before.st_mode):
            raise _VerificationError(f"not a regular file: {path.name}")
        chunks: list[bytes] = []
        while True:
            chunk = os.read(fd, 1024 * 1024)
            if not chunk:
                break
            chunks.append(chunk)
        after = os.fstat(fd)
        identity = (before.st_dev, before.st_ino, before.st_size, before.st_mtime_ns)
        final_identity = (after.st_dev, after.st_ino, after.st_size, after.st_mtime_ns)
        if identity != final_identity:
            raise _VerificationError(f"file changed while reading: {path.name}")
        data = b"".join(chunks)
        if len(data) != after.st_size:
            raise _VerificationError(f"file size changed while reading: {path.name}")
        return data
    except OSError as exc:
        raise _VerificationError(f"cannot read {path.name}: {exc}") from exc
    finally:
        os.close(fd)


def _digest_and_size(path: Path) -> tuple[int, str]:
    data = _read_stable(path)
    return len(data), hashlib.sha256(data).hexdigest()


def _is_sha256(value: Any) -> bool:
    return isinstance(value, str) and len(value) == _SHA256_HEX_LENGTH and all(
        character in "0123456789abcdef" for character in value
    )


def _parse_object(data: bytes, label: str) -> dict[str, Any]:
    try:
        value = json.loads(data.decode("utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        raise _VerificationError(f"{label} is not valid UTF-8 JSON: {exc}") from exc
    if not isinstance(value, dict):
        raise _VerificationError(f"{label} must contain a JSON object")
    return value


def _manifest_entries(manifest: dict[str, Any]) -> dict[str, dict[str, Any]]:
    entries = manifest.get("files")
    if not isinstance(entries, list):
        raise _VerificationError("manifest.files must be an array")
    normalized: dict[str, dict[str, Any]] = {}
    for number, entry in enumerate(entries):
        if not isinstance(entry, dict) or set(entry) != {"path", "size", "sha256"}:
            raise _VerificationError(f"manifest.files[{number}] has an invalid shape")
        relative = entry["path"]
        if not _safe_relative_path(relative):
            raise _VerificationError(f"manifest contains unsafe path: {relative!r}")
        if relative in {_MANIFEST_NAME, _TERMINAL_SEAL_NAME}:
            raise _VerificationError(f"manifest must exclude {relative}")
        size = entry["size"]
        if isinstance(size, bool) or not isinstance(size, int) or size < 0:
            raise _VerificationError(f"manifest size is invalid for {relative}")
        if not _is_sha256(entry["sha256"]):
            raise _VerificationError(f"manifest sha256 is invalid for {relative}")
        if relative in normalized:
            raise _VerificationError(f"manifest contains duplicate path: {relative}")
        normalized[relative] = entry
    return normalized


def _check_coverage(actual: set[str], listed: set[str]) -> list[str]:
    errors: list[str] = []
    missing = sorted(listed - actual)
    extra = sorted(actual - listed - {_MANIFEST_NAME, _TERMINAL_SEAL_NAME})
    if missing:
        errors.append("manifest lists missing files: " + ", ".join(missing))
    if extra:
        errors.append("unlisted files: " + ", ".join(extra))

    for required in sorted(_REQUIRED_FILES):
        if required not in listed:
            errors.append(f"required evidence file is not covered: {required}")
    for prefix in _REQUIRED_PREFIXES:
        if not any(path.startswith(prefix) for path in listed):
            errors.append(f"required evidence area is not covered: {prefix}")
    return errors


def _accepted_from_result(result: dict[str, Any]) -> bool:
    """Derive acceptance eligibility from result content, never from integrity."""
    protected = result.get("protected_input_blockers")
    if protected not in (None, []):
        return False
    eligible = result.get("local_acceptance_eligibility", result.get("local_acceptance_eligible"))
    eligibility_record = result.get("acceptance_eligibility")
    if eligible is None and isinstance(eligibility_record, dict):
        eligible = eligibility_record.get("eligible")
    if not isinstance(eligible, bool) or not eligible:
        return False
    if result.get("exit_code") != 0:
        return False
    blockers = result.get("blockers", result.get("acceptance_blockers"))
    if blockers is not None and blockers != []:
        return False
    if blockers is None and isinstance(eligibility_record, dict):
        blockers = eligibility_record.get("blockers")
        if blockers not in ([], None):
            return False
    components = result.get("component_verdicts")
    if isinstance(components, dict):
        runtime = components.get("runtime")
        teardown = components.get("teardown")
        evidence = components.get("evidence")
    else:
        runtime = result.get("runtime_result")
        teardown = result.get("teardown_result")
        evidence = result.get("evidence_result")
    return runtime == "PASS" and teardown == "PASS" and evidence == "COMPLETE"


def verify_bundle(path: str | os.PathLike[str]) -> dict[str, Any]:
    """Verify one NG-ELC schema-v2 bundle without modifying or leaving it.

    The returned report always has ``code``, boolean ``integrity`` and boolean
    ``accepted`` fields.  ``NGELC-VALID`` means all manifest, coverage, link,
    seal, hash and size checks passed; it does not imply that the recorded trial
    was acceptance-eligible.
    """
    bundle = Path(path)
    try:
        root_stat = bundle.lstat()
    except (OSError, ValueError):
        return _report(CODE_BUNDLE_MISSING, bundle=bundle, errors=["bundle does not exist"])
    if stat.S_ISLNK(root_stat.st_mode):
        return _report(CODE_TAMPERED, bundle=bundle, errors=["bundle path must not be a symlink"])
    if not stat.S_ISDIR(root_stat.st_mode):
        return _report(CODE_BUNDLE_MISSING, bundle=bundle, errors=["bundle is not a directory"])

    actual, tree_errors = _scan_tree(bundle)
    manifest_path = bundle / _MANIFEST_NAME
    seal_path = bundle / _TERMINAL_SEAL_NAME
    if _MANIFEST_NAME not in actual:
        code = CODE_TAMPERED if tree_errors else CODE_BUNDLE_MISSING
        return _report(code, bundle=bundle, errors=["manifest.json is missing"] + tree_errors)

    try:
        manifest_bytes = _read_stable(manifest_path)
        manifest = _parse_object(manifest_bytes, "manifest.json")
    except _VerificationError as exc:
        return _report(CODE_TAMPERED, bundle=bundle, errors=[str(exc)] + tree_errors)

    manifest_version = manifest.get("schema_version")
    if isinstance(manifest_version, int) and not isinstance(manifest_version, bool) and manifest_version < SCHEMA_VERSION:
        return _report(CODE_SCHEMA_OLD, bundle=bundle, schema_version=manifest_version, errors=tree_errors)
    if manifest_version != SCHEMA_VERSION:
        return _report(CODE_TAMPERED, bundle=bundle, errors=["manifest schema_version is not 2"] + tree_errors)
    if tree_errors:
        return _report(CODE_TAMPERED, bundle=bundle, schema_version=SCHEMA_VERSION, errors=tree_errors)

    if _TERMINAL_SEAL_NAME not in actual:
        return _report(CODE_TERMINAL_SEAL_MISSING, bundle=bundle, schema_version=SCHEMA_VERSION, errors=tree_errors)

    try:
        seal = _parse_object(_read_stable(seal_path), "terminal-seal.json")
        if seal.get("schema_version", SCHEMA_VERSION) != SCHEMA_VERSION:
            raise _VerificationError("terminal-seal.json schema_version is not 2")
        seal_hash = seal.get("manifest_sha256")
        if not _is_sha256(seal_hash):
            raise _VerificationError("terminal-seal.json manifest_sha256 is invalid")
        _, actual_manifest_hash = _digest_and_size(manifest_path)
        if seal_hash != actual_manifest_hash:
            raise _VerificationError("terminal seal does not match manifest.json")
        listed = _manifest_entries(manifest)
        coverage_errors = _check_coverage(actual, set(listed))
        if coverage_errors:
            raise _VerificationError("; ".join(coverage_errors))

        hash_errors: list[str] = []
        for relative in sorted(listed):
            expected = listed[relative]
            try:
                size, digest = _digest_and_size(bundle / relative)
            except _VerificationError as exc:
                hash_errors.append(str(exc))
                continue
            if size != expected["size"]:
                hash_errors.append(f"size mismatch: {relative}")
            if digest != expected["sha256"]:
                hash_errors.append(f"sha256 mismatch: {relative}")
        if hash_errors:
            raise _VerificationError("; ".join(hash_errors))

        final_actual, final_tree_errors = _scan_tree(bundle)
        if final_tree_errors:
            raise _VerificationError("; ".join(final_tree_errors))
        final_coverage_errors = _check_coverage(final_actual, set(listed))
        if final_coverage_errors:
            raise _VerificationError("; ".join(final_coverage_errors))

        result = _parse_object(_read_stable(bundle / "result.json"), "result.json")
        if result.get("schema_version") != SCHEMA_VERSION:
            raise _VerificationError("result.json schema_version is not 2")
        accepted = _accepted_from_result(result)
        if manifest.get("protected_input_blockers"):
            accepted = False
        return _report(
            CODE_VALID,
            integrity=True,
            accepted=accepted,
            bundle=bundle,
            schema_version=SCHEMA_VERSION,
            checked_files=len(listed),
        )
    except _VerificationError as exc:
        return _report(
            CODE_TAMPERED,
            bundle=bundle,
            schema_version=SCHEMA_VERSION,
            errors=[str(exc)] + tree_errors,
        )


__all__ = [
    "CODE_BUNDLE_MISSING",
    "CODE_SCHEMA_OLD",
    "CODE_TAMPERED",
    "CODE_TERMINAL_SEAL_MISSING",
    "CODE_VALID",
    "SCHEMA_VERSION",
    "verify_bundle",
]
