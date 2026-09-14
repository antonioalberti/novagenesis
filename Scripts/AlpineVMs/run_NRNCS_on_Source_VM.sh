#!/bin/bash
# run_NRNCS_on_Source_VM.sh — Start NRNCS on the source guest
#
# Opens a dedicated SSH terminal to the Source Alpine VM and starts NRNCS.
# Must start AFTER both PGCS processes are running and have discovered
# each other via raw socket.
#
# Usage: bash run_NRNCS_on_Source_VM.sh
# Prerequisites:
#   - PGCS running on both Source and Repo VMs
#   - AIOPT3 NRNCS binary compiled at NG_BUILD_PATH
#   - An operator-selected SSH key and verified known-hosts file
#
# Order: run after both PGCS scripts (Terminal 3), wait ~2s

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
BASE="$NG_REPO_PATH"

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
    'set -eu\nBASE=%q\nBUILD_PATH=%q\nEXPECTED_BRANCH=AIOPT3\ncd "$BASE"\nCURRENT_BRANCH=$(git symbolic-ref --quiet --short HEAD)\ntest "$CURRENT_BRANCH" = "$EXPECTED_BRANCH"\nCOMMIT=$(git rev-parse HEAD)\nRECEIPT="$BUILD_PATH/.ng-build-receipt"\ntest -r "$RECEIPT"\nRECEIPT_BRANCH=$(sed -n "s/^branch=//p" "$RECEIPT")\nRECEIPT_COMMIT=$(sed -n "s/^commit=//p" "$RECEIPT")\ntest "$RECEIPT_BRANCH" = "$EXPECTED_BRANCH"\ntest "$RECEIPT_COMMIT" = "$COMMIT"\necho "build provenance verified: branch=$CURRENT_BRANCH commit=$COMMIT"\ncd "$BUILD_PATH"\nexec gdb -batch -return-child-result -ex %q -ex %q -ex %q --args ./NRNCS %q' \
    "$BASE" "$NG_BUILD_PATH" run bt quit "${BASE}/IO/NRNCS/"

echo "=== NRNCS on Source VM (${VM_IP}) ==="
echo "Opening SSH terminal... (Ctrl+C to stop NRNCS)"
echo ""

ssh "${SSH_OPTIONS[@]}" -t -i "$SSH_KEY" "${SSH_USER}@${VM_IP}" "$REMOTE_CMD"
