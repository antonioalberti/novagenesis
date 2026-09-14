import json
import os
import tempfile
import unittest
from argparse import Namespace
from pathlib import Path
from unittest.mock import patch

import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import ng_remote_executor as executor


class ControllerLifecycleTests(unittest.TestCase):
    def test_run_trial_retries_transient_preflight_after_fresh_boot(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            plan_path = root / "plan.json"
            plan_path.write_text(json.dumps({
                "schema_version": 1,
                "roles": [
                    {"name": "PGCS-Source", "vm": "source", "command": ["fake", "source"], "readiness": [{"id": "ready-source", "pattern": "READY"}]},
                    {"name": "PGCS-Repository", "vm": "repository", "command": ["fake", "repo"], "readiness": [{"id": "ready-repo", "pattern": "READY"}]},
                ],
                "require_fresh_boot": True,
                "diagnostic_only": True,
                "timeouts": {"readiness": 2, "observation": 0.01, "total": 3},
            }))
            config = {
                "NG_SSH_KEY": str(root / "key"), "NG_SSH_USER": "root",
                "SOURCE_VM_IP": "source", "REPO_VM_IP": "repo",
                "SOURCE_VM_MAC": "08:00:27:65:00:08", "REPO_VM_MAC": "08:00:27:79:bb:15",
                "SOURCE_VM_IFACE": "eth0", "REPO_VM_IFACE": "eth0",
                "NG_REPO_PATH": "/opt/ng", "NG_BUILD_PATH": "/opt/ng/build-final", "NG_EVIDENCE_PATH": str(root / "evidence"),
                "NG_REMOTE_EVIDENCE_PATH": "/var/ng", "NG_SSH_KNOWN_HOSTS": str(root / "known_hosts"),
            }
            args = Namespace(plan=str(plan_path), scenario="L2", debug_profile="obs-normal", helper_local=str(root / "helper.py"), trial="fresh-boot-retry")
            (root / "helper.py").write_text("helper")
            (root / "ng_role_wrapper.py").write_text("wrapper")
            calls = []
            def fake_remote(_config, host, _helper, action, _state, _trial, payload=None):
                calls.append((host, action, payload or {}))
                if action == "observe":
                    return {"role": payload["role"], "matches": [{"id": payload["patterns"][0]["id"], "stream": "stdout.log", "line": "READY"}], "offsets": {"stdout.log": 5, "stderr.log": 0}, "identity_matches": True}
                if action == "stop":
                    return {"results": [{"role": payload["role"], "result": "STOPPED"}]}
                if action == "inventory":
                    return {"processes": [], "ipc": {"shm": {"available": True, "returncode": 0, "ids": []}, "semaphores": {"available": True, "returncode": 0, "ids": []}}, "ipc_new_ids": {"shm": [], "semaphores": []}}
                return {}
            preflights = iter([RuntimeError("guest preflight failed: Connection timed out"), {"ok": True}, {"ok": True}])
            def fake_preflight(*_args):
                result = next(preflights)
                if isinstance(result, Exception):
                    raise result
                return result
            with patch.object(executor, "load_config_from_env", return_value=config), patch.object(executor, "restart_vms", return_value={"restarted": ["101", "102"]}), patch.object(executor, "guest_preflight", side_effect=fake_preflight), patch.object(executor, "ensure_remote_dir"), patch.object(executor, "copy_helper"), patch.object(executor, "remote_call", side_effect=fake_remote), patch.object(executor, "collect_remote"):
                rc = executor.run_trial(args)
            self.assertEqual(rc, 11)
            self.assertEqual(sum(1 for host, action, _payload in calls if host == "source" and action == "launch"), 1)

    def test_run_trial_uses_ordered_fake_remote_lifecycle(self):
        with tempfile.TemporaryDirectory() as td:
            root = Path(td)
            plan_path = root / "plan.json"
            plan_path.write_text(json.dumps({
                "schema_version": 1,
                "roles": [
                    {"name": "PGCS-Source", "vm": "source", "command": ["fake", "source"], "readiness": [{"id": "ready-source", "pattern": "READY"}]},
                    {"name": "PGCS-Repository", "vm": "repository", "command": ["fake", "repo"], "readiness": [{"id": "ready-repo", "pattern": "READY"}]},
                ],
                "require_fresh_boot": False,
                "diagnostic_only": True,
                "timeouts": {"readiness": 2, "observation": 0.01, "total": 3},
            }))
            config = {
                "NG_SSH_KEY": str(root / "key"), "NG_SSH_USER": "root",
                "SOURCE_VM_IP": "source", "REPO_VM_IP": "repo",
                "SOURCE_VM_MAC": "08:00:27:65:00:08", "REPO_VM_MAC": "08:00:27:79:bb:15",
                "SOURCE_VM_IFACE": "eth0", "REPO_VM_IFACE": "eth0",
                "NG_REPO_PATH": "/opt/ng", "NG_BUILD_PATH": "/opt/ng/build-final", "NG_EVIDENCE_PATH": str(root / "evidence"),
                "NG_REMOTE_EVIDENCE_PATH": "/var/ng", "NG_SSH_KNOWN_HOSTS": str(root / "known_hosts"),
            }
            args = Namespace(plan=str(plan_path), scenario="L2", debug_profile="obs-normal", helper_local=str(root / "helper.py"), trial="fake-trial")
            (root / "helper.py").write_text("helper")
            (root / "ng_role_wrapper.py").write_text("wrapper")
            calls = []
            def fake_remote(_config, host, _helper, action, _state, _trial, payload=None):
                calls.append((host, action, payload or {}))
                if action == "observe":
                    return {"role": payload["role"], "matches": [{"id": payload["patterns"][0]["id"], "stream": "stdout.log", "line": "READY"}], "offsets": {"stdout.log": 5, "stderr.log": 0}, "identity_matches": True}
                if action == "stop":
                    return {"results": [{"role": payload["role"], "result": "STOPPED"}]}
                if action == "inventory":
                    return {"processes": [], "ipc": {"shm": {"available": True, "returncode": 0, "ids": []}, "semaphores": {"available": True, "returncode": 0, "ids": []}}, "ipc_new_ids": {"shm": [], "semaphores": []}}
                return {}
            def fake_copy(_config, _host, _local, _remote):
                return None
            def fake_collect(_config, _host, _state, local_dir):
                Path(local_dir).mkdir(parents=True, exist_ok=True)
                (Path(local_dir) / "collected.json").write_text("{}")
            with patch.object(executor, "load_config_from_env", return_value=config), patch.object(executor, "guest_preflight", return_value={"ok": True}), patch.object(executor, "ensure_remote_dir"), patch.object(executor, "copy_helper", side_effect=fake_copy), patch.object(executor, "remote_call", side_effect=fake_remote), patch.object(executor, "collect_remote", side_effect=fake_collect):
                rc = executor.run_trial(args)
            self.assertEqual(rc, 11)
            actions = [item[1] for item in calls]
            self.assertIn("prepare", actions)
            self.assertIn("start-monitor", actions)
            self.assertIn("renew", actions)
            self.assertEqual(actions.count("stop"), 2)
            self.assertIn("seal", actions)
            result = json.loads((root / "evidence" / "fake-trial" / "result.json").read_text())
            self.assertEqual(result["runtime_result"], "INCONCLUSIVE")
            self.assertEqual(result["teardown_result"], "PASS")


if __name__ == "__main__":
    unittest.main()
