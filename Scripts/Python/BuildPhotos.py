#!/usr/bin/env python3
# BuildPhotos — Generates test JPEG images for ContentApp testing.
#
# Stdlib-only baseline JPEG encoder: grayscale, constant-color 8x8 blocks.
# "different" uses random block intensities; "equal" uses uniform grayscale.
# These are synthetic delivery payloads, not pixel-equivalent Pillow output.
#
# Usage:
#   BuildPhotos.py [--staging DIR] [different|equal] COUNT [WIDTH] [HEIGHT]
#
# Defaults: different, 200x200. Without --staging, writes to the current
# directory (legacy behavior). With --staging, DIR must not already exist.
# Staged output includes manifest.sha256 and is made read-only.

import argparse
import hashlib
import os
from pathlib import Path
import re
import secrets
import socket
import struct
import sys


def segment(marker, payload):
    return b"\xff" + bytes([marker]) + struct.pack(">H", len(payload) + 2) + payload


class BitWriter:
    def __init__(self):
        self.data = bytearray()
        self.bits = 0
        self.count = 0

    def write(self, value, count):
        self.bits = (self.bits << count) | value
        self.count += count
        while self.count >= 8:
            self.count -= 8
            byte = (self.bits >> self.count) & 255
            self.data.append(byte)
            if byte == 255:
                self.data.append(0)
        self.bits &= (1 << self.count) - 1

    def finish(self):
        if self.count:
            padding = 8 - self.count
            self.write((1 << padding) - 1, padding)
        return bytes(self.data)


def jpeg(width, height, intensity=None):
    """Encode a baseline grayscale JPEG with DC-only 8x8 blocks."""
    header = b"\xff\xd8"
    header += segment(
        0xE0, b"JFIF\x00\x01\x01\x00\x00\x01\x00\x01\x00\x00"
    )
    header += segment(0xDB, b"\x00" + bytes([16]) * 64)
    header += segment(
        0xC0, struct.pack(">BHHB", 8, height, width, 1) + b"\x01\x11\x00"
    )

    # DC table: categories 0..11 have canonical four-bit codes 0..11.
    # AC table: only EOB is needed, with the one-bit code 0.
    dc_counts = bytes([0, 0, 0, 12] + [0] * 12)
    ac_counts = bytes([1] + [0] * 15)
    header += segment(
        0xC4,
        b"\x00" + dc_counts + bytes(range(12))
        + b"\x10" + ac_counts + b"\x00",
    )
    header += segment(0xDA, b"\x01\x01\x00\x00\x3f\x00")

    blocks = ((width + 7) // 8) * ((height + 7) // 8)
    values = secrets.token_bytes(blocks) if intensity is None else None
    writer = BitWriter()
    previous = 0
    for index in range(blocks):
        value = values[index] if values is not None else intensity
        dc = round((value - 128) / 2)
        difference = dc - previous
        previous = dc
        category = abs(difference).bit_length()
        writer.write(category, 4)
        if category:
            amplitude = (
                difference if difference >= 0
                else difference + (1 << category) - 1
            )
            writer.write(amplitude, category)
        writer.write(0, 1)  # All AC coefficients are zero: EOB.
    return header + writer.finish() + b"\xff\xd9"


def write_synced(path, data, exclusive):
    with path.open("xb" if exclusive else "wb") as stream:
        stream.write(data)
        stream.flush()
        os.fsync(stream.fileno())


def sync_directory(path):
    descriptor = os.open(path, os.O_RDONLY | os.O_DIRECTORY)
    try:
        os.fsync(descriptor)
    finally:
        os.close(descriptor)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--staging", type=Path)
    parser.add_argument("parameters", nargs="+", metavar="ARG")
    args = parser.parse_args()

    parameters = list(args.parameters)
    mode = "different"
    if parameters[0] in ("different", "equal"):
        mode = parameters.pop(0)
    if not 1 <= len(parameters) <= 3:
        parser.error("expected [different|equal] COUNT [WIDTH] [HEIGHT]")
    try:
        count = int(parameters[0])
        width = int(parameters[1]) if len(parameters) > 1 else 200
        height = int(parameters[2]) if len(parameters) > 2 else 200
    except ValueError:
        parser.error("COUNT, WIDTH and HEIGHT must be integers")
    if count <= 0 or not (1 <= width <= 65535 and 1 <= height <= 65535):
        parser.error("COUNT must be positive; dimensions must be 1..65535")

    hostname = socket.gethostname()
    if not re.fullmatch(r"[A-Za-z0-9_.-]+", hostname):
        parser.error("hostname must contain only letters, digits, _, . or -")

    staged = args.staging is not None
    target = args.staging if staged else Path.cwd()
    if staged:
        # Exclusive creation is intentional, even for an existing empty directory.
        target.mkdir(parents=True, exist_ok=False)

    entries = []
    for index in range(count):
        name = f"{index:05d}-{hostname}.jpg"
        intensity = int(255 * index / count) if mode == "equal" else None
        data = jpeg(width, height, intensity)
        write_synced(target / name, data, exclusive=staged)
        entries.append((name, hashlib.sha256(data).hexdigest()))

    if staged:
        # Every image has been closed and fsynced before the manifest is written.
        manifest = "".join(f"{digest}  {name}\n" for name, digest in entries)
        write_synced(
            target / "manifest.sha256", manifest.encode("ascii"), exclusive=True
        )
        for name, _ in entries:
            path = target / name
            path.chmod(path.stat().st_mode & ~0o222)
        manifest_path = target / "manifest.sha256"
        manifest_path.chmod(manifest_path.stat().st_mode & ~0o222)
        target.chmod(target.stat().st_mode & ~0o222)
        sync_directory(target)
        sync_directory(target.parent)
        print(f"Staged {count} photos: {target}")
        print(f"Manifest: {manifest_path}")
    else:
        print(f"Generated {count} photos in {target} (legacy mutable mode)")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError) as error:
        print(f"BuildPhotos: {error}", file=sys.stderr)
        sys.exit(1)
