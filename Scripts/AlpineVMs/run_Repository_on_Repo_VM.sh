#!/bin/bash
# run_Repository_on_Repo_VM.sh — Start ContentApp Repository on the repository guest
#
# Opens a dedicated SSH terminal to the Repo Alpine VM and starts ContentApp
# in Repository mode. Must start AFTER both PGCS processes are running.
#
# Usage: bash run_Repository_on_Repo_VM.sh
# Prerequisites:
#   - PGCS on both VMs must already be running
#   - ContentApp binary compiled on Repo VM
#
# Order: run after NRNCS (Terminal 4), before Source (Terminal 5)

: "${REPO_VM_IP:?Repository VM IP is required; source ng-vm.env}"
: "${NG_REPO_PATH:?NG_REPO_PATH is required; source ng-vm.env}"
SSH_KEY=${NG_SSH_KEY:-$HOME/.ssh/id_ed25519}
SSH_USER=${NG_SSH_USER:-root}
VM_IP="$REPO_VM_IP"
BASE="$NG_REPO_PATH"
IO_DIR=${REPO_IO_DIR:-${BASE}/IO/Repository1}

echo "=== ContentApp Repository on Repo VM (${VM_IP}) ==="
echo "Opening SSH terminal... (Ctrl+C to stop ContentApp)"
echo ""

ssh -t -i "${SSH_KEY}" "${SSH_USER}@${VM_IP}" \
  "mkdir -p ${IO_DIR} && \
   echo 'IO directory ready.' && \
   cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt full\" -ex \"info registers\" -ex \"thread apply all bt full\" -ex \"quit\" --args \
   ./ContentApp ${IO_DIR}/ Repository"