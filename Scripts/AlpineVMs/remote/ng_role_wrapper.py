#!/usr/bin/env python3
"""Keep a trial role anchored while its command and descendants run."""
from __future__ import annotations

import os
import signal
import subprocess
import sys
import time


child: subprocess.Popen[bytes] | None = None


def on_signal(signum: int, _frame: object) -> None:
    if child is not None and child.poll() is None:
        try:
            child.send_signal(signum)
        except ProcessLookupError:
            pass


def group_members() -> list[int]:
    members = []
    try:
        entries = os.listdir("/proc")
    except OSError:
        return members
    for entry in entries:
        if not entry.isdigit():
            continue
        try:
            raw = open(f"/proc/{entry}/stat", encoding="utf-8").read()
            close = raw.rfind(")")
            fields = raw[close + 2 :].split()
            if int(fields[2]) == os.getpgrp() and int(entry) != os.getpid():
                members.append(int(entry))
        except (OSError, ValueError, IndexError):
            pass
    return sorted(members)


def main() -> int:
    command = sys.argv[1:]
    if command and command[0] == "--":
        command = command[1:]
    if not command:
        print("command is required", file=sys.stderr)
        return 2
    signal.signal(signal.SIGTERM, on_signal)
    signal.signal(signal.SIGINT, on_signal)
    global child
    child = subprocess.Popen(command, stdin=subprocess.DEVNULL)
    while child.poll() is None:
        time.sleep(0.05)
    rc = child.returncode
    # Keep the anchor alive while any descendant remains in this trial group.
    # The controller can therefore validate the anchor before SIGKILL.
    while group_members():
        time.sleep(0.05)
    return int(rc if rc is not None else 1)


if __name__ == "__main__":
    raise SystemExit(main())
