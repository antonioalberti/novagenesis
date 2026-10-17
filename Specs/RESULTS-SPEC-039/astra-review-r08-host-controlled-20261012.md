## 1. VERDICT: AMEND

The amendment provides sensible collision checks, restrictive permissions and fail-closed classification, but **does not establish continuous, identity-bound exclusivity on the shared host**. In particular, the production failure-path unlink and the cleanup check-to-unlink race remain unsafe. There is also a conflicting SPEC identity and an incomplete evidence-storage contract.

This assessment covers only the supplied text and projection. No tools, filesystem inspection, hash verification or runtime validation were performed; r07 B1/B2/B3 are not reopened.

## 2. Must-fix blockers

### B1 — Conflicting frozen SPEC identity

**Locations:** Owner request, “FROZEN IDENTITIES”; packet “Frozen identity and privacy”; “Full SPEC SHA-256”; manifest `selected_fields.spec.sha256` and `run_record_host_profile_delta.spec_sha256_binding`.

The owner’s SPEC digest ends `da29722992`; the packet and schema binding use `da29712822992`. These are different identities. The exact-packet review authorisation is explicit, but the claimed underlying SPEC binding is inconsistent.

**Required amendment:** Reconcile the SPEC digest throughout the authorisation metadata, packet and manifest against the intended frozen document. Do not silently choose either value; obtain fresh exact-packet consent for revised bytes.

### B2 — Exclusive creation does not prevent replacement or foreign unlink

**Locations:** SPEC §7.1 reservation and cleanup paragraphs; D2b lifecycle clause; `Common/src/GW.cpp:1300–1322`; manifest `sandbox.cleanup_authority` and `semantic_finalizer_contract.inventory_checks`.

`IPC_EXCL`/`O_EXCL` establish exclusivity **at creation**, not throughout the run. Mode `0600` and umask `0077` do not exclude other processes with the same UID or privileged processes.

Concrete unsafe cases:

- A semaphore can be replaced between exclusive creation and the pathname inspection used to record its inode. A later pathname lookup alone does not prove that inode belongs to the object returned by `sem_open`.
- A matching inode check followed by `sem_unlink(name)` is not an atomic conditional deletion. Replacement between those operations can cause foreign deletion.
- `ReturnIPCSHMID` unconditionally calls `sem_unlink` after a failed `sem_open`. Reservation does not guarantee later opens succeed. A replacement combined with an open failure can therefore reach an unchecked unlink before the finalizer intervenes.
- The ordinary production `O_CREAT` opens can attach to replacements or recreate missing resources without exclusive receipts.

SHM deletion by recorded **shmid** is materially safer than deletion by semaphore name: simple key replacement does not redirect `IPC_RMID` to the new segment. Nevertheless, production reopening by key and identity continuity still need protection.

**Required amendment:** Specify enforceable exclusion of competing mutation from creation through teardown and cleanup, including receipt capture, or retain the host-profile block. For the cited production error path, the smallest local mitigation is a compile-time-gated D2 change in already-allowlisted `GW.cpp` that suppresses unchecked unlink and records uncertainty for controlled cleanup. This requires explicit documentary approval and does **not**, by itself, resolve the general name-based cleanup race. Merely adding another inode check is insufficient.

An eventual `INCONCLUSIVE` result or inventory mismatch cannot undo foreign deletion.

### B3 — Required verification evidence lacks a complete representable lifecycle

**Locations:** `evidence_schemas.collection_responsibility.harness_run`; `runtime_bundle`; `semantic_finalizer_contract.artifact_checks` and `inventory_checks`; `ipc_inventory`; `run_record_host_profile_delta.reservations`.

The finalizer is required to verify evidence whose storage and binding are not fully specified:

- A durable before/after-create journal is mandatory, but no journal location, entry/state format or final-bundle binding is defined. The allowlisted files contain no designated journal; completed reservation arrays and `journal_fsynced=true` do not themselves define one.
- The finalizer must reconcile raw `/proc/sysvipc` digests with normalised summaries, but the inventory schema retains only digests and summaries, not the raw table bytes needed for that recomputation.
- Libc evidence consists of a version string, expected pathname and verification boolean. No verification procedure or retained evidence binds the exact bare-decimal production name, returned semaphore object and observed inode to the target libc. The recorded `glibc 2.39` identity alone does not prove that mapping.

**Required amendment:** Define where these bounded evidence records live, their schema/state transitions and the finalizer’s verification rules. They may be embedded in existing allowlisted artifacts; extra files are not inherently necessary. Preserve honest partial/failure evidence without asserting successful mapping or reservation. Unverified or mismatched mapping must remain `INCONCLUSIVE`, with no speculative deletion.

## 3. Optional advisories — non-blocking

- Say **five candidate key/name pairs: five SHM segments plus five semaphores**, rather than “five resources”.
- Add documentary negative cases for same-UID replacement, failure after creation but before receipt persistence, libc-mapping mismatch and cleanup-time replacement.
- The schema/finalizer division is reasonable: cross-artifact equality need not be duplicated in JSON Schema. PASS and safe FAIL are conservatively constrained **in intent**, but schema-valid labels cannot discharge B2 or the evidence gaps in B3.
- Allowing unrelated non-candidate IPC is reasonable. Exact global inventory equality will conservatively make runs `INCONCLUSIVE` when unrelated host activity changes those inventories; that is not itself a safety defect.

## 4. Gate statement

**Documentary outcome only: AMEND. All operational authority remains NO.** This review authorises no implementation, configure/build, harness or finalizer execution, IPC, namespace, VM, SSH, D3 measurement, commit, push or release.