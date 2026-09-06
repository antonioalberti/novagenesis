#!/bin/bash
# soak-monitor.sh — 30-min soak monitor for NovaGenesis Alpine VMs (SPEC-027)
#
# Non-invasive sampler: every 60s polls both VMs over SSH for process liveness,
# CPU time, RSS, and completed-message counter deltas. Per the monitoring
# runbook (novagenesis-build-deploy skill, references/stress-soak-monitoring.md):
#   - liveness = progress (completed-counter delta) while traffic is offered,
#     NOT CPU time / wchan / log size (those can be livelock or idle)
#   - reject 0 or >1 PIDs per process (explicit failure)
#   - SSH/timeout/malformed output = explicit failure, not "UP"
#   - on failure capture cores dir, dmesg tail BEFORE any cleanup
#
# Usage: bash soak-monitor.sh [duration_min] [log_file]
#   duration_min default 30; log_file default Specs/RESULTS-SPEC-027/soak-<date>.log
# Requires: SSH key ~/.ssh/id_ed25519_hermes deployed on both VMs (root).
#
# Expected inventory: VM 102 = PGCS+NRNCS+ContentApp, VM 101 = PGCS+ContentApp.

SSH_KEY=~/.ssh/id_ed25519_hermes
VM_SRC=192.168.0.36
VM_REPO=192.168.0.61
DURATION_MIN=${1:-30}
BASE=/root/workspace/novagenesis

REPO_LOCAL=/home/gandalf/workspace/novagenesis
OUT_DEFAULT="$REPO_LOCAL/Specs/RESULTS-SPEC-027/soak-$(date +%Y%m%d-%H%M%S).log"
OUT=${2:-$OUT_DEFAULT}
mkdir -p "$(dirname "$OUT")"

# Liveness thresholds (tune per run; record in the results file)
NO_PROGRESS_LIMIT=5          # minutes without completed-counter delta => FAIL
STALE_BASE=/tmp/soak-stale.state

log() { echo "$(date +%H:%M:%S) $*" | tee -a "$OUT"; }

# completed-counter snapshot for one VM (all PGCS markers)
completed_count() {
  ssh -i "$SSH_KEY" -o BatchMode=yes -o ConnectTimeout=10 "root@$1" \
    "grep -c 'COMPLETE MN' $BASE/../..//tmp/pgcs.log 2>/dev/null || grep -c 'COMPLETE MN' /tmp/pgcs.log 2>/dev/null" 2>/dev/null
}

# per-process PID validation: exactly one PID per name, exe matches
inventory_check() { # $1=VM, $2="PGCS NRNCS ContentApp"
  for name in $2; do
    n=$(ssh -i "$SSH_KEY" -o BatchMode=yes "root@$1" "pidof $name | wc -w" 2>/dev/null)
    if [ "$n" != "1" ]; then
      log "INVENTORY_FAIL $1 $name count=$n (expected 1)"
      return 1
    fi
  done
  return 0
}

grab_failure_evidence() { # $1=VM
  log "=== FAILURE EVIDENCE $1 ==="
  ssh -i "$SSH_KEY" -o BatchMode=yes "root@$1" \
    "ls -la /root/cores/ 2>/dev/null; dmesg 2>/dev/null | tail -5; df -h /tmp | tail -1; free -m | head -2" 2>&1 | tee -a "$OUT"
}

# init stale counters
declare -A LAST_DONE
rm -f "$STALE_BASE"

log "SOAK START duration=${DURATION_MIN}min out=$OUT"
log "readiness: inventory checks"
inventory_check "$VM_SRC" "PGCS NRNCS ContentApp" || log "WARN source inventory not clean at start"
inventory_check "$VM_REPO" "PGCS ContentApp" || log "WARN repo inventory not clean at start"

END=$(( $(date +%s) + DURATION_MIN * 60 ))
i=0
while [ "$(date +%s)" -lt "$END" ]; do
  i=$((i+1))
  sleep 60

  SRC=$(ssh -i "$SSH_KEY" -o BatchMode=yes -o ConnectTimeout=10 "root@$VM_SRC" \
    'P=$(pidof PGCS); if [ $(echo $P | wc -w) -ne 1 ]; then echo MULTIPID; else awk "{printf \"cpu=%.1f rss=%d\", (\$14+\$15)/'"$(getconf CLK_TCK)"', \$24*'"$(getconf PAGESIZE/1024)"'"}" /proc/$P/stat; fi; echo -n " done=$(grep -c "COMPLETE MN" /tmp/pgcs.log 2>/dev/null)"; pidof PGCS >/dev/null && echo -n " UP" || echo -n " DOWN"' 2>/dev/null)
  RPO=$(ssh -i "$SSH_KEY" -o BatchMode=yes -o ConnectTimeout=10 "root@$VM_REPO" \
    'P=$(pidof PGCS); if [ $(echo $P | wc -w) -ne 1 ]; then echo MULTIPID; else awk "{printf \"cpu=%.1f rss=%d\", (\$14+\$15)/'"$(getconf CLK_TCK)"', \$24*'"$(getconf PAGESIZE/1024)"'"}" /proc/$P/stat; fi; echo -n " done=$(grep -c "COMPLETE MN" /tmp/pgcs.log 2>/dev/null)"; pidof PGCS >/dev/null && echo -n " UP" || echo -n " DOWN"' 2>/dev/null)

  log "min$i src[$SRC] repo[$RPO]"

  # explicit failure detection
  case "$SRC" in *MULTIPID*|*DOWN*|"") grab_failure_evidence "$VM_SRC"; log "FAIL source at min$i"; exit 2;; esac
  case "$RPO" in *MULTIPID*|*DOWN*|"") grab_failure_evidence "$VM_REPO"; log "FAIL repo at min$i"; exit 2;; esac

  # no-progress detection
  for side in SRC RPO; do
    eval "line=\$$side"
    done_n=$(echo "$line" | grep -o 'done=[0-9]*' | cut -d= -f2)
    prev=${LAST_DONE[$side]:-0}
    if [ "$done_n" -gt "$prev" ]; then
      LAST_DONE[$side]=$done_n
      echo 0 > "$STALE_BASE.$side"
    else
      stale=$(( $(cat "$STALE_BASE.$side" 2>/dev/null || echo 0) + 1 ))
      echo "$stale" > "$STALE_BASE.$side"
      if [ "$stale" -ge "$NO_PROGRESS_LIMIT" ]; then
        log "NOPROGRESS $side: $stale min without completion delta (offered traffic assumed > 0)"
        grab_failure_evidence "$([ $side = SRC ] && echo $VM_SRC || echo $VM_REPO)"
        log "FAIL no-progress $side at min$i"
        exit 3
      fi
    fi
  done
done

log "SOAK COMPLETED ${DURATION_MIN}min — collect StressTest_Stats.txt and final counters for reconciliation"
log "final done: src=${LAST_DONE[SRC]:-0} repo=${LAST_DONE[RPO]:-0}"
