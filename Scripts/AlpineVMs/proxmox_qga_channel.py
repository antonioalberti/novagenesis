#!/usr/bin/env python3
"""Small, fail-closed adapter for Proxmox/QEMU Guest Agent execution."""
from __future__ import annotations

import argparse
import json
import os
import subprocess
import tempfile
from pathlib import Path
from typing import Any, Callable, Mapping, Sequence


class QGAChannelError(ValueError):
    """Raised when a QEMU Guest Agent request is unsafe or malformed."""


Runner = Callable[[list[str]], Mapping[str, Any]]


def _validate_token(value: str, name: str) -> str:
    if not isinstance(value, str) or not value or "\x00" in value:
        raise QGAChannelError(f"invalid {name}")
    return value


def build_qm_argv(vmid: str, command: Sequence[str] | str) -> list[str]:
    """Build a qm guest-exec vector without allowing shell command strings."""
    vmid = _validate_token(str(vmid), "vmid")
    if not vmid.isdigit():
        raise QGAChannelError("vmid must be numeric")
    if isinstance(command, str):
        raise QGAChannelError("command must be an argument vector")
    if not isinstance(command, Sequence) or not command:
        raise QGAChannelError("command must be a non-empty argument vector")
    argv = [str(item) for item in command]
    if any(not item or "\x00" in item for item in argv):
        raise QGAChannelError("command contains an empty or NUL argument")
    return ["qm", "guest", "exec", vmid, "--synchronous", "1", "--timeout", "0", "--", *argv]


def parse_qga_exec_response(payload: Mapping[str, Any] | str) -> dict[str, Any]:
    """Normalize the JSON returned by ``qm guest exec``."""
    if isinstance(payload, str):
        try:
            payload = json.loads(payload)
        except json.JSONDecodeError as exc:
            raise QGAChannelError("QGA response is not valid JSON") from exc
    if not isinstance(payload, Mapping):
        raise QGAChannelError("QGA response must be a JSON object")
    if "exitcode" not in payload:
        raise QGAChannelError("QGA response has no guest exitcode")
    try:
        exit_code = int(payload["exitcode"])
    except (TypeError, ValueError) as exc:
        raise QGAChannelError("QGA guest exitcode is invalid") from exc
    return {
        "exit_code": exit_code,
        "stdout": str(payload.get("out-data", "")),
        "stderr": str(payload.get("err-data", "")),
        "exited": bool(payload.get("exited", False)),
    }


def persist_channel_evidence(path: str | Path, result: Mapping[str, Any]) -> Path:
    """Atomically persist a channel result outside temporary storage."""
    destination = Path(path)
    if not destination.is_absolute():
        raise QGAChannelError("evidence path must be absolute")
    resolved = destination.resolve()
    if resolved == Path("/tmp") or Path("/tmp") in resolved.parents:
        raise QGAChannelError("evidence path must not be under /tmp")
    destination.parent.mkdir(parents=True, exist_ok=True)
    payload = {"schema_version": 1, "channel": dict(result)}
    fd, temporary = tempfile.mkstemp(
        prefix=f".{destination.name}.", suffix=".tmp", dir=destination.parent
    )
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as handle:
            json.dump(payload, handle, ensure_ascii=False, indent=2)
            handle.write("\n")
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary, destination)
    except Exception:
        try:
            os.unlink(temporary)
        except FileNotFoundError:
            pass
        raise
    return destination


class QGAChannel:
    """Run bounded commands in one explicitly selected Proxmox guest."""

    backend = "proxmox-qga"

    def __init__(
        self,
        *,
        host: str,
        vmid: str,
        ssh_user: str = "root",
        ssh_key: str | None = None,
        known_hosts: str | None = None,
        timeout: float = 30.0,
        run: Runner | None = None,
    ) -> None:
        self.host = _validate_token(host, "host")
        self.vmid = _validate_token(str(vmid), "vmid")
        if not self.vmid.isdigit():
            raise QGAChannelError("vmid must be numeric")
        self.ssh_user = _validate_token(ssh_user, "ssh_user")
        self.ssh_key = ssh_key
        self.known_hosts = known_hosts
        self.timeout = timeout
        self._run = run or self._subprocess_run

    def _ssh_argv(self, qm_argv: Sequence[str]) -> list[str]:
        target = f"{self.ssh_user}@{self.host}"
        argv = ["ssh", "-o", "BatchMode=yes", "-o", "ConnectTimeout=5"]
        if self.ssh_key:
            argv.extend(["-i", _validate_token(self.ssh_key, "ssh_key")])
        if self.known_hosts:
            argv.extend(["-o", "UserKnownHostsFile=" + _validate_token(self.known_hosts, "known_hosts")])
            argv.extend(["-o", "StrictHostKeyChecking=yes"])
        return [*argv, target, *qm_argv]

    def _subprocess_run(self, argv: list[str]) -> Mapping[str, Any]:
        completed = subprocess.run(
            argv,
            text=True,
            capture_output=True,
            timeout=self.timeout,
            check=False,
        )
        return {
            "returncode": completed.returncode,
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }

    def _call(self, qm_argv: Sequence[str]) -> Mapping[str, Any]:
        try:
            result = self._run(self._ssh_argv(qm_argv))
        except subprocess.TimeoutExpired as exc:
            raise QGAChannelError("QGA transport timeout") from exc
        if not isinstance(result, Mapping):
            raise QGAChannelError("QGA runner returned an invalid result")
        return result

    def check(self) -> dict[str, Any]:
        """Prove QGA availability and effective UID zero in the guest."""
        ping = self._call(["qm", "agent", self.vmid, "ping"])
        if int(ping.get("returncode", 1)) != 0:
            return {
                "backend": self.backend,
                "host": self.host,
                "vmid": self.vmid,
                "channel_ready": False,
                "reason": "qga_ping_failed",
            }
        probe = self._call(build_qm_argv(self.vmid, ["/usr/bin/id", "-u"]))
        if int(probe.get("returncode", 1)) != 0:
            return {
                "backend": self.backend,
                "host": self.host,
                "vmid": self.vmid,
                "channel_ready": False,
                "reason": "qga_exec_failed",
            }
        try:
            parsed = parse_qga_exec_response(str(probe.get("stdout", "")))
        except QGAChannelError:
            return {
                "backend": self.backend,
                "host": self.host,
                "vmid": self.vmid,
                "channel_ready": False,
                "reason": "qga_response_invalid",
            }
        if parsed["exit_code"] != 0:
            return {
                "backend": self.backend,
                "host": self.host,
                "vmid": self.vmid,
                "channel_ready": False,
                "reason": "guest_uid_probe_failed",
                "guest_exit_code": parsed["exit_code"],
            }
        try:
            uid = int(parsed["stdout"].strip())
        except ValueError:
            return {
                "backend": self.backend,
                "host": self.host,
                "vmid": self.vmid,
                "channel_ready": False,
                "reason": "guest_uid_invalid",
            }
        result = {
            "backend": self.backend,
            "host": self.host,
            "vmid": self.vmid,
            "channel_ready": uid == 0,
            "uid": uid,
            "probe": {
                "command": ["/usr/bin/id", "-u"],
                "transport_exit_code": int(probe.get("returncode", 1)),
                "guest_exit_code": parsed["exit_code"],
                "stdout": parsed["stdout"],
                "stderr": parsed["stderr"],
            },
        }
        if uid != 0:
            result["reason"] = "guest_uid_not_root"
        return result

    def exec(self, command: Sequence[str] | str) -> dict[str, Any]:
        """Execute one command and preserve guest and transport exit codes."""
        response = self._call(build_qm_argv(self.vmid, command))
        transport_exit_code = int(response.get("returncode", 1))
        if transport_exit_code != 0:
            return {
                "backend": self.backend,
                "host": self.host,
                "vmid": self.vmid,
                "transport_exit_code": transport_exit_code,
                "exit_code": None,
                "stdout": str(response.get("stdout", "")),
                "stderr": str(response.get("stderr", "")),
            }
        parsed = parse_qga_exec_response(str(response.get("stdout", "")))
        return {
            "backend": self.backend,
            "host": self.host,
            "vmid": self.vmid,
            "transport_exit_code": transport_exit_code,
            **parsed,
        }


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", required=True)
    parser.add_argument("--vmid", required=True)
    parser.add_argument("--ssh-user", default="root")
    parser.add_argument("--ssh-key")
    parser.add_argument("--known-hosts")
    sub = parser.add_subparsers(dest="operation", required=True)
    check = sub.add_parser("check")
    check.add_argument("--evidence")
    execute = sub.add_parser("exec")
    execute.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args(argv)
    channel = QGAChannel(
        host=args.host,
        vmid=args.vmid,
        ssh_user=args.ssh_user,
        ssh_key=args.ssh_key,
        known_hosts=args.known_hosts,
    )
    if args.operation == "check":
        result = channel.check()
    else:
        result = channel.exec(args.command)
    if getattr(args, "evidence", None):
        persist_channel_evidence(args.evidence, result)
        result = {**result, "evidence_path": str(Path(args.evidence).resolve())}
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 0 if result.get("channel_ready", result.get("exit_code") == 0) else 1


if __name__ == "__main__":
    raise SystemExit(main())
