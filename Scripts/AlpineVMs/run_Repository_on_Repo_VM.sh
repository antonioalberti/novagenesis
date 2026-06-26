#!/bin/bash
# run_Repository_on_Repo_VM.sh — Start ContentApp Repository on Repo VM (192.168.0.61)
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

SSH_KEY=~/.ssh/id_ed25519_hermes
VM_IP=192.168.0.61
BASE=/root/workspace/novagenesis
IO_DIR=${BASE}/IO/Repository1

echo "=== ContentApp Repository on Repo VM (${VM_IP}) ==="
echo "Opening SSH terminal... (Ctrl+C to stop ContentApp)"
echo ""

ssh -t -i ${SSH_KEY} root@${VM_IP} \
  "mkdir -p ${IO_DIR} && \
   echo 'IO directory ready.' && \
   cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt full\" -ex \"info registers\" -ex \"thread apply all bt full\" -ex \"quit\" --args \
   ./ContentApp ${IO_DIR}/ Repository"