#!/bin/bash
# pull-and-build-vms.sh — Git pull + profile-aware build on Alpine VMs 101 and 102
#
# Usage:
#   NG_BUILD_PROFILE=normal bash pull-and-build-vms.sh
#   NG_BUILD_PROFILE=legacy bash pull-and-build-vms.sh
#
# Normal builds use NRNCS and do not build standalone PSS/GIRS/HTS.
# Legacy builds explicitly enable the deprecated standalone services.

set -u

: "${REPO_VM_IP:?Repository VM IP is required; source ng-vm.env}"
: "${SOURCE_VM_IP:?Source VM IP is required; source ng-vm.env}"
: "${NG_REPO_PATH:?NG_REPO_PATH is required; source ng-vm.env}"
SSH_KEY=${NG_SSH_KEY:-$HOME/.ssh/id_ed25519}
SSH_USER=${NG_SSH_USER:-root}
BASE="$NG_REPO_PATH"
BRANCH=AIOPT3
PROFILE=${NG_BUILD_PROFILE:-normal}

VM101="${SSH_USER}@${REPO_VM_IP}"
VM102="${SSH_USER}@${SOURCE_VM_IP}"

case "$PROFILE" in
    normal)
        LEGACY_FLAG=OFF
        EXPECTED_BINARIES="PGCS ContentApp NRNCS NBTestApp IoTTestApp"
        ;;
    legacy)
        LEGACY_FLAG=ON
        EXPECTED_BINARIES="PGCS ContentApp NRNCS NBTestApp IoTTestApp PSS GIRS HTS"
        ;;
    *)
        echo "ERROR: NG_BUILD_PROFILE must be normal or legacy (got '$PROFILE')" >&2
        exit 2
        ;;
esac

printf '%s\n' "=== Pull + Build Script for Alpine VMs ===" "Profile: $PROFILE" ""

# Check VM reachability
for VM in "$VM101" "$VM102"; do
    HOST=${VM#*@}
    printf 'Checking %s ... ' "$HOST"
    if ssh -o ConnectTimeout=3 -o StrictHostKeyChecking=no -i "$SSH_KEY" "$VM" "echo ok" >/dev/null 2>&1; then
        echo "REACHABLE"
    else
        echo "UNREACHABLE — aborting"
        exit 1
    fi
done

echo ""

# The command is POSIX-sh compatible for Alpine's default shell.
BUILD_CMD="
set -eu
cd ${BASE}
printf '%s\\n' '--- Git status ---'
git rev-parse --abbrev-ref HEAD
git stash
printf '%s\\n' '--- Pulling ${BRANCH} ---'
git pull origin ${BRANCH}
printf '%s\\n' '--- Configuring ${PROFILE} build ---'
rm -rf ${BASE}/cmake-build-debug
cmake -S ${BASE} -B ${BASE}/cmake-build-debug -DNG_ENABLE_LEGACY_STANDALONE=${LEGACY_FLAG}
printf '%s\\n' '--- Building ---'
cmake --build ${BASE}/cmake-build-debug -j\$(nproc)
printf '%s\\n' '--- Verifying expected binaries ---'
for binary in ${EXPECTED_BINARIES}; do
    test -x ${BASE}/cmake-build-debug/\$binary
    printf 'present: %s\\n' \"\$binary\"
done
if [ '${PROFILE}' = 'normal' ]; then
    for binary in PSS GIRS HTS; do
        test ! -e ${BASE}/cmake-build-debug/\$binary
        printf 'absent: %s\\n' \"\$binary\"
    done
fi
printf '%s\\n' '--- Build complete ---'
"

run_build() {
    local vm="$1"
    ssh -o StrictHostKeyChecking=no -i "$SSH_KEY" "$vm" "$BUILD_CMD"
}

echo "=== Repository guest (${REPO_VM_IP}) ==="
run_build "$VM101" > /tmp/ng-build-vm101.log 2>&1 &
PID101=$!

echo "=== Source guest (${SOURCE_VM_IP}) ==="
run_build "$VM102"
STATUS102=$?

wait "$PID101"
STATUS101=$?

printf '\n=== Results ===\n'
printf 'Repository guest (%s): %s\n' "$REPO_VM_IP" "$([ "$STATUS101" -eq 0 ] && echo OK || echo FAILED)"
printf 'Source guest (%s):     %s\n' "$SOURCE_VM_IP" "$([ "$STATUS102" -eq 0 ] && echo OK || echo FAILED)"

if [ "$STATUS101" -eq 0 ] && [ "$STATUS102" -eq 0 ]; then
    echo "=== BUILD SUCCESS on both VMs ==="
else
    echo "=== BUILD FAILED on one or more VMs ===" >&2
    echo "VM 101 log: /tmp/ng-build-vm101.log" >&2
    exit 1
fi
