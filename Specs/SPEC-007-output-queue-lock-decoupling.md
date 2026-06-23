# SPEC-007 — ReadFromOutputQueue: Decouple SHM Write from Queue Lock

**Status:** Draft — pending approval  
**Date:** 2026-06-23  
**Scope:** `Common/src/GW.cpp` `ReadFromOutputQueue()` — decouple `WriteToSharedMemory3` from `OutputQueueMutex`  
**Codebase:** NovaGenesis, branch `AIOPT2`  
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

Split `ReadFromOutputQueue` into two phases:

1. **Pop phase** (under lock): pop one message from the queue, release lock
2. **Write phase** (no lock): call `WriteToSharedMemory3` outside the lock
3. **Handle result** (under lock): if OK, mark message for deletion; if ERROR, re-push the message to the queue

This way, while `WriteToSharedMemory3` is blocking on the SHM semaphore or the `'w'` flag, the main thread can push new messages to the OutputQueue.

### 4.2 What changes

| Component | Before (SPEC-006 v2.0) | After (SPEC-007) |
|-----------|------------------------|------------------|
| `ReadFromOutputQueue` | Lock → iterate all queues → `WriteToSharedMemory3` for each → unlock → retry loop | Lock → pop one message → unlock → `WriteToSharedMemory3` → if fail, re-push under lock → repeat |
| Retry strategy | Retry loop holding the mutex (SPEC-006b) | Retry by re-pushing to queue; the `OutputQueueCV` notification cycle handles it naturally |
| Lock hold time | Up to 10ms × N messages per iteration | O(1) per pop/re-push (microseconds) |

### 4.3 Why this is safe

1. **OutputQueueMutex still protects the map**: All access to `OutputQueues` (push, pop, iterate) goes through `OutputQueueMutex`. The only change is that `WriteToSharedMemory3` is called outside the lock.

2. **Message ownership is clear**: After `top()`, the message pointer is not in any queue (it's still pointed to by the priority_queue's `top()` until `pop()`). We `pop()` under lock, then the message is owned by the local variable `PM1`. No other thread can access it.

3. **Re-push on failure is safe**: If `WriteToSharedMemory3` fails, we re-acquire the lock and push the message back. The priority queue's comparator (`DereferenceCompareNode`) uses `GetTime()` and `GetTag()`, so the message retains its original priority and will be retried in order.

4. **No message drops**: Messages are only removed from the queue after `WriteToSharedMemory3` succeeds. Failed messages go back to the queue.

5. **No lost wakeups**: After re-pushing a failed message, `NewOutputMessage` is set to `true` and `OutputQueueCV.notify_one()` is called, so the thread will immediately retry.

6. **No starvation of new pushes**: The main thread can push while the output thread is in `WriteToSharedMemory3`. New messages will be processed in priority order on the next pop.

### 4.4 Trade-offs

| Aspect | Before (SPEC-006 v2.0) | After (SPEC-007) |
|--------|------------------------|------------------|
| Lock hold per message | Up to 10ms (SHM sem + write) | ~1us (pop/re-push only) |
| Main thread blocking | Yes — blocked during SHM write + retry | No — can push while SHM write in progress |
| Throughput under load | Degrades — queue grows, RTTs increase | Sustained — SHM writes overlap with pushes |
| Message ordering | Strict (all queues processed atomically) | Per-queue FIFO (pop one at a time, re-push failures) |
| CPU on contention | High — retry loop spins | Low — failed messages sleep via CV wait |
| Retry latency | 1ms (sleep between retries) | ~10ms (CV wait for next notification cycle) |

The only trade-off is slightly higher retry latency for failed SHM writes (1ms → ~10ms via CV wait cycle). This is acceptable because:
- The SHM `'w'` flag typically clears within 10-20ms (the peer's Gateway poll interval)
- A 10ms retry is still 1000× faster than the current 28-150s RTT backlog
- The previous 1ms retry was ineffective anyway (the peer needs 10+ms to read)

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
    // Wait for output messages or stop flag (blocking wait - zero CPU when idle)
    {
      std::unique_lock<std::mutex> lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [this]()
                         { return NewOutputMessage || StopGateway; });
      if (StopGateway) break;
      NewOutputMessage = false;
    }

    // Pop one message at a time, write to SHM OUTSIDE the lock
    bool hasMessages = true;
    while (hasMessages && StopGateway == false)
    {
      // Phase 1: Pop (under lock — microseconds)
      PM1 = NULL;
      currentOQS.clear();

      {
        std::lock_guard<std::mutex> qlock(OutputQueueMutex);

        for (auto it = OutputQueues.begin(); it != OutputQueues.end(); it++)
        {
          if (!it->second.empty())
          {
            PM1 = it->second.top();
            currentOQS = it->first;
            it->second.pop();
            break;  // Pop one, release lock immediately
          }
        }
      } // Lock released here

      if (PM1 == NULL)
      {
        hasMessages = false;
        break;
      }

      // Phase 2: Write to SHM (OUTSIDE the lock — can take up to 10ms)
      if (WriteToSharedMemory3(currentOQS, PM1) == OK)
      {
        PM1->MarkToDelete();
      }
      else
      {
        // SHM busy — re-push the message for retry on next cycle
        {
          std::lock_guard<std::mutex> qlock(OutputQueueMutex);
          OutputQueues[currentOQS].push(PM1);
          NewOutputMessage = true;
        }
        OutputQueueCV.notify_one();

        // Brief sleep to avoid busy-spin on a blocked SHM slot
        tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
      }
    }
  }
}
```

**Diff summary:**

1. Remove the `deliveredAll` retry loop that held the lock during `WriteToSharedMemory3`
2. Add inner `while (hasMessages)` loop that pops one message at a time
3. Move `WriteToSharedMemory3` call outside the `lock_guard` scope
4. On failure: re-push the message under lock, set `NewOutputMessage = true`, notify CV
5. On failure: sleep 1ms outside the lock before retrying (prevents CPU spin)
6. On success: `MarkToDelete()` outside the lock (safe — message is not in any queue)

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

**Done when:** Commit pushed to `AIOPT2` branch.

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

3. **`WriteToSharedMemory3` has its own `sem_trywait` spin loop**: The function already has a 100×100us = 10ms spin on the per-SHM-key semaphore. This spin happens INSIDE the `OutputQueueMutex` lock. Even without the `'w'` flag issue, this spin alone can block the main thread for 10ms per message.

4. **The priority queue's `top()` + `pop()` is not atomic across queues**: The current code iterates all queues and writes to each one while holding the lock. The new code pops from the first non-empty queue and releases the lock. This means the order of processing across different queues may change. However, since all messages in a given queue are ordered by time, and the CV notification handles retries, this is safe.

5. **The `NewOutputMessage` flag is set by both `PushToOutputQueue` and the re-push path**: This is correct — the flag means "there is at least one message in some queue". The CV wait will return immediately if the flag is set. After the pop loop exhausts all messages, the thread goes back to CV wait. If a re-push happened during the SHM write, the flag is already set, so the thread will immediately process the re-pushed message.

---

## 10. Acceptance Criteria

- [ ] `ReadFromOutputQueue` does NOT call `WriteToSharedMemory3` inside `OutputQueueMutex` scope
- [ ] `WriteToSharedMemory3` is called after the lock is released
- [ ] Failed messages are re-pushed to the queue with `NewOutputMessage = true` + `notify_one()`
- [ ] `MarkToDelete()` is called outside the lock
- [ ] PGCS + NRNCS + ContentApp compile successfully
- [ ] RTTs remain stable (< 1s) during 3-minute run
- [ ] All 50 photos published and received
- [ ] No `(WARNING: Output Queue semaphore timeout)` messages
- [ ] No SIGSEGV or memory errors
- [ ] CPU < 20% per process
- [ ] Commit pushed to `AIOPT2` branch
- [ ] Dashboard updated with SPEC-007 reference
- [ ] Obsidian task file updated with results
