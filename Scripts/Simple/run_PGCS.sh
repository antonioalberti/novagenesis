#!/bin/bash
#
# run_PGCS.sh — Start PGCS in deterministic (-p) or discovery (-de) mode
#
# Usage:
#   Discovery mode:  bash run_PGCS.sh
#   Deterministic:   bash run_PGCS.sh <peer_mac>
#
# Examples:
#   bash run_PGCS.sh                              # -de (broadcast discovery)
#   bash run_PGCS.sh 02:42:ac:14:00:0b            # -p (peer MAC)

BASE=/home/gandalf/workspace/novagenesis

# Auto-detect the first Ethernet interface (enp*, eth*, ens*)
IFACE=$(ip link show | grep -E "^[0-9]+: e" | head -1 | awk -F': ' '{print $2}' | awk '{print $1}')

if [ -z "$IFACE" ]; then
    echo "ERROR: No Ethernet interface found."
    exit 1
fi

echo "Using interface: $IFACE"

cd $BASE/cmake-build-debug

if [ $# -ge 1 ]; then
    # Deterministic mode: -p with peer MAC
    PEER_MAC="$1"
    echo "Deterministic mode: peer MAC = ${PEER_MAC}"
    sudo gdb -batch -ex "run" -ex "bt" -ex "quit" --args ./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain "$IFACE" "$PEER_MAC" 1200
else
    # Broadcast discovery mode
    echo "Discovery mode (-de)"
    sudo gdb -batch -ex "run" -ex "bt" -ex "quit" --args ./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -de Ethernet "$IFACE" 1200
fi