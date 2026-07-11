#!/bin/ash
# NovaGenesis Repository VM Setup Script
# Run as root on repo61 (192.168.0.61)

set -e

echo "=== NovaGenesis Repository VM Setup ==="

# Update and install dependencies
apk update
apk add build-base cmake git linux-headers libstdc++-dev

# Create workspace
mkdir -p /root/workspace
cd /root/workspace

# Clone NovaGenesis repository
if [ ! -d "novagenesis" ]; then
    git clone https://github.com/antonioalberti/novagenesis.git
fi

cd novagenesis

# Build the project
mkdir -p cmake-build-debug
cd cmake-build-debug
cmake ..
make -j$(nproc)

# Create IO directories
mkdir -p /root/workspace/novagenesis/IO/Repository1
mkdir -p /root/workspace/novagenesis/IO/logs

# Create startup script for Repository VM
cat > /root/start-ng-repo.sh <<'EOF'
#!/bin/ash
cd /root/workspace/novagenesis/cmake-build-debug
./PGCS &
./ContentApp &
echo "NovaGenesis Repository processes started"
EOF
chmod +x /root/start-ng-repo.sh

# Make network interface persistent - STATIC IP
cat > /etc/network/interfaces <<'EOF'
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet static
    address 192.168.0.61
    netmask 255.255.255.0
    gateway 192.168.0.1
EOF

# Enable networking on boot
rc-update add networking boot

echo "=== Setup complete ==="
echo "To start NovaGenesis: /root/start-ng-repo.sh"