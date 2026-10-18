#!/bin/bash

# Start NG-Source Services
# This script initializes PGCS and ContentApp for the Source role
# Using raw Ethernet sockets with broadcast discovery

cd /home/ng/workspace/novagenesis/cmake-build-debug/

# Start PGCS with Ethernet broadcast discovery
# Format: ./PGCS <Path> <Port> <RoleLocal> -de Ethernet <InterfaceLocal> <MTU>
./PGCS /home/ng/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -de Ethernet eth0 1200 &

sleep 2

# Start ContentApp in Source mode
./ContentApp /home/ng/workspace/novagenesis/IO/Source1/ Source &

echo "Source services started successfully"
echo "PGCS and ContentApp are running in background"
