# SPEC-017: Content-Name Correlation Bug — CoreInfoPayload01 Subscription Update Loop

**Date:** 2026-07-09  
**Status:** Superseded by SPEC-021 — Fix proposed, never implemented  
**Author:** Hermes Agent  
**Related:** SPEC-018 (ResetPayload), SPEC-020 (subscription re-delivery)

---

> **NOTICE:** The fix proposed in this SPEC was never implemented. The root cause (subscription loop without break) was addressed differently in SPEC-020 and SPEC-021. This SPEC is kept for historical reference.

## 1. Problem

After the fix in `CoreInfoPayload01` (removing the call to `ExtractPayloadCharArrayFromMessageCharArray()`), the hash mismatch persists in ~28/100 files. The fix confirmed that in at least 1 case (`00091-alpine-ng-source.jpg`) the binary content received is correct but was saved with the wrong name. This is **RC3 (Root Cause 3)**: content↔filename correlation bug.

## 2. Root Cause

### 2.1 The loop without `break`

In `CoreInfoPayload01::Run()` (ContentApp and PGCS), the code that updates subscriptions iterates **all** subscriptions and assigns the same filename to all that have `HasContent=true`:

```cpp
// ContentApp/src/CoreInfoPayload01.cpp:100-131
for (unsigned int i = 0; i < PCore->Subscriptions.size(); i++)
{
    Subscription* PS = PCore->Subscriptions[i];

    if (PS->Status == "Waiting delivery" && PS->HasContent)
    {
        PS->Status = "Processing required";
        PS->FileName = Values.at(0);  // <-- BUG: mesmo filename para TODAS
        // <-- falta break aqui!
    }
}
```

### 2.2 Failure scenario

1. Two deliveries (`-info --payload`) arrive in separate messages, one for file `00022` and another for `00091`
2. `CoreDeliveryBind01` processes two `-d --b` and sets `HasContent=true` on subscriptions for **both** files
3. `CoreInfoPayload01` processes the `-info --payload` for `00022`:
   - Saves payload to `00022-alpine-ng-source.jpg` ✓
   - Iterates subscriptions → finds BOTH with `HasContent=true`
   - Assigns `FileName = "00022-alpine-ng-source.jpg"` to **both** subscriptions ✗
4. `CoreInfoPayload01` processes the `-info --payload` for `00091`:
   - Saves payload to `00091-alpine-ng-source.jpg` (overwrites `00022`!)
   - Assigns `FileName = "00091-alpine-ng-source.jpg"` to **both** ✗
5. `CoreRunEvaluate01` checks hash of `00022` → reads `00091-alpine-ng-source.jpg` (which has `00091` content) → hash mismatch
6. Same for `00091` → if it reads the same file, hash also doesn't match

### 2.3 Why only some files affected

The bug only occurs when two or more deliveries have `HasContent` set before `CoreInfoPayload01` processes the corresponding `-info --payload`. This depends on the arrival order of `-info --payload` vs `-d --b` messages, which varies with NRNCS network timing.

---

## 3. Proposed Correction

### 3.1 Add `break` after updating the first subscription

Each `-info --payload` message corresponds to **exactly one** file delivery. The loop should update only **one** subscription and exit.

**File:** `ContentApp/src/CoreInfoPayload01.cpp` (line 121)  
**File:** `PGCS/src/CoreInfoPayload01.cpp` (line 111)

```cpp
if (PS->Status == "Waiting delivery" && PS->HasContent)
{
    PS->Status = "Processing required";
    PS->FileName = Values.at(0);
    break;  // SPEC-017: Only one subscription per -info --payload message
}
```

### 3.2 Justification

- Subscription order in vector is creation order (first to subscribe = first in vector)
- `CoreDeliveryBind01` processes `-d --b` in arrival order and marks `HasContent=true` on the corresponding subscription
- The first subscription with `HasContent=true` is the one matching the current `-info --payload` message
- The `break` ensures each `-info --payload` updates exactly one subscription

### 3.3 Safety

Even if subscription order doesn't match delivery order (rare case), the worst case is one subscription stays in "Waiting delivery" without being updated — instead of TWO subscriptions getting wrong filenames. The NRNCS timeout will re-deliver the missing content.

---

## 4. Affected Files

| File | Line | Change |
|------|------|--------|
| `ContentApp/src/CoreInfoPayload01.cpp` | 121 | Add `break;` after `PS->FileName = Values.at(0);` |
| `PGCS/src/CoreInfoPayload01.cpp` | 111 | Add `break;` after `PS->FileName = Values.at(0);` |

---

## 5. Verification

1. Apply correction (add `break` in both files)
2. Compilar: `cd cmake-build-debug && make -j$(nproc)`
3. Testar com `--publish 0.1`
4. Verificar ZERO erros "hash of the file ... is not the same"
5. Opcional: copiar ficheiros recebidos e comparar com originais usando o script de hash NG