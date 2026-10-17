# SPEC-039 v1.12 — r09 same-SPEC documentary amendment review

Status: FROZEN candidate, unsubmitted. Owner authorised preparing the same-SPEC documentary amendment for r08 B2/B3 only. This packet has no implementation, build, runtime, IPC, D3, commit, push or release authority. Fresh owner consent bound to the exact packet SHA is required before Astra submission.

## Frozen identities
- Repository AIOPT5 document HEAD: `756f493b7049434ab67cf2382486a48f606a470c`; tree: `9adf665c0eb06213a29a6c12d650451948c7a4ab`.
- Functional source baseline: `1af604dad66148618cc0e578c28adb4a9829e607`; tree: `5a3ebb9fbdf61185f05b18e58b8799ced40eb60b`.
- SPEC-039 v1.12 SHA-256: `a2feea01cb80b60256da3a7abd5e8c2196954d6e008d6f488cd4fb37f2058af1`.
- Full manifest v1.12/r09 SHA-256: `613c5843f503ff75f704aef6fd9c7b68fb994029d5f822691223212bfbf7bc9a`.
- Prior immutable r08 packet SHA-256: `3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272`.
- Current full source baseline `Common/src/GW.cpp` SHA-256: `7893766ab808473f53b66d8fb9e3804ae5c579bc37d700d90e78155a2ab2cc5e`.
- The working tree is dirty; only the exact identities above define this review. Current status census (not all changes are in scope):
```
 M Specs/SPEC-039-common-performance-measurement.md
 M Specs/SPEC-062-ng-elc-aiopt5-cross-vm-lifecycle-oracle.md
?? Specs/RESULTS-SPEC-039/
?? Specs/RESULTS-SPEC-059/
```

## Review request and boundary
Assess only whether this same-SPEC v1.12 documentary response adequately resolves the r08 B2/B3 contract blockers and preserves fail-closed behavior. In particular assess: (1) whether dedicated UID plus a serialized owner-controlled launcher/lease is an enforceable prerequisite, and whether the explicit BLOCKED state prevents use of the ordinary host UID; (2) whether guarding all three pinned `GW.cpp` sem_open-failure `sem_unlink` paths only in the opt-in diagnostic build is a sufficient documented local mitigation without broad cleanup; (3) whether the append-only fsynced journal event contract, crash/ambiguous-outcome handling and final bundle binding are complete; (4) whether retained canonical-base64 `/proc/sysvipc` bytes support digest recomputation and parsed-summary consistency; (5) whether the evidence procedure adequately binds each returned `sem_t*` to the exact libc and `/dev/shm` inode, or must remain INCONCLUSIVE; and (6) whether the changed schema projection and finalizer requirements can prevent false PASS/unsafe FAIL.

Do not infer that the launcher exists, that any code guard has been implemented, or that a runtime probe has been performed. The manifest explicitly records the launcher as NOT_PROVISIONED and the command as BLOCKED. Treat the proposed launcher mechanism as a prerequisite to assess, not evidence. Distinguish normative blockers from optional hardening, and do not reopen unchanged r07 B1/B2/B3 checker findings. A positive review remains documentary and does not authorize any operational action.

## Previous review history and corrected B1
The r08 first response returned AMEND and listed a conflicting SPEC digest. That B1 arose from the sender envelope omitting `128`; the packet/SPEC/manifest agreed. The follow-up withdrew B1 and retained B2/B3. The first response's B1 is summarized as a withdrawn envelope error; the follow-up is the corrected full response below. Raw SSE and receipts remain archived byte-for-byte.

### Corrected r08 follow-up response (full derived response; raw SSE preserved separately)
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

## Independent read-only reviews (consolidated)
Three non-overlapping agents independently audited the exact r08 packet/source baseline and prior response; no agent changed files or ran build/runtime/IPC/VM/SSH. Their common findings: B1 is the submission-envelope typo, not a packet defect; B2 is supported because mode 0600/exclusive create do not provide continuous same-UID exclusivity, and the three GW.cpp failure branches call sem_unlink by name after sem_open failure; B3 is supported because the old contract did not retain/replay journal bytes, raw sysvipc bytes, or a verifiable returned-sem_t mapping. They cautioned that merely classifying a later result INCONCLUSIVE cannot undo an unsafe deletion.

## Relevant current SPEC clauses
Path: `Specs/SPEC-039-common-performance-measurement.md`; full SHA-256 listed above. The exact current wording for §4.5, D2b, §5 proposed artifacts, §6 next steps, and §7.1 host execution profile follows:

### D2b and diagnostic scope
```markdown
5. **D2b — Correctness test code/build/run:** only after this v1.12 host-controlled profile is positively reviewed under fresh exact-SHA consent, the dedicated-identity preconditions are provisioned and verified, and a separate owner execution decision is recorded, implement only the frozen allowlist: root `CMakeLists.txt`; `Common/src/GW.h`; `Common/src/GW.cpp`; new `tests/spec039/gw_q0_q1_d2.cpp`; and new `tests/spec039/q0_q1_d2.manifest.json`. Add the separate option `NG_ENABLE_NG039_GW_Q0_Q1_D2` (default `OFF`) and target `ng039_gw_q0_q1_d2`; define `NG_GW_QUEUE_TEST_OBSERVATION` for `Common` and the harness only inside an isolated diagnostic target graph: the option-on root CMake path must declare only `Common` and this harness and return before production application targets, with a configure-time target-census guard rejecting any other target. Keep the existing D1-H harness/manifest as historical evidence and do not overwrite them. Gate observer declarations, storage, methods and production call sites together. In the diagnostic build only, guard all three pinned `GW.cpp` branches that call `sem_unlink` after `sem_open` failure (`GW.cpp:970,1263,1322`): they must record a bounded failure event and must not call `sem_unlink`; the macro-off default build preserves existing behavior. Add targeted tests proving each injected `sem_open` failure path performs no unlink. This safety change is specified, not authorized for implementation by this document. Build the D2 target in a fresh isolated checkout at the pinned functional baseline. The dedicated-UID launcher and lease must be pinned and verified before any D2 action; until provisioned, no launch command is authorized and the old direct `timeout <harness>` invocation is prohibited. The manifest must freeze the exact launcher, lease identity, run/finalizer command and process-tree bounds before the separate execution gate. Resolve all path aliases before any command. Keep worktree, build directories, HOME/TMPDIR and results outside `.hermes`; use only named workspace/repository roots. The harness spawns no child processes.

   The trusted dedicated-UID launcher and exclusive lease must be verified before any IPC action; the direct host-UID launch command is prohibited. Only then capture complete host inventories for `/proc/sysvipc/{shm,sem,msg}` and `/dev/shm`; host inventories may be non-empty, but all five candidate key/name pairs must be absent. Derive and record `D2_PROCESS_KEY` and selected attempt using §7.1's exact algorithm; candidate keys are `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. Embed exact raw `/proc/sysvipc` bytes (canonical base64) in each inventory artifact; finalizer must recompute raw digests and normalized summaries from those bytes. Create and fsync `reservation-journal.jsonl` before reservation. Abort before construction on unreadable/over-cap inventory, candidate collision, key overflow, missing lease, or ambiguity. Then exclusively reserve all five SHM segments and five POSIX semaphores and journal each intent/outcome. NovaGenesis constructors may open only these reserved resources. A collision, mode mismatch, unverified returned-handle/libc mapping, incomplete journal or failed reservation blocks construction/PASS; do no speculative cleanup.

   After Gateway/output threads stop and join and `Process/GW` are destroyed, retain the dedicated-UID lease through finalization. Remove only exact run-created SHM IDs/names whose successful journal receipts and current identities match. Name-based `sem_unlink` is permitted only while the verified exclusive execution domain remains held; if the lease is lost, another process appears, or identity/journal state is ambiguous, perform no unlink/RMID for the disputed object and preserve residue. Append/fsync each cleanup intent and outcome; an intent without a durable outcome is `INCONCLUSIVE`, never evidence of removal. Capture final inventory, including exact raw table bytes, and require it to match pre. Any failed cleanup, residual candidate, unexplained change, blocked join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad cleanup. The finalizer verifies and hashes the journal and all bundle artifacts. Keep D2 at `N=1`, Q0/Q1 correctness only; no performance claim, algorithm change, `PGCS`/`PG`, VM, commit or push unless separately authorized.
```

### Astra checkpoint
```markdown
### 4.5 GPT-6 Astra consultation checkpoints (advisory; no execution authority)

| Checkpoint | Packet and question | Gate informed |
|---|---|---|
| Before D2 test-seam/code | Exact SPEC/manifest/packet hashes, functional baseline, D1-H manifest, proposed observer/interface diff and isolated CMake target graph; the dedicated-UID launcher, exclusive lease and exact process-tree evidence; all three guarded `sem_unlink` paths; journal event schema/state transitions; raw `/proc/sysvipc` retention/recomputation and exact returned-`sem_t`→libc/dev-shm inode mapping. Ask whether the trust boundary actually excludes same-UID mutation from pre-inventory through finalizer, whether any failure path can still unlink an unowned object, whether crash/replay semantics forbid ambiguous cleanup, and whether the schemas can support a non-vacuous PASS without claiming unverified evidence. | The frozen v1.10/r07 B1/B2/B3 packet received `APPROVE` only for its documentary findings. The immutable v1.11/r08 packet `3295a1bffd234673aaa82e2e7e254f3d5bad6c1e747f4d18123f702510445272` received `AMEND`; follow-up withdrew B1 (sender-envelope typo) and retained B2/B3. This v1.12 amendment requires a new exact-SHA review after manifest/packet freeze. Review is advisory and cannot authorize D2 or expand owner scope. |
| After D2 correctness result, before D3 | Frozen diff/build/test manifest, all raw outcomes including failures, identities and cleanup evidence. Ask whether the D2 assertions are supported and what remains inconclusive. | Planned checkpoint; a separate Astra submission requires owner consent. The owner decides whether to accept D2 and authorize D3. |
| After D3, before any implementation SPEC | Frozen raw data, analysis, source/build/workload identities and hypothesis classification. Ask whether the evidence supports a future implementation proposal and its acceptance metric. | Future checkpoint only; separate owner consent is required. Astra advice never authorises production changes. |

The owner authorised one GPT-6 Astra review via OpenAI Codex for the frozen v1.2 pre-code D2 packet. That call returned HOLD on the v1.2 SPEC SHA-256 `8973948334538715a9e02474be7b1d88a24e6b9235339870ba126d0570d91` and packet SHA-256 `f29274fa1fa38701ee8a5b9c4ae47366dee9e8f75e372dac826541c57ccca942`; its consent was consumed. On 2026-10-05 the owner separately authorised one review of the sanitized v1.4 packet SHA-256 `ba4ec6ec4b6660e98c3e7d0cdbbd6f18b7945ab6f6bdd30c2294e6c654150948`, bound to SPEC v1.4 SHA-256 `f54901cad03209f94fe98cfad3bf62953895ed24fef3d771a89b89f5229217e1` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`. Astra returned AMEND, response ID `resp_0a12ead61fee5339016ac370498edc87d28ec6212dafa33cf9`. The archived raw SSE SHA-256 is `25b8c29265d327c34323e12f2c35177c2069c13b1d7e99f83a42ffea75240a58`; archive manifest SHA-256 is `46f41c9af22a6b1c9e86e2a66682dfd83f7323fb25d1c47aded2aa52c95e6782`. The r03 AMEND is advisory; its one-call consent is consumed and grants no implementation, build, runtime, IPC/VM, or D3 authority. Raw SSE SHA-256 `14f95d55a4a5611f518a0e236c98edd255e6809acd256b105177f975577bef09`; parsed-response SHA-256 `6fae28a46fd385e0d5aa87e8a1bc7149c832fcd8878170b470dab558941bc587`; archive manifest SHA-256 `b26c7767c9a74f2c9ee716a59257f79064c5480191addbc5f61da998989ca5fd`. The packet’s “NOT submitted” line is a time-frozen pre-review snapshot; this paragraph records the later submission/result. The v1.6 r04 packet SHA-256 `754ad970c1bb5e915262e825856345416360154274ece78ae80642ca6a30d829`, tied to the v1.6 SPEC SHA-256 `3296530c13801de945ed97b920532533c1a4a44d628075a9c5e61cdcc7ee01cb` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_0d91609fd866592e016ac3b3b903e487d2891f1ae2407fb075`). Raw SSE SHA-256 `a1c62af8a4e35a59aec3428fdf45818f2dd89b3f57dce9c3bf8be9a6279fe08d`; parsed-response SHA-256 `3655369b4c6c348e439ec87e48f6a8c1e789c46d5d4500890bee8577b8a969d3`; archive manifest SHA-256 `3f6dbf25e22ffd3ce5ce9290935fd41db162fd81aa67923c445b5ea250fbe2a8`. That exact-SHA consent is consumed. The review closed r03 findings 3 and 4b–4d at design level only; finding 4a remains must-fix. The v1.7 r05 packet SHA-256 `1b45bd2838f82686414257a9fe1c4cec0945b1faba04d59170d99e0e3367508a`, tied to SPEC v1.7 SHA-256 `b73bcc7361bb797b4ddd40952860f0092fb840bb11b31bf0a99659bef8475b70` and baseline `1af604dad66148618cc0e578c28adb4a9829e607`, was reviewed once on 2026-10-05 and returned AMEND (response ID `resp_066534905404d5a5016ac3b9c91aac87d2b521fcde5b122863`). Raw SSE SHA-256 `3d375e4cc4551793dff18a299e3e82ded99bf631a5fdfb0aed195a4dae24dc31`; parsed-response SHA-256 `9b0fa2aa410d6055e50da31134e42af29d97ece12aac77d971299ece7e3851dc`; archive manifest SHA-256 `27eabdd3b469b7044f9ed4935058ffe509f3a92712371924cc73b76dd619266a`. Usage: 22,569 input / 1,650 output / 24,219 total tokens; 8.11725 estimated credits (not USD); quota at return 5h 51%, weekly 86%. Three 4a must-fixes remain: preprocessor-forwarded and function-like observer macros; `-ansi` dialect override; Python assertions disabled by `PYTHONOPTIMIZE=1`. Optional hardening: actual `-o` target mapping, dynamic-linker conflict rejection, and stronger run-record schema `allOf`. Findings 3 and 4b–4d remain design-level closures only. The r05 consent is consumed. SPEC-039 v1.8 is local/unreviewed; any r06 review requires fresh exact-SHA consent. D2 remains blocked.

No VM/SSH/QGA operation, algorithmic production change, commit, push, or release is authorised by these gates. All Astra reviews to date were separately owner-authorised by exact packet SHA and their one-call consents are consumed; none expands the D2 seam/runtime scope. The old NG-062 trial locks and evidence remain untouched.
```

### Proposed runtime artifacts
```markdown
| D2 proposed allowlist (not implemented) | `CMakeLists.txt`, `Common/src/GW.h`, `Common/src/GW.cpp`, new `tests/spec039/gw_q0_q1_d2.cpp`, and `tests/spec039/q0_q1_d2.manifest.json`; separate opt-in option defaults OFF. No NGAL/Process/PGCS/PG, algorithm, separate source helper file, or D3 timing files are proposed. The same allowlisted harness binary's `--finalize` mode runs after the test exits and performs finalization without constructing Process/GW or touching IPC. The trusted dedicated-UID launcher/lease is a provisioned external runtime prerequisite, not part of the five-file code allowlist; it does not exist here and direct host-UID execution is prohibited. |
```

### Pre-D2 handoff
```markdown
Before D2 code: freeze the compile-time-gated observer, exact five-file allowlist, in-memory future-message admission through `PushToInputQueue`, diagnostic no-unlink behavior, dedicated-UID launcher identity/lease, journal/inventory schemas and §7.1 cleanup oracle in a new pre-code packet. Obtain fresh exact-SHA consent before Astra. Do not apply the diff until positive documentary review and a separate execution decision; launcher provisioning is also a distinct prerequisite. Q0 distinguishes zero application offers from bootstrap queue state; Q1 uses a future `Message::Time` created locally because wire serialization does not carry that scheduling field. Cleanup must reconcile the Process-owned pending test message only after Gateway/output workers stop. The owner’s D2 scope is one correctness-only local run and does not include NGAL ingress coverage, D3 or algorithm changes. Q2–Q4 remain deferred; do not import approval or measurements from archived AIOPT3 SPECs.
```

### Host-controlled profile
```markdown
### 7.1 Host-controlled execution profile — v1.12 (documentary; execution blocked)

The v1.11 direct launch under the ordinary host UID is prohibited for D2. Mode `0600`, umask `0077`, exclusive creation and a pre-inventory do not prevent a same-UID process from replacing a POSIX semaphore name or removing/reusing a SysV resource during the run. This amendment does not claim the current host can satisfy the stronger prerequisite. No D2 configure/build, harness/finalizer execution, IPC creation/removal or measurement may occur until the dedicated single-run identity and launcher are provisioned, independently inspected, pinned in the manifest, and separately authorised by the owner. If they cannot be provided, the D2 execution profile remains `BLOCKED` before any candidate resource is created.

The run and its sequential finalizer must execute under a dedicated non-login UID reserved solely for this D2 run, through a trusted owner-controlled single-run launcher. The launcher must prevent another process from entering that UID's execution domain from before pre-inventory until finalization/lease release, hold an exclusive run lease across launch and finalization, and record its exact path, file identity/hash, UID/GID, lease identity and process-tree identity. A process census alone is not proof of exclusivity. The ordinary interactive UID, an unverified cooperative lock, or a missing launcher/lease is insufficient. Only controlled supervisor/harness/finalizer descendants for this run may use the UID; no login shell, unrelated service or concurrent run may use it. Treat host root/administrator as trusted; if root-level concurrent mutation is in the threat model, this shared-host profile is unacceptable and remains blocked. Record precondition evidence in the run record. Loss or uncertainty of identity/lease is `INCONCLUSIVE`, prohibits further name-based unlink/RMID, and preserves residue for owner-directed recovery. The launcher is outside the five-file code allowlist and has not been provisioned or authorised by this documentary amendment.

The three pinned `GW.cpp` paths at lines 970, 1263 and 1322 call `sem_unlink(name)` when `sem_open` fails. In the future diagnostic build only, the already-listed `NG_GW_QUEUE_TEST_OBSERVATION` compile-time guard must suppress all three unlinks and emit a bounded failure event; the normal macro-off build remains unchanged. Failure to open a semaphore aborts the diagnostic run; only the separate finalizer may clean a resource, and only from a complete reservation journal under a still-valid dedicated-UID lease. Synthetic fault-injection coverage must prove each path makes zero `sem_unlink` calls. This required source behavior is design text only; code changes still need separate owner authorisation.

Before constructing `Process/GW`, capture complete host-visible inventories for `/proc/sysvipc/shm`, `/proc/sysvipc/sem`, `/proc/sysvipc/msg`, and `/dev/shm`. Global inventories may be non-empty, but every candidate key/name must be absent. Capture each raw `/proc/sysvipc` table byte-for-byte in canonical base64 fields inside its phase inventory artifact; the finalizer decodes those bytes, recomputes each SHA-256 over the exact raw bytes, parses the table using the pinned Linux table grammar, and independently recomputes normalized rows/IDs before comparing them to stored summaries. Reject non-canonical base64, duplicate/invalid rows, digest mismatch, parse mismatch, unreadable/over-cap input or any inventory artifact exceeding its bound as `INCONCLUSIVE` before resource creation. The `/dev/shm` structured listing is sorted by bytewise name and retains each entry's kind, device, inode, UID/GID, mode, link count and size; the finalizer compares actual post/final listings with captured records. Inventory incompleteness never means “empty”.

Choose `D2_PROCESS_KEY` deterministically: for attempts `0..31`, compute `0x40000000 + (uint32_be(SHA256(ASCII(run_id + ':' + decimal(attempt)))[0:4]) mod 0x3FFFFFF8)` and select the first base whose four-key range is unoccupied and fits positive signed 32-bit `key_t`; key `11` is checked separately. Record the selected attempt and candidate set `{11, D2_PROCESS_KEY, D2_PROCESS_KEY+1, D2_PROCESS_KEY+2, D2_PROCESS_KEY+3}`. If no base is available, abort `INCONCLUSIVE`.

Before any candidate create, create `reservation-journal.jsonl` in the run result directory using exclusive non-symlink creation and mode `0600`; fsync the file and containing directory. This file is a required runtime artifact, capped at 256 KiB and 128 events, one strict JSON object per newline-terminated record, with monotonically increasing sequence numbers and the same run ID. Each event contains schema version, sequence, event kind, monotonic timestamp, process PID/UID, resource kind, candidate key/source name, and nullable identity/result fields. Event kinds are `JOURNAL_OPENED`, `CREATE_INTENT`, `CREATED`, `CREATE_FAILED`, `CLEANUP_INTENT`, `REMOVED`, `CLEANUP_FAILED`, and `JOURNAL_CLOSED`. Write and fsync `CREATE_INTENT` before each `shmget`/`sem_open`; after the call, append and fsync exactly one `CREATED` record with returned identity or `CREATE_FAILED` with errno. Keep the descriptor open, verify its file identity before each append, and never rewrite/truncate prior events. The finalizer appends/fsyncs `CLEANUP_INTENT` before cleanup and `REMOVED`/`CLEANUP_FAILED` after. It may close the journal only after final inventory capture and all events are durable. A missing event, sequence gap/duplicate, partial line, identity mismatch, crash between intent and outcome, or failure to fsync makes the run `INCONCLUSIVE`; the finalizer must not infer success or retry deletion for an ambiguous resource.

After pre-inventory and journal readiness, set harness umask to `0077`; exclusively reserve five SysV SHM keys with `shmget(..., IPC_CREAT|IPC_EXCL|0600)` at pinned `MaxSegmentSize` and five exact POSIX semaphore names with `sem_open(..., O_CREAT|O_EXCL, 0600, 1)`. Verify effective modes are exactly `0600`. Each `CREATED` event records exact key/name and exclusive-create evidence: for SHM, `shmid`, key, `IPC_STAT` size, creator PID/UID, owner UID and mode; for semaphore, exact source name, `sem_t*` address, `/dev/shm` resolved path, device/inode/UID/GID/mode/link count, and libc identity. A failed/mismatched reservation aborts before `Process/GW` construction; remove only resources whose successful journal receipt and current identity are exact while the lease remains valid. Otherwise do no deletion and report `INCONCLUSIVE`.

For each semaphore, record the `sem_open` provider via `dladdr`, resolved libc path, file device/inode, GNU build ID when available, SHA-256 of the opened libc file, `gnu_get_libc_version()`, and exact production source name/path. Immediately after successful exclusive `sem_open`, capture `/proc/self/maps` and prove the returned `sem_t*` address lies within a shared mapping whose device/inode equals the `stat` identity of exact `/dev/shm/sem.<decimal-name>`. Retain the exact matching maps line or a strict projection including address range, permissions, offset, device and inode in the reservation journal. Recompute/validate this linkage from retained evidence; version/path plus a boolean is insufficient. The mapping procedure is target-libc-specific and is not claimed tested here. If `sem_open` returns no handle, mapping is absent, path/device/inode disagree, or mapping cannot be captured, abort before `Process/GW`; do not set `semaphore_name_mapping_verified=true`, do not PASS, and do no speculative deletion.

After Gateway/output threads stop and join and `Process/GW` are destroyed, capture post inventory. The finalizer may remove only an exact run-created SHM ID or semaphore name after verifying the complete journal, valid dedicated-UID lease, and current key/name plus creator/device/inode/UID/mode identity. Identity inspection followed by name-based `sem_unlink` is safe only while the exclusive single-run execution domain remains enforced; this is a required precondition, not an assumption derived from `0600`. If the lease is absent/uncertain, another process appears in the domain, journal is incomplete, or any identity differs, do not call `sem_unlink` or `IPC_RMID` on that resource. A crash after `CLEANUP_INTENT` but before a durable outcome is never treated as successful removal; no automatic retry of that ambiguous operation is allowed.

Capture final complete inventory and require it to match pre exactly for all `/proc/sysvipc` raw bytes/parsed summaries and `/dev/shm` entries. The final run record lists `reservation-journal.jsonl` exactly once with final size/hash and every other required artifact exactly once. The finalizer strictly parses journal/inventories, recomputes event order/transitions, raw table hashes and normalized inventories, proves each cleanup event maps to one successful create receipt, and derives status from those bytes rather than caller booleans. It must not accept `journal_fsynced=true`, `mapping_verified=true` or a final inventory match as substitutes for underlying evidence. Missing/contradictory evidence, unexplained change, residual candidate, failed join, timeout or ambiguous exit is `INCONCLUSIVE`; no broad host cleanup is permitted. If a failure occurs before journal closure, retain partial evidence and do not claim PASS/FAIL. D2 remains one controlled process tree, one Q0/Q1 correctness run, no network/VM, D3 timing, performance claim, algorithm change, PGCS/PG, commit or push unless separately authorised.

1. Host-controlled preflight before construction: the run starts only after the §7.1 inventory and attribution prerequisites pass. No constructor-time IPC may precede that successful preflight.
```

## Exact relevant source excerpts from pinned functional baseline
These are excerpts read from the immutable baseline commit; they show all three `GW.cpp` sem_open-failure branches. The macro-gated no-unlink requirement is proposed only; these source bytes have not been modified.

### GW.cpp:963-971
```cpp
963:       // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)
964: 
965:     } // (mutex == SEM_FAILED) means it was unable to create/open the semaphore
966:     else
967:     {
968:       perror("reading SHM : unable to create/open semaphore");
969: 
970:       sem_unlink(SemaphoreName.c_str());
971:     }
```

### GW.cpp:1256-1264
```cpp
1256:         // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)
1257: 
1258:       } //(mutex != SEM_FAILED)
1259:       else
1260:       {
1261:         perror("writing SHM : unable to open/create the semaphore");
1262: 
1263:         sem_unlink(_oqs.c_str());
1264:       }
```

### GW.cpp:1318-1324
```cpp
1318:   else
1319:   {
1320:     perror("While initiating the SHM: unable to create semaphore");
1321: 
1322:     sem_unlink(SemaphoreName.c_str());
1323: 
1324:     S << Offset << "(ERROR: Unable to create the semaphore while discovering the peer shmid)" << endl;
```

## Changed machine-readable policies and schemas
The manifest is a complete file. The following projection carries the full modified runtime policies and changed schema branches. The full run-record schema digest and complete changed inventory/journal schema digests are pinned above/in the manifest; omitted unchanged r07 schema fields are not claimed as re-reviewed.
```json
{
  "classification_delta": {
    "artifact_names_unique": "Exactly one of each seven required runtime artifacts; optional harness-diagnostics.txt at most once; run-record.json is separately required.",
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
      "complete closed journal and seven runtime artifacts",
      "raw inventories/hashes independently recomputable",
      "diagnostic failure paths cannot unlink by name"
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
      "returned sem_t mapping absent or mismatched"
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
        "returned sem_t pointer maps to wrong inode"
      ],
      "status": "historical v1.8 local result; detailed standalone fixtures were not retained in the current worktree",
      "valid_count": 4
    },
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
      "each returned sem_t* is proven to map to exact recorded libc-backed /dev/shm inode"
    ],
    "required_artifact_names": [
      "stdout.txt",
      "stderr.txt",
      "ipc-pre.json",
      "ipc-post.json",
      "ipc-final.json",
      "observer-events.json",
      "reservation-journal.jsonl"
    ]
  },
  "collection_responsibility": {
    "artifact_name_uniqueness": "Exactly one each of stdout.txt, stderr.txt, ipc-pre.json, ipc-post.json, ipc-final.json, observer-events.json and reservation-journal.jsonl; optional harness-diagnostics.txt at most once; run-record.json is separate.",
    "failure_summary_binding": "The finalizer recomputes code, phase, last_event_sequence, inventory_availability, cleanup_outcome and missing_artifacts. ASSERTION_FAILED is valid only for an observed correctness-predicate failure with complete safe evidence; contradictory summary fields finalize INCONCLUSIVE/EVIDENCE_INVALID.",
    "finalizer_bounds": "10 s timeout + 5 s kill grace; RLIMIT_AS 128 MiB, CPU 10 s, FSIZE 4 MiB, NOFILE 32, CORE 0.",
    "finalizer_failure": "No valid finalized record means INCONCLUSIVE and cannot PASS.",
    "finalizer_semantic_authority": "Same allowlisted --finalize binary under same dedicated UID/lease validates artifact hashes, journal transitions, raw inventory bytes/digests/parsed summaries, libc mapping evidence, observer correlations and exact cleanup identities. Missing or contradictory evidence is INCONCLUSIVE; no broad cleanup.",
    "harness_run": "Only after dedicated UID launcher/lease is verified: write bounded pending run record, create exclusive append-only reservation-journal.jsonl, fsync every pre/post-create and cleanup event, and retain exact raw /proc/sysvipc bytes per phase. No final status or closed-artifact hash before finalization.",
    "missing_or_invalid_staging": "Metadata-only INCONCLUSIVE; assertions NOT_RUN, measurements null, inventories unavailable, exact cleanup status unknown, missing artifact names; no invented values.",
    "outer_finalizer": "Same allowlisted binary --finalize runs after harness exit/log closure under the still-valid serialized UID launcher/lease. It replays the durable journal, verifies raw inventories/ownership, logs cleanup intent/outcome durably, and performs only exact identity-bound cleanup. Incomplete or ambiguous journal permits no deletion.",
    "reservation_journal_artifact": "reservation-journal.jsonl is required, final, max 256 KiB/128 events, append-only strict JSONL; each record is fsynced and its artifact hash is bound in run-record.",
    "runtime_manifest_file": "none; q0_q1_d2.manifest.json is plan/schema input, not part of runtime bundle.",
    "shell_redirection": "The outer supervisor captures stdout/stderr under D2_RESULTS and closes descriptors before finalization."
  },
  "full_r08_run_record_schema_canonical_sha256": "a2f55e22e68986a3d495de20dedbe0e41792a3f0505239bfb81cc3b26acd999d",
  "full_r09_ipc_inventory_schema_canonical_sha256": "470718e3627322d2f95e5efe001927eab0eef7ca0dd638207f16f77aa5fdcace",
  "full_r09_run_record_schema_canonical_sha256": "64564375e980206fc879fb945d93cd88c4d8ca0fc72e0555913be4f5aac66a04",
  "ipc_inventory": {
    "$id": "urn:ng039:ipc-inventory:v4",
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
            "device": {
              "minimum": 0,
              "type": "integer"
            },
            "gid": {
              "minimum": 0,
              "type": "integer"
            },
            "inode": {
              "minimum": 0,
              "type": "integer"
            },
            "kind": {
              "enum": [
                "regular",
                "symlink",
                "other"
              ]
            },
            "mode": {
              "minimum": 0,
              "type": "integer"
            },
            "name": {
              "maxLength": 255,
              "minLength": 1,
              "pattern": "^[^/]+$",
              "type": "string"
            },
            "nlink": {
              "minimum": 0,
              "type": "integer"
            },
            "size_bytes": {
              "maximum": 9223372036854775807,
              "minimum": 0,
              "type": "integer"
            },
            "uid": {
              "minimum": 0,
              "type": "integer"
            }
          },
          "required": [
            "name",
            "kind",
            "size_bytes",
            "device",
            "inode",
            "uid",
            "gid",
            "mode",
            "nlink"
          ],
          "type": "object"
        },
        "maxItems": 4096,
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
      "proc_sysvipc_raw_b64": {
        "additionalProperties": false,
        "properties": {
          "msg": {
            "contentEncoding": "base64",
            "maxLength": 1398104,
            "pattern": "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$",
            "type": "string"
          },
          "sem": {
            "contentEncoding": "base64",
            "maxLength": 1398104,
            "pattern": "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$",
            "type": "string"
          },
          "shm": {
            "contentEncoding": "base64",
            "maxLength": 1398104,
            "pattern": "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$",
            "type": "string"
          }
        },
        "required": [
          "shm",
          "sem",
          "msg"
        ],
        "type": "object"
      },
      "proc_sysvipc_sha256": {
        "additionalProperties": false,
        "properties": {
          "msg": {
            "pattern": "^[0-9a-f]{64}$",
            "type": [
              "string",
              "null"
            ]
          },
          "sem": {
            "pattern": "^[0-9a-f]{64}$",
            "type": [
              "string",
              "null"
            ]
          },
          "shm": {
            "pattern": "^[0-9a-f]{64}$",
            "type": [
              "string",
              "null"
            ]
          }
        },
        "required": [
          "shm",
          "sem",
          "msg"
        ],
        "type": "object"
      },
      "run_id": {
        "maxLength": 64,
        "minLength": 16,
        "type": "string"
      },
      "schema_version": {
        "const": "ng039-ipc-inventory/v4"
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
            "creator_pid": {
              "minimum": 0,
              "type": "integer"
            },
            "creator_uid": {
              "minimum": 0,
              "type": "integer"
            },
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
            "mode": {
              "minimum": 0,
              "type": "integer"
            },
            "owner_uid": {
              "minimum": 0,
              "type": "integer"
            },
            "size_bytes": {
              "maximum": 9223372036854775807,
              "minimum": 0,
              "type": "integer"
            }
          },
          "required": [
            "id",
            "size_bytes",
            "key",
            "creator_pid",
            "creator_uid",
            "owner_uid",
            "mode"
          ],
          "type": "object"
        },
        "maxItems": 4096,
        "type": "array",
        "uniqueItems": true
      },
      "sysv_shm_total_bytes": {
        "maximum": 9223372036854775807,
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
      "sysv_shm_total_bytes",
      "proc_sysvipc_sha256",
      "proc_sysvipc_raw_b64"
    ],
    "type": "object"
  },
  "local_validation_this_freeze": {
    "astra_submitted": false,
    "d2_d3_measurement": false,
    "harness_execution": false,
    "interpretation": "Documentary contract only; dedicated launcher is unprovisioned and D2 remains blocked.",
    "ipc_or_vm_activity": false,
    "json_duplicate_key_check": "PASS: strict source parse",
    "json_schema_meta_validation": "PASS: all five Draft 2020-12 schemas meta-validated with /usr/bin/python3 jsonschema",
    "manifest_generation": "PASS: generated as new r09 from immutable r08 manifest; r08 manifest/packet and review records preserved",
    "owner_exact_sha_consent_for_r09": false,
    "pass_execution_gate_structure": "PASS: PASS/FAIL schema branches require verified dedicated identity/lease; structural check only, no runtime fixture",
    "raw_table_digest_probe": "PASS: exact bytes accepted when SHA matches; deliberately mismatched digest rejected by local contract probe",
    "runtime_finalizer_execution": false,
    "spec_sha256": "a2feea01cb80b60256da3a7abd5e8c2196954d6e008d6f488cd4fb37f2058af1",
    "synthetic_schema_instances": "PASS: one raw inventory and one journal event instance accepted; no finalizer/runtime fixture was executed"
  },
  "projection_scope": "Complete r09 host-profile policies, complete changed ipc_inventory and reservation_journal_event schemas, complete collection_responsibility fields, and the run-record schema delta affecting artifact list, execution identity, reservations, and PASS/FAIL branches. Unchanged run-record properties are represented by canonical digests, not claimed as reviewed again.",
  "r09_synthetic_validation": {
    "all_five_schema_meta_valid": true,
    "finalizer_executed": false,
    "pass_fail_identity_lease_schema_gates_present": true,
    "raw_digest_match_and_mismatch_probes_pass": true,
    "runtime_or_ipc_executed": false,
    "valid_inventory_fixture_accepted": true,
    "valid_journal_event_fixture_accepted": true
  },
  "reservation_journal_event": {
    "$id": "urn:ng039:reservation-journal-event:v1",
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
        "const": "ng039-reservation-journal-event/v1"
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
  "run_record_delta": {
    "$id": "urn:ng039:run-record:v4",
    "$schema": "https://json-schema.org/draft/2020-12/schema",
    "PASS_FAIL_identity_lease_branches": {
      "1": {
        "execution_identity": {
          "properties": {
            "dedicated_uid_verified": {
              "const": true
            },
            "interactive_login_disabled": {
              "const": true
            },
            "launcher_path": {
              "minLength": 1,
              "type": "string"
            },
            "launcher_sha256": {
              "pattern": "^[0-9a-f]{64}$",
              "type": "string"
            },
            "launcher_verified": {
              "const": true
            },
            "lease_acquired": {
              "const": true
            },
            "lease_held_through_finalizer": {
              "const": true
            },
            "no_concurrent_uid_process": {
              "const": true
            }
          },
          "required": [
            "dedicated_uid_verified",
            "launcher_verified",
            "launcher_path",
            "launcher_sha256",
            "lease_acquired",
            "lease_held_through_finalizer",
            "no_concurrent_uid_process",
            "interactive_login_disabled"
          ],
          "type": "object"
        },
        "required_execution_identity": true,
        "reservation_properties": {
          "journal_artifact_name": {
            "const": "reservation-journal.jsonl"
          },
          "journal_closed": {
            "const": true
          },
          "journal_event_count": {
            "maximum": 128,
            "minimum": 1,
            "type": "integer"
          },
          "journal_fsynced": {
            "const": true
          },
          "journal_sha256": {
            "pattern": "^[0-9a-f]{64}$",
            "type": "string"
          },
          "libc_identity": {
            "properties": {
              "provider_path": {
                "minLength": 1,
                "type": "string"
              },
              "sha256": {
                "pattern": "^[0-9a-f]{64}$",
                "type": "string"
              },
              "version": {
                "minLength": 4,
                "type": "string"
              }
            },
            "required": [
              "provider_path",
              "sha256",
              "version"
            ]
          },
          "libc_version": {
            "minLength": 4,
            "type": "string"
          },
          "posix_semaphores": {
            "maxItems": 5,
            "minItems": 5
          },
          "semaphore_name_mapping_verified": {
            "const": true
          },
          "sysv_shm": {
            "maxItems": 5,
            "minItems": 5
          }
        }
      },
      "2": {
        "execution_identity": {
          "properties": {
            "dedicated_uid_verified": {
              "const": true
            },
            "interactive_login_disabled": {
              "const": true
            },
            "launcher_path": {
              "minLength": 1,
              "type": "string"
            },
            "launcher_sha256": {
              "pattern": "^[0-9a-f]{64}$",
              "type": "string"
            },
            "launcher_verified": {
              "const": true
            },
            "lease_acquired": {
              "const": true
            },
            "lease_held_through_finalizer": {
              "const": true
            },
            "no_concurrent_uid_process": {
              "const": true
            }
          },
          "required": [
            "dedicated_uid_verified",
            "launcher_verified",
            "launcher_path",
            "launcher_sha256",
            "lease_acquired",
            "lease_held_through_finalizer",
            "no_concurrent_uid_process",
            "interactive_login_disabled"
          ],
          "type": "object"
        },
        "required_execution_identity": true,
        "reservation_properties": {
          "journal_artifact_name": {
            "const": "reservation-journal.jsonl"
          },
          "journal_closed": {
            "const": true
          },
          "journal_event_count": {
            "maximum": 128,
            "minimum": 1,
            "type": "integer"
          },
          "journal_fsynced": {
            "const": true
          },
          "journal_sha256": {
            "pattern": "^[0-9a-f]{64}$",
            "type": "string"
          },
          "libc_identity": {
            "properties": {
              "provider_path": {
                "minLength": 1,
                "type": "string"
              },
              "sha256": {
                "pattern": "^[0-9a-f]{64}$",
                "type": "string"
              },
              "version": {
                "minLength": 4,
                "type": "string"
              }
            },
            "required": [
              "provider_path",
              "sha256",
              "version"
            ]
          },
          "libc_version": {
            "minLength": 4,
            "type": "string"
          },
          "posix_semaphores": {
            "maxItems": 5,
            "minItems": 5
          },
          "semaphore_name_mapping_verified": {
            "const": true
          },
          "sysv_shm": {
            "maxItems": 5,
            "minItems": 5
          }
        }
      },
      "4": {
        "execution_identity": {
          "properties": {
            "dedicated_uid_verified": {
              "const": true
            },
            "interactive_login_disabled": {
              "const": true
            },
            "launcher_path": {
              "minLength": 1,
              "type": "string"
            },
            "launcher_sha256": {
              "pattern": "^[0-9a-f]{64}$",
              "type": "string"
            },
            "launcher_verified": {
              "const": true
            },
            "lease_acquired": {
              "const": true
            },
            "lease_held_through_finalizer": {
              "const": true
            },
            "no_concurrent_uid_process": {
              "const": true
            }
          },
          "required": [
            "dedicated_uid_verified",
            "launcher_verified",
            "launcher_path",
            "launcher_sha256",
            "lease_acquired",
            "lease_held_through_finalizer",
            "no_concurrent_uid_process",
            "interactive_login_disabled"
          ],
          "type": "object"
        },
        "required_execution_identity": true,
        "reservation_properties": {
          "journal_artifact_name": {
            "const": "reservation-journal.jsonl"
          },
          "journal_closed": {
            "const": true
          },
          "journal_event_count": {
            "maximum": 128,
            "minimum": 1,
            "type": "integer"
          },
          "journal_fsynced": {
            "const": true
          },
          "journal_sha256": {
            "pattern": "^[0-9a-f]{64}$",
            "type": "string"
          },
          "libc_identity": {
            "properties": {
              "provider_path": {
                "minLength": 1,
                "type": "string"
              },
              "sha256": {
                "pattern": "^[0-9a-f]{64}$",
                "type": "string"
              },
              "version": {
                "minLength": 4,
                "type": "string"
              }
            },
            "required": [
              "provider_path",
              "sha256",
              "version"
            ]
          },
          "libc_version": {
            "minLength": 4,
            "type": "string"
          },
          "posix_semaphores": {
            "maxItems": 5,
            "minItems": 5
          },
          "semaphore_name_mapping_verified": {
            "const": true
          },
          "sysv_shm": {
            "maxItems": 5,
            "minItems": 5
          }
        }
      },
      "5": {
        "execution_identity": {
          "properties": {
            "dedicated_uid_verified": {
              "const": true
            },
            "interactive_login_disabled": {
              "const": true
            },
            "launcher_path": {
              "minLength": 1,
              "type": "string"
            },
            "launcher_sha256": {
              "pattern": "^[0-9a-f]{64}$",
              "type": "string"
            },
            "launcher_verified": {
              "const": true
            },
            "lease_acquired": {
              "const": true
            },
            "lease_held_through_finalizer": {
              "const": true
            },
            "no_concurrent_uid_process": {
              "const": true
            }
          },
          "required": [
            "dedicated_uid_verified",
            "launcher_verified",
            "launcher_path",
            "launcher_sha256",
            "lease_acquired",
            "lease_held_through_finalizer",
            "no_concurrent_uid_process",
            "interactive_login_disabled"
          ],
          "type": "object"
        },
        "required_execution_identity": true,
        "reservation_properties": {
          "journal_artifact_name": {
            "const": "reservation-journal.jsonl"
          },
          "journal_closed": {
            "const": true
          },
          "journal_event_count": {
            "maximum": 128,
            "minimum": 1,
            "type": "integer"
          },
          "journal_fsynced": {
            "const": true
          },
          "journal_sha256": {
            "pattern": "^[0-9a-f]{64}$",
            "type": "string"
          },
          "libc_identity": {
            "properties": {
              "provider_path": {
                "minLength": 1,
                "type": "string"
              },
              "sha256": {
                "pattern": "^[0-9a-f]{64}$",
                "type": "string"
              },
              "version": {
                "minLength": 4,
                "type": "string"
              }
            },
            "required": [
              "provider_path",
              "sha256",
              "version"
            ]
          },
          "libc_version": {
            "minLength": 4,
            "type": "string"
          },
          "posix_semaphores": {
            "maxItems": 5,
            "minItems": 5
          },
          "semaphore_name_mapping_verified": {
            "const": true
          },
          "sysv_shm": {
            "maxItems": 5,
            "minItems": 5
          }
        }
      }
    },
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
              "reservation-journal.jsonl",
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
      "maxItems": 8,
      "type": "array",
      "uniqueItems": true
    },
    "execution_identity": {
      "additionalProperties": false,
      "properties": {
        "dedicated_uid_verified": {
          "type": [
            "boolean",
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
        "interactive_login_disabled": {
          "type": [
            "boolean",
            "null"
          ]
        },
        "launcher_device": {
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        },
        "launcher_inode": {
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        },
        "launcher_path": {
          "maxLength": 1024,
          "type": [
            "string",
            "null"
          ]
        },
        "launcher_sha256": {
          "pattern": "^[0-9a-f]{64}$",
          "type": [
            "string",
            "null"
          ]
        },
        "launcher_verified": {
          "type": [
            "boolean",
            "null"
          ]
        },
        "lease_acquired": {
          "type": [
            "boolean",
            "null"
          ]
        },
        "lease_device": {
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        },
        "lease_held_through_finalizer": {
          "type": [
            "boolean",
            "null"
          ]
        },
        "lease_inode": {
          "minimum": 0,
          "type": [
            "integer",
            "null"
          ]
        },
        "lease_path": {
          "maxLength": 1024,
          "type": [
            "string",
            "null"
          ]
        },
        "no_concurrent_uid_process": {
          "type": [
            "boolean",
            "null"
          ]
        },
        "process_tree_pids": {
          "items": {
            "minimum": 2,
            "type": "integer"
          },
          "maxItems": 32,
          "type": [
            "array",
            "null"
          ],
          "uniqueItems": true
        },
        "uid": {
          "minimum": 1,
          "type": [
            "integer",
            "null"
          ]
        }
      },
      "required": [
        "dedicated_uid_verified",
        "uid",
        "gid",
        "interactive_login_disabled",
        "launcher_verified",
        "launcher_path",
        "launcher_sha256",
        "launcher_device",
        "launcher_inode",
        "lease_path",
        "lease_device",
        "lease_inode",
        "lease_acquired",
        "lease_held_through_finalizer",
        "no_concurrent_uid_process",
        "process_tree_pids"
      ],
      "type": "object"
    },
    "reservations": {
      "journal_artifact_name": {
        "const": "reservation-journal.jsonl"
      },
      "journal_closed": {
        "type": [
          "boolean",
          "null"
        ]
      },
      "journal_event_count": {
        "maximum": 128,
        "minimum": 0,
        "type": [
          "integer",
          "null"
        ]
      },
      "journal_fsynced": {
        "type": "boolean"
      },
      "journal_sha256": {
        "pattern": "^[0-9a-f]{64}$",
        "type": [
          "string",
          "null"
        ]
      },
      "libc_identity": {
        "additionalProperties": false,
        "properties": {
          "build_id": {
            "maxLength": 256,
            "type": [
              "string",
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
          "inode": {
            "minimum": 0,
            "type": [
              "integer",
              "null"
            ]
          },
          "provider_path": {
            "maxLength": 1024,
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
          },
          "version": {
            "maxLength": 64,
            "type": [
              "string",
              "null"
            ]
          }
        },
        "required": [
          "provider_path",
          "device",
          "inode",
          "build_id",
          "sha256",
          "version"
        ],
        "type": "object"
      },
      "libc_version": {
        "maxLength": 64,
        "minLength": 4,
        "type": [
          "string",
          "null"
        ]
      },
      "posix_semaphores": {
        "items": {
          "additionalProperties": false,
          "properties": {
            "created_exclusively": {
              "const": true
            },
            "device": {
              "minimum": 0,
              "type": "integer"
            },
            "inode": {
              "minimum": 1,
              "type": "integer"
            },
            "key": {
              "minimum": 1,
              "type": "integer"
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
              "const": 384,
              "type": "integer"
            },
            "name": {
              "pattern": "^[0-9]+$",
              "type": "string"
            },
            "path": {
              "pattern": "^/dev/shm/sem\\.[0-9]+$",
              "type": "string"
            },
            "sem_t_address": {
              "minimum": 0,
              "type": [
                "integer",
                "null"
              ]
            },
            "uid": {
              "minimum": 0,
              "type": "integer"
            }
          },
          "required": [
            "created_exclusively",
            "device",
            "inode",
            "key",
            "mode",
            "name",
            "path",
            "uid",
            "sem_t_address",
            "map_start",
            "map_end",
            "map_device_major",
            "map_device_minor",
            "map_inode",
            "mapping_line"
          ],
          "type": "object"
        },
        "maxItems": 5,
        "type": "array"
      },
      "semaphore_name_mapping_verified": {
        "type": [
          "boolean",
          "null"
        ]
      },
      "sysv_shm": {
        "items": {
          "additionalProperties": false,
          "properties": {
            "created_exclusively": {
              "const": true
            },
            "creator_pid": {
              "minimum": 2,
              "type": "integer"
            },
            "creator_uid": {
              "minimum": 0,
              "type": "integer"
            },
            "id": {
              "minimum": 0,
              "type": "integer"
            },
            "key": {
              "minimum": 1,
              "type": "integer"
            },
            "mode": {
              "const": 384,
              "type": "integer"
            },
            "owner_uid": {
              "minimum": 0,
              "type": "integer"
            },
            "size_bytes": {
              "maximum": 33554532,
              "minimum": 1,
              "type": "integer"
            }
          },
          "required": [
            "created_exclusively",
            "creator_pid",
            "creator_uid",
            "id",
            "key",
            "mode",
            "owner_uid",
            "size_bytes"
          ],
          "type": "object"
        },
        "maxItems": 5,
        "type": "array"
      }
    },
    "schema_version": {
      "const": "ng039-d2-run-record/v4"
    },
    "spec_sha256": {
      "const": "a2feea01cb80b60256da3a7abd5e8c2196954d6e008d6f488cd4fb37f2058af1"
    },
    "title": "SPEC-039 D2 v4 staged/final runtime result record"
  },
  "runtime_bundle": {
    "candidate_object_mode_octal": "0600",
    "host_dev_shm_inventory_entries": 4096,
    "host_inventory_unrelated_segment_bytes_cap": 9223372036854775807,
    "host_sysv_inventory_entries_per_table": 4096,
    "max_artifact_bytes": 4194304,
    "max_artifacts_excluding_run_record": 8,
    "max_candidate_sysv_shm_segment_bytes": 33554532,
    "max_candidate_sysv_shm_segments": 5,
    "max_candidate_sysv_shm_total_bytes": 167772660,
    "max_files_total": 9,
    "max_run_record_bytes": 262144,
    "max_total_bytes": 33554432,
    "optional_files": [
      "harness-diagnostics.txt"
    ],
    "proc_sysvipc_raw_tables": {
      "digest": "SHA-256 over exact decoded bytes",
      "encoding": "canonical base64 in each ipc-pre/post/final.json",
      "finalizer": "strictly decode, recompute digest, parse pinned Linux table grammar, recompute normalized rows/IDs and compare; mismatch is INCONCLUSIVE",
      "max_decoded_bytes_per_table": 1048576,
      "tables": [
        "shm",
        "sem",
        "msg"
      ]
    },
    "required_files": [
      "run-record.json",
      "stdout.txt",
      "stderr.txt",
      "ipc-pre.json",
      "ipc-post.json",
      "ipc-final.json",
      "observer-events.json",
      "reservation-journal.jsonl"
    ],
    "required_libc_identity": "record exact sem_open provider path/device/inode/build-id/hash/version and prove returned sem_t* address-to-/proc/self/maps shared mapping whose device/inode matches exact /dev/shm semaphore object",
    "reservation_journal": {
      "file_name": "reservation-journal.jsonl",
      "final_bundle_binding": "listed exactly once in run-record artifacts and hashed after JOURNAL_CLOSED; final run-record hash is stored only in outer archive index",
      "format": "strict JSONL, one canonical object per newline-terminated line",
      "fsync": "fsync file after every event and containing directory after exclusive create",
      "max_bytes": 262144,
      "max_events": 128,
      "required": true,
      "schema_version": "ng039-reservation-journal-event/v1"
    },
    "reservation_umask_octal": "0077",
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
    "candidate_key_algorithm": "For attempt n=0..31 compute base=0x40000000 + (uint32_be(SHA256(ASCII(run_id + ':' + decimal(n)))[0:4]) mod 0x3FFFFFF8); select first free contiguous base..base+3, exclude key 11 separately, and record n/base. All values are positive signed 32-bit key_t.",
    "candidate_keys": "11 and D2_PROCESS_KEY through D2_PROCESS_KEY+3",
    "candidate_keys_absent_pre_required": true,
    "candidate_object_mode": "0600 (effective SHM and POSIX semaphore permissions must match exactly)",
    "candidate_posix_semaphores": {
      "creation": "sem_open exact source name with O_CREAT|O_EXCL, mode 0600; record exact /dev/shm path device/inode/UID/mode",
      "max_names": 5,
      "mode": "0600"
    },
    "candidate_sysv_shm": {
      "creation": "shmget IPC_CREAT|IPC_EXCL|0600; verify IPC_STAT creator PID/key/UID/mode/size",
      "max_segments": 5,
      "segment_bytes": 33554532,
      "total_bytes": 167772660
    },
    "cleanup_authority": "only exact candidate resources with complete journal receipts and matching current identities while dedicated-UID exclusive lease remains valid; if lease/identity/journal is uncertain, no unlink/RMID for disputed resource and no broad cleanup",
    "cwd": "$D2_RESULTS",
    "dev_shm_inventory_bounds": {
      "max_entries": 4096,
      "max_entry_bytes": 9223372036854775807,
      "max_total_bytes": 9223372036854775807
    },
    "diagnostic_sem_open_failure_paths": [
      {
        "baseline_line": 970,
        "path": "Common/src/GW.cpp"
      },
      {
        "baseline_line": 1263,
        "path": "Common/src/GW.cpp"
      },
      {
        "baseline_line": 1322,
        "path": "Common/src/GW.cpp"
      }
    ],
    "diagnostic_sem_open_failure_policy": "With NG_GW_QUEUE_TEST_OBSERVATION enabled, all three sem_open failure branches emit bounded failure evidence and never call sem_unlink; macro-off default behavior remains unchanged. Documentary requirement only; implementation unauthorized.",
    "direct_host_uid_launch_permitted": false,
    "environment_contract_ref": "cmake.environment_contract",
    "exclusive_candidate_reservation_required": true,
    "execution_profile": "host-controlled; BLOCKED until an owner-controlled dedicated non-login UID and serialized single-run launcher/lease are provisioned and verified",
    "finalizer_command": "BLOCKED: must be frozen through the same dedicated-UID launcher/lease before D2 authorization",
    "finalizer_kill_grace_s": 5,
    "finalizer_rlimits": {
      "RLIMIT_AS_bytes": 134217728,
      "RLIMIT_CORE_bytes": 0,
      "RLIMIT_CPU_seconds": 10,
      "RLIMIT_FSIZE_bytes": 4194304,
      "RLIMIT_NOFILE": 32
    },
    "finalizer_timeout_s": 10,
    "full_pre_post_final_inventory_required": true,
    "harness_spawns_children": false,
    "host_controlled": true,
    "host_inventory_may_be_nonempty": true,
    "kill_grace_s": 5,
    "launch_command": "BLOCKED: dedicated UID launcher and lease are not provisioned or pinned; direct host-UID launch is prohibited",
    "launch_stdout_stderr": "Outer supervisor captures stdout.txt/stderr.txt under D2_RESULTS and closes them before finalization.",
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
    "outer_status_capture": {
      "ambiguous_status": "128-255 without independent signal evidence is INCONCLUSIVE",
      "outer_code": "raw timeout-wrapper exit status",
      "outer_signal": "only from independent unambiguous wait evidence; otherwise null",
      "timeout_fired": "true on supervisor timeout; code 124 conservatively treated as timeout"
    },
    "reservation_umask": "0077 set in harness process before reservation",
    "rlimits": {
      "RLIMIT_AS_bytes": 536870912,
      "RLIMIT_CORE_bytes": 0,
      "RLIMIT_CPU_seconds": 30,
      "RLIMIT_FSIZE_bytes": 4194304,
      "RLIMIT_NOFILE": 64
    },
    "runtime_finalizer_cleanup": "after run process exits, exact cleanup is allowed only from complete durable reservation journal; incomplete identity evidence permits no deletion",
    "semaphore_name_mapping": "Record exact libc provider identity; prove returned sem_t* lies in a shared /proc/self/maps mapping whose device/inode equals stat of exact /dev/shm/sem.<decimal-name>; retain matching mapping evidence in journal. Missing/mismatch is INCONCLUSIVE before Process/GW construction.",
    "sysv_inventory_bounds": {
      "max_entries_per_table": 4096,
      "max_reported_bytes": 9223372036854775807
    },
    "timeout_s": 20
  },
  "semantic_finalizer_contract": {
    "artifact_checks": [
      "exact allowlisted file set",
      "optional harness-diagnostics.txt at most once",
      "reject symlink/non-regular files",
      "check byte caps and SHA-256 against actual closed files",
      "check total bundle cap",
      "seven required artifact names exactly once, including reservation-journal.jsonl",
      "strict journal JSONL event schema/order/run-ID validation and file hash verification",
      "verify dedicated UID launcher/lease identity and continuous lease evidence before cleanup",
      "seven required runtime artifact names exactly once, including reservation-journal.jsonl",
      "strict reservation-journal schema/order/run-ID/replay validation and closed-file hashing",
      "dedicated UID launcher/lease identity and continuity verified before cleanup"
    ],
    "assertion_to_phase": {
      "bootstrap_queue_drained": "bootstrap",
      "candidate_keys_absent_final": "final_inventory",
      "candidate_keys_absent_pre": "pre_inventory",
      "candidate_resources_attributable": "post_inventory",
      "candidate_resources_removed": "cleanup",
      "candidate_resources_reserved": "reservation",
      "evidence_complete": "finalize",
      "final_inventory_complete": "final_inventory",
      "final_inventory_matches_pre": "final_inventory",
      "finalization_complete": "finalize",
      "first_target_wait_fresh": "q1",
      "gateway_joined": "shutdown",
      "host_inventory_preflight": "pre_inventory",
      "output_worker_joined": "shutdown",
      "post_inventory_complete": "post_inventory",
      "pre_inventory_complete": "pre_inventory",
      "process_blocks_verified": "construction",
      "q0_empty_observation_delta": "q0",
      "q0_zero_controlled_offers": "q0",
      "scalar_run_identity_valid": "reservation",
      "target_admitted": "q1",
      "target_delete_false_before_gateway": "construction",
      "target_erased_once": "shutdown",
      "target_future_at_end": "q1",
      "target_is_head": "q1",
      "target_not_popped": "q1",
      "target_not_run": "q1",
      "wait_decision_correlated": "q1"
    },
    "execution_identity": {
      "failure": "missing/unverified launcher blocks before IPC; lease loss or competing process yields INCONCLUSIVE and no name-based cleanup of disputed resources",
      "lease": "held continuously from before pre-inventory through finalizer completion",
      "required": "dedicated non-login UID and trusted serialized launcher/lease; process census alone insufficient"
    },
    "expected_outcome_rule": "Every negative fixture is rejected as a PASS/FAIL claim and finalized INCONCLUSIVE with the listed bounded reason; a finalizer error/no finalized record is also never PASS.",
    "failure_summary_checks": [
      "failure.code is recomputed from the highest-precedence observed condition; ASSERTION_FAILED is used only for a demonstrated correctness predicate failure after all evidence/safety gates pass.",
      "failure.phase must be the phase mapped to the selected failed predicate or uncertainty condition.",
      "failure.missing_artifacts equals the exact required artifact names absent from the frozen result directory; FAIL requires the empty list.",
      "failure.inventory_availability is derived from actual inventory-file presence and successful parsing, not caller-provided booleans.",
      "failure.cleanup_outcome derives from stop/join, exclusive reservation receipts, exact unlink/RMID results and final inventory equality; FAIL requires complete.",
      "failure.last_event_sequence equals the greatest validated event sequence, or null only when no valid event exists.",
      "Any contradiction in code/phase/availability/cleanup/missing-artifact fields yields INCONCLUSIVE/EVIDENCE_INVALID."
    ],
    "inventory_checks": [
      "schema, run_id and phase agree with record role",
      "is_empty is recomputed but need not be true for host-wide pre/post/final inventories",
      "candidate keys and semaphore names are absent from pre-inventory",
      "post inventory contains no unexplained changes; exact run-created candidates may be present before cleanup",
      "only exact run-created SHM IDs and semaphore names are removed; final inventory equals pre",
      "each raw /proc/sysvipc table is present as canonical base64; finalizer recomputes SHA-256 from decoded bytes and reparses to independently derive normalized summaries",
      "all candidate resources have successful exclusive-reservation journal events, effective mode 0600 and exact kernel/file identity",
      "dedicated non-login UID and exclusive launcher lease verified continuously through finalizer; ordinary host UID is invalid",
      "diagnostic sem_open failure paths cannot issue sem_unlink; all three pinned sites are guarded in the diagnostic build",
      "returned sem_t* address lies in a shared mapping whose device/inode equals exact semaphore path identity under the recorded libc",
      "only exact run-created SHM IDs/semaphore names are removed while lease remains valid; final raw/parsed inventories equal pre",
      "each /proc/sysvipc table is retained as canonical base64; finalizer recomputes SHA-256 and reparses normalized rows/IDs",
      "all candidates have successful journal events, effective mode 0600 and exact identity",
      "dedicated UID and exclusive launcher lease verified continuously; ordinary host UID invalid",
      "diagnostic sem_open failure paths cannot issue sem_unlink",
      "returned sem_t* address lies in shared mapping whose device/inode equals exact semaphore file under recorded libc",
      "only exact run-created SHM IDs/semaphore names removed while lease valid; final raw/parsed inventories equal pre"
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
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "candidate key/name already present before reservation"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "exclusive SHM/semaphore reservation collision or partial reservation"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "reservation journal missing an object or creator/inode identity"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "candidate post resource identity differs from exclusive reservation receipt"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "failed exact cleanup or residual candidate resource"
      },
      {
        "expected_failure_code": "EVIDENCE_INVALID",
        "expected_final_status": "INCONCLUSIVE",
        "input": "final IPC inventory differs from pre-inventory"
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
    "reservation_journal": {
      "binding": "required final runtime artifact; run-record artifact list binds bytes/size/SHA-256; final run-record SHA stays in outer archive index",
      "crash_rule": "intent without durable outcome is ambiguous; no automatic deletion retry for that resource and no PASS/FAIL",
      "event_order": [
        "JOURNAL_OPENED",
        "CREATE_INTENT",
        "CREATED|CREATE_FAILED",
        "CLEANUP_INTENT",
        "REMOVED|CLEANUP_FAILED",
        "JOURNAL_CLOSED"
      ],
      "file_name": "reservation-journal.jsonl",
      "max_bytes": 262144,
      "max_events": 128,
      "required_behavior": "exclusive mode 0600, append-only strict JSONL, monotone sequence, same run_id, fsync each record and parent directory after create",
      "schema_id": "urn:ng039:reservation-journal-event:v1"
    },
    "schema_boundary": "JSON Schema checks shape and simple local status/value implications; the finalizer owns cross-artifact/equality/ordering rules so they are not duplicated.",
    "single_authority": "same allowlisted ng039_gw_q0_q1_d2 binary --finalize mode; no extra runner/helper file",
    "status_precedence": [
      "PASS only if all Q0/Q1 predicates, exclusive reservation, exact cleanup and final inventory equality pass",
      "FAIL only on an observable correctness assertion failure with complete evidence and safe exact cleanup",
      "otherwise INCONCLUSIVE; do not delete any resource lacking an exact reservation identity"
    ],
    "strict_json": [
      "reject duplicate object keys",
      "reject NaN/Infinity",
      "validate run-record, IPC inventory and observer event schemas"
    ]
  }
}
```

## Bounded synthetic fixtures for this review (not runtime tests)
- Schema-positive shape: complete empty-host inventory with exact raw empty-table base64 and corresponding SHA digests; one `JOURNAL_OPENED` event.
- Required negative classes: launcher absent; lease lost; same-UID intruder; diagnostic unlink call; raw bytes/digest/parsed-summary mismatch; noncanonical base64; journal gap/partial line/fsync loss; crash between cleanup intent/outcome; missing/mismatched sem_t mapping; false PASS with any of the above.
- These are contract requirements only; no semantic finalizer or harness exists/executed. No IPC resource was created.

## Requested verdict format
1. VERDICT: APPROVE / AMEND / HOLD, explicitly limited to documentary contract.
2. Must-fix blockers with exact clause/schema location and concrete false-PASS, unsafe-cleanup or unreplayable-evidence risk; otherwise “none”.
3. Optional advisories separated from blockers.
4. Gate statement: the launcher remains unprovisioned; no implementation/configure/build/runtime/IPC/D3/commit/push/release authority is granted.

The packet is focused and does not claim Astra inspected the full SPEC or manifest beyond the exact embedded excerpts/projection. Do not use tools; assess only supplied contents.
