import hashlib
import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from evidence_verifier import (
    CODE_BUNDLE_MISSING,
    CODE_SCHEMA_OLD,
    CODE_TAMPERED,
    CODE_TERMINAL_SEAL_MISSING,
    CODE_VALID,
    verify_bundle,
)


class EvidenceVerifierTests(unittest.TestCase):
    def _write_json(self, path: Path, value: object) -> None:
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(json.dumps(value, sort_keys=True, separators=(",", ":")) + "\n", encoding="utf-8")

    def _make_bundle(
        self,
        root: Path,
        *,
        accepted: bool = True,
        result_updates: dict[str, object] | None = None,
        manifest_updates: dict[str, object] | None = None,
    ) -> Path:
        bundle = root / "bundle"
        files = {
            "result.json": {
                "schema_version": 2,
                "component_verdicts": {
                    "runtime": "PASS",
                    "teardown": "PASS",
                    "evidence": "COMPLETE",
                },
                "exit_code": 0,
                "local_acceptance_eligible": accepted,
                "blockers": [] if accepted else ["example-blocker"],
            },
            "controller-events.jsonl": '{"phase":"seal","outcome":"PASS"}\n',
            "provenance.json": {"capture_complete": True, "build_linkage": True},
            "workload.json": {"expected": [{"name": "photo.jpg", "size": 7, "sha256": hashlib.sha256(b"payload").hexdigest()}]},
            "oracle.json": {"source": {}, "repository": {}, "sticky_divergence": False},
            "ownership.json": {"resources": []},
            "cleanup.json": {"final_absence": True},
            "preservation.json": {"verified": True},
            "plan/original.json": {"schema_version": 1},
            "roles/Source/stdout.log": "READY\n",
            "inventory/final.json": {"processes": [], "ipc": []},
            "artifacts/source/photo.jpg": b"payload",
            "artifacts/repository/photo.jpg": b"payload",
        }
        if result_updates:
            files["result.json"].update(result_updates)
        for relative, value in files.items():
            path = bundle / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            if isinstance(value, bytes):
                path.write_bytes(value)
            elif isinstance(value, str):
                path.write_text(value, encoding="utf-8")
            else:
                self._write_json(path, value)

        entries = []
        for path in sorted(p for p in bundle.rglob("*") if p.is_file()):
            relative = path.relative_to(bundle).as_posix()
            data = path.read_bytes()
            entries.append({"path": relative, "size": len(data), "sha256": hashlib.sha256(data).hexdigest()})
        manifest = {"schema_version": 2, "files": entries}
        if manifest_updates:
            manifest.update(manifest_updates)
        manifest_path = bundle / "manifest.json"
        self._write_json(manifest_path, manifest)
        manifest_hash = hashlib.sha256(manifest_path.read_bytes()).hexdigest()
        self._write_json(bundle / "terminal-seal.json", {"schema_version": 2, "manifest_sha256": manifest_hash})
        return bundle

    def test_minimum_schema_v2_bundle_is_valid_and_accepted(self):
        with tempfile.TemporaryDirectory() as td:
            report = verify_bundle(self._make_bundle(Path(td)))
            self.assertEqual(report["code"], CODE_VALID)
            self.assertTrue(report["integrity"])
            self.assertTrue(report["accepted"])
            self.assertEqual(report["schema_version"], 2)

    def test_missing_bundle_has_distinct_code_and_is_not_accepted(self):
        with tempfile.TemporaryDirectory() as td:
            report = verify_bundle(Path(td) / "does-not-exist")
            self.assertEqual(report["code"], CODE_BUNDLE_MISSING)
            self.assertFalse(report["integrity"])
            self.assertFalse(report["accepted"])

    def test_schema_v1_is_reported_as_old_before_seal_validation(self):
        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td))
            manifest = json.loads((bundle / "manifest.json").read_text(encoding="utf-8"))
            manifest["schema_version"] = 1
            self._write_json(bundle / "manifest.json", manifest)
            report = verify_bundle(bundle)
            self.assertEqual(report["code"], CODE_SCHEMA_OLD)
            self.assertFalse(report["integrity"])
            self.assertFalse(report["accepted"])

    def test_missing_terminal_seal_is_not_valid(self):
        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td))
            (bundle / "terminal-seal.json").unlink()
            report = verify_bundle(bundle)
            self.assertEqual(report["code"], CODE_TERMINAL_SEAL_MISSING)
            self.assertFalse(report["integrity"])
            self.assertFalse(report["accepted"])

    def test_changed_payload_is_reported_as_tampered(self):
        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td))
            (bundle / "artifacts/source/photo.jpg").write_bytes(b"changed")
            report = verify_bundle(bundle)
            self.assertEqual(report["code"], CODE_TAMPERED)
            self.assertFalse(report["integrity"])
            self.assertFalse(report["accepted"])

    def test_unaccepted_result_reports_explicit_blocker_without_losing_integrity(self):
        with tempfile.TemporaryDirectory() as td:
            report = verify_bundle(self._make_bundle(Path(td), accepted=False))
            self.assertEqual(report["code"], CODE_VALID)
            self.assertTrue(report["integrity"])
            self.assertFalse(report["accepted"])
            self.assertEqual(
                report["errors"],
                [
                    "NGELC-ACCEPTANCE-INELIGIBLE: bundle is intact but not acceptance-eligible",
                    "NGELC-ACCEPTANCE-BLOCKER: acceptance blocker present (result.blockers)",
                ],
            )

    def test_acceptance_blocker_aliases_are_all_reported_without_precedence_loss(self):
        with tempfile.TemporaryDirectory() as td:
            report = verify_bundle(
                self._make_bundle(
                    Path(td),
                    result_updates={
                        "local_acceptance_eligible": False,
                        "blockers": [],
                        "acceptance_blockers": ["top-level alias"],
                        "acceptance_eligibility": {"eligible": False, "blockers": ["nested alias"]},
                    },
                )
            )
            self.assertEqual(report["code"], CODE_VALID)
            self.assertTrue(report["integrity"])
            self.assertFalse(report["accepted"])
            self.assertEqual(
                report["errors"],
                [
                    "NGELC-ACCEPTANCE-INELIGIBLE: bundle is intact but not acceptance-eligible",
                    "NGELC-ACCEPTANCE-BLOCKER: acceptance blocker present (result.acceptance_blockers)",
                    "NGELC-ACCEPTANCE-BLOCKER: acceptance blocker present (result.acceptance_eligibility.blockers)",
                ],
            )

    def test_protected_input_blockers_from_result_and_manifest_are_safe_and_aggregated(self):
        secret = "token=super-secret /srv/private/input.txt"
        with tempfile.TemporaryDirectory() as td:
            report = verify_bundle(
                self._make_bundle(
                    Path(td),
                    result_updates={
                        "local_acceptance_eligible": False,
                        "blockers": [],
                        "acceptance_blockers": [],
                        "acceptance_eligibility": {"eligible": False, "blockers": []},
                        "protected_input_blockers": [secret],
                    },
                    manifest_updates={"protected_input_blockers": ["/etc/shadow was captured"]},
                )
            )
            self.assertEqual(report["code"], CODE_VALID)
            self.assertTrue(report["integrity"])
            self.assertFalse(report["accepted"])
            self.assertEqual(
                report["errors"],
                [
                    "NGELC-ACCEPTANCE-INELIGIBLE: bundle is intact but not acceptance-eligible",
                    "NGELC-PROTECTED-INPUT-BLOCKER: protected input blocker present (result.protected_input_blockers)",
                    "NGELC-PROTECTED-INPUT-BLOCKER: protected input blocker present (manifest.protected_input_blockers)",
                ],
            )
            self.assertNotIn(secret, "\n".join(report["errors"]))
            self.assertNotIn("/etc/shadow", "\n".join(report["errors"]))

    def test_hostile_non_string_control_and_long_blockers_do_not_leak(self):
        hostile = "secret=abc /private/path\x00\n\r\t" + ("x" * 10_000)
        with tempfile.TemporaryDirectory() as td:
            report = verify_bundle(
                self._make_bundle(
                    Path(td),
                    result_updates={
                        "local_acceptance_eligible": False,
                        "blockers": [hostile, 42, None, {"secret": hostile}],
                        "acceptance_blockers": [hostile],
                        "acceptance_eligibility": {"eligible": False, "blockers": [False, [hostile]]},
                        "protected_input_blockers": [hostile, {"path": "/private/path"}],
                    },
                    manifest_updates={"protected_input_blockers": [hostile, 7]},
                )
            )
            self.assertEqual(report["code"], CODE_VALID)
            self.assertTrue(report["integrity"])
            self.assertFalse(report["accepted"])
            self.assertTrue(report["errors"])
            self.assertTrue(all(isinstance(error, str) for error in report["errors"]))
            self.assertTrue(all(len(error) <= 200 for error in report["errors"]))
            self.assertTrue(all(not any(ord(char) < 32 for char in error) for error in report["errors"]))
            rendered = "\n".join(report["errors"])
            self.assertNotIn("secret=abc", rendered)
            self.assertNotIn("/private/path", rendered)
            self.assertNotIn(hostile, rendered)

    def test_duplicate_blockers_produce_deterministic_deduplicated_reasons(self):
        updates = {
            "local_acceptance_eligible": False,
            "blockers": ["duplicate", "duplicate"],
            "acceptance_blockers": ["duplicate", "duplicate"],
            "acceptance_eligibility": {"eligible": False, "blockers": ["duplicate", "duplicate"]},
            "protected_input_blockers": ["duplicate", "duplicate"],
        }
        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td), result_updates=updates, manifest_updates={"protected_input_blockers": ["duplicate", "duplicate"]})
            first = verify_bundle(bundle)
            second = verify_bundle(bundle)
            self.assertEqual(first["errors"], second["errors"])
            self.assertEqual(first["errors"], list(dict.fromkeys(first["errors"])))

    def test_consumer_distinguishes_intact_nonaccepted_bundle_from_tampering(self):
        def classify(report):
            if report["code"] == CODE_VALID and report["integrity"] and not report["accepted"]:
                return "valid-nonaccepted"
            if report["code"] == CODE_TAMPERED and not report["integrity"]:
                return "tampered"
            return "other"

        with tempfile.TemporaryDirectory() as valid_td, tempfile.TemporaryDirectory() as tampered_td:
            valid = verify_bundle(self._make_bundle(Path(valid_td), accepted=False))
            tampered_bundle = self._make_bundle(Path(tampered_td))
            (tampered_bundle / "artifacts/source/photo.jpg").write_bytes(b"changed")
            tampered = verify_bundle(tampered_bundle)
            self.assertEqual(classify(valid), "valid-nonaccepted")
            self.assertEqual(classify(tampered), "tampered")

    def test_corrupted_manifest_reports_stable_integrity_failure_reason(self):
        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td))
            manifest_path = bundle / "manifest.json"
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            for entry in manifest["files"]:
                if entry["path"] == "artifacts/source/photo.jpg":
                    entry["sha256"] = "0" * 64
                    break
            self._write_json(manifest_path, manifest)
            self._write_json(
                bundle / "terminal-seal.json",
                {"schema_version": 2, "manifest_sha256": hashlib.sha256(manifest_path.read_bytes()).hexdigest()},
            )
            report = verify_bundle(bundle)
            self.assertEqual(report["code"], CODE_TAMPERED)
            self.assertFalse(report["integrity"])
            self.assertFalse(report["accepted"])
            self.assertEqual(report["errors"], ["sha256 mismatch: artifacts/source/photo.jpg"])

    def test_corrupted_terminal_seal_reports_stable_integrity_failure_reason(self):
        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td))
            seal_path = bundle / "terminal-seal.json"
            seal = json.loads(seal_path.read_text(encoding="utf-8"))
            seal["manifest_sha256"] = "0" * 64
            self._write_json(seal_path, seal)
            report = verify_bundle(bundle)
            self.assertEqual(report["code"], CODE_TAMPERED)
            self.assertFalse(report["integrity"])
            self.assertFalse(report["accepted"])
            self.assertEqual(report["errors"], ["terminal seal does not match manifest.json"])

    def test_unlisted_file_and_symlink_fail_exact_coverage(self):
        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td))
            (bundle / "unexpected.txt").write_text("not covered", encoding="utf-8")
            report = verify_bundle(bundle)
            self.assertEqual(report["code"], CODE_TAMPERED)

        with tempfile.TemporaryDirectory() as td:
            bundle = self._make_bundle(Path(td))
            (bundle / "unsafe-link").symlink_to(bundle / "result.json")
            report = verify_bundle(bundle)
            self.assertEqual(report["code"], CODE_TAMPERED)


if __name__ == "__main__":
    unittest.main()
