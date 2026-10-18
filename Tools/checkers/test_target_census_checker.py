"""Synthetic File API graphs only; no configured-build proof or CMake calls."""
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

CHECKER = Path(__file__).with_name("target_census_checker.py")
TARGETS = {
    "default": ["Common", "ContentApp", "IoTTestApp", "NBTestApp", "PGCS", "NRNCS"],
    "diagnostic": ["Common", "ng039_gw_q0_q1_d2"],
}


class CensusTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="ng039-census-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.reply = self.root / ".cmake/api/v1/reply"
        self.reply.mkdir(parents=True)
        self.model_path = self.reply / "codemodel-v2.json"

    def write(self, path, value):
        path.write_text(json.dumps(value), encoding="utf-8")

    def graph(self, profile="default", names=None):
        refs = []
        for i, name in enumerate(TARGETS[profile] if names is None else names):
            ref = {"name": name, "id": f"{name}::{i}", "jsonFile": f"target-{i}.json"}
            refs.append(ref)
            self.write(self.reply / ref["jsonFile"], {"name": name, "id": ref["id"], "type": "STATIC_LIBRARY"})
        self.model = {"kind": "codemodel", "version": {"major": 2, "minor": 0},
                      "paths": {"build": str(self.root), "source": str(self.root / "source")},
                      "configurations": [{"name": "Debug", "targets": refs}]}
        self.write(self.model_path, self.model)

    def run_checker(self, profile="default", configuration="Debug", extra=()):
        return subprocess.run([sys.executable, str(CHECKER), profile, str(self.root),
                               str(self.model_path), "--configuration", configuration, *extra],
                              capture_output=True, text=True, env=os.environ.copy())

    def test_positive_both_profiles(self):
        for profile in TARGETS:
            with self.subTest(profile=profile):
                self.graph(profile)
                result = self.run_checker(profile)
                self.assertEqual(result.returncode, 0, result.stderr)
                data = json.loads(result.stdout)
                self.assertEqual(data["actual"], sorted(TARGETS[profile]))
                self.assertIn("not", data["limitations"])

    def rejected(self, **kwargs):
        result = self.run_checker(**kwargs)
        self.assertEqual(result.returncode, 2, result.stdout + result.stderr)
        self.assertFalse(result.stdout)

    def test_extra_unknown_and_known_targets(self):
        for profile in TARGETS:
            for extra in ["Unexpected", "GIRS", "HTS", "PSS",
                          "PGCS" if profile == "diagnostic" else "ng039_gw_q0_q1_d2"]:
                with self.subTest(profile=profile, extra=extra):
                    self.graph(profile, TARGETS[profile] + [extra])
                    self.rejected(profile=profile)

    def test_missing_targets(self):
        for profile in TARGETS:
            for missing in TARGETS[profile]:
                with self.subTest(profile=profile, missing=missing):
                    self.graph(profile, [n for n in TARGETS[profile] if n != missing])
                    self.rejected(profile=profile)

    def test_duplicate_names_ids_and_files(self):
        for field in ["name", "id", "jsonFile"]:
            with self.subTest(field=field):
                self.graph()
                refs = self.model["configurations"][0]["targets"]
                refs[1][field] = refs[0][field]
                self.write(self.model_path, self.model)
                self.rejected()

    def test_absent_and_malformed_inputs(self):
        for raw in [None, "{", "[]", "null", '{"kind":"codemodel","kind":"codemodel"}',
                    '{"value":NaN}']:
            with self.subTest(raw=raw):
                self.graph()
                if raw is None:
                    self.model_path.unlink()
                else:
                    self.model_path.write_text(raw)
                self.rejected()

    def test_invalid_model_fields(self):
        cases = [("kind", "index"), ("version", None), ("version", {"major": True, "minor": 0}),
                 ("version", {"major": 2}), ("paths", {}),
                 ("paths", {"build": str(self.root.parent), "source": "/source"}),
                 ("configurations", None), ("configurations", []),
                 ("configurations", [{}]), ("configurations", [{"name": "Debug"}]),
                 ("configurations", [{"name": "Debug", "targets": []}])]
        for field, value in cases:
            with self.subTest(field=field, value=value):
                self.graph()
                self.model[field] = value
                self.write(self.model_path, self.model)
                self.rejected()

    def test_configuration_selection(self):
        self.graph()
        self.rejected(configuration="Release")
        self.model["configurations"].append(self.model["configurations"][0].copy())
        self.write(self.model_path, self.model)
        self.rejected()

    def test_unnamed_configuration(self):
        self.graph()
        self.model["configurations"][0]["name"] = ""
        self.write(self.model_path, self.model)
        self.assertEqual(self.run_checker(configuration="").returncode, 0)

    def test_reference_traversal_and_absolute_paths(self):
        for ref in ["../target.json", "sub/../../target.json", "/target.json", "sub\\target.json"]:
            with self.subTest(ref=ref):
                self.graph()
                self.model["configurations"][0]["targets"][0]["jsonFile"] = ref
                self.write(self.model_path, self.model)
                self.rejected()

    def test_reference_symlink_escape(self):
        self.graph()
        target = self.reply / "target-0.json"
        outside = self.root / "outside.json"
        outside.write_bytes(target.read_bytes())
        target.unlink()
        target.symlink_to(outside)
        self.rejected()

    def test_codemodel_escape(self):
        self.graph()
        outside = self.root / "model.json"
        outside.write_bytes(self.model_path.read_bytes())
        self.model_path.unlink()
        self.model_path.symlink_to(outside)
        self.rejected()

    def test_target_missing_malformed_and_identity(self):
        for value in [None, [], {}, {"name": "Wrong", "id": "Common::0", "type": "STATIC_LIBRARY"},
                      {"name": "Common", "id": "Common::0", "type": "INVALID"}]:
            with self.subTest(value=value):
                self.graph()
                target = self.reply / "target-0.json"
                if value is None:
                    target.unlink()
                else:
                    self.write(target, value)
                self.rejected()

    def test_read_only_tree(self):
        self.graph()
        before = {str(p): p.read_bytes() for p in self.root.rglob("*") if p.is_file()}
        self.assertEqual(self.run_checker().returncode, 0)
        after = {str(p): p.read_bytes() for p in self.root.rglob("*") if p.is_file()}
        self.assertEqual(before, after)

    def test_unknown_profile_and_removed_manifest_option(self):
        self.graph()
        self.rejected(profile="unknown")
        self.rejected(extra=("--manifest", "missing.json"))


if __name__ == "__main__":
    unittest.main()
