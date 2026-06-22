#!/bin/bash
# ContentApp Source — generates photos and starts ContentApp in Source mode.
# Prerequisite: PGCS must already be running (sudo sh run_PGCS.sh).
#
# Usage: sudo sh run_Source.sh [num_photos] [width] [height]
#   sudo sh run_Source.sh            # defaults: 50 photos, 800x600
#   sudo sh run_Source.sh 100 640 480
#
# Photo generation runs as the invoking user (no root needed).
# Only ContentApp needs root (raw sockets).

set -e

BASE=/home/gandalf/workspace/novagenesis
PHOTOS=${1:-50}
WIDTH=${2:-800}
HEIGHT=${3:-600}
IO_DIR="$BASE/IO/Source1"

# If running as root, drop to the original user for photo generation
REAL_USER="${SUDO_USER:-$USER}"
REAL_HOME=$(eval echo "~$REAL_USER")

echo "=== ContentApp Source ==="

# Ensure IO directory
if [ ! -d "$IO_DIR" ]; then
    echo "Creating $IO_DIR ..."
    mkdir -p "$IO_DIR"
    chown "$REAL_USER:$REAL_USER" "$IO_DIR" 2>/dev/null || true
fi

# Generate photos as the real user (Python packages are installed for them)
echo "Generating ${PHOTOS} photos (${WIDTH}x${HEIGHT}) ..."
if [ "$(id -u)" = "0" ] && [ -n "$SUDO_USER" ]; then
    # Running via sudo — use real user's Python
    sudo -u "$REAL_USER" python3 "$BASE/Scripts/Python/BuildPhotos.py" different "$PHOTOS" "$WIDTH" "$HEIGHT"
else
    python3 "$BASE/Scripts/Python/BuildPhotos.py" different "$PHOTOS" "$WIDTH" "$HEIGHT"
fi

# Move photos to IO/Source1
count=$(ls -1 ./*.jpg 2>/dev/null | wc -l)
if [ "$count" -gt 0 ]; then
    mv ./*.jpg "$IO_DIR/"
    chown "$REAL_USER:$REAL_USER" "$IO_DIR"/*.jpg 2>/dev/null || true
    echo "Moved ${count} photos to $IO_DIR"
else
    echo "WARNING: No photos generated."
fi

# Run ContentApp under gdb (debug mode — captures crash backtrace)
echo "Starting ContentApp in Source mode under gdb (IO: $IO_DIR) ..."
cd "$BASE/cmake-build-debug"
gdb -batch -ex "run" -ex "bt full" -ex "info registers" -ex "thread apply all bt full" -ex "quit" --args ./ContentApp "$IO_DIR/" Source
