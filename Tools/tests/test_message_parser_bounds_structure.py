#!/usr/bin/env python3
"""SPEC-075 structural RED/GREEN for the active Message parser."""
from __future__ import annotations

import pathlib
import re
import sys

root = pathlib.Path(__file__).resolve().parents[2]
source = root / "Common/src/Message.cpp"
text = source.read_text(encoding="utf-8")

start_marker = "int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()"
end_marker = "// ******************************************************\n// Message size functions"
start = text.find(start_marker)
end = text.find(end_marker, start)
if start < 0 or end < 0:
    print("ERROR: active parser region not found", file=sys.stderr)
    raise SystemExit(2)

body = text[start:end]
needle = "Msg[t] == 'n'"
pos = body.find(needle)
if pos < 0:
    print("ERROR: four-byte prefix recognition condition not found", file=sys.stderr)
    raise SystemExit(2)

condition_start = body.rfind("if (", 0, pos)
condition_end = body.find("\n      {", pos)
if condition_start < 0 or condition_end < 0:
    print("ERROR: prefix condition boundaries not found", file=sys.stderr)
    raise SystemExit(2)

condition = body[condition_start:condition_end]
required = ["Msg[t + 1]", "Msg[t + 2]", "Msg[t + 3]"]
if not all(token in condition for token in required):
    print("ERROR: incomplete four-byte prefix condition", file=sys.stderr)
    raise SystemExit(2)

normalised = " ".join(condition.split())
if "(MessageSize - t) >= 4" in normalised or "t + 3 < MessageSize" in normalised:
    print("PASS: explicit four-byte remaining-length guard is in the prefix condition")
    raise SystemExit(0)

print(
    "RED: active parser evaluates Msg[t+1], Msg[t+2], and Msg[t+3] "
    "without an explicit remaining-length guard in the prefix condition",
    file=sys.stderr,
)
raise SystemExit(1)
