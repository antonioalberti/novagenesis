#!/bin/bash

# Parallel Compile script for NovaGenesis
# Run from: <workspace-root>/novagenesis/Make/
# Usage: bash compile-parallel.sh [jobs] [service1] [service2] ...
#   If no arguments: compiles all 5 services with max parallel jobs
#   First numeric arg = max parallel jobs (default: number of CPU cores)

set -e
shopt -s nullglob

cd "$(dirname "$0")/.." || exit 1

BUILD_DIR="cmake-build-debug"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# ---- Parse arguments ----
JOBS=$(nproc)                    # Default: all cores
SERVICES="PGCS NRNCS NBTestApp IoTTestApp ContentApp"

if [ $# -gt 0 ]; then
    # Check if first arg is a number (job count)
    if [[ "$1" =~ ^[0-9]+$ ]]; then
        JOBS=$1
        shift
    fi
    # Remaining args are service names
    if [ $# -gt 0 ]; then
        SERVICES="$@"
    fi
fi

# Clamp to number of services (don't spawn more jobs than work)
NUM_SERVICES=$(echo "$SERVICES" | wc -w)
if [ "$JOBS" -gt "$NUM_SERVICES" ]; then
    JOBS=$NUM_SERVICES
fi

echo "============================================"
echo " NovaGenesis Parallel Build"
echo " Services : $SERVICES"
echo " Max jobs : $JOBS"
echo " Build dir: $BUILD_DIR"
echo "============================================"

# ---- Status tracking ----
STATUS_DIR=$(mktemp -d /tmp/ng-build-XXXXXX)
LOG_DIR="$STATUS_DIR/logs"
mkdir -p "$LOG_DIR"
trap "rm -rf $STATUS_DIR" EXIT

ACTIVE=0
FAILED=0
COMPILED=0

echo ""
echo "Logs directory: $LOG_DIR"
echo ""

for service in $SERVICES; do
    # Wait if we're at max concurrency
    while [ "$ACTIVE" -ge "$JOBS" ]; do
        # Check if any child finished
        for f in "$STATUS_DIR"/*.done; do
            svc=$(basename "$f" .done)
            exit_code=$(cat "$STATUS_DIR/$svc.exit" 2>/dev/null)
            if [ "$exit_code" = "0" ]; then
                echo "  [OK] $service (see $LOG_DIR/$svc.log)"
                COMPILED=$((COMPILED + 1))
            else
                echo "  [FAIL] $service (exit $exit_code)"
                echo "  ---- Full stderr output for $svc ----"
                cat "$LOG_DIR/$svc.log" 2>/dev/null
                echo "  ---- End of $svc output ----"
                echo ""
                FAILED=$((FAILED + 1))
            fi
            rm -f "$STATUS_DIR/$svc.done" "$STATUS_DIR/$svc.exit"
            ACTIVE=$((ACTIVE - 1))
        done
        sleep 0.5
    done

    echo "  [>>] Compiling $service..."
    ACTIVE=$((ACTIVE + 1))

    (
        # Compile, capture ALL stderr (errors + warnings) to log file
        if g++ -std=c++20 -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated \
            -o "$BUILD_DIR/$service" "$service/src/"*.cpp Common/src/*.cpp \
            -I Common/src/ -lpthread -lrt 2>"$LOG_DIR/$service.log"; then
            echo 0 > "$STATUS_DIR/$service.exit"
        else
            echo $? > "$STATUS_DIR/$service.exit"
        fi
        touch "$STATUS_DIR/$service.done"
    ) &
done

# ---- Wait for remaining jobs ----
echo ""
echo "  Waiting for remaining jobs to complete..."
echo ""
while [ "$((COMPILED + FAILED))" -lt "$NUM_SERVICES" ]; do
    for f in "$STATUS_DIR"/*.done; do
        # Handle case where no .done files exist yet
        [ -f "$f" ] || continue
        svc=$(basename "$f" .done)
        exit_code=$(cat "$STATUS_DIR/$svc.exit" 2>/dev/null)
        if [ "$exit_code" = "0" ]; then
            echo "  [OK] $svc compiled successfully"
            # Show warning count if any
            warn_count=$(grep -c ": warning:" "$LOG_DIR/$svc.log" 2>/dev/null || true)
            if [ "$warn_count" -gt 0 ]; then
                echo "        ($warn_count warnings, see $LOG_DIR/$svc.log)"
            fi
            COMPILED=$((COMPILED + 1))
        else
            echo "  [FAIL] $svc (exit $exit_code)"
            echo "  ---- Full stderr output for $svc ----"
            cat "$LOG_DIR/$svc.log" 2>/dev/null
            echo "  ---- End of $svc output ----"
            echo ""
            FAILED=$((FAILED + 1))
        fi
        rm -f "$STATUS_DIR/$svc.done" "$STATUS_DIR/$svc.exit"
        ACTIVE=$((ACTIVE - 1))
    done
    [ "$((COMPILED + FAILED))" -ge "$NUM_SERVICES" ] && break
    sleep 0.5
done

# ---- Report ----
echo ""
echo "============================================"
echo " Build complete: $COMPILED succeeded, $FAILED failed"
echo "============================================"

if [ "$FAILED" -gt 0 ]; then
    echo ""
    echo "Full build logs are in: $LOG_DIR"
    ls -la "$LOG_DIR"/*.log 2>/dev/null
    exit 1
fi
