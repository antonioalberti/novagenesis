# SPEC-027 — PGCS Multithreading

**Status:** In Progress — Phase 1 (AMEND-3) Implemented — Pending Test (commit 1396c62; soak 30 min @500 msg/s passed, record in Specs/RESULTS-SPEC-027/). Phase 2 (AMEND-1/2/5/6) approved on separate branch. Telemetry/guard-test debt in SPEC-028.  
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
2. Calls `recvfrom`.
3. Removes the Ethernet header.
4. Performs reassembly.
5. Delivers complete byte buffers to GW.

`NGAL_SAR SAR` is stack-local to `ReceiveDispatcher`. Consequently, creating several instances of the existing dispatcher over the same SSIDs would create independent reassembly states while allowing fragments from one message to reach different threads.

The current `NUMBER_OF_THREADS_AT_SOCKET_DISPATCHER=4` is effectively unused for receive parallelism following the NGAL migration.

Additional issues visible in the supplied code:

- Initialization starts a dispatcher before socket setup has fully finished.
- `AlreadyCreatedPeerPGCSFrameReceivingIEEEThread` gates socket registration as well as thread creation.
- Additional successfully created SSIDs can be left unregistered and unclosed.
- The dispatcher silently limits polling to 32 sockets.
- `Sizes[i]` assumes that poll order and configuration order match.
- `recvfrom` returns a signed result, but the code casts it to unsigned before error checking.
- Ethernet header subtraction occurs before validating the frame length.
- `sll_len` is not reset before every receive.
- Reassembly cleanup runs only on poll timeouts; continuous traffic can prevent cleanup.
- There is no explicit receiver stop/join lifecycle.

### 2.2 Send path

`PG::SendToARawSocket` creates a local SAR object and passes references to shared PG members:

- `MessageNumber`
- `SequenceNumber`
- `MessageCounter`

`NGAL_Transport_RAW::SendFragment` contains the retry loop: up to 12,000 attempts, with 10 ms sleeps after failures. This is approximately 120 seconds of retry sleeping, excluding syscall time.

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

### 2.5 Source-discovery prerequisite

`PGCS/src/NGAL_SAR.h` and `PGCS/src/NGAL_CS.cpp` were not present at the supplied paths. Their implementations, GW queue members, and existing SPEC examples were not supplied.

Phase 0 shall locate their actual build-resolved paths and record them. Names introduced below are proposed interfaces, not claims about undisclosed existing members.

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
Use monotonic time for internal expiration and preserve the verified SAR timeout duration. Schedule cleanup at least once per second even under continuous traffic. Fragment processing and cleanup shall use the same shard lock; never acquire all shard locks simultaneously.

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

**REQ-027-028 — Send serialization.**  
Keep RAW sending on its existing owner during receive rollout. Add an explicit send ownership guard or mutex around `PG::SendToARawSocket` for callers that can otherwise overlap.

Do not hold GW queue, reassembly, or SHM locks across `SendSegmented` or its retry loop. A future concurrent sender must allocate message numbers once per message and use local fragment sequence state; atomics alone on the three existing members are insufficient.

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
| Repository VM | `192.168.0.61` | 2 vCPUs, 2 GB RAM |
| Source VM | `192.168.0.36` | 2 vCPUs, 2 GB RAM |

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

## 9. Risks and Mitigations

| Risk | Consequence | Mitigation |
|---|---|---|
| Multiple dispatchers consume one socket with local SAR | Fragment splitting and loss | Exclusive descriptor ownership plus shared sharded SAR |
| Ordinary duplicate RAW listeners | Duplicate message delivery | Interface binding, fan-out groups, atomic fallback |
| Custom EtherType has poor flow-hash distribution | One worker remains hot | LB fan-out with message-keyed shared state |
| Message number collision or wrap | Assemblies mixed | Sender/domain keying, bounded lifetime, collision tests; no invented wire epoch |
| GW remains CPU-bound | Little end-to-end gain | Stage metrics, short ingress locks, retain single-owner semantics |
| Four workers oversubscribe two cores | Higher delay and context switches | Benchmark N=1/2/4; do not equate more threads with improvement |
| Large messages exhaust 2 GB RAM | OOM or unstable delay | Byte/count budgets, actual-capacity accounting |
| SHM writer publishes before complete copy | Partial consumer read | Semaphore-protected copy and final flag publication |
| Retry boundary leaks semaphore | Permanent IPC stall | Acquired-state cleanup and injected boundary tests |
| Stale SHMID or semaphore identity mismatch | Split IPC coordination | Audited resource refresh and musl peer tests |
| Long RAW retry blocks shutdown | Slow termination | Preserve runtime retries; add explicit shutdown cancellation and bounded nonblocking send handling |
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

### 10.2 Build and diagnostics

1. Clean GCC 15/musl builds succeed on both VMs for all existing binaries.
2. New code has no unresolved compiler warnings.
3. Tests run with ASan/UBSan where supported.
4. Race detection is run on a compatible environment; sanitizer runtime limitations on musl are documented rather than counted as passes.
5. Build-script failure injection produces a nonzero overall result for either VM.

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