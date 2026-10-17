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