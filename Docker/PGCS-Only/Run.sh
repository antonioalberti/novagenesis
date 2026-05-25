#!/bin/bash
# PGCS-Only container entry point
# Usage: docker run ... ng-pgcs:latest [args for PGCS]
# Default: broadcast discovery mode

BASE=/home/ng/workspace/novagenesis
IO=${BASE}/IO/PGCS/
BUILD=${BASE}/Build

# Create IO directory if needed
mkdir -p ${IO}

# Copy PGCS.ini from host IO if available (mounted volume)
if [ -f "${BASE}/IO/PGCS/PGCS.ini" ]; then
    cp ${BASE}/IO/PGCS/PGCS.ini ${IO}/
fi

cd ${BUILD}

# Run PGCS with args passed via docker run, or default -de mode
if [ $# -gt 0 ]; then
    exec ./PGCS "$@"
else
    exec ./PGCS ${IO} 0 Intra_Domain -de Ethernet eth0 1200
fi