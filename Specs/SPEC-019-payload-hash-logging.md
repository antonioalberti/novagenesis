# SPEC-019: Payload Hash Logging for Traceability

**Date:** 2026-07-11  
**Status:** Implemented — ContentApp and NRNCS  
**Author:** Hermes Agent  
**Related:** SPEC-014 (data race), SPEC-017 (loop without break), SPEC-018 (ResetPayload), SPEC-020 (subscription re-delivery), **SPEC-022-nrinfopayload01-separate-messages.md** (cache model)

---

## 1. Objective

Add NG hash logging (MurmurHash3_x86_32, seed 3571) at every point where a payload is processed, to trace content integrity through the pipeline without post-hoc verification.

## 2. Payload Pipeline with Hash Logging

```ascii
Source (source guest)                    Repo (repository guest)
───────────────                    ──────────────
ContentApp Source
  └─ Publishes file (JPG, TXT)
       ↓ SHM via PGCS
NRNCS (Source)
  └─ NRInfoPayload01.cpp
     └─ (NRNCS cached payload: file=X, size=N, hash=H)  ← SPEC-019
       ↓ NGAL raw socket
NRNCS (Repo)
  └─ NRInfoPayload01.cpp
     └─ ResetPayload() + SetPayloadFromCharArray()       ← SPEC-018
       ↓ SHM via PGCS
ContentApp Repository
  └─ CoreInfoPayload01.cpp
     └─ (ContentApp received payload: file=X, size=N, hash=H)  ← SPEC-019
     └─ ConvertPayloadFromCharArrayToFile()
     └─ Marks subscription as "Delivered"                    ← SPEC-020
```

## 3. Logging Points

| Point | File | Status | Output |
|-------|------|--------|--------|
| NRNCS (cached) | `NRNCS/src/NRInfoPayload01.cpp` | ✅ Implemented | `(NRNCS cached payload: file=X, size=N, hash=H)` |
| ContentApp (received) | `ContentApp/src/CoreInfoPayload01.cpp` | ✅ Implemented | `(ContentApp received payload: file=X, size=N, hash=H)` |

## 4. Implementação

### 4.1 ContentApp (já implementado, linhas 104-123)

```cpp
// SPEC-019: Log payload hash at ContentApp for traceability
{
    string PayloadHash;
    File F1;
    F1.OpenInputFile(Values.at(0), PayloadPath, "BINARY");
    F1.seekg(0, ios::end);
    long long PayloadSize = F1.tellg();
    F1.seekg(0);
    if (PayloadSize > 0) {
        char* payload_bytes = new char[PayloadSize];
        F1.read(payload_bytes, PayloadSize);
        PayloadHash = NameGenerator::GetInstance().GenerateFromCharArray(payload_bytes, PayloadSize);
        delete[] payload_bytes;
    }
    F1.CloseFile();
    PB->S << Offset << "(ContentApp received payload: file=" << Values.at(0)
          << ", size=" << PayloadSize << " bytes, hash=" << PayloadHash << ")" << endl;
}
```

Note: Reads file from disk **after** `ConvertPayloadFromCharArrayToFile()`. This verifies what was actually written.

### 4.2 NRNCS (already implemented, lines 92-100)

```cpp
// SPEC-019: Log payload hash at NRNCS for traceability
{
    string PayloadHash;
    unsigned char* payload_bytes = (unsigned char*)Payload;
    PayloadHash = NameGenerator::GetInstance().GenerateFromCharArray(
        (const char*)payload_bytes, Size);
    PB->S << Offset << "(NRNCS cached payload: file=" << Values.at(0)
          << ", size=" << Size << " bytes, hash=" << PayloadHash << ")" << endl;
}
```

Note: Calculates hash of payload **in-memory** before copying to InlineResponseMessage. No I/O needed.

## 5. Log Verification

NRNCS and Repo logs confirm hash logging works:

**NRNCS (source guest):**
```
(NRNCS cached payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
```

**ContentApp (repository guest):**
```
(ContentApp received payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
```

Hash `789703CA` consistent in both — payload arrives intact at ContentApp.

## 6. Affected Files

| File | Action | Reason |
|------|--------|--------|
| `ContentApp/src/CoreInfoPayload01.cpp` | ✅ Already implemented | — |
| `NRNCS/src/NRInfoPayload01.cpp` | ✅ Already implemented | — |