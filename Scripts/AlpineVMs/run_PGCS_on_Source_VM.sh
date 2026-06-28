#!/bin/bash
# run_PGCS_on_Source_VM.sh — Start PGCS on Source VM (192.168.0.36)
#
# Opens a dedicated SSH terminal to the Source Alpine VM and starts PGCS
# in deterministic mode (-p) targeting the Repo VM's MAC address for
# inter-VM raw socket discovery over the ProxMox bridge.
#
# Usage: bash run_PGCS_on_Source_VM.sh
# Prerequisites:
#   - Source VM (102) must be running
#   - SSH key ~/.ssh/id_ed25519_hermes deployed on the VM
#   - PGCS binary compiled (AIOPT2 branch)
#
# Order: run this first (Terminal 1), then run_PGCS_on_Repo_VM.sh (Terminal 2)

SSH_KEY=~/.ssh/id_ed25519_hermes
VM_IP=192.168.0.36
PEER_MAC=08:00:27:65:00:08  # Repo VM MAC
BASE=/root/workspace/novagenesis

echo "=== PGCS on Source VM (${VM_IP}) ==="
echo "Interface: eth0 | Peer MAC: ${PEER_MAC}"

# Clean previous execution
echo "Cleaning previous execution..."
ssh -i ${SSH_KEY} root@${VM_IP} "bash ${BASE}/Scripts/Simple/clean.sh" 2>&1
echo "Clean done."

echo "Opening SSH terminal... (Ctrl+C to stop PGCS)"
echo ""

ssh -t -i ${SSH_KEY} root@${VM_IP} \
  "cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt\" -ex \"quit\" --args \
   ./PGCS ${BASE}/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 ${PEER_MAC} 1200"