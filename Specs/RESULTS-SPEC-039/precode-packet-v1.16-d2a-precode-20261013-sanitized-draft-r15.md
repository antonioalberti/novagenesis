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
