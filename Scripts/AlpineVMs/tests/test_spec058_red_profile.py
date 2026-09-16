"""RED tests for SPEC-058 native privileged local profile.

These tests describe the new contract before implementation. They do not run
NovaGenesis roles or perform host cleanup; subprocess and identity calls are
mocked where side effects would otherwise occur.
"""
from __future__ import annotations

import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import ng_remote_executor as executor


class Spec058ProfileRedTests(unittest.TestCase):
    def test_native_profile_requires_matching_explicit_cli_and_root(self):
        plan = {"local_profile": "native-privileged"}
        result = executor.validate_local_profile(
            plan, cli_profile="native-privileged", mode="local", euid=0
        )
        self.assertEqual(result["name"], "native-privileged")
        self.assertEqual(result["euid"], 0)

    def test_native_profile_requires_explicit_cli_selection(self):
        with self.assertRaises(executor.ConfigError):
            executor.validate_local_profile(
                {"local_profile": "native-privileged"},
                cli_profile=None, mode="local", euid=0
            )

    def test_native_profile_rejects_non_root_before_side_effects(self):
        with self.assertRaises(executor.ConfigError):
            executor.validate_local_profile(
                {"local_profile": "native-privileged"},
                cli_profile="native-privileged", mode="local", euid=1000
            )

    def test_profile_conflict_and_remote_use_fail_closed(self):
        with self.assertRaises(executor.ConfigError):
            executor.validate_local_profile(
                {"local_profile": "native-privileged"},
                cli_profile="unprivileged", mode="local", euid=0
            )
        with self.assertRaises(executor.ConfigError):
            executor.validate_local_profile(
                {"local_profile": "native-privileged"},
                cli_profile="native-privileged", mode="remote", euid=0
            )

    def test_root_without_native_selection_does_not_select_native(self):
        result = executor.validate_local_profile(
            {}, cli_profile=None, mode="local", euid=0
        )
        self.assertEqual(result["name"], "unprivileged")


class Spec058CleanupRedTests(unittest.TestCase):
    def test_cleanup_uses_only_repository_script_and_records_zero_baseline(self):
        with tempfile.TemporaryDirectory(prefix="spec058-cleanup-", dir=Path.home()) as td:
            repo = Path(td)
            script = repo / "Scripts" / "Simple" / "clean.sh"
            script.parent.mkdir(parents=True)
            script.write_text("#!/bin/sh\nexit 0\n", encoding="utf-8")
            evidence = repo / "evidence"
            evidence.mkdir()
            snapshots = iter([
                {"processes": [], "ipc": {"shm": [], "semaphores": [], "queues": []}},
                {"processes": [], "ipc": {"shm": [], "semaphores": [], "queues": []}},
            ])
            completed = subprocess.CompletedProcess(
                ["bash", str(script)], 0, b"cleaned\n", b""
            )
            with patch.object(executor, "local_host_inventory", side_effect=lambda: next(snapshots)), \
                 patch.object(executor, "_run_bounded_command", return_value=completed) as run:
                result = executor.run_native_cleanup(repo, evidence, euid=0)

            self.assertTrue(result["ok"])
            self.assertEqual(run.call_count, 1)
            self.assertEqual(run.call_args.args[0][0], "bash")
            self.assertTrue(run.call_args.args[0][1].startswith("/proc/self/fd/"))
            self.assertTrue((evidence / "native-cleanup.json").is_file())

    def test_cleanup_failure_blocks_launch_and_never_reports_zero(self):
        with tempfile.TemporaryDirectory(prefix="spec058-cleanup-fail-", dir=Path.home()) as td:
            repo = Path(td)
            script = repo / "Scripts" / "Simple" / "clean.sh"
            script.parent.mkdir(parents=True)
            script.write_text("#!/bin/sh\nexit 1\n", encoding="utf-8")
            evidence = repo / "evidence"
            evidence.mkdir()
            snapshot = {"processes": [123], "ipc": {"shm": [7], "semaphores": [], "queues": []}}
            completed = subprocess.CompletedProcess(
                ["bash", str(script)], 1, "", "failure\n"
            )
            with patch.object(executor, "local_host_inventory", return_value=snapshot), \
                 patch.object(executor.subprocess, "run", return_value=completed) as run:
                result = executor.run_native_cleanup(repo, evidence, euid=0)

            self.assertFalse(result["ok"])
            self.assertNotEqual(result["baseline"], "ZERO")
            run.assert_not_called()

    def test_cleanup_uses_a_sanitized_privileged_environment(self):
        with tempfile.TemporaryDirectory(prefix="spec058-cleanup-env-", dir=Path.home()) as td:
            repo = Path(td)
            script = repo / "Scripts" / "Simple" / "clean.sh"
            script.parent.mkdir(parents=True)
            script.write_text("#!/bin/sh\nexit 0\n", encoding="utf-8")
            evidence = repo / "evidence"
            evidence.mkdir()
            snapshot = {"processes": [], "ipc": {"shm": [], "semaphores": [], "queues": []}}
            completed = subprocess.CompletedProcess(["bash", str(script)], 0, b"", b"")
            with patch.object(executor, "local_host_inventory", return_value=snapshot), \
                 patch.object(executor, "_run_bounded_command", return_value=completed) as run:
                executor.run_native_cleanup(repo, evidence, euid=0)
            env = run.call_args.kwargs["env"]
            self.assertNotIn("BASH_ENV", env)
            self.assertNotIn("LD_PRELOAD", env)
            self.assertEqual(env["PATH"], "/usr/sbin:/usr/bin:/sbin:/bin")
            self.assertEqual(env["NG_ELC_NATIVE"], "1")
            self.assertEqual(env["NG_CLEAN_BASE"], str(repo.resolve()))


class Spec058PtyRedTests(unittest.TestCase):
    def test_role_launch_declares_pty_and_separate_session(self):
        with tempfile.TemporaryDirectory(prefix="spec058-pty-", dir=Path.home()) as td:
            root = Path(td)
            stdout_path = root / "stdout.log"
            stderr_path = root / "stderr.log"
            with patch.object(executor.subprocess, "Popen") as popen:
                popen.return_value.pid = 4321
                result = executor.launch_local_role_pty(
                    "fixture", [sys.executable, "-c", "print('READY')"],
                    cwd=str(root), env=os.environ.copy(),
                    stdout_path=stdout_path, stderr_path=stderr_path,
                )
                argv = popen.call_args.args[0]
                kwargs = popen.call_args.kwargs
                self.assertEqual(argv[:3], ["/usr/bin/setsid", "--wait", "--ctty"])
                self.assertIs(kwargs["stdin"], kwargs["stdout"])
                self.assertIs(kwargs["stdout"], kwargs["stderr"])
                self.assertFalse(kwargs["start_new_session"])
                self.assertEqual(result["process"].pid, 4321)
                self.assertTrue(result["stream_topology"]["stdout_stderr_merged"])
                result["close"]()

    def test_real_role_has_a_controlling_terminal_and_bounded_capture(self):
        with tempfile.TemporaryDirectory(prefix="spec058-pty-real-test-", dir=Path.home()) as td:
            root = Path(td)
            stdout_path = root / "stdout.log"
            stderr_path = root / "stderr.log"
            code = "import os; print('TTY=%s FG=%s' % (os.isatty(1), os.tcgetpgrp(1) == os.getpgrp()), flush=True)"
            result = executor.launch_local_role_pty(
                "fixture", [sys.executable, "-c", code], cwd=str(root),
                env=os.environ.copy(), stdout_path=stdout_path,
                stderr_path=stderr_path, max_bytes=1024,
            )
            self.assertEqual(result["process"].wait(timeout=10), 0)
            result["close"]()
            self.assertIn("TTY=True FG=True", stdout_path.read_text(encoding="utf-8"))
            self.assertIsNone(getattr(result["stdout"], "error", None))

    def test_pty_capture_quota_is_fail_closed(self):
        with tempfile.TemporaryDirectory(prefix="spec058-pty-quota-", dir=Path.home()) as td:
            root = Path(td)
            stdout_path = root / "stdout.log"
            stderr_path = root / "stderr.log"
            code = "print('X' * 10000, flush=True)"
            result = executor.launch_local_role_pty(
                "fixture", [sys.executable, "-c", code], cwd=str(root),
                env=os.environ.copy(), stdout_path=stdout_path,
                stderr_path=stderr_path, max_bytes=128,
            )
            result["process"].wait(timeout=10)
            result["close"]()
            self.assertEqual(getattr(result["stdout"], "error", None), "PTY capture quota exceeded")
            self.assertLessEqual(stdout_path.stat().st_size, 128)


if __name__ == "__main__":
    unittest.main()
