# Affected-file record: `SPEC-046-multi-vm-runner-safe-teardown.md`

SPEC title: SPEC-046: Runner multi-VM e teardown seguro

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | c132e23 | AIOPT3-origin; historical line; status/gates apply to this snapshot only |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | c132e23 | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | c132e23 | AIOPT3-origin copy; not authority for AIOPT5 |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT4` — source `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT5` — source `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- No literal file paths were detected in the recognized scope/file sections.

No explicit file paths found in the recognized sections for this SPEC. This is an inventory gap, not evidence that no files were affected.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `1af604dad66148618cc0e578c28adb4a9829e607` — SPEC-046: Record extended PGCS readiness diagnostic (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md`
- Commit `a953a1457743c1c1b8154b6dec1bb5ea3dd81175` — SPEC-046: Record supervised PGCS diagnostic (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- Commit `357596a494891242a8390e95f414ef40c5e9a6e0` — docs: reconcile release handoff records (link basis: the SPEC file changed in this commit)
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- Commit `c132e2392389fc9d29de14b0fced82941ec53cc3` — SPEC-046: Add bounded process supervisor (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/supervise_process.py`
  - `Scripts/AlpineVMs/supervise_process.sh`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`

### `AIOPT4`
- Commit `1af604dad66148618cc0e578c28adb4a9829e607` — SPEC-046: Record extended PGCS readiness diagnostic (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md`
- Commit `a953a1457743c1c1b8154b6dec1bb5ea3dd81175` — SPEC-046: Record supervised PGCS diagnostic (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- Commit `357596a494891242a8390e95f414ef40c5e9a6e0` — docs: reconcile release handoff records (link basis: the SPEC file changed in this commit)
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- Commit `c132e2392389fc9d29de14b0fced82941ec53cc3` — SPEC-046: Add bounded process supervisor (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/supervise_process.py`
  - `Scripts/AlpineVMs/supervise_process.sh`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`

### `AIOPT5`
- Commit `1af604dad66148618cc0e578c28adb4a9829e607` — SPEC-046: Record extended PGCS readiness diagnostic (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md`
- Commit `a953a1457743c1c1b8154b6dec1bb5ea3dd81175` — SPEC-046: Record supervised PGCS diagnostic (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- Commit `357596a494891242a8390e95f414ef40c5e9a6e0` — docs: reconcile release handoff records (link basis: the SPEC file changed in this commit)
  - `Specs/SPEC-045-core-evaluate-functional-harness.md`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`
- Commit `c132e2392389fc9d29de14b0fced82941ec53cc3` — SPEC-046: Add bounded process supervisor (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/supervise_process.py`
  - `Scripts/AlpineVMs/supervise_process.sh`
  - `Specs/SPEC-046-multi-vm-runner-safe-teardown.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
