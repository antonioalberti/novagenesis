#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

docker container prune -f

echo "Creating NRNCS container, which stands for Name Resolution and Network Cache Service" 

sh "$SCRIPT_DIR/run-Core-NRNCS.sh" 0

echo "Creating name binding testing application" 

sh "$SCRIPT_DIR/run-PGCS-NBTestApp.sh"
