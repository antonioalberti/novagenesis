## 1. VERDICT: AMEND — documentary scope only

Reviewed only the supplied packet identified as SHA-256 `55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146`. No tools, hash recomputation or runtime validation were used.

The original r09 prohibition on “touching IPC” is resolved. The finalizer’s IPC allowance is now narrowly and consistently limited to required inventory reads, exact current-identity inspection, and journal-authorised `IPC_RMID`/`sem_unlink`, with an immediate identity and lease recheck before each destructive call.

However, **one finalizer-sequence contradiction remains on abnormal termination**.

## 2. Must-fix blocker

### B1 — Unconditional finalizer startup prerequisites conflict with required failure finalisation

**Exact packet locations**
- **§7.1, numbered item 2:** uncertain ownership requires `_Exit` without invoking potentially unsafe `Process/GW` destructors; after process exit, “only the parent-side finalizer in item 5” may produce the final `INCONCLUSIVE` record. Partial construction and blocked join follow the same fail-closed classification.
- **Manifest `collection_responsibility.outer_finalizer`:** the finalizer “starts only after” harness exit/log closure, worker stop/join **and `Process/GW` destruction**.
- **Manifest `finalizer_ipc_boundary.owner_and_sequence` and `.preconditions`:** likewise require completed worker joins and `Process/GW` destruction.

**Concrete risk:** the specified abnormal-exit path deliberately cannot establish those startup prerequisites. An implementation must therefore either breach the unconditional startup rule to produce the required final record, or omit the finalisation explicitly assigned to it. Process termination is not evidence that C++ destructors or joins completed.

**Required amendment:** distinguish normal cleanup sequencing from abnormal-exit evidence finalisation. Explicitly permit the same sole finalizer to produce an `INCONCLUSIVE` record after confirmed process-tree termination/log closure when joins or destruction were incomplete or unavailable. Separately state the cleanup eligibility for that path, retaining all journal, identity and continuous-lease gates—or explicitly prohibit destructive calls there. Do not treat termination as successful join/destruction. Align the cited SPEC and manifest clauses.

## 3. Safeguards and optional advisories

**Safeguards remain intact at documentary level:**
- Finalizer construction of `Process/GW`, candidate creation/opening, production message traffic, broad/speculative cleanup and retry after ambiguous removal remain prohibited.
- Lease loss or uncertainty, journal ambiguity and identity mismatch prohibit destructive calls and preserve residue; ambiguous intent/outcome records cannot establish removal or trigger automatic retry.
- B2/B3 protections remain: dedicated-UID continuous exclusion, diagnostic suppression of all three pinned failure-path unlinks, retained/replayed journal and raw inventories, mapping evidence, and evidence-derived classification. These are requirements, not implementation or runtime proof.

**Optional r09 advisories remain non-blocking:** tighten mapping-evidence representation; separate historical fixture counts from newly required cases and remove duplicated policy entries; clarify that journal bytes do not independently prove historical fsync calls. No unchanged r07 checker findings are reopened.

## 4. Authority boundary

The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED**. **No operational authority is granted**: no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM/SSH, D3, cleanup, commit, push or release.