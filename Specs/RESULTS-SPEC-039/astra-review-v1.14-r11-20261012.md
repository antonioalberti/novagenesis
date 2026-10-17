## Verdict: AMEND — documentary only

Performed the single review authorised by owner message **1559215860005081219**, for packet SHA-256 `feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829`.

Reviewed only the supplied text; no tools, hash recomputation, code execution or runtime validation. **The original join/destruction prerequisite conflict is resolved, but two related contradictions remain.**

## Must-fix blockers

### B1 — Continuous lease remains an unconditional prerequisite for abnormal finalisation

**Locations**
- Manifest `sandbox.finalizer_ipc_boundary.preconditions.both_modes`, repeated under `semantic_finalizer_delta.finalizer_ipc_boundary`: requires a “valid dedicated non-login UID launcher and continuous lease”.
- `sandbox.finalizer_mode_selection`: otherwise permits abnormal evidence-only finalisation after confirmed reaping/log closure when normal eligibility—including continuous lease—fails.
- §7.1 normal/abnormal sequence: an uncertain lease selects no-cleanup handling, followed by the evidence-only finalisation contract.

**Conflict:** after lease loss, confirmed termination and closed logs are insufficient to satisfy the explicit *both-modes* prerequisite. The contract therefore both offers abnormal finalisation and prohibits invoking it.

**Required amendment:** distinguish the trusted-supervisor/evidence prerequisites from continuous exclusion required for IPC inspection and cleanup. Explicitly define whether lease loss permits evidence-only finalisation, and under what file-safety conditions. If it instead prevents finalisation, consistently require pending/partial evidence and `BLOCKED/INCONCLUSIVE`, rather than promising a final record. Retain the prohibition on IPC operations and automatic recovery cleanup.

### B2 — Recovery after a cleanup attempt cannot truthfully report `not_started`

**Locations**
- §7.1 normal/abnormal sequence includes “a finalizer failure requiring recovery” among abnormal-mode cases.
- The same clause, `semantic_finalizer_delta.abnormal_evidence_only.cleanup_outcome`, and run-record v5 `abnormal_evidence_only_constraints` unconditionally require `cleanup_outcome=not_started`.

**Conflict:** a normal finalizer can fail after `CLEANUP_INTENT` or a destructive call. A subsequent evidence-only recovery record would have to misreport historical cleanup as never started, or breach its schema. Performing no cleanup **during abnormal finalisation** does not establish that no earlier cleanup occurred.

**Required amendment:** either restrict this abnormal-mode contract to runs with no prior cleanup attempt and specify a separate pending/recovery disposition, or permit an evidence-derived historical cleanup outcome for recovery. Preserve ambiguity, prohibit retries, and keep recovery `INCONCLUSIVE`.

## Bounded findings otherwise satisfied

- **Normal gating:** exactly one durable `RUN_SHUTDOWN_COMPLETE`, after joins/destruction and before cleanup intents, plus verified supervisor wait/log evidence is expressly required.
- **Supervisor binding:** the semantic contract requires exact recorded PID-set equality, unique per-PID raw wait receipts, decoded termination states and hashes for present closed logs. JSON Schema alone does not enforce these cross-artifact checks.
- **Mode selection:** expressly evidence-derived, not caller-controlled, subject to B1 above.
- **Abnormal restrictions:** only `INCONCLUSIVE`; no IPC inspection, cleanup or journal mutation. Present but unverified post/final files remain artifacts, not verified inventories.
- **Schema representation:** the shown nullable reservation fields and journal v2 event support the amendment without dummy resource values. Full observation-field compatibility cannot be independently established from omitted, digest-only schema portions. PASS/FAIL remain contractually dependent on complete normal evidence.
- **Bundle accounting:** eight required evidence artifacts, one optional diagnostic, and the separate run record correctly give a ten-file maximum. Missing-artifact semantics exclude optional diagnostics and present-but-unverified files.
- **B2/B3 safeguards from prior reviews:** retained at documentary level; no new false-PASS route is established by the supplied changes.

## Optional advice

Scope the generic strict-parsing and “eight artifacts exactly once” checks explicitly to normal eligibility: malformed or absent evidence must remain preservable in abnormal mode, not prevent its permitted `INCONCLUSIVE` record.

Journal bytes alone do not independently prove historical fsync completion. Reported local validation results are supplied claims, not validation performed in this review.

## Authority boundary

The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED**. This verdict grants **no operational authority**: no implementation, configure/build, harness/finalizer execution, runtime/IPC, namespace, VM/SSH, D3, cleanup, commit, push or release.