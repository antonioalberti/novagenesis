#!/bin/ash
# NovaGenesis Repository VM Setup Script
# Run as root on a repository guest after exporting VM_IP, GATEWAY and NG_REPO_PATH.

set -e
: "${VM_IP:?Set VM_IP before running}"
: "${GATEWAY:?Set GATEWAY before running}"
: "${NG_REPO_PATH:?Set NG_REPO_PATH before running}"
WORKSPACE=$(dirname "$NG_REPO_PATH")

echo "=== NovaGenesis Repository VM Setup ==="

# Update and install dependencies
apk update
apk add build-base cmake git linux-headers libstdc++-dev

# Create workspace
mkdir -p "$WORKSPACE"
cd "$WORKSPACE"

# Clone NovaGenesis repository
if [ ! -d "$NG_REPO_PATH" ]; then
    git clone https://github.com/antonioalberti/novagenesis.git
fi

cd "$NG_REPO_PATH"

# Build the project
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make -j$(nproc)

# Create IO directories
mkdir -p "$NG_REPO_PATH/IO/Repository1" "$NG_REPO_PATH/IO/logs"

# Make network interface persistent - STATIC IP
cat > /etc/network/interfaces <<'EOF'
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
    address $VM_IP
    netmask 255.255.255.0
    gateway $GATEWAY
EOF

# Enable networking on boot
rc-update add networking boot

echo "=== Setup complete ==="
echo "Setup complete. Start the AIOPT3 scenario from the control host using Scripts/AlpineVMs/run_*.sh."