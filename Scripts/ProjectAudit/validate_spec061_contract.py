#!/usr/bin/env python3
"""Read-only SPEC-061 packet validator; not the readiness checker."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import stat
import sys
from pathlib import Path
from typing import Any

from jsonschema import Draft202012Validator

ROOT = Path(__file__).resolve().parents[2]
POLICY = ROOT / "Specs/SPEC-061-ng-readiness-policy-v1.json"
REGISTRY = ROOT / "Specs/SPEC-061-ng-readiness-adapter-registry-v1.json"
REGISTRY_SCHEMA = ROOT / "Scripts/ProjectAudit/schemas/ng-readiness-adapter-registry-v1.schema.json"
RECORD_SCHEMA = ROOT / "Scripts/ProjectAudit/schemas/ng-readiness-evidence-record-v1.schema.json"
MANIFEST_SCHEMA = ROOT / "Scripts/ProjectAudit/schemas/ng-readiness-input-manifest-v1.schema.json"
REPORT_SCHEMA = ROOT / "Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json"
PROFILE = ROOT / "Scripts/ProjectAudit/schemas/ng-readiness-trusted-execution-profile-v1.json"
DEFAULT_MANIFEST = ROOT / "Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json"


def load(path: Path) -> dict:
    with path.open("r", encoding="utf-8", newline="") as fh:
        return json.load(fh)


def digest_bytes(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def canonical(value: object) -> bytes:
    # The packet's JCS-compatible JSON values are limited to UTF-8 strings,
    # integers, booleans, null, arrays and objects; sorted compact JSON is the
    # exact canonical form used by this documentation-only validator.
    return json.dumps(value, ensure_ascii=False, sort_keys=True, separators=(",", ":")).encode("utf-8")


def record_digest(record: dict) -> str:
    body = {k: v for k, v in record.items() if k != "record_digest"}
    return hashlib.sha256(canonical(body)).hexdigest()


def directory_observation(path: Path, exclusions: list[str]) -> tuple[dict, int, str]:
    tuples: list[list[Any]] = []
    excluded = tuple(x.rstrip("/") for x in exclusions)

    def is_excluded(rel: str) -> bool:
        parts = Path(rel).parts
        if "__pycache__" in parts or ".pytest_cache" in parts or rel.endswith((".pyc", ".pyo")):
            return True
        return any(rel == x or rel.startswith(x + "/") for x in excluded)

    for base, dirs, files in os.walk(path, topdown=True, followlinks=False):
        dirs.sort()
        files.sort()
        relbase = Path(base).relative_to(path)
        dirs[:] = [d for d in dirs if not is_excluded((relbase / d).as_posix())]
        names = dirs + files
        for name in names:
            child = Path(base) / name
            rel = (relbase / name).as_posix()
            if is_excluded(rel):
                continue
            st = os.lstat(child)
            mode = st.st_mode
            kind = (
                "DIRECTORY" if stat.S_ISDIR(mode) else
                "REGULAR_FILE" if stat.S_ISREG(mode) else
                "SYMLINK" if stat.S_ISLNK(mode) else
                "OTHER"
            )
            tuples.append([
                rel, kind, st.st_dev, st.st_ino, st.st_mode, st.st_nlink,
                st.st_size, st.st_mtime_ns, st.st_ctime_ns,
            ])
    tuples.sort(key=lambda row: str(row[0]))
    st = os.stat(path)
    identity = {
        key: getattr(st, key)
        for key in ("st_dev", "st_ino", "st_mode", "st_nlink", "st_size", "st_mtime_ns", "st_ctime_ns")
    }
    return identity, len(tuples), hashlib.sha256(canonical(tuples)).hexdigest()


def errors_for(validator: Draft202012Validator, value: Any) -> list[str]:
    return [f"{list(err.path)}: {err.message}" for err in validator.iter_errors(value)]


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate the SPEC-061 documentation packet without runtime side effects")
    parser.add_argument("--manifest", default=str(DEFAULT_MANIFEST))
    args = parser.parse_args()
    manifest_path = Path(args.manifest).resolve()
    errors: list[str] = []

    policy = load(POLICY)
    registry = load(REGISTRY)
    manifest = load(manifest_path)
    schemas = {
        "registry": load(REGISTRY_SCHEMA),
        "record": load(RECORD_SCHEMA),
        "manifest": load(MANIFEST_SCHEMA),
        "report": load(REPORT_SCHEMA),
    }
    for name, schema in schemas.items():
        try:
            Draft202012Validator.check_schema(schema)
        except Exception as exc:  # pragma: no cover - validator contract failure
            errors.append(f"schema_meta:{name}:{exc}")

    errors += [f"registry:{x}" for x in errors_for(Draft202012Validator(schemas["registry"]), registry)]
    errors += [f"manifest:{x}" for x in errors_for(Draft202012Validator(schemas["manifest"]), manifest)]
    if errors:
        for error in errors:
            print(f"FAIL {error}", file=sys.stderr)
        return 1

    if policy["record_schema_sha256"] != digest_bytes(RECORD_SCHEMA):
        errors.append("policy.record_schema_sha256 mismatch")
    if policy["record_contract"]["schema_sha256"] != digest_bytes(RECORD_SCHEMA):
        errors.append("policy.record_contract.schema_sha256 mismatch")
    if policy["adapter_registry_sha256"] != digest_bytes(REGISTRY):
        errors.append("policy.adapter_registry_sha256 mismatch")
    if policy["adapter_registry_contract"]["sha256"] != digest_bytes(REGISTRY):
        errors.append("policy.adapter_registry_contract.sha256 mismatch")
    if policy["trusted_execution_profile_sha256"] != digest_bytes(PROFILE):
        errors.append("policy.trusted_execution_profile_sha256 mismatch")
    if policy["report_schema"]["sha256"] != digest_bytes(REPORT_SCHEMA):
        errors.append("policy.report_schema.sha256 mismatch")
    if policy["input_manifest_schema"]["sha256"] != digest_bytes(MANIFEST_SCHEMA):
        errors.append("policy.input_manifest_schema.sha256 mismatch")
    if policy["adapter_registry_contract"]["schema_sha256"] != digest_bytes(REGISTRY_SCHEMA):
        errors.append("policy.adapter_registry_contract.schema_sha256 mismatch")
    if manifest["adapter_registry_sha256"] != digest_bytes(REGISTRY):
        errors.append("manifest.adapter_registry_sha256 mismatch")
    if manifest["policy_bytes_sha256"] != digest_bytes(POLICY):
        errors.append("manifest.policy_bytes_sha256 mismatch")
    if manifest["policy_sha256"] != hashlib.sha256(canonical(policy)).hexdigest():
        errors.append("manifest.policy_sha256 mismatch")

    if registry["trusted_current_contract"]["status"] != "EXTERNAL_ONLY_NOT_IN_V1":
        errors.append("trusted current contract is not external-only")
    if any(a["maximum_effective_evidence_class"] == "INDEPENDENTLY_CHECKED" for a in registry["adapters"]):
        errors.append("v1 registry contains an independently-checked adapter")
    if registry["canonical_record_contract"]["record_schema_sha256"] != digest_bytes(RECORD_SCHEMA):
        errors.append("registry canonical_record_contract schema hash mismatch")

    kinds = set(policy["supported_record_kinds"])
    schema_kinds = set(schemas["record"]["properties"]["record_kind"]["enum"])
    registry_kinds = {kind for adapter in registry["adapters"] for kind in adapter["record_kinds"]}
    if kinds != schema_kinds or kinds != registry_kinds:
        errors.append("record-kind coverage mismatch")
    adapters = {adapter["id"]: adapter for adapter in registry["adapters"]}
    manifest_ids = set()
    record_validator = Draft202012Validator(schemas["record"])
    for selection in manifest["records"]:
        if selection["id"] in manifest_ids:
            errors.append(f"duplicate manifest record id: {selection['id']}")
        manifest_ids.add(selection["id"])
        adapter = adapters.get(selection["adapter_id"])
        if adapter is None:
            errors.append(f"unknown adapter: {selection['adapter_id']}")
            continue
        if selection["record_kind"] not in adapter["record_kinds"]:
            errors.append(f"adapter kind mismatch: {selection['id']}")
        if selection["source_format"] != adapter["source_format"] or selection["source_version"] != adapter["source_version"]:
            errors.append(f"adapter source mismatch: {selection['id']}")
        if selection["availability_expectation"] == "MISSING_ALLOWED":
            if selection["expected"] != {"sha256": None, "size": None}:
                errors.append(f"missing expected projection is not null: {selection['id']}")
            continue
        source = Path(selection["path"])
        if selection["path_kind"] == "REGULAR_FILE":
            if not source.is_file() or source.is_symlink():
                errors.append(f"selected regular file unavailable/unsafe: {selection['id']}")
                continue
            if selection["expected"]["size"] != source.stat().st_size or selection["expected"]["sha256"] != digest_bytes(source):
                errors.append(f"selected source digest mismatch: {selection['id']}")
            record = load(source)
            errors += [f"record:{selection['id']}:{x}" for x in errors_for(record_validator, record)]
            if record.get("record_id") != selection["id"] or record.get("record_kind") != selection["record_kind"]:
                errors.append(f"record identity mismatch: {selection['id']}")
            if record.get("record_digest", {}).get("sha256") != record_digest(record):
                errors.append(f"record digest mismatch: {selection['id']}")
        elif selection["path_kind"] == "DIRECTORY":
            if not source.is_dir() or source.is_symlink():
                errors.append(f"selected directory unavailable/unsafe: {selection['id']}")
                continue
            identity, count, entries_sha = directory_observation(source, policy["directory_observation_contract"]["exclusions"])
            expected = selection["expected"]
            if expected["identity"] != identity:
                errors.append(f"directory identity mismatch: {selection['id']}")
            if expected["entry_count"] != count:
                errors.append(f"directory entry count mismatch: {selection['id']}: expected={expected['entry_count']} actual={count}")
            if expected["entries_sha256"] != entries_sha:
                errors.append(f"directory tree digest mismatch: {selection['id']}: expected={expected['entries_sha256']} actual={entries_sha}")

    if manifest["plan_record_path"] != next(x["path"] for x in manifest["records"] if x["id"] == manifest["plan_record_id"]):
        errors.append("plan_record_path does not select the plan record file")
    if not any(x["record_kind"] == "toolchain" for x in manifest["records"]):
        errors.append("manifest has no explicit toolchain selection")
    root_wrapper = ROOT / "Specs/RESULTS-SPEC-061/fixtures/records/repo-root.current.json"
    if root_wrapper.exists():
        errors.append("duplicate repo-root.current JSON wrapper exists")
    plan = load(Path(manifest["plan_record_path"]))
    if plan["payload"]["path"] != manifest["plan_source_path"]:
        errors.append("plan_source_path does not equal governing-plan payload.path")

    if policy["git_probe_contract"]["candidate_sha_pattern"] != "^[0-9a-f]{40}$":
        errors.append("git candidate SHA contract is not exact lowercase 40-hex")
    if policy["git_probe_contract"]["network"] != "DENY" or policy["git_probe_contract"]["fetch"] != "REJECT" or policy["git_probe_contract"]["shell"] != "REJECT":
        errors.append("git probe confinement is not closed")
    checks = policy["check_registry"]
    check_ids = [check["id"] for check in checks]
    if len(checks) != 20 or len(set(check_ids)) != 20:
        errors.append("check registry cardinality/uniqueness mismatch")
    for check in checks:
        predicate = check["predicate"]
        success = predicate["success"]
        if not predicate.get("success_selectors") or success.get("selectors") != predicate["success_selectors"]:
            errors.append(f"missing success selectors: {check['id']}")
        if not predicate.get("semantic_requirements") or "All required roles" in success.get("when", ""):
            errors.append(f"generic success predicate: {check['id']}")

    if errors:
        for error in errors:
            print(f"FAIL {error}", file=sys.stderr)
        return 1
    print(json.dumps({
        "SPEC061_VALIDATION": "PASS",
        "manifest": str(manifest_path),
        "checks": len(checks),
        "manifest_records": len(manifest["records"]),
        "registry_adapters": len(registry["adapters"]),
        "independently_checked_reachable": False,
        "read_only": True,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
