#!/usr/bin/env python3
"""Inventory and assess NovaGenesis runtime observability without C++ edits."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import subprocess
import shutil
from pathlib import Path
from typing import Any, Mapping


DEBUG_RE = re.compile(r"^\s*#\s*define\s+(DEBUG[A-Z0-9_]*)\b")
COMMENTED_DEBUG_RE = re.compile(r"^\s*//+\s*#\s*define\s+(DEBUG[A-Z0-9_]*)\b")
GUARD_RE = re.compile(r"^\s*#\s*(?:ifdef\s+|ifndef\s+|if\s+defined\s*\(\s*|if\s+)(DEBUG[A-Z0-9_]*)")
SINK_RE = re.compile(r"\b(?:cerr|cout|clog|printf|fprintf|perror|LOG:|WARNING|ERROR|ALARM|FATAL)\b|\b(?:S|Debug|DEBUG)\s*<<")
FORBIDDEN = {
    ("PGCS/src/PG.cpp", "DEBUG"),
    ("Common/src/GW.cpp", "DEBUG"),
    ("Common/src/Process.cpp", "DEBUG"),
    ("Common/src/Block.cpp", "DEBUG"),
    ("Common/src/Message.cpp", "DEBUG"),
    ("PGCS/src/PG.cpp", "DEBUG2"),
    ("PGCS/src/PG.cpp", "DEBUG3"),
    ("PGCS/src/PG.cpp", "DEBUG5"),
    ("PGCS/src/PG.cpp", "DEBUG6"),
    ("*", "DEBUG2"), ("*", "DEBUG3"), ("*", "DEBUG5"), ("*", "DEBUG6"),
    ("*", "DEBUG_NETWORK_QUEUE"),
}


def strip_c_comments(line: str, in_block: bool) -> tuple[str, bool]:
    output = []
    i = 0
    while i < len(line):
        if in_block:
            end = line.find("*/", i)
            if end < 0:
                return "".join(output), True
            i = end + 2
            in_block = False
        elif line.startswith("//", i):
            break
        elif line.startswith("/*", i):
            in_block = True
            i += 2
        else:
            output.append(line[i])
            i += 1
    return "".join(output), in_block


def build_inventory(root: Path) -> dict[str, Any]:
    files = []
    excluded = {".git", "build", "cmake-build-debug", "cmake-build-sanitizer", "cmake-build-relwithdebinfo", "rapidjson", "third_party", "vendor"}
    for path in sorted(root.rglob("*")):
        if path.suffix not in {".cpp", ".h"} or not path.is_file() or any(part in excluded for part in path.relative_to(root).parts):
            continue
        text = path.read_text(errors="replace")
        active, commented, guards, sinks = [], [], [], []
        in_block = False
        for lineno, line in enumerate(text.splitlines(), 1):
            code, in_block = strip_c_comments(line, in_block)
            commented_match = COMMENTED_DEBUG_RE.search(line)
            active_match = DEBUG_RE.search(code)
            if commented_match:
                commented.append({"macro": commented_match.group(1), "line": lineno})
            elif active_match:
                active.append({"macro": active_match.group(1), "line": lineno})
            match = GUARD_RE.search(code)
            if match:
                guards.append({"macro": match.group(1), "line": lineno})
            if SINK_RE.search(code):
                sinks.append({"line": lineno, "text": code.strip()[:240]})
        if active or commented or guards or sinks:
            files.append({
                "path": str(path.relative_to(root)),
                "source_sha256": hashlib.sha256(text.encode()).hexdigest(),
                "active_defines": active,
                "commented_defines": commented,
                "guards": guards,
                "effective": "unknown",
                "compiled": "unknown",
                "runtime_reachable": "unknown",
                "observed": "unknown",
                "log_sink_count": len(sinks),
                "sinks": sinks,
            })
    return {"schema_version": 1, "root": str(root.resolve()), "files": files, "file_count": len(files), "status": "partial", "exclusions": sorted(excluded)}


def validate_profile(profile: dict[str, Any], inventory: dict[str, Any]) -> None:
    if not isinstance(profile, dict) or profile.get("schema_version") != 1 or not isinstance(profile.get("id"), str):
        raise ValueError("profile requires schema_version 1 and id")
    defines = profile.get("defines", [])
    if not isinstance(defines, list):
        raise ValueError("profile defines must be an array")
    rows = {row["path"]: row for row in inventory.get("files", [])}
    for item in defines:
        if not isinstance(item, dict) or not isinstance(item.get("file"), str) or not isinstance(item.get("macro"), str):
            raise ValueError("profile defines must contain file and macro")
        pair = (item["file"], item["macro"])
        if pair in FORBIDDEN or ("*", item["macro"]) in FORBIDDEN:
            raise ValueError(f"forbidden broad debug selection: {item['file']}:{item['macro']}")
        if rows and item["file"] not in rows:
            raise ValueError(f"profile file not present in inventory: {item['file']}")
        if rows:
            row = rows[item["file"]]
            macros = {x["macro"] for x in row["active_defines"] + row["commented_defines"] + row["guards"]}
            if item["macro"] not in macros:
                raise ValueError(f"profile macro not present in source: {item['file']}:{item['macro']}")
            if item["macro"] in {x["macro"] for x in row["active_defines"]}:
                raise ValueError(f"profile cannot override active source macro: {item['file']}:{item['macro']}")
    if profile.get("variant") == "verbose-diagnostic" and len(defines) > 2:
        raise ValueError("verbose-diagnostic is limited to two targeted selections")


def assess_lines(lines: list[str], gates: list[dict[str, str]]) -> dict[str, Any]:
    results = []
    for gate in gates:
        gid, pattern = gate.get("id"), gate.get("pattern")
        if not isinstance(gid, str) or not isinstance(pattern, str):
            raise ValueError("gate requires id and pattern")
        try:
            compiled = re.compile(pattern)
        except re.error as exc:
            raise ValueError(f"invalid gate pattern {gid}: {exc}") from exc
        match_line = next((i for i, line in enumerate(lines, 1) if compiled.search(line)), None)
        results.append({"id": gid, "pattern": pattern, "result": "OBSERVED" if match_line else "INCONCLUSIVE", "line": match_line})
    return {"schema_version": 1, "gates": results}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as fh:
        for chunk in iter(lambda: fh.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def git_index_snapshot(root: Path) -> dict[str, Any]:
    """Capture the staged index identity used by the build manifest."""
    try:
        run = subprocess.run(["git", "ls-files", "--stage", "-z"], cwd=root, capture_output=True, check=False)
        if run.returncode != 0:
            return {"captured": False, "entries": [], "errors": ["git index unavailable"]}
        entries: list[dict[str, Any]] = []
        canonical: list[str] = []
        for raw in run.stdout.split(b"\0"):
            if not raw:
                continue
            header, relative_bytes = raw.split(b"\t", 1)
            mode, blob_id, stage = header.decode("ascii").split()
            relative = relative_bytes.decode("utf-8")
            entry = {"path": relative, "mode": mode, "blob_id": blob_id.lower(), "stage": int(stage)}
            entries.append(entry)
            canonical.append(f"{mode} {blob_id.lower()} {stage}\t{relative}\n")
        digest = hashlib.sha256("".join(sorted(canonical)).encode("utf-8")).hexdigest()
        return {"captured": True, "entries": sorted(entries, key=lambda item: (item["path"], item["stage"])), "sha256": digest, "errors": []}
    except (OSError, UnicodeDecodeError, ValueError):
        return {"captured": False, "entries": [], "errors": ["git index unavailable"]}


def git_snapshot(root: Path) -> dict[str, Any]:
    try:
        head = subprocess.run(["git", "rev-parse", "HEAD"], cwd=root, capture_output=True, text=True, check=True).stdout.strip()
        status = subprocess.run(["git", "status", "--short"], cwd=root, capture_output=True, text=True, check=True).stdout.splitlines()
        submodule = subprocess.run(["git", "submodule", "status", "--recursive"], cwd=root, capture_output=True, text=True, check=False)
        index = git_index_snapshot(root)
        return {
            "head": head,
            "dirty": bool(status),
            "status": status,
            "tree_sha256": tree_sha256(root),
            "submodules": submodule.stdout.splitlines() if submodule.returncode == 0 else ["git submodule status unavailable"],
            "submodules_complete": submodule.returncode == 0,
            "index": index,
            "index_sha256": index.get("sha256"),
            "index_entries": index.get("entries", []),
            "content_snapshot": {"captured": not status, "files": [], "entries": [], "errors": [] if not status else ["dirty source archive not captured"]},
        }
    except (OSError, subprocess.CalledProcessError):
        return {"head": "unknown", "dirty": True, "status": ["not-a-git-tree"], "tree_sha256": tree_sha256(root), "submodules": ["git snapshot unavailable"], "submodules_complete": False, "index": {"captured": False, "entries": [], "errors": ["git index unavailable"]}, "index_sha256": None, "index_entries": [], "content_snapshot": {"captured": False, "files": [], "entries": [], "errors": ["git snapshot unavailable"]}}


def tree_sha256(root: Path) -> str:
    digest = hashlib.sha256()
    excluded = {".git", "build", "cmake-build-debug", "cmake-build-sanitizer", "cmake-build-relwithdebinfo", "__pycache__"}
    for path in sorted(root.rglob("*")):
        if not path.is_file() or any(part in excluded for part in path.relative_to(root).parts):
            continue
        rel = path.relative_to(root).as_posix().encode()
        digest.update(rel + b"\0")
        digest.update(path.read_bytes())
        digest.update(b"\0")
    return digest.hexdigest()


def _normalise_loader_output(output: str) -> str:
    return re.sub(r"0x[0-9a-fA-F]+", "0xADDR", output)


def _identity_command(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, capture_output=True, text=True, timeout=10, check=False)


def _identity_output(run: subprocess.CompletedProcess[str]) -> str:
    return (run.stdout or "") + (run.stderr or "")


def _static_ldd_result(run: subprocess.CompletedProcess[str], output: str) -> bool:
    """Recognise only the explicit non-dynamic-loader result."""
    return run.returncode != 0 and output.strip().lower().endswith("not a dynamic executable")


def _static_elf_identity(path: str, binary_sha256: str, ldd_run: subprocess.CompletedProcess[str], ldd_output: str) -> dict[str, Any] | None:
    file_run = _identity_command(["file", "-b", path])
    header_run = _identity_command(["readelf", "-h", path])
    program_run = _identity_command(["readelf", "-l", path])
    file_output = _identity_output(file_run)
    header_output = _identity_output(header_run)
    program_output = _identity_output(program_run)
    file_static = "statically linked" in file_output.lower() or "static-pie linked" in file_output.lower()
    elf_header_valid = header_run.returncode == 0 and "ELF Header:" in header_output
    has_interpreter = "INTERP" in program_output
    elf_static = program_run.returncode == 0 and not has_interpreter
    if not (file_run.returncode == 0 and file_output.lstrip().startswith("ELF") and file_static and elf_header_valid and elf_static):
        return None
    if not _static_ldd_result(ldd_run, ldd_output):
        return None
    return {
        "status": "ok",
        "linkage": "static",
        "binary_path": str(Path(path).resolve()),
        "binary_sha256": binary_sha256,
        "file_output": file_output,
        "file_output_sha256": hashlib.sha256(file_output.encode()).hexdigest(),
        "elf_header_output": header_output,
        "elf_header_sha256": hashlib.sha256(header_output.encode()).hexdigest(),
        "elf_program_headers_output": program_output,
        "elf_program_headers_sha256": hashlib.sha256(program_output.encode()).hexdigest(),
        "ldd_returncode": ldd_run.returncode,
        "ldd_output": ldd_output,
        "ldd_sha256": hashlib.sha256(ldd_output.encode()).hexdigest(),
        "libraries": [],
        "library_count": 0,
    }


def runtime_library_identity(binaries: dict[str, dict[str, Any]]) -> dict[str, Any]:
    """Record dynamic ``ldd`` or independently proven static ELF identity."""
    records: dict[str, Any] = {}
    methods: set[str] = set()
    for name, binary in binaries.items():
        path = str(Path(binary["path"]).resolve())
        binary_sha256 = binary["sha256"]
        try:
            ldd_run = _identity_command(["ldd", path])
            ldd_output = _identity_output(ldd_run)
            static_record = _static_elf_identity(path, binary_sha256, ldd_run, ldd_output)
        except (OSError, subprocess.SubprocessError):
            static_record = None
            ldd_run = None
            ldd_output = ""
        if static_record is not None:
            records[name] = static_record
            methods.add("static")
            continue
        if ldd_run is None:
            records[name] = {"status": "unavailable", "linkage": "dynamic", "binary_path": path, "binary_sha256": binary_sha256, "libraries": []}
            methods.add("ldd")
            continue
        stable_output = _normalise_loader_output(ldd_output)
        records[name] = {
            "status": "ok" if ldd_run.returncode == 0 else "unavailable",
            "linkage": "dynamic",
            "binary_path": path,
            "output_sha256": hashlib.sha256(stable_output.encode()).hexdigest(),
            "binary_sha256": binary_sha256,
            "libraries": sorted(line.strip() for line in stable_output.splitlines() if line.strip()),
            "library_count": len([line for line in stable_output.splitlines() if line.strip()]),
        }
        methods.add("ldd")
    method = next(iter(methods)) if len(methods) == 1 else "mixed"
    return {"method": method, "binaries": runtime_identity_for_roles(records)}


def runtime_identity_for_roles(records: Mapping[str, Mapping[str, Any]]) -> dict[str, dict[str, Any]]:
    """Expose runtime-library identity under every local-plan role name."""
    identities = {str(name): dict(record) for name, record in records.items()}
    contentapp = identities.get("ContentApp")
    if contentapp is not None:
        identities.setdefault("Repository", dict(contentapp))
        identities.setdefault("Source", dict(contentapp))
    return identities


def toolchain_identity() -> dict[str, Any]:
    compiler = os.environ.get("CXX", "c++")
    compiler_path = shutil.which(compiler) if not Path(compiler).is_absolute() else compiler
    resolved = str(Path(compiler_path).resolve()) if compiler_path else compiler
    run = subprocess.run([resolved, "--version"], capture_output=True, text=True, check=False)
    output = run.stdout or run.stderr
    version = output.splitlines()[0] if output else "unavailable"
    return {"compiler": resolved, "version": version, "version_sha256": hashlib.sha256(version.encode()).hexdigest(), "status": "ok" if run.returncode == 0 else "unavailable"}


def manifest_outputs_for_roles(binaries: Mapping[str, Mapping[str, Any]]) -> dict[str, dict[str, Any]]:
    """Expose build outputs under every role name used by the local plan."""
    outputs = {str(name): dict(record) for name, record in binaries.items()}
    contentapp = outputs.get("ContentApp")
    if contentapp is not None:
        outputs.setdefault("Repository", dict(contentapp))
        outputs.setdefault("Source", dict(contentapp))
    return outputs


def build_variant(source: Path, profile_path: Path, variant: str, output: Path, jobs: int = 1, configure_only: bool = False) -> dict[str, Any]:
    inventory = build_inventory(source)
    profile = json.loads(profile_path.read_text(encoding="utf-8"))
    validate_profile(profile, inventory)
    if variant not in {"normal", "targeted-debug", "verbose-diagnostic"}:
        raise ValueError(f"unknown build variant: {variant}")
    if variant == "normal" and profile.get("defines"):
        raise ValueError("normal build cannot contain debug defines")
    output = output.resolve()
    if output == source.resolve() or source.resolve() in output.parents:
        raise ValueError("isolated build output must be outside the source tree")
    if output.exists() and any(output.iterdir()):
        raise ValueError("isolated build output must be a new empty directory")
    output.mkdir(parents=True, exist_ok=True)
    launcher = Path(__file__).with_name("ng_debug_compiler_launcher.py").resolve()
    env = os.environ.copy()
    cmake_args = ["cmake", "-S", str(source.resolve()), "-B", str(output), "-DCMAKE_BUILD_TYPE=Debug", "-DNG_ENABLE_LEGACY_STANDALONE=OFF", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"]
    if variant != "normal":
        env["NG_DEBUG_PROFILE_FILE"] = str(profile_path.resolve())
        env["NG_DEBUG_SOURCE_ROOT"] = str(source.resolve())
        env["NG_DEBUG_LAUNCH_LOG"] = str(output / "debug-launcher.jsonl")
        cmake_args.append(f"-DCMAKE_CXX_COMPILER_LAUNCHER={launcher}")
    commands = [cmake_args]
    returncodes = []
    configure = subprocess.run(cmake_args, cwd=source, env=env, capture_output=True, text=True, check=False)
    returncodes.append(configure.returncode)
    (output / "configure.stdout.log").write_text(configure.stdout, encoding="utf-8")
    (output / "configure.stderr.log").write_text(configure.stderr, encoding="utf-8")
    if configure.returncode != 0:
        raise RuntimeError(f"cmake configure failed; see {output / 'configure.stderr.log'}")
    if not configure_only:
        build_cmd = ["cmake", "--build", str(output), "--parallel", str(max(1, jobs))]
        commands.append(build_cmd)
        build = subprocess.run(build_cmd, cwd=source, env=env, capture_output=True, text=True, check=False)
        returncodes.append(build.returncode)
        (output / "build.stdout.log").write_text(build.stdout, encoding="utf-8")
        (output / "build.stderr.log").write_text(build.stderr, encoding="utf-8")
        if build.returncode != 0:
            raise RuntimeError(f"cmake build failed; see {output / 'build.stderr.log'}")
    binaries = {}
    for name in ("PGCS", "NRNCS", "ContentApp", "NBTestApp", "IoTTestApp"):
        path = output / name
        if path.is_file():
            binaries[name] = {"path": str(path), "size": path.stat().st_size, "sha256": sha256(path)}
    effective_debug = []
    launch_log = output / "debug-launcher.jsonl"
    if launch_log.is_file():
        effective_debug = [json.loads(line) for line in launch_log.read_text(encoding="utf-8").splitlines() if line.strip()]
    receipt_commands = []
    for command, returncode, stdout_name, stderr_name in zip(
        commands,
        returncodes,
        ("configure.stdout.log", "build.stdout.log"),
        ("configure.stderr.log", "build.stderr.log"),
    ):
        stdout_path = output / stdout_name
        stderr_path = output / stderr_name
        stdout_bytes = stdout_path.read_bytes() if stdout_path.is_file() else b""
        stderr_bytes = stderr_path.read_bytes() if stderr_path.is_file() else b""
        captured = {
            "argv": command,
            "cwd": str(source.resolve()),
            "returncode": returncode,
            "stdout_sha256": hashlib.sha256(stdout_bytes).hexdigest(),
            "stderr_sha256": hashlib.sha256(stderr_bytes).hexdigest(),
        }
        captured["command_sha256"] = hashlib.sha256(
            json.dumps(captured, sort_keys=True, separators=(",", ":")).encode("utf-8")
        ).hexdigest()
        receipt_commands.append(captured)
    receipt = {
        "schema_version": 1,
        "working_directory": str(source.resolve()),
        "output_directory": str(output),
        "commands": receipt_commands,
        "outputs": {
            name: {"path": record["path"], "size": record["size"], "sha256": record["sha256"]}
            for name, record in manifest_outputs_for_roles(binaries).items()
        },
    }
    receipt["receipt_sha256"] = hashlib.sha256(
        json.dumps({key: value for key, value in receipt.items() if key != "receipt_sha256"}, sort_keys=True, separators=(",", ":")).encode("utf-8")
    ).hexdigest()
    manifest = {
        "schema_version": 1,
        "variant": variant,
        "profile": profile,
        "source": str(source.resolve()),
        "source_snapshot": git_snapshot(source),
        "output": str(output),
        "recipe": {"commands": commands, "working_directory": str(source.resolve()), "executed": True, "returncodes": returncodes},
        "commands": commands,
        "build_receipt": receipt,
        "toolchain": toolchain_identity(),
        "options": {"variant": variant, "jobs": max(1, jobs), "configure_only": configure_only, "cmake_args": cmake_args},
        "runtime_library_identity": runtime_library_identity(binaries),
        "configure_only": configure_only,
        "debug_launcher_log": str(output / "debug-launcher.jsonl") if variant != "normal" else None,
        "effective_debug_invocations": effective_debug,
        "compile_commands": {"path": str(output / "compile_commands.json"), "sha256": sha256(output / "compile_commands.json")} if (output / "compile_commands.json").is_file() else None,
        "binaries": binaries,
        "inventory_file_count": inventory["file_count"],
    }
    (output / "build-manifest.json").write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    return manifest


def write_inventory_artifacts(result: dict[str, Any], output: Path) -> None:
    output.mkdir(parents=True, exist_ok=True)
    (output / "inventory.json").write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    coverage = {"schema_version": 1, "file_count": result["file_count"], "source_root": result["root"], "status": "partial"}
    (output / "coverage.json").write_text(json.dumps(coverage, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    lines = [f"# Observability inventory\n", f"Files with macros or sinks: {result['file_count']}\n", "\n| File | Active | Commented | Guards | Sinks |\n|---|---|---|---|---:|\n"]
    for row in result["files"]:
        active = ", ".join(x["macro"] for x in row["active_defines"]) or "—"
        commented = ", ".join(x["macro"] for x in row["commented_defines"]) or "—"
        guards = ", ".join(x["macro"] for x in row["guards"]) or "—"
        lines.append(f"| `{row['path']}` | {active} | {commented} | {guards} | {row['log_sink_count']} |\n")
    (output / "inventory.md").write_text("".join(lines), encoding="utf-8")
    with (output / "logs.jsonl").open("w", encoding="utf-8") as log_file, (output / "macro-effective.jsonl").open("w", encoding="utf-8") as macro_file:
        for row in result["files"]:
            for sink in row["sinks"]:
                log_file.write(json.dumps({"file": row["path"], "line": sink["line"], "text": sink["text"], "observed": "unknown"}, ensure_ascii=False) + "\n")
            for kind in ("active_defines", "commented_defines", "guards"):
                for item in row[kind]:
                    macro_file.write(json.dumps({"file": row["path"], "macro": item["macro"], "line": item["line"], "kind": kind, "effective": row["effective"], "compiled": row["compiled"]}, ensure_ascii=False) + "\n")


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest="op", required=True)
    inv = sub.add_parser("inventory")
    inv.add_argument("--source", required=True, type=Path)
    inv.add_argument("--output", required=True, type=Path)
    val = sub.add_parser("validate")
    val.add_argument("--inventory", required=True, type=Path)
    val.add_argument("--profile", required=True, type=Path)
    ass = sub.add_parser("assess")
    ass.add_argument("--log", required=True, type=Path)
    ass.add_argument("--gates", required=True, type=Path)
    bld = sub.add_parser("build")
    bld.add_argument("--source", required=True, type=Path)
    bld.add_argument("--profile", required=True, type=Path)
    bld.add_argument("--variant", required=True, choices=["normal", "targeted-debug", "verbose-diagnostic"])
    bld.add_argument("--output", required=True, type=Path)
    bld.add_argument("--jobs", type=int, default=1)
    bld.add_argument("--configure-only", action="store_true")
    args = p.parse_args(argv)
    try:
        if args.op == "inventory":
            result = build_inventory(args.source)
            if args.output.suffix.lower() == ".json":
                args.output.parent.mkdir(parents=True, exist_ok=True)
                args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
                output = str(args.output)
            else:
                write_inventory_artifacts(result, args.output)
                output = str(args.output / "inventory.json")
            print(json.dumps({"ok": True, "file_count": result["file_count"], "output": output}))
        elif args.op == "validate":
            validate_profile(json.loads(args.profile.read_text()), json.loads(args.inventory.read_text()))
            print(json.dumps({"ok": True, "profile": str(args.profile)}))
        elif args.op == "assess":
            lines = args.log.read_text(errors="replace").splitlines()
            result = assess_lines(lines, json.loads(args.gates.read_text()))
            print(json.dumps(result, ensure_ascii=False, indent=2))
        else:
            manifest = build_variant(args.source, args.profile, args.variant, args.output, args.jobs, args.configure_only)
            print(json.dumps({"ok": True, "manifest": str(args.output / 'build-manifest.json'), "binaries": sorted(manifest["binaries"])}))
        return 0
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        print(f"OBSERVABILITY_ERROR: {exc}")
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
