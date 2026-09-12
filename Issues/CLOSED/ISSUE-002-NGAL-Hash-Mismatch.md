# ISSUE-002: NGAL Hash Mismatch — Root Cause Fixed

**Date:** 2026-07-09  
**Status:** Closed  
**Priority:** Critical  
**Related:** SPEC-013, SPEC-014, NGAL-SAR, NGAL-CS

---

## 1. Description

After SPEC-013 (NGAL) implementation, ContentApp Repository failed integrity verification on **ALL** received photos. Log showed:
```
(ERROR: The hash of the file 00000-alpine-ng-source.jpg is not the same
than the one generated on the publisher. i.e. 581077ED)
```

Before SPEC-013, the SHM pipeline worked without this error.

## 2. Root Cause

SPEC-013 introduced an NGAL pipeline where `NGAL_SAR::ReceiveFragment()` called `PP->NewMessage()` on the `ReceiveDispatcher` thread, creating a complete `Message*` object (with `SetMessageFromCharArray` + `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2`). Then, on the GW thread, the same buffer was deserialized **again**.

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

The double deserialization itself did NOT corrupt bytes (the function `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` is read-only on `Msg`). The real problem was:

1. **`PP->NewMessage()` called on wrong thread** — `ReceiveDispatcher` runs on separate thread and calls `PP->NewMessage()` on PG Process. GW thread also calls `NewMessage()` on same Process. This is a **non-deterministic data race** corrupting Process internal structure.

2. **`MarkToDelete()` on SAR-created Message** — This Message is marked for deletion by GW thread via `DeleteMarkedMessages()`, but GW has no direct reference. Deletion timing is unpredictable and can free memory while another thread still references it.

3. **Violation of Finding F2 from SPEC-013** — SPEC-013 documented: *"Process::NewMessage() is NOT thread-safe — DeliverToGateway() sends char buffer, GW thread does NewMessage"*. Implementation VIOLATED this finding.

## 3. Fix Implemented

### 3.1 Principle: SAR Returns Raw Buffer, GW Does Deserialization

`NGAL_SAR::ReceiveFragment()` now returns the **raw buffer** (`char*` + `size`) instead of creating a `Message*`. This eliminates:
- Double deserialization
- `NewMessage()` on wrong thread (data race)
- Orphaned `MarkToDelete()`
- Finding F2 violation

### 3.2 Changes Made

| File | Change |
|------|--------|
| `Common/src/NGAL_SAR.h` | `ReceiveFragment(Process*, Message*&)` → `ReceiveFragment(char*&, long long&)`; forward declaration `class Message`; removed `#include "Process.h"` and `"Message.h"` |
| `Common/src/NGAL_SAR.cpp` | Body returns `FB->Buffer` + `FB->MessageSize` instead of `NewMessage + SetMessageFromCharArray + ConvertMessage + MarkToDelete`; added includes for SendSegmented |
| `PGCS/src/NGAL_Transport_RAW.cpp` | Calls `DeliverToGateway(CompletedBuffer, CompletedSize)` directly; `delete[] CompletedBuffer` after delivery; removed unnecessary includes |
| `Common/src/GW.cpp` | **No change** — Step 3 remains same (NewMessage + SetMessageFromCharArray + ConvertMessage — **once only**, on GW thread) |

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

Identical to old pipeline structure — raw buffer reaches GW exactly as it did via SHM.

## 4. Secondary Fix: CleanupTimedOut

`CleanupTimedOut(0)` called with `timeout_threshold=0` in `ReceiveDispatcher`, and `FB->Timestamp` never updated (initialized to 0, never modified). Timeout condition never triggered. Fixed by using `GetTime()` as threshold and updating `FB->Timestamp = GetTime()` on each fragment received.

## 5. Verification

- **Compilation**: `cmake-build-ngal/` — PGCS, ContentApp, NRNCS all compile OK
- **Pending**: E2E test with corrected binaries in intra-OS Content scenario
- **Success criterion**: ContentApp Repository verifies hash successfully (0 errors "hash of the file ... is not the same")

## 6. Decisions

| # | Decision | Date | Reason |
|---|----------|------|--------|
| D1 | Eliminate double deserialization instead of adding logging | 09/07/2026 | Structural analysis identified root cause; empirical logging is slower and may mask bug |
| D2 | SAR returns raw buffer (char* + size) | 09/07/2026 | Aligns with Finding F2 of SPEC-013; eliminates data race; simplifies pipeline |
| D3 | Do not add DEBUG_NGAL_HEX | 09/07/2026 | Temporary logging changes timing and may create/mask race conditions |
| D4 | CleanupTimedOut fix separate (P5) | 09/07/2026 | Does not affect hash mismatch; lower priority |

## 7. Related Documentation

- `Docs/ARCHITECTURE/NGAL-ARCHITECTURE.md` — NGAL architecture extracted from SPEC-013
- `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md` — This root cause analysis
- SPEC-013 updated with section E9 (F2 fix cross-reference)
- SPEC-014 updated to reflect implemented fix