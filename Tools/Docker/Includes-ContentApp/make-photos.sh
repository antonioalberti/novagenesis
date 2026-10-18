#!/bin/bash
# Generates test JPEG images using BuildPhotos.py (Python).
# Usage: sh make-photos.sh <count> <width> <height>
#   sh make-photos.sh 10 800 600   # creates 10 images, 800x600 each

count=${1:-10}
width=${2:-800}
height=${3:-600}

echo "Creating ${count} .jpg photos with size ${width}x${height} pixels"

python3 /home/ng/workspace/novagenesis/Scripts/Python/BuildPhotos.py different "${count}" "${width}" "${height}"

echo "Done. Created ${count} images."
