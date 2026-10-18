#!/bin/sh
#
# stop-content-scenario.sh — Stop and clean NovaGenesis ContentApp Docker scenario
#
# Usage: bash stop-content-scenario.sh <repos> <sources>

REPOS=${1:-2}
SOURCES=${2:-2}

echo "=== Stopping ContentApp scenario ==="

# Stop in reverse order: sources first, then repos, then core
for j in $(seq 1 $SOURCES); do
    docker stop "Source$j" 2>/dev/null && echo "  Stopped Source$j"
done

for i in $(seq 1 $REPOS); do
    docker stop "Repository$i" 2>/dev/null && echo "  Stopped Repository$i"
done

docker stop Core0 2>/dev/null && echo "  Stopped Core0"

# Remove containers
for j in $(seq 1 $SOURCES); do
    docker rm "Source$j" 2>/dev/null
done
for i in $(seq 1 $REPOS); do
    docker rm "Repository$i" 2>/dev/null
done
docker rm Core0 2>/dev/null

# Remove network
docker network rm ng-content-net 2>/dev/null && echo "  Removed network ng-content-net"

# Clean up
docker container prune -f > /dev/null 2>&1
docker network prune -f > /dev/null 2>&1

echo "=== Done ==="
