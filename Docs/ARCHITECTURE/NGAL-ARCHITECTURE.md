# NGAL Architecture — NovaGenesis Adaptation Layer

**Source:** SPEC-013 (NGAL Adaptation Layer) — sections E1, E2, E3  
**Status:** Implemented (commit 164cc49+)  
**Branch:** AIOPT3

---

## E1 — The NGAL Concept

The **NovaGenesis Adaptation Layer (NGAL)** is the NovaGenesis equivalent of the **ATM Adaptation Layer (AAL)**. Just as AAL sits between ATM (cell transport) and higher layers (voice, video, data), NGAL sits between the **NovaGenesis message layer** (GW/Message/CommandLine) and the **physical transport** (raw Ethernet sockets, UDP, future TSN, 5G NR, LoRaWAN).

### E1.1 — NGAL Sublayer Architecture

```
  ┌─────────────────────────────────────────────┐
  │         GW / Message / CommandLine          │  ← NG message layer
  │   (priority queue, PushToInputQueue, Run)   │
  ├─────────────────────────────────────────────┤
  │            NGAL-CS                          │  ← Convergence Sublayer
  │   (delivery to GW: PushToInputQueue)         │
  │   (IPC abstraction: inter-process SHM)       │
  ├─────────────────────────────────────────────┤
  │            NGAL-SAR                         │  ← Segmentation & Reassembly
  │   (SAR-PDU header: 8B size + 8B seg field)  │
  │   (MessageNumber / SequenceNumber mgmt)      │
  ├─────────────────────────────────────────────┤
  │    NGAL-T (Transport Sublayer)              │  ← Transport-specific
  │   ┌─────────┐  ┌─────────┐  ┌──────────┐   │
  │   │ RAW     │  │ UDP     │  │ Future.. │   │
  │   │ socket  │  │ socket  │  │ (TSN,5G) │   │
  │   └─────────┘  └─────────┘  └──────────┘   │
  └─────────────────────────────────────────────┘
```

### E1.2 — Sublayer Responsibilities

| Sublayer | Abbrev | Responsibility | Current location |
|----------|--------|----------------|------------------|
| **Convergence** | NGAL-CS | Receive completed message → deliver to GW via `PushToInputQueue` (intra-process) or SHM (inter-process) | `FinishReceivingThread` → `WriteToSharedMemory3` → GW `ReadFromSharedMemory3` → `PushToInputQueue` |
| **Segmentation & Reassembly** | NGAL-SAR | Split NG messages into network-sized PDUs; reassemble fragments; insert/parse size header (8B) + segmentation header (8B) | Inline in `SendToARawSocket`, `SocketDispatcher3`, `SendToAUDPSocket`, `ReceiveFromAUDPSocket` |
| **Transport** | NGAL-T | Raw socket I/O, UDP socket I/O, future transports | `SendToARawSocket` (last mile), `SocketDispatcher3` (first mile), `CreateRawSocket`, `CreateUDPSocket` |

### E1.3 — NGAL-PDU Format

Every NG message sent over a network transport is encapsulated in an **NGAL-PDU**:

```
  ┌──────────┬──────────────────┬──────────────────────┐
  │ 8 bytes  │   8 bytes        │   Variable length     │
  │  Size    │  Segmentation    │   NG Message          │
  │  Header  │  Header          │   (serialised)        │
  ├──────────┼──────────────────┼──────────────────────┤
  │TotalSize │ MN (4B) | SN (4B)│  Command lines +      │
  │big-endian│ big-endian       │  payload              │
  └──────────┴──────────────────┴──────────────────────┘
```

Where:
- **Size Header** (8 bytes): `TotalSize = MessageSize + 8`. Big-endian, 64-bit. Here `MessageSize` is the serialised NG message size (command lines + payload, obtained via `M->GetMessageSize()`). The `+8` accounts for the Size Header itself. Used by the receiver to allocate the reassembly buffer.
- **Segmentation Header** (8 bytes): `MN` (Message Number, 4 bytes) + `SN` (Sequence Number, 4 bytes). Big-endian.
  - `MN`: Random 32-bit identifier for this message. Used to correlate fragments.
  - `SN`: Fragment index within the message (0, 1, 2...). `NoS = ceil((8+MessageSize)/BlockSize)`.
- **NG Message**: Serialised form of the Message object (command lines + payload). Size = `TotalSize - 8`.

**NGAL-PDU header insertion** (send path):
```
  [SizeHeader(8B)] [SegHeader(8B)] [Payload(N bytes)]
```

**NGAL-PDU header removal** (receive path):
```
  recvfrom() → TempBuffer
    → OpenHeaderSegmentationField(TempBuffer[0..7], MN, SN)
    → OpenHeaderMessageSizeField(TempBuffer[8..15], TotalSize)
    → Payload = TempBuffer[16 .. 16+TotalSize-8]
```

---

## E2 — Current Architecture Map

### E2.1 — PG.cpp function → NGAL layer mapping

| PG.cpp function | Lines | NGAL Sublayer | Notes |
|-----------------|-------|---------------|-------|
| `CreateRawSocket` | 25 | NGAL-T (RAW) | Simple — create PF_PACKET socket. Keep |
| `CreateUDPSocket` | 26 | NGAL-T (UDP) | Simple — create socket. Keep |
| `GetHostIPAddress` | 75 | NGAL-T | Discovery utility. Keep |
| `GetHostRawAddress` | 57 | NGAL-T | Discovery utility. Keep |
| `Hex2Char` | 13 | NGAL-T | Utility. Keep |
| **`SendToARawSocket`** | **394** | **NGAL-SAR + NGAL-T (RAW)** | **Giant. ~80% is NGAL-SAR (common to RAW+UDP). ~20% is NGAL-T RAW (sendto)** |
| **`SendToAUDPSocket`** | **333** | **NGAL-SAR + NGAL-T (UDP)** | **Giant. ~80% is NGAL-SAR (identical to RAW). ~20% is NGAL-T UDP** |
| **`SocketDispatcher3`** | **474** | **NGAL-T (RAW) + NGAL-SAR + NGAL-CS** | **Does 3 jobs: recv RAW, reassembly SAR, child-thread dispatch CS** |
| `ReceiveFromAUDPSocket` | **~394** | **NGAL-T (UDP) + NGAL-SAR + NGAL-CS** | **~70% NGAL-SAR identical to SocketDispatcher3** |
| **`FinishReceivingThread`** | **102** | **NGAL-CS** | **Convergence → SHM. Will be replaced by PushToInputQueue direct** |
| **`WriteToSharedMemory3`** | **270** | **NGAL-CS (IPC)** | **Intra-process: will be eliminated (direct). Inter-process: keep** |
| `OpenHeaderMessageSizeField` | 33 | NGAL-SAR | Header parsing. Must go to NGAL |
| `OpenHeaderSegmentationField` | 30 | NGAL-SAR | Header parsing. Must go to NGAL |
| `NewAction` | 93 | — | If-else chain. Keep in PG.cpp |
| `GetAction` / `DeleteAction` | 5 each | — | Dead stubs. Remove |
| Thread wrappers | 24 | — | Keep |
| `get_in_addr` | 9 | NGAL-T (UDP) | Utility |
| `ResetStatistics` | 4 | — | Dead stub. Remove |

### E2.2 — Current data flow (raw socket receive)

```
RAW socket
    │ recvfrom()
    ▼
SocketDispatcher3()           ← NGAL-T (recv frame)
    │ reassembly (fragments)
    ▼
MessageReceiving completo
    │ dispatch para child thread
    ▼
FinishReceivingThread()      ← NGAL-CS (convergence via SHM)
    │ WriteToSharedMemory3()
    ▼
Shared Memory (key 11+z)     ← IPC intra-process
    │
    ▼
GW::Gateway() loop           ← GW thread
    │ ReadFromSharedMemory3()
    │ PushToInputQueue(PM)
    ▼
GW::Run(PM)                  ← processamento
```

### E2.3 — Current data flow (raw socket send)

**Correction (F1):** The intra-process send path does **NOT** go through SHM. `SendToARawSocket` is called directly from PG action code (PGRunHello01, PGMsgCl01, PGRunHello02, PGRunHello03) running on the GW thread. The `GW::PushToOutputQueue → ReadFromOutputQueue → WriteToSharedMemory3` path is for **inter-process** delivery only (e.g. PGCS GW writes to SHM so that a peer process like NRNCS can read via `GW::ReadFromSharedMemory3`).

**Intra-process send path (actual):**
```
GW::Run(PM)                  ← GW thread processes message
    │ PG::Run(...) → action code (PGRunHello01, PGMsgCl01, ...)
    ▼
PG::SendToARawSocket()        ← Same GW thread
    │ NGAL-SAR: serialise, segment
    │ NGAL-T: sendto() on Ethernet frame
    ▼
RAW socket                    ← network output
```

**Inter-process send path (via OutputQueue → SHM):**
```
GW::PushToOutputQueue(OQS, M) ← block origin
    │
    ▼
GW::ReadFromOutputQueue thread ← GW output thread
    │ WriteToSharedMemory3(OQS, M)
    ▼
Shared Memory (key OQS+z)
    │
    ▼
Peer process GW::ReadFromSharedMemory3() ← different process
```

**Key insight:** The SHM hop on the **send** side only exists for inter-process delivery. For intra-process sends, `SendToARawSocket` is already called directly — no SHM to eliminate.

---

## E3 — Proposed Architecture: NGAL Refactored

### E3.1 — New files

| File | Content | Lines (est.) | Replaces |
|------|---------|-------------|----------|
| `Common/src/NGAL_SAR.h/.cpp` | NGAL Segmentation & Reassembly — common SAR-PDU header insert/parse, fragment/reassemble | ~200 | ~600 lines of `SendToARawSocket` + `SendToAUDPSocket` + `SocketDispatcher3` + `ReceiveFromAUDPSocket` |
| `Common/src/NGAL_CS.h/.cpp` | NGAL Convergence Sublayer — deliver completed message to GW::PushToInputQueue (intra-process) or SHM (inter-process) | ~150 | `FinishReceivingThread` + `WriteToSharedMemory3` (intra-process part) |
| `PGCS/src/NGAL_Transport_RAW.h/.cpp` | NGAL-T RAW implementation — raw socket recv/send, child thread dispatch | ~250 | Last-mile send/recv in `SendToARawSocket` + `SocketDispatcher3` |
| `PGCS/src/NGAL_Transport_UDP.h/.cpp` | NGAL-T UDP implementation — UDP recv/send | ~250 | Last-mile send/recv in `SendToAUDPSocket` + `ReceiveFromAUDPSocket` (future) |

**Total new code:** ~850 lines (vs ~1900 lines duplicated today).

### E3.2 — NGAL_SAR class design

```cpp
// Common/src/NGAL_SAR.h
class NGAL_SAR {
public:
  // ── Send SAR ──
  // Serialise Message → NGAL-PDU fragments. Calls transport_callback for each fragment.
  // transport_callback receives: (char* fragment_data, unsigned int fragment_size, unsigned int fragment_index)
  int SendSegmented(Message* M,
                    unsigned int BlockSize,
                    unsigned int& MessageNumber,
                    unsigned int& SequenceNumber,
                    unsigned int& MessageCounter,
                    std::function<int(char*, unsigned int, unsigned int)> transport_callback);

  // ── Receive SAR ──
  // Process one frame from transport. Manages reassembly buffer internally.
  // Returns completed Message* via output parameter when reassembly finishes.
  // Returns nullptr if more fragments needed.
  int ReceiveFragment(unsigned char* TempBuffer,
                      unsigned int numbytes,
                      unsigned int BlockSize,
                      unsigned int HeaderOffset,  // 0 for RAW, 0 for UDP (both same format)
                      Process* PP,
                      Message*& CompletedMessage);

  // ── Header helpers (static) ──
  static long long OpenHeaderMessageSizeField(unsigned char* _Buffer);
  static void OpenHeaderSegmentationField(unsigned char* _Buffer,
                                          unsigned int& _MessageNumber,
                                          unsigned int& _SequenceNumber);
  static void BuildHeaderSizeField(unsigned char* Header, long long TotalSize);
  static void BuildHeaderSegmentationField(unsigned char* Header,
                                            unsigned int MessageNumber,
                                            unsigned int SequenceNumber);

private:
  struct FragmentBuffer {
    unsigned int MessageNumber;
    unsigned int NoS;        // Expected segments count
    long long MessageSize;
    long long ReceivedSoFar;
    unsigned int SegmentsSoFar;
    char* Buffer;
    double Timestamp;        // For timeout cleanup
    bool ContinueReceiving;
  };

  std::vector<FragmentBuffer*> ReassemblyBuffers;

  void CleanupTimedOut(double timeout_threshold);
};
```

### E3.3 — NGAL_CS class design

> **Revision (F2):** `DeliverToGateway` must NOT call `NewMessage()` from the receiver thread (data race on `Controls[]`). Instead, it pushes the raw char buffer to a thread-safe queue, and the GW thread does `NewMessage` + deserialisation + `PushToInputQueue`.

```cpp
// Common/src/NGAL_CS.h
class NGAL_CS {
public:
  // Push a completed message's raw serialised buffer to the GW's
  // intermediate receive queue (thread-safe). The GW thread will
  // later call NewMessage + SetMessageFromCharArray + PushToInputQueue.
  static int DeliverToGateway(GW* PGW,
                              char* MessageCharArray,
                              long long MessageSize);

  // Write a message to a peer process's SHM (inter-process IPC)
  // Keeps existing WriteToSharedMemory3 semantics
  static int DeliverToSHM(File* _PF, char* _MessageCharArray, long long _MessageSize,
                          GW* PGW, int shm_key, size_t MaxSegmentSize);
};
```

The GW class needs a new intermediate queue for raw char buffers:
```cpp
// Added to GW.h
std::queue<std::pair<char*, long long>> NetworkReceiveQueue;
std::mutex NetworkReceiveQueueMutex;
std::condition_variable NetworkReceiveQueueCV;
```

The GW thread's `Gateway()` loop must drain `NetworkReceiveQueue` (alongside `InputQueue` and `ReadFromSharedMemory3`), calling `NewMessage` + `SetMessageFromCharArray` + `PushToInputQueue` for each entry.

### E3.4 — NGAL_Transport_RAW class design

```cpp
// PGCS/src/NGAL_Transport_RAW.h
class NGAL_Transport_RAW {
public:
  int CreateRawSocket(int& _SID);

  // Send one NGAL-PDU fragment via raw socket
  int SendFragment(int SSID, int ifindex,
                   unsigned char* SourceMAC, unsigned char* DestMAC,
                   unsigned char* FragmentData, unsigned int FragmentSize);

  // Main receive dispatcher thread (replaces SocketDispatcher3)
  // Runs in its own thread. Receives frames, feeds NGAL_SAR, delivers via NGAL_CS.
  static void ReceiveDispatcher(PG* PPG);

private:
  // Child thread: picks up completed fragments from TemporaryBuffers
  static void ChildReceiver(PG* PPG, unsigned int Index);
};
```

### E3.5 — New data flow (raw socket receive) — PROPOSED

```
RAW socket
    │ recvfrom() [non-blocking or per-socket poll()]
    ▼
NGAL_Transport_RAW::ReceiveDispatcher()   ← NGAL-T (RAW) — ONLY recv + dispatch
    │ NGAL_SAR::ReceiveFragment()          ← NGAL-SAR — reassembly
    ▼
Message* completo
    │ NGAL_CS::DeliverToGateway(PGW, PM)   ← NGAL-CS — direct
    ▼
GW::PushToInputQueue(PM)                  ← thread-safe
    │
    ▼
GW::Run(PM)                               ← processamento
```

**SHM removed** from intra-process receive path. SHM remains ONLY for inter-process IPC (PGCS↔NRNCS).

> **Warning (F2):** `Process::NewMessage()` is NOT thread-safe — it accesses `Controls[]`, `Messages[]`, `NoM`, `MessageCounter` without a mutex. The current code avoids races because only the GW thread calls `NewMessage()`. After this change, the ReceiveDispatcher thread will also need to allocate Message objects. `NGAL_CS::DeliverToGateway()` must therefore either: (a) pass the raw char buffer to the GW thread (via a thread-safe queue) and let the GW thread do `NewMessage` + deserialisation, or (b) add a mutex to `Process::NewMessage()`. Option (a) is preferred — it keeps the GW thread as the sole allocator and matches the existing pattern.

> **Note (F3):** The current `SocketDispatcher3` uses blocking `recvfrom()` and iterates over multiple SSIDs in a single loop. Eliminating child threads introduces a latency risk: heavy traffic on one socket starves the others. The proposed `ReceiveDispatcher` should use non-blocking `recvfrom()` + `poll()` on all SSIDs, or accept the latency regression as documented.

### E3.6 — New data flow (raw socket send) — PROPOSED

> **Correction (F1):** The intra-process send path already calls `SendToARawSocket` directly from the GW thread. No SHM elimination is needed here. The refactoring changes how the SAR+T logic is invoked, not the inter-thread communication.

**Intra-process send path (refactored):**
```
GW::Run(PM)                             ← GW thread processes message
    │ PG::Run(...) → action code
    │   PG::SendToARawSocket(Interface, Identifier, Size, M)
    │     → NGAL_SAR::SendSegmented(M, BlockSize, ..., transport_callback)
    │       where transport_callback = NGAL_Transport_RAW::SendFragment()
    ▼
RAW socket                              ← network output
```

**Inter-process send path (unchanged):**
```
GW::PushToOutputQueue(OQS, M)           ← block origin
    │
    ▼
GW::ReadFromOutputQueue thread          ← GW output thread
    │ GW::WriteToSharedMemory3(OQS, M)
    ▼
Shared Memory (key OQS+z)               ← IPC inter-process — unchanged
    │
    ▼
Peer process GW::ReadFromSharedMemory3()
```

> **Note (F8):** `PG::MessageNumber`, `PG::SequenceNumber`, `PG::MessageCounter` (PG.h:238-244) are instance variables with no mutex protection. They are currently only accessed from the GW thread. If `NGAL_SAR::SendSegmented()` were ever called from multiple threads, these would need a mutex or must be passed as local variables per invocation (not instance state). The proposed design keeps them as instance state on the PG object — this is safe only because all callers run on the GW thread.

---

## E4 — UDP Obsolete

### E4.1 — Current status

UDP (`SendToAUDPSocket`, `ReceiveFromAUDPSocket`, `CreateUDPSocket`, `get_in_addr`) is not in active use. The code was written for a 5G GFDM scenario that is no longer relevant. Contains:
- The same ~280 lines of NGAL-SAR as RAW (duplicated)
- UDP-specific `bind()` + `recvfrom()` logic
- `sockaddr_in` construction with `IP:Port` parsing
- UDP `sendto()` instead of RAW `sendto()`

### E4.2 — Decision

**Do not rewrite UDP now.** When needed in future:
1. Create `NGAL_Transport_UDP` following the same contract as `NGAL_Transport_RAW`
2. Implement `SendFragment()` and `ReceiveDispatcher()` specific to UDP
3. Reuse `NGAL_SAR` and `NGAL_CS` without modifications

### E4.3 — Immediate actions for UDP

| Action | Justification |
|--------|---------------|
| Keep `CreateUDPSocket` in PG.cpp | Harmless, ~26 lines, no maintenance cost |
| **Remove** `get_in_addr` (9 lines) | Obsolete — only used by UDP. Recover from git if needed |
| Remove `SendToAUDPSocket` (333 lines) | Obsolete, duplicated with RAW. Recover from git when needed |
| Remove `ReceiveFromAUDPSocket` (~394 lines) | Obsolete, duplicated. Recover from git when needed |
| Remove `ReceiveFromAUDPSocketThreadWrapper` | Obsolete |

> **Note (F7):** This table supersedes the earlier E4.3 note suggesting "keep `get_in_addr`". Since `get_in_addr` is only used by the UDP receive path, and that path is being removed, `get_in_addr` should also be removed. If UDP is re-introduced in the future, both `get_in_addr` and the full UDP receive path can be recovered from git.

---

## E5 — Elimination of SHM Hop on Intra-Process Path

### E5.1 — Concurrency analysis

SHM was introduced as a concurrency barrier between receive threads and the GW thread. Today, `GW::PushToInputQueue` is already thread-safe:

```cpp
// GW.cpp:211
void GW::PushToInputQueue(Message* M) {
    // ... validations ...
    {
        std::lock_guard<std::mutex> lock(InputQueueMutex);     // ← mutex
        InputQueue.push(M);
        InputQueueTag++;
    }
    InputQueueCV.notify_one();                                  // ← CV
}
```

`Process::NewMessage()` allocates on heap — no shared state between threads.

### E5.2 — Safety condition

> **Critical correction (F2):** The original analysis claimed `Process::NewMessage()` is safe because it "only allocates on the heap." This is **wrong**. `Process::NewMessage()` (Process.cpp:542-567) accesses shared state without any mutex: `Controls[]`, `Messages[]`, `NoM`, `MessageCounter`. If the ReceiveDispatcher thread calls `DeliverToGateway()` → `NewMessage()`, it races with the GW thread which also calls `NewMessage()`. The SHM hop was inadvertently protecting against this race by serialising access through the GW thread only.

| Operation | Thread | Shared resource | Protection |
|-----------|--------|-----------------|------------|
| `NewMessage()` | **GW thread AND Receiver thread (after change)** | `Controls[]`, `Messages[]`, `NoM`, `MessageCounter` | **NONE — DATA RACE if both threads call it** |
| `SetMessageFromCharArray()` | Receiver thread | Message object | None — new object (safe) |
| `PushToInputQueue()` | Receiver thread | InputQueue | `InputQueueMutex` + CV |
| `InputQueue.top()/pop()` | GW thread | InputQueue | `InputQueueMutex` |
| `Run(PM)` | GW thread | Message object | Exclusive — only GW processes |
| `MessageNumber`, `SequenceNumber`, `MessageCounter` | GW thread | PG instance vars | None — but only one thread accesses (safe in current proposal) |

**Safety condition for SHM hop elimination:**

`NGAL_CS::DeliverToGateway()` must NOT call `Process::NewMessage()` from the receiver thread. Instead, it should:
1. Push the raw serialised char buffer + size to a thread-safe intermediate queue (using the same `InputQueueMutex` + CV pattern)
2. Let the GW thread pop from that queue, call `NewMessage()`, `SetMessageFromCharArray()`, `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()`, and then `PushToInputQueue()`

This keeps `NewMessage()` single-threaded (GW thread only) and avoids the data race entirely. The intermediate queue replaces the SHM hop with a much cheaper mutex+CV hop (no `shmat`/`shmdt` syscalls).

### E5.3 — Impact

> **Correction (F1):** The original impact table listed both `PG::WriteToSharedMemory3` and `GW::ReadFromSharedMemory3` as eliminated. This was based on the mistaken belief that the send path also used SHM. In reality, only the **receive** path uses SHM intra-process. The send path SHM (GW::WriteToSharedMemory3 for inter-process) stays unchanged.

| Before | After |
|--------|-------|
| `PG::WriteToSharedMemory3(F1, data, size)` (SHM syscall: shmat, sem_trywait, memcpy, sem_post, shmdt) on the **receive** path | `NGAL_CS::DeliverToGateway(PGW, PM)` → push char buffer to intermediate queue → GW thread calls `NewMessage` + `PushToInputQueue` (mutex lock, push, CV notify) |
| GW: `ReadFromSharedMemory3()` (shmat, sem_trywait, memcpy, NewMessage, sem_post, PushToInputQueue, shmdt) for intra-process receive messages | Eliminated — GW reads from the intermediate queue instead of SHM for intra-process network messages |
| GW: `WriteToSharedMemory3(OQS, M)` on the **send** path (inter-process IPC) | **Unchanged** — remains for PGCS↔NRNCS/ContentApp IPC |
| ~270 lines (PG::WriteToSharedMemory3 receive usage) + inline SHM in FinishReceivingThread | ~30 lines (DeliverToGateway + intermediate queue) |

### E5.4 — Where SHM STAYS

SHM remains the IPC mechanism between processes: PGCS↔NRNCS, PGCS↔ContentApp. The function `WriteToSharedMemory3` (or `NGAL_CS::DeliverToSHM`) remains for this purpose. What changes is that network-received messages **inside PGCS** no longer go through SHM to reach the GW.

---

## E6 — Implementation Steps

### E6.1 — Dependencies between steps

```
E1 (NGAL_SAR.h)
  │
  ├──→ E2 (NGAL_SAR.cpp — send)
  │         │
  │         └──→ E3 (NGAL_SAR.cpp — receive)
  │
  ├──→ E4 (NGAL_CS.h/.cpp)
  │
  ├──→ E5 (NGAL_Transport_RAW.h — skeleton)
  │         │
  │         └──→ E6 (NGAL_Transport_RAW.cpp — send)
  │                   │
  │                   └──→ E7 (NGAL_Transport_RAW.cpp — receive dispatcher)
  │                             │
  │                             └──→ E8 (Integrate into PG.cpp)
  │                                       │
  │                                       └──→ E10 (Compile & test)
  │
  └──→ E9 (Remove UDP dead code + stubs)
```

### E6.2 — Detailed steps

#### E1 — Create `Common/src/NGAL_SAR.h`

- Declare class `NGAL_SAR` with complete interface
- Declare `struct FragmentBuffer` private
- Declare static header methods (build/open)
- Include in `CMakeLists.txt` (`add_library(Common ...)`)

**Files:** `Common/src/NGAL_SAR.h`, `CMakeLists.txt`  
**Lines:** ~60