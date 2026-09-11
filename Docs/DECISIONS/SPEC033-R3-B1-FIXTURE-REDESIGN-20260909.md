# SPEC-033 R3 B1 — Redesign do fixture público

Data: 2026-09-09
Branch: AIOPT3
Status: Test-only characterization complete; production B1 blocked

## Scope

The fixture uses the normal `Process`/`GW` lifecycle and public `Gateway()` path.
It does not add a private seam, friend declaration, callback, API change, or
production edit.

Files:

- Fixture: `<workspace-root>/ng-spec033-characterization-20260909/gw_b1_shm_exhaustion.cpp`
- Watchdog: `<workspace-root>/ng-spec033-characterization-20260909/run-gw-b1-fixture.py`
- SPEC: `Specs/SPEC-033-R3-B-gw-caller-safety.md`

## Changes

- Public startup work is drained before capacity exhaustion, using observable
  `Process` count `<= 3` and a 5-second warmup deadline.
- The B1 SHM message is prepared before starting the public Gateway cycle.
- Completion is observed only through the SHM transition `w -> f`.
- The fixture uses a unique SysV IPC key per process.
- The fixture has a 5-second SHM completion deadline and always requests public
  Gateway stop before joining on the normal path.
- Normal-path cleanup checks all four SHM slots and all four named semaphores.
- The external runner imposes a 20-second whole-process watchdog, sends SIGTERM,
  escalates to SIGKILL after 2 seconds, records crash/timeout separately, and
  removes fixture-owned IPC after every trial.

## Baseline evidence

Normal binary, three isolated trials:

- `warmup_done=1` in 3/3;
- `shm_prepared=1` in 3/3;
- exit `139` (`SIGSEGV`) in 3/3;
- no watchdog timeout;
- reserved IPC range empty after cleanup.

ASAN attribution:

```text
Message::SetMessageFromCharArray(Message.cpp:865)
GW::ReadFromSharedMemory3(GW.cpp:857)
GW::Gateway(GW.cpp:649)
```

The stack is reached after warmup and capacity fill, so the earlier startup
attribution gap is closed. The crash is RED characterization evidence only.

## Astra status

The first complete Astra review returned `NO-GO` for production implementation.
It required:

- demonstrated whole-process watchdog and bounded teardown;
- exact B1 attribution after startup isolation;
- checked SHM and semaphore cleanup across all parallel slots;
- valid serialized-message and malformed-size controls;
- post-patch GREEN with normal worker termination and cleanup verification.

The watchdog, attribution and external crash cleanup are now demonstrated. Two
Astra review retries returned `0/0` tokens and produced no new verdict. Therefore
no production implementation is authorized by this record yet.

## Updated test-only controls — 2026-09-10

The public fixture was extended without production changes and compiled against
the current AIOPT3 source. Valid serialized-message and malformed-size controls
both passed 3/3 direct trials with `cleanup_ok=1`. The allocation-exhaustion
control remained RED in 3/3 trials, reaching `PHASE shm-prepared` and exiting
139 on the unchanged baseline B1 path. Full results:
`Specs/RESULTS-SPEC-027/spec033-r3-b1-controls-20260910.json`.

The fixture is ready for a fresh Astra review. Production B1 remains blocked.

## Updated Astra review — 2026-09-10

A fresh GPT-6 Astra review was performed after the controls. Verdict:
**CONDITIONAL GO — submit B1 for fresh scope review only**. No production
implementation was authorized.

Astra confirmed:

- valid control: 3/3 through the corrected file-backed runner, with expected
  message/marking changes;
- malformed-zero control: 3/3 through the corrected runner, with unchanged
  counters, free SHM state and cleanup;
- malformed-max control: 3/3 through the corrected runner, with unchanged
  counters, free SHM state and cleanup;
- exhaustion: repeatable RED, freshly attributed on the current source by ASAN
  to `Message.cpp:865` via `GW.cpp:857`;
- the earlier runner discrepancy was resolved by changing capture from a pipe to
  a file-backed child log; all nine normal control trials now return `rc=0`;
- production acceptance still requires the post-change RED→GREEN assertions.

The current-source ASAN report is preserved at
`<workspace-root>/ng-spec033-characterization-20260909/exhaustion-current-asan.log`.

## Remaining gates

1. Send the complete B1 source, runner, logs, current ASAN evidence and exact
   production boundary for formal scope review.
2. Obtain an explicit conditional GO for the B1 production change.
3. Obtain user approval for the exact production file/line scope.
4. Implement, rebuild and run RED→GREEN plus matched runtime validation.

No production implementation is authorized at this stage.

## Subsequent Astra scope review — 2026-09-10

A later review with the complete fixture, corrected runner, current ASAN log and
expanded results returned **NO-GO for B1 production implementation**. It allows
continued test-only remediation.

- the valid control now constructs a Message through the production `Message`
  API, serializes `-run --initialization 0.1`, injects those bytes, and asserts
  `serialized_roundtrip_ok=1` plus a parsed command in the received message;
- the exhaustion control prepares the same serialized bytes before filling
  Process capacity, so RED exercises valid data plus allocation failure;
- two boundary malformed-size controls pass 3/3 through the corrected runner;
- retained message flags, command-bearing messages, primary SHM free state and
  all four slot/semaphore accessibility are asserted before external cleanup;
- runner mode expectations are now explicit: baseline crash is accepted only in
  `exhaustion` mode, while unexpected normal-mode crashes and cleanup failures
  fail the run;
- complete `ReadFromSharedMemory3()` cleanup/control-flow context was supplied to
  Astra; production-cleanup proof remains a post-change acceptance gate;
- marker-based received-message identity, attachment count (`shm_nattch=0`) and
  exclusive fixture ownership are now asserted;
The existing controls remain characterization evidence. Astra's latest review
provided a **conditional GO for implementing only B1**, limited to
`Common/src/GW.cpp::ReadFromSharedMemory3()`, original lines 852–914. This is
not a merge or release acceptance; the exact post-change gates remain mandatory.

## B1 implementation and local validation — 2026-09-10

After explicit user approval, the production change was limited to
`Common/src/GW.cpp::ReadFromSharedMemory3()`, original lines 852–914. The
allocation status and pointer are now checked before PM use; failure skips
conversion/debug/enqueue and preserves the existing cleanup route. The
unconditional allocation-path `Status = OK` was removed while scan-level status
aggregation remains cumulative.

Verification:

- Full CMake build: PASS.
- Valid, malformed-zero, malformed-max and exhaustion-fixed runner modes: 3/3
  PASS each, normal exit 0.
- Rebuilt ASAN versions of all four modes: exit 0, no sanitizer report.
- Production diff: `Common/src/GW.cpp` only.

Formal Astra post-change acceptance and matched Alpine runtime validation remain
pending. Do not merge/release based only on these local controls.

## Final Astra acceptance — 2026-09-10

Astra reviewed the exact committed change `e086868` and the post-change
results. Verdict: **PASS — B1 accepted independently**.

- Local fixture: PASS, including normal, ASAN and runner-negative tests.
- Matched Alpine runtime: PASS on repository guest/source guest after fresh VM restart;
  100/100 JPEGs matched the publish-time manifest, with zero missing or
  mismatched files.
- Scope: only `Common/src/GW.cpp::ReadFromSharedMemory3()`, original lines
  852–914.
- B2/B3/C remain blocked and are not accepted by this verdict.
- This does not constitute broader merge/release approval.

Durable evidence: `<local-repository-path>/IO/NG-018-B1-runtime-20260910/sha256-verification.json`.
