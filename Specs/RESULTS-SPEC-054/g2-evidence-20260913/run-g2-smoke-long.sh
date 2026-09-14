#!/usr/bin/env bash
set -u -o pipefail

cd /home/gandalf/workspace/novagenesis
set -a
. Scripts/AlpineVMs/ng-vm.env
set +a

E="Specs/RESULTS-SPEC-054/g2-evidence-20260913/smoke-long"
mkdir -p "$E"

has_markers() {
    python3 -c 'import sys; from pathlib import Path; p=Path(sys.argv[1]); t=p.read_text(errors="replace") if p.exists() else ""; raise SystemExit(0 if all(x in t for x in sys.argv[2:]) else 1)' "$@"
}

has_any_marker() {
    python3 -c 'import sys; from pathlib import Path; p=Path(sys.argv[1]); t=p.read_text(errors="replace") if p.exists() else ""; raise SystemExit(0 if any(x in t for x in sys.argv[2:]) else 1)' "$@"
}

PIDS=""
PHASE="START"

printf '%s\n' 'phase=PGCS-Source'
timeout --signal=TERM --kill-after=10s 420 bash Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh >"$E/01-pgcs-source.log" 2>&1 & p1=$!
PIDS="$PIDS $p1"
for _ in $(seq 1 90); do
    if has_markers "$E/01-pgcs-source.log" 'Created the client socket with CSID' 'Created the server socket with SSID'; then PHASE="PGCS_SOURCE_READY"; break; fi
    sleep 1
done

if [ "$PHASE" = "PGCS_SOURCE_READY" ]; then
    printf '%s\n' 'phase=PGCS-Repository'
    timeout --signal=TERM --kill-after=10s 420 bash Scripts/AlpineVMs/run_PGCS_on_Repo_VM.sh >"$E/02-pgcs-repository.log" 2>&1 & p2=$!
    PIDS="$PIDS $p2"
    for _ in $(seq 1 90); do
        if has_markers "$E/02-pgcs-repository.log" 'Created the client socket with CSID' 'Created the server socket with SSID'; then PHASE="PGCS_BOTH_READY"; break; fi
        sleep 1
    done
fi

if [ "$PHASE" = "PGCS_BOTH_READY" ]; then
    printf '%s\n' 'phase=NRNCS'
    timeout --signal=TERM --kill-after=10s 420 bash Scripts/AlpineVMs/run_NRNCS_on_Source_VM.sh >"$E/03-nrncs.log" 2>&1 & p3=$!
    PIDS="$PIDS $p3"
    for _ in $(seq 1 120); do
        if has_markers "$E/03-nrncs.log" 'OPERATIONAL: Everything ok!'; then PHASE="NRNCS_READY"; break; fi
        sleep 1
    done
fi

if [ "$PHASE" = "NRNCS_READY" ]; then
    printf '%s\n' 'phase=Repository'
    timeout --signal=TERM --kill-after=10s 420 bash Scripts/AlpineVMs/run_Repository_on_Repo_VM.sh >"$E/04-repository.log" 2>&1 & p4=$!
    PIDS="$PIDS $p4"
    for _ in $(seq 1 90); do
        if has_any_marker "$E/04-repository.log" 'State: Operational' 'Opening SSH terminal'; then PHASE="REPOSITORY_STARTED"; break; fi
        sleep 1
    done
fi

if [ "$PHASE" = "REPOSITORY_STARTED" ]; then
    printf '%s\n' 'phase=Source'
    timeout --signal=TERM --kill-after=10s 420 bash Scripts/AlpineVMs/run_Source_on_Source_VM.sh 1 100 100 >"$E/05-source.log" 2>&1 & p5=$!
    PIDS="$PIDS $p5"
    for _ in $(seq 1 120); do
        if has_markers "$E/05-source.log" 'Photos sealed and verified.'; then PHASE="SOURCE_STARTED"; break; fi
        sleep 1
    done
fi

if [ "$PHASE" = "SOURCE_STARTED" ]; then
    printf '%s\n' 'phase=wait-repository-photo'
    for _ in $(seq 1 180); do
        if has_markers "$E/04-repository.log" 'ContentApp received payload: file=00000-alpine-ng-source.jpg'; then PHASE="PHOTO_RECEIVED"; break; fi
        sleep 1
    done
fi

printf 'phase=%s\n' "$PHASE"
printf '%s\n' 'phase=wait-timeouts'
for p in $PIDS; do wait "$p" 2>/dev/null || true; done
printf '%s\n' 'phase=complete'
for f in "$E"/*.log; do [ -f "$f" ] || continue; printf '%s|bytes=%s\n' "$f" "$(stat -c %s "$f")"; done
