#!/bin/sh
set -eu
SCRIPT="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)/supervise_process.py"
exec python3 "$SCRIPT" "$@"
