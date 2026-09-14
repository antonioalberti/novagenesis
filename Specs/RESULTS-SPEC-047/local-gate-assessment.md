# SPEC-047 — Resultado do trial local PGCS -lc

Date: 2026-09-12
Branch: AIOPT3
HEAD: 1af604dad66148618cc0e578c28adb4a9829e607
Command: `cmake-build-debug/PGCS /home/gandalf/workspace/novagenesis/IO/PGCS/ 0 Intra_Domain -lc`
Bound: 160 s
Start: 2026-09-12T10:48:04Z
End: 2026-09-12T10:50:44Z
Wrapper exit: 124 (expected bounded timeout; no crash marker)

## Evidence

- `local-preflight.txt`: host, identity, HEAD/remote, dirty state, binary/config provenance and initial inventory.
- `local-dirty.patch`: dirty tracked diff preserved before cleanup.
- `local-untracked.txt`: untracked paths preserved before cleanup.
- `local-pgcs.log`: complete bounded PGCS output.
- `local-pgcs-result.txt`: timestamps and timeout classification.
- `local-clean-before.log`, `local-clean-after.log`: cleanup records.
- `local-final-inventory.txt`: initial final inventory record; orphan SHM IDs 52–55 were subsequently removed explicitly and rechecked with `ipcs`.

## Gate classification

| Gate | Result | Evidence / limitation |
|---|---|---|
| G0 identity/config | PASS | HEAD, dirty state, binary/config hashes and exact command preserved |
| G1 process | PASS | PGCS remained alive for the full 160 s bound; exited by timeout 124 |
| G2 sockets | NOT APPLICABLE | `-lc` explicitly runs without network |
| G3 GW/SHM | PASS | GW operational; four SHM segments created; periodic processing continued |
| G4 periodic | PASS | first Core periodic at Time 15785.4 after configured 120 s; subsequent cycles at 5 s; scheduled-message markers present |
| G5 hello | NOT APPLICABLE | local `-lc` has no peer/network path; no hello claim made |
| G6 transport/reception | NOT APPLICABLE | local `-lc` has no network path |
| cleanup processes | PASS | no NovaGenesis processes remained |
| cleanup semaphores | PASS | no System V or named semaphores remained |
| cleanup SHM | PASS after explicit removal | clean script left IDs 52–55 unattached; IDs were removed with owner-level `ipcrm` and final `ipcs` was empty |

## Observed behavior

The local process reached the Core periodic loop and repeatedly attempted discovery, producing:

- `Waiting for NRNCS discovery`;
- `Sending a discovery message to the domain's NRNCS`;
- `Warning: Unable to send the discovery message. The domain NRNCS is still unknown`.

This is expected for a PGCS-only `-lc` baseline and does not validate NRNCS discovery, hello emission, RAW transport or subscriptions.

## Verdict

The local baseline removes H1 (PG/GW/SHM did not progress) and H2 (periodic scheduling did not occur) for this build and mode. It confirms that the previous multi-VM logs were insufficient to infer a RAW failure. The next trial must add the smallest network-capable PGCS configuration, with socket and hello gates, before adding NRNCS/ContentApp/NBTestApp.

No production code was changed by this trial. SPEC-047 remains open pending Astra review and a network-capable local trial.
