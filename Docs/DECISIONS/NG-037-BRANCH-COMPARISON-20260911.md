# NG-037 Branch Comparison Report

**Date:** 2026-09-11
**Repository:** `antonioalberti/novagenesis`
**Working tree:** `<local-repository-path>`
**Scope:** Compare `master` and `AIOPT3` before changing the remote default branch.

## 1. Branch and remote state

| Item | Observed value |
|---|---|
| Local active branch | `AIOPT3` |
| `AIOPT3` / `origin/AIOPT3` | `b69a9ae` |
| `master` / `origin/master` | `e305782` |
| Remote default branch | `master` |
| Local `origin/HEAD` | `origin/master` |
| Graph divergence (`master...AIOPT3`) | `0 261` |
| Tracked files | `master`: 584; `AIOPT3`: 638 |
| Diff size | 698 paths; 60,678 additions; 49,315 deletions |

The graph shows that all commits reachable from `master` are also reachable from `AIOPT3`; `AIOPT3` is 261 commits ahead. Promotion therefore does not require merging `master` into `AIOPT3`.

The remote API was read directly and reported `default_branch: master`. No remote branch setting was changed during this audit.

## 2. Relevant tree differences

The difference is substantive, not a naming-only branch change. AIOPT3 contains the current NovaGenesis implementation and its associated evidence/documentation, including:

- current NGAL, GW, Message, Process and application changes;
- current NRNCS cache-model and delivery documentation;
- SPEC/results/decision records for the active development line;
- normal/legacy runtime-profile work from SPEC-036;
- current Alpine deployment and validation scripts.

AIOPT3 also removes or changes historical/legacy material, including the standalone EPGS implementation and related launch/Docker integration, as recorded by NG-036. These differences must remain part of the accepted AIOPT3 baseline; the branch promotion itself must not attempt to reconcile them with `master`.

## 3. Operational reference scan

The tracked AIOPT3 tree was searched outside `Docs/HISTORICAL` and the new SPEC for operational references to `origin/master`, pulling/checking out `master`, or `BRANCH=master`.

Result: no active operational references were found by this scan.

The Alpine deployment script already declares:

```text
BRANCH=AIOPT3
```

Active SPECs and decision records consistently describe AIOPT3 as the development/integration branch. Remaining `master` references are historical, comparative, or part of this promotion's audit/rollback documentation and must not be blindly rewritten.

## 4. Validation state

`Scripts/Simple/validate-specs.sh` was executed on AIOPT3.

Result: 2 checks passed and 9 checks failed. The failures are pre-existing repository hygiene findings, not introduced by SPEC-037:

- stale AIOPT2 references in `PLAN-CONSOLIDATION-2026-07-16.md`;
- stale SPEC-015/016 references in that historical consolidation plan;
- four non-canonical status strings in existing SPEC files;
- duplicate SPEC number families for SPEC-001, SPEC-022 and SPEC-033.

The branch-field consistency check passed, including the new SPEC-037. Related-SPEC resolution also passed.

No build or runtime test was claimed by this report. Existing validated AIOPT3 evidence remains the applicable baseline and is referenced by NG-036, NG-020 and NG-022.

## 5. Initial imperfection register

| ID | Finding | Classification | Impact / condition |
|---|---|---|---|
| I1 | Existing SPEC validation failures listed above | Accepted with follow-up | Do not block branch promotion if recorded as pre-existing; resolve through a separate documentation/SPEC hygiene task |
| I2 | NG-036 normal cross-process runtime validation remains open | Candidate blocker until reviewed | Must not be hidden; promotion requires explicit decision that the current AIOPT3 runtime baseline is safe enough, or it remains blocking |
| I3 | Some historical documents mention `master` or AIOPT2 | Accepted without immediate follow-up when clearly historical | Active operational instructions must not depend on them |
| I4 | `master` is 261 commits behind and has 54 fewer tracked files | Accepted as secondary-branch divergence | `master` remains rollback/history only; no claim of parity is made |
| I5 | Three untracked IO evidence directories exist locally | Excluded from branch promotion | Preserve unchanged; do not stage or delete them |
| I6 | Remote default-branch administration is external to the local checkout | Blocking until access/approval is available | Verify the remote setting after the change; local `origin/HEAD` is not sufficient |

I2 and I6 remain open gates. I1, I3, I4 and I5 can be accepted only with the conditions shown above.

## 6. Recommendation

Proceed with the promotion only after:

1. the owner explicitly accepts or rejects I2 as a blocking gate;
2. administrative access to change the remote default branch is confirmed;
3. the comparison report and imperfection register are committed on AIOPT3;
4. a clean default-branch checkout and remote read-back are performed after the change.

No merge, force-push, branch deletion or source-code change is required for the promotion.
