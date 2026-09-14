import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from ng_observability import build_inventory, assess_lines, validate_profile


class ObservabilityTests(unittest.TestCase):
    def test_inventory_distinguishes_active_commented_and_guarded(self):
        with tempfile.TemporaryDirectory() as td:
            source = Path(td) / "Demo.cpp"
            source.write_text(
                "// #define DEBUG\n"
                "#define DEBUG_ACTIVE\n"
                "#ifdef DEBUG\n"
                "cerr << \"marker\" << endl;\n"
                "#endif\n",
                encoding="utf-8",
            )
            inv = build_inventory(Path(td))
            row = inv["files"][0]
            self.assertIn("DEBUG", {item["macro"] for item in row["commented_defines"]})
            self.assertIn("DEBUG_ACTIVE", {item["macro"] for item in row["active_defines"]})
            self.assertIn("DEBUG", {item["macro"] for item in row["guards"]})
            self.assertGreater(row["log_sink_count"], 0)

    def test_forbidden_broad_profile_is_rejected(self):
        profile = {"id": "bad", "defines": [{"file": "PGCS/src/PG.cpp", "macro": "DEBUG"}]}
        with self.assertRaises(ValueError):
            validate_profile(profile, {"files": []})

    def test_missing_marker_is_inconclusive(self):
        result = assess_lines(["ordinary line\n"], [{"id": "ready", "pattern": "READY"}])
        self.assertEqual(result["gates"][0]["result"], "INCONCLUSIVE")


if __name__ == "__main__":
    unittest.main()
