# SPEC-032 — GW Input Queue Batching

- **Status:** Draft; Hermes implements, verifies, and benchmarks.
- **Target:** GW processing backlog at 2000 msg/s bidirectional.
- **Scope:** Bounded input extraction, bulk network admission, synchronization, and comparable backlog telemetry.
- **Dependencies:** Retain SPEC-028-B heartbeat, SPEC-031 receive batching, and permanent SPEC-MUSL-001.
- **Delivery:** Specification only; no implementation diffs.

## 1. Evidence and objective

The measured SPEC-031 run sustained approximately 7.7 minutes at 2000 msg/s bidirectional with zero observed loss, but repository guest outstanding peaked at **2762**, returning to **2 within approximately 130 seconds**.
The RX path kept up; GW processing accumulated work.
The current consumer pops only one due message per outer loop despite its “pop all” comment.
Each message therefore incurs repeated queue checks, mutex operations, scheduling, and outer-loop housekeeping.
Network admission also deserializes an unbounded queue while holding `NetworkReceiveQueueMutex` and locks/notifies separately for each input push.

Batching amortizes that overhead and limits producer lock blocking.
It does not make `Run()` itself faster; improvement is a hypothesis requiring measurement, not a guaranteed throughput gain.

**Required result:** At 2000 msg/s, reduce backlog peak by at least 50%, maintain zero loss, and reduce drain time using identical measurement definitions.

## 2. Exact consumer design

1. Set default `InputBatchLimit = 32`; support test values 1, 16, and 32.
2. Use fixed-capacity or preallocated local storage; no batch-container allocation under `InputQueueMutex`.
3. At the existing due-message extraction point, acquire `InputQueueMutex` once.
4. Capture one scheduler-time snapshot using the existing `GetTime()` clock.
5. Pop up to the limit, repeatedly selecting `InputQueue.top()` while its time is **strictly less than** the snapshot, preserving the existing eligibility test.
6. Leave future messages queued; never wait to fill a batch.
7. Transfer extracted entries to consumer-local pending-work accounting before unlocking.
8. Release the mutex and invoke the existing per-message execution path sequentially in extraction order.
9. Preserve existing `Run(PM1, PM2)` response handling and per-message cleanup semantics; do not introduce concurrent `Run()` calls.
10. After one batch, service bounded network admission and the SHM poll deadline, then begin another cycle.
11. A nonempty due queue must not incur an intentional sleep between batches.
12. Stop is checked between messages. On stop, reinsert unexecuted local entries under the mutex with their original scheduling keys, then follow existing shutdown policy.
13. Poll SHM when its existing interval expires even under sustained due work; do not retain the current starvation-prone `!hasDueMessage` gate.

The limit bounds work between housekeeping opportunities, not execution time: one slow handler can still delay progress.

## 3. Ordering contract

`InputQueue` is a **priority queue**, not a general FIFO: ascending `GetTime()`, then ascending `GetTag()`.
Do not replace it with `std::queue` or discard scheduled execution.

- Preserve priority order within each extraction snapshot.
- Preserve FIFO admission order for equal-time messages using unique, monotonic tags assigned under `InputQueueMutex`.
- Serialize concurrent admissions by lock acquisition; bulk entries receive consecutive tags in producer order.
- Newly admitted messages do not preempt an already extracted batch.
- Thus global priority interleaving differs from single-pop behavior when a newly admitted message has an earlier timestamp; this bounded snapshot behavior requires scheduler-test approval.
- FIFO does not mean insertion order across different scheduled times.
- Time and tag must remain immutable while an entry is queued or locally pending.
- Audit unsigned-tag wraparound; existing ordering must not be claimed valid across wrap. Verify test headroom and document a separate widening fix if required.

## 4. Admission and notification design

Keep `PushToInputQueue(Message*)` as the single-entry interface, delegating to a bounded bulk-admission helper.

- Validate pointers and command-line counts outside `InputQueueMutex`; retain the existing `NoCL > 2` acceptance rule.
- A null pointer is rejected without dereferencing it; fix the current null-path `MarkToDelete()` defect.
- For accepted messages, assign tags, increment `InputQueueTag`, and push under one lock.
- Move `SetTag(InputQueueTag)` inside that lock; its current unlocked read can race with concurrent producers.
- Preserve `UnmarkToDelete()` protection before publishing accepted messages.
- Record whether the queue was empty before the first accepted push.
- Unlock, then issue **one `InputQueueCV.notify_one()` only for an empty→nonempty transition**.
- An all-rejected batch produces no notification. No caller may hold `InputQueueMutex` when invoking either public admission interface.
- SHM and action-generated single messages remain supported; do not hold SHM buffers or semaphores merely to accumulate a batch.

For network admission, extract at most 32 raw entries in FIFO order under `NetworkReceiveQueueMutex`, then unlock.
Deserialize outside both queue mutexes and bulk-admit accepted messages without waiting for additional traffic.
Leave excess raw entries for subsequent outer-loop turns; never nest the two queue mutexes.

## 5. Waiting and scheduling

The current wait predicate treats any future queued message as ready and can spin.
Use a due-work/stop predicate and a bounded timed recheck instead.

- Under `InputQueueMutex`, examine stop, due work, and the next scheduled deadline.
- Bound idle/future waits by the existing SHM polling interval; use steady-clock durations for recheck deadlines.
- Release the mutex during the wait; on timeout or wake, recompute readiness and service housekeeping.
- Empty→nonempty admission wakes promptly.
- Because earlier arrivals into a nonempty future queue intentionally do not notify, their extra detection delay is bounded by the poll interval.
- Do not sleep until a distant future deadline without that bound.
- Validate this scheduling tradeoff with timer and p99 tests; sub-poll-interval wake guarantees would require a separately approved notification policy.
- Synchronize stop publication with the input wait protocol and notify all relevant existing waiters; verify shutdown cannot lose a wake.

## 6. Ownership and cleanup

- Raw buffers transfer from ReceiveDispatcher to the raw queue, then to the GW local admission batch.
- GW releases each extracted buffer with `delete[]` exactly once, including malformed input and allocation failure paths.
- Confirm `SetMessageFromCharArray`/conversion retains no borrowed buffer after release; the excerpt alone does not prove that contract.
- `Message*` remains Process-managed: queue/local containers hold scheduling responsibility, not permission to call `delete`.
- Accepted queued and locally pending messages must remain protected from `PP->DeleteMarkedMessages()`.
- After each `Run`, retain existing mark-and-reclaim behavior; never blindly mark a message that was forwarded or rescheduled.
- Rejected nonnull messages follow existing Process marking/reclamation policy.
- Hermes must audit handler/GC behavior: if another handler can mark a locally pending message, establish a valid lifetime pin before batching or block rollout.
- Early returns, supported exceptions, and stop must release raw buffers and return or retain every unexecuted message; never silently discard local work.

## 7. Thread-safety and observability

Known accessors are GW consumption, GW network/SHM admission, GW action callbacks, and PG heartbeat.
Other threads may call public admission; Hermes must enumerate all call sites before approval.
ReceiveDispatcher touches the raw queue, not `InputQueue`; the output worker uses its separate queue synchronization.

- Guard all input queue operations, sizes, tag assignment, and local-pending accounting with `InputQueueMutex`.
- Heartbeat reads a coherent pair under that mutex: `queued` and `local_pending` (including the active handler).
- Define processing outstanding as **queued + local_pending**; extraction must not manufacture a backlog reduction.
- Decrement pending once each execution completes, without dereferencing a potentially reclaimed message.
- Publish queued depth separately and audit SPEC-028-B’s existing outstanding formula before comparing results.
- Obtain DEBUG queue-size snapshots under the appropriate mutex; do not iterate output queues unlocked.
- Do not hold queue locks during deserialization, `Run`, GC, logging, or buffer destruction.
- Audit shared logging and message lifetime independently; queue locking alone does not make them thread-safe.

## 8. Verification and benchmark protocol

Hermes records commits, build flags, batch limits, hardware, direction/rate interpretation, payload mix, offered totals, and workload timing.
Baseline is current SPEC-031 with batching disabled; use identical telemetry instrumentation in baseline and candidate.

1. Run at least three paired baseline/candidate trials at the measured **2000 msg/s bidirectional** workload and approximately 7.7-minute offer window.
2. Retain 10-second heartbeat logs; additionally capture exact outstanding high-water under the queue lock and timestamped drain samples at no worse than one-second resolution.
3. Reconstruct the historical 130-second interval’s start event. If unavailable, treat it as context, not a directly comparable stopwatch measurement.
4. For paired trials, define drain time as last stress admission to outstanding ≤2 continuously for 10 seconds, with all stress IDs delivered.
5. Report peak and drain time on both peers; no hiding a regression by moving work into local or raw queues.
6. Measure per-direction end-to-end p99 latency using the same clock-safe method in both builds; report clock synchronization uncertainty where applicable.
7. Verify offered-at-admission, sent success, received unique IDs, duplicates, gaps, dropped, and guards after drain.
8. Repeat the 14-minute 500 msg/s bidirectional soak; require zero loss and no timer, shutdown, memory, or CPU regression.
9. Test equal-time FIFO, future scheduling, earlier arrivals, partial batches, concurrent producers, invalid inputs, reentrant admission, GC, and stop with pending work.

**Acceptance:** Every paired trial must show at least 50% lower outstanding peak on the bottleneck peer, zero loss on both peers, and shorter drain time.
Against a reproduced 2762 baseline, the peak ceiling is **1381**.
Require p99 no worse than 10% above paired baseline, no duplicate deliveries, and no growing raw-queue backlog.
Publish all trial results, not only the best; unchanged processing cost or failed gates means no performance acceptance.

## 9. Rollback, portability, and adjacent defect

Provide one rollback switch restoring legacy extraction/admission/wait behavior; batch size 1 alone is not full rollback.
Drain and stop before switching builds; do not migrate live local batches.
Retain null safety, race fixes, and honest telemetry where independently verified.
Rollback on ordering violations, loss, lifetime faults, sustained latency regression, or failed performance gates.

Use existing C++ mutex/condition-variable support; add no glibc-only APIs or dependency on condition-variable fairness.
Keep SPEC-MUSL-001 permanent and SPEC-031’s portability guards unchanged.
Build and soak on the actual musl target; verify timed waits, stop notification, linking, and memory use.
Run race/lifetime tooling on a supported host if unavailable on musl; host tooling does not replace target benchmarks.

**NRNCS verdict:** Serving old 288KB content for a publication expected to contain new 9.9KB content is a credible cache-coherency defect candidate in SPEC-020/022/023 lineage, not evidence of transport loss.
Confirm publication identity and invalidation semantics in a separate reproduction; do not include cache changes in SPEC-032.
# SPEC-032 — GW Input Queue Batching

- **Status:** Draft; Hermes implements, verifies, and benchmarks.
- **Target:** GW processing backlog at 2000 msg/s bidirectional.
- **Scope:** Bounded input extraction, bulk network admission, synchronization, and comparable backlog telemetry.
- **Dependencies:** Retain SPEC-028-B heartbeat, SPEC-031 receive batching, and permanent SPEC-MUSL-001.
- **Delivery:** Specification only; no implementation diffs.

## 1. Evidence and objective

The measured SPEC-031 run sustained approximately 7.7 minutes at 2000 msg/s bidirectional with zero observed loss, but repository guest outstanding peaked at **2762**, returning to **2 within approximately 130 seconds**.
The RX path kept up; GW processing accumulated work.
The current consumer pops only one due message per outer loop despite its “pop all” comment.
Each message therefore incurs repeated queue checks, mutex operations, scheduling, and outer-loop housekeeping.
Network admission also deserializes an unbounded queue while holding `NetworkReceiveQueueMutex` and locks/notifies separately for each input push.

Batching amortizes that overhead and limits producer lock blocking.
It does not make `Run()` itself faster; improvement is a hypothesis requiring measurement, not a guaranteed throughput gain.

**Required result:** At 2000 msg/s, reduce backlog peak by at least 50%, maintain zero loss, and reduce drain time using identical measurement definitions.

## 2. Exact consumer design

1. Set default `InputBatchLimit = 32`; support test values 1, 16, and 32.
2. Use fixed-capacity or preallocated local storage; no batch-container allocation under `InputQueueMutex`.
3. At the existing due-message extraction point, acquire `InputQueueMutex` once.
4. Capture one scheduler-time snapshot using the existing `GetTime()` clock.
5. Pop up to the limit, repeatedly selecting `InputQueue.top()` while its time is **strictly less than** the snapshot, preserving the existing eligibility test.
6. Leave future messages queued; never wait to fill a batch.
7. Transfer extracted entries to consumer-local pending-work accounting before unlocking.
8. Release the mutex and invoke the existing per-message execution path sequentially in extraction order.
9. Preserve existing `Run(PM1, PM2)` response handling and per-message cleanup semantics; do not introduce concurrent `Run()` calls.
10. After one batch, service bounded network admission and the SHM poll deadline, then begin another cycle.
11. A nonempty due queue must not incur an intentional sleep between batches.
12. Stop is checked between messages. On stop, reinsert unexecuted local entries under the mutex with their original scheduling keys, then follow existing shutdown policy.
13. Poll SHM when its existing interval expires even under sustained due work; do not retain the current starvation-prone `!hasDueMessage` gate.

The limit bounds work between housekeeping opportunities, not execution time: one slow handler can still delay progress.

## 3. Ordering contract

`InputQueue` is a **priority queue**, not a general FIFO: ascending `GetTime()`, then ascending `GetTag()`.
Do not replace it with `std::queue` or discard scheduled execution.

- Preserve priority order within each extraction snapshot.
- Preserve FIFO admission order for equal-time messages using unique, monotonic tags assigned under `InputQueueMutex`.
- Serialize concurrent admissions by lock acquisition; bulk entries receive consecutive tags in producer order.
- Newly admitted messages do not preempt an already extracted batch.
- Thus global priority interleaving differs from single-pop behavior when a newly admitted message has an earlier timestamp; this bounded snapshot behavior requires scheduler-test approval.
- FIFO does not mean insertion order across different scheduled times.
- Time and tag must remain immutable while an entry is queued or locally pending.
- Audit unsigned-tag wraparound; existing ordering must not be claimed valid across wrap. Verify test headroom and document a separate widening fix if required.

## 4. Admission and notification design

Keep `PushToInputQueue(Message*)` as the single-entry interface, delegating to a bounded bulk-admission helper.

- Validate pointers and command-line counts outside `InputQueueMutex`; retain the existing `NoCL > 2` acceptance rule.
- A null pointer is rejected without dereferencing it; fix the current null-path `MarkToDelete()` defect.
- For accepted messages, assign tags, increment `InputQueueTag`, and push under one lock.
- Move `SetTag(InputQueueTag)` inside that lock; its current unlocked read can race with concurrent producers.
- Preserve `UnmarkToDelete()` protection before publishing accepted messages.
- Record whether the queue was empty before the first accepted push.
- Unlock, then issue **one `InputQueueCV.notify_one()` only for an empty→nonempty transition**.
- An all-rejected batch produces no notification. No caller may hold `InputQueueMutex` when invoking either public admission interface.
- SHM and action-generated single messages remain supported; do not hold SHM buffers or semaphores merely to accumulate a batch.

For network admission, extract at most 32 raw entries in FIFO order under `NetworkReceiveQueueMutex`, then unlock.
Deserialize outside both queue mutexes and bulk-admit accepted messages without waiting for additional traffic.
Leave excess raw entries for subsequent outer-loop turns; never nest the two queue mutexes.

## 5. Waiting and scheduling

The current wait predicate treats any future queued message as ready and can spin.
Use a due-work/stop predicate and a bounded timed recheck instead.

- Under `InputQueueMutex`, examine stop, due work, and the next scheduled deadline.
- Bound idle/future waits by the existing SHM polling interval; use steady-clock durations for recheck deadlines.
- Release the mutex during the wait; on timeout or wake, recompute readiness and service housekeeping.
- Empty→nonempty admission wakes promptly.
- Because earlier arrivals into a nonempty future queue intentionally do not notify, their extra detection delay is bounded by the poll interval.
- Do not sleep until a distant future deadline without that bound.
- Validate this scheduling tradeoff with timer and p99 tests; sub-poll-interval wake guarantees would require a separately approved notification policy.
- Synchronize stop publication with the input wait protocol and notify all relevant existing waiters; verify shutdown cannot lose a wake.

## 6. Ownership and cleanup

- Raw buffers transfer from ReceiveDispatcher to the raw queue, then to the GW local admission batch.
- GW releases each extracted buffer with `delete[]` exactly once, including malformed input and allocation failure paths.
- Confirm `SetMessageFromCharArray`/conversion retains no borrowed buffer after release; the excerpt alone does not prove that contract.
- `Message*` remains Process-managed: queue/local containers hold scheduling responsibility, not permission to call `delete`.
- Accepted queued and locally pending messages must remain protected from `PP->DeleteMarkedMessages()`.
- After each `Run`, retain existing mark-and-reclaim behavior; never blindly mark a message that was forwarded or rescheduled.
- Rejected nonnull messages follow existing Process marking/reclamation policy.
- Hermes must audit handler/GC behavior: if another handler can mark a locally pending message, establish a valid lifetime pin before batching or block rollout.
- Early returns, supported exceptions, and stop must release raw buffers and return or retain every unexecuted message; never silently discard local work.

## 7. Thread-safety and observability

Known accessors are GW consumption, GW network/SHM admission, GW action callbacks, and PG heartbeat.
Other threads may call public admission; Hermes must enumerate all call sites before approval.
ReceiveDispatcher touches the raw queue, not `InputQueue`; the output worker uses its separate queue synchronization.

- Guard all input queue operations, sizes, tag assignment, and local-pending accounting with `InputQueueMutex`.
- Heartbeat reads a coherent pair under that mutex: `queued` and `local_pending` (including the active handler).
- Define processing outstanding as **queued + local_pending**; extraction must not manufacture a backlog reduction.
- Decrement pending once each execution completes, without dereferencing a potentially reclaimed message.
- Publish queued depth separately and audit SPEC-028-B’s existing outstanding formula before comparing results.
- Obtain DEBUG queue-size snapshots under the appropriate mutex; do not iterate output queues unlocked.
- Do not hold queue locks during deserialization, `Run`, GC, logging, or buffer destruction.
- Audit shared logging and message lifetime independently; queue locking alone does not make them thread-safe.

## 8. Verification and benchmark protocol

Hermes records commits, build flags, batch limits, hardware, direction/rate interpretation, payload mix, offered totals, and workload timing.
Baseline is current SPEC-031 with batching disabled; use identical telemetry instrumentation in baseline and candidate.

1. Run at least three paired baseline/candidate trials at the measured **2000 msg/s bidirectional** workload and approximately 7.7-minute offer window.
2. Retain 10-second heartbeat logs; additionally capture exact outstanding high-water under the queue lock and timestamped drain samples at no worse than one-second resolution.
3. Reconstruct the historical 130-second interval’s start event. If unavailable, treat it as context, not a directly comparable stopwatch measurement.
4. For paired trials, define drain time as last stress admission to outstanding ≤2 continuously for 10 seconds, with all stress IDs delivered.
5. Report peak and drain time on both peers; no hiding a regression by moving work into local or raw queues.
6. Measure per-direction end-to-end p99 latency using the same clock-safe method in both builds; report clock synchronization uncertainty where applicable.
7. Verify offered-at-admission, sent success, received unique IDs, duplicates, gaps, dropped, and guards after drain.
8. Repeat the 14-minute 500 msg/s bidirectional soak; require zero loss and no timer, shutdown, memory, or CPU regression.
9. Test equal-time FIFO, future scheduling, earlier arrivals, partial batches, concurrent producers, invalid inputs, reentrant admission, GC, and stop with pending work.

**Acceptance:** Every paired trial must show at least 50% lower outstanding peak on the bottleneck peer, zero loss on both peers, and shorter drain time.
Against a reproduced 2762 baseline, the peak ceiling is **1381**.
Require p99 no worse than 10% above paired baseline, no duplicate deliveries, and no growing raw-queue backlog.
Publish all trial results, not only the best; unchanged processing cost or failed gates means no performance acceptance.

## 9. Rollback, portability, and adjacent defect

Provide one rollback switch restoring legacy extraction/admission/wait behavior; batch size 1 alone is not full rollback.
Drain and stop before switching builds; do not migrate live local batches.
Retain null safety, race fixes, and honest telemetry where independently verified.
Rollback on ordering violations, loss, lifetime faults, sustained latency regression, or failed performance gates.

Use existing C++ mutex/condition-variable support; add no glibc-only APIs or dependency on condition-variable fairness.
Keep SPEC-MUSL-001 permanent and SPEC-031’s portability guards unchanged.
Build and soak on the actual musl target; verify timed waits, stop notification, linking, and memory use.
Run race/lifetime tooling on a supported host if unavailable on musl; host tooling does not replace target benchmarks.

**NRNCS verdict:** Serving old 288KB content for a publication expected to contain new 9.9KB content is a credible cache-coherency defect candidate in SPEC-020/022/023 lineage, not evidence of transport loss.
Confirm publication identity and invalidation semantics in a separate reproduction; do not include cache changes in SPEC-032.
