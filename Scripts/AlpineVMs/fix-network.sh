#!/bin/ash
# Emergency network fix - run as root on VM after reboot

set -e

echo "=== Network Diagnostics ==="

# 1. Check current state
echo "--- Current network config ---"
ip addr show 2>/dev/null || echo "No IP"
cat /etc/network/interfaces 2>/dev/null || echo "No interfaces file"
cat /etc/hostname 2>/dev/null || echo "No hostname"

# 2. Determine IP from argument or hostname
IP_ARG="$1"
HOSTNAME=$(cat /etc/hostname 2>/dev/null || echo "localhost")

if [ -n "$IP_ARG" ]; then
    IP="$IP_ARG"
    echo "Using provided IP: $IP"
elif [ "$HOSTNAME" = "source36" ]; then
    IP="192.168.0.36"
elif [ "$HOSTNAME" = "repo61" ]; then
    IP="192.168.0.61"
else
    echo "ERROR: Unknown hostname '$HOSTNAME'"
    echo "Please specify IP manually:"
    echo "  ash /mnt/fix-network.sh 192.168.0.36"
    exit 1
fi

echo "Hostname: $HOSTNAME"
echo "IP: $IP"

# 3. Fix /etc/network/interfaces
echo "--- Fixing /etc/network/interfaces ---"
cat > /etc/network/interfaces <<EOF
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
    address $IP
    netmask 255.255.255.0
    gateway 192.168.0.1
EOF

# 4. Fix DNS
echo "--- Fixing DNS ---"
echo "nameserver 8.8.8.8" > /etc/resolv.conf

# 5. Enable networking
echo "--- Enabling networking ---"
rc-update add networking boot 2>/dev/null || true

# 6. Start networking
echo "--- Starting networking ---"
rc-service networking restart || {
    echo "WARNING: networking service failed, trying manual ifup..."
    ifup eth0 2>/dev/null || true
}

# 7. Verify
echo "--- Verification ---"
ip addr show eth0

# 8. Test internet
echo "--- Testing internet ---"
ping -c 2 8.8.8.8 && echo "SUCCESS: Internet OK" || echo "WARNING: No internet"