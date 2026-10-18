# Affected-file record: `SPEC-060-ng-elc-remote-payload-evidence.md`

SPEC title: SPEC-060: Gate de evidência remota de payload para a release de engenharia v1.0

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | no | — | — | — | — | — | — |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | (not declared) | In Progress — gate remoto de engenharia; controller-only concluído; integração remota OPEN | (not declared) | <br> | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | no | — | — | — | — | — | — |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT4` — source `Specs/SPEC-060-ng-elc-remote-payload-evidence.md`
- No literal file paths were detected in the recognized scope/file sections.

No explicit file paths found in the recognized sections for this SPEC. This is an inventory gap, not evidence that no files were affected.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT4`
- Commit `f2a6d9ecd829948d2a4aa6a0f485e5b0954ec6f3` — docs(release): apply Astra TRL3 research rebaseline (link basis: the SPEC file changed in this commit)
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/RESULTS-SPEC-059/release-scope-abc-rebaseline-proposal-20260920.md`
  - `Specs/SESSION-HANDOFF-SPEC-059.md`
  - `Specs/SPEC-060-ng-elc-remote-payload-evidence.md`
- Commit `36425aaef1cc93d6e626ad9a5088dad17e03050b` — spec(evidence): define remote verifier attestation seam (link basis: the SPEC file changed in this commit)
  - `Specs/SPEC-060-ng-elc-remote-payload-evidence.md`
- Commit `2d8bee6ae980ab49c76d3c2d23826adbec13de02` — docs(evidence): record conditional Astra review (link basis: the SPEC file changed in this commit)
  - `Scripts/AlpineVMs/tests/test_spec060_payload_evidence.py`
  - `Specs/RESULTS-SPEC-059/astra-spec060-f1bf3bc-abc-final-review-20260920.md`
  - `Specs/RESULTS-SPEC-059/release-scope-abc-rebaseline-proposal-20260920.md`
  - `Specs/SPEC-060-ng-elc-remote-payload-evidence.md`
- Commit `f1bf3bc94db4857c3b94966f9183372850948992` — fix(evidence): anchor origin reads and enforce aggregate bounds (link basis: the SPEC file changed in this commit)
  - `Scripts/AlpineVMs/payload_evidence.py`
  - `Scripts/AlpineVMs/tests/test_spec060_payload_evidence.py`
  - `Specs/SPEC-060-ng-elc-remote-payload-evidence.md`
- Commit `8038dca54233062c9b7a0bae089aa82c233767e6` — fix(spec): keep audit-readable metadata (link basis: the SPEC file changed in this commit)
  - `Specs/SPEC-060-ng-elc-remote-payload-evidence.md`
- Commit `e308fdd526a9bb7f07d6935e5be1c312a6740dcb` — feat(evidence): add remote payload contract fixtures (link basis: the SPEC file changed in this commit)
  - `Scripts/AlpineVMs/evidence_verifier.py`
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/payload_evidence.py`
  - `Scripts/AlpineVMs/tests/test_spec060_payload_evidence.py`
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/RESULTS-SPEC-054/astra-normal-remote-trial-packet-review-20260920.md`
  - `Specs/RESULTS-SPEC-054/astra-spec060-corrected-review-20260920.md`
  - `Specs/RESULTS-SPEC-054/astra-spec060-final-proposal-review-20260920.md`
  - `Specs/RESULTS-SPEC-054/astra-spec060-remote-payload-evidence-review-20260920.md`
  - `Specs/RESULTS-SPEC-054/normal-remote-trial-packet-20260920.md`
  - `Specs/SPEC-060-ng-elc-remote-payload-evidence.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
