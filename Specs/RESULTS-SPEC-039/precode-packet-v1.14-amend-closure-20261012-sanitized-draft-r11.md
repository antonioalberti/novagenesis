# SPEC-039 v1.14 — r11 response to Astra r10 abnormal-finalization B1

Status: FROZEN CANDIDATE, UNSUBMITTED. The owner authorized preparation of this same-SPEC documentary response. Submission requires fresh consent bound to the exact packet SHA after freeze. No code, configure/build, harness/finalizer execution, IPC, namespace, VM/SSH, D3, cleanup, commit, push or release authority.

## Frozen identities
- AIOPT5 document HEAD: `756f493b7049434ab67cf2382486a48f606a470c`; tree: `9adf665c0eb06213a29a6c12d650451948c7a4ab`.
- Functional source baseline: `1af604dad66148618cc0e578c28adb4a9829e607`; tree: `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`.
- SPEC-039 v1.14 SHA-256: `bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609`.
- Full manifest v1.14/r11 SHA-256: `c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b`.
- Prior immutable r10 packet SHA-256: `55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146`; SPEC/manifest SHA-256: `ad32c0b54548f13aeae4f19a660f74f5e93ace65e81f5e1fef054b6946158623 / `5707467a0c4e41d254dfe64c30b5d34b74da165bb755f9d8d792f209fda28a13`.
- Pinned baseline source hashes: `{"Common/src/GW.cpp": "7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e", "Common/src/Process.cpp": "bd45b77db19adf7a3c0f78bf5970996afce9720b77ea15519f5a4bcf703845d1"}`.
- Working-tree status (context only):
```
M Specs/SPEC-039-common-performance-measurement.md
 M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md
?? Specs/RESULTS-SPEC-039/
?? Specs/RESULTS-SPEC-059/
```

## Exact r10 verdict and bounded request
The owner authorized one documentary review of exact packet r10; that consent is consumed. Astra returned `AMEND`: r09's IPC prohibition is resolved, B2/B3 remain intact, but abnormal termination requires a final `INCONCLUSIVE` record even when normal joins/destruction did not complete, while the r10 manifest required those normal prerequisites unconditionally. The r10 response is embedded verbatim below.

Review whether this same-SPEC v1.14 amendment closes that single blocker while preserving B2/B3 and no-false-PASS behavior. Specifically assess: (1) whether normal cleanup requires a durable `RUN_SHUTDOWN_COMPLETE` journal event after all worker joins and `Process/GW` destruction, plus exact supervisor process wait/log evidence; (2) whether `supervisor-termination.json` binds the complete recorded PID set to raw wait statuses and closed-log hashes; (3) whether `normal_cleanup` and `abnormal_evidence_only` mode selection is evidence-derived rather than caller-controlled; (4) whether abnormal mode can only emit `INCONCLUSIVE`, lists only actually absent names from the eight required evidence artifacts (not optional diagnostics), preserves present but unverified post/final files without treating them as inventories, and cannot inspect/mutate IPC or cleanup; (5) whether run-record v5 and journal-event v2 schemas can represent this without inventing reservation/observation values, while PASS/FAIL still require the full normal evidence set; (6) whether eight required runtime evidence artifacts plus at most one optional diagnostic artifact (nine excluding run-record.json; ten files total including it) and missing-artifact semantics agree everywhere; and (7) any remaining must-fix contradiction or concrete false-PASS/safety risk.

Do not infer that the launcher exists, the receipt is implemented, or any runtime/finalizer/IPC behavior has been tested. The launcher remains NOT_PROVISIONED and D2 BLOCKED. A documentary verdict grants no implementation or execution authority. Return concise UK English: verdict APPROVE/AMEND/HOLD (documentary only), blockers with precise packet locations or none, optional advice separately, and an explicit no-operational-authority statement.

## Full r10 Astra response — immutable prior evidence
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

## Relevant SPEC clauses — exact current text
Path: `Specs/SPEC-039-common-performance-measurement.md`; full SHA-256 pinned above.

### Current proposed artifact/finalizer-mode row
```markdown
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No modifications to NGAL, `Process`, `PGCS` or `PG`, no algorithm change, separate source helper file, or D3 timing files are proposed. The D2 harness may construct the existing `Process/GW` under test; the same allowlisted harness binary's `--finalize` mode runs after the test exits without constructing `Process/GW`, creating IPC resources, or sending/receiving production messages. In `normal_cleanup` mode, it requires a durable `RUN_SHUTDOWN_COMPLETE` journal event proving worker joins and `Process/GW` destruction, plus a validated required `supervisor-termination.json` artifact containing one raw `waitpid` receipt per recorded process and stdout/stderr closure state/hashes; only then may it read inventories, inspect exact identities, and perform journal-authorized `IPC_RMID`/`sem_unlink` under the continuously verified lease. In `abnormal_evidence_only` mode, after the supervisor has reaped every recorded process and closed both logs, the finalizer may read pending/journal/already-closed evidence files only to preserve/hash available content and report availability, then write a final `INCONCLUSIVE` record. `failure.missing_artifacts` lists only required files actually absent. A present but unverified post/final inventory file may be preserved, but does not make its inventory summary available or empty. No IPC inspection, cleanup, or journal mutation is permitted; joins and `Process/GW` destruction are never claimed. If termination or log closure is unconfirmed, do not invoke finalization or release the lease. The trusted dedicated-UID launcher/lease is a provisioned external runtime prerequisite, not part of the five-file code allowlist; it does not exist here and direct host-UID execution is prohibited. |
```

### §7.1 abnormal-ownership exit clause (current exact text)
```markdown
If any target pop/Run, delete flag, count, or identity makes ownership uncertain, never dereference, erase, delete, or invoke the C++ Process/GW destructors on a possibly stale pointer: write a bounded/fsynced `run-record.pending.json` with `record_finalized=false` and no fabricated measurements, exit nonzero via `_Exit`, and let the isolated process/namespace terminate. The trusted supervisor must wait/reap every PID in the frozen harness process tree, close stdout/stderr, and write/fsync the required `supervisor-termination.json` containing exact per-PID raw `waitpid` statuses, log-closure state, and hashes of available closed logs. Only after every PID is reaped and both logs are closed may the parent-side finalizer in item 5 run `abnormal_evidence_only`: it writes a final `INCONCLUSIVE` record with unavailable post/final inventories and every missing artifact explicit, leaves the journal byte-for-byte as found, and performs no IPC inspection or cleanup. Absence of `RUN_SHUTDOWN_COMPLETE` forbids normal cleanup; process exit does not prove worker join or `Process/GW` destruction. A partial construction, blocked join, timeout, signal, or uncertain ownership follows this same no-cleanup path. If termination receipts or log closure are incomplete, do not invoke the finalizer or release the lease; keep the pending record and classify the gate `BLOCKED/INCONCLUSIVE`.
```

### §7.1 normal and abnormal finalizer sequence (current exact text)
```markdown
After Gateway/output workers stop and join and `Process/GW` are destroyed, the harness must append/fsync exactly one `RUN_SHUTDOWN_COMPLETE` event to the still-open reservation journal. This event is the positive proof that normal shutdown completed; a caller boolean or process exit is not a substitute. The trusted supervisor then waits/reaps every recorded harness process PID and closes stdout/stderr before invoking the same allowlisted `--finalize` binary. It selects `normal_cleanup` only when the journal contains exactly one valid shutdown-complete event, every recorded process has a verified wait receipt, both logs are closed, no timeout/signal/ambiguous termination occurred, and the lease remains continuously valid. In this mode the finalizer captures post inventory, validates/replays the journal, reads required inventories, inspects exact current identities (`shmctl(..., IPC_STAT, ...)` for SysV SHM and `stat`/equivalent metadata for the exact POSIX semaphore object), and removes only journal-proven run-created resources. For each resource it appends/fsyncs `CLEANUP_INTENT`, rechecks lease and identity immediately before the destructive call, invokes only exact `shmctl(exact_shmid, IPC_RMID, ...)` or `sem_unlink(exact_recorded_name)`, then appends/fsyncs the matching outcome. A missing/duplicate shutdown event, incomplete wait receipt, open log, timeout, signal, uncertain lease or ambiguous journal selects no-cleanup handling; no ambiguous deletion is retried.

In `abnormal_evidence_only` mode, only after the finalizer validates `supervisor-termination.json` and confirms every recorded process was reaped and both logs were closed, the same finalizer may read the pending run record, journal bytes and already-closed evidence artifact files solely to preserve/hash available content and report availability, then write a final run record with `record_finalized=true`, `status=INCONCLUSIVE`, a specific failure code/phase, accurate inventory availability, and `failure.missing_artifacts` equal to the required runtime evidence-artifact names actually absent (excluding `run-record.json`, which is the record being written, and excluding optional `harness-diagnostics.txt`, whose absence is not a failure). `supervisor-termination.json` is mandatory to invoke either finalization mode; if it is missing, do not invoke the finalizer, do not release the lease, and preserve the pending/partial evidence. An existing but partial, stale, or shutdown-unverified `ipc-post.json` or `ipc-final.json` may be preserved and hashed as a file, but must not be treated as a verified inventory: the run-record post/final inventory summaries remain unavailable, with `is_empty=null` and no claimed inventory digest. Do not label a present file as missing. The finalizer must not inspect `/proc/sysvipc` or `/dev/shm`, call `IPC_STAT`, `IPC_RMID` or `sem_unlink`, append/close/repair the reservation journal, or perform any cleanup; existing residue and ambiguous journal bytes are preserved for owner-directed recovery. `cleanup_outcome` is `not_started`, cleanup/normal-shutdown fields remain false or null unless directly evidenced, and unavailable post/final inventories are never represented as empty. Partial construction, `_Exit` on uncertain ownership, blocked join, signal, timeout, missing `RUN_SHUTDOWN_COMPLETE`, or a finalizer failure requiring recovery cannot PASS or FAIL through this mode. Process-tree termination and log closure are required to run this mode, but do not prove worker joins or `Process/GW` destruction. If the supervisor cannot verify every recorded PID is reaped or both logs closed, it must not invoke finalization or release the lease; retain the pending/partial evidence and report `BLOCKED/INCONCLUSIVE`.

In `normal_cleanup` mode, capture a complete final inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required runtime artifact exactly once. The finalizer strictly parses journal/inventories, recomputes event order/transitions, raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. PASS/FAIL require the complete bundle and safe normal cleanup; missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. In `abnormal_evidence_only` mode, do not fabricate or require post/final inventory artifacts; list in `failure.missing_artifacts` exactly the actually absent names from the required runtime evidence-artifact set (excluding the run record itself and optional `harness-diagnostics.txt`, whose absence is not a failure). The supervisor receipt is required to invoke finalization; if it is absent, leave the run pending and do not release the lease. A present but unverified post/final inventory file may be preserved and hashed as an artifact, but its inventory summary remains unavailable (`available=false`, `is_empty=null`, `sha256=null`); preserve actual pre-inventory availability and always finalize `INCONCLUSIVE`. This mode never claims cleanup or normal shutdown. D2 remains one controlled correctness run only; no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.
```

### §7.1 run-record/finalizer schema contract (current exact text)
```markdown
5. Bounded run-record and dual-mode finalizer contract: `tests/spec039/q0_q1_d2.manifest.json` defines strict Draft 2020-12 schemas for the pending/final run record v5, `supervisor-termination.json` receipt v1, IPC inventories, observer events and `reservation-journal.jsonl` events v2. The finalized run record's `finalization_mode` is exactly `normal_cleanup` or `abnormal_evidence_only`; pending records use null, and the finalizer derives the chosen mode from validated journal and supervisor evidence rather than a caller label. The same allowlisted `--finalize` binary is the only semantic authority for cross-artifact consistency. It must strictly parse JSON/JSONL (reject duplicate keys, non-finite values, non-canonical base64, partial journal lines and sequence gaps), validate the `supervisor-termination.json` schema and file hash; require its unique `wait_receipts[].pid` set to equal `execution_identity.process_tree_pids`; decode each raw `waitpid` status and verify every recorded PID is reaped; require stdout/stderr closed flags and verify each available log hash against the corresponding artifact, check dedicated-UID lease evidence, recompute raw `/proc/sysvipc` digests/normalized rows, validate returned `sem_t*` mapping evidence, correlate observers, and derive outcome from bytes rather than labels. The journal schema includes exactly one `RUN_SHUTDOWN_COMPLETE` event only after Gateway/output workers have stopped and joined and `Process/GW` has been destroyed; it precedes all cleanup intents. `normal_cleanup` is eligible only when that event is durable, all recorded harness PIDs have verified wait receipts, logs are closed, and the lease is valid. In that mode PASS requires all Q0/Q1 predicates, complete exact-create receipts, all journal events/fsyncs, safe stop/join/destruction, exact cleanup receipts and final raw/parsed inventories equal to pre. FAIL is permitted only for a directly observed correctness assertion failure with complete evidence and safe normal cleanup. In `abnormal_evidence_only` mode, entered only after all recorded harness PIDs are reaped and both logs are closed, the same finalizer writes a finalized run record with `status=INCONCLUSIVE`, accurate failure/missing-artifact/inventory-availability fields, and no fabricated measurements. It must not inspect or mutate IPC, perform cleanup, repair/close the journal, or claim joins/destruction; `cleanup_outcome=not_started`, post/final inventories are unavailable rather than empty, and residue is preserved. This mode is mandatory for partial construction, `_Exit`, blocked join, timeout/signal, and missing shutdown event. If termination receipts/log closure are incomplete, do not invoke finalization or release the lease; retain the pending record. No semantic finalizer exists or was executed; this is a future documentary contract only.
```

### §7.1 new abnormal-mode negative cases and bundle policy (current exact text)
```markdown
The bounded semantic regression set must reject claimed PASS/FAIL and produce `INCONCLUSIVE` for: missing/unverified dedicated-UID launcher or lease; a second/foreign process entering the run identity; any diagnostic `sem_open` failure path that calls `sem_unlink`; pre-existing candidate key/name; unreadable, over-cap or raw/parsed-mismatched `/proc/sysvipc` inventory; non-canonical base64; missing, partial, reordered, duplicated or unfsynced journal events; crash after cleanup intent without a durable outcome; failed exclusive reservation; candidate identity/mode mismatch; missing or mismatched returned-`sem_t` mapping; residual candidate; unexpected non-candidate change; or final inventory mismatch. Additional negative fixtures must reject a missing/duplicate/misordered `RUN_SHUTDOWN_COMPLETE`; `normal_cleanup` without complete supervisor wait receipts or closed logs; invoking either mode before every recorded PID is reaped; any IPC/cleanup/journal-repair attempt in `abnormal_evidence_only`; any PASS/FAIL, empty-inventory claim, join/destruction claim, or unlisted missing artifact in abnormal mode. The finalizer must perform no speculative deletion for disputed/unproven resources. PASS and safe FAIL fixtures require a complete, replayable journal and all required evidence. For normal PASS/FAIL runs, the runtime bundle contains nine required files: `run-record.json`, `stdout.txt`, `stderr.txt`, `ipc-pre.json`, `ipc-post.json`, `ipc-final.json`, `observer-events.json`, `reservation-journal.jsonl`, and `supervisor-termination.json`; only `harness-diagnostics.txt` is optional, so the normal total-file cap is ten. Abnormal evidence-only INCONCLUSIVE records may omit unavailable artifacts and must enumerate them in `failure.missing_artifacts`; the same per-file and total-byte caps apply to files that are present. Each artifact remains ≤4 MiB; the journal is additionally capped at 256 KiB/128 events, the final run record at 256 KiB, total bundle at 32 MiB. Raw table bytes are retained within each IPC inventory JSON as canonical base64 and count toward that artifact's 4 MiB cap. Any overflow is incomplete evidence and `INCONCLUSIVE`.
```

### §7.1 process-tree/runtime boundary and next gate (current exact text)
```markdown
The host-controlled runtime shape remains one controlled D2 process tree under the dedicated UID, one Q0/Q1 correctness run, no network input and no performance timing. The launcher serializes the run and finalizer under the verified exclusive lease; the harness itself spawns no child process. The host IPC namespace may contain unrelated objects, but all candidate resources must be absent before reservation and all exact cleanup remains bound to the durable journal. No broad host cleanup is permitted. If the launcher/lease is absent or lost, a second process enters the identity domain, the journal is incomplete, or ownership is ambiguous, do not perform name-based unlink/RMID on disputed objects; classify `INCONCLUSIVE`/`BLOCKED` and preserve residue for owner-directed recovery.

**Next gate:** validate the v1.14 SPEC/r11 manifest and packet against the r10 failure-finalization finding: v5 run-record/v2 journal schemas; durable `RUN_SHUTDOWN_COMPLETE` before normal cleanup; required `supervisor-termination.json` with exact PID wait receipts and closed-log hashes; abnormal evidence-only `INCONCLUSIVE` after confirmed termination/log closure; zero IPC inspection/destructive cleanup in abnormal mode; no fabricated inventories, joins or destruction. The r10 one-call consent is consumed; obtain fresh owner consent bound to the exact r11 packet SHA before Astra. D2 implementation/configure/build/run remains blocked until positive review, provisioned launcher/preconditions and separate owner execution gate. D3 and Q2–Q4 remain deferred. This documentary work is not correctness or performance evidence.
```

Focused-scope note: §7.1 queue-observation and compile/link-checker detail is unchanged from r10 and omitted; this packet does not claim review of those unchanged clauses. The full SPEC and manifest hashes above bind the complete documents.

## Machine-readable r11 manifest projection
The complete manifest is pinned above. This projection includes the full new supervisor schema, full journal-event schema v2, every changed run-record v5 branch/property, current finalizer modes, artifact counts/caps and relevant classification rules. Unchanged r10 run-record schema portions are identified by canonical digests and are not represented as a full-schema review.
```json
{
  "classification_delta": {
    "abnormal_evidence_only_outcome": {
      "allowed_file_operations": [
        "read pending run record, immutable journal bytes and already-closed artifact files for hashing/availability",
        "write/fsync one final INCONCLUSIVE run record"
      ],
      "forbidden": [
        "read /proc/sysvipc or /dev/shm",
        "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
        "append/repair/close reservation journal",
        "cleanup, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
      ],
      "required": [
        "all process_tree_pids have one valid raw waitpid receipt and are reaped",
        "stdout/stderr closed, with hashes for files that exist"
      ],
      "result": "final run-record status INCONCLUSIVE, finalization_mode abnormal_evidence_only, failure.cleanup_outcome not_started, unavailable post/final inventories and missing artifacts explicit"
    },
    "artifact_names_unique": "Exactly one of each eight required runtime evidence artifacts; optional harness-diagnostics.txt at most once; run-record.json is separately required. Abnormal evidence-only INCONCLUSIVE records list only actually absent names from those eight evidence artifacts; a present unverified post/final inventory file may be retained while its inventory summary stays unavailable. supervisor-termination.json is mandatory to invoke finalization.",
    "normal_cleanup_eligibility": {
      "allowed": "Only the exact normal-mode operations in allowed_operations, using journal-proven resources and immediate lease/identity recheck.",
      "required": [
        "exactly one durable RUN_SHUTDOWN_COMPLETE after all worker joins and Process/GW destruction",
        "supervisor-termination.json hash/schema valid; wait_receipts PID set exactly equals recorded process_tree_pids",
        "all raw wait statuses decode as reaped normal exits; timeout_fired=false",
        "stdout/stderr closed and present-file hashes match",
        "continuous dedicated-UID lease"
      ]
    },
    "required_artifact_names": [
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
  "collection_responsibility": {
    "artifact_name_uniqueness": "Eight required runtime evidence artifact names are unique; optional harness-diagnostics.txt occurs at most once; run-record.json is separately required. In abnormal mode, missing_artifacts lists only absent names from the evidence-artifact set; present but unverified inventory files may be retained while their summary remains unavailable. supervisor-termination.json is mandatory before finalizer invocation.",
    "finalizer_semantic_authority": "Same allowlisted --finalize binary selects modes from the durable journal and validated supervisor receipt. Normal mode replays evidence and performs exact identity-bound cleanup. Abnormal evidence-only mode emits only INCONCLUSIVE, does not inspect IPC or mutate journal, and preserves residue.",
    "harness_run": "Only after dedicated UID launcher/lease verified: create pending record/journal, create/reserve candidates and collect evidence. After all worker joins and Process/GW destruction, append/fsync one RUN_SHUTDOWN_COMPLETE before any cleanup intent. The supervisor later emits supervisor-termination.json after process wait/log closure.",
    "outer_finalizer": "The same allowlisted --finalize binary requires the supervisor-termination.json receipt for either mode. Normal mode additionally requires RUN_SHUTDOWN_COMPLETE before exact cleanup. Abnormal mode requires all process PIDs reaped and logs closed, then writes only final INCONCLUSIVE evidence; no IPC inspection or cleanup.",
    "supervisor_termination_receipt": "Trusted launcher writes/fsyncs supervisor-termination.json after waiting/reaping every recorded process PID and closing stdout/stderr. The finalizer binds the receipt PID set to execution_identity.process_tree_pids, decodes raw waitpid statuses, checks closed-log hashes and validates launcher/lease identity."
  },
  "local_validation": {
    "abnormal_mode_negative_probes": "PASS: schema rejected PASS claim, cleanup claim, optional diagnostics listed as missing and empty wait receipt; set-equality probe detected omitted missing evidence; RUN_SHUTDOWN_COMPLETE order checked",
    "abnormal_run_record_positive_instance": "PASS: finalized INCONCLUSIVE record accepted with unavailable inventories and present but unverified ipc-post/ipc-final files; missing_artifacts exactly matched absent evidence names",
    "astra_submitted": false,
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only; launcher/lease is NOT_PROVISIONED; D2 blocked. Fresh exact-SHA consent for r11 not yet requested or granted.",
    "ipc_or_vm_activity": false,
    "json_duplicate_key_check": "PASS: strict JSON parse",
    "json_schema_meta_validation": "PASS: five Draft 2020-12 JSON Schemas meta-validated; collection_responsibility is manifest policy data, not a schema",
    "manifest_generation": "PASS: created r11 from immutable submitted r10 manifest; r10 packet/manifest/response/raw/receipt preserved",
    "owner_exact_sha_consent_for_r11": false,
    "runtime_finalizer_execution": false,
    "spec_sha256": "bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609",
    "supervisor_pid_set_cross_artifact_validation": "CONTRACT_ONLY_NOT_EXECUTED: requires semantic finalizer, not JSON Schema",
    "supervisor_receipt_positive_instance": "PASS: schema-valid receipt with raw waitpid status accepted"
  },
  "manifest_sha256": "c7d9598d1827b6ee91ab0e6ad9d15ebeaa04e73ddf181b363eb5f2f04f1e589b",
  "prior_v1_journal_schema_canonical_sha256": "e8b61f9f6e2fab26dff581d685b9777d87f110520b3fa755a745b7ff27841a37",
  "r10_review": {
    "manifest_sha256": "5707467a0c4e41d254dfe64c30b5d34b74da165bb755f9d8d792f209fda28a13",
    "must_fix": "Separate normal cleanup (durable shutdown event plus wait/log evidence) from abnormal evidence-only finalization (confirmed process termination/log closure, INCONCLUSIVE, no IPC inspection or destructive cleanup).",
    "one_call_consent_consumed": true,
    "packet_sha256": "55141aab788839b0346d7ec1bda46c9e4cf86e9d33bc64ce67e242fcb2ff7146",
    "raw_sse_path": "Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.raw.sse",
    "raw_sse_sha256": "4c35170e7d37953daf9bbc7082e0e92cd73b6b45067521a151c3e7654d6f2704",
    "response_path": "Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012.md",
    "response_sha256": "6aa0669c3cec1b11c2516d6c361d78460663cde9f097cc4c5de8264557e64c2e",
    "spec_sha256": "b4e1c0c2db88a0d1a8f02ab98c934b7b33ec23fd2162c8e1b2921b72afbaf88d",
    "submission_receipt_path": "Specs/RESULTS-SPEC-039/astra-review-v1.13-r10-20261012-submission.json",
    "submission_receipt_sha256": "046afc37a36e7441ee341d6c663f3718fa0bf8ca76717cc98791f58f95373ae8",
    "verdict": "AMEND — documentary scope only; the r09 IPC boundary is resolved and r08 B2/B3 remain intact; sole new B1 is abnormal finalization conflicting with unconditional normal shutdown prerequisites"
  },
  "reservation_journal_event_schema_v2": {
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
  "review_scope": "Same-SPEC v1.14 documentary response to the sole r10 abnormal-finalization B1. No code, configure/build, harness/finalizer execution, IPC, D3, commit or push. Fresh exact-SHA owner consent is required for one r11 Astra review. D2 remains blocked until the dedicated launcher/lease is provisioned and separately authorized. The normal cleanup mode is gated by RUN_SHUTDOWN_COMPLETE and the supervisor receipt. The abnormal mode must not issue IPC calls or claim joins/destruction.",
  "run_record_schema_delta_v5": {
    "$id": "urn:ng039:run-record:v5",
    "abnormal_reservation_nullable_fields": {
      "candidate_keys": {
        "items": {
          "maximum": 2147483647,
          "minimum": 1,
          "type": "integer"
        },
        "maxItems": 5,
        "minItems": 0,
        "type": "array",
        "uniqueItems": true
      },
      "d2_process_key": {
        "maximum": 2147483643,
        "minimum": 1,
        "type": [
          "integer",
          "null"
        ]
      },
      "exclusive_reservation_complete": {
        "type": [
          "boolean",
          "null"
        ]
      },
      "journal_fsynced": {
        "type": [
          "boolean",
          "null"
        ]
      },
      "libc_version": {
        "maxLength": 64,
        "minLength": 4,
        "type": [
          "string",
          "null"
        ]
      },
      "reservation_umask": {
        "enum": [
          63,
          null
        ],
        "type": [
          "integer",
          "null"
        ]
      },
      "selected_attempt": {
        "maximum": 31,
        "minimum": 0,
        "type": [
          "integer",
          "null"
        ]
      },
      "semaphore_name_mapping_verified": {
        "type": [
          "boolean",
          "null"
        ]
      }
    },
    "artifact_maxItems": 9,
    "artifact_name_enum": {
      "enum": [
        "stdout.txt",
        "stderr.txt",
        "ipc-pre.json",
        "ipc-post.json",
        "ipc-final.json",
        "observer-events.json",
        "reservation-journal.jsonl",
        "harness-diagnostics.txt",
        "supervisor-termination.json"
      ]
    },
    "failure_missing_artifacts": {
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
    "finalization_conditionals": {
      "PASS_FAIL_mode_and_reservation_delta": {
        "1": {
          "finalization_mode": {
            "const": "normal_cleanup"
          },
          "full_branch_sha256": "3fdb702537a3787f5f03ab01cfee18e8d127de8fbe3fc6049317f44b673d2312",
          "new_reservation_pins": {
            "candidate_keys": {
              "maxItems": 5,
              "minItems": 5,
              "type": "array"
            },
            "d2_process_key": {
              "maximum": 2147483643,
              "minimum": 1,
              "type": "integer"
            },
            "exclusive_reservation_complete": {
              "const": true
            },
            "reservation_umask": {
              "const": 63
            },
            "selected_attempt": {
              "maximum": 31,
              "minimum": 0,
              "type": "integer"
            }
          }
        },
        "2": {
          "finalization_mode": {
            "const": "normal_cleanup"
          },
          "full_branch_sha256": "058f1c9b4c024846b9796ec65e5bc8446ae11ceeb72b77b792144978871850f4",
          "new_reservation_pins": {
            "candidate_keys": {
              "maxItems": 5,
              "minItems": 5,
              "type": "array"
            },
            "d2_process_key": {
              "maximum": 2147483643,
              "minimum": 1,
              "type": "integer"
            },
            "exclusive_reservation_complete": {
              "const": true
            },
            "reservation_umask": {
              "const": 63
            },
            "selected_attempt": {
              "maximum": 31,
              "minimum": 0,
              "type": "integer"
            }
          }
        },
        "4": {
          "finalization_mode": {
            "const": "normal_cleanup"
          },
          "full_branch_sha256": "64ef6670648b1e2725825cff1056938877f259267b31f0912912515906fac3ae",
          "new_reservation_pins": {
            "candidate_keys": {
              "maxItems": 5,
              "minItems": 5,
              "type": "array"
            },
            "d2_process_key": {
              "maximum": 2147483643,
              "minimum": 1,
              "type": "integer"
            },
            "exclusive_reservation_complete": {
              "const": true
            },
            "reservation_umask": {
              "const": 63
            },
            "selected_attempt": {
              "maximum": 31,
              "minimum": 0,
              "type": "integer"
            }
          }
        },
        "5": {
          "finalization_mode": {
            "const": "normal_cleanup"
          },
          "full_branch_sha256": "56420075bc61244eec20d134e401af51f0ccffa9554ffc7ee3df5ddeca389c27",
          "new_reservation_pins": {
            "candidate_keys": {
              "maxItems": 5,
              "minItems": 5,
              "type": "array"
            },
            "d2_process_key": {
              "maximum": 2147483643,
              "minimum": 1,
              "type": "integer"
            },
            "exclusive_reservation_complete": {
              "const": true
            },
            "reservation_umask": {
              "const": 63
            },
            "selected_attempt": {
              "maximum": 31,
              "minimum": 0,
              "type": "integer"
            }
          }
        }
      },
      "abnormal_evidence_only_constraints": {
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
                  "const": "not_started"
                }
              },
              "required": [
                "cleanup_outcome",
                "missing_artifacts"
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
      "finalized_record_binds_supervisor_artifact": {
        "if": {
          "properties": {
            "record_finalized": {
              "const": true
            }
          },
          "required": [
            "record_finalized"
          ]
        },
        "then": {
          "properties": {
            "artifacts": {
              "contains": {
                "properties": {
                  "name": {
                    "const": "supervisor-termination.json"
                  }
                },
                "required": [
                  "name"
                ],
                "type": "object"
              },
              "maxContains": 1,
              "minContains": 1
            },
            "finalization_mode": {
              "enum": [
                "normal_cleanup",
                "abnormal_evidence_only"
              ]
            }
          },
          "required": [
            "finalization_mode",
            "artifacts"
          ]
        }
      },
      "pending_record_mode_null": {
        "finalization_mode": {
          "type": "null"
        },
        "full_branch_sha256": "9dd008c8b6a8dd40e8621a28b69d62984b8b75e3a86906c130bd2845566becfb"
      }
    },
    "finalization_mode": {
      "enum": [
        "normal_cleanup",
        "abnormal_evidence_only",
        null
      ],
      "type": [
        "string",
        "null"
      ]
    },
    "full_v5_run_record_schema_sha256": "59e9aade316f74dd9fe7447ffe323cdcec536046e88c6e6750af1bea7a1fd879",
    "prior_v4_run_record_schema_sha256": "0a71723a528da56b83fa3325ac5a30db6f4a328e49393f8b168a19d68ef91f53",
    "schema_version": {
      "const": "ng039-d2-run-record/v5"
    },
    "spec_sha256": {
      "const": "bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609"
    },
    "title": "SPEC-039 D2 v5 normal/abnormal final run record"
  },
  "runtime_bundle": {
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
    ],
    "supervisor_termination_receipt": {
      "abnormal_rule": "signaled/timed-out but fully reaped processes may enter abnormal_evidence_only; no cleanup or live IPC inspection",
      "file_name": "supervisor-termination.json",
      "log_binding": "stdout_closed/stderr_closed are true only after corresponding FDs close; non-null log hashes bind to the closed file artifacts",
      "pid_binding": "process_tree_pids and wait_receipts.pid sets must equal execution_identity.process_tree_pids exactly; no duplicate or omitted PID",
      "producer": "trusted dedicated-UID launcher after wait/reap of the harness process tree and closure of stdout/stderr",
      "required": true,
      "schema_version": "ng039-supervisor-termination/v1",
      "wait_status": "retain raw waitpid status integers; finalizer decodes each status itself, never trusts a normalized success boolean"
    }
  },
  "sandbox": {
    "abnormal_mode_live_inventory_capture": false,
    "cleanup_authority": "Destructive cleanup is permitted only in normal_cleanup after durable RUN_SHUTDOWN_COMPLETE, valid supervisor receipt, closed logs, known non-timeout/non-signal wait statuses, complete journal and immediate identity/lease rechecks. abnormal_evidence_only performs no IPC inspection or destructive call and preserves residue.",
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending run record, immutable journal bytes and already-closed artifact files for hashing/availability",
          "write/fsync one final INCONCLUSIVE run record"
        ],
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "cleanup, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "required": [
          "all process_tree_pids have one valid raw waitpid receipt and are reaped",
          "stdout/stderr closed, with hashes for files that exist"
        ],
        "result": "final run-record status INCONCLUSIVE, finalization_mode abnormal_evidence_only, failure.cleanup_outcome not_started, unavailable post/final inventories and missing artifacts explicit"
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If supervisor receipt, process reaping, log closure, journal, lease or identity is missing/ambiguous, no destructive IPC call; if process termination/log closure is unconfirmed, do not invoke finalizer or release lease. Abnormal evidence mode always INCONCLUSIVE and never retries cleanup.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Derive mode from validated RUN_SHUTDOWN_COMPLETE plus supervisor-termination.json; do not accept a caller-supplied mode claim.",
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
      "owner_and_sequence": "The launcher records process_tree_pids before execution and writes supervisor-termination.json only after reaping every recorded PID and closing logs. The same allowlisted --finalize binary is invoked only after those supervisor conditions hold. It derives normal_cleanup only from the durable shutdown event plus a valid receipt; that mode additionally requires all Gateway/output workers joined and Process/GW destroyed, then validates evidence and performs exact cleanup. Otherwise it may select abnormal_evidence_only, which writes only final INCONCLUSIVE evidence and leaves IPC/journal bytes untouched; no join or destruction is inferred.",
      "preconditions": {
        "abnormal_evidence_only": [
          "finalizer does not require or claim worker joins or Process/GW destruction; if termination/log closure is unconfirmed, do not invoke finalizer or release lease"
        ],
        "both_modes": [
          "valid dedicated non-login UID launcher and continuous lease",
          "schema-valid supervisor-termination.json with exact PID-set/wait/log evidence",
          "every recorded process PID reaped and stdout/stderr closed before finalizer invocation"
        ],
        "normal_cleanup": [
          "exactly one fsynced RUN_SHUTDOWN_COMPLETE after worker joins and Process/GW destruction, before any cleanup intent"
        ]
      }
    },
    "finalizer_mode_selection": "Finalizer derives normal_cleanup only from exactly one durable RUN_SHUTDOWN_COMPLETE journal event, validated supervisor receipt, closed logs, no timeout/signal/ambiguous wait, and continuously valid lease. Otherwise, after all recorded PIDs are reaped and logs are closed, it may choose abnormal_evidence_only; the caller cannot force a mode.",
    "inventory_requirements_by_mode": {
      "abnormal_evidence_only": "No live post/final inventory capture; summaries remain unavailable. Existing closed files may be preserved but are not treated as verified inventories.",
      "normal_cleanup": "Complete pre/post/final inventories are required and compared exactly."
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
    "no_broad_host_cleanup": true,
    "normal_cleanup_full_pre_post_final_inventory_required": true,
    "runtime_finalizer_cleanup": "normal_cleanup: only exact journal-authorized IPC_STAT/IPC_RMID/sem_unlink under continuous lease after RUN_SHUTDOWN_COMPLETE. abnormal_evidence_only: write final INCONCLUSIVE evidence record only; no live IPC inspection, destructive IPC call, journal repair/close or cleanup.",
    "supervisor_termination_receipt": "Required closed artifact supervisor-termination.json v1; contains run_id, pinned launcher/lease identity, exact process_tree_pids, one raw waitpid status per PID, stdout/stderr closure flags/hashes, timeout flag and capture monotonic time. Finalizer requires exact PID-set equality, decodes each wait status, and checks each present log digest."
  },
  "semantic_finalizer_delta": {
    "abnormal_evidence_only": {
      "cleanup_outcome": "not_started",
      "inventories": "post/final summaries unavailable and not empty; a present unverified artifact file may be preserved but is not a verified inventory; preserve actual pre availability",
      "journal_mutation": false,
      "live_ipc_operations": false,
      "missing_artifacts": "failure.missing_artifacts equals the exact absent-name set over the eight required evidence artifacts; supervisor receipt mandatory for finalizer invocation",
      "pending_record": "finalize with explicit failure code/phase and no fabricated measurements",
      "status": "INCONCLUSIVE only",
      "trigger": "partial construction, _Exit, blocked join, timeout/signal, absent/invalid shutdown event or any other non-normal shutdown after supervisor termination/log closure"
    },
    "failure_summary_checks": [
      "failure.code is recomputed from the highest-precedence observed condition; ASSERTION_FAILED is used only for a demonstrated correctness predicate failure after all evidence/safety gates pass.",
      "failure.phase must be the phase mapped to the selected failed predicate or uncertainty condition.",
      "failure.missing_artifacts equals the exact absent-name set over the eight required runtime evidence artifacts, excluding run-record.json; it is not required to list a present but unverified post/final inventory file. Supervisor receipt is mandatory to invoke either finalization mode; FAIL requires an empty list.",
      "failure.inventory_availability is derived from actual inventory-file presence and successful parsing, not caller-provided booleans.",
      "failure.cleanup_outcome derives from stop/join, exclusive reservation receipts, exact unlink/RMID results and final inventory equality; FAIL requires complete.",
      "failure.last_event_sequence equals the greatest validated event sequence, or null only when no valid event exists.",
      "Any contradiction in code/phase/availability/cleanup/missing-artifact fields yields INCONCLUSIVE/EVIDENCE_INVALID."
    ],
    "finalizer_ipc_boundary": {
      "abnormal_evidence_only": {
        "allowed_file_operations": [
          "read pending run record, immutable journal bytes and already-closed artifact files for hashing/availability",
          "write/fsync one final INCONCLUSIVE run record"
        ],
        "forbidden": [
          "read /proc/sysvipc or /dev/shm",
          "IPC_STAT, IPC_RMID, sem_unlink or any IPC operation",
          "append/repair/close reservation journal",
          "cleanup, PASS/FAIL, infer joins/Process/GW destruction, or call unavailable inventories empty"
        ],
        "required": [
          "all process_tree_pids have one valid raw waitpid receipt and are reaped",
          "stdout/stderr closed, with hashes for files that exist"
        ],
        "result": "final run-record status INCONCLUSIVE, finalization_mode abnormal_evidence_only, failure.cleanup_outcome not_started, unavailable post/final inventories and missing artifacts explicit"
      },
      "allowed_operations": [
        "read complete /proc/sysvipc/{shm,sem,msg} table bytes and /dev/shm inventory for the required pre/post/final phases",
        "shmctl(exact_recorded_shmid, IPC_STAT, ...) to inspect the exact SysV shared-memory object identity",
        "stat/equivalent metadata inspection of the exact journal-recorded /dev/shm semaphore object, including device/inode/UID/GID/mode/link count",
        "shmctl(exact_recorded_shmid, IPC_RMID, ...) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass",
        "sem_unlink(exact_recorded_name) only for a complete successful CREATE receipt whose exact current identity and continuous lease recheck pass"
      ],
      "failure_rule": "If supervisor receipt, process reaping, log closure, journal, lease or identity is missing/ambiguous, no destructive IPC call; if process termination/log closure is unconfirmed, do not invoke finalizer or release lease. Abnormal evidence mode always INCONCLUSIVE and never retries cleanup.",
      "forbidden_operations": [
        "construct Process/GW",
        "create/open candidate IPC resources or attach/use them for message traffic",
        "invoke Gateway production IPC/send/receive paths",
        "delete an object not exactly bound to its successful journal CREATE receipt",
        "retry a removal whose durable outcome is ambiguous",
        "broad/speculative host cleanup"
      ],
      "mode_selection": "Derive mode from validated RUN_SHUTDOWN_COMPLETE plus supervisor-termination.json; do not accept a caller-supplied mode claim.",
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
      "owner_and_sequence": "The launcher records process_tree_pids before execution and writes supervisor-termination.json only after reaping every recorded PID and closing logs. The same allowlisted --finalize binary is invoked only after those supervisor conditions hold. It derives normal_cleanup only from the durable shutdown event plus a valid receipt; that mode additionally requires all Gateway/output workers joined and Process/GW destroyed, then validates evidence and performs exact cleanup. Otherwise it may select abnormal_evidence_only, which writes only final INCONCLUSIVE evidence and leaves IPC/journal bytes untouched; no join or destruction is inferred.",
      "preconditions": {
        "abnormal_evidence_only": [
          "finalizer does not require or claim worker joins or Process/GW destruction; if termination/log closure is unconfirmed, do not invoke finalizer or release lease"
        ],
        "both_modes": [
          "valid dedicated non-login UID launcher and continuous lease",
          "schema-valid supervisor-termination.json with exact PID-set/wait/log evidence",
          "every recorded process PID reaped and stdout/stderr closed before finalizer invocation"
        ],
        "normal_cleanup": [
          "exactly one fsynced RUN_SHUTDOWN_COMPLETE after worker joins and Process/GW destruction, before any cleanup intent"
        ]
      }
    },
    "new_artifact_checks": [
      "exact allowlisted file set",
      "optional harness-diagnostics.txt at most once",
      "reject symlink/non-regular files",
      "check byte caps and SHA-256 against actual closed files",
      "check total bundle cap",
      "eight required artifact names exactly once, including reservation-journal.jsonl",
      "strict journal JSONL event schema/order/run-ID validation and file hash verification",
      "verify dedicated UID launcher/lease identity and continuous lease evidence before cleanup",
      "eight required runtime artifact names exactly once, including reservation-journal.jsonl",
      "strict reservation-journal schema/order/run-ID/replay validation and closed-file hashing",
      "dedicated UID launcher/lease identity and continuity verified before cleanup",
      "exactly eight required runtime evidence artifacts exactly once, including reservation-journal.jsonl and supervisor-termination.json; optional harness-diagnostics.txt at most once"
    ],
    "new_inventory_checks": [
      "normal_cleanup requires durable RUN_SHUTDOWN_COMPLETE and supervisor wait/log receipt; abnormal_evidence_only performs no live inventory/IPC read or destructive call and marks post/final summaries unavailable even when unverified artifact files are preserved"
    ],
    "reservation_journal": {
      "binding": "required runtime artifact for normal complete bundle; bytes/size/SHA-256 bound in final run-record when present; final run-record SHA stays in outer archive index",
      "crash_rule": "Without RUN_SHUTDOWN_COMPLETE, never perform cleanup. Abnormal evidence-only mode preserves the journal byte-for-byte and never closes/repairs it.",
      "event_order": [
        "JOURNAL_OPENED",
        "CREATE_INTENT",
        "CREATED|CREATE_FAILED",
        "RUN_SHUTDOWN_COMPLETE",
        "CLEANUP_INTENT",
        "REMOVED|CLEANUP_FAILED",
        "JOURNAL_CLOSED"
      ],
      "file_name": "reservation-journal.jsonl",
      "max_bytes": 262144,
      "max_events": 128,
      "required_behavior": "exclusive mode 0600, append-only strict JSONL, monotone sequence, same run_id, fsync every event and parent directory after create",
      "schema_id": "urn:ng039:reservation-journal-event:v2",
      "schema_version": "ng039-reservation-journal-event/v2",
      "shutdown_event": "Exactly one durable RUN_SHUTDOWN_COMPLETE after all Gateway/output workers joined and Process/GW destroyed; required before normal-cleanup intent."
    }
  },
  "spec": {
    "change_from_v1_11": "Same-SPEC documentary response to r08 B2/B3: block direct ordinary-UID execution; require dedicated single-run UID/launcher/lease; specify diagnostic no-unlink guards for all three GW.cpp sem_open-failure paths; define durable journal, raw inventory retention/recomputation, and returned-sem_t mapping evidence. No implementation/runtime authority.",
    "change_from_v1_12": "Resolve the sole r09 B1 by consistently defining finalizer IPC authority: bounded inventory/current-identity inspection and exact journal-authorized removal under continuous dedicated-UID lease; prohibit Process/GW construction, resource creation, production message traffic, ambiguous retries and broad cleanup. Preserve r09 B2/B3 controls. Documentary only.",
    "change_from_v1_13": "Resolve the r10 abnormal-finalization blocker: add journaled RUN_SHUTDOWN_COMPLETE for normal cleanup, a supervisor-termination.json receipt binding every recorded process PID to raw waitpid status and closed-log hashes, and distinct normal_cleanup/abnormal_evidence_only modes. Abnormal mode emits only INCONCLUSIVE after termination/log-closure proof and performs no IPC inspection or destructive cleanup. Documentary only.",
    "path": "Specs/SPEC-039-common-performance-measurement.md",
    "sha256": "bbb26a1331d832ab2dcbf37f1d243d7b9239e6ff1d1932319ad1141923baa609",
    "status": "In Progress",
    "version": "v1.14"
  },
  "supervisor_termination_schema": {
    "$id": "urn:ng039:supervisor-termination:v1",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "additionalProperties": false,
    "properties": {
      "captured_monotonic_ns": {
        "minimum": 0,
        "type": "integer"
      },
      "launcher_sha256": {
        "pattern": "^[0-9a-f]{64}$",
        "type": "string"
      },
      "lease_device": {
        "minimum": 0,
        "type": "integer"
      },
      "lease_inode": {
        "minimum": 1,
        "type": "integer"
      },
      "lease_path": {
        "maxLength": 1024,
        "minLength": 1,
        "type": "string"
      },
      "process_tree_pids": {
        "items": {
          "minimum": 2,
          "type": "integer"
        },
        "maxItems": 32,
        "minItems": 1,
        "type": "array",
        "uniqueItems": true
      },
      "run_id": {
        "maxLength": 64,
        "minLength": 16,
        "type": "string"
      },
      "schema_version": {
        "const": "ng039-supervisor-termination/v1"
      },
      "stderr_closed": {
        "type": "boolean"
      },
      "stderr_sha256": {
        "pattern": "^[0-9a-f]{64}$",
        "type": [
          "string",
          "null"
        ]
      },
      "stdout_closed": {
        "type": "boolean"
      },
      "stdout_sha256": {
        "pattern": "^[0-9a-f]{64}$",
        "type": [
          "string",
          "null"
        ]
      },
      "timeout_fired": {
        "type": "boolean"
      },
      "wait_receipts": {
        "items": {
          "additionalProperties": false,
          "properties": {
            "pid": {
              "minimum": 2,
              "type": "integer"
            },
            "raw_wait_status": {
              "maximum": 65535,
              "minimum": 0,
              "type": "integer"
            }
          },
          "required": [
            "pid",
            "raw_wait_status"
          ],
          "type": "object"
        },
        "maxItems": 32,
        "minItems": 1,
        "type": "array",
        "uniqueItems": true
      }
    },
    "required": [
      "schema_version",
      "run_id",
      "launcher_sha256",
      "lease_path",
      "lease_device",
      "lease_inode",
      "process_tree_pids",
      "wait_receipts",
      "stdout_closed",
      "stderr_closed",
      "stdout_sha256",
      "stderr_sha256",
      "timeout_fired",
      "captured_monotonic_ns"
    ],
    "title": "SPEC-039 trusted supervisor termination receipt v1",
    "type": "object"
  }
}
```

## Source-grounded lifecycle excerpts from the immutable functional baseline
These source excerpts are unchanged and do not implement the proposed journal/receipt contract. They are included only to ground the stated thread join and Process/GW destruction sequence.

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

## r09 context and preserved boundaries
The r09 review resolved the earlier IPC-scope contradiction. Its optional mapping/fsync/fixture advisories remain non-blocking and are not promoted here. B2/B3 protections must remain intact: dedicated-UID lease, diagnostic no-unlink guards, raw journal/inventory evidence, exact mapping, and no broad cleanup. This packet asks only for the new abnormal-finalization B1.

Packet SHA-256 is computed out of band after these exact bytes are frozen and is intentionally not embedded into itself. Review only the supplied packet; no tools or runtime are requested.
