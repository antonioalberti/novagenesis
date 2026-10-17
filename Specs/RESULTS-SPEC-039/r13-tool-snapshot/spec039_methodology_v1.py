#!/usr/bin/env python3
"""Offline semantic interpreter for the SPEC-039 journal transition model.

This module does not execute or replace the production finalizer. It consumes
an explicit, versioned model object and classifies only synthetic journal data.
"""
from __future__ import annotations

import json
import re
from typing import Any

EVENT_FIELDS = {
    "schema_version",
    "run_id",
    "sequence",
    "event",
    "monotonic_ns",
    "pid",
    "uid",
    "resource_type",
    "candidate_key",
    "source_name",
    "error_number",
    "identity",
}


def _model_index(model: object) -> tuple[dict[str, Any], dict[str, Any]]:
    if not isinstance(model, dict):
        raise ValueError("model must be an object")
    required = {
        "model_id",
        "model_version",
        "schema_version",
        "journal_states",
        "journal_initial_state",
        "journal_terminal_state",
        "resource_states",
        "resource_initial_state",
        "resource_kinds",
        "identity_binding",
        "required_events",
        "events",
        "authority_modes",
        "mode_effects",
        "cleanup_outcomes",
        "normal_cleanup_failure",
    }
    if set(model) != required:
        raise ValueError("model has missing or unrecognized fields")
    if not isinstance(model["model_id"], str) or not model["model_id"]:
        raise ValueError("model_id must be a non-empty string")
    if isinstance(model["model_version"], bool) or model["model_version"] != 1:
        raise ValueError("unsupported model_version")
    if not isinstance(model["schema_version"], str) or not model["schema_version"]:
        raise ValueError("schema_version must be a non-empty string")
    for name in ("journal_states", "resource_states"):
        values = model[name]
        if not isinstance(values, list) or not values or any(not isinstance(v, str) or not v for v in values):
            raise ValueError(f"{name} must be a non-empty string list")
        if len(values) != len(set(values)):
            raise ValueError(f"{name} contains duplicate states")
    journal_states = set(model["journal_states"])
    resource_states = set(model["resource_states"])
    if model["journal_initial_state"] not in journal_states or model["journal_terminal_state"] not in journal_states:
        raise ValueError("journal initial/terminal state is not declared")
    if model["resource_initial_state"] not in resource_states:
        raise ValueError("resource initial state is not declared")

    kinds = model["resource_kinds"]
    if not isinstance(kinds, dict) or not isinstance(kinds.get("journal"), str):
        raise ValueError("resource_kinds must declare a journal kind")
    candidates = kinds.get("candidates")
    if not isinstance(candidates, list) or not candidates or any(not isinstance(v, str) or not v for v in candidates):
        raise ValueError("candidate resource kinds must be a non-empty string list")
    if len(candidates) != len(set(candidates)):
        raise ValueError("candidate resource kinds contain duplicates")
    identity_binding = model["identity_binding"]
    if not isinstance(identity_binding, dict) or set(identity_binding) != set(candidates):
        raise ValueError("identity_binding must define exactly one binding for each candidate kind")
    for kind, fields in identity_binding.items():
        if not isinstance(fields, list) or not fields or any(not isinstance(v, str) or not v for v in fields):
            raise ValueError(f"identity binding for {kind} must be a non-empty field list")
        if len(fields) != len(set(fields)):
            raise ValueError(f"identity binding for {kind} repeats a field")
    if identity_binding != {
        "sysv_shm": ["candidate_key", "shmid"],
        "posix_semaphore": ["source_name", "device", "inode"],
    }:
        raise ValueError("resource identity bindings omit the exact SysV ID or POSIX device/inode tuple")

    rules = model["events"]
    if not isinstance(rules, list) or not rules:
        raise ValueError("events must be a non-empty list")
    by_event: dict[str, Any] = {}
    requirement_ids: set[str] = set()
    for rule in rules:
        if not isinstance(rule, dict):
            raise ValueError("event rule must be an object")
        allowed_rule_fields = {
            "requirement_id",
            "event",
            "scope",
            "phase_from",
            "phase_to",
            "resource_from",
            "resource_to",
            "requires_no_resource_states",
        }
        required_rule_fields = allowed_rule_fields - {"requires_no_resource_states"}
        if not required_rule_fields <= set(rule) or not set(rule) <= allowed_rule_fields:
            raise ValueError("event rule has missing or unrecognized fields")
        name = rule.get("event")
        requirement_id = rule.get("requirement_id")
        if not isinstance(name, str) or not name or name in by_event:
            raise ValueError("event names must be unique non-empty strings")
        if not isinstance(requirement_id, str) or not requirement_id or requirement_id in requirement_ids:
            raise ValueError("requirement IDs must be unique non-empty strings")
        if rule.get("scope") not in {"journal", "resource"}:
            raise ValueError(f"invalid scope for {name}")
        phases = rule.get("phase_from")
        if not isinstance(phases, list) or not phases or not set(phases) <= journal_states:
            raise ValueError(f"invalid phase_from for {name}")
        if rule.get("phase_to") not in journal_states:
            raise ValueError(f"invalid phase_to for {name}")
        if rule["scope"] == "resource":
            if rule.get("resource_from") not in resource_states or rule.get("resource_to") not in resource_states:
                raise ValueError(f"invalid resource transition for {name}")
        elif rule.get("resource_from") is not None or rule.get("resource_to") is not None:
            raise ValueError(f"journal event {name} cannot have a resource transition")
        required_empty = rule.get("requires_no_resource_states", [])
        if not isinstance(required_empty, list) or not set(required_empty) <= resource_states:
            raise ValueError(f"invalid resource-state guard for {name}")
        by_event[name] = rule
        requirement_ids.add(requirement_id)

    required_events = model["required_events"]
    if not isinstance(required_events, list) or any(not isinstance(v, str) or not v for v in required_events) or len(required_events) != len(set(required_events)):
        raise ValueError("required_events must be a unique list")
    if set(required_events) != set(by_event):
        raise ValueError("declared event set differs from transition table")
    transition_pairs = (
        ("CREATED", "CREATE_INTENT"),
        ("CREATE_FAILED", "CREATE_INTENT"),
        ("CLEANUP_INTENT", "CREATED"),
        ("REMOVED", "CLEANUP_INTENT"),
        ("CLEANUP_FAILED", "CLEANUP_INTENT"),
    )
    for outcome_event, prior_event in transition_pairs:
        if by_event[outcome_event]["scope"] != "resource" or by_event[prior_event]["scope"] != "resource":
            raise ValueError(f"{outcome_event}/{prior_event} must be resource-scoped")
        if by_event[outcome_event]["resource_from"] != by_event[prior_event]["resource_to"]:
            raise ValueError(f"{outcome_event} does not consume the state produced by {prior_event}")
    create_pending = by_event["CREATE_INTENT"]["resource_to"]
    cleanup_pending = by_event["CLEANUP_INTENT"]["resource_to"]
    if create_pending == cleanup_pending:
        raise ValueError("creation and cleanup pending states must be distinct")
    if by_event["CREATE_INTENT"]["resource_from"] != model["resource_initial_state"]:
        raise ValueError("CREATE_INTENT must start from resource_initial_state")
    if create_pending not in by_event["RUN_SHUTDOWN_COMPLETE"].get("requires_no_resource_states", []):
        raise ValueError("RUN_SHUTDOWN_COMPLETE must reject unresolved creation intents")
    close_guards = set(by_event["JOURNAL_CLOSED"].get("requires_no_resource_states", []))
    if not {create_pending, cleanup_pending} <= close_guards:
        raise ValueError("JOURNAL_CLOSED must reject unresolved create/cleanup intents")
    opened = by_event["JOURNAL_OPENED"]
    shutdown = by_event["RUN_SHUTDOWN_COMPLETE"]
    closed = by_event["JOURNAL_CLOSED"]
    if opened["scope"] != "journal" or opened["phase_from"] != [model["journal_initial_state"]]:
        raise ValueError("JOURNAL_OPENED must start the global lifecycle")
    open_state = opened["phase_to"]
    shutdown_state = shutdown["phase_to"]
    if shutdown["scope"] != "journal" or open_state not in shutdown["phase_from"] or shutdown_state == open_state:
        raise ValueError("RUN_SHUTDOWN_COMPLETE must advance the global lifecycle")
    if closed["scope"] != "journal" or shutdown_state not in closed["phase_from"] or closed["phase_to"] != model["journal_terminal_state"]:
        raise ValueError("JOURNAL_CLOSED must terminate the post-shutdown lifecycle")
    for name in ("CREATE_INTENT", "CREATED", "CREATE_FAILED"):
        if open_state not in by_event[name]["phase_from"] or by_event[name]["phase_to"] != open_state:
            raise ValueError(f"{name} must remain in the pre-shutdown phase")
    for name in ("CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED"):
        if shutdown_state not in by_event[name]["phase_from"] or by_event[name]["phase_to"] != shutdown_state:
            raise ValueError(f"{name} must remain in the post-shutdown phase")

    modes = model["authority_modes"]
    if not isinstance(modes, dict) or set(modes) != {"requirement_ids", "common_requirements", "normal_requirements", "mode_names"}:
        raise ValueError("authority_modes must be an object")
    authority_ids = modes.get("requirement_ids")
    if authority_ids != {"common": "AUTH-001", "normal": "AUTH-002", "abnormal": "AUTH-003", "blocked": "AUTH-004"}:
        raise ValueError("authority mode requirement IDs are missing, duplicated or unstable")
    common = modes.get("common_requirements")
    normal = modes.get("normal_requirements")
    names = modes.get("mode_names")
    required_common = {"trusted_launcher", "valid_supervisor_receipt", "all_pids_reaped", "logs_closed", "stable_result_directory", "continuous_lease"}
    required_normal = {"durable_shutdown_complete", "normal_waits_valid", "no_timeout_or_signal"}
    if not isinstance(common, list) or not required_common <= set(common):
        raise ValueError("common authority gates omit a mandatory identity/lease/process prerequisite")
    if not isinstance(normal, list) or not required_normal <= set(normal):
        raise ValueError("normal mode gates omit a mandatory shutdown/wait prerequisite")
    if any(not isinstance(v, str) or not v for v in common + normal) or len(common) != len(set(common)) or len(normal) != len(set(normal)):
        raise ValueError("authority gate lists must be unique non-empty strings")
    if names != {"normal": "normal_cleanup", "abnormal": "abnormal_evidence_only", "blocked": "blocked"}:
        raise ValueError("mode_names do not match the closed mode set")

    mode_effects = model["mode_effects"]
    expected_mode_effects = {"normal_cleanup", "abnormal_evidence_only", "blocked"}
    if not isinstance(mode_effects, dict) or set(mode_effects) != expected_mode_effects:
        raise ValueError("mode_effects must define exactly the three closed modes")
    for mode_name, effect in mode_effects.items():
        if not isinstance(effect, dict) or set(effect) != {"requirement_id", "status", "requires", "forbidden"} or not isinstance(effect.get("requires"), list) or not effect["requires"]:
            raise ValueError(f"mode effects for {mode_name} are incomplete")
        if not isinstance(effect.get("forbidden"), list) or not isinstance(effect.get("status"), list):
            raise ValueError(f"mode effects for {mode_name} are missing forbidden/status sets")
        if any(not isinstance(v, str) or not v for v in effect["requires"] + effect["forbidden"] + effect["status"]):
            raise ValueError(f"mode effects for {mode_name} must use non-empty strings")
        if len(effect["requires"]) != len(set(effect["requires"])) or len(effect["forbidden"]) != len(set(effect["forbidden"])) or len(effect["status"]) != len(set(effect["status"])):
            raise ValueError(f"mode effects for {mode_name} contain duplicates")
        expected_id = authority_ids["normal"] if mode_name == "normal_cleanup" else authority_ids["abnormal"] if mode_name == "abnormal_evidence_only" else authority_ids["blocked"]
        if effect.get("requirement_id") != expected_id:
            raise ValueError(f"mode effects for {mode_name} have an unstable requirement ID")
    if set(mode_effects["normal_cleanup"]["status"]) != {"PASS", "FAIL", "INCONCLUSIVE"}:
        raise ValueError("normal mode status set is not the closed acceptance set")
    abnormal_forbidden = set(mode_effects["abnormal_evidence_only"]["forbidden"])
    if not {"live_ipc_inspection", "cleanup", "journal_mutation", "retry", "PASS", "FAIL"} <= abnormal_forbidden:
        raise ValueError("abnormal evidence-only mode permits a forbidden side effect or status")
    blocked_forbidden = set(mode_effects["blocked"]["forbidden"])
    if not {"finalizer_entry", "finalizer_reentry", "cleanup"} <= blocked_forbidden:
        raise ValueError("blocked mode permits finalizer entry or cleanup")
    if set(mode_effects["abnormal_evidence_only"]["status"]) != {"INCONCLUSIVE"}:
        raise ValueError("abnormal evidence-only mode must yield only INCONCLUSIVE")
    if set(mode_effects["blocked"]["status"]) != {"BLOCKED", "INCONCLUSIVE"}:
        raise ValueError("blocked mode status set is not fail-closed")

    outcomes = model["cleanup_outcomes"]
    outcome_names = {"not_started", "partial", "complete", "unknown"}
    if not isinstance(outcomes, dict) or set(outcomes) != outcome_names:
        raise ValueError("cleanup_outcomes must define exactly the closed outcome set")
    outcome_ids: set[str] = set()
    for name, disposition in outcomes.items():
        if not isinstance(disposition, dict):
            raise ValueError(f"cleanup outcome {name} must be an object")
        outcome_fields = {
            "not_started": {"requirement_id", "journal_required", "created_count", "cleanup_intent_count", "create_failed_allowed", "run_status_effect"},
            "partial": {"requirement_id", "journal_required", "all_created_have_one_terminal_outcome", "cleanup_failed_count_min", "run_status_effect"},
            "complete": {"requirement_id", "journal_required", "created_count_min", "all_created_removed", "cleanup_failed_count", "run_status_effect"},
            "unknown": {"requirement_id", "journal_required", "run_status_effect"},
        }
        if set(disposition) != outcome_fields[name]:
            raise ValueError(f"cleanup outcome {name} has missing or unrecognized fields")
        requirement_id = disposition.get("requirement_id")
        if not isinstance(requirement_id, str) or not requirement_id or requirement_id in outcome_ids:
            raise ValueError("cleanup outcome requirement IDs must be unique non-empty strings")
        outcome_ids.add(requirement_id)
        if disposition.get("run_status_effect") == "PASS":
            raise ValueError("cleanup history cannot itself authorize PASS")
        if requirement_id != {"not_started": "COUT-001", "partial": "COUT-002", "complete": "COUT-003", "unknown": "COUT-004"}[name]:
            raise ValueError(f"cleanup outcome {name} has an unstable requirement ID")
    expected_status_effects = {
        "not_started": "NO_PASS_INFERENCE",
        "partial": "INCONCLUSIVE",
        "complete": "DOES_NOT_IMPLY_PASS",
        "unknown": "INCONCLUSIVE",
    }
    if any(outcomes[name].get("run_status_effect") != effect for name, effect in expected_status_effects.items()):
        raise ValueError("cleanup outcome history must not imply overall PASS")
    not_started = outcomes["not_started"]
    partial = outcomes["partial"]
    complete = outcomes["complete"]
    unknown = outcomes["unknown"]
    if not_started.get("journal_required") != "complete_transition_valid_closed" or not_started.get("created_count") != 0 or not_started.get("cleanup_intent_count") != 0:
        raise ValueError("not_started requires a complete journal with no created resource or cleanup intent")
    if partial.get("journal_required") != "complete_transition_valid_closed" or partial.get("all_created_have_one_terminal_outcome") is not True or partial.get("cleanup_failed_count_min") != 1:
        raise ValueError("partial requires complete created-resource coverage and a proven cleanup failure")
    if complete.get("journal_required") != "complete_transition_valid_closed" or complete.get("created_count_min") != 1 or complete.get("all_created_removed") is not True or complete.get("cleanup_failed_count") != 0:
        raise ValueError("complete requires all created resources removed with no cleanup failure")
    if unknown.get("journal_required") != "otherwise":
        raise ValueError("unknown must cover every history outside the other three dispositions")

    failure = model["normal_cleanup_failure"]
    if not isinstance(failure, dict) or set(failure) != {
        "requirement_id",
        "stop_destructive_calls_after_failure",
        "retry_after_intent",
        "outcome_source",
        "complete_history_after_journal_closed_later_evidence_failure",
        "authority_loss",
    } or failure.get("requirement_id") != "FIN-001":
        raise ValueError("normal_cleanup_failure must be a structured requirement")
    if failure.get("stop_destructive_calls_after_failure") is not True or failure.get("retry_after_intent") is not False:
        raise ValueError("normal cleanup failure must stop destructive retries")
    later = failure.get("complete_history_after_journal_closed_later_evidence_failure")
    authority = failure.get("authority_loss")
    if later != {"cleanup_outcome": "complete", "run_status": "INCONCLUSIVE"}:
        raise ValueError("completed cleanup history must remain distinct from later run failure")
    if not isinstance(authority, dict) or authority.get("finalizer_entry") is not False or authority.get("finalizer_reentry") is not False or authority.get("preserve_pending") is not True:
        raise ValueError("authority loss must block finalizer entry/re-entry and preserve pending evidence")
    return model, by_event


def _resource_key(item: dict[str, Any], rule: dict[str, Any], model: dict[str, Any]) -> tuple[str, int | None, str | None] | None:
    kinds = model["resource_kinds"]
    if not isinstance(kinds, dict):
        raise ValueError("resource_kinds must be an object")
    if rule["scope"] == "journal":
        if item["resource_type"] != kinds.get("journal") or item["candidate_key"] is not None or item["source_name"] is not None:
            raise ValueError("journal-scoped event has resource identity")
        return None
    candidates = kinds.get("candidates")
    if not isinstance(candidates, list) or item["resource_type"] not in candidates:
        raise ValueError("unsupported candidate resource type")
    key = item["candidate_key"]
    name = item["source_name"]
    if item["resource_type"] == "sysv_shm":
        if isinstance(key, bool) or not isinstance(key, int) or key <= 0 or name is not None:
            raise ValueError("invalid SysV SHM resource key")
    elif item["resource_type"] == "posix_semaphore":
        if key is not None or not isinstance(name, str) or not name:
            raise ValueError("invalid POSIX semaphore resource name")
    return (item["resource_type"], key, name)


def _stable_resource_identity(item: dict[str, Any], model: dict[str, Any]) -> tuple[Any, ...]:
    resource_type = item["resource_type"]
    fields = model["identity_binding"].get(resource_type)
    identity = item["identity"]
    if not isinstance(fields, list) or not isinstance(identity, dict):
        raise ValueError("resource event lacks bound identity evidence")
    values: list[Any] = []
    for field in fields:
        if field not in identity or identity[field] is None:
            raise ValueError(f"resource identity is missing {field}")
        value = identity[field]
        if field in {"candidate_key", "shmid", "device", "inode"}:
            if isinstance(value, bool) or not isinstance(value, int) or value < 0:
                raise ValueError(f"resource identity field {field} must be a non-negative integer")
        elif field == "source_name" and (not isinstance(value, str) or not value):
            raise ValueError("resource identity source_name must be non-empty")
        values.append(value)
    if resource_type == "sysv_shm" and identity["candidate_key"] != item["candidate_key"]:
        raise ValueError("identity candidate_key differs from event resource key")
    if resource_type == "posix_semaphore" and identity["source_name"] != item["source_name"]:
        raise ValueError("identity source_name differs from event resource name")
    return tuple(values)


def replay_journal(events: object, model: object) -> dict[str, Any]:
    """Replay a complete journal; invalid or ambiguous histories yield unknown."""
    try:
        checked_model, rules = _model_index(model)
        if not isinstance(events, list) or not events:
            raise ValueError("journal is empty or not a list")
        run_ids: set[str] = set()
        previous_time = -1
        resource_states: dict[tuple[str, int | None, str | None], str] = {}
        created: set[tuple[str, int | None, str | None]] = set()
        cleanup_intents: set[tuple[str, int | None, str | None]] = set()
        cleanup_results: dict[tuple[str, int | None, str | None], str] = {}
        identities: dict[tuple[str, int | None, str | None], tuple[Any, ...]] = {}
        phase = checked_model["journal_initial_state"]
        for expected_sequence, item in enumerate(events, 1):
            if not isinstance(item, dict) or set(item) != EVENT_FIELDS:
                raise ValueError("event has missing or extra fields")
            if item["schema_version"] != checked_model["schema_version"]:
                raise ValueError("event schema_version mismatch")
            if not isinstance(item["run_id"], str) or not item["run_id"]:
                raise ValueError("run_id must be a non-empty string")
            run_ids.add(item["run_id"])
            if isinstance(item["sequence"], bool) or not isinstance(item["sequence"], int) or item["sequence"] != expected_sequence:
                raise ValueError("event sequence is not contiguous")
            if isinstance(item["monotonic_ns"], bool) or not isinstance(item["monotonic_ns"], int) or item["monotonic_ns"] < previous_time:
                raise ValueError("event monotonic time is invalid or decreasing")
            previous_time = item["monotonic_ns"]
            if isinstance(item["pid"], bool) or not isinstance(item["pid"], int) or item["pid"] < 2:
                raise ValueError("pid must be an integer of at least 2")
            if isinstance(item["uid"], bool) or not isinstance(item["uid"], int) or item["uid"] < 0:
                raise ValueError("uid must be a non-negative integer")
            if item["error_number"] is not None and (isinstance(item["error_number"], bool) or not isinstance(item["error_number"], int)):
                raise ValueError("error_number must be an integer or null")
            if item["identity"] is not None and not isinstance(item["identity"], dict):
                raise ValueError("identity must be an object or null")
            event_name = item["event"]
            rule = rules.get(event_name)
            if rule is None:
                raise ValueError("unknown event")
            if phase not in rule["phase_from"]:
                raise ValueError(f"{event_name} is forbidden in phase {phase}")
            for forbidden_state in rule.get("requires_no_resource_states", []):
                if forbidden_state in resource_states.values():
                    raise ValueError(f"{event_name} requires no resource in state {forbidden_state}")
            resource = _resource_key(item, rule, checked_model)
            if rule["scope"] == "resource":
                if resource is None:
                    raise ValueError("resource event has no resource identity")
                old_state = resource_states.get(resource, checked_model["resource_initial_state"])
                if old_state != rule["resource_from"]:
                    raise ValueError(f"{event_name} requires resource state {rule['resource_from']}, got {old_state}")
                if event_name == "CREATED":
                    identities[resource] = _stable_resource_identity(item, checked_model)
                elif event_name in {"CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED"}:
                    event_identity = _stable_resource_identity(item, checked_model)
                    if resource not in identities or event_identity != identities[resource]:
                        raise ValueError(f"{event_name} identity differs from the exact CREATED resource")
                resource_states[resource] = rule["resource_to"]
                if event_name == "CREATED":
                    created.add(resource)
                elif event_name == "CLEANUP_INTENT":
                    cleanup_intents.add(resource)
                elif event_name in {"REMOVED", "CLEANUP_FAILED"}:
                    cleanup_results[resource] = event_name
            phase = rule["phase_to"]
        if len(run_ids) != 1:
            raise ValueError("journal contains multiple run IDs")
        if phase != checked_model["journal_terminal_state"]:
            raise ValueError("journal is not closed")
        if any(state in {"create_pending", "cleanup_pending"} for state in resource_states.values()):
            raise ValueError("journal closes with a pending transaction")
        if not created and not cleanup_intents:
            outcome = "not_started"
        elif created == cleanup_intents == set(cleanup_results):
            if any(result == "CLEANUP_FAILED" for result in cleanup_results.values()):
                outcome = "partial"
            elif all(result == "REMOVED" for result in cleanup_results.values()):
                outcome = "complete"
            else:
                outcome = "unknown"
        else:
            outcome = "unknown"
        return {
            "valid": True,
            "outcome": outcome,
            "reason": "transition-valid complete journal",
            "created_count": len(created),
            "cleanup_intent_count": len(cleanup_intents),
            "cleanup_result_count": len(cleanup_results),
        }
    except (KeyError, TypeError, ValueError, AssertionError) as exc:
        return {
            "valid": False,
            "outcome": "unknown",
            "reason": str(exc),
            "created_count": 0,
            "cleanup_intent_count": 0,
            "cleanup_result_count": 0,
        }


def classify_journal(events: object, model: object) -> str:
    return replay_journal(events, model)["outcome"]


def select_finalization_mode(gates: object, model: object) -> str:
    """Choose a documentary mode only when every shared authority gate is true."""
    try:
        if not isinstance(gates, dict):
            return "blocked"
        checked_model, _ = _model_index(model)
        contract = checked_model["authority_modes"]
        common = contract.get("common_requirements")
        normal = contract.get("normal_requirements")
        names = contract.get("mode_names")
        if not isinstance(common, list) or not common or not isinstance(normal, list) or not normal or not isinstance(names, dict):
            return "blocked"
        if any(not isinstance(key, str) or not key for key in common + normal):
            return "blocked"
        if any(gates.get(key) is not True for key in common):
            return names.get("blocked", "blocked")
        if all(gates.get(key) is True for key in normal):
            return names.get("normal", "normal_cleanup")
        return names.get("abnormal", "abnormal_evidence_only")
    except (KeyError, TypeError, ValueError):
        return "blocked"


def check_model_projections(
    spec_model: object,
    manifest_model: object,
    packet_model: object,
) -> list[str]:
    """Require the SPEC model to equal its structured manifest/packet copies."""
    errors: list[str] = []
    try:
        _model_index(spec_model)
    except (KeyError, TypeError, ValueError) as exc:
        errors.append(f"SPEC model invalid: {exc}")
    if manifest_model != spec_model:
        errors.append("manifest journal-transition projection differs from SPEC")
    if packet_model != spec_model:
        errors.append("packet journal-transition projection differs from SPEC")
    return errors


MODEL_BEGIN = "<!-- NG039-JOURNAL-MODEL-BEGIN -->"
MODEL_END = "<!-- NG039-JOURNAL-MODEL-END -->"
PINS_BEGIN = "<!-- NG039-ARTIFACT-PINS-BEGIN -->"
PINS_END = "<!-- NG039-ARTIFACT-PINS-END -->"


def _reject_duplicate_pairs(pairs: list[tuple[str, Any]]) -> dict[str, Any]:
    result: dict[str, Any] = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def _reject_non_finite(value: str) -> None:
    raise ValueError(f"non-finite JSON value: {value}")


def strict_json_loads(text: str) -> Any:
    """Parse JSON while rejecting duplicate keys and NaN/Infinity values."""
    return json.loads(
        text,
        object_pairs_hook=_reject_duplicate_pairs,
        parse_constant=_reject_non_finite,
    )


def extract_model(markdown: str) -> dict[str, Any]:
    """Extract the sole canonical model block from a SPEC/packet Markdown file."""
    if markdown.count(MODEL_BEGIN) != 1 or markdown.count(MODEL_END) != 1:
        raise ValueError("expected exactly one marked journal-transition model block")
    pattern = re.compile(
        re.escape(MODEL_BEGIN) + r"\s*```json\s*(.*?)\s*```\s*" + re.escape(MODEL_END),
        re.DOTALL,
    )
    matches = list(pattern.finditer(markdown))
    if len(matches) != 1:
        raise ValueError("journal-transition model markers do not enclose one JSON block")
    model = strict_json_loads(matches[0].group(1))
    _model_index(model)
    return model


def extract_pins(markdown: str) -> dict[str, str]:
    """Extract exactly one packet pin object, excluding the packet's own digest."""
    if markdown.count(PINS_BEGIN) != 1 or markdown.count(PINS_END) != 1:
        raise ValueError("expected exactly one marked artifact-pin block")
    pattern = re.compile(
        re.escape(PINS_BEGIN) + r"\s*```json\s*(.*?)\s*```\s*" + re.escape(PINS_END),
        re.DOTALL,
    )
    matches = list(pattern.finditer(markdown))
    if len(matches) != 1:
        raise ValueError("artifact-pin markers do not enclose one JSON block")
    pins = strict_json_loads(matches[0].group(1))
    required = {"document_head", "spec_sha256", "manifest_sha256"}
    if not isinstance(pins, dict) or set(pins) != required or any(not isinstance(v, str) for v in pins.values()):
        raise ValueError("artifact pins must contain exactly HEAD, SPEC SHA and manifest SHA")
    return pins


def audit_contract(
    spec_text: str,
    manifest: object,
    packet_text: str,
    actual_spec_sha256: str,
    actual_manifest_sha256: str,
    expected_head: str,
) -> list[str]:
    """Check exact model projections, relevant schema values and artifact pins."""
    errors: list[str] = []

    def read_path(root: object, *keys: str) -> Any:
        current = root
        for key in keys:
            if not isinstance(current, dict) or key not in current:
                raise KeyError(".".join(keys))
            current = current[key]
        return current

    def expect(label: str, actual: object, required: object) -> None:
        if actual != required:
            errors.append(f"{label} differs from the canonical SPEC model or frozen identity")

    try:
        spec_model = extract_model(spec_text)
    except (TypeError, ValueError) as exc:
        errors.append(f"SPEC model invalid: {exc}")
        spec_model = None
    try:
        packet_model = extract_model(packet_text)
    except (TypeError, ValueError) as exc:
        errors.append(f"packet model invalid: {exc}")
        packet_model = None
    if spec_model is not None:
        try:
            manifest_model = read_path(manifest, "classification_rules", "journal_transition_model")
        except KeyError as exc:
            errors.append(f"manifest model missing: {exc}")
            manifest_model = None
        errors.extend(check_model_projections(spec_model, manifest_model, packet_model))
        outcomes = spec_model.get("cleanup_outcomes") if isinstance(spec_model, dict) else None
        if isinstance(outcomes, dict):
            expected_outcomes = set(outcomes)
            mode_projection = {
                "model_id": spec_model["model_id"],
                "authority_modes": spec_model["authority_modes"],
                "mode_effects": spec_model["mode_effects"],
            }
            try:
                event_enum = set(read_path(manifest, "evidence_schemas", "reservation_journal_event", "properties", "event", "enum"))
                expect("reservation journal schema event enum", event_enum, set(spec_model["required_events"]))
                identity_properties = read_path(manifest, "evidence_schemas", "reservation_journal_event", "properties", "identity", "properties")
                for resource_type, fields in spec_model["identity_binding"].items():
                    if not set(fields) <= set(identity_properties):
                        errors.append(f"reservation journal identity schema omits binding fields for {resource_type}")
            except (KeyError, TypeError):
                errors.append("reservation journal event/identity schema projection missing")
            try:
                expect(
                    "manifest authority mode projection",
                    read_path(manifest, "classification_rules", "authority_mode_projection"),
                    spec_model["authority_modes"],
                )
                expect(
                    "manifest mode effect projection",
                    read_path(manifest, "classification_rules", "mode_effect_projection"),
                    spec_model["mode_effects"],
                )
                expect(
                    "manifest cleanup outcome requirement IDs",
                    read_path(manifest, "classification_rules", "cleanup_outcome_requirement_ids"),
                    [outcomes[name]["requirement_id"] for name in ("not_started", "partial", "complete", "unknown")],
                )
            except KeyError as exc:
                errors.append(f"manifest mode projection missing: {exc}")
            projection_paths = (
                ("classification_rules", "cleanup_outcome_projection"),
                ("sandbox", "finalizer_ipc_boundary", "cleanup_outcome_classifier"),
                ("sandbox", "finalizer_ipc_boundary", "abnormal_evidence_only", "cleanup_outcome_classifier"),
                ("semantic_finalizer_contract", "abnormal_evidence_only", "cleanup_outcome"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "abnormal_evidence_only", "cleanup_outcome_classifier"),
            )
            for path in projection_paths:
                try:
                    expect("manifest." + ".".join(path), read_path(manifest, *path), outcomes)
                except KeyError:
                    errors.append("manifest projection missing: " + ".".join(path))
            for path in (
                ("semantic_finalizer_contract", "normal_cleanup_failure"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "normal_cleanup_failure"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "failure_rule"),
                ("sandbox", "finalizer_ipc_boundary", "failure_rule"),
            ):
                try:
                    expect("manifest." + ".".join(path), read_path(manifest, *path), spec_model["normal_cleanup_failure"])
                except KeyError:
                    errors.append("manifest normal-failure projection missing: " + ".".join(path))
            for path in (
                ("sandbox", "finalizer_mode_selection"),
                ("sandbox", "finalizer_ipc_boundary", "mode_selection"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "mode_selection"),
            ):
                try:
                    expect("manifest." + ".".join(path), read_path(manifest, *path), mode_projection)
                except KeyError:
                    errors.append("manifest mode-effects projection missing: " + ".".join(path))
            for path in (
                ("evidence_schemas", "run_record", "properties", "failure", "oneOf", 1, "properties", "cleanup_outcome", "enum"),
                ("evidence_schemas", "run_record", "allOf", -1, "then", "properties", "failure", "properties", "cleanup_outcome", "enum"),
            ):
                try:
                    current = manifest
                    for key in path:
                        current = current[key] if isinstance(current, (dict, list)) else None
                    expect("manifest schema " + ".".join(map(str, path)), set(current), expected_outcomes)
                except (KeyError, IndexError, TypeError):
                    errors.append("manifest cleanup-outcome schema enum missing: " + ".".join(map(str, path)))

    for label, path, target in (
        ("manifest SPEC digest", ("spec", "sha256"), actual_spec_sha256),
        ("run-record SPEC digest", ("evidence_schemas", "run_record", "properties", "spec_sha256", "const"), actual_spec_sha256),
        ("manifest document HEAD", ("source", "document_checkout_head"), expected_head),
        ("manifest review revision", ("review_revision",), "r13"),
        ("manifest SPEC version", ("spec", "version"), "v1.15"),
        ("manifest Astra submission state", ("local_validation_this_freeze", "astra_submitted"), False),
        ("manifest exact-SHA consent state", ("local_validation_this_freeze", "owner_exact_sha_consent_for_r13"), False),
        ("manifest review-artifact state", ("review_artifacts", "submission_state"), "UNSUBMITTED_CONSENT_REQUIRED"),
    ):
        try:
            expect(label, read_path(manifest, *path), target)
        except KeyError:
            errors.append("manifest identity missing: " + ".".join(path))
    try:
        pins = extract_pins(packet_text)
        expected_pins = {
            "document_head": expected_head,
            "spec_sha256": actual_spec_sha256,
            "manifest_sha256": actual_manifest_sha256,
        }
        expect("packet artifact pins", pins, expected_pins)
    except (TypeError, ValueError) as exc:
        errors.append(f"packet pins invalid: {exc}")
    return errors
