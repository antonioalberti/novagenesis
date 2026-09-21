# SPEC-061: Read-Only Release-Readiness Evidence Orchestrator — R0–R2

**Author:** Antonio Alberti / Hermes Agent
**Date:** 2026-09-21
**Revision:** 6
**Status:** Draft
**Branch:** AIOPT3
**Component:** `Scripts/ProjectAudit`; release-readiness evidence inspection
**Implementation Commit:** pending
**Related:** `SPEC-054-release-master-plan-and-audit.md`; `SPEC-055-ng-elc-local-mode.md`; `SPEC-056-ng-elc-evidence-provenance-hardening.md`; `SPEC-057-ng-elc-canonical-entrypoint-rename.md`; `SPEC-060-ng-elc-remote-payload-evidence.md`; `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json`; `Scripts/ProjectAudit/schemas/ng-readiness-input-manifest-v1.schema.json`; `Scripts/ProjectAudit/schemas/ng-readiness-evidence-record-v1.schema.json`; `Scripts/ProjectAudit/schemas/ng-readiness-trusted-execution-profile-v1.json`; `Scripts/ProjectAudit/schemas/ng-readiness-adapter-registry-v1.schema.json`; `Scripts/ProjectAudit/validate_spec061_contract.py`; `Specs/SPEC-061-ng-readiness-policy-v1.json`; `Specs/SPEC-061-ng-readiness-adapter-registry-v1.json`
**Fixture:** `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json` (SHA-256 `2f7866f029c1e25041090b12e360e5f71efbda1fdf649bad48c355b6ddc71358`)
**Task linkage:** pending explicit task creation after SPEC review

---

## 1. Problem

The NovaGenesis release evidence chain is currently distributed across several components with different authority and side-effect contracts:

- `Scripts/ProjectAudit/ng_release_audit.py` audits plan/task/SPEC traceability and Git drift, but does not prove code-to-build-to-executable provenance.
- `Scripts/AlpineVMs/ng_remote_executor.py` is the NG Experiment Lifecycle Controller (NG-ELC) runtime entrypoint; it performs preflight, launch, observation, oracle evaluation, teardown and evidence capture.
- `Scripts/AlpineVMs/local_provenance.py` contains strong provenance/build-linkage helpers, but some helpers write snapshots or perform subprocess checks.
- `Scripts/AlpineVMs/evidence_verifier.py` verifies an existing schema-v2 runtime bundle; it does not establish R0–R2 before a trial.
- `Scripts/AlpineVMs/payload_evidence.py` validates payload plans and compares expected/observed maps, but does not orchestrate the complete local release-readiness decision.

This fragmentation requires manual reconstruction and creates risks of:

- treating a documentation `HEAD` as the functional SHA;
- treating an old build receipt or matching binary hashes as current build provenance;
- treating configuration values as proof of the effective runtime environment;
- treating JPEG-shaped bytes or current directory emptiness as qualified workload evidence;
- treating historical `PASS/PASS/COMPLETE` as current acceptance;
- allowing a tool to mutate the inputs it is supposed to inspect.

The current NG-060 decision remains explicitly:

```text
R0 = BLOCKED
R1 = OPEN, dependent on R0
R2 = BLOCKED
functional_sha = NO-SHA
```

The proposed tool must report this state rather than repair it, infer missing values or authorize an experiment.

This SPEC is about **NG-060 as a task** and its local R0–R2 evidence. It is not a replacement for `SPEC-060`, which governs the remote payload-evidence scope.

## 2. Root Cause Analysis

### 2.1 Gate authority and evidence inspection are coupled inconsistently

SPEC-054 defines the release gate index and high-level read-only audit. SPEC-055 and SPEC-056 define runtime and provenance contracts. No current SPEC governs a standalone, read-only evaluator that consumes existing evidence and produces a machine-readable R0–R2 assessment.

### 2.2 Existing provenance helpers are not all pure inspectors

`capture_plan_snapshot()` writes plan/configuration files. `capture_local_provenance()` can preserve manifests and snapshots. `validate_build_linkage()` may execute a manifest-selected compiler with `--version`. These behaviors are suitable for a controlled lifecycle capture path, but cannot be silently wrapped by a tool whose contract is read-only inspection.

### 2.3 Identity boundaries are easy to conflate

The following identities must remain separate:

```text
review HEAD
≠ functional SHA
≠ build-source SHA
≠ receipt-publication SHA
≠ checker identity
```

The current documentation commit `a2c0fe6994bf114f48b98400e3ec76d6d4fc8fab` is not a functional candidate. The historical receipt-backed source `4b01e5e258b7228958e027eb1df963048f6629c3` is not automatically the NG-060 candidate.

### 2.4 Current audit output does not distinguish all strategic blockers

The high-level audit can report generic gate states such as R0–R7 `OPEN` and overall `BLOCKED`. The R0–R2 evaluator must preserve that report separately from the strategic classification `R0 BLOCKED / R1 OPEN / R2 BLOCKED`, rather than overwriting one with the other.

## 3. Proposed Solution

Create a standalone read-only evaluator:

```text
Scripts/ProjectAudit/ng_readiness_evidence.py
```

The evaluator will inspect explicitly supplied, existing local artifacts and produce a deterministic, machine-readable readiness report for R0–R2. It will orchestrate **checks**, not the lifecycle that produces the evidence.

### 3.1 Decision: read-only R0–R2 scope first

**Decision:** Version 1 covers only R0, R1 and R2 for the NG-060 local normal path.
**Reason:** These gates are the current prerequisite boundary and can be evaluated without executing VM, SSH, QGA, build, payload generation, staging or runtime actions.

R3–R7 may receive separate adapters or amendments later. They are not part of this implementation.

### 3.2 Decision: explicit inputs, never automatic candidate selection

**Decision:** The checker requires explicit paths and identities and reports missing values as blockers.
**Reason:** A documentation `HEAD`, matching binary hash or historical receipt must never silently become the functional candidate.

Required or explicitly reported inputs include:

- repository path;
- review `HEAD` and remote branch reference;
- functional candidate SHA, if one exists;
- gate-definition revision and governing plan;
- original/expanded/effective plan and configuration evidence;
- build manifest/receipt;
- controller/helper identity records;
- role-to-executable records and executable hashes;
- expected workload map;
- existing Source/Repository maps and isolation/baseline evidence.

The checker does not select, freeze or amend any of these inputs.

### 3.3 Decision: separate report schema

**Decision:** Use a versioned readiness-report schema, distinct from NG-ELC `result.json`, manifest/seal schema-v2 and runtime acceptance.
**Reason:** A read-only assessment is not a runtime trial bundle and must not be mistaken for one.

The complete normative contract is `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json`. The following field list is a synchronized projection of that schema; the JSON Schema and policy registry remain authoritative.

| Field | Required shape |
|---|---|
| `schema_version` | integer `1` |
| `checker` | closed object: `path`, `sha256`, `pre_exec_verified`, `launcher_id`, `launcher_sha256`, `python_path`, `python_sha256`, `sandbox_profile_id`, `sandbox_profile_sha256`, `trust_anchor_id`, `trust_anchor_sha256` |
| `policy` | closed object: `id`, canonical `sha256`, raw-file `bytes_sha256`, `adapter_registry_id`, `adapter_registry_sha256` |
| `trust_anchor` | closed object: `id`, `sha256`, `verified_by_launcher: true` |
| `input_manifest` | closed object: `path`, raw manifest `sha256`, `policy_bytes_sha256`, `adapter_registry_sha256` |
| `phase` | exactly `PRE_TRIAL_READINESS` |
| `identities` | closed identity-slot object defined below |
| `inputs` | explicit input observations; no implicit discovery |
| `checks` | exactly one entry per policy check, including `applicability` and `reason_codes[]` |
| `dependencies` | check-ID edges with `dependency_effect: BLOCKING` in v1 |
| `gates` | exactly `R0`, `R1`, `R2` with their required check IDs and computed states |
| `summary` | computed assessment, exit code/class, blocking gates and source-document states |
| `runtime_authorized` | boolean, always `false` |
| `release_authorized` | boolean, always `false` |

The `identities` object MUST use explicit status slots rather than naked nullable identities:

```text
identity_slot_40/64 = {status: PRESENT|UNAVAILABLE, value: SHA|null, reason_code: CODE|null}
remote_ref = {status: PRESENT|UNAVAILABLE, name: ref|null, target_sha: SHA|null, reason_code: CODE|null}
functional_sha_display = NO-SHA when functional_sha.status=UNAVAILABLE and value=null;
```

A `PRESENT` slot requires a non-null value and a null reason code. An `UNAVAILABLE` slot requires a null value and a non-null reason code. The checker MUST NOT infer one identity from another.

Each `checks[]` record contains `id`, `gate`, `required`, `state`, `reason_code`, sorted unique `reason_codes`, `expected`, `observed`, `evidence_refs`, `depends_on` and `applicability`. Each `inputs[]` record additionally preserves `adapter_id`, `adapter_version`, `source_format`, `source_version`, `path_kind`, `input_format`, `availability_expectation`, `directory_role`, `producer_authentication` and stable-read metadata. `evidence_refs` MUST identify explicit input IDs, never free-form paths. `dependencies[]` records contain `check_id`, `depends_on` and `dependency_effect=BLOCKING`.

`state` MUST be one of `SATISFIED`, `UNSATISFIED`, `UNKNOWN`, `ERROR` or `NOT_APPLICABLE`. Reason codes MUST come from the closed registry in `Specs/SPEC-061-ng-readiness-policy-v1.json`; `NOT_APPLICABLE` is a predicate applicability state only and is never an evidence class. Gate states MUST be one of `SATISFIED`, `OPEN`, `BLOCKED`, `ERROR` or `NOT_APPLICABLE`. The computed overall assessment MUST be one of `SCOPED_SATISFIED`, `BLOCKED`, `ERROR` or `NOT_APPLICABLE`.

`UNKNOWN` and `ERROR` are blocking for a required predicate. `NOT_APPLICABLE` requires a policy-backed justification. The report contract is normative for this SPEC and is not a future placeholder.

The policy registry is also normative for semantic acceptance. Every check entry carries explicit `success_selectors`, `operator`, `expected` and `semantic_requirements`; a generic assertion that all roles pass is not a predicate. The v1 adapter registry deliberately has no adapter whose maximum effective class is `INDEPENDENTLY_CHECKED`: that class is not reachable until the external launcher, sandbox, Ed25519 trust anchor, per-kind attestation contract and positive fixture are provisioned and re-reviewed.

### 3.4 Decision: fail-closed side-effect boundary

The checker MUST:

- read inputs without modifying them;
- avoid network access;
- avoid privilege escalation;
- avoid shell-sourced environment files;
- avoid executing manifest commands, compilers, binaries or runtime entrypoints;
- restrict subprocess use to an audited local Git-inspection allowlist, if subprocesses are needed at all;
- reject unsafe symlinks, aliases, overlaps and mutable inputs;
- emit the report to stdout by default;
- permit an explicit report output path only when it is outside inspected inputs and runtime roots.

Report emission is the only allowed bounded output. The checker must not create trial directories, payloads, build directories, evidence bundles, seals or cleanup state.

If Git inspection requires a subprocess, v1 MUST use only an absolute, policy-recorded Git executable with `shell=False`, `close_fds=True`, a sanitized environment and the following exact argument templates (with validated placeholders only):

```text
/usr/bin/git --no-pager --no-optional-locks -C <absolute-repo> rev-parse --verify <40-lowercase-hex>^{commit}
/usr/bin/git --no-pager --no-optional-locks -C <absolute-repo> rev-parse --verify HEAD^{commit}
/usr/bin/git --no-pager --no-optional-locks -C <absolute-repo> rev-parse --verify <full-local-ref>^{commit}
/usr/bin/git --no-pager --no-optional-locks -C <absolute-repo> cat-file -t <40-lowercase-hex>
/usr/bin/git --no-pager --no-optional-locks -C <absolute-repo> cat-file -e <40-lowercase-hex>^{commit}
/usr/bin/git --no-pager --no-optional-locks -C <absolute-repo> -c core.fsmonitor=false -c core.untrackedCache=false status --porcelain=v1 -z --untracked-files=all --ignore-submodules=all
/usr/bin/git --no-pager --no-optional-locks -C <absolute-repo> ls-tree -r -z --full-tree <40-lowercase-hex>
```

`<full-local-ref>` MUST match an explicitly supplied local ref such as `refs/remotes/origin/AIOPT3`; remote access is never permitted. No other executable, subcommand, option, environment variable or argument form is allowed. The allowlist excludes fetch, pull, push, lazy object retrieval, hooks, filters, external diff/textconv, pager, terminal prompt, SSH transport and inherited Git command/configuration variables. A candidate object confirmed absent from the permitted local object store produces `R0.CANDIDATE_COMMIT=UNKNOWN/MISSING_INPUT` with `observed=null`; inspection failure retains its applicable error classification. No missing-object condition authorizes a fetch.

The subprocess environment MUST contain exactly an allowlisted absolute `PATH`, `GIT_TERMINAL_PROMPT=0`, `GIT_OPTIONAL_LOCKS=0`, `GIT_CONFIG_NOSYSTEM=1`, `GIT_CONFIG_SYSTEM=/dev/null`, `GIT_CONFIG_GLOBAL=/dev/null`, `GIT_NO_LAZY_FETCH=1`, `LC_ALL=C` and `LANG=C`; inherited `GIT_*`, pager, SSH, diff/filter and tracing variables MUST be removed. This environment is valid only inside the required `SANDBOXED_GIT` boundary in §11.6.1.
The machine-readable `git_probe_contract` in the policy is the exact command/return-code/stdout/stderr contract; this Markdown list is explanatory and cannot broaden it.

The closed Git execution profile in §11.6.1 is the only authorization contract: it specifies the supported executable/version identity, repository-metadata rejection rules, sandbox, no-lazy-fetch behavior and exact environment. A prose prohibition without that profile is insufficient; an implementation must fail closed if any profile condition is unavailable.

Reuse review MUST cover module initialization, imports and transitive calls, not only named functions. The checker launch contract MUST prevent bytecode/cache writes before project imports and exclude unreviewed startup/import hooks. Any dependency capable of hidden execution or writes MUST be removed, isolated by the approved boundary, or rejected.

Inputs need not be permanently immutable. They MUST satisfy the stable-read contract for this inspection. Evidence files MUST be regular files; special files and symlink traversal in selected paths, including ancestor components, are prohibited.

The checker MUST bind each read to a safely opened file descriptor, compare its identity with the selected path identity, and compare descriptor identity, size, `mtime_ns` and `ctime_ns` before and after reading. Root identity and directory-enumeration stability MUST also be checked where emptiness, maps or isolation affect a verdict. Detected changes produce `UNSTABLE_INPUT` and exit class `UNSAFE_INCOMPLETE`. These checks do not prove an atomic multi-file snapshot or future immutability.

Hard-link sharing among distinct selected evidence paths is prohibited except for the explicitly permitted shared-executable case. Output containment MUST be checked against canonical directory identities, including ancestor aliases. File output MUST use exclusive, no-follow creation in a validated existing parent, without creating parent directories or overwriting any entry. Unsafe races or unavailable platform safeguards MUST fail closed.

### 3.5 Decision: reuse only pure, reviewed predicates

The implementation must maintain a reuse/applicability matrix:

| Existing component | Reuse rule |
|---|---|
| `ng_release_audit.py` | Reuse concepts and/or pure parsers only after review; preserve its generic audit result separately |
| `local_provenance.file_identity()` | Candidate for reuse after side-effect review |
| `local_provenance.capture_git_state()` | Candidate for a read-only adapter; no content-snapshot writing mode |
| `local_provenance.capture_plan_snapshot()` | Do not call unchanged; it writes files |
| `local_provenance.validate_build_linkage()` | Do not call unchanged if it executes compiler/subprocess checks; extract or duplicate only pure validation under explicit review |
| `local_provenance.capture_local_provenance()` | Do not call unchanged; it may write snapshots and preserve manifests |
| `evidence_verifier.verify_bundle()` | May inspect an existing bundle as supporting evidence, but cannot close R0–R2 or convert historical acceptance into current PASS |
| `payload_evidence.validate_payload_plan()` / `compare_payload_maps()` | Candidate for reuse after full semantic and side-effect review; missing maps remain blockers |
| `ng_remote_executor.py` | Do not invoke; it remains the runtime NG-ELC authority |

Any unsupported or unsafe reuse becomes an explicit blocker, not a silently omitted check.

### 3.6 Supported evidence records and trust classification

Version 1 MUST consume only explicitly selected canonical JSON record files or explicitly selected directory observations. A directory observation is selected directly by a manifest entry with `path_kind=DIRECTORY`; a JSON wrapper with the same identity is not an alternative source of truth. Each canonical record MUST be one top-level object with no duplicate keys, `schema_version: 1`, no unknown fields, deterministic arrays and a `record_digest`. The checked-in closed contracts are `Scripts/ProjectAudit/schemas/ng-readiness-evidence-record-v1.schema.json` (SHA-256 `080e77b8c3883801221129410b754aac300e734d72b1c3d0adbce69376b978d0`), `Scripts/ProjectAudit/schemas/ng-readiness-adapter-registry-v1.schema.json` (SHA-256 `8fbb7531489a20808133e3d82c7414e6ca6aea7469883e4fe2e9a73aff48212f`) and `Specs/SPEC-061-ng-readiness-adapter-registry-v1.json` (SHA-256 `12bbaa2994af209519d56b8da47fb3746a0afc56063ba67e359786a9ca7fa981`). The registry defines canonical source bytes, payload-branch validation, namespace resolution and the external producer-attestation contract. An input manifest selects records and adapters; the checker MUST NOT discover arbitrary files by filename or accept an adapter not present in that registry.

Every canonical record uses this common envelope; manifest selections additionally carry adapter metadata (`adapter_id`, `adapter_version`, `source_format`, `source_version`, `path_kind`, `input_format` and `availability_expectation`).

```text
record_id: string, unique within the input manifest, [a-z0-9][a-z0-9_.-]{0,63}
record_kind: one supported kind listed below
schema_version: integer, exactly 1
candidate_binding: object (functional_sha, build_source_sha, plan_sha256,
  effective_config_sha256, workload_expected_sha256, phase)
producer: object (id, version, identity_sha256, attestation_ref)
evidence_class_claim: enum RECORDED_CLAIM | INDEPENDENTLY_CHECKED |
  HISTORICAL_ONLY | UNVERIFIED
artifact_refs: array of explicit referenced artifacts
payload: kind-specific object
```

Every `artifact_refs[]` entry MUST contain `id` (unique within the record), `role`, `kind`, absolute `path`, `required` boolean, `size`, `sha256`, `tree_sha256` and `digest_coverage`. Regular-file byte digests and directory-tree digests are different domains; a directory has no byte SHA-256. The descriptor's observed identity is checked against the selected expected identity and the canonical record digest; it MUST NOT rely on a self-declared digest inside the artifact.

`candidate_binding` MUST contain `functional_sha` and `build_source_sha` as full 40-lowercase-hex strings or null, `plan_sha256`, `effective_config_sha256` and `workload_expected_sha256` as 64-lowercase-hex strings or null, and `phase: "PRE_TRIAL_READINESS"`. Required nullability is decided by the predicate policy, not inferred from the record kind.

The supported v1 record kinds and exact payload fields are:

| Record kind | Closed required payload fields |
|---|---|
| `freeze-r0-r2` | `review_head`, `remote_ref`, `functional_sha`, `governing_plan_ref`, `effective_config_ref`, `workload_expected_ref`, `profile`, `recorded_context`, `phase` |
| `effective-config` | `profile`, `values`, `values_sha256`, `recorded_context`, `source_refs` |
| `build-manifest` | `build_source_sha`, `receipt_ref`, `toolchain_ref`, `helper_identity_refs`, `executable_identity_refs`, `recipe_ref`, `working_directory`, `build_linkage` |
| `build-receipt` | `receipt_id`, `source_sha`, `recipe`, `return_codes`, `toolchain_ref`, `binary_refs`, `receipt_status` |
| `toolchain` | `compiler_id`, `compiler_version`, `compiler_path`, `compiler_sha256`, `linker_id`, `linker_version`, `linker_path`, `linker_sha256`, `build_system_id`, `build_system_version` |
| `helper-identity` | `module_path`, `source_sha256`, `imported_by`, `transitive_imports`, `side_effect_review` |
| `executable-identity` | `role`, `path`, `size`, `sha256`, `aliases`, `identity_status` |
| `workload-expected` | `map_id`, `items`, `map_sha256`, `generator` |
| `workload-source` | `source_ref`, `items`, `generator_ref`, `source_status` |
| `repository-baseline` | `captured_before_trial`, `root`, `baseline_status`, `entries`, `entries_sha256` |
| `jpeg-qualification` | `decoder`, `criteria`, `items` |
| `payload-copy` | `root`, `items`, `source_ref`, `copy_preservation` |
| `git-repository` | `root`, `git_dir`, `config_digest`, `metadata_closure`, `sandbox_profile`, `unsupported_modes` |
| `producer-attestation` | `algorithm`, `producer_id`, `producer_identity_sha256`, `subject_ref`, `subject_sha256`, `policy_sha256`, `statement_sha256`, `public_key`, `signature`, `trust_anchor_id`, `trust_anchor_key_id` |
| `capture-attestation` | `subject_refs`, `statement_sha256`, `captured_before_trial`, `capture_sequence`, `producer_attestation_ref` |
| `release-audit` | `schema_version`, `read_only`, `branch`, `head`, `worktree`, `gates`, `issues`, `summary`, `verdict` |
| `bundle-verification` | `schema_version`, `code`, `integrity`, `accepted`, `checked_files`, `errors`, `bundle_ref` |
| `governing-plan` | `path`, `size`, `sha256`, `source_status` |
| `directory-observation` | `path`, `directory_role`, `availability`, `directory_identity`, `entry_count`, `entries_sha256`, `stable_read` |
Each item object MUST contain `name` (safe basename), `size` (positive integer) and `sha256` (64 lowercase hex); item arrays MUST be sorted lexicographically by `name` and contain no duplicate names. The v1 workload policy fixes the five names to `spec056-photo-01.jpg` through `spec056-photo-05.jpg`; a different set is invalid for NG-060 R2.

Input-provided `evidence_class_claim` is a claim, not a trust grant. The checker MUST derive and report `effective_evidence_class`. Rehashing an artifact proves its observed byte identity, not the truth of its build, baseline, runtime-context or decoder claims. `INDEPENDENTLY_CHECKED` is not reachable in v1. The checker MUST report it as unavailable until the externally provisioned launcher, sandbox, trust anchor, per-kind attestation contract and positive fixture are enabled by a separately reviewed packet. In the current v1 registry, the maximum effective class is at most `RECORDED_CLAIM`, `HISTORICAL_ONLY` or `UNVERIFIED`; schema validation, rehashing or a self-supplied key cannot promote evidence. `NOT_APPLICABLE` is never an evidence class.

The v1 policy accepts JPEG qualification only from a `jpeg-qualification` record whose decoder name is `Pillow`, whose version and immutable module identity SHA-256 are present, whose record producer is policy-accepted, whose full-decode result is true for each exact payload hash, and whose positive dimensions are recorded for all five items. A structurally valid but unqualified declaration, marker-shaped bytes, extension or producer claim cannot satisfy R2.

Two non-null source identities in the §11.5.4 equality set that differ produce `UNSATISFIED`/`CROSS_SOURCE_SHA_MISMATCH`. Missing required bindings produce `UNKNOWN`/`BINDING_MISSING`. Neither condition authorizes reconciliation. The mismatch attribution set is limited to the explicit equality set; receipt-publication, review-head and remote-ref target identities remain separate Git identities.

`receipt_publication_sha` identifies where the receipt was published; it is not the receipt's attested source SHA and need not equal it. Review HEAD, remote-ref target and receipt-publication SHA MUST NOT be subjected to source-equality requirements merely because they are Git identities. Missing required bindings remain blocking. Artifact SHA-256 values are not Git source identities.

The checker MUST distinguish the evidence file's recorded context from the checker process context. In particular, the checker EUID, current environment and current directory do not prove the EUID, environment or directory of a future NG-ELC role.

## 4. Normative R0–R2 Evaluation Contract

### 4.0 Deterministic aggregation algorithm

The checker MUST execute the following total algorithm; no report or partial gate result is valid before each preceding phase succeeds:

1. Parse UTF-8 JSON with duplicate-key rejection, validate the report/input-manifest schemas and policy identity, validate all limits, and bind CLI values to the selected manifest as defined in §6.2. Any failure is `CHECKER_ERROR` or `UNSAFE_INCOMPLETE` according to the policy registry.
2. Validate the check DAG and gate DAG before evaluating any check. Reject self-edges, duplicate edges, unresolved IDs, mismatched check/dependency sets, invalid gate IDs, cross-layer cycles and `INFORMATIONAL` edges in v1. Emit no computed gate state from an invalid graph.
3. Verify the externally provisioned launcher, checker, pinned Python, Git executable, sandbox profile, adapter registry and producer trust anchor against the trusted-execution profile before any Git child or readiness assessment. If a trusted prerequisite is absent or unverified, emit only the separate diagnostic failure envelope and the policy exit class; never fabricate a schema-conforming readiness report. In v1, no evidence can reach `INDEPENDENTLY_CHECKED` because the enablement contract is intentionally external and disabled.
4. Observe every selected input through the descriptor-relative stable-read algorithm. Preserve unavailable inputs as report records with null observed fields and the policy-selected reason code.
5. Evaluate checks in topological order. A check with a direct error is `ERROR`; a direct missing, malformed, mismatched or unsafe required input receives the state and reason prescribed by the policy registry. A check is dependency-deferred only when all of its non-satisfied prerequisites are blocking dependencies and it has no independent direct incident; it is then `UNKNOWN` with `DEPENDENCY_BLOCKED` and null observed data.
6. Aggregate direct check states into each gate. Direct `ERROR` yields gate `ERROR`; direct `UNSATISFIED` or non-deferred `UNKNOWN` yields `BLOCKED`; all required checks satisfied or policy-justified not-applicable yields a candidate `SATISFIED` state.
7. Apply gate dependencies. A gate with no direct error/blocker but a required non-satisfied dependency is `OPEN`; a gate is `SATISFIED` only when its direct checks and all required gate dependencies are satisfied. `NOT_APPLICABLE` is permitted only where the policy registry marks the check/gate not applicable.
8. Set `blocking_gate_ids` to the sorted IDs of every required gate whose final state is not `SATISFIED` or policy-justified `NOT_APPLICABLE`. `OPEN` is therefore included because it is non-satisfied.
9. Apply exit precedence `40 > 30 > 20 > 0`: any unsafe/incomplete condition yields `UNSAFE_INCOMPLETE`/40; otherwise any schema/checker/graph error yields `CHECKER_ERROR`/30; otherwise any required `OPEN` or `BLOCKED` gate yields `BLOCKED`/20; only all required gates satisfied yields `SCOPED_SATISFIED`/0; an all-inapplicable policy scope yields `NOT_APPLICABLE`/0. Any exit-30 or exit-40 condition sets `summary.assessment=ERROR`; affected gates are `ERROR` when attribution is valid.
10. Serialize arrays and objects in the order defined by the report schema and emit the report only after all checks and serialization succeed.

Before a valid graph, policy identity and trusted checker/Git execution identity exist, the checker MUST return the applicable nonzero exit without a schema-conforming readiness assessment or fabricated gate states. A diagnostic failure envelope, if emitted, is a separate error artifact and MUST NOT be presented as a readiness report. Dependency-deferred checks are excluded from independent-direct-blocker detection but are never counted as satisfied; a gate may become `OPEN` only through its recorded unsatisfied gate dependency and only when it has no independent direct incident. Multiple incidents MUST be retained in deterministic order in `reason_codes[]`/diagnostics; the single `reason_code` field is the highest-priority code selected by policy.

Check-level `depends_on` and `dependencies[]` MUST agree exactly; references MUST resolve uniquely and dependency cycles MUST produce `DEPENDENCY_CYCLE`/30. Source-document states never determine computed states. The current identified fixture has this expected propagation trace: `R0.CANDIDATE_EXPLICIT=UNKNOWN/MISSING_INPUT` ⇒ `R0=BLOCKED`; required missing R1 evidence produces direct blockers, so `R1=BLOCKED` (the source-document state may remain `OPEN` in `source_document_states`); `R2.WORKLOAD_EXPECTED=UNKNOWN/MISSING_INPUT` has no R0 check dependency ⇒ `R2=BLOCKED`; summary=`BLOCKED`, exit=`20`, blocking gates=`[R0,R1,R2]`. This trace is fixture-specific and MUST NOT be hard-coded for other manifests.

### R0 — Exact-SHA and effective-plan boundary

The checker MUST evaluate, without selecting or freezing:

1. Whether an explicit functional candidate SHA was supplied.
2. Whether the candidate commit exists and is a commit object.
3. Whether review `HEAD`, remote branch ref, functional candidate, receipt-publication commit and checker identity are recorded separately.
4. Whether the governing plan, scenario and effective-configuration evidence exist and have stable file identities.
5. Whether the plan/profile/CLI identity claims are internally consistent in the supplied evidence.
6. Whether missing effective local paths, EUID evidence or freeze evidence are reported as `UNKNOWN`/`UNSATISFIED`.
7. Whether no candidate is inferred from documentation `HEAD`, old receipts or binary hashes.

The checker MUST NOT claim that configuration values prove a future process environment or privilege level.

### R1 — Build receipt and code-to-executable linkage

The checker MUST evaluate existing evidence for:

1. source/build candidate identity;
2. controller and imported-helper paths and hashes;
3. build recipe, working directory, options and toolchain records already preserved;
4. build receipt identity and preservation;
5. role-to-executable mapping with absolute paths, sizes and SHA-256;
6. executable basename/path collision and alias checks;
7. agreement between manifest source identity and supplied candidate identity;
8. explicit cross-SHA mismatch or unsupported equivalence;
9. missing receipt, manifest, helper identity or executable identity.

The checker MUST NOT execute a new build, compiler, executable or manifest command. Matching historical executable hashes alone MUST NOT satisfy R1.

### R2 — Workload and Source/Repository evidence

R2 is a **pre-trial readiness gate**. It MUST NOT be conflated with post-trial payload delivery acceptance.

The checker MUST evaluate existing evidence for:

1. a non-empty expected map containing exactly the five NG-060 workload entries, safe names, positive sizes and valid SHA-256 values;
2. a freshness binding from the expected map and the preserved Source evidence to the explicit candidate SHA, configuration identity and workload generator/seed/parameters;
3. the preserved Source evidence record, with stable file identities and exact bindings to the expected map;
4. explicit Source/Repository root isolation, non-aliasing and non-nesting evidence;
5. a bound pre-trial Repository baseline proving the state of the selected root before any delivery attempt;
6. JPEG qualification evidence bound to the exact five file hashes, including decoder/qualification identity, validity and dimensions;
7. independently rehashable copies or references to preserved copies.

Observed Repository maps and post-trial delivery comparisons are outside v1. An empty pre-trial Repository baseline MUST NOT be compared with the non-empty expected map as a delivery failure; post-trial matching is not a prerequisite for pre-trial R2 and is governed by a future SPEC/adapter.

Markers, file extensions, “JPEG-shaped” bytes and unqualified declarations cannot satisfy JPEG qualification. The checker MUST NOT execute a decoder to create qualification evidence in v1; it may consume an explicitly selected `jpeg-qualification` record bound to the exact file hashes.

The checker MUST NOT generate files, create roots, stage IO, infer a prior empty baseline from current emptiness or claim post-trial payload acceptance from pre-trial records.

### 4.2.1 v1 PRE_TRIAL_READINESS phase firewall

Every selected record, payload and binding MUST resolve to `PRE_TRIAL_READINESS`. `repository-baseline.captured_before_trial` MUST be `true`; a missing or false value is `PHASE_MISMATCH`/20. Records describing runtime results, delivery observations, post-trial Repository maps, `result.json`, terminal acceptance, schema-v2 runtime bundles or any `POST_TRIAL` phase are rejected with `POST_TRIAL_EVIDENCE`/20 and cannot satisfy R0–R2. Preserved copies are admissible only when explicitly bound as pre-trial Source evidence; post-trial payload acceptance is outside this tool and requires a separate SPEC/adapter.

## 5. Report and Exit Contract

### 5.1 JSON report shape

The following is a non-normative shape sketch only; it is not a report instance and MUST NOT be validated as one. The complete conforming contract is the checked-in JSON Schema `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json` (SHA-256 `ecfe485db6e40e978872891cd190af7438b258ccd1f432fe8c3b6051f081f333`) plus the policy registry.

```json
{
  "schema_version": 1,
  "checker": {"path": "<absolute>", "sha256": "<64-hex>", "pre_exec_verified": true, "launcher_id": "ng060-trusted-launcher-v1", "launcher_sha256": "<64-hex>", "python_path": "<absolute>", "python_sha256": "<64-hex>", "sandbox_profile_id": "ng060-sandbox-v1", "sandbox_profile_sha256": "<64-hex>", "trust_anchor_id": "ng060-producer-trust-anchor-v1", "trust_anchor_sha256": "<64-hex>"},
  "policy": {"id": "ng060-r0-r2-read-only-v1", "sha256": "<64-hex>", "bytes_sha256": "<64-hex>", "adapter_registry_id": "ng-readiness-adapter-registry-v1", "adapter_registry_sha256": "<64-hex>"},
  "trust_anchor": {"id": "ng060-producer-trust-anchor-v1", "sha256": "<64-hex>", "verified_by_launcher": true},
  "input_manifest": {"path": "<absolute-input-manifest>", "sha256": "<64-hex>", "policy_bytes_sha256": "<64-hex>", "adapter_registry_sha256": "<64-hex>"},
  "phase": "PRE_TRIAL_READINESS",
  "identities": {
    "review_head": "<40-hex-or-null>",
    "remote_ref": {"name": "refs/remotes/origin/AIOPT3", "target_sha": "<40-hex-or-null>"},
    "functional_sha": null,
    "functional_sha_display": "NO-SHA",
    "build_source_sha": "<40-hex-or-null>",
    "receipt_publication_sha": "<40-hex-or-null>",
    "checker_identity": "<64-hex>"
  },
  "inputs": [],
  "checks": [],
  "dependencies": [],
  "gates": {"R0": {}, "R1": {}, "R2": {}},
  "summary": {
    "assessment": "BLOCKED",
    "exit_code": 20,
    "exit_class": "BLOCKED",
    "blocking_gate_ids": [],
    "source_document_states": {}
  },
  "runtime_authorized": false,
  "release_authorized": false
}
```

The report MUST include separate source-document statuses and computed gate states, plus `phase: "PRE_TRIAL_READINESS"`. Post-trial payload acceptance is outside the v1 report contract and MUST NOT be claimed by this tool.

### 5.2 Exit codes

The implementation MUST use this numeric exit-code table:

| Exit code | Class | Meaning |
|---:|---|---|
| `0` | `SCOPED_SATISFIED` | All required scoped predicates are satisfied; this is not operational authorization or release acceptance |
| `20` | `BLOCKED` | One or more required predicates are `UNKNOWN`, `UNSATISFIED`, or dependency-blocked |
| `30` | `CHECKER_ERROR` | Input, schema, policy or checker error prevented a valid assessment |
| `40` | `UNSAFE_INCOMPLETE` | Side-effect boundary, unstable input, unsafe path/alias or incomplete inspection was detected |
| `0` | `NOT_APPLICABLE` | All scoped predicates are policy-defined not-applicable; this does not authorize operation |

Precedence is `40 > 30 > 20 > 0`. `NOT_APPLICABLE` does not block only when policy-justified. Missing optional evidence is allowed only when the selected policy marks it optional and the report records that applicability decision.

Exit code zero MUST NOT mean operational authorization, release readiness or permission to run a trial.

### 5.3 Determinism and observation identity

For identical identified inputs, policy bytes and checker identity, substantive findings, reason codes, gate states and exit class MUST be identical. Verdict-relevant Git refs, filesystem identities, bytes, sizes, timestamps used for stability checks and other observations are part of the identified input set and MUST be recorded in the report. Only non-decision metadata such as report creation time and duration may vary without changing findings.

## 6. Implementation Details

### 6.1 Proposed files

| File | Change | Rationale |
|---|---|---|
| `Scripts/ProjectAudit/ng_readiness_evidence.py` | New standalone read-only checker | Avoid coupling inspection to NG-ELC runtime |
| `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json` | New report schema | Versioned machine-readable contract |
| `Scripts/ProjectAudit/tests/test_ng_readiness_evidence.py` | New unit/fixture tests | Prove fail-closed behavior and side-effect confinement |
| `Scripts/ProjectAudit/tests/fixtures/ng-readiness/` | Synthetic input fixtures | Exercise satisfied, missing, ambiguous and tampered cases without runtime |
| `Specs/RESULTS-SPEC-061/` | Future retained reports/reviews | Keep implementation evidence separate from runtime bundles |

No `Scripts/AlpineVMs/ng_remote_executor.py` or C++ production file is modified by this SPEC v1.

### 6.2 Proposed CLI shape

The v1 CLI MUST require explicit inputs and MUST make the candidate absence explicit:

```text
ng060-trusted-launcher-v1 \
  --python /home/gandalf/.hermes/hermes-agent/venv/bin/python3 \
  --checker /home/gandalf/workspace/novagenesis/Scripts/ProjectAudit/ng_readiness_evidence.py \
  --repo <absolute-repository> \
  --plan <absolute-plan> \
  --candidate-sha <40-lowercase-hex-or-omit> \
  --input-manifest <absolute-existing-input-manifest> \
  --manifest-sha256 <64-lowercase-hex> \
  --policy /home/gandalf/workspace/novagenesis/Specs/SPEC-061-ng-readiness-policy-v1.json \
  --policy-sha256 <RFC8785-canonical-policy-digest> \
  --policy-bytes-sha256 <raw-policy-file-digest> \
  --adapter-registry /home/gandalf/workspace/novagenesis/Specs/SPEC-061-ng-readiness-adapter-registry-v1.json \
  --phase PRE_TRIAL_READINESS \
  --report <new-absolute-output-path>
```

The command above is a launcher interface contract, not an executable command: the launcher is not provisioned in this Draft and the checker does not exist yet. The launcher MUST verify its own immutable identity, the checker bytes, the pinned Python bytes, `/usr/bin/git`, the trusted execution profile and the externally provisioned producer trust anchor before spawning Python.

The selected manifest, `--repo`, `--plan`, `--candidate-sha`, `--phase`, policy path/digest, raw policy-file digest and adapter-registry path/digest MUST agree exactly with the manifest and trusted launcher pins. Any disagreement is `IDENTITY_MISMATCH` with exit code `30`; no precedence, merge or default is permitted. The report MUST identify the exact input-manifest path and both policy digest domains. `--candidate-sha` omitted means manifest `candidate_sha: null`; an explicit CLI candidate cannot override a null or different manifest value.

No implicit environment-file loading, shell evaluation, network access, runtime-controller invocation, output overwrite, or evidence-root discovery is allowed. `--report-stdout` may be supported only when stdout is the sole caller-owned output and is not treated as an inspected evidence path; `--report` and `--report-stdout` are mutually exclusive.

The implementation MUST preserve the process boundary described in §3.4. Tests and syntax checks MUST use `-B` (and `PYTHONDONTWRITEBYTECODE=1` for non-isolated test invocations), disable pytest cache providers and place any temporary test directory outside the repository and all inspected roots. Bytecode-compiling commands, normal pytest cache creation and other commands that write repository-local bytecode/cache are prohibited by the acceptance procedure.

### 6.3 Migration and rollback

The first implementation is additive and independent of the NG-ELC runtime. It must not change existing acceptance semantics, master-plan gates, SPEC statuses or task states automatically.

Rollback is a normal documentation/code revert of the SPEC-061 implementation commit. Historical evidence and existing bundles must remain untouched.

### 6.3 Trusted launcher, sandbox and producer trust-anchor preconditions

The checker is not independently trusted merely because its selected bytes have a digest. Proposal requires the external provisioning contract in `Scripts/ProjectAudit/schemas/ng-readiness-trusted-execution-profile-v1.json` and a separately provisioned `ng060-trusted-launcher-v1` at `/usr/local/libexec/ng060-trusted-launcher-v1`. The launcher MUST perform no-follow, root-owner, mode, held-descriptor, byte-digest and TOCTOU identity checks for the launcher, checker, pinned Python interpreter (`/home/gandalf/.hermes/hermes-agent/venv/bin/python3`, SHA-256 `572cfa2a8597e2f431c785613539b1d219fa99eecf5ffb5765731a2ebc3e692a`) and `/usr/bin/git` (SHA-256 `2a8c18fbf43da9f692d75474c72bea9dfd796c260b0f3dfe456376abc3bbd668`) before spawning the checker. It MUST establish the declared sandbox before Python starts and MUST return nonzero without report output when any precondition fails.

The producer trust anchor is an out-of-band immutable Ed25519 keyset at `/etc/novagenesis/spec061/producer-trust-anchor-v1.json`. Its provisioned SHA-256, key IDs and launcher verification result are intentionally not invented in this Draft. A record-supplied public key is an optional equality echo only; it is never a verification source. Until the launcher, sandbox and trust-anchor identities are provisioned and verified, `INDEPENDENTLY_CHECKED` cannot be accepted and this SPEC cannot advance to Proposal.

## 7. Verification

### 7.1 Acceptance Criteria

- [ ] For the named negative fixture, source-document states remain R0 `BLOCKED`, R1 `OPEN`, R2 `BLOCKED`, while computed evaluator gates are R0 `BLOCKED`, R1 `BLOCKED`, R2 `BLOCKED`; another input set is assessed from its own evidence rather than hard-coded to this result.
- [ ] The checker never infers a functional SHA from documentation `HEAD`, old receipts or binary hashes.
- [ ] Missing candidate, effective configuration, receipt, helper identity, workload qualification or isolation evidence cannot default to success.
- [ ] The checker does not modify inspected inputs, runtime roots, source files, task notes, SPECs, Dashboard or master-plan files.
- [ ] The checker performs no network access, privilege escalation, build, compiler execution, binary execution, manifest-command execution, payload generation, staging or runtime invocation.
- [ ] Only an audited local Git-inspection subprocess allowlist is permitted, if subprocesses are required.
- [ ] Unsafe aliases, symlinks, overlaps, mutable inputs and ambiguous executable mappings block the relevant check.
- [ ] Report schema, checker identity, policy identity, input identities and reason codes are present.
- [ ] `UNKNOWN` and `ERROR` block required R0–R2 predicates; `NOT_APPLICABLE` requires justification.
- [ ] Exit classes distinguish satisfied scope, blocked/unsatisfied scope, checker/input errors and unsafe/incomplete inspection, with documented precedence and optional-evidence handling.
- [ ] Identical inputs and policy yield identical substantive findings.
- [ ] Historical PASS, valid bundle integrity and matching old binary hashes cannot become current gate satisfaction.
- [ ] Reused predicates are documented with source, side-effect behavior, governing SPEC and applicability.
- [ ] Negative fixtures cover absent candidate, stale receipt, source-head divergence, missing effective config, unsafe roots, invalid expected map, JPEG qualification gap and side-effect attempts.
- [ ] V1 does not claim an `INDEPENDENTLY_CHECKED` positive fixture; the packet records the absence explicitly. Any future enablement MUST add a per-kind attested positive fixture, external trust material and a new Astra review before changing that status.
- [ ] Tests cover import-time effects, indirect Git execution, input mutation/change detection, output-path aliases/overwrite attempts, gate/dependency aggregation and exit-code precedence.
- [ ] `python3 Scripts/ProjectAudit/validate_spec061_contract.py --manifest Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json` passes as the SPEC-061-scoped validator; the repository-wide `Scripts/Simple/validate-specs.sh` result is recorded separately and is not silently claimed as passed.
- [ ] Astra reviews the complete implementation diff and actual report before the tool is used to support a gate decision.

### 7.2.1 Confined acceptance contract

Future implementation only; no commands are authorized by this Draft. The acceptance suite MUST be launched by the reviewed `ng060-trusted-launcher-v1` profile, with the pinned interpreter and reviewed pytest distribution explicitly exposed by that launcher. The launcher MUST create a fresh external scratch parent, verify its descriptor identity is disjoint from the repository and every inspected root, and pass a previously nonexistent child as pytest basetemp. The suite may write synthetic fixtures and harness output only beneath that scratch parent; it MUST distinguish harness scratch writes from checker report output and MUST NOT clear or reuse pre-existing paths.

The exact launcher/bootstrap invocation, reviewed pytest dependency closure and plugin allowlist MUST be supplied before Proposal. Until then, the following are acceptance requirements rather than executable commands:

- AST syntax parsing uses the pinned interpreter with `-I -B -S` and no bytecode/cache writes.
- pytest is run only through the reviewed launcher; ambient site initialization, `.pth` processing, plugin autoload and repository-local imports are disabled.
- the checker is exercised with a caller-owned report path created exclusively by the checker under the validated parent.
- actual test counts, report bytes, output identity and filesystem mutation observations are retained as evidence.

The focused suite MUST NOT invoke `ng_release_audit.py` until its transitive Git, import, output, timeout and environment contract is separately reviewed.

### 7.2.2 SPEC-061-scoped packet validator

The documentation-only packet validator is `Scripts/ProjectAudit/validate_spec061_contract.py`. It is read-only, does not execute Git, runtime, compiler, VM, SSH, QGA or network operations, and validates JSON Schema meta-contracts, registry/manifest coverage, exact policy/schema/record digests, selected file identities, the excluded-scope directory observation, plan source/record separation, explicit predicate selectors, DAG cardinality and the external-only trust boundary. The exact command is:

```text
python3 Scripts/ProjectAudit/validate_spec061_contract.py --manifest Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json
```

Its `SPEC061_VALIDATION=PASS` output is the scoped package result. `Scripts/Simple/validate-specs.sh` is a repository-wide legacy validator and its unrelated failures MUST be reported separately.

### 7.3 Expected current outcome

For the identified current repository snapshot/fixture only, and until a functional candidate, effective configuration, current build linkage and fresh qualified workload evidence exist, the checker is expected to return a blocking report, not PASS:

```text
source_document_state: R0=BLOCKED, R1=OPEN, R2=BLOCKED
computed_gate_state:   R0=BLOCKED, R1=BLOCKED, R2=BLOCKED
functional_sha = NO-SHA
runtime_authorized = false
release_authorized = false
```

### 7.4 Named fixture matrix

| Fixture ID | Manifest | Manifest SHA-256 | Expected assessment |
|---|---|---|---|
| `current-ng060-blocked` | `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json` | `2f7866f029c1e25041090b12e360e5f71efbda1fdf649bad48c355b6ddc71358` | source-document R0 `BLOCKED`, R1 `OPEN`, R2 `BLOCKED`; computed R0/R1/R2 `BLOCKED`; `functional_sha=null/NO-SHA`; exit `20`; runtime/release authorization `false` |

The policy artifact identity is `Specs/SPEC-061-ng-readiness-policy-v1.json` with canonical SHA-256 `4ab73c6c00bce32a59526e6334bf38db3bc1586e478ef4f7d647d0114e64aeb1` and raw-file SHA-256 `8fe289c8ea744424569605ce396b66b4f487eb26024bd9b76695c68f94239939`. The report schema is `ecfe485db6e40e978872891cd190af7438b258ccd1f432fe8c3b6051f081f333`; the input-manifest schema is `6e980d1762cdbf75e747e43def493db8a18ea50116757cbdc014fd0bf518ddfe`; the evidence-record schema is `080e77b8c3883801221129410b754aac300e734d72b1c3d0adbce69376b978d0`; the adapter-registry schema is `8fbb7531489a20808133e3d82c7414e6ca6aea7469883e4fe2e9a73aff48212f`; the adapter registry is `12bbaa2994af209519d56b8da47fb3746a0afc56063ba67e359786a9ca7fa981`. The checker identity remains implementation-derived and MUST be supplied by the trusted launcher after implementation; it is not a hidden fixture input.
## 8. Decisions Log

| # | Decision | Date | Reason |
|---|---|---|---|
| D1 | Create a dedicated SPEC before implementation | 2026-09-21 | Existing SPECs define adjacent responsibilities but no complete read-only orchestrator contract |
| D2 | Limit v1 to R0–R2 | 2026-09-21 | These are the current exact-SHA, build-linkage and workload prerequisites |
| D3 | Keep the checker separate from NG-ELC runtime | 2026-09-21 | Evidence inspection must not produce or execute the evidence it evaluates |
| D4 | Fail closed on missing/ambiguous evidence | 2026-09-21 | Prevent plausible data, historical PASS or matching hashes from becoming acceptance |
| D5 | Treat report emission as the only bounded output | 2026-09-21 | Preserve read-only semantics while allowing automation |
| D6 | Do not reuse side-effecting provenance helpers unchanged | 2026-09-21 | Snapshot capture and compiler execution violate the pure-inspection boundary |
| D7 | Apply Astra AMEND corrections before implementation | 2026-09-21 | The normative report contract, evidence formats, gate aggregation, execution boundary and test-write policy must be closed before Proposal/implementation |
| D8 | Require external schema/policy artifacts and named fixture before Proposal | 2026-09-21 | A prose-only contract cannot provide deterministic validation, trust, identity binding or reproducible current-state assessment |

## 9. Pitfalls

1. A documentation commit is not a functional candidate.
2. Binary SHA-256 equality does not prove how a binary was built.
3. Configuration files do not prove effective privilege or future process environment.
4. Current directory emptiness does not prove a recorded pre-trial baseline.
5. JPEG markers do not prove JPEG decodability.
6. A valid schema-v2 bundle proves integrity, not necessarily acceptance.
7. A high-level audit result must not overwrite the strategic R0/R1/R2 assessment.
8. A checker that writes snapshots, executes compilers or invokes NG-ELC is not read-only.
9. Automatic task, Dashboard or master-plan updates would create record drift and are out of scope.
10. SPEC-061 must not be used to bypass the separate SPEC-057 canonical-entrypoint decision.

## 10. Open Questions

1. Confirm that `SPEC-061` remains the next unallocated number at implementation time; do not renumber existing SPECs if another branch allocates it.
2. Decide whether a separate NG-061 task is created after final SPEC review or whether NG-050 coordinates the draft until implementation approval.
3. Resolve interaction with the future `SPEC-057` entrypoint rename without changing that SPEC's scope.

The report schema, exit-code table, supported record kinds, cross-source policy, gate aggregation, subprocess boundary and test-write policy are normative in Revision 6 and are no longer open design questions.

## 11. Normative v1 schemas, policy and execution profiles

### 11.1 Serialization and unknown-field rules

All v1 JSON records and reports MUST be UTF-8 JSON without a byte-order mark. Duplicate object keys, NaN, Infinity, non-integer values in integer fields, invalid UTF-8, unknown object members, invalid enum values and arrays that are not in the specified order MUST be rejected. Object members are compared by name; arrays are compared in their declared deterministic order. IDs and references are case-sensitive.

The report policy digest is SHA-256 over RFC 8785 canonical UTF-8 serialization of the complete policy-definition object in §11.5, with only its report-only top-level `sha256` member removed before canonicalization. The report `policy` object is an identity reference; it is not the policy definition being hashed. A parser MUST reject duplicate keys before canonicalization.

### 11.2 Complete report-v1 field schema

The report is a closed object with exactly these top-level fields:

| Field | Type | Required | Constraints |
|---|---|---:|---|
| `schema_version` | integer | yes | exactly `1` |
| `checker` | object | yes | exactly `{path,sha256,pre_exec_verified}`; absolute regular-file path, 64-hex digest and trusted-launcher boolean |
| `policy` | object | yes | exactly `{id,sha256}`; policy ID `ng060-r0-r2-read-only-v1` and 64-hex digest |
| `phase` | string | yes | exactly `PRE_TRIAL_READINESS` |
| `identities` | object | yes | exact fields in the identity table below |
| `inputs` | array | yes | zero or more unique input records, sorted by `id` |
| `checks` | array | yes | the stable check IDs in §11.4, sorted by `id` |
| `dependencies` | array | yes | unique edges, sorted by `(check_id,depends_on)` |
| `gates` | object | yes | exactly keys `R0`, `R1`, `R2` |
| `summary` | object | yes | exact fields below |
| `runtime_authorized` | boolean | yes | constant `false` |
| `release_authorized` | boolean | yes | constant `false` |

The `identities` object has exactly these fields:

| Field | Type | Nullable | Constraint |
|---|---|---:|---|
| `review_head` | string | yes | null or full 40-lowercase-hex Git commit SHA |
| `remote_ref` | object | yes | null or `{name: full-local-ref,target_sha: full-40-hex-or-null}`; only `target_sha` is the projected Git target identity |
| `functional_sha` | string | yes | null or full 40-lowercase-hex supplied candidate; never inferred |
| `functional_sha_display` | string | no | exactly `functional_sha` when non-null, otherwise exactly `NO-SHA` |
| `build_source_sha` | string | yes | null or full 40-lowercase-hex source SHA attested by build evidence |
| `receipt_publication_sha` | string | yes | null or full 40-lowercase-hex publication location identity |
| `checker_identity` | string | no | 64-lowercase-hex SHA-256 of the checker file |

Each `inputs[]` record has exactly these fields:

| Field | Type | Nullable | Constraint |
|---|---|---:|---|
| `id` | string | no | unique manifest ID, regex `[a-z0-9][a-z0-9_.-]{0,63}` |
| `record_kind` | enum string | no | one of §3.6 kinds |
| `schema_version` | integer | no | exactly `1` |
| `path` | string | no | absolute path selected by the manifest |
| `required` | boolean | no | requiredness from the policy |
| `expected` | object | no | exactly `{size,sha256}`; each may be null when not supplied |
| `observed` | object | no | exactly `{availability,file_type,size,sha256}`; size/hash MUST be null when unavailable |
| `stable_read` | object | no | exactly `{status,before,after,fd_before,fd_after}` |
| `candidate_binding` | object | no | exact fields from §3.6; nulls are explicit |
| `evidence_class_claim` | enum string | no | claim from the selected record |
| `effective_evidence_class` | enum string | no | derived by the checker, never copied blindly |

`observed.availability` is one of `PRESENT`, `UNAVAILABLE`, `UNREADABLE`, `UNSAFE`; `observed.file_type` is one of `REGULAR`, `MISSING`, `SYMLINK`, `SPECIAL`, `DIRECTORY`, `UNKNOWN`. `stable_read.status` is one of `STABLE`, `UNSTABLE`, `UNAVAILABLE`, `NOT_ATTEMPTED`. Each of `before`, `after`, `fd_before` and `fd_after` is either null or an exact identity object `{st_dev,st_ino,st_size,st_mtime_ns,st_ctime_ns}` with integer fields. An unavailable input MUST retain its expected identity separately while observed size, digest and stable-read identities remain null.

Each `checks[]` record has exactly:

```text
id: stable check ID from §11.4
 gate: R0 | R1 | R2
 required: boolean
 state: SATISFIED | UNSATISFIED | UNKNOWN | ERROR | NOT_APPLICABLE
 reason_code: v1 reason-code enum from §3.3
 expected: deterministic JSON value or null
 observed: deterministic JSON value or null
 evidence_refs: sorted unique array of input IDs
 depends_on: sorted unique array of check IDs
 applicability: {applicable: boolean, basis: REQUIRED | POLICY_NOT_APPLICABLE,
                 justification: string|null}
```

If `applicability.applicable` is false, `basis` MUST be `POLICY_NOT_APPLICABLE` and `justification` MUST be non-null; otherwise `basis` MUST be `REQUIRED` and `justification` MUST be null. A required check with unavailable evidence has `observed: null`, reason `MISSING_INPUT` or `DEPENDENCY_BLOCKED`, never an omitted field.

Each `dependencies[]` record has exactly `{check_id,depends_on,dependency_effect}` where `check_id` is one stable check ID, `depends_on` is a sorted unique array of stable check IDs, and `dependency_effect` is `BLOCKING` or `INFORMATIONAL`. The set of `dependencies[]` edges MUST equal the union of every check's `depends_on` list; unresolved IDs and cycles are `CHECKER_ERROR`.

Each `gates.R*` object has exactly `{state,required_check_ids,depends_on,source_document_state,blocking_reasons}`. `state` is `SATISFIED`, `OPEN`, `BLOCKED`, `ERROR` or `NOT_APPLICABLE`; `required_check_ids` is sorted and unique; `depends_on` is a sorted unique array of gate IDs; `source_document_state` is `{source:string,value:PASS|OPEN|BLOCKED|COMPLETE|UNKNOWN|NOT_RECORDED|OTHER,raw:string}` with `raw` required for `OTHER`; and `blocking_reasons` is a sorted array of v1 reason codes.

`summary` has exactly `{assessment,exit_code,exit_class,blocking_gate_ids,source_document_states}`. `assessment` is `SCOPED_SATISFIED`, `BLOCKED`, `ERROR` or `NOT_APPLICABLE`; `exit_code` is `0`, `20`, `30` or `40`; `exit_class` is `SCOPED_SATISFIED`, `BLOCKED`, `CHECKER_ERROR`, `UNSAFE_INCOMPLETE` or `NOT_APPLICABLE`; `blocking_gate_ids` is a sorted unique subset of `R0,R1,R2`; and `source_document_states` has exactly `R0,R1,R2` values equal to the corresponding gate objects' `source_document_state`.

### 11.2.1 Complete report-v1 JSON Schema

The normative machine-readable report schema is `Scripts/ProjectAudit/schemas/ng-readiness-report-v1.schema.json`, SHA-256 `ecfe485db6e40e978872891cd190af7438b258ccd1f432fe8c3b6051f081f333`. It is a closed Draft 2020-12 schema with `additionalProperties: false` at every object, explicit `$defs`, closed enums, array uniqueness and all nested field types. The policy registry supplies the fixed check IDs, dependency edges, deterministic ordering and predicate applicability; the schema alone does not grant evidence trust.

### 11.2.2 Unavailable-input and nullability matrix

Every nullable field remains present. A missing path is `observed.availability=UNAVAILABLE`, `file_type=MISSING`, `size=null`, `sha256=null`, `stable_read.status=UNAVAILABLE`; an unreadable regular file is `UNREADABLE` with `READ_FAILURE`; a symlink/special/unsafe path is `UNSAFE` with `UNSAFE_PATH` or `UNSAFE_ALIAS`; a changed descriptor or directory is `UNSTABLE` with `UNSTABLE_INPUT`; a dependency-deferred check has `observed=null` and `DEPENDENCY_BLOCKED`. Expected identity is never overwritten by observed identity, empty strings are forbidden, and `NO-SHA` is display-only. The report's `remote_ref` is the tagged slot `{status,name,target_sha,reason_code}`; only a PRESENT `target_sha` participates in the separate Git identity field.


The input manifest is a closed object with exactly:

```text
schema_version: integer, exactly 1
manifest_id: string, [a-z0-9][a-z0-9_.-]{0,63}
policy_id: string, exactly ng060-r0-r2-read-only-v1
policy_path: absolute string
policy_sha256: RFC8785 canonical policy digest
policy_bytes_sha256: raw policy-file digest
adapter_registry_id: exactly ng-readiness-adapter-registry-v1
adapter_registry_path: absolute string
adapter_registry_sha256: raw adapter-registry digest
phase: string, exactly PRE_TRIAL_READINESS
repo_path: absolute string, existing directory selected for local Git inspection
repo_record_id: selected directory-observation record ID
plan_source_path: absolute string, source governing plan file
plan_record_path: absolute string, selected governing-plan record file
plan_record_id: selected governing-plan record ID
review_head: full SHA or null
remote_ref: {name: full local ref string, target_sha: full SHA or null}
candidate_sha: full SHA or null
records: array of record selections
```

Each `records[]` entry is a closed object with exactly:

```text
id: unique manifest ID
record_kind: supported §3.6 enum, including governing-plan and directory-observation
adapter_id: closed registry adapter ID
adapter_version: positive integer registry version
source_format: adapter-declared source format
source_version: adapter-declared source version
path: absolute string
path_kind: REGULAR_FILE or DIRECTORY
input_format: UTF8_JSON, MARKDOWN, DIRECTORY_ENUMERATION or ABSENT
availability_expectation: PRESENT or MISSING_ALLOWED
directory_role: required only for directory-observation; otherwise null
required: boolean
expected: present regular file `{size: non-negative integer,sha256: 64-lowercase-hex}`, missing-allowed `{size:null,sha256:null}`, or directory identity object
candidate_binding: exact §3.6 binding object
accepted_evidence_class_claim: §3.6 evidence-class enum
```

`records[]` is sorted by `id`; IDs are unique across the manifest and report. `candidate_sha` null means no supplied candidate and MUST yield `functional_sha: null`/`NO-SHA`; the literal string `NO-SHA` is invalid in the manifest. The manifest MUST enumerate the plan and every record consumed by a check. A selected but missing path remains a manifest entry with null observed values in the report. No defaults, environment-file expansion, globbing, directory scans or filename-based evidence discovery are permitted.

### 11.3.1 Complete input-manifest-v1 JSON Schema

The normative machine-readable input-manifest schema is `Scripts/ProjectAudit/schemas/ng-readiness-input-manifest-v1.schema.json`, SHA-256 `6e980d1762cdbf75e747e43def493db8a18ea50116757cbdc014fd0bf518ddfe`. It rejects unknown members and duplicate keys and defines the closed record-selection envelope, separate plan source/record paths, absolute paths, expected identities, nullable bindings and evidence-class claim enum.

### 11.3.2 Manifest/CLI identity binding

The selected manifest, policy artifact, adapter registry, trusted execution profile and CLI values are one identity tuple. `repo_path` MUST equal `--repo`, `plan_source_path` MUST equal `--plan-source-path`, `plan_record_path` MUST equal the selected record file named by `plan_record_id`, `candidate_sha` MUST equal the explicit `--candidate-sha` or both be absent/null, `phase` MUST equal `--phase`, `policy_id`/canonical `policy_sha256`/raw `policy_bytes_sha256` MUST equal the selected policy artifact, and registry/profile paths and digests MUST equal the launcher pins. The selected governing-plan record payload path MUST equal `plan_source_path`; the record file path MUST equal `plan_record_path`. `repo_record_id` MUST resolve to the direct directory observation whose path equals `repo_path`; no JSON wrapper with the same ID is accepted. Any disagreement is `IDENTITY_MISMATCH`/30; no source has precedence and no merge is allowed. The report records the manifest path and observed bytes digest, so the exact manifest bytes are part of every assessment.

### 11.4 Stable checks and gate assignment

The v1 check set is fixed and closed:

| Check ID | Gate | Required by default | Direct dependency |
|---|---|---:|---|
| `R0.CANDIDATE_EXPLICIT` | R0 | yes | none |
| `R0.CANDIDATE_COMMIT` | R0 | yes | `R0.CANDIDATE_EXPLICIT` |
| `R0.IDENTITY_SEPARATION` | R0 | yes | `R0.CANDIDATE_EXPLICIT` |
| `R0.PLAN_BINDING` | R0 | yes | `R0.CANDIDATE_EXPLICIT` |
| `R0.EFFECTIVE_CONFIG` | R0 | yes | `R0.PLAN_BINDING` |
| `R0.FREEZE` | R0 | yes | `R0.PLAN_BINDING` |
| `R1.BUILD_SOURCE_BINDING` | R1 | yes | `R0.CANDIDATE_COMMIT` |
| `R1.RECEIPT` | R1 | yes | `R1.BUILD_SOURCE_BINDING` |
| `R1.TOOLCHAIN` | R1 | yes | `R1.RECEIPT` |
| `R1.HELPER_IDENTITIES` | R1 | yes | `R1.BUILD_SOURCE_BINDING` |
| `R1.EXECUTABLE_IDENTITIES` | R1 | yes | `R1.RECEIPT` |
| `R1.ROLE_LINKAGE` | R1 | yes | `R1.EXECUTABLE_IDENTITIES` |
| `R1.SHA_RECONCILIATION` | R1 | yes | `R1.BUILD_SOURCE_BINDING` |
| `R2.WORKLOAD_EXPECTED` | R2 | yes | none |
| `R2.WORKLOAD_FRESHNESS` | R2 | yes | `R2.WORKLOAD_EXPECTED` |
| `R2.SOURCE_PRESERVED` | R2 | yes | `R2.WORKLOAD_EXPECTED` |
| `R2.ROOT_ISOLATION` | R2 | yes | `R2.SOURCE_PRESERVED` |
| `R2.REPOSITORY_BASELINE` | R2 | yes | `R2.ROOT_ISOLATION` |
| `R2.JPEG_QUALIFICATION` | R2 | yes | `R2.WORKLOAD_EXPECTED` |
| `R2.PRESERVED_COPIES` | R2 | yes | `R2.SOURCE_PRESERVED` |

Gate dependencies are `R1 -> R0` and `R2 -> R0`. The implementation MUST emit every stable check, including dependency-deferred checks. It MUST NOT invent a new check ID or silently omit an inapplicable check; policy applicability must be explicit.

### 11.4.1 Dependency graph validation

The checker validates two separate DAGs before evaluation: check-level edges and gate-level edges. It rejects self-edges, duplicate edges, unresolved check/gate IDs, check records whose `depends_on` differs from `dependencies[]`, required-check IDs missing from a gate, checks assigned to the wrong gate, cross-layer cycles and any `INFORMATIONAL` edge in v1. A graph failure emits no partial gate result and returns `DEPENDENCY_REFERENCE_ERROR` or `DEPENDENCY_CYCLE` with exit class `CHECKER_ERROR`/30. Every stable check is emitted exactly once; `required` and policy applicability must agree.


The normative policy artifact is `Specs/SPEC-061-ng-readiness-policy-v1.json`, canonical SHA-256 `4ab73c6c00bce32a59526e6334bf38db3bc1586e478ef4f7d647d0114e64aeb1`, raw-file SHA-256 `8fe289c8ea744424569605ce396b66b4f487eb26024bd9b76695c68f94239939`. The Markdown text in this SPEC is explanatory; the JSON artifact is the only policy source accepted by an implementation.

### 11.5.1 Complete v1 reason-code and predicate registry

The policy artifact's `reason_codes` member is closed. Each entry defines `code`, `allowed_states`, `exit_class`, `gate_effect`, `applies_to`, and typed `observed` semantics (`NULL` or `NON_NULL`). The registry also contains `UNSUPPORTED_ADAPTER`, `ADAPTER_UNREVIEWED` and `REQUIRED_MAPPING_MISSING`. Its `check_registry` contains all 20 checks with `not_applicable=false`, selected input roles/cardinality/nullability, corroboration, direct incidents, success criteria and deterministic reason order. A code or predicate not in the artifact registry is invalid. `NOT_APPLICABLE` belongs to predicate applicability and never to evidence class.

The policy artifact's `check_registry`, `gate_dependencies`, workload, decoder, producer-authentication, limits and admissibility fields are normative. A selected record's manifest digest authenticates only its selected bytes; it does not authenticate the named producer. `INDEPENDENTLY_CHECKED` requires an independently verifiable trusted-producer attestation bound to the record bytes and the selected policy. Without that attestation the effective class cannot exceed `RECORDED_CLAIM`, `HISTORICAL_ONLY` or `UNVERIFIED`.

### 11.5.2 Policy artifact and RFC 8785 digest procedure

The file `Specs/SPEC-061-ng-readiness-policy-v1.json` is the exact UTF-8 policy artifact. Its raw-file digest is `8fe289c8ea744424569605ce396b66b4f487eb26024bd9b76695c68f94239939`; its canonical policy digest is `4ab73c6c00bce32a59526e6334bf38db3bc1586e478ef4f7d647d0114e64aeb1`. It contains no BOM, duplicate keys or non-finite values and is parsed with duplicate-key rejection. The canonical digest removes only an explicitly present report-only top-level `sha256` member, applies RFC 8785 JSON Canonicalization Scheme to the remaining object, encodes those exact bytes as UTF-8 and hashes them with SHA-256. The raw and canonical digests are separate identities selected in the input manifest and repeated in the report; a changed artifact produces `HASH_MISMATCH` or `BINDING_MISMATCH` and cannot be silently accepted.

### 11.5.3 Record-kind schemas, trust and admissibility

The canonical evidence-record schema is `Scripts/ProjectAudit/schemas/ng-readiness-evidence-record-v1.schema.json`; the adapter registry is `Specs/SPEC-061-ng-readiness-adapter-registry-v1.json`. The input-manifest schema validates selections, while the evidence-record schema validates the common envelope and all 19 closed payload branches. The adapter registry defines source format/version, path kind, exact field mappings, reference namespaces, digest domain, information loss and failure classification. Each `*_ref` is typed as a record, artifact, commit, policy or role reference; raw paths cannot appear as references. A selected Markdown, provenance-v2 or directory source is admissible only through its named adapter, and its maximum effective evidence class is enforced. Positive current canonical records use `spec061-trusted-record-v1`; the fixture deliberately uses `spec061-fixture-record-v1` with a historical-only cap. A record claiming `INDEPENDENTLY_CHECKED` without exactly one producer attestation verified against the independently provisioned trust anchor is downgraded and produces `PRODUCER_AUTHENTICATION_MISSING` or `PRODUCER_UNTRUSTED`.

### 11.5.4 Cross-source SHA reconciliation

For a non-null candidate, the equality set is explicit: manifest `candidate_sha`, report `identities.functional_sha`, `freeze-r0-r2.payload.functional_sha`, `build-manifest.payload.build_source_sha`, `build-receipt.payload.source_sha`, and every required record `candidate_binding.functional_sha`/`build_source_sha`. All non-null source identities in that set MUST be equal. If `functional_sha` is null, required source checks are `UNKNOWN`/`MISSING_INPUT` and cannot satisfy. If two non-null source identities differ, the result is `UNSATISFIED`/`CROSS_SOURCE_SHA_MISMATCH`, not `UNSUPPORTED_RECONCILIATION`. `review_head`, `remote_ref.target_sha`, `receipt_publication_sha` and artifact SHA-256 values are excluded from this equality set.

### 11.5.5 Named fixture and expected-report matrix

The current fixture is `current-ng060-blocked`, manifest `Specs/RESULTS-SPEC-061/fixtures/current-ng060-blocked.input-manifest.json`, raw SHA-256 `2f7866f029c1e25041090b12e360e5f71efbda1fdf649bad48c355b6ddc71358`, policy canonical SHA-256 `4ab73c6c00bce32a59526e6334bf38db3bc1586e478ef4f7d647d0114e64aeb1` and policy raw-file SHA-256 `8fe289c8ea744424569605ce396b66b4f487eb26024bd9b76695c68f94239939`. Its expected projection is:

```text
functional_sha = null / NO-SHA
source_document_state = {R0: BLOCKED, R1: OPEN, R2: BLOCKED}
computed R0.CANDIDATE_EXPLICIT = UNKNOWN / MISSING_INPUT -> R0 BLOCKED
computed direct R1 evidence incidents = HISTORICAL_ONLY / MISSING_INPUT -> R1 BLOCKED
computed R2.WORKLOAD_EXPECTED = UNKNOWN / MISSING_INPUT -> R2 BLOCKED
summary.assessment = BLOCKED
summary.exit_code = 20
summary.blocking_gate_ids = [R0,R1,R2]
runtime_authorized = false
release_authorized = false
```

The fixture selects canonical JSON wrappers for the historical Markdown/provenance/Git evidence, an explicit governing-plan record and a directory-observation wrapper. Missing current evidence, including `toolchain.current`, uses `spec061-missing-json-v1` with `availability_expectation=MISSING_ALLOWED` and `UNVERIFIED` claim. The implementation-derived checker identity and final report hash are intentionally not pre-existing inputs; acceptance computes them only after the implementation is reviewed. Any other manifest has its own assessment and MUST NOT inherit this projection.

### 11.6 Closed execution, Git and filesystem profile

#### 11.6.1 Closed Git execution profile

The checker MUST NOT spawn an unsandboxed Git process. The only permitted mode is `SANDBOXED_GIT`, supplied by a trusted launcher that verifies `/usr/bin/git` bytes against the policy SHA-256 before execution, places the repository/object database in a read-only mount view, disables network interfaces, applies `no_new_privs`, prevents helper/process escape and terminates the complete child process group on timeout. If that containment cannot be established, the checker returns `GIT_SANDBOX_UNAVAILABLE`/40 without spawning Git.

Before the first Git child, the checker MUST descriptor-safely inspect the complete repository metadata closure. It MUST reject `.git` indirection through a symlink or unsupported file, external `commondir`, `core.worktree`, `config.worktree`, `include`/`includeIf`, submodule configuration, `objects/info/alternates`, HTTP alternates, promisor/partial-clone configuration, alternate object directories, nested repositories, unsafe helper-valued settings and unsupported object formats. Local configuration and implicitly read Git metadata MUST either be represented by a selected `git-repository` input record with observed identities or cause fail-closed rejection.

The first and only preflight tree probe, when needed to detect gitlinks, uses the absolute policy executable and the exact argv template from `Specs/SPEC-061-ng-readiness-policy-v1.json`: `--no-pager --no-optional-locks -C <repo> ls-tree -r -z --full-tree <sha>`. The parser MUST retain mode, object type, object ID and path; any mode `160000` gitlink, malformed NUL record or unsupported path produces `UNSAFE_REPOSITORY`/40 and no follow-on Git command. Subsequent allowed commands are exactly the policy templates and use the absolute executable, `--no-optional-locks`, NUL-delimited status/tree output, `--ignore-submodules=all`, and `GIT_NO_LAZY_FETCH=1`. No `git` basename resolution, fetch, pull, push, helper, hook, filter, external diff/textconv, pager, SSH, lazy object retrieval or inherited environment is permitted.

The sandbox environment is exactly the policy allowlist: `PATH=/usr/bin:/bin`, `GIT_TERMINAL_PROMPT=0`, `GIT_OPTIONAL_LOCKS=0`, `GIT_CONFIG_NOSYSTEM=1`, `GIT_CONFIG_SYSTEM=/dev/null`, `GIT_CONFIG_GLOBAL=/dev/null`, `GIT_NO_LAZY_FETCH=1`, `LC_ALL=C`, `LANG=C`; all other inherited variables are removed. Stdout/stderr are drained with the policy byte limit, stdin is `DEVNULL`, `shell=False`, `close_fds=True`, `pass_fds=()`, finite timeout is mandatory, and a timeout reaps the process group.

#### 11.6.2 Python startup and import boundary

The trusted launcher MUST invoke the absolute policy-reviewed Python interpreter with `-I -B -S`, a sanitized environment containing no `PYTHONPATH`, `PYTHONHOME`, `PYTHONSTARTUP`, `PYTHONINSPECT`, `PYTHONWARNINGS`, `PYTHONUSERBASE`, `PYTHONBREAKPOINT` or plugin variables, and with project/repository roots absent from startup search paths. The first executable operation may import only builtin `sys`, verify isolated/no-bytecode/no-site flags and verify the launcher identity; it MUST fail before project imports or output creation if the flags or identities are wrong.

Only the reviewed standard-library/import set and explicitly reviewed project modules may be imported. Dynamic imports, `sitecustomize`, `.pth`, startup hooks, local `.pyc`/`__pycache__` use, unreviewed entry-point plugins and import-time file/process/network/cache writes are prohibited. Every transitive import is part of the reuse matrix. Any violation produces `IMPORT_SIDE_EFFECT` or `UNSAFE_STARTUP`/40. A checker that hashes its own mutable source path after import MUST NOT claim that hash as execution identity; `checker.pre_exec_verified` is true only when the trusted launcher supplied the pre-execution digest and the checker path was not writable by the checking UID.

#### 11.6.3 Descriptor-relative safe-open and path-race algorithm

For every selected path, the implementation MUST validate an absolute NUL-free path, open existing ancestor components one by one from a trusted directory descriptor with `O_DIRECTORY|O_CLOEXEC|O_NOFOLLOW`, and open a regular-file leaf with `O_RDONLY|O_NONBLOCK|O_CLOEXEC|O_NOFOLLOW`. It MUST hash only bytes read from the held descriptor, compare `fstat` identity before and after reading, reject `st_nlink > 1` unless the policy explicitly permits the exact shared-executable identity, reject special files and symlinks, and compare expected size/hash before accepting the record. `Path.resolve`, `Path.is_file`, `read_bytes`, path-based reopen/hash helpers and lstat-then-open verdict logic are forbidden.

Directory roots MUST be opened and retained by descriptor. Enumeration MUST use the directory descriptor, sort raw entry names deterministically, record each child name/type/device/inode/mode/link count, and repeat enumeration after child inspection. Any entry-set change, identity replacement, mount/alias ambiguity, special entry, symlink, enumeration error or unsupported platform safeguard produces `UNSAFE_INCOMPLETE`/40. These checks prove only observation-time stability, not an atomic multi-file snapshot or future immutability.

The report parent directory MUST already exist and be opened component-by-component without following symlinks. After final serialization, create the report exactly once with `O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC`, mode `0600`, using the validated parent descriptor. `EEXIST`, an unsafe/missing parent, root alias, write/fsync failure, broken pipe or interruption produces `OUTPUT_PATH_CONFLICT`/40; no mkdir, overwrite, replace, temporary sibling or unrelated cleanup is permitted. `--report` and `--report-stdout` are mutually exclusive; stdout is caller-owned and never evidence.

The implementation MUST enforce policy limits for manifest/record/artifact bytes, JSON depth, directory entries, Git output and subprocess/runtime duration. FIFOs, sockets, devices and other potentially blocking objects are rejected before read. Limit violation is `LIMIT_EXCEEDED`/40.

### 11.7 Verification confinement

The implementation acceptance suite MUST be launched by the provisioned trusted execution profile `Scripts/ProjectAudit/schemas/ng-readiness-trusted-execution-profile-v1.json` (SHA-256 `0e43540586bf7258e720a5683a8d95f2ed327c7d3edac8058afd5cbfa708e0cb`). The profile is currently `NOT_PROVISIONED`; therefore no acceptance command is executable and no Proposal status is permitted. Once provisioned, the launcher MUST use the pinned Python interpreter with `-I -B -S`, `PYTHONDONTWRITEBYTECODE=1`, `PYTEST_DISABLE_PLUGIN_AUTOLOAD=1`, empty `PYTEST_ADDOPTS`/`PYTEST_PLUGINS`, an explicit reviewed plugin list, and a newly created external temporary directory whose descriptor identity is verified not to overlap the repository or inspected roots. Existing scratch paths MUST NOT be reused or recursively cleared. Syntax checking uses AST parsing, not bytecode compilation; the focused suite uses only synthetic fixtures and must not invoke `ng_release_audit.py` until its transitive contract is separately reviewed. Tests assert no writes outside the explicit report destination and approved external scratch, no network, no privilege escalation and no prohibited subprocess.

The post-implementation gate is mandatory: before this checker supports any R0–R2 decision, Astra MUST inspect the complete implementation diff, actual executed test commands and counts, actual generated report bytes, checker/launcher/trust-anchor identities and side-effect evidence. A passing test or zero exit code is not sufficient authorization.

## 12. Review Boundary

This is a **Draft** SPEC. It authorizes no implementation, build, runtime, staging, payload generation, push, tag, publication or release. Revision 6 closes the record, adapter, predicate, aggregation, schema and fixture contracts, but Proposal remains blocked until the trusted launcher, sandbox and producer trust anchor are provisioned with immutable identities. The next action is Astra review of this Revision 6 document and all referenced artifacts. After implementation, Astra MUST review the complete implementation diff, actual tests/results, actual report and side-effect evidence before any gate use.
