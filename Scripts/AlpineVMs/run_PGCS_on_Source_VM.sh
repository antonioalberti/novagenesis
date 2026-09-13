#!/bin/bash
# run_PGCS_on_Source_VM.sh — Start PGCS on the source guest
#
# Opens a dedicated SSH terminal to the Source Alpine VM and starts PGCS
# in deterministic mode (-p) targeting the Repo VM's MAC address for
# inter-VM raw socket discovery over the ProxMox bridge.
#
# Usage: bash run_PGCS_on_Source_VM.sh
# Prerequisites:
#   - The source guest must be running
#   - An operator-selected SSH key must be available
#   - PGCS binary compiled (AIOPT2 branch)
#
# Order: run this first (Terminal 1), then run_PGCS_on_Repo_VM.sh (Terminal 2)

: "${SOURCE_VM_IP:?Source VM IP is required; source ng-vm.env}"
: "${REPO_VM_MAC:?Repository VM MAC is required; source ng-vm.env}"
: "${NG_REPO_PATH:?NG_REPO_PATH is required; source ng-vm.env}"
SSH_KEY=${NG_SSH_KEY:-$HOME/.ssh/id_ed25519}
SSH_USER=${NG_SSH_USER:-root}
VM_IP="$SOURCE_VM_IP"
PEER_MAC="$REPO_VM_MAC"
BASE="$NG_REPO_PATH"

echo "=== PGCS on Source VM (${VM_IP}) ==="
echo "Interface: eth0 | Peer MAC: ${PEER_MAC}"

# Clean previous execution
echo "Cleaning previous execution..."
ssh -i "${SSH_KEY}" "${SSH_USER}@${VM_IP}" "bash ${BASE}/Scripts/Simple/clean.sh" 2>&1
echo "Clean done."

echo "Opening SSH terminal... (Ctrl+C to stop PGCS)"
echo ""

ssh -t -i "${SSH_KEY}" "${SSH_USER}@${VM_IP}" \
  "cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt\" -ex \"quit\" --args \
   ./PGCS ${BASE}/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 ${PEER_MAC} 1200"