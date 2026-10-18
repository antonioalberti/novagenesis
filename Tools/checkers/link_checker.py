#!/usr/bin/env python3
"""Link command checker for SPEC-039 D2.
Validates link.txt against the diagnostic selector and basic link command structure.
Usage: link_checker.py <link.txt> <build_root> <target>
Exits 0 on PASS, 2 on failure with error on stderr.
Designed to work identically under normal Python and PYTHONOPTIMIZE=1.
"""
import hashlib
import os
import shlex
import sys
from pathlib import Path

COMPILER = os.path.realpath("/usr/bin/x86_64-linux-gnu-g++-13")

def reject(message: str) -> None:
    raise ValueError(message)

def validate_link_file(link_file: str, build_root: str, target: str) -> dict:
    root = Path(build_root).resolve(strict=True)
    # Expected link.txt path (may not exist)
    expected = (root / "CMakeFiles" / (target + ".dir")) / "link.txt"
    supplied_resolved = None
    try:
        expected_resolved = expected.resolve(strict=True)
    except FileNotFoundError:
        expected_resolved = None  # We'll compare paths without resolving expected
    supplied = Path(link_file)
    # First check that the supplied path matches the expected path (without requiring existence)
    try:
        supplied_resolved = supplied.resolve(strict=False)
    except Exception:
        reject(f"invalid link.txt path: {link_file}")
    if supplied_resolved is None:
        reject("invalid link.txt path: {link_file}")
    if expected_resolved is not None:
        if supplied_resolved != expected_resolved:
            reject("link.txt path is not the expected target file")
    else:
        # If expected does not exist, we still want to ensure the supplied path is under the build root
        # and has the correct relative path.
        try:
            if not supplied_resolved.is_relative_to(root):
                reject("link.txt escapes build root")
            # Check relative path
            rel_path = supplied_resolved.relative_to(root)
            expected_rel = Path("CMakeFiles") / (target + ".dir") / "link.txt"
            if rel_path != expected_rel:
                reject("link.txt path is not the expected target file")
        except Exception:
            reject("link.txt path is not the expected target file")
    # Now we know the file exists and is a regular file (not a symlink) and exists
    if supplied.is_symlink() or not supplied.is_file():
        reject("link.txt must be a regular non-symlink file")
    # Now we know the file exists and is a regular file, so we can resolve it safely
    actual = supplied.resolve(strict=True)
    try:
        actual.relative_to(root)
    except ValueError:
        reject("link.txt escapes build root")
    content = actual.read_text(encoding="utf-8")
    try:
        argv = shlex.split(content, posix=True)
    except ValueError:
        reject("malformed link command quoting")
    if not argv:
        reject("empty link command")
    # Validate the compiler (first argument)
    compiler_path = os.path.realpath(argv[0])
    if compiler_path != COMPILER:
        reject("linker compiler identity mismatch")
    if any(x.startswith("@") for x in argv):
        reject("response files unsupported")
    # We will parse the argv into a list of (flag, operand) pairs, starting from index 1
    # because index 0 is the compiler and we've already validated it.
    i = 1
    args = []
    while i < len(argv):
        arg = argv[i]
        if arg.startswith("-"):
            # Check if it's a flag that takes an operand
            if arg in ("-o", "-L", "-l", "-Wl"):
                if i + 1 >= len(argv):
                    reject(f"flag {arg} requires an operand")
                operand = argv[i+1]
                args.append((arg, operand))
                i += 2
            else:
                # Flag without operand
                args.append((arg, None))
                i += 1
        else:
            # Non-flag argument (e.g., object file, library, linker script)
            args.append(("", arg))
            i += 1
    # Now validate the parsed args.
    seen_output = False
    seen_wl_bdynamic = False
    seen_lc = False
    output_operand = None
    input_files = []
    # Forbidden flags dictionary
    forbidden = {
        "-static": "standalone static link",
        "-static-libgcc": "static libgcc",
        "-static-libstdc++": "static libstdc++",
        "-shared": "shared library",
        "-dynamic": "dynamic linker",
        "-Bdynamic": "direct Bdynamic (must use -Wl,-Bdynamic)",
        "-pie": "position independent executable",
        "-static-pie": "static position independent executable",
        "-rdynamic": "runtime dynamic symbols",
        "--dynamic-linker": "explicit dynamic linker",
        "-Xlinker": "forwarded linker control -Xlinker unsupported",
    }
    for flag, operand in args:
        # First check for forbidden flags
        if flag in forbidden:
            reject(f"forbidden flag {flag}: {forbidden[flag]}")
        if flag == "-o":
            if seen_output:
                reject("duplicate -o flag")
            seen_output = True
            output_operand = operand
        elif flag == "-l":
            # We allow any -l flag, but we will check for -lc separately
            if operand == "c":
                seen_lc = True
        elif flag == "-Wl":
            # Parse the operand as comma-separated options
            options = operand.split(",")
            for opt in options:
                if opt == "-Bdynamic":
                    seen_wl_bdynamic = True
                else:
                    reject(f"unsupported linker option via -Wl: {opt}")
        elif flag == "":
            # Non-flag argument: treat as input file
            input_files.append(operand)
        else:
            # Unknown flag (not forbidden and not recognized)
            reject(f"unsupported flag: {flag}")
    # Additional checks
    if not seen_output:
        reject("missing -o flag")
    if not seen_wl_bdynamic:
        reject("missing -Wl,-Bdynamic")
    if not seen_lc:
        reject("missing -lc")
    # Validate output operand: must be under build root, not a symlink
    if output_operand is not None:
        out_path = Path(output_operand)
        if not out_path.is_absolute():
            out_path = root / out_path
        out_path = out_path.resolve(strict=False)
        if not out_path.is_relative_to(root):
            reject(f"output escapes build root: {output_operand}")
        if out_path.is_symlink():
            reject("output must not be a symlink")
    # Validate input files: must exist, be regular .o files, under build root, no symlink traversal
    if not input_files:
        reject("no input files provided")
    for inp in input_files:
        try:
            in_path = Path(inp)
            if not in_path.is_absolute():
                in_path = root / in_path
            in_path = in_path.resolve(strict=True)
            # Now perform all checks that depend on in_path
            if not in_path.is_relative_to(root):
                reject(f"input file escapes build root: {inp}")
            if not in_path.is_file():
                reject(f"input file does not exist or is not a regular file: {inp}")
            if in_path.is_symlink():
                reject(f"input file must not be a symlink: {inp}")
            if in_path.suffix != ".o":
                reject(f"input file must have .o extension: {inp}")
        except FileNotFoundError:
            reject(f"input file does not exist or is not a regular file: {inp}")
        except OSError:
            reject(f"invalid input file path: {inp}")
    return {
        "status": "PASS",
        "link_file_sha256": hashlib.sha256(actual.read_bytes()).hexdigest(),
        "compiler": argv[0],
    }

if __name__ == "__main__":
    try:
        print(validate_link_file(sys.argv[1], sys.argv[2], sys.argv[3]))
    except Exception as exc:
        print("link check failed: " + str(exc), file=sys.stderr)
        raise SystemExit(2)