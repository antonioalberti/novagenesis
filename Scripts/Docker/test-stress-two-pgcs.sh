#!/bin/bash
#
# test-stress-two-pgcs.sh — Launch 2 PGCS containers with StressTest enabled
# to verify bidirectional Ethernet traffic and message delivery under stress.
#
# Usage: sudo bash Scripts/Docker/test-stress-two-pgcs.sh

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BASE="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "=== PGCS StressTest: 2 PGCS instances ==="

# Clean up from previous runs
docker stop  pgcs1 pgcs2 2>/dev/null || true
docker rm    pgcs1 pgcs2 2>/dev/null || true
docker network rm pgcs-test-net 2>/dev/null || true
docker container prune -f > /dev/null 2>&1

# Create isolated network
docker network create --driver bridge pgcs-test-net

# Copy PGCS.ini with StressTest=1 to temp location
TMPDIR=$(mktemp -d)
cp "$BASE/IO/PGCS/PGCS.ini" "$TMPDIR/"
# Enable StressTest
sed -i 's/^StressTest .*/StressTest 1/' "$TMPDIR/PGCS.ini"
echo "StressTest=1"

# Launch PGCS 1
echo
echo "=== Starting PGCS1 ==="
docker run -itd \
    --privileged \
    --network pgcs-test-net \
    --name pgcs1 \
    ng-contentapp:latest \
    bash -c '
      cd /home/ng/workspace/novagenesis/Build
      mkdir -p /home/ng/workspace/novagenesis/IO/PGCS/
      cp /home/ng/workspace/novagenesis/IO/PGCS/PGCS.ini /home/ng/workspace/novagenesis/IO/PGCS/ 2>/dev/null || true
      ./PGCS /home/ng/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -de Ethernet eth0 1200 2>&1 | tee /tmp/pgcs1.log
    '

# Copy StressTest-enabled PGCS.ini into container (overrides default)
docker cp "$TMPDIR/PGCS.ini" pgcs1:/home/ng/workspace/novagenesis/IO/PGCS/

# Wait for PGCS1 to initialize
sleep 3

# Launch PGCS 2
echo
echo "=== Starting PGCS2 ==="
docker run -itd \
    --privileged \
    --network pgcs-test-net \
    --name pgcs2 \
    ng-contentapp:latest \
    bash -c '
      cd /home/ng/workspace/novagenesis/Build
      mkdir -p /home/ng/workspace/novagenesis/IO/PGCS/
      ./PGCS /home/ng/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -de Ethernet eth0 1200 2>&1 | tee /tmp/pgcs2.log
    '

docker cp "$TMPDIR/PGCS.ini" pgcs2:/home/ng/workspace/novagenesis/IO/PGCS/
rm -rf "$TMPDIR"

echo
echo "=== Both PGCSes running ==="
echo
echo "Monitor traffic:  sudo tcpdump -i any -nn 'ether proto 0x1234'"
echo "PGCS1 log:        docker logs -f pgcs1"
echo "PGCS2 log:        docker logs -f pgcs2"
echo "StressTest stats: docker exec pgcs1 cat /home/ng/workspace/novagenesis/IO/PGCS/StressTest_Stats.txt"
echo
echo "Stop:             docker stop pgcs1 pgcs2 && docker rm pgcs1 pgcs2 && docker network rm pgcs-test-net"