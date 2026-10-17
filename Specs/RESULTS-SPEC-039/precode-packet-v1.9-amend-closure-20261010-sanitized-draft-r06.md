# SPEC-039 v1.9 — pre-code closure review packet r06 (sanitized draft)

Status: local preparation only; NOT submitted. Advisory documentary review only. No code edit, CMake configure/build, harness execution, IPC/VM operation, D3 timing, performance claim, or acceptance decision is requested or authorized by this packet. Date: 2026-10-10.

## Frozen identity and privacy

- Governing SPEC: `Specs/SPEC-039-common-performance-measurement.md`, v1.9, SHA-256 `2477ac94b67ecf6e81665fe2c8899cbfdff31edec08830dc2b60ebbaa7b0d0d0`.
- Document checkout: branch `AIOPT5`, HEAD `756f493b7049434ab67cf2382486a48f606a470c`, tree `9adf665c0eb06213a29a6c12d650451948c7a4ab`. The reviewed SPEC itself is clean; unrelated worktree changes are excluded. Local HEAD is two commits ahead of `origin/AIOPT5`; this packet is not committed or pushed.
- Functional source baseline: commit `1af604dad66148618cc0e578c28adb4a9829e607`, tree `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`. All code claims below bind to immutable baseline Git objects, not the dirty primary checkout.
- Task scope: NG-039 parent / NG-040 child. Pre-packet task snapshots: NG-039 SHA-256 `c36aad8d563fd0853a3fee575be4ab88e50481a0a70843f326a336499ced3d84`; NG-040 SHA-256 `f8797413f668e89dcbd021d7a3bbe590b1c052cc685366f48d754d302239e0b4`; Dashboard SHA-256 `57aed9be4fbe8109fb7cfb37e3dc8f2e53a056091ef7aceeeb8f157f149bf26a`. Both tasks remain `bloqueada`; the Dashboard identifies NG-039/NG-040 as the active AIOPT5 workstream. These are pre-freeze snapshot digests; the task/Dashboard handoff will record the final packet hash after freeze.
- The D2 changed-file allowlist remains exactly five files: `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`. This review does not authorize editing any of them.
- Sanitization: local checkout roots are represented by stable role aliases (`NG_REPO_ROOT`, `D1H_WORKTREE`, `D2_WORKTREE`, `D2_SCRATCH`, `D2_RESULTS`). No unrelated task details, user identifiers, credentials, or raw operational evidence are included.

## Prior review and response under review

Astra r05 reviewed packet SHA-256 `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a` and returned `AMEND` (response `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). The response file SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`, raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`, and archive-manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a` were rechecked against the locally archived artifacts.

The three r05 must-fixes were: (1) forwarded/function-like observer macro definitions could evade scanning; (2) `-ansi` could bypass the intended C++20 dialect; and (3) Python `assert` could disappear under optimization. The optional hardening was actual `-o` target binding, dynamic-link override rejection, and schema-level PASS/FAIL consistency. The v1.8 response is recorded in the same SPEC number and is not a new SPEC or code authorization.

## D1-H evidence boundary

The isolated D1-H manifest SHA-256 `9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a` matches its recorded bytes. Its source CMake/harness hashes match the recorded source values. The manifest records compile/link exit 0 and explicitly records that the executable was not run. The prior binary and build-log paths are no longer available, so their recorded hashes cannot be independently rechecked in this session; D1-H is therefore a historical compile/link claim, not runtime or correctness evidence.

The immutable baseline CMake source has SHA-256 `5f9cd0676b2dcdcd0a500d778f5d5baea312301d6abc382db5eab95f2bbb1d07`. The relevant target declarations and default-off legacy option are:

```text
1:cmake_minimum_required(VERSION 3.5.1)
2:project(novagenesis)
3:
4:set(CMAKE_CXX_STANDARD 20)
5:
6:option(NG_ENABLE_LEGACY_STANDALONE
7:       "Build deprecated standalone PSS, GIRS and HTS services"
8:       OFF)
25:add_library(Common
144:set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer")
145:set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++") # SPEC-MUSL-001: musl static link fix (cherry-pick e714ab1)
147:add_executable(ContentApp
192:if(NG_ENABLE_LEGACY_STANDALONE)
193:add_executable(GIRS
238:if(NG_ENABLE_LEGACY_STANDALONE)
239:add_executable(HTS
258:add_executable(IoTTestApp
301:add_executable(NBTestApp
330:add_executable(PGCS
403:if(NG_ENABLE_LEGACY_STANDALONE)
404:add_executable(PSS
441:add_executable(NRNCS
```

## Current contract and requested decision

The packet below embeds the complete v1.9 SPEC and the complete sanitized r06 plan manifest, including the proposed compile/link checker logic, target census, resource limits, run-record/IPC/observer schemas, Q0/Q1 oracle, and synthetic fixture matrix. The machine-readable plan is not an execution result. The exact manifest SHA-256 is `3bf3a4284c163b35c2e08ffcb196b9bd19adb5aed9dd9b5c0f0b501454f360b2.

A read-only preflight against the embedded plan produced: all 3 Draft 2020-12 schemas meta-valid; 4 valid and 14 invalid run-record fixtures (14/14 rejected); 2 synthetic compile profiles passed and 26/26 negative compiler-command cases rejected; the link checker passed 45 subprocess tests (3 positive + 12 negative under each of normal Python, `python3 -O`, and `PYTHONOPTIMIZE=1`). These are synthetic contract checks only. No real `compile_commands.json` or `link.txt` was checked; no configure/build, harness run, IPC/VM operation, D3 timing, or performance measurement occurred.

### Review questions

1. Does the same-SPEC v1.8/v1.9 documentary response close all three r05 finding-4a must-fixes? If not, give the exact field/clause and a concrete bypass.
2. Are actual-output target mapping, dynamic-link conflict rejection, and the v2 run-record `allOf` constraints sufficient and internally consistent? Identify any missing case that prevents a safe `PASS` or a bounded `FAIL`.
3. Confirm findings 3 and 4b–4d remain design-level only; do not upgrade them to implementation or runtime properties.
4. Return `APPROVE`, `AMEND`, or `NO-GO` for this exact packet and state the evidence boundary. No implementation, configure/build, execution, IPC/VM, D2 acceptance, D3, or performance authority is requested or granted.

Fresh owner consent bound to the final packet SHA is required before any Astra submission. This draft has NOT been submitted.

===== FILE: Specs/SPEC-039-common-performance-measurement.md | SHA-256 2477ac94b67ecf6e81665fe2c8899cbfdff31edec08830dc2b60ebbaa7b0d0d0 =====
# SPEC-039: Gateway Measurement Baseline on AIOPT5

**Version:** v1.9
**Date:** 2026-10-10
**Author:** Hermes; owner-requested gate amendment
**Status:** In Progress
**Gate:** v1.1 D0 approved for Q0/Q1; D1-H compile/link passed; v1.2 pre-code review returned Astra HOLD; v1.4 packet returned Astra AMEND; v1.5 r03 and v1.6 r04 returned Astra AMEND; v1.7 r05 returned Astra AMEND with three must-fixes in finding 4a; v1.8 same-SPEC documentary response is local and unreviewed; v1.9 is an unreviewed, privacy-only checkout-path redaction; D2 implementation/runtime remain blocked; D3 not authorised
**Branch:** AIOPT5
**Baseline commit:** `1af604dad66148618cc0e578c28adb4a9829e607`
**D0-approved v1.1 draft SHA-256:** `8026e1320149c318a5218eaa36c730f262081a5bbe0c2d582b23c307c7144e1c` (owner approval: 2026-10-04; documentary Q0/Q1 scope only; does not approve this v1.8 amendment)
**Owner task:** NG-040, under NG-039
**Related:** `SPEC-032-gw-input-queue-batching.md`, `SPEC-033-message-commandline-refactor.md`

> v1.8 retains the v1.1 Q0/Q1 conditions and the owner’s separate D2 scope (compile-time-gated test seam in Common plus one isolated local correctness run; no algorithm change). The v1.5 r03, v1.6 r04 and v1.7 r05 packets returned AMEND. The r04/r05 reviews closed findings 3 and 4b–4d at design level only; finding 4a remains must-fix. The r05 response identified preprocessor-forwarded/function-like macro gaps, `-ansi` as an alternate dialect selector, and Python `assert` checks disabled by `PYTHONOPTIMIZE=1`; it also recommended binding target identity to actual `-o`, rejecting conflicting dynamic-linker controls, and encoding more PASS/failure consistency in schema `allOf`. This same-SPEC amendment hardens those contracts while keeping the original scope and five-file implementation allowlist. No review approved implementation; D2 code/build/runtime remain blocked pending a fresh exact-SHA review and separate owner authorization. D0 approval alone grants none of these permissions; D3 remains unauthorised.

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
5. **D2b — Correctness test code/build/run:** only after the v1.8 closure amendment is reviewed under fresh owner consent and all blocking review conditions are resolved, implement only the frozen allowlist: root `CMakeLists.txt`; `Common/src/GW.h`; `Common/src/GW.cpp`; new `tests/spec039/gw_q0_q1_d2.cpp`; and new `tests/spec039/q0_q1_d2.manifest.json`. Add the separate option `NG_ENABLE_NG039_GW_Q0_Q1_D2` (default `OFF`) and target `ng039_gw_q0_q1_d2`; define `NG_GW_QUEUE_TEST_OBSERVATION` for `Common` and the harness only inside an isolated diagnostic target graph: the option-on root CMake path must declare only `Common` and this harness and return before production application targets, with a configure-time target-census guard rejecting any other target. Keep the existing D1-H harness/manifest as historical evidence and do not overwrite them. Gate observer declarations, storage, methods and production call sites together. Build the D2 target in a fresh isolated checkout at the pinned functional baseline. The manifest must freeze this bounded launch shape (actual paths recorded there): `timeout --signal=TERM --kill-after=5s 20s unshare --user --map-root-user --ipc --mount --pid --propagation private --fork --kill-child --mount-proc <harness>`. Run no shell/helper child in the namespace; the harness must create no child processes. If user/IPC/mount/PID namespace setup or the private mount fails, abort—no host-IPC fallback. The harness must make mount propagation private, mount a new tmpfs at `/dev/shm`, and capture pre-run `/proc/sysvipc/{shm,sem,msg}` plus `/dev/shm` inventory, requiring an empty starting inventory. Key `11` still reaches `ReturnIPCSHMID` and the Gateway SHM reader; never assume it eliminates IPC. After stopping/joining Gateway/output threads and destroying Process/GW, record the scoped post-run inventory; remove only SysV IDs and semaphore files absent from the empty pre-run inventory, then record a final empty inventory. Any failed attribution/removal or residual object is BLOCKED/INCONCLUSIVE; do not clean host/foreign IPC. `unshare --kill-child` plus outer `timeout --kill-after` bounds the one harness process; no test subprocesses are permitted. Preserve all outcomes and identities. Also compile the default (observer-disabled) Common target and verify observer symbols are absent. No timing claim is permitted at D2. Failed, incomplete, or unobservable assertions block D3; any design that exceeds the authorised scope returns to the owner.
6. **D3 — Characterisation:** Q0/Q1 timing is a separate gate, allowed only after D2 passes and the owner explicitly authorises the frozen measurement plan. Preserve all run outcomes and identities; classify the future-head hypothesis `MEASURED`, `INCONCLUSIVE`, or `BLOCKED`.
7. **D4 — Follow-up decision:** decide whether to stop, approve another measurement slice, or propose a performance implementation SPEC. Any future change requires a predeclared baseline/metric, actual-source review, correctness controls and its own owner-approved implementation allowlist. Astra or subagent opinions never grant implementation or runtime authority.

### 4.5 GPT-6 Astra consultation checkpoints (advisory; no execution authority)

| Checkpoint | Packet and question | Gate informed |
|---|---|---|
| Before D2 test-seam/code | Exact SPEC revision and SHA, functional baseline SHA, D1-H manifest, proposed observer/interface diff, isolated CMake target graph and complete proposed D2 manifest/resource limits, Q0/Q1 assertions, direct production `PushToInputQueue` stimulus, explicit non-coverage of NGAL/wire-time behavior, IPC/mount namespace and cleanup plan. Ask whether the isolated target graph truly prevents test macro propagation to production consumers, whether the sticky wait/lifetime/cleanup oracles close every claim without private-access hacks, whether the proposed manifest and resource limits are complete, and whether any claim exceeds the owner scope. | The original pre-code review was performed against v1.2 and returned HOLD. The v1.4 packet returned AMEND. The sanitized v1.5 r03 packet SHA-256 `7121fa9307beb790ff093319d6d9bc54fb9d296bf68d8c705647fbcebf740797`, tied to v1.5 SPEC SHA-256 `cf6efdf1dc1bcc7179106e109d3143a5cbe89e8c93cb7725aa34498b5407b6cc` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, returned AMEND on 2026-10-05 (response ID `resp_020e2d49d7eb0fb1016ac39985744887d28b258e11ecf96233`). That one-call consent is consumed. The r04 and r05 one-call exact-SHA consents are also consumed; r05 returned AMEND with three remaining 4a must-fixes. A v1.8/r06 follow-up requires fresh explicit owner consent bound to its exact final packet SHA. Review is advisory and cannot expand the owner-approved scope. |
| After D2 correctness result, before D3 | Frozen diff/build/test manifest, all raw outcomes including failures, identities and cleanup evidence. Ask whether the D2 assertions are supported and what remains inconclusive. | Planned checkpoint; a separate Astra submission requires owner consent. The owner decides whether to accept D2 and authorize D3. |
| After D3, before any implementation SPEC | Frozen raw data, analysis, source/build/workload identities and hypothesis classification. Ask whether the evidence supports a future implementation proposal and its acceptance metric. | Future checkpoint only; separate owner consent is required. Astra advice never authorises production changes. |

The owner authorised one GPT-6 Astra review via OpenAI Codex for the frozen v1.2 pre-code D2 packet. That call returned HOLD on the v1.2 SPEC SHA-256 `8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91` and packet SHA-256 `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`; its consent was consumed. On 2026-10-05 the owner separately authorised one review of the sanitized v1.4 packet SHA-256 `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948`, bound to SPEC v1.4 SHA-256 `f54901cad03209f94fe98cfad3bf62953895ed24fef3d771a89b89f5229217e1` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`. Astra returned AMEND, response ID `resp_0a12ead61fee5339016ac370498edc87d28ec6212dafa33cf9`. The archived raw SSE SHA-256 is `25b8c29265d327c34323e12f2c35177c2069c13b1d7e99f83a42ffea75240a58`; archive manifest SHA-256 is `46f41c9af22a6b1c9e86e2a66682dfd83f7323fb25d1c47aded2aa52c95e6782`. The r03 AMEND is advisory; its one-call consent is consumed and grants no implementation, build, runtime, IPC/VM, or D3 authority. Raw SSE SHA-256 `14f95d55a4a5611f518a0e236c98edd255e6809acd256b105177f975577bef09`; parsed-response SHA-256 `6fae28a46fd385e0d5aa87e8a1bc7149c832fcd8878170b470dab558941bc587`; archive manifest SHA-256 `b26c7767c9a74f2c9ee716a59257f79064c5480191addbc5f61da998989ca5fd`. The packet’s “NOT submitted” line is a time-frozen pre-review snapshot; this paragraph records the later submission/result. The v1.6 r04 packet SHA-256 `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829`, tied to the v1.6 SPEC SHA-256 `3296530c13801de945ed97b920532533c1a4a44d628075a9c5e61cdcc7ee01cb` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_0d91609fd866592e016ac3b3b903e487d2891f1ae2407fb075`). Raw SSE SHA-256 `a1c62af8a4e35a59aec3428fdf45818f2dd89b3f57dce9c3bf8be9a6279fe08d`; parsed-response SHA-256 `3655369b4c6c348e439ec87e48f6a8c1e789c46d5d4500890bee8577b8a969d3`; archive manifest SHA-256 `3f6dbf25e22ffd3ce5ce9290935fd41db162fd81aa67923c445b5ea250fbe2a8`. That exact-SHA consent is consumed. The review closed r03 findings 3 and 4b–4d at design level only; finding 4a remains must-fix. The v1.7 r05 packet SHA-256 `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a`, tied to SPEC v1.7 SHA-256 `b73bcc7361bb797b4ddd40952860f0092fb840bb11b31bf0a99659bef8475b70` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). Raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`; parsed-response SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`; archive manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a`. Usage: 22,569 input / 1,650 output / 24,219 total tokens; 8.11725 estimated credits (not USD); quota at return 5h 51%, weekly 86%. Three 4a must-fixes remain: preprocessor-forwarded and function-like observer macros; `-ansi` dialect override; Python assertions disabled by `PYTHONOPTIMIZE=1`. Optional hardening: actual `-o` target mapping, dynamic-linker conflict rejection, and stronger run-record schema `allOf`. Findings 3 and 4b–4d remain design-level closures only. The r05 consent is consumed. SPEC-039 v1.8 is local/unreviewed; any r06 review requires fresh exact-SHA consent. D2 remains blocked.

No VM/SSH/QGA operation, algorithmic production change, commit, push, or release is authorised by these gates. All Astra reviews to date were separately owner-authorised by exact packet SHA and their one-call consents are consumed; none expands the D2 seam/runtime scope. The old NG-062 trial locks and evidence remain untouched.

## 5. Files and evidence

| Path | State/intent |
|---|---|
| `Specs/SPEC-039-common-performance-measurement.md` | v1.9 same-SPEC working text; v1.1 D0 approval remains bound only to its recorded digest. v1.5 r03, v1.6 r04 and v1.7 r05 returned AMEND; v1.8 is local and unreviewed. v1.9 only replaces a workstation-specific checkout path and changes no design, scope, or gate; it is also unreviewed. |
| `Common/src/GW.cpp`, `Common/src/GW.h` | Read-only under D0; a D2 seam may touch only the explicitly frozen compile-time-gated scope in §4. |
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No NGAL/Process/PGCS/PG, algorithm, separate runner file, or D3 timing files are proposed. The same allowlisted harness binary’s `--finalize` mode runs only after the namespaced test exits, outside namespaces, and performs evidence finalization without constructing Process/GW, IPC, or child processes. |
| Isolated D1-H artifacts | Compile/link-only probe and manifest in the isolated D1-H checkout under `<NG039_D1H_CHECKOUT>/tests/spec039/`; manifest SHA-256 `9a7c4cda741e81192642053c4bdc60c3a5c0a46afcfa6c658ff2e2324f24225a`. Not runtime or correctness evidence. |
| `Specs/RESULTS-SPEC-039/` | D2/D3 results do not exist; create only after the applicable scope and evidence contract are approved. Historical AIOPT3 results remain in the NG-050 archive. |
| Obsidian NG-039 / NG-040 | Scope and blockers; neither an experimental oracle nor build evidence. |

## 6. Rollback and open questions

If the approved D0 design is later abandoned, mark this Proposal as abandoned/superseded instead of deleting it; leave the archive, current AIOPT5 diffs and historical trial evidence untouched. Later test-only changes must have an explicit allowlist and a reversible isolated checkout.

Before D2 code: freeze the compile-time-gated observer, exact five-file allowlist, in-memory future-message admission through `PushToInputQueue`, command, IPC/mount namespace and ownership cleanup oracle in the pre-code packet; obtain the authorised GPT-6 Astra review before applying the diff. Q0 distinguishes zero application offers from bootstrap queue state; Q1 uses a future `Message::Time` created locally because wire serialization does not carry that scheduling field. Cleanup must reconcile the Process-owned pending test message only after Gateway/output workers stop. The owner’s D2 scope is one correctness-only local run and does not include NGAL ingress coverage, D3 or algorithm changes. Q2–Q4 remain deferred; do not import approval or measurements from archived AIOPT3 SPECs.

## 7. D2a closure amendment — v1.8 (documentary; not implementation authorization)

This section records the response to the v1.7 r05 Astra AMEND within the same SPEC number and bounded D2 scope. The r04/r05 reviews closed findings 3 and 4b–4d at design level only; finding 4a remains must-fix in three areas: preprocessor-forwarded/function-like observer macros, `-ansi` dialect selection, and Python `assert` checks removable by `PYTHONOPTIMIZE=1`. This amendment adds a supported compile-argument grammar, derives target identity from actual `-o` output, removes assertion-dependent link checks, rejects dynamic-link controls, and encodes status/cleanup/inventory conditions in schema `allOf`. No D2 code, build, harness execution, namespace/IPC operation, or D3 timing is authorised. The r05 consent is consumed; D2 remains blocked pending a fresh review of a new exact-SHA packet and separate owner execution authorization.

1. Sandbox before construction: launch the harness in the frozen user/IPC/mount/PID namespaces; make mount propagation private; mount a new tmpfs at `/dev/shm`; and capture empty SysV IPC and semaphore/file inventories before constructing any `Process`, `HT`, `GW`, or test `Message`. If namespace, mount, or inventory setup fails, abort without fallback to host IPC. No constructor-time IPC may precede the empty pre-inventory.

2. HT construction, Message lifetime, and failure-safe ownership: after sandbox setup, construct the isolated non-`PGCS` `Process`. Its pinned constructor calls `NewBlock("HT", ...)` then `NewBlock("GW", ...)` (`Process.cpp:228–230`), and the `GW` constructor retrieves that HT (`GW.cpp:99–101`) before initialization dereferences it (`GWRunInitialization01.cpp:68–80`). Do not call `NewBlock("HT")` or `NewBlock("GW")` again; verify exactly two blocks exist and one lookup succeeds for each name. Before starting Gateway, create the Process-owned target, capture `GetDeleteFlag()==false` while single-threaded, and bind its ID with the observer. Capture the immutable scalar `Message::InstantiationNumber` (`Message.h:158`) before each production `Run`; use only that copied ID for Run-entry and Run-return events, publishing the return event before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after a possible reclamation; duplicate, zero, stale or wrapped identity is INCONCLUSIVE. The three-argument constructor used by `Process::NewMessage` initializes `Delete=false` and `ApplicationDeleted=DELETED_BY_CORE` (`Message.cpp:37–70`, especially 63–64); `DeleteMarkedMessages` deletes only true-flagged messages (`Process.cpp:885–917`). Keep the target out of the queue and do not mark it through Q0; use exactly three inert command lines `-ng039 --probe 0.2`, which match neither the registered action nor the `0.1` fallback in `Block::Run` (`Block.cpp:244–313`). For the expected Q0/Q1 pending path, after Gateway and its output worker stop/join, verify no target pop/Run and `Delete=false`; call `EraseMessage` only while Process is quiescent and require `GetNumberOfMessages()` to decrease by exactly one. Keep the detached object alive through `Process` destruction: its destructor calls `DeleteMessages()` before deleting `Blocks`, so after EraseMessage it will not own/delete this target; `GW::~GW` destroys actions, while the queue container drops raw pointer values. Delete the detached target exactly once after Process/GW destruction. If any target pop/Run, delete flag, count, or identity makes ownership uncertain, never dereference, erase, delete, or invoke the C++ Process/GW destructors on a possibly stale pointer: write a bounded/fsynced `run-record.pending.json` with `record_finalized=false` and no fabricated measurements, exit nonzero via `_Exit`, and let the isolated process/namespace terminate; after it exits, only the parent-side finalizer in item 5 may produce the final INCONCLUSIVE record, with unavailable post/final inventories explicitly not empty—not PASS. A partial construction or blocked join follows the same fail-closed classification; the outer timeout is the bound if join cannot return.

3. Fresh, correlated observation contract: keep the 64-bit event sequence and add a saturating `gateway_iteration_id`, assigned once at the start of each Gateway loop iteration and carried across its wait and due/pop decision. Preserve two distinct sticky scalar records: (a) while the existing `InputQueueMutex` lock is held when the real `wait_for` returns, if the registered target is the head, capture `WaitCapture{sequence, iteration_id, target_id, head_id, head_time, depth, timeout_ms, predicate_return}` once and never overwrite it; (b) while the production due/pop critical-section lock is held, capture the corresponding `DecisionCapture{sequence, iteration_id, candidate_id, candidate_time, actual_due_branch, actual_popped}` from the existing strict comparison/pop branch, without performing a second comparison, and never overwrite that capture. The records correlate only if the iteration IDs match, `decision.sequence > wait.sequence`, and the wait target/head ID matches the actual decision candidate. Set `correlation_status=MATCHED` only then. Otherwise use `PENDING`, `DECISION_NOT_REACHED`, `HEAD_MISMATCH`, `INVALID`, or `OVERFLOW`; no incomplete pair is exposed as a valid due result. Stop before the decision, a changed head, invalid/duplicate/wrapped ID, missed event, stale sequence, or counter overflow makes Q1 BLOCKED/INCONCLUSIVE, never zero/PASS. Under Q0/Q1, capture sequence/counter baselines at each window start: Q0 requires a fresh empty-observation delta inside its 100 ms no-controlled-offer window after bootstrap drain; Q1 requires target admission/head identity, a matched wait/decision pair newer than the Q1 baseline, zero target pops/Run entries/returns throughout 100 ms, and a future target time at end. Use `std::chrono::steady_clock` for correctness windows; `Message::Time` remains the scheduler clock. Set `gateway_started=true` under `InputQueueMutex` before the loop and never reset it; later registration fails. Before production `Run`, copy `Message::InstantiationNumber` to a scalar; emit Run-entry/return with that copy and publish the return before `DeleteMarkedMessages()` can reclaim the object. Never dereference a Message pointer after possible reclamation. These are correctness-window rules, not timing/performance evidence.

4. Frozen interface/build/manifest contract: the five-path D2 changed-file allowlist in §5 is a file-edit boundary, not a CMake target census; record and validate those as separate manifest fields. Retain the default-OFF `NG_ENABLE_NG039_GW_Q0_Q1_D2` option. In the opt-in diagnostic CMake configuration, define only `Common` and `ng039_gw_q0_q1_d2`; create those targets in the option-on branch and execute a root-listfile `return()` before any production executable declarations. The option-ON target census must equal `{Common, ng039_gw_q0_q1_d2}`; its forbidden application-target set is `{ContentApp, GIRS, HTS, IoTTestApp, NBTestApp, PGCS, PSS, NRNCS}` (including the three targets behind `NG_ENABLE_LEGACY_STANDALONE`); fail closed if any appears. `PUBLIC` macro propagation is permitted only in this isolated two-target graph, so all compiled consumers of macro-dependent `GW` layout are `Common` and the harness. In a separate fresh option-OFF build directory, explicitly set `NG_ENABLE_NG039_GW_Q0_Q1_D2=OFF` and `NG_ENABLE_LEGACY_STANDALONE=OFF`; retain the baseline target graph `{Common, ContentApp, IoTTestApp, NBTestApp, PGCS, NRNCS}`. `GIRS`, `HTS`, and `PSS` are the three additional Common consumers guarded by `NG_ENABLE_LEGACY_STANDALONE` and must be absent in this default-profile census. Prove no observer macro, state, method, call, or symbol appears. Record `compile_commands.json` evidence for each configured target and macro. The frozen D2 manifest must specify schema/version, exact SPEC/baseline/tree/document HEAD identities, dirty/untracked census, the five-file allowlist, symbolic path roles and their local resolution; the sanitized review packet may redact host roots with stable aliases, but the private execution manifest must resolve each alias to an absolute path before any command; diagnostic/default target census, CMake/generator/compiler/effective flags, exact configure/build/launch commands and cwd, Q0/Q1 tuples and observer fields/update sites, namespace/mount and IPC inventory procedure, all time/resource ceilings, bounded output/evidence filenames and schemas, normal cleanup and abnormal failure matrix, and which result/hash fields are `null` until a run occurs. Planned ceilings: private `/dev/shm` tmpfs `size=16m,nr_inodes=128,noexec,nosuid,nodev`; before constructing Process set and record `RLIMIT_AS=512 MiB`, `RLIMIT_FSIZE=4 MiB`, `RLIMIT_NOFILE=64`, and `RLIMIT_CORE=0`; cap the runtime bundle at eight files total: `run-record.json` plus seven artifacts maximum. The six required artifacts are `stdout.txt`, `stderr.txt`, `ipc-pre.json`, `ipc-post.json`, `ipc-final.json`, and `observer-events.json`; only `harness-diagnostics.txt` may be the optional eighth file. The tracked `q0_q1_d2.manifest.json` is the plan/schema input, not a runtime-bundle file. Cap each artifact at 4 MiB, the run record at 256 KiB, and the complete bundle at 32 MiB. Hash/size entries in `run-record.json` cover the artifacts, not the record itself; archive its own hash out of band. The one harness process has a 20 s outer timeout and 5 s kill grace; it creates no child process. If a limit or namespace setup fails, abort before constructors. Output/inventory limits exceeded, forced termination, blocked join, or missing final inventory are BLOCKED/INCONCLUSIVE, not cleanup PASS. Keep planned inputs distinct from post-run evidence; never use unresolved tokens such as `BASELINE_SHA` where an exact commit can be frozen. Any unbounded field, unexpected target, or unexplained result omission blocks the review gate. v1.6 freezes both fresh configure profiles with `-G "Unix Makefiles"`, `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`, `-DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13`, `-DCMAKE_BUILD_TYPE=Debug`, `-DCMAKE_CXX_STANDARD=20`, `-DCMAKE_CXX_EXTENSIONS=ON`, `-DCMAKE_CXX_FLAGS=-O0`, and `-DCMAKE_CXX_FLAGS_DEBUG="-O0 -g"`; the verified local toolchain is CMake 3.28.3 and GNU C++ 13.3.0. The pinned root CMake also appends compile flags `-O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer` and executable link flags `-static -static-libgcc -static-libstdc++`; the manifest/checker must retain these effective source-owned flags rather than mistaking configure values for the complete command. The v1.8 compile-database verifier treats the database as untrusted input. Require a JSON array of entry objects with nonempty `directory` and `file`; accept a string-array `arguments` field or a shell `command` string parsed exactly once with `shlex.split(posix=True)`. If both encodings exist, their token arrays must be identical; malformed, missing or conflicting encodings fail. Reject command-string shell expansion, substitution, control and glob metacharacters rather than modeling shell behavior; never execute or expand the command. Resolve each `directory` under the selected fresh build root, each source `file` under the pinned worktree, and each output under the build root. Parse exactly one `-c` source and one `-o` output (joined or split spelling); derive the target only from that actual output path’s unique `CMakeFiles/<target>.dir/` segment. If compilation-database `output` metadata exists, normalize it relative to `directory` and require exact agreement with `-o`; reject conflicting target mappings or paths outside their permitted roots. Do not infer target identity from a regex match in arbitrary command text. Before configure/build/checker invocation, clear every environment variable listed in `environment_contract.unset`; record the retained `PATH`, `HOME`, `TMPDIR` and `LC_ALL` values so inherited compiler/include/toolchain injection cannot silently alter the frozen profile.

Use an explicit supported-argument grammar: allow only the pinned project/debug flags (`-O0`, `-g`, `-g3`, `-Wall`, `-fmessage-length=0`, `-pthread`, `-Wno-deprecated`, `-lpthread`, `-lrt`, `-fno-omit-frame-pointer`), exactly one `-std=gnu++20`, compile/dependency switches `-c`, `-MD`, `-MMD`, `-MP`, paired or GCC-supported joined forms of `-o`, `-MF`, `-MT`, `-I`, `-isystem`, and `-iquote`, and parsed joined/split `-D`/`-U` forms. Include/path/dependency operands must be well-formed and consumed by their switch; source and output operands must match the entry metadata. Every other driver option or opaque argument fails closed. In particular reject `@response` files, `-Wp`/`-Wp,*`, `-Xpreprocessor`, forced-include forms (`-include`, `-imacros`), `-ansi`, `-traditional`, `-x`, `-specs`, `-B`, `-wrapper`, `-fplugin` and any unknown `-f`/`-m`/driver option. This prevents an unparsed option from changing language, macro, optimization or compiler behavior.

Parse all joined and split `-D`/`-U` operands. Parse the identifier before `=` and recognize function-like spelling `NAME(...)`; reject malformed spellings and reject any object- or function-like define/undef whose base name is `NG_GW_QUEUE_TEST_OBSERVATION` except the one permitted diagnostic object-like `NG_GW_QUEUE_TEST_OBSERVATION=1`. The default graph must have no such definition or undefinition; the diagnostic graph must have exactly one such define on `Common` and the harness, none elsewhere, and no duplicate, conflicting, forwarded or function-like form. Require exactly one `-std=gnu++20`; explicitly reject `-ansi`, `-traditional`, `-x` and any other dialect selector. Every optimization token must be `-O0` (duplicates may arise from the pinned CMake flag layers); unknown optimization/debug overrides fail under the grammar. Require the pinned Debug/project-source flags. Record the compile-database hash, per-target unit count and parsed macro set, and verify exact ON/OFF target census.

For the diagnostic executable, parse `link.txt` with `shlex.split(posix=True)` and use explicit `if` checks that raise a nonzero error; never use Python `assert` for validation. Require the pinned compiler realpath, no response files, and exactly one standalone token each for `-static`, `-static-libgcc`, and `-static-libstdc++`. Reject conflicting dynamic-link controls including `-Bdynamic`, `-dynamic`, `-shared`, `-Wl,-Bdynamic`, and any `-Wl,` payload containing `-Bdynamic` or `--dynamic-linker`. The result must remain fail-closed when run with `PYTHONOPTIMIZE=1` or `python3 -O`.

Freeze synthetic fixtures for: wrong compiler; `command`/`arguments` disagreement; false or conflicting `output` target metadata; unexpected target; direct and split bare/zero-valued/duplicate/undefined observer macros; `-Wp,-DNG_GW_QUEUE_TEST_OBSERVATION=1`; `-Xpreprocessor -D...`; joined and split function-like observer defines; `-ansi`; later non-C++20 `-std`; later non-`-O0`; `-include`/`-imacros`; unknown `-specs`/driver switches; response files; missing standalone `-static`; wrong linker compiler; and dynamic-link overrides. Run link negatives in both normal and `PYTHONOPTIMIZE=1` modes. These are planned synthetic checker tests only; no real compile database, CMake configure/build or harness run exists for review preparation. Resource ceilings additionally include `RLIMIT_CPU=30s`. No `RLIMIT_NPROC` is set: under the user namespace it is tied to the mapped host UID and could make thread creation depend on unrelated UID-wide activity; the harness creates only its fixed Gateway/output threads and no helper child, while the outer process-tree timeout kills the isolated tree. No explicit stack or memlock ceiling is added because the test uses no `mlock` and has no unbounded recursive work; `RLIMIT_AS` bounds process address space. The pinned baseline sets `MaxSegmentSize=MAX_MESSAGE_SIZE=33,554,532` bytes (`Process.h:115`, `GW.cpp:82`); the initial non-PGCS setup calls `ReturnIPCSHMID(11, ...)`, and the only other creation site uses four `NUMBER_OF_PARALLEL_SHARED_MEMORIES` slots (`Process.h:116`, `GW.cpp:713`). Bound the private IPC namespace to at most five SysV SHM segments of 33,554,532 bytes each, 167,772,660 bytes total; any larger segment or total is INCONCLUSIVE, not cleanup PASS. SysV inventory arrays separately cap at 4096 IDs; exceeding any inventory/list/output cap is incomplete evidence and INCONCLUSIVE. No external network workload is offered in D2. The frozen launch uses the allowlisted binary as `--run <run_id>` only inside `unshare`; after `timeout`/`unshare` returns and redirected logs close, the parent invokes that same binary as `--finalize <D2_RESULTS> <run_id> <observed-status>` outside namespaces. Finalization is a bounded post-processing mode of the same five-file harness, not a new runner file or a second correctness trial; it starts no Process/GW, touches no IPC and spawns no child. Bound the parent invocation to a 10 s outer timeout plus 5 s kill grace, `RLIMIT_AS=128 MiB`, `RLIMIT_CPU=10s`, `RLIMIT_FSIZE=4 MiB`, and `RLIMIT_NOFILE=32`; any timeout, limit, write, or validation failure yields no PASS and must leave the result INCONCLUSIVE.

5. Machine-readable run-result and finalization contract: the tracked `tests/spec039/q0_q1_d2.manifest.json` must include strict Draft 2020-12 schemas for the transient `run-record.pending.json` and final `run-record.json` (`ng039-d2-run-record/v2`), plus the IPC-inventory and observer-event schemas (`ng039-ipc-inventory/v2`, `ng039-observer-events/v2`) and cross-field rules. For v1.8, encode cross-field classification in the run-record schema `allOf`, not only prose: a non-finalized record is `INCONCLUSIVE` with no failure claim; `PASS` requires `record_finalized=true`, `failure=null`, outer and harness codes zero, no signal/timeout, every required assertion `PASS`, all six required artifact names present at least once, complete available empty pre/final inventories, a complete attributable post-inventory (which may be non-empty before cleanup), every cleanup flag true, no counter overflow, matched Q1 correlation; `FAIL` requires a finalized `ASSERTION_FAILED` record, at least one assertion `FAIL`, no `INCONCLUSIVE`/`NOT_RUN` assertion, a known harness code, a normal known outer code 0–123 (thereby excluding timeout 124, wrapper errors 125–127, and ambiguous 128–255 exits), no signal/timeout, all six required artifact names, complete attributable inventories and safe cleanup; finalized `INCONCLUSIVE` requires a bounded failure object. Add valid and invalid synthetic fixtures for each conditional; keep other semantic rules explicit in the manifest.

The harness `--run` mode atomically writes a bounded staging record with `record_finalized=false` and provisional `INCONCLUSIVE` status; it does not claim final stdout/stderr hashes or a parent-observed exit. After the namespaced command and redirection descriptors close, the outer parent invokes the same allowlisted binary as `--finalize`, outside namespaces, with the run ID and observed command status. The parent preserves the raw `outer_code`: code 0 is normal success, codes 1–123 are normal non-zero exits, code 124 is conservatively recorded as `timeout_fired=true`, codes 125–127 are wrapper/invocation failures, and codes 128–255 do not by themselves identify a signal. Set `signal` only when an independent wait result identifies it; otherwise store null and classify an ambiguous non-zero outcome as INCONCLUSIVE. Never infer a harness signal from a shell/timeout status alone. This mode creates no Process/GW, uses no IPC, spawns no child, and is post-processing—not a second correctness run or exit-code remap. It binds the actual outer status, final artifact sizes/hashes and classifications, writes final `run-record.json` using an fsynced temporary file plus atomic rename, then removes the staging record. If the staging record is absent/invalid, finalization writes a metadata-only `INCONCLUSIVE` record from pinned plan identities and observed outer status: all assertions `NOT_RUN`, measurements null, inventories unavailable, cleanup unknown, missing-artifact list populated, and no fabricated observations. If the finalizer fails or no valid final record exists, the result is INCONCLUSIVE and cannot PASS.

Overall status is exactly `PASS`, `FAIL`, or `INCONCLUSIVE`; each assertion is `PASS`, `FAIL`, `INCONCLUSIVE`, or `NOT_RUN`. `PASS` requires `record_finalized=true`, every required assertion PASS, a complete empty pre-inventory, a complete attributable post-inventory, successful removal of all post objects absent from pre, a complete empty final inventory, complete stop/join/target reconciliation and all required artifacts finalized within caps. The post-inventory may legitimately be non-empty before cleanup; `post_inventory_empty` is not an acceptance requirement. `FAIL` is permitted only for a demonstrably false assertion when evidence, inventories and safe cleanup are complete and no uncertainty trigger applies; its `failure.code` is `ASSERTION_FAILED`. `INCONCLUSIVE` takes precedence for timeout/signal, missing/invalid staging/final record or artifacts, finalizer failure, counter overflow, identity/correlation mismatch, partial construction, blocked join, ambiguous ownership, incomplete/unattributable post-inventory, failed removal, incomplete final inventory, or any required assertion `INCONCLUSIVE`/`NOT_RUN`.

For non-PASS records, bounded `failure` requires code, phase, message ≤1024 characters, last event sequence or null, inventory availability, cleanup outcome, and missing artifact names. Pre must be complete and empty; post must be complete and attributable to the private run; final must be complete and empty. `/dev/shm` entry size is bounded by the 16 MiB private tmpfs, distinct from the 4 MiB JSON-artifact limit; each SysV ID array caps at 4096 entries, and overflow is incomplete evidence. The final bundle remains at most eight files: `run-record.json` plus seven artifacts, six required (`stdout.txt`, `stderr.txt`, `ipc-pre.json`, `ipc-post.json`, `ipc-final.json`, `observer-events.json`) and optional `harness-diagnostics.txt`; each artifact ≤4 MiB, final record ≤256 KiB, total bundle ≤32 MiB. `run-record.pending.json` is a ≤256 KiB transient, excluded from the final bundle and deleted after successful finalization. Hash artifacts, not the final record itself; archive the final record’s SHA-256 out of band. Missing/incomplete evidence is INCONCLUSIVE, never PASS.

Next gate: local v1.8 fixtures passed: three Draft 2020-12 schemas meta-valid; four valid and fourteen invalid run-record status fixtures behave as specified; compile-command checker accepted both profiles and rejected 26 negative cases; the link checker passed 15 normal/`python3 -O`/`PYTHONOPTIMIZE=1` cases (three positive, twelve negative). These are synthetic checker tests only—no real `compile_commands.json`, CMake configure/build, harness run, IPC/VM or timing result. Freeze sanitized r06 after final SPEC/manifest/task hashes, then present its exact SHA for fresh owner consent. Until a positive review and separate owner execution authorization, do not implement D2, configure/build/run the harness, touch IPC/VMs, or begin D3; Q2–Q4 remain deferred. This documentary work is not correctness or performance evidence.

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
| v1.9 | 2026-10-10 | Editorial privacy-only redaction of the workstation-specific D1-H checkout path to a stable role alias; no design, scope, or gate change. Unreviewed; refresh exact hashes before preparing r06. |

===== END FILE: Specs/SPEC-039-common-performance-measurement.md =====

===== FILE: Specs/RESULTS-SPEC-039/precode-manifest-v1.9-r06.json | SHA-256 3bf3a4284c163b35c2e08ffcb196b9bd19adb5aed9dd9b5c0f0b501454f360b2 =====
{
  "amendment_response": {
    "optional_r05_hardening": {
      "actual_output_based_target_mapping": "incorporated",
      "conflicting_dynamic_link_controls": "incorporated",
      "schema_level_status_consistency": "incorporated"
    },
    "r05_must_fix_4a": [
      "Reject compiler-option encodings that bypass macro inspection, including -Wp, and -Xpreprocessor; recognize function-like macro names by base identifier.",
      "Reject -ansi and all unsupported/alternate dialect, optimization, response-file, forced-include, toolchain, and driver-option forms; use an explicit supported-argument grammar.",
      "Replace Python assert checks in link validation with explicit nonzero failures that remain active under python3 -O and PYTHONOPTIMIZE=1."
    ],
    "r05_verdict": "AMEND",
    "review_status": "unreviewed; r06 requires new explicit owner consent bound to final packet SHA",
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
      "at least one assertion FAIL and no assertion INCONCLUSIVE or NOT_RUN; safety/inventory/cleanup assertions PASS",
      "known harness code 0..255; normal outer code 0..123; signal=null; timeout_fired=false",
      "all six required artifacts; available/complete empty pre and final inventories; complete attributable post inventory; every cleanup flag true; no uncertainty trigger"
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
    "inconclusive_precedence": [
      "timeout/signal",
      "missing/invalid stage/final/artifact",
      "finalizer failure",
      "overflow or identity/correlation ambiguity",
      "partial construction or blocked join",
      "incomplete/unattributable post or failed removal",
      "incomplete final inventory",
      "NOT_RUN/INCONCLUSIVE assertion",
      "schema/resource/bundle cap violation",
      "wrapper/invocation error or ambiguous signal-style outer code"
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
      "every required assertion is PASS; all six required artifact names are present at least once",
      "pre and final inventories are available, complete, and empty; post inventory is available, complete, and attributable but may be non-empty before cleanup",
      "every cleanup flag is true; counter_overflow=false; Q1 correlation_status=MATCHED"
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
      "checker_logic": "import json, os, re, shlex, sys\nfrom pathlib import Path\n\nEXPECTED = {\n    \"diagnostic\": {\"Common\", \"ng039_gw_q0_q1_d2\"},\n    \"default\": {\"Common\", \"ContentApp\", \"IoTTestApp\", \"NBTestApp\", \"PGCS\", \"NRNCS\"},\n}\nCOMPILER = os.path.realpath(\"/usr/bin/x86_64-linux-gnu-g++-13\")\nOBSERVER = \"NG_GW_QUEUE_TEST_OBSERVATION\"\nREQUIRED = {\"-O0\", \"-g\", \"-g3\", \"-Wall\", \"-fmessage-length=0\", \"-pthread\", \"-Wno-deprecated\", \"-lpthread\", \"-lrt\", \"-fno-omit-frame-pointer\", \"-c\"}\nPAIRED = {\"-o\", \"-MF\", \"-MT\", \"-I\", \"-isystem\", \"-iquote\"}\nJOINED = (\"-isystem\", \"-iquote\", \"-MF\", \"-MT\", \"-I\", \"-o\")\nDEP = {\"-MD\", \"-MMD\", \"-MP\"}\nSHELL_CONTROL = set(\";|&<>\")\n\ndef reject(message):\n    raise ValueError(message)\n\ndef norm_path(value, directory):\n    p = Path(value)\n    return (p if p.is_absolute() else directory / p).resolve(strict=False)\n\ndef macro_parts(value):\n    m = re.fullmatch(r\"([A-Za-z_][A-Za-z0-9_]*)(\\([^)]*\\))?(?:=(.*))?\", value)\n    if not m:\n        reject(\"malformed -D/-U operand\")\n    return m.group(1), m.group(2), m.group(3)\n\ndef target_from_output(output_path):\n    parts = output_path.parts\n    found = []\n    for i, part in enumerate(parts[:-1]):\n        if part == \"CMakeFiles\" and parts[i + 1].endswith(\".dir\"):\n            found.append(parts[i + 1][:-4])\n    if len(found) != 1:\n        reject(\"target cannot be uniquely derived from actual -o output\")\n    return found[0]\n\ndef entry_tokens(entry):\n    directory = Path(entry[\"directory\"]).resolve(strict=False)\n    command = entry.get(\"command\")\n    arguments = entry.get(\"arguments\")\n    if command is not None:\n        if not isinstance(command, str):\n            reject(\"command must be a string\")\n        words = shlex.split(command, posix=True)\n        if any(c in command for c in \"$`;&|<>\"):\n            reject(\"shell expansion/control syntax is unsupported\")\n        if arguments is not None and (not isinstance(arguments, list) or words != arguments):\n            reject(\"command/arguments disagreement\")\n    elif isinstance(arguments, list):\n        words = list(arguments)\n    else:\n        reject(\"compile entry has neither command nor arguments\")\n    if not words or not all(isinstance(x, str) for x in words):\n        reject(\"empty or malformed argv\")\n    if os.path.realpath(words[0]) != COMPILER:\n        reject(\"compiler identity mismatch\")\n    if any(x.startswith(\"@\") for x in words):\n        reject(\"response files are unsupported\")\n    if any(x.startswith(\"-Wp\") or x == \"-Xpreprocessor\" for x in words):\n        reject(\"forwarded preprocessor arguments are unsupported\")\n    return words, directory\n\ndef validate_compile_database(entries, profile):\n    if profile not in EXPECTED or not isinstance(entries, list) or not entries:\n        reject(\"unknown profile or empty compile database\")\n    seen = set()\n    for entry in entries:\n        if not isinstance(entry, dict) or not {\"directory\", \"file\"}.issubset(entry):\n            reject(\"malformed compile entry\")\n        argv, directory = entry_tokens(entry)\n        target_outputs = []\n        sources = []\n        standards = []\n        optimizations = []\n        macros = []\n        observed_defines = []\n        i = 1\n        while i < len(argv):\n            token = argv[i]\n            if token in PAIRED:\n                i += 1\n                if i >= len(argv) or not argv[i] or argv[i].startswith(\"-\"):\n                    reject(\"missing/malformed option operand\")\n                value = argv[i]\n                if token == \"-o\":\n                    target_outputs.append(value)\n                i += 1\n                continue\n            if token in (\"-D\", \"-U\"):\n                op = token\n                i += 1\n                if i >= len(argv):\n                    reject(\"missing macro operand\")\n                raw = argv[i]\n                base, function_like, value = macro_parts(raw)\n                macros.append((op, base, function_like, value, raw))\n                if base == OBSERVER:\n                    observed_defines.append((op, function_like, value, raw))\n                i += 1\n                continue\n            if token.startswith(\"-D\") or token.startswith(\"-U\"):\n                op, raw = token[:2], token[2:]\n                base, function_like, value = macro_parts(raw)\n                macros.append((op, base, function_like, value, raw))\n                if base == OBSERVER:\n                    observed_defines.append((op, function_like, value, raw))\n                i += 1\n                continue\n            if token.startswith(\"-std=\"):\n                standards.append(token)\n                i += 1\n                continue\n            if token.startswith(\"-O\"):\n                optimizations.append(token)\n                if token != \"-O0\":\n                    reject(\"optimization override\")\n                i += 1\n                continue\n            if token in REQUIRED or token in DEP:\n                i += 1\n                continue\n            matched = False\n            for prefix in JOINED:\n                if token.startswith(prefix) and len(token) > len(prefix):\n                    value = token[len(prefix):]\n                    if not value or any(c in value for c in SHELL_CONTROL):\n                        reject(\"malformed joined operand\")\n                    if prefix == \"-o\":\n                        target_outputs.append(value)\n                    matched = True\n                    break\n            if matched:\n                i += 1\n                continue\n            if token.startswith(\"-\"):\n                reject(\"unsupported compiler/driver option: \" + token)\n            if any(c in token for c in SHELL_CONTROL):\n                reject(\"shell control token\")\n            sources.append(token)\n            i += 1\n        if len(target_outputs) != 1:\n            reject(\"exactly one actual -o output is required\")\n        output_path = norm_path(target_outputs[0], directory)\n        target = target_from_output(output_path)\n        if target not in EXPECTED[profile]:\n            reject(\"unexpected target: \" + target)\n        seen.add(target)\n        if \"output\" in entry and norm_path(entry[\"output\"], directory) != output_path:\n            reject(\"output metadata disagrees with actual -o path\")\n        if len(sources) != 1 or norm_path(sources[0], directory) != norm_path(entry[\"file\"], directory):\n            reject(\"source operand disagrees with entry file\")\n        if standards != [\"-std=gnu++20\"]:\n            reject(\"effective language mode must be exactly gnu++20\")\n        if not optimizations or any(x != \"-O0\" for x in optimizations):\n            reject(\"effective optimization must be -O0 only\")\n        if not REQUIRED.issubset(set(argv)):\n            reject(\"required project/debug flags missing\")\n        if profile == \"default\" and observed_defines:\n            reject(\"observer macro defined or undefined in default profile\")\n        if profile == \"diagnostic\" and target in {\"Common\", \"ng039_gw_q0_q1_d2\"}:\n            if observed_defines != [(\"-D\", None, \"1\", OBSERVER + \"=1\")]:\n                reject(\"diagnostic observer macro must be exactly one object-like =1 define\")\n    if seen != EXPECTED[profile]:\n        reject(\"target census mismatch\")\n    return {\"profile\": profile, \"targets\": sorted(seen), \"entries\": len(entries)}\n\nif __name__ == \"__main__\":\n    with open(sys.argv[1], encoding=\"utf-8\") as f:\n        data = json.load(f)\n    print(json.dumps(validate_compile_database(data, sys.argv[2]), sort_keys=True))\n",
      "command_arguments_binding": "If both command and arguments are present, POSIX-shlex command tokens must equal arguments exactly. Never execute compile-database text.",
      "compiler_realpath_required": "/usr/bin/x86_64-linux-gnu-g++-13",
      "database_sha256_and_target_counts_recorded": true,
      "implementation_boundary": "Inline preflight checker in the authorized D2 manifest/harness package; no extra production or changed-file allowlist path.",
      "metadata_binding": "If compile_commands.json has output metadata, normalize it relative to directory and require equality with the actual -o path. Require the source operand to normalize exactly to the entry file.",
      "negative_fixtures": [
        "wrong compiler",
        "command/arguments disagreement",
        "false output metadata",
        "conflicting output metadata",
        "unexpected target",
        "direct bare observer define",
        "split bare observer define",
        "direct zero-valued define",
        "split zero-valued define",
        "duplicate observer define",
        "observer undefine",
        "-Wp,-DNG_GW_QUEUE_TEST_OBSERVATION=1",
        "-Xpreprocessor -DNG_GW_QUEUE_TEST_OBSERVATION=1",
        "joined function-like define",
        "split function-like define",
        "-ansi",
        "later -std=gnu++17",
        "later -std=gnu++23",
        "later -O1",
        "-Ofast",
        "-include",
        "-imacros",
        "-specs",
        "response file",
        "-fplugin",
        "-B toolchain override"
      ],
      "not_executed_in_r06": true,
      "observer_macro": {
        "default_profile": "no define or undefine in any target",
        "diagnostic_profile": "exactly one object-like -DNG_GW_QUEUE_TEST_OBSERVATION=1 for every Common/harness compile entry; no undefine, duplicate, conflicting value, or function-like form",
        "name": "NG_GW_QUEUE_TEST_OBSERVATION"
      },
      "planned_command": "python3 <inline compile-database checker> \"$DB\" \"$PROFILE\"",
      "reject_explicitly": [
        "@response files",
        "-Wp and -Wp,*",
        "-Xpreprocessor",
        "-include",
        "-imacros",
        "-ansi",
        "-traditional",
        "-x",
        "-specs",
        "-B",
        "-wrapper",
        "-fplugin",
        "unknown -f/-m/driver flags",
        "shell expansion/control tokens",
        "missing/extra source or -o operands"
      ],
      "required": true,
      "required_optimization": "one or more -O0 tokens only; every other -O* form rejected",
      "required_standard": "exactly one -std=gnu++20",
      "supported_exact_flags": [
        "-O0",
        "-g",
        "-g3",
        "-Wall",
        "-fmessage-length=0",
        "-pthread",
        "-Wno-deprecated",
        "-lpthread",
        "-lrt",
        "-fno-omit-frame-pointer",
        "-c",
        "-MD",
        "-MMD",
        "-MP"
      ],
      "supported_macro_switches": [
        "-DNAME[=VALUE]",
        "-UNAME",
        "joined and split spellings, parsed by base identifier; function-like spelling recognized"
      ],
      "supported_operand_switches": [
        "-o",
        "-MF",
        "-MT",
        "-I",
        "-isystem",
        "-iquote"
      ],
      "target_census": {
        "exact": true,
        "legacy_absent_off": [
          "GIRS",
          "HTS",
          "PSS"
        ],
        "off": [
          "Common",
          "ContentApp",
          "IoTTestApp",
          "NBTestApp",
          "PGCS",
          "NRNCS"
        ],
        "on": [
          "Common",
          "ng039_gw_q0_q1_d2"
        ],
        "on_forbidden": [
          "ContentApp",
          "GIRS",
          "HTS",
          "IoTTestApp",
          "NBTestApp",
          "PGCS",
          "PSS",
          "NRNCS"
        ]
      },
      "target_identity_source": "Parse the actual -o operand, resolve it relative to the compile entry directory, and derive CMakeFiles/<target>.dir from that normalized output path; do not select the first regex hit in arbitrary command text or trust output metadata alone."
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
    "default_build_command": "cmake --build \"$D2_SCRATCH/build-default\" --target Common --parallel 2",
    "default_configure_command": "cmake -S \"$D2_WORKTREE\" -B \"$D2_SCRATCH/build-default\" -G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g' -DNG_ENABLE_NG039_GW_Q0_Q1_D2=OFF -DNG_ENABLE_LEGACY_STANDALONE=OFF",
    "default_value": "OFF",
    "diagnostic_build_command": "cmake --build \"$D2_SCRATCH/build-diagnostic\" --target ng039_gw_q0_q1_d2 --parallel 2",
    "diagnostic_configure_command": "cmake -S \"$D2_WORKTREE\" -B \"$D2_SCRATCH/build-diagnostic\" -G \"Unix Makefiles\" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=/usr/bin/x86_64-linux-gnu-g++-13 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_EXTENSIONS=ON -DCMAKE_CXX_FLAGS=-O0 '-DCMAKE_CXX_FLAGS_DEBUG=-O0 -g' -DNG_ENABLE_NG039_GW_Q0_Q1_D2=ON -DNG_ENABLE_LEGACY_STANDALONE=OFF",
    "diagnostic_option": "NG_ENABLE_NG039_GW_Q0_Q1_D2",
    "diagnostic_value": "ON",
    "export_compile_commands": true,
    "legacy_option": "NG_ENABLE_LEGACY_STANDALONE",
    "legacy_value_in_both_fresh_configs": "OFF",
    "link_command_verification": {
      "checker_logic": "import json, os, shlex, sys\n\nCOMPILER = os.path.realpath(\"/usr/bin/x86_64-linux-gnu-g++-13\")\nREQUIRED = (\"-static\", \"-static-libgcc\", \"-static-libstdc++\")\n\ndef validate_link_tokens(argv):\n    if not isinstance(argv, list) or not argv:\n        raise ValueError(\"empty linker argv\")\n    if os.path.realpath(argv[0]) != COMPILER:\n        raise ValueError(\"linker compiler identity mismatch\")\n    if any(token.startswith(\"@\") for token in argv):\n        raise ValueError(\"response files are unsupported\")\n    if any(argv.count(flag) != 1 for flag in REQUIRED):\n        raise ValueError(\"required standalone static linker flags must each occur once\")\n    for i, token in enumerate(argv):\n        if token in {\"-Bdynamic\", \"-dynamic\", \"-shared\", \"--dynamic-linker\"}:\n            raise ValueError(\"conflicting dynamic link mode\")\n        if token.startswith(\"-Wl,\"):\n            forwarded = token[4:].split(\",\")\n            if any(x in {\"-Bdynamic\", \"-dynamic\", \"--dynamic-linker\"} or x.startswith(\"--dynamic-linker=\") for x in forwarded):\n                raise ValueError(\"conflicting forwarded dynamic link mode\")\n            if any(forwarded[j] == \"-B\" and j + 1 < len(forwarded) and forwarded[j + 1] == \"dynamic\" for j in range(len(forwarded))):\n                raise ValueError(\"conflicting forwarded -B dynamic mode\")\n        if token == \"-Xlinker\" and i + 1 < len(argv):\n            next_token = argv[i + 1]\n            if next_token in {\"-Bdynamic\", \"-dynamic\", \"-shared\", \"--dynamic-linker\"} or next_token.startswith(\"--dynamic-linker=\"):\n                raise ValueError(\"conflicting -Xlinker dynamic mode\")\n    return \"link flags PASS\"\n\nif __name__ == \"__main__\":\n    try:\n        argv = shlex.split(sys.stdin.read(), posix=True)\n        print(validate_link_tokens(argv))\n    except Exception as exc:\n        print(\"link check failed: \" + str(exc), file=sys.stderr)\n        raise SystemExit(2)\n",
      "compiler_realpath_required": "/usr/bin/x86_64-linux-gnu-g++-13",
      "exact_count": "each required linker token occurs exactly once as a standalone token",
      "failure_behavior": "Explicit if/raise/SystemExit nonzero checks; no Python assert; execute identical negative fixtures under normal Python, python3 -O, and PYTHONOPTIMIZE=1.",
      "implementation_boundary": "Explicit conditional checker in the authorized D2 manifest/harness package; no assert-based acceptance checks.",
      "negative_fixtures": [
        "wrong linker compiler",
        "response file",
        "missing -static",
        "duplicate -static",
        "missing -static-libgcc",
        "missing -static-libstdc++",
        "-Bdynamic",
        "-dynamic",
        "-shared",
        "-Wl,-Bdynamic",
        "-Wl,--dynamic-linker=...",
        "-Xlinker --dynamic-linker ..."
      ],
      "not_executed_in_r06": true,
      "planned_command": "python3 <inline link-command checker> \"$D2_SCRATCH/build-diagnostic/CMakeFiles/ng039_gw_q0_q1_d2.dir/link.txt\"",
      "reject_dynamic_controls": [
        "-Bdynamic",
        "-dynamic",
        "-shared",
        "-Wl,-Bdynamic",
        "-Wl payload containing -Bdynamic or --dynamic-linker",
        "-Xlinker followed by a dynamic-link override"
      ],
      "required": true,
      "required_exact_arguments": [
        "-static",
        "-static-libgcc",
        "-static-libstdc++"
      ],
      "response_files": "reject any @... token",
      "tokenizer": "shlex.split(posix=True)"
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
    "d1h_evidence_status": "historical manifest and source hashes verified; binary/log unavailable for independent rehash; never runtime evidence",
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
    "r05_response_sha256_rechecked": "9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc"
  },
  "evidence_schemas": {
    "collection_responsibility": {
      "finalizer_bounds": "10 s timeout + 5 s kill grace; RLIMIT_AS 128 MiB, CPU 10 s, FSIZE 4 MiB, NOFILE 32, CORE 0.",
      "finalizer_failure": "No valid finalized record means INCONCLUSIVE and cannot PASS.",
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
          "const": "2477ac94b67ecf6e81665fe2c8899cbfdff31edec08830dc2b60ebbaa7b0d0d0"
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
    "absolute_alias_resolution_required_before_any_future_execution": true,
    "d2_build_directories_created": false,
    "d2_execution_leaf_must_not_be_created_before_review_and_separate_owner_gate": true,
    "d2_execution_leaf_preflight": "absent and not a symlink",
    "d2_harness_or_ipc_executed": false,
    "d2_worktree_created": false,
    "review_directory_preflight": "existing directory; not a symlink",
    "review_packet_directory": "NG_REPO_ROOT/Specs/RESULTS-SPEC-039"
  },
  "local_validation_this_freeze": {
    "assert_based_link_validation_present": false,
    "astra_submitted": false,
    "compile_checker_synthetic_negative_cases_rejected": 26,
    "compile_checker_synthetic_profiles_passed": 2,
    "external_or_paid_calls": false,
    "ipc_vm_d3_or_performance_measurement": false,
    "link_checker_modes": [
      "normal",
      "python3 -O",
      "PYTHONOPTIMIZE=1"
    ],
    "link_checker_negative_cases_per_mode": 12,
    "link_checker_positive_cases_per_mode": 3,
    "link_checker_subprocess_runs": 45,
    "real_compile_commands_configure_build_or_harness_run": false,
    "run_record_invalid_fixtures": 14,
    "run_record_invalid_fixtures_rejected": 14,
    "run_record_valid_fixtures": 4,
    "three_embedded_draft_2020_12_schemas_meta_validate": true
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
    "D1H_WORKTREE": "separate historical compile/link-only worktree at the immutable baseline",
    "D2_RESULTS": "${NG_REPO_ROOT}/Specs/RESULTS-SPEC-039/ng039-d2-v1.9-20261010",
    "D2_SCRATCH": "${HERMES_SCRATCH}/ng039-d2-v1.9-1af604d",
    "D2_WORKTREE": "${NG_REPO_PARENT}/novagenesis-ng039-d2-v1.9-1af604d",
    "HERMES_SCRATCH": "profile scratch root; host prefix withheld",
    "NG_REPO_PARENT": "parent of NG_REPO_ROOT; host prefix withheld",
    "NG_REPO_ROOT": "AIOPT5 repository root; host prefix withheld from the sanitized packet",
    "absolute_resolution_required_before_execution": true
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
  "review_artifacts": {
    "manifest_path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.9-r06.json",
    "packet_path": "Specs/RESULTS-SPEC-039/precode-packet-v1.9-amend-closure-20261010-sanitized-draft-r06.md",
    "packet_sha256": "computed after final packet bytes; not embedded in this manifest to avoid a hash cycle",
    "submission_state": "not submitted; exact-SHA owner consent pending"
  },
  "review_date": "2026-10-10",
  "review_revision": "r06",
  "review_scope": "Document-only, advisory pre-code review. No Astra submission or D2/D3 execution is authorised by this packet.",
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
    "finalizer_command": "prlimit --core=0 --as=134217728 --fsize=4194304 --nofile=32 --cpu=10 -- timeout --signal=TERM --kill-after=5s 10s \"$D2_SCRATCH/build-diagnostic/ng039_gw_q0_q1_d2\" --finalize \"$D2_RESULTS\" \"$RUN_ID\" --outer-code \"$OUTER_CODE\" --outer-signal \"$OUTER_SIGNAL\" --timeout-fired \"$TIMEOUT_FIRED\"",
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
    "launch_command": "prlimit --as=536870912 --fsize=4194304 --nofile=64 --core=0 --cpu=30 -- timeout --signal=TERM --kill-after=5s 20s unshare --user --map-root-user --ipc --mount --pid --propagation private --fork --kill-child --mount-proc \"$D2_SCRATCH/build-diagnostic/ng039_gw_q0_q1_d2\" --run \"$RUN_ID\"",
    "launch_stdout_stderr": "shell redirection to stdout.txt and stderr.txt outside namespaces; close both before finalizer invocation",
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
  "schema_version": "ng039-spec039-precode-review-manifest/v1.9",
  "scope": {
    "algorithm_change": false,
    "correctness_only": true,
    "d3_timing": false,
    "ngal_or_wire_time_coverage": false,
    "production_vm_or_host_ipc_fallback": false
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
    "document_checkout_relation": "local HEAD is two commits ahead of origin/AIOPT5",
    "document_checkout_tracking": "origin/AIOPT5",
    "document_checkout_tree": "9adf665c0eb06213a29a6c12d650451948c7a4ab",
    "functional_baseline_commit": "1af604dad66148618cc0e578c28adb4a9829e607",
    "functional_baseline_tree": "5a3ebb9fbdf61185f05b18e58b8799ced40eb60b",
    "primary_checkout_status_at_freeze_input": {
      "r06_outputs_not_yet_created": true,
      "reviewed_spec_status": "clean",
      "unrelated_modified_path_present": true,
      "unrelated_untracked_results_path_present": true
    },
    "reviewed_spec_clean_in_primary_worktree": true,
    "source_for_code_claims": "immutable baseline Git objects only",
    "unrelated_changes_included_or_validated": false,
    "unrelated_primary_worktree_changes_present": true
  },
  "source_snapshots_before_packet_freeze": {
    "dashboard_sha256": "57aed9be4fbe8109fb7cfb37e3dc8f2e53a056091ef7aceeeb8f157f149bf26a",
    "ng039_task_sha256": "c36aad8d563fd0853a3fee575be4ab88e50481a0a70843f326a336499ced3d84",
    "ng040_task_sha256": "f8797413f668e89dcbd021d7a3bbe590b1c052cc685366f48d754d302239e0b4",
    "post_freeze_note": "The final task/Dashboard will record r06 hashes after packet freeze; those subsequent handoff edits are intentionally not fed back into the packet.",
    "snapshot_timing": "before r06 files and post-freeze task/Dashboard receipt updates"
  },
  "spec": {
    "change_from_v1_8": "Privacy-only stable alias for the D1-H checkout; no design, scope, or gate change.",
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "2477ac94b67ecf6e81665fe2c8899cbfdff31edec08830dc2b60ebbaa7b0d0d0",
    "status": "In Progress",
    "version": "v1.9"
  },
  "synthetic_validation": {
    "compile_negative_cases": [
      "wrong compiler",
      "command/arguments disagreement",
      "false output metadata",
      "conflicting output metadata",
      "unexpected target",
      "direct bare observer define",
      "split bare observer define",
      "direct zero-valued define",
      "split zero-valued define",
      "duplicate observer define",
      "observer undefine",
      "-Wp,-DNG_GW_QUEUE_TEST_OBSERVATION=1",
      "-Xpreprocessor -DNG_GW_QUEUE_TEST_OBSERVATION=1",
      "joined function-like define",
      "split function-like define",
      "-ansi",
      "later -std=gnu++17",
      "later -std=gnu++23",
      "later -O1",
      "-Ofast",
      "-include",
      "-imacros",
      "-specs",
      "response file",
      "-fplugin",
      "-B toolchain override"
    ],
    "d3_measurement": false,
    "harness_or_ipc_execution": false,
    "link_negative_cases": [
      "wrong linker compiler",
      "response file",
      "missing -static",
      "duplicate -static",
      "missing -static-libgcc",
      "missing -static-libstdc++",
      "-Bdynamic",
      "-dynamic",
      "-shared",
      "-Wl,-Bdynamic",
      "-Wl,--dynamic-linker=...",
      "-Xlinker --dynamic-linker ..."
    ],
    "purpose": "Pre-code contract probes only; never compile, runtime, IPC, VM, or performance evidence.",
    "r06_local_preflight": {
      "compile_checker": "PASS: 2 synthetic target profiles and 26/26 negative cases rejected",
      "json_schema_meta_validation": "PASS: 3/3 embedded Draft 2020-12 schemas",
      "link_checker": "PASS: 45 subprocess runs; 3 positive + 12 negative cases under normal, python3 -O, and PYTHONOPTIMIZE=1",
      "link_checker_assertions": "none; explicit nonzero failures",
      "real_compile_database_configure_build_harness_ipc_vm_d3": "not run",
      "run_record_status_fixtures": "PASS: 4 valid and 14 invalid; all invalid records rejected by the revised allOf schema"
    },
    "real_cmake_configure_or_build": false,
    "real_compile_database_or_link_txt": false,
    "schema_status_cases": {
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
    }
  },
  "task_id": "NG-040"
}

===== END FILE: Specs/RESULTS-SPEC-039/precode-manifest-v1.9-r06.json =====
