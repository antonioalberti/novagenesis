# SPEC-047 — Resultado do trial PGCS-only multi-VM

Date: 2026-09-12
Branch/commit: AIOPT3 / `1af604d`
Guests: VM101 repo61 `192.168.0.61`; VM102 source36 `192.168.0.36`
Window: 180 s; both supervisors returned timeout `124` after graceful SIGTERM.

## Provenance

- Both guests were reset to `1af604d` and independently rebuilt with `Make/compile.sh PGCS NRNCS ContentApp`.
- PGCS/NRNCS/ContentApp hashes matched across guests.
- `PGCS.ini` hash matched across guests.
- Both preflights passed: commit, executable, configuration and writable `/tmp` log destination.
- Peer MACs were passed literally and observed in the startup logs.

## Gate results

| Gate | Result | Evidence |
|---|---|---|
| G0 identity/config | PASS | Preflight and trial contract |
| G1 process | PASS | Both PGCS remained alive for 180 s; supervisors returned bounded timeout 124 |
| G2 socket creation | PASS | Both client/server sockets created; correct local/peer MACs in logs |
| G3 SHM/GW | PASS limited | Input SHM keys 11–14 created; no crash/fatal marker; generic GW operational marker present |
| G4 periodic activation | PASS limited | Bidirectional hello frames began after the configured first-periodic window and repeated at regular intervals; high-level periodic markers were not enabled in this build |
| G5 hello emission | PASS | 36 captured EtherType `0x1234` frames containing `ng -hello --ihc 0.1`; 18 in each direction |
| G6 wire transport | PASS | VM101↔VM102 frames observed on `vmbr0`; 18 source→repo and 18 repo→source; destination MACs correct |
| receiver application handling | OPEN | Packet capture proves ingress to bridge, not PGCS receive/dispatch/application state; normal logs lack receive markers |
| teardown | PASS | Both processes terminated; `clean.sh` removed SHM IDs 0–3 on each guest; final SHM/semaphore inventories empty |

## Interpretation

This trial resolves the previous ambiguity at the lower network edge: the PGCS pair reaches socket setup, emits bilateral hellos, and transmits them over the intended EtherType with correct MAC direction. The prior absence of high-level log markers was observability/buffering, not evidence that periodic activation failed.

It does not yet prove receiver-side parsing, peer binding storage, discovery convergence, NRNCS readiness, subscriptions, payload delivery, or SPEC-044 functional correctness.

The first remaining unproven edge is **receiver-side PGCS processing after raw-frame ingress**. No production change is authorized by this result. Further debug instrumentation, if needed, requires an amended/approved SPEC and another Astra review.

Evidence:

- `trial-contract.txt`
- `repo-preflight.txt`, `source-preflight.txt`
- `repo-supervisor.txt`, `source-supervisor.txt`
- `remote-repo-pgcs.log`, `remote-source-pgcs.log`
- `ng047-tcpdump.txt`
- `repo-cleanup.txt`, `source-cleanup.txt`
