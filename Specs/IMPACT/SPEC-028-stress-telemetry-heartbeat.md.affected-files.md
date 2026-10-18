# Affected-file record: `SPEC-028-stress-telemetry-heartbeat.md`

SPEC title: SPEC-028: Stress Telemetry Heartbeat for PGCS

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | AIOPT3 | 0.1 | Draft — user must approve before implementation | (not declared) | (not declared) | AIOPT3-origin; historical line; status/gates apply to this snapshot only |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | 0.1 | Draft — user must approve before implementation | (not declared) | (not declared) | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | AIOPT3 | 0.1 | Draft — user must approve before implementation | (not declared) | (not declared) | AIOPT3-origin copy; not authority for AIOPT5 |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-028-stress-telemetry-heartbeat.md`
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 46.
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 47.
- path: `PGCS/src/PG.h` — source SPEC line 96.
- path: `PGCS/src/PG.cpp` — source SPEC line 97.
- path: `PGCS/src/PGRunPeriodic01.cpp` — source SPEC line 98.
- path: `PGCS/src/PGRunStresstest01.cpp` — source SPEC line 99.
- path: `PGCS/src/PGStresstestPing01.cpp` — source SPEC line 100.
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 101.
- path: `Common/src/GW.cpp` — source SPEC line 102.
- path: `PG.cpp` — source SPEC line 104.

### `AIOPT4` — source `Specs/SPEC-028-stress-telemetry-heartbeat.md`
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 46.
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 47.
- path: `PGCS/src/PG.h` — source SPEC line 96.
- path: `PGCS/src/PG.cpp` — source SPEC line 97.
- path: `PGCS/src/PGRunPeriodic01.cpp` — source SPEC line 98.
- path: `PGCS/src/PGRunStresstest01.cpp` — source SPEC line 99.
- path: `PGCS/src/PGStresstestPing01.cpp` — source SPEC line 100.
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 101.
- path: `Common/src/GW.cpp` — source SPEC line 102.
- path: `PG.cpp` — source SPEC line 104.

### `AIOPT5` — source `Specs/SPEC-028-stress-telemetry-heartbeat.md`
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 46.
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 47.
- path: `PGCS/src/PG.h` — source SPEC line 96.
- path: `PGCS/src/PG.cpp` — source SPEC line 97.
- path: `PGCS/src/PGRunPeriodic01.cpp` — source SPEC line 98.
- path: `PGCS/src/PGRunStresstest01.cpp` — source SPEC line 99.
- path: `PGCS/src/PGStresstestPing01.cpp` — source SPEC line 100.
- path: `Common/src/NGAL_SAR.cpp` — source SPEC line 101.
- path: `Common/src/GW.cpp` — source SPEC line 102.
- path: `PG.cpp` — source SPEC line 104.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `75caf4cda356b708b944799ba4c00142cae0a01d` — SPEC-028-B validated: heartbeat telemetry working, ZERO loss at 500 msg/s (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`
- Commit `37fa0a71aaf835d45d19b16badcf8574508f7c18` — SPEC-028-B fix: start heartbeat AFTER PGCS.ini parse in PGRunInitialization01 (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PGRunInitialization01.cpp`
- Commit `27b4b7b9f53b4be797cb88740cbdbade584c352e` — SPEC-028-B: stress telemetry heartbeat implemented (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.h`
  - `Common/src/NGAL_SAR.cpp`
  - `Common/src/NGAL_SAR.h`
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PG.h`
  - `PGCS/src/PGRunPeriodic01.cpp`
  - `PGCS/src/PGRunStresstest01.cpp`
  - `PGCS/src/PGStresstestPing01.cpp`
- Commit `faa6c4e1f606b4bd355697b122d4c682038e1d7a` — SPEC-028-D: directed guard tests 6/6 PASS (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`
- Commit `f3f843bb2e06f14d1c0366581404667c46019d6b` — SPEC-028 (Draft): stress telemetry heartbeat — 10s SPEC028_STATS line, counters mapped (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-028-stress-telemetry-heartbeat.md`

### `AIOPT4`
- Commit `75caf4cda356b708b944799ba4c00142cae0a01d` — SPEC-028-B validated: heartbeat telemetry working, ZERO loss at 500 msg/s (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`
- Commit `37fa0a71aaf835d45d19b16badcf8574508f7c18` — SPEC-028-B fix: start heartbeat AFTER PGCS.ini parse in PGRunInitialization01 (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PGRunInitialization01.cpp`
- Commit `27b4b7b9f53b4be797cb88740cbdbade584c352e` — SPEC-028-B: stress telemetry heartbeat implemented (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.h`
  - `Common/src/NGAL_SAR.cpp`
  - `Common/src/NGAL_SAR.h`
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PG.h`
  - `PGCS/src/PGRunPeriodic01.cpp`
  - `PGCS/src/PGRunStresstest01.cpp`
  - `PGCS/src/PGStresstestPing01.cpp`
- Commit `faa6c4e1f606b4bd355697b122d4c682038e1d7a` — SPEC-028-D: directed guard tests 6/6 PASS (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`
- Commit `f3f843bb2e06f14d1c0366581404667c46019d6b` — SPEC-028 (Draft): stress telemetry heartbeat — 10s SPEC028_STATS line, counters mapped (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-028-stress-telemetry-heartbeat.md`

### `AIOPT5`
- Commit `75caf4cda356b708b944799ba4c00142cae0a01d` — SPEC-028-B validated: heartbeat telemetry working, ZERO loss at 500 msg/s (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`
- Commit `37fa0a71aaf835d45d19b16badcf8574508f7c18` — SPEC-028-B fix: start heartbeat AFTER PGCS.ini parse in PGRunInitialization01 (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PGRunInitialization01.cpp`
- Commit `27b4b7b9f53b4be797cb88740cbdbade584c352e` — SPEC-028-B: stress telemetry heartbeat implemented (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.h`
  - `Common/src/NGAL_SAR.cpp`
  - `Common/src/NGAL_SAR.h`
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PG.h`
  - `PGCS/src/PGRunPeriodic01.cpp`
  - `PGCS/src/PGRunStresstest01.cpp`
  - `PGCS/src/PGStresstestPing01.cpp`
- Commit `faa6c4e1f606b4bd355697b122d4c682038e1d7a` — SPEC-028-D: directed guard tests 6/6 PASS (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`
- Commit `f3f843bb2e06f14d1c0366581404667c46019d6b` — SPEC-028 (Draft): stress telemetry heartbeat — 10s SPEC028_STATS line, counters mapped (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-028-stress-telemetry-heartbeat.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
