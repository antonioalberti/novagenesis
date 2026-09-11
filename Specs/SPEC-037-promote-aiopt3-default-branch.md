# SPEC-037: Promote AIOPT3 to the Default NovaGenesis Branch

**Version:** v1.0
**Date:** 2026-09-11
**Author:** Antonio Alberti + Hermes Agent
**Status:** Implemented
**Branch:** AIOPT3
**Implementation commit:** `5ab91ce` baseline; final promotion result recorded in `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
**Related:** SPEC-036-deprecate-standalone-pss-girs-hts.md; Docs/DECISIONS/SPEC-STATUS-REGISTER.md

## 1. Problem / Objective

The NovaGenesis repository still advertises `master` as its remote default branch, while current development, integration, deployment scripts, SPECs and validated work use `AIOPT3`. The repository must make `AIOPT3` the principal/default branch and retain `master` as a secondary historical and compatibility branch.

The promotion is allowed to proceed with explicitly accepted imperfections. Acceptance is not an assertion that AIOPT3 is perfect; every accepted imperfection must be classified, evidenced, assigned a follow-up or re-evaluation condition, and excluded from the blocking set only when its impact is understood.

## 2. Current Baseline and Evidence

Observed in `/home/gandalf/workspace/novagenesis` on 2026-09-11:

| Item | Current state |
|---|---|
| Active integration branch | `AIOPT3` |
| `AIOPT3` / `origin/AIOPT3` | `b69a9ae` — `SPEC-037: define AIOPT3 default branch promotion` |
| `master` / `origin/master` | `e305782` — `Fix: add return 0; to GenerateSCNFromCharArrayBinaryPatterns functions to prevent SIGILL` |
| Remote HEAD | `origin/HEAD -> origin/master` |
| Divergence | `master...AIOPT3`: `0 261` — AIOPT3 is 261 commits ahead and has no commits absent from master according to the local graph |
| Tracked files | `master`: 584; `AIOPT3`: 638 |
| Working tree | AIOPT3 aligned with origin; three untracked `IO/` evidence directories are present and must not be included in branch administration changes |
| Deployment branch reference | `Scripts/AlpineVMs/pull-and-build-vms.sh` already uses `AIOPT3` |

The comparison is a branch-administration and documentation task, not a request to merge or rewrite either branch.

## 3. Preserved Principles and Invariants

This change must preserve:

1. **Reproducibility and traceability:** branch identity, commit baseline, accepted imperfections and rollback must be recorded so a deployment can be reproduced.
2. **Explicit contracts:** the default branch must point to the branch that current build/deployment instructions actually consume.
3. **Non-destructive history:** `master` remains readable and recoverable; no force-push, history rewrite or deletion is permitted.
4. **Evidence over assumption:** a successful local branch rename or symbolic-ref change is insufficient; the remote default branch and relevant consumers must be read back and verified.
5. **Scope separation:** this promotion must not silently alter the NovaGenesis protocol, inverted pub/sub model, payload-cache semantics or unrelated production code.

Deliberate concession: AIOPT3 may become the default while some non-critical defects, deferred tests or documentation drift remain open, provided they are in the accepted-imperfection register and do not violate the blocking criteria below.

## 4. Proposed Solution

### 4.1 Audit and classification

1. Compare `master` and `AIOPT3` by commit graph, tree, build, tests, SPECs, scripts, documentation and deployment consumers.
2. Search tracked files for `master`, `origin/master`, checkout/pull instructions and stale branch assumptions.
3. Classify findings as:
   - **Blocking:** unsafe default branch, unreproducible deployment, failed minimum validation, loss of history, or undisclosed contract-breaking change.
   - **Accepted with follow-up:** known issue with bounded impact, evidence and an owner/task or explicit re-evaluation condition.
   - **Accepted without immediate follow-up:** cosmetic or historical drift with no effect on build, deployment, protocol or user safety.
   - **Resolved before promotion:** corrected and verified as part of this SPEC.

### 4.2 Branch policy

- `AIOPT3` becomes the remote default branch and principal integration branch.
- `master` remains a secondary branch and is not deleted, rewritten or force-updated.
- New development and deployment instructions target `AIOPT3`.
- Historical documents may retain `master` references when they describe the historical baseline; active instructions must identify the intended branch explicitly.
- `origin/HEAD` is refreshed locally after the remote default change, but this local symbolic ref is not treated as proof of the remote setting.

### 4.3 Verification

After the remote change:

1. Read the remote default branch through the available repository interface and verify it is `AIOPT3`.
2. Verify `master` still exists at the pre-change commit and remains fetchable.
3. Clone or inspect a clean temporary checkout using the default branch and verify it resolves to AIOPT3.
4. Run the minimum build/spec validation selected in the acceptance matrix.
5. Verify Alpine deployment scripts and active documentation refer to AIOPT3.
6. Record all accepted imperfections and open follow-ups in the task and evidence packet.

## 5. Files and External State Affected

| Target | Change | Reason |
|---|---|---|
| Repository remote settings | Set default branch to `AIOPT3` | Make the active integration branch principal |
| `origin/HEAD` in local clones | Refresh symbolic default after remote change | Prevent stale local discovery |
| Active scripts/documentation | Replace stale `master` assumptions where operational | Align users and deployments with the new contract |
| Historical documents | Preserve historical `master` references, optionally annotate context | Do not rewrite evidence |
| `master` branch | No deletion, rewrite or force-push | Preserve rollback and historical compatibility |
| Untracked `IO/` evidence | No modification or inclusion | Protect experimental evidence from branch administration |

## 6. Acceptance Criteria

1. A comparison report identifies the exact commits, graph relationship, tree differences and relevant operational differences between `master` and `AIOPT3`.
2. The accepted-imperfection register exists and each item has classification, impact, evidence, owner/follow-up or re-evaluation condition.
3. No blocking item remains undisclosed at promotion time.
4. The repository remote reports `AIOPT3` as its default branch.
5. `master` remains present and unchanged at its pre-promotion commit unless a separately approved change is made later.
6. A clean default-branch checkout resolves to AIOPT3 and can run the selected minimum validation.
7. Active build/deployment instructions use AIOPT3; historical references are explicitly historical.
8. `Scripts/Simple/validate-specs.sh` and the selected build/test checks pass, or failures are listed as accepted imperfections with evidence and a follow-up.
9. No source, protocol, pub/sub, cache or runtime behaviour is changed solely to perform this branch promotion.
10. Rollback is documented as a remote default-branch change back to `master`; no force-push is required.

## 7. Minimum Validation Matrix

| Check | Required result | Failure treatment |
|---|---|---|
| Git graph/tree comparison | Complete report | Blocking if incomplete |
| Active branch-reference scan | All operational references classified | Blocking if a deployment consumer is unknown |
| SPEC validation | PASS, or documented pre-existing failures | Accepted only with explicit evidence and impact |
| Clean AIOPT3 checkout | Default checkout resolves to AIOPT3 | Blocking |
| Build baseline | Selected build completes or known failure is classified | Blocking for unexplained build failure |
| Runtime/integration tests | Existing accepted baseline reused where valid | Do not claim new coverage; record deferred tests |
| Remote branch read-back | Default is AIOPT3; master still exists | Blocking |
| Rollback rehearsal/design review | Reversible without history rewrite | Blocking |

## 8. Rollback Plan

If the promoted default causes an undisclosed critical problem:

1. Restore the remote default branch setting to `master` through the repository administration interface.
2. Refresh `origin/HEAD` in affected local clones.
3. Do not rewrite, delete or force-push either branch.
4. Preserve the evidence and update this SPEC/task with the failure and decision.
5. Reopen the blocking imperfection as a separate reviewed task or amend this SPEC before retrying.

A rollback changes repository metadata only; it does not revert commits on either branch.

## 9. Known Risks and Deliberate Imperfections

The following are candidates for classification, not automatic acceptance:

- AIOPT3 contains deferred validation gates, including the normal cross-process runtime validation recorded by NG-036.
- The SPEC register and historical documentation contain legacy/stale references that may require classification rather than wholesale rewriting.
- `master` has a smaller/older tree and is not a current deployment baseline.
- The working tree contains untracked IO evidence directories that are outside this change.
- Remote branch protection/default-branch settings may require administrative access not available from the local checkout.

No candidate is accepted until its impact and evidence are recorded in the NG-037 task.

## 10. Decisions Log

| # | Decision | Date | Reason |
|---|---|---|---|
| D1 | Promote AIOPT3 rather than merge it into master | 2026-09-11 | AIOPT3 is the active integration/deployment branch and is 261 commits ahead of master |
| D2 | Retain master as a secondary branch | 2026-09-11 | Preserve history, compatibility and rollback |
| D3 | Permit bounded, explicit imperfections | 2026-09-11 | Avoid blocking the branch promotion on unrelated or non-critical unfinished work |
| D4 | No source/protocol change in this SPEC | 2026-09-11 | Keep branch administration separate from production implementation |
| D5 | Promote AIOPT3 after corrected 100-photo evidence and remote read-back | 2026-09-11 | Runtime and administration gates passed; master remains preserved |

## 11. Implementation Notes

- All GitHub-facing text must be in English.
- Use a normal commit on `AIOPT3` for this SPEC and related repository documentation; do not force-push.
- The task note `NG-037-promocao-aiopt3-default-v1-20260911.md` is the Obsidian source of truth for the accepted-imperfection register and operational state.
- A remote default-branch change is an external state change and must be read back before claiming completion.
