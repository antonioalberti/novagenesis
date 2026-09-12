# SPEC-012 — Hash Abstraction & ID Generation Strategy

**Version:** v0.3  
**Date:** 2026-07-06  
**Author:** (derived from analysis)  
**Status:** Proposal — awaiting decisions (Q1-Q5 resolved, pending implementation)  
**Branch:** AIOPT3

---

## E0 — Problem Statement

NovaGenesis currently generates **Self-Certifying Names (SCNs)** / **Block IDs (BIDs)** / **Process IDs (PIDs)** using MurmurHash3_x86_32 (32-bit, i.e. 4 bytes → 8 hex characters) in dozens of locations across the codebase. The implementation has three structural problems:

1. **Duplicated logic across two class hierarchies** — Both `Process` and `Block` contain nearly identical `GenerateSCNFrom*` methods (4Bytes, 16Bytes, 32Bytes variants) plus default wrappers, totalling ~25 methods doing essentially the same thing.
2. **Hash implementation is hard-coded** — `MurmurHash3_x86_32()` is called directly in ~30 places inside `Block.cpp` and `Process.cpp`. Switching hash function (e.g. to SHA-3, BLAKE3, or a cryptographic KDF) requires editing each call site.
3. **Size is hard-coded as 32-bit** — The 16Byte and 32Byte variant methods exist but are unused by any default wrapper. Comparing different hash sizes for the Alpine VM test scenario requires manual edits.

---

## E1 — Current Architecture Map

### E1.1 — Source files involved

| File | Role | Hash lines |
|------|------|-----------|
| `Common/src/MurmurHash3.h/.cpp` | Third-party: Austin Appleby's MurmurHash3 (public domain) | 1 function used: `MurmurHash3_x86_32` |
| `Common/src/Block.h/.cpp` | Hash generation for blocks: BID, SCN from messages/char arrays | ~32 call sites |
| `Common/src/Process.h/.cpp` | Hash generation for processes: PID, SCN from process/char arrays | ~49 call sites |
| `Common/src/MessageBuilder.cpp` | Creates command lines with hashed values (bindings, keys) | ~40 call sites (delegates to Block) |
| `EPGS/Common/MurmurHash3.c/.h` | Third-party port (C version) | — |
| `EPGS/Common/ng_epgs_hash.c/.h` | EPGS-specific C wrapper | 1 function |

### E1.2 — Hash function call chain (typical)

```
User code (e.g. GWRunInitialization01.cpp)
  → Block::GenerateSCNFromCharArrayBinaryPatterns(string, string&)     [Block.cpp:1414]
    → Block::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string, string&) [Block.cpp:1045]
      → MurmurHash3_x86_32(key, size, seed, Bytes)                      [Block.cpp:1091]
```

### E1.3 — Methods duplicated across Process and Block

Both classes implement these variants (identical structure, different "subject"):

| Method family | Process | Block |
|--------------|---------|-------|
| `FromSubjectBinaryPatterns4Bytes` | ✅ | ✅ |
| `FromSubjectBinaryPatterns16Bytes` | ✅ | ✅ |
| `FromSubjectBinaryPatterns32Bytes` | ✅ | ✅ |
| `FromCharArrayBinaryPatterns4Bytes(char*)` | ✅ | ✅ |
| `FromCharArrayBinaryPatterns16Bytes(char*)` | ✅ | ✅ |
| `FromCharArrayBinaryPatterns4Bytes(string)` | ✅ | ✅ |
| `FromCharArrayBinaryPatterns16Bytes(string)` | ✅ | ✅ |
| `FromCharArrayBinaryPatterns(string, string&)` (default → 4B) | ✅ | ✅ |
| `FromCharArrayBinaryPatterns(char*, size, string&)` (default → 4B) | ✅ | ✅ |
| `FromMessageBinaryPatterns*` | ❌ | ✅ |
| `FromProcessBinaryPatterns*` | ✅ | ❌ |
| `FromBlockBinaryPatterns*` | ❌ | ✅ |
| **Total methods** | **9** | **13** |

### E1.4 — Default wrappers (hard-coded to 4Bytes/32-bit)

```
Block.cpp:1398  GenerateSCNFromBlockBinaryPatterns   → 4Bytes
Block.cpp:1406  GenerateSCNFromCharArrayBinaryPatterns(char*) → 4Bytes
Block.cpp:1414  GenerateSCNFromCharArrayBinaryPatterns(string) → 4Bytes
Block.cpp:1422  GenerateSCNFromMessageBinaryPatterns → 4Bytes
Process.cpp:1035 GenerateSCNFromProcessBinaryPatterns → 4Bytes
```

---

## E2 — Proposed Architecture

### E2.1 — New class: `NameGenerator` (renamed from IDGenerator per decision Q1)

Create a single `NameGenerator` class/object responsible for ALL name (SCN/BID/PID) generation in the system. It is a **global singleton** shared by all Process-derived instances within the same binary.

```
┌──────────────────────────────────────────┐
│              NameGenerator                │
├──────────────────────────────────────────┤
│ - strategy: NameStrategy*                │
│ - hash_fn: HashFunction*                 │
│ - seed: uint32_t (= 0, fixed per Q2)     │
│ - output_size: size_t                    │
├──────────────────────────────────────────┤
│ + Generate(args...): string              │
│ + GenerateFromProcess(Process*): string  │
│ + GenerateFromBlock(Block*): string      │
│ + GenerateFromMessage(Message*): string  │
│ + GenerateFromString(string): string     │
│ + SetStrategy(NameStrategy*)             │
│ + GetOutputSize(): size_t                │
│ + static GetInstance(): NameGenerator&   │
└──────────────────────────────────────────┘```
```

### E2.2 — Strategy pattern for output size

```cpp
enum class IDOutputSize : uint8_t {
    BITS_32  = 4,   // 4 bytes  → 8 hex chars (current default)
    BITS_64  = 8,   // 8 bytes  → 16 hex chars
    BITS_128 = 16,  // 16 bytes → 32 hex chars (currently declared but unused)
    BITS_256 = 32,  // 32 bytes → 64 hex chars (currently declared but unused)
    BITS_512 = 64   // Future: 512-bit IDs
};
```

### E2.3 — Strategy pattern for hash function

```cpp
class HashFunction {
public:
    virtual ~HashFunction() = default;
    virtual string Compute(const void* input, size_t size, uint32_t seed) = 0;
    virtual string Name() const = 0;
    virtual IDOutputSize DefaultOutputSize() const = 0;
};

class MurmurHash3_x86_32 : public HashFunction { /* existing, 32-bit */ };
class MurmurHash3_x86_128 : public HashFunction { /* 128-bit variant */ };
class SHA256Hash : public HashFunction { /* future */ };
class Blake3Hash : public HashFunction { /* future */ };
class IdentityHash : public HashFunction { /* no hash - returns raw input as hex */ };
```

### E2.4 — Strategy pattern for ID content (not just hash input)

```cpp
class IDStrategy {
public:
    virtual ~IDStrategy() = default;
    virtual vector<const void*> GatherInputs(Process* _PP) = 0;
    virtual vector<const void*> GatherInputs(Block* _PB) = 0;
    virtual string StrategyName() const = 0;
};
```

### E2.5 — Ownership (global singleton)

```cpp
// NameGenerator.h
class NameGenerator {
public:
    static NameGenerator& GetInstance();   // global singleton
    ...
};

// In Process.h - all processes share the same global NameGenerator
// No per-process member needed - call NameGenerator::GetInstance() directly
```

> **Decision Q1:** Global singleton. Per-process ownership rejected because it could create incompatibilities between processes in the same address space. The global approach is sufficient since different VMs (e.g. Alpine 101/102) already run separate binaries, each with its own singleton. Multiple strategies/sizes coexist via per-call parameters to the same global instance.

---

## E3 — Current call sites to migrate

### E3.1 — Block.cpp (32 call sites)

| Location | Current call | New call |
|----------|-------------|----------|
| L72 | `GenerateSCNFromBlockBinaryPatterns(this, SCN)` | `NameGenerator::GetInstance().GenerateIDFromBlock(this)` |
| L447 | `GenerateSCNFromBlockBinaryPatterns4Bytes(...)` → `MurmurHash3_x86_32(...)` | `NameGenerator::GetInstance().GenerateID(...)` |
| L539 | `GenerateSCNFromBlockBinaryPatterns16Bytes(...)` | `NameGenerator::GetInstance().SetOutputSize(BITS_128)->GenerateID(...)` |
| L632 | `GenerateSCNFromBlockBinaryPatterns32Bytes(...)` | `NameGenerator::GetInstance().SetOutputSize(BITS_256)->GenerateID(...)` |
| L726..L1354 | All `GenerateSCNFromCharArrayBinaryPatterns*` variants | `NameGenerator::GetInstance().GenerateIDFromString(...)` |
| L1360..L1424 | `GenerateSCNFromMessageBinaryPatterns*` default wrappers | Remove — logic moves to NameGenerator |

### E3.2 — Process.cpp (49 call sites)

| Location | Current call | New call |
|----------|-------------|----------|
| L105 | `GenerateSCNFromProcessBinaryPatterns(this, SCN)` | `NameGenerator::GetInstance().GenerateIDFromProcess(this)` |
| L108 | `GenerateSCNFromCharArrayBinaryPatterns(LN, HASH_SCN)` | `NameGenerator::GetInstance().GenerateIDFromString(LN)` |
| L148..209 | All domain/OS/process level SCNs | `PIDGenerator->GenerateIDFromString(...)` |
| L1035..1414 | All variant bodies | Move to IDGenerator |

### E3.3 — MessageBuilder.cpp (~40 call sites)

All call `PB->GenerateSCNFromCharArrayBinaryPatterns(...)` which delegates to Block.
Migration path: MessageBuilder receives an NameGenerator reference, or uses the Block's generator.

### E3.4 — EPGS subsystem

`ng_epgs_hash.c` → either share the C++ NameGenerator (if EPGS gets C++ linkage) or keep as a standalone C shim that calls into the NameGenerator via a C API.

---

## E4 — Migration steps (phased, per Q5 decision)

**Decision Q5:** Phased migration. Each step is a separate compilable commit, done sequentially E4.1→E4.8. No sweeping commit.

### E4.1 — Create `NameGenerator` class (dedicated files)

```
Common/src/NameGenerator.h
Common/src/NameGenerator.cpp
Common/src/HashFunction.h          // abstract base
Common/src/MurmurHash3Strategy.h   // wraps existing MurmurHash3 into HashFunction interface
Common/src/NameStrategy.h          // abstract base
Common/src/DefaultNameStrategy.h   // preserves current behaviour
```

### E4.2 — Implement `MurmurHash3Strategy`

Wraps the existing `MurmurHash3_x86_32` call. Same seed, same algorithm.

### E4.3 — Implement `DefaultNameStrategy`

Preserves the current input-gathering logic exactly:
- For Process: LN, Path, DLN, SCN (same binary pattern as `GenerateSCNFromProcessBinaryPatterns4Bytes`)
- For Block: LN, block type info, parent process info
- For string: the raw string bytes

### E4.4 — Add NameGenerator to Process

Not needed as a member — global singleton is called directly.

### E4.5 — Migrate Process.cpp

Replace all `GenerateSCNFromCharArrayBinaryPatterns(LN, ...)` with `NameGenerator::GetInstance().GenerateFromString(LN)`. This is the bulk of changes (~40 lines).

### E4.6 — Migrate Block.cpp

Remove duplicated hash methods from Block. Block calls `NameGenerator::GetInstance()` directly (no parent Process pointer needed).

## E4.7 — Migrate MessageBuilder.cpp

MessageBuilder calls `NameGenerator::GetInstance()` directly.

### E4.8 — Remove old methods

After migration, remove from `Process.h/.cpp` and `Block.h/.cpp`:
- All `GenerateSCNFrom*BinaryPatterns(4|16|32)Bytes` variants
- All `GenerateSCNFromCharArrayBinaryPatterns*` variants
- All `GenerateSCNFromMessageBinaryPatterns*` variants

### E4.9 — EPGS integration

Deferred. EPGS keeps current `ng_epgs_hash.c` as-is. A C wrapper can be added later if interoperability between EPGS and NameGenerator becomes necessary.

> **Decision Q3:** EPGS unchanged for now. Not worth reworking a stable subsystem. C wrapper if needed later.

---

## E5 — Alpine VM test plan (comparing hash sizes)

### E5.1 — Objective

Run the same 1core-1repo-1source scenario on Alpine VMs 101/102 with different hash sizes and measure:
- Collision rate (if any)
- Throughput (messages/second)
- Memory usage per binding entry
- Startup time

### E5.2 — Configurations to test

| Config | Output size | Hash function | Notes |
|--------|------------|---------------|-------|
| A (baseline) | 32-bit (4B) | MurmurHash3_x86_32 | Current behaviour |
| B | 64-bit (8B) | MurmurHash3_x86_128 (truncated) | — |
| C | 128-bit (16B) | MurmurHash3_x86_128 | Uses existing 16Bytes variant |
| D | 256-bit (32B) | MurmurHash3 + SHA-256 | Future comparison |
| E | 32-bit (4B) | Identity (no hash, raw LN→hex) | Tests non-hash ID strategy |

### E5.3 — How to switch

```bash
# In config file or environment:
export NG_ID_STRATEGY=default
export NG_HASH_FUNCTION=murmur3_x86_32
export NG_ID_SIZE=4   # 4, 8, 16, 32 bytes
```

Or at compile time via CMake:
```cmake
option(NG_ID_SIZE "ID output size in bytes" 4)
option(NG_HASH_FN "Hash function backend" "murmur3_x86_32")
```

---

## E6 — Future strategies (v2 — tagged names)

**Decision Q4:** Tagged ID format deferred to v2. The v1 focus is extracting hash logic into `NameGenerator` without changing output format.

| Strategy | Hash function | Output | Use case | Target |
|----------|--------------|--------|----------|--------|
| `identity` | None | Raw LN as hex | Debugging, deterministic IDs | v2 |
| `murmur3_32` | MurmurHash3 x86_32 | 4 bytes (no tag) | Current baseline | v1 baseline |
| `murmur3_128` | MurmurHash3 x86_128 | 16 bytes | Lower collision risk | v1+v2 |
| `sha256_trunc` | SHA-256 | 8/16/32 bytes | Cryptographically verifiable IDs | v2 |
| `blake3` | BLAKE3 | configurable | High-performance + crypto | v2 |
| `ed25519_pubkey` | Ed25519 public key | 32 bytes | Self-certifying identity | v2 |

### Tagged ID format (v2 proposal)

Each Self-Certifying Name includes a tag byte describing its generation method, making names self-descriptive and forward-compatible:

```
Byte 0: ID type tag
  - 0x00 = 32-bit MurmurHash3 (current, backward compat)
  - 0x01 = 64-bit MurmurHash3
  - 0x02 = 128-bit MurmurHash3
  - 0x03 = 32-bit Identity (no hash)
  - 0x80+ = future cryptographic IDs
Bytes 1..N: raw ID bytes (size determined by tag)
```

> *Rationale:* A receiver can interpret any name without prior negotiation, enabling mixed deployments (e.g. 32-bit and 128-bit names in the same domain). Overhead is 1 byte per name, negligible for 8-64 hex char names.

---

## E7 — Backward compatibility

**Critical constraint:** Changing hash output size changes ALL existing SCNs, BIDs, PIDs, and binding keys. The system will NOT interoperate with existing deployments unless we keep 32-bit as the default.

**v1 approach:** Keep 32-bit MurmurHash3 as the sole output. The NameGenerator refactor is purely structural — same algorithm, same output size, same seed. No backward compatibility issue because nothing changes externally.

**v2 approach:** Tagged ID format (see E6 above) adds forward compatibility. Old 32-bit untagged names (0x00 tag) are still valid.

---

## E8 — Files to create

| File | Purpose |
|------|---------|
| `Common/src/NameGenerator.h` | Main class declaration (global singleton) |
| `Common/src/NameGenerator.cpp` | Implementation |
| `Common/src/HashFunction.h` | Abstract base for hash backends |
| `Common/src/MurmurHash3Strategy.h` | Wraps existing MurmurHash3 |
| `Common/src/MurmurHash3Strategy.cpp` | Implementation |
| `Common/src/NameStrategy.h` | Abstract base for input-gathering |
| `Common/src/DefaultNameStrategy.h/.cpp` | Preserves current behaviour |
| `Specs/SPEC-012-hash-abstraction.md` | This spec |

## E8 — Files to modify

| File | Change |
|------|--------|
| `Common/src/Process.h` | Remove old method declarations |
| `Common/src/Process.cpp` | Replace all hash calls with `NameGenerator::GetInstance().*` |
| `Common/src/Block.h` | Remove old method declarations |
| `Common/src/Block.cpp` | Remove duplicated hash logic, call `NameGenerator::GetInstance()` |
| `Common/src/MessageBuilder.h/.cpp` | Call `NameGenerator::GetInstance()` |
| `CMakeLists.txt` | Add new source files |

## E9 — Decisions taken

| # | Question | Decision |
|---|----------|----------|
| Q1 | Should NameGenerator be per-Process or global? | **Global singleton.** Avoids incompatibilities between processes. Different VMs run separate binaries, each with its own singleton. |
| Q2 | Seed value? | **Fixed seed = 0** (current behaviour). Keeps IDs deterministic between runs. |
| Q3 | EPGS C code: rewrite in C++ or keep C wrapper? | **Keep as-is.** C wrapper can be added later if EPGS-NameGenerator interop becomes necessary. |
| Q4 | Tagged ID format: implement in v1 or defer? | **Defer to v2.** v1 focus is structural refactor only. Tagged IDs saved as proposal in §E6. |
| Q5 | Remove old methods in one commit or phased? | **Phased** (E4.1→E4.8). Each step is an independent compilable commit. |

## E10 — Alpine VM test plan (comparing hash sizes)

### E10.1 — Objective

Run the same 1core-1repo-1source scenario on Alpine VMs 101/102 with different hash sizes and measure:
- Collision rate (if any)
- Throughput (messages/second)
- Memory usage per binding entry
- Startup time

### E10.2 — Configurations to test

| Config | Output size | Hash function | Notes |
|--------|------------|---------------|-------|
| A (baseline) | 32-bit (4B) | MurmurHash3_x86_32 | Current behaviour |
| B | 64-bit (8B) | MurmurHash3_x86_128 (truncated) | — |
| C | 128-bit (16B) | MurmurHash3_x86_128 | Uses existing 16Bytes variant |
| D | 256-bit (32B) | MurmurHash3 + SHA-256 | Future comparison |
| E | 32-bit (4B) | Identity (no hash, raw LN→hex) | Tests non-hash ID strategy |

### E10.3 — How to switch (after implementation)

```bash
# Via environment or config:
export NG_NAME_STRATEGY=default
export NG_HASH_FUNCTION=murmur3_x86_32
export NG_NAME_SIZE=4   # 4, 8, 16, 32 bytes
```

Or at compile time via CMake:
```cmake
option(NG_NAME_SIZE "Name output size in bytes" 4)
option(NG_HASH_FN "Hash function backend" "murmur3_x86_32")
```

## E11 — Success criteria

1. [ ] All 9 targets build with `NameGenerator` replacing old methods
2. [ ] Alpine VM 1core-1repo-1source test passes identically to pre-refactor (baseline: 32-bit MurmurHash3)
3. [ ] Config change `NG_NAME_SIZE=16` produces different IDs
4. [ ] Config change `NG_NAME_STRATEGY=identity` produces legible-name-based IDs (no hash)
5. [ ] Zero performance regression in throughput test (or documented trade-off)
6. [ ] All ~120 hash call sites migrated
7. [ ] Old methods removed from Block and Process