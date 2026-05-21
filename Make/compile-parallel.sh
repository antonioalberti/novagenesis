#!/bin/bash

# Parallel Compile script for NovaGenesis
# Run from: /home/gandalf/workspace/novagenesis/Make/
# Usage: bash compile-parallel.sh [jobs] [service1] [service2] ...
#   If no arguments: compiles all 5 services with max parallel jobs
#   First numeric arg = max parallel jobs (default: number of CPU cores)

set -e
shopt -s nullglob

cd "$(dirname "$0")/.." || exit 1

BUILD_DIR="cmake-build-debug"
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
trap "rm -rf $STATUS_DIR" EXIT

ACTIVE=0
FAILED=0
COMPILED=0

for service in $SERVICES; do
    # Wait if we're at max concurrency
    while [ "$ACTIVE" -ge "$JOBS" ]; do
        # Check if any child finished
        for f in "$STATUS_DIR"/*.done; do
            svc=$(basename "$f" .done)
            exit_code=$(cat "$STATUS_DIR/$svc.exit" 2>/dev/null)
            if [ "$exit_code" = "0" ]; then
                echo "  ✓ $svc"
                COMPILED=$((COMPILED + 1))
            else
                echo "  ✗ $svc FAILED (exit $exit_code)"
                FAILED=$((FAILED + 1))
            fi
            rm -f "$STATUS_DIR/$svc.done" "$STATUS_DIR/$svc.exit"
            ACTIVE=$((ACTIVE - 1))
        done
        sleep 0.5
    done

    echo "  ▶ Compiling $service..."
    ACTIVE=$((ACTIVE + 1))

    (
        if g++ -std=c++20 -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated \
            -o "$BUILD_DIR/$service" "$service/src/"*.cpp Common/src/*.cpp \
            -I Common/src/ -lpthread -lrt 2>"$STATUS_DIR/$service.log"; then
            echo 0 > "$STATUS_DIR/$service.exit"
        else
            echo $? > "$STATUS_DIR/$service.exit"
            # Show first 10 error lines
            head -10 "$STATUS_DIR/$service.log" > "$STATUS_DIR/$service.err"
        fi
        touch "$STATUS_DIR/$service.done"
    ) &
done

# ---- Wait for remaining jobs ----
echo "  Waiting for remaining jobs..."
while [ "$COMPILED" -lt "$NUM_SERVICES" ] && [ "$((COMPILED + FAILED))" -lt "$NUM_SERVICES" ]; do
    for f in "$STATUS_DIR"/*.done; do
        svc=$(basename "$f" .done)
        exit_code=$(cat "$STATUS_DIR/$svc.exit" 2>/dev/null)
        if [ "$exit_code" = "0" ]; then
            echo "  ✓ $svc"
            COMPILED=$((COMPILED + 1))
        else
            echo "  ✗ $svc FAILED"
            cat "$STATUS_DIR/$svc.err" 2>/dev/null
            FAILED=$((FAILED + 1))
        fi
        rm -f "$STATUS_DIR/$svc.done" "$STATUS_DIR/$svc.exit"
        ACTIVE=$((ACTIVE - 1))
    done
    [ "$((COMPILED + FAILED))" -ge "$NUM_SERVICES" ] && break
    sleep 0.5
done

# ---- Report ----
echo "============================================"
echo " Build complete: $COMPILED succeeded, $FAILED failed"
echo "============================================"

if [ "$FAILED" -gt 0 ]; then
    exit 1
fi
