#!/bin/ash
# Auto-config for NovaGenesis VMs - PERSISTENT version
# Usage: autoconfig.sh <source|repo>

set -e

VM_TYPE="$1"
: "${VM_IP:?Set VM_IP for this guest}"
: "${VM_HOSTNAME:?Set VM_HOSTNAME for this guest}"
: "${GATEWAY:?Set GATEWAY for this network}"
NETMASK=${NETMASK:-255.255.255.0}

# Check if running on live system (tmpfs root)
if mount | grep -q ' / type tmpfs'; then
    echo "WARNING: Running on live Alpine system (tmpfs root)."
    echo "All changes will be LOST on reboot!"
    echo "Please install Alpine to disk first using alpine-phase1-install.sh"
    echo "Continuing anyway in 5 seconds..."
    sleep 5
fi

# Detect network interface (first non-loopback) - BusyBox compatible
IFACE=$(ip link show | grep -v "lo:" | head -1 | cut -d: -f2 | tr -d ' ')
if [ -z "$IFACE" ]; then
    echo "ERROR: No network interface detected!"
    exit 1
fi
echo "Detected network interface: $IFACE"

echo "=== Configuring $VM_HOSTNAME with IP $VM_IP ==="

# 1. Hostname (persistent)
echo "$VM_HOSTNAME" > /etc/hostname
hostname "$VM_HOSTNAME"

# 2. Network config (persistent)
cat > /etc/network/interfaces <<EOF
auto lo
iface lo inet loopback

auto $IFACE
iface $IFACE inet static
    address $VM_IP
    netmask $NETMASK
    gateway $GATEWAY
EOF

# 3. DNS (persistent)
echo "nameserver 8.8.8.8" > /etc/resolv.conf
echo "nameserver 8.8.4.4" >> /etc/resolv.conf

# 4. Enable networking on boot (CRITICAL for persistence)
rc-update add networking boot 2>/dev/null || true
rc-update add hostname boot 2>/dev/null || true

# 5. Start networking now
rc-service networking restart 2>/dev/null || ifup $IFACE

# 6. Verify network
echo "=== Network status ==="
ip addr show $IFACE

# 7. Install SSH
apk add openssh 2>/dev/null || true
rc-update add sshd default 2>/dev/null || true
rc-service sshd start 2>/dev/null || true

# 8. Configure SSH keys
mkdir -p /root/.ssh
chmod 700 /root/.ssh
if [ ! -f /mnt/authorized_keys ]; then
    echo "ERROR: provide operator-managed /mnt/authorized_keys before enabling SSH" >&2
    exit 1
fi
cp /mnt/authorized_keys /root/.ssh/authorized_keys
chmod 600 /root/.ssh/authorized_keys

# 9. Configure sshd
sed -i 's/#*PubkeyAuthentication.*/PubkeyAuthentication yes/' /etc/ssh/sshd_config
sed -i 's/#*PasswordAuthentication.*/PasswordAuthentication no/' /etc/ssh/sshd_config
sed -i 's/#*KbdInteractiveAuthentication.*/KbdInteractiveAuthentication no/' /etc/ssh/sshd_config
sed -i 's/#*PermitRootLogin.*/PermitRootLogin prohibit-password/' /etc/ssh/sshd_config
rc-service sshd restart 2>/dev/null || true

# 10. Verify SSH
echo "=== SSH status ==="
rc-service sshd status 2>/dev/null || echo "sshd status unknown"

echo "=== Config complete ==="
echo "Hostname: $(hostname)"
echo "IP: $VM_IP"
echo "Test: ping -c 1 8.8.8.8"
