#!/bin/bash
# ContentApp Repository — ensures IO/Repository1 exists and starts ContentApp in Repository mode.
# Prerequisite: PGCS must already be running (sh run_PGCS.sh).
#
# Usage: sh run_Repository.sh

set -e

BASE=${NG_REPO_PATH:-$(cd ../..; pwd)}
IO_DIR="$BASE/IO/Repository1"

echo "=== ContentApp Repository ==="

# Ensure IO directory
if [ ! -d "$IO_DIR" ]; then
    echo "Creating $IO_DIR ..."
    mkdir -p "$IO_DIR"
else
    echo "$IO_DIR already exists."
    existing=$(ls -1 "$IO_DIR" 2>/dev/null | wc -l)
    if [ "$existing" -gt 0 ]; then
        echo "Contains ${existing} files."
    fi
fi

# Run ContentApp under gdb (debug mode — captures crash backtrace)
echo "Starting ContentApp in Repository mode under gdb (IO: $IO_DIR) ..."
cd "$BASE/cmake-build-debug"
gdb -batch -ex "run" -ex "bt full" -ex "info registers" -ex "thread apply all bt full" -ex "quit" --args ./ContentApp "$IO_DIR/" Repository
