# SPEC-033-R3-B: GW caller-safety amendments

Status: Draft for Astra review — no production changes
Branch: AIOPT3
Parent: SPEC-033-R3-caller-safety-amendment.md

## Findings

1. `Common/src/GW.cpp:850` in private `GW::ReadFromSharedMemory3()` calls
`PP->NewMessage(GetTime(), 0, false, PM);` without checking status or PM, then
immediately calls `PM->SetMessageFromCharArray(...)` at line 852 and later
`PushToInputQueue(PM)` at line 892.
2. `Common/src/GW.cpp:113` in the GW constructor calls `PP->NewMessage(...)`
without checking status, then dereferences `PIM` at line 116 and passes it to
`Run()` at line 119. This is a separate lifecycle/startup failure path.
3. `Common/src/GW.cpp:609` already checks `NewMessage() == OK` before use and is
not part of B1, although its drop/cleanup semantics require evidence.
4. `PushToInputQueue(nullptr)` at lines 265–269 calls `M->MarkToDelete()` in
the null branch and is a separate B2 queue-hardening issue.

## Proposed split

B0 — constructor/startup allocation failure (`GW.cpp:113-122`)
B1 — shared-memory receive allocation failure (`GW.cpp:847-904`)
B2 — null input-queue rejection (`GW.cpp:214-270`)
B3 — network receive already-checked path (`GW.cpp:603-622`), only if cleanup
or capacity-drop behavior is shown unsafe.

Each amendment must keep its own test fixture, production file scope, commit and
Astra gate. No `Process.cpp`, headers, queue implementation, parser, wire format,
or lifecycle redesign is included.

## B1 proposed contract

When shared-memory data is valid but Process allocation returns `ERROR`, GW must:
- not dereference PM;
- release `HeaderTimeStampField` and `Payload`;
- release the semaphore and detach shared memory using the existing cleanup path;
- mark the SHM segment free exactly as the existing consumed/error path requires;
- return/continue as an error/drop without enqueueing a null message;
- preserve Process counters and avoid marking unrelated messages.

The valid-success path and malformed-size path must remain unchanged.

## Test constraint

`ReadFromSharedMemory3()` is private. The existing GW constructor itself has an
unchecked allocation at B0, so a capacity-exhaustion fixture must not bypass
object lifecycle with an unreviewed `private` access hack. Astra should choose
whether to authorize a narrowly scoped test seam, a friend declaration, or an
integration fixture that drives the public `Gateway()` path. No seam is proposed
as production code in this draft.

## B1 characterization result

A public-lifecycle fixture was added at
`/home/gandalf/workspace/ng-spec033-characterization-20260909/gw_b1_shm_exhaustion.cpp`.
It constructs `Process` and `GW` normally, fills the Process after GW startup,
creates a valid shared-memory segment, injects a bounded message, and drives the
public `Gateway()` with normal stop/teardown.

The first public-lifecycle fixture compiled successfully and reached the receive
path, but its baseline crash was initially misattributed because pending startup
work could fail first. An ASAN run after adding a normal-capacity warmup produced
an exact stack attribution to `Message::SetMessageFromCharArray()` at
`Message.cpp:865`, called from `GW::ReadFromSharedMemory3()` at `GW.cpp:857`,
with a null `PM` after allocation failure. This confirms the B1 dereference.

The subsequent synchronized fixture revision did not complete: the warmup/public
Gateway cycle remained blocked for 420 seconds and was terminated. Its run is not
acceptance evidence. The fixture therefore remains test-only and must be redesigned
with a bounded, independently observable warmup/termination condition before any
B1 production authorization. No B1 production code has been changed.

### B1 fixture redesign (test-only)

The replacement fixture removes the temporal warmup as a readiness criterion. The
`GW` constructor completes synchronously, and the test then prepares one valid SHM
message before starting the public `Gateway()` cycle. Readiness and completion are
observable only through the SHM state transition `w -> f`; the worker has a hard
bounded deadline and is stopped through the public `SetStopGatewayFlag()` path on
both success and timeout. The fixture records whether the transition occurred,
the final Process count/marking state, and the cleanup result. A timeout is an
inconclusive fixture failure, never acceptance evidence.

The fixture continues to use the normal public lifecycle and does not add a private
access seam, friend declaration, production callback, or API change. The valid
payload is deliberately minimal and independent of the Process capacity fill. The
acceptance assertion remains narrow: when the real `NewMessage()` allocation fails,
`ReadFromSharedMemory3()` must not dereference or enqueue `PM`, must complete the
existing SHM cleanup/free-state path, and must preserve Process occupancy and
marking counters.

### B1 redesign characterization result

The rewritten fixture was compiled against the restored baseline and executed in
three isolated trials with a unique SysV IPC key per process and a 5-second public
Gateway completion deadline. All trials reached `PHASE shm-prepared` and then
terminated with exit code 139 (`SIGSEGV`) in the known baseline B1 path; none hung
in the former unbounded warmup. The fixture cleanup wrapper removed all IPC
segments in its reserved key range after each trial. These runs are RED
characterization evidence only, not acceptance evidence and not authorization for
production changes.

The warmup-observable version then completed the public warmup at `count=3` in
all three normal trials, filled the Process to 30,000, reached `PHASE shm-prepared`,
and terminated with exit code 139. An ASAN build independently attributed the
first failure to the intended B1 path:

```text
Message::SetMessageFromCharArray(Message.cpp:865)
GW::ReadFromSharedMemory3(GW.cpp:857)
GW::Gateway(GW.cpp:649)
```

This closes the previous startup-attribution gap, but the process still crashes on
the unchanged baseline by design. The external runner remains responsible for
watchdog enforcement and orphan-IPC cleanup on crash/timeout until the fixture's
post-patch cleanup assertions are exercised.


A RED fixture was added at
`/home/gandalf/workspace/ng-spec033-characterization-20260909/gw_b0_constructor.cpp`.
Against the baseline, normal control preserved the historical `+3` message and
`+1` marked behavior, while full capacity caused constructor exit `139`.

The authorized B0 guard was implemented only in `Common/src/GW.cpp:113-122`.
The existing `PIM = 0` initialization at line 96/97 was confirmed. Post-fix:

- normal control passed 3/3;
- full-capacity construction passed 3/3 with unchanged count and marking;
- full CMake build passed;
- no other production file changed.

GPT-6 Astra verdict: **ACCEPT B0 — complete**. Commit `1f89c31` contains only
B0 and was pushed to `origin/AIOPT3`. B1, B2 and B3 remain excluded and blocked.
Astra ordering is B0 → B1 → B2, with B3 evidence-gated. B2 must include both
the null-branch dereference and the DEBUG dereferences before the null check.

1. Should B0 precede B1 because constructor allocation failure is earlier in the
lifecycle and can prevent reaching shared-memory receive?
2. Is B1's cleanup/drop contract complete, especially SHM free-state handling?
3. What test seam, if any, is acceptable for the private method without changing
runtime API or ownership semantics?
4. Should B2 remain separate from B0/B1?
5. Authorize only one next production group and list exact files/lines.

## B1 test-only controls — 2026-09-10

The fixture was extended with three explicit modes without changing production
source: `valid`, `malformed-zero` and `exhaustion`.

- Valid serialized-message control: 3/3 PASS, `rc=0`, `consumed=1`,
  `before=3`, `after=4`, `marked_before=0`, `marked_after=1`, cleanup OK.
- Malformed-size control (`total_size=0`): 3/3 PASS, `rc=0`, `consumed=1`,
  `before=3`, `after=3`, `marked_before=0`, `marked_after=0`, SHM returned to
  `f`, cleanup OK.
- Allocation-exhaustion control: 3/3 baseline RED, reached `PHASE
  shm-prepared`, then exited 139 in the known B1 path.

The complete machine-readable record is
`Specs/RESULTS-SPEC-027/spec033-r3-b1-controls-20260910.json`. These results
close the two missing test-only controls and prepare a fresh Astra review. They
do not authorize production changes; the unchanged baseline RED is retained.

Direct valid and two boundary malformed-size controls each pass 3/3 through
the corrected file-backed runner. The current-source ASAN exhaustion run
reproduces the intended RED at `Message.cpp:865` via `GW.cpp:857`. The earlier
runner discrepancy is resolved. These results prepare a fresh Astra scope
review but do not authorize production changes; the baseline RED is retained.