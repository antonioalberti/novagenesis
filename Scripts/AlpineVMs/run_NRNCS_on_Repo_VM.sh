#!/bin/bash
# run_NRNCS_on_Repo_VM.sh — Start NRNCS on the repository guest
#
# Opens a dedicated SSH terminal to the Repo Alpine VM and starts NRNCS.
# Must start AFTER both PGCS processes are running and have discovered
# each other via raw socket.
#
# Usage: bash run_NRNCS_on_Repo_VM.sh
# Prerequisites:
#   - PGCS running on both Source and Repo VMs
#   - NRNCS binary compiled on Repo VM (cmake-build-debug/NRNCS)
#
# Order: run after both PGCS scripts (Terminal 3), wait ~2s

: "${REPO_VM_IP:?Repository VM IP is required; source ng-vm.env}"
: "${NG_REPO_PATH:?NG_REPO_PATH is required; source ng-vm.env}"
SSH_KEY=${NG_SSH_KEY:-$HOME/.ssh/id_ed25519}
SSH_USER=${NG_SSH_USER:-root}
VM_IP="$REPO_VM_IP"
BASE="$NG_REPO_PATH"

echo "=== NRNCS on Repo VM (${VM_IP}) ==="
echo "Opening SSH terminal... (Ctrl+C to stop NRNCS)"
echo ""

ssh -t -i "${SSH_KEY}" "${SSH_USER}@${VM_IP}" \
  "cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt\" -ex \"quit\" --args \
   ./NRNCS ${BASE}/IO/NRNCS/"