#!/bin/bash
# Start ContentApp Source on Source VM (192.168.0.36).
# Run LAST, after PGCS/NRNCS and Repository are running.
#
# Usage: bash run_Source_on_Source_VM.sh [num_photos] [width] [height]
# Defaults: 100 photos, 800x600.
# Prerequisites: Python 3.11+, busybox sha256sum, gdb on Source VM.
# Each invocation creates a fresh immutable staging directory. Source1 is
# neither modified nor used. Preserve the printed manifest hash off the VM.

set -euo pipefail

SSH_KEY=~/.ssh/id_ed25519_hermes
VM_IP=192.168.0.36
PHOTOS=${1:-100}
WIDTH=${2:-800}
HEIGHT=${3:-600}
BASE=/root/workspace/novagenesis

if (( $# > 3 )); then
    echo "Usage: $0 [num_photos] [width] [height]" >&2
    exit 2
fi
for value in "$PHOTOS" "$WIDTH" "$HEIGHT"; do
    if [[ ! "$value" =~ ^[1-9][0-9]*$ ]]; then
        echo "Photo count and dimensions must be positive decimal integers." >&2
        exit 2
    fi
done

echo "=== ContentApp Source on Source VM (${VM_IP}) ==="
echo "Photos: ${PHOTOS} (${WIDTH}x${HEIGHT})"
echo "Opening SSH terminal... (Ctrl+C to stop ContentApp)"
echo ""

ssh -t -i "${SSH_KEY}" "root@${VM_IP}" "
    set -eu
    cd '${BASE}'
    RUN_ID=\$(python3 -c 'import uuid; print(uuid.uuid4().hex)')
    IO_DIR='${BASE}/IO/SourceStaging/'\"\$RUN_ID\"

    python3 Scripts/Python/BuildPhotos.py \
        --staging \"\$IO_DIR\" different '${PHOTOS}' '${WIDTH}' '${HEIGHT}'

    echo \"Run-id: \$RUN_ID\"
    echo \"Publish directory: \$IO_DIR\"
    echo 'Publish-time manifest SHA-256 (retain with test results):'
    sha256sum \"\$IO_DIR/manifest.sha256\"

    # Check persisted bytes, not just the generator's in-memory hashes.
    # Busybox sha256sum supports these plain two-space manifest lines.
    (cd \"\$IO_DIR\" && sha256sum -c manifest.sha256 >/dev/null)

    echo 'Photos sealed and verified. Starting ContentApp Source...'
    cd '${BASE}/cmake-build-debug'
    exec gdb -batch \
        -ex 'run' -ex 'bt full' -ex 'info registers' \
        -ex 'thread apply all bt full' -ex 'quit' --args \
        ./ContentApp \"\$IO_DIR/\" Source
"
===== NOTES =====
- Phase 0: pass the fresh staging path directly as ContentApp’s IO argument; never change `IO/Source1` or a shared symlink.
- Staging creation is exclusive; failed runs leave unusable staging directories rather than reusing or overwriting inputs.
- Manifest follows all image closes/fsyncs; files, manifest and directory lose write bits before the launch-time disk verification.
- Busybox-compatible plain `<sha256>  <basename>` lines; manifest excludes itself.
- Stdlib-only requires replacing NumPy/Pillow: generated JPEGs use random grayscale 8×8 blocks, not the original independent RGB pixels. Hermes must approve this workload change.
- Hermes must confirm ContentApp accepts the staging IO path without writing there and publishes only JPEGs, not `manifest.sha256`.
- Root can bypass chmod; retain the printed manifest hash externally and recheck it before delivery verification. Chmod is a tripwire, not a root security boundary.
- Not run here: Hermes must validate JPEG decoding/dimensions, Busybox checks, 1000/1000 delivery, and exactly one mismatch after corrupting one received file.
