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

## Remaining gates

1. Add a valid serialized-message success control and malformed-size regression
   control without changing production code.
2. Obtain a usable fresh Astra review of the complete updated evidence.
3. Only if Astra gives conditional GO, apply the separately authorized B1 change
   to `Common/src/GW.cpp` lines 847–904.
4. Rebuild and run the identical fixture GREEN, then perform matched VM runtime
   validation.
