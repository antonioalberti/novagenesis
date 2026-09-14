#!/bin/bash
# run_PGCS_on_Repo_VM.sh — Start PGCS on the repository guest
#
# Opens a dedicated SSH terminal to the Repo Alpine VM and starts PGCS
# in deterministic mode (-p) targeting the Source VM's MAC address for
# inter-VM raw socket discovery over the ProxMox bridge.
#
# Usage: bash run_PGCS_on_Repo_VM.sh
# Prerequisites:
#   - The repository guest must be running
#   - AIOPT3 PGCS binary compiled at NG_BUILD_PATH
#   - An operator-selected SSH key and verified known-hosts file
#
# Order: run after run_PGCS_on_Source_VM.sh (Terminal 2), wait ~2s

set -euo pipefail

: "${REPO_VM_IP:?Repository VM IP is required; source ng-vm.env}"
: "${SOURCE_VM_MAC:?Source VM MAC is required; source ng-vm.env}"
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

if [[ ! "$SOURCE_VM_MAC" =~ ^[0-9a-f]{2}(:[0-9a-f]{2}){5}$ ]]; then
    printf 'SOURCE_VM_MAC must be a lowercase colon-separated MAC address: %s\n' "$SOURCE_VM_MAC" >&2
    exit 2
fi

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
PEER_MAC="$SOURCE_VM_MAC"
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
    'set -eu\nBASE=%q\nBUILD_PATH=%q\nEXPECTED_BRANCH=AIOPT3\ncd "$BASE"\nCURRENT_BRANCH=$(git symbolic-ref --quiet --short HEAD)\ntest "$CURRENT_BRANCH" = "$EXPECTED_BRANCH"\nCOMMIT=$(git rev-parse HEAD)\nRECEIPT="$BUILD_PATH/.ng-build-receipt"\ntest -r "$RECEIPT"\nRECEIPT_BRANCH=$(sed -n "s/^branch=//p" "$RECEIPT")\nRECEIPT_COMMIT=$(sed -n "s/^commit=//p" "$RECEIPT")\ntest "$RECEIPT_BRANCH" = "$EXPECTED_BRANCH"\ntest "$RECEIPT_COMMIT" = "$COMMIT"\necho "build provenance verified: branch=$CURRENT_BRANCH commit=$COMMIT"\ncd "$BUILD_PATH"\nexec gdb -batch -return-child-result -ex %q -ex %q -ex %q --args ./PGCS %q 0 Intra_Domain -p Ethernet Intra_Domain eth0 %q 1200' \
    "$BASE" "$NG_BUILD_PATH" run bt quit "${BASE}/IO/PGCS/" "$PEER_MAC"

echo "=== PGCS on Repo VM (${VM_IP}) ==="
echo "Interface: eth0 | Peer MAC: ${PEER_MAC}"
echo "Opening SSH terminal... (Ctrl+C to stop PGCS)"
echo ""

# Cleanup is a separate, explicitly authorized phase; this launcher only starts PGCS.
ssh "${SSH_OPTIONS[@]}" -t -i "$SSH_KEY" "${SSH_USER}@${VM_IP}" "$REMOTE_CMD"
