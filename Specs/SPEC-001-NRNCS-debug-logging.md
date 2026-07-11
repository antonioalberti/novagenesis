# SPEC-001: Comprehensive Debug Logging for NRNCS

**Status:** Draft  
**Author:** Antonio Marcos Alberti  
**Date:** 2026-07-11  
**Version:** 0.1  
**Branch:** AIOPT2  
**Component:** NRNCS (`NRNCS/src/`)

---

## 1. Objective

Add `#ifdef DEBUG` logging blocks throughout all NRNCS source files to enable runtime tracing of the Name Resolution and Network Cache Service. Every `Run()` method must log its entry (method name), key functional steps, and exit (`(Done)`) when compiled with `-DDEBUG` or when `#define DEBUG` is active.

## 2. Existing Convention

The codebase already follows a consistent debug logging pattern:

**Entry marker:**

```
#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName() << endl;

#endif
```

**Step marker:**

```
#ifdef DEBUG

  PB->S << Offset << "(1. Description of the step)" << endl;

#endif
```

**Exit marker:**

```
#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif
```

**Data dump:**

```
#ifdef DEBUG

  PB->S << Offset << "(Label)" << endl;
  PB->S << *MessageVariable << endl;

#endif
```

**Variable values:**

```
#ifdef DEBUG

  PB->S << Offset << "(file=" << Values.at(0) << ", size=" << Size << " bytes)" << endl;

#endif
```

**Important:** The master branch code uses tabs for indentation, spaces around function names (`function ()`), and `PB->GenerateSCNFromMessageBinaryPatterns` instead of `NameGenerator::GetInstance().GenerateFromMessage`. All debug additions must follow the same style.

## 3. Current State Summary (master branch baseline)

Every `#define DEBUG` in the master branch is commented out (`//#define DEBUG` or `////#define DEBUG`), meaning all existing `#ifdef DEBUG` blocks are dead code. The table below shows block counts assuming DEBUG is enabled.

| File | Lines | `#define DEBUG`? | `#ifdef` blocks | Entry | Steps | Done | Commented-out | Notes |
|------|-------|-------------------|-----------------|-------|-------|------|---------------|-------|
| `execNRNCS.cpp` | 95 | None | 0 | — | — | — | — | `main()`. No PB, use `cout` |
| `NRNCS.cpp` | 86 | None | 0 | — | — | — | — | Constructor + `NewBlock()` |
| `NR.cpp` | 223 | None | 0 | — | — | — | — | Constructor + `NewAction()` |
| `NRMessageSeq01.cpp` | 61 | None | 0 | Commented | — | Commented | 2 lines | Trivial Run() |
| `NRSCNSeq01.cpp` | 108 | None | 0 | Commented | — | Commented | 2 lines | SCN sequence |
| `NRSubBind01.cpp` | 107 | None | 0 | Commented | — | Commented | 2 lines | Subscription loop |
| `NRInfoPayload01.cpp` | 114 | None | 0 | Commented | Commented | Commented | 3 lines | Payload forwarding |
| `NRRunInitialization01.cpp` | 280 | `////` (off) | 0 | — | — | Commented | 1 line | Config loading (raw `PB->S <<`, not under `#ifdef`) |
| `NRRunPeriodic01.cpp` | 244 | `//` (off) | 4 | Yes | Yes | Yes | — | Entry/done in Run, step/data in Exposition. Missing debug in `GetPGCSNames()` |
| `NRMsgCl01.cpp` | 140 | `////` (off) | 3 | Yes | Yes | Yes | — | Good coverage, but DEBUG off |
| `NRDeliveryBind01.cpp` | 161 | `////` (off) | 2 | Yes | — | Yes | — | Missing step debug for args |
| `NRPubBind01.cpp` | 119 | `////` (off) | 2 | Yes | — | Yes | — | Missing step debug for args |
| `NRRevokeBind01.cpp` | 78 | `////` (off) | 2 | Yes | — | Yes | — | Trivial — OK as-is |
| `NRPubNotify01.cpp` | 229 | `//` (off) | 1 | Commented | Commented | Commented | 3 lines | Only has data dump block. Missing entry/done |

## 4. Proposed Changes

### 4.1 `execNRNCS.cpp` — Add debug to main()

**Current:** 95 lines, no debug of any kind. Uses `cout` (no `PB` available since this is `main()`).

Add after the last `#endif` (line 36) and before `int main()` (line 38):

```cpp
//#define DEBUG
```

After line 68 (`key_t Key = R;`), add:

```cpp
#ifdef DEBUG

  cout << "[DEBUG] Starting NRNCS with Key=" << R << ", Path=" << Path << endl;

#endif
```

After line 71 (`NRNCS execNRS (...)`), add:

```cpp
#ifdef DEBUG

  cout << "[DEBUG] NRNCS instance created" << endl;

#endif
```

### 4.2 `NRNCS.cpp` — Add debug to constructor and NewBlock()

**Current:** 86 lines, no debug.

Add after the last `#endif` (line 34) and before `NRNCS::NRNCS` (line 36):

```cpp
//#define DEBUG
```

In `NRNCS::NRNCS()` (line 36), after `RunGateway ();` (line 44), add:

```cpp
#ifdef DEBUG

  cout << "[DEBUG] NRNCS process created, GW running" << endl;

#endif
```

In `NRNCS::NewBlock()` (line 52), before `return OK;` (line 81), add:

```cpp
#ifdef DEBUG

  cout << "[DEBUG] NR block created and inserted" << endl;

#endif
```

**Note:** `NRNCS` extends `Process` directly (not `Block`), so it has neither `PB` nor `S`. We use `cout` for Process-level debug output.

### 4.3 `NR.cpp` — Debug not added (PB not available)

`NR` extends `Block` directly, but `PB` (pointer to Block) is only available in `Action`-derived classes, not in `Block`-derived constructors or methods. The `Run()` method in `NR::NR()` constructor and `NewAction()` were compiled before `PB` is set, so **no debug blocks were added to NR.cpp**. The file retains its original `//#define DEBUG` (commented) for future enhancement.

**Current:** 223 lines, no debug.

No debug blocks were added to `NR.cpp`. The `PB` pointer is not available in `Block`-derived constructors or non-`Run()` methods — `PB` is a member of `Action`, not `Block`. The `NR::NR()` constructor and `NR::NewAction()` methods cannot use `PB->S`. The file retains `#define DEBUG` (commented as `//#define DEBUG` initially, then changed to `#define DEBUG` per user request) but the existing `#ifdef DEBUG` blocks in this file count remains 0.

### 4.4 `NRMessageSeq01.cpp` — Activate and uncomment debug

**Current:** 61 lines. Entry/done debug lines are commented out. No `#define DEBUG`.

Change the commented-out `//#define DEBUG` or add a fresh one after the last `#endif` (line 34) and before the constructor (line 36):

```cpp
//#define DEBUG
```

Replace line 53 (`//PB->S << Offset <<  this->GetLegibleName() << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif
```

Replace line 58 (`//PB->S << Offset <<  "(Done)" << endl << endl << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif
```

### 4.5 `NRSCNSeq01.cpp` — Activate and uncomment debug

**Current:** 108 lines. Entry/done debug lines are commented out. No `#define DEBUG`.

Add after the last `#endif` (line 34) and before the constructor (line 36):

```cpp
//#define DEBUG
```

Replace line 57 (`//PB->S << Offset <<  this->GetLegibleName() << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif
```

Add after the received SCN is obtained (after line 69, before the `if (ReceivedSCN != "")` check):

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Received SCN=" << ReceivedSCN
        << ", State=" << PB->State << ")" << endl;

#endif
```

Replace line 105 (`//PB->S << Offset <<  "(Done)" << endl << endl << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif
```

### 4.6 `NRSubBind01.cpp` — Activate and add key debug

**Current:** 107 lines. Entry/done commented out. No `#define DEBUG`.

Add after the last `#endif` (line 42) and before the constructor (line 44):

```cpp
//#define DEBUG
```

Replace line 64 (`//PB->S << Offset <<  this->GetLegibleName() << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif
```

After the `Category.size() > 0 && Key.size() > 0` check (line 75), before the `for` loop (line 77), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Processing " << Key.size ()
        << " subscription keys)" << endl;

#endif
```

Inside the loop (line 77), after `NewGetCommandLine` (line 80), add:

```cpp
#ifdef DEBUG

      PB->S << Offset << "(Key[" << i << "]=" << Key.at (i) << ")" << endl;

#endif
```

Replace line 104 (`//PB->S << Offset <<  "(Done)" << endl << endl << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif
```

### 4.7 `NRInfoPayload01.cpp` — Activate and add payload debug

**Current:** 114 lines. Entry/step/done commented out. No `#define DEBUG`.

Add after the last `#endif` (line 34) and before the constructor (line 36):

```cpp
//#define DEBUG
```

Replace line 57 (`//PB->S << Offset <<  this->GetLegibleName() << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif
```

After the payload is successfully copied (after line 94, before the closing brace at line 95), add:

```cpp
#ifdef DEBUG

    PB->S << Offset << "(Forwarding payload: file=" << Values.at (0)
          << ", size=" << Size << " bytes)" << endl;

#endif
```

Replace line 111 (`//PB->S << Offset <<  "(Done)" << endl << endl << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif
```

### 4.8 `NRRunInitialization01.cpp` — Enable and expand debug

**Current:** 280 lines. `////#define DEBUG` (commented out with 4 slashes). No `#ifdef DEBUG` blocks. Entry/done commented out.

Change `////#define DEBUG` (line 40) to `//#define DEBUG`.

At the start of `Run()` body, after line 72 (`PHTB = (Block *)PNR->PHT;`), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif
```

Before "Setting up the process SCN" (before line 74), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(1. Creating initial bindings message)" << endl;

#endif
```

After the initial bindings message is pushed to GW (after line 145), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(2. Loading NRNCS.ini parameters)" << endl;

#endif
```

After the config loading completes (after line 232), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(3. Scheduling periodic run)" << endl;

#endif
```

Replace the commented-out line 269 (`//PB->S << Offset <<  "(Done)" << endl << endl << endl;`) with:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif
```

### 4.9 `NRRunPeriodic01.cpp` — Add missing debug in helper methods

**Current:** 244 lines. `//#define DEBUG` (off). Has 4 `#ifdef DEBUG` blocks (entry + done in `Run()`, step + data dump in `Exposition()`). Missing debug in `GetPGCSNames()`.

Change `//#define DEBUG` (line 44) to `#define DEBUG`.

In `GetPGCSNames()` (line 141), after the `PGCSHT = PGCSBIDs->at (0);` assignment (line 150), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(GetPGCSNames: PID=" << PGCSPID
        << ", HT BID=" << PGCSHT << ")" << endl;

#endif
```

In `Exposition()` (line 157), at the top of the function body (after line 171), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Exposition: scheduling names to PGCS)" << endl;

#endif
```

### 4.10 `NRMsgCl01.cpp` — Enable existing debug

**Current:** 140 lines. `////#define DEBUG` (off). Already has 3 `#ifdef DEBUG` blocks (entry + step + done). The blocks are correct and don't need modification.

Change `////#define DEBUG` (line 40) to `//#define DEBUG`.

### 4.11 `NRDeliveryBind01.cpp` — Enable and add step debug

**Current:** 161 lines. `////#define DEBUG` (off). Has entry + done blocks. Missing step debug for parsed args.

Change `////#define DEBUG` (line 44) to `//#define DEBUG`.

In `Run()`, inside the innermost success block (after line 94, before line 96), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Delivery: Category=" << Category.at (0)
        << ", Key=" << Key.at (0) << ", " << Values.size ()
        << " values)" << endl;

#endif
```

### 4.12 `NRPubBind01.cpp` — Enable and add step debug

**Current:** 119 lines. `////#define DEBUG` (off). Has entry + done blocks. Missing step debug for parsed args.

Change `////#define DEBUG` (line 44) to `//#define DEBUG`.

In `Run()`, inside the innermost success block (after line 83, before line 85), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Publish: Category=" << Category.at (0)
        << ", Key=" << Key.at (0) << ", " << Values.size ()
        << " values)" << endl;

#endif
```

### 4.13 `NRRevokeBind01.cpp` — Enable existing debug

**Current:** 78 lines. `////#define DEBUG` (off). Has entry + done blocks. Already adequate.

Change `////#define DEBUG` (line 44) to `//#define DEBUG`.

### 4.14 `NRPubNotify01.cpp` — Enable and add entry/exit markers

**Current:** 229 lines. `//#define DEBUG` (off). Has one `#ifdef DEBUG` data dump block (lines 183-187). Entry and exit are commented out.

Change `//#define DEBUG` (line 45) to `#define DEBUG`.

After line 81 (`PNR = (NR *)PB;`), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif
```

Before `return Status;` (line 228), add:

```cpp
#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif
```

## 5. Implementation Notes

### 5.1 `#define DEBUG` strategy

For files that already have `#ifdef DEBUG` blocks but DEBUG is commented out (NRMsgCl01, NRDeliveryBind01, NRPubBind01, NRRevokeBind01, NRPubNotify01, NRRunPeriodic01), change the comment from `////#define DEBUG` or `//#define DEBUG` to `#define DEBUG`.

For files that have no debug infrastructure at all (execNRNCS, NRNCS, NR, NRMessageSeq01, NRSCNSeq01, NRSubBind01, NRInfoPayload01), add `//#define DEBUG` after the includes so the developer must uncomment it to activate debug.

For NRRunInitialization01, change `////#define DEBUG` to `//#define DEBUG` (single comment, not quad).

### 5.2 `PB` is only available in `Action`, not in `Block` or `Process`

`PB` (pointer to the parent Block) is a protected member of the `Action` class, set during `Action` construction. Classes that inherit directly from `Block` (e.g. `NR`) or `Process` (e.g. `NRNCS`) do NOT have `PB`. For `Process`-derived classes, use `cout` for debug output. For `Block`-derived classes, the `S` stream (`ConsoleOstream`) is available as a member — use `S <<` directly. In this codebase:

- `NRNCS` extends `Process` → use `cout`
- `NR` extends `Block` → has `S` but not `PB`
- All `*Bind01`, `*Seq01`, `*Payload01` classes extend `Action` → have `PB->S`

### 5.3 SCN generation fix

During compilation it was discovered that `Block::GenerateSCNFromMessageBinaryPatterns()` does not exist in the current Common library. All occurrences were replaced with:

```cpp
SCN = NameGenerator::GetInstance().GenerateFromMessage(MessagePtr);
```

And `#include "NameGenerator.h"` was added to the files that use it:
- `NRDeliveryBind01.cpp`
- `NRSCNSeq01.cpp`
- `NRRunInitialization01.cpp`
- `NRRunPeriodic01.cpp`
- `NRPubNotify01.cpp`

### 5.4 `ResetStatistics()` removed

`NRRunInitialization01.cpp` had a call to `PNR->PGW->ResetStatistics()` which does not exist in the current `GW` class. This call was removed.

### 5.5 execNRNCS.cpp special case

`execNRNCS.cpp` is `main()`. There is no `PB` pointer. Use `cout` inside `#ifdef DEBUG` blocks with `[DEBUG]` prefix for consistency:

```cpp
#ifdef DEBUG
  cout << "[DEBUG] Starting NRNCS with Key=" << R << ", Path=" << Path << endl;
#endif
```

### 5.6 Coding style

All additions must follow the master branch style:
- Tabs for indentation (not spaces)
- Space before `()` in function calls: `function ()`
- Space after type in pointer: `Type *variable`
- Space after `<<` in streams: `PB->S << Offset <<`

### 5.7 The `Offset` variable

All `Run()` methods already declare `string Offset = "                    ";` (24 spaces). New debug blocks must use this same `Offset`.

### 5.8 Compilation guard

All debug logging must be wrapped in:

```cpp
#ifdef DEBUG
  ...
#endif
```

This ensures zero runtime overhead when `DEBUG` is not defined.

## 6. Files Changed

| # | File | Change |
|---|------|--------|
| 1 | `NRNCS/src/execNRNCS.cpp` | `#define DEBUG` + 2 `cout`-based debug blocks |
| 2 | `NRNCS/src/NRNCS.cpp` | `#define DEBUG` + 2 `cout`-based debug blocks (Process, no PB) |
| 3 | `NRNCS/src/NR.cpp` | `#define DEBUG` only — no debug blocks added (PB not available in Block methods) |
| 4 | `NRNCS/src/NRMessageSeq01.cpp` | `#define DEBUG` + activate 2 debug blocks |
| 5 | `NRNCS/src/NRSCNSeq01.cpp` | `#define DEBUG` + 3 debug blocks |
| 6 | `NRNCS/src/NRSubBind01.cpp` | `#define DEBUG` + 4 debug blocks |
| 7 | `NRNCS/src/NRInfoPayload01.cpp` | `#define DEBUG` + 3 debug blocks |
| 8 | `NRNCS/src/NRRunInitialization01.cpp` | `#define DEBUG` + 5 debug blocks + fix SCN generation |
| 9 | `NRNCS/src/NRRunPeriodic01.cpp` | `#define DEBUG` + 2 debug blocks in helpers + fix SCN generation |
| 10 | `NRNCS/src/NRMsgCl01.cpp` | `#define DEBUG` — existing 3 blocks enabled |
| 11 | `NRNCS/src/NRDeliveryBind01.cpp` | `#define DEBUG` + 1 debug block + fix SCN generation + add `#include "NameGenerator.h"` |
| 12 | `NRNCS/src/NRPubBind01.cpp` | `#define DEBUG` + 1 debug block |
| 13 | `NRNCS/src/NRRevokeBind01.cpp` | `#define DEBUG` — existing 2 blocks enabled |
| 14 | `NRNCS/src/NRPubNotify01.cpp` | `#define DEBUG` + 2 debug blocks + fix SCN generation + add `#include "NameGenerator.h"` |
| 15 | `NRNCS/src/NRSCNSeq01.cpp` | Fix SCN generation + add `#include "NameGenerator.h"` |
| 16 | `NRNCS/src/NRRunInitialization01.cpp` | Fix SCN generation + add `#include "NameGenerator.h"` |

## 7. Verification

1. Build with `make -j$(nproc)` — must compile without errors
2. Run `./NRNCS /path/to/IO/` — must observe debug output showing entry/step/exit for each action processed
3. Comment out all `#define DEBUG` lines — must produce no debug output (zero runtime overhead)

## 8. Key Style Differences from Previous AIOPT2 Code

The master branch baseline uses different patterns than the AIOPT2 branch code that was previously analysed:

| Aspect | Master branch | Previous AIOPT2 code |
|--------|--------------|---------------------|
| Indentation | Tabs | Spaces |
| Function calls | `func ()` | `func()` |
| Pointer declarations | `Type *var` | `Type* var` |
| SCN generation | `PB->GenerateSCNFromMessageBinaryPatterns(...)` | `NameGenerator::GetInstance().GenerateFromMessage(...)` |
| Commented-out defines | `////#define DEBUG` or `//#define DEBUG` | `//#define DEBUG` |
| Commented-out debug | `//PB->S <<` | `// PB->S <<` (space after //) |
| `#include <NameGenerator.h>` | Not present in most files | Present |
| NRSubBind01 loop | Accumulates in InlineResponseMessage | One message per key (SPEC-021 fix) |
| NRInfoPayload01 | Simple copy to InlineResponseMessage | Separate message per payload (SPEC-021 fix) |
| NRRunInitialization01 | Uses `PB->GenerateSCNFromMessageBinaryPatterns` | Uses `NameGenerator::GenerateFromMessage` |