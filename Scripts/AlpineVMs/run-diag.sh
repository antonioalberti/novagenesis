#!/bin/ash
mount -t vboxsf AlpineVMs /mnt 2>/dev/null
ash /mnt/check-vm-state.sh > /mnt/diag.txt 2>&1
cat /mnt/diag.txt