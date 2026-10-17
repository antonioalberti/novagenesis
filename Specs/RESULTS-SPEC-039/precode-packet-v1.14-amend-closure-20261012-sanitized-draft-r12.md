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
