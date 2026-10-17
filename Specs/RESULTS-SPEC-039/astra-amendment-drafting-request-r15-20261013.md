# Astra amendment-drafting context — SPEC-039 r15 HOLD

**Purpose:** Ask Astra to AUTHOR a proposed same-SPEC documentary amendment addressing all six blockers in its r15 HOLD. This is not a request for another readiness verdict and is not permission to edit the repository.

**Frozen current repository identity:** branch `AIOPT5`, HEAD `756f493b7049434ab67cf2382486a48f606a470c`. Dirty diff SHA-256 `146d5875426eb886854c89ddf88337a69660a9ff32b8748a7cfaadd8ef0a7b3a`. Current `git status --short --untracked-files=normal`:

```text
 M Specs/SPEC-039-common-performance-measurement.md
 M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md
?? Specs/RESULTS-SPEC-039/
?? Specs/RESULTS-SPEC-059/
```

Untracked-file count (from `git ls-files --others --exclude-standard`): 382. The existing dirty SPEC-062 work and other untracked evidence remain out of scope and must not be modified. The isolated functional worktree remains at `1af604dad66148618cc0e578c28adb4a9829e607` with its pre-existing D1-H CMake overlay and untracked D1-H harness/manifest; do not treat its overlay as a clean baseline or write to it.

**r15 review target:** packet SHA-256 `ab4855bdc79c81ad637903abc4071a892d64224a17519a60d0992a160226b7c1`. Astra's exact response is `HOLD`, bound to that packet. The r15 packet itself is preserved byte-for-byte as the historical submitted target. Its embedded “not submitted/consent false” fields describe its pre-submission freeze snapshot and are stale as current status; the separately attached r15 submission receipt is authoritative for the later one-call submission and HOLD. Astra r14 `APPROVE` applied only to bounded closure of the r13 journal-policy contradiction, not D2 readiness, implementation, provisioning or runtime.

## Direct task to Astra

Please produce a **concrete, patch-ready proposal** that resolves r15 blockers B1–B6 in the existing SPEC-039 lineage. Do not create a new SPEC number. Return either exact unified diffs or complete replacement sections for the same SPEC-039, the launcher/lease proposal, and every active manifest/packet projection that must change to remain consistent. Include a finding-to-closure table with exact clauses/JSON pointers and identify which assertions are only design proposals versus independently evidenced facts.

Do not answer only with advice or restate the HOLD. Do not claim that a design patch proves implementation, tests, runtime behavior, or readiness. Where the owner’s existing authorization does not settle a design choice, do not silently decide or widen scope: give the safest bounded option, alternatives and consequences, and mark the exact owner decision required. In particular, make explicit the run/process cardinality; the partial-reservation failure path; the lease authority/FD model; and the compatible libc-provider/linkage proof. Any proposed equations, deadlines, argv, cgroup/PID sets, timeout/escalation rules, or cleanup transitions must be explicit and internally consistent—not “counts reconcile” or “manifest-pinned” placeholders.

### Bounded authority and non-goals

This request authorizes **one Astra drafting call only**, to return a proposed documentary patch. It does not authorize Astra to submit another review or claim approval. It does not authorize Hermes or Astra to edit files, implement code, configure/build, run a harness/finalizer, provision UID/service/launcher/lease, create/remove IPC, perform cleanup/retry, access VMs/SSH, execute D2/D3, claim acceptance, commit, or push. All such actions remain separate gates requiring separate owner authorization. The existing D2 scope remains correctness-only; D3 timing/performance remains excluded. The frozen packet is a target/context snapshot, not authority.

## Required blocker-by-blocker response

1. **B1 — observer order:** reconcile construction-time initialization, registration/reset, monotonic queue-wide counters, bootstrap drain, Q0/Q1 boundaries, sticky wait/decision capture, event sequence/iteration/head identity and the 100 ms baseline. Supply exact drain equations and finite deadlines. Keep Q0 claim to fresh observed-empty events unless a stronger interval oracle is fully specified.
2. **B2 — run/process count and hello:** reconcile one authorized correctness experiment/run versus two process starts/run IDs/records; define exact Process name/key, hello-trigger/bootstrap behavior, target per condition, run/journal boundaries and Q1 `NOT_RUN` rule. Preserve the narrow owner authorization; surface a decision if a two-process pair exceeds it.
3. **B3 — semaphore fail-closed:** for all three pinned `sem_open` failure paths, specify sticky non-resettable error state, operation/errno, synchronization, propagation through constructor/Gateway/workers, abort/checkpoint/final classification, zero `sem_unlink`, and fault-injection evidence. No Q0/Q1 window or PASS after failure; macro-off behavior remains unchanged.
4. **B4 — provider/CMake:** resolve the conflict between `-static` and `dladdr` proof of the `sem_open` provider/libc file identity. Give a concrete compatible approach and exact line-anchored CMake/interface/target-graph/macro-propagation/census contract; do not assert unprovided source is already present.
5. **B5 — supervisor/lease:** define lease owner and who can unlock it, distinguish cgroup census from harness PIDs requiring wait receipts and the live supervisor, pin exact timeout/escalation/log-cap/finalizer-failure behavior, and explain containment and lease-release/no-release states. Do not call an inherited lock-capable FD “inspection-only.”
6. **B6 — partial reservation:** reconcile pre-construction reservation failure with shutdown-complete/destruction-gated cleanup. Prefer a conservative path that does not fabricate shutdown/destruction evidence or authorize harness-side cleanup; if proposing a separate cleanup phase, define its exact authority and journal/receipt contract and identify owner decision needed.

## Evidence limitations and source identities

The primary source/design baseline is commit `1af604dad66148618cc0e578c28adb4a9829e607`, tree `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`. Full baseline source files are attached below. The D1-H CMake overlay and test-only harness/manifest are separate attachments and are dirty/untracked in that isolated worktree; clearly separate them from the baseline. Main checkout HEAD is `756f493b7049434ab67cf2382486a48f606a470c`, not the functional baseline. The host preflight was bounded/name-filtered and found no named dedicated identity/unit/launcher in searched paths; this is not an exhaustive host audit. D1-H proves compile/link only; its executable was not run. The D2 harness, D2 manifest, UID, unit, supervisor and lease are absent/unprovisioned. No runtime, IPC, D2 correctness, or D3 measurement has occurred.

The prior r15 reviewer used no tools and independently recomputed no hashes. Treat all supplied hashes and historical outputs as supplied evidence; inspect only attached bytes. The r15 packet is the exact target; later helper files are supplemental context, not silent scope expansion. If any file is missing or too large to assess, name it and state the resulting limit rather than inferring its contents.

## Attached artifacts

The sections below contain complete byte-for-byte UTF-8/text copies of the pinned files. Each section states its path, byte count, SHA-256 and role. Source code is context only; do not edit or execute it.

## FILE: Specs/RESULTS-SPEC-039/precode-packet-v1.16-d2a-precode-20261013-sanitized-draft-r15.md

Role: Exact previously submitted review target; preserved snapshot, not current consent/authority.
Bytes: 194163
SHA-256: ab4855bdc79c81ad637903abc4071a892d64224a17519a60d0992a160226b7c1

````text
# SPEC-039 D2a pre-code design review packet — local draft r15

**Status:** not submitted; exact-SHA owner consent not requested or received.  
**Requested scope:** documentary pre-code design readiness of D2 Q0/Q1 and launcher/lease only.  
**Not authorized:** implementation, configure/build, harness/finalizer execution, IPC, host provisioning, VM/SSH, commit/push, D2/D3 acceptance, or any Astra call.  
**Main repository:** AIOPT5 HEAD `756f493b7049434ab67cf2382486a48f606a470c`; working tree dirty.  
**Functional baseline:** `1af604dad66148618cc0e578c28adb4a9829e607` tree `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`; isolated worktree remains dirty with preserved D1-H CMake overlay and untracked D1-H harness/manifest.  
**Live SPEC:** SHA-256 `f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025`.  
**Companion manifest:** `precode-manifest-v1.16-d2a-r15.json` SHA-256 `4ed5646dcaab8af74344f3acba22c962fef8703b2d4d0fddd463c0381b9e3827`.  
**Launcher/lease proposal:** `d2-launcher-lease-proposal-v1-20261013.md` SHA-256 `ab386f6a94228ab9312efa4526e68b67ab43fc0f7936b789a9ebc459b59f67e6`.  
**Packet SHA-256:** recorded only in the separate local freeze receipt after final assembly.

This draft is a candidate for owner inspection. Astra r14 `APPROVE` is limited to the r13 journal-policy contradiction and does not approve the D2 profile, launcher, source delta, runtime, or implementation.

## 1. Decision requested from reviewer

Return `APPROVE`, `AMEND`, or `HOLD` for **D2a pre-code design readiness only**. Map blockers to exact SPEC clauses/source paths. Decide whether the historic D2 pre-code findings are closed; whether the proposed Q0 bootstrap-drain and empty-observation event prove queue state without changing queue semantics; whether `N=1` and two sequential fresh Q0/Q1 process starts under one paired authorization are consistent with §§2–4 and §7.1; whether the root-supervisor/dedicated-UID/cgroup/receipt/lease design satisfies §7.1; whether the five-file CMake/source graph is sufficiently bounded; and whether all three diagnostic `sem_unlink` failures are fail-closed. Do not approve or imply implementation, provisioning, build, D2 run, finalizer behavior, IPC cleanup, D3, commit, or push.

## 2. Review history and authority limits

### Historical D2 pre-code HOLD — v1.2

Historical packet SHA-256 `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`; its one-call consent is consumed. The full historical response is extracted from the local tracker transcript, SHA-256 `a9d900654287be6f3d9afd5c6e90f1b8a53fe03a03202f8311330345675dcd05` (private repository paths are redacted in the rendered copy if present).

<details>
<summary>Complete historical Astra HOLD response</summary>

## Verdict: HOLD — suitable queue-only approach, but the frozen design has unresolved blockers

The direct `Process::NewMessage` → `GW::PushToInputQueue` stimulus is sufficient **in principle for this bounded D2 queue-admission/scheduling test**. It is not a transport, NGAL, wire-time, payload-delivery, or performance test.

The proposed five-file allowlist and default-OFF seam are consistent with that narrow purpose. However, the packet does not yet establish a sufficiently precise, lifetime-safe and non-stale oracle to recommend implementation unchanged. This is an advisory gate finding, not authorization or D2 sign-off.

### Review binding

This review is limited to:

- Packet SHA-256: `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`
- Functional commit: `1af604dad66148618cc0e578c28adb4a9829e607`
- Functional tree: `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`
- SPEC-039 v1.2, stated unredacted source-file SHA-256: `8973948334538715a9e02474be7b1d88a24e6b9235335839870ba126d0570d91`

These are the supplied identities, not independently verified hashes. Source conclusions below rest on the supplied excerpts; omitted implementations and local runtime behavior remain unverified. D1-H provides only the packet-reported compile/link evidence.

## Must-fix findings

### 1. Make sandbox establishment precede **all** Process/Common resource activity

The final proposed harness sequence says to create Process and the target, then record an empty pre-run inventory. That conflicts with the sandbox description and cannot serve as the resource baseline.

Construction already executes GW initialization (`GW.cpp:113–123`), which calls `ReturnIPCSHMID(11, …)` for a non-`PGCS` process (`GWRunInitialization01.cpp:156–173`). That function attempts `sem_open` and `shmget`.

The required ordering must be unambiguous:

1. Enter the prescribed namespaces.
2. Successfully establish private mount propagation and fresh `/dev/shm`.
3. Capture and require empty inventories.
4. Only then construct Process/GW and the target.

Any earlier initialization capable of IPC or semaphore activity must also be excluded. Otherwise construction can contaminate the baseline or touch a host-backed `/dev/shm` before its replacement.

### 2. Resolve target lifetime **before admission**, not just after stop

Preallocating before the Gateway thread correctly avoids concurrent `Process::NewMessage`. It does not by itself prove survival through bootstrap and Q0.

Gateway calls `DeleteMarkedMessages()` after dispatch (`GW.cpp:591–593`), and that routine deletes Process-owned messages whose delete flag is true. The target is allocated but not yet queued during bootstrap; `PushToInputQueue` only unmarks it later, at admission.

The packet does not include the `Message` constructor’s delete-flag initialization. Freeze an explicit pre-thread non-deletable state using the existing public lifetime API, and verify the applicable baseline semantics. Do not rely on an unspecified constructor default.

Also replace “ownership-safe cleanup branch” with a concrete ownership decision:

- Successful pending case: stop/join, erase the exact target, require the exact count decrement, preserve its lifetime through queue destruction, then delete it.
- Unexpected dispatch/deletion case: do not dereference or delete a potentially freed target. The decision must use saved scalar identity and established ownership evidence after join.
- Failed reconciliation: no unconditional target deletion; report failure.

The successful-path ordering is sensible. The failure-path ownership oracle is presently only asserted.

### 3. Define snapshot freshness, event correlation, and persistent violation evidence

A mutex-protected snapshot prevents torn reads, but **does not automatically prove an interval-wide claim**.

The proposed fields need explicit validity and event-sequence semantics so that:

- Constructor/bootstrap admission is not mistaken for Q1 admission.
- A stale empty-queue observation is not reused as Q0 evidence.
- A wait result is associated with the queue/head/stop state at that wait site, rather than a later pop-site state.
- Q1’s non-due decision demonstrably occurred after target admission.
- An unexpected target pop or dispatch cannot disappear when later events overwrite “last” fields.

Freeze cumulative target pop/Run counts or sticky violation evidence, plus suitable scalar event generations/validity markers. Define how the target identity is selected without hard-coded instantiation assumptions or unsynchronized observer mutation.

For Q0, reconcile the packet’s two formulations:

- SPEC: a 100 ms no-offer window that **includes an observed empty queue**.
- Final harness shape: an empty queue **for 100 ms**.

The latter requires stronger evidence than sampled emptiness. Keep the narrower SPEC assertion unless the frozen observer explicitly supports the stronger one.

The captured predicate-overload `wait_for` boolean proves its return value—not blocking duration, immediacy, or a unique wake reason.

### 4. Complete the supposedly exact pre-code contract

The packet supplies a prose implementation shape, not the proposed observer/interface diff identified in §4.5. It also leaves important details unfrozen:

- Exact inert command tuples and checked construction results.
- Observer field initialization before constructor-time admission/Run activity.
- Exact event increment sites, target-registration mechanism and lock ownership.
- Build configuration, actual launch paths, resource limits and evidence destinations.
- Failure/timeout outcome handling.

The inert tuples must avoid both exact action matching and the `"0.1"` fallback shown in `Block.cpp:246–250, 291–309`; “unregistered” must cover both.

Any macro-dependent GW layout must be consistent across all translation units in the D2 executable. Separate ON/OFF build artifacts must not be mixed. Absence of observer symbols alone is insufficient to demonstrate absence of observer state or inline instrumentation in the default build; the gated source and effective compile definitions also need inspection.

These are closure requirements, not permission to add files, alter production algorithms, or submit another review.

## What the excerpts support

- **Real admission path:** `NoCL > 2` leads to queue insertion under `InputQueueMutex`.
- **Real scheduling decision:** the shown loop pops one head only when `ScheduledTime < Time`. Capturing that exact boolean once and branching on it is the right seam design.
- **Future-head hypothesis:** the wait predicate is true for any nonempty queue, including a future head. This supports a source-level immediate-return/busy-loop hypothesis, not a measured bottleneck.
- **Bootstrap is real:** initialization queues bindings and a hello trigger. Key `11` suppresses the hello action when executed; it does not prevent that trigger from initially being queued.
- **Exposition suppression:** the shown initialization schedules exposition only for process name `PGCS`.
- **IPC remains real:** key `11` does not eliminate semaphore or shared-memory attempts.
- **Cleanup count check is necessary:** `EraseMessage` returns `OK` even without finding the pointer.
- **Transport exclusion is correct:** the supplied receive path allocates with time zero, while the shown serializer emits command lines and optional payload rather than the local scheduling field.

## Q0/Q1 proved-versus-unproved matrix

“Supported” below means supported by excerpts, **not runtime-proved**.

| Claim | Supported now | Unproved / required D2 evidence |
|---|---|---|
| Q0 has zero controlled application offers | Harness intent only | Recorded phase boundaries and zero controlled offers |
| Q0 starts after bootstrap drain | Bootstrap work is source-supported | Drain and fresh empty observation within 5 s |
| Q0 is continuously empty for 100 ms | No | Requires stronger interval oracle; otherwise report only observed emptiness |
| Q0 establishes an idle CPU/polling floor | No | Outside D2; no performance conclusion |
| Q1 exercises production input admission | Public path and admission condition supported | Exactly one target offer and matching admission identity/count |
| Q1 target is depth-one head | No runtime evidence | Fresh head/depth/time observation after admission |
| Q1 observes the actual non-due branch | Correct observation site identified | Valid post-admission event using the exact production boolean |
| Q1 remains pending, unpopped and undispatched for 100 ms | Intended source behavior supported | Persistent target evidence across the window and final reconciliation |
| Q1 captures the production wait result | Feasible at existing call | Correlated, fresh wait observation |
| Q1 proves immediate waits or busy-loop cost | No | Boolean/timeout value is not elapsed-time evidence; no D2 timing claim |
| Target survives bootstrap and cleanup safely | Successful teardown strategy partly supported | Initial delete state, failure ownership, exact erase decrement, destruction behavior |
| Both workers stop and join | Stop/notify and join sites shown | Successful local join; timeout is failure, not PASS |
| IPC and semaphore effects are isolated and removed | Appropriate namespace strategy proposed | Actual namespace/mount success, inventories, attribution and final empty state |
| Default Common is unaffected by the seam | Gating intent only | Default-build definitions, gated-source inspection and artifact checks |
| NGAL delivery, wire scheduling or inverted pub/sub behavior is validated | No | Explicitly outside this D2 test |

## Optional hardening

- Record namespace identities and mount information alongside inventories to make containment evidence auditable.
- Keep logs bounded and preserve partial phase evidence outside the disposable `/dev/shm`; a killed run cannot supply a successful final inventory.
- Record remaining future-time margin at Q1 start and finish. The nominal 10 s horizon is not guaranteed under arbitrary scheduling delays; fail closed rather than retiming the target.
- Prefer a quiet correctness build: the excerpted DEBUG path reads queue state outside its mutex and is unnecessary to this oracle.
- Distinguish namespace destruction after timeout from demonstrated in-harness cleanup. Neither timeout nor forced destruction establishes a D2 cleanup PASS.

**Bottom line:** the queue-only stimulus and basic seam placement are appropriate. Resolve sandbox ordering, pre-admission lifetime, snapshot proof semantics and the exact frozen contract before treating the design as implementation-ready. All runtime assertions remain unproved, and D3 remains a separate gate.

</details>

### r13 AMEND

Packet SHA-256 `0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727`; response SHA-256 `94c118fe12b2b27143a521b43be5ea194d4d0969b4ea35c8829d868d91af47d0. This was the abnormal-mode journal-policy contradiction, not a D2 source approval.

<details>
<summary>r13 AMEND response</summary>

## Verdict: AMEND — r12 B1/B2 documentary closure only

I assessed only the supplied packet, identified by the required SHA-256 `0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727`. This is a supplied identity, not a hash I independently recomputed.

**The canonical model addresses the original r12 defects, but an attached active policy contradiction prevents documentary closure.**

### Findings on the original defects

- **r12 B1 is addressed in §7.2’s transition contract.** Creation and cleanup have distinct pending states. `RUN_SHUTDOWN_COMPLETE` rejects unresolved creation intents; `REMOVED` and `CLEANUP_FAILED` require `cleanup_pending`. Consequently, the original five-event counterexample cannot validly become `not_started`. Orphan outcomes and wrong-phase creation are rejected.
- **Resource identity continuity is explicit.** `CREATED`, `CLEANUP_INTENT`, and terminal cleanup outcomes must share `(candidate_key, shmid)` or `(source_name, device, inode)`. The supplied helper excerpt checks the binding’s fields and correspondence to the event key/name. This establishes the documentary requirement, not independently verified full-interpreter behavior.
- **r12 B2’s original exclusion of `complete` is corrected.** The attached §7.1 normal-finalization paragraph, numbered item 5, §7.2 `FIN-001`, and manifest failure-rule projections permit historical `complete` after later evidence/finalization failure while overall status remains `INCONCLUSIVE`.
- The attached rules do not make historical cleanup completion sufficient for PASS, permit destructive retry, or override lost authority.

## Must-fix blocker

### 1. Abnormal-mode policy both requires and prohibits the journal replay needed to derive cleanup history

**Exact SPEC locations**

- **§7.1, paragraph beginning “In `abnormal_evidence_only` mode…”**: requires the bounded classifier to validate event shape, sequence, phase/resource transitions, and stable identity binding.
- **§7.2, `COUT-001`–`COUT-004`**: requires a complete, transition-valid closed journal for determinate outcomes; otherwise `unknown`.

**Manifest JSON pointers**  
These refer to the underlying manifest paths represented by the packet’s active-path projections:

- `/sandbox/finalizer_ipc_boundary/abnormal_evidence_only/forbidden/3`
- `/semantic_finalizer_contract/finalizer_ipc_boundary/abnormal_evidence_only/forbidden/3`
- `/sandbox/inventory_requirements_by_mode/abnormal_evidence_only`
- `/sandbox/runtime_finalizer_cleanup`
- `/semantic_finalizer_contract/abnormal_evidence_only/non_control_validation`

Those clauses forbid or exclude parsing/replay of non-control evidence without expressly excepting bounded journal-history classification. Yet the same attached policies require that classification, notably:

- `/sandbox/finalizer_ipc_boundary/abnormal_evidence_only/cleanup_outcome_classifier`
- `/semantic_finalizer_contract/abnormal_evidence_only/cleanup_outcome`
- `/semantic_finalizer_contract/failure_summary_checks/4`

**Why this blocks closure:** The reservation journal is evidence beyond the pending record and supervisor receipt—the two artifacts designated for strict control validation. Establishing terminal coverage, orphan rejection, and physical-identity continuity requires parsing and replaying its history. A blanket prohibition on that operation conflicts with the mandatory evidence-derived outcome contract. Merely preserving/hash-binding raw bytes cannot establish `complete`, `partial`, or `not_started`.

**Required documentary amendment:** Explicitly distinguish:

1. strict control-record validation;
2. bounded, read-only journal parsing/replay solely for `COUT-001`–`COUT-004`; and
3. normal complete-bundle/schema/inventory validation, which must not gate abnormal preservation.

State consistently that malformed, unavailable, truncated, or invalid journal history yields `unknown` without blocking an otherwise authorized abnormal `INCONCLUSIVE` record. Preserve the prohibitions on journal mutation, live IPC inspection, cleanup, retry, and authority bypass.

## Optional advice

- Label the manifest’s 58-method results and the historical receipt’s 61-method results explicitly as **58 model/projection tests plus 3 CLI tests**. The supplied commands explain the difference; this is not a blocker.
- Keep synthetic transition coverage separate from `FIN-001` behavioral coverage. The packet correctly labels destructive-stop/retry behavior as **not executed**.

## Context sufficiency and evidence limitations

The supplied context is sufficient to identify the blocker and assess the canonical B1/B2 corrections. It is **not sufficient to certify whole-file consistency**: omitted SPEC text and digest-only manifest sections were not inspected. In particular, `/evidence_recovery`, `/semantic_finalizer_contract/artifact_checks`, and the remaining run-record schema conditionals would matter to a broader claim about recovery, bundle gating, and schema compatibility. Their digests do not establish their contents or consistency.

The historical validation receipt expressly binds an earlier packet, not this expanded packet. Its synthetic results are reported evidence, not independently reproduced results. The final expanded-packet checker sidecar is not supplied. Cross-artifact PID validation and production-finalizer/runtime behavior remain unverified.

I used no tools, executed no tests, inspected no filesystem, and recomputed no hashes. **No implementation, build, runtime, IPC/VM, D2/D3, commit, push, or follow-up-call authority is granted or triggered.**


</details>

### r14 bounded APPROVE

Packet SHA-256 `c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95`; response SHA-256 `2d0183b961cd5d7a7ee48c555f0bf04345ed9bcb07761697d84ec0fdc57d9406. Astra explicitly limited the approval to the supplied r13 journal-policy contradiction, not whole-file or runtime correctness.

<details>
<summary>r14 APPROVE response</summary>

## Verdict: APPROVE

**v1.16/r14 resolves the bounded r13 journal-policy contradiction and preserves the accepted r12 B1/B2 corrections at the documentary level.** This conclusion covers the supplied excerpts and active projections, not whole-file or runtime correctness.

### Findings

1. **The journal exception is explicit and narrowly scoped.**  
   SPEC §7.1, numbered item 5, and §7.2 `AUTH-005` permit bounded, read-only parsing/replay of `reservation-journal.jsonl` solely to derive cleanup history under `COUT-001`–`COUT-004`. The previously conflicting manifest projections now expressly include that exception:
   - `/sandbox/finalizer_ipc_boundary/abnormal_evidence_only/forbidden/3`
   - `/semantic_finalizer_contract/finalizer_ipc_boundary/abnormal_evidence_only/forbidden/3`
   - `/sandbox/inventory_requirements_by_mode/abnormal_evidence_only`
   - `/sandbox/runtime_finalizer_cleanup`
   - `/semantic_finalizer_contract/abnormal_evidence_only/non_control_validation`

   The attached classification, mode-selection, artifact-check and failure-summary projections also reflect this distinction.

2. **Unusable journal history does not become a final-record gate.**  
   Invalid, unavailable, malformed, truncated or incomplete history yields `unknown` without blocking an otherwise authorised abnormal `INCONCLUSIVE` record. This is explicit in §7.1 item 5 and §7.2’s journal-history policy, including:
   - `/classification_rules/abnormal_journal_history_policy`
   - `/semantic_finalizer_contract/abnormal_evidence_only/journal_history_policy`

3. **Other non-control evidence remains raw-only.**  
   The attached abnormal-mode policies preserve/hash safely readable, bounded raw bytes without schema replay or normal bundle/inventory gates. Missing and unavailable evidence remain distinct; preserved post/final files do not establish verified or empty inventories.

4. **Safety and authority restrictions remain intact.**  
   Journal mutation, live IPC inspection, cleanup, retry and abnormal PASS/FAIL remain forbidden. Common authority failures block finalisation rather than authorising abnormal-mode entry. Historical cleanup completion never implies PASS.

5. **The r12 corrections are preserved.**
   - **B1:** §7.2 `JRN-002`–`JRN-009` retain distinct creation/cleanup pending states, orphan-outcome rejection, phase constraints and stable resource-identity continuity.
   - **B2:** §7.1’s normal-finalisation paragraph and item 5, §7.2 `FIN-001`, and the attached failure-rule projections retain historical `complete` after later evidence/finalisation failure while overall status remains `INCONCLUSIVE`.

### Must-fix blockers

None identified within this bounded amendment review.

### Optional advice

For editorial clarity, qualify the generic inventory-parsing language at `/semantic_finalizer_contract/failure_summary_checks/3` explicitly by mode. The supplied abnormal-mode overrides already prohibit inventory replay and require unavailable post/final summaries, so this is not a blocker.

### Context sufficiency and omissions

The supplied context is sufficient for this single contradiction-closure assessment. Omitted SPEC text and digest-only manifest sections—including `/evidence_recovery` and `/runtime_bundle`—were not inspected. Their hashes do not establish consistency.

The reported synthetic tests were not independently reproduced. The post-freeze checker/schema-validation sidecar is not supplied; full-interpreter, cross-artifact PID and production-finaliser behaviour remain unverified.

The packet identity is supplied as SHA-256 `c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95`; I did not recompute it or other hashes, use tools, or inspect a filesystem.

**This documentary review grants no implementation or operational authority. It does not authorise implementation, build, runtime, IPC/VM, D2/D3, commit, push or any follow-up submission/call, and does not establish or replace owner consent.**


</details>

## 3. Disposition of the four historical D2 HOLD findings

| Finding | Current documentary response | Remaining evidence/gate |
|---|---|---|
| Establish host IPC/resource attribution before constructing Process/GW | §7.1 requires dedicated UID/launcher/continuous lease, complete pre-inventory and preconstruction reservation. | UID/launcher/lease and service-side visibility are absent; preflight was bounded and read-only. |
| Explicit target Message lifetime and failure-safe ownership | §7.1 item 2 pins construction order, scalar identity, non-dispatch, stop/join, erase/count, and lifetime through destruction. | No D2 harness or lifecycle test exists; only baseline source was inspected. |
| Fresh and correlated observation | §4.4/§7.1 item 3 define sequence, iteration ID, sticky wait/decision captures, real due/pop observation, saturation and Q0/Q1 windows. | The exact bootstrap-drain point and observer reset/baseline interaction are still proposed clarifications; no runtime proof exists. |
| Exact interface, target graph, commands, evidence | §4.4–4.5 and §7.1 define the allowlist, diagnostic graph, checker shapes, schemas and finalization. This packet adds a file-level design map and candidate launcher proposal. | No byte-applicable diff, D2 target, D2 manifest, installed launcher/service, or service receipts exist; command/property details still require review. |

The v1.16 text addresses these findings at documentary level. This does not establish D2 readiness or authorize execution.

## 4. Proposed observer/interface and test delta — design only, not applied

The following file/call-site proposal is not a compilable patch and was not applied or tested. It is supplied to make the earlier interface/diff question reviewable before any code change.

### Proposed `GW.h` additions (design-only; not applied)

```cpp
#ifdef NG_GW_QUEUE_TEST_OBSERVATION
#include <cstdint>

enum class D2CorrelationStatus : std::uint8_t {
  None = 0, Pending, Matched, DecisionNotReached, HeadMismatch, Invalid, Overflow
};
struct D2WaitCapture {
  bool valid = false;
  std::uint64_t sequence = 0, iteration_id = 0;
  std::uint32_t target_id = 0, head_id = 0;
  double head_time = 0.0;
  std::uint64_t queue_depth = 0;
  std::int64_t timeout_ms = 0;
  bool predicate_return = false;
};
struct D2DecisionCapture {
  bool valid = false;
  std::uint64_t sequence = 0, iteration_id = 0;
  std::uint32_t candidate_id = 0;
  double candidate_time = 0.0;
  bool actual_due_branch = false, actual_popped = false;
};
struct GWQueueTestObservation {
  bool valid = false, gateway_started = false, gateway_stopped = false;
  bool overflow = false, bootstrap_drained = false;
  std::uint64_t event_sequence = 0, gateway_iteration_id = 0;
  std::uint32_t target_id = 0;
  std::uint64_t queue_depth = 0;
  std::uint64_t all_admissions = 0, all_pops = 0;
  std::uint64_t all_run_entries = 0, all_run_returns = 0;
  std::uint64_t empty_observation_count = 0;
  std::uint64_t target_admissions = 0, target_non_due = 0;
  std::uint64_t target_pops = 0, target_run_entries = 0, target_run_returns = 0;
  D2WaitCapture first_target_wait{};
  D2DecisionCapture first_target_decision{};
  D2CorrelationStatus correlation = D2CorrelationStatus::None;
};
int RegisterD2Target(std::uint32_t target_id);
GWQueueTestObservation SnapshotD2Observation();
#endif
```

**Projection issue for review:** `RegisterD2Target` is specified to reset/bind the observation. The proposed implementation resets target-only sticky state but preserves/checkpoints queue-wide bootstrap counters; a full reset after `Process` construction would erase evidence of the already-queued initialization message. Reviewer should decide whether to amend/clarify this same-SPEC contract before implementation.

### Proposed `GW.cpp` instrumentation points (no code applied)

| Existing source site | Proposed test-only event |
|---|---|
| `PushToInputQueue` successful push, `GW.cpp:251–262` | Increment all-admission counter/event sequence; increment target admission for matching scalar ID; capture resulting queue depth. |
| Real `wait_for` return while `InputQueueMutex` is reacquired, `GW.cpp:472–480` | Capture returned predicate value and sticky first target wait; count an empty observation only when not stopping and queue is actually empty at that locked point. |
| Existing strict due/pop critical section, `GW.cpp:487–503` | Capture candidate ID/time, actual comparison result, and actual pop; never recompute due-ness in the observer. |
| Existing `Run` and deletion boundary, `GW.cpp:591–593` | Copy `InstantiationNumber` before `Run`; record entry and return with copied scalar; publish return before `DeleteMarkedMessages`. |
| Completed loop boundary after deletion and IPC poll, `GW.cpp:593–655` | Set a sticky `bootstrap_drained` event only after queue-wide counts reconcile; for Q1, only the registered target remains as sole head. |
| Gateway start/stop, `GW.cpp:434–451,658–667` | One-way started state before loop; stopped only after output worker joins. |
| All failed `sem_open` paths, `GW.cpp:970,1263,1322` | Diagnostic macro only: bounded failure event and zero `sem_unlink`; default macro-off behavior unchanged. |

All values are scalars; no observer `Message*`. Counters saturate and set overflow; overflow blocks PASS. Updates at already-locked sites do not relock. The wait and decision records correlate only when iteration IDs match, decision sequence is later, and target/head/candidate IDs match. No I/O, callbacks, extra due comparison, or default-build symbol/state/call.

### Proposed Q0/Q1 bootstrap and oracle protocol

- Q0: Process constructor completes its synchronous initialization; the initialization action may queue `StoringInitialBinds`. Queue depth zero alone is not bootstrap completion. Require a sticky completed-loop drain event after all queued admissions are popped and all Run entries return; the constructor's synchronous Run is already complete before the harness receives Process/GW, though its marked object may remain Process-owned. Start the 100 ms no-controlled-offer window from a snapshot of the drain event. Count only real empty-queue observations at `wait_for` return with stop false; require a positive fresh delta, zero unexpected admission/pop/Run deltas, and no overflow.
- Q1: One fresh process start for the Q1 condition. Create one Process-owned future message with three inert command lines `-ng039 --probe 0.2`; register its nonzero scalar `InstantiationNumber`; offer it via real `PushToInputQueue`. The drain event for Q1 is valid only after all non-target bootstrap work is reconciled and the future target is the sole queue head. Require a fresh matched wait/decision pair, actual non-due/no-pop, zero target pop/Run-entry/Run-return throughout 100 ms, and future scheduler time at end. No wire-time, NGAL, network, PGCS or performance claim.
- N=1 interpretation: one GW consumer per condition; the paired D2 experiment is proposed as two fresh sequential process starts (Q0 then Q1), separate run IDs/journals/final records under one root-controlled service/continuous lease. If Q0 fails in a way that leaves cleanup/identity/lease uncertain, Q1 is recorded NOT_RUN; do not retry. This is a review question, not operational authorization.
- Stop Gateway, join its output worker, quiesce Process, verify scalar observer state and target ownership, then require exact `EraseMessage` count decrease. Keep the detached target alive through Process/GW destruction and delete it once. Ambiguity means BLOCKED/INCONCLUSIVE, no retry.


## 5. Candidate launcher/lease proposal

The full proposal is preserved as a separate local file and embedded below. It remains uninstalled, untested, and non-normative.

<details>
<summary>Complete launcher/lease proposal</summary>

# NG-039 D2 launcher/lease provisioning proposal — draft only

**Status:** candidate design for owner/reviewer decision; not normative, installed, or tested.  
**Purpose:** satisfy SPEC-039 §7.1's dedicated-identity and continuous-lease prerequisite for one local D2 Q0/Q1 correctness run.  
**Boundary:** this proposal does not authorize D2 code edits, configure/build, harness/finalizer execution, IPC creation/removal, Astra submission, VM/SSH, commit, or push.

## 1. Host facts checked read-only

- Local host exposes `systemd 255.4`; `flock`, `setpriv`, `runuser`, `useradd`, and `groupadd` are available. None was used to mutate state.
- The owner's home directory is mode `0750`; a future D2 UID must not be added to the owner's group or granted access to the home tree merely to reach test files.
- `/var/lib` and `/srv` are root-owned `0755`; `/proc/sysvipc` is readable (`0555`) and `/dev/shm` is host-visible (`01777`). Candidate `<WORKSPACE_ROOT>` is `/var/lib/ng039-d2/workspace`, outside the private home and `.hermes`; §7.1 leaves `<WORKSPACE_ROOT>` to be resolved and recorded in the run manifest.
- No matching named `ng039` account, service unit, or launcher was found in the bounded preflight; this is not an exhaustive root-owned filesystem audit. No D2 test files exist in either examined checkout.

## 2. Candidate identity and files — pending approval

| Item | Proposed value | Required invariant |
|---|---|---|
| Dedicated account/group | `ng039d2` | Non-login shell `/usr/sbin/nologin`; locked password; no supplementary groups, SSH key, sudoers/polkit launch grant, or unrelated system/user service; reserved only for this D2 gate. These facts must be independently checked at install and pre-run. |
| Systemd unit | `/etc/systemd/system/ng039-d2.service` | Root-owned, fixed one-shot entry (`User=root`, `Group=root`, `Restart=no`, `KillMode=control-group`). Only root may start/modify it. Candidate hardening: `NoNewPrivileges=yes`, `ProtectSystem=strict`, `ProtectHome=yes`, `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no`, `PrivateUsers=no`, `PrivateNetwork=no`, `ProtectProc=default`, `RestrictAddressFamilies=AF_UNIX`, `CapabilityBoundingSet=CAP_SETUID CAP_SETGID CAP_KILL CAP_DAC_READ_SEARCH`, plus explicit `ReadWritePaths` for the pinned workspace/control paths and host `/dev/shm`. The exact property set must be reviewed and a no-IPC service-side preflight must prove it does not hide or replace `/proc/sysvipc` or the host `/dev/shm` mount. |
| Supervisor | `/usr/local/libexec/ng039-d2-supervisor` | Root-owned, non-writable by `ng039d2`; fixed argument contract; SHA-256 pinned in the run manifest. It holds the root-only lease, launches only the pinned harness and finalizer without a shell, and drops each controlled child to `ng039d2` using an explicit `setpriv --reuid=… --regid=… --clear-groups --no-new-privs -- …` argv. The harness never inherits the lease FD. The finalizer receives a fixed `--lease-fd <n>` handle after the harness exits; it may inspect the pinned FD but must not close/unlock it. The supervisor retains its own lease FD until finalizer exit and final-record verification. The build/check phase is separate and must finish, be pinned, and be made read-only before this one-shot run service starts. The finalizer is sequential, never concurrent with the harness. |
| Workspace root | `/var/lib/ng039-d2/workspace` (`<WORKSPACE_ROOT>`) | Root-managed and not writable by `ng039d2`. Freeze fresh sibling roots beneath it: `source/<run-id>/`, `build/<run-id>/`, `results/<run-id>/`, and `scratch/<run-id>/{home,tmp}`. Source/manifest and the post-build executable become root-owned/read-only; build output is writable only during configure/build; results and the distinct HOME/TMPDIR are writable only by the D2 UID. Verify pairwise non-overlap, symlink-free parents, device/inode and ownership before use. No `.hermes` paths. |
| Control and lease | `/var/lib/ng039-d2/control/<run-id>/`; `/run/lock/ng039-d2/lease.lock` | Control parents are root-managed; `/var/lib/ng039-d2/control` and the per-run control directory are root:`ng039d2` mode `0710`, so the D2 UID can traverse but cannot list or modify them. `supervisor-termination.json` and closed logs are root:`ng039d2` mode `0440`, opened with no-follow/exclusive identity checks and fsynced/hashed by the supervisor before the finalizer receives read-only access. Lease directory/file are root-managed `0700`/`0600`; supervisor checks regular-file, owner/mode, `st_dev`/`st_ino`, opens with `O_NOFOLLOW|O_CLOEXEC`, obtains nonblocking exclusive `flock`, and continuously verifies the same held FD/identity through final-record closure. `ng039d2` cannot open, replace, or remove the lease. |
| IPC visibility | systemd `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no` | No IPC or mount namespace may hide/replace host resources. Before any Nova/IPC run, a no-Nova preflight under the exact unit must compare service/host IPC namespace identity, raw `/proc/sysvipc` readability/hash and `/dev/shm` mount/device/inode identity without creating/removing IPC. Exact `sem_t`-to-object device/inode mapping remains a required check during the separately authorized D2 reservation phase. No network input. |

**Owner trust assumption:** host root/systemd remains trusted, as allowed by §7.1. The fixed root-owned unit/launcher is the only permitted entry to `ng039d2`; account/session/service/polkit audits plus a live UID census and systemd-cgroup membership check must show no unrelated process before pre-inventory and at every boundary. Any unlisted process or process outside the unit cgroup blocks finalization and preserves evidence. The systemd cgroup is the process-tree source of truth; a process group alone is insufficient. If root-level concurrent mutation is in scope, this shared-host profile is unacceptable and D2 remains blocked.

## 3. Proposed run/supervisor protocol

1. A separately authorized owner action starts the one root-owned service with a root-owned, one-use authorization record that names the paired D2 experiment: one fresh Q0 process start followed by one fresh Q1 process start (`N=1` GW consumer per condition), with separate run IDs, journals and final records. The supervisor atomically marks the authorization consumed before any D2 action; repeated starts fail closed, and another experiment requires another owner decision. No automatic restart or retry. The root supervisor validates the fixed unit, script/binary hashes, manifest, account/session/service/polkit state, workspace roots, result-directory identity and lease-file identity before any inventory.
2. The supervisor obtains the root-only exclusive lease before Q0 pre-inventory and retains it continuously across Q0, its finalizer, Q1, its finalizer and final-record closure. It checks all processes with the D2 UID against the unit cgroup and expected process ledger before/after each phase; any unrelated or escaped process blocks the experiment.
3. Build/configure/checks are not performed by the run service: they require their own post-review authorization, explicit `env -i` commands under `<WORKSPACE_ROOT>`, and a frozen executable hash. Candidate retained environment is exactly `PATH=/usr/bin:/bin`, `HOME=<WORKSPACE_ROOT>/scratch/<run-id>/home`, `TMPDIR=<WORKSPACE_ROOT>/scratch/<run-id>/tmp`, `LANG=C.UTF-8`, and `LC_ALL=C.UTF-8`; HOME and TMPDIR are distinct. Before the run unit starts, the exact source/manifest and executable are root-owned/read-only. After the no-Nova service visibility check and Q0 pre-inventory, the supervisor starts the pinned harness as a single `ng039d2` child via fixed no-shell `setpriv` argv, with a new session/process group and bounded stdout/stderr FDs opened in the protected control directory. The harness itself creates no child process; the Gateway output worker is a thread.
4. The supervisor records the systemd unit/cgroup identity and, for every process, PID, PPID, PGID, `/proc` start time, real/effective/saved/filesystem UID/GID, executable path/device/inode/hash, cgroup membership, and raw `waitpid` status. It compares the live cgroup process set with the expected ledger (root supervisor plus at most one sequential D2 child) at each boundary; unknown members, escaped children, or incomplete reaping are `BLOCKED/INCONCLUSIVE`.
5. After each harness exits, the supervisor fsyncs/closes/hashes its log FDs and writes that run's root-owned `supervisor-termination.json` in the protected control directory. The same binary's `--finalize` mode runs sequentially as `ng039d2`, with read-only receipt/log access and a fixed inherited lease FD while the root supervisor retains its own FD. The finalizer must preserve the handle through final-record closure and must not unlock it. After finalizer exit, the supervisor verifies the lease identity/lock and final-record hash before advancing. Q1 begins only after Q0 finalization and a fresh Q1 pre-inventory under the same lease. Any Q0 failure that leaves inventory, cleanup, identity or lease uncertain stops the sequence; Q1 is recorded `NOT_RUN`, never retried. The supervisor must not invoke/re-enter a finalizer if receipt, log, process-set, identity or lease checks fail.
6. The supervisor seals and hashes each result tree after its final record. It releases the lease only after both run records are closed (or Q0 has failed closed with Q1 `NOT_RUN`) and exits the unit. `KillMode=control-group` is a last-resort containment mechanism, not proof of reaping; each receipt must prove zero unexpected cgroup members and the expected wait results before the next phase. Timeout/signal behavior is pinned in the manifest; no retry or broad cleanup.
7. Any lease/UID/process/log/identity mismatch, timeout, unexpected child, incomplete receipt or failed journal boundary preserves pending/partial evidence and stops. D3 is not part of this unit.

## 4. Installation and test scope — not authorized or performed

Provisioning would require privileged changes to the host account database, `/etc/systemd/system`, `/usr/local/libexec`, `/etc/tmpfiles.d`, `/run/lock/ng039-d2`, and `/var/lib/ng039-d2`. Before installation, freeze exact files, contents, modes/owners, hashes, unit properties, rollback and no-run acceptance tests and obtain owner approval. A no-Nova/no-IPC service check must verify the actual UID transition, root-only unit/launcher and lease permissions, polkit/sudoers restrictions, live UID census, cgroup tracking, IPC namespace identity, raw `/proc/sysvipc` access, `/dev/shm` mount/device/inode visibility, safe control/log FDs, receipt read-only access, one-use authorization and fail-closed behavior. It must not create/remove IPC or invoke the real D2 harness. Exact semaphore backing-object mapping cannot be established by this no-IPC check; prove it only in the separately authorized D2 reservation phase before constructing `Process/GW`.

**Not done:** no user/group, file, systemd unit, lock, directory, or launcher was created; no `sudo` was run; no service was started; no Astra call was made. This design remains a proposal until owner decision and documentary review.


</details>

## 6. Full current governing SPEC-039 v1.16

Canonical file SHA-256 `f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025. The complete current file follows unchanged; this packet requests only D2a pre-code design review, not whole-SPEC approval.

<details>
<summary>Complete SPEC-039 v1.16</summary>

# SPEC-039: Gateway Measurement Baseline on AIOPT5

**Version:** v1.16
**Date:** 2026-10-13
**Author:** Hermes; owner-directed same-SPEC documentary amendment
**Status:** In Progress
**Gate:** D0 remains limited to Q0/Q1. The v1.10/r07 checker amendments received documentary `APPROVE` only. The v1.11/r08 host-profile packet received `AMEND`; follow-up withdrew B1 as a sender-envelope typo and confirmed B2/B3. The v1.12/r09 and v1.13/r10 IPC/finalization findings were addressed in the same SPEC. Astra r12 returned `AMEND` on the journal classifier and cleanup-history projection; r13 recognised those canonical B1/B2 corrections but returned `AMEND` because abnormal-mode policy prohibited the bounded journal replay required by COUT-001–004. The owner authorised the v1.16 same-SPEC documentary correction and local validation on 2026-10-13. The v1.16 model makes the journal replay exception explicit while preserving raw-only handling of other non-control evidence. The model/checker/fixtures remain synthetic and documentary; no production finalizer is implemented or executed. This revision is unreviewed; D2 remains blocked pending any separately authorised exact-SHA review, provisioned prerequisites and a distinct owner execution decision. D3 is not authorised.
**Branch:** AIOPT5
**Baseline commit:** `1af604dad66148618cc0e578c28adb4a9829e607`
**D0-approved v1.1 draft SHA-256:** `8026e1320149c318a5218eaa36c730f262081a5bbe0c2d582b23c307c7144e1c` (owner approval: 2026-10-04; documentary Q0/Q1 scope only; does not approve this v1.16 amendment)
**Owner task:** NG-040, under NG-039
**Related:** `SPEC-032-gw-input-queue-batching.md`, `SPEC-033-message-commandline-refactor.md`

> v1.16 is the owner-authorized same-SPEC response to the r13 abnormal-mode journal-policy AMEND. It preserves the r12 B1/B2 lifecycle corrections and makes bounded, read-only journal-history classification for COUT-001–004 the sole parsing/replay exception in `abnormal_evidence_only`; invalid history remains `unknown` without blocking an otherwise authorized `INCONCLUSIVE` record. Other non-control evidence remains raw-preserved without schema replay; mutation, live IPC inspection, cleanup and retry remain forbidden. The updated model, checker and fixtures are local documentary/synthetic controls—not a production finalizer, runtime test or implementation authorization. A new Astra call is not authorized; D2/D3 remain blocked.

> This AIOPT5 Proposal uses the existing SPEC-039 number; it is not a restoration of the archived AIOPT3 Proposal or its approval. The archived copy was deliberately set aside during the NG-050 release freeze; its SHA-256 is `c46dd50a6d58c760eba165a8dfdc3e390df41e8049cf0f95527655350c80da4f`. Keep that copy intact. The v1.1 D0 approval was documentary only. Later, separately scoped owner permissions are recorded in §4; they do not transfer to other gates or authorise a performance claim.

## 1. Problem and decision

The owner has paused SPEC-027 RAW multi-worker reception and prioritised measuring the Gateway (`GW`) without changing `PGCS` or `PG`. The old Common performance plan and its measurements belonged to AIOPT3 and cannot serve as an AIOPT5 before/after baseline. First establish whether the GW scheduling, network-buffer consumption, or shared-memory (SHM) paths consume material time or strand work. Optimisation selection is a later decision, not a premise of this SPEC.

The committed AIOPT5 `GW.cpp` computes a wait of at most 1 ms and uses `!InputQueue.empty()` as a condition-variable predicate (`Gateway`, near lines 488–515). A future-dated head leaves the queue non-empty and is not popped until its scheduled time is strictly less than `GetTime()` (near lines 523–552). This is a source-level busy-loop candidate, not a measured dominant bottleneck. The Gateway drains the network receive queue under its mutex before attempting a SHM read (near lines 658–763); the relative contribution and fairness of those stages remain unmeasured.

## 2. Scope and immutable starting point

- Current first target: characterise one existing GW consumer (`N=1`) in two conditions only: idle and future-only. Do not infer RAW receive workers from `N=1`. Due-work, network-only and mixed network/SHM conditions are explicitly deferred until the first result justifies a second, separately approved measurement slice.
- For a future execution, use an isolated checkout at the exact baseline commit and record whether any test harness or diagnostics alter that tree. The current AIOPT5 checkout has staged and unstaged changes, including SPEC-027 telemetry in `GW.cpp`/`GW.h`; neither those changes nor the failed 2026-10-02 diagnostic run are a clean comparator. Do not reset, stash, overwrite, or silently transplant them.
- Preserve message bytes, inverted pub/sub, message lifetime, one-due-message-per-iteration scheduling, strict due-time test, priority, output retry semantics, SHM layout/semaphores, and process teardown. `SPEC-032-gw-input-queue-batching.md` remains Deferred.
- Exclude changes to `PGCS`/`PG`, RAW receive concurrency, remote VMs, release/Phase 0 acceptance, full Common/SAR/HT benchmarking, and a GW algorithmic patch. The archived SPEC-041 is a hypothesis and historical Proposal only, not an approved AIOPT5 implementation contract.

## 3. D0-approved measurement design (execution not authorised)

A run is one fresh, isolated local process start with one GW consumer and a fixed, recorded workload. Requests/messages within the same run are not independent repetitions. The initial Q0/Q1 proposal is five independent runs per condition, each with 30 s warm-up, 120 s measured interval, and up to 30 s bounded drain; the owner may revise these constants before authorising execution, but they must then be frozen in a manifest. Never reuse dirty caches, live trial locks, or existing IPC resources at the D2 candidate keys/names.

First gate only, on the same executable and fixed environment:

| Condition | Input | Primary diagnostic question |
|---|---|---|
| Q0 idle | No offered application messages | CPU and polling floor |
| Q1 future head | One message scheduled well beyond the measured window | Immediate returns/cycles while waiting |

Q2 (due-work service), Q3 (network-only publication) and Q4 (mixed network/SHM fairness) remain named questions, not planned runs or acceptance gates. They need a separate owner decision after Q0/Q1. The harness must document how it reaches the real GW path, validates the future message without executing it early, and creates/cleans its own local resources. D0 v1.1 did not authorise source edits; the later, narrowly scoped D2 seam permission is recorded in §4 and does not allow algorithm changes or work outside the frozen test seam.

Q0's zero application-offer stimulus is not proof that `InputQueue` is empty: Process/GW construction runs initialization and can queue bootstrap or periodic messages. Record observed queue state separately; do not claim a true idle floor unless the run proves no queued head or explicitly labels Q0 as “no application offers.”

For every run, capture the baseline SHA and source-tree identity, compiler/libc, effective per-target flags/macros, executable and configuration hashes, workload inputs/seed/rate/payload distribution, CPU allocation, log mode, wall-clock duration, CPU time, peak RSS, and exit/cleanup status. Use separate quiet `-O0` and quiet verified `-O2` builds if approved; their difference is a build-profile effect, not an algorithmic GW gain. Sanitizer builds are correctness diagnostics, never timing comparators. Keep logs bounded; disable per-message DEBUG in timed windows.

For Q0/Q1, independently observe GW cycles, wait calls, immediately returned waits with a future head, blocked duration and wake reasons, process CPU time, peak RSS, SHM poll attempts and maximum inter-poll gap, future-message offered/admitted/pending/executed counts and cleanup status. A future message must not execute within the window; reconcile its pending state. Define each counter's increment point, owner and unit. Missing boundary observations mean `INCONCLUSIVE` for that claim, not zero. First inventory existing counters and their actual update/emission sites; do not assume the dirty SPEC-027 header or a previous smoke supplies them. If Q0/Q1 indicate a material issue, a later approved slice can add due-work completion, queue residence, output retries and network/SHM fairness metrics.

Acceptance of the *Q0/Q1 measurement* gate requires a frozen manifest, repeatable harness/inputs, all planned run outcomes including failed/incomplete runs, raw data plus a small analysis script, source/binary/configuration bindings, and a written diagnosis of future-head waiting versus idle. Report run-level distributions and uncertainty rather than treating cycles as independent runs. This limited result cannot identify the dominant end-to-end bottleneck. Do not call an optimisation beneficial or instrumentation overhead ≤2% without comparable quiet and instrumented measurements that support the specific claim.

## 4. Gates and permissions

1. **D0 — Documentary review:** v1.1 was approved by the owner on 2026-10-04 for Q0/Q1 only, against the v1.1 digest above. This approval does not transfer to other scopes or confer execution authority.
2. **D1 — Local preparation:** pin the exact functional checkout, define the real production-linked fixture, freeze commands/manifest, and obtain scoped authorization for any test-only code/build. Preserve the dirty primary AIOPT5 tree; do not transplant its unrelated changes. The unavailable namespace profile is not replaced by an immediately runnable host profile: §7.1 now requires a provisioned dedicated single-run UID/launcher, otherwise D2 remains blocked before IPC creation.
3. **D1-H — Compile/link probe:** owner-authorised source/build-only probe in an isolated checkout at `1af604dad66148618cc0e578c28adb4a9829e607`. It passed, but the executable was not run. This proves symbol/type linkage only—not GW construction, Q0/Q1 behavior, correctness, or performance.
4. **D2a — Correctness oracle and testability:** the owner authorised a compile-time-gated test seam in `Common` and one local D2 correctness run, with no algorithm change, D3 timing, `PGCS`/`PG`, VM, commit or push. Freeze the exact files, oracle, host-controlled resource-attribution procedure, command and cleanup procedure in the pre-code packet. The current public API does not expose queue contents or pending/execution state; `GW::Gateway()` starts an output worker and touches IPC. For Q1, construct one Process-owned `Message` with an in-memory future `Time` and offer it through the real public `GW::PushToInputQueue` path. Do **not** use `NGAL_CS::DeliverToGateway` for this future-head stimulus: `GW::Gateway()` creates received messages with `NewMessage(0, ...)` (`GW.cpp:614–623`), while wire serialization emits command lines rather than the local scheduling `Time` (`Message.cpp:1599–1602`). The D2 run therefore tests queue admission/scheduling, not NGAL receive/deserialization, wire-format timing, or network delivery. Use three inert, syntactically shaped command lines whose action tuple is not registered in GW; this satisfies `NoCL > 2` (`GW.cpp:241–263`) while ensuring an unexpected early `Run` has no external action.

   The compile-time-gated observer exposes `GWQueueTestObservation` through a test-only scalar-copy accessor; it never retains a `Message*`. The snapshot includes a 64-bit event sequence, validity, one-way Gateway-start/stop state, registered target ID, event message ID, observed queue depth/head identity/head time, actual wait timeout/return, the existing strict due decision and actual pop result, sticky target admission/non-due/pop/Run entry/return counters, monotonic empty-queue count, overflow state, and a first-target-wait record containing its own sequence, target/head identity, head time/depth, timeout, wait return and due result. Value-initialise storage as `GWQueueTestObservation D2Observation{}`; `None` is the zero enum value. `RegisterD2Target` runs once before Gateway start, locks `InputQueueMutex`, rejects ID zero or a started Gateway, resets/binds the observation, and leaves it invalid until the first real event. Set `gateway_started=true` under that mutex before entering the loop; never clear it or allow re-registration. Writers increment sequence, update scalar data and counters, then publish validity; counters saturate and set overflow rather than wrap. At the first real `wait_for` return with the registered target as head, fill the sticky first-target-wait record once; later events cannot overwrite it. At Q0/Q1 start the harness captures sequence/counter baselines and tests for new events before the window ends. Writers at already-locked admission/wait/pop sites use a non-locking helper; Run/stop boundaries take the mutex only outside those scopes. No I/O, callbacks, or test-only symbols/state/calls in the default build.
5. **D2b — Correctness test code/build/run:** only after this v1.14 host-controlled profile is positively reviewed under fresh exact-SHA consent, the dedicated-identity preconditions are provisioned and verified, and a separate owner execution decision is recorded, implement only the frozen allowlist: root `CMakeLists.txt`; `Common/src/GW.h`; `Common/src/GW.cpp`; new `tests/spec039/gw_q0_q1_d2.cpp`; and new `tests/spec039/q0_q1_d2.manifest.json`. Add the separate option `NG_ENABLE_NG039_GW_Q0_Q1_D2` (default `OFF`) and target `ng039_gw_q0_q1_d2`; define `NG_GW_QUEUE_TEST_OBSERVATION` for `Common` and the harness only inside an isolated diagnostic target graph: the option-on root CMake path must declare only `Common` and this harness and return before production application targets, with a configure-time target-census guard rejecting any other target. Keep the existing D1-H harness/manifest as historical evidence and do not overwrite them. Gate observer declarations, storage, methods and production call sites together. In the diagnostic build only, guard all three pinned `GW.cpp` branches that call `sem_unlink` after `sem_open` failure (`GW.cpp:970,1263,1322`): they must record a bounded failure event and must not call `sem_unlink`; the macro-off default build preserves existing behavior. Add targeted tests proving each injected `sem_open` failure path performs no unlink. This safety change is specified, not authorized for implementation by this document. Build the D2 target in a fresh isolated checkout at the pinned functional baseline. The dedicated-UID launcher and lease must be pinned and verified before any D2 action; until provisioned, no launch command is authorized and the old direct `timeout <harness>` invocation is prohibited. The manifest must freeze exact launcher/lease identity, run/finalizer commands, normal-versus-abnormal mode predicates, the `supervisor-termination.json` schema, and process-tree bounds before the separate execution gate. Resolve all path aliases before any command. Keep worktree, build directories, HOME/TMPDIR and results outside `.hermes`; use only named workspace/repository roots. The harness spawns no child processes.

   The trusted dedicated-UID launcher and exclusive lease must be verified before any IPC action; the direct host-UID launch command is prohibited. Only then capture complete host inventories for `/proc/sysvipc/{shm,sem,msg}` and `/dev/shm`; host inventories may be non-empty, but all five candidate key/name pairs must be absent. Derive and record `D2_PROCESS_KEY` and selected attempt using §7.1's exact algorithm; candidate keys are `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. Embed exact raw `/proc/sysvipc` bytes (canonical base64) in each inventory artifact; finalizer must recompute raw digests and normalized summaries from those bytes. Create and fsync `reservation-journal.jsonl` before reservation. Abort before construction on unreadable/over-cap inventory, candidate collision, key overflow, missing lease, or ambiguity. Then exclusively reserve all five SHM segments and five POSIX semaphores and journal each intent/outcome. NovaGenesis constructors may open only these reserved resources. A collision, mode mismatch, unverified returned-handle/libc mapping, incomplete journal or failed reservation blocks construction/PASS; do no speculative cleanup.

   After Gateway/output threads stop and join and `Process/GW` are destroyed, retain the dedicated-UID lease through finalization. Remove only exact run-created SHM IDs/names whose successful journal receipts and current identities match. Name-based `sem_unlink` is permitted only while the verified exclusive execution domain remains held; if the lease is lost, another process appears, or identity/journal state is ambiguous, perform no unlink/RMID for the disputed object and preserve residue. Append/fsync each cleanup intent and outcome; an intent without a durable outcome is `INCONCLUSIVE`, never evidence of removal. Capture final inventory, including exact raw table bytes, and require it to match pre. Any failed cleanup, residual candidate, unexplained change, blocked join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad cleanup. The finalizer verifies and hashes the journal and all bundle artifacts. Keep D2 at `N=1`, Q0/Q1 correctness only; no performance claim, algorithm change, `PGCS`/`PG`, VM, commit or push unless separately authorized.
6. **D3 — Characterisation:** Q0/Q1 timing is a separate gate, allowed only after D2 passes and the owner explicitly authorises the frozen measurement plan. Preserve all run outcomes and identities; classify the future-head hypothesis `MEASURED`, `INCONCLUSIVE`, or `BLOCKED`.
7. **D4 — Follow-up decision:** decide whether to stop, approve another measurement slice, or propose a performance implementation SPEC. Any future change requires a predeclared baseline/metric, actual-source review, correctness controls and its own owner-approved implementation allowlist. Astra or subagent opinions never grant implementation or runtime authority.

### 4.5 GPT-6 Astra consultation checkpoints (advisory; no execution authority)

| Checkpoint | Packet and question | Gate informed |
|---|---|---|
| Before D2 test-seam/code | Exact SPEC/manifest/packet hashes, functional baseline, D1-H manifest, proposed observer/interface diff and isolated CMake target graph; the dedicated-UID launcher, exclusive lease, `supervisor-termination.json` wait receipts and exact process-tree evidence; all three guarded `sem_unlink` paths; journal event schema/state transitions; raw `/proc/sysvipc` retention/recomputation and exact returned-`sem_t`→libc/dev-shm inode mapping. Ask whether the trust boundary actually excludes same-UID mutation from pre-inventory through finalizer, whether any failure path can still unlink an unowned object, whether crash/replay semantics forbid ambiguous cleanup, whether normal-cleanup and abnormal-evidence-only finalizer modes are mutually consistent, whether process-tree wait receipts and closed logs suffice to authorize an INCONCLUSIVE-only no-cleanup path, and whether the schemas can support a non-vacuous PASS without claiming unverified evidence. | The frozen v1.10/r07 B1/B2/B3 packet received `APPROVE` only for its documentary findings. The immutable v1.11/r08 packet `3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272` received `AMEND`; follow-up withdrew B1 (sender-envelope typo) and retained B2/B3. The v1.12/r09 packet returned `AMEND` on the IPC boundary; the v1.13/r10 packet closed that issue but returned `AMEND` on abnormal finalization. The v1.14/r12 and v1.15/r13 exact-SHA reviews are archived; r13 returned `AMEND` on the abnormal journal-policy contradiction. The owner authorised this v1.16 documentary correction and local validation only. Any subsequent Astra review requires a separate exact-SHA decision; no review grants D2 authority. |
| After D2 correctness result, before D3 | Frozen diff/build/test manifest, all raw outcomes including failures, identities and cleanup evidence. Ask whether the D2 assertions are supported and what remains inconclusive. | Planned checkpoint; a separate Astra submission requires owner consent. The owner decides whether to accept D2 and authorize D3. |
| After D3, before any implementation SPEC | Frozen raw data, analysis, source/build/workload identities and hypothesis classification. Ask whether the evidence supports a future implementation proposal and its acceptance metric. | Future checkpoint only; separate owner consent is required. Astra advice never authorises production changes. |

The owner authorised one GPT-6 Astra review via OpenAI Codex for the frozen v1.2 pre-code D2 packet. That call returned HOLD on the v1.2 SPEC SHA-256 `8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91` and packet SHA-256 `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`; its consent was consumed. On 2026-10-05 the owner separately authorised one review of the sanitized v1.4 packet SHA-256 `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948`, bound to SPEC v1.4 SHA-256 `f54901cad03209f94fe98cfad3bf62953895ed24fef3d771a89b89f5229217e1` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`. Astra returned AMEND, response ID `resp_0a12ead61fee5339016ac370498edc87d28ec6212dafa33cf9`. The archived raw SSE SHA-256 is `25b8c29265d327c34323e12f2c35177c2069c13b1d7e99f83a42ffea75240a58`; archive manifest SHA-256 is `46f41c9af22a6b1c9e86e2a66682dfd83f7323fb25d1c47aded2aa52c95e6782`. The r03 AMEND is advisory; its one-call consent is consumed and grants no implementation, build, runtime, IPC/VM, or D3 authority. Raw SSE SHA-256 `14f95d55a4a5611f518a0e236c98edd255e6809acd256b105177f975577bef09`; parsed-response SHA-256 `6fae28a46fd385e0d5aa87e8a1bc7149c832fcd8878170b470dab558941bc587`; archive manifest SHA-256 `b26c7767c9a74f2c9ee716a59257f79064c5480191addbc5f61da998989ca5fd`. The packet’s “NOT submitted” line is a time-frozen pre-review snapshot; this paragraph records the later submission/result. The v1.6 r04 packet SHA-256 `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829`, tied to the v1.6 SPEC SHA-256 `3296530c13801de945ed97b920532533c1a4a44d628075a9c5e61cdcc7ee01cb` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_0d91609fd866592e016ac3b3b903e487d2891f1ae2407fb075`). Raw SSE SHA-256 `a1c62af8a4e35a59aec3428fdf45818f2dd89b3f57dce9c3bf8be9a6279fe08d`; parsed-response SHA-256 `3655369b4c6c348e439ec87e48f6a8c1e789c46d5d4500890bee8577b8a969d3`; archive manifest SHA-256 `3f6dbf25e22ffd3ce5ce9290935fd41db162fd81aa67923c445b5ea250fbe2a8`. That exact-SHA consent is consumed. The review closed r03 findings 3 and 4b–4d at design level only; finding 4a remains must-fix. The v1.7 r05 packet SHA-256 `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a`, tied to SPEC v1.7 SHA-256 `b73bcc7361bb797b4ddd40952860f0092fb840bb11b31bf0a99659bef8475b70` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). Raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`; parsed-response SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`; archive manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a`. Usage: 22,569 input / 1,650 output / 24,219 total tokens; 8.11725 estimated credits (not USD); quota at return 5h 51%, weekly 86%. Three 4a must-fixes remain: preprocessor-forwarded and function-like observer macros; `-ansi` dialect override; Python assertions disabled by `PYTHONOPTIMIZE=1`. Optional hardening: actual `-o` target mapping, dynamic-linker conflict rejection, and stronger run-record schema `allOf`. Findings 3 and 4b–4d remain design-level closures only. The r05 consent is consumed. SPEC-039 v1.8 is local/unreviewed; any r06 review requires fresh exact-SHA consent. D2 remains blocked.

No VM/SSH/QGA operation, algorithmic production change, commit, push, or release is authorised by these gates. All Astra reviews to date were separately owner-authorised by exact packet SHA and their one-call consents are consumed; none expands the D2 seam/runtime scope. The old NG-062 trial locks and evidence remain untouched.

## 5. Files and evidence

| Path | State/intent |
|---|---|
| `Specs/SPEC-039-common-performance-measurement.md` | v1.16 same-SPEC response to r13 AMEND; preserves immutable r12/r13 submissions and defines the bounded read-only journal-history exception for COUT-001–004. Documentary/synthetic only. Dedicated-UID launcher and diagnostic no-unlink behavior remain unimplemented/unprovisioned. Any Astra call and execution require separate authorization. |
| `Common/src/GW.cpp`, `Common/src/GW.h` | Read-only under D0; a D2 seam may touch only the explicitly frozen compile-time-gated scope in §4. |
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No modifications to NGAL, `Process`, `PGCS` or `PG`, no algorithm change, separate source helper file, or D3 timing files are proposed. The D2 harness may construct the existing `Process/GW` under test; the same allowlisted harness binary's `--finalize` mode runs after the test exits without constructing `Process/GW`, creating IPC resources, or sending/receiving production messages. In `normal_cleanup` mode, it requires a durable `RUN_SHUTDOWN_COMPLETE` journal event proving worker joins and `Process/GW` destruction, plus a validated required `supervisor-termination.json` artifact containing one raw `waitpid` receipt per recorded process and stdout/stderr closure state/hashes; only then may it read inventories, inspect exact identities, and perform journal-authorized `IPC_RMID`/`sem_unlink` under the continuously verified lease. In `abnormal_evidence_only` mode, only while trusted launcher, execution/result-directory identities and lease remain valid and a valid supervisor receipt proves every process reaped and logs closed, the finalizer may hash raw, safely readable allowlisted evidence without requiring the complete bundle or parsing/replaying non-control artifacts. It writes `INCONCLUSIVE`; `failure.missing_artifacts` and `failure.unavailable_artifacts` are disjoint, accurate sets; cleanup history is exactly one of `not_started`, `partial`, `complete` or `unknown`, derived from valid evidence and never forced to `not_started`. It performs no IPC inspection, cleanup, journal mutation or retry. If lease, receipt, process/log proof or result-directory authority is unconfirmed, leave the pending record and report `BLOCKED/INCONCLUSIVE`. The trusted dedicated-UID launcher/lease is a provisioned external runtime prerequisite, not part of the five-file code allowlist; it does not exist here and direct host-UID execution is prohibited. |
| Isolated D1-H artifacts | Historical manifest `tests/spec039/q0_q1.manifest.json` SHA-256 `9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a`; source file hashes match. A fresh compile/link-only rebuild on 2026-10-10 used the unchanged isolated baseline checkout and an output root under `workspace/ng039-d1h-rebuild-20261010/` (outside `.hermes`): configure/build exit 0, CMake 3.28.3, GNU C++ 13.3.0, binary SHA-256 `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`, configure-log SHA-256 `65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb`, build-log SHA-256 `658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98`. This is a new build, not recovery of the old binary; the executable was not run, and this proves no correctness/performance behavior. |
| `Specs/RESULTS-SPEC-039/` | Contains historical r06/r07/r08 review packets and responses plus the D1-H build-only receipt; none is a D2/D3 runtime result. The v1.12/r09 and v1.13/r10 packets were submitted and returned `AMEND`; the v1.14/r11 draft is documentary only. No correctness or timing result exists. Historical AIOPT3 results remain in the NG-050 archive. |
| Obsidian NG-039 / NG-040 | Scope and blockers; neither an experimental oracle nor build evidence. |

## 6. Rollback and open questions

If the approved D0 design is later abandoned, mark this Proposal as abandoned/superseded instead of deleting it; leave the archive, current AIOPT5 diffs and historical trial evidence untouched. Later test-only changes must have an explicit allowlist and a reversible isolated checkout.

Before D2 code: freeze the compile-time-gated observer, exact five-file allowlist, in-memory future-message admission through `PushToInputQueue`, diagnostic no-unlink behavior, dedicated-UID launcher identity/lease, journal/inventory schemas and §7.1 cleanup oracle in a new pre-code packet. Obtain fresh exact-SHA consent before Astra. Do not apply the diff until positive documentary review and a separate execution decision; launcher provisioning is also a distinct prerequisite. Q0 distinguishes zero application offers from bootstrap queue state; Q1 uses a future `Message::Time` created locally because wire serialization does not carry that scheduling field. Cleanup must reconcile the Process-owned pending test message only after Gateway/output workers stop. The owner’s D2 scope is one correctness-only local run and does not include NGAL ingress coverage, D3 or algorithm changes. Q2–Q4 remain deferred; do not import approval or measurements from archived AIOPT3 SPECs.

## 7. D2a closure amendment — v1.8–v1.16 (documentary; not implementation authorization)

This section preserves the unchanged D2 correctness scope and the r08–r13 review history. The v1.12/r09 packet's IPC-boundary B1 was resolved in v1.13/r10. The v1.14 amendment separated normal cleanup from abnormal evidence-only finalization. The v1.15 amendment formalized the journal lifecycle/outcome model and resolved the r12 orphan-outcome and projection-drift findings. The v1.16 amendment resolves the r13 policy contradiction by separating bounded journal-history replay from other non-control evidence handling. No D2 code, configure/build, runtime or IPC is authorized by this documentary amendment.

### 7.1 Host-controlled execution profile — v1.14 baseline with v1.16 journal-policy amendment (documentary; execution blocked)

The v1.11 direct launch under the ordinary host UID is prohibited for D2. Mode `0600`, umask `0077`, exclusive creation and a pre-inventory do not prevent a same-UID process from replacing a POSIX semaphore name or removing/reusing a SysV resource during the run. This amendment does not claim the current host can satisfy the stronger prerequisite. No D2 configure/build, harness/finalizer execution, IPC creation/removal or measurement may occur until the dedicated single-run identity and launcher are provisioned, independently inspected, pinned in the manifest, and separately authorised by the owner. If they cannot be provided, the D2 execution profile remains `BLOCKED` before any candidate resource is created.

The run and its sequential finalizer must execute under a dedicated non-login UID reserved solely for this D2 run, through a trusted owner-controlled single-run launcher. The launcher must prevent another process from entering that UID's execution domain from before pre-inventory until finalization/lease release, hold an exclusive run lease across launch and finalization, and record its exact path, file identity/hash, UID/GID, lease identity and process-tree identity. A process census alone is not proof of exclusivity. The ordinary interactive UID, an unverified cooperative lock, or a missing launcher/lease is insufficient. Only controlled supervisor/harness/finalizer descendants for this run may use the UID; no login shell, unrelated service or concurrent run may use it. Treat host root/administrator as trusted; if root-level concurrent mutation is in the threat model, this shared-host profile is unacceptable and remains blocked. Record precondition evidence in the run record. Loss or uncertainty of identity/lease is `INCONCLUSIVE`, prohibits further name-based unlink/RMID, and preserves residue for owner-directed recovery. The launcher is outside the five-file code allowlist and has not been provisioned or authorised by this documentary amendment.

The three pinned `GW.cpp` paths at lines 970, 1263 and 1322 call `sem_unlink(name)` when `sem_open` fails. In the future diagnostic build only, the already-listed `NG_GW_QUEUE_TEST_OBSERVATION` compile-time guard must suppress all three unlinks and emit a bounded failure event; the normal macro-off build remains unchanged. Failure to open a semaphore aborts the diagnostic run; only the separate finalizer may clean a resource, and only from a complete reservation journal under a still-valid dedicated-UID lease. Synthetic fault-injection coverage must prove each path makes zero `sem_unlink` calls. This required source behavior is design text only; code changes still need separate owner authorisation.

Before constructing `Process/GW`, capture complete host-visible inventories for `/proc/sysvipc/shm`, `/proc/sysvipc/sem`, `/proc/sysvipc/msg`, and `/dev/shm`. Global inventories may be non-empty, but every candidate key/name must be absent. Capture each raw `/proc/sysvipc` table byte-for-byte in canonical base64 fields inside its phase inventory artifact; the finalizer decodes those bytes, recomputes each SHA-256 over the exact raw bytes, parses the table using the pinned Linux table grammar, and independently recomputes normalized rows/IDs before comparing them to stored summaries. Reject non-canonical base64, duplicate/invalid rows, digest mismatch, parse mismatch, unreadable/over-cap input or any inventory artifact exceeding its bound as `INCONCLUSIVE` before resource creation. The `/dev/shm` structured listing is sorted by bytewise name and retains each entry's kind, device, inode, UID/GID, mode, link count and size; the finalizer compares actual post/final listings with captured records. Inventory incompleteness never means “empty”.

Choose `D2_PROCESS_KEY` deterministically: for attempts `0..31`, compute `0x40000000 + (uint32_be(SHA256(ASCII(run_id + ':' + decimal(attempt)))[0:4]) mod 0x3FFFFFF8)` and select the first base whose four-key range is unoccupied and fits positive signed 32-bit `key_t`; key `11` is checked separately. Record the selected attempt and candidate set `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. If no base is available, abort `INCONCLUSIVE`.

Before any candidate create, create `reservation-journal.jsonl` in the run result directory using exclusive non-symlink creation and mode `0600`; fsync the file and containing directory. This file is a required runtime artifact, capped at 256 KiB and 128 events, one strict JSON object per newline-terminated record, with monotonically increasing sequence numbers and the same run ID; use `ng039-reservation-journal-event/v2` for the event schema. Each event contains schema version, sequence, event kind, monotonic timestamp, process PID/UID, resource kind, candidate key/source name, and nullable identity/result fields. Event kinds are `JOURNAL_OPENED`, `CREATE_INTENT`, `CREATED`, `CREATE_FAILED`, `RUN_SHUTDOWN_COMPLETE`, `CLEANUP_INTENT`, `REMOVED`, `CLEANUP_FAILED`, and `JOURNAL_CLOSED`. Append/fsync `RUN_SHUTDOWN_COMPLETE` exactly once only after all Gateway/output workers have stopped and joined and `Process/GW` has been destroyed; it must precede every `CLEANUP_INTENT`. Write and fsync `CREATE_INTENT` before each `shmget`/`sem_open`; after the call, append and fsync exactly one `CREATED` record with returned identity or `CREATE_FAILED` with errno. Keep the descriptor open, verify its file identity before each append, and never rewrite/truncate prior events. In normal cleanup, the finalizer appends/fsyncs each `CLEANUP_INTENT` before cleanup and `REMOVED`/`CLEANUP_FAILED` after. It may close the journal only after final inventory capture and all events are durable. Abnormal evidence-only finalization never appends to, repairs or closes this journal. A missing event, sequence gap/duplicate, partial line, identity mismatch, crash between intent and outcome, or failure to fsync makes the run `INCONCLUSIVE`; the finalizer must not infer success or retry deletion for an ambiguous resource.

After pre-inventory and journal readiness, set harness umask to `0077`; exclusively reserve five SysV SHM keys with `shmget(..., IPC_CREAT|IPC_EXCL|0600)` at pinned `MaxSegmentSize` and five exact POSIX semaphore names with `sem_open(..., O_CREAT|O_EXCL, 0600, 1)`. Verify effective modes are exactly `0600`. Each `CREATED` event records exact key/name and exclusive-create evidence: for SHM, `shmid`, key, `IPC_STAT` size, creator PID/UID, owner UID and mode; for semaphore, exact source name, `sem_t*` address, `/dev/shm` resolved path, device/inode/UID/GID/mode/link count, and libc identity. A failed/mismatched reservation aborts before `Process/GW` construction; remove only resources whose successful journal receipt and current identity are exact while the lease remains valid. Otherwise do no deletion and report `INCONCLUSIVE`.

For each semaphore, record the `sem_open` provider via `dladdr`, resolved libc path, file device/inode, GNU build ID when available, SHA-256 of the opened libc file, `gnu_get_libc_version()`, and exact production source name/path. Immediately after successful exclusive `sem_open`, capture `/proc/self/maps` and prove the returned `sem_t*` address lies within a shared mapping whose device/inode equals the `stat` identity of exact `/dev/shm/sem.<decimal-name>`. Retain the exact matching maps line or a strict projection including address range, permissions, offset, device and inode in the reservation journal. Recompute/validate this linkage from retained evidence; version/path plus a boolean is insufficient. The mapping procedure is target-libc-specific and is not claimed tested here. If `sem_open` returns no handle, mapping is absent, path/device/inode disagree, or mapping cannot be captured, abort before `Process/GW`; do not set `semaphore_name_mapping_verified=true`, do not PASS, and do no speculative deletion.

After Gateway/output workers stop and join and `Process/GW` are destroyed, the harness must append/fsync exactly one `RUN_SHUTDOWN_COMPLETE` event to the still-open reservation journal. This event is the positive proof that normal shutdown completed; a caller boolean or process exit is not a substitute. The trusted supervisor then waits/reaps every recorded harness process PID and closes stdout/stderr before invoking the same allowlisted `--finalize` binary. It selects `normal_cleanup` only when the journal contains exactly one valid shutdown-complete event, every recorded process has a verified wait receipt, both logs are closed, no timeout/signal/ambiguous termination occurred, and the lease remains continuously valid. In this mode the finalizer captures post inventory, validates/replays the journal, reads required inventories, inspects exact current identities (`shmctl(..., IPC_STAT, ...)` for SysV SHM and `stat`/equivalent metadata for the exact POSIX semaphore object), and removes only journal-proven run-created resources. For each resource it appends/fsyncs `CLEANUP_INTENT`, rechecks lease and identity immediately before the destructive call, invokes only exact `shmctl(exact_shmid, IPC_RMID, ...)` or `sem_unlink(exact_recorded_name)`, then appends/fsyncs the matching outcome. The `normal_cleanup` gate above is satisfied only while the trusted launcher, supervisor receipt, execution identity, result-directory identity and continuous lease remain valid through final-record closure. If normal-shutdown proof is absent while these common authority/file-safety conditions remain valid, the finalizer may choose `abnormal_evidence_only`. Loss or ambiguity of the lease, supervisor proof, execution identity or result-directory authority is not an abnormal-mode trigger: do not invoke or re-enter the finalizer; preserve the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A failure during normal cleanup after any `CLEANUP_INTENT` stops further destructive calls and retries; while authority remains valid, record `INCONCLUSIVE` with the evidence-derived cleanup outcome in the current finalization, otherwise retain pending recovery state. No ambiguous deletion is retried.

In `abnormal_evidence_only` mode, only while the trusted launcher, execution identity, result-directory identity and continuous lease remain verified, and after a valid `supervisor-termination.json` proves every recorded process was reaped and both logs were closed, the finalizer strictly validates the pending run record and supervisor receipt. It may then open only allowlisted evidence names using no-follow regular-file checks, preserving and hashing safely readable closed raw bytes within per-file caps; it does not require all eight evidence files or enforce the normal complete-bundle/schema checks. `failure.missing_artifacts` lists required evidence names actually absent; `failure.unavailable_artifacts` lists required names present but unreadable/unsafe/over-cap. These sets are disjoint, exclude the run record and optional `harness-diagnostics.txt`, and neither may falsely classify a present file as missing. Malformed but safely readable non-control evidence is preserved and hashed, not parsed or used to block an otherwise permitted `INCONCLUSIVE` record. The bounded journal-history classifier validates event shape, sequence, phase/resource transitions and the stable resource-identity binding defined by §7.2; it is not a full runtime finalizer. `failure.cleanup_outcome` is derived only by the §7.2 `COUT-001`–`COUT-004` rules. Invalid, malformed, truncated, orphan, identity-mismatched or uncovered-resource history yields `unknown`. Abnormal mode always sets `record_finalized=true`, `finalization_mode=abnormal_evidence_only`, and `status=INCONCLUSIVE`, including when the historical cleanup outcome is `complete`. A cleanup outcome never proves final inventory or PASS. The abnormal finalizer performs no new cleanup, does not retry or mutate the journal, and preserves its bytes. If the lease, receipt, process/log proof or result-directory authority fails at any point, do not write/re-enter finalization; keep the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A present but partial, stale, or shutdown-unverified `ipc-post.json` or `ipc-final.json` may be preserved and hashed as a file, but must not be treated as a verified inventory: the run-record post/final inventory summaries remain unavailable, with `is_empty=null` and no claimed inventory digest. The finalizer never reads `/proc/sysvipc` or `/dev/shm`, calls `IPC_STAT`, `IPC_RMID` or `sem_unlink`, appends/repairs/closes the journal, or performs cleanup during this mode.

In `normal_cleanup` mode, capture a complete final inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required runtime artifact exactly once. The finalizer strictly parses journal/inventories, replays the §7.2 transition/identity model, recomputes raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. PASS/FAIL require the complete bundle and safe normal cleanup; missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. If failure occurs after any `CLEANUP_INTENT`, stop subsequent destructive calls and retries; while authority remains valid, derive `cleanup_outcome` from the closed, transition-valid journal under §7.2. `partial` requires every created resource to have one terminal cleanup outcome and at least one proven `CLEANUP_FAILED`; an uncovered or ambiguous resource is `unknown`. If a complete closed journal proves every created resource was removed and no cleanup failed, preserve `cleanup_outcome=complete` even when a later evidence/inventory/final-record check fails; the overall run remains `INCONCLUSIVE` unless all normal acceptance gates pass. Cleanup history never upgrades PASS or substitutes for inventory evidence. If authority or safe record writing is lost, leave pending/partial evidence and report `BLOCKED/INCONCLUSIVE`; never re-enter finalization to claim `not_started`. The abnormal evidence-only path preserves the historical cleanup outcome and performs no further cleanup. D2 remains one controlled correctness run only; no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.

1. Host-controlled preflight before construction: the run starts only after the §7.1 inventory and attribution prerequisites pass. No constructor-time IPC may precede that successful preflight.

2. HT construction, Message lifetime, and failure-safe ownership: after the host-controlled preflight succeeds, construct the isolated non-`PGCS` `Process`. Its pinned constructor calls `NewBlock("HT", ...)` then `NewBlock("GW", ...)` (`Process.cpp:228–230`), and the `GW` constructor retrieves that HT (`GW.cpp:99–101`) before initialization dereferences it (`GWRunInitialization01.cpp:68–80`). Do not call `NewBlock("HT")` or `NewBlock("GW")` again; verify exactly two blocks exist and one lookup succeeds for each name. Before starting Gateway, create the Process-owned target, capture `GetDeleteFlag()==false` while single-threaded, and bind its ID with the observer. Capture the immutable scalar `Message::InstantiationNumber` (`Message.h:158`) before each production `Run`; use only that copied ID for Run-entry and Run-return events, publishing the return event before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after a possible reclamation; duplicate, zero, stale or wrapped identity is INCONCLUSIVE. The three-argument constructor used by `Process::NewMessage` initializes `Delete=false` and `ApplicationDeleted=DELETED_BY_CORE` (`Message.cpp:37–70`, especially 63–64); `DeleteMarkedMessages` deletes only true-flagged messages (`Process.cpp:885–917`). Keep the target out of the queue and do not mark it through Q0; use exactly three inert command lines `-ng039 --probe 0.2`, which match neither the registered action nor the `0.1` fallback in `Block::Run` (`Block.cpp:244–313`). For the expected Q0/Q1 pending path, after Gateway and its output worker stop/join, verify no target pop/Run and `Delete=false`; call `EraseMessage` only while Process is quiescent and require `GetNumberOfMessages()` to decrease by exactly one. Keep the detached object alive through `Process` destruction: its destructor calls `DeleteMessages()` before deleting `Blocks`, so after EraseMessage it will not own/delete this target; `GW::~GW` destroys actions, while the queue container drops raw pointer values. Delete the detached target exactly once after Process/GW destruction. If any target pop/Run, delete flag, count, or identity makes ownership uncertain, never dereference, erase, delete, or invoke the C++ Process/GW destructors on a possibly stale pointer: write a bounded/fsynced `run-record.pending.json` with `record_finalized=false` and no fabricated measurements, exit nonzero via `_Exit`, and let the isolated process/namespace terminate. The trusted supervisor must wait/reap every PID in the frozen harness process tree, close stdout/stderr, and write/fsync the required `supervisor-termination.json` containing exact per-PID raw `waitpid` statuses, log-closure state, and hashes of available closed logs. Only after every PID is reaped and both logs are closed may the parent-side finalizer in item 5 run `abnormal_evidence_only`: it writes a final `INCONCLUSIVE` record with unavailable post/final inventories and every missing artifact explicit, leaves the journal byte-for-byte as found, and performs no IPC inspection or cleanup. Absence of `RUN_SHUTDOWN_COMPLETE` forbids normal cleanup; process exit does not prove worker join or `Process/GW` destruction. A partial construction, blocked join, timeout, signal, or uncertain ownership follows this same no-cleanup path. If termination receipts or log closure are incomplete, do not invoke the finalizer or release the lease; keep the pending record and classify the gate `BLOCKED/INCONCLUSIVE`.

3. Fresh, correlated observation contract: keep the 64-bit event sequence and add a saturating `gateway_iteration_id`, assigned once at the start of each Gateway loop iteration and carried across its wait and due/pop decision. Preserve two distinct sticky scalar records: (a) while the existing `InputQueueMutex` lock is held when the real `wait_for` returns, if the registered target is the head, capture `WaitCapture{sequence, iteration_id, target_id, head_id, head_time, depth, timeout_ms, predicate_return}` once and never overwrite it; (b) while the production due/pop critical-section lock is held, capture the corresponding `DecisionCapture{sequence, iteration_id, candidate_id, candidate_time, actual_due_branch, actual_popped}` from the existing strict comparison/pop branch, without performing a second comparison, and never overwrite that capture. The records correlate only if the iteration IDs match, `decision.sequence > wait.sequence`, and the wait target/head ID matches the actual decision candidate. Set `correlation_status=MATCHED` only then. Otherwise use `PENDING`, `DECISION_NOT_REACHED`, `HEAD_MISMATCH`, `INVALID`, or `OVERFLOW`; no incomplete pair is exposed as a valid due result. Stop before the decision, a changed head, invalid/duplicate/wrapped ID, missed event, stale sequence, or counter overflow makes Q1 BLOCKED/INCONCLUSIVE, never zero/PASS. Under Q0/Q1, capture sequence/counter baselines at each window start: Q0 requires a fresh empty-observation delta inside its 100 ms no-controlled-offer window after bootstrap drain; Q1 requires target admission/head identity, a matched wait/decision pair newer than the Q1 baseline, zero target pops/Run entries/returns throughout 100 ms, and a future target time at end. Use `std::chrono::steady_clock` for correctness windows; `Message::Time` remains the scheduler clock. Set `gateway_started=true` under `InputQueueMutex` before the loop and never reset it; later registration fails. Before production `Run`, copy `Message::InstantiationNumber` to a scalar; emit Run-entry/return with that copy and publish the return before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after possible reclamation. These are correctness-window rules, not timing/performance evidence.

4. Bounded source/build verification contract: the five-path code allowlist and the CMake target census remain separate checks. The diagnostic graph contains exactly `Common` and `ng039_gw_q0_q1_d2`; the normal graph contains the pinned default targets, with the observer macro absent. The root CMake file returns before production application targets in the diagnostic branch. The compile-database checker receives the database, profile, pinned worktree root and fresh build root as four positional inputs after `python3 -c <checker>`; do not insert a `--` separator unless the checker explicitly consumes it. It verifies that each compile entry's working directory and actual `-o` output resolve under the build root, the source operand and entry `file` resolve to the same regular source inside the worktree, any `output` metadata agrees with `-o`, exactly one `-c` occurs, and compiler realpath/target census match the manifest. Require exactly one `-std=gnu++20`, only `-O0` optimization tokens, and the observer define exactly once on `Common` and the harness in diagnostic mode and nowhere in default mode. Parse joined and split `-D`/`-U` switches by base identifier, including function-like macro spellings; reject every function-like or undefined `NG_GW_QUEUE_TEST_OBSERVATION` form. Reject response files, `-Wp`, `-Xpreprocessor`, `-ansi`, `-traditional`, `-x`, forced-include and macro-file forms in both split (`-include file`, `-imacros file`) and joined (`-includefile`, `-imacrosfile`) spelling, plus toolchain-selection and plugin forms that can bypass those checks. Do not reject unrelated warning/debug switches through a purported universal grammar; preserve every complete argument vector and the compile database SHA-256 as evidence. This checker is scoped to the pinned CMake output and is not a generic validator for arbitrary compile databases.

Resolve the roots before configure. Use fresh, pairwise-disjoint source/build/results paths beneath `<WORKSPACE_ROOT>`; never place NovaGenesis source, build or result artifacts in `.hermes`. Invoke configure, build and both checkers through the same minimal `env -i` environment, retaining only explicit `PATH`, `HOME`, `TMPDIR`, `LANG` and `LC_ALL` values. Set `HOME` and `TMPDIR` to distinct directories under the named D2 scratch root. Record those retained values, command working directories, tool versions and exact commands in the manifest/run identity; inherited environment variables are not passed through. No broad environment-variable inventory or custom denylist is required.

For link validation, invoke `python3 -c <checker> <link.txt> <build-root> ng039_gw_q0_q1_d2` without an unconsumed `--`; the checker opens that exact named `link.txt` itself, requires a regular non-symlink file at the expected diagnostic target path under the build root, and never validates stdin or another caller-supplied string. Parse its contents with `shlex.split(posix=True)`, verify the pinned compiler realpath and exactly one standalone `-static`, `-static-libgcc`, and `-static-libstdc++`, and reject response files plus direct dynamic-link modes. For this target, reject `-Wl` and `-Xlinker` entirely rather than normalizing every forwarding grammar; this rejects `-Wl,-shared` and `-Xlinker -B -Xlinker dynamic` without a general linker-option parser. The checker uses explicit nonzero failures, not Python `assert`.

Freeze only the directly relevant synthetic cases: source/output path escape, mismatched compile metadata, repeated `-c`, wrong compiler, observer macro via `-Wp`/`-Xpreprocessor`, joined `-include`/`-imacros` override, alternate standard or optimization, wrong link-file path/content, missing/duplicate required static flag, `-Wl,-shared`, and the split `-Xlinker` dynamic override. For both literal checker entrypoints, test one complete accepted invocation and a missing-operand rejection, in normal Python and `PYTHONOPTIMIZE=1`; test the two joined forced-include negatives in the same modes. No real D2 configure/build/run is implied by these synthetic tests.

5. Bounded run-record and dual-mode finalizer contract: `tests/spec039/q0_q1_d2.manifest.json` defines strict Draft 2020-12 schemas for the pending/final run record v5, `supervisor-termination.json` receipt v1, IPC inventories, observer events and `reservation-journal.jsonl` events v2. The finalized run record's `finalization_mode` is exactly `normal_cleanup` or `abnormal_evidence_only`; pending records use null, and the finalizer derives the mode from journal and supervisor evidence rather than a caller label. The same allowlisted `--finalize` binary is the sole semantic authority. Both finalization modes require a strictly valid pending run record and supervisor receipt; every recorded PID reaped; closed stdout/stderr; unchanged execution/result-directory identities; and a dedicated-UID lease continuously held through final-record closure. If any common prerequisite is absent, ambiguous or lost, do not invoke/re-enter the finalizer or write a final record; retain pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. In `normal_cleanup`, strictly parse/replay all required JSON/JSONL, verify raw `/proc/sysvipc` hashes/rows, exact artifact set, wait/log receipt, lease and semaphore mapping, then inspect exact identities and perform only journal-authorised cleanup after `CLEANUP_INTENT` fsync and immediate identity/lease recheck. `RUN_SHUTDOWN_COMPLETE` must be durable after joins/destruction and before any cleanup intent. PASS/FAIL require the full normal evidence and inventory gates. If cleanup fails after an intent, stop further destructive calls/retries and derive the historical `cleanup_outcome` from §7.2: `partial` requires full created-resource terminal coverage plus a proven `CLEANUP_FAILED`; an uncovered or ambiguous resource is `unknown`. A complete closed journal may retain `cleanup_outcome=complete` if a later inventory/evidence/final-record check fails, but overall status is `INCONCLUSIVE` unless all normal acceptance gates pass. If the final record cannot safely be written, leave pending recovery evidence.

In `abnormal_evidence_only`, normal shutdown proof may be absent, but all common prerequisites still hold. Strictly validate the pending record and supervisor receipt; for other allowlisted evidence files, use no-follow regular-file checks, preserve/hash safely readable closed raw bytes within bounds, but do not require the full bundle or parse/replay inventory, observer or other non-control schemas. The reservation journal is subject only to the bounded read-only cleanup-history exception stated below, not to normal-bundle or inventory validation. Malformed but safely readable non-control evidence other than the reservation journal is preserved and hashed, not parsed or used to block a final `INCONCLUSIVE` record. The only parsing/replay exception is bounded read-only parsing/replay of `reservation-journal.jsonl` solely to derive `failure.cleanup_outcome` under `COUT-001`–`COUT-004`; invalid, unavailable, malformed, truncated or incomplete history yields `unknown` and does not block an otherwise authorized abnormal `INCONCLUSIVE` record. The bounded journal-history classifier validates event shape, sequence, phase/resource transitions and the stable resource-identity binding defined by §7.2; it is not a full runtime finalizer. `failure.cleanup_outcome` is derived only by the §7.2 `COUT-001`–`COUT-004` rules. `failure.missing_artifacts` is exactly the required evidence names actually absent; `failure.unavailable_artifacts` records required names present but unsafe, unreadable or over-cap. The lists are disjoint; neither includes the record itself or optional `harness-diagnostics.txt`. Bound raw-byte reads by the per-file and 32 MiB total caps; classify over-budget files unavailable instead of aborting final-record creation. Present post/final files may be preserved but their inventory summaries remain unavailable, never empty. This mode always yields `INCONCLUSIVE`, performs no IPC inspection, cleanup or journal mutation, and never retries. A cleanup intent or prior removal is reported truthfully, not rewritten as `not_started`.

The bounded semantic regression set must reject claimed PASS/FAIL for missing/unverified launcher or lease, a second/foreign process, diagnostic `sem_open` failure calling `sem_unlink`, pre-existing candidate keys/names, unreadable/over-cap/raw-parsed-mismatched normal inventories, noncanonical base64, missing/reordered/duplicated/unfsynced normal journal events, candidate identity/mode mismatch, missing `sem_t` mapping, residual candidates, unexpected host changes, or final-inventory mismatch. Reject normal cleanup without a durable `RUN_SHUTDOWN_COMPLETE`, complete supervisor wait receipts, closed logs and continuous lease. Reject invocation of either mode before all recorded PIDs are reaped and logs closed; when common launcher/receipt/lease/result-directory authority is absent, retain pending evidence and report `BLOCKED/INCONCLUSIVE`. Abnormal fixtures must accept malformed but safely readable non-control evidence as raw preserved/hashable bytes without requiring a complete bundle or parsing it; reject claims of PASS/FAIL, empty inventories, joins/destruction, optional diagnostics in `missing_artifacts`, overlap between `missing_artifacts` and `unavailable_artifacts`, or an inaccurate cleanup-history outcome. Journal-history fixtures follow §7.2: `not_started` requires a complete transition-valid closed journal with zero `CREATED` and zero `CLEANUP_INTENT` (a `CREATE_FAILED` may occur); `partial` requires one terminal cleanup outcome for every created resource and at least one `CLEANUP_FAILED`; `complete` requires every created resource exactly one `REMOVED` and no cleanup failure but never implies PASS; all other histories are `unknown`. Include schema-valid negatives for orphan `REMOVED` and `CLEANUP_FAILED` without cleanup intent, cleanup identity mismatch, pending intent, created-without-cleanup and one resource left uncovered while another is cleaned. Abnormal mode must never retry cleanup or mutate journal/IPC. The finalizer must perform no speculative deletion for disputed resources. Normal PASS/FAIL requires a complete, replayable journal and every required evidence artifact; the normal bundle has eight evidence artifacts plus `run-record.json`, optional `harness-diagnostics.txt`, and a ten-file maximum. Normal artifacts are each ≤4 MiB, the journal ≤256 KiB/128 events, the final run record ≤256 KiB, and the complete normal bundle ≤32 MiB. Abnormal raw preservation remains no-follow and bounded: per-file/total read caps apply, but excess/unsafe present files are recorded as unavailable rather than making strict schema/completeness validation a prerequisite to the `INCONCLUSIVE` record.

The host-controlled runtime shape remains one controlled D2 process tree under the dedicated UID, one Q0/Q1 correctness run, no network input and no performance timing. The launcher serializes the run and finalizer under the verified exclusive lease; the harness itself spawns no child process. The host IPC namespace may contain unrelated objects, but all candidate resources must be absent before reservation and all exact cleanup remains bound to the durable journal. No broad host cleanup is permitted. If the launcher/lease or result-directory identity is absent/lost, a second process enters the identity domain, or supervisor evidence is incomplete, do not perform name-based unlink/RMID or invoke/re-enter finalization; preserve pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. An abnormal record may be written only while common lease and file-safety authority remains valid.

**Next gate:** complete local v1.16/r14 verification of the policy model, all duplicate SPEC/manifest/packet projections, and the read-only checker; record exact hashes and evidence classes. The r13 one-call consent is consumed, and this amendment does not authorize another Astra call. Any later review requires a separate owner decision on the final exact packet SHA. D2 implementation/configure/build/run remains blocked until a fresh positive documentary review, provisioned launcher/preconditions and separate owner execution gate. D3 and Q2–Q4 remain deferred. This work is documentary/synthetic, not finalizer runtime, correctness or performance evidence.

### 7.2 Canonical lifecycle and outcome model — v1.16

The following versioned JSON object is the single normative model for journal transitions, resource identity binding, finalizer-mode eligibility and cleanup-history classification. The manifest's `classification_rules.journal_transition_model` and the review packet's marked model block must be structurally identical to this block; the read-only checker verifies those projections and related schema enums. Human-readable requirements use the stable IDs below. This is an offline contract model, not an operational finalizer or runtime evidence.

<!-- NG039-JOURNAL-MODEL-BEGIN -->
```json
{
  "model_id": "ng039-journal-finalization-model",
  "model_version": 2,
  "schema_version": "ng039-reservation-journal-event/v2",
  "journal_states": [
    "absent",
    "open",
    "shutdown_complete",
    "closed"
  ],
  "journal_initial_state": "absent",
  "journal_terminal_state": "closed",
  "resource_states": [
    "none",
    "create_pending",
    "create_failed",
    "created",
    "cleanup_pending",
    "removed",
    "cleanup_failed"
  ],
  "resource_initial_state": "none",
  "resource_kinds": {
    "journal": "journal",
    "candidates": [
      "sysv_shm",
      "posix_semaphore"
    ]
  },
  "identity_binding": {
    "sysv_shm": [
      "candidate_key",
      "shmid"
    ],
    "posix_semaphore": [
      "source_name",
      "device",
      "inode"
    ]
  },
  "required_events": [
    "JOURNAL_OPENED",
    "CREATE_INTENT",
    "CREATED",
    "CREATE_FAILED",
    "RUN_SHUTDOWN_COMPLETE",
    "CLEANUP_INTENT",
    "REMOVED",
    "CLEANUP_FAILED",
    "JOURNAL_CLOSED"
  ],
  "events": [
    {
      "requirement_id": "JRN-001",
      "event": "JOURNAL_OPENED",
      "scope": "journal",
      "phase_from": [
        "absent"
      ],
      "phase_to": "open",
      "resource_from": null,
      "resource_to": null
    },
    {
      "requirement_id": "JRN-002",
      "event": "CREATE_INTENT",
      "scope": "resource",
      "phase_from": [
        "open"
      ],
      "phase_to": "open",
      "resource_from": "none",
      "resource_to": "create_pending"
    },
    {
      "requirement_id": "JRN-003",
      "event": "CREATED",
      "scope": "resource",
      "phase_from": [
        "open"
      ],
      "phase_to": "open",
      "resource_from": "create_pending",
      "resource_to": "created"
    },
    {
      "requirement_id": "JRN-004",
      "event": "CREATE_FAILED",
      "scope": "resource",
      "phase_from": [
        "open"
      ],
      "phase_to": "open",
      "resource_from": "create_pending",
      "resource_to": "create_failed"
    },
    {
      "requirement_id": "JRN-005",
      "event": "RUN_SHUTDOWN_COMPLETE",
      "scope": "journal",
      "phase_from": [
        "open"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": null,
      "resource_to": null,
      "requires_no_resource_states": [
        "create_pending"
      ]
    },
    {
      "requirement_id": "JRN-006",
      "event": "CLEANUP_INTENT",
      "scope": "resource",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": "created",
      "resource_to": "cleanup_pending"
    },
    {
      "requirement_id": "JRN-007",
      "event": "REMOVED",
      "scope": "resource",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": "cleanup_pending",
      "resource_to": "removed"
    },
    {
      "requirement_id": "JRN-008",
      "event": "CLEANUP_FAILED",
      "scope": "resource",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": "cleanup_pending",
      "resource_to": "cleanup_failed"
    },
    {
      "requirement_id": "JRN-009",
      "event": "JOURNAL_CLOSED",
      "scope": "journal",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "closed",
      "resource_from": null,
      "resource_to": null,
      "requires_no_resource_states": [
        "create_pending",
        "cleanup_pending"
      ]
    }
  ],
  "authority_modes": {
    "requirement_ids": {
      "common": "AUTH-001",
      "normal": "AUTH-002",
      "abnormal": "AUTH-003",
      "blocked": "AUTH-004",
      "abnormal_journal_history": "AUTH-005"
    },
    "common_requirements": [
      "trusted_launcher",
      "valid_supervisor_receipt",
      "all_pids_reaped",
      "logs_closed",
      "stable_result_directory",
      "continuous_lease"
    ],
    "normal_requirements": [
      "durable_shutdown_complete",
      "normal_waits_valid",
      "no_timeout_or_signal"
    ],
    "mode_names": {
      "normal": "normal_cleanup",
      "abnormal": "abnormal_evidence_only",
      "blocked": "blocked"
    }
  },
  "mode_effects": {
    "normal_cleanup": {
      "requirement_id": "AUTH-002",
      "status": [
        "PASS",
        "FAIL",
        "INCONCLUSIVE"
      ],
      "requires": [
        "full_bundle_schema_journal_replay",
        "complete_inventory_verification",
        "exact_journal_authorized_cleanup"
      ],
      "forbidden": [
        "cleanup_before_shutdown_complete",
        "broad_host_cleanup"
      ]
    },
    "abnormal_evidence_only": {
      "requirement_id": "AUTH-003",
      "status": [
        "INCONCLUSIVE"
      ],
      "requires": [
        "common_authority_gates_true",
        "strict_pending_record_supervisor_receipt",
        "bounded_raw_evidence_preservation",
        "attempt_bounded_read_only_journal_history_classification"
      ],
      "forbidden": [
        "live_ipc_inspection",
        "cleanup",
        "journal_mutation",
        "retry",
        "PASS",
        "FAIL"
      ],
      "journal_history_policy": {
        "requirement_id": "AUTH-005",
        "artifact": "reservation-journal.jsonl",
        "access": "bounded_read_only_parse_replay",
        "purpose": "derive_cleanup_outcome_only",
        "outcome_requirement_ids": [
          "COUT-001",
          "COUT-002",
          "COUT-003",
          "COUT-004"
        ],
        "invalid_or_unavailable_history": "unknown",
        "unknown_history_blocks_authorized_inconclusive_record": false,
        "strict_pending_and_supervisor_records": true,
        "other_non_control_artifacts": "bounded_raw_preservation_without_schema_parse_or_replay",
        "normal_bundle_schema_inventory_gate_required": false,
        "journal_mutation": false
      }
    },
    "blocked": {
      "requirement_id": "AUTH-004",
      "status": [
        "BLOCKED",
        "INCONCLUSIVE"
      ],
      "requires": [
        "preserve_pending_evidence"
      ],
      "forbidden": [
        "finalizer_entry",
        "finalizer_reentry",
        "cleanup",
        "unsafe_record_write"
      ]
    }
  },
  "cleanup_outcomes": {
    "not_started": {
      "requirement_id": "COUT-001",
      "journal_required": "complete_transition_valid_closed",
      "created_count": 0,
      "cleanup_intent_count": 0,
      "create_failed_allowed": true,
      "run_status_effect": "NO_PASS_INFERENCE"
    },
    "partial": {
      "requirement_id": "COUT-002",
      "journal_required": "complete_transition_valid_closed",
      "all_created_have_one_terminal_outcome": true,
      "cleanup_failed_count_min": 1,
      "run_status_effect": "INCONCLUSIVE"
    },
    "complete": {
      "requirement_id": "COUT-003",
      "journal_required": "complete_transition_valid_closed",
      "created_count_min": 1,
      "all_created_removed": true,
      "cleanup_failed_count": 0,
      "run_status_effect": "DOES_NOT_IMPLY_PASS"
    },
    "unknown": {
      "requirement_id": "COUT-004",
      "journal_required": "otherwise",
      "run_status_effect": "INCONCLUSIVE"
    }
  },
  "normal_cleanup_failure": {
    "requirement_id": "FIN-001",
    "stop_destructive_calls_after_failure": true,
    "retry_after_intent": false,
    "outcome_source": "complete_transition_valid_closed_journal",
    "complete_history_after_journal_closed_later_evidence_failure": {
      "cleanup_outcome": "complete",
      "run_status": "INCONCLUSIVE"
    },
    "authority_loss": {
      "finalizer_entry": false,
      "finalizer_reentry": false,
      "preserve_pending": true,
      "status": "BLOCKED/INCONCLUSIVE"
    }
  }
}
```
<!-- NG039-JOURNAL-MODEL-END -->

`JRN-002` and `JRN-006` deliberately use separate pending states. `JRN-003`/`JRN-004` can consume only a create intent; `JRN-007`/`JRN-008` can consume only a cleanup intent after a successful create. `JRN-005` rejects unresolved create intents; `JRN-009` rejects unresolved intents but may close with an uncleaned `created` resource, which `COUT-004` classifies as `unknown`. For identity continuity, `CREATED`, `CLEANUP_INTENT` and the terminal cleanup event must carry the same stable binding: SysV SHM uses `(candidate_key, shmid)`; a POSIX semaphore uses `(source_name, device, inode)`. Other identity attributes (owner/mode/libc/mapping) remain separate required checks in the normal finalizer; these model bindings do not replace them.

`AUTH-001` is a common gate for both finalizer modes. If any common launcher, receipt, reaping, log-closure, result-directory or continuous-lease requirement is false or unknown, choose `blocked`, do not invoke/re-enter the finalizer, and preserve pending evidence. With common authority intact, `AUTH-002` selects normal cleanup only when every normal gate is true; otherwise `AUTH-003` permits bounded abnormal evidence preservation only, ending `INCONCLUSIVE` with no IPC inspection, cleanup, journal mutation or retry. `COUT-001` through `COUT-004` describe cleanup history, not overall acceptance. `FIN-001` requires stopping destructive actions and retries after failure; a complete closed history remains `complete` if a later evidence/final-record step fails, while run status remains `INCONCLUSIVE`.

## 8. Revision history

| Version | Date | Change |
|---|---|---|
| v1.1 | 2026-10-04 | Owner-approved documentary D0 contract for Q0/Q1 only. |
| v1.2 | 2026-10-04 | Recorded the separately bounded D2 seam/correctness scope, D1-H compile/link result, and pre-code Astra checkpoint; the exact-SHA review returned HOLD. No D2 code/runtime followed. |
| v1.3 | 2026-10-05 | First local draft of the same-SPEC documentary closure requirements; superseded by v1.4 after a source-grounded HT construction prerequisite was identified. |
| v1.4 | 2026-10-05 | Same-SPEC closure draft with HT-before-GW prerequisite. The exact pre-code packet SHA `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948` was reviewed by GPT-6 Astra and returned AMEND. No D2 code/runtime followed. |
| v1.5 | 2026-10-05 | Same-SPEC documentary closure response to the v1.4 AMEND. Sanitized r03 packet SHA `7121fa9307beb790ff093319d6d9bc54fb9d296bf68d8c705647fbcebf740797` was reviewed by GPT-6 Astra and returned AMEND. No D2 code/runtime followed. |
| v1.6 | 2026-10-05 | Same-SPEC response to the r03 AMEND. Sanitized r04 packet SHA `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829` returned AMEND: findings 3 and 4b–4d closed at design level; finding 4a remained must-fix. No D2 code/runtime followed. |
| v1.7 | 2026-10-05 | Same-SPEC documentary response to r04; sanitized r05 packet returned AMEND with three 4a must-fixes. No D2 code/runtime followed. |
| v1.8 | 2026-10-05 | Same-SPEC response to r05: supported compiler-argument grammar, forwarded/function-like macro rejection, `-ansi` rejection, output-derived target identity, assertion-independent exact link checks, dynamic-link conflict rejection, and schema-level PASS/failure conditions. Local/unreviewed; fresh exact-SHA consent is required for r06; D2/D3 remain blocked. |
| v1.9 | 2026-10-10 | Editorial privacy-only redaction of the workstation-specific D1-H checkout path to a stable role alias; no design, scope, or gate change. |
| v1.10 | 2026-10-10 | Owner-directed, same-SPEC response to r06 AMEND. Keeps the source/output containment, exact `-c` count, minimal `env -i`, actual `link.txt` binding and no-false-PASS contract; relaxes generic argument grammar, exhaustive environment denylisting and duplicated schema/finalizer rules. The frozen B1/B2/B3 r07 packet later received documentary `APPROVE` only. |
| v1.11 | 2026-10-11 | Owner-directed replacement of the unavailable user/IPC/mount/PID namespace sandbox by host-controlled candidate-resource attribution, 0600 exclusive reservations, recorded libc semaphore-name mapping, pre/post/final inventory equality and fail-closed identity-checked cleanup. No AppArmor exception or global security change. |
| v1.12 | 2026-10-12 | Same-SPEC documentary response to r08 B2/B3: require a dedicated single-run UID/launcher or remain blocked; suppress diagnostic `sem_unlink` after all three `sem_open` failure paths; add fsynced reservation journal, retained raw `/proc/sysvipc` bytes/recomputation and evidence binding the returned `sem_t` mapping to the exact inode. Astra r09 substantially accepted B2/B3 but returned AMEND on contradictory finalizer IPC scope; no implementation/runtime authorization. |
| v1.13 | 2026-10-12 | Same-SPEC response to the sole r09 B1: reconcile the finalizer's responsibility by allowing only bounded inventory/current-identity inspection and journal-authorized exact `IPC_RMID`/`sem_unlink` under the continuous dedicated-UID lease, while forbidding Process/GW construction, resource creation, production message traffic, ambiguous retries and broad cleanup. Astra r10 resolved the IPC boundary and preserved B2/B3, but returned AMEND on abnormal finalization. No implementation/runtime authorization. |
| v1.14 | 2026-10-12 | Same-SPEC response to r10 abnormal-finalization B1: adds durable `RUN_SHUTDOWN_COMPLETE`, supervisor PID wait/log receipts, run-record v5 and journal-event v2. The r11 documentary review returned AMEND on lease-loss eligibility and truthful cleanup history after a cleanup attempt. The r12 amendment clarifies common lease/file-authority gates, separates normal full-schema validation from abnormal bounded raw preservation, adds missing/unavailable artifact sets and evidence-derived cleanup outcomes, and forbids finalizer re-entry or cleanup retries. No implementation/runtime authority. |
| v1.15 | 2026-10-13 | Owner-authorized same-SPEC methodology correction to Astra r12 B1/B2: adds canonical versioned journal/mode/outcome model, distinct create/cleanup states, exact stable identity binding, structured projection requirements and a read-only checker contract. Historical `complete` cleanup remains separate from overall `INCONCLUSIVE` status. r12 artifacts remain immutable. No Astra follow-up or operational authority granted. |
| v1.16 | 2026-10-13 | Owner-authorized same-SPEC response to Astra r13 AMEND: defines bounded, read-only journal-history parsing/replay solely for COUT-001–004 in `abnormal_evidence_only`; malformed, unavailable, truncated or incomplete history is `unknown` without blocking an otherwise authorized `INCONCLUSIVE` record. Other non-control evidence remains raw-only; mutation, live IPC inspection, cleanup and retry remain forbidden. Model v2 and local projection checks are unreviewed; no Astra follow-up or operational authority granted. |


</details>

## 7. Pinned functional-baseline source excerpts

The following excerpts are exact text from the isolated functional baseline. Each heading binds the SHA-256 of the complete source file; line ranges are shown for review. CMake’s D1-H-only overlay is separated below and is not a D2 change.

### CMakeLists.txt

`<D2_WORKTREE>/CMakeLists.txt` — complete-file SHA-256 `d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d`

#### Lines 1–20

~~~cmake
cmake_minimum_required(VERSION 3.5.1)
project(novagenesis)

set(CMAKE_CXX_STANDARD 20)

option(NG_ENABLE_LEGACY_STANDALONE
       "Build deprecated standalone PSS, GIRS and HTS services"
       OFF)

option(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS
       "Build NG-039 Gateway compile/link probe only (no runtime execution)"
       OFF)

if(NG_ENABLE_LEGACY_STANDALONE)
    message(WARNING
            "NG_ENABLE_LEGACY_STANDALONE=ON: PSS, GIRS and HTS are deprecated; use NRNCS")
endif()

include_directories(ContentApp/src)
include_directories(Common/src)
~~~

#### Lines 29–157

~~~cmake
add_library(Common
        Common/src/rapidjson/error/en.h
        Common/src/rapidjson/error/error.h
        Common/src/rapidjson/internal/biginteger.h
        Common/src/rapidjson/internal/diyfp.h
        Common/src/rapidjson/internal/dtoa.h
        Common/src/rapidjson/internal/ieee754.h
        Common/src/rapidjson/internal/itoa.h
        Common/src/rapidjson/internal/meta.h
        Common/src/rapidjson/internal/pow10.h
        Common/src/rapidjson/internal/stack.h
        Common/src/rapidjson/internal/strfunc.h
        Common/src/rapidjson/internal/strtod.h
        Common/src/rapidjson/internal/swap.h
        Common/src/rapidjson/msinttypes/inttypes.h
        Common/src/rapidjson/msinttypes/stdint.h
        Common/src/rapidjson/allocators.h
        Common/src/rapidjson/document.h
        Common/src/rapidjson/encodedstream.h
        Common/src/rapidjson/encodings.h
        Common/src/rapidjson/filereadstream.h
        Common/src/rapidjson/filewritestream.h
        Common/src/rapidjson/memorybuffer.h
        Common/src/rapidjson/memorystream.h
        Common/src/rapidjson/pointer.h
        Common/src/rapidjson/prettywriter.h
        Common/src/rapidjson/rapidjson.h
        Common/src/rapidjson/reader.h
        Common/src/rapidjson/stringbuffer.h
        Common/src/rapidjson/writer.h
        Common/src/Action.cpp
        Common/src/Action.h
        Common/src/Block.cpp
        Common/src/Block.h
        Common/src/CLI.cpp
        Common/src/CLI.h
        Common/src/CLIRunInitialization01.cpp
        Common/src/CLIRunInitialization01.h
        Common/src/CLISCNAck01.cpp
        Common/src/CLISCNAck01.h
        Common/src/CommandLine.cpp
        Common/src/CommandLine.h
        Common/src/DataStore.h
        Common/src/fast_mutex.h
        Common/src/File.cpp
        Common/src/File.h
        Common/src/GW.cpp
        Common/src/GW.h
        Common/src/GWHelloIPC02.cpp
        Common/src/GWHelloIPC02.h
        Common/src/GWMsgCl01.cpp
        Common/src/GWMsgCl01.h
        Common/src/GWRunInitialization01.cpp
        Common/src/GWRunInitialization01.h
        Common/src/GWRunHelloIPC02.cpp
        Common/src/GWRunHelloIPC02.h
        Common/src/GWExposition02.cpp
        Common/src/GWExposition02.h
        Common/src/HashMultimaps.h
        Common/src/HT.cpp
        Common/src/HT.h
        Common/src/HTBindReport01.cpp
        Common/src/HTBindReport01.h
        Common/src/HTDeliveryBind01.cpp
        Common/src/HTDeliveryBind01.h
        Common/src/HTGetBind01.cpp
        Common/src/HTGetBind01.h
        Common/src/HTInfoPayload01.cpp
        Common/src/HTInfoPayload01.h
        Common/src/HTListBind01.cpp
        Common/src/HTListBind01.h
        Common/src/HTMessageSeq01.cpp
        Common/src/HTMessageSeq01.h
        Common/src/HTMsgCl01.cpp
        Common/src/HTMsgCl01.h
        Common/src/HTRevokeBind01.cpp
        Common/src/HTRevokeBind01.h
        Common/src/HTRunInitialization01.cpp
        Common/src/HTRunInitialization01.h
        Common/src/HTRunPeriodic01.cpp
        Common/src/HTRunPeriodic01.h
        Common/src/HTSCNSeq01.cpp
        Common/src/HTSCNSeq01.h
        Common/src/HTStatusS01.cpp
        Common/src/HTStatusS01.h
        Common/src/HTStoreBind01.cpp
        Common/src/HTStoreBind01.h
        Common/src/Message.cpp
        Common/src/Message.h
        Common/src/MessageBuilder.cpp
        Common/src/MessageBuilder.h
        Common/src/MessageReceiving.h
        Common/src/MurmurHash3.cpp
        Common/src/MurmurHash3.h
        Common/src/MurmurHash3Strategy.cpp
        Common/src/MurmurHash3Strategy.h
        Common/src/NameGenerator.cpp
        Common/src/NameGenerator.h
        Common/src/NGAL_SAR.cpp
        Common/src/NGAL_SAR.h
        Common/src/NGAL_CS.cpp
        Common/src/NGAL_CS.h
        Common/src/NameStrategy.h
        Common/src/DefaultNameStrategy.cpp
        Common/src/DefaultNameStrategy.h
        Common/src/HashFunction.h
        Common/src/OutputVariable.cpp
        Common/src/OutputVariable.h
        Common/src/Process.cpp
        Common/src/Process.h
        Common/src/Prompt_iostream.cpp
        Common/src/Prompt_iostream.h
        Common/src/Publication.h
        Common/src/Shm.h
        Common/src/Subscription.h
        Common/src/tinythread.cpp
        Common/src/tinythread.h
        Common/src/Tuple.h)

if(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS)
    add_executable(ng039_gw_q0_q1_harness
            tests/spec039/gw_q0_q1_harness.cpp)
    target_link_libraries(ng039_gw_q0_q1_harness LINK_PRIVATE Common)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++") # SPEC-MUSL-001: musl static link fix (cherry-pick e714ab1)

add_executable(ContentApp
~~~

### Common/src/GW.cpp

`<D2_WORKTREE>/Common/src/GW.cpp` — complete-file SHA-256 `7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e`

#### Lines 90–128

~~~cpp
  // SPEC-006: Named semaphore "Output_Queue" removed from constructor.
  // OutputQueueMutex alone protects OutputQueues.

  Action* PA = 0;
  CommandLine* PCL = 0;
  Block* PHTB = 0;
  Message* PIM = 0;
  Message* InlineResponseMessage = NULL;

  PP->GetBlock("HT", PHTB);

  PHT = (HT*)PHTB;

  // Creating the actions
  NewAction("-run --initialization 0.1", PA);
  NewAction("-m --cl 0.1", PA);
  NewAction("-hello --ipc 0.2", PA);      // GWHelloIPC02 - receiver (replaces GWHelloIPC01)
  NewAction("-hello --ipc 2.0", PA);      // GWHelloIPC02 - receiver with HT BID support
  NewAction("-run --helloIPC 0.2", PA);   // GWRunHelloIPC02 - periodic emitter
  NewAction("-exposition 0.2", PA);       // GWExposition02 - periodic peer exposition on PGCS
  NewAction("-run --exposition 0.2", PA); // GWExposition02 - periodic trigger

  // Creating a -run --initialization message
  int PIMStatus = PP->NewMessage(GetTime(), 0, false, PIM);

  // Adding only the run initialization command line and executing it when the
  // message allocation succeeded. On allocation failure, leave the gateway
  // constructible without inspecting or marking an unowned output.
  if (PIMStatus == OK && PIM != 0)
  {
    PIM->NewCommandLine("-run", "--initialization", "0.1", PCL);

    // Run
    Run(PIM, InlineResponseMessage);

    // Mark to delete
    PIM->MarkToDelete();
  }
}
~~~

#### Lines 218–276

~~~cpp
// Be aware to lock Input queue mutex (IQM) before accessing this function
void GW::PushToInputQueue(Message* M)
{
  unsigned int NoCL = 0;

  if (M == 0)
  {
    S << "          (ERROR: The message being store in input queue is corrupted at input queue)" << endl;
    return;
  }

#ifdef DEBUG

  S << "[1]       (Pushing the following message to InputQueue. Size = " << InputQueue.size()
    << ". Message instantiation number is " << M->InstantiationNumber << ")" << endl;

  S << "(" << endl
    << *M << ")" << endl;

#endif

  if (M != 0)
  {
    if (M->GetNumberofCommandLines(NoCL) == OK)
    {
      if (NoCL > 2)
      {
        // Avoid deleting the message
        M->UnmarkToDelete();

        // Set the message tag
        M->SetTag(InputQueueTag);

        // Lock for push + notify (thread-safe w.r.t. Gateway() pop)
        {
          std::lock_guard<std::mutex> lock(InputQueueMutex);

          // Push the message to the queue
          InputQueue.push(M);

          // Increases the tag counter
          InputQueueTag++;
        }
        // Notify after unlock to minimize time input thread is blocked
        InputQueueCV.notify_one();
      }
      else
      {
        // Mark to delete the message
        M->MarkToDelete();
      }
    }
    else
    {
      S << "          (ERROR: Unable to read the number of command lines at input queue)" << endl;
      M->MarkToDelete();
    }
  }
}
~~~

#### Lines 451–503

~~~cpp
  while (StopGateway == false)
  {
    // Calculate how long to wait: time until next scheduled message
    {
      std::lock_guard<std::mutex> lock(InputQueueMutex);
      if (!InputQueue.empty())
      {
        double nextTime = InputQueue.top()->GetTime();
        double now = GetTime();
        secondsUntilNext = nextTime - now;
        if (secondsUntilNext < 0)
          secondsUntilNext = 0;
      }
      else
      {
        secondsUntilNext = 1.0; // no messages, check SHM every 1s max
      }
    }

    // Wait for input queue or stop flag (timer-aware blocking wait)
    {
      std::unique_lock<std::mutex> lock(InputQueueMutex);
      waitTimeout = std::chrono::milliseconds(
          std::min((long long)(secondsUntilNext * 1000), SHM_POLL_INTERVAL_MS));
      InputQueueCV.wait_for(lock, waitTimeout,
                            [this]()
                            { return !InputQueue.empty() || StopGateway; });
      if (StopGateway)
        break;
    }

    // Lock, pop all due messages, unlock — minimize critical section
    PM1 = NULL;
    RunFlag = false;
    Time = GetTime();

    {
      std::lock_guard<std::mutex> lock(InputQueueMutex);
      if (!InputQueue.empty())
      {
        PM1 = InputQueue.top();
        ScheduledTime = PM1->GetTime();
        if (ScheduledTime < Time)
        {
          InputQueue.pop();
          RunFlag = true;
        }
        else
        {
          PM1 = NULL; // not yet due, leave in queue
        }
      }
    }
~~~

#### Lines 505–597

~~~cpp
    // Step 2 : Run procedure to interpret and run the received message
    if (PM1 != NULL && RunFlag == true)
    {

#ifdef DEBUG

      S << endl
        << setprecision(10) << "          (t = " << Time << ")" << endl;
      S << "[1]       (IQ Size = " << InputQueue.size() << ")" << endl;

      map<std::string, priority_queue<Message*, vector<Message*>, DereferenceCompareNode>>::iterator it;
      unsigned int Counter = 0;
      for (it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        Counter++;
        S << "[6]       (OQ[" << Counter << "] Size = " << it->second.size() << ")" << endl;
      }
      S << Offset << "(Messages in memory = " << PP->GetNumberOfMessages() << ")" << endl;

      S << "[7]       Current peer processes on this OS are:" << endl;

      // List peers from category 20 (hello IPC discovered peers)

      vector<string> Cat20Keys;
      if (PHT->GetBindingKeys(20, Cat20Keys) == OK)
      {
        for (unsigned int i = 0; i < Cat20Keys.size(); i++)
        {
          vector<string>* Cat20Vals = new vector<string>;
          if (PHT->GetBinding(20, Cat20Keys.at(i), Cat20Vals) == OK)
          {
            S << "            Cat[20] " << Cat20Keys.at(i);
            for (unsigned int j = 0; j < Cat20Vals->size(); j++)
            {
              S << " -> " << Cat20Vals->at(j);
            }
            S << endl;
          }
          delete Cat20Vals;
        }
      }

      // List peers from category 19 (hello IPC discovered peers)

      vector<string> Cat19Keys;
      if (PHT->GetBindingKeys(19, Cat19Keys) == OK)
      {
        for (unsigned int j = 0; j < Cat19Keys.size(); j++)
        {
          vector<string>* Cat19Vals = new vector<string>;
          if (PHT->GetBinding(19, Cat19Keys.at(j), Cat19Vals) == OK)
          {
            S << "            Cat[19] " << Cat19Keys.at(j);
            for (unsigned int k = 0; k < Cat19Vals->size(); k++)
            {
              S << " -> " << Cat19Vals->at(k);
            }
            S << endl;
          }
          delete Cat19Vals;
        }
      }

      // List peers from category 15 (hello IHC discovered peers)

      vector<string> Cat15Keys;
      if (PHT->GetBindingKeys(15, Cat15Keys) == OK)
      {
        for (unsigned int j = 0; j < Cat15Keys.size(); j++)
        {
          vector<string>* Cat15Vals = new vector<string>;
          if (PHT->GetBinding(15, Cat15Keys.at(j), Cat15Vals) == OK)
          {
            S << "            Cat[15] " << Cat15Keys.at(j);
            for (unsigned int k = 0; k < Cat15Vals->size(); k++)
            {
              S << " -> " << Cat15Vals->at(k);
            }
            S << endl;
          }
          delete Cat15Vals;
        }
      }

#endif

      Run(PM1, PM2);

      PP->DeleteMarkedMessages();

      PM1 = NULL;
      RunFlag = false;
    }
~~~

#### Lines 614–624

~~~cpp
          Message* PM = NULL;
          if (PP->NewMessage(0, 0, false, PM) == OK)
          {
          #ifdef DEBUG_NETWORK_QUEUE
            cerr << "[DEBUG] GW::Gateway: NETWORK_QUEUE_DELIVERED size=" << size 
                 << " queue_remaining=" << NetworkReceiveQueue.size() << endl;
          #endif
            PM->SetMessageFromCharArray(buffer, size);
            PM->ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();
            PushToInputQueue(PM);
          }
~~~

#### Lines 658–667

~~~cpp
  // Stop the output thread
  if (StopGateway == false)
  {
    StopGateway = true;
    OutputQueueCV.notify_all();
  }

  T->join();
  delete T;
}
~~~

#### Lines 963–971

~~~cpp
      // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)

    } // (mutex == SEM_FAILED) means it was unable to create/open the semaphore
    else
    {
      perror("reading SHM : unable to create/open semaphore");

      sem_unlink(SemaphoreName.c_str());
    }
~~~

#### Lines 1258–1264

~~~cpp
      } //(mutex != SEM_FAILED)
      else
      {
        perror("writing SHM : unable to open/create the semaphore");

        sem_unlink(_oqs.c_str());
      }
~~~

#### Lines 1310–1327

~~~cpp
    else
    {
      perror("shmat");

      S << Offset << "(ERROR: Unable to get the shared memory segment identified by the key = " << _Key << ")"
        << endl;
    }
  }
  else
  {
    perror("While initiating the SHM: unable to create semaphore");

    sem_unlink(SemaphoreName.c_str());

    S << Offset << "(ERROR: Unable to create the semaphore while discovering the peer shmid)" << endl;
  }

  return Status;
~~~

#### Lines 1331–1337

~~~cpp
void GW::SetStopGatewayFlag(bool _F)
{
  StopGateway = _F;
  // Wake up both threads blocked on wait()
  InputQueueCV.notify_all();
  OutputQueueCV.notify_all();
}
~~~

### Common/src/GW.h

`<D2_WORKTREE>/Common/src/GW.h` — complete-file SHA-256 `54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44`

#### Lines 137–182

~~~cpp
struct DereferenceCompareNode : public binary_function<const Message*, const Message*, bool>
{
  bool operator()(const Message* M1, const Message* M2) const
  {
    if (M1->GetTime() != M2->GetTime())
    {
      return M1->GetTime() > M2->GetTime();
    }
    else
    {
      return M1->GetTag() > M2->GetTag();
    }
  }
};

class HT;

class GW : public Block
{
private:
  // ------------------------------------------------------------------------------------------------------------------------------
  // Priority queues
  // ------------------------------------------------------------------------------------------------------------------------------

  // Input message queue
  priority_queue<Message*, vector<Message*>, DereferenceCompareNode> InputQueue;

  // Per key output message queue
  map<std::string, priority_queue<Message*, vector<Message*>, DereferenceCompareNode>> OutputQueues;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary variables
  // ------------------------------------------------------------------------------------------------------------------------------

  // Auxiliary tag for messages stored on input queue
  unsigned int InputQueueTag;

  // Auxiliary tag for messages stored on output queue
  unsigned int OutputQueueTag;

  // Stop gateway flag
  std::atomic<bool> StopGateway;

  // Mutex and condition variable for input queue synchronization
  std::mutex InputQueueMutex;
  std::condition_variable InputQueueCV;
~~~

#### Lines 185–270

~~~cpp
  // Thread-safe intermediate queue for raw char buffers from the NGAL
  // ReceiveDispatcher. The GW thread drains this queue in Gateway().
  // These are public so that NGAL_CS::DeliverToGateway() can access them.
  // Marked as public: but contained within the private: section above.
public:
  std::queue<std::pair<char*, long long>> NetworkReceiveQueue;
  std::mutex NetworkReceiveQueueMutex;
  std::condition_variable NetworkReceiveQueueCV;
  // ⚠ Access returns to private for everything below ⚠
private:

  // Mutex and condition variable for output queue synchronization
  std::mutex OutputQueueMutex;
  std::condition_variable OutputQueueCV;

  // Flag to notify output thread of new messages
  bool NewOutputMessage;

  // Cached semaphores for shared memory IPC
  std::map<std::string, sem_t*> CachedSemaphores;

  // Pointer to the HT block
  HT* PHT;

  // ------------------------------------------------------------------------------------------------------------------------------
  // OS Shared Memory IPC functions
  // ------------------------------------------------------------------------------------------------------------------------------

  int ReadFromSharedMemory3();

  // Write to the shared memory
  int WriteToSharedMemory3(std::string OQS, Message* M);

public:
  // First attachment to learned shared memory (public for PG access)
  int ReturnIPCSHMID(key_t _Key, int& _shmid);

  // Define the maximum segment size on shared memory
  size_t MaxSegmentSize;

  // Schedule status flag
  bool ScheduleStatusFlag;

  // Constructor
  GW(string _LN, Process* _PP, unsigned int _Index, string _Path);

  // Destructor
  ~GW();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Action related functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Allocate and add an Action on Actions container
  void NewAction(const string _LN, Action*& _PA);

  // Get an Action
  int GetAction(string _LN, Action*& _PA);

  // Delete an Action
  int DeleteAction(string _LN);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Core public functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Push a Message into the input message priority queue
  void PushToInputQueue(Message* M);

  // Monitor IPC, read Messages from other processes and pushes to the InputQueue
  // Read Messages from the InputQueue and call the GW::Run(Message *M)
  // Read Messages from the OutputQueue and forward to other processes via IPC
  void Gateway();

  // Push a Message into the output message priority queue. Only the GW can push messages at the output queue
  void PushToOutputQueue(std::string OQS, Message* M);

  // Read a Message from output message priority queues. Only the GW can forward messages to shared memory instances
  void ReadFromOutputQueue();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary functions
  // ------------------------------------------------------------------------------------------------------------------------------

  void SetStopGatewayFlag(bool _F);
~~~

### Common/src/Process.cpp

`<D2_WORKTREE>/Common/src/Process.cpp` — complete-file SHA-256 `bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1`

#### Lines 228–256

~~~cpp
  // Create the process blocks
  NewBlock("HT", PB3);
  NewBlock("GW", PB2);
  // NewBlock("CLI",PB1);
}

Process::~Process()
{
  cout << "Deleting " << LN << endl;

  cout << "Deleting Messages container" << endl;

  DeleteMessages();

  cout << "Done" << endl;

  vector<Block*>::iterator it;
  unsigned int i = 0;

  for (it = Blocks.begin(); it != Blocks.end(); it++)
  {
    delete Blocks[i];

    i++;
  }

  Blocks.clear();

  delete PMB;
~~~

#### Lines 381–405

~~~cpp
  if (_LN == "GW")
  {
    GW* PGW = new GW(_LN, this, 1, Path);

    _PB = (Block*)PGW;

    Blocks.push_back(_PB);

    return OK;
  }

  if (_LN == "HT")
  {
    HT* PDM = new HT(_LN, this, 0, Path);

    _PB = (Block*)PDM;

    Blocks.push_back(_PB);

    return OK;
  }

  return ERROR;
}
~~~

#### Lines 541–577

~~~cpp
// Allocate and add a Message on Messages container
int Process::NewMessage(double _Time, short _Type, bool _HasPayload, Message*& M)
{
  int Status = ERROR;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == FREE)
    {
      Message* PM = new Message(_Time, _Type, _HasPayload);

      M = PM;

      PM->InstantiationNumber = MessageCounter;

      MessageCounter++;

      NoM++;

      Messages[i] = PM;

      Controls[i] = BUSY;

      Status = OK;

#ifdef DEBUG
      cout << "          (Creating a message at " << GetLegibleName() << " with instantiation number " << PM->InstantiationNumber << ". Number of messages is " << NoM << ")" << endl;

      cout << "(Allocated the message with index = " << i << ". Number of messages is " << NoM << ".)" << endl;
#endif

      break;
    }
  }

  return Status;
}
~~~

#### Lines 713–738

~~~cpp
int Process::EraseMessage(Message* M)
{
  int Status = OK;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Messages[i] == M)
    {
      Messages[i] = NULL;

      Controls[i] = FREE;

      NoM--;

#ifdef DEBUG

      cout << "(Erased the message index = " << i << ". The number of messages is " << NoM << ">)" << endl;

#endif

      break;
    }
  }

  return Status;
}
~~~

#### Lines 885–917

~~~cpp
void Process::DeleteMarkedMessages()
{
#ifdef DEBUG
  cout << endl
       << "(------------------------- Deleting marked messages in memory -------------------------)" << endl;
#endif

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == BUSY)
    {
      Message* Temp = Messages[i];

      if (Temp->GetDeleteFlag() == true)
      {

#ifdef DEBUG
        cout << "(The following message with index " << i << " will be deleted.)" << endl;

        cout << "(" << endl
             << *Messages[i] << ")" << endl
             << endl;

#endif

        delete Temp;

        Messages[i] = NULL;

        Controls[i] = FREE; // Frees the position

        NoM--;
      }
~~~

#### Lines 1004–1019

~~~cpp
// Run the Gateway
void Process::RunGateway()
{
  GW* PGW = 0;
  Block* PGWB = 0;

  // Get the GW pointer
  GetBlock("GW", PGWB);

  // Cast to GW pointer
  PGW = (GW*)PGWB;

  // Run the gateway
  // Block main thread execution on this function
  PGW->Gateway();
}
~~~

### Common/src/Message.cpp

`<D2_WORKTREE>/Common/src/Message.cpp` — complete-file SHA-256 `96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518`

#### Lines 37–70

~~~cpp
Message::Message(double _Time, short _Type, bool _HasPayload)
{
  // HeaderFile.SetName("");
  PayloadFile.SetName("");
  // MessageFile.SetName("");

  InstantiationTime = GetSystemTime();

  SCN = "";
  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;

  // HeaderSize=0;
  PayloadSize = 0;
  MessageSize = 0;

  // DeleteHeaderArray=false;
  DeletePayloadArray = false;
  DeleteMessageArray = false;

  // Header=0;
  Payload = 0;
  Msg = 0;

  Delete = false;
  ApplicationDeleted = DELETED_BY_CORE;

  InstantiationNumber = 0;

  NoCL = 0;
  CommandLines = NULL;
}
~~~

#### Lines 1590–1602

~~~cpp
  if (MessageSize == 0 && DeleteMessageArray == false)
  {
    ostringstream oss;

    if (NoCL > 0)
    {
      char* buffer;
      long long length = 0;

      for (unsigned int i = 0; i < NoCL; i++)
      {
        oss << *CommandLines[i];
      }
~~~

### Common/src/Message.h

`<D2_WORKTREE>/Common/src/Message.h` — complete-file SHA-256 `bdbbfee17af8512b2c64e9b61cf83e73b5293cffcd7067c0a10787c80bac4e70`

#### Lines 150–171

~~~cpp

  // Auxiliary flag to enable deletion of *Payload array.
  bool DeletePayloadArray;

  // Auxiliary flag to enable deletion of *Message array.
  bool DeleteMessageArray;

  // Instantiation number to check for double deletion
  unsigned int InstantiationNumber;

  // An application avoid deletion of message by turning this flag true. False means that the core will delete the message automatically
  bool ApplicationDeleted;

  // ******************************************************
  // Constructor/destructor
  // ******************************************************

  // Empty message constructor.
  Message(double _Time, short _Type, bool _HasPayload);

  // Simple constructor.
  Message(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path);
~~~

### Common/src/Block.cpp

`<D2_WORKTREE>/Common/src/Block.cpp` — complete-file SHA-256 `26dfa06b575cea6bb9c8d21a18b894f37e73ad91c7e7a7450fefe517903e1622`

#### Lines 244–313

~~~cpp
                if (C1 == '-' && C2 == '-' && C3 == '-')
                {
                  // Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
                  string LN1(PCL->Name + " " + PCL->Alternative + " " + PCL->Version);

                  // Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
                  string LN2(PCL->Name + " " + PCL->Alternative + " 0.1");

#ifdef DEBUG

                  S << Offset << "(LN1 = " << LN1 << ", LN2 = " << LN2 << ".)" << endl;
#endif

                  for (unsigned int j = 0; j < Actions.size(); j++)
                  {
                    if (Actions[j]->GetLegibleName() == LN1)
                    {
                      PA1 = Actions[j];

#ifdef DEBUG
                      S << Offset << "(Found)" << endl;
#endif

                      break;
                    }
                  }

                  if (PA1 != 0)
                  {
#ifdef DEBUG
                    S << Offset << "(Action is " << PA1->GetLegibleName() << ")" << endl;
#endif

                    // Call the action
                    CLStatus[i] = PA1->Run(_ReceivedMessage, PCL, ScheduledMessages, _InlineResponseMessage);

                    PA1 = 0;

                    PA2 = 0;
                  }
                  else
                  {

#ifdef DEBUG
                    S << Offset << "(Warning: " << LN1 << " not implemented.)" << endl;
#endif

                    for (unsigned int j = 0; j < Actions.size(); j++)
                    {
                      if (Actions[j]->GetLegibleName() == LN2)
                      {
                        PA2 = Actions[j];

                        break;
                      }
                    }
                  }

                  if (PA1 == 0 && PA2 != 0)
                  {
#ifdef DEBUG
                    S << Offset << "(Action is " << PA2->GetLegibleName() << ")" << endl;
#endif

                    // Call the action
                    CLStatus[i] = PA2->Run(_ReceivedMessage, PCL, ScheduledMessages, _InlineResponseMessage);

                    PA2 = 0;
                  }
                }
~~~

### Common/src/GWRunInitialization01.cpp

`<D2_WORKTREE>/Common/src/GWRunInitialization01.cpp` — complete-file SHA-256 `1656b703c892831a05043a8bf82ae1bed73d701c8ffea7b95af5a02df0c978af`

#### Lines 68–119

~~~cpp
  PGW = (GW*)PB;

  PHTB = (Block*)PGW->PHT;

  // Setting up the process SCN as the space limiter
  Limiters.push_back(PB->PP->Intra_Process);

  // Setting up the CLI block SCN as the source SCN
  Sources.push_back(PB->GetSelfCertifyingName());

  // Setting up the HT block SCN as the destination SCN
  Destinations.push_back(PHTB->GetSelfCertifyingName());

  // Creating a new message
  PB->PP->NewMessage(GetTime(), 0, false, StoringInitialBinds);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, StoringInitialBinds, PCL);

  // Block related

  // ******************************************************
  // Binding: PeerPID -> LegibleName (Category 20)
  // ******************************************************

  Category = 20;
  Key = PB->PP->GetSelfCertifyingName();
  Values.push_back(PB->PP->GetLegibleName());
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  PMB->NewStoreBindingCommandLineFromBLNToHashBLN("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBLN("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBID("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToHashBLN("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToBlocksIndex("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBlocksIndexToBID("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPIDToBID("0.1", PB, StoringInitialBinds, PCL);

  // Limiters related

  PMB->NewStoreBindingCommandLineFromLimiterToHashLimiter("0.1", "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToLimiter("0.1", "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN("0.1", "OS", PB->PP->OSSCN, StoringInitialBinds, PCL);
~~~

#### Lines 152–179

~~~cpp
  // ***************************************************************************************
  // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
  // ***************************************************************************************

  if (PB->PP->GetLegibleName() != "PGCS")
  {
    key_t IPCKey = 11;
    int shmid = 0;

    if (PGW->ReturnIPCSHMID(IPCKey, shmid) == OK)
    {
      // Setting up the category
      Category = 17;

      // Setting up the key
      Key = PB->IntToString(IPCKey);

      Values.push_back(PB->IntToString(shmid));

      // Creating the ng -sr --b 0.1 command line
      PMB->NewCommonCommandLine("-sr", "--b", "0.1", Category, Key, &Values, StoringInitialBinds, PCL);
    }
  }

  // Push the message to the GW input queue
  PGW->PushToInputQueue(StoringInitialBinds);

  // ******************************************************
~~~

#### Lines 190–225

~~~cpp
  // Schedule a message to run exposition first time
  // ******************************************************
  if (PB->PP->GetLegibleName() == "PGCS")
  {
    Values.clear();
    Limiters.clear();
    Sources.clear();
    Destinations.clear();

    // Setting up the process SCN as the space limiter
    Limiters.push_back(PB->PP->Intra_Process);

    // Setting up the block SCN as the source SCN
    Sources.push_back(PB->GetSelfCertifyingName());

    // Setting up the block SCN as the destination SCN
    Destinations.push_back(PB->GetSelfCertifyingName());

    // Creating a new message (schedule for next period)
    PB->PP->NewMessage(GetTime() + 1.0, 1, false, RunExposition);

    // Creating the ng -cl -m command line
    PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, RunExposition, PCL);

    // Adding a ng -run --exposition 0.2 command line
    RunExposition->NewCommandLine("-run", "--exposition", "0.2", PCL);

    // Generate the SCN
    SCN = NameGenerator::GetInstance().GenerateFromMessage(RunExposition);

    // Creating the ng -scn --s command line
    PMB->NewSCNCommandLine("0.1", SCN, RunExposition, PCL);

    // Push the message to the GW input queue
    PGW->PushToInputQueue(RunExposition);
  }
~~~

### Preserved D1-H CMake overlay — not D2

Working CMake SHA-256 `d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d`; baseline CMake blob SHA-256 `5f9cd0676b2dcdcd0a500d778f5d5baea312301d6abc382db5eab95f2bbb1d07.

~~~diff
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 809cefb..949afa0 100755
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -7,6 +7,10 @@ option(NG_ENABLE_LEGACY_STANDALONE
        "Build deprecated standalone PSS, GIRS and HTS services"
        OFF)
 
+option(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS
+       "Build NG-039 Gateway compile/link probe only (no runtime execution)"
+       OFF)
+
 if(NG_ENABLE_LEGACY_STANDALONE)
     message(WARNING
             "NG_ENABLE_LEGACY_STANDALONE=ON: PSS, GIRS and HTS are deprecated; use NRNCS")
@@ -141,6 +145,12 @@ add_library(Common
         Common/src/tinythread.h
         Common/src/Tuple.h)
 
+if(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS)
+    add_executable(ng039_gw_q0_q1_harness
+            tests/spec039/gw_q0_q1_harness.cpp)
+    target_link_libraries(ng039_gw_q0_q1_harness LINK_PRIVATE Common)
+endif()
+
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer")
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++") # SPEC-MUSL-001: musl static link fix (cherry-pick e714ab1)
~~~

## 8. Historical D1-H artifacts and limits

D1-H proved compile/link only. Its executable SHA-256 was `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`; it was not executed. The preserved harness and manifest are included for context, not as D2 implementation.

<details>
<summary>D1-H harness source</summary>

~~~cpp
// NG-039 D1-H compile/link probe only.
//
// This translation unit proves that the pinned Common library exports the
// public Gateway entry points named by the Q0/Q1 design. It deliberately does
// not construct Process/GW, start threads, touch IPC, enqueue messages, or
// claim any runtime correctness. The D2 fixture is blocked until queue state
// and message execution can be observed without changing production code.

#include "Process.h"
#include "GW.h"

#include <iostream>
#include <type_traits>

namespace
{
using ProcessRunGateway = void (Process::*)();
using GatewayLoop = void (GW::*)();
using GatewayEnqueue = void (GW::*)(Message*);
using GatewayStop = void (GW::*)(bool);

static_assert(std::is_same_v<decltype(&Process::RunGateway), ProcessRunGateway>);
static_assert(std::is_same_v<decltype(&GW::Gateway), GatewayLoop>);
static_assert(std::is_same_v<decltype(&GW::PushToInputQueue), GatewayEnqueue>);
static_assert(std::is_same_v<decltype(&GW::SetStopGatewayFlag), GatewayStop>);

// Taking and retaining the addresses forces the linker to resolve the actual
// production definitions in Common without executing any of them.
ProcessRunGateway volatile process_run_gateway = &Process::RunGateway;
GatewayLoop volatile gateway_loop = &GW::Gateway;
GatewayEnqueue volatile gateway_enqueue = &GW::PushToInputQueue;
GatewayStop volatile gateway_stop = &GW::SetStopGatewayFlag;
} // namespace

int main()
{
  const bool linked = process_run_gateway != nullptr &&
                      gateway_loop != nullptr &&
                      gateway_enqueue != nullptr &&
                      gateway_stop != nullptr;

  std::cout << "NG-039 D1-H compile/link probe only; no process, thread, IPC, "
               "Q0/Q1 fixture, or measurement was run. D2 remains blocked.\n";
  return linked ? 0 : 1;
}
~~~

</details>

<details>
<summary>D1-H manifest (path aliases)</summary>

~~~json
{
  "schema_version": 1,
  "task_id": "NG-039",
  "child_task_id": "NG-040",
  "branch": "AIOPT5",
  "document_commit": "b40bce89fe8692a5092889dd675e38d6af814d4c",
  "document_sha256": "8dd8234d2fbdd147f6e942f4bd509e953ea824471fd37c8a87029ecd675abd7b",
  "d0_approved_draft_sha256": "8026e1320149c318a5218eaa36c730f262081a5bbe0c2d582b23c307c7144e1c",
  "functional_baseline_sha": "1af604dad66148618cc0e578c28adb4a9829e607",
  "functional_baseline_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
  "scope": {
    "conditions": [
      {
        "id": "Q0",
        "definition": "No offered application messages; true queue-idle state has not been established."
      },
      {
        "id": "Q1",
        "definition": "One future-dated application message intended to remain pending throughout the measured window; admission, sole-head status, and non-execution are not yet observable."
      }
    ],
    "gateway_consumer_count": 1,
    "excluded_conditions": ["Q2", "Q3", "Q4"],
    "production_changes_allowed": false,
    "pgcs_or_pg_changes_allowed": false,
    "aiopt4_in_scope": false
  },
  "authorization": {
    "d0": "DOCUMENT_ONLY",
    "d1_h": "AUTHORIZED_FOR_ISOLATED_TEST_ONLY_SOURCE_AND_BUILD",
    "d2_correctness": "NOT_AUTHORIZED",
    "d3_measurement": "NOT_AUTHORIZED",
    "runtime_execution": false,
    "ipc_or_semaphore_creation": false,
    "vm_or_ssh_operations": false,
    "paid_astra_review": false
  },
  "build_only_target": {
    "option": "NG_ENABLE_NG039_GW_Q0_Q1_HARNESS",
    "target": "ng039_gw_q0_q1_harness",
    "source": "tests/spec039/gw_q0_q1_harness.cpp",
    "purpose": "Compile/link references to the production Process/GW public entry points; no runtime fixture."
  },
  "source_map": {
    "process_entry": "Process::RunGateway() -> GW::Gateway()",
    "queue_admission": "GW::PushToInputQueue(Message*) requires more than two command lines.",
    "future_message_rule": "GW::Gateway() pops only when ScheduledTime < GetTime().",
    "baseline_paths": [
      "Common/src/Process.cpp",
      "Common/src/GW.cpp",
      "Common/src/GW.h"
    ]
  },
  "d2_blockers": [
    "GW construction performs initialization that can enqueue bootstrap messages; Q0 as a truly idle input queue is not established by the current public path.",
    "InputQueue has no public snapshot/clear API, so Q1 admission, sole-head/pending status, and non-execution cannot be independently asserted through the public interface.",
    "GW::Gateway() starts an output worker and polls/creates local IPC resources; dynamic correctness requires a separately authorized lifecycle and disposable-namespace design."
  ],
  "d0_run_parameters_not_frozen_for_execution": {
    "independent_runs_per_condition": 5,
    "warmup_seconds": 30,
    "measurement_seconds": 120,
    "bounded_drain_seconds": 30,
    "note": "D0 proposal values only; D2/D3 remain unauthorized and these values must be confirmed/frozen before any run."
  },
  "implemented_paths": [
    "CMakeLists.txt",
    "tests/spec039/gw_q0_q1_harness.cpp",
    "tests/spec039/q0_q1.manifest.json"
  ],
  "deferred_paths_pending_observability_and_separate_gate": [
    "tests/spec039/run_gw_q0_q1.py",
    "tests/spec039/analyse_gw_q0_q1.py"
  ],
  "forbidden_paths_and_actions": [
    "Common/src/GW.cpp",
    "Common/src/GW.h",
    "PGCS/src/**",
    "Make/**",
    "Scripts/**",
    "Specs/RESULTS-SPEC-039/**",
    "Do not run the executable, CTest, a runner, a profiler, a sampler, or a benchmark.",
    "Do not create, attach, inspect destructively, or clean SysV IPC, POSIX semaphores, or shared memory.",
    "Do not change the primary AIOPT5 worktree, commit, push, use VMs/SSH/QGA, or contact paid Astra."
  ],
  "isolated_checkout": "<D2_WORKTREE>",
  "external_build_directory": "<HISTORICAL_D1H_SCRATCH>/build",
  "configure_command": "cmake -S <D2_WORKTREE> -B <HISTORICAL_D1H_SCRATCH>/build -DNG_ENABLE_NG039_GW_Q0_Q1_HARNESS=ON",
  "build_command": "cmake --build <HISTORICAL_D1H_SCRATCH>/build --target ng039_gw_q0_q1_harness --parallel 2",
  "authorization_evidence": {
    "state": "APPROVED",
    "session": "20261004_123636_9a3f44",
    "user_message_id": 100566,
    "scope": "isolated test-only source and build; no executable/runtime, IPC, D2 correctness, D3 measurement, production changes, VM/SSH, or paid Astra"
  },
  "build_evidence": {
    "result": "PASS",
    "exit_code": 0,
    "cmake_version": "3.28",
    "generator": "Unix Makefiles",
    "compiler": "/usr/bin/c++",
    "compiler_version": "GNU 13.3.0",
    "target": "ng039_gw_q0_q1_harness",
    "cxx_defines": "",
    "cxx_flags": "-O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer -std=gnu++20",
    "link_flags": "-O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer -static -static-libgcc -static-libstdc++",
    "binary_path": "<HISTORICAL_D1H_SCRATCH>/build/ng039_gw_q0_q1_harness",
    "binary_sha256": "ddf06f6d47e37564b9e52866c3d272ce3cc6cd8b433700c864c4499d676c31ab",
    "source_sha256": {
      "CMakeLists.txt": "d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d",
      "tests/spec039/gw_q0_q1_harness.cpp": "15913ee8b82fed7ad259668ed84e6e261aa444a3df443f3521929eba01b8dc92"
    },
    "build_log_path": "<HISTORICAL_D1H_SCRATCH>/build/build-output.txt",
    "build_log_sha256": "095009a9e4fb7097aef005300f7562ab8ce941c691538a88a4064f944f4d19aa",
    "executable_runtime_executed": false
  },
  "runtime_command": null
}
~~~

</details>

<details>
<summary>D1-H rebuild receipt (local paths redacted)</summary>

~~~json
{
  "binary": {
    "bytes": 7366600,
    "executed": false,
    "path": "<D1H_REBUILD_ROOT>/build/ng039_gw_q0_q1_harness",
    "sha256": "060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d"
  },
  "build": {
    "argv": [
      "cmake",
      "--build",
      "<D1H_REBUILD_ROOT>/build",
      "--target",
      "ng039_gw_q0_q1_harness",
      "--parallel",
      "2"
    ],
    "compiler": "/usr/bin/c++",
    "compiler_version": "GNU 13.3.0",
    "exit_code": 0,
    "log": "<D1H_REBUILD_ROOT>/build.log",
    "log_bytes": 8936,
    "log_sha256": "658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98",
    "reported_warnings": 2
  },
  "configure": {
    "argv": [
      "cmake",
      "-S",
      "<D1H_WORKTREE>",
      "-B",
      "<D1H_REBUILD_ROOT>/build",
      "-DNG_ENABLE_NG039_GW_Q0_Q1_HARNESS=ON",
      "-DNG_ENABLE_LEGACY_STANDALONE=OFF"
    ],
    "cmake_version": "3.28.3",
    "exit_code": 0,
    "generator": "Unix Makefiles",
    "log": "<D1H_REBUILD_ROOT>/configure.log",
    "log_bytes": 652,
    "log_sha256": "65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb"
  },
  "d1h_manifest_sha256": "9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a",
  "distinction_from_lost_artifacts": "This is a fresh build with new hashes; it does not recover or independently rehash the historical binary/log hashes in the original D1-H manifest.",
  "environment_capture": "Inherited environment was used for this D1-H reconstruction and was not snapshotted; the separate r07 D2 plan specifies a minimal explicit env -i environment.",
  "gate": "D1-H compile/link only",
  "output_is_outside_hermes": true,
  "output_root": "workspace/ng039-d1h-rebuild-20261010",
  "rebuild_date": "2026-10-10",
  "schema_version": "ng039-d1h-build-receipt/v1",
  "scope": "Compile/link only. No harness execution, IPC, namespace, correctness test, timing, D2, D3, VM/SSH, commit or push.",
  "source_head": "1af604dad66148618cc0e578c28adb4a9829e607",
  "source_sha256": {
    "CMakeLists.txt": "d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d",
    "tests/spec039/gw_q0_q1_harness.cpp": "15913ee8b82fed7ad259668ed84e6e261aa444a3df443f3521929eba01b8dc92"
  },
  "source_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
  "source_worktree": "<D1H_WORKTREE>",
  "task_id": "NG-039",
  "working_directory": "<D1H_WORKTREE>"
}
~~~

</details>

## 9. Read-only host preflight and r14 local-validation evidence

### D2 host preflight

Original receipt SHA-256 `38d9d232b27476e89190b82ddc6538d1534362b647d14d904921ae6499ca51b6. It is a bounded read-only observation, not an exhaustive absence proof.

~~~json
{
  "gates_still_required": [
    "Freeze the complete D2 pre-code profile and review packet, including the dedicated identity/launcher/lease design, exact allowlist, target graph, oracle, and supervisor/finalizer evidence contract.",
    "Obtain owner consent bound to the final exact packet SHA before any additional Astra call.",
    "Provision and independently verify the dedicated non-login UID, trusted single-run launcher, and exclusive lease; this host-side change is outside the five-file D2 test-code allowlist and was not performed.",
    "Record a separate owner execution decision before any D2 configure/build/harness/IPC run.",
    "D3 remains separate and requires D2 PASS plus its own explicit authorization."
  ],
  "host_checks": {
    "checked_launcher_directories": [
      "/usr/local/sbin",
      "/usr/local/bin",
      "<OWNER_HOME>/.local/bin",
      "<OWNER_HOME>/.config/systemd/user",
      "/etc/systemd/system"
    ],
    "limitation": "The directory check was name-filtered over the listed locations, not an exhaustive privileged filesystem audit; absence of matching names is not proof that no arbitrary-named launcher/account exists.",
    "matching_named_launchers_or_units": [],
    "processes": {
      "matches": [],
      "query": "ps -eo user,uid,pid,ppid,comm filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination"
    },
    "sudo_or_root_changes": false,
    "systemd": {
      "exit_code": 0,
      "matches": [],
      "query": "systemctl list-unit-files --no-legend filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination"
    },
    "user_accounts": {
      "limitation": "Name-filtered search only; no account name or UID for the future dedicated identity is currently pinned in this receipt.",
      "matches": [],
      "query": "local passwd entries whose names contain ng039, q0q1, or q0_q1"
    }
  },
  "not_performed": [
    "No SPEC or production source edit",
    "No UID/group/service/launcher/lease provisioning",
    "No sudo",
    "No build/configure",
    "No D2 harness or finalizer execution",
    "No IPC creation/removal or runtime test",
    "No Astra call",
    "No commit or push"
  ],
  "receipt_sha256": "38d9d232b27476e89190b82ddc6538d1534362b647d14d904921ae6499ca51b6",
  "repository_state": {
    "branch": "AIOPT5",
    "git_diff_check": "PASS",
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "preexisting_dirty_paths_preserved": [
      "Specs/SPEC-039-common-performance-measurement.md",
      "Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md",
      "Specs/RESULTS-SPEC-039/",
      "Specs/RESULTS-SPEC-059/"
    ],
    "spec_sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025"
  },
  "review_scope": {
    "limitation": "Astra approved only the bounded documentary closure of the r13 journal-policy contradiction; it did not approve the full D2 profile, implementation, or runtime.",
    "r14_packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
    "r14_verdict": "APPROVE"
  },
  "scope": "Read-only local host/worktree readiness checks to identify prerequisites for the SPEC-039 D2 gate. This is not a runtime or authorization receipt.",
  "status": "BLOCKED_PREPARATION_ONLY",
  "worktrees": {
    "isolated_d1h_checkout": {
      "actions": "Preserved; no edits/build/run performed during this preflight.",
      "branch": "DETACHED",
      "d1h_files": [
        "tests/spec039/gw_q0_q1_harness.cpp",
        "tests/spec039/q0_q1.manifest.json"
      ],
      "d2_files": {
        "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
        "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
      },
      "head": "1af604dad66148618cc0e578c28adb4a9829e607",
      "status": [
        "M CMakeLists.txt",
        "?? tests/"
      ]
    },
    "primary_checkout_d2_files": {
      "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
      "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
    }
  }
}
~~~

### r14 local verification (not D2 validation)

Receipt SHA-256 `ebafdb331e823f25975e4e9c7f85cc03fe75e809e64253ed177f8b1ec77931af`. Reported: 63 tests normal and optimized, five Draft 2020-12 schemas meta-validated, read-only methodology checker PASS; D2 configure/build/runtime, finalizer, IPC and cross-artifact PID validation were NOT EXECUTED.

<details>
<summary>r14 local-validation receipt (paths sanitized)</summary>

~~~json
{
  "amendment_summary": {
    "model_version": 2,
    "new_requirement_id": "AUTH-005",
    "policy": "Bounded read-only parsing/replay of reservation-journal.jsonl is solely for COUT-001..COUT-004. Invalid, unavailable, malformed, truncated or incomplete history yields unknown without blocking an otherwise authorized abnormal INCONCLUSIVE record. Other non-control evidence remains raw-only; journal mutation, IPC inspection, cleanup and retry remain forbidden.",
    "r13_verdict": "AMEND",
    "reviewed_r13_packet_sha256": "0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727",
    "same_spec_number": "SPEC-039"
  },
  "created_date": "2026-10-13",
  "evidence_boundary": {
    "Astra_followup": "NOT_SUBMITTED; exact-SHA consent not granted",
    "D2_D3": "BLOCKED",
    "build_or_configure": "NOT_EXECUTED",
    "commit_push": "NOT_PERFORMED",
    "cross_artifact_pid_set": "NOT_EXECUTED",
    "ipc_vm_ssh": "NOT_EXECUTED",
    "offline_synthetic_and_documentary_only": true,
    "production_finalizer": "NOT_EXECUTED",
    "runtime_correctness_performance": "NOT_EXECUTED"
  },
  "identity": {
    "branch": "AIOPT5",
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "manifest_path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.16-r14.json",
    "manifest_sha256": "f04b5d6333b14db6dc91a4cd0b7afd5f77c2f148797d1f0631b843457c1e393f",
    "packet_bytes": 182410,
    "packet_path": "Specs/RESULTS-SPEC-039/precode-packet-v1.16-amend-closure-20261013-sanitized-draft-r14.md",
    "packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
    "packet_submitted": false,
    "r14_exact_sha_consent": false,
    "repository": "<AIOPT5_REPO>",
    "spec_path": "Specs/SPEC-039-common-performance-measurement.md",
    "spec_sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025"
  },
  "owner_authorization": {
    "build_runtime_ipc_vm_commit_push_authorized": false,
    "message": "Sim — autorizo a emenda na mesma SPEC-039 e validação local, sem novo Astra nem runtime",
    "new_astra_call_authorized": false,
    "scope": "SPEC-039 documentation, corresponding manifest/packet projections and offline local validation only"
  },
  "purpose": "Read-only validation of the owner-authorized same-SPEC-039 v1.16/r14 documentary amendment; no Astra follow-up was sent.",
  "r13_immutability": {
    "all_verified_unchanged": true,
    "manifest_sha256": "2f378565c90a10b4b08abcc9f0469993dd243cd73cd56c8477f4710408c5a8a5",
    "packet_sha256": "0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727",
    "r13_tool_snapshot_manifest_sha256": "c4846cb9c9faef42b33224e858bd80fa74a6643233a2a201fb97d17408ac9fbb",
    "raw_sse_sha256": "b819bf078d2690fdfbb945e123873d66d469a72f819a3a4a38e3c6282b7a32b7",
    "response_sha256": "94c118fe12b2b27143a521b43be5ea194d4d0969b4ea35c8829d868d91af47d0",
    "submission_receipt_sha256": "45b604e15399cfb087bad6ab9b16a03d8013df19d75a0fdae5f229f8aecc1a33"
  },
  "receipt_type": "ng039-spec039-local-validation/v1",
  "status": "PASS_WITH_EXPLICIT_LIMITATIONS",
  "validation": {
    "git_diff_check": "PASS",
    "live_read_only_checker": {
      "Astra": "NOT_SUBMITTED",
      "command": "PYTHONDONTWRITEBYTECODE=1 python3 verify_spec039_methodology_v1.py --repo-root <AIOPT5_REPO> --spec Specs/SPEC-039-common-performance-measurement.md --manifest Specs/RESULTS-SPEC-039/precode-manifest-v1.16-r14.json --packet Specs/RESULTS-SPEC-039/precode-packet-v1.16-amend-closure-20261013-sanitized-draft-r14.md --expected-head 756f493b7049434ab67cf2382486a48f606a470c",
      "cross_artifact_pid_set": "NOT_EXECUTED",
      "errors": [],
      "hashes": {
        "manifest": "f04b5d6333b14db6dc91a4cd0b7afd5f77c2f148797d1f0631b843457c1e393f",
        "packet": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
        "spec": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025"
      },
      "policy_text_projections_checked": 17,
      "runtime_finalizer": "NOT_EXECUTED",
      "status": "PASS",
      "writes_performed": false
    },
    "schema_meta_validation": {
      "draft": "2020-12",
      "jsonschema": "4.10.3",
      "method": "Draft202012Validator.check_schema",
      "python": "3.12.3",
      "schema_names": [
        "ipc_inventory",
        "observer_events",
        "reservation_journal_event",
        "run_record",
        "supervisor_termination"
      ],
      "schemas_checked": 5,
      "status": "PASS"
    },
    "stale_r13_negative_probe": {
      "inputs": "live v1.16 SPEC with the frozen r13 manifest and packet",
      "purpose": "prove stale model, policy and artifact pins cannot pass against the v1.16 contract",
      "status": "EXPECTED_FAIL_CLOSED",
      "writes_performed": false
    },
    "unit_tests": {
      "normal": {
        "cli_methods": 3,
        "command": "PYTHONDONTWRITEBYTECODE=1 python3 -m unittest -q test_spec039_journal_transitions test_spec039_projection_checker test_spec039_methodology_cli",
        "errors": 0,
        "failures": 0,
        "model_projection_methods": 60,
        "status": "PASS",
        "total_methods": 63
      },
      "optimized": {
        "cli_methods": 3,
        "command": "PYTHONDONTWRITEBYTECODE=1 PYTHONOPTIMIZE=1 python3 -m unittest -q test_spec039_journal_transitions test_spec039_projection_checker test_spec039_methodology_cli",
        "errors": 0,
        "failures": 0,
        "model_projection_methods": 60,
        "status": "PASS",
        "total_methods": 63
      },
      "test_first_regression": {
        "green": "PASS: policy validation, projections and the completed suite passed after the amendment.",
        "red": "PASS: the new bounded-journal-policy test first failed because model_version 2 was unsupported by the old checker."
      }
    }
  }
}
~~~

</details>

## 10. Exact companion manifest

The exact machine-readable manifest is embedded below and separately frozen at `precode-manifest-v1.16-d2a-r15.json`, SHA-256 `4ed5646dcaab8af74344f3acba22c962fef8703b2d4d0fddd463c0381b9e3827`.

~~~json
{
  "baseline_source_files": {
    "CMakeLists.txt": {
      "baseline_blob_sha256": "5f9cd0676b2dcdcd0a500d778f5d5baea312301d6abc382db5eab95f2bbb1d07",
      "worktree_sha256": "d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d"
    },
    "Common/src/Block.cpp": {
      "baseline_blob_sha256": "26dfa06b575cea6bb9c8d21a18b894f37e73ad91c7e7a7450fefe517903e1622",
      "worktree_sha256": "26dfa06b575cea6bb9c8d21a18b894f37e73ad91c7e7a7450fefe517903e1622"
    },
    "Common/src/GW.cpp": {
      "baseline_blob_sha256": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e",
      "worktree_sha256": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e"
    },
    "Common/src/GW.h": {
      "baseline_blob_sha256": "54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44",
      "worktree_sha256": "54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44"
    },
    "Common/src/GWRunInitialization01.cpp": {
      "baseline_blob_sha256": "1656b703c892831a05043a8bf82ae1bed73d701c8ffea7b95af5a02df0c978af",
      "worktree_sha256": "1656b703c892831a05043a8bf82ae1bed73d701c8ffea7b95af5a02df0c978af"
    },
    "Common/src/Message.cpp": {
      "baseline_blob_sha256": "96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518",
      "worktree_sha256": "96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518"
    },
    "Common/src/Message.h": {
      "baseline_blob_sha256": "bdbbfee17af8512b2c64e9b61cf83e73b5293cffcd7067c0a10787c80bac4e70",
      "worktree_sha256": "bdbbfee17af8512b2c64e9b61cf83e73b5293cffcd7067c0a10787c80bac4e70"
    },
    "Common/src/Process.cpp": {
      "baseline_blob_sha256": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1",
      "worktree_sha256": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1"
    }
  },
  "bounded_prior_review_history": [
    {
      "checkpoint": "historical D2 pre-code v1.2",
      "consent": "consumed; no follow-up consent",
      "packet_sha256": "f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942",
      "response_sha256": "a9d900654287be6f3d9afd5c6e90f1b8a53fe03a03202f8311330345675dcd05",
      "spec_sha256": "8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91",
      "verdict": "HOLD"
    },
    {
      "checkpoint": "v1.15/r13",
      "packet_sha256": "0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727",
      "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.15-r13-20261013.md",
      "response_sha256": "94c118fe12b2b27143a521b43be5ea194d4d0969b4ea35c8829d868d91af47d0",
      "scope": "abnormal-mode journal-policy contradiction",
      "verdict": "AMEND"
    },
    {
      "checkpoint": "v1.16/r14",
      "packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
      "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.16-r14-20261013.md",
      "response_sha256": "2d0183b961cd5d7a7ee48c555f0bf04345ed9bcb07761697d84ec0fdc57d9406",
      "scope": "r13 journal-policy contradiction only; not full D2/profile/runtime",
      "verdict": "APPROVE"
    }
  ],
  "consent": {
    "astra_calls_authorized": 0,
    "astra_exact_sha_consent_for_this_packet": false,
    "d2_execution_authorized": false,
    "provisioning_authorized": false
  },
  "created_date": "2026-10-13",
  "d1h_cmake_overlay_sha256": "75b25d39ae24b524695eebcba758915fbe7c1526c242dc0bec7ba5c2b79cb710",
  "functional_baseline": {
    "head": "1af604dad66148618cc0e578c28adb4a9829e607",
    "pre_existing_overlay": "D1-H CMake change and untracked harness/manifest; preserve unchanged",
    "tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
    "working_tree_status": "M CMakeLists.txt\n?? tests/",
    "worktree_alias": "<D2_WORKTREE>"
  },
  "historical_d2_hold_response": {
    "response_chars": 10687,
    "response_sha256": "a9d900654287be6f3d9afd5c6e90f1b8a53fe03a03202f8311330345675dcd05",
    "transcript_alias": "ng039_d2_precode_f29274fa"
  },
  "host_launcher_proposal": {
    "candidate_lease": "/run/lock/ng039-d2/lease.lock",
    "candidate_supervisor": "/usr/local/libexec/ng039-d2-supervisor",
    "candidate_uid": "ng039d2",
    "candidate_unit": "/etc/systemd/system/ng039-d2.service",
    "candidate_workspace_root": "/var/lib/ng039-d2/workspace",
    "path": "Specs/RESULTS-SPEC-039/d2-launcher-lease-proposal-v1-20261013.md",
    "sha256": "ab386f6a94228ab9312efa4526e68b67ab43fc0f7936b789a9ebc459b59f67e6",
    "status": "draft, not installed or tested"
  },
  "normative_contract": {
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025",
    "status": "In Progress",
    "version": "v1.16"
  },
  "not_yet_created": [
    "tests/spec039/gw_q0_q1_d2.cpp",
    "tests/spec039/q0_q1_d2.manifest.json",
    "dedicated UID",
    "systemd unit",
    "supervisor executable",
    "lease/control/workspace directories"
  ],
  "proposed_code_allowlist": [
    "CMakeLists.txt",
    "Common/src/GW.h",
    "Common/src/GW.cpp",
    "tests/spec039/gw_q0_q1_d2.cpp",
    "tests/spec039/q0_q1_d2.manifest.json"
  ],
  "proposed_oracle_resolution": {
    "bootstrap_drain": "Proposed sticky scalar event after a completed Gateway iteration proves constructor-queued bootstrap work is drained: Q0 requires queue_depth=0 and all-queue admission/pop/Run-entry/Run-return counts reconciled; Q1 requires only the registered future target remain at head and all non-target bootstrap work reconciled. The synchronous constructor Run is complete before the harness receives Process/GW.",
    "empty_sample_point": "Only at actual condition_variable::wait_for return after InputQueueMutex is reacquired, if StopGateway is false and InputQueue is empty; increment one saturating observation and event sequence there.",
    "q1_correlation": "WaitCapture and DecisionCapture are sticky; capture returned wait predicate value and the existing strict due/pop branch only; same iteration ID, increasing sequence and identical target/head/candidate scalar ID are required; overflow/mismatch is INCONCLUSIVE.",
    "run_cardinality": "Interpret N=1 as one GW consumer per condition; proposed paired experiment is two fresh, sequential Q0/Q1 process starts with distinct run IDs/journals/final records under one trusted service/lease. This interpretation is a review question, not execution authorization.",
    "synchronization": "One lock at existing admission/wait/pop sites; no recursive lock; scalar-copy accessor under InputQueueMutex; no observer Message*; counters saturate and overflow fails closed."
  },
  "readiness_evidence": {
    "d1h_binary_executed": false,
    "d1h_binary_sha256": "060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d",
    "d1h_harness_sha256": "15913ee8b82fed7ad259668ed84e6e261aa444a3df443f3521929eba01b8dc92",
    "d1h_manifest_sha256": "9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a",
    "d1h_receipt_sha256": "3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28",
    "d2_files_present_in_isolated_worktree": {
      "tests/spec039/gw_q0_q1_d2.cpp": false,
      "tests/spec039/q0_q1_d2.manifest.json": false
    },
    "d2_files_present_in_main_checkout": {
      "tests/spec039/gw_q0_q1_d2.cpp": false,
      "tests/spec039/q0_q1_d2.manifest.json": false
    },
    "host_preflight": {
      "d2_source_files_present": {
        "gw_q0_q1_d2.cpp": false,
        "q0_q1_d2.manifest.json": false
      },
      "gates_still_required": [
        "Freeze the complete D2 pre-code profile and review packet, including the dedicated identity/launcher/lease design, exact allowlist, target graph, oracle, and supervisor/finalizer evidence contract.",
        "Obtain owner consent bound to the final exact packet SHA before any additional Astra call.",
        "Provision and independently verify the dedicated non-login UID, trusted single-run launcher, and exclusive lease; this host-side change is outside the five-file D2 test-code allowlist and was not performed.",
        "Record a separate owner execution decision before any D2 configure/build/harness/IPC run.",
        "D3 remains separate and requires D2 PASS plus its own explicit authorization."
      ],
      "not_performed": [
        "No SPEC or production source edit",
        "No UID/group/service/launcher/lease provisioning",
        "No sudo",
        "No build/configure",
        "No D2 harness or finalizer execution",
        "No IPC creation/removal or runtime test",
        "No Astra call",
        "No commit or push"
      ],
      "observations": {
        "gates_still_required": [
          "Freeze the complete D2 pre-code profile and review packet, including the dedicated identity/launcher/lease design, exact allowlist, target graph, oracle, and supervisor/finalizer evidence contract.",
          "Obtain owner consent bound to the final exact packet SHA before any additional Astra call.",
          "Provision and independently verify the dedicated non-login UID, trusted single-run launcher, and exclusive lease; this host-side change is outside the five-file D2 test-code allowlist and was not performed.",
          "Record a separate owner execution decision before any D2 configure/build/harness/IPC run.",
          "D3 remains separate and requires D2 PASS plus its own explicit authorization."
        ],
        "host_checks": {
          "checked_launcher_directories": [
            "/usr/local/sbin",
            "/usr/local/bin",
            "<OWNER_HOME>/.local/bin",
            "<OWNER_HOME>/.config/systemd/user",
            "/etc/systemd/system"
          ],
          "limitation": "The directory check was name-filtered over the listed locations, not an exhaustive privileged filesystem audit; absence of matching names is not proof that no arbitrary-named launcher/account exists.",
          "matching_named_launchers_or_units": [],
          "processes": {
            "matches": [],
            "query": "ps -eo user,uid,pid,ppid,comm filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination"
          },
          "sudo_or_root_changes": false,
          "systemd": {
            "exit_code": 0,
            "matches": [],
            "query": "systemctl list-unit-files --no-legend filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination"
          },
          "user_accounts": {
            "limitation": "Name-filtered search only; no account name or UID for the future dedicated identity is currently pinned in this receipt.",
            "matches": [],
            "query": "local passwd entries whose names contain ng039, q0q1, or q0_q1"
          }
        },
        "not_performed": [
          "No SPEC or production source edit",
          "No UID/group/service/launcher/lease provisioning",
          "No sudo",
          "No build/configure",
          "No D2 harness or finalizer execution",
          "No IPC creation/removal or runtime test",
          "No Astra call",
          "No commit or push"
        ],
        "receipt_sha256": "38d9d232b27476e89190b82ddc6538d1534362b647d14d904921ae6499ca51b6",
        "repository_state": {
          "branch": "AIOPT5",
          "git_diff_check": "PASS",
          "head": "756f493b7049434ab67cf2382486a48f606a470c",
          "preexisting_dirty_paths_preserved": [
            "Specs/SPEC-039-common-performance-measurement.md",
            "Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md",
            "Specs/RESULTS-SPEC-039/",
            "Specs/RESULTS-SPEC-059/"
          ],
          "spec_sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025"
        },
        "review_scope": {
          "limitation": "Astra approved only the bounded documentary closure of the r13 journal-policy contradiction; it did not approve the full D2 profile, implementation, or runtime.",
          "r14_packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
          "r14_verdict": "APPROVE"
        },
        "scope": "Read-only local host/worktree readiness checks to identify prerequisites for the SPEC-039 D2 gate. This is not a runtime or authorization receipt.",
        "status": "BLOCKED_PREPARATION_ONLY",
        "worktrees": {
          "isolated_d1h_checkout": {
            "actions": "Preserved; no edits/build/run performed during this preflight.",
            "branch": "DETACHED",
            "d1h_files": [
              "tests/spec039/gw_q0_q1_harness.cpp",
              "tests/spec039/q0_q1.manifest.json"
            ],
            "d2_files": {
              "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
              "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
            },
            "head": "1af604dad66148618cc0e578c28adb4a9829e607",
            "status": [
              "M CMakeLists.txt",
              "?? tests/"
            ]
          },
          "primary_checkout_d2_files": {
            "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
            "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
          }
        }
      },
      "receipt_sha256": "38d9d232b27476e89190b82ddc6538d1534362b647d14d904921ae6499ca51b6",
      "runtime_build_ipc": "not run by this preflight",
      "scope": "bounded read-only checks only; not an exhaustive host audit",
      "status": "BLOCKED_PREPARATION_ONLY",
      "uid_launcher_service": "not provisioned or verified"
    },
    "local_r14_validation_receipt_sha256": "ebafdb331e823f25975e4e9c7f85cc03fe75e809e64253ed177f8b1ec77931af",
    "r14_local_tests": "63 normal + 63 optimized; 5 Draft 2020-12 schemas meta-validated; read-only checker PASS; D2 runtime/finalizer NOT EXECUTED"
  },
  "receipt_type": "ng039-spec039-d2a-precode-manifest/v1",
  "repository_identity": {
    "branch": "AIOPT5",
    "commit_or_push": "not performed",
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "working_tree_status": "M Specs/SPEC-039-common-performance-measurement.md\n M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md\n?? Specs/RESULTS-SPEC-039/\n?? Specs/RESULTS-SPEC-059/"
  },
  "review_questions": [
    "Do current v1.16 clauses and supplied source excerpts close all four historical D2 pre-code HOLD findings at documentary level?",
    "Is the proposed exact bootstrap-drain and empty-observation event point sufficient to prove Q0 queue state without changing queue behavior?",
    "Is the Q0/Q1 cardinality interpretation (two fresh sequential process starts under one service/lease) consistent with §§2–4 and §7.1, or must it be amended before code?",
    "Does the root-owned systemd supervisor, dedicated UID, control-plane receipt/log ownership, cgroup process ledger, root-only continuous lease and inherited finalizer lease FD satisfy §7.1, including the proposed WORKSPACE_ROOT?",
    "Is the proposed compile-time-gated observer field/interface map and D2-only CMake graph specific enough for the pre-code gate; identify any exact remaining design blocker.",
    "Does the three-path diagnostic sem_unlink suppression remain bounded and fail-closed, with macro-off default behavior unchanged?"
  ],
  "review_scope": "D2a pre-code design readiness only; no implementation, build, runtime, IPC, host provisioning, D2/D3 acceptance, or operational authority",
  "status": "LOCAL_DRAFT_UNSUBMITTED",
  "validation_boundary": {
    "Astra": false,
    "IPC": false,
    "VM_SSH": false,
    "commit_push": false,
    "configure_build": false,
    "packet_assembly": "local deterministic assembly only",
    "packet_submitted": false,
    "runtime": false,
    "system_changes": false,
    "test_harness": false
  }
}
~~~

## 11. Explicit state and review boundary

- Main repository remains on AIOPT5 HEAD `756f493b7049434ab67cf2382486a48f606a470c`; the SPEC and review artifacts are working-tree files, not committed.
- Isolated baseline remains `1af604dad66148618cc0e578c28adb4a9829e607` / tree `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`; pre-existing D1-H modifications are preserved.
- D2 source/manifest files are absent in both examined checkouts. No dedicated UID, launcher, lease, unit, or control/workspace directory has been created.
- No D2 configure/build, harness/finalizer, runtime, IPC, VM/SSH, cleanup/retry, commit, push, or Astra call occurred.
- The observer contract refinements, Q0/Q1 two-process interpretation, workspace-root choice, and systemd unit are proposals for review, not owner decisions or amendments to SPEC-039.
- Any later Astra review needs fresh owner consent bound to the final packet SHA. Provisioning and D2 execution remain separate decisions; D3 remains a later independent gate.
````

## FILE: Specs/SPEC-039-common-performance-measurement.md

Role: Current normative document, same SPEC lineage.
Bytes: 79222
SHA-256: f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025

````text
# SPEC-039: Gateway Measurement Baseline on AIOPT5

**Version:** v1.16
**Date:** 2026-10-13
**Author:** Hermes; owner-directed same-SPEC documentary amendment
**Status:** In Progress
**Gate:** D0 remains limited to Q0/Q1. The v1.10/r07 checker amendments received documentary `APPROVE` only. The v1.11/r08 host-profile packet received `AMEND`; follow-up withdrew B1 as a sender-envelope typo and confirmed B2/B3. The v1.12/r09 and v1.13/r10 IPC/finalization findings were addressed in the same SPEC. Astra r12 returned `AMEND` on the journal classifier and cleanup-history projection; r13 recognised those canonical B1/B2 corrections but returned `AMEND` because abnormal-mode policy prohibited the bounded journal replay required by COUT-001–004. The owner authorised the v1.16 same-SPEC documentary correction and local validation on 2026-10-13. The v1.16 model makes the journal replay exception explicit while preserving raw-only handling of other non-control evidence. The model/checker/fixtures remain synthetic and documentary; no production finalizer is implemented or executed. This revision is unreviewed; D2 remains blocked pending any separately authorised exact-SHA review, provisioned prerequisites and a distinct owner execution decision. D3 is not authorised.
**Branch:** AIOPT5
**Baseline commit:** `1af604dad66148618cc0e578c28adb4a9829e607`
**D0-approved v1.1 draft SHA-256:** `8026e1320149c318a5218eaa36c730f262081a5bbe0c2d582b23c307c7144e1c` (owner approval: 2026-10-04; documentary Q0/Q1 scope only; does not approve this v1.16 amendment)
**Owner task:** NG-040, under NG-039
**Related:** `SPEC-032-gw-input-queue-batching.md`, `SPEC-033-message-commandline-refactor.md`

> v1.16 is the owner-authorized same-SPEC response to the r13 abnormal-mode journal-policy AMEND. It preserves the r12 B1/B2 lifecycle corrections and makes bounded, read-only journal-history classification for COUT-001–004 the sole parsing/replay exception in `abnormal_evidence_only`; invalid history remains `unknown` without blocking an otherwise authorized `INCONCLUSIVE` record. Other non-control evidence remains raw-preserved without schema replay; mutation, live IPC inspection, cleanup and retry remain forbidden. The updated model, checker and fixtures are local documentary/synthetic controls—not a production finalizer, runtime test or implementation authorization. A new Astra call is not authorized; D2/D3 remain blocked.

> This AIOPT5 Proposal uses the existing SPEC-039 number; it is not a restoration of the archived AIOPT3 Proposal or its approval. The archived copy was deliberately set aside during the NG-050 release freeze; its SHA-256 is `c46dd50a6d58c760eba165a8dfdc3e390df41e8049cf0f95527655350c80da4f`. Keep that copy intact. The v1.1 D0 approval was documentary only. Later, separately scoped owner permissions are recorded in §4; they do not transfer to other gates or authorise a performance claim.

## 1. Problem and decision

The owner has paused SPEC-027 RAW multi-worker reception and prioritised measuring the Gateway (`GW`) without changing `PGCS` or `PG`. The old Common performance plan and its measurements belonged to AIOPT3 and cannot serve as an AIOPT5 before/after baseline. First establish whether the GW scheduling, network-buffer consumption, or shared-memory (SHM) paths consume material time or strand work. Optimisation selection is a later decision, not a premise of this SPEC.

The committed AIOPT5 `GW.cpp` computes a wait of at most 1 ms and uses `!InputQueue.empty()` as a condition-variable predicate (`Gateway`, near lines 488–515). A future-dated head leaves the queue non-empty and is not popped until its scheduled time is strictly less than `GetTime()` (near lines 523–552). This is a source-level busy-loop candidate, not a measured dominant bottleneck. The Gateway drains the network receive queue under its mutex before attempting a SHM read (near lines 658–763); the relative contribution and fairness of those stages remain unmeasured.

## 2. Scope and immutable starting point

- Current first target: characterise one existing GW consumer (`N=1`) in two conditions only: idle and future-only. Do not infer RAW receive workers from `N=1`. Due-work, network-only and mixed network/SHM conditions are explicitly deferred until the first result justifies a second, separately approved measurement slice.
- For a future execution, use an isolated checkout at the exact baseline commit and record whether any test harness or diagnostics alter that tree. The current AIOPT5 checkout has staged and unstaged changes, including SPEC-027 telemetry in `GW.cpp`/`GW.h`; neither those changes nor the failed 2026-10-02 diagnostic run are a clean comparator. Do not reset, stash, overwrite, or silently transplant them.
- Preserve message bytes, inverted pub/sub, message lifetime, one-due-message-per-iteration scheduling, strict due-time test, priority, output retry semantics, SHM layout/semaphores, and process teardown. `SPEC-032-gw-input-queue-batching.md` remains Deferred.
- Exclude changes to `PGCS`/`PG`, RAW receive concurrency, remote VMs, release/Phase 0 acceptance, full Common/SAR/HT benchmarking, and a GW algorithmic patch. The archived SPEC-041 is a hypothesis and historical Proposal only, not an approved AIOPT5 implementation contract.

## 3. D0-approved measurement design (execution not authorised)

A run is one fresh, isolated local process start with one GW consumer and a fixed, recorded workload. Requests/messages within the same run are not independent repetitions. The initial Q0/Q1 proposal is five independent runs per condition, each with 30 s warm-up, 120 s measured interval, and up to 30 s bounded drain; the owner may revise these constants before authorising execution, but they must then be frozen in a manifest. Never reuse dirty caches, live trial locks, or existing IPC resources at the D2 candidate keys/names.

First gate only, on the same executable and fixed environment:

| Condition | Input | Primary diagnostic question |
|---|---|---|
| Q0 idle | No offered application messages | CPU and polling floor |
| Q1 future head | One message scheduled well beyond the measured window | Immediate returns/cycles while waiting |

Q2 (due-work service), Q3 (network-only publication) and Q4 (mixed network/SHM fairness) remain named questions, not planned runs or acceptance gates. They need a separate owner decision after Q0/Q1. The harness must document how it reaches the real GW path, validates the future message without executing it early, and creates/cleans its own local resources. D0 v1.1 did not authorise source edits; the later, narrowly scoped D2 seam permission is recorded in §4 and does not allow algorithm changes or work outside the frozen test seam.

Q0's zero application-offer stimulus is not proof that `InputQueue` is empty: Process/GW construction runs initialization and can queue bootstrap or periodic messages. Record observed queue state separately; do not claim a true idle floor unless the run proves no queued head or explicitly labels Q0 as “no application offers.”

For every run, capture the baseline SHA and source-tree identity, compiler/libc, effective per-target flags/macros, executable and configuration hashes, workload inputs/seed/rate/payload distribution, CPU allocation, log mode, wall-clock duration, CPU time, peak RSS, and exit/cleanup status. Use separate quiet `-O0` and quiet verified `-O2` builds if approved; their difference is a build-profile effect, not an algorithmic GW gain. Sanitizer builds are correctness diagnostics, never timing comparators. Keep logs bounded; disable per-message DEBUG in timed windows.

For Q0/Q1, independently observe GW cycles, wait calls, immediately returned waits with a future head, blocked duration and wake reasons, process CPU time, peak RSS, SHM poll attempts and maximum inter-poll gap, future-message offered/admitted/pending/executed counts and cleanup status. A future message must not execute within the window; reconcile its pending state. Define each counter's increment point, owner and unit. Missing boundary observations mean `INCONCLUSIVE` for that claim, not zero. First inventory existing counters and their actual update/emission sites; do not assume the dirty SPEC-027 header or a previous smoke supplies them. If Q0/Q1 indicate a material issue, a later approved slice can add due-work completion, queue residence, output retries and network/SHM fairness metrics.

Acceptance of the *Q0/Q1 measurement* gate requires a frozen manifest, repeatable harness/inputs, all planned run outcomes including failed/incomplete runs, raw data plus a small analysis script, source/binary/configuration bindings, and a written diagnosis of future-head waiting versus idle. Report run-level distributions and uncertainty rather than treating cycles as independent runs. This limited result cannot identify the dominant end-to-end bottleneck. Do not call an optimisation beneficial or instrumentation overhead ≤2% without comparable quiet and instrumented measurements that support the specific claim.

## 4. Gates and permissions

1. **D0 — Documentary review:** v1.1 was approved by the owner on 2026-10-04 for Q0/Q1 only, against the v1.1 digest above. This approval does not transfer to other scopes or confer execution authority.
2. **D1 — Local preparation:** pin the exact functional checkout, define the real production-linked fixture, freeze commands/manifest, and obtain scoped authorization for any test-only code/build. Preserve the dirty primary AIOPT5 tree; do not transplant its unrelated changes. The unavailable namespace profile is not replaced by an immediately runnable host profile: §7.1 now requires a provisioned dedicated single-run UID/launcher, otherwise D2 remains blocked before IPC creation.
3. **D1-H — Compile/link probe:** owner-authorised source/build-only probe in an isolated checkout at `1af604dad66148618cc0e578c28adb4a9829e607`. It passed, but the executable was not run. This proves symbol/type linkage only—not GW construction, Q0/Q1 behavior, correctness, or performance.
4. **D2a — Correctness oracle and testability:** the owner authorised a compile-time-gated test seam in `Common` and one local D2 correctness run, with no algorithm change, D3 timing, `PGCS`/`PG`, VM, commit or push. Freeze the exact files, oracle, host-controlled resource-attribution procedure, command and cleanup procedure in the pre-code packet. The current public API does not expose queue contents or pending/execution state; `GW::Gateway()` starts an output worker and touches IPC. For Q1, construct one Process-owned `Message` with an in-memory future `Time` and offer it through the real public `GW::PushToInputQueue` path. Do **not** use `NGAL_CS::DeliverToGateway` for this future-head stimulus: `GW::Gateway()` creates received messages with `NewMessage(0, ...)` (`GW.cpp:614–623`), while wire serialization emits command lines rather than the local scheduling `Time` (`Message.cpp:1599–1602`). The D2 run therefore tests queue admission/scheduling, not NGAL receive/deserialization, wire-format timing, or network delivery. Use three inert, syntactically shaped command lines whose action tuple is not registered in GW; this satisfies `NoCL > 2` (`GW.cpp:241–263`) while ensuring an unexpected early `Run` has no external action.

   The compile-time-gated observer exposes `GWQueueTestObservation` through a test-only scalar-copy accessor; it never retains a `Message*`. The snapshot includes a 64-bit event sequence, validity, one-way Gateway-start/stop state, registered target ID, event message ID, observed queue depth/head identity/head time, actual wait timeout/return, the existing strict due decision and actual pop result, sticky target admission/non-due/pop/Run entry/return counters, monotonic empty-queue count, overflow state, and a first-target-wait record containing its own sequence, target/head identity, head time/depth, timeout, wait return and due result. Value-initialise storage as `GWQueueTestObservation D2Observation{}`; `None` is the zero enum value. `RegisterD2Target` runs once before Gateway start, locks `InputQueueMutex`, rejects ID zero or a started Gateway, resets/binds the observation, and leaves it invalid until the first real event. Set `gateway_started=true` under that mutex before entering the loop; never clear it or allow re-registration. Writers increment sequence, update scalar data and counters, then publish validity; counters saturate and set overflow rather than wrap. At the first real `wait_for` return with the registered target as head, fill the sticky first-target-wait record once; later events cannot overwrite it. At Q0/Q1 start the harness captures sequence/counter baselines and tests for new events before the window ends. Writers at already-locked admission/wait/pop sites use a non-locking helper; Run/stop boundaries take the mutex only outside those scopes. No I/O, callbacks, or test-only symbols/state/calls in the default build.
5. **D2b — Correctness test code/build/run:** only after this v1.14 host-controlled profile is positively reviewed under fresh exact-SHA consent, the dedicated-identity preconditions are provisioned and verified, and a separate owner execution decision is recorded, implement only the frozen allowlist: root `CMakeLists.txt`; `Common/src/GW.h`; `Common/src/GW.cpp`; new `tests/spec039/gw_q0_q1_d2.cpp`; and new `tests/spec039/q0_q1_d2.manifest.json`. Add the separate option `NG_ENABLE_NG039_GW_Q0_Q1_D2` (default `OFF`) and target `ng039_gw_q0_q1_d2`; define `NG_GW_QUEUE_TEST_OBSERVATION` for `Common` and the harness only inside an isolated diagnostic target graph: the option-on root CMake path must declare only `Common` and this harness and return before production application targets, with a configure-time target-census guard rejecting any other target. Keep the existing D1-H harness/manifest as historical evidence and do not overwrite them. Gate observer declarations, storage, methods and production call sites together. In the diagnostic build only, guard all three pinned `GW.cpp` branches that call `sem_unlink` after `sem_open` failure (`GW.cpp:970,1263,1322`): they must record a bounded failure event and must not call `sem_unlink`; the macro-off default build preserves existing behavior. Add targeted tests proving each injected `sem_open` failure path performs no unlink. This safety change is specified, not authorized for implementation by this document. Build the D2 target in a fresh isolated checkout at the pinned functional baseline. The dedicated-UID launcher and lease must be pinned and verified before any D2 action; until provisioned, no launch command is authorized and the old direct `timeout <harness>` invocation is prohibited. The manifest must freeze exact launcher/lease identity, run/finalizer commands, normal-versus-abnormal mode predicates, the `supervisor-termination.json` schema, and process-tree bounds before the separate execution gate. Resolve all path aliases before any command. Keep worktree, build directories, HOME/TMPDIR and results outside `.hermes`; use only named workspace/repository roots. The harness spawns no child processes.

   The trusted dedicated-UID launcher and exclusive lease must be verified before any IPC action; the direct host-UID launch command is prohibited. Only then capture complete host inventories for `/proc/sysvipc/{shm,sem,msg}` and `/dev/shm`; host inventories may be non-empty, but all five candidate key/name pairs must be absent. Derive and record `D2_PROCESS_KEY` and selected attempt using §7.1's exact algorithm; candidate keys are `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. Embed exact raw `/proc/sysvipc` bytes (canonical base64) in each inventory artifact; finalizer must recompute raw digests and normalized summaries from those bytes. Create and fsync `reservation-journal.jsonl` before reservation. Abort before construction on unreadable/over-cap inventory, candidate collision, key overflow, missing lease, or ambiguity. Then exclusively reserve all five SHM segments and five POSIX semaphores and journal each intent/outcome. NovaGenesis constructors may open only these reserved resources. A collision, mode mismatch, unverified returned-handle/libc mapping, incomplete journal or failed reservation blocks construction/PASS; do no speculative cleanup.

   After Gateway/output threads stop and join and `Process/GW` are destroyed, retain the dedicated-UID lease through finalization. Remove only exact run-created SHM IDs/names whose successful journal receipts and current identities match. Name-based `sem_unlink` is permitted only while the verified exclusive execution domain remains held; if the lease is lost, another process appears, or identity/journal state is ambiguous, perform no unlink/RMID for the disputed object and preserve residue. Append/fsync each cleanup intent and outcome; an intent without a durable outcome is `INCONCLUSIVE`, never evidence of removal. Capture final inventory, including exact raw table bytes, and require it to match pre. Any failed cleanup, residual candidate, unexplained change, blocked join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad cleanup. The finalizer verifies and hashes the journal and all bundle artifacts. Keep D2 at `N=1`, Q0/Q1 correctness only; no performance claim, algorithm change, `PGCS`/`PG`, VM, commit or push unless separately authorized.
6. **D3 — Characterisation:** Q0/Q1 timing is a separate gate, allowed only after D2 passes and the owner explicitly authorises the frozen measurement plan. Preserve all run outcomes and identities; classify the future-head hypothesis `MEASURED`, `INCONCLUSIVE`, or `BLOCKED`.
7. **D4 — Follow-up decision:** decide whether to stop, approve another measurement slice, or propose a performance implementation SPEC. Any future change requires a predeclared baseline/metric, actual-source review, correctness controls and its own owner-approved implementation allowlist. Astra or subagent opinions never grant implementation or runtime authority.

### 4.5 GPT-6 Astra consultation checkpoints (advisory; no execution authority)

| Checkpoint | Packet and question | Gate informed |
|---|---|---|
| Before D2 test-seam/code | Exact SPEC/manifest/packet hashes, functional baseline, D1-H manifest, proposed observer/interface diff and isolated CMake target graph; the dedicated-UID launcher, exclusive lease, `supervisor-termination.json` wait receipts and exact process-tree evidence; all three guarded `sem_unlink` paths; journal event schema/state transitions; raw `/proc/sysvipc` retention/recomputation and exact returned-`sem_t`→libc/dev-shm inode mapping. Ask whether the trust boundary actually excludes same-UID mutation from pre-inventory through finalizer, whether any failure path can still unlink an unowned object, whether crash/replay semantics forbid ambiguous cleanup, whether normal-cleanup and abnormal-evidence-only finalizer modes are mutually consistent, whether process-tree wait receipts and closed logs suffice to authorize an INCONCLUSIVE-only no-cleanup path, and whether the schemas can support a non-vacuous PASS without claiming unverified evidence. | The frozen v1.10/r07 B1/B2/B3 packet received `APPROVE` only for its documentary findings. The immutable v1.11/r08 packet `3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272` received `AMEND`; follow-up withdrew B1 (sender-envelope typo) and retained B2/B3. The v1.12/r09 packet returned `AMEND` on the IPC boundary; the v1.13/r10 packet closed that issue but returned `AMEND` on abnormal finalization. The v1.14/r12 and v1.15/r13 exact-SHA reviews are archived; r13 returned `AMEND` on the abnormal journal-policy contradiction. The owner authorised this v1.16 documentary correction and local validation only. Any subsequent Astra review requires a separate exact-SHA decision; no review grants D2 authority. |
| After D2 correctness result, before D3 | Frozen diff/build/test manifest, all raw outcomes including failures, identities and cleanup evidence. Ask whether the D2 assertions are supported and what remains inconclusive. | Planned checkpoint; a separate Astra submission requires owner consent. The owner decides whether to accept D2 and authorize D3. |
| After D3, before any implementation SPEC | Frozen raw data, analysis, source/build/workload identities and hypothesis classification. Ask whether the evidence supports a future implementation proposal and its acceptance metric. | Future checkpoint only; separate owner consent is required. Astra advice never authorises production changes. |

The owner authorised one GPT-6 Astra review via OpenAI Codex for the frozen v1.2 pre-code D2 packet. That call returned HOLD on the v1.2 SPEC SHA-256 `8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91` and packet SHA-256 `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`; its consent was consumed. On 2026-10-05 the owner separately authorised one review of the sanitized v1.4 packet SHA-256 `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948`, bound to SPEC v1.4 SHA-256 `f54901cad03209f94fe98cfad3bf62953895ed24fef3d771a89b89f5229217e1` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`. Astra returned AMEND, response ID `resp_0a12ead61fee5339016ac370498edc87d28ec6212dafa33cf9`. The archived raw SSE SHA-256 is `25b8c29265d327c34323e12f2c35177c2069c13b1d7e99f83a42ffea75240a58`; archive manifest SHA-256 is `46f41c9af22a6b1c9e86e2a66682dfd83f7323fb25d1c47aded2aa52c95e6782`. The r03 AMEND is advisory; its one-call consent is consumed and grants no implementation, build, runtime, IPC/VM, or D3 authority. Raw SSE SHA-256 `14f95d55a4a5611f518a0e236c98edd255e6809acd256b105177f975577bef09`; parsed-response SHA-256 `6fae28a46fd385e0d5aa87e8a1bc7149c832fcd8878170b470dab558941bc587`; archive manifest SHA-256 `b26c7767c9a74f2c9ee716a59257f79064c5480191addbc5f61da998989ca5fd`. The packet’s “NOT submitted” line is a time-frozen pre-review snapshot; this paragraph records the later submission/result. The v1.6 r04 packet SHA-256 `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829`, tied to the v1.6 SPEC SHA-256 `3296530c13801de945ed97b920532533c1a4a44d628075a9c5e61cdcc7ee01cb` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_0d91609fd866592e016ac3b3b903e487d2891f1ae2407fb075`). Raw SSE SHA-256 `a1c62af8a4e35a59aec3428fdf45818f2dd89b3f57dce9c3bf8be9a6279fe08d`; parsed-response SHA-256 `3655369b4c6c348e439ec87e48f6a8c1e789c46d5d4500890bee8577b8a969d3`; archive manifest SHA-256 `3f6dbf25e22ffd3ce5ce9290935fd41db162fd81aa67923c445b5ea250fbe2a8`. That exact-SHA consent is consumed. The review closed r03 findings 3 and 4b–4d at design level only; finding 4a remains must-fix. The v1.7 r05 packet SHA-256 `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a`, tied to SPEC v1.7 SHA-256 `b73bcc7361bb797b4ddd40952860f0092fb840bb11b31bf0a99659bef8475b70` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). Raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`; parsed-response SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`; archive manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a`. Usage: 22,569 input / 1,650 output / 24,219 total tokens; 8.11725 estimated credits (not USD); quota at return 5h 51%, weekly 86%. Three 4a must-fixes remain: preprocessor-forwarded and function-like observer macros; `-ansi` dialect override; Python assertions disabled by `PYTHONOPTIMIZE=1`. Optional hardening: actual `-o` target mapping, dynamic-linker conflict rejection, and stronger run-record schema `allOf`. Findings 3 and 4b–4d remain design-level closures only. The r05 consent is consumed. SPEC-039 v1.8 is local/unreviewed; any r06 review requires fresh exact-SHA consent. D2 remains blocked.

No VM/SSH/QGA operation, algorithmic production change, commit, push, or release is authorised by these gates. All Astra reviews to date were separately owner-authorised by exact packet SHA and their one-call consents are consumed; none expands the D2 seam/runtime scope. The old NG-062 trial locks and evidence remain untouched.

## 5. Files and evidence

| Path | State/intent |
|---|---|
| `Specs/SPEC-039-common-performance-measurement.md` | v1.16 same-SPEC response to r13 AMEND; preserves immutable r12/r13 submissions and defines the bounded read-only journal-history exception for COUT-001–004. Documentary/synthetic only. Dedicated-UID launcher and diagnostic no-unlink behavior remain unimplemented/unprovisioned. Any Astra call and execution require separate authorization. |
| `Common/src/GW.cpp`, `Common/src/GW.h` | Read-only under D0; a D2 seam may touch only the explicitly frozen compile-time-gated scope in §4. |
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No modifications to NGAL, `Process`, `PGCS` or `PG`, no algorithm change, separate source helper file, or D3 timing files are proposed. The D2 harness may construct the existing `Process/GW` under test; the same allowlisted harness binary's `--finalize` mode runs after the test exits without constructing `Process/GW`, creating IPC resources, or sending/receiving production messages. In `normal_cleanup` mode, it requires a durable `RUN_SHUTDOWN_COMPLETE` journal event proving worker joins and `Process/GW` destruction, plus a validated required `supervisor-termination.json` artifact containing one raw `waitpid` receipt per recorded process and stdout/stderr closure state/hashes; only then may it read inventories, inspect exact identities, and perform journal-authorized `IPC_RMID`/`sem_unlink` under the continuously verified lease. In `abnormal_evidence_only` mode, only while trusted launcher, execution/result-directory identities and lease remain valid and a valid supervisor receipt proves every process reaped and logs closed, the finalizer may hash raw, safely readable allowlisted evidence without requiring the complete bundle or parsing/replaying non-control artifacts. It writes `INCONCLUSIVE`; `failure.missing_artifacts` and `failure.unavailable_artifacts` are disjoint, accurate sets; cleanup history is exactly one of `not_started`, `partial`, `complete` or `unknown`, derived from valid evidence and never forced to `not_started`. It performs no IPC inspection, cleanup, journal mutation or retry. If lease, receipt, process/log proof or result-directory authority is unconfirmed, leave the pending record and report `BLOCKED/INCONCLUSIVE`. The trusted dedicated-UID launcher/lease is a provisioned external runtime prerequisite, not part of the five-file code allowlist; it does not exist here and direct host-UID execution is prohibited. |
| Isolated D1-H artifacts | Historical manifest `tests/spec039/q0_q1.manifest.json` SHA-256 `9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a`; source file hashes match. A fresh compile/link-only rebuild on 2026-10-10 used the unchanged isolated baseline checkout and an output root under `workspace/ng039-d1h-rebuild-20261010/` (outside `.hermes`): configure/build exit 0, CMake 3.28.3, GNU C++ 13.3.0, binary SHA-256 `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`, configure-log SHA-256 `65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb`, build-log SHA-256 `658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98`. This is a new build, not recovery of the old binary; the executable was not run, and this proves no correctness/performance behavior. |
| `Specs/RESULTS-SPEC-039/` | Contains historical r06/r07/r08 review packets and responses plus the D1-H build-only receipt; none is a D2/D3 runtime result. The v1.12/r09 and v1.13/r10 packets were submitted and returned `AMEND`; the v1.14/r11 draft is documentary only. No correctness or timing result exists. Historical AIOPT3 results remain in the NG-050 archive. |
| Obsidian NG-039 / NG-040 | Scope and blockers; neither an experimental oracle nor build evidence. |

## 6. Rollback and open questions

If the approved D0 design is later abandoned, mark this Proposal as abandoned/superseded instead of deleting it; leave the archive, current AIOPT5 diffs and historical trial evidence untouched. Later test-only changes must have an explicit allowlist and a reversible isolated checkout.

Before D2 code: freeze the compile-time-gated observer, exact five-file allowlist, in-memory future-message admission through `PushToInputQueue`, diagnostic no-unlink behavior, dedicated-UID launcher identity/lease, journal/inventory schemas and §7.1 cleanup oracle in a new pre-code packet. Obtain fresh exact-SHA consent before Astra. Do not apply the diff until positive documentary review and a separate execution decision; launcher provisioning is also a distinct prerequisite. Q0 distinguishes zero application offers from bootstrap queue state; Q1 uses a future `Message::Time` created locally because wire serialization does not carry that scheduling field. Cleanup must reconcile the Process-owned pending test message only after Gateway/output workers stop. The owner’s D2 scope is one correctness-only local run and does not include NGAL ingress coverage, D3 or algorithm changes. Q2–Q4 remain deferred; do not import approval or measurements from archived AIOPT3 SPECs.

## 7. D2a closure amendment — v1.8–v1.16 (documentary; not implementation authorization)

This section preserves the unchanged D2 correctness scope and the r08–r13 review history. The v1.12/r09 packet's IPC-boundary B1 was resolved in v1.13/r10. The v1.14 amendment separated normal cleanup from abnormal evidence-only finalization. The v1.15 amendment formalized the journal lifecycle/outcome model and resolved the r12 orphan-outcome and projection-drift findings. The v1.16 amendment resolves the r13 policy contradiction by separating bounded journal-history replay from other non-control evidence handling. No D2 code, configure/build, runtime or IPC is authorized by this documentary amendment.

### 7.1 Host-controlled execution profile — v1.14 baseline with v1.16 journal-policy amendment (documentary; execution blocked)

The v1.11 direct launch under the ordinary host UID is prohibited for D2. Mode `0600`, umask `0077`, exclusive creation and a pre-inventory do not prevent a same-UID process from replacing a POSIX semaphore name or removing/reusing a SysV resource during the run. This amendment does not claim the current host can satisfy the stronger prerequisite. No D2 configure/build, harness/finalizer execution, IPC creation/removal or measurement may occur until the dedicated single-run identity and launcher are provisioned, independently inspected, pinned in the manifest, and separately authorised by the owner. If they cannot be provided, the D2 execution profile remains `BLOCKED` before any candidate resource is created.

The run and its sequential finalizer must execute under a dedicated non-login UID reserved solely for this D2 run, through a trusted owner-controlled single-run launcher. The launcher must prevent another process from entering that UID's execution domain from before pre-inventory until finalization/lease release, hold an exclusive run lease across launch and finalization, and record its exact path, file identity/hash, UID/GID, lease identity and process-tree identity. A process census alone is not proof of exclusivity. The ordinary interactive UID, an unverified cooperative lock, or a missing launcher/lease is insufficient. Only controlled supervisor/harness/finalizer descendants for this run may use the UID; no login shell, unrelated service or concurrent run may use it. Treat host root/administrator as trusted; if root-level concurrent mutation is in the threat model, this shared-host profile is unacceptable and remains blocked. Record precondition evidence in the run record. Loss or uncertainty of identity/lease is `INCONCLUSIVE`, prohibits further name-based unlink/RMID, and preserves residue for owner-directed recovery. The launcher is outside the five-file code allowlist and has not been provisioned or authorised by this documentary amendment.

The three pinned `GW.cpp` paths at lines 970, 1263 and 1322 call `sem_unlink(name)` when `sem_open` fails. In the future diagnostic build only, the already-listed `NG_GW_QUEUE_TEST_OBSERVATION` compile-time guard must suppress all three unlinks and emit a bounded failure event; the normal macro-off build remains unchanged. Failure to open a semaphore aborts the diagnostic run; only the separate finalizer may clean a resource, and only from a complete reservation journal under a still-valid dedicated-UID lease. Synthetic fault-injection coverage must prove each path makes zero `sem_unlink` calls. This required source behavior is design text only; code changes still need separate owner authorisation.

Before constructing `Process/GW`, capture complete host-visible inventories for `/proc/sysvipc/shm`, `/proc/sysvipc/sem`, `/proc/sysvipc/msg`, and `/dev/shm`. Global inventories may be non-empty, but every candidate key/name must be absent. Capture each raw `/proc/sysvipc` table byte-for-byte in canonical base64 fields inside its phase inventory artifact; the finalizer decodes those bytes, recomputes each SHA-256 over the exact raw bytes, parses the table using the pinned Linux table grammar, and independently recomputes normalized rows/IDs before comparing them to stored summaries. Reject non-canonical base64, duplicate/invalid rows, digest mismatch, parse mismatch, unreadable/over-cap input or any inventory artifact exceeding its bound as `INCONCLUSIVE` before resource creation. The `/dev/shm` structured listing is sorted by bytewise name and retains each entry's kind, device, inode, UID/GID, mode, link count and size; the finalizer compares actual post/final listings with captured records. Inventory incompleteness never means “empty”.

Choose `D2_PROCESS_KEY` deterministically: for attempts `0..31`, compute `0x40000000 + (uint32_be(SHA256(ASCII(run_id + ':' + decimal(attempt)))[0:4]) mod 0x3FFFFFF8)` and select the first base whose four-key range is unoccupied and fits positive signed 32-bit `key_t`; key `11` is checked separately. Record the selected attempt and candidate set `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. If no base is available, abort `INCONCLUSIVE`.

Before any candidate create, create `reservation-journal.jsonl` in the run result directory using exclusive non-symlink creation and mode `0600`; fsync the file and containing directory. This file is a required runtime artifact, capped at 256 KiB and 128 events, one strict JSON object per newline-terminated record, with monotonically increasing sequence numbers and the same run ID; use `ng039-reservation-journal-event/v2` for the event schema. Each event contains schema version, sequence, event kind, monotonic timestamp, process PID/UID, resource kind, candidate key/source name, and nullable identity/result fields. Event kinds are `JOURNAL_OPENED`, `CREATE_INTENT`, `CREATED`, `CREATE_FAILED`, `RUN_SHUTDOWN_COMPLETE`, `CLEANUP_INTENT`, `REMOVED`, `CLEANUP_FAILED`, and `JOURNAL_CLOSED`. Append/fsync `RUN_SHUTDOWN_COMPLETE` exactly once only after all Gateway/output workers have stopped and joined and `Process/GW` has been destroyed; it must precede every `CLEANUP_INTENT`. Write and fsync `CREATE_INTENT` before each `shmget`/`sem_open`; after the call, append and fsync exactly one `CREATED` record with returned identity or `CREATE_FAILED` with errno. Keep the descriptor open, verify its file identity before each append, and never rewrite/truncate prior events. In normal cleanup, the finalizer appends/fsyncs each `CLEANUP_INTENT` before cleanup and `REMOVED`/`CLEANUP_FAILED` after. It may close the journal only after final inventory capture and all events are durable. Abnormal evidence-only finalization never appends to, repairs or closes this journal. A missing event, sequence gap/duplicate, partial line, identity mismatch, crash between intent and outcome, or failure to fsync makes the run `INCONCLUSIVE`; the finalizer must not infer success or retry deletion for an ambiguous resource.

After pre-inventory and journal readiness, set harness umask to `0077`; exclusively reserve five SysV SHM keys with `shmget(..., IPC_CREAT|IPC_EXCL|0600)` at pinned `MaxSegmentSize` and five exact POSIX semaphore names with `sem_open(..., O_CREAT|O_EXCL, 0600, 1)`. Verify effective modes are exactly `0600`. Each `CREATED` event records exact key/name and exclusive-create evidence: for SHM, `shmid`, key, `IPC_STAT` size, creator PID/UID, owner UID and mode; for semaphore, exact source name, `sem_t*` address, `/dev/shm` resolved path, device/inode/UID/GID/mode/link count, and libc identity. A failed/mismatched reservation aborts before `Process/GW` construction; remove only resources whose successful journal receipt and current identity are exact while the lease remains valid. Otherwise do no deletion and report `INCONCLUSIVE`.

For each semaphore, record the `sem_open` provider via `dladdr`, resolved libc path, file device/inode, GNU build ID when available, SHA-256 of the opened libc file, `gnu_get_libc_version()`, and exact production source name/path. Immediately after successful exclusive `sem_open`, capture `/proc/self/maps` and prove the returned `sem_t*` address lies within a shared mapping whose device/inode equals the `stat` identity of exact `/dev/shm/sem.<decimal-name>`. Retain the exact matching maps line or a strict projection including address range, permissions, offset, device and inode in the reservation journal. Recompute/validate this linkage from retained evidence; version/path plus a boolean is insufficient. The mapping procedure is target-libc-specific and is not claimed tested here. If `sem_open` returns no handle, mapping is absent, path/device/inode disagree, or mapping cannot be captured, abort before `Process/GW`; do not set `semaphore_name_mapping_verified=true`, do not PASS, and do no speculative deletion.

After Gateway/output workers stop and join and `Process/GW` are destroyed, the harness must append/fsync exactly one `RUN_SHUTDOWN_COMPLETE` event to the still-open reservation journal. This event is the positive proof that normal shutdown completed; a caller boolean or process exit is not a substitute. The trusted supervisor then waits/reaps every recorded harness process PID and closes stdout/stderr before invoking the same allowlisted `--finalize` binary. It selects `normal_cleanup` only when the journal contains exactly one valid shutdown-complete event, every recorded process has a verified wait receipt, both logs are closed, no timeout/signal/ambiguous termination occurred, and the lease remains continuously valid. In this mode the finalizer captures post inventory, validates/replays the journal, reads required inventories, inspects exact current identities (`shmctl(..., IPC_STAT, ...)` for SysV SHM and `stat`/equivalent metadata for the exact POSIX semaphore object), and removes only journal-proven run-created resources. For each resource it appends/fsyncs `CLEANUP_INTENT`, rechecks lease and identity immediately before the destructive call, invokes only exact `shmctl(exact_shmid, IPC_RMID, ...)` or `sem_unlink(exact_recorded_name)`, then appends/fsyncs the matching outcome. The `normal_cleanup` gate above is satisfied only while the trusted launcher, supervisor receipt, execution identity, result-directory identity and continuous lease remain valid through final-record closure. If normal-shutdown proof is absent while these common authority/file-safety conditions remain valid, the finalizer may choose `abnormal_evidence_only`. Loss or ambiguity of the lease, supervisor proof, execution identity or result-directory authority is not an abnormal-mode trigger: do not invoke or re-enter the finalizer; preserve the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A failure during normal cleanup after any `CLEANUP_INTENT` stops further destructive calls and retries; while authority remains valid, record `INCONCLUSIVE` with the evidence-derived cleanup outcome in the current finalization, otherwise retain pending recovery state. No ambiguous deletion is retried.

In `abnormal_evidence_only` mode, only while the trusted launcher, execution identity, result-directory identity and continuous lease remain verified, and after a valid `supervisor-termination.json` proves every recorded process was reaped and both logs were closed, the finalizer strictly validates the pending run record and supervisor receipt. It may then open only allowlisted evidence names using no-follow regular-file checks, preserving and hashing safely readable closed raw bytes within per-file caps; it does not require all eight evidence files or enforce the normal complete-bundle/schema checks. `failure.missing_artifacts` lists required evidence names actually absent; `failure.unavailable_artifacts` lists required names present but unreadable/unsafe/over-cap. These sets are disjoint, exclude the run record and optional `harness-diagnostics.txt`, and neither may falsely classify a present file as missing. Malformed but safely readable non-control evidence is preserved and hashed, not parsed or used to block an otherwise permitted `INCONCLUSIVE` record. The bounded journal-history classifier validates event shape, sequence, phase/resource transitions and the stable resource-identity binding defined by §7.2; it is not a full runtime finalizer. `failure.cleanup_outcome` is derived only by the §7.2 `COUT-001`–`COUT-004` rules. Invalid, malformed, truncated, orphan, identity-mismatched or uncovered-resource history yields `unknown`. Abnormal mode always sets `record_finalized=true`, `finalization_mode=abnormal_evidence_only`, and `status=INCONCLUSIVE`, including when the historical cleanup outcome is `complete`. A cleanup outcome never proves final inventory or PASS. The abnormal finalizer performs no new cleanup, does not retry or mutate the journal, and preserves its bytes. If the lease, receipt, process/log proof or result-directory authority fails at any point, do not write/re-enter finalization; keep the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A present but partial, stale, or shutdown-unverified `ipc-post.json` or `ipc-final.json` may be preserved and hashed as a file, but must not be treated as a verified inventory: the run-record post/final inventory summaries remain unavailable, with `is_empty=null` and no claimed inventory digest. The finalizer never reads `/proc/sysvipc` or `/dev/shm`, calls `IPC_STAT`, `IPC_RMID` or `sem_unlink`, appends/repairs/closes the journal, or performs cleanup during this mode.

In `normal_cleanup` mode, capture a complete final inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required runtime artifact exactly once. The finalizer strictly parses journal/inventories, replays the §7.2 transition/identity model, recomputes raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. PASS/FAIL require the complete bundle and safe normal cleanup; missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. If failure occurs after any `CLEANUP_INTENT`, stop subsequent destructive calls and retries; while authority remains valid, derive `cleanup_outcome` from the closed, transition-valid journal under §7.2. `partial` requires every created resource to have one terminal cleanup outcome and at least one proven `CLEANUP_FAILED`; an uncovered or ambiguous resource is `unknown`. If a complete closed journal proves every created resource was removed and no cleanup failed, preserve `cleanup_outcome=complete` even when a later evidence/inventory/final-record check fails; the overall run remains `INCONCLUSIVE` unless all normal acceptance gates pass. Cleanup history never upgrades PASS or substitutes for inventory evidence. If authority or safe record writing is lost, leave pending/partial evidence and report `BLOCKED/INCONCLUSIVE`; never re-enter finalization to claim `not_started`. The abnormal evidence-only path preserves the historical cleanup outcome and performs no further cleanup. D2 remains one controlled correctness run only; no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.

1. Host-controlled preflight before construction: the run starts only after the §7.1 inventory and attribution prerequisites pass. No constructor-time IPC may precede that successful preflight.

2. HT construction, Message lifetime, and failure-safe ownership: after the host-controlled preflight succeeds, construct the isolated non-`PGCS` `Process`. Its pinned constructor calls `NewBlock("HT", ...)` then `NewBlock("GW", ...)` (`Process.cpp:228–230`), and the `GW` constructor retrieves that HT (`GW.cpp:99–101`) before initialization dereferences it (`GWRunInitialization01.cpp:68–80`). Do not call `NewBlock("HT")` or `NewBlock("GW")` again; verify exactly two blocks exist and one lookup succeeds for each name. Before starting Gateway, create the Process-owned target, capture `GetDeleteFlag()==false` while single-threaded, and bind its ID with the observer. Capture the immutable scalar `Message::InstantiationNumber` (`Message.h:158`) before each production `Run`; use only that copied ID for Run-entry and Run-return events, publishing the return event before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after a possible reclamation; duplicate, zero, stale or wrapped identity is INCONCLUSIVE. The three-argument constructor used by `Process::NewMessage` initializes `Delete=false` and `ApplicationDeleted=DELETED_BY_CORE` (`Message.cpp:37–70`, especially 63–64); `DeleteMarkedMessages` deletes only true-flagged messages (`Process.cpp:885–917`). Keep the target out of the queue and do not mark it through Q0; use exactly three inert command lines `-ng039 --probe 0.2`, which match neither the registered action nor the `0.1` fallback in `Block::Run` (`Block.cpp:244–313`). For the expected Q0/Q1 pending path, after Gateway and its output worker stop/join, verify no target pop/Run and `Delete=false`; call `EraseMessage` only while Process is quiescent and require `GetNumberOfMessages()` to decrease by exactly one. Keep the detached object alive through `Process` destruction: its destructor calls `DeleteMessages()` before deleting `Blocks`, so after EraseMessage it will not own/delete this target; `GW::~GW` destroys actions, while the queue container drops raw pointer values. Delete the detached target exactly once after Process/GW destruction. If any target pop/Run, delete flag, count, or identity makes ownership uncertain, never dereference, erase, delete, or invoke the C++ Process/GW destructors on a possibly stale pointer: write a bounded/fsynced `run-record.pending.json` with `record_finalized=false` and no fabricated measurements, exit nonzero via `_Exit`, and let the isolated process/namespace terminate. The trusted supervisor must wait/reap every PID in the frozen harness process tree, close stdout/stderr, and write/fsync the required `supervisor-termination.json` containing exact per-PID raw `waitpid` statuses, log-closure state, and hashes of available closed logs. Only after every PID is reaped and both logs are closed may the parent-side finalizer in item 5 run `abnormal_evidence_only`: it writes a final `INCONCLUSIVE` record with unavailable post/final inventories and every missing artifact explicit, leaves the journal byte-for-byte as found, and performs no IPC inspection or cleanup. Absence of `RUN_SHUTDOWN_COMPLETE` forbids normal cleanup; process exit does not prove worker join or `Process/GW` destruction. A partial construction, blocked join, timeout, signal, or uncertain ownership follows this same no-cleanup path. If termination receipts or log closure are incomplete, do not invoke the finalizer or release the lease; keep the pending record and classify the gate `BLOCKED/INCONCLUSIVE`.

3. Fresh, correlated observation contract: keep the 64-bit event sequence and add a saturating `gateway_iteration_id`, assigned once at the start of each Gateway loop iteration and carried across its wait and due/pop decision. Preserve two distinct sticky scalar records: (a) while the existing `InputQueueMutex` lock is held when the real `wait_for` returns, if the registered target is the head, capture `WaitCapture{sequence, iteration_id, target_id, head_id, head_time, depth, timeout_ms, predicate_return}` once and never overwrite it; (b) while the production due/pop critical-section lock is held, capture the corresponding `DecisionCapture{sequence, iteration_id, candidate_id, candidate_time, actual_due_branch, actual_popped}` from the existing strict comparison/pop branch, without performing a second comparison, and never overwrite that capture. The records correlate only if the iteration IDs match, `decision.sequence > wait.sequence`, and the wait target/head ID matches the actual decision candidate. Set `correlation_status=MATCHED` only then. Otherwise use `PENDING`, `DECISION_NOT_REACHED`, `HEAD_MISMATCH`, `INVALID`, or `OVERFLOW`; no incomplete pair is exposed as a valid due result. Stop before the decision, a changed head, invalid/duplicate/wrapped ID, missed event, stale sequence, or counter overflow makes Q1 BLOCKED/INCONCLUSIVE, never zero/PASS. Under Q0/Q1, capture sequence/counter baselines at each window start: Q0 requires a fresh empty-observation delta inside its 100 ms no-controlled-offer window after bootstrap drain; Q1 requires target admission/head identity, a matched wait/decision pair newer than the Q1 baseline, zero target pops/Run entries/returns throughout 100 ms, and a future target time at end. Use `std::chrono::steady_clock` for correctness windows; `Message::Time` remains the scheduler clock. Set `gateway_started=true` under `InputQueueMutex` before the loop and never reset it; later registration fails. Before production `Run`, copy `Message::InstantiationNumber` to a scalar; emit Run-entry/return with that copy and publish the return before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after possible reclamation. These are correctness-window rules, not timing/performance evidence.

4. Bounded source/build verification contract: the five-path code allowlist and the CMake target census remain separate checks. The diagnostic graph contains exactly `Common` and `ng039_gw_q0_q1_d2`; the normal graph contains the pinned default targets, with the observer macro absent. The root CMake file returns before production application targets in the diagnostic branch. The compile-database checker receives the database, profile, pinned worktree root and fresh build root as four positional inputs after `python3 -c <checker>`; do not insert a `--` separator unless the checker explicitly consumes it. It verifies that each compile entry's working directory and actual `-o` output resolve under the build root, the source operand and entry `file` resolve to the same regular source inside the worktree, any `output` metadata agrees with `-o`, exactly one `-c` occurs, and compiler realpath/target census match the manifest. Require exactly one `-std=gnu++20`, only `-O0` optimization tokens, and the observer define exactly once on `Common` and the harness in diagnostic mode and nowhere in default mode. Parse joined and split `-D`/`-U` switches by base identifier, including function-like macro spellings; reject every function-like or undefined `NG_GW_QUEUE_TEST_OBSERVATION` form. Reject response files, `-Wp`, `-Xpreprocessor`, `-ansi`, `-traditional`, `-x`, forced-include and macro-file forms in both split (`-include file`, `-imacros file`) and joined (`-includefile`, `-imacrosfile`) spelling, plus toolchain-selection and plugin forms that can bypass those checks. Do not reject unrelated warning/debug switches through a purported universal grammar; preserve every complete argument vector and the compile database SHA-256 as evidence. This checker is scoped to the pinned CMake output and is not a generic validator for arbitrary compile databases.

Resolve the roots before configure. Use fresh, pairwise-disjoint source/build/results paths beneath `<WORKSPACE_ROOT>`; never place NovaGenesis source, build or result artifacts in `.hermes`. Invoke configure, build and both checkers through the same minimal `env -i` environment, retaining only explicit `PATH`, `HOME`, `TMPDIR`, `LANG` and `LC_ALL` values. Set `HOME` and `TMPDIR` to distinct directories under the named D2 scratch root. Record those retained values, command working directories, tool versions and exact commands in the manifest/run identity; inherited environment variables are not passed through. No broad environment-variable inventory or custom denylist is required.

For link validation, invoke `python3 -c <checker> <link.txt> <build-root> ng039_gw_q0_q1_d2` without an unconsumed `--`; the checker opens that exact named `link.txt` itself, requires a regular non-symlink file at the expected diagnostic target path under the build root, and never validates stdin or another caller-supplied string. Parse its contents with `shlex.split(posix=True)`, verify the pinned compiler realpath and exactly one standalone `-static`, `-static-libgcc`, and `-static-libstdc++`, and reject response files plus direct dynamic-link modes. For this target, reject `-Wl` and `-Xlinker` entirely rather than normalizing every forwarding grammar; this rejects `-Wl,-shared` and `-Xlinker -B -Xlinker dynamic` without a general linker-option parser. The checker uses explicit nonzero failures, not Python `assert`.

Freeze only the directly relevant synthetic cases: source/output path escape, mismatched compile metadata, repeated `-c`, wrong compiler, observer macro via `-Wp`/`-Xpreprocessor`, joined `-include`/`-imacros` override, alternate standard or optimization, wrong link-file path/content, missing/duplicate required static flag, `-Wl,-shared`, and the split `-Xlinker` dynamic override. For both literal checker entrypoints, test one complete accepted invocation and a missing-operand rejection, in normal Python and `PYTHONOPTIMIZE=1`; test the two joined forced-include negatives in the same modes. No real D2 configure/build/run is implied by these synthetic tests.

5. Bounded run-record and dual-mode finalizer contract: `tests/spec039/q0_q1_d2.manifest.json` defines strict Draft 2020-12 schemas for the pending/final run record v5, `supervisor-termination.json` receipt v1, IPC inventories, observer events and `reservation-journal.jsonl` events v2. The finalized run record's `finalization_mode` is exactly `normal_cleanup` or `abnormal_evidence_only`; pending records use null, and the finalizer derives the mode from journal and supervisor evidence rather than a caller label. The same allowlisted `--finalize` binary is the sole semantic authority. Both finalization modes require a strictly valid pending run record and supervisor receipt; every recorded PID reaped; closed stdout/stderr; unchanged execution/result-directory identities; and a dedicated-UID lease continuously held through final-record closure. If any common prerequisite is absent, ambiguous or lost, do not invoke/re-enter the finalizer or write a final record; retain pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. In `normal_cleanup`, strictly parse/replay all required JSON/JSONL, verify raw `/proc/sysvipc` hashes/rows, exact artifact set, wait/log receipt, lease and semaphore mapping, then inspect exact identities and perform only journal-authorised cleanup after `CLEANUP_INTENT` fsync and immediate identity/lease recheck. `RUN_SHUTDOWN_COMPLETE` must be durable after joins/destruction and before any cleanup intent. PASS/FAIL require the full normal evidence and inventory gates. If cleanup fails after an intent, stop further destructive calls/retries and derive the historical `cleanup_outcome` from §7.2: `partial` requires full created-resource terminal coverage plus a proven `CLEANUP_FAILED`; an uncovered or ambiguous resource is `unknown`. A complete closed journal may retain `cleanup_outcome=complete` if a later inventory/evidence/final-record check fails, but overall status is `INCONCLUSIVE` unless all normal acceptance gates pass. If the final record cannot safely be written, leave pending recovery evidence.

In `abnormal_evidence_only`, normal shutdown proof may be absent, but all common prerequisites still hold. Strictly validate the pending record and supervisor receipt; for other allowlisted evidence files, use no-follow regular-file checks, preserve/hash safely readable closed raw bytes within bounds, but do not require the full bundle or parse/replay inventory, observer or other non-control schemas. The reservation journal is subject only to the bounded read-only cleanup-history exception stated below, not to normal-bundle or inventory validation. Malformed but safely readable non-control evidence other than the reservation journal is preserved and hashed, not parsed or used to block a final `INCONCLUSIVE` record. The only parsing/replay exception is bounded read-only parsing/replay of `reservation-journal.jsonl` solely to derive `failure.cleanup_outcome` under `COUT-001`–`COUT-004`; invalid, unavailable, malformed, truncated or incomplete history yields `unknown` and does not block an otherwise authorized abnormal `INCONCLUSIVE` record. The bounded journal-history classifier validates event shape, sequence, phase/resource transitions and the stable resource-identity binding defined by §7.2; it is not a full runtime finalizer. `failure.cleanup_outcome` is derived only by the §7.2 `COUT-001`–`COUT-004` rules. `failure.missing_artifacts` is exactly the required evidence names actually absent; `failure.unavailable_artifacts` records required names present but unsafe, unreadable or over-cap. The lists are disjoint; neither includes the record itself or optional `harness-diagnostics.txt`. Bound raw-byte reads by the per-file and 32 MiB total caps; classify over-budget files unavailable instead of aborting final-record creation. Present post/final files may be preserved but their inventory summaries remain unavailable, never empty. This mode always yields `INCONCLUSIVE`, performs no IPC inspection, cleanup or journal mutation, and never retries. A cleanup intent or prior removal is reported truthfully, not rewritten as `not_started`.

The bounded semantic regression set must reject claimed PASS/FAIL for missing/unverified launcher or lease, a second/foreign process, diagnostic `sem_open` failure calling `sem_unlink`, pre-existing candidate keys/names, unreadable/over-cap/raw-parsed-mismatched normal inventories, noncanonical base64, missing/reordered/duplicated/unfsynced normal journal events, candidate identity/mode mismatch, missing `sem_t` mapping, residual candidates, unexpected host changes, or final-inventory mismatch. Reject normal cleanup without a durable `RUN_SHUTDOWN_COMPLETE`, complete supervisor wait receipts, closed logs and continuous lease. Reject invocation of either mode before all recorded PIDs are reaped and logs closed; when common launcher/receipt/lease/result-directory authority is absent, retain pending evidence and report `BLOCKED/INCONCLUSIVE`. Abnormal fixtures must accept malformed but safely readable non-control evidence as raw preserved/hashable bytes without requiring a complete bundle or parsing it; reject claims of PASS/FAIL, empty inventories, joins/destruction, optional diagnostics in `missing_artifacts`, overlap between `missing_artifacts` and `unavailable_artifacts`, or an inaccurate cleanup-history outcome. Journal-history fixtures follow §7.2: `not_started` requires a complete transition-valid closed journal with zero `CREATED` and zero `CLEANUP_INTENT` (a `CREATE_FAILED` may occur); `partial` requires one terminal cleanup outcome for every created resource and at least one `CLEANUP_FAILED`; `complete` requires every created resource exactly one `REMOVED` and no cleanup failure but never implies PASS; all other histories are `unknown`. Include schema-valid negatives for orphan `REMOVED` and `CLEANUP_FAILED` without cleanup intent, cleanup identity mismatch, pending intent, created-without-cleanup and one resource left uncovered while another is cleaned. Abnormal mode must never retry cleanup or mutate journal/IPC. The finalizer must perform no speculative deletion for disputed resources. Normal PASS/FAIL requires a complete, replayable journal and every required evidence artifact; the normal bundle has eight evidence artifacts plus `run-record.json`, optional `harness-diagnostics.txt`, and a ten-file maximum. Normal artifacts are each ≤4 MiB, the journal ≤256 KiB/128 events, the final run record ≤256 KiB, and the complete normal bundle ≤32 MiB. Abnormal raw preservation remains no-follow and bounded: per-file/total read caps apply, but excess/unsafe present files are recorded as unavailable rather than making strict schema/completeness validation a prerequisite to the `INCONCLUSIVE` record.

The host-controlled runtime shape remains one controlled D2 process tree under the dedicated UID, one Q0/Q1 correctness run, no network input and no performance timing. The launcher serializes the run and finalizer under the verified exclusive lease; the harness itself spawns no child process. The host IPC namespace may contain unrelated objects, but all candidate resources must be absent before reservation and all exact cleanup remains bound to the durable journal. No broad host cleanup is permitted. If the launcher/lease or result-directory identity is absent/lost, a second process enters the identity domain, or supervisor evidence is incomplete, do not perform name-based unlink/RMID or invoke/re-enter finalization; preserve pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. An abnormal record may be written only while common lease and file-safety authority remains valid.

**Next gate:** complete local v1.16/r14 verification of the policy model, all duplicate SPEC/manifest/packet projections, and the read-only checker; record exact hashes and evidence classes. The r13 one-call consent is consumed, and this amendment does not authorize another Astra call. Any later review requires a separate owner decision on the final exact packet SHA. D2 implementation/configure/build/run remains blocked until a fresh positive documentary review, provisioned launcher/preconditions and separate owner execution gate. D3 and Q2–Q4 remain deferred. This work is documentary/synthetic, not finalizer runtime, correctness or performance evidence.

### 7.2 Canonical lifecycle and outcome model — v1.16

The following versioned JSON object is the single normative model for journal transitions, resource identity binding, finalizer-mode eligibility and cleanup-history classification. The manifest's `classification_rules.journal_transition_model` and the review packet's marked model block must be structurally identical to this block; the read-only checker verifies those projections and related schema enums. Human-readable requirements use the stable IDs below. This is an offline contract model, not an operational finalizer or runtime evidence.

<!-- NG039-JOURNAL-MODEL-BEGIN -->
```json
{
  "model_id": "ng039-journal-finalization-model",
  "model_version": 2,
  "schema_version": "ng039-reservation-journal-event/v2",
  "journal_states": [
    "absent",
    "open",
    "shutdown_complete",
    "closed"
  ],
  "journal_initial_state": "absent",
  "journal_terminal_state": "closed",
  "resource_states": [
    "none",
    "create_pending",
    "create_failed",
    "created",
    "cleanup_pending",
    "removed",
    "cleanup_failed"
  ],
  "resource_initial_state": "none",
  "resource_kinds": {
    "journal": "journal",
    "candidates": [
      "sysv_shm",
      "posix_semaphore"
    ]
  },
  "identity_binding": {
    "sysv_shm": [
      "candidate_key",
      "shmid"
    ],
    "posix_semaphore": [
      "source_name",
      "device",
      "inode"
    ]
  },
  "required_events": [
    "JOURNAL_OPENED",
    "CREATE_INTENT",
    "CREATED",
    "CREATE_FAILED",
    "RUN_SHUTDOWN_COMPLETE",
    "CLEANUP_INTENT",
    "REMOVED",
    "CLEANUP_FAILED",
    "JOURNAL_CLOSED"
  ],
  "events": [
    {
      "requirement_id": "JRN-001",
      "event": "JOURNAL_OPENED",
      "scope": "journal",
      "phase_from": [
        "absent"
      ],
      "phase_to": "open",
      "resource_from": null,
      "resource_to": null
    },
    {
      "requirement_id": "JRN-002",
      "event": "CREATE_INTENT",
      "scope": "resource",
      "phase_from": [
        "open"
      ],
      "phase_to": "open",
      "resource_from": "none",
      "resource_to": "create_pending"
    },
    {
      "requirement_id": "JRN-003",
      "event": "CREATED",
      "scope": "resource",
      "phase_from": [
        "open"
      ],
      "phase_to": "open",
      "resource_from": "create_pending",
      "resource_to": "created"
    },
    {
      "requirement_id": "JRN-004",
      "event": "CREATE_FAILED",
      "scope": "resource",
      "phase_from": [
        "open"
      ],
      "phase_to": "open",
      "resource_from": "create_pending",
      "resource_to": "create_failed"
    },
    {
      "requirement_id": "JRN-005",
      "event": "RUN_SHUTDOWN_COMPLETE",
      "scope": "journal",
      "phase_from": [
        "open"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": null,
      "resource_to": null,
      "requires_no_resource_states": [
        "create_pending"
      ]
    },
    {
      "requirement_id": "JRN-006",
      "event": "CLEANUP_INTENT",
      "scope": "resource",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": "created",
      "resource_to": "cleanup_pending"
    },
    {
      "requirement_id": "JRN-007",
      "event": "REMOVED",
      "scope": "resource",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": "cleanup_pending",
      "resource_to": "removed"
    },
    {
      "requirement_id": "JRN-008",
      "event": "CLEANUP_FAILED",
      "scope": "resource",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "shutdown_complete",
      "resource_from": "cleanup_pending",
      "resource_to": "cleanup_failed"
    },
    {
      "requirement_id": "JRN-009",
      "event": "JOURNAL_CLOSED",
      "scope": "journal",
      "phase_from": [
        "shutdown_complete"
      ],
      "phase_to": "closed",
      "resource_from": null,
      "resource_to": null,
      "requires_no_resource_states": [
        "create_pending",
        "cleanup_pending"
      ]
    }
  ],
  "authority_modes": {
    "requirement_ids": {
      "common": "AUTH-001",
      "normal": "AUTH-002",
      "abnormal": "AUTH-003",
      "blocked": "AUTH-004",
      "abnormal_journal_history": "AUTH-005"
    },
    "common_requirements": [
      "trusted_launcher",
      "valid_supervisor_receipt",
      "all_pids_reaped",
      "logs_closed",
      "stable_result_directory",
      "continuous_lease"
    ],
    "normal_requirements": [
      "durable_shutdown_complete",
      "normal_waits_valid",
      "no_timeout_or_signal"
    ],
    "mode_names": {
      "normal": "normal_cleanup",
      "abnormal": "abnormal_evidence_only",
      "blocked": "blocked"
    }
  },
  "mode_effects": {
    "normal_cleanup": {
      "requirement_id": "AUTH-002",
      "status": [
        "PASS",
        "FAIL",
        "INCONCLUSIVE"
      ],
      "requires": [
        "full_bundle_schema_journal_replay",
        "complete_inventory_verification",
        "exact_journal_authorized_cleanup"
      ],
      "forbidden": [
        "cleanup_before_shutdown_complete",
        "broad_host_cleanup"
      ]
    },
    "abnormal_evidence_only": {
      "requirement_id": "AUTH-003",
      "status": [
        "INCONCLUSIVE"
      ],
      "requires": [
        "common_authority_gates_true",
        "strict_pending_record_supervisor_receipt",
        "bounded_raw_evidence_preservation",
        "attempt_bounded_read_only_journal_history_classification"
      ],
      "forbidden": [
        "live_ipc_inspection",
        "cleanup",
        "journal_mutation",
        "retry",
        "PASS",
        "FAIL"
      ],
      "journal_history_policy": {
        "requirement_id": "AUTH-005",
        "artifact": "reservation-journal.jsonl",
        "access": "bounded_read_only_parse_replay",
        "purpose": "derive_cleanup_outcome_only",
        "outcome_requirement_ids": [
          "COUT-001",
          "COUT-002",
          "COUT-003",
          "COUT-004"
        ],
        "invalid_or_unavailable_history": "unknown",
        "unknown_history_blocks_authorized_inconclusive_record": false,
        "strict_pending_and_supervisor_records": true,
        "other_non_control_artifacts": "bounded_raw_preservation_without_schema_parse_or_replay",
        "normal_bundle_schema_inventory_gate_required": false,
        "journal_mutation": false
      }
    },
    "blocked": {
      "requirement_id": "AUTH-004",
      "status": [
        "BLOCKED",
        "INCONCLUSIVE"
      ],
      "requires": [
        "preserve_pending_evidence"
      ],
      "forbidden": [
        "finalizer_entry",
        "finalizer_reentry",
        "cleanup",
        "unsafe_record_write"
      ]
    }
  },
  "cleanup_outcomes": {
    "not_started": {
      "requirement_id": "COUT-001",
      "journal_required": "complete_transition_valid_closed",
      "created_count": 0,
      "cleanup_intent_count": 0,
      "create_failed_allowed": true,
      "run_status_effect": "NO_PASS_INFERENCE"
    },
    "partial": {
      "requirement_id": "COUT-002",
      "journal_required": "complete_transition_valid_closed",
      "all_created_have_one_terminal_outcome": true,
      "cleanup_failed_count_min": 1,
      "run_status_effect": "INCONCLUSIVE"
    },
    "complete": {
      "requirement_id": "COUT-003",
      "journal_required": "complete_transition_valid_closed",
      "created_count_min": 1,
      "all_created_removed": true,
      "cleanup_failed_count": 0,
      "run_status_effect": "DOES_NOT_IMPLY_PASS"
    },
    "unknown": {
      "requirement_id": "COUT-004",
      "journal_required": "otherwise",
      "run_status_effect": "INCONCLUSIVE"
    }
  },
  "normal_cleanup_failure": {
    "requirement_id": "FIN-001",
    "stop_destructive_calls_after_failure": true,
    "retry_after_intent": false,
    "outcome_source": "complete_transition_valid_closed_journal",
    "complete_history_after_journal_closed_later_evidence_failure": {
      "cleanup_outcome": "complete",
      "run_status": "INCONCLUSIVE"
    },
    "authority_loss": {
      "finalizer_entry": false,
      "finalizer_reentry": false,
      "preserve_pending": true,
      "status": "BLOCKED/INCONCLUSIVE"
    }
  }
}
```
<!-- NG039-JOURNAL-MODEL-END -->

`JRN-002` and `JRN-006` deliberately use separate pending states. `JRN-003`/`JRN-004` can consume only a create intent; `JRN-007`/`JRN-008` can consume only a cleanup intent after a successful create. `JRN-005` rejects unresolved create intents; `JRN-009` rejects unresolved intents but may close with an uncleaned `created` resource, which `COUT-004` classifies as `unknown`. For identity continuity, `CREATED`, `CLEANUP_INTENT` and the terminal cleanup event must carry the same stable binding: SysV SHM uses `(candidate_key, shmid)`; a POSIX semaphore uses `(source_name, device, inode)`. Other identity attributes (owner/mode/libc/mapping) remain separate required checks in the normal finalizer; these model bindings do not replace them.

`AUTH-001` is a common gate for both finalizer modes. If any common launcher, receipt, reaping, log-closure, result-directory or continuous-lease requirement is false or unknown, choose `blocked`, do not invoke/re-enter the finalizer, and preserve pending evidence. With common authority intact, `AUTH-002` selects normal cleanup only when every normal gate is true; otherwise `AUTH-003` permits bounded abnormal evidence preservation only, ending `INCONCLUSIVE` with no IPC inspection, cleanup, journal mutation or retry. `COUT-001` through `COUT-004` describe cleanup history, not overall acceptance. `FIN-001` requires stopping destructive actions and retries after failure; a complete closed history remains `complete` if a later evidence/final-record step fails, while run status remains `INCONCLUSIVE`.

## 8. Revision history

| Version | Date | Change |
|---|---|---|
| v1.1 | 2026-10-04 | Owner-approved documentary D0 contract for Q0/Q1 only. |
| v1.2 | 2026-10-04 | Recorded the separately bounded D2 seam/correctness scope, D1-H compile/link result, and pre-code Astra checkpoint; the exact-SHA review returned HOLD. No D2 code/runtime followed. |
| v1.3 | 2026-10-05 | First local draft of the same-SPEC documentary closure requirements; superseded by v1.4 after a source-grounded HT construction prerequisite was identified. |
| v1.4 | 2026-10-05 | Same-SPEC closure draft with HT-before-GW prerequisite. The exact pre-code packet SHA `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948` was reviewed by GPT-6 Astra and returned AMEND. No D2 code/runtime followed. |
| v1.5 | 2026-10-05 | Same-SPEC documentary closure response to the v1.4 AMEND. Sanitized r03 packet SHA `7121fa9307beb790ff093319d6d9bc54fb9d296bf68d8c705647fbcebf740797` was reviewed by GPT-6 Astra and returned AMEND. No D2 code/runtime followed. |
| v1.6 | 2026-10-05 | Same-SPEC response to the r03 AMEND. Sanitized r04 packet SHA `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829` returned AMEND: findings 3 and 4b–4d closed at design level; finding 4a remained must-fix. No D2 code/runtime followed. |
| v1.7 | 2026-10-05 | Same-SPEC documentary response to r04; sanitized r05 packet returned AMEND with three 4a must-fixes. No D2 code/runtime followed. |
| v1.8 | 2026-10-05 | Same-SPEC response to r05: supported compiler-argument grammar, forwarded/function-like macro rejection, `-ansi` rejection, output-derived target identity, assertion-independent exact link checks, dynamic-link conflict rejection, and schema-level PASS/failure conditions. Local/unreviewed; fresh exact-SHA consent is required for r06; D2/D3 remain blocked. |
| v1.9 | 2026-10-10 | Editorial privacy-only redaction of the workstation-specific D1-H checkout path to a stable role alias; no design, scope, or gate change. |
| v1.10 | 2026-10-10 | Owner-directed, same-SPEC response to r06 AMEND. Keeps the source/output containment, exact `-c` count, minimal `env -i`, actual `link.txt` binding and no-false-PASS contract; relaxes generic argument grammar, exhaustive environment denylisting and duplicated schema/finalizer rules. The frozen B1/B2/B3 r07 packet later received documentary `APPROVE` only. |
| v1.11 | 2026-10-11 | Owner-directed replacement of the unavailable user/IPC/mount/PID namespace sandbox by host-controlled candidate-resource attribution, 0600 exclusive reservations, recorded libc semaphore-name mapping, pre/post/final inventory equality and fail-closed identity-checked cleanup. No AppArmor exception or global security change. |
| v1.12 | 2026-10-12 | Same-SPEC documentary response to r08 B2/B3: require a dedicated single-run UID/launcher or remain blocked; suppress diagnostic `sem_unlink` after all three `sem_open` failure paths; add fsynced reservation journal, retained raw `/proc/sysvipc` bytes/recomputation and evidence binding the returned `sem_t` mapping to the exact inode. Astra r09 substantially accepted B2/B3 but returned AMEND on contradictory finalizer IPC scope; no implementation/runtime authorization. |
| v1.13 | 2026-10-12 | Same-SPEC response to the sole r09 B1: reconcile the finalizer's responsibility by allowing only bounded inventory/current-identity inspection and journal-authorized exact `IPC_RMID`/`sem_unlink` under the continuous dedicated-UID lease, while forbidding Process/GW construction, resource creation, production message traffic, ambiguous retries and broad cleanup. Astra r10 resolved the IPC boundary and preserved B2/B3, but returned AMEND on abnormal finalization. No implementation/runtime authorization. |
| v1.14 | 2026-10-12 | Same-SPEC response to r10 abnormal-finalization B1: adds durable `RUN_SHUTDOWN_COMPLETE`, supervisor PID wait/log receipts, run-record v5 and journal-event v2. The r11 documentary review returned AMEND on lease-loss eligibility and truthful cleanup history after a cleanup attempt. The r12 amendment clarifies common lease/file-authority gates, separates normal full-schema validation from abnormal bounded raw preservation, adds missing/unavailable artifact sets and evidence-derived cleanup outcomes, and forbids finalizer re-entry or cleanup retries. No implementation/runtime authority. |
| v1.15 | 2026-10-13 | Owner-authorized same-SPEC methodology correction to Astra r12 B1/B2: adds canonical versioned journal/mode/outcome model, distinct create/cleanup states, exact stable identity binding, structured projection requirements and a read-only checker contract. Historical `complete` cleanup remains separate from overall `INCONCLUSIVE` status. r12 artifacts remain immutable. No Astra follow-up or operational authority granted. |
| v1.16 | 2026-10-13 | Owner-authorized same-SPEC response to Astra r13 AMEND: defines bounded, read-only journal-history parsing/replay solely for COUT-001–004 in `abnormal_evidence_only`; malformed, unavailable, truncated or incomplete history is `unknown` without blocking an otherwise authorized `INCONCLUSIVE` record. Other non-control evidence remains raw-only; mutation, live IPC inspection, cleanup and retry remain forbidden. Model v2 and local projection checks are unreviewed; no Astra follow-up or operational authority granted. |
````

## FILE: Specs/RESULTS-SPEC-039/precode-manifest-v1.16-d2a-r15.json

Role: Current D2a draft manifest and policy projections.
Bytes: 15731
SHA-256: 4ed5646dcaab8af74344f3acba22c962fef8703b2d4d0fddd463c0381b9e3827

```text
{
  "baseline_source_files": {
    "CMakeLists.txt": {
      "baseline_blob_sha256": "5f9cd0676b2dcdcd0a500d778f5d5baea312301d6abc382db5eab95f2bbb1d07",
      "worktree_sha256": "d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d"
    },
    "Common/src/Block.cpp": {
      "baseline_blob_sha256": "26dfa06b575cea6bb9c8d21a18b894f37e73ad91c7e7a7450fefe517903e1622",
      "worktree_sha256": "26dfa06b575cea6bb9c8d21a18b894f37e73ad91c7e7a7450fefe517903e1622"
    },
    "Common/src/GW.cpp": {
      "baseline_blob_sha256": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e",
      "worktree_sha256": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e"
    },
    "Common/src/GW.h": {
      "baseline_blob_sha256": "54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44",
      "worktree_sha256": "54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44"
    },
    "Common/src/GWRunInitialization01.cpp": {
      "baseline_blob_sha256": "1656b703c892831a05043a8bf82ae1bed73d701c8ffea7b95af5a02df0c978af",
      "worktree_sha256": "1656b703c892831a05043a8bf82ae1bed73d701c8ffea7b95af5a02df0c978af"
    },
    "Common/src/Message.cpp": {
      "baseline_blob_sha256": "96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518",
      "worktree_sha256": "96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518"
    },
    "Common/src/Message.h": {
      "baseline_blob_sha256": "bdbbfee17af8512b2c64e9b61cf83e73b5293cffcd7067c0a10787c80bac4e70",
      "worktree_sha256": "bdbbfee17af8512b2c64e9b61cf83e73b5293cffcd7067c0a10787c80bac4e70"
    },
    "Common/src/Process.cpp": {
      "baseline_blob_sha256": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1",
      "worktree_sha256": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1"
    }
  },
  "bounded_prior_review_history": [
    {
      "checkpoint": "historical D2 pre-code v1.2",
      "consent": "consumed; no follow-up consent",
      "packet_sha256": "f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942",
      "response_sha256": "a9d900654287be6f3d9afd5c6e90f1b8a53fe03a03202f8311330345675dcd05",
      "spec_sha256": "8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91",
      "verdict": "HOLD"
    },
    {
      "checkpoint": "v1.15/r13",
      "packet_sha256": "0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727",
      "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.15-r13-20261013.md",
      "response_sha256": "94c118fe12b2b27143a521b43be5ea194d4d0969b4ea35c8829d868d91af47d0",
      "scope": "abnormal-mode journal-policy contradiction",
      "verdict": "AMEND"
    },
    {
      "checkpoint": "v1.16/r14",
      "packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
      "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.16-r14-20261013.md",
      "response_sha256": "2d0183b961cd5d7a7ee48c555f0bf04345ed9bcb07761697d84ec0fdc57d9406",
      "scope": "r13 journal-policy contradiction only; not full D2/profile/runtime",
      "verdict": "APPROVE"
    }
  ],
  "consent": {
    "astra_calls_authorized": 0,
    "astra_exact_sha_consent_for_this_packet": false,
    "d2_execution_authorized": false,
    "provisioning_authorized": false
  },
  "created_date": "2026-10-13",
  "d1h_cmake_overlay_sha256": "75b25d39ae24b524695eebcba758915fbe7c1526c242dc0bec7ba5c2b79cb710",
  "functional_baseline": {
    "head": "1af604dad66148618cc0e578c28adb4a9829e607",
    "pre_existing_overlay": "D1-H CMake change and untracked harness/manifest; preserve unchanged",
    "tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
    "working_tree_status": "M CMakeLists.txt\n?? tests/",
    "worktree_alias": "<D2_WORKTREE>"
  },
  "historical_d2_hold_response": {
    "response_chars": 10687,
    "response_sha256": "a9d900654287be6f3d9afd5c6e90f1b8a53fe03a03202f8311330345675dcd05",
    "transcript_alias": "ng039_d2_precode_f29274fa"
  },
  "host_launcher_proposal": {
    "candidate_lease": "/run/lock/ng039-d2/lease.lock",
    "candidate_supervisor": "/usr/local/libexec/ng039-d2-supervisor",
    "candidate_uid": "ng039d2",
    "candidate_unit": "/etc/systemd/system/ng039-d2.service",
    "candidate_workspace_root": "/var/lib/ng039-d2/workspace",
    "path": "Specs/RESULTS-SPEC-039/d2-launcher-lease-proposal-v1-20261013.md",
    "sha256": "ab386f6a94228ab9312efa4526e68b67ab43fc0f7936b789a9ebc459b59f67e6",
    "status": "draft, not installed or tested"
  },
  "normative_contract": {
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025",
    "status": "In Progress",
    "version": "v1.16"
  },
  "not_yet_created": [
    "tests/spec039/gw_q0_q1_d2.cpp",
    "tests/spec039/q0_q1_d2.manifest.json",
    "dedicated UID",
    "systemd unit",
    "supervisor executable",
    "lease/control/workspace directories"
  ],
  "proposed_code_allowlist": [
    "CMakeLists.txt",
    "Common/src/GW.h",
    "Common/src/GW.cpp",
    "tests/spec039/gw_q0_q1_d2.cpp",
    "tests/spec039/q0_q1_d2.manifest.json"
  ],
  "proposed_oracle_resolution": {
    "bootstrap_drain": "Proposed sticky scalar event after a completed Gateway iteration proves constructor-queued bootstrap work is drained: Q0 requires queue_depth=0 and all-queue admission/pop/Run-entry/Run-return counts reconciled; Q1 requires only the registered future target remain at head and all non-target bootstrap work reconciled. The synchronous constructor Run is complete before the harness receives Process/GW.",
    "empty_sample_point": "Only at actual condition_variable::wait_for return after InputQueueMutex is reacquired, if StopGateway is false and InputQueue is empty; increment one saturating observation and event sequence there.",
    "q1_correlation": "WaitCapture and DecisionCapture are sticky; capture returned wait predicate value and the existing strict due/pop branch only; same iteration ID, increasing sequence and identical target/head/candidate scalar ID are required; overflow/mismatch is INCONCLUSIVE.",
    "run_cardinality": "Interpret N=1 as one GW consumer per condition; proposed paired experiment is two fresh, sequential Q0/Q1 process starts with distinct run IDs/journals/final records under one trusted service/lease. This interpretation is a review question, not execution authorization.",
    "synchronization": "One lock at existing admission/wait/pop sites; no recursive lock; scalar-copy accessor under InputQueueMutex; no observer Message*; counters saturate and overflow fails closed."
  },
  "readiness_evidence": {
    "d1h_binary_executed": false,
    "d1h_binary_sha256": "060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d",
    "d1h_harness_sha256": "15913ee8b82fed7ad259668ed84e6e261aa444a3df443f3521929eba01b8dc92",
    "d1h_manifest_sha256": "9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a",
    "d1h_receipt_sha256": "3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28",
    "d2_files_present_in_isolated_worktree": {
      "tests/spec039/gw_q0_q1_d2.cpp": false,
      "tests/spec039/q0_q1_d2.manifest.json": false
    },
    "d2_files_present_in_main_checkout": {
      "tests/spec039/gw_q0_q1_d2.cpp": false,
      "tests/spec039/q0_q1_d2.manifest.json": false
    },
    "host_preflight": {
      "d2_source_files_present": {
        "gw_q0_q1_d2.cpp": false,
        "q0_q1_d2.manifest.json": false
      },
      "gates_still_required": [
        "Freeze the complete D2 pre-code profile and review packet, including the dedicated identity/launcher/lease design, exact allowlist, target graph, oracle, and supervisor/finalizer evidence contract.",
        "Obtain owner consent bound to the final exact packet SHA before any additional Astra call.",
        "Provision and independently verify the dedicated non-login UID, trusted single-run launcher, and exclusive lease; this host-side change is outside the five-file D2 test-code allowlist and was not performed.",
        "Record a separate owner execution decision before any D2 configure/build/harness/IPC run.",
        "D3 remains separate and requires D2 PASS plus its own explicit authorization."
      ],
      "not_performed": [
        "No SPEC or production source edit",
        "No UID/group/service/launcher/lease provisioning",
        "No sudo",
        "No build/configure",
        "No D2 harness or finalizer execution",
        "No IPC creation/removal or runtime test",
        "No Astra call",
        "No commit or push"
      ],
      "observations": {
        "gates_still_required": [
          "Freeze the complete D2 pre-code profile and review packet, including the dedicated identity/launcher/lease design, exact allowlist, target graph, oracle, and supervisor/finalizer evidence contract.",
          "Obtain owner consent bound to the final exact packet SHA before any additional Astra call.",
          "Provision and independently verify the dedicated non-login UID, trusted single-run launcher, and exclusive lease; this host-side change is outside the five-file D2 test-code allowlist and was not performed.",
          "Record a separate owner execution decision before any D2 configure/build/harness/IPC run.",
          "D3 remains separate and requires D2 PASS plus its own explicit authorization."
        ],
        "host_checks": {
          "checked_launcher_directories": [
            "/usr/local/sbin",
            "/usr/local/bin",
            "<OWNER_HOME>/.local/bin",
            "<OWNER_HOME>/.config/systemd/user",
            "/etc/systemd/system"
          ],
          "limitation": "The directory check was name-filtered over the listed locations, not an exhaustive privileged filesystem audit; absence of matching names is not proof that no arbitrary-named launcher/account exists.",
          "matching_named_launchers_or_units": [],
          "processes": {
            "matches": [],
            "query": "ps -eo user,uid,pid,ppid,comm filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination"
          },
          "sudo_or_root_changes": false,
          "systemd": {
            "exit_code": 0,
            "matches": [],
            "query": "systemctl list-unit-files --no-legend filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination"
          },
          "user_accounts": {
            "limitation": "Name-filtered search only; no account name or UID for the future dedicated identity is currently pinned in this receipt.",
            "matches": [],
            "query": "local passwd entries whose names contain ng039, q0q1, or q0_q1"
          }
        },
        "not_performed": [
          "No SPEC or production source edit",
          "No UID/group/service/launcher/lease provisioning",
          "No sudo",
          "No build/configure",
          "No D2 harness or finalizer execution",
          "No IPC creation/removal or runtime test",
          "No Astra call",
          "No commit or push"
        ],
        "receipt_sha256": "38d9d232b27476e89190b82ddc6538d1534362b647d14d904921ae6499ca51b6",
        "repository_state": {
          "branch": "AIOPT5",
          "git_diff_check": "PASS",
          "head": "756f493b7049434ab67cf2382486a48f606a470c",
          "preexisting_dirty_paths_preserved": [
            "Specs/SPEC-039-common-performance-measurement.md",
            "Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md",
            "Specs/RESULTS-SPEC-039/",
            "Specs/RESULTS-SPEC-059/"
          ],
          "spec_sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025"
        },
        "review_scope": {
          "limitation": "Astra approved only the bounded documentary closure of the r13 journal-policy contradiction; it did not approve the full D2 profile, implementation, or runtime.",
          "r14_packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
          "r14_verdict": "APPROVE"
        },
        "scope": "Read-only local host/worktree readiness checks to identify prerequisites for the SPEC-039 D2 gate. This is not a runtime or authorization receipt.",
        "status": "BLOCKED_PREPARATION_ONLY",
        "worktrees": {
          "isolated_d1h_checkout": {
            "actions": "Preserved; no edits/build/run performed during this preflight.",
            "branch": "DETACHED",
            "d1h_files": [
              "tests/spec039/gw_q0_q1_harness.cpp",
              "tests/spec039/q0_q1.manifest.json"
            ],
            "d2_files": {
              "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
              "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
            },
            "head": "1af604dad66148618cc0e578c28adb4a9829e607",
            "status": [
              "M CMakeLists.txt",
              "?? tests/"
            ]
          },
          "primary_checkout_d2_files": {
            "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
            "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
          }
        }
      },
      "receipt_sha256": "38d9d232b27476e89190b82ddc6538d1534362b647d14d904921ae6499ca51b6",
      "runtime_build_ipc": "not run by this preflight",
      "scope": "bounded read-only checks only; not an exhaustive host audit",
      "status": "BLOCKED_PREPARATION_ONLY",
      "uid_launcher_service": "not provisioned or verified"
    },
    "local_r14_validation_receipt_sha256": "ebafdb331e823f25975e4e9c7f85cc03fe75e809e64253ed177f8b1ec77931af",
    "r14_local_tests": "63 normal + 63 optimized; 5 Draft 2020-12 schemas meta-validated; read-only checker PASS; D2 runtime/finalizer NOT EXECUTED"
  },
  "receipt_type": "ng039-spec039-d2a-precode-manifest/v1",
  "repository_identity": {
    "branch": "AIOPT5",
    "commit_or_push": "not performed",
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "working_tree_status": "M Specs/SPEC-039-common-performance-measurement.md\n M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md\n?? Specs/RESULTS-SPEC-039/\n?? Specs/RESULTS-SPEC-059/"
  },
  "review_questions": [
    "Do current v1.16 clauses and supplied source excerpts close all four historical D2 pre-code HOLD findings at documentary level?",
    "Is the proposed exact bootstrap-drain and empty-observation event point sufficient to prove Q0 queue state without changing queue behavior?",
    "Is the Q0/Q1 cardinality interpretation (two fresh sequential process starts under one service/lease) consistent with §§2–4 and §7.1, or must it be amended before code?",
    "Does the root-owned systemd supervisor, dedicated UID, control-plane receipt/log ownership, cgroup process ledger, root-only continuous lease and inherited finalizer lease FD satisfy §7.1, including the proposed WORKSPACE_ROOT?",
    "Is the proposed compile-time-gated observer field/interface map and D2-only CMake graph specific enough for the pre-code gate; identify any exact remaining design blocker.",
    "Does the three-path diagnostic sem_unlink suppression remain bounded and fail-closed, with macro-off default behavior unchanged?"
  ],
  "review_scope": "D2a pre-code design readiness only; no implementation, build, runtime, IPC, host provisioning, D2/D3 acceptance, or operational authority",
  "status": "LOCAL_DRAFT_UNSUBMITTED",
  "validation_boundary": {
    "Astra": false,
    "IPC": false,
    "VM_SSH": false,
    "commit_push": false,
    "configure_build": false,
    "packet_assembly": "local deterministic assembly only",
    "packet_submitted": false,
    "runtime": false,
    "system_changes": false,
    "test_harness": false
  }
}
```

## FILE: Specs/RESULTS-SPEC-039/d2-launcher-lease-proposal-v1-20261013.md

Role: Current uninstalled/unverified proposal.
Bytes: 11154
SHA-256: ab386f6a94228ab9312efa4526e68b67ab43fc0f7936b789a9ebc459b59f67e6

```text
# NG-039 D2 launcher/lease provisioning proposal — draft only

**Status:** candidate design for owner/reviewer decision; not normative, installed, or tested.  
**Purpose:** satisfy SPEC-039 §7.1's dedicated-identity and continuous-lease prerequisite for one local D2 Q0/Q1 correctness run.  
**Boundary:** this proposal does not authorize D2 code edits, configure/build, harness/finalizer execution, IPC creation/removal, Astra submission, VM/SSH, commit, or push.

## 1. Host facts checked read-only

- Local host exposes `systemd 255.4`; `flock`, `setpriv`, `runuser`, `useradd`, and `groupadd` are available. None was used to mutate state.
- The owner's home directory is mode `0750`; a future D2 UID must not be added to the owner's group or granted access to the home tree merely to reach test files.
- `/var/lib` and `/srv` are root-owned `0755`; `/proc/sysvipc` is readable (`0555`) and `/dev/shm` is host-visible (`01777`). Candidate `<WORKSPACE_ROOT>` is `/var/lib/ng039-d2/workspace`, outside the private home and `.hermes`; §7.1 leaves `<WORKSPACE_ROOT>` to be resolved and recorded in the run manifest.
- No matching named `ng039` account, service unit, or launcher was found in the bounded preflight; this is not an exhaustive root-owned filesystem audit. No D2 test files exist in either examined checkout.

## 2. Candidate identity and files — pending approval

| Item | Proposed value | Required invariant |
|---|---|---|
| Dedicated account/group | `ng039d2` | Non-login shell `/usr/sbin/nologin`; locked password; no supplementary groups, SSH key, sudoers/polkit launch grant, or unrelated system/user service; reserved only for this D2 gate. These facts must be independently checked at install and pre-run. |
| Systemd unit | `/etc/systemd/system/ng039-d2.service` | Root-owned, fixed one-shot entry (`User=root`, `Group=root`, `Restart=no`, `KillMode=control-group`). Only root may start/modify it. Candidate hardening: `NoNewPrivileges=yes`, `ProtectSystem=strict`, `ProtectHome=yes`, `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no`, `PrivateUsers=no`, `PrivateNetwork=no`, `ProtectProc=default`, `RestrictAddressFamilies=AF_UNIX`, `CapabilityBoundingSet=CAP_SETUID CAP_SETGID CAP_KILL CAP_DAC_READ_SEARCH`, plus explicit `ReadWritePaths` for the pinned workspace/control paths and host `/dev/shm`. The exact property set must be reviewed and a no-IPC service-side preflight must prove it does not hide or replace `/proc/sysvipc` or the host `/dev/shm` mount. |
| Supervisor | `/usr/local/libexec/ng039-d2-supervisor` | Root-owned, non-writable by `ng039d2`; fixed argument contract; SHA-256 pinned in the run manifest. It holds the root-only lease, launches only the pinned harness and finalizer without a shell, and drops each controlled child to `ng039d2` using an explicit `setpriv --reuid=… --regid=… --clear-groups --no-new-privs -- …` argv. The harness never inherits the lease FD. The finalizer receives a fixed `--lease-fd <n>` handle after the harness exits; it may inspect the pinned FD but must not close/unlock it. The supervisor retains its own lease FD until finalizer exit and final-record verification. The build/check phase is separate and must finish, be pinned, and be made read-only before this one-shot run service starts. The finalizer is sequential, never concurrent with the harness. |
| Workspace root | `/var/lib/ng039-d2/workspace` (`<WORKSPACE_ROOT>`) | Root-managed and not writable by `ng039d2`. Freeze fresh sibling roots beneath it: `source/<run-id>/`, `build/<run-id>/`, `results/<run-id>/`, and `scratch/<run-id>/{home,tmp}`. Source/manifest and the post-build executable become root-owned/read-only; build output is writable only during configure/build; results and the distinct HOME/TMPDIR are writable only by the D2 UID. Verify pairwise non-overlap, symlink-free parents, device/inode and ownership before use. No `.hermes` paths. |
| Control and lease | `/var/lib/ng039-d2/control/<run-id>/`; `/run/lock/ng039-d2/lease.lock` | Control parents are root-managed; `/var/lib/ng039-d2/control` and the per-run control directory are root:`ng039d2` mode `0710`, so the D2 UID can traverse but cannot list or modify them. `supervisor-termination.json` and closed logs are root:`ng039d2` mode `0440`, opened with no-follow/exclusive identity checks and fsynced/hashed by the supervisor before the finalizer receives read-only access. Lease directory/file are root-managed `0700`/`0600`; supervisor checks regular-file, owner/mode, `st_dev`/`st_ino`, opens with `O_NOFOLLOW|O_CLOEXEC`, obtains nonblocking exclusive `flock`, and continuously verifies the same held FD/identity through final-record closure. `ng039d2` cannot open, replace, or remove the lease. |
| IPC visibility | systemd `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no` | No IPC or mount namespace may hide/replace host resources. Before any Nova/IPC run, a no-Nova preflight under the exact unit must compare service/host IPC namespace identity, raw `/proc/sysvipc` readability/hash and `/dev/shm` mount/device/inode identity without creating/removing IPC. Exact `sem_t`-to-object device/inode mapping remains a required check during the separately authorized D2 reservation phase. No network input. |

**Owner trust assumption:** host root/systemd remains trusted, as allowed by §7.1. The fixed root-owned unit/launcher is the only permitted entry to `ng039d2`; account/session/service/polkit audits plus a live UID census and systemd-cgroup membership check must show no unrelated process before pre-inventory and at every boundary. Any unlisted process or process outside the unit cgroup blocks finalization and preserves evidence. The systemd cgroup is the process-tree source of truth; a process group alone is insufficient. If root-level concurrent mutation is in scope, this shared-host profile is unacceptable and D2 remains blocked.

## 3. Proposed run/supervisor protocol

1. A separately authorized owner action starts the one root-owned service with a root-owned, one-use authorization record that names the paired D2 experiment: one fresh Q0 process start followed by one fresh Q1 process start (`N=1` GW consumer per condition), with separate run IDs, journals and final records. The supervisor atomically marks the authorization consumed before any D2 action; repeated starts fail closed, and another experiment requires another owner decision. No automatic restart or retry. The root supervisor validates the fixed unit, script/binary hashes, manifest, account/session/service/polkit state, workspace roots, result-directory identity and lease-file identity before any inventory.
2. The supervisor obtains the root-only exclusive lease before Q0 pre-inventory and retains it continuously across Q0, its finalizer, Q1, its finalizer and final-record closure. It checks all processes with the D2 UID against the unit cgroup and expected process ledger before/after each phase; any unrelated or escaped process blocks the experiment.
3. Build/configure/checks are not performed by the run service: they require their own post-review authorization, explicit `env -i` commands under `<WORKSPACE_ROOT>`, and a frozen executable hash. Candidate retained environment is exactly `PATH=/usr/bin:/bin`, `HOME=<WORKSPACE_ROOT>/scratch/<run-id>/home`, `TMPDIR=<WORKSPACE_ROOT>/scratch/<run-id>/tmp`, `LANG=C.UTF-8`, and `LC_ALL=C.UTF-8`; HOME and TMPDIR are distinct. Before the run unit starts, the exact source/manifest and executable are root-owned/read-only. After the no-Nova service visibility check and Q0 pre-inventory, the supervisor starts the pinned harness as a single `ng039d2` child via fixed no-shell `setpriv` argv, with a new session/process group and bounded stdout/stderr FDs opened in the protected control directory. The harness itself creates no child process; the Gateway output worker is a thread.
4. The supervisor records the systemd unit/cgroup identity and, for every process, PID, PPID, PGID, `/proc` start time, real/effective/saved/filesystem UID/GID, executable path/device/inode/hash, cgroup membership, and raw `waitpid` status. It compares the live cgroup process set with the expected ledger (root supervisor plus at most one sequential D2 child) at each boundary; unknown members, escaped children, or incomplete reaping are `BLOCKED/INCONCLUSIVE`.
5. After each harness exits, the supervisor fsyncs/closes/hashes its log FDs and writes that run's root-owned `supervisor-termination.json` in the protected control directory. The same binary's `--finalize` mode runs sequentially as `ng039d2`, with read-only receipt/log access and a fixed inherited lease FD while the root supervisor retains its own FD. The finalizer must preserve the handle through final-record closure and must not unlock it. After finalizer exit, the supervisor verifies the lease identity/lock and final-record hash before advancing. Q1 begins only after Q0 finalization and a fresh Q1 pre-inventory under the same lease. Any Q0 failure that leaves inventory, cleanup, identity or lease uncertain stops the sequence; Q1 is recorded `NOT_RUN`, never retried. The supervisor must not invoke/re-enter a finalizer if receipt, log, process-set, identity or lease checks fail.
6. The supervisor seals and hashes each result tree after its final record. It releases the lease only after both run records are closed (or Q0 has failed closed with Q1 `NOT_RUN`) and exits the unit. `KillMode=control-group` is a last-resort containment mechanism, not proof of reaping; each receipt must prove zero unexpected cgroup members and the expected wait results before the next phase. Timeout/signal behavior is pinned in the manifest; no retry or broad cleanup.
7. Any lease/UID/process/log/identity mismatch, timeout, unexpected child, incomplete receipt or failed journal boundary preserves pending/partial evidence and stops. D3 is not part of this unit.

## 4. Installation and test scope — not authorized or performed

Provisioning would require privileged changes to the host account database, `/etc/systemd/system`, `/usr/local/libexec`, `/etc/tmpfiles.d`, `/run/lock/ng039-d2`, and `/var/lib/ng039-d2`. Before installation, freeze exact files, contents, modes/owners, hashes, unit properties, rollback and no-run acceptance tests and obtain owner approval. A no-Nova/no-IPC service check must verify the actual UID transition, root-only unit/launcher and lease permissions, polkit/sudoers restrictions, live UID census, cgroup tracking, IPC namespace identity, raw `/proc/sysvipc` access, `/dev/shm` mount/device/inode visibility, safe control/log FDs, receipt read-only access, one-use authorization and fail-closed behavior. It must not create/remove IPC or invoke the real D2 harness. Exact semaphore backing-object mapping cannot be established by this no-IPC check; prove it only in the separately authorized D2 reservation phase before constructing `Process/GW`.

**Not done:** no user/group, file, systemd unit, lock, directory, or launcher was created; no `sudo` was run; no service was started; no Astra call was made. This design remains a proposal until owner decision and documentary review.
```

## FILE: Specs/RESULTS-SPEC-039/astra-review-v1.16-d2a-r15-20261013.parsed.md

Role: Complete parsed response; its exact raw SSE digest is in the submission receipt.
Bytes: 14363
SHA-256: e6ea5ac8fde7da03214ac243d22982a563cd36d03d4f9bf895335275182a89d3

```text
# Verdict: HOLD

**The packet is not yet ready for the D2a pre-code gate.** The queue-only approach, scalar observer, five-file boundary, and root-supervised dedicated-UID architecture are reasonable foundations. However, the frozen proposal leaves material contradictions in observation ordering, process cardinality, failure propagation, build/provider identity, and supervisor lifecycle handling.

This is exactly one documentary review of supplied packet SHA-256:

`ab4855bdc79c81ad637903abc4071a892d64224a17519a60d0992a160226b7c1`

All hashes, source locations, host observations, and historical results are supplied identities/evidence—not independently verified. I used no tools.

## 1. Must-fix blockers

### B1. Freeze a coherent observer initialization, drain, and window protocol

**SPEC locations:** §4 item 4; §7.1 items 2–3.  
**Source locations:** `Common/src/GW.cpp:113–123, 251–262, 451–503, 591–593`; `Common/src/GWRunInitialization01.cpp:152–179`; `Common/src/GW.h:153–182`.  
**Proposal locations:** packet §4, “Projection issue” and “Proposed Q0/Q1 bootstrap and oracle protocol.”

Three issues remain:

1. **Reset semantics conflict.** SPEC says registration “resets/binds the observation”; the proposal preserves queue-wide constructor/bootstrap history and resets only target state. Constructor-time admission precedes registration. A full reset would erase precisely the history needed for drain reconciliation.

2. **Q1’s sticky captures can precede its window baseline.** The proposal admits the target before declaring Q1 bootstrap drained. Gateway can capture the first target wait and decision before the harness snapshots that drain event. SPEC nevertheless requires a matched pair newer than the Q1 baseline, while prohibiting overwriting the first pair. That permitted execution cannot satisfy the oracle.

3. **The API projection is incomplete.** It lacks a drain sequence/iteration checkpoint, current head identity/time, and the event-message identity required by §4 item 4. “Counts reconcile” does not specify the equations or whether constructor synchronous `Run` participates in the queue-wide Run counters.

**Required amendment:** freeze a single event-order contract, including:

- member-storage placement and initialization before constructor activity;
- registration preserving monotonic queue history;
- exact Q0 and Q1 drain equations and lock ownership;
- treatment of synchronous constructor `Run` versus queued dispatch;
- when admission, the sticky pair, drain, and the 100 ms baseline occur;
- scalar evidence tying drain/head observations to their event generations;
- finite bootstrap/window deadlines and failure outcomes.

The proposed **empty observation at the real `wait_for` return, under the reacquired mutex and with stop false, is appropriate**. It supports a fresh observed-empty event—not continuous emptiness or an idle CPU floor.

### B2. Resolve the one-run versus two-process contract and bootstrap stimulus

**SPEC locations:** §§2–3; §4 items 4–5; §6; §7.1 item 2 and concluding runtime-shape paragraph.  
**Source locations:** `Common/src/Process.cpp:228–230, 541–577`; `Common/src/GW.cpp:113–123`; `Common/src/GWRunInitialization01.cpp:152–179, 190–225`.  
**Proposal locations:** packet §4 Q0/Q1 protocol; launcher proposal §3 items 1–2.

`N=1` means one GW consumer; it does **not** determine the number of fresh process starts. Two sequential starts are compatible with `N=1` in principle. But the proposal is not currently the frozen SPEC contract:

- §3 defines each fresh process start as a run.
- Several D2 clauses authorize/describe “one” correctness run.
- §7.1 item 2 keeps a preallocated target out of the queue “through Q0,” implying a different phase arrangement from separate Q0/Q1 processes.
- The launcher proposes two run IDs, journals, and final records.

The bootstrap contract also needs the **exact Process name/key and hello-trigger behavior**. The historical response identifies a queued hello trigger and key-11 suppression; the new proposal introduces a derived process key but does not supply the hello creation/action logic necessary to establish the resulting bootstrap work.

**Required amendment:** explicitly define one paired experiment containing two sequential correctness runs—or retain one process with two phases—and reconcile every active projection. Specify target existence/admission per condition, process parameters, all bootstrap messages, and Q1 continuation/`NOT_RUN` rules. Keep the D3 repetition/timing plan separate. No existing consent should be interpreted as silently expanding run cardinality.

### B3. No-unlink guards are specified, but diagnostic abort behavior is not

**SPEC locations:** §4 item 5; §7.1 paragraph beginning “The three pinned `GW.cpp` paths…”; §7.1 item 5.  
**Source locations:**

- `Common/src/GW.cpp:963–971` — read-side failure;
- `Common/src/GW.cpp:1258–1264` — write-side failure;
- `Common/src/GW.cpp:1310–1327` — initialization/discovery failure;
- `Common/src/GWRunInitialization01.cpp:156–179` — caller continues after unsuccessful discovery.

Suppressing all three `sem_unlink` calls is the correct bounded safety change. **Recording a failure alone does not implement the required abort.** In particular, the shown initialization caller simply skips a binding and continues toward admission when discovery fails.

The observer projection supplies no semaphore-failure state, and no exact contract connects constructor, Gateway, and output-worker failures to harness termination and finalizer eligibility.

**Required amendment:** define a bounded sticky failure record, synchronization, errno capture, and the exact termination/checkpoint behavior for each path. Ensure registration cannot erase constructor-time failure, no Q0/Q1 window can begin after such failure, and no failure becomes PASS. Specify how all three paths are fault-injected within the allowlist, proving both zero unlink calls and fail-closed outcome. Preserve macro-off behavior.

### B4. Static linking conflicts with the required libc-provider proof; the CMake delta remains unfrozen

**SPEC locations:** §4 item 5; §4.5 pre-code checkpoint; §7.1 semaphore-mapping paragraph and item 4.  
**Source locations:** supplied overlay `CMakeLists.txt:29–157`, especially the flags immediately preceding `add_executable(ContentApp)` at line 157.  
**Affected new files:** `tests/spec039/gw_q0_q1_d2.cpp` and `tests/spec039/q0_q1_d2.manifest.json`—no proposed line-numbered contents supplied.

The link checker mandates `-static`, while the semaphore contract requires `dladdr` to identify the `sem_open` provider as a resolved libc file with device/inode/hash. A fully static executable cannot simply be assumed to expose a separate loaded libc provider file through that mechanism. No static-compatible provider-evidence design is supplied.

Separately, the two-target graph is only described. The packet does not freeze:

- diagnostic branch placement relative to existing flags and D1-H targets;
- precise target properties and macro propagation to all Common translation units;
- the configure-time target-census implementation;
- executable linkage needed by the provider-evidence procedure.

**Required amendment:** choose a mutually consistent linking and provider-provenance contract while retaining exact returned-handle-to-semaphore-inode evidence. Supply the proposed CMake/interface delta or an equally exact line-anchored design. The five-file allowlist is appropriately bounded, but its graph is not yet implementation-ready.

### B5. The supervisor architecture needs an exact authority and termination protocol

**SPEC locations:** §7.1 dedicated-identity/continuous-lease paragraphs; §7.1 items 2 and 5; §7.2 `AUTH-001`–`AUTH-004`.  
**Proposal locations:** launcher proposal §2 “Supervisor” and “Control and lease”; §3 items 4–7.  
**Source locations:** external `/usr/local/libexec/ng039-d2-supervisor` and `/etc/systemd/system/ng039-d2.service` are not supplied as source and have no available line numbers. Relevant harness shutdown sites are `Common/src/GW.cpp:658–667, 1331–1337`.

The root-owned service, restricted account, protected receipt directory, and cgroup ledger are stronger than an ordinary-UID cooperative lock. They can support §7.1 under its trusted-root assumption. They do not yet constitute an exact protocol:

- **Lease FD authority:** an inherited duplicate of a flock-held open file description is not inspection-only. The finalizer can issue `LOCK_UN` on that description; the supervisor retaining another descriptor does not prevent that. “Must not unlock” is a trusted-code rule, not kernel-enforced root-only lease control. Post-exit checking alone cannot establish uninterrupted authority.
- **Receipt PID scope:** the supervisor is included in the process ledger, but cannot be reaped before invoking the finalizer. The contract must distinguish cgroup census members from the harness PID set whose wait receipts gate finalization.
- **Blocked termination:** SPEC item 2 says not to release the lease when reaping/log proof is incomplete. The proposal says mismatches stop the service, whose exit releases its flock. The exact behavior is unresolved.
- **Timeouts and bounds:** signals, escalation, deadlines, log-cap handling, and failed-finalizer behavior are said to be manifest-pinned, but no exact values/protocol are supplied.

**Required amendment:** define who owns and may manipulate the lease, what evidence establishes continuous authority, the precise per-phase PID/receipt sets, and a bounded failure-state machine. If trusted finalizer access to the shared lock description is intentional, state that trust explicitly rather than claiming the FD is inspection-only. Freeze the service/launcher contract and distinguish final-record refusal from containment and lease-release behavior.

Provisioning and service-side verification remain later gates; their absence is not itself the documentary blocker.

### B6. Reservation-failure cleanup conflicts with the shutdown gate

**SPEC locations:** §7.1 paragraph beginning “After pre-inventory and journal readiness…”; §7.1 normal-shutdown paragraphs and item 2; §7.2 `JRN-005`–`JRN-009`, `AUTH-002`.  
**Source locations:** constructor exposure at `Common/src/GW.cpp:113–123` and `Common/src/GWRunInitialization01.cpp:156–179`. The reservation/finalizer implementation belongs to the absent `tests/spec039/gw_q0_q1_d2.cpp`; no source lines are available.

The reservation paragraph permits removing successfully reserved resources after a failed reservation, before construction. Elsewhere, only the sequential finalizer may clean, and only after a shutdown event proving joins and Process/GW destruction. The packet does not define how a never-constructed run satisfies that gate—or whether it must retain all residue.

**Required amendment:** specify one consistent path for partial reservation failure. Either define valid evidence for the never-constructed state and its normal-cleanup eligibility, or require abnormal evidence-only handling with no cleanup. Do not imply permission for harness-side cleanup or fabricate destruction evidence.

## 2. Disposition of the historical D2 HOLD findings

| Historical finding | Documentary disposition |
|---|---|
| Resource establishment before construction | Ordering is corrected under the replacement host-controlled profile. Full closure still depends on B5–B6; the obsolete namespace profile is not required. |
| Target lifetime and ownership | Substantially addressed: `Delete=false` is source-supported; inert tuples avoid exact and `0.1` fallback actions; scalar identity, exact erase-count decrease, detached lifetime, and `_Exit` on uncertainty are sensible. Phase/cardinality consistency remains B2. |
| Fresh, correlated observation | Not closed: B1 leaves reset, drain, and sticky-pair freshness unresolved. Saturation, persistent target counters, and same-iteration correlation are good design choices. |
| Exact interface/build/commands/evidence | Not closed: B3–B5 and absent concrete schemas/commands leave implementation-significant choices unfrozen. |

The prior **r14 approval remains limited to the r13 journal-policy contradiction**. It is not approval of D2, this observer, this launcher, or runtime behavior.

## 3. Optional advice

- Record Q1 scheduler-time margin at admission and window end; never retime a late target to salvage the run.
- Label Q0 as “no controlled application offers with a fresh observed-empty event,” unless a stronger interval oracle is explicitly adopted.
- Preserve the frozen packet’s pre-consent status fields. The outer consent envelope records the later authorization for this review; those historical fields need not be silently rewritten.
- Exact equality of host-wide IPC inventories is conservative and may produce unrelated-host-activity `INCONCLUSIVE` results. Do not weaken attribution or perform broad cleanup to obtain a pass.

## 4. Context sufficiency and omissions

The supplied full SPEC, companion manifest, proposal, history, and source excerpts are sufficient to identify these design blockers.

They are not sufficient to certify complete source or cross-artifact consistency. Material omissions include:

- an exact observer/CMake delta and concrete D2 harness contract;
- the five actual D2 schemas and literal checker entrypoints;
- complete hello-trigger behavior, IPC call contexts, and the completed-loop instrumentation site;
- exact service/supervisor files, run/finalizer argv, resource limits, and timeout state machine;
- a compatible static-provider provenance design.

The reported D1-H builds establish only reported compile/link evidence. The reported r14 tests are synthetic/documentary evidence. Neither establishes queue correctness, IPC isolation, cleanup success, lease continuity, or finalizer behavior.

## 5. Authority boundary

**This review grants no implementation, provisioning, configure/build, harness/finalizer, runtime, IPC, VM/SSH, cleanup/retry, D2/D3 execution or acceptance, commit, push, or further-call authority.** Any positive future documentary advice would likewise not replace a separate owner decision.
```

## FILE: Specs/RESULTS-SPEC-039/astra-review-v1.16-d2a-r15-20261013-submission.json

Role: Authoritative record that r15 was submitted once and returned HOLD.
Bytes: 4588
SHA-256: 63cd63ea814f391fb6bc7a73294bb5e5ce2377fdb683335a3d3b37d8e927bafe

```text
{
  "auto_reload": "not reported by the quota endpoint",
  "baseline": {
    "commit": "1af604dad66148618cc0e578c28adb4a9829e607",
    "tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b"
  },
  "date": "2026-10-13",
  "instructions": {
    "path": "astra-session-v1.16-d2a-r15-20261013/review-instructions.md",
    "sha256": "756f4a3d8eba508f92058b6aa0137c784237858a6441793eea31f648f891eb1f"
  },
  "manifest": {
    "path": "precode-manifest-v1.16-d2a-r15.json",
    "sha256": "4ed5646dcaab8af74344f3acba22c962fef8703b2d4d0fddd463c0381b9e3827"
  },
  "operations_not_authorized_or_performed": [
    "code edit",
    "configure/build",
    "UID/launcher/lease provisioning",
    "harness/finalizer runtime",
    "IPC",
    "VM/SSH",
    "cleanup/retry",
    "D2 acceptance",
    "D3",
    "commit",
    "push",
    "additional Astra call"
  ],
  "owner_authorization": {
    "calls_authorized": 1,
    "calls_made": 1,
    "credit_instruction": "Owner directed to consult the existing Codex usage session and said not to worry about subscription credits; review was made against included subscription usage, without purchasing flexible credits.",
    "exact_packet_sha256": "ab4855bdc79c81ad637903abc4071a892d64224a17519a60d0992a160226b7c1",
    "owner_message": "Eu nao estava aqui para autorizar. Mas, agora estou."
  },
  "packet": {
    "bytes": 194163,
    "path": "Specs/RESULTS-SPEC-039/precode-packet-v1.16-d2a-precode-20261013-sanitized-draft-r15.md",
    "sha256": "ab4855bdc79c81ad637903abc4071a892d64224a17519a60d0992a160226b7c1"
  },
  "proposal": {
    "path": "d2-launcher-lease-proposal-v1-20261013.md",
    "sha256": "ab386f6a94228ab9312efa4526e68b67ab43fc0f7936b789a9ebc459b59f67e6"
  },
  "provider_credit_balance": "0; no flexible credits reported or available in the response",
  "quota_headers_after_call": {
    "active-limit": "premium",
    "credits-balance": "0",
    "credits-has-credits": "False",
    "credits-unlimited": "False",
    "plan-type": "plus",
    "primary-used-percent": "2",
    "primary-window-minutes": "300",
    "safety-buffering-enabled": "true",
    "safety-buffering-faster-model": "gpt-6-luna",
    "secondary-used-percent": "13",
    "secondary-window-minutes": "10080"
  },
  "quota_observation_before_call": {
    "credits_balance": "0",
    "credits_has_credits": false,
    "overage_limit_reached": false,
    "primary_used_percent": 1,
    "recorded_at_utc": "2026-10-13T17:40:54.907161+00:00",
    "secondary_used_percent": 13,
    "session": "@session:default/20261010_102003_091b24"
  },
  "raw_sse": {
    "bytes": 829964,
    "copied_before_any_followup_call": true,
    "path": "astra-session-v1.16-d2a-r15-20261013/astra-review-v1.16-d2a-r15-20261013.raw.sse",
    "sha256": "f381eb4922b0d45322b9e38ae44278846c40c15b4f823b1471988ed715d5092e"
  },
  "receipt_type": "ng039-astra-documentary-review-submission/v1",
  "request": {
    "path": "astra-session-v1.16-d2a-r15-20261013/request-user-turn.md",
    "sha256": "4fc063f7c4d6ea3d3475616256d581ff70261e95a0de95df2c63b67dc34a8306"
  },
  "response": {
    "parser_duplicate_normalization": "NONE; raw SSE parser text matched the transcript byte-for-byte",
    "path": "astra-review-v1.16-d2a-r15-20261013.parsed.md",
    "sha256": "e6ea5ac8fde7da03214ac243d22982a563cd36d03d4f9bf895335275182a89d3"
  },
  "readable_response_derivative": {
    "path": "astra-review-v1.16-d2a-r15-20261013.md",
    "sha256": "aae8ee3d515744f947755ab3f67d3f097623be04356db5200535c86a559fe6f2",
    "normalization": "Derived readable copy: trailing whitespace stripped and one LF appended. The exact parser response is preserved unchanged in the parsed response file, session transcript, and raw SSE."
  },
  "scope": "D2a pre-code design readiness only; no implementation or operational authority",
  "session": {
    "path": "astra-session-v1.16-d2a-r15-20261013/ng039-v116-d2a-r15.jsonl",
    "sha256": "a78fd7a28ebc74d145d93e35ce25e1127ca6d4df25f48ddfc4010fd2a84c588f",
    "turns": 2,
    "user_turn_exactly_matches_archived_request": true
  },
  "spec": {
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025"
  },
  "usage": {
    "cached_tokens": 0,
    "credits": 16.90325,
    "input_tokens": 49385,
    "label": "ng039-v1.16-d2a-r15",
    "latency_s": 121.383,
    "model": "gpt-6-astra",
    "output_tokens": 3038,
    "plan": "plus",
    "primary_used_percent": "2",
    "secondary_used_percent": "13",
    "total_tokens": 52423,
    "ts": "2026-10-13T17:46:53Z"
  },
  "verdict": "HOLD"
}
```

## FILE: Specs/RESULTS-SPEC-039/precode-freeze-v1.16-d2a-r15-20261013.json

Role: Local freeze/identity checks; pre-submission metadata is historical.
Bytes: 2938
SHA-256: 7fc89795cd46efc6651650fd1e911b5d951daa65c9606b75eecd3e111d046678

```text
{
  "astra_calls": 0,
  "created_date": "2026-10-13",
  "integrity_checks": {
    "d1h_harness_embedded": true,
    "d1h_manifest_sanitized_embedded": true,
    "hold_response_embedded": true,
    "host_preflight_projection_nonempty": "PASS",
    "manifest_embedded_exactly": "PASS",
    "manifest_exact": true,
    "manifest_json_valid": true,
    "packet_and_manifest_hashes_match_header": "PASS",
    "private_home_paths_absent": "PASS",
    "proposal_exact": true,
    "r13_response_embedded": true,
    "r14_response_embedded": true,
    "source_excerpt_groups_embedded": true,
    "spec_exact": true
  },
  "launcher_proposal": {
    "path": "Specs/RESULTS-SPEC-039/d2-launcher-lease-proposal-v1-20261013.md",
    "sha256": "ab386f6a94228ab9312efa4526e68b67ab43fc0f7936b789a9ebc459b59f67e6",
    "status": "draft; not installed or tested"
  },
  "manifest": {
    "bytes": 15731,
    "path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.16-d2a-r15.json",
    "sha256": "4ed5646dcaab8af74344f3acba22c962fef8703b2d4d0fddd463c0381b9e3827"
  },
  "operations_not_performed": [
    "D2 code edit",
    "configure/build",
    "harness/finalizer",
    "runtime",
    "IPC",
    "UID/launcher/lease provisioning",
    "VM/SSH",
    "cleanup/retry",
    "commit",
    "push"
  ],
  "owner_exact_sha_consent": false,
  "packet": {
    "bytes": 194163,
    "path": "Specs/RESULTS-SPEC-039/precode-packet-v1.16-d2a-precode-20261013-sanitized-draft-r15.md",
    "sha256": "ab4855bdc79c81ad637903abc4071a892d64224a17519a60d0992a160226b7c1",
    "submitted": false
  },
  "parallel_review_batch": {
    "contract_source_pins_matched": true,
    "current_launcher_differs_from_reviewed_snapshot": true,
    "current_launcher_sha256": "ab386f6a94228ab9312efa4526e68b67ab43fc0f7936b789a9ebc459b59f67e6",
    "current_packet_reviewed_by_subagents": false,
    "delegation_id": "deleg_467bf700",
    "interpretation": "The three reports informed revisions; they do not approve the revised launcher proposal or this assembled packet.",
    "launcher_reviewed_sha256": "0ec971b93ccc662b6000d65162af18a9a66bf9567355e2cc45476f5b4a1190fd",
    "reviewers": 3
  },
  "receipt_type": "ng039-spec039-d2a-precode-local-freeze/v1",
  "remaining_review_questions": [
    "Q0 bootstrap-drain and observation event point",
    "Q0/Q1 two-process cardinality and per-run finalizer sequence",
    "workspace-root resolution and unit sandbox/IPC visibility",
    "byte-applicable observer/CMake diff readiness",
    "root-supervisor/lease FD continuity and receipt protection"
  ],
  "source_identity": {
    "baseline_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
    "functional_baseline": "1af604dad66148618cc0e578c28adb4a9829e607",
    "main_branch": "AIOPT5",
    "main_head": "756f493b7049434ab67cf2382486a48f606a470c",
    "spec_sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025"
  },
  "status": "LOCAL_DRAFT_FROZEN_UNSUBMITTED"
}
```

## FILE: Specs/RESULTS-SPEC-039/d2-host-readiness-preflight-20261013.json

Role: Read-only, name-filtered readiness evidence; not exhaustive or operational authority.
Bytes: 4194
SHA-256: 38d9d232b27476e89190b82ddc6538d1534362b647d14d904921ae6499ca51b6

```text
{
  "receipt_type": "ng039-d2-host-readiness-preflight/v1",
  "date": "2026-10-13",
  "status": "BLOCKED_PREPARATION_ONLY",
  "scope": "Read-only local host/worktree readiness checks to identify prerequisites for the SPEC-039 D2 gate. This is not a runtime or authorization receipt.",
  "owner_request": "Do the necessary preparation to reach the next D2 gate.",
  "repository": {
    "path": "/home/gandalf/workspace/novagenesis",
    "branch": "AIOPT5",
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "spec_sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025",
    "git_diff_check": "PASS",
    "preexisting_dirty_paths_preserved": [
      "Specs/SPEC-039-common-performance-measurement.md",
      "Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md",
      "Specs/RESULTS-SPEC-039/",
      "Specs/RESULTS-SPEC-059/"
    ]
  },
  "review_scope": {
    "r14_packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
    "r14_verdict": "APPROVE",
    "limitation": "Astra approved only the bounded documentary closure of the r13 journal-policy contradiction; it did not approve the full D2 profile, implementation, or runtime."
  },
  "host_checks": {
    "sudo_or_root_changes": false,
    "user_accounts": {
      "query": "local passwd entries whose names contain ng039, q0q1, or q0_q1",
      "matches": [],
      "limitation": "Name-filtered search only; no account name or UID for the future dedicated identity is currently pinned in this receipt."
    },
    "processes": {
      "query": "ps -eo user,uid,pid,ppid,comm filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination",
      "matches": []
    },
    "systemd": {
      "query": "systemctl list-unit-files --no-legend filtered for ng039, q0_q1_d2, gw_q0_q1, reservation-journal, supervisor-termination",
      "exit_code": 0,
      "matches": []
    },
    "checked_launcher_directories": [
      "/usr/local/sbin",
      "/usr/local/bin",
      "/home/gandalf/.local/bin",
      "/home/gandalf/.config/systemd/user",
      "/etc/systemd/system"
    ],
    "matching_named_launchers_or_units": [],
    "limitation": "The directory check was name-filtered over the listed locations, not an exhaustive privileged filesystem audit; absence of matching names is not proof that no arbitrary-named launcher/account exists."
  },
  "worktrees": {
    "primary_checkout_d2_files": {
      "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
      "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
    },
    "isolated_d1h_checkout": {
      "path": "/home/gandalf/workspace/novagenesis-ng039-q0q1-1af604d",
      "head": "1af604dad66148618cc0e578c28adb4a9829e607",
      "branch": "DETACHED",
      "status": [
        "M CMakeLists.txt",
        "?? tests/"
      ],
      "d1h_files": [
        "tests/spec039/gw_q0_q1_harness.cpp",
        "tests/spec039/q0_q1.manifest.json"
      ],
      "d2_files": {
        "tests/spec039/gw_q0_q1_d2.cpp": "ABSENT",
        "tests/spec039/q0_q1_d2.manifest.json": "ABSENT"
      },
      "actions": "Preserved; no edits/build/run performed during this preflight."
    }
  },
  "gates_still_required": [
    "Freeze the complete D2 pre-code profile and review packet, including the dedicated identity/launcher/lease design, exact allowlist, target graph, oracle, and supervisor/finalizer evidence contract.",
    "Obtain owner consent bound to the final exact packet SHA before any additional Astra call.",
    "Provision and independently verify the dedicated non-login UID, trusted single-run launcher, and exclusive lease; this host-side change is outside the five-file D2 test-code allowlist and was not performed.",
    "Record a separate owner execution decision before any D2 configure/build/harness/IPC run.",
    "D3 remains separate and requires D2 PASS plus its own explicit authorization."
  ],
  "not_performed": [
    "No SPEC or production source edit",
    "No UID/group/service/launcher/lease provisioning",
    "No sudo",
    "No build/configure",
    "No D2 harness or finalizer execution",
    "No IPC creation/removal or runtime test",
    "No Astra call",
    "No commit or push"
  ]
}
```

## FILE: Specs/RESULTS-SPEC-039/precode-validation-v1.16-r14-20261013.json

Role: Historical synthetic/documentary validation only; not D2 runtime.
Bytes: 5876
SHA-256: ebafdb331e823f25975e4e9c7f85cc03fe75e809e64253ed177f8b1ec77931af

```text
{
  "receipt_type": "ng039-spec039-local-validation/v1",
  "created_date": "2026-10-13",
  "status": "PASS_WITH_EXPLICIT_LIMITATIONS",
  "purpose": "Read-only validation of the owner-authorized same-SPEC-039 v1.16/r14 documentary amendment; no Astra follow-up was sent.",
  "owner_authorization": {
    "message": "Sim — autorizo a emenda na mesma SPEC-039 e validação local, sem novo Astra nem runtime",
    "scope": "SPEC-039 documentation, corresponding manifest/packet projections and offline local validation only",
    "new_astra_call_authorized": false,
    "build_runtime_ipc_vm_commit_push_authorized": false
  },
  "identity": {
    "repository": "/home/gandalf/workspace/novagenesis",
    "branch": "AIOPT5",
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "spec_path": "Specs/SPEC-039-common-performance-measurement.md",
    "spec_sha256": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025",
    "manifest_path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.16-r14.json",
    "manifest_sha256": "f04b5d6333b14db6dc91a4cd0b7afd5f77c2f148797d1f0631b843457c1e393f",
    "packet_path": "Specs/RESULTS-SPEC-039/precode-packet-v1.16-amend-closure-20261013-sanitized-draft-r14.md",
    "packet_sha256": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95",
    "packet_bytes": 182410,
    "packet_submitted": false,
    "r14_exact_sha_consent": false
  },
  "amendment_summary": {
    "reviewed_r13_packet_sha256": "0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727",
    "r13_verdict": "AMEND",
    "new_requirement_id": "AUTH-005",
    "model_version": 2,
    "policy": "Bounded read-only parsing/replay of reservation-journal.jsonl is solely for COUT-001..COUT-004. Invalid, unavailable, malformed, truncated or incomplete history yields unknown without blocking an otherwise authorized abnormal INCONCLUSIVE record. Other non-control evidence remains raw-only; journal mutation, IPC inspection, cleanup and retry remain forbidden.",
    "same_spec_number": "SPEC-039"
  },
  "validation": {
    "unit_tests": {
      "normal": {
        "command": "PYTHONDONTWRITEBYTECODE=1 python3 -m unittest -q test_spec039_journal_transitions test_spec039_projection_checker test_spec039_methodology_cli",
        "model_projection_methods": 60,
        "cli_methods": 3,
        "total_methods": 63,
        "failures": 0,
        "errors": 0,
        "status": "PASS"
      },
      "optimized": {
        "command": "PYTHONDONTWRITEBYTECODE=1 PYTHONOPTIMIZE=1 python3 -m unittest -q test_spec039_journal_transitions test_spec039_projection_checker test_spec039_methodology_cli",
        "model_projection_methods": 60,
        "cli_methods": 3,
        "total_methods": 63,
        "failures": 0,
        "errors": 0,
        "status": "PASS"
      },
      "test_first_regression": {
        "red": "PASS: the new bounded-journal-policy test first failed because model_version 2 was unsupported by the old checker.",
        "green": "PASS: policy validation, projections and the completed suite passed after the amendment."
      }
    },
    "live_read_only_checker": {
      "command": "PYTHONDONTWRITEBYTECODE=1 python3 verify_spec039_methodology_v1.py --repo-root /home/gandalf/workspace/novagenesis --spec Specs/SPEC-039-common-performance-measurement.md --manifest Specs/RESULTS-SPEC-039/precode-manifest-v1.16-r14.json --packet Specs/RESULTS-SPEC-039/precode-packet-v1.16-amend-closure-20261013-sanitized-draft-r14.md --expected-head 756f493b7049434ab67cf2382486a48f606a470c",
      "status": "PASS",
      "errors": [],
      "writes_performed": false,
      "hashes": {
        "spec": "f3f467b85e8d53e32035b7c842be45ddbf0467a6c98051e2132bed48bbff8025",
        "manifest": "f04b5d6333b14db6dc91a4cd0b7afd5f77c2f148797d1f0631b843457c1e393f",
        "packet": "c5d42e42ea1fcb015b393de59819fa7c89669cc9e56ba44a13e9c816b3c17e95"
      },
      "policy_text_projections_checked": 17,
      "cross_artifact_pid_set": "NOT_EXECUTED",
      "runtime_finalizer": "NOT_EXECUTED",
      "Astra": "NOT_SUBMITTED"
    },
    "stale_r13_negative_probe": {
      "status": "EXPECTED_FAIL_CLOSED",
      "inputs": "live v1.16 SPEC with the frozen r13 manifest and packet",
      "writes_performed": false,
      "purpose": "prove stale model, policy and artifact pins cannot pass against the v1.16 contract"
    },
    "schema_meta_validation": {
      "status": "PASS",
      "python": "3.12.3",
      "jsonschema": "4.10.3",
      "draft": "2020-12",
      "schemas_checked": 5,
      "schema_names": [
        "ipc_inventory",
        "observer_events",
        "reservation_journal_event",
        "run_record",
        "supervisor_termination"
      ],
      "method": "Draft202012Validator.check_schema"
    },
    "git_diff_check": "PASS"
  },
  "r13_immutability": {
    "manifest_sha256": "2f378565c90a10b4b08abcc9f0469993dd243cd73cd56c8477f4710408c5a8a5",
    "packet_sha256": "0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727",
    "response_sha256": "94c118fe12b2b27143a521b43be5ea194d4d0969b4ea35c8829d868d91af47d0",
    "raw_sse_sha256": "b819bf078d2690fdfbb945e123873d66d469a72f819a3a4a38e3c6282b7a32b7",
    "submission_receipt_sha256": "45b604e15399cfb087bad6ab9b16a03d8013df19d75a0fdae5f229f8aecc1a33",
    "all_verified_unchanged": true,
    "r13_tool_snapshot_manifest_sha256": "c4846cb9c9faef42b33224e858bd80fa74a6643233a2a201fb97d17408ac9fbb"
  },
  "evidence_boundary": {
    "offline_synthetic_and_documentary_only": true,
    "production_finalizer": "NOT_EXECUTED",
    "build_or_configure": "NOT_EXECUTED",
    "runtime_correctness_performance": "NOT_EXECUTED",
    "ipc_vm_ssh": "NOT_EXECUTED",
    "cross_artifact_pid_set": "NOT_EXECUTED",
    "Astra_followup": "NOT_SUBMITTED; exact-SHA consent not granted",
    "commit_push": "NOT_PERFORMED",
    "D2_D3": "BLOCKED"
  }
}
```

## FILE: Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json

Role: Historical compile/link-only evidence; executable not run.
Bytes: 2391
SHA-256: 3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28

```text
{
  "binary": {
    "bytes": 7366600,
    "executed": false,
    "path": "<D1H_REBUILD_ROOT>/build/ng039_gw_q0_q1_harness",
    "sha256": "060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d"
  },
  "build": {
    "argv": [
      "cmake",
      "--build",
      "<D1H_REBUILD_ROOT>/build",
      "--target",
      "ng039_gw_q0_q1_harness",
      "--parallel",
      "2"
    ],
    "compiler": "/usr/bin/c++",
    "compiler_version": "GNU 13.3.0",
    "exit_code": 0,
    "log": "<D1H_REBUILD_ROOT>/build.log",
    "log_bytes": 8936,
    "log_sha256": "658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98",
    "reported_warnings": 2
  },
  "configure": {
    "argv": [
      "cmake",
      "-S",
      "<D1H_WORKTREE>",
      "-B",
      "<D1H_REBUILD_ROOT>/build",
      "-DNG_ENABLE_NG039_GW_Q0_Q1_HARNESS=ON",
      "-DNG_ENABLE_LEGACY_STANDALONE=OFF"
    ],
    "cmake_version": "3.28.3",
    "exit_code": 0,
    "generator": "Unix Makefiles",
    "log": "<D1H_REBUILD_ROOT>/configure.log",
    "log_bytes": 652,
    "log_sha256": "65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb"
  },
  "d1h_manifest_sha256": "9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a",
  "distinction_from_lost_artifacts": "This is a fresh build with new hashes; it does not recover or independently rehash the historical binary/log hashes in the original D1-H manifest.",
  "environment_capture": "Inherited environment was used for this D1-H reconstruction and was not snapshotted; the separate r07 D2 plan specifies a minimal explicit env -i environment.",
  "gate": "D1-H compile/link only",
  "output_is_outside_hermes": true,
  "output_root": "workspace/ng039-d1h-rebuild-20261010",
  "rebuild_date": "2026-10-10",
  "schema_version": "ng039-d1h-build-receipt/v1",
  "scope": "Compile/link only. No harness execution, IPC, namespace, correctness test, timing, D2, D3, VM/SSH, commit or push.",
  "source_head": "1af604dad66148618cc0e578c28adb4a9829e607",
  "source_sha256": {
    "CMakeLists.txt": "d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d",
    "tests/spec039/gw_q0_q1_harness.cpp": "15913ee8b82fed7ad259668ed84e6e261aa444a3df443f3521929eba01b8dc92"
  },
  "source_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
  "source_worktree": "<D1H_WORKTREE>",
  "task_id": "NG-039",
  "working_directory": "<D1H_WORKTREE>"
}
```

## FILE: baseline@1af604d:Common/src/GW.cpp

Role: Exact source bytes from the pinned functional baseline; read-only design context.
Bytes: 42128
SHA-256: 7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e

```text
/*
        NovaGenesis

        Name:		Gateway
        Object:		GW
        File:		GW.cpp
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.2

        Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _GWMSGCL01_H
#include "GWMsgCl01.h"
#endif

#ifndef _GWRUNINITIALIZATION01_H
#include "GWRunInitialization01.h"
#endif

#ifndef _GWHelloIPC02_H
#include "GWHelloIPC02.h"
#endif

#ifndef _GWRUNHELLOIPC02_H
#include "GWRunHelloIPC02.h"
#endif

#ifndef _GWEXPOSITION02_H
#include "GWExposition02.h"
#endif

// #define DEBUG // To follow message processing

#define LG(msg) S << endl \
                  << "[" << fixed << setprecision(3) << GetTime() << "s]        " << msg << endl
// #define DEBUG_NETWORK_QUEUE  // Uncomment to log NetworkReceiveQueue processing

// #define DEBUG_NETWORK_QUEUE

// #define DEBUG1  // To follow shared memory access
// #define DEBUG3 // Even more on shm access

union semun
{
  int val;              /* used for SETVAL only */
  struct semid_ds* buf; /* for IPC_STAT and IPC_SET, not discussed here */
  ushort* array;        /* used for GETALL and SETALL */
};

GW::GW(string _LN, Process* _PP, unsigned int _Index, string _Path)
    : Block(_LN, _PP, _Index, _Path)
{
  InputQueueTag = 0;
  OutputQueueTag = 0;
  StopGateway = false;
  State = "initialization";
  ScheduleStatusFlag = false;
  MaxSegmentSize = MAX_MESSAGE_SIZE;

  // Setting the delays
  DelayBeforeStatusIPC = 5;

  // Initialize output message notification flag
  NewOutputMessage = false;

  // SPEC-006: Named semaphore "Output_Queue" removed from constructor.
  // OutputQueueMutex alone protects OutputQueues.

  Action* PA = 0;
  CommandLine* PCL = 0;
  Block* PHTB = 0;
  Message* PIM = 0;
  Message* InlineResponseMessage = NULL;

  PP->GetBlock("HT", PHTB);

  PHT = (HT*)PHTB;

  // Creating the actions
  NewAction("-run --initialization 0.1", PA);
  NewAction("-m --cl 0.1", PA);
  NewAction("-hello --ipc 0.2", PA);      // GWHelloIPC02 - receiver (replaces GWHelloIPC01)
  NewAction("-hello --ipc 2.0", PA);      // GWHelloIPC02 - receiver with HT BID support
  NewAction("-run --helloIPC 0.2", PA);   // GWRunHelloIPC02 - periodic emitter
  NewAction("-exposition 0.2", PA);       // GWExposition02 - periodic peer exposition on PGCS
  NewAction("-run --exposition 0.2", PA); // GWExposition02 - periodic trigger

  // Creating a -run --initialization message
  int PIMStatus = PP->NewMessage(GetTime(), 0, false, PIM);

  // Adding only the run initialization command line and executing it when the
  // message allocation succeeded. On allocation failure, leave the gateway
  // constructible without inspecting or marking an unowned output.
  if (PIMStatus == OK && PIM != 0)
  {
    PIM->NewCommandLine("-run", "--initialization", "0.1", PCL);

    // Run
    Run(PIM, InlineResponseMessage);

    // Mark to delete
    PIM->MarkToDelete();
  }
}

GW::~GW()
{

  vector<Action*>::iterator it4;

  Action* Temp;

  for (it4 = Actions.begin(); it4 != Actions.end(); it4++)
  {
    Temp = *it4;

    if (Temp != 0)
    {
      delete Temp;
    }

    Temp = 0;
  }
}

// Allocate and add an Action on Actions container
void GW::NewAction(const string _LN, Action*& _PA)
{
  if (_LN == "-m --cl 0.1")
  {
    GWMsgCl01* P = new GWMsgCl01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --initialization 0.1")
  {
    GWRunInitialization01* P = new GWRunInitialization01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-hello --ipc 0.2")
  {
    GWHelloIPC02* P = new GWHelloIPC02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-hello --ipc 2.0")
  {
    GWHelloIPC02* P = new GWHelloIPC02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --helloIPC 0.2")
  {
    GWRunHelloIPC02* P = new GWRunHelloIPC02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-exposition 0.2")
  {
    GWExposition02* P = new GWExposition02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --exposition 0.2")
  {
    GWExposition02* P = new GWExposition02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }
}

// Get an Action
int GW::GetAction(string _LN, Action*& _PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int GW::DeleteAction(string _LN)
{
  int Status = ERROR;
  return Status;
}

// Push a Message into the input message priority queue
// Be aware to lock Input queue mutex (IQM) before accessing this function
void GW::PushToInputQueue(Message* M)
{
  unsigned int NoCL = 0;

  if (M == 0)
  {
    S << "          (ERROR: The message being store in input queue is corrupted at input queue)" << endl;
    return;
  }

#ifdef DEBUG

  S << "[1]       (Pushing the following message to InputQueue. Size = " << InputQueue.size()
    << ". Message instantiation number is " << M->InstantiationNumber << ")" << endl;

  S << "(" << endl
    << *M << ")" << endl;

#endif

  if (M != 0)
  {
    if (M->GetNumberofCommandLines(NoCL) == OK)
    {
      if (NoCL > 2)
      {
        // Avoid deleting the message
        M->UnmarkToDelete();

        // Set the message tag
        M->SetTag(InputQueueTag);

        // Lock for push + notify (thread-safe w.r.t. Gateway() pop)
        {
          std::lock_guard<std::mutex> lock(InputQueueMutex);

          // Push the message to the queue
          InputQueue.push(M);

          // Increases the tag counter
          InputQueueTag++;
        }
        // Notify after unlock to minimize time input thread is blocked
        InputQueueCV.notify_one();
      }
      else
      {
        // Mark to delete the message
        M->MarkToDelete();
      }
    }
    else
    {
      S << "          (ERROR: Unable to read the number of command lines at input queue)" << endl;
      M->MarkToDelete();
    }
  }
}

// Push a Message into the output message priority queue
void GW::PushToOutputQueue(std::string OQS, Message* M)
{
  unsigned int NoCL = 0;
  size_t MaxSegmentSize = MAX_MESSAGE_SIZE;
  long long MessageSize = 0;
  string Offset = "          ";

#ifdef DEBUG
  S << "[6]       (Pushing the following message to the OutputQueue " << OQS << ". Size = " << OutputQueues[OQS].size() << ")" << endl;
  S << "(" << endl
    << *M << ")" << endl;
#endif

  if (M != 0)
  {
    M->ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray();

    if (M->GetMessageSize(MessageSize) == OK)
    {
      if (MessageSize > 0 && MessageSize <= (MaxSegmentSize - 8))
      {
        if (M->GetNumberofCommandLines(NoCL) == OK)
        {
          if (NoCL > 2)
          {
            M->UnmarkToDelete();
            M->SetTag(OutputQueueTag);

            // Lock for push + notify (thread-safe w.r.t. ReadFromOutputQueue)
            {
              std::lock_guard<std::mutex> lock(OutputQueueMutex);
              OutputQueues[OQS].push(M);
              OutputQueueTag++;
              NewOutputMessage = true;
            }
            OutputQueueCV.notify_one();
          }
          else
          {
            S << Offset << "(ERROR: The message has less than 3 command lines at output queue)" << endl;
            M->MarkToDelete();
          }
        }
        else
        {
          S << Offset << "(ERROR: Unable to read the number of command lines at output queue)" << endl;
          M->MarkToDelete();
        }
      }
      else
      {
        S << Offset << "(ERROR: Invalid message size at output queue)" << endl;
        M->MarkToDelete();
      }
    }
    else
    {
      S << Offset << "(ERROR: Unable to get the message size at output queue)" << endl;
      M->MarkToDelete();
    }
  }
  else
  {
    S << "          (ERROR: The message being store in output queue is corrupted)" << endl;
    M->MarkToDelete();
  }
}

// Read a Message from output message priority queues. Only the GW can forward messages to shared memory instances
// SPEC-007: Decouple WriteToSharedMemory3 from OutputQueueMutex. Pop at most
// one message per queue into a local batch, write each to SHM outside the lock,
// re-push failures under lock. Re-evaluate NewOutputMessage after partial drain
// to prevent the output thread from sleeping with pending messages.
void GW::ReadFromOutputQueue()
{
  Message* PM1 = NULL;
  std::string currentOQS;

  while (StopGateway == false)
  {
    // Wait for output messages or stop flag (blocking wait — zero CPU when idle)
    {
      std::unique_lock<std::mutex> lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [this]()
                         { return NewOutputMessage || StopGateway; });
      if (StopGateway)
        break;
      NewOutputMessage = false;
    }

    // Phase 1: Pop at most one message per queue (under lock — microseconds)
    std::vector<std::pair<std::string, Message*>> batch;
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);

      for (auto it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        if (!it->second.empty())
        {
          PM1 = it->second.top();
          it->second.pop();
          batch.emplace_back(it->first, PM1);
        }
      }
    } // Lock released

    if (batch.empty())
      continue;

    // Phase 2: Write each message to SHM (OUTSIDE the lock)
    bool anyFailed = false;

    for (auto& kv : batch)
    {
      if (WriteToSharedMemory3(kv.first, kv.second) == OK)
      {
        kv.second->MarkToDelete();
      }
      else
      {
        // SHM busy — re-push under lock for retry on next cycle
        {
          std::lock_guard<std::mutex> qlock(OutputQueueMutex);
          OutputQueues[kv.first].push(kv.second);
        }
        anyFailed = true;
      }
    }

    // Re-evaluate NewOutputMessage: check if queues still have work
    // (critical: without this, the output thread could sleep with pending
    // messages because NewOutputMessage was cleared before the cycle started)
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);
      bool hasMoreWork = false;
      for (auto it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        if (!it->second.empty())
        {
          hasMoreWork = true;
          break;
        }
      }
      NewOutputMessage = hasMoreWork;
    }

    // Backpressure: brief sleep if any message failed (peer SHM still busy)
    if (anyFailed)
    {
      tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
    }
  }
}

// Read Messages from the queues and forward to other processes via IPC/IHC
void GW::Gateway()
{
  Message* PM1 = NULL;
  Message* PM2 = NULL;
  bool RunFlag = false;
  unsigned int NumberOfCycles = 0;
  string Offset = "          ";
  double ScheduledTime = 0;
  double Time = 0;
  long long int MessageSize = 0;
  std::chrono::milliseconds waitTimeout;
  constexpr long long SHM_POLL_INTERVAL_MS = 1; // SHM poll every 1ms (SPEC-007a)
  double secondsUntilNext = 1.0;                // default 1s when queue empty

  // Start output queue thread
  tthread::thread* T = new tthread::thread(&GW::ReadFromOutputQueueThreadWrapper, this);

  while (StopGateway == false)
  {
    // Calculate how long to wait: time until next scheduled message
    {
      std::lock_guard<std::mutex> lock(InputQueueMutex);
      if (!InputQueue.empty())
      {
        double nextTime = InputQueue.top()->GetTime();
        double now = GetTime();
        secondsUntilNext = nextTime - now;
        if (secondsUntilNext < 0)
          secondsUntilNext = 0;
      }
      else
      {
        secondsUntilNext = 1.0; // no messages, check SHM every 1s max
      }
    }

    // Wait for input queue or stop flag (timer-aware blocking wait)
    {
      std::unique_lock<std::mutex> lock(InputQueueMutex);
      waitTimeout = std::chrono::milliseconds(
          std::min((long long)(secondsUntilNext * 1000), SHM_POLL_INTERVAL_MS));
      InputQueueCV.wait_for(lock, waitTimeout,
                            [this]()
                            { return !InputQueue.empty() || StopGateway; });
      if (StopGateway)
        break;
    }

    // Lock, pop all due messages, unlock — minimize critical section
    PM1 = NULL;
    RunFlag = false;
    Time = GetTime();

    {
      std::lock_guard<std::mutex> lock(InputQueueMutex);
      if (!InputQueue.empty())
      {
        PM1 = InputQueue.top();
        ScheduledTime = PM1->GetTime();
        if (ScheduledTime < Time)
        {
          InputQueue.pop();
          RunFlag = true;
        }
        else
        {
          PM1 = NULL; // not yet due, leave in queue
        }
      }
    }

    // Step 2 : Run procedure to interpret and run the received message
    if (PM1 != NULL && RunFlag == true)
    {

#ifdef DEBUG

      S << endl
        << setprecision(10) << "          (t = " << Time << ")" << endl;
      S << "[1]       (IQ Size = " << InputQueue.size() << ")" << endl;

      map<std::string, priority_queue<Message*, vector<Message*>, DereferenceCompareNode>>::iterator it;
      unsigned int Counter = 0;
      for (it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        Counter++;
        S << "[6]       (OQ[" << Counter << "] Size = " << it->second.size() << ")" << endl;
      }
      S << Offset << "(Messages in memory = " << PP->GetNumberOfMessages() << ")" << endl;

      S << "[7]       Current peer processes on this OS are:" << endl;

      // List peers from category 20 (hello IPC discovered peers)

      vector<string> Cat20Keys;
      if (PHT->GetBindingKeys(20, Cat20Keys) == OK)
      {
        for (unsigned int i = 0; i < Cat20Keys.size(); i++)
        {
          vector<string>* Cat20Vals = new vector<string>;
          if (PHT->GetBinding(20, Cat20Keys.at(i), Cat20Vals) == OK)
          {
            S << "            Cat[20] " << Cat20Keys.at(i);
            for (unsigned int j = 0; j < Cat20Vals->size(); j++)
            {
              S << " -> " << Cat20Vals->at(j);
            }
            S << endl;
          }
          delete Cat20Vals;
        }
      }

      // List peers from category 19 (hello IPC discovered peers)

      vector<string> Cat19Keys;
      if (PHT->GetBindingKeys(19, Cat19Keys) == OK)
      {
        for (unsigned int j = 0; j < Cat19Keys.size(); j++)
        {
          vector<string>* Cat19Vals = new vector<string>;
          if (PHT->GetBinding(19, Cat19Keys.at(j), Cat19Vals) == OK)
          {
            S << "            Cat[19] " << Cat19Keys.at(j);
            for (unsigned int k = 0; k < Cat19Vals->size(); k++)
            {
              S << " -> " << Cat19Vals->at(k);
            }
            S << endl;
          }
          delete Cat19Vals;
        }
      }

      // List peers from category 15 (hello IHC discovered peers)

      vector<string> Cat15Keys;
      if (PHT->GetBindingKeys(15, Cat15Keys) == OK)
      {
        for (unsigned int j = 0; j < Cat15Keys.size(); j++)
        {
          vector<string>* Cat15Vals = new vector<string>;
          if (PHT->GetBinding(15, Cat15Keys.at(j), Cat15Vals) == OK)
          {
            S << "            Cat[15] " << Cat15Keys.at(j);
            for (unsigned int k = 0; k < Cat15Vals->size(); k++)
            {
              S << " -> " << Cat15Vals->at(k);
            }
            S << endl;
          }
          delete Cat15Vals;
        }
      }

#endif

      Run(PM1, PM2);

      PP->DeleteMarkedMessages();

      PM1 = NULL;
      RunFlag = false;
    }

    // Step 3 : NGAL: drain the NetworkReceiveQueue
    // The ReceiveDispatcher pushes raw char buffers here. The GW thread
    // does NewMessage + deserialisation + PushToInputQueue.
    {
      std::lock_guard<std::mutex> lock(NetworkReceiveQueueMutex);
      while (!NetworkReceiveQueue.empty())
      {
        auto entry = NetworkReceiveQueue.front();
        NetworkReceiveQueue.pop();

        char* buffer = entry.first;
        long long size = entry.second;

        if (buffer != 0 && size > 0)
        {
          Message* PM = NULL;
          if (PP->NewMessage(0, 0, false, PM) == OK)
          {
          #ifdef DEBUG_NETWORK_QUEUE
            cerr << "[DEBUG] GW::Gateway: NETWORK_QUEUE_DELIVERED size=" << size 
                 << " queue_remaining=" << NetworkReceiveQueue.size() << endl;
          #endif
            PM->SetMessageFromCharArray(buffer, size);
            PM->ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();
            PushToInputQueue(PM);
          }
        }

        delete[] buffer;
      }
    }

    // Step 4 : Read OS IPC — poll shared memory for messages from other processes
    // Phase 2: rate limit to 100ms when no due messages to process (idle or waiting for future messages)
    {
      static auto lastSHMPoll = std::chrono::steady_clock::now();
      auto now = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSHMPoll).count();
      bool hasDueMessage = false;
      {
        std::lock_guard<std::mutex> lock(InputQueueMutex);
        if (!InputQueue.empty())
        {
          double nextTime = InputQueue.top()->GetTime();
          double currentTime = GetTime();
          if (nextTime <= currentTime)
            hasDueMessage = true;
        }
      }
      if (!hasDueMessage && elapsed >= SHM_POLL_INTERVAL_MS)
      {
        ReadFromSharedMemory3();
        lastSHMPoll = std::chrono::steady_clock::now();
      }
    }

    NumberOfCycles++;
  }

  // Stop the output thread
  if (StopGateway == false)
  {
    StopGateway = true;
    OutputQueueCV.notify_all();
  }

  T->join();
  delete T;
}

int GW::ReadFromSharedMemory3()
{
  int Status = ERROR;
  string Offset = "          ";
  double StartingTime;

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
       z++) // Modified in 9th April 2021 to deal with parallel shared memories.
  {
    sem_t* mutex;
    void* shm_address;
    unsigned char* data;     // Manipulate the content on shared memory
    long long TotalSize = 0; // The total size of the message being transferred
    // The shared memory segment address
    string SemaphoreName;

    // Set the semaphore name
    SemaphoreName = IntToString(
        PP->Key + z); // Modified in 9th April 2021 to deal with parallel shared memories.

    // Cache semaphore to avoid sem_open/sem_close per iteration (Phase 2)
    mutex = CachedSemaphores[SemaphoreName];
    if (mutex == NULL)
    {
      mutex = sem_open(SemaphoreName.c_str(), O_CREAT, 0666, 1);
      if (mutex != SEM_FAILED)
      {
        CachedSemaphores[SemaphoreName] = mutex;
      }
    }

    // Check for error on semaphore open
    if (mutex != NULL && mutex != SEM_FAILED)
    {

#ifdef DEBUG3

      S << Offset << "(Opened the semaphore " << SemaphoreName << ")" << endl;

#endif

      if (PP->shmid[z] == -1)
      { // Modified in 9th April 2021 to deal with parallel shared memories.
        // Create and connect to the segment
        if ((PP->shmid[z] = shmget(PP->Key + z, MaxSegmentSize, IPC_CREAT | 0666)) < 0)
        { // Modified in 9th April 2021 to deal with parallel shared memories.
          perror("shmget");
          exit(1);
        }

        //(shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)

        LG("(Created an input shared memory segment with key = " << (PP->Key + z)
                                                                 << " and identifier = " << PP->shmid[z] << ")");
      }
      else
      {
        // Test to see if it worked
        // Modified in 9th April 2021 to deal with parallel shared memories.
        if ((shm_address = shmat(PP->shmid[z], NULL, 0)) != NULL)
        {

          // Retry sem_trywait with backoff to avoid CPU spin
          int LockAttempts = 0;
          while (sem_trywait(mutex) != 0 && LockAttempts < 100)
          {
            tthread::this_thread::sleep_for(tthread::chrono::microseconds(100));
            LockAttempts++;
          }
          if (LockAttempts < 100)
          {
#ifdef DEBUG3
            S << Offset << "(Locked the semaphore " << SemaphoreName << ")" << endl;
#endif

            // *****************************
            // Start receiving a new message
            // *****************************

            data = (unsigned char*)shm_address;

            // Test to see if it worked
            if (data != (unsigned char*)(-1))
            {
              if (data[0] == (unsigned char)'w')
              {
                StartingTime = GetTime();

#ifdef DEBUG1
                S << endl
                  << endl
                  << "          (Time = " << StartingTime << ")" << endl;

                S << "[0]       (Reading from shared memory)" << endl;

                S << "          (The value of the first byte in shared memory is " << (char)data[0]
                  << " (f = Free to write, w = still waiting for peer reading)" << endl;

                S << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

                unsigned char* HeaderSizeField = new unsigned char[sizeof(TotalSize)];

                for (int i = 0; i < 8; i++)
                {
                  HeaderSizeField[i] = data[i + 1];

                  // printf("%i %d %c \n",i,data[i],data[i]);
                }

                unsigned long long A = (unsigned long long)HeaderSizeField[0];
                unsigned long long B = (unsigned long long)HeaderSizeField[1];
                unsigned long long C = (unsigned long long)HeaderSizeField[2];
                unsigned long long D = (unsigned long long)HeaderSizeField[3];
                unsigned long long E = (unsigned long long)HeaderSizeField[4];
                unsigned long long F = (unsigned long long)HeaderSizeField[5];
                unsigned long long G = (unsigned long long)HeaderSizeField[6];
                unsigned long long H = (unsigned long long)HeaderSizeField[7];

                TotalSize = (A << 56) |
                            (B << 48) |
                            (C << 40) |
                            (D << 32) |
                            (E << 24) |
                            (F << 16) |
                            (G << 8) |
                            H;

                if (TotalSize > 0 && TotalSize < MAX_MESSAGE_SIZE)
                {
#ifdef DEBUG1
                  // S << Offset << "(TotalSize = "<<TotalSize<<")"<< endl;

                  // S << Offset << "A = " <<A<<endl;
                  // S << Offset << "B = " <<B<<endl;
                  // S << Offset << "C = " <<C<<endl;
                  // S << Offset << "D = " <<D<<endl;
                  // S << Offset << "E = " <<E<<endl;
                  // S << Offset << "G = " <<G<<endl;
                  // S << Offset << "H = " <<H<<endl;
#endif

                  char* Payload = new char[TotalSize - 8];

                  // Read all the message
                  for (int j = 8; j < TotalSize; j++)
                  {
                    Payload[j - 8] = (char)data[j + 1];
                  }

                  double TimeStamp = 0;

                  // Read the timestamp header
                  unsigned char* HeaderTimeStampField = new unsigned char[8];

                  for (int n = 0; n < 8; n++)
                  {
                    HeaderTimeStampField[n] = data[TotalSize + 1 + n];

                    // printf("%i %d %c \n",i,data[i],data[i]);
                  }

                  unsigned long long I = (unsigned long long)HeaderTimeStampField[0];
                  unsigned long long J = (unsigned long long)HeaderTimeStampField[1];
                  unsigned long long K = (unsigned long long)HeaderTimeStampField[2];
                  unsigned long long L = (unsigned long long)HeaderTimeStampField[3];
                  unsigned long long M = (unsigned long long)HeaderTimeStampField[4];
                  unsigned long long N = (unsigned long long)HeaderTimeStampField[5];
                  unsigned long long O = (unsigned long long)HeaderTimeStampField[6];
                  unsigned long long P = (unsigned long long)HeaderTimeStampField[7];

                  unsigned long long TempTimeStamp;

                  TempTimeStamp = (I << 56) |
                                  (J << 48) |
                                  (K << 40) |
                                  (L << 32) |
                                  (M << 24) |
                                  (N << 16) |
                                  (O << 8) |
                                  P;

                  TimeStamp = ((double)(TempTimeStamp)) / 10e9;

                  // Allocate a new Message object
                  Message* PM = 0;

                  const int PMStatus = PP->NewMessage(GetTime(), 0, false, PM);
                  if (PMStatus == OK && PM != 0)
                  {
                    PM->SetMessageFromCharArray(Payload, (TotalSize - 8));

                    PM->ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();

#ifdef DEBUG1
                    long long SS;
                    long long SP;
                    PM->GetMessageSize(SS);
                    PM->GetPayloadSize(SP);

                    S << Offset << "(The total size of the message is = " << TotalSize - 8 << ")" << endl;
                    S << Offset << "(The total size of the message is = " << SS << ")" << endl;
                    S << Offset << "(The total size of the payload content in message is = " << SP << ")" << endl;

                    if (TotalSize < 3000 && SP > 1)
                    {
                      S << Offset << "(Showing what is on the payload array. Limited to messages smaller than 3000 bytes)" << endl;

                      for (unsigned int l = 0; l < (TotalSize - 8); l++)
                      {
                        printf("%i %d %c \n", l, Payload[l], Payload[l]);
                      }

                      S << Offset << "(Shown)" << endl;
                    }
#endif

#ifdef DEBUG2
                    S << Offset << "(MessageSize = " << (TotalSize - 8) << ")" << endl;

                    S << Offset
                      << "(The following message was received from another process in the same OS)"
                      << endl;

                    S << "(" << endl
                      << *PM << endl
                      << ")" << endl;
#endif

                    // Pushing message to the queue
                    PushToInputQueue(PM);
                    Status = OK;
                  }

                  delete[] HeaderTimeStampField;

                  delete[] Payload;

#ifdef DEBUG3
                  S << Offset << "(Freeing the shared memory to the peer)" << endl;
#endif

                  // Clean the memory segment
                  memset(data, (unsigned char)'f', 1);

                  // S << Offset << "(Finished reading)"<< endl;

                  // S << Offset << "(Time = "<<GetTime()<<")"<<endl;

                } // (TotalSize < 0 && TotalSize > 1073741816) means that the message is with error or misconfigured
                else
                {
                  // Total size is negative or zero
                  // Clean the memory segment
                  memset(data, (unsigned char)'f', MaxSegmentSize);
                }

                delete[] HeaderSizeField;
              }

            } // (data == (unsigned char *)(-1)) means that the data at the SHM was not set properly
            else
            {
              S << Offset << "(Warning: unable to read the data on the shared memory)" << endl;

              perror("reading SHM : shmat");
            }

            if (sem_post(mutex) == 0)
            {
#ifdef DEBUG3
              S << Offset << "(Unlocked the semaphore " << SemaphoreName << ")" << endl;
#endif
            }
            else
            {
              perror("reading SHM : sem_post");
            }

          } // (LockAttempts < 100) successfully locked

          // Detach from shared memory
          if (shmdt(shm_address) == -1)
          {
            perror("reading SHM : shmdt");
          }

        } // (shm_address = shmat(PP->shmid, NULL, 0)) == NULL) means it is unable to attach to the shared memory segment
        else
        {
          perror("reading SHM : shmat");
        }
      }

      // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)

    } // (mutex == SEM_FAILED) means it was unable to create/open the semaphore
    else
    {
      perror("reading SHM : unable to create/open semaphore");

      sem_unlink(SemaphoreName.c_str());
    }
  }

  return Status;
}

// Write to the shared memory
int GW::WriteToSharedMemory3(std::string OQS, Message* M)
{
  int Status = ERROR;
  unsigned int Category = 17;

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
       z++) // Modified in 9th April 2021 to deal with parallel shared memories.
  {
    long long MessageSize = 0; // The size of the Message (in bytes) being transferred
    long long TotalSize = 0;   // The total size of the message being transferred
    unsigned int NoCL = 0;
    unsigned char* data; // Manipulate the content on shared memory
    int shmid = -1;      // Shared memory ID on OS for a certain key_t (init to -1 for safety)
    void* shm_address;   // The shared memory segment address
    string Offset = "          ";
    sem_t* mutex;
    vector<string>* Values = new vector<string>;

    int _shm_key = StringToInt(OQS) + z;

    string _oqs = IntToString(_shm_key);

    if (GetHTBindingValues(Category, _oqs, Values) == OK)
    {
      // Assert the Values vector
      if (Values != 0)
      {
        if (!Values->empty())
        {
          // Carry the destination pointer
          shmid = StringToInt(Values->at(0));

#ifdef DEBUG2
          S << Offset << "(Shared memory ID (shmid) already on HT = " << shmid << ")" << endl;
#endif
        }
      }
    }
    else
    {
      if (ReturnIPCSHMID((key_t)_shm_key, shmid) == OK)
      {
        Values->push_back(IntToString(shmid));

        StoreHTBindingValues(Category, _oqs, Values);

        LG("(Storing the shared memory ID (shmid) = " << shmid << " on HT. The related key is "
                                                      << _oqs << ")");
      }
    }

    if (shmid != -1)
    {
      // Cache semaphore to avoid sem_open/sem_close per iteration (Phase 2)
      mutex = CachedSemaphores[_oqs];
      if (mutex == NULL)
      {
        mutex = sem_open(_oqs.c_str(), O_CREAT, 0666, 1);
        if (mutex != SEM_FAILED)
        {
          CachedSemaphores[_oqs] = mutex;
        }
      }

      // Check for error on semaphore open
      if (mutex != NULL && mutex != SEM_FAILED)
      {

        // ***************************************************************************************
        // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
        // ***************************************************************************************

#ifdef DEBUG3
        S << Offset << "(Opened the semaphore " << _oqs << ")" << endl;
#endif

        if ((shm_address = shmat(shmid, NULL, 0)) != NULL)
        {

          // Retry sem_trywait with backoff to avoid CPU spin
          int LockAttempts = 0;
          while (sem_trywait(mutex) != 0 && LockAttempts < 100)
          {
            tthread::this_thread::sleep_for(tthread::chrono::microseconds(100));
            LockAttempts++;
          }
          if (LockAttempts < 100)
          {

#ifdef DEBUG3
            S << Offset << "(Locked the semaphore " << _oqs << ")" << endl;
#endif

            data = (unsigned char*)shm_address;

            if (data != (unsigned char*)(-1))
            {
              if (data[0] != (unsigned char)'f' && data[0] != (unsigned char)'w')
              {
                data[0] = (unsigned char)'f';

                LG("(Initialized the shared memory with key " << _oqs
                                                              << " regarding the r/w control flag)");
              }
            }

#ifdef DEBUG3
            // S << "          (The value of the first byte in shared memory is "<<(char)data[0]<<" (f = Free to write, w = still waiting for peer reading)" << endl;
            S << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

            if (data[0] == (unsigned char)'f')
            {

#ifdef DEBUG2
              S << "[5]       (Writing a message to the shared memory segment with key = " << _oqs
                << " identified by " << shmid << ")" << endl;
#endif

              // Get the Message size
              if (M->GetMessageSize(MessageSize) == OK)
              {
                unsigned char* Payload;

                // Get the message in a char array format
                if (M->GetMessageFromCharArray(Payload) == OK)
                {
                  M->GetNumberofCommandLines(NoCL);

                  TotalSize = MessageSize + 8;

                  unsigned char* HeaderSizeField = new unsigned char[TotalSize];
                  unsigned char* PDU = new unsigned char[TotalSize];

                  HeaderSizeField[0] = (unsigned char)(TotalSize >> (8 * 7)) & 0xff;
                  HeaderSizeField[1] = (unsigned char)(TotalSize >> (8 * 6)) & 0xff;
                  HeaderSizeField[2] = (unsigned char)(TotalSize >> (8 * 5)) & 0xff;
                  HeaderSizeField[3] = (unsigned char)(TotalSize >> (8 * 4)) & 0xff;
                  HeaderSizeField[4] = (unsigned char)(TotalSize >> (8 * 3)) & 0xff;
                  HeaderSizeField[5] = (unsigned char)(TotalSize >> (8 * 2)) & 0xff;
                  HeaderSizeField[6] = (unsigned char)(TotalSize >> (8 * 1)) & 0xff;
                  HeaderSizeField[7] = (unsigned char)(TotalSize >> (8 * 0)) & 0xff;

                  for (long long j = 0; j < TotalSize; j++)
                  {
                    if (j < 8)
                    {
                      PDU[j] = HeaderSizeField[j];
                      // printf("%i %d %c \n",j,PDU[j],PDU[j]);
                    }

                    if (j >= 8 && j < (MessageSize + 8))
                    {
                      PDU[j] = (unsigned char)Payload[(j - 8)];
                      // printf("%i %d %c \n",j,PDU[j],PDU[j]);
                    }
                  }

                  if (NoCL > 2 && TotalSize <= (long long)MaxSegmentSize)
                  {

#ifdef DEBUG2
                    S << Offset << "(Sending the following message)" << endl;

                    S << "(" << endl
                      << *M << ")" << endl;

                    S << Offset << "(The size of the payload is = " << MessageSize << ")" << endl;

                    S << Offset << "(The size of the total message is = " << TotalSize << ")" << endl;

                    S << Offset << "(The size of the memory segment is = " << MaxSegmentSize << ")"
                      << endl;

                    // S << Offset << "(Make the shared memory unavailable while not read by the peer)"<< endl;
#endif

                    // Clean the memory segment before writing
                    // March 6th, 2021: changed from MaxSegmentSize to 1, since there is not need to clean more than this
                    memset(data, (unsigned char)'w', 1);

                    for (long long k = 1; k < TotalSize + 1; k++)
                    {
                      data[k] = PDU[k - 1];
                    }

                    // Send the timestap of this operation after the message

                    unsigned long long Time = (unsigned long long)(GetTime() * 10e9);

                    unsigned char* HeaderTimeStampField = new unsigned char[8];

                    HeaderTimeStampField[0] = (unsigned char)(Time >> (8 * 7)) & 0xff;
                    HeaderTimeStampField[1] = (unsigned char)(Time >> (8 * 6)) & 0xff;
                    HeaderTimeStampField[2] = (unsigned char)(Time >> (8 * 5)) & 0xff;
                    HeaderTimeStampField[3] = (unsigned char)(Time >> (8 * 4)) & 0xff;
                    HeaderTimeStampField[4] = (unsigned char)(Time >> (8 * 3)) & 0xff;
                    HeaderTimeStampField[5] = (unsigned char)(Time >> (8 * 2)) & 0xff;
                    HeaderTimeStampField[6] = (unsigned char)(Time >> (8 * 1)) & 0xff;
                    HeaderTimeStampField[7] = (unsigned char)(Time >> (8 * 0)) & 0xff;

                    for (unsigned int n = 0; n < 8; n++)
                    {
                      data[TotalSize + 1 + n] = HeaderTimeStampField[n];
                    }

                    delete[] HeaderTimeStampField;

                    Status = OK;
                  }
                  else
                  {
                    S << Offset
                      << "(ERROR: The message is too big for the memory segment or it has less than 3 command lines)"
                      << endl;

                    // Mark to delete the message. Added in March 7th, 2021
                    M->MarkToDelete();
                  }

                  delete[] PDU;
                  delete[] HeaderSizeField;
                }
                else
                {
                  S << Offset << "(ERROR: Unable to get message from char array)" << endl;
                }
              }

            } // (data[0] != (unsigned char)'f') means the memory is not free to write, despite of locking the semaphore.
            else
            {
#ifdef DEBUG1
              S << Offset << "(Warning: The peer behind shmid " << shmid << " and key " << _oqs
                << " still does not read the shared memory. Trying again later)" << endl;
#endif

              Status = ERROR;

              // Sleep before trying to send in the shared memory again
              tthread::this_thread::sleep_for(tthread::chrono::microseconds(5));
            }

            if (sem_post(mutex) == 0) // Successfully locked the semaphore, so unlock it
            {
#ifdef DEBUG3
              S << Offset << "(Unlocked the semaphore " << _oqs << ")" << endl;
#endif
            }
            else
            {
              perror("writing SHM : sem_post");
            }

          } // (LockAttempts < 100) successfully locked
          else
          {
            // Failed to lock after 100 attempts (10ms)
#ifdef DEBUG1
            perror("writing SHM : unable to lock the semaphore");
#endif
          }

          // Sleep while wait for shared memory semaphore
          tthread::this_thread::sleep_for(tthread::chrono::microseconds(5));

          // Detach from shared memory, if attached
          if (shmdt(shm_address) == -1)
          {
            perror("writing SHM : shmdt");
          }

        } // ((shm_address = shmat(shmid, NULL, 0)) == NULL) means UNABLE TO ATTACH
        else
        {
          perror("writing SHM : semop");
        }

        // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)

      } //(mutex != SEM_FAILED)
      else
      {
        perror("writing SHM : unable to open/create the semaphore");

        sem_unlink(_oqs.c_str());
      }

    } //(shmid <= 0)
    else
    {
      S << Offset << "(ERROR: Unable to determine shared memory ID in operating system)" << endl;
    }

    delete Values;

    if (Status == OK)
    {
      break;
    }
  }

  // S << Offset << "(Finished)"<< endl;

  return Status;
}

// First attachment to learned shared memory
int GW::ReturnIPCSHMID(key_t _Key, int& _shmid)
{
  // Initialize to error value to prevent undefined behavior (AMD vs Intel divergence)
  _shmid = -1;

  // GW		*PGW=0;
  string Offset = "          ";
  string SemaphoreName;
  sem_t* mutex;
  int Status = ERROR;

  // Set the semaphore name
  SemaphoreName = IntToString(_Key);

  mutex = sem_open(SemaphoreName.c_str(), O_CREAT, 0666, 1);

  // Check for error on semaphore open
  if (mutex != SEM_FAILED)
  {
    // Connect to the segment
    if ((_shmid = shmget(_Key, MaxSegmentSize, IPC_CREAT | 0666)) != -1)
    {
      Status = OK;
    }
    else
    {
      perror("shmat");

      S << Offset << "(ERROR: Unable to get the shared memory segment identified by the key = " << _Key << ")"
        << endl;
    }
  }
  else
  {
    perror("While initiating the SHM: unable to create semaphore");

    sem_unlink(SemaphoreName.c_str());

    S << Offset << "(ERROR: Unable to create the semaphore while discovering the peer shmid)" << endl;
  }

  return Status;
}

// Auxiliary functions
void GW::SetStopGatewayFlag(bool _F)
{
  StopGateway = _F;
  // Wake up both threads blocked on wait()
  InputQueueCV.notify_all();
  OutputQueueCV.notify_all();
}

// Set a value behind a key
int GW::StoreHTBindingValues(unsigned int _Category, string _Key, vector<string>* _Values)
{
  int Status = ERROR;

  if (PHT->StoreBinding(_Category, _Key, _Values) == OK)
  {
    Status = OK;
  }

  return Status;
}

// Discover the values behind a key
int GW::GetHTBindingValues(unsigned int _Category, string _Key, vector<string>*& _Values)
{
  int Status = ERROR;

  if (PHT->GetBinding(_Category, _Key, _Values) == OK)
  {
    Status = OK;
  }

  return Status;
}

// Wrapper function for ReadFromOutputQueue() thread
void GW::ReadFromOutputQueueThreadWrapper(void* _PGW)
{
  GW* P = static_cast<GW*>(_PGW);

  P->ReadFromOutputQueue();
}
```

## FILE: baseline@1af604d:Common/src/GW.h

Role: Exact source bytes from the pinned functional baseline; read-only design context.
Bytes: 8462
SHA-256: 54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44

```text
/*
        NovaGenesis

        Name:		Gateway
        Object:		GW
        File:		GW.h
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.1

        Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _GW_H
#define _GW_H

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _GLIBCXX_IOMANIP
#include <iomanip>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _QUEUE_H
#include <queue>
#endif

#ifndef _SYS_MSG_H
#include <sys/msg.h>
#endif

#ifndef _SYS_STAT_H
#include <sys/stat.h>
#endif

#ifndef _SYS_SHM_H
#include <sys/shm.h>
#endif

#ifndef _SYS_IPC_H
#include <sys/ipc.h>
#endif

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _GLIBCXX_FUNCTIONAL
#include <functional>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_SEM_H
#include <sys/sem.h>
#endif

#ifndef _SEMAPHORE_H
#include <semaphore.h>
#endif

#ifndef __FCNTL_H
#include <fcntl.h>
#endif

#ifndef __MAP_H
#include <map>
#endif

#ifndef _CONDITION_VARIABLE
#include <condition_variable>
#endif

#ifndef _MUTEX
#include <mutex>
#endif

#ifndef _ATOMIC_H
#include <atomic>
#endif

#ifndef _UTILITY_H
#include <utility>
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#define ERROR 1
#define OK 0

using namespace std;
using namespace tthread;

struct DereferenceCompareNode : public binary_function<const Message*, const Message*, bool>
{
  bool operator()(const Message* M1, const Message* M2) const
  {
    if (M1->GetTime() != M2->GetTime())
    {
      return M1->GetTime() > M2->GetTime();
    }
    else
    {
      return M1->GetTag() > M2->GetTag();
    }
  }
};

class HT;

class GW : public Block
{
private:
  // ------------------------------------------------------------------------------------------------------------------------------
  // Priority queues
  // ------------------------------------------------------------------------------------------------------------------------------

  // Input message queue
  priority_queue<Message*, vector<Message*>, DereferenceCompareNode> InputQueue;

  // Per key output message queue
  map<std::string, priority_queue<Message*, vector<Message*>, DereferenceCompareNode>> OutputQueues;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary variables
  // ------------------------------------------------------------------------------------------------------------------------------

  // Auxiliary tag for messages stored on input queue
  unsigned int InputQueueTag;

  // Auxiliary tag for messages stored on output queue
  unsigned int OutputQueueTag;

  // Stop gateway flag
  std::atomic<bool> StopGateway;

  // Mutex and condition variable for input queue synchronization
  std::mutex InputQueueMutex;
  std::condition_variable InputQueueCV;

  // ── NGAL: Network Receive Queue ──
  // Thread-safe intermediate queue for raw char buffers from the NGAL
  // ReceiveDispatcher. The GW thread drains this queue in Gateway().
  // These are public so that NGAL_CS::DeliverToGateway() can access them.
  // Marked as public: but contained within the private: section above.
public:
  std::queue<std::pair<char*, long long>> NetworkReceiveQueue;
  std::mutex NetworkReceiveQueueMutex;
  std::condition_variable NetworkReceiveQueueCV;
  // ⚠ Access returns to private for everything below ⚠
private:

  // Mutex and condition variable for output queue synchronization
  std::mutex OutputQueueMutex;
  std::condition_variable OutputQueueCV;

  // Flag to notify output thread of new messages
  bool NewOutputMessage;

  // Cached semaphores for shared memory IPC
  std::map<std::string, sem_t*> CachedSemaphores;

  // Pointer to the HT block
  HT* PHT;

  // ------------------------------------------------------------------------------------------------------------------------------
  // OS Shared Memory IPC functions
  // ------------------------------------------------------------------------------------------------------------------------------

  int ReadFromSharedMemory3();

  // Write to the shared memory
  int WriteToSharedMemory3(std::string OQS, Message* M);

public:
  // First attachment to learned shared memory (public for PG access)
  int ReturnIPCSHMID(key_t _Key, int& _shmid);

  // Define the maximum segment size on shared memory
  size_t MaxSegmentSize;

  // Schedule status flag
  bool ScheduleStatusFlag;

  // Constructor
  GW(string _LN, Process* _PP, unsigned int _Index, string _Path);

  // Destructor
  ~GW();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Action related functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Allocate and add an Action on Actions container
  void NewAction(const string _LN, Action*& _PA);

  // Get an Action
  int GetAction(string _LN, Action*& _PA);

  // Delete an Action
  int DeleteAction(string _LN);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Core public functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Push a Message into the input message priority queue
  void PushToInputQueue(Message* M);

  // Monitor IPC, read Messages from other processes and pushes to the InputQueue
  // Read Messages from the InputQueue and call the GW::Run(Message *M)
  // Read Messages from the OutputQueue and forward to other processes via IPC
  void Gateway();

  // Push a Message into the output message priority queue. Only the GW can push messages at the output queue
  void PushToOutputQueue(std::string OQS, Message* M);

  // Read a Message from output message priority queues. Only the GW can forward messages to shared memory instances
  void ReadFromOutputQueue();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary functions
  // ------------------------------------------------------------------------------------------------------------------------------

  void SetStopGatewayFlag(bool _F);

  // Get the values behind a key
  int GetHTBindingValues(unsigned int _Category, string _Key, vector<string>*& _Values);

  // Set a value behind a key
  int StoreHTBindingValues(unsigned int _Category, string _Key, vector<string>* _Values);

  // Wrapper function for ReadFromOutputQueue() thread
  static void ReadFromOutputQueueThreadWrapper(void* _PGW);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Delay parameters
  // Delay parameters
  double DelayBeforeStatusIPC;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Friend classes
  // ------------------------------------------------------------------------------------------------------------------------------
  friend class GWMsgCl01;
  friend class GWRunInitialization01;
  friend class GWHelloIPC02;
  friend class GWRunHelloIPC02;
  friend class GWExposition02;
  friend class Message;
  friend class PG; // SPEC-028-B: heartbeat reads InputQueue depth under InputQueueMutex.
};

#endif
```

## FILE: baseline@1af604d:Common/src/GWRunInitialization01.cpp

Role: Exact source bytes from the pinned functional baseline; read-only design context.
Bytes: 9601
SHA-256: 1656b703c892831a05043a8bf82ae1bed73d701c8ffea7b95af5a02df0c978af

```text
/*
        NovaGenesis

        Name:		GWRunInitialization01
        Object:		GWRunInitialization01
        File:		GWRunInitialization01.cpp
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.1

        Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _GWRUNINITIALIZATION01_H
#include "GWRunInitialization01.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

GWRunInitialization01::GWRunInitialization01(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

GWRunInitialization01::~GWRunInitialization01()
{
}

// Run the actions behind a received command line
// ng -run --initialization 0.1
int GWRunInitialization01::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = OK;
  Message* StoringInitialBinds = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Block* PHTB = 0;
  CommandLine* PCL = 0;
  GW* PGW = 0;
  int Category;
  string Key;
  vector<string> Values;
  string OperatingSystemLegibleNameSCN;
  string Offset = "                    ";
  Message* RunExposition = 0;

  PGW = (GW*)PB;

  PHTB = (Block*)PGW->PHT;

  // Setting up the process SCN as the space limiter
  Limiters.push_back(PB->PP->Intra_Process);

  // Setting up the CLI block SCN as the source SCN
  Sources.push_back(PB->GetSelfCertifyingName());

  // Setting up the HT block SCN as the destination SCN
  Destinations.push_back(PHTB->GetSelfCertifyingName());

  // Creating a new message
  PB->PP->NewMessage(GetTime(), 0, false, StoringInitialBinds);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, StoringInitialBinds, PCL);

  // Block related

  // ******************************************************
  // Binding: PeerPID -> LegibleName (Category 20)
  // ******************************************************

  Category = 20;
  Key = PB->PP->GetSelfCertifyingName();
  Values.push_back(PB->PP->GetLegibleName());
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  PMB->NewStoreBindingCommandLineFromBLNToHashBLN("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBLN("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBID("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToHashBLN("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToBlocksIndex("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBlocksIndexToBID("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPIDToBID("0.1", PB, StoringInitialBinds, PCL);

  // Limiters related

  PMB->NewStoreBindingCommandLineFromLimiterToHashLimiter("0.1", "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToLimiter("0.1", "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN("0.1", "OS", PB->PP->OSSCN, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromRepresentativeSCNToHashLimiter("0.1", PB->PP->OSSCN, "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromLimiterToHashLimiter("0.1", "Domain", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToLimiter("0.1", "Domain", StoringInitialBinds, PCL);

  // **********************************************************************************
  // The next two lines store a temporary domain name, while Domain Service is booting
  // **********************************************************************************

  PMB->NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN("0.1", "Domain", PB->PP->DSCN, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromRepresentativeSCNToHashLimiter("0.1", PB->PP->DSCN, "Domain", StoringInitialBinds, PCL);

  // ******************************************************
  // Binding PID to Input IPC Key
  // ******************************************************

  // Setting up the category
  Category = 13;

  // Setting up the key
  Key = PB->PP->GetSelfCertifyingName();

  Values.push_back(PB->IntToString(PB->PP->Key));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine("-sr", "--b", "0.1", Category, Key, &Values, StoringInitialBinds, PCL);

  Values.clear();

  // ***************************************************************************************
  // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
  // ***************************************************************************************

  if (PB->PP->GetLegibleName() != "PGCS")
  {
    key_t IPCKey = 11;
    int shmid = 0;

    if (PGW->ReturnIPCSHMID(IPCKey, shmid) == OK)
    {
      // Setting up the category
      Category = 17;

      // Setting up the key
      Key = PB->IntToString(IPCKey);

      Values.push_back(PB->IntToString(shmid));

      // Creating the ng -sr --b 0.1 command line
      PMB->NewCommonCommandLine("-sr", "--b", "0.1", Category, Key, &Values, StoringInitialBinds, PCL);
    }
  }

  // Push the message to the GW input queue
  PGW->PushToInputQueue(StoringInitialBinds);

  // ******************************************************
  // Finish
  // ******************************************************

  // Generate the SCN
  SCN = NameGenerator::GetInstance().GenerateFromMessage(StoringInitialBinds);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine("0.1", SCN, StoringInitialBinds, PCL);

  // ******************************************************
  // Schedule a message to run exposition first time
  // ******************************************************
  if (PB->PP->GetLegibleName() == "PGCS")
  {
    Values.clear();
    Limiters.clear();
    Sources.clear();
    Destinations.clear();

    // Setting up the process SCN as the space limiter
    Limiters.push_back(PB->PP->Intra_Process);

    // Setting up the block SCN as the source SCN
    Sources.push_back(PB->GetSelfCertifyingName());

    // Setting up the block SCN as the destination SCN
    Destinations.push_back(PB->GetSelfCertifyingName());

    // Creating a new message (schedule for next period)
    PB->PP->NewMessage(GetTime() + 1.0, 1, false, RunExposition);

    // Creating the ng -cl -m command line
    PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, RunExposition, PCL);

    // Adding a ng -run --exposition 0.2 command line
    RunExposition->NewCommandLine("-run", "--exposition", "0.2", PCL);

    // Generate the SCN
    SCN = NameGenerator::GetInstance().GenerateFromMessage(RunExposition);

    // Creating the ng -scn --s command line
    PMB->NewSCNCommandLine("0.1", SCN, RunExposition, PCL);

    // Push the message to the GW input queue
    PGW->PushToInputQueue(RunExposition);
  }

  // ******************************************************
  // Schedule the first hello IPC emission
  // ******************************************************

  // ******************************************************
  // Schedule the first hello IPC emission
  // ******************************************************

  Message* RunHelloIPC = NULL;
  Limiters.clear();
  Sources.clear();
  Destinations.clear();

  // Setting up the process SCN as the space limiter
  Limiters.push_back(PB->PP->Intra_Process);

  // Setting up the block SCN as the source SCN
  Sources.push_back(PB->GetSelfCertifyingName());

  // Setting up the block SCN as the destination SCN
  Destinations.push_back(PB->GetSelfCertifyingName());

  // Creating a new message (schedule for next period)
  PB->PP->NewMessage(GetTime() + 1.0, 1, false, RunHelloIPC);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, RunHelloIPC, PCL);

  // Adding a ng -run --helloIPC 0.2 command line
  RunHelloIPC->NewCommandLine("-run", "--helloIPC", "0.2", PCL);

  // Generate the SCN
  SCN = NameGenerator::GetInstance().GenerateFromMessage(RunHelloIPC);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine("0.1", SCN, RunHelloIPC, PCL);

  // Push the message to the GW input queue
  PGW->PushToInputQueue(RunHelloIPC);

  // PB->S << Offset <<  "(Done)" << endl << endl << endl;

  // ******************************************************
  // Move to operational state
  // ******************************************************

  PB->State = "operational";

  PB->S << Offset << "(State: Operational)" << endl
        << endl
        << endl;

  // ******************************************************
  // Finish
  // ******************************************************

  return Status;
}
```

## FILE: baseline@1af604d:Common/src/Process.cpp

Role: Exact source bytes from the pinned functional baseline; read-only design context.
Bytes: 41839
SHA-256: bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1

```text
/*
        NovaGenesis

        Name:		Process
        Object:		Process
        File:		Process.cpp
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.1

        Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CLI_H
#include "CLI.h"
#endif

#ifndef _MURMURHASH3_H_
#include "MurmurHash3.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

// #define DEBUG
// #define DEBUG1
// #define DEBUG3

Process::Process(string _LN, key_t _Key, string _Path)
{
  LN = _LN;
  SCN = "";
  Path = "";
  Key = _Key;
  Path = _Path;
  MessageCounter = 0;

  // Modified in 9th April 2021 to deal with parallel shared memories.

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES; z++)
  {
    shmid[z] = -1;
  }

  NoM = 0;

  InstantiationTime = GetTime();

  PMB = new MessageBuilder(this);

  // Generate the required prime numbers
  unsigned int n_primes = 0;

  for (unsigned int z = 2; n_primes < 16; z++)
  {
    for (unsigned int w = 0; w < n_primes; w++)
    {
      if (z % primes[w] == 0)
      {
        goto not_prime;
      }
    }

    primes[n_primes] = z;

    n_primes++;

  not_prime:;
  }

  string HASH_SCN = "";

  // Generate an SCN for the process
  SCN = NameGenerator::GetInstance().GenerateFromString(LN + Path);

  // Generate the hash of the LN for the process
  HASH_SCN = NameGenerator::GetInstance().GenerateFromString(LN);

  cout << "(The process legible name is " << LN << ")" << endl;

  cout << "(The hash of process legible name is " << HASH_SCN << ")" << endl;

  cout << "(The process self-certifying name is " << SCN << ")" << endl;

  cout << "(The process shared memory key is = " << Key << ")" << endl;

  // Auxiliary pointers
  Block* PB2 = 0;
  Block* PB3 = 0;

  // *****************************************
  // Generate	HID and OSID
  // *****************************************
  struct utsname uts;

  if (uname(&uts) != -1)
  {
    stringstream osname;
    stringstream hostname;

    osname << uts.sysname << " ";
    osname << uts.release << " ";
    osname << uts.version << " ";
    osname << uts.nodename;

    hostname << uts.nodename << " ";
    hostname << uts.machine;

    OSLN = osname.str();

    HLN = hostname.str();

    cout << "(The operating system legible name is " << OSLN << ")" << endl;

    cout << "(The host legible name is " << HLN << ")" << endl;

    OSSCN = NameGenerator::GetInstance().GenerateFromString(OSLN);

    // OSSCN=OSSCN+"_OSID";

    cout << "(The operating system self-certifying name is = " << OSSCN << ")" << endl;

    HSCN = NameGenerator::GetInstance().GenerateFromString(HLN);

    // HSCN=HSCN+"_HID";

    cout << "(The host self-certifying name is = " << HSCN << ")" << endl;
  }

  // **********************************************************************************
  // Generate	DID - Deprecated in March 2016 - Replaced by Domain Service.
  // **********************************************************************************

  // DLN="NovaGenesis Domain @ INATEL, Brazil";

  // GenerateSCNFromCharArrayBinaryPatterns(DLN,DSCN);

  // DSCN=DSCN+"_DID";

  // cout << "(The domain self-certifying name is = " << DSCN << ")" << endl<< endl << endl;

  // **********************************************************************************
  // Default domain name - March 2016
  // Temporary name until Domain Service publishes the final name
  // Requires update o domain name on hash tables after DS publishing
  // the official domain name
  // **********************************************************************************

  DLN = "Unnamed";

  DSCN = NameGenerator::GetInstance().GenerateFromString(DLN);

  // ************************************************************
  // New limiters scheme. March 2016
  // ************************************************************

  // Generate a SCN from the "Process" word
  Intra_Process = NameGenerator::GetInstance().GenerateFromString("Intra_Process");

  cout << "(Intra_Process = " << Intra_Process << ")" << endl;

  // Generate a SCN from the "Operating System" word
  Intra_OS = NameGenerator::GetInstance().GenerateFromString("Intra_OS");

  cout << "(Intra_OS = " << Intra_OS << ")" << endl;

  // Generate a SCN from the "Domain" word
  Intra_Node = NameGenerator::GetInstance().GenerateFromString("Intra_Node");

  cout << "(Intra_Node = " << Intra_Node << ")" << endl;

  // Generate a SCN from the "Domain" word
  Intra_Domain = NameGenerator::GetInstance().GenerateFromString("Intra_Domain");

  cout << "(Intra_Domain = " << Intra_Domain << ")" << endl;

  // Generate a SCN from the "Domain" word
  Inter_Domain = NameGenerator::GetInstance().GenerateFromString("Inter_Domain");

  cout << "(Inter_Domain = " << Inter_Domain << ")" << endl;

  // **********************************************************************************
  // Initializes the messages container
  // **********************************************************************************

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    Messages[i] = NULL;

    Controls[i] = true; // Can I write to some positions
  }

  // Create the process blocks
  NewBlock("HT", PB3);
  NewBlock("GW", PB2);
  // NewBlock("CLI",PB1);
}

Process::~Process()
{
  cout << "Deleting " << LN << endl;

  cout << "Deleting Messages container" << endl;

  DeleteMessages();

  cout << "Done" << endl;

  vector<Block*>::iterator it;
  unsigned int i = 0;

  for (it = Blocks.begin(); it != Blocks.end(); it++)
  {
    delete Blocks[i];

    i++;
  }

  Blocks.clear();

  delete PMB;
}

// Set block legible name
void Process::SetLegibleName(string _LN)
{
  LN = _LN;
}

// Set block self-certyfing name
void Process::SetSelfCertifyingName(string _SCN)
{
  SCN = _SCN;
}

// Set working path
void Process::SetPath(string _Path)
{
  Path = _Path;

  Block* PB = 0;

  vector<Block*>::iterator it;

  for (it = Blocks.begin(); it != Blocks.end(); it++)
  {
    PB = *it;

    PB->SetPath(_Path);
  }
}

// Get block legible name
string Process::GetLegibleName()
{
  return LN;
}

// Get process self-certifying name
string Process::GetSelfCertifyingName()
{
  return SCN;
}

// Get working path
string Process::GetPath()
{
  return Path;
}

// Get Blocks container size
unsigned int Process::GetBlocksSize()
{
  return Blocks.size();
}

// Get Host Legible Name
string Process::GetHostLegibleName()
{
  return HLN;
}

// Get Host Self-certifying Name
string Process::GetHostSelfCertifyingName()
{
  return HSCN;
}

// Get Operating System Legible Name
string Process::GetOperatingSystemLegibleName()
{
  return OSLN;
}

// Get Operating System Self-certifying Name
string Process::GetOperatingSystemSelfCertifyingName()
{
  return OSSCN;
}

// Get Domain Self-certifying Name
string Process::GetDomainSelfCertifyingName()
{
  return DSCN;
}

// Insert an allocated Block on Blocks container
void Process::Process::InsertBlock(Block* _PB)
{
  Blocks.push_back(_PB);
}

// Allocate a new block based on a name and add a Block on Blocks container
int Process::NewBlock(string _LN, Block*& _PB)
{
  if (_LN == "CLI")
  {
    Block* PGWB = 0;
    Block* PHTB = 0;
    GW* PGW = 0;
    HT* PHT = 0;
    string GWLN = "GW";
    string HTLN = "HT";

    GetBlock(GWLN, PGWB);

    PGW = (GW*)PGWB;

    GetBlock(HTLN, PHTB);

    PHT = (HT*)PHTB;

    unsigned int Index = 0;

    Index = GetBlocksSize();

    CLI* PCLI = new CLI(_LN, this, Index, PGW, PHT, Path);

    _PB = (Block*)PCLI;

    InsertBlock(_PB);

    return OK;
  }

  if (_LN == "GW")
  {
    GW* PGW = new GW(_LN, this, 1, Path);

    _PB = (Block*)PGW;

    Blocks.push_back(_PB);

    return OK;
  }

  if (_LN == "HT")
  {
    HT* PDM = new HT(_LN, this, 0, Path);

    _PB = (Block*)PDM;

    Blocks.push_back(_PB);

    return OK;
  }

  return ERROR;
}

// Get a Block by its legible name
int Process::GetBlock(string _LN, Block*& _PB)
{
  Block* PB = 0;
  vector<Block*>::iterator it;
  int Status = ERROR;

  for (it = Blocks.begin(); it != Blocks.end(); it++)
  {
    PB = *it;

    if (PB->GetLegibleName() == _LN)
    {
      _PB = PB;

      Status = OK;

      break;
    }
  }

  return Status;
}

// Get a Block by its index on Blocks container
int Process::GetBlock(unsigned int _Index, Block*& _PB)
{
  Block* PB = 0;
  vector<Block*>::iterator it;
  int Status = ERROR;
  unsigned int i = 0;

  for (it = Blocks.begin(); it != Blocks.end(); it++)
  {
    PB = *it;

    if (i == _Index)
    {
      _PB = PB;

      Status = OK;

      break;
    }

    i++;
  }

  return Status;
}

// Get a Block index by its legible name
int Process::GetBlockIndex(string _LN, unsigned int& _Index)
{
  int Status = ERROR;

  for (unsigned int i = 0; i < Blocks.size(); i++)
  {
    if (Blocks[i]->GetLegibleName() == _LN)
    {
      _Index = i;

      Status = OK;

      break;
    }
  }

  return Status;
}

// Delete a Block by its legible name
int Process::DeleteBlock(string _LN)
{
  return 0;
}

// Delete a Block by its index on Blocks container
int Process::DeleteBlock(unsigned int _Index)
{
  Block* PB = 0;
  vector<Block*>::iterator it;
  unsigned int i = 0;
  int Status = ERROR;

  for (it = Blocks.begin(); it != Blocks.end(); it++)
  {
    if (_Index == i)
    {
      PB = Blocks[i];

      Blocks.erase(it);

      delete PB;

      Status = OK;

      break;
    }

    i++;
  }

  return Status;
}

// Delete a Block by its pointer
int Process::DeleteBlock(Block* B)
{
  Block* PB = 0;
  vector<Block*>::iterator it;
  int i = 0;
  int Status = ERROR;

  for (it = Blocks.begin(); it != Blocks.end(); it++)
  {
    PB = Blocks[i];

    if (B == PB)
    {
      Blocks.erase(it);

      delete PB;

      Status = OK;

      break;
    }

    i++;
  }

  return Status;
}

// Allocate and add a Message on Messages container
int Process::NewMessage(double _Time, short _Type, bool _HasPayload, Message*& M)
{
  int Status = ERROR;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == FREE)
    {
      Message* PM = new Message(_Time, _Type, _HasPayload);

      M = PM;

      PM->InstantiationNumber = MessageCounter;

      MessageCounter++;

      NoM++;

      Messages[i] = PM;

      Controls[i] = BUSY;

      Status = OK;

#ifdef DEBUG
      cout << "          (Creating a message at " << GetLegibleName() << " with instantiation number " << PM->InstantiationNumber << ". Number of messages is " << NoM << ")" << endl;

      cout << "(Allocated the message with index = " << i << ". Number of messages is " << NoM << ".)" << endl;
#endif

      break;
    }
  }

  return Status;
}

// Allocate and add a simple Message on Messages container
int Process::NewMessage(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path, Message*& M)
{
  int Status = ERROR;

  M = NULL;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == FREE)
    {
      Message* PM = new Message(_Time, _Type, _HasPayload, _HeaderFileName, _Path);

      M = PM;

      PM->InstantiationNumber = MessageCounter;

      MessageCounter++;

      NoM++;

      Messages[i] = PM;

      Controls[i] = BUSY;

#ifdef DEBUG

      cout << "(Allocated the message with index = " << i << ". Number of messages is " << NoM << ".)" << endl;

#endif

      Status = OK;

      break;
    }
  }

  return Status;
}

// Allocate and add a complete Message on Messages container
int Process::NewMessage(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path, Message*& M)
{
  int Status = ERROR;

  M = NULL;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == FREE)
    {
      Message* PM = new Message(_Time, _Type, _HasPayload, _HeaderFileName, _PayloadFileName, _MessageFileName, _Path);

      M = PM;

      PM->InstantiationNumber = MessageCounter;

      MessageCounter++;

      NoM++;

      Messages[i] = PM;

      Controls[i] = BUSY;

#ifdef DEBUG

      cout << "(Allocated the message with index = " << i << ". Number of messages is " << NoM << ".)" << endl;

#endif

      Status = OK;

      break;
    }
  }

  return Status;
}

int Process::NewMessage(Message* _Original, Message*& M)
{
  int Status = ERROR; // Pessimistic approach ;-)

  M = NULL;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == FREE)
    {
      Message* PM = new Message(*_Original);

      M = PM;

      PM->InstantiationNumber = MessageCounter;

      MessageCounter++;

      NoM++;

      Messages[i] = PM;

      Controls[i] = BUSY;

#ifdef DEBUG

      cout << "(Allocated the message with index = " << i << ". Number of messages is " << NoM << ".)" << endl;

#endif

      Status = OK;

      break;
    }
  }

  // S <<"          (Creating a message at block "<<GetLegibleName()<<" with instantiation number " <<PM->InstantiationNumber<< ". Number of messages is "<<NoM<<")" << endl;

  return Status;
}

// Get a Message
int Process::GetMessage(unsigned int _Index, Message*& M)
{

  if (_Index < NoM)
  {
    M = Messages[_Index];
  }

  return OK;
}

// Erase a Message from the container
int Process::EraseMessage(Message* M)
{
  int Status = OK;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Messages[i] == M)
    {
      Messages[i] = NULL;

      Controls[i] = FREE;

      NoM--;

#ifdef DEBUG

      cout << "(Erased the message index = " << i << ". The number of messages is " << NoM << ">)" << endl;

#endif

      break;
    }
  }

  return Status;
}

// Tests if a message is ok
int Process::HasMessage(Message* M, bool& _Answer)
{
  int Status = OK;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Messages[i] == M)
    {
      _Answer = true;

      break;
    }
  }

  return Status;
}

// Delete a Message by its pointer
int Process::DeleteMessage(Message* M)
{
  int Status = OK;

  bool Deleted = false;

  int Index = 0;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Messages[i] == M)
    {
      if (M->GetDeleteFlag() == true)
      {
        delete M;

        Messages[i] = NULL;

        Controls[i] = FREE;

        NoM--;

        Deleted = true;

        Index = i;

        break;
      }
    }
  }

#ifdef DEBUG

  if (Deleted == true)
  {
    cout << "(Deleted the message with index = " << Index << ". Number of messages is " << NoM << ".)" << endl;
  }

#endif

  return Status;
}

// Get number of Message object on Messages container
unsigned int Process::GetNumberOfMessages()
{
  return NoM;
}

// Get message at Message position _Index
Message* Process::GetMessage(unsigned int _Index)
{
  Message* Temp = 0;

  if (_Index < MAX_MESSAGES_IN_MEMORY)
  {
    Temp = Messages[_Index];
  }

  return Temp;
}

void Process::ShowMessages()
{
  unsigned int NoCL;

  cout << endl
       << "(------------------------- Showing the " << NoM << " messages in memory -------------------------)" << endl;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == BUSY) // Only the occupied positions
    {
      Messages[i]->GetNumberofCommandLines(NoCL);

      cout << "(Message with index " << i << ", " << NoCL << " command lines, delete flag (true (1) - false (0)) = " << Messages[i]->GetDeleteFlag() << ", application deleted flag = " << Messages[i]->GetApplicationDeletedFlag() << ".)" << endl;

      cout << "(" << endl
           << *Messages[i] << ")" << endl
           << endl;
    }
    else
    {

      cout << "(Empty index " << i << ")" << endl;
    }
  }
}

void Process::DeleteMessages()
{
#ifdef DEBUG
  cout << endl
       << "(------------------------- Deleting messages in memory -------------------------)" << endl;
#endif

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == BUSY) // Only the occupied positions
    {
      Message* Temp = Messages[i];

      if (Temp->GetDeleteFlag() == true)
      {

#ifdef DEBUG
        cout << "(The following message with index " << i << " will be deleted.)" << endl;

        cout << "(" << endl
             << *Messages[i] << ")" << endl
             << endl;
#endif

        delete Temp;

        Messages[i] = NULL;

        Controls[i] = FREE;

        NoM--;
      }
    }
  }
}

// Delete the messages marked to be deleted
void Process::DeleteMarkedMessages()
{
#ifdef DEBUG
  cout << endl
       << "(------------------------- Deleting marked messages in memory -------------------------)" << endl;
#endif

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == BUSY)
    {
      Message* Temp = Messages[i];

      if (Temp->GetDeleteFlag() == true)
      {

#ifdef DEBUG
        cout << "(The following message with index " << i << " will be deleted.)" << endl;

        cout << "(" << endl
             << *Messages[i] << ")" << endl
             << endl;

#endif

        delete Temp;

        Messages[i] = NULL;

        Controls[i] = FREE; // Frees the position

        NoM--;
      }
#ifdef DEBUG
      else
      {

        cout << i << " busy, but not marked to be deleted." << endl;

        cout << "(" << endl
             << *Messages[i] << ")" << endl
             << endl;
      }
#endif
    }
#ifdef DEBUG
    else
    {
      cout << i << " is free to new messages." << endl;
    }
#endif
  }
}

void Process::MarkUnmarkedMessagesPerTime(double _Threshold)
{

  // S << "( Marking messages according to time Threshold)"<< endl;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == BUSY)
    {
      Message* Temp = Messages[i];

      if (Temp->GetDeleteFlag() == false && Temp->GetApplicationDeletedFlag() == DELETED_BY_CORE && Temp->GetTime() < _Threshold)
      {
        Temp->MarkToDelete();
      }
    }
  }
}

void Process::MarkMalformedMessagesPerNoCLs(unsigned int _NoCL)
{
  // S << "( Marking messages according to number of command lines)"<< endl;

  unsigned int NoCL = 0;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Controls[i] == BUSY)
    {
      Message* Temp = Messages[i];

      if (Temp->GetDeleteFlag() == false && Temp->GetApplicationDeletedFlag() == DELETED_BY_CORE)
      {
        Temp->GetNumberofCommandLines(NoCL);

        if (NoCL <= _NoCL)
        {
          Temp->MarkToDelete();
        }
      }
    }
  }
}

// Test if a message is OK to run it
bool Process::OkToRun(Message* _M)
{
  bool Status = false;

  for (unsigned int i = 0; i < MAX_MESSAGES_IN_MEMORY; i++)
  {
    if (Messages[i] == _M)
    {
      if (Controls[i] == BUSY)
      {
        Status = true;

        break;
      }
    }
  }

  return Status;
}

// Run the Gateway
void Process::RunGateway()
{
  GW* PGW = 0;
  Block* PGWB = 0;

  // Get the GW pointer
  GetBlock("GW", PGWB);

  // Cast to GW pointer
  PGW = (GW*)PGWB;

  // Run the gateway
  // Block main thread execution on this function
  PGW->Gateway();
}

// Initiate a Command Line Prompt for users
void Process::RunPrompt()
{
  Block* PCLIB = 0;

  GetBlock("CLI", PCLIB);

  // Cast to CLI pointer
  CLI* PCLI = (CLI*)PCLIB;

  // Create a CLI prompt thread
  tthread::thread t1(&CLI::PromptThreadWrapper, PCLI);

  // Wait for the threads to finish
  t1.join();
}

// Set a value behind a key
int Process::StoreHTBindingValues(unsigned int _Category, string _Key, vector<string>* _Values, Block* _PB)
{
  int Status = ERROR;
  Block* PHTB = 0;
  HT* PHT = 0;
  string Offset = "                                        ";

  GetBlock(0, PHTB);

  if (PHTB != 0)
  {
    PHT = (HT*)PHTB;

    if (PHT->StoreBinding(_Category, _Key, _Values) == OK)
    {
      Status = OK;
    }
  }

  return Status;
}

// Discover the values behind a key
int Process::GetHTBindingValues(unsigned int _Category, string _Key, vector<string>*& _Values)
{
  int Status = ERROR;
  Block* PHTB = 0;
  HT* PHT = 0;
  string Offset = "                                        ";

  GetBlock(0, PHTB);

  if (PHTB != 0)
  {
    PHT = (HT*)PHTB;

    if (PHT->GetBinding(_Category, _Key, _Values) == OK)
    {
      Status = OK;
    }
    else
    {
      // cout << Offset <<  "(Warning: The key "<<_Key<<" does not exist on the HT Block at category "<<_Category<<")" << endl;
    }
  }

  return Status;
}

void Process::ListBindings()
{
  Message* PListBindingsMessage = 0;
  CommandLine* PListBindingsCL;
  Block* PHTB = 0;
  Message* InlineResponseMessage;

  GetBlock("HT", PHTB);

  if (PHTB != 0)
  {
    // Creating a -run --initialization message
    NewMessage(GetTime(), 0, false, PListBindingsMessage);

    // Adding only the list bindings command line
    PListBindingsMessage->NewCommandLine("-list", "--b", "0.1", PListBindingsCL);

    // Execute the procedure
    PHTB->Run(PListBindingsMessage, InlineResponseMessage);

    // Delete the temporary message
    PListBindingsMessage->MarkToDelete();
  }
}

// Discover the values behind a legible name on category _Cat
int Process::DiscoverHomonymsEntitiesIDsFromLN(unsigned int _Cat, string _LN, vector<string>*& _Values, Block* _PB)
{
  int Status = ERROR;
  Block* PHTB = 0;
  HT* PHT = 0;
  string Offset = "                                        ";
  string HashLN;

  GetBlock("HT", PHTB);

  if (PHTB != 0)
  {
    PHT = (HT*)PHTB;

    HashLN = NameGenerator::GetInstance().GenerateFromString(_LN);

    if (PHT->GetBinding(_Cat, HashLN, _Values) == OK)
    {
      Status = OK;
    }
    else
    {
      // cout << Offset <<  "(Warning: The key "<<HashLN<<" does not exist on the HT Block at category "<<_Cat<<")" << endl;
    }
  }

  return Status;
}

// Discover the values behind a legible name on category _Cat
int Process::DiscoverHomonymsEntitiesIDsFromLN(unsigned int _Cat, string _LN, Block* _PB)
{
  int Status = ERROR;
  Block* PHTB = 0;
  HT* PHT = 0;
  string Offset = "                                        ";
  string HashLN;
  vector<string>* Values = new vector<string>;

  GetBlock("HT", PHTB);

  if (PHTB != 0)
  {
    PHT = (HT*)PHTB;

    HashLN = NameGenerator::GetInstance().GenerateFromString(_LN);

    if (PHT->GetBinding(_Cat, HashLN, Values) == OK)
    {
      Status = OK;
    }
    else
    {
      // cout << Offset <<  "(Warning: The key "<<HashLN<<" does not exist on the HT Block at category "<<_Cat<<")" << endl;
    }
  }

  delete Values;

  return Status;
}

// Discover inside a Process the Blocks that have the same legible names.
int Process::DiscoverHomonymsBlocksBIDsFromPID(string _PID, string _BlockLN, vector<string>*& _BIDs, Block* _PB)
{
  int Status = ERROR;
  string HashProcessLegibleName;
  string HashBlockLegibleName;
  unsigned int Category = 0;
  string Version = "0.1";
  string Key = "";
  vector<string>* BottomUpBIDs = new vector<string>;
  vector<string>* TopDownBIDs = new vector<string>;
  string Offset = "                                        ";

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _PID;

  // cout << Offset <<  "(DiscoverHomonymsBlocksBIDsFromPID: Doing the first consultation for the PID = "<<_PID<<" and Block LN = "<<_BlockLN<<")" << endl;

  // cout << Offset <<  "(PID = " <<_PID<< ")" << endl;

  // Getting the bind values on the HT block
  if (GetHTBindingValues(Category, Key, BottomUpBIDs) == OK)
  {
    if (BottomUpBIDs != 0)
    {
      if (BottomUpBIDs->size() > 0)
      {
        // Generate a SCN from the received block name
        HashBlockLegibleName = NameGenerator::GetInstance().GenerateFromString(_BlockLN);

        // Setting up the category
        Category = 2;

        // Setting up the binding key
        Key = HashBlockLegibleName;

        // cout << Offset <<  "(Doing the second consultation)" << endl;

        // Getting the bind values on the HT block
        if (GetHTBindingValues(Category, Key, TopDownBIDs) == OK)
        {
          if (TopDownBIDs != 0)
          {
            if (TopDownBIDs->size() > 0)
            {
              // cout << Offset <<  "(Preparing the results vector)" << endl;

              // cout << Offset <<  "(BottomUpBIDs size is "<<BottomUpBIDs->size()<<")" << endl;

              // cout << Offset <<  "(TopDownBIDs size is "<<TopDownBIDs->size()<<")" << endl;

              for (unsigned int i = 0; i < BottomUpBIDs->size(); i++)
              {
                //_PB->S << Offset <<  "(BottomUpBIDs["<<i<<"] = " <<BottomUpBIDs->at(i)<< ")" << endl;

                for (unsigned int j = 0; j < TopDownBIDs->size(); j++)
                {
                  //_PB->S << Offset <<  "( TopDownBIDs["<<j<<"] = " <<TopDownBIDs->at(j)<< ")" << endl;

                  if (BottomUpBIDs->at(i) == TopDownBIDs->at(j))
                  {
                    _BIDs->push_back(TopDownBIDs->at(j));

                    //_PB->S << Offset <<  "(Selected BID = " <<TopDownBIDs->at(j)<< ")" << endl;

                    Status = OK;
                  }
                }
              }
            }
            else
            {
              cout << Offset << "(Warning: Top down BIDs vector is empty)" << endl;
            }
          }
          else
          {
            cout << Offset << "(Warning: Top down BIDs vector is null)" << endl;
          }
        }
        else
        {
          // cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
        }
      }
      else
      {
        // cout << Offset <<  "(Warning: Bottom up BIDs vector is empty)" << endl;
      }
    }
    else
    {
      cout << Offset << "(Warning: Bottom up BIDs vector is null)" << endl;
    }
  }
  else
  {
    // cout << Offset <<  "(Warning: First consultation returned an warning status)" << endl;
  }

  delete BottomUpBIDs;
  delete TopDownBIDs;

  return Status;
}

// Discover inside an OS the Processes that have the same legible names.
int Process::DiscoverHomonymsProcessesPIDsFromOSID(string _OSID, string _ProcessLN, vector<string>*& _PIDs, Block* _PB)
{
  int Status = ERROR;
  string HashProcessLegibleName;
  unsigned int Category = 0;
  string Version = "0.1";
  string Key = "";
  vector<string>* BottomUpPIDs = new vector<string>;
  vector<string>* TopDownPIDs = new vector<string>;
  string Offset = "                                        ";

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _OSID;

  // cout << Offset <<  "(DiscoverHomonymsProcessesPIDsFromOSID = "<<_OSID<<" and Process LN = "<<_ProcessLN<<")" << endl;

  // Getting the bind values on the HT block
  if (GetHTBindingValues(Category, Key, BottomUpPIDs) == OK)
  {
    if (BottomUpPIDs != 0)
    {
      if (BottomUpPIDs->size() > 0)
      {
        // Generate a SCN from the received process name
        HashProcessLegibleName = NameGenerator::GetInstance().GenerateFromString(_ProcessLN);

        // Setting up the category
        Category = 2;

        // Setting up the binding key
        Key = HashProcessLegibleName;

        // cout << Offset <<  "(Doing the second consultation)" << endl;

        // Getting the bind values on the HT block
        if (GetHTBindingValues(Category, Key, TopDownPIDs) == OK)
        {
          if (TopDownPIDs != 0)
          {
            if (TopDownPIDs->size() > 0)
            {
              // cout << Offset <<  "(Preparing the results vector)" << endl;

              // cout << Offset <<  "(BottomUpPIDs size is "<<BottomUpPIDs->size()<<")" << endl;

              // cout << Offset <<  "(TopDownPIDs size is "<<TopDownPIDs->size()<<")" << endl;

              for (unsigned int i = 0; i < BottomUpPIDs->size(); i++)
              {
                //_PB->S << Offset <<  "(BottomUpPIDs["<<i<<"] = " <<BottomUpPIDs->at(i)<< ")" << endl;

                for (unsigned int j = 0; j < TopDownPIDs->size(); j++)
                {
                  //_PB->S << Offset <<  "( TopDownPIDs["<<j<<"] = " <<TopDownPIDs->at(j)<< ")" << endl;

                  if (BottomUpPIDs->at(i) == TopDownPIDs->at(j))
                  {
                    _PIDs->push_back(TopDownPIDs->at(j));

                    //_PB->S << Offset <<  "(Selected PID = " <<TopDownPIDs->at(j)<< ")" << endl;

                    Status = OK;
                  }
                }
              }
            }
            else
            {
              cout << Offset << "(Warning: Top down PIDs vector is empty)" << endl;
            }
          }
          else
          {
            cout << Offset << "(Warning: Top down PIDs vector is null)" << endl;
          }
        }
        else
        {
          // cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
        }
      }
      else
      {
        cout << Offset << "(Warning: Bottom up PIDs vector is empty)" << endl;
      }
    }
    else
    {
      // cout << Offset <<  "(Warning: Bottom up PIDs vector is null)" << endl;
    }
  }
  else
  {
    // cout << Offset <<  "(Warning: First consultation returned an warning status)" << endl;
  }

  delete BottomUpPIDs;
  delete TopDownPIDs;

  return Status;
}

// Discover inside a Process the Blocks that have the same legible names.
int Process::DiscoverHomonymsBlocksBIDsFromProcessLegibleName(string _ProcessLN, string _BlockLN, string& _PID, vector<string>*& _BIDs, Block* _PB)
{
  int Status = ERROR;
  string HashProcessLegibleName;
  string HashBlockLegibleName;
  unsigned int Category = 3;
  string Version = "0.1";
  string Key = "";
  vector<string>* PIDs = new vector<string>;
  vector<string>* BottomUpBIDs = new vector<string>;
  vector<string>* TopDownBIDs = new vector<string>;
  string Offset = "                                        ";

  // ***************************************************
  // Discovering the BID of the related PID
  // ***************************************************

  // Generate a SCN from the received process name
  HashProcessLegibleName = NameGenerator::GetInstance().GenerateFromString(_ProcessLN);

  // Setting up the category
  Category = 2;

  // Setting up the binding key
  Key = HashProcessLegibleName;

#ifdef DEBUG1

  cout << Offset << "HashProcessLegibleName = " << HashProcessLegibleName << endl;

  cout << Offset << "(DiscoverHomonymsBlocksBIDsFromProcessLegibleName = " << _ProcessLN << " and Block LN = " << _BlockLN << ")" << endl;

#endif

  // Getting the process PID
  if (GetHTBindingValues(Category, Key, PIDs) == OK)
  {
    if (PIDs != 0)
    {
      if (PIDs->size() > 0)
      {
        // Setting up the category
        Category = 5;

        // Setting up the binding key
        Key = PIDs->at(0);

        _PID = Key;

#ifdef DEBUG1
        cout << Offset << "(PID = " << Key << ")" << endl;

        cout << Offset << "(Doing the second consultation)" << endl;
#endif

        // Getting the bind values on the HT block
        if (GetHTBindingValues(Category, Key, BottomUpBIDs) == OK)
        {
          if (BottomUpBIDs != 0)
          {
            if (BottomUpBIDs->size() > 0)
            {
              // Generate a SCN from the received block name
              HashBlockLegibleName = NameGenerator::GetInstance().GenerateFromString(_BlockLN);

              // Setting up the category
              Category = 2;

              // Setting up the binding key
              Key = HashBlockLegibleName;

#ifdef DEBUG1
              cout << Offset << "(Doing the third consultation)" << endl;
#endif

              // Getting the bind values on the HT block
              if (GetHTBindingValues(Category, Key, TopDownBIDs) == OK)
              {
                if (TopDownBIDs != 0)
                {
                  if (TopDownBIDs->size() > 0)
                  {

#ifdef DEBUG1
                    cout << Offset << "(Preparing the results vector)" << endl;

                    cout << Offset << "(BottomUpBIDs size is " << BottomUpBIDs->size() << ")" << endl;

                    cout << Offset << "(TopDownBIDs size is " << TopDownBIDs->size() << ")" << endl;
#endif

                    for (unsigned int i = 0; i < BottomUpBIDs->size(); i++)
                    {
#ifdef DEBUG1
                      _PB->S << Offset << "(BottomUpBIDs[" << i << "] = " << BottomUpBIDs->at(i) << ")" << endl;
#endif
                      for (unsigned int j = 0; j < TopDownBIDs->size(); j++)
                      {
#ifdef DEBUG1
                        _PB->S << Offset << "( TopDownBIDs[" << j << "] = " << TopDownBIDs->at(j) << ")" << endl;
#endif

                        if (BottomUpBIDs->at(i) == TopDownBIDs->at(j))
                        {
                          _BIDs->push_back(TopDownBIDs->at(j));
#ifdef DEBUG1
                          _PB->S << Offset << "(BID = " << TopDownBIDs->at(j) << ")" << endl;
#endif

                          Status = OK;
                        }
                      }
                    }
                  }
                  else
                  {
                    cout << Offset << "(Warning: Top down BIDs vector is empty)" << endl;
                  }
                }
                else
                {
                  cout << Offset << "(Warning: Top down BIDs vector is null)" << endl;
                }
              }
              else
              {
                // cout << Offset <<  "(Warning: Third consultation returned an warning status)" << endl;
              }
            }
            else
            {
              cout << Offset << "(Warning: Bottom up BIDs vector is empty)" << endl;
            }
          }
          else
          {
            // cout << Offset <<  "(Warning: Bottom up BIDs vector is null)" << endl;
          }
        }
        else
        {
          // cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
        }
      }
      else
      {
        cout << Offset << "(Warning: PIDs vector is empty)" << endl;
      }
    }
    else
    {
      cout << Offset << "(Warning: PIDs vector is null)" << endl;
    }
  }
  else
  {
    cout << Offset << "(Warning: First consultation returned an warning status)" << endl;
  }

  delete PIDs;
  delete BottomUpBIDs;
  delete TopDownBIDs;

  return Status;
}

// Discover inside a Domain entities' 4 level-tuples that have the same legible names.
int Process::DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames(string _ProcessLN, string _BlockLN, vector<Tuple*>& _4LTuple, Block* _PB)
{
  int Status = ERROR;
  string Offset = "                                        ";

  vector<string>* HIDs = new vector<string>;
  vector<string>* OSIDs = new vector<string>;
  vector<string>* PIDs = new vector<string>;
  vector<string>* BIDs = new vector<string>;

  // cout << Offset <<  "(DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames = "<<_ProcessLN<<" and Block LN = "<<_BlockLN<<")" << endl;

  if (DiscoverHomonymsEntitiesIDsFromLN(9, "Host", HIDs, _PB) == OK)
  {
#ifdef DEBUG3
    cout << Offset << "(HIDs.size() = " << HIDs->size() << ")" << endl;
#endif

    for (unsigned int i = 0; i < HIDs->size(); i++)
    {
#ifdef DEBUG3
      cout << endl
           << Offset << "(HID [" << i << "] = " << HIDs->at(i) << ")" << endl;
#endif
      if (DiscoverOSIDsFromHID(HIDs->at(i), OSIDs, _PB) == OK)
      {

#ifdef DEBUG3
        cout << Offset << "(OSIDs.size() = " << OSIDs->size() << ")" << endl;
#endif

        for (unsigned int j = 0; j < OSIDs->size(); j++)
        {
#ifdef DEBUG3
          cout << endl
               << Offset << "(OSID [" << j << "] = " << OSIDs->at(j) << ")" << endl;
#endif

          if (DiscoverHomonymsProcessesPIDsFromOSID(OSIDs->at(j), _ProcessLN, PIDs, _PB) == OK)
          {
#ifdef DEBUG3
            cout << Offset << "(PIDs.size() = " << PIDs->size() << ")" << endl;
#endif

            for (unsigned int k = 0; k < PIDs->size(); k++)
            {
#ifdef DEBUG3
              cout << endl
                   << Offset << "(PID [" << k << "] = " << PIDs->at(k) << ")" << endl;
#endif

              if (DiscoverHomonymsBlocksBIDsFromPID(PIDs->at(k), _BlockLN, BIDs, _PB) == OK)
              {
#ifdef DEBUG3
                cout << Offset << "(BIDs.size() = " << BIDs->size() << ")" << endl;
#endif

                if (BIDs->size() == 1)
                {
                  Tuple* Temp = new Tuple;

                  Temp->Values.push_back(HIDs->at(i));

                  Temp->Values.push_back(OSIDs->at(j));

                  Temp->Values.push_back(PIDs->at(k));

                  Temp->Values.push_back(BIDs->at(0));

                  _4LTuple.push_back(Temp);

#ifdef DEBUG3
                  cout << Offset << "(Saving a four level tuple!)" << endl;
#endif

                  Status = OK;
                }
                else
                {
                  cout << Offset << "(Warning: Unexpected number of homonyms blocks)" << endl;
                }

                BIDs->clear();
              }
              else
              {
                // cout << Offset <<  "(Warning: Fourth consultation returned an warning status)" << endl;
              }
            }

            PIDs->clear();
          }
          else
          {
            // cout << Offset <<  "(Warning: Third consultation returned an warning status)" << endl;
          }
        }

        OSIDs->clear();
      }
      else
      {
        // cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
      }
    }
  }
  else
  {
    // cout << Offset <<  "(Warning: First consultation returned an warning status)" << endl;
  }

  HIDs->clear();
  OSIDs->clear();
  PIDs->clear();
  BIDs->clear();

  delete HIDs;
  delete OSIDs;
  delete PIDs;
  delete BIDs;

  return Status;
}

// Discover inside a Process the existent Blocks
int Process::DiscoverBIDsFromPID(string _PID, vector<string>*& _BIDs, Block* _PB)
{
  int Status = ERROR;
  unsigned int Category = 0;
  string Version = "0.1";
  string Key = "";
  string Offset = "                                        ";

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _PID;

  // cout << Offset <<  "(DiscoverBIDsFromPID = "<<_PID<<")" << endl;

  // Getting the bind values on the HT block
  if (GetHTBindingValues(Category, Key, _BIDs) == OK)
  {
    Status = OK;
  }
  else
  {
    cout << Offset << "(Warning: The GetHTBinding function returned warning status.)" << endl;
  }

  return Status;
}

// Discover inside an OS the existent Processes
int Process::DiscoverPIDsFromOSID(string _OSID, vector<string>*& _PIDs, Block* _PB)
{
  int Status = ERROR;
  unsigned int Category = 0;
  string Version = "0.1";
  string Key = "";
  string Offset = "                                        ";

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _OSID;

  cout << Offset << "(DiscoverPIDsFromOSID = " << _OSID << ")" << endl;

  // Getting the bind values on the HT block
  if (GetHTBindingValues(Category, Key, _PIDs) == OK)
  {
    Status = OK;
  }
  else
  {
    cout << Offset << "(Warning: The GetHTBinding function returned warning status.)" << endl;
  }

  return Status;
}

// Discover inside a Host the existent OSs
int Process::DiscoverOSIDsFromHID(string _HID, vector<string>*& _OSIDs, Block* _PB)
{
  int Status = ERROR;
  unsigned int Category = 0;
  string Version = "0.1";
  string Key = "";
  string Offset = "                                        ";

  // Setting up the category
  Category = 6;

  // Setting up the binding key
  Key = _HID;

  // cout << Offset <<  "(DiscoverOSIDsFromHID = "<<_HID<<")" << endl;

  // Getting the bind values on the HT block
  if (GetHTBindingValues(Category, Key, _OSIDs) == OK)
  {
    Status = OK;
  }
  else
  {
    cout << Offset << "(Warning: The GetHTBinding function returned warning status.)" << endl;
  }

  return Status;
}

// Discover inside a Domain the existent Hosts
int Process::DiscoverHIDsFromDID(string _DID, vector<string>*& _HIDs, Block* _PB)
{
  int Status = ERROR;
  unsigned int Category = 0;
  string Version = "0.1";
  string Key = "";
  string Offset = "                                        ";

  // Setting up the category
  Category = 7;

  // Setting up the binding key
  Key = _DID;

  // cout << Offset <<  "(DiscoverHIDsFromDID = "<<_DID<<")" << endl;

  // Getting the bind values on the HT block
  if (GetHTBindingValues(Category, Key, _HIDs) == OK)
  {
    Status = OK;
  }
  else
  {
    cout << Offset << "(Warning: The GetHTBinding function returned warning status.)" << endl;
  }

  return Status;
}

double Process::GetTime()
{
  struct timespec t;

  clock_gettime(CLOCK_MONOTONIC, &t);

  return ((t.tv_sec) + (double)(t.tv_nsec / 1e9));
}

string Process::DoubleToString(double _Double)
{
  stringstream ss;

  ss << _Double;

  return ss.str();
}

// Converting string to double
double Process::StringToDouble(string _String)
{
  return atof(_String.c_str());
}
```

## FILE: baseline@1af604d:Common/src/Message.cpp

Role: Exact source bytes from the pinned functional baseline; read-only design context.
Bytes: 57403
SHA-256: 96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518

```text
/*
        NovaGenesis

        Name:		Message
        Object:		Message
        File:		Message.cpp
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.1

        Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#include <string.h>

////#define DEBUG // To follow message processing
////#define DEBUG1 // To follow message deletion

Message::Message(double _Time, short _Type, bool _HasPayload)
{
  // HeaderFile.SetName("");
  PayloadFile.SetName("");
  // MessageFile.SetName("");

  InstantiationTime = GetSystemTime();

  SCN = "";
  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;

  // HeaderSize=0;
  PayloadSize = 0;
  MessageSize = 0;

  // DeleteHeaderArray=false;
  DeletePayloadArray = false;
  DeleteMessageArray = false;

  // Header=0;
  Payload = 0;
  Msg = 0;

  Delete = false;
  ApplicationDeleted = DELETED_BY_CORE;

  InstantiationNumber = 0;

  NoCL = 0;
  CommandLines = NULL;
}

Message::Message(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path)
{
  // HeaderFile.SetName(_HeaderFileName);
  // HeaderFile.SetPath(_Path);
  // HeaderFile.SetOption("BINARY");

  PayloadFile.SetName("");
  // MessageFile.SetName("");

  InstantiationTime = GetSystemTime();

  SCN = "";
  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;

  // HeaderSize=0;
  PayloadSize = 0;
  MessageSize = 0;

  // DeleteHeaderArray=false;
  DeletePayloadArray = false;
  DeleteMessageArray = false;

  // Header=0;
  Payload = 0;
  Msg = 0;

  Delete = false;
  ApplicationDeleted = DELETED_BY_CORE;

  InstantiationNumber = 0;

  NoCL = 0;
  CommandLines = NULL;
}

Message::Message(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path)
{
  // HeaderFile.SetName(_HeaderFileName);
  // HeaderFile.SetPath(_Path);
  // HeaderFile.SetOption("BINARY");

  PayloadFile.SetName(_PayloadFileName);
  PayloadFile.SetPath(_Path);
  PayloadFile.SetOption("BINARY");

  // MessageFile.SetName(_MessageFileName);
  // MessageFile.SetPath(_Path);
  // MessageFile.SetOption("BINARY");

  SCN = "";

  InstantiationTime = GetSystemTime();

  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;

  // HeaderSize=0;
  PayloadSize = 0;
  MessageSize = 0;

  // DeleteHeaderArray=false;
  DeletePayloadArray = false;
  DeleteMessageArray = false;

  // Header=0;
  Payload = 0;
  Msg = 0;

  Delete = false;
  ApplicationDeleted = DELETED_BY_CORE;

  InstantiationNumber = 0;

  NoCL = 0;
  CommandLines = NULL;
}

// Message copy constructor
Message::Message(const Message& M)
{
  // Header=0;
  Payload = 0;
  Msg = 0;

  string Option = "BINARY";

  // HeaderFile.SetName(M.HeaderFile.GetName());
  // HeaderFile.SetPath(M.HeaderFile.GetPath());
  // HeaderFile.SetOption(Option);

  PayloadFile.SetName(M.PayloadFile.GetName());
  PayloadFile.SetPath(M.PayloadFile.GetPath());
  PayloadFile.SetOption(Option);

  // MessageFile.SetName(M.MessageFile.GetName());
  // MessageFile.SetPath(M.MessageFile.GetPath());
  // MessageFile.SetOption(Option);

  SetHasPayloadFlag(M.GetHasPayloadFlag());

  InstantiationTime = GetSystemTime();

  NoCL = 0;
  CommandLines = NULL;

  for (unsigned int i = 0; i < M.NoCL; i++)
  {
    CommandLine* PCL1 = 0;
    CommandLine* PCL2 = 0;

    PCL1 = M.CommandLines[i];

    if (PCL1 != 0)
    {
      NewCommandLine(PCL1, PCL2);
    }
  }

  /*

  if (M.HeaderSize > 0)
          {
                  HeaderSize=M.HeaderSize;

                  Header=new char[HeaderSize];

                  for (int j=0; j<HeaderSize ;j++)
                          {
                                  Header[j]=M.Header[j];
                          }
          }*/

  if (M.PayloadSize > 0)
  {
    PayloadSize = M.PayloadSize;

    Payload = new char[PayloadSize];

    for (int k = 0; k < PayloadSize; k++)
    {
      Payload[k] = M.Payload[k];
    }
  }

  // Do not initialize those values, because after that it is not possible to change the array
  MessageSize = 0;
  DeleteMessageArray = false;

  SCN = M.GetSelfCertifyingName();
  Time = M.GetTime();
  Type = M.GetType();
  Tag = M.GetTag();

  Delete = M.Delete;
  ApplicationDeleted = M.ApplicationDeleted;

  // DeleteHeaderArray=M.DeleteHeaderArray;
  DeletePayloadArray = M.DeletePayloadArray;

  InstantiationNumber = 0;
}

Message::~Message()
{
  // cout << "(" << endl << *this << ")"<< endl << endl << endl;

  // cout << "(The message instantion number is "<<InstantiationNumber<<")"<<endl;
  // cout << "(The size of the header is "<<HeaderSize<<" bytes)"<<endl;
  // cout << "(The size of the payload is "<<PayloadSize<<" bytes)"<<endl;
  // cout << "(The size of the message is "<<MessageSize<<" bytes)"<<endl;
  // cout << "(The delete header flag is "<<DeleteHeaderArray<<" (0=false, 1=true))"<<endl;
  // cout << "(The delete payload flag is "<<DeletePayloadArray<<" (0=false, 1=true))"<<endl;
  // cout << "(The delete message flag is "<<DeleteMessageArray<<" (0=false, 1=true))"<<endl;

  /*

  if (DeleteHeaderArray == true && Header != 0 && HeaderSize > 0)
          {
                  delete[] Header;
          }
  else
          {
                  //cout << "(Warning: The header array pointer is null)"<<endl;
          } */

  if (DeletePayloadArray == true && Payload != 0 && PayloadSize > 0)
  {
    delete[] Payload;
  }
  else
  {
    // cout << "(Warning: The payload array pointer is null)"<<endl;
  }

  if (DeleteMessageArray == true && Msg != 0 && MessageSize > 0)
  {
    delete[] Msg;
  }
  else
  {
    // cout << "(Warning: The message array pointer is null)"<<endl;
  }

  if (CommandLines != NULL)
  {
    for (unsigned int i = 0; i < NoCL; i++)
    {
      delete CommandLines[i];
    }

    delete[] CommandLines;
  }
}

void Message::SetMessage(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path)
{
  // HeaderFile.SetName(_HeaderFileName);
  // HeaderFile.SetPath(_Path);
  // HeaderFile.SetOption("BINARY");

  PayloadFile.SetName(_PayloadFileName);
  PayloadFile.SetPath(_Path);
  PayloadFile.SetOption("BINARY");

  // MessageFile.SetName(_MessageFileName);
  // MessageFile.SetPath(_Path);
  // MessageFile.SetOption("BINARY");

  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;
}

// Set message's SCN
void Message::SetSelfCertifyingName(string _SCN)
{
  SCN = _SCN;
}

// Set message's type
void Message::SetType(short _Type)
{
  Type = _Type;
}

void Message::SetHasPayloadFlag(bool _Flag)
{
  HasPayloadFlag = _Flag;
}

// Set time
void Message::SetTime(double _Time)
{
  Time = _Time;
}

// Set tag
void Message::SetTag(unsigned int _Tag)
{
  Tag = _Tag;
}

/*

// Set header file name
void Message::SetHeaderFileName(string _Name)
{
        HeaderFile.SetName(_Name);
} */

// Set payload file name
void Message::SetPayloadFileName(string _Name)
{
  PayloadFile.SetName(_Name);
}

/*
// Set payload file name
void Message::SetMessageFileName(string _Name)
{
        MessageFile.SetName(_Name);
}*/

/*
// Get header file name
string Message::GetHeaderFileName()
{
        string Temp;

        HeaderFile.GetName(Temp);

        return Temp;
}*/

// Get payload file name
string Message::GetPayloadFileName()
{
  string Temp;

  PayloadFile.GetName(Temp);

  return Temp;
}

/*

// Get message file name
string Message::GetMessageFileName()
{
        string Temp;

        MessageFile.GetName(Temp);

        return Temp;
}*/

/*
// Set header file path
void Message::SetHeaderFilePath(string _Path)
{
        HeaderFile.SetPath(_Path);
}*/

// Set payload file path
void Message::SetPayloadFilePath(string _Path)
{
  PayloadFile.SetPath(_Path);
}

/*
// Set payload file path
void Message::SetMessageFilePath(string _Path)
{
        MessageFile.SetPath(_Path);
}*/

/*
// Get header file path
string Message::GetHeaderFilePath()
{
        string Temp;

        HeaderFile.GetPath(Temp);

        return Temp;
}*/

// Get payload file path
string Message::GetPayloadFilePath()
{
  string Temp;

  PayloadFile.GetPath(Temp);

  return Temp;
}

/*
// Get message file path
string Message::GetMessageFilePath()
{
        string Temp;

        MessageFile.GetPath(Temp);

        return Temp;
}*/

// Set message to be deleted
void Message::MarkToDelete()
{
  // Only mark to delete if the Application Deleted flag was false.
  // If true, the application should make the Application Deleted flag false first to enable automatic deletion.
  // If the application does not turn the Application Deleted flag false the message will be never deleted.
  // SetApplicationDeletedFlag(DELETED_BY_CORE)
  if (ApplicationDeleted == DELETED_BY_CORE)
  {
    Delete = true;

#ifdef DEBUG1
    cout << "Marking the following message to delete" << endl;

    cout << "(" << endl
         << *this << ")" << endl;
#endif
  }
}

// Avoid a message to be deleted
void Message::UnmarkToDelete()
{
  Delete = false;
}

// Set application driven deletion
void Message::SetApplicationDeletedFlag(bool _Flag)
{
  ApplicationDeleted = _Flag;
}

/*
// Set header file option
void Message::SetHeaderFileOption(string _Option)
{
        HeaderFile.SetOption(_Option);
}*/

// Set payload file option
void Message::SetPayloadFileOption(string _Option)
{
  PayloadFile.SetOption(_Option);
}

/*
// Set payload file option
void Message::SetMessageFileOption(string _Option)
{
        MessageFile.SetOption(_Option);
}*/

/*
// Get header file option
string Message::GetHeaderFileOption()
{
        string Temp;

        HeaderFile.GetOption(Temp);

        return Temp;
}*/

// Get payload file option
string Message::GetPayloadFileOption()
{
  string Temp;

  PayloadFile.GetOption(Temp);

  return Temp;
}

/*
// Get message file option
string Message::GetMessageFileOption()
{
        string Temp;

        MessageFile.GetOption(Temp);

        return Temp;
}*/

// Get message's SCN
string Message::GetSelfCertifyingName() const
{
  return SCN;
}

// Get message's type
short Message::GetType() const
{
  return Type;
}

bool Message::GetHasPayloadFlag() const
{
  return HasPayloadFlag;
}

// Get time
double Message::GetTime() const
{
  return Time;
}

// Get tag
unsigned int Message::GetTag() const
{
  return Tag;
}

// Set message to be deleted
bool Message::GetDeleteFlag()
{
  return Delete;
}

// Set application driven deletion
bool Message::GetApplicationDeletedFlag()
{
  return ApplicationDeleted;
}

int Message::NewCommandLine(CommandLine*& CL)
{
  CommandLine* PCL = new CommandLine;

  CL = PCL;

  CommandLine** Temp = new CommandLine*[NoCL + 1];

  for (unsigned int i = 0; i < NoCL; i++)
  {
    Temp[i] = CommandLines[i];
  }

  Temp[NoCL] = PCL;

  if (CommandLines != NULL)
  {
    delete[] CommandLines;
  }

  CommandLines = Temp;

  NoCL++;

  return OK;
}

int Message::NewCommandLine(CommandLine* _PCL, CommandLine*& CL)
{
  CommandLine* PCL = new CommandLine(*_PCL);

  CL = PCL;

  CommandLine** Temp = new CommandLine*[NoCL + 1];

  for (unsigned int i = 0; i < NoCL; i++)
  {
    Temp[i] = CommandLines[i];
  }

  Temp[NoCL] = PCL;

  if (CommandLines != NULL)
  {
    delete[] CommandLines;
  }

  CommandLines = Temp;

  NoCL++;

  return OK;
}

int Message::NewCommandLine(string _Name, string _Alternative, string _Version, CommandLine*& C)
{
  CommandLine* PCL = new CommandLine(_Name, _Alternative, _Version);

  C = PCL;

  CommandLine** Temp = new CommandLine*[NoCL + 1];

  for (unsigned int i = 0; i < NoCL; i++)
  {
    Temp[i] = CommandLines[i];
  }

  Temp[NoCL] = PCL;

  if (CommandLines != NULL)
  {
    delete[] CommandLines;
  }

  CommandLines = Temp;

  NoCL++;

  return OK;
}

int Message::GetCommandLine(unsigned int _Index, CommandLine*& CL)
{
  if (_Index < NoCL)
  {
    CL = CommandLines[_Index];
  }

  return OK;
}

// Get a CommandLine by its name and alternative
int Message::GetCommandLine(string _Name, string _Alternative, CommandLine*& CL)
{
  int Status = ERROR;
  CommandLine* PCL = 0;

  // cout << "NCL = "<< NCL <<endl;

  for (unsigned int i = 0; i < NoCL; i++)
  {
    // Select a command line
    GetCommandLine(i, PCL);

    if (PCL->Name == _Name && PCL->Alternative == _Alternative)
    {
      CL = PCL;

      Status = OK;

      break;
    }
  }

  return Status;
}

// Get a CommandLine Index
int Message::GetCommandLineIndex(CommandLine* CL, unsigned int& _Index)
{
  int Status = ERROR;
  vector<CommandLine*>::iterator it;

  for (unsigned int i = 0; i < NoCL; i++)
  {
    if (CommandLines[i] == CL)
    {
      _Index = i;

      Status = OK;

      break;
    }

    i++;
  }

  return Status;
}

int Message::GetNumberofCommandLines(unsigned int& _Number)
{
  _Number = NoCL;

  return OK;
}

int Message::GetNumberofCommandLineArguments(unsigned int _CommandLine, unsigned int& _Number)
{
  if (_CommandLine < NoCL)
  {
    return CommandLines[_CommandLine]->GetNumberofArguments(_Number);
  }

  return ERROR;
}

int Message::GetNumberofCommandLineArgumentElements(unsigned int _CommandLine, unsigned int _Argument, unsigned int& _Number)
{
  if (_CommandLine < NoCL)
  {
    return CommandLines[_CommandLine]->GetNumberofArgumentElements(_Argument, _Number);
  }

  return ERROR;
}

int Message::SetCommandLineArgumentElement(unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string _Value)
{
  int Status = ERROR;

  if (_CommandLine < NoCL)
  {
    CommandLines[_CommandLine]->SetArgumentElement(_Argument, _Element, _Value);

    Status = OK;
  }

  return Status;
}

int Message::GetCommandLineArgumentElement(unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string& _Value)
{
  if (_CommandLine < NoCL)
  {
    CommandLines[_CommandLine]->GetArgumentElement(_Argument, _Element, _Value);

    return OK;
  }

  return ERROR;
}

/*
 int Message::GetHeaderFile(File& _F)
{
        _F=HeaderFile;

        return OK;
}*/

int Message::GetPayloadFile(File& _F)
{
  _F = PayloadFile;

  return OK;
}

/*
int Message::GetMessageFile(File& _F)
{
        _F=MessageFile;

        return OK;
}*/

/*
// Set *Header from char array. A copy of the char array is done.
int Message::SetHeaderFromCharArray(char* _Value, long _Size)
{
        int Status=ERROR;

        if (HeaderSize == 0 && DeleteHeaderArray == false)
                {
                        if (_Size > 0)
                                {
                                        HeaderSize=_Size;

                                        Header=new char[HeaderSize];

                                        for (long j=0; j<HeaderSize ; j++)
                                                {
                                                        Header[j]=_Value[j];
                                                }

                                        Status=OK;

                                        DeleteHeaderArray=true;
                                }
                }

        return Status;
}
*/

// SPEC-018: Reset payload state so a new payload can be loaded on a reused Message.
// Frees the existing Payload array and resets the flags that prevent re-loading.
void Message::ResetPayload()
{
  if (DeletePayloadArray == true && Payload != 0 && PayloadSize > 0)
  {
    delete[] Payload;
  }

  Payload = 0;
  PayloadSize = 0;
  DeletePayloadArray = false;
  HasPayloadFlag = false;
}

// Set *Payload from char array. A copy of the char array is done.
int Message::SetPayloadFromCharArray(char* _Value, long long _Size)
{
  int Status = ERROR;

  if (PayloadSize == 0 && DeletePayloadArray == false)
  {
    if (_Size > 0)
    {
      PayloadSize = _Size;

      Payload = new char[PayloadSize];

      for (long long j = 0; j < PayloadSize; j++)
      {
        Payload[j] = _Value[j];
      }

      Status = OK;

      DeletePayloadArray = true;

      HasPayloadFlag = true;
    }
  }

  return Status;
}

// Set *Msg from char array. A copy of the char array is done.
int Message::SetMessageFromCharArray(char* _Value, long long _Size)
{
  int Status = ERROR;

  if (MessageSize == 0 && DeleteMessageArray == false)
  {
    if (_Size > 0)
    {
      MessageSize = _Size;

      Msg = new char[MessageSize];

      for (long long j = 0; j < MessageSize; j++)
      {
        Msg[j] = (char)_Value[j];

        // printf("%i %d %c \n",j,Msg[j],Msg[j]);
      }

      Status = OK;

      DeleteMessageArray = true;
    }
  }

  return Status;
}

/*
// Get *Header from char array. Don't delete[] the array pointer
int Message::GetHeaderFromCharArray(char*& _Value)
{
        int Status=ERROR;

        if (HeaderSize > 0 && DeleteHeaderArray == true)
                {
                        _Value=Header;

                        Status=OK;
                }

        return Status;
}*/

// Get *Payload from char array. Don't delete[] the array pointer
int Message::GetPayloadFromCharArray(char*& _Value)
{
  int Status = ERROR;

  if (PayloadSize > 0 && DeletePayloadArray == true)
  {
    _Value = Payload;

    Status = OK;
  }

  return Status;
}

// Get *Msg from char array. Don't delete[] the array pointer
int Message::GetMessageFromCharArray(char*& _Value)
{
  int Status = ERROR;

  if (MessageSize > 0 && DeleteMessageArray == true)
  {
    _Value = Msg;

    Status = OK;
  }

  return Status;
}

// Get *Msg from char array. Don't delete[] the array pointer
int Message::GetMessageFromCharArray(unsigned char*& _Value)
{
  int Status = ERROR;

  if (MessageSize > 0 && DeleteMessageArray == true)
  {
    _Value = (unsigned char*)Msg;

    Status = OK;
  }

  return Status;
}

double Message::GetSystemTime()
{
  struct timespec t;

  clock_gettime(CLOCK_MONOTONIC, &t);

  return ((t.tv_sec) + (double)(t.tv_nsec / 1e9));
}

// Get the object instantiation time in seconds
double Message::GetInstantiationTime()
{
  return InstantiationTime;
}

// ******************************************************
// Header format convertions
// ******************************************************

/*
// Convert the Header from file to a char array pointed by *Header. The HeaderFile handler needs to be properly configured.
int Message::ConvertHeaderFromFileToCharArray()
{
        int Status=ERROR;

        if (HeaderFile.GetName() != "")
                {
                        Status=HeaderFile.OpenInputFile();

                        if (HeaderSize == 0)
                                {
                                        // Read the number of characters in the MessageFile.
                                        HeaderSize=HeaderFile.tellg();

                                        if (HeaderSize > 0)
                                                {
                                                        // Change the position to read from the beginning
                                                        HeaderFile.seekg(0);

                                                        Header=new char[HeaderSize];

                                                        HeaderFile.read(Header,HeaderSize);

                                                        //for (int i=0; i< _Size; i++)
                                                        //	{
                                                        //		printf("%i %d %c \n",i,_Value[i],_Value[i]);
                                                        //	}
                                                }

                                        HeaderFile.CloseFile();

                                        DeleteHeaderArray=true;

                                        Status=OK;
                                }
                }

        return Status;
}
*/

/*
// Convert the Header from CommandLines to a char array pointed by *Header. The CommandLines need to be properly configured.
int Message::ConvertHeaderFromCommandLinesToCharArray()
{
        int Status=ERROR;

        stringstream ss;

        if (NoCL > 0)
                {
                        if (HeaderSize == 0 && DeleteHeaderArray == false)
                                {
                                        //cout << "CommandLines.size() = "<< CommandLines.size() << endl;

                                        for (unsigned int i=0; i< NoCL; i++)
                                                {
                                                        ss << *CommandLines[i];

                                                        //cout << "Command Line:"<< endl<<*CommandLines[i] << endl;
                                                }

                                        string tmp(ss.str());

                                        //cout <<"Header = "<<tmp<< endl;

                                        HeaderSize=(long)tmp.size();

                                        //cout <<"Header size = "<<HeaderSize<< endl;

                                        Header=new char[HeaderSize];

                                        for (long k=0; k<HeaderSize; k++)
                                                {
                                                        Header[k]=tmp[k];
                                                }

                                        DeleteHeaderArray=true;

                                        //for (int i=0; i< HeaderSize; i++)
                                        //	{
                                        //		printf("%i %d %c \n",i,Header[i],Header[i]);
                                        //	}

                                        Status=OK;
                                }
                        else
                                {
                                        cout << "(ERROR: Header convertion error)" << endl;
                                }
                }
        else
                {
                        cout << "(ERROR: Empty header)" << endl;
                }

        return Status;
}*/

/*

// Convert the Header from char array to CommandLines. The char array pointed by *Header needs to be properly configured.
int Message::ConvertHeaderFromCharArrayToCommandLines()
{
        int 		Status=ERROR;

        if (HeaderSize > 0 && DeleteHeaderArray == true && Header != 0)
                {
                        stringstream ss;

                        for (int t=0; t<HeaderSize; t++)
                                {
                                        ss << Header[t];
                                }

                        ss >> *this;

                        Status=OK;
                }

        return Status;
}

*/

/*

// Convert the Header from file to CommandLines. The HeaderFile handler needs to be properly configured.
int Message::ConvertHeaderFromFileToCommandLines()
{
        int Status=0;

        Status=HeaderFile.OpenInputFile();

        HeaderFile >> *this;

        return Status;
}

*/

/*
// Convert the Header from char array to file. The char array pointed by *Header needs to be properly configured as well as the HeaderFile handler
int Message::ConvertHeaderFromCharArrayToFile()
{
        int Status=ERROR;

        if (HeaderFile.GetName() != "")
                {
                        Status=HeaderFile.OverwriteOutputFile();

                        if (HeaderSize > 0 && DeleteHeaderArray == true)
                                {
                                        // Change the position to read from the beginning
                                        HeaderFile.seekg(0);

                                        // Write char array content to the MessageFile
                                        HeaderFile.write(Header,HeaderSize);

                                        //for (int i=0; i< _Size; i++)
                                        //	{
                                        //		printf("%i %d %c \n",i,_Value[i],_Value[i]);
                                        //	}

                                        HeaderFile.CloseFile();
                                }
                }

        return Status;
}

*/

/*
// Convert the Header from CommandLines to file. The CommandLines need to be properly configured as well as the HeaderFile handler
int Message::ConvertHeaderFromCommandLinesToFile()
{
        int Status=OK;CommandLines

        if (NoCL > 0)
                {
                        if (HeaderFile.GetName() != "" && DeleteHeaderArray == false)
                                {
                                        Status=HeaderFile.OverwriteOutputFile();

                                        for (unsigned int i=0; i< NoCL; i++)
                                                {
                                                        HeaderFile << *CommandLines[i];
                                                }

                                        DeleteHeaderArray=true;

                                        Status=OK;

                                        HeaderFile.CloseFile();
                                }
                }

        return Status;
}
*/

// ******************************************************
// Payload format convertions
// ******************************************************

// Convert the Payload from file to a char array pointed by *Payload. The PayloadFile handler needs to be properly configured.
int Message::ConvertPayloadFromFileToCharArray()
{
  int Status = OK;
  long long Length = 0;

  if (PayloadFile.GetName() != "")
  {
    Status = PayloadFile.OpenInputFile();

    if (PayloadSize == 0 && DeletePayloadArray == false && Status == OK && HasPayloadFlag == true)
    {
      // Stores current position
      Length = PayloadFile.tellg();

      // Change position to the beginning
      PayloadFile.seekg(0);

      if (Length > 0)
      {
        PayloadSize = Length;

        Payload = new char[PayloadSize];

        PayloadFile.read(Payload, PayloadSize);

        DeletePayloadArray = true;

        PayloadFile.CloseFile();
      }
    }
  }

  return Status;
}

// Convert the Payload from char array to file. The char array pointed by *Payload needs to be properly configured as well as the PayloadFile handler
int Message::ConvertPayloadFromCharArrayToFile()
{
  int Status = OK;

  if (PayloadFile.GetName() != "")
  {
    PayloadFile.OpenOutputFile(PayloadFile.GetName(), PayloadFile.GetPath(), PayloadFile.GetOption());

    Status = PayloadFile.OverwriteOutputFile();

    if (PayloadSize > 0 && DeletePayloadArray == true)
    {
      // Change the position to read from the beginning
      PayloadFile.seekg(0);

      // Write char array content to the PayloadFile
      PayloadFile.write(Payload, PayloadSize);

      for (long long i = 0; i < PayloadSize; i++)
      {
        // printf("%i %d %c \n",i,Payload[i],Payload[i]);
      }

      PayloadFile.CloseFile();
    }
    else
    {
      cout << "ERROR: Payload size equals to zero or delete payload arrays flag is false" << endl;
    }
  }
  else
  {
    cout << "ERROR: Empty file name" << endl;
  }

  return Status;
}

// Extract Payload char array from message char array. The Msg char array needs to be properly configured.
int Message::ExtractPayloadCharArrayFromMessageCharArray()
{
  int Status = OK;
  string Option;
  string ng = "ng";
  long long Position = 0;
  long long Size = 0;
  long long length = 0;
  stringstream ss;

  // cout << "          (Message size = " <<MessageSize<< endl;

  for (long long i = 0; i < MessageSize; i++)
  {
    ss << Msg[i];

    // printf("%i %d %c \n",i,Msg[i],Msg[i]);
  }

  ss.seekg(0);

  char Line[4096];

  while (ss.getline(Line, sizeof(Line), '\n'))
  {
    if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
    {
    }
    else
    {
      break;
    }
  }

  // Stores current position
  Position = ss.tellg();

  // Get the total size
  Size = ss.str().length();

  // cout << "Size = " << Size << endl;

  // Calculates the length of the payload portion
  length = Size - Position;

  // cout << "length = " << length << endl;

  // cout << "Reading from position = " << Position << endl;

  if (length > 1 && Position > 0 && Size > 0)
  {
    // There is a payload
    HasPayloadFlag = true;

    // Change the position to write at the beginning
    PayloadFile.seekp(0, ios::beg);

    Payload = new char[length];

    ss.read(Payload, length);

    PayloadSize = length;

    // cout << "Payload size = " << PayloadSize << endl;

    DeletePayloadArray = true;
  }

  // cout << "          (The extraction of the payload was a sucess)" << endl;

  return Status;
}

// ******************************************************
// Entire message format conversions
// ******************************************************

/*
// Convert the Message from file to CommandLines and Payload file. The MessageFile handler needs to be properly configured
int Message::ConvertMessageFromFileToCommandLinesandPayloadFile()
{
        int Status=ERROR;

        if (MessageFile.GetName() != "")
                {
                        Status=MessageFile.OpenInputFile();

                        MessageFile >> *this;

                        MessageFile.CloseFile();
                }

        return Status;
}

*/

// Convert the Message from file to a char array pointed by *Message. The MessageFile handler needs to be properly configured.
int Message::ConvertMessageFromFileToCharArray()
{
  int Status = ERROR;

  if (MessageFile.GetName() != "")
  {
    // cout << "> Message Name = " << MessageFile.GetName() << endl;

    Status = MessageFile.OpenInputFile();

    // Read the number of characters in the MessageFile.
    MessageSize = MessageFile.tellg();

    // cout << "> Message Size = " << MessageSize << endl;

    if (MessageSize > 0 && DeleteMessageArray == false)
    {
      // Change the position to read from the beginning
      MessageFile.seekg(0);

      Msg = new char[MessageSize];

      MessageFile.read(Msg, MessageSize);

      // for (int i=0; i< MessageSize; i++)
      //	{
      //		printf("%i %d %c \n",i,Msg[i],Msg[i]);
      //	}

      DeleteMessageArray = true;
    }
    else
    {
      cout << "> ERROR: The Msg char array have been previously created. It cannot be recreated in this version."
           << endl;
    }

    MessageFile.CloseFile();
  }

  return Status;
}

/*
// Convert the Message from char array to a complete Message file. The char array pointed by *Message needs to be properly configured as well as the MessageFile handler
int Message::ConvertMessageFromCharArrayToFile()
{
        int Status=ERROR;

        if (MessageFile.GetName() != "")
                {
                        Status=MessageFile.OverwriteOutputFile();

                        if (MessageSize > 0 && DeleteMessageArray == true)
                                {
                                        // Change the position to read from the beginning
                                        MessageFile.seekg(0);

                                        // Write char array content to the MessageFile
                                        MessageFile.write(Msg,MessageSize);

                                        //for (int i=0; i< _Size; i++)
                                        //	{
                                        //		printf("%i %d %c \n",i,_Value[i],_Value[i]);
                                        //	}

                                        MessageFile.CloseFile();
                                }
                }

        return Status;
}

*/

/*
// Convert the Message from CommandLines and Payload file to a complete Message file. The CommandLines need to be properly configured as well as the MessageFile handler
int Message::ConvertMessageFromCommandLinesandPayloadFileToFile()
{
        int Status=ERROR;

        if (MessageFile.GetName() != "")
                {
                        Status=MessageFile.OverwriteOutputFile();

                        char			*buffer;
                        long		    length=0;

                        for (unsigned int i=0; i<NoCL; i++)
                                {
                                        MessageFile << *CommandLines[i];
                                }

                        if (HasPayloadFlag == true && PayloadSize > 0)
                                {
                                        MessageFile << endl; // Add a blank line to separate the payload from the header.

                                        if (PayloadFile.OpenInputFile() == OK)
                                                {
                                                        //cout << "Loading payload from file" << endl;

                                                        // Read the number of characters in the Payload file.
                                                        length=PayloadFile.tellg();

                                                        // Change the position to read from the beginning
                                                        PayloadFile.seekg(0, ios::beg);

                                                        if (length > 0)
                                                                {
                                                                        buffer=new char[length];

                                                                        PayloadFile.read(buffer,length);

                                                                        MessageFile.write(buffer,length);

                                                                        delete[] buffer;
                                                                }

                                                        PayloadFile.CloseFile();
                                                }
                                        else
                                                {
                                                        //cout << "Loading payload from char array" << endl;

                                                        if (PayloadSize > 0)
                                                                {
                                                                        buffer=new char[PayloadSize];

                                                                        for (long long int i=0; i<PayloadSize; i++)
                                                                                {
                                                                                        buffer[i]=(char)Payload[i];
                                                                                }

                                                                        MessageFile.write(buffer,PayloadSize);

                                                                        delete[] buffer;
                                                                }
                                                }
                                }
                }

        return Status;
}

*/

// Convert the Message from CommandLines and Payload file to a char array. The CommandLines need to be properly configured as well as the PayloadFile handler
int Message::ConvertMessageFromCommandLinesandPayloadFileToCharArray()
{
  int Status = ERROR;

  if (MessageSize == 0 && DeleteMessageArray == false)
  {
    ostringstream oss;

    if (NoCL > 0)
    {
      char* buffer;
      long length = 0;

      for (unsigned int i = 0; i < NoCL; i++)
      {
        oss << *CommandLines[i];
      }

      if (HasPayloadFlag == true && PayloadSize > 0)
      {
        oss
            << endl; // Add a blank line to separate the payload ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2from the header.

        if (PayloadFile.OpenInputFile() == OK)
        {
          // cout << "Loading payload from file" << endl;

          // Read the number of characters in the Payload file.
          length = PayloadFile.tellg();

          // Change the position to read from the beginning
          PayloadFile.seekg(0, ios::beg);

          if (length > 0)
          {
            buffer = new char[length];

            PayloadFile.read(buffer, length);

            oss.write(buffer, length);

            delete[] buffer;
          }

          PayloadFile.CloseFile();
        }
        else
        {
          // cout << "Loading payload from char array" << endl;

          if (PayloadSize > 0)
          {
            buffer = new char[PayloadSize];

            for (long long int i = 0; i < PayloadSize; i++)
            {
              buffer[i] = (char)Payload[i];
            }

            oss.write(buffer, PayloadSize);

            delete[] buffer;
          }
        }
      }

      const std::string tmp = oss.str();

      MessageSize = tmp.size();

      Msg = new char[MessageSize];

      for (int i = 0; i < MessageSize; i++)
      {
        Msg[i] = tmp[i];

        // printf("%i %d %c \n",i,Msg[i],Msg[i]);
      }

      DeleteMessageArray = true;

      Status = OK;
    }
  }

  return Status;
}

// Convert the Message from CommandLines and Payload char array to a message char array. The CommandLines need to be properly configured as well as the Payload char array
int Message::ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray()
{
  int Status = ERROR;

  if (MessageSize == 0 && DeleteMessageArray == false)
  {
    ostringstream oss;

    if (NoCL > 0)
    {
      char* buffer;
      long long length = 0;

      for (unsigned int i = 0; i < NoCL; i++)
      {
        oss << *CommandLines[i];
      }

      if (HasPayloadFlag == true && PayloadSize > 0)
      {
        oss << endl; // Add a blank line to separate the payload from the header.

        if (PayloadFile.OpenInputFile() == OK)
        {
#ifdef DEBUG
          cout << "Loading payload from file" << endl;
#endif
          // Read the number of characters in the Payload file.
          length = (long long)PayloadFile.tellg();

          // Change the position to read from the beginning
          PayloadFile.seekg(0, ios::beg);

          if (length > 0)
          {
            buffer = new char[length];

            PayloadFile.read(buffer, length);

            oss.write(buffer, length);

            delete[] buffer;
          }

          PayloadFile.CloseFile();
        }
        else
        {
#ifdef DEBUG
          cout << "Loading payload from char array" << endl;
#endif

          if (PayloadSize > 0)
          {
            buffer = new char[PayloadSize];

#ifdef DEBUG
            cout << "(The size of the payload of the message with instantiation number " << InstantiationNumber << " is " << PayloadSize << " bytes)" << endl;
#endif
            for (long long i = 0; i < PayloadSize; i++)
            {
              buffer[i] = (char)Payload[i];
            }

            oss.write(buffer, PayloadSize);

            delete[] buffer;
          }
        }
      }

      const std::string tmp = oss.str();

      MessageSize = tmp.size();

#ifdef DEBUG
      cout << "(The size of the message with instantiation number " << InstantiationNumber << " is " << MessageSize << " bytes)" << endl;
#endif
      Msg = new char[MessageSize];

      for (long long j = 0; j < MessageSize; j++)
      {
        Msg[j] = tmp[j];
#ifdef DEBUG
        printf("%i %d %c \n", j, Msg[j], Msg[j]);
#endif
      }

      DeleteMessageArray = true;

      Status = OK;
    }
  }

  return Status;
}

// Convert the complete message char array into command lines and payload file

/*
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadFile()
{
        int Status=ERROR;

        if (MessageSize > 0 && DeleteMessageArray == true)
                {
                        stringstream ss;

                        for (int t=0; t<MessageSize; t++)
                                {
                                        ss << Msg[t];

                                        //printf("%i %d %c \n",t,Msg[t],Msg[t]);
                                }

                        ss >> *this;

                        Status=OK;
                }

        return Status;
}

*/

// Convert the complete message char array into command lines and payload char array
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray()
{

  int Status = ERROR;
  CommandLine* PCL = 0;
  string Option;
  string ng = "ng";
  long long int Position = 0;
  long int Size = 0;
  int length = 0;
  unsigned int i = 0;

  // cout<< "Message size = "<<MessageSize<<endl;

  if (MessageSize > 0 && DeleteMessageArray == true)
  {
    stringstream ss;

    for (int t = 0; t < MessageSize; t++)
    {
      ss << Msg[t];

      // printf("%i %d %c \n",t,Msg[t],Msg[t]);
    }

    char Line[4096];

    while (ss.getline(Line, sizeof(Line), '\n'))
    {
      if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
      {
        istringstream ins(Line);

        NewCommandLine(PCL);

        ins >> *CommandLines[i];

        i++;
      }
      else
      {
        break;
      }
    }

    // Stores current position
    Position = ss.tellg();

    // Get the total size
    Size = ss.str().length();

    // cout << "Size = " << Size << endl;

    // Calculates the length of the payload portion
    length = Size - Position;

    // cout << "length = " << length << endl;

    // cout << "Reading from position = " << Position << endl;

    if (length > 1 && Position > 0 && Size > 0)
    {
      // There is a payload
      HasPayloadFlag = true;

      // Change the position to write at the beggining
      PayloadFile.seekp(0);

      Payload = new char[length];

      ss.read(Payload, length);

      PayloadSize = length;

      DeletePayloadArray = true;
    }

    Status = OK;
  }

  return Status;
}

// Convert the complete message char array into command lines and payload char array
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray(File* _PF)
{
  int Status = ERROR;
  CommandLine* PCL = 0;
  string Option;
  long long t = 0; // Pointer to scan the buffer
  char* Temp;

  if (MessageSize > 16 && DeleteMessageArray == true)
  {
    //*_PF<<"MessageSize = "<< MessageSize <<endl;

    // While over the received buffer to delineate command lines and payload
    while (t < MessageSize)
    {
      //*_PF<<endl<<"Jumping to position = "<<t<<endl;

      // Search for a ng command line
      if (Msg[t] == 'n' && Msg[t + 1] == 'g' && Msg[t + 2] == ' ' && Msg[t + 3] == '-')
      {
        //*_PF<<"A command line was found"<<endl;

        // Command line detected
        // Continues up to the end of this line
        for (long long u = t; u < MessageSize; u++)
        {
          if (Msg[u] == '\n')
          {
            int CommandLineSize = (int)(u - t + 1);

            //*_PF<<"Command line size = "<<CommandLineSize<<endl;

            Temp = new char[CommandLineSize];

            //*_PF<<"Showing the discovered command line:"<<endl;

            for (int v = 0; v < (u - t); v++)
            {
              Temp[v] = Msg[t + v];

              //*_PF<<Temp[v];
            }

            //*_PF<<endl<<"Creating the new command line object"<<endl;

            NewCommandLine(PCL);

            //*_PF<<"Copying the temporary array content to command line object"<<endl;

            PCL->ConvertCommandLineFromCharArray(Temp, CommandLineSize);

            //*_PF<<"Deleting the temporary array"<<endl;

            delete Temp;

            //*_PF<<endl<<"Jumping to position = "<<u<<endl;

            t = u;

            break;
          }
        }
      }
      else
      {
        //*_PF<<"Jumping the blank line."<<endl;

        t++; // Jumps the blank line

        //*_PF<<endl<<"Jumping to position = "<<t<<endl;

        //*_PF<<"Start copying the payload to array and file"<<endl;

        // There is a payload
        HasPayloadFlag = true;

        PayloadSize = MessageSize - t;

        //*_PF<<"Payload size = "<<PayloadSize<<endl;

        Payload = new char[PayloadSize];

        long long r = 0;

        // Continues up	 to the end of the buffer
        for (int u = t; u < MessageSize; u++)
        {
          Payload[r] = Msg[u];

          //*_PF<<Payload[r]<<endl;

          r++;
        }

        DeletePayloadArray = true;

        t = MessageSize - 1;

        //*_PF<<"Passei tudo"<<endl;
      }

      t++;
    }

    Status = OK;
  }

  return Status;
}

// Convert the complete message char array into command lines and payload char array
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()
{

  int Status = OK;
  CommandLine* PCL = 0;
  string Option;
  long long t = 0; // Pointer to scan the buffer
  char* Temp;
  int Aux = OK;

  if (MessageSize > 16 && DeleteMessageArray == true)
  {

#ifdef DEBUG
    cout << "MessageSize = " << MessageSize << endl;
#endif

    // While over the received buffer to delineate command lines and payload
    while (t < MessageSize)
    {
#ifdef DEBUG
      cout << endl
           << "Jumping to position = " << t << endl;
#endif

      // Search for a ng command line
      if (Msg[t] == 'n' && Msg[t + 1] == 'g' && Msg[t + 2] == ' ' && Msg[t + 3] == '-')
      {
#ifdef DEBUG
        cout << "A command line was found" << endl;
#endif

        // Command line detected
        // Continues up to the end of this line
        for (long long u = t; u < MessageSize; u++)
        {
          if (Msg[u] == '\n')
          {
            int CommandLineSize = (int)(u - t + 1);

#ifdef DEBUG
            cout << "Command line size = " << CommandLineSize << endl;
#endif

            Temp = new char[CommandLineSize];

#ifdef DEBUG
            cout << "Showing the discovered command line:" << endl;
#endif

            for (int v = 0; v < (u - t); v++)
            {
              Temp[v] = Msg[t + v];
#ifdef DEBUG
              cout << Temp[v];
#endif
            }

#ifdef DEBUG
            cout << endl
                 << "Creating the new command line object" << endl;
#endif
            NewCommandLine(PCL);

#ifdef DEBUG
            cout << "Copying the temporary array content to command line object" << endl;
#endif

            Aux = PCL->ConvertCommandLineFromCharArray(Temp, CommandLineSize);

            if (Aux == ERROR)
            {
              Status = ERROR;
            }

#ifdef DEBUG
            cout << "Deleting the temporary array" << endl;
#endif
            delete[] Temp;

#ifdef DEBUG
            cout << endl
                 << "Jumping to position = " << u << endl;
#endif
            t = u;

            break;
          }
        }
      }
      else
      {

#ifdef DEBUG
        cout << "Jumping the blank line." << endl;
#endif
        t++; // Jumps the blank line

#ifdef DEBUG
        cout << endl
             << "Jumping to position = " << t << endl;

        cout << "Start copying the payload to array and file" << endl;
#endif
        // There is a payload
        HasPayloadFlag = true;

        PayloadSize = MessageSize - t;

#ifdef DEBUG
        cout << "Payload size = " << PayloadSize << endl;
#endif

        Payload = new char[PayloadSize];

        long long r = 0;

        // Continues up to the end of the buffer
        for (int u = t; u < MessageSize; u++)
        {
          Payload[r] = Msg[u];

#ifdef DEBUG
          cout << Payload[r] << endl;
#endif
          r++;
        }

        DeletePayloadArray = true;

        t = MessageSize - 1;

#ifdef DEBUG
        cout << "Finished" << endl;
#endif
      }

      t++;
    }
  }

  return Status;
}

// ******************************************************
// Message size functions
// ******************************************************

/*
// Get header size in bytes.
int Message::GetHeaderSize(long& _Size)
{
        int Status=ERROR;

        if (DeleteHeaderArray == true)
                {
                        _Size=HeaderSize;

                        Status=OK;
                }

        return Status;
}

*/

// Get payload size in bytes
int Message::GetPayloadSize(long long& _Size)
{
  int Status = ERROR;

  if (DeletePayloadArray == true)
  {
    _Size = PayloadSize;

    Status = OK;
  }

  return Status;
}

// Get message size in bytes
int Message::GetMessageSize(long long& _Size)
{
  int Status = ERROR;

  if (MessageSize > 0)
  {
    _Size = MessageSize;

    Status = OK;
  }
  else
  {
    ostringstream oss;

    if (NoCL > 0)
    {
      char* buffer;
      long length = 0;

      for (unsigned int i = 0; i < NoCL; i++)
      {
        oss << *CommandLines[i];
      }

      if (HasPayloadFlag == true)
      {
        oss << endl; // Add a blank line to separate the payload from the header.

        if (PayloadFile.OpenInputFile() == OK)
        {
          // cout << "Loading payload from file" << endl;

          // Read the number of characters in the Payload file.
          length = PayloadFile.tellg();

          // Change the position to read from the beginning
          PayloadFile.seekg(0, ios::beg);

          if (length > 0)
          {
            buffer = new char[length];

            PayloadFile.read(buffer, length);

            oss.write(buffer, length);

            delete[] buffer;
          }

          PayloadFile.CloseFile();
        }
        else
        {
          // cout << "Loading payload from char array" << endl;

          if (PayloadSize > 0)
          {
            buffer = new char[PayloadSize];

            for (long long int i = 0; i < PayloadSize; i++)
            {
              buffer[i] = (char)Payload[i];
            }

            oss.write(buffer, PayloadSize);

            delete[] buffer;
          }
        }
      }

      const std::string tmp = oss.str();

      _Size = tmp.size();

#ifdef DEBUG
      cout << "Message Size = " << _Size << endl;
#endif

      Status = OK;
    }
  }

  return Status;
}

// ******************************************************
// Checking for command lines
// ******************************************************
int Message::DoesThisCommandLineExistsInMessage(string _Name, string _Alternative, bool& _Verdict)
{
  int Status = ERROR;
  CommandLine* PCL = 0;

  for (unsigned int i = 0; i < NoCL; i++)
  {
    // Select a command line
    GetCommandLine(i, PCL);

    if (PCL->Name == _Name && PCL->Alternative == _Alternative)
    {
      _Verdict = true;

      Status = OK;

      break;
    }
  }

  return Status;
}

// ******************************************************
// Checking for arguments
// ******************************************************
int Message::DoAllTheseValuesExistInSomeCommandLine(vector<string>* _Values, bool& _Verdict)
{
  int Status = ERROR;
  CommandLine* PCL = 0;
  unsigned int NA = 0;
  vector<string> Argument;

  for (unsigned int i = 0; i < NoCL; i++)
  {
    // Select a command line
    GetCommandLine(i, PCL);

    // Get the number of arguments in this command line
    GetNumberofCommandLineArguments(i, NA);

    // cout << endl << "i = "<<i<<endl;

    vector<bool> Matches;

    for (unsigned int m = 0; m < _Values->size(); m++)
    {
      Matches.push_back(false);
    }

    // cout << "NA = "<< NA <<endl;

    for (unsigned int j = 0; j < NA; j++)
    {
      // cout << "j = "<<j<<endl;

      // Select an argument
      PCL->GetArgument(j, Argument);

      // Select the values in this argument
      for (unsigned int k = 0; k < Argument.size(); k++)
      {
        // cout << "k = "<<k<<endl;

        // Select a value at the input list
        for (unsigned int l = 0; l < _Values->size(); l++)
        {
          // cout << "_Values-at("<<l<<") = "<<_Values->at(l) << "         Argument->at("<<k<<") = "<<Argument.at(k)<<endl;

          // Check whether this value exists on the selected argument
          if (Argument.at(k) == _Values->at(l))
          {
            Matches[l] = true;

            // cout << "Match!"<<endl;
          }
          else
          {
            // cout << "Do not match."<<endl;
          }
        }
      }

      Argument.clear();
    }

    _Verdict = true;

    for (unsigned int m = 0; m < _Values->size(); m++)
    {
      // cout << "Matches["<<m<<"] = "<<Matches[m]<<endl;

      _Verdict = _Verdict && Matches[m];
    }

    // cout << "The Verdict was (0 = false, 1 = true) = "<<_Verdict<<endl;

    if (_Verdict == true)
    {
      break;
    }
  }

  return Status;
}

// Convert the entire message object to an ostream
// Converte o objeto mensagem inteiro para um ostream
ostream& operator<<(ostream& os, Message& M)
{
  for (unsigned int i = 0; i < M.NoCL; i++)
  {
    os << *M.CommandLines[i];
  }

  if (M.HasPayloadFlag == true && M.PayloadSize > 0)
  {
    os << endl
       << "There is a payload of " << M.PayloadSize << " bytes" << endl;

    // os<<endl;

    // for (long long j=0;j<M.PayloadSize;j++)
    //	{
    //		os<<M.Payload[j];
    //	}
  }

  return os;
}

// Convert the entire message object to a file
// Converte o objeto mensagem inteiro para um arquivo
fstream& operator<<(fstream& fs, Message& M)
{

  char* buffer;
  long length = 0;

  for (unsigned int i = 0; i < M.NoCL; i++)
  {
    fs << *M.CommandLines[i];
  }

  if (M.HasPayloadFlag == 1)
  {
    fs << endl; // Add a blank line to separate the payload from the header.

    M.PayloadFile.OpenInputFile();

    // Read the number of characters in the Payload file.
    length = M.PayloadFile.tellg();

    // Change the position to read from the beginning
    M.PayloadFile.seekg(0);

    if (length > 0)
    {
      buffer = new char[length];

      M.PayloadFile.read(buffer, length);

      fs.write(buffer, length);

      delete[] buffer;
    }

    M.PayloadFile.CloseFile();
  }

  return fs;
}

// Convert an stringstream to an objext
// Converte um stringstream da mensagem para um objeto
stringstream& operator>>(stringstream& ss, Message& M)
{
  unsigned int i = 0;
  CommandLine* PCL = 0;
  string Option;
  string ng = "ng";
  long long int Position = 0;
  long int Size = 0;
  int length = 0;
  char* buffer;

  ss.seekg(0);

  char Line[4096];

  while (ss.getline(Line, sizeof(Line), '\n'))
  {
    if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
    {
      istringstream ins(Line);

      M.NewCommandLine(PCL);

      ins >> *M.CommandLines[i];

      i++;
    }
    else
    {
      break;
    }
  }

  // Stores current position
  Position = ss.tellg();

  // Get the total size
  Size = ss.str().length();

  // cout << "Size = " << Size << endl;

  // Calculates the length of the payload portion
  length = Size - Position;

  // cout << "length = " << length << endl;

  // cout << "Reading from position = " << Position << endl;

  if (length > 1 && Position > 0 && Size > 0)
  {
    // There is a payload
    M.HasPayloadFlag = true;

    // Open the output file
    M.PayloadFile.OverwriteOutputFile();

    // Change the position to write at the begining
    M.PayloadFile.seekp(0, ios::beg);

    buffer = new char[length];

    ss.read(buffer, length);

    M.PayloadFile.write(buffer, length);

    M.PayloadFile.CloseFile();

    delete[] buffer;
  }

  return ss;
}

// Convert the message file to an objext
// Converte o arquivo da mensagem para um objeto
fstream& operator>>(fstream& fs, Message& M)
{
  unsigned int i = 0;
  CommandLine* PCL = 0;
  string Option;
  char* buffer;
  int length = 0;
  long long int Position = 0;
  long int Size = 0;
  string ng = "ng";

  fs.seekg(0);

  char Line[4096];

  while (fs.getline(Line, sizeof(Line), '\n'))
  {
    if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
    {
      istringstream ins(Line);

      M.NewCommandLine(PCL);

      ins >> *M.CommandLines[i];

      i++;
    }
    else
    {
      break;
    }
  }

  // Stores current position
  Position = fs.tellg();

  // Change position to the end
  fs.seekg(ios::cur, ios::end);

  // Get the total size
  Size = fs.tellg();

  // Calculates the length of the payload portion
  length = Size - Position;

  // Returns the reading pointer to the correct position
  fs.seekg(Position, ios::beg);

  // cout << "Reading from position = " << Position << endl;

  if (length > 0 && Position > 0 && Size > 0)
  {
    // There is a payload
    M.HasPayloadFlag = true;

    // Open the output file
    M.PayloadFile.OverwriteOutputFile();

    // Change the position to write at the beginning
    M.PayloadFile.seekp(0, ios::beg);

    buffer = new char[length];

    fs.read(buffer, length);

    M.PayloadFile.write(buffer, length);

    M.PayloadFile.CloseFile();

    delete[] buffer;
  }

  return fs;
}
```

## FILE: baseline@1af604d:Common/src/Message.h

Role: Exact source bytes from the pinned functional baseline; read-only design context.
Bytes: 15486
SHA-256: bdbbfee17af8512b2c64e9b61cf83e73b5293cffcd7067c0a10787c80bac4e70

```text
/*
        NovaGenesis

        Name:		Message
        Object:		Message
        File:		Message.h
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.1

        Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _MESSAGE_H
#define _MESSAGE_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _FSTREAM_H
#include <fstream>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _COMMANDLINE_H
#include "CommandLine.h"
#endif

#ifndef _COMMANDLINE1_H
#include "CommandLine.h"
#endif

#ifndef _FILE_H
#include "File.h"
#endif

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#define ERROR 1
#define OK 0

#define DELETED_BY_APP true
#define DELETED_BY_CORE false

using namespace tthread;

class Message
{
private:
  // Tag to allow deleting
  bool Delete;

public:
  // ******************************************************
  // Attributes
  // ******************************************************

  // Self-certifying name
  string SCN;

  // Type
  short Type;

  // CommandLine container
  CommandLine** CommandLines;

  // Number of command lines
  unsigned int NoCL;

  // Header file handler. Allows to handle a file with the message command lines
  // File 					HeaderFile;

  // Payload file handler. Allows to handle a file with the message payload
  File PayloadFile;

  // Message file handler. Allows to handle a file with the entire message
  File MessageFile;

  // Has payload control flag. True means that the message has a payload.
  bool HasPayloadFlag;

  // Header char array. Can be used to carry a header to memory instead of using a file
  // char 					*Header;

  // Payload char array. Can be used to carry a payload in memory instead of saving it to a file
  char* Payload;

  // Message char array. Can be used to carry the message to memory instead of using a file
  char* Msg;

  // The size of the header in bytes
  // long					HeaderSize;

  // The size of the payload in bytes
  long long PayloadSize;

  // The size of the message in bytes
  long long MessageSize;

  // Time for priority queue comparison
  double Time;

  // Timestamp for performance analysis
  double TimeStamp;

  // Object creation time for performance analysis
  double InstantiationTime;

  // Tag for message tie on priority queue ordering
  unsigned int Tag;

  // Auxiliary flag to enable deletion of *Header array.
  // bool 					DeleteHeaderArray;

  // Auxiliary flag to enable deletion of *Payload array.
  bool DeletePayloadArray;

  // Auxiliary flag to enable deletion of *Message array.
  bool DeleteMessageArray;

  // Instantiation number to check for double deletion
  unsigned int InstantiationNumber;

  // An application avoid deletion of message by turning this flag true. False means that the core will delete the message automatically
  bool ApplicationDeleted;

  // ******************************************************
  // Constructor/destructor
  // ******************************************************

  // Empty message constructor.
  Message(double _Time, short _Type, bool _HasPayload);

  // Simple constructor.
  Message(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path);

  // Complete constructor.
  Message(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path);

  // Copy constructor
  Message(const Message& M);

  // Destructor
  ~Message();

  // ******************************************************
  // Basic set/get functions
  // ******************************************************

  void
  SetMessage(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path);

  // Set message's SCN
  void SetSelfCertifyingName(string _SCN);

  // Set message's type
  void SetType(short _Type);

  // Set has payload flag
  void SetHasPayloadFlag(bool _Flag);

  // Set time
  void SetTime(double _Time);

  // Set tag
  void SetTag(unsigned int _Tag);

  // Set header file name
  // void SetHeaderFileName(string _Name);

  // Set payload file name
  void SetPayloadFileName(string _Name);

  // Set payload file name
  void SetMessageFileName(string _Name);

  // Set header file name
  // string GetHeaderFileName();

  // Set payload file name
  string GetPayloadFileName();

  // Set payload file name
  string GetMessageFileName();

  // Set header file path
  // void SetHeaderFilePath(string _Path);

  // Set payload file path
  void SetPayloadFilePath(string _Path);

  // Set message file path
  void SetMessageFilePath(string _Path);

  // Set message to be deleted
  void MarkToDelete();

  // Set message to be deleted
  void UnmarkToDelete();

  // Set application driven deletion
  void SetApplicationDeletedFlag(bool _Flag);

  // Set header file path
  // string GetHeaderFilePath();

  // Set payload file path
  string GetPayloadFilePath();

  // Set message file path
  string GetMessageFilePath();

  // Set header file option
  // void SetHeaderFileOption(string _Option);

  // Set payload file option
  void SetPayloadFileOption(string _Option);

  // Set message file option
  // void SetMessageFileOption(string _Option);

  // Set header file option
  // string GetHeaderFileOption();

  // Set payload file option
  string GetPayloadFileOption();

  // Set message file option
  // string GetMessageFileOption();

  // Get has payload flag
  bool GetHasPayloadFlag() const;

  // Get message's SCN
  string GetSelfCertifyingName() const;

  // Get message's type
  short GetType() const;

  // Get time
  double GetTime() const;

  // Get tag
  unsigned int GetTag() const;

  // Get application delete flag status
  bool GetDeleteFlag();

  // Get application delete flag status
  bool GetApplicationDeletedFlag();

  // Allocate an empty command line and add it on CommandLines container
  int NewCommandLine(CommandLine*& CL);

  // Allocate a copy command line and add it on CommandLines container
  int NewCommandLine(CommandLine* _PCL, CommandLine*& CL);

  // Allocate and add a CommandLine on CommandLines container
  int NewCommandLine(string _Name, string _Alternative, string _Version, CommandLine*& C);

  // Get a CommandLine by its index
  int GetCommandLine(unsigned int _Index, CommandLine*& CL);

  // Get a CommandLine by its name and alternative
  int GetCommandLine(string _Name, string _Alternative, CommandLine*& CL);

  // Get a CommandLine Index
  int GetCommandLineIndex(CommandLine* CL, unsigned int& _Index);

  // Get number of command lines
  int GetNumberofCommandLines(unsigned int& _Number);

  // Get number of arguments
  int GetNumberofCommandLineArguments(unsigned int _CommandLine, unsigned int& _Number);

  // Get number of elements in a certain argument
  int GetNumberofCommandLineArgumentElements(unsigned int _CommandLine, unsigned int _Argument, unsigned int& _Number);

  // Set an element at an Argument
  int SetCommandLineArgumentElement(unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string _Value);

  // Get an element at an Argument
  int GetCommandLineArgumentElement(unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string& _Value);

  // Get header file
  // int GetHeaderFile(File& F);

  // Get payload file
  int GetPayloadFile(File& F);

  // Get message file
  int GetMessageFile(File& F);

  // Set *Header from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
  // int SetHeaderFromCharArray(char* _Value, long _Size);

  // Set *Payload from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
  int SetPayloadFromCharArray(char* _Value, long long _Size);

  // SPEC-018: Reset payload state so a new payload can be loaded on a reused Message.
  // Frees the existing Payload array and resets PayloadSize/DeletePayloadArray/HasPayloadFlag.
  void ResetPayload();

  // Set *Msg from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
  int SetMessageFromCharArray(char* _Value, long long _Size);

  // Get *Header from char array. Don't delete[] the array pointer
  // int GetHeaderFromCharArray(char*& _Value);

  // Get *Payload from char array. Don't delete[] the array pointer
  int GetPayloadFromCharArray(char*& _Value);

  // Get *Msg from char array. Don't delete[] the array pointer
  int GetMessageFromCharArray(char*& _Value);

  // Get *Msg from char array. Don't delete[] the array pointer
  int GetMessageFromCharArray(unsigned char*& _Value);

  // Return the current system time in seconds
  double GetSystemTime();

  // Get the object instantiation time in seconds
  double GetInstantiationTime();

  // ******************************************************
  // Header format convertions
  // ******************************************************

  // Convert the Header from file to a char array pointed by *Header. The HeaderFile handler needs to be properly configured.
  // int ConvertHeaderFromFileToCharArray();

  // Convert the Header from CommandLines to a char array pointed by *Header. The CommandLines need to be properly configured.
  int ConvertHeaderFromCommandLinesToCharArray();

  // Convert the Header from char array to CommandLines. The char array pointed by *Header needs to be properly configured.
  // int ConvertHeaderFromCharArrayToCommandLines();

  // Convert the Header from file to CommandLines. The HeaderFile handler needs to be properly configured.
  int ConvertHeaderFromFileToCommandLines();

  // Convert the Header from char array to file. The char array pointed by *Header needs to be properly configured as well as the HeaderFile handler
  // int ConvertHeaderFromCharArrayToFile();

  // Convert the Header from CommandLines to file. The CommandLines need to be properly configured as well as the HeaderFile handler
  // int ConvertHeaderFromCommandLinesToFile();

  // ******************************************************
  // Payload format convertions
  // ******************************************************

  // Convert the Payload from file to a char array pointed by *Payload. The PayloadFile handler needs to be properly configured.
  int ConvertPayloadFromFileToCharArray();

  // Convert the Payload from char array to file. The char array pointed by *Payload needs to be properly configured as well as the PayloadFile handler
  int ConvertPayloadFromCharArrayToFile();

  // Extract Payload char array from message char array. The Msg char array needs to be properly configured.
  int ExtractPayloadCharArrayFromMessageCharArray();

  // ******************************************************
  // Entire message format convertions
  // ******************************************************

  // Convert the Message from file to CommandLines and Payload file. The MessageFile handler needs to be properly configured
  // int ConvertMessageFromFileToCommandLinesandPayloadFile();

  // Convert the Message from file to a char array pointed by *Msg. The MessageFile handler needs to be properly configured.
  int ConvertMessageFromFileToCharArray();

  // Convert the Message from char array to a complete Message file. The char array pointed by *Message needs to be properly configured as well as the MessageFile handler
  // int ConvertMessageFromCharArrayToFile();

  // Convert the Message from CommandLines and Payload file to a complete Message file. The CommandLines need to be properly configured as well as the MessageFile handler
  // int ConvertMessageFromCommandLinesandPayloadFileToFile();

  // Convert the Message from CommandLines and Payload file to a char array. The CommandLines need to be properly configured as well as the PayloadFile handler
  int ConvertMessageFromCommandLinesandPayloadFileToCharArray();

  // Convert the Message from CommandLines and Payload char array to a message char array. The CommandLines need to be properly configured as well as the Payload char array
  int ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray();

  // Convert the complete message char array into command lines and payload file (if any was set up in advance)
  // int ConvertMessageFromCharArrayToCommandLinesandPayloadFile();

  // Convert the complete message char array into command lines and payload char array
  int ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray();

  // Convert the complete message char array into command lines and payload char array. Added in May 2015
  int ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray(File* _PF);

  // Convert the complete message char array into command lines and payload char array. Rebuild in May 2015
  int ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();

  // ******************************************************
  // Message size functions
  // ******************************************************

  // Get header size in bytes. A previous conversion from file or CommandLines to *Header char array is required
  // int GetHeaderSize(long& _Size);

  // Get payload size in bytes. A previous conversion from file to *Payload char array is required
  int GetPayloadSize(long long& _Size);

  // Get message size in bytes. A previous conversion from file and/or CommandLines to *Message char array is required
  int GetMessageSize(long long& _Size);

  // ******************************************************
  // Checking for command lines
  // ******************************************************
  int DoesThisCommandLineExistsInMessage(string _Name, string _Alternative, bool& _Verdict);

  // ******************************************************
  // Checking for arguments
  // ******************************************************

  // Check if the values on the vector exist on some command line. They all need to exist in the same command line
  int DoAllTheseValuesExistInSomeCommandLine(vector<string>* _Values, bool& _Verdict);

  // ******************************************************
  // Operators overloading
  // ******************************************************

  // Overloading ostream operator
  friend ostream& operator<<(ostream& os, Message& M);

  // Overloading fstream operator
  friend fstream& operator<<(fstream& fs, Message& M);

  // Overloading stringstream operator
  friend stringstream& operator>>(stringstream& fs, Message& M);

  // Overloading fstream operator
  friend fstream& operator>>(fstream& fs, Message& M);
};

#endif
```

## FILE: baseline@1af604d:Common/src/Block.cpp

Role: Exact source bytes from the pinned functional baseline; read-only design context.
Bytes: 12288
SHA-256: 26dfa06b575cea6bb9c8d21a18b894f37e73ad91c7e7a7450fefe517903e1622

```text
/*
        NovaGenesis

        Name:		Block
        Object:		Block
        File:		Block.cpp
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.1

        Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _MURMURHASH3_H_
#include "MurmurHash3.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

// #define DEBUG // To follow message processing
// #define DEBUG1 // To follow inlineresponse message generation only
// #define DEBUG2

Block::Block(string _LN, Process* _PP, unsigned int _Index, string _Path)
{
  LN = _LN;
  Index = _Index;
  SCN = "";
  PP = _PP;
  StopProcessingMessage = false;
  Path = _Path;
  string Offset = "          ";

  // Generate the required prime numbers
  unsigned int n_primes = 0;

  for (unsigned int z = 2; n_primes < 32; z++)
  {
    for (unsigned int w = 0; w < n_primes; w++)
    {
      if (z % primes[w] == 0)
      {
        goto not_prime;
      }
    }

    primes[n_primes] = z;

    n_primes++;

  not_prime:;
  }

  SCN = NameGenerator::GetInstance().GenerateFromString(LN);

  cout << Offset << "(" << LN << " block has the SCN = " << SCN << ")" << endl;
}

Block::~Block()
{
}

// Set block legible name
void Block::SetLegibleName(string _LN)
{
  LN = _LN;
}

// Set block self-certifying name
void Block::SetSelfCertifyingName(string _SCN)
{
  SCN = _SCN;
}

// Set working path
void Block::SetPath(string _Path)
{
  Path = _Path;
}

// Set operational state
void Block::SetState(string _State)
{
  State = _State;
}

// Get block legible name
string Block::GetLegibleName()
{
  return LN;
}

// Get block self-certifying name
string Block::GetSelfCertifyingName()
{
  return SCN;
}

// Get working path
string Block::GetPath()
{
  return Path;
}

// Get operational state
string Block::GetState()
{
  return State;
}

// Get index
unsigned int Block::GetIndex()
{
  return Index;
}

// Run the actions behind a received message, generating a resulting message
int Block::Run(Message* _ReceivedMessage, Message*& _InlineResponseMessage)
{
  int Status = ERROR;           // Overall execution status
  vector<int> CLStatus;         // Status of every command line action
  unsigned int NCL = 0;         // Number of command lines
  CommandLine* PCL = 0;         // Pointer to the command line being executed
  Action* PA1 = 0;              // Pointer to the action that is performing such execution
  Action* PA2 = 0;              // Pointer to the action that is performing such execution
  ActionsIterator it;           // Iterator to find out a value
  unsigned int i = 0;           // Command lines counter
  string Offset = "          "; // Auxiliary variable for logging

  if (LN != "GW")
  {
    Offset = "                    ";
  }

#ifdef DEBUG

  S << endl
    << endl;

  S << Offset << "*************************************************************************************************" << endl;
  S << Offset << "   Block = " << LN << ", State = " << State << ", SCN = " << SCN << endl;
  S << Offset << "*************************************************************************************************" << endl;

  S << Offset << "*****************************************" << endl;
  S << Offset << "   Block = " << LN << ", State = " << State << endl;
  S << Offset << "*****************************************" << endl;

#endif

  // ***********************************************************************
  // Call the corresponding actions to perform the processing
  // ***********************************************************************

  if (PP->OkToRun(_ReceivedMessage) == true)
  {
    // SPEC-003: Moved DEBUGX and message dump AFTER OkToRun check.
    // Previously, these accessed _ReceivedMessage before verifying it
    // is still in Process::Messages[], causing use-after-free SIGSEGV
    // when a freed message pointer was dequeued from the InputQueue.
#ifdef DEBUG2
    {
      unsigned int _dbgNCL = 0;
      _ReceivedMessage->GetNumberofCommandLines(_dbgNCL);
      S << Offset << "(DEBUGX: _RcvMsg=" << _ReceivedMessage
        << " NoCL=" << _dbgNCL
        << " HasPayload=" << _ReceivedMessage->HasPayloadFlag
        << " PayloadSize=" << _ReceivedMessage->PayloadSize
        << ")" << endl;
      for (unsigned int _dbgi = 0; _dbgi < _dbgNCL && _dbgi < 10; _dbgi++)
      {
        CommandLine* _dbgCL = 0;
        if (_ReceivedMessage->GetCommandLine(_dbgi, _dbgCL) == OK && _dbgCL != 0)
        {
          S << Offset << "(DEBUGX: CL[" << _dbgi << "] ptr=" << _dbgCL
            << " Name=\"" << _dbgCL->Name
            << "\" Alt=\"" << _dbgCL->Alternative
            << "\" Ver=\"" << _dbgCL->Version
            << "\" NoA=" << _dbgCL->NoA
            << " Arguments=" << _dbgCL->Arguments
            << ")" << endl;
        }
        else
        {
          S << Offset << "(DEBUGX: CL[" << _dbgi << "] = NULL)" << endl;
        }
      }
    }

    S << endl
      << "(" << endl
      << *_ReceivedMessage << ")" << endl;
#endif

    if (_ReceivedMessage->GetNumberofCommandLines(NCL) == OK)
    {
      if (NCL != 0)
      {
#ifdef DEBUG
        S << endl
          << Offset << "(NCL = " << NCL << ")" << endl;
#endif

        CLStatus.resize(NCL, ERROR);

        // Vector of indexes to the block Messages container
        vector<Message*> ScheduledMessages;

        while (i < NCL && StopProcessingMessage == false)
        {
          if (_ReceivedMessage->GetCommandLine(i, PCL) == OK)
          {
            if (PCL != 0)
            {
              if (PCL->Name != "" && PCL->Alternative != "")
              {
                char C1 = PCL->Name[0];
                char C2 = PCL->Alternative[0];
                char C3 = PCL->Alternative[1];

                if (C1 == '-' && C2 == '-' && C3 == '-')
                {
                  // Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
                  string LN1(PCL->Name + " " + PCL->Alternative + " " + PCL->Version);

                  // Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
                  string LN2(PCL->Name + " " + PCL->Alternative + " 0.1");

#ifdef DEBUG

                  S << Offset << "(LN1 = " << LN1 << ", LN2 = " << LN2 << ".)" << endl;
#endif

                  for (unsigned int j = 0; j < Actions.size(); j++)
                  {
                    if (Actions[j]->GetLegibleName() == LN1)
                    {
                      PA1 = Actions[j];

#ifdef DEBUG
                      S << Offset << "(Found)" << endl;
#endif

                      break;
                    }
                  }

                  if (PA1 != 0)
                  {
#ifdef DEBUG
                    S << Offset << "(Action is " << PA1->GetLegibleName() << ")" << endl;
#endif

                    // Call the action
                    CLStatus[i] = PA1->Run(_ReceivedMessage, PCL, ScheduledMessages, _InlineResponseMessage);

                    PA1 = 0;

                    PA2 = 0;
                  }
                  else
                  {

#ifdef DEBUG
                    S << Offset << "(Warning: " << LN1 << " not implemented.)" << endl;
#endif

                    for (unsigned int j = 0; j < Actions.size(); j++)
                    {
                      if (Actions[j]->GetLegibleName() == LN2)
                      {
                        PA2 = Actions[j];

                        break;
                      }
                    }
                  }

                  if (PA1 == 0 && PA2 != 0)
                  {
#ifdef DEBUG
                    S << Offset << "(Action is " << PA2->GetLegibleName() << ")" << endl;
#endif

                    // Call the action
                    CLStatus[i] = PA2->Run(_ReceivedMessage, PCL, ScheduledMessages, _InlineResponseMessage);

                    PA2 = 0;
                  }
                }
                else
                {
                  S << Offset << "(ERROR: Corrupted command line (2)!)" << endl;

                  CLStatus[i] = ERROR;

                  StopProcessingMessage = true;

                  _ReceivedMessage->MarkToDelete();

                  break;
                }
              }
              else
              {
                S << Offset << "(ERROR: Corrupted command line (1)!)" << endl;
              }
            }
            else
            {
              S << "(ERROR: Invalid command line)" << endl;

              CLStatus[i] = ERROR;
            }
          }
          else
          {
            S << "(ERROR: Unable to get the command line)" << endl;

            CLStatus[i] = ERROR;
          }

          i++;
        }

        // Set overall status
        for (unsigned int j = 0; j < NCL; j++)
        {
          if (CLStatus[j] == ERROR)
          {
            Status = ERROR;
          }
        }

        // SPEC-003: Removed ScheduledMessages cleanup loop.
        // With the GWStatusS01Msg dead code eliminated from GWMsgCl01,
        // no action pushes to ScheduledMessages anymore. The vector is
        // always empty, making the cleanup loop unnecessary.

#ifdef DEBUG1

        // SPEC-DEBUG-HT: Log the InlineResponseMessage after processing ALL command lines
        // so we can see the accumulated -d --b and -info --payload CLs with a single payload.
        if (LN == "HT" && _InlineResponseMessage != 0)
        {
          unsigned int _dbgIRM_NCL = 0;
          _InlineResponseMessage->GetNumberofCommandLines(_dbgIRM_NCL);
          S << Offset << "(SPEC-DEBUG-HT: InlineResponseMessage after processing "
            << NCL << " CL(s) — total CLs = " << _dbgIRM_NCL
            << ", HasPayload = " << _InlineResponseMessage->HasPayloadFlag
            << ", PayloadSize = " << _InlineResponseMessage->PayloadSize
            << ")" << endl;
          S << Offset << "(BEGIN InlineResponseMessage at "<<GetTime()<<" seconds)" << endl;
          S << *_InlineResponseMessage;
          S << Offset << "(END InlineResponseMessage)" << endl;
        }
#endif

        StopProcessingMessage = false;
      }
      else
      {
        S << "(ERROR: Invalid number of command lines)" << endl;

        Status = ERROR;
      }
    }
    else
    {
      S << "(ERROR: Unable to get the number of command lines)" << endl;

      Status = ERROR;
    }
  }
  else
  {
    S << "(ERROR: Unable to run a message that is not at the Messages container)" << endl;
  }

  return Status;
}

// Allocate and add an Action on Actions container
void Block::NewAction(const string _LN, Action*& _PA)
{
}

// Get an Action
int Block::GetAction(string _LN, Action*& _PA)
{
  return ERROR;
}

// Delete an Action
int Block::DeleteAction(string _LN)
{
  return ERROR;
}

// Auxiliary functions
int Block::StringToInt(string _String)
{
  stringstream ss(_String);

  int Temp = 0;

  ss >> Temp;

  return Temp;
}

string Block::IntToString(int _Int)
{
  stringstream ss;

  ss << _Int;

  return ss.str();
}

string Block::DoubleToString(double _Double)
{
  stringstream ss;

  ss << _Double;

  return ss.str();
}

// Converting string to double
double Block::StringToDouble(string _String)
{
  return atof(_String.c_str());
}

string Block::UnsignedIntToString(unsigned int _Int)
{
  string ret;
  stringstream ss;

  ss << _Int;

  ret = ss.str();

  return ret;
}

double Block::GetTime()
{
  struct timespec t;

  clock_gettime(CLOCK_MONOTONIC, &t);

  return ((t.tv_sec) + (double)(t.tv_nsec / 1e9));
}
```

## FILE: baseline@1af604d:CMakeLists.txt

Role: Exact baseline build graph before the D1-H overlay.
Bytes: 16520
SHA-256: 5f9cd0676b2dcdcd0a500d778f5d5baea312301d6abc382db5eab95f2bbb1d07

```text
cmake_minimum_required(VERSION 3.5.1)
project(novagenesis)

set(CMAKE_CXX_STANDARD 20)

option(NG_ENABLE_LEGACY_STANDALONE
       "Build deprecated standalone PSS, GIRS and HTS services"
       OFF)

if(NG_ENABLE_LEGACY_STANDALONE)
    message(WARNING
            "NG_ENABLE_LEGACY_STANDALONE=ON: PSS, GIRS and HTS are deprecated; use NRNCS")
endif()

include_directories(ContentApp/src)
include_directories(Common/src)
include_directories(Common/src/rapidjson)
include_directories(Common/src/rapidjson/error)
include_directories(Common/src/rapidjson/internal)
include_directories(IoTTestApp/src)
include_directories(NBTestApp/src)
include_directories(PGCS/src)
include_directories(NRNCS/src)

add_library(Common
        Common/src/rapidjson/error/en.h
        Common/src/rapidjson/error/error.h
        Common/src/rapidjson/internal/biginteger.h
        Common/src/rapidjson/internal/diyfp.h
        Common/src/rapidjson/internal/dtoa.h
        Common/src/rapidjson/internal/ieee754.h
        Common/src/rapidjson/internal/itoa.h
        Common/src/rapidjson/internal/meta.h
        Common/src/rapidjson/internal/pow10.h
        Common/src/rapidjson/internal/stack.h
        Common/src/rapidjson/internal/strfunc.h
        Common/src/rapidjson/internal/strtod.h
        Common/src/rapidjson/internal/swap.h
        Common/src/rapidjson/msinttypes/inttypes.h
        Common/src/rapidjson/msinttypes/stdint.h
        Common/src/rapidjson/allocators.h
        Common/src/rapidjson/document.h
        Common/src/rapidjson/encodedstream.h
        Common/src/rapidjson/encodings.h
        Common/src/rapidjson/filereadstream.h
        Common/src/rapidjson/filewritestream.h
        Common/src/rapidjson/memorybuffer.h
        Common/src/rapidjson/memorystream.h
        Common/src/rapidjson/pointer.h
        Common/src/rapidjson/prettywriter.h
        Common/src/rapidjson/rapidjson.h
        Common/src/rapidjson/reader.h
        Common/src/rapidjson/stringbuffer.h
        Common/src/rapidjson/writer.h
        Common/src/Action.cpp
        Common/src/Action.h
        Common/src/Block.cpp
        Common/src/Block.h
        Common/src/CLI.cpp
        Common/src/CLI.h
        Common/src/CLIRunInitialization01.cpp
        Common/src/CLIRunInitialization01.h
        Common/src/CLISCNAck01.cpp
        Common/src/CLISCNAck01.h
        Common/src/CommandLine.cpp
        Common/src/CommandLine.h
        Common/src/DataStore.h
        Common/src/fast_mutex.h
        Common/src/File.cpp
        Common/src/File.h
        Common/src/GW.cpp
        Common/src/GW.h
        Common/src/GWHelloIPC02.cpp
        Common/src/GWHelloIPC02.h
        Common/src/GWMsgCl01.cpp
        Common/src/GWMsgCl01.h
        Common/src/GWRunInitialization01.cpp
        Common/src/GWRunInitialization01.h
        Common/src/GWRunHelloIPC02.cpp
        Common/src/GWRunHelloIPC02.h
        Common/src/GWExposition02.cpp
        Common/src/GWExposition02.h
        Common/src/HashMultimaps.h
        Common/src/HT.cpp
        Common/src/HT.h
        Common/src/HTBindReport01.cpp
        Common/src/HTBindReport01.h
        Common/src/HTDeliveryBind01.cpp
        Common/src/HTDeliveryBind01.h
        Common/src/HTGetBind01.cpp
        Common/src/HTGetBind01.h
        Common/src/HTInfoPayload01.cpp
        Common/src/HTInfoPayload01.h
        Common/src/HTListBind01.cpp
        Common/src/HTListBind01.h
        Common/src/HTMessageSeq01.cpp
        Common/src/HTMessageSeq01.h
        Common/src/HTMsgCl01.cpp
        Common/src/HTMsgCl01.h
        Common/src/HTRevokeBind01.cpp
        Common/src/HTRevokeBind01.h
        Common/src/HTRunInitialization01.cpp
        Common/src/HTRunInitialization01.h
        Common/src/HTRunPeriodic01.cpp
        Common/src/HTRunPeriodic01.h
        Common/src/HTSCNSeq01.cpp
        Common/src/HTSCNSeq01.h
        Common/src/HTStatusS01.cpp
        Common/src/HTStatusS01.h
        Common/src/HTStoreBind01.cpp
        Common/src/HTStoreBind01.h
        Common/src/Message.cpp
        Common/src/Message.h
        Common/src/MessageBuilder.cpp
        Common/src/MessageBuilder.h
        Common/src/MessageReceiving.h
        Common/src/MurmurHash3.cpp
        Common/src/MurmurHash3.h
        Common/src/MurmurHash3Strategy.cpp
        Common/src/MurmurHash3Strategy.h
        Common/src/NameGenerator.cpp
        Common/src/NameGenerator.h
        Common/src/NGAL_SAR.cpp
        Common/src/NGAL_SAR.h
        Common/src/NGAL_CS.cpp
        Common/src/NGAL_CS.h
        Common/src/NameStrategy.h
        Common/src/DefaultNameStrategy.cpp
        Common/src/DefaultNameStrategy.h
        Common/src/HashFunction.h
        Common/src/OutputVariable.cpp
        Common/src/OutputVariable.h
        Common/src/Process.cpp
        Common/src/Process.h
        Common/src/Prompt_iostream.cpp
        Common/src/Prompt_iostream.h
        Common/src/Publication.h
        Common/src/Shm.h
        Common/src/Subscription.h
        Common/src/tinythread.cpp
        Common/src/tinythread.h
        Common/src/Tuple.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++") # SPEC-MUSL-001: musl static link fix (cherry-pick e714ab1)

add_executable(ContentApp
        ContentApp/src/ContentApp.cpp
        ContentApp/src/ContentApp.h
        ContentApp/src/Core.cpp
        ContentApp/src/Core.h
        ContentApp/src/CoreDeliveryBind01.cpp
        ContentApp/src/CoreDeliveryBind01.h
        ContentApp/src/CoreInfoPayload01.cpp
        ContentApp/src/CoreInfoPayload01.h
        ContentApp/src/CoreMsgCl01.cpp
        ContentApp/src/CoreMsgCl01.h
        ContentApp/src/CoreNotifyS01.cpp
        ContentApp/src/CoreNotifyS01.h
        ContentApp/src/CoreRunDiscover01.cpp
        ContentApp/src/CoreRunDiscover01.h
        ContentApp/src/CoreRunEvaluate01.cpp
        ContentApp/src/CoreRunEvaluate01.h
        ContentApp/src/CoreRunExpose01.cpp
        ContentApp/src/CoreRunExpose01.h
        ContentApp/src/CoreRunInitialize01.cpp
        ContentApp/src/CoreRunInitialize01.h
        ContentApp/src/CoreRunInvite01.cpp
        ContentApp/src/CoreRunInvite01.h
        ContentApp/src/CoreRunPeriodic01.cpp
        ContentApp/src/CoreRunPeriodic01.h
        ContentApp/src/CoreRunContentPublish01.cpp
        ContentApp/src/CoreRunContentPublish01.h
        ContentApp/src/CoreRunPublish01.cpp
        ContentApp/src/CoreRunPublish01.h
        ContentApp/src/CoreRunPublish02.cpp
        ContentApp/src/CoreRunPublish02.h
        ContentApp/src/CoreRunPublish03.cpp
        ContentApp/src/CoreRunPublish03.h
        ContentApp/src/CoreRunSubscribe01.cpp
        ContentApp/src/CoreRunSubscribe01.h
        ContentApp/src/CoreSCNAck01.cpp
        ContentApp/src/CoreSCNAck01.h
        ContentApp/src/CoreSCNSeq01.cpp
        ContentApp/src/CoreSCNSeq01.h
        ContentApp/src/CoreStatusS01.cpp
        ContentApp/src/CoreStatusS01.h
        ContentApp/src/execContentApp.cpp)

target_link_libraries(ContentApp LINK_PRIVATE Common)

if(NG_ENABLE_LEGACY_STANDALONE)
add_executable(GIRS
        GIRS/src/execGIRS.cpp
        GIRS/src/GIRS.cpp
        GIRS/src/GIRS.h
        GIRS/src/IR.cpp
        GIRS/src/IR.h
        GIRS/src/IRDeliveryBind01.cpp
        GIRS/src/IRDeliveryBind01.h
        GIRS/src/IRGetBind01.cpp
        GIRS/src/IRGetBind01.h
        GIRS/src/IRGetBind02.cpp
        GIRS/src/IRGetBind02.h
        GIRS/src/IRInfoPayload01.cpp
        GIRS/src/IRInfoPayload01.h
        GIRS/src/IRInfoPayload02.cpp
        GIRS/src/IRInfoPayload02.h
        GIRS/src/IRMessageSeq01.cpp
        GIRS/src/IRMessageSeq01.h
        GIRS/src/IRMsgCl01.cpp
        GIRS/src/IRMsgCl01.h
        GIRS/src/IRMsgCl02.cpp
        GIRS/src/IRMsgCl02.h
        GIRS/src/IRRevokeBind01.cpp
        GIRS/src/IRRevokeBind01.h
        GIRS/src/IRRunInitialization01.cpp
        GIRS/src/IRRunInitialization01.h
        GIRS/src/IRRunPeriodic01.cpp
        GIRS/src/IRRunPeriodic01.h
        GIRS/src/IRSCNAck01.cpp
        GIRS/src/IRSCNAck01.h
        GIRS/src/IRSCNSeq01.cpp
        GIRS/src/IRSCNSeq01.h
        GIRS/src/IRSCNSeq02.cpp
        GIRS/src/IRSCNSeq02.h
        GIRS/src/IRStatusS01.cpp
        GIRS/src/IRStatusS01.h
        GIRS/src/IRStoreBind01.cpp
        GIRS/src/IRStoreBind01.h
        GIRS/src/IRStoreBind02.cpp
        GIRS/src/IRStoreBind02.h)

target_include_directories(GIRS PRIVATE GIRS/src)
target_link_libraries(GIRS LINK_PRIVATE Common)
endif()

if(NG_ENABLE_LEGACY_STANDALONE)
add_executable(HTS
        HTS/src/DHT.cpp
        HTS/src/DHT.h
        HTS/src/DHTDeliveryBind01.cpp
        HTS/src/DHTDeliveryBind01.h
        HTS/src/DHTMsgCl01.cpp
        HTS/src/DHTMsgCl01.h
        HTS/src/DHTRunInitialization01.cpp
        HTS/src/DHTRunInitialization01.h
        HTS/src/DHTRunPeriodic01.cpp
        HTS/src/DHTRunPeriodic01.h
        HTS/src/execHTS.cpp
        HTS/src/HTS.cpp
        HTS/src/HTS.h)

target_include_directories(HTS PRIVATE HTS/src)
target_link_libraries(HTS LINK_PRIVATE Common)
endif()

add_executable(IoTTestApp
        IoTTestApp/src/Core.cpp
        IoTTestApp/src/Core.h
        IoTTestApp/src/CoreDeliveryBind01.cpp
        IoTTestApp/src/CoreDeliveryBind01.h
        IoTTestApp/src/CoreInfoPayload01.cpp
        IoTTestApp/src/CoreInfoPayload01.h
        IoTTestApp/src/CoreMsgCl01.cpp
        IoTTestApp/src/CoreMsgCl01.h
        IoTTestApp/src/CoreNotifyS01.cpp
        IoTTestApp/src/CoreNotifyS01.h
        IoTTestApp/src/CoreRunDiscover01.cpp
        IoTTestApp/src/CoreRunDiscover01.h
        IoTTestApp/src/CoreRunEvaluate01.cpp
        IoTTestApp/src/CoreRunEvaluate01.h
        IoTTestApp/src/CoreRunExpose01.cpp
        IoTTestApp/src/CoreRunExpose01.h
        IoTTestApp/src/CoreRunInitialize01.cpp
        IoTTestApp/src/CoreRunInitialize01.h
        IoTTestApp/src/CoreRunInvite01.cpp
        IoTTestApp/src/CoreRunInvite01.h
        IoTTestApp/src/CoreRunPeriodic01.cpp
        IoTTestApp/src/CoreRunPeriodic01.h
        IoTTestApp/src/CoreRunPublish01.cpp
        IoTTestApp/src/CoreRunPublish01.h
        IoTTestApp/src/CoreRunPublish02.cpp
        IoTTestApp/src/CoreRunPublish02.h
        IoTTestApp/src/CoreRunPublish03.cpp
        IoTTestApp/src/CoreRunPublish03.h
        IoTTestApp/src/CoreRunSubscribe01.cpp
        IoTTestApp/src/CoreRunSubscribe01.h
        IoTTestApp/src/CoreSCNAck01.cpp
        IoTTestApp/src/CoreSCNAck01.h
        IoTTestApp/src/CoreSCNSeq01.cpp
        IoTTestApp/src/CoreSCNSeq01.h
        IoTTestApp/src/CoreStatusS01.cpp
        IoTTestApp/src/CoreStatusS01.h
        IoTTestApp/src/execIoTTestApp.cpp
        IoTTestApp/src/IoTTestApp.cpp
        IoTTestApp/src/IoTTestApp.h)

target_link_libraries(IoTTestApp LINK_PRIVATE Common)

add_executable(NBTestApp
        NBTestApp/src/NBTestApp.cpp
        NBTestApp/src/NBTestApp.h
        NBTestApp/src/Core.cpp
        NBTestApp/src/Core.h
        NBTestApp/src/CoreDeliveryBind01.cpp
        NBTestApp/src/CoreDeliveryBind01.h
        NBTestApp/src/CoreMsgCl01.cpp
        NBTestApp/src/CoreMsgCl01.h
        NBTestApp/src/CoreRunDiscover01.cpp
        NBTestApp/src/CoreRunDiscover01.h
        NBTestApp/src/CoreRunEvaluate01.cpp
        NBTestApp/src/CoreRunEvaluate01.h
        NBTestApp/src/CoreRunInitialize01.cpp
        NBTestApp/src/CoreRunInitialize01.h
        NBTestApp/src/CoreRunPeriodic01.cpp
        NBTestApp/src/CoreRunPeriodic01.h
        NBTestApp/src/CoreRunSubscribe01.cpp
        NBTestApp/src/CoreRunSubscribe01.h
        NBTestApp/src/CoreSCNAck01.cpp
        NBTestApp/src/CoreSCNAck01.h
        NBTestApp/src/CoreSCNSeq01.cpp
        NBTestApp/src/CoreSCNSeq01.h
        NBTestApp/src/CoreStatusS01.cpp
        NBTestApp/src/CoreStatusS01.h
        NBTestApp/src/execNBTestApp.cpp)

target_link_libraries(NBTestApp LINK_PRIVATE Common)

add_executable(PGCS
        PGCS/src/Core.cpp
        PGCS/src/Core.h
        PGCS/src/CoreDeliveryBind01.cpp
        PGCS/src/CoreDeliveryBind01.h
        PGCS/src/CoreInfoPayload01.cpp
        PGCS/src/CoreInfoPayload01.h
        PGCS/src/CoreMsgCl01.cpp
        PGCS/src/CoreMsgCl01.h
        PGCS/src/CoreNotifyS01.cpp
        PGCS/src/CoreNotifyS01.h
        PGCS/src/CoreRunDiscover01.cpp
        PGCS/src/CoreRunDiscover01.h
        PGCS/src/CoreRunEvaluate01.cpp
        PGCS/src/CoreRunEvaluate01.h
        PGCS/src/CoreRunExpose01.cpp
        PGCS/src/CoreRunExpose01.h
        PGCS/src/CoreRunInitialize01.cpp
        PGCS/src/CoreRunInitialize01.h
        PGCS/src/CoreRunInvite01.cpp
        PGCS/src/CoreRunInvite01.h
        PGCS/src/CoreRunPeriodic01.cpp
        PGCS/src/CoreRunPeriodic01.h
        PGCS/src/CoreRunPublish01.cpp
        PGCS/src/CoreRunPublish01.h
        PGCS/src/CoreRunPublish02.cpp
        PGCS/src/CoreRunPublish02.h
        PGCS/src/CoreRunPublish03.cpp
        PGCS/src/CoreRunPublish03.h
        PGCS/src/CoreRunSubscribe01.cpp
        PGCS/src/CoreRunSubscribe01.h
        PGCS/src/CoreSCNAck01.cpp
        PGCS/src/CoreSCNAck01.h
        PGCS/src/CoreSCNSeq01.cpp
        PGCS/src/CoreSCNSeq01.h
        PGCS/src/CoreStatusS01.cpp
        PGCS/src/CoreStatusS01.h
        PGCS/src/execPGCS.cpp
        PGCS/src/PG.cpp
        PGCS/src/PG.h
        PGCS/src/PGCS.cpp
        PGCS/src/PGCS.h
        PGCS/src/PGHelloIHC01.cpp
        PGCS/src/PGHelloIHC01.h
        PGCS/src/PGHelloIHC02.cpp
        PGCS/src/PGHelloIHC02.h
        PGCS/src/PGHelloIHC03.cpp
        PGCS/src/PGHelloIHC03.h
        PGCS/src/NGAL_Transport_RAW.cpp
        PGCS/src/NGAL_Transport_RAW.h
        PGCS/src/PGMsgCl01.cpp
        PGCS/src/PGMsgCl01.h
        PGCS/src/PGRunExposition01.cpp
        PGCS/src/PGRunExposition01.h
        PGCS/src/PGRunHello01.cpp
        PGCS/src/PGRunHello01.h
        PGCS/src/PGRunHello02.cpp
        PGCS/src/PGRunHello02.h
        PGCS/src/PGRunHello03.cpp
        PGCS/src/PGRunHello03.h
        PGCS/src/PGRunInitialization01.cpp
        PGCS/src/PGRunInitialization01.h
        PGCS/src/PGRunPeriodic01.cpp
        PGCS/src/PGRunPeriodic01.h
        PGCS/src/PGRunPublishing01.cpp
        PGCS/src/PGRunPublishing01.h
        PGCS/src/PGRunStresstest01.cpp
        PGCS/src/PGRunStresstest01.h
        PGCS/src/PGStresstestPing01.cpp
        PGCS/src/PGStresstestPing01.h)

target_link_libraries(PGCS LINK_PRIVATE Common)

if(NG_ENABLE_LEGACY_STANDALONE)
add_executable(PSS
        PSS/src/execPSS.cpp
        PSS/src/PS.cpp
        PSS/src/PS.h
        PSS/src/PSDeliveryBind01.cpp
        PSS/src/PSDeliveryBind01.h
        PSS/src/PSInfoPayload01.cpp
        PSS/src/PSInfoPayload01.h
        PSS/src/PSMessageSeq01.cpp
        PSS/src/PSMessageSeq01.h
        PSS/src/PSMsgCl01.cpp
        PSS/src/PSMsgCl01.h
        PSS/src/PSPubBind01.cpp
        PSS/src/PSPubBind01.h
        PSS/src/PSPubNotify01.cpp
        PSS/src/PSPubNotify01.h
        PSS/src/PSRevokeBind01.cpp
        PSS/src/PSRevokeBind01.h
        PSS/src/PSRunInitialization01.cpp
        PSS/src/PSRunInitialization01.h
        PSS/src/PSRunPeriodic01.cpp
        PSS/src/PSRunPeriodic01.h
        PSS/src/PSS.cpp
        PSS/src/PSS.h
        PSS/src/PSSCNAck01.cpp
        PSS/src/PSSCNAck01.h
        PSS/src/PSSCNSeq01.cpp
        PSS/src/PSSCNSeq01.h
        PSS/src/PSStatusS01.cpp
        PSS/src/PSStatusS01.h
        PSS/src/PSSubBind01.cpp
        PSS/src/PSSubBind01.h)

target_include_directories(PSS PRIVATE PSS/src)
target_link_libraries(PSS LINK_PRIVATE Common)
endif()

add_executable(NRNCS
        NRNCS/src/execNRNCS.cpp
        NRNCS/src/NR.cpp
        NRNCS/src/NR.h
        NRNCS/src/NRDeliveryBind01.cpp
        NRNCS/src/NRDeliveryBind01.h
        NRNCS/src/NRInfoPayload01.cpp
        NRNCS/src/NRInfoPayload01.h
        NRNCS/src/NRMessageSeq01.cpp
        NRNCS/src/NRMessageSeq01.h
        NRNCS/src/NRMsgCl01.cpp
        NRNCS/src/NRMsgCl01.h
        NRNCS/src/NRPubBind01.cpp
        NRNCS/src/NRPubBind01.h
        NRNCS/src/NRPubNotify01.cpp
        NRNCS/src/NRPubNotify01.h
        NRNCS/src/NRRevokeBind01.cpp
        NRNCS/src/NRRevokeBind01.h
        NRNCS/src/NRRunInitialization01.cpp
        NRNCS/src/NRRunInitialization01.h
        NRNCS/src/NRRunPeriodic01.cpp
        NRNCS/src/NRRunPeriodic01.h
        NRNCS/src/NRNCS.cpp
        NRNCS/src/NRNCS.h
        NRNCS/src/NRSCNSeq01.cpp
        NRNCS/src/NRSCNSeq01.h
        NRNCS/src/NRSubBind01.cpp
        NRNCS/src/NRSubBind01.h)

target_link_libraries(NRNCS LINK_PRIVATE Common)
```

## FILE: isolated-D1H-worktree/CMakeLists.txt

Role: Pre-existing modified overlay; not a clean baseline and not authorized for editing.
Bytes: 16856
SHA-256: d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d

```text
cmake_minimum_required(VERSION 3.5.1)
project(novagenesis)

set(CMAKE_CXX_STANDARD 20)

option(NG_ENABLE_LEGACY_STANDALONE
       "Build deprecated standalone PSS, GIRS and HTS services"
       OFF)

option(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS
       "Build NG-039 Gateway compile/link probe only (no runtime execution)"
       OFF)

if(NG_ENABLE_LEGACY_STANDALONE)
    message(WARNING
            "NG_ENABLE_LEGACY_STANDALONE=ON: PSS, GIRS and HTS are deprecated; use NRNCS")
endif()

include_directories(ContentApp/src)
include_directories(Common/src)
include_directories(Common/src/rapidjson)
include_directories(Common/src/rapidjson/error)
include_directories(Common/src/rapidjson/internal)
include_directories(IoTTestApp/src)
include_directories(NBTestApp/src)
include_directories(PGCS/src)
include_directories(NRNCS/src)

add_library(Common
        Common/src/rapidjson/error/en.h
        Common/src/rapidjson/error/error.h
        Common/src/rapidjson/internal/biginteger.h
        Common/src/rapidjson/internal/diyfp.h
        Common/src/rapidjson/internal/dtoa.h
        Common/src/rapidjson/internal/ieee754.h
        Common/src/rapidjson/internal/itoa.h
        Common/src/rapidjson/internal/meta.h
        Common/src/rapidjson/internal/pow10.h
        Common/src/rapidjson/internal/stack.h
        Common/src/rapidjson/internal/strfunc.h
        Common/src/rapidjson/internal/strtod.h
        Common/src/rapidjson/internal/swap.h
        Common/src/rapidjson/msinttypes/inttypes.h
        Common/src/rapidjson/msinttypes/stdint.h
        Common/src/rapidjson/allocators.h
        Common/src/rapidjson/document.h
        Common/src/rapidjson/encodedstream.h
        Common/src/rapidjson/encodings.h
        Common/src/rapidjson/filereadstream.h
        Common/src/rapidjson/filewritestream.h
        Common/src/rapidjson/memorybuffer.h
        Common/src/rapidjson/memorystream.h
        Common/src/rapidjson/pointer.h
        Common/src/rapidjson/prettywriter.h
        Common/src/rapidjson/rapidjson.h
        Common/src/rapidjson/reader.h
        Common/src/rapidjson/stringbuffer.h
        Common/src/rapidjson/writer.h
        Common/src/Action.cpp
        Common/src/Action.h
        Common/src/Block.cpp
        Common/src/Block.h
        Common/src/CLI.cpp
        Common/src/CLI.h
        Common/src/CLIRunInitialization01.cpp
        Common/src/CLIRunInitialization01.h
        Common/src/CLISCNAck01.cpp
        Common/src/CLISCNAck01.h
        Common/src/CommandLine.cpp
        Common/src/CommandLine.h
        Common/src/DataStore.h
        Common/src/fast_mutex.h
        Common/src/File.cpp
        Common/src/File.h
        Common/src/GW.cpp
        Common/src/GW.h
        Common/src/GWHelloIPC02.cpp
        Common/src/GWHelloIPC02.h
        Common/src/GWMsgCl01.cpp
        Common/src/GWMsgCl01.h
        Common/src/GWRunInitialization01.cpp
        Common/src/GWRunInitialization01.h
        Common/src/GWRunHelloIPC02.cpp
        Common/src/GWRunHelloIPC02.h
        Common/src/GWExposition02.cpp
        Common/src/GWExposition02.h
        Common/src/HashMultimaps.h
        Common/src/HT.cpp
        Common/src/HT.h
        Common/src/HTBindReport01.cpp
        Common/src/HTBindReport01.h
        Common/src/HTDeliveryBind01.cpp
        Common/src/HTDeliveryBind01.h
        Common/src/HTGetBind01.cpp
        Common/src/HTGetBind01.h
        Common/src/HTInfoPayload01.cpp
        Common/src/HTInfoPayload01.h
        Common/src/HTListBind01.cpp
        Common/src/HTListBind01.h
        Common/src/HTMessageSeq01.cpp
        Common/src/HTMessageSeq01.h
        Common/src/HTMsgCl01.cpp
        Common/src/HTMsgCl01.h
        Common/src/HTRevokeBind01.cpp
        Common/src/HTRevokeBind01.h
        Common/src/HTRunInitialization01.cpp
        Common/src/HTRunInitialization01.h
        Common/src/HTRunPeriodic01.cpp
        Common/src/HTRunPeriodic01.h
        Common/src/HTSCNSeq01.cpp
        Common/src/HTSCNSeq01.h
        Common/src/HTStatusS01.cpp
        Common/src/HTStatusS01.h
        Common/src/HTStoreBind01.cpp
        Common/src/HTStoreBind01.h
        Common/src/Message.cpp
        Common/src/Message.h
        Common/src/MessageBuilder.cpp
        Common/src/MessageBuilder.h
        Common/src/MessageReceiving.h
        Common/src/MurmurHash3.cpp
        Common/src/MurmurHash3.h
        Common/src/MurmurHash3Strategy.cpp
        Common/src/MurmurHash3Strategy.h
        Common/src/NameGenerator.cpp
        Common/src/NameGenerator.h
        Common/src/NGAL_SAR.cpp
        Common/src/NGAL_SAR.h
        Common/src/NGAL_CS.cpp
        Common/src/NGAL_CS.h
        Common/src/NameStrategy.h
        Common/src/DefaultNameStrategy.cpp
        Common/src/DefaultNameStrategy.h
        Common/src/HashFunction.h
        Common/src/OutputVariable.cpp
        Common/src/OutputVariable.h
        Common/src/Process.cpp
        Common/src/Process.h
        Common/src/Prompt_iostream.cpp
        Common/src/Prompt_iostream.h
        Common/src/Publication.h
        Common/src/Shm.h
        Common/src/Subscription.h
        Common/src/tinythread.cpp
        Common/src/tinythread.h
        Common/src/Tuple.h)

if(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS)
    add_executable(ng039_gw_q0_q1_harness
            tests/spec039/gw_q0_q1_harness.cpp)
    target_link_libraries(ng039_gw_q0_q1_harness LINK_PRIVATE Common)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++") # SPEC-MUSL-001: musl static link fix (cherry-pick e714ab1)

add_executable(ContentApp
        ContentApp/src/ContentApp.cpp
        ContentApp/src/ContentApp.h
        ContentApp/src/Core.cpp
        ContentApp/src/Core.h
        ContentApp/src/CoreDeliveryBind01.cpp
        ContentApp/src/CoreDeliveryBind01.h
        ContentApp/src/CoreInfoPayload01.cpp
        ContentApp/src/CoreInfoPayload01.h
        ContentApp/src/CoreMsgCl01.cpp
        ContentApp/src/CoreMsgCl01.h
        ContentApp/src/CoreNotifyS01.cpp
        ContentApp/src/CoreNotifyS01.h
        ContentApp/src/CoreRunDiscover01.cpp
        ContentApp/src/CoreRunDiscover01.h
        ContentApp/src/CoreRunEvaluate01.cpp
        ContentApp/src/CoreRunEvaluate01.h
        ContentApp/src/CoreRunExpose01.cpp
        ContentApp/src/CoreRunExpose01.h
        ContentApp/src/CoreRunInitialize01.cpp
        ContentApp/src/CoreRunInitialize01.h
        ContentApp/src/CoreRunInvite01.cpp
        ContentApp/src/CoreRunInvite01.h
        ContentApp/src/CoreRunPeriodic01.cpp
        ContentApp/src/CoreRunPeriodic01.h
        ContentApp/src/CoreRunContentPublish01.cpp
        ContentApp/src/CoreRunContentPublish01.h
        ContentApp/src/CoreRunPublish01.cpp
        ContentApp/src/CoreRunPublish01.h
        ContentApp/src/CoreRunPublish02.cpp
        ContentApp/src/CoreRunPublish02.h
        ContentApp/src/CoreRunPublish03.cpp
        ContentApp/src/CoreRunPublish03.h
        ContentApp/src/CoreRunSubscribe01.cpp
        ContentApp/src/CoreRunSubscribe01.h
        ContentApp/src/CoreSCNAck01.cpp
        ContentApp/src/CoreSCNAck01.h
        ContentApp/src/CoreSCNSeq01.cpp
        ContentApp/src/CoreSCNSeq01.h
        ContentApp/src/CoreStatusS01.cpp
        ContentApp/src/CoreStatusS01.h
        ContentApp/src/execContentApp.cpp)

target_link_libraries(ContentApp LINK_PRIVATE Common)

if(NG_ENABLE_LEGACY_STANDALONE)
add_executable(GIRS
        GIRS/src/execGIRS.cpp
        GIRS/src/GIRS.cpp
        GIRS/src/GIRS.h
        GIRS/src/IR.cpp
        GIRS/src/IR.h
        GIRS/src/IRDeliveryBind01.cpp
        GIRS/src/IRDeliveryBind01.h
        GIRS/src/IRGetBind01.cpp
        GIRS/src/IRGetBind01.h
        GIRS/src/IRGetBind02.cpp
        GIRS/src/IRGetBind02.h
        GIRS/src/IRInfoPayload01.cpp
        GIRS/src/IRInfoPayload01.h
        GIRS/src/IRInfoPayload02.cpp
        GIRS/src/IRInfoPayload02.h
        GIRS/src/IRMessageSeq01.cpp
        GIRS/src/IRMessageSeq01.h
        GIRS/src/IRMsgCl01.cpp
        GIRS/src/IRMsgCl01.h
        GIRS/src/IRMsgCl02.cpp
        GIRS/src/IRMsgCl02.h
        GIRS/src/IRRevokeBind01.cpp
        GIRS/src/IRRevokeBind01.h
        GIRS/src/IRRunInitialization01.cpp
        GIRS/src/IRRunInitialization01.h
        GIRS/src/IRRunPeriodic01.cpp
        GIRS/src/IRRunPeriodic01.h
        GIRS/src/IRSCNAck01.cpp
        GIRS/src/IRSCNAck01.h
        GIRS/src/IRSCNSeq01.cpp
        GIRS/src/IRSCNSeq01.h
        GIRS/src/IRSCNSeq02.cpp
        GIRS/src/IRSCNSeq02.h
        GIRS/src/IRStatusS01.cpp
        GIRS/src/IRStatusS01.h
        GIRS/src/IRStoreBind01.cpp
        GIRS/src/IRStoreBind01.h
        GIRS/src/IRStoreBind02.cpp
        GIRS/src/IRStoreBind02.h)

target_include_directories(GIRS PRIVATE GIRS/src)
target_link_libraries(GIRS LINK_PRIVATE Common)
endif()

if(NG_ENABLE_LEGACY_STANDALONE)
add_executable(HTS
        HTS/src/DHT.cpp
        HTS/src/DHT.h
        HTS/src/DHTDeliveryBind01.cpp
        HTS/src/DHTDeliveryBind01.h
        HTS/src/DHTMsgCl01.cpp
        HTS/src/DHTMsgCl01.h
        HTS/src/DHTRunInitialization01.cpp
        HTS/src/DHTRunInitialization01.h
        HTS/src/DHTRunPeriodic01.cpp
        HTS/src/DHTRunPeriodic01.h
        HTS/src/execHTS.cpp
        HTS/src/HTS.cpp
        HTS/src/HTS.h)

target_include_directories(HTS PRIVATE HTS/src)
target_link_libraries(HTS LINK_PRIVATE Common)
endif()

add_executable(IoTTestApp
        IoTTestApp/src/Core.cpp
        IoTTestApp/src/Core.h
        IoTTestApp/src/CoreDeliveryBind01.cpp
        IoTTestApp/src/CoreDeliveryBind01.h
        IoTTestApp/src/CoreInfoPayload01.cpp
        IoTTestApp/src/CoreInfoPayload01.h
        IoTTestApp/src/CoreMsgCl01.cpp
        IoTTestApp/src/CoreMsgCl01.h
        IoTTestApp/src/CoreNotifyS01.cpp
        IoTTestApp/src/CoreNotifyS01.h
        IoTTestApp/src/CoreRunDiscover01.cpp
        IoTTestApp/src/CoreRunDiscover01.h
        IoTTestApp/src/CoreRunEvaluate01.cpp
        IoTTestApp/src/CoreRunEvaluate01.h
        IoTTestApp/src/CoreRunExpose01.cpp
        IoTTestApp/src/CoreRunExpose01.h
        IoTTestApp/src/CoreRunInitialize01.cpp
        IoTTestApp/src/CoreRunInitialize01.h
        IoTTestApp/src/CoreRunInvite01.cpp
        IoTTestApp/src/CoreRunInvite01.h
        IoTTestApp/src/CoreRunPeriodic01.cpp
        IoTTestApp/src/CoreRunPeriodic01.h
        IoTTestApp/src/CoreRunPublish01.cpp
        IoTTestApp/src/CoreRunPublish01.h
        IoTTestApp/src/CoreRunPublish02.cpp
        IoTTestApp/src/CoreRunPublish02.h
        IoTTestApp/src/CoreRunPublish03.cpp
        IoTTestApp/src/CoreRunPublish03.h
        IoTTestApp/src/CoreRunSubscribe01.cpp
        IoTTestApp/src/CoreRunSubscribe01.h
        IoTTestApp/src/CoreSCNAck01.cpp
        IoTTestApp/src/CoreSCNAck01.h
        IoTTestApp/src/CoreSCNSeq01.cpp
        IoTTestApp/src/CoreSCNSeq01.h
        IoTTestApp/src/CoreStatusS01.cpp
        IoTTestApp/src/CoreStatusS01.h
        IoTTestApp/src/execIoTTestApp.cpp
        IoTTestApp/src/IoTTestApp.cpp
        IoTTestApp/src/IoTTestApp.h)

target_link_libraries(IoTTestApp LINK_PRIVATE Common)

add_executable(NBTestApp
        NBTestApp/src/NBTestApp.cpp
        NBTestApp/src/NBTestApp.h
        NBTestApp/src/Core.cpp
        NBTestApp/src/Core.h
        NBTestApp/src/CoreDeliveryBind01.cpp
        NBTestApp/src/CoreDeliveryBind01.h
        NBTestApp/src/CoreMsgCl01.cpp
        NBTestApp/src/CoreMsgCl01.h
        NBTestApp/src/CoreRunDiscover01.cpp
        NBTestApp/src/CoreRunDiscover01.h
        NBTestApp/src/CoreRunEvaluate01.cpp
        NBTestApp/src/CoreRunEvaluate01.h
        NBTestApp/src/CoreRunInitialize01.cpp
        NBTestApp/src/CoreRunInitialize01.h
        NBTestApp/src/CoreRunPeriodic01.cpp
        NBTestApp/src/CoreRunPeriodic01.h
        NBTestApp/src/CoreRunSubscribe01.cpp
        NBTestApp/src/CoreRunSubscribe01.h
        NBTestApp/src/CoreSCNAck01.cpp
        NBTestApp/src/CoreSCNAck01.h
        NBTestApp/src/CoreSCNSeq01.cpp
        NBTestApp/src/CoreSCNSeq01.h
        NBTestApp/src/CoreStatusS01.cpp
        NBTestApp/src/CoreStatusS01.h
        NBTestApp/src/execNBTestApp.cpp)

target_link_libraries(NBTestApp LINK_PRIVATE Common)

add_executable(PGCS
        PGCS/src/Core.cpp
        PGCS/src/Core.h
        PGCS/src/CoreDeliveryBind01.cpp
        PGCS/src/CoreDeliveryBind01.h
        PGCS/src/CoreInfoPayload01.cpp
        PGCS/src/CoreInfoPayload01.h
        PGCS/src/CoreMsgCl01.cpp
        PGCS/src/CoreMsgCl01.h
        PGCS/src/CoreNotifyS01.cpp
        PGCS/src/CoreNotifyS01.h
        PGCS/src/CoreRunDiscover01.cpp
        PGCS/src/CoreRunDiscover01.h
        PGCS/src/CoreRunEvaluate01.cpp
        PGCS/src/CoreRunEvaluate01.h
        PGCS/src/CoreRunExpose01.cpp
        PGCS/src/CoreRunExpose01.h
        PGCS/src/CoreRunInitialize01.cpp
        PGCS/src/CoreRunInitialize01.h
        PGCS/src/CoreRunInvite01.cpp
        PGCS/src/CoreRunInvite01.h
        PGCS/src/CoreRunPeriodic01.cpp
        PGCS/src/CoreRunPeriodic01.h
        PGCS/src/CoreRunPublish01.cpp
        PGCS/src/CoreRunPublish01.h
        PGCS/src/CoreRunPublish02.cpp
        PGCS/src/CoreRunPublish02.h
        PGCS/src/CoreRunPublish03.cpp
        PGCS/src/CoreRunPublish03.h
        PGCS/src/CoreRunSubscribe01.cpp
        PGCS/src/CoreRunSubscribe01.h
        PGCS/src/CoreSCNAck01.cpp
        PGCS/src/CoreSCNAck01.h
        PGCS/src/CoreSCNSeq01.cpp
        PGCS/src/CoreSCNSeq01.h
        PGCS/src/CoreStatusS01.cpp
        PGCS/src/CoreStatusS01.h
        PGCS/src/execPGCS.cpp
        PGCS/src/PG.cpp
        PGCS/src/PG.h
        PGCS/src/PGCS.cpp
        PGCS/src/PGCS.h
        PGCS/src/PGHelloIHC01.cpp
        PGCS/src/PGHelloIHC01.h
        PGCS/src/PGHelloIHC02.cpp
        PGCS/src/PGHelloIHC02.h
        PGCS/src/PGHelloIHC03.cpp
        PGCS/src/PGHelloIHC03.h
        PGCS/src/NGAL_Transport_RAW.cpp
        PGCS/src/NGAL_Transport_RAW.h
        PGCS/src/PGMsgCl01.cpp
        PGCS/src/PGMsgCl01.h
        PGCS/src/PGRunExposition01.cpp
        PGCS/src/PGRunExposition01.h
        PGCS/src/PGRunHello01.cpp
        PGCS/src/PGRunHello01.h
        PGCS/src/PGRunHello02.cpp
        PGCS/src/PGRunHello02.h
        PGCS/src/PGRunHello03.cpp
        PGCS/src/PGRunHello03.h
        PGCS/src/PGRunInitialization01.cpp
        PGCS/src/PGRunInitialization01.h
        PGCS/src/PGRunPeriodic01.cpp
        PGCS/src/PGRunPeriodic01.h
        PGCS/src/PGRunPublishing01.cpp
        PGCS/src/PGRunPublishing01.h
        PGCS/src/PGRunStresstest01.cpp
        PGCS/src/PGRunStresstest01.h
        PGCS/src/PGStresstestPing01.cpp
        PGCS/src/PGStresstestPing01.h)

target_link_libraries(PGCS LINK_PRIVATE Common)

if(NG_ENABLE_LEGACY_STANDALONE)
add_executable(PSS
        PSS/src/execPSS.cpp
        PSS/src/PS.cpp
        PSS/src/PS.h
        PSS/src/PSDeliveryBind01.cpp
        PSS/src/PSDeliveryBind01.h
        PSS/src/PSInfoPayload01.cpp
        PSS/src/PSInfoPayload01.h
        PSS/src/PSMessageSeq01.cpp
        PSS/src/PSMessageSeq01.h
        PSS/src/PSMsgCl01.cpp
        PSS/src/PSMsgCl01.h
        PSS/src/PSPubBind01.cpp
        PSS/src/PSPubBind01.h
        PSS/src/PSPubNotify01.cpp
        PSS/src/PSPubNotify01.h
        PSS/src/PSRevokeBind01.cpp
        PSS/src/PSRevokeBind01.h
        PSS/src/PSRunInitialization01.cpp
        PSS/src/PSRunInitialization01.h
        PSS/src/PSRunPeriodic01.cpp
        PSS/src/PSRunPeriodic01.h
        PSS/src/PSS.cpp
        PSS/src/PSS.h
        PSS/src/PSSCNAck01.cpp
        PSS/src/PSSCNAck01.h
        PSS/src/PSSCNSeq01.cpp
        PSS/src/PSSCNSeq01.h
        PSS/src/PSStatusS01.cpp
        PSS/src/PSStatusS01.h
        PSS/src/PSSubBind01.cpp
        PSS/src/PSSubBind01.h)

target_include_directories(PSS PRIVATE PSS/src)
target_link_libraries(PSS LINK_PRIVATE Common)
endif()

add_executable(NRNCS
        NRNCS/src/execNRNCS.cpp
        NRNCS/src/NR.cpp
        NRNCS/src/NR.h
        NRNCS/src/NRDeliveryBind01.cpp
        NRNCS/src/NRDeliveryBind01.h
        NRNCS/src/NRInfoPayload01.cpp
        NRNCS/src/NRInfoPayload01.h
        NRNCS/src/NRMessageSeq01.cpp
        NRNCS/src/NRMessageSeq01.h
        NRNCS/src/NRMsgCl01.cpp
        NRNCS/src/NRMsgCl01.h
        NRNCS/src/NRPubBind01.cpp
        NRNCS/src/NRPubBind01.h
        NRNCS/src/NRPubNotify01.cpp
        NRNCS/src/NRPubNotify01.h
        NRNCS/src/NRRevokeBind01.cpp
        NRNCS/src/NRRevokeBind01.h
        NRNCS/src/NRRunInitialization01.cpp
        NRNCS/src/NRRunInitialization01.h
        NRNCS/src/NRRunPeriodic01.cpp
        NRNCS/src/NRRunPeriodic01.h
        NRNCS/src/NRNCS.cpp
        NRNCS/src/NRNCS.h
        NRNCS/src/NRSCNSeq01.cpp
        NRNCS/src/NRSCNSeq01.h
        NRNCS/src/NRSubBind01.cpp
        NRNCS/src/NRSubBind01.h)

target_link_libraries(NRNCS LINK_PRIVATE Common)
```

## FILE: isolated-D1H-worktree/diff-CMakeLists.txt

Role: Exact pre-existing overlay delta from baseline.
Bytes: 1145
SHA-256: 75b25d39ae24b524695eebcba758915fbe7c1526c242dc0bec7ba5c2b79cb710

```text
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 809cefb..949afa0 100755
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -7,6 +7,10 @@ option(NG_ENABLE_LEGACY_STANDALONE
        "Build deprecated standalone PSS, GIRS and HTS services"
        OFF)
 
+option(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS
+       "Build NG-039 Gateway compile/link probe only (no runtime execution)"
+       OFF)
+
 if(NG_ENABLE_LEGACY_STANDALONE)
     message(WARNING
             "NG_ENABLE_LEGACY_STANDALONE=ON: PSS, GIRS and HTS are deprecated; use NRNCS")
@@ -141,6 +145,12 @@ add_library(Common
         Common/src/tinythread.h
         Common/src/Tuple.h)
 
+if(NG_ENABLE_NG039_GW_Q0_Q1_HARNESS)
+    add_executable(ng039_gw_q0_q1_harness
+            tests/spec039/gw_q0_q1_harness.cpp)
+    target_link_libraries(ng039_gw_q0_q1_harness LINK_PRIVATE Common)
+endif()
+
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer")
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++") # SPEC-MUSL-001: musl static link fix (cherry-pick e714ab1)
 
```

## FILE: isolated-D1H-worktree/tests/spec039/gw_q0_q1_harness.cpp

Role: Pre-existing untracked D1-H artifact; compile/link context only, not a D2 implementation.
Bytes: 1814
SHA-256: 15913ee8b82fed7ad259668ed84e6e261aa444a3df443f3521929eba01b8dc92

```text
// NG-039 D1-H compile/link probe only.
//
// This translation unit proves that the pinned Common library exports the
// public Gateway entry points named by the Q0/Q1 design. It deliberately does
// not construct Process/GW, start threads, touch IPC, enqueue messages, or
// claim any runtime correctness. The D2 fixture is blocked until queue state
// and message execution can be observed without changing production code.

#include "Process.h"
#include "GW.h"

#include <iostream>
#include <type_traits>

namespace
{
using ProcessRunGateway = void (Process::*)();
using GatewayLoop = void (GW::*)();
using GatewayEnqueue = void (GW::*)(Message*);
using GatewayStop = void (GW::*)(bool);

static_assert(std::is_same_v<decltype(&Process::RunGateway), ProcessRunGateway>);
static_assert(std::is_same_v<decltype(&GW::Gateway), GatewayLoop>);
static_assert(std::is_same_v<decltype(&GW::PushToInputQueue), GatewayEnqueue>);
static_assert(std::is_same_v<decltype(&GW::SetStopGatewayFlag), GatewayStop>);

// Taking and retaining the addresses forces the linker to resolve the actual
// production definitions in Common without executing any of them.
ProcessRunGateway volatile process_run_gateway = &Process::RunGateway;
GatewayLoop volatile gateway_loop = &GW::Gateway;
GatewayEnqueue volatile gateway_enqueue = &GW::PushToInputQueue;
GatewayStop volatile gateway_stop = &GW::SetStopGatewayFlag;
} // namespace

int main()
{
  const bool linked = process_run_gateway != nullptr &&
                      gateway_loop != nullptr &&
                      gateway_enqueue != nullptr &&
                      gateway_stop != nullptr;

  std::cout << "NG-039 D1-H compile/link probe only; no process, thread, IPC, "
               "Q0/Q1 fixture, or measurement was run. D2 remains blocked.\n";
  return linked ? 0 : 1;
}
```

## FILE: isolated-D1H-worktree/tests/spec039/q0_q1.manifest.json

Role: Pre-existing untracked D1-H artifact; compile/link context only, not a D2 implementation.
Bytes: 5748
SHA-256: 9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a

```text
{
  "schema_version": 1,
  "task_id": "NG-039",
  "child_task_id": "NG-040",
  "branch": "AIOPT5",
  "document_commit": "b40bce89fe8692a5092889dd675e38d6af814d4c",
  "document_sha256": "8dd8234d2fbdd147f6e942f4bd509e953ea824471fd37c8a87029ecd675abd7b",
  "d0_approved_draft_sha256": "8026e1320149c318a5218eaa36c730f262081a5bbe0c2d582b23c307c7144e1c",
  "functional_baseline_sha": "1af604dad66148618cc0e578c28adb4a9829e607",
  "functional_baseline_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
  "scope": {
    "conditions": [
      {
        "id": "Q0",
        "definition": "No offered application messages; true queue-idle state has not been established."
      },
      {
        "id": "Q1",
        "definition": "One future-dated application message intended to remain pending throughout the measured window; admission, sole-head status, and non-execution are not yet observable."
      }
    ],
    "gateway_consumer_count": 1,
    "excluded_conditions": ["Q2", "Q3", "Q4"],
    "production_changes_allowed": false,
    "pgcs_or_pg_changes_allowed": false,
    "aiopt4_in_scope": false
  },
  "authorization": {
    "d0": "DOCUMENT_ONLY",
    "d1_h": "AUTHORIZED_FOR_ISOLATED_TEST_ONLY_SOURCE_AND_BUILD",
    "d2_correctness": "NOT_AUTHORIZED",
    "d3_measurement": "NOT_AUTHORIZED",
    "runtime_execution": false,
    "ipc_or_semaphore_creation": false,
    "vm_or_ssh_operations": false,
    "paid_astra_review": false
  },
  "build_only_target": {
    "option": "NG_ENABLE_NG039_GW_Q0_Q1_HARNESS",
    "target": "ng039_gw_q0_q1_harness",
    "source": "tests/spec039/gw_q0_q1_harness.cpp",
    "purpose": "Compile/link references to the production Process/GW public entry points; no runtime fixture."
  },
  "source_map": {
    "process_entry": "Process::RunGateway() -> GW::Gateway()",
    "queue_admission": "GW::PushToInputQueue(Message*) requires more than two command lines.",
    "future_message_rule": "GW::Gateway() pops only when ScheduledTime < GetTime().",
    "baseline_paths": [
      "Common/src/Process.cpp",
      "Common/src/GW.cpp",
      "Common/src/GW.h"
    ]
  },
  "d2_blockers": [
    "GW construction performs initialization that can enqueue bootstrap messages; Q0 as a truly idle input queue is not established by the current public path.",
    "InputQueue has no public snapshot/clear API, so Q1 admission, sole-head/pending status, and non-execution cannot be independently asserted through the public interface.",
    "GW::Gateway() starts an output worker and polls/creates local IPC resources; dynamic correctness requires a separately authorized lifecycle and disposable-namespace design."
  ],
  "d0_run_parameters_not_frozen_for_execution": {
    "independent_runs_per_condition": 5,
    "warmup_seconds": 30,
    "measurement_seconds": 120,
    "bounded_drain_seconds": 30,
    "note": "D0 proposal values only; D2/D3 remain unauthorized and these values must be confirmed/frozen before any run."
  },
  "implemented_paths": [
    "CMakeLists.txt",
    "tests/spec039/gw_q0_q1_harness.cpp",
    "tests/spec039/q0_q1.manifest.json"
  ],
  "deferred_paths_pending_observability_and_separate_gate": [
    "tests/spec039/run_gw_q0_q1.py",
    "tests/spec039/analyse_gw_q0_q1.py"
  ],
  "forbidden_paths_and_actions": [
    "Common/src/GW.cpp",
    "Common/src/GW.h",
    "PGCS/src/**",
    "Make/**",
    "Scripts/**",
    "Specs/RESULTS-SPEC-039/**",
    "Do not run the executable, CTest, a runner, a profiler, a sampler, or a benchmark.",
    "Do not create, attach, inspect destructively, or clean SysV IPC, POSIX semaphores, or shared memory.",
    "Do not change the primary AIOPT5 worktree, commit, push, use VMs/SSH/QGA, or contact paid Astra."
  ],
  "isolated_checkout": "/home/gandalf/workspace/novagenesis-ng039-q0q1-1af604d",
  "external_build_directory": "/home/gandalf/.hermes/cache/scratch/ng039-q0q1-1af604d/build",
  "configure_command": "cmake -S /home/gandalf/workspace/novagenesis-ng039-q0q1-1af604d -B /home/gandalf/.hermes/cache/scratch/ng039-q0q1-1af604d/build -DNG_ENABLE_NG039_GW_Q0_Q1_HARNESS=ON",
  "build_command": "cmake --build /home/gandalf/.hermes/cache/scratch/ng039-q0q1-1af604d/build --target ng039_gw_q0_q1_harness --parallel 2",
  "authorization_evidence": {
    "state": "APPROVED",
    "session": "20261004_123636_9a3f44",
    "user_message_id": 100566,
    "scope": "isolated test-only source and build; no executable/runtime, IPC, D2 correctness, D3 measurement, production changes, VM/SSH, or paid Astra"
  },
  "build_evidence": {
    "result": "PASS",
    "exit_code": 0,
    "cmake_version": "3.28",
    "generator": "Unix Makefiles",
    "compiler": "/usr/bin/c++",
    "compiler_version": "GNU 13.3.0",
    "target": "ng039_gw_q0_q1_harness",
    "cxx_defines": "",
    "cxx_flags": "-O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer -std=gnu++20",
    "link_flags": "-O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer -static -static-libgcc -static-libstdc++",
    "binary_path": "/home/gandalf/.hermes/cache/scratch/ng039-q0q1-1af604d/build/ng039_gw_q0_q1_harness",
    "binary_sha256": "ddf06f6d47e37564b9e52866c3d272ce3cc6cd8b433700c864c4499d676c31ab",
    "source_sha256": {
      "CMakeLists.txt": "d9dfcf2931f62a926241f93a61ed38e3d5d33e377293b84e22082f0c9a323c2d",
      "tests/spec039/gw_q0_q1_harness.cpp": "15913ee8b82fed7ad259668ed84e6e261aa444a3df443f3521929eba01b8dc92"
    },
    "build_log_path": "/home/gandalf/.hermes/cache/scratch/ng039-q0q1-1af604d/build/build-output.txt",
    "build_log_sha256": "095009a9e4fb7097aef005300f7562ab8ce941c691538a88a4064f944f4d19aa",
    "executable_runtime_executed": false
  },
  "runtime_command": null
}
```

## FILE: Specs/RESULTS-SPEC-039/r13-tool-snapshot/spec039_methodology_v1.py

Role: Historical synthetic checker/fixture context; not D2 runtime evidence.
Bytes: 39277
SHA-256: 1066196faf1484d646feaaa2b00fe2a1267d731fe42d93a03fb45488f75fd581

````text
#!/usr/bin/env python3
"""Offline semantic interpreter for the SPEC-039 journal transition model.

This module does not execute or replace the production finalizer. It consumes
an explicit, versioned model object and classifies only synthetic journal data.
"""
from __future__ import annotations

import json
import re
from typing import Any

EVENT_FIELDS = {
    "schema_version",
    "run_id",
    "sequence",
    "event",
    "monotonic_ns",
    "pid",
    "uid",
    "resource_type",
    "candidate_key",
    "source_name",
    "error_number",
    "identity",
}


def _model_index(model: object) -> tuple[dict[str, Any], dict[str, Any]]:
    if not isinstance(model, dict):
        raise ValueError("model must be an object")
    required = {
        "model_id",
        "model_version",
        "schema_version",
        "journal_states",
        "journal_initial_state",
        "journal_terminal_state",
        "resource_states",
        "resource_initial_state",
        "resource_kinds",
        "identity_binding",
        "required_events",
        "events",
        "authority_modes",
        "mode_effects",
        "cleanup_outcomes",
        "normal_cleanup_failure",
    }
    if set(model) != required:
        raise ValueError("model has missing or unrecognized fields")
    if not isinstance(model["model_id"], str) or not model["model_id"]:
        raise ValueError("model_id must be a non-empty string")
    if isinstance(model["model_version"], bool) or model["model_version"] != 1:
        raise ValueError("unsupported model_version")
    if not isinstance(model["schema_version"], str) or not model["schema_version"]:
        raise ValueError("schema_version must be a non-empty string")
    for name in ("journal_states", "resource_states"):
        values = model[name]
        if not isinstance(values, list) or not values or any(not isinstance(v, str) or not v for v in values):
            raise ValueError(f"{name} must be a non-empty string list")
        if len(values) != len(set(values)):
            raise ValueError(f"{name} contains duplicate states")
    journal_states = set(model["journal_states"])
    resource_states = set(model["resource_states"])
    if model["journal_initial_state"] not in journal_states or model["journal_terminal_state"] not in journal_states:
        raise ValueError("journal initial/terminal state is not declared")
    if model["resource_initial_state"] not in resource_states:
        raise ValueError("resource initial state is not declared")

    kinds = model["resource_kinds"]
    if not isinstance(kinds, dict) or not isinstance(kinds.get("journal"), str):
        raise ValueError("resource_kinds must declare a journal kind")
    candidates = kinds.get("candidates")
    if not isinstance(candidates, list) or not candidates or any(not isinstance(v, str) or not v for v in candidates):
        raise ValueError("candidate resource kinds must be a non-empty string list")
    if len(candidates) != len(set(candidates)):
        raise ValueError("candidate resource kinds contain duplicates")
    identity_binding = model["identity_binding"]
    if not isinstance(identity_binding, dict) or set(identity_binding) != set(candidates):
        raise ValueError("identity_binding must define exactly one binding for each candidate kind")
    for kind, fields in identity_binding.items():
        if not isinstance(fields, list) or not fields or any(not isinstance(v, str) or not v for v in fields):
            raise ValueError(f"identity binding for {kind} must be a non-empty field list")
        if len(fields) != len(set(fields)):
            raise ValueError(f"identity binding for {kind} repeats a field")
    if identity_binding != {
        "sysv_shm": ["candidate_key", "shmid"],
        "posix_semaphore": ["source_name", "device", "inode"],
    }:
        raise ValueError("resource identity bindings omit the exact SysV ID or POSIX device/inode tuple")

    rules = model["events"]
    if not isinstance(rules, list) or not rules:
        raise ValueError("events must be a non-empty list")
    by_event: dict[str, Any] = {}
    requirement_ids: set[str] = set()
    for rule in rules:
        if not isinstance(rule, dict):
            raise ValueError("event rule must be an object")
        allowed_rule_fields = {
            "requirement_id",
            "event",
            "scope",
            "phase_from",
            "phase_to",
            "resource_from",
            "resource_to",
            "requires_no_resource_states",
        }
        required_rule_fields = allowed_rule_fields - {"requires_no_resource_states"}
        if not required_rule_fields <= set(rule) or not set(rule) <= allowed_rule_fields:
            raise ValueError("event rule has missing or unrecognized fields")
        name = rule.get("event")
        requirement_id = rule.get("requirement_id")
        if not isinstance(name, str) or not name or name in by_event:
            raise ValueError("event names must be unique non-empty strings")
        if not isinstance(requirement_id, str) or not requirement_id or requirement_id in requirement_ids:
            raise ValueError("requirement IDs must be unique non-empty strings")
        if rule.get("scope") not in {"journal", "resource"}:
            raise ValueError(f"invalid scope for {name}")
        phases = rule.get("phase_from")
        if not isinstance(phases, list) or not phases or not set(phases) <= journal_states:
            raise ValueError(f"invalid phase_from for {name}")
        if rule.get("phase_to") not in journal_states:
            raise ValueError(f"invalid phase_to for {name}")
        if rule["scope"] == "resource":
            if rule.get("resource_from") not in resource_states or rule.get("resource_to") not in resource_states:
                raise ValueError(f"invalid resource transition for {name}")
        elif rule.get("resource_from") is not None or rule.get("resource_to") is not None:
            raise ValueError(f"journal event {name} cannot have a resource transition")
        required_empty = rule.get("requires_no_resource_states", [])
        if not isinstance(required_empty, list) or not set(required_empty) <= resource_states:
            raise ValueError(f"invalid resource-state guard for {name}")
        by_event[name] = rule
        requirement_ids.add(requirement_id)

    required_events = model["required_events"]
    if not isinstance(required_events, list) or any(not isinstance(v, str) or not v for v in required_events) or len(required_events) != len(set(required_events)):
        raise ValueError("required_events must be a unique list")
    if set(required_events) != set(by_event):
        raise ValueError("declared event set differs from transition table")
    transition_pairs = (
        ("CREATED", "CREATE_INTENT"),
        ("CREATE_FAILED", "CREATE_INTENT"),
        ("CLEANUP_INTENT", "CREATED"),
        ("REMOVED", "CLEANUP_INTENT"),
        ("CLEANUP_FAILED", "CLEANUP_INTENT"),
    )
    for outcome_event, prior_event in transition_pairs:
        if by_event[outcome_event]["scope"] != "resource" or by_event[prior_event]["scope"] != "resource":
            raise ValueError(f"{outcome_event}/{prior_event} must be resource-scoped")
        if by_event[outcome_event]["resource_from"] != by_event[prior_event]["resource_to"]:
            raise ValueError(f"{outcome_event} does not consume the state produced by {prior_event}")
    create_pending = by_event["CREATE_INTENT"]["resource_to"]
    cleanup_pending = by_event["CLEANUP_INTENT"]["resource_to"]
    if create_pending == cleanup_pending:
        raise ValueError("creation and cleanup pending states must be distinct")
    if by_event["CREATE_INTENT"]["resource_from"] != model["resource_initial_state"]:
        raise ValueError("CREATE_INTENT must start from resource_initial_state")
    if create_pending not in by_event["RUN_SHUTDOWN_COMPLETE"].get("requires_no_resource_states", []):
        raise ValueError("RUN_SHUTDOWN_COMPLETE must reject unresolved creation intents")
    close_guards = set(by_event["JOURNAL_CLOSED"].get("requires_no_resource_states", []))
    if not {create_pending, cleanup_pending} <= close_guards:
        raise ValueError("JOURNAL_CLOSED must reject unresolved create/cleanup intents")
    opened = by_event["JOURNAL_OPENED"]
    shutdown = by_event["RUN_SHUTDOWN_COMPLETE"]
    closed = by_event["JOURNAL_CLOSED"]
    if opened["scope"] != "journal" or opened["phase_from"] != [model["journal_initial_state"]]:
        raise ValueError("JOURNAL_OPENED must start the global lifecycle")
    open_state = opened["phase_to"]
    shutdown_state = shutdown["phase_to"]
    if shutdown["scope"] != "journal" or open_state not in shutdown["phase_from"] or shutdown_state == open_state:
        raise ValueError("RUN_SHUTDOWN_COMPLETE must advance the global lifecycle")
    if closed["scope"] != "journal" or shutdown_state not in closed["phase_from"] or closed["phase_to"] != model["journal_terminal_state"]:
        raise ValueError("JOURNAL_CLOSED must terminate the post-shutdown lifecycle")
    for name in ("CREATE_INTENT", "CREATED", "CREATE_FAILED"):
        if open_state not in by_event[name]["phase_from"] or by_event[name]["phase_to"] != open_state:
            raise ValueError(f"{name} must remain in the pre-shutdown phase")
    for name in ("CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED"):
        if shutdown_state not in by_event[name]["phase_from"] or by_event[name]["phase_to"] != shutdown_state:
            raise ValueError(f"{name} must remain in the post-shutdown phase")

    modes = model["authority_modes"]
    if not isinstance(modes, dict) or set(modes) != {"requirement_ids", "common_requirements", "normal_requirements", "mode_names"}:
        raise ValueError("authority_modes must be an object")
    authority_ids = modes.get("requirement_ids")
    if authority_ids != {"common": "AUTH-001", "normal": "AUTH-002", "abnormal": "AUTH-003", "blocked": "AUTH-004"}:
        raise ValueError("authority mode requirement IDs are missing, duplicated or unstable")
    common = modes.get("common_requirements")
    normal = modes.get("normal_requirements")
    names = modes.get("mode_names")
    required_common = {"trusted_launcher", "valid_supervisor_receipt", "all_pids_reaped", "logs_closed", "stable_result_directory", "continuous_lease"}
    required_normal = {"durable_shutdown_complete", "normal_waits_valid", "no_timeout_or_signal"}
    if not isinstance(common, list) or not required_common <= set(common):
        raise ValueError("common authority gates omit a mandatory identity/lease/process prerequisite")
    if not isinstance(normal, list) or not required_normal <= set(normal):
        raise ValueError("normal mode gates omit a mandatory shutdown/wait prerequisite")
    if any(not isinstance(v, str) or not v for v in common + normal) or len(common) != len(set(common)) or len(normal) != len(set(normal)):
        raise ValueError("authority gate lists must be unique non-empty strings")
    if names != {"normal": "normal_cleanup", "abnormal": "abnormal_evidence_only", "blocked": "blocked"}:
        raise ValueError("mode_names do not match the closed mode set")

    mode_effects = model["mode_effects"]
    expected_mode_effects = {"normal_cleanup", "abnormal_evidence_only", "blocked"}
    if not isinstance(mode_effects, dict) or set(mode_effects) != expected_mode_effects:
        raise ValueError("mode_effects must define exactly the three closed modes")
    for mode_name, effect in mode_effects.items():
        if not isinstance(effect, dict) or set(effect) != {"requirement_id", "status", "requires", "forbidden"} or not isinstance(effect.get("requires"), list) or not effect["requires"]:
            raise ValueError(f"mode effects for {mode_name} are incomplete")
        if not isinstance(effect.get("forbidden"), list) or not isinstance(effect.get("status"), list):
            raise ValueError(f"mode effects for {mode_name} are missing forbidden/status sets")
        if any(not isinstance(v, str) or not v for v in effect["requires"] + effect["forbidden"] + effect["status"]):
            raise ValueError(f"mode effects for {mode_name} must use non-empty strings")
        if len(effect["requires"]) != len(set(effect["requires"])) or len(effect["forbidden"]) != len(set(effect["forbidden"])) or len(effect["status"]) != len(set(effect["status"])):
            raise ValueError(f"mode effects for {mode_name} contain duplicates")
        expected_id = authority_ids["normal"] if mode_name == "normal_cleanup" else authority_ids["abnormal"] if mode_name == "abnormal_evidence_only" else authority_ids["blocked"]
        if effect.get("requirement_id") != expected_id:
            raise ValueError(f"mode effects for {mode_name} have an unstable requirement ID")
    if set(mode_effects["normal_cleanup"]["status"]) != {"PASS", "FAIL", "INCONCLUSIVE"}:
        raise ValueError("normal mode status set is not the closed acceptance set")
    abnormal_forbidden = set(mode_effects["abnormal_evidence_only"]["forbidden"])
    if not {"live_ipc_inspection", "cleanup", "journal_mutation", "retry", "PASS", "FAIL"} <= abnormal_forbidden:
        raise ValueError("abnormal evidence-only mode permits a forbidden side effect or status")
    blocked_forbidden = set(mode_effects["blocked"]["forbidden"])
    if not {"finalizer_entry", "finalizer_reentry", "cleanup"} <= blocked_forbidden:
        raise ValueError("blocked mode permits finalizer entry or cleanup")
    if set(mode_effects["abnormal_evidence_only"]["status"]) != {"INCONCLUSIVE"}:
        raise ValueError("abnormal evidence-only mode must yield only INCONCLUSIVE")
    if set(mode_effects["blocked"]["status"]) != {"BLOCKED", "INCONCLUSIVE"}:
        raise ValueError("blocked mode status set is not fail-closed")

    outcomes = model["cleanup_outcomes"]
    outcome_names = {"not_started", "partial", "complete", "unknown"}
    if not isinstance(outcomes, dict) or set(outcomes) != outcome_names:
        raise ValueError("cleanup_outcomes must define exactly the closed outcome set")
    outcome_ids: set[str] = set()
    for name, disposition in outcomes.items():
        if not isinstance(disposition, dict):
            raise ValueError(f"cleanup outcome {name} must be an object")
        outcome_fields = {
            "not_started": {"requirement_id", "journal_required", "created_count", "cleanup_intent_count", "create_failed_allowed", "run_status_effect"},
            "partial": {"requirement_id", "journal_required", "all_created_have_one_terminal_outcome", "cleanup_failed_count_min", "run_status_effect"},
            "complete": {"requirement_id", "journal_required", "created_count_min", "all_created_removed", "cleanup_failed_count", "run_status_effect"},
            "unknown": {"requirement_id", "journal_required", "run_status_effect"},
        }
        if set(disposition) != outcome_fields[name]:
            raise ValueError(f"cleanup outcome {name} has missing or unrecognized fields")
        requirement_id = disposition.get("requirement_id")
        if not isinstance(requirement_id, str) or not requirement_id or requirement_id in outcome_ids:
            raise ValueError("cleanup outcome requirement IDs must be unique non-empty strings")
        outcome_ids.add(requirement_id)
        if disposition.get("run_status_effect") == "PASS":
            raise ValueError("cleanup history cannot itself authorize PASS")
        if requirement_id != {"not_started": "COUT-001", "partial": "COUT-002", "complete": "COUT-003", "unknown": "COUT-004"}[name]:
            raise ValueError(f"cleanup outcome {name} has an unstable requirement ID")
    expected_status_effects = {
        "not_started": "NO_PASS_INFERENCE",
        "partial": "INCONCLUSIVE",
        "complete": "DOES_NOT_IMPLY_PASS",
        "unknown": "INCONCLUSIVE",
    }
    if any(outcomes[name].get("run_status_effect") != effect for name, effect in expected_status_effects.items()):
        raise ValueError("cleanup outcome history must not imply overall PASS")
    not_started = outcomes["not_started"]
    partial = outcomes["partial"]
    complete = outcomes["complete"]
    unknown = outcomes["unknown"]
    if not_started.get("journal_required") != "complete_transition_valid_closed" or not_started.get("created_count") != 0 or not_started.get("cleanup_intent_count") != 0:
        raise ValueError("not_started requires a complete journal with no created resource or cleanup intent")
    if partial.get("journal_required") != "complete_transition_valid_closed" or partial.get("all_created_have_one_terminal_outcome") is not True or partial.get("cleanup_failed_count_min") != 1:
        raise ValueError("partial requires complete created-resource coverage and a proven cleanup failure")
    if complete.get("journal_required") != "complete_transition_valid_closed" or complete.get("created_count_min") != 1 or complete.get("all_created_removed") is not True or complete.get("cleanup_failed_count") != 0:
        raise ValueError("complete requires all created resources removed with no cleanup failure")
    if unknown.get("journal_required") != "otherwise":
        raise ValueError("unknown must cover every history outside the other three dispositions")

    failure = model["normal_cleanup_failure"]
    if not isinstance(failure, dict) or set(failure) != {
        "requirement_id",
        "stop_destructive_calls_after_failure",
        "retry_after_intent",
        "outcome_source",
        "complete_history_after_journal_closed_later_evidence_failure",
        "authority_loss",
    } or failure.get("requirement_id") != "FIN-001":
        raise ValueError("normal_cleanup_failure must be a structured requirement")
    if failure.get("stop_destructive_calls_after_failure") is not True or failure.get("retry_after_intent") is not False:
        raise ValueError("normal cleanup failure must stop destructive retries")
    later = failure.get("complete_history_after_journal_closed_later_evidence_failure")
    authority = failure.get("authority_loss")
    if later != {"cleanup_outcome": "complete", "run_status": "INCONCLUSIVE"}:
        raise ValueError("completed cleanup history must remain distinct from later run failure")
    if not isinstance(authority, dict) or authority.get("finalizer_entry") is not False or authority.get("finalizer_reentry") is not False or authority.get("preserve_pending") is not True:
        raise ValueError("authority loss must block finalizer entry/re-entry and preserve pending evidence")
    return model, by_event


def _resource_key(item: dict[str, Any], rule: dict[str, Any], model: dict[str, Any]) -> tuple[str, int | None, str | None] | None:
    kinds = model["resource_kinds"]
    if not isinstance(kinds, dict):
        raise ValueError("resource_kinds must be an object")
    if rule["scope"] == "journal":
        if item["resource_type"] != kinds.get("journal") or item["candidate_key"] is not None or item["source_name"] is not None:
            raise ValueError("journal-scoped event has resource identity")
        return None
    candidates = kinds.get("candidates")
    if not isinstance(candidates, list) or item["resource_type"] not in candidates:
        raise ValueError("unsupported candidate resource type")
    key = item["candidate_key"]
    name = item["source_name"]
    if item["resource_type"] == "sysv_shm":
        if isinstance(key, bool) or not isinstance(key, int) or key <= 0 or name is not None:
            raise ValueError("invalid SysV SHM resource key")
    elif item["resource_type"] == "posix_semaphore":
        if key is not None or not isinstance(name, str) or not name:
            raise ValueError("invalid POSIX semaphore resource name")
    return (item["resource_type"], key, name)


def _stable_resource_identity(item: dict[str, Any], model: dict[str, Any]) -> tuple[Any, ...]:
    resource_type = item["resource_type"]
    fields = model["identity_binding"].get(resource_type)
    identity = item["identity"]
    if not isinstance(fields, list) or not isinstance(identity, dict):
        raise ValueError("resource event lacks bound identity evidence")
    values: list[Any] = []
    for field in fields:
        if field not in identity or identity[field] is None:
            raise ValueError(f"resource identity is missing {field}")
        value = identity[field]
        if field in {"candidate_key", "shmid", "device", "inode"}:
            if isinstance(value, bool) or not isinstance(value, int) or value < 0:
                raise ValueError(f"resource identity field {field} must be a non-negative integer")
        elif field == "source_name" and (not isinstance(value, str) or not value):
            raise ValueError("resource identity source_name must be non-empty")
        values.append(value)
    if resource_type == "sysv_shm" and identity["candidate_key"] != item["candidate_key"]:
        raise ValueError("identity candidate_key differs from event resource key")
    if resource_type == "posix_semaphore" and identity["source_name"] != item["source_name"]:
        raise ValueError("identity source_name differs from event resource name")
    return tuple(values)


def replay_journal(events: object, model: object) -> dict[str, Any]:
    """Replay a complete journal; invalid or ambiguous histories yield unknown."""
    try:
        checked_model, rules = _model_index(model)
        if not isinstance(events, list) or not events:
            raise ValueError("journal is empty or not a list")
        run_ids: set[str] = set()
        previous_time = -1
        resource_states: dict[tuple[str, int | None, str | None], str] = {}
        created: set[tuple[str, int | None, str | None]] = set()
        cleanup_intents: set[tuple[str, int | None, str | None]] = set()
        cleanup_results: dict[tuple[str, int | None, str | None], str] = {}
        identities: dict[tuple[str, int | None, str | None], tuple[Any, ...]] = {}
        phase = checked_model["journal_initial_state"]
        for expected_sequence, item in enumerate(events, 1):
            if not isinstance(item, dict) or set(item) != EVENT_FIELDS:
                raise ValueError("event has missing or extra fields")
            if item["schema_version"] != checked_model["schema_version"]:
                raise ValueError("event schema_version mismatch")
            if not isinstance(item["run_id"], str) or not item["run_id"]:
                raise ValueError("run_id must be a non-empty string")
            run_ids.add(item["run_id"])
            if isinstance(item["sequence"], bool) or not isinstance(item["sequence"], int) or item["sequence"] != expected_sequence:
                raise ValueError("event sequence is not contiguous")
            if isinstance(item["monotonic_ns"], bool) or not isinstance(item["monotonic_ns"], int) or item["monotonic_ns"] < previous_time:
                raise ValueError("event monotonic time is invalid or decreasing")
            previous_time = item["monotonic_ns"]
            if isinstance(item["pid"], bool) or not isinstance(item["pid"], int) or item["pid"] < 2:
                raise ValueError("pid must be an integer of at least 2")
            if isinstance(item["uid"], bool) or not isinstance(item["uid"], int) or item["uid"] < 0:
                raise ValueError("uid must be a non-negative integer")
            if item["error_number"] is not None and (isinstance(item["error_number"], bool) or not isinstance(item["error_number"], int)):
                raise ValueError("error_number must be an integer or null")
            if item["identity"] is not None and not isinstance(item["identity"], dict):
                raise ValueError("identity must be an object or null")
            event_name = item["event"]
            rule = rules.get(event_name)
            if rule is None:
                raise ValueError("unknown event")
            if phase not in rule["phase_from"]:
                raise ValueError(f"{event_name} is forbidden in phase {phase}")
            for forbidden_state in rule.get("requires_no_resource_states", []):
                if forbidden_state in resource_states.values():
                    raise ValueError(f"{event_name} requires no resource in state {forbidden_state}")
            resource = _resource_key(item, rule, checked_model)
            if rule["scope"] == "resource":
                if resource is None:
                    raise ValueError("resource event has no resource identity")
                old_state = resource_states.get(resource, checked_model["resource_initial_state"])
                if old_state != rule["resource_from"]:
                    raise ValueError(f"{event_name} requires resource state {rule['resource_from']}, got {old_state}")
                if event_name == "CREATED":
                    identities[resource] = _stable_resource_identity(item, checked_model)
                elif event_name in {"CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED"}:
                    event_identity = _stable_resource_identity(item, checked_model)
                    if resource not in identities or event_identity != identities[resource]:
                        raise ValueError(f"{event_name} identity differs from the exact CREATED resource")
                resource_states[resource] = rule["resource_to"]
                if event_name == "CREATED":
                    created.add(resource)
                elif event_name == "CLEANUP_INTENT":
                    cleanup_intents.add(resource)
                elif event_name in {"REMOVED", "CLEANUP_FAILED"}:
                    cleanup_results[resource] = event_name
            phase = rule["phase_to"]
        if len(run_ids) != 1:
            raise ValueError("journal contains multiple run IDs")
        if phase != checked_model["journal_terminal_state"]:
            raise ValueError("journal is not closed")
        if any(state in {"create_pending", "cleanup_pending"} for state in resource_states.values()):
            raise ValueError("journal closes with a pending transaction")
        if not created and not cleanup_intents:
            outcome = "not_started"
        elif created == cleanup_intents == set(cleanup_results):
            if any(result == "CLEANUP_FAILED" for result in cleanup_results.values()):
                outcome = "partial"
            elif all(result == "REMOVED" for result in cleanup_results.values()):
                outcome = "complete"
            else:
                outcome = "unknown"
        else:
            outcome = "unknown"
        return {
            "valid": True,
            "outcome": outcome,
            "reason": "transition-valid complete journal",
            "created_count": len(created),
            "cleanup_intent_count": len(cleanup_intents),
            "cleanup_result_count": len(cleanup_results),
        }
    except (KeyError, TypeError, ValueError, AssertionError) as exc:
        return {
            "valid": False,
            "outcome": "unknown",
            "reason": str(exc),
            "created_count": 0,
            "cleanup_intent_count": 0,
            "cleanup_result_count": 0,
        }


def classify_journal(events: object, model: object) -> str:
    return replay_journal(events, model)["outcome"]


def select_finalization_mode(gates: object, model: object) -> str:
    """Choose a documentary mode only when every shared authority gate is true."""
    try:
        if not isinstance(gates, dict):
            return "blocked"
        checked_model, _ = _model_index(model)
        contract = checked_model["authority_modes"]
        common = contract.get("common_requirements")
        normal = contract.get("normal_requirements")
        names = contract.get("mode_names")
        if not isinstance(common, list) or not common or not isinstance(normal, list) or not normal or not isinstance(names, dict):
            return "blocked"
        if any(not isinstance(key, str) or not key for key in common + normal):
            return "blocked"
        if any(gates.get(key) is not True for key in common):
            return names.get("blocked", "blocked")
        if all(gates.get(key) is True for key in normal):
            return names.get("normal", "normal_cleanup")
        return names.get("abnormal", "abnormal_evidence_only")
    except (KeyError, TypeError, ValueError):
        return "blocked"


def check_model_projections(
    spec_model: object,
    manifest_model: object,
    packet_model: object,
) -> list[str]:
    """Require the SPEC model to equal its structured manifest/packet copies."""
    errors: list[str] = []
    try:
        _model_index(spec_model)
    except (KeyError, TypeError, ValueError) as exc:
        errors.append(f"SPEC model invalid: {exc}")
    if manifest_model != spec_model:
        errors.append("manifest journal-transition projection differs from SPEC")
    if packet_model != spec_model:
        errors.append("packet journal-transition projection differs from SPEC")
    return errors


MODEL_BEGIN = "<!-- NG039-JOURNAL-MODEL-BEGIN -->"
MODEL_END = "<!-- NG039-JOURNAL-MODEL-END -->"
PINS_BEGIN = "<!-- NG039-ARTIFACT-PINS-BEGIN -->"
PINS_END = "<!-- NG039-ARTIFACT-PINS-END -->"


def _reject_duplicate_pairs(pairs: list[tuple[str, Any]]) -> dict[str, Any]:
    result: dict[str, Any] = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def _reject_non_finite(value: str) -> None:
    raise ValueError(f"non-finite JSON value: {value}")


def strict_json_loads(text: str) -> Any:
    """Parse JSON while rejecting duplicate keys and NaN/Infinity values."""
    return json.loads(
        text,
        object_pairs_hook=_reject_duplicate_pairs,
        parse_constant=_reject_non_finite,
    )


def extract_model(markdown: str) -> dict[str, Any]:
    """Extract the sole canonical model block from a SPEC/packet Markdown file."""
    if markdown.count(MODEL_BEGIN) != 1 or markdown.count(MODEL_END) != 1:
        raise ValueError("expected exactly one marked journal-transition model block")
    pattern = re.compile(
        re.escape(MODEL_BEGIN) + r"\s*```json\s*(.*?)\s*```\s*" + re.escape(MODEL_END),
        re.DOTALL,
    )
    matches = list(pattern.finditer(markdown))
    if len(matches) != 1:
        raise ValueError("journal-transition model markers do not enclose one JSON block")
    model = strict_json_loads(matches[0].group(1))
    _model_index(model)
    return model


def extract_pins(markdown: str) -> dict[str, str]:
    """Extract exactly one packet pin object, excluding the packet's own digest."""
    if markdown.count(PINS_BEGIN) != 1 or markdown.count(PINS_END) != 1:
        raise ValueError("expected exactly one marked artifact-pin block")
    pattern = re.compile(
        re.escape(PINS_BEGIN) + r"\s*```json\s*(.*?)\s*```\s*" + re.escape(PINS_END),
        re.DOTALL,
    )
    matches = list(pattern.finditer(markdown))
    if len(matches) != 1:
        raise ValueError("artifact-pin markers do not enclose one JSON block")
    pins = strict_json_loads(matches[0].group(1))
    required = {"document_head", "spec_sha256", "manifest_sha256"}
    if not isinstance(pins, dict) or set(pins) != required or any(not isinstance(v, str) for v in pins.values()):
        raise ValueError("artifact pins must contain exactly HEAD, SPEC SHA and manifest SHA")
    return pins


def audit_contract(
    spec_text: str,
    manifest: object,
    packet_text: str,
    actual_spec_sha256: str,
    actual_manifest_sha256: str,
    expected_head: str,
) -> list[str]:
    """Check exact model projections, relevant schema values and artifact pins."""
    errors: list[str] = []

    def read_path(root: object, *keys: str) -> Any:
        current = root
        for key in keys:
            if not isinstance(current, dict) or key not in current:
                raise KeyError(".".join(keys))
            current = current[key]
        return current

    def expect(label: str, actual: object, required: object) -> None:
        if actual != required:
            errors.append(f"{label} differs from the canonical SPEC model or frozen identity")

    try:
        spec_model = extract_model(spec_text)
    except (TypeError, ValueError) as exc:
        errors.append(f"SPEC model invalid: {exc}")
        spec_model = None
    try:
        packet_model = extract_model(packet_text)
    except (TypeError, ValueError) as exc:
        errors.append(f"packet model invalid: {exc}")
        packet_model = None
    if spec_model is not None:
        try:
            manifest_model = read_path(manifest, "classification_rules", "journal_transition_model")
        except KeyError as exc:
            errors.append(f"manifest model missing: {exc}")
            manifest_model = None
        errors.extend(check_model_projections(spec_model, manifest_model, packet_model))
        outcomes = spec_model.get("cleanup_outcomes") if isinstance(spec_model, dict) else None
        if isinstance(outcomes, dict):
            expected_outcomes = set(outcomes)
            mode_projection = {
                "model_id": spec_model["model_id"],
                "authority_modes": spec_model["authority_modes"],
                "mode_effects": spec_model["mode_effects"],
            }
            try:
                event_enum = set(read_path(manifest, "evidence_schemas", "reservation_journal_event", "properties", "event", "enum"))
                expect("reservation journal schema event enum", event_enum, set(spec_model["required_events"]))
                identity_properties = read_path(manifest, "evidence_schemas", "reservation_journal_event", "properties", "identity", "properties")
                for resource_type, fields in spec_model["identity_binding"].items():
                    if not set(fields) <= set(identity_properties):
                        errors.append(f"reservation journal identity schema omits binding fields for {resource_type}")
            except (KeyError, TypeError):
                errors.append("reservation journal event/identity schema projection missing")
            try:
                expect(
                    "manifest authority mode projection",
                    read_path(manifest, "classification_rules", "authority_mode_projection"),
                    spec_model["authority_modes"],
                )
                expect(
                    "manifest mode effect projection",
                    read_path(manifest, "classification_rules", "mode_effect_projection"),
                    spec_model["mode_effects"],
                )
                expect(
                    "manifest cleanup outcome requirement IDs",
                    read_path(manifest, "classification_rules", "cleanup_outcome_requirement_ids"),
                    [outcomes[name]["requirement_id"] for name in ("not_started", "partial", "complete", "unknown")],
                )
            except KeyError as exc:
                errors.append(f"manifest mode projection missing: {exc}")
            projection_paths = (
                ("classification_rules", "cleanup_outcome_projection"),
                ("sandbox", "finalizer_ipc_boundary", "cleanup_outcome_classifier"),
                ("sandbox", "finalizer_ipc_boundary", "abnormal_evidence_only", "cleanup_outcome_classifier"),
                ("semantic_finalizer_contract", "abnormal_evidence_only", "cleanup_outcome"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "abnormal_evidence_only", "cleanup_outcome_classifier"),
            )
            for path in projection_paths:
                try:
                    expect("manifest." + ".".join(path), read_path(manifest, *path), outcomes)
                except KeyError:
                    errors.append("manifest projection missing: " + ".".join(path))
            for path in (
                ("semantic_finalizer_contract", "normal_cleanup_failure"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "normal_cleanup_failure"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "failure_rule"),
                ("sandbox", "finalizer_ipc_boundary", "failure_rule"),
            ):
                try:
                    expect("manifest." + ".".join(path), read_path(manifest, *path), spec_model["normal_cleanup_failure"])
                except KeyError:
                    errors.append("manifest normal-failure projection missing: " + ".".join(path))
            for path in (
                ("sandbox", "finalizer_mode_selection"),
                ("sandbox", "finalizer_ipc_boundary", "mode_selection"),
                ("semantic_finalizer_contract", "finalizer_ipc_boundary", "mode_selection"),
            ):
                try:
                    expect("manifest." + ".".join(path), read_path(manifest, *path), mode_projection)
                except KeyError:
                    errors.append("manifest mode-effects projection missing: " + ".".join(path))
            for path in (
                ("evidence_schemas", "run_record", "properties", "failure", "oneOf", 1, "properties", "cleanup_outcome", "enum"),
                ("evidence_schemas", "run_record", "allOf", -1, "then", "properties", "failure", "properties", "cleanup_outcome", "enum"),
            ):
                try:
                    current = manifest
                    for key in path:
                        current = current[key] if isinstance(current, (dict, list)) else None
                    expect("manifest schema " + ".".join(map(str, path)), set(current), expected_outcomes)
                except (KeyError, IndexError, TypeError):
                    errors.append("manifest cleanup-outcome schema enum missing: " + ".".join(map(str, path)))

    for label, path, target in (
        ("manifest SPEC digest", ("spec", "sha256"), actual_spec_sha256),
        ("run-record SPEC digest", ("evidence_schemas", "run_record", "properties", "spec_sha256", "const"), actual_spec_sha256),
        ("manifest document HEAD", ("source", "document_checkout_head"), expected_head),
        ("manifest review revision", ("review_revision",), "r13"),
        ("manifest SPEC version", ("spec", "version"), "v1.15"),
        ("manifest Astra submission state", ("local_validation_this_freeze", "astra_submitted"), False),
        ("manifest exact-SHA consent state", ("local_validation_this_freeze", "owner_exact_sha_consent_for_r13"), False),
        ("manifest review-artifact state", ("review_artifacts", "submission_state"), "UNSUBMITTED_CONSENT_REQUIRED"),
    ):
        try:
            expect(label, read_path(manifest, *path), target)
        except KeyError:
            errors.append("manifest identity missing: " + ".".join(path))
    try:
        pins = extract_pins(packet_text)
        expected_pins = {
            "document_head": expected_head,
            "spec_sha256": actual_spec_sha256,
            "manifest_sha256": actual_manifest_sha256,
        }
        expect("packet artifact pins", pins, expected_pins)
    except (TypeError, ValueError) as exc:
        errors.append(f"packet pins invalid: {exc}")
    return errors
````

## FILE: Specs/RESULTS-SPEC-039/r13-tool-snapshot/test_spec039_journal_transitions.py

Role: Historical synthetic checker/fixture context; not D2 runtime evidence.
Bytes: 23316
SHA-256: 41397e374e3a3e98f159334e606ad2c77b8136e0728dc314cd989079ebd36668

```text
#!/usr/bin/env python3
"""RED probe: r12 must reject REMOVED without a cleanup intent."""
from __future__ import annotations

import importlib.util
import sys
import types
import unittest
from pathlib import Path

# The fallback runs the frozen r12 classifier until the replacement exists;
# the import stub isolates semantic replay and does not claim schema validation.
jsonschema_stub = types.ModuleType("jsonschema")
jsonschema_stub.Draft202012Validator = type(
    "Draft202012Validator",
    (),
    {"__init__": lambda self, schema: None, "validate": lambda self, item: None},
)
sys.modules.setdefault("jsonschema", jsonschema_stub)

LEGACY = Path(__file__).with_name("verify_spec039_v14_r12.py")
spec = importlib.util.spec_from_file_location("legacy_r12", LEGACY)
if not spec or not spec.loader:
    raise RuntimeError("could not load frozen r12 classifier for the RED probe")
legacy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(legacy)
try:
    import spec039_methodology_v1 as method
except ImportError:
    method = None

MODEL = {
    "model_id": "ng039-journal-finalization-model",
    "model_version": 1,
    "schema_version": "ng039-reservation-journal-event/v2",
    "journal_states": ["absent", "open", "shutdown_complete", "closed"],
    "journal_initial_state": "absent",
    "journal_terminal_state": "closed",
    "resource_states": ["none", "create_pending", "create_failed", "created", "cleanup_pending", "removed", "cleanup_failed"],
    "resource_initial_state": "none",
    "resource_kinds": {"journal": "journal", "candidates": ["sysv_shm", "posix_semaphore"]},
    "identity_binding": {"sysv_shm": ["candidate_key", "shmid"], "posix_semaphore": ["source_name", "device", "inode"]},
    "required_events": ["JOURNAL_OPENED", "CREATE_INTENT", "CREATED", "CREATE_FAILED", "RUN_SHUTDOWN_COMPLETE", "CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED", "JOURNAL_CLOSED"],
    "events": [
        {"requirement_id": "JRN-001", "event": "JOURNAL_OPENED", "scope": "journal", "phase_from": ["absent"], "phase_to": "open", "resource_from": None, "resource_to": None},
        {"requirement_id": "JRN-002", "event": "CREATE_INTENT", "scope": "resource", "phase_from": ["open"], "phase_to": "open", "resource_from": "none", "resource_to": "create_pending"},
        {"requirement_id": "JRN-003", "event": "CREATED", "scope": "resource", "phase_from": ["open"], "phase_to": "open", "resource_from": "create_pending", "resource_to": "created"},
        {"requirement_id": "JRN-004", "event": "CREATE_FAILED", "scope": "resource", "phase_from": ["open"], "phase_to": "open", "resource_from": "create_pending", "resource_to": "create_failed"},
        {"requirement_id": "JRN-005", "event": "RUN_SHUTDOWN_COMPLETE", "scope": "journal", "phase_from": ["open"], "phase_to": "shutdown_complete", "resource_from": None, "resource_to": None, "requires_no_resource_states": ["create_pending"]},
        {"requirement_id": "JRN-006", "event": "CLEANUP_INTENT", "scope": "resource", "phase_from": ["shutdown_complete"], "phase_to": "shutdown_complete", "resource_from": "created", "resource_to": "cleanup_pending"},
        {"requirement_id": "JRN-007", "event": "REMOVED", "scope": "resource", "phase_from": ["shutdown_complete"], "phase_to": "shutdown_complete", "resource_from": "cleanup_pending", "resource_to": "removed"},
        {"requirement_id": "JRN-008", "event": "CLEANUP_FAILED", "scope": "resource", "phase_from": ["shutdown_complete"], "phase_to": "shutdown_complete", "resource_from": "cleanup_pending", "resource_to": "cleanup_failed"},
        {"requirement_id": "JRN-009", "event": "JOURNAL_CLOSED", "scope": "journal", "phase_from": ["shutdown_complete"], "phase_to": "closed", "resource_from": None, "resource_to": None, "requires_no_resource_states": ["create_pending", "cleanup_pending"]},
    ],
    "authority_modes": {
        "requirement_ids": {"common": "AUTH-001", "normal": "AUTH-002", "abnormal": "AUTH-003", "blocked": "AUTH-004"},
        "common_requirements": ["trusted_launcher", "valid_supervisor_receipt", "all_pids_reaped", "logs_closed", "stable_result_directory", "continuous_lease"],
        "normal_requirements": ["durable_shutdown_complete", "normal_waits_valid", "no_timeout_or_signal"],
        "mode_names": {"normal": "normal_cleanup", "abnormal": "abnormal_evidence_only", "blocked": "blocked"},
    },
    "mode_effects": {
        "normal_cleanup": {"requirement_id": "AUTH-002", "status": ["PASS", "FAIL", "INCONCLUSIVE"], "requires": ["full_bundle_schema_journal_replay", "complete_inventory_verification", "exact_journal_authorized_cleanup"], "forbidden": ["cleanup_before_shutdown_complete", "broad_host_cleanup"]},
        "abnormal_evidence_only": {"requirement_id": "AUTH-003", "status": ["INCONCLUSIVE"], "requires": ["common_authority_gates_true", "strict_pending_record_supervisor_receipt", "bounded_raw_evidence_preservation"], "forbidden": ["live_ipc_inspection", "cleanup", "journal_mutation", "retry", "PASS", "FAIL"]},
        "blocked": {"requirement_id": "AUTH-004", "status": ["BLOCKED", "INCONCLUSIVE"], "requires": ["preserve_pending_evidence"], "forbidden": ["finalizer_entry", "finalizer_reentry", "cleanup", "unsafe_record_write"]},
    },
    "cleanup_outcomes": {
        "not_started": {"requirement_id": "COUT-001", "journal_required": "complete_transition_valid_closed", "created_count": 0, "cleanup_intent_count": 0, "create_failed_allowed": True, "run_status_effect": "NO_PASS_INFERENCE"},
        "partial": {"requirement_id": "COUT-002", "journal_required": "complete_transition_valid_closed", "all_created_have_one_terminal_outcome": True, "cleanup_failed_count_min": 1, "run_status_effect": "INCONCLUSIVE"},
        "complete": {"requirement_id": "COUT-003", "journal_required": "complete_transition_valid_closed", "created_count_min": 1, "all_created_removed": True, "cleanup_failed_count": 0, "run_status_effect": "DOES_NOT_IMPLY_PASS"},
        "unknown": {"requirement_id": "COUT-004", "journal_required": "otherwise", "run_status_effect": "INCONCLUSIVE"},
    },
    "normal_cleanup_failure": {
        "requirement_id": "FIN-001",
        "stop_destructive_calls_after_failure": True,
        "retry_after_intent": False,
        "outcome_source": "complete_transition_valid_closed_journal",
        "complete_history_after_journal_closed_later_evidence_failure": {"cleanup_outcome": "complete", "run_status": "INCONCLUSIVE"},
        "authority_loss": {"finalizer_entry": False, "finalizer_reentry": False, "preserve_pending": True, "status": "BLOCKED/INCONCLUSIVE"},
    },
}


def event(
    sequence: int,
    name: str,
    resource: str = "journal",
    key: int | None = None,
    source: str | None = None,
    error: int | None = None,
    identity: dict[str, object] | None = None,
    run_id: str = "NG039-D2-0123456789",
) -> dict[str, object]:
    return {
        "schema_version": "ng039-reservation-journal-event/v2",
        "run_id": run_id,
        "sequence": sequence,
        "event": name,
        "monotonic_ns": sequence,
        "pid": 2345,
        "uid": 1001,
        "resource_type": resource,
        "candidate_key": key,
        "source_name": source,
        "error_number": error,
        "identity": identity,
    }


def record(name: str, resource: str = "journal", key: int | None = None, source: str | None = None, error: int | None = None, identity: dict[str, object] | None = None) -> tuple[object, ...]:
    return name, resource, key, source, error, identity


def shm_identity(key: int, shmid: int) -> dict[str, int]:
    return {"candidate_key": key, "shmid": shmid}


def semaphore_identity(name: str, device: int, inode: int) -> dict[str, object]:
    return {"source_name": name, "device": device, "inode": inode}


def journal(*records: tuple[object, ...]) -> list[dict[str, object]]:
    return [event(i, *record) for i, record in enumerate(records, 1)]


def classify(events: list[object]) -> str:
    if method is None:
        return legacy.classify(events, {})
    return method.classify_journal(events, MODEL)


def mode(gates: dict[str, object], model: dict[str, object] = MODEL) -> str:
    if method is None or not hasattr(method, "select_finalization_mode"):
        return "abnormal_evidence_only"
    return method.select_finalization_mode(gates, model)


class FinalizationModeTests(unittest.TestCase):
    def setUp(self) -> None:
        self.common = {key: True for key in MODEL["authority_modes"]["common_requirements"]}
        self.normal = {key: True for key in MODEL["authority_modes"]["normal_requirements"]}

    def test_lost_lease_blocks_instead_of_selecting_abnormal_mode(self) -> None:
        gates = {**self.common, **self.normal, "continuous_lease": False}
        self.assertEqual("blocked", mode(gates))

    def test_missing_supervisor_proof_blocks_both_modes(self) -> None:
        gates = {**self.common, "valid_supervisor_receipt": None}
        self.assertEqual("blocked", mode(gates))

    def test_common_authority_without_normal_proof_selects_abnormal_evidence_only(self) -> None:
        self.assertEqual("abnormal_evidence_only", mode(self.common))

    def test_all_common_and_normal_prerequisites_select_normal_cleanup(self) -> None:
        self.assertEqual("normal_cleanup", mode({**self.common, **self.normal}))

    def test_invalid_authority_model_blocks_mode_selection(self) -> None:
        authority = {
            **MODEL["authority_modes"],
            "common_requirements": [key for key in MODEL["authority_modes"]["common_requirements"] if key != "continuous_lease"],
        }
        invalid_model = {**MODEL, "authority_modes": authority}
        gates = {key: True for key in authority["common_requirements"]}
        self.assertEqual("blocked", mode(gates, invalid_model))


class JournalTransitionTests(unittest.TestCase):
    def test_removed_without_cleanup_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_no_created_resources_is_not_started(self) -> None:
        events = journal(record("JOURNAL_OPENED"), record("RUN_SHUTDOWN_COMPLETE"), record("JOURNAL_CLOSED"))
        self.assertEqual("not_started", classify(events))

    def test_failed_creation_without_cleanup_is_not_started(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATE_FAILED", "sysv_shm", 11, error=12),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("not_started", classify(events))

    def test_successful_cleanup_is_complete(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("complete", classify(events))

    def test_failed_cleanup_is_partial_only_when_all_created_resources_are_accounted(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CREATE_INTENT", "sysv_shm", 12),
            record("CREATED", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CLEANUP_FAILED", "sysv_shm", 11, error=16, identity=shm_identity(11, 77)),
            record("CLEANUP_INTENT", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("REMOVED", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("partial", classify(events))

    def test_created_resource_with_no_cleanup_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_uncovered_created_resource_is_unknown_even_if_another_was_removed(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CREATE_INTENT", "sysv_shm", 12),
            record("CREATED", "sysv_shm", 12, identity=shm_identity(12, 78)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_removed_cannot_consume_create_intent_after_create_failed(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATE_FAILED", "sysv_shm", 11, error=12),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_created_resource_cannot_be_removed_without_cleanup_intent(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_intent_before_shutdown_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_create_intent_after_shutdown_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_shutdown_with_open_create_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_intent_without_terminal_outcome_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_posix_semaphore_identity_uses_name_not_key(self) -> None:
        name = "/ng039-test-sem"
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "posix_semaphore", source=name),
            record("CREATED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("REMOVED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("complete", classify(events))

    def test_posix_inode_mismatch_is_unknown(self) -> None:
        name = "/ng039-test-sem"
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "posix_semaphore", source=name),
            record("CREATED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 81)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 82)),
            record("REMOVED", "posix_semaphore", source=name, identity=semaphore_identity(name, 1, 82)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_intent_identity_must_match_created_resource(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 78)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 78)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_result_identity_must_match_created_resource(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 78)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_created_resource_requires_identity_evidence(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11),
            record("REMOVED", "sysv_shm", 11),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_created_without_create_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_create_failed_without_create_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_FAILED", "sysv_shm", 11, error=12),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_identity_key_must_match_top_level_candidate_key(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(12, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_duplicate_cleanup_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_duplicate_cleanup_terminal_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_INTENT", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("REMOVED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_cleanup_failed_without_cleanup_intent_is_unknown(self) -> None:
        events = journal(
            record("JOURNAL_OPENED"),
            record("CREATE_INTENT", "sysv_shm", 11),
            record("CREATED", "sysv_shm", 11, identity=shm_identity(11, 77)),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("CLEANUP_FAILED", "sysv_shm", 11, error=16, identity=shm_identity(11, 77)),
            record("JOURNAL_CLOSED"),
        )
        self.assertEqual("unknown", classify(events))

    def test_missing_shutdown_or_events_after_close_are_unknown(self) -> None:
        missing_shutdown = journal(record("JOURNAL_OPENED"), record("JOURNAL_CLOSED"))
        after_close = journal(
            record("JOURNAL_OPENED"),
            record("RUN_SHUTDOWN_COMPLETE"),
            record("JOURNAL_CLOSED"),
            record("REMOVED", "sysv_shm", 11),
        )
        self.assertEqual("unknown", classify(missing_shutdown))
        self.assertEqual("unknown", classify(after_close))

    def test_sequence_time_and_run_identity_corruption_are_unknown(self) -> None:
        valid = journal(record("JOURNAL_OPENED"), record("RUN_SHUTDOWN_COMPLETE"), record("JOURNAL_CLOSED"))
        bad_sequence = [dict(item) for item in valid]
        bad_sequence[1]["sequence"] = 4
        bad_time = [dict(item) for item in valid]
        bad_time[2]["monotonic_ns"] = 0
        bad_run = [dict(item) for item in valid]
        bad_run[2]["run_id"] = "different-run"
        for invalid in (bad_sequence, bad_time, bad_run, [], ["not-an-event"]):
            self.assertEqual("unknown", classify(invalid))


if __name__ == "__main__":
    unittest.main(verbosity=2)
```

## FILE: Specs/RESULTS-SPEC-039/r13-tool-snapshot/test_spec039_projection_checker.py

Role: Historical synthetic checker/fixture context; not D2 runtime evidence.
Bytes: 11417
SHA-256: 6a5af4a2927ce7bea5c8c7d7963a2e666721803473e8066cfc822206618304d5

````text
#!/usr/bin/env python3
"""Tests for structured SPEC/manifest/packet contract projection checks."""
from __future__ import annotations

import copy
import json
import unittest

import test_spec039_journal_transitions as transitions

try:
    import spec039_methodology_v1 as method
except ImportError:
    method = None

MODEL = transitions.MODEL
BEGIN = "<!-- NG039-JOURNAL-MODEL-BEGIN -->"
END = "<!-- NG039-JOURNAL-MODEL-END -->"
PINS_BEGIN = "<!-- NG039-ARTIFACT-PINS-BEGIN -->"
PINS_END = "<!-- NG039-ARTIFACT-PINS-END -->"
HEAD = "756f493b7049434ab67cf2382486a48f606a470c"
SPEC_SHA = "a" * 64
MANIFEST_SHA = "b" * 64


def check(spec_model: dict[str, object], manifest_model: dict[str, object], packet_model: dict[str, object]) -> list[str]:
    if method is None or not hasattr(method, "check_model_projections"):
        return []
    return method.check_model_projections(spec_model, manifest_model, packet_model)


def extract(markdown: str) -> dict[str, object]:
    if method is None or not hasattr(method, "extract_model"):
        return {}
    return method.extract_model(markdown)


def strict_loads(text: str) -> object:
    if method is None or not hasattr(method, "strict_json_loads"):
        return json.loads(text)
    return method.strict_json_loads(text)


def model_block(raw_json: str) -> str:
    return f"{BEGIN}\n```json\n{raw_json}\n```\n{END}"


def pins_block(pins: dict[str, str]) -> str:
    return f"{PINS_BEGIN}\n```json\n{json.dumps(pins, sort_keys=True)}\n```\n{PINS_END}"


def manifest_fixture() -> dict[str, object]:
    outcomes = copy.deepcopy(MODEL["cleanup_outcomes"])
    mode_projection = {"model_id": MODEL["model_id"], "authority_modes": copy.deepcopy(MODEL["authority_modes"]), "mode_effects": copy.deepcopy(MODEL["mode_effects"])}
    normal_failure = copy.deepcopy(MODEL["normal_cleanup_failure"])
    enum_values = sorted(outcomes)
    boundary = {
        "mode_selection": copy.deepcopy(mode_projection),
        "failure_rule": copy.deepcopy(normal_failure),
        "normal_cleanup_failure": copy.deepcopy(normal_failure),
        "cleanup_outcome_classifier": copy.deepcopy(outcomes),
        "abnormal_evidence_only": {"cleanup_outcome_classifier": copy.deepcopy(outcomes)},
    }
    return {
        "review_revision": "r13",
        "spec": {"sha256": SPEC_SHA, "version": "v1.15"},
        "source": {"document_checkout_head": HEAD},
        "review_artifacts": {"submission_state": "UNSUBMITTED_CONSENT_REQUIRED"},
        "local_validation_this_freeze": {"astra_submitted": False, "owner_exact_sha_consent_for_r13": False},
        "classification_rules": {
            "journal_transition_model": copy.deepcopy(MODEL),
            "cleanup_outcome_projection": outcomes,
            "cleanup_outcome_requirement_ids": [outcomes[name]["requirement_id"] for name in ("not_started", "partial", "complete", "unknown")],
            "authority_mode_projection": copy.deepcopy(MODEL["authority_modes"]),
            "mode_effect_projection": copy.deepcopy(MODEL["mode_effects"]),
        },
        "evidence_schemas": {
            "reservation_journal_event": {
                "properties": {
                    "event": {"enum": list(MODEL["required_events"])},
                    "identity": {"properties": {field: {} for fields in MODEL["identity_binding"].values() for field in fields}},
                }
            },
            "run_record": {
                "properties": {
                    "spec_sha256": {"const": SPEC_SHA},
                    "failure": {"oneOf": [{"type": "null"}, {"properties": {"cleanup_outcome": {"enum": enum_values}}}]},
                },
                "allOf": [{}, {"then": {"properties": {"failure": {"properties": {"cleanup_outcome": {"enum": enum_values}}}}}}],
            }
        },
        "sandbox": {
            "finalizer_mode_selection": copy.deepcopy(mode_projection),
            "finalizer_ipc_boundary": copy.deepcopy(boundary),
        },
        "semantic_finalizer_contract": {
            "normal_cleanup_failure": copy.deepcopy(normal_failure),
            "abnormal_evidence_only": {"cleanup_outcome": copy.deepcopy(outcomes)},
            "finalizer_ipc_boundary": copy.deepcopy(boundary),
        },
    }


def audit(manifest: dict[str, object], pins: dict[str, str] | None = None) -> list[str]:
    if method is None or not hasattr(method, "audit_contract"):
        return []
    if pins is None:
        pins = {"document_head": HEAD, "spec_sha256": SPEC_SHA, "manifest_sha256": MANIFEST_SHA}
    spec_text = model_block(json.dumps(MODEL, indent=2))
    packet_text = pins_block(pins) + "\\n" + model_block(json.dumps(MODEL, indent=2))
    return method.audit_contract(spec_text, manifest, packet_text, SPEC_SHA, MANIFEST_SHA, HEAD)


class ProjectionCheckerTests(unittest.TestCase):
    def test_equal_projections_are_accepted(self) -> None:
        self.assertEqual([], check(MODEL, MODEL, MODEL))

    def test_manifest_projection_drift_is_reported(self) -> None:
        stale = copy.deepcopy(MODEL)
        stale["events"][6]["resource_from"] = "create_pending"
        errors = check(MODEL, stale, MODEL)
        self.assertTrue(errors, "a stale manifest transition must be detected")

    def test_packet_projection_drift_is_reported(self) -> None:
        stale = copy.deepcopy(MODEL)
        stale["cleanup_outcomes"]["complete"]["run_status_effect"] = "PASS"
        errors = check(MODEL, MODEL, stale)
        self.assertTrue(errors, "a packet cleanup-outcome contradiction must be detected")

    def test_duplicate_requirement_ids_are_reported(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["events"][1]["requirement_id"] = invalid["events"][0]["requirement_id"]
        errors = check(invalid, invalid, invalid)
        self.assertTrue(errors, "requirement IDs must be unique")

    def test_unknown_model_field_is_rejected(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["cleanup_outcomes"]["complete"]["status_override"] = "PASS"
        self.assertTrue(check(invalid, invalid, invalid))

    def test_cleanup_outcome_disposition_set_must_be_closed(self) -> None:
        invalid = copy.deepcopy(MODEL)
        del invalid["cleanup_outcomes"]["partial"]
        self.assertTrue(check(invalid, invalid, invalid))

    def test_transition_table_cannot_reuse_creation_state_for_removal(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["events"][6]["resource_from"] = "create_pending"
        self.assertTrue(check(invalid, invalid, invalid))

    def test_common_authority_requirements_are_mandatory(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["authority_modes"]["common_requirements"].remove("continuous_lease")
        self.assertTrue(check(invalid, invalid, invalid))

    def test_cleanup_history_cannot_authorize_pass(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["cleanup_outcomes"]["complete"]["run_status_effect"] = "PASS"
        self.assertTrue(check(invalid, invalid, invalid))

    def test_abnormal_mode_cannot_allow_cleanup_or_ipc(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["mode_effects"]["abnormal_evidence_only"]["forbidden"].remove("cleanup")
        self.assertTrue(check(invalid, invalid, invalid))

    def test_normal_mode_requires_closed_status_set(self) -> None:
        invalid = copy.deepcopy(MODEL)
        invalid["mode_effects"]["normal_cleanup"]["status"] = ["PASS"]
        self.assertTrue(check(invalid, invalid, invalid))

    def test_model_block_is_extracted_as_structured_json(self) -> None:
        text = model_block(json.dumps(MODEL, indent=2))
        self.assertEqual(MODEL, extract(text))

    def test_duplicate_model_blocks_are_rejected(self) -> None:
        text = model_block(json.dumps(MODEL)) + "\\n" + model_block(json.dumps(MODEL))
        with self.assertRaises(ValueError):
            extract(text)

    def test_duplicate_json_keys_are_rejected(self) -> None:
        with self.assertRaises(ValueError):
            strict_loads('{"event":"A","event":"B"}')

    def test_missing_model_block_is_rejected(self) -> None:
        with self.assertRaises(ValueError):
            extract("no canonical model here")

    def test_complete_artifact_bindings_are_accepted(self) -> None:
        self.assertEqual([], audit(manifest_fixture()))

    def test_manifest_spec_digest_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["spec"]["sha256"] = "c" * 64
        self.assertTrue(audit(manifest))

    def test_schema_spec_digest_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["evidence_schemas"]["run_record"]["properties"]["spec_sha256"]["const"] = "c" * 64
        self.assertTrue(audit(manifest))

    def test_packet_manifest_pin_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        pins = {"document_head": HEAD, "spec_sha256": SPEC_SHA, "manifest_sha256": "c" * 64}
        self.assertTrue(audit(manifest, pins))

    def test_cleanup_outcome_schema_enum_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["evidence_schemas"]["run_record"]["properties"]["failure"]["oneOf"][1]["properties"]["cleanup_outcome"]["enum"].remove("complete")
        self.assertTrue(audit(manifest))

    def test_nested_cleanup_projection_drift_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["semantic_finalizer_contract"]["finalizer_ipc_boundary"]["abnormal_evidence_only"]["cleanup_outcome_classifier"]["complete"]["run_status"] = "PASS"
        self.assertTrue(audit(manifest))

    def test_stale_review_revision_is_reported(self) -> None:
        manifest = manifest_fixture()
        manifest["review_revision"] = "r12"
        self.assertTrue(audit(manifest))

    def test_manifest_consent_or_submission_flag_is_rejected(self) -> None:
        manifest = manifest_fixture()
        manifest["local_validation_this_freeze"]["astra_submitted"] = True
        self.assertTrue(audit(manifest))
        manifest = manifest_fixture()
        manifest["local_validation_this_freeze"]["owner_exact_sha_consent_for_r13"] = True
        self.assertTrue(audit(manifest))

    def test_journal_schema_event_enum_matches_model(self) -> None:
        manifest = manifest_fixture()
        manifest["evidence_schemas"]["reservation_journal_event"]["properties"]["event"]["enum"].remove("CLEANUP_FAILED")
        self.assertTrue(audit(manifest))

    def test_identity_binding_fields_exist_in_journal_schema(self) -> None:
        manifest = manifest_fixture()
        del manifest["evidence_schemas"]["reservation_journal_event"]["properties"]["identity"]["properties"]["shmid"]
        self.assertTrue(audit(manifest))

    def test_authority_mode_projection_matches_model(self) -> None:
        manifest = manifest_fixture()
        manifest["classification_rules"]["authority_mode_projection"]["common_requirements"].remove("continuous_lease")
        self.assertTrue(audit(manifest))

    def test_mode_effect_projection_matches_model(self) -> None:
        manifest = manifest_fixture()
        manifest["classification_rules"]["mode_effect_projection"]["abnormal_evidence_only"]["forbidden"].remove("cleanup")
        self.assertTrue(audit(manifest))


if __name__ == "__main__":
    unittest.main(verbosity=2)
````

## FILE: Specs/RESULTS-SPEC-039/r13-tool-snapshot/snapshot-manifest.json

Role: Historical synthetic checker/fixture context; not D2 runtime evidence.
Bytes: 1314
SHA-256: c4846cb9c9faef42b33224e858bd80fa74a6643233a2a201fb97d17408ac9fbb

```text
{
  "archive_type": "immutable-r13-tool-snapshot/v1",
  "r13_packet_sha256": "0ce8ded82a76281c729ae0bf111d4cb49c7e763490ef8ea221b1c0e4f3686727",
  "r13_receipt_sha256": "f5c3fa5aef02e1c5343079eec706ef9df094630668a74ff05b4433bb6bc224b1",
  "files": {
    "spec039_methodology_v1.py": {
      "source": "/home/gandalf/CodeRepository/infra-novagenesis-tools/spec039_methodology_v1.py",
      "snapshot": "Specs/RESULTS-SPEC-039/r13-tool-snapshot/spec039_methodology_v1.py",
      "sha256": "1066196faf1484d646feaaa2b00fe2a1267d731fe42d93a03fb45488f75fd581",
      "mode": "0o600"
    },
    "test_spec039_journal_transitions.py": {
      "source": "/home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_journal_transitions.py",
      "snapshot": "Specs/RESULTS-SPEC-039/r13-tool-snapshot/test_spec039_journal_transitions.py",
      "sha256": "41397e374e3a3e98f159334e606ad2c77b8136e0728dc314cd989079ebd36668",
      "mode": "0o600"
    },
    "test_spec039_projection_checker.py": {
      "source": "/home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_projection_checker.py",
      "snapshot": "Specs/RESULTS-SPEC-039/r13-tool-snapshot/test_spec039_projection_checker.py",
      "sha256": "6a5af4a2927ce7bea5c8c7d7963a2e666721803473e8066cfc822206618304d5",
      "mode": "0o600"
    }
  }
}
```
