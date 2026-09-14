import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from ng_debug_compiler_launcher import command_for_source


class DebugLauncherTests(unittest.TestCase):
    def test_define_is_injected_only_for_exact_source_path(self):
        profile = {"defines": [{"file": "PGCS/src/PGRunPeriodic01.cpp", "macro": "DEBUG"}]}
        command = ["g++", "-c", "/repo/PGCS/src/PGRunPeriodic01.cpp", "-o", "x.o"]
        self.assertIn("-DDEBUG", command_for_source(command, profile, Path("/repo")))

    def test_unlisted_source_is_unchanged(self):
        profile = {"defines": [{"file": "PGCS/src/PGRunPeriodic01.cpp", "macro": "DEBUG"}]}
        command = ["g++", "-c", "/repo/PGCS/src/PGRunHello02.cpp", "-o", "x.o"]
        self.assertEqual(command_for_source(command, profile, Path("/repo")), command)


if __name__ == "__main__":
    unittest.main()
