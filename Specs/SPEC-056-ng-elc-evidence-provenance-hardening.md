# SPEC-056: NG-ELC Local Evidence/Provenance Hardening

**Author:** Antonio Alberti / Hermes Agent  
**Date:** 2026-09-13  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commits:** `a571e7e`, `dbf1241`, `c1f5ad8` (bounded verifier, deterministic workload and canonical R06 provenance/build-linkage increments; acceptance remains open)
**Related:** SPEC-055-ng-elc-local-mode.md; SPEC-054-release-master-plan-and-audit.md; SPEC-046-multi-vm-runner-safe-teardown.md  
**Task linkage:** NG-056 (child of NG-050); NG-055 (local NG-ELC)  
**Canonical tool during SPEC-056:** `NG Experiment Lifecycle Controller (NG-ELC)` — `Scripts/AlpineVMs/ng_remote_executor.py`; canonical rename is separately specified by SPEC-057 and must not alter this hardening contract.

## 1. Problema e autoridade

The supplied local controller can report success without independently reproducible payload evidence, complete provenance, or verified lifecycle cleanup.

Verified gaps in the supplied code include:

- discarded Source/Repository hash maps and no payload preservation;
- provenance limited to HEAD, paths and executable hashes;
- missing enforcement of total timeout and local log quota;
- no observation-time role-crash check;
- non-sticky file divergence and non-accumulated marker coverage;
- unvalidated local trial-path construction;
- leader-only stopping and inventory not governing teardown success;
- IPC deletion based on appearance rather than ownership;
- no controller-owned temporary-removal phase;
- optimistic evidence completeness and fragile sealing-error handling.

The supplied lifecycle test omits its IO directory and can pass after preflight rejection without launching roles.

Omitted helpers, complete repository contents, historical bundle hashes and live host state **cannot be verified** here.

This amendment supersedes conflicting local acceptance claims in SPEC-055. It does not change the remote contract. The historical `local-intra-os-20260913-final` bundle remains a **reported local 5/5 observation, acceptance pending**, not accepted evidence.

## 2. Âmbito e não-goals

### 2.1 Âmbito do controlador/testes

Changes are limited to:

- NG-ELC Python controller and supporting Python test/evidence utilities;
- local plan/scenario validation;
- synthetic workload generation and build-provenance capture;
- tests, schemas, documentation and task-status corrections.

Use the existing NG-ELC entry point; do not introduce a parallel supervisor.

### 2.2 Não-goals

- No C++ or NovaGenesis production-code changes.
- No protocol, wire-format, payload-forwarding or pub/sub changes.
- No SSH, SCP, VM reboot, `sudo`, shell launchers or terminal launchers in local mode.
- No implicit deletion of user-owned build/IO directories.
- No remote behavior changes disguised as local hardening.
- No local result closing G2, G3 overall, G0/G10, or any multi-VM/release gate.

## 3. Requisitos normativos

“MUST” requirements are acceptance gates. Missing, unreadable or ambiguous evidence MUST NOT be treated as success.

### R01 — Validated paths and isolation

Before launching any role, NG-ELC MUST:

1. Validate `trial_id` as one safe path component; reject absolute paths, separators, `.` and `..`.
2. Resolve and verify containment of the exclusively created trial directory beneath the configured evidence root. Reject symlink escapes and existing trial directories.
3. Require all four `NG_LOCAL_*` paths to be explicit, absolute and placeholder-free.
4. Verify evidence storage is durable, outside `/tmp`, and not tmpfs/ramfs. Unknown storage classification fails preflight.
5. Reject Source/Repository aliasing, nesting, symlink escapes and shared artifact inodes.
6. Ensure evidence cannot be removed by any declared cleanup target.
7. Acquire exclusive ownership of mutable trial resources; reject conflicting NG processes or unknown isolation state.

Build and IO may use `/tmp`; evidence may not.

### R02 — Fresh deterministic workload

The canonical plan MUST declare:

- four roles in order: PGCS `-lc`, NRNCS, Repository, Source;
- explicit commands, working directories, readiness requirements and role lifetime expectations;
- a versioned deterministic generator, seed/parameters and five synthetic JPEG names;
- file oracle, SHA-256, finite deadlines, quotas and cleanup ownership.

NG-ELC MUST create fresh trial-owned IO without overwriting existing contents. Before launch it MUST preserve a five-entry expected workload map and prove Repository empty.

A supplied build directory is read-only, user-owned by default. Merely referencing a directory in configuration does not confer deletion ownership.

### R03 — Fail-closed oracle

For file oracles:

1. Preserve the expected workload map, observed Source map and Repository map, including relative name, size and SHA-256.
2. PASS requires exact expected count, exact name sets and byte equality against the initial workload—not merely Source↔Repository equality.
3. Preserve both artifact sets as independently rehashable regular files. Reject unsafe links and unstable reads.
4. After both application roles are ready, extras, changed Source content or an observed same-name hash mismatch MUST latch `FAIL`; subsequent equality cannot erase it.
5. Missing Repository files alone remain pending until the deadline, then yield `INCONCLUSIVE`. Missing readiness yields `INCONCLUSIVE`, not payload `FAIL`.
6. PASS remains provisional until preserved copies reproduce the successful maps. A changed or unreadable snapshot cannot yield accepted PASS.

For marker oracles:

- accumulate unique required `(role, marker_id)` pairs across polls;
- duplicate matches cannot substitute for missing requirements;
- evidence overflow or incomplete coverage cannot yield PASS.

A missing functional oracle or `diagnostic_only` plan MUST never produce runtime PASS.

### R04 — Bounded runtime and liveness

NG-ELC MUST:

- use monotonic deadlines;
- enforce `timeouts.total` as an overall budget, including a declared teardown/evidence reserve;
- cap each readiness and observation deadline by the remaining runtime budget;
- bound subprocess calls, inventory, hashing/copying and cleanup operations;
- enforce declared aggregate log and artifact limits;
- stop admitting runtime work before the teardown reserve is consumed.

Quota exhaustion, total-time exhaustion or unavailable required observations MUST prevent PASS.

All required persistent roles MUST remain alive through the declared observation endpoint. An unexpected exit, including exit code zero, MUST latch runtime `FAIL`. Check earlier roles during later readiness phases and all roles during observation.

### R05 — Signals and failure handling

SIGINT and SIGTERM MUST enter the same bounded abort-and-cleanup path. Partial launch, provenance failures, inventory failures, signal races and cleanup exceptions MUST not bypass finalization.

Once spawned, a process MUST be registered for cleanup before subsequent fallible identity/log operations.

Cleanup MUST continue for other owned resources when one operation fails. Repeated termination signals MUST not interrupt the bounded finalization attempt. Restore prior handlers before returning.

Uncatchable controller termination cannot promise cleanup: an unfinished trial MUST remain visibly unsealed and block conflicting reuse until explicit recovery verifies ownership and absence.

### R06 — Source, controller and build provenance

Before launch, preserve:

- HEAD and exact source-state identity, including tracked modifications, staged changes, untracked build inputs and relevant submodule state;
- content snapshots or a content-addressed source archive sufficient to reconstruct dirty inputs; status/diff alone is insufficient;
- controller and imported project-helper identities and hashes;
- original plan, expanded plan, scenario and observability contract;
- effective trial configuration and execution-affecting environment;
- build recipe/commands, options, toolchain versions and build-input identity;
- per-role resolved executable, executable SHA-256 and relevant runtime-library identity.

Secret values MUST not be published. Required secret-dependent inputs need a documented protected provenance mechanism; silent omission is not complete provenance.

The recorded executable MUST match the launched executable. Replacement or source/build identity drift MUST block PASS.

Binary hashes plus HEAD alone MUST NOT be described as proof of a build from that HEAD. Missing build linkage blocks acceptance eligibility; a diagnostic execution may continue only with an explicit blocker and no acceptance claim.

Dirty but fully captured inputs may support a diagnostic observation. They do not satisfy the clean release-candidate prerequisite.

## 4. Invariantes de lifecycle

The local lifecycle MUST preserve:

`prepare → preflight → launch → readiness → observe → stop → inventory → evidence-preserve → temporary-cleanup → final-verification → seal → result/release`

1. No launch before path validation, ownership acquisition, baseline inventories, fresh workload and required provenance capture.
2. No runtime PASS without all required readiness, oracle and role-liveness conditions.
3. Runtime failures are sticky; evidence or teardown success cannot erase them.
4. No destructive action based only on a path, process name, PID or IPC-set difference.
5. No temporary deletion before the evidence-preservation checkpoint.
6. No teardown PASS without verified absence of trial processes and IPC.
7. No `COMPLETE` without the required inventory and successful offline manifest validation.
8. No changes to inventoried evidence after final sealing.
9. Exit zero requires `PASS/PASS/COMPLETE` and no local acceptance blocker. Interruption returns 130 only when cleanup and evidence satisfy the established interruption contract; otherwise failure classification takes precedence.
10. Logging failure MUST NOT suppress cleanup or cause success.

If durable output itself is unavailable, return nonzero and report the failure through available stderr; do not promise a durable result that could not be written.

## 5. Schema de evidência e selagem

Use a versioned local evidence schema, **version 2**, without silently changing remote readers.

| Artifact | Required content |
|---|---|
| `result.json` | Trial/scenario/mode, component verdicts, exit code, reason codes, local acceptance eligibility and blockers |
| `controller-events.jsonl` | Ordered phase/operation events, monotonic offsets, outcomes and errors |
| `plan/` | Original and expanded plan, scenario, observability contract, effective configuration |
| `provenance.json` and referenced inputs | R06 provenance, capture completeness and build linkage |
| `workload.json` | Generator identity, parameters and initial expected name/size/SHA-256 map |
| `oracle.json` | Both observed maps, comparisons, readiness context, sticky divergence and snapshot verification |
| `artifacts/source/`, `artifacts/repository/` | Preserved compared payloads |
| `roles/<role>/` | Complete captured stdout/stderr and launch/exit identity |
| `inventory/` | Process and IPC baseline, pre-cleanup and final inventories; errors explicitly represented |
| `ownership.json` | Resource identities, attribution method and ownership decisions |
| `cleanup.json` | Ordered actions, retention/deletion decisions and final absence checks |
| `preservation.json` | Verified evidence-preservation checkpoint preceding temporary deletion |
| `manifest.json` | Relative path, byte size and SHA-256 for every required evidence file, including final result |

Process identity records MUST include host boot identity, PID, PGID, starttime and executable identity. IPC records MUST include namespace, kind, identifier, available reuse-disambiguating metadata and attribution evidence.

### Sealing protocol

1. Stop writers; preserve and verify payloads, logs and provenance.
2. Durably write the preservation checkpoint.
3. Perform allowed temporary cleanup and final verification; close the event log.
4. Stage final result and manifest. The manifest excludes itself and the terminal seal record to avoid circular hashing.
5. Verify all listed hashes and required-artifact coverage, flush files and directories, then atomically publish a terminal seal record containing the manifest SHA-256.
6. Return success only after reopening and validating the published seal.

A missing terminal seal, incomplete inventory, hash mismatch or altered file makes the bundle unaccepted regardless of a provisional result. Failure handling MUST be bounded; no uncontrolled manifest retry. Sealing failure changes evidence to `INCOMPLETE`; it does not rewrite a proven teardown verdict to `UNKNOWN` without a teardown-specific reason.

An offline verifier MUST reproduce maps and component eligibility from the preserved bundle without the original build or IO directories.

## 6. Regras de cleanup e ownership

### C01 — Processes

- Inventory relevant NG processes before launch.
- Launch each role in its own process group and retain attributable descendant identities.
- Stop in reverse role order, including surviving group members when the leader has exited.
- Revalidate identities immediately before signalling; never signal a reused PID/group or an unrelated process.
- Verify leader, group-member and attributable descendant absence after TERM/KILL.
- Surviving resources mean teardown `FAIL`; unavailable or ambiguous verification means `UNKNOWN`.
- Preserve SPEC-055’s non-success classification when forced termination is required.

No `killall`, executable-name-only kill or leader-only absence assertion is permitted.

### C02 — IPC

`after − before` is only a discovery set, never deletion authority.

Deletion requires either:

- verified trial-exclusive IPC isolation; or
- positively attributable ownership with object-identity revalidation before removal.

If local permissions cannot establish either, preflight MUST reject the run. A controller lock alone does not prove IPC ownership.

Baseline and unrelated concurrently created IPC MUST remain untouched. Inventory failure MUST not be represented as an empty set. Ambiguous objects are retained and teardown becomes `UNKNOWN`.

### C03 — Temporary directories

Automatic removal requires a recorded controller-created identity beneath an explicitly allowed staging root. Revalidate containment and identity immediately before deletion.

Repository, user-owned build/IO, evidence roots and pre-existing directories MUST never be recursively removed. If preservation fails, retain temporaries and report their locations. Failed removal of required trial temporaries prevents teardown PASS.

## 7. Testes RED→GREEN

Each regression MUST first demonstrate its failure against the supplied behavior where applicable, then pass after implementation. Fault-injection tests MUST assert verdicts **and side effects**.

| Test | Required GREEN observation |
|---|---|
| Correct lifecycle fixture | Creates build/IO and attributable test provenance; asserts four actual launches, readiness, observation, reverse stop and final inventories |
| Separate preflight rejection | Missing IO produces no launch and explicit rejection |
| Trial-path attacks | Absolute/traversal IDs and symlink escapes rejected; no outside writes |
| Workload isolation | Nonempty Repository, aliased paths and shared artifact inodes rejected |
| Reproducible five-photo success | Offline rehash of both preserved sets equals initial map after staging removal |
| Divergence | Mismatch→equality remains FAIL; extras fail; missing-only deadline is INCONCLUSIVE |
| Marker accumulation | Split-poll requirements pass; duplicate-only matches do not |
| Role death | Early-role crash during later readiness and crash during observation cannot PASS |
| Bounds | Multiple readiness waits cannot multiply total budget; log/artifact flooding causes bounded non-success |
| Signals/partial launch | SIGINT/SIGTERM and post-spawn identity failure clean all attributable children |
| Descendants/PID reuse | Exited leader with live child is cleaned; reused identity is not signalled |
| IPC concurrency | Unrelated new IPC survives; attributable IPC is removed; inventory failure yields UNKNOWN |
| Provenance | Dirty inputs are captured; missing recipe/controller identity and executable replacement block eligibility |
| Cleanup ordering | Deletion occurs only after verified preservation; user-owned directories remain |
| Seal failures/tampering | Copy, write, flush, manifest and final-seal faults never yield accepted COMPLETE; mutation fails offline verification |
| Cleanup exceptions | One failed stop/removal does not skip remaining actions; no unbounded retry |
| Remote regression | Existing remote CLI defaults, validation and lifecycle tests remain unchanged and green |

Use isolated fake-process/IPC fixtures for destructive unit tests. Record test commands, candidate identity and complete output—not only aggregate counts.

## 8. Critérios de aceitação e pré-requisitos externos

### 8.1 Conclusão do controlador/testes

All of the following are required:

- R01–R06, lifecycle invariants and C01–C03 have passing targeted tests.
- Local and remote regression suites pass.
- Independent offline verification detects missing/tampered evidence.
- Historical SPEC-055/NG-055 checked claims are corrected to distinguish implementation, observation and acceptance.
- Implementation commit and complete test bundle are recorded.
- No C++/production/protocol changes appear in the reviewed change set.

This establishes **hardening implementation readiness**, not SPEC acceptance or release approval.

### 8.2 Pré-requisitos de release/revisão — não resolvidos apenas por testes

1. Freeze an identifiable **clean candidate** with verified build linkage.
2. Execute **one fresh bounded five-photo local trial** using that candidate and new staging.
3. Submit candidate changes, tests and the complete sealed bundle for Astra’s read-only review.
4. Obtain explicit Astra approval before closing SPEC-056 or SPEC-055’s local acceptance scope.
5. Preserve NG-050 release prerequisites, G2/G3 dependencies and all multi-VM gates. Local PASS cannot close them.
6. Multi-VM tests remain separate: full stop/start of VMs 101/102 before each new test and verification of exactly one PID per NG process.

G0/G10 remain blocked until their own clean-candidate and release requirements are met. No tag or release promotion follows automatically from this amendment.

## 9. Ordem de implementação

1. Correct status claims; define schema, reason codes and offline verifier.
2. Add RED regressions, including the corrected launch-capable lifecycle fixture.
3. Implement path/isolation validation and ownership-ledger foundations.
4. Implement fresh workload, complete provenance and retained oracle evidence.
5. Implement deadlines, quotas, liveness and signal-safe finalization.
6. Implement identity-safe process/IPC cleanup and preservation-before-deletion.
7. Implement transactional sealing and fault-injection coverage.
8. Run local and remote regression suites; freeze the clean candidate.
9. Run the single fresh local trial and submit for Astra review.

## 10. Rollback

- Disable or revert SPEC-056 local changes if safety or remote regression gates fail; retain the established remote behavior.
- Do not restore prior local success claims as accepted evidence.
- Preserve all historical and failed bundles without rewriting their seals; annotate status in separate documentation.
- Never delete uncertain processes, IPC or staging during rollback. Recover only through verified ownership procedures.
- Reopen NG-055/SPEC-055 local acceptance blockers and record the reverted candidate, failure and outstanding resources.
- No rollback may change C++, protocol behavior or multi-VM acceptance status.

## 11. Incremento verificado — offline verifier wiring

Commits `a571e7e`, `dbf1241` and `c1f5ad8` wire the local lifecycle to invoke `verify_bundle()` after sealing, add the R02 deterministic five-file workload and add canonical R06 provenance/build-linkage capture with fail-closed executable drift/missing-manifest checks. Remote schema-v1 behaviour remains preserved. Astra reviews through `Specs/RESULTS-SPEC-056/astra-r14-review-20260914.md` returned `HOLD` for formal publication approval while preserving R12–R14 as bounded work. R12–R14 fix receipt containment and the tested quoted-argv disclosure, and improve publication/ownership controls, but complete durable sealing, receipt-to-build/runtime proof, C01 ownership and full remote compatibility/retained evidence remain open. A clean candidate trial with a production build manifest, complete ownership/cleanup evidence and local acceptance remain open under this SPEC.
