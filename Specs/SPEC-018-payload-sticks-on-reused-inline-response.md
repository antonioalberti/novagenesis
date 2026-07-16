# SPEC-018: SetPayloadFromCharArray One-Shot Guard — Payload Sticks on Reused InlineResponseMessage

**Date:** 2026-07-10  
**Status:** Superseded — Superseded by **SPEC-022-nrinfopayload01-separate-messages.md** (cache model)  
**Author:** Hermes Agent  
**Related:** SPEC-018 (ResetPayload), SPEC-020 (subscription re-delivery)

---

> **NOTICE:** This SPEC describes the **forwarding model** for NRInfoPayload01 which has been superseded by the **cache model** in SPEC-022 (canonical). The NRInfoPayload01 no longer forwards payloads — it caches them to disk. See SPEC-022 for the current implementation.

---

## 1. Problem

After the fixes in `CoreInfoPayload01` (removing `ExtractPayloadCharArrayFromMessageCharArray()` call) and SPEC-017, the hash mismatch persists in 17/100 files. All 17 files received in the ContentApp Repository contain identical bytes to file `00019-alpine-ng-source.jpg`, regardless of the filename. The NRNCS has all 100 files stored correctly (verified by hash), eliminating the storage path as cause.

## 2. Experimental Diagnosis

### 2.1 Hash verification

The NG hash function (`MurmurHash3_x86_32`, seed=3571, output big-endian 8 hex chars) was ported to Python and verified:

- Source1 (100 original files): 100/100 hashes match publisher hashes in log
- NRNCS (100 files in NRNCS): 100/100 content identical to Source1, 100/100 hashes correct
- Repository1 (99 ficheiros recebidos): 82 correctos, 17 com conteúdo idêntico a 00019

### 2.2 Padrão de falha

The 17 wrong files in Repository1 are all byte-for-byte identical to `00019-alpine-ng-source.jpg` (289342 bytes). The corresponding original files have different sizes (288871B to 289443B), confirming these are not truncations or corruptions — they are completely wrong content.

### 2.3 Elimination of previous causes

| Root Cause | SPEC | Status | Applied? |
|---|---|---|---|
| RC1 — ReceiveFragment data race | SPEC-014 | FIXED | Yes |
| RC2 — ExtractPayloadCharArrayFromMessageCharArray | (removed call) | FIXED | Yes (call commented out) |
| RC3 — Subscription loop without break | SPEC-017 | FIXED | Yes (break added) |
| **RC4 — SetPayloadFromCharArray one-shot guard** | **SPEC-018** | **New** | **No** |

## 3. Root Cause (RC4)

### 3.1 The one-shot guard

`Message::SetPayloadFromCharArray()` (Message.cpp:817-843) has a guard that prevents the payload from being replaced:

```cpp
int Message::SetPayloadFromCharArray(char* _Value, long long _Size)
{
  int Status = ERROR;

  if (PayloadSize == 0 && DeletePayloadArray == false)  // <-- GUARD
  {
    if (_Size > 0)
    {
      PayloadSize = _Size;
      Payload = new char[PayloadSize];
      // ... copy ...
      Status = OK;
      DeletePayloadArray = true;
      HasPayloadFlag = true;
    }
  }

  return Status;
}
```

The same guard exists in `ConvertPayloadFromFileToCharArray()` (Message.cpp:1171):
```cpp
if (PayloadSize == 0 && DeletePayloadArray == false && Status == OK && HasPayloadFlag == true)
```

### 3.2 How the InlineResponseMessage is reused

The content subscription flow is:

1. ContentApp Repository creates subscriptions: `ng -s --b 0.1 [ < 1 s 18 > < N s key1 key2 ... keyN > ]`
2. PSS receives the message. `PSSubBind01::Run()` converts each key into a `ng -g --b` command line **in the same InlineResponseMessage** (loop at lines 77-81)
3. The InlineResponseMessage goes to the PGCS HT. `Block::Run()` processes all `ng -g --b` CLs sequentially, passing the **same** `_InlineResponseMessage` to each call of `HTGetBind01::Run()`
4. `HTGetBind01::Run()` for Category 18 (lines 121-137):
   - `InlineResponseMessage->SetMessage(..., _Values->at(0), ...)` — sets filename
   - `InlineResponseMessage->ConvertPayloadFromFileToCharArray()` — loads file into payload
   - `PMB->NewInfoPayloadCommandLine(...)` — adds `-info --payload`

### 3.3 Failure scenario

With 19 keys in a single subscription:

1. `HTGetBind01` processes the 1st `ng -g --b` (e.g., key=A0638CC0, file=00003):
   - `SetMessage` sets PayloadFile = 00003-alpine-ng-source.jpg
   - `ConvertPayloadFromFileToCharArray` loads 00003 → PayloadSize > 0, DeletePayloadArray = true
   - Adds `-info --payload 0.1 [ < 1 s 00003-alpine-ng-source.jpg > ]`
   - **But this CL stays in the same message as the other 18 gets not yet processed**

2. `HTGetBind01` processes the 2nd `ng -g --b` (e.g., key=4AF7B897, file=00019):
   - `SetMessage` sets PayloadFile = 00019-alpine-ng-source.jpg
   - `ConvertPayloadFromFileToCharArray` **FAILS SILENTLY** because `PayloadSize != 0` (guard: `PayloadSize == 0 && DeletePayloadArray == false`)
   - The payload remains the content of 00003 (or first file processed)
   - Adds `-info --payload 0.1 [ < 1 s 00019-alpine-ng-source.jpg > ]`

3. The same happens for the remaining 17 keys: the filename is correct in the `-info --payload` CL, but the Payload char array always contains the first file's content.

4. When the InlineResponseMessage is serialized by `ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray()`, the single Payload is appended after all CLs. At the receiver, `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` extracts all CLs and then the payload (one single block). All `-info --payload` CLs reference the same payload.

5. The ContentApp processes each `-info --payload` sequentially. Each sets the filename and writes the same Payload to that file. Result: all files have the same content (the first one delivered).

### 3.4 Why 00019 is the "winning" content

The key processing order in the HT depends on the order of `ng -g --b` CLs, which in turn depends on the order in the `Key` vector in `PSSubBind01`. The first key processed whose file exists in the HT determines the payload that "sticks". The log shows that 00006 and 00019 were resolved in the first rounds, suggesting they were among the first processed. The content of 00019 is what appears in all 17 wrong files, indicating that 00019 was the first file whose payload was loaded successfully.

## 4. Correction

### 4.1 Reset payload before each SetPayloadFromCharArray / ConvertPayloadFromFileToCharArray

The correct solution is to ensure the payload is cleared before loading a new file. Add a `ResetPayload()` method that frees the existing payload and resets flags, and call it before each load.

**Simpler alternative:** Remove the one-shot guard from `SetPayloadFromCharArray` and `ConvertPayloadFromFileToCharArray`, allowing the payload to be replaced. But this may cause memory leak if the previous Payload is not freed.

**Adopted solution:** Add `ResetPayload()` and call it explicitly before each `SetPayloadFromCharArray` / `ConvertPayloadFromFileToCharArray` at the sites where the InlineResponseMessage is reused.

### 4.2 Affected files

| File | Change |
|---|---|
| `Common/src/Message.h` | Declare `void ResetPayload();` |
| `Common/src/Message.cpp` | Implement `ResetPayload()`; modify guards in `SetPayloadFromCharArray` and `ConvertPayloadFromFileToCharArray` |
| `Common/src/HTGetBind01.cpp` | Call `ResetPayload()` before `ConvertPayloadFromFileToCharArray()` |
| `NRNCS/src/NRInfoPayload01.cpp` | Call `ResetPayload()` before `SetPayloadFromCharArray()` |
| `PSS/src/PSInfoPayload01.cpp` | Call `ResetPayload()` before `SetPayloadFromCharArray()` |

## 5. Verification

1. Apply the correction
2. Compile: `cd cmake-build-debug && make -j$(nproc)`
3. Deploy to VMs 101/102
4. Test with `--publish 0.1`
5. Verify ZERO errors "hash of the file ... is not the same"
6. Verify all 100 received files have content identical to originals
