# Affected-file record: `SPEC-030-photo-delivery-integrity.md`

SPEC title: SPEC-030: Photo Delivery Integrity — Immutable Inputs + Publish-Time Hash Manifest

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | AIOPT3 | v1.0 (Draft — per Astra requirement + user directive 2026-09-07) | Draft — Hermes review pending, then Astra implements | (not declared) | (not declared) | AIOPT3-origin; historical line; status/gates apply to this snapshot only |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | v1.0 (Draft — per Astra requirement + user directive 2026-09-07) | Draft — Hermes review pending, then Astra implements | (not declared) | (not declared) | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | AIOPT3 | v1.0 (Draft — per Astra requirement + user directive 2026-09-07) | Draft — Hermes review pending, then Astra implements | (not declared) | (not declared) | AIOPT3-origin copy; not authority for AIOPT5 |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-030-photo-delivery-integrity.md`
- path: `Scripts/Python/BuildPhotos.py` — source SPEC line 72.
- path: `Scripts/AlpineVMs/run_Source_on_Source_VM.sh` — source SPEC line 73.
- path: `Scripts/Python/VerifyDelivery.py` — source SPEC line 74.

### `AIOPT4` — source `Specs/SPEC-030-photo-delivery-integrity.md`
- path: `Scripts/Python/BuildPhotos.py` — source SPEC line 72.
- path: `Scripts/AlpineVMs/run_Source_on_Source_VM.sh` — source SPEC line 73.
- path: `Scripts/Python/VerifyDelivery.py` — source SPEC line 74.

### `AIOPT5` — source `Specs/SPEC-030-photo-delivery-integrity.md`
- path: `Scripts/Python/BuildPhotos.py` — source SPEC line 72.
- path: `Scripts/AlpineVMs/run_Source_on_Source_VM.sh` — source SPEC line 73.
- path: `Scripts/Python/VerifyDelivery.py` — source SPEC line 74.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `11e1f91a57bfd972b381566ed735f6abf2e2f517` — SPEC-030 implemented and runtime-verified: 1000/1000 byte-exact photo delivery (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/Python/BuildPhotos.py`
  - `Scripts/Python/VerifyDelivery.py`
  - `Specs/RESULTS-SPEC-027/spec030-runtime-1000-photos.json`
- Commit `350e6048d3a6e6bd9793ba95dc256226bd0984c1` — SPEC-030 (Draft): photo delivery integrity — immutable inputs + publish-time sha256 manifest (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-030-photo-delivery-integrity.md`

### `AIOPT4`
- Commit `11e1f91a57bfd972b381566ed735f6abf2e2f517` — SPEC-030 implemented and runtime-verified: 1000/1000 byte-exact photo delivery (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/Python/BuildPhotos.py`
  - `Scripts/Python/VerifyDelivery.py`
  - `Specs/RESULTS-SPEC-027/spec030-runtime-1000-photos.json`
- Commit `350e6048d3a6e6bd9793ba95dc256226bd0984c1` — SPEC-030 (Draft): photo delivery integrity — immutable inputs + publish-time sha256 manifest (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-030-photo-delivery-integrity.md`

### `AIOPT5`
- Commit `11e1f91a57bfd972b381566ed735f6abf2e2f517` — SPEC-030 implemented and runtime-verified: 1000/1000 byte-exact photo delivery (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/Python/BuildPhotos.py`
  - `Scripts/Python/VerifyDelivery.py`
  - `Specs/RESULTS-SPEC-027/spec030-runtime-1000-photos.json`
- Commit `350e6048d3a6e6bd9793ba95dc256226bd0984c1` — SPEC-030 (Draft): photo delivery integrity — immutable inputs + publish-time sha256 manifest (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-030-photo-delivery-integrity.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
