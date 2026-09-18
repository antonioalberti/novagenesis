import json
import os
import shlex
import subprocess
import sys
from pathlib import Path
from unittest.mock import Mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import ng_observability
import ng_remote_executor as executor
from proxmox_qga_channel import (
    QGAChannel,
    QGAChannelError,
    build_qm_argv,
    parse_qga_exec_response,
    persist_channel_evidence,
)


def test_build_qm_argv_preserves_guest_argument_boundaries():
    assert build_qm_argv("100", ["/usr/bin/id", "-u"]) == [
        "qm", "guest", "exec", "100", "--synchronous", "1", "--timeout", "0", "--", "/usr/bin/id", "-u"
    ]


def test_build_qm_argv_requests_synchronous_unbounded_guest_wait():
    argv = build_qm_argv("100", ["/usr/bin/id", "-u"])
    assert argv == [
        "qm", "guest", "exec", "100", "--synchronous", "1", "--timeout", "0", "--", "/usr/bin/id", "-u"
    ]


def test_ssh_remote_shell_preserves_qm_argument_boundaries(tmp_path):
    qm = tmp_path / "qm"
    log = tmp_path / "argv.json"
    qm.write_text("#!/bin/sh\nprintf '%s\\n' \"$@\" > \"$QGA_ARG_LOG\"\n", encoding="utf-8")
    qm.chmod(0o755)
    channel = QGAChannel(host="192.168.0.200", vmid="100")
    qm_argv = build_qm_argv("100", ["/usr/bin/printf", "%s", "hello world"])
    ssh_argv = channel._ssh_argv(qm_argv)
    target_index = ssh_argv.index("root@192.168.0.200")
    remote_command = " ".join(ssh_argv[target_index + 1 :])
    env = {**os.environ, "PATH": f"{tmp_path}:{os.environ['PATH']}", "QGA_ARG_LOG": str(log)}
    subprocess.run(["/bin/sh", "-c", remote_command], env=env, check=True)
    assert log.read_text(encoding="utf-8").splitlines()[-3:] == ["/usr/bin/printf", "%s", "hello world"]


def test_build_qm_argv_rejects_shell_command_strings():
    try:
        build_qm_argv("100", "/usr/bin/id -u")
    except QGAChannelError as exc:
        assert "argument vector" in str(exc)
    else:
        raise AssertionError("shell command string was accepted")


def test_parse_qga_exec_response_uses_guest_exitcode_not_transport_fields():
    result = parse_qga_exec_response({
        "exitcode": 7,
        "exited": 1,
        "out-data": "partial output\n",
        "err-data": "failure\n",
    })
    assert result == {
        "exit_code": 7,
        "stdout": "partial output\n",
        "stderr": "failure\n",
        "exited": True,
    }


def test_channel_check_proves_qga_and_uid_zero():
    run = Mock(side_effect=[
        {"returncode": 0, "stdout": "", "stderr": ""},
        {"returncode": 0, "stdout": json.dumps({"exitcode": 0, "exited": 1, "out-data": "0\n"}), "stderr": ""},
    ])
    channel = QGAChannel(host="192.168.0.200", vmid="100", run=run)
    result = channel.check()
    assert result["channel_ready"] is True
    assert result["uid"] == 0
    assert result["backend"] == "proxmox-qga"
    assert run.call_count == 2


def test_channel_check_fails_closed_on_nonzero_guest_uid():
    run = Mock(side_effect=[
        {"returncode": 0, "stdout": "", "stderr": ""},
        {"returncode": 0, "stdout": json.dumps({"exitcode": 0, "exited": 1, "out-data": "1000\n"}), "stderr": ""},
    ])
    channel = QGAChannel(host="192.168.0.200", vmid="100", run=run)
    result = channel.check()
    assert result["channel_ready"] is False
    assert result["reason"] == "guest_uid_not_root"


def test_ng_elc_exposes_qga_channel_check():
    args = executor.build_parser().parse_args([
        "channel-check", "--channel", "proxmox-qga", "--host", "192.168.0.200", "--vmid", "100"
    ])
    assert args.operation == "channel-check"
    assert args.channel == "proxmox-qga"

    run = Mock(return_value={
        "returncode": 0,
        "stdout": json.dumps({"exitcode": 21, "exited": 1, "out-data": "", "err-data": "blocked\n"}),
        "stderr": "",
    })
    channel = QGAChannel(host="192.168.0.200", vmid="100", run=run)
    result = channel.exec(["/usr/bin/false"])
    assert result["transport_exit_code"] == 0
    assert result["exit_code"] == 21
    assert result["stderr"] == "blocked\n"


def test_channel_evidence_is_persisted_outside_tmp():
    from tempfile import TemporaryDirectory

    result = {
        "backend": "proxmox-qga",
        "host": "192.168.0.200",
        "vmid": "100",
        "channel_ready": True,
        "uid": 0,
        "probe": {
            "command": ["/usr/bin/id", "-u"],
            "transport_exit_code": 0,
            "guest_exit_code": 0,
            "stdout": "0\n",
            "stderr": "",
        },
    }
    with TemporaryDirectory(dir=Path.home()) as td:
        path = Path(td) / "channel.json"
        written = persist_channel_evidence(path, result)
        assert written == path
        saved = json.loads(path.read_text())
        assert saved["schema_version"] == 1
        assert saved["channel"] == result
        assert "password" not in path.read_text().lower()


def test_build_manifest_covers_contentapp_role_aliases():
    binaries = {
        "ContentApp": {"path": "/build/ContentApp", "size": 10, "sha256": "a" * 64},
        "PGCS": {"path": "/build/PGCS", "size": 20, "sha256": "b" * 64},
    }
    outputs = ng_observability.manifest_outputs_for_roles(binaries)
    assert outputs["Repository"] == outputs["ContentApp"]
    assert outputs["Source"] == outputs["ContentApp"]
    assert outputs["Repository"] is not outputs["ContentApp"]


def test_runtime_library_manifest_covers_contentapp_role_aliases():
    records = {"ContentApp": {"status": "ok", "binary_sha256": "a" * 64, "libraries": []}}
    identities = ng_observability.runtime_identity_for_roles(records)
    assert identities["Repository"] == identities["ContentApp"]
    assert identities["Source"] == identities["ContentApp"]
    assert identities["Repository"] is not identities["ContentApp"]


def test_local_stop_process_group_stops_owned_process(tmp_path):
    del tmp_path
    proc = subprocess.Popen(["/bin/sleep", "30"], start_new_session=True)
    try:
        pgid = os.getpgid(proc.pid)
        starttime = executor.process_starttime(proc.pid)
        result = executor.local_stop_process_group(proc, pgid, starttime)
        assert result["ok"] is True
        assert result["result"] == "SIGTERM"
        assert proc.poll() is not None
    finally:
        if proc.poll() is None:
            proc.kill()
            proc.wait(timeout=5)


def test_local_stop_process_group_records_already_exited_process():
    proc = subprocess.Popen(["/bin/sleep", "1"], start_new_session=True)
    pgid = os.getpgid(proc.pid)
    proc.wait(timeout=5)
    result = executor.local_stop_process_group(proc, pgid, None)
    assert result["ok"] is True
    assert result["result"] == "already-exited"


def test_local_stop_process_group_fails_closed_on_identity_mismatch():
    proc = subprocess.Popen(["/bin/sleep", "30"], start_new_session=True)
    try:
        pgid = os.getpgid(proc.pid)
        starttime = executor.process_starttime(proc.pid)
        result = executor.local_stop_process_group(proc, pgid, "wrong-starttime")
        assert result["ok"] is False
        assert result["result"] == "identity-mismatch"
        assert proc.poll() is None
    finally:
        if proc.poll() is None:
            proc.kill()
            proc.wait(timeout=5)
