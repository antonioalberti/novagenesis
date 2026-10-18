# Affected-file record: `SPEC-031-recvmmsg-batched-receive.md`

SPEC title: SPEC-031 — Batched RAW Receive with `recvmmsg()`

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | (none) | (not declared) | Draft — approval required before implementation | (not declared) | (not declared) | Implemented in source; SPEC Draft/current recvfrom baseline stale; no benchmark acceptance implied |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | (none) | (not declared) | Draft — approval required before implementation | (not declared) | (not declared) | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | (none) | (not declared) | Draft — approval required before implementation | (not declared) | (not declared) | Implemented in source; SPEC Draft/current recvfrom baseline stale; no benchmark acceptance implied |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-031-recvmmsg-batched-receive.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT4` — source `Specs/SPEC-031-recvmmsg-batched-receive.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT5` — source `Specs/SPEC-031-recvmmsg-batched-receive.md`
- No literal file paths were detected in the recognized scope/file sections.

No explicit file paths found in the recognized sections for this SPEC. This is an inventory gap, not evidence that no files were affected.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `a0c47a31370ca7c87b307cf254bc494d3c0c1ee0` — SPEC-031 fix: remove MSG_TRUNC as INPUT flag on recvmmsg (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/NGAL_Transport_RAW.cpp`
- Commit `aa300c317ffaa7e38bca7cb149c389d28550c1ef` — SPEC-032 diagnostic: regression isolated to GW batching implementation (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
- Commit `b0968898beef63b206f70eec3a3b3580ea7e7eb4` — DIAGNOSTIC: revert SPEC-032 GW batching to isolate receive-path regression (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `8e92e295033f894793fee6a5c2c12c7014f05e17` — SPEC-031 @2000 msg/s final: PASS - zero loss, backlog transient (drains to 2) (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-after-2000.json`
- Commit `58d30a4329fb763ca239638a2ad0b32c1bb870f2` — SPEC-031 AFTER @500 msg/s: stable, zero loss with recvmmsg batching (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-after-500.json`
- Commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` — SPEC-031: batched RAW receive with recvmmsg (implemented by Hermes per Astra SPEC) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/NGAL_SAR.h`
  - `PGCS/src/NGAL_Transport_RAW.cpp`
  - `PGCS/src/PG.cpp`
- Commit `b0edd69ba83ed028a2225ca38a7249bd13c637ff` — SPEC-031 (Draft): batched RAW receive with recvmmsg + BEFORE baseline captured (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-before-baseline.json`
  - `Specs/SPEC-031-recvmmsg-batched-receive.md`

### `AIOPT4`
- Commit `a0c47a31370ca7c87b307cf254bc494d3c0c1ee0` — SPEC-031 fix: remove MSG_TRUNC as INPUT flag on recvmmsg (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/NGAL_Transport_RAW.cpp`
- Commit `aa300c317ffaa7e38bca7cb149c389d28550c1ef` — SPEC-032 diagnostic: regression isolated to GW batching implementation (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
- Commit `b0968898beef63b206f70eec3a3b3580ea7e7eb4` — DIAGNOSTIC: revert SPEC-032 GW batching to isolate receive-path regression (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `8e92e295033f894793fee6a5c2c12c7014f05e17` — SPEC-031 @2000 msg/s final: PASS - zero loss, backlog transient (drains to 2) (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-after-2000.json`
- Commit `58d30a4329fb763ca239638a2ad0b32c1bb870f2` — SPEC-031 AFTER @500 msg/s: stable, zero loss with recvmmsg batching (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-after-500.json`
- Commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` — SPEC-031: batched RAW receive with recvmmsg (implemented by Hermes per Astra SPEC) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/NGAL_SAR.h`
  - `PGCS/src/NGAL_Transport_RAW.cpp`
  - `PGCS/src/PG.cpp`
- Commit `b0edd69ba83ed028a2225ca38a7249bd13c637ff` — SPEC-031 (Draft): batched RAW receive with recvmmsg + BEFORE baseline captured (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-before-baseline.json`
  - `Specs/SPEC-031-recvmmsg-batched-receive.md`

### `AIOPT5`
- Commit `a0c47a31370ca7c87b307cf254bc494d3c0c1ee0` — SPEC-031 fix: remove MSG_TRUNC as INPUT flag on recvmmsg (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/NGAL_Transport_RAW.cpp`
- Commit `aa300c317ffaa7e38bca7cb149c389d28550c1ef` — SPEC-032 diagnostic: regression isolated to GW batching implementation (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
- Commit `b0968898beef63b206f70eec3a3b3580ea7e7eb4` — DIAGNOSTIC: revert SPEC-032 GW batching to isolate receive-path regression (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `8e92e295033f894793fee6a5c2c12c7014f05e17` — SPEC-031 @2000 msg/s final: PASS - zero loss, backlog transient (drains to 2) (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-after-2000.json`
- Commit `58d30a4329fb763ca239638a2ad0b32c1bb870f2` — SPEC-031 AFTER @500 msg/s: stable, zero loss with recvmmsg batching (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-after-500.json`
- Commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` — SPEC-031: batched RAW receive with recvmmsg (implemented by Hermes per Astra SPEC) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/NGAL_SAR.h`
  - `PGCS/src/NGAL_Transport_RAW.cpp`
  - `PGCS/src/PG.cpp`
- Commit `b0edd69ba83ed028a2225ca38a7249bd13c637ff` — SPEC-031 (Draft): batched RAW receive with recvmmsg + BEFORE baseline captured (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec031-before-baseline.json`
  - `Specs/SPEC-031-recvmmsg-batched-receive.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
