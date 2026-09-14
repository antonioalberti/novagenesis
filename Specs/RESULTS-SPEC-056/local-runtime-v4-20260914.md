# Local runtime trial — PGCS crash after readiness

**Date:** 2026-09-14
**Branch:** `AIOPT3`
**Candidate:** `d303b05ad2ea379308b7dbb41445f456f595d2c3`
**Trial:** `m0-runtime-v4`
**Plan:** `Scripts/AlpineVMs/plans/local-intra-os.example.json`

## Result

The fresh canonical local NG-ELC trial passed provenance/build linkage eligibility and launched the real processes, but did not reach payload runtime acceptance:

```text
runtime_result=INCONCLUSIVE
teardown_result=UNKNOWN
 evidence_result=INCOMPLETE
exit_code=20
```

The final result recorded an evidence publication blocker because the early abort left no repository artifact area. This is a consequence of the runtime abort, not a successful trial.

## First failing edge

- PGCS launched and emitted `State: Operational`.
- NRNCS launched, but during its readiness wait the controller observed PGCS exit with return code `-11` (SIGSEGV).
- The controller stopped NRNCS; Repository and Source ContentApp roles were not launched.
- The lifecycle inventory recorded no residual trial processes or owned IPC after cleanup.

PGCS stdout ended immediately after the Core block was created. No stderr was emitted. The retained bundle is under the ignored local evidence root:

```text
cmake-build-debug/.ng-evidence/local-intra-os-v4-d303b05/m0-runtime-v4/
```

## Interpretation

This is a runtime correctness/lifecycle blocker for M0/M1. It is not evidence of a payload hash failure, subscription failure or remote transport failure, because the trial stopped before those stages. No release acceptance is claimed.
