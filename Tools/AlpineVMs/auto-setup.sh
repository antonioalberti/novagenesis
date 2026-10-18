#!/bin/ash
VM_TYPE="$1"
if [ -z "$VM_TYPE" ]; then
    echo "Usage: ash /mnt/auto-setup.sh [source|repo]"
    exit 1
fi
mount -t vboxsf AlpineVMs /mnt 2>/dev/null
if mount | grep -q ' / type tmpfs'; then
    echo "Live ISO - installing..."
    ash /mnt/alpine-phase1-install.sh "$VM_TYPE"
else
    echo "Disk boot - saving diagnostics..."
    ash /mnt/check-vm-state.sh > /mnt/diag.txt 2>&1
    echo "Saved to /mnt/diag.txt"
    ip addr show | grep inet
fi