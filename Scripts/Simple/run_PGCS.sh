#!/bin/bash
# PGCS - Start this first (broadcast discovery mode)

BASE=/home/gandalf/workspace/novagenesis

# Auto-detect the first Ethernet interface (enp*, eth*, ens*)
IFACE=$(ip link show | grep -E "^[0-9]+: e" | head -1 | awk -F': ' '{print $2}' | awk '{print $1}')

if [ -z "$IFACE" ]; then
    echo "ERROR: No Ethernet interface found."
    exit 1
fi

echo "Using interface: $IFACE"

cd $BASE/cmake-build-debug
# Usage: ./PGCS Path Port Role -de Ethernet Interface MTU
./PGCS $BASE/IO/PGCS/ 0 Intra_Domain -de Ethernet "$IFACE" 1200
