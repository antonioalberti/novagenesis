#!/bin/bash
# pull-and-build-vms.sh — Git stash + pull + compile on VMs 101 and 102
#
# Connects via SSH to both NovaGenesis Alpine VMs, stashes any local
# changes, pulls the latest AIOPT2 branch, then compiles PGCS, ContentApp,
# NRNCS in parallel with make -j$(nproc).
#
# Usage: bash pull-and-build-vms.sh
# Prerequisites:
#   - VMs 101 and 102 running and SSH-reachable
#   - SSH key ~/.ssh/id_ed25519_hermes deployed on both VMs
#
# Order: Run before deploy-spec015/deploy.sh when binaries need updating

set -e

SSH_KEY=~/.ssh/id_ed25519_hermes
BASE=/root/workspace/novagenesis
BRANCH=AIOPT2

VM101="root@192.168.0.61"
VM102="root@192.168.0.36"

echo "=== Pull + Build Script for Alpine VMs ==="
echo ""

# Check VM reachability
for VM in "$VM101" "$VM102"; do
    HOST=$(echo $VM | cut -d@ -f2)
    echo -n "Checking $HOST ... "
    if ssh -o ConnectTimeout=3 -o StrictHostKeyChecking=no -i "$SSH_KEY" "$VM" "echo ok" >/dev/null 2>&1; then
        echo "REACHABLE"
    else
        echo "UNREACHABLE — aborting"
        exit 1
    fi
done

echo ""

# Build command to run on each VM
BUILD_CMD="
echo '--- Git status on ${BASE} ---' && \
cd ${BASE} && \
echo 'Branch: \$(git rev-parse --abbrev-ref HEAD)' && \
echo 'Stashing local changes...' && \
git stash 2>&1 && \
echo 'Pulling latest ${BRANCH}...' && \
git pull origin ${BRANCH} 2>&1 && \
echo 'Git HEAD: \$(git rev-parse --short HEAD)' && \
echo '--- Building ---' && \
cd ${BASE}/cmake-build-debug && \
cmake .. 2>&1 | tail -3 && \
make -j\$(nproc) 2>&1 | tail -5 && \
echo '--- Build complete ---' && \
echo 'Binaries:' && \
ls -la ${BASE}/cmake-build-debug/PGCS ${BASE}/cmake-build-debug/ContentApp ${BASE}/cmake-build-debug/NRNCS 2>&1
"

# Run on VM 101 (Repository) in background
echo "=== VM 101 (Repository — 192.168.0.61) ==="
ssh -o StrictHostKeyChecking=no -i "$SSH_KEY" "$VM101" "$BUILD_CMD" &
PID101=$!

sleep 2

# Run on VM 102 (Source) in foreground
echo "=== VM 102 (Source — 192.168.0.36) ==="
ssh -o StrictHostKeyChecking=no -i "$SSH_KEY" "$VM102" "$BUILD_CMD"
STATUS102=$?

# Wait for VM 101
echo ""
echo "=== Waiting for VM 101 to finish... ==="
wait $PID101
STATUS101=$?

echo ""
echo "=== Results ==="
echo "VM 101 (Repository @ 192.168.0.61): $([ $STATUS101 -eq 0 ] && echo 'OK' || echo 'FAILED')"
echo "VM 102 (Source @ 192.168.0.36):     $([ $STATUS102 -eq 0 ] && echo 'OK' || echo 'FAILED')"

if [ $STATUS101 -eq 0 ] && [ $STATUS102 -eq 0 ]; then
    echo ""
    echo "=== BUILD SUCCESS on both VMs ==="
    echo "Next step: deploy the binaries with deploy-spec015/deploy.sh"
    echo "(or copy directly from cmake-build-debug/ if SSH paths differ)"
else
    echo ""
    echo "=== BUILD FAILED on one or more VMs ==="
    echo "Check the output above for errors."
    exit 1
fi