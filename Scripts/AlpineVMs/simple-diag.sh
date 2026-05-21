#!/bin/ash
mount -t vboxsf AlpineVMs /mnt
ash /mnt/check-vm-state.sh > /mnt/diag.txt
cat /mnt/diag.txt