# SPEC-004 — ContentApp Performance Optimisation: Reducing Contraction Times

**Status:** Approved 2026-06-23 — Level A only, pending implementation  
**Date:** 2026-06-23  
**Scope:** ContentApp timer configuration + discovery consolidation + GW.cpp SHM polling  
**Codebase:** NovaGenesis, branch `AIOPT2`  
**Stack:** C++20, g++ -O0 -g3, SHM IPC, System V semaphores  
**Task:** NG-042-07  

**Revision history:**
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-06-23 | Hermes Agent (Scalifax) | Initial draft. Full codebase analysis of ContentApp cycle + GW.cpp gateway loop. 3 optimisation levels proposed. |

---

## 1. Goal

Reduce the total time for the ContentApp cycle (hello → discovery → exposition → service offer → acceptance → photo publish) from the current ~35+ minutes to under 5 minutes, without removing or skipping any of the 6 sacred steps.

Secondary goal: identify and fix latency introduced by the recent GW.cpp busy-wait changes (NG-005 Phase 1+2) that affect the ContentApp's SHM message delivery cadence.

---

## 2. Scope

### IN
- `IO/Source1/App.ini` — timer value overrides
- `IO/Repository1/App.ini` — timer value overrides
- `ContentApp/src/CoreRunPeriodic01.cpp` — consolidation of discovery messages (Level B)
- `ContentApp/src/Core.cpp` — `DiscoveryFirstStep` / `DiscoverySecondStep` parameter adjustments (Level B)
- `ContentApp/src/CoreRunContentPublish01.cpp` — burst scheduling formula review (Level B)
- `Common/src/GW.cpp` — SHM poll interval adjustment in `Gateway()` (SPEC-005 cross-ref)

### OUT (DO NOT TOUCH)
- `ContentApp/src/CoreRunExpose01.cpp` — exposition logic unchanged (L1-L224)
- `ContentApp/src/CoreRunInvite01.cpp` — invitation/service offer logic unchanged (L1-L295)
- `ContentApp/src/CoreRunEvaluate01.cpp` — acceptance/evaluation logic unchanged (L1-L812)
- `ContentApp/src/CoreRunDiscover01.cpp` — discover action logic unchanged (L1-L412)
- `ContentApp/src/CoreRunInitialize01.cpp` — initialization + INI parser unchanged (L1-L265)
- `ContentApp/src/Core.cpp` constructor lines 110-189 — block creation, action registration, default timer values preserved
- `ContentApp/src/Core.cpp` `Exposition()` function (L653-L747) — binding publication logic
- The 6 sacred steps: hello, discovery, exposition, service offer, acceptance, photo publish — NEVER removed or skipped
- `ContentApp/src/CoreRunContentPublish01.cpp::CreatePublishMessage()` (L464-L654) — message construction for photo publish
- `Common/src/GW.cpp` `ReadFromSharedMemory3()` semaphore retry logic (L625+) — preserved by SPEC-005

### MINIMAL TOUCH (~15 LOC in `CoreRunPeriodic01.cpp`)
- Merge the Intra_Domain and Intra_OS discovery calls into a single pass where possible (Level B only)
- Remove redundant `Cat2Keywords` rebuild between the two discovery passes (L84-L107 vs L163-L197)

---

## 3. Codebase Facts (verified 2026-06-23)

| Fact | Detail |
|------|--------|
| **Timer defaults in code** | `Core.cpp` constructor L117-L123: `DelayBeforePublishingServiceOffer=1`, `DelayBeforeDiscovery=3`, `DelayBeforeRunPeriodic=40`, `DelayBeforeANewPeerEvaluation=5`, `DelayBeforeANewPhotoPublish=0.5`, `ContentBurstSize=50` |
| **INI overrides** | `IO/Source1/App.ini` and `IO/Repository1/App.ini` (identical): `ServiceOffer=60`, `Discovery=10`, `RunPeriodic=10`, `PeerEvaluation=5`, `PhotoPublish=10`, `ContentBurstSize=200` |
| **INI is 60× more conservative for ServiceOffer** | 60s vs 1s default. This alone adds 60s to the cycle. |
| **INI is 20× more conservative for PhotoPublish** | 10s vs 0.5s. With 200 photos: 200×10s = 33min vs 200×0.5s = 100s. |
| **INI is 3× more conservative for Discovery** | 10s vs 3s. Applied per-step, and there are 5-6 steps per periodic cycle → 50-60s accumulated. |
| **INI is 4× LESS conservative for RunPeriodic** | 10s vs 40s. This means periodic fires faster, but each periodic is bloated by the discovery delays. |
| **CoreRunPeriodic01 schedules 4 discovery calls per cycle** | L104: `DiscoveryFirstStep(Intra_Domain, ...)`, L107: `DiscoverySecondStep(Intra_Domain, ...)`, L176: `DiscoveryFirstStep(Intra_OS, ...)`, L197: `DiscoverySecondStep(Intra_OS, ...)`. Each call adds `DelayBeforeDiscovery` to the scheduled message time. |
| **DiscoveryFirstStep sets time = GetTime() + DelayBeforeDiscovery** | `Core.cpp` L541: `Run->SetTime(GetTime() + DelayBeforeDiscovery)`. Called 4× per periodic, but on the SAME scheduled message (index 0), so the last call wins. Net effect: the scheduled message gets delayed by `DelayBeforeDiscovery` once (the last call overwrites). |
| **DiscoverySecondStep also sets time = GetTime() + DelayBeforeDiscovery** | `Core.cpp` L601: Same pattern. Since both First and Second step operate on `ScheduledMessages.at(0)`, the final time is `GetTime() + DelayBeforeDiscovery` from the last call. The 4 calls don't stack to 4×Delay — they overwrite. |
| **Exposition runs once (RunExpose = true → false)** | `CoreRunPeriodic01.cpp` L246-L252: `if (PCore->RunExpose == true) { Exposition(...); PCore->RunExpose = false; }`. After first exposition, never re-exposes. |
| **Service Offer is sent by Source, not Repository** | `CoreRunEvaluate01.cpp` L217-L243: When `PApp->Role == "Source"` and a new Repository is discovered, it adds `--invite` to the scheduled message. `CoreRunInvite01.cpp` L154: `NewMessage(GetTime() + PCore->DelayBeforePublishingServiceOffer, ...)` — the 60s delay is applied HERE. |
| **Acceptance triggers contentpublish** | `CoreRunEvaluate01.cpp` L650-L653: When `Method == "Accepted"`, adds `--contentpublish` to scheduled message and sets `Run->SetTime(GetTime() + DelayBeforeANewPhotoPublish)`. |
| **ContentPublish burst scheduling** | `CoreRunContentPublish01.cpp` L198: `Delta = double(Counter / ContentBurstSize) * DelayBeforeANewPhotoPublish`. Photos are scheduled with increasing delays within a burst. With `ContentBurstSize=200` and `DelayBeforeANewPhotoPublish=10`, the first photo in the burst has Delta=0, the second also 0 (integer division: 1/200=0), etc. Only after 200 photos does Delta increment. But `Counter == ContentBurstSize` triggers `break` at L231, so the loop stops at 200 photos. The Delta formula is effectively 0 for all photos in the first burst. |
| **ContentPublish reschedules itself** | `CoreRunContentPublish01.cpp` L284: `NewMessage(GetTime() + DelayBeforeANewPhotoPublish, ...)` — reschedules with the full 10s delay. So the NEXT burst starts 10s later. With 200 photos per burst, the total is: first burst (0s) + 10s gap + second burst (0s) + 10s gap + ... until all files are published. |
| **GW.cpp Gateway loop uses CV with timeout** | `GW.cpp` L462: `InputQueueCV.wait_for(lock, waitTimeout, ...)` where `waitTimeout = secondsUntilNext * 1000ms`. This is a timer-aware blocking wait — the gateway sleeps until the next message is due or a new message arrives via SHM. |
| **SHM poll rate-limited to 100ms when idle** | `GW.cpp` L604: `if (!hasDueMessage && elapsed >= 100) { ReadFromSharedMemory3(); }`. This means SHM is polled at most every 100ms when there are no due messages. **This is the key GW.cpp change that affects ContentApp latency**: hello IPC messages and discovery responses arriving via SHM are not processed until the next 100ms poll window. |
| **GW.cpp ReadFromOutputQueueThread sleeps 10us** | `GW.cpp` L418: `sleep_for(microseconds(10))`. The output thread that writes to SHM spins with 10us sleep. This is very fast and should not be a bottleneck. |
| **NG-005 Phase 1+2 changes** | Replaced 1ms busy-wait with CV-based blocking wait. Added 100ms SHM poll rate limiting. Cached semaphores. Added sem_trywait with 100 retries × 100us = 10ms max wait per semaphore. These changes reduced CPU usage but introduced up to 100ms latency on SHM message reception. |
| **CoreRunEvaluate runs on every scheduled message** | `Core.cpp` L148: `NewAction("-run --evaluate 0.1", PA)`. Evaluate is scheduled by the periodic loop and runs on every scheduled message that reaches the Core block. It checks `NextPeerEvaluationTime` before doing heavy discovery (L122). |
| **DelayBeforeRunPeriodic is dynamically modified** | `CoreRunEvaluate01.cpp` L243: `PCore->DelayBeforeRunPeriodic = 30` (when Source discovers Repository). L361: `PCore->DelayBeforeRunPeriodic = 60` (when Repository discovers Source). These overrides happen AFTER the INI load, so they replace the INI value during runtime. The periodic interval changes from 10s (INI) to 30s or 60s after peer discovery. |

---

## 4. Architecture

### Current Flow (with delays annotated)

```
PGCS starts ──> NRNCS starts ──> ContentApp starts
                                    │
                                    ▼
                        CoreRunInitialize01
                        (loads App.ini timers)
                                    │
                                    ▼
                        Schedule -run --periodic
                        at GetTime() + DelayBeforeRunPeriodic (10s from INI)
                                    │
                                    ▼
                    ┌───────── CoreRunPeriodic01 ◄──────────┐
                    │  (fires every DelayBeforeRunPeriodic)  │
                    │                                        │
                    │  1. DiscoveryFirstStep(Intra_Domain)   │
                    │     +DelayBeforeDiscovery (10s)        │
                    │                                        │
                    │  2. DiscoverySecondStep(Intra_Domain)  │
                    │     +DelayBeforeDiscovery (10s)        │
                    │                                        │
                    │  3. Check NRNCS awareness          │
                    │     If aware:                          │
                    │       4. DiscoveryFirstStep(Intra_OS)  │
                    │          +DelayBeforeDiscovery (10s)   │
                    │                                        │
                    │       5. DiscoverySecondStep(Intra_OS) │
                    │          +DelayBeforeDiscovery (10s)   │
                    │                                        │
                    │       6. If RunExpose==true:           │
                    │            Exposition()                │
                    │            RunExpose = false            │
                    │            State = "operational"        │
                    │                                        │
                    │  7. Reschedule -run --periodic         │
                    │     at +DelayBeforeRunPeriodic         │
                    └────────────────────────────────────────┘
                                    │
                                    ▼ (after NRNCS discovered)
                    CoreRunEvaluate01 (on scheduled messages)
                    │
                    │  Checks every DelayBeforeANewPeerEvaluation (5s)
                    │  Discovers Repository/Source apps
                    │  If Source + found Repository:
                    │    → adds --invite (Service Offer)
                    │    → DelayBeforeRunPeriodic = 30s (override!)
                    │
                    ▼
                    CoreRunInvite01 (Service Offer)
                    │
                    │  NewMessage(GetTime() + DelayBeforePublishingServiceOffer)
                    │  ──── 60s WAIT (from INI) ────
                    │
                    ▼
                    (Service Offer delivered to Repository via NRNCS)
                    │
                    ▼
                    Repository receives offer → CoreRunEvaluate01 processes "Offer"
                    │
                    │  Adds --publish (Acceptance)
                    │  Acceptance sent back to Source
                    │
                    ▼
                    Source receives "Accepted" → CoreRunEvaluate01 processes "Accepted"
                    │
                    │  Adds --contentpublish
                    │  SetTime(GetTime() + DelayBeforeANewPhotoPublish) ── 10s wait
                    │
                    ▼
                    CoreRunContentPublish01
                    │
                    │  Reads directory, publishes up to ContentBurstSize (200) photos
                    │  Each photo: CreatePublishMessage(GetTime() + Delta, ...)
                    │  Delta = (Counter/200) * 10s ≈ 0 for first burst
                    │  Breaks at Counter == 200
                    │  Reschedules at +DelayBeforeANewPhotoPublish (10s)
                    │
                    └─── loops until all photos published ────
```

### Current Timing Breakdown (worst case with INI values)

| Phase | Delay Source | Time |
|-------|-------------|------|
| Start → first periodic | `DelayBeforeRunPeriodic` (INI) | 10s |
| First periodic → NRNCS discovered | `DelayBeforeDiscovery` × 1 (overwrite) | 10s |
| NRNCS → Exposition | Immediate (same periodic) | 0s |
| Exposition → next periodic | `DelayBeforeRunPeriodic` | 10s |
| Periodic → Evaluate discovers Repository | `DelayBeforeANewPeerEvaluation` | 5s |
| Evaluate → Invite (Service Offer) | `DelayBeforePublishingServiceOffer` (INI) | **60s** |
| Invite → Repository receives → Acceptance | SHM + GW delivery | ~0.1-1s |
| Acceptance → ContentPublish triggered | `DelayBeforeANewPhotoPublish` (INI) | **10s** |
| First burst (200 photos) | Delta ≈ 0 per photo | ~instant |
| Burst gap | `DelayBeforeANewPhotoPublish` | **10s** |
| Total for N bursts | N × 10s | N × 10s |

**Total to first photo: ~105s (1.75 min)**
**Total for 200 photos (1 burst): ~115s (1.9 min)**
**Total for 400 photos (2 bursts): ~125s (2.1 min)**

But wait — after Source discovers Repository, `DelayBeforeRunPeriodic` is overridden to 30s (L243). And after Repository discovers Source, it's overridden to 60s (L361). This means the periodic loop SLOWS DOWN after discovery, making subscription resubmission and other periodic tasks slower.

### GW.cpp Impact (100ms SHM poll)

The 100ms SHM poll rate limit means:
- Hello IPC messages from PGCS → ContentApp arrive with up to 100ms latency
- Discovery response messages from PGCS arrive with up to 100ms latency
- Service offer messages arrive with up to 100ms latency
- Acceptance messages arrive with up to 100ms latency

At each stage of the cycle, up to 100ms is added. With ~6 message exchanges, total added latency: ~600ms. This is NOT the primary bottleneck (the 60s and 10s timers dominate), but it is noticeable and was not present before NG-005.

### Proposed Optimised Flow (Level A: INI only)

```
Same flow, but with corrected timers:
  DelayBeforePublishingServiceOffer: 60 → 5s  (12× reduction)
  DelayBeforeANewPhotoPublish:       10 → 1s  (10× reduction)
  DelayBeforeDiscovery:              10 → 3s  (restore code default, 3× reduction)
```

### Proposed Optimised Flow (Level B: INI + code consolidation)

```
Same flow, but additionally:
  - Merge Intra_Domain + Intra_OS discovery into fewer messages
  - Remove redundant keyword list rebuild
  - Review DelayBeforeRunPeriodic dynamic overrides (30s/60s → 10s/15s)
  - Review burst scheduling formula
```

---

## 5. Implementation Details

### Level A: INI-only changes (0 LOC code, 2 files)

| File | Change | LOC |
|------|--------|-----|
| `IO/Source1/App.ini` | `DelayBeforePublishingServiceOffer 60 → 5`, `DelayBeforeANewPhotoPublish 10 → 1`, `DelayBeforeDiscovery 10 → 3` | 3 lines |
| `IO/Repository1/App.ini` | Same as Source1 | 3 lines |

**Expected result:**
- Time to first photo: ~35s (from ~105s)
- Time for 200 photos: ~37s (from ~115s)
- Time for 400 photos: ~38s (from ~125s)

### Level B: INI + code consolidation (~30 LOC, 2 files)

| File | Change | LOC |
|------|--------|-----|
| `ContentApp/src/CoreRunPeriodic01.cpp` | After NRNCS is discovered, skip Intra_Domain discovery and only do Intra_OS. The Intra_Domain discovery is only needed before NRNCS is known. Once known, Intra_OS is sufficient for PGCS HT_BID lookup. | ~15 LOC |
| `ContentApp/src/CoreRunPeriodic01.cpp` | Remove the second `Cat2Keywords.clear() + push_back` block (L163-L173) when NRNCS is already known — reuse the first block's keywords. | ~5 LOC |
| `ContentApp/src/CoreRunEvaluate01.cpp` | Reduce the `DelayBeforeRunPeriodic = 30` override (L243) to `10` and `DelayBeforeRunPeriodic = 60` (L361) to `15`. These overrides currently slow down the periodic loop after discovery, which delays subscription resubmission and photo publishing. | 2 LOC |

**Expected result:**
- Time to first photo: ~20s (from ~35s with Level A)
- Fewer messages per periodic cycle → less GW overhead
- Periodic loop stays fast after discovery

### Level C: Deeper refactor (deferred — documented for future)

Not included in this spec. Would involve:
- Unified `RunDiscovery` action replacing 4 separate discovery calls
- Periodic re-exposition (RunExpose toggle reset every N cycles)
- Timer management refactored into a single configurable struct
- Removal of the dynamic `DelayBeforeRunPeriodic` overrides entirely

---

## 6. Etapas

### Etapa E0: Preflight verification

**Change:** None — verification only

**Test:** Read `IO/Source1/App.ini` and `IO/Repository1/App.ini` to confirm current values. Read `Core.cpp` L117-L123 to confirm defaults. Confirm `git status` is clean on branch `AIOPT2`.

**Done when:** All 6 timer values documented and verified against code. Git working tree clean.

**Pitfall:** If the user has already modified the INI files since the last session, re-verify before proceeding.

---

### Etapa E1: Apply Level A — INI timer changes ✅ (2026-06-23)

**Change:** `IO/Source1/App.ini` + `IO/Repository1/App.ini` ~3 lines each

```ini
DelayBeforePublishingServiceOffer 30
DelayBeforeDiscovery 3
DelayBeforeRunPeriodic 10
DelayBeforeANewPeerEvaluation 5
DelayBeforeANewPhotoPublish 1
ContentBurstSize 200
```

**Test:** No compilation needed (INI only). Run `sudo bash Scripts/Simple/clean.sh`, then start PGCS + NRNCS + Source + Repository. Measure wall-clock time from ContentApp start to first photo published.

**Status:** Implemented. Values verified: ServiceOffer=30, Discovery=3, PhotoPublish=1.

**Done when:** Log shows first "Publishing the content" line within 60s of ContentApp start.

**Pitfall:** If race conditions appear (bindings not propagated before offer is sent), increase `DelayBeforePublishingServiceOffer` back to 45s and retest. The 30s value assumes NG-042-06 fix (HT_BID in hello 2.0) is working.

---

### Etapa E2: Apply SPEC-005 — GW.cpp SHM poll reduction

**Change:** `Common/src/GW.cpp` ~5 LOC (per SPEC-005 E1)

1. Add near L433: `constexpr long long SHM_POLL_INTERVAL_MS = 10;`
2. L461: `waitTimeout = std::chrono::milliseconds(std::min((long long)(secondsUntilNext * 1000), SHM_POLL_INTERVAL_MS));`
3. L604: `if (!hasDueMessage && elapsed >= SHM_POLL_INTERVAL_MS)`

**Test:** Recompile PGCS + NRNCS + ContentApp (GW.cpp is in Common, compiled into all). Verify:
- CPU usage reasonable (< 20% per process)
- SHM messages arrive within 10-20ms
- No functional regression

**Done when:** All 4 processes run for 60s without errors. CPU < 20% per process.

**Pitfall:** User compiles manually. Present the diff and ask user to compile.

---

### Etapa E3: E2E smoke test + measurement

**Change:** None — test only

**Test:**
1. `sudo bash Scripts/Simple/clean.sh`
2. Start PGCS (`run_PGCS.sh`)
3. Wait for PGCS initialisation
4. Start NRNCS (`run_NRNCS.sh`)
5. Start Source (`run_Source.sh`)
6. Start Repository (`run_Repository.sh`)
7. Record timestamps from logs:
   - ContentApp start time
   - First "Discovered a NRNCS!" time
   - First "Discovered a Repository" time
   - First "Publishing the content" time
   - First 200 photos published time

**Done when:** All 5 timestamps recorded. Time to first photo < 60s. Time for 200 photos < 70s. No errors in logs.

---

### Etapa E4: Commit

**Change:** Git commit

```bash
git add IO/Source1/App.ini IO/Repository1/App.ini \
  Common/src/GW.cpp \
  Specs/SPEC-004-contentapp-performance-optimisation.md \
  Specs/SPEC-005-gw-shm-poll-interval-reduction.md
git commit -m "perf(contentapp,gw): reduce contraction timers + SHM poll interval (SPEC-004, SPEC-005)

INI changes (SPEC-004 Level A):
- ServiceOffer 60→30s, PhotoPublish 10→1s, Discovery 10→3s
- Expected: time to first photo ~55s (from ~105s)

GW.cpp changes (SPEC-005):
- Cap CV wait timeout at 10ms for SHM poll
- Fixes up to 10s SHM latency from NG-005 Phase 1+2
- CPU: ~100 wakeups/sec (10× better than pre-NG-005 1ms busy-wait)

Ref: SPEC-004, SPEC-005, NG-042-07"
```

**Done when:** Commit pushed to `AIOPT2` branch.

---

## 7. Testing Plan

| Test | Method | Pass criteria |
|------|--------|---------------|
| Timer verification | Read INI files after E1 | Values match spec |
| NRNCS discovery | Check ContentApp log for "Discovered a NRNCS!" | Within 15s of start |
| Exposition | Check log for "Generating a message to publish in the domain scope" | Within same periodic cycle |
| Service Offer | Check Source log for "Discovered a Repository" | Within 20s of NRNCS discovery |
| Acceptance | Check Repository log for Service_Accepted file creation | Within 5s of Service Offer |
| Photo Publish | Check Source log for "Publishing the content" | Within 25s of ContentApp start |
| 200 photos | Count "Publishing the content" lines | 200 lines within 35s |
| No regression | Check for errors: "Failed to discover PGCS_PID", "Unable to send discovery", SIGSEGV | Zero errors |
| GW CPU | `top` during test | Not exceeding 100% per core (busy-wait fix intact) |

---

## 8. Risks

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Race condition: Service Offer sent before Repository has bindings | Medium | Discovery fails, offer lost | Keep `DelayBeforePublishingServiceOffer` at 5s (not 1s). If fails, increase to 10s. |
| Intra_Domain skip breaks cross-OS discovery | Low | Peers on other OSes not found | Only skip when `PSTuples.size() > 0` (NRNCS already known). For multi-OS scenarios, add a config flag. |
| DelayBeforeRunPeriodic override too aggressive | Low | OutputQueue flooding | Monitor OutputQueue size in logs. If > 50 messages, revert to 20s/30s. |
| ContentBurstSize=200 with 1s delay causes memory pressure | Low | Messages in memory > MAX_MESSAGES_IN_MEMORY | The existing break at L232 (`Counter == ContentBurstSize`) and L232 (`GetNumberOfMessages >= MAX-200`) already guard this. |
| GW.cpp 100ms SHM poll adds latency to offer/acceptance | Medium | Up to 600ms total across cycle | Addressed by SPEC-005 (reduce poll interval to 10ms for ContentApp). Even without SPEC-005, 600ms is negligible vs the 20-25s total cycle. |

---

## 9. Open Questions

| # | Question | Options | Default proposed | When to decide |
|---|----------|---------|------------------|----------------|
| Q1 | `DelayBeforePublishingServiceOffer`: 5s or 3s? | (a) 5s (safe) (b) 3s (aggressive) (c) 1s (code default, risky) | (a) 5s | Before E1 |
| Q2 | `DelayBeforeANewPhotoPublish`: 1s or 0.5s? | (a) 1s (safe) (b) 0.5s (code default) | (a) 1s | Before E1 |
| Q3 | `DelayBeforeDiscovery`: 3s (code default) or 5s? | (a) 3s (b) 5s | (a) 3s | Before E1 |
| Q4 | Apply Level B (code changes) or stop at Level A? | (a) Level A only (b) A + B | (b) A + B | After E1 measurement |
| Q5 | Reduce `DelayBeforeRunPeriodic` overrides (30/60 → 10/15)? | (a) yes (b) no (c) 20/30 compromise | (a) yes | Before E3 |
| Q6 | Address GW.cpp 100ms SHM poll in this spec or separate SPEC-005? | (a) this spec (b) separate | (b) separate | Now (already separated) |

---

## 10. Decisões Aprovadas

| # | Decisão | Data | Rationale |
|---|---------|------|-----------|
| D1 | `DelayBeforePublishingServiceOffer`: 60 → **30s** | 2026-06-23 | User chose 30s (Q1). Conservative but 2× improvement. |
| D2 | `DelayBeforeANewPhotoPublish`: 10 → **1s** | 2026-06-23 | User chose 1s (Q2). 10× improvement, safe. |
| D3 | `DelayBeforeDiscovery`: 10 → **3s** (code default) | 2026-06-23 | User chose 3s (Q3). Restores original design value. |
| D4 | **Level A only** (INI changes, no code changes) | 2026-06-23 | User chose Level A (Q4). Level B deferred. |
| D5 | Maintain `DelayBeforeRunPeriodic` overrides at 30/60s | 2026-06-23 | User chose to keep (Q5). Level B not applied, overrides preserved. |
| D6 | SPEC-005 SHM poll interval: **10ms** | 2026-06-23 | User chose 10ms (Q6). Applied as separate spec. |
| D7 | `DelayBeforeRunPeriodic`: keep INI at 10s | 2026-06-23 | Not questioned — INI value preserved. |
| D8 | `DelayBeforeANewPeerEvaluation`: keep at 5s | 2026-06-23 | Not questioned — INI value preserved. |
| D9 | `ContentBurstSize`: keep at 200 | 2026-06-23 | Not questioned — INI value preserved. |

---

## 11. Pitfalls Discovered During Design

1. **DiscoveryFirstStep/SecondStep overwrite, not stack**: Both functions call `Run->SetTime(GetTime() + DelayBeforeDiscovery)` on the same `ScheduledMessages.at(0)`. The 4 calls per periodic don't stack to 4×Delay — the last call wins. So the effective delay is `DelayBeforeDiscovery` (10s from INI), not 40s. This is better than initially feared, but still 10s per periodic cycle.

2. **DelayBeforeRunPeriodic is dynamically overridden**: `CoreRunEvaluate01.cpp` changes this value at runtime (L243: 30s, L361: 60s) after peer discovery. The INI value (10s) is only used until the first peer is found. This means the periodic loop SLOWS DOWN after discovery, which is counter-intuitive for performance optimisation. Level B proposes reducing these overrides.

3. **ContentBurstSize break condition**: The loop at `CoreRunContentPublish01.cpp` L231 breaks when `Counter == ContentBurstSize` (200). With 200 photos and ContentBurstSize=200, all photos are published in a single burst with Delta≈0. The 10s delay only applies between bursts (L284: reschedule). So the actual photo publish time for ≤200 photos is near-instant — the bottleneck is the 60s service offer delay and the 10s pre-contentpublish delay.

4. **GW.cpp 100ms SHM poll is NOT the primary bottleneck**: The 100ms poll adds ~600ms across the full cycle (6 message exchanges). Compared to the 60s + 10s + 10s = 80s from timers, this is 0.75%. However, it IS a regression from the pre-NG-005 behavior (1ms busy-wait) and should be addressed in SPEC-005 for correctness.

5. **INI parser ignores values ≤ 0**: `CoreRunInitialize01.cpp` L208: `if (Temp > 0)`. If an INI value is 0 or negative, the code default is kept. This is a safety feature but means you cannot set a timer to 0 (instant) via INI.

6. **Both INI files are identical**: `IO/Source1/App.ini` and `IO/Repository1/App.ini` have the same values. Any change must be applied to both.

---

## 12. Acceptance Criteria

- [x] INI files updated: ServiceOffer=30, PhotoPublish=1, Discovery=3 (E1)
- [ ] GW.cpp updated: SHM_POLL_INTERVAL_MS=10, CV timeout capped (E2)
- [ ] PGCS + NRNCS + ContentApp compile successfully after GW.cpp change (E2)
- [ ] NRNCS discovery completes within 15s of ContentApp start
- [ ] Service Offer sent within 50s of ContentApp start
- [ ] First photo published within 60s of ContentApp start
- [ ] 200 photos published within 70s of ContentApp start
- [ ] No "Failed to discover PGCS_PID" errors in logs
- [ ] No SIGSEGV or memory errors
- [ ] GW CPU < 20% per process (no busy-wait regression)
- [ ] SHM message latency < 20ms
- [ ] Commit pushed to `AIOPT2` branch (E4)
- [ ] Dashboard updated (NG-042-07 progress)
- [ ] Obsidian task file updated with results
