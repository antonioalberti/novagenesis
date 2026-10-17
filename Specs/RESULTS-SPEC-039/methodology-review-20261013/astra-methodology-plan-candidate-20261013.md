# Astra plan-only review: why SPEC-039/NG-039/NG-040 keep stalling

You are GPT-6 Astra. This is a request for a **methodology improvement plan**, not a fresh acceptance review of r12, not a request to amend production code, and not permission to execute anything. The owner asked us to use the accumulated evidence, identify why the review/test cycle keeps stalling, and recommend how to improve the process, tooling and transferability beyond the Gateway Q0/Q1 experiment. Do not return an `APPROVE`/`AMEND` verdict on r12; give an evidence-grounded plan.

## Exact scope and authority boundary
- Current repository: NovaGenesis AIOPT5, branch `AIOPT5`, HEAD `756f493b7049434ab67cf2382486a48f606a470c`, tree `9adf665c0eb06213a29a6c12d650451948c7a4ab`; functional test baseline `1af604dad66148618cc0e578c28adb4a9829e607`, tree `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`.
- Current task state: NG-039 and child NG-040 are `bloqueada`; r12 was submitted once and returned `AMEND`; that one-call consent is consumed. D2 correctness run and D3 measurement have not occurred. D1-H was compile/link only; the binary was not run. Dedicated UID/launcher/lease remains `NOT_PROVISIONED`.
- The owner asked for a methodology plan. **No exact-SHA consent for this new plan packet has yet been given**, so do not treat this request as authorization to call Astra again or make changes. The plan packet SHA will be presented separately for owner consent.
- No code, SPEC, manifest, fixture, runtime, IPC, VM, commit or push change is authorized by this plan request. Do not recommend silently weakening owner-approved safety constraints.

## Questions
1. Classify each round’s finding as a distinct concrete defect, recurrence of an earlier defect, sender/envelope/process error, or advisory. Do not conflate a new symptom with a repeated root cause. Note the bounded r07 documentary `APPROVE` and what it did not approve.
2. Give a ranked root-cause analysis for successive AMENDs. Separate evidence-backed causes from hypotheses and cite the supplied records.
3. Identify the smallest process changes that would have prevented the r09–r12 loop. Assess: one canonical executable contract/state model; phase/resource transition modeling; property/model-based fixtures; automated consistency checks for repeated SPEC/manifest/packet projections; exact-SHA/envelope checks; and independent pre-Astra audit. Mark what is necessary versus overengineering.
4. Recommend a bounded sequence to finish current Q0/Q1 without confusing documentary closure, D2 correctness, and D3 performance. Include stop conditions and owner decisions; do not grant authorization.
5. Decide whether methodology improvement belongs in a new process SPEC/task/tool, a focused change to existing SPEC/skill, or a smaller combination. Define scope so it does not create another unbounded review programme or duplicate policy source.
6. Explain how the reusable method should scale beyond Q0/Q1 (e.g., serialization, SAR/HT, runtime/lifecycle) while retaining subsystem-specific invariants, fixtures, and authority gates.
7. Give a practical plan: phases, deliverables, measurable acceptance criteria, owner decisions, and minimum correction versus optional hardening. Prefer finite artifacts and a finite review loop. No implementation patch is requested.

Return concise UK English: (A) executive diagnosis, (B) round-by-round recurrence table, (C) ranked causes/evidence, (D) minimal plan/tooling/acceptance criteria, (E) owner decisions and explicit non-authorities. State limitations plainly. If a source is summarized in the task note rather than supplied as the original, label it as a summary.

===== CURRENT REPOSITORY IDENTITY =====
HEAD `756f493b7049434ab67cf2382486a48f606a470c`; tree `9adf665c0eb06213a29a6c12d650451948c7a4ab`; functional baseline `1af604dad66148618cc0e578c28adb4a9829e607` / `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`.
`git status --short --branch` at packet assembly:
```text
## AIOPT5...origin/AIOPT5 [ahead 2]
 M Specs/SPEC-039-common-performance-measurement.md
 M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md
?? Specs/RESULTS-SPEC-039/
?? Specs/RESULTS-SPEC-059/
```
Current SPEC SHA `0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115`; manifest SHA `63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917`; prior r12 packet SHA `a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd`; validation receipt SHA `481fb1dd8b5cad5951a5916a16e4e03481dc8fe70687923a3f60c7c77b5e4c73`. r12 response SHA `6f7228f955475d5fbad6cc2c3508c960c86e264f49c1037f82f4096b769d4e97`; raw SSE SHA `b6a6e0ded8ff307bc43a13177832436087f309e16c8ee47c2215c6fdf93caf19`. These identities were rechecked during packet assembly.

===== VERIFIED ROUND-BY-ROUND SUMMARY =====
- r03–r05 (summarized in the current NG-039 task note; original responses are not all present in the active results directory): compiler-checker bypasses/false-PASS concerns including forwarded/function-like macros, `-ansi`, optimized Python `assert`, path/target provenance, environment, link validation, and schema/finalizer status division.
- r06: `AMEND` for trusted-root containment/exact `-c`, deterministic environment, exact `link.txt` binding/forwarding controls, and status/schema/finalizer gaps.
- r07 first packet: `AMEND` for literal `python -c` commands using an unconsumed `--` and joined forced-include/macro-file forms. A later r07 packet got `AMEND` because manifest/run-record schema pinned a different SPEC digest. After correction, Astra returned a bounded `APPROVE` for B1/B2/B3 documentary closure only; no D2 implementation/runtime authority.
- r08 host-controlled review: `AMEND` for shared-host continuous identity/exclusion, unchecked unlink/TOCTOU and journal/raw-inventory/semaphore-mapping evidence. The initial identity B1 was withdrawn as our envelope typo (the sender omitted `128` from the SPEC SHA); B2/B3 remained real.
- r09: `AMEND` because one clause prohibited the finalizer from touching IPC while other clauses assigned exact inspection/removal. r10 resolved that boundary.
- r10: `AMEND` because abnormal failure finalization required an INCONCLUSIVE record while unconditional finalizer prerequisites required successful joins and Process/GW destruction.
- r11: `AMEND` because lease loss could select abnormal mode despite a both-mode lease prerequisite; cleanup history could be misreported as `not_started` after an intent/removal. Advisory: normal strict bundle validation must not block safe abnormal raw preservation.
- r12: `AMEND`. Full SPEC and manifest were supplied with the packet. B1: schema-valid orphan `REMOVED` transaction is classified `not_started`; B2: SPEC and manifest disagree about `complete` versus `partial/unknown` after a normal finalization failure. The r12 review confirmed r11 lease/authority B1 closed and r08 B2/B3 retained.

**Important scope reality:** there has been no D2 harness/semantic-finalizer execution, no Q0/Q1 correctness run, and no D3 timing. The previous local r12 checker passed its declared fixtures, but the newly reproduced transition was absent from the matrix. Thus review rounds have been about the design/evidence contract, not repeated failures of an executed Gateway test.

===== EXACT LOCAL COUNTEREXAMPLE (HERMES-RERUN, READ-ONLY) =====
```text
JOURNAL_OPENED
CREATE_INTENT(resource=sysv_shm,key=11)
RUN_SHUTDOWN_COMPLETE
REMOVED(resource=sysv_shm,key=11)
JOURNAL_CLOSED
```
Each event passed the `reservation_journal_event` schema; sequence/timestamps were consecutive. `verify_spec039_v14_r12.classify(...)` returned `not_started`; expected contract result is `unknown`. No files, IPC, services, runtime or production code were touched. Relevant code is included below.

===== CURRENT SPEC / MANIFEST NORMALIZED EXTRACTS =====
SPEC-039 clauses:
```text
SPEC-039:119: After Gateway/output workers stop and join and `Process/GW` are destroyed, the harness must append/fsync exactly one `RUN_SHUTDOWN_COMPLETE` event to the still-open reservation journal. This event is the positive proof that normal shutdown completed; a caller boolean or process exit is not a substitute. The trusted supervisor then waits/reaps every recorded harness process PID and closes stdout/stderr before invoking the same allowlisted `--finalize` binary. It selects `normal_cleanup` only when the journal contains exactly one valid shutdown-complete event, every recorded process has a verified wait receipt, both logs are closed, no timeout/signal/ambiguous termination occurred, and the lease remains continuously valid. In this mode the finalizer captures post inventory, validates/replays the journal, reads required inventories, inspects exact current identities (`shmctl(..., IPC_STAT, ...)` for SysV SHM and `stat`/equivalent metadata for the exact POSIX semaphore object), and removes only journal-proven run-created resources. For each resource it appends/fsyncs `CLEANUP_INTENT`, rechecks lease and identity immediately before the destructive call, invokes only exact `shmctl(exact_shmid, IPC_RMID, ...)` or `sem_unlink(exact_recorded_name)`, then appends/fsyncs the matching outcome. The `normal_cleanup` gate above is satisfied only while the trusted launcher, supervisor receipt, execution identity, result-directory identity and continuous lease remain valid through final-record closure. If normal-shutdown proof is absent while these common authority/file-safety conditions remain valid, the finalizer may choose `abnormal_evidence_only`. Loss or ambiguity of the lease, supervisor proof, execution identity or result-directory authority is not an abnormal-mode trigger: do not invoke or re-enter the finalizer; preserve the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A failure during normal cleanup after any `CLEANUP_INTENT` stops further destructive calls and retries; while authority remains valid, record `INCONCLUSIVE` with the evidence-derived cleanup outcome in the current finalization, otherwise retain pending recovery state. No ambiguous deletion is retried.
SPEC-039:121: In `abnormal_evidence_only` mode, only while the trusted launcher, execution identity, result-directory identity and continuous lease remain verified, and after a valid `supervisor-termination.json` proves every recorded process was reaped and both logs were closed, the finalizer strictly validates the pending run record and supervisor receipt. It may then open only allowlisted evidence names using no-follow regular-file checks, preserving and hashing safely readable closed raw bytes within per-file caps; it does not require all eight evidence files, parse/replay their JSON or journal schemas, or enforce the normal complete-bundle check. `failure.missing_artifacts` lists required evidence names actually absent; the new `failure.unavailable_artifacts` lists required names present but unreadable/unsafe/over-cap. These sets are disjoint, exclude the run record and optional `harness-diagnostics.txt`, and neither may falsely classify a present file as missing. Malformed but safely readable evidence is preserved and hashed, not parsed or used to block an otherwise permitted `INCONCLUSIVE` record. A bounded cleanup-history classifier may recognize only well-formed journal intent/outcome records to distinguish `not_started`, `partial` or `complete`; it does not perform full schema replay, and any missing, malformed, truncated or conflicting history yields `unknown` without preventing evidence finalization. The final record sets `record_finalized=true`, `finalization_mode=abnormal_evidence_only`, and `status=INCONCLUSIVE`. `failure.cleanup_outcome` is `not_started` only if a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; it is `partial` when valid durable evidence proves at least one cleanup action failed; it is `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the run remains `INCONCLUSIVE` because abnormal mode has no final-inventory proof; and it is `unknown` when cleanup history/outcomes are absent, malformed, truncated or ambiguous. The abnormal finalizer performs no new cleanup, does not retry, and preserves journal bytes. If the lease, receipt, process/log proof or result-directory authority fails at any point, do not write/re-enter finalization; keep the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A present but partial, stale, or shutdown-unverified `ipc-post.json` or `ipc-final.json` may be preserved and hashed as a file, but must not be treated as a verified inventory: the run-record post/final inventory summaries remain unavailable, with `is_empty=null` and no claimed inventory digest. The finalizer never reads `/proc/sysvipc` or `/dev/shm`, calls `IPC_STAT`, `IPC_RMID` or `sem_unlink`, appends/repairs/closes the journal, or performs cleanup during this mode.
SPEC-039:123: In `normal_cleanup` mode, capture a complete final inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required runtime artifact exactly once. The finalizer strictly parses journal/inventories, recomputes event order/transitions, raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. PASS/FAIL require the complete bundle and safe normal cleanup; missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. If normal cleanup has written any `CLEANUP_INTENT` and then fails, stop subsequent destructive calls and retries; while lease and result-directory authority still hold, write an `INCONCLUSIVE` final record with `cleanup_outcome=partial` or `unknown` as supported by the journal. If authority or safe record writing is lost, leave the pending/partial evidence and report `BLOCKED/INCONCLUSIVE`; never re-enter finalization to claim `not_started`. The abnormal evidence-only path described above preserves the historical cleanup outcome and performs no further cleanup. D2 remains one controlled correctness run only; no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.
SPEC-039:139: 5. Bounded run-record and dual-mode finalizer contract: `tests/spec039/q0_q1_d2.manifest.json` defines strict Draft 2020-12 schemas for the pending/final run record v5, `supervisor-termination.json` receipt v1, IPC inventories, observer events and `reservation-journal.jsonl` events v2. The finalized run record's `finalization_mode` is exactly `normal_cleanup` or `abnormal_evidence_only`; pending records use null, and the finalizer derives the mode from journal and supervisor evidence rather than a caller label. The same allowlisted `--finalize` binary is the sole semantic authority. Both finalization modes require a strictly valid pending run record and supervisor receipt; every recorded PID reaped; closed stdout/stderr; unchanged execution/result-directory identities; and a dedicated-UID lease continuously held through final-record closure. If any common prerequisite is absent, ambiguous or lost, do not invoke/re-enter the finalizer or write a final record; retain pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. In `normal_cleanup`, strictly parse/replay all required JSON/JSONL, verify raw `/proc/sysvipc` hashes/rows, exact artifact set, wait/log receipt, lease and semaphore mapping, then inspect exact identities and perform only journal-authorised cleanup after `CLEANUP_INTENT` fsync and immediate identity/lease recheck. `RUN_SHUTDOWN_COMPLETE` must be durable after joins/destruction and before any cleanup intent. PASS/FAIL require the full normal evidence and inventory gates; if a normal cleanup attempt fails after an intent, stop further destructive calls/retries and write `INCONCLUSIVE` with `cleanup_outcome=partial` or `unknown` as supported by durable evidence. If the final record cannot safely be written, leave pending recovery evidence.
SPEC-039:141: In `abnormal_evidence_only`, normal shutdown proof may be absent, but all common prerequisites still hold. Strictly validate the pending record and supervisor receipt; for other allowlisted evidence files, use no-follow regular-file checks, preserve/hash safely readable closed raw bytes within bounds, but do not require the full bundle or parse/replay inventory, observer or journal schemas. Malformed but safely readable evidence must not block a final `INCONCLUSIVE` record. `failure.missing_artifacts` is exactly the required evidence names actually absent; `failure.unavailable_artifacts` records required names present but unsafe, unreadable or over-cap. The lists are disjoint; neither includes the record itself or optional `harness-diagnostics.txt`. Bound raw-byte reads by the per-file and 32 MiB total caps; classify over-budget files unavailable instead of aborting final-record creation. Present post/final files may be preserved but their inventory summaries remain unavailable, never empty. Derive `failure.cleanup_outcome`: `not_started` only when a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; `partial` when valid durable evidence proves a cleanup attempt failed; `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the abnormal record remains `INCONCLUSIVE` without final-inventory proof; and `unknown` when cleanup history or outcomes are incomplete, malformed, truncated or ambiguous. This mode always yields `INCONCLUSIVE`, performs no IPC inspection, cleanup or journal mutation, and never retries. A cleanup intent or prior removal is reported truthfully, not rewritten as `not_started`.
SPEC-039:143: The bounded semantic regression set must reject claimed PASS/FAIL for missing/unverified launcher or lease, a second/foreign process, diagnostic `sem_open` failure calling `sem_unlink`, pre-existing candidate keys/names, unreadable/over-cap/raw-parsed-mismatched normal inventories, noncanonical base64, missing/reordered/duplicated/unfsynced normal journal events, candidate identity/mode mismatch, missing `sem_t` mapping, residual candidates, unexpected host changes, or final-inventory mismatch. Reject normal cleanup without a durable `RUN_SHUTDOWN_COMPLETE`, complete supervisor wait receipts, closed logs and continuous lease. Reject invocation of either mode before all recorded PIDs are reaped and logs closed; when common launcher/receipt/lease/result-directory authority is absent, retain pending evidence and report `BLOCKED/INCONCLUSIVE`. Abnormal fixtures must accept malformed but safely readable non-control evidence as raw preserved/hashable bytes without requiring a complete bundle or parsing it; reject claims of PASS/FAIL, empty inventories, joins/destruction, optional diagnostics in `missing_artifacts`, overlap between `missing_artifacts` and `unavailable_artifacts`, or an inaccurate cleanup-history outcome. `not_started` requires a complete journal proving no cleanup intent and no successful `CREATED` resource; a proven failed attempt is `partial`; `complete` requires exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure; missing/truncated/malformed, intent-without-outcome, or created-without-cleanup history is `unknown`. Abnormal mode must never retry cleanup or mutate journal/IPC. The finalizer must perform no speculative deletion for disputed resources. Normal PASS/FAIL requires a complete, replayable journal and every required evidence artifact; the normal bundle has eight evidence artifacts plus `run-record.json`, optional `harness-diagnostics.txt`, and a ten-file maximum. Normal artifacts are each ≤4 MiB, the journal ≤256 KiB/128 events, the final run record ≤256 KiB, and the complete normal bundle ≤32 MiB. Abnormal raw preservation remains no-follow and bounded: per-file/total read caps apply, but excess/unsafe present files are recorded as unavailable rather than making strict schema/completeness validation a prerequisite to the `INCONCLUSIVE` record.
SPEC-039:145: The host-controlled runtime shape remains one controlled D2 process tree under the dedicated UID, one Q0/Q1 correctness run, no network input and no performance timing. The launcher serializes the run and finalizer under the verified exclusive lease; the harness itself spawns no child process. The host IPC namespace may contain unrelated objects, but all candidate resources must be absent before reservation and all exact cleanup remains bound to the durable journal. No broad host cleanup is permitted. If the launcher/lease or result-directory identity is absent/lost, a second process enters the identity domain, or supervisor evidence is incomplete, do not perform name-based unlink/RMID or invoke/re-enter finalization; preserve pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. An abnormal record may be written only while common lease and file-safety authority remains valid.
SPEC-039:147: **Next gate:** resolve the r11 B1 lease-loss versus abnormal-mode eligibility and B2 historical cleanup-outcome contradiction in this same v1.14 SPEC; scope strict completeness/schema/replay checks to `normal_cleanup`, while abnormal evidence-only mode safely preserves malformed/partial non-control evidence and records missing/unavailable files plus `not_started`/`partial`/`complete`/`unknown` cleanup history without IPC or retries. Update run-record v5, manifest and packet; validate both abnormal and normal failure branches and bind fresh exact hashes. The r11 one-call consent is consumed; obtain fresh owner consent bound to the final r12 packet SHA before Astra. D2 implementation/configure/build/run remains blocked until a positive documentary review, provisioned launcher/preconditions and separate owner execution gate. D3 and Q2–Q4 remain deferred. This work is documentary, not correctness or performance evidence.
```
Manifest policy paths (exact current values, extracted from the full hash-pinned manifest):
```json
{
  "classification_rules.cleanup_outcome_contract": {
    "pass_requires": [
      "record_finalized=true; failure=null; outer_code=0; harness_code=0; signal=null; timeout_fired=false",
      "every required assertion PASS; actual Q0/Q1 values are non-null and satisfy the frozen local conditions",
      "complete host pre/post/final inventories; candidate keys/names absent in pre; all five SHM and semaphore candidates exclusively reserved, journaled, mode 0600, and mapped under the recorded libc",
      "post candidate identities match exclusive reservations; exact run-created resources removed after stop/join; final complete inventory equals pre byte/semantic identity; no counter overflow; Q1 correlation MATCHED",
      "verified dedicated UID, launcher identity and exclusive lease held from pre-inventory through finalizer; no concurrent process in the execution identity domain",
      "all three diagnostic GW.cpp sem_open-failure paths are built with the no-sem_unlink guard and their injected failure tests pass",
      "complete strictly ordered fsynced reservation journal, closed and hash-bound as reservation-journal.jsonl",
      "raw /proc/sysvipc bytes retained in canonical base64; recomputed hashes and parsed normalized summaries match each inventory record",
      "each returned sem_t* is proven to map to the exact recorded libc-backed /dev/shm device/inode for its decimal source name",
      "verified dedicated non-login UID/launcher identity and exclusive lease continuously held through finalizer",
      "all three diagnostic GW.cpp sem_open-failure branches are guarded; targeted failure tests show no sem_unlink",
      "complete ordered fsynced reservation journal is closed and hash-bound as required artifact",
      "raw /proc/sysvipc bytes retained; hashes and normalized summaries recomputed from exact bytes",
      "each returned sem_t* is proven to map to exact recorded libc-backed /dev/shm inode",
      "validated supervisor-termination.json receipt binds the exact process-tree PID set to raw waitpid statuses; stdout/stderr closed and available log hashes match",
      "normal_cleanup has durable RUN_SHUTDOWN_COMPLETE after worker joins and Process/GW destruction; no timeout/signal/ambiguous wait",
      "abnormal_evidence_only is INCONCLUSIVE, requires confirmed process reaping and closed logs, and performs zero IPC inspection/destructive calls",
      "schema-valid supervisor-termination.json whose raw wait receipts cover the exact process_tree_pids and whose closed-log hashes match",
      "durable RUN_SHUTDOWN_COMPLETE before normal_cleanup and before every cleanup intent"
    ],
    "fail_requires": [
      "record_finalized=true and failure.code=ASSERTION_FAILED",
      "at least one correctness assertion FAIL; every inventory/reservation/cleanup assertion PASS; no INCONCLUSIVE/NOT_RUN assertion",
      "known harness exit; normal outer code 0..123; signal=null; timeout_fired=false",
      "all run-created IPC removed by exact identity and final inventory equals pre; failure.missing_artifacts=[]; no uncertainty trigger",
      "failure summary exactly matches recomputed evidence",
      "dedicated UID/launcher/lease verified",
      "journal complete and replayable",
      "raw table digests/summaries recomputable",
      "returned semaphore mapping verified",
      "all required run artifacts including reservation-journal.jsonl present",
      "complete closed journal and eight runtime artifacts",
      "raw inventories/hashes independently recomputable",
      "diagnostic failure paths cannot unlink by name",
      "validated supervisor-termination.json receipt binds the exact process-tree PID set to raw waitpid statuses; stdout/stderr closed and available log hashes match",
      "normal_cleanup has durable RUN_SHUTDOWN_COMPLETE after worker joins and Process/GW destruction; no timeout/signal/ambiguous wait",
      "abnormal_evidence_only is INCONCLUSIVE, requires confirmed process reaping and closed logs, and performs zero IPC inspection/destructive calls",
      "schema-valid supervisor-termination.json whose raw wait receipts cover the exact process_tree_pids and whose closed-log hashes match",
      "durable RUN_SHUTDOWN_COMPLETE before normal_cleanup and before every cleanup intent"
    ],
    "inconclusive_precedence": [
      "timeout/signal or ambiguous outer code",
      "missing/invalid stage/final/artifact or duplicate artifact name",
      "finalizer/schema/size/bundle failure",
      "identity/sequence/correlation ambiguity or counter overflow",
      "partial construction or blocked join",
      "incomplete host inventory, failed exclusive reservation, incomplete journal, incorrect mode, libc name-mapping mismatch, or candidate identity mismatch",
      "failed exact cleanup, residual candidate, or final inventory mismatch",
      "contradictory observation/assertion or unavailable required field",
      "any NOT_RUN/INCONCLUSIVE acceptance assertion",
      "missing/unverified dedicated UID launcher or lease before any IPC creation",
      "lease loss or second process entering the dedicated UID domain",
      "sem_open failure path attempts sem_unlink in diagnostic build",
      "raw /proc/sysvipc bytes absent, non-canonical, over-cap, digest-mismatched or parser-mismatched",
      "reservation journal missing/partial/reordered/duplicated/unfsynced or crash between cleanup intent and outcome",
      "returned sem_t* mapping missing/mismatched with exact libc/dev-shm identity",
      "missing/unverified dedicated UID launcher/lease before IPC creation",
      "lease loss or second process enters execution identity",
      "diagnostic sem_open failure path calls sem_unlink",
      "raw /proc/sysvipc bytes missing/noncanonical/over-cap/hash- or parse-mismatched",
      "journal event absent/partial/reordered/duplicate/unfsynced",
      "crash after cleanup intent without durable outcome",
      "returned sem_t mapping absent or mismatched",
      "missing/invalid supervisor-termination.json, PID-set mismatch, duplicate/missing wait receipt, undecodable wait status, open stdout/stderr log",
      "abnormal_evidence_only attempts IPC inspection, cleanup, journal repair/close, claims PASS/FAIL or claims unavailable inventory is empty",
      "normal_cleanup without exactly one durable RUN_SHUTDOWN_COMPLETE before any cleanup intent",
      "missing/invalid supervisor receipt, PID-set mismatch, duplicate/missing wait receipt, undecodable raw wait status, or open logs",
      "normal_cleanup without exactly one durable RUN_SHUTDOWN_COMPLETE before cleanup",
      "abnormal mode attempts IPC/cleanup/journal mutation, claims PASS/FAIL, or reports unavailable inventories as empty",
      "both modes require continuous lease and stable result-directory identity through final record; lease loss yields pending BLOCKED/INCONCLUSIVE",
      "abnormal evidence-only may preserve malformed but safely readable non-control files without schema/replay; normal strict completeness rules do not gate this path",
      "cleanup_outcome is evidence-derived: not_started only with a complete no-intent/no-CREATED proof; partial for a proven failed attempt; complete only with one REMOVED outcome for every successful CREATED resource and no cleanup failure; otherwise unknown, with no retry"
    ],
    "local_fixture_coverage": {
      "invalid_count": 14,
      "invalid_examples": [
        "PASS assertion NOT_RUN",
        "candidate key/name collision before reservation",
        "partial exclusive reservation",
        "incomplete reservation journal",
        "mismatched candidate resource identity",
        "residual candidate after cleanup",
        "unexpected non-candidate change",
        "final inventory differs from pre",
        "incomplete final inventory",
        "unmatched Q1",
        "nonzero PASS exit",
        "missing required artifact",
        "FAIL without false assertion",
        "pending with failure claim",
        "missing dedicated UID/launcher lease",
        "second process enters execution UID",
        "diagnostic sem_open failure calls sem_unlink",
        "raw sysvipc bytes/digest/parsed summary mismatch",
        "partial or reordered reservation journal",
        "crash after cleanup intent before durable outcome",
        "returned sem_t pointer maps to wrong dev-shm inode",
        "missing dedicated UID launcher/lease",
        "second process enters exclusive run identity",
        "sem_open failure path issues sem_unlink",
        "raw sysvipc digest/parsed summary mismatch",
        "missing/partial/reordered/unfsynced journal event",
        "cleanup intent without outcome after crash",
        "returned sem_t pointer maps to wrong inode",
        "missing supervisor wait receipt for a recorded process PID",
        "supervisor receipt PID set differs from execution_identity.process_tree_pids",
        "stdout/stderr not closed before finalizer",
        "normal cleanup without durable RUN_SHUTDOWN_COMPLETE",
        "abnormal evidence-only mode attempts IPC_RMID/sem_unlink or journal repair",
        "abnormal final record claims PASS/FAIL or empty post/final inventory",
        "actually absent required evidence artifact omitted from failure.missing_artifacts",
        "optional harness-diagnostics.txt listed in failure.missing_artifacts",
        "missing supervisor-termination.json",
        "waitpid receipt PID mismatch/duplicate/missing",
        "closed-log hash mismatch",
        "normal cleanup without RUN_SHUTDOWN_COMPLETE",
        "abnormal mode performs cleanup or claims PASS/FAIL",
        "lease loss blocks finalizer entry/re-entry and leaves pending evidence",
        "malformed but safely readable non-control artifact is raw-preserved in abnormal mode",
        "missing/unavailable evidence sets overlap or misclassify a present file",
        "cleanup intent followed by failure reported as not_started",
        "cleanup retry after ambiguous durable intent/outcome",
        "empty, unknown, malformed or reordered journal classified as not_started, partial or complete"
      ],
      "status": "historical v1.8 local result; detailed standalone fixtures were not retained in the current worktree",
      "valid_count": 4
    }
  },
  "local_validation_this_freeze": {
    "abnormal_partial_evidence_fixture": "PASS: INCONCLUSIVE record accepted with present raw post/final artifacts and a separate unavailable-artifact name; no non-control parsing required",
    "astra_submitted": false,
    "cleanup_history_outcome_fixtures": "PASS: a bounded offline classifier accepted only schema-valid, ordered, complete journal streams; not_started requires no-intent/no-CREATED proof; failed intent=partial; complete requires exactly one REMOVED outcome for every successful CREATED resource; empty/unknown/malformed/reordered/truncated or created-without-any-cleanup streams=unknown",
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only. Continuous lease and stable result-directory authority remain required for either finalization mode; if they fail, preserve pending evidence. Launcher is NOT_PROVISIONED; D2 blocked until separate gates.",
    "ipc_or_vm_activity": false,
    "json_schema_meta_validation": "PASS: five Draft 2020-12 JSON Schemas meta-validated; collection_responsibility is manifest policy data, not a schema",
    "lease_loss_policy_static_check": "PASS: the manifest contract says common prerequisite failure blocks finalizer/re-entry and leaves pending evidence; no lease-loss runtime/finalizer fixture was executed",
    "manifest_generation": "PASS: r12 generated from immutable submitted r11 manifest; r11 packet/response/raw/receipt preserved",
    "missing_unavailable_set_fixtures": "PASS: exact absent set, exact unavailable set and disjointness checked; optional diagnostics rejected from missing_artifacts",
    "owner_exact_sha_consent_for_r12": false,
    "runtime_finalizer_execution": false,
    "spec_sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "supervisor_pid_set_cross_artifact_validation": "NOT EXECUTED: semantic finalizer absent; not claimed by schema tests"
  },
  "sandbox.finalizer_ipc_boundary": {
    "abnormal_evidence_only": {
      "allowed_file_operations": [
        "read pending record and supervisor receipt for strict validation",
        "enumerate allowlisted evidence names without following links",
        "hash safely readable closed raw file bytes within per-file and aggregate read caps",
        "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
      ],
      "cleanup_outcome_classifier": {
        "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
        "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
        "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
        "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
      },
      "forbidden": [
        "read /proc/sysvipc or /dev/shm",
        "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
        "append/repair/close reservation journal",
        "strict-parse/replay non-control evidence schemas or require complete normal bundle",
        "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
      ],
      "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
      "required": [
        "trusted dedicated non-login UID launcher and execution identity verified",
        "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
        "every recorded PID reaped and stdout/stderr closed",
        "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
        "continuous dedicated-UID lease held through final-record closure",
        "pending run record strictly valid",
        "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
      ],
      "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
      "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
    },
    "allowed_operations": [
      "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
      "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
      "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
      "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
      "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
    ],
    "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
    "forbidden_operations": [
      "construct Process/GW",
      "create/open candidate IPC resources or attach/use them for message traffic",
      "invoke Gateway production IPC/send/receive paths",
      "delete an object not exactly bound to its successful journal CREATE receipt",
      "retry a removal whose durable outcome is ambiguous",
      "broad/speculative host cleanup"
    ],
    "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
    "normal_cleanup": {
      "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
      "required": [
        "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
        "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
        "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
        "stdout/stderr closed and present-file hashes match",
        "continuous dedicated-UID lease"
      ]
    },
    "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
    "preconditions": {
      "abnormal_evidence_only": [
        "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
        "pending run record and supervisor receipt strictly validate",
        "no requirement for normal joins/destruction or complete evidence bundle"
      ],
      "both_modes": [
        "trusted dedicated non-login UID launcher and execution identity verified",
        "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
        "every recorded PID reaped and stdout/stderr closed",
        "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
        "continuous dedicated-UID lease held through final-record closure"
      ],
      "normal_cleanup": [
        "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
        "no timeout/signal/ambiguous normal wait",
        "complete strict normal evidence bundle and inventory/journal validation"
      ]
    }
  },
  "semantic_finalizer_contract.finalizer_ipc_boundary": {
    "abnormal_evidence_only": {
      "allowed_file_operations": [
        "read pending record and supervisor receipt for strict validation",
        "enumerate allowlisted evidence names without following links",
        "hash safely readable closed raw file bytes within per-file and aggregate read caps",
        "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
      ],
      "cleanup_outcome_classifier": {
        "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
        "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
        "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
        "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
      },
      "forbidden": [
        "read /proc/sysvipc or /dev/shm",
        "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
        "append/repair/close reservation journal",
        "strict-parse/replay non-control evidence schemas or require complete normal bundle",
        "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
      ],
      "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
      "required": [
        "trusted dedicated non-login UID launcher and execution identity verified",
        "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
        "every recorded PID reaped and stdout/stderr closed",
        "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
        "continuous dedicated-UID lease held through final-record closure",
        "pending run record strictly valid",
        "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
      ],
      "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
      "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
    },
    "allowed_operations": [
      "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
      "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
      "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
      "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
      "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
    ],
    "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
    "forbidden_operations": [
      "construct Process/GW",
      "create/open candidate IPC resources or attach/use them for message traffic",
      "invoke Gateway production IPC/send/receive paths",
      "delete an object not exactly bound to its successful journal CREATE receipt",
      "retry a removal whose durable outcome is ambiguous",
      "broad/speculative host cleanup"
    ],
    "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
    "normal_cleanup": {
      "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
      "required": [
        "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
        "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
        "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
        "stdout/stderr closed and present-file hashes match",
        "continuous dedicated-UID lease"
      ]
    },
    "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
    "preconditions": {
      "abnormal_evidence_only": [
        "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
        "pending run record and supervisor receipt strictly validate",
        "no requirement for normal joins/destruction or complete evidence bundle"
      ],
      "both_modes": [
        "trusted dedicated non-login UID launcher and execution identity verified",
        "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
        "every recorded PID reaped and stdout/stderr closed",
        "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
        "continuous dedicated-UID lease held through final-record closure"
      ],
      "normal_cleanup": [
        "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
        "no timeout/signal/ambiguous normal wait",
        "complete strict normal evidence bundle and inventory/journal validation"
      ]
    }
  },
  "semantic_finalizer_contract.normal_cleanup_failure": "After any durable CLEANUP_INTENT, stop subsequent destructive calls and retries on failure. While common authority remains valid, write a final INCONCLUSIVE record with cleanup_outcome=partial/complete/unknown as supported by durable journal evidence and final-inventory state. If lease/file authority is lost or safe final-record writing fails, leave pending recovery evidence; do not re-enter abnormal finalization to claim not_started."
}
```

===== ACTIVE TASK / DASHBOARD EXTRACTS =====
===== NG-039 frontmatter/current state =====
id: NG-039
titulo: Profissionalização de performance da library Common
area: NovaGenesis
tipo: tarefa
status: bloqueada
prioridade: alta
versao: 1
inicio: 2026-09-11
prazo:
fim:
responsavel: Antonio Alberti
stakeholders: manutenção NovaGenesis; GPT-6 Astra
parent:
depends_on:
  - SPEC-039-common-performance-measurement.md
blocked_by: "Por decisão do proprietário, antes de alterar a SPEC após Astra r12 AMEND, fazer uma revisão metodológica do ciclo r06–r12 e obter um plano de prevenção. Preparar packet plan-only, apresentar SHA final e aguardar consentimento exact-SHA para no máximo uma chamada Astra. Não editar SPEC/manifest nem fazer outro follow-up antes dessa decisão. D2 continua dependente de launcher/lease e autorização operacional; D3 depende de D2 PASS e autorização própria."
tags:
  - tarefa
  - novagenesis
  - performance
  - common
  - spec-driven

## 5. Estado actual
Última actualização: 2026-10-13

- **Estado vigente:** `bloqueada` para D2/D3. Astra r12 retornou `AMEND`, resposta normalizada `Specs/RESULTS-SPEC-039/astra-review-v1.14-r12-20261013.md` SHA-256 `6f7228f955475d5fbad6cc2c3508c960c86e264f49c1037f82f4096b769d4e97`; raw SSE `b6a6e0ded8ff307bc43a13177832436087f309e16c8ee47c2215c6fdf93caf19`; recibo `1c7b5f6c15629087731b2cecc762665523f6983a4eb550e6ad3fc17f70cffa82`. O review confirmou o fecho documental de r11 B1 e apontou dois blockers r12: (B1) reproduzi in-memory o journal schema-válido `JOURNAL_OPENED, CREATE_INTENT(A), RUN_SHUTDOWN_COMPLETE, REMOVED(A), JOURNAL_CLOSED`, classificado erroneamente `not_started` em vez de `unknown`; (B2) §7.1 limita a `partial/unknown` o outcome quando normal finalização falha após intent, mas `semantic_finalizer_contract.normal_cleanup_failure` do manifesto permite também `complete`. Os checks r12 anteriores continuam PASS para os casos cobertos, mas não cobrem o novo caso órfão. As hashes do packet/spec/manifest permanecem as acima; nenhum desses artefactos foi alterado após a revisão. PID cross-artifact segue `NOT EXECUTED`; launcher/lease não provisionado; nenhum código, build, runtime, IPC, VM, commit ou push foi feito.
- **Feito — busca de artefactos:** em `workspace/novagenesis` e `CodeRepository` foram examinados 12.116 ficheiros; os ficheiros originais D1-H `ng039_gw_q0_q1_harness` e `build-output.txt`/log não foram encontrados por nome ou pelos SHA-256 históricos. Os pacotes antigos só preservam os hashes, não os bytes.
- **Feito — D1-H recomposto:** build isolado, compile/link apenas, a partir do checkout base `1af604dad66148618cc0e578c28adb4a9829e607`; CMake 3.28.3 e GNU C++ 13.3.0; configure/build exit 0. Binário novo SHA-256 `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`; não foi executado. Recibo `Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json` SHA-256 `3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28`. Os outputs ficam em `workspace/ng039-d1h-rebuild-20261010/`, fora de `.hermes`; este build novo não recupera nem revalida os hashes antigos.
- **Feito — revisão r06:** Astra retornou `AMEND` no packet `cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711`, response `resp_0e96e100afdf6c5d016aca6a11615c81919e900a168d341cf9`; consentimento daquela chamada consumido.
- **R07 submetido sob autorização exact-SHA:** a única revisão documental Astra autorizada examinou o packet `f6fa7f178d433c6b1ccdf86bb2a915ae4acf1c567ae8b428ad351c524681cb15`, SPEC `7129…b1ff2a`, manifest `e193…64cfc9c5`, recibo `92ac…f70e53c` e HEAD `756f…470c`. A resposta foi **AMEND**, arquivada em `Specs/RESULTS-SPEC-039/astra-review-r07-20261010.md` SHA-256 `9aebba801166f875b9c6e030923450a4e28122852fb8d4e0cc13fe7a198088ab`; raw SSE SHA-256 `adf9bca11b98f5188818ce013b03a9c778f406f5b9996a0fffc6a1a29d2725fa`; recibo de submissão `astra-review-r07-20261010-submission.json` SHA-256 `0c10f65df5b8fa0e28332204e76488f8121477e9931fe116be38c44f7dd012a2`. O r07 exacto está agora histórico; não é reutilizável para nova submissão.
- **B1/B2/B3 corrigidos in-place:** B1 — remoção do `--` não consumido; B2 — rejeição de `-include...`/`-imacros...` unidos; B3 — manifesto e schema `run_record` agora ligam ao SPEC final `f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26`. SPEC SHA `f774…630f26`; manifest `1a90…a6a70d`; packet `ffc7…e8f46af`; HEAD `756f…470c`.
- **Validação sintética B1/B2/B3:** comandos literais passaram em Python normal e `PYTHONOPTIMIZE=1`: compile 2 positivos + 15 negativos por modo; link 1 positivo com controlo de stdin + 8 negativos por modo; dois negativos de argumento ausente por modo. Total 56 checks, todos conforme esperado. Recibo `r07-synthetic-validation-20261010.json`, SHA `6cc8dccffe7208d651c83d28473e7308ae58b2e55c9e4d48e9645c07fa86355a`. Schema/finalizer não ampliados; finalizer não implementado/executado.
- **Feito — revisão Astra B1/B2/B3:** verdict `APPROVE — documentary closure only`; transcript e raw SSE preservados em `Specs/RESULTS-SPEC-039/astra-session-r07-b1b2b3-20261011/`; resposta legível e recibo de submissão em `Specs/RESULTS-SPEC-039/astra-review-r07-b1b2b3-20261011.{md,submission.json}`. O packet foi confirmado byte-idêntico ao conteúdo submetido. Astra não reproduziu os testes sintéticos nem executou ferramentas.
- **Feito — emenda SPEC-039 v1.11/r08 (mesma SPEC):** substituído o sandbox indisponível por inventário host-controlled, reservas exclusivas `0600`, journal, verificação de identidade e cleanup exacto; sem exceção AppArmor nem limpeza ampla. Hashes congelados: SPEC `bc735702fff9adde69a00705715e4025cedca5f3e1314eb7200da29712822992`; manifest `d0518b500343fe80fef834b4b9a3f819396224e8da9ec601690100749d819301`; packet `3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272`, submetido à Astra; verdict `AMEND`.
- **Feito — preflight D2:** o proprietário autorizou o slice D2 exacto. A worktree isolada permanece detached em `1af604dad66148618cc0e578c28adb4a9829e607`; contém apenas o diff D1-H preexistente em `CMakeLists.txt` e os dois ficheiros D1-H não rastreados. Os dois caminhos D2 permanecem ausentes, sem código novo criado.
- **Próximo passo:** preparar revisão metodológica plan-only para Astra usando os AMENDs r06–r12, o counterexample reproduzido e a divergência SPEC/manifest; congelar packet próprio e pedir consentimento exact-SHA. Até o plano ser revisto pelo proprietário, não preparar r13 nem alterar a SPEC/manifest. O consentimento r12 está consumido. D2/D3 permanecem em gates separados.
- **Risco confirmado por Astra e revisões independentes:** `GW::ReturnIPCSHMID` chama `sem_unlink(name)` quando `sem_open` falha, sem comparar identidade; a verificação seguida de unlink por nome também mantém uma janela TOCTOU. Qualquer mismatch deve bloquear PASS, mas `INCONCLUSIVE` posterior não reverte uma remoção indevida.
- **Bloqueio de namespace resolvido:** o requisito `unshare` foi removido; não se altera AppArmor. A revisão documental ocorreu, mas r08 retornou `AMEND`; D2/D3 permanecem bloqueados.
- **Astra r08 — veredicto documental final: AMEND (B1 withdrawn; B2/B3 blocking):** resposta inicial `astra-review-r08-host-controlled-20261012.md` SHA-256 `32ab51a41285516ec725462e14bd9c9bec11ffc215b113bc588c2cd1c13d5a85`; follow-up com correção de identidade e três revisões independentes `astra-review-r08-followup-agent-reconciliation-20261012.md` SHA-256 `6ff9bd65cab5cadb535aeb428781ede9b17ab357a6cdf6121e358af70630ff7b`; raw SSE do follow-up SHA-256 `fa3348e5a680811772bd5c472cc24feff55c2b080a09f8b6e50e451475ea084d`; submission receipt SHA-256 `9ee6196d6e76f7e793801809b2a8177b3c4750fa195d7d692624c550ce26db90`. Astra confirmou que B1 era typo do nosso envelope, retirou-o e manteve B2 (exclusividade/TOCTOU e unlink sem identidade) e B3 (contrato insuficiente para revalidar journal, inventário bruto e mapping libc). A chamada foi limitada ao documento; sem autorização operacional.
- **Astra r10 — `AMEND` documental:** response `Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.md` SHA-256 `6aa0669c3cec1b11c2516d6c361d78460663cde9f097cc4c5de8264557e64c2e`; raw SSE `astra-review-v1.13-r10-20261012.raw.sse` SHA-256 `4c35170e7d37953daf9bbc7082e0e92cd73b6b45067521a151c3e7654d6f2704`; submission receipt SHA-256 `046afc37a36e7441ee341d6c663f3718fa0bf8ca76717cc98791f58f95373ae8`. Exact r10 packet and bound identities were verified against the Session transcript. Astra confirms B1 IPC scope resolved and B2/B3 intact; new B1 is failure finalization: `_Exit`/blocked join requires an `INCONCLUSIVE` final record, but the manifest finalizer requires successful joins and `Process/GW` destruction. The one-call consent is consumed; no operational authority. Usage recorded: 15,645 input / 864 output tokens; 5.20725 estimated credits (not USD).
- **Pacote r09 submetido à Astra e devolvido `AMEND`:** SPEC `Specs/SPEC-039-common-performance-measurement.md` SHA-256 `a2feea01cb80b60256da3a7abd5e8c2196954d6e008d6f488cd4fb37f2058af1`; manifesto `Specs/RESULTS-SPEC-039/precode-manifest-v1.12-r09.json` SHA-256 `613c5843f503ff75f704aef6fd9c7b68fb994029d5f822691223212bfbf7bc9a`; packet `Specs/RESULTS-SPEC-039/precode-packet-v1.12-amend-closure-20261012-sanitized-draft-r09.md` SHA-256 `560ee5654d9a8859e3e21aa916a1dca9f0e564fe1f8eac41fb4ee8f4bd3397a6` (96.321 bytes). Exact packet embedded and verified. Astra response identifies one B1 contradiction about finalizer IPC permission; r09 is now historical and cannot be resubmitted without a new packet and fresh consent.
- **Pacote v1.13/r10 submetido e devolvido `AMEND`:** SPEC SHA-256 `b4e1c0c2db88a0d1a8f02ab98c934b7b33ec23fd2162c8e1b2921b72afbaf88d`; manifest `Specs/RESULTS-SPEC-039/precode-manifest-v1.13-r10.json` SHA-256 `5707467a0c4e41d254dfe64c30b5d34b74da165bb755f9d8d792f209fda28a13`; packet `Specs/RESULTS-SPEC-039/precode-packet-v1.13-amend-closure-20261012-sanitized-draft-r10.md` SHA-256 `55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146` (64.727 bytes). O packet exacto foi confirmado byte a byte no Session. Astra fechou a contradição IPC anterior, mas encontrou conflito de finalização anormal (ver r10 review); r10 é imutável e não pode ser reutilizado como próximo round.
- **Pacote v1.14/r11 submetido à Astra; verdict `AMEND`, documental:** HEAD `756f493b7049434ab67cf2382486a48f606a470c`; SPEC SHA-256 `bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609`; manifest `c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b`; packet `feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829`. Resposta `astra-review-v1.14-r11-20261012.md` SHA-256 `9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7`; raw SSE `5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb`; submission receipt `4fa8e4828ea27ffd94185ff00d3a5d3364bdf152090b4d093569863991c6aafd`. Blockers: (B1) lease loss contradizia a elegibilidade abnormal; (B2) cleanup após intent podia ser rotulado falsamente `not_started`. Corrigidos na mesma SPEC v1.14/r12; advisory adicional: full-bundle/schema checks não podem bloquear a preservação raw abnormal. Uso real da chamada: 16.425 input / 1.082 output tokens, 5,72925 créditos estimados (não USD), 52% janela primária / 45% semanal no retorno. A chamada única foi autorizada pelo owner message `1559215860005081219`; consentimento consumido, sem autoridade operacional.
- **Validação local r11:** 5/5 JSON Schemas Draft 2020-12 meta-válidos (o sexto campo `collection_responsibility` é política do manifesto, não schema); fixture anormal INCONCLUSIVE aceitou `ipc-post.json`/`ipc-final.json` presentes porém não verificados sem marcá-los como ausentes; a igualdade do conjunto `missing_artifacts` com os artefactos realmente ausentes e a detecção de omissão passaram. Rejeitados PASS/cleanup anormal, inclusão de `harness-diagnostics.txt` em `missing_artifacts` e recibo wait vazio. Consistência PID entre artefactos permanece contrato semântico, não executada. Verificador read-only reproduzível `CodeRepository/infra-novagenesis-tools/verify_spec039_v14_r11.py` SHA-256 `2a58ce2f002108c2f49af67c6785b4d3fd3a975d27d0e2f843a313e0913242af`; o recibo local `Specs/RESULTS-SPEC-039/precode-validation-v1.14-r11-20261012.json` vincula o comando e os hashes finais (SHA-256 `62e5d3c5b28176ebf156c383a49b962ec795917542e8a84997dbf5283725e76f`). Nenhum finalizer/runtime/IPC.
- **R12 same-SPEC submetido e revisto:** packet `a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd`; Astra `AMEND`. Arquivo do pedido completo, com SPEC/manifest integrais, suporte, resultados e packet exato: `Specs/RESULTS-SPEC-039/astra-session-v1.14-r12-20261013/request-user-turn.md` SHA-256 `b57962e2946373c3f929c3cac9ad297a401c72c0a465103fb40637efa9da7382`. Transcript e raw SSE foram preservados; resposta parser duplicada foi mantida intacta e uma cópia derivada remove apenas a segunda repetição idêntica. O finding B1 foi reproduzido localmente; B2 é contradição documental entre SPEC e manifesto. Nenhuma emenda foi aplicada.

- **Ferramentas locais privadas:** `CodeRepository/infra-novagenesis-tools/` (diretório `700`, scripts `600`): geradores r09–r12, verificadores read-only r11/r12 e submissões one-shot r09–r11. Nenhuma ferramenta SPEC-039 fica em `.hermes`; diretório sem Git/remote.
- **Próximo gate:** aprovação do plano metodológico antes de decidir a correção same-SPEC B1/B2 ou criar tooling/SPEC de processo. Nova consulta Astra só após consentimento exact-SHA do packet plan-only. Não há autorização para D2/D3.

===== NG-040 frontmatter/current state =====
id: NG-040
titulo: Baseline, benchmarks e telemetria da Common
area: NovaGenesis
tipo: tarefa
status: bloqueada
prioridade: alta
versao: 1
inicio: 2026-09-11
prazo:
fim:
responsavel: Antonio Alberti
stakeholders: manutenção NovaGenesis; GPT-6 Astra
parent: NG-039
depends_on:
  - SPEC-039-common-performance-measurement.md
blocked_by: "Por decisão do proprietário, revisar primeiro a metodologia de Astra/fixtures/verificação que levou aos AMENDs r06–r12. Preparar packet plan-only, apresentar SHA final e aguardar consentimento exact-SHA antes de uma chamada. Não alterar SPEC/manifest nem iniciar D2/D3 até fechar esse gate."
tags:
  - tarefa
  - novagenesis
  - performance

## 5. Estado actual
Última actualização: 2026-10-13

- **Estado vigente:** `bloqueada` para D2/D3. Astra r12 retornou `AMEND`; resposta normalizada `Specs/RESULTS-SPEC-039/astra-review-v1.14-r12-20261013.md` SHA-256 `6f7228f955475d5fbad6cc2c3508c960c86e264f49c1037f82f4096b769d4e97`; raw SSE `b6a6e0ded8ff307bc43a13177832436087f309e16c8ee47c2215c6fdf93caf19`; recibo `1c7b5f6c15629087731b2cecc762665523f6983a4eb550e6ad3fc17f70cffa82`. Astra confirmou o fecho de r11 B1, mas identificou (B1) um REMOVED órfão, após CREATE_INTENT e sem CREATED/CLEANUP_INTENT, que o classificador retorna erroneamente como `not_started` (Hermes reproduziu-o com eventos schema-válidos); e (B2) a SPEC §7.1 restringe a `partial/unknown` um failure pós-intent, enquanto o manifesto também permite `complete`. O verificador anterior passou apenas os casos que cobria; não executou esse negativo. O PID cross-artifact segue `NOT EXECUTED`; launcher/lease ausente; sem runtime ou benchmark AIOPT5.
- **D1-H feito:** reconstrução compile/link apenas em `/home/gandalf/workspace/ng039-d1h-rebuild-20261010/`, fora de `.hermes`; configure/build exit 0, CMake 3.28.3, GNU C++ 13.3.0. Novo binário SHA-256 `060266a24546bc9a8e77ec7b0af409cb480bae770e56f65e95f2727877e5cc7d`; executável não rodado. Recibo `Specs/RESULTS-SPEC-039/d1h-rebuild-20261010.json` SHA-256 `3a71e39a80241332429c9298d687b1bbedc8078f1d6f00b37df7c6e5602e9f28`. Os hashes históricos do binário/log não foram recuperados.
- **R06:** Astra `AMEND` no packet SHA-256 `cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711`; consentimento exact-SHA daquela chamada está consumido.
- **R07 documental:** submetida sob autorização exact-SHA para uma única revisão Astra; retorno **AMEND**. Packet histórico `f6fa7f178d433c6b1ccdf86bb2a915ae4acf1c567ae8b428ad351c524681cb15`, manifest `e193e8f9f6c812145b4da4ffae507bf9aa997847bae172ade2d001d164cfc9c5`, SPEC v1.10 acima. Resposta `Specs/RESULTS-SPEC-039/astra-review-r07-20261010.md` SHA-256 `9aebba801166f875b9c6e030923450a4e28122852fb8d4e0cc13fe7a198088ab`; raw SSE SHA-256 `adf9bca11b98f5188818ce013b03a9c778f406f5b9996a0fffc6a1a29d2725fa`.
- **Validação local histórica pré-B1/B2:** schemas 3/3 meta-válidos; compile checker 2 perfis positivos e 13 negativos por modo; link checker 1 positivo, 1 controlo de stdin e 8 negativos por modo; normal e `PYTHONOPTIMIZE=1`. O recibo desse snapshot era `92ac…`. A validação actual B1/B2 e o recibo corrente constam abaixo. O finalizer cross-artifact não foi implementado/executado e não há resultado correctness Q0/Q1.
- **B1/B2/B3 corrigidos e validados in-place:** B1 (argv sem `--`), B2 (`-include/-imacros` unidos), B3 (identidade SPEC no manifesto/schema). Snapshot final: SPEC `f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26`, manifest `1a90346598f5799565bf1f39610ccb45f43afdb8c7acba08d82616e747a6a70d`, packet `ffc71bf48dd0f2c2bea7a3448923a9e11ba5d3a16ebbea19ff81a9725e8f46af`, receipt `6cc8dccffe7208d651c83d28473e7308ae58b2e55c9e4d48e9645c07fa86355a`, HEAD `756f493b7049434ab67cf2382486a48f606a470c`. Validação sintética: 56 checks; advisory `failure.phase` adiado.
- **Feito — revisão Astra B1/B2/B3:** verdict `APPROVE — documentary closure only`; o packet foi confirmado byte-idêntico ao submit. A aprovação não prova a execução dos 56 checks sintéticos, nem implementa/executa finalizer ou D2.
- **Feito — emenda v1.11/r08:** perfil host-controlled com inventário pré/pós/final, reserva exclusiva `0600`, journal durável, libc/name mapping registado e cleanup exacto por identidade; sem exceção AppArmor nem cleanup amplo. Hashes: SPEC `bc735702fff9adde69a00705715e4025cedca5f3e1314eb7200da29712822992`; manifest `d0518b500343fe80fef834b4b9a3f819396224e8da9ec601690100749d819301`; packet `3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272`. Astra r08 inicial retornou `AMEND`; follow-up retirou B1 como erro do envelope e confirmou B2/B3. Respostas, raw SSE e submission receipts arquivados em NG-039. Três revisões independentes corroboraram B2/B3.
- **Feito — preflight D2:** a autorização do proprietário foi recebida, mas o sandbox obrigatório falhou antes de qualquer escrita. A worktree baseline `1af604d…` foi preservada; não há ficheiros D2 novos.
- **Próximo passo:** contribuir para a revisão metodológica plan-only r06–r12 e aguardar a decisão do proprietário sobre o plano. Depois, decidir a correção same-SPEC e eventual tooling; qualquer Astra exige novo packet exact-SHA e consentimento. D2/D3 seguem bloqueados por gates próprios.
- **B2/B3 e bloqueio operacional:** Astra r10 confirmou que as salvaguardas documentais de r08 B2/B3 continuam intactas. Launcher dedicado/lease ainda não provisionado; nenhum D2 configure/build/runtime/IPC ou D3 ocorreu nesta etapa, nem foi autorizado.
- **R12 same-SPEC submetido e revisto:** Astra `AMEND`; as duas contradições e hashes da resposta estão no estado vigente. O pedido exato e o transcript estão em `Specs/RESULTS-SPEC-039/astra-session-v1.14-r12-20261013/`; nenhuma emenda à SPEC/manifest/packet foi feita.

===== Dashboard active AIOPT5 focus =====
## 🎯 Foco activo — NovaGenesis / branch AIOPT5

Entre as notas `NG-0xx`, o âmbito activo AIOPT5 fica limitado às tarefas abaixo; o status indica a etapa do ciclo, não que todas estejam em execução neste momento:

- [[NG-039-performance-common-v1-20260911]] — `bloqueada`: Astra r12 `AMEND` (classificador aceita REMOVED órfão como `not_started`; outcome normal diverge entre SPEC/manifest). Por decisão do proprietário, gate atual é revisão metodológica r06–r12; preparar packet plan-only e aguardar consentimento exact-SHA. Sem r13, D2 ou D3 por enquanto.
- [[NG-040-baseline-common-performance-v1-20260911]] — `bloqueada`: mesmo gate de revisão metodológica; launcher/lease ausente e nenhum teste D2/benchmark AIOPT5 executado.
- [[NG-061-mapear-commits-aiopt5-v1-20260926]] — `em-revisao`: 143 evidências raw arquivadas/retiradas do worktree; diagramas Hermes movidos para `/home/gandalf/Temp`; restam 265 duplicados root-owned.
- [[NG-064-ng-elc-aiopt5-compatibility-v3-20260927]] — `bloqueada` para runtime; revisão documental SPEC-062 v1.1 concluída por agora. Falha reproduzida: launch ambíguo pode resultar em `teardown_status=PASS` sem handle/stop; o proprietário decidiu não autorizar correção nesta etapa.

**Fora do foco de execução, embora relacionados com AIOPT5:** [[NG-062-retomar-spec027-recepcao-raw-multiworker-v6-20260926]] permanece `bloqueada`/suspensa. [[NG-064-ng-elc-aiopt5-compatibility-v3-20260927]] permanece `bloqueada` para código/execução, embora a revisão documental de SPEC-062 tenha sido retomada por pedido do proprietário. [[NG-043-gw-deadline-wait-v1-20260911]] fica diferida: a SPEC-041 está arquivada em AIOPT3 e só pode ser reconsiderada depois do gate GW e de decisão própria.

O estado de ciclo das notas permanece intacto. As tarefas NG abertas fora do foco abaixo não estão concluídas nem canceladas. NG-019 mantém-se como nota histórica sem status normalizado. Não retomar trabalho fora do foco sem nova decisão de prioridade.

===== NG-039 early-round summary from task note =====
### Histórico detalhado — checkpoint de 2026-10-05 (superseded)

Este checkpoint supersede o próximo passo do registo de 2026-10-04.
- **Feito:** v1.4 packet `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948` recebeu **AMEND** na única revisão autorizada. Resposta ID `resp_0a12ead61fee5339016ac370498edc87d28ec6212dafa33cf9`; raw SSE SHA-256 `25b8c29265d327c34323e12f2c35177c2069c13b1d7e99f83a42ffea75240a58`; parsed-response SHA-256 `b374fc24de401f084a22e9e3f7892689ed167cf54dec25715aa999522b7e4f0c`; archive manifest SHA-256 `46f41c9af22a6b1c9e86e2a66682dfd83f7323fb25d1c47aded2aa52c95e6782`. Uso Codex: 17,496 input, 2,301 output, 19,797 total; 7.8255 créditos estimados (não USD); quota no retorno 5h 20%, semanal 69%. SPEC-039 v1.5 local SHA-256 `cf6efdf1dc1bcc7179106e109d3143a5cbe89e8c93cb7725aa34498b5407b6cc`; inclui contrato documental de run-record/status/evidência. Nenhum código, build, execução, IPC/VM ou Astra follow-up.
- **Feito:** packet r03 `7121fa9307beb790ff093319d6d9bc54fb9d296bf68d8c705647fbcebf740797` foi submetido uma vez à GPT-6 Astra/Codex e retornou **AMEND** (response ID `resp_020e2d49d7eb0fb1016ac39985744887d28b258e11ecf96233`). Raw SSE SHA-256 `14f95d55a4a5611f518a0e236c98edd255e6809acd256b105177f975577bef09`; parsed-response SHA-256 `6fae28a46fd385e0d5aa87e8a1bc7149c832fcd8878170b470dab558941bc587`; archive manifest SHA-256 `b26c7767c9a74f2c9ee716a59257f79064c5480191addbc5f61da998989ca5fd`. Uso: 18,905 input / 2,568 output / 21,473 total; 8.57825 créditos estimados (não USD); quota no retorno 5h 8%, semanal 79%. Nenhum código, build, execução, IPC/VM ou D3.
- **Feito:** r04 packet `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829` recebeu Astra **AMEND** (response `resp_0d91609fd866592e016ac3b3b903e487d2891f1ae2407fb075`); findings 3 e 4b–d fechados só a nível de desenho; finding 4a permaneceu aberto. Raw/parsed/archive SHA-256 e uso estão preservados no histórico abaixo.
- **Feito:** r05 packet `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a` recebeu Astra **AMEND** (response `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). Raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`; parsed-response SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`; archive manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a`. Uso 22,569 input / 1,650 output / 24,219 total; 8.11725 créditos estimados (não USD); quota no retorno 5h 51%, semanal 86%. 4a continua must-fix: `-Wp,`/`-Xpreprocessor` e macro function-like escapam ao scan; `-ansi` contorna o check de dialect; `assert` do checker de link desaparece com `PYTHONOPTIMIZE=1`. Hardening opcional: target pelo argumento real `-o`, conflito `-Bdynamic` e schema `allOf` para assertions/cleanup/inventários. Nenhum código, configure/build, run, IPC/VM ou D3.
- **Feito:** SPEC-039 v1.8 SHA-256 `41a635e28fdac973668412030f705d7975fa77caebdb00960267cd1f84150db5` atualiza os verificadores e schema na mesma SPEC. Validação sintética local: 3 schemas meta-válidos; 4 registros válidos/14 inválidos; compile checker 2 perfis positivos/26 negativos; link checker 15 execuções, 3 positivas/12 negativas em Python normal, `-O` e `PYTHONOPTIMIZE=1`. Não houve `compile_commands.json` real, CMake configure/build, execução de harness, IPC/VM ou medição.
- **Falta:** persistir o manifest v1.8 e montar/validar o packet r06 sanitizado; re-hashar SPEC/tarefas/Dashboard e obter consentimento novo vinculado ao SHA exato antes de Astra.
- **Próximo passo:** preparar r06 como revisão documental somente; apresentar o SHA e aguardar autorização. Sem D2.
- **Bloqueio:** r05 AMEND; r06 ainda não foi persistido nem revisto e não tem consentimento exact-SHA. D2 code/build/run, IPC/VM, D3 e Q2–Q4 permanecem bloqueados/diferidos.

===== CURRENT R12 REVIEW PACKET — exact frozen bytes =====
SHA-256 `a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd`. This is the primary document under discussion; included as history/context, not being re-reviewed for approval in this plan request.


===== FILE: current r12 packet, frozen primary evidence =====
# SPEC-039 v1.14 — r12 response to Astra r11 AMEND

Status: FROZEN CANDIDATE, UNSUBMITTED. Same-SPEC documentary amendment only. Fresh owner consent is required for exactly one review bound to the final r12 packet SHA. No code, build, finalizer, runtime, IPC, VM/SSH, D3, cleanup, commit, push or release authority.

## Frozen identities
- AIOPT5 document HEAD: `756f493b7049434ab67cf2382486a48f606a470c`; tree: `9adf665c0eb06213a29a6c12d650451948c7a4ab`.
- Functional source baseline: `1af604dad66148618cc0e578c28adb4a9829e607`; tree: `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`.
- SPEC-039 v1.14 SHA-256: `0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115`.
- Full manifest v1.14/r12 SHA-256: `63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917`.
- Prior immutable r11 packet SHA-256: `feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829`; prior SPEC/manifest: `bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609 / `c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b`.
- Prior r11 response SHA-256: `9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7`; raw SSE and submission receipt hashes: `5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb / `4fa8e4828ea27ffd94185ff00d3a5d3364bdf152090b4d093569863991c6aafd`.
- Functional baseline source hashes: `{"Common/src/GW.cpp": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e", "Common/src/Process.cpp": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1"}`.
- Working-tree status (context only):
```
M Specs/SPEC-039-common-performance-measurement.md
 M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md
?? Specs/RESULTS-SPEC-039/
?? Specs/RESULTS-SPEC-059/
```

## Exact r11 verdict and bounded closure request
The owner authorized exactly one documentary review of r11; that consent is consumed. Astra returned AMEND. B1: mode selection allowed abnormal finalization after normal eligibility failed, but both-mode preconditions still required continuous lease. B2: abnormal schema forced `cleanup_outcome=not_started` even after a cleanup intent/removal may have occurred. Optional advice: normal strict schema/completeness parsing must not block bounded abnormal evidence preservation. The full r11 response is embedded verbatim below.

Review whether r12 closes these two blockers and the bounded evidence-preservation advice while preserving r08 B2/B3. Specifically assess: (1) lease/supervisor/result-directory authority is a common prerequisite; lease or identity loss blocks finalizer entry/re-entry and leaves pending evidence; (2) normal strict full-bundle checking is separated from abnormal raw preservation; malformed but safely readable non-control artifacts can be preserved and do not block INCONCLUSIVE; (3) `missing_artifacts` and `unavailable_artifacts` are exact, disjoint, exclude optional diagnostics and the run record; (4) cleanup history is `not_started` only with complete journal proof of no intent, `partial` for proven failed attempt, `unknown` for missing/ambiguous history, with no retries or rewrite; (5) a normal cleanup failure remains INCONCLUSIVE with truthful outcome while authority permits, otherwise pending; and (6) schema, SPEC and manifest contain no bypass or false-pass contradiction.

Do not infer implementation or runtime readiness. The launcher remains NOT_PROVISIONED and D2 BLOCKED. Documentary review grants no operational authority. Return concise UK English: verdict APPROVE/AMEND/HOLD, must-fix blockers with exact packet locations or none, optional advice separately, and explicit no-operational-authority statement.

## Full r11 Astra response — immutable prior evidence
## Verdict: AMEND — documentary only

Performed the single review authorised by owner message **1559215860005081219**, for packet SHA-256 `feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829`.

Reviewed only the supplied text; no tools, hash recomputation, code execution or runtime validation. **The original join/destruction prerequisite conflict is resolved, but two related contradictions remain.**

## Must-fix blockers

### B1 — Continuous lease remains an unconditional prerequisite for abnormal finalisation

**Locations**
- Manifest `sandbox.finalizer_ipc_boundary.preconditions.both_modes`, repeated under `semantic_finalizer_delta.finalizer_ipc_boundary`: requires a “valid dedicated non-login UID launcher and continuous lease”.
- `sandbox.finalizer_mode_selection`: otherwise permits abnormal evidence-only finalisation after confirmed reaping/log closure when normal eligibility—including continuous lease—fails.
- §7.1 normal/abnormal sequence: an uncertain lease selects no-cleanup handling, followed by the evidence-only finalisation contract.

**Conflict:** after lease loss, confirmed termination and closed logs are insufficient to satisfy the explicit *both-modes* prerequisite. The contract therefore both offers abnormal finalisation and prohibits invoking it.

**Required amendment:** distinguish the trusted-supervisor/evidence prerequisites from continuous exclusion required for IPC inspection and cleanup. Explicitly define whether lease loss permits evidence-only finalisation, and under what file-safety conditions. If it instead prevents finalisation, consistently require pending/partial evidence and `BLOCKED/INCONCLUSIVE`, rather than promising a final record. Retain the prohibition on IPC operations and automatic recovery cleanup.

### B2 — Recovery after a cleanup attempt cannot truthfully report `not_started`

**Locations**
- §7.1 normal/abnormal sequence includes “a finalizer failure requiring recovery” among abnormal-mode cases.
- The same clause, `semantic_finalizer_delta.abnormal_evidence_only.cleanup_outcome`, and run-record v5 `abnormal_evidence_only_constraints` unconditionally require `cleanup_outcome=not_started`.

**Conflict:** a normal finalizer can fail after `CLEANUP_INTENT` or a destructive call. A subsequent evidence-only recovery record would have to misreport historical cleanup as never started, or breach its schema. Performing no cleanup **during abnormal finalisation** does not establish that no earlier cleanup occurred.

**Required amendment:** either restrict this abnormal-mode contract to runs with no prior cleanup attempt and specify a separate pending/recovery disposition, or permit an evidence-derived historical cleanup outcome for recovery. Preserve ambiguity, prohibit retries, and keep recovery `INCONCLUSIVE`.

## Bounded findings otherwise satisfied

- **Normal gating:** exactly one durable `RUN_SHUTDOWN_COMPLETE`, after joins/destruction and before cleanup intents, plus verified supervisor wait/log evidence is expressly required.
- **Supervisor binding:** the semantic contract requires exact recorded PID-set equality, unique per-PID raw wait receipts, decoded termination states and hashes for present closed logs. JSON Schema alone does not enforce these cross-artifact checks.
- **Mode selection:** expressly evidence-derived, not caller-controlled, subject to B1 above.
- **Abnormal restrictions:** only `INCONCLUSIVE`; no IPC inspection, cleanup or journal mutation. Present but unverified post/final files remain artifacts, not verified inventories.
- **Schema representation:** the shown nullable reservation fields and journal v2 event support the amendment without dummy resource values. Full observation-field compatibility cannot be independently established from omitted, digest-only schema portions. PASS/FAIL remain contractually dependent on complete normal evidence.
- **Bundle accounting:** eight required evidence artifacts, one optional diagnostic, and the separate run record correctly give a ten-file maximum. Missing-artifact semantics exclude optional diagnostics and present-but-unverified files.
- **B2/B3 safeguards from prior reviews:** retained at documentary level; no new false-PASS route is established by the supplied changes.

## Optional advice

Scope the generic strict-parsing and “eight artifacts exactly once” checks explicitly to normal eligibility: malformed or absent evidence must remain preservable in abnormal mode, not prevent its permitted `INCONCLUSIVE` record.

Journal bytes alone do not independently prove historical fsync completion. Reported local validation results are supplied claims, not validation performed in this review.

## Authority boundary

The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED**. This verdict grants **no operational authority**: no implementation, configure/build, harness/finalizer execution, runtime/IPC, namespace, VM/SSH, D3, cleanup, commit, push or release.

## Current SPEC clauses — exact text
Path `Specs/SPEC-039-common-performance-measurement.md`, full SHA pinned above.

### Proposed D2/finalizer boundary
```markdown
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No modifications to NGAL, `Process`, `PGCS` or `PG`, no algorithm change, separate source helper file, or D3 timing files are proposed. The D2 harness may construct the existing `Process/GW` under test; the same allowlisted harness binary's `--finalize` mode runs after the test exits without constructing `Process/GW`, creating IPC resources, or sending/receiving production messages. In `normal_cleanup` mode, it requires a durable `RUN_SHUTDOWN_COMPLETE` journal event proving worker joins and `Process/GW` destruction, plus a validated required `supervisor-termination.json` artifact containing one raw `waitpid` receipt per recorded process and stdout/stderr closure state/hashes; only then may it read inventories, inspect exact identities, and perform journal-authorized `IPC_RMID`/`sem_unlink` under the continuously verified lease. In `abnormal_evidence_only` mode, only while trusted launcher, execution/result-directory identities and lease remain valid and a valid supervisor receipt proves every process reaped and logs closed, the finalizer may hash raw, safely readable allowlisted evidence without requiring the complete bundle or parsing/replaying non-control artifacts. It writes `INCONCLUSIVE`; `failure.missing_artifacts` and `failure.unavailable_artifacts` are disjoint, accurate sets; cleanup history is exactly one of `not_started`, `partial`, `complete` or `unknown`, derived from valid evidence and never forced to `not_started`. It performs no IPC inspection, cleanup, journal mutation or retry. If lease, receipt, process/log proof or result-directory authority is unconfirmed, leave the pending record and report `BLOCKED/INCONCLUSIVE`. The trusted dedicated-UID launcher/lease is a provisioned external runtime prerequisite, not part of the five-file code allowlist; it does not exist here and direct host-UID execution is prohibited. |
```

### §7.1 normal and abnormal lifecycle sequence
```markdown
After Gateway/output workers stop and join and `Process/GW` are destroyed, the harness must append/fsync exactly one `RUN_SHUTDOWN_COMPLETE` event to the still-open reservation journal. This event is the positive proof that normal shutdown completed; a caller boolean or process exit is not a substitute. The trusted supervisor then waits/reaps every recorded harness process PID and closes stdout/stderr before invoking the same allowlisted `--finalize` binary. It selects `normal_cleanup` only when the journal contains exactly one valid shutdown-complete event, every recorded process has a verified wait receipt, both logs are closed, no timeout/signal/ambiguous termination occurred, and the lease remains continuously valid. In this mode the finalizer captures post inventory, validates/replays the journal, reads required inventories, inspects exact current identities (`shmctl(..., IPC_STAT, ...)` for SysV SHM and `stat`/equivalent metadata for the exact POSIX semaphore object), and removes only journal-proven run-created resources. For each resource it appends/fsyncs `CLEANUP_INTENT`, rechecks lease and identity immediately before the destructive call, invokes only exact `shmctl(exact_shmid, IPC_RMID, ...)` or `sem_unlink(exact_recorded_name)`, then appends/fsyncs the matching outcome. The `normal_cleanup` gate above is satisfied only while the trusted launcher, supervisor receipt, execution identity, result-directory identity and continuous lease remain valid through final-record closure. If normal-shutdown proof is absent while these common authority/file-safety conditions remain valid, the finalizer may choose `abnormal_evidence_only`. Loss or ambiguity of the lease, supervisor proof, execution identity or result-directory authority is not an abnormal-mode trigger: do not invoke or re-enter the finalizer; preserve the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A failure during normal cleanup after any `CLEANUP_INTENT` stops further destructive calls and retries; while authority remains valid, record `INCONCLUSIVE` with the evidence-derived cleanup outcome in the current finalization, otherwise retain pending recovery state. No ambiguous deletion is retried.

In `abnormal_evidence_only` mode, only while the trusted launcher, execution identity, result-directory identity and continuous lease remain verified, and after a valid `supervisor-termination.json` proves every recorded process was reaped and both logs were closed, the finalizer strictly validates the pending run record and supervisor receipt. It may then open only allowlisted evidence names using no-follow regular-file checks, preserving and hashing safely readable closed raw bytes within per-file caps; it does not require all eight evidence files, parse/replay their JSON or journal schemas, or enforce the normal complete-bundle check. `failure.missing_artifacts` lists required evidence names actually absent; the new `failure.unavailable_artifacts` lists required names present but unreadable/unsafe/over-cap. These sets are disjoint, exclude the run record and optional `harness-diagnostics.txt`, and neither may falsely classify a present file as missing. Malformed but safely readable evidence is preserved and hashed, not parsed or used to block an otherwise permitted `INCONCLUSIVE` record. A bounded cleanup-history classifier may recognize only well-formed journal intent/outcome records to distinguish `not_started`, `partial` or `complete`; it does not perform full schema replay, and any missing, malformed, truncated or conflicting history yields `unknown` without preventing evidence finalization. The final record sets `record_finalized=true`, `finalization_mode=abnormal_evidence_only`, and `status=INCONCLUSIVE`. `failure.cleanup_outcome` is `not_started` only if a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; it is `partial` when valid durable evidence proves at least one cleanup action failed; it is `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the run remains `INCONCLUSIVE` because abnormal mode has no final-inventory proof; and it is `unknown` when cleanup history/outcomes are absent, malformed, truncated or ambiguous. The abnormal finalizer performs no new cleanup, does not retry, and preserves journal bytes. If the lease, receipt, process/log proof or result-directory authority fails at any point, do not write/re-enter finalization; keep the pending/partial record and report `BLOCKED/INCONCLUSIVE`. A present but partial, stale, or shutdown-unverified `ipc-post.json` or `ipc-final.json` may be preserved and hashed as a file, but must not be treated as a verified inventory: the run-record post/final inventory summaries remain unavailable, with `is_empty=null` and no claimed inventory digest. The finalizer never reads `/proc/sysvipc` or `/dev/shm`, calls `IPC_STAT`, `IPC_RMID` or `sem_unlink`, appends/repairs/closes the journal, or performs cleanup during this mode.

In `normal_cleanup` mode, capture a complete final inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required runtime artifact exactly once. The finalizer strictly parses journal/inventories, recomputes event order/transitions, raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. PASS/FAIL require the complete bundle and safe normal cleanup; missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. If normal cleanup has written any `CLEANUP_INTENT` and then fails, stop subsequent destructive calls and retries; while lease and result-directory authority still hold, write an `INCONCLUSIVE` final record with `cleanup_outcome=partial` or `unknown` as supported by the journal. If authority or safe record writing is lost, leave the pending/partial evidence and report `BLOCKED/INCONCLUSIVE`; never re-enter finalization to claim `not_started`. The abnormal evidence-only path described above preserves the historical cleanup outcome and performs no further cleanup. D2 remains one controlled correctness run only; no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.
```

### §7.1 v5 run-record and finalizer contract
```markdown
5. Bounded run-record and dual-mode finalizer contract: `tests/spec039/q0_q1_d2.manifest.json` defines strict Draft 2020-12 schemas for the pending/final run record v5, `supervisor-termination.json` receipt v1, IPC inventories, observer events and `reservation-journal.jsonl` events v2. The finalized run record's `finalization_mode` is exactly `normal_cleanup` or `abnormal_evidence_only`; pending records use null, and the finalizer derives the mode from journal and supervisor evidence rather than a caller label. The same allowlisted `--finalize` binary is the sole semantic authority. Both finalization modes require a strictly valid pending run record and supervisor receipt; every recorded PID reaped; closed stdout/stderr; unchanged execution/result-directory identities; and a dedicated-UID lease continuously held through final-record closure. If any common prerequisite is absent, ambiguous or lost, do not invoke/re-enter the finalizer or write a final record; retain pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. In `normal_cleanup`, strictly parse/replay all required JSON/JSONL, verify raw `/proc/sysvipc` hashes/rows, exact artifact set, wait/log receipt, lease and semaphore mapping, then inspect exact identities and perform only journal-authorised cleanup after `CLEANUP_INTENT` fsync and immediate identity/lease recheck. `RUN_SHUTDOWN_COMPLETE` must be durable after joins/destruction and before any cleanup intent. PASS/FAIL require the full normal evidence and inventory gates; if a normal cleanup attempt fails after an intent, stop further destructive calls/retries and write `INCONCLUSIVE` with `cleanup_outcome=partial` or `unknown` as supported by durable evidence. If the final record cannot safely be written, leave pending recovery evidence.

In `abnormal_evidence_only`, normal shutdown proof may be absent, but all common prerequisites still hold. Strictly validate the pending record and supervisor receipt; for other allowlisted evidence files, use no-follow regular-file checks, preserve/hash safely readable closed raw bytes within bounds, but do not require the full bundle or parse/replay inventory, observer or journal schemas. Malformed but safely readable evidence must not block a final `INCONCLUSIVE` record. `failure.missing_artifacts` is exactly the required evidence names actually absent; `failure.unavailable_artifacts` records required names present but unsafe, unreadable or over-cap. The lists are disjoint; neither includes the record itself or optional `harness-diagnostics.txt`. Bound raw-byte reads by the per-file and 32 MiB total caps; classify over-budget files unavailable instead of aborting final-record creation. Present post/final files may be preserved but their inventory summaries remain unavailable, never empty. Derive `failure.cleanup_outcome`: `not_started` only when a complete validated journal proves neither `CLEANUP_INTENT` nor a successful `CREATED` resource; `partial` when valid durable evidence proves a cleanup attempt failed; `complete` only when a complete validated journal records exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure, while the abnormal record remains `INCONCLUSIVE` without final-inventory proof; and `unknown` when cleanup history or outcomes are incomplete, malformed, truncated or ambiguous. This mode always yields `INCONCLUSIVE`, performs no IPC inspection, cleanup or journal mutation, and never retries. A cleanup intent or prior removal is reported truthfully, not rewritten as `not_started`.
```

### §7.1 bounded regression and artifact rules
```markdown
The bounded semantic regression set must reject claimed PASS/FAIL for missing/unverified launcher or lease, a second/foreign process, diagnostic `sem_open` failure calling `sem_unlink`, pre-existing candidate keys/names, unreadable/over-cap/raw-parsed-mismatched normal inventories, noncanonical base64, missing/reordered/duplicated/unfsynced normal journal events, candidate identity/mode mismatch, missing `sem_t` mapping, residual candidates, unexpected host changes, or final-inventory mismatch. Reject normal cleanup without a durable `RUN_SHUTDOWN_COMPLETE`, complete supervisor wait receipts, closed logs and continuous lease. Reject invocation of either mode before all recorded PIDs are reaped and logs closed; when common launcher/receipt/lease/result-directory authority is absent, retain pending evidence and report `BLOCKED/INCONCLUSIVE`. Abnormal fixtures must accept malformed but safely readable non-control evidence as raw preserved/hashable bytes without requiring a complete bundle or parsing it; reject claims of PASS/FAIL, empty inventories, joins/destruction, optional diagnostics in `missing_artifacts`, overlap between `missing_artifacts` and `unavailable_artifacts`, or an inaccurate cleanup-history outcome. `not_started` requires a complete journal proving no cleanup intent and no successful `CREATED` resource; a proven failed attempt is `partial`; `complete` requires exactly one `REMOVED` outcome for every successful `CREATED` resource and no cleanup failure; missing/truncated/malformed, intent-without-outcome, or created-without-cleanup history is `unknown`. Abnormal mode must never retry cleanup or mutate journal/IPC. The finalizer must perform no speculative deletion for disputed resources. Normal PASS/FAIL requires a complete, replayable journal and every required evidence artifact; the normal bundle has eight evidence artifacts plus `run-record.json`, optional `harness-diagnostics.txt`, and a ten-file maximum. Normal artifacts are each ≤4 MiB, the journal ≤256 KiB/128 events, the final run record ≤256 KiB, and the complete normal bundle ≤32 MiB. Abnormal raw preservation remains no-follow and bounded: per-file/total read caps apply, but excess/unsafe present files are recorded as unavailable rather than making strict schema/completeness validation a prerequisite to the `INCONCLUSIVE` record.
```

### §7.1 runtime/recovery boundary
```markdown
The host-controlled runtime shape remains one controlled D2 process tree under the dedicated UID, one Q0/Q1 correctness run, no network input and no performance timing. The launcher serializes the run and finalizer under the verified exclusive lease; the harness itself spawns no child process. The host IPC namespace may contain unrelated objects, but all candidate resources must be absent before reservation and all exact cleanup remains bound to the durable journal. No broad host cleanup is permitted. If the launcher/lease or result-directory identity is absent/lost, a second process enters the identity domain, or supervisor evidence is incomplete, do not perform name-based unlink/RMID or invoke/re-enter finalization; preserve pending/partial evidence and report `BLOCKED/INCONCLUSIVE`. An abnormal record may be written only while common lease and file-safety authority remains valid.
```

## Machine-readable r12 manifest projection
The complete manifest SHA is pinned above. This projection contains all changed mode policies, the full changed failure object schema and abnormal conditional, relevant collection rules and the current validation claims. Unchanged schema portions are digest-bound, not claimed as re-reviewed.
```json
{
  "collection_responsibility": {
    "artifact_name_uniqueness": "Normal cleanup requires each of eight evidence artifacts exactly once, optional diagnostics at most once, and run-record separately. Abnormal evidence-only does not require a complete bundle; missing_artifacts is exact absent required names, unavailable_artifacts is present but unsafe/unreadable/over-cap required names, and the sets are disjoint.",
    "finalizer_semantic_authority": "The same allowlisted finalizer uses common lease/supervisor/result-directory gates. Normal mode performs strict completeness/schema/replay checks and exact cleanup. Abnormal mode strictly validates pending record and supervisor receipt only, raw-preserves bounded evidence, records actual cleanup history, and never performs IPC, cleanup, journal mutation or retry.",
    "harness_run": "Only after dedicated UID launcher/lease verified: create pending record/journal, create/reserve candidates and collect evidence. After all worker joins and Process/GW destruction, append/fsync one RUN_SHUTDOWN_COMPLETE before any cleanup intent. The supervisor later emits supervisor-termination.json after process wait/log closure.",
    "outer_finalizer": "The finalizer requires valid lease, result-directory identity, supervisor receipt, complete process reaping and closed logs for either mode. If common authority fails, leave pending and report BLOCKED/INCONCLUSIVE. Normal mode may clean only after durable shutdown proof; abnormal mode records INCONCLUSIVE without live IPC or new cleanup.",
    "supervisor_termination_receipt": "Trusted launcher writes/fsyncs supervisor-termination.json after waiting/reaping every recorded process PID and closing stdout/stderr. The finalizer binds the receipt PID set to execution_identity.process_tree_pids, decodes raw waitpid statuses, checks closed-log hashes and validates launcher/lease identity."
  },
  "journal_event_schema_v2": {
    "$id": "urn:ng039:reservation-journal-event:v2",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "additionalProperties": false,
    "properties": {
      "candidate_key": {
        "minimum": 1,
        "type": [
          "integer",
          "null"
        ]
      },
      "error_number": {
        "minimum": 0,
        "type": [
          "integer",
          "null"
        ]
      },
      "event": {
        "enum": [
          "JOURNAL_OPENED",
          "CREATE_INTENT",
          "CREATED",
          "CREATE_FAILED",
          "RUN_SHUTDOWN_COMPLETE",
          "CLEANUP_INTENT",
          "REMOVED",
          "CLEANUP_FAILED",
          "JOURNAL_CLOSED"
        ]
      },
      "identity": {
        "additionalProperties": false,
        "properties": {
          "candidate_key": {
            "minimum": 1,
            "type": [
              "integer",
              "null"
            ]
          },
          "creator_pid": {
            "minimum": 2,
            "type": [
              "integer",
              "null"
            ]
          },
          "creator_uid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "device": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "gid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "inode": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "libc_path": {
            "maxLength": 1024,
            "type": [
              "string",
              "null"
            ]
          },
          "libc_sha256": {
            "pattern": "^[0-9a-f]{64}$",
            "type": [
              "string",
              "null"
            ]
          },
          "libc_version": {
            "maxLength": 64,
            "type": [
              "string",
              "null"
            ]
          },
          "map_device_major": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_device_minor": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_end": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_inode": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "map_start": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "mapping_line": {
            "maxLength": 4096,
            "type": [
              "string",
              "null"
            ]
          },
          "mode": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "nlink": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "owner_uid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "path": {
            "maxLength": 512,
            "type": [
              "string",
              "null"
            ]
          },
          "sem_t_address": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "shmid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "size_bytes": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "source_name": {
            "maxLength": 255,
            "type": [
              "string",
              "null"
            ]
          },
          "uid": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          }
        },
        "type": [
          "object",
          "null"
        ]
      },
      "monotonic_ns": {
        "minimum": 0,
        "type": "integer"
      },
      "pid": {
        "minimum": 2,
        "type": "integer"
      },
      "resource_type": {
        "enum": [
          "sysv_shm",
          "posix_semaphore",
          "journal",
          null
        ],
        "type": [
          "string",
          "null"
        ]
      },
      "run_id": {
        "maxLength": 64,
        "minLength": 16,
        "type": "string"
      },
      "schema_version": {
        "const": "ng039-reservation-journal-event/v2"
      },
      "sequence": {
        "maximum": 128,
        "minimum": 1,
        "type": "integer"
      },
      "source_name": {
        "maxLength": 255,
        "type": [
          "string",
          "null"
        ]
      },
      "uid": {
        "minimum": 0,
        "type": "integer"
      }
    },
    "required": [
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
      "identity"
    ],
    "type": "object"
  },
  "manifest_sha256": "63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917",
  "r11_review": {
    "consent_message_id": "1559215860005081219",
    "manifest_sha256": "c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b",
    "must_fix": "Keep abnormal finalization behind common lease/supervisor/result-directory authority; derive historical cleanup as not_started/partial/complete/unknown from evidence and never retry; scope strict bundle parsing to normal mode.",
    "one_call_consent_consumed": true,
    "packet_sha256": "feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829",
    "raw_sse_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.raw.sse",
    "raw_sse_sha256": "5c9ffcebbb2bd90535465bc04a54fa13155531c267bf9ab69b69f8e830174acb",
    "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.md",
    "response_sha256": "9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7",
    "spec_sha256": "bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609",
    "submission_receipt_path": "Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012-submission.json",
    "submission_receipt_sha256": "4fa8e4828ea27ffd94185ff00d3a5d3364bdf152090b4d093569863991c6aafd",
    "verdict": "AMEND — documentary scope only; r10 B1 was resolved, but r11 identified two remaining blockers: lease loss could select abnormal finalization despite a both-modes lease prerequisite; cleanup history after a cleanup intent could be misreported as not_started. Optional advice: scope strict full-bundle parsing/completeness to normal mode."
  },
  "review_scope": "Same-SPEC v1.14 documentary response to Astra r11 B1/B2 and evidence-preservation advice. No code, configure/build, harness/finalizer execution, IPC, D3, commit or push. Fresh exact-SHA owner consent is required for one r12 Astra review. D2 remains blocked until dedicated launcher/lease is provisioned and a separate owner execution decision is recorded. Abnormal evidence-only writes a final record only while the trusted launcher, execution/result-directory identities, supervisor proof and continuous lease remain valid; otherwise preserve pending evidence. It records historical cleanup outcome and never retries.",
  "run_record_v5_changed_contract": {
    "$id": "urn:ng039:run-record:v5",
    "abnormal_finalization_conditional": {
      "if": {
        "properties": {
          "finalization_mode": {
            "const": "abnormal_evidence_only"
          }
        },
        "required": [
          "finalization_mode"
        ]
      },
      "then": {
        "properties": {
          "cleanup": {
            "properties": {
              "candidate_resources_removed": {
                "enum": [
                  false,
                  null
                ]
              },
              "complete": {
                "const": false
              },
              "final_inventory_complete": {
                "enum": [
                  false,
                  null
                ]
              },
              "final_inventory_matches_pre": {
                "enum": [
                  false,
                  null
                ]
              },
              "finalizer_complete": {
                "const": true
              },
              "gateway_joined": {
                "enum": [
                  false,
                  null
                ]
              },
              "output_worker_joined": {
                "enum": [
                  false,
                  null
                ]
              },
              "process_destroyed": {
                "enum": [
                  false,
                  null
                ]
              }
            },
            "required": [
              "complete",
              "candidate_resources_removed",
              "finalizer_complete",
              "gateway_joined",
              "output_worker_joined",
              "process_destroyed",
              "final_inventory_complete",
              "final_inventory_matches_pre"
            ],
            "type": "object"
          },
          "failure": {
            "properties": {
              "cleanup_outcome": {
                "enum": [
                  "not_started",
                  "partial",
                  "complete",
                  "unknown"
                ]
              }
            },
            "required": [
              "cleanup_outcome",
              "missing_artifacts",
              "unavailable_artifacts"
            ],
            "type": "object"
          },
          "inventories": {
            "properties": {
              "final": {
                "properties": {
                  "available": {
                    "const": false
                  },
                  "complete": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "is_empty": {
                    "type": "null"
                  },
                  "sha256": {
                    "type": "null"
                  }
                },
                "required": [
                  "available",
                  "complete",
                  "is_empty",
                  "sha256"
                ],
                "type": "object"
              },
              "post": {
                "properties": {
                  "available": {
                    "const": false
                  },
                  "complete": {
                    "enum": [
                      false,
                      null
                    ]
                  },
                  "is_empty": {
                    "type": "null"
                  },
                  "sha256": {
                    "type": "null"
                  }
                },
                "required": [
                  "available",
                  "complete",
                  "is_empty",
                  "sha256"
                ],
                "type": "object"
              }
            },
            "required": [
              "post",
              "final"
            ],
            "type": "object"
          },
          "record_finalized": {
            "const": true
          },
          "status": {
            "const": "INCONCLUSIVE"
          }
        },
        "required": [
          "record_finalized",
          "status",
          "failure",
          "cleanup",
          "inventories",
          "artifacts"
        ]
      }
    },
    "failure_object_schema": {
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
            "HOST_INVENTORY_FAILED",
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
            "CANDIDATE_IDENTITY_UNCERTAIN",
            "CANDIDATE_CLEANUP_FAILED",
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
              "reservation-journal.jsonl",
              "supervisor-termination.json"
            ]
          },
          "maxItems": 8,
          "type": "array",
          "uniqueItems": true
        },
        "phase": {
          "enum": [
            "preflight",
            "reservation",
            "pre_inventory",
            "construction",
            "q0",
            "q1",
            "shutdown",
            "post_inventory",
            "cleanup",
            "final_inventory",
            "capture",
            "validation",
            "unknown",
            "finalize"
          ]
        },
        "unavailable_artifacts": {
          "items": {
            "enum": [
              "stdout.txt",
              "stderr.txt",
              "ipc-pre.json",
              "ipc-post.json",
              "ipc-final.json",
              "observer-events.json",
              "reservation-journal.jsonl",
              "supervisor-termination.json"
            ]
          },
          "maxItems": 8,
          "type": "array",
          "uniqueItems": true
        }
      },
      "required": [
        "code",
        "phase",
        "message",
        "last_event_sequence",
        "inventory_availability",
        "cleanup_outcome",
        "missing_artifacts",
        "unavailable_artifacts"
      ],
      "type": "object"
    },
    "prior_r11_run_record_schema_sha256": "59e9aade316f74dd9fe7447ffe323cdcec536046e88c6e6750af1bea7a1fd879",
    "run_record_schema_sha256": "ba05cd0bf45cac024771361e713d3d7df0a69f967ad618ce5c98047f87d72b24",
    "schema_version": {
      "const": "ng039-d2-run-record/v5"
    },
    "spec_sha256": {
      "const": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115"
    }
  },
  "runtime_bundle": {
    "abnormal_evidence_policy": "No complete-bundle requirement or non-control JSON/JSONL schema/replay in abnormal_evidence_only. Preserve/hash safe raw files under per-file/32 MiB aggregate read caps; classify absent required names as missing and present unsafe/unreadable/over-cap required names as unavailable; optional diagnostics is never required or listed missing.",
    "max_artifact_bytes": 4194304,
    "max_artifacts_excluding_run_record": 9,
    "max_files_total": 10,
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
      "observer-events.json",
      "reservation-journal.jsonl",
      "supervisor-termination.json"
    ]
  },
  "sandbox": {
    "abnormal_evidence_preservation": "Open only allowlisted no-follow regular files after supervisor proof; hash raw bytes within per-file/aggregate limits. Missing required names and present-but-unavailable required names are disjoint arrays. Unsafe common authority/lease means pending BLOCKED/INCONCLUSIVE, not finalization.",
    "abnormal_mode_live_inventory_capture": false,
    "cleanup_authority": "Destructive cleanup only in normal_cleanup after durable shutdown proof and full common gates. Abnormal evidence-only performs no IPC/cleanup/journal mutation; it requires the same continuous lease and result-directory authority through final-record closure.",
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending record and supervisor receipt for strict validation",
          "enumerate allowlisted evidence names without following links",
          "hash safely readable closed raw file bytes within per-file and aggregate read caps",
          "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
        ],
        "cleanup_outcome_classifier": {
          "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
          "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
          "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
          "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
        },
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "strict-parse/replay non-control evidence schemas or require complete normal bundle",
          "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
        "required": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure",
          "pending run record strictly valid",
          "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
        ],
        "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
        "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
      "normal_cleanup": {
        "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
        "required": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
          "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
          "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
          "stdout/stderr closed and present-file hashes match",
          "continuous dedicated-UID lease"
        ]
      },
      "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
      "preconditions": {
        "abnormal_evidence_only": [
          "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
          "pending run record and supervisor receipt strictly validate",
          "no requirement for normal joins/destruction or complete evidence bundle"
        ],
        "both_modes": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure"
        ],
        "normal_cleanup": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
          "no timeout/signal/ambiguous normal wait",
          "complete strict normal evidence bundle and inventory/journal validation"
        ]
      }
    },
    "finalizer_mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
    "inventory_requirements_by_mode": {
      "abnormal_evidence_only": "No live post/final inventory capture; do not require complete artifact bundle or parse/replay non-control evidence. Raw-preserve safely readable closed files; report unavailable separately from absent.",
      "normal_cleanup": "Complete pre/post/final inventories, strict journal/inventory parsing and full required artifact set are necessary for PASS/FAIL."
    },
    "launcher_requirement": {
      "dedicated_non_login_uid": true,
      "exclusive_run_lease": true,
      "launcher_in_five_file_code_allowlist": false,
      "lease_held_from_pre_inventory_through_finalizer": true,
      "owner_execution_authorization_required": true,
      "process_census_alone_proves_exclusivity": false,
      "trusted_root_assumption": "host root/administrator is trusted; otherwise shared-host execution remains prohibited"
    },
    "launcher_status": "NOT_PROVISIONED; no D2 configure/build/harness/finalizer or IPC action until separate owner authorization",
    "normal_cleanup_full_pre_post_final_inventory_required": true,
    "runtime_finalizer_cleanup": "normal_cleanup: strict complete bundle, journal replay, bounded exact identity inspection and journal-authorized cleanup under continuous lease. abnormal_evidence_only: bounded raw evidence preservation and one INCONCLUSIVE record only; no full bundle requirement, parsing/replay of non-control artifacts, IPC inspection, cleanup, journal mutation or retry.",
    "supervisor_termination_receipt": "Required closed artifact supervisor-termination.json v1; contains run_id, pinned launcher/lease identity, exact process_tree_pids, one raw waitpid status per PID, stdout/stderr closure flags/hashes, timeout flag and capture monotonic time. Finalizer requires exact PID-set equality, decodes each wait status, and checks each present log digest."
  },
  "semantic_finalizer": {
    "abnormal_evidence_only": {
      "cleanup_attempt": "never retry; preserve historical outcome as partial/complete/unknown and remain INCONCLUSIVE",
      "cleanup_outcome": "not_started/partial/complete/unknown derived from complete journal evidence; no full final-inventory verification in abnormal mode",
      "inventories": "post/final summaries unavailable and not empty; present raw files may be preserved; no live inventory capture",
      "journal_mutation": false,
      "lease_loss": "do not invoke/re-enter finalizer or write a final record; preserve pending and report BLOCKED/INCONCLUSIVE",
      "live_ipc_operations": false,
      "missing_artifacts": "exact actually absent required evidence-name set",
      "non_control_validation": "raw bounded preservation only; no strict schema parsing/replay or full-bundle gate",
      "status": "INCONCLUSIVE only",
      "trigger": "partial construction, _Exit, blocked join, timeout/signal, absent/invalid RUN_SHUTDOWN_COMPLETE or other non-normal process termination, only while all common lease/supervisor/result-directory gates remain valid",
      "unavailable_artifacts": "present but unsafe/unreadable/over-cap required names; disjoint from missing"
    },
    "artifact_checks": [
      "Both modes: accept only fixed artifact names; no-follow regular-file checks; preserve/hash only safely readable closed files within declared read bounds.",
      "Both modes: validate supervisor-termination.json and pending run-record before final-record writing.",
      "normal_cleanup only: require all eight evidence artifacts; require unique artifact names; apply complete bundle/file caps; strict-parse/replay journal and schemas; recompute raw inventories and all cross-artifact relationships.",
      "abnormal_evidence_only: do not require the complete evidence bundle; do not strict-parse/replay non-control JSON/JSONL; hash raw bytes. Missing and unavailable evidence-name sets are disjoint; optional diagnostics are excluded from missing_artifacts."
    ],
    "artifact_checks_by_mode": {
      "abnormal_evidence_only": [
        "strict pending-record and supervisor-receipt validation only",
        "no completeness or schema/replay requirement for non-control evidence",
        "raw hash safely readable closed allowlisted files within per-file and aggregate read caps",
        "classify absent required names as missing and present unsafe/unreadable/over-cap required names as unavailable",
        "optional diagnostics may be preserved but are never required or listed missing"
      ],
      "normal_cleanup": [
        "exact required artifact set",
        "optional diagnostics at most once",
        "strict JSON/JSONL/schema/order/run-ID/replay validation",
        "full inventory parsing/recomputation",
        "full bundle byte and count caps"
      ]
    },
    "failure_summary_checks": [
      "failure.code is recomputed from the highest-precedence observed condition; ASSERTION_FAILED is used only for a demonstrated correctness predicate failure after all evidence/safety gates pass.",
      "failure.phase must be the phase mapped to the selected failed predicate or uncertainty condition.",
      "failure.missing_artifacts equals the actually absent required evidence-name set; optional diagnostics and run-record.json are excluded.",
      "failure.inventory_availability is derived from actual inventory-file presence and successful parsing, not caller-provided booleans.",
      "failure.cleanup_outcome derives from stop/join, exclusive reservation receipts, exact unlink/RMID results and final inventory equality; FAIL requires complete.",
      "failure.last_event_sequence equals the greatest validated event sequence, or null only when no valid event exists.",
      "Any contradiction in code/phase/availability/cleanup/missing-artifact fields yields INCONCLUSIVE/EVIDENCE_INVALID.",
      "failure.unavailable_artifacts equals required evidence names that are present but unsafe/unreadable/over-cap; it is disjoint from missing_artifacts, excludes optional diagnostics and run-record.json, and cannot block abnormal INCONCLUSIVE when safe raw preservation is possible."
    ],
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending record and supervisor receipt for strict validation",
          "enumerate allowlisted evidence names without following links",
          "hash safely readable closed raw file bytes within per-file and aggregate read caps",
          "write/fsync one final INCONCLUSIVE run record while continuous lease/result-directory authority holds"
        ],
        "cleanup_outcome_classifier": {
          "complete": "only a complete validated journal records exactly one REMOVED outcome for every successful CREATED resource and no cleanup failure; overall run remains INCONCLUSIVE without final inventory proof",
          "not_started": "only a complete validated journal proves neither CLEANUP_INTENT nor a successful CREATED resource",
          "partial": "validated durable journal evidence proves at least one cleanup operation failed or ended incomplete",
          "unknown": "journal absent/malformed/truncated/conflicting or any intent/outcome is ambiguous"
        },
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "strict-parse/replay non-control evidence schemas or require complete normal bundle",
          "cleanup, retry, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "missing_artifacts": "Exact required evidence names absent from result directory; excludes run-record.json and optional harness-diagnostics.txt.",
        "required": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure",
          "pending run record strictly valid",
          "all opened non-control evidence paths are allowlisted and opened no-follow as regular closed files"
        ],
        "result": "Final status INCONCLUSIVE; cleanup history is evidence-derived, no cleanup is performed/retried; present partial artifacts are preserved as raw bytes and never treated as verified inventories.",
        "unavailable_artifacts": "Required names present but unsafe/unreadable/over-cap; disjoint from missing_artifacts and excludes optional diagnostics."
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If lease, supervisor/process/log proof, execution identity, pending record, result-directory identity or safe output authority is missing/ambiguous, do not invoke/re-enter the finalizer or write a final record; preserve pending/partial evidence and report BLOCKED/INCONCLUSIVE. A cleanup failure after CLEANUP_INTENT stops all later destructive calls and retries; write INCONCLUSIVE with evidence-derived partial/unknown outcome only while common authority remains valid.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Evidence-derived. Select normal_cleanup only with durable RUN_SHUTDOWN_COMPLETE, normal wait statuses and full common prerequisites. Select abnormal_evidence_only only when normal shutdown proof is absent but all common lease/supervisor/result-directory prerequisites remain valid. Lease loss, invalid receipt, unreaped PID, open log or changed/unsafe result directory is not an abnormal-mode trigger: do not invoke/re-enter finalizer; preserve pending evidence and report BLOCKED/INCONCLUSIVE.",
      "normal_cleanup": {
        "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
        "required": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
          "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
          "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
          "stdout/stderr closed and present-file hashes match",
          "continuous dedicated-UID lease"
        ]
      },
      "owner_and_sequence": "The launcher binds the result directory and process_tree_pids before execution and writes the supervisor receipt only after reaping every PID and closing logs. The finalizer checks common authority/lease/file-safety gates first. Normal mode strictly validates complete evidence and performs exact journal-authorized cleanup. Abnormal mode strictly validates only pending run-record and supervisor receipt, raw-preserves bounded allowlisted non-control evidence, writes INCONCLUSIVE and records historical cleanup outcome; it performs no IPC, cleanup, journal mutation or retry.",
      "preconditions": {
        "abnormal_evidence_only": [
          "normal shutdown proof may be absent, but every both_modes prerequisite remains true",
          "pending run record and supervisor receipt strictly validate",
          "no requirement for normal joins/destruction or complete evidence bundle"
        ],
        "both_modes": [
          "trusted dedicated non-login UID launcher and execution identity verified",
          "valid supervisor-termination.json schema/hash; unique wait_receipts PID set equals execution_identity.process_tree_pids",
          "every recorded PID reaped and stdout/stderr closed",
          "result-directory device/inode/owner/mode remains bound; only no-follow allowlisted paths are opened",
          "continuous dedicated-UID lease held through final-record closure"
        ],
        "normal_cleanup": [
          "exactly one durable RUN_SHUTDOWN_COMPLETE after Gateway/output joins and Process/GW destruction, before cleanup intents",
          "no timeout/signal/ambiguous normal wait",
          "complete strict normal evidence bundle and inventory/journal validation"
        ]
      }
    },
    "normal_cleanup_failure": "After any durable CLEANUP_INTENT, stop subsequent destructive calls and retries on failure. While common authority remains valid, write a final INCONCLUSIVE record with cleanup_outcome=partial/complete/unknown as supported by durable journal evidence and final-inventory state. If lease/file authority is lost or safe final-record writing fails, leave pending recovery evidence; do not re-enter abnormal finalization to claim not_started."
  },
  "spec": {
    "change_from_v1_11": "Same-SPEC documentary response to r08 B2/B3: block direct ordinary-UID execution; require dedicated single-run UID/launcher/lease; specify diagnostic no-unlink guards for all three GW.cpp sem_open-failure paths; define durable journal, raw inventory retention/recomputation, and returned-sem_t mapping evidence. No implementation/runtime authority.",
    "change_from_v1_12": "Resolve the sole r09 B1 by consistently defining finalizer IPC authority: bounded inventory/current-identity inspection and exact journal-authorized removal under continuous dedicated-UID lease; prohibit Process/GW construction, resource creation, production message traffic, ambiguous retries and broad cleanup. Preserve r09 B2/B3 controls. Documentary only.",
    "change_from_v1_13": "Same-SPEC r12 response to r11 AMEND: require common continuous lease/supervisor/file-authority for both modes and block finalization on loss; derive abnormal cleanup history as not_started/partial/complete/unknown without retry; add unavailable_artifacts; scope full strict evidence parsing/completeness to normal_cleanup and raw bounded preservation to abnormal evidence-only.",
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "status": "In Progress",
    "version": "v1.14"
  },
  "validation_claims": {
    "abnormal_partial_evidence_fixture": "PASS: INCONCLUSIVE record accepted with present raw post/final artifacts and a separate unavailable-artifact name; no non-control parsing required",
    "astra_submitted": false,
    "cleanup_history_outcome_fixtures": "PASS: a bounded offline classifier accepted only schema-valid, ordered, complete journal streams; not_started requires no-intent/no-CREATED proof; failed intent=partial; complete requires exactly one REMOVED outcome for every successful CREATED resource; empty/unknown/malformed/reordered/truncated or created-without-any-cleanup streams=unknown",
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only. Continuous lease and stable result-directory authority remain required for either finalization mode; if they fail, preserve pending evidence. Launcher is NOT_PROVISIONED; D2 blocked until separate gates.",
    "ipc_or_vm_activity": false,
    "json_schema_meta_validation": "PASS: five Draft 2020-12 JSON Schemas meta-validated; collection_responsibility is manifest policy data, not a schema",
    "lease_loss_policy_static_check": "PASS: the manifest contract says common prerequisite failure blocks finalizer/re-entry and leaves pending evidence; no lease-loss runtime/finalizer fixture was executed",
    "manifest_generation": "PASS: r12 generated from immutable submitted r11 manifest; r11 packet/response/raw/receipt preserved",
    "missing_unavailable_set_fixtures": "PASS: exact absent set, exact unavailable set and disjointness checked; optional diagnostics rejected from missing_artifacts",
    "owner_exact_sha_consent_for_r12": false,
    "runtime_finalizer_execution": false,
    "spec_sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "supervisor_pid_set_cross_artifact_validation": "NOT EXECUTED: semantic finalizer absent; not claimed by schema tests"
  }
}
```

## Source-grounded lifecycle excerpts from immutable functional baseline
These source snippets are unchanged context only and do not implement the proposed journal, supervisor or finalizer contract.

### GW.cpp:658-667
```cpp
658:   // Stop the output thread
659:   if (StopGateway == false)
660:   {
661:     StopGateway = true;
662:     OutputQueueCV.notify_all();
663:   }
664: 
665:   T->join();
666:   delete T;
667: }
```

### Process.cpp:228-257
```cpp
228:   // Create the process blocks
229:   NewBlock("HT", PB3);
230:   NewBlock("GW", PB2);
231:   // NewBlock("CLI",PB1);
232: }
233: 
234: Process::~Process()
235: {
236:   cout << "Deleting " << LN << endl;
237: 
238:   cout << "Deleting Messages container" << endl;
239: 
240:   DeleteMessages();
241: 
242:   cout << "Done" << endl;
243: 
244:   vector<Block*>::iterator it;
245:   unsigned int i = 0;
246: 
247:   for (it = Blocks.begin(); it != Blocks.end(); it++)
248:   {
249:     delete Blocks[i];
250: 
251:     i++;
252:   }
253: 
254:   Blocks.clear();
255: 
256:   delete PMB;
257: }
```

The prior r11 response is supplied as historical immutable evidence; r12 supersedes neither its artifact identity nor verdict. The complete r11 packet and response remain archived.
Packet SHA-256 is computed out-of-band and intentionally is not embedded in itself.


===== FILE: current r12 response =====
## 1. VERDICT: AMEND — documentary scope only

Reviewed the supplied r12 packet identified as SHA-256 `a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd`, with the supplied full SPEC and manifest as consistency context.

**r11 B1 is closed at documentary level:** both modes share the lease, supervisor and result-directory authority gate; authority loss blocks entry/re-entry and preserves pending evidence. Bounded abnormal raw preservation is substantially addressed, and r08 B2/B3 protections remain specified. **Truthful cleanup-history classification still has the contradictions below.**

## 2. MUST-FIX blockers

### B1 — The supplied cleanup classifier accepts an invalid transaction as `not_started`

**Locations**
- `verify_spec039_v14_r12.py`, `classify()`: shared `pending` set, `REMOVED`/`CLEANUP_FAILED` handling, and `intent_count == 0` return.
- `precode-manifest-v1.14-r12.json`, `local_validation_this_freeze.cleanup_history_outcome_fixtures`.
- Primary packet, `validation_claims.cleanup_history_outcome_fixtures`.
- SPEC §7.1, bounded regression rules requiring malformed/conflicting history to yield `unknown`.

**Concrete contradiction:** by inspection, the following stream, using the supplied schema-valid event shape and consecutive sequence/timestamp values, returns `not_started`:

```text
JOURNAL_OPENED
CREATE_INTENT(resource A)
RUN_SHUTDOWN_COMPLETE
REMOVED(resource A)
JOURNAL_CLOSED
```

`REMOVED` consumes the pending **creation** intent despite there being neither `CREATED` nor `CLEANUP_INTENT`. With no successful create and zero cleanup intents, the final branch returns `not_started`, ignoring the contradictory removal outcome. The classifier also lacks a prohibition on creation transactions after shutdown.

This contradicts the claimed rejection of malformed/reordered history and the evidence-derived outcome contract.

**Required amendment:** distinguish creation and cleanup transaction states, enforce lifecycle phase ordering, and reject orphan or wrong-kind outcomes as `unknown`. Align the bounded classifier contract and validation claims; include these specific negative cases. Invalid history must still permit safe raw preservation and an otherwise authorised `INCONCLUSIVE` record.

### B2 — Normal-finalisation failure rules still exclude a supported `complete` cleanup outcome

**Locations**
- SPEC §7.1, paragraph beginning **“In `normal_cleanup` mode, capture a complete final inventory…”**, and numbered item **5**: after an intent, failure requires `cleanup_outcome=partial` or `unknown`.
- Manifest `sandbox.finalizer_ipc_boundary.failure_rule` and its duplicate under `semantic_finalizer_contract.finalizer_ipc_boundary`.
- Conversely, manifest `semantic_finalizer_contract.normal_cleanup_failure` explicitly permits `partial/complete/unknown`.
- These conflicting clauses are reproduced in the primary packet.

**Concrete contradiction:** all removals can have durable successful outcomes and the journal can be closed, while subsequent evidence validation fails. With common authority intact, cleanup history can support `complete` while overall status remains `INCONCLUSIVE`. The manifest expressly permits that result, but the SPEC’s broader post-intent failure rule excludes it.

**Required amendment:** consistently distinguish destructive-cleanup failure from later finalisation/evidence failure. Permit `complete` when supported by the historical cleanup evidence, without implying PASS, verified final inventory or permission to retry.

**Other requested checks:** no stale unconditional abnormal `not_started` requirement remains in `classification_rules.pass_requires` or `fail_requires`; the abnormal schema includes `complete`. Missing/unavailable enums cover the eight required evidence names and exclude the record and optional diagnostics; exactness and disjointness remain semantic requirements. PID cross-artifact validation is explicitly **NOT EXECUTED**, not falsely reported as passed.

**Optional advice:** consolidate duplicated policy clauses and clearly separate historical fixture counts from current claims.

## 3. Evidence limitation

I used no tools, executed no code or tests, recomputed no hashes, and independently inspected no filesystem, launcher or runtime artefacts. The classifier counterexample is textual reasoning, not an executed probe. Supplied synthetic checks and compile/link-only D1-H evidence do not establish D2 runtime correctness or performance.

## 4. Authority boundary

**No implementation, configure/build, runtime, harness/finalizer, IPC, cleanup, VM/SSH, commit, push or D3 authority is granted.** The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED** pending the separate prerequisites and owner decision. This review authorises no follow-up call.

===== FILE: current r12 validation receipt =====
{
  "schema_version": "ng039-precode-validation-receipt/v2",
  "created_utc_date": "2026-10-12",
  "scope": "Local documentary/schema/classifier checks for SPEC-039 v1.14/r12 only; not runtime, implementation, IPC or finalizer evidence.",
  "repository": "AIOPT5",
  "document_head": "756f493b7049434ab67cf2382486a48f606a470c",
  "files": {
    "spec": {
      "path": "Specs/SPEC-039-common-performance-measurement.md",
      "sha256": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115"
    },
    "manifest": {
      "path": "Specs/RESULTS-SPEC-039/precode-manifest-v1.14-r12.json",
      "sha256": "63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917"
    },
    "packet": {
      "path": "Specs/RESULTS-SPEC-039/precode-packet-v1.14-amend-closure-20261012-sanitized-draft-r12.md",
      "sha256": "a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd",
      "bytes": 70789
    }
  },
  "validation_drivers": {
    "contract_verifier": {
      "path": "/home/gandalf/CodeRepository/infra-novagenesis-tools/verify_spec039_v14_r12.py",
      "sha256": "6a29bd9b24c2c60440383edeb1c89a14af04ce19ea7e25d564bcb1b211f8948c",
      "command": "PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 /home/gandalf/CodeRepository/infra-novagenesis-tools/verify_spec039_v14_r12.py",
      "result": "PASS; read-only; no files written"
    },
    "classifier_probes": {
      "path": "/home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_v14_r12_classifier.py",
      "sha256": "b68c4299be7416aa26289d4d7c6628831b021294072d3091c81b10cc4984f80c",
      "command": "PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 /home/gandalf/CodeRepository/infra-novagenesis-tools/test_spec039_v14_r12_classifier.py",
      "result": "PASS; executes only synthetic in-memory journal records; no files written"
    }
  },
  "executed_checks": {
    "git_diff_check": "PASS",
    "draft_2020_12_schemas_meta_valid": {
      "result": "PASS",
      "count": 5,
      "note": "collection_responsibility is manifest policy data, not a JSON Schema."
    },
    "cleanup_outcome_contract": {
      "result": "PASS",
      "outcomes": ["not_started", "partial", "complete", "unknown"],
      "stale_abnormal_not_started_rule": "ABSENT from classification_rules.pass_requires and fail_requires"
    },
    "cleanup_history_classifier": {
      "result": "PASS",
      "executed_synthetic_cases": {
        "schema_valid_complete_no_intent_and_no_created_resource": "not_started",
        "schema_valid_complete_failed_cleanup": "partial",
        "schema_valid_complete_successful_cleanup": "complete",
        "empty": "unknown",
        "unknown_label": "unknown",
        "malformed_record": "unknown",
        "reordered_sequence": "unknown",
        "intent_without_outcome": "unknown",
        "truncated_without_journal_closed": "unknown",
        "created_resource_without_any_cleanup_intent_or_outcome": "unknown",
        "created_resource_without_its_own_cleanup_while_another_is_cleaned": "unknown"
      }
    },
    "supervisor_receipt_fixture": {
      "result": "PASS",
      "details": "A synthetic schema-valid receipt is accepted; an empty wait_receipts array is rejected."
    },
    "supervisor_pid_set_cross_artifact_validation": "NOT EXECUTED: semantic finalizer is absent; this receipt does not claim it as schema or fixture coverage."
  },
  "authorization": {
    "astra_submitted": false,
    "fresh_owner_exact_packet_sha_consent": false,
    "implementation_build_runtime_ipc_vm_d3": "NOT PERFORMED / NOT AUTHORIZED"
  },
  "limitations": [
    "The cleanup classifier and fixtures are documentary, bounded offline probes; they do not execute a runtime finalizer.",
    "The worktree remains uncommitted; SHA-256 pins bind the reviewed bytes, not a clean Git tree.",
    "This sidecar intentionally does not embed its own digest to avoid a self-referential hash cycle."
  ]
}


===== FILE: r12 classifier implementation =====
#!/usr/bin/env python3
"""Read-only local contract verifier for SPEC-039 v1.14/r12."""
from __future__ import annotations

import copy
import hashlib
import json
import subprocess
from pathlib import Path

from jsonschema import Draft202012Validator

REPO = Path("/home/gandalf/workspace/novagenesis")
RESULTS = REPO / "Specs/RESULTS-SPEC-039"
SPEC = REPO / "Specs/SPEC-039-common-performance-measurement.md"
MANIFEST = RESULTS / "precode-manifest-v1.14-r12.json"
PACKET = RESULTS / "precode-packet-v1.14-amend-closure-20261012-sanitized-draft-r12.md"
R11 = RESULTS / "astra-review-v1.14-r11-20261012.md"
PINS = {
    "head": "756f493b7049434ab67cf2382486a48f606a470c",
    "spec": "0cda2743e797cfbed952a1bad7c30e8e0f15d37c57e51d70e3388b9453103115",
    "manifest": "63ccf50e6ecd19940f0c0b5e98d438ae6e34f5b2dbf0304faef97b5eca7c0917",
    "packet": "a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd",
}


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def strict_pairs(pairs: list[tuple[str, object]]) -> dict[str, object]:
    result: dict[str, object] = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"duplicate JSON key: {key}")
        result[key] = value
    return result


def reject_constant(value: str) -> None:
    raise ValueError(f"non-finite JSON value {value}")


def reject(action, label: str) -> None:
    try:
        action()
    except Exception:
        return
    raise AssertionError(f"negative probe unexpectedly accepted: {label}")


def classify(events: object, journal_schema: dict[str, object]) -> str:
    """Classify only a complete, schema-valid, ordered journal stream."""
    if not isinstance(events, list) or not events:
        return "unknown"
    validator = Draft202012Validator(journal_schema)
    try:
        for item in events:
            validator.validate(item)
    except Exception:
        return "unknown"

    if [item["sequence"] for item in events] != list(range(1, len(events) + 1)):
        return "unknown"
    if any(events[i]["monotonic_ns"] < events[i - 1]["monotonic_ns"] for i in range(1, len(events))):
        return "unknown"
    if len({item["run_id"] for item in events}) != 1:
        return "unknown"

    labels = [item["event"] for item in events]
    if labels[0] != "JOURNAL_OPENED" or labels[-1] != "JOURNAL_CLOSED":
        return "unknown"
    if labels.count("JOURNAL_OPENED") != 1 or labels.count("JOURNAL_CLOSED") != 1:
        return "unknown"
    if labels.count("RUN_SHUTDOWN_COMPLETE") != 1:
        return "unknown"
    shutdown = labels.index("RUN_SHUTDOWN_COMPLETE")
    if any(label in {"CLEANUP_INTENT", "REMOVED", "CLEANUP_FAILED"} for label in labels[:shutdown]):
        return "unknown"

    created: set[tuple[object, object, object]] = set()
    pending: set[tuple[object, object, object]] = set()
    outcomes: dict[tuple[object, object, object], str] = {}
    for item in events[1:-1]:
        label = item["event"]
        resource = (item["resource_type"], item["candidate_key"], item["source_name"])
        if label == "CREATE_INTENT":
            if resource in created or resource in pending:
                return "unknown"
            pending.add(resource)
        elif label == "CREATED":
            if resource not in pending:
                return "unknown"
            pending.remove(resource)
            created.add(resource)
        elif label == "CREATE_FAILED":
            if resource not in pending:
                return "unknown"
            pending.remove(resource)
        elif label == "CLEANUP_INTENT":
            if resource not in created or resource in pending or resource in outcomes:
                return "unknown"
            pending.add(resource)
        elif label in {"REMOVED", "CLEANUP_FAILED"}:
            if resource not in pending:
                return "unknown"
            pending.remove(resource)
            outcomes[resource] = label
        elif label != "RUN_SHUTDOWN_COMPLETE":
            return "unknown"

    if pending:
        return "unknown"
    intent_count = labels.count("CLEANUP_INTENT")
    if intent_count == 0:
        return "not_started" if not created else "unknown"
    if len(outcomes) != intent_count or set(outcomes) != created:
        return "unknown"
    if any(outcome == "CLEANUP_FAILED" for outcome in outcomes.values()):
        return "partial"
    return "complete" if all(outcome == "REMOVED" for outcome in outcomes.values()) else "unknown"


def event(sequence: int, name: str, resource: str = "journal", key: int | None = None) -> dict[str, object]:
    return {
        "schema_version": "ng039-reservation-journal-event/v2",
        "run_id": "NG039-D2-0123456789",
        "sequence": sequence,
        "event": name,
        "monotonic_ns": sequence,
        "pid": 2345,
        "uid": 1001,
        "resource_type": resource,
        "candidate_key": key,
        "source_name": None,
        "error_number": None,
        "identity": None,
    }


def main() -> None:
    head = subprocess.check_output(["git", "-C", str(REPO), "rev-parse", "HEAD"], text=True).strip()
    actual = {"head": head, "spec": sha(SPEC), "manifest": sha(MANIFEST), "packet": sha(PACKET)}
    if actual != PINS:
        raise SystemExit("frozen pin mismatch: " + json.dumps(actual, sort_keys=True))

    manifest = json.loads(MANIFEST.read_text(), object_pairs_hook=strict_pairs, parse_constant=reject_constant)
    packet = PACKET.read_text()
    if sha(SPEC) != manifest["spec"]["sha256"] or sha(SPEC) not in packet or sha(MANIFEST) not in packet:
        raise AssertionError("SPEC/manifest/packet binding mismatch")
    if R11.read_text() not in packet:
        raise AssertionError("prior r11 response missing/not exact")

    schemas = [entry for entry in manifest["evidence_schemas"].values() if "$schema" in entry]
    assert len(schemas) == 5
    for schema in schemas:
        Draft202012Validator.check_schema(schema)

    rules = manifest["classification_rules"]
    stale_rule = "abnormal_evidence_only is INCONCLUSIVE, has cleanup_outcome=not_started and performs no IPC/cleanup/journal mutation"
    assert stale_rule not in rules["pass_requires"]
    assert stale_rule not in rules["fail_requires"]

    record_schema = manifest["evidence_schemas"]["run_record"]
    required_names = set(rules["required_artifact_names"])
    failure_schema = record_schema["properties"]["failure"]["oneOf"][1]
    missing_schema = failure_schema["properties"]["missing_artifacts"]
    unavailable_schema = failure_schema["properties"]["unavailable_artifacts"]
    assert len(required_names) == 8
    assert set(missing_schema["items"]["enum"]) == required_names
    assert set(unavailable_schema["items"]["enum"]) == required_names
    assert missing_schema["maxItems"] == unavailable_schema["maxItems"] == 8
    assert "harness-diagnostics.txt" not in missing_schema["items"]["enum"]
    abnormal = record_schema["allOf"][-1]["then"]
    assert set(abnormal["properties"]["failure"]["properties"]["cleanup_outcome"]["enum"]) == {
        "not_started", "partial", "complete", "unknown"
    }

    journal_schema = manifest["evidence_schemas"]["reservation_journal_event"]
    no_intent = [event(1, "JOURNAL_OPENED"), event(2, "RUN_SHUTDOWN_COMPLETE"), event(3, "JOURNAL_CLOSED")]
    success = [
        event(1, "JOURNAL_OPENED"), event(2, "CREATE_INTENT", "sysv_shm", 11),
        event(3, "CREATED", "sysv_shm", 11), event(4, "RUN_SHUTDOWN_COMPLETE"),
        event(5, "CLEANUP_INTENT", "sysv_shm", 11), event(6, "REMOVED", "sysv_shm", 11),
        event(7, "JOURNAL_CLOSED"),
    ]
    failed = copy.deepcopy(success)
    failed[5] = event(6, "CLEANUP_FAILED", "sysv_shm", 11)
    assert classify(no_intent, journal_schema) == "not_started"
    assert classify(success, journal_schema) == "complete"
    assert classify(failed, journal_schema) == "partial"
    for invalid in (
        [], ["UNKNOWN_EVENT"], [event(1, "JOURNAL_OPENED"), {"event": "JOURNAL_CLOSED"}],
        [event(2, "JOURNAL_OPENED"), event(1, "JOURNAL_CLOSED")], success[:-1],
        [event(1, "JOURNAL_OPENED"), event(2, "CLEANUP_INTENT", "sysv_shm", 11), event(3, "JOURNAL_CLOSED")],
    ):
        assert classify(invalid, journal_schema) == "unknown"
    created_without_cleanup = copy.deepcopy(success)
    created_without_cleanup.insert(4, event(4, "CREATE_INTENT", "sysv_shm", 12))
    created_without_cleanup.insert(5, event(5, "CREATED", "sysv_shm", 12))
    for sequence, item in enumerate(created_without_cleanup, 1):
        item["sequence"] = sequence
        item["monotonic_ns"] = sequence
    assert classify(created_without_cleanup, journal_schema) == "unknown"
    supervisor_schema = manifest["evidence_schemas"]["supervisor_termination"]
    supervisor = {"schema_version": "ng039-supervisor-termination/v1", "run_id": "NG039-D2-0123456789", "launcher_sha256": "a" * 64, "lease_path": "/run/lock/ng039/test.lock", "lease_device": 1, "lease_inode": 2, "process_tree_pids": [2345], "wait_receipts": [{"pid": 2345, "raw_wait_status": 9}], "stdout_closed": True, "stderr_closed": True, "stdout_sha256": "b" * 64, "stderr_sha256": "c" * 64, "timeout_fired": True, "captured_monotonic_ns": 10}
    Draft202012Validator(supervisor_schema).validate(supervisor)
    reject(lambda: Draft202012Validator(supervisor_schema).validate({**supervisor, "wait_receipts": []}), "empty supervisor receipts")

    assert manifest["local_validation_this_freeze"]["astra_submitted"] is False
    print(json.dumps({"identity": "PASS", "schemas": "5/5 meta-valid", "cleanup_classifier": "PASS", "classifier_cases": {"not_started": "PASS", "partial": "PASS", "complete": "PASS", "empty_unknown_malformed_reordered_truncated_created_without_cleanup": "unknown"}, "stale_abnormal_outcome_rules": "ABSENT", "cross_artifact_pid_set": "NOT EXECUTED", "astra_submitted": False, "writes_performed": False}, indent=2))


if __name__ == "__main__":
    main()


===== FILE: r12 classifier test matrix =====
#!/usr/bin/env python3
"""Focused fail-closed probes for the r12 cleanup-history classifier."""
from __future__ import annotations

import importlib.util
import json
from pathlib import Path

MODULE = Path(__file__).with_name("verify_spec039_v14_r12.py")
spec = importlib.util.spec_from_file_location("verify_spec039_v14_r12", MODULE)
assert spec and spec.loader
validator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(validator)


def journal_schema() -> dict[str, object]:
    manifest = json.loads(
        (Path("/home/gandalf/workspace/novagenesis")
         / "Specs/RESULTS-SPEC-039/precode-manifest-v1.14-r12.json").read_text()
    )
    return manifest["evidence_schemas"]["reservation_journal_event"]


def event(sequence: int, name: str, resource: str = "journal", key: int | None = None) -> dict[str, object]:
    return {
        "schema_version": "ng039-reservation-journal-event/v2",
        "run_id": "NG039-D2-0123456789",
        "sequence": sequence,
        "event": name,
        "monotonic_ns": sequence,
        "pid": 2345,
        "uid": 1001,
        "resource_type": resource,
        "candidate_key": key,
        "source_name": None,
        "error_number": None,
        "identity": None,
    }


def classify(events: list[object]) -> str:
    return validator.classify(events, journal_schema())


def expect(events: list[object], outcome: str) -> None:
    got = classify(events)
    assert got == outcome, f"expected {outcome!r}, got {got!r} for {events!r}"


def main() -> None:
    no_intent = [
        event(1, "JOURNAL_OPENED"),
        event(2, "RUN_SHUTDOWN_COMPLETE"),
        event(3, "JOURNAL_CLOSED"),
    ]
    successful_cleanup = [
        event(1, "JOURNAL_OPENED"),
        event(2, "CREATE_INTENT", "sysv_shm", 11),
        event(3, "CREATED", "sysv_shm", 11),
        event(4, "RUN_SHUTDOWN_COMPLETE"),
        event(5, "CLEANUP_INTENT", "sysv_shm", 11),
        event(6, "REMOVED", "sysv_shm", 11),
        event(7, "JOURNAL_CLOSED"),
    ]
    failed_cleanup = successful_cleanup.copy()
    failed_cleanup[5] = event(6, "CLEANUP_FAILED", "sysv_shm", 11)

    expect(no_intent, "not_started")
    expect(successful_cleanup, "complete")
    expect(failed_cleanup, "partial")
    created_without_any_cleanup = [
        event(1, "JOURNAL_OPENED"),
        event(2, "CREATE_INTENT", "sysv_shm", 11),
        event(3, "CREATED", "sysv_shm", 11),
        event(4, "RUN_SHUTDOWN_COMPLETE"),
        event(5, "JOURNAL_CLOSED"),
    ]
    expect(created_without_any_cleanup, "unknown")

    # Absence, unknown labels, malformed events, incomplete transactions and
    # any sequence/order violation fail closed as unknown.
    expect([], "unknown")
    expect(["UNKNOWN_EVENT"], "unknown")
    expect([event(1, "JOURNAL_OPENED"), {"event": "JOURNAL_CLOSED"}], "unknown")
    expect([event(2, "JOURNAL_OPENED"), event(1, "JOURNAL_CLOSED")], "unknown")
    expect([event(1, "JOURNAL_OPENED"), event(2, "CLEANUP_INTENT", "sysv_shm", 11), event(3, "JOURNAL_CLOSED")], "unknown")
    expect(successful_cleanup[:-1], "unknown")
    created_without_cleanup = successful_cleanup.copy()
    created_without_cleanup.insert(4, event(4, "CREATE_INTENT", "sysv_shm", 12))
    created_without_cleanup.insert(5, event(5, "CREATED", "sysv_shm", 12))
    for sequence, item in enumerate(created_without_cleanup, 1):
        item["sequence"] = sequence
        item["monotonic_ns"] = sequence
    expect(created_without_cleanup, "unknown")
    reversed_cleanup = successful_cleanup.copy()
    reversed_cleanup[4], reversed_cleanup[5] = reversed_cleanup[5], reversed_cleanup[4]
    expect(reversed_cleanup, "unknown")


if __name__ == "__main__":
    main()


===== FILE: Specs/RESULTS-SPEC-039/astra-review-r06-20261010.md; SHA-256 9058bc8a6490b1774d0bd0fb8f185dbaa8669c73353faffffd87d8ba2cd75a2e; 7321 bytes =====
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


===== FILE: Specs/RESULTS-SPEC-039/astra-review-r07-20261010.md; SHA-256 9aebba801166f875b9c6e030923450a4e28122852fb8d4e0cc13fe7a198088ab; 4852 bytes =====
## 1. VERDICT: AMEND

r07 substantially resolves the four r06 findings at documentary level: trusted-root containment and exact `-c` cardinality are now explicit; the minimal five-variable `env -i` contract is frozen; link validation reads the named file and rejects forwarding; and local schema constraints are complemented by a sufficiently explicit, fail-closed semantic-finalizer contract. That bounded division is proportionate and does not require a generic compiler/linker grammar, exhaustive environment denylisting, or duplicated cross-artifact enforcement. However, the frozen checker invocations still disagree with their argument handling, and the compile checker admits joined forced-include forms that can bypass observer-macro inspection. These prevent full closure while preserving the r05 protections. This conclusion is textual only: supplied identity strings are consistent where repeated, but hashes have not been independently recomputed; the sidecar receipt’s contents are not supplied. Reported synthetic checks and D1-H compile/link evidence establish neither D2 execution nor implemented finalizer behaviour.

## 2. Must-fix blockers

### B1. Both planned checker commands shift the positional arguments

**Locations:** Manifest `cmake.compile_commands_verification.planned_command` and `.checker_logic`; `cmake.link_command_verification.planned_command` and `.checker_logic`; SPEC §7 item 4.

Both commands use:

```text
/usr/bin/python3 -c <checker> -- <inputs...>
```

Both snippets nevertheless treat `sys.argv[1]` as their first input. With this invocation, that argument is the literal `--`, not `$DB` or `$LINK_TXT`. The remaining inputs are also shifted.

**Acceptance risk:** The frozen entry points do not validate the intended database or named link file. Function-level synthetic positives do not establish that these complete invocations are correct. This is a documentary invocation mismatch, not an observed execution failure.

**Required amendment:** Remove the separator following the `-c` programme, or explicitly consume and validate it before reading the inputs. Freeze a positive and negative invocation-level check for each exact command shape, including the trusted roots and expected diagnostic target.

### B2. Joined forced-include options can bypass observer-macro validation

**Locations:** Manifest `cmake.compile_commands_verification.checker_logic`, specifically the compiler-mode rejection condition and `parse_macro_switches`; SPEC §7 item 4’s requirement to reject forced-include forms that bypass the critical checks.

The rejection condition tests exact tokens `-include` and `-imacros`, but not their joined operand forms. An otherwise conforming diagnostic argument vector can therefore retain:

```text
-DNG_GW_QUEUE_TEST_OBSERVATION=1
```

and append, for example:

```text
-include/path/to/override.h
```

where the header undefines the observer macro. The joined token is neither rejected nor examined by `parse_macro_switches`; a `.h` operand also does not disturb the checker’s source-suffix test.

**Acceptance risk:** The checker can certify the required command-line definition while an uninspected forced include changes the macro state seen by the translation unit. This reopens an acceptance-critical observer bypass; it is not a request to validate unrelated compiler options.

**Required amendment:** Reject joined as well as separate forced-include and macro-file forms supported by the pinned compiler. Add targeted negative fixtures for those forms under normal Python and `PYTHONOPTIMIZE=1`. No generic argument grammar is necessary.

## 3. Optional advisories — non-blocking

- **Failure-phase vocabulary:** Manifest `semantic_finalizer_contract.assertion_to_phase` uses `bootstrap`, `post_inventory` and `final_inventory`, whereas `evidence_schemas.run_record.properties.failure` does not permit those values in `phase`. Align the mapping and enum before implementing finalization. The existing no-valid-final-record → INCONCLUSIVE rule prevents false acceptance, but this mismatch can unnecessarily prevent informative, schema-valid failure records.
- **Evidence and authority bookkeeping:** Preserve the distinction between packet-reported checker/schema probes and the unimplemented semantic finalizer. Record this review separately against the frozen identity; the packet’s “not submitted” and preparation-only statements are historical snapshot text, superseded for this single documentary review by the explicit authorisation in the request—not by operational permission.

## 4. Gate statement

**Documentary outcome: AMEND; closure is not yet achieved. D2 implementation/execution authority remains NO.** This review authorises no configure/build, harness or finalizer execution, IPC/namespace/VM/SSH activity, D3 measurement, commit or push.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-r07-b1b2-20261011.md; SHA-256 edd29c9967af7c91778f028cf7405c46ee06ef68875c1ebd95042d044a1bb212; 2332 bytes =====
1. **VERDICT: AMEND** — B1 and B2 are closed at documentary level, but the amended manifest still binds acceptance to a different SPEC digest from the authorised frozen SPEC.

2. **Must-fix blockers**
   - **Stale SPEC identity in the manifest and run-record schema.**  
     **Locations:** embedded `Specs/RESULTS-SPEC-039/precode-manifest-v1.10-r07.json`, `$.spec.sha256` and `$.evidence_schemas.run_record.properties.spec_sha256.const`; compare the packet’s **Frozen identity and privacy** section and embedded SPEC file header.  
     Both manifest fields retain `7129b0fac7788b0b7c8cf3d2f93bae1a4e05da6873b99c2cd7b3c33247b1ff2a`, whereas the authorised SPEC digest is `910cb308acccdc31f11a0667acf914aa0443e64d630a6cc938a16ce059407bbd`. Consequently, the schema rejects a run record identifying the reviewed amendment and permits the superseded identity instead. This is a concrete acceptance/provenance inconsistency, not additional compiler hardening. Align both identity fields with the governing frozen SPEC and refresh the affected frozen identities and receipt binding.

3. **Optional advisories — non-blocking**
   - **B1 closure:** SPEC §7 item 4 and both manifest checker `planned_command`/`checker_logic` pairs now map operands directly to the intended `sys.argv` positions, without `--`. Missing operands cause caught exceptions and exit 2; these checks do not depend on Python assertions.
   - **B2 closure:** the compile checker’s `startswith(("-include", "-imacros", ...))` rejection covers joined forms as well as the explicitly rejected split forms. The manifest reports the relevant negatives and entrypoint checks under normal Python and `PYTHONOPTIMIZE=1`.
   - Those synthetic outcomes remain supplied documentary claims, not independently reproduced evidence. The sidecar’s separate hash binding is acceptable in principle; its contents were not supplied for inspection.
   - The deferred `failure.phase` vocabulary observation remains non-blocking; no concrete false PASS attributable to it is established here.

4. **Gate statement:** Documentary outcome only: B1/B2 closed, but packet acceptance requires the identity correction above. No tools or code were executed, and no build, runtime, finalizer or performance behaviour was verified. **D2 implementation/execution authority remains NO.**

===== FILE: Specs/RESULTS-SPEC-039/astra-review-r07-b1b2b3-20261011-derived.md; SHA-256 43425d2dff3c639667edf50d01deed5e3e789cad04223ba09a0852276e597852; 3227 bytes =====
## APPROVE — documentary closure only

**Reviewed binding:** packet `ffc71bf48dd0f2c2bea7a3448923a9e11ba5d3a16ebbea19ff81a9725e8f46af`, at the supplied repository HEAD. This verdict applies only to this frozen packet.

### Prior AMEND disposition

| Item | Exact locations | Assessment |
|---|---|---|
| **B1 — literal checker argument mapping** | SPEC §7 item 4; manifest `cmake.compile_commands_verification.{planned_command,checker_logic}` and `cmake.link_command_verification.{planned_command,checker_logic}` | **Closed by textual inspection.** Neither command inserts an unconsumed `--`. Compile inputs map to `sys.argv[1:5]`; link inputs map to `sys.argv[1:4]`. Missing required operands cause exceptions handled with nonzero exit 2, independently of Python assertions. |
| **B2 — joined forced-include/macro-file bypass** | SPEC §7 item 4; manifest `cmake.compile_commands_verification.checker_logic`, compiler-mode/toolchain rejection | **Closed for the identified bypass.** Prefix rejection of `-include` and `-imacros` covers joined as well as split spellings. The corresponding negatives are included in the declared two-mode synthetic matrix. No generic compiler grammar is needed for this closure. |
| **B3 — current SPEC identity** | Manifest `spec.sha256` and `evidence_schemas.run_record.properties.spec_sha256.const` | **Closed at documentary binding level.** Both contain `f7742629a6069810bade1df295277dfcb2c0ff6c47d614236b4b1e29ca630f26`, matching the supplied frozen SPEC identity. |

**Remaining acceptance/provenance/false-PASS blockers: none identified within this narrow documentary contract.** The retained schema/local checks and mandatory semantic finalizer contract prohibit accepting assertion labels alone. This is design-level closure, not proof of an implemented finalizer or a successful D2 result.

### Optional hardening — not conditions of approval

- Reconcile `semantic_finalizer_contract.assertion_to_phase` values such as `bootstrap`, `post_inventory`, and `final_inventory` with the run-record `failure.phase` enum. The existing fail-closed requirements prevent this vocabulary mismatch from authorizing PASS.
- Clarify historical versus current bookkeeping in `synthetic_validation.r07_local_preflight` and `local_validation_this_freeze`; older counts and submission/consent flags coexist with the amended summaries. These do not override this request’s exact-packet review authorization.

### Evidence and authority limits

I inspected only the supplied text. **I ran no tools, recomputed no hashes, and independently reproduced no synthetic results.** The synthetic receipt’s digest is supplied externally, but its contents are not included; I therefore do not attest to its exact-input bindings or execution results. D1-H remains packet-reported fresh compile/link-only evidence, not recovered historical evidence or correctness/performance evidence.

**Does this packet authorize ANY code edit, configure/build, harness execution, IPC/VM action, D3/performance work, commit, push, or release? No.** This advisory APPROVE closes the documentary B1/B2/B3 response only. D2 operations still require a separate exact-scope owner execution decision; D3 remains deferred.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-r08-host-controlled-20261012.md; SHA-256 32ab51a41285516ec725462e14bd9c9bec11ffc215b113bc588c2cd1c13d5a85; 6180 bytes =====
## 1. VERDICT: AMEND

The amendment provides sensible collision checks, restrictive permissions and fail-closed classification, but **does not establish continuous, identity-bound exclusivity on the shared host**. In particular, the production failure-path unlink and the cleanup check-to-unlink race remain unsafe. There is also a conflicting SPEC identity and an incomplete evidence-storage contract.

This assessment covers only the supplied text and projection. No tools, filesystem inspection, hash verification or runtime validation were performed; r07 B1/B2/B3 are not reopened.

## 2. Must-fix blockers

### B1 — Conflicting frozen SPEC identity

**Locations:** Owner request, “FROZEN IDENTITIES”; packet “Frozen identity and privacy”; “Full SPEC SHA-256”; manifest `selected_fields.spec.sha256` and `run_record_host_profile_delta.spec_sha256_binding`.

The owner’s SPEC digest ends `da29722992`; the packet and schema binding use `da29712822992`. These are different identities. The exact-packet review authorisation is explicit, but the claimed underlying SPEC binding is inconsistent.

**Required amendment:** Reconcile the SPEC digest throughout the authorisation metadata, packet and manifest against the intended frozen document. Do not silently choose either value; obtain fresh exact-packet consent for revised bytes.

### B2 — Exclusive creation does not prevent replacement or foreign unlink

**Locations:** SPEC §7.1 reservation and cleanup paragraphs; D2b lifecycle clause; `Common/src/GW.cpp:1300–1322`; manifest `sandbox.cleanup_authority` and `semantic_finalizer_contract.inventory_checks`.

`IPC_EXCL`/`O_EXCL` establish exclusivity **at creation**, not throughout the run. Mode `0600` and umask `0077` do not exclude other processes with the same UID or privileged processes.

Concrete unsafe cases:

- A semaphore can be replaced between exclusive creation and the pathname inspection used to record its inode. A later pathname lookup alone does not prove that inode belongs to the object returned by `sem_open`.
- A matching inode check followed by `sem_unlink(name)` is not an atomic conditional deletion. Replacement between those operations can cause foreign deletion.
- `ReturnIPCSHMID` unconditionally calls `sem_unlink` after a failed `sem_open`. Reservation does not guarantee later opens succeed. A replacement combined with an open failure can therefore reach an unchecked unlink before the finalizer intervenes.
- The ordinary production `O_CREAT` opens can attach to replacements or recreate missing resources without exclusive receipts.

SHM deletion by recorded **shmid** is materially safer than deletion by semaphore name: simple key replacement does not redirect `IPC_RMID` to the new segment. Nevertheless, production reopening by key and identity continuity still need protection.

**Required amendment:** Specify enforceable exclusion of competing mutation from creation through teardown and cleanup, including receipt capture, or retain the host-profile block. For the cited production error path, the smallest local mitigation is a compile-time-gated D2 change in already-allowlisted `GW.cpp` that suppresses unchecked unlink and records uncertainty for controlled cleanup. This requires explicit documentary approval and does **not**, by itself, resolve the general name-based cleanup race. Merely adding another inode check is insufficient.

An eventual `INCONCLUSIVE` result or inventory mismatch cannot undo foreign deletion.

### B3 — Required verification evidence lacks a complete representable lifecycle

**Locations:** `evidence_schemas.collection_responsibility.harness_run`; `runtime_bundle`; `semantic_finalizer_contract.artifact_checks` and `inventory_checks`; `ipc_inventory`; `run_record_host_profile_delta.reservations`.

The finalizer is required to verify evidence whose storage and binding are not fully specified:

- A durable before/after-create journal is mandatory, but no journal location, entry/state format or final-bundle binding is defined. The allowlisted files contain no designated journal; completed reservation arrays and `journal_fsynced=true` do not themselves define one.
- The finalizer must reconcile raw `/proc/sysvipc` digests with normalised summaries, but the inventory schema retains only digests and summaries, not the raw table bytes needed for that recomputation.
- Libc evidence consists of a version string, expected pathname and verification boolean. No verification procedure or retained evidence binds the exact bare-decimal production name, returned semaphore object and observed inode to the target libc. The recorded `glibc 2.39` identity alone does not prove that mapping.

**Required amendment:** Define where these bounded evidence records live, their schema/state transitions and the finalizer’s verification rules. They may be embedded in existing allowlisted artifacts; extra files are not inherently necessary. Preserve honest partial/failure evidence without asserting successful mapping or reservation. Unverified or mismatched mapping must remain `INCONCLUSIVE`, with no speculative deletion.

## 3. Optional advisories — non-blocking

- Say **five candidate key/name pairs: five SHM segments plus five semaphores**, rather than “five resources”.
- Add documentary negative cases for same-UID replacement, failure after creation but before receipt persistence, libc-mapping mismatch and cleanup-time replacement.
- The schema/finalizer division is reasonable: cross-artifact equality need not be duplicated in JSON Schema. PASS and safe FAIL are conservatively constrained **in intent**, but schema-valid labels cannot discharge B2 or the evidence gaps in B3.
- Allowing unrelated non-candidate IPC is reasonable. Exact global inventory equality will conservatively make runs `INCONCLUSIVE` when unrelated host activity changes those inventories; that is not itself a safety defect.

## 4. Gate statement

**Documentary outcome only: AMEND. All operational authority remains NO.** This review authorises no implementation, configure/build, harness or finalizer execution, IPC, namespace, VM, SSH, D3 measurement, commit, push or release.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-r08-followup-agent-reconciliation-20261012.md; SHA-256 6ff9bd65cab5cadb535aeb428781ede9b17ab357a6cdf6121e358af70630ff7b; 5173 bytes =====
## 1. Correction to prior B1 — WITHDRAWN

B1 was a **submission-envelope error, not a packet defect**. The supplied packet consistently uses:

`bc735702fff9adde69a00705715e4025cedca5f3e1314eb7200da29712822992`

This appears in “Frozen identity and privacy”, “Full SPEC SHA-256”, manifest `spec.sha256` and `run_record_host_profile_delta.spec_sha256_binding`.

The sender’s earlier digest omitted `128`. I withdraw the conflicting-packet-identity finding and its requirement for packet amendment or fresh consent **on that account**. This correction concerns the unchanged packet.

## 2. Remaining blockers

### B2 — SUPPORTED, with a qualification on the proposed remedy

The supplied text supports independent review A:

- **Creation is not continuous exclusion.** §7.1 specifies exclusive reservation and `0600` permissions, but no enforceable exclusion of same-UID competing mutation through receipt capture, production use and cleanup.
- **Production opens are not identity-bound.** `GW.cpp:693,713` and `1300,1306` use `O_CREAT`/`IPC_CREAT`, allowing attachment to replacements or recreation after disappearance.
- **Unchecked unlink is explicit.** `GW.cpp:1318–1322` calls `sem_unlink` following `sem_open` failure, without checking ownership or identity.
- **Cleanup remains check-then-act.** §7.1 and D2b require identity rechecking before name-based unlink; they do not make that check and unlink atomic. Replacement can also occur between semaphore creation and pathname-based receipt capture.

The finalizer’s `inventory_checks` requirement that the production failure path “cannot cause” foreign unlink states an obligation, not a mechanism satisfying it. Subsequent `INCONCLUSIVE` classification cannot undo deletion.

**Required documentary resolution:** establish enforceable identity continuity and exclusion across the relevant lifecycle, or retain the host-profile block. Suppressing the reachable unchecked unlink is a possible local mitigation within already-allowlisted `GW.cpp`, but is neither a complete remedy nor an approved change. Likewise, “INCONCLUSIVE/no cleanup when ambiguous” is safe only if unsafe production deletion is also prevented; it does not itself establish safe execution.

SHM removal by recorded **shmid** is materially safer than semaphore removal by name; the two should not be treated as identical races.

### B3 — SUPPORTED, with precision about the evidence gap

The supplied projection supports independent review B:

- **Journal lifecycle:** `collection_responsibility.harness_run` requires fsynced before/after-create records, but `runtime_bundle` and `reservations` do not designate their storage, event/state representation, recovery rules or binding into retained final evidence. Reservation arrays plus `journal_fsynced=true` do not supply that contract.
- **Raw inventory verification:** `semantic_finalizer_contract.inventory_checks` requires raw `/proc/sysvipc` digests and normalised summaries to agree. The complete `ipc_inventory` schema retains digests and summaries, not the raw bytes needed to recompute those digests and validate their relationship to the summaries.
- **Semaphore mapping:** §7.1 requires verification, but `reservations` supplies a libc version, name/path, inode metadata and a verification boolean without defining the procedure and evidence connecting the **returned semaphore object** to that pathname identity.

**Qualification:** the packet already defines general artifact hashing and an outer-index hash for the run record. Therefore, the defect is not “no cryptographic binding anywhere”; it is the absence of a specified retained journal/mapping/raw-evidence representation covered by those mechanisms. Existing allowlisted artifacts could hold that evidence; additional files are not inherently required.

**Required documentary resolution:** define bounded evidence storage, lifecycle and verification rules, including honest partial/failure records. Preserve `INCONCLUSIVE` and no speculative deletion when mapping or ownership cannot be established.

There is no substantive disagreement with reviews A/B after these qualifications. Review C’s envelope correction is consistent with the supplied text; archived SSE and deduplication claims are not independently verified here.

## 3. Corrected documentary verdict — AMEND

For unchanged packet SHA-256  
`3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272`:

**B1 withdrawn; B2 and B3 remain blocking.** The prior overall verdict therefore remains **AMEND**, solely on lifecycle safety and evidence-contract grounds.

**Optional advisory, not a blocker:** say “five candidate key/name pairs: five SHM segments and five semaphores”, rather than “five resources”.

This assessment uses only the supplied packet and correction context. No tools, hash recomputation or runtime validation were performed; the r07 review boundary remains unchanged.

## 4. Gate

**Documentary only; operational authority remains NO.** This response authorises no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM, SSH, D3, cleanup, commit, push or release.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-v1.12-r09-20261012.md; SHA-256 4b52a1759768714492d4c2bc5099fc9a3626a334e1a33c65e627f1725494fd01; 4413 bytes =====
## 1. VERDICT: AMEND — documentary contract only

Reviewed only the supplied packet identified as SHA-256 `560ee5654d9a8859e3e21aa916a1d7a6` **[correction: exact identity below]**:

`560ee5654d9a8859e3e21aa916a1dca9f0e564fe1f8eac41fb4ee8f4bd3397a6`

The r09 amendment substantially addresses r08 B2/B3, but contains one blocking contradiction about finalizer responsibilities. No unchanged r07 checker findings are reopened.

- **Dedicated UID/launcher/lease:** adequate as a mandatory, fail-closed prerequisite, not proof of an existing enforcement mechanism. The explicit `NOT_PROVISIONED`/`BLOCKED` states, prohibition on ordinary-host-UID execution, rejection of census-only evidence and trusted-root boundary appropriately prevent a readiness claim.
- **Diagnostic unlink suppression:** properly bounded to the three pinned branches, the opt-in isolated diagnostic build and targeted zero-unlink tests. It complements, rather than replaces, continuous exclusion.
- **Journal and inventories:** the required retained bytes, intent/outcome ordering, fsync obligations, ambiguous-crash handling, no-retry rule and final hash binding substantially close the earlier evidence gaps. Raw inventory digests and summaries must be independently recomputed.
- **Semaphore mapping:** the retained matching maps-line route can support pointer-range/shared-mapping/device/inode checks against the recorded semaphore and libc identity. This is an untested evidence procedure; missing or contradictory linkage must remain `INCONCLUSIVE`.
- **Classification:** schemas alone do not establish safety. The combined semantic contract appropriately requires evidence-derived status and excludes unsafe or incomplete runs from both PASS and correctness FAIL, subject to resolving the blocker below.

## 2. Must-fix blocker

### B1 — Contradictory finalizer IPC boundary

**Exact locations**

- **“Proposed runtime artifacts”**, D2 proposed allowlist row: `--finalize` performs finalisation “without constructing Process/GW **or touching IPC**”.
- **§7.1**, paragraphs beginning “The three pinned…” and “After Gateway/output threads…”: only the separate finalizer may clean resources; it verifies current identities and performs removal.
- **`collection_responsibility.outer_finalizer`**: the finalizer logs cleanup intent/outcome and performs exact identity-bound cleanup.
- **`sandbox.runtime_finalizer_cleanup`**: cleanup occurs after the run process exits.

**Concrete risk:** these requirements cannot all be satisfied. An implementation following the no-IPC clause cannot perform the assigned cleanup; an implementation following the cleanup clauses breaches that explicit boundary. It also leaves current-identity inspection and cleanup ownership contradictory at the point where safety depends on them.

**Required amendment:** establish one consistent owner and sequence. If `--finalize` owns cleanup, replace “or touching IPC” with a narrowly defined allowance for inventory/current-identity inspection and journal-authorised exact removal under the continuously verified lease. Continue to prohibit Process/GW construction, resource creation, production IPC use and speculative or broad cleanup. Align the cited clauses.

## 3. Optional advisories — not blockers

- Make the matching `mapping_line` mandatory for successful semaphore evidence, or add explicit mapping permissions and offset fields. The current structured alternative lacks those fields; the semantic contract must reject projections that cannot prove a shared mapping.
- Separate historical fixture counts from newly required negative classes and remove duplicated policy entries. These are not evidence that r09 semantic fixtures ran.
- State explicitly that retained journal bytes support replay but do not independently prove historical fsync calls; durability also depends on the subsequently verified collector implementation.

## 4. Authority boundary

The launcher remains **unprovisioned**, and execution remains **BLOCKED**. No tools, code execution, hash recomputation or runtime validation were performed.

**No operational authority is granted:** no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM/SSH, D3, cleanup, commit, push or release. Documentary approval, if subsequently obtained, would not replace the separate provisioning, verification and owner-authorisation gates.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.md; SHA-256 6aa0669c3cec1b11c2516d6c361d78460663cde9f097cc4c5de8264557e64c2e; 3701 bytes =====
## 1. VERDICT: AMEND — documentary scope only

Reviewed only the supplied packet identified as SHA-256 `55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146`. No tools, hash recomputation or runtime validation were used.

The original r09 prohibition on “touching IPC” is resolved. The finalizer’s IPC allowance is now narrowly and consistently limited to required inventory reads, exact current-identity inspection, and journal-authorised `IPC_RMID`/`sem_unlink`, with an immediate identity and lease recheck before each destructive call.

However, **one finalizer-sequence contradiction remains on abnormal termination**.

## 2. Must-fix blocker

### B1 — Unconditional finalizer startup prerequisites conflict with required failure finalisation

**Exact packet locations**
- **§7.1, numbered item 2:** uncertain ownership requires `_Exit` without invoking potentially unsafe `Process/GW` destructors; after process exit, “only the parent-side finalizer in item 5” may produce the final `INCONCLUSIVE` record. Partial construction and blocked join follow the same fail-closed classification.
- **Manifest `collection_responsibility.outer_finalizer`:** the finalizer “starts only after” harness exit/log closure, worker stop/join **and `Process/GW` destruction**.
- **Manifest `finalizer_ipc_boundary.owner_and_sequence` and `.preconditions`:** likewise require completed worker joins and `Process/GW` destruction.

**Concrete risk:** the specified abnormal-exit path deliberately cannot establish those startup prerequisites. An implementation must therefore either breach the unconditional startup rule to produce the required final record, or omit the finalisation explicitly assigned to it. Process termination is not evidence that C++ destructors or joins completed.

**Required amendment:** distinguish normal cleanup sequencing from abnormal-exit evidence finalisation. Explicitly permit the same sole finalizer to produce an `INCONCLUSIVE` record after confirmed process-tree termination/log closure when joins or destruction were incomplete or unavailable. Separately state the cleanup eligibility for that path, retaining all journal, identity and continuous-lease gates—or explicitly prohibit destructive calls there. Do not treat termination as successful join/destruction. Align the cited SPEC and manifest clauses.

## 3. Safeguards and optional advisories

**Safeguards remain intact at documentary level:**
- Finalizer construction of `Process/GW`, candidate creation/opening, production message traffic, broad/speculative cleanup and retry after ambiguous removal remain prohibited.
- Lease loss or uncertainty, journal ambiguity and identity mismatch prohibit destructive calls and preserve residue; ambiguous intent/outcome records cannot establish removal or trigger automatic retry.
- B2/B3 protections remain: dedicated-UID continuous exclusion, diagnostic suppression of all three pinned failure-path unlinks, retained/replayed journal and raw inventories, mapping evidence, and evidence-derived classification. These are requirements, not implementation or runtime proof.

**Optional r09 advisories remain non-blocking:** tighten mapping-evidence representation; separate historical fixture counts from newly required cases and remove duplicated policy entries; clarify that journal bytes do not independently prove historical fsync calls. No unchanged r07 checker findings are reopened.

## 4. Authority boundary

The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED**. **No operational authority is granted**: no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM/SSH, D3, cleanup, commit, push or release.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-v1.14-r11-20261012.md; SHA-256 9ef48bdfafbfd9818613d9356f9b011972f6efc0af1d2433f7a039c3ba260ed7; 4981 bytes =====
## Verdict: AMEND — documentary only

Performed the single review authorised by owner message **1559215860005081219**, for packet SHA-256 `feb69f164ec879e1b567c44f83e7e56811d2a347fbe904a16337a7a8e3bad829`.

Reviewed only the supplied text; no tools, hash recomputation, code execution or runtime validation. **The original join/destruction prerequisite conflict is resolved, but two related contradictions remain.**

## Must-fix blockers

### B1 — Continuous lease remains an unconditional prerequisite for abnormal finalisation

**Locations**
- Manifest `sandbox.finalizer_ipc_boundary.preconditions.both_modes`, repeated under `semantic_finalizer_delta.finalizer_ipc_boundary`: requires a “valid dedicated non-login UID launcher and continuous lease”.
- `sandbox.finalizer_mode_selection`: otherwise permits abnormal evidence-only finalisation after confirmed reaping/log closure when normal eligibility—including continuous lease—fails.
- §7.1 normal/abnormal sequence: an uncertain lease selects no-cleanup handling, followed by the evidence-only finalisation contract.

**Conflict:** after lease loss, confirmed termination and closed logs are insufficient to satisfy the explicit *both-modes* prerequisite. The contract therefore both offers abnormal finalisation and prohibits invoking it.

**Required amendment:** distinguish the trusted-supervisor/evidence prerequisites from continuous exclusion required for IPC inspection and cleanup. Explicitly define whether lease loss permits evidence-only finalisation, and under what file-safety conditions. If it instead prevents finalisation, consistently require pending/partial evidence and `BLOCKED/INCONCLUSIVE`, rather than promising a final record. Retain the prohibition on IPC operations and automatic recovery cleanup.

### B2 — Recovery after a cleanup attempt cannot truthfully report `not_started`

**Locations**
- §7.1 normal/abnormal sequence includes “a finalizer failure requiring recovery” among abnormal-mode cases.
- The same clause, `semantic_finalizer_delta.abnormal_evidence_only.cleanup_outcome`, and run-record v5 `abnormal_evidence_only_constraints` unconditionally require `cleanup_outcome=not_started`.

**Conflict:** a normal finalizer can fail after `CLEANUP_INTENT` or a destructive call. A subsequent evidence-only recovery record would have to misreport historical cleanup as never started, or breach its schema. Performing no cleanup **during abnormal finalisation** does not establish that no earlier cleanup occurred.

**Required amendment:** either restrict this abnormal-mode contract to runs with no prior cleanup attempt and specify a separate pending/recovery disposition, or permit an evidence-derived historical cleanup outcome for recovery. Preserve ambiguity, prohibit retries, and keep recovery `INCONCLUSIVE`.

## Bounded findings otherwise satisfied

- **Normal gating:** exactly one durable `RUN_SHUTDOWN_COMPLETE`, after joins/destruction and before cleanup intents, plus verified supervisor wait/log evidence is expressly required.
- **Supervisor binding:** the semantic contract requires exact recorded PID-set equality, unique per-PID raw wait receipts, decoded termination states and hashes for present closed logs. JSON Schema alone does not enforce these cross-artifact checks.
- **Mode selection:** expressly evidence-derived, not caller-controlled, subject to B1 above.
- **Abnormal restrictions:** only `INCONCLUSIVE`; no IPC inspection, cleanup or journal mutation. Present but unverified post/final files remain artifacts, not verified inventories.
- **Schema representation:** the shown nullable reservation fields and journal v2 event support the amendment without dummy resource values. Full observation-field compatibility cannot be independently established from omitted, digest-only schema portions. PASS/FAIL remain contractually dependent on complete normal evidence.
- **Bundle accounting:** eight required evidence artifacts, one optional diagnostic, and the separate run record correctly give a ten-file maximum. Missing-artifact semantics exclude optional diagnostics and present-but-unverified files.
- **B2/B3 safeguards from prior reviews:** retained at documentary level; no new false-PASS route is established by the supplied changes.

## Optional advice

Scope the generic strict-parsing and “eight artifacts exactly once” checks explicitly to normal eligibility: malformed or absent evidence must remain preservable in abnormal mode, not prevent its permitted `INCONCLUSIVE` record.

Journal bytes alone do not independently prove historical fsync completion. Reported local validation results are supplied claims, not validation performed in this review.

## Authority boundary

The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED**. This verdict grants **no operational authority**: no implementation, configure/build, harness/finalizer execution, runtime/IPC, namespace, VM/SSH, D3, cleanup, commit, push or release.

===== FILE: Specs/RESULTS-SPEC-039/astra-review-v1.14-r12-20261013.md; SHA-256 6f7228f955475d5fbad6cc2c3508c960c86e264f49c1037f82f4096b769d4e97; 4804 bytes =====
## 1. VERDICT: AMEND — documentary scope only

Reviewed the supplied r12 packet identified as SHA-256 `a88434f223c3b66635d2589140477c01c5142ae94ba0bb839d026c3fde8013bd`, with the supplied full SPEC and manifest as consistency context.

**r11 B1 is closed at documentary level:** both modes share the lease, supervisor and result-directory authority gate; authority loss blocks entry/re-entry and preserves pending evidence. Bounded abnormal raw preservation is substantially addressed, and r08 B2/B3 protections remain specified. **Truthful cleanup-history classification still has the contradictions below.**

## 2. MUST-FIX blockers

### B1 — The supplied cleanup classifier accepts an invalid transaction as `not_started`

**Locations**
- `verify_spec039_v14_r12.py`, `classify()`: shared `pending` set, `REMOVED`/`CLEANUP_FAILED` handling, and `intent_count == 0` return.
- `precode-manifest-v1.14-r12.json`, `local_validation_this_freeze.cleanup_history_outcome_fixtures`.
- Primary packet, `validation_claims.cleanup_history_outcome_fixtures`.
- SPEC §7.1, bounded regression rules requiring malformed/conflicting history to yield `unknown`.

**Concrete contradiction:** by inspection, the following stream, using the supplied schema-valid event shape and consecutive sequence/timestamp values, returns `not_started`:

```text
JOURNAL_OPENED
CREATE_INTENT(resource A)
RUN_SHUTDOWN_COMPLETE
REMOVED(resource A)
JOURNAL_CLOSED
```

`REMOVED` consumes the pending **creation** intent despite there being neither `CREATED` nor `CLEANUP_INTENT`. With no successful create and zero cleanup intents, the final branch returns `not_started`, ignoring the contradictory removal outcome. The classifier also lacks a prohibition on creation transactions after shutdown.

This contradicts the claimed rejection of malformed/reordered history and the evidence-derived outcome contract.

**Required amendment:** distinguish creation and cleanup transaction states, enforce lifecycle phase ordering, and reject orphan or wrong-kind outcomes as `unknown`. Align the bounded classifier contract and validation claims; include these specific negative cases. Invalid history must still permit safe raw preservation and an otherwise authorised `INCONCLUSIVE` record.

### B2 — Normal-finalisation failure rules still exclude a supported `complete` cleanup outcome

**Locations**
- SPEC §7.1, paragraph beginning **“In `normal_cleanup` mode, capture a complete final inventory…”**, and numbered item **5**: after an intent, failure requires `cleanup_outcome=partial` or `unknown`.
- Manifest `sandbox.finalizer_ipc_boundary.failure_rule` and its duplicate under `semantic_finalizer_contract.finalizer_ipc_boundary`.
- Conversely, manifest `semantic_finalizer_contract.normal_cleanup_failure` explicitly permits `partial/complete/unknown`.
- These conflicting clauses are reproduced in the primary packet.

**Concrete contradiction:** all removals can have durable successful outcomes and the journal can be closed, while subsequent evidence validation fails. With common authority intact, cleanup history can support `complete` while overall status remains `INCONCLUSIVE`. The manifest expressly permits that result, but the SPEC’s broader post-intent failure rule excludes it.

**Required amendment:** consistently distinguish destructive-cleanup failure from later finalisation/evidence failure. Permit `complete` when supported by the historical cleanup evidence, without implying PASS, verified final inventory or permission to retry.

**Other requested checks:** no stale unconditional abnormal `not_started` requirement remains in `classification_rules.pass_requires` or `fail_requires`; the abnormal schema includes `complete`. Missing/unavailable enums cover the eight required evidence names and exclude the record and optional diagnostics; exactness and disjointness remain semantic requirements. PID cross-artifact validation is explicitly **NOT EXECUTED**, not falsely reported as passed.

**Optional advice:** consolidate duplicated policy clauses and clearly separate historical fixture counts from current claims.

## 3. Evidence limitation

I used no tools, executed no code or tests, recomputed no hashes, and independently inspected no filesystem, launcher or runtime artefacts. The classifier counterexample is textual reasoning, not an executed probe. Supplied synthetic checks and compile/link-only D1-H evidence do not establish D2 runtime correctness or performance.

## 4. Authority boundary

**No implementation, configure/build, runtime, harness/finalizer, IPC, cleanup, VM/SSH, commit, push or D3 authority is granted.** The launcher remains **NOT_PROVISIONED** and D2 **BLOCKED** pending the separate prerequisites and owner decision. This review authorises no follow-up call.

===== END OF PLAN-ONLY EVIDENCE BUNDLE =====