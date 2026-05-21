#!/bin/ash
# Start NovaGenesis services for Source VM
# Based on working example with correct MAC addresses and parameters

set -e

export BASE="/root/workspace/novagenesis"

echo "=== Starting NovaGenesis Source VM (192.168.0.36) ==="
echo "MAC Address: 08:00:27:79:bb:15"
echo ""

cd $BASE/cmake-build-debug

# Create necessary IO directories
mkdir -p $BASE/IO/PGCS
mkdir -p $BASE/IO/NRNCS
mkdir -p $BASE/IO/Source1
mkdir -p $BASE/IO/logs

# Start PGCS with peer information (communicating with Repo VM)
# Format: ./PGCS <Path> <Port> <Role> -p Ethernet <Peer_Role> <Interface> <Peer_MAC> <MTU>
echo "Starting PGCS (Source)..."
./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 08:00:27:65:00:08 1200 > $BASE/IO/logs/PGCS.log 2>&1 &

sleep 2

# Start NRNCS (Name Resolution and Network Cache Service)
# Format: ./NRNCS <Path>
echo "Starting NRNCS..."
./NRNCS $BASE/IO/NRNCS/ > $BASE/IO/logs/NRNCS.log 2>&1 &

sleep 2

# Start ContentApp as Source
# Format: ./ContentApp <Path> <Role>
echo "Starting ContentApp (Source)..."
./ContentApp $BASE/IO/Source1/ Source > $BASE/IO/logs/ContentApp.log 2>&1 &

sleep 2

echo "=== NovaGenesis Source services started ==="
echo "PGCS PID: $(pgrep PGCS)"
echo "NRNCS PID: $(pgrep NRNCS)"
echo "ContentApp PID: $(pgrep ContentApp)"
echo ""
echo "To monitor: tail -f $BASE/IO/logs/*.log"
