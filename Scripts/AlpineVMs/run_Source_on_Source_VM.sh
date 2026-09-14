#!/bin/bash
# Start ContentApp Source on the source guest.
# Run LAST, after PGCS/NRNCS and Repository are running.
#
# Usage: bash run_Source_on_Source_VM.sh [num_photos] [width] [height]
# Defaults: 100 photos, 800x600.
# Prerequisites: Python 3.11+, busybox sha256sum, gdb on Source VM.
# Each invocation creates a fresh immutable staging directory. Source1 is
# neither modified nor used. Preserve the printed manifest hash off the VM.

set -euo pipefail

: "${SOURCE_VM_IP:?Source VM IP is required; source ng-vm.env}"
: "${NG_REPO_PATH:?NG_REPO_PATH is required; source ng-vm.env}"
: "${NG_BUILD_PATH:?NG_BUILD_PATH is required; source ng-vm.env}"
: "${NG_SSH_KEY:?NG_SSH_KEY is required; source ng-vm.env}"
: "${NG_SSH_USER:?NG_SSH_USER is required; source ng-vm.env}"
: "${NG_SSH_KNOWN_HOSTS:?NG_SSH_KNOWN_HOSTS is required; source ng-vm.env}"

require_absolute_path() {
    local name="$1"
    local value="$2"
    case "$value" in
        /*) ;;
        *)
            printf '%s must be an absolute path: %s\n' "$name" "$value" >&2
            exit 2
            ;;
    esac
}

require_absolute_path NG_REPO_PATH "$NG_REPO_PATH"
require_absolute_path NG_BUILD_PATH "$NG_BUILD_PATH"
require_absolute_path NG_SSH_KEY "$NG_SSH_KEY"
require_absolute_path NG_SSH_KNOWN_HOSTS "$NG_SSH_KNOWN_HOSTS"

if [[ ! -r "$NG_SSH_KEY" ]]; then
    printf 'NG_SSH_KEY is not readable: %s\n' "$NG_SSH_KEY" >&2
    exit 2
fi
if [[ ! -r "$NG_SSH_KNOWN_HOSTS" ]]; then
    printf 'NG_SSH_KNOWN_HOSTS is not readable: %s\n' "$NG_SSH_KNOWN_HOSTS" >&2
    exit 2
fi

SSH_KEY="$NG_SSH_KEY"
SSH_USER="$NG_SSH_USER"
VM_IP="$SOURCE_VM_IP"
PHOTOS=${1:-100}
WIDTH=${2:-800}
HEIGHT=${3:-600}
BASE="$NG_REPO_PATH"

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

SSH_OPTIONS=(
    -o BatchMode=yes
    -o IdentitiesOnly=yes
    -o StrictHostKeyChecking=yes
    -o "UserKnownHostsFile=${NG_SSH_KNOWN_HOSTS}"
    -o ConnectTimeout=10
    -o ConnectionAttempts=1
    -o ServerAliveInterval=5
    -o ServerAliveCountMax=3
    -o ForwardAgent=no
    -o ClearAllForwardings=yes
)

printf -v REMOTE_CMD \
    'set -eu\nBASE=%q\nBUILD_PATH=%q\nEXPECTED_BRANCH=AIOPT3\ncd "$BASE"\nCURRENT_BRANCH=$(git symbolic-ref --quiet --short HEAD)\ntest "$CURRENT_BRANCH" = "$EXPECTED_BRANCH"\nCOMMIT=$(git rev-parse HEAD)\nRECEIPT="$BUILD_PATH/.ng-build-receipt"\ntest -r "$RECEIPT"\nRECEIPT_BRANCH=$(sed -n "s/^branch=//p" "$RECEIPT")\nRECEIPT_COMMIT=$(sed -n "s/^commit=//p" "$RECEIPT")\ntest "$RECEIPT_BRANCH" = "$EXPECTED_BRANCH"\ntest "$RECEIPT_COMMIT" = "$COMMIT"\necho "build provenance verified: branch=$CURRENT_BRANCH commit=$COMMIT"\nRUN_ID=$(python3 -c %q)\nIO_DIR=%q/$RUN_ID\n\npython3 Scripts/Python/BuildPhotos.py \\\n    --staging "$IO_DIR" different %q %q %q\n\necho "Run-id: $RUN_ID"\necho "Publish directory: $IO_DIR"\necho %q\nsha256sum "$IO_DIR/manifest.sha256"\n\n(cd "$IO_DIR" && sha256sum -c manifest.sha256 >/dev/null)\n\necho %q\ncd "$BUILD_PATH"\nexec gdb -batch -return-child-result \\\n    -ex %q -ex %q -ex %q \\\n    -ex %q --args \\\n    ./ContentApp "$IO_DIR/" Source' \
    "$BASE" \
    "$NG_BUILD_PATH" \
    'import uuid; print(uuid.uuid4().hex)' \
    "${BASE}/IO/SourceStaging" \
    "$PHOTOS" "$WIDTH" "$HEIGHT" \
    'Publish-time manifest SHA-256 (retain with test results):' \
    'Photos sealed and verified. Starting ContentApp Source...' \
    'run' 'bt full' 'info registers' 'thread apply all bt full'

echo "=== ContentApp Source on Source VM (${VM_IP}) ==="
echo "Photos: ${PHOTOS} (${WIDTH}x${HEIGHT})"
echo "Opening SSH terminal... (Ctrl+C to stop ContentApp)"
echo ""

ssh "${SSH_OPTIONS[@]}" -t -i "$SSH_KEY" "${SSH_USER}@${VM_IP}" "$REMOTE_CMD"

# ===== NOTES =====
# - Phase 0: pass the fresh staging path directly as ContentApp’s IO argument; never change
#   `IO/Source1` or a shared symlink.
# - Staging creation is exclusive; failed runs leave unusable staging directories rather than
#   reusing or overwriting inputs.
# - Manifest follows all image closes/fsyncs; files, manifest and directory lose write bits before
#   the launch-time disk verification.
# - Busybox-compatible plain `<sha256>  <basename>` lines; manifest excludes itself.
# - Stdlib-only requires replacing NumPy/Pillow: generated JPEGs use random grayscale 8×8 blocks,
#   not the original independent RGB pixels. Hermes must approve this workload change.
# - Hermes must confirm ContentApp accepts the staging IO path without writing there and publishes
#   only JPEGs, not `manifest.sha256`.
# - Root can bypass chmod; retain the printed manifest hash externally and recheck it before delivery
#   verification. Chmod is a tripwire, not a root security boundary.
# - Not run here: Hermes must validate JPEG decoding/dimensions, Busybox checks, 1000/1000 delivery,
#   and exactly one mismatch after corrupting one received file.
