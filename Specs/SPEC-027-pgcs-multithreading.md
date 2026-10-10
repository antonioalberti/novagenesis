# SPEC-027 — PGCS Multithreading

**Status:** In Progress
**Reviewed snapshot:** AIOPT5 `1af604dad66148618cc0e578c28adb4a9829e607`; reviewed SPEC Git blob `0a02900a86ebfb9d34861b9244396dc9571790ee` (pre-amendment identity).
**Historical evidence:** The supplied history records directed guard tests 6/6 PASS at `faa6c4e1f606b4bd355697b122d4c682038e1d7a` and Phase 1 TEST SIGNED OFF by Astra at `50e351c9f0ed7aea5658df42902e055dac848843`. These records apply only to their documented source/build identities; ancestry does not establish acceptance of the reviewed AIOPT5 snapshot.
**Current exact-SHA assessment:** Phase 0 PARTIAL; Phase 1 OPEN; Phases 2–4 OPEN; Phase 5 PARTIAL; Phases 6–7 OPEN. PARTIAL does not mean an exit gate passed. Source-level Phase 1 contradictions remain; the supplied packet contains no current-build evidence establishing a complete Phase 0–7 exit gate.
Historical loss observations and deferred diagnostics are not acceptance waivers. The existing normative correctness/performance thresholds and rollout gates remain in force; §8.5 and §10.2 criterion 6 add run-provenance evidence without removing or waiving prior criteria. The owner-approved 2026-09-26 temporal amendment below defines SAR expiry and sender MN/retry bounds without changing the wire format. This status record neither declares SPEC-027 complete nor authorizes implementation or operational actions.
**Target:** NovaGenesis PGCS / NGAL / Gateway IPC  
**Specification file:** `Specs/SPEC-027-pgcs-multithreading.md`  
**Build platform:** Alpine Linux, musl, g++ / GCC 15  
**Validation platform:** Two Alpine VMs, each with 2 vCPUs and 2 GB RAM

---

## 1. Objective

Increase PGCS delivery throughput by introducing configurable receive concurrency and safe parallel SHM writes, without changing NovaGenesis wire formats, message interpretation, or IPC interoperability.

The implementation shall:

- Support **N receive workers** using socket fan-out or exclusive per-socket dispatch.
- Prevent fragments belonging to one message from being lost or mixed across reassembly contexts.
- Reduce contention at the Gateway input boundary without making the message engine generally concurrent.
- Allow independent writes to the four existing SHM segments.
- Preserve a single-worker compatibility configuration.
- Demonstrate improvements through repeatable throughput, loss, and one-way-delay measurements.

Increasing `NUMBER_OF_THREADS_AT_SOCKET_DISPATCHER` alone is not an implementation of this specification.

## 2. Current Architecture and Findings

### 2.1 Receive path

The active path is:

`PGRunInitialization01::Run` → `PG::ReceiveDispatcherWrapper` → `NGAL_Transport_RAW::ReceiveDispatcher` → `NGAL_SAR::ReceiveFragment` → `NGAL_CS::DeliverToGateway`.

A **single dispatcher thread** currently:

1. Polls the SSID collection.
2. Calls `recvmmsg` to receive frames in batches.
3. Removes the Ethernet header.
4. Performs reassembly.
5. Delivers complete byte buffers to GW.

`NGAL_SAR SAR` is stack-local to `ReceiveDispatcher`. Consequently, creating several instances of the existing dispatcher over the same SSIDs would create independent reassembly states while allowing fragments from one message to reach different threads.

The current `NUMBER_OF_THREADS_AT_SOCKET_DISPATCHER=4` is effectively unused for receive parallelism following the NGAL migration.

Exact-snapshot clarifications for AIOPT5 at the reviewed SHA:

- Initialization creates/registers the first receive socket and starts the dispatcher inside the interface-configuration loop; the same `AlreadyCreatedPeerPGCSFrameReceivingIEEEThread` gate also surrounds later socket registration.
- The dispatcher polls a fixed maximum of 32 descriptors, and block-size lookup uses the poll index to access `Sizes[i]`.
- The shown receive path uses a signed `int` result from `recvmmsg` and checks it before processing slots. Each slot rejects truncation/oversize and frames shorter than Ethernet header plus segment header before subtracting the Ethernet header; the earlier `recvfrom` signedness and pre-subtraction findings are historical and are not confirmed as current defects by this snapshot.
- `RawReceivePool::Reset()` is called before each batch. Its complete per-slot address-length reset semantics still require Phase 0 verification.
- The current drain policy permits up to two `recvmmsg` calls per ready descriptor (up to 64 frames when each batch contains 32), while §5.2 normatively limits draining to at most 32 frames per ready descriptor. §5.2 remains unchanged unless the owner explicitly approves a design change.
- Socket creation tests `_SID > 0`, which rejects descriptor zero, and RAW receive compares the EtherType against decimal `13330` rather than the normative byte-order-safe comparison.
- Reassembly cleanup runs only after poll timeouts; continuous traffic can delay cleanup. SAR records `time(0)` per newly accepted fragment, while dispatcher cleanup passes `PGCS::GetTime()` (`CLOCK_MONOTONIC`), so the source's nominal 30-second idle comparison mixes clock epochs and is not a verified elapsed-time timeout. There is no absolute age cap. The receive loop has no stop/wake path, and shutdown joins occur after the SSID collection is destroyed.

The AIOPT5 path uses `recvmmsg` batching within one dispatcher; batching reduces syscall overhead but is not multi-worker receive concurrency. Earlier `recvfrom`-specific findings and any claim about complete `sll_len` reset must be revalidated against the actual batch implementation, not carried forward as current defects without source evidence.

### 2.2 Send path

`PG::SendToARawSocket` creates a local SAR object and passes references to shared PG members:

- `MessageNumber`
- `SequenceNumber`
- `MessageCounter`

`NGAL_Transport_RAW::SendFragment` contains the retry loop: up to 12,000 attempts, with 10 ms sleeps after failures. This is approximately 120 seconds of retry sleeping, excluding syscall time.

The current AIOPT5 baseline has no total-message send deadline and the `sendto()` call is not deadline-bounded. REQ-027-028 below specifies the accepted candidate behavior; the 20-second fragment and 120-second message limits are not claims about the current executable.

Receive parallelism must not accidentally enable concurrent mutation of these send members.

### 2.3 Gateway boundary

The supplied receive code states that `DeliverToGateway` copies a completed buffer and that GW performs `NewMessage`, parsing, and conversion.

This boundary is intentional. Receive workers shall not call `Process::NewMessage`, mutate `Process::Messages`, execute actions, or concurrently use shared message-building state.

The actual `NGAL_CS` and GW queue implementation must be inspected before choosing its synchronization changes.

### 2.4 SHM path

`PG::WriteToSharedMemory3` probes four segments, using keys `11 + z`, named POSIX semaphores, and a single flag byte:

- `'f'`: free for writing.
- `'w'`: published and awaiting consumption.

The existing writer holds a semaphore while inspecting and writing a segment.

Correctness issues to resolve before parallelization include:

- Incorrect `shmat` failure test; failure is `(void*)-1`.
- Retry accounting can acquire a semaphore at the attempt boundary without entering the release path.
- Bounds checking omits the flag byte and trailing timestamp.
- Negative sizes and arithmetic overflow are not rejected before allocation.
- Logging, allocation, and sleeps occur in critical sections.
- Semaphore-open failure triggers `sem_unlink`, potentially disrupting another process.
- SHMID lookup and shared `File` access have not been established as thread-safe.

### 2.5 Source-discovery status and remaining prerequisite

At the reviewed AIOPT5 snapshot, root `CMakeLists.txt` lists `Common/src/NGAL_SAR.h/.cpp` and `Common/src/NGAL_CS.h/.cpp`. The former `PGCS/src` path references are historical discovery notes, not the current source locations.

The supplied AIOPT5 source also locates the raw ingress queue in `Common/src/GW.h/.cpp`, its copying producer in `Common/src/NGAL_CS.cpp`, and part of the SHM reader in `GW::ReadFromSharedMemory3`.

This closes only the demonstrated path-location portion of Phase 0. Phase 0 shall still record the complete build-target/source map, tinythread API, ownership and IPC call-site map, full SHM reader/writer audit, SAR validation/duplicate/replay/timeout behavior, ordering dependencies, baseline identity/results, and peer compatibility fixtures.

Proposed interfaces remain proposed unless their implementation is explicitly demonstrated. Source presence is not test or exit-gate evidence.

## 3. Scope and Non-Goals

### 3.1 In scope

- RAW Ethernet/Wi-Fi receive concurrency.
- Reassembly synchronization and resource limits.
- GW raw-buffer ingress synchronization.
- Thread-safe synchronous SHM writing and a bounded optional writer pool.
- Lifecycle, instrumentation, and Alpine build reliability.

### 3.2 Out of scope

- Changes to EtherType, fragmentation format, SCNs, or command-line semantics.
- General multithreaded execution of GW actions.
- General concurrent access to HT or the Process message registry.
- New network reliability, acknowledgement, or ordering protocols.
- Replacing the SHM flag protocol with a ring buffer.
- Unconditionally parallelizing RAW sends.

## 4. Numbered Requirements

### 4.1 Protocol and compatibility

**REQ-027-001 — Wire compatibility.**  
Retain EtherType `0x1234`, Ethernet framing, segmentation fields, message-size encoding, fragment numbering, and existing SAR completion rules. Protocol comparisons shall use `htons(0x1234)` or `ntohs`, not decimal `13330`.

**REQ-027-002 — IPC compatibility.**  
Retain the four SHM keys, semaphore identity, flag meanings, eight-byte size encoding, payload layout, and eight-byte trailing timestamp encoding. Do not silently change the current timestamp scaling of `GetTime() * 10e9`.

**REQ-027-003 — Compatibility mode.**  
A configuration with one receive worker and synchronous SHM writes shall remain supported. Safety fixes apply in this mode too.

**REQ-027-004 — Ordering.**  
Preserve existing required ordering and GW scheduling semantics. Do not claim global receive order across independent sockets. Phase 0 shall identify any per-peer or per-destination ordering dependency before enabling parallel IPC jobs for that dependency.

### 4.2 Receive ownership

**REQ-027-005 — Explicit worker count.**  
Add `ReceiverWorkers` to `PGCS.ini`, with supported values 1–4 initially and default 1 until acceptance. Test N=1, N=2, and N=4; recommend N=2 on the target VMs only if measurements justify it.

`PG::Number_Of_Threads_At_Socket_Dispatcher` shall hold the effective configured count. Update misleading comments in `PG.h` and `Common/src/Process.h`; do not change a shared macro default for unrelated processes without auditing its consumers.

**REQ-027-006 — Exclusive receive ownership.**  
Each receive descriptor shall have exactly one polling worker. Multiple workers shall not poll the same descriptor.

**REQ-027-007 — Socket topology.**  
Provide two modes:

| Mode | Ownership | Limitation |
|---|---|---|
| `per-socket` | Distinct, interface-bound receive sockets assigned exclusively to workers | One active socket provides no receive-syscall parallelism |
| `fanout` | N compatible AF_PACKET sockets per interface in one `PACKET_FANOUT` group | Requires kernel support and shared/sharded reassembly |

Ordinary duplicate RAW sockets are not load balancing: they can receive copies of the same frame. Fan-out members shall use consistent protocol, interface binding, and group configuration.

**REQ-027-008 — Safe fallback.**  
If fan-out setup fails, close all partially created members and rebuild an exclusive per-socket configuration. Log requested/effective topology and worker count. Never leave a mixture of ordinary duplicate sockets and fan-out members active.

**REQ-027-009 — Immutable receive configuration.**  
Replace positional SSID/size lookup in the hot path with immutable `RawRxSocket` descriptors containing at least:

- File descriptor.
- Interface index.
- Block size.
- Logical receive-domain identifier.
- Fan-out group identity, where applicable.

Duplicate configuration entries for one receive interface shall not create duplicate listeners. Conflicting block sizes shall cause a configuration error.

### 4.3 Reassembly

**REQ-027-010 — Cross-worker fragment safety.**  
All fragments of one message shall address the same logical reassembly state regardless of the receiving worker.

Kernel flow hashing is not sufficient proof of message affinity for custom EtherType `0x1234`. Load-balancing fan-out may intentionally split a message across sockets.

**REQ-027-011 — Reassembly identity.**  
Introduce `ReassemblyKey` containing at least:

- Logical receive domain/interface.
- Source MAC.
- Destination MAC.
- EtherType.
- Existing on-wire message number.

Do not include worker index, fan-out member FD, or fragment sequence number in the message identity. Use sequence number inside the selected message state.

This prevents different senders using the same message number from sharing a buffer.

**REQ-027-012 — Sharded state.**  
Use a PG-owned `RawReassemblyService` with a fixed initial count of 16 shards. Each shard owns a mutex and a map from `ReassemblyKey` to reassembly state.

The first safe implementation may use one SAR instance per message key if compatible with the located SAR implementation. Otherwise refactor SAR to expose explicit per-message state. Preserve its validated wire rules.

**REQ-027-013 — Completion and ownership.**  
While holding the shard lock, process a fragment and atomically extract a completed buffer. Release the lock before GW delivery, logging, allocation outside SAR, or IPC work.

Only one worker may complete a particular active assembly. Duplicate fragments shall not advance completion twice. Replay behavior after completion shall match the verified baseline; this change does not add exactly-once network delivery.

**REQ-027-014 — Validation and limits.**  
Validate frame length, segment-header length, sequence range, declared message size, block size, and all offset arithmetic before copying.

Define bounded incomplete-message count and byte usage, enforced before allocation. Initial aggregate limits shall be 1,024 incomplete messages and 128 MiB of reassembly storage. Limits include actual allocation capacity, not just received bytes.

Oversize, malformed, conflicting, expired, and capacity-rejected assemblies shall have separate counters.

**REQ-027-015 — Expiration.**  
Use one monotonic clock (`std::chrono::steady_clock`) for both timestamps and expiry comparisons; do not mix `time(0)`, wall-clock time, or another clock epoch into an elapsed-time comparison.

For every incomplete assembly, expire it when either (a) no new valid, nonduplicate fragment has been accepted for **30 seconds** (`T_idle`), or (b) **150 seconds** (`T_max`) have elapsed since its SN=0 fragment created the state. `T_max` is absolute and is never refreshed by progress. Only a newly accepted fragment refreshes `T_idle`; duplicate, malformed, rejected, or already-seen fragments do not. Expired assemblies are removed before another fragment can extend them.

Schedule cleanup at least once per second even under continuous traffic. Fragment processing and cleanup shall use the same owning-shard lock; never acquire all shard locks simultaneously. Tests shall cover just-before, exact-boundary, and just-after expiry using an injectable monotonic clock.

### 4.4 GW queue

**REQ-027-016 — Single message-engine owner.**  
GW remains the sole owner of message creation, conversion, normal action dispatch, and existing Process message-registry mutation.

**REQ-027-017 — Bounded MPSC ingress.**  
Provide a bounded multi-producer/single-consumer queue for completed raw buffers. Proposed GW members:

- `RawInputMutex`
- `RawInputCondition`
- `RawInputQueue`
- `RawInputBytes`
- `RawInputClosed`

Initial limits: 4,096 entries and 128 MiB, whichever is reached first.

Keep this raw ingress distinct from any scheduled `Message*` queue.

**REQ-027-018 — Short queue critical sections.**  
Queue locking shall cover admission checks, pointer/descriptor insertion, accounting, and removal only. No parsing, payload copying, logging, SHM access, or action execution shall occur under this lock.

GW shall remove batches of at most 32 entries per acquisition, then process outside the lock while preserving normal scheduling fairness.

**REQ-027-019 — Explicit transfer contract.**  
Add an owned-buffer delivery API alongside the existing copying `NGAL_CS::DeliverToGateway` API.

Proposed `DeliverOwnedToGateway` contract:

- On success, the queue owns the buffer.
- On rejection, ownership remains with the caller.
- The old API retains its documented copy behavior.

Queue-full rejection shall be bounded and counted; receivers shall not block indefinitely.

**REQ-027-020 — Queue measurements.**  
Measure queue lock wait/hold time, occupancy, high-water marks, enqueue rejection, and GW residence time before introducing more complex queue structures.

### 4.5 SHM parallelization

**REQ-027-021 — Segment-level exclusion.**  
Different workers may write different segments concurrently. A segment’s named POSIX semaphore remains the cross-process authority; an in-process mutex cannot replace it.

Every flag read, free check, payload publication, and consumer reset shall occur under that semaphore. Verify the existing reader before enabling parallel writes.

**REQ-027-022 — Publication.**  
While holding the acquired semaphore:

1. Verify that the flag is `'f'`.
2. Copy the size, payload, and timestamp.
3. Set the flag to `'w'`.
4. Release the semaphore.

Never overwrite a `'w'` segment. Startup flag initialization must occur under the same semaphore; unknown flags during normal operation shall be reported rather than silently erasing potentially live data.

**REQ-027-023 — Bounds.**  
Reject negative sizes, invalid buffers, overflow, and insufficient SHM capacity before writing.

For payload size `M`, required physical capacity is:

`1 + 8 + M + 8 = M + 17` bytes.

The existing size field remains `M + 8`. Validate against actual segment capacity obtained from `shmctl(IPC_STAT)` as well as applicable configured limits.

**REQ-027-024 — Semaphore acquisition.**  
Use an explicit acquired flag and scoped cleanup. Every successful `sem_trywait` shall receive exactly one `sem_post`, including the final allowed attempt.

Handle `EINTR`, `EAGAIN`, and permanent errors separately. Scan other segments before sleeping; never sleep or retry while holding a segment semaphore.

Do not call `sem_unlink` on an ordinary writer error.

**REQ-027-025 — Fair selection.**  
Use a synchronized rotating starting index rather than making all writers start at segment zero. Probe all four segments with a bounded retry deadline. A logical write shall publish to at most one segment.

**REQ-027-026 — Writer resources.**  
Resolve SHM IDs on the existing owning thread and publish immutable segment descriptors to writers. Do not call an unaudited `GW::ReturnIPCSHMID` concurrently.

Cache mappings and semaphore handles only after defining invalidation for segment recreation. Close/detach resources after writer joins. Serialize diagnostics separately from segment locks; do not concurrently write through the caller’s shared `File*`.

**REQ-027-027 — Optional writer pool.**  
Retain synchronous `WriteToSharedMemory3` return semantics: `OK` means published, not merely queued.

Add a separate asynchronous submission API for migrated call sites, with a bounded queue, owned payload, and explicit completion result delivered to the owning thread. Use `ShmWriterWorkers` values 0–4, where 0 means synchronous operation.

Start evaluation with two writers. Queue acceptance shall never be counted as completed IPC delivery. Preserve required destination ordering by limiting that ordering domain to one outstanding job where necessary.

### 4.6 Send, lifecycle, and shared state

**REQ-027-028 — Send serialization, bounded deadlines, and message-number ownership.**
Keep RAW sending on its existing owner during receive rollout. Add an explicit send ownership guard or mutex around `PG::SendToARawSocket` for callers that can otherwise overlap.

Allocate one MN once per message and keep its fragment sequence state local to that send. For the same sender/peer context represented by the reassembly key, a new sender shall not reuse an MN for at least **150 seconds after the last possible fragment-transmission attempt**. The allocator shall enforce this across concurrent calls. It shall preserve its quarantine state across process restart; if that state cannot be restored, it shall fail closed and defer RAW sending for at least 150 seconds. Exhaustion shall fail closed rather than reuse a still-quarantined MN. No generation/epoch field is added to the wire format. Legacy peers that choose MN randomly remain outside this deterministic guarantee; their residual collision risk shall be reported.

Bound each fragment-send operation to **20 seconds** from its first attempt, including `sendto()` time, readiness waits, and retries. Bound the complete segmented-message send to **120 seconds** from the first attempt for its first fragment; this deadline is shared across all fragments and is not reset per fragment. Use nonblocking or otherwise deadline-aware I/O so a syscall cannot overrun either deadline. On either deadline, stop sending remaining fragments and return an explicit failure; if any fragment may have been emitted, retain the MN quarantine for the required interval.

These limits intentionally replace the current baseline's up-to-12,000 retries/10 ms **per fragment** with a 20-second per-fragment ceiling and a 120-second **whole-message** ceiling. Both deadlines use the same monotonic clock as REQ-027-015. The 150-second absolute reassembly age is the 120-second message deadline plus one 30-second idle interval.

Do not hold GW queue, reassembly, or SHM locks across `SendSegmented` or its retry loop. Atomics alone on the three existing shared PG members are insufficient.

**REQ-027-029 — Controlled shutdown.**  
Add idempotent `StartReceiveWorkers`, `StopReceiveWorkers`, and corresponding IPC-worker lifecycle methods. Stop flags shall be atomic or mutex-protected, never plain concurrently accessed booleans.

Wake blocked workers, join them, and only then destroy sockets, SAR state, queues, statistics, and PG dependencies. Partial startup failure shall unwind all resources.

**REQ-027-030 — No construction race.**  
All worker-visible PG members shall be initialized before `Run` can launch workers. Receive startup shall occur only after complete socket configuration and confirmation that GW ingress is ready.

Replace the 100 ms thread-creation sleep with explicit lifecycle state.

**REQ-027-031 — Statistics ownership.**  
Keep `OutputVariable`, `StressStats`, and other stream mutation on one owner. Workers shall publish counters through synchronized snapshots or atomics. Existing `StressSent`, `StressReceived`, `StressDropped`, and `StressDelayCount` shall not acquire unsynchronized multiwriter access.

## 5. Proposed Implementation Structure

### 5.1 File and symbol changes

| File | Required changes |
|---|---|
| `PGCS/src/PG.h` | Add runtime configuration, worker lifecycle declarations, receive/IPC runtime ownership, send guard |
| `PGCS/src/PG.cpp` | Initialize runtime state before initialization actions; stop/join before destructor cleanup; refactor `WriteToSharedMemory3`; preserve wrapper compatibility |
| `PGCS/src/PGRunInitialization01.cpp` | Parse new settings; remove the premature `break` after `DelayBetweenExpositions`; construct all sockets before startup; unwind failures |
| `PGCS/src/NGAL_Transport_RAW.h/.cpp` | Add `RawRxSocket`, `RawReceiverContext`, socket binding/fan-out setup, context-based dispatcher, signed receive checks, periodic cleanup |
| `PGCS/src/RawReassemblyService.h/.cpp` — new | Implement `ReassemblyKey`, sharded ownership, budgets, cleanup, completion extraction |
| `PGCS/src/PGShmWriter.h/.cpp` — new | Implement safe segment-write primitive, resource descriptors, optional bounded worker pool |
| Actual build-resolved `NGAL_SAR.h/.cpp` | Add context/per-message reassembly API as needed; retain wire encoding and legacy caller compatibility |
| Actual build-resolved `NGAL_CS.h/.cpp` | Add explicit owned-buffer enqueue API and rejection result |
| `Common/src/GW.h/.cpp`, subject to path verification | Add or adapt raw ingress queue, batching, ownership, shutdown, instrumentation |
| `Common/src/Process.h` | Correct dispatcher macro documentation; avoid broad namespace or registry changes |
| `PGCS/src/PGCS.h/.cpp`, subject to path verification | Reconcile `Threads`, `NoT`, SSID ownership, and destruction with the new runtime |
| `PGCS/src/PGRunStresstest01.cpp` and `PGStresstestPing01.cpp` | Add deterministic workload IDs, unique-delivery accounting, delay samples |
| Build-resolved CMake files | Register new sources, C++ standard, thread linkage, test targets |
| `Scripts/AlpineVMs/pull-and-build-vms.sh` | Propagate actual remote build and background-job failures |
| `Scripts/AlpineVMs/benchmark-pgcs-mt.sh` — new | Run reproducible configuration/workload matrix and save artifacts |

Do not register one thread simultaneously in a PG-owned vector and `PGCS::Threads` as two independent owners.

### 5.2 Receive loop

Each worker shall:

1. Poll only its assigned descriptors and a shutdown wake descriptor.
2. Handle `POLLERR`, `POLLHUP`, and `POLLNVAL` without a busy loop.
3. Reset receive address length before each `recvfrom`.
4. Store the return value as `ssize_t`.
5. Validate length before Ethernet-header subtraction.
6. Verify the protocol and existing destination/outgoing-frame acceptance policy.
7. Build the reassembly key from validated metadata.
8. Call `RawReassemblyService::ReceiveFragment`.
9. Enqueue completed ownership outside all SAR locks.
10. Run scheduled cleanup independently of poll timeout.

Use dynamic poll storage instead of truncating to 32 sockets. Drain at most 32 frames per ready descriptor before rotating, preventing one busy socket from starving others.

### 5.3 Fan-out policy

The initial fan-out implementation shall use `PACKET_FANOUT_LB` with shared sharded reassembly. This explicitly supports fragments landing on different workers.

`PACKET_FANOUT_HASH` may be evaluated later, but shall not replace shared reassembly solely on an assumption of custom-protocol hash affinity.

Group setup shall avoid collision with other PGCS instances and isolate distinct interfaces. Runtime socket-group resizing is out of scope; stop and rebuild the group instead.

### 5.4 Lock discipline

| Resource | Synchronization | Forbidden while held |
|---|---|---|
| Reassembly shard | `tthread::mutex` | GW enqueue, SHM, send, stream logging |
| GW raw ingress | `tthread::mutex` | Parsing, copying payloads, action execution |
| IPC job queue | `tthread::mutex` | Semaphore waits, payload publication |
| SHM segment | Named POSIX semaphore | Queue locks, sleeps, logging |
| RAW send ownership | Owner confinement or dedicated mutex | Acquisition by receive workers |
| Statistics output | Single owner | Worker direct stream mutation |

Prefer non-nested locking. Any unavoidable new nesting requires a documented order and a test covering shutdown.

## 6. Alpine musl / GCC 15 Compatibility

**REQ-027-032 — Thread API.**  
Use explicit `tthread::thread`, `tthread::mutex`, `tthread::lock_guard`, `tthread::condition_variable`, and supported `tthread::this_thread` calls. Verify the bundled tinythread signatures before implementation.

`std::atomic` and standard ownership containers are permitted. Do not introduce unqualified `thread` or `mutex`, or depend on `using namespace tthread`.

**REQ-027-033 — Build configuration.**  
Use the project’s explicit C++ standard, at least C++11 for these changes, and CMake `Threads::Threads` linkage. Validate with Alpine GCC 15 rather than assuming a glibc build proves compatibility.

Include required headers directly. Avoid tests against libc-private include guards such as `_STRING_H` and `_UNISTD_H` in touched code. Audit mixed `linux/if.h` and `net/if.h` inclusion to avoid redefinitions.

**REQ-027-034 — Platform correctness.**  
Use documented POSIX/Linux return values and types:

- Socket descriptor zero is valid: test `fd >= 0`.
- `shmat` failure is `(void*)-1`.
- `sem_open` failure is `SEM_FAILED`.
- Receive results use `ssize_t`.
- Capacity arithmetic uses checked unsigned operations.

Verify semaphore naming on musl. Prefer POSIX slash-prefixed names, but prove they resolve to the same peer-visible semaphore identity or coordinate both reader and writer changes.

**REQ-027-035 — Trustworthy build script.**  
Fix `pull-and-build-vms.sh` so:

- Remote commands run with a known shell and reliable failure handling.
- `cmake` and `make` failures cannot be hidden by `tail`.
- `wait` status is captured without `|| true` overwriting it.
- Both VMs build the same recorded commit.
- Full configure/build logs are saved.
- Dirty worktrees are reported and not silently confused with the measured revision.

Use a dedicated release benchmark build with identical flags before and after. Build all existing binaries, not PGCS alone.

## 7. Phased Implementation Plan

### Phase 0 — Inventory and baseline

1. Locate SAR, CS, GW, tinythread, CMake targets, SHM readers, and existing SPEC conventions.
2. Document SAR keying, timeout, duplicate behavior, ownership, and all IPC call sites.
3. Record the baseline commit and toolchain versions.
4. Run the unmodified baseline workload matrix.

**Exit gate:** Source-path map, ownership map, raw baseline results, and peer compatibility fixtures are available.

### Phase 1 — Single-thread safety repairs

1. Fix receive signedness, frame validation, protocol comparison, descriptor-zero handling, and length resetting.
2. Fix SHM attachment checks, semaphore accounting, bounds, and cleanup.
3. Fix INI parsing and build-script failure reporting.
4. Keep effective receive count at one.

**Exit gate:** Malformed-input, semaphore-boundary, SHM-capacity, and interoperability tests pass. Report performance separately from the original baseline.

### Phase 2 — Lifecycle and immutable socket descriptors

1. Add runtime/context ownership and wakeable shutdown.
2. Build sockets before thread launch.
3. Remove SSID/size positional assumptions and unused socket leaks.
4. Preserve compatibility wrappers without creating duplicate workers.

**Exit gate:** Repeated start/stop and injected partial-startup failures show stable descriptor/thread counts and no use-after-free.

### Phase 3 — Sharded reassembly

1. Introduce metadata-aware keys and shared service.
2. Integrate first with one receive worker.
3. Add expiration under continuous traffic and memory budgets.
4. Test synthetic cross-worker fragment submissions.

**Exit gate:** Interleaved senders, identical message numbers, reordered and duplicate fragments, timeout, and capacity tests pass without mixed payloads or duplicate completion of an active assembly.

### Phase 4 — Multiple receive workers

1. Enable per-socket ownership.
2. Add bound AF_PACKET fan-out groups.
3. Enable N=2 and N=4.
4. Test fallback and interface isolation.

**Exit gate:** Genuine fan-out traffic reaches multiple workers, produces no duplicated ingress from duplicate listeners, and passes fragmented-message integrity tests.

### Phase 5 — GW ingress contention reduction

1. Instrument the existing queue.
2. Introduce bounded owned-buffer ingress.
3. Add bounded batch removal.
4. Keep message conversion and execution single-threaded.

**Exit gate:** Queue ownership and saturation tests pass; lock wait/hold distributions and throughput changes are recorded.

### Phase 6 — Safe SHM concurrency

1. Validate synchronous concurrent calls on different segments.
2. Add rotating segment selection and immutable resource descriptors.
3. Add optional writer pool and completion reporting.
4. Migrate only audited callers.

**Exit gate:** Concurrent local and external writers do not corrupt or overwrite unread messages. Busy-peer and shutdown tests terminate within configured bounds.

### Phase 7 — Two-VM acceptance and rollout

1. Run the complete benchmark matrix.
2. Select the supported configuration from measurements.
3. Commit results and rollback settings.
4. Retain default N=1 if performance or correctness gates fail.

**Exit gate:** Section 10 acceptance criteria are met and reproducible.

## 8. Validation and Metrics

### 8.1 Environment

| Role | Address | Resources |
|---|---|---|
| Repository VM | `<repository-guest-ip>` | 2 vCPUs, 2 GB RAM |
| Source VM | `<source-guest-ip>` | 2 vCPUs, 2 GB RAM |

Record kernel, Alpine, musl, GCC, CMake, commit, optimization flags, NIC/MTU, socket buffer settings, logging level, CPU utilization, RSS, and VM scheduling/steal time.

Run traffic in both directions separately and bidirectionally. Exclude deployment and build activity from measurements.

### 8.2 Workload matrix

For each supported configuration, test:

- Small messages that fit in one NGAL fragment.
- Payloads immediately around the configured segmentation boundary.
- 16 KiB and 64 KiB messages.
- Interleaved logical senders and repeated message numbers across senders.
- A sustained mixed-size workload.
- A stalled SHM consumer.
- Injected malformed, missing, reordered, and duplicate fragments.
- Reassembly progress just below, at, and beyond `T_idle=30 s`; progress that stays active past `T_max=150 s`; duplicates/malformed frames must not refresh timers; continuous traffic must not postpone scheduled cleanup.
- Fragment-send failure at the 20-second deadline and segmented-message failure at the 120-second total deadline, including proof that blocked I/O is bounded and later fragments are not emitted after expiry.
- Same-context MN quarantine under concurrent sends, partial-send failure, restart with/without persisted allocator state, counter wrap/exhaustion, and no wire-format change. Keep legacy random-MN peers as an explicitly residual-risk compatibility case.

Configurations:

1. Original baseline.
2. Safety-fixed N=1, synchronous SHM.
3. N=2 fan-out, synchronous SHM.
4. N=4 fan-out, synchronous SHM.
5. N=2 with two SHM writers.
6. N=4 with four SHM writers.
7. Per-socket mode with sufficient independent receive sockets.

Use 30 seconds warm-up, at least 120 seconds measured traffic, and five repetitions per throughput configuration. Include a 30-minute soak.

### 8.3 Metric definitions

**Delivery throughput**

Report unique complete application deliveries per second and useful payload MiB/s. Also report:

- Successfully submitted sends.
- Complete SAR assemblies.
- GW ingress admissions.
- GW conversions.
- SHM publications.
- Final consumer deliveries.

Do not treat fragments or queue admissions as application delivery.

**Stress-test loss**

Use run ID and message ID from an application-level stress workload without changing the transport header.

`Loss % = 100 × (successfully sent unique messages − received unique messages) / successfully sent unique messages`

Stop transmission and allow a documented drain interval before reconciliation. Report submission failures, duplicates, late arrivals, and stage-specific drops separately.

**One-way delay**

Measure sender application timestamp to final receiving application delivery. Report median, p95, and p99.

Synchronize VM clocks and record measured offset/error before and after each run. Target uncertainty below 1 ms and below 10% of the claimed improvement. If unmet, mark one-way-delay conclusions inconclusive; RTT/2 is not an equivalent substitute.

Use monotonic clocks for same-host stage timings.

### 8.4 Before/after results table

Populate from committed artifacts; values below are intentionally unmeasured.

| Metric | Original baseline | Fixed N=1 | N=2 + 2 SHM writers | N=4 + 4 SHM writers |
|---|---:|---:|---:|---:|
| Delivery throughput, messages/s | TBD | TBD | TBD | TBD |
| Useful payload throughput, MiB/s | TBD | TBD | TBD | TBD |
| Stress-test loss, % | TBD | TBD | TBD | TBD |
| One-way delay median, ms | TBD | TBD | TBD | TBD |
| One-way delay p95, ms | TBD | TBD | TBD | TBD |
| One-way delay p99, ms | TBD | TBD | TBD | TBD |
| GW ingress lock wait p95, µs | TBD | TBD | TBD | TBD |
| GW ingress residence p95, ms | TBD | TBD | TBD | TBD |
| SHM publication latency p95, ms | TBD | TBD | TBD | TBD |
| Process CPU / peak RSS | TBD | TBD | TBD | TBD |

Store configuration, raw samples, summary statistics, and packet captures under a revision-labelled SPEC-027 results directory.

### 8.5 Per-run identity and provenance

Each correctness or performance run shall have a versioned, machine-readable SPEC-027 run record. The record shall link the measured outcome to the exact source, build, configuration, workload, environment, and evidence artifacts; it shall not import the NG-ELC schema or its acceptance semantics.

At minimum, the record shall include:

- **Run identity and purpose:** unique run ID; SPEC revision/blob; phase and requirements exercised; classification as `diagnostic`, `correctness`, or `performance`; repetition number; and explicit comparison-arm identity.
- **Source and executable identity:** exact source commit and tracked/dirty state for each participant VM; selected dirty-diff identity where applicable; effective compiler/toolchain and build flags/macros; build command/result and complete log identities; and SHA-256 identities of the actual executables used on each VM.
- **Effective configuration:** requested and effective worker count/topology, socket/interface settings, MTU and receive buffers, block size and resource budgets, SAR `T_idle`/`T_max`, per-fragment and whole-message send deadlines, MN allocator/quarantine policy, SHM mode, logging profile, peer versions, workload seed, direction, and offered load.
- **Measurement window:** warm-up, measured start/end, repetitions, sender-stop/drain interval, reconciliation point, and clock offset/uncertainty before and after each run when one-way delay is reported.
- **Counter semantics and stage outcomes:** counter scope, unit, increment point, reset/overflow behavior, and attribution to run/process/worker/stage; distinguish successfully submitted unique sends, complete SAR assemblies, GW admissions, conversions, SHM publications, and final unique consumer deliveries. Admission or fragment counts are not application delivery counts.
- **Failures and resources:** submission failures, malformed/oversize/conflicting/expired/capacity-rejected assemblies, queue rejection, duplicates and late arrivals as separate outcomes; plus the applicable queue occupancy/high-water/residence, lock wait/hold, SHM publication latency, CPU, RSS, and VM scheduling/steal measurements.
- **Evidence linkage:** relative artifact paths, sizes, and SHA-256 identities for configuration, workload inputs, logs, raw samples, summaries, captures, and test reports; an explicit requirement → test/run → artifact → result mapping.

File identity/snapshot helpers may process only explicitly selected regular files under an approved evidence root, with bounded file/count limits, deterministic paths, duplicate/out-of-root rejection, and a tested policy for symlinks and files changing during hashing. Hashes identify bytes; they do not by themselves prove which binary executed or establish performance.

Verbose DEBUG output is diagnostic-only and shall not be used as performance-acceptance evidence. A diagnostic run and a performance run are distinct records/build identities; the `targeted-debug` RAW/SAR profile shall not be enabled for the measured performance matrix. Missing runtime or compiled-state evidence remains `UNKNOWN`, not `PASS`.

### 8.6 Diagnostic-only PGCS cross-VM smoke

A single bounded PGCS-only smoke may be used to check the original AIOPT5 peers' discovery path before the full Phase 0 baseline is ready. It is diagnostic evidence only and is not a Phase 0 exit gate.

- Run one unmodified PGCS binary per guest, Repository and Source, at the SPEC's target resources. Use the baseline PGCS configuration; do not enable stress generation or alter the wire format. Do not launch NRNCS or ContentApp.
- The smoke is limited to socket readiness, peer discovery/receiver-handler evidence, and small PGCS control/hello messages. It does not test application payload delivery, fragmentation, SHM, concurrency, throughput, loss, latency, or any Phase 1–7 criterion.
- Use a source-pinned, explicitly approved remote controller plan. The plan must be marked diagnostic-only and must not restart/stop a VM as a side effect. If its execution path can invoke `qm stop`, force-stop, or an unbounded restart, do not run it. Any restart requires a separately approved graceful lifecycle path.
- Start roles under an identifiable process-group supervisor. Record VM/role, command, PID/PGID and process identity, start/stop times, timeout, signal escalation, and child exit status. Stop only the identity-verified process group; collect its durable raw logs before any cleanup.
- Before launch, verify each guest independently against the exact source SHA, resources, interface/MTU/MAC, binary and configuration identity, process state, and IPC baseline. Cleanup is limited to explicitly authorized project state after its ownership and contents have been inventoried; never remove unrelated processes or IPC.
- Require both socket markers and a receiver-side peer-discovery marker. Preserve a bounded bridge capture filtered to EtherType `0x1234` and verify the expected Source↔Repository MAC pairs. Socket creation or process liveness alone is not discovery; missing peer-handler or bidirectional on-wire evidence is `READINESS-INCONCLUSIVE`.
- Create the SPEC-027 §8.5 run record with classification `diagnostic`, exact source/build/config/controller/plan identities, raw evidence hashes, and explicit untested edges. A controller's diagnostic result or schema does not replace or extend §8.5, and cannot be used to claim correctness or performance acceptance.

The full Phase 0 exit gate remains the source/ownership map, peer-compatibility fixtures, and raw results for the complete §8.2 matrix, including five repetitions per throughput configuration and the 30-minute soak. This smoke satisfies none of those requirements by itself.

### 8.7 Targeted DEBUG follow-up for peer-discovery diagnosis

A separate, bounded `diagnostic` follow-up may investigate the inconclusive §8.6 peer-discovery result. It does not amend, replace, or retroactively complete the §8.6 run, and it satisfies no Phase 0 acceptance gate.

- **Permitted source delta:** only enable existing commented `#define DEBUG` switches and add side-effect-free diagnostic log statements compiled exclusively under `#ifdef DEBUG`. No changes to conditions, data structures, state transitions, scheduling, timing, queues, IPC, socket behaviour, message contents, wire format, or configuration. The candidate files are `PGCS/src/PGRunPeriodic01.cpp`, `PGCS/src/PGRunHello01.cpp`, `PGCS/src/NGAL_Transport_RAW.cpp`, `Common/src/NGAL_SAR.cpp`, `PGCS/src/PGHelloIHC01.cpp`, and `PGCS/src/PGRunExposition01.cpp`. Do not enable broad `PGCS/src/PG.cpp` diagnostics or `DEBUG1`/`DEBUG2`/other verbose profiles under this scope.
- **Diagnostic chain:** record whether the periodic path invokes IHC hello generation; the outbound IHC 0.1 target; raw send/receive; SAR completion or rejection; entry into the receiver's IHC 0.1 handler and the known/unknown/self/duplicate decision; and whether the peer tuple is actually appended. The current discovery text is emitted before `PGCSTuples.push_back`; add a separate DEBUG-only confirmation after that insertion. Record whether `PGRunExposition01` subsequently sees the peer tuple and reaches its peer-HT lookup/exposition path. Do not treat a pre-insertion marker as proof of completed registration.
- **Identity and capture:** classify the run as `diagnostic` and assign a new run ID. Bind each guest's exact source commit, selected DEBUG-only diff, enabled macro list, build command/toolchain, binary SHA-256, configuration, controller and plan identities in a new §8.5 record. Preserve stdout and stderr (RAW/SAR diagnostics write to `stderr`) as separate hashed artifacts. Keep the baseline PGCS configuration and `StressTest 0`; do not launch NRNCS or ContentApp. Keep the observation bounded to 150 seconds and ensure the filtered EtherType `0x1234` capture spans the entire observation window; hitting any byte/frame cap early makes that evidence incomplete.
- **Review and execution gate:** this DEBUG build is not an “unmodified PGCS binary” under §8.6. Do not alter C++ or launch either guest until this amendment is reviewed and approved. Before any follow-up guest run, resolve the existing controller HOLD with deterministic non-NovaGenesis fixtures covering raw-versus-parsed IPC reconciliation, identity-safe process/IPC teardown, and independent final-seal verification, and obtain the required review and explicit run authorization. No VM stop/restart is permitted by this diagnostic scope.

The full Phase 0 exit gate remains the source/ownership map, peer-compatibility fixtures, and raw results for the complete §8.2 matrix, including five repetitions per throughput configuration and the 30-minute soak. Neither §8.6 nor §8.7 satisfies those requirements by itself.

### 8.8 Single-worker Gateway characterisation and low-overhead instrumentation (N=1)

**Owner decision (2026-09-30):** multithread receive work is deferred for the current article path; the article's performance claim will first be examined on the current single-worker configuration. The owner authorised this in-place amendment to specify bounded stage instrumentation. REQ-027-005 and the full Phase 0 acceptance remain unchanged. **Owner decision (2026-10-01):** the owner authorises one bounded, diagnostic N=1 validation on VMs 101 and 102 after the local source/build/test gates and guest preflight pass. The permitted VM scope is exact-source guest build verification followed by at most one PGCS-only diagnostic smoke to check whether telemetry snapshots and RAW/SAR/GW counters are emitted. No stress/load campaign, photo/payload run, NRNCS or ContentApp launch, overhead claim, production optimisation, multi-worker receive, or Phase 0 acceptance is authorised by this decision. Use a source-pinned plan/controller, preserve guest-local state, and stop if preflight, identity, cleanup, or evidence gates fail; use graceful VM shutdown only. The amendment does not authorise any broader runtime trial.

**Owner decision (2026-10-02):** After the single diagnostic smoke on 2026-10-02 returned `teardown=FAIL` and `evidence=INCOMPLETE`, the owner authorizes exactly one remediation verification after the controller correction, exact-source guest build, local tests/review, and guest preflight pass. The additional run remains PGCS-only and diagnostic, uses a fresh trial ID and evidence root, and verifies telemetry emission plus controller teardown/evidence behavior. It does not authorize NRNCS/ContentApp, payload, stress/load, overhead/performance claims, production optimisation, multi-worker receive, or Phase 0 acceptance. Preserve the earlier sealed trial and locks; do not release them or reuse their evidence root. If any preflight, identity, cleanup, or evidence gate fails, do not launch.

#### What the later work established

- Commit `e8692dd1a96ab17d2a7f28513efdd200c5f0b789` implemented GW input-queue batching on 2026-09-07. Commit `77efe27fafbd7483507ed1448cbeedf04782bff4` restored the exact pre-SPEC-032 tree on 2026-09-09. At AIOPT5 HEAD `1af604dad66148618cc0e578c28adb4a9829e607`, `GW.cpp` has subsequent safety guards but no retained SPEC-032 queue-batching implementation. SPEC-033 remains a Draft and explicitly defers throughput optimisation.
- The 2026-09-07 single-worker `recvmmsg` report for build `c638292` at 2,000 msg/s reports no sampled drops/guard hits but a transient Repository outstanding count of 2,762, later sampled at 2. The report attributes the backlog to GW input processing; its old build/config/workload linkage is insufficient for current AIOPT5 acceptance.
- A 2026-09-09 post-restoration 2000 msg/s diagnostic sampled a Gateway queue peak of 10,002 and final queues of 2/265. Its offered/received rates were approximate, heartbeat boundaries asynchronous, and work remained queued; the report explicitly forbids calling it zero loss or a soak.
- The 2026-09-10 B3 audit traced buffer ownership and found the copy into `NetworkReceiveQueue` occurs under `NetworkReceiveQueueMutex`, while GW drains and deserialises entries under that mutex. It was a read-only ownership audit, not a performance profile. The current source also processes scheduled input one message per scheduler iteration. These are candidates for measurement, not proof that any one is the current bottleneck.

Therefore, Gateway congestion has been investigated and experimentally suspected in historical work, but it has not been conclusively localised or fixed on the current AIOPT5 exact baseline. Treat the old rates/backlogs as hypotheses, not current performance evidence.

#### Existing telemetry and limits

At AIOPT5 HEAD `1af604dad66148618cc0e578c28adb4a9829e607`, `PG::EmitHeartbeat` emits `SPEC028_STATS` every 10 seconds when `StressTest=1`. It includes `offered`, `sent_ok`, `received`, `completed`, `dropped`, `guard_reject`, `outstanding`, and the process-wide `sar_*` counters. Preserve that line's existing field names/order for current consumers; publish the new stage metrics under a separate `SPEC027_STAGE_STATS` prefix.

- `offered` and `sent_ok` identify successful admissions to GW input queues, not successful socket transmission. `received` counts entry into the stress-ping application action. `completed` and `sar_*` cover all SAR traffic in the process, not stress messages alone. `outstanding` is only a point-in-time `InputQueue.size()` snapshot; it does not include `NetworkReceiveQueue` or messages currently being processed.
- `dropped` and `guard_reject` are derived sums of SAR event counters. They are diagnostic event counts, can overlap, and are not counts of unique lost application messages. Do not use the legacy `StressDropped` field as a loss measure.
- The heartbeat has no `NetworkReceiveQueue` depth, byte count, residence time, or lock timing; no scheduler service-time distribution; and no SHM semaphore-wait or operation-latency counters. The Draft text of SPEC-028 does not fully describe the telemetry now present in source; use the pinned source for this measurement and do not silently change the existing heartbeat contract.

The network receive path is in-process: SAR completion → `NGAL_CS::DeliverToGateway` → `NetworkReceiveQueue` → deserialisation → `InputQueue` → GW action. SHM is a separate inter-process path, not a mandatory stage after every RAW receive. Measure SHM reads and writes separately so its counters cannot be mistaken for network-ingress work.

#### Stage instrumentation contract

Add exact cumulative counters and current/high-water gauges at existing ownership boundaries. Capture sampled durations with `std::chrono::steady_clock`; use fixed-size, bounded histograms and per-thread/component-local sampling state. Initially time one in every 128 eligible events per calling thread, report sample count and histogram bucket bounds, and derive approximate p50/p95/p99. The sampling rate may be reduced if the overhead gate below fails. Do not allocate per event, add a blocking telemetry lock, or emit per-message telemetry.

| Stage | Required signals | Purpose |
|---|---|---|
| RAW receive | Frames/bytes returned by the receive call; NG frames/bytes accepted after protocol and length validation; non-NG, truncated, and too-short outcomes; sampled receive/validation duration | Distinguish socket intake and filtering from later SAR/GW work. Retain existing `rx_truncated`/`rx_too_short` counters. |
| SAR | Existing completion/rejection counters; current and high-water incomplete assembly count/charged bytes; sampled `ReceiveFragment` duration | Detect reassembly work or incomplete-message accumulation without confusing process-wide transport counts with stress-only delivery. |
| GW raw ingress | Enqueued/dequeued entries and bytes; current/high-water entries and bytes; sampled enqueue-to-dequeue residence; producer and consumer mutex wait/hold; buffer-copy and per-entry message-creation/deserialisation duration; rejection/error outcomes | Localise backlog, copy cost, lock contention, and consumer service at `NetworkReceiveQueue`. Preserve the single GW consumer. |
| GW scheduler | `InputQueue` admissions, dispatches and completions; current/high-water depth; sampled enqueue-to-action-start residence, scheduled lateness, action service time, and existing input-lock wait/hold | Distinguish scheduler backlog from raw-ingress backlog and slow action execution without changing the priority comparator or scheduling semantics. |
| SHM / IPC | Separate read and write polls/attempts, segments probed, semaphore-busy/retry counts, successful messages/bytes, failures by reason, current/high-water output-queue depth, and sampled semaphore wait and read/write duration | Determine whether inter-process IPC is delaying its own path. Do not infer SHM congestion from a RAW/GW counter gap alone. |
| End-to-end | Preserve `offered`, `sent_ok`, `received`, and existing stress delay/loss outputs; record start/end deltas and drain state | Reconcile the diagnostic stages with final application delivery. Existing one-way delay depends on clock synchronisation and is not a per-stage timing. |

Instrument queue depth/bytes while holding the queue's existing mutex; collect timing only at the sampled rate. For `NetworkReceiveQueue`, separately time the producer's copy and lock section, the consumer's lock wait/hold, and the message work performed while the consumer currently holds that lock. Do not move parsing, copying, or lock boundaries in this instrumentation change. For scheduled messages, attach the monotonic enqueue timestamp without changing the existing `(GetTime(), GetTag())` priority/order contract; audit every enqueue, pop, retry, and deletion path before choosing the internal representation. For sampled queue residence, make the sampling decision before reading `std::chrono::steady_clock`; unsampled enqueue/dequeue operations shall not read the clock, and a default argument that evaluates `now()` per event is prohibited. For SysV SHM attachment, treat `shmat` result `(void*)-1` as failure, increment the corresponding attach-error counter once, and do not dereference, semaphore-process, or detach that failure sentinel.

Counters are cumulative for the process; gauges are point-in-time/high-water. Use deltas between recorded snapshots, preserve run identity through §8.5, and include units, scope, sample rate, and enabled macros in the run record. Sample host CPU and RSS externally at a fixed interval and bind the samples to the same run. Keep stress-only and process-wide counters explicitly separate.

#### Diagnostic build and overhead gate

Start from a clean worktree at the pinned AIOPT5 baseline and apply only the reviewed instrumentation diff. Do not use the current dirty worktree for performance measurements. The measurement binary shall have general `DEBUG` and all per-message queue logging disabled. At the pinned AIOPT5 baseline, both `#define DEBUG_NETWORK_QUEUE` lines in `Common/src/GW.cpp` are commented out; the sole `#ifdef DEBUG_NETWORK_QUEUE` logging site is therefore disabled at compile time. Do not change runtime behavior to satisfy the stale contrary description. Before any measurement, verify the exact source macros and captured stderr for each arm; keep general `DEBUG` and all per-message queue logging disabled and identical. If the source baseline changes, re-check this premise and amend this SPEC before measuring.

Emit one bounded `SPEC027_STAGE_STATS` snapshot every 10 seconds and one final snapshot after a documented drain; no per-message logs. For the overhead comparison, both arms shall use the pinned source with `DEBUG_NETWORK_QUEUE` disabled as verified above and identical remaining logging settings; that logging state is a shared prerequisite, not instrumentation overhead. Compare matched runs with stage telemetry disabled/enabled, using identical source, build flags, workload, environment, warm-up, measurement window, and drain procedure except for the instrumentation switch. The instrumentation overhead must be no more than 2% in end-to-end throughput and p95 delay; otherwise reduce sampling or remove costly signals and repeat the comparison. Retain the §8.5 run record and raw samples. Any result intended as article or Phase 0 performance evidence must still satisfy the applicable §8.2 repetitions and soak; this diagnostic gate alone is not acceptance.

#### Localisation and decision rules

Use repeated, drained runs and at least three consecutive 10-second snapshots at steady load; a single queue sample or a gap between cumulative counters is not a bottleneck diagnosis.

- Suspect RAW/SAR only when receive/SAR service and resource use rise while downstream queue depth/residence remains stable and the finding repeats.
- Suspect GW raw ingress when `NetworkReceiveQueue` depth/bytes/residence grow, producer lock wait rises, or consumer lock/service time dominates.
- Suspect scheduler/action service when raw ingress drains but `InputQueue` depth, residence, or scheduled lateness grows.
- Suspect SHM only when its own semaphore waits/retries or read/write duration are high and its corresponding IPC output backlog or delayed reads corroborate the result.
- If counters do not reconcile after drain, classify the run as inconclusive; do not relabel the residual as loss or claim a stage bottleneck.

Do not repeat SPEC-032 batching merely because old queue samples were high. If the N=1 evidence identifies a bottleneck, propose the smallest targeted correction under this SPEC and compare the same workload before/after. Only measured receive saturation with downstream headroom may justify reopening the deferred worker-count experiment. Full §8.2 repetitions/soak and Phase 0 acceptance remain separate; no result from this characterisation waives them.

## 9. Risks and Mitigations

| Risk | Consequence | Mitigation |
|---|---|---|
| Multiple dispatchers consume one socket with local SAR | Fragment splitting and loss | Exclusive descriptor ownership plus shared sharded SAR |
| Ordinary duplicate RAW listeners | Duplicate message delivery | Interface binding, fan-out groups, atomic fallback |
| Custom EtherType has poor flow-hash distribution | One worker remains hot | LB fan-out with message-keyed shared state |
| Message number collision or wrap | Assemblies mixed | New sender's 150-second quarantine, restart/wrap tests, and sender/domain keying; legacy random-MN peers remain a documented residual risk; no invented wire epoch |
| GW remains CPU-bound | Little end-to-end gain | Stage metrics, short ingress locks, retain single-owner semantics |
| Four workers oversubscribe two cores | Higher delay and context switches | Benchmark N=1/2/4; do not equate more threads with improvement |
| Large messages exhaust 2 GB RAM | OOM or unstable delay | Byte/count budgets, actual-capacity accounting |
| SHM writer publishes before complete copy | Partial consumer read | Semaphore-protected copy and final flag publication |
| Retry boundary leaks semaphore | Permanent IPC stall | Acquired-state cleanup and injected boundary tests |
| Stale SHMID or semaphore identity mismatch | Split IPC coordination | Audited resource refresh and musl peer tests |
| Legacy per-fragment RAW retry can outlive idle expiry or block send/shutdown | Partial assemblies, unbounded delay, or slow termination | Enforce 20-second per-fragment and 120-second whole-message monotonic deadlines with deadline-aware I/O and cancellation |
| Hidden build failure | Testing stale binaries | Correct shell status propagation and artifact hashes |
| Unsynchronized clocks | Misleading delay gains | Clock-error reporting and inconclusive-result policy |

## 10. Acceptance Criteria

### 10.1 Correctness

1. Existing peers exchange unfragmented and fragmented messages without wire-format changes.
2. Existing SHM readers consume messages written by the new writer and vice versa.
3. N=1, N=2, and N=4 configurations pass integrity tests.
4. Same-number messages from different senders never mix.
5. Cross-worker fragments complete correctly.
6. No corruption, duplicate active-assembly completion, semaphore leak, descriptor leak, or unread-SHM overwrite occurs.
7. Reassembly, GW ingress, and IPC job memory remain bounded during overload.
8. Worker shutdown completes within two seconds in injected blocked-poll, busy-SHM, and send-failure tests after cancellation is requested.
9. Process message creation and action execution remain on the existing GW owner.
10. Incomplete reassembly expires at the 30-second idle threshold or the 150-second absolute age, whichever occurs first; progress cannot extend the absolute age and wall-clock jumps do not alter expiry.
11. A fragment send cannot exceed 20 seconds and a segmented message cannot exceed 120 seconds total; timeout cancels remaining fragments, returns failure, and preserves required MN quarantine after any possible emission.
12. Same-context MNs are not reused within the 150-second quarantine across concurrent calls, process restart, or counter wrap/exhaustion, while the on-wire format remains byte-compatible. Legacy random-MN peers are reported as residual risk, not as deterministically protected.

### 10.2 Build and diagnostics

1. Clean GCC 15/musl builds succeed on both VMs for all existing binaries.
2. New code has no unresolved compiler warnings.
3. Tests run with ASan/UBSan where supported.
4. Race detection is run on a compatible environment; sanitizer runtime limitations on musl are documented rather than counted as passes.
5. Build-script failure injection produces a nonzero overall result for either VM.
6. Every correctness/performance result used for acceptance has a SPEC-027 run record satisfying §8.5, with source/build/configuration identity and referenced artifact hashes independently checked against the recorded files and executables.

### 10.3 Performance

The following are engineering targets, not measured claims:

1. At a selected receive-limited fragmented workload, N=2 shall improve median maximum sustainable end-to-end throughput by at least **20%** over safety-fixed N=1.
2. Sustainable throughput means stress-test loss **≤0.1%** with no continuously growing backlog.
3. At a fixed offered load of 80% of the safety-fixed N=1 sustainable rate:
   - Loss shall remain ≤0.1%.
   - One-way p95 shall not regress by more than 10%, accounting for clock uncertainty.
4. N=1 safety fixes shall not regress throughput by more than 5% without a documented correctness-related explanation and approval.
5. The 30-minute soak shall show stable memory usage after warm-up and no persistent worker imbalance or runaway queue growth.

If the workload is sender-, GW-, or SHM-limited, report that bottleneck explicitly. Microbenchmark speedups do not replace end-to-end acceptance. A missed performance target blocks changing the deployment default, not recording the result.

## 11. Rollout and Rollback

Initial deployment configuration:

- `ReceiverWorkers 1`
- `ReceiverMode per-socket`
- `ShmWriterWorkers 0`

Candidate two-core configuration after acceptance:

- `ReceiverWorkers 2`
- `ReceiverMode fanout`
- `ShmWriterWorkers 2`

Startup shall print requested and effective topology, receive socket ownership, worker counts, resource budgets, and SHM mode.

Rollback shall disable fan-out and the writer pool through configuration and restart PGCS without changing peer software or SHM layout. Retain the safety repairs and instrumentation.

SPEC-027 is complete only when the implementation, compatibility tests, two-VM build logs, measured before/after results, and selected deployment configuration are committed together.