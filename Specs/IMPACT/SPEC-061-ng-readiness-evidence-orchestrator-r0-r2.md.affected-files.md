# Affected-file record: `SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`

SPEC title: SPEC-061: Read-Only Release-Readiness Evidence Orchestrator — R0–R2

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | no | — | — | — | — | — | — |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | (not declared) | Draft | (not declared) | pending | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | no | — | — | — | — | — | — |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT4` — source `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
- path: `Scripts/ProjectAudit/ng_readiness_evidence.py` — source SPEC line 416.
- path: `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json` — source SPEC line 417.
- path: `Scripts/ProjectAudit/tests/test_ng_readiness_evidence.py` — source SPEC line 418.
- path: `Scripts/AlpineVMs/ng_remote_executor.py` — source SPEC line 422.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT4`
- Commit `51ea4c8a127a9c00bf340d2da02c702a19613434` — SPEC-061: resolve readiness AMEND (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/ProjectAudit/tests/test_validate_spec061_contract.py`
  - `Scripts/ProjectAudit/validate_spec061_contract.py`
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/build-manifest.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/build-receipt.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/freeze.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/git-repository.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/plan.current.json`
  - `Specs/SPEC-061-ng-readiness-adapter-registry-v1.json`
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
  - `Specs/SPEC-061-ng-readiness-policy-v1.json`
- Commit `c28a1061df1296f97bffef1709c4e4a6f65d403a` — fix(spec-061): exclude mutable docs from root observation (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json`
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
  - `Specs/SPEC-061-ng-readiness-policy-v1.json`
- Commit `ae05684313607fc967395daf5a5cf8e653e9e8e4` — docs(spec-061): promote approved contract to proposal (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
- Commit `91e1dd5ce98ca7877dcbcf50a1c2542ffd904ec0` — docs(spec-061): reconcile revision 8 report contract (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
- Commit `7ae63bad9314124ec08aaad921450aa1af2b1daa` — fix(spec-061): close revision 8 contract semantics (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/ProjectAudit/schemas/ng-readiness-input-manifest-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json`
  - `Scripts/ProjectAudit/validate_spec061_contract.py`
  - `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/freeze.current.json`
  - `Specs/SPEC-061-ng-readiness-adapter-registry-v1.json`
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
  - `Specs/SPEC-061-ng-readiness-policy-v1.json`
- Commit `65b431cf16959f1d653db69bd4bdd17570ce6aca` — SPEC-061: close trusted profile amendments (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/ProjectAudit/schemas/ng-readiness-input-manifest-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-trusted-execution-profile-v1.json`
  - `Scripts/ProjectAudit/validate_spec061_contract.py`
  - `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json`
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
  - `Specs/SPEC-061-ng-readiness-policy-v1.json`
- Commit `14ac9a29020af020c8c91468f65bba2b9b17c0b2` — SPEC-061: close Astra amendments for Revision 6 (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/ProjectAudit/schemas/ng-readiness-adapter-registry-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-input-manifest-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-trusted-execution-profile-v1.json`
  - `Scripts/ProjectAudit/validate_spec061_contract.py`
  - `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/git-repository.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/repo-root.current.json`
  - `Specs/SPEC-061-ng-readiness-adapter-registry-v1.json`
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
  - `Specs/SPEC-061-ng-readiness-policy-v1.json`
- Commit `82a3a30c52debede11423a3b523838816f7f046b` — docs(spec-061): close readiness evidence contract (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/ProjectAudit/schemas/ng-readiness-evidence-record-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-input-manifest-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json`
  - `Scripts/ProjectAudit/schemas/ng-readiness-trusted-execution-profile-v1.json`
  - `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/build-manifest.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/build-receipt.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/freeze.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/git-repository.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/plan.current.json`
  - `Specs/RESULTS-SPEC-061/fixtures/records/repo-root.current.json`
  - `Specs/SPEC-061-ng-readiness-adapter-registry-v1.json`
  - `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md`
  - `Specs/SPEC-061-ng-readiness-policy-v1.json`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
