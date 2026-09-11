# SPEC-020: Subscription Re-Delivery Bug — Root Cause and Correction

**Date:** 2026-07-11  
**Status:** Implemented  
**Author:** Hermes Agent  
**Related:** SPEC-014 (data race), SPEC-018 (ResetPayload), SPEC-019 (hash logging)

---

## 1. Problem

The ContentApp Repository receives the payload correctly (hash matches) but the subscription is never updated to "Delivered", causing infinite re-subscription every ~60s.

**Repo61 log:**
```
(RTT from NRNCS was 0.0102803020 seconds for the key 789703CA)
(ContentApp received payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
(Subscription 0 has Status Waiting delivery, Key = 789703CA, HasContent = 1, Time from subscription = 27.3s)
--- 60s later ---
(RTT from NRNCS was 87.3206969120 seconds for the key 789703CA)
(ContentApp received payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
(Subscription 0 has Status Waiting delivery, Key = 789703CA, HasContent = 1, Time from subscription = 147.3s)
```

The RTT increases by 60s each time — the subscription is being re-submitted on a periodic timer.

## 2. Root Cause

### 2.1 Three actors in subscription lifecycle

| Actor | File | Function |
|-------|------|----------|
| **CoreDeliveryBind01** | `ContentApp/src/CoreDeliveryBind01.cpp` | Processes `-d --b` → marks `HasContent = true` |
| **CoreInfoPayload01** | `ContentApp/src/CoreInfoPayload01.cpp` | Processes `-info --payload` → saves payload + marks "Delivered" |
| **CoreRunPeriodic01** | `ContentApp/src/CoreRunPeriodic01.cpp` | Periodic timer: re-submits subscriptions still "Waiting delivery" |

### 2.2 Failure cycle

```
1. Initialization:
   Subscription[0].Status = "Waiting delivery"
   Subscription[0].HasContent = false

2. CoreDeliveryBind01 processes "-d --b" (delivery announced):
   Subscription[0].HasContent = true    ← BEFORE payload arrives

3. CoreInfoPayload01 processes "-info --payload" (payload received):
   if (PS->Status == "Waiting delivery" && !PS->HasContent)  ← OLD CONDITION
   → FAILS because HasContent is already true
   → Subscription[0].Status remains "Waiting delivery"

4. CoreRunPeriodic01 (timer ~60s):
   if ((GetTime() - PS->Timestamp) > TIMEOUT && PS->Status == "Waiting delivery")
   → Re-submits subscription to NRNCS

5. NRNCS re-delivers same payload
   → CoreInfoPayload01 receives again
   → Same condition fails
   → Infinite loop
```

### 2.3 Why RTT increases by 60s each time

Each re-subscription goes through NRNCS which keeps the payload cached. The reported RTT is the time since the original subscription, not since the re-subscription. The 60s increment is the `TIMEOUT` interval in `CoreRunPeriodic01` (line 295).

## 3. Correction (SPEC-020)

### 3.1 Remove the `!PS->HasContent` guard

`CoreDeliveryBind01` marks `HasContent = true` before `CoreInfoPayload01` processes the payload. Therefore, `!PS->HasContent` is always `false` when `CoreInfoPayload01` runs, blocking the update.

**Old condition (BUG):**
```cpp
if (PS->Status == "Waiting delivery" && !PS->HasContent)
```

**Correct condition (SPEC-020):**
```cpp
if (PS->Status == "Waiting delivery")
```

### 3.2 Keep "Processing required" — don't force "Delivered"

`CoreRunEvaluate01` (line 501) depends on `Status == "Processing required"` to process the acceptance. If SPEC-020 forces `Status = "Delivered"` immediately, the acceptance is never generated.

**Correct fix:**
```cpp
if (PS->Status == "Waiting delivery")
{
    PS->Status = "Processing required";   // CoreRunEvaluate01 needs this
    PS->FileName = Values.at(0);
    break;  // SPEC-017: only one subscription per message
}
```

**Don't do:**
```cpp
PS->HasContent = true;    // CoreDeliveryBind01 already did this
PS->Status = "Delivered"; // ❌ Kills the acceptance!
```

### 3.3 Why no re-delivery without "Delivered"

`CoreRunPeriodic01` (line 295) only re-submits if:
```cpp
if ((GetTime() - PS->Timestamp) > TIMEOUT && PS->Status == "Waiting delivery")
```

With `Status == "Processing required"`, the condition is **not** satisfied. `CoreRunPeriodic01` never re-submits a subscription already being processed. "Delivered" is unnecessary to prevent re-delivery.

### 3.4 Complete cycle

```
Waiting delivery ──CoreInfoPayload01──→ Processing required ──CoreRunEvaluate01──→ Delete
    ↑                    (condition fix)        │                                      │
CoreDeliveryBind01                              │ (creates acceptance,                  │
(arranges HasContent)                           │  publishes response)                 │
                                                └── acceptance → Source ──→ contentpublish
```

## 4. Affected Files

| File | Current code | Action |
|------|--------------|--------|
| `ContentApp/src/CoreInfoPayload01.cpp` | `if (PS->Status == "Waiting delivery")` ✅ | Fix already applied locally |

## 5. Verification

1. Compile: `cd build && make -j$(nproc)`
2. Deploy to repository guest and source guest
3. Start NRNCS + PGCS + ContentApp on both VMs
4. Check Repo log: **ZERO** occurrences of `(The following message contains a subscription of delayed deliveries)`
5. Verify each subscription shows `Status = Delivered` after first delivery
6. Test with `--publish 0.1` — verify 100/100 files without re-delivery

## 6. Note: PGCS

`PGCS/src/CoreInfoPayload01.cpp` has condition `PS->Status == "Waiting delivery" && PS->HasContent == true` — different from ContentApp bug. PGCS does **not** have re-delivery because:
- The `CoreRunPeriodic01` that re-submits subscriptions is in ContentApp, not PGCS
- PGCS is a relay; content subscriptions are managed by ContentApp

Therefore, PGCS does not need SPEC-020 correction.

## 7. Decisions

| # | Decision | Date | Reason |
|---|----------|------|--------|
| D1 | Remove `!PS->HasContent` instead of reordering CoreDeliveryBind01 | 2026-07-11 | `HasContent` is semantically correct as "delivery announced" signal; the correct condition is to check only Status |
| D2 | Keep "Processing required" + "Delivered" (double assignment) | 2026-07-11 | "Processing required" used by other code checking subscription state; "Delivered" is official terminal state |