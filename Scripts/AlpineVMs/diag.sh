#!/bin/ash
mount -t vboxsf AlpineVMs /mnt
ash /mnt/check-vm-state.sh > /mnt/d.txt
cat /mnt/d.txt