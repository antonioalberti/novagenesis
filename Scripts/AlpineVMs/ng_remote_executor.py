#!/usr/bin/env python3
"""Fail-closed controller for staged NovaGenesis VM trials.

The controller uses short, non-PTY SSH calls and a per-trial helper on each
guest.  It deliberately does not invoke the older manual run_*.sh launchers.
"""
from __future__ import annotations

import argparse
import ctypes
import datetime as dt
import hashlib
import json
import math
import os
import pty
import pwd
import re
import shlex
import signal
import stat
import shutil
import subprocess
import sys
import termios
import threading
import time
import uuid
from pathlib import Path
from typing import Any, Mapping

try:
    import fcntl
except ImportError:  # pragma: no cover - non-Linux fail-closed path
    fcntl = None  # type: ignore[assignment]

from evidence_verifier import CODE_VALID, verify_bundle
from local_provenance import (
    capture_local_provenance as local_provenance,
    capture_git_state,
    capture_plan_snapshot as local_write_v2_plan,
    _current_index_blob as local_provenance_module_current_index_blob,
    _expand_value as _expand_value_for_secrets,
    collect_secret_values,
    file_identity,
    sanitize_config,
    sanitize_evidence_tree,
    validate_build_linkage,
)


class ConfigError(ValueError):
    pass


REQUIRED_ENV = (
    "NG_SSH_KEY", "NG_SSH_USER", "SOURCE_VM_IP", "REPO_VM_IP",
    "SOURCE_VM_MAC", "REPO_VM_MAC", "SOURCE_VM_IFACE", "REPO_VM_IFACE",
    "NG_REPO_PATH", "NG_BUILD_PATH", "NG_EVIDENCE_PATH", "NG_REMOTE_EVIDENCE_PATH",
    "NG_SSH_KNOWN_HOSTS",
)
MAC_RE = re.compile(r"^[0-9a-f]{2}(?::[0-9a-f]{2}){5}$")
IFACE_RE = re.compile(r"^[A-Za-z0-9_.-]{1,32}$")
PLACEHOLDER_RE = re.compile(r"<[^>]+>")
ROLE_RE = re.compile(r"^[A-Za-z0-9_.-]+$")
SCENARIOS = {"L0", "L1", "L2", "L3", "L4", "L5", "pgcs-only", "nrncs-only", "repository-control", "photos-100"}
SCENARIOS.add("local-intra-os")
OPTIONAL_ENV = ("NG_VM_HOST", "NG_VM_USER", "NG_VM_SSH_KEY", "NG_VM_KNOWN_HOSTS", "NG_REPO_VM_ID", "NG_SOURCE_VM_ID")
SCENARIO_FILE = {"L0": "L0", "L1": "L1", "L2": "L2", "L3": "L3", "L4": "L4", "L5": "L5", "pgcs-only": "L2", "nrncs-only": "L3", "repository-control": "L4", "photos-100": "L5", "local-intra-os": "LOCAL"}
LOCAL_REQUIRED_ENV = ("NG_LOCAL_REPO_PATH", "NG_LOCAL_BUILD_PATH", "NG_LOCAL_IO_PATH", "NG_LOCAL_EVIDENCE_PATH")
LOCAL_OPTIONAL_ENV = ("NG_LOCAL_BUILD_MANIFEST",)
_LOCAL_SECRET_VALUES: set[str] = set()


def utc_id() -> str:
    stamp = dt.datetime.now(dt.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    return f"{stamp}-{uuid.uuid4().hex[:12]}"


def load_config_from_env(env: Mapping[str, str] | None = None) -> dict[str, str]:
    values = dict(os.environ if env is None else env)
    missing = [key for key in REQUIRED_ENV if not values.get(key)]
    if missing:
        raise ConfigError("missing required configuration: " + ", ".join(missing))
    for key in REQUIRED_ENV:
        value = values[key]
        if "\0" in value or PLACEHOLDER_RE.search(value):
            raise ConfigError(f"invalid placeholder/control character in {key}")
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_.-]*", values["NG_SSH_USER"]):
        raise ConfigError("NG_SSH_USER contains unsafe characters")
    for key in ("NG_SSH_KEY", "NG_SSH_KNOWN_HOSTS", "NG_REPO_PATH", "NG_BUILD_PATH", "NG_EVIDENCE_PATH", "NG_REMOTE_EVIDENCE_PATH"):
        if not Path(values[key]).is_absolute():
            raise ConfigError(f"{key} must be an absolute path")
    if Path(values["NG_REMOTE_EVIDENCE_PATH"]).resolve() == Path("/tmp") or Path("/tmp") in Path(values["NG_REMOTE_EVIDENCE_PATH"]).resolve().parents:
        raise ConfigError("NG_REMOTE_EVIDENCE_PATH must not be under /tmp")
    for key in ("SOURCE_VM_MAC", "REPO_VM_MAC"):
        if not MAC_RE.fullmatch(values[key]):
            raise ConfigError(f"{key} must be lowercase colon-separated hexadecimal MAC")
    for key in ("SOURCE_VM_IFACE", "REPO_VM_IFACE"):
        if not IFACE_RE.fullmatch(values[key]):
            raise ConfigError(f"{key} contains unsafe interface characters")
    if values["SOURCE_VM_IP"] == values["REPO_VM_IP"]:
        raise ConfigError("source and repository SSH destinations must differ")
    optional = {key: values[key] for key in OPTIONAL_ENV if values.get(key)}
    for key in ("NG_VM_SSH_KEY", "NG_VM_KNOWN_HOSTS"):
        if key in optional and not Path(optional[key]).is_absolute():
            raise ConfigError(f"{key} must be an absolute path")
    for key in ("NG_REPO_VM_ID", "NG_SOURCE_VM_ID"):
        if key in optional and optional[key] not in {"101", "102"}:
            raise ConfigError(f"{key} must be 101 or 102")
    return {key: values[key] for key in REQUIRED_ENV} | optional


def load_local_config(env: Mapping[str, str] | None = None) -> dict[str, str]:
    values = dict(os.environ if env is None else env)
    missing = [key for key in LOCAL_REQUIRED_ENV if not values.get(key)]
    if missing:
        raise ConfigError("missing required local configuration: " + ", ".join(missing))
    for key in LOCAL_REQUIRED_ENV:
        value = values[key]
        if "\0" in value or PLACEHOLDER_RE.search(value):
            raise ConfigError(f"invalid placeholder/control character in {key}")
        if not Path(value).is_absolute():
            raise ConfigError(f"{key} must be an absolute path")
    if Path(values["NG_LOCAL_EVIDENCE_PATH"]).resolve() == Path("/tmp") or Path("/tmp") in Path(values["NG_LOCAL_EVIDENCE_PATH"]).resolve().parents:
        raise ConfigError("NG_LOCAL_EVIDENCE_PATH must not be under /tmp")
    optional = {}
    for key in LOCAL_OPTIONAL_ENV:
        if values.get(key):
            if "\0" in values[key] or PLACEHOLDER_RE.search(values[key]) or not Path(values[key]).is_absolute():
                raise ConfigError(f"{key} must be an absolute, placeholder-free path")
            optional[key] = values[key]
    return {key: values[key] for key in LOCAL_REQUIRED_ENV} | optional


def validate_local_profile(plan: Mapping[str, Any], cli_profile: str | None, *, mode: str, euid: int | None = None) -> dict[str, Any]:
    """Resolve the explicit local profile without implicit privilege changes."""
    declared = plan.get("local_profile", "unprivileged")
    if declared not in {"unprivileged", "native-privileged"}:
        raise ConfigError(f"unknown local profile: {declared}")
    if mode != "local" and (declared != "unprivileged" or cli_profile is not None):
        raise ConfigError("local profiles are not valid in remote mode")
    if cli_profile is not None and cli_profile not in {"unprivileged", "native-privileged"}:
        raise ConfigError(f"unknown local profile: {cli_profile}")
    if cli_profile is not None and cli_profile != declared:
        raise ConfigError(f"local profile conflict: plan={declared}, cli={cli_profile}")
    if declared == "native-privileged":
        if cli_profile != "native-privileged":
            raise ConfigError("native-privileged local profile requires explicit CLI selection")
        effective_uid = os.geteuid() if euid is None else int(euid)
        if effective_uid != 0:
            raise ConfigError("native-privileged local profile requires effective UID 0")
        return {"name": declared, "euid": effective_uid, "explicit": True, "plan_profile": declared, "cli_profile": cli_profile, "plan_explicit": True, "cli_explicit": True}
    return {"name": "unprivileged", "euid": os.geteuid() if euid is None else int(euid), "explicit": cli_profile is not None, "plan_profile": declared, "cli_profile": cli_profile, "plan_explicit": "local_profile" in plan, "cli_explicit": cli_profile is not None}


def local_host_inventory() -> dict[str, Any]:
    """Read a non-destructive host process/IPC inventory for native preflight."""
    processes: list[dict[str, Any]] = []
    for name in ("PGCS", "NRNCS", "ContentApp", "IoTTestApp", "NBTestApp"):
        try:
            result = subprocess.run(["/usr/bin/pgrep", "-x", name], capture_output=True, text=True, timeout=5, check=False, env=native_safe_environment())
        except (OSError, subprocess.SubprocessError):
            return {"processes": None, "ipc": {"shm": None, "semaphores": None}}
        if result.returncode == 0:
            for raw_pid in result.stdout.split():
                if raw_pid.isdigit():
                    processes.append({"pid": int(raw_pid), "name": name})
        elif result.returncode not in {1}:
            return {"processes": None, "ipc": {"shm": None, "semaphores": None}}
    ipc = local_ipc_snapshot(include_foreign=True)
    try:
        ipc["posix_semaphores"] = sorted(str(path) for path in Path("/dev/shm").glob("sem.*"))
    except OSError:
        ipc["posix_semaphores"] = None
    return {"processes": processes, "ipc": ipc}


def _inventory_is_zero(inventory: Mapping[str, Any]) -> bool:
    processes = inventory.get("processes")
    ipc = inventory.get("ipc")
    if not isinstance(processes, list) or not isinstance(ipc, Mapping):
        return False
    kinds = ("shm", "semaphores", "queues")
    if "posix_semaphores" in ipc:
        kinds = kinds + ("posix_semaphores",)
    return not processes and all(isinstance(ipc.get(kind), (set, list, tuple)) and not ipc.get(kind) for kind in kinds)


def native_safe_environment(extra: Mapping[str, str] | None = None) -> dict[str, str]:
    """Build a minimal privileged environment without loader/shell injection."""
    safe = {
        "PATH": "/usr/sbin:/usr/bin:/sbin:/bin",
        "LANG": "C",
        "LC_ALL": "C",
        "TERM": "xterm",
    }
    if extra:
        forbidden = {"BASH_ENV", "ENV", "LD_PRELOAD", "LD_LIBRARY_PATH", "PYTHONPATH", "PYTHONHOME", "GCONV_PATH", "IFS", "CDPATH"}
        for key, value in extra.items():
            if key in forbidden or key.startswith("LD_"):
                continue
            if "\0" not in key and "\0" not in value:
                safe[key] = value
    return safe


def _run_bounded_command(argv: list[str], *, env: Mapping[str, str], pass_fds: tuple[int, ...] = (), timeout: float = 60.0, output_limit: int = 65536) -> subprocess.CompletedProcess[bytes]:
    """Run an argv command with bounded stdout/stderr capture and timeout."""
    proc = subprocess.Popen(argv, env=dict(env), stdout=subprocess.PIPE, stderr=subprocess.PIPE, pass_fds=pass_fds, close_fds=True)
    buffers: dict[str, bytearray] = {"stdout": bytearray(), "stderr": bytearray()}
    overflow: set[str] = set()

    def drain(name: str, stream: Any) -> None:
        if stream is None:
            return
        try:
            while True:
                chunk = stream.read(65536)
                if not chunk:
                    return
                remaining = output_limit - len(buffers[name])
                if remaining <= 0:
                    overflow.add(name)
                    stream.close()
                    return
                buffers[name].extend(chunk[:remaining])
                if len(chunk) > remaining:
                    overflow.add(name)
                    stream.close()
                    return
        except (OSError, ValueError):
            return

    readers = [threading.Thread(target=drain, args=(name, getattr(proc, name)), daemon=True) for name in ("stdout", "stderr")]
    for reader in readers:
        reader.start()
    deadline = time.monotonic() + timeout
    while proc.poll() is None and time.monotonic() < deadline:
        time.sleep(0.01)
    timed_out = proc.poll() is None
    if timed_out:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait(timeout=5)
    for reader in readers:
        reader.join(timeout=2)
    if any(reader.is_alive() for reader in readers):
        for stream_name in ("stdout", "stderr"):
            stream = getattr(proc, stream_name, None)
            if stream is not None:
                try:
                    stream.close()
                except OSError:
                    pass
    returncode = proc.returncode if proc.returncode is not None else 124
    if timed_out:
        returncode = 124
    if overflow:
        returncode = 125
    return subprocess.CompletedProcess(argv, returncode, bytes(buffers["stdout"]), bytes(buffers["stderr"]))


def run_native_cleanup(repo_path: Path, evidence_dir: Path, *, euid: int | None = None) -> dict[str, Any]:
    """Run only the repository-owned cleanup and fail closed on ambiguity."""
    effective_uid = os.geteuid() if euid is None else int(euid)
    if effective_uid != 0:
        raise ConfigError("native cleanup requires effective UID 0")
    repo = Path(repo_path).resolve(strict=True)
    script_candidate = repo / "Scripts" / "Simple" / "clean.sh"
    try:
        script_fd = os.open(str(script_candidate), os.O_RDONLY | getattr(os, "O_CLOEXEC", 0) | getattr(os, "O_NOFOLLOW", 0))
        script = Path(os.readlink(f"/proc/self/fd/{script_fd}")).resolve(strict=True)
    except OSError as exc:
        raise ConfigError(f"repository-owned clean.sh cannot be opened safely: {exc}") from exc
    try:
        script.relative_to(repo)
    except ValueError as exc:
        os.close(script_fd)
        raise ConfigError("native cleanup script escapes repository") from exc
    if script.name != "clean.sh" or not script.is_file():
        os.close(script_fd)
        raise ConfigError("repository-owned clean.sh is missing")
    script_identity_before = file_identity(script)
    before = local_host_inventory()
    if not _inventory_is_zero(before):
        result = {
            "schema_version": 1,
            "effective_uid": effective_uid,
            "command": None,
            "script": script_identity_before,
            "returncode": None,
            "stdout": "",
            "stderr": "",
            "before": before,
            "after": before,
            "baseline": "NONZERO",
            "ok": False,
            "reason": "pre-clean inventory is non-empty or unavailable; refusing global cleanup",
        }
        local_json_write(Path(evidence_dir) / "native-cleanup.json", result)
        os.close(script_fd)
        return result
    try:
        command = ["bash", f"/proc/self/fd/{script_fd}"]
        completed = _run_bounded_command(command, env=native_safe_environment(), pass_fds=(script_fd,), timeout=60.0, output_limit=65536)
    except (OSError, subprocess.SubprocessError) as exc:
        command = ["bash", str(script)]
        completed = subprocess.CompletedProcess(command, 125, b"", str(exc).encode())
    finally:
        try:
            os.close(script_fd)
        except OSError:
            pass
    after = local_host_inventory()
    stdout = completed.stdout.decode("utf-8", errors="replace") if isinstance(completed.stdout, bytes) else str(completed.stdout or "")
    stderr = completed.stderr.decode("utf-8", errors="replace") if isinstance(completed.stderr, bytes) else str(completed.stderr or "")
    script_identity_after = file_identity(script)
    script_stable = script_identity_after == script_identity_before
    result = {
        "schema_version": 1,
        "effective_uid": effective_uid,
        "command": command,
        "script": script_identity_before,
        "script_identity_after": script_identity_after,
        "script_stable": script_stable,
        "returncode": completed.returncode,
        "stdout": stdout[-65536:],
        "stderr": stderr[-65536:],
        "before": before,
        "after": after,
        "baseline": "ZERO" if _inventory_is_zero(after) else "NONZERO",
        "ok": completed.returncode == 0 and script_stable and _inventory_is_zero(after),
    }
    local_json_write(Path(evidence_dir) / "native-cleanup.json", result)
    return result


class _PtyCapture:
    def __init__(self, master_fd: int, stdout_path: Path, max_bytes: int = 64 * 1024 * 1024):
        self.master_fd = master_fd
        self.stdout_path = stdout_path
        self.max_bytes = max_bytes
        self.bytes_captured = 0
        self.error: str | None = None
        self._closed = threading.Event()
        self._thread = threading.Thread(target=self._drain, name="ng-elc-pty-capture", daemon=True)
        self._thread.start()

    def _drain(self) -> None:
        try:
            self.stdout_path.parent.mkdir(parents=True, exist_ok=True)
            with self.stdout_path.open("wb") as stream:
                while True:
                    try:
                        chunk = os.read(self.master_fd, 65536)
                    except OSError as exc:
                        if getattr(exc, "errno", None) == 5:
                            break
                        self.error = str(exc)
                        break
                    if not chunk:
                        break
                    remaining = self.max_bytes - self.bytes_captured
                    if remaining <= 0:
                        self.error = "PTY capture quota exceeded"
                        break
                    stream.write(chunk[:remaining])
                    stream.flush()
                    self.bytes_captured += min(len(chunk), remaining)
                    if len(chunk) > remaining:
                        self.error = "PTY capture quota exceeded"
                        break
        except OSError as exc:
            self.error = str(exc)
        finally:
            self._closed.set()

    def close(self) -> None:
        self._thread.join(timeout=5)
        if self._thread.is_alive():
            try:
                os.close(self.master_fd)
            except OSError:
                pass
            self._thread.join(timeout=1)
            if self._thread.is_alive() and self.error is None:
                self.error = "PTY capture drain timeout"
        try:
            os.close(self.master_fd)
        except OSError:
            pass


def launch_local_role_pty(role: str, argv: list[str], *, cwd: str | None, env: Mapping[str, str], stdout_path: Path, stderr_path: Path, pass_fds: tuple[int, ...] = (), max_bytes: int = 64 * 1024 * 1024) -> dict[str, Any]:
    """Launch one role with a dedicated controlling PTY and bounded capture."""
    master_fd, slave_fd = pty.openpty()
    stderr = stderr_path.open("w", encoding="utf-8")
    capture: _PtyCapture | None = None
    proc: subprocess.Popen[Any] | None = None
    launch_argv = ["/usr/bin/setsid", "--wait", "--ctty", *argv]
    try:
        proc = subprocess.Popen(
            launch_argv,
            cwd=cwd,
            env=dict(env),
            stdin=slave_fd,
            stdout=slave_fd,
            stderr=slave_fd,
            start_new_session=False,
            close_fds=True,
            text=False,
            pass_fds=pass_fds,
        )
        capture = _PtyCapture(master_fd, stdout_path, max_bytes=max_bytes)
    except BaseException:
        if capture is not None:
            capture.close()
        if proc is not None:
            try:
                proc.terminate()
                proc.wait(timeout=5)
            except (OSError, subprocess.SubprocessError):
                pass
        stderr.close()
        try:
            os.close(master_fd)
        except OSError:
            pass
        raise
    finally:
        try:
            os.close(slave_fd)
        except OSError:
            pass
    assert proc is not None
    def close_all() -> None:
        capture.close()
        stderr.close()
    return {"role": role, "process": proc, "stdout": capture, "stderr": stderr, "pty": True, "stream_topology": {"stdout_stderr_merged": True, "authoritative": "stdout.log"}, "close": close_all}


def expand_argv(argv: list[str], variables: Mapping[str, str] | None = None) -> list[str]:
    vars_ = dict(os.environ if variables is None else variables)
    out = []
    for arg in argv:
        if not isinstance(arg, str) or "\0" in arg:
            raise ValueError("argv contains invalid argument")
        out.append(re.sub(r"\$\{([A-Za-z_][A-Za-z0-9_]*)\}", lambda m: vars_.get(m.group(1), m.group(0)), arg))
    return out


def validate_plan(plan: dict[str, Any], mode: str = "remote") -> None:
    if not isinstance(plan, dict) or plan.get("schema_version") != 1:
        raise ValueError("plan schema_version must be 1")
    roles = plan.get("roles")
    if not isinstance(roles, list):
        raise ValueError("plan roles must be an array")
    names: set[str] = set()
    for role in roles:
        if not isinstance(role, dict) or not isinstance(role.get("name"), str):
            raise ValueError("each role needs a name")
        name = role["name"]
        if name in names or not ROLE_RE.fullmatch(name):
            raise ValueError(f"invalid or duplicate role name: {name}")
        names.add(name)
        argv = role.get("command")
        if not isinstance(argv, list) or not argv or not all(isinstance(x, str) for x in argv):
            raise ValueError(f"role {name} command must be an argv array")
        if any("\0" in x for x in argv):
            raise ValueError(f"role {name} command contains NUL")
        if "shell" in role:
            raise ValueError("shell commands are not accepted; use argv arrays")
        allowed_vm = ("local",) if mode == "local" else ("source", "repository")
        if role.get("vm") not in allowed_vm:
            raise ValueError(f"role {name} vm must be one of {allowed_vm}")
        readiness = role.get("readiness", [])
        if not isinstance(readiness, list) or not all(isinstance(x, dict) and isinstance(x.get("id"), str) and isinstance(x.get("pattern"), str) for x in readiness):
            raise ValueError(f"role {name} readiness must contain id/pattern objects")
        if role.get("cwd") is not None and not isinstance(role["cwd"], str):
            raise ValueError(f"role {name} cwd must be a string")
    timeouts = plan.get("timeouts", {})
    if not isinstance(timeouts, dict):
        raise ValueError("timeouts must be an object")
    for key in ("readiness", "observation", "total"):
        value = float(timeouts.get(key, 0))
        if not math.isfinite(value) or value < 0 or (key == "readiness" and value <= 0):
            raise ValueError(f"timeout {key} must be finite and bounded")
    if float(timeouts.get("readiness", 0)) + float(timeouts.get("observation", 0)) > float(timeouts.get("total", 0)):
        raise ValueError("plan timeout budget is inconsistent")
    quota = float(plan.get("log_quota_bytes", 64 * 1024 * 1024))
    if not math.isfinite(quota) or quota <= 0 or quota > 512 * 1024 * 1024:
        raise ValueError("log_quota_bytes outside bounded range")
    if not isinstance(plan.get("diagnostic_only", False), bool):
        raise ValueError("diagnostic_only must be boolean")
    profile = plan.get("local_profile", "unprivileged")
    if mode == "local" and profile not in {"unprivileged", "native-privileged"}:
        raise ValueError("local_profile must be unprivileged or native-privileged")
    if mode != "local" and "local_profile" in plan:
        raise ValueError("local_profile is only valid in local mode")
    if mode == "local" and "workload" in plan:
        validate_workload_spec(plan["workload"])
    oracle = plan.get("runtime_oracle")
    if oracle is None and not plan.get("diagnostic_only", False):
        raise ValueError("non-diagnostic plan requires an explicit runtime_oracle")
    if oracle is not None and not isinstance(oracle, dict):
        raise ValueError("runtime_oracle must be an object")
    if oracle is not None and oracle.get("type") not in ({"markers", "files"} if mode == "local" else {"markers"}):
        raise ValueError("runtime_oracle type is not supported for this mode")
    if oracle is not None and oracle.get("type") == "files":
        for key in ("source", "repository", "pattern"):
            if not isinstance(oracle.get(key), str) or not oracle[key]:
                raise ValueError(f"file runtime_oracle requires {key}")
        if not isinstance(oracle.get("expected_count"), int) or oracle["expected_count"] < 0:
            raise ValueError("file runtime_oracle expected_count must be a non-negative integer")
        if oracle.get("hash") != "sha256":
            raise ValueError("file runtime_oracle must use sha256")
    if oracle is not None:
        required = oracle.get("required", [])
        if oracle.get("type") == "files":
            required = []
        if not isinstance(required, list) or (oracle.get("type") == "markers" and not required) or not all(isinstance(x, dict) and isinstance(x.get("role"), str) and isinstance(x.get("id"), str) and isinstance(x.get("pattern"), str) for x in required):
            raise ValueError("marker runtime_oracle requires non-empty role/id/pattern entries")
        role_names = {role["name"] for role in roles}
        if any(item["role"] not in role_names for item in required):
            raise ValueError("runtime_oracle references an unknown role")


def classify_result(runtime: str, teardown: str, evidence: str) -> int:
    if teardown != "PASS" or evidence != "COMPLETE":
        return 20
    if runtime == "PASS":
        return 0
    if runtime in {"INCONCLUSIVE", "NOT_RUN", "ABORTED"}:
        return 11
    return 10


def ssh_base(config: Mapping[str, str], host: str) -> list[str]:
    return [
        "ssh", "-T", "-o", "BatchMode=yes", "-o", "IdentitiesOnly=yes",
        "-o", "StrictHostKeyChecking=yes", "-o", f"UserKnownHostsFile={config['NG_SSH_KNOWN_HOSTS']}",
        "-o", "ConnectTimeout=10", "-o", "ConnectionAttempts=1", "-o", "ServerAliveInterval=5",
        "-o", "ServerAliveCountMax=3", "-o", "ForwardAgent=no", "-o", "ClearAllForwardings=yes",
        "-i", config["NG_SSH_KEY"], f"{config['NG_SSH_USER']}@{host}",
    ]


def scp_base(config: Mapping[str, str]) -> list[str]:
    return [
        "scp", "-q", "-o", "BatchMode=yes", "-o", "IdentitiesOnly=yes",
        "-o", "StrictHostKeyChecking=yes", "-o", f"UserKnownHostsFile={config['NG_SSH_KNOWN_HOSTS']}",
        "-o", "ConnectTimeout=10", "-o", "ConnectionAttempts=1", "-o", "ServerAliveInterval=5",
        "-o", "ServerAliveCountMax=3", "-o", "ForwardAgent=no", "-o", "ClearAllForwardings=yes",
        "-i", config["NG_SSH_KEY"],
    ]


def remote_shell(config: Mapping[str, str], host: str, argv: list[str], input_text: str = "") -> subprocess.CompletedProcess[str]:
    command = shlex.join(argv)
    return subprocess.run(ssh_base(config, host) + [command], input=input_text, text=True, capture_output=True, timeout=60, check=False)


def remote_call(config: Mapping[str, str], host: str, helper: str, action: str, state_dir: str, trial_id: str, payload: dict[str, Any] | None = None) -> dict[str, Any]:
    remote = ["python3", helper, action, "--state-dir", state_dir, "--trial-id", trial_id]
    run = remote_shell(config, host, remote, json.dumps(payload or {}))
    if run.returncode != 0:
        raise RuntimeError(f"remote {action} failed on {host}: {run.stderr.strip() or run.stdout.strip()}")
    try:
        result = json.loads(run.stdout.strip().splitlines()[-1])
    except (json.JSONDecodeError, IndexError) as exc:
        raise RuntimeError(f"invalid helper response from {host}: {run.stdout[-500:]}") from exc
    if not result.get("ok"):
        raise RuntimeError(f"helper rejected {action} on {host}: {result}")
    return result["result"]


def ensure_remote_dir(config: Mapping[str, str], host: str, path: str) -> None:
    run = remote_shell(config, host, ["mkdir", "-p", path])
    if run.returncode != 0:
        raise RuntimeError(f"cannot create remote trial directory on {host}: {run.stderr.strip()}")


def copy_helper(config: Mapping[str, str], host: str, local_helper: Path, remote_file: str) -> None:
    target = f"{config['NG_SSH_USER']}@{host}:{remote_file}"
    run = subprocess.run(scp_base(config) + [str(local_helper), target], capture_output=True, text=True, timeout=60, check=False)
    if run.returncode != 0:
        raise RuntimeError(f"cannot copy helper to {host}: {run.stderr.strip()}")
    local_hash = hashlib.sha256(local_helper.read_bytes()).hexdigest()
    remote = remote_shell(config, host, ["sha256sum", remote_file])
    if remote.returncode != 0 or remote.stdout.split()[0] != local_hash:
        raise RuntimeError(f"helper hash verification failed on {host}")


def vm_ssh_base(config: Mapping[str, str]) -> list[str]:
    required = ("NG_VM_HOST", "NG_VM_USER", "NG_VM_SSH_KEY", "NG_VM_KNOWN_HOSTS")
    missing = [key for key in required if not config.get(key)]
    if missing:
        raise ConfigError("fresh VM restart requires: " + ", ".join(missing))
    return [
        "ssh", "-T", "-o", "BatchMode=yes", "-o", "IdentitiesOnly=yes",
        "-o", "StrictHostKeyChecking=yes", "-o", f"UserKnownHostsFile={config['NG_VM_KNOWN_HOSTS']}",
        "-o", "ConnectTimeout=10", "-o", "ConnectionAttempts=1", "-o", "ServerAliveInterval=5",
        "-o", "ServerAliveCountMax=3", "-o", "ForwardAgent=no", "-o", "ClearAllForwardings=yes",
        "-i", config["NG_VM_SSH_KEY"], f"{config['NG_VM_USER']}@{config['NG_VM_HOST']}",
    ]


def vm_shell(config: Mapping[str, str], argv: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(vm_ssh_base(config) + [shlex.join(argv)], capture_output=True, text=True, timeout=120, check=False)


def restart_vms(config: Mapping[str, str]) -> dict[str, Any]:
    repo_id = config.get("NG_REPO_VM_ID", "101")
    source_id = config.get("NG_SOURCE_VM_ID", "102")
    ids = [repo_id, source_id]
    for vm_id in ids:
        current = vm_shell(config, ["qm", "status", vm_id])
        if current.returncode != 0:
            raise RuntimeError(f"qm status {vm_id} failed: {current.stderr.strip()}")
        if "status: running" in current.stdout:
            stop = vm_shell(config, ["qm", "stop", vm_id])
            if stop.returncode != 0:
                raise RuntimeError(f"qm stop {vm_id} failed: {stop.stderr.strip()}")
        elif "status: stopped" not in current.stdout:
            raise RuntimeError(f"VM {vm_id} has unknown state: {current.stdout.strip()}")
    for vm_id in ids:
        start = vm_shell(config, ["qm", "start", vm_id])
        if start.returncode != 0:
            raise RuntimeError(f"qm start {vm_id} failed: {start.stderr.strip()}")
    statuses = []
    for vm_id in ids:
        status = vm_shell(config, ["qm", "status", vm_id])
        statuses.append({"vm_id": vm_id, "returncode": status.returncode, "stdout": status.stdout, "stderr": status.stderr})
        if status.returncode != 0 or "status: running" not in status.stdout:
            raise RuntimeError(f"VM {vm_id} did not reach running state")
    return {"restarted": ids, "statuses": statuses}


def guest_preflight(config: Mapping[str, str], host: str, roles: list[dict[str, Any]]) -> dict[str, Any]:
    iface = config["SOURCE_VM_IFACE"] if host == config["SOURCE_VM_IP"] else config["REPO_VM_IFACE"]
    mac = config["SOURCE_VM_MAC"] if host == config["SOURCE_VM_IP"] else config["REPO_VM_MAC"]
    request = {"repo": config["NG_REPO_PATH"], "evidence": config["NG_REMOTE_EVIDENCE_PATH"], "iface": iface, "mac": mac, "roles": roles}
    script = (
        "import json,os,shutil,subprocess,sys; "
        "q=json.loads(sys.argv[1]); errors=[]; "
        "ev=q['evidence']; "
        "errors += ['persistent evidence path missing or under /tmp'] if not (os.path.isdir(ev) and not os.path.realpath(ev).startswith('/tmp/') and os.path.realpath(ev)!='/tmp') else []; "
        "iface_path='/sys/class/net/'+q['iface']+'/address'; "
        "actual_mac=open(iface_path).read().strip().lower() if os.path.isfile(iface_path) else ''; "
        "errors += ['interface missing'] if not actual_mac else []; "
        "errors += ['interface MAC mismatch'] if actual_mac and actual_mac != q['mac'] else []; "
        "tools={x:bool(shutil.which(x)) for x in ['python3','sha256sum','setsid','ipcs']}; "
        "tools.update({'gdb':bool(shutil.which('gdb'))}); "
        "errors += [x+' missing' for x,v in tools.items() if not v and (x!='gdb' or any('gdb' in a for r in q['roles'] for a in r['argv']))]; "
        "stale=[]; "
        "names={'PGCS','NRNCS','ContentApp','NBTestApp','IoTTestApp'}; stale=[]; "
        "[stale.append({'pid':e,'cmdline':open('/proc/'+e+'/cmdline','rb').read().replace(b'\\0',b' ').decode('utf-8','replace')}) for e in os.listdir('/proc') if e != str(os.getpid()) and e.isdigit() and (open('/proc/'+e+'/comm').read().strip() in names or any(os.path.basename(x.decode('utf-8','replace')) in names for x in open('/proc/'+e+'/cmdline','rb').read().split(b'\\0') if x))]; "
        "errors += ['stale NovaGenesis processes present'] if stale else []; "
        "roles=[]; "
        "[roles.append({'name':r['name'],'binary':r['argv'][0],'binary_exists':os.path.isfile(r['argv'][0]),'binary_executable':os.access(r['argv'][0],os.X_OK),'cwd_exists':(r.get('cwd') is None or os.path.isdir(r['cwd']))}) or errors.extend(([r['name']+' binary missing'] if not os.path.isfile(r['argv'][0]) else [])+([r['name']+' binary not executable'] if not os.access(r['argv'][0],os.X_OK) else [])+([r['name']+' cwd missing'] if r.get('cwd') and not os.path.isdir(r['cwd']) else [])) for r in q['roles']]; "
        "git=subprocess.run(['git','-C',q['repo'],'rev-parse','HEAD'],capture_output=True,text=True); "
        "errors += ['guest repository is not readable'] if git.returncode != 0 else []; "
        "print(json.dumps({'ok':not errors,'errors':errors,'tools':tools,'stale_processes':stale,'roles':roles,'git_head':git.stdout.strip(),'boot_id':open('/proc/sys/kernel/random/boot_id').read().strip()}))"
    )
    run = remote_shell(config, host, ["python3", "-c", script, json.dumps(request)])
    if run.returncode != 0:
        raise RuntimeError(f"guest preflight failed on {host}: {run.stderr.strip()}")
    try:
        result = json.loads(run.stdout.strip().splitlines()[-1])
    except (json.JSONDecodeError, IndexError) as exc:
        raise RuntimeError(f"invalid guest preflight response from {host}") from exc
    if not result.get("ok"):
        raise RuntimeError(f"guest preflight rejected {host}: {result.get('errors')}")
    return result


def wait_for_guest_preflight(config: Mapping[str, str], host: str, roles: list[dict[str, Any]], deadline: float) -> dict[str, Any]:
    """Wait for a rebooted guest's SSH/preflight path without masking real failures."""
    transient_markers = (
        "Connection timed out",
        "Connection refused",
        "No route to host",
        "Connection reset",
    )
    while True:
        try:
            return guest_preflight(config, host, roles)
        except RuntimeError as exc:
            if not any(marker in str(exc) for marker in transient_markers):
                raise
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise RuntimeError(f"guest preflight deadline expired on {host}: {exc}") from exc
            time.sleep(min(2.0, remaining))


def load_plan(path: Path, mode: str = "remote") -> dict[str, Any]:
    plan = json.loads(path.read_text(encoding="utf-8"))
    validate_plan(plan, mode=mode)
    return plan


def canonical_scenario(scenario: str) -> str:
    try:
        return SCENARIO_FILE[scenario]
    except KeyError as exc:
        raise ValueError(f"unknown scenario: {scenario}") from exc


def validate_scenario_plan(plan: dict[str, Any], scenario: str, debug_profile: str, source_root: Path, mode: str = "remote") -> dict[str, Any]:
    validate_plan(plan, mode=mode)
    level = canonical_scenario(scenario)
    scenario_path = source_root / "Scripts" / "AlpineVMs" / "observability" / "scenarios" / f"{level}.json"
    if not scenario_path.is_file():
        raise ValueError(f"scenario contract missing: {scenario_path}")
    scenario_data = json.loads(scenario_path.read_text(encoding="utf-8"))
    expected = list(scenario_data.get("roles", []))
    actual = [role["name"] for role in plan["roles"]]
    if actual != expected:
        raise ValueError(f"plan roles do not exactly match {level}: expected {expected}, got {actual}")
    profile_path = source_root / "Scripts" / "AlpineVMs" / "observability" / "profiles" / f"{debug_profile}.json"
    if not profile_path.is_file():
        raise ValueError(f"debug profile missing: {profile_path}")
    from ng_observability import build_inventory, validate_profile
    validate_profile(json.loads(profile_path.read_text(encoding="utf-8")), build_inventory(source_root))
    return {"level": level, "scenario": scenario_data, "profile": json.loads(profile_path.read_text(encoding="utf-8")), "profile_path": str(profile_path)}


def load_plan_for_trial(path: Path, scenario: str, debug_profile: str, mode: str = "remote") -> tuple[dict[str, Any], dict[str, Any]]:
    plan = load_plan(path, mode=mode)
    source_root = Path(__file__).resolve().parents[2]
    return plan, validate_scenario_plan(plan, scenario, debug_profile, source_root, mode=mode)


def host_for(config: Mapping[str, str], role: Mapping[str, Any]) -> tuple[str, str]:
    vm = role["vm"]
    if vm == "source":
        return vm, config["SOURCE_VM_IP"]
    return vm, config["REPO_VM_IP"]


def local_record(path: Path, event: str, **fields: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    record = {"utc": dt.datetime.now(dt.timezone.utc).isoformat(), "event": event, **sanitize_config(fields, _known_secrets=_LOCAL_SECRET_VALUES)}
    with path.open("a", encoding="utf-8") as fh:
        fh.write(json.dumps(record, ensure_ascii=False) + "\n")


def renew_all(config: Mapping[str, str], helper: str, state_dirs: Mapping[str, str], trial_id: str, sequences: dict[str, int]) -> None:
    for host, state_dir in state_dirs.items():
        sequences[host] = sequences.get(host, 0) + 1
        remote_call(config, host, helper, "renew", state_dir, trial_id, {"seconds": 30, "sequence": sequences[host]})


def wait_readiness(config: Mapping[str, str], host: str, helper: str, state_dir: str, trial_id: str, role: Mapping[str, Any], deadline: float, offsets: dict[str, int], evidence_events: Path, state_dirs: Mapping[str, str], lease_sequences: dict[str, int]) -> dict[str, Any]:
    expected = {item["id"] for item in role.get("readiness", [])}
    if not expected:
        return {"result": "INCONCLUSIVE", "missing": [], "reason": "no readiness contract"}
    seen: set[str] = set()
    carries: dict[str, str] = {}
    last_renew = 0.0
    while time.monotonic() < deadline:
        now = time.monotonic()
        if now - last_renew >= 5.0:
            renew_all(config, helper, state_dirs, trial_id, lease_sequences)
            last_renew = now
        result = remote_call(config, host, helper, "observe", state_dir, trial_id, {"role": role["name"], "patterns": role.get("readiness", []), "offsets": offsets, "carries": carries, "max_bytes": int(role.get("max_log_read_bytes", 1024 * 1024))})
        offsets.update(result.get("offsets", {}))
        carries.update(result.get("carries", {}))
        if result.get("overflow"):
            return {"result": "INCONCLUSIVE", "markers": sorted(seen), "reason": "log-overflow", "offsets": offsets, "carries": carries}
        if result.get("identity_matches") is False:
            raise RuntimeError(f"remote identity lost for role {role['name']}")
        for match in result.get("matches", []):
            seen.add(match["id"])
            local_record(evidence_events, "marker", role=role["name"], marker=match)
        if expected <= seen:
            return {"result": "OBSERVED", "markers": sorted(seen), "offsets": offsets}
        time.sleep(1.0)
    return {"result": "INCONCLUSIVE", "markers": sorted(seen), "missing": sorted(expected - seen), "offsets": offsets}


def collect_remote(config: Mapping[str, str], host: str, state_dir: str, local_dir: Path) -> None:
    local_dir.mkdir(parents=True, exist_ok=True)
    target = f"{config['NG_SSH_USER']}@{host}:{state_dir}/."
    run = subprocess.run(scp_base(config) + ["-r", target, str(local_dir)], capture_output=True, text=True, timeout=180, check=False)
    if run.returncode != 0:
        raise RuntimeError(f"evidence collection failed on {host}: {run.stderr.strip()}")


def _cleanup_publication_artifacts(evidence_dir: Path) -> list[str]:
    """Remove or quarantine every partially published manifest artifact."""
    root = Path(evidence_dir)
    candidates = [root / "manifest.json", root / "manifest.sha256", root / "terminal-seal.json"]
    candidates.extend(root / f".{name}.{os.getpid()}.tmp" for name in ("manifest.json", "manifest.sha256", "terminal-seal.json"))
    errors: list[str] = []
    try:
        candidates.extend(root.glob(".manifest.json.*.tmp"))
        candidates.extend(root.glob(".manifest.sha256.*.tmp"))
        candidates.extend(root.glob(".terminal-seal.json.*.tmp"))
    except BaseException as exc:
        # The fixed publication names are still cleaned even when directory
        # enumeration itself is fault-injected or unavailable.
        errors.append(f"publication artifact enumeration failed: {type(exc).__name__}")
    for candidate in dict.fromkeys(candidates):
        try:
            candidate.unlink(missing_ok=True)
        except FileNotFoundError:
            continue
        except BaseException as exc:
            quarantine = root.parent / f".{root.name}.publication-quarantine"
            try:
                quarantine.mkdir(parents=True, exist_ok=True)
                os.replace(candidate, quarantine / f"{candidate.name}.{os.getpid()}")
            except BaseException as quarantine_exc:
                errors.append(f"publication artifact cleanup failed: {candidate.name}: {type(quarantine_exc).__name__}")
            else:
                errors.append(f"publication artifact quarantined: {candidate.name}: {type(exc).__name__}")
    return errors


def _durable_atomic_write(path: Path, data: bytes, *, publication_artifact: bool = False) -> None:
    """Replace one evidence record atomically and verify the reopened bytes."""
    temporary = path.with_name(f".{path.name}.{os.getpid()}.tmp")
    try:
        path.parent.mkdir(parents=True, exist_ok=True)
        with temporary.open("xb") as handle:
            handle.write(data)
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary, path)
        directory_fd = os.open(path.parent, os.O_RDONLY | getattr(os, "O_DIRECTORY", 0))
        try:
            os.fsync(directory_fd)
        finally:
            os.close(directory_fd)
        with path.open("rb") as handle:
            reopened = handle.read()
        if reopened != data or hashlib.sha256(reopened).digest() != hashlib.sha256(data).digest():
            raise OSError(f"durable evidence write failed reopen/hash validation: {path.name}")
    except BaseException:
        try:
            temporary.unlink()
        except BaseException:
            pass
        if publication_artifact:
            _cleanup_publication_artifacts(path.parent)
        raise


def _local_manifest_entries(evidence_dir: Path) -> tuple[list[dict[str, Any]], list[str]]:
    """Hash a stable v2 tree and enforce verifier-required coverage."""
    excluded = {"manifest.json", "manifest.sha256", "terminal-seal.json"}
    entries: list[dict[str, Any]] = []
    errors: list[str] = []
    try:
        paths = sorted(evidence_dir.rglob("*"))
    except OSError as exc:
        return [], [f"final evidence scan failed: {type(exc).__name__}"]
    for path in paths:
        relative = path.relative_to(evidence_dir).as_posix()
        if relative in excluded:
            continue
        try:
            if path.is_symlink() or not path.is_file():
                if path.is_symlink():
                    errors.append(f"final evidence scan found unsafe link: {relative}")
                continue
            before = path.stat()
            data = path.read_bytes()
            after = path.stat()
        except (OSError, RuntimeError) as exc:
            errors.append(f"final evidence hash failed: {relative}: {type(exc).__name__}")
            continue
        if (before.st_dev, before.st_ino, before.st_size, before.st_mtime_ns) != (after.st_dev, after.st_ino, after.st_size, after.st_mtime_ns) or len(data) != after.st_size:
            errors.append(f"final evidence hash race: {relative}")
            continue
        digest = hashlib.sha256(data).hexdigest()
        entries.append({"path": relative, "size": len(data), "sha256": digest})
    required = {"result.json", "controller-events.jsonl", "provenance.json", "workload.json", "oracle.json", "ownership.json", "cleanup.json", "preservation.json"}
    listed = {entry["path"] for entry in entries}
    for name in sorted(required - listed):
        errors.append(f"required evidence file is not covered: {name}")
    for prefix in ("plan/", "artifacts/source/", "artifacts/repository/", "roles/", "inventory/"):
        if not any(name.startswith(prefix) for name in listed):
            errors.append(f"required evidence area is not covered: {prefix}")
    return entries, errors


def _write_evidence_manifest(evidence_dir: Path, metadata: dict[str, Any], schema_version: int | None = None) -> Path:
    """Publish a manifest, sealing local v2 only after a clean final scan."""
    effective_schema = schema_version if schema_version is not None else (2 if metadata.get("mode") == "local" else 1)
    if effective_schema not in {1, 2}:
        raise ValueError("unsupported evidence schema version")
    evidence_dir = Path(evidence_dir)
    existing: dict[str, Any] = {}
    blockers: list[str] = []
    metadata_secrets: set[str] = set()
    manifest_path = evidence_dir / "manifest.json"
    if not evidence_dir.is_dir():
        raise OSError(f"evidence root is unavailable: {evidence_dir}")
    # Never leave an older inventory/seal available while a new publication is
    # being evaluated.  Failure to remove either is itself a publication block.
    stale_publications = (manifest_path, evidence_dir / "manifest.sha256", evidence_dir / "terminal-seal.json")
    for stale in stale_publications:
        if not stale.exists():
            continue
        try:
            stale.unlink()
        except OSError as exc:
            blockers.append(f"protected-input cannot safely remove prior publication {stale.name}: {type(exc).__name__}")
    if effective_schema == 2:
        metadata_secrets = collect_secret_values(metadata)
        metadata = sanitize_config(metadata, _known_secrets=_LOCAL_SECRET_VALUES | metadata_secrets)
        supplied = metadata.get("protected_input_blockers", [])
        if isinstance(supplied, list):
            blockers.extend(str(item) for item in supplied)
        result_path = evidence_dir / "result.json"
        try:
            loaded = json.loads(result_path.read_text(encoding="utf-8")) if result_path.is_file() else {}
            existing = loaded if isinstance(loaded, dict) else {}
            prior = existing.get("protected_input_blockers", [])
            if isinstance(prior, list):
                blockers.extend(str(item) for item in prior)
        except (OSError, json.JSONDecodeError, TypeError):
            existing = {}

    def publish_blockers() -> None:
        if not blockers or effective_schema != 2:
            return
        safe_result = dict(existing)
        safe_result["protected_input_blockers"] = list(dict.fromkeys(blockers))
        safe_result["evidence_result"] = "INCOMPLETE"
        safe_result["local_acceptance_eligible"] = False
        acceptance = safe_result.get("acceptance_blockers", [])
        if not isinstance(acceptance, list):
            acceptance = []
        acceptance = list(acceptance)
        for blocker in blockers:
            if blocker not in acceptance:
                acceptance.append(blocker)
        safe_result["acceptance_blockers"] = acceptance
        if safe_result.get("exit_code") == 0:
            safe_result["exit_code"] = 21
        _durable_atomic_write(
            evidence_dir / "result.json",
            (json.dumps(sanitize_config(safe_result, _known_secrets=_LOCAL_SECRET_VALUES | metadata_secrets), ensure_ascii=False, indent=2) + "\n").encode("utf-8"),
        )

    def abort_publication(reason: str) -> None:
        if reason and reason not in blockers:
            blockers.append(reason)
        # Publication artifacts must be gone (or quarantined) before the
        # failure record is written.  This prevents a stale seal from being
        # mistaken for the result of the failed attempt.
        blockers.extend(_cleanup_publication_artifacts(evidence_dir))
        blockers[:] = list(dict.fromkeys(blockers))
        if effective_schema == 2:
            publish_blockers()
        raise OSError("evidence publication aborted: " + "; ".join(blockers))

    if blockers and effective_schema == 2:
        abort_publication("protected-input blocker was already recorded")

    if effective_schema == 2:
        safety = sanitize_evidence_tree(evidence_dir, _LOCAL_SECRET_VALUES | metadata_secrets)
        blockers.extend(str(item) for item in safety.get("blockers", []))
        blockers = list(dict.fromkeys(blockers))
        if blockers:
            metadata = {**metadata, "protected_input_blockers": blockers}
            abort_publication("sanitization or protected-input scan reported a blocker")

    entries, entry_errors = _local_manifest_entries(evidence_dir) if effective_schema == 2 else ([], [])
    if entry_errors and effective_schema == 2:
        blockers.extend(entry_errors)
        metadata = {**metadata, "protected_input_blockers": blockers}
        abort_publication("required-artifact or hash validation reported a blocker")
    if effective_schema == 1:
        entries = []
        for path in sorted(evidence_dir.rglob("*")):
            if path.is_file() and path.name not in {"manifest.json", "manifest.sha256", "terminal-seal.json"}:
                entries.append({"path": str(path.relative_to(evidence_dir)), "size": path.stat().st_size, "sha256": hashlib.sha256(path.read_bytes()).hexdigest()})
    manifest = {**metadata, "schema_version": effective_schema, "files": entries}
    manifest_bytes = (json.dumps(manifest, ensure_ascii=False, indent=2) + "\n").encode("utf-8")
    _durable_atomic_write(manifest_path, manifest_bytes, publication_artifact=True)
    if effective_schema == 1:
        checksum = f"{hashlib.sha256(manifest_bytes).hexdigest()}  manifest.json\n".encode("utf-8")
        _durable_atomic_write(evidence_dir / "manifest.sha256", checksum, publication_artifact=True)
        return manifest_path

    final_safety = sanitize_evidence_tree(evidence_dir, _LOCAL_SECRET_VALUES | metadata_secrets)
    final_blockers = [str(item) for item in final_safety.get("blockers", [])]
    final_entries, final_errors = _local_manifest_entries(evidence_dir)
    if final_blockers or final_errors or final_entries != entries:
        blockers.extend(final_blockers)
        blockers.extend(final_errors)
        if final_entries != entries:
            blockers.append("final evidence scan changed the inventoried file set")
        metadata = {**metadata, "protected_input_blockers": list(dict.fromkeys(blockers))}
        abort_publication("final scan reported a blocker")
    reopened_manifest = manifest_path.read_bytes()
    if reopened_manifest != manifest_bytes or hashlib.sha256(reopened_manifest).hexdigest() != hashlib.sha256(manifest_bytes).hexdigest():
        abort_publication("manifest reopen/hash validation failed")
    manifest_hash = hashlib.sha256(reopened_manifest).hexdigest()
    seal = {"schema_version": 2, "kind": "terminal-seal", "manifest_path": "manifest.json", "manifest_sha256": manifest_hash, "sealed": True}
    seal_bytes = (json.dumps(seal, ensure_ascii=False, indent=2) + "\n").encode("utf-8")
    seal_path = evidence_dir / "terminal-seal.json"
    _durable_atomic_write(seal_path, seal_bytes, publication_artifact=True)
    if seal_path.read_bytes() != seal_bytes or json.loads(seal_path.read_text(encoding="utf-8")).get("manifest_sha256") != hashlib.sha256(manifest_path.read_bytes()).hexdigest():
        abort_publication("terminal seal reopen/hash validation failed")
    return manifest_path


def write_evidence_manifest(evidence_dir: Path, metadata: dict[str, Any], schema_version: int | None = None) -> Path:
    """Publish evidence or clean publication artifacts before failure escapes."""
    root = Path(evidence_dir)
    try:
        return _write_evidence_manifest(root, metadata, schema_version)
    except BaseException:
        _cleanup_publication_artifacts(root)
        raise


def local_verification_summary(report: Mapping[str, Any]) -> dict[str, Any]:
    """Persist the stable offline-verifier verdict without bundle-local paths."""
    code = report.get("code", "NGELC-VERIFIER-ERROR")
    accepted = bool(report.get("accepted", False)) if code == CODE_VALID else False
    reason_codes = [code]
    if code == CODE_VALID and not accepted:
        reason_codes.append("NGELC-ACCEPTANCE-INELIGIBLE")
    summary = {
        "code": code,
        "integrity": bool(report.get("integrity", False)),
        "accepted": accepted,
        "reason_codes": reason_codes,
        "errors": list(report.get("errors", [])),
    }
    if report.get("schema_version") is not None:
        summary["schema_version"] = report["schema_version"]
    return summary


def apply_local_verification(
    result: dict[str, Any],
    eligibility: dict[str, Any],
    report: Mapping[str, Any],
    runtime: str,
    teardown_result: str,
    evidence_result: str,
) -> tuple[dict[str, Any], int, str]:
    """Make local acceptance depend on the sealed bundle's offline verdict."""
    summary = local_verification_summary(report)
    result["offline_verification"] = summary
    result["reason_codes"] = summary["reason_codes"]
    protected = result.get("protected_input_blockers", [])
    if isinstance(protected, list) and protected:
        evidence_result = "INCOMPLETE"
        eligibility["eligible"] = False
        for blocker in protected:
            if blocker not in eligibility["blockers"]:
                eligibility["blockers"].append(blocker)
    if summary["code"] != CODE_VALID:
        evidence_result = "INCOMPLETE"
        eligibility["eligible"] = False
        blocker = f"offline verification failed: {summary['code']}"
        if blocker not in eligibility["blockers"]:
            eligibility["blockers"].append(blocker)
    elif not summary["accepted"]:
        eligibility["eligible"] = False
        blocker = "offline verification is not acceptance-eligible"
        if blocker not in eligibility["blockers"]:
            eligibility["blockers"].append(blocker)
    code = classify_result(runtime, teardown_result, evidence_result)
    # Keep the established local diagnostic/runtime code when runtime already
    # failed; the verifier still marks evidence incomplete and eligibility false.
    if summary["code"] != CODE_VALID and runtime in {"INCONCLUSIVE", "NOT_RUN", "ABORTED"} and teardown_result == "PASS":
        code = 11
    if not eligibility["eligible"] and code == 0:
        code = 21
    result.update({
        "teardown_result": teardown_result,
        "evidence_result": evidence_result,
        "local_acceptance_eligible": eligibility["eligible"],
        "acceptance_blockers": eligibility["blockers"],
        "exit_code": code,
    })
    return result, code, evidence_result


def local_ipc_ids(kind: str, *, include_foreign: bool = False) -> set[str] | None:
    command = [{"shm": "/usr/bin/ipcs", "semaphores": "/usr/bin/ipcs", "queues": "/usr/bin/ipcs"}.get(kind, ""), {"shm": "-m", "semaphores": "-s", "queues": "-q"}.get(kind, "")]
    if not command[1]:
        return None
    try:
        run = subprocess.run(command, capture_output=True, text=True, timeout=10, check=False, env=native_safe_environment() if include_foreign else None)
    except (OSError, subprocess.SubprocessError):
        return None
    if run.returncode != 0:
        return None
    owner = pwd.getpwuid(os.getuid()).pw_name
    header_token = {"shm": "shmid", "semaphores": "semid", "queues": "msqid"}[kind]
    lines = run.stdout.splitlines()
    header_seen = False
    if kind == "shm":
        header_required = {"key", header_token, "owner", "perms", "bytes", "nattch"}
    elif kind == "semaphores":
        header_required = {"key", header_token, "owner", "perms", "nsems"}
    else:
        header_required = {"key", header_token, "owner", "perms", "used-bytes", "messages"}
    ids: set[str] = set()
    for line in lines:
        fields = line.split()
        if header_token in fields:
            if not header_required <= set(fields):
                return None
            header_seen = True
            continue
        if not header_seen or not fields:
            continue
        if not fields[0].startswith("0x"):
            return None
        try:
            int(fields[0], 16)
        except ValueError:
            return None
        minimum_columns = 6 if kind == "shm" else 5
        if kind == "queues":
            minimum_columns = 6
        if len(fields) < minimum_columns or not fields[1].isdigit():
            return None
        permission_index = 3
        if not re.fullmatch(r"[0-7]{3,4}", fields[permission_index]):
            return None
        if kind == "shm":
            if not fields[4].isdigit() or not fields[5].isdigit():
                return None
        elif kind == "semaphores":
            if not fields[4].isdigit():
                return None
        elif not fields[4].isdigit() or not fields[5].isdigit():
            return None
        if include_foreign or fields[2] == owner:
            ids.add(fields[1])
    if not header_seen:
        return None
    return ids


def local_ipc_snapshot(*, include_foreign: bool = False) -> dict[str, Any]:
    return {kind: local_ipc_ids(kind, include_foreign=include_foreign) for kind in ("shm", "semaphores", "queues")}


def read_proc_stat(pid: int) -> dict[str, Any]:
    try:
        stat = Path(f"/proc/{pid}/stat").read_text(encoding="utf-8")
    except FileNotFoundError:
        return {"status": "gone"}
    except OSError as exc:
        return {"status": "unknown", "error": str(exc)}
    closing = stat.rfind(")")
    if closing < 0:
        return {"status": "unknown", "error": "malformed stat: missing comm terminator"}
    fields = stat[closing + 2:].split()
    if len(fields) < 20:
        return {"status": "unknown", "error": "malformed stat: too few fields"}
    try:
        return {"status": "ok", "state": fields[0], "ppid": int(fields[1]), "pgid": int(fields[2]), "starttime": fields[19], "comm": stat[stat.find("(") + 1:closing]}
    except (ValueError, IndexError) as exc:
        return {"status": "unknown", "error": f"malformed stat: {exc}"}


def process_executable_identity(pid: int) -> dict[str, Any]:
    """Read the executable identity without following a caller-supplied path."""
    try:
        return {"status": "ok", "path": os.readlink(f"/proc/{pid}/exe")}
    except FileNotFoundError:
        return {"status": "gone"}
    except OSError as exc:
        return {"status": "unknown", "error": str(exc)}


def process_starttime(pid: int) -> str | None:
    record = _safe_read_proc_stat(pid)
    return record.get("starttime") if record.get("status") == "ok" else None


def process_state(pid: int) -> str | None:
    record = _safe_read_proc_stat(pid)
    return record.get("state") if record.get("status") == "ok" else None


def local_ownership_anchor(trial_id: str) -> dict[str, Any]:
    """Make this long-lived controller a verified child-subreaper anchor."""
    if sys.platform != "linux":
        return {"method": "linux-prctl-subreaper", "pid": os.getpid(), "trial_id": trial_id, "verified": False, "error": "unsupported platform"}
    try:
        libc = ctypes.CDLL(None, use_errno=True)
        prctl = libc.prctl
        prctl.argtypes = [ctypes.c_int, ctypes.c_ulong, ctypes.c_ulong, ctypes.c_ulong, ctypes.c_ulong]
        prctl.restype = ctypes.c_int
        if prctl(36, 1, 0, 0, 0) != 0:  # PR_SET_CHILD_SUBREAPER
            error = ctypes.get_errno()
            raise OSError(error, os.strerror(error))
        current = ctypes.c_ulong(0)
        if prctl(37, ctypes.addressof(current), 0, 0, 0) != 0 or current.value != 1:  # PR_GET_CHILD_SUBREAPER
            raise OSError("kernel did not confirm child-subreaper state")
        return {"method": "linux-prctl-subreaper", "pid": os.getpid(), "trial_id": trial_id, "verified": True}
    except (OSError, AttributeError):
        return {"method": "linux-prctl-subreaper", "pid": os.getpid(), "trial_id": trial_id, "verified": False, "error": "child-subreaper setup failed"}


def _process_environment_markers(pid: int) -> dict[str, str] | None:
    try:
        raw = Path(f"/proc/{pid}/environ").read_bytes()
    except OSError:
        return None
    markers: dict[str, str] = {}
    for item in raw.split(b"\0"):
        if b"=" not in item:
            continue
        key, value = item.split(b"=", 1)
        if key in {b"NG_ELC_TRIAL_ID", b"NG_ELC_TRIAL_ROLE"}:
            try:
                markers[key.decode()] = value.decode()
            except UnicodeDecodeError:
                return None
    return markers


def local_group_members_status(pgid: int) -> dict[str, Any]:
    members: list[dict[str, Any]] = []
    complete = True
    try:
        entries = Path("/proc").iterdir()
    except OSError:
        return {"members": [], "complete": False}
    for entry in entries:
        if not entry.name.isdigit():
            continue
        record = _safe_read_proc_stat(int(entry.name))
        if record["status"] == "gone":
            continue
        if record["status"] != "ok":
            complete = False
            continue
        if record["pgid"] == pgid:
            executable = _safe_executable_identity(int(entry.name))
            if executable["status"] != "ok":
                complete = False
                continue
            members.append({"pid": int(entry.name), "pgid": pgid, "comm": record["comm"], "starttime": record["starttime"], "executable": executable["path"]})
    return {"members": sorted(members, key=lambda item: item["pid"]), "complete": complete}


def local_group_members(pgid: int) -> list[dict[str, Any]]:
    return local_group_members_status(pgid)["members"]


def local_process_descendants(root_pid: int) -> dict[str, Any]:
    records: dict[int, dict[str, Any]] = {}
    complete = True
    try:
        entries = Path("/proc").iterdir()
    except OSError:
        return {"descendants": [], "complete": False}
    for entry in entries:
        if not entry.name.isdigit():
            continue
        pid = int(entry.name)
        record = _safe_read_proc_stat(pid)
        if record["status"] == "gone":
            continue
        if record["status"] != "ok":
            complete = False
            continue
        if record.get("state") in {"Z", "X"}:
            continue
        records[pid] = {"pid": pid, "ppid": record["ppid"], "pgid": record["pgid"], "starttime": record["starttime"], "comm": record["comm"]}
    children: dict[int, list[int]] = {}
    for item in records.values():
        children.setdefault(item["ppid"], []).append(item["pid"])
    descendants: list[dict[str, Any]] = []
    pending = list(children.get(root_pid, []))
    seen: set[int] = set()
    while pending:
        pid = pending.pop(0)
        if pid in seen:
            continue
        seen.add(pid)
        item = records.get(pid)
        if item is None:
            complete = False
            continue
        descendants.append(item)
        executable = _safe_executable_identity(pid)
        if executable.get("status") != "ok":
            complete = False
        else:
            item["executable"] = executable["path"]
        markers = _process_environment_markers(pid)
        if markers:
            item.update({"trial_id": markers.get("NG_ELC_TRIAL_ID"), "trial_role": markers.get("NG_ELC_TRIAL_ROLE")})
        pending.extend(children.get(pid, []))
    return {"descendants": sorted(descendants, key=lambda item: item["pid"]), "complete": complete}


def _safe_read_proc_stat(pid: int) -> dict[str, Any]:
    try:
        return read_proc_stat(pid)
    except BaseException as exc:
        return {"status": "unknown", "error": f"identity scan failed: {exc}"}


def _safe_executable_identity(pid: int) -> dict[str, Any]:
    try:
        return process_executable_identity(pid)
    except BaseException as exc:
        return {"status": "unknown", "error": f"executable identity scan failed: {exc}"}


def local_stop_tracked_descendants(tracked: Mapping[int, Mapping[str, Any]]) -> tuple[bool, list[dict[str, Any]]]:
    residual: list[dict[str, Any]] = []
    for pid, expected in tracked.items():
        record = _safe_read_proc_stat(pid)
        if record["status"] == "gone":
            continue
        if record["status"] != "ok":
            residual.append({"pid": pid, "reason": "identity-unavailable", "error": record.get("error")})
            continue
        if record.get("state") in {"Z", "X"}:
            continue
        expected_pgid = expected.get("pgid")
        if expected_pgid is None:
            residual.append({"pid": pid, "reason": "identity-unavailable", "error": "tracked process-group identity missing"})
            continue
        if record["starttime"] != expected.get("starttime"):
            residual.append({"pid": pid, "reason": "identity-mismatch", "expected_starttime": expected.get("starttime"), "actual_starttime": record["starttime"]})
            continue
        if record["pgid"] != expected_pgid:
            expected = dict(expected)
            expected["reparented_pgid"] = record["pgid"]
        expected_executable = expected.get("executable")
        if not isinstance(expected_executable, str) or not expected_executable:
            residual.append({"pid": pid, "reason": "executable-identity-unavailable", "error": "tracked executable identity is missing"})
            continue
        executable = _safe_executable_identity(pid)
        if executable.get("status") != "ok":
            residual.append({"pid": pid, "reason": "executable-identity-unavailable", "error": executable.get("error")})
            continue
        if executable.get("path") != expected_executable:
            residual.append({"pid": pid, "reason": "executable-identity-mismatch", "expected_executable": expected_executable, "actual_executable": executable.get("path")})
            continue
        try:
            os.kill(pid, signal.SIGTERM)
        except ProcessLookupError:
            continue
        except OSError as exc:
            residual.append({"pid": pid, "reason": "term-error", "error": str(exc)})
            continue
        deadline = time.monotonic() + 2
        unknown: dict[str, Any] | None = None
        while time.monotonic() < deadline:
            current = _safe_read_proc_stat(pid)
            if current["status"] == "gone" or (current["status"] == "ok" and current["state"] in {"Z", "X"}):
                break
            if current["status"] != "ok":
                unknown = current
                break
            if current["starttime"] != expected.get("starttime"):
                residual.append({"pid": pid, "reason": "identity-mismatch-after-term", "expected_starttime": expected.get("starttime"), "actual_starttime": current["starttime"]})
                unknown = {"status": "mismatch"}
                break
            if expected_executable is not None:
                executable = _safe_executable_identity(pid)
                if executable.get("status") == "gone":
                    confirmed = _safe_read_proc_stat(pid)
                    if confirmed.get("status") == "gone" or confirmed.get("state") in {"Z", "X"}:
                        break
                    time.sleep(0.05)
                    continue
                if executable.get("status") != "ok" or executable.get("path") != expected_executable:
                    residual.append({"pid": pid, "reason": "executable-identity-mismatch-after-term", "expected_executable": expected_executable, "actual_executable": executable.get("path"), "error": executable.get("error")})
                    unknown = {"status": "mismatch"}
                    break
            time.sleep(0.05)
        if unknown:
            residual.append({"pid": pid, "reason": "identity-unavailable-after-term", "error": unknown.get("error")})
            continue
        current = _safe_read_proc_stat(pid)
        if current["status"] == "gone" or (current["status"] == "ok" and current["state"] in {"Z", "X"}):
            continue
        if current["status"] != "ok":
            residual.append({"pid": pid, "reason": "identity-unavailable-before-kill", "error": current.get("error")})
            continue
        if current["starttime"] != expected.get("starttime"):
            residual.append({"pid": pid, "reason": "identity-mismatch-before-kill", "expected_starttime": expected.get("starttime"), "actual_starttime": current.get("starttime")})
            continue
        if expected_executable is not None:
            executable = _safe_executable_identity(pid)
            if executable.get("status") != "ok" or executable.get("path") != expected_executable:
                residual.append({"pid": pid, "reason": "executable-identity-mismatch-before-kill", "expected_executable": expected_executable, "actual_executable": executable.get("path"), "error": executable.get("error")})
                continue
        try:
            os.kill(pid, signal.SIGKILL)
        except ProcessLookupError:
            continue
        except OSError as exc:
            residual.append({"pid": pid, "reason": "kill-error", "error": str(exc)})
            continue
        deadline = time.monotonic() + 2
        while time.monotonic() < deadline:
            current = _safe_read_proc_stat(pid)
            if current["status"] == "gone" or (current["status"] == "ok" and current["state"] in {"Z", "X"}):
                break
            if current["status"] != "ok":
                residual.append({"pid": pid, "reason": "identity-unavailable-after-kill", "error": current.get("error")})
                break
            if current.get("starttime") != expected.get("starttime"):
                residual.append({"pid": pid, "reason": "identity-mismatch-after-kill", "expected_starttime": expected.get("starttime"), "actual_starttime": current.get("starttime")})
                break
            time.sleep(0.05)
        else:
            residual.append({"pid": pid, "reason": "survived-descendant-stop", "starttime": expected.get("starttime")})
    return not residual, residual


def local_descendant_reconciliation(
    root_pid: int,
    tracked: dict[int, dict[str, Any]],
    pgid: int | None,
    *,
    scans: int = 4,
    owner_pid: int | None = None,
    trial_id: str | None = None,
    role: str | None = None,
) -> dict[str, Any]:
    """Boundedly drain tracked descendants after the leader signal.

    Descendant discovery is never treated as complete after one sample.  Each
    post-signal round records scan uncertainty, merges both ancestry and group
    observations, and revalidates tracked PID identities before signalling.
    """
    rounds = max(1, min(int(scans), 8))
    residual: list[dict[str, Any]] = []
    unknown: list[dict[str, Any]] = []
    scan_ledger: list[dict[str, Any]] = []
    for number in range(rounds):
        try:
            ancestry = local_process_descendants(root_pid)
        except BaseException as exc:
            ancestry = {"descendants": [], "complete": False, "error": str(exc)}
        scan_ledger.append({"round": number + 1, "ancestry_complete": bool(ancestry.get("complete", False))})
        if not ancestry.get("complete", False):
            unknown.append({"round": number + 1, "kind": "ancestry-scan", "error": ancestry.get("error", "scan incomplete")})
        for child in ancestry.get("descendants", []):
            if isinstance(child, Mapping) and isinstance(child.get("pid"), int):
                tracked[child["pid"]] = dict(child)

        # Once the leader exits, the verified subreaper is the ownership
        # anchor.  Only marker-matched descendants may enter this role's
        # ledger; an unmarked process is deliberately left unsignalled and
        # makes the teardown unresolved.
        if owner_pid is not None:
            try:
                anchored = local_process_descendants(owner_pid)
            except BaseException as exc:
                anchored = {"descendants": [], "complete": False, "error": str(exc)}
            scan_ledger[-1]["anchor_complete"] = bool(anchored.get("complete", False))
            if not anchored.get("complete", False):
                unknown.append({"round": number + 1, "kind": "anchor-scan", "error": anchored.get("error", "anchor scan incomplete")})
            for child in anchored.get("descendants", []):
                if not isinstance(child, Mapping) or not isinstance(child.get("pid"), int):
                    continue
                if child.get("pid") == root_pid:
                    continue
                if child.get("trial_id") == trial_id and child.get("trial_role") == role:
                    tracked[child["pid"]] = dict(child)
                elif child.get("trial_id") == trial_id:
                    continue
                else:
                    unknown.append({"round": number + 1, "kind": "anchor-unattributed-process", "pid": child.get("pid")})

        if pgid is not None:
            try:
                group = local_group_members_status(pgid)
            except BaseException as exc:
                group = {"members": [], "complete": False, "error": str(exc)}
            scan_ledger[-1]["group_complete"] = bool(group.get("complete", False))
            if not group.get("complete", False):
                unknown.append({"round": number + 1, "kind": "group-scan", "error": group.get("error", "scan incomplete")})
            for member in group.get("members", []):
                if isinstance(member, Mapping) and isinstance(member.get("pid"), int) and member["pid"] != root_pid:
                    tracked[member["pid"]] = dict(member)

        try:
            stopped, round_residual = local_stop_tracked_descendants(tracked)
        except BaseException as exc:
            stopped, round_residual = False, [{"reason": "descendant-stop-unknown", "error": str(exc)}]
        del stopped
        residual.extend(round_residual)
        if number + 1 < rounds:
            time.sleep(0.05)

    def unique(items: list[dict[str, Any]]) -> list[dict[str, Any]]:
        result: list[dict[str, Any]] = []
        seen: set[str] = set()
        for item in items:
            key = json.dumps(item, sort_keys=True, default=str)
            if key not in seen:
                seen.add(key)
                result.append(item)
        return result

    residual = unique(residual)
    unknown = unique(unknown)
    return {
        "ok": not residual and not unknown,
        "scan_count": rounds,
        "scans": scan_ledger,
        "tracked": sorted(tracked.values(), key=lambda item: item.get("pid", 0)),
        "residual": residual,
        "unknown": unknown,
    }


def local_process_snapshot(processes: Mapping[str, dict[str, Any]]) -> list[dict[str, Any]]:
    result = []
    for role, item in processes.items():
        proc = item["process"]
        result.append({
            "role": role,
            "pid": proc.pid,
            "pgid": item.get("pgid"),
            "starttime": item.get("starttime"),
            "returncode": proc.poll(),
            "running": proc.poll() is None,
            "executable": item["argv"][0],
            "launched_identity": item.get("launched_identity"),
            "tracked_descendants": sorted(item.get("tracked_descendants", {}).values(), key=lambda child: child.get("pid", 0)),
            "descendant_scan_seen": item.get("descendant_scan_seen", False),
            "descendant_scan_complete": item.get("descendant_scan_complete", True),
            "rollback_uncertain": item.get("rollback_uncertain", False),
            "unresolved": item.get("unresolved", False),
            "descendant_rollback_ledger": item.get("descendant_rollback_ledger", {}),
        })
    return result


def log_quota_exceeded(paths: list[Path], quota: int) -> bool:
    return any(path.is_file() and path.stat().st_size > quota for path in paths)


def local_acceptance_eligibility(provenance: Mapping[str, Any]) -> dict[str, Any]:
    checks = {
        "git_clean": "dirty source tree",
        "build_linkage": "build linkage missing",
        "controller_identity": "controller identity missing",
        "plan_snapshot": "plan snapshot missing",
    }
    blockers = [message for key, message in checks.items() if not provenance.get(key, False)]
    if "build_manifest" in provenance and not provenance.get("build_manifest"):
        blockers.append("build manifest is empty or invalid")
    for key, message in (
        ("workload_verified", "fresh workload is not verified"),
        ("evidence_schema_v2", "evidence schema v2 is not verified"),
        ("source_snapshot_complete", "source content snapshot is incomplete"),
    ):
        if key in provenance and not provenance.get(key, False):
            blockers.append(message)
    workload_error = provenance.get("workload_error")
    if workload_error:
        blockers.append(str(workload_error))
    if provenance.get("protected_inputs") or provenance.get("protected_input_blockers"):
        blockers.append("protected-input provenance blocker")
    return {"eligible": not blockers, "blockers": blockers}


def local_observe(processes: Mapping[str, dict[str, Any]], offsets: dict[str, int], carries: dict[str, str], patterns: list[dict[str, Any]], max_bytes: int = 64 * 1024 * 1024) -> dict[str, Any]:
    for item in processes.values():
        scan = local_process_descendants(item["process"].pid)
        tracked = item.setdefault("tracked_descendants", {})
        item["descendant_scan_seen"] = True
        item["descendant_scan_complete"] = item.get("descendant_scan_complete", True) and scan["complete"]
        for descendant in scan["descendants"]:
            tracked[descendant["pid"]] = descendant
    matches = []
    for role, item in processes.items():
        for stream, handle in (("stdout", item["stdout"]), ("stderr", item["stderr"])):
            handle.flush()
            path = item[stream + "_path"]
            if log_quota_exceeded([path], max_bytes):
                return {"matches": matches, "processes": local_process_snapshot(processes), "overflow": True}
            text = path.read_text(encoding="utf-8", errors="replace") if path.exists() else ""
            key = f"{role}:{stream}"
            start = offsets.get(key, 0)
            chunk = carries.get(key, "") + text[start:]
            offsets[key] = len(text)
            lines = chunk.splitlines(keepends=True)
            carries[key] = "" if not lines or lines[-1].endswith("\n") else lines.pop()
            for pattern in patterns:
                needle = pattern["pattern"]
                for line in lines:
                    if needle in line:
                        matches.append({"role": role, "stream": stream, "id": pattern["id"], "line": line.rstrip("\n")})
    return {"matches": matches, "processes": local_process_snapshot(processes)}


def latch_oracle_result(previous: str, current: str) -> str:
    """Keep an observed oracle failure sticky across later polls."""
    if previous == "FAIL" or current == "FAIL":
        return "FAIL"
    return current


def marker_requirements_satisfied(seen: set[tuple[str, str]], required: list[dict[str, Any]]) -> bool:
    required_keys = {(item["role"], item["id"]) for item in required}
    return required_keys <= seen


def marker_runtime_result(previous: str, seen: set[tuple[str, str]], required: list[dict[str, Any]]) -> str:
    if previous == "FAIL":
        return "FAIL"
    return "PASS" if marker_requirements_satisfied(seen, required) else "INCONCLUSIVE"


def preserve_runtime_failure(previous: str) -> str:
    return previous if previous == "FAIL" else "INCONCLUSIVE"


def local_file_oracle(oracle: Mapping[str, Any], variables: Mapping[str, str]) -> dict[str, Any]:
    def digest_map(raw_path: str) -> tuple[dict[str, dict[str, Any]], list[str]]:
        path = Path(expand_argv([raw_path], variables)[0])
        result: dict[str, dict[str, Any]] = {}
        unsafe: list[str] = []
        for item in sorted(path.glob(oracle["pattern"])):
            if item.is_symlink():
                unsafe.append(item.name)
                continue
            if item.is_file():
                data = item.read_bytes()
                result[item.name] = {"size": len(data), "sha256": hashlib.sha256(data).hexdigest()}
        return result, unsafe
    source, unsafe_source = digest_map(oracle["source"])
    repository, unsafe_repository = digest_map(oracle["repository"])
    expected = oracle.get("expected_map")
    if expected is not None and not isinstance(expected, Mapping):
        raise ValueError("file oracle expected_map must be an object")
    expected_map = dict(expected or {})
    source_matches_expected = not expected_map or source == expected_map
    repository_matches_expected = not expected_map or repository == expected_map
    source_extra = sorted(set(source) - set(expected_map)) if expected_map else []
    repository_extra = sorted(set(repository) - set(expected_map)) if expected_map else []
    changed_source = sorted(name for name in set(source) & set(expected_map) if source[name] != expected_map[name])
    changed_repository = sorted(name for name in set(repository) & set(expected_map) if repository[name] != expected_map[name])
    result = {
        "source_count": len(source),
        "repository_count": len(repository),
        "expected_count": oracle["expected_count"],
        "source_map": source,
        "repository_map": repository,
        "expected_map": expected_map,
        "unsafe_source": unsafe_source,
        "unsafe_repository": unsafe_repository,
        "source_equals_repository": source == repository,
        "source_matches_expected": source_matches_expected,
        "repository_matches_expected": repository_matches_expected,
        "missing_in_repository": sorted(set(source) - set(repository)),
        "extra_in_repository": sorted(set(repository) - set(source)),
        "hash_mismatches": sorted(name for name in set(source) & set(repository) if source[name] != repository[name]),
        "source_extra": source_extra,
        "repository_extra": repository_extra,
        "changed_source": changed_source,
        "changed_repository": changed_repository,
    }
    complete = len(source) == oracle["expected_count"] and len(repository) == oracle["expected_count"]
    safe = not unsafe_source and not unsafe_repository
    if expected_map:
        expected_count_ok = len(expected_map) == oracle["expected_count"] and complete
        divergence = bool(source_extra or repository_extra or changed_source or changed_repository)
        result["result"] = "FAIL" if safe and divergence else ("PASS" if safe and expected_count_ok and source_matches_expected and repository_matches_expected else "INCONCLUSIVE")
    else:
        result["result"] = "PASS" if complete and safe and result["source_equals_repository"] else ("FAIL" if complete and safe else "INCONCLUSIVE")
    return result


def local_digest_map(directory: Path, pattern: str = "*") -> dict[str, dict[str, Any]]:
    """Capture a stable, link-free name/size/SHA-256 map for a local directory."""
    result: dict[str, dict[str, Any]] = {}
    if not directory.is_dir():
        return result
    for path in sorted(directory.glob(pattern)):
        if path.is_symlink() or not path.is_file():
            continue
        data = path.read_bytes()
        result[path.name] = {"size": len(data), "sha256": hashlib.sha256(data).hexdigest()}
    return result


def local_json_write(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(sanitize_config(value, _known_secrets=_LOCAL_SECRET_VALUES), ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def validate_workload_spec(workload: Mapping[str, Any]) -> None:
    """Validate the small, versioned deterministic local-workload contract."""
    if not isinstance(workload, Mapping) or workload.get("schema_version") != 1:
        raise ValueError("local workload schema_version must be 1")
    generator = workload.get("generator")
    if not isinstance(generator, Mapping):
        raise ValueError("local workload generator must be an object")
    if generator.get("id") != "synthetic-jpeg" or not isinstance(generator.get("version"), str) or not generator["version"]:
        raise ValueError("local workload generator identity is invalid")
    seed = generator.get("seed")
    if isinstance(seed, bool) or not isinstance(seed, int):
        raise ValueError("local workload seed must be an integer")
    parameters = generator.get("parameters")
    if not isinstance(parameters, Mapping):
        raise ValueError("local workload parameters must be an object")
    if parameters.get("count") != 5:
        raise ValueError("local workload count must be five")
    size_bytes = parameters.get("size_bytes")
    if isinstance(size_bytes, bool) or not isinstance(size_bytes, int) or not 16 <= size_bytes <= 16 * 1024 * 1024:
        raise ValueError("local workload size_bytes is outside the bounded range")
    prefix = parameters.get("prefix")
    if not isinstance(prefix, str) or not re.fullmatch(r"[A-Za-z0-9_.-]+", prefix):
        raise ValueError("local workload prefix is unsafe")
    names = workload.get("names")
    if not isinstance(names, list) or len(names) != 5 or len(set(names)) != 5:
        raise ValueError("local workload must declare five unique names")
    for name in names:
        if not isinstance(name, str) or not re.fullmatch(r"[A-Za-z0-9_.-]+\.jpg", name):
            raise ValueError("local workload names must be safe JPEG names")


def generate_deterministic_workload(workload: Mapping[str, Any], destination: Path) -> dict[str, dict[str, Any]]:
    """Generate the declared five JPEG-shaped files without overwriting data."""
    validate_workload_spec(workload)
    destination = Path(destination)
    if destination.exists() and any(destination.iterdir()):
        raise ConfigError(f"workload staging directory is not fresh: {destination}")
    destination.mkdir(parents=True, exist_ok=True)
    generator = workload["generator"]
    parameters = generator["parameters"]
    size_bytes = parameters["size_bytes"]
    parameter_bytes = json.dumps(parameters, sort_keys=True, separators=(",", ":")).encode("utf-8")
    for name in workload["names"]:
        material = hashlib.sha256(
            b"SPEC-056/R02/" + str(generator["version"]).encode("utf-8") + b"/" +
            str(generator["seed"]).encode("ascii") + b"/" + name.encode("utf-8") + b"/" + parameter_bytes
        ).digest()
        header = bytes((0xff, 0xd8, 0xff, 0xe0)) + b"SPEC056" + material[:8]
        footer = bytes((0xff, 0xd9))
        body_size = size_bytes - len(header) - len(footer)
        body = (material * ((body_size + len(material) - 1) // len(material)))[:body_size]
        path = destination / name
        with path.open("xb") as handle:
            handle.write(header + body + footer)
    return local_digest_map(destination, "*.jpg")


def _path_aliases(first: Path, second: Path) -> bool:
    """Detect equal, nested, symlink-resolved, or same-inode IO directories."""
    first_resolved = first.resolve(strict=False)
    second_resolved = second.resolve(strict=False)
    if first_resolved == second_resolved or first_resolved in second_resolved.parents or second_resolved in first_resolved.parents:
        return True
    try:
        return first.exists() and second.exists() and first.samefile(second)
    except OSError:
        return True


def _directory_has_entries(path: Path) -> bool:
    if not path.exists():
        return False
    if not path.is_dir():
        return True
    try:
        next(path.iterdir())
    except StopIteration:
        return False
    return True


def _local_oracle_directories(plan: Mapping[str, Any], variables: Mapping[str, str]) -> tuple[Path, Path]:
    oracle = plan.get("runtime_oracle") or {}
    source = oracle.get("source", "${NG_LOCAL_IO_PATH}/Source1")
    repository = oracle.get("repository", "${NG_LOCAL_IO_PATH}/Repository1")
    return Path(expand_argv([source], variables)[0]), Path(expand_argv([repository], variables)[0])


def local_prepare_v2_layout(evidence_dir: Path, plan: Mapping[str, Any], contract: Mapping[str, Any], config: Mapping[str, str], variables: Mapping[str, str]) -> dict[str, Any]:
    """Create durable v2 directories and preserve the pre-launch workload map."""
    for directory in (evidence_dir / "artifacts" / "source", evidence_dir / "artifacts" / "repository", evidence_dir / "inventory"):
        directory.mkdir(parents=True, exist_ok=True)
    source, repository = _local_oracle_directories(plan, variables)
    declared_workload = plan.get("workload")
    if declared_workload is not None:
        validate_workload_spec(declared_workload)
        aliasing = _path_aliases(source, repository)
        repository_initial_map = local_digest_map(repository, "*.jpg")
        repository_nonempty = _directory_has_entries(repository)
        staging_io = evidence_dir / "staging" / "io"
        staging_source = staging_io / "Source1"
        staging_repository = staging_io / "Repository1"
        staging_io.mkdir(parents=True, exist_ok=False)
        for directory in (staging_source, staging_repository, staging_io / "PGCS", staging_io / "NRNCS"):
            directory.mkdir(parents=True, exist_ok=False)
        expected_map = generate_deterministic_workload(declared_workload, staging_source)
        variables["NG_LOCAL_IO_PATH"] = str(staging_io)
        effective_config = dict(config)
        effective_config["NG_LOCAL_IO_PATH"] = str(staging_io)
        local_write_v2_plan(evidence_dir, plan, contract, effective_config, variables)
        verified = not repository_nonempty and not aliasing and len(expected_map) == 5
        reason = None
        if repository_nonempty:
            reason = "Repository directory is not empty"
        elif aliasing:
            reason = "Source and Repository paths alias or nest"
        workload = {
            "schema_version": 2,
            "generator": dict(declared_workload["generator"]),
            "parameters": dict(declared_workload["generator"]["parameters"]),
            "names": list(declared_workload["names"]),
            "expected_map": expected_map,
            "expected": [{"name": name, **expected_map[name]} for name in sorted(expected_map)],
            "repository_initial_map": repository_initial_map,
            "repository_initial": [{"name": name, **repository_initial_map[name]} for name in sorted(repository_initial_map)],
            "repository_empty": not repository_nonempty,
            "aliasing_rejected": aliasing,
            "source_path": str(staging_source),
            "repository_path": str(staging_repository),
            "configured_source_path": str(source),
            "configured_repository_path": str(repository),
            "staging_io_path": str(staging_io),
            "verified": verified,
            "reason": reason,
        }
    else:
        # Keep older local plans usable as diagnostic fixtures. They remain
        # ineligible because they do not declare a deterministic workload.
        local_write_v2_plan(evidence_dir, plan, contract, config, variables)
        expected = local_digest_map(source, "*.jpg")
        repository_initial = local_digest_map(repository, "*.jpg")
        workload = {
            "schema_version": 2,
            "generator": {"id": "external-input", "version": "unverified", "seed": None, "parameters": {}, "verified": False},
            "source_path": str(source),
            "repository_path": str(repository),
            "expected": [{"name": name, **expected[name]} for name in sorted(expected)],
            "repository_initial": [{"name": name, **repository_initial[name]} for name in sorted(repository_initial)],
            "expected_map": expected,
            "repository_initial_map": repository_initial,
            "repository_empty": not _directory_has_entries(repository),
            "aliasing_rejected": _path_aliases(source, repository),
            "verified": False,
            "reason": "plan does not declare a deterministic workload",
        }
    local_json_write(evidence_dir / "workload.json", workload)
    return workload


def preserve_local_artifacts(oracle: Mapping[str, Any], variables: Mapping[str, str], evidence_dir: Path) -> dict[str, Any]:
    """Copy both oracle sides into evidence and verify their hash maps."""
    result = local_file_oracle(oracle, variables)
    preserved: dict[str, dict[str, dict[str, Any]]] = {}
    for label, raw_path, source_map in (
        ("source", oracle["source"], result["source_map"]),
        ("repository", oracle["repository"], result["repository_map"]),
    ):
        source_dir = Path(expand_argv([raw_path], variables)[0])
        target_dir = evidence_dir / "artifacts" / label
        target_dir.mkdir(parents=True, exist_ok=True)
        preserved[label] = {}
        for name, expected in source_map.items():
            source_path = source_dir / name
            target_path = target_dir / name
            if source_path.is_symlink() or not source_path.is_file():
                continue
            shutil.copy2(source_path, target_path)
            preserved[label][name] = {"size": target_path.stat().st_size, "sha256": hashlib.sha256(target_path.read_bytes()).hexdigest()}
    result["preserved_source_map"] = preserved["source"]
    result["preserved_repository_map"] = preserved["repository"]
    result["preservation_verified"] = (
        result["source_map"] == result["preserved_source_map"]
        and result["repository_map"] == result["preserved_repository_map"]
    )
    return result


def _source_identity_matches(provenance: Mapping[str, Any]) -> tuple[bool, str | None]:
    source = provenance.get("source")
    repo = provenance.get("repo_path")
    if not isinstance(source, Mapping) or not isinstance(repo, str) or not repo:
        return False, "source identity is missing"
    try:
        current = capture_git_state(repo, include_tree=True)
    except (OSError, RuntimeError, subprocess.SubprocessError, ValueError) as exc:
        return False, f"source identity unavailable: {exc}"
    for field in ("head", "status", "tree_sha256", "index_sha256", "index_entries", "submodules", "submodules_complete"):
        if source.get(field) != current.get(field):
            return False, f"source {field} drift"
    index = source.get("index")
    if not isinstance(index, Mapping) or index.get("captured") is not True:
        return False, "source index identity is incomplete"
    content = source.get("content_snapshot")
    if not isinstance(content, Mapping) or content.get("captured") is not True or content.get("errors"):
        return False, "source content snapshot is incomplete"
    for record in content.get("files", []):
        relative = record.get("path")
        if not isinstance(relative, str):
            return False, "source snapshot path is invalid"
        try:
            identity = file_identity(Path(repo) / relative)
        except (FileNotFoundError, OSError, ValueError) as exc:
            return False, f"source snapshot input unavailable: {exc}"
        if identity["sha256"] != record.get("sha256"):
            return False, f"source snapshot drift: {relative}"
        evidence_dir = provenance.get("evidence_dir")
        snapshot_path = record.get("snapshot_path")
        if not isinstance(evidence_dir, str) or not isinstance(snapshot_path, str):
            return False, f"source preserved snapshot is missing: {relative}"
        try:
            preserved_path = (Path(evidence_dir) / snapshot_path).resolve(strict=False)
            preserved_path.relative_to(Path(evidence_dir).resolve())
            preserved = file_identity(preserved_path)
        except (FileNotFoundError, OSError, ValueError) as exc:
            return False, f"source preserved snapshot unavailable: {exc}"
        if preserved["sha256"] != record.get("snapshot_sha256", record.get("sha256")) or preserved["size"] != record.get("size"):
            return False, f"source preserved snapshot drift: {relative}"
        staged_path = record.get("staged_snapshot_path")
        if staged_path:
            try:
                staged_candidate = (Path(evidence_dir) / staged_path).resolve(strict=False)
                staged_candidate.relative_to(Path(evidence_dir).resolve())
                staged = file_identity(staged_candidate)
            except (FileNotFoundError, OSError, ValueError) as exc:
                return False, f"staged source snapshot unavailable: {exc}"
            if staged["sha256"] != record.get("staged_sha256") or staged["size"] != record.get("staged_size"):
                return False, f"staged source snapshot drift: {relative}"
        captured_staged_hash = record.get("staged_sha256")
        if captured_staged_hash is not None:
            current_staged = local_provenance_module_current_index_blob(Path(repo), relative)
            if current_staged is None:
                return False, f"current staged source input unavailable: {relative}"
            if current_staged["sha256"] != captured_staged_hash or current_staged["size"] != record.get("staged_size"):
                return False, f"current index blob drift: {relative}"
    return True, None


def _hash_open_fd(fd: int) -> tuple[int, str]:
    """Hash an already-open regular file, never a pathname."""

    digest = hashlib.sha256()
    size = 0
    os.lseek(fd, 0, os.SEEK_SET)
    while True:
        chunk = os.read(fd, 1024 * 1024)
        if not chunk:
            break
        size += len(chunk)
        digest.update(chunk)
    os.lseek(fd, 0, os.SEEK_SET)
    return size, digest.hexdigest()


def _create_sealed_memfd(name: str) -> int:
    """Create a Linux sealing-capable memfd on old Python builds too."""
    flags = getattr(os, "MFD_CLOEXEC", 0x0001) | getattr(os, "MFD_ALLOW_SEALING", 0x0002)
    creator = getattr(os, "memfd_create", None)
    if creator is not None:
        return creator(name, flags)
    syscall_numbers = {"x86_64": 319, "amd64": 319, "aarch64": 279, "arm64": 279}
    number = syscall_numbers.get(os.uname().machine)
    if number is None:
        raise OSError(f"memfd_create is unavailable on {os.uname().machine}")
    fd = ctypes.CDLL(None, use_errno=True).syscall(number, ctypes.c_char_p(name.encode()), ctypes.c_uint(flags))
    if fd < 0:
        error = ctypes.get_errno()
        raise OSError(error, os.strerror(error))
    return int(fd)


def _seal_memfd(fd: int) -> int:
    add_seals = getattr(fcntl, "F_ADD_SEALS", 1033)
    get_seals = getattr(fcntl, "F_GET_SEALS", 1034)
    seals = getattr(fcntl, "F_SEAL_SEAL", 0x0001) | getattr(fcntl, "F_SEAL_SHRINK", 0x0002) | getattr(fcntl, "F_SEAL_GROW", 0x0004) | getattr(fcntl, "F_SEAL_WRITE", 0x0008)
    fcntl.fcntl(fd, add_seals, seals)
    if fcntl.fcntl(fd, get_seals) & seals != seals:
        raise OSError("memfd seals were not applied")
    return seals


def prepare_local_executable(provenance: Mapping[str, Any], role: str, argv0: str) -> dict[str, Any]:
    """Make a sealed executable image and bind the launch to that image.

    The source pathname is opened once, copied to a Linux memfd, and sealed
    against writes before ``Popen`` receives its inherited ``/proc`` path.
    Other platforms deliberately fail closed: a pathname re-check is not a
    binding primitive.
    """

    if not provenance.get("build_linkage"):
        raise ValueError(f"build linkage missing for {role}")
    manifest_record = provenance.get("build_manifest")
    if isinstance(manifest_record, Mapping) and manifest_record.get("path") and manifest_record.get("sha256"):
        try:
            current_manifest = file_identity(manifest_record["path"])
        except (FileNotFoundError, OSError, ValueError) as exc:
            raise ValueError(f"build manifest unavailable: {exc}") from exc
        if current_manifest["sha256"] != manifest_record["sha256"] or current_manifest["size"] != manifest_record.get("size"):
            raise ValueError("build manifest drift")
        preserved_path = manifest_record.get("preserved_path")
        evidence_dir = provenance.get("evidence_dir")
        if not isinstance(preserved_path, str) or not isinstance(evidence_dir, str):
            raise ValueError("preserved manifest evidence is missing")
        try:
            preserved_candidate = (Path(evidence_dir) / preserved_path).resolve(strict=False)
            preserved_candidate.relative_to(Path(evidence_dir).resolve())
            preserved = file_identity(preserved_candidate)
        except (FileNotFoundError, OSError, ValueError) as exc:
            raise ValueError(f"preserved manifest evidence unavailable: {exc}") from exc
        if preserved["sha256"] != manifest_record.get("preserved_sha256") or preserved["size"] != manifest_record.get("preserved_size"):
            raise ValueError("preserved manifest evidence drift")
    else:
        raise ValueError("preserved build manifest is missing")
    expected = provenance.get("binaries", {}).get(role) if isinstance(provenance.get("binaries"), Mapping) else None
    if not isinstance(expected, Mapping) or not expected.get("resolved_path") or not expected.get("sha256"):
        raise ValueError(f"build linkage missing for {role}")
    if sys.platform != "linux" or fcntl is None or not hasattr(os, "O_NOFOLLOW"):
        raise ValueError(f"immutable executable binding is unavailable on this platform for {role}")
    source_ok, source_error = _source_identity_matches(provenance)
    if not source_ok:
        raise ValueError(source_error or f"source identity unavailable for {role}")
    candidate = Path(argv0) if Path(argv0).is_absolute() else Path(shutil.which(argv0) or argv0)
    resolved = candidate.resolve(strict=False)
    if str(resolved) != expected["resolved_path"]:
        raise ValueError(f"launched executable path diverges for {role}")
    source_fd: int | None = None
    image_fd: int | None = None
    try:
        source_fd = os.open(str(resolved), os.O_RDONLY | os.O_CLOEXEC | os.O_NOFOLLOW)
        before = os.fstat(source_fd)
        if not stat_is_regular_executable(before.st_mode):
            raise ValueError(f"launched executable is not a regular executable for {role}")
        source_size, source_hash = _hash_open_fd(source_fd)
        if source_hash != expected["sha256"] or source_size != expected.get("size", source_size):
            raise ValueError(f"launched executable drift for {role}")
        image_fd = _create_sealed_memfd(f"ng-elc-{role}")
        os.fchmod(image_fd, before.st_mode & 0o7777)
        os.lseek(source_fd, 0, os.SEEK_SET)
        copied = 0
        while True:
            chunk = os.read(source_fd, 1024 * 1024)
            if not chunk:
                break
            view = memoryview(chunk)
            while view:
                written = os.write(image_fd, view)
                if written <= 0:
                    raise OSError("short write while staging executable")
                copied += written
                view = view[written:]
        os.lseek(image_fd, 0, os.SEEK_SET)
        image_size, image_hash = _hash_open_fd(image_fd)
        after = os.fstat(source_fd)
        if (before.st_dev, before.st_ino, before.st_size, before.st_mtime_ns) != (after.st_dev, after.st_ino, after.st_size, after.st_mtime_ns):
            raise ValueError(f"executable source raced during immutable staging for {role}")
        if copied != source_size or image_size != source_size or image_hash != expected["sha256"]:
            raise ValueError(f"immutable executable image verification failed for {role}")
        seals = _seal_memfd(image_fd)
        proc_path = f"/proc/self/fd/{image_fd}"
        return {
            "source_path": str(resolved),
            "source_dev": before.st_dev,
            "source_ino": before.st_ino,
            "source_size": source_size,
            "source_sha256": source_hash,
            "representation": "sealed-memfd",
            "fd": image_fd,
            "proc_path": proc_path,
            "size": image_size,
            "sha256": image_hash,
            "seals": seals,
        }
    except (FileNotFoundError, OSError, ValueError) as exc:
        if image_fd is not None:
            try:
                os.close(image_fd)
            except OSError:
                pass
        raise ValueError(f"immutable executable unavailable for {role}: {exc}") from exc
    finally:
        if source_fd is not None:
            try:
                os.close(source_fd)
            except OSError:
                pass


def stat_is_regular_executable(mode: int) -> bool:
    return stat.S_ISREG(mode) and bool(mode & 0o111)


def local_executable_linkage(provenance: Mapping[str, Any], role: str, argv0: str, checked_path: str | None = None) -> tuple[bool, str | None]:
    """Compatibility probe; actual launches use the returned sealed image."""

    del checked_path  # retained only for the legacy diagnostic signature
    try:
        prepared = prepare_local_executable(provenance, role, argv0)
        try:
            os.close(prepared["fd"])
        except OSError:
            pass
        return True, None
    except (OSError, ValueError) as exc:
        return False, str(exc)


def local_preflight(config: Mapping[str, str], plan: Mapping[str, Any], variables: Mapping[str, str], workload: Mapping[str, Any] | None = None, provenance: Mapping[str, Any] | None = None) -> dict[str, Any]:
    errors: list[str] = []
    paths = {key: str(Path(config[key]).resolve()) for key in LOCAL_REQUIRED_ENV}
    for key in ("NG_LOCAL_REPO_PATH", "NG_LOCAL_BUILD_PATH", "NG_LOCAL_IO_PATH"):
        if not Path(paths[key]).is_dir():
            errors.append(f"{key} directory missing")
    if workload is not None and plan.get("workload") is not None:
        if not workload.get("repository_empty", False):
            errors.append("Repository directory is not empty")
        if workload.get("aliasing_rejected", False):
            errors.append("Source and Repository paths alias or nest")
        if not workload.get("verified", False) and not errors:
            errors.append(str(workload.get("reason") or "fresh workload is not verified"))
    roles = []
    executable_names: dict[str, tuple[str, str]] = {}
    for role in plan.get("roles", []):
        argv = expand_argv(role["command"], variables)
        executable = Path(argv[0]) if Path(argv[0]).is_absolute() else Path(shutil.which(argv[0]) or argv[0])
        resolved_executable = executable.resolve(strict=False)
        cwd = Path(expand_argv([role["cwd"]], variables)[0]) if role.get("cwd") else None
        item = {"name": role["name"], "executable": str(resolved_executable), "resolved_path": str(resolved_executable), "executable_exists": resolved_executable.is_file(), "executable_mode": bool(resolved_executable.is_file() and os.access(resolved_executable, os.X_OK)), "cwd": str(cwd) if cwd else None, "cwd_exists": bool(cwd is None or cwd.is_dir())}
        if item["executable_exists"] and item["executable_mode"]:
            try:
                item["executable_identity"] = file_identity(resolved_executable)
                identity = (item["executable_identity"]["path"], item["executable_identity"]["sha256"])
                basename = resolved_executable.name
                if basename in executable_names and executable_names[basename] != identity:
                    errors.append(f"executable basename/path collision for {basename}")
                executable_names[basename] = identity
            except (FileNotFoundError, OSError, ValueError) as exc:
                item["executable_identity_error"] = str(exc)
                errors.append(f"{role['name']} executable identity unavailable")
        roles.append(item)
        if not item["executable_exists"]:
            errors.append(f"{role['name']} executable missing")
        elif not item["executable_mode"]:
            errors.append(f"{role['name']} executable not executable")
        if not item["cwd_exists"]:
            errors.append(f"{role['name']} cwd missing")
        if provenance is not None:
            linked, linkage_error = local_executable_linkage(provenance, role["name"], str(resolved_executable), checked_path=str(resolved_executable))
            if not linked:
                errors.append(linkage_error or f"build linkage missing for {role['name']}")
    return {"ok": not errors, "errors": errors, "paths": paths, "roles": roles}


def local_ipc_details(kind: str) -> dict[str, dict[str, int]] | None:
    try:
        run = subprocess.run(["ipcs", "-m" if kind == "shm" else "-s", "-p"], capture_output=True, text=True, timeout=10, check=False)
    except (OSError, subprocess.SubprocessError):
        return None
    if run.returncode != 0:
        return None
    owner = pwd.getpwuid(os.getuid()).pw_name
    header_token = "shmid" if kind == "shm" else "semid"
    lines = run.stdout.splitlines()
    if not any(header_token in line.split() for line in lines):
        return None
    details: dict[str, dict[str, int]] = {}
    header_seen = False
    header_required = {header_token, "owner", "cpid", "lpid"}
    for line in lines:
        fields = line.split()
        if header_token in fields:
            if not header_required <= set(fields):
                return None
            header_seen = True
            continue
        if not header_seen or not fields:
            continue
        if not fields[0].isdigit():
            return None
        if len(fields) < 4 or not fields[2].isdigit() or not fields[3].isdigit():
            return None
        if fields[1] == owner:
            details[fields[0]] = {"creator_pid": int(fields[2])}
    if not header_seen:
        return None
    return details


def attribute_new_ipc(before: Mapping[str, set[str] | None], after: Mapping[str, set[str] | None], details: Mapping[str, Mapping[str, Mapping[str, int]] | None] | None, trial_pids: set[int]) -> dict[str, Any]:
    owned: dict[str, list[str]] = {kind: [] for kind in ("shm", "semaphores")}
    unattributed: dict[str, list[str]] = {kind: [] for kind in ("shm", "semaphores")}
    inventory_unavailable = (
        details is None
        or any((details.get(kind) if details else None) is None for kind in ("shm", "semaphores"))
        or any(before.get(kind) is None or after.get(kind) is None for kind in ("shm", "semaphores"))
    )
    for kind in ("shm", "semaphores"):
        for identifier in sorted((after.get(kind) or set()) - (before.get(kind) or set())):
            item = ((details or {}).get(kind) or {}).get(identifier)
            if item and item.get("creator_pid") in trial_pids:
                owned[kind].append(identifier)
            else:
                unattributed[kind].append(identifier)
    reason = "inventory-unavailable" if inventory_unavailable else ("unattributed-ipc" if any(unattributed.values()) else None)
    return {"ok": reason is None, "owned": owned, "unattributed": unattributed, "reason": reason}


def local_remove_new_ipc(before: Mapping[str, set[str] | None], trial_pids: set[int]) -> tuple[bool, dict[str, list[str]], str | None]:
    after = local_ipc_snapshot()
    new_ids = {kind: sorted((after.get(kind) or set()) - (before.get(kind) or set())) for kind in ("shm", "semaphores")}
    details = {kind: (local_ipc_details(kind) if new_ids[kind] else {}) for kind in ("shm", "semaphores")}
    attribution = attribute_new_ipc(before, after, details, trial_pids)
    if not attribution["ok"]:
        return False, new_ids, attribution["reason"]
    ok = True
    ipcrm_failed = False
    for kind, ids in attribution["owned"].items():
        flag = "-m" if kind == "shm" else "-s"
        for identifier in ids:
            try:
                run = subprocess.run(["ipcrm", flag, identifier], capture_output=True, text=True, timeout=10, check=False)
            except (OSError, subprocess.SubprocessError):
                ok = False
                ipcrm_failed = True
                continue
            if run.returncode != 0:
                ipcrm_failed = True
                ok = False
    remaining = local_ipc_snapshot()
    if any(remaining.get(kind) is None or before.get(kind) is None for kind in ("shm", "semaphores")):
        return False, new_ids, "inventory-unavailable"
    residual = any((remaining.get(kind) or set()) - (before.get(kind) or set()) for kind in ("shm", "semaphores"))
    if residual:
        return False, new_ids, "residual-ipc"
    if ipcrm_failed:
        return False, new_ids, "ipcrm-failed"
    return ok, new_ids, None


def local_stop_process_group(proc: subprocess.Popen[Any], pgid: int, expected_starttime: str | None, expected_executable: str | None = None) -> dict[str, Any]:
    if pgid is None:
        return {"ok": False, "result": "identity-unavailable", "pid": proc.pid, "residual": [{"pid": proc.pid, "reason": "process-group-identity-missing"}]}
    if proc.poll() is not None:
        group_scan = local_group_members_status(pgid)
        if not group_scan["complete"]:
            return {"ok": False, "result": "group-inventory-unavailable", "pid": proc.pid, "pgid": pgid, "residual": [{"pid": proc.pid, "pgid": pgid, "reason": "identity-unavailable"}]}
        if group_scan["members"]:
            return {"ok": False, "result": "group-member-residual", "pid": proc.pid, "pgid": pgid, "returncode": proc.returncode, "residual": group_scan["members"]}
        return {"ok": True, "result": "already-exited", "returncode": proc.returncode}
    record = _safe_read_proc_stat(proc.pid)
    if record["status"] != "ok" or record["starttime"] != expected_starttime or record["pgid"] != pgid:
        return {"ok": False, "result": "identity-mismatch", "pid": proc.pid, "pgid": pgid}
    if expected_executable is not None:
        executable = _safe_executable_identity(proc.pid)
        if executable.get("status") != "ok" or executable.get("path") != expected_executable:
            return {"ok": False, "result": "executable-identity-mismatch", "pid": proc.pid, "pgid": pgid, "residual": [{"pid": proc.pid, "reason": "executable-identity-mismatch"}]}
    try:
        os.killpg(pgid, signal.SIGTERM)
    except (OSError, ProcessLookupError) as exc:
        return {"ok": False, "result": "term-error", "error": str(exc)}
    try:
        proc.wait(timeout=10)
        group_scan = local_group_members_status(pgid)
        if not group_scan["complete"]:
            return {"ok": False, "result": "group-inventory-unavailable", "pid": proc.pid, "pgid": pgid, "residual": [{"pid": proc.pid, "pgid": pgid, "reason": "identity-unavailable"}]}
        if group_scan["members"]:
            return {"ok": False, "result": "group-member-residual", "pid": proc.pid, "pgid": pgid, "returncode": proc.returncode, "residual": group_scan["members"]}
        return {"ok": True, "result": "SIGTERM", "returncode": proc.returncode}
    except subprocess.TimeoutExpired:
        record = _safe_read_proc_stat(proc.pid)
        if record["status"] != "ok" or record["starttime"] != expected_starttime or record["pgid"] != pgid:
            return {"ok": False, "result": "identity-mismatch-before-kill", "pid": proc.pid, "pgid": pgid}
        if expected_executable is not None:
            executable = _safe_executable_identity(proc.pid)
            if executable.get("status") != "ok" or executable.get("path") != expected_executable:
                return {"ok": False, "result": "executable-identity-mismatch-before-kill", "pid": proc.pid, "pgid": pgid}
        try:
            os.killpg(pgid, signal.SIGKILL)
        except (OSError, ProcessLookupError) as exc:
            return {"ok": False, "result": "kill-error", "error": str(exc)}
        try:
            proc.wait(timeout=5)
            group_scan = local_group_members_status(pgid)
            if not group_scan["complete"]:
                return {"ok": False, "result": "group-inventory-unavailable", "pid": proc.pid, "pgid": pgid, "residual": [{"pid": proc.pid, "pgid": pgid, "reason": "identity-unavailable"}]}
            if group_scan["members"]:
                return {"ok": False, "result": "group-member-residual", "pid": proc.pid, "pgid": pgid, "returncode": proc.returncode, "residual": group_scan["members"]}
            return {"ok": True, "result": "SIGKILL", "returncode": proc.returncode}
        except subprocess.TimeoutExpired:
            return {"ok": False, "result": "kill-timeout", "pid": proc.pid, "pgid": pgid}


def register_local_process(
    processes: dict[str, dict[str, Any]],
    role: str,
    proc: subprocess.Popen[Any],
    argv: list[str],
    stdout: Any,
    stderr: Any,
    stdout_path: Path,
    stderr_path: Path,
    launched_identity: Mapping[str, Any],
) -> dict[str, Any]:
    """Register a child before querying any fallible process identity."""

    item: dict[str, Any] = {
        "process": proc,
        "argv": argv,
        "pgid": None,
        "starttime": None,
        "tracked_descendants": {},
        "descendant_scan_seen": False,
        "descendant_scan_complete": True,
        "stdout": stdout,
        "stderr": stderr,
        "stdout_path": stdout_path,
        "stderr_path": stderr_path,
        "launched_identity": dict(launched_identity),
    }
    processes[role] = item
    try:
        item["pgid"] = os.getpgid(proc.pid)
        item["starttime"] = process_starttime(proc.pid)
        if item["starttime"] is None:
            raise RuntimeError("process starttime identity unavailable")
        executable = _safe_executable_identity(proc.pid)
        if executable.get("status") != "ok":
            raise RuntimeError("process executable identity unavailable")
        item["executable"] = executable["path"]
    except BaseException as exc:
        # Keep the ledger entry while rollback checks complete.  Never use a
        # Popen handle alone as signalling authority when PID/starttime/PGID
        # or executable identity is unavailable.
        item["identity_failure"] = str(exc)
        item["rollback_uncertain"] = True
        item["rollback_started"] = True
        try:
            scan = local_process_descendants(proc.pid)
        except BaseException as scan_exc:
            scan = {"descendants": [], "complete": False, "error": str(scan_exc)}
        item["descendant_scan_seen"] = True
        item["descendant_scan_complete"] = scan.get("complete", False)
        item["tracked_descendants"].update({child["pid"]: child for child in scan.get("descendants", [])})
        rollback_identity = _safe_read_proc_stat(proc.pid)
        rollback_executable = _safe_executable_identity(proc.pid)
        identity_ok = (
            item.get("pgid") is not None
            and item.get("starttime") is not None
            and rollback_identity.get("status") == "ok"
            and rollback_identity.get("starttime") == item.get("starttime")
            and rollback_identity.get("pgid") == item.get("pgid")
            and rollback_executable.get("status") == "ok"
        )
        kill_ok = False
        wait_ok = False
        if identity_ok:
            try:
                proc.kill()
                kill_ok = True
            except (OSError, ProcessLookupError, subprocess.SubprocessError) as rollback_exc:
                item["rollback_kill_error"] = str(rollback_exc)
            try:
                proc.wait(timeout=5)
                wait_ok = True
            except (OSError, subprocess.SubprocessError) as rollback_exc:
                item["rollback_wait_error"] = str(rollback_exc)
        else:
            item["rollback_identity_error"] = "PID/starttime/PGID/executable identity could not be revalidated"
        try:
            after_scan = local_process_descendants(proc.pid)
        except BaseException as scan_exc:
            after_scan = {"descendants": [], "complete": False, "error": str(scan_exc)}
        item["descendant_scan_complete"] = item.get("descendant_scan_complete", False) and after_scan.get("complete", False)
        item["tracked_descendants"].update({child["pid"]: child for child in after_scan.get("descendants", [])})
        try:
            descendants_ok, residual = local_stop_tracked_descendants(item["tracked_descendants"])
        except BaseException as stop_exc:
            descendants_ok, residual = False, [{"reason": "rollback-descendant-stop-error", "error": str(stop_exc)}]
        item["rollback_descendants_ok"] = descendants_ok
        item["rollback_descendant_residual"] = residual
        item["rollback_complete"] = bool(kill_ok and wait_ok and item["descendant_scan_complete"] and descendants_ok)
        item["unresolved"] = not item["rollback_complete"]
        try:
            stdout.close()
            stderr.close()
        except OSError:
            pass
        raise RuntimeError(f"process identity registration failed for {role}: {exc}") from exc
    return item


def run_local_trial(args: argparse.Namespace) -> int:
    global _LOCAL_SECRET_VALUES
    config = load_local_config(getattr(args, "env", None))
    plan, contract = load_plan_for_trial(Path(args.plan), args.scenario, args.debug_profile, mode="local")
    profile = validate_local_profile(plan, getattr(args, "local_profile", None), mode="local")
    trial_id = validate_trial_id(args.trial or utc_id())
    evidence_root = Path(config["NG_LOCAL_EVIDENCE_PATH"]).resolve()
    evidence_dir = evidence_root / trial_id
    if evidence_dir.exists():
        raise ConfigError(f"local evidence directory already exists: {evidence_dir}")
    evidence_dir.mkdir(parents=True)
    events = evidence_dir / "controller-events.jsonl"
    local_record(events, "trial-start", trial_id=trial_id, scenario=args.scenario, debug_profile=args.debug_profile, contract_level=contract["level"], mode="local", local_profile=profile["name"], effective_uid=profile["euid"])
    variables = dict(config)
    variables["TRIAL_ID"] = trial_id
    _LOCAL_SECRET_VALUES = collect_secret_values({"plan": _expand_value_for_secrets(plan, variables), "config": variables})
    _LOCAL_SECRET_VALUES.update(collect_secret_values(os.environ))
    for declared_role in plan.get("roles", []):
        if isinstance(declared_role, Mapping):
            _LOCAL_SECRET_VALUES.update(collect_secret_values(_expand_value_for_secrets(declared_role.get("env", {}), variables)))
    overall_deadline = time.monotonic() + float(plan["timeouts"]["total"])
    local_record(events, "prepare", mode="local", evidence_dir=str(evidence_dir))
    workload = local_prepare_v2_layout(evidence_dir, plan, contract, config, variables)
    ownership_anchor = local_ownership_anchor(trial_id)
    local_record(events, "ownership-anchor", anchor=ownership_anchor)
    effective_config = dict(config)
    if workload.get("staging_io_path"):
        effective_config["NG_LOCAL_IO_PATH"] = str(workload["staging_io_path"])
    provenance = local_provenance(
        repo=effective_config["NG_LOCAL_REPO_PATH"],
        controller=Path(__file__).resolve(),
        helpers={
            "evidence_verifier": Path(__file__).with_name("evidence_verifier.py"),
            "local_provenance": Path(__file__).with_name("local_provenance.py"),
            "ng_observability": Path(__file__).with_name("ng_observability.py"),
        },
        plan=plan,
        variables=variables,
        config=effective_config,
        evidence_dir=evidence_dir,
        contract=contract,
    )
    provenance["workload_verified"] = bool(workload["verified"])
    provenance["workload_error"] = workload.get("reason")
    provenance["evidence_schema_v2"] = True
    provenance["local_profile"] = profile["name"]
    provenance["local_profile_selection"] = dict(profile)
    provenance["effective_uid"] = profile["euid"]
    provenance["ownership_anchor"] = ownership_anchor
    if not ownership_anchor.get("verified"):
        provenance.setdefault("protected_input_blockers", []).append("ownership anchor could not be verified")
    eligibility = local_acceptance_eligibility(provenance)
    provenance["acceptance_eligibility"] = eligibility
    local_json_write(evidence_dir / "provenance.json", provenance)
    local_record(events, "provenance-written", provenance_path=str(evidence_dir / "provenance.json"), acceptance_eligibility=eligibility)
    processes: dict[str, dict[str, Any]] = {}
    before_ipc = local_ipc_snapshot()
    local_json_write(evidence_dir / "inventory" / "baseline.json", {"schema_version": 2, "processes": [], "ipc": {kind: (sorted(value) if isinstance(value, set) else None) for kind, value in before_ipc.items()}})
    runtime = "PASS"
    teardown_result = "PASS" if ownership_anchor.get("verified") else "UNKNOWN"
    evidence_result = "COMPLETE"
    interrupted = False
    offsets: dict[str, int] = {}
    carries: dict[str, str] = {}
    oracle = plan.get("runtime_oracle")
    effective_oracle = dict(oracle) if isinstance(oracle, Mapping) else oracle
    if isinstance(effective_oracle, dict) and effective_oracle.get("type") == "files" and workload.get("expected_map"):
        if workload.get("staging_io_path"):
            effective_oracle["source"] = str(Path(workload["staging_io_path"]) / "Source1")
            effective_oracle["repository"] = str(Path(workload["staging_io_path"]) / "Repository1")
        effective_oracle["expected_map"] = dict(workload["expected_map"])
    oracle_snapshot: dict[str, Any] = {"schema_version": 2, "type": (effective_oracle or {}).get("type"), "preservation_verified": False}
    protected_input_blockers: list[str] = list(provenance.get("protected_input_blockers", []))
    try:
        preflight = local_preflight(effective_config, plan, variables, workload, provenance)
        local_record(events, "preflight", result=preflight)
        if not preflight["ok"]:
            raise ConfigError("local preflight rejected: " + ", ".join(preflight["errors"]))
        if profile["name"] == "native-privileged":
            cleanup = run_native_cleanup(Path(effective_config["NG_LOCAL_REPO_PATH"]), evidence_dir, euid=profile["euid"])
            local_record(events, "native-cleanup", result=cleanup)
            if not cleanup["ok"]:
                raise ConfigError("native privileged cleanup did not establish a zero baseline")
            before_ipc = local_ipc_snapshot()
            local_json_write(evidence_dir / "inventory" / "baseline.json", {"schema_version": 2, "processes": [], "ipc": {kind: (sorted(value) if isinstance(value, set) else None) for kind, value in before_ipc.items()}})
        if not ownership_anchor.get("verified"):
            raise ConfigError("local ownership anchor is not verified; refusing to launch")
        for role in plan["roles"]:
            argv = expand_argv(role["command"], variables)
            checked_role = next(item for item in preflight["roles"] if item["name"] == role["name"])
            checked_executable = checked_role["resolved_path"]
            argv[0] = checked_executable
            cwd = expand_argv([role["cwd"]], variables)[0] if role.get("cwd") else None
            if cwd and not Path(cwd).is_dir():
                raise ConfigError(f"local cwd does not exist for {role['name']}: {cwd}")
            role_dir = evidence_dir / "roles" / role["name"]
            role_dir.mkdir(parents=True, exist_ok=True)
            stdout_path = role_dir / "stdout.log"
            stderr_path = role_dir / "stderr.log"
            stdout: Any = None
            stderr: Any = None
            pty_launch: dict[str, Any] | None = None
            if profile["name"] != "native-privileged":
                stdout = stdout_path.open("w", encoding="utf-8")
                stderr = stderr_path.open("w", encoding="utf-8")
            declared_role_env = {key: expand_argv([value], variables)[0] for key, value in role.get("env", {}).items()}
            role_env = native_safe_environment(declared_role_env) if profile["name"] == "native-privileged" else {**os.environ, **declared_role_env}
            role_env.update({"NG_ELC_TRIAL_ID": trial_id, "NG_ELC_TRIAL_ROLE": role["name"]})
            # The immutable image, not the checked pathname, is the launch
            # target. Its inherited fd is closed in the parent only after the
            # Popen fork/exec handoff.
            launch_identity: dict[str, Any] | None = None
            try:
                launch_identity = prepare_local_executable(provenance, role["name"], checked_executable)
                argv[0] = launch_identity["proc_path"]
                if profile["name"] == "native-privileged":
                    pty_launch = launch_local_role_pty(role["name"], argv, cwd=cwd, env=role_env, stdout_path=stdout_path, stderr_path=stderr_path, pass_fds=(launch_identity["fd"],), max_bytes=int(plan.get("log_quota_bytes", 64 * 1024 * 1024)))
                    proc = pty_launch["process"]
                    stdout = pty_launch["stdout"]
                    stderr = pty_launch["stderr"]
                else:
                    proc = subprocess.Popen(argv, cwd=cwd, env=role_env, stdout=stdout, stderr=stderr, start_new_session=True, text=True, pass_fds=(launch_identity["fd"],))
            except Exception:
                if launch_identity is not None and launch_identity.get("fd") is not None:
                    try:
                        os.close(launch_identity["fd"])
                    except OSError:
                        pass
                if pty_launch is not None:
                    pty_launch["close"]()
                else:
                    if stdout is not None:
                        stdout.close()
                    if stderr is not None:
                        stderr.close()
                raise
            try:
                os.close(launch_identity["fd"])
            except OSError:
                pass
            registered = register_local_process(processes, role["name"], proc, argv, stdout, stderr, stdout_path, stderr_path, launch_identity)
            registered["launched_identity"]["child_pid"] = proc.pid
            launch_public = sanitize_config({"role": role["name"], "pid": proc.pid, "pgid": registered["pgid"], "starttime": registered["starttime"], "executable": registered["launched_identity"]["proc_path"], "argv": argv, "cwd": cwd, "launched_identity": registered["launched_identity"]})
            local_record(events, "launch", **launch_public)
            local_json_write(role_dir / "launch.json", launch_public)
            expected = role.get("readiness", [])
            deadline = min(time.monotonic() + float(plan["timeouts"]["readiness"]), overall_deadline)
            seen: set[str] = set()
            while time.monotonic() < deadline and set(item["id"] for item in expected) - seen:
                dead_roles = [name for name, item in processes.items() if item["process"].poll() is not None]
                if dead_roles:
                    runtime = "INCONCLUSIVE"
                    local_record(events, "readiness-process-exit", role=role["name"], dead_roles=dead_roles)
                    break
                observed = local_observe(processes, offsets, carries, expected, int(plan.get("log_quota_bytes", 64 * 1024 * 1024)))
                if observed.get("overflow"):
                    runtime = "INCONCLUSIVE"
                    local_record(events, "readiness-log-overflow", role=role["name"])
                    break
                for match in observed["matches"]:
                    if match["role"] == role["name"]:
                        seen.add(match["id"])
                        local_record(events, "readiness-marker", marker=match)
                if proc.poll() is not None and set(item["id"] for item in expected) - seen:
                    break
                time.sleep(0.2)
            readiness_result = "OBSERVED" if set(item["id"] for item in expected) <= seen else "INCONCLUSIVE"
            local_record(events, "readiness", role=role["name"], result=readiness_result, markers=sorted(seen), process_returncode=proc.poll())
            if readiness_result != "OBSERVED":
                runtime = "INCONCLUSIVE"
                break

        if not processes:
            runtime = "INCONCLUSIVE"
            local_record(events, "no-roles-launched", reason="preflight completed without launchable roles")

        if runtime == "PASS":
            oracle = effective_oracle
            deadline = min(time.monotonic() + float(plan["timeouts"]["observation"]), overall_deadline)
            file_result = None
            oracle_verdict = "INCONCLUSIVE"
            marker_seen: set[tuple[str, str]] = set()
            while time.monotonic() < deadline:
                dead_roles = [name for name, item in processes.items() if item["process"].poll() is not None]
                if dead_roles:
                    runtime = "FAIL"
                    oracle_verdict = latch_oracle_result(oracle_verdict, "FAIL")
                    local_record(events, "observation-process-exit", dead_roles=dead_roles)
                    break
                if oracle and oracle.get("type") == "files":
                    file_result = local_file_oracle(oracle, variables)
                    oracle_verdict = latch_oracle_result(oracle_verdict, file_result["result"])
                    local_record(events, "file-oracle", result=file_result)
                    if oracle_verdict == "FAIL" or file_result["result"] == "PASS":
                        break
                elif oracle and oracle.get("type") == "markers":
                    observed = local_observe(processes, offsets, carries, oracle["required"], int(plan.get("log_quota_bytes", 64 * 1024 * 1024)))
                    if observed.get("overflow"):
                        runtime = "INCONCLUSIVE"
                        oracle_verdict = latch_oracle_result(oracle_verdict, "INCONCLUSIVE")
                        local_record(events, "observation-log-overflow")
                        break
                    for match in observed["matches"]:
                        marker_seen.add((match["role"], match["id"]))
                        local_record(events, "runtime-marker", marker=match)
                    if marker_requirements_satisfied(marker_seen, oracle["required"]):
                        oracle_verdict = "PASS"
                        break
                time.sleep(min(0.2, max(0.0, deadline - time.monotonic())))
            if oracle and oracle.get("type") == "files":
                runtime = latch_oracle_result(runtime, oracle_verdict)
            elif plan.get("diagnostic_only", False):
                runtime = "INCONCLUSIVE"
            elif oracle and oracle.get("type") == "markers":
                runtime = marker_runtime_result(runtime, marker_seen, oracle["required"])
    except KeyboardInterrupt:
        interrupted = True
        runtime = "ABORTED"
        local_record(events, "interrupted")
    except Exception as exc:
        runtime = preserve_runtime_failure(runtime)
        local_record(events, "runtime-error", error=str(exc))
    finally:
        for role in reversed(list(processes)):
            item = processes[role]
            proc = item["process"]
            pgid = item["pgid"]
            if item.get("rollback_uncertain"):
                teardown_result = "FAIL"
                local_record(events, "identity-rollback", role=role, rollback_complete=item.get("rollback_complete", False), unresolved=item.get("unresolved", True), residual=item.get("rollback_descendant_residual", []))
            # Capture ownership while the leader is present, then drain with
            # several bounded post-signal scans.  The ledger is explicit so a
            # late/reparented child or any uncertain scan prevents teardown PASS.
            try:
                initial_scan = local_process_descendants(proc.pid)
            except BaseException as exc:
                initial_scan = {"descendants": [], "complete": False, "error": str(exc)}
            item["descendant_scan_seen"] = True
            item["descendant_scan_complete"] = item.get("descendant_scan_complete", True) and initial_scan.get("complete", False)
            item.setdefault("tracked_descendants", {}).update({descendant["pid"]: descendant for descendant in initial_scan.get("descendants", [])})
            if initial_scan.get("error"):
                item["descendant_scan_error"] = initial_scan["error"]
            stop_result = local_stop_process_group(proc, pgid, item["starttime"], item.get("executable"))
            if not stop_result["ok"]:
                teardown_result = "FAIL"
            local_record(events, "stop", role=role, **stop_result)
            reconciliation = local_descendant_reconciliation(proc.pid, item.get("tracked_descendants", {}), pgid, scans=4, owner_pid=ownership_anchor["pid"], trial_id=trial_id, role=role)
            item["descendant_rollback_ledger"] = reconciliation
            item["descendant_scan_complete"] = item.get("descendant_scan_complete", True) and not reconciliation["unknown"]
            item["tracked_descendants"] = {child["pid"]: child for child in reconciliation["tracked"] if isinstance(child.get("pid"), int)}
            descendants_ok = reconciliation["ok"]
            descendant_residual = reconciliation["residual"]
            if not item.get("descendant_scan_complete", True) or not descendants_ok:
                teardown_result = "FAIL"
                local_record(
                    events,
                    "descendant-cleanup",
                    role=role,
                    complete=item.get("descendant_scan_complete", True),
                    residual=descendant_residual,
                    unknown=reconciliation["unknown"],
                    scan_count=reconciliation["scan_count"],
                )
            group_scan = local_group_members_status(pgid)
            members = group_scan["members"]
            if not group_scan["complete"]:
                teardown_result = "FAIL"
                local_record(events, "process-group-inventory-unavailable", role=role, pgid=pgid, reason="identity-unavailable")
            if members:
                teardown_result = "FAIL"
                member_tracked = {member["pid"]: member for member in members}
                members_ok, member_residual = local_stop_tracked_descendants(member_tracked)
                local_record(
                    events,
                    "process-group-cleanup",
                    role=role,
                    pgid=pgid,
                    members=members,
                    ok=members_ok,
                    residual=member_residual or [{**member, "reason": "surviving-process-group-member"} for member in members],
                )
                final_group_scan = local_group_members_status(pgid)
                if not final_group_scan["complete"]:
                    teardown_result = "FAIL"
                    local_record(events, "process-group-inventory-unavailable", role=role, pgid=pgid, reason="identity-unavailable-after-cleanup")
                if final_group_scan["members"]:
                    teardown_result = "FAIL"
                    local_record(events, "process-group-residual", role=role, pgid=pgid, members=final_group_scan["members"], reason="surviving-process-group-member")
                members = final_group_scan["members"]
            item["unresolved"] = bool(
                item.get("rollback_uncertain")
                or not stop_result.get("ok", False)
                or not item.get("descendant_scan_complete", False)
                or not descendants_ok
                or not group_scan.get("complete", False)
                or bool(members)
            )
            if item["unresolved"]:
                teardown_result = "FAIL"
                local_record(events, "ownership-unresolved", role=role, reason="descendant-or-process-group-ownership-not-proven")
            item["group_members_after_stop"] = members
            item["process"] = proc
            local_json_write(evidence_dir / "roles" / role / "exit.json", {"role": role, "pid": proc.pid, "returncode": proc.returncode, "group_members_after_stop": members, "tracked_descendants": sorted(item.get("tracked_descendants", {}).values(), key=lambda child: child.get("pid", 0)), "descendant_rollback_ledger": item.get("descendant_rollback_ledger", {}), "rollback_uncertain": item.get("rollback_uncertain", False), "unresolved": item.get("unresolved", False)})
            item["stdout"].close()
            item["stderr"].close()
            capture_error = getattr(item["stdout"], "error", None)
            if capture_error:
                runtime = preserve_runtime_failure(runtime)
                evidence_result = "INCOMPLETE"
                teardown_result = "FAIL"
                local_record(events, "pty-capture-error", role=role, error=capture_error)
        oracle = effective_oracle
        if oracle and oracle.get("type") == "files":
            try:
                oracle_snapshot = preserve_local_artifacts(oracle, variables, evidence_dir)
                local_json_write(evidence_dir / "oracle.json", oracle_snapshot)
                local_record(events, "evidence-preserved", oracle_path=str(evidence_dir / "oracle.json"), preservation_verified=oracle_snapshot["preservation_verified"])
                if not oracle_snapshot["preservation_verified"]:
                    evidence_result = "INCOMPLETE"
            except Exception as exc:
                evidence_result = "INCOMPLETE"
                local_record(events, "evidence-preservation-error", error=str(exc))
        else:
            (evidence_dir / "artifacts" / "source").mkdir(parents=True, exist_ok=True)
            (evidence_dir / "artifacts" / "repository").mkdir(parents=True, exist_ok=True)
            local_json_write(evidence_dir / "artifacts" / "source" / "_not-captured.json", {"schema_version": 2, "captured": False, "reason": "no file oracle"})
            local_json_write(evidence_dir / "artifacts" / "repository" / "_not-captured.json", {"schema_version": 2, "captured": False, "reason": "no file oracle"})
        if not processes:
            local_json_write(evidence_dir / "roles" / "_not-launched.json", {"schema_version": 2, "launched": False, "reason": "preflight or readiness prevented launch"})
        local_json_write(evidence_dir / "oracle.json", oracle_snapshot)
        local_json_write(evidence_dir / "preservation.json", {"schema_version": 2, "verified": bool(oracle_snapshot.get("preservation_verified")), "before_temporary_cleanup": True, "oracle": "oracle.json"})
        local_json_write(evidence_dir / "inventory" / "pre-cleanup.json", {"schema_version": 2, "processes": local_process_snapshot(processes), "ipc": {"baseline": {kind: (sorted(value) if isinstance(value, set) else None) for kind, value in before_ipc.items()}}})
        ipc_ok, new_ipc, ipc_reason = local_remove_new_ipc(before_ipc, {item["process"].pid for item in processes.values()})
        local_record(events, "inventory", processes=local_process_snapshot(processes), ipc_new_ids=new_ipc, ipc_cleanup=ipc_ok, ipc_cleanup_reason=ipc_reason)
        if not ipc_ok:
            teardown_result = "FAIL"
        safety = sanitize_evidence_tree(evidence_dir, _LOCAL_SECRET_VALUES)
        if safety["blockers"]:
            protected_input_blockers.extend(safety["blockers"])
            evidence_result = "INCOMPLETE"
            eligibility["eligible"] = False
            for blocker in safety["blockers"]:
                if blocker not in eligibility["blockers"]:
                    eligibility["blockers"].append(blocker)
            local_record(events, "protected-input-blocker", blockers=safety["blockers"], quarantined=safety["quarantined"])
        local_json_write(evidence_dir / "ownership.json", {"schema_version": 2, "method": "linux-subreaper-anchor", "anchor": ownership_anchor, "trial_pids": sorted(item["process"].pid for item in processes.values()), "ipc_new_ids": new_ipc, "attribution": "fail-closed" if ipc_reason else "creator-pid", "ownership_proven": all(item.get("unresolved") is False for item in processes.values()) and bool(ownership_anchor.get("verified"))})
        local_json_write(evidence_dir / "cleanup.json", {"schema_version": 2, "ordered_actions": ["stop", "evidence-preserve", "ipc-cleanup", "final-inventory"], "ipc_cleanup": ipc_ok, "ipc_reason": ipc_reason, "result": teardown_result})
        local_json_write(evidence_dir / "inventory" / "final.json", {"schema_version": 2, "processes": local_process_snapshot(processes), "ipc": {"new_ids": new_ipc, "cleanup_ok": ipc_ok, "cleanup_reason": ipc_reason}})
        if teardown_result == "PASS":
            local_record(events, "teardown", result="PASS")
        else:
            local_record(events, "teardown", result=teardown_result)
    code = 130 if interrupted and teardown_result == "PASS" and evidence_result == "COMPLETE" else classify_result(runtime, teardown_result, evidence_result)
    if not eligibility["eligible"] and code == 0:
        code = 21
    result = {"schema_version": 2, "trial_id": trial_id, "scenario": args.scenario, "debug_profile": args.debug_profile, "mode": "local", "local_profile": profile["name"], "local_profile_selection": dict(profile), "effective_uid": profile["euid"], "runtime_result": runtime, "teardown_result": teardown_result, "evidence_result": evidence_result, "local_acceptance_eligible": eligibility["eligible"], "acceptance_blockers": eligibility["blockers"], "protected_input_blockers": protected_input_blockers, "exit_code": code}
    result_path = evidence_dir / "result.json"
    local_json_write(result_path, result)
    try:
        # Seal once before invoking the offline verifier, then reseal after its
        # stable verdict is recorded. No evidence is mutated after the final seal.
        local_record(events, "bundle-seal", phase="pre-verification")
        write_evidence_manifest(evidence_dir, {"trial_id": trial_id, "scenario": args.scenario, "debug_profile": args.debug_profile, "mode": "local", "runtime_result": runtime, "teardown_result": teardown_result})
        try:
            verification = verify_bundle(evidence_dir)
        except Exception as exc:
            verification = {"code": "NGELC-VERIFIER-ERROR", "integrity": False, "accepted": False, "errors": [str(exc)]}
        result, code, evidence_result = apply_local_verification(result, eligibility, verification, runtime, teardown_result, evidence_result)
        local_json_write(evidence_dir / "offline-verification.json", result["offline_verification"])
        local_json_write(result_path, result)
        for _attempt in range(2):
            local_record(events, "offline-verification", report=result["offline_verification"])
            local_record(events, "bundle-seal", phase="final-verification")
            write_evidence_manifest(evidence_dir, {"trial_id": trial_id, "scenario": args.scenario, "debug_profile": args.debug_profile, "mode": "local", "runtime_result": runtime, "teardown_result": teardown_result})
            try:
                final_verification = verify_bundle(evidence_dir)
            except Exception as exc:
                final_verification = {"code": "NGELC-VERIFIER-ERROR", "integrity": False, "accepted": False, "errors": [str(exc)]}
            expected_accepted = result["offline_verification"].get("accepted", False)
            final_summary = local_verification_summary(final_verification)
            if final_verification.get("code") == CODE_VALID and final_verification.get("accepted", False) == expected_accepted:
                break
            if final_summary == result["offline_verification"]:
                # The failed verdict is already persisted in this sealed bundle.
                break
            result, code, evidence_result = apply_local_verification(result, eligibility, final_verification, runtime, teardown_result, evidence_result)
            local_json_write(evidence_dir / "offline-verification.json", result["offline_verification"])
            local_json_write(result_path, result)
    except OSError as exc:
        evidence_result = "INCOMPLETE"
        teardown_result = "UNKNOWN"
        code = classify_result(runtime, teardown_result, evidence_result)
        result.update({"teardown_result": teardown_result, "evidence_result": evidence_result, "exit_code": code})
        local_json_write(result_path, result)
        local_record(events, "manifest-error", error=str(exc))
        try:
            write_evidence_manifest(evidence_dir, {"trial_id": trial_id, "scenario": args.scenario, "debug_profile": args.debug_profile, "mode": "local", "runtime_result": runtime, "teardown_result": teardown_result})
        except OSError:
            pass
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return code


def teardown(config: Mapping[str, str], plan: dict[str, Any], helper: str, state_dirs: dict[str, str], trial_id: str, evidence_dir: Path, prepared_hosts: set[str]) -> tuple[str, str]:
    teardown_result = "PASS"
    host_results = {host: "PASS" for host in prepared_hosts}
    for role in reversed(plan.get("roles", [])):
        _vm, host = host_for(config, role)
        if host not in prepared_hosts:
            continue
        try:
            result = remote_call(config, host, helper, "stop", state_dirs[host], trial_id, {"role": role["name"], "grace": 10})
            item = result.get("results", [{}])[0]
            if item.get("result") not in {"STOPPED", "NOT_FOUND"} or "SIGKILL" in item.get("signals", []):
                teardown_result = "FAIL"
                host_results[host] = "FAIL"
        except Exception as exc:
            teardown_result = "UNKNOWN"
            host_results[host] = "UNKNOWN"
            local_record(evidence_dir / "controller-events.jsonl", "teardown-error", role=role["name"], error=str(exc))
    for host in prepared_hosts:
        state_dir = state_dirs[host]
        label = "source" if host == config["SOURCE_VM_IP"] else "repository"
        try:
            remote_call(config, host, helper, "seal", state_dir, trial_id)
            final_inventory = None
            deadline = time.monotonic() + 10.0
            while time.monotonic() < deadline:
                final_inventory = remote_call(config, host, helper, "inventory", state_dir, trial_id)
                live = any(item.get("identity_matches") for item in final_inventory.get("processes", []))
                new_ipc = any(final_inventory.get("ipc_new_ids", {}).get(kind) for kind in ("shm", "semaphores"))
                ipc_ok = all(final_inventory.get("ipc", {}).get(kind, {}).get("available") and final_inventory.get("ipc", {}).get(kind, {}).get("returncode") == 0 for kind in ("shm", "semaphores"))
                if not live and not new_ipc and ipc_ok:
                    break
                time.sleep(0.25)
            if not final_inventory:
                raise RuntimeError("final inventory missing")
            live = any(item.get("identity_matches") for item in final_inventory.get("processes", []))
            new_ipc = any(final_inventory.get("ipc_new_ids", {}).get(kind) for kind in ("shm", "semaphores"))
            ipc_ok = all(final_inventory.get("ipc", {}).get(kind, {}).get("available") and final_inventory.get("ipc", {}).get(kind, {}).get("returncode") == 0 for kind in ("shm", "semaphores"))
            if live or new_ipc or not ipc_ok:
                host_results[host] = "FAIL" if ipc_ok else "UNKNOWN"
                teardown_result = host_results[host] if teardown_result == "PASS" else teardown_result
            collect_remote(config, host, state_dir, evidence_dir / label)
            if host_results[host] == "PASS":
                remote_call(config, host, helper, "release", state_dir, trial_id)
        except Exception as exc:
            teardown_result = "UNKNOWN"
            host_results[host] = "UNKNOWN"
            local_record(evidence_dir / "controller-events.jsonl", "collection-error", host=host, error=str(exc))
    return teardown_result, "COMPLETE" if teardown_result == "PASS" else "INCOMPLETE"


def run_trial(args: argparse.Namespace) -> int:
    config = load_config_from_env()
    plan, contract = load_plan_for_trial(Path(args.plan), args.scenario, args.debug_profile)
    validate_local_profile(plan, getattr(args, "local_profile", None), mode="remote")
    trial_id = args.trial or utc_id()
    state_root, _ = trial_paths(config, trial_id)
    evidence_dir = Path(config["NG_EVIDENCE_PATH"]).resolve() / trial_id
    if evidence_dir.exists():
        raise ConfigError(f"local evidence directory already exists: {evidence_dir}")
    evidence_dir.mkdir(parents=True)
    local_record(evidence_dir / "controller-events.jsonl", "trial-start", trial_id=trial_id, scenario=args.scenario, debug_profile=args.debug_profile, contract_level=contract["level"])
    helper_local = Path(args.helper_local).resolve()
    wrapper_local = helper_local.with_name("ng_role_wrapper.py")
    if not helper_local.is_file() or not wrapper_local.is_file():
        raise ConfigError("helper and role wrapper must both exist locally")
    hosts = [config["SOURCE_VM_IP"], config["REPO_VM_IP"]]
    state_dirs = {host: state_root for host in hosts}
    tool_dir = f"{config['NG_REMOTE_EVIDENCE_PATH'].rstrip('/')}/.ng-tools"
    helper_remote = f"{tool_dir}/ng_trial_helper.py"
    wrapper_remote = f"{tool_dir}/ng_role_wrapper.py"
    prepared_hosts: set[str] = set()
    runtime = "PASS"
    interrupted = False
    lease_sequences: dict[str, int] = {}
    old_int = signal.getsignal(signal.SIGINT)
    old_term = signal.getsignal(signal.SIGTERM)
    def abort_signal(_signum: int, _frame: Any) -> None:
        raise KeyboardInterrupt()
    signal.signal(signal.SIGINT, abort_signal)
    signal.signal(signal.SIGTERM, abort_signal)
    try:
        if plan.get("require_fresh_boot", True):
            restart = restart_vms(config)
            local_record(evidence_dir / "controller-events.jsonl", "vms-restarted", result=restart)
        variables = dict(config)
        variables["TRIAL_ID"] = trial_id
        by_host: dict[str, list[dict[str, Any]]] = {host: [] for host in hosts}
        for role in plan.get("roles", []):
            _vm, host = host_for(config, role)
            argv = expand_argv(role["command"], variables)
            cwd = expand_argv([role["cwd"]], variables)[0] if role.get("cwd") else None
            by_host[host].append({"name": role["name"], "argv": argv, "cwd": cwd})
        preflight_deadline = time.monotonic() + float(plan.get("timeouts", {}).get("readiness", 240))
        for host in hosts:
            preflight = wait_for_guest_preflight(config, host, by_host[host], preflight_deadline)
            local_record(evidence_dir / "controller-events.jsonl", "guest-preflight", host=host, result=preflight)
            if not preflight.get("ok", False):
                raise RuntimeError(f"guest preflight rejected {host}")
        for host in hosts:
            ensure_remote_dir(config, host, tool_dir)
            copy_helper(config, host, helper_local, helper_remote)
            copy_helper(config, host, wrapper_local, wrapper_remote)
            remote_call(config, host, helper_remote, "prepare", state_dirs[host], trial_id, {"lock_path": f"{config['NG_REMOTE_EVIDENCE_PATH'].rstrip('/')}/.ng-trial.lock", "log_quota_bytes": int(plan.get("log_quota_bytes", 64 * 1024 * 1024)), "metadata": {"scenario": args.scenario, "debug_profile": args.debug_profile, "contract": contract["level"]}})
            prepared_hosts.add(host)
            remote_call(config, host, helper_remote, "start-monitor", state_dirs[host], trial_id)
        offsets: dict[str, dict[str, int]] = {}
        for role in plan.get("roles", []):
            _vm, host = host_for(config, role)
            argv = expand_argv(role["command"], variables)
            cwd = expand_argv([role["cwd"]], variables)[0] if role.get("cwd") else None
            remote_call(config, host, helper_remote, "launch", state_dirs[host], trial_id, {"role": role["name"], "argv": argv, "cwd": cwd, "env": role.get("env", {})})
            offsets.setdefault(role["name"], {})
            result = wait_readiness(config, host, helper_remote, state_dirs[host], trial_id, role, time.monotonic() + float(plan.get("timeouts", {}).get("readiness", 240)), offsets[role["name"]], evidence_dir / "controller-events.jsonl", state_dirs, lease_sequences)
            local_record(evidence_dir / "controller-events.jsonl", "readiness", role=role["name"], result=result)
            if result["result"] != "OBSERVED":
                runtime = "INCONCLUSIVE"
                break
        if runtime == "PASS":
            observation = float(plan.get("timeouts", {}).get("observation", 30))
            deadline = time.monotonic() + observation
            oracle = plan.get("runtime_oracle")
            oracle_seen: set[tuple[str, str]] = set()
            oracle_offsets: dict[str, dict[str, int]] = {}
            oracle_carries: dict[str, dict[str, str]] = {}
            required = oracle.get("required", []) if oracle else []
            while time.monotonic() < deadline:
                renew_all(config, helper_remote, state_dirs, trial_id, lease_sequences)
                if oracle:
                    for requirement in required:
                        role = next(item for item in plan["roles"] if item["name"] == requirement["role"])
                        _vm, host = host_for(config, role)
                        role_offsets = oracle_offsets.setdefault(role["name"], {})
                        role_carries = oracle_carries.setdefault(role["name"], {})
                        observed = remote_call(config, host, helper_remote, "observe", state_dirs[host], trial_id, {"role": role["name"], "patterns": [{"id": requirement["id"], "pattern": requirement["pattern"]}], "offsets": role_offsets, "carries": role_carries, "max_bytes": int(role.get("max_log_read_bytes", 1024 * 1024))})
                        role_offsets.update(observed.get("offsets", {}))
                        role_carries.update(observed.get("carries", {}))
                        if observed.get("overflow"):
                            runtime = "INCONCLUSIVE"
                        for match in observed.get("matches", []):
                            oracle_seen.add((requirement["role"], match["id"]))
                            local_record(evidence_dir / "controller-events.jsonl", "runtime-marker", role=requirement["role"], marker=match)
                    if all((item["role"], item["id"]) in oracle_seen for item in required):
                        break
                time.sleep(min(5.0, max(0.0, deadline - time.monotonic())))
            if oracle and not all((item["role"], item["id"]) in oracle_seen for item in required):
                runtime = "INCONCLUSIVE"
            if plan.get("diagnostic_only", False):
                runtime = "INCONCLUSIVE"
                local_record(evidence_dir / "controller-events.jsonl", "diagnostic-only", reason="no functional runtime oracle")
    except KeyboardInterrupt:
        interrupted = True
        runtime = "ABORTED"
        local_record(evidence_dir / "controller-events.jsonl", "interrupted")
    except Exception as exc:
        runtime = "INCONCLUSIVE"
        local_record(evidence_dir / "controller-events.jsonl", "runtime-error", error=str(exc))
    finally:
        signal.signal(signal.SIGINT, signal.SIG_IGN)
        signal.signal(signal.SIGTERM, signal.SIG_IGN)
        try:
            teardown_result, evidence_result = teardown(config, plan, helper_remote, state_dirs, trial_id, evidence_dir, prepared_hosts)
        finally:
            signal.signal(signal.SIGINT, old_int)
            signal.signal(signal.SIGTERM, old_term)
    try:
        manifest = write_evidence_manifest(evidence_dir, {"trial_id": trial_id, "scenario": args.scenario, "debug_profile": args.debug_profile, "runtime_result": runtime, "teardown_result": teardown_result})
        local_record(evidence_dir / "controller-events.jsonl", "manifest-sealed", manifest_path=str(manifest))
    except OSError as exc:
        evidence_result = "INCOMPLETE"
        teardown_result = "UNKNOWN"
        local_record(evidence_dir / "controller-events.jsonl", "manifest-error", error=str(exc))
    code = 130 if interrupted and teardown_result == "PASS" and evidence_result == "COMPLETE" else classify_result(runtime, teardown_result, evidence_result)
    result = {"schema_version": 1, "trial_id": trial_id, "scenario": args.scenario, "debug_profile": args.debug_profile, "runtime_result": runtime, "teardown_result": teardown_result, "evidence_result": evidence_result, "exit_code": code}
    (evidence_dir / "result.json").write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return code


def cmd_dry_run(args: argparse.Namespace) -> int:
    mode = getattr(args, "mode", "remote")
    config = load_local_config() if mode == "local" else load_config_from_env()
    plan, contract = load_plan_for_trial(Path(args.plan), args.scenario, args.debug_profile, mode=mode)
    profile = validate_local_profile(plan, getattr(args, "local_profile", None), mode=mode)
    print(json.dumps({"ok": True, "operation": "dry-run", "mode": mode, "scenario": args.scenario, "debug_profile": args.debug_profile, "local_profile": profile["name"], "roles": [r["name"] for r in plan["roles"]], "contract_level": contract["level"], "config_keys": sorted(config)}, ensure_ascii=False, indent=2))
    return 0


def cmd_run(args: argparse.Namespace) -> int:
    return run_local_trial(args) if getattr(args, "mode", "remote") == "local" else run_trial(args)


def validate_trial_id(trial_id: str) -> str:
    """Accept only one non-empty, non-special path component."""
    if not isinstance(trial_id, str) or not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]{0,127}", trial_id):
        raise ConfigError("invalid trial id")
    if trial_id in {".", ".."} or "/" in trial_id or "\\\\" in trial_id:
        raise ConfigError("trial id must be one safe path component")
    return trial_id


def trial_paths(config: Mapping[str, str], trial_id: str) -> tuple[str, str]:
    trial_id = validate_trial_id(trial_id)
    return (f"{config['NG_REMOTE_EVIDENCE_PATH'].rstrip('/')}/{trial_id}", trial_id)


def default_remote_helper(config: Mapping[str, str]) -> str:
    return f"{config['NG_REMOTE_EVIDENCE_PATH'].rstrip('/')}/.ng-tools/ng_trial_helper.py"


def cmd_status(args: argparse.Namespace) -> int:
    config = load_config_from_env()
    state, trial_id = trial_paths(config, args.trial)
    helper = args.helper or default_remote_helper(config)
    results = {}
    for label, host in (("source", config["SOURCE_VM_IP"]), ("repository", config["REPO_VM_IP"])):
        results[label] = remote_call(config, host, helper, "status", state, trial_id)
    print(json.dumps({"ok": True, "operation": "status", "trial_id": trial_id, "guests": results}, ensure_ascii=False, indent=2))
    return 0


def cmd_collect(args: argparse.Namespace) -> int:
    config = load_config_from_env()
    state, trial_id = trial_paths(config, args.trial)
    output = Path(args.output).resolve() / trial_id
    output.mkdir(parents=True, exist_ok=True)
    for host, label in ((config["SOURCE_VM_IP"], "source"), (config["REPO_VM_IP"], "repository")):
        collect_remote(config, host, state, output / label)
    print(json.dumps({"ok": True, "operation": "collect", "trial_id": trial_id, "output": str(output)}, ensure_ascii=False, indent=2))
    return 0


def cmd_cleanup(args: argparse.Namespace) -> int:
    config = load_config_from_env()
    state, trial_id = trial_paths(config, args.trial)
    output = Path(args.output).resolve() / trial_id
    output.mkdir(parents=True, exist_ok=True)
    results = {}
    failed = False
    helper = args.helper or default_remote_helper(config)
    for host, label in ((config["SOURCE_VM_IP"], "source"), (config["REPO_VM_IP"], "repository")):
        try:
            stopped = remote_call(config, host, helper, "stop-all", state, trial_id, {"grace": args.grace})
            inventory = remote_call(config, host, helper, "inventory", state, trial_id)
            remote_call(config, host, helper, "seal", state, trial_id)
            collect_remote(config, host, state, output / label)
            results[label] = {"stopped": stopped, "inventory": inventory}
            failed = failed or any(item.get("result") not in {"STOPPED", "NOT_FOUND"} for item in stopped.get("results", []))
            failed = failed or any(item.get("identity_matches") for item in inventory.get("processes", []))
            failed = failed or any(inventory.get("ipc_new_ids", {}).get(kind) for kind in ("shm", "semaphores"))
            failed = failed or any(inventory.get("ipc", {}).get(kind, {}).get("returncode") != 0 for kind in ("shm", "semaphores"))
            remote_call(config, host, helper, "release", state, trial_id)
        except Exception as exc:
            failed = True
            results[label] = {"error": str(exc)}
    result = {"ok": not failed, "operation": "cleanup", "trial_id": trial_id, "guests": results, "vm_stop": "not-performed"}
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 20 if failed else 0


def cmd_verify_bundle(args: argparse.Namespace) -> int:
    """Verify a local evidence bundle without runtime configuration."""
    from evidence_verifier import (
        CODE_BUNDLE_MISSING,
        CODE_SCHEMA_OLD,
        CODE_TAMPERED,
        CODE_TERMINAL_SEAL_MISSING,
        CODE_VALID,
        verify_bundle,
    )

    report = verify_bundle(args.bundle)
    if args.require_acceptance and report.get("code") == CODE_VALID and not report.get("accepted", False):
        report = {**report, "accepted": False, "errors": [*report.get("errors", []), "bundle is intact but not acceptance-eligible"]}
    print(json.dumps(report, ensure_ascii=False, indent=2))
    if report["code"] == CODE_VALID and (not args.require_acceptance or report.get("accepted", False)):
        return 0
    return {
        CODE_BUNDLE_MISSING: 30,
        CODE_TAMPERED: 31,
        CODE_SCHEMA_OLD: 32,
        CODE_TERMINAL_SEAL_MISSING: 33,
    }.get(report["code"], 31)


def cmd_preflight(args: argparse.Namespace) -> int:
    mode = getattr(args, "mode", "remote")
    if mode == "local":
        config = load_local_config()
        plan, contract = load_plan_for_trial(Path(args.plan), args.scenario, args.debug_profile, mode="local")
        profile = validate_local_profile(plan, getattr(args, "local_profile", None), mode="local")
        paths = {key: str(Path(config[key]).resolve()) for key in LOCAL_REQUIRED_ENV}
        errors = [f"{key} path missing" for key, value in paths.items() if key != "NG_LOCAL_EVIDENCE_PATH" and not Path(value).exists()]
        if errors:
            raise ConfigError("local preflight rejected: " + ", ".join(errors))
        print(json.dumps({"ok": True, "operation": "preflight", "mode": "local", "scenario": args.scenario, "local_profile": profile["name"], "contract_level": contract["level"], "paths": paths}, ensure_ascii=False, indent=2))
        return 0
    config = load_config_from_env()
    plan, contract = load_plan_for_trial(Path(args.plan), args.scenario, args.debug_profile, mode="remote")
    validate_local_profile(plan, getattr(args, "local_profile", None), mode="remote")
    variables = dict(config)
    variables["TRIAL_ID"] = "preflight"
    by_host: dict[str, list[dict[str, Any]]] = {config["SOURCE_VM_IP"]: [], config["REPO_VM_IP"]: []}
    for role in plan.get("roles", []):
        _vm, host = host_for(config, role)
        argv = expand_argv(role["command"], variables)
        cwd = expand_argv([role["cwd"]], variables)[0] if role.get("cwd") else None
        by_host[host].append({"name": role["name"], "argv": argv, "cwd": cwd})
    results = {}
    for label, host in (("source", config["SOURCE_VM_IP"]), ("repository", config["REPO_VM_IP"])):
        results[label] = guest_preflight(config, host, by_host[host])
    print(json.dumps({"ok": True, "operation": "preflight", "scenario": args.scenario, "contract_level": contract["level"], "guests": results}, ensure_ascii=False, indent=2))
    return 0


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest="operation", required=True)
    for name, fn in (("dry-run", cmd_dry_run), ("preflight", cmd_preflight)):
        sp = sub.add_parser(name)
        sp.add_argument("--plan", required=True)
        sp.add_argument("--scenario", required=True, choices=sorted(SCENARIOS))
        sp.add_argument("--mode", choices=("remote", "local"), default="remote")
        sp.add_argument("--debug-profile", default="obs-normal")
        sp.add_argument("--local-profile", choices=("unprivileged", "native-privileged"), default=None)
        sp.set_defaults(func=fn)
    run = sub.add_parser("run")
    run.add_argument("--plan", required=True)
    run.add_argument("--scenario", required=True, choices=sorted(SCENARIOS))
    run.add_argument("--mode", choices=("remote", "local"), default="remote")
    run.add_argument("--debug-profile", default="obs-normal")
    run.add_argument("--local-profile", choices=("unprivileged", "native-privileged"), default=None)
    run.add_argument("--helper-local", default=str(Path(__file__).parent / "remote" / "ng_trial_helper.py"))
    run.add_argument("--trial")
    run.set_defaults(func=cmd_run)
    for name, fn in (("status", cmd_status), ("collect", cmd_collect), ("cleanup", cmd_cleanup)):
        sp = sub.add_parser(name)
        sp.add_argument("--trial", required=True)
        sp.add_argument("--helper", default=None)
        if name in {"collect", "cleanup"}:
            sp.add_argument("--output", default=".")
        if name == "cleanup":
            sp.add_argument("--grace", type=float, default=10.0)
        sp.set_defaults(func=fn)
    verify = sub.add_parser("verify-bundle")
    verify.add_argument("--bundle", required=True)
    verify.add_argument("--json", action="store_true", help="retain JSON output for scripting")
    verify.add_argument("--require-acceptance", action="store_true")
    verify.set_defaults(func=cmd_verify_bundle)
    return p


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return args.func(args)
    except (ConfigError, ValueError) as exc:
        print(f"CONFIG_ERROR: {exc}", file=sys.stderr)
        return 2
    except Exception as exc:
        print(f"EXECUTOR_ERROR: {exc}", file=sys.stderr)
        return 11


if __name__ == "__main__":
    raise SystemExit(main())
