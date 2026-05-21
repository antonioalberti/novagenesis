#!/bin/bash
# ================================================================================
# stop_test.sh
# ================================================================================
# Stops all processes from a test run using the saved PIDs file.
# Usage: bash stop_test.sh <log_directory>
# Example: bash stop_test.sh ../../IO/logs/run_20260503_093000
# ================================================================================

if [ -z "$1" ]; then
    echo "Usage: $0 <log_directory>"
    echo "Example: $0 ../../IO/logs/run_20260503_093000"
    exit 1
fi

LOGDIR="$1"
PIDFILE="$LOGDIR/PIDs.txt"

if [ ! -f "$PIDFILE" ]; then
    echo "Error: PID file not found: $PIDFILE"
    echo "Make sure you specify the correct run directory."
    exit 1
fi

echo "============================================================"
echo "Stopping test processes from: $LOGDIR"
echo "============================================================"

while IFS='=' read -r name pid; do
    if kill -0 "$pid" 2>/dev/null; then
        echo "Stopping $name (PID $pid)..."
        kill "$pid"
        sleep 1
        # Force kill if still running
        if kill -0 "$pid" 2>/dev/null; then
            echo "  Force killing $name (PID $pid)..."
            kill -9 "$pid"
        fi
    else
        echo "$name (PID $pid) already stopped."
    fi
done < "$PIDFILE"

echo ""
echo "All processes stopped."
echo "============================================================"