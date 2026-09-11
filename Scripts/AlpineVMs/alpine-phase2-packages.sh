#!/bin/ash
# Phase2: Install packages after Alpine is on disk
# Run as root after first reboot (Alpine on disk)

set -e

echo "=== Phase2: Installing packages ==="

# Verify network
echo "=== Verifying network ==="
ip addr show eth0

if ! ping -c 1 -W 5 8.8.8.8 >/dev/null 2>&1; then
    echo "ERROR: No internet. Check network."
    exit 1
fi
echo "Internet OK"

# Setup repos
echo "=== Setting up repositories ==="
cat > /etc/apk/repositories <<EOF
https://dl-cdn.alpinelinux.org/alpine/v3.23/main
https://dl-cdn.alpinelinux.org/alpine/v3.23/community
EOF

# Update
echo "=== Updating packages ==="
apk update

# Install SSH
echo "=== Installing OpenSSH ==="
apk add openssh
rc-update add sshd default
rc-service sshd start

# Do not embed a root password in a public script. Set credentials locally
# through the console or an approved secret-management procedure.

# Skip Guest Additions (causes hangs)
echo "=== Skipping Guest Additions (use PS/2 mouse) ==="

# Install Python + libraries
echo "=== Installing Python ==="
apk add python3 py3-pip py3-numpy py3-pillow

echo "=== Phase2 complete ==="
echo "Mouse: Use Right Ctrl to release (PS/2 mouse set)"