# SPEC-033-R3-B2: GW input-queue null-input safety

**Author:** Antonio Alberti / Hermes Agent  
**Date:** 2026-09-10  
**Status:** Implemented — B2 local boundary accepted by Astra; broader caller/allocator safety remains outside scope.  
**Branch:** AIOPT3
**Implementation commit:** `53f06ff`
**Related:** `SPEC-033-R3-caller-safety-amendment.md`, `SPEC-033-R3-B-gw-caller-safety.md`, `SPEC-033-R3-B1` (accepted independently)

## 1. Context and decision boundary

NG-020/SPEC-033 R3 is split into independently reviewable caller-safety groups. B0 and B1 are already accepted independently; this SPEC covers only the input-queue boundary.

B1 remains closed and unchanged:

- production file: `Common/src/GW.cpp`;
- function: `GW::ReadFromSharedMemory3()`;
- original scope: lines 852–914;
- implementation: commit `e086868`;
- final documentation: commit `c38eb08`;
- matched Alpine runtime: 100/100 JPEGs with coincident SHA-256, zero missing/mismatched.

B2 is not an allocator-contract, `Process.cpp`, caller-migration, queue redesign, batching, parser, wire-format, B3 network-path, or C output-contract amendment. It addresses the null boundary of `GW::PushToInputQueue(Message*)` only.

The current baseline contains unconditional dereferences before the null check:

- `GW.cpp:226`: `M->InstantiationNumber` under `DEBUG`;
- `GW.cpp:229`: `operator<<(*M)` under `DEBUG`;
- `GW.cpp:273`: `M->MarkToDelete()` in the null branch.

The local source was checked at the pinned working tree `c38eb08` and shows the complete function at lines 219–275. Therefore the earlier shorthand range `214–270` is insufficient: it omits line 273 and includes unrelated `DeleteAction()` lines.

## 2. Proposed scope

One production file and one function only:

- `Common/src/GW.cpp::GW::PushToInputQueue(Message*)`, current lines 219–275.

Permitted production edit regions, subject to fresh Astra approval and user approval:

- lines 221–233: reject `M == nullptr` before any debug dereference;
- lines 270–274: replace the unsafe null branch with a diagnostic-only, non-dereferencing return path.

The following must not change:

- function signature (`void`);
- non-null command-line counting;
- `NoCL > 2` queue insertion, tag assignment, mutex scope, counter increment and notification;
- `NoCL <= 2` marking behavior;
- existing error diagnostic meaning, except for removing the invalid dereference;
- all other production files and caller sites.

## 3. Required contract

For `M == nullptr`, `PushToInputQueue()` must:

1. return normally without dereferencing `M`;
2. emit the existing corruption diagnostic or an equivalent diagnostic that does not dereference `M`;
3. perform no message-method call, queue insertion, tag increment, notification, or `MarkToDelete()` call;
4. leave queue state, queue tag/counters and any retained message unchanged.

For `M != nullptr`, behavior must remain semantically identical, including debug output, command count handling, marking, queue synchronization and notification.

This contract protects the queue boundary only. It does not prove that an upstream allocation failure is handled safely, nor does it repair dangling pointers or `Process::NewMessage()` output semantics.

## 4. Caller classification required before implementation

The repository-wide `PushToInputQueue()` search found many call expressions across Common, PGCS, NRNCS and ContentApp. A complete semantic matrix is still required before production approval. The matrix must exclude declarations, definitions, comments and unrelated overloads and record, for each real call:

- exact function and line;
- argument provenance and whether it can be null/non-null;
- status check and all preceding uses;
- ownership/alias relationship;
- downstream queue/consumer behavior;
- classification: `failure-safe`, `invariant-protected`, `blocker`, or `unresolved`.

B2 must not be expanded to all callers merely because a static search finds them. Direct null-boundary hardening and upstream allocation-failure fixes remain separate decisions.

## 5. Test-only gate before production

The test-only fixture and runner must be frozen and executed against unchanged `c38eb08` before implementation. No private-access hack, friend declaration, production callback, header change or API change is authorised by this SPEC.

Required cases:

1. `nullptr` input with `DEBUG` disabled: baseline must produce attributable RED evidence at the null branch.
2. `nullptr` input with `DEBUG` enabled: baseline must produce attributable RED evidence at the pre-check debug dereference.
3. Non-null message with more than two command lines: successful enqueue and notification behavior preserved.
4. Non-null message with at most two command lines: existing marking behavior preserved.
5. Command-count failure only if a public, supported construction path can produce it without corrupting private state.
6. External watchdog, sanitizer attribution and cleanup; timeout is inconclusive, not a pass.
7. Publicly observable queue/tag/retained-message state remains unchanged for null input. If this requires private access, a new seam or API, stop and request a separate SPEC/review.

A direct public invocation of `PushToInputQueue(nullptr)` is sufficient to characterize the boundary. It does not establish that a particular production caller reaches the boundary after `NewMessage()` failure.

## 6. Current test-only characterization

Against unchanged production at `c38eb08`, using the public `Process`/`GW` lifecycle:

- `null`, non-`DEBUG`: 3/3 baseline RED, normalized `rc=139`; ASAN reaches `GW.cpp:273` and `Message.cpp:453`.
- `null-debug`: 3/3 baseline RED, normalized `rc=139`; ASAN reaches `GW.cpp:226`.
- `non-null-many` (three command lines): 3/3 PASS, `rc=0`.
- `non-null-few` (one command line): 3/3 PASS, `rc=0`.
- `non-null-many-debug`: 3/3 PASS, `rc=0`.
- `non-null-few-debug`: 3/3 PASS, `rc=0`.
- External cleanup reported `cleanup_ok=1` for all 18 runner trials, with no residual B2 shared-memory segment in the reserved range.

The complete test-only files and logs are retained in:

`<workspace-root>/ng-spec033-characterization-20260909/`

- fixture: `gw_b2_input_queue.cpp`;
- runner: `run-gw-b2-fixture.py`;
- mechanical inventory: `NG-020-B2-caller-inventory.md`;
- inventory generation log: `NG-020-B2-caller-inventory.log`.

The runner now records per-trial logs, supports explicit `baseline`/`post-fix` expectations, normalizes Python signal return `-11` to the contract value `139`, kills timed-out children, decodes partial timeout output, and verifies that no reserved-key shared-memory resource remains after cleanup. It still cannot observe private queue depth/tag/notification state without a new seam; that limitation remains open and is not silently treated as acceptance.

The first overload of `Process::NewMessage(double, short, bool, Message*&)` at `Common/src/Process.cpp:542–577` initializes `Status` to `ERROR` but does not initialize `M` before searching for a free slot. On capacity failure, `M` therefore retains its incoming value. The other file-based overloads explicitly set `M = NULL` before allocation. This is an upstream contract issue and is outside B2; callers that ignore status may pass a stale or indeterminate pointer to the queue, while B2 can only protect the boundary once control reaches `PushToInputQueue()`.

The mechanical inventory contains 69 production call expressions after excluding comments and the GW definition. It is triage only: it does not classify ownership, nullability or allocator invariants.

## 7. Astra and implementation gates

In order:

1. Complete and revision-pin the caller matrix and exact allowlist.
2. Establish fixture feasibility using public interfaces only.
3. Produce test-only RED and non-null controls on unchanged production.
4. Send the complete current source, SPEC text, matrix and test evidence to GPT-6 Astra with repository URL `https://github.com/antonioalberti/novagenesis`.
5. Obtain a fresh conditional `GO` for this exact B2 group.
6. Obtain explicit user approval for the exact production allowlist.
7. Implement only the approved regions of `GW.cpp`.
8. Run the identical fixture/runner RED→GREEN, including both DEBUG configurations and sanitizer checks.
9. Run the full build.
10. Run matched Alpine regression after clean VM stop/start, with verified revision/build identity and required runtime evidence.
11. Obtain post-implementation Astra acceptance for B2. Keep B3 and C blocked.

Current status: this SPEC is implemented for the B2 local boundary and accepted by Astra. The unresolved callers and broader allocator/ownership contract remain outside scope and do not authorize further production changes.

## 8. Implementation and local acceptance — 2026-09-10

The approved null guard was implemented only in `Common/src/GW.cpp::GW::PushToInputQueue(Message*)` within the allowlist above. The non-null body and signature were preserved.

Post-change evidence against the pinned baseline:

- 18/18 normal runner trials passed: null and non-null controls in DEBUG and non-DEBUG, 3/3 per mode.
- 6/6 ASAN null trials passed: 3/3 DEBUG and 3/3 non-DEBUG, no sanitizer report.
- Full local CMake build passed; only pre-existing warnings were reported.
- `git diff --check` passed.
- Per-trial cleanup passed for the exact current IPC keys.

GPT-6 Astra verdict: `ACCEPT B2 — local boundary acceptance only`.

This acceptance covers only the null boundary of `PushToInputQueue()`. It does not accept the unresolved callers, the CLI upstream failure path, `Process::NewMessage()` output semantics, B3, C, or broader allocator/ownership safety. No Alpine runtime gate is imposed by this B2 SPEC; B1's separate Alpine evidence is not transferred to B2.

The implementation commit is recorded by Git after this SPEC update. B2 remains a subtarefa of NG-020; B3 and C remain blocked.

## 7. Rollback

Rollback is a revert of the single B2 implementation commit on `AIOPT3`; no reset or force-push. B1 commit `e086868` must remain intact.
