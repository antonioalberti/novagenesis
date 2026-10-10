from __future__ import annotations

import json
import shlex
import subprocess
import sys
from pathlib import Path
from types import SimpleNamespace

import pytest

ALPINEVM_DIR = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ALPINEVM_DIR))

from ng_elc_guest_adapter import (  # noqa: E402
    AdapterError,
    QGAChannelConfig,
    ReadOnlyChannelAdapter,
    SSHChannelConfig,
)


def _config(tmp_path: Path, **overrides) -> SSHChannelConfig:
    identity = tmp_path / "identity"
    known_hosts = tmp_path / "known_hosts"
    identity.write_text("test identity placeholder", encoding="utf-8")
    identity.chmod(0o600)
    known_hosts.write_text("test host key placeholder", encoding="utf-8")
    values = {
        "host": "192.0.2.10",
        "user": "root",
        "identity_file": identity,
        "known_hosts_file": known_hosts,
        "expected_hostname": "alpine-test",
        "expected_uid": 0,
        "timeout_s": 7.0,
    }
    values.update(overrides)
    return SSHChannelConfig(**values)


def test_ssh_probe_uses_strict_key_auth_and_verifies_guest_identity(tmp_path: Path):
    config = _config(tmp_path)
    calls = []

    def runner(argv, **kwargs):
        calls.append((argv, kwargs))
        return SimpleNamespace(
            returncode=0,
            stdout=(
                "NGELC_HOSTNAME=alpine-test\nNGELC_HOSTNAME_EXIT=0\n"
                "NGELC_UID=0\nNGELC_UID_EXIT=0\n"
            ),
            stderr="",
        )

    result = ReadOnlyChannelAdapter(runner=runner).probe_ssh(config)

    assert result["status"] == "READY"
    assert result["readiness_scope"] == "channel"
    assert result["channel"] == "ssh"
    assert result["hostname"] == "alpine-test"
    assert result["uid"] == 0
    assert result["guest_exit_codes"] == {"hostname": 0, "uid": 0}
    assert result["transport_exit_code"] == 0
    assert len(calls) == 1
    argv, kwargs = calls[0]
    assert argv[0] == "ssh"
    assert "-i" in argv and str(config.identity_file) in argv
    assert "BatchMode=yes" in argv
    assert "PasswordAuthentication=no" in argv
    assert "IdentitiesOnly=yes" in argv
    assert "StrictHostKeyChecking=yes" in argv
    assert f"UserKnownHostsFile={config.known_hosts_file}" in argv
    assert f"{config.user}@{config.host}" in argv
    remote_argv = shlex.split(argv[-1])
    assert remote_argv[:2] == ["/bin/sh", "-c"]
    assert kwargs["timeout"] == config.timeout_s
    assert kwargs["capture_output"] is True
    assert kwargs["text"] is True
    assert kwargs["check"] is False


def test_ssh_probe_fails_closed_on_wrong_guest_identity(tmp_path: Path):
    config = _config(tmp_path)

    def runner(argv, **kwargs):
        return SimpleNamespace(
            returncode=0,
            stdout=(
                "NGELC_HOSTNAME=unexpected\nNGELC_HOSTNAME_EXIT=0\n"
                "NGELC_UID=0\nNGELC_UID_EXIT=0\n"
            ),
            stderr="",
        )

    with pytest.raises(AdapterError, match="hostname"):
        ReadOnlyChannelAdapter(runner=runner).probe_ssh(config)


def test_ssh_probe_fails_closed_on_host_key_rejection(tmp_path: Path):
    config = _config(tmp_path)

    def runner(argv, **kwargs):
        return SimpleNamespace(
            returncode=255,
            stdout="",
            stderr="Host key verification failed.",
        )

    with pytest.raises(AdapterError, match="SSH transport"):
        ReadOnlyChannelAdapter(runner=runner).probe_ssh(config)


def test_ssh_probe_fails_closed_when_remote_identity_command_fails(tmp_path: Path):
    config = _config(tmp_path)

    def runner(argv, **kwargs):
        return SimpleNamespace(
            returncode=0,
            stdout=(
                "NGELC_HOSTNAME=alpine-test\nNGELC_HOSTNAME_EXIT=0\n"
                "NGELC_UID=\nNGELC_UID_EXIT=127\n"
            ),
            stderr="",
        )

    with pytest.raises(AdapterError, match="uid.*command failed"):
        ReadOnlyChannelAdapter(runner=runner).probe_ssh(config)


def test_ssh_probe_fails_closed_on_external_timeout(tmp_path: Path):
    import subprocess

    config = _config(tmp_path)

    def runner(argv, **kwargs):
        raise subprocess.TimeoutExpired(argv, kwargs["timeout"], output="partial")

    with pytest.raises(AdapterError, match="timed out"):
        ReadOnlyChannelAdapter(runner=runner).probe_ssh(config)


def test_ssh_probe_rejects_a_group_or_world_readable_identity_file(tmp_path: Path):
    config = _config(tmp_path)
    config.identity_file.chmod(0o644)

    def runner(argv, **kwargs):
        raise AssertionError("SSH must not run with an insecure identity file")

    with pytest.raises(AdapterError, match="permissions"):
        ReadOnlyChannelAdapter(runner=runner).probe_ssh(config)


def test_qga_probe_preserves_transport_and_guest_exit_status_and_remote_argv(tmp_path: Path):
    proxmox = _config(tmp_path, host="192.0.2.20", expected_hostname=None, timeout_s=12.0)
    config = QGAChannelConfig(
        proxmox_ssh=proxmox,
        vmid=100,
        expected_hostname="agent178",
        expected_uid=0,
    )
    calls = []

    def runner(argv, **kwargs):
        calls.append((argv, kwargs))
        return SimpleNamespace(
            returncode=0,
            stdout=json.dumps({
                "exitcode": 0,
                "exited": 1,
                "out-data": (
                    "NGELC_HOSTNAME=agent178\nNGELC_HOSTNAME_EXIT=0\n"
                    "NGELC_UID=0\nNGELC_UID_EXIT=0\n"
                ),
                "err-data": "",
            }),
            stderr="",
        )

    result = ReadOnlyChannelAdapter(runner=runner).probe_qga(config)

    assert result["status"] == "READY"
    assert result["readiness_scope"] == "channel"
    assert result["channel"] == "qga"
    assert result["vmid"] == 100
    assert result["transport_exit_code"] == 0
    assert result["guest_exit_code"] == 0
    assert result["hostname"] == "agent178"
    assert result["uid"] == 0
    assert result["guest_exit_codes"] == {"hostname": 0, "uid": 0}
    argv, kwargs = calls[0]
    remote_argv = shlex.split(argv[-1])
    assert remote_argv[:8] == ["qm", "guest", "exec", "100", "--synchronous", "1", "--timeout", "0"]
    assert remote_argv[8:10] == ["--", "/usr/bin/sh"]
    assert kwargs["timeout"] == proxmox.timeout_s


def test_qga_probe_rejects_missing_guest_exit_code(tmp_path: Path):
    proxmox = _config(tmp_path, host="192.0.2.20", expected_hostname=None)
    config = QGAChannelConfig(proxmox_ssh=proxmox, vmid=100, expected_hostname="agent178")

    def runner(argv, **kwargs):
        return SimpleNamespace(
            returncode=0,
            stdout=json.dumps({"pid": 12, "exited": 0}),
            stderr="",
        )

    with pytest.raises(AdapterError, match="exitcode"):
        ReadOnlyChannelAdapter(runner=runner).probe_qga(config)


def test_qga_probe_rejects_guest_identity_mismatch(tmp_path: Path):
    proxmox = _config(tmp_path, host="192.0.2.20", expected_hostname=None)
    config = QGAChannelConfig(proxmox_ssh=proxmox, vmid=100, expected_hostname="agent178")

    def runner(argv, **kwargs):
        return SimpleNamespace(
            returncode=0,
            stdout=json.dumps({
                "exitcode": 0,
                "exited": 1,
                "out-data": (
                    "NGELC_HOSTNAME=wrong-vm\nNGELC_HOSTNAME_EXIT=0\n"
                    "NGELC_UID=0\nNGELC_UID_EXIT=0\n"
                ),
            }),
            stderr="",
        )

    with pytest.raises(AdapterError, match="hostname"):
        ReadOnlyChannelAdapter(runner=runner).probe_qga(config)


def test_cli_help_exposes_only_read_only_channel_probe_actions():
    result = subprocess.run(
        [sys.executable, str(ALPINEVM_DIR / "ng_elc_guest_adapter.py"), "--help"],
        capture_output=True,
        text=True,
        check=False,
    )

    assert result.returncode == 0
    assert "probe-ssh" in result.stdout
    assert "probe-qga" in result.stdout
    assert "read-only" in result.stdout.lower()
