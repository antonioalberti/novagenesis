# SPEC-033-R3-C1: caller failure handling for reused outputs

**Author:** Antonio Alberti / Hermes Agent  
**Date:** 2026-09-10  
**Status:** Proposal — review only; no production authorization  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Parent:** `SPEC-033-R3-C-process-output-safety.md`  
**Related:** `SPEC-033-R3-caller-safety-amendment.md`

## 1. Purpose

Resolve three concrete caller-compatibility risks identified by GPT-6 Astra before any producer-only change to the first `Process::NewMessage()` overload.

The first overload currently preserves its incoming output pointer on capacity failure. A producer-only change that initializes `M = NULL` would change the input observed by later builders, senders and queue calls in these reused-output loops.

This SPEC is a caller amendment proposal. It does not authorize changes to `Process.cpp`, the first overload, other callers, headers, queue internals or exception policy.

## 2. Exact proposed caller scope

Only these three functions/files:

1. `PGCS/src/PGRunPublishing01.cpp::PGRunPublishing01::Run()` — call at current line 122, output `Publish`.
2. `PGCS/src/PGRunHello02.cpp::PGRunHello02::Run()` — call at current line 104, output `PGIHCHello`.
3. `PGCS/src/PGHelloIHC03.cpp::PGHelloIHC03::Run()` — call at current line 209, output `StoreBind01Msg`.

No other caller is included by implication or naming similarity.

## 3. Confirmed compatibility risks

### 3.1 PGRunPublishing01

`Publish` is declared once before the `PSTuples` loop. A successful iteration builds and submits the message, but does not reset or reclaim the output variable. A later capacity failure currently preserves the previous pointer; clearing it in the producer would send `NULL` to subsequent builders and `PushToInputQueue()`.

### 3.2 PGRunHello02

`PGIHCHello` is declared once before the stack loop. A successful iteration builds, sends and marks the message. The pointer is not reset before the next allocation. A later capacity failure can preserve the previous, possibly marked pointer; clearing it in the producer would change subsequent builder/send/mark inputs to `NULL`.

### 3.3 PGHelloIHC03

`StoreBind01Msg` is declared once before the compatible-peer loop. A successful iteration builds multiple bindings and submits the message. No reset/reclaim follows. A later capacity failure can preserve the previously submitted pointer; producer clearing would change subsequent builder inputs to `NULL`.

Astra classified all three as concrete compatibility risks, while not claiming a specific downstream crash without helper/ownership evidence.

## 4. Required failure behavior to decide

For each of the three loops, a failed first-overload allocation must bypass every message-dependent operation for that iteration, including builders, name generation, serialization, sending, queue submission and message marking.

GPT-6 Astra selected the conservative policy for all three functions: **return `ERROR` immediately after allocation failure**. This avoids reporting success after incomplete work, preserves earlier successful effects without rollback, and prevents subsequent operations from consuming a null or stale output. No failed output may be deleted, marked, unmarked or resubmitted.

The detailed test-only manifest is:

`Docs/DECISIONS/NG-020-C1-test-only-manifest-20260910.md`

The policy and exact allowlist remain proposals until the test-only evidence is produced and reviewed.
## 5. Required evidence before implementation

- Full current functions and relevant helper contracts for all three paths.
- Test-only capacity-exhaustion fixtures that reproduce success-then-failure in the same loop.
- Assertions for no builder/send/queue/mark operation on failed allocation.
- Assertions that previously queued/sent messages remain valid and are not deleted by the failed iteration.
- Explicit decision for skip-versus-return behavior in each function.
- Fresh Astra review of this SPEC and exact allowlist.
- Explicit user approval of the caller allowlist before production edits.

## 6. Relationship with producer-only C

The producer-only C proposal remains blocked. It must not be implemented before this caller compatibility issue is resolved or explicitly accepted as a changed contract.

C1 does not authorize the producer change. Conversely, C1 does not authorize unrelated repairs to the first overload or the remaining unresolved callers.

## 7. Current disposition

`NO-GO` for production. This document records the smallest caller group identified as incompatible with an unqualified producer-only output-clearing change. B1 and B2 remain accepted independently; B3 remains closed as no defect demonstrated.
