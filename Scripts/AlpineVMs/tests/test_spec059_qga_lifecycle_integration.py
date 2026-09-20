import json
import os
import subprocess
import sys
import tempfile
from pathlib import Path
from unittest.mock import Mock

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import ng_remote_executor as executor
from proxmox_qga_channel import QGAChannel


@pytest.mark.parametrize(
    "loss_kind",
    ["partial-output", "no-output"],
)
def test_qga_exec_loss_enters_controller_reconciliation_and_finalization(loss_kind):
    """Exercise the real QGAChannel.exec loss result at the local controller seam."""
    with tempfile.TemporaryDirectory(prefix="ng-qga-integration-", dir=Path.home()) as raw_root:
        root = Path(raw_root)
        build = root / "build"
        io = root / "io"
        evidence = root / "evidence"
        for path in (build, io / "Source1", io / "Repository1"):
            path.mkdir(parents=True)

        child_code = "import time; time.sleep(30)"
        leader_code = """\
import signal, subprocess, sys, time
child = subprocess.Popen([sys.executable, '-c', sys.argv[1]])
print('READY', flush=True)
def stop(_signum, _frame):
    if child.poll() is None:
        child.terminate()
    try:
        child.wait(timeout=5)
    except subprocess.TimeoutExpired:
        child.kill()
        child.wait(timeout=5)
    raise SystemExit(0)
signal.signal(signal.SIGTERM, stop)
time.sleep(30)
"""
        role_name = "NG-like-leader"
        plan = {
            "schema_version": 1,
            "roles": [
                {
                    "name": role_name,
                    "vm": "local",
                    "command": [sys.executable, "-c", leader_code, child_code],
                    "cwd": str(build),
                    "readiness": [{"id": "ready", "pattern": "READY"}],
                }
            ],
            "timeouts": {"readiness": 2, "observation": 1, "total": 5},
            "runtime_oracle": {
                "type": "markers",
                "required": [{"role": role_name, "id": "runtime", "pattern": "READY"}],
            },
        }
        plan_path = root / "plan.json"
        plan_path.write_text(json.dumps(plan), encoding="utf-8")
        config = {
            "NG_LOCAL_REPO_PATH": str(root),
            "NG_LOCAL_BUILD_PATH": str(build),
            "NG_LOCAL_IO_PATH": str(io),
            "NG_LOCAL_EVIDENCE_PATH": str(evidence),
        }
        trial_id = f"qga-integration-{loss_kind}"
        args = executor.argparse.Namespace(
            plan=str(plan_path),
            scenario="local-intra-os",
            debug_profile="obs-normal",
            trial=trial_id,
            env=config,
            local_profile=None,
        )
        raw_transport = (
            {
                "returncode": 29,
                "stdout": "partial controller output\\n",
                "stderr": "Agent error: transport closed\\n",
            }
            if loss_kind == "partial-output"
            else {
                "returncode": None,
                "stdout": "",
                "stderr": "",
                "exception": "TimeoutExpired",
                "timeout_seconds": 3,
            }
        )
        runner = Mock()
        if loss_kind == "partial-output":
            runner.return_value = raw_transport
        else:
            runner.side_effect = subprocess.TimeoutExpired(["ssh"], timeout=3)
        channel = QGAChannel(host="192.168.0.200", vmid="100", timeout=3, run=runner)
        observe_calls = 0

        def qga_backed_observe(*_args, **_kwargs):
            nonlocal observe_calls
            observe_calls += 1
            if observe_calls == 1:
                return {"matches": [{"role": role_name, "stream": "stdout", "id": "ready", "line": "READY"}]}
            return channel.exec(["/bin/true"])

        def synthetic_provenance(**_kwargs):
            return {
                "git_clean": True,
                "build_linkage": True,
                "controller_identity": True,
                "plan_snapshot": True,
                "source_snapshot_complete": True,
            }

        def synthetic_executable(_provenance, _role, argv0):
            return {"proc_path": argv0, "fd": os.open(os.devnull, os.O_RDONLY), "fixture": True}

        def synthetic_preflight(*_args):
            return {"ok": True, "roles": [{"name": role_name, "resolved_path": sys.executable}]}

        with pytest.MonkeyPatch.context() as patch:
            patch.setattr(executor, "load_plan_for_trial", lambda *_args, **_kwargs: (plan, {"level": "fixture"}))
            patch.setattr(executor, "local_provenance", synthetic_provenance)
            patch.setattr(executor, "prepare_local_executable", synthetic_executable)
            patch.setattr(executor, "local_preflight", synthetic_preflight)
            patch.setattr(executor, "local_observe", qga_backed_observe)
            patch.setattr(
                executor,
                "local_ipc_snapshot",
                lambda **_kwargs: {"shm": set(), "semaphores": set(), "queues": set(), "posix_semaphores": set()},
            )
            patch.setattr(
                executor,
                "local_remove_new_ipc",
                lambda *_args, **_kwargs: (True, {"shm": [], "semaphores": [], "queues": [], "posix_semaphores": []}, None),
            )
            result_code = executor.run_local_trial(args)

        trial = evidence / trial_id
        result = json.loads((trial / "result.json").read_text(encoding="utf-8"))
        assert result_code != 0
        assert result["runtime_result"] != "PASS"
        assert result["teardown_result"] == "PASS"
        assert result["exit_code"] != 0
        assert (trial / "result.json").is_file()
        assert (trial / "manifest.json").is_file()
        assert (trial / "terminal-seal.json").is_file()
        assert executor.verify_bundle(trial)["accepted"] is False

        channel_evidence = trial / "qga" / "transport-loss.json"
        assert channel_evidence.is_file()
        saved = json.loads(channel_evidence.read_text(encoding="utf-8"))
        assert saved["terminal_event"]["operation_id"] == trial_id
        assert saved["terminal_event"]["phase"] == "observation-teardown"
        assert saved["terminal_event"]["sequence"] == 1
        assert saved["terminal_event"]["guest_outcome"] == "unknown"
        assert saved["channel"]["guest_exit_code"] is None
        assert saved["channel"]["exit_code"] is None
        assert saved["channel"]["raw_transport"] == raw_transport
        assert json.loads((trial / "qga" / "transport-loss.raw.json").read_text(encoding="utf-8")) == raw_transport

        events = [json.loads(line) for line in (trial / "controller-events.jsonl").read_text(encoding="utf-8").splitlines()]
        launch = next(item for item in events if item["event"] == "launch")
        assert executor.local_group_members_status(launch["pgid"])["members"] == []
        assert runner.call_count == 1
