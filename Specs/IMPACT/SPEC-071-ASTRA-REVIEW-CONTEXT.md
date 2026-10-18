# SPEC-071 — GPT-6 Astra Review Context

Status: the design-only GPT-6 Astra review of v0.12 returned `AMEND` on packet SHA `439bab94579ad33c174afde2612031c9d0f1afbc593ebb903524c38ec1e246b7`. The current v0.13 draft and Draft 2020-12 report schema passed local contract-fixture validation; the registered handler's `claims` route conflicts with the separate strict profile query schema, and remote dispatch remains blocked pending owner resolution. Receipt: `Specs/IMPACT/SPEC-071-v0.13-design-validation-receipt.md`. The owner requested a v0.13 follow-up, but it has not been frozen or sent; fresh exact-SHA consent is pending. No auditor code/tests/runtime exist, and post-implementation Astra validation remains a later, separate gate.

This file preserves detailed context for the eventual final review. It records the earlier design review as history, not as approval of later SPEC or code revisions.

## 1. Review objective

Current draft after AMEND closure: SPEC-071 v0.13-draft. The previous v0.12 design-only review returned `AMEND`; v0.13 has not been reviewed by Astra and is not approved for implementation. Before code, obtain owner approval and a fresh exact-packet Astra follow-up; after implementation/tests, perform the separate final tool validation.

A separate mandatory post-implementation review must later validate the completed code and tests against a new exact-SHA packet.

## 2. Stage-specific packet contents

### Previously submitted design-only packet (historical v0.12)

The exact v0.12-draft SPEC and context snapshot submitted under packet SHA `439bab94579ad33c174afde2612031c9d0f1afbc593ebb903524c38ec1e246b7` was returned `AMEND`. This is historical evidence only; do not treat it as reviewing or approving v0.13.

### v0.13 pre-implementation design follow-up packet

At the v0.13 design follow-up gate and before code, freeze a new exact-SHA packet (not the v0.12 packet) containing the complete current SPEC-071 v0.13 and its Draft 2020-12 report schema, the AMEND closure map, a pinned schema-validation/contract-test receipt, the relevant NG-070 report/task, the exact original Git-object excerpts in `Specs/IMPACT/SPEC-071-v0.13-git-object-evidence.md` (SHA-256 `cbe68d76656dd6a574c42a9cb7d821ac3bdbfc1dfe85ca972f609868b723d5df`), branch-specific motivating SPEC/sidecar evidence, current AIOPT3/AIOPT4/AIOPT5 roots/HEADs/worktree-state digests, and active Hermes contracts (`decision_query` JSON Schema, `validate_query`, TypeSafe Choice serializer/response validator, `assess_progress`, plugin registration/pre-tool hooks and policy), each with exact path/hash/line ranges. Include the prior v0.12 packet/AMEND identities and full derived response, but do not imply the old verdict covers v0.13. State that auditor source/tests/runtime are absent. Omit unrelated Dashboard content and secrets; record all omissions and hashes. Obtain fresh exact-packet consent; request a design-readiness `APPROVE`/`AMEND`/`NO-GO` only and require the reviewer to explicitly return `implementation authorization: NO`. Owner approval of the resulting SPEC and separate implementation authorization remain later gates.

### Later implementation-validation packet

At minimum, embed the exact bytes of:

- Final SPEC-071 (owner-approved revision).
- `Specs/SPEC-070-branch-spec-sanitization-and-impact-sidecars.md` and `Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md` as governing background, with the relevant sidecars.
- `Specs/SPEC-031-recvmmsg-batched-receive.md`, `Specs/SPEC-069-ng039-public-evidence-deduplication.md`, `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`, and `Specs/SPEC-045-core-evaluate-functional-harness.md` or the exact final branch variants that supply the motivating examples.
- NG-071 task note and the active Dashboard entry.
- Final source, tests, schemas, fixtures, documented CLI contract, full diff, generated consolidated report and per-SPEC sample reports.
- Test/validator commands with full outputs and exit codes, actual test/fixture inventory, known limitations, and sanitized `decision_query` handoff/receipt if Jev is exercised.

Do not submit only file paths or excerpts while implying the full artifacts were reviewed. Use `===== FILE: path =====` delimiters and a SHA-256 manifest. At either review gate, verify the exact commit HEAD, branch, dirty/untracked state, included/excluded files, and packet digest immediately before dispatch. The design-only packet must not imply implementation validation.

## 3. Frozen design decisions from the owner

1. Scan AIOPT3/AIOPT4/AIOPT5 separately; bind all findings to each branch's exact pinned HEAD.
2. Build a local CLI evidence bundle (JSON/Markdown). Hermes invokes the registered `decision_query` for selected claims; the script does not make direct raw HTTP requests.
3. Egress is limited to sanitized claims, status metadata, hashes, and short evidence summaries. Exclude source code, raw logs, PII, secrets, and protected evidence.
4. Report gate/status states and counts. Emit a progress percentage only when the relevant SPEC defines criteria and weights; Jev confidence is never progress.
5. V1 covers five drift classes: status/implementation-commit mismatch; current-path claim vs source; dangling/ambiguous references and branch applicability; declared affected paths vs commit changes; acceptance/test evidence vs current branch. Deterministic checks run first; semantic/evidence findings that are not mechanically established are review-needed.
6. Produce both a consolidated summary and per-SPEC findings, keeping branch snapshots separate.
7. Print a concise summary to stdout and perform no writes by default. Persist summary and per-SPEC reports only when an explicit `--out-dir` is supplied.
8. Use `/home/gandalf/workspace/novagenesis/Scripts/` as the actual project location in this environment.
9. After implementation and tests, obtain a GPT-6 Astra validation against the exact frozen packet before declaring the tool validated. Astra advice does not grant implementation, deployment, or release authority; the owner decides acceptance.
10. The v0.12 design-only Astra review returned `AMEND`; v0.13 records the response, but no follow-up review has been sent and no implementation authorization exists.
11. Before code, owner approval and a fresh exact-SHA Astra `APPROVE` on v0.13 are required; after implementation/tests, perform a separate final tool review.

## 4. NG-070 evidence that motivated this tool

These are historical audit observations; refresh the source files and branch evidence at the final review SHA before claiming they remain current.

- SPEC-031: the reviewed headers described Draft/`recvfrom()` while commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` was an ancestor of the three NG-070 pinned branch heads and the implementation used `recvmmsg()`. Performance acceptance remained unverified. The future detector must distinguish status/source drift from performance acceptance.
- SPEC-069: a post-deletion instruction still required a pair-equality check whose candidate `.last.sse` inputs had just been deleted. The future detector should detect contradictory procedure/evidence ordering without claiming semantic certainty from a string match alone.
- SPEC-044/045: `Related:` entries referenced SPEC-040/041, absent from the pinned branch trees and reachable local history checked by NG-070. The future detector must preserve exact branch scope and distinguish missing references from historical references.
- `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`: its 24-SPEC figure was a dated snapshot at `cd804a7` (2026-07-16), not a current cross-branch inventory; NG-070 found 58 AIOPT3, 60 AIOPT4, and 48 AIOPT5 SPEC files at its pinned snapshots.
- AIOPT5 copies often carried `Branch: AIOPT3`; branch origin/header alone did not prove current AIOPT5 authority. The tool must not equate file presence or metadata with applicability.
- The requested progress chart was correctly reported as status distribution, not implementation completion: there is no common per-SPEC percentage field or denominator.

## 5. Historical snapshot references — refresh before Astra

NG-070 froze these audit inputs. They are reference-only and MUST NOT be reused as the final tool-review identity without checking the current repositories and actual tool scan inputs:

- AIOPT3: `24a3851e1d5a90f039b9e148a1af38dd03c4d925` (58 SPECs).
- AIOPT4: `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` (60 SPECs).
- AIOPT5: `8d2386dcb10d2b37f133d4b24c2cb96761753d82` (48 pinned SPECs at that point; local-only SPEC-070 and later SPEC-071 are separate working-tree documents).

The NG-070 audit union was 63 unique branch-pinned names; it later had local-only SPEC-070 and SPEC-071 sidecars. These counts describe those historical audit snapshots, not a current inventory.

## 6. Invariants for Astra to challenge against actual code/evidence

- No false clean result: missing/unpinned/ambiguous inputs must be `INDETERMINATE` or review-needed, never `NO_DRIFT_DETECTED`.
- Findings cite SPEC path/section, branch, exact HEAD, evidence paths/commit IDs, rule ID, and classification rationale.
- Branch evidence cannot cross-contaminate; dirty/untracked working-tree state is separate from committed tree evidence.
- Deterministic checks and `decision_query` advice remain separate in storage and output; Jev cannot promote a gate to PASS, compute progress, or authorize edits.
- JEV/TypeSafe route is off unless deliberately invoked through the registered Hermes `decision_query` path and applicable exact-scope approval/cost gates.
- Offline/local scan remains usable; malformed Jev output, timeout, or unavailable Jev cannot create false PASS or trigger silent backend fallback.
- Default run is read-only and stdout-only; `--out-dir` is required to persist reports. No branch switch, build/test, source edits, stage/commit/push, VM/runtime operation, or credential reads.
- Report status categories must not imply implementation acceptance. Percentages are emitted only from explicit SPEC criteria/weights and local evidence.
- No secrets, PII, raw logs, or protected evidence in the final external review packet. Record omissions/redactions and constrain the review claim accordingly.

## 7. Astra review questions

### v0.13 pre-implementation design follow-up

Ask GPT-6 Astra for `APPROVE`, `AMEND`, or `NO-GO` on design readiness of the exact v0.13 packet only. Require numbered findings, exact file/section/line evidence, a clear evidence boundary, and an explicit returned field `implementation authorization: NO`; if the field is missing, malformed, or ambiguous, treat it as absent and keep implementation blocked. Astra does not grant code authorization. Astra must not claim to validate auditor code, tests, runtime, or generated reports, because none exists. This call requires fresh packet-specific owner consent; the prior v0.12 consent is consumed.

1. Do v0.13 clauses and the closure map resolve each v0.12 must-fix blocker without weakening the owner-selected boundaries?
2. Does the rule/evidence matrix prevent false clean results across all five drift classes and NG-070 cases, including history, branch applicability, duplicate IDs, co-change, and semantic uncertainty?
3. Are the three-axis result model, committed/worktree snapshot rules, external-test admissibility, and resource bounds sufficiently precise and testable?
4. Do the CLI, report/input/output manifests, JSON Schema, exit/status mapping, no-write default, `--out-dir` protections, and stdout/stderr contract agree without cross-branch collisions or write bypasses?
5. Does the nested `decision_query` candidate match the registered handler and direct TypeSafe adapter (`claims[0]`), and is it acceptable to treat the separate strict profile schema's rejection of `claims` as a blocking integration limitation while keeping scanner egress disabled? If not, state whether a separately authorized host-schema change is required. Do not claim schema conformance or dispatch readiness.
6. Does the progress design match the installed `assess_progress` ledger contract, with declared checkbox counts separate from verified progress and no percentage without the trust callbacks?
7. Are the privacy/egress rules testable, including the one-time v0.12 Astra exception versus future per-packet consent, redaction, limits, stale consent, timeout and no retry?
8. Is the supplied packet complete enough to review these claims? Name any missing governing evidence or artifact and narrow the verdict accordingly.

### Later implementation validation

After implementation, tests and sample reports exist, ask Astra to assess:

1. Does the implementation detect each of the five drift classes without conflating deterministic checks, semantic review, or branch applicability?
2. Can missing data, stale refs, dirty trees, ambiguous status, malformed output, and Jev failures ever result in false `NO_DRIFT_DETECTED`, false PASS, or misleading progress?
3. Are the claims in output reproducible from the pinned inputs and cited evidence, including branch-specific history and current implementation paths?
4. Does `decision_query` remain a bounded advisory route with the stated data minimization, opt-in, cost, and no-silent-fallback behavior?
5. Are stdout/default no-write and explicit `--out-dir` persistence safe, deterministic, and free from accidental overwrite or cross-branch file collisions?
6. Are tests sufficient for the five NG-070 examples and negative/fail-closed cases? Identify untested acceptance criteria and any overclaimed test result.
7. Is the exact reviewed packet complete enough to support the requested validation, and what limitations remain?

Astra must not be asked to infer a missing commit, fill in missing runtime evidence, broaden the approved scope, or edit/authorize the repository. Any proposed contract change returns to the owner and the same SPEC-071 lineage.

## 8. Prior design-only Astra return (historical scope)

- Reviewed artifact: SPEC-071 v0.12-draft design packet, SHA-256 `439bab94579ad33c174afde2612031c9d0f1afbc593ebb903524c38ec1e246b7`.
- Verdict: `AMEND`; resolve blockers 1–7 in the same SPEC-071 lineage before calling the design final-review-ready.
- The response did not supply a legible `implementation authorization` value. Treat as no authorization; the SPEC/task gate remains closed.
- This return did not validate code, tests, runtime, generated reports, the full Dashboard, or source facts beyond the supplied packet. It did not independently recompute the packet SHA.
- Full response, raw SSE, transcript and normalization receipt are archived under `Specs/IMPACT/ASTRA-REVIEWS/SPEC-071-design-439bab94579a/`.

## 9. Final packet fields to fill after implementation

- Final repository path and branch:
- Exact effective HEAD SHA:
- `git status --short` output and dirty/untracked disposition:
- Final SPEC-071 version and SHA-256:
- Implementation/test/schema/fixture/report file list and per-file SHA-256:
- Frozen review packet path and SHA-256:
- AIOPT3/AIOPT4/AIOPT5 scan refs and resolved SHAs:
- Test/validator commands, outputs, exit codes, and actual case counts:
- Generated sample outputs and their hashes:
- `decision_query` usage/receipt (or “not exercised”) and egress/cost status:
- Known failures, limitations, omissions/redactions:
- Exact owner consent identifier for this packet SHA:

Until these fields are verified and the owner consents to that exact packet, this remains a context template only and no final Astra submission is authorized.
