#!/bin/ash
# Phase3: Enable SSH key authentication
# Run as root on installed system

set -e

echo "=== Phase3: SSH Setup ==="

# Mount shared folder if not already mounted
mount -t vboxsf AlpineVMs /mnt 2>/dev/null || echo "Already mounted or mount failed"

# Ensure sshd is installed and running
apk add openssh 2>/dev/null || true
rc-update add sshd default
rc-service sshd start 2>/dev/null || true

# Create .ssh directory
mkdir -p /root/.ssh
chmod 700 /root/.ssh

# Copy authorized_keys from shared folder
if [ -f /mnt/authorized_keys ]; then
    cp /mnt/authorized_keys /root/.ssh/authorized_keys
    chmod 600 /root/.ssh/authorized_keys
    echo "Authorized keys copied from shared folder"
else
    echo "WARNING: /mnt/authorized_keys not found"
    echo "Please mount shared folder first: mount -t vboxsf AlpineVMs /mnt"
fi

# Configure sshd for key authentication
sed -i 's/#*PubkeyAuthentication.*/PubkeyAuthentication yes/' /etc/ssh/sshd_config
sed -i 's/#*PasswordAuthentication.*/PasswordAuthentication no/' /etc/ssh/sshd_config
sed -i 's/#*KbdInteractiveAuthentication.*/KbdInteractiveAuthentication no/' /etc/ssh/sshd_config
sed -i 's/#*PermitRootLogin.*/PermitRootLogin prohibit-password/' /etc/ssh/sshd_config

# Restart sshd
rc-service sshd restart

echo "=== SSH enabled ==="
echo "Test from host: ssh -i ~/.ssh/novagenesis_vms root@<IP>"