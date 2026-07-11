#!/bin/ash
# Alpine Linux automated installation for NovaGenesis Source VM
# Run as root after booting from ISO

# Keyboard (manual - avoids setup-keymap dependency error)
echo 'KEYMAP="us"' > /etc/conf.d/keymaps
rc-update add keymaps boot 2>/dev/null || true

# Hostname
setup-hostname source36

# Network - STATIC IP on eth0 (permanent)
cat > /etc/network/interfaces <<'EOF'
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
    address 192.168.0.36
    netmask 255.255.255.0
    gateway 192.168.0.1
EOF

# Timezone
setup-timezone -z UTC

# Install OpenRC and OpenSSH
apk add openrc openssh

# Root password
echo "root:novagenesis" | chpasswd

# Enable and start SSH
rc-update add sshd default
rc-service sshd start

# Install to disk (sys mode, auto-partition)
echo "y" | setup-disk -m sys /dev/sda

# Reboot
echo "Installation complete. Rebooting..."
reboot