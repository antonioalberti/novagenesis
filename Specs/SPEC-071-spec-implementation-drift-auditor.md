# SPEC-071: NovaGenesis SPEC Implementation Drift Auditor

**Version:** v0.13-draft
**Date:** 2026-10-17
**Author:** Antonio Alberti / Hermes Agent
**Status:** Draft — post-Astra AMEND corrections in v0.13; owner approval still required; no implementation authorized
**Branch:** AIOPT5
**Baseline commit:** `8d2386dcb10d2b37f133d4b24c2cb96761753d82`
**Implementation commit:** —
**Related:** `SPEC-070-branch-spec-sanitization-and-impact-sidecars.md`, `SPEC-031-recvmmsg-batched-receive.md`, `SPEC-039-common-performance-measurement.md`
**Owner task:** `NG-071-detector-drift-spec-implementation-v1-20261017.md`

## 1. Problem and objective

NovaGenesis SPEC status and current implementation evidence can drift apart. A SPEC may remain Draft after an implementation commit, describe an old source path as current, refer to a SPEC file absent from the reviewed tree, prescribe a verification step that cannot run after the stated deletion, or carry an AIOPT3 origin label that is mistaken for AIOPT5 authority. Task status and checkboxes can likewise drift from the governing SPEC and lack verifiable evidence. The current branch audit also showed that missing status fields and historical copies make a single percentage-of-progress misleading.

The CLI is a local, read-only evidence scanner. It emits a bounded JSON/Markdown report and a snapshot-bound gate ledger; it does not call remote services or Hermes tools itself. The Hermes orchestrator may separately pass a selected, minimized one-claim handoff to the registered `decision_query` and may pass a trusted gate ledger to local-only `assess_progress`. Those tool results remain separate advisory/progress outputs and never change the scanner's deterministic findings. The local evaluator remains authoritative for inventory, status, references, commit ancestry, paths, and scan coverage.

## 2. Inputs and examples motivating the tool

Initial drift cases from the 2026-10-17 branch audit:

1. `SPEC-031` said Draft and described `recvfrom()` as the current path, while implementation commit `c638292fcf9ceef1a23e0cdbd6a41af4c44e780f` is an ancestor of all three pinned heads and their `ReceiveDispatcher` uses `recvmmsg()`.
2. `SPEC-069` instructed a post-deletion verification to rerun a pair-equality check that requires the deleted `.last.sse` candidates to exist.
3. AIOPT5 `SPEC-044/045` contained `Related:` references to SPEC-040/041, absent from the pinned trees and reachable local history.
4. The status register still said 24 SPEC files at AIOPT3 `cd804a7`, while the reviewed snapshots contained 58/60/48.
5. A `Branch: AIOPT3` field frequently marked document origin/scope, but was easy to mistake for current AIOPT4/AIOPT5 authority.

These are seed fixtures/examples, not an exhaustive drift taxonomy or evidence that every status field is wrong.

## 3. Proposed operating model (draft)

### 3.1 Local evidence layer and snapshot contract

The CLI requires one explicit `--source BRANCH=PATH` for each of AIOPT3, AIOPT4 and AIOPT5. At preflight, it records the actual branch name, full `HEAD`, tree SHA, repository identity, shallow-history state, and dirty/untracked census for each root. It never fetches, checks out, switches branches, builds, tests, changes files or reads credentials.

Default scope is the immutable committed tree at the captured `HEAD`. Dirty and untracked files are counted and hashed as worktree state but are not silently substituted for committed bytes. Optional `--include-worktree` scans them as a distinct `snapshot_layer=worktree`, with individual content hashes; results from that layer must never merge with committed-tree findings. A branch alias mismatch, unresolved ref, unreadable object, incomplete required history, or file that changes during a worktree overlay scan yields `INDETERMINATE` for affected checks. No network fallback or implicit branch discovery is allowed.

For each SPEC, the local collector may read only bounded SPEC text, the same-tree referenced SPEC files, paths explicitly declared by the SPEC, Git metadata/diffs, and explicitly referenced test/evidence artifacts. It must not follow symlinks outside the selected root, read `.git` secrets/config, credentials, or unrelated files. Version and size limits are fixed by §4; hitting a limit marks coverage incomplete, never clean.

Collected facts include:

- Full SPEC filename (never bare numeric ID), header/status/branch/baseline and implementation commit.
- Acceptance criteria and explicit file/scope/allowlist declarations.
- Same-tree reference resolution, with current/historical role kept separate.
- SPEC-linked commit ancestry and changed paths; co-change alone is not causation.
- Evidence locators, command/run identity, exit code and hashes where present; no test is executed by the scanner.

The collector must not treat file presence, commit ancestry, status text, a test filename or a Jev/Astra opinion as proof that an acceptance criterion passed.

### 3.2 Rule catalogue and evidence threshold

V1 covers the five owner-selected classes with explicit rules. Rule IDs are stable within report schema v1:

| Rule ID | Coverage class | Minimum evidence for `CONFIRMED_DRIFT` | Otherwise |
|---|---|---|---|
| `STATUS_IMPL` | SPEC status/implementation-commit mismatch | Same-snapshot SPEC claim and pinned Git/source evidence directly contradict (e.g., status says Draft while a cited implementation commit and current implementation are present) | Missing ancestry, ambiguous status, or only co-change evidence is `INDETERMINATE`/`POSSIBLE_DRIFT` |
| `CURRENT_SOURCE` | Current path/behavior claim vs source | A normative current claim and same-HEAD source/path evidence directly conflict | Semantic uncertainty is `POSSIBLE_DRIFT`; missing source is `INDETERMINATE` |
| `REFERENCE_APPLICABILITY` | Dangling references and branch applicability | Full filename/active dependency is absent from the exact tree, or the SPEC explicitly claims authority contradicted by its declared scope | Historical mention, missing branch header, duplicate ID, or incomplete history stays `HISTORICAL`/`UNKNOWN`/`INDETERMINATE` as appropriate |
| `DECLARED_SCOPE_DIFF` | Declared affected/closed paths vs observed changes | Only when the SPEC explicitly defines a closed allowlist and the same-snapshot path set violates it | Prose lists and commit co-change are not allowlists or causal proof; classify as `POSSIBLE_DRIFT`/`INDETERMINATE` |
| `ACCEPTANCE_EVIDENCE` | Acceptance/test evidence vs current branch | A same-snapshot test/evidence artifact or recorded acceptance result directly contradicts the SPEC's explicit claim | Missing, unbound, or unverifiable test evidence is `INDETERMINATE`, not failure, drift, or PASS |

The five NG-070 examples map to rules as follows: SPEC-031→`STATUS_IMPL`/`CURRENT_SOURCE` (benchmark acceptance remains a separate unmet evidence gate); SPEC-069→`PROCEDURE_ORDER`; SPEC-044/045→`REFERENCE_APPLICABILITY`; the old status register→`DATED_INVENTORY`; AIOPT3-origin copies→`REFERENCE_APPLICABILITY`. These examples do not collapse the five owner-selected coverage classes.

| Subrule | Confirmed condition | Non-trigger / uncertainty |
|---|---|---|
| `PROCEDURE_ORDER` (within `ACCEPTANCE_EVIDENCE`) | A normative procedure explicitly deletes/removes an input before a later required step that names/requires that same input, and no regeneration/retained alternative exists in the same procedure/snapshot | If ordering, requirement, or replacement input is ambiguous, emit `POSSIBLE_DRIFT` or `INDETERMINATE`; a before-delete equality test is not contradicted by a later delete |
| `DATED_INVENTORY` (within `STATUS_IMPL`) | Text asserts a current/exhaustive inventory for an identified snapshot and the complete same-tree inventory disagrees | A dated count tied to an older commit/date is `HISTORICAL`, not drift; missing snapshot/date/inventory completeness is `INDETERMINATE` |

For reference checks, an absent full filename in a branch tree is `CONFIRMED_DRIFT` only when the inspected SPEC makes it an active mandatory dependency; an explanatory/historical mention is classified separately. If relevant history is shallow/unavailable, do not conclude the target never existed. For duplicate IDs, resolve by full filename or return ambiguity; for branch metadata, treat `Branch:` as stated origin/scope only, never as proof of authority on a copy branch.

Additional attribution rules: use full filenames for duplicate SPEC IDs; rename detection is accepted only when Git history is complete and the rename is unambiguous; an explicit SPEC-ID commit reference is stronger than co-change but is still not acceptance evidence; partial/shallow history cannot establish absence or ancestry. Externally produced test evidence is admissible only when it binds the exact criterion/test ID, test command and tool/version, source snapshot ID, run identity, exit status, and output-artifact digests plus an independently verifiable receipt. A missing/mismatched binding is `INDETERMINATE`; a raw log, task checkbox, declared status, or success label alone is never acceptance evidence. Each result cites the rule ID, SPEC section/line, source root, snapshot layer, full HEAD/tree identity, evidence hashes/locations and reasoning.

### 3.3 Finding, applicability and execution axes

Do not overload one status field. Each check reports three independent axes:

- `outcome`: `CONFIRMED_DRIFT`, `POSSIBLE_DRIFT`, `NO_DRIFT_DETECTED`, or `INDETERMINATE`.
- `applicability`: `CURRENT`, `HISTORICAL`, `NOT_APPLICABLE`, or `UNKNOWN`.
- `execution`: `COMPLETED`, `PARTIAL`, `SKIPPED`, `UNSUPPORTED`, or `FAILED`.

`HISTORICAL` is not a finding outcome: a historical SPEC may still have an unresolved documentary issue, but that issue is not automatically a current-branch instruction. `NO_DRIFT_DETECTED` is legal only when a named rule completed against admissible, complete inputs. An empty findings list is not sufficient. A summary is `COMPLETE_NO_DRIFT` only if every planned applicable rule completed and returned no drift. `COMPLETE_WITH_DRIFT` is legal only if every planned applicable rule completed and at least one `CONFIRMED_DRIFT` exists with no unresolved `POSSIBLE_DRIFT`/`INDETERMINATE` findings. Any partial/unsupported/skipped required check, possible/indeterminate finding, unknown applicability, or missing mandatory input makes the relevant branch/run `INCOMPLETE`. Disabled or failed optional `decision_query` does not downgrade a completed deterministic check and never promotes an incomplete one.

### 3.4 Optional `decision_query` handoff (Hermes host only)

The CLI never invokes a model, plugin tool, or HTTP endpoint. It may emit a `decision_query_handoff` record in the JSON report; Hermes may separately choose one claim and invoke the registered tool. No handoff is executed automatically. Use the explicit `typesafe_jev_direct` route only; do not select `auto`, substitute another backend, import a helper, or create a direct provider client.

The registered route contract is:

- `operation=verify`, `backend=typesafe_jev_direct`, policy ID `hermes_local_decision_stack.typesafe_jev_direct@1`, one bounded claim, exact `source_revision.snapshot_id` (SHA-256), and actual dirty/untracked flags.
- The nested handler query includes the local `validate_query` envelope (`request_id`, `task_id`, `risk_level`, `constraints`, `limits`, `evidence_refs`, `source_revision`, and language/evidence-summary options) plus exactly one `claims` item. The registered tool descriptor permits additional fields; the current adapter's `_bounded_request` consumes `claims[0]`.
- **Known host-schema mismatch:** the installed `decision_query.schema.json` has `additionalProperties: false` and does not define `claims`, while the registered handler descriptor permits extra fields, `contracts.validate_query` accepts the extension, and the direct TypeSafe adapter requires it. Thus the candidate matches the current handler/adapter code but is not valid under that separate strict schema. Do not claim both contracts pass or silently modify Hermes. Keep remote dispatch blocked until the owner decides which contract is authoritative or separately authorizes a schema/host correction; local scanning and report generation remain usable.
- Constraints must explicitly use `privacy=approved_remote`, `egress=approved_only`, and `cost=metered`. The installed TypeSafe Choice contract has exactly `supports`, `contradicts`, and `insufficient_information` outcomes.
- `claims[0]` is the exact SPEC proposition being tested, not an assertion that it is true. `options.typesafe_evidence_summary` contains only bounded factual observations and the criterion being evaluated; it must not pre-label the conclusion. Supply exactly one claim per request. The route asks whether the summary supports that proposition and may return `contradicts` where it does not.
- The CLI caps the single `claims[0]` value at 1,000 characters and the evidence summary at 8,000 characters, uses only repository-relative identifiers/hashes (no local absolute paths), and excludes code, full SPEC text, raw logs, credentials, PII, and protected evidence.

The registered Hermes plugin owns the profile opt-in, native approval-mode behavior, payload/rate/cost snapshot, exact one-shot dispatch binding, callback, receipt, replay protection, and fail-closed behavior. A handoff is merely a candidate, not a callable API object: it carries no credential, approval ticket, or implicit consent. Hermes must validate the nested query against the actual handler/adapter contract and obtain the plugin's exact one-shot approval; because the strict profile schema currently rejects `claims`, remote dispatch remains blocked until the owner resolves that mismatch. The `candidate_reason`, full report and evidence references remain local; for `typesafe_jev_direct`, the registered adapter sends only the constructed Choice payload fields (model, `claims[0]` as the claim, bounded evidence summary, language tags, source snapshot ID and fixed question/criteria), never the outer query, evidence paths, or raw files. Tests must inspect the exact serialized outbound payload for that privacy boundary. The direct TypeSafe Choice route has fixed outcomes `supports`, `contradicts`, and `insufficient_information`; `claims[0]` is the SPEC proposition under review, while the evidence summary is a separate factual account. The CLI must not send a proposition already directly contradicted by its deterministic evidence; such cases are classified locally. If Jev returns `contradicts` for an ambiguous handoff, record an advisory review flag only; do not promote it to `CONFIRMED_DRIFT`. A consumed receipt is advisory only and must retain `executed_backend`, `status`, `decision_type=choice`, the single typed answer, `allow_candidate=false`, and `calibration_status=unvalidated`; it cannot set a local gate or progress state. Errors/timeouts/malformed responses remain separate advisory failures; deterministic findings stay unchanged. No retries, fallback, or automatic dispatch.

Non-live sanitized mapping example (synthetic hashes; never dispatch this fixture):

```json
{
  "candidate_id": "specdrift-rproc-demo01",
  "candidate_reason": "A bounded evidence claim may benefit from advisory semantic review.",
  "consent_required": true,
  "query": {
    "request_id": "specdrift-rproc-demo01",
    "task_id": "NG-071",
    "operation": "verify",
    "risk_level": "low",
    "backend": "typesafe_jev_direct",
    "constraints": {"privacy": "approved_remote", "egress": "approved_only", "cost": "metered", "latency": "medium", "device": "auto"},
    "policy_id": "hermes_local_decision_stack.typesafe_jev_direct@1",
    "limits": {"max_context_chars": 20000, "max_evidence_items": 20, "timeout_ms": 30000},
    "claims": ["The AIOPT5 ReceiveDispatcher in the pinned source tree calls recvmmsg."],
    "evidence_refs": [{"source_kind": "file", "snapshot_id": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "path": "PGCS/src/NGAL_Transport_RAW.cpp", "repository": "AIOPT5", "effective_head_sha": "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", "content_digest": "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc", "location": "ReceiveDispatcher"}],
    "source_revision": {"snapshot_id": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "dirty": false, "untracked": false},
    "options": {"source_language": "en", "request_language": "en", "typesafe_evidence_summary": "At the synthetic AIOPT5 snapshot, ReceiveDispatcher contains a recvmmsg call; implementation commit c638292fcf9ceef1a23e0cdbd6a41af4c44e780f is an ancestor. No performance benchmark evidence is included."}
  }
}
```

The sole `claims[0]` entry is the SPEC proposition under review, not a fact asserted by the scanner; the evidence summary states the supplied observations. Hermes/tool validation must bind the exact query to the current snapshot and approval ticket. This fixture demonstrates schema mapping for the registered handler only; the separate strict profile query schema rejects the `claims` extension, so the mismatch remains a blocked live-egress gate. It is not a live request, valid evidence receipt, or acceptance evidence.

### 3.5 Progress reporting through the existing local ledger

The CLI does not compute a percentage and does not call `assess_progress` itself. It emits a `progress_ledger` only when an owner-authored, snapshot-bound scope/weight decision supplies a non-empty `weight_approval_ref` and the ledger satisfies the existing schema; that export is a candidate, not trusted approval. If no such reference/complete weighted scope exists, `progress_ledger` is null and the report carries only a separate declared-criteria status summary with an explicit unavailability reason. Hermes may pass an eligible ledger to `assess_progress` as a separate local-only step. V1 must not duplicate its percentage arithmetic.

A percentage is available only from the separate local `assess_progress` result when the ledger has a complete owner-approved scope/mapping and weights, a non-empty approval reference, a verified immutable snapshot manifest, and host-provided owner-approval and gate-evidence verifiers. Included gates use `met`, `not_met`, `blocked`, or `unknown`; owner-deferred gates require an explicit decision reference. Every `met` gate requires same-snapshot artifact and verifier-receipt hashes. Unverified `met` is downgraded to `unknown`, receives no numerator credit, and remains in the denominator. The existing calculator uses approved included weights, reports its denominator revision, and rounds half-up to two decimals. The CLI itself never reports a percentage.

Without those callbacks, approved weights, or a complete denominator, the CLI may emit the ledger and gate/status counts with an unavailability reason, but no percentage. Keep this separate from branch SPEC status distributions and drift findings. `assess_progress` never calls Jev, and Jev confidence is never progress.

### 3.6 Task/SPEC progress-ledger mapping

When `--task` is supplied, the CLI hashes that one task note, extracts its stable task ID, declared status, exact `depends_on` filenames, and acceptance criteria, and binds it to the explicit branch/SPEC snapshot. It does not crawl other task files or treat status/checkboxes as verified completion. A missing/ambiguous dependency or a task branch with no unique corresponding SPEC produces an applicability finding, not a guessed mapping.

Each task/SPEC record carries `progress_summary`: a snapshot-bound count of criteria and their source-declared states (`declared_met`, `declared_not_met`, `declared_blocked`, `declared_unknown`, `owner_deferred`, `unclassified`). These counts describe text/checklist declarations only, not verified completion. The CLI always sets `percentage_available=false` and `percentage=null`. If an owner-authored approval reference and weighted scope are absent, `progress_ledger` is null and the summary states why; a present ledger remains an untrusted candidate pending host verification.

The exported ledger uses the existing local progress contract (`hermes_decision_stack/progress.py`, schema version 1), not a duplicate calculator. For this tool, `scope_id` is `spec_implementation` or `task_overall`; `source_identity` is `{kind, source_id, source_sha256}`; the top level includes `snapshot_id`, `scope_complete`, `mapping_complete`, `weights_approved`, `weight_approval_ref`, and `gates`. Each gate has exactly `{gate_id, parent_id, requirement_ref, included, state, weight, weight_source_ref, decision_ref, evidence}`; evidence has exactly `{snapshot_id, artifact_sha256, verifier_id, verifier_receipt_sha256}`. Weights must be positive finite integers/decimal strings, never floats or booleans. A claimed `met` leaf gate without a same-snapshot artifact digest and verifier-receipt digest is downgraded to `unknown` by `assess_progress`; owner approval and snapshot/evidence callbacks are host trust boundaries, not fields the CLI may self-assert.

The CLI exports the ledger but does not call the Hermes tool. Hermes may pass the object to `assess_progress`, whose host callbacks must independently verify the snapshot manifest, owner-authored scope/weights, and each evidence/verifier receipt. If any verifier is absent or fails, `assess_progress` downgrades claimed `met` gates to `unknown` and withholds the percentage. The auditor report never embeds or approximates the assessment result. Hermes may present the separate local `assess_progress` receipt beside the report, including its `percentage_available`, denominator revision, verified/unverified gate counts, and unavailable reasons; when unavailable, present statuses only.

### 3.7 NovaGenesis principle impact

This is repository/evidence tooling, not a change to NovaGenesis protocol or runtime architecture. It preserves the principles catalogue's separation of identity from location and its requirement for verifiable provenance/privacy by binding findings to content hashes and immutable snapshot IDs while keeping local paths separate from remote summaries. It also favors synergy and simplicity by using the registered Hermes `decision_query` and local `assess_progress` contracts instead of adding provider clients or a second progress engine. No deliberate deviation from the NovaGenesis principles is proposed. Source basis: `NovaGenesis - Princípios de Projeto.md` §§2, 4, 10, 13 and 14; these are tool-level applications, not claims that the audit CLI is itself a network entity/protocol service.

## 4. Candidate files affected (not yet authorized for implementation)

| Path | Candidate change | Reason |
|---|---|---|
| `Scripts/Python/spec_implementation_drift_auditor.py` | New read-only CLI | Scans the three explicit branch worktree roots and produces normalized findings/ledgers |
| `Scripts/tests/test_spec_implementation_drift_auditor.py` | New tests/fixtures | Cover the NG-070 cases, every rule, negative attribution cases and fail-closed limits |
| `Scripts/Schemas/spec_implementation_drift_report.v1.schema.json` | New report schema | Validate the canonical JSON report and branch/SPEC/finding records |
| `Specs/IMPACT/` | Explicit `--out-dir` output target | Default stdout/no writes; only persist when explicitly requested |
| `Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md` | Review-context brief | Carries prior design AMEND and post-implementation packet checklist; refresh at final review |

**Repository path:** place the tool under `/home/gandalf/workspace/novagenesis/Scripts/` (the owner selected this checkout path; `/workspace/novagenesis/Scripts` is unavailable in this environment).

### CLI contract

V1 command form:

```text
python3 Scripts/Python/spec_implementation_drift_auditor.py \
  --source AIOPT3=<git-worktree-path> \
  --source AIOPT4=<git-worktree-path> \
  --source AIOPT5=<git-worktree-path> \
  [--task <single-task-note-path>] \
  [--include-worktree] \
  [--stdout-format markdown|json] \
  [--out-dir <existing-parent-directory>]
```

- Exactly one source for each named branch is required. Each path must be a Git worktree whose actual branch matches the label; capture `HEAD` once before scanning. No implicit `origin/` lookup, fetch, branch switching, or detached-HEAD aliasing.
- `--task` is optional; when supplied, scan only that explicit task note and its exact `depends_on` SPEC filenames. Do not crawl the Obsidian vault. Task status/checkboxes are declarations, not proof of acceptance.
- Committed Git objects are the default scan layer. `--include-worktree` adds a separate dirty/untracked overlay with its own digests and results; it never substitutes worktree text for committed bytes.
- `--stdout-format` defaults to `markdown`; it can select the full canonical report as JSON instead. Both stdout modes include the consolidated summary and all per-SPEC findings in deterministic order. With no `--out-dir`, the command writes nothing to disk. With `--out-dir`, it creates one unique run subdirectory and writes the canonical JSON/Markdown outputs, input manifest, output hash manifest, and its detached SHA-256 file.
- Deterministic resource bounds: at most 10,000 SPEC files, 1 MiB per text artifact, 64 MiB of text/evidence reads per source root, and 50,000 commits per history-dependent rule. If a cap is reached, record the skipped/partial check and mark that branch/run `INCOMPLETE`; never truncate and report clean.
- Run directory: `<out-dir>/spec-drift-v1-<run_id>/`, where `run_id` is the first 16 hex characters of SHA-256 over canonical JSON containing tool, parser, rule-set and schema versions, branch labels, resolved HEAD/tree identities, task digest if supplied, and snapshot mode. Canonical JSON is UTF-8 with Unicode preserved, object keys sorted lexicographically, separators `,` and `:` without spaces, non-finite numbers forbidden, and no trailing newline. Files: `summary.json`, `summary.md`, `branches/<branch>/specs/<full-SPEC-filename>.json`, corresponding `.md`, `input-manifest.json`, `output-manifest.json`, and `output-manifest.sha256`. Output ordering is deterministic: sources by branch label, SPECs by full filename, rules by rule ID, and evidence refs by repository-relative path and location.
- `input-manifest.json` binds every selected source root, SPEC/task/evidence artifact to its branch, snapshot layer, repository-relative path, content SHA-256, size, and inclusion reason. The canonical SHA-256 of its canonical JSON bytes is required as `input_manifest_sha256` in `summary.json`; the manifest does not contain its own digest.
- `output-manifest.json` lists the relative path, byte size, media type and SHA-256 of `summary.json`, `summary.md`, every per-SPEC JSON/Markdown file, and `input-manifest.json`. It excludes itself and its detached digest. `output-manifest.sha256` contains the standard `sha256sum` line for `output-manifest.json`; no self-referential digest is used.
- Refuse an existing run directory, a symlink at the target leaf, path escape, or overwrite. Create files using temp names under the new run directory and atomic rename. On any partial-write error, return exit 4 and leave an explicitly untrusted partial directory; do not delete/rewrite source files.
- Exit codes: `0` = all planned applicable checks complete and no drift; `1` = complete run with one or more `CONFIRMED_DRIFT` findings and no unresolved/indeterminate checks; `2` = incomplete/possible/indeterminate coverage (confirmed findings remain in output); `3` = invalid CLI/source preflight (diagnostic on stderr, no report/run directory); `4` = output/internal I/O failure (diagnostic on stderr; any partial run directory is untrusted and has no valid output manifest). For exits `0`–`2`, stdout carries the full selected-format report and stderr is empty; for `3`–`4`, stdout is empty and stderr carries one bounded diagnostic. Exit code is not an acceptance decision.

### Canonical JSON report contract

The machine-readable report schema is `Scripts/Schemas/spec_implementation_drift_report.v1.schema.json` (Draft 2020-12). The root requires `schema_version`, `run_id`, `generated_at_utc`, `tool_version`, `parser_version`, `rule_set_version`, `run_status`, `exit_code`, `input_manifest_sha256`, `sources`, `summary`, `specs`, `task` (object or null), and `limitations`. Valid reports use `COMPLETE_NO_DRIFT`/exit `0`, `COMPLETE_WITH_DRIFT`/exit `1`, or `INCOMPLETE`/exit `2`; preflight and internal failures use exit `3`/`4` on stderr and are not valid run reports. These values follow §3's coverage rules, not the number of files scanned. `sources` contains one record per branch with the user label, actual branch, full HEAD/tree SHA, snapshot layer, dirty/untracked census digests/counts, and history-completeness flags. `summary` includes SPEC count, planned/completed/partial/skipped/unsupported/failed rule counts, finding counts by outcome, and applicability counts. `specs` contains one record per full filename per branch, with source content SHA, declared header/status, applicability, check-execution coverage, findings, declared `progress_summary`, optional `progress_ledger`, and decision-query handoff candidates (or empty list). The schema file also defines `$defs.input_manifest`, `$defs.output_manifest`, `$defs.query_handoff`, `$defs.direct_typesafe_handler_query`, and `$defs.progress_summary` for the sibling artifacts/contracts. Semantic validation must reconcile counts, branch uniqueness, finding IDs, exit/status mapping, input hash and all output-manifest paths/hashes; JSON Schema alone cannot prove those cross-record equalities.

Each finding requires `finding_id`, `rule_id`, `outcome`, `applicability`, `execution`, `spec_filename`, `spec_section`, `reason`, `evidence_refs`, and `limitations`. `finding_id` is the first 24 hex characters of a SHA-256 over canonical JSON containing rule ID, branch label, full HEAD/tree snapshot ID, snapshot layer, full SPEC filename/hash, and normalized source location/subject; it is stable across report ordering. Each `evidence_ref` binds `source_kind`, `snapshot_id`, repository-relative `path`, `effective_head_sha`, `content_sha256`, and line/commit/test locator. Absolute local paths, credentials and raw code/log excerpts are not placed in a remote handoff. No text is silently truncated: over-limit input is marked `UNSUPPORTED`/`PARTIAL` and the run cannot be clean. The Markdown files are deterministic presentations of the same canonical JSON records; JSON is authoritative. All enums and required fields are closed by the v1 schema; unknown parser/rule versions are unsupported/incomplete, not ignored.



## 5. Safety, privacy, and authority gates

- Default operation is local and read-only. The CLI's `decision_query_handoff` is only a candidate for a separate Hermes call; it is not executed by the script.
- Routine Jev egress is default-off and limited to the allowlisted, sanitized one-claim payload in §3.4. Never send full SPECs, repositories, code, raw logs, PII, secrets, absolute local paths, or protected evidence through the routine Jev route. Unknown classification fails closed.
- Astra review is a separate user-authorized path, not routine Jev egress. One exact-SHA, design-only packet was expressly approved and sent; that consent is consumed and does not authorize another packet or code/source egress. A later implementation-review packet requires fresh, exact-SHA consent and must respect the evidence/redaction boundary in §5.1.
- No branch checkout/switch, write (except explicit `--out-dir` report output), build, test, stage, commit, push, VM/runtime operation, credential read, or network request occurs in the default CLI mode.
- Freeze a distinct exact SHA for each branch and a separate digest/status for dirty or untracked inputs. Never mix them into one “current” result.
- Use explicit local rules for deterministic checks; keep semantic JEV advice as a separate field and receipt.
- Require owner approval of the final SPEC before implementation. A design review or Jev opinion is not implementation, publication, deployment, or release authorization.
- GPT-6 Astra review: the design-only review of SPEC-071 v0.12-draft returned `AMEND`. This v0.13-draft records the closure changes but has not been reviewed by Astra. The prior verdict does not approve this revision or authorize code. A separate mandatory post-implementation review follows completed tests before declaring the tool validated; both stages require exact packet identity and fresh owner consent.

### 5.1 GPT-6 Astra review packet gates

#### Design-only review returned `AMEND` on v0.12-draft

The prior design-only review was bound to packet SHA-256 `439bab94579ad33c174afde2612031c9d0f1afbc593ebb903524c38ec1e246b7` and request-instructions SHA-256 `34ee329e2cdd2b0a6154cf534f081cb4811aac021882537fb8395252ff0f32b5`. Astra found the local-first direction and owner-selected boundaries sound in principle, but returned seven design blockers plus evidence limitations/optional hardening. Section 5.2 maps every must-fix finding to v0.13 clauses and acceptance tests. The exact response, raw SSE, transcript, usage record and packet are archived under `Specs/IMPACT/ASTRA-REVIEWS/SPEC-071-design-439bab94579a/`.

The response's `implementation authorization` field was malformed and has no legible value; it is treated as missing, not as permission. No implementation is authorized. V0.13 is a post-review working revision. The owner has now requested a design follow-up on v0.13; its packet is being prepared but is not frozen or sent, and fresh packet-specific consent is still required. Before any implementation, the owner must approve the reviewed SPEC and Astra must return `APPROVE` on the exact follow-up packet. The post-implementation validation remains a separate later gate.

#### Final implementation review

After implementation, tests and sample reports are complete, prepare a self-contained, stateless review packet. `Specs/IMPACT/SPEC-071-ASTRA-REVIEW-CONTEXT.md` is the preparation brief; it is not itself an Astra submission and its historical branch pins must be refreshed at review time.

The final packet must include and hash the exact bytes of:

- Final SPEC-071, the NG-071 task record and relevant Dashboard entry.
- The final implementation diff and every new/changed source, test, schema, fixture and report-format contract file only after each artifact is classified and its inclusion is permitted by the exact packet consent; otherwise list the omission and limit the review claims. No source, PII, secrets or protected evidence is egressed by default.
- Exact branch refs and resolved full HEAD SHAs for AIOPT3/AIOPT4/AIOPT5; current `git status --short`, dirty/untracked-file treatment, and explicitly excluded files.
- Test/validator commands, full outputs and exit codes; test/fixture inventory and actual counts; generated summary/per-SPEC sample reports; negative cases and known failures/limitations.
- The five NG-070 cases with exact original Git-object clauses, current branch variants, original/status-register snapshots, the relevant affected-file sidecars, and the exact detection rule/fixture/result for each.
- The final and prior SPEC-071 revisions, the exact v0.12 design packet/AMEND/response identity, the closure matrix, NG-071 task record, and relevant Dashboard row.
- The active Hermes `decision_query`/`assess_progress` contract excerpts and registration/pre-tool hooks relevant to the actual handoff, with exact source hashes and version; no credentials or profile secrets.
- `decision_query` boundary, sanitized example handoff/receipt if exercised, remote-call approval/cost treatment, no-egress operation and output-write behavior.

Immediately before dispatch, re-read the effective repository HEAD and branch, recompute the packet manifest SHA-256, and verify the submitted bytes exactly match the frozen packet. Check current review quota and any rate-card estimate, keeping subscription quota, flexible credits and estimated cost as separate quantities. Present the exact packet SHA and bounded review scope to the owner and obtain fresh explicit consent before any paid/external call. A changed input creates a new packet SHA and requires fresh consent; discard Astra findings against stale inputs. If dispatch may have started but the client times out, treat delivery/billing as unknown; do not retry without fresh consent for that exact packet. Do not send secrets, PII, raw logs or protected evidence. If material evidence is omitted or redacted, name the omission and limit the review claim accordingly.

Ask Astra for `APPROVE`, `AMEND` or `NO-GO` against the SPEC acceptance criteria, with numbered findings, exact file/section/line evidence, concrete false-PASS/false-negative risks, and any unresolved requirement. Preserve the exact response and packet identity. Resolve any `AMEND` in the same SPEC-071 lineage; Astra advice is not owner authorization. The tool is not declared validated until Astra's exact-SHA gate and owner acceptance are recorded.

### 5.2 Astra AMEND closure map — v0.13-draft

| Finding | v0.13 response | Closure evidence required |
|---|---|---|
| 1. Rule/acceptance matrix | §§2, 3.2 and 6 map the five seed cases to five classes plus the procedure/inventory subrules; attribution rules cover duplicates, co-change, renames and incomplete history. | Later tests must exercise each rule with positive, negative and insufficient-evidence fixtures; Astra must re-review the frozen v0.13 packet. |
| 2. Outcome/applicability/coverage conflation | §3.3 defines three axes and clean-run aggregation; §4 defines run states/exit codes. | Schema/validator and tests must reject false clean results when any required check is partial, skipped, unsupported, unknown or failed. |
| 3. Snapshot/evidence admissibility | §§3.1 and 4 make committed HEAD the default, separate optional worktree overlays, prohibit network/checkout, and define shallow/moving-input/limit behavior. | Tests must prove exact HEAD/tree binding, dirty-overlay separation, stale/unreadable/shallow-history INDETERMINATE and no snapshot mixing. |
| 4. CLI/schema/write safety | §4 and the Draft 2020-12 schema artifact freeze source arguments, complete stdout report, stderr failure behavior, output paths, cross-field exit mapping, canonical input manifest and output hash manifest, no-write default, limits, and fail-on-existing/symlink/path escape behavior. | Schema, semantic reconciliation, invocation tests, collision/symlink/path-escape/partial-write negatives, and exact input/output hash-manifest verification are required. |
| 5. Hermes handoff/Choice semantics | §3.4 and `$defs.query_handoff`/`$defs.direct_typesafe_handler_query` model the exact registered handler/adapter request with one `claims` item, typed Choice outcomes, one-shot gate and no auto-dispatch; §3.4 records the incompatible strict profile schema explicitly. | A non-live fixture must prove `validate_query` + `_bounded_request` accept the handler query, while the separate strict profile schema rejects `claims`; do not claim schema-conformance or permit remote dispatch until the owner resolves this host-contract mismatch. Inspect serialized Choice bytes for path/source exclusion. |
| 6. Progress evidence | §§3.5–3.6 and `$defs.progress_summary`/`$defs.progress_ledger` reuse the local contract; declared counts are not verified completion and the CLI never computes a percentage. | Tests must show no ledger without its owner-authored weight-approval reference, missing callback/verifier withholds percentage, unverified `met` becomes `unknown`, unknown gates remain in denominator, and Jev cannot affect results; the separate host receipt is not embedded in the auditor report. |
| 7. Privacy/egress | §5 distinguishes routine Jev from the exact-SHA Astra exception; query candidate and local evidence remain distinct from the narrowly built Choice payload, local reads are scoped, and live calls require the host one-shot consent/cost gate. | Negative tests for prohibited fields, path/PII/raw-content leakage, oversize summaries, stale approval, timeout/ambiguous dispatch, no retry/fallback; final Astra packet redaction manifest. |
| 8. Review evidence limitations | §5.1 requires original Git-object clauses, branch snapshots, full relevant sidecars and active Hermes contract excerpts in the future implementation packet. | Final packet must hash and include those artifacts or name each omission and narrow its review claims. |
| 9. Stale labels/counts | This SPEC uses v0.13 labels; the branch report's 64-sidecar count remains historical, not silently refreshed. | Context brief and report headings must label the reviewed revision/timeline; no historical source bytes are rewritten. |
| 10. Resource bounds | §4 sets file/count/history/summary limits; limit exhaustion is INCOMPLETE, never clean. | Tests at, below and above each bound, including deterministic truncation/abort behavior. |

“Addressed” here means the normative draft now states the response. It is not proof of implemented behavior, test passage, Astra re-approval, or implementation authorization.

## 6. Acceptance criteria (implementation gate; all provisional until owner approval)

- [ ] Owner approves this SPEC-071 revision; implementation still requires a separate exact-scope owner authorization in the governing task.
- [ ] Inputs are exactly the three explicit AIOPT3/AIOPT4/AIOPT5 source roots; each recorded HEAD/tree is pinned before scanning, committed content is default, and dirty/untracked overlay is separate and opt-in.
- [ ] The v1 rule matrix covers five owner-selected classes plus `PROCEDURE_ORDER` and `DATED_INVENTORY`; every rule has rule ID, prerequisites, evidence threshold, non-trigger, and expected positive/negative/unknown result.
- [ ] Each finding separates outcome, applicability, and execution axes. `COMPLETE_NO_DRIFT` is impossible if any planned applicable check is skipped, partial, unsupported, failed, unknown, or lacks required evidence.
- [ ] The Draft 2020-12 report schema, JSON/Markdown stdout contract, `stdout`/`stderr` routing, exit codes, filenames, input-manifest digest, output-manifest list and detached digest, summary/per-SPEC reconciliation, limits, no-write default, explicit `--out-dir`, collision/symlink/path-escape refusal, and partial-write failure behavior are frozen and validated.
- [ ] If `--task` is supplied, the task hash, exact `depends_on` SPEC filename, and branch/SPEC mapping are reported; task status and checkboxes alone never prove criterion completion.
- [ ] `progress_summary` labels checkbox/status counts as source declarations only and never includes a percentage; `progress_ledger` matches the existing `assess_progress` contract and is absent without an owner-authored weighted-scope reference. Without the trusted host snapshot/owner/evidence verifiers, no percentage is emitted; unknown criteria remain in the approved denominator and unverified `met` claims receive no numerator credit.
- [ ] The CLI never calls Jev, Astra, or a provider endpoint. It may emit a single-claim `decision_query_handoff` for the registered handler; a non-live fixture validates the nested request against the report schema, `validate_query`, and `_bounded_request`, and proves the separate strict profile schema's `claims` rejection is flagged. No remote dispatch is eligible until the owner resolves that schema/handler mismatch; the local scan remains usable.
- [ ] Local-read and egress limits are tested, including absolute-path/credential/PII/raw-content rejection, disabled JEV, stale approval, malformed/blocked/timeout response, and no retry/fallback. Default local scan remains usable without remote credentials.
- [ ] Tests cover each NG-070 motivating case and each rule, including duplicate IDs, historical vs active references, co-change-only evidence, renames, incomplete history, dirty overlays, missing/changed inputs, unsupported parse, and resource-limit exhaustion.
- [ ] Before implementation, v0.13 receives a separate exact-SHA design follow-up by GPT-6 Astra with `APPROVE`; the owner separately authorizes code. The existing v0.12 `AMEND` is not approval.
- [ ] After implementation and tests, the final exact-SHA tool packet receives GPT-6 Astra validation; all findings are resolved in the same SPEC-071 lineage and owner acceptance is recorded before calling the tool validated.

## 7. Decisions, review status, and remaining gates

Owner decisions recorded 2026-10-17:

- Branch scope: v1 scans AIOPT3/AIOPT4/AIOPT5 separately, each with its own exact pinned HEAD; never merge branch evidence into one snapshot.
- Integration/output: a local CLI emits a bounded JSON/Markdown evidence bundle; Hermes uses the registered `decision_query` on selected claims. No direct raw HTTP client is in scope.
- Remote data boundary: only sanitized claims, status metadata, hashes and short evidence summaries; no source code, raw logs, PII or protected evidence.
- Progress: report gate/status counts; emit a percentage only where the SPEC defines criteria and weights. Do not use Jev confidence as progress.
- Drift coverage: v1 requires all five classes—status/implementation-commit mismatch; current-path claims vs source; dangling/ambiguous references and branch applicability; declared affected paths vs commit changes; acceptance/test evidence vs current branch. Run deterministic checks first; evidence/semantic findings not mechanically established must be flagged for review, not auto-resolved.
- Report layout: produce both a consolidated summary and per-SPEC findings, with every branch snapshot kept separate.
- Persistence: default is a concise summary on stdout with no file writes. Only an explicit `--out-dir` may write the consolidated summary and per-SPEC report files.
- Review sequencing: GPT-6 Astra reviewed v0.12 on exact packet SHA `439bab94579ad33c174afde2612031c9d0f1afbc593ebb903524c38ec1e246b7` and returned `AMEND`. The owner has requested a v0.13 design follow-up; packet preparation is in progress and no new call has been sent. Fresh exact-SHA consent is required. Astra `APPROVE` and separate owner approval of the reviewed SPEC are required before code; a distinct final tool validation follows implementation/tests. No review result grants code/deployment authority.

Remaining gates:

1. Freeze/hash the exact v0.13 follow-up packet, obtain fresh exact-SHA consent, and receive Astra `APPROVE` for design readiness.
2. The owner decides whether the direct handler contract is accepted despite the strict profile-schema mismatch or authorizes a separate host-schema correction; remote egress stays disabled until then.
3. The owner reviews and approves the reviewed v0.13 SPEC; any further changes stay in SPEC-071.
4. Obtain separate owner implementation authorization in the active NG task; the current “Go ahead” authorized only documentary AMEND closure, not code.
5. After authorized implementation and tests, prepare a new final implementation packet and obtain the separately required Astra tool validation.

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
| v0.13-draft | 2026-10-17 | Closed Astra AMEND 1–7 in the same lineage; froze CLI/report/manifests and Draft 2020-12 schema, documented the handler `claims` route plus strict profile-schema mismatch/blocked-egress gate, separated progress summary/ledger, and defined review acceptance |

## 9. Reference basis

- Hermes local decision-stack contract: `hermes-local-decision-stack` and `references/typesafe-direct-payload.md`. Use the registered `decision_query` route; do not clone the provider API directly in the script.
- TypeSafe API reference (`Systemone`, `/v1/systemone`): one or more typed questions can be asked about supplied state, including Choice questions; consulted 2026-10-17.
- TypeSafe submitted-data/retention guidance: consult current terms and agreements before any live egress; policy page last updated 2026-10-11.

