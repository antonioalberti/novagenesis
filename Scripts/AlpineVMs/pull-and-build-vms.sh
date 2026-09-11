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

SSH_KEY=~/.ssh/id_ed25519_hermes
BASE=/root/workspace/novagenesis
BRANCH=AIOPT3
PROFILE=${NG_BUILD_PROFILE:-normal}

VM101="root@192.168.0.61"
VM102="root@192.168.0.36"

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

echo "=== VM 101 (Repository — 192.168.0.61) ==="
run_build "$VM101" > /tmp/ng-build-vm101.log 2>&1 &
PID101=$!

echo "=== VM 102 (Source — 192.168.0.36) ==="
run_build "$VM102"
STATUS102=$?

wait "$PID101"
STATUS101=$?

printf '\n=== Results ===\n'
printf 'VM 101 (Repository @ 192.168.0.61): %s\n' "$([ "$STATUS101" -eq 0 ] && echo OK || echo FAILED)"
printf 'VM 102 (Source @ 192.168.0.36):     %s\n' "$([ "$STATUS102" -eq 0 ] && echo OK || echo FAILED)"

if [ "$STATUS101" -eq 0 ] && [ "$STATUS102" -eq 0 ]; then
    echo "=== BUILD SUCCESS on both VMs ==="
else
    echo "=== BUILD FAILED on one or more VMs ===" >&2
    echo "VM 101 log: /tmp/ng-build-vm101.log" >&2
    exit 1
fi
