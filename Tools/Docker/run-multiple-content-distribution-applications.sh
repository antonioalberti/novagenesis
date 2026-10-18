#!/bin/sh
#
# run-multiple-content-distribution-applications.sh
# Launch a complete NovaGenesis ContentApp scenario with Docker.
#
# Usage: bash run-multiple-content-distribution-applications.sh <repos> <sources> <photos> <width> <height>
#   bash run-multiple-content-distribution-applications.sh 2 2 10 800 600
#
# Creates: Core0 (NRNCS + PGCS), Repository1..N, Source1..N
# Network: ng-content-net (Ethernet broadcast for PGCS discovery)

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPOS=${1:-2}
SOURCES=${2:-2}
PHOTOS=${3:-10}
WIDTH=${4:-800}
HEIGHT=${5:-600}

echo "=== NovaGenesis ContentApp Docker Scenario ==="
echo "  Repos: $REPOS | Sources: $SOURCES | Photos: ${PHOTOS}x${WIDTH}x${HEIGHT}"
echo

# Clean up previous run
echo "=== Cleaning previous containers ==="
docker stop Core0 Repository1 Repository2 Source1 Source2 2>/dev/null || true
docker rm Core0 Repository1 Repository2 Source1 Source2 2>/dev/null || true
docker network rm ng-content-net 2>/dev/null || true
docker container prune -f > /dev/null 2>&1

# Create network for PGCS Ethernet discovery
echo "=== Creating network ng-content-net ==="
docker network create --driver bridge ng-content-net

# Core0: NRNCS + PGCS (name resolution)
echo
echo "=== Starting Core0 (NRNCS + PGCS) ==="
docker run -itd \
    --privileged \
    --network ng-content-net \
    --ipc=shareable \
    --name Core0 \
    ng-nrncs:latest

# Wait for Core0 PGCS to be ready
sleep 5

# Repositories
for i in $(seq 1 $REPOS); do
    echo
    echo "=== Starting Repository$i (PGCS + ContentApp) ==="
    docker run -itd \
        --privileged \
        --network ng-content-net \
        --ipc=container:Core0 \
        -e CA_PATH="/home/ng/workspace/novagenesis/IO/Repository$i/" \
        -e CA_TYPE="Repository" \
        --name "Repository$i" \
        ng-contentapp:latest

    docker exec -w /home/ng/workspace/novagenesis/IO/ "Repository$i" mkdir -p "Repository$i"
    docker exec -w /home/ng/workspace/novagenesis/IO/ "Repository$i" chmod -R 777 "Repository$i"
    docker cp "$SCRIPT_DIR/Includes-ContentApp/App.ini" "Repository$i:/home/ng/workspace/novagenesis/IO/Repository$i/"
done

# Sources
for j in $(seq 1 $SOURCES); do
    echo
    echo "=== Starting Source$j (PGCS + ContentApp + photos) ==="
    docker run -itd \
        --privileged \
        --network ng-content-net \
        --ipc=container:Core0 \
        -e CA_PATH="/home/ng/workspace/novagenesis/IO/Source$j/" \
        -e CA_TYPE="Source" \
        --name "Source$j" \
        ng-contentapp:latest

    docker exec -w /home/ng/workspace/novagenesis/IO/ "Source$j" mkdir -p "Source$j"
    docker exec -w /home/ng/workspace/novagenesis/IO/ "Source$j" chmod -R 777 "Source$j"
    docker cp "$SCRIPT_DIR/Includes-ContentApp/App.ini" "Source$j:/home/ng/workspace/novagenesis/IO/Source$j/"
    docker cp "$SCRIPT_DIR/Includes-ContentApp/make-photos.sh" "Source$j:/home/ng/workspace/novagenesis/IO/Source$j/"

    sleep 3
    echo "  Generating $PHOTOS photos (${WIDTH}x${HEIGHT})..."
    docker exec -w /home/ng/workspace/novagenesis/IO/Source$j/ "Source$j" \
        sh make-photos.sh "$PHOTOS" "$WIDTH" "$HEIGHT"
done

echo
echo "=== Scenario ready ==="
echo "  Core0:       docker logs -f Core0"
echo "  Repositories: docker exec Repository1 ls IO/Repository1/"
echo "  Sources:      docker exec Source1 ls IO/Source1/"
echo "  Traffic:      sudo tcpdump -i any -nn 'ether proto 0x1234'"
echo "  Stop:         bash $SCRIPT_DIR/stop-content-scenario.sh $REPOS $SOURCES"
