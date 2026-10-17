## Verdict: AMEND — r12 B1/B2 documentary closure only

I assessed only the supplied packet, identified by the required SHA-256 `0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727`. This is a supplied identity, not a hash I independently recomputed.

**The canonical model addresses the original r12 defects, but an attached active policy contradiction prevents documentary closure.**

### Findings on the original defects

- **r12 B1 is addressed in §7.2’s transition contract.** Creation and cleanup have distinct pending states. `RUN_SHUTDOWN_COMPLETE` rejects unresolved creation intents; `REMOVED` and `CLEANUP_FAILED` require `cleanup_pending`. Consequently, the original five-event counterexample cannot validly become `not_started`. Orphan outcomes and wrong-phase creation are rejected.
- **Resource identity continuity is explicit.** `CREATED`, `CLEANUP_INTENT`, and terminal cleanup outcomes must share `(candidate_key, shmid)` or `(source_name, device, inode)`. The supplied helper excerpt checks the binding’s fields and correspondence to the event key/name. This establishes the documentary requirement, not independently verified full-interpreter behavior.
- **r12 B2’s original exclusion of `complete` is corrected.** The attached §7.1 normal-finalization paragraph, numbered item 5, §7.2 `FIN-001`, and manifest failure-rule projections permit historical `complete` after later evidence/finalization failure while overall status remains `INCONCLUSIVE`.
- The attached rules do not make historical cleanup completion sufficient for PASS, permit destructive retry, or override lost authority.

## Must-fix blocker

### 1. Abnormal-mode policy both requires and prohibits the journal replay needed to derive cleanup history

**Exact SPEC locations**

- **§7.1, paragraph beginning “In `abnormal_evidence_only` mode…”**: requires the bounded classifier to validate event shape, sequence, phase/resource transitions, and stable identity binding.
- **§7.2, `COUT-001`–`COUT-004`**: requires a complete, transition-valid closed journal for determinate outcomes; otherwise `unknown`.

**Manifest JSON pointers**  
These refer to the underlying manifest paths represented by the packet’s active-path projections:

- `/sandbox/finalizer_ipc_boundary/abnormal_evidence_only/forbidden/3`
- `/semantic_finalizer_contract/finalizer_ipc_boundary/abnormal_evidence_only/forbidden/3`
- `/sandbox/inventory_requirements_by_mode/abnormal_evidence_only`
- `/sandbox/runtime_finalizer_cleanup`
- `/semantic_finalizer_contract/abnormal_evidence_only/non_control_validation`

Those clauses forbid or exclude parsing/replay of non-control evidence without expressly excepting bounded journal-history classification. Yet the same attached policies require that classification, notably:

- `/sandbox/finalizer_ipc_boundary/abnormal_evidence_only/cleanup_outcome_classifier`
- `/semantic_finalizer_contract/abnormal_evidence_only/cleanup_outcome`
- `/semantic_finalizer_contract/failure_summary_checks/4`

**Why this blocks closure:** The reservation journal is evidence beyond the pending record and supervisor receipt—the two artifacts designated for strict control validation. Establishing terminal coverage, orphan rejection, and physical-identity continuity requires parsing and replaying its history. A blanket prohibition on that operation conflicts with the mandatory evidence-derived outcome contract. Merely preserving/hash-binding raw bytes cannot establish `complete`, `partial`, or `not_started`.

**Required documentary amendment:** Explicitly distinguish:

1. strict control-record validation;
2. bounded, read-only journal parsing/replay solely for `COUT-001`–`COUT-004`; and
3. normal complete-bundle/schema/inventory validation, which must not gate abnormal preservation.

State consistently that malformed, unavailable, truncated, or invalid journal history yields `unknown` without blocking an otherwise authorized abnormal `INCONCLUSIVE` record. Preserve the prohibitions on journal mutation, live IPC inspection, cleanup, retry, and authority bypass.

## Optional advice

- Label the manifest’s 58-method results and the historical receipt’s 61-method results explicitly as **58 model/projection tests plus 3 CLI tests**. The supplied commands explain the difference; this is not a blocker.
- Keep synthetic transition coverage separate from `FIN-001` behavioral coverage. The packet correctly labels destructive-stop/retry behavior as **not executed**.

## Context sufficiency and evidence limitations

The supplied context is sufficient to identify the blocker and assess the canonical B1/B2 corrections. It is **not sufficient to certify whole-file consistency**: omitted SPEC text and digest-only manifest sections were not inspected. In particular, `/evidence_recovery`, `/semantic_finalizer_contract/artifact_checks`, and the remaining run-record schema conditionals would matter to a broader claim about recovery, bundle gating, and schema compatibility. Their digests do not establish their contents or consistency.

The historical validation receipt expressly binds an earlier packet, not this expanded packet. Its synthetic results are reported evidence, not independently reproduced results. The final expanded-packet checker sidecar is not supplied. Cross-artifact PID validation and production-finalizer/runtime behavior remain unverified.

I used no tools, executed no tests, inspected no filesystem, and recomputed no hashes. **No implementation, build, runtime, IPC/VM, D2/D3, commit, push, or follow-up-call authority is granted or triggered.**
