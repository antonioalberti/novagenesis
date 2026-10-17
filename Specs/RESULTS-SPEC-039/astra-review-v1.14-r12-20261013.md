## 1. VERDICT: AMEND — documentary scope only

Reviewed the supplied r12 packet identified as SHA-256 `a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd`, with the supplied full SPEC and manifest as consistency context.

**r11 B1 is closed at documentary level:** both modes share the lease, supervisor and result-directory authority gate; authority loss blocks entry/re-entry and preserves pending evidence. Bounded abnormal raw preservation is substantially addressed, and r08 B2/B3 protections remain specified. **Truthful cleanup-history classification still has the contradictions below.**

## 2. MUST-FIX blockers

### B1 — The supplied cleanup classifier accepts an invalid transaction as `not_started`

**Locations**
- `verify_spec039_v14_r12.py`, `classify()`: shared `pending` set, `REMOVED`/`CLEANUP_FAILED` handling, and `intent_count == 0` return.
- `precode-manifest-v1.14-r12.json`, `local_validation_this_freeze.cleanup_history_outcome_fixtures`.
- Primary packet, `validation_claims.cleanup_history_outcome_fixtures`.
- SPEC §7.1, bounded regression rules requiring malformed/conflicting history to yield `unknown`.

**Concrete contradiction:** by inspection, the following stream, using the supplied schema-valid event shape and consecutive sequence/timestamp values, returns `not_started`:

```text
JOURNAL_OPENED
CREATE_INTENT(resource A)
RUN_SHUTDOWN_COMPLETE
REMOVED(resource A)
JOURNAL_CLOSED
```

`REMOVED` consumes the pending **creation** intent despite there being neither `CREATED` nor `CLEANUP_INTENT`. With no successful create and zero cleanup intents, the final branch returns `not_started`, ignoring the contradictory removal outcome. The classifier also lacks a prohibition on creation transactions after shutdown.

This contradicts the claimed rejection of malformed/reordered history and the evidence-derived outcome contract.

**Required amendment:** distinguish creation and cleanup transaction states, enforce lifecycle phase ordering, and reject orphan or wrong-kind outcomes as `unknown`. Align the bounded classifier contract and validation claims; include these specific negative cases. Invalid history must still permit safe raw preservation and an otherwise authorised `INCONCLUSIVE` record.

### B2 — Normal-finalisation failure rules still exclude a supported `complete` cleanup outcome

**Locations**
- SPEC §7.1, paragraph beginning **“In `normal_cleanup` mode, capture a complete final inventory…”**, and numbered item **5**: after an intent, failure requires `cleanup_outcome=partial` or `unknown`.
- Manifest `sandbox.finalizer_ipc_boundary.failure_rule` and its duplicate under `semantic_finalizer_contract.finalizer_ipc_boundary`.
- Conversely, manifest `semantic_finalizer_contract.normal_cleanup_failure` explicitly permits `partial/complete/unknown`.
- These conflicting clauses are reproduced in the primary packet.

**Concrete contradiction:** all removals can have durable successful outcomes and the journal can be closed, while subsequent evidence validation fails. With common authority intact, cleanup history can support `complete` while overall status remains `INCONCLUSIVE`. The manifest expressly permits that result, but the SPEC’s broader post-intent failure rule excludes it.

**Required amendment:** consistently distinguish destructive-cleanup failure from later finalisation/evidence failure. Permit `complete` when supported by the historical cleanup evidence, without implying PASS, verified final inventory or permission to retry.

**Other requested checks:** no stale unconditional abnormal `not_started` requirement remains in `classification_rules.pass_requires` or `fail_requires`; the abnormal schema includes `complete`. Missing/unavailable enums cover the eight required evidence names and exclude the record and optional diagnostics; exactness and disjointness remain semantic requirements. PID cross-artifact validation is explicitly **NOT EXECUTED**, not falsely reported as passed.

**Optional advice:** consolidate duplicated policy clauses and clearly separate historical fixture counts from current claims.

## 3. Evidence limitation

I used no tools, executed no code or tests, recomputed no hashes, and independently inspected no filesystem, launcher or runtime artefacts. The classifier counterexample is textual reasoning, not an executed probe. Supplied synthetic checks and compile/link-only D1-H evidence do not establish D2 runtime correctness or performance.

## 4. Authority boundary

**No implementation, configure/build, runtime, harness/finalizer, IPC, cleanup, VM/SSH, commit, push or D3 authority is granted.** The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED** pending the separate prerequisites and owner decision. This review authorises no follow-up call.