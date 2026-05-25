#!/bin/bash
#
# test-stress-deterministic.sh — 2 PGCS containers with deterministic -p mode
# Fixed MAC addresses for precise delay/jitter measurement.
# StressTest bidirectional: each PGCS sends and receives stress pings.
#
# Usage: sudo bash Scripts/Docker/test-stress-deterministic.sh

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BASE="$(cd "$SCRIPT_DIR/../.." && pwd)"

IMAGE="ng-pgcs:latest"

echo "=== PGCS Deterministic StressTest ==="
echo "Mode: -de (broadcast discovery)"
echo

# 1. Build the PGCS-only image
echo "=== Building PGCS-only Docker image ==="
docker build -t ${IMAGE} -f Docker/PGCS-Only/Dockerfile ${BASE}
echo

# 2. Clean up from previous runs
echo "=== Cleaning previous state ==="
docker stop  pgcs1 pgcs2 2>/dev/null || true
docker rm    pgcs1 pgcs2 2>/dev/null || true
docker network rm pgcs-stress-net 2>/dev/null || true
docker container prune -f > /dev/null 2>&1
rm -rf /tmp/pgcs1-io /tmp/pgcs2-io
echo

# 3. Create isolated bridge network
echo "=== Creating network ==="
docker network create --driver bridge pgcs-stress-net
echo

# 4. Prepare IO directories with StressTest-enabled PGCS.ini BEFORE starting
echo "=== Preparing IO directories ==="
mkdir -p /tmp/pgcs1-io/PGCS /tmp/pgcs2-io/PGCS
for IO in /tmp/pgcs1-io /tmp/pgcs2-io; do
    cp "$BASE/IO/PGCS/PGCS.ini" "${IO}/PGCS/"
    sed -i 's/^StressTest .*/StressTest 1/' "${IO}/PGCS/PGCS.ini"
    sed -i 's/^StressInterval .*/StressInterval 0.1/' "${IO}/PGCS/PGCS.ini"
done
echo "PGCS.ini:"
grep -E "Stress" /tmp/pgcs1-io/PGCS/PGCS.ini
echo

# 5. Launch PGCS1
echo "=== Starting PGCS1 ==="
docker run -d \
    --privileged \
    --ipc=private \
    --network pgcs-stress-net \
    --name pgcs1 \
    -v /tmp/pgcs1-io:/home/ng/workspace/novagenesis/IO \
    ${IMAGE} \
    /home/ng/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain \
    -de Ethernet eth0 1200

# 6. Launch PGCS2
echo
echo "=== Starting PGCS2 ==="
docker run -d \
    --privileged \
    --ipc=private \
    --network pgcs-stress-net \
    --name pgcs2 \
    -v /tmp/pgcs2-io:/home/ng/workspace/novagenesis/IO \
    ${IMAGE} \
    /home/ng/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain \
    -de Ethernet eth0 1200

# 7. Wait for discovery
echo
echo "=== Waiting 15s for hello discovery ==="
sleep 15
echo

# 8. Show status
echo "=== Status ==="
echo
echo "Container status:"
docker ps --filter "name=pgcs" --format "table {{.Names}}\t{{.Status}}"
echo

echo "Monitor:"
echo "  Traffic:   sudo tcpdump -i any -nn 'ether proto 0x1234'"
echo "  PGCS1 log: docker logs -f pgcs1"
echo "  PGCS2 log: docker logs -f pgcs2"
echo "  Stats1:    cat /tmp/pgcs1-io/PGCS/StressTest_Stats.txt"
echo "  Stats2:    cat /tmp/pgcs2-io/PGCS/StressTest_Stats.txt"
echo "  CPU:       docker stats pgcs1 pgcs2"
echo ""
echo "Debug (if crash):"
echo "  docker logs pgcs2 2>&1 | tail -100"
echo "  docker exec pgcs1 gdb -batch -ex bt -ex info registers /home/ng/workspace/novagenesis/Build/PGCS PID"
echo "  docker exec pgcs1 strace -p PGCS_PID 2>&1"
echo ""
echo "Stop:       docker stop pgcs1 pgcs2 && docker rm pgcs1 pgcs2 && docker network rm pgcs-stress-net"
echo

echo "=== Running — press Ctrl+C to stop watching logs, containers keep running ==="