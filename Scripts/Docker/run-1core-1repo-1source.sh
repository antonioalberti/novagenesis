#!/bin/sh
# run-1core-1repo-1source.sh
# Simplified scenario: 1 Core (NRNCS+PGCS), 1 Repository, 1 Source

set -e
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BASE="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "=== NovaGenesis: 1 Core + 1 Repo + 1 Source ==="
echo

# Clean up
echo "=== Cleaning ==="
docker stop Core0 Repository1 Source1 2>/dev/null || true
docker rm   Core0 Repository1 Source1 2>/dev/null || true
docker network rm ng-content-net 2>/dev/null || true
docker container prune -f > /dev/null 2>&1

# Create network
echo "=== Creating network ==="
docker network create --driver bridge ng-content-net

# Core0: NRNCS + PGCS
echo
echo "=== Starting Core0 (NRNCS + PGCS) ==="
docker run -itd \
    --privileged \
    --network ng-content-net \
    --ipc=shareable \
    --name Core0 \
    ng-nrncs:latest

sleep 5

# Repository1
echo
echo "=== Starting Repository1 (PGCS + ContentApp) ==="
docker run -itd \
    --privileged \
    --network ng-content-net \
    --ipc=container:Core0 \
    -e CA_PATH="/home/ng/workspace/novagenesis/IO/Repository1/" \
    -e CA_TYPE="Repository" \
    --name Repository1 \
    ng-contentapp:latest

docker exec -w /home/ng/workspace/novagenesis/IO/ Repository1 mkdir -p Repository1
docker exec -w /home/ng/workspace/novagenesis/IO/ Repository1 chmod -R 777 Repository1
docker cp "$SCRIPT_DIR/Includes-ContentApp/App.ini" "Repository1:/home/ng/workspace/novagenesis/IO/Repository1/"

# Source1
echo
echo "=== Starting Source1 (PGCS + ContentApp + photos) ==="
docker run -itd \
    --privileged \
    --network ng-content-net \
    --ipc=container:Core0 \
    -e CA_PATH="/home/ng/workspace/novagenesis/IO/Source1/" \
    -e CA_TYPE="Source" \
    --name Source1 \
    ng-contentapp:latest

docker exec -w /home/ng/workspace/novagenesis/IO/ Source1 mkdir -p Source1
docker exec -w /home/ng/workspace/novagenesis/IO/ Source1 chmod -R 777 Source1
docker cp "$SCRIPT_DIR/Includes-ContentApp/App.ini" "Source1:/home/ng/workspace/novagenesis/IO/Source1/"
docker cp "$SCRIPT_DIR/Includes-ContentApp/make-photos.sh" "Source1:/home/ng/workspace/novagenesis/IO/Source1/"

sleep 3
echo "  Generating 10 photos (800x600)..."
docker exec -w /home/ng/workspace/novagenesis/IO/Source1/ Source1 sh make-photos.sh 10 800 600

echo
echo "=== Scenario ready ==="
echo "  Core0:       docker logs -f Core0"
echo "  Repository1: docker exec Repository1 ls IO/Repository1/"
echo "  Source1:     docker exec Source1 ls IO/Source1/"
echo "  CPU:         docker stats Core0 Repository1 Source1"
echo "  Stop:        docker stop Core0 Repository1 Source1 && docker rm Core0 Repository1 Source1 && docker network rm ng-content-net"
