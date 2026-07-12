#!/bin/bash
# run_Source_on_Source_VM.sh — Start ContentApp Source on Source VM (192.168.0.36)
#
# Opens a dedicated SSH terminal to the Source Alpine VM, generates test
# photos via BuildPhotos.py, and starts ContentApp in Source mode.
# Must start AFTER all other processes are running.
#
# Usage: bash run_Source_on_Source_VM.sh [num_photos] [width] [height]
#   Default: 100 photos, 800x600
#
# Prerequisites:
#   - PGCS on both VMs + NRNCS on Source VM must already be running
#   - Python3 + Pillow on Source VM (for photo generation)
#
# Order: run LAST (Terminal 5), after Repository on Repo VM

SSH_KEY=~/.ssh/id_ed25519_hermes
VM_IP=192.168.0.36
PHOTOS=${1:-300}
WIDTH=${2:-800}
HEIGHT=${3:-600}
BASE=/root/workspace/novagenesis
IO_DIR=${BASE}/IO/Source1

echo "=== ContentApp Source on Source VM (${VM_IP}) ==="
echo "Photos: ${PHOTOS} (${WIDTH}x${HEIGHT})"
echo "Opening SSH terminal... (Ctrl+C to stop ContentApp)"
echo ""

ssh -t -i ${SSH_KEY} root@${VM_IP} \
  "cd ${BASE} && \
   python3 Scripts/Python/BuildPhotos.py different ${PHOTOS} ${WIDTH} ${HEIGHT} && \
   mkdir -p ${IO_DIR} && \
   mv *.jpg ${IO_DIR}/ 2>/dev/null; \
   echo 'Photos generated. Starting ContentApp Source...' && \
   cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt full\" -ex \"info registers\" -ex \"thread apply all bt full\" -ex \"quit\" --args \
   ./ContentApp ${IO_DIR}/ Source"
