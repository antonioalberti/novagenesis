# SPEC-033 R3 B1 — Redesign do fixture público

Data: 2026-09-09
Branch: AIOPT3
Status: Test-only characterization complete; production B1 blocked

## Scope

The fixture uses the normal `Process`/`GW` lifecycle and public `Gateway()` path.
It does not add a private seam, friend declaration, callback, API change, or
production edit.

Files:

- Fixture: `/home/gandalf/workspace/ng-spec033-characterization-20260909/gw_b1_shm_exhaustion.cpp`
- Watchdog: `/home/gandalf/workspace/ng-spec033-characterization-20260909/run-gw-b1-fixture.py`
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

- valid control: 3/3 direct PASS, with expected message/marking changes;
- malformed-zero control: 3/3 direct PASS and one clean ASAN run;
- exhaustion: repeatable RED, now freshly attributed on the current source by
  ASAN to `Message.cpp:865` via `GW.cpp:857`;
- the external runner discrepancy for malformed-zero must be resolved before
  treating the runner as a reliable acceptance gate or making a favorable
  production-scope decision;
- broader malformed-size coverage, complete fixture/source bundle and explicit
  cleanup assertions remain required for production acceptance.

The current-source ASAN report is preserved at
`/home/gandalf/workspace/ng-spec033-characterization-20260909/exhaustion-current-asan.log`.

## Remaining gates

1. Resolve the malformed-zero external-runner discrepancy and add any required
   malformed-size cases.
2. Send the complete B1 source, runner, logs, current ASAN evidence and exact
   production boundary for formal scope review.
3. Obtain an explicit conditional GO for the B1 production change.
4. Obtain user approval for the exact production file/line scope.
5. Implement, rebuild and run RED→GREEN plus matched runtime validation.

No production implementation is authorized at this stage.