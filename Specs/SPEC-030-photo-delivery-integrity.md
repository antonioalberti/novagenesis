# SPEC-030: Photo Delivery Integrity — Immutable Inputs + Publish-Time Hash Manifest

**Version:** v1.0 (Draft — per Astra requirement + user directive 2026-09-07)
**Date:** 2026-09-07
**Author:** Spec by Astra (gpt-6-astra); implementation by Astra; verification by Hermes
**Status:** Draft — Hermes review pending, then Astra implements
**Branch:** AIOPT3
**Related:** SPEC-027-pgcs-multithreading.md, Specs/RESULTS-SPEC-027/amend3v2-photos-1000.json

---

## 1. Problem

In the 2026-09-07 1000-photo delivery test, 20/1000 files (00000–00019) had md5
mismatches between the local Source1 directory and the Repository copies. Investigation
concluded the cause was a **test-procedure artifact**: the operator re-ran
`BuildPhotos.py different 1000` while the Source ContentApp was publishing, and
`BuildPhotos different` generates **random pixel content per run**. The bytes actually
published arrived intact (all 1000 Repository copies are valid JPEG 800×600).

However, this cannot be distinguished from real corruption **by the test evidence
itself** — only by out-of-band reasoning about the operator's actions. The user has
ruled: **this situation must be impossible in future runs.** Two defects:

1. **Mutable test inputs.** Files in the publish directory were overwritten while the
   Source ContentApp was running/publishing.
2. **No integrity manifest.** There was no publication-time record of what was
   published, so delivery could not be verified byte-for-byte against what was sent.

## 2. Root Cause

- `BuildPhotos.py` writes into the live publish directory (`IO/Source1/`) with
  non-deterministic content and no coordination with the running ContentApp.
- The delivery pipeline has no end-to-end integrity check: the test compared
  *repository content* against *current source directory state*, which is not an
  invariant of the system under test.

## 3. Proposed Solution (Astra design)

Three independent measures; each is a hard requirement:

### 3.1 Immutable publish directory

- Generate photos into a **staging directory** (`IO/SourceStaging/<run-id>/`),
  never directly into `IO/Source1/`.
- The Source ContentApp is started only after generation completes, and the run-id
  directory is treated as **read-only** for the duration of the run (chmod a-w as a
  tripwire; ContentApp does not need write access there).
- Regeneration for a new test always creates a **new run-id directory**; no in-place
  overwrite ever happens.

### 3.2 Publish-time hash manifest

- Before the Source ContentApp starts (or as its first action if integrated), produce
  `IO/SourceStaging/<run-id>/manifest.sha256` containing `sha256sum` lines
  (`<hash>  <filename>`) for every file to be published.
- The manifest is part of the staged, read-only content.
- After delivery, verification is: compute sha256 over each Repository-received file
  and compare against the manifest — **the only accepted integrity comparison**.

### 3.3 Delivery verification script

- New script `Scripts/Python/VerifyDelivery.py <manifest> <received-dir>`:
  - Reports `total / matched / missing / mismatched` and exits non-zero on any
    missing or mismatched file.
  - This becomes the standard completion criterion for payload-delivery tests.

## 4. Files Affected

| File | Change | Reason |
|---|---|---|
| `Scripts/Python/BuildPhotos.py` | Write to a staging dir given by a new `--staging <dir>` argument (default keeps old behaviour for backward compat) | 3.1 |
| `Scripts/AlpineVMs/run_Source_on_Source_VM.sh` | Use staging dir + generate manifest before starting ContentApp; pass run-id | 3.1, 3.2 |
| `Scripts/Python/VerifyDelivery.py` | **New** — manifest-based verification | 3.3 |
| `Specs/RESULTS-SPEC-027/` | Future photo tests record `<run-id>`, manifest hash, and VerifyDelivery output | traceability |

No changes to PGCS/NGAL/GW/ContentApp C++ code, wire format, or IPC. This is test
tooling only.

## 5. Acceptance Criteria

1. `BuildPhotos.py --staging <dir> N` writes all N photos into `<dir>` and nothing
   into `IO/Source1/`.
2. A `manifest.sha256` exists in the staging dir and lists exactly the generated files.
3. Staging dir (and manifest) are read-only during the run; attempting to modify a
   file fails (or the modification is detectable via manifest mismatch).
4. A 1000-photo delivery run with immutable inputs verifies **1000/1000 matched,
   0 missing, 0 mismatched** via `VerifyDelivery.py`.
5. Deliberately corrupting one received file (test of the verifier itself) makes
   `VerifyDelivery.py` report exactly 1 mismatched and exit non-zero.
6. No C++ source changes in the diff.

## 6. Rollback Plan

Revert the tooling commit. No runtime behaviour depends on it.

## 7. Decisions Log

| # | Decision | Date | Reason |
|---|---|---|---|
| 1 | Test tooling only; no C++ changes | 2026-09-07 | The delivery pipeline itself showed no defect |
| 2 | sha256 manifest at publish time | 2026-09-07 | Verifies *what was published*, not *what the dir happens to contain later* |
| 3 | Run-id staging directories | 2026-09-07 | Regeneration must never overwrite published inputs |

## 8. Pitfalls

- `sha256sum` exists on Alpine (busybox) — verify format matches Python-side parsing.
- ContentApp may iterate `IO/Source1/` at start-up: the staging dir must be the one
  passed as the ContentApp IO argument for the run (keep `IO/Source1` as a symlink or
  pass the staging path directly — implementation choice, Phase 0 decision).
- The manifest must be generated **after** the last file is fully written (fsync/EOF),
  never concurrently with generation.
