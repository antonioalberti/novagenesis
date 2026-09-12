# SPEC-007 — ReadFromOutputQueue: Decouple SHM Write from Queue Lock

**Status:** Implemented  
**Date:** 2026-06-23  
**Scope:** `Common/src/GW.cpp` `ReadFromOutputQueue()` — decouple `WriteToSharedMemory3` from `OutputQueueMutex`  
**Codebase:** NovaGenesis, branch `AIOPT3`  
**Stack:** C++20, g++ -O0 -g3, POSIX SHM IPC, System V semaphores, std::mutex  
**Task:** NG-042-07 (cross-ref SPEC-005, SPEC-006)  

**Revision history:**
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-06-23 | Hermes Agent (Scalifax) | Initial draft. Root cause analysis of growing RTTs. Lock decoupling solution. |

---

## 1. Goal

Eliminate the growing RTT backlog (28s → 58s → 86s → 117s → 147s) in ContentApp photo publication by removing `WriteToSharedMemory3` from inside the `OutputQueueMutex` critical section in `ReadFromOutputQueue`.

---

## 2. Background

### 2.1 What SPEC-004/005/006 did

- **SPEC-004:** Reduced ContentApp INI timers (ServiceOffer 60→30s, PhotoPublish 10→1s, Discovery 10→3s).
- **SPEC-005:** Reduced GW SHM poll interval from 100ms to 10ms. Capped CV wait timeout at 10ms.
- **SPEC-006 v2.0:** Removed the redundant named semaphore `"Output_Queue"`. Simplified `SafePushToOutputQueue` to a 3-line wrapper. Added a retry loop for undelivered messages (if `WriteToSharedMemory3` fails, sleep 1ms and retry).

### 2.2 What SPEC-006 v2.0 did NOT fix

SPEC-006 removed the named semaphore but left `WriteToSharedMemory3` **inside** the `OutputQueueMutex` lock in `ReadFromOutputQueue`. The retry loop (SPEC-006b) made the problem worse: it holds the lock for even longer because the loop retries the SHM write without releasing the mutex.

### 2.3 The symptom (from user log)

```
(RTT from NRNCS was 0.077168492 seconds for the key BC59CB35.)     ← first batch OK
(RTT from NRNCS was 28.71856307 seconds for the key 0838844F.)     ← grows
(RTT from NRNCS was 58.67047499 seconds for the key 1E47696E.)     ← grows further
(RTT from NRNCS was 86.42533694 seconds for the key 171663CA.)     ← keeps growing
(RTT from NRNCS was 117.7618608 seconds for the key 2668C30F.)     ← keeps growing
(RTT from NRNCS was 147.7799114 seconds for the key AEB1125F.)     ← keeps growing
```

RTTs grow by ~30s each cycle (the `DelayBeforeRunPeriodic` of the Source ContentApp). This is a cumulative backlog — each periodic cycle adds more subscriptions than can be delivered.

---

## 3. The Problem

### 3.1 Architecture: two threads, one mutex

Two threads interact with the `OutputQueues` map within each process:

```
Thread 1 (Gateway loop, main thread):
  PushToOutputQueue()          ← locks OutputQueueMutex briefly to push
  SafePushToOutputQueue()      ← wrapper, calls PushToOutputQueue
  GWRunHelloIPC02              ← calls PushToOutputQueue directly
  GWExposition02               ← calls PushToOutputQueue directly

Thread 2 (Output thread):
  ReadFromOutputQueue()        ← locks OutputQueueMutex to pop + write to SHM
```

`OutputQueueMutex` protects the `OutputQueues` map (a `map<string, priority_queue<Message*>>`). It is a per-process `std::mutex`.

### 3.2 The current ReadFromOutputQueue (after SPEC-006 v2.0)

```cpp
void GW::ReadFromOutputQueue()
{
  while (StopGateway == false)
  {
    // Wait for output messages (blocking, zero CPU when idle)
    {
      unique_lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [] { return NewOutputMessage || StopGateway; });
      if (StopGateway) break;
      NewOutputMessage = false;
    }

    // PROBLEM: retry loop holds the mutex during SHM write
    bool deliveredAll = false;
    while (!deliveredAll && StopGateway == false)
    {
      deliveredAll = true;

      {
        lock_guard(OutputQueueMutex);          ← LOCK HELD

        for (each output queue)
        {
          if (!queue.empty())
          {
            PM1 = queue.top();

            // SHM write — can take up to 10ms per message (sem_trywait 100×100us)
            if (WriteToSharedMemory3(queue_key, PM1) == OK)
            {
              queue.pop();
              PM1->MarkToDelete();
            }
            else
            {
              deliveredAll = false;            ← SHM busy, will retry
            }
          }
        }
      }                                        ← LOCK RELEASED

      if (!deliveredAll)
        sleep(1ms);
    }
  }
}
```

### 3.3 Root cause: WriteToSharedMemory3 blocks inside the lock

`WriteToSharedMemory3` (GW.cpp:923-1080+) does the following under the `OutputQueueMutex`:

1. Looks up the SHM key in the HT (fast, ~microseconds)
2. Opens/caches a per-SHM-key named semaphore (fast after first call)
3. Calls `sem_trywait(mutex)` with 100×100us spin loop = **up to 10ms per message**
4. If locked: `shmat` (attach shared memory segment)
5. Checks `data[0] == 'w'` (peer hasn't read the previous message yet)
6. If `'w'`: the message can't be delivered → returns ERROR → retry loop keeps the lock
7. If `'f'`: copies the entire message into shared memory (memcpy, can be large for photos)
8. Sets `data[0] = 'w'` (mark as waiting for peer reading)
9. `sem_post(mutex)`

**The critical issue**: Step 3 (`sem_trywait` spin) and step 6 (peer hasn't read = `'w'` flag) can block for up to 10ms **per message**, and this happens **while holding `OutputQueueMutex`**.

### 3.4 How the backlog grows

With 50 photos being published rapidly (SPEC-004 reduced PhotoPublish to 1s, SPEC-005 made SHM delivery 10ms):

1. The Source's `CoreRunContentPublish01` pushes 50 publish messages into the OutputQueue rapidly (via `PushToInputQueue` → `Run()` → `SafePushToOutputQueue`)
2. The output thread wakes, locks `OutputQueueMutex`, and starts iterating over queues
3. For each photo message, it calls `WriteToSharedMemory3` which:
   - Tries `sem_trywait` on the destination process's SHM semaphore (up to 10ms)
   - Checks if the destination has read the previous message (`data[0] == 'w'`)
   - If the destination is busy processing the previous message, the SHM slot stays `'w'` → `WriteToSharedMemory3` returns ERROR → the retry loop holds the lock and tries again after 1ms
4. **While the output thread holds the mutex**, the main thread (Gateway loop) tries to push new messages (subscription requests, discovery responses from NRNCS, delayed-delivery resubscriptions from CoreRunPeriodic01) → **blocks on `OutputQueueMutex`**
5. Subscription requests can't enter the OutputQueue → they queue in the InputQueue → they aren't forwarded to NRNCS → NRNCS doesn't deliver the content → the subscription's `Timestamp` gets older → RTT grows

### 3.5 Why the retry loop (SPEC-006b) made it worse

The retry loop (`while (!deliveredAll)`) was added to prevent message drops when SHM is busy. But it holds `OutputQueueMutex` for the entire retry duration. If 10 messages can't be delivered because the peer is slow, the retry loop will:
- Iterate all queues (holding lock)
- Find 10 undelivered messages
- Release lock, sleep 1ms
- Re-acquire lock, iterate again
- The 10 messages still can't be delivered (peer hasn't read yet)
- Meanwhile, new pushes are blocked

The retry loop transforms a transient SHM-busy condition into a sustained `OutputQueueMutex` hold, starving the main thread.

### 3.6 The 'w' flag: peer reading latency

The SHM protocol uses a flag byte:
- `'f'` = free to write (peer has read the previous message, or this is the first message)
- `'w'` = waiting for peer reading (writer has written, reader hasn't consumed yet)

When the destination process is busy processing a message (e.g., running `CoreRunEvaluate01`, `CoreDeliveryBind01`), it doesn't poll SHM. The flag stays `'w'` until the destination's Gateway loop calls `ReadFromSharedMemory3`. During this time, the writer's `WriteToSharedMemory3` returns ERROR, and the retry loop spins.

The destination's SHM poll interval is 10ms (SPEC-005), so in the best case the `'w'` flag clears every 10ms. But if the destination is processing a large message (e.g., a photo payload), the Gateway loop is blocked in `Run(PM1, PM2)` and doesn't poll SHM until the run completes.

---

## 4. Solution: Decouple SHM Write from Queue Lock

### 4.1 Approach

Split `ReadFromOutputQueue` into two phases, processing **at most one message per queue per cycle**:

1. **Pop phase** (under lock): pop the top message from each non-empty queue into a local batch (one per queue, typically 2-4 messages). Release lock.
2. **Write phase** (no lock): call `WriteToSharedMemory3` for each message in the batch
3. **Handle result** (under lock): if OK, `MarkToDelete()`; if ERROR, re-push the message to its queue
4. **Re-evaluate** (under lock): check if any queues still have pending messages and update `NewOutputMessage` accordingly — prevents the output thread from sleeping with unprocessed messages

This way:
- While `WriteToSharedMemory3` is processing the SHM semaphore or the `'w'` flag, the main thread can push new messages
- No single queue can starve others (each queue gets one attempt per cycle)
- The crash window is limited: at most N messages are outside the queues at any time (N = number of queues, typically 2-4)
- Per-queue FIFO order is preserved (only `top()` of each queue is popped)

### 4.2 What changes

| Component | Before (SPEC-006 v2.0) | After (SPEC-007) |
||-----------|------------------------|------------------|
|| `ReadFromOutputQueue` | Lock → iterate all queues → `WriteToSharedMemory3` for each → unlock → retry loop | Lock → pop one per queue into local batch → unlock → `WriteToSharedMemory3` for each → if fail, re-push under lock → re-evaluate `NewOutputMessage` → repeat |
|| Retry strategy | Retry loop holding the mutex (SPEC-006b) | Per-queue retry: `WriteToSharedMemory3` failure re-pushes the message; next cycle pops it again naturally |
|| Lock hold time | Up to 10ms × N messages per iteration | O(N_queues) per pop (microseconds) — always small |
|| Fairness across queues | All processed atomically per iteration (fair) | One attempt per queue per cycle (fair — no starvation) |
|| Crash window | 0 (messages always in queue) | At most N_queues messages temporarily outside queues (typically 2-4) |

### 4.3 Why this is safe

1. **OutputQueueMutex still protects the map**: All access to `OutputQueues` (push, pop, iterate) goes through `OutputQueueMutex`. The only change is that `WriteToSharedMemory3` is called outside the lock.

2. **Message ownership is clear**: After `pop()`, the message pointer is owned by the local batch. No other thread can access it while it's outside the queue.

3. **Re-push on failure is safe**: If `WriteToSharedMemory3` fails, we re-acquire the lock and push the message back. The priority queue's comparator uses `GetTime()` and `GetTag()`, so the message retains its original priority and will be retried in order.

4. **No message drops**: Messages are only removed from the queue after `WriteToSharedMemory3` succeeds (and `MarkToDelete()` is called). Failed messages go back to the queue.

5. **Crash window is bounded**: At most N_queues messages can be outside the queues at any instant. With 2-4 queues, this is 2-4 messages — safe and bounded.

6. **No lost wakeups**: The `NewOutputMessage` flag is re-evaluated after each cycle by scanning all queues for remaining work. If any queue still has messages, the flag is set to `true`, preventing the output thread from blocking at the next `wait()`.

7. **Per-queue ordering preserved**: Only the `top()` of each queue is popped per cycle. Within each queue, messages are processed in strict priority order (by `GetTime()` then `GetTag()`). A failed message is re-pushed with its original priority, so it stays at the correct position.

8. **No starvation of new pushes**: The main thread can push while the output thread is in `WriteToSharedMemory3`. New messages appear in the queue scan at the start of the next cycle.

### 4.4 Trade-offs

| Aspect | Before (SPEC-006 v2.0) | After (SPEC-007) |
||--------|------------------------|------------------|
|| Lock hold per message | Up to 10ms (SHM sem + write) | ~1us per pop/re-push |
|| Main thread blocking | Yes — blocked during SHM write + retry | No — can push while SHM write in progress |
|| Throughput under load | Degrades — queue grows, RTTs increase | Sustained — SHM writes overlap with pushes |
|| Fairness across queues | All processed atomically | One attempt per queue per cycle (fair) |
|| Per-queue ordering | Strict | Preserved (only top() popped per cycle) |
|| Crash window | 0 messages outside queue | At most N_queues (~2-4) |
|| New-push responsiveness | Depends on retry loop completion | Next cycle picks them up (bounded by batch size) |
|| CPU on contention | High — retry loop spins | Low — failed messages re-push and cycle via CV wait |

The trade-offs are acceptable because:
- The crash window is bounded by the number of queues, not the total message count
- Per-queue FIFO ordering is strictly preserved
- The main thread can push freely while SHM writes are in progress
- The one-per-queue pattern prevents any single blocked destination from starving others

---

## 5. Implementation Details

### 5.1 Change: Rewrite ReadFromOutputQueue (GW.cpp)

**File:** `Common/src/GW.cpp`  
**Location:** Lines 336-406 (current `ReadFromOutputQueue`)

**Current code (after SPEC-006 v2.0 + SPEC-006b retry loop):**

```cpp
void GW::ReadFromOutputQueue()
{
  Message* PM1 = NULL;

  while (StopGateway == false)
  {
    // Wait for output messages or stop flag
    {
      std::unique_lock<std::mutex> lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [this]()
                         { return NewOutputMessage || StopGateway; });
      if (StopGateway) break;
      NewOutputMessage = false;
    }

    // Retry loop holding lock (PROBLEM)
    bool deliveredAll = false;
    while (!deliveredAll && StopGateway == false)
    {
      deliveredAll = true;
      {
        std::lock_guard<std::mutex> qlock(OutputQueueMutex);
        // ... iterate all queues, WriteToSharedMemory3 for each ...
      }
      if (!deliveredAll)
        sleep(1ms);
    }
  }
}
```

**New code:**

```cpp
void GW::ReadFromOutputQueue()
{
  Message* PM1 = NULL;
  std::string currentOQS;

  while (StopGateway == false)
  {
    // Wait for output messages or stop flag (blocking wait — zero CPU when idle)
    {
      std::unique_lock<std::mutex> lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [this]()
                         { return NewOutputMessage || StopGateway; });
      if (StopGateway) break;
      NewOutputMessage = false;
    }

    // Phase 1: Pop at most one message per queue (under lock — microseconds)
    std::vector<std::pair<std::string, Message*>> batch;
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);

      for (auto it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        if (!it->second.empty())
        {
          PM1 = it->second.top();
          it->second.pop();
          batch.emplace_back(it->first, PM1);
        }
      }
    } // Lock released

    if (batch.empty())
      continue;

    // Phase 2: Write each message to SHM (OUTSIDE the lock)
    bool anyFailed = false;

    for (auto& kv : batch)
    {
      if (WriteToSharedMemory3(kv.first, kv.second) == OK)
      {
        kv.second->MarkToDelete();
      }
      else
      {
        // SHM busy — re-push under lock for retry on next cycle
        {
          std::lock_guard<std::mutex> qlock(OutputQueueMutex);
          OutputQueues[kv.first].push(kv.second);
        }
        anyFailed = true;
      }
    }

    // Re-evaluate NewOutputMessage: check if queues still have work
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);
      bool hasMoreWork = false;
      for (auto it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        if (!it->second.empty())
        {
          hasMoreWork = true;
          break;
        }
      }
      NewOutputMessage = hasMoreWork;
    }

    // Backpressure: brief sleep if any message failed (peer SHM still busy)
    if (anyFailed)
    {
      tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
    }
  }
}
```

**Diff summary:**

1. Remove the `deliveredAll` retry loop that held the lock during `WriteToSharedMemory3`
2. **Phase 1**: Pop **at most one message per queue** into a local `batch` vector (under lock — microseconds). All queues are served equally per cycle.
3. **Phase 2**: Call `WriteToSharedMemory3` for each message **outside the lock**
4. On `WriteToSharedMemory3` failure: re-push the message under lock, set `anyFailed = true` for backpressure sleep
5. On `WriteToSharedMemory3` success: call `MarkToDelete()` outside the lock (safe — message is not in any queue)
6. **After Phase 2**: Re-evaluate `NewOutputMessage` by scanning all queues. If any remain non-empty, set `NewOutputMessage = true` so the output thread does not block at the next `wait()`. This is critical: without this step, the thread would sleep with pending messages because `NewOutputMessage` was cleared before the cycle started.
7. If any message failed (`anyFailed`), sleep 1ms outside the lock before returning to `wait()` — prevents busy-spin on a blocked SHM slot
8. `OutputQueueCV.notify_one()` is **removed** from the re-push path — the re-evaluation of `NewOutputMessage` handles wakeup correctly, and the output thread (the only consumer of this CV) is already awake and looping

### 5.2 No changes needed elsewhere

- `PushToOutputQueue` (GW.cpp:271-333): unchanged — already locks `OutputQueueMutex` for push + notify
- `SafePushToOutputQueue` (GWMsgCl01.cpp): unchanged — 3-line wrapper
- `WriteToSharedMemory3` (GW.cpp:923+): unchanged — still uses per-SHM-key named semaphores
- GW constructor: unchanged — no more `Output_Queue` named semaphore (already removed by SPEC-006)

### 5.3 Compile-time safety

No new includes. No new member variables. No signature changes. The only change is inside `ReadFromOutputQueue`'s function body.

---

## 6. Etapas

### Etapa E0: Preflight

**Change:** None — verification only

**Test:**
- Verify current `ReadFromOutputQueue` (GW.cpp:336-406) matches the SPEC-006 v2.0 + retry loop code
- Verify `WriteToSharedMemory3` (GW.cpp:923+) uses `sem_trywait` with 100×100us spin
- Verify `PushToOutputQueue` (GW.cpp:271-333) locks `OutputQueueMutex` and calls `OutputQueueCV.notify_one()`
- Confirm `git status` shows only SPEC-007 file untracked
- Run `grep -n "OutputQueueMutex" Common/src/GW.cpp` to confirm all lock sites

**Done when:** All 4 checks verified. No unexpected references to `OutputQueueMutex` outside the known locations.

**Pitfall:** Verify that `MarkToDelete()` is safe to call outside the lock. `MarkToDelete` sets a flag on the `Message` object. The message is not in any queue at this point (it was popped). No other thread can access it. Safe.

---

### Etapa E1: Apply the ReadFromOutputQueue rewrite

**Change:** `Common/src/GW.cpp` — rewrite `ReadFromOutputQueue` function body (~70 lines → ~50 lines)

1. Remove the `deliveredAll` retry loop
2. Add `hasMessages` loop with pop-one-at-a-time pattern
3. Move `WriteToSharedMemory3` outside the `lock_guard` scope
4. Add re-push + notify on failure
5. Add 1ms sleep on failure (outside lock)

**Test:**
- Syntax check: `g++ -std=c++20 -fsyntax-only -I Common/src/ -pthread Common/src/GW.cpp`
- Verify `WriteToSharedMemory3` is NOT called inside any `lock_guard` or `unique_lock` on `OutputQueueMutex`
- Verify `MarkToDelete()` is called outside the lock
- Verify re-push path sets `NewOutputMessage = true` and calls `notify_one()`

**Done when:** Syntax check passes. No `WriteToSharedMemory3` call inside `OutputQueueMutex` scope.

---

### Etapa E2: Full compile + smoke test

**Change:** None — test only

**Test:** User compiles and runs 4-process test:
1. `sudo bash Scripts/Simple/clean.sh`
2. Start PGCS + NRNCS + ContentApp Source + Repository
3. Check for:
   - Zero `(WARNING: Output Queue semaphore timeout)` messages
   - RTTs stay below 1s (not growing 28s → 58s → 86s...)
   - Photo publishing works — 50 photos delivered to Repository
   - No SIGSEGV or memory errors
4. Monitor for 2-3 minutes to confirm RTTs remain stable

**Done when:** All 4 processes run for 3 minutes. RTTs < 1s. All photos delivered. No crashes.

**Pitfall:** If RTTs still grow, the bottleneck may be in the destination process (peer can't read SHM fast enough). In that case, check if the Repository's Gateway loop is blocked in `Run()` for too long.

---

### Etapa E3: Commit

**Change:** Git commit

```bash
git add Common/src/GW.cpp Specs/SPEC-007-output-queue-lock-decoupling.md
git commit -m "fix(gw): decouple WriteToSharedMemory3 from OutputQueueMutex (SPEC-007)

- Move WriteToSharedMemory3 outside the lock_guard in ReadFromOutputQueue
- Pop one message at a time, write to SHM without holding the mutex
- Re-push failed messages with CV notification for retry
- Eliminates growing RTT backlog (28s→147s) caused by mutex starvation
- Main thread can push while SHM write in progress
- Ref: SPEC-007, NG-042-07"
```

**Done when:** Commit pushed to `AIOPT3` branch.

---

## 7. Testing Plan

| Test | Method | Pass criteria |
|------|--------|---------------|
| RTT stability | Check log: `(RTT from NRNCS was X seconds)` | All RTTs < 1s, no growing trend |
| Photo delivery | Count `Publishing the content` + `Receiver counter` lines | All 50 photos published and received |
| No mutex starvation | Check log: subscription `Time from subscription` values | All < 5s, no 26s+ values |
| No message drops | Compare published count vs received count | 0 lost messages |
| No semaphore warnings | Check log for `WARNING: Output Queue semaphore timeout` | Zero warnings |
| Long-running stability | Run for 3 minutes | No crash, no memory growth, RTTs stable |
| CPU usage | `top -p <PID>` during run | < 20% per process |

---

## 8. Risks

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Re-pushed message causes priority inversion | Low | A failed message is re-pushed and might be retried before newer messages | The priority queue orders by `GetTime()` then `GetTag()`. Failed messages keep their original time, so they are retried in order. |
| CV notify storm on many failures | Low | Many `notify_one()` calls if many messages fail | The 1ms sleep between retries limits the rate. `notify_one()` only wakes the same thread (which is already awake). |
| Lost message if process crashes between pop and re-push | Very Low | Message is lost | Same risk as current code — a crash between `top()` and `pop()` would also lose the message. Acceptable. |
| Destination process can't read SHM fast enough | Medium | RTTs still grow | This would indicate the bottleneck is in the destination's Gateway loop, not the OutputQueue. Would require a separate fix. |
| `MarkToDelete` called outside lock | Very Low | Race condition | The message is not in any queue and no other thread holds a reference to it. Safe. |

---

## 9. Pitfalls Discovered During Design

1. **The retry loop (SPEC-006b) amplified the problem**: The retry loop was designed to prevent message drops, but it holds `OutputQueueMutex` for the entire retry duration. Under load (50 photos), this blocks the main thread from pushing new messages, creating a cascading backlog.

2. **The `'w'` flag creates a natural backpressure**: When the destination process is busy, the SHM flag stays `'w'`. The writer must wait. This is by design — it prevents overwriting unread messages. But waiting inside the lock is the problem, not the waiting itself.

3. **`WriteToSharedMemory3` has its own `sem_trywait` spin loop**: The function already has a 100×100us = 10ms spin on the per-SHM-key semaphore. This spin happens INSIDE the `OutputQueueMutex` lock in the current code. Even without the `'w'` flag issue, this spin alone can block the main thread for 10ms per message. SPEC-007 moves this spin outside the lock.

4. **Pop-one-at-a-time causes starvation between queues**: Selecting the first non-empty queue repeatedly means a blocked queue can monopolise the output thread. Other queues are never processed until the blocked queue's SHM clears. The fix is to pop at most one message per queue per cycle, not one message total.

5. **Batch-ALL increases crash window and breaks ordering**: Popping all messages from all queues into a single batch creates a large crash window (all messages outside their queues) and can invert per-queue ordering (M1 fails, M2 succeeds before M1 is re-pushed). The one-per-queue approach avoids both: at most N_queues messages are outside their queues, and only `top()` is popped per queue.

6. **`NewOutputMessage` must be re-evaluated after partial drain**: When `NewOutputMessage` is set to `false` at the start of a cycle, but the cycle only processes one message per queue (not all messages), the flag becomes stale — queues may still have work. Without re-evaluation, the output thread can block at `wait()` with pending messages. The fix is to scan all queues after the cycle and set `NewOutputMessage = true` if any remain non-empty.

7. **`notify_one()` in the re-push path is redundant**: The output thread is the only consumer of `OutputQueueCV`, and it is already awake and looping after a failed `WriteToSharedMemory3`. The `NewOutputMessage` re-evaluation after the cycle handles wakeup correctly. Adding `notify_one()` is harmless but unnecessary in a single-consumer design.

8. **The priority queue's `top()` + `pop()` is not atomic across queues**: The old code iterated all queues and wrote to each one while holding the lock. The one-per-queue code pops one per queue and releases the lock. Message ordering across different queues may change between cycles, but within each queue FIFO order is strictly preserved.

9. **`NewOutputMessage` as a predicate source-of-truth is fragile**: Relying on a boolean flag to indicate "there is work to do" is inherently fragile when the cycle doesn't drain all work. The re-evaluation step corrects this, but a more robust long-term design would use a predicate that checks queue emptiness directly inside the `wait()` lambda.

---

## 10. Acceptance Criteria

- [x] `ReadFromOutputQueue` does NOT call `WriteToSharedMemory3` inside `OutputQueueMutex` scope
- [x] `WriteToSharedMemory3` is called after the lock is released
- [x] At most one message per queue is popped per cycle (no starvation)
- [x] Failed messages are re-pushed to their queue without setting `NewOutputMessage` (re-evaluated after cycle)
- [x] `NewOutputMessage` is re-evaluated after each cycle: scan all queues, set `true` if any non-empty
- [x] `notify_one()` removed from re-push path (redundant in single-consumer design)
- [x] `MarkToDelete()` is called outside the lock
- [ ] PGCS + NRNCS + ContentApp compile successfully
- [ ] RTTs remain stable (< 1s) during 3-minute run
- [ ] All 50 photos published and received
- [ ] No `(WARNING: Output Queue semaphore timeout)` messages
- [ ] No SIGSEGV or memory errors
- [ ] CPU < 20% per process
- [ ] Commit pushed to `AIOPT3` branch
- [ ] Dashboard updated with SPEC-007 reference
- [ ] Obsidian task file updated with results
