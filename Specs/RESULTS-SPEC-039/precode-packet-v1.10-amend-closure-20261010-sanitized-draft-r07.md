# SPEC-039 v1.10 — r07 working amendment after Astra AMEND

Status: two prior r07 snapshots were submitted and returned AMEND. This same-file working amendment fixes B1 (literal checker argv mapping), B2 (joined `-include`/`-imacros` rejection), and B3 (manifest/run-record SPEC identity); it is not submitted. Documentary pre-code review only. No D2 implementation, configure/build, harness execution, IPC/VM, D3, commit or push is authorized. Date: 2026-10-11.

## Frozen identity and privacy

- Document repository: AIOPT5 at immutable HEAD `756f493b7049434ab67cf2382486a48f606a470c`, tree `9adf665c0eb06213a29a6c12d650451948c7a4ab`. The worktree is dirty; the exact SPEC working-tree SHA is frozen below. No unrelated SPEC-062 diff is included in this review scope.
- Functional baseline: commit `1af604dad66148618cc0e578c28adb4a9829e607`, tree `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`; code claims are based on those immutable Git objects only.
- SPEC-039 v1.10 working-tree SHA-256: `f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26`.
- Manifest working-tree SHA-256: `1a90346598f5799565bf1f39610ccb45f43afdb8c7acba08d82616e747a6a70d`.
- Synthetic validation remains a separate sidecar and is not embedded or hash-pinned here, avoiding a packet/receipt hash cycle; its current digest is recorded in NG-039/NG-040 after this packet is frozen.
- The packet is sanitized: workstation roots are stable aliases; no credentials or personal identifiers are included. NovaGenesis project/build/result paths are under the workspace/repository roles, never `.hermes`.

## Prior review and current authority boundary

The prior r06 packet returned **AMEND**. The submitted r07 snapshot SHA-256 `f6fa7f178d433c6b1ccdf86bb2a915ae4acf1c567ae8b428ad351c524681cb15` returned **AMEND**; its parsed response is `Specs/RESULTS-SPEC-039/astra-review-r07-20261010.md` SHA-256 `9aebba801166f875b9c6e030923450a4e28122852fb8d4e0cc13fe7a198088ab`. The B1/B2 follow-up review also returned **AMEND** solely because the manifest and run-record schema still bound the former SPEC digest; its parsed response is `Specs/RESULTS-SPEC-039/astra-review-r07-b1b2-20261011.md` SHA-256 `edd29c9967af7c91778f028cf7405c46ee06ef68875c1ebd95042d044a1bb212`. This working amendment resolves B1/B2 and the B3 provenance binding. Both prior consents are consumed; fresh consent tied to this amended packet's final SHA is required before any Astra call.

## D1-H evidence boundary — compile/link only

The original recorded D1-H binary and build log remain unavailable for independent rehash. A new build-only reconstruction was performed from the same isolated baseline worktree after verifying the recorded CMake/harness source hashes. CMake configure and target build/link both exited 0; compiler GNU C++ 13.3.0, CMake 3.28.3. The new binary SHA-256 is `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`; configure/build logs and the receipt are bound in the embedded manifest. The executable was not run. This is new compile/link evidence, not recovery of the original binary and not Q0/Q1 correctness or performance evidence.

## r06 findings and bounded r07 response

1. **Build-root/source-root containment and exact `-c`:** the purpose-specific compile-database checker receives the trusted worktree and build roots, validates source/working/output paths and metadata, and requires exactly one `-c`. It does not claim to be a universal command parser.
2. **Environment:** configure, build, checkers and finalizer use a minimal explicit `env -i` set (`PATH`, `HOME`, `TMPDIR`, `LANG`, `LC_ALL`); inherited variables are absent by construction. The exact expanded values and working directories are recorded.
3. **Link-file binding/forwarding:** the checker opens the named expected `link.txt` itself, verifies its target path, pinned compiler and exact standalone static flags, and rejects all `-Wl`/`-Xlinker` forms rather than implementing a general forwarding normalizer.
4. **Schema/finalizer consistency:** JSON Schema enforces shape and simple local PASS/FAIL conditions; the same allowlisted binary’s finalizer recomputes cross-artifact hashes, identities, inventory summaries, observer correlations, Q0/Q1 predicates and failure summaries. Malformed records that falsely claim PASS/FAIL must finalize as INCONCLUSIVE, never be accepted.

This is the owner-directed relaxation: keep checks that prevent wrong-root builds, bypassed observer macros, unbound linker evidence and false PASS; drop a generic compiler/linker grammar, exhaustive environment denylist and duplicate enforcement of cross-artifact semantics in both schema and finalizer. The D2 gate remains one isolated Q0/Q1 correctness run with the existing five-file code allowlist; D3 and Q2–Q4 stay deferred.

## Questions for advisory review

1. Do the literal checker entrypoints now consume the intended arguments without an unconsumed `--`, including missing-operand rejection?
2. Do the joined `-include` and `-imacros` rejections close the observer-macro bypass without restoring a generic compiler grammar?
3. Is any remaining issue acceptance-breaking for this narrow documentary contract, rather than optional hardening?

No implementation, execution, or operational approval is requested or implied.

## Complete governing SPEC-039 v1.10 snapshot

===== FILE: Specs/SPEC-039-common-performance-measurement.md | SHA-256 f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26 =====
# SPEC-039: Gateway Measurement Baseline on AIOPT5

**Version:** v1.10
**Date:** 2026-10-10
**Author:** Hermes; owner-requested gate amendment
**Status:** In Progress
**Gate:** v1.1 D0 approved for Q0/Q1; D1-H compile/link recorded PASS; v1.2 pre-code review returned Astra HOLD; v1.4 packet returned Astra AMEND; v1.5 r03, v1.6 r04 and v1.7 r05 returned Astra AMEND; v1.8 is the same-SPEC documentary response; v1.9 is a privacy-only path redaction; r06 packet SHA-256 `cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711` returned Astra AMEND (response `resp_0e96e100afdf6c5d016aca6a11615c81919e900a168d341cf9`); v1.10 is a same-SPEC documentary response, unreviewed; D2 implementation/runtime remain blocked; D3 not authorised
**Branch:** AIOPT5
**Baseline commit:** `1af604dad66148618cc0e578c28adb4a9829e607`
**D0-approved v1.1 draft SHA-256:** `8026e1320149c318a5218eaa36c730f262081a5bbe0c2d582b23c307c7144e1c` (owner approval: 2026-10-04; documentary Q0/Q1 scope only; does not approve this v1.10 amendment)
**Owner task:** NG-040, under NG-039
**Related:** `SPEC-032-gw-input-queue-batching.md`, `SPEC-033-message-commandline-refactor.md`

> v1.8 retained the v1.1 Q0/Q1 conditions and the separately approved D2 scope: one compile-time-gated Common test seam and one isolated correctness run, without an algorithm change. The r03, r04, r05 and r06 packets returned AMEND. This v1.10 response keeps the same SPEC number, D2 acceptance objective and five-file code allowlist, while narrowing the checker and schema work to the concrete r06 counterexamples. The r06 review was advisory textual inspection only; it did not execute checkers or authorize implementation. D2 remains blocked pending a fresh review of a frozen exact-SHA packet and a separate owner execution decision; D3 remains unauthorised.

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

A run is one fresh, isolated local process start with one GW consumer and a fixed, recorded workload. Requests/messages within the same run are not independent repetitions. The initial Q0/Q1 proposal is five independent runs per condition, each with 30 s warm-up, 120 s measured interval, and up to 30 s bounded drain; the owner may revise these constants before authorising execution, but they must then be frozen in a manifest. Never reuse dirty caches, live trial locks, or an existing SysV IPC namespace.

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
2. **D1 — Local preparation:** pin the exact functional checkout, define the real production-linked fixture, specify a disposable resource namespace, freeze commands/manifest, and obtain scoped authorization for any test-only code/build. Preserve the dirty primary AIOPT5 tree; do not transplant its unrelated changes.
3. **D1-H — Compile/link probe:** owner-authorised source/build-only probe in an isolated checkout at `1af604dad66148618cc0e578c28adb4a9829e607`. It passed, but the executable was not run. This proves symbol/type linkage only—not GW construction, Q0/Q1 behavior, correctness, or performance.
4. **D2a — Correctness oracle and testability:** the owner authorised a compile-time-gated test seam in `Common` and one local D2 correctness run, with no algorithm change, D3 timing, `PGCS`/`PG`, VM, commit or push. Freeze the exact files, oracle, command, isolated resource namespace and cleanup procedure in the pre-code packet. The current public API does not expose queue contents or pending/execution state; `GW::Gateway()` starts an output worker and touches IPC. For Q1, construct one Process-owned `Message` with an in-memory future `Time` and offer it through the real public `GW::PushToInputQueue` path. Do **not** use `NGAL_CS::DeliverToGateway` for this future-head stimulus: `GW::Gateway()` creates received messages with `NewMessage(0, ...)` (`GW.cpp:614–623`), while wire serialization emits command lines rather than the local scheduling `Time` (`Message.cpp:1599–1602`). The D2 run therefore tests queue admission/scheduling, not NGAL receive/deserialization, wire-format timing, or network delivery. Use three inert, syntactically shaped command lines whose action tuple is not registered in GW; this satisfies `NoCL > 2` (`GW.cpp:241–263`) while ensuring an unexpected early `Run` has no external action.

   The compile-time-gated observer exposes `GWQueueTestObservation` through a test-only scalar-copy accessor; it never retains a `Message*`. The snapshot includes a 64-bit event sequence, validity, one-way Gateway-start/stop state, registered target ID, event message ID, observed queue depth/head identity/head time, actual wait timeout/return, the existing strict due decision and actual pop result, sticky target admission/non-due/pop/Run entry/return counters, monotonic empty-queue count, overflow state, and a first-target-wait record containing its own sequence, target/head identity, head time/depth, timeout, wait return and due result. Value-initialise storage as `GWQueueTestObservation D2Observation{}`; `None` is the zero enum value. `RegisterD2Target` runs once before Gateway start, locks `InputQueueMutex`, rejects ID zero or a started Gateway, resets/binds the observation, and leaves it invalid until the first real event. Set `gateway_started=true` under that mutex before entering the loop; never clear it or allow re-registration. Writers increment sequence, update scalar data and counters, then publish validity; counters saturate and set overflow rather than wrap. At the first real `wait_for` return with the registered target as head, fill the sticky first-target-wait record once; later events cannot overwrite it. At Q0/Q1 start the harness captures sequence/counter baselines and tests for new events before the window ends. Writers at already-locked admission/wait/pop sites use a non-locking helper; Run/stop boundaries take the mutex only outside those scopes. No I/O, callbacks, or test-only symbols/state/calls in the default build.
5. **D2b — Correctness test code/build/run:** only after the v1.10 closure response to r06 is positively reviewed under fresh owner consent and all blocking review conditions are resolved, implement only the frozen allowlist: root `CMakeLists.txt`; `Common/src/GW.h`; `Common/src/GW.cpp`; new `tests/spec039/gw_q0_q1_d2.cpp`; and new `tests/spec039/q0_q1_d2.manifest.json`. Add the separate option `NG_ENABLE_NG039_GW_Q0_Q1_D2` (default `OFF`) and target `ng039_gw_q0_q1_d2`; define `NG_GW_QUEUE_TEST_OBSERVATION` for `Common` and the harness only inside an isolated diagnostic target graph: the option-on root CMake path must declare only `Common` and this harness and return before production application targets, with a configure-time target-census guard rejecting any other target. Keep the existing D1-H harness/manifest as historical evidence and do not overwrite them. Gate observer declarations, storage, methods and production call sites together. Build the D2 target in a fresh isolated checkout at the pinned functional baseline. The manifest must freeze this bounded launch shape (actual paths recorded there): `timeout --signal=TERM --kill-after=5s 20s unshare --user --map-root-user --ipc --mount --pid --propagation private --fork --kill-child --mount-proc <harness>`. Resolve all path aliases before any command. Keep the worktree, build directories, HOME/TMPDIR and run outputs outside `.hermes`; use only the named repository/workspace roots in the reviewed manifest. Run no shell/helper child in the namespace; the harness must create no child processes. If user/IPC/mount/PID namespace setup or the private mount fails, abort—no host-IPC fallback. The harness must make mount propagation private, mount a new tmpfs at `/dev/shm`, and capture pre-run `/proc/sysvipc/{shm,sem,msg}` plus `/dev/shm` inventory, requiring an empty starting inventory. Key `11` still reaches `ReturnIPCSHMID` and the Gateway SHM reader; never assume it eliminates IPC. After stopping/joining Gateway/output threads and destroying Process/GW, record the scoped post-run inventory; remove only SysV IDs and semaphore files absent from the empty pre-run inventory, then record a final empty inventory. Any failed attribution/removal is INCONCLUSIVE; never clean host or foreign IPC. Capture bounded stdout/stderr outside the namespaces, close both before finalization, and let the parent preserve the raw outer status; ambiguous codes/signals are INCONCLUSIVE. Use a separate bounded outer timeout and process-tree kill grace. Keep the D2 run at `N=1`, Q0/Q1 correctness only; no due-work/network/mixed load, throughput/CPU/p99 claim, algorithm change, `PGCS`/`PG`, VM, commit or push unless separately authorized. If a new gate or blocker is added by review, resolve it in this same SPEC and packet before seeking fresh consent.
6. **D3 — Characterisation:** Q0/Q1 timing is a separate gate, allowed only after D2 passes and the owner explicitly authorises the frozen measurement plan. Preserve all run outcomes and identities; classify the future-head hypothesis `MEASURED`, `INCONCLUSIVE`, or `BLOCKED`.
7. **D4 — Follow-up decision:** decide whether to stop, approve another measurement slice, or propose a performance implementation SPEC. Any future change requires a predeclared baseline/metric, actual-source review, correctness controls and its own owner-approved implementation allowlist. Astra or subagent opinions never grant implementation or runtime authority.

### 4.5 GPT-6 Astra consultation checkpoints (advisory; no execution authority)

| Checkpoint | Packet and question | Gate informed |
|---|---|---|
| Before D2 test-seam/code | Exact SPEC revision and SHA, functional baseline SHA, D1-H manifest, proposed observer/interface diff, isolated CMake target graph and complete proposed D2 manifest/resource limits, Q0/Q1 assertions, direct production `PushToInputQueue` stimulus, explicit non-coverage of NGAL/wire-time behavior, IPC/mount namespace and cleanup plan. Ask whether the isolated target graph truly prevents test macro propagation to production consumers, whether the sticky wait/lifetime/cleanup oracles close every claim without private-access hacks, whether the proposed manifest and resource limits are complete, and whether any claim exceeds the owner scope. | The original pre-code review was performed against v1.2 and returned HOLD. The v1.4 packet returned AMEND. The sanitized v1.5 r03 packet SHA-256 `7121fa9307beb790ff093319d6d9bc54fb9d296bf68d8c705647fbcebf740797`, tied to v1.5 SPEC SHA-256 `cf6efdf1dc1bcc7179106e109d3143a5cbe89e8c93cb7725aa34498b5407b6cc` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, returned AMEND on 2026-10-05 (response ID `resp_020e2d49d7eb0fb1016ac3b3b903e487d2891f1ae2407fb075`). The v1.6 r04 and v1.7 r05 reviews also returned AMEND; each one-call consent is consumed. The sanitized v1.9/r06 packet SHA-256 `cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711` returned AMEND on 2026-10-10 (response ID `resp_0e96e100afdf6c5d016aca6a11615c81919e900a168d341cf9`); its one-call consent is consumed. This v1.10 response is unreviewed; any r07 follow-up requires fresh explicit owner consent bound to the exact final r07 packet SHA. Review is advisory and cannot expand the owner-approved scope. |
| After D2 correctness result, before D3 | Frozen diff/build/test manifest, all raw outcomes including failures, identities and cleanup evidence. Ask whether the D2 assertions are supported and what remains inconclusive. | Planned checkpoint; a separate Astra submission requires owner consent. The owner decides whether to accept D2 and authorize D3. |
| After D3, before any implementation SPEC | Frozen raw data, analysis, source/build/workload identities and hypothesis classification. Ask whether the evidence supports a future implementation proposal and its acceptance metric. | Future checkpoint only; separate owner consent is required. Astra advice never authorises production changes. |

The owner authorised one GPT-6 Astra review via OpenAI Codex for the frozen v1.2 pre-code D2 packet. That call returned HOLD on the v1.2 SPEC SHA-256 `8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91` and packet SHA-256 `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`; its consent was consumed. On 2026-10-05 the owner separately authorised one review of the sanitized v1.4 packet SHA-256 `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948`, bound to SPEC v1.4 SHA-256 `f54901cad03209f94fe98cfad3bf62953895ed24fef3d771a89b89f5229217e1` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`. Astra returned AMEND, response ID `resp_0a12ead61fee5339016ac370498edc87d28ec6212dafa33cf9`. The archived raw SSE SHA-256 is `25b8c29265d327c34323e12f2c35177c2069c13b1d7e99f83a42ffea75240a58`; archive manifest SHA-256 is `46f41c9af22a6b1c9e86e2a66682dfd83f7323fb25d1c47aded2aa52c95e6782`. The r03 AMEND is advisory; its one-call consent is consumed and grants no implementation, build, runtime, IPC/VM, or D3 authority. Raw SSE SHA-256 `14f95d55a4a5611f518a0e236c98edd255e6809acd256b105177f975577bef09`; parsed-response SHA-256 `6fae28a46fd385e0d5aa87e8a1bc7149c832fcd8878170b470dab558941bc587`; archive manifest SHA-256 `b26c7767c9a74f2c9ee716a59257f79064c5480191addbc5f61da998989ca5fd`. The packet’s “NOT submitted” line is a time-frozen pre-review snapshot; this paragraph records the later submission/result. The v1.6 r04 packet SHA-256 `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829`, tied to the v1.6 SPEC SHA-256 `3296530c13801de945ed97b920532533c1a4a44d628075a9c5e61cdcc7ee01cb` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_0d91609fd866592e016ac3b3b903e487d2891f1ae2407fb075`). Raw SSE SHA-256 `a1c62af8a4e35a59aec3428fdf45818f2dd89b3f57dce9c3bf8be9a6279fe08d`; parsed-response SHA-256 `3655369b4c6c348e439ec87e48f6a8c1e789c46d5d4500890bee8577b8a969d3`; archive manifest SHA-256 `3f6dbf25e22ffd3ce5ce9290935fd41db162fd81aa67923c445b5ea250fbe2a8`. That exact-SHA consent is consumed. The review closed r03 findings 3 and 4b–4d at design level only; finding 4a remains must-fix. The v1.7 r05 packet SHA-256 `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a`, tied to SPEC v1.7 SHA-256 `b73bcc7361bb797b4ddd40952860f0092fb840bb11b31bf0a99659bef8475b70` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). Raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`; parsed-response SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`; archive manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a`. Usage: 22,569 input / 1,650 output / 24,219 total tokens; 8.11725 estimated credits (not USD); quota at return 5h 51%, weekly 86%. Three 4a must-fixes remain: preprocessor-forwarded and function-like observer macros; `-ansi` dialect override; Python assertions disabled by `PYTHONOPTIMIZE=1`. Optional hardening: actual `-o` target mapping, dynamic-linker conflict rejection, and stronger run-record schema `allOf`. Findings 3 and 4b–4d remain design-level closures only. The r05 consent is consumed. SPEC-039 v1.8 is local/unreviewed; any r06 review requires fresh exact-SHA consent. D2 remains blocked.

No VM/SSH/QGA operation, algorithmic production change, commit, push, or release is authorised by these gates. All Astra reviews to date were separately owner-authorised by exact packet SHA and their one-call consents are consumed; none expands the D2 seam/runtime scope. The old NG-062 trial locks and evidence remain untouched.

## 5. Files and evidence

| Path | State/intent |
|---|---|
| `Specs/SPEC-039-common-performance-measurement.md` | v1.10 same-SPEC response to r06, local and unreviewed; r03/r04/r05/r06 AMENDs remain historical. No D2 implementation/runtime authority is conferred. |
| `Common/src/GW.cpp`, `Common/src/GW.h` | Read-only under D0; a D2 seam may touch only the explicitly frozen compile-time-gated scope in §4. |
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No NGAL/Process/PGCS/PG, algorithm, separate runner file, or D3 timing files are proposed. The same allowlisted harness binary’s `--finalize` mode runs only after the namespaced test exits, outside namespaces, and performs evidence finalization without constructing Process/GW, IPC, or child processes. |
| Isolated D1-H artifacts | Historical manifest `tests/spec039/q0_q1.manifest.json` SHA-256 `9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a`; source file hashes match. A fresh compile/link-only rebuild on 2026-10-10 used the unchanged isolated baseline checkout and an output root under `workspace/ng039-d1h-rebuild-20261010/` (outside `.hermes`): configure/build exit 0, CMake 3.28.3, GNU C++ 13.3.0, binary SHA-256 `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`, configure-log SHA-256 `65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb`, build-log SHA-256 `658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98`. This is a new build, not recovery of the old binary; the executable was not run, and this proves no correctness/performance behavior. |
| `Specs/RESULTS-SPEC-039/` | Contains sanitized r06/r07 pre-code packets and review records plus the D1-H build-only receipt; none is a D2/D3 runtime result. No correctness or timing result exists. Historical AIOPT3 results remain in the NG-050 archive. |
| Obsidian NG-039 / NG-040 | Scope and blockers; neither an experimental oracle nor build evidence. |

## 6. Rollback and open questions

If the approved D0 design is later abandoned, mark this Proposal as abandoned/superseded instead of deleting it; leave the archive, current AIOPT5 diffs and historical trial evidence untouched. Later test-only changes must have an explicit allowlist and a reversible isolated checkout.

Before D2 code: freeze the compile-time-gated observer, exact five-file allowlist, in-memory future-message admission through `PushToInputQueue`, command, IPC/mount namespace and ownership cleanup oracle in the pre-code packet; obtain the authorised GPT-6 Astra review before applying the diff. Q0 distinguishes zero application offers from bootstrap queue state; Q1 uses a future `Message::Time` created locally because wire serialization does not carry that scheduling field. Cleanup must reconcile the Process-owned pending test message only after Gateway/output workers stop. The owner’s D2 scope is one correctness-only local run and does not include NGAL ingress coverage, D3 or algorithm changes. Q2–Q4 remain deferred; do not import approval or measurements from archived AIOPT3 SPECs.

## 7. D2a closure amendment — v1.8–v1.10 (documentary; not implementation authorization)

This section preserves the unchanged D2 correctness scope and responds to the r05 and r06 textual AMENDs. The owner-approved relaxation is deliberate: this is a pinned, local CMake probe, not a general-purpose compiler/linker command security analyser. Verify the paths, compiler identity, language/optimization mode, observer macro, linker file and status evidence that affect this acceptance decision; record the complete generated command and its digest for review, but do not build an exhaustive grammar for unrelated compiler options. The one-shot review consent for r06 is consumed. Preparing this r07 packet does not authorize another Astra call or D2 execution.

1. Sandbox before construction: launch the harness in the frozen user/IPC/mount/PID namespaces; make mount propagation private; mount a new tmpfs at `/dev/shm`; and capture empty SysV IPC and semaphore/file inventories before constructing any `Process`, `HT`, `GW`, or test `Message`. If namespace, mount, or inventory setup fails, abort without fallback to host IPC. No constructor-time IPC may precede the empty pre-inventory.

2. HT construction, Message lifetime, and failure-safe ownership: after sandbox setup, construct the isolated non-`PGCS` `Process`. Its pinned constructor calls `NewBlock("HT", ...)` then `NewBlock("GW", ...)` (`Process.cpp:228–230`), and the `GW` constructor retrieves that HT (`GW.cpp:99–101`) before initialization dereferences it (`GWRunInitialization01.cpp:68–80`). Do not call `NewBlock("HT")` or `NewBlock("GW")` again; verify exactly two blocks exist and one lookup succeeds for each name. Before starting Gateway, create the Process-owned target, capture `GetDeleteFlag()==false` while single-threaded, and bind its ID with the observer. Capture the immutable scalar `Message::InstantiationNumber` (`Message.h:158`) before each production `Run`; use only that copied ID for Run-entry and Run-return events, publishing the return event before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after a possible reclamation; duplicate, zero, stale or wrapped identity is INCONCLUSIVE. The three-argument constructor used by `Process::NewMessage` initializes `Delete=false` and `ApplicationDeleted=DELETED_BY_CORE` (`Message.cpp:37–70`, especially 63–64); `DeleteMarkedMessages` deletes only true-flagged messages (`Process.cpp:885–917`). Keep the target out of the queue and do not mark it through Q0; use exactly three inert command lines `-ng039 --probe 0.2`, which match neither the registered action nor the `0.1` fallback in `Block::Run` (`Block.cpp:244–313`). For the expected Q0/Q1 pending path, after Gateway and its output worker stop/join, verify no target pop/Run and `Delete=false`; call `EraseMessage` only while Process is quiescent and require `GetNumberOfMessages()` to decrease by exactly one. Keep the detached object alive through `Process` destruction: its destructor calls `DeleteMessages()` before deleting `Blocks`, so after EraseMessage it will not own/delete this target; `GW::~GW` destroys actions, while the queue container drops raw pointer values. Delete the detached target exactly once after Process/GW destruction. If any target pop/Run, delete flag, count, or identity makes ownership uncertain, never dereference, erase, delete, or invoke the C++ Process/GW destructors on a possibly stale pointer: write a bounded/fsynced `run-record.pending.json` with `record_finalized=false` and no fabricated measurements, exit nonzero via `_Exit`, and let the isolated process/namespace terminate; after it exits, only the parent-side finalizer in item 5 may produce the final INCONCLUSIVE record, with unavailable post/final inventories explicitly not empty—not PASS. A partial construction or blocked join follows the same fail-closed classification; the outer timeout is the bound if join cannot return.

3. Fresh, correlated observation contract: keep the 64-bit event sequence and add a saturating `gateway_iteration_id`, assigned once at the start of each Gateway loop iteration and carried across its wait and due/pop decision. Preserve two distinct sticky scalar records: (a) while the existing `InputQueueMutex` lock is held when the real `wait_for` returns, if the registered target is the head, capture `WaitCapture{sequence, iteration_id, target_id, head_id, head_time, depth, timeout_ms, predicate_return}` once and never overwrite it; (b) while the production due/pop critical-section lock is held, capture the corresponding `DecisionCapture{sequence, iteration_id, candidate_id, candidate_time, actual_due_branch, actual_popped}` from the existing strict comparison/pop branch, without performing a second comparison, and never overwrite that capture. The records correlate only if the iteration IDs match, `decision.sequence > wait.sequence`, and the wait target/head ID matches the actual decision candidate. Set `correlation_status=MATCHED` only then. Otherwise use `PENDING`, `DECISION_NOT_REACHED`, `HEAD_MISMATCH`, `INVALID`, or `OVERFLOW`; no incomplete pair is exposed as a valid due result. Stop before the decision, a changed head, invalid/duplicate/wrapped ID, missed event, stale sequence, or counter overflow makes Q1 BLOCKED/INCONCLUSIVE, never zero/PASS. Under Q0/Q1, capture sequence/counter baselines at each window start: Q0 requires a fresh empty-observation delta inside its 100 ms no-controlled-offer window after bootstrap drain; Q1 requires target admission/head identity, a matched wait/decision pair newer than the Q1 baseline, zero target pops/Run entries/returns throughout 100 ms, and a future target time at end. Use `std::chrono::steady_clock` for correctness windows; `Message::Time` remains the scheduler clock. Set `gateway_started=true` under `InputQueueMutex` before the loop and never reset it; later registration fails. Before production `Run`, copy `Message::InstantiationNumber` to a scalar; emit Run-entry/return with that copy and publish the return before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after possible reclamation. These are correctness-window rules, not timing/performance evidence.

4. Bounded source/build verification contract: the five-path code allowlist and the CMake target census remain separate checks. The diagnostic graph contains exactly `Common` and `ng039_gw_q0_q1_d2`; the normal graph contains the pinned default targets, with the observer macro absent. The root CMake file returns before production application targets in the diagnostic branch. The compile-database checker receives the database, profile, pinned worktree root and fresh build root as four positional inputs after `python3 -c <checker>`; do not insert a `--` separator unless the checker explicitly consumes it. It verifies that each compile entry's working directory and actual `-o` output resolve under the build root, the source operand and entry `file` resolve to the same regular source inside the worktree, any `output` metadata agrees with `-o`, exactly one `-c` occurs, and compiler realpath/target census match the manifest. Require exactly one `-std=gnu++20`, only `-O0` optimization tokens, and the observer define exactly once on `Common` and the harness in diagnostic mode and nowhere in default mode. Parse joined and split `-D`/`-U` switches by base identifier, including function-like macro spellings; reject every function-like or undefined `NG_GW_QUEUE_TEST_OBSERVATION` form. Reject response files, `-Wp`, `-Xpreprocessor`, `-ansi`, `-traditional`, `-x`, forced-include and macro-file forms in both split (`-include file`, `-imacros file`) and joined (`-includefile`, `-imacrosfile`) spelling, plus toolchain-selection and plugin forms that can bypass those checks. Do not reject unrelated warning/debug switches through a purported universal grammar; preserve every complete argument vector and the compile database SHA-256 as evidence. This checker is scoped to the pinned CMake output and is not a generic validator for arbitrary compile databases.

Resolve the roots before configure. Use fresh, pairwise-disjoint source/build/results paths beneath `<WORKSPACE_ROOT>`; never place NovaGenesis source, build or result artifacts in `.hermes`. Invoke configure, build and both checkers through the same minimal `env -i` environment, retaining only explicit `PATH`, `HOME`, `TMPDIR`, `LANG` and `LC_ALL` values. Set `HOME` and `TMPDIR` to distinct directories under the named D2 scratch root. Record those retained values, command working directories, tool versions and exact commands in the manifest/run identity; inherited environment variables are not passed through. No broad environment-variable inventory or custom denylist is required.

For link validation, invoke `python3 -c <checker> <link.txt> <build-root> ng039_gw_q0_q1_d2` without an unconsumed `--`; the checker opens that exact named `link.txt` itself, requires a regular non-symlink file at the expected diagnostic target path under the build root, and never validates stdin or another caller-supplied string. Parse its contents with `shlex.split(posix=True)`, verify the pinned compiler realpath and exactly one standalone `-static`, `-static-libgcc`, and `-static-libstdc++`, and reject response files plus direct dynamic-link modes. For this target, reject `-Wl` and `-Xlinker` entirely rather than normalizing every forwarding grammar; this rejects `-Wl,-shared` and `-Xlinker -B -Xlinker dynamic` without a general linker-option parser. The checker uses explicit nonzero failures, not Python `assert`.

Freeze only the directly relevant synthetic cases: source/output path escape, mismatched compile metadata, repeated `-c`, wrong compiler, observer macro via `-Wp`/`-Xpreprocessor`, joined `-include`/`-imacros` override, alternate standard or optimization, wrong link-file path/content, missing/duplicate required static flag, `-Wl,-shared`, and the split `-Xlinker` dynamic override. For both literal checker entrypoints, test one complete accepted invocation and a missing-operand rejection, in normal Python and `PYTHONOPTIMIZE=1`; test the two joined forced-include negatives in the same modes. No real D2 configure/build/run is implied by these synthetic tests.

5. Bounded run-record and finalizer contract: the tracked `tests/spec039/q0_q1_d2.manifest.json` retains strict Draft 2020-12 schemas for the pending/final run record, IPC inventory and observer events. The schema enforces record shape, required fields, non-null Q0/Q1 measurements for PASS, the straightforward local implications of PASS/FAIL, unique artifact names by semantic validation, and safe failure fields. The schema does not duplicate every cross-artifact relation. The same allowlisted binary's `--finalize` mode is the single semantic authority for those relations: it strictly parses JSON (reject duplicate keys and non-finite numbers), checks the actual file set, names, sizes and SHA-256 values, validates inventory phase/run IDs and derived empty/size summaries, and correlates observer event sequences, target IDs, wait/decision iteration IDs and counters with the Q0/Q1 assertions. It recomputes the acceptance predicates from recorded observations rather than trusting assertion labels. A PASS requires Q0 zero controlled offers plus a fresh empty-queue observation; Q1 target admission/head identity, a matched fresh wait/decision pair for the same target with a false due branch and no pop, zero target Run entries/returns, and a future head at the window end; plus complete safe cleanup and final empty inventory. A FAIL is allowed only for a directly observed correctness assertion failure when the required artifacts and inventories are complete, cleanup is safe, and no uncertainty remains. Missing, contradictory, uncorrelated or unavailable evidence is INCONCLUSIVE. The finalizer must leave no PASS/FAIL claim if it cannot establish these facts.

The bounded semantic regression set supplies malformed records that falsely claim PASS or FAIL; these are negative inputs, not valid outcome fixtures. For a claimed PASS with a null Q1 decision identity or non-zero target pop count, the finalizer must produce `INCONCLUSIVE` with `failure.code=EVIDENCE_INVALID`, never preserve PASS. A claimed FAIL with unknown/incomplete cleanup must become `INCONCLUSIVE`; a missing required artifact must become `INCONCLUSIVE` with `EVIDENCE_MISSING`. Run-ID, hash, sequence or failure-summary mismatch between the record and its referenced artifacts must become `INCONCLUSIVE` with `EVIDENCE_INVALID`. The finalizer derives `missing_artifacts` from the exact bundle, `inventory_availability` from parsed inventory files, and `cleanup_outcome` from actual cleanup flags/inventories; `code`, `phase` and `last_event_sequence` must correspond to the highest-precedence observed condition. If a supplied failure summary contradicts those recomputed values, it is evidence-invalid and cannot be `FAIL`. Keep separate positive fixtures for valid PASS, safe correctness FAIL, finalized INCONCLUSIVE and pending-record INCONCLUSIVE. Artifact filenames are unique and each required file appears exactly once; the six required artifacts remain `stdout.txt`, `stderr.txt`, `ipc-pre.json`, `ipc-post.json`, `ipc-final.json`, and `observer-events.json`, with only `harness-diagnostics.txt` optional. Preserve current bounded sizes: each artifact ≤4 MiB, final record ≤256 KiB, total bundle ≤32 MiB; `/dev/shm` is a private 16 MiB tmpfs, and SysV SHM is capped at five segments of 33,554,532 bytes each (167,772,660 bytes total). Inventory overflow or failed attribution/removal is INCONCLUSIVE.

The isolated runtime shape remains one harness process, one Q0/Q1 correctness run, no child process, no host fallback, no network input and no performance timing. Establish user/IPC/mount/PID namespaces and private mount propagation before constructing Process/GW; mount a fresh `/dev/shm` tmpfs; require empty pre-inventory; after stop/join, remove only attributable resources and verify final emptiness. Bound execution with the existing 20 s outer timeout plus 5 s kill grace and the recorded resource limits. Preserve raw outer status; do not infer a signal from a shell exit code. These limits are a narrow containment boundary for constructor-time IPC, not a new performance workload.

**Next gate:** the authorised B1/B2 follow-up review found those two items closed but returned AMEND for one concrete provenance defect: the manifest-level SPEC digest and the run-record `spec_sha256` constant still named the prior SPEC bytes. This same-SPEC working amendment aligns both fields to the frozen current SPEC digest; it adds no checker, finalizer or D2 scope. The non-blocking `failure.phase` vocabulary advisory remains deferred. Revalidate the manifest/schema binding, freeze the revised packet in place, and seek fresh explicit owner consent tied to its new SHA before another Astra call. D2 implementation/configure/build/run remains blocked until a positive review and a separate exact-scope owner execution decision; D3 and Q2–Q4 remain deferred. This documentary work is not correctness or performance evidence.

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
| v1.10 | 2026-10-10 | Owner-directed, same-SPEC response to r06 AMEND. Keeps the source/output containment, exact `-c` count, minimal `env -i`, actual `link.txt` binding and no-false-PASS contract; relaxes generic argument grammar, exhaustive environment denylisting and duplicated schema/finalizer rules. Local and unreviewed; r07 requires fresh exact-SHA Astra consent. |
===== END FILE: Specs/SPEC-039-common-performance-measurement.md =====

## Complete machine-readable plan manifest

===== FILE: Specs/RESULTS-SPEC-039/precode-manifest-v1.10-r07.json | SHA-256 1a90346598f5799565bf1f39610ccb45f43afdb8c7acba08d82616e747a6a70d =====
{
  "amendment_response": {
    "optional_r05_hardening": {
      "actual_output_based_target_mapping": "incorporated",
      "conflicting_dynamic_link_controls": "incorporated",
      "schema_level_status_consistency": "incorporated"
    },
    "owner_directed_relaxation": {
      "environment": "use env -i with five explicit retained values; no exhaustive inherited-variable denylist",
      "generic_compile_argument_grammar": "relaxed: validate only provenance and acceptance-critical compiler controls; retain complete argv vectors and database digest for review",
      "linker_forwarding": "reject -Wl/-Xlinker outright; do not implement a forwarding normalizer",
      "schema_vs_finalizer": "schema validates shape/local implications; one finalizer owns cross-artifact semantics",
      "scope": "same SPEC-039, same five-file D2 code allowlist and same Q0/Q1 correctness gate"
    },
    "r05_must_fix_4a": [
      "Reject compiler-option encodings that bypass macro inspection, including -Wp, and -Xpreprocessor; recognize function-like macro names by base identifier.",
      "Reject -ansi and all unsupported/alternate dialect, optimization, response-file, forced-include, toolchain, and driver-option forms; use an explicit supported-argument grammar.",
      "Replace Python assert checks in link validation with explicit nonzero failures that remain active under python3 -O and PYTHONOPTIMIZE=1."
    ],
    "r05_verdict": "AMEND",
    "r06_must_fixes": [
      "compile source/output/build-root containment and exactly one -c",
      "freeze a deterministic environment for configure/build/checkers",
      "read and bind the exact link.txt; reject forwarded linker controls",
      "prevent schema-valid PASS/FAIL contradictions using local schema constraints plus a bounded semantic finalizer"
    ],
    "r06_verdict": "AMEND",
    "r07_b1b2_identity_fix": "In-place correction after Astra B1/B2 review: source plan digest and run-record spec_sha256 const bind the final B3-amended frozen SPEC SHA; no semantic-finalizer or D2 change.",
    "r07_follow_up_scope": "In-place r07 working amendment after archived r07 reviews: B1 direct argv mapping, B2 joined forced-include/macro-file rejection, and B3 alignment of manifest/run-record SPEC identity to the frozen current SPEC. The failure.phase vocabulary advisory remains non-blocking; no D2/finalizer implementation is added.",
    "review_status": "Two prior r07 snapshots were submitted and returned AMEND; this same-file working amendment resolves B1/B2/B3 and is not submitted. Fresh exact-SHA owner consent is required for any follow-up.",
    "v1_10_documentary_response": [
      "Compile checker receives trusted worktree/build roots; validates actual -o/source metadata and exactly one -c; checks compiler, target census, C++20, O0 and macro state.",
      "Direct joined/split observer -D/-U forms, including function-like names, are inspected; forwarded preprocessor, response-file and alternate-dialect bypasses are rejected.",
      "All commands use minimal env -i with explicit PATH/HOME/TMPDIR/LANG/LC_ALL; no inherited variables pass through.",
      "Link checker opens the exact expected regular non-symlink link.txt path; requires pinned compiler and exact static flags; rejects response files and all -Wl/-Xlinker forwarding.",
      "Schema enforces bounded record shape and local PASS/FAIL implications; the same-binary finalizer recomputes cross-artifact hashes, identities, Q0/Q1 observations, inventories and status.",
      "The checker contract is explicitly purpose-specific; it is not represented as a generic security proof for arbitrary compiler/linker commands."
    ],
    "v1_8_documentary_response": [
      "Compile-command target identity comes from the actual -o output, with metadata/path disagreement rejected.",
      "Joined and split -D/-U spellings are parsed; malformed, forwarded, duplicate, conflicting, undefined, and function-like observer-macro forms fail closed.",
      "Require exactly one -std=gnu++20 and only -O0; reject -ansi, -traditional, -x, unknown driver flags, and response files.",
      "Link validation uses shlex tokenization, exact standalone static flags, pinned compiler identity, response-file rejection, and explicit dynamic-link conflict checks.",
      "Run-record schema allOf expresses PASS/FAIL/INCONCLUSIVE cross-field conditions; fixtures cover each conditional."
    ]
  },
  "changed_file_allowlist": [
    "CMakeLists.txt",
    "Common/src/GW.h",
    "Common/src/GW.cpp",
    "tests/spec039/gw_q0_q1_d2.cpp",
    "tests/spec039/q0_q1_d2.manifest.json"
  ],
  "classification_rules": {
    "artifact_names_unique": "The finalizer rejects duplicate names even when artifact hashes differ; each required artifact appears exactly once.",
    "assertion_values": [
      "PASS",
      "FAIL",
      "INCONCLUSIVE",
      "NOT_RUN"
    ],
    "dev_shm_bounds": {
      "max_entries": 128,
      "max_file_bytes": 16777216,
      "sum_bytes_max": 16777216
    },
    "fail_requires": [
      "record_finalized=true and failure.code=ASSERTION_FAILED",
      "at least one correctness assertion FAIL; every safety/inventory/cleanup assertion PASS; no INCONCLUSIVE/NOT_RUN assertion",
      "known harness exit; normal outer code 0..123; signal=null; timeout_fired=false",
      "all six required artifacts exactly once and verified; pre/final empty inventories, complete attributable post inventory and safe cleanup",
      "failure.cleanup_outcome=complete; failure.missing_artifacts=[]; all uncertainty triggers absent",
      "failure code/phase/last-event/availability/cleanup/missing-artifact summary exactly matches recomputed evidence"
    ],
    "failure_safety_assertions": [
      "evidence_complete",
      "final_inventory_complete",
      "final_inventory_empty",
      "finalization_complete",
      "gateway_joined",
      "namespace_setup",
      "output_worker_joined",
      "post_inventory_complete",
      "post_resources_attributable",
      "post_resources_removed",
      "pre_inventory_complete",
      "pre_inventory_empty",
      "process_blocks_verified",
      "scalar_run_identity_valid",
      "target_erased_once",
      "wait_decision_correlated"
    ],
    "failure_summary_consistency": "Finalizer computes all failure fields from validated assertions, events, artifacts and inventories; caller-supplied contradictions map to INCONCLUSIVE/EVIDENCE_INVALID.",
    "inconclusive_precedence": [
      "timeout/signal or ambiguous outer code",
      "missing/invalid stage/final/artifact or duplicate artifact name",
      "finalizer/schema/size/bundle failure",
      "identity/sequence/correlation ambiguity or counter overflow",
      "partial construction or blocked join",
      "incomplete/unattributable post inventory or failed removal",
      "incomplete/nonempty final inventory",
      "contradictory observation/assertion or unavailable required field",
      "any NOT_RUN/INCONCLUSIVE acceptance assertion"
    ],
    "local_fixture_coverage": {
      "invalid_count": 14,
      "invalid_examples": [
        "PASS assertion NOT_RUN",
        "unsafe cleanup",
        "nonempty pre-inventory",
        "incomplete final inventory",
        "unmatched Q1",
        "nonzero PASS exit",
        "missing required artifact",
        "FAIL without false assertion",
        "FAIL with NOT_RUN",
        "incomplete post-inventory",
        "FAIL ambiguous outer code",
        "FAIL unknown harness exit",
        "finalized INCONCLUSIVE without failure",
        "pending with failure claim"
      ],
      "status": "historical v1.8 local result; detailed standalone fixtures were not retained in the current worktree",
      "valid_count": 4
    },
    "observed_exit": {
      "ambiguous_code": "125-127 wrapper/invocation failure and 128-255 without independent signal evidence are INCONCLUSIVE; never infer a unique harness signal from a shell code",
      "outer_code": "raw timeout-wrapper exit code; always preserve it",
      "signal": "only set from an independent unambiguous wait result; otherwise null",
      "timeout_fired": "true on an explicit supervisor timeout; code 124 is treated conservatively as timeout"
    },
    "pass_requires": [
      "record_finalized=true; failure=null; outer_code=0; harness_code=0; signal=null; timeout_fired=false",
      "every required assertion PASS; actual Q0/Q1 values are non-null and satisfy the frozen local conditions",
      "all six required artifact names appear exactly once; actual bytes/sizes/hashes match the record and bundle caps",
      "pre and final inventories are complete, available and empty; post inventory is complete/available and attributable",
      "all cleanup flags true; no counter overflow; Q1 wait/decision identity and sequence correlation independently recomputed as MATCHED"
    ],
    "post_inventory": "Complete and attributable; may be non-empty if it contains only objects absent from empty pre; remove them and require complete empty final.",
    "required_artifact_names": [
      "stdout.txt",
      "stderr.txt",
      "ipc-pre.json",
      "ipc-post.json",
      "ipc-final.json",
      "observer-events.json"
    ],
    "run_id_uniqueness": "unique per execution; compare across records.",
    "semantic_finalizer": "The finalizer recomputes evidence completeness, artifact hashes, inventory summaries, event sequence/counter consistency, Q0/Q1 correlations and overall status; schema-valid assertion labels alone are never acceptance.",
    "staged_and_final_inconclusive": {
      "finalized": "status=INCONCLUSIVE and failure is a bounded object",
      "not_finalized": "status=INCONCLUSIVE and failure=null; no failure claim before finalization"
    },
    "staging": "record_finalized=false => status INCONCLUSIVE; staging is never in final bundle.",
    "status_values": [
      "PASS",
      "FAIL",
      "INCONCLUSIVE"
    ],
    "sysv_bounds": {
      "max_segment_bytes": 33554532,
      "max_segments": 5,
      "max_total_bytes": 167772660,
      "reported_total_equals_segment_sum": true,
      "unique_ids": true
    }
  },
  "cmake": {
    "build_type": "Debug",
    "changed_file_allowlist_is_not_target_census": true,
    "cmake_version": "3.28.3",
    "compile_commands_verification": {
      "argument_scope": "Purpose-specific pinned-CMake checker, not a universal compiler command parser. Preserve the complete argv vector and compile database SHA-256; do not reject unrelated warning/debug flags solely because they are outside a generic grammar.",
      "checker_logic": "\\\nimport json, os, re, shlex, sys\nfrom pathlib import Path\n\nEXPECTED = {\n    \"diagnostic\": {\"Common\", \"ng039_gw_q0_q1_d2\"},\n    \"default\": {\"Common\", \"ContentApp\", \"IoTTestApp\", \"NBTestApp\", \"PGCS\", \"NRNCS\"},\n}\nCOMPILER = os.path.realpath(\"/usr/bin/x86_64-linux-gnu-g++-13\")\nOBSERVER = \"NG_GW_QUEUE_TEST_OBSERVATION\"\nREQUIRED = {\"-O0\", \"-g\", \"-g3\", \"-Wall\", \"-fmessage-length=0\", \"-pthread\", \"-Wno-deprecated\", \"-lpthread\", \"-lrt\", \"-fno-omit-frame-pointer\"}\nSOURCE_SUFFIXES = (\".c\", \".cc\", \".cpp\", \".cxx\", \".c++\", \".C\")\n\ndef reject(message):\n    raise ValueError(message)\n\ndef resolved(value, base, strict):\n    if not isinstance(value, str) or not value.strip():\n        reject(\"empty path metadata\")\n    p = Path(value)\n    if not p.is_absolute():\n        p = base / p\n    return p.resolve(strict=strict)\n\ndef under(path, root):\n    try:\n        path.relative_to(root)\n        return True\n    except ValueError:\n        return False\n\ndef tokens_for(entry):\n    if not isinstance(entry, dict): reject(\"entry must be object\")\n    if not isinstance(entry.get(\"directory\"), str) or not entry[\"directory\"]: reject(\"directory is empty\")\n    if not isinstance(entry.get(\"file\"), str) or not entry[\"file\"]: reject(\"file is empty\")\n    args = entry.get(\"arguments\")\n    command = entry.get(\"command\")\n    if args is not None and (not isinstance(args, list) or not args or any(not isinstance(x, str) for x in args)):\n        reject(\"arguments must be a nonempty string array\")\n    if command is not None:\n        if not isinstance(command, str) or not command: reject(\"command must be nonempty string\")\n        if any(c in command for c in \"$`;&|<>*?\"): reject(\"shell/glob syntax is unsupported\")\n        try: parsed = shlex.split(command, posix=True)\n        except ValueError: reject(\"malformed command quoting\")\n        if not parsed: reject(\"empty command\")\n        if args is not None and parsed != args: reject(\"command/arguments mismatch\")\n        if args is None: args = parsed\n    if args is None: reject(\"entry has neither command nor arguments\")\n    return args\n\ndef parse_macro_switches(argv):\n    found=[]; i=0\n    while i < len(argv):\n        token=argv[i]\n        if token in (\"-D\", \"-U\"):\n            if i+1 >= len(argv): reject(\"missing -D/-U operand\")\n            op=token; value=argv[i+1]; i+=2\n        elif token.startswith(\"-D\") and len(token)>2:\n            op=\"-D\"; value=token[2:]; i+=1\n        elif token.startswith(\"-U\") and len(token)>2:\n            op=\"-U\"; value=token[2:]; i+=1\n        else:\n            i+=1; continue\n        match=re.fullmatch(r\"([A-Za-z_][A-Za-z0-9_]*)(\\([^)]*\\))?(?:=(.*))?\", value)\n        if not match: reject(\"malformed macro switch\")\n        if match.group(1)==OBSERVER:\n            found.append((op, match.group(2), match.group(3), value))\n    return found\n\ndef output_operand(argv):\n    found=[]; i=0\n    while i < len(argv):\n        token=argv[i]\n        if token == \"-o\":\n            if i+1 >= len(argv) or not argv[i+1] or argv[i+1].startswith(\"-\"): reject(\"missing -o operand\")\n            found.append(argv[i+1]); i+=2; continue\n        if token.startswith(\"-o\") and len(token)>2 and not token.startswith(\"-open\"):\n            found.append(token[2:])\n        i+=1\n    if len(found)!=1: reject(\"exactly one -o output is required\")\n    return found[0]\n\ndef target_from_output(path):\n    parts=path.parts; names=[]\n    for i,part in enumerate(parts[:-1]):\n        if part==\"CMakeFiles\" and parts[i+1].endswith(\".dir\"):\n            names.append(parts[i+1][:-4])\n    if len(names)!=1: reject(\"target must derive uniquely from actual -o path\")\n    return names[0]\n\ndef validate_compile_database(data, profile, worktree_root, build_root):\n    if profile not in EXPECTED: reject(\"unknown profile\")\n    if not isinstance(data, list) or not data: reject(\"compile database must be nonempty array\")\n    source_root=Path(worktree_root).resolve(strict=True)\n    build=Path(build_root).resolve(strict=True)\n    if not source_root.is_dir() or not build.is_dir(): reject(\"trusted roots must be existing directories\")\n    if under(source_root, build) or under(build, source_root): reject(\"source/build roots must be disjoint\")\n    seen=set(); pairs=set(); counts={}\n    for entry in data:\n        argv=tokens_for(entry)\n        if os.path.realpath(argv[0]) != COMPILER: reject(\"compiler realpath mismatch\")\n        if any(x.startswith(\"@\") for x in argv): reject(\"response files are unsupported\")\n        if any(x == \"-Wp\" or x.startswith(\"-Wp,\") or x == \"-Xpreprocessor\" for x in argv): reject(\"forwarded preprocessor controls are unsupported\")\n        if any(x in {\"-ansi\", \"-traditional\", \"-traditional-cpp\", \"-x\", \"-include\", \"-imacros\", \"-specs\", \"-B\", \"-wrapper\", \"-fplugin\"} or x.startswith((\"-include\", \"-imacros\", \"-specs=\", \"-B\", \"-fplugin=\")) for x in argv):\n            reject(\"compiler mode/toolchain override is unsupported\")\n        directory=resolved(entry[\"directory\"], Path.cwd(), strict=True)\n        if not directory.is_dir() or not under(directory, build): reject(\"compile directory escapes build root\")\n        source=resolved(entry[\"file\"], directory, strict=True)\n        if not source.is_file() or not under(source, source_root): reject(\"source escapes pinned worktree\")\n        source_args=[x for x in argv[1:] if x.endswith(SOURCE_SUFFIXES)]\n        if len(source_args)!=1 or resolved(source_args[0],directory,strict=True)!=source: reject(\"exactly one source operand must match entry file\")\n        if argv.count(\"-c\")!=1: reject(\"exactly one -c is required\")\n        out=resolved(output_operand(argv),directory,strict=False)\n        if not under(out,build): reject(\"actual -o output escapes build root\")\n        metadata=entry.get(\"output\")\n        if metadata is not None:\n            meta=resolved(metadata,directory,strict=False)\n            if meta!=out: reject(\"output metadata disagrees with actual -o\")\n        target=target_from_output(out)\n        if target not in EXPECTED[profile]: reject(\"unexpected target derived from actual output\")\n        standards=[x for x in argv if x.startswith(\"-std=\")]\n        if standards != [\"-std=gnu++20\"]: reject(\"effective language mode must be exactly gnu++20\")\n        optimizations=[x for x in argv if x.startswith(\"-O\")]\n        if not optimizations or any(x!=\"-O0\" for x in optimizations): reject(\"effective optimization must be -O0\")\n        if not REQUIRED.issubset(set(argv)): reject(\"required pinned project/debug flags missing\")\n        macros=parse_macro_switches(argv)\n        if profile==\"default\" and macros: reject(\"observer macro defined or undefined in default profile\")\n        if profile==\"diagnostic\" and target in {\"Common\",\"ng039_gw_q0_q1_d2\"}:\n            if macros != [(\"-D\", None, \"1\", OBSERVER+\"=1\")]: reject(\"diagnostic observer must be exactly one object-like =1 define\")\n        pair=(target,str(out))\n        if pair in pairs: reject(\"duplicate target/output compile entry\")\n        pairs.add(pair); seen.add(target); counts[target]=counts.get(target,0)+1\n    if seen != EXPECTED[profile]: reject(\"target census mismatch\")\n    return {\"profile\":profile,\"targets\":sorted(seen),\"target_counts\":counts,\"entry_count\":len(data)}\n\nif __name__ == \"__main__\":\n    try:\n        with open(sys.argv[1],encoding=\"utf-8\") as f: db=json.load(f)\n        print(json.dumps(validate_compile_database(db,sys.argv[2],sys.argv[3],sys.argv[4]),sort_keys=True))\n    except Exception as exc:\n        print(\"compile check failed: \"+str(exc),file=sys.stderr); raise SystemExit(2)",
      "compile_action_cardinality": "exactly one -c, exactly one source operand matching entry.file, and exactly one actual -o output per entry",
      "compile_database_sha256_and_target_counts_recorded": true,
      "compiler_realpath_required": "/usr/bin/x86_64-linux-gnu-g++-13",
      "critical_flags": {
        "observer_macro": "parse joined/split -D/-U by base name including function-like spelling; default profile has none; diagnostic Common/harness each have exactly one object-like =1 define",
        "optimization": "one or more -O0 tokens; no other -O token",
        "standard": "exactly one -std=gnu++20"
      },
      "expected_targets": {
        "basis": "CMakeLists.txt at immutable baseline; existing standalone targets are behind NG_ENABLE_LEGACY_STANDALONE=OFF. Diagnostic graph is a future exact two-target root-listfile early-return graph, not present in the baseline.",
        "default_off": [
          "Common",
          "ContentApp",
          "IoTTestApp",
          "NBTestApp",
          "PGCS",
          "NRNCS"
        ],
        "diagnostic_forbidden": [
          "ContentApp",
          "GIRS",
          "HTS",
          "IoTTestApp",
          "NBTestApp",
          "PGCS",
          "PSS",
          "NRNCS"
        ],
        "diagnostic_on": [
          "Common",
          "ng039_gw_q0_q1_d2"
        ],
        "exact": true,
        "legacy_absent_default_off": [
          "GIRS",
          "HTS",
          "PSS"
        ]
      },
      "metadata_binding": "If entry.output exists, normalized output must equal actual -o. command and arguments, when both present, must tokenize identically.",
      "negative_fixtures": [
        "source outside worktree",
        "output outside build root",
        "output metadata mismatch",
        "duplicate -c",
        "wrong compiler realpath",
        "-Wp observer forwarding",
        "-Xpreprocessor observer forwarding",
        "default-profile observer undefine",
        "-ansi alternate dialect",
        "later non-C++20 standard",
        "non--O0 optimization",
        "target census mismatch",
        "diagnostic function-like observer define",
        "joined -include observer override",
        "joined -imacros observer override"
      ],
      "not_executed_in_r07": false,
      "planned_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/python3 -c <compile-checker> \"$DB\" \"$PROFILE\" \"$D2_WORKTREE\" \"$BUILD_ROOT\"",
      "r07_python_optimize_probe": "PASS after B1/B2 amendment: literal commands ran under normal Python and PYTHONOPTIMIZE=1.",
      "r07_synthetic_execution": "PASS after B1/B2 amendment: 2 positive profiles and 15 distinct negative mutations rejected under normal Python and PYTHONOPTIMIZE=1 (30 negative executions); exact entrypoint missing-operand rejection covered separately.",
      "reject_bypasses": [
        "@response files",
        "-Wp and -Wp,*",
        "-Xpreprocessor",
        "-ansi",
        "-traditional",
        "-traditional-cpp",
        "-x",
        "-include",
        "-imacros",
        "-specs",
        "-B",
        "-wrapper",
        "-fplugin"
      ],
      "required": true,
      "required_project_switches": [
        "-O0",
        "-Wall",
        "-Wno-deprecated",
        "-fmessage-length=0",
        "-fno-omit-frame-pointer",
        "-g",
        "-g3",
        "-lpthread",
        "-lrt",
        "-pthread"
      ],
      "root_rules": [
        "Both roots must resolve to existing directories.",
        "Roots must be disjoint.",
        "Each compile directory and actual -o output must resolve under build_root.",
        "Each source path must resolve to a regular file under worktree_root.",
        "Resolve existing symlink components before containment checks."
      ],
      "target_identity_source": "Derive only from the actual -o path CMakeFiles/<target>.dir segment; never from arbitrary command text or metadata.",
      "trusted_root_inputs": [
        "worktree_root",
        "build_root"
      ]
    },
    "compiler": "/usr/bin/x86_64-linux-gnu-g++-13",
    "compiler_version": "GNU C++ 13.3.0",
    "configure_common_flags": "-G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g'",
    "configure_generator": "Unix Makefiles",
    "configured_target_census": {
      "basis": "CMakeLists.txt at immutable baseline; existing standalone targets are behind NG_ENABLE_LEGACY_STANDALONE=OFF. Diagnostic graph is a future exact two-target root-listfile early-return graph, not present in the baseline.",
      "default_off": [
        "Common",
        "ContentApp",
        "IoTTestApp",
        "NBTestApp",
        "PGCS",
        "NRNCS"
      ],
      "diagnostic_forbidden": [
        "ContentApp",
        "GIRS",
        "HTS",
        "IoTTestApp",
        "NBTestApp",
        "PGCS",
        "PSS",
        "NRNCS"
      ],
      "diagnostic_on": [
        "Common",
        "ng039_gw_q0_q1_d2"
      ],
      "exact": true,
      "legacy_absent_default_off": [
        "GIRS",
        "HTS",
        "PSS"
      ]
    },
    "cxx_extensions": "ON",
    "cxx_standard": "20 (gnu++20)",
    "default_build_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake --build \"$D2_SCRATCH/build-default\" --target Common --parallel 2",
    "default_configure_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake -S \"$D2_WORKTREE\" -B \"$D2_SCRATCH/build-default\" -G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g' -DNG_ENABLE_NG039_GW_Q0_Q1_D2=OFF -DNG_ENABLE_LEGACY_STANDALONE=OFF",
    "default_value": "OFF",
    "diagnostic_build_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake --build \"$D2_SCRATCH/build-diagnostic\" --target ng039_gw_q0_q1_d2 --parallel 2",
    "diagnostic_configure_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/cmake -S \"$D2_WORKTREE\" -B \"$D2_SCRATCH/build-diagnostic\" -G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g' -DNG_ENABLE_NG039_GW_Q0_Q1_D2=ON -DNG_ENABLE_LEGACY_STANDALONE=OFF",
    "diagnostic_option": "NG_ENABLE_NG039_GW_Q0_Q1_D2",
    "diagnostic_value": "ON",
    "environment_contract": {
      "applies_to": [
        "cmake configure",
        "cmake build",
        "compile checker",
        "link checker",
        "finalizer"
      ],
      "directory_setup": "Create HOME and TMPDIR beneath D2_SCRATCH before configure using the authorized parent process; reject symlinks and overlap.",
      "launcher": "env -i; no inherited variable is passed to child processes",
      "not_executed_in_r07": true,
      "recording": "Record the exact expanded five name/value pairs and working directory in the build/run identity before configure; HOME and TMPDIR must be distinct children of D2_SCRATCH.",
      "retained": {
        "HOME": "${D2_SCRATCH}/home",
        "LANG": "C",
        "LC_ALL": "C",
        "PATH": "/usr/bin:/bin",
        "TMPDIR": "${D2_SCRATCH}/tmp"
      },
      "unset_inherited": "All variables not explicitly retained above are absent by construction; no per-variable denylist."
    },
    "export_compile_commands": true,
    "legacy_option": "NG_ENABLE_LEGACY_STANDALONE",
    "legacy_value_in_both_fresh_configs": "OFF",
    "link_command_verification": {
      "binding_control": "A valid named link.txt passes when stdin is deliberately supplied adversarial text; checker reads the file, not stdin.",
      "checker_logic": "\\\nimport hashlib, os, shlex, sys\nfrom pathlib import Path\nCOMPILER=os.path.realpath(\"/usr/bin/x86_64-linux-gnu-g++-13\")\nREQUIRED=(\"-static\",\"-static-libgcc\",\"-static-libstdc++\")\nDIRECT_DYNAMIC={\"-shared\",\"-dynamic\",\"-Bdynamic\",\"-pie\",\"-static-pie\",\"-rdynamic\",\"--dynamic-linker\"}\ndef reject(message): raise ValueError(message)\ndef validate_link_file(link_file,build_root,target):\n    root=Path(build_root).resolve(strict=True)\n    expected=(root/\"CMakeFiles\"/(target+\".dir\")/\"link.txt\").resolve(strict=True)\n    supplied=Path(link_file)\n    if supplied.is_symlink() or not supplied.is_file(): reject(\"link.txt must be a regular non-symlink file\")\n    actual=supplied.resolve(strict=True)\n    if actual!=expected: reject(\"link.txt path is not the expected target file\")\n    try: actual.relative_to(root)\n    except ValueError: reject(\"link.txt escapes build root\")\n    content=actual.read_text(encoding=\"utf-8\")\n    try: argv=shlex.split(content,posix=True)\n    except ValueError: reject(\"malformed link command quoting\")\n    if not argv or os.path.realpath(argv[0])!=COMPILER: reject(\"linker compiler identity mismatch\")\n    if any(x.startswith(\"@\") for x in argv): reject(\"response files unsupported\")\n    if any(x==\"-Wl\" or x.startswith(\"-Wl,\") or x==\"-Xlinker\" or x.startswith(\"-Xlinker\") for x in argv): reject(\"forwarded linker controls unsupported for this target\")\n    if any(x in DIRECT_DYNAMIC or x.startswith(\"--dynamic-linker=\") for x in argv): reject(\"direct dynamic-link override\")\n    if any(argv.count(flag)!=1 for flag in REQUIRED): reject(\"each static-link token must occur exactly once\")\n    return {\"status\":\"PASS\",\"link_file_sha256\":hashlib.sha256(actual.read_bytes()).hexdigest(),\"compiler\":argv[0]}\nif __name__==\"__main__\":\n    try: print(validate_link_file(sys.argv[1],sys.argv[2],sys.argv[3]))\n    except Exception as exc: print(\"link check failed: \"+str(exc),file=sys.stderr); raise SystemExit(2)",
      "compiler_realpath_required": "/usr/bin/x86_64-linux-gnu-g++-13",
      "failure_behavior": "Explicit ValueError/nonzero exit; never Python assert.",
      "file_binding": "Open the named link.txt directly; require a regular non-symlink file whose canonical path equals build_root/CMakeFiles/ng039_gw_q0_q1_d2.dir/link.txt.",
      "forwarding_policy": "Reject every -Wl/-Xlinker spelling for this target instead of normalizing arbitrary forwarding syntax.",
      "modes_to_test": [
        "normal",
        "PYTHONOPTIMIZE=1"
      ],
      "negative_fixtures": [
        "wrong link.txt path",
        "missing standalone -static",
        "duplicate standalone -static",
        "wrong linker compiler realpath",
        "@response file",
        "-Wl,-shared",
        "split -Xlinker -B -Xlinker dynamic",
        "symlinked link.txt"
      ],
      "not_executed_in_r07": false,
      "planned_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/python3 -c <link-checker> \"$LINK_TXT\" \"$BUILD_ROOT\" ng039_gw_q0_q1_d2",
      "r07_python_optimize_probe": "PASS after B1/B2 amendment: literal link command ran under normal Python and PYTHONOPTIMIZE=1; positive stdin-binding control and all 8 targeted negatives exercised.",
      "r07_synthetic_execution": "PASS after B1/B2 amendment: 1 stdin-binding positive and 8 distinct negatives rejected in normal Python and PYTHONOPTIMIZE=1 (16 negative executions); exact entrypoint missing-target rejection covered separately.",
      "reject_dynamic_controls": [
        "-shared",
        "-dynamic",
        "-Bdynamic",
        "-pie",
        "-static-pie",
        "-rdynamic",
        "--dynamic-linker",
        "any -Wl form",
        "any -Xlinker form",
        "@response file"
      ],
      "required": true,
      "required_exact_arguments": [
        "-static",
        "-static-libgcc",
        "-static-libstdc++"
      ],
      "tokenizer": "shlex.split(posix=True) on the bytes read from that actual file; stdin is never used."
    },
    "macro": "NG_GW_QUEUE_TEST_OBSERVATION",
    "macro_scope": "Common PUBLIC + harness PRIVATE only in isolated option-ON graph",
    "project_appended_cxx_flags": [
      "-O0",
      "-g3",
      "-Wall",
      "-fmessage-length=0",
      "-pthread",
      "-Wno-deprecated",
      "-lpthread",
      "-lrt",
      "-fno-omit-frame-pointer"
    ],
    "project_executable_link_flags": [
      "-static",
      "-static-libgcc",
      "-static-libstdc++"
    ],
    "root_return_before_production_targets": true,
    "target_graph_guard_required": true
  },
  "evidence_recovery": {
    "d1h_binary_at_recorded_path_available_now": false,
    "d1h_build_log_at_recorded_path_available_now": false,
    "d1h_evidence_status": "Original D1-H binary/log remain unavailable and their historical hashes cannot be rechecked. A fresh compile/link-only rebuild was performed 2026-10-10 outside .hermes; its different binary/log hashes are preserved in Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json. Runtime remains unexecuted.",
    "d1h_fresh_rebuild": {
      "binary_sha256": "060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d",
      "build_exit_code": 0,
      "build_log_sha256": "658cdeec0de9d743438c2683f0bca710b3a7b53302374303793a7838d3246e98",
      "configure_exit_code": 0,
      "configure_log_sha256": "65fb7c5ef0f1613c7aa181794678203c1d70d0c1313f8b20a51b471ef8330feb",
      "interpretation": "New evidence; do not conflate with historical recorded D1-H hashes.",
      "receipt_path": "Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json",
      "receipt_sha256": "3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28",
      "runtime_executed": false,
      "worktree_head": "1af604dad66148618cc0e578c28adb4a9829e607"
    },
    "d1h_manifest_sha256": "9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a",
    "d1h_manifest_sha256_rechecked": true,
    "d1h_reported_binary_sha256": "ddf06f6d47e37564b9e52866c3d272ce3cc6cd8b433700c864c4499d676c31ab",
    "d1h_reported_build_exit_code": 0,
    "d1h_reported_build_log_sha256": "095009a9e4fb7097aef005300f7562ab8ce941c691538a88a4064f944f4d19aa",
    "historical_v1_8_synthetic_results": {
      "compile_checker": {
        "negative_cases": 26,
        "positive_profiles": 2
      },
      "link_checker": {
        "cases": 15,
        "modes": [
          "normal",
          "python3 -O",
          "PYTHONOPTIMIZE=1"
        ],
        "negative": 12,
        "positive": 3
      },
      "not_real_build_or_runtime_evidence": true,
      "run_record_fixtures": {
        "invalid": 14,
        "valid": 4
      },
      "schemas_meta_valid": 3,
      "source": "NG-039/NG-040 task notes and prior-session record, not a retained standalone validator run bundle"
    },
    "r05_archive_manifest_sha256_rechecked": "27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a",
    "r05_raw_sse_sha256_rechecked": "3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31",
    "r05_response_sha256_rechecked": "9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc",
    "r07_synthetic_validation_receipt": {
      "binding_policy": "Generate/refresh the receipt only after the packet and manifest bytes are frozen; receipt records their exact input hashes. Record the receipt SHA-256 out-of-band in NG-039/NG-040. Do not embed a receipt hash in this manifest, avoiding a hash cycle.",
      "path": "Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json",
      "semantic_scope": "Synthetic checker/schema checks only; no executable cross-artifact finalizer exists yet."
    }
  },
  "evidence_schemas": {
    "collection_responsibility": {
      "artifact_name_uniqueness": "Exactly one of each six required file; optional harness-diagnostics.txt at most once; no other name.",
      "failure_summary_binding": "The finalizer recomputes code, phase, last_event_sequence, inventory_availability, cleanup_outcome and missing_artifacts. ASSERTION_FAILED is valid only for an observed correctness-predicate failure with complete safe evidence; contradictory summary fields finalize INCONCLUSIVE/EVIDENCE_INVALID.",
      "finalizer_bounds": "10 s timeout + 5 s kill grace; RLIMIT_AS 128 MiB, CPU 10 s, FSIZE 4 MiB, NOFILE 32, CORE 0.",
      "finalizer_failure": "No valid finalized record means INCONCLUSIVE and cannot PASS.",
      "finalizer_semantic_authority": "The same allowlisted binary recomputes cross-artifact validity and status; it validates actual file names/sizes/hashes, run IDs/phases, inventory-derived summaries, event ordering/counters, target-correlated wait/decision identity, Q0/Q1 predicates and safe cleanup. Any missing or contradictory evidence is INCONCLUSIVE.",
      "harness_run": "Writes bounded run-record.pending.json (record_finalized=false, INCONCLUSIVE) and available inventories/events; does not claim closed-log hashes or outer exit.",
      "missing_or_invalid_staging": "Metadata-only INCONCLUSIVE; assertions NOT_RUN, measurements null, inventories unavailable, cleanup unknown, missing artifact names; no invented values.",
      "outer_finalizer": "Same allowlisted binary --finalize outside namespaces; binds outer status, hashes closed artifacts, validates staging/caps, atomically writes final record, removes staging. No Process/GW, IPC or child.",
      "runtime_manifest_file": "none; q0_q1_d2.manifest.json is plan/schema input, not part of runtime bundle.",
      "shell_redirection": "Captures stdout/stderr outside namespaces and closes descriptors before collection."
    },
    "ipc_inventory": {
      "$id": "urn:ng039:ipc-inventory:v2",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "properties": {
        "complete": {
          "type": "boolean"
        },
        "dev_shm_entries": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "kind": {
                "enum": [
                  "regular",
                  "symlink",
                  "other"
                ]
              },
              "name": {
                "maxLength": 255,
                "minLength": 1,
                "pattern": "^[^/]+$",
                "type": "string"
              },
              "size_bytes": {
                "maximum": 16777216,
                "minimum": 0,
                "type": "integer"
              }
            },
            "required": [
              "name",
              "kind",
              "size_bytes"
            ],
            "type": "object"
          },
          "maxItems": 128,
          "type": "array"
        },
        "error": {
          "maxLength": 512,
          "type": [
            "string",
            "null"
          ]
        },
        "is_empty": {
          "type": [
            "boolean",
            "null"
          ]
        },
        "phase": {
          "enum": [
            "pre",
            "post",
            "final"
          ]
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-ipc-inventory/v2"
        },
        "sysv_ids": {
          "additionalProperties": false,
          "properties": {
            "msg": {
              "items": {
                "minimum": 0,
                "type": "integer"
              },
              "maxItems": 4096,
              "type": "array",
              "uniqueItems": true
            },
            "sem": {
              "items": {
                "minimum": 0,
                "type": "integer"
              },
              "maxItems": 4096,
              "type": "array",
              "uniqueItems": true
            },
            "shm": {
              "items": {
                "minimum": 0,
                "type": "integer"
              },
              "maxItems": 4096,
              "type": "array",
              "uniqueItems": true
            }
          },
          "required": [
            "shm",
            "sem",
            "msg"
          ],
          "type": "object"
        },
        "sysv_shm_segments": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "id": {
                "minimum": 0,
                "type": "integer"
              },
              "key": {
                "type": [
                  "integer",
                  "null"
                ]
              },
              "size_bytes": {
                "maximum": 33554532,
                "minimum": 0,
                "type": "integer"
              }
            },
            "required": [
              "id",
              "size_bytes",
              "key"
            ],
            "type": "object"
          },
          "maxItems": 5,
          "type": "array",
          "uniqueItems": true
        },
        "sysv_shm_total_bytes": {
          "maximum": 167772660,
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        }
      },
      "required": [
        "schema_version",
        "run_id",
        "phase",
        "complete",
        "is_empty",
        "sysv_ids",
        "dev_shm_entries",
        "error",
        "sysv_shm_segments",
        "sysv_shm_total_bytes"
      ],
      "type": "object"
    },
    "observer_events": {
      "$id": "urn:ng039:observer-events:v2",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "properties": {
        "events": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "candidate_was_popped": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "correlation_status": {
                "enum": [
                  "PENDING",
                  "MATCHED",
                  "DECISION_NOT_REACHED",
                  "HEAD_MISMATCH",
                  "INVALID",
                  "OVERFLOW"
                ]
              },
              "counter_overflow": {
                "type": "boolean"
              },
              "decision_candidate_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "decision_candidate_time": {
                "type": [
                  "number",
                  "null"
                ]
              },
              "decision_due_branch": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "decision_iteration_id": {
                "maximum": 18446744073709551615,
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "decision_sequence": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "decision_was_popped": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "empty_queue_observation_count": {
                "minimum": 0,
                "type": "integer"
              },
              "event": {
                "enum": [
                  "None",
                  "QueueSnapshot",
                  "Admission",
                  "WaitReturn",
                  "PopDecision",
                  "RunEntry",
                  "RunReturn",
                  "Stop"
                ]
              },
              "event_message_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_depth": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_head_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_head_time": {
                "type": [
                  "number",
                  "null"
                ]
              },
              "first_target_wait_iteration_id": {
                "maximum": 18446744073709551615,
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_message_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_returned_by_predicate": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "first_target_wait_sequence": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_timeout_ms": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "first_target_wait_valid": {
                "type": "boolean"
              },
              "gateway_iteration_id": {
                "maximum": 18446744073709551615,
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "gateway_started": {
                "type": "boolean"
              },
              "has_head": {
                "type": "boolean"
              },
              "head_message_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "head_time": {
                "type": [
                  "number",
                  "null"
                ]
              },
              "queue_depth": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "registered_target_id": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              },
              "scheduled_time_is_due": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "sequence": {
                "minimum": 0,
                "type": "integer"
              },
              "stop_requested": {
                "type": "boolean"
              },
              "target_admission_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_non_due_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_pop_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_run_entry_count": {
                "minimum": 0,
                "type": "integer"
              },
              "target_run_return_count": {
                "minimum": 0,
                "type": "integer"
              },
              "valid": {
                "type": "boolean"
              },
              "wait_returned_by_predicate": {
                "type": [
                  "boolean",
                  "null"
                ]
              },
              "wait_timeout_ms": {
                "minimum": 0,
                "type": [
                  "integer",
                  "null"
                ]
              }
            },
            "required": [
              "sequence",
              "event",
              "valid",
              "gateway_started",
              "registered_target_id",
              "event_message_id",
              "queue_depth",
              "has_head",
              "head_message_id",
              "head_time",
              "wait_timeout_ms",
              "wait_returned_by_predicate",
              "scheduled_time_is_due",
              "candidate_was_popped",
              "empty_queue_observation_count",
              "target_admission_count",
              "target_non_due_count",
              "target_pop_count",
              "target_run_entry_count",
              "target_run_return_count",
              "first_target_wait_valid",
              "first_target_wait_sequence",
              "first_target_wait_message_id",
              "first_target_wait_head_id",
              "first_target_wait_head_time",
              "first_target_wait_depth",
              "first_target_wait_timeout_ms",
              "first_target_wait_returned_by_predicate",
              "stop_requested",
              "counter_overflow",
              "gateway_iteration_id",
              "first_target_wait_iteration_id",
              "decision_sequence",
              "decision_iteration_id",
              "decision_candidate_id",
              "decision_candidate_time",
              "decision_due_branch",
              "decision_was_popped",
              "correlation_status"
            ],
            "type": "object"
          },
          "maxItems": 4096,
          "type": "array"
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-observer-events/v2"
        }
      },
      "required": [
        "schema_version",
        "run_id",
        "events"
      ],
      "type": "object"
    },
    "run_record": {
      "$defs": {
        "assertion_status": {
          "enum": [
            "PASS",
            "FAIL",
            "INCONCLUSIVE",
            "NOT_RUN"
          ]
        }
      },
      "$id": "urn:ng039:d2-run-record:v2",
      "$schema": "https://json-schema.org/draft/2020-12/schema",
      "additionalProperties": false,
      "allOf": [
        {
          "if": {
            "properties": {
              "record_finalized": {
                "const": false
              }
            },
            "required": [
              "record_finalized"
            ]
          },
          "then": {
            "properties": {
              "failure": {
                "type": "null"
              },
              "status": {
                "const": "INCONCLUSIVE"
              }
            },
            "required": [
              "status",
              "failure"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "PASS"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "artifacts": {
                "allOf": [
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stdout.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stderr.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-pre.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-post.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-final.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "observer-events.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  }
                ]
              },
              "assertions": {
                "properties": {
                  "bootstrap_queue_drained": {
                    "const": "PASS"
                  },
                  "evidence_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_empty": {
                    "const": "PASS"
                  },
                  "finalization_complete": {
                    "const": "PASS"
                  },
                  "first_target_wait_fresh": {
                    "const": "PASS"
                  },
                  "gateway_joined": {
                    "const": "PASS"
                  },
                  "namespace_setup": {
                    "const": "PASS"
                  },
                  "output_worker_joined": {
                    "const": "PASS"
                  },
                  "post_inventory_complete": {
                    "const": "PASS"
                  },
                  "post_resources_attributable": {
                    "const": "PASS"
                  },
                  "post_resources_removed": {
                    "const": "PASS"
                  },
                  "pre_inventory_complete": {
                    "const": "PASS"
                  },
                  "pre_inventory_empty": {
                    "const": "PASS"
                  },
                  "process_blocks_verified": {
                    "const": "PASS"
                  },
                  "q0_empty_observation_delta": {
                    "const": "PASS"
                  },
                  "q0_zero_controlled_offers": {
                    "const": "PASS"
                  },
                  "scalar_run_identity_valid": {
                    "const": "PASS"
                  },
                  "target_admitted": {
                    "const": "PASS"
                  },
                  "target_delete_false_before_gateway": {
                    "const": "PASS"
                  },
                  "target_erased_once": {
                    "const": "PASS"
                  },
                  "target_future_at_end": {
                    "const": "PASS"
                  },
                  "target_is_head": {
                    "const": "PASS"
                  },
                  "target_not_popped": {
                    "const": "PASS"
                  },
                  "target_not_run": {
                    "const": "PASS"
                  },
                  "wait_decision_correlated": {
                    "const": "PASS"
                  }
                },
                "required": [
                  "bootstrap_queue_drained",
                  "evidence_complete",
                  "final_inventory_complete",
                  "final_inventory_empty",
                  "finalization_complete",
                  "first_target_wait_fresh",
                  "gateway_joined",
                  "namespace_setup",
                  "output_worker_joined",
                  "post_inventory_complete",
                  "post_resources_attributable",
                  "post_resources_removed",
                  "pre_inventory_complete",
                  "pre_inventory_empty",
                  "process_blocks_verified",
                  "q0_empty_observation_delta",
                  "q0_zero_controlled_offers",
                  "scalar_run_identity_valid",
                  "target_admitted",
                  "target_delete_false_before_gateway",
                  "target_erased_once",
                  "target_future_at_end",
                  "target_is_head",
                  "target_not_popped",
                  "target_not_run",
                  "wait_decision_correlated"
                ]
              },
              "cleanup": {
                "properties": {
                  "complete": {
                    "const": true
                  },
                  "final_inventory_complete": {
                    "const": true
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "const": true
                  },
                  "gateway_stop_requested": {
                    "const": true
                  },
                  "output_worker_joined": {
                    "const": true
                  },
                  "post_inventory_attributable": {
                    "const": true
                  },
                  "post_resources_removed": {
                    "const": true
                  },
                  "process_destroyed": {
                    "const": true
                  },
                  "target_deleted_once": {
                    "const": true
                  },
                  "target_detached": {
                    "const": true
                  }
                },
                "required": [
                  "complete",
                  "final_inventory_complete",
                  "finalizer_complete",
                  "gateway_joined",
                  "gateway_stop_requested",
                  "output_worker_joined",
                  "post_inventory_attributable",
                  "post_resources_removed",
                  "process_destroyed",
                  "target_deleted_once",
                  "target_detached"
                ]
              },
              "exit": {
                "properties": {
                  "harness_code": {
                    "const": 0
                  },
                  "outer_code": {
                    "const": 0
                  },
                  "signal": {
                    "type": "null"
                  },
                  "timeout_fired": {
                    "const": false
                  }
                },
                "required": [
                  "harness_code",
                  "outer_code",
                  "signal",
                  "timeout_fired"
                ]
              },
              "failure": {
                "type": "null"
              },
              "inventories": {
                "properties": {
                  "final": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      },
                      "is_empty": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty"
                    ]
                  },
                  "post": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "pre": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      },
                      "is_empty": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty"
                    ]
                  }
                },
                "required": [
                  "pre",
                  "post",
                  "final"
                ]
              },
              "observations": {
                "properties": {
                  "counter_overflow": {
                    "const": false
                  },
                  "q1": {
                    "properties": {
                      "correlation_status": {
                        "const": "MATCHED"
                      }
                    },
                    "required": [
                      "correlation_status"
                    ]
                  }
                },
                "required": [
                  "counter_overflow",
                  "q1"
                ]
              },
              "record_finalized": {
                "const": true
              }
            },
            "required": [
              "record_finalized",
              "failure",
              "exit",
              "assertions",
              "artifacts",
              "inventories",
              "cleanup",
              "observations"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "FAIL"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "artifacts": {
                "allOf": [
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stdout.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "stderr.txt"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-pre.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-post.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "ipc-final.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  },
                  {
                    "contains": {
                      "properties": {
                        "name": {
                          "const": "observer-events.json"
                        }
                      },
                      "required": [
                        "name"
                      ],
                      "type": "object"
                    },
                    "minContains": 1
                  }
                ]
              },
              "assertions": {
                "allOf": [
                  {
                    "anyOf": [
                      {
                        "properties": {
                          "bootstrap_queue_drained": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "bootstrap_queue_drained"
                        ]
                      },
                      {
                        "properties": {
                          "evidence_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "evidence_complete"
                        ]
                      },
                      {
                        "properties": {
                          "final_inventory_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "final_inventory_complete"
                        ]
                      },
                      {
                        "properties": {
                          "final_inventory_empty": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "final_inventory_empty"
                        ]
                      },
                      {
                        "properties": {
                          "finalization_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "finalization_complete"
                        ]
                      },
                      {
                        "properties": {
                          "first_target_wait_fresh": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "first_target_wait_fresh"
                        ]
                      },
                      {
                        "properties": {
                          "gateway_joined": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "gateway_joined"
                        ]
                      },
                      {
                        "properties": {
                          "namespace_setup": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "namespace_setup"
                        ]
                      },
                      {
                        "properties": {
                          "output_worker_joined": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "output_worker_joined"
                        ]
                      },
                      {
                        "properties": {
                          "post_inventory_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "post_inventory_complete"
                        ]
                      },
                      {
                        "properties": {
                          "post_resources_attributable": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "post_resources_attributable"
                        ]
                      },
                      {
                        "properties": {
                          "post_resources_removed": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "post_resources_removed"
                        ]
                      },
                      {
                        "properties": {
                          "pre_inventory_complete": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "pre_inventory_complete"
                        ]
                      },
                      {
                        "properties": {
                          "pre_inventory_empty": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "pre_inventory_empty"
                        ]
                      },
                      {
                        "properties": {
                          "process_blocks_verified": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "process_blocks_verified"
                        ]
                      },
                      {
                        "properties": {
                          "q0_empty_observation_delta": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "q0_empty_observation_delta"
                        ]
                      },
                      {
                        "properties": {
                          "q0_zero_controlled_offers": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "q0_zero_controlled_offers"
                        ]
                      },
                      {
                        "properties": {
                          "scalar_run_identity_valid": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "scalar_run_identity_valid"
                        ]
                      },
                      {
                        "properties": {
                          "target_admitted": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_admitted"
                        ]
                      },
                      {
                        "properties": {
                          "target_delete_false_before_gateway": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_delete_false_before_gateway"
                        ]
                      },
                      {
                        "properties": {
                          "target_erased_once": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_erased_once"
                        ]
                      },
                      {
                        "properties": {
                          "target_future_at_end": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_future_at_end"
                        ]
                      },
                      {
                        "properties": {
                          "target_is_head": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_is_head"
                        ]
                      },
                      {
                        "properties": {
                          "target_not_popped": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_not_popped"
                        ]
                      },
                      {
                        "properties": {
                          "target_not_run": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "target_not_run"
                        ]
                      },
                      {
                        "properties": {
                          "wait_decision_correlated": {
                            "const": "FAIL"
                          }
                        },
                        "required": [
                          "wait_decision_correlated"
                        ]
                      }
                    ]
                  },
                  {
                    "properties": {
                      "evidence_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "evidence_complete"
                    ]
                  },
                  {
                    "properties": {
                      "final_inventory_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "final_inventory_complete"
                    ]
                  },
                  {
                    "properties": {
                      "final_inventory_empty": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "final_inventory_empty"
                    ]
                  },
                  {
                    "properties": {
                      "finalization_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "finalization_complete"
                    ]
                  },
                  {
                    "properties": {
                      "gateway_joined": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "gateway_joined"
                    ]
                  },
                  {
                    "properties": {
                      "namespace_setup": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "namespace_setup"
                    ]
                  },
                  {
                    "properties": {
                      "output_worker_joined": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "output_worker_joined"
                    ]
                  },
                  {
                    "properties": {
                      "post_inventory_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "post_inventory_complete"
                    ]
                  },
                  {
                    "properties": {
                      "post_resources_attributable": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "post_resources_attributable"
                    ]
                  },
                  {
                    "properties": {
                      "post_resources_removed": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "post_resources_removed"
                    ]
                  },
                  {
                    "properties": {
                      "pre_inventory_complete": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "pre_inventory_complete"
                    ]
                  },
                  {
                    "properties": {
                      "pre_inventory_empty": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "pre_inventory_empty"
                    ]
                  },
                  {
                    "properties": {
                      "process_blocks_verified": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "process_blocks_verified"
                    ]
                  },
                  {
                    "properties": {
                      "scalar_run_identity_valid": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "scalar_run_identity_valid"
                    ]
                  },
                  {
                    "properties": {
                      "target_erased_once": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "target_erased_once"
                    ]
                  },
                  {
                    "properties": {
                      "wait_decision_correlated": {
                        "const": "PASS"
                      }
                    },
                    "required": [
                      "wait_decision_correlated"
                    ]
                  }
                ],
                "properties": {
                  "bootstrap_queue_drained": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "evidence_complete": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "final_inventory_complete": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "final_inventory_empty": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "finalization_complete": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "first_target_wait_fresh": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "gateway_joined": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "namespace_setup": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "output_worker_joined": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "post_inventory_complete": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "post_resources_attributable": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "post_resources_removed": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "pre_inventory_complete": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "pre_inventory_empty": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "process_blocks_verified": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "q0_empty_observation_delta": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "q0_zero_controlled_offers": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "scalar_run_identity_valid": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_admitted": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_delete_false_before_gateway": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_erased_once": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_future_at_end": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_is_head": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_not_popped": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "target_not_run": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  },
                  "wait_decision_correlated": {
                    "enum": [
                      "PASS",
                      "FAIL"
                    ]
                  }
                },
                "required": [
                  "bootstrap_queue_drained",
                  "evidence_complete",
                  "final_inventory_complete",
                  "final_inventory_empty",
                  "finalization_complete",
                  "first_target_wait_fresh",
                  "gateway_joined",
                  "namespace_setup",
                  "output_worker_joined",
                  "post_inventory_complete",
                  "post_resources_attributable",
                  "post_resources_removed",
                  "pre_inventory_complete",
                  "pre_inventory_empty",
                  "process_blocks_verified",
                  "q0_empty_observation_delta",
                  "q0_zero_controlled_offers",
                  "scalar_run_identity_valid",
                  "target_admitted",
                  "target_delete_false_before_gateway",
                  "target_erased_once",
                  "target_future_at_end",
                  "target_is_head",
                  "target_not_popped",
                  "target_not_run",
                  "wait_decision_correlated"
                ]
              },
              "cleanup": {
                "properties": {
                  "complete": {
                    "const": true
                  },
                  "final_inventory_complete": {
                    "const": true
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "const": true
                  },
                  "gateway_stop_requested": {
                    "const": true
                  },
                  "output_worker_joined": {
                    "const": true
                  },
                  "post_inventory_attributable": {
                    "const": true
                  },
                  "post_resources_removed": {
                    "const": true
                  },
                  "process_destroyed": {
                    "const": true
                  },
                  "target_deleted_once": {
                    "const": true
                  },
                  "target_detached": {
                    "const": true
                  }
                },
                "required": [
                  "complete",
                  "final_inventory_complete",
                  "finalizer_complete",
                  "gateway_joined",
                  "gateway_stop_requested",
                  "output_worker_joined",
                  "post_inventory_attributable",
                  "post_resources_removed",
                  "process_destroyed",
                  "target_deleted_once",
                  "target_detached"
                ]
              },
              "exit": {
                "properties": {
                  "harness_code": {
                    "maximum": 255,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "outer_code": {
                    "maximum": 123,
                    "minimum": 0,
                    "type": "integer"
                  },
                  "signal": {
                    "type": "null"
                  },
                  "timeout_fired": {
                    "const": false
                  }
                },
                "required": [
                  "harness_code",
                  "outer_code",
                  "signal",
                  "timeout_fired"
                ]
              },
              "failure": {
                "properties": {
                  "code": {
                    "const": "ASSERTION_FAILED"
                  }
                },
                "required": [
                  "code"
                ],
                "type": "object"
              },
              "inventories": {
                "properties": {
                  "final": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      },
                      "is_empty": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty"
                    ]
                  },
                  "post": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "pre": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      },
                      "is_empty": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty"
                    ]
                  }
                },
                "required": [
                  "pre",
                  "post",
                  "final"
                ]
              },
              "observations": {
                "properties": {
                  "counter_overflow": {
                    "const": false
                  },
                  "q1": {
                    "properties": {
                      "correlation_status": {
                        "const": "MATCHED"
                      }
                    },
                    "required": [
                      "correlation_status"
                    ]
                  }
                },
                "required": [
                  "counter_overflow",
                  "q1"
                ]
              },
              "record_finalized": {
                "const": true
              }
            },
            "required": [
              "record_finalized",
              "failure",
              "exit",
              "assertions",
              "artifacts",
              "inventories",
              "cleanup",
              "observations"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "record_finalized": {
                "const": true
              },
              "status": {
                "const": "INCONCLUSIVE"
              }
            },
            "required": [
              "record_finalized",
              "status"
            ]
          },
          "then": {
            "properties": {
              "failure": {
                "type": "object"
              }
            },
            "required": [
              "failure"
            ]
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "PASS"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "observations": {
                "properties": {
                  "q0": {
                    "properties": {
                      "controlled_application_offers": {
                        "const": 0
                      },
                      "empty_observation_delta": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "end_sequence": {
                        "type": "integer"
                      },
                      "start_sequence": {
                        "type": "integer"
                      }
                    },
                    "required": [
                      "start_sequence",
                      "end_sequence",
                      "controlled_application_offers",
                      "empty_observation_delta"
                    ]
                  },
                  "q1": {
                    "properties": {
                      "actual_due_branch": {
                        "const": false
                      },
                      "actual_popped": {
                        "const": false
                      },
                      "correlation_status": {
                        "const": "MATCHED"
                      },
                      "decision_candidate_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "decision_candidate_time": {
                        "type": "number"
                      },
                      "decision_iteration_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "decision_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "end_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "first_target_wait_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "gateway_iteration_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "head_time_at_end": {
                        "type": "number"
                      },
                      "process_time_at_end": {
                        "type": "number"
                      },
                      "start_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "target_message_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "target_pop_count": {
                        "const": 0
                      },
                      "target_run_entry_count": {
                        "const": 0
                      },
                      "target_run_return_count": {
                        "const": 0
                      },
                      "wait_depth": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_head_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_head_time": {
                        "type": "number"
                      },
                      "wait_iteration_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_predicate_return": {
                        "type": "boolean"
                      },
                      "wait_sequence": {
                        "minimum": 0,
                        "type": "integer"
                      },
                      "wait_target_id": {
                        "minimum": 1,
                        "type": "integer"
                      },
                      "wait_timeout_ms": {
                        "minimum": 0,
                        "type": "integer"
                      }
                    },
                    "required": [
                      "target_message_id",
                      "start_sequence",
                      "end_sequence",
                      "first_target_wait_sequence",
                      "wait_sequence",
                      "decision_sequence",
                      "gateway_iteration_id",
                      "wait_iteration_id",
                      "decision_iteration_id",
                      "wait_target_id",
                      "wait_head_id",
                      "decision_candidate_id",
                      "wait_depth",
                      "wait_timeout_ms",
                      "wait_head_time",
                      "decision_candidate_time",
                      "head_time_at_end",
                      "process_time_at_end",
                      "wait_predicate_return",
                      "actual_due_branch",
                      "actual_popped",
                      "target_pop_count",
                      "target_run_entry_count",
                      "target_run_return_count",
                      "correlation_status"
                    ]
                  }
                },
                "required": [
                  "q0",
                  "q1"
                ]
              }
            }
          }
        },
        {
          "if": {
            "properties": {
              "status": {
                "const": "FAIL"
              }
            },
            "required": [
              "status"
            ]
          },
          "then": {
            "properties": {
              "artifacts": {
                "minItems": 6
              },
              "assertions": {
                "properties": {
                  "evidence_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_complete": {
                    "const": "PASS"
                  },
                  "final_inventory_empty": {
                    "const": "PASS"
                  },
                  "finalization_complete": {
                    "const": "PASS"
                  },
                  "gateway_joined": {
                    "const": "PASS"
                  },
                  "namespace_setup": {
                    "const": "PASS"
                  },
                  "output_worker_joined": {
                    "const": "PASS"
                  },
                  "post_inventory_complete": {
                    "const": "PASS"
                  },
                  "post_resources_attributable": {
                    "const": "PASS"
                  },
                  "post_resources_removed": {
                    "const": "PASS"
                  },
                  "pre_inventory_complete": {
                    "const": "PASS"
                  },
                  "pre_inventory_empty": {
                    "const": "PASS"
                  },
                  "process_blocks_verified": {
                    "const": "PASS"
                  },
                  "scalar_run_identity_valid": {
                    "const": "PASS"
                  },
                  "target_erased_once": {
                    "const": "PASS"
                  },
                  "wait_decision_correlated": {
                    "const": "PASS"
                  }
                }
              },
              "cleanup": {
                "properties": {
                  "complete": {
                    "const": true
                  },
                  "final_inventory_complete": {
                    "const": true
                  },
                  "finalizer_complete": {
                    "const": true
                  },
                  "gateway_joined": {
                    "const": true
                  },
                  "gateway_stop_requested": {
                    "const": true
                  },
                  "output_worker_joined": {
                    "const": true
                  },
                  "post_inventory_attributable": {
                    "const": true
                  },
                  "post_resources_removed": {
                    "const": true
                  },
                  "process_destroyed": {
                    "const": true
                  },
                  "target_deleted_once": {
                    "const": true
                  },
                  "target_detached": {
                    "const": true
                  }
                }
              },
              "failure": {
                "properties": {
                  "cleanup_outcome": {
                    "const": "complete"
                  },
                  "code": {
                    "const": "ASSERTION_FAILED"
                  },
                  "inventory_availability": {
                    "properties": {
                      "final": {
                        "const": true
                      },
                      "post": {
                        "const": true
                      },
                      "pre": {
                        "const": true
                      }
                    },
                    "required": [
                      "pre",
                      "post",
                      "final"
                    ]
                  },
                  "missing_artifacts": {
                    "const": []
                  }
                },
                "required": [
                  "code",
                  "cleanup_outcome",
                  "missing_artifacts",
                  "inventory_availability"
                ]
              },
              "inventories": {
                "properties": {
                  "final": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      },
                      "is_empty": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty"
                    ]
                  },
                  "post": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete"
                    ]
                  },
                  "pre": {
                    "properties": {
                      "available": {
                        "const": true
                      },
                      "complete": {
                        "const": true
                      },
                      "is_empty": {
                        "const": true
                      }
                    },
                    "required": [
                      "available",
                      "complete",
                      "is_empty"
                    ]
                  }
                },
                "required": [
                  "pre",
                  "post",
                  "final"
                ]
              }
            },
            "required": [
              "artifacts",
              "failure",
              "assertions",
              "cleanup",
              "inventories"
            ]
          }
        }
      ],
      "properties": {
        "artifacts": {
          "items": {
            "additionalProperties": false,
            "properties": {
              "bytes": {
                "maximum": 4194304,
                "minimum": 0,
                "type": "integer"
              },
              "name": {
                "enum": [
                  "stdout.txt",
                  "stderr.txt",
                  "ipc-pre.json",
                  "ipc-post.json",
                  "ipc-final.json",
                  "observer-events.json",
                  "harness-diagnostics.txt"
                ]
              },
              "sha256": {
                "pattern": "^[0-9a-f]{64}$",
                "type": "string"
              }
            },
            "required": [
              "name",
              "bytes",
              "sha256"
            ],
            "type": "object"
          },
          "maxItems": 7,
          "type": "array",
          "uniqueItems": true
        },
        "assertions": {
          "additionalProperties": false,
          "properties": {
            "bootstrap_queue_drained": {
              "$ref": "#/$defs/assertion_status"
            },
            "evidence_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "final_inventory_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "final_inventory_empty": {
              "$ref": "#/$defs/assertion_status"
            },
            "finalization_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "first_target_wait_fresh": {
              "$ref": "#/$defs/assertion_status"
            },
            "gateway_joined": {
              "$ref": "#/$defs/assertion_status"
            },
            "namespace_setup": {
              "$ref": "#/$defs/assertion_status"
            },
            "output_worker_joined": {
              "$ref": "#/$defs/assertion_status"
            },
            "post_inventory_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "post_resources_attributable": {
              "$ref": "#/$defs/assertion_status"
            },
            "post_resources_removed": {
              "$ref": "#/$defs/assertion_status"
            },
            "pre_inventory_complete": {
              "$ref": "#/$defs/assertion_status"
            },
            "pre_inventory_empty": {
              "$ref": "#/$defs/assertion_status"
            },
            "process_blocks_verified": {
              "$ref": "#/$defs/assertion_status"
            },
            "q0_empty_observation_delta": {
              "$ref": "#/$defs/assertion_status"
            },
            "q0_zero_controlled_offers": {
              "$ref": "#/$defs/assertion_status"
            },
            "scalar_run_identity_valid": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_admitted": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_delete_false_before_gateway": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_erased_once": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_future_at_end": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_is_head": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_not_popped": {
              "$ref": "#/$defs/assertion_status"
            },
            "target_not_run": {
              "$ref": "#/$defs/assertion_status"
            },
            "wait_decision_correlated": {
              "$ref": "#/$defs/assertion_status"
            }
          },
          "required": [
            "namespace_setup",
            "pre_inventory_empty",
            "process_blocks_verified",
            "target_delete_false_before_gateway",
            "bootstrap_queue_drained",
            "q0_zero_controlled_offers",
            "q0_empty_observation_delta",
            "target_admitted",
            "target_is_head",
            "first_target_wait_fresh",
            "target_not_popped",
            "target_not_run",
            "target_future_at_end",
            "gateway_joined",
            "output_worker_joined",
            "target_erased_once",
            "final_inventory_empty",
            "evidence_complete",
            "pre_inventory_complete",
            "post_inventory_complete",
            "post_resources_attributable",
            "post_resources_removed",
            "final_inventory_complete",
            "finalization_complete",
            "wait_decision_correlated",
            "scalar_run_identity_valid"
          ],
          "type": "object"
        },
        "baseline_commit": {
          "const": "1af604dad66148618cc0e578c28adb4a9829e607"
        },
        "baseline_tree": {
          "const": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b"
        },
        "cleanup": {
          "additionalProperties": false,
          "properties": {
            "complete": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "final_inventory_complete": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "finalizer_complete": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "gateway_joined": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "gateway_stop_requested": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "output_worker_joined": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "post_inventory_attributable": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "post_resources_removed": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "process_destroyed": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "target_deleted_once": {
              "type": [
                "boolean",
                "null"
              ]
            },
            "target_detached": {
              "type": [
                "boolean",
                "null"
              ]
            }
          },
          "required": [
            "gateway_stop_requested",
            "gateway_joined",
            "output_worker_joined",
            "target_detached",
            "process_destroyed",
            "target_deleted_once",
            "complete",
            "post_inventory_attributable",
            "post_resources_removed",
            "final_inventory_complete",
            "finalizer_complete"
          ],
          "type": "object"
        },
        "configured_targets": {
          "const": [
            "Common",
            "ng039_gw_q0_q1_d2"
          ]
        },
        "document_head": {
          "const": "756f493b7049434ab67cf2382486a48f606a470c"
        },
        "exit": {
          "additionalProperties": false,
          "properties": {
            "harness_code": {
              "type": [
                "integer",
                "null"
              ]
            },
            "outer_code": {
              "type": [
                "integer",
                "null"
              ]
            },
            "signal": {
              "maxLength": 64,
              "type": [
                "string",
                "null"
              ]
            },
            "timeout_fired": {
              "type": "boolean"
            }
          },
          "required": [
            "outer_code",
            "harness_code",
            "signal",
            "timeout_fired"
          ],
          "type": "object"
        },
        "failure": {
          "oneOf": [
            {
              "type": "null"
            },
            {
              "additionalProperties": false,
              "properties": {
                "cleanup_outcome": {
                  "enum": [
                    "complete",
                    "partial",
                    "not_started",
                    "unknown"
                  ]
                },
                "code": {
                  "enum": [
                    "ASSERTION_FAILED",
                    "NAMESPACE_SETUP_FAILED",
                    "PRE_INVENTORY_FAILED",
                    "PARTIAL_CONSTRUCTION",
                    "IDENTITY_MISMATCH",
                    "COUNTER_OVERFLOW",
                    "TIMEOUT",
                    "SIGNAL",
                    "JOIN_BLOCKED",
                    "OWNERSHIP_UNCERTAIN",
                    "INVENTORY_INCOMPLETE",
                    "EVIDENCE_MISSING",
                    "EVIDENCE_INVALID",
                    "RESOURCE_LIMIT",
                    "OTHER",
                    "POST_INVENTORY_UNATTRIBUTABLE",
                    "POST_RESOURCE_REMOVE_FAILED",
                    "FINALIZER_FAILED"
                  ]
                },
                "inventory_availability": {
                  "additionalProperties": false,
                  "properties": {
                    "final": {
                      "type": "boolean"
                    },
                    "post": {
                      "type": "boolean"
                    },
                    "pre": {
                      "type": "boolean"
                    }
                  },
                  "required": [
                    "pre",
                    "post",
                    "final"
                  ],
                  "type": "object"
                },
                "last_event_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "message": {
                  "maxLength": 1024,
                  "type": "string"
                },
                "missing_artifacts": {
                  "items": {
                    "enum": [
                      "stdout.txt",
                      "stderr.txt",
                      "ipc-pre.json",
                      "ipc-post.json",
                      "ipc-final.json",
                      "observer-events.json",
                      "harness-diagnostics.txt"
                    ]
                  },
                  "maxItems": 7,
                  "type": "array",
                  "uniqueItems": true
                },
                "phase": {
                  "enum": [
                    "preflight",
                    "namespace",
                    "pre_inventory",
                    "construction",
                    "q0",
                    "q1",
                    "shutdown",
                    "cleanup",
                    "capture",
                    "validation",
                    "unknown",
                    "finalize"
                  ]
                }
              },
              "required": [
                "code",
                "phase",
                "message",
                "last_event_sequence",
                "inventory_availability",
                "cleanup_outcome",
                "missing_artifacts"
              ],
              "type": "object"
            }
          ]
        },
        "inventories": {
          "additionalProperties": false,
          "properties": {
            "final": {
              "additionalProperties": false,
              "properties": {
                "available": {
                  "type": "boolean"
                },
                "complete": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "is_empty": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "reason": {
                  "maxLength": 512,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "sha256": {
                  "pattern": "^[0-9a-f]{64}$",
                  "type": [
                    "string",
                    "null"
                  ]
                }
              },
              "required": [
                "available",
                "is_empty",
                "sha256",
                "reason",
                "complete"
              ],
              "type": "object"
            },
            "post": {
              "additionalProperties": false,
              "properties": {
                "available": {
                  "type": "boolean"
                },
                "complete": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "is_empty": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "reason": {
                  "maxLength": 512,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "sha256": {
                  "pattern": "^[0-9a-f]{64}$",
                  "type": [
                    "string",
                    "null"
                  ]
                }
              },
              "required": [
                "available",
                "is_empty",
                "sha256",
                "reason",
                "complete"
              ],
              "type": "object"
            },
            "pre": {
              "additionalProperties": false,
              "properties": {
                "available": {
                  "type": "boolean"
                },
                "complete": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "is_empty": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "reason": {
                  "maxLength": 512,
                  "type": [
                    "string",
                    "null"
                  ]
                },
                "sha256": {
                  "pattern": "^[0-9a-f]{64}$",
                  "type": [
                    "string",
                    "null"
                  ]
                }
              },
              "required": [
                "available",
                "is_empty",
                "sha256",
                "reason",
                "complete"
              ],
              "type": "object"
            }
          },
          "required": [
            "pre",
            "post",
            "final"
          ],
          "type": "object"
        },
        "observations": {
          "additionalProperties": false,
          "properties": {
            "counter_overflow": {
              "type": "boolean"
            },
            "q0": {
              "additionalProperties": false,
              "properties": {
                "controlled_application_offers": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "empty_observation_delta": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "end_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "start_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                }
              },
              "required": [
                "start_sequence",
                "end_sequence",
                "controlled_application_offers",
                "empty_observation_delta"
              ],
              "type": "object"
            },
            "q1": {
              "additionalProperties": false,
              "properties": {
                "actual_due_branch": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "actual_popped": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "correlation_status": {
                  "enum": [
                    "PENDING",
                    "MATCHED",
                    "DECISION_NOT_REACHED",
                    "HEAD_MISMATCH",
                    "INVALID",
                    "OVERFLOW",
                    null
                  ]
                },
                "decision_candidate_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "decision_candidate_time": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "decision_iteration_id": {
                  "maximum": 18446744073709551615,
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "decision_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "end_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "first_target_wait_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "gateway_iteration_id": {
                  "maximum": 18446744073709551615,
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "head_time_at_end": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "process_time_at_end": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "start_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_message_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_pop_count": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_run_entry_count": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "target_run_return_count": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_depth": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_head_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_head_time": {
                  "type": [
                    "number",
                    "null"
                  ]
                },
                "wait_iteration_id": {
                  "maximum": 18446744073709551615,
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_predicate_return": {
                  "type": [
                    "boolean",
                    "null"
                  ]
                },
                "wait_sequence": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_target_id": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                },
                "wait_timeout_ms": {
                  "minimum": 0,
                  "type": [
                    "integer",
                    "null"
                  ]
                }
              },
              "required": [
                "start_sequence",
                "end_sequence",
                "target_message_id",
                "first_target_wait_sequence",
                "target_pop_count",
                "target_run_entry_count",
                "target_run_return_count",
                "head_time_at_end",
                "process_time_at_end",
                "gateway_iteration_id",
                "wait_sequence",
                "wait_iteration_id",
                "wait_target_id",
                "wait_head_id",
                "wait_head_time",
                "wait_depth",
                "wait_timeout_ms",
                "wait_predicate_return",
                "decision_sequence",
                "decision_iteration_id",
                "decision_candidate_id",
                "decision_candidate_time",
                "actual_due_branch",
                "actual_popped",
                "correlation_status"
              ],
              "type": "object"
            },
            "target_message_id": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            }
          },
          "required": [
            "target_message_id",
            "q0",
            "q1",
            "counter_overflow"
          ],
          "type": "object"
        },
        "process_identity": {
          "additionalProperties": false,
          "properties": {
            "ipc_key": {
              "const": 11
            },
            "name": {
              "const": "NG039TestProcess"
            }
          },
          "required": [
            "name",
            "ipc_key"
          ],
          "type": "object"
        },
        "record_finalized": {
          "type": "boolean"
        },
        "run_id": {
          "maxLength": 64,
          "minLength": 16,
          "pattern": "^NG039-D2-[A-Za-z0-9][A-Za-z0-9._-]{7,47}$",
          "type": "string"
        },
        "schema_version": {
          "const": "ng039-d2-run-record/v2"
        },
        "spec_sha256": {
          "const": "f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26"
        },
        "status": {
          "enum": [
            "PASS",
            "FAIL",
            "INCONCLUSIVE"
          ]
        },
        "task_id": {
          "const": "NG-039"
        }
      },
      "required": [
        "schema_version",
        "task_id",
        "run_id",
        "spec_sha256",
        "baseline_commit",
        "baseline_tree",
        "document_head",
        "configured_targets",
        "process_identity",
        "status",
        "exit",
        "assertions",
        "observations",
        "inventories",
        "cleanup",
        "failure",
        "artifacts",
        "record_finalized"
      ],
      "title": "SPEC-039 D2 v2 staged/final runtime result record",
      "type": "object"
    }
  },
  "fixture": {
    "bootstrap_deadline_s": 5,
    "expected_block_count": 2,
    "future_horizon_s": 10,
    "harness_must_not_call_newblock_for_ht_or_gw": true,
    "key": 11,
    "process_constructor_creates_blocks_in_order": [
      "HT",
      "GW"
    ],
    "process_name": "NG039TestProcess",
    "q0_application_offers": 0,
    "q0_empty_observation_delta_required": true,
    "q0_window_ms": 100,
    "q1_window_ms": 100,
    "target_message": {
      "action_match": "none; neither exact registered action nor 0.1 fallback",
      "admit_after_q0_via": "GW::PushToInputQueue",
      "command_lines": [
        "-ng039 --probe 0.2",
        "-ng039 --probe 0.2",
        "-ng039 --probe 0.2"
      ],
      "creation": "Process::NewMessage before Gateway thread starts",
      "has_payload": false,
      "initial_delete_flag": false,
      "owner": "Process",
      "time_expression": "Process::GetTime()+10.0",
      "type": 0
    }
  },
  "local_path_audit": {
    "d1h_rebuild_root": "${WORKSPACE_ROOT}/ng039-d1h-rebuild-20261010",
    "d1h_rebuild_root_outside_hermes": true,
    "d2_aliases_must_be_resolved_and_pairwise_checked_before_any_execution": true,
    "d2_build_directories_created": false,
    "d2_execution_leaf_must_not_be_created_before_positive_review_and_separate_owner_gate": true,
    "d2_harness_or_ipc_executed": false,
    "d2_worktree_created": false,
    "r07_manifest_created_before_packet_freeze": true,
    "r07_outputs_were_not_present_at_pre-freeze_source_snapshot": false,
    "r07_packet_created_after_manifest_freeze": true,
    "review_directory": "${NG_REPO_ROOT}/Specs/RESULTS-SPEC-039",
    "review_directory_is_symlink": false
  },
  "local_validation_this_freeze": {
    "astra_submitted": true,
    "d1h_fresh_compile_link_rebuild": "PASS, exit 0; no runtime",
    "fresh_astra_consent_for_r07": false,
    "ipc_vm_d3_or_performance_measurement": false,
    "python_optimize_mode": "PASS: /usr/bin/python3 with PYTHONOPTIMIZE=1 exercised the declared synthetic checker matrix; see Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json.",
    "r07_checker_entrypoints": "PASS: without an unconsumed -- separator, literal complete compile default/diagnostic and link commands accept correct inputs; missing build-root/target inputs reject in both modes.",
    "r07_checker_syntax": "PASS, compile and link checker snippets both compile; neither contains Python assert",
    "r07_compile_checker_fixtures": "PASS: 2 profiles, 15 distinct negatives rejected in each mode; includes joined -include/-imacros.",
    "r07_compile_checker_synthetic": "PASS: 2 positives and 15/15 negatives rejected in both normal Python and PYTHONOPTIMIZE=1 (30 negative executions).",
    "r07_cross_artifact_finalizer_execution": "not executed; finalizer is a proposed same-binary D2 contract, not implemented.",
    "r07_finalizer_failure_summary_fixtures": "Design cases frozen in manifest; no finalizer implementation/execution, so cross-artifact behavior remains unverified.",
    "r07_link_checker_fixtures": "PASS: valid link plus stdin-binding control, 8 negatives rejected in each mode; missing target rejects in both modes.",
    "r07_link_checker_synthetic": "PASS: 1 stdin-binding positive and 8/8 negatives rejected in both modes (16 negative executions).",
    "r07_python_optimize_mode": "PASS: B1/B2 exact-command and full checker probes ran with /usr/bin/python3 plus PYTHONOPTIMIZE=1.",
    "r07_run_record_fixtures": "PASS at schema layer: valid PASS/safe FAIL accepted; claimed PASS contradictions and unsafe/missing-evidence FAIL rejected; semantic finalizer not executed.",
    "r07_run_record_schema_cases": "PASS: valid PASS/safe FAIL accepted; null candidate/iteration identity, nonzero pop, unsafe FAIL cleanup and missing FAIL artifact rejected.",
    "r07_schema_meta_validation": "PASS, 3/3 Draft 2020-12 schemas",
    "real_d2_compile_database_configure_build_runtime": false
  },
  "observer": {
    "correlation_rule": [
      "wait and decision iteration_id equal",
      "decision.sequence greater than wait.sequence",
      "wait target_id and head_id equal decision candidate_id",
      "only then correlation_status=MATCHED; otherwise preserve PENDING/DECISION_NOT_REACHED/HEAD_MISMATCH/INVALID/OVERFLOW"
    ],
    "decision_capture_fields": [
      "sequence",
      "iteration_id",
      "candidate_id",
      "candidate_time",
      "actual_due_branch",
      "actual_popped"
    ],
    "event_sequence_bits": 64,
    "gateway_iteration_id_one_per_loop": true,
    "gateway_started_one_way": true,
    "iteration_id_bits": 64,
    "iteration_id_saturates": true,
    "run_identity_capture": "Copy Message::InstantiationNumber to a scalar immediately before each production Run; use the copy for RunEntry/RunReturn before DeleteMarkedMessages; never dereference a Message after reclamation.",
    "sequence_and_counters": "saturate; set overflow flag; fail inconclusive",
    "sticky_counts": [
      "admission",
      "non_due",
      "pop",
      "Run_entry",
      "Run_return",
      "empty_queue"
    ],
    "sticky_decision_capture": true,
    "sticky_first_target_wait_record": true,
    "stores_message_pointer": false,
    "target_wait_fields": [
      "sequence",
      "iteration_id",
      "target_id",
      "head_id",
      "head_time",
      "depth",
      "timeout_ms",
      "wait_return"
    ],
    "update_sites": [
      {
        "event": "target admission after valid NoCL>2 insertion under InputQueueMutex",
        "file": "Common/src/GW.cpp",
        "function": "GW::PushToInputQueue",
        "lines": [
          219,
          267
        ]
      },
      {
        "event": "one-way gateway_started and locked queue-head/empty snapshot",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          451,
          467
        ]
      },
      {
        "event": "saturating gateway_iteration_id increment once at loop-iteration start",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          451,
          452
        ]
      },
      {
        "event": "actual wait_for result; sticky first target wait if target is head",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          470,
          480
        ]
      },
      {
        "event": "strict due decision and actual pop/non-due outcome under the queue mutex",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          482,
          503
        ]
      },
      {
        "event": "scalar target Run entry/return around production Run call before reclamation",
        "file": "Common/src/GW.cpp",
        "function": "GW::Gateway",
        "lines": [
          506,
          597
        ]
      },
      {
        "event": "stop-request observation outside queue-lock recursion",
        "file": "Common/src/GW.cpp",
        "function": "GW::SetStopGatewayFlag",
        "lines": [
          1331,
          1337
        ]
      }
    ]
  },
  "parent_task_id": "NG-039",
  "path_aliases": {
    "D1H_REBUILD_ROOT": "${WORKSPACE_ROOT}/ng039-d1h-rebuild-20261010",
    "D1H_WORKTREE": "isolated historical D1-H source worktree at the immutable functional baseline",
    "D2_RESULTS": "${NG_REPO_ROOT}/Specs/RESULTS-SPEC-039/ng039-d2-v1.10-20261010",
    "D2_SCRATCH": "${WORKSPACE_ROOT}/ng039-d2-scratch-v1.10-1af604d",
    "D2_WORKTREE": "${WORKSPACE_ROOT}/novagenesis-ng039-d2-v1.10-1af604d",
    "NG_REPO_PARENT": "parent of the active AIOPT5 checkout",
    "NG_REPO_ROOT": "active AIOPT5 checkout root",
    "WORKSPACE_ROOT": "workspace root; outside .hermes",
    "absolute_resolution_required_before_execution": true,
    "containment_rule": "Resolve each alias once before any command; reject symlink/overlap and require the worktree, build roots, HOME, TMPDIR and result leaf to remain under their named roots; all are outside .hermes."
  },
  "planned_results": {
    "artifacts_not_generated": true,
    "binary_sha256": null,
    "compile_commands_sha256": null,
    "default_binary_sha256": null,
    "diagnostic_binary_sha256": null,
    "finalizer_exit_code": null,
    "inventory_hashes": null,
    "process_exit_code": null,
    "run_id": null,
    "run_record_sha256": null,
    "status": null
  },
  "prior_review": {
    "archive_manifest_sha256": "27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a",
    "one_call_consent_consumed": true,
    "packet_sha256": "1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a",
    "parsed_response_sha256": "9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc",
    "raw_sse_sha256": "3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31",
    "response_id": "resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863",
    "scope": "advisory textual pre-code review only; no implementation/runtime authority",
    "verdict": "AMEND"
  },
  "r06_review": {
    "archive_sha256": "7d57b56e73546c15ca0b437439f361223acbb19baf76890d6be381f01ed89209",
    "authority": "advisory textual inspection only; no implementation/configure/build/runtime/IPC/VM/D2/D3 authority",
    "manifest_sha256": "3bf3a4284c163b35c2e08ffcb196b9bd19adb5aed9dd9b5c0f0b501454f360b2",
    "must_fixes": [
      "compile source/output/build-root containment and exactly one -c",
      "freeze a deterministic environment for configure/build/checkers",
      "read and bind the exact link.txt; reject forwarded linker controls",
      "prevent schema-valid PASS/FAIL contradictions using local schema constraints plus a bounded semantic finalizer"
    ],
    "one_call_consent_consumed": true,
    "packet_sha256": "cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711",
    "parsed_response_sha256": "9058bc8a6490b1774d0bd0fb8f185dbaa8669c73353faffffd87d8ba2cd75a2e",
    "raw_sse_sha256": "66a73a0ddb4884b591dbe2d3f78aed696da4d1c1a2fb206085e29e1fbc767cc0",
    "response_id": "resp_0e96e100afdf6c5d016aca6a11615c81919e900a168d341cf9",
    "spec_sha256": "2477ac94b67ecf6e81665fe2c8899cbfdff31edec08830dc2b60ebbaa7b0d0d0",
    "verdict": "AMEND"
  },
  "review_artifacts": {
    "manifest_path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.10-r07.json",
    "packet_path": "Specs/RESULTS-SPEC-039/precode-packet-v1.10-amend-closure-20261010-sanitized-draft-r07.md",
    "packet_sha256": "computed after final packet bytes; not embedded to avoid a hash cycle",
    "prior_submitted_manifest_sha256": "e193e8f9f6c812145b4da4ffae507bf9aa997847bae172ade2d001d164cfc9c5",
    "prior_submitted_packet_sha256": "f6fa7f178d433c6b1ccdf86bb2a915ae4acf1c567ae8b428ad351c524681cb15",
    "submission_state": "prior r07 and B1/B2 snapshots were each submitted once and returned AMEND; current same-file working amendment resolves B3 provenance binding and is not submitted; fresh exact-SHA Astra consent required"
  },
  "review_date": "2026-10-10",
  "review_revision": "r07",
  "review_scope": "Owner-authorized in-place documentary correction of Astra r07 AMEND B1/B2/B3 only. This working amendment is not submitted. No D2 implementation/configure/build/runtime, IPC/VM, D3 or performance claim is authorized.",
  "runtime_bundle": {
    "max_artifact_bytes": 4194304,
    "max_artifacts_excluding_run_record": 7,
    "max_files_total": 8,
    "max_run_record_bytes": 262144,
    "max_sysv_shm_segment_bytes": 33554532,
    "max_sysv_shm_segments": 5,
    "max_sysv_shm_total_bytes": 167772660,
    "max_total_bytes": 33554432,
    "optional_files": [
      "harness-diagnostics.txt"
    ],
    "required_files": [
      "run-record.json",
      "stdout.txt",
      "stderr.txt",
      "ipc-pre.json",
      "ipc-post.json",
      "ipc-final.json",
      "observer-events.json"
    ],
    "run_record_sha256_location": "outer archive index; not self-referenced",
    "staging_file": {
      "deleted_after_finalization": true,
      "included_in_final_bundle": false,
      "maximum_bytes": 262144,
      "name": "run-record.pending.json"
    },
    "tracked_plan_manifest_is_not_runtime_file": true
  },
  "sandbox": {
    "cwd": "$D2_RESULTS",
    "dev_shm_tmpfs": {
      "nr_inodes": 128,
      "options": [
        "noexec",
        "nosuid",
        "nodev"
      ],
      "size_mib": 16
    },
    "empty_pre_inventory_required": true,
    "environment_contract_ref": "cmake.environment_contract",
    "finalizer_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/prlimit --core=0 --as=134217728 --fsize=4194304 --nofile=32 --cpu=10 -- /usr/bin/timeout --signal=TERM --kill-after=5s 10s \"$D2_SCRATCH/build-diagnostic/ng039_gw_q0_q1_d2\" --finalize \"$D2_RESULTS\" \"$RUN_ID\" --outer-code \"$OUTER_CODE\" --outer-signal \"$OUTER_SIGNAL\" --timeout-fired \"$TIMEOUT_FIRED\"",
    "finalizer_exit_is_separate_from_test_exit": true,
    "finalizer_kill_grace_s": 5,
    "finalizer_rlimits": {
      "RLIMIT_AS_bytes": 134217728,
      "RLIMIT_CORE_bytes": 0,
      "RLIMIT_CPU_seconds": 10,
      "RLIMIT_FSIZE_bytes": 4194304,
      "RLIMIT_NOFILE": 32
    },
    "finalizer_timeout_s": 10,
    "harness_spawns_children": false,
    "host_fallback": false,
    "kill_grace_s": 5,
    "launch_command": "env -i PATH=/usr/bin:/bin HOME=\"$D2_SCRATCH/home\" TMPDIR=\"$D2_SCRATCH/tmp\" LANG=C LC_ALL=C /usr/bin/prlimit --as=536870912 --fsize=4194304 --nofile=64 --core=0 --cpu=30 -- /usr/bin/timeout --signal=TERM --kill-after=5s 20s /usr/bin/unshare --user --map-root-user --ipc --mount --pid --propagation private --fork --kill-child --mount-proc \"$D2_SCRATCH/build-diagnostic/ng039_gw_q0_q1_d2\" --run \"$RUN_ID\"",
    "launch_stdout_stderr": "Parent opens stdout.txt/stderr.txt under D2_RESULTS before unshare; closes descriptors before finalizer.",
    "mount_propagation": "private",
    "namespaces": [
      "user",
      "ipc",
      "mount",
      "pid"
    ],
    "outer_status_capture": {
      "ambiguous_status": "128-255 without independent signal evidence is INCONCLUSIVE; do not infer signal from shell exit code",
      "outer_code": "raw exit status from timeout wrapper",
      "outer_signal": "populate only from unambiguous independent wait evidence; otherwise null",
      "timeout_fired": "true on supervisor timeout; code 124 conservatively treated as timeout"
    },
    "rlimits": {
      "RLIMIT_AS_bytes": 536870912,
      "RLIMIT_CORE_bytes": 0,
      "RLIMIT_CPU_seconds": 30,
      "RLIMIT_FSIZE_bytes": 4194304,
      "RLIMIT_NOFILE": 64
    },
    "sysv_shm": {
      "basis": "one ReturnIPCSHMID(11) setup segment plus four Process shmid[] parallel slots; fail INCONCLUSIVE above bound",
      "expected_initial_key": 11,
      "max_segment_bytes": 33554532,
      "max_segments": 5,
      "max_total_bytes": 167772660
    },
    "timeout_s": 20
  },
  "schema_version": "ng039-spec039-precode-review-manifest/v1.10",
  "scope": {
    "algorithm_change": false,
    "correctness_only": true,
    "d3_timing": false,
    "ngal_or_wire_time_coverage": false,
    "production_vm_or_host_ipc_fallback": false
  },
  "semantic_finalizer_contract": {
    "artifact_checks": [
      "exact allowlisted file set",
      "six required artifact names exactly once",
      "optional harness-diagnostics.txt at most once",
      "reject symlink/non-regular files",
      "check byte caps and SHA-256 against actual closed files",
      "check total bundle cap"
    ],
    "assertion_to_phase": {
      "bootstrap_queue_drained": "bootstrap",
      "evidence_complete": "finalize",
      "final_inventory_complete": "final_inventory",
      "final_inventory_empty": "final_inventory",
      "finalization_complete": "finalize",
      "first_target_wait_fresh": "q1",
      "gateway_joined": "shutdown",
      "namespace_setup": "namespace",
      "output_worker_joined": "shutdown",
      "post_inventory_complete": "post_inventory",
      "post_resources_attributable": "post_inventory",
      "post_resources_removed": "cleanup",
      "pre_inventory_complete": "pre_inventory",
      "pre_inventory_empty": "pre_inventory",
      "process_blocks_verified": "construction",
      "q0_empty_observation_delta": "q0",
      "q0_zero_controlled_offers": "q0",
      "scalar_run_identity_valid": "q1",
      "target_admitted": "q1",
      "target_delete_false_before_gateway": "construction",
      "target_erased_once": "shutdown",
      "target_future_at_end": "q1",
      "target_is_head": "q1",
      "target_not_popped": "q1",
      "target_not_run": "q1",
      "wait_decision_correlated": "q1"
    },
    "expected_outcome_rule": "Every negative fixture is rejected as a PASS/FAIL claim and finalized INCONCLUSIVE with the listed bounded reason; a finalizer error/no finalized record is also never PASS.",
    "failure_summary_checks": [
      "failure.code is recomputed from the highest-precedence observed condition; ASSERTION_FAILED is used only for a demonstrated correctness predicate failure after all evidence/safety gates pass.",
      "failure.phase must be the phase mapped to the selected failed predicate or uncertainty condition.",
      "failure.missing_artifacts equals the exact required artifact names absent from the frozen result directory; FAIL requires the empty list.",
      "failure.inventory_availability is derived from actual inventory-file presence and successful parsing, not caller-provided booleans.",
      "failure.cleanup_outcome is derived from stop/join/ownership/removal/final-inventory evidence; FAIL requires complete.",
      "failure.last_event_sequence equals the greatest validated event sequence, or null only when no valid event exists.",
      "message is bounded diagnostic text; code/phase/availability/cleanup/missing-artifact fields are authoritative, and any contradiction yields INCONCLUSIVE/EVIDENCE_INVALID."
    ],
    "inventory_checks": [
      "schema, run_id and phase agree with record role",
      "is_empty recomputed from SysV IDs and /dev/shm entries",
      "SHM size total recomputed from segment sizes",
      "IDs unique and resource caps enforced",
      "pre empty",
      "post complete and attributable",
      "final empty"
    ],
    "negative_fixtures": [
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed PASS with null Q1 decision identity"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed PASS with target_pop_count greater than zero"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed FAIL with unknown/incomplete cleanup"
      },
      {
        "expected_failure_code": "EVIDENCE_MISSING",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed FAIL with missing required artifact"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "failure summary code/phase/availability/cleanup disagrees with observed evidence"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "record/artifact run_id, hash or sequence mismatch"
      }
    ],
    "observer_checks": [
      "event sequence strictly increases and no overflow",
      "counters are monotone",
      "Q0 assertion recomputed from offer and empty-observation deltas",
      "Q1 wait/decision iteration IDs match",
      "decision sequence greater than wait sequence",
      "target/head/candidate IDs match",
      "due branch and actual pop are false",
      "target pop/run counters stay zero",
      "future head remains future at window end"
    ],
    "positive_fixtures": [
      "PASS",
      "safe FAIL",
      "finalized INCONCLUSIVE",
      "non-finalized pending INCONCLUSIVE"
    ],
    "schema_boundary": "JSON Schema checks shape and simple local status/value implications; the finalizer owns cross-artifact/equality/ordering rules so they are not duplicated.",
    "single_authority": "same allowlisted ng039_gw_q0_q1_d2 binary --finalize mode; no extra runner/helper file",
    "status_precedence": [
      "PASS only if all recomputed acceptance predicates and safe cleanup pass",
      "FAIL only on an observable correctness assertion failure with complete evidence and safe cleanup",
      "otherwise INCONCLUSIVE"
    ],
    "strict_json": [
      "reject duplicate object keys",
      "reject NaN/Infinity",
      "validate run-record, IPC inventory and observer event schemas"
    ]
  },
  "source": {
    "baseline_file_sha256": {
      "CMakeLists.txt": "5f9cd0676b2dcdcd0a500d778f5d5baea312301d6abc382db5eab95f2bbb1d07",
      "Common/src/GW.cpp": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e",
      "Common/src/GW.h": "54a97c54fe75fd76e157afc1c70ebce370f3c6edab33233babab3f3fc68f0b44",
      "Common/src/Message.cpp": "96f718f16dfa34fb081c02df872a3959a4255180798e6022e61d198905035518",
      "Common/src/Process.cpp": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1"
    },
    "document_checkout_branch": "AIOPT5",
    "document_checkout_head": "756f493b7049434ab67cf2382486a48f606a470c",
    "document_checkout_relation": "local HEAD is two commits ahead of origin/AIOPT5; review packet is a working-tree artifact, not checked-in",
    "document_checkout_tracking": "origin/AIOPT5",
    "document_checkout_tree": "9adf665c0eb06213a29a6c12d650451948c7a4ab",
    "functional_baseline_commit": "1af604dad66148618cc0e578c28adb4a9829e607",
    "functional_baseline_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
    "primary_checkout_status_at_freeze_input": {
      "r07_manifest_created": true,
      "r07_outputs_not_yet_created": false,
      "r07_packet_not_yet_created": true,
      "spec039_modified": true,
      "spec062_modified_unrelated": true,
      "unrelated_changes_included_or_validated": false,
      "unrelated_modified_path_present": true,
      "unrelated_untracked_results_path_present": true,
      "untracked_results_spec039_present": true,
      "untracked_results_spec059_unrelated": true
    },
    "reviewed_spec_clean_in_primary_worktree": false,
    "source_for_code_claims": "immutable baseline Git objects only",
    "unrelated_changes_included_or_validated": false,
    "unrelated_primary_worktree_changes_present": true,
    "working_tree_diff_sha256_for_spec039": "09ccf21b1925f33224431061c6282d2b958b14d077bc9e69126cac8c30bdf326"
  },
  "source_snapshots_before_packet_freeze": {
    "dashboard_sha256": "324d834ea944f95f9335afa4b8160713b5c5126e1666ceea476959897eea8649",
    "ng039_task_sha256": "5cc1d4993ab8d9d30a3abbea0e63f42b3ffa3524933e93eae1008acfa7b73035",
    "ng040_task_sha256": "23e4c759a3f9dac965bd19d917f352a68f32da29980a6935101863437eb302d3",
    "post_freeze_note": "Task/Dashboard hashes and D1-H receipt are recorded after packet freeze; these handoff changes are not fed back into the reviewed packet.",
    "snapshot_timing": "2026-10-10 before r07 packet creation and before post-freeze NG-039/NG-040/Dashboard handoff updates"
  },
  "spec": {
    "change_from_v1_9": "Owner-directed bounded response to r06 AMEND in the same SPEC number: retain direct provenance and no-false-PASS checks; relax generic argument parsing, exhaustive environment denylisting and duplicated schema/finalizer logic.",
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26",
    "status": "In Progress",
    "version": "v1.10"
  },
  "synthetic_validation": {
    "compile_negative_cases": [
      "source outside worktree",
      "output outside build root",
      "output metadata mismatch",
      "duplicate -c",
      "wrong compiler realpath",
      "-Wp observer forwarding",
      "-Xpreprocessor observer forwarding",
      "default-profile observer undefine",
      "-ansi alternate dialect",
      "later non-C++20 standard",
      "non--O0 optimization",
      "target census mismatch",
      "diagnostic function-like observer define",
      "joined -include observer override",
      "joined -imacros observer override"
    ],
    "current_working_amendment_validation": {
      "compile_negative_cases_per_mode": 15,
      "compile_negative_rejections_total": 30,
      "compile_positive_profiles_per_mode": 2,
      "entrypoint_missing_operand_rejections_per_mode": 2,
      "link_negative_cases_per_mode": 8,
      "link_negative_rejections_total": 16,
      "link_positive_stdin_binding_per_mode": 1,
      "modes": [
        "normal",
        "PYTHONOPTIMIZE=1"
      ],
      "result": "PASS: all listed positives accepted and negatives rejected.",
      "scope": "B1/B2 correction only; complete checker matrix rerun synthetically, no D2 build/runtime/finalizer/IPC/VM",
      "total_checks_both_modes": 56
    },
    "harness_execution": false,
    "ipc_vm_d3_or_performance": false,
    "link_negative_cases": [
      "wrong link.txt path",
      "missing standalone -static",
      "duplicate standalone -static",
      "wrong linker compiler realpath",
      "@response file",
      "-Wl,-shared",
      "split -Xlinker -B -Xlinker dynamic",
      "symlinked link.txt"
    ],
    "purpose": "Bounded local probes of the frozen r07 checker/schema contract only; not D2 build/runtime/performance evidence.",
    "python_optimize_mode": "PASS: full amended checker matrix and literal entrypoint checks ran under normal Python and PYTHONOPTIMIZE=1; see the sidecar receipt.",
    "r07_local_preflight": {
      "PYTHONOPTIMIZE": "PASS: /usr/bin/python3 with PYTHONOPTIMIZE=1 exercised the declared synthetic checker matrix; see Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json.",
      "checker_syntax": "PASS 2/2; no Python assert",
      "compile_checker": "PASS: 2 positive profiles; 13 distinct negative fixtures rejected under normal and PYTHONOPTIMIZE=1 (26 negative executions).",
      "cross_artifact_finalizer": "contract specified only; no executable finalizer to test.",
      "finalizer_semantic_fixtures": "expected outcomes frozen; execution pending",
      "finalizer_semantics": "Specification/fixtures frozen, execution not performed because the D2 finalizer is not implemented.",
      "link_checker": "PASS: positive link file and stdin-binding control; 8 distinct negative fixtures rejected under normal and PYTHONOPTIMIZE=1 (16 negative executions).",
      "python_optimize_mode": "PASS with system /usr/bin/python3 and PYTHONOPTIMIZE=1; receipt Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json",
      "r07_checker_entrypoints": "PASS: without an unconsumed -- separator, literal complete compile default/diagnostic and link commands accept correct inputs; missing build-root/target inputs reject in both modes.",
      "r07_compile_checker_fixtures": "PASS: 2 profiles, 15 distinct negatives rejected in each mode; includes joined -include/-imacros.",
      "r07_compile_checker_synthetic": "PASS: 2 positives and 15/15 negatives rejected in both normal Python and PYTHONOPTIMIZE=1 (30 negative executions).",
      "r07_link_checker_fixtures": "PASS: valid link plus stdin-binding control, 8 negatives rejected in each mode; missing target rejects in both modes.",
      "r07_link_checker_synthetic": "PASS: 1 stdin-binding positive and 8/8 negatives rejected in both modes (16 negative executions).",
      "r07_python_optimize_mode": "PASS: B1/B2 exact-command and full checker probes ran with /usr/bin/python3 plus PYTHONOPTIMIZE=1.",
      "real_d2_configure_build_runtime": "not run",
      "run_record_schema": "PASS: valid PASS/safe FAIL; claimed PASS contradiction and unsafe/missing-evidence FAIL cases rejected.",
      "schema_meta_validation": "PASS 3/3"
    },
    "real_compile_database_or_link_txt": false,
    "real_d2_configure_or_build": false,
    "run_record_negative_cases": [
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed PASS with null Q1 decision identity"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed PASS with target_pop_count greater than zero"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed FAIL with unknown/incomplete cleanup"
      },
      {
        "expected_failure_code": "EVIDENCE_MISSING",
        "expected_final_status": "INCONCLUSIVE",
        "input": "claimed FAIL with missing required artifact"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "failure summary code/phase/availability/cleanup disagrees with observed evidence"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "record/artifact run_id, hash or sequence mismatch"
      }
    ]
  },
  "task_id": "NG-040"
}
===== END FILE: Specs/RESULTS-SPEC-039/precode-manifest-v1.10-r07.json =====

## r06 response retained as the amendment baseline

===== FILE: Specs/RESULTS-SPEC-039/astra-review-r06-20261010.md | SHA-256 9058bc8a6490b1774d0bd0fb8f185dbaa8669c73353faffffd87d8ba2cd75a2e =====
## AMEND — advisory pre-code review only

**Packet reviewed:** `cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711`

The supplied text addresses the three specifically summarized r05 defects, but the frozen checker/manifest contract still has material gaps. **This exact packet is not ready for documentary closure.**

### r05 must-fix disposition

| Summarized r05 issue | Disposition from textual inspection |
|---|---|
| Forwarded/function-like observer macros | **Closed for the identified spellings at design level.** The compile checker rejects `-Wp*` and `-Xpreprocessor`, extracts function-like macro base names, and requires exactly one diagnostic object-like `=1` definition. |
| `-ansi` dialect override | **Closed at design level.** The supported grammar rejects `-ansi` and requires exactly one `-std=gnu++20`. |
| Python `assert` disabled by optimization | **Closed at design level.** The supplied link checker uses explicit exceptions/nonzero termination, not `assert`. |

These dispositions do **not** establish that the full build-verification contract is complete or implemented.

## Must-fix findings

### 1. Compile-checker path containment is specified but not enforced

**Location:** SPEC §7 item 4; `cmake.compile_commands_verification.checker_logic`.

The SPEC requires directories and outputs under the selected fresh build root and sources under the pinned worktree. The checker receives only the database and profile; it has no trusted root inputs and performs no containment checks.

**Concrete bypass:** In an otherwise accepted diagnostic database, use an external directory and external source, with an output such as:

`/outside/build/CMakeFiles/Common.dir/foreign.cpp.o`

Make `file` agree with the source operand and `output` agree with `-o`; retain the required flags and a corresponding harness entry. The checker can accept the expected target census despite neither source nor output belonging to the frozen roots.

**Required amendment:** Bind trusted build/worktree roots to validation and enforce normalized containment and nonempty metadata. Also enforce the promised exactly-one `-c`: currently repeated `-c` tokens are accepted. The output-derived target extraction is an improvement, but is not sufficient provenance validation.

### 2. The required environment contract is absent

**Location:** SPEC §7 item 4 references `environment_contract.unset`; the embedded manifest contains no such field.

Consequently, the promised pre-configure/build/checker environment sanitization is not frozen. Compiler-affecting inherited variables such as `CPATH`, `CPLUS_INCLUDE_PATH`, `COMPILER_PATH`, and `LIBRARY_PATH` are not necessarily visible in the inspected argument arrays.

**Required amendment:** Supply the exact environment contract, including removal rules and retained-value recording. Resolve how it applies consistently to configure, build, and checker invocations. Do not describe that contract as complete while its referenced field is missing.

### 3. Link validation has both an invocation mismatch and remaining override gaps

**Location:** `cmake.link_command_verification.planned_command` and `.checker_logic`.

- The planned invocation passes `link.txt` as a positional argument, but the checker reads **standard input** and never reads that argument. As frozen, it does not reliably inspect the named file; empty stdin rejects, while other stdin could validate unrelated text.
- The denylist misses forwarded controls. For example, an otherwise accepted token list with **`-Wl,-shared`** passes the checker: its `-Wl` branch does not reject `-shared`. Similarly, **`-Xlinker -B -Xlinker dynamic`** is not recognized as a dynamic-mode override.

These are textual checker counterexamples, not claims that a particular resulting link succeeds.

**Required amendment:** Bind the invocation to the actual named `link.txt`, and define a supported link-argument grammar or complete forwarding normalization with fail-closed handling of unsupported controls. Include these cases in the frozen fixture matrix. Assertion-independent validation fixes r05’s optimization defect, but does not by itself establish static-link correctness.

### 4. Schema-valid status is still insufficiently bound to observations

**Location:** `evidence_schemas.run_record.allOf`, especially the PASS branch, and the finalizer validation contract.

The new conditions substantially improve status consistency, but still admit contradictory or unavailable evidence.

**Concrete schema-level counterexamples:**

- An otherwise conforming PASS record can set Q0/Q1 measurement fields to `null`, including wait/decision identities and sequences, while retaining `correlation_status="MATCHED"` and assertion labels `PASS`.
- It can report `target_pop_count=1` or `actual_popped=true` while `target_not_popped="PASS"`.
- A schema-valid FAIL can report `failure.cleanup_outcome="unknown"` or missing required artifacts in `failure.missing_artifacts`, despite the other fields claiming complete evidence and cleanup.

The prose disallows these outcomes, but the schema does not establish them. No complete cross-artifact semantic validator is supplied.

**Required amendment:** Encode straightforward non-null/value/status implications in the schema and explicitly freeze the remaining finalizer semantic checks: sequence/identity correlation, observation-to-assertion consistency, inventory-content/hash binding, and failure-summary consistency. Contradictory or unavailable evidence must produce **INCONCLUSIVE**, not PASS or bounded FAIL.

## Optional findings

- Require artifact-name uniqueness, not merely `uniqueItems` on complete artifact objects. Different hashes for the same filename currently permit duplicate-name entries.
- Reject command-string glob syntax explicitly as promised by the SPEC. The current command check does not reject `*`, `?`, or bracket patterns.
- Reconcile synthetic-test provenance: several fields describe historical, unretained v1.8 fixtures or “not executed in r06,” while others claim this-freeze r06 validation. Distinguish historical reports from fresh checks and retained evidence.
- Correct the unterminated Markdown backtick around the manifest digest; this is editorial only.

## Evidence limits and retained design closures

- This was **textual inspection of the supplied packet only**. I did not execute checkers, validate schemas with a tool, recompute hashes, inspect Git objects, or access archived responses.
- The r05 response is available here only through its supplied non-verbatim summary and asserted archive digest.
- Synthetic outcomes are packet-reported claims, not independently reproduced results.
- D1-H remains a historical compile/link claim with unavailable binary/log rehash evidence—not runtime or correctness evidence.
- **Findings 3 and 4b–4d remain design-level closures only**, as summarized in this packet. Nothing here upgrades observer behavior, ownership safety, cleanup, isolation, or finalization into implemented or runtime-verified properties.

**Authority explicitly withheld:** This AMEND grants no implementation, file-edit, configure/build, execution, IPC/VM, D2 acceptance, D3, performance, or operational authority. No retry or additional review call is authorized by this response.
===== END FILE: Specs/RESULTS-SPEC-039/astra-review-r06-20261010.md =====

## D1-H rebuild-only receipt

===== FILE: Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json | SHA-256 3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28 =====
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
===== END FILE: Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json =====

## Synthetic validation receipt

The receipt is a separate sidecar, not embedded in this packet, to avoid a packet/receipt hash cycle. After the SPEC, manifest and packet are frozen, rerun the declared synthetic matrix against those exact bytes and write `Specs/RESULTS-SPEC-039/r07-synthetic-validation-20261010.json`. Record that receipt's SHA-256 out-of-band in NG-039/NG-040. Any earlier receipt snapshot is superseded for exact-input provenance; it is not a review or runtime result.
