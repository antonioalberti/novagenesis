#!/usr/bin/env python3
"""Bounded process-group supervisor for NG test runners.

The command runs in its own process group. On timeout the supervisor sends
SIGTERM, waits a bounded grace period, then SIGKILLs the same process group.
The original command status is preserved unless timeout/escalation occurs.
"""
from __future__ import annotations

import argparse
import os
import signal
import subprocess
import sys
import time
from pathlib import Path


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--timeout", type=float, required=True)
    ap.add_argument("--term-grace", type=float, default=5.0)
    ap.add_argument("--log", type=Path)
    ap.add_argument("command", nargs=argparse.REMAINDER)
    args = ap.parse_args()
    if args.command and args.command[0] == "--":
        args.command = args.command[1:]
    if not args.command:
        ap.error("a command is required")
    if args.timeout <= 0 or args.term_grace < 0:
        ap.error("timeout must be positive and term-grace non-negative")

    log = args.log.open("w", buffering=1) if args.log else None
    started = time.monotonic()
    proc = subprocess.Popen(
        args.command,
        start_new_session=True,
        stdout=log or None,
        stderr=subprocess.STDOUT if log else None,
    )
    print(f"SUPERVISOR pid={proc.pid} pgid={os.getpgid(proc.pid)}", flush=True)
    timed_out = False
    escalated = False
    try:
        proc.wait(timeout=args.timeout)
    except subprocess.TimeoutExpired:
        timed_out = True
        pgid = os.getpgid(proc.pid)
        print(f"TIMEOUT elapsed={time.monotonic()-started:.3f}s signal=SIGTERM", flush=True)
        try:
            os.killpg(pgid, signal.SIGTERM)
        except ProcessLookupError:
            pass
        try:
            proc.wait(timeout=args.term_grace)
        except subprocess.TimeoutExpired:
            escalated = True
            print("TERM_GRACE_EXPIRED signal=SIGKILL", flush=True)
            try:
                os.killpg(pgid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            proc.wait()
    finally:
        if log:
            log.close()

    elapsed = time.monotonic() - started
    print(
        f"RESULT rc={proc.returncode} timed_out={int(timed_out)} "
        f"escalated={int(escalated)} elapsed={elapsed:.3f}s",
        flush=True,
    )
    if timed_out:
        return 124 if not escalated else 125
    return proc.returncode if proc.returncode is not None else 125


if __name__ == "__main__":
    sys.exit(main())
