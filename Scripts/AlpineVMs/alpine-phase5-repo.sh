#!/bin/ash
# Phase 5: Start NovaGenesis services for Repository VM
# Usage: ash /mnt/alpine-phase5-repo.sh
# This script starts PGCS, ContentApp (Repository role), and NRNCS

set -e

echo "=== NovaGenesis Phase 5: Repository VM Setup ==="

# Configuration
IO_PATH="/root/workspace/novagenesis/IO"
PGCS_PORT="5000"
PGCS_ROLE="Repository"
SOURCE_VM_IP="192.168.0.36"
REPO_VM_IP="192.168.0.61"

# Change to build directory
cd /root/workspace/novagenesis/cmake-build-debug

# Start PGCS with peer information (communicating with source VM)
# Parameters: Path Port Role -p Stack Role Interface Identifier Size
echo "Starting PGCS (Repository)..."
./PGCS "$IO_PATH" "$PGCS_PORT" "$PGCS_ROLE" -p \
    "Ethernet" "Source" "eth0" "$SOURCE_VM_IP" "1500" \
    > /root/workspace/novagenesis/IO/logs/PGCS.log 2>&1 &

sleep 2

# Start ContentApp as Repository
# Parameters: Path Role
echo "Starting ContentApp (Repository)..."
./ContentApp "$IO_PATH/Repository1" "Repository" \
    > /root/workspace/novagenesis/IO/logs/ContentApp.log 2>&1 &

sleep 2

# Start NRNCS (Name Resolution and Network Cache Service)
# Parameters: Path
echo "Starting NRNCS..."
./NRNCS "$IO_PATH" \
    > /root/workspace/novagenesis/IO/logs/NRNCS.log 2>&1 &

sleep 2

echo "=== NovaGenesis Repository services started ==="
echo "PGCS PID: $(pgrep PGCS)"
echo "ContentApp PID: $(pgrep ContentApp)"
echo "NRNCS PID: $(pgrep NRNCS)"
echo ""
echo "To monitor: tail -f /root/workspace/novagenesis/IO/logs/*.log"
echo ""
echo "Note: ContentApp measures publish/subscribe times in the logs"