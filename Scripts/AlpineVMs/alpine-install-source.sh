#!/bin/ash
# Alpine Linux automated installation for NovaGenesis Source VM
# Run as root after booting from ISO

# Keyboard (manual - avoids setup-keymap dependency error)
echo 'KEYMAP="us"' > /etc/conf.d/keymaps
rc-update add keymaps boot 2>/dev/null || true

# Deployment-specific values must be supplied by the operator.
: "${VM_HOSTNAME:?Set VM_HOSTNAME for this guest}"
: "${VM_IP:?Set VM_IP for this guest}"
: "${GATEWAY:?Set GATEWAY for this network}"
NETMASK=${NETMASK:-255.255.255.0}

# Hostname
setup-hostname "$VM_HOSTNAME"

# Network - STATIC IP on eth0 (permanent)
cat > /etc/network/interfaces <<'EOF'
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
    address $VM_IP
    netmask $NETMASK
    gateway $GATEWAY
EOF

# Timezone
setup-timezone -z UTC

# Install OpenRC and OpenSSH
apk add openrc openssh

# Do not embed or assign a password. Set a local console credential or install
# an operator-managed SSH key before enabling remote administration.
echo "No root password is configured by this script; use local operator setup."

# Enable and start SSH
rc-update add sshd default
rc-service sshd start

# Install to disk (sys mode, auto-partition)
echo "y" | setup-disk -m sys /dev/sda

# Reboot
echo "Installation complete. Rebooting..."
reboot