#!/usr/bin/env python3
"""Compare received files against a publish-time SHA-256 manifest.

Usage: VerifyDelivery.py <manifest> <received-dir>
Exit status: 0 = all matched, 1 = delivery failure, 2 = invalid input.
Extra received files are ignored; the manifest defines the expected payload set.
"""

import argparse
import hashlib
from pathlib import Path
import re
import sys


def read_manifest(path):
    entries = []
    seen = set()
    with path.open("r", encoding="utf-8", newline="") as stream:
        for number, line in enumerate(stream, 1):
            if line.endswith("\n"):
                line = line[:-1]
            # Accept sha256sum's text and binary markers. Escaped filenames
            # and nested paths are intentionally unsupported for photo payloads.
            match = re.fullmatch(r"([0-9a-fA-F]{64}) ([ *])(.+)", line)
            if match is None:
                raise ValueError(f"invalid manifest line {number}")
            digest, _, name = match.groups()
            if (
                name in (".", "..", "manifest.sha256")
                or "/" in name
                or "\\" in name
                or any(ord(char) < 32 or ord(char) == 127 for char in name)
            ):
                raise ValueError(f"unsafe or reserved filename on line {number}")
            if name in seen:
                raise ValueError(f"duplicate filename on line {number}: {name}")
            seen.add(name)
            entries.append((name, digest.lower()))
    if not entries:
        raise ValueError("manifest is empty")
    return entries


def sha256_file(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("manifest", type=Path)
    parser.add_argument("received_dir", type=Path)
    args = parser.parse_args()

    try:
        entries = read_manifest(args.manifest)
        if not args.received_dir.is_dir():
            raise ValueError(f"not a directory: {args.received_dir}")
    except (OSError, UnicodeError, ValueError) as error:
        print(f"VerifyDelivery: {error}", file=sys.stderr)
        return 2

    matched = missing = mismatched = 0
    for name, expected in entries:
        path = args.received_dir / name
        try:
            if path.is_symlink():
                raise ValueError("symlink is not a received regular file")
            if not path.exists():
                missing += 1
                print(f"MISSING {name}", file=sys.stderr)
                continue
            if not path.is_file():
                raise ValueError("not a regular file")
            actual = sha256_file(path)
            if actual != expected:
                mismatched += 1
                print(
                    f"MISMATCH {name}: expected {expected}, got {actual}",
                    file=sys.stderr,
                )
            else:
                matched += 1
        except FileNotFoundError:
            missing += 1
            print(f"MISSING {name}", file=sys.stderr)
        except (OSError, ValueError) as error:
            mismatched += 1
            print(f"MISMATCH {name}: {error}", file=sys.stderr)

    print(
        f"total / matched / missing / mismatched: "
        f"{len(entries)} / {matched} / {missing} / {mismatched}"
    )
    return 1 if missing or mismatched else 0


if __name__ == "__main__":
    sys.exit(main())
