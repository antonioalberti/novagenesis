# SPEC-033 R3-C1 — Test-only manifest

**Source revision:** `0dc2b38e1d95e09877a4b3e7c90cf2d3bce282b9`  
**Status:** Test-only proposal; production `NO-GO`  
**Validator:** GPT-6 Astra

## Selected failure policy

Astra recommends immediate `return ERROR` after a failed first-overload allocation in all three functions:

- `PGRunPublishing01::Run()` — avoids publishing an incomplete iteration while preserving prior queued messages;
- `PGRunHello02::Run()` — avoids building/sending/marking a null or previously marked message;
- `PGHelloIHC03::Run()` — avoids reporting `OK` after incomplete binding-message construction.

No cleanup may delete, mark, unmark or resubmit the output from the failed allocation. Earlier successful effects are not rolled back.

## Proposed test-only manifest

These are proposed paths only; no production hook or caller change is authorized:

1. `<workspace-root>/ng-spec033-characterization-20260909/c1_reused_output_fixture.cpp`
   - public-lifecycle fixture;
   - two success-then-failure iterations for each target loop;
   - real `Process::NewMessage()` capacity exhaustion;
   - valid PGCS tuples, stack/interface/identifier data, Core/GW/HT blocks and command lines.

2. `<workspace-root>/ng-spec033-characterization-20260909/run-c1-reused-output-fixture.py`
   - external watchdog;
   - baseline and future post-fix phases;
   - durable per-trial logs;
   - exact IPC/process cleanup;
   - fail-closed timeout/unexpected-exit handling.

3. `<workspace-root>/ng-spec033-characterization-20260909/c1_event_trace.harness.md`
   - event-to-assertion mapping for allocation, builders, SCN generation, serialization, send, queue submission and marking;
   - identifies which observations are public and which remain unavailable.

4. `<workspace-root>/ng-spec033-characterization-20260909/c1-results.json`
   - source revision and build identity;
   - per-function/per-trial allocation results;
   - downstream-event counts;
   - prior-message identity/liveness;
   - cleanup and teardown results.

## Required test scenarios

For each of the three target functions:

- all-eligible success control;
- first allocation success followed by second allocation failure;
- prior successful message remains valid and is not deleted by the failed iteration;
- failed iteration produces no builder, name-generation, serialization, send, queue or mark event;
- exact `ERROR` return is observed;
- no unrelated tuple, scheduled-message or received-message cleanup occurs.

The fixture must not manipulate private `Process` fields or add production hooks. If event-level observation cannot be achieved through existing public paths, stop and submit a separate seam proposal for review.

## Current Astra disposition

`CONDITIONAL GO — test-only evidence work; NO-GO for production.`

Candidate production allowlist remains limited to the three function/call sites in `SPEC-033-R3-C1-caller-failure-handling.md`. Before implementation, the test manifest, helper contracts and event observability must be reviewed by Astra, followed by explicit user approval of the exact caller allowlist.
