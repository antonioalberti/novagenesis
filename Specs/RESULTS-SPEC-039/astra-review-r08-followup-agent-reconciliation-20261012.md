## 1. Correction to prior B1 — WITHDRAWN

B1 was a **submission-envelope error, not a packet defect**. The supplied packet consistently uses:

`bc735702fff9adde69a00705715e4025cedca5f3e1314eb7200da29712822992`

This appears in “Frozen identity and privacy”, “Full SPEC SHA-256”, manifest `spec.sha256` and `run_record_host_profile_delta.spec_sha256_binding`.

The sender’s earlier digest omitted `128`. I withdraw the conflicting-packet-identity finding and its requirement for packet amendment or fresh consent **on that account**. This correction concerns the unchanged packet.

## 2. Remaining blockers

### B2 — SUPPORTED, with a qualification on the proposed remedy

The supplied text supports independent review A:

- **Creation is not continuous exclusion.** §7.1 specifies exclusive reservation and `0600` permissions, but no enforceable exclusion of same-UID competing mutation through receipt capture, production use and cleanup.
- **Production opens are not identity-bound.** `GW.cpp:693,713` and `1300,1306` use `O_CREAT`/`IPC_CREAT`, allowing attachment to replacements or recreation after disappearance.
- **Unchecked unlink is explicit.** `GW.cpp:1318–1322` calls `sem_unlink` following `sem_open` failure, without checking ownership or identity.
- **Cleanup remains check-then-act.** §7.1 and D2b require identity rechecking before name-based unlink; they do not make that check and unlink atomic. Replacement can also occur between semaphore creation and pathname-based receipt capture.

The finalizer’s `inventory_checks` requirement that the production failure path “cannot cause” foreign unlink states an obligation, not a mechanism satisfying it. Subsequent `INCONCLUSIVE` classification cannot undo deletion.

**Required documentary resolution:** establish enforceable identity continuity and exclusion across the relevant lifecycle, or retain the host-profile block. Suppressing the reachable unchecked unlink is a possible local mitigation within already-allowlisted `GW.cpp`, but is neither a complete remedy nor an approved change. Likewise, “INCONCLUSIVE/no cleanup when ambiguous” is safe only if unsafe production deletion is also prevented; it does not itself establish safe execution.

SHM removal by recorded **shmid** is materially safer than semaphore removal by name; the two should not be treated as identical races.

### B3 — SUPPORTED, with precision about the evidence gap

The supplied projection supports independent review B:

- **Journal lifecycle:** `collection_responsibility.harness_run` requires fsynced before/after-create records, but `runtime_bundle` and `reservations` do not designate their storage, event/state representation, recovery rules or binding into retained final evidence. Reservation arrays plus `journal_fsynced=true` do not supply that contract.
- **Raw inventory verification:** `semantic_finalizer_contract.inventory_checks` requires raw `/proc/sysvipc` digests and normalised summaries to agree. The complete `ipc_inventory` schema retains digests and summaries, not the raw bytes needed to recompute those digests and validate their relationship to the summaries.
- **Semaphore mapping:** §7.1 requires verification, but `reservations` supplies a libc version, name/path, inode metadata and a verification boolean without defining the procedure and evidence connecting the **returned semaphore object** to that pathname identity.

**Qualification:** the packet already defines general artifact hashing and an outer-index hash for the run record. Therefore, the defect is not “no cryptographic binding anywhere”; it is the absence of a specified retained journal/mapping/raw-evidence representation covered by those mechanisms. Existing allowlisted artifacts could hold that evidence; additional files are not inherently required.

**Required documentary resolution:** define bounded evidence storage, lifecycle and verification rules, including honest partial/failure records. Preserve `INCONCLUSIVE` and no speculative deletion when mapping or ownership cannot be established.

There is no substantive disagreement with reviews A/B after these qualifications. Review C’s envelope correction is consistent with the supplied text; archived SSE and deduplication claims are not independently verified here.

## 3. Corrected documentary verdict — AMEND

For unchanged packet SHA-256  
`3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272`:

**B1 withdrawn; B2 and B3 remain blocking.** The prior overall verdict therefore remains **AMEND**, solely on lifecycle safety and evidence-contract grounds.

**Optional advisory, not a blocker:** say “five candidate key/name pairs: five SHM segments and five semaphores”, rather than “five resources”.

This assessment uses only the supplied packet and correction context. No tools, hash recomputation or runtime validation were performed; the r07 review boundary remains unchanged.

## 4. Gate

**Documentary only; operational authority remains NO.** This response authorises no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM, SSH, D3, cleanup, commit, push or release.