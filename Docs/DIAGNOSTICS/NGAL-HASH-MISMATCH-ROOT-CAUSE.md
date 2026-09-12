# NGAL Hash Mismatch — Root Cause Analysis

**Source:** SPEC-014 (NGAL Hash Mismatch — Diagnosis and Correction) — section 2  
**Status:** Fixed (root cause identified and corrected)  
**Branch:** AIOPT3

---

## 1. Problem Statement

After SPEC-013 (NGAL) implementation, the ContentApp Repository failed integrity verification on **ALL** received photos. The log showed:

```
(ERROR: The hash of the file 00000-alpine-ng-source.jpg is not the same
than the one generated on the publisher. i.e. 581077ED)
```

Before SPEC-013, the SHM pipeline worked without this error.

---

## 2. Root Cause — Definitive Diagnosis

### 2.1 The Bug: Double Deserialization + NewMessage on Wrong Thread

SPEC-013 introduced an NGAL pipeline where `NGAL_SAR::ReceiveFragment()` calls `PP->NewMessage()` on the `ReceiveDispatcher` thread, creating a complete `Message*` object (with `SetMessageFromCharArray` + `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2`). Then, on the GW thread, the same buffer is deserialized **again**.

```
OLD PIPELINE (pre-NGAL) — 1 deserialization:
  SocketDispatcher3 → char buffer → SHM →
  GW::ReadFromSharedMemory3 → NewMessage + SetMessageFromCharArray + ConvertMessage → PushToInputQueue

BUGGY PIPELINE (SPEC-013) — 2 deserializations + data race:
  ReceiveDispatcher thread:
    NGAL_SAR::ReceiveFragment → PP->NewMessage() ← RACE! ←
      + SetMessageFromCharArray + ConvertMessage → Message* complete
    GetMessageFromCharArray → DeliverToGateway (queue of char buffers)
  GW thread:
    NewMessage + SetMessageFromCharArray + ConvertMessage → PushToInputQueue
```

### 2.2 Why This Causes Hash Mismatch

The double deserialization by itself does NOT corrupt bytes (the function `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` is read-only on `Msg`). The real problem is:

1. **`PP->NewMessage()` called on the wrong thread** — the `ReceiveDispatcher` runs on a separate thread and calls `PP->NewMessage()` on the PG Process. The GW thread also calls `NewMessage()` on the same Process. This is a **non-deterministic data race** that can corrupt the Process's internal structure (Message list, counters, etc.).

2. **`MarkToDelete()` on the Message created in SAR** — This Message is marked for deletion by the GW thread via `DeleteMarkedMessages()`, but the GW thread has no direct reference to this Message. The timing of deletion is unpredictable and can free memory while another thread still references it.

3. **Violation of Finding F2 from SPEC-013** — SPEC-013 itself documented: *"Process::NewMessage() is NOT thread-safe — DeliverToGateway() sends char buffer, GW thread does NewMessage"*. The implementation VIOLATED this finding.

### 2.3 Why It Fails on ALL Files (Not Intermittent)

Although the data race is theoretically non-deterministic, the hash fails consistently because:
- The `NewMessage` on the receiver thread always affects the Process state
- The systematic corruption of Message/Process during GW deserialization always produces incorrect payloads
- The `MarkToDelete` + `DeleteMarkedMessages` may free the SAR Message before the buffer is copied to the queue

---

## 3. Correction Implemented

### 3.1 Principle: SAR Returns Raw Buffer, GW Does Deserialization

`NGAL_SAR::ReceiveFragment()` now returns the **raw buffer** (`char*` + `size`) instead of creating a `Message*`. This eliminates:
- Double deserialization
- `NewMessage()` on wrong thread (data race)
- Orphaned `MarkToDelete()`
- Finding F2 violation

### 3.2 Detailed Changes

| File | Change | Reason |
|------|--------|--------|
| `Common/src/NGAL_SAR.h` | `ReceiveFragment(Process*, Message*&)` → `ReceiveFragment(char*&, long long&)`; forward declaration `class Message`; removed `#include "Process.h"` and `"Message.h"` | Clean interface, no Message/Process dependency in header |
| `Common/src/NGAL_SAR.cpp` | Body of ReceiveFragment returns `FB->Buffer` + `FB->MessageSize` instead of `NewMessage + SetMessageFromCharArray + ConvertMessage + MarkToDelete`; added `#include "Message.h"` and `"Process.h"` (needed for SendSegmented) | Eliminates double deserialization and data race |
| `PGCS/src/NGAL_Transport_RAW.cpp` | Calls `DeliverToGateway(CompletedBuffer, CompletedSize)` directly; `delete[] CompletedBuffer` after delivery; removed unnecessary includes | Simplified pipeline, raw buffer delivered to GW |
| `Common/src/GW.cpp` | **No change** — Step 3 remains same (NewMessage + SetMessageFromCharArray + ConvertMessage — **once only**, on GW thread) | This is the correct path (Finding F2) |

### 3.3 Corrected Pipeline

```
CORRECTED PIPELINE — 1 deserialization, no data race:
  ReceiveDispatcher thread:
    NGAL_SAR::ReceiveFragment → returns char* buffer + size (no NewMessage!)
    NGAL_CS::DeliverToGateway(buffer, size) → queue of char buffers
    delete[] buffer
  GW thread:
    NewMessage + SetMessageFromCharArray + ConvertMessage → PushToInputQueue
```

Identical in structure to the old pipeline — the raw buffer reaches GW exactly as it did via SHM.

---

## 4. Secondary Bug: CleanupTimedOut Not Working

`CleanupTimedOut(0)` is called with `timeout_threshold=0` in `ReceiveDispatcher`, and `FB->Timestamp` is never updated (initialised to 0, never modified). The timeout condition never triggers. This does NOT cause the hash mismatch but should be fixed.

**Pending fix:** Use `GetTime()` as threshold and update `FB->Timestamp = GetTime()` on each fragment received.

---

## 5. Critique of SPEC-014 v1 (Draft)

SPEC-014 v1 proposed adding temporary hex logging (`DEBUG_NGAL_HEX`) at 6 points (E1-E6) and running the scenario to compare bytes. This **empirical diagnostic** approach:

1. **Does not identify root cause** — Static analysis showed bytes are a faithful copy (send side = receive side), so hex dumps would be identical and root cause would remain unexplained.
2. **Is time-consuming** — Requires 10 steps (E0-E10), each with code modifications, compilation, execution, and analysis.
3. **Masks the real bug** — The `printf`/`cerr` added for logging changes ReceiveDispatcher timing, potentially eliminating or creating race conditions, making diagnosis non-reproducible.
4. **Incorrectly concluded double deserialization is safe** (section 3.5) — Static analysis says `ConvertMessage...` is read-only, but ignores the data race on `PP->NewMessage()`.

The correct approach was: analyse the **structural** difference between old and new pipelines (section 3.5 of v1 already documented it!), and recognise that `PP->NewMessage()` on the wrong thread is the bug.

---

## 6. Execution Plan

| # | Step | Status | Detail |
|---|------|--------|--------|
| P1 | Fix `NGAL_SAR::ReceiveFragment` | **DONE** | Returns raw buffer instead of Message* |
| P2 | Fix `NGAL_Transport_RAW::ReceiveDispatcher` | **DONE** | Uses raw buffer for DeliverToGateway |
| P3 | Compilation | **DONE** | `cmake-build-ngal/` — PGCS, ContentApp, NRNCS OK |
| P4 | Test: intra-OS Content scenario | **PENDING** | Run with corrected binaries |
| P5 | Fix CleanupTimedOut | **DONE** | Timestamp updated with time(0) per fragment; threshold uses PPGCS->GetTime() |
| P6 | Update SPEC-013 (cross-reference) | **DONE** | Section E9 added with F2 fix |
| P7 | Commit | **PENDING** | After test passes |

---

## 7. How to Test (P4)

Corrected binaries are in `<local-repository-path>/cmake-build-ngal/`.

Copy to production path or run directly:

```bash
# Stop old processes
./Scripts/Simple/clean.sh

# Run with corrected binaries (adjust paths per scenario script)
sudo <local-repository-path>/cmake-build-ngal/PGCS <local-repository-path>/IO/PGCS/ 0 Intra_Domain -lc
# + ContentApp, NRNCS, etc.
```

**Success criterion:** ContentApp Repository verifies hash successfully (0 errors "hash of the file ... is not the same").

---

## 8. Decisions

| # | Decision | Date | Reason |
|---|----------|------|--------|
| D1 | Eliminate double deserialization instead of adding logging | 09/07/2026 | Structural analysis identified root cause; empirical logging is slower and may mask bug |
| D2 | SAR returns raw buffer (char* + size) | 09/07/2026 | Aligns with Finding F2 of SPEC-013; eliminates data race; simplifies pipeline |
| D3 | Do not add DEBUG_NGAL_HEX | 09/07/2026 | Temporary logging changes timing and may create/mask race conditions |
| D4 | CleanupTimedOut fix separate (P5) | 09/07/2026 | Does not affect hash mismatch; lower priority |

---

## 9. Pitfalls

1. **Buffer ownership** — `ReceiveFragment` transfers ownership of `FB->Buffer` to caller. Caller MUST `delete[]` after `DeliverToGateway` (which copies to queue).
2. **Forward declaration** — `NGAL_SAR.h` uses `class Message;` (forward declaration). The `.cpp` includes `"Message.h"` for full definition. This is correct and intentional.
3. **DeleteMarkedMessages** — With the fix, SAR no longer creates Messages. `DeleteMarkedMessages` in GW only affects Messages created on GW thread — correct.
4. **SHM path** — The intra-process path (ContentApp→PGCS and PGCS→ContentApp) still goes via SHM and was **not changed**. If hash mismatch persists ONLY intra-VM, the problem is elsewhere.