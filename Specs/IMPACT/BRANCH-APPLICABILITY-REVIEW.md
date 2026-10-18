# Branch Applicability Review — AIOPT3, AIOPT4, AIOPT5

**Review history:** Sections 2–3 record the initial findings and proposed edits before owner approval. Section 4 records the later owner-approved local edits, which remain uncommitted; no branch refs/history were changed. Inventory and 64-sidecar counts are historical and remain pinned to the SHAs listed below.
**SPEC-071 review chronology:** The exact v0.12 design packet returned `AMEND`; the current working draft is v0.13-draft and remains unreviewed. This is separate from the NG-070 branch inventories, which remain historical at the pinned SHAs.
**Governing task:** `NG-070-saneamento-specs-branches-v1-20261017.md`
**Governing SPEC:** `SPEC-070-branch-spec-sanitization-and-impact-sidecars.md`
**Snapshot date:** 2026-10-17

## 1. Pinned snapshots and inventory

| Branch | Exact HEAD | SPEC files | Branch-field distribution | Project role used for this review |
|---|---|---:|---|---|
| AIOPT3 | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | 58 | 39 `Branch: AIOPT3`; 19 no branch field | Historical pre-release improvement line |
| AIOPT4 | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | 60 | 41 `Branch: AIOPT3`; 19 no branch field; zero `Branch: AIOPT4` | Archive of the paused/failed Release 1.0.0 attempt |
| AIOPT5 | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | 48 | 25 `Branch: AIOPT3`; 3 `Branch: AIOPT5`; 20 no branch field | Active executable C/C++ improvement line |

Across the pinned trees there are 63 unique SPEC basenames: 45 appear in all three; 13 appear in AIOPT3 and AIOPT4 only; 2 are AIOPT4-only; 3 are AIOPT5-only. `SPEC-070` is local-only, so the generated set contains 64 per-SPEC sidecars.

Interpret `Branch:` as the document's stated branch context/origin, not proof it governs the branch where a copy happens to appear. Therefore, the 41 AIOPT3-labelled documents in AIOPT4 are archived AIOPT3-origin records, not AIOPT4 authority. Likewise, the 25 AIOPT3-labelled documents in AIOPT5 are not automatically AIOPT5 work instructions. A missing branch field is indeterminate, not permission to infer the branch.

## 2. Proposed sanitation — concrete findings

### A. Master SPEC register

`Docs/DECISIONS/SPEC-STATUS-REGISTER.md` says it is based on `cd804a7` / AIOPT3, dated 2026-07-16, and lists 24 SPECs. The pinned trees now contain 58/60/48 files. Proposed: preserve the old table as a dated historical snapshot, clearly withdraw its claim to be the current master inventory, and add a compact branch-aware snapshot section linking to `Specs/IMPACT/` and the exact SHAs above. Do not bulk-rewrite historical commit or branch references.

### B. SPEC-031 implementation/status drift — all three branch snapshots

`SPEC-031-recvmmsg-batched-receive.md` still says `Draft — approval required before implementation` and describes one `recvfrom()` per ready socket as the current path, then proposes replacing it with `recvmmsg()`. Commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` is an ancestor of all three pinned heads, changed `PGCS/src/NGAL_Transport_RAW.cpp`, `Common/src/NGAL_SAR.h`, and `PGCS/src/PG.cpp`, and its commit message identifies the SPEC-031 implementation. The current `ReceiveDispatcher` source at each pinned head calls `recvmmsg()`.

Proposed same-SPEC correction: retain the historical baseline/proposal context, record the implementation commit, and update status to distinguish implemented code from any still-pending verification/benchmark gates. Do not claim performance acceptance unless the SPEC's benchmark criteria and evidence are separately verified.

### C. SPEC-069 post-deletion verification wording — AIOPT5

The committed `SPEC-069-ng039-public-evidence-deduplication.md` §6 says to rerun the same pair/hash check after deletion, but that check requires the deleted `.last.sse` files to exist. The exact AIOPT5 commit `8d2386dcb10d2b37f133d4b24c2cb96761753d82` deletes 11 `.last.sse` files and adds the SPEC.

Proposed same-SPEC correction: keep the before-delete check for existence, size, SHA-256 and byte equality; after deletion, verify retained `.raw.sse` hashes, deleted-path absence, reference absence, file/byte counts and the exact diff. Preserve the r12 exception. This is a documentary follow-up; do not rewrite history or claim the post-delete checks were independently repeated unless evidence confirms them.

### D. Unresolved SPEC-040 / SPEC-041 references — AIOPT5

No SPEC-040 or SPEC-041 file exists in any of the three pinned trees, and neither filename appears in reachable Git history. In AIOPT5, `SPEC-044-core-run-evaluate-invalid-pg-downcast.md` still lists both in `Related:` and refers to SPEC-041 in its body; `SPEC-045-core-evaluate-functional-harness.md` lists SPEC-041 in `Related:`. `SPEC-039` calls SPEC-041 archived, which is explicitly historical context rather than a live dependency.

Proposed correction: remove or explicitly mark the unresolved `Related:` links in SPEC-044/045 as historical references to absent documents; do not invent replacement filenames. Retain historical explanatory text only where it is needed to explain the diagnostic origin.

### E. Historical branch/remote statements — clarify, do not refresh by assumption

- `SPEC-037-promote-aiopt3-default-branch.md` is an Implemented AIOPT3 branch-policy event. Its “still advertises master” sentence is the pre-promotion problem statement, and its remote-ref table is dated 2026-09-11. Proposed: label that paragraph explicitly as the pre-promotion snapshot; do not replace it with an unverified current remote claim.
- `SPEC-054-release-master-plan-and-audit.md` identifies AIOPT3 as its branch and says AIOPT4 is out of scope. When viewed from AIOPT4/AIOPT5, treat it as AIOPT3-origin release history. Its remote deletion/stale-local-ref statements are time-bound claims; do not refresh them without a new read-back.

### F. Explicitly preserve, rather than “sanitize” into false authority

- `SPEC-039` §14 and §70 explicitly state that the v1.18 §9 is current authority and older §§4–7 recipes are historical/non-executable. Its D2 gate remains HOLD; D2 implementation/runtime and D3 are not authorized. No edit is proposed from the older-recipe finding alone.
- `SPEC-062` is AIOPT5-scoped but remains Draft; the owner task says runtime is blocked. Do not treat presence in AIOPT5 as approval.
- `SPEC-027` has no `Branch:` header but records an AIOPT5 reviewed snapshot; Phase 0/1 remain partial/open and the associated work is paused. Do not treat its In Progress status as execution authorization.
- Keep duplicate IDs (`SPEC-001`, `SPEC-022`, and the SPEC-033 family) and historical files intact. Use full filenames in cross-references; do not renumber or delete them as a cleanup shortcut.

## 3. Sidecars and generator

- Sidecar directory: `Specs/IMPACT/`
- Count verified: 64 unique source basenames, matching the three pinned branch trees plus local-only SPEC-070.
- Each sidecar distinguishes literal paths declared in scope from paths in commits that co-modified the SPEC or explicitly named its identifier. Missing linkage is labelled as an evidence gap, not “no files affected.”
- Reusable tool: `Scripts/Python/generate_spec_impact_sidecars.py`
- Current pinned run: `python3 Scripts/Python/generate_spec_impact_sidecars.py --write`
- Tool validation: `py_compile`, `--help`, dry-run, write path, sidecar filename/count/pin/structure checks and whitespace checks passed. The tool refuses to overwrite sidecars unless they are generated records with the same pins.

## 4. Owner-approved edits applied locally

The owner approved the minimal correction set on 2026-10-17. Changes are uncommitted in branch worktrees; branch refs, history and unrelated work remain untouched.

- AIOPT3 worktree `/home/gandalf/workspace/novagenesis-ng070-aiopt3`: `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`, `Specs/SPEC-031-recvmmsg-batched-receive.md`, `Specs/SPEC-037-promote-aiopt3-default-branch.md`, `Specs/SPEC-054-release-master-plan-and-audit.md`.
- AIOPT4 worktree `/home/gandalf/workspace/novagenesis-ng070-aiopt4`: the same four files.
- AIOPT5 current worktree: `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`, `Specs/SPEC-031-recvmmsg-batched-receive.md`, `Specs/SPEC-037-promote-aiopt3-default-branch.md`, `Specs/SPEC-069-ng039-public-evidence-deduplication.md`, `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`, and `Specs/SPEC-045-core-evaluate-functional-harness.md`.

The 64 sidecars remain explicitly pinned to the pre-edit branch SHAs and record those source snapshots; they do not claim to represent the new uncommitted text. Regenerate against new committed SHAs after any separately authorized commit. No commit/push was requested or made.

## 5. Verification state

- The three pinned branch heads remain unchanged.
- `git diff --check` passed for the authorized SPEC/register edits in all three worktrees.
- No build, runtime, VM, Astra, commit, push, or history rewrite was performed.
- Re-review the exact diffs in the three worktrees before any commit; none is authorized here.
