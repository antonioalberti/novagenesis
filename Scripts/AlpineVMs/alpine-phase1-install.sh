#!/bin/ash
# Phase 1: Install Alpine to disk with static IP (no internet required)
# Run as root on Alpine Live ISO

set -e

VM_TYPE="$1"

if [ "$VM_TYPE" != "source" ] && [ "$VM_TYPE" != "repo" ]; then
    echo "Usage: ash /mnt/alpine-phase1-install.sh <source|repo>"
    exit 1
fi

if [ "$VM_TYPE" = "source" ]; then
    IP="192.168.0.36"
    HOSTNAME="source36"
else
    IP="192.168.0.61"
    HOSTNAME="repo61"
fi

# Detect network interface (first non-loopback) - BusyBox compatible
IFACE=$(ip link show | grep -v "lo:" | head -1 | cut -d: -f2 | tr -d ' ')
if [ -z "$IFACE" ]; then
    echo "ERROR: No network interface detected!"
    exit 1
fi
echo "Detected network interface: $IFACE"

echo "=== Phase1: Installing $HOSTNAME to disk ==="

# 1. Hostname
setup-hostname "$HOSTNAME"

# 2. Network - STATIC IP
cat > /etc/network/interfaces <<EOF
auto lo
iface lo inet loopback

auto $IFACE
iface $IFACE inet static
    address $IP
    netmask 255.255.255.0
    gateway 192.168.0.1
EOF

# 3. DNS
echo "nameserver 8.8.8.8" > /etc/resolv.conf

# 4. Enable networking
rc-update add networking boot

# 5. Install to disk
echo "=== Installing Alpine to disk (this takes ~2 minutes) ==="
echo "y" | setup-disk -m sys /dev/sda

# 6. Ensure target is mounted
if [ ! -d "/mnt/target/etc" ]; then
    echo "ERROR: /mnt/target not found. Trying to find mount point..."
    ls /mnt/
    exit 1
fi

# 7. Copy ALL network config to installed system
echo "=== Persisting network configuration ==="
mkdir -p /mnt/target/etc/network
cp /etc/network/interfaces /mnt/target/etc/network/interfaces
# Also copy to target's network interfaces
cp /etc/resolv.conf /mnt/target/etc/resolv.conf
echo "$HOSTNAME" > /mnt/target/etc/hostname

# 8. Ensure networking service is enabled in installed system
chroot /mnt/target rc-update add networking boot 2>/dev/null || true
chroot /mnt/target rc-update add hostname boot 2>/dev/null || true

# 9. Verify persistence
echo "=== Verifying persisted config ==="
echo "--- /mnt/target/etc/network/interfaces ---"
cat /mnt/target/etc/network/interfaces
echo "--- /mnt/target/etc/hostname ---"
cat /mnt/target/etc/hostname

echo "=== Phase 1 complete ==="
echo "Rebooting into installed system..."
reboot
