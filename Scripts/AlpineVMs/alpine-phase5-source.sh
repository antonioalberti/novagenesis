#!/bin/ash
# Phase 5: Start NovaGenesis services for Source VM
# Usage: ash /mnt/alpine-phase5-source.sh
# This script starts PGCS and ContentApp (Source role)

set -e

echo "=== NovaGenesis Phase 5: Source VM Setup ==="

# Configuration
: "${NG_REPO_PATH:?Set NG_REPO_PATH before running}"
: "${REPO_VM_IP:?Set REPO_VM_IP before running}"
IO_PATH="${NG_REPO_PATH}/IO"
PGCS_PORT="5000"
PGCS_ROLE="Source"


# Change to build directory
cd "${NG_REPO_PATH}/cmake-build-debug"

# Start PGCS with peer information (communicating with repo VM)
# Parameters: Path Port Role -p Stack Role Interface Identifier Size
echo "Starting PGCS (Source)..."
./PGCS "$IO_PATH" "$PGCS_PORT" "$PGCS_ROLE" -p \
    "Ethernet" "Repository" "eth0" "$REPO_VM_IP" "1500" \
    > "${IO_PATH}/logs/PGCS.log" 2>&1 &

sleep 2

# Start ContentApp as Source
# Parameters: Path Role
echo "Starting ContentApp (Source)..."
./ContentApp "$IO_PATH/Source1" "Source" \
    > "${IO_PATH}/logs/ContentApp.log" 2>&1 &

sleep 2

echo "=== NovaGenesis Source services started ==="
echo "PGCS PID: $(pgrep PGCS)"
echo "ContentApp PID: $(pgrep ContentApp)"
echo ""
echo "To monitor: tail -f ${IO_PATH}/logs/*.log"
