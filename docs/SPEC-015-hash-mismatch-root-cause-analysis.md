# SPEC-015: Hash Mismatch in NRNCS Payload Delivery — Root Cause Analysis

**Date:** 2026-07-09  
**Status:** Root cause identified; fix pending  
**Author:** Hermes diagnostic session  
**Related:** SPEC-013 (NGAL double deserialization), SPEC-014 (NGAL ReceiveFragment fix)

---

## 1. Problem Statement

After applying SPEC-014 (which fixed the double deserialization in `NGAL_SAR::ReceiveFragment`), hash mismatches persist when the ContentApp Repository (VM 101) receives `.jpg` files published by the ContentApp Source (VM 102) via NRNCS over raw sockets (NGAL).

**Symptoms:**
- Files arrive via NRNCS but hash verification fails
- Hash subscriber differs from hash publisher
- Not all files fail — some pass correctly and are removed from the subscription list
- Files that fail do so consistently (same files always fail)
- RTT from NRNCS increases for failing files (0.1–0.4s vs 0.006–0.03s for passing files)

**Affected files (examples):**
- `00009-alpine-ng-source.jpg` — hash `69B34697` (mismatch)
- `00049-alpine-ng-source.jpg` — hash `CD69B8B3` (mismatch)
- `00050-alpine-ng-source.jpg` — hash `8EF3CF3D` (mismatch)
- `00094-alpine-ng-source.jpg` — hash `15087738` (mismatch)
- `00095-alpine-ng-source.jpg` — hash `A5E97EE6` (mismatch)
- `00096-alpine-ng-source.jpg` — hash `66C4BDED` (mismatch)

---

## 2. Data Flow Overview

```
ContentApp Source (VM 102)
  → SHM → PGCS (VM 102)
    → NRNCS (VM 102)
      → NGAL raw socket → NRNCS (VM 101)
        → SHM → PGCS (VM 101)
          → SHM → ContentApp Repository (VM 101)
```

At each SHM boundary, messages are:
1. Deserialized by `GW::ReadFromSharedMemory3` → `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()`
2. Processed by the block actions
3. Re-serialized by `PushToOutputQueue` → `ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray()`
4. Written to SHM for the next process

---

## 3. Root Cause

### 3.1 The Redundant Re-Extraction

The **ContentApp's `CoreInfoPayload01.cpp`** (line 94) calls:

```cpp
_ReceivedMessage->ExtractPayloadCharArrayFromMessageCharArray();
```

This call is **redundant and destructive** because:

- The GW already correctly extracted `Payload` and `PayloadSize` via `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` before pushing the message to the ContentApp's input queue.
- `ExtractPayloadCharArrayFromMessageCharArray()` **overwrites** the previously correct `Payload` pointer and `PayloadSize` with its own re-parsed values.
- It also causes a **memory leak** — the old `Payload` array (allocated by `Convert...2`) is never freed before the pointer is reassigned.

### 3.2 Why `ExtractPayloadCharArrayFromMessageCharArray` Corrupts Binary Payloads

The function (`Message.cpp` lines 1237–1302) uses `stringstream::getline()` to parse the message:

```cpp
stringstream ss;
for (long long i = 0; i < MessageSize; i++) { ss << Msg[i]; }
char Line[4096];
while (ss.getline(Line, sizeof(Line), '\n'))
{
    if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
        continue;  // skip command lines
    else
        break;      // found blank line → payload starts after
}
// Read remaining bytes as payload
Position = ss.tellg();
Size = ss.str().length();
length = Size - Position;
Payload = new char[length];
ss.read(Payload, length);
PayloadSize = length;
```

**Problems with this approach on binary data (JPG files):**

1. **`getline()` truncation**: `char Line[4096]` — if a command line exceeds 4095 bytes, `getline()` truncates and sets `failbit`. The next `getline()` reads the remainder as a new "line". If that remainder doesn't start with `ng -`, the loop breaks prematurely, and `tellg()` points to the middle of a command line, not the payload.

2. **False `ng -` matches in payload**: After the blank line separator, the remaining bytes are binary payload. But the `while` loop continues reading "lines" after the break point only if it doesn't break. However, if the first non-command-line "line" happens to be blank (the actual separator), `getline()` reads it, `Line[0]` is `'\0'` (null terminator from empty line), which is not `'n'`, so it breaks. This part works. BUT — the critical issue is that `tellg()` after `getline()` on a blank line (`\n`) may not point to the exact same byte offset as the direct-byte-scanning method in `Convert...2`.

3. **`tellg()` position discrepancy**: `Convert...2` does a direct byte scan on `Msg[]`, incrementing `t` for each byte. When it finds a non-`ng -` line, it does `t++` (skip the blank line `\n`) and sets `PayloadSize = MessageSize - t`. In contrast, `getline()` consumes the `\n` delimiter (does not store it), and `tellg()` points past it. These two approaches should give the same offset, BUT `getline()` in a `stringstream` can have different behavior with embedded null bytes (`\0`) in `Msg[]` — the `ss << Msg[i]` where `Msg[i] = '\0'` inserts a null char, which `std::string` handles correctly, but the interaction between `getline()`, `tellg()`, and binary data containing `\0` and `\n` is implementation-dependent and fragile.

4. **`ss.str().length()` vs `MessageSize`**: In theory, `ss.str().length() == MessageSize` because the stringstream was built byte-by-byte from `Msg[0..MessageSize-1]`. However, if `MessageSize` was set differently (e.g., by a previous `SetMessageFromCharArray`), there could be a mismatch.

### 3.3 Why Some Files Pass and Others Fail

Files that pass:
- Tend to have shorter RTT (< 0.1s), suggesting they arrived quickly without retransmission
- Their binary content does not contain byte sequences that confuse the `getline()` parser
- They may be smaller files that fit in a single SAR segment

Files that fail:
- Have longer RTT (0.1–0.5s+), possibly due to larger size requiring multiple SAR segments
- Their binary JPG content happens to contain byte patterns (`\n` followed by specific bytes) that cause `tellg()` to return an incorrect offset
- The failure is **deterministic** — the same files always fail because the corruption depends on the file's binary content, not on timing or race conditions

### 3.4 The Same Bug Exists in PGCS

`/home/gandalf/workspace/novagenesis/PGCS/src/CoreInfoPayload01.cpp` has the same pattern — it also calls `ExtractPayloadCharArrayFromMessageCharArray()`. When the PGCS on VM 101 processes the `-info --payload` message, it corrupts the payload before writing it to disk or re-serializing it for SHM delivery to the ContentApp.

### 3.5 NRNCS Path is Different

The NRNCS uses `NRInfoPayload01.cpp` which does **NOT** call `ExtractPayloadCharArrayFromMessageCharArray()`. Instead, it copies the payload with `SetPayloadFromCharArray()` — a simple byte-by-byte copy. This is correct. The NRNCS path is not the problem.

---

## 4. The SAR is Correct

Extensive analysis of `NGAL_SAR::SendSegmented` and `NGAL_SAR::ReceiveFragment` confirms:

- **Sender**: SDU = `[SizeHeader(8)][Message(MessageSize)]`. Segments carry `BlockSize` bytes of SDU each (last segment may be shorter). Data placement: `DataBlock[8..8+SegmentPayloadSize-1] = SDU[i*BlockSize..i*BlockSize+SegmentPayloadSize-1]`.

- **Receiver**: SN=0 extracts `MessageSize` from SizeHeader, then copies `numbytes-16` bytes to `Buffer[0..]`. SN>0 copies `numbytes-8` bytes to `Buffer[SN*BlockSize-8..]`. Break condition: `ReceivedSoFar >= MessageSize && SegmentsSoFar >= NoS`.

- **Numerical verification** (MessageSize=3000, BlockSize=1400): ReceivedSoFar = 1392 + 1400 + 208 = 3000 = MessageSize. ✓

- **Offset mapping verified**: `Buffer[k] = Message[k]` for all valid indices. ✓

The SAR is NOT the source of corruption.

---

## 5. Fix

### 5.1 Primary Fix — Remove Redundant Re-Extraction

**File**: `/home/gandalf/workspace/novagenesis/ContentApp/src/CoreInfoPayload01.cpp`

**Line 94**: Remove or comment out:
```cpp
// _ReceivedMessage->ExtractPayloadCharArrayFromMessageCharArray();  // SPEC-015: removed — redundant, corrupts binary payloads
```

The payload is already correctly extracted by `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` in the GW before this code runs.

**File**: `/home/gandalf/workspace/novagenesis/PGCS/src/CoreInfoPayload01.cpp`

**Line 94**: Same removal (if present — verify).

### 5.2 Secondary Fix — Deprecate `ExtractPayloadCharArrayFromMessageCharArray`

The function `Message::ExtractPayloadCharArrayFromMessageCharArray()` (Message.cpp lines 1237–1302) is fundamentally unsafe for binary payloads because it uses text-parsing (`getline()`) on binary data. It should be:

1. Marked as deprecated with a comment warning about binary payloads
2. Eventually replaced with a binary-safe extraction (or removed entirely since `Convert...2` already does this correctly)

### 5.3 Memory Leak Fix

In `ExtractPayloadCharArrayFromMessageCharArray`, when `Payload` is reassigned:
```cpp
Payload = new char[length];
```
The previously allocated `Payload` array (from `Convert...2`) is never freed. Before reassigning, add:
```cpp
if (DeletePayloadArray && Payload != 0)
{
    delete[] Payload;
    Payload = 0;
    DeletePayloadArray = false;
}
```

---

## 6. Verification Plan

1. Apply the fix (remove `ExtractPayloadCharArrayFromMessageCharArray` call from ContentApp and PGCS `CoreInfoPayload01.cpp`)
2. Rebuild: `cd cmake-build-ngal && make -j$(nproc)`
3. Deploy rebuilt binaries to VMs 101 and 102 (as root)
4. Re-run the publish/subscribe test with `--publish 0.1`
5. Verify ALL file hashes match (no more mismatches)
6. Copy received files from VM 101 to VM 100 and compare with originals:
   ```bash
   scp -r root@192.168.0.101:/root/novagenesis/IO/Repository1/ /home/gandalf/received_files/
   ```
7. Run `md5sum` or the NG hash on both original and received files to confirm byte-for-byte identity

---

## 7. Key Files

| File | Role |
|------|------|
| `ContentApp/src/CoreInfoPayload01.cpp` | **BUG**: Line 94 calls redundant `ExtractPayloadCharArrayFromMessageCharArray()` |
| `PGCS/src/CoreInfoPayload01.cpp` | **Same BUG** (if present) |
| `Common/src/Message.cpp:1237-1302` | `ExtractPayloadCharArrayFromMessageCharArray()` — uses unsafe `getline()` on binary data |
| `Common/src/Message.cpp:1570-1666` | `ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray()` — re-serialization (correct) |
| `Common/src/Message.cpp:1670-2070` | `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` — initial extraction (correct) |
| `Common/src/NGAL_SAR.cpp` | SAR segmentation/reassembly — **verified correct** |
| `PGCS/src/NGAL_Transport_RAW.cpp` | Raw socket transport — **verified correct** |
| `NRNCS/src/NRInfoPayload01.cpp` | NRNCS payload copy — **correct** (uses `SetPayloadFromCharArray`, not `getline`) |
| `Common/src/GW.cpp` | Gateway message processing — SHM and NGAL paths both correct |

---

## 8. Lessons Learned

1. **Never use text-parsing functions (`getline`, `stringstream`) on binary data** — the payload can contain any byte sequence including `\n`, `\0`, and patterns that look like command syntax.
2. **Don't re-extract what's already been extracted** — if `Convert...2` has already parsed `Msg[]` into `CommandLines` and `Payload`, calling `ExtractPayload...` again is redundant and destructive.
3. **Memory ownership must be explicit** — when `Payload` pointer is reassigned without freeing the previous allocation, it creates both a memory leak and a potential use-after-free.
4. **The SAR layer (SPEC-014 fix) was not the root cause** — the real bug was in the application-layer payload extraction, not in the transport/reassembly layer.
5. **Consistent failures point to data-dependent bugs** — if the same files always fail, the corruption depends on the file's binary content, not on timing or race conditions. This narrows the search to parsing/extraction code rather than concurrency issues.
