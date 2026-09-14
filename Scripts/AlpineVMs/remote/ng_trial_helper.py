#!/usr/bin/env python3
"""Remote, identity-aware process helper for NovaGenesis trials.

The controller invokes this file through short, non-PTY SSH calls.  Each
application is placed in its own session/process group and described by an
atomic JSON record.  The helper never uses process-name matching for signals.
"""
from __future__ import annotations

import argparse
import datetime as dt
import errno
import json
import os
import signal
import subprocess
import sys
import time
from pathlib import Path
from typing import Any


class HelperError(RuntimeError):
    pass


def utc_now() -> str:
    return dt.datetime.now(dt.timezone.utc).isoformat().replace("+00:00", "Z")


def atomic_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_name(f".{path.name}.{os.getpid()}.tmp")
    tmp.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    os.replace(tmp, path)


def read_json(path: Path, default: Any = None) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError:
        return default


def event(state: Path, kind: str, **fields: Any) -> None:
    record = {"utc": utc_now(), "kind": kind, **fields}
    with (state / "events.jsonl").open("a", encoding="utf-8") as fh:
        fh.write(json.dumps(record, ensure_ascii=False) + "\n")
        fh.flush()
        os.fsync(fh.fileno())


def boot_id() -> str:
    try:
        return Path("/proc/sys/kernel/random/boot_id").read_text().strip()
    except OSError:
        return "unknown"


def proc_stat(pid: int) -> dict[str, Any] | None:
    try:
        raw = Path(f"/proc/{pid}/stat").read_text(encoding="utf-8")
    except (FileNotFoundError, ProcessLookupError, PermissionError):
        return None
    close = raw.rfind(")")
    if close < 0:
        return None
    fields = raw[close + 2 :].split()
    try:
        # fields starts at stat field 3 (state); field 22 is index 19.
        return {
            "pid": pid,
            "state": fields[0],
            "ppid": int(fields[1]),
            "pgid": int(fields[2]),
            "sid": int(fields[3]),
            "starttime": int(fields[19]),
        }
    except (IndexError, ValueError):
        return None


def proc_exe(pid: int) -> str | None:
    try:
        return os.readlink(f"/proc/{pid}/exe")
    except OSError:
        return None


def proc_argv(pid: int) -> list[str]:
    try:
        raw = Path(f"/proc/{pid}/cmdline").read_bytes()
        return [x.decode("utf-8", "replace") for x in raw.split(b"\0") if x]
    except OSError:
        return []


def group_members(pgid: int) -> list[int]:
    members: list[int] = []
    for entry in Path("/proc").iterdir():
        if not entry.name.isdigit():
            continue
        info = proc_stat(int(entry.name))
        if info and info["pgid"] == pgid:
            members.append(info["pid"])
    return sorted(members)


def parse_ipc_ids(text: str) -> list[str]:
    ids = []
    for line in text.splitlines():
        fields = line.split()
        if fields and fields[0].isdigit():
            ids.append(fields[0])
    return ids


def ipc_snapshot() -> dict[str, Any]:
    result: dict[str, Any] = {}
    for kind, command in (("shm", ["ipcs", "-m"]), ("semaphores", ["ipcs", "-s"])):
        try:
            run = subprocess.run(command, capture_output=True, text=True, timeout=10, check=False)
            result[kind] = {"available": True, "returncode": run.returncode, "ids": parse_ipc_ids(run.stdout), "stdout": run.stdout, "stderr": run.stderr}
        except (FileNotFoundError, subprocess.TimeoutExpired) as exc:
            result[kind] = {"available": False, "ids": [], "error": type(exc).__name__}
    return result


def capture_identity(pid: int, role: str, trial_id: str, argv: list[str]) -> dict[str, Any]:
    info = proc_stat(pid)
    if not info:
        raise HelperError(f"process {pid} disappeared before identity capture")
    return {
        "trial_id": trial_id,
        "role": role,
        "pid": pid,
        "pgid": info["pgid"],
        "sid": info["sid"],
        "starttime": info["starttime"],
        "boot_id": boot_id(),
        "exe": proc_exe(pid),
        "argv": list(argv),
        "captured_utc": utc_now(),
    }


def identity_matches(record: dict[str, Any]) -> bool:
    try:
        pid = int(record["pid"])
        expected_start = int(record["starttime"])
        expected_pgid = int(record["pgid"])
    except (KeyError, TypeError, ValueError):
        return False
    info = proc_stat(pid)
    if not info or info["starttime"] != expected_start or info["pgid"] != expected_pgid:
        return False
    if record.get("boot_id") not in (None, "unknown", boot_id()):
        return False
    expected_exe = record.get("exe")
    if expected_exe and proc_exe(pid) not in (expected_exe, None):
        return False
    return True


def role_path(state: Path, role: str) -> Path:
    if not role or "/" in role or "\\" in role or role in {".", ".."}:
        raise HelperError("invalid role")
    return state / "roles" / f"{role}.json"


def validate_argv(argv: Any) -> list[str]:
    if not isinstance(argv, list) or not argv or not all(isinstance(x, str) for x in argv):
        raise HelperError("argv must be a non-empty array of strings")
    if any("\0" in x for x in argv):
        raise HelperError("argv contains NUL")
    return argv


def prepare(state: Path, trial_id: str, payload: dict[str, Any]) -> dict[str, Any]:
    try:
        log_quota = int(payload.get("log_quota_bytes", 64 * 1024 * 1024))
    except (TypeError, ValueError) as exc:
        raise HelperError("log quota is not an integer") from exc
    if log_quota <= 0 or log_quota > 512 * 1024 * 1024:
        raise HelperError("log quota outside bounded range")
    if state.exists():
        raise HelperError("trial state already exists; recovery must use status/cleanup")
    state.parent.mkdir(parents=True, exist_ok=True)
    state.mkdir()
    (state / "roles").mkdir()
    (state / "logs").mkdir()
    lock_path = Path(payload.get("lock_path", state.parent / ".ng-trial.lock"))
    if not lock_path.is_absolute():
        raise HelperError("lock path must be absolute")
    try:
        fd = os.open(lock_path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
        with os.fdopen(fd, "w", encoding="utf-8") as fh:
            fh.write(trial_id + "\n")
            fh.flush()
            os.fsync(fh.fileno())
    except FileExistsError as exc:
        raise HelperError("another trial owns the guest lock") from exc
    record = {
        "schema_version": 1,
        "trial_id": trial_id,
        "boot_id": boot_id(),
        "prepared_utc": utc_now(),
        "sealed": False,
        "remote_pid": os.getpid(),
        "lock_path": str(lock_path),
        "ipc_before": ipc_snapshot(),
        "log_quota_bytes": log_quota,
        "metadata": payload.get("metadata", {}),
    }
    if record["log_quota_bytes"] <= 0 or record["log_quota_bytes"] > 512 * 1024 * 1024:
        raise HelperError("log quota outside bounded range")
    atomic_json(state / "trial.json", record)
    atomic_json(state / "lease.json", {"trial_id": trial_id, "sequence": 0, "expires_monotonic": time.monotonic() + 60, "expires_epoch": time.time() + 60})
    event(state, "prepared", trial_id=trial_id, boot_id=record["boot_id"])
    return record


def start_monitor(state: Path, trial_id: str) -> dict[str, Any]:
    existing = read_json(state / "monitor.json")
    if existing and identity_matches(existing):
        return existing
    log = (state / "logs" / "monitor.log").open("a", buffering=1, encoding="utf-8")
    try:
        proc = subprocess.Popen(
            [sys.executable, str(Path(__file__).resolve()), "monitor", "--state-dir", str(state), "--trial-id", trial_id],
            stdin=subprocess.DEVNULL,
            stdout=log,
            stderr=subprocess.STDOUT,
            start_new_session=True,
            close_fds=True,
        )
        record = capture_identity(proc.pid, "monitor", trial_id, [sys.executable, str(Path(__file__).resolve()), "monitor"])
    finally:
        log.close()
    setattr(proc, "_child_created", False)
    atomic_json(state / "monitor.json", record)
    event(state, "monitor_started", pid=proc.pid, pgid=record["pgid"])
    return record


def launch(state: Path, trial_id: str, payload: dict[str, Any]) -> dict[str, Any]:
    role = payload.get("role")
    argv = validate_argv(payload.get("argv"))
    if not isinstance(role, str):
        raise HelperError("role is required")
    path = role_path(state, role)
    old = read_json(path)
    if old and identity_matches(old):
        return old
    cwd = payload.get("cwd")
    if cwd is not None and (not isinstance(cwd, str) or not os.path.isabs(cwd)):
        raise HelperError("cwd must be an absolute path")
    env = os.environ.copy()
    extra = payload.get("env", {})
    if not isinstance(extra, dict) or not all(isinstance(k, str) and isinstance(v, str) for k, v in extra.items()):
        raise HelperError("env must be an object of strings")
    env.update(extra)
    log_dir = state / "logs" / role
    log_dir.mkdir(parents=True, exist_ok=True)
    stdout = (log_dir / "stdout.log").open("ab", buffering=0)
    stderr = (log_dir / "stderr.log").open("ab", buffering=0)
    wrapper = Path(__file__).with_name("ng_role_wrapper.py")
    wrapped_argv = [sys.executable, str(wrapper), "--", *argv]
    try:
        proc = subprocess.Popen(
            wrapped_argv,
            cwd=cwd,
            env=env,
            stdin=subprocess.DEVNULL,
            stdout=stdout,
            stderr=stderr,
            start_new_session=True,
            close_fds=True,
        )
        record = capture_identity(proc.pid, role, trial_id, wrapped_argv)
    finally:
        stdout.close()
        stderr.close()
    setattr(proc, "_child_created", False)
    record.update({"command_argv": argv, "wrapper": str(wrapper), "cwd": cwd, "launched_utc": utc_now(), "status": "running"})
    atomic_json(path, record)
    event(state, "role_started", role=role, pid=proc.pid, pgid=record["pgid"], argv=argv)
    return record


def wait_group_gone(pgid: int, timeout: float) -> bool:
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        if not group_members(pgid):
            return True
        time.sleep(0.05)
    return not group_members(pgid)


def stop_role(state: Path, role: str, grace: float = 10.0) -> dict[str, Any]:
    path = role_path(state, role)
    record = read_json(path)
    if not record:
        return {"role": role, "result": "NOT_FOUND"}
    pgid = int(record.get("pgid", -1))
    if not identity_matches(record):
        result = {"role": role, "result": "IDENTITY_UNKNOWN", "record": record}
        event(state, "role_stop_refused", role=role, reason="identity_unknown")
        return result
    if pgid <= 1 or pgid == os.getpgrp():
        result = {"role": role, "result": "IDENTITY_UNKNOWN", "reason": "unsafe_pgid"}
        event(state, "role_stop_refused", role=role, reason="unsafe_pgid")
        return result
    signals: list[str] = []
    try:
        os.killpg(pgid, signal.SIGTERM)
        signals.append("SIGTERM")
    except ProcessLookupError:
        pass
    gone = wait_group_gone(pgid, grace)
    if not gone:
        if not identity_matches(record):
            result = {"role": role, "result": "IDENTITY_UNKNOWN", "signals": signals}
            event(state, "role_stop_refused", role=role, reason="identity_changed")
            return result
        try:
            os.killpg(pgid, signal.SIGKILL)
            signals.append("SIGKILL")
        except ProcessLookupError:
            pass
        gone = wait_group_gone(pgid, min(10.0, grace))
    result = {"role": role, "result": "STOPPED" if gone else "RESIDUAL", "signals": signals, "members": group_members(pgid)}
    atomic_json(path, {**record, "status": result["result"].lower(), "stopped_utc": utc_now(), "signals": signals})
    event(state, "role_stopped", **result)
    return result


def inventory(state: Path, persist: bool = True) -> dict[str, Any]:
    processes = []
    for path in sorted((state / "roles").glob("*.json")) if (state / "roles").exists() else []:
        rec = read_json(path)
        if rec:
            processes.append({"kind": "role", "record": rec, "identity_matches": identity_matches(rec), "members": group_members(int(rec.get("pgid", -1)))})
    monitor_record = read_json(state / "monitor.json")
    if monitor_record:
        processes.append({"kind": "monitor", "record": monitor_record, "identity_matches": identity_matches(monitor_record), "members": group_members(int(monitor_record.get("pgid", -1)))})
    trial = read_json(state / "trial.json", {})
    baseline = trial.get("ipc_before", {})
    ipc = ipc_snapshot()
    new_ids = {}
    for kind in ("shm", "semaphores"):
        before = set(baseline.get(kind, {}).get("ids", []))
        after = set(ipc.get(kind, {}).get("ids", []))
        new_ids[kind] = sorted(after - before)
    result = {"trial_id": trial.get("trial_id"), "boot_id": boot_id(), "processes": processes, "ipc": ipc, "ipc_new_ids": new_ids, "utc": utc_now()}
    if persist:
        atomic_json(state / "inventory.json", result)
        event(state, "inventory", process_count=len(processes))
    return result


def renew(state: Path, trial_id: str, seconds: float, sequence: int) -> dict[str, Any]:
    if not (0 < seconds <= 300) or sequence <= 0:
        raise HelperError("lease seconds or sequence outside bounded range")
    lease = read_json(state / "lease.json", {})
    if lease.get("trial_id") != trial_id:
        raise HelperError("unknown trial lease")
    if sequence <= int(lease.get("sequence", 0)):
        raise HelperError("lease sequence is not increasing")
    lease.update({"sequence": sequence, "expires_monotonic": time.monotonic() + seconds, "expires_epoch": time.time() + seconds, "renewed_utc": utc_now()})
    atomic_json(state / "lease.json", lease)
    event(state, "lease_renewed", sequence=sequence, expires_epoch=lease["expires_epoch"])
    return lease


def observe(state: Path, role: str, payload: dict[str, Any]) -> dict[str, Any]:
    """Read only bounded new log bytes and match declared readiness markers."""
    role_path(state, role)
    patterns = payload.get("patterns", [])
    if not isinstance(patterns, list) or not all(isinstance(x, dict) for x in patterns):
        raise HelperError("patterns must be an array of objects")
    offsets = payload.get("offsets", {})
    carries = payload.get("carries", {})
    max_bytes = int(payload.get("max_bytes", 1024 * 1024))
    if max_bytes <= 0 or max_bytes > 16 * 1024 * 1024:
        raise HelperError("max_bytes outside bounded range")
    if not isinstance(offsets, dict) or not isinstance(carries, dict):
        raise HelperError("offsets and carries must be objects")
    log_dir = state / "logs" / role
    matches = []
    new_offsets: dict[str, int] = {}
    new_carries: dict[str, str] = {}
    overflow = False
    for name in ("stdout.log", "stderr.log"):
        path = log_dir / name
        offset = int(offsets.get(name, 0))
        carry = str(carries.get(name, ""))
        stream_overflow = False
        try:
            size = path.stat().st_size
            if offset < 0 or offset > size:
                raise HelperError(f"invalid log offset for {role}/{name}")
            if size - offset > max_bytes:
                overflow = True
                stream_overflow = True
                new_offsets[name] = offset
                new_carries[name] = carry
                continue
            with path.open("rb") as fh:
                fh.seek(offset)
                raw = fh.read(max_bytes + 1)
            if len(raw) > max_bytes:
                overflow = True
                stream_overflow = True
                raw = raw[:max_bytes]
            new_offsets[name] = offset + len(raw)
            text = carry + raw.decode("utf-8", "replace")
        except FileNotFoundError:
            new_offsets[name] = offset
            new_carries[name] = carry
            text = carry
        if not stream_overflow:
            complete_lines = text.splitlines(keepends=True)
            tail = ""
            if complete_lines and not complete_lines[-1].endswith(("\n", "\r")):
                tail = complete_lines.pop()
            new_carries[name] = tail[-8192:]
            for line_number, line in enumerate(complete_lines, 1):
                line = line.rstrip("\r\n")
                for pattern in patterns:
                    marker = pattern.get("id")
                    expression = pattern.get("pattern")
                    if not isinstance(marker, str) or not isinstance(expression, str):
                        raise HelperError("each pattern requires id and pattern")
                    import re
                    if re.search(expression, line):
                        matches.append({"id": marker, "stream": name, "line": line, "line_number": line_number})
    return {"role": role, "matches": matches, "offsets": new_offsets, "carries": new_carries, "overflow": overflow, "identity_matches": identity_matches(read_json(role_path(state, role), {}))}


def stop_all(state: Path, grace: float) -> list[dict[str, Any]]:
    results = []
    for path in sorted((state / "roles").glob("*.json")) if (state / "roles").exists() else []:
        if path.name == "monitor.json":
            continue
        result = stop_role(state, path.stem, grace)
        results.append(result)
    return results


def monitor(state: Path, trial_id: str) -> int:
    def terminate(_signum: int, _frame: Any) -> None:
        stop_all(state, 5.0)
        raise SystemExit(0)

    signal.signal(signal.SIGTERM, terminate)
    signal.signal(signal.SIGINT, terminate)
    event(state, "monitor_loop", pid=os.getpid())
    while True:
        trial = read_json(state / "trial.json", {})
        if trial.get("trial_id") != trial_id or trial.get("sealed"):
            stop_all(state, 5.0)
            return 0
        try:
            total_log_bytes = sum(path.stat().st_size for path in (state / "logs").rglob("*") if path.is_file())
        except OSError:
            total_log_bytes = int(trial.get("log_quota_bytes", 0)) + 1
        if total_log_bytes > int(trial.get("log_quota_bytes", 0)):
            event(state, "log_quota_exceeded", bytes=total_log_bytes, quota=trial.get("log_quota_bytes"))
            stop_all(state, 5.0)
            return 1
        lease = read_json(state / "lease.json", {})
        try:
            expired = lease.get("trial_id") != trial_id or float(lease.get("expires_monotonic", 0)) < time.monotonic()
        except (TypeError, ValueError):
            expired = True
        if expired:
            event(state, "lease_expired")
            stop_all(state, 5.0)
            return 0
        time.sleep(1.0)


def seal(state: Path, trial_id: str) -> dict[str, Any]:
    trial = read_json(state / "trial.json", {})
    if trial.get("trial_id") != trial_id:
        raise HelperError("unknown trial")
    trial.update({"sealed": True, "sealed_utc": utc_now()})
    atomic_json(state / "trial.json", trial)
    event(state, "sealed")
    return trial


def release_lock(state: Path, trial_id: str) -> dict[str, Any]:
    trial = read_json(state / "trial.json", {})
    if trial.get("trial_id") != trial_id or not trial.get("sealed"):
        raise HelperError("trial is not sealed or trial id does not match")
    inv = read_json(state / "inventory.json", {})
    live = [item for item in inv.get("processes", []) if item.get("identity_matches")]
    new_ipc = inv.get("ipc_new_ids", {})
    if live or any(new_ipc.get(kind) for kind in ("shm", "semaphores")):
        raise HelperError("cannot release lock while trial resources remain")
    lock_path = Path(trial.get("lock_path", ""))
    try:
        if lock_path.read_text(encoding="utf-8").strip() != trial_id:
            raise HelperError("lock owner does not match trial")
        lock_path.unlink()
    except FileNotFoundError:
        raise HelperError("trial lock is missing")
    event(state, "lock_released", lock_path=str(lock_path))
    return {"released": True, "lock_path": str(lock_path)}


def dispatch(args: argparse.Namespace, payload: dict[str, Any]) -> dict[str, Any]:
    state = Path(args.state_dir)
    if args.action == "prepare":
        return prepare(state, args.trial_id, payload)
    if args.action == "start-monitor":
        return start_monitor(state, args.trial_id)
    if args.action == "launch":
        return launch(state, args.trial_id, payload)
    if args.action == "stop":
        return {"results": [stop_role(state, payload["role"], float(payload.get("grace", 10.0)))]}
    if args.action == "stop-all":
        return {"results": stop_all(state, float(payload.get("grace", 10.0)))}
    if args.action == "status":
        return inventory(state, persist=False)
    if args.action == "inventory":
        return inventory(state)
    if args.action == "renew":
        return renew(state, args.trial_id, float(payload.get("seconds", 30.0)), int(payload.get("sequence", 0)))
    if args.action == "observe":
        return observe(state, str(payload["role"]), payload)
    if args.action == "seal":
        return seal(state, args.trial_id)
    if args.action == "release":
        return release_lock(state, args.trial_id)
    if args.action == "monitor":
        return {"monitor_rc": monitor(state, args.trial_id)}
    if args.action == "preflight":
        return {"boot_id": boot_id(), "python": sys.version, "cwd": os.getcwd(), "disk": os.statvfs(state if state.exists() else Path("/" )).f_bavail}
    raise HelperError(f"unknown action: {args.action}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("action", choices=["prepare", "start-monitor", "launch", "stop", "stop-all", "status", "inventory", "renew", "observe", "seal", "release", "monitor", "preflight"])
    parser.add_argument("--state-dir", required=True)
    parser.add_argument("--trial-id", required=True)
    args = parser.parse_args()
    try:
        raw = sys.stdin.read()
        payload = json.loads(raw) if raw.strip() else {}
        result = dispatch(args, payload)
        print(json.dumps({"ok": True, "result": result}, ensure_ascii=False))
        return 0
    except Exception as exc:
        print(json.dumps({"ok": False, "error": type(exc).__name__, "message": str(exc)}, ensure_ascii=False))
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
