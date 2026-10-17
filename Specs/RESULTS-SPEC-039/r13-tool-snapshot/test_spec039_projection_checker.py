#!/usr/bin/env python3
"""Tests for structured SPEC/manifest/packet contract projection checks."""
from __future__ import annotations

import copy
import json
import unittest

import test_spec039_journal_transitions as transitions

try:
    import spec039_methodology_v1 as method
except ImportError:
    method = None

MODEL = transitions.MODEL
BEGIN = "<!-- NG039-JOURNAL-MODEL-BEGIN -->"
END = "<!-- NG039-JOURNAL-MODEL-END -->"
PINS_BEGIN = "<!-- NG039-ARTIFACT-PINS-BEGIN -->"
PINS_END = "<!-- NG039-ARTIFACT-PINS-END -->"
HEAD = "756f493b7049434ab67cf2382486a48f606a470c"
SPEC_SHA = "a" * 64
MANIFEST_SHA = "b" * 64


def check(spec_model: dict[str, object], manifest_model: dict[str, object], packet_model: dict[str, object]) -> list[str]:
    if method is None or not hasattr(method, "check_model_projections"):
        return []
    return method.check_model_projections(spec_model, manifest_model, packet_model)


def extract(markdown: str) -> dict[str, object]:
    if method is None or not hasattr(method, "extract_model"):
        return {}
    return method.extract_model(markdown)


def strict_loads(text: str) -> object:
    if method is None or not hasattr(method, "strict_json_loads"):
        return json.loads(text)
    return method.strict_json_loads(text)


def model_block(raw_json: str) -> str:
    return f"{BEGIN}\n```json\n{raw_json}\n```\n{END}"


def pins_block(pins: dict[str, str]) -> str:
    return f"{PINS_BEGIN}\n```json\n{json.dumps(pins, sort_keys=True)}\n```\n{PINS_END}"


def manifest_fixture() -> dict[str, object]:
    outcomes = copy.deepcopy(MODEL["cleanup_outcomes"])
    mode_projection = {"model_id": MODEL["model_id"], "authority_modes": copy.deepcopy(MODEL["authority_modes"]), "mode_effects": copy.deepcopy(MODEL["mode_effects"])}
    normal_failure = copy.deepcopy(MODEL["normal_cleanup_failure"])
    enum_values = sorted(outcomes)
    boundary = {
        "mode_selection": copy.deepcopy(mode_projection),
        "failure_rule": copy.deepcopy(normal_failure),
        "normal_cleanup_failure": copy.deepcopy(normal_failure),
        "cleanup_outcome_classifier": copy.deepcopy(outcomes),
        "abnormal_evidence_only": {"cleanup_outcome_classifier": copy.deepcopy(outcomes)},
    }
    return {
        "review_revision": "r13",
        "spec": {"sha256": SPEC_SHA, "version": "v1.15"},
        "source": {"document_checkout_head": HEAD},
        "review_artifacts": {"submission_state": "UNSUBMITTED_CONSENT_REQUIRED"},
        "local_validation_this_freeze": {"astra_submitted": False, "owner_exact_sha_consent_for_r13": False},
        "classification_rules": {
            "journal_transition_model": copy.deepcopy(MODEL),
            "cleanup_outcome_projection": outcomes,
            "cleanup_outcome_requirement_ids": [outcomes[name]["requirement_id"] for name in ("not_started", "partial", "complete", "unknown")],
            "authority_mode_projection": copy.deepcopy(MODEL["authority_modes"]),
            "mode_effect_projection": copy.deepcopy(MODEL["mode_effects"]),
        },
        "evidence_schemas": {
            "reservation_journal_event": {
                "properties": {
                    "event": {"enum": list(MODEL["required_events"])},
                    "identity": {"properties": {field: {} for fields in MODEL["identity_binding"].values() for field in fields}},
                }
            },
            "run_record": {
                "properties": {
                    "spec_sha256": {"const": SPEC_SHA},
                    "failure": {"oneOf": [{"type": "null"}, {"properties": {"cleanup_outcome": {"enum": enum_values}}}]},
                },
                "allOf": [{}, {"then": {"properties": {"failure": {"properties": {"cleanup_outcome": {"enum": enum_values}}}}}}],
            }
        },
        "sandbox": {
            "finalizer_mode_selection": copy.deepcopy(mode_projection),
            "finalizer_ipc_boundary": copy.deepcopy(boundary),
        },
        "semantic_finalizer_contract": {
            "normal_cleanup_failure": copy.deepcopy(normal_failure),
            "abnormal_evidence_only": {"cleanup_outcome": copy.deepcopy(outcomes)},
            "finalizer_ipc_boundary": copy.deepcopy(boundary),
        },
    }


def audit(manifest: dict[str, object], pins: dict[str, str] | None = None) -> list[str]:
    if method is None or not hasattr(method, "audit_contract"):
        return []
    if pins is None:
        pins = {"document_head": HEAD, "spec_sha256": SPEC_SHA, "manifest_sha256": MANIFEST_SHA}
    spec_text = model_block(json.dumps(MODEL, indent=2))
    packet_text = pins_block(pins) + "\\n" + model_block(json.dumps(MODEL, indent=2))
    return method.audit_contract(spec_text, manifest, packet_text, SPEC_SHA, MANIFEST_SHA, HEAD)


class ProjectionCheckerTests(unittest.TestCase):
    def test_equal_projections_are_accepted(self) -> None:
        self.assertEqual([], check(MODEL, MODEL, MODEL))

    def test_manifest_projection_drift_is_reported(self) -> None:
        stale = copy.deepcopy(MODEL)
        stale["events"][6]["resource_from"] = "create_pending"
        errors = check(MODEL, stale, MODEL)
        self.assertTrue(errors, "a stale manifest transition must be detected")

    def test_packet_projection_drift_is_reported(self) -> None:
        stale = copy.deepcopy(MODEL)
        stale["cleanup_outcomes"]["complete"]["run_status_effect"] = "PASS"
        errors = check(MODEL, MODEL, stale)
        self.assertTrue(errors, "a packet cleanup-outcome contradiction must be detected")

    def test_duplicate_requirement_ids_are_reported(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["events"][1]["requirement_id"] = invalid["events"][0]["requirement_id"]
        errors = check(invalid, invalid, invalid)
        self.assertTrue(errors, "requirement IDs must be unique")

    def test_unknown_model_field_is_rejected(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["cleanup_outcomes"]["complete"]["status_override"] = "PASS"
        self.assertTrue(check(invalid, invalid, invalid))

    def test_cleanup_outcome_disposition_set_must_be_closed(self) -> None:
        invalid = copy.deepcopy(MODEL)
        del invalid["cleanup_outcomes"]["partial"]
        self.assertTrue(check(invalid, invalid, invalid))

    def test_transition_table_cannot_reuse_creation_state_for_removal(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["events"][6]["resource_from"] = "create_pending"
        self.assertTrue(check(invalid, invalid, invalid))

    def test_common_authority_requirements_are_mandatory(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["authority_modes"]["common_requirements"].remove("continuous_lease")
        self.assertTrue(check(invalid, invalid, invalid))

    def test_cleanup_history_cannot_authorize_pass(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["cleanup_outcomes"]["complete"]["run_status_effect"] = "PASS"
        self.assertTrue(check(invalid, invalid, invalid))

    def test_abnormal_mode_cannot_allow_cleanup_or_ipc(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["mode_effects"]["abnormal_evidence_only"]["forbidden"].remove("cleanup")
        self.assertTrue(check(invalid, invalid, invalid))

    def test_normal_mode_requires_closed_status_set(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["mode_effects"]["normal_cleanup"]["status"] = ["PASS"]
        self.assertTrue(check(invalid, invalid, invalid))

    def test_model_block_is_extracted_as_structured_json(self) -> None:
        text = model_block(json.dumps(MODEL, indent=2))
        self.assertEqual(MODEL, extract(text))

    def test_duplicate_model_blocks_are_rejected(self) -> None:
        text = model_block(json.dumps(MODEL)) + "\\n" + model_block(json.dumps(MODEL))
        with self.assertRaises(ValueError):
            extract(text)

    def test_duplicate_json_keys_are_rejected(self) -> None:
        with self.assertRaises(ValueError):
            strict_loads('{"event":"A","event":"B"}')

    def test_missing_model_block_is_rejected(self) -> None:
        with self.assertRaises(ValueError):
            extract("no canonical model here")

    def test_complete_artifact_bindings_are_accepted(self) -> None:
        self.assertEqual([], audit(manifest_fixture()))

    def test_manifest_spec_digest_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["spec"]["sha256"] = "c" * 64
        self.assertTrue(audit(manifest))

    def test_schema_spec_digest_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["evidence_schemas"]["run_record"]["properties"]["spec_sha256"]["const"] = "c" * 64
        self.assertTrue(audit(manifest))

    def test_packet_manifest_pin_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        pins = {"document_head": HEAD, "spec_sha256": SPEC_SHA, "manifest_sha256": "c" * 64}
        self.assertTrue(audit(manifest, pins))

    def test_cleanup_outcome_schema_enum_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["evidence_schemas"]["run_record"]["properties"]["failure"]["oneOf"][1]["properties"]["cleanup_outcome"]["enum"].remove("complete")
        self.assertTrue(audit(manifest))

    def test_nested_cleanup_projection_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["semantic_finalizer_contract"]["finalizer_ipc_boundary"]["abnormal_evidence_only"]["cleanup_outcome_classifier"]["complete"]["run_status"] = "PASS"
        self.assertTrue(audit(manifest))

    def test_stale_review_revision_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["review_revision"] = "r12"
        self.assertTrue(audit(manifest))

    def test_manifest_consent_or_submission_flag_is_rejected(self) -> None:
        manifest = manifest_fixture()
        manifest["local_validation_this_freeze"]["astra_submitted"] = True
        self.assertTrue(audit(manifest))
        manifest = manifest_fixture()
        manifest["local_validation_this_freeze"]["owner_exact_sha_consent_for_r13"] = True
        self.assertTrue(audit(manifest))

    def test_journal_schema_event_enum_matches_model(self) -> None:
        manifest = manifest_fixture()
        manifest["evidence_schemas"]["reservation_journal_event"]["properties"]["event"]["enum"].remove("CLEANUP_FAILED")
        self.assertTrue(audit(manifest))

    def test_identity_binding_fields_exist_in_journal_schema(self) -> None:
        manifest = manifest_fixture()
        del manifest["evidence_schemas"]["reservation_journal_event"]["properties"]["identity"]["properties"]["shmid"]
        self.assertTrue(audit(manifest))

    def test_authority_mode_projection_matches_model(self) -> None:
        manifest = manifest_fixture()
        manifest["classification_rules"]["authority_mode_projection"]["common_requirements"].remove("continuous_lease")
        self.assertTrue(audit(manifest))

    def test_mode_effect_projection_matches_model(self) -> None:
        manifest = manifest_fixture()
        manifest["classification_rules"]["mode_effect_projection"]["abnormal_evidence_only"]["forbidden"].remove("cleanup")
        self.assertTrue(audit(manifest))


if __name__ == "__main__":
    unittest.main(verbosity=2)
