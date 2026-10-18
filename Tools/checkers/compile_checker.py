#!/usr/bin/env python3
"""
Compile command checker for SPEC-039 D2.
Validates compile_commands.json against pinned compiler, grammar, and target census.
Usage: compile_checker.py <compile_commands.json> <profile> <worktree_root> <build_root>
Profile must be 'default' or 'diagnostic'.
Exits 0 on PASS, 2 on failure with error on stderr.
Designed to work identically under normal Python and PYTHONOPTIMIZE=1.
"""
import json
import os
import re
import shlex
import sys
from pathlib import Path
from typing import NoReturn

EXPECTED = {
    "diagnostic": {"Common", "ng039_gw_q0_q1_d2"},
    "default": {"Common", "ContentApp", "IoTTestApp", "NBTestApp", "PGCS", "NRNCS"},
}

COMPILER = os.path.realpath("/usr/bin/x86_64-linux-gnu-g++-13")
OBSERVER = "NG_GW_QUEUE_TEST_OBSERVATION"
REQUIRED = {
    "-O0", "-g", "-g3", "-Wall", "-fmessage-length=0",
    "-pthread", "-Wno-deprecated", "-lpthread", "-lrt", "-fno-omit-frame-pointer"
}
SOURCE_SUFFIXES = (".c", ".cc", ".cpp", ".cxx", ".c++", ".C")


def reject(message: str) -> NoReturn:
    raise ValueError(message)


def resolved(value: str, base: Path, strict: bool) -> Path:
    if not isinstance(value, str) or not value.strip():
        reject("empty path metadata")
    p = Path(value)
    if not p.is_absolute():
        p = base / p
    return p.resolve(strict=strict)


def under(path: Path, root: Path) -> bool:
    try:
        path.relative_to(root)
        return True
    except ValueError:
        return False


def tokens_for(entry: dict) -> list:
    if not isinstance(entry, dict):
        reject("entry must be object")
    if not isinstance(entry.get("directory"), str) or not entry["directory"]:
        reject("directory is empty")
    if not isinstance(entry.get("file"), str) or not entry["file"]:
        reject("file is empty")

    args = entry.get("arguments")
    command = entry.get("command")

    if args is not None and (
        not isinstance(args, list) or not args or any(not isinstance(x, str) for x in args)
    ):
        reject("arguments must be a nonempty string array")

    if command is not None:
        if not isinstance(command, str) or not command:
            reject("command must be nonempty string")
        if any(c in command for c in "$`;&|<>*?"):
            reject("shell/glob syntax is unsupported")
        try:
            parsed = shlex.split(command, posix=True)
        except ValueError:
            reject("malformed command quoting")
        if not parsed:
            reject("empty command")
        if args is not None and parsed != args:
            reject("command/arguments mismatch")
        if args is None:
            args = parsed

    if args is None:
        reject("entry has neither command nor arguments")
    return args


def parse_compile_arguments(argv: list) -> tuple:
    """Consume the closed §7 grammar; operands are never scanned as flags."""
    flags, sources, outputs, macros = [], [], [], []
    switches = ('-isystem', '-iquote', '-MF', '-MT', '-I', '-o', '-D', '-U')
    i = 1
    while i < len(argv):
        token = argv[i]
        if not token or any(ord(c) < 32 or ord(c) == 127 for c in token):
            reject('empty/control-character argument')
        if token in REQUIRED | {'-std=gnu++20', '-c', '-MD', '-MMD', '-MP'}:
            flags.append(token)
            i += 1
            continue
        switch = next((s for s in switches if token.startswith(s)), None)
        if switch is not None:
            value = token[len(switch):]
            if not value:
                i += 1
                if i >= len(argv):
                    reject('missing ' + switch + ' operand')
                value = argv[i]
            if (not value.strip() or value.startswith(('-', '@'))
                    or any(ord(c) < 32 or ord(c) == 127 for c in value)):
                reject('malformed ' + switch + ' operand')
            if switch in ('-D', '-U'):
                match = re.fullmatch(
                    r'([A-Za-z_][A-Za-z0-9_]*)(\((?:[A-Za-z_][A-Za-z0-9_]*(?:,[A-Za-z_][A-Za-z0-9_]*)*)?\))?(?:=(.*))?',
                    value,
                )
                if not match or (switch == '-U' and (match.group(2) or match.group(3) is not None)):
                    reject('malformed macro switch')
                if match.group(1) == OBSERVER:
                    macros.append((switch, match.group(2), match.group(3), value))
            elif switch == '-o':
                outputs.append(value)
            i += 1
            continue
        if token.startswith(('-', '@')) or not token.endswith(SOURCE_SUFFIXES):
            reject('unsupported compiler argument: ' + token)
        sources.append(token)
        i += 1
    if len(outputs) != 1:
        reject('exactly one -o output is required')
    return flags, sources, outputs[0], macros


def target_from_output(path: Path) -> str:
    parts = path.parts
    names = []
    for i, part in enumerate(parts[:-1]):
        if part == "CMakeFiles" and parts[i + 1].endswith(".dir"):
            names.append(parts[i + 1][:-4])
    if len(names) != 1:
        reject("target must derive uniquely from actual -o path")
    return names[0]


def validate_compile_database(data: list, profile: str, worktree_root: str, build_root: str) -> dict:
    if profile not in EXPECTED:
        reject("unknown profile")
    if not isinstance(data, list) or not data:
        reject("compile database must be nonempty array")

    source_root = Path(worktree_root).resolve(strict=True)
    build = Path(build_root).resolve(strict=True)

    if not source_root.is_dir() or not build.is_dir():
        reject("trusted roots must be existing directories")
    if under(source_root, build) or under(build, source_root):
        reject("source/build roots must be disjoint")

    seen = set()
    pairs = set()
    counts = {}

    for entry in data:
        argv = tokens_for(entry)

        if os.path.realpath(argv[0]) != COMPILER:
            reject("compiler realpath mismatch")

        flags, source_args, output, macros = parse_compile_arguments(argv)

        directory = resolved(entry["directory"], Path.cwd(), strict=True)
        if not directory.is_dir() or not under(directory, build):
            reject("compile directory escapes build root")

        source = resolved(entry["file"], directory, strict=True)
        if not source.is_file() or not under(source, source_root):
            reject("source escapes pinned worktree")

        if len(source_args) != 1 or resolved(source_args[0], directory, strict=True) != source:
            reject("exactly one source operand must match entry file")

        if flags.count("-c") != 1:
            reject("exactly one -c is required")

        out = resolved(output, directory, strict=False)
        if not under(out, build):
            reject("actual -o output escapes build root")

        metadata = entry.get("output")
        if metadata is not None:
            meta = resolved(metadata, directory, strict=False)
            if meta != out:
                reject("output metadata disagrees with actual -o")

        target = target_from_output(out)
        if target not in EXPECTED[profile]:
            reject("unexpected target derived from actual output")

        standards = [x for x in flags if x.startswith("-std=")]
        if standards != ["-std=gnu++20"]:
            reject("effective language mode must be exactly gnu++20")

        optimizations = [x for x in flags if x.startswith("-O")]
        if not optimizations or any(x != "-O0" for x in optimizations):
            reject("effective optimization must be -O0")

        if not REQUIRED.issubset(set(flags)):
            reject("required pinned project/debug flags missing")

        if profile == "default" and macros:
            reject("observer macro defined or undefined in default profile")
        if profile == "diagnostic" and target in {"Common", "ng039_gw_q0_q1_d2"}:
            if macros != [("-D", None, "1", OBSERVER + "=1")]:
                reject("diagnostic observer must be exactly one object-like =1 define")

        pair = (target, str(out))
        if pair in pairs:
            reject("duplicate target/output compile entry")
        pairs.add(pair)
        seen.add(target)
        counts[target] = counts.get(target, 0) + 1

    if seen != EXPECTED[profile]:
        reject("target census mismatch")

    return {
        "profile": profile,
        "targets": sorted(seen),
        "target_counts": counts,
        "entry_count": len(data),
    }


if __name__ == "__main__":
    try:
        with open(sys.argv[1], encoding="utf-8") as f:
            db = json.load(f)
        print(
            json.dumps(
                validate_compile_database(db, sys.argv[2], sys.argv[3], sys.argv[4]),
                sort_keys=True,
            )
        )
    except Exception as exc:
        print("compile check failed: " + str(exc), file=sys.stderr)
        raise SystemExit(2)