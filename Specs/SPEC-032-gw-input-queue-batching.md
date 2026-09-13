# SPEC-032: GW input queue batching

**Status:** Deferred — previous implementation withdrawn 2026-09-09
**Branch:** AIOPT3
**Decision:** Restore the pre-SPEC-032 baseline and replan SPEC-033 first.
**Rollback commit:** 77efe27fafbd7483507ed1448cbeedf04782bff4
**Related:** SPEC-031, SPEC-033

No GW batching implementation is active. Do not resume the diagnostic batching
loop or carry its scheduler changes into SPEC-033.

The original specification and evidence are retained in
`../Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/` and the preservation tag
`recovery/pre-spec032-rollback-20260909T061553`.

Reopening requires a new explicit user decision, validated message-lifetime and
scheduler contracts, and paired baseline/candidate end-to-end measurements.
SPEC-032 is not a prerequisite for SPEC-033.
