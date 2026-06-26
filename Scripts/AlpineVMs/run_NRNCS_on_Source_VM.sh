#!/bin/bash
# run_NRNCS_on_Source_VM.sh — Start NRNCS on Source VM (192.168.0.36)
#
# Opens a dedicated SSH terminal to the Source Alpine VM and starts NRNCS.
# Must start AFTER both PGCS processes are running and have discovered
# each other via raw socket.
#
# Usage: bash run_NRNCS_on_Source_VM.sh
# Prerequisites:
#   - PGCS running on both Source and Repo VMs
#   - NRNCS binary compiled on Source VM (cmake-build-debug/NRNCS)
#
# Order: run after both PGCS scripts (Terminal 3), wait ~2s

SSH_KEY=~/.ssh/id_ed25519_hermes
VM_IP=192.168.0.36
BASE=/root/workspace/novagenesis

echo "=== NRNCS on Source VM (${VM_IP}) ==="
echo "Opening SSH terminal... (Ctrl+C to stop NRNCS)"
echo ""

ssh -t -i ${SSH_KEY} root@${VM_IP} \
  "cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt\" -ex \"quit\" --args \
   ./NRNCS ${BASE}/IO/NRNCS/"