#!/bin/ash
# NovaGenesis setup script - run after Phase2
# Usage: ash /mnt/alpine-phase4-ng.sh <source|repo>

set -e
: "${NG_REPO_PATH:?Set NG_REPO_PATH before running}"

VM_TYPE="$1"

if [ "$VM_TYPE" != "source" ] && [ "$VM_TYPE" != "repo" ]; then
    echo "Usage: ash /mnt/alpine-phase4-ng.sh <source|repo>"
    exit 1
fi

echo "=== NovaGenesis Setup for $VM_TYPE ==="

# Install build dependencies
echo "=== Installing build tools ==="
apk add build-base cmake git linux-headers libstdc++-dev python3 py3-pip py3-numpy py3-pillow

# Create workspace
mkdir -p "$(dirname "$NG_REPO_PATH")"
cd "$(dirname "$NG_REPO_PATH")"

# Clone NovaGenesis
if [ ! -d "$NG_REPO_PATH" ]; then
    echo "=== Cloning NovaGenesis ==="
    git clone https://github.com/antonioalberti/novagenesis.git
fi

cd "$NG_REPO_PATH"

# Build
echo "=== Building NovaGenesis ==="
mkdir -p cmake-build-debug
cd cmake-build-debug

# Fix CMakeLists.txt - remove profiling flags that cause linker errors on Alpine
echo "=== Fixing CMakeLists.txt ==="
cd ..
sed -i 's/-p -pg //' CMakeLists.txt
sed -i 's/-lpthread -lrt//' CMakeLists.txt
cd cmake-build-debug

cmake ..
make -j1

# Create IO directories
echo "=== Setting up directories ==="
mkdir -p "$NG_REPO_PATH/IO/logs"
if [ "$VM_TYPE" = "source" ]; then
    mkdir -p "$NG_REPO_PATH/IO/Source1"
    # Create startup script for source
    cat > /root/start-ng.sh <<EOF
#!/bin/ash
cd $NG_REPO_PATH/cmake-build-debug
./PGCS &
./NRNCS &
./ContentApp &
echo "NovaGenesis Source started"
EOF
else
    mkdir -p "$NG_REPO_PATH/IO/Repository1"
    # Create startup script for repo
    cat > /root/start-ng.sh <<EOF
#!/bin/ash
cd $NG_REPO_PATH/cmake-build-debug
./PGCS &
./ContentApp &
echo "NovaGenesis Repository started"
EOF
fi

chmod +x /root/start-ng.sh

echo "=== Setup complete ==="
echo "To start: /root/start-ng.sh"
echo "To stop: ash /mnt/stop-ng.sh"