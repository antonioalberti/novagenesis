#!/usr/bin/env python3
"""Read-only SPEC-039 target census of an existing CMake File API codemodel v2.

Usage: target_census_checker.py PROFILE BUILD_ROOT CODEMODEL --configuration NAME
Use --configuration '' for an unnamed single-configuration generator. CODEMODEL
must be an existing codemodel object in BUILD_ROOT/.cmake/api/v1/reply, not an
index. No index discovery, configure/build, query creation, or manifest override
is performed. Expectations are the fixed SPEC-039 profiles below.

PASS describes only the supplied selected graph. Synthetic graphs do not prove
an actual configured build; freshness, cache options, provenance, compile/link
properties, and all other configurations require separate evidence.
"""
import argparse
import json
from pathlib import Path
import sys
from typing import NoReturn

EXPECTED = {
    "diagnostic": {"Common", "ng039_gw_q0_q1_d2"},
    "default": {"Common", "ContentApp", "IoTTestApp", "NBTestApp", "PGCS", "NRNCS"},
}
LIMITATIONS = ("Supplied graph only; synthetic input is not real configured-build proof. "
               "Does not establish freshness, cache options, provenance, compile/link "
               "properties, other configurations, or SPEC section 9 gate closure.")


def reject(message) -> NoReturn:
    raise ValueError(message)


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            reject(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def read_object(path):
    with path.open(encoding="utf-8") as stream:
        value = json.load(stream, object_pairs_hook=unique_object,
                          parse_constant=lambda value: reject(f"invalid JSON constant: {value}"))
    if not isinstance(value, dict):
        reject(f"JSON object required: {path}")
    return value


def text(value, label, empty=False):
    if not isinstance(value, str) or (not empty and not value) or "\x00" in value:
        reject(f"invalid {label}")
    return value


def validate_target_census(profile, build_root, codemodel_path, configuration):
    """Validate all target references in one explicitly selected configuration."""
    if profile not in EXPECTED:
        reject(f"unknown profile: {profile}")
    text(configuration, "configuration", empty=True)
    root = Path(build_root).resolve(strict=True)
    if not root.is_dir():
        reject("build root must be a directory")
    reply = (root / ".cmake/api/v1/reply").resolve(strict=True)
    if not reply.is_relative_to(root) or not reply.is_dir():
        reject("reply directory escapes build root or is not a directory")
    supplied = Path(codemodel_path)
    if ".." in supplied.parts:
        reject("codemodel traversal is forbidden")
    model_path = supplied.resolve(strict=True)
    if model_path.parent != reply or not model_path.is_file():
        reject("codemodel must be a file directly in the build reply directory")
    model = read_object(model_path)
    version = model.get("version")
    if (model.get("kind") != "codemodel" or not isinstance(version, dict)
            or type(version.get("major")) is not int or version["major"] != 2
            or type(version.get("minor")) is not int or version["minor"] < 0):
        reject("expected CMake codemodel version 2")
    paths = model.get("paths")
    if not isinstance(paths, dict):
        reject("missing codemodel paths")
    build = Path(text(paths.get("build"), "codemodel build path"))
    source = Path(text(paths.get("source"), "codemodel source path"))
    if not source.is_absolute() or not build.is_absolute() or build.resolve(strict=True) != root:
        reject("codemodel build root mismatch or nonabsolute paths")
    configs = model.get("configurations")
    if not isinstance(configs, list) or not configs:
        reject("missing configurations")
    by_name = {}
    for config in configs:
        if not isinstance(config, dict):
            reject("invalid configuration object")
        name = text(config.get("name"), "configuration name", empty=True)
        if name in by_name:
            reject("duplicate configuration name")
        if not isinstance(config.get("targets"), list):
            reject("missing configuration targets")
        by_name[name] = config
    if configuration not in by_name:
        reject(f"configuration not found: {configuration!r}")
    names, ids, files = set(), set(), set()
    for ref in by_name[configuration]["targets"]:
        if not isinstance(ref, dict):
            reject("invalid target reference")
        name = text(ref.get("name"), "target name")
        target_id = text(ref.get("id"), "target id")
        filename = text(ref.get("jsonFile"), "target jsonFile")
        # File API jsonFile references are filenames within the reply directory.
        if filename in (".", "..") or "/" in filename or "\\" in filename:
            reject("target reference traversal or path escape")
        target_path = (reply / filename).resolve(strict=True)
        if target_path.parent != reply or not target_path.is_file():
            reject("target reference escapes reply directory or is not a file")
        if name in names or target_id in ids or target_path in files:
            reject("duplicate target name, id, or file reference")
        target = read_object(target_path)
        if target.get("name") != name or target.get("id") != target_id:
            reject("target reference identity mismatch")
        if target.get("type") not in {"EXECUTABLE", "STATIC_LIBRARY", "SHARED_LIBRARY",
                                      "MODULE_LIBRARY", "OBJECT_LIBRARY", "INTERFACE_LIBRARY", "UTILITY"}:
            reject("invalid or missing target type")
        names.add(name)
        ids.add(target_id)
        files.add(target_path)
    missing, extra = EXPECTED[profile] - names, names - EXPECTED[profile]
    if missing or extra:
        reject(f"target census mismatch: missing={sorted(missing)}, extra={sorted(extra)}")
    return {"profile": profile, "build_root": str(root), "codemodel": str(model_path),
            "configuration": configuration, "expected": sorted(EXPECTED[profile]),
            "actual": sorted(names), "status": "PASS", "limitations": LIMITATIONS}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("profile", choices=sorted(EXPECTED))
    parser.add_argument("build_root")
    parser.add_argument("codemodel", help="Existing File API codemodel v2 JSON (not index)")
    parser.add_argument("--configuration", required=True)
    args = parser.parse_args()
    try:
        result = validate_target_census(args.profile, args.build_root, args.codemodel, args.configuration)
        print(json.dumps(result, sort_keys=True))
    except (ValueError, OSError, RuntimeError) as exc:
        print(f"target census check failed: {exc}", file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main())
