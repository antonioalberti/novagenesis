#!/bin/bash
# SPEC-015 deployment script — copy rebuilt binaries to VMs 101 and 102
# Run from VM 100 after the Alpine VMs are network-reachable
set -e

DEPLOY_DIR="/home/gandalf/workspace/novagenesis/deploy-spec015"
BINARIES="ContentApp PGCS NRNCS"
VM101="root@192.168.0.101"
VM102="root@192.168.0.102"
REMOTE_PATH="/root/novagenesis"

echo "=== SPEC-015 Deploy Script ==="
echo "Binaries: $DEPLOY_DIR/$BINARIES"
echo ""

# Check VM reachability
for VM in "$VM101" "$VM102"; do
    HOST=$(echo $VM | cut -d@ -f2)
    echo -n "Checking $HOST ... "
    if ssh -o ConnectTimeout=3 -o StrictHostKeyChecking=no "$VM" "echo ok" >/dev/null 2>&1; then
        echo "REACHABLE"
    else
        echo "UNREACHABLE — aborting"
        exit 1
    fi
done

echo ""
echo "Deploying to VM 101 ($VM101)..."
for bin in $BINARIES; do
    echo "  Copying $bin ..."
    scp -o StrictHostKeyChecking=no "$DEPLOY_DIR/$bin" "$VM101:$REMOTE_PATH/$bin"
done

echo ""
echo "Deploying to VM 102 ($VM102)..."
for bin in $BINARIES; do
    echo "  Copying $bin ..."
    scp -o StrictHostKeyChecking=no "$DEPLOY_DIR/$bin" "$VM102:$REMOTE_PATH/$bin"
done

echo ""
echo "=== Deploy complete. Verify with: ==="
echo "  ssh $VM101 'ls -la $REMOTE_PATH/ContentApp $REMOTE_PATH/PGCS $REMOTE_PATH/NRNCS'"
echo "  ssh $VM102 'ls -la $REMOTE_PATH/ContentApp $REMOTE_PATH/PGCS $REMOTE_PATH/NRNCS'"
