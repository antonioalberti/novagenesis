#!/usr/bin/env python3
"""Fail-closed controller for staged NovaGenesis VM trials.

The controller uses short, non-PTY SSH calls and a per-trial helper on each
guest.  It deliberately does not invoke the older manual run_*.sh launchers.
"""
from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import math
import os
import pwd
import re
import shlex
import signal
import shutil
import subprocess
import sys
import time
import uuid
from pathlib import Path
from typing import Any, Mapping

from evidence_verifier import CODE_VALID, verify_bundle


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
    record = {"utc": dt.datetime.now(dt.timezone.utc).isoformat(), "event": event, **fields}
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


def write_evidence_manifest(evidence_dir: Path, metadata: dict[str, Any], schema_version: int | None = None) -> Path:
    """Write a manifest, keeping remote v1 and allowing local v2 sealing."""
    effective_schema = schema_version if schema_version is not None else (2 if metadata.get("mode") == "local" else 1)
    if effective_schema not in {1, 2}:
        raise ValueError("unsupported evidence schema version")
    entries = []
    for path in sorted(evidence_dir.rglob("*")):
        if not path.is_file() or path.name in {"manifest.json", "manifest.sha256", "terminal-seal.json"}:
            continue
        digest = hashlib.sha256(path.read_bytes()).hexdigest()
        entries.append({"path": str(path.relative_to(evidence_dir)), "size": path.stat().st_size, "sha256": digest})
    manifest = {**metadata, "schema_version": effective_schema, "files": entries}
    manifest_path = evidence_dir / "manifest.json"
    manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    manifest_hash = hashlib.sha256(manifest_path.read_bytes()).hexdigest()
    if effective_schema == 1:
        (evidence_dir / "manifest.sha256").write_text(f"{manifest_hash}  manifest.json\n", encoding="utf-8")
    if effective_schema == 2:
        (evidence_dir / "terminal-seal.json").write_text(
            json.dumps({"schema_version": 2, "kind": "terminal-seal", "manifest_path": "manifest.json", "manifest_sha256": manifest_hash, "sealed": True}, ensure_ascii=False, indent=2) + "\n",
            encoding="utf-8",
        )
    return manifest_path


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


def local_ipc_ids(kind: str) -> set[str] | None:
    command = ["ipcs", "-m" if kind == "shm" else "-s"]
    try:
        run = subprocess.run(command, capture_output=True, text=True, timeout=10, check=False)
    except (OSError, subprocess.SubprocessError):
        return None
    if run.returncode != 0:
        return None
    owner = pwd.getpwuid(os.getuid()).pw_name
    header_token = "shmid" if kind == "shm" else "semid"
    lines = run.stdout.splitlines()
    header_seen = False
    header_required = {"key", header_token, "owner", "perms", "bytes", "nattch"} if kind == "shm" else {"key", header_token, "owner", "perms", "nsems"}
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
        if len(fields) < minimum_columns or not fields[1].isdigit():
            return None
        permission_index = 3
        if not re.fullmatch(r"[0-7]{3,4}", fields[permission_index]):
            return None
        if kind == "shm":
            if not fields[4].isdigit() or not fields[5].isdigit():
                return None
        elif not fields[4].isdigit():
            return None
        if fields[2] == owner:
            ids.add(fields[1])
    if not header_seen:
        return None
    return ids


def local_ipc_snapshot() -> dict[str, set[str] | None]:
    return {"shm": local_ipc_ids("shm"), "semaphores": local_ipc_ids("semaphores")}


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


def process_starttime(pid: int) -> str | None:
    record = read_proc_stat(pid)
    return record.get("starttime") if record.get("status") == "ok" else None


def process_state(pid: int) -> str | None:
    record = read_proc_stat(pid)
    return record.get("state") if record.get("status") == "ok" else None


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
        record = read_proc_stat(int(entry.name))
        if record["status"] == "gone":
            continue
        if record["status"] != "ok":
            complete = False
            continue
        if record["pgid"] == pgid:
            members.append({"pid": int(entry.name), "pgid": pgid, "comm": record["comm"], "starttime": record["starttime"]})
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
        record = read_proc_stat(pid)
        if record["status"] == "gone":
            continue
        if record["status"] != "ok":
            complete = False
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
        pending.extend(children.get(pid, []))
    return {"descendants": sorted(descendants, key=lambda item: item["pid"]), "complete": complete}


def local_stop_tracked_descendants(tracked: Mapping[int, Mapping[str, Any]]) -> tuple[bool, list[dict[str, Any]]]:
    residual: list[dict[str, Any]] = []
    for pid, expected in tracked.items():
        record = read_proc_stat(pid)
        if record["status"] == "gone":
            continue
        if record["status"] != "ok":
            residual.append({"pid": pid, "reason": "identity-unavailable", "error": record.get("error")})
            continue
        expected_pgid = expected.get("pgid")
        if expected_pgid is None:
            residual.append({"pid": pid, "reason": "identity-unavailable", "error": "tracked process-group identity missing"})
            continue
        if record["starttime"] != expected.get("starttime"):
            residual.append({"pid": pid, "reason": "identity-mismatch", "expected_starttime": expected.get("starttime"), "actual_starttime": record["starttime"]})
            continue
        if record["pgid"] != expected_pgid:
            residual.append({"pid": pid, "reason": "identity-mismatch", "expected_pgid": expected_pgid, "actual_pgid": record["pgid"]})
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
            current = read_proc_stat(pid)
            if current["status"] == "gone" or (current["status"] == "ok" and current["state"] in {"Z", "X"}):
                break
            if current["status"] != "ok":
                unknown = current
                break
            if current["starttime"] != expected.get("starttime") or current["pgid"] != expected_pgid:
                residual.append({"pid": pid, "reason": "identity-mismatch-after-term", "expected_starttime": expected.get("starttime"), "actual_starttime": current["starttime"], "expected_pgid": expected_pgid, "actual_pgid": current["pgid"]})
                unknown = {"status": "mismatch"}
                break
            time.sleep(0.05)
        if unknown:
            residual.append({"pid": pid, "reason": "identity-unavailable-after-term", "error": unknown.get("error")})
            continue
        current = read_proc_stat(pid)
        if current["status"] == "gone" or (current["status"] == "ok" and current["state"] in {"Z", "X"}):
            continue
        if current["status"] != "ok":
            residual.append({"pid": pid, "reason": "identity-unavailable-before-kill", "error": current.get("error")})
            continue
        if current["starttime"] != expected.get("starttime") or current["pgid"] != expected_pgid:
            residual.append({"pid": pid, "reason": "identity-mismatch-before-kill", "expected_starttime": expected.get("starttime"), "actual_starttime": current.get("starttime"), "expected_pgid": expected_pgid, "actual_pgid": current.get("pgid")})
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
            current = read_proc_stat(pid)
            if current["status"] == "gone" or (current["status"] == "ok" and current["state"] in {"Z", "X"}):
                break
            if current["status"] != "ok":
                residual.append({"pid": pid, "reason": "identity-unavailable-after-kill", "error": current.get("error")})
                break
            if current.get("starttime") != expected.get("starttime") or current.get("pgid") != expected_pgid:
                residual.append({"pid": pid, "reason": "identity-mismatch-after-kill", "expected_starttime": expected.get("starttime"), "actual_starttime": current.get("starttime"), "expected_pgid": expected_pgid, "actual_pgid": current.get("pgid")})
                break
            time.sleep(0.05)
        else:
            residual.append({"pid": pid, "reason": "survived-descendant-stop", "starttime": expected.get("starttime")})
    return not residual, residual


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
    ):
        if key in provenance and not provenance.get(key, False):
            blockers.append(message)
    return {"eligible": not blockers, "blockers": blockers}


def validate_local_build_manifest(manifest: Any, source_head: str | None, binaries: Mapping[str, Mapping[str, Any]]) -> tuple[bool, str | None]:
    """Validate the minimum semantic source/build/executable linkage."""
    if not isinstance(manifest, Mapping) or not manifest:
        return False, "build manifest is empty or invalid"
    if not manifest.get("commands"):
        return False, "build recipe missing"
    snapshot = manifest.get("source_snapshot")
    if not isinstance(snapshot, Mapping) or not snapshot.get("head") or snapshot.get("head") != source_head:
        return False, "build source identity mismatch"
    expected = manifest.get("binaries")
    if not isinstance(expected, Mapping) or not expected:
        return False, "build binaries missing"
    for role, binary in binaries.items():
        if not isinstance(binary, Mapping) or not binary.get("resolved_path") or not binary.get("sha256"):
            return False, f"build linkage missing for {role}"
        candidate = expected.get(role)
        if not isinstance(candidate, Mapping):
            return False, f"build binary missing for {role}"
        if candidate.get("path") != binary.get("resolved_path") or candidate.get("sha256") != binary.get("sha256"):
            return False, f"build binary mismatch for {role}"
    return True, None


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
                result[item.name] = {"size": item.stat().st_size, "sha256": hashlib.sha256(item.read_bytes()).hexdigest()}
        return result, unsafe
    source, unsafe_source = digest_map(oracle["source"])
    repository, unsafe_repository = digest_map(oracle["repository"])
    result = {
        "source_count": len(source),
        "repository_count": len(repository),
        "expected_count": oracle["expected_count"],
        "source_map": source,
        "repository_map": repository,
        "unsafe_source": unsafe_source,
        "unsafe_repository": unsafe_repository,
        "source_equals_repository": source == repository,
        "missing_in_repository": sorted(set(source) - set(repository)),
        "extra_in_repository": sorted(set(repository) - set(source)),
        "hash_mismatches": sorted(name for name in set(source) & set(repository) if source[name] != repository[name]),
    }
    complete = len(source) == oracle["expected_count"] and len(repository) == oracle["expected_count"]
    safe = not unsafe_source and not unsafe_repository
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
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def local_write_v2_plan(evidence_dir: Path, plan: Mapping[str, Any], contract: Mapping[str, Any], config: Mapping[str, str], variables: Mapping[str, str]) -> None:
    """Persist the local plan/configuration inputs before a role is launched."""
    plan_dir = evidence_dir / "plan"
    expanded = json.loads(json.dumps(plan))
    for role in expanded.get("roles", []):
        role["command"] = expand_argv(role.get("command", []), variables)
        if role.get("cwd"):
            role["cwd"] = expand_argv([role["cwd"]], variables)[0]
        role["env"] = {key: expand_argv([value], variables)[0] for key, value in role.get("env", {}).items()}
    local_json_write(plan_dir / "original.json", dict(plan))
    local_json_write(plan_dir / "expanded.json", expanded)
    local_json_write(plan_dir / "scenario.json", contract.get("scenario", {}))
    local_json_write(plan_dir / "observability.json", contract.get("profile", {}))
    local_json_write(plan_dir / "effective-config.json", {key: str(value) for key, value in sorted(config.items())})
    local_json_write(evidence_dir / "plan.json", dict(plan))


def local_prepare_v2_layout(evidence_dir: Path, plan: Mapping[str, Any], contract: Mapping[str, Any], config: Mapping[str, str], variables: Mapping[str, str]) -> dict[str, Any]:
    """Create durable v2 directories and preserve the pre-launch workload map."""
    local_write_v2_plan(evidence_dir, plan, contract, config, variables)
    for directory in (evidence_dir / "artifacts" / "source", evidence_dir / "artifacts" / "repository", evidence_dir / "inventory"):
        directory.mkdir(parents=True, exist_ok=True)
    source = Path(expand_argv(["${NG_LOCAL_IO_PATH}/Source1"], variables)[0])
    repository = Path(expand_argv(["${NG_LOCAL_IO_PATH}/Repository1"], variables)[0])
    expected = local_digest_map(source, "*.jpg")
    repository_initial = local_digest_map(repository, "*.jpg")
    workload = {
        "schema_version": 2,
        "generator": {"id": "external-input", "version": "unverified", "seed": None, "parameters": {}, "verified": False},
        "source_path": str(source),
        "repository_path": str(repository),
        "expected": [{"name": name, **expected[name]} for name in sorted(expected)],
        "repository_initial": [{"name": name, **repository_initial[name]} for name in sorted(repository_initial)],
        "repository_empty": not repository_initial,
        "verified": False,
        "reason": "fresh deterministic workload generation is not part of this increment",
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


def local_provenance(config: Mapping[str, str], plan: Mapping[str, Any], variables: Mapping[str, str]) -> dict[str, Any]:
    repo = Path(config["NG_LOCAL_REPO_PATH"])
    git = subprocess.run(["git", "-C", str(repo), "rev-parse", "HEAD"], capture_output=True, text=True, timeout=10, check=False)
    status_run = subprocess.run(["git", "-C", str(repo), "status", "--short"], capture_output=True, text=True, timeout=10, check=False)
    controller = Path(__file__).resolve()
    build_manifest_path = Path(config["NG_LOCAL_BUILD_MANIFEST"]).resolve() if config.get("NG_LOCAL_BUILD_MANIFEST") else None
    build_manifest_data: dict[str, Any] | None = None
    build_manifest_error: str | None = None
    if build_manifest_path and build_manifest_path.is_file():
        try:
            loaded = json.loads(build_manifest_path.read_text(encoding="utf-8"))
            build_manifest_data = loaded if isinstance(loaded, dict) else None
        except (OSError, json.JSONDecodeError) as exc:
            build_manifest_error = f"build manifest unreadable: {exc}"
    binaries: dict[str, Any] = {}
    for role in plan.get("roles", []):
        argv = expand_argv(role["command"], variables)
        resolved = Path(argv[0]) if Path(argv[0]).is_absolute() else Path(shutil.which(argv[0]) or argv[0])
        binaries[role["name"]] = {
            "argv0": argv[0],
            "resolved_path": str(resolved),
            "sha256": hashlib.sha256(resolved.read_bytes()).hexdigest() if resolved.is_file() else None,
        }
    build_linkage, linkage_error = validate_local_build_manifest(build_manifest_data, git.stdout.strip() if git.returncode == 0 else None, binaries)
    return {
        "schema_version": 2,
        "mode": "local",
        "git_head": git.stdout.strip() if git.returncode == 0 else None,
        "git_status": status_run.stdout.splitlines() if status_run.returncode == 0 else ["git status unavailable"],
        "git_clean": git.returncode == 0 and status_run.returncode == 0 and not status_run.stdout.strip(),
        "repo_path": str(repo.resolve()),
        "build_path": str(Path(config["NG_LOCAL_BUILD_PATH"]).resolve()),
        "io_path": str(Path(config["NG_LOCAL_IO_PATH"]).resolve()),
        "controller_identity": controller.is_file(),
        "controller": {"path": str(controller), "sha256": hashlib.sha256(controller.read_bytes()).hexdigest()},
        "build_linkage": build_linkage,
        "build_linkage_reason": build_manifest_error or linkage_error,
        "build_manifest": {"path": str(build_manifest_path), "sha256": hashlib.sha256(build_manifest_path.read_bytes()).hexdigest(), "valid": build_linkage} if build_manifest_path and build_manifest_path.is_file() else None,
        "plan_snapshot": False,
        "binaries": binaries,
    }


def local_preflight(config: Mapping[str, str], plan: Mapping[str, Any], variables: Mapping[str, str]) -> dict[str, Any]:
    errors: list[str] = []
    paths = {key: str(Path(config[key]).resolve()) for key in LOCAL_REQUIRED_ENV}
    for key in ("NG_LOCAL_REPO_PATH", "NG_LOCAL_BUILD_PATH", "NG_LOCAL_IO_PATH"):
        if not Path(paths[key]).is_dir():
            errors.append(f"{key} directory missing")
    roles = []
    for role in plan.get("roles", []):
        argv = expand_argv(role["command"], variables)
        executable = Path(argv[0]) if Path(argv[0]).is_absolute() else Path(shutil.which(argv[0]) or argv[0])
        cwd = Path(expand_argv([role["cwd"]], variables)[0]) if role.get("cwd") else None
        item = {"name": role["name"], "executable": str(executable), "executable_exists": executable.is_file(), "executable_mode": bool(executable.is_file() and os.access(executable, os.X_OK)), "cwd": str(cwd) if cwd else None, "cwd_exists": bool(cwd is None or cwd.is_dir())}
        roles.append(item)
        if not item["executable_exists"]:
            errors.append(f"{role['name']} executable missing")
        elif not item["executable_mode"]:
            errors.append(f"{role['name']} executable not executable")
        if not item["cwd_exists"]:
            errors.append(f"{role['name']} cwd missing")
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


def local_stop_process_group(proc: subprocess.Popen[Any], pgid: int, expected_starttime: str | None) -> dict[str, Any]:
    if proc.poll() is not None:
        group_scan = local_group_members_status(pgid)
        if not group_scan["complete"]:
            return {"ok": False, "result": "group-inventory-unavailable", "pid": proc.pid, "pgid": pgid, "residual": [{"pid": proc.pid, "pgid": pgid, "reason": "identity-unavailable"}]}
        if group_scan["members"]:
            return {"ok": False, "result": "group-member-residual", "pid": proc.pid, "pgid": pgid, "returncode": proc.returncode, "residual": group_scan["members"]}
        return {"ok": True, "result": "already-exited", "returncode": proc.returncode}
    record = read_proc_stat(proc.pid)
    if record["status"] != "ok" or record["starttime"] != expected_starttime or record["pgid"] != pgid:
        return {"ok": False, "result": "identity-mismatch", "pid": proc.pid, "pgid": pgid}
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
        record = read_proc_stat(proc.pid)
        if record["status"] != "ok" or record["starttime"] != expected_starttime or record["pgid"] != pgid:
            return {"ok": False, "result": "identity-mismatch-before-kill", "pid": proc.pid, "pgid": pgid}
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


def run_local_trial(args: argparse.Namespace) -> int:
    config = load_local_config(getattr(args, "env", None))
    plan, contract = load_plan_for_trial(Path(args.plan), args.scenario, args.debug_profile, mode="local")
    trial_id = validate_trial_id(args.trial or utc_id())
    evidence_root = Path(config["NG_LOCAL_EVIDENCE_PATH"]).resolve()
    evidence_dir = evidence_root / trial_id
    if evidence_dir.exists():
        raise ConfigError(f"local evidence directory already exists: {evidence_dir}")
    evidence_dir.mkdir(parents=True)
    events = evidence_dir / "controller-events.jsonl"
    local_record(events, "trial-start", trial_id=trial_id, scenario=args.scenario, debug_profile=args.debug_profile, contract_level=contract["level"], mode="local")
    variables = dict(config)
    variables["TRIAL_ID"] = trial_id
    overall_deadline = time.monotonic() + float(plan["timeouts"]["total"])
    local_record(events, "prepare", mode="local", evidence_dir=str(evidence_dir))
    provenance = local_provenance(config, plan, variables)
    workload = local_prepare_v2_layout(evidence_dir, plan, contract, config, variables)
    provenance["plan_snapshot"] = True
    provenance["workload_verified"] = bool(workload["verified"])
    provenance["evidence_schema_v2"] = True
    eligibility = local_acceptance_eligibility(provenance)
    provenance["acceptance_eligibility"] = eligibility
    (evidence_dir / "provenance.json").write_text(json.dumps(provenance, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    local_record(events, "provenance-written", provenance_path=str(evidence_dir / "provenance.json"), acceptance_eligibility=eligibility)
    processes: dict[str, dict[str, Any]] = {}
    before_ipc = local_ipc_snapshot()
    local_json_write(evidence_dir / "inventory" / "baseline.json", {"schema_version": 2, "processes": [], "ipc": {kind: (sorted(value) if isinstance(value, set) else None) for kind, value in before_ipc.items()}})
    runtime = "PASS"
    teardown_result = "PASS"
    evidence_result = "COMPLETE"
    interrupted = False
    offsets: dict[str, int] = {}
    carries: dict[str, str] = {}
    oracle_snapshot: dict[str, Any] = {"schema_version": 2, "type": (plan.get("runtime_oracle") or {}).get("type"), "preservation_verified": False}
    try:
        preflight = local_preflight(config, plan, variables)
        local_record(events, "preflight", result=preflight)
        if not preflight["ok"]:
            raise ConfigError("local preflight rejected: " + ", ".join(preflight["errors"]))
        for role in plan["roles"]:
            argv = expand_argv(role["command"], variables)
            cwd = expand_argv([role["cwd"]], variables)[0] if role.get("cwd") else None
            if cwd and not Path(cwd).is_dir():
                raise ConfigError(f"local cwd does not exist for {role['name']}: {cwd}")
            role_dir = evidence_dir / "roles" / role["name"]
            role_dir.mkdir(parents=True, exist_ok=True)
            stdout_path = role_dir / "stdout.log"
            stderr_path = role_dir / "stderr.log"
            stdout = stdout_path.open("w", encoding="utf-8")
            stderr = stderr_path.open("w", encoding="utf-8")
            role_env = os.environ.copy()
            role_env.update({key: expand_argv([value], variables)[0] for key, value in role.get("env", {}).items()})
            proc = subprocess.Popen(argv, cwd=cwd, env=role_env, stdout=stdout, stderr=stderr, start_new_session=True, text=True)
            pgid = os.getpgid(proc.pid)
            starttime = process_starttime(proc.pid)
            processes[role["name"]] = {"process": proc, "argv": argv, "pgid": pgid, "starttime": starttime, "tracked_descendants": {}, "descendant_scan_seen": False, "descendant_scan_complete": True, "stdout": stdout, "stderr": stderr, "stdout_path": stdout_path, "stderr_path": stderr_path}
            local_record(events, "launch", role=role["name"], pid=proc.pid, pgid=pgid, starttime=starttime, executable=argv[0], argv=argv, cwd=cwd)
            local_json_write(role_dir / "launch.json", {"role": role["name"], "pid": proc.pid, "pgid": pgid, "starttime": starttime, "executable": argv[0], "argv": argv, "cwd": cwd})
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
            oracle = plan.get("runtime_oracle")
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
            if proc.poll() is None:
                final_scan = local_process_descendants(proc.pid)
                item["descendant_scan_seen"] = True
                item["descendant_scan_complete"] = item.get("descendant_scan_complete", True) and final_scan["complete"]
                item.setdefault("tracked_descendants", {}).update({descendant["pid"]: descendant for descendant in final_scan["descendants"]})
            stop_result = local_stop_process_group(proc, pgid, item["starttime"])
            if not stop_result["ok"]:
                teardown_result = "FAIL"
            local_record(events, "stop", role=role, **stop_result)
            descendants_ok, descendant_residual = local_stop_tracked_descendants(item.get("tracked_descendants", {}))
            if not item.get("descendant_scan_seen", False) or not item.get("descendant_scan_complete", True) or not descendants_ok:
                teardown_result = "FAIL"
                local_record(events, "descendant-cleanup", role=role, complete=item.get("descendant_scan_complete", True), residual=descendant_residual)
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
            item["group_members_after_stop"] = members
            item["process"] = proc
            local_json_write(evidence_dir / "roles" / role / "exit.json", {"role": role, "pid": proc.pid, "returncode": proc.returncode, "group_members_after_stop": members})
            item["stdout"].close()
            item["stderr"].close()
        oracle = plan.get("runtime_oracle")
        if oracle and oracle.get("type") == "files":
            try:
                oracle_snapshot = preserve_local_artifacts(oracle, variables, evidence_dir)
                (evidence_dir / "oracle.json").write_text(json.dumps(oracle_snapshot, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
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
        local_json_write(evidence_dir / "ownership.json", {"schema_version": 2, "method": "creator-pid", "trial_pids": sorted(item["process"].pid for item in processes.values()), "ipc_new_ids": new_ipc, "attribution": "fail-closed" if ipc_reason else "creator-pid"})
        local_json_write(evidence_dir / "cleanup.json", {"schema_version": 2, "ordered_actions": ["stop", "evidence-preserve", "ipc-cleanup", "final-inventory"], "ipc_cleanup": ipc_ok, "ipc_reason": ipc_reason, "result": teardown_result})
        local_json_write(evidence_dir / "inventory" / "final.json", {"schema_version": 2, "processes": local_process_snapshot(processes), "ipc": {"new_ids": new_ipc, "cleanup_ok": ipc_ok, "cleanup_reason": ipc_reason}})
        if teardown_result == "PASS":
            local_record(events, "teardown", result="PASS")
        else:
            local_record(events, "teardown", result=teardown_result)
    code = 130 if interrupted and teardown_result == "PASS" and evidence_result == "COMPLETE" else classify_result(runtime, teardown_result, evidence_result)
    if not eligibility["eligible"] and code == 0:
        code = 21
    result = {"schema_version": 2, "trial_id": trial_id, "scenario": args.scenario, "debug_profile": args.debug_profile, "mode": "local", "runtime_result": runtime, "teardown_result": teardown_result, "evidence_result": evidence_result, "local_acceptance_eligible": eligibility["eligible"], "acceptance_blockers": eligibility["blockers"], "exit_code": code}
    result_path = evidence_dir / "result.json"
    result_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
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
        result_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
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
            result_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    except OSError as exc:
        evidence_result = "INCOMPLETE"
        teardown_result = "UNKNOWN"
        code = classify_result(runtime, teardown_result, evidence_result)
        result.update({"teardown_result": teardown_result, "evidence_result": evidence_result, "exit_code": code})
        result_path.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
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
    print(json.dumps({"ok": True, "operation": "dry-run", "mode": mode, "scenario": args.scenario, "debug_profile": args.debug_profile, "roles": [r["name"] for r in plan["roles"]], "contract_level": contract["level"], "config_keys": sorted(config)}, ensure_ascii=False, indent=2))
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
        paths = {key: str(Path(config[key]).resolve()) for key in LOCAL_REQUIRED_ENV}
        errors = [f"{key} path missing" for key, value in paths.items() if key != "NG_LOCAL_EVIDENCE_PATH" and not Path(value).exists()]
        if errors:
            raise ConfigError("local preflight rejected: " + ", ".join(errors))
        print(json.dumps({"ok": True, "operation": "preflight", "mode": "local", "scenario": args.scenario, "contract_level": contract["level"], "paths": paths}, ensure_ascii=False, indent=2))
        return 0
    config = load_config_from_env()
    plan, contract = load_plan_for_trial(Path(args.plan), args.scenario, args.debug_profile, mode="remote")
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
        sp.set_defaults(func=fn)
    run = sub.add_parser("run")
    run.add_argument("--plan", required=True)
    run.add_argument("--scenario", required=True, choices=sorted(SCENARIOS))
    run.add_argument("--mode", choices=("remote", "local"), default="remote")
    run.add_argument("--debug-profile", default="obs-normal")
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
