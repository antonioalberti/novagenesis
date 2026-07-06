# clang-format Report — NovaGenesis Repository

**Date:** 2026-07-06  
**Tool:** clang-format v18.1.3 (Ubuntu)  
**Style file:** `.clang-format` (pre-existing, 315 bytes)  

---

## 1. Configuration Applied

The existing `.clang-format` was used as-is:

```yaml
BasedOnStyle: LLVM
IndentWidth: 2
UseTab: Never
BreakBeforeBraces: Allman
AllowShortFunctionsOnASingleLine: None
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
SpaceBeforeParens: ControlStatements
PointerAlignment: Left
ColumnLimit: 0
SortIncludes: false
IndentPPDirectives: None
```

Key characteristics: Allman brace style (braces on new line), 2-space indent, no tab characters, pointers aligned left, no column limit, no include sorting.

---

## 2. Formatting Statistics

| Metric | Value |
|--------|-------|
| **Total C/C++ source files in repo** | 434 |
| **Files formatted** | **390** |
| **Files already clean (no changes)** | 2 (Action.cpp, Block.cpp) |
| **Third-party files skipped** | 9 |
| **Lines inserted** | 34,701 |
| **Lines deleted** | 35,850 |
| **Net lines removed** | 1,149 |
| **clang-format errors/warnings** | **0** |

### Files skipped (third-party):

| File | Reason |
|------|--------|
| `Common/src/fast_mutex.h` | Marcus Geelnard's tinythread library |
| `Common/src/MurmurHash3.cpp` | Austin Appleby's public domain MurmurHash3 |
| `Common/src/MurmurHash3.h` | Austin Appleby's public domain MurmurHash3 |
| `Common/src/tinythread.cpp` | Marcus Geelnard's tinythread library |
| `Common/src/tinythread.h` | Marcus Geelnard's tinythread library |
| `EPGS/Common/MurmurHash3.c` | Austin Appleby's MurmurHash3 (port) |
| `EPGS/Common/MurmurHash3.h` | Austin Appleby's MurmurHash3 (port) |
| `EPGS/Common/tinythread.cpp` | Marcus Geelnard's tinythread library |
| `EPGS/Common/tinythread.h` | Marcus Geelnard's tinythread library |

### Other exclusions:
- `.git/` — version control metadata
- `cmake-build-debug/` — build artifacts (root-owned)
- `Docker/` — Dockerfiles and container resources
- `Logs/` — runtime logs
- `Plots/` — plot outputs
- `Make/` — makefiles
- `Docs/` — documentation
- `Temp/` — temporary/scratch files

---

## 3. Changes by Component

| Component | Files |
|-----------|-------|
| **Common/src** | 71 source files (GW, HT, Message, CLI, Process, etc.) |
| **PGCS/src** | 67 source files (Core, PG, Hello IPC, etc.) |
| **ContentApp/src** | 41 source files |
| **IoTTestApp/src** | 39 source files |
| **GIRS/src** | 39 source files |
| **PSS/src** | 31 source files |
| **NRNCS/src** | 27 source files |
| **NBTestApp/src** | 25 source files |
| **HTS/src** | 13 source files |
| **EPGS** | 28 source files (C-based EPGS subsystem) |
| **IO/src** | (excluded — under Docs/IO/) |

---

## 4. Build Status

| Target | Status | Details |
|--------|--------|---------|
| **Common** | ✅ | Static library `libCommon.a` linked cleanly |
| **ContentApp** | ✅ | Executable built |
| **HTS** | ✅ | Executable built |
| **GIRS** | ✅ | Executable built |
| **NBTestApp** | ✅ | Executable built |
| **IoTTestApp** | ✅ | Executable built |
| **NRNCS** | ✅ | Executable built |
| **PSS** | ✅ | Executable built |
| **PGCS** | ✅ | Executable built |

**Result: 0 compilation errors.** All 9 targets built successfully.

### Warnings (all pre-existing — none introduced by formatting):

The following warning categories appeared during build. These are **existing code quality issues** present before formatting:

- `-Wunused-variable` — unused local variables (widespread across components)
- `-Wunused-but-set-variable` — variables assigned but never read
- `-Wsign-compare` — signed/unsigned integer comparison in for-loops
- `-Wmismatched-new-delete` — `delete` used on `new[]` allocations (Message.cpp, MessageBuilder.cpp)
- `-Woverloaded-virtual` — hidden virtual functions (IoTTestApp)
- `"missing terminating character"` — broken `#ifndef` guard in ContentApp.cpp and CoreStatusS01.cpp

None of these warnings are related to formatting changes.

---

## 5. Suggested Improvements to the `.clang-format` Configuration

### 5.1. Consider enabling `SortIncludes`

```yaml
SortIncludes: true
```

Currently disabled. Enabling it would standardise include order across all files (LLVM convention: main header → project headers → standard library → system headers → conditional includes). This makes dependency analysis easier and prevents subtle ODR violations.

**Impact:** Would touch every file's include section. Recommend enabling in a separate pass after verifying no subtle header-order dependencies exist.

### 5.2. Consider adding a column limit

```yaml
ColumnLimit: 120  # or 100
```

Currently `ColumnLimit: 0` (disabled). A limit of 120 columns (common for C++ projects) would:
- Prevent excessively long lines
- Improve code review diffs
- Make side-by-side diffs readable
- Align with modern C++ style guides (Google: 80, LLVM: 120, Chromium: 120)

**However:** The current Allman brace style already helps with line width. Many files use long descriptive variable names (e.g. `GenerateSCNFromCharArrayBinaryPatterns16Bytes`), so a limit below 120 would cause many line breaks.

### 5.3. Standardise pointer/reference alignment

```yaml
PointerAlignment: Left
```

Current setting is `Left` (`Type* name`). This is fine and consistent with LLVM convention. No change needed, but be aware that some files may have been written with right-aligned pointers (`Type *name`) and will be reformatted.

### 5.4. Consider `AlignConsecutiveAssignments` / `AlignConsecutiveDeclarations`

```yaml
AlignConsecutiveAssignments: true
AlignConsecutiveDeclarations: true
```

The codebase uses aligned declarations in many places (block IDs, SCN tables, binding values). Enabling these would preserve and enhance that style.

### 5.5. Consider `AllowShortFunctionsOnASingleLine: InlineOnly`

Current setting `None` forces all function bodies to break. `InlineOnly` would allow single-line functions defined inside class definitions (e.g. `int getX() { return x; }`) to stay on one line while still breaking standalone function definitions.

---

## 6. Summary

- **390 files** formatted consistently across all NovaGenesis components
- **9 third-party files** preserved unchanged
- **2 files** were already compliant
- **0 clang-format errors** during formatting
- **0 build errors** after formatting
- **0 formatting-related warnings**
- All 9 build targets compile successfully
- Pre-existing warnings (~50+) are all code-quality issues unrelated to formatting

The codebase is now uniformly formatted with Allman braces, 2-space indentation, left-aligned pointers, and no tab characters. Future commits will be automatically enforced when contributors run `clang-format` before committing.