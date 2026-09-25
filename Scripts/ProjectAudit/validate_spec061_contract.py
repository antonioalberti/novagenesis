#!/usr/bin/env python3
"""Read-only SPEC-061 packet validator; not the readiness checker."""
from __future__ import annotations

import argparse
import fnmatch
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


class DuplicateKeyError(ValueError):
    """Raised when a JSON object repeats a member name."""


def _reject_duplicate_keys(pairs: list[tuple[str, Any]]) -> dict[str, Any]:
    result: dict[str, Any] = {}
    for key, value in pairs:
        if key in result:
            raise DuplicateKeyError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def _reject_nonfinite(value: str) -> None:
    raise ValueError(f"non-finite JSON number: {value}")


def load(path: Path) -> Any:
    with path.open("r", encoding="utf-8", newline="") as fh:
        return json.load(
            fh,
            object_pairs_hook=_reject_duplicate_keys,
            parse_constant=_reject_nonfinite,
        )


def digest_bytes(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _jcs_string(value: str) -> str:
    return json.dumps(value, ensure_ascii=False, separators=(",", ":"))


def canonical(value: object) -> bytes:
    """Serialize the packet's JSON subset using RFC 8785-compatible rules.

    The versioned contracts permit only integers (not floating-point numbers),
    booleans, null, strings, arrays and objects. Object keys are ordered by
    UTF-16 code units as required by JCS.
    """
    if value is None:
        return b"null"
    if value is True:
        return b"true"
    if value is False:
        return b"false"
    if isinstance(value, int):
        return str(value).encode("ascii")
    if isinstance(value, float):
        raise ValueError("floating-point values are outside the SPEC-061 canonical subset")
    if isinstance(value, str):
        return _jcs_string(value).encode("utf-8")
    if isinstance(value, list):
        return b"[" + b",".join(canonical(item) for item in value) + b"]"
    if isinstance(value, dict):
        keys = sorted(value, key=lambda key: key.encode("utf-16-be"))
        members = []
        for key in keys:
            members.append(_jcs_string(key).encode("utf-8") + b":" + canonical(value[key]))
        return b"{" + b",".join(members) + b"}"
    raise TypeError(f"unsupported canonical JSON value: {type(value).__name__}")


def record_digest(record: dict[str, Any]) -> str:
    body = {k: v for k, v in record.items() if k != "record_digest"}
    return hashlib.sha256(canonical(body)).hexdigest()


def validate_directory_observation_contract(contract: Any) -> list[str]:
    errors: list[str] = []
    if not isinstance(contract, dict):
        return ["directory observation contract must be an object"]
    roles = {"SOURCE_ROOT", "PAYLOAD_ROOT"}
    exclusions = contract.get("exclusions_by_role")
    generated = contract.get("generated_artifact_patterns_by_role")
    if not isinstance(exclusions, dict) or set(exclusions) != roles:
        errors.append("directory exclusions must declare exactly SOURCE_ROOT and PAYLOAD_ROOT")
        return errors
    if not isinstance(generated, dict) or set(generated) != roles:
        errors.append("generated-artifact exclusions must declare exactly SOURCE_ROOT and PAYLOAD_ROOT")
        return errors
    if any(not isinstance(items, list) or any(not isinstance(item, str) for item in items) for items in exclusions.values()):
        errors.append("directory exclusions must be lists of strings")
    if any(not isinstance(items, list) or any(not isinstance(item, str) for item in items) for items in generated.values()):
        errors.append("generated-artifact exclusions must be lists of strings")
    if errors:
        return errors
    if "Docs" not in exclusions["SOURCE_ROOT"]:
        errors.append("SOURCE_ROOT must retain the documented Docs exclusion")
    if exclusions["PAYLOAD_ROOT"]:
        errors.append("PAYLOAD_ROOT must be fully enumerated without directory exclusions")
    if generated["PAYLOAD_ROOT"]:
        errors.append("PAYLOAD_ROOT must be fully enumerated without generated-artifact exclusions")
    return errors


def directory_observation(path: Path, contract: dict[str, Any], role: str) -> tuple[dict[str, int], int, str]:
    errors = validate_directory_observation_contract(contract)
    if errors:
        raise ValueError("invalid directory observation contract: " + "; ".join(errors))
    exclusions = contract["exclusions_by_role"][role] if role in contract["exclusions_by_role"] else None
    if exclusions is None:
        raise ValueError(f"unknown directory role: {role}")
    patterns = contract["generated_artifact_patterns_by_role"][role]

    def is_excluded(rel: str) -> bool:
        if any(rel == item or rel.startswith(item.rstrip("/") + "/") for item in exclusions):
            return True
        for pattern in patterns:
            variants = {pattern}
            if pattern.startswith("**/"):
                variants.add(pattern[3:])
            if pattern.endswith("/**"):
                variants.add(pattern[:-3])
                if pattern.startswith("**/"):
                    variants.add(pattern[3:-3])
            if any(fnmatch.fnmatchcase(rel, candidate) for candidate in variants):
                return True
        return False

    tuples: list[list[Any]] = []
    for base, dirs, files in os.walk(path, topdown=True, followlinks=False):
        dirs.sort()
        files.sort()
        relbase = Path(base).relative_to(path)
        dirs[:] = [d for d in dirs if not is_excluded((relbase / d).as_posix())]
        for name in dirs + files:
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
        for key in (
            "st_dev", "st_ino", "st_mode", "st_nlink",
            "st_size", "st_mtime_ns", "st_ctime_ns",
        )
    }
    return identity, len(tuples), hashlib.sha256(canonical(tuples)).hexdigest()


def errors_for(validator: Draft202012Validator, value: Any) -> list[str]:
    errors = []
    for error in validator.iter_errors(value):
        path = ".".join(str(part) for part in error.path)
        errors.append(f"{path or '$'}: {error.message}")
    return sorted(errors)


def check_dependency_dag(checks: list[dict[str, Any]], errors: list[str]) -> None:
    ids = [check.get("id") for check in checks]
    known = set(ids)
    if len(ids) != len(known):
        errors.append("check registry contains duplicate ids")
    graph: dict[str, list[str]] = {}
    for check in checks:
        check_id = str(check.get("id"))
        deps = check.get("depends_on", [])
        if not isinstance(deps, list) or len(deps) != len(set(deps)):
            errors.append(f"duplicate/non-list dependencies: {check_id}")
            continue
        for dep in deps:
            if dep not in known:
                errors.append(f"unknown dependency: {check_id}->{dep}")
            if dep == check_id:
                errors.append(f"self dependency: {check_id}")
        graph[check_id] = deps

    visiting: set[str] = set()
    visited: set[str] = set()

    def visit(node: str, stack: list[str]) -> None:
        if node in visiting:
            cycle = "->".join(stack + [node])
            errors.append(f"dependency cycle: {cycle}")
            return
        if node in visited:
            return
        visiting.add(node)
        for dep in graph.get(node, []):
            if dep in graph:
                visit(dep, stack + [node])
        visiting.remove(node)
        visited.add(node)

    for node in graph:
        visit(node, [])


def check_gate_contract(policy: dict[str, Any], checks: list[dict[str, Any]], errors: list[str]) -> None:
    gates = {check.get("gate") for check in checks}
    expected_gates = {"R0", "R1", "R2"}
    if gates != expected_gates:
        errors.append("check registry gate coverage mismatch")
    dependencies = policy.get("gate_dependencies")
    if not isinstance(dependencies, dict) or set(dependencies) != expected_gates:
        errors.append("gate_dependencies must contain exactly R0, R1 and R2")
        return
    for gate, deps in dependencies.items():
        if not isinstance(deps, list) or len(deps) != len(set(deps)):
            errors.append(f"invalid gate dependency list: {gate}")
            continue
        for dep in deps:
            if dep not in expected_gates:
                errors.append(f"unknown gate dependency: {gate}->{dep}")
            if dep == gate:
                errors.append(f"self gate dependency: {gate}")

    graph = {gate: list(deps) for gate, deps in dependencies.items()}
    visiting: set[str] = set()
    visited: set[str] = set()

    def visit(node: str) -> None:
        if node in visiting:
            errors.append(f"gate dependency cycle: {node}")
            return
        if node in visited:
            return
        visiting.add(node)
        for dep in graph.get(node, []):
            if dep in graph:
                visit(dep)
        visiting.remove(node)
        visited.add(node)

    for gate in graph:
        visit(gate)

    gate_of = {check.get("id"): check.get("gate") for check in checks}
    for check in checks:
        check_gate = check.get("gate")
        allowed = set(graph.get(check_gate, [])) | {check_gate}
        for dep in check.get("depends_on", []):
            dep_gate = gate_of.get(dep)
            if dep_gate not in allowed:
                errors.append(f"cross-layer dependency is not declared: {check.get('id')}->{dep}")

def check_git_contract(policy: dict[str, Any], report_schema: dict[str, Any], errors: list[str]) -> None:
    contract = policy.get("git_probe_contract", {})
    if contract.get("candidate_sha_pattern") != "^[0-9a-f]{40}$":
        errors.append("git candidate SHA contract is not exact lowercase 40-hex")
    if contract.get("network") != "DENY" or contract.get("fetch") != "REJECT" or contract.get("shell") != "REJECT":
        errors.append("git probe confinement is not closed")
    commands = contract.get("commands", [])
    command_ids = {command.get("id") for command in commands}
    required_ids = {
        "candidate_rev_parse",
        "candidate_cat_file_type",
        "candidate_cat_file_presence",
        "head_rev_parse",
        "local_ref_rev_parse",
        "status",
        "ls_tree",
    }
    if not required_ids.issubset(command_ids):
        errors.append(f"git probe commands missing: {sorted(required_ids - command_ids)}")
    allowed_subcommands = {"rev-parse", "cat-file", "status", "ls-tree"}
    for command in commands:
        argv = command.get("argv", [])
        if not argv or argv[0] != "/usr/bin/git":
            errors.append(f"git command does not pin /usr/bin/git: {command.get('id')}")
        if "-C" not in argv or "<absolute-repo>" not in argv:
            errors.append(f"git command does not pin an absolute repository: {command.get('id')}")
        subcommands = [arg for arg in argv if arg in allowed_subcommands]
        if len(subcommands) != 1:
            errors.append(f"git command is not a single allowlisted operation: {command.get('id')}")
        if any(token in argv for token in ("fetch", "clone", "pull", "push", "sh", "bash", "-c")):
            errors.append(f"forbidden Git/network/shell token in command: {command.get('id')}")
        if command.get("expected_exit") not in (0, 1):
            errors.append(f"Git command lacks closed exit classification: {command.get('id')}")
        if command.get("stderr") not in ("empty", "diagnostic"):
            errors.append(f"Git command lacks closed stderr classification: {command.get('id')}")

    projection = contract.get("report_projection")
    if not isinstance(projection, dict):
        errors.append("git_probe_contract.report_projection missing")
    else:
        required_projection = {
            "rev_parse_candidate",
            "cat_file_type",
            "cat_file_presence",
            "network",
            "rev_parse_head",
            "rev_parse_local_ref",
            "status",
            "ls_tree",
        }
        if set(projection) != required_projection:
            errors.append("git report projection is not a closed eight-field map")
        observed = report_schema.get("$defs", {}).get("observed", {})
        properties = observed.get("properties", {})
        if "git_probe" not in properties:
            errors.append("report schema does not project observed.git_probe")
        git_probe = report_schema.get("$defs", {}).get("git_probe", {})
        git_properties = git_probe.get("properties", {})
        if set(git_properties) != required_projection:
            errors.append("report schema git_probe projection is not closed")


def check_profile_contract(profile: dict[str, Any], errors: list[str]) -> None:
    required_top = {
        "$id", "$schema", "external_provisioning", "git", "id", "limits",
        "producer_trust_anchor", "python", "sandbox", "schema_version",
        "status", "trusted_launcher",
    }
    if set(profile) != required_top:
        errors.append("trusted execution profile top-level keys are not closed")
    if profile.get("$id") != "ng-readiness-trusted-execution-profile-v1" or profile.get("schema_version") != 1:
        errors.append("trusted execution profile identity/version mismatch")
    if profile.get("status") != "REQUIRED_BEFORE_PROPOSAL":
        errors.append("trusted execution profile status is not REQUIRED_BEFORE_PROPOSAL")
    limits = profile.get("limits", {})
    required_limits = {"artifact_bytes", "directory_entries", "git_stdout_stderr_bytes", "json_depth", "manifest_bytes", "record_bytes", "subprocess_timeout_s", "total_runtime_s"}
    if set(limits) != required_limits or any(not isinstance(limits.get(key), int) or limits[key] <= 0 for key in required_limits):
        errors.append("trusted execution profile limits are not closed positive integers")
    launcher = profile.get("trusted_launcher", {})
    external_launcher = profile.get("external_provisioning", {}).get("launcher", {})
    if launcher.get("argv_contract") != external_launcher.get("argv_contract"):
        errors.append("trusted profile launcher argv contracts mismatch")
    expected_args = {"--python", "--checker", "--repo", "--input-manifest", "--manifest-sha256", "--policy", "--policy-sha256", "--policy-bytes-sha256", "--adapter-registry", "--adapter-registry-sha256", "--phase", "--report"}
    if set(launcher.get("argv_contract", [])) != expected_args | {"--plan-source-path", "--plan-record-path", "--candidate-sha"}:
        errors.append("trusted launcher argv contract is not closed")
    sandbox = profile.get("sandbox", {})
    if sandbox.get("network") != "DENY" or sandbox.get("git_repository_mount") != "READ_ONLY" or sandbox.get("process_execution") != "ALLOWLISTED_GIT_ONLY" or sandbox.get("no_new_privs") is not True or sandbox.get("kill_process_group_on_timeout") is not True:
        errors.append("trusted sandbox controls are not fail-closed")
    if sandbox.get("status") != "NOT_PROVISIONED" or launcher.get("status") != "NOT_PROVISIONED":
        errors.append("unprovisioned trusted controls are not explicitly marked")
    anchor = profile.get("producer_trust_anchor", {})
    if anchor.get("source") != "TRUSTED_LAUNCHER_ONLY" or anchor.get("public_keys_from_selected_records") != "NEVER_TRUST" or anchor.get("status") != "NOT_PROVISIONED" or anchor.get("sha256") is not None:
        errors.append("producer trust anchor is not external-only and unprovisioned")
    for path_value in (profile.get("git", {}).get("path"), profile.get("python", {}).get("path"), external_launcher.get("path"), profile.get("producer_trust_anchor", {}).get("path")):
        if not isinstance(path_value, str) or not path_value.startswith("/"):
            errors.append("trusted profile contains a non-absolute executable/provisioning path")



def enum_values_for_field(schema: Any, field_name: str) -> set[Any]:
    values: set[Any] = set()
    if isinstance(schema, dict):
        properties = schema.get("properties")
        if isinstance(properties, dict) and field_name in properties:
            candidate = properties[field_name]
            if isinstance(candidate, dict) and isinstance(candidate.get("enum"), list):
                values.update(candidate["enum"])
        for value in schema.values():
            values.update(enum_values_for_field(value, field_name))
    elif isinstance(schema, list):
        for value in schema:
            values.update(enum_values_for_field(value, field_name))
    return values


def check_policy_contract(policy: dict[str, Any], report_schema: dict[str, Any], record_schema: dict[str, Any], errors: list[str]) -> None:
    checks = policy.get("check_registry", [])
    by_id = {check.get("id"): check for check in checks}
    check_dependency_dag(checks, errors)
    check_gate_contract(policy, checks, errors)
    if len(checks) != 20 or len(by_id) != 20:
        errors.append("check registry cardinality/uniqueness mismatch")

    candidate = by_id.get("R0.CANDIDATE_EXPLICIT", {}).get("predicate", {})
    roles = {role.get("role"): role for role in candidate.get("input_roles", [])}
    for role_name in ("manifest_candidate", "cli_candidate"):
        role = roles.get(role_name)
        if not role:
            errors.append(f"candidate role missing: {role_name}")
            continue
        expected_cardinality = "EXACTLY_ONE" if role_name == "manifest_candidate" else "ZERO_OR_ONE"
        if role.get("cardinality") != expected_cardinality or role.get("nullable") is not True or role.get("required_for_success") is not True or role.get("null_means") != "MISSING_INPUT":
            errors.append(f"candidate role is not explicit/nullable with closed missing semantics: {role_name}")
    if candidate.get("success", {}).get("operator") != "ALL":
        errors.append("candidate success is not an ALL relation")
    candidate_text = json.dumps(candidate, sort_keys=True)
    if "omitted_equals_" in candidate_text.lower() or ("candidate is omitted" in candidate_text.lower() and "manifest" in candidate_text.lower()):
        errors.append("candidate omission semantics remain contradictory")

    build_text = json.dumps(by_id.get("R1.BUILD_SOURCE_BINDING", {}), sort_keys=True)
    if "linked" in build_text.lower() or "proven" not in build_text.lower():
        errors.append("R1.BUILD_SOURCE_BINDING does not use schema value PROVEN")
    baseline_text = json.dumps(by_id.get("R2.REPOSITORY_BASELINE", {}), sort_keys=True)
    if "pre_trial_empty" in baseline_text.lower() or "baseline_status is complete" in baseline_text.lower() or "baseline_status is empty" not in baseline_text.lower():
        errors.append("R2.REPOSITORY_BASELINE is not closed to schema value EMPTY")
    copy_checks = [check for check in checks if "copy_preservation" in json.dumps(check, sort_keys=True)]
    if not copy_checks or any("verified" in json.dumps(check, sort_keys=True).lower() for check in copy_checks):
        errors.append("payload-copy predicate does not use IMMUTABLE_REFERENCE")

    root = by_id.get("R2.ROOT_ISOLATION", {}).get("predicate", {})
    root_roles = {role.get("role"): role for role in root.get("input_roles", [])}
    if set(root_roles) != {"source_root", "payload_repository_root"}:
        errors.append("R2.ROOT_ISOLATION does not declare distinct source and payload Repository roots")
    else:
        if root_roles["source_root"].get("directory_role") != "SOURCE_ROOT":
            errors.append("source_root role is not bound to SOURCE_ROOT")
        if root_roles["payload_repository_root"].get("directory_role") != "PAYLOAD_ROOT":
            errors.append("payload_repository_root role is not bound to PAYLOAD_ROOT")
        if root_roles["source_root"].get("source") != "RECORD" or root_roles["payload_repository_root"].get("source") != "RECORD":
            errors.append("root roles are not both record-backed")
        if root_roles["source_root"].get("record_kinds") != ["directory-observation"] or root_roles["payload_repository_root"].get("record_kinds") != ["directory-observation"]:
            errors.append("root roles do not select directory-observation records")
        root_text = json.dumps(root, sort_keys=True)
        if "aliases" not in root_text and "same path" not in root_text:
            errors.append("root isolation does not close alias rejection")

    value_contract = policy.get("predicate_value_contract", {})
    for check_id, fields in value_contract.items():
        if check_id not in by_id or not isinstance(fields, dict):
            errors.append(f"invalid predicate value contract entry: {check_id}")
            continue
        check_text = json.dumps(by_id[check_id], sort_keys=True)
        for field_name, expected_value in fields.items():
            allowed = enum_values_for_field(record_schema, field_name)
            if expected_value not in allowed:
                errors.append(f"predicate value is not allowed by evidence schema: {check_id}:{field_name}={expected_value}")
            if str(expected_value) not in check_text:
                errors.append(f"predicate value contract is not reflected in check text: {check_id}:{field_name}")

    git_text = json.dumps(policy.get("git_probe_contract", {}), sort_keys=True)
    if "object_absent" not in git_text or "probe_nonzero" not in git_text or "sandbox_unavailable" not in git_text:
        errors.append("Git failure classes are incomplete")
    check_git_contract(policy, report_schema, errors)

    required_fragments = {
        "R0.EFFECTIVE_CONFIG": ("values_sha256", "source_refs", "profile"),
        "R1.TOOLCHAIN": ("compiler_id", "compiler_version", "policy-pinned"),
        "R1.ROLE_LINKAGE": ("helper_identity_refs", "executable_identity_refs"),
        "R2.WORKLOAD_FRESHNESS": ("effective-config", "review_head", "fresh"),
    }
    for check_id, fragments in required_fragments.items():
        text = json.dumps(by_id.get(check_id, {}), sort_keys=True).lower()
        for fragment in fragments:
            if fragment.lower() not in text:
                errors.append(f"{check_id} missing semantic closure fragment: {fragment}")

    for check in checks:
        check_id = str(check.get("id"))
        predicate = check.get("predicate", {})
        success = predicate.get("success", {})
        selectors = predicate.get("success_selectors", [])
        if not selectors or success.get("selectors") != selectors:
            errors.append(f"missing success selectors: {check_id}")
        if len(selectors) != len(set(selectors)):
            errors.append(f"duplicate success selector: {check_id}")
        semantics = predicate.get("semantic_requirements", [])
        if not semantics:
            errors.append(f"missing semantic requirements: {check_id}")
        reason_order = predicate.get("reason_order", [])
        if not reason_order or len(reason_order) != len(set(reason_order)) or reason_order[-1] != "DECISION_SATISFIED":
            errors.append(f"reason precedence is not closed: {check_id}")
        for incident in predicate.get("direct_incidents", []):
            if incident.get("reason_code") not in reason_order:
                errors.append(f"incident reason absent from reason_order: {check_id}")
            if incident.get("reason_code") == "REQUIRED_MAPPING_MISSING" and incident.get("observed") != "NULL":
                errors.append(f"REQUIRED_MAPPING_MISSING observed value is not NULL: {check_id}")
        roles = predicate.get("input_roles", [])
        role_names = [role.get("role") for role in roles]
        if len(role_names) != len(set(role_names)):
            errors.append(f"duplicate input role: {check_id}")
        for role in roles:
            if role.get("source") == "RECORD" and not role.get("record_kinds"):
                errors.append(f"record role has no authoritative record kind: {check_id}:{role.get('role')}")
            if not role.get("selector"):
                errors.append(f"role has no selector: {check_id}:{role.get('role')}")
        if success.get("state") != "SATISFIED" or success.get("reason_code") != "DECISION_SATISFIED":
            errors.append(f"success result is not closed: {check_id}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate the SPEC-061 documentation packet without runtime side effects")
    parser.add_argument("--manifest", default=str(DEFAULT_MANIFEST))
    args = parser.parse_args()
    manifest_path = Path(args.manifest).resolve()
    errors: list[str] = []

    try:
        policy = load(POLICY)
        registry = load(REGISTRY)
        manifest = load(manifest_path)
        schemas = {
            "registry": load(REGISTRY_SCHEMA),
            "record": load(RECORD_SCHEMA),
            "manifest": load(MANIFEST_SCHEMA),
            "report": load(REPORT_SCHEMA),
        }
        profile = load(PROFILE)
    except (OSError, json.JSONDecodeError, DuplicateKeyError) as exc:
        print(f"FAIL input parsing: {exc}", file=sys.stderr)
        return 1

    for name, schema in schemas.items():
        try:
            Draft202012Validator.check_schema(schema)
        except Exception as exc:  # pragma: no cover
            errors.append(f"schema_meta:{name}:{exc}")

    errors += [f"registry:{x}" for x in errors_for(Draft202012Validator(schemas["registry"]), registry)]
    errors += [f"manifest:{x}" for x in errors_for(Draft202012Validator(schemas["manifest"]), manifest)]
    errors += validate_directory_observation_contract(policy.get("directory_observation_contract", {}))
    if errors:
        for error in sorted(set(errors)):
            print(f"FAIL {error}", file=sys.stderr)
        return 1

    expected_hashes = {
        "policy.record_schema_sha256": (policy["record_schema_sha256"], RECORD_SCHEMA),
        "policy.record_contract.schema_sha256": (policy["record_contract"]["schema_sha256"], RECORD_SCHEMA),
        "policy.adapter_registry_sha256": (policy["adapter_registry_sha256"], REGISTRY),
        "policy.adapter_registry_contract.sha256": (policy["adapter_registry_contract"]["sha256"], REGISTRY),
        "policy.trusted_execution_profile_sha256": (policy["trusted_execution_profile_sha256"], PROFILE),
        "policy.sandbox_profile.profile_sha256": (policy["sandbox_profile"]["profile_sha256"], PROFILE),
        "policy.trusted_launcher.profile_sha256": (policy["trusted_launcher"]["profile_sha256"], PROFILE),
        "policy.report_schema.sha256": (policy["report_schema"]["sha256"], REPORT_SCHEMA),
        "policy.input_manifest_schema.sha256": (policy["input_manifest_schema"]["sha256"], MANIFEST_SCHEMA),
        "policy.adapter_registry_contract.schema_sha256": (policy["adapter_registry_contract"]["schema_sha256"], REGISTRY_SCHEMA),
    }
    for name, (declared, path) in expected_hashes.items():
        if declared != digest_bytes(path):
            errors.append(f"{name} mismatch")
    if profile["external_provisioning"]["launcher"]["argv_contract"] != profile["trusted_launcher"]["argv_contract"]:
        errors.append("trusted execution profile launcher argv contracts mismatch")
    if policy["adapter_registry_contract"]["trusted_current_adapter_id"] is not None:
        errors.append("policy still names a v1 trusted-current adapter")
    if "trusted-record-v1" in json.dumps(schemas["manifest"], sort_keys=True):
        errors.append("manifest schema still permits removed trusted adapter")
    if manifest["adapter_registry_sha256"] != digest_bytes(REGISTRY):
        errors.append("manifest.adapter_registry_sha256 mismatch")
    if manifest["policy_bytes_sha256"] != digest_bytes(POLICY):
        errors.append("manifest.policy_bytes_sha256 mismatch")
    if manifest["policy_sha256"] != hashlib.sha256(canonical(policy)).hexdigest():
        errors.append("manifest.policy_sha256 mismatch")

    if registry["trusted_current_contract"]["status"] != "EXTERNAL_ONLY_NOT_IN_V1":
        errors.append("trusted current contract is not external-only")
    if any(adapter["maximum_effective_evidence_class"] == "INDEPENDENTLY_CHECKED" for adapter in registry["adapters"]):
        errors.append("v1 registry contains an independently-checked adapter")
    if registry["canonical_record_contract"]["record_schema_sha256"] != digest_bytes(RECORD_SCHEMA):
        errors.append("registry canonical_record_contract schema hash mismatch")

    kinds = set(policy["supported_record_kinds"])
    schema_kinds = set(schemas["record"]["properties"]["record_kind"]["enum"])
    registry_kinds = {kind for adapter in registry["adapters"] for kind in adapter["record_kinds"]}
    if kinds != schema_kinds or kinds != registry_kinds:
        errors.append("record-kind coverage mismatch")

    adapter_ids = [adapter.get("id") for adapter in registry["adapters"]]
    if len(adapter_ids) != len(set(adapter_ids)):
        errors.append("registry contains duplicate adapter ids")
    for adapter in registry["adapters"]:
        if adapter.get("review_status") != "REVIEWED":
            errors.append(f"adapter is not REVIEWED: {adapter.get('id')}")
        if adapter.get("maximum_effective_evidence_class") == "INDEPENDENTLY_CHECKED":
            errors.append(f"adapter exceeds v1 trust ceiling: {adapter.get('id')}")

    if manifest.get("policy_id") != policy.get("id") or manifest.get("policy_path") != str(POLICY):
        errors.append("manifest policy identity/path mismatch")
    if manifest.get("adapter_registry_id") != registry.get("id") or manifest.get("adapter_registry_path") != str(REGISTRY):
        errors.append("manifest adapter-registry identity/path mismatch")
    if manifest.get("repo_path") != str(ROOT) or not Path(manifest.get("repo_path", "")).is_dir():
        errors.append("manifest repo_path is not the selected repository root")

    manifest_record_ids = [item.get("id") for item in manifest["records"]]
    if manifest_record_ids != sorted(manifest_record_ids):
        errors.append("manifest records are not sorted by id")
    role_records = {
        role: [item for item in manifest["records"] if item.get("directory_role") == role]
        for role in ("SOURCE_ROOT", "PAYLOAD_ROOT")
    }
    for role, selections in role_records.items():
        if len(selections) != 1 or selections[0].get("record_kind") != "directory-observation" or selections[0].get("path_kind") != "DIRECTORY":
            errors.append(f"manifest must select exactly one DIRECTORY directory-observation for {role}")
    if all(len(items) == 1 for items in role_records.values()):
        source_root = Path(role_records["SOURCE_ROOT"][0]["path"])
        payload_root = Path(role_records["PAYLOAD_ROOT"][0]["path"])
        if source_root == payload_root:
            errors.append("SOURCE_ROOT and PAYLOAD_ROOT are aliases")
        elif source_root in payload_root.parents:
            root_check = next((check for check in policy.get("check_registry", []) if check.get("id") == "R2.ROOT_ISOLATION"), {})
            policy_roles = {role.get("role"): role for role in root_check.get("predicate", {}).get("input_roles", [])}
            payload_constraint = json.dumps(policy_roles.get("payload_repository_root", {}), sort_keys=True).lower()
            if "containment is allowed only" not in payload_constraint or "payload_root" not in payload_constraint:
                errors.append("PAYLOAD_ROOT containment is not explicitly policy-bound")
        elif payload_root in source_root.parents:
            errors.append("PAYLOAD_ROOT is an unsafe ancestor of SOURCE_ROOT")
    repo_selection = [item for item in manifest["records"] if item.get("id") == manifest.get("repo_record_id")]
    if len(repo_selection) != 1 or repo_selection[0].get("path_kind") != "DIRECTORY" or repo_selection[0].get("path") != manifest.get("repo_path"):
        errors.append("repo_record_id does not select the manifest repo_path directory")
    plan_selection = [item for item in manifest["records"] if item.get("id") == manifest.get("plan_record_id")]
    if len(plan_selection) != 1 or plan_selection[0].get("record_kind") != "governing-plan":
        errors.append("plan_record_id does not select exactly one governing-plan record")

    adapters = {adapter["id"]: adapter for adapter in registry["adapters"]}
    manifest_ids: set[str] = set()
    record_validator = Draft202012Validator(schemas["record"])
    for selection in manifest["records"]:
        selection_id = selection["id"]
        if selection_id in manifest_ids:
            errors.append(f"duplicate manifest record id: {selection_id}")
        manifest_ids.add(selection_id)
        source = Path(selection["path"])
        if not source.is_absolute():
            errors.append(f"manifest path is not absolute: {selection_id}")
        adapter = adapters.get(selection["adapter_id"])
        if adapter is None:
            errors.append(f"unknown adapter: {selection['adapter_id']}")
            continue
        if selection["adapter_version"] != adapter["version"]:
            errors.append(f"adapter version mismatch: {selection_id}")
        if selection["record_kind"] not in adapter["record_kinds"]:
            errors.append(f"adapter kind mismatch: {selection_id}")
        if selection["source_format"] != adapter["source_format"] or selection["source_version"] != adapter["source_version"]:
            errors.append(f"adapter source mismatch: {selection_id}")
        if selection["path_kind"] != adapter["path_kind"]:
            errors.append(f"adapter path-kind mismatch: {selection_id}")
        if selection["availability_expectation"] == "MISSING_ALLOWED" and adapter.get("missing_allowed") is not True:
            errors.append(f"adapter does not permit missing input: {selection_id}")
        if selection["availability_expectation"] == "PRESENT" and selection["path_kind"] != "DIRECTORY" and selection["directory_role"] is not None:
            errors.append(f"regular-file selection has a directory role: {selection_id}")
        if selection["path_kind"] == "DIRECTORY" and selection["directory_role"] != adapter.get("directory_role"):
            errors.append(f"adapter directory-role mismatch: {selection_id}")
        if selection["availability_expectation"] == "MISSING_ALLOWED":
            if selection["expected"] != {"sha256": None, "size": None}:
                errors.append(f"missing expected projection is not null: {selection_id}")
            if source.exists():
                errors.append(f"missing-allowed input unexpectedly exists: {selection_id}")
            continue
        if selection["path_kind"] == "REGULAR_FILE":
            if not source.is_file() or source.is_symlink():
                errors.append(f"selected regular file unavailable/unsafe: {selection_id}")
                continue
            if selection["expected"]["size"] != source.stat().st_size or selection["expected"]["sha256"] != digest_bytes(source):
                errors.append(f"selected source digest mismatch: {selection_id}")
            try:
                record = load(source)
            except (OSError, json.JSONDecodeError, DuplicateKeyError) as exc:
                errors.append(f"record parse failure:{selection_id}:{exc}")
                continue
            errors += [f"record:{selection_id}:{x}" for x in errors_for(record_validator, record)]
            if record.get("record_id") != selection_id or record.get("record_kind") != selection["record_kind"]:
                errors.append(f"record identity mismatch: {selection_id}")
            source_meta = record.get("source", {})
            if source_meta.get("adapter_id") != selection["adapter_id"] or source_meta.get("adapter_version") != selection["adapter_version"]:
                errors.append(f"record source adapter mismatch: {selection_id}")
            if source_meta.get("format") != selection["source_format"] or source_meta.get("version") != selection["source_version"]:
                errors.append(f"record source format/version mismatch: {selection_id}")
            if record.get("candidate_binding") != selection.get("candidate_binding"):
                errors.append(f"record candidate binding mismatch: {selection_id}")
            if record.get("evidence_class_claim") != selection.get("accepted_evidence_class_claim"):
                errors.append(f"record evidence-class claim mismatch: {selection_id}")
            if record.get("record_digest", {}).get("sha256") != record_digest(record):
                errors.append(f"record digest mismatch: {selection_id}")
        elif selection["path_kind"] == "DIRECTORY":
            if not source.is_dir() or source.is_symlink():
                errors.append(f"selected directory unavailable/unsafe: {selection_id}")
                continue
            identity, count, entries_sha = directory_observation(
                source,
                policy["directory_observation_contract"],
                selection["directory_role"],
            )
            expected = selection["expected"]
            if expected["identity"] != identity:
                errors.append(f"directory identity mismatch: {selection_id}")
            if expected["entry_count"] != count:
                errors.append(f"directory entry count mismatch: {selection_id}: expected={expected['entry_count']} actual={count}")
            if expected["entries_sha256"] != entries_sha:
                errors.append(f"directory tree digest mismatch: {selection_id}: expected={expected['entries_sha256']} actual={entries_sha}")

    plan_records = [item for item in manifest["records"] if item["id"] == manifest["plan_record_id"]]
    if len(plan_records) != 1 or manifest["plan_record_path"] != plan_records[0]["path"]:
        errors.append("plan_record_path does not select exactly the plan record file")
    toolchain_records = [item for item in manifest["records"] if item["record_kind"] == "toolchain"]
    if len(toolchain_records) != 1:
        errors.append("manifest must select exactly one explicit toolchain record")
    root_wrapper = ROOT / "Specs/RESULTS-SPEC-061/fixtures/records/repo-root.current.json"
    if root_wrapper.exists():
        errors.append("duplicate repo-root.current JSON wrapper exists")
    try:
        plan = load(Path(manifest["plan_record_path"]))
        if plan["payload"]["path"] != manifest["plan_source_path"]:
            errors.append("plan_source_path does not equal governing-plan payload.path")
    except (OSError, json.JSONDecodeError, DuplicateKeyError, KeyError) as exc:
        errors.append(f"governing plan selection failure: {exc}")

    check_profile_contract(profile, errors)
    check_policy_contract(policy, schemas["report"], schemas["record"], errors)

    if errors:
        for error in sorted(set(errors)):
            print(f"FAIL {error}", file=sys.stderr)
        return 1
    print(json.dumps({
        "SPEC061_VALIDATION": "PASS",
        "manifest": str(manifest_path),
        "checks": len(policy["check_registry"]),
        "manifest_records": len(manifest["records"]),
        "registry_adapters": len(registry["adapters"]),
        "independently_checked_reachable": False,
        "read_only": True,
    }, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
