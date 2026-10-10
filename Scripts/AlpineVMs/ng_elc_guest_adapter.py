#!/usr/bin/env python3
"""Read-only SSH/QGA channel probes for the AIOPT5 NG-ELC profile.

This module never launches NovaGenesis roles or changes guest/VM state. SSH is
used for Alpine guests; QGA is optional and is invoked through the configured
Proxmox SSH endpoint, not assumed on VM101/102.
"""
from __future__ import annotations

import argparse
import json
import math
import os
import re
import shlex
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable, Sequence


class AdapterError(RuntimeError):
    """A channel probe could not establish its bounded read-only contract."""


@dataclass(frozen=True)
class SSHChannelConfig:
    host: str
    user: str
    identity_file: Path
    known_hosts_file: Path
    expected_hostname: str | None = None
    expected_uid: int = 0
    timeout_s: float = 8.0


@dataclass(frozen=True)
class QGAChannelConfig:
    proxmox_ssh: SSHChannelConfig
    vmid: int
    expected_hostname: str
    expected_uid: int = 0


SSH_READ_ONLY_PROBE = (
    "hostname=$(hostname 2>/dev/null); hostname_rc=$?; "
    "uid=$(id -u 2>/dev/null); uid_rc=$?; "
    "printf 'NGELC_HOSTNAME=%s\\nNGELC_HOSTNAME_EXIT=%s\\n' \"$hostname\" \"$hostname_rc\"; "
    "printf 'NGELC_UID=%s\\nNGELC_UID_EXIT=%s\\n' \"$uid\" \"$uid_rc\""
)


def _safe_text(value: Any, limit: int = 8192) -> str:
    if value is None:
        return ""
    if isinstance(value, bytes):
        text = value.decode("utf-8", errors="replace")
    else:
        text = str(value)
    if len(text) > limit:
        return text[:limit] + "…[truncated]"
    return text


def _validate_ssh_config(config: SSHChannelConfig) -> None:
    if not isinstance(config, SSHChannelConfig):
        raise AdapterError("SSH config has the wrong type")
    if not isinstance(config.host, str) or not re.fullmatch(r"[A-Za-z0-9](?:[A-Za-z0-9.-]*[A-Za-z0-9])?", config.host):
        raise AdapterError("SSH host must be a literal hostname or IP without shell syntax")
    if not isinstance(config.user, str) or not re.fullmatch(r"[A-Za-z_][A-Za-z0-9._-]{0,31}", config.user):
        raise AdapterError("SSH user is invalid")
    if not isinstance(config.expected_uid, int) or isinstance(config.expected_uid, bool) or config.expected_uid < 0:
        raise AdapterError("expected_uid must be a non-negative integer")
    if config.expected_hostname is not None and (
        not isinstance(config.expected_hostname, str)
        or not re.fullmatch(r"[A-Za-z0-9](?:[A-Za-z0-9.-]*[A-Za-z0-9])?", config.expected_hostname)
    ):
        raise AdapterError("expected_hostname is invalid")
    if (
        not isinstance(config.timeout_s, (int, float))
        or isinstance(config.timeout_s, bool)
        or not math.isfinite(config.timeout_s)
        or config.timeout_s < 0.1
        or config.timeout_s > 120
    ):
        raise AdapterError("SSH timeout must be finite and between 0.1 and 120 seconds")
    for label, raw_path in (("identity", config.identity_file), ("known_hosts", config.known_hosts_file)):
        path = Path(raw_path)
        if not path.is_absolute() or path.is_symlink() or not path.is_file():
            raise AdapterError(f"SSH {label} file must be an existing absolute regular file")
        if label == "identity" and path.stat().st_mode & 0o077:
            raise AdapterError("SSH identity file permissions must exclude group/other access")


def _parse_probe_identity(output: str, channel: str) -> tuple[str, int, dict[str, int]]:
    required = {
        "NGELC_HOSTNAME",
        "NGELC_HOSTNAME_EXIT",
        "NGELC_UID",
        "NGELC_UID_EXIT",
    }
    facts: dict[str, str] = {}
    for line in output.splitlines():
        if "=" not in line:
            continue
        key, value = line.split("=", 1)
        if key in required:
            if key in facts:
                raise AdapterError(f"{channel} probe returned duplicate {key} facts")
            facts[key] = value
    missing = required - set(facts)
    if missing:
        raise AdapterError(f"{channel} probe returned incomplete guest identity facts: {sorted(missing)}")
    exit_codes: dict[str, int] = {}
    for operation, key in (("hostname", "NGELC_HOSTNAME_EXIT"), ("uid", "NGELC_UID_EXIT")):
        if not facts[key].isdecimal():
            raise AdapterError(f"{channel} probe returned invalid {operation} command status")
        exit_codes[operation] = int(facts[key])
        if exit_codes[operation] != 0:
            raise AdapterError(f"{channel} {operation} command failed (exitcode {exit_codes[operation]})")
    hostname = facts["NGELC_HOSTNAME"]
    uid_text = facts["NGELC_UID"]
    if not re.fullmatch(r"[A-Za-z0-9](?:[A-Za-z0-9.-]*[A-Za-z0-9])?", hostname):
        raise AdapterError(f"{channel} probe returned an invalid hostname")
    if not uid_text.isdecimal():
        raise AdapterError(f"{channel} probe returned an invalid UID")
    return hostname, int(uid_text), exit_codes


class ReadOnlyChannelAdapter:
    """Probe only fixed, read-only channel identity facts."""

    def __init__(self, runner: Callable[..., Any] = subprocess.run):
        self._runner = runner

    def probe_ssh(self, config: SSHChannelConfig) -> dict[str, Any]:
        _validate_ssh_config(config)
        remote_command = shlex.join(["/bin/sh", "-c", SSH_READ_ONLY_PROBE])
        argv = [
            "ssh",
            "-F", "/dev/null",
            "-i", str(config.identity_file),
            "-o", "IdentitiesOnly=yes",
            "-o", "IdentityAgent=none",
            "-o", "BatchMode=yes",
            "-o", "PasswordAuthentication=no",
            "-o", "PreferredAuthentications=publickey",
            "-o", "StrictHostKeyChecking=yes",
            "-o", f"UserKnownHostsFile={config.known_hosts_file}",
            "-o", "RequestTTY=no",
            "-o", "ControlMaster=no",
            "-o", "ClearAllForwardings=yes",
            "-o", f"ConnectTimeout={max(1, math.ceil(config.timeout_s))}",
            f"{config.user}@{config.host}",
            remote_command,
        ]
        started = time.monotonic()
        try:
            completed = self._runner(
                argv,
                capture_output=True,
                text=True,
                timeout=config.timeout_s,
                check=False,
            )
        except subprocess.TimeoutExpired as exc:
            raise AdapterError("SSH probe timed out at the external channel deadline") from exc
        except OSError as exc:
            raise AdapterError(f"SSH client could not be executed: {exc}") from exc
        elapsed_ms = round((time.monotonic() - started) * 1000, 3)
        stdout = _safe_text(completed.stdout)
        stderr = _safe_text(completed.stderr)
        if completed.returncode != 0:
            detail = stderr.strip() or "no SSH diagnostic"
            raise AdapterError(f"SSH transport failed (exit {completed.returncode}): {detail}")

        hostname, uid, guest_exit_codes = _parse_probe_identity(stdout, "SSH")
        if config.expected_hostname is not None and hostname != config.expected_hostname:
            raise AdapterError(
                f"SSH hostname identity mismatch: expected {config.expected_hostname!r}, got {hostname!r}"
            )
        if uid != config.expected_uid:
            raise AdapterError(f"SSH guest UID identity mismatch: expected {config.expected_uid}, got {uid}")
        return {
            "status": "READY",
            "readiness_scope": "channel",
            "channel": "ssh",
            "host": config.host,
            "user": config.user,
            "hostname": hostname,
            "uid": uid,
            "guest_exit_codes": guest_exit_codes,
            "transport_exit_code": completed.returncode,
            "elapsed_ms": elapsed_ms,
            "stdout": stdout,
            "stderr": stderr,
        }

    def probe_qga(self, config: QGAChannelConfig) -> dict[str, Any]:
        if not isinstance(config, QGAChannelConfig):
            raise AdapterError("QGA config has the wrong type")
        _validate_ssh_config(config.proxmox_ssh)
        if (
            not isinstance(config.vmid, int)
            or isinstance(config.vmid, bool)
            or config.vmid < 100
            or config.vmid > 999_999_999
        ):
            raise AdapterError("QGA VMID must be an integer in the Proxmox VMID range")
        if (
            not isinstance(config.expected_hostname, str)
            or not re.fullmatch(r"[A-Za-z0-9](?:[A-Za-z0-9.-]*[A-Za-z0-9])?", config.expected_hostname)
        ):
            raise AdapterError("QGA expected_hostname is invalid")
        if (
            not isinstance(config.expected_uid, int)
            or isinstance(config.expected_uid, bool)
            or config.expected_uid < 0
        ):
            raise AdapterError("QGA expected_uid must be a non-negative integer")

        qm_argv = [
            "qm", "guest", "exec", str(config.vmid),
            "--synchronous", "1", "--timeout", "0", "--",
            "/usr/bin/sh", "-c", SSH_READ_ONLY_PROBE,
        ]
        remote_command = shlex.join(qm_argv)
        transport = config.proxmox_ssh
        ssh_argv = [
            "ssh",
            "-F", "/dev/null",
            "-i", str(transport.identity_file),
            "-o", "IdentitiesOnly=yes",
            "-o", "IdentityAgent=none",
            "-o", "BatchMode=yes",
            "-o", "PasswordAuthentication=no",
            "-o", "PreferredAuthentications=publickey",
            "-o", "StrictHostKeyChecking=yes",
            "-o", f"UserKnownHostsFile={transport.known_hosts_file}",
            "-o", "RequestTTY=no",
            "-o", "ControlMaster=no",
            "-o", "ClearAllForwardings=yes",
            "-o", f"ConnectTimeout={max(1, math.ceil(transport.timeout_s))}",
            f"{transport.user}@{transport.host}",
            remote_command,
        ]
        started = time.monotonic()
        try:
            completed = self._runner(
                ssh_argv,
                capture_output=True,
                text=True,
                timeout=transport.timeout_s,
                check=False,
            )
        except subprocess.TimeoutExpired as exc:
            raise AdapterError("QGA probe timed out at the external SSH/channel deadline") from exc
        except OSError as exc:
            raise AdapterError(f"SSH client for QGA could not be executed: {exc}") from exc
        elapsed_ms = round((time.monotonic() - started) * 1000, 3)
        raw_stdout = _safe_text(completed.stdout)
        raw_stderr = _safe_text(completed.stderr)
        if completed.returncode != 0:
            detail = raw_stderr.strip() or "no SSH/QGA diagnostic"
            raise AdapterError(f"QGA transport failed (SSH exit {completed.returncode}): {detail}")
        try:
            guest_result = json.loads(raw_stdout)
        except json.JSONDecodeError as exc:
            raise AdapterError("QGA returned malformed JSON") from exc
        if not isinstance(guest_result, dict):
            raise AdapterError("QGA returned an unexpected response type")
        guest_exit = guest_result.get("exitcode")
        exited = guest_result.get("exited")
        if not isinstance(guest_exit, int) or isinstance(guest_exit, bool):
            raise AdapterError("QGA response is incomplete: missing guest exitcode")
        if exited not in (True, 1):
            raise AdapterError("QGA response is incomplete: guest command has not exited")
        guest_stdout = _safe_text(guest_result.get("out-data"))
        guest_stderr = _safe_text(guest_result.get("err-data"))
        if guest_exit != 0:
            raise AdapterError(f"QGA guest command failed (guest exitcode {guest_exit}): {guest_stderr}")
        hostname, uid, guest_exit_codes = _parse_probe_identity(guest_stdout, "QGA")
        if hostname != config.expected_hostname:
            raise AdapterError(
                f"QGA hostname identity mismatch: expected {config.expected_hostname!r}, got {hostname!r}"
            )
        if uid != config.expected_uid:
            raise AdapterError(f"QGA guest UID identity mismatch: expected {config.expected_uid}, got {uid}")
        return {
            "status": "READY",
            "readiness_scope": "channel",
            "channel": "qga",
            "proxmox_host": transport.host,
            "vmid": config.vmid,
            "hostname": hostname,
            "uid": uid,
            "guest_exit_codes": guest_exit_codes,
            "transport_exit_code": completed.returncode,
            "guest_exit_code": guest_exit,
            "elapsed_ms": elapsed_ms,
            "raw_response": raw_stdout,
            "transport_stderr": raw_stderr,
            "guest_stderr": guest_stderr,
        }


def _add_transport_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--host", required=True, help="literal SSH host or IP")
    parser.add_argument("--user", required=True, help="SSH account name")
    parser.add_argument("--identity-file", required=True, type=Path)
    parser.add_argument("--known-hosts-file", required=True, type=Path)
    parser.add_argument("--expected-uid", type=int, default=0)
    parser.add_argument("--timeout-s", type=float, default=8.0)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Run bounded read-only SSH/QGA channel readiness probes.")
    subparsers = parser.add_subparsers(dest="action", required=True)
    ssh_parser = subparsers.add_parser("probe-ssh", help="probe a host-key-verified SSH guest channel")
    _add_transport_arguments(ssh_parser)
    ssh_parser.add_argument("--expected-hostname", required=True)

    qga_parser = subparsers.add_parser("probe-qga", help="probe optional QGA through the Proxmox SSH channel")
    _add_transport_arguments(qga_parser)
    qga_parser.add_argument("--vmid", required=True, type=int)
    qga_parser.add_argument("--expected-hostname", required=True, help="expected hostname inside the QGA guest")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    adapter = ReadOnlyChannelAdapter()
    try:
        transport = SSHChannelConfig(
            host=args.host,
            user=args.user,
            identity_file=args.identity_file,
            known_hosts_file=args.known_hosts_file,
            expected_hostname=args.expected_hostname if args.action == "probe-ssh" else None,
            expected_uid=args.expected_uid,
            timeout_s=args.timeout_s,
        )
        if args.action == "probe-ssh":
            result = adapter.probe_ssh(transport)
        else:
            result = adapter.probe_qga(
                QGAChannelConfig(
                    proxmox_ssh=transport,
                    vmid=args.vmid,
                    expected_hostname=args.expected_hostname,
                    expected_uid=args.expected_uid,
                )
            )
        print(json.dumps(result, sort_keys=True))
        return 0
    except AdapterError as exc:
        print(json.dumps({"status": "BLOCKED", "reason": str(exc)}, sort_keys=True), file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
