import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from remote.ng_trial_helper import capture_identity, identity_matches, launch, observe, prepare, stop_role


class RemoteIdentityTests(unittest.TestCase):
    def test_identity_matches_live_process(self):
        proc = subprocess.Popen(["sleep", "3"])
        try:
            record = capture_identity(proc.pid, "demo", "trial-1", ["sleep", "3"])
            self.assertEqual(record["pid"], proc.pid)
            self.assertTrue(identity_matches(record))
        finally:
            proc.terminate()
            proc.wait(timeout=2)

    def test_reused_or_stale_starttime_is_rejected(self):
        proc = subprocess.Popen(["sleep", "3"])
        try:
            record = capture_identity(proc.pid, "demo", "trial-1", ["sleep", "3"])
            record["starttime"] += 1
            self.assertFalse(identity_matches(record))
        finally:
            proc.terminate()
            proc.wait(timeout=2)

    def test_log_marker_split_across_reads_is_preserved(self):
        with tempfile.TemporaryDirectory() as td:
            state = Path(td) / "trial"
            prepare(state, "trial-logs", {})
            launch(state, "trial-logs", {"role": "synthetic", "argv": ["sleep", "3"]})
            log = state / "logs" / "synthetic" / "stdout.log"
            log.write_bytes(b"REA")
            first = observe(state, "synthetic", {"patterns": [{"id": "ready", "pattern": "READY"}], "offsets": {}, "carries": {}, "max_bytes": 32})
            self.assertEqual(first["matches"], [])
            log.write_bytes(b"READY\n")
            second = observe(state, "synthetic", {"patterns": [{"id": "ready", "pattern": "READY"}], "offsets": first["offsets"], "carries": first["carries"], "max_bytes": 32})
            self.assertEqual([item["id"] for item in second["matches"]], ["ready"])
            stop_role(state, "synthetic", 1)
            self.assertTrue((state.parent / ".ng-trial.lock").exists())

    def test_log_overflow_is_reported_without_advancing_offset(self):
        with tempfile.TemporaryDirectory() as td:
            state = Path(td) / "trial"
            prepare(state, "trial-overflow", {})
            launch(state, "trial-overflow", {"role": "synthetic", "argv": ["sleep", "3"]})
            log = state / "logs" / "synthetic" / "stdout.log"
            log.write_bytes(b"x" * 100)
            result = observe(state, "synthetic", {"patterns": [], "offsets": {}, "carries": {}, "max_bytes": 16})
            self.assertTrue(result["overflow"])
            self.assertEqual(result["offsets"]["stdout.log"], 0)
            stop_role(state, "synthetic", 1)


if __name__ == "__main__":
    unittest.main()
