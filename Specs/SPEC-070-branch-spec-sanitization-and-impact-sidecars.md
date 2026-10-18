# SPEC-070: Branch-Specific SPEC Validity and Affected-File Records

**Version:** v1.0
**Date:** 2026-10-17
**Author:** Hermes Agent, at the owner's request
**Status:** In Progress
**Branch:** AIOPT5
**Baseline commit:** `8d2386dcb10d2b37f133d4b24c2cb96761753d82`
**Implementation commit:** —
**Related:** `SPEC-038-public-repository-hygiene.md`, `SPEC-069-ng039-public-evidence-deduplication.md`
**Owner task:** `NG-070-saneamento-specs-branches-v1-20261017.md`

## 1. Objective

Create a reproducible, conservative audit of NovaGenesis SPEC applicability across pinned AIOPT3, AIOPT4, and AIOPT5 snapshots, and provide one provenance-aware affected-file sidecar for every unique SPEC filename in those snapshots and any local-only SPEC. Preserve all original SPECs and Git history. Present proposed sanitation of original SPECs for owner review before applying it.

## 2. Branch context and pinned inputs

The audit records each branch's immutable commit and treats branch role as context, not proof of a SPEC's applicability:

| Branch | Pinned commit | Role used for classification |
|---|---|---|
| AIOPT3 | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | Historical pre-release improvement line |
| AIOPT4 | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | Archive of the paused/failed Release 1.0.0 attempt |
| AIOPT5 | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | Active line for executable C/C++ improvements |

If any branch ref moves before a required regeneration or review, freeze the new exact SHA and invalidate generated records tied to the prior snapshot; do not mix findings from different snapshots.

## 3. Classification contract

For each SPEC in each branch snapshot, classify its documentary authority as one of:

- `Current for this branch`: the SPEC explicitly governs current work on that branch and its gates/status are consistent with the pinned evidence.
- `Historical`: describes a completed or past baseline/event; preserve as history and make clear it is not an active instruction.
- `Superseded`: replaced by an identified SPEC or decision; preserve the supersession link.
- `Branch-specific`: valid only for the named branch/snapshot or workflow; do not apply its instructions to another branch by implication.
- `Indeterminate`: available evidence does not establish applicability; do not infer current authority from filename, status label, or code presence alone.

A dated problem statement is not automatically stale when it is clearly historical. A proposal's intended files are not proof of implementation. A historical commit or status label is not proof that its acceptance gates still hold at a later branch head. Record concrete contradictions and stale operational instructions separately from branch-local historical context.

## 4. Affected-file sidecar format

Create one file per unique SPEC basename in `Specs/IMPACT/` using `<SPEC-basename>.affected-files.md`. Include local-only SPEC files found in the working tree, clearly marked as not present in pinned branch snapshots. Each sidecar records:

1. Presence, version, status, baseline, and implementation commit as stated by each pinned SPEC snapshot.
2. Literal file/path strings in explicit file-, scope-, or allowlist-related sections, tagged as declared/proposed scope rather than proven changes.
3. Changed paths from Git commits that either modified that exact SPEC file or explicitly named its SPEC identifier; record commit SHA, subject, changed paths, and linkage basis.
4. Ambiguous duplicate-ID commits as unattributed; do not assign generic `SPEC-NNN` commits to a variant without exact evidence.
5. Evidence limitations: tree presence, source mentions, declared scope, and commit co-change do not independently prove causation, implementation acceptance, or completeness.

The sidecars are an evidence index, not a replacement for the SPEC, an acceptance record, or implementation authorization. If no reliable path evidence exists, state the gap rather than inventing an affected-file list.

## 5. Files affected and generation procedure

- Store the reusable generator at `Scripts/Python/generate_spec_impact_sidecars.py`.
- Default operation is dry-run; `--write` creates files only after checking all destination names for collisions.
- The generator reads pinned Git objects and records exact SHAs. Use repeatable `--ref NAME=REV` arguments to select later branch snapshots; never silently use a moved branch ref as though it were the original baseline.
- Keep any local-only SPEC content distinct from pinned branch content and record its worktree state and SHA-256.
- Generate one sidecar for each unique SPEC filename in the pinned branch union plus each local-only SPEC, then verify exact set equality, file count, paths, SHA pins, and whitespace.
- Do not overwrite existing sidecars. A materially new audit snapshot requires an explicit new output set/review rather than silent replacement.

## 6. Scope and authority

### Included

- Read-only comparison of SPEC inventories and relevant SPEC/branch claims at the pinned heads.
- Identification of stale statuses, missing cross-references, conflicting baselines, and branch-misapplied operational instructions, with source anchors.
- Creation and validation of the requested affected-file sidecars and the reusable local generator.
- A proposed change list for original SPEC files and the master status register, presented to the owner before application.

### Excluded

- Editing, deleting, moving, renaming, or reclassifying original SPECs or the master status register before owner review and approval of the exact proposed changes.
- Git branch switching, staging, committing, pushing, force-pushing, or history rewriting.
- Code/runtime changes to NovaGenesis, builds, VM operations, release actions, or Astra submissions.
- Treating sidecar generation, commit-message matching, or file presence as proof that an implementation passed its acceptance criteria.

## 7. Validation and review gates

- Verify each pinned SHA against its local Git object and count SPEC files programmatically per branch and across the unique filename union.
- Verify every sidecar basename maps to exactly one source SPEC basename, and that no source SPEC in scope is omitted or duplicated.
- Check each sidecar's branch presence matrix and generated source path; separately identify local-only inputs.
- Check Markdown tables, file/path extraction, duplicate SPEC-ID handling, generated whitespace, and the literal dry-run/write command behavior.
- Provide independent read-only branch audits for AIOPT3, AIOPT4, and AIOPT5. Discard any audit tied to a prior AIOPT5 SHA after that ref moves; rerun on the new exact SHA.
- Astra review: N/A for this bounded documentary inventory and local evidence-index generator; no architecture, runtime, or acceptance claim is being submitted.
- Owner approval is required before applying any proposed change to an original SPEC or the master register. No commit or push is authorized by this SPEC.

## 8. Acceptance criteria

- [x] Exact AIOPT3/AIOPT4/AIOPT5 baseline SHAs are recorded and still match the reviewed snapshots.
- [x] SPEC inventories reconcile to the pinned trees; all unique source basenames, including local-only SPECs, have exactly one sidecar (64 verified against 58/60/48 branch counts plus local-only SPEC-070).
- [x] Each sidecar distinguishes declared paths from Git co-change/message evidence and states limitations; duplicate-ID commits remain unassigned unless exact.
- [x] Branch-specific classifications and sanitation proposals have source anchors and do not erase historical context or imply implementation acceptance.
- [x] Generator passes syntax/help/dry-run and its bounded write path; the generated sidecar set passes count/name/content checks.
- [x] Proposed original-SPEC/register edits were presented to the owner and applied only after explicit scope approval; no commit or push was made.

## 9. Rollback and preservation

Do not modify existing source SPECs, register content, or history as part of generation. If an output set fails validation, preserve the generated files for review, report the exact failures, and do not overwrite or silently regenerate them. Correct the generator and produce a distinct reviewed output set only after invalidating the prior records explicitly.

## 10. Decisions

| Date | Decision | Reason |
|---|---|---|
| 2026-10-17 | Use conservative classification and preserve original SPECs/history | Owner selected the conservative approach and requested sidecars for every SPEC |
| 2026-10-17 | Store reusable generator under repository `Scripts/` | Owner explicitly directed future tools be kept there |
| 2026-10-17 | Apply the approved minimal documentary corrections only in the specified branch snapshots | Owner approved cross-branch application; preserve history and unrelated work |

Approved allowlist (no commit/push): AIOPT3, AIOPT4, and AIOPT5 `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`, `Specs/SPEC-031-recvmmsg-batched-receive.md`, and `Specs/SPEC-037-promote-aiopt3-default-branch.md`; AIOPT3/AIOPT4 `Specs/SPEC-054-release-master-plan-and-audit.md`; AIOPT5 `Specs/SPEC-069-ng039-public-evidence-deduplication.md`, `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`, and `Specs/SPEC-045-core-evaluate-functional-harness.md`. No other file or branch is authorized by this decision.
