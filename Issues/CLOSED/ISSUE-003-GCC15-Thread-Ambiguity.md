# ISSUE-003: GCC 15 thread Ambiguity in Process.cpp

**Date:** 2026-06-25  
**Status:** Closed  
**Priority:** High  
**Related:** SPEC-MUSL-001, Alpine Linux, GCC 15

---

## 1. Description

Compiling NovaGenesis on Alpine Linux with GCC 15.2.0 failed with:
```
error: reference to 'thread' is ambiguous
 1031 |    thread t1(&CLI::PromptThreadWrapper,PCLI);
```

**Cause:** Line `thread t1(...)` was ambiguous because:
1. `tthread::thread` — from `tinythread.h` (included via `Message.h` → `Process.h`)
2. `std::thread` — included transitively via `GW.h:121` → `<condition_variable>` → `<stop_token>` → `<thread>` (GCC 15+ behavior)

GCC ≤14 did not include `<thread>` transitively from `<condition_variable>`, so code compiled without issues.

## 2. Fix Applied

Changed `thread t1(...)` to `tthread::thread t1(...)` — explicit namespace qualifier.

**File:** `Common/src/Process.cpp:1031`  
**Commit:** cd804a7

## 3. Platform Impact

| Platform | Impact |
|----------|--------|
| Alpine Linux (GCC 15) | ❌ Failed — fix required |
| Ubuntu 24.04 (GCC 13) | ✅ Compiles with or without fix |
| Docker Ubuntu (GCC 13) | ✅ Compiles with or without fix |

## 4. Safety Analysis

- `tthread::thread` is the same type `thread` resolved to before the ambiguity
- `CLI::PromptThreadWrapper` accepts `void*` — `PCLI` (CLI*) converts correctly
- `t1.join()` needs no qualifier — `t1` is already typed as local variable
- No other files use unqualified `thread` (verified via grep)
- Function `RunPrompt()` only compiled with `#ifdef DEBUG` — doesn't affect production builds

## 5. Resolution

**Closed** — Fix applied in AIOPT3 branch. Compiles successfully on all target platforms.