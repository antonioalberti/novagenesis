# SPEC-013 — NovaGenesis Adaptation Layer (NGAL)

**Version:** v0.2
**Date:** 2026-07-07
**Author:** (derived from analysis)
**Status:** Proposal (revised — v0.2 incorporates critical review findings F1–F12)

---

## E0 — Problem Statement

The `PG` block in PGCS (`PGCS/src/PG.cpp`, 2695 lines) currently implements five responsibilities interleaved in the same file:

1. **Network adaptation** — raw Ethernet socket I/O and UDP socket I/O
2. **Frame segmentation & reassembly** — SAR logic for NG messages over network PDUs
3. **NGAL-PDU header insertion/parsing** — 8-byte size field + 8-byte segmentation field prepended to every NG message
4. **Child-thread dispatch** — distribution of reassembled messages to worker threads (`FinishReceivingThread`)
5. **IPC convergence** — writing received messages to shared memory (`WriteToSharedMemory3`) for the GW to consume via `ReadFromSharedMemory3`

This structure has five concrete problems:

| # | Problem | Evidence |
|---|---------|----------|
| P1 | **Massive duplication between RAW and UDP send paths** | `SendToARawSocket` (394 linhas) vs `SendToAUDPSocket` (333 linhas) share ~80% of code — serialisation, size header, segmentation loop, all identical |
| P2 | **Massive duplication between RAW and UDP receive paths** | `SocketDispatcher3` (474 linhas) vs `ReceiveFromAUDPSocket` (~394 linhas) share ~70% of reassembly logic — `MessageReceiving` buffer, stop criteria, timeout |
| P3 | **Single-function overload: `SocketDispatcher3` does 3 jobs** | Frame reception + child-thread dispatch (~120 linhas de semáforo/retry) + timeout cleanup |
| P4 | **Unnecessary SHM hop for intra-process message delivery** | `FinishReceivingThread` writes to SHM, GW reads from SHM, GW calls `PushToInputQueue` — but `PushToInputQueue` is already thread-safe (mutex + CV). Comment in PG.cpp:2191 confirms: *"Obviously, this should be replaced in future."* |
| P5 | **UDP transport implementation is obsolete** | No active use. When needed, must be rewritten from scratch aligned with NGAL, not patched. |

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

> **Note (F5):** The code at PG.cpp:576-583 writes `MessageSize` (not `MessageSize + 8`) into the Size Header bytes. The `+8` refers to the total PDU length used by the receiver for buffer allocation, not to the value written into the header field. The receiver reads the Size Header to get `MessageSize`, then allocates `MessageSize + 8` for the full SDU.

> **Note (F6):** The NG protocol type is `0x1234` on the wire. In the code, `SocketDispatcher3` checks `saddrll.sll_protocol == 13330` (which is `0x3412` — `0x1234` in little-endian host byte order for `sll_protocol`). When creating the socket (PG.cpp:442,485), `proto = 0x1234` is passed to `htons()`. Any future NGAL-T implementation must preserve this byte-order handling.

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
| `CreateRawSocket` | 25 | NGAL-T (RAW) | Simples — criar socket PF_PACKET. Mantém-se |
| `CreateUDPSocket` | 26 | NGAL-T (UDP) | Simples — criar socket. Mantém-se |
| `GetHostIPAddress` | 75 | NGAL-T | Utilitário de descoberta. Mantém-se |
| `GetHostRawAddress` | 57 | NGAL-T | Utilitário de descoberta. Mantém-se |
| `Hex2Char` | 13 | NGAL-T | Utilitário. Mantém-se |
| **`SendToARawSocket`** | **394** | **NGAL-SAR + NGAL-T (RAW)** | **Gigante. ~80% é NGAL-SAR (comum a RAW+UDP). ~20% é NGAL-T RAW (sendto)** |
| **`SendToAUDPSocket`** | **333** | **NGAL-SAR + NGAL-T (UDP)** | **Gigante. ~80% é NGAL-SAR (idêntico a RAW). ~20% é NGAL-T UDP** |
| **`SocketDispatcher3`** | **474** | **NGAL-T (RAW) + NGAL-SAR + NGAL-CS** | **Faz 3 jobs: recv RAW, reassembly SAR, child-thread dispatch CS** |
|| `ReceiveFromAUDPSocket` | **~394** | **NGAL-T (UDP) + NGAL-SAR + NGAL-CS** | **~70% NGAL-SAR idêntico a SocketDispatcher3** |
| **`FinishReceivingThread`** | **102** | **NGAL-CS** | **Convergence → SHM. Será substituída por PushToInputQueue directo** |
| **`WriteToSharedMemory3`** | **270** | **NGAL-CS (IPC)** | **Intra-process: será eliminado (directo). Inter-process: mantém** |
| `OpenHeaderMessageSizeField` | 33 | NGAL-SAR | Header parsing. Deve ir para NGAL |
| `OpenHeaderSegmentationField` | 30 | NGAL-SAR | Header parsing. Deve ir para NGAL |
| `NewAction` | 93 | — | If-else chain. Mantém-se em PG.cpp |
| `GetAction` / `DeleteAction` | 5 cada | — | Stubs mortos. Remover |
| Thread wrappers | 24 | — | Mantém-se |
| `get_in_addr` | 9 | NGAL-T (UDP) | Utilitário |
| `ResetStatistics` | 4 | — | Stub morto. Remover |

### E2.2 — Current data flow (raw socket receive)

```
RAW socket
    │ recvfrom()
    ▼
SocketDispatcher3()           ← NGAL-T (recv frame)
    │ reassembly (fragmentos)
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

> **Correction (F1):** The intra-process send path does **NOT** go through SHM. `SendToARawSocket` is called directly from PG action code (PGRunHello01, PGMsgCl01, PGRunHello02, PGRunHello03) running on the GW thread. The `GW::PushToOutputQueue → ReadFromOutputQueue → WriteToSharedMemory3` path is for **inter-process** delivery only (e.g. PGCS GW writes to SHM so that a peer process like NRNCS can read via `GW::ReadFromSharedMemory3`).

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
| `Common/src/NGAL_SAR.h/.cpp` | NGAL Segmentation & Reassembly — common SAR-PDU header insert/parse, fragment/reassemble | ~200 | ~600 linhas de `SendToARawSocket` + `SendToAUDPSocket` + `SocketDispatcher3` + `ReceiveFromAUDPSocket` |
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
NGAL_Transport_RAW::ReceiveDispatcher()   ← NGAL-T (RAW) — APENAS recv + dispatch
    │ NGAL_SAR::ReceiveFragment()          ← NGAL-SAR — reassembly
    ▼
Message* completo
    │ NGAL_CS::DeliverToGateway(PGW, PM)   ← NGAL-CS — directo
    ▼
GW::PushToInputQueue(PM)                  ← thread-safe
    │
    ▼
GW::Run(PM)                               ← processamento
```

**SHM removido** do caminho intra-process receive. SHM mantém-se APENAS para IPC inter-processos (PGCS↔NRNCS).

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
    │       onde transport_callback = NGAL_Transport_RAW::SendFragment()
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

## E4 — UDP Obsoleto

### E4.1 — Status actual

UDP (`SendToAUDPSocket`, `ReceiveFromAUDPSocket`, `CreateUDPSocket`, `get_in_addr`) não está em uso activo. O código foi escrito para um cenário 5G GFDM que não é mais relevante. Contém:

- As mesmas ~280 linhas de NGAL-SAR que RAW (duplicadas)
- Lógica de `bind()` + `recvfrom()` UDP específica
- Construção de `sockaddr_in` com parsing de `IP:Port`
- `sendto()` UDP em vez de `sendto()` RAW

### E4.2 — Decisão

**Não re-escrever UDP agora.** Quando necessário no futuro:

1. Criar `NGAL_Transport_UDP` seguindo o mesmo contrato de `NGAL_Transport_RAW`
2. Implementar `SendFragment()` e `ReceiveDispatcher()` específicos UDP
3. Reutilizar `NGAL_SAR` e `NGAL_CS` sem modificações

### E4.3 — Acções imediatas para UDP

| Acção | Justificação |
|-------|-------------|
| Manter `CreateUDPSocket` em PG.cpp | Inócua, ~26 linhas, sem custo de manutenção |
| **Remover** `get_in_addr` (9 linhas) | Obsoleto — só usado por UDP. Recuperar do git se necessário |
| Remover `SendToAUDPSocket` (333 linhas) | Obsoleto, duplicado com RAW. Recuperar do git quando necessário |
| Remover `ReceiveFromAUDPSocket` (~394 linhas) | Obsoleto, duplicado. Recuperar do git quando necessário |
| Remover `ReceiveFromAUDPSocketThreadWrapper` | Obsoleto |

> **Note (F7):** This table supersedes the earlier E4.3 note suggesting "keep `get_in_addr`". Since `get_in_addr` is only used by the UDP receive path, and that path is being removed, `get_in_addr` should also be removed. If UDP is re-introduced in the future, both `get_in_addr` and the full UDP receive path can be recovered from git.

---

## E5 — Eliminação do Hop SHM no Caminho Intra-Processo

### E5.1 — Análise de concorrência

A SHM foi introduzida como barreira de concorrência entre as threads de recepção e a thread do GW. Hoje, `GW::PushToInputQueue` já é thread-safe:

```cpp
// GW.cpp:211
void GW::PushToInputQueue(Message* M) {
    // ... validações ...
    {
        std::lock_guard<std::mutex> lock(InputQueueMutex);     // ← mutex
        InputQueue.push(M);
        InputQueueTag++;
    }
    InputQueueCV.notify_one();                                  // ← CV
}
```

`Process::NewMessage()` aloca no heap — sem estado partilhado entre threads.

### E5.2 — Condição de segurança

> **Critical correction (F2):** The original analysis claimed `Process::NewMessage()` is safe because it "only allocates on the heap." This is **wrong**. `Process::NewMessage()` (Process.cpp:542-567) accesses shared state without any mutex: `Controls[]`, `Messages[]`, `NoM`, `MessageCounter`. If the ReceiveDispatcher thread calls `DeliverToGateway()` → `NewMessage()`, it races with the GW thread which also calls `NewMessage()`. The SHM hop was inadvertently protecting against this race by serialising access through the GW thread only.

| Operação | Thread | Recurso partilhado | Protecção |
|----------|--------|-------------------|-----------|
| `NewMessage()` | **GW thread AND Receiver thread (after change)** | `Controls[]`, `Messages[]`, `NoM`, `MessageCounter` | **NONE — DATA RACE if both threads call it** |
| `SetMessageFromCharArray()` | Receiver thread | Message object | Nenhum — objecto novo (safe) |
| `PushToInputQueue()` | Receiver thread | InputQueue | `InputQueueMutex` + CV |
| `InputQueue.top()/pop()` | GW thread | InputQueue | `InputQueueMutex` |
| `Run(PM)` | GW thread | Message object | Exclusivo — só GW processa |
| `MessageNumber`, `SequenceNumber`, `MessageCounter` | GW thread | PG instance vars | Nenhum — mas só uma thread acede (safe na proposta actual) |

**Condição de segurança para eliminação do SHM hop:**

`NGAL_CS::DeliverToGateway()` must NOT call `Process::NewMessage()` from the receiver thread. Instead, it should:
1. Push the raw serialised char buffer + size to a thread-safe intermediate queue (using the same `InputQueueMutex` + CV pattern)
2. Let the GW thread pop from that queue, call `NewMessage()`, `SetMessageFromCharArray()`, `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()`, and then `PushToInputQueue()`

This keeps `NewMessage()` single-threaded (GW thread only) and avoids the data race entirely. The intermediate queue replaces the SHM hop with a much cheaper mutex+CV hop (no `shmat`/`shmdt` syscalls).

### E5.3 — Impacto

> **Correction (F1):** The original impact table listed both PG::WriteToSharedMemory3 and GW::ReadFromSharedMemory3 as eliminated. This was based on the mistaken belief that the send path also used SHM. In reality, only the **receive** path uses SHM intra-process. The send path SHM (GW::WriteToSharedMemory3 for inter-process) stays unchanged.

| Antes | Depois |
|-------|--------|
| `PG::WriteToSharedMemory3(F1, data, size)` (SHM syscall: shmat, sem_trywait, memcpy, sem_post, shmdt) on the **receive** path | `NGAL_CS::DeliverToGateway(PGW, PM)` → push char buffer to intermediate queue → GW thread calls `NewMessage` + `PushToInputQueue` (mutex lock, push, CV notify) |
| GW: `ReadFromSharedMemory3()` (shmat, sem_trywait, memcpy, NewMessage, sem_post, PushToInputQueue, shmdt) for intra-process receive messages | Eliminado — GW reads from the intermediate queue instead of SHM for intra-process network messages |
| GW: `WriteToSharedMemory3(OQS, M)` on the **send** path (inter-process IPC) | **Unchanged** — remains for PGCS↔NRNCS/ContentApp IPC |
| ~270 lines (PG::WriteToSharedMemory3 receive usage) + inline SHM in FinishReceivingThread | ~30 lines (DeliverToGateway + intermediate queue) |

### E5.4 — Onde a SHM se MANTÉM

A SHM continua a ser o mecanismo de IPC entre processos: PGCS↔NRNCS, PGCS↔ContentApp. A função `WriteToSharedMemory3` (ou `NGAL_CS::DeliverToSHM`) mantém-se para este propósito. O que muda é que as mensagens recebidas da rede **dentro do PGCS** já não passam pela SHM para chegar ao GW.

---

## E6 — Etapas de Implementação

### E6.1 — Dependências entre etapas

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

### E6.2 — Etapas detalhadas

#### E1 — Criar `Common/src/NGAL_SAR.h`

- Declarar classe `NGAL_SAR` com interface completa
- Declarar `struct FragmentBuffer` privada
- Declarar métodos estáticos de header (build/open)
- Incluir em `CMakeLists.txt` (`add_library(Common ...)`)

**Ficheiros:** `Common/src/NGAL_SAR.h`, `CMakeLists.txt`
**Linhas:** ~60

#### E2 — Implementar `NGAL_SAR::SendSegmented()` em `Common/src/NGAL_SAR.cpp`

- Extrair de `SendToARawSocket` linhas 568–795 (serialização + segmentação + retry)
- Isolar a lógica de: `ConvertMessageFromCommandLines...()` → `BuildHeaderSizeField()` → `BuildHeaderSegmentationField()` → loop de fragmentos
- O `transport_callback` recebe cada fragmento e envia
- Manter retry + backoff + log warnings

**Ficheiros:** `Common/src/NGAL_SAR.cpp`
**Linhas:** ~120

#### E3 — Implementar `NGAL_SAR::ReceiveFragment()` em `Common/src/NGAL_SAR.cpp`

- Extrair de `SocketDispatcher3` linhas 969–1200 (reassembly buffer)
- Extrair de `ReceiveFromAUDPSocket` linhas 1945–2141 (reassembly equivalente)
- Unificar: `OpenHeaderSegmentationField` → buffer lookup → append → stop criteria → return Message*
- Incluir `CleanupTimedOut()` com `TIMEOUT` threshold
- Parâmetro `HeaderOffset` para suportar RAW (offset 0) e UDP (offset 0 — ambos o mesmo formato)

**Ficheiros:** `Common/src/NGAL_SAR.cpp`
**Linhas:** ~120

#### E4 — Criar `Common/src/NGAL_CS.h/.cpp`

- Implementar `DeliverToGateway(GW* PGW, char* MessageCharArray, long long MessageSize)`:
  - `lock_guard(NetworkReceiveQueueMutex)` → `NetworkReceiveQueue.push({MessageCharArray, MessageSize})` → `NetworkReceiveQueueCV.notify_one()`
  - **NOT** call `NewMessage()` or `PushToInputQueue()` — those run on the GW thread only
- Implementar `DeliverToSHM()` como wrapper para `WriteToSharedMemory3` (mantendo semântica actual)
- Add `NetworkReceiveQueue`, `NetworkReceiveQueueMutex`, `NetworkReceiveQueueCV` to `GW.h`
- Modify `GW::Gateway()` loop to drain `NetworkReceiveQueue` alongside `InputQueue` and `ReadFromSharedMemory3`
- Incluir em `CMakeLists.txt` — add `NGAL_CS.h/.cpp` to the explicit file list in `add_library(Common ...)`

**Ficheiros:** `Common/src/NGAL_CS.h`, `Common/src/NGAL_CS.cpp`, `Common/src/GW.h`, `Common/src/GW.cpp`, `CMakeLists.txt`
**Linhas:** ~40 (NGAL_CS.h/.cpp) + ~20 (GW changes) + ~5 (CMake)

#### E5 — Criar `PGCS/src/NGAL_Transport_RAW.h`

- Declarar `NGAL_Transport_RAW` class
- `CreateRawSocket(int&)`
- `SendFragment(int SSID, int ifindex, unsigned char* SrcMAC, unsigned char* DstMAC, unsigned char* data, unsigned int size)`
- `ReceiveDispatcher(PG*)` — main dispatcher thread (replaces SocketDispatcher3)
- ~~`ChildReceiver(PG*, unsigned int Index)`~~ — **Removed** (no child threads; dispatcher does reassembly + delivery directly)

> **Note (F9):** The `NGAL_SAR::SendSegmented` callback signature is `std::function<int(char*, unsigned int, unsigned int)>` (data, size, fragment_index). `NGAL_Transport_RAW::SendFragment` has a different signature (includes SSID, ifindex, MACs). The binding is done via a lambda capture in `PG::SendToARawSocket`:
> ```cpp
> auto callback = [this, SSID, ifindex, &SourceMAC, &DestMAC]
>   (char* data, unsigned int size, unsigned int) -> int {
>   return NGAL_Transport_RAW::SendFragment(SSID, ifindex, SourceMAC, DestMAC,
>                                           (unsigned char*)data, size);
> };
> ```
> This must be documented in the header file.

**Ficheiros:** `PGCS/src/NGAL_Transport_RAW.h`
**Linhas:** ~40

#### E6 — Implementar send path em `PGCS/src/NGAL_Transport_RAW.cpp`

- `CreateRawSocket()`: copiar de PG.cpp (25 linhas)
- `SendFragment()`: extrair de `SendToARawSocket` linhas 706–763:
  - Construir `sockaddr_ll`
  - Construir Ethernet frame (`ethframe` union) com h_dest, h_source, h_proto
  - `sendto()` com retry
- Nota: `SendFragment()` será chamado como callback por `NGAL_SAR::SendSegmented()`

**Ficheiros:** `PGCS/src/NGAL_Transport_RAW.cpp`
**Linhas:** ~80

#### E7 — Implementar receive path em `PGCS/src/NGAL_Transport_RAW.cpp`

- `ReceiveDispatcher()`:
  - Loop: `recvfrom()` → verificar protocolo (`sll_protocol == 13330` / `0x3412`, which is `0x1234` in host byte order) → `NGAL_SAR::ReceiveFragment()` → if complete message → `NGAL_CS::DeliverToGateway(PGW, TheMessage, MessageSize)` (pushes char buffer to `NetworkReceiveQueue`)
  - Eliminar child-thread dispatch (`TemporaryBuffers1` + semáforo `EthernetWiFi_*`)
  - Recepção directa no dispatcher, sem dispatch para child threads
  - **Use `poll()` on all SSIDs** instead of blocking `recvfrom()` in a sequential loop to avoid socket starvation (see F3)
- ~~`ChildReceiver()`~~ — **Eliminado.** Não há mais child threads para este caminho. O dispatcher faz reassembly + entrega directa.

**Justificação da eliminação do child-thread dispatch:**
- O dispatch para child threads foi introduzido para paralelizar processamento quando a SHM era o bottleneck. Com entrega directa via `NetworkReceiveQueue`,
a thread do dispatcher chama `NGAL_SAR::ReceiveFragment()` (rápido: memcpy + buffer management) e depois `NGAL_CS::DeliverToGateway()` (rápido: mutex lock + CV notify).
- O semáforo `EthernetWiFi_*` com random-retry era uma solução para contenção no `TemporaryBuffers` — desaparece com a eliminação do child-thread dispatch.
- Se no futuro for necessário paralelismo no receive path, introduz-se um pool de threads NGAL-T, não um esquema ad-hoc de semáforos.
- **Latency trade-off (F3):** Without child threads, the single dispatcher thread must reassemble + deliver each message before reading the next frame. Under burst traffic, this adds latency compared to the current parallel scheme. The `poll()` approach mitigates this by ensuring all sockets are served fairly.

**Ficheiros:** `PGCS/src/NGAL_Transport_RAW.cpp`
**Linhas:** ~140

#### E8 — Integrar NGAL em PG.cpp

Modificações em PG.cpp:

1. **Constructor (linhas 139–237):** Sem alterações — as Actions e timers mantêm-se
2. **Destructor (linhas 239–293):** Sem alterações — cleanup de vectores mantém-se
3. **`SendToARawSocket` (linhas 465–858):**
   - Substituir corpo por delegação para `NGAL_SAR::SendSegmented()` com callback lambda que captures `SSID`, `ifindex`, `SourceMAC`, `DestMAC` and calls `NGAL_Transport_RAW::SendFragment()`
   - Manter assinatura `(string _Interface, string _Identifier, unsigned int _Size, Message* M)` para compatibilidade com callers existentes (PGRunHello01, PGMsgCl01, PGRunHello02, PGRunHello03)
   - The SAR state (`MessageNumber`, `SequenceNumber`, `MessageCounter`) stays on the PG instance — safe because all callers run on the GW thread (F8)
4. **`SocketDispatcher3` (linhas 861–1334):**
   - Substituir corpo por delegação para `NGAL_Transport_RAW::ReceiveDispatcher()`
   - Ou eliminar e chamar ReceiveDispatcher directamente do código de lançamento de threads
5. **`FinishReceivingThread` (linhas 1336–1437):**
   - **Eliminar.** Substituído por `NGAL_CS::DeliverToGateway()` dentro do ReceiveDispatcher
6. **`WriteToSharedMemory3` (linhas 2195–2464):**
   - Manter para IPC inter-processos (PGCS↔NRNCS, PGCS↔ContentApp)
   - Adicionar wrapper `NGAL_CS::DeliverToSHM()` como fachada
7. **`NewAction` (linhas 2532–2624):** Sem alterações
8. **`GetAction` / `DeleteAction` (linhas 2627–2638):** Remover stubs mortos
9. **`ResetStatistics` (linha 2678–2681):** Remover stub morto
10. **Thread wrappers (linhas 2641–2664):**
    - Remover `FinishReceivingThreadWrapper`
    - Adicionar `ReceiveDispatcherWrapper` se necessário
11. **Incluir novos headers:**
    - `NGAL_SAR.h`
    - `NGAL_CS.h`
    - `NGAL_Transport_RAW.h`

**Ficheiros:** `PGCS/src/PG.cpp`, `PGCS/src/PG.h`
**Linhas alteradas:** ~ +30 (includes + delegações) / ~ -750 (código removido)

#### E9 — Remover código UDP obsoleto e stubs mortos

Remover de PG.cpp:

| Função | Linhas | Motivo |
|--------|--------|--------|
| `SendToAUDPSocket` | 1467–1799 (333 linhas) | Obsoleto, duplicado |
| `ReceiveFromAUDPSocket` | 1801–2186 (386 linhas) | Obsoleto, duplicado |
| `ReceiveFromAUDPSocketThreadWrapper` | 2641–2646 (6 linhas) | Obsoleto |
| `GetAction` | 2627–2631 (5 linhas) | Stub morto (sempre ERROR) |
| `DeleteAction` | 2634–2638 (5 linhas) | Stub morto (sempre ERROR) |
| `ResetStatistics` | 2678–2681 (4 linhas) | Stub morto (vazio) |
| `get_in_addr` | 2667–2675 (9 linhas) | Obsoleto — só UDP usava. Remover (F7 resolved) |

**Total removido:** ~748 linhas.

#### E10 — Compilar e testar

1. Actualizar `CMakeLists.txt`:
   - Adicionar `NGAL_SAR.h/.cpp` e `NGAL_CS.h/.cpp` a `Common/src/*` (já no glob)
   - Adicionar `NGAL_Transport_RAW.h/.cpp` ao target PGCS
2. Compilar PGCS: `bash compile.sh PGCS` (ou CMake)
3. Verificar erros de linkedição e sintaxe
4. Executar `sudo bash Scripts/Simple/clean.sh`
5. Executar cenário local 1core-1repo-1source com `bash templates/run-local-1core-1repo-1source.sh`
6. Verificar:
   - PGCS arranca sem crash
   - Mensagens são enviadas via raw socket (logs OK)
   - Mensagens recebidas via raw socket são processadas pelo GW
   - SHM IPC entre PGCS e NRNCS/ContentApp continua funcional

---

## E7 — Resumo de Impacto

### E7.1 — Linhas de código

| Componente | Antes | Depois | Δ |
|-----------|-------|--------|---|
| `PG.cpp` | 2695 linhas | ~1200 linhas | −1495 |
| `NGAL_SAR.h/.cpp` | — | ~250 linhas | +250 |
| `NGAL_CS.h/.cpp` | — | ~140 linhas | +140 |
| `NGAL_Transport_RAW.h/.cpp` | — | ~240 linhas | +240 |
| GW.cpp (NetworkReceiveQueue) | — | ~30 linhas | +30 |
| **Total** | **2695** | **~1860** | **−835** |

> **Note (F10):** The line reduction from PG.cpp includes ~600 lines of SAR logic that is **moved** (not deleted) to `NGAL_SAR.cpp`, plus ~748 lines of dead code (UDP + stubs) that is genuinely deleted. The net reduction accounts for both movement and deletion.

### E7.2 — Responsabilidades clarificadas

| Antes | Depois |
|-------|--------|
| PG.cpp: 5 responsabilidades (send, recv, SAR, dispatch, IPC) | PG.cpp: 1 responsabilidade (bloco PG — coordenação + delegação) |
| SHM receive: 2 papéis (barreira concorrência intra-processo + IPC inter-processo) | SHM receive: 1 papel (IPC inter-processo apenas). Intra-process: `NetworkReceiveQueue` (mutex+CV) |
| SHM send: inter-process IPC only (was never intra-process) | SHM send: unchanged (inter-process IPC only) |
| SAR: duplicado 4× (RAW send, RAW recv, UDP send, UDP recv) | SAR: 1 implementação (NGAL_SAR) |
| Child-thread dispatch: ad-hoc com semáforos | Single dispatcher thread + `NetworkReceiveQueue` → GW thread |

### E7.3 — Ficheiros alterados vs criados

| Ficheiro | Acção |
|----------|-------|
| `Common/src/NGAL_SAR.h` | **CRIAR** |
| `Common/src/NGAL_SAR.cpp` | **CRIAR** |
| `Common/src/NGAL_CS.h` | **CRIAR** |
| `Common/src/NGAL_CS.cpp` | **CRIAR** |
| `PGCS/src/NGAL_Transport_RAW.h` | **CRIAR** |
| `PGCS/src/NGAL_Transport_RAW.cpp` | **CRIAR** |
| `PGCS/src/PG.cpp` | **ALTERAR** (remover ~1500 linhas, adicionar ~30) |
| `PGCS/src/PG.h` | **ALTERAR** (remover declarações de funções eliminadas, remover `TemporaryBuffers1`, `BufferStatus`, `Number_Of_Threads_At_Socket_Dispatcher`) |
| `Common/src/GW.h` | **ALTERAR** (adicionar `NetworkReceiveQueue`, `NetworkReceiveQueueMutex`, `NetworkReceiveQueueCV`) |
| `Common/src/GW.cpp` | **ALTERAR** (drain `NetworkReceiveQueue` in `Gateway()` loop) |
| `CMakeLists.txt` | **ALTERAR** (adicionar `NGAL_SAR.h/.cpp`, `NGAL_CS.h/.cpp` to `add_library(Common ...)` file list; adicionar `NGAL_Transport_RAW.h/.cpp` to `add_executable(PGCS ...)` file list) |

### E7.4 — Perguntas em aberto (Q&A)

| # | Questão | Opções |
|---|---------|--------|
| Q1 | Manter `WriteToSharedMemory3` em PG.cpp ou mover para `NGAL_CS::DeliverToSHM`? | Mover para NGAL_CS — coeso com a subcamada de convergência. **Revised:** `PG::WriteToSharedMemory3` stays in PG.cpp for now (receive-path elimination) — only its *usage* in FinishReceivingThread is removed. `NGAL_CS::DeliverToSHM` is a wrapper that calls `PG::WriteToSharedMemory3`. `GW::WriteToSharedMemory3(OQS, M)` (inter-process send) stays in GW.cpp unchanged. |
| Q2 | `EthernetWiFiSemaphoreName` desaparece com child threads? | Sim — eliminado. Se paralelismo for necessário no futuro, fazer com thread pool NGAL-T |
| Q3 | O `ethframe` union (linha 129) fica em NGAL_Transport_RAW ou mantém-se em PG.cpp? | Mover para NGAL_Transport_RAW — é específico RAW |
| Q4 | `PGRunExposition01` (que coexiste com GWExposition02) também usa NGAL? | Não — PGRunExposition01 é todo intra-processo. Mantém-se |
| Q5 | UDP: manter código ou remover completamente? | **Remover.** Recuperar do git se necessário no futuro. `get_in_addr` também removido (F7) |
| Q6 | How does `SendToARawSocket(Interface, Identifier, Size, M)` delegate to `NGAL_SAR::SendSegmented(Message*, BlockSize, MN, SN, MC, callback)`? (F9) | `SendToARawSocket` body becomes: `NGAL_SAR::SendSegmented(M, BlockSize, MessageNumber, SequenceNumber, MessageCounter, [this, Interface, Identifier](char* data, unsigned int size, unsigned int idx) { return NGAL_Transport_RAW::SendFragment(...); })`. The Interface/Identifier are captured in the lambda closure. |
| Q7 | CMake: how exactly are new files added? (F12) | `NGAL_SAR.h/.cpp` and `NGAL_CS.h/.cpp` are added to `add_library(Common ...)` file list. `NGAL_Transport_RAW.h/.cpp` are added to `add_executable(PGCS ...)` file list. PGCS already links Common (`target_link_libraries(PGCS LINK_PRIVATE Common)`), so `NGAL_SAR`/`NGAL_CS` are available to PGCS via that link. |
| Q8 | Should `Process::NewMessage()` get a mutex? (F2) | **No.** Keep `NewMessage()` single-threaded (GW thread only). `NGAL_CS::DeliverToGateway` pushes raw buffer to `NetworkReceiveQueue`; GW thread does `NewMessage`+deser+`PushToInputQueue`. |

---

## E8 — Glossário NGAL

| Termo | Significado |
|-------|-------------|
| **NGAL** | NovaGenesis Adaptation Layer — camada entre NG messages e transporte físico |
| **NGAL-CS** | Convergence Sublayer — entrega ao GW intra-processo ou via SHM inter-processo |
| **NGAL-SAR** | Segmentation & Reassembly — fragmentação e remontagem de mensagens |
| **NGAL-T** | Transport Sublayer — I/O específico de cada transporte (RAW, UDP, futuro) |
| **NGAL-PDU** | Protocol Data Unit — cabeçalho NGAL (16 bytes) + payload NG |
| **SAR-PDU** | Unidade de segmentação — um fragmento de mensagem com cabeçalho de segmentação |
| **MN** | Message Number — identificador único (32-bit aleatório) para correlação de fragmentos |
| **SN** | Sequence Number — índice do fragmento (0, 1, 2...) |
| **NoS** | Number of Segments — total de fragmentos esperados para uma mensagem |

---

*Fim de SPEC-013.*