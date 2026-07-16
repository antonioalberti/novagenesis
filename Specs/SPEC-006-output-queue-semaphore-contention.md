# SPEC-006 — Output Queue Semaphore Contention Fix

**Status:** Implemented  
**Date:** 2026-06-23  
**Scope:** GWMsgCl01.cpp `SafePushToOutputQueue` + GW.cpp `ReadFromOutputQueue` + GW.cpp constructor — remove redundant named semaphore "Output_Queue"  
**Codebase:** NovaGenesis, branch `AIOPT3`  
**Stack:** C++20, g++ -O0 -g3, POSIX named semaphores, std::mutex  
**Task:** NG-042-07 (cross-ref SPEC-005)

**Revision history:**
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-06-23 | Hermes Agent (Scalifax) | Initial draft. Lock ordering analysis + deadlock-safe fix proposal. |
| v2.0 | 2026-06-23 | Hermes Agent (Scalifax) | Root cause revised: named_sem is redundant. Solution changed to remove named_sem entirely, use only OutputQueueMutex. |

---

## 1. Goal

Eliminate the `(WARNING: Output Queue semaphore timeout)` messages that appeared after SPEC-005 (10ms SHM poll interval) without introducing a deadlock, while preserving the latency improvements from SPEC-005 and preventing message drops.

---

## 2. Background: How the Output Queue works

The Output Queue is a per-process priority queue (`OutputQueues` map in GW) that buffers messages waiting to be written to shared memory (SHM) for delivery to other processes. Two threads interact with it WITHIN the same process:

```
              SafePushToOutputQueue
              (GWMsgCl01.cpp)                    ReadFromOutputQueue
                   │                                  (GW.cpp)
                   │                                      │
              ┌────▼─────┐                         ┌─────▼───────┐
              │ Output   │                         │ Read from   │
              │ Queue    │                         │ OutputQueue │
              │          │                         │             │
              │ push(M)  │                         │ WriteToSHM  │
              └──────────┘                         └─────────────┘
```

| Side | Thread | File:Function | What it does |
|------|--------|---------------|--------------|
| **Writer** | Main (gateway loop) | `GWMsgCl01.cpp:61-88` → `GW.cpp:271-306` | Pushes forwarded messages to `OutputQueues[OQS]` |
| **Reader** | Output thread (separate) | `GW.cpp:340-419` | Pops from `OutputQueues`, writes to SHM via `WriteToSharedMemory3` |

Two synchronisation mechanisms exist:

1. **`OutputQueueMutex`** (`std::mutex`, member of GW class, GW.h:180) — LOCAL to each process. Protects the `OutputQueues` map against concurrent access between the main thread and the output thread WITHIN the same process.

2. **Named semaphore `"Output_Queue"`** (`sem_open("Output_Queue", O_CREAT, 0666, 1)`, POSIX named semaphore) — SYSTEM-WIDE, shared across ALL NG processes (PGCS, NRNCS, Source, Repository). Used by `SafePushToOutputQueue` (writer) and `ReadFromOutputQueue` (reader).

---

## 3. The Problem

### 3.1 Symptom

After SPEC-005 (10ms SHM poll interval), repeated warnings appear in the log:

```
(WARNING: Output Queue semaphore timeout)
```

These appear in bursts after periods of high message throughput (e.g., after publishing 50 photos). The message is silently dropped — never delivered.

### 3.2 Root Cause: The named semaphore is redundant AND harmful

The named semaphore "Output_Queue" serves no purpose that `OutputQueueMutex` does not already serve. Analysis of all access points to `OutputQueues`:

| Access point | Lock used | Protects against |
|---|---|---|
| `PushToOutputQueue` (GW.cpp:301) | `lock_guard(OutputQueueMutex)` | Concurrent push vs pop |
| `ReadFromOutputQueue` (GW.cpp:360-361) | `unique_lock(OutputQueueMutex)` + `OutputQueueCV.wait` | Concurrent pop vs push |
| `ReadFromOutputQueue` (GW.cpp:381) | `lock_guard(OutputQueueMutex)` | Concurrent iterate vs push |
| `SafePushToOutputQueue` (GWMsgCl01.cpp:71) | `sem_trywait("Output_Queue")` | **Redundant** — PushToOutputQueue already locks OutputQueueMutex |
| `ReadFromOutputQueue` (GW.cpp:385) | `sem_trywait("Output_Queue")` inside lock_guard(OutputQueueMutex) | **Redundant** — OutputQueueMutex already held |

`OutputQueueMutex` fully protects the `OutputQueues` map. The named semaphore adds a second layer of mutual exclusion that is:

1. **Redundant** — protects the same data structure that OutputQueueMutex already protects
2. **Cross-process** — system-wide, so PGCS contends with NRNCS, Source, and Repository for a semaphore that protects per-process data
3. **Inconsistent** — 3 out of 4 writers bypass it entirely (see §3.3)
4. **Harmful** — causes message drops via timeout when contended (see §3.4)

### 3.3 Three writers bypass the named semaphore entirely

`SafePushToOutputQueue` (which acquires the named_sem) is only called from `GWMsgCl01.cpp` (L382, L407). But `PushToOutputQueue` is called directly from 3 other places:

| Caller | File:Line | Uses named_sem? |
|---|---|---|
| `SafePushToOutputQueue` | GWMsgCl01.cpp:81 → GW.cpp:271 | ✅ Yes |
| `GWRunHelloIPC02` | GWRunHelloIPC02.cpp:121 | ❌ No |
| `GWExposition02` (fresh hello) | GWExposition02.cpp:266 | ❌ No |
| `GWExposition02` (self hello) | GWExposition02.cpp:392 | ❌ No |

This means the named semaphore does NOT provide consistent mutual exclusion. It only blocks some writers, not all. The real protection is `OutputQueueMutex` inside `PushToOutputQueue`, which all 4 callers go through.

### 3.4 Mechanism of message drops

The current code uses `sem_trywait` (non-blocking) with a 100×100us = 10ms spin loop:

```cpp
// GWMsgCl01.cpp L70-80 — Writer side
int lockAttempts = 0;
while (sem_trywait(mutex) != 0 && lockAttempts < 100)
{
    tthread::this_thread::sleep_for(tthread::chrono::microseconds(100));
    lockAttempts++;
}
if (lockAttempts >= 100)
{
    _PB->S << _Offset << "(WARNING: Output Queue semaphore timeout)" << endl;
    return ERROR;  // ← Message silently dropped!
}
```

When the named semaphore is contended (because SPEC-005 increased message throughput 100-1000×), the writer spins for 10ms and then returns ERROR — the message is lost. The same pattern exists in the Reader side (GW.cpp L384-389), where a timeout causes the reader to skip the cycle.

### 3.5 Why SPEC-005 exposed this

Before SPEC-005, the gateway loop slept for `secondsUntilNext` (up to 10s). Message flow was sparse → negligible semaphore contention. After SPEC-005, the gateway wakes every 10ms → message flow is 100-1000× faster → contention appears.

This is **not a bug in SPEC-005** — it exposed a pre-existing fragility in the Output Queue synchronisation design that was latent because the system never ran fast enough to trigger it.

### 3.6 Why the v1.0 fix (reorder locks + sem_wait) is wrong

The v1.0 spec proposed fixing lock ordering inversion and switching to `sem_wait` (blocking). This is unsafe for 3 reasons:

**Reason 1 — Cross-process deadlock on crash.** The named semaphore "Output_Queue" is system-wide. If ANY process crashes while holding it (SIGSEGV, OOM kill, manual kill), ALL other processes block forever in `sem_wait`. The current `sem_trywait` at least times out and continues (with drops). With `sem_wait`, a single crash hangs the entire system.

**Reason 2 — Cross-process convoy.** Even without crashes, `sem_wait` on a system-wide semaphore serialises all processes' output queue operations. PGCS waiting for "Output_Queue" blocks NRNCS, even though their OutputQueues are completely independent. This defeats the purpose of having separate processes.

**Reason 3 — Inconsistent coverage.** Since 3 out of 4 writers bypass the named_sem (see §3.3), fixing its lock order does not achieve consistent mutual exclusion. The named_sem is structurally incapable of doing its job.

---

## 4. Solution: Remove the named semaphore entirely

### 4.1 Approach

Remove the named semaphore "Output_Queue" from all code paths. Use only `OutputQueueMutex` (which is already correctly placed and consistent) for all synchronisation of the `OutputQueues` map.

This is a simplification, not a patch — we eliminate a redundant mechanism instead of fixing it.

### 4.2 What changes

| Component | Before | After |
|---|---|---|
| `SafePushToOutputQueue` (GWMsgCl01.cpp) | sem_trywait("Output_Queue") + spin loop + sem_post | Direct call to `PushToOutputQueue` (which already locks OutputQueueMutex) |
| `ReadFromOutputQueue` (GW.cpp) | lock_guard(OutputQueueMutex) + sem_trywait("Output_Queue") + sem_post | lock_guard(OutputQueueMutex) only |
| GW constructor (GW.cpp) | sem_open("Output_Queue") + cache | Removed |

### 4.3 Why this is safe

**OutputQueueMutex already protects everything:**
- All pushes go through `PushToOutputQueue` (L301: `lock_guard(OutputQueueMutex)`)
- All pops/iteration go through `ReadFromOutputQueue` (L381: `lock_guard(OutputQueueMutex)`)
- The condition variable `OutputQueueCV` + `NewOutputMessage` flag handle idle waiting with zero CPU

**No cross-process interaction on OutputQueues:**
- Each process has its own `OutputQueues` map (member of GW class, heap-allocated per process)
- Each process has its own `OutputQueueMutex` (member of GW class)
- The named semaphore was the ONLY cross-process synchronisation on this data, and it was protecting per-process data — a category error

**WriteToSharedMemory3 is unaffected:**
- `WriteToSharedMemory3` uses per-SHM-key named semaphores (e.g., "11", "12", etc.), NOT "Output_Queue"
- These per-SHM semaphores coordinate writer/reader for a specific shared memory segment — a different concern, untouched by this spec

### 4.4 Trade-offs

| Aspect | Before (named_sem + sem_trywait) | After (OutputQueueMutex only) |
|--------|-----|------|
| Message drops | ✅ Yes — on 10ms timeout | ❌ Never — no timeout possible |
| CPU on contention | ⚠️ 10ms spin (100×100us) | ❌ Zero — mutex blocks efficiently |
| Deadlock risk | ❌ None (non-blocking) | ❌ None (single lock, no ordering) |
| Cross-process crash resilience | ⚠️ Other processes time out but continue | ✅ Unaffected — no shared semaphore |
| Cross-process convoy | ⚠️ All processes contend on one system-wide sem | ✅ None — each process has own mutex |
| Code complexity | ⚠️ Spin loop + backoff + named sem + cache | ✅ Simple — mutex already in place |
| Consistency | ⚠️ 3/4 writers bypass named_sem | ✅ All writers go through OutputQueueMutex |

All metrics improve. No downsides.

---

## 5. Implementation Details

### 5.1 Change A: Simplify `SafePushToOutputQueue` (GWMsgCl01.cpp)

**File:** `Common/src/GWMsgCl01.cpp`  
**Location:** Lines 51-88

**Current code (L51-88):**
```cpp
static sem_t* GetCachedOutputQueueSemaphore()
{
  static sem_t* cachedSem = NULL;
  if (cachedSem == NULL)
  {
    cachedSem = sem_open("Output_Queue", O_CREAT, 0666, 1);
  }
  return cachedSem;
}

static int SafePushToOutputQueue(GW* _PGW, string _OQS, Message* _M,
                                 string _Offset, Block* _PB, int& _Status)
{
  sem_t* mutex = GetCachedOutputQueueSemaphore();
  if (mutex == NULL || mutex == SEM_FAILED)
  {
    perror("Output Queue: unable to open semaphore");
    return ERROR;
  }
  int lockAttempts = 0;
  while (sem_trywait(mutex) != 0 && lockAttempts < 100)
  {
    tthread::this_thread::sleep_for(tthread::chrono::microseconds(100));
    lockAttempts++;
  }
  if (lockAttempts >= 100)
  {
    _PB->S << _Offset << "(WARNING: Output Queue semaphore timeout)" << endl;
    return ERROR;
  }
  _PGW->PushToOutputQueue(_OQS, _M);
  _Status = OK;
  if (sem_post(mutex) != 0)
  {
    perror("Writing Output Queue : sem_post");
  }
  return OK;
}
```

**New code:**
```cpp
static int SafePushToOutputQueue(GW* _PGW, string _OQS, Message* _M,
                                 string _Offset, Block* _PB, int& _Status)
{
  // PushToOutputQueue internally acquires OutputQueueMutex for thread-safe push.
  // The named semaphore "Output_Queue" was removed (SPEC-006): it was redundant
  // (OutputQueueMutex already protects OutputQueues), inconsistent (3/4 writers
  // bypassed it), and caused cross-process contention + message drops.
  _PGW->PushToOutputQueue(_OQS, _M);
  _Status = OK;
  return OK;
}
```

**Diff summary:**
1. Delete `GetCachedOutputQueueSemaphore()` function (L51-59, 9 lines)
2. Delete all sem_trywait/sem_post/spin loop logic in `SafePushToOutputQueue` (L64-86, ~23 lines)
3. `SafePushToOutputQueue` becomes a 3-line wrapper around `PushToOutputQueue`

### 5.2 Change B: Remove named_sem from `ReadFromOutputQueue` (GW.cpp)

**File:** `Common/src/GW.cpp`  
**Location:** Lines 340-419

**Current code (L340-419):**
```cpp
void GW::ReadFromOutputQueue()
{
  Message* PM1 = NULL;
  sem_t* mutex = NULL;

  // Cache the semaphore (avoid sem_open/sem_close per iteration)
  mutex = CachedSemaphores["Output_Queue"];
  if (mutex == NULL)
  {
    mutex = sem_open("Output_Queue", O_CREAT, 0666, 1);
    if (mutex != SEM_FAILED)
      CachedSemaphores["Output_Queue"] = mutex;
  }

  string SemaphoreName = "Output_Queue";

  while (StopGateway == false)
  {
    // Wait for output messages or stop flag (blocking wait - zero CPU when idle)
    {
      std::unique_lock<std::mutex> lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [this]()
                         { return NewOutputMessage || StopGateway; });
      if (StopGateway)
        break;
      NewOutputMessage = false;
    }

    if (mutex == NULL || mutex == SEM_FAILED)
    {
      mutex = sem_open(SemaphoreName.c_str(), O_CREAT, 0666, 1);
      if (mutex == SEM_FAILED)
      {
        perror("Output Queue: unable to open semaphore");
        continue;
      }
      CachedSemaphores["Output_Queue"] = mutex;
    }

    // Lock the output queue mutex for thread-safe iteration
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);

      // Retry sem_trywait with backoff
      int LockAttempts = 0;
      while (sem_trywait(mutex) != 0 && LockAttempts < 100)
      {
        tthread::this_thread::sleep_for(tthread::chrono::microseconds(100));
        LockAttempts++;
      }

      if (LockAttempts < 100)
      {
        map<...>::iterator it;

        for (it = OutputQueues.begin(); it != OutputQueues.end(); it++)
        {
          if (!it->second.empty())
          {
            PM1 = it->second.top();

            // Shared memory IPC
            if (WriteToSharedMemory3(it->first, PM1) == OK)
            {
              it->second.pop();
              PM1->MarkToDelete();
            }
          }
        }

        if (sem_post(mutex) != 0)
        {
          perror("Writing Output Queue : sem_post");
        }
      }
    } // unlock OutputQueueMutex

    // Sleep briefly to avoid CPU spin on retry
    tthread::this_thread::sleep_for(tthread::chrono::microseconds(10));
  }
}
```

**New code:**
```cpp
void GW::ReadFromOutputQueue()
{
  Message* PM1 = NULL;

  while (StopGateway == false)
  {
    // Wait for output messages or stop flag (blocking wait - zero CPU when idle)
    {
      std::unique_lock<std::mutex> lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [this]()
                         { return NewOutputMessage || StopGateway; });
      if (StopGateway)
        break;
      NewOutputMessage = false;
    }

    // Lock the output queue mutex for thread-safe iteration and pop
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);

      map<std::string, priority_queue<Message*, vector<Message*>, DereferenceCompareNode>>::iterator it;

      for (it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        if (!it->second.empty())
        {
          PM1 = it->second.top();

          // Shared memory IPC
          if (WriteToSharedMemory3(it->first, PM1) == OK)
          {
            it->second.pop();
            PM1->MarkToDelete();
          }
        }
      }
    } // unlock OutputQueueMutex
  }
}
```

**Diff summary:**
1. Remove `sem_t* mutex = NULL;` declaration (L343)
2. Remove the semaphore caching block (L345-352, 8 lines)
3. Remove `string SemaphoreName = "Output_Queue";` (L354)
4. Remove the semaphore re-open fallback block (L368-377, 10 lines)
5. Remove the `sem_trywait` spin loop inside the lock_guard block (L383-389, 7 lines)
6. Remove the `if (LockAttempts < 100)` conditional wrapper around the for-loop (L391, L414)
7. Remove `sem_post(mutex)` call (L410-413, 4 lines)
8. Remove the trailing `sleep_for(10us)` (L417-418, 2 lines) — no longer needed since there is no retry loop

### 5.3 Change C: Remove named_sem cache from GW constructor (GW.cpp)

**File:** `Common/src/GW.cpp`  
**Location:** Lines 84-89

**Current code (L84-89):**
```cpp
  // Cache Output_Queue semaphore to avoid repeated sem_open
  sem_t* out_sem = sem_open("Output_Queue", O_CREAT, 0666, 1);
  if (out_sem != SEM_FAILED)
  {
    CachedSemaphores["Output_Queue"] = out_sem;
  }
```

**New code:** (delete these 6 lines entirely)

**Diff summary:** Remove the semaphore open + cache from the constructor. `CachedSemaphores["Output_Queue"]` is no longer referenced anywhere after Changes A and B.

### 5.4 Compile-time safety

No new includes needed. The `<semaphore.h>` header remains needed for `WriteToSharedMemory3` (which uses per-SHM-key named semaphores). The `<fcntl.h>` header in GWMsgCl01.cpp (L44-46) was only needed for `O_CREAT` in `sem_open("Output_Queue", O_CREAT, ...)` — but `WriteToSharedMemory3` in GW.cpp also uses `O_CREAT`, so `<fcntl.h>` remains needed in GW.cpp. In GWMsgCl01.cpp, `<fcntl.h>` can optionally be removed since no `sem_open` calls remain, but leaving it is harmless.

---

## 6. Etapas

### Etapa E0: Preflight

**Change:** None — verification only

**Test:**
- Verify `sem_trywait` on "Output_Queue" is used in exactly 2 places: GWMsgCl01.cpp (L71) and GW.cpp (L385)
- Verify `sem_open("Output_Queue", ...)` appears in exactly 3 places: GWMsgCl01.cpp (L56), GW.cpp (L85), GW.cpp (L349)
- Verify no OTHER files reference "Output_Queue" semaphore
- Verify `PushToOutputQueue` acquires `OutputQueueMutex` internally (L301)
- Confirm `git status` shows only SPEC-006 file untracked

**Done when:** All 5 checks verified. No other references to "Output_Queue" exist outside the 5 locations listed above.

**Pitfall:** Run `grep -rn "Output_Queue" Common/src/` to confirm no other files use this semaphore name. `WriteToSharedMemory3` uses per-SHM-key semaphores (e.g., "11", "12") — these are DIFFERENT semaphores and must NOT be touched.

---

### Etapa E1: Apply Change A — simplify SafePushToOutputQueue (GWMsgCl01.cpp)

**Change:** `Common/src/GWMsgCl01.cpp` — delete `GetCachedOutputQueueSemaphore()` + rewrite `SafePushToOutputQueue` (~32 lines deleted, 3 lines remain)

1. Delete `GetCachedOutputQueueSemaphore()` function (L51-59)
2. Replace `SafePushToOutputQueue` body (L61-88) with 3-line wrapper

**Test:**
- Syntax check: `g++ -std=c++20 -fsyntax-only -I Common/src/ -pthread Common/src/GWMsgCl01.cpp`
- Verify no `sem_trywait`, `sem_open`, `sem_post` on "Output_Queue" remain in this file

**Done when:** Syntax check passes. Zero references to "Output_Queue" in GWMsgCl01.cpp.

---

### Etapa E2: Apply Change B — remove named_sem from ReadFromOutputQueue (GW.cpp)

**Change:** `Common/src/GW.cpp` — remove semaphore logic from `ReadFromOutputQueue` (~27 lines deleted)

1. Remove `sem_t* mutex = NULL;` declaration
2. Remove semaphore caching block (L345-352)
3. Remove `string SemaphoreName = "Output_Queue";` (L354)
4. Remove semaphore re-open fallback (L368-377)
5. Remove `sem_trywait` spin loop inside lock_guard (L383-389)
6. Remove `if (LockAttempts < 100)` wrapper
7. Remove `sem_post(mutex)` call
8. Remove trailing `sleep_for(10us)`

**Test:**
- Syntax check: `g++ -std=c++20 -fsyntax-only -I Common/src/ -pthread Common/src/GW.cpp`
- Verify `ReadFromOutputQueue` uses only `OutputQueueMutex` + `OutputQueueCV`

**Done when:** Syntax check passes. Zero references to "Output_Queue" in `ReadFromOutputQueue`.

---

### Etapa E3: Apply Change C — remove named_sem from GW constructor (GW.cpp)

**Change:** `Common/src/GW.cpp` — delete 6 lines from constructor (L84-89)

1. Remove `sem_open("Output_Queue", O_CREAT, 0666, 1)` and cache block

**Test:**
- Syntax check: `g++ -std=c++20 -fsyntax-only -I Common/src/ -pthread Common/src/GW.cpp`
- Run `grep -n "Output_Queue" Common/src/GW.cpp` → expect 0 matches

**Done when:** Syntax check passes. Zero references to "Output_Queue" in entire GW.cpp.

---

### Etapa E4: Full compile + smoke test

**Change:** None — test only

**Test:** User compiles and runs 4-process test:
1. `sudo bash Scripts/Simple/clean.sh`
2. Start PGCS + NRNCS + ContentApp Source + Repository
3. Check for:
   - Zero `(WARNING: Output Queue semaphore timeout)` messages
   - Photo publishing works as fast as SPEC-005 demonstrated (~30ms)
   - No crashes after 5 minutes of running
   - CPU < 20% per process

**Done when:** 5-minute run with zero warnings, all photos delivered.

---

### Etapa E5: Clean up stale named semaphore

**Change:** Remove the OS-level named semaphore (may persist from previous runs)

```bash
# Check if it exists
ls /dev/shm/sem.Output_Queue 2>/dev/null

# Remove it (sem_unlink equivalent)
sudo rm -f /dev/shm/sem.Output_Queue
```

**Done when:** `/dev/shm/sem.Output_Queue` does not exist.

---

### Etapa E6: Update SPEC-005 cross-reference

**Change:** Update acceptance criteria in SPEC-005 to note that semaphore warnings are resolved by SPEC-006

**Done when:** SPEC-005 updated, Dashboard updated.

---

### Etapa E7: Commit

**Change:** Git commit

```bash
git add Common/src/GW.cpp Common/src/GWMsgCl01.cpp \
  Specs/SPEC-006-output-queue-semaphore-contention.md
git commit -m "fix(gw): remove redundant Output Queue named semaphore (SPEC-006)

- Remove named semaphore 'Output_Queue' from SafePushToOutputQueue,
  ReadFromOutputQueue, and GW constructor
- OutputQueueMutex (std::mutex, per-process) already protects OutputQueues
  map — named semaphore was redundant, inconsistent (3/4 writers bypassed
  it), and caused cross-process contention + message drops
- Eliminates '(WARNING: Output Queue semaphore timeout)' exposed by
  SPEC-005 (10ms SHM poll interval)
- No deadlock risk: single lock (OutputQueueMutex), no ordering needed
- No cross-process crash risk: no shared semaphore to hold on crash

Ref: SPEC-006 v2.0, SPEC-005, NG-042-07"
```

**Done when:** Commit pushed to `AIOPT3`.

---

## 7. Safety Analysis

### 7.1 Deadlock analysis

With a single lock (`OutputQueueMutex`) protecting `OutputQueues`:

```
Writer: lock_guard(OutputQueueMutex) → push → unlock
Reader: lock_guard(OutputQueueMutex) → iterate + pop → unlock
```

There is only ONE lock. Deadlock requires at least two locks held in opposite order. With one lock, deadlock is impossible by construction.

### 7.2 Livelock / starvation

`std::mutex` on Linux (NPTL) does not guarantee fairness, but in practice the OS scheduler prevents starvation. The hold time is microsecond-scale (push to priority queue, or iterate + WriteToSharedMemory3), so even under contention, wait times are negligible.

### 7.3 Hold time

`OutputQueueMutex` is held for the duration of:
- **Writer:** `OutputQueues[OQS].push(M)` + `NewOutputMessage = true` + `notify_one()` — microsecond-scale
- **Reader:** iterate all OutputQueues + `WriteToSharedMemory3` per non-empty queue — microsecond-scale per message

`WriteToSharedMemory3` acquires a per-SHM-key named semaphore (e.g., "11"), copies the message to shared memory, and releases. This is fast (memcpy-scale). The per-SHM-key semaphore is separate from "Output_Queue" and is NOT removed by this spec.

### 7.4 Cross-process crash resilience

`OutputQueueMutex` is a `std::mutex` — it lives in process memory, not in `/dev/shm`. If a process crashes:
- Its `OutputQueueMutex` dies with it — no other process is affected
- No stale semaphore in `/dev/shm` to block other processes
- The per-SHM-key semaphores used by `WriteToSharedMemory3` may still be held by a crashed process, but that is a pre-existing concern unrelated to this spec

### 7.5 Error handling

`SafePushToOutputQueue` no longer returns ERROR on semaphore timeout (the timeout no longer exists). It calls `PushToOutputQueue`, which either succeeds (push to queue) or hits an internal error (message too small, too large, < 3 command lines). These internal errors are handled by `PushToOutputQueue` itself (marks message for delete, logs error). `SafePushToOutputQueue` always returns OK unless `PushToOutputQueue` has an internal failure (which would have happened regardless of the named semaphore).

### 7.6 Regression risk

**Low.** The change removes ~65 lines of redundant synchronisation code and keeps the `OutputQueueMutex` + `OutputQueueCV` mechanism that was already doing the real work. The 3 writers that already bypassed the named_sem (GWRunHelloIPC02, GWExposition02 ×2) prove that `OutputQueueMutex` alone is sufficient — they have been working correctly without the named_sem since they were written.

---

## 8. Testing Plan

| Test | Method | Pass criteria |
|------|--------|---------------|
| Syntax check | `g++ -fsyntax-only` both files | Zero errors |
| Grep verification | `grep -rn "Output_Queue" Common/src/` | Zero matches |
| 4-process smoke test | `clean.sh` + run all 4 | Zero `semaphore timeout` warnings |
| 5-min stability test | Continuous run | No crash, no warnings |
| Photo delivery | Log inspection | All photos published + delivered, ~30ms latency |
| CPU usage | `top` during test | < 20% per process |
| Stale semaphore | `ls /dev/shm/sem.Output_Queue` | Does not exist after clean.sh |

---

## 9. Risks

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| OutputQueueMutex contention causes latency | Very Low | Microsecond delay | Mutex hold time is microseconds. No spin, no timeout — OS schedules efficiently. |
| WriteToSharedMemory3 blocked by per-SHM semaphore while holding OutputQueueMutex | Low | Reader holds OutputQueueMutex longer | This is pre-existing behaviour — the named_sem did not prevent this (Reader held OutputQueueMutex during WriteToSharedMemory3 regardless). The hold time is bounded by SHM write (memcpy-scale). |
| Stale named semaphore persists in /dev/shm | Medium | Confusion on next run | Etapa E5 removes it. `clean.sh` should also be updated to `sem_unlink("Output_Queue")`. |

---

## 10. Verification Checklist

- [ ] Zero references to "Output_Queue" in `Common/src/GW.cpp` and `Common/src/GWMsgCl01.cpp`
- [ ] `SafePushToOutputQueue` is a 3-line wrapper around `PushToOutputQueue`
- [ ] `ReadFromOutputQueue` uses only `OutputQueueMutex` + `OutputQueueCV`
- [ ] GW constructor does not open "Output_Queue" semaphore
- [ ] Syntax check passes for both files
- [ ] No warnings during 5-minute test
- [ ] All photos delivered with ~30ms latency
- [ ] CPU < 20% per process
- [ ] `/dev/shm/sem.Output_Queue` removed
- [ ] Committed and pushed to `AIOPT3`
