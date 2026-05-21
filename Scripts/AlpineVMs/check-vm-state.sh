#!/bin/ash
# Diagnostic script to check VM state and network configuration
# Run as: ash /mnt/check-vm-state.sh

echo "=========================================="
echo "Alpine VM State Diagnostic"
echo "=========================================="

# Check if running on live system
echo ""
echo "1. Checking if running on Live ISO (tmpfs)..."
if mount | grep -q ' / type tmpfs'; then
    echo "   ⚠️  RUNNING ON LIVE ISO (tmpfs root)"
    echo "   All changes to /etc/ will be LOST on reboot!"
    echo "   ➡️  Solution: Run alpine-phase1-install.sh to install to disk"
else
    echo "   ✅ Running on installed system (disk)"
fi

# Check root filesystem
echo ""
echo "2. Root filesystem:"
mount | grep ' / '

# Check network interface
echo ""
echo "3. Network interfaces:"
ip link show | grep -E '^[0-9]+:'

# Check current network config
echo ""
echo "4. Network configuration (/etc/network/interfaces):"
if [ -f /etc/network/interfaces ]; then
    cat /etc/network/interfaces
else
    echo "   ⚠️  No /etc/network/interfaces found!"
fi

# Check if networking service is enabled
echo ""
echo "5. OpenRC services (networking):"
if command -v rc-status >/dev/null 2>&1; then
    rc-status | grep -E 'networking|hostname'
    echo ""
    echo "   Boot runlevel:"
    rc-status boot | grep -E 'networking|hostname'
else
    echo "   ⚠️  OpenRC not available (live system?)"
fi

# Check hostname
echo ""
echo "6. Hostname:"
echo "   Current: $(hostname)"
echo "   /etc/hostname: $(cat /etc/hostname 2>/dev/null || echo 'NOT SET')"

# Check DNS
echo ""
echo "7. DNS configuration (/etc/resolv.conf):"
cat /etc/resolv.conf 2>/dev/null || echo "   ⚠️  No /etc/resolv.conf found!"

# Check disk installation
echo ""
echo "8. Disk installation status:"
if [ -d /mnt/target ]; then
    echo "   /mnt/target exists (installation in progress?)"
    ls -la /mnt/target/ 2>/dev/null | head -5
else
    echo "   /mnt/target not mounted"
fi

# Recommendations
echo ""
echo "=========================================="
echo "RECOMMENDATIONS:"
echo "=========================================="

if mount | grep -q ' / type tmpfs'; then
    echo ""
    echo "⚠️  You are running on Live ISO!"
    echo ""
    echo "Correct workflow:"
    echo "  1. Run: ash /mnt/alpine-phase1-install.sh source   (or 'repo')"
    echo "  2. Wait for installation to complete (~2 minutes)"
    echo "  3. VM will reboot automatically"
    echo "  4. After reboot, login as root (password: novagenesis)"
    echo "  5. Run: ash /mnt/alpine-phase2-packages.sh"
    echo "  6. Continue with phase3, phase4 as needed"
    echo ""
    echo "DO NOT use autoconfig.sh on live ISO - it won't persist!"
else
    echo ""
    echo "✅ You are running on installed system."
    echo ""
    echo "If network is not working after reboot:"
    echo "  1. Check interface name: ip link show"
    echo "  2. Update /etc/network/interfaces with correct interface"
    echo "  3. Run: rc-service networking restart"
    echo "  4. Enable on boot: rc-update add networking boot"
    echo ""
    echo "Or run: ash /mnt/alpine-phase1-install.sh source/repo"
    echo "        (it will reinstall and fix network config)"
fi

echo ""
echo "=========================================="