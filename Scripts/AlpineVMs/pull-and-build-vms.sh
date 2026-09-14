#!/bin/bash
# pull-and-build-vms.sh — pinned Git + profile-aware build on Alpine VMs 101 and 102
#
# Usage:
#   NG_BUILD_PROFILE=normal bash pull-and-build-vms.sh
#   NG_BUILD_PROFILE=legacy bash pull-and-build-vms.sh
#
# Normal builds use NRNCS and do not build standalone PSS/GIRS/HTS.
# Legacy builds explicitly enable the deprecated standalone services.
#
# Safety contract: guest Git state is never altered to make a build pass, and
# the build directory is never deleted implicitly. Both guests are moved only
# by a fast-forward to one controller-pinned AIOPT3 commit.

set -euo pipefail

: "${REPO_VM_IP:?Repository VM IP is required; source ng-vm.env}"
: "${SOURCE_VM_IP:?Source VM IP is required; source ng-vm.env}"
: "${NG_REPO_PATH:?NG_REPO_PATH is required; source ng-vm.env}"
: "${NG_BUILD_PATH:?NG_BUILD_PATH is required; source ng-vm.env}"
: "${NG_SSH_KEY:?NG_SSH_KEY is required; source ng-vm.env}"
: "${NG_SSH_USER:?NG_SSH_USER is required; source ng-vm.env}"
: "${NG_SSH_KNOWN_HOSTS:?NG_SSH_KNOWN_HOSTS is required; source ng-vm.env}"
: "${NG_EVIDENCE_PATH:?NG_EVIDENCE_PATH is required; source ng-vm.env}"

require_absolute_path() {
    local variable_name="$1"
    local path_value="$2"

    case "$path_value" in
        /*)
            ;;
        *)
            printf 'ERROR: %s must be an absolute path (got %s)\n' \
                "$variable_name" "$path_value" >&2
            exit 2
            ;;
    esac
}

require_readable_file() {
    local variable_name="$1"
    local path_value="$2"

    if [[ ! -r "$path_value" ]]; then
        printf 'ERROR: %s is not readable: %s\n' "$variable_name" "$path_value" >&2
        exit 2
    fi
}

require_absolute_path NG_REPO_PATH "$NG_REPO_PATH"
require_absolute_path NG_BUILD_PATH "$NG_BUILD_PATH"
require_absolute_path NG_SSH_KEY "$NG_SSH_KEY"
require_absolute_path NG_SSH_KNOWN_HOSTS "$NG_SSH_KNOWN_HOSTS"
require_absolute_path NG_EVIDENCE_PATH "$NG_EVIDENCE_PATH"
require_readable_file NG_SSH_KEY "$NG_SSH_KEY"
require_readable_file NG_SSH_KNOWN_HOSTS "$NG_SSH_KNOWN_HOSTS"

if [[ ! "$NG_SSH_USER" =~ ^[A-Za-z_][A-Za-z0-9_.-]*$ ]]; then
    printf 'ERROR: NG_SSH_USER contains unsafe characters: %s\n' "$NG_SSH_USER" >&2
    exit 2
fi

mkdir -p "$NG_EVIDENCE_PATH"
if [[ ! -d "$NG_EVIDENCE_PATH" || ! -w "$NG_EVIDENCE_PATH" ]]; then
    printf 'ERROR: NG_EVIDENCE_PATH is not a writable directory: %s\n' "$NG_EVIDENCE_PATH" >&2
    exit 2
fi

BASE="$NG_REPO_PATH"
BUILD_PATH="$NG_BUILD_PATH"
BRANCH=AIOPT3
PROFILE=${NG_BUILD_PROFILE:-normal}
BUILD_RUN_ID=${NG_BUILD_RUN_ID:-$(date -u +%Y%m%dT%H%M%SZ)-$$}
if [[ ! "$BUILD_RUN_ID" =~ ^[A-Za-z0-9._-]+$ ]]; then
    printf 'ERROR: NG_BUILD_RUN_ID contains unsafe characters: %s\n' "$BUILD_RUN_ID" >&2
    exit 2
fi
EVIDENCE_DIR="${NG_EVIDENCE_PATH%/}/build-${BUILD_RUN_ID}"
mkdir -p "$EVIDENCE_DIR"

case "$PROFILE" in
    normal)
        LEGACY_FLAG=OFF
        EXPECTED_BINARIES=(PGCS ContentApp NRNCS NBTestApp IoTTestApp)
        ;;
    legacy)
        LEGACY_FLAG=ON
        EXPECTED_BINARIES=(PGCS ContentApp NRNCS NBTestApp IoTTestApp PSS GIRS HTS)
        ;;
    *)
        printf "ERROR: NG_BUILD_PROFILE must be normal or legacy (got '%s')\n" "$PROFILE" >&2
        exit 2
        ;;
esac

VM101="${NG_SSH_USER}@${REPO_VM_IP}"
VM102="${NG_SSH_USER}@${SOURCE_VM_IP}"
SSH_OPTIONS=(
    -T
    -i "$NG_SSH_KEY"
    -o BatchMode=yes
    -o IdentitiesOnly=yes
    -o StrictHostKeyChecking=yes
    -o "UserKnownHostsFile=$NG_SSH_KNOWN_HOSTS"
    -o ConnectTimeout=10
    -o ConnectionAttempts=1
    -o ServerAliveInterval=5
    -o ServerAliveCountMax=3
    -o ForwardAgent=no
    -o ClearAllForwardings=yes
)

shell_quote() {
    local value="$1"
    value=${value//\'/\'\\\'\'}
    printf "'%s'" "$value"
}

printf '%s\n' \
    "=== Pull + Build Script for Alpine VMs ===" \
    "Profile: $PROFILE" \
    "Evidence: $EVIDENCE_DIR" \
    ""

for VM in "$VM101" "$VM102"; do
    HOST=${VM#*@}
    printf 'Checking %s ... ' "$HOST"
    if ssh "${SSH_OPTIONS[@]}" "$VM" /bin/true >/dev/null 2>&1; then
        echo "REACHABLE"
    else
        echo "UNREACHABLE — aborting"
        exit 1
    fi
done

# Resolve one immutable commit from the repository guest before either build.
REMOTE_PIN_SCRIPT='set -eu
BASE=$1
BRANCH=$2
cd "$BASE"
CURRENT_BRANCH=$(git symbolic-ref --quiet --short HEAD)
test "$CURRENT_BRANCH" = "$BRANCH"
GIT_STATUS=$(git status --porcelain --untracked-files=all)
test -z "$GIT_STATUS"
git ls-remote --exit-code origin "refs/heads/$BRANCH" | awk "NR == 1 { print \$1; exit }"
'
EXPECTED_COMMIT="$(ssh "${SSH_OPTIONS[@]}" "$VM101" /bin/sh -s -- \
    "$(shell_quote "$BASE")" "$(shell_quote "$BRANCH")" \
    <<<"$REMOTE_PIN_SCRIPT")"
EXPECTED_COMMIT="${EXPECTED_COMMIT//$'\r'/}"
if [[ ! "$EXPECTED_COMMIT" =~ ^[0-9a-f]{40}$ ]]; then
    printf 'ERROR: origin/%s did not yield one valid commit SHA: %s\n' "$BRANCH" "$EXPECTED_COMMIT" >&2
    exit 1
fi
printf 'Pinned source commit: %s\n' "$EXPECTED_COMMIT"

REMOTE_BUILD_SCRIPT='set -eu
BASE=$1
BUILD_PATH=$2
BRANCH=$3
EXPECTED_COMMIT=$4
PROFILE=$5
LEGACY_FLAG=$6
shift 6

cd "$BASE"
printf "%s\\n" "--- Git status ---"
CURRENT_BRANCH=$(git symbolic-ref --quiet --short HEAD)
printf "branch: %s\\n" "$CURRENT_BRANCH"
test "$CURRENT_BRANCH" = "$BRANCH"
GIT_STATUS=$(git status --porcelain --untracked-files=all)
test -z "$GIT_STATUS"
printf "%s\\n" "--- Fetching pinned branch ---"
git fetch --prune origin "$BRANCH"
git cat-file -e "$EXPECTED_COMMIT^{commit}"
if [ "$(git rev-parse HEAD)" != "$EXPECTED_COMMIT" ]; then
    git merge --ff-only "$EXPECTED_COMMIT"
fi
test "$(git rev-parse HEAD)" = "$EXPECTED_COMMIT"
COMMIT=$(git rev-parse HEAD)
printf "source commit: %s\\n" "$COMMIT"
printf "%s\\n" "--- Configuring $PROFILE build ---"
cmake -S "$BASE" -B "$BUILD_PATH" "-DNG_ENABLE_LEGACY_STANDALONE=$LEGACY_FLAG"
printf "%s\\n" "--- Building ---"
JOBS=$(nproc)
cmake --build "$BUILD_PATH" -j "$JOBS"
printf "%s\\n" "--- Verifying expected binaries ---"
for binary in "$@"; do
    test -x "$BUILD_PATH/$binary"
    printf "present: %s\\n" "$binary"
done
if [ "$PROFILE" = "normal" ]; then
    for binary in PSS GIRS HTS; do
        test ! -e "$BUILD_PATH/$binary"
        printf "absent: %s\\n" "$binary"
    done
fi
RECEIPT="$BUILD_PATH/.ng-build-receipt"
TMP_RECEIPT="$RECEIPT.tmp.$$"
{
    printf "branch=%s\\n" "$BRANCH"
    printf "commit=%s\\n" "$COMMIT"
    printf "profile=%s\\n" "$PROFILE"
    printf "build_path=%s\\n" "$BUILD_PATH"
    printf "repo_path=%s\\n" "$BASE"
} > "$TMP_RECEIPT"
mv -f "$TMP_RECEIPT" "$RECEIPT"
printf "build receipt: %s\\n" "$RECEIPT"
printf "%s\\n" "--- Build complete ---"
'

build_vm() {
    local vm="$1"
    local log_path="$2"
    local binary
    local remote_args=(
        "$(shell_quote "$BASE")"
        "$(shell_quote "$BUILD_PATH")"
        "$(shell_quote "$BRANCH")"
        "$(shell_quote "$EXPECTED_COMMIT")"
        "$(shell_quote "$PROFILE")"
        "$(shell_quote "$LEGACY_FLAG")"
    )

    for binary in "${EXPECTED_BINARIES[@]}"; do
        remote_args+=("$(shell_quote "$binary")")
    done

    ssh "${SSH_OPTIONS[@]}" "$vm" /bin/sh -s -- "${remote_args[@]}" \
        <<<"$REMOTE_BUILD_SCRIPT" >"$log_path" 2>&1
}

LOG101="$EVIDENCE_DIR/repository-build.log"
LOG102="$EVIDENCE_DIR/source-build.log"
printf 'branch=%s\nprofile=%s\npinned_commit=%s\nrepository_guest=%s\nsource_guest=%s\n' \
    "$BRANCH" "$PROFILE" "$EXPECTED_COMMIT" "$VM101" "$VM102" \
    > "$EVIDENCE_DIR/build-metadata.txt"

cancel_builds() {
    if [[ -n "${PID101:-}" && "${STATUS101:-RUNNING}" = RUNNING ]] && kill -0 "$PID101" 2>/dev/null; then
        kill "$PID101" 2>/dev/null || true
    fi
    if [[ -n "${PID102:-}" && "${STATUS102:-RUNNING}" = RUNNING ]] && kill -0 "$PID102" 2>/dev/null; then
        kill "$PID102" 2>/dev/null || true
    fi
}

on_exit() {
    local rc=$?
    cancel_builds
    trap - EXIT
    return "$rc"
}
trap on_exit EXIT

printf '%s\n' "=== Repository guest (${REPO_VM_IP}) ==="
build_vm "$VM101" "$LOG101" &
PID101=$!
printf '%s\n' "=== Source guest (${SOURCE_VM_IP}) ==="
build_vm "$VM102" "$LOG102" &
PID102=$!

STATUS101=RUNNING
STATUS102=RUNNING
REMAINING=2
FAILED=0
while (( REMAINING > 0 )); do
    if wait -n -p FINISHED_PID; then
        CHILD_STATUS=0
    else
        CHILD_STATUS=$?
    fi
    if [[ "$FINISHED_PID" = "$PID101" ]]; then
        STATUS101=$CHILD_STATUS
    elif [[ "$FINISHED_PID" = "$PID102" ]]; then
        STATUS102=$CHILD_STATUS
    else
        printf 'ERROR: wait returned unknown build PID: %s\n' "$FINISHED_PID" >&2
        FAILED=1
    fi
    REMAINING=$((REMAINING - 1))
    if (( CHILD_STATUS != 0 )); then
        FAILED=1
        cancel_builds
    fi
done

[[ "$STATUS101" = 0 ]] || FAILED=1
[[ "$STATUS102" = 0 ]] || FAILED=1

printf '\n=== Results ===\n'
printf 'Repository guest (%s): %s (exit %s)\n' "$REPO_VM_IP" \
    "$([[ "$STATUS101" = 0 ]] && echo OK || echo FAILED)" "$STATUS101"
printf 'Source guest (%s):     %s (exit %s)\n' "$SOURCE_VM_IP" \
    "$([[ "$STATUS102" = 0 ]] && echo OK || echo FAILED)" "$STATUS102"
printf 'Repository log: %s\nSource log: %s\n' "$LOG101" "$LOG102"

if (( FAILED == 0 )); then
    echo "=== BUILD SUCCESS on both VMs ==="
else
    echo "=== BUILD FAILED on one or more VMs ===" >&2
    exit 1
fi
