# GPT-6 Astra design-only review — SPEC-071 v0.12-draft

Review class: design review of a draft SPEC only. The drift-auditor tool is NOT implemented. This packet cannot validate code, tests, runtime behavior, or generated reports and cannot authorize implementation.
Requested verdict: APPROVE (design readiness only), AMEND, or NO-GO; explicitly state implementation authorization: NO. Give numbered findings with file/section evidence and distinguish blocking design gaps from optional improvements.

## Frozen repository/worktree identity
### AIOPT3: worktree=/home/gandalf/workspace/novagenesis-ng070-aiopt3; branch=AIOPT3; HEAD=24a3851e1d5a90f039b9e148a1af38dd03c4d925
git status --short --untracked-files=normal:
 M Docs/DECISIONS/SPEC-STATUS-REGISTER.md
 M Specs/SPEC-031-recvmmsg-batched-receive.md
 M Specs/SPEC-037-promote-aiopt3-default-branch.md
 M Specs/SPEC-054-release-master-plan-and-audit.md
git ls-files --others --exclude-standard count=0

### AIOPT4: worktree=/home/gandalf/workspace/novagenesis-ng070-aiopt4; branch=AIOPT4; HEAD=0dd0dd4b2fd0a9d416aff232ad07672fe39746b6
git status --short --untracked-files=normal:
 M Docs/DECISIONS/SPEC-STATUS-REGISTER.md
 M Specs/SPEC-031-recvmmsg-batched-receive.md
 M Specs/SPEC-037-promote-aiopt3-default-branch.md
 M Specs/SPEC-054-release-master-plan-and-audit.md
git ls-files --others --exclude-standard count=0

### AIOPT5: worktree=/home/gandalf/workspace/novagenesis; branch=AIOPT5; HEAD=8d2386dcb10d2b37f133d4b24c2cb96761753d82
git status --short --untracked-files=normal:
 M .gitignore
 M CMakeLists.txt
 M Docs/DECISIONS/SPEC-STATUS-REGISTER.md
 M Scripts/Python/validate_schemas.py
 M Scripts/test_checkers/default/compile_commands.json
 M Scripts/test_checkers/diagnostic/compile_commands.json
 M Scripts/tests/test_spec027_stage_wiring.py
 M Specs/SPEC-031-recvmmsg-batched-receive.md
 M Specs/SPEC-037-promote-aiopt3-default-branch.md
 M Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md
 M Specs/SPEC-045-core-evaluate-functional-harness.md
 M Specs/SPEC-069-ng039-public-evidence-deduplication.md
 D checkers/__pycache__/compile_checker.cpython-314.opt-1.pyc
 D checkers/__pycache__/compile_checker.cpython-314.pyc
 D checkers/__pycache__/link_checker.cpython-314.opt-1.pyc
 D checkers/__pycache__/link_checker.cpython-314.pyc
 D checkers/__pycache__/target_census_checker.cpython-314.opt-1.pyc
 D checkers/__pycache__/target_census_checker.cpython-314.pyc
 D checkers/__pycache__/test_compile_checker.cpython-314.opt-1.pyc
 D checkers/__pycache__/test_compile_checker.cpython-314.pyc
 D checkers/__pycache__/test_link_checker.cpython-314.opt-1.pyc
 D checkers/__pycache__/test_link_checker.cpython-314.pyc
 D checkers/__pycache__/test_target_census_checker.cpython-314.opt-1.pyc
 D checkers/__pycache__/test_target_census_checker.cpython-314.pyc
 M checkers/test_evidence_schemas.py
?? .hermes/
?? Scripts/Python/generate_spec_impact_sidecars.py
?? Specs/IMPACT/
?? Specs/SPEC-070-branch-spec-sanitization-and-impact-sidecars.md
?? Specs/SPEC-071-spec-implementation-drift-auditor.md
git ls-files --others --exclude-standard count=73
UNTRACKED: .hermes/plans/2026-10-17_102933-sanitizacao-registros-publicos-ng039.md
UNTRACKED: Scripts/Python/generate_spec_impact_sidecars.py
UNTRACKED: Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md
UNTRACKED: Specs/IMPACT/SPEC-001-NRNCS-debug-logging.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-001-hello-ipc-ht-bid.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-002-message-leak-fix.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-003-fix-use-after-free-and-scheduled-messages.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-004-contentapp-performance-optimisation.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-005-gw-shm-poll-interval-reduction.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-006-output-queue-semaphore-contention.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-007-output-queue-lock-decoupling.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-008-ht-bid-raw-socket-discovery.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-009-alpine-vm-ssh-run-scripts.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-010-contentapp-hello-ipc20-bindings.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-011-periodic-exposition-contentapp.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-012-hash-abstraction.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-013-ngal-adaptation-layer.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-014-ngal-hash-mismatch-diagnosis.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-017-content-name-correlation.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-018-payload-sticks-on-reused-inline-response.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-019-payload-hash-logging.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-020-subscription-redelivery-fix.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-021-one-message-per-file.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-022-nrinfopayload01-separate-messages.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-022-one-file-per-message-nrinfopayload01.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-023-contentpublish-burst-throttling.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-027-pgcs-multithreading.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-028-stress-telemetry-heartbeat.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-029-pgcs-self-mac-failfast.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-030-photo-delivery-integrity.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-031-recvmmsg-batched-receive.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-032-gw-input-queue-batching.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-033-R2-newmessage-exhaustion-output.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-033-R3-B-gw-caller-safety.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-033-R3-B2-gw-input-queue-null-safety.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-033-R3-C-process-output-safety.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-033-R3-C1-caller-failure-handling.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-033-R3-caller-safety-amendment.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-033-message-commandline-refactor.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-034-pgcs-low-rate-diagnostic.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-035-nbtestapp-nrncs-intermediate.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-036-deprecate-standalone-pss-girs-hts.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-037-promote-aiopt3-default-branch.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-038-public-repository-hygiene.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-039-common-performance-measurement.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-044-core-run-evaluate-invalid-pg-downcast.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-045-core-evaluate-functional-harness.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-046-multi-vm-runner-safe-teardown.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-047-pgcs-readiness-periodic-activation.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-048-pgcs-receiver-processing-evidence.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-049-nrncs-readiness-binding-oracles.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-050-contentapp-repository-control-plane-readback.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-051-cross-vm-nrncs-discovery-diagnosis.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-052-remote-ssh-multi-vm-executor.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-053-debug-log-observability-matrix.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-054-release-master-plan-and-audit.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-055-ng-elc-local-mode.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-056-ng-elc-evidence-provenance-hardening.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-057-ng-elc-canonical-entrypoint-rename.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-058-ng-elc-privileged-local-profile.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-059-ng-elc-qemu-guest-agent-channel.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-060-ng-elc-remote-payload-evidence.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-069-ng039-public-evidence-deduplication.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-070-branch-spec-sanitization-and-impact-sidecars.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-071-ASTRA-DESIGN-REVIEW-INSTRUCTIONS.txt
UNTRACKED: Specs/IMPACT/SPEC-071-ASTRA-DESIGN-REVIEW-PACKET.md
UNTRACKED: Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md
UNTRACKED: Specs/IMPACT/SPEC-071-spec-implementation-drift-auditor.md.affected-files.md
UNTRACKED: Specs/IMPACT/SPEC-MUSL-001-gcc15-thread-ambiguity.md.affected-files.md
UNTRACKED: Specs/SPEC-070-branch-spec-sanitization-and-impact-sidecars.md
UNTRACKED: Specs/SPEC-071-spec-implementation-drift-auditor.md

The three worktrees are not interchangeable. The branch review report and NG-070 sidecars refer to historical pinned snapshots; current dirty document contents are identified separately below.
No C/C++ implementation source is included because this request is about design readiness; the packet makes no claim that the cited historical implementation findings were independently re-executed.

===== FILE: /home/gandalf/workspace/novagenesis/Specs/SPEC-071-spec-implementation-drift-auditor.md | sha256=7eade367a0be6d80f1f0f9380d9d44ad6af13559bd242dc85d2c22e8ab9c70c8 | bytes=17891 =====
# SPEC-071: NovaGenesis SPEC Implementation Drift Auditor

**Version:** v0.12-draft
**Date:** 2026-10-17
**Author:** Antonio Alberti / Hermes Agent
**Status:** Draft — design questions open; implementation not authorized
**Branch:** AIOPT5
**Baseline commit:** `8d2386dcb10d2b37f133d4b24c2cb96761753d82`
**Implementation commit:** —
**Related:** `SPEC-070-branch-spec-sanitization-and-impact-sidecars.md`, `SPEC-031-recvmmsg-batched-receive.md`, `SPEC-039-common-performance-measurement.md`
**Owner task:** `NG-071-detector-drift-spec-implementation-v1-20261017.md`

## 1. Problem and objective

NovaGenesis SPEC status and current implementation evidence can drift apart. A SPEC may remain Draft after an implementation commit, describe an old source path as current, refer to a SPEC file absent from the reviewed tree, prescribe a verification step that cannot run after the stated deletion, or carry an AIOPT3 origin label that is mistaken for AIOPT5 authority. The current branch audit also showed that missing status fields and historical copies make a single percentage-of-progress misleading.

This tool is intended to detect and explain those mismatches from exact, read-only repository snapshots. It may call the existing Hermes `decision_query` tool as an optional advisory reviewer for a bounded evidence claim, but local rules remain authoritative for inventory, status, references, commit ancestry, file paths and any progress calculation.

## 2. Inputs and examples motivating the tool

Initial drift cases from the 2026-10-17 branch audit:

1. `SPEC-031` said Draft and described `recvfrom()` as the current path, while implementation commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` is an ancestor of all three pinned heads and their `ReceiveDispatcher` uses `recvmmsg()`.
2. `SPEC-069` instructed a post-deletion verification to rerun a pair-equality check that requires the deleted `.last.sse` candidates to exist.
3. AIOPT5 `SPEC-044/045` contained `Related:` references to SPEC-040/041, absent from the pinned trees and reachable local history.
4. The status register still said 24 SPEC files at AIOPT3 `cd804a7`, while the reviewed snapshots contained 58/60/48.
5. A `Branch: AIOPT3` field frequently marked document origin/scope, but was easy to mistake for current AIOPT4/AIOPT5 authority.

These are seed fixtures/examples, not an exhaustive drift taxonomy or evidence that every status field is wrong.

## 3. Proposed operating model (draft)

### Local evidence layer

The tool should operate locally and read-only by default. It should pin exact branch HEADs and distinguish clean committed trees from dirty/untracked working-tree content. For each SPEC, it may collect:

- Frontmatter/header identity, branch, status, baseline and implementation commit.
- Acceptance criteria and explicit file/scope/allowlist declarations.
- SPEC references and their existence in the same pinned tree.
- SPEC-linked commit subjects, ancestry and changed-file lists.
- Locally available test/evidence locators, preserving their exact hash/snapshot and whether they are raw, summarized, missing or unverified.

The collector must not treat file presence, commit ancestry, a status string or a JEV response as proof that an acceptance criterion passed. It must not switch branches, run builds/tests, edit SPECs, stage/commit/push, or read credentials. Any future output file must have an owner-approved path and explicit write mode; dry-run/stdout is the default candidate. Hermes currently also exposes a local-only `assess_progress` tool; whether its snapshot-bound ledger contract should be reused or adapted must be checked before duplicating that logic.

### Drift finding model

Candidate finding states (names are provisional):

- `CONFIRMED_DRIFT` — a deterministic contradiction is demonstrated by pinned local evidence (for example, a Related target absent from that exact tree, or a declared current path differing from the source at the same snapshot).
- `POSSIBLE_DRIFT` — a mismatch is plausible but needs semantic or human review.
- `INDETERMINATE` — the SPEC, source, test or authority evidence is absent, ambiguous, dirty or unpinned.
- `NO_DRIFT_DETECTED` — only for checks with an explicit, completed rule set; this does not mean the SPEC is implemented or accepted.
- `HISTORICAL` — the text is a dated decision/problem statement or a copied branch-origin document, not a current instruction for the inspected branch.

Every finding should cite the SPEC filename/section, branch and exact HEAD, evidence paths/commit SHA, the rule used, and the reason it was classified. Missing evidence is never silently converted to `NO_DRIFT_DETECTED`.

### Optional `decision_query` support

Use the registered `decision_query` integration, not a direct HTTP client. The preferred initial use is one bounded, typed Choice question per ambiguous claim, such as whether a small supplied evidence summary supports, contradicts, or is insufficient for a SPEC implementation claim. The tool must label Jev's result advisory and preserve it separately from the deterministic local finding.

`decision_query` must not discover facts by itself, validate local file hashes, assign a SPEC's final status, compute a progress percentage, clear a blocker, authorize edits, or execute any action. A Jev `supported` answer cannot promote a gate to PASS; `contradicted` or `insufficient` can only raise a review flag. Malformed responses, errors, missing evidence and timeouts remain `INDETERMINATE`/review-needed; no silent fallback or automatic retry is allowed.

The registered TypeSafe route is default-off and metered. No JEV call is authorized by this Draft. Any live use requires the existing profile opt-in, exact one-shot payload/rate/cost binding, and the applicable Hermes approval behavior. Send only the approved claim and minimized evidence summary; do not transmit full SPECs, repositories, code, raw logs, PII or protected evidence by default.

### Progress reporting

Do not ask Jev to invent a percentage. If an owner-approved, SPEC-defined denominator and weighting exist, compute progress locally from criterion-level evidence states and show the denominator, weights, source snapshot and unknowns. Otherwise report gate/status counts only; any equal-weight count must be explicitly labelled an indicator rather than a SPEC completion percentage. A JEV confidence score must never be displayed as implementation progress.

## 4. Candidate files affected (not yet authorized for implementation)

| Path | Candidate change | Reason |
|---|---|---|
| `Scripts/Python/spec_implementation_drift_auditor.py` | New read-only CLI/tool | Local snapshot inventory and drift findings; exact location/CLI remains an owner decision |
| `Scripts/tests/test_spec_implementation_drift_auditor.py` | New tests/fixtures | Pin the NG-070 findings and fail-closed cases; exact layout TBD |
| `Specs/IMPACT/` or owner-approved results directory | Optional report output | Persisted output path and write behavior remain open |
| `Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md` | Review-context template for the post-implementation Astra gate | Contains static design context and a checklist of exact future evidence; must be refreshed and is not submitted now |

No source SPEC, branch, task status, evidence artifact, or Hermes configuration is modified by the future tool unless separately authorized.

**Path decision (2026-10-17):** use the current repository location `/home/gandalf/workspace/novagenesis/Scripts`. The requested `/workspace/novagenesis/Scripts` path is absent in this execution environment; the owner selected the existing repository path.

## 5. Safety, privacy, and authority gates

- No branch checkout/switch, write, build, test, stage, commit, push, VM/runtime operation or external call in the default mode.
- Freeze a distinct exact SHA for each branch and a separate digest/status for dirty or untracked inputs. Never mix them into one “current” result.
- Use explicit local rules for deterministic checks; keep semantic JEV advice as a separate field and receipt.
- Default JEV route disabled. No raw source/evidence egress; minimize any owner-authorized request to claim, evidence summary, file/hash references and exact snapshot identity.
- Require owner approval of the final SPEC before implementation. A design approval or JEV opinion is not implementation, publication or release authorization.
- GPT-6 Astra review: the owner has requested a design-only review of the current SPEC Draft now, after packet freeze and exact-SHA consent; this cannot validate implementation or authorize code. A second, mandatory post-implementation review follows completed tests before declaring the tool validated. Both packets must bind exact inputs; the owner remains the acceptance authority, and Astra review does not authorize deployment or unrelated repository changes.

### 5.1 GPT-6 Astra review packet gates

#### Design-only review requested now

The current requested review is limited to SPEC-071 v0.12-draft and whether its design is coherent, sufficiently bounded, testable, privacy-safe, and suitable to advance to final SPEC review. It cannot validate a tool that has not been implemented. The design packet must include the full current SPEC and the detailed NG-070 audit/context artifacts, identify the exact AIOPT5 source HEAD and each historical branch snapshot separately, enumerate dirty/untracked state and exclusions, and state that the CLI schema/filename contract is still open. Ask Astra for `APPROVE` (design readiness only), `AMEND` or `NO-GO`, with `implementation authorization: NO` and numbered evidence-grounded findings. Do not transmit until the exact packet SHA is presented and the owner gives fresh consent for that SHA.

#### Final implementation review

After implementation, tests and sample reports are complete, prepare a self-contained, stateless review packet. `Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md` is the preparation brief; it is not itself an Astra submission and its historical branch pins must be refreshed at review time.

The final packet must include and hash the exact bytes of:

- Final SPEC-071, the NG-071 task record and relevant Dashboard entry.
- The final implementation diff and every new/changed source, test, schema, fixture and report-format contract file.
- Exact branch refs and resolved full HEAD SHAs for AIOPT3/AIOPT4/AIOPT5; current `git status --short`, dirty/untracked-file treatment, and explicitly excluded files.
- Test/validator commands, full outputs and exit codes; test/fixture inventory and actual counts; generated summary/per-SPEC sample reports; negative cases and known failures/limitations.
- The five NG-070 motivating drift cases, the implemented detection rule for each, local evidence references and expected result.
- `decision_query` boundary, sanitized example handoff/receipt if exercised, remote-call approval/cost treatment, no-egress operation and output-write behavior.

Immediately before dispatch, re-read the effective repository HEAD and branch, recompute the packet manifest SHA-256, and verify the submitted bytes exactly match the frozen packet. Check current review quota and any rate-card estimate, keeping subscription quota, flexible credits and estimated cost as separate quantities. Present the exact packet SHA and bounded review scope to the owner and obtain fresh explicit consent before any paid/external call. A changed input creates a new packet SHA and requires fresh consent; discard Astra findings against stale inputs. If dispatch may have started but the client times out, treat delivery/billing as unknown; do not retry without fresh consent for that exact packet. Do not send secrets, PII, raw logs or protected evidence. If material evidence is omitted or redacted, name the omission and limit the review claim accordingly.

Ask Astra for `APPROVE`, `AMEND` or `NO-GO` against the SPEC acceptance criteria, with numbered findings, exact file/section/line evidence, concrete false-PASS/false-negative risks, and any unresolved requirement. Preserve the exact response and packet identity. Resolve any `AMEND` in the same SPEC-071 lineage; Astra advice is not owner authorization. The tool is not declared validated until Astra's exact-SHA gate and owner acceptance are recorded.

## 6. Initial acceptance criteria (provisional)

- [ ] The owner resolves the design questions below and the SPEC is updated in the same SPEC-071 lineage.
- [ ] The final schema/report distinguishes deterministic findings, JEV advice, missing evidence, branch applicability and dirty working-tree state.
- [ ] Snapshot identity and references are validated locally; stale/unpinned evidence produces `INDETERMINATE`, never a false clean result.
- [ ] Tests reproduce the five NG-070 examples and include negative cases for wrong branch/head, missing targets, stale status, conflicting file/path evidence, missing acceptance evidence, malformed JEV output, JEV unavailable and disabled egress.
- [ ] Any progress figure is locally computed from explicit owner-approved criteria/weights; otherwise the tool emits no percent.
- [ ] JEV is off by default and a no-egress local run is fully usable.
- [ ] Owner approval of this SPEC is recorded before implementation. No implementation is authorized by the Draft.
- [ ] A design-only GPT-6 Astra review of the exact frozen SPEC packet returns `APPROVE`, `AMEND` or `NO-GO`; the record states `implementation authorization: NO` and does not imply code readiness.
- [ ] After implementation and tests, GPT-6 Astra reviews the exact frozen implementation packet; findings are resolved in this SPEC lineage and owner acceptance is recorded before claiming the tool is validated.

## 7. Decisions recorded and open question

Owner decisions recorded 2026-10-17:

- Branch scope: v1 scans AIOPT3/AIOPT4/AIOPT5 separately, each with its own exact pinned HEAD; never merge branch evidence into one snapshot.
- Integration/output: a local CLI emits a bounded JSON/Markdown evidence bundle; Hermes uses the registered `decision_query` on selected claims. No direct raw HTTP client is in scope.
- Remote data boundary: only sanitized claims, status metadata, hashes and short evidence summaries; no source code, raw logs, PII or protected evidence.
- Progress: report gate/status counts; emit a percentage only where the SPEC defines criteria and weights. Do not use Jev confidence as progress.
- Drift coverage: v1 requires all five classes—status/implementation-commit mismatch; current-path claims vs source; dangling/ambiguous references and branch applicability; declared affected paths vs commit changes; acceptance/test evidence vs current branch. Run deterministic checks first; evidence/semantic findings not mechanically established must be flagged for review, not auto-resolved.
- Report layout: produce both a consolidated summary and per-SPEC findings, with every branch snapshot kept separate.
- Persistence: default is a concise summary on stdout with no file writes. Only an explicit `--out-dir` may write the consolidated summary and per-SPEC report files.
- Review sequencing: the owner requested a design-only GPT-6 Astra review of this exact Draft now; it is separate from the mandatory post-implementation review and cannot approve implementation. Submission remains blocked until the packet SHA and exact scope are presented and explicitly authorized.

Remaining specification work:

1. Freeze report filenames, schema, and exact CLI contract in this SPEC; test documented output paths and fail-closed behavior before owner approval.

## 8. Revision history

| Version | Date | Change |
|---|---|---|
| v0.1-draft | 2026-10-17 | Initial design draft based on NG-070 findings; questions open; no code authorized |
| v0.2-draft | 2026-10-17 | Recorded owner choices on branch scope, decision_query boundary, remote data and progress; drift classes/report shape remain open |
| v0.3-draft | 2026-10-17 | Selected all five v1 drift classes; deterministic checks precede review-needed semantic/evidence findings; report granularity remains open |
| v0.4-draft | 2026-10-17 | Selected consolidated summary plus per-SPEC reports; explicit output directory/write-mode detail remains open |
| v0.5-draft | 2026-10-17 | Set stdout/no-write default; persist summary and per-SPEC reports only with explicit `--out-dir` |
| v0.6-draft | 2026-10-17 | Recorded path-resolution blocker: requested `/workspace/...` is absent; current checkout uses `/home/gandalf/workspace/...` |
| v0.7-draft | 2026-10-17 | Owner selected the current repository `Scripts/` path; only schema/CLI freeze remains open |
| v0.8-draft | 2026-10-17 | Added mandatory post-implementation Astra gate with exact-packet identity and evidence requirements |
| v0.9-draft | 2026-10-17 | Added a detailed Astra context brief and post-implementation review acceptance criterion |
| v0.10-draft | 2026-10-17 | Added GPT-6 Astra packet freeze, quota/cost, exact-SHA consent, timeout, and no-secret submission gates |
| v0.11-draft | 2026-10-17 | Added owner-requested design-only Astra review gate, separate from post-implementation validation |
| v0.12-draft | 2026-10-17 | Bounded the current Astra review to design readiness and added exact-packet consent before submission |

## 9. Reference basis

- Hermes local decision-stack contract: `hermes-local-decision-stack` and `references/typesafe-direct-payload.md`. Use the registered `decision_query` route; do not clone the provider API directly in the script.
- TypeSafe API reference (`Systemone`, `/v1/systemone`): one or more typed questions can be asked about supplied state, including Choice questions; consulted 2026-10-17.
- TypeSafe submitted-data/retention guidance: consult current terms and agreements before any live egress; policy page last updated 2026-10-11.
===== END FILE =====
===== FILE: /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md | sha256=b7bbc42ad34fcacbe33e22547a4aec3bd4b3a1a8ff921c8e31a2a48472d80acf | bytes=12330 =====
# SPEC-071 — GPT-6 Astra Review Context

Status: preparation brief only. The owner requested a design-only review of the current SPEC Draft now; this file is not the frozen packet and has not been sent. No tool implementation exists, so this review cannot validate implementation. A separate, mandatory final review follows implementation and tests, with fresh packet hashing and consent.

This file preserves the detailed context so Astra can assess the SPEC design now and the implemented tool later. The current review must be explicitly limited to the SPEC design. At the later implementation gate, include the complete relevant file contents in a new stateless packet and replace all pending fields with verified results.

## 1. Review objective

Current stage: conduct a design-only review of SPEC-071 v0.11-draft. Assess whether the proposed contract is coherent, sufficiently bounded, testable, and safe to proceed to final SPEC approval. The tool has not been implemented, so Astra must not claim to validate implementation behavior, tests, runtime, or actual reports.

A separate mandatory post-implementation review must later validate the completed code and tests against a new exact-SHA packet.

## 2. Stage-specific packet contents

### Current design-only review packet

Include the complete current SPEC-071 v0.12-draft, the full NG-070 branch applicability report, SPEC-070, NG-071 task note, NG-070 task note, this context brief, the Hermes `decision_query` contract, and line-anchored excerpts of the exact SPEC-031/044/045/069 clauses relevant to the drift examples, with whole-file SHA-256 identities and branch labels for each excerpt. Include the active Dashboard entry only, plus the whole Dashboard digest; omit unrelated task rows and state this limitation. Record the current AIOPT5 HEAD, each AIOPT3/AIOPT4 worktree HEAD, dirty/untracked status in all three worktrees, and which exact files are included/excluded. State plainly that no tool code, tests, runtime result, or generated tool report exists yet; the CLI schema/filename contract is still open. The question is design readiness only.

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
10. The owner has now requested a separate design-only Astra review of the current SPEC Draft; this is not a code/tool validation and remains pending exact packet SHA presentation and consent.

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

### Current design-only review

Ask GPT-6 Astra to return `APPROVE` (design readiness only), `AMEND`, or `NO-GO`, with numbered findings and exact file/section evidence. Explicitly require `implementation authorization: NO`. Ask:

1. Is the proposed local-first/decision_query split coherent, bounded and implementable without a direct provider client?
2. Are the five selected drift classes sufficiently defined, and are deterministic findings kept separate from semantic/review-needed findings?
3. Are snapshot identity, branch separation, historical applicability and dirty/untracked state handled fail-closed?
4. Do the progress, egress, cost, no-write and no-auto-approval boundaries avoid false claims or hidden side effects?
5. Does the SPEC clearly identify unresolved CLI/schema details as open, without claiming the tool exists or works?
6. Is the supplied design packet adequate to assess these claims? Name any missing governing evidence and limit verdict scope accordingly.

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

## 8. Final packet fields to fill after implementation

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

Until these fields are verified and the owner consents to that exact packet, this remains a context template only and no Astra submission is authorized.
===== END FILE =====
===== FILE: /home/gandalf/workspace/novagenesis/Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md | sha256=a52b4a877083ec1a37c6b5359cfa57381298db3bd30b0df3a463861230710f16 | bytes=8643 =====
# Branch Applicability Review — AIOPT3, AIOPT4, AIOPT5

**Review status:** Findings and proposed edits only. No pre-existing SPEC or status-register content has been changed.
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
===== END FILE =====
===== FILE: /home/gandalf/workspace/novagenesis/Specs/SPEC-070-branch-spec-sanitization-and-impact-sidecars.md | sha256=de8dce5e5872280aad210ad0245e3aac5f7b91972695afe31825bbb7fc2347c2 | bytes=9369 =====
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
===== END FILE =====
===== FILE: /home/gandalf/ObsidianVault/Tarefas/NG-071-detector-drift-spec-implementation-v1-20261017.md | sha256=0e92b7b86d62cb41640d03f03c5506cf832ed2ef9c3f1c65fd08f1f782d83946 | bytes=8081 =====
---
id: NG-071
titulo: Desenhar auditor de drift entre SPEC e implementação NovaGenesis
area: NovaGenesis
tipo: tarefa
status: planeada
prioridade: media
versao: 1
inicio: 2026-10-17
prazo:
fim:
responsavel: Antonio Alberti
stakeholders: manutenção NovaGenesis
parent:
depends_on:
  - SPEC-071-spec-implementation-drift-auditor.md
blocked_by: Freeze the design packet and obtain exact-SHA dispatch consent before Astra; finalize CLI schema/contract and obtain SPEC approval before implementation.
tags:
  - tarefa
  - novagenesis
  - spec-drift
  - decision-query
---

# NG-071 — Desenhar auditor de drift entre SPEC e implementação NovaGenesis

## 1. Resultado pretendido

Produzir uma SPEC aprovada para um utilitário local-first em `Scripts/` que detecte desvios entre o contrato de uma SPEC, a branch/snapshot analisada e as evidências de implementação, com `decision_query` apenas como apoio consultivo opcional.

## 2. Âmbito

### Incluído

- Desenhar um relatório read-only, baseado em SHAs e com estados de evidência não ambíguos.
- Definir quais sinais locais o detector confronta: frontmatter/status, critérios de aceitação, referências, ficheiros declarados, commits/diffs, testes e evidências apontadas.
- Definir a interface para usar `decision_query` como segundo parecer tipado, sob os gates existentes de privacidade, aprovação e custo.
- Usar os desvios concretos encontrados na NG-070 como fixtures de desenho: SPEC-031, SPEC-069, SPEC-044/045, registo mestre desactualizado e cópias AIOPT3-origin em outros branches.
- Preparar um pacote de contexto completo e de escopo restrito para uma revisão de desenho do Draft por GPT-6 Astra; enviar somente depois de congelar o SHA do pacote e obter consentimento explícito para esse SHA.
- Deixar preparado o gate de validação final por Astra após implementação e testes, em tarefa/packet futuro.

### Excluído

- Implementar o utilitário ou fazer chamadas live a Jev/TypeSafe nesta fase. A revisão limitada do Draft por GPT-6 Astra é a única revisão externa desta fase e só pode ser enviada após consentimento exact-SHA e gate de quota/custo; não valida código/implementação.
- Enviar SPECs completas, código, logs brutos, PII ou evidência protegida a um serviço remoto.
- Permitir que Jev calcule autonomamente percentagens, aceite gates, altere status, edite arquivos ou autorize ações.
- Mudar SPECs existentes, branches, códigos de produção, configurações Hermes, commits ou pushes além das correções já autorizadas em NG-070.

## 3. Critérios de aceitação

- [ ] As decisões de desenho do proprietário estão registadas.
- [ ] A SPEC define entradas/snapshots, catálogo de drift, estados de saída, provas/referências, limites de privacidade/egress/custo, comportamento fail-closed e ausência de side effects.
- [ ] Está explícito o que o cálculo local determina e o que `decision_query` pode apenas aconselhar.
- [ ] Fixtures derivadas dos cinco desvios NG-070 cobrem resultado positivo, contradição, evidência insuficiente e branch/snapshot mismatch.
- [ ] O proprietário aprova a SPEC antes de qualquer implementação.
- [ ] A revisão de desenho por GPT-6 Astra foi feita sobre o packet de SHA exacto aprovado pelo proprietário; resultado e limites foram registados, sem autorização de implementação.

## 4. Plano de trabalho

- [x] Recuperar os achados exact-SHA e artefactos da tarefa NG-070.
- [x] Preparar SPEC-071 Draft e perguntas de desenho para o proprietário.
- [x] Preparar o brief de contexto para revisão GPT-6 Astra pós-implementação.
- [ ] Congelar o packet design-only para Astra, apresentar o SHA exacto e aguardar consentimento para a chamada.
- [ ] Fazer a revisão design-only após consentimento, preservar packet/retorno e registar o veredicto sem conceder autorização de implementação.
- [ ] Resolver feedback e fechar a SPEC na mesma linhagem.
- [ ] Obter aprovação de implementação; sem ela, manter o trabalho read-only.

## 5. Estado actual

Última actualização: 2026-10-17

- Feito: recolhidos os casos concretos de drift; SPEC-071 Draft v0.12 e brief detalhado para revisão Astra estão registados.
- Falta: congelar packet de design review, obter consentimento exact-SHA, submeter apenas após consentimento e resolver feedback.
- Próximo passo: gerar packet completo com HEAD e hashes actuais; mostrar SHA/tamanho/limites ao proprietário antes de qualquer chamada.
- Bloqueio: ferramenta não implementada; Astra só pode rever o desenho agora, não validar código/testes.

## 6. Decisões

| Data | Decisão | Motivo | Impacto |
|---|---|---|---|
| 2026-10-17 | Desenhar primeiro; não implementar nesta tarefa | Pedido explícito para co-desenhar a SPEC | Código, chamadas live, commit e push continuam excluídos |
| 2026-10-17 | v1 analisa AIOPT3/AIOPT4/AIOPT5 separadamente, em SHAs pinned | Resposta do proprietário | Evidências de branches diferentes não podem ser misturadas |
| 2026-10-17 | CLI produz bundle JSON/Markdown; Hermes chama `decision_query` para claims seleccionados | Resposta do proprietário | Sem cliente HTTP directo; Jev continua advisory |
| 2026-10-17 | Egress limitado a claims sanitizados, metadados, hashes e resumos; sem código/logs/PII | Resposta do proprietário | Minimização de dados remotos |
| 2026-10-17 | Estados/contagens como padrão; percentual só com critérios e pesos definidos na SPEC | Resposta do proprietário | Jev não calcula progresso |
| 2026-10-17 | Cobertura v1 inclui as cinco classes de drift; checks determinísticos primeiro, findings semânticos/evidenciais seguem para revisão | Resposta do proprietário | Sem resolução automática por JEV; resumo consolidado + relatório por SPEC |
| 2026-10-17 | Saída: resumo consolidado + findings por SPEC, mantendo branches separados | Resposta do proprietário | Formato JSON/Markdown; destino vem de `--out-dir` |
| 2026-10-17 | Persistência opt-in: stdout sem writes por padrão; `--out-dir` explícito para gravar relatórios | Resposta do proprietário | Sem mutações de arquivos por padrão |
| 2026-10-17 | Local de código: `/home/gandalf/workspace/novagenesis/Scripts` | Resposta do proprietário; `/workspace/novagenesis/Scripts` não existe neste ambiente | Implementar no `Scripts/` do checkout atual |
| 2026-10-17 | Revisão final obrigatória por GPT-6 Astra após implementação/testes, com packet completo e SHA exato | Pedido do proprietário | Não enviar agora; pedir consentimento exact-SHA e registar APPROVE/AMEND/NO-GO depois de construir/testar |
| 2026-10-17 | Astra design-only do SPEC Draft solicitado agora | Pedido do proprietário | Escopo limitado à prontidão do desenho; packet SHA e consentimento ainda pendentes; implementation authorization = NO |

## 7. Dependências

- SPEC: `SPEC-071-spec-implementation-drift-auditor.md`.
- Relatório-base: `Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md` e sidecars NG-070.

## 8. Evidência

- AIOPT3/AIOPT4/AIOPT5 exact-SHA findings are recorded in NG-070 and `Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md`; frozen heads: AIOPT3 `24a3851e1d5a90f039b9e148a1af38dd03c4d925`, AIOPT4 `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`, AIOPT5 `8d2386dcb10d2b37f133d4b24c2cb96761753d82`.
- Draft SPEC: `/home/gandalf/workspace/novagenesis/Specs/SPEC-071-spec-implementation-drift-auditor.md`; sidecar: `/home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-071-spec-implementation-drift-auditor.md.affected-files.md`.
- Astra context template: `/home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md` (preparation only; not submitted). It requires exact HEAD/packet SHA, full relevant context, tests/results and explicit consent after implementation.
- O utilizador seleccionou o repositório actual para `Scripts/`: `/home/gandalf/workspace/novagenesis/Scripts`; `/workspace/novagenesis/Scripts` não existe neste ambiente.
- Existing advisory path: plugin `hermes_decision_stack`, tool `decision_query`; no live call or implementation occurred in this design phase.
===== END FILE =====
===== FILE: /home/gandalf/ObsidianVault/Tarefas/NG-070-saneamento-specs-branches-v1-20261017.md | sha256=e46554e585a3139de4ad4e7eb8eab9712c21a03a9de2faf2ca665185f265d344 | bytes=9249 =====
---
id: NG-070
titulo: Auditoria e saneamento documental das SPECs AIOPT3/AIOPT4/AIOPT5
area: NovaGenesis
tipo: tarefa
status: concluida
prioridade: media
versao: 1
inicio: 2026-10-17
prazo:
fim: 2026-10-17
responsavel: Antonio Alberti
stakeholders: manutenção NovaGenesis
parent:
depends_on:
  - SPEC-070-branch-spec-sanitization-and-impact-sidecars.md
blocked_by:
tags:
  - tarefa
  - novagenesis
  - specs
  - branch-hygiene
---

# NG-070 — Auditoria e saneamento documental das SPECs AIOPT3/AIOPT4/AIOPT5

## 1. Resultado pretendido

Produzir uma classificação verificável das SPECs em AIOPT3, AIOPT4 e AIOPT5 e propor correções documentais que deixem claro quais contratos ainda são válidos em cada linha, preservando os documentos e o histórico Git.

## 2. Âmbito

### Incluído

- Fixar e registar os HEADs exactos das branches locais AIOPT3, AIOPT4 e AIOPT5.
- Inventariar SPECs em cada branch, incluindo duplicatas de ID e ficheiros ausentes em determinadas branches.
- Classificar cada SPEC por aplicabilidade: actual na branch, histórica, supersedida, específica de outra branch, ou indeterminada; fundamentar cada classificação com evidência do próprio repositório.
- Identificar referências a branches, commits, baselines, estado de implementação, dependências e instruções operacionais que já não correspondam à branch em questão.
- Criar um sidecar Markdown por nome de arquivo SPEC único encontrado na união dos três branches, sob `Specs/IMPACT/`, listando caminhos declarados nas SPECs e arquivos observados em commits associados, com branch/SHA e fonte de cada atribuição; marcar incertezas em vez de inventar completude.
- Apresentar propostas concretas de alteração e riscos antes de editar qualquer SPEC ou registo do repositório.
- Preservar as alterações locais preexistentes no checkout AIOPT5, sem as incorporar na auditoria como se fossem parte de um commit publicado.

### Excluído

- Alterar, apagar, mover ou renomear SPECs, registos ou evidências do repositório antes de aprovação explícita das propostas.
- Reescrever ou alterar histórico Git, fazer commits, pushes ou trocar de branch.
- Alterar código, executar builds, testes de runtime, operações em VMs, ou submeter material a Astra.
- Inferir aplicabilidade apenas pelo nome da branch, pelo estado do registo mestre ou por referências históricas isoladas.

## 3. Critérios de aceitação

- [x] HEAD imutável das três branches registado e confirmado; estado sujo do checkout AIOPT5 separado da evidência commitada.
- [x] Inventário por branch reconciliado com os ficheiros SPEC nela existentes: AIOPT3 58, AIOPT4 60, AIOPT5 48; união branch-pinned de 63 nomes e mais um SPEC-070 local-only.
- [x] Cada SPEC inventariada classificada ou marcada indeterminada, com razão/evidência e proposta quando houver drift; a classificação está em cada sidecar e o resumo em `Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md`.
- [x] Duplicatas de IDs de SPEC e referências pendentes identificadas sem apagar histórico; as propostas continuam sujeitas à decisão do proprietário.
- [x] Relatório e propostas apresentados ao proprietário; as correções aprovadas foram aplicadas apenas na allowlist e nenhuma história Git foi apagada.

## 4. Plano de trabalho

- [x] Fixar os HEADs AIOPT3/AIOPT4/AIOPT5 e preservar o estado actual do checkout AIOPT5 como baseline da auditoria.
- [x] Inventariar e comparar os arquivos SPEC nos três HEADs sem mudar de branch nem tocar na worktree suja.
- [x] Verificar status, referências cruzadas, commits/baselines e claims branch-specific nos heads pinados, distinguindo evidência commitada de notas históricas.
- [x] Criar 64 sidecars em `Specs/IMPACT/`, incluindo um para cada filename do conjunto pinado e SPEC-070 local-only; verificar contagem e links.
- [x] Preparar matriz de classificação e propostas de saneamento com âncoras verificáveis em `Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md`.
- [x] Rever as propostas com o proprietário antes de qualquer escrita nos SPECs originais; aplicar somente a allowlist explicitamente aprovada.

## 5. Estado actual

Última actualização: 2026-10-17

- Feito: branch audit, classificações, 64 sidecars, gerador e correções aprovadas nas worktrees AIOPT3/AIOPT4/AIOPT5; `git diff --check` passou em todas.
- Falta: nada dentro do escopo autorizado.
- Próximo passo: nenhum; commits/push só mediante pedido separado. Os diffs locais ficam disponíveis para revisão.
- Bloqueio: nenhum para a tarefa concluída; commits/push permanecem fora do escopo e não foram autorizados.

## 6. Decisões

| Data | Decisão | Motivo | Impacto |
|---|---|---|---|
| 2026-10-17 | Usar saneamento conservador: classificar e propor; preservar arquivos e histórico | Decisão explícita do proprietário | Edições originais aguardaram revisão e consentimento exact-scope |
| 2026-10-17 | Aplicar a allowlist documental em AIOPT3/AIOPT4/AIOPT5 | Consentimento explícito do proprietário após apresentação das propostas | Diffs locais não commitados; commit/push não autorizados |

## 7. Riscos e bloqueios

| Item | Impacto | Mitigação | Estado |
|---|---|---|---|
| Checkout AIOPT5 contém modificações, deleções e arquivos locais não relacionados preexistentes | Risco de sobrescrita ou atribuição errada | Limitar alterações à allowlist aprovada; preservar o restante do trabalho | aberto |
| Branches podem conter contratos válidos apenas para uma linha histórica | Saneamento excessivo pode apagar contexto ou reautorizar trabalho pausado | Preservar texto histórico; classificar aplicabilidade por branch e pedir revisão antes de editar | aberto |

## 8. Dependências

- Branches locais `AIOPT3`, `AIOPT4` e `AIOPT5`; registar os respectivos SHA antes da análise.
- A auditoria documental não autoriza retomar implementação, runtime ou release em qualquer branch.

## 9. Evidência

- Repositório: `/home/gandalf/workspace/novagenesis`
- Heads verificados: AIOPT3 `24a3851e1d5a90f039b9e148a1af38dd03c4d925`; AIOPT4 `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`; AIOPT5 `8d2386dcb10d2b37f133d4b24c2cb96761753d82`; `origin/*` correspondentes permanecem iguais.
- Sidecars e relatório de baseline: `/home/gandalf/workspace/novagenesis/Specs/IMPACT/`; gerador: `/home/gandalf/workspace/novagenesis/Scripts/Python/generate_spec_impact_sidecars.py`. 64/64 sidecars verificadas por igualdade de nomes, pin SHAs, estrutura e whitespace; os sidecars permanecem ligados aos heads pré-edição.
- Worktrees e correções aprovadas, sem commit: AIOPT3 `/home/gandalf/workspace/novagenesis-ng070-aiopt3` (register, SPEC-031, SPEC-037, SPEC-054); AIOPT4 `/home/gandalf/workspace/novagenesis-ng070-aiopt4` (mesmos quatro); AIOPT5 worktree principal (register, SPEC-031, SPEC-037, SPEC-044, SPEC-045, SPEC-069). `git diff --check` passou nos três.
- Achados verificados e corrigidos: `SPEC-031` agora regista `Implemented — Pending Test`, commit `c638292`, e o `recvfrom()` como baseline histórico; benchmark acceptance continua não verificada. O código `ReceiveDispatcher` usa `recvmmsg()` nos três heads. `SPEC-069` §6 separa checks pre-delete/post-delete. No AIOPT5, refs de `SPEC-044/045` a SPEC-040/041 foram marcadas como contexto histórico indisponível, não dependências activas.
- Registo mestre: `Docs/DECISIONS/SPEC-STATUS-REGISTER.md` ainda declara base `cd804a7`, 24 SPECs (2026-07-16); o aviso histórico foi adicionado nos três branches sem reescrever a tabela antiga.
- Checkout AIOPT5 contém alterações preexistentes fora do escopo; não foram alteradas. Nenhum commit, push, build, runtime, VM ou chamada Astra.

## 10. Histórico de versões

| Versão | Data | Alteração |
|---|---|---|
| v1 | 2026-10-17 | Criação da tarefa e definição do escopo conservador |

## 11. Distribuição dos estados declarados (2026-10-17)

A percentagem é a parcela de SPECs com cada estado declarado, não uma medida real da conclusão de cada SPEC. A barra representa 24 blocos aproximados por worktree.

Legenda: `█` Implemented (inclui Implemented — Pending Test); `░` Draft/Proposal/In Progress; `▒` Blocked/Deferred; `▓` Superseded; `?` sem Status ou estado misto/não normalizado.

```text
AIOPT3  n=58  ██████░░░░░░░░░░▒▒▓▓????  100%
AIOPT4  n=60  ██████░░░░░░░░░░░▒▒▓????  100%
AIOPT5  n=49  ████████░░░░░░░░▒▓▓?????  100%
```

| Worktree | SPECs | Implemented* | Draft/Proposal/In Progress | Blocked/Deferred | Superseded | Unknown/mixed |
|---|---:|---:|---:|---:|---:|---:|
| AIOPT3 | 58 | 15 (25.9%) | 25 (43.1%) | 4 (6.9%) | 4 (6.9%) | 10 (17.2%) |
| AIOPT4 | 60 | 15 (25.0%) | 27 (45.0%) | 4 (6.7%) | 4 (6.7%) | 10 (16.7%) |
| AIOPT5 | 49 | 16 (32.7%) | 16 (32.7%) | 3 (6.1%) | 4 (8.2%) | 10 (20.4%) |

`*` “Implemented — Pending Test” is grouped with Implemented. AIOPT5 includes 48 SPECs at the pinned HEAD plus the local-only SPEC-070. The AIOPT4 set is an archived snapshot; repeated AIOPT3-origin SPECs in AIOPT4/AIOPT5 are not thereby active on those branches. Percentages are rounded to one decimal and describe status mix only.
===== END FILE =====
===== FILE: /home/gandalf/.hermes/skills/software-development/hermes-local-decision-stack/references/typesafe-direct-payload.md | sha256=6e905a4d6e7da4ee1a2ead5b73cdfb4ac2a91159d5ac85836c9a0fdbc2495f0d | bytes=2860 =====
# TypeSafe Direct (typesafe_jev_direct) Payload & Requirements

## Required callback interface

The canonical `decision_query` API expects the host to pass:

```python
decision_query(query, typesafe_jev_fn=callable, typesafe_jev_authorized=True)
```

These are host-injected callbacks, not tool arguments. The bare helper has no authorized remote callbacks and can return `blocked`; the registered Hermes handler can provide the direct route via pre-tool binding hooks and a local sidecar. Resolve the active plugin symlink and inspect `register`, `_typesafe_direct_query`, and the hooks before diagnosing a tool-interface limitation. Invoke the registered tool for a live test; never substitute a direct helper import. A successful receipt with `executed_backend=typesafe_jev_direct` proves that call dispatched, not that every Gateway/Dashboard process has refreshed credentials.

## Payload structure that passes schema validation

```json
{
  "backend": "typesafe_jev_direct",
  "policy_id": "hermes_local_decision_stack.typesafe_jev_direct@1",
  "operation": "verify",
  "claims": ["explicit claim matching evidence"],
  "evidence_refs": [{
    "source_kind": "file",
    "snapshot_id": "<sha256>",
    "path": "/abs/path",
    "repository": "repo-name",
    "effective_head_sha": "<40-hex>",
    "content_digest": "<64-hex>|null",
    "location": "<string>|null"
  }],
  "source_revision": {
    "snapshot_id": "<sha256>",
    "dirty": true,
    "untracked": true
  },
  "options": {
    "source_language": "pt",
    "request_language": "pt",
    "typesafe_evidence_summary": "summary text"
  },
  "constraints": {
    "privacy": "approved_remote",
    "egress": "approved_only",
    "cost": "metered",
    "latency": "medium",
    "device": "auto"
  }
}
```

## Common validation failures

| Field | Rule | Failure example |
|-------|------|-----------------|
| `evidence_refs` | Must be objects with `source_kind` + `snapshot_id` | Passing plain strings |
| `source_revision.dirty/untracked` | Must match `git status` | `false` when workspace has modifications |
| `claims` vs evidence | Claim must align with evidence, not contradict | Claim "file exists" when evidence says "does not exist" |
| `source_language` | Must match claim text language | `en` for Portuguese claim |
| `policy_id` | Must be `hermes_local_decision_stack.typesafe_jev_direct@1` | Using default TOOLS-003 policy |

## Runtime gates (all required for live dispatch)

1. Profile opt-in: `allow_typesafe_jev_direct: true` in config
2. Credential: `TYPESAFE_API_KEY` in profile secrets
3. One-shot consent: exact payload digest + rate snapshot + cost bound + expiry
4. Transport callback: `typesafe_jev_fn` provided by plugin
5. `typesafe_jev_authorized: true` passed by host

Without all five, the backend descriptor stays `readiness: "unavailable"` and routing returns `blocked`.
===== END FILE =====
===== FILE: /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-071-spec-implementation-drift-auditor.md.affected-files.md | sha256=116f27ccf4b248316cc9825a6e43183c9d0e60b88a80fbed1bd7ad4347c91024 | bytes=2918 =====
# Affected-file record: `SPEC-071-spec-implementation-drift-auditor.md`

SPEC title: SPEC-071: NovaGenesis SPEC Implementation Drift Auditor

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | no | — | — | — | — | — | — |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | no | — | — | — | — | — | — |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | no | — | — | — | — | — | — |
| `Local working tree` | SHA-256 `7eade367a0be6d80f1f0f9380d9d44ad6af13559bd242dc85d2c22e8ab9c70c8` | yes | AIOPT5 | (working copy) | Draft — design questions open; implementation not authorized | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | — | Local-only working copy; no branch authority |

Local-only source: `Specs/SPEC-071-spec-implementation-drift-auditor.md`; Git state: `?? Specs/SPEC-071-spec-implementation-drift-auditor.md`. This content is not part of the pinned branch snapshots.

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `Local working tree` — source `Specs/SPEC-071-spec-implementation-drift-auditor.md`
- path: `Scripts/Python/spec_implementation_drift_auditor.py` — source SPEC line 73.
- path: `Scripts/tests/test_spec_implementation_drift_auditor.py` — source SPEC line 74.
- path: `Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md` — source SPEC line 76.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

No changed path was established from an unambiguous SPEC-linked commit in these snapshots.

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
===== END FILE =====
===== EXCERPT: /home/gandalf/ObsidianVault/Tarefas/Dashboard.md | full_sha256=9000fb660b50bd74f2206b7c5e2d282476032ab90b1604fde089b4b261916ba4 | relevant_row_only =====
18: - [[NG-071-detector-drift-spec-implementation-v1-20261017]] — `planeada`: SPEC-071 v0.12 Draft; Astra design-only solicitado agora, limitado ao SPEC (ferramenta não existe ainda); packet SHA/consentimento pendentes. Contexto detalhado e gate pós-implementação registados; sem envio ainda.
O restante do Dashboard foi omitido porque contém tarefas não relacionadas; não se pede a Astra que avalie esse conteúdo.
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis-ng070-aiopt3/Specs/SPEC-031-recvmmsg-batched-receive.md | branch=AIOPT3 | HEAD=24a3851e1d5a90f039b9e148a1af38dd03c4d925 | full_sha256=de4241aabf0a97ca3a092c79b93fd61fe7e68ea92b528511364e3906579dbda6 | excerpt_sha256=228ee4e244849e673b936849daeb6847aa77febe5a3e34a02fc89e5d4102cc3d =====
1: # SPEC-031 — Batched RAW Receive with `recvmmsg()`
2: 
3: **Status:** Implemented — Pending Test (performance/benchmark acceptance remains unverified)
4: **Branch:** AIOPT3
5: **Implementation commit:** `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f`
6: **Scope:** One performance change: receive-side syscall batching.
7: **Sequence:** Implementation committed; verify the acceptance criteria and publish BEFORE/AFTER evidence before deployment.
8: 
9: > **Implementation notice:** Commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` implemented this receive-side change and is an ancestor of all three branch heads pinned by the 2026-10-17 audit. The `recvfrom()` path below is the pre-implementation baseline; the pinned `ReceiveDispatcher` source uses `recvmmsg()`. Source presence and implementation commit do not establish the performance acceptance criteria in §§3–6.
10: 
11: ## 1. Context and non-goals
... [omitted lines; see full-file digest below] ...
13: - Phase 1 signed off; SPEC-028-B heartbeat and SPEC-030 verified.
14: - Reference: 500 msg/s bidirectional, measured zero loss, one-way delay 0.13/0.64 ms, RSS ~2.8 MB, PGCS ~1 busy core per VM.
15: - Pre-implementation baseline: `NGAL_Transport_RAW::ReceiveDispatcher` polled the SSID collection, called one `recvfrom()` per ready socket, stripped the 14-byte Ethernet header, called `SAR.ReceiveFragment`, and invoked `DeliverToGateway` on completion. This is historical baseline context, not the current path at the pinned heads.
16: - Change only reception mechanics. Preserve framing, SAR, delivery, socket configuration, and wire format.
17: - No send batching, affinity changes, socket-buffer tuning, SAR changes, or NRNCS stale-cache fix in this benchmark.
... [omitted lines; see full-file digest below] ...
21: ### Placement and allocation
22: 
23: Replace the ready-socket `recvfrom()` operation inside `ReceiveDispatcher` with `recvmmsg()`.
24: 
25: - Batch size: **32 frames** per call.
... [omitted lines; see full-file digest below] ...
37: 
38: ```c
39: recvmmsg(fd, msgs, 32, MSG_DONTWAIT | MSG_TRUNC, NULL);
40: ```
41: 
... [omitted lines; see full-file digest below] ...
82: 8. Record per-direction offered/received/completed rates, one-way latency distribution, PGCS/dispatcher CPU, RSS peak/steady state, and relevant kernel/interface drops.
83: 9. Use the same synchronized-clock latency method as BEFORE; report clock uncertainty.
84: 10. Capture separate matched syscall-profile runs with `strace -f -c` covering `recvfrom`, `recvmmsg`, and `poll`; tracing results are not latency/throughput results.
85: 11. Normalize receive syscall counts by successfully dequeued Ethernet frames, using identical known fragmentation or independent ingress-frame counts.
86: 12. Include unsuccessful receive calls in the syscall total. Ordinary `/proc` process/I/O counters are not a per-syscall substitute.
... [omitted lines; see full-file digest below] ...
102: ## 5. musl / Alpine compatibility
103: 
104: - Linux-specific API; musl on Alpine provides `recvmmsg()` with `<sys/socket.h>` and `_GNU_SOURCE` enabled before system headers.
105: - Use `<linux/if_packet.h>` for `sockaddr_ll`; verify the target Alpine toolchain builds the declaration.
106: - Verify runtime support on the actual kernel and RAW socket type; container seccomp may deny the syscall.
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis-ng070-aiopt3/Specs/SPEC-037-promote-aiopt3-default-branch.md | branch=AIOPT3 | HEAD=24a3851e1d5a90f039b9e148a1af38dd03c4d925 | full_sha256=1f6da15d599550b80362f0b4a6e89222d97a64e36ccccc7277ddd08ef9a9a9f0 | excerpt_sha256=793dc8346b54038be8353e1caf6a70aa1f1ac80437e6d927b0f89af648f560df =====
4: **Date:** 2026-09-11
5: **Author:** Antonio Alberti + Hermes Agent
6: **Status:** Implemented
7: **Branch:** AIOPT3
8: **Implementation commit:** `5ab91ce` baseline; final promotion result recorded in `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
9: **Related:** SPEC-036-deprecate-standalone-pss-girs-hts.md; Docs/DECISIONS/SPEC-STATUS-REGISTER.md
... [omitted lines; see full-file digest below] ...
11: ## 1. Problem / Objective
12: 
13: > **Historical problem statement:** The following sentence records the remote state before the AIOPT3 promotion on 2026-09-11; it is not a claim about the current remote default setting.
14: 
15: The NovaGenesis repository still advertises `master` as its remote default branch, while current development, integration, deployment scripts, SPECs and validated work use `AIOPT3`. The repository must make `AIOPT3` the principal/default branch and retain `master` as a secondary historical and compatibility branch.
16: 
17: The promotion is allowed to proceed with explicitly accepted imperfections. Acceptance is not an assertion that AIOPT3 is perfect; every accepted imperfection must be classified, evidenced, assigned a follow-up or re-evaluation condition, and excluded from the blocking set only when its impact is understood.
... [omitted lines; see full-file digest below] ...
25: | Active integration branch | `AIOPT3` |
26: | `AIOPT3` / `origin/AIOPT3` | `b69a9ae` — `SPEC-037: define AIOPT3 default branch promotion` |
27: | `master` / `origin/master` | `e305782` — `Fix: add return 0; to GenerateSCNFromCharArrayBinaryPatterns functions to prevent SIGILL` |
28: | Remote HEAD | `origin/HEAD -> origin/master` |
29: | Divergence | `master...AIOPT3`: `0 261` — AIOPT3 is 261 commits ahead and has no commits absent from master according to the local graph |
30: | Tracked files | `master`: 584; `AIOPT3`: 638 |
31: | Working tree | AIOPT3 aligned with origin; three untracked `IO/` evidence directories are present and must not be included in branch administration changes |
32: | Deployment branch reference | `Scripts/AlpineVMs/pull-and-build-vms.sh` already uses `AIOPT3` |
... [omitted lines; see full-file digest below] ...
40: 1. **Reproducibility and traceability:** branch identity, commit baseline, accepted imperfections and rollback must be recorded so a deployment can be reproduced.
41: 2. **Explicit contracts:** the default branch must point to the branch that current build/deployment instructions actually consume.
42: 3. **Non-destructive history:** `master` remains readable and recoverable; no force-push, history rewrite or deletion is permitted.
43: 4. **Evidence over assumption:** a successful local branch rename or symbolic-ref change is insufficient; the remote default branch and relevant consumers must be read back and verified.
44: 5. **Scope separation:** this promotion must not silently alter the NovaGenesis protocol, inverted pub/sub model, payload-cache semantics or unrelated production code.
... [omitted lines; see full-file digest below] ...
50: ### 4.1 Audit and classification
51: 
52: 1. Compare `master` and `AIOPT3` by commit graph, tree, build, tests, SPECs, scripts, documentation and deployment consumers.
53: 2. Search tracked files for `master`, `origin/master`, checkout/pull instructions and stale branch assumptions.
54: 3. Classify findings as:
55:    - **Blocking:** unsafe default branch, unreproducible deployment, failed minimum validation, loss of history, or undisclosed contract-breaking change.
56:    - **Accepted with follow-up:** known issue with bounded impact, evidence and an owner/task or explicit re-evaluation condition.
57:    - **Accepted without immediate follow-up:** cosmetic or historical drift with no effect on build, deployment, protocol or user safety.
58:    - **Resolved before promotion:** corrected and verified as part of this SPEC.
59: 
... [omitted lines; see full-file digest below] ...
61: 
62: - `AIOPT3` becomes the remote default branch and principal integration branch.
63: - `master` remains a secondary branch and is not deleted, rewritten or force-updated.
64: - New development and deployment instructions target `AIOPT3`.
65: - Historical documents may retain `master` references when they describe the historical baseline; active instructions must identify the intended branch explicitly.
66: - `origin/HEAD` is refreshed locally after the remote default change, but this local symbolic ref is not treated as proof of the remote setting.
67: 
... [omitted lines; see full-file digest below] ...
71: 
72: 1. Read the remote default branch through the available repository interface and verify it is `AIOPT3`.
73: 2. Verify `master` still exists at the pre-change commit and remains fetchable.
74: 3. Clone or inspect a clean temporary checkout using the default branch and verify it resolves to AIOPT3.
75: 4. Run the minimum build/spec validation selected in the acceptance matrix.
... [omitted lines; see full-file digest below] ...
83: | Repository remote settings | Set default branch to `AIOPT3` | Make the active integration branch principal |
84: | `origin/HEAD` in local clones | Refresh symbolic default after remote change | Prevent stale local discovery |
85: | Active scripts/documentation | Replace stale `master` assumptions where operational | Align users and deployments with the new contract |
86: | Historical documents | Preserve historical `master` references, optionally annotate context | Do not rewrite evidence |
87: | `master` branch | No deletion, rewrite or force-push | Preserve rollback and historical compatibility |
88: | Untracked `IO/` evidence | No modification or inclusion | Protect experimental evidence from branch administration |
89: 
90: ## 6. Acceptance Criteria
91: 
92: 1. A comparison report identifies the exact commits, graph relationship, tree differences and relevant operational differences between `master` and `AIOPT3`.
93: 2. The accepted-imperfection register exists and each item has classification, impact, evidence, owner/follow-up or re-evaluation condition.
94: 3. No blocking item remains undisclosed at promotion time.
95: 4. The repository remote reports `AIOPT3` as its default branch.
96: 5. `master` remains present and unchanged at its pre-promotion commit unless a separately approved change is made later.
97: 6. A clean default-branch checkout resolves to AIOPT3 and can run the selected minimum validation.
98: 7. Active build/deployment instructions use AIOPT3; historical references are explicitly historical.
99: 8. `Scripts/Simple/validate-specs.sh` and the selected build/test checks pass, or failures are listed as accepted imperfections with evidence and a follow-up.
100: 9. No source, protocol, pub/sub, cache or runtime behaviour is changed solely to perform this branch promotion.
101: 10. Rollback is documented as a remote default-branch change back to `master`; no force-push is required.
102: 
103: ## 7. Minimum Validation Matrix
... [omitted lines; see full-file digest below] ...
111: | Build baseline | Selected build completes or known failure is classified | Blocking for unexplained build failure |
112: | Runtime/integration tests | Existing accepted baseline reused where valid | Do not claim new coverage; record deferred tests |
113: | Remote branch read-back | Default is AIOPT3; master still exists | Blocking |
114: | Rollback rehearsal/design review | Reversible without history rewrite | Blocking |
115: 
... [omitted lines; see full-file digest below] ...
118: If the promoted default causes an undisclosed critical problem:
119: 
120: 1. Restore the remote default branch setting to `master` through the repository administration interface.
121: 2. Refresh `origin/HEAD` in affected local clones.
122: 3. Do not rewrite, delete or force-push either branch.
... [omitted lines; see full-file digest below] ...
131: 
132: - AIOPT3 contains deferred validation gates, including the normal cross-process runtime validation recorded by NG-036.
133: - The SPEC register and historical documentation contain legacy/stale references that may require classification rather than wholesale rewriting.
134: - `master` has a smaller/older tree and is not a current deployment baseline.
135: - The working tree contains untracked IO evidence directories that are outside this change.
136: - Remote branch protection/default-branch settings may require administrative access not available from the local checkout.
... [omitted lines; see full-file digest below] ...
142: | # | Decision | Date | Reason |
143: |---|---|---|---|
144: | D1 | Promote AIOPT3 rather than merge it into master | 2026-09-11 | AIOPT3 is the active integration/deployment branch and is 261 commits ahead of master |
145: | D2 | Retain master as a secondary branch | 2026-09-11 | Preserve history, compatibility and rollback |
146: | D3 | Permit bounded, explicit imperfections | 2026-09-11 | Avoid blocking the branch promotion on unrelated or non-critical unfinished work |
147: | D4 | No source/protocol change in this SPEC | 2026-09-11 | Keep branch administration separate from production implementation |
148: | D5 | Promote AIOPT3 after corrected 100-photo evidence and remote read-back | 2026-09-11 | Runtime and administration gates passed; master remains preserved |
149: 
150: ## 11. Implementation Notes
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis-ng070-aiopt3/Specs/SPEC-054-release-master-plan-and-audit.md | branch=AIOPT3 | HEAD=24a3851e1d5a90f039b9e148a1af38dd03c4d925 | full_sha256=760e552e782d525b475a2e843a0ae72f21d60aeecae74b98b03f185a39c4f40b | excerpt_sha256=f64bb814d16427585aaba92880289d5412ea4fd659d650ca0f4a23e5024e22e4 =====
3: **Author:** Antonio Alberti / Hermes Agent  
4: **Date:** 2026-09-13  
5: **Status:** In Progress  
6: **Branch:** AIOPT3  
7: **Implementation commit:** —  
8: **Related:** SPEC-038-public-repository-hygiene.md; SPEC-044-core-run-evaluate-invalid-pg-downcast.md; SPEC-045-core-evaluate-functional-harness.md; SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-047-pgcs-readiness-periodic-activation.md; SPEC-048-pgcs-receiver-processing-evidence.md; SPEC-049-nrncs-readiness-binding-oracles.md; SPEC-050-contentapp-repository-control-plane-readback.md; SPEC-051-cross-vm-nrncs-discovery-diagnosis.md; SPEC-055-ng-elc-local-mode.md; SPEC-056-ng-elc-evidence-provenance-hardening.md; SPEC-057-ng-elc-canonical-entrypoint-rename.md
9: 
10: > **Historical scope notice:** This is an AIOPT3 release-plan snapshot dated 2026-09-13. The branch and remote-ref statements below are historical observations, not a current remote read-back or AIOPT4/AIOPT5 authority.
11: 
12: **Canonical experiment tool:** `NG Experiment Lifecycle Controller (NG-ELC)` — current implementation `Scripts/AlpineVMs/ng_remote_executor.py`; canonical rename proposed by SPEC-057: `Scripts/AlpineVMs/ng_elc.py`
13: 
14: **Branch history:** `AIOPT1` and `AIOPT2` are deleted from the remote. Existing mentions are historical; this clone still has stale local/tracking refs for `AIOPT1`, while active release work remains exclusively on `AIOPT3`.
15: 
16: ## 1. Problema
... [omitted lines; see full-file digest below] ...
56: - implementação de optimizações NG-041, NG-043, NG-044 e NG-045;
57: - alterações experimentais de performance no candidato;
58: - criação ou promoção de AIOPT4;
59: - qualquer gate não relacionado com o âmbito acima, salvo decisão explícita.
60: 
... [omitted lines; see full-file digest below] ...
148: - [x] A ferramenta permanece read-only por omissão.
149: - [x] O plano descreve o procedimento de início/fecho de sessão para novos agentes.
150: - [x] AIOPT4 e as optimizações permanecem fora do âmbito da release.
151: - [ ] Astra revê o plano, a ferramenta e o primeiro relatório antes de usar os resultados para aceitar um gate.
152: 
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis-ng070-aiopt4/Specs/SPEC-031-recvmmsg-batched-receive.md | branch=AIOPT4 | HEAD=0dd0dd4b2fd0a9d416aff232ad07672fe39746b6 | full_sha256=de4241aabf0a97ca3a092c79b93fd61fe7e68ea92b528511364e3906579dbda6 | excerpt_sha256=228ee4e244849e673b936849daeb6847aa77febe5a3e34a02fc89e5d4102cc3d =====
1: # SPEC-031 — Batched RAW Receive with `recvmmsg()`
2: 
3: **Status:** Implemented — Pending Test (performance/benchmark acceptance remains unverified)
4: **Branch:** AIOPT3
5: **Implementation commit:** `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f`
6: **Scope:** One performance change: receive-side syscall batching.
7: **Sequence:** Implementation committed; verify the acceptance criteria and publish BEFORE/AFTER evidence before deployment.
8: 
9: > **Implementation notice:** Commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` implemented this receive-side change and is an ancestor of all three branch heads pinned by the 2026-10-17 audit. The `recvfrom()` path below is the pre-implementation baseline; the pinned `ReceiveDispatcher` source uses `recvmmsg()`. Source presence and implementation commit do not establish the performance acceptance criteria in §§3–6.
10: 
11: ## 1. Context and non-goals
... [omitted lines; see full-file digest below] ...
13: - Phase 1 signed off; SPEC-028-B heartbeat and SPEC-030 verified.
14: - Reference: 500 msg/s bidirectional, measured zero loss, one-way delay 0.13/0.64 ms, RSS ~2.8 MB, PGCS ~1 busy core per VM.
15: - Pre-implementation baseline: `NGAL_Transport_RAW::ReceiveDispatcher` polled the SSID collection, called one `recvfrom()` per ready socket, stripped the 14-byte Ethernet header, called `SAR.ReceiveFragment`, and invoked `DeliverToGateway` on completion. This is historical baseline context, not the current path at the pinned heads.
16: - Change only reception mechanics. Preserve framing, SAR, delivery, socket configuration, and wire format.
17: - No send batching, affinity changes, socket-buffer tuning, SAR changes, or NRNCS stale-cache fix in this benchmark.
... [omitted lines; see full-file digest below] ...
21: ### Placement and allocation
22: 
23: Replace the ready-socket `recvfrom()` operation inside `ReceiveDispatcher` with `recvmmsg()`.
24: 
25: - Batch size: **32 frames** per call.
... [omitted lines; see full-file digest below] ...
37: 
38: ```c
39: recvmmsg(fd, msgs, 32, MSG_DONTWAIT | MSG_TRUNC, NULL);
40: ```
41: 
... [omitted lines; see full-file digest below] ...
82: 8. Record per-direction offered/received/completed rates, one-way latency distribution, PGCS/dispatcher CPU, RSS peak/steady state, and relevant kernel/interface drops.
83: 9. Use the same synchronized-clock latency method as BEFORE; report clock uncertainty.
84: 10. Capture separate matched syscall-profile runs with `strace -f -c` covering `recvfrom`, `recvmmsg`, and `poll`; tracing results are not latency/throughput results.
85: 11. Normalize receive syscall counts by successfully dequeued Ethernet frames, using identical known fragmentation or independent ingress-frame counts.
86: 12. Include unsuccessful receive calls in the syscall total. Ordinary `/proc` process/I/O counters are not a per-syscall substitute.
... [omitted lines; see full-file digest below] ...
102: ## 5. musl / Alpine compatibility
103: 
104: - Linux-specific API; musl on Alpine provides `recvmmsg()` with `<sys/socket.h>` and `_GNU_SOURCE` enabled before system headers.
105: - Use `<linux/if_packet.h>` for `sockaddr_ll`; verify the target Alpine toolchain builds the declaration.
106: - Verify runtime support on the actual kernel and RAW socket type; container seccomp may deny the syscall.
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis-ng070-aiopt4/Specs/SPEC-037-promote-aiopt3-default-branch.md | branch=AIOPT4 | HEAD=0dd0dd4b2fd0a9d416aff232ad07672fe39746b6 | full_sha256=1f6da15d599550b80362f0b4a6e89222d97a64e36ccccc7277ddd08ef9a9a9f0 | excerpt_sha256=793dc8346b54038be8353e1caf6a70aa1f1ac80437e6d927b0f89af648f560df =====
4: **Date:** 2026-09-11
5: **Author:** Antonio Alberti + Hermes Agent
6: **Status:** Implemented
7: **Branch:** AIOPT3
8: **Implementation commit:** `5ab91ce` baseline; final promotion result recorded in `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
9: **Related:** SPEC-036-deprecate-standalone-pss-girs-hts.md; Docs/DECISIONS/SPEC-STATUS-REGISTER.md
... [omitted lines; see full-file digest below] ...
11: ## 1. Problem / Objective
12: 
13: > **Historical problem statement:** The following sentence records the remote state before the AIOPT3 promotion on 2026-09-11; it is not a claim about the current remote default setting.
14: 
15: The NovaGenesis repository still advertises `master` as its remote default branch, while current development, integration, deployment scripts, SPECs and validated work use `AIOPT3`. The repository must make `AIOPT3` the principal/default branch and retain `master` as a secondary historical and compatibility branch.
16: 
17: The promotion is allowed to proceed with explicitly accepted imperfections. Acceptance is not an assertion that AIOPT3 is perfect; every accepted imperfection must be classified, evidenced, assigned a follow-up or re-evaluation condition, and excluded from the blocking set only when its impact is understood.
... [omitted lines; see full-file digest below] ...
25: | Active integration branch | `AIOPT3` |
26: | `AIOPT3` / `origin/AIOPT3` | `b69a9ae` — `SPEC-037: define AIOPT3 default branch promotion` |
27: | `master` / `origin/master` | `e305782` — `Fix: add return 0; to GenerateSCNFromCharArrayBinaryPatterns functions to prevent SIGILL` |
28: | Remote HEAD | `origin/HEAD -> origin/master` |
29: | Divergence | `master...AIOPT3`: `0 261` — AIOPT3 is 261 commits ahead and has no commits absent from master according to the local graph |
30: | Tracked files | `master`: 584; `AIOPT3`: 638 |
31: | Working tree | AIOPT3 aligned with origin; three untracked `IO/` evidence directories are present and must not be included in branch administration changes |
32: | Deployment branch reference | `Scripts/AlpineVMs/pull-and-build-vms.sh` already uses `AIOPT3` |
... [omitted lines; see full-file digest below] ...
40: 1. **Reproducibility and traceability:** branch identity, commit baseline, accepted imperfections and rollback must be recorded so a deployment can be reproduced.
41: 2. **Explicit contracts:** the default branch must point to the branch that current build/deployment instructions actually consume.
42: 3. **Non-destructive history:** `master` remains readable and recoverable; no force-push, history rewrite or deletion is permitted.
43: 4. **Evidence over assumption:** a successful local branch rename or symbolic-ref change is insufficient; the remote default branch and relevant consumers must be read back and verified.
44: 5. **Scope separation:** this promotion must not silently alter the NovaGenesis protocol, inverted pub/sub model, payload-cache semantics or unrelated production code.
... [omitted lines; see full-file digest below] ...
50: ### 4.1 Audit and classification
51: 
52: 1. Compare `master` and `AIOPT3` by commit graph, tree, build, tests, SPECs, scripts, documentation and deployment consumers.
53: 2. Search tracked files for `master`, `origin/master`, checkout/pull instructions and stale branch assumptions.
54: 3. Classify findings as:
55:    - **Blocking:** unsafe default branch, unreproducible deployment, failed minimum validation, loss of history, or undisclosed contract-breaking change.
56:    - **Accepted with follow-up:** known issue with bounded impact, evidence and an owner/task or explicit re-evaluation condition.
57:    - **Accepted without immediate follow-up:** cosmetic or historical drift with no effect on build, deployment, protocol or user safety.
58:    - **Resolved before promotion:** corrected and verified as part of this SPEC.
59: 
... [omitted lines; see full-file digest below] ...
61: 
62: - `AIOPT3` becomes the remote default branch and principal integration branch.
63: - `master` remains a secondary branch and is not deleted, rewritten or force-updated.
64: - New development and deployment instructions target `AIOPT3`.
65: - Historical documents may retain `master` references when they describe the historical baseline; active instructions must identify the intended branch explicitly.
66: - `origin/HEAD` is refreshed locally after the remote default change, but this local symbolic ref is not treated as proof of the remote setting.
67: 
... [omitted lines; see full-file digest below] ...
71: 
72: 1. Read the remote default branch through the available repository interface and verify it is `AIOPT3`.
73: 2. Verify `master` still exists at the pre-change commit and remains fetchable.
74: 3. Clone or inspect a clean temporary checkout using the default branch and verify it resolves to AIOPT3.
75: 4. Run the minimum build/spec validation selected in the acceptance matrix.
... [omitted lines; see full-file digest below] ...
83: | Repository remote settings | Set default branch to `AIOPT3` | Make the active integration branch principal |
84: | `origin/HEAD` in local clones | Refresh symbolic default after remote change | Prevent stale local discovery |
85: | Active scripts/documentation | Replace stale `master` assumptions where operational | Align users and deployments with the new contract |
86: | Historical documents | Preserve historical `master` references, optionally annotate context | Do not rewrite evidence |
87: | `master` branch | No deletion, rewrite or force-push | Preserve rollback and historical compatibility |
88: | Untracked `IO/` evidence | No modification or inclusion | Protect experimental evidence from branch administration |
89: 
90: ## 6. Acceptance Criteria
91: 
92: 1. A comparison report identifies the exact commits, graph relationship, tree differences and relevant operational differences between `master` and `AIOPT3`.
93: 2. The accepted-imperfection register exists and each item has classification, impact, evidence, owner/follow-up or re-evaluation condition.
94: 3. No blocking item remains undisclosed at promotion time.
95: 4. The repository remote reports `AIOPT3` as its default branch.
96: 5. `master` remains present and unchanged at its pre-promotion commit unless a separately approved change is made later.
97: 6. A clean default-branch checkout resolves to AIOPT3 and can run the selected minimum validation.
98: 7. Active build/deployment instructions use AIOPT3; historical references are explicitly historical.
99: 8. `Scripts/Simple/validate-specs.sh` and the selected build/test checks pass, or failures are listed as accepted imperfections with evidence and a follow-up.
100: 9. No source, protocol, pub/sub, cache or runtime behaviour is changed solely to perform this branch promotion.
101: 10. Rollback is documented as a remote default-branch change back to `master`; no force-push is required.
102: 
103: ## 7. Minimum Validation Matrix
... [omitted lines; see full-file digest below] ...
111: | Build baseline | Selected build completes or known failure is classified | Blocking for unexplained build failure |
112: | Runtime/integration tests | Existing accepted baseline reused where valid | Do not claim new coverage; record deferred tests |
113: | Remote branch read-back | Default is AIOPT3; master still exists | Blocking |
114: | Rollback rehearsal/design review | Reversible without history rewrite | Blocking |
115: 
... [omitted lines; see full-file digest below] ...
118: If the promoted default causes an undisclosed critical problem:
119: 
120: 1. Restore the remote default branch setting to `master` through the repository administration interface.
121: 2. Refresh `origin/HEAD` in affected local clones.
122: 3. Do not rewrite, delete or force-push either branch.
... [omitted lines; see full-file digest below] ...
131: 
132: - AIOPT3 contains deferred validation gates, including the normal cross-process runtime validation recorded by NG-036.
133: - The SPEC register and historical documentation contain legacy/stale references that may require classification rather than wholesale rewriting.
134: - `master` has a smaller/older tree and is not a current deployment baseline.
135: - The working tree contains untracked IO evidence directories that are outside this change.
136: - Remote branch protection/default-branch settings may require administrative access not available from the local checkout.
... [omitted lines; see full-file digest below] ...
142: | # | Decision | Date | Reason |
143: |---|---|---|---|
144: | D1 | Promote AIOPT3 rather than merge it into master | 2026-09-11 | AIOPT3 is the active integration/deployment branch and is 261 commits ahead of master |
145: | D2 | Retain master as a secondary branch | 2026-09-11 | Preserve history, compatibility and rollback |
146: | D3 | Permit bounded, explicit imperfections | 2026-09-11 | Avoid blocking the branch promotion on unrelated or non-critical unfinished work |
147: | D4 | No source/protocol change in this SPEC | 2026-09-11 | Keep branch administration separate from production implementation |
148: | D5 | Promote AIOPT3 after corrected 100-photo evidence and remote read-back | 2026-09-11 | Runtime and administration gates passed; master remains preserved |
149: 
150: ## 11. Implementation Notes
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis-ng070-aiopt4/Specs/SPEC-054-release-master-plan-and-audit.md | branch=AIOPT4 | HEAD=0dd0dd4b2fd0a9d416aff232ad07672fe39746b6 | full_sha256=760e552e782d525b475a2e843a0ae72f21d60aeecae74b98b03f185a39c4f40b | excerpt_sha256=f64bb814d16427585aaba92880289d5412ea4fd659d650ca0f4a23e5024e22e4 =====
3: **Author:** Antonio Alberti / Hermes Agent  
4: **Date:** 2026-09-13  
5: **Status:** In Progress  
6: **Branch:** AIOPT3  
7: **Implementation commit:** —  
8: **Related:** SPEC-038-public-repository-hygiene.md; SPEC-044-core-run-evaluate-invalid-pg-downcast.md; SPEC-045-core-evaluate-functional-harness.md; SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-047-pgcs-readiness-periodic-activation.md; SPEC-048-pgcs-receiver-processing-evidence.md; SPEC-049-nrncs-readiness-binding-oracles.md; SPEC-050-contentapp-repository-control-plane-readback.md; SPEC-051-cross-vm-nrncs-discovery-diagnosis.md; SPEC-055-ng-elc-local-mode.md; SPEC-056-ng-elc-evidence-provenance-hardening.md; SPEC-057-ng-elc-canonical-entrypoint-rename.md
9: 
10: > **Historical scope notice:** This is an AIOPT3 release-plan snapshot dated 2026-09-13. The branch and remote-ref statements below are historical observations, not a current remote read-back or AIOPT4/AIOPT5 authority.
11: 
12: **Canonical experiment tool:** `NG Experiment Lifecycle Controller (NG-ELC)` — current implementation `Scripts/AlpineVMs/ng_remote_executor.py`; canonical rename proposed by SPEC-057: `Scripts/AlpineVMs/ng_elc.py`
13: 
14: **Branch history:** `AIOPT1` and `AIOPT2` are deleted from the remote. Existing mentions are historical; this clone still has stale local/tracking refs for `AIOPT1`, while active release work remains exclusively on `AIOPT3`.
15: 
16: ## 1. Problema
... [omitted lines; see full-file digest below] ...
56: - implementação de optimizações NG-041, NG-043, NG-044 e NG-045;
57: - alterações experimentais de performance no candidato;
58: - criação ou promoção de AIOPT4;
59: - qualquer gate não relacionado com o âmbito acima, salvo decisão explícita.
60: 
... [omitted lines; see full-file digest below] ...
148: - [x] A ferramenta permanece read-only por omissão.
149: - [x] O plano descreve o procedimento de início/fecho de sessão para novos agentes.
150: - [x] AIOPT4 e as optimizações permanecem fora do âmbito da release.
151: - [ ] Astra revê o plano, a ferramenta e o primeiro relatório antes de usar os resultados para aceitar um gate.
152: 
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis/Specs/SPEC-031-recvmmsg-batched-receive.md | branch=AIOPT5 | HEAD=8d2386dcb10d2b37f133d4b24c2cb96761753d82 | full_sha256=de4241aabf0a97ca3a092c79b93fd61fe7e68ea92b528511364e3906579dbda6 | excerpt_sha256=228ee4e244849e673b936849daeb6847aa77febe5a3e34a02fc89e5d4102cc3d =====
1: # SPEC-031 — Batched RAW Receive with `recvmmsg()`
2: 
3: **Status:** Implemented — Pending Test (performance/benchmark acceptance remains unverified)
4: **Branch:** AIOPT3
5: **Implementation commit:** `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f`
6: **Scope:** One performance change: receive-side syscall batching.
7: **Sequence:** Implementation committed; verify the acceptance criteria and publish BEFORE/AFTER evidence before deployment.
8: 
9: > **Implementation notice:** Commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` implemented this receive-side change and is an ancestor of all three branch heads pinned by the 2026-10-17 audit. The `recvfrom()` path below is the pre-implementation baseline; the pinned `ReceiveDispatcher` source uses `recvmmsg()`. Source presence and implementation commit do not establish the performance acceptance criteria in §§3–6.
10: 
11: ## 1. Context and non-goals
... [omitted lines; see full-file digest below] ...
13: - Phase 1 signed off; SPEC-028-B heartbeat and SPEC-030 verified.
14: - Reference: 500 msg/s bidirectional, measured zero loss, one-way delay 0.13/0.64 ms, RSS ~2.8 MB, PGCS ~1 busy core per VM.
15: - Pre-implementation baseline: `NGAL_Transport_RAW::ReceiveDispatcher` polled the SSID collection, called one `recvfrom()` per ready socket, stripped the 14-byte Ethernet header, called `SAR.ReceiveFragment`, and invoked `DeliverToGateway` on completion. This is historical baseline context, not the current path at the pinned heads.
16: - Change only reception mechanics. Preserve framing, SAR, delivery, socket configuration, and wire format.
17: - No send batching, affinity changes, socket-buffer tuning, SAR changes, or NRNCS stale-cache fix in this benchmark.
... [omitted lines; see full-file digest below] ...
21: ### Placement and allocation
22: 
23: Replace the ready-socket `recvfrom()` operation inside `ReceiveDispatcher` with `recvmmsg()`.
24: 
25: - Batch size: **32 frames** per call.
... [omitted lines; see full-file digest below] ...
37: 
38: ```c
39: recvmmsg(fd, msgs, 32, MSG_DONTWAIT | MSG_TRUNC, NULL);
40: ```
41: 
... [omitted lines; see full-file digest below] ...
82: 8. Record per-direction offered/received/completed rates, one-way latency distribution, PGCS/dispatcher CPU, RSS peak/steady state, and relevant kernel/interface drops.
83: 9. Use the same synchronized-clock latency method as BEFORE; report clock uncertainty.
84: 10. Capture separate matched syscall-profile runs with `strace -f -c` covering `recvfrom`, `recvmmsg`, and `poll`; tracing results are not latency/throughput results.
85: 11. Normalize receive syscall counts by successfully dequeued Ethernet frames, using identical known fragmentation or independent ingress-frame counts.
86: 12. Include unsuccessful receive calls in the syscall total. Ordinary `/proc` process/I/O counters are not a per-syscall substitute.
... [omitted lines; see full-file digest below] ...
102: ## 5. musl / Alpine compatibility
103: 
104: - Linux-specific API; musl on Alpine provides `recvmmsg()` with `<sys/socket.h>` and `_GNU_SOURCE` enabled before system headers.
105: - Use `<linux/if_packet.h>` for `sockaddr_ll`; verify the target Alpine toolchain builds the declaration.
106: - Verify runtime support on the actual kernel and RAW socket type; container seccomp may deny the syscall.
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis/Specs/SPEC-037-promote-aiopt3-default-branch.md | branch=AIOPT5 | HEAD=8d2386dcb10d2b37f133d4b24c2cb96761753d82 | full_sha256=1f6da15d599550b80362f0b4a6e89222d97a64e36ccccc7277ddd08ef9a9a9f0 | excerpt_sha256=793dc8346b54038be8353e1caf6a70aa1f1ac80437e6d927b0f89af648f560df =====
4: **Date:** 2026-09-11
5: **Author:** Antonio Alberti + Hermes Agent
6: **Status:** Implemented
7: **Branch:** AIOPT3
8: **Implementation commit:** `5ab91ce` baseline; final promotion result recorded in `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
9: **Related:** SPEC-036-deprecate-standalone-pss-girs-hts.md; Docs/DECISIONS/SPEC-STATUS-REGISTER.md
... [omitted lines; see full-file digest below] ...
11: ## 1. Problem / Objective
12: 
13: > **Historical problem statement:** The following sentence records the remote state before the AIOPT3 promotion on 2026-09-11; it is not a claim about the current remote default setting.
14: 
15: The NovaGenesis repository still advertises `master` as its remote default branch, while current development, integration, deployment scripts, SPECs and validated work use `AIOPT3`. The repository must make `AIOPT3` the principal/default branch and retain `master` as a secondary historical and compatibility branch.
16: 
17: The promotion is allowed to proceed with explicitly accepted imperfections. Acceptance is not an assertion that AIOPT3 is perfect; every accepted imperfection must be classified, evidenced, assigned a follow-up or re-evaluation condition, and excluded from the blocking set only when its impact is understood.
... [omitted lines; see full-file digest below] ...
25: | Active integration branch | `AIOPT3` |
26: | `AIOPT3` / `origin/AIOPT3` | `b69a9ae` — `SPEC-037: define AIOPT3 default branch promotion` |
27: | `master` / `origin/master` | `e305782` — `Fix: add return 0; to GenerateSCNFromCharArrayBinaryPatterns functions to prevent SIGILL` |
28: | Remote HEAD | `origin/HEAD -> origin/master` |
29: | Divergence | `master...AIOPT3`: `0 261` — AIOPT3 is 261 commits ahead and has no commits absent from master according to the local graph |
30: | Tracked files | `master`: 584; `AIOPT3`: 638 |
31: | Working tree | AIOPT3 aligned with origin; three untracked `IO/` evidence directories are present and must not be included in branch administration changes |
32: | Deployment branch reference | `Scripts/AlpineVMs/pull-and-build-vms.sh` already uses `AIOPT3` |
... [omitted lines; see full-file digest below] ...
40: 1. **Reproducibility and traceability:** branch identity, commit baseline, accepted imperfections and rollback must be recorded so a deployment can be reproduced.
41: 2. **Explicit contracts:** the default branch must point to the branch that current build/deployment instructions actually consume.
42: 3. **Non-destructive history:** `master` remains readable and recoverable; no force-push, history rewrite or deletion is permitted.
43: 4. **Evidence over assumption:** a successful local branch rename or symbolic-ref change is insufficient; the remote default branch and relevant consumers must be read back and verified.
44: 5. **Scope separation:** this promotion must not silently alter the NovaGenesis protocol, inverted pub/sub model, payload-cache semantics or unrelated production code.
... [omitted lines; see full-file digest below] ...
50: ### 4.1 Audit and classification
51: 
52: 1. Compare `master` and `AIOPT3` by commit graph, tree, build, tests, SPECs, scripts, documentation and deployment consumers.
53: 2. Search tracked files for `master`, `origin/master`, checkout/pull instructions and stale branch assumptions.
54: 3. Classify findings as:
55:    - **Blocking:** unsafe default branch, unreproducible deployment, failed minimum validation, loss of history, or undisclosed contract-breaking change.
56:    - **Accepted with follow-up:** known issue with bounded impact, evidence and an owner/task or explicit re-evaluation condition.
57:    - **Accepted without immediate follow-up:** cosmetic or historical drift with no effect on build, deployment, protocol or user safety.
58:    - **Resolved before promotion:** corrected and verified as part of this SPEC.
59: 
... [omitted lines; see full-file digest below] ...
61: 
62: - `AIOPT3` becomes the remote default branch and principal integration branch.
63: - `master` remains a secondary branch and is not deleted, rewritten or force-updated.
64: - New development and deployment instructions target `AIOPT3`.
65: - Historical documents may retain `master` references when they describe the historical baseline; active instructions must identify the intended branch explicitly.
66: - `origin/HEAD` is refreshed locally after the remote default change, but this local symbolic ref is not treated as proof of the remote setting.
67: 
... [omitted lines; see full-file digest below] ...
71: 
72: 1. Read the remote default branch through the available repository interface and verify it is `AIOPT3`.
73: 2. Verify `master` still exists at the pre-change commit and remains fetchable.
74: 3. Clone or inspect a clean temporary checkout using the default branch and verify it resolves to AIOPT3.
75: 4. Run the minimum build/spec validation selected in the acceptance matrix.
... [omitted lines; see full-file digest below] ...
83: | Repository remote settings | Set default branch to `AIOPT3` | Make the active integration branch principal |
84: | `origin/HEAD` in local clones | Refresh symbolic default after remote change | Prevent stale local discovery |
85: | Active scripts/documentation | Replace stale `master` assumptions where operational | Align users and deployments with the new contract |
86: | Historical documents | Preserve historical `master` references, optionally annotate context | Do not rewrite evidence |
87: | `master` branch | No deletion, rewrite or force-push | Preserve rollback and historical compatibility |
88: | Untracked `IO/` evidence | No modification or inclusion | Protect experimental evidence from branch administration |
89: 
90: ## 6. Acceptance Criteria
91: 
92: 1. A comparison report identifies the exact commits, graph relationship, tree differences and relevant operational differences between `master` and `AIOPT3`.
93: 2. The accepted-imperfection register exists and each item has classification, impact, evidence, owner/follow-up or re-evaluation condition.
94: 3. No blocking item remains undisclosed at promotion time.
95: 4. The repository remote reports `AIOPT3` as its default branch.
96: 5. `master` remains present and unchanged at its pre-promotion commit unless a separately approved change is made later.
97: 6. A clean default-branch checkout resolves to AIOPT3 and can run the selected minimum validation.
98: 7. Active build/deployment instructions use AIOPT3; historical references are explicitly historical.
99: 8. `Scripts/Simple/validate-specs.sh` and the selected build/test checks pass, or failures are listed as accepted imperfections with evidence and a follow-up.
100: 9. No source, protocol, pub/sub, cache or runtime behaviour is changed solely to perform this branch promotion.
101: 10. Rollback is documented as a remote default-branch change back to `master`; no force-push is required.
102: 
103: ## 7. Minimum Validation Matrix
... [omitted lines; see full-file digest below] ...
111: | Build baseline | Selected build completes or known failure is classified | Blocking for unexplained build failure |
112: | Runtime/integration tests | Existing accepted baseline reused where valid | Do not claim new coverage; record deferred tests |
113: | Remote branch read-back | Default is AIOPT3; master still exists | Blocking |
114: | Rollback rehearsal/design review | Reversible without history rewrite | Blocking |
115: 
... [omitted lines; see full-file digest below] ...
118: If the promoted default causes an undisclosed critical problem:
119: 
120: 1. Restore the remote default branch setting to `master` through the repository administration interface.
121: 2. Refresh `origin/HEAD` in affected local clones.
122: 3. Do not rewrite, delete or force-push either branch.
... [omitted lines; see full-file digest below] ...
131: 
132: - AIOPT3 contains deferred validation gates, including the normal cross-process runtime validation recorded by NG-036.
133: - The SPEC register and historical documentation contain legacy/stale references that may require classification rather than wholesale rewriting.
134: - `master` has a smaller/older tree and is not a current deployment baseline.
135: - The working tree contains untracked IO evidence directories that are outside this change.
136: - Remote branch protection/default-branch settings may require administrative access not available from the local checkout.
... [omitted lines; see full-file digest below] ...
142: | # | Decision | Date | Reason |
143: |---|---|---|---|
144: | D1 | Promote AIOPT3 rather than merge it into master | 2026-09-11 | AIOPT3 is the active integration/deployment branch and is 261 commits ahead of master |
145: | D2 | Retain master as a secondary branch | 2026-09-11 | Preserve history, compatibility and rollback |
146: | D3 | Permit bounded, explicit imperfections | 2026-09-11 | Avoid blocking the branch promotion on unrelated or non-critical unfinished work |
147: | D4 | No source/protocol change in this SPEC | 2026-09-11 | Keep branch administration separate from production implementation |
148: | D5 | Promote AIOPT3 after corrected 100-photo evidence and remote read-back | 2026-09-11 | Runtime and administration gates passed; master remains preserved |
149: 
150: ## 11. Implementation Notes
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis/Specs/SPEC-069-ng039-public-evidence-deduplication.md | branch=AIOPT5 | HEAD=8d2386dcb10d2b37f133d4b24c2cb96761753d82 | full_sha256=ffa4adb2dd1354dac2368b61af66a8bfff502c110573eccdc03967a2a17b1135 | excerpt_sha256=e495fea62f5798083f145f998f08ba687592a09edc89f034396990a02dc9de26 =====
4: **Date:** 2026-10-17
5: **Author:** Antonio Alberti / Hermes Agent
6: **Status:** Implemented
7: **Branch:** AIOPT5
8: **Baseline commit:** `ac357849ed7109c0e13ea3faf809b01dc9645694`
... [omitted lines; see full-file digest below] ...
12: ## 1. Problem and objective
13: 
14: `Specs/RESULTS-SPEC-039/` contains identical copies of raw Astra response streams under both `*.raw.sse` and session-local `*.last.sse` names. The duplicate files increase the public working-tree footprint without adding distinct bytes of evidence. This bounded change removes only the 11 unreferenced session-local copies listed below; it retains every canonical `*.raw.sse` byte-for-byte.
15: 
16: ## 2. Scope and authority
... [omitted lines; see full-file digest below] ...
18: The owner requested creation and implementation of this task on 2026-10-17, then explicitly authorised proceeding with publication. This authorises one normal commit/push of the exact allowlist and this SPEC to AIOPT5. It does not authorise public history rewriting, deletion of any other records, or changes to D2/D3 evidence.
19: 
20: Included: deletion of the 11 listed `*.last.sse` paths after rechecking exact byte equality and references; retain the matching `*.raw.sse` files and all other files.
21: 
22: Excluded: all other duplicate-content groups, including named fixtures and submitted packet copies; archival/migration; compression or rewriting of raw responses; history rewriting/force-push; unrelated pre-existing working-tree changes.
23: 
24: Exception retained: `Specs/RESULTS-SPEC-039/astra-session-v1.14-r12-20261013/ng039-v114-r12-documentary-review.last.sse` is byte-identical to its `.raw.sse` counterpart but is explicitly named by the immutable `raw_sse_path_in_session` field in `astra-review-v1.14-r12-20261013-submission.json`. Keep both paths; editing that submission receipt is outside this bounded cleanup.
25: 
26: ## 3. Exact deletion allowlist
... [omitted lines; see full-file digest below] ...
30: | Delete this exact path | Retain this exact path | SHA-256 | Bytes |
31: |---|---|---|---:|
32: | `Specs/RESULTS-SPEC-039/astra-session-r07-20261010/ng039-r07-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r07-20261010.raw.sse` | `adf9bca11b98f5188818ce013b03a9c778f406f5b9996a0fffc6a1a29d2725fa` | 310167 |
33: | `Specs/RESULTS-SPEC-039/astra-session-r07-b1b2-20261011/ng039-r07-b1b2-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r07-b1b2-20261011.raw.sse` | `e767364f1a0eaf475fe5f1fb9a2aa2c108d70f08a6c978cc83646d24b3fe1bef` | 157378 |
34: | `Specs/RESULTS-SPEC-039/astra-session-r08-followup-20261012/ng039-r08-followup-agent-reconciliation.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r08-followup-agent-reconciliation-20261012.raw.sse` | `fa3348e5a680811772bd5c472cc24feff55c2b080a09f8b6e50e451475ea084d` | 298206 |
35: | `Specs/RESULTS-SPEC-039/astra-session-r08-host-controlled-20261012/ng039-r08-host-controlled-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-r08-host-controlled-20261012.raw.sse` | `7913242dff66e39015e81cbdc0a00c2c375a57927f0baad920902a3894dcabae` | 384640 |
36: | `Specs/RESULTS-SPEC-039/astra-session-v1.12-r09-20261012/ng039-v112-r09-b2b3-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.12-r09-20261012.raw.sse` | `93f78263662d085a45fde57ea4377c8420b381c958f556a0a80f6a70a089f80e` | 287144 |
37: | `Specs/RESULTS-SPEC-039/astra-session-v1.13-r10-20261012/ng039-v113-r10-finalizer-boundary-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.raw.sse` | `4c35170e7d37953daf9bbc7082e0e92cd73b6b45067521a151c3e7654d6f2704` | 242173 |
38: | `Specs/RESULTS-SPEC-039/astra-session-v1.14-r11-20261012/ng039-v114-r11-abnormal-finalization-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.raw.sse` | `5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb` | 310926 |
39: | `Specs/RESULTS-SPEC-039/astra-session-v1.15-r13-20261013/ng039-v115-r13-documentary-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.15-r13-20261013.raw.sse` | `b819bf078d2690fdfbb945e123873d66d469a72f819a3a4a38e3c6282b7a32b7` | 367282 |
40: | `Specs/RESULTS-SPEC-039/astra-session-v1.16-d2a-r15-20261013/ng039-v116-d2a-r15.last.sse` | `Specs/RESULTS-SPEC-039/astra-session-v1.16-d2a-r15-20261013/astra-review-v1.16-d2a-r15-20261013.raw.sse` | `f381eb4922b0d45322b9e38ae44278846c40c15b4f823b1471988ed715d5092e` | 829964 |
41: | `Specs/RESULTS-SPEC-039/astra-session-v1.16-r14-20261013/ng039-v116-r14-abnormal-journal-policy-review.last.sse` | `Specs/RESULTS-SPEC-039/astra-review-v1.16-r14-20261013.raw.sse` | `cd5a9a0c53f7e19b83986035500d7a3e4017a3564d21c10d39c02c80afd17093` | 279093 |
42: | `Specs/RESULTS-SPEC-039/methodology-review-20261013/ng039-methodology-plan-only-20261013.last.sse` | `Specs/RESULTS-SPEC-039/methodology-review-20261013/astra-methodology-plan-only-20261013.raw.sse` | `6b2e2367af5292b023391b7ff8351aa1eca3bbf8cac23a20a05663a65be34cb8` | 768846 |
43: 
44: ## 4. Files affected
... [omitted lines; see full-file digest below] ...
51: ## 5. Acceptance criteria
52: 
53: 1. Before deletion, every listed pair exists, matches the exact size and SHA-256 in §3, and is byte-identical.
54: 2. No pre-existing repository file or NG-039 task note references any deletion path. The path references in this SPEC §3 are the approved allowlist; retained raw paths remain referenced wherever provenance requires it.
55: 3. Exactly the 11 allowlisted paths are deleted from `Specs/RESULTS-SPEC-039/`; the 11 retained raw files remain byte-identical. The r12 `.last.sse` remains because an immutable submission receipt references that exact path.
56: 4. The results directory changes from 145 files / 18,164,472 bytes to 134 files / 13,928,653 bytes: a reduction of 11 files / 4,235,819 bytes.
57: 5. `git diff --check` passes. No non-allowlisted pre-existing change is altered or staged.
... [omitted lines; see full-file digest below] ...
60: ## 6. Verification and rollback
61: 
62: Before removal, use a read-only Python probe to verify each listed pair exists, matches the exact size/SHA-256, and is byte-identical. After removal, use a separate read-only check: verify each retained `.raw.sse` path and hash, confirm the 11 allowlisted `.last.sse` paths are absent (while preserving the r12 exception), scan for references to deleted paths, verify the results-directory counts/bytes, inspect `git diff --name-status`, and run `git diff --check`. Do not rerun a pair-equality check against deleted candidates.
63: 
64: Rollback is limited to restoring a deleted duplicate from its retained byte-identical raw counterpart if an acceptance check fails. Do not use `git restore` on unrelated dirty files. Stop if any pair differs or a reference is discovered.
65: 
66: ## 7. Review and publication gates
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis/Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md | branch=AIOPT5 | HEAD=8d2386dcb10d2b37f133d4b24c2cb96761753d82 | full_sha256=e16de9f714ef4a81530892bfa41a97d0f12803925ced271c742166a52dee11ae | excerpt_sha256=4e70573370ff19d3bc8c5be3a462f5154c1734329de2208877eebaec3e718360 =====
3: **Author:** Antonio Alberti / Hermes
4: **Date:** 2026-09-12
5: **Status:** In Progress
6: **Branch:** AIOPT3
7: **Implementation commit:** —
8: **Related:** No verifiable SPEC-040/SPEC-041 file is present in the pinned repository snapshots.
9: **Historical references:** `SPEC-040-common-performance-build-profiles.md` and `SPEC-041-gw-deadline-aware-wait.md` are absent from the pinned trees and reachable history. References below explain historical diagnostic context only; they are not active dependencies.
10: 
11: ## 1. Problema e evidência
... [omitted lines; see full-file digest below] ...
28: A função `CheckSubscriptions()` recebe a acção criada para um objecto `Core`; `PB` é o owner da acção e o diagnóstico confirma que o objecto dinâmico é `Core`, não `PG`. O cast viola o contrato de tipos C++ e torna o acesso subsequente a `PPGB->PGCSTuples` comportamento indefinido.
29: 
30: O relatório foi observado durante um smoke Sanitizer histórico da alteração de espera do Gateway. O documento SPEC-041 não está disponível nos snapshots ou no histórico alcançável; esta menção explica a origem do diagnóstico, não uma dependência activa.
31: 
32: ## 2. Objectivo
... [omitted lines; see full-file digest below] ...
83: ## 7. Rollback
84: 
85: Reverter apenas a alteração em `CoreRunEvaluate01.cpp`, preservando a fixture, o log RED e a evidência de revisão. Não reverter nem misturar alterações históricas de perfil de build ou espera do Gateway; os documentos SPEC-040/041 estão indisponíveis nos snapshots e histórico alcançável.
86: 
87: ## 8. Riscos
... [omitted lines; see full-file digest below] ...
105: - `Specs/RESULTS-SPEC-044/post-fix-20260912.md` — qualificação pós-correção e hashes.
106: - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md` — runtime multi-VM normal-path no commit rebuilt; não substitui a matriz de subscriptions.
107: - Contexto histórico: `SPEC-041-gw-deadline-aware-wait.md` (documento indisponível nos snapshots/histórico; não é uma dependência activa).
108: 
109: ## 10. Estado
110: 
111: Esta SPEC separa formalmente o diagnóstico UBSan do trabalho histórico de espera do Gateway. O documento SPEC-041 está indisponível nos snapshots/histórico e não é uma dependência activa. A alteração allowlisted foi implementada após revisão Astra e autorização do utilizador. Builds e smoke Sanitizer passaram; a aceitação funcional permanece aberta até à execução da matriz da SPEC-045 e à revisão Astra pós-correção.
112: 
113: ## 11. Gate histórico de release
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== EXCERPT: /home/gandalf/workspace/novagenesis/Specs/SPEC-045-core-evaluate-functional-harness.md | branch=AIOPT5 | HEAD=8d2386dcb10d2b37f133d4b24c2cb96761753d82 | full_sha256=1a2819629cd81da79e2ec53e22fd6b7c8e8c1c4f3c7b04f7cafb67ae540875ea | excerpt_sha256=edc35aeeab62d993000dbb362571ebea68fac2d99173ba629d9108023b80d05c =====
3: **Author:** Antonio Alberti / Hermes
4: **Date:** 2026-09-12
5: **Status:** In Progress
6: **Branch:** AIOPT3
7: **Implementation commit:** —
8: **Related:** `SPEC-044-core-run-evaluate-invalid-pg-downcast.md`
9: **Historical reference:** `SPEC-041-gw-deadline-aware-wait.md` is absent from the pinned repository snapshots and reachable history; it is not an active dependency.
10: 
11: ## 1. Problema
... [omitted lines; see full-file digest below] ...
===== END EXCERPT =====
===== TOOL DESCRIPTOR CAPTURE: Hermes decision stack (2026-10-17) =====
decision_query: Route one bounded evidence decision. OpenRouter Jev is separate. Direct TypeSafe is default-off, binds exact one-shot payload/rate/cost, follows Hermes approvals.mode, and has no silent fallback. Parameters exposed: request_id (string), operation (string), backend (string); no live call was made.

assess_progress: local-only report for one strict snapshot-bound task/SPEC ledger; never calls Jev; no percentage unless trusted local owner approval and evidence verifier are supplied by host.
===== END TOOL DESCRIPTOR =====
## Input SHA-256 manifest (captured before packet freeze)

- 7eade367a0be6d80f1f0f9380d9d44ad6af13559bd242dc85d2c22e8ab9c70c8  17891 bytes  [included-full]  /home/gandalf/workspace/novagenesis/Specs/SPEC-071-spec-implementation-drift-auditor.md
- b7bbc42ad34fcacbe33e22547a4aec3bd4b3a1a8ff921c8e31a2a48472d80acf  12330 bytes  [included-full]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md
- a52b4a877083ec1a37c6b5359cfa57381298db3bd30b0df3a463861230710f16  8643 bytes  [included-full]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md
- de8dce5e5872280aad210ad0245e3aac5f7b91972695afe31825bbb7fc2347c2  9369 bytes  [included-full]  /home/gandalf/workspace/novagenesis/Specs/SPEC-070-branch-spec-sanitization-and-impact-sidecars.md
- 0e92b7b86d62cb41640d03f03c5506cf832ed2ef9c3f1c65fd08f1f782d83946  8081 bytes  [included-full]  /home/gandalf/ObsidianVault/Tarefas/NG-071-detector-drift-spec-implementation-v1-20261017.md
- e46554e585a3139de4ad4e7eb8eab9712c21a03a9de2faf2ca665185f265d344  9249 bytes  [included-full]  /home/gandalf/ObsidianVault/Tarefas/NG-070-saneamento-specs-branches-v1-20261017.md
- 6e905a4d6e7da4ee1a2ead5b73cdfb4ac2a91159d5ac85836c9a0fdbc2495f0d  2860 bytes  [included-full]  /home/gandalf/.hermes/skills/software-development/hermes-local-decision-stack/references/typesafe-direct-payload.md
- 116f27ccf4b248316cc9825a6e43183c9d0e60b88a80fbed1bd7ad4347c91024  2918 bytes  [included-full]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-071-spec-implementation-drift-auditor.md.affected-files.md
- 9000fb660b50bd74f2206b7c5e2d282476032ab90b1604fde089b4b261916ba4  96674 bytes  [dashboard-relevant-row-only; excerpt_sha256=ff760a7ffb4fc06f7897a40e82fdb32429e5b3bb082b33823ea01cc11cc48b93]  /home/gandalf/ObsidianVault/Tarefas/Dashboard.md
- de4241aabf0a97ca3a092c79b93fd61fe7e68ea92b528511364e3906579dbda6  8782 bytes  [excerpted-lines; excerpt_sha256=228ee4e244849e673b936849daeb6847aa77febe5a3e34a02fc89e5d4102cc3d; lines=1-11,13-17,21-25,37-41,82-86,102-106]  /home/gandalf/workspace/novagenesis-ng070-aiopt3/Specs/SPEC-031-recvmmsg-batched-receive.md
- 1f6da15d599550b80362f0b4a6e89222d97a64e36ccccc7277ddd08ef9a9a9f0  10760 bytes  [excerpted-lines; excerpt_sha256=793dc8346b54038be8353e1caf6a70aa1f1ac80437e6d927b0f89af648f560df; lines=4-9,11-17,25-32,40-44,50-59,61-67,71-75,83-103,111-115,118-122,131-136,142-150]  /home/gandalf/workspace/novagenesis-ng070-aiopt3/Specs/SPEC-037-promote-aiopt3-default-branch.md
- 760e552e782d525b475a2e843a0ae72f21d60aeecae74b98b03f185a39c4f40b  11213 bytes  [excerpted-lines; excerpt_sha256=f64bb814d16427585aaba92880289d5412ea4fd659d650ca0f4a23e5024e22e4; lines=3-16,56-60,148-152]  /home/gandalf/workspace/novagenesis-ng070-aiopt3/Specs/SPEC-054-release-master-plan-and-audit.md
- de4241aabf0a97ca3a092c79b93fd61fe7e68ea92b528511364e3906579dbda6  8782 bytes  [excerpted-lines; excerpt_sha256=228ee4e244849e673b936849daeb6847aa77febe5a3e34a02fc89e5d4102cc3d; lines=1-11,13-17,21-25,37-41,82-86,102-106]  /home/gandalf/workspace/novagenesis-ng070-aiopt4/Specs/SPEC-031-recvmmsg-batched-receive.md
- 1f6da15d599550b80362f0b4a6e89222d97a64e36ccccc7277ddd08ef9a9a9f0  10760 bytes  [excerpted-lines; excerpt_sha256=793dc8346b54038be8353e1caf6a70aa1f1ac80437e6d927b0f89af648f560df; lines=4-9,11-17,25-32,40-44,50-59,61-67,71-75,83-103,111-115,118-122,131-136,142-150]  /home/gandalf/workspace/novagenesis-ng070-aiopt4/Specs/SPEC-037-promote-aiopt3-default-branch.md
- 760e552e782d525b475a2e843a0ae72f21d60aeecae74b98b03f185a39c4f40b  11213 bytes  [excerpted-lines; excerpt_sha256=f64bb814d16427585aaba92880289d5412ea4fd659d650ca0f4a23e5024e22e4; lines=3-16,56-60,148-152]  /home/gandalf/workspace/novagenesis-ng070-aiopt4/Specs/SPEC-054-release-master-plan-and-audit.md
- de4241aabf0a97ca3a092c79b93fd61fe7e68ea92b528511364e3906579dbda6  8782 bytes  [excerpted-lines; excerpt_sha256=228ee4e244849e673b936849daeb6847aa77febe5a3e34a02fc89e5d4102cc3d; lines=1-11,13-17,21-25,37-41,82-86,102-106]  /home/gandalf/workspace/novagenesis/Specs/SPEC-031-recvmmsg-batched-receive.md
- 1f6da15d599550b80362f0b4a6e89222d97a64e36ccccc7277ddd08ef9a9a9f0  10760 bytes  [excerpted-lines; excerpt_sha256=793dc8346b54038be8353e1caf6a70aa1f1ac80437e6d927b0f89af648f560df; lines=4-9,11-17,25-32,40-44,50-59,61-67,71-75,83-103,111-115,118-122,131-136,142-150]  /home/gandalf/workspace/novagenesis/Specs/SPEC-037-promote-aiopt3-default-branch.md
- ffa4adb2dd1354dac2368b61af66a8bfff502c110573eccdc03967a2a17b1135  8166 bytes  [excerpted-lines; excerpt_sha256=e495fea62f5798083f145f998f08ba687592a09edc89f034396990a02dc9de26; lines=4-8,12-16,18-26,30-44,51-57,60-66]  /home/gandalf/workspace/novagenesis/Specs/SPEC-069-ng039-public-evidence-deduplication.md
- e16de9f714ef4a81530892bfa41a97d0f12803925ced271c742166a52dee11ae  7841 bytes  [excerpted-lines; excerpt_sha256=4e70573370ff19d3bc8c5be3a462f5154c1734329de2208877eebaec3e718360; lines=3-11,28-32,83-87,105-113]  /home/gandalf/workspace/novagenesis/Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md
- 1a2819629cd81da79e2ec53e22fd6b7c8e8c1c4f3c7b04f7cafb67ae540875ea  8450 bytes  [excerpted-lines; excerpt_sha256=edc35aeeab62d993000dbb362571ebea68fac2d99173ba629d9108023b80d05c; lines=3-11]  /home/gandalf/workspace/novagenesis/Specs/SPEC-045-core-evaluate-functional-harness.md
- 5b8c0da78ec8635b482393dd0fbd1da3514441352a5144b6d398d3dd3f0f5bd3  8439 bytes  [digest-reference-only; sidecar contents not supplied]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-031-recvmmsg-batched-receive.md.affected-files.md
- 05bddaf9f81a329fdd8eb5cb9da043de44f0210547caddfa4c9cd3c3e4876c0d  16178 bytes  [digest-reference-only; sidecar contents not supplied]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-037-promote-aiopt3-default-branch.md.affected-files.md
- 3f3ba5170b82e6a4ee80c648f4b7a4c60a7837fe97f6b370fd8c66d6ca6d1678  7993 bytes  [digest-reference-only; sidecar contents not supplied]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-044-core-run-evaluate-invalid-pg-downcast.md.affected-files.md
- b2733a8be82fcb3bb071b30231f4641087976d263f6c52af130ee0b667974ad7  6658 bytes  [digest-reference-only; sidecar contents not supplied]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-045-core-evaluate-functional-harness.md.affected-files.md
- 5508bcfa12a50a86e66a702e2a71cdc8281d3f80dd884cf38a8881a57bd7a752  68893 bytes  [digest-reference-only; sidecar contents not supplied]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-054-release-master-plan-and-audit.md.affected-files.md
- 9a8fe2d001bab04579b72f6b46529c7f07ec4fa6ea4616064bc3b0f4a991a576  6774 bytes  [digest-reference-only; sidecar contents not supplied]  /home/gandalf/workspace/novagenesis/Specs/IMPACT/SPEC-069-ng039-public-evidence-deduplication.md.affected-files.md

The packet file and request-instructions file were pre-created empty before worktree status capture; their generated untracked paths are listed in the snapshot. Their final digests are external to this packet to avoid self-hashing.

## Design-only review questions

1. Is the local-first scan plus Hermes `decision_query` handoff coherent and sufficiently bounded?
2. Are the five drift classes and finding states adequately separated, particularly deterministic findings vs semantic review-needed findings?
3. Are exact branch/HEAD identity, dirty/untracked handling, historical SPEC applicability and false-clean prevention sufficiently specified?
4. Are the egress, no-write, progress and no-auto-approval controls testable and safe?
5. Is it correct to leave the CLI schema/file-name contract open before implementation, or are other design blockers missing?
6. Does this packet provide enough evidence for design readiness? Identify omissions that limit your verdict.

Return exactly one top-level verdict: `APPROVE` (design readiness only), `AMEND`, or `NO-GO`; explicitly state `implementation authorization: NO`. Give numbered findings with exact supplied file/section references, separate blockers from optional hardening, and do not claim code/test/runtime validation. Preserve the human owner's authority over any requested SPEC change.

## Evidence boundary

This packet includes no implementation source, no test execution, no runtime data, and no JEV request/response. The cited branch audit is a historical documentary report; all three worktrees contain dirty/untracked state. The review must assess only the design packet bytes and explicitly state these limitations. Do not infer full Dashboard review from the included row, and do not treat the historical branch SHAs as current implementation review pins.
