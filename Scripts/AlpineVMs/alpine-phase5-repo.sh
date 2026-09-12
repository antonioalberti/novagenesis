#!/bin/ash
# Phase 5: Start NovaGenesis services for Repository VM
# Usage: ash /mnt/alpine-phase5-repo.sh
# This script starts PGCS, ContentApp (Repository role), and NRNCS

set -e

echo "=== NovaGenesis Phase 5: Repository VM Setup ==="

# Configuration
: "${NG_REPO_PATH:?Set NG_REPO_PATH before running}"
: "${SOURCE_VM_IP:?Set SOURCE_VM_IP before running}"
IO_PATH="${NG_REPO_PATH}/IO"
PGCS_PORT="5000"
PGCS_ROLE="Repository"


# Change to build directory
cd "${NG_REPO_PATH}/cmake-build-debug"

# Start PGCS with peer information (communicating with source VM)
# Parameters: Path Port Role -p Stack Role Interface Identifier Size
echo "Starting PGCS (Repository)..."
./PGCS "$IO_PATH" "$PGCS_PORT" "$PGCS_ROLE" -p \
    "Ethernet" "Source" "eth0" "$SOURCE_VM_IP" "1500" \
    > "${IO_PATH}/logs/PGCS.log" 2>&1 &

sleep 2

# Start ContentApp as Repository
# Parameters: Path Role
echo "Starting ContentApp (Repository)..."
./ContentApp "$IO_PATH/Repository1" "Repository" \
    > "${IO_PATH}/logs/ContentApp.log" 2>&1 &

sleep 2

# Start NRNCS (Name Resolution and Network Cache Service)
# Parameters: Path
echo "Starting NRNCS..."
./NRNCS "$IO_PATH" \
    > "${IO_PATH}/logs/NRNCS.log" 2>&1 &

sleep 2

echo "=== NovaGenesis Repository services started ==="
echo "PGCS PID: $(pgrep PGCS)"
echo "ContentApp PID: $(pgrep ContentApp)"
echo "NRNCS PID: $(pgrep NRNCS)"
echo ""
echo "To monitor: tail -f ${IO_PATH}/logs/*.log"
echo ""
echo "Note: ContentApp measures publish/subscribe times in the logs"