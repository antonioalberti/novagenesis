# SPEC-059 r16 diagnostic — setup rejection before role launch

Trial: `qga-r16-20260918`
Execution channel: `proxmox-qga`
VM: `100`
Candidate build manifest: source HEAD `3eb46c78ff89604687147c2be23244c3140396ff`
Controller source HEAD observed during trial: later documentation HEAD `81acedfa68cb7989b78edb67515ffafa9b55aa85`

## Result

- QGA transport: `0`
- Guest controller exit: `20`
- `effective_uid`: `0`
- `local_profile`: `native-privileged`, explicit in plan and CLI
- `runtime_result`: `INCONCLUSIVE`
- `teardown_result`: `UNKNOWN` in terminal result because final evidence publication was incomplete
- `evidence_result`: `INCOMPLETE`
- `local_acceptance_eligible`: `false`
- roles launched: none
- `build_linkage_reason`: `source head divergence`
- additional publication blocker: `artifacts/repository/` was not covered after preflight rejected before role launch

The controller recorded cleanup `PASS`, preservation `verified=true`, and final inventory with zero processes, zero new SysV IPC objects and zero POSIX semaphore objects. Independent host inspection after QGA completion also found no NG processes, no SysV IPC objects and no `/dev/shm/sem.*` entries.

This is a diagnostic setup/provenance rejection, not a NovaGenesis runtime result and not evidence of payload or teardown behaviour. No second trial is authorized by this record. The next candidate must be rebuilt from the current clean HEAD, with the manifest and launched source tree bound to the same immutable SHA, then reviewed before any new trial.

Raw QGA records are adjacent: `qga-r16-qga-response.json` and `qga-r16-preflight-response.json`.
