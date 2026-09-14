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

    def _make_bundle(self, root: Path, *, accepted: bool = True) -> Path:
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

    def test_unaccepted_result_does_not_make_an_intact_bundle_untrusted(self):
        with tempfile.TemporaryDirectory() as td:
            report = verify_bundle(self._make_bundle(Path(td), accepted=False))
            self.assertEqual(report["code"], CODE_VALID)
            self.assertTrue(report["integrity"])
            self.assertFalse(report["accepted"])

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
