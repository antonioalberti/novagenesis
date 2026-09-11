# SPEC-036: Staged deprecation of standalone PSS, GIRS and HTS

**Version:** v1.0
**Date:** 2026-09-11
**Author:** Antonio Alberti, with GPT-6 Astra review
**Status:** Proposal
**Branch:** AIOPT3
**Baseline:** `4ff9ec9`
**Task:** `NG-036-deprecacao-standalone-v1-20260911.md`
**Related:** `SPEC-008-ht-bid-raw-socket-discovery.md`, `SPEC-018-payload-sticks-on-reused-inline-response.md`, `SPEC-021-one-message-per-file.md`, `SPEC-022-nrinfopayload01-separate-messages.md`

## 1. Problem / Objective

The repository documents the 2021 integration of PSS, GIRS and HTS into NRNCS, but still builds, packages, launches, discovers and documents the standalone topology as an ordinary supported alternative.

This SPEC establishes a staged deprecation policy:

1. NRNCS becomes the default and only recommended domain-service runtime.
2. Standalone PSS, GIRS and HTS source trees remain available.
3. Standalone binaries are available only through an explicit legacy build profile.
4. Normal builds, images, scripts, tests and operational documentation use NRNCS.
5. Legacy mode is retained for compatibility and historical research, without an implied feature-parity commitment or removal date.
6. Historical evidence remains immutable.

This SPEC is a repository-consistency and lifecycle change. It does not delete the standalone implementations and does not change the inverted pub/sub model.

## 2. Root Cause

The NRNCS integration was not followed by a repository-wide retirement policy:

- `CMakeLists.txt` unconditionally builds `GIRS`, `HTS` and `PSS`.
- Dockerfiles copy the standalone source trees.
- Local and VM scripts launch or require standalone binaries.
- PGCS discovers and exposes standalone services.
- ContentApp, IoTTestApp and NBTestApp retain PSS-first discovery/fallback paths.
- GIRS, HTS and PSS retain cross-discovery of one another.
- Active documentation and SPECs mix current NRNCS behaviour with legacy distributed-mode instructions.

This is not evidence that `Common/src/HT*` is obsolete. Shared `HT*` classes and the NRNCS data path are separate scope and must not be changed merely because the name resembles standalone HTS.

## 3. Proposed Solution

### 3.1 Build and source policy

Introduce `NG_ENABLE_LEGACY_STANDALONE`, defaulting to `OFF`.

- `OFF`: do not define standalone PSS/GIRS/HTS executable targets and do not add their include directories globally.
- `ON`: define the existing standalone targets and emit a clear deprecation warning naming NRNCS as the replacement.
- Keep `PSS/src`, `GIRS/src`, `HTS/src` and their entry points unchanged unless a later SPEC authorises source changes.
- Apply equivalent explicit opt-in semantics to other active build paths discovered during implementation.
- Use fresh build directories for profile validation; stale binaries must not count as evidence.

### 3.2 Runtime policy

The implementation must preserve the distinction between normal and legacy profiles.

Normal profile:

- PGCS and applications use NRNCS as the domain service.
- Standalone PSS/GIRS/HTS are not actively selected or required.
- Standalone advertisements must not create false NRNCS readiness.
- Generic packet forwarding and unrelated routing semantics remain unchanged.

Legacy profile:

- Existing standalone discovery and exposition remain available only when explicitly enabled.
- Startup output must identify the mode as deprecated legacy operation.
- Compatibility behaviour is tested separately from normal NRNCS operation.

If mixed deployments must remain the default, this SPEC must be amended and re-reviewed before implementation. Fallback behaviour must not survive accidentally through an unbounded PSS-first branch.

### 3.3 Scripts, Docker and configuration

- Normal Docker images do not build, copy, ship or require standalone components.
- Any retained legacy image or launcher is explicitly labelled and opt-in.
- `pull-and-build-vms.sh` checks binaries according to the selected profile and propagates configure/build/SSH failures.
- `Intra_OS_Content_Test_PSS.sh` no longer launches the legacy topology as an ordinary test; it becomes a migration notice or an explicitly named legacy launcher.
- `IoT_Test_LoRa.sh` is changed only after its complete argument and IO contract is inspected; replacing `HTS` with `NRNCS` is not assumed to be equivalent.
- Tracked IO/configuration is classified before changes. Untracked evidence directories are not modified.

### 3.4 Documentation and SPEC policy

- README and active operational documentation recommend NRNCS exclusively.
- Active SPECs and diagnostics receive applicability notes and corrected current instructions.
- Historical documents and evidence under `Docs/HISTORICAL` remain byte-for-byte unchanged.
- References that remain must be classified as current NRNCS, explicit legacy, or historical.
- The canonical inverted pub/sub model remains `Docs/ARCHITECTURE/NG-INVERTED-PUB-SUB-MODEL.md` and is not weakened by this change.

## 4. Files Affected and Traceability Matrix

Disposition classes:

- **M** — must change or be explicitly validated during implementation;
- **R** — may remain, but must be classified as legacy or historical;
- **N** — must not change under this SPEC.

| Area | Locations | Class | Required disposition |
|---|---|---:|---|
| Standalone source | `PSS/src`, `GIRS/src`, `HTS/src`, `exec*.cpp` | R | Retain; legacy-only buildability |
| Runtime profile helper | `Common/src/NGRuntimeProfile.h` | M | Default `normal`; explicit `NG_RUNTIME_PROFILE=legacy` enables legacy selection |
| Shared implementation | `Common/src/HT*`, messaging, GW | N | No retirement or semantic change from this SPEC |
| NRNCS data path | `NRNCS/src`, `Common/src/HTGetBind01.cpp` | N | Preserve cache and delivery semantics |
| Root build | `CMakeLists.txt` | M | Add explicit OFF-by-default legacy profile and scoped includes |
| Other build paths | `Make/`, image CMake and helpers | M/R | Inventory and gate active routes |
| Docker | `Dockerfile`, `Docker/PGCS-Only/Dockerfile` | M | Remove unconditional standalone packaging |
| Local scripts | `Scripts/Simple/` | M/R | Normal scripts use NRNCS; legacy launcher is explicit |
| VM scripts | `Scripts/AlpineVMs/pull-and-build-vms.sh` | M | Profile-aware checks and reliable failure propagation |
| IO/config | tracked `IO/PSS`, `IO/GIRS`, `IO/HTS`, `IO/NRNCS` references | M/R | Classify and update active consumers; preserve required legacy inputs |
| PGCS runtime | `PGRunPeriodic01.cpp`, `PGRunExposition01.cpp` | M | Implement approved normal/legacy discovery policy |
| Applications | ContentApp, IoTTestApp, NBTestApp discovery | M | Remove accidental PSS-first normal selection; test both profiles |
| Legacy discovery | GIRS/HTS/PSS periodic and status paths | R | Retain only under legacy profile unless separately justified |
| Active documentation | `README.md`, active docs | M | One consistent deprecation policy |
| Active SPECs/diagnostics | SPEC-008, SPEC-018, SPEC-021, timer diagnostic | M | Add applicability/classification without erasing evidence |
| Historical documentation | `Docs/HISTORICAL/` | N/R | Preserve contents; external classification permitted |
| Untracked evidence | existing `IO/NG-*` evidence directories | N | Do not modify or commit as part of this SPEC |

The implementation inventory must use tracked-file searches and classify every relevant remaining reference. A zero-match requirement is invalid: legitimate legacy and historical references are allowed when explicitly classified.

## 5. Acceptance Criteria

1. Every relevant tracked reference is classified and every M item is resolved.
2. A fresh normal build succeeds without producing or requiring PSS, GIRS or HTS standalone binaries.
3. A fresh legacy build succeeds for all three standalone executables and reports the deprecation warning.
4. Normal images and scripts do not package or launch standalone services.
5. Retained legacy routes are explicit and separately labelled.
6. The discovery matrix is validated:

| Profile | Available services | Required result |
|---|---|---|
| Normal | NRNCS only | Discovery and readiness succeed |
| Normal | Standalone only | No standalone selection and no false NRNCS readiness |
| Normal | Both | NRNCS selected; no active standalone exposition by normal path |
| Legacy | Standalone only | Existing compatibility path remains functional |
| Legacy | Both | Approved legacy precedence is demonstrated |

7. All three relevant applications are covered by the normal/legacy discovery tests.
8. NRNCS publication, resolution and ContentApp delivery remain functional with byte-exact manifests and hashes.
9. The revised IoT/LoRa/EPGS scenario is validated, or the missing prerequisite is recorded as a blocking gap rather than a pass.
10. Active documentation contains no unqualified recommendation or requirement for standalone deployment.
11. Historical evidence and excluded untracked IO evidence are unchanged.
12. Build, runtime and documentation changes are independently revertible.

These criteria are proposed gates; no implementation or test result is claimed by this SPEC.

## 6. Rollback Plan

- Keep build/packaging, runtime policy and documentation changes in separable commits, all prefixed `SPEC-036:`.
- Revert the affected commits if a normal NRNCS regression gate fails.
- Recreate fresh build directories and rebuild images after rollback.
- Do not use legacy opt-in as proof that rollback succeeded.
- No source deletion or data migration is required.

## 7. Implementation Notes

- Reserve the SPEC number before committing this document; `SPEC-036` is the first unused number found in `Specs/` at the baseline.
- Inspect complete files before changing discovery control flow; matching-line excerpts are not sufficient for ownership or lifecycle decisions.
- Do not mechanically replace every occurrence of `PSS`, `PS`, `HTS` or `HT`; some are shared identifiers, historical evidence or internal storage names.
- Do not modify untracked runtime evidence directories.
- Separate stale-cache or payload defects from this lifecycle change.
- Run normal and legacy builds from fresh directories and verify executable absence/presence explicitly.
- Obtain a final Astra review of the implementation results before marking this SPEC Implemented.

## 8. Decisions Log

| ID | Proposed decision | State |
|---|---|---|
| D1 | Retain standalone source trees with an OFF-by-default legacy build profile | Accepted for implementation |
| D2 | Use NRNCS-only discovery/exposition in normal mode | Accepted for implementation |
| D3 | Preserve legacy compatibility only in explicit `NG_RUNTIME_PROFILE=legacy` mode; build capability and runtime selection remain separate | Accepted for implementation |
| D4 | Existing PSS launcher requires `NG_RUNTIME_PROFILE=legacy` and fails closed otherwise | Accepted for implementation |
| D5 | Keep historical evidence immutable | Required preservation rule |
| D6 | Do not change shared HT implementation, protocol or payload semantics | Required scope boundary |

## 9. Pitfalls

- Documentation-only deprecation leaves runtime behaviour inconsistent.
- Removing build targets while retaining Docker/script dependencies breaks deployment.
- Stale binaries can make a disabled target appear available.
- PSS-first short-circuit discovery must not be mistaken for NRNCS-first behaviour.
- Gating only tuple lookup may leave standalone readiness checks active.
- Blanket replacement of `HTS`, `PS` or `PSS` can damage shared code and historical accuracy.
- Historical payload-forwarding descriptions must not override the canonical inverted pub/sub model.
- A passing legacy build does not prove normal-mode deprecation is complete.
