# SPEC-005 — GW.cpp SHM Poll Interval Reduction

**Status:** Approved 2026-06-23 — 10ms interval, pending implementation  
**Date:** 2026-06-23  
**Scope:** GW.cpp `Gateway()` SHM poll rate limiting  
**Codebase:** NovaGenesis, branch `AIOPT2`  
**Stack:** C++20, g++ -O0 -g3, SHM IPC, System V semaphores, std::condition_variable  
**Task:** NG-042-07 (cross-ref SPEC-004)  

**Revision history:**
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-06-23 | Hermes Agent (Scalifax) | Initial draft. Analysis of NG-005 Phase 1+2 100ms SHM poll rate limit. |

---

## 1. Goal

Reduce the SHM poll interval in `GW.cpp::Gateway()` from 100ms to 10ms to eliminate the latency regression introduced by NG-005 Phase 1+2, while preserving the CPU usage reduction achieved by the condition-variable-based blocking wait.

---

## 2. Scope

### IN
- `Common/src/GW.cpp` — `Gateway()` function, SHM poll rate limit at L604

### OUT (DO NOT TOUCH)
- `Common/src/GW.cpp` — `InputQueueCV.wait_for()` logic (L458-L467) — the CV-based blocking wait is preserved
- `Common/src/GW.cpp` — `ReadFromSharedMemory3()` function (L625+) — SHM read logic unchanged
- `Common/src/GW.cpp` — `ReadFromOutputQueueThread` (L380-L419) — output thread unchanged
- `Common/src/GW.cpp` — semaphore caching (`CachedSemaphores` map) — preserved
- `Common/src/GW.cpp` — `sem_trywait` retry logic with 100×100us backoff — preserved
- All ContentApp source files — see SPEC-004
- All other Common/src/ files

### MINIMAL TOUCH (~2 LOC in `GW.cpp`)
- L604: Change `elapsed >= 100` to `elapsed >= 10`
- Optionally: make the poll interval a named constant instead of a magic number

---

## 3. Codebase Facts (verified 2026-06-23)

| Fact | Detail |
|------|--------|
| **NG-005 Phase 1+2 replaced 1ms busy-wait with CV-based wait** | Before: `while (StopGateway == false) { ... process ... }` with 1ms sleep. After: `InputQueueCV.wait_for(lock, waitTimeout, predicate)` — the gateway sleeps until next message is due or SHM needs polling. |
| **SHM poll rate-limited to 100ms** | `GW.cpp` L589-L608: A `static` `lastSHMPoll` timestamp is kept. SHM is only polled when `!hasDueMessage && elapsed >= 100`. This means: (a) if there ARE due messages in InputQueue, SHM is not polled (messages are processed first); (b) if there are NO due messages, SHM is polled at most every 100ms. |
| **100ms was chosen to limit CPU usage** | The original 1ms busy-wait caused ~100% CPU. The 100ms poll gives ~10 polls/sec when idle, which is a 100× reduction in poll frequency. However, 100ms adds noticeable latency to SHM message delivery. |
| **The CV wait timeout is separate from the SHM poll** | `GW.cpp` L461: `waitTimeout = secondsUntilNext * 1000ms`. The CV wait is based on the next scheduled message time. The SHM poll is an additional check that happens after the CV wait returns. If `secondsUntilNext` is large (e.g., 10s for ContentApp periodic), the gateway could sleep for 10s without polling SHM — UNLESS a message arrives via CV notification. But SHM messages do NOT trigger CV notification (they are polled, not pushed). |
| **SHM messages do not trigger InputQueueCV** | `ReadFromSharedMemory3()` reads from SHM and pushes to `InputQueue`, but it does NOT call `InputQueueCV.notify_one()`. The CV is only notified by `PushToInputQueue()` (internal process messages). This means SHM messages are ONLY detected by the poll — they cannot wake the CV early. |
| **This is a latent bug**: If `secondsUntilNext = 10s` (ContentApp periodic interval), the gateway will sleep for 10s without polling SHM. SHM messages (hello IPC, discovery responses, service offers) arriving during this sleep will not be processed for up to 10s. | The 100ms rate limit only applies when `!hasDueMessage && elapsed >= 100`. But the CV `wait_for` can sleep for `secondsUntilNext` (up to 1s default when queue empty, or up to 10s when a periodic message is scheduled). After the CV wait returns, the 100ms check runs. So the effective poll interval is `min(secondsUntilNext, 100ms)` ONLY when the queue is empty. When the queue has a scheduled message, the poll interval is `secondsUntilNext` (could be 10s). |
| **Wait — re-examine**: The CV `wait_for` with timeout will return either when notified or when the timeout expires. After it returns, the code checks for due messages, processes them, then checks the SHM poll. If `secondsUntilNext = 10s`, the CV sleeps up to 10s. During this time, SHM is NOT polled. After the CV returns (either by timeout or notification), `elapsed` is checked against 100ms. If the last poll was 10s ago, `elapsed >= 100` is true, so SHM is polled. | So the effective SHM poll interval is: `max(CV_wait_timeout, 100ms)` when idle. If `secondsUntilNext = 1.0s` (default when queue empty), SHM is polled every ~1s. If `secondsUntilNext = 10s` (periodic scheduled), SHM is polled every ~10s. This is MUCH worse than 100ms. |
| **The real SHM poll interval is dominated by secondsUntilNext, not 100ms** | The 100ms rate limit is a MINIMUM, not a maximum. It prevents polling MORE often than 100ms. But it does NOT ensure polling AT LEAST every 100ms. The actual poll interval is `max(secondsUntilNext, 100ms)`. |
| **This means the ContentApp's 10s periodic interval causes up to 10s SHM latency** | When a ContentApp has a periodic message scheduled 10s in the future, the gateway CV sleeps for up to 10s. During this sleep, SHM messages (hello IPC, discovery responses) are not polled. They arrive in SHM but are not read until the CV timeout expires. |

---

## 4. Architecture

### Current Gateway Loop (simplified)

```
Gateway() {
  while (!StopGateway) {
    // 1. Calculate secondsUntilNext
    secondsUntilNext = InputQueue.top()->GetTime() - GetTime();
    // (or 1.0s default if queue empty)

    // 2. Sleep on CV with timeout = secondsUntilNext
    InputQueueCV.wait_for(lock, secondsUntilNext * 1000ms, ...);
    // NOTE: SHM messages do NOT notify this CV!
    // Only internal PushToInputQueue() calls notify.

    // 3. Process all due messages from InputQueue
    while (InputQueue has due messages) {
      Run(message);
    }

    // 4. Poll SHM (rate-limited to 100ms)
    if (!hasDueMessage && elapsed_since_last_SHM_poll >= 100ms) {
      ReadFromSharedMemory3();
      // This pushes new SHM messages to InputQueue
      // But does NOT notify CV (messages are now in queue,
      // will be processed on next loop iteration)
    }
  }
}
```

### The Problem

```
Timeline with secondsUntilNext = 10s (ContentApp periodic):

t=0s:   CV wait starts (timeout=10s)
t=1s:   PGCS sends hello IPC via SHM → arrives in SHM buffer
        ... NOT polled! Gateway is sleeping on CV ...
t=5s:   NRNCS sends discovery response via SHM → arrives in SHM buffer
        ... NOT polled! Gateway is sleeping on CV ...
t=10s:  CV timeout expires
        Gateway wakes up
        Processes due message from InputQueue
        Polls SHM → finds hello + discovery response
        Pushes to InputQueue
        Next iteration: processes them

Result: hello and discovery response waited 9s / 5s in SHM buffer
```

### Proposed Fix

```
Option A: Reduce CV timeout to max(secondsUntilNext, 100ms)
  → Gateway wakes every 100ms max, polls SHM
  → CPU: ~10 wakeups/sec (vs ~1/sec now, vs ~1000/sec pre-NG-005)

Option B: Reduce CV timeout to max(secondsUntilNext, 10ms)
  → Gateway wakes every 10ms max, polls SHM
  → CPU: ~100 wakeups/sec (vs ~1/sec now)
  → Still 10× better than pre-NG-005 (1000/sec)

Option C: Have ReadFromSharedMemory3() notify CV after pushing messages
  → But ReadFromSharedMemory3 is called BY the gateway, not by external threads
  → This doesn't help — the gateway needs to wake up to call it in the first place

Option D: Use a dedicated SHM polling thread
  → A separate thread that polls SHM every 10ms and notifies CV when messages arrive
  → Most architecturally clean, but adds a thread + complexity
  → Overkill for this optimisation round
```

**Recommended: Option B (10ms)** — balances latency and CPU. 100 wakeups/sec is acceptable for a single-threaded gateway and is 10× better than the original 1ms busy-wait.

If CPU is a concern, Option A (100ms) is also acceptable — it caps SHM latency at 100ms instead of 10s, which is a 100× improvement over the current situation.

---

## 5. Implementation Details

### Option B: 10ms SHM poll interval (~5 LOC)

| File | Change | LOC |
|------|--------|-----|
| `Common/src/GW.cpp` L604 | Change `elapsed >= 100` to `elapsed >= 10` | 1 LOC |
| `Common/src/GW.cpp` L461 | Cap `waitTimeout` at 10ms: `waitTimeout = std::chrono::milliseconds(std::min((long long)(secondsUntilNext * 1000), 10LL));` | 1 LOC |
| `Common/src/GW.cpp` (near L433) | Add named constant: `constexpr long long SHM_POLL_INTERVAL_MS = 10;` | 1 LOC |

The key change is capping the CV wait timeout. Currently:
```cpp
waitTimeout = std::chrono::milliseconds((long long)(secondsUntilNext * 1000));
```
This sleeps for `secondsUntilNext` seconds (up to 10s). After the change:
```cpp
waitTimeout = std::chrono::milliseconds(
    std::min((long long)(secondsUntilNext * 1000), SHM_POLL_INTERVAL_MS));
```
This sleeps for at most 10ms, ensuring SHM is polled at least every 10ms.

**Side effect**: The gateway will wake 100× per second when idle. Each wakeup is a quick check (is queue empty? is SHM poll due?) that returns immediately if nothing to do. The CPU cost of 100 wakeup/sec is negligible compared to the original 1000/sec busy-wait.

---

## 6. Etapas

### Etapa E0: Preflight

**Change:** None

**Test:** Verify `GW.cpp` L589-L608 and L458-L467 match the analysis in SPEC-005 §3. Confirm `git status` clean on `AIOPT2`.

**Done when:** Code verified, git clean.

---

**Status:** Implemented 2026-06-23. Changes verified: SHM_POLL_INTERVAL_MS=10, CV timeout capped, poll condition updated.

### Etapa E1: Apply SHM poll interval reduction ✅ (2026-06-23)

**Change:** `Common/src/GW.cpp` ~5 LOC

1. Add near L433: `constexpr long long SHM_POLL_INTERVAL_MS = 10;`
2. L461: `waitTimeout = std::chrono::milliseconds(std::min((long long)(secondsUntilNext * 1000), SHM_POLL_INTERVAL_MS));`
3. L604: `if (!hasDueMessage && elapsed >= SHM_POLL_INTERVAL_MS)`

**Test:** Recompile PGCS + NRNCS + ContentApp. Run 4-process test. Check:
- CPU usage is reasonable (should be < 20% per process, not 100%)
- SHM messages arrive within 10-20ms (check log timestamps)
- No functional regression

**Done when:** All 4 processes run for 60s without errors. CPU < 20% per process. No SIGSEGV.

**Pitfall:** If CPU spikes back to ~100%, increase `SHM_POLL_INTERVAL_MS` to 50 or 100. The 10ms value is a trade-off — if it's too aggressive, 50ms is still 200× better than the current 10s.

---

### Etapa E2: E2E smoke test with SPEC-004

**Change:** None — test only

**Test:** Run the full SPEC-004 E4 test with SPEC-005 applied. Measure:
- Time from hello IPC send to hello IPC receive (SHM latency)
- Time from discovery response to processing (SHM latency)
- Total time to first photo (should be unchanged from SPEC-004 Level A/B since SHM latency was not the bottleneck)

**Done when:** SHM latency < 20ms. No regression in functional tests.

---

### Etapa E3: Commit

**Change:** Git commit

```bash
git add Common/src/GW.cpp
git commit -m "perf(gw): reduce SHM poll interval from 100ms to 10ms (SPEC-005)

- Cap CV wait timeout at 10ms to ensure SHM is polled frequently
- Fixes latency regression from NG-005 Phase 1+2 (100ms→10s SHM poll)
- CPU: ~100 wakeups/sec (10× better than pre-NG-005 1ms busy-wait)
- Ref: SPEC-005, NG-042-07"
```

**Done when:** Commit pushed to `AIOPT2`.

---

## 7. Testing Plan

| Test | Method | Pass criteria |
|------|--------|---------------|
| CPU usage | `top -p <PID>` during 60s run | < 20% per process |
| SHM latency | Compare log timestamps between sender and receiver | < 20ms |
| Hello IPC delivery | ContentApp log shows "Discovered" within 2s of PGCS hello | Within 2s |
| Discovery completion | Full cycle completes | No timeout errors |
| No busy-wait regression | CPU does NOT spike to 100% | < 20% sustained |
| Long-running stability | Run for 5 minutes | No crash, no memory growth |

---

## 8. Risks

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| CPU usage increase from 100 wakeups/sec | Medium | CPU goes from ~1% to ~5-10% | Acceptable. If too high, increase to 50ms. |
| CV wait timeout too short → unnecessary wakeups | Low | Minor CPU waste | The wakeup checks `InputQueue.empty()` and returns immediately. Cost is negligible. |
| Interaction with sem_trywait backoff | Low | Semaphore contention | The 100×100us backoff (10ms max) is independent of the poll interval. No interaction. |
| `std::min` type mismatch | Low | Compile error | Use explicit casts: `std::min((long long)(secondsUntilNext * 1000), SHM_POLL_INTERVAL_MS)` |

---

## 9. Open Questions

| # | Question | Options | Default proposed | When to decide |
|---|----------|---------|------------------|----------------|
| Q1 | Poll interval: 10ms or 50ms or 100ms? | (a) 10ms (b) 50ms (c) 100ms | (a) 10ms | Before E1 |
| Q2 | Apply this spec before or after SPEC-004? | (a) Before (b) After (c) together | (c) together | Before E1 |

---

## 10. Decisões Aprovadas

| # | Decisão | Data | Rationale |
|---|---------|------|-----------|
| D1 | SHM poll interval: **10ms** | 2026-06-23 | User chose 10ms (Q1). Most aggressive option, 10× better than pre-NG-005. |
| D2 | Apply together with SPEC-004 | 2026-06-23 | User confirmed (Q2). Both specs implemented in same session. |

---

## 11. Pitfalls Discovered During Design

1. **The 100ms rate limit is a MINIMUM, not a MAXIMUM**: The code `if (elapsed >= 100)` prevents polling MORE often than 100ms. But the CV `wait_for` can sleep for `secondsUntilNext` (up to 10s), which means the actual poll interval is `max(secondsUntilNext, 100ms)`. The 100ms constant only matters when `secondsUntilNext < 100ms`. For ContentApp with 10s periodic, the effective SHM poll interval is 10s, not 100ms. This is the key finding of this spec.

2. **SHM messages do not trigger CV notification**: `ReadFromSharedMemory3()` pushes to `InputQueue` but does not call `InputQueueCV.notify_one()`. This is by design — `ReadFromSharedMemory3()` is called from within the gateway loop, not from an external thread. But it means SHM messages can only be detected by polling, not by event-driven wakeup.

3. **The fix must cap the CV timeout, not just the poll interval**: Just changing `elapsed >= 100` to `elapsed >= 10` at L604 is NOT sufficient. The gateway will still sleep for `secondsUntilNext` on the CV. The fix must ALSO cap `waitTimeout` at L461 to ensure the gateway wakes up frequently enough to reach the SHM poll check.

4. **Static variable in Gateway()**: `static auto lastSHMPoll` at L590 is a function-local static. It is shared across all calls to `Gateway()`. Since there is only one gateway per process, this is correct. But it means the variable persists across test runs within the same process — not an issue since processes are restarted between tests.

---

## 12. Acceptance Criteria

- [x] `SHM_POLL_INTERVAL_MS` constant defined in `GW.cpp`
- [x] CV wait timeout capped at `SHM_POLL_INTERVAL_MS`
- [x] SHM poll condition uses `SHM_POLL_INTERVAL_MS`
- [ ] PGCS + NRNCS + ContentApp compile successfully
- [ ] CPU usage < 20% per process during 60s run
- [ ] SHM message latency < 20ms
- [ ] No functional regression (hello, discovery, offer, acceptance, publish all work)
- [ ] No SIGSEGV or memory errors
- [ ] Commit pushed to `AIOPT2` branch
- [ ] Dashboard updated with SPEC-005 reference
