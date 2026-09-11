#!/bin/ash
# NovaGenesis Source VM Setup Script
# Run as root on a source guest after exporting VM_IP, GATEWAY and NG_REPO_PATH.

set -e
: "${VM_IP:?Set VM_IP before running}"
: "${GATEWAY:?Set GATEWAY before running}"
: "${NG_REPO_PATH:?Set NG_REPO_PATH before running}"
WORKSPACE=$(dirname "$NG_REPO_PATH")

echo "=== NovaGenesis Source VM Setup ==="

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
mkdir -p "$NG_REPO_PATH/IO/Source1" "$NG_REPO_PATH/IO/logs"

# Create startup script for Source VM
cat > /root/start-ng-source.sh <<EOF
#!/bin/ash
cd $NG_REPO_PATH/cmake-build-debug
./PGCS &
./NRNCS &
./ContentApp &
echo "NovaGenesis Source processes started"
EOF
chmod +x /root/start-ng-source.sh

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
echo "To start NovaGenesis: /root/start-ng-source.sh"