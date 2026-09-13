# SPEC-028: Stress Telemetry Heartbeat for PGCS

**Version:** 0.1
**Date:** 2026-09-06
**Author:** Hermes Agent (on behalf of Astra/gpt-6-astra review)
**Status:** Draft — user must approve before implementation
**Branch:** AIOPT3
**Related:** SPEC-027-pgcs-multithreading.md (Phase 1 soak), /tmp/codex-mt-spec/astra_soak_verdict.md item 2 (SPEC-028-B minimum)

---

## 1. Problem

The SPEC-027 Phase 1 soak demonstrated 30-minute process survival and bounded RSS at the configured 500 msg/s, but Astra's verdict explicitly records that **progress and coarse accounting were unverified**: "zero guard hits" is unverified, and there is no way to observe a stalled or livelocked process from the outside. The existing stress-test accounting has the following gaps:

1. **No periodic telemetry.** Counters (`StressSent`, `StressReceived`, `StressDropped` in `PGCS/src/PG.h:280-282`) are printed only once, in the `PG::~PG()` destructor (`PGCS/src/PG.cpp:246-247`). A hung process never prints.
2. **DEBUG-gated visibility.** The most informative existing lines (SAR reassembly completion, guard rejects in `Common/src/NGAL_SAR.cpp`) are compiled behind `#ifdef DEBUG` or are WARN lines only. With DEBUG off, the operator sees nothing.
3. **No separation of transport receipt from application completion.** `StressReceived` is incremented at application level (`PGStresstestPing01.cpp:68`); there is no counter for transport-level reassembly completion (`COMPLETE MN` in `Common/src/NGAL_SAR.cpp:523` region) or for guard rejects. A transient gap between sent and received cannot be attributed to in-flight work vs. loss.
4. **`StressDropped` is dead.** It is initialised (`PG.cpp:170`) and printed, but never incremented anywhere in `PGCS/src/`.
5. **Counters are plain integers.** After SPEC-027 Phase 1 introduced the GW input queue mutex/condvar structure and with multiple threads touching stress counters, unsynchronised `++` on `unsigned long long` is a data race.

## 2. Proposed Solution

One background heartbeat thread in PG that wakes every 10 seconds and emits a single flushed, greppable line to `stderr`:

```text
SPEC028_STATS run=<run-scn-or-t0> pid=<getpid()> elapsed_s=<int> offered=<N> sent_ok=<N> received=<N> completed=<N> dropped=<N> guard_reject=<N> outstanding=<N>
```

Constraints (all from Astra verdict item 2):

- **Active with DEBUG off.** The heartbeat is not compiled behind `#ifdef DEBUG`. It is gated only on `StressEnabled` (the heartbeat is stress-telemetry; PGCS otherwise stays silent).
- **Cumulative counters** since process start (monotonically non-decreasing).
- **No per-message logging, no histograms, no metrics framework, no new service.** One line every 10 seconds, `std::cerr` + `std::flush`. Nothing else.
- **No behaviour change** to the data path: counters are updated inline at existing points; the heartbeat only reads them.

### 2.1 Counter definitions and counting boundaries

Units are **messages** (complete stress-ping application messages) unless stated otherwise. Every counter is `std::atomic<unsigned long long>` with `memory_order_relaxed` updates (individual counters only; no multi-counter consistency is promised across a single line).

| Field | Meaning (exact boundary) | Existing variable | Insertion point | Touching threads |
|---|---|---|---|---|
| `offered` | Messages handed to `PGW->PushToInputQueue()` by `PGRunPeriodic01::StresstestScheduling()` (scheduled stress pings offered into the GW input queue), counted immediately after a successful push. | **new** `StressOffered` (PG member, beside `StressSent`) | `PGCS/src/PGRunPeriodic01.cpp`, end of the per-message loop body (after `PushToInputQueue`), and PG constructor init at `PGCS/src/PG.cpp` near line 168 | GW worker thread executing the `-run --periodic` action |
| `sent_ok` | Stress ping messages pushed to the GW input queue by `PGRunStresstest01::Run` (i.e. accepted for sending), counted immediately after the successful `PushToInputQueue`. This reuses the existing counter; boundary unchanged. | `StressSent` (`PG.h:280`) | existing increment `PGCS/src/PGRunStresstest01.cpp:160` — type only changes to atomic | GW worker thread executing `-run --stresstest` |
| `received` | **Application receipt**: `PGStresstestPing01::Run` entered for a stress ping with `StressEnabled` true. Reuses the existing counter; boundary unchanged. | `StressReceived` (`PG.h:281`) | existing increment `PGCS/src/PGStresstestPing01.cpp:68` — type only changes to atomic | GW worker thread executing `-stresstest --ping` |
| `completed` | **Transport receipt**: a stress message fully reassembled by the SAR layer — the `COMPLETE MN` return path of `NGAL_SAR::ReceiveFragment` (the branch at `Common/src/NGAL_SAR.cpp` returning 0 after `CompletedBuffer` handoff, near line 523). This is the boundary between "reassembled and handed to `NGAL_CS::DeliverToGateway`" and "delivered". A transport completion does **not** imply application processing happened. | **new** `SARCompleted` (PG member; SAR gets a raw pointer/reference set at dispatcher setup, or the counter lives in NGAL_SAR and PG reads it — Phase 0 of implementation shall choose whichever touches fewer files) | `Common/src/NGAL_SAR.cpp`, in the COMPLETE branch before `return 0` | Receive dispatcher thread(s) (SPEC-027: 1–4 workers) |
| `dropped` | Messages **abandoned at transport receive**: `INVALID_MN=0` (`NGAL_SAR.cpp:292`) and `FRAME_TOO_SHORT` (`NGAL_SAR.cpp:301`) increments, plus one increment per reassembly buffer deleted by `CleanupTimedOut()` with `SegmentsSoFar < NoS`. Boundary: fragments that will never reach GW. Send-path scheduling failures are **not** here (they manifest as `offered − sent_ok`). | **new** `SARDropped` (PG member, same wiring as `SARCompleted`) | `Common/src/NGAL_SAR.cpp` at the three cited sites | Receive dispatcher thread(s); cleanup runs on the dispatcher poll-timeout path |
| `guard_reject` | Sum of SAR guard rejects: `SN_OUT_OF_RANGE` (`NGAL_SAR.cpp:416`), `DUPLICATE SN` (`:439`), `BLOCKSIZE_MISMATCH` (`:427`), `FRAME_TOO_SHORT` (`:301`), `OUT_OF_BOUNDS` (`:451`), `BUFFER_LIMIT_REACHED` (`:354`). One counter, incremented at each of the six sites. Note `FRAME_TOO_SHORT` increments both `dropped` and `guard_reject` — that is intended (different questions: "was it lost?" vs "did a guard fire?"). | **new** `SARGuardRejects` (PG member, same wiring) | `Common/src/NGAL_SAR.cpp`, six cited sites | Receive dispatcher thread(s) |
| `outstanding` | Point-in-time snapshot, **not** cumulative: number of application messages currently queued in `GW::InputQueue` awaiting GW processing. Read as `InputQueue.size()` under `InputQueueMutex` (same lock used by `GW::PushToInputQueue` at `Common/src/GW.cpp:242` and the consumer at `GW.cpp:466-489`). | existing `GW::InputQueue` | read-only in heartbeat | heartbeat thread (read), GW producer/consumer threads (write) |

Derived, printed-only fields (no new counters):

- `run`: a fixed per-process identifier — the PGCS self-certifying name (`PB->PP->GetSelfCertifyingName()`), captured once at heartbeat start. `pid`: `getpid()`. `elapsed_s`: integer seconds since heartbeat thread start (`time(0)` delta). These make lines from different runs/pids greppable and orderable.
- Sanity relation for the operator (not enforced): `completed ≈ sent_ok` on the peer after network transit; `received ≤ completed`; `outstanding > 0` persistently with flat `received` indicates a GW-side stall, not loss. Per Astra: **do not label transient counter differences "loss"** without accounting for in-flight work (`sent_ok − received` on the receiver, plus queue/reassembly backlog).

### 2.2 Heartbeat thread

- New members in `PG` (`PGCS/src/PG.h`): the six new atomic counters, `std::thread HeartbeatThread`, `bool HeartbeatStop` (atomic), and a start timestamp.
- Started at the end of `PG::Initialization` (or `PG::Run` initialization path) **only if** `StressEnabled`; joined/stopped in `PG::~PG()` before the final `[StressTest] Final:` line is printed.
- Loop: sleep 10 s (in 1 s slices checking `HeartbeatStop`), then format and emit:

```cpp
std::cerr << "SPEC028_STATS run=" << RunId << " pid=" << getpid()
          << " elapsed_s=" << Elapsed
          << " offered=" << StressOffered.load(std::memory_order_relaxed)
          << " sent_ok=" << StressSent.load(std::memory_order_relaxed)
          << " received=" << StressReceived.load(std::memory_order_relaxed)
          << " completed=" << SARCompleted.load(std::memory_order_relaxed)
          << " dropped=" << SARDropped.load(std::memory_order_relaxed)
          << " guard_reject=" << SARGuardRejects.load(std::memory_order_relaxed)
          << " outstanding=" << Outstanding
          << std::endl; // std::endl flushes
```

- Field order is exactly as specified by Astra and must not be reordered (consumers grep on the fixed prefix `SPEC028_STATS` and parse `key=value` pairs).
- The `outstanding` read takes `InputQueueMutex` briefly. If the lock is contended, the heartbeat waits — do not add a try-lock-and-skip (a skipped field would break the fixed format).

### 2.3 Thread-safety analysis

Threads touching each counter after this SPEC:

| Counter | Writers | Readers | Safety mechanism |
|---|---|---|---|
| `StressOffered`, `StressSent` | GW worker thread(s) running periodic/stresstest actions | heartbeat | `std::atomic<unsigned long long>`, relaxed |
| `StressReceived` | GW worker thread(s) running the ping action | heartbeat, `PGStresstestPing01` loss-rate reporting (`PGStresstestPing01.cpp:96-124`), `PG::~PG()` | atomic. Note: `%` and division on the loaded value remain correct; the existing `StressReceived % 100` sampling is unchanged |
| `SARCompleted`, `SARDropped`, `SARGuardRejects` | receive dispatcher thread(s) (1–4 per SPEC-027) | heartbeat, `PG::~PG()` | atomic; if the counters live in `NGAL_SAR` instances, Phase 0 must guarantee a single SAR instance is shared (SPEC-027 made the SAR shared across workers) or aggregate over instances |
| `GW::InputQueue.size()` | GW producers/consumers under `InputQueueMutex` | heartbeat | read under `InputQueueMutex` |
| `HeartbeatStop` | `PG::~PG()` (writer), heartbeat thread (reader) | — | `std::atomic<bool>` |

Non-goals: no batching, no sharded counters, no seqlocks. Relaxed atomics on `unsigned long long` are sufficient — individual counter monotonicity is the only guarantee claimed, and no counter is read against another for decisions.

## 3. Files Affected

| File | Change |
|---|---|
| `PGCS/src/PG.h` | Add atomic counter members (`StressOffered`, `SARCompleted`, `SARDropped`, `SARGuardRejects`), change `StressSent`/`StressReceived` to atomics, add heartbeat thread members and stop flag |
| `PGCS/src/PG.cpp` | Initialise new counters; start/stop heartbeat thread; emit one final `SPEC028_STATS`-shaped line (prefix `SPEC028_FINAL`) in `PG::~PG()` alongside the existing final summary |
| `PGCS/src/PGRunPeriodic01.cpp` | Increment `StressOffered` after each successful `PushToInputQueue` in `StresstestScheduling` |
| `PGCS/src/PGRunStresstest01.cpp` | Existing `StressSent++` unchanged except atomic type |
| `PGCS/src/PGStresstestPing01.cpp` | Existing `StressReceived++` unchanged except atomic type |
| `Common/src/NGAL_SAR.cpp` | Increment `SARCompleted`/`SARDropped`/`SARGuardRejects` at the sites listed in §2.1 (one added line each; no change to control flow) |
| `Common/src/GW.cpp` | None (heartbeat reads `InputQueue` under the existing `InputQueueMutex`) |

No wire-format, protocol, or IPC changes. No new files except, at most, a small `PGHeartbeat` helper if the reviewer prefers `PG.cpp` not to grow.

## 4. Acceptance Criteria

1. With `StressEnabled=true` and DEBUG **off**, PGCS emits exactly one `SPEC028_STATS ...` line to stderr every 10 seconds (±1 s), from process start until shutdown.
2. The line is flushed: `grep SPEC028_STATS <stderr-capture>` shows the line within one heartbeat interval of the events it reports, including when stderr is redirected to a file.
3. Field names and order match the Astra-specified format exactly; all values are decimal integers.
4. All seven counter fields are monotonically non-decreasing across a run; `outstanding` may go up and down.
5. Counters are race-free under TSAN (or, if TSAN is unavailable on the musl build, under a 30-minute 500 msg/s soak with no torn/implausible values — e.g. `received > completed` by more than the in-flight backlog).
6. `guard_reject` reflects each of the six guard conditions; a directed test (or fault injection at build time) that triggers at least `SN_OUT_OF_RANGE` and `DUPLICATE SN` shows the increment on the next heartbeat.
7. `completed` counts only SAR `COMPLETE MN` events; `received` counts only `PGStresstestPing01::Run` invocations; the two remain distinguishable in a run with artificial GW-side delay.
8. `dropped` increments on `INVALID_MN=0`, `FRAME_TOO_SHORT`, and timed-out reassembly cleanup; a 30-minute soak shows `dropped` consistent with (and explaining) any `completed < sent_ok` gap.
9. With `StressEnabled=false`, PGCS emits no `SPEC028_STATS` lines and behaviour is otherwise identical.
10. No per-message log lines, histograms, or new metrics framework were added; diff touches only the files in §3.
11. `PG::~PG()` still prints the existing `[StressTest] Final:` line, followed by one `SPEC028_FINAL` line with the same field layout.
12. The heartbeat thread is joined at shutdown (no leaked thread, no output after destruction).

## 5. Rollback Plan

Revert the single implementation commit. The heartbeat is additive: no data-path code depends on the counters (they are only incremented and read). Removing the heartbeat thread start/stop and reverting the atomic typedefs restores the exact prior behaviour. `StressSent`/`StressReceived` semantics are unchanged, so the existing `StressTest_Stats.txt` output and loss-rate calculation are unaffected by rollback.

## 6. Pitfalls

1. **SAR instance identity.** SPEC-027 moved toward a shared SAR. If the implementation instead leaves per-dispatcher SAR instances, per-instance counters must be aggregated before printing, or completions will be undercounted. Verify during Phase 0 of the implementation.
2. **`FRAME_TOO_SHORT` double counting.** It increments both `dropped` and `guard_reject`. Reviewers diffing counters must expect this; document it in a code comment at the site.
3. **Lock ordering.** The heartbeat takes `InputQueueMutex` while GW workers hold it frequently at 500 msg/s. The heartbeat must take it once per 10 s and never while holding any other lock. Do not sample other state inside the same critical section.
4. **Destructor ordering.** The heartbeat must be stopped before `PG::~PG()` touches members it reads (e.g. before printing the final counters), or it may read freed memory during shutdown.
5. **`endl` vs `\n`.** `\n` does not flush; the heartbeat must use `std::endl` (or explicit `std::flush`) or the "greppable within one interval" acceptance criterion fails when stderr is file-redirected (fully buffered).
6. **Not "loss".** `offered − received` at any instant includes scheduled-but-not-yet-sent, in-network, reassembling, and queued-in-GW messages. Tooling and humans reading the lines must not report this delta as loss (Astra verdict item 2, last sentence).
7. **`StressReceived % 100` sampling** in `PGStresstestPing01.cpp` still reads the atomic; make sure the change to atomic does not alter the sampled-frequency behaviour (load once, use the local).
8. **musl/GCC 15.** `std::atomic<unsigned long long>` is lock-free on the target platforms, but verify with `is_lock_free()` in a one-time debug line rather than assuming; if not lock-free on some target, relaxed atomics still work but heartbeat reads may briefly contend.
9. **heartbeat gating.** Gating on `StressEnabled` means guard-reject telemetry is absent when stress is off. This is accepted for SPEC-028 minimum scope (stress telemetry); general NGAL telemetry is out of scope and must not be snuck in.
