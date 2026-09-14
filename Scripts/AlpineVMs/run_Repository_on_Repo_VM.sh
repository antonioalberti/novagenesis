#!/bin/bash
# run_Repository_on_Repo_VM.sh — Start ContentApp Repository on the repository guest
#
# Opens a dedicated SSH terminal to the Repo Alpine VM and starts ContentApp
# in Repository mode. Must start AFTER both PGCS processes are running.
#
# Usage: bash run_Repository_on_Repo_VM.sh
# Prerequisites:
#   - PGCS on both VMs must already be running
#   - AIOPT3 ContentApp binary compiled at NG_BUILD_PATH
#   - An operator-selected SSH key and verified known-hosts file
#
# Order: run after NRNCS (Terminal 4), before Source (Terminal 5)

set -euo pipefail

: "${REPO_VM_IP:?Repository VM IP is required; source ng-vm.env}"
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
VM_IP="$REPO_VM_IP"
BASE="$NG_REPO_PATH"
IO_DIR="${REPO_IO_DIR:-${BASE}/IO/Repository1}"
require_absolute_path REPO_IO_DIR "$IO_DIR"

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
    'set -eu\nBASE=%q\nBUILD_PATH=%q\nIO_DIR=%q\nEXPECTED_BRANCH=AIOPT3\ncd "$BASE"\nCURRENT_BRANCH=$(git symbolic-ref --quiet --short HEAD)\ntest "$CURRENT_BRANCH" = "$EXPECTED_BRANCH"\nCOMMIT=$(git rev-parse HEAD)\nRECEIPT="$BUILD_PATH/.ng-build-receipt"\ntest -r "$RECEIPT"\nRECEIPT_BRANCH=$(sed -n "s/^branch=//p" "$RECEIPT")\nRECEIPT_COMMIT=$(sed -n "s/^commit=//p" "$RECEIPT")\ntest "$RECEIPT_BRANCH" = "$EXPECTED_BRANCH"\ntest "$RECEIPT_COMMIT" = "$COMMIT"\necho "build provenance verified: branch=$CURRENT_BRANCH commit=$COMMIT"\nmkdir -p "$IO_DIR"\necho %q\ncd "$BUILD_PATH"\nexec gdb -batch -return-child-result -ex %q -ex %q -ex %q -ex %q --args ./ContentApp %q Repository' \
    "$BASE" "$NG_BUILD_PATH" "$IO_DIR" 'IO directory ready.' 'run' 'bt full' 'info registers' 'thread apply all bt full' "${IO_DIR}/"

echo "=== ContentApp Repository on Repo VM (${VM_IP}) ==="
echo "Opening SSH terminal... (Ctrl+C to stop ContentApp)"
echo ""

ssh "${SSH_OPTIONS[@]}" -t -i "$SSH_KEY" "${SSH_USER}@${VM_IP}" "$REMOTE_CMD"
