# SPEC-069: NG-039 Public Evidence Deduplication

**Version:** v1.0
**Date:** 2026-10-17
**Author:** Antonio Alberti / Hermes Agent
**Status:** Implemented
**Branch:** AIOPT5
**Baseline commit:** `ac357849ed7109c0e13ea3faf809b01dc9645694`
**Implementation commit:** Recorded in the NG-069 task evidence after publication.
**Related:** `SPEC-039-common-performance-measurement.md`

## 1. Problem and objective

`Specs/RESULTS-SPEC-039/` contains identical copies of raw Astra response streams under both `*.raw.sse` and session-local `*.last.sse` names. The duplicate files increase the public working-tree footprint without adding distinct bytes of evidence. This bounded change removes only the 11 unreferenced session-local copies listed below; it retains every canonical `*.raw.sse` byte-for-byte.

## 2. Scope and authority

The owner requested creation and implementation of this task on 2026-10-17, then explicitly authorised proceeding with publication. This authorises one normal commit/push of the exact allowlist and this SPEC to AIOPT5. It does not authorise public history rewriting, deletion of any other records, or changes to D2/D3 evidence.

Included: deletion of the 11 listed `*.last.sse` paths after rechecking exact byte equality and references; retain the matching `*.raw.sse` files and all other files.

Excluded: all other duplicate-content groups, including named fixtures and submitted packet copies; archival/migration; compression or rewriting of raw responses; history rewriting/force-push; unrelated pre-existing working-tree changes.

Exception retained: `Specs/RESULTS-SPEC-039/astra-session-v1.14-r12-20261013/ng039-v114-r12-documentary-review.last.sse` is byte-identical to its `.raw.sse` counterpart but is explicitly named by the immutable `raw_sse_path_in_session` field in `astra-review-v1.14-r12-20261013-submission.json`. Keep both paths; editing that submission receipt is outside this bounded cleanup.

## 3. Exact deletion allowlist

For each row, the retained raw file and deletion candidate must have the listed SHA-256 and byte count before deletion.

| Delete this exact path | Retain this exact path | SHA-256 | Bytes |
|---|---|---|---:|
| `Specs/RESULTS-SPEC-039/astra-session-r07-20261010/ng039-r07-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r07-20261010.raw.sse` | `adf9bca11b98f5188818ce013b03a9c778f406f5b9996a0fffc6a1a29d2725fa` | 310167 |
| `Specs/RESULTS-SPEC-039/astra-session-r07-b1b2-20261011/ng039-r07-b1b2-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r07-b1b2-20261011.raw.sse` | `e767364f1a0eaf475fe5f1fb9a2aa2c108d70f08a6c978cc83646d24b3fe1bef` | 157378 |
| `Specs/RESULTS-SPEC-039/astra-session-r08-followup-20261012/ng039-r08-followup-agent-reconciliation.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r08-followup-agent-reconciliation-20261012.raw.sse` | `fa3348e5a680811772bd5c472cc24feff55c2b080a09f8b6e50e451475ea084d` | 298206 |
| `Specs/RESULTS-SPEC-039/astra-session-r08-host-controlled-20261012/ng039-r08-host-controlled-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r08-host-controlled-20261012.raw.sse` | `7913242dff66e39015e81cbdc0a00c2c375a57927f0baad920902a3894dcabae` | 384640 |
| `Specs/RESULTS-SPEC-039/astra-session-v1.12-r09-20261012/ng039-v112-r09-b2b3-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.12-r09-20261012.raw.sse` | `93f78263662d085a45fde57ea4377c8420b381c958f556a0a80f6a70a089f80e` | 287144 |
| `Specs/RESULTS-SPEC-039/astra-session-v1.13-r10-20261012/ng039-v113-r10-finalizer-boundary-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.raw.sse` | `4c35170e7d37953daf9bbc7082e0e92cd73b6b45067521a151c3e7654d6f2704` | 242173 |
| `Specs/RESULTS-SPEC-039/astra-session-v1.14-r11-20261012/ng039-v114-r11-abnormal-finalization-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.raw.sse` | `5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb` | 310926 |
| `Specs/RESULTS-SPEC-039/astra-session-v1.15-r13-20261013/ng039-v115-r13-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.15-r13-20261013.raw.sse` | `b819bf078d2690fdfbb945e123873d66d469a72f819a3a4a38e3c6282b7a32b7` | 367282 |
| `Specs/RESULTS-SPEC-039/astra-session-v1.16-d2a-r15-20261013/ng039-v116-d2a-r15.last.sse` | `Specs/RESULTS-SPEC-039/astra-session-v1.16-d2a-r15-20261013/astra-review-v1.16-d2a-r15-20261013.raw.sse` | `f381eb4922b0d45322b9e38ae44278846c40c15b4f823b1471988ed715d5092e` | 829964 |
| `Specs/RESULTS-SPEC-039/astra-session-v1.16-r14-20261013/ng039-v116-r14-abnormal-journal-policy-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.16-r14-20261013.raw.sse` | `cd5a9a0c53f7e19b83986035500d7a3e4017a3564d21c10d39c02c80afd17093` | 279093 |
| `Specs/RESULTS-SPEC-039/methodology-review-20261013/ng039-methodology-plan-only-20261013.last.sse` | `Specs/RESULTS-SPEC-039/methodology-review-20261013/astra-methodology-plan-only-20261013.raw.sse` | `6b2e2367af5292b023391b7ff8351aa1eca3bbf8cac23a20a05663a65be34cb8` | 768846 |

## 4. Files affected

- Delete only the 11 paths in §3.
- Create this SPEC.
- Update the local Obsidian task `NG-069-deduplicar-registros-publicos-ng039-v1-20261017.md` and the Dashboard index.
- Do not alter the retained raw responses or other repository files.

## 5. Acceptance criteria

1. Before deletion, every listed pair exists, matches the exact size and SHA-256 in §3, and is byte-identical.
2. No pre-existing repository file or NG-039 task note references any deletion path. The path references in this SPEC §3 are the approved allowlist; retained raw paths remain referenced wherever provenance requires it.
3. Exactly the 11 allowlisted paths are deleted from `Specs/RESULTS-SPEC-039/`; the 11 retained raw files remain byte-identical. The r12 `.last.sse` remains because an immutable submission receipt references that exact path.
4. The results directory changes from 145 files / 18,164,472 bytes to 134 files / 13,928,653 bytes: a reduction of 11 files / 4,235,819 bytes.
5. `git diff --check` passes. No non-allowlisted pre-existing change is altered or staged.
6. Publish the exact allowlist and this SPEC using a normal fast-forward commit/push to AIOPT5; verify the remote commit SHA and affected paths. Do not rewrite history; state that prior copies remain accessible in Git history.

## 6. Verification and rollback

Use a read-only Python probe to verify the pair hashes, sizes, and byte equality immediately before removal. After removal, rerun the same allowlist/hash check, scan for path references, count the results directory, inspect `git diff --name-status`, and run `git diff --check`.

Rollback is limited to restoring a deleted duplicate from its retained byte-identical raw counterpart if an acceptance check fails. Do not use `git restore` on unrelated dirty files. Stop if any pair differs or a reference is discovered.

## 7. Review and publication gates

- Technical verification: local exact-SHA/hash/reference checks by the implementing agent, followed by owner read-back of the diff.
- Astra review: N/A; this is a mechanical deletion of exact byte duplicates, retains the original raw evidence, and makes no design/runtime claim.
- Publication: owner authorised normal commit/push on 2026-10-17; publish only the exact allowlist and this SPEC after rechecking the remote head. No force-push.
- History purge: explicitly out of scope; requires separate plan and authorisation.

## 8. Principle and invariants impact

Preserved: immutable original response bytes, review provenance, decisions, test fixtures, and fail-closed acceptance distinctions. No protocol, performance measurement, runtime, or architecture principle is changed. The removed session-local files contain no unique bytes and are not referenced by the repository or task note at the frozen baseline.
