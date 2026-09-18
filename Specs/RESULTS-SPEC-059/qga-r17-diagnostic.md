# SPEC-059 r17 diagnostic — QGA loss during observation/teardown

Trial: `qga-r17-20260918`
Execution channel: Proxmox/QEMU Guest Agent
Candidate: `68f0678694ed6a79fdb9b5818d211932c9e88e96`

## Verified sequence

- QGA preflight: transport exit 0, guest exit 0, UID 0, explicit `native-privileged`.
- Build provenance: `build_linkage=true`, `capture_complete=true`, raw manifest preserved byte-identically.
- Native cleanup: PASS with zero baseline; repository-owned `Scripts/Simple/clean.sh`, script identity stable.
- Four roles launched with sealed memfd executables and EUID 0.
- Readiness observed for PGCS, NRNCS, Repository and Source.
- Workload oracle observed five expected JPEGs in Source, exact expected hashes.
- Repository remained empty throughout the bounded observation window.
- QGA transport ended with `transport_exit_code=29`, no guest exitcode, stderr `Agent error: PID ld does not exist`.
- Controller stopped Source and Repository; NRNCS stop reached `identity-mismatch` and the controller did not produce a final sealed result before transport loss.
- Host inspection after loss: no NG processes, 16 SysV shared-memory segments and 16 POSIX semaphores remained.
- Authorized QGA recovery executed `Scripts/Simple/clean.sh` with exit 0; all 16 SHM and 16 POSIX semaphores were removed.
- Final independent inventory: zero NG processes, zero SysV IPC objects, zero `/dev/shm/sem.*`.

## Verdict

`runtime_result`: INCONCLUSIVE.
`teardown_result`: incomplete/unknown due QGA loss and NRNCS identity mismatch.
`evidence_result`: incomplete; no final controller seal.
`local_acceptance_eligible`: not established.
Repository-empty is a negative delivery observation only; it does not localize the Source→Repository fault.

No additional NG trial is authorized by this record. The next action is Astra review of the complete r17 evidence, with teardown/process identity and QGA-loss reconciliation kept ahead of payload diagnosis.

Raw records: `qga-r17-qga-response.json`, `qga-r17-recovery-response.json`.
