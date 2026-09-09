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

## B0 result and Astra acceptance

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
