#!/usr/bin/env python3
"""CMake compiler launcher for exact, isolated debug selections."""
from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path
from typing import Any


def load_profile(path: Path) -> dict[str, Any]:
    profile = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(profile, dict) or not isinstance(profile.get("defines", []), list):
        raise ValueError("invalid debug profile")
    return profile


def source_argument(command: list[str]) -> str | None:
    for arg in command[1:]:
        if arg.endswith((".c", ".cc", ".cpp", ".cxx")):
            return arg
    return None


def command_for_source(command: list[str], profile: dict[str, Any], source_root: Path) -> list[str]:
    if not command:
        raise ValueError("empty compiler command")
    source = source_argument(command)
    if source is None:
        return command
    source_path = Path(source)
    if not source_path.is_absolute():
        source_path = (Path.cwd() / source_path).resolve()
    else:
        source_path = source_path.resolve()
    try:
        relative = source_path.relative_to(source_root.resolve()).as_posix()
    except ValueError:
        return command
    additions = []
    for item in profile.get("defines", []):
        if item.get("file") == relative:
            macro = item.get("macro")
            if not isinstance(macro, str) or not macro.isidentifier():
                raise ValueError(f"invalid macro: {macro!r}")
            flag = f"-D{macro}"
            if flag not in command:
                additions.append(flag)
    return [command[0], *additions, *command[1:]]


def audit_invocation(command: list[str], selected: list[str], source_root: Path) -> None:
    audit_path = os.environ.get("NG_DEBUG_LAUNCH_LOG")
    if not audit_path:
        return
    source = source_argument(command)
    record = {
        "source": str(Path(source).resolve()) if source else None,
        "injected": [arg for arg in selected[1:] if arg.startswith("-D") and arg not in command],
        "compiler": command[0] if command else None,
    }
    path = Path(audit_path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as fh:
        fh.write(json.dumps(record, ensure_ascii=False) + "\n")


def main(argv: list[str] | None = None) -> int:
    command = list(sys.argv[1:] if argv is None else argv)
    profile_path = os.environ.get("NG_DEBUG_PROFILE_FILE")
    source_root = os.environ.get("NG_DEBUG_SOURCE_ROOT")
    if not profile_path or not source_root:
        print("NG_DEBUG_PROFILE_FILE and NG_DEBUG_SOURCE_ROOT are required", file=sys.stderr)
        return 2
    try:
        profile = load_profile(Path(profile_path))
        selected = command_for_source(command, profile, Path(source_root))
        audit_invocation(command, selected, Path(source_root))
        return subprocess.run(selected, check=False).returncode
    except (OSError, ValueError, subprocess.SubprocessError) as exc:
        print(f"DEBUG_LAUNCHER_ERROR: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
