# SPEC-062: NG-ELC AIOPT5 Cross-VM Lifecycle and 1000-Photo Oracle

**Version:** v1.0-draft
**Date:** 2026-09-28
**Author:** Hermes Agent — draft for Antonio Alberti's review
**Status:** Draft
**Branch:** AIOPT5
**Baseline commit:** `1af604dad66148618cc0e578c28adb4a9829e607`
**AIOPT4 reference commit:** `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`
**Implementation commit:** `5c50a2674e44d676133711de8ba616142da68aab` — local preservation checkpoint for the previously owner-authorized core, read-only channel adapter and tests; this is not SPEC acceptance, service readiness, or authorization for a launch, workload or trial
**Related:** `SPEC-027-pgcs-multithreading.md`; `SPEC-030-photo-delivery-integrity.md` (historical draft input only; not adopted as an AIOPT5 acceptance contract)

> **Draft boundary:** The owner has fixed the target VMs (100/101/102), the Source→Repository ContentApp purpose, 1000-photo workload goal, first-class observability, one 600-second total ContentApp window starting at Repository launch (including both app startups, transfer, required drain and stable completion observation), and 120-second graceful shutdown per guest. The primary performance interval conservatively starts immediately before Source ContentApp launch and ends at stable Repository completion; Source startup is included, and the interval cannot extend the 600-second cap. The exact photo pass rule, JPEG corpus, performance thresholds and other phase budgets remain proposals for review. This SPEC remains Draft; the owner separately authorized the local core and bounded read-only SSH/QGA channel adapter/probes described in §9. That authorization does not cover NovaGenesis service launches, workload, a trial or overall SPEC acceptance. The owner-approved SPEC-027 §8.5 run-record requirement is part of the AIOPT5 test contract. Its text identity is separately pinned to `Specs/SPEC-027-pgcs-multithreading.md` blob `546e4131064beaa82992a78749be67c68ceb6cf6` / SHA-256 `c481af00535c189dd1b24cce71096ab4a7e13f34e771175df251290713d17ad5`; runs must bind that SPEC identity alongside the source-code HEAD. This draft preserves but does not edit that contract.

## 1. Purpose and authority

NG-ELC is the experiment-lifecycle tool for repeatable NovaGenesis proof-of-concept runs on the Alpine guests VM 101 and VM 102, coordinated and evidenced from VM 100. The first AIOPT5 profile shall validate the normal ContentApp path from Source to Repository with a 1000-photo workload, independently verify the received payloads, and report measured end-to-end performance and resource observations.

This SPEC defines an AIOPT5-specific controller/profile contract. It does not import AIOPT4 acceptance semantics, change the NovaGenesis protocol, or amend `Specs/SPEC-027-pgcs-multithreading.md` (hereafter, SPEC-027). The owner-approved, versioned machine-readable §8.5 run-record requirement is normative for AIOPT5 runs. Bind each run to both the source-code HEAD and the approved SPEC-027 text identity recorded above. NG-ELC output is supporting evidence and must be projected explicitly into that record; it is not a replacement for the record or SPEC-027 acceptance.

The project owner has authorised the use and management of VMs 100, 101 and 102 for NovaGenesis proof-of-concept experiments. That authority does not make this draft approved, waive the task's review gate, or authorise an experiment before the profile and lifecycle have been reviewed. No VM operation is part of this document-only task. Scaling beyond the three-VM arrangement is a future design direction, not an implementation or acceptance target of v1.

## 2. Problem

AIOPT4 contains useful experiment-runner, provenance and evidence work, but its artifacts do not form a reviewed, safe, AIOPT5-compatible cross-VM path:

- At the pinned AIOPT4 source, `run_trial()` creates the evidence directory and trial-start record, then defaults `require_fresh_boot=true` and calls `restart_vms()` before guest preflight. `restart_vms()` uses `qm stop` then `qm start`, without graceful-shutdown confirmation. SSH calls have individual 60/120-second limits, readiness timers restart per role, and no absolute deadline spans restart through evidence sealing. This is incompatible with the safe AIOPT5 lifecycle proposed here; do not port or execute it unchanged.
- The AIOPT4 L2 plan runs two PGCS roles and is explicitly diagnostic-only. Its readiness markers do not demonstrate end-to-end payload delivery.
- The L5 plan starts PGCS peers, NRNCS, Repository and Source, but its marker oracle proves progress only. AIOPT4 `Specs/SPEC-060-ng-elc-remote-payload-evidence.md` states that remote payload collection/integration remains open; the pinned remote flow passes `payload_result=None`, so it cannot establish normal-payload acceptance.
- The local intra-OS profile uses four roles on one host and a five-file synthetic workload. It is not a two-guest Source-to-Repository experiment.
- Static source observability and local provenance do not, by themselves, prove which binary ran on each remote guest, that 1000 photos reached Repository, or what end-to-end rate was achieved.
- AIOPT5's tracked `VerifyDelivery.py` compares manifest entries but explicitly ignores extra received files. It cannot, unchanged, prove that the receiver contains exactly the expected 1000-file set.

The proposed solution separates process orchestration, runtime observability, the independent payload oracle, performance measurement and SPEC-027 run-record eligibility. Missing or ambiguous evidence must remain `UNKNOWN`, `INCOMPLETE` or a blocker, never be converted into success.

## 3. Fixed inputs and compatibility findings

The AIOPT4 review is pinned to commit `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`; findings from other commits are not part of this review. Its SPEC-027 blob is `0a02900a86ebfb9d34861b9244396dc9571790ee`, the same source-document identity as the AIOPT5 code baseline. The owner-approved AIOPT5 SPEC-027 text identity used for the §8.5 contract is separately pinned to blob `546e4131064beaa82992a78749be67c68ceb6cf6` / SHA-256 `c481af00535c189dd1b24cce71096ab4a7e13f34e771175df251290713d17ad5`. Preserve the distinction between source-code HEAD and the approved specification text revision; neither identity substitutes for the other.

| AIOPT4 artifact at the pinned commit | Classification for AIOPT5 |
|---|---|
| `Scripts/AlpineVMs/ng_remote_executor.py` | Selectively reusable only as a design reference after controller-only safety review. It creates evidence then restarts before guest preflight, uses `qm stop`/`qm start`, has per-call SSH limits and per-role readiness deadlines but no absolute lifecycle deadline. Do not execute or port wholesale. |
| `Scripts/AlpineVMs/plans/L2-pgcs-only.example.json` | Diagnostic-only two-PGCS topology. Useful as a topology sketch; not a payload oracle or acceptance profile. Its `require_fresh_boot=true` must not be carried forward. |
| `Scripts/AlpineVMs/plans/L5-e7-marker.example.json` | Role-order/readiness sketch only. It is marker-only and does not prove payload integrity or performance; as pinned it sets `diagnostic_only=false` without an `acceptance_profile`, which the remote plan validator rejects. Do not use it verbatim. |
| `Scripts/AlpineVMs/plans/local-intra-os.example.json` | Local-only four-role, five-file oracle. Not a cross-VM profile. The local profile's data/oracle shape may inform fixtures but not AIOPT5 acceptance. |
| `Scripts/AlpineVMs/ng_observability.py` | Static source/log-sink inventory only. It records effective/compiled/runtime/observed states as `unknown` and reports partial coverage; it is not runtime telemetry or a performance oracle. |
| `Scripts/AlpineVMs/local_provenance.py` | Some local provenance techniques may be adaptable after side-effect review. It does not attest each remote participant; `capture_plan_snapshot()` writes files and `validate_build_linkage()` may execute a compiler subprocess, so helpers must not be assumed read-only/pure. |
| AIOPT4 `Specs/SPEC-055-ng-elc-local-mode.md` and `Specs/SPEC-056-ng-elc-evidence-provenance-hardening.md` | Local-lifecycle and local evidence lessons. The former remains `In Progress`; the latter explicitly does not change the remote execution contract. Do not promote their local observations to cross-VM acceptance. |
| AIOPT4 `Specs/SPEC-057-ng-elc-canonical-entrypoint-rename.md` | Canonical-entrypoint rename is `Proposal` at the pinned commit. Keep the AIOPT4 entrypoint name as historical input; do not assume `ng_elc.py` exists or silently rename it in this scope. |
| AIOPT4 `Specs/SPEC-059-ng-elc-qemu-guest-agent-channel.md` | Proxmox/QEMU Guest Agent is an optional backend-specific adapter for privileged execution in VM 100. It does not establish QGA availability on VMs 101/102 and must not be a requirement for the Alpine profile. |
| AIOPT4 `Specs/SPEC-060-ng-elc-remote-payload-evidence.md` | Remote payload contract distinguishes diagnostic marker-only plans from normal payload evidence. Remote integration is `OPEN`; absent `payload_result` is a blocker, not a pass. Its schema/acceptance contract is not imported into this SPEC. |
| AIOPT4 `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md` | A read-only release-readiness evidence evaluator, not an experiment lifecycle controller. Its report/policy/adapter schemas are separate and are not adopted here. |

AIOPT4's release master plan and status register are historical AIOPT4/AIOPT3 release records, not the AIOPT5 Phase-0 authority. The status register is anchored to AIOPT3 commit `cd804a7` from 2026-07-16 and lists 24 SPECs; it is stale for current numbering. The AIOPT4 pinned source tree contains `Specs/SPEC-061-ng-readiness-evidence-orchestrator-r0-r2.md` and no `Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md`; the pinned AIOPT5 baseline `1af604dad66148618cc0e578c28adb4a9829e607` likewise has no SPEC-062 file. SPEC-062 is therefore the next unused project number in the reviewed lineage; this selection does not assert that any AIOPT4 implementation is accepted on AIOPT5.

## 4. Proposed design principles and preserved invariants

1. **Independent oracle:** determine success from an independently generated expected payload manifest and a post-run inventory/hash of the actual Repository files. Readiness text, process liveness, PGCS frames, counters and ELC's own summary cannot substitute for this oracle.
2. **Exact provenance:** bind every run to the exact source revision and tracked/dirty state, build/toolchain/flags, actual per-role executable identity, effective configuration, workload manifest, environment and raw evidence for each participating VM.
3. **Fail closed:** unknown state, incomplete inventory, ambiguous process identity, missing evidence or a deadline expiry blocks launch or yields a non-success result.
4. **No wire/protocol change:** this SPEC concerns the experiment controller, profile, oracle and evidence. Changes to C++, wire format, ContentApp semantics or SPEC-027 acceptance require a separately approved scope/SPEC.
5. **No acceptance by implication:** photo delivery, PGCS RAW throughput, readiness, build success, evidence integrity and teardown are separate results. One result cannot promote another.
6. **Portable control boundary:** QGA is optional. The initial Alpine profile uses an explicitly configured, host-key-verified SSH channel for guest operations; Proxmox status may be used for read-only identity/state read-back.
7. **Bounded initial topology, extensible representation:** v1 is limited to VM 100 as controller/evidence collector, VM 102 as Source and VM 101 as Repository. Plans and records identify participants by role plus VM identity rather than relying on fixed array indices, so a future multi-VM design can extend the model without making v1 an unbounded scheduler project.

## 5. AIOPT5 profile contract

Two distinct experiment classes are supported; they must not be conflated:

### 5.1 `spec027-phase0-raw-baseline`

Proposed profile for `spec027-phase0-raw-baseline`: this is an ELC orchestration/evidence profile for the unmodified two-VM RAW/PGCS baseline specified by `Specs/SPEC-027-pgcs-multithreading.md` at AIOPT5 HEAD `1af604dad66148618cc0e578c28adb4a9829e607` (SPEC Git blob `0a02900a86ebfb9d34861b9244396dc9571790ee`). Its committed §7 and §§8.1–8.4 define this profile; it is distinct from the 1000-photo ContentApp profile.

- **Participants and target:** VM 100 is controller/evidence collector; VM 102 is Source and VM 101 Repository; the two Alpine guests must each satisfy SPEC-027's 2-vCPU/2-GB target. Preflight records role/VM/IP/MAC mapping, exact guest HEAD/dirty state, kernel/Alpine/musl/GCC/CMake, build flags, NIC/MTU, socket buffers, logging, CPU/RSS and VM steal/scheduling. Any mismatch blocks the baseline run; this profile does not resize/reinstall guests.
- **Inventory outputs:** source/build-target map for SAR, CS, GW, tinythread, CMake and SHM readers; ownership and IPC call-site map; SAR keying, timeout, duplicate/replay and ownership behavior; peer-compatibility fixtures. Phase 0 exits only when these maps, raw baseline results and peer fixtures are available.
- **Workload:** run the unmodified `Original baseline` configuration from SPEC-027 §8.2. Cover the listed single-fragment and segmentation-boundary sizes, 16/64-KiB messages, interleaved senders/repeated message numbers, mixed-size traffic, stalled SHM consumer, and malformed/missing/reordered/duplicate fragments. Run each direction separately and bidirectionally; exclude deployment/build activity.
- **Measurement:** use the committed SPEC-027 §8.2 schedule of 30-second warm-up, at least 120 seconds of measured traffic, five throughput repetitions and a 30-minute soak. Report §8.3 unique complete application deliveries/s, useful payload MiB/s, loss after sender-stop/drain reconciliation and one-way delay only with valid clock uncertainty. Populate §8.4 result-table/resource metrics from evidence when collected; these observations do not add exit gates beyond §7.
- **SPEC text binding:** the owner-approved SPEC-027 text revision (Git blob `546e4131064beaa82992a78749be67c68ceb6cf6`) contains the temporal cases and §8.5 run-record contract. A plan pins this SPEC identity separately from the source-code HEAD `1af604dad66148618cc0e578c28adb4a9829e607`; run records must carry both. Keep unmodified source-baseline observations distinguishable from results assessed under the approved specification revision; do not silently change the code SHA or merge comparison arms.
- **Record and limits:** bind every correctness/performance result to its own versioned owner-approved SPEC-027 §8.5 run record and evidence links; record the approved SPEC text identity and source-code HEAD together. The 1000-photo profile is not substituted for the RAW/PGCS matrix; readiness markers, ContentApp results or an ELC `PASS` do not satisfy Phase 0's exit gate.

This profile defines the ELC plan and required evidence; it does not assert that any Phase 0 test, soak, compatibility fixture or acceptance gate has been executed.

### 5.2 `contentapp-source-to-repository-1000-photos`

This is the owner-defined application-level experiment goal; the detailed acceptance rules below are proposals for review:

- Controller/evidence collector: VM 100.
- Source ContentApp and its supporting NovaGenesis roles: VM 102.
- Repository ContentApp and its supporting NovaGenesis roles: VM 101.
- Expected direction: Source → Repository.
- Expected offered photo count: exactly 1000 (owner-specified goal).
- Proposed corpus for approval: 1000 decodable JPEG images. Format, corpus identity and decoder are still open.
- Proposed launch contract: freeze each guest's source-tree identity, build receipt and actual executable hashes, role commands/paths, effective PGCS interface settings, guest MTU, peer identity/MAC, working directories, evidence roots, workload manifest and finite deadlines.
- Proposed readiness order: PGCS peers, NRNCS, Repository ContentApp, then Source ContentApp. Record each gate independently; markers establish readiness only. Verify readiness predicates against the selected AIOPT5 output: the legacy L5 text `State: Operational` is not emitted as a literal by ContentApp at the pinned source HEAD and must not be reused without separate runtime evidence.
- Proposed workload handling: generate and hash before the measured window; independently inventory the Source publish set against the frozen manifest before and after the run. Do not modify the dataset while a publisher is active. Matched arms use the same bytes and manifest; regenerating a random dataset between arms is not a matched workload.

The two profile classes remain separate. Every actual correctness/performance execution has exactly one SPEC-027 §8.5 run record and one unique run ID; never share a record between separate RAW/PGCS and ContentApp executions. One record may reference requirements from both profiles only if one physical execution genuinely exercised both. A 1000-photo result cannot replace the raw message/fragment, peer compatibility, queue/SHM, loss, delay or worker-topology evidence required by SPEC-027.

SPEC-027 §§8.3–8.4 and §10.3 remain a separate RAW/PGCS benchmark contract. Its 20% N=2 target and ≤0.1% loss limits apply only to the selected receive-limited fragmented workload, not to photos or photo goodput. Report photo `photos/s` and verified-payload `MiB/s` separately; do not infer either from fragment, queue-admission or marker counts.

### 5.3 Proposed per-run identity and explicit plan values

The executable plan for each run must contain concrete, non-empty values for participant VM IDs/roles, source and build roots, effective config paths, executable paths, role commands, interface/MAC/MTU settings, evidence roots, workload identity, deadlines, readiness predicates, observation window, drain interval, repetition and comparison-arm identity. Values are captured from the approved guest preflight and sealed into the evidence; unresolved paths or inherited defaults are a preflight blocker. No secret values are written to a plan or run record; credential references identify a protected local secret source without exposing its value.

The baseline source pin for the proposed Phase-0 profile is AIOPT5 `1af604dad66148618cc0e578c28adb4a9829e607`. Each guest's actual `HEAD`, tracked/dirty state and selected dirty-diff identity must be recorded independently. A planned SHA is not proof that a guest is at that SHA. Any changed candidate requires a new plan/run identity and build receipt.

## 6. Proposed independent 1000-photo oracle

### 6.1 Dataset and integrity

The following exact-set and file-format rules are proposed for owner review; they are not yet approved pass thresholds:

1. Candidate rule: exactly 1000 expected regular files with unique safe basenames. Record expected count, total bytes, each filename, size and SHA-256 in a publish-time manifest bound to the unique run ID; preserve and hash the manifest itself.
2. Validate that every input is a decodable JPEG before launch. File extensions or JPEG-like headers alone are not validation. Record decoder/tool identity and results.
3. Store the workload in a unique, isolated staging directory before Source ContentApp starts. Bind the publish-time manifest digest and run ID to the plan/evidence. Verify the frozen staging/Source-publish snapshot against that manifest immediately before launch and after the run. A dedicated Repository destination or verified clean per-run namespace is proposed to avoid stale files contaminating the result. Do not regenerate, overwrite or mutate that dataset during a run.
4. Reconcile the complete expected, Source-published and Repository-received maps after sender stop and drain. The sender-stop/drain needed to establish transfer completion and the stable receiver snapshot must finish within the single owner-approved 600-second ContentApp deadline. Candidate pass threshold: exactly 1000 expected and received photo files; zero missing, mismatched, duplicate names or unexpected photo files; and exact per-file size/SHA-256 equality to the original manifest. Keep manifests/controller evidence outside the ContentApp payload directory. Report expected/received/matched/missing/mismatched/extra counts, total expected/received bytes and manifest SHA-256. Require a run-bound receiver snapshot and an explicit stability policy to limit changes between inventory and hashing. Any final hashing/sealing after the deadline may verify only that within-deadline snapshot; late delivery, an unstable/changed snapshot, or post-deadline delivery needed to complete the set cannot turn a timeout into success.
5. Re-hash preserved evidence copies independently. A `result.json`, marker, reported count or bundle seal without the underlying re-checkable expected/received maps is insufficient.

`Specs/SPEC-030-photo-delivery-integrity.md` is Draft and is used only as historical design input. The pinned `BuildPhotos.py` accepts arbitrary positive counts, uses `secrets.token_bytes` for `different` content without recording a seed, and may leave partial staging after an interrupted generation; it does not independently re-scan the finished set before writing its manifest. The pinned `VerifyDelivery.py` checks listed entries but ignores extras, has no run-ID/manifest-digest binding, and does not protect against mutation between inventory and hashing. These are gaps to address in a separately approved implementation, not claims that an oracle already exists. Reusing a preserved, re-hashed dataset across matched arms is a proposal; per-arm regeneration is not a matched workload.

### 6.2 Proposed functional outcome

The names and conditions below define a candidate result vocabulary only. They become effective acceptance rules only after owner approval.

- `PHOTO_TRANSFER_PASS` requires all 1000 expected images to be valid, present in Repository, exact byte matches, and no unexpected photo files after sender-stop/drain/reconciliation has completed and the stable receiver snapshot has been observed within the fixed 600-second ContentApp deadline.
- A confirmed missing, extra, corrupt, duplicate or non-decodable expected image is a sticky functional `FAIL` for that run.
- An inaccessible receiver, incomplete inventory, unstable destination, unverified input manifest, process/transport ambiguity or deadline expiry is `INCONCLUSIVE`/`BLOCKED`, never `PASS`.
- Lifecycle, functional result, evidence integrity, teardown and SPEC-027 acceptance eligibility are recorded as separate result dimensions.

### 6.3 Proposed measurement boundaries and performance measures

For each photo run, record raw values and the precise measurement method. The owner-approved primary rate interval is a conservative source-launch-to-completion measure; its start is immediately before Source ContentApp launch, while the separate owner-approved total ContentApp deadline still starts at Repository launch. Controller polling/control overhead is included and must be labelled:

- measurement start: controller monotonic timestamp immediately before launching Source ContentApp, after the immutable dataset/manifest is verified. This conservative interval includes Source startup, Service Offer acceptance and publication setup; label the primary rate `Source-launch-to-Repository-completion observed rate`, not transfer-only or wire-only goodput;
- measurement end: controller monotonic timestamp when Repository completion is observed, the receiver is quiescent, and the expected set is stable. Sender-stop/drain needed to establish that completion must finish before the fixed 600-second deadline from Repository launch. Record the polling interval and observation method. Final inventory/hash/decoder duration may follow under its own finite evidence budget, but can only verify the stable snapshot observed within 600 seconds and cannot extend the transfer-success window;
- source-path note: at pinned AIOPT5 HEAD, `ContentApp/src/CoreRunEvaluate01.cpp:643-662` schedules `--contentpublish` after accepted Service Offer/hash verification; `ContentApp/src/CoreRunContentPublish01.cpp:167-174` emits the first `Publishing the content …` log before scheduling that file's publication. Preserve this event as a diagnostic substage timestamp when present; it is not required to establish the primary Source-launch boundary or to extend the owner-approved deadline;
- end-to-end observed elapsed seconds, offered count, verified count and verified payload bytes;
- verified photos/second and verified payload MiB/second, calculated from the measured interval and exact verified bytes;
- separate workload-generation, readiness, drain, final hashing/decoding and teardown durations so they are not silently mixed into the transfer interval;
- all raw per-run values, repetition and comparison arm. A single run reports an observed rate only; it is not evidence of improvement or a stable performance distribution.

Controller-side observation includes its polling/control overhead; the report must label the metric `controller-observed end-to-end delivery rate`, not claim a wire-only rate. Per-photo one-way latency is `UNKNOWN` unless sender/receiver event timestamps are available and clock offsets/uncertainty satisfy the governing SPEC-027 rule. Do not substitute RTT/2. No minimum throughput threshold is set by this draft: the initial purpose is to measure and establish a reproducible baseline. Any performance pass/fail threshold or claimed speed-up requires an explicit owner-approved comparison protocol and matched source, binaries, configuration, workload manifest, offered load and observation windows.

### 6.4 Proposed observability requirements

The proposed minimum is a structured event stream with run ID, UTC and monotonic time, VM ID, role, lifecycle phase, event/counter name, unit, value, source, and evidence reference. It should include, when available:

- launch command identity, PID/PGID, process start time, executable path/hash, exit status and signal/stop result per role;
- readiness observations, publish-start, unique photo offered, source-side submission, intermediate NRNCS/cache/notification/subscription observations, Repository arrival/persistence and final oracle result;
- counter definitions: scope, unit, increment point, reset/overflow behavior, role/process/stage attribution. SPEC-027 stage semantics remain authoritative for SPEC-027 runs; fragments or queue admission are not application delivery;
- per-guest and per-process CPU time/usage, peak RSS, disk free/used, network bytes/errors/drops, process liveness and relevant queue/resource measurements, sampled at a declared interval;
- effective network/interface configuration and time synchronisation/offset uncertainty whenever a cross-host latency claim is made;
- complete raw role logs, raw samples, configuration, plan, manifests, captures and summaries with relative paths, sizes and SHA-256 values.

Absent or unsupported measurements are recorded as `UNKNOWN` with a reason. `ng_observability.py` source inventory, application log markers, `OutputVariable` files or a green lifecycle result are supporting diagnostic evidence; none independently proves the 1000-photo oracle or performance. If a required stage counter does not exist in AIOPT5, record the gap and obtain separate SPEC approval for instrumentation rather than infer it from logs.

## 7. Proposed safe lifecycle and side-effect contract — approval pending

### 7.1 Proposed separation of VM and experiment-process lifecycle

Proposed rule: `run` does not restart, shut down, reset, resize or reconfigure a VM. The plan rejects `require_fresh_boot=true` and any plan/command path containing `qm stop`, force-stop, `qm reset` or broad process-kill commands before the first side effect. The controller does not power off VM 100, which hosts the controller and evidence collection path. Successful trials leave VM power state unchanged.

Proposed rule: the controller may start and stop only its own experiment roles, each launched under a uniquely identified process group/supervisor. Before every signal, revalidate VM, PID, PGID, process start time and executable identity. Send SIGTERM first with a finite grace period; SIGKILL may target only the same still-verified experiment-owned process group after the grace deadline, with the escalation recorded. Never use `killall`, unscoped `pkill`, or cleanup that can remove resources not attributed to this run. If ownership or inventory is ambiguous, perform no destructive cleanup and mark teardown non-success.

Proposed role teardown stops the Source workload producer first, observes the bounded drain, then stops remaining application roles in reverse dependency order, collects logs and payload evidence, inventories processes and IPC, verifies attributable cleanup, and seals evidence. Evidence is durable and independently checked before temporary staging is removed. Foreign processes/IPC, unavailable inventories and incomplete preservation block the run or its acceptance.

### 7.2 Proposed optional guest power operations

Proposal: guest power management is a separate, explicit `prepare/start` or `graceful-stop` operation, never an implicit consequence of `run` and never a fallback after experiment cleanup failure.

The proposed single graceful-stop request is a guest-local OS shutdown command over the configured, host-key-verified SSH session. The controller then performs read-only Proxmox `qm status` read-back for the named VM. The owner-approved maximum graceful-shutdown deadline is 120 seconds per guest. If SSH fails, the VM state is unknown, or read-back does not confirm `stopped` before the deadline, return `SHUTDOWN_BLOCKED`, make no second shutdown request through another channel, do not issue `qm stop`/force-stop, and do not proceed to disk/cleanup operations that require a stopped guest. This path is a design proposal, not a verified capability; it must be reviewed and exercised under a separately approved gate before use. QGA is not assumed available on VMs 101/102.

A start operation, if separately selected in an approved plan, uses one explicit `qm start` request followed by bounded Proxmox state read-back and guest SSH checks for VM identity, boot ID, exact repository state and role. No silent retries or automatic restart loop are permitted. VM 100 remains outside this power-control path.

### 7.3 Proposed preflight and deadlines

Proposal: preflight is read-only and precedes build, role launch or cleanup. It verifies the authorized VMID↔role↔guest↔IP/MAC mapping before any effect, guest identity/state, requested resources, guest OS/toolchain, exact Git identity/dirty state, paths and free space, effective interfaces/MTU/MAC, role binary identities, evidence-root persistence/writability, baseline process and IPC inventories, and workload manifest. If any result is unavailable or contradictory, stop before launch.

Every run must carry finite monotonic budgets for preflight, readiness, total workload, observation, drain, teardown and evidence sealing. One absolute run deadline bounds the sum; restarting a per-role timeout must not extend the total deadline. **Owner-approved ContentApp limit:** exactly one 600-second total deadline starts at Repository ContentApp launch and covers Repository and Source startup/readiness, transfer of all 1000 photos, required sender-stop/drain, stable completion observation and the receiver snapshot used to declare success. It must never reset or be followed by another ContentApp timer. Any drain needed to declare success is nested within this deadline; the separately proposed 180-second drain budget is only a maximum sub-budget and never adds time. After 600 seconds, final hashing/evidence sealing may continue under its own finite budget, but late delivery or a changed snapshot cannot convert timeout into success. The owner-approved graceful guest-shutdown limit is 120 seconds per guest. **Other proposed budgets still requiring review:** preflight 180 seconds per guest; one shared 600-second readiness budget for PGCS peers and NRNCS; 180 seconds maximum for sender-stop/drain, nested within the ContentApp deadline; 300 seconds for teardown; 300 seconds for evidence sealing; and 4200 seconds overall per run. The §6.3 rate interval starts before Source ContentApp launch and is nested within, not an extension of, the owner-approved total ContentApp deadline. Other numeric values are provisional engineering limits, not measured AIOPT5 behavior or performance thresholds. Any exhausted budget is a recorded timeout and blocks success; the controller must not restart the clock or silently continue. Numeric values must appear in the reviewed executable plan and run record. The current AIOPT4 timeout values are not presumed enforced or appropriate for an AIOPT5 1000-photo run.

## 8. Proposed projection into the owner-approved AIOPT5 SPEC-027 §8.5 run-record contract

**Version binding:** This projection uses the owner-approved SPEC-027 text identity pinned to Git blob `546e4131064beaa82992a78749be67c68ceb6cf6` / SHA-256 `c481af00535c189dd1b24cce71096ab4a7e13f34e771175df251290713d17ad5`. The AIOPT5 source-code baseline remains `1af604dad66148618cc0e578c28adb4a9829e607`. These are distinct identities and both must be recorded; the approved §8.5 requirement remains part of the AIOPT5 baseline contract.

Proposal: for each result used by the owner-amended SPEC-027, the controller or a separate adapter emits its §8.5 run record as its own versioned, machine-readable artifact. The record is a projection of that section's requirements, not a copy of NG-ELC `result.json`, an ELC schema, or an ELC `accepted` flag.

The projection maps and independently verifies:

| SPEC-027 §8.5 requirement | Evidence to map from the run |
|---|---|
| Run identity/purpose | Unique run ID, SPEC revision/blob, phase/requirement IDs, `diagnostic`/`correctness`/`performance` classification, repetition, comparison arm |
| Source/executable identity | Per-VM HEAD and tracked/dirty state; selected diff identity; compiler/toolchain, build flags/macros, command/result/log hashes; hash of the actual launched executable for each role |
| Effective configuration | Requested/effective topology, roles, socket/interface settings, MTU/receive buffers, block/resource budgets, preflight/readiness/total-ContentApp/drain/teardown/seal deadlines and absolute run deadline, graceful-shutdown deadline, SAR `T_idle`/`T_max`, fragment/message send deadlines, MN allocator/quarantine policy, SHM mode, log profile, peer versions, workload seed or immutable dataset identity, direction/offered load |
| Measurement window | Source ContentApp launch and Repository completion rate-window timestamps; first Source publish event if captured; repetition, sender stop, drain, stable snapshot and reconciliation; clock offset/uncertainty when one-way delay is reported. Separately record the single total-ContentApp deadline origin (Repository launch), workload offer and delivery completion so lifecycle timeout is not mistaken for measured transfer duration |
| Counters/stage outcomes | Counter scope/unit/increment/reset/overflow and attribution; keep submissions, SAR complete assemblies, GW admission, conversion, SHM publication and final unique application delivery distinct |
| Failure/resources/evidence | Separate submission failures, malformed/oversize/conflicting/expired/capacity-rejected assemblies, queue rejections, duplicates and late arrivals; applicable queue occupancy/high-water/residence, lock wait/hold, SHM publication latency, CPU/RSS/VM scheduling measurements; and relative paths, sizes, SHA-256 for configuration, inputs, logs, raw samples, summaries, captures and tests, with requirement → run/test → artifact → result mapping |
| File identity/snapshot policy from §8.5 | Process only explicitly selected regular files under an approved evidence root; enforce bounded file/count limits and deterministic relative paths; reject duplicates and out-of-root paths; record and test the symlink and mutation-during-hashing policy. Reuse a compliant helper; this adds no separate NG-ELC schema |

Each value must be sourced from captured evidence or set explicitly to `UNKNOWN`/incomplete with a reason. ELC's own runtime classification, marker verdict, bundle integrity, or `accepted` field cannot fill an unobserved SPEC-027 field or override SPEC-027's thresholds. For the 1000-photo profile, the payload oracle and performance summary are linked as application-level artifacts; they do not erase the separate RAW/PGCS performance and compatibility obligations.

## 9. Proposed implementation and review gates — not approved

1. **Owner/Astra review:** the requested GPT-6 Astra review via Codex was completed against the prior packet digest. This revision incorporates its bounded clarifications; owner review of the exact updated SPEC remains required for overall SPEC acceptance. On 2026-09-28, the owner separately authorized the bounded local-only implementation in item 2. Neither Astra review authorizes guest operation.
2. **Owner-authorized local-only increment (2026-09-28):** implement the fixed-profile plan validator, injected-adapter lifecycle core, independent Source-manifest→Repository snapshot oracle, immutable snapshot identity check, and write-once SPEC-027 §8.5 record envelope. Use local fake adapters and focused tests only; add no generic schema. No SSH/QGA adapter, C++/guest instrumentation, build, guest preflight, VM/process operation, workload or trial is included. Missing readiness, decoder, source identity or immutable-snapshot evidence remains blocking/UNKNOWN. Tests cover no power actions, readiness fail-closed, the single Repository-origin 600-second deadline including source stop/drain/stable snapshot, Source-launch rate boundary, exact 1000-file inventory, independent Source manifest, extras/corruption/symlinks/mutation, record uniqueness and CLI entrypoints. This narrow code authorization does not approve the draft's unresolved thresholds, corpus, decoder, operational plan or acceptance criteria.
**Owner-authorized real channel adapter increment (2026-09-28; bounded, read-only; implemented locally):** `Scripts/AlpineVMs/ng_elc_guest_adapter.py` exposes only `probe-ssh` and `probe-qga`. SSH to VM101/102 uses explicit identity and `known_hosts`, disables password authentication, requires strict host-key verification, bounds the external command, captures the hostname/UID probe's own exit codes, and returns separate `readiness_scope=channel` evidence. QGA is optional and exercised only through the configured Proxmox SSH channel to VM100; the QGA call uses `--synchronous 1 --timeout 0`, serializes the complete remote argv once with `shlex.join`, and preserves transport exit status, guest exitcode, raw response and bounded output separately. No fallback to QGA on VM101/102, no adapter power control, no NovaGenesis process launch/build/cleanup, no guest file changes, workload or trial. The final real CLI smoke on 2026-09-28 returned `READY` for SSH VM101 (`alpine-ng-repo`, UID 0) and VM102 (`alpine-ng-source`, UID 0), and QGA VM100 (`1stagent`, UID 0); transport and guest exitcodes were 0. Adapter-specific tests: 10 passed; full `Scripts/AlpineVMs/tests`: 30 passed; `py_compile`, CLI help and `git diff --check` passed. These prove transport-channel readiness only. A read-only process inventory found no PGCS/NRNCS/ContentApp; service readiness remains UNKNOWN. VMs101/102 were gracefully powered off after the probes and Proxmox read-back confirmed both `stopped`. No generic schema or legacy controller port is authorized.
3. **Read-only guest preflight:** after the contract and implementation review, capture per-guest identity/resource/path/evidence-root conditions without mutation. Any unresolved guest, backup, storage, 2-vCPU/2-GiB target, or preserved-worktree blocker keeps Phase 0 blocked.
4. **Approved bounded trial:** only after exact candidate/build identity and preflight are accepted, execute a separately reviewed plan on VMs 101/102. Preserve results and run records; no automatic VM restart/shutdown. Any operational deviation invalidates the affected run classification.
5. **Performance comparison:** first report the measured baseline and its uncertainty. A speed-up or performance gate requires a separately explicit comparison matrix and owner-approved threshold; diagnostics/debug builds are excluded from the performance arm.
6. **Future scale:** multi-VM scheduling, distributed controller/high-availability, additional hypervisors/QGA backends and aggregate dashboards are deferred. The v1 profile must not claim multi-VM scale validation.

## 10. Proposed acceptance criteria — none approved yet

- [ ] AIOPT4 inputs are reviewed at the exact pinned SHA and separated into reusable, adaptable, incompatible and open items.
- [ ] An AIOPT5 two-guest profile identifies VM 100/101/102 roles, exact per-run source/build/config/path/binary identities, readiness, workload, deadlines, evidence and result.
- [ ] The 1000-photo oracle proves exact-set, byte-level Repository delivery and JPEG decodability from a frozen publish-time workload manifest.
- [ ] Functional delivery, measured performance, observability, teardown, evidence integrity and SPEC-027 acceptance eligibility are distinct outcomes.
- [ ] Performance is reported with defined timing boundaries, raw values, verified byte/photo counts and resource observations; no threshold or speed-up is claimed without an approved matched protocol.
- [ ] Lifecycle has read-only preflight, bounded identity-safe process teardown, no implicit VM restart, no `qm stop`/force-stop path, the owner-approved 120-second graceful shutdown limit per guest, a reviewed shutdown/read-back channel, and fail-closed behavior on timeout/unknown state.
- [ ] Every SPEC-027 correctness/performance execution has exactly one versioned §8.5 record bound to the owner-approved SPEC text identity and source-code HEAD; records are not shared across distinct executions. Required invariant fields, unique run identifier and measured-start/end timestamps are covered by focused contract tests; no NG-ELC schema or acceptance semantics are imported.
- [ ] The evidence file selector/snapshot behavior preserves §8.5: explicit regular-file selection under the approved root, bounded count/size, deterministic paths, duplicate/out-of-root rejection, and tested symlink/mutation policy.
- [ ] Controller-only tests, static checks, scoped reference checks and owner review of the updated SPEC pass before any trial; VM/Phase-0 gates remain blocked until the plan is approved.
- [ ] Future multi-VM expansion is explicitly deferred and not represented as validated.

## 11. Files and rollback

| File/surface | Scoped change | Reason |
|---|---|---|
| `Scripts/AlpineVMs/ng_elc_contentapp_runner.py` | Local-only fixed-profile validator/controller core and payload snapshot verifier; no guest transport | Bounded safe lifecycle contract |
| `Scripts/AlpineVMs/tests/test_ng064_contentapp_runner.py` | Fake-adapter lifecycle, snapshot, record-envelope and CLI tests | Regression evidence without VM access |
| `Scripts/AlpineVMs/ng_elc_guest_adapter.py` | Implemented bounded host-key-verified SSH to VM101/102 and optional QGA to VM100; read-only probes with raw transport/guest-exit evidence; no role launch or legacy-controller port | Owner-authorized bounded increment; channel readiness only |
| `Scripts/AlpineVMs/tests/test_ng064_guest_adapter.py` | 10 focused tests for SSH/QGA quoting, identity, strict auth, deadlines and fail-closed results | Local test evidence; does not imply service readiness |
| `Scripts/AlpineVMs/ng_remote_executor.py` and guest-side helpers | No port/modify/use in this increment | Avoid unsafe legacy lifecycle and unapproved guest effects |
| `Specs/RESULTS-SPEC-027/` | Store records/raw artifacts only after a separately approved real run | Reproducible evidence |

The local Python core, channel adapter and tests are preserved in AIOPT5 commit `5c50a2674e44d676133711de8ba616142da68aab`. This is a preservation checkpoint for the previously owner-authorized local component scope, not SPEC approval or acceptance. No production C++/shell files, NovaGenesis service launch, workload or trial were included. VMs101/102 were started solely for historical read-only SSH channel probes under the owner's standing authorization, then gracefully powered off and read back as `stopped`. Overall SPEC acceptance and any NovaGenesis service/workload run remain unapproved.

## 12. Owner directions, proposals and open questions

| Date | Decision/proposal | Rationale | State |
|---|---|---|---|
| 2026-09-28 | Treat NG-ELC as the AIOPT5 experiment lifecycle tool for VMs 101/102; make the 1000-photo Source→Repository ContentApp oracle and observability central; design for later, not current, multi-VM growth | Owner direction | Confirmed direction; SPEC approval pending |
| 2026-09-28 | Project evidence into the owner-approved SPEC-027 §8.5 run-record contract; do not import NG-ELC schema or acceptance | Preserve the separate run-record contract the owner requested | Bind the approved SPEC text blob and source-code HEAD separately |
| 2026-09-28 | Reject automatic VM restart and all `qm stop`/force-stop paths; keep VM power-state management separate from experiment-process lifecycle | AIOPT4 runner is unsafe for current guests; avoid power effects inside a run | Proposed; review pending |
| 2026-09-28 | Use exact-set payload verification; existing verifier's omission of extra-file checks is insufficient | A 1000/1000 hash comparison alone can miss unexpected receiver files | Proposed threshold/rule; review pending |
| 2026-09-28 | Use exactly one 600-second total ContentApp deadline from Repository launch, covering both app startups, transfer and completion observation; keep the owner-approved 120-second guest shutdown limit separate | Owner selected a 10-minute total window from Repository launch because initialization can be long | Fixed timing boundary; no second or restarted ContentApp timer |
| 2026-09-28 | Keep all drain required for success inside the fixed 600-second deadline; allow later hashing only against the within-deadline stable snapshot; use Source ContentApp launch as the conservative primary rate start and label startup as included | Astra found that a separate drain timer could otherwise imply a late success and that the first offer boundary was not yet bound to an observable event | Applied at owner request; rate is Source-launch-to-Repository-completion, not wire-only |
| 2026-09-28 | Preserve SPEC-027 §8.5 file-selection/snapshot limits in the projection and planned focused tests; keep one record per actual execution | The approved run-record contract already requires bounded, rooted, deterministic file evidence | Applied without adding another schema or governance layer |
| 2026-09-28 | Authorize only the local profile-specific plan/controller core and fake-adapter tests; exclude SSH/QGA, builds, guest preflight and runtime operations | Remove the missing tracked plan/controller-core blocker without guessing current guest values | Local code/tests may proceed; the operational plan, real readiness/snapshot adapter and trial remain blocked |
| 2026-09-28 | Authorize an AIOPT5-specific SSH/QGA adapter and read-only channel-readiness checks; separately grant standing permission to power on/off VMs101/102 for NovaGenesis | Direct owner instruction; adapter implementation, full 30-test suite and real SSH/QGA channel smoke completed | Channel-only `READY`; no service readiness, build, role launch, workload or trial; graceful shutdown/read-back preserved, no `qm stop`/force-stop fallback |
| 2026-10-10 | Preserve the previously authorized local component code and tests in an AIOPT5 commit | Owner requested reducing untracked worktree noise and committing selected preserved work by SPEC/worktree | Commit `5c50a2674e44d676133711de8ba616142da68aab`; local suite `30 passed`; SPEC remains Draft and blocked; no push, build, VM action, service launch, workload, trial or acceptance |

Open before any NovaGenesis role launch/workload/trial: confirm the separate proposed PGCS/NRNCS readiness budget; select the exact 1000-image corpus and decoder; verify an observable Repository readiness predicate and immutable within-deadline snapshot procedure; bind concrete guest source/build/config/path/binary/network/evidence identities; review graceful-shutdown behavior/read-back through the selected channel; settle other proposed teardown budgets; determine which stage counters exist versus require separately approved instrumentation; and select a matched-performance protocol/threshold after a baseline. The initial SSH/QGA smoke is channel-access evidence only and does not prove service readiness or satisfy complete guest preflight. Drain does not extend the 600-second deadline. The 600-second total ContentApp window from Repository launch and 120-second shutdown limit per guest are owner-approved and are not reopened here.
