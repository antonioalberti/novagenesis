# Affected-file record: `SPEC-044-core-run-evaluate-invalid-pg-downcast.md`

SPEC title: SPEC-044: Corrigir downcast inválido em CoreRunEvaluate01

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | — | AIOPT3-origin; historical line; status/gates apply to this snapshot only |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | — | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | — | AIOPT3-origin copy; not authority for AIOPT5 |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- path: `PGCS/src/CoreRunEvaluate01.cpp` — source SPEC line 43.
- path: `CoreRunEvaluate01.h` — source SPEC line 55.

### `AIOPT4` — source `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- path: `PGCS/src/CoreRunEvaluate01.cpp` — source SPEC line 43.
- path: `CoreRunEvaluate01.h` — source SPEC line 55.

### `AIOPT5` — source `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- path: `PGCS/src/CoreRunEvaluate01.cpp` — source SPEC line 43.
- path: `CoreRunEvaluate01.h` — source SPEC line 55.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `8561bb3c31c3ead2e46f3076289504dd26c248b0` — docs(spec): remove out-of-scope ghost references (link basis: the SPEC file changed in this commit)
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- Commit `c542164bd67363532d928826e04f0c5ac29e7dd6` — docs(release): record candidate freeze and scope (link basis: the SPEC file changed in this commit)
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/RELEASE-READINESS-v1.0.0.md`
  - `Specs/RESULTS-SPEC-054/g0-freeze-20260914.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`
  - `Specs/SPEC-047-pgcs-readiness-periodic-activation.md`
- Commit `c71c19129a8fdf0902737ae1fd445cfca3b7ad6e` — SPEC-044: Record attached session teardown failures (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
- Commit `9aa774860ee51c0deb0381cd2b2e76684b9e5495` — SPEC-044: Classify VM teardown SHM exits (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
- Commit `3fecd1694d4348fc56a371b27ceb7a0bb3ee4d3b` — SPEC-044: Record multi-VM runtime evidence (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- Commit `21a5512b0dc34df0c7f730805e55637144087073` — SPEC-044: Fix invalid CoreRunEvaluate01 PG downcast (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `PGCS/src/CoreRunEvaluate01.cpp`
  - `Specs/RESULTS-SPEC-044/post-fix-20260912.md`
  - `Specs/RESULTS-SPEC-044/preliminary-red-20260912.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`

### `AIOPT4`
- Commit `8561bb3c31c3ead2e46f3076289504dd26c248b0` — docs(spec): remove out-of-scope ghost references (link basis: the SPEC file changed in this commit)
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- Commit `c542164bd67363532d928826e04f0c5ac29e7dd6` — docs(release): record candidate freeze and scope (link basis: the SPEC file changed in this commit)
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/RELEASE-READINESS-v1.0.0.md`
  - `Specs/RESULTS-SPEC-054/g0-freeze-20260914.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`
  - `Specs/SPEC-047-pgcs-readiness-periodic-activation.md`
- Commit `c71c19129a8fdf0902737ae1fd445cfca3b7ad6e` — SPEC-044: Record attached session teardown failures (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
- Commit `9aa774860ee51c0deb0381cd2b2e76684b9e5495` — SPEC-044: Classify VM teardown SHM exits (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
- Commit `3fecd1694d4348fc56a371b27ceb7a0bb3ee4d3b` — SPEC-044: Record multi-VM runtime evidence (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- Commit `21a5512b0dc34df0c7f730805e55637144087073` — SPEC-044: Fix invalid CoreRunEvaluate01 PG downcast (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `PGCS/src/CoreRunEvaluate01.cpp`
  - `Specs/RESULTS-SPEC-044/post-fix-20260912.md`
  - `Specs/RESULTS-SPEC-044/preliminary-red-20260912.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`

### `AIOPT5`
- Commit `c71c19129a8fdf0902737ae1fd445cfca3b7ad6e` — SPEC-044: Record attached session teardown failures (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
- Commit `9aa774860ee51c0deb0381cd2b2e76684b9e5495` — SPEC-044: Classify VM teardown SHM exits (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
- Commit `3fecd1694d4348fc56a371b27ceb7a0bb3ee4d3b` — SPEC-044: Record multi-VM runtime evidence (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
- Commit `21a5512b0dc34df0c7f730805e55637144087073` — SPEC-044: Fix invalid CoreRunEvaluate01 PG downcast (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `PGCS/src/CoreRunEvaluate01.cpp`
  - `Specs/RESULTS-SPEC-044/post-fix-20260912.md`
  - `Specs/RESULTS-SPEC-044/preliminary-red-20260912.md`
  - `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
