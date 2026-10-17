#!/usr/bin/env python3
"""RED probe: r12 must reject REMOVED without a cleanup intent."""
from __future__ import annotations

import importlib.util
import sys
import types
import unittest
from pathlib import Path

# The fallback runs the frozen r12 classifier until the replacement exists;
# the import stub isolates semantic replay and does not claim schema validation.
jsonschema_stub = types.ModuleType("jsonschema")
jsonschema_stub.Draft202012Validator = type(
    "Draft202012Validator",
    (),
    {"__init__": lambda self, schema: None, "validate": lambda self, item: None},
)
sys.modules.setdefault("jsonschema", jsonschema_stub)

LEGACY = Path(__file__).with_name("verify_spec039_v14_r12.py")
spec = importlib.util.spec_from_file_location("legacy_r12", LEGACY)
if not spec or not spec.loader:
    raise RuntimeError("could not load frozen r12 classifier for the RED probe")
legacy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(legacy)
try:
    import spec039_methodology_v1 as method
except ImportError:
    method = None

MODEL = {
    "model_id": "ng039-journal-finalization-model",
    "model_version": 1,
    "schema_version": "ng039-reservation-journal-event/v2",
    "journal_states": ["absent", "open", "shutdown_complete", "closed"],
    "journal_initial_state": "absent",
    "journal_terminal_state": "closed",
    "resource_states": ["none", "create_pending", "create_failed", "created", "cleanup_pending", "removed", "cleanup_failed"],
    "resource_initial_state": "none",
    "resource_kinds": {"journal": "journal", "candidates": ["sysv_shm", "posix_semaphore"]},
    "identity_binding": {"sysv_shm": ["candidate_key", "shmid"], "posix_semaphore": ["source_name", "device", "inode"]},
    "required_events": ["JOURNAL_OPENED", "CREATE_INTENT", "CREATED", "CREATE_FAILED", "RUN_SHUTDOWN_COMPLETE", "CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED", "JOURNAL_CLOSED"],
    "events": [
        {"requirement_id": "JRN-001", "event": "JOURNAL_OPENED", "scope": "journal", "phase_from": ["absent"], "phase_to": "open", "resource_from": None, "resource_to": None},
        {"requirement_id": "JRN-002", "event": "CREATE_INTENT", "scope": "resource", "phase_from": ["open"], "phase_to": "open", "resource_from": "none", "resource_to": "create_pending"},
        {"requirement_id": "JRN-003", "event": "CREATED", "scope": "resource", "phase_from": ["open"], "phase_to": "open", "resource_from": "create_pending", "resource_to": "created"},
        {"requirement_id": "JRN-004", "event": "CREATE_FAILED", "scope": "resource", "phase_from": ["open"], "phase_to": "open", "resource_from": "create_pending", "resource_to": "create_failed"},
        {"requirement_id": "JRN-005", "event": "RUN_SHUTDOWN_COMPLETE", "scope": "journal", "phase_from": ["open"], "phase_to": "shutdown_complete", "resource_from": None, "resource_to": None, "requires_no_resource_states": ["create_pending"]},
        {"requirement_id": "JRN-006", "event": "CLEANUP_INTENT", "scope": "resource", "phase_from": ["shutdown_complete"], "phase_to": "shutdown_complete", "resource_from": "created", "resource_to": "cleanup_pending"},
        {"requirement_id": "JRN-007", "event": "REMOVED", "scope": "resource", "phase_from": ["shutdown_complete"], "phase_to": "shutdown_complete", "resource_from": "cleanup_pending", "resource_to": "removed"},
        {"requirement_id": "JRN-008", "event": "CLEANUP_FAILED", "scope": "resource", "phase_from": ["shutdown_complete"], "phase_to": "shutdown_complete", "resource_from": "cleanup_pending", "resource_to": "cleanup_failed"},
        {"requirement_id": "JRN-009", "event": "JOURNAL_CLOSED", "scope": "journal", "phase_from": ["shutdown_complete"], "phase_to": "closed", "resource_from": None, "resource_to": None, "requires_no_resource_states": ["create_pending", "cleanup_pending"]},
    ],
    "authority_modes": {
        "requirement_ids": {"common": "AUTH-001", "normal": "AUTH-002", "abnormal": "AUTH-003", "blocked": "AUTH-004"},
        "common_requirements": ["trusted_launcher", "valid_supervisor_receipt", "all_pids_reaped", "logs_closed", "stable_result_directory", "continuous_lease"],
        "normal_requirements": ["durable_shutdown_complete", "normal_waits_valid", "no_timeout_or_signal"],
        "mode_names": {"normal": "normal_cleanup", "abnormal": "abnormal_evidence_only", "blocked": "blocked"},
    },
    "mode_effects": {
        "normal_cleanup": {"requirement_id": "AUTH-002", "status": ["PASS", "FAIL", "INCONCLUSIVE"], "requires": ["full_bundle_schema_journal_replay", "complete_inventory_verification", "exact_journal_authorized_cleanup"], "forbidden": ["cleanup_before_shutdown_complete", "broad_host_cleanup"]},
        "abnormal_evidence_only": {"requirement_id": "AUTH-003", "status": ["INCONCLUSIVE"], "requires": ["common_authority_gates_true", "strict_pending_record_supervisor_receipt", "bounded_raw_evidence_preservation"], "forbidden": ["live_ipc_inspection", "cleanup", "journal_mutation", "retry", "PASS", "FAIL"]},
        "blocked": {"requirement_id": "AUTH-004", "status": ["BLOCKED", "INCONCLUSIVE"], "requires": ["preserve_pending_evidence"], "forbidden": ["finalizer_entry", "finalizer_reentry", "cleanup", "unsafe_record_write"]},
    },
    "cleanup_outcomes": {
        "not_started": {"requirement_id": "COUT-001", "journal_required": "complete_transition_valid_closed", "created_count": 0, "cleanup_intent_count": 0, "create_failed_allowed": True, "run_status_effect": "NO_PASS_INFERENCE"},
        "partial": {"requirement_id": "COUT-002", "journal_required": "complete_transition_valid_closed", "all_created_have_one_terminal_outcome": True, "cleanup_failed_count_min": 1, "run_status_effect": "INCONCLUSIVE"},
        "complete": {"requirement_id": "COUT-003", "journal_required": "complete_transition_valid_closed", "created_count_min": 1, "all_created_removed": True, "cleanup_failed_count": 0, "run_status_effect": "DOES_NOT_IMPLY_PASS"},
        "unknown": {"requirement_id": "COUT-004", "journal_required": "otherwise", "run_status_effect": "INCONCLUSIVE"},
    },
    "normal_cleanup_failure": {
        "requirement_id": "FIN-001",
        "stop_destructive_calls_after_failure": True,
        "retry_after_intent": False,
        "outcome_source": "complete_transition_valid_closed_journal",
        "complete_history_after_journal_closed_later_evidence_failure": {"cleanup_outcome": "complete", "run_status": "INCONCLUSIVE"},
        "authority_loss": {"finalizer_entry": False, "finalizer_reentry": False, "preserve_pending": True, "status": "BLOCKED/INCONCLUSIVE"},
    },
}


def event(
    sequence: int,
    name: str,
    resource: str = "journal",
    key: int | None = None,
    source: str | None = None,
    error: int | None = None,
    identity: dict[str, object] | None = None,
    run_id: str = "NG039-D2-0123456789",
) -> dict[str, object]:
    return {
        "schema_version": "ng039-reservation-journal-event/v2",
        "run_id": run_id,
        "sequence": sequence,
        "event": name,
        "monotonic_ns": sequence,
        "pid": 2345,
        "uid": 1001,
        "resource_type": resource,
        "candidate_key": key,
        "source_name": source,
        "error_number": error,
        "identity": identity,
    }


def record(name: str, resource: str = "journal", key: int | None = None, source: str | None = None, error: int | None = None, identity: dict[str, object] | None = None) -> tuple[object, ...]:
    return name, resource, key, source, error, identity


def shm_identity(key: int, shmid: int) -> dict[str, int]:
    return {"candidate_key": key, "shmid": shmid}


def semaphore_identity(name: str, device: int, inode: int) -> dict[str, object]:
    return {"source_name": name, "device": device, "inode": inode}


def journal(*records: tuple[object, ...]) -> list[dict[str, object]]:
    return [event(i, *record) for i, record in enumerate(records, 1)]


def classify(events: list[object]) -> str:
    if method is None:
        return legacy.classify(events, {})
    return method.classify_journal(events, MODEL)


def mode(gates: dict[str, object], model: dict[str, object] = MODEL) -> str:
    if method is None or not hasattr(method, "select_finalization_mode"):
        return "abnormal_evidence_only"
    return method.select_finalization_mode(gates, model)


class FinalizationModeTests(unittest.TestCase):
    def setUp(self) -> None:
        self.common = {key: True for key in MODEL["authority_modes"]["common_requirements"]}
        self.normal = {key: True for key in MODEL["authority_modes"]["normal_requirements"]}

    def test_lost_lease_blocks_instead_of_selecting_abnormal_mode(self) -> None:
        gates = {**self.common, **self.normal, "continuous_lease": False}
        self.assertEqual("blocked", mode(gates))

    def test_missing_supervisor_proof_blocks_both_modes(self) -> None:
        gates = {**self.common, "valid_supervisor_receipt": None}
        self.assertEqual("blocked", mode(gates))

    def test_common_authority_without_normal_proof_selects_abnormal_evidence_only(self) -> None:
        self.assertEqual("abnormal_evidence_only", mode(self.common))

    def test_all_common_and_normal_prerequisites_select_normal_cleanup(self) -> None:
        self.assertEqual("normal_cleanup", mode({**self.common, **self.normal}))

    def test_invalid_authority_model_blocks_mode_selection(self) -> None:
        authority = {
            **MODEL["authority_modes"],
            "common_requirements": [key for key in MODEL["authority_modes"]["common_requirements"] if key != "continuous_lease"],
        }
        invalid_model = {**MODEL, "authority_modes": authority}
        gates = {key: True for key in authority["common_requirements"]}
        self.assertEqual("blocked", mode(gates, invalid_model))


class JournalTransitionTests(unittest.TestCase):
    def test_removed_without_cleanup_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_no_created_resources_is_not_started(self) -> None:
        events = journal(record("JOURNAL_OPENED"), record("RUN_SHUTDOWN_COMPLETE"), record("JOURNAL_CLOSED"))
        self.assertEqual("not_started", classify(events))

    def test_failed_creation_without_cleanup_is_not_started(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATE_FAILED", "sysv_shm", 11, error=12),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("not_started", classify(events))

    def test_successful_cleanup_is_complete(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("complete", classify(events))

    def test_failed_cleanup_is_partial_only_when_all_created_resources_are_accounted(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CREATE_INTENT", "sysv_shm", 12),
            record("CREATED", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CLEANUP_FAILED", "sysv_shm", 11, error=16, identity=shm_identity(11, 77)),
            record("CLEANUP_INTENT", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("REMOVED", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("partial", classify(events))

    def test_created_resource_with_no_cleanup_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_uncovered_created_resource_is_unknown_even_if_another_was_removed(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CREATE_INTENT", "sysv_shm", 12),
            record("CREATED", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_removed_cannot_consume_create_intent_after_create_failed(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATE_FAILED", "sysv_shm", 11, error=12),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_created_resource_cannot_be_removed_without_cleanup_intent(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_intent_before_shutdown_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_create_intent_after_shutdown_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_shutdown_with_open_create_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_intent_without_terminal_outcome_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_posix_semaphore_identity_uses_name_not_key(self) -> None:
        name = "/ng039-test-sem"
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "posix_semaphore", source=name),
            record("CREATED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("REMOVED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("complete", classify(events))

    def test_posix_inode_mismatch_is_unknown(self) -> None:
        name = "/ng039-test-sem"
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "posix_semaphore", source=name),
            record("CREATED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 82)),
            record("REMOVED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 82)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_intent_identity_must_match_created_resource(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 78)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 78)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_result_identity_must_match_created_resource(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 78)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_created_resource_requires_identity_evidence(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11),
            record("REMOVED", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_created_without_create_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_create_failed_without_create_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_FAILED", "sysv_shm", 11, error=12),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_identity_key_must_match_top_level_candidate_key(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(12, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_duplicate_cleanup_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_duplicate_cleanup_terminal_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_failed_without_cleanup_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_FAILED", "sysv_shm", 11, error=16, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_missing_shutdown_or_events_after_close_are_unknown(self) -> None:
        missing_shutdown = journal(record("JOURNAL_OPENED"), record("JOURNAL_CLOSED"))
        after_close = journal(
            record("JOURNAL_OPENED"),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
            record("REMOVED", "sysv_shm", 11),
        )
        self.assertEqual("unknown", classify(missing_shutdown))
        self.assertEqual("unknown", classify(after_close))

    def test_sequence_time_and_run_identity_corruption_are_unknown(self) -> None:
        valid = journal(record("JOURNAL_OPENED"), record("RUN_SHUTDOWN_COMPLETE"), record("JOURNAL_CLOSED"))
        bad_sequence = [dict(item) for item in valid]
        bad_sequence[1]["sequence"] = 4
        bad_time = [dict(item) for item in valid]
        bad_time[2]["monotonic_ns"] = 0
        bad_run = [dict(item) for item in valid]
        bad_run[2]["run_id"] = "different-run"
        for invalid in (bad_sequence, bad_time, bad_run, [], ["not-an-event"]):
            self.assertEqual("unknown", classify(invalid))


if __name__ == "__main__":
    unittest.main(verbosity=2)
