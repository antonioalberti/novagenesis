#!/bin/ash
# Phase 5: Start NovaGenesis services for Source VM
# Usage: ash /mnt/alpine-phase5-source.sh
# This script starts PGCS and ContentApp (Source role)

set -e

echo "=== NovaGenesis Phase 5: Source VM Setup ==="

# Configuration
IO_PATH="/root/workspace/novagenesis/IO"
PGCS_PORT="5000"
PGCS_ROLE="Source"
REPO_VM_IP="192.168.0.61"
SOURCE_VM_IP="192.168.0.36"

# Change to build directory
cd /root/workspace/novagenesis/cmake-build-debug

# Start PGCS with peer information (communicating with repo VM)
# Parameters: Path Port Role -p Stack Role Interface Identifier Size
echo "Starting PGCS (Source)..."
./PGCS "$IO_PATH" "$PGCS_PORT" "$PGCS_ROLE" -p \
    "Ethernet" "Repository" "eth0" "$REPO_VM_IP" "1500" \
    > /root/workspace/novagenesis/IO/logs/PGCS.log 2>&1 &

sleep 2

# Start ContentApp as Source
# Parameters: Path Role
echo "Starting ContentApp (Source)..."
./ContentApp "$IO_PATH/Source1" "Source" \
    > /root/workspace/novagenesis/IO/logs/ContentApp.log 2>&1 &

sleep 2

echo "=== NovaGenesis Source services started ==="
echo "PGCS PID: $(pgrep PGCS)"
echo "ContentApp PID: $(pgrep ContentApp)"
echo ""
echo "To monitor: tail -f /root/workspace/novagenesis/IO/logs/*.log"