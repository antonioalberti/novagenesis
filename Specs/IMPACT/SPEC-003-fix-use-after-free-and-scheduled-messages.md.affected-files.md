# Affected-file record: `SPEC-003-fix-use-after-free-and-scheduled-messages.md`

SPEC title: SPEC-003: Correção de Use-After-Free e Eliminação de ScheduledMessages

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | (none) | (not declared) | (not declared in header) | (not declared) | (not declared) | Indeterminate: no Branch header establishes applicability |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | (none) | (not declared) | (not declared in header) | (not declared) | (not declared) | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | (none) | (not declared) | (not declared in header) | (not declared) | (not declared) | Indeterminate: no Branch header establishes applicability |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
- path: `Common/src/GWMsgCl01.cpp` — source SPEC line 156.
- path: `Common/src/Block.cpp` — source SPEC line 157.
- path: `Common/src/Process.cpp` — source SPEC line 158.

### `AIOPT4` — source `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
- path: `Common/src/GWMsgCl01.cpp` — source SPEC line 156.
- path: `Common/src/Block.cpp` — source SPEC line 157.
- path: `Common/src/Process.cpp` — source SPEC line 158.

### `AIOPT5` — source `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
- path: `Common/src/GWMsgCl01.cpp` — source SPEC line 156.
- path: `Common/src/Block.cpp` — source SPEC line 157.
- path: `Common/src/Process.cpp` — source SPEC line 158.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `3adb1635dbaf00658afc746855df117e71109aa5` — SPEC-033 Phase B step 5: fix factually wrong SPEC-003 comment (D4). The comment claimed no Action pushes to ScheduledMessages anymore; 5 still do (CoreMsgCl01 x4 modules, IRMsgCl02). Corrected: the cleanup loop stays removed because pushed messages belong to the Process container (sole owner) — deletion here would be a double-ownership bug; disposition is the later PushToInputQueue transfer. Added NDEBUG-gated invariant: each scheduled message must still be a live Process-owned slot at end of the CL loop. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/Block.cpp`
- Commit `977856c2b9610c431e93319204a95f742c8fc62e` — SPEC-033 Phase C.2 COMPLETE: all 5 lifecycle-script questions answered by code sweep of Common/PGCS/NRNCS/ContentApp. P1: Actions use APIs only (no wire parsing). P2: no cardinality lookahead. P3: ScheduledMessages is the only intra-Run retention, dies with Run; no Message* members anywhere. P4: Type is live and semantic (0=core, 1=app), keep it. P5: no new wire fields needed. Finding: SPEC-003 comment in Block.cpp is factually wrong (5 Actions still push to ScheduledMessages) (link basis: commit subject/body names this SPEC identifier)
  - `Docs/MESSAGE-LIFECYCLE.md`
- Commit `887e9693f090bfb934447d2aafd0c5adcf1e5db7` — docs: restore Specs/ and Docs/ from git history + HT debug log (link basis: the SPEC file changed in this commit)
  - `Common/src/Block.cpp`
  - `Common/src/GWSCNSeq01.cpp`
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/REVERT.md`
  - `NRNCS/src/NRInfoPayload01.cpp`
  - `NRNCS/src/NRMsgCl01.cpp`
  - `NRNCS/src/NRSCNSeq01.cpp`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
  - `Specs/SPEC-004-contentapp-performance-optimisation.md`
  - `Specs/SPEC-005-gw-shm-poll-interval-reduction.md`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-010-contentapp-hello-ipc20-bindings.md`
  - `Specs/SPEC-011-periodic-exposition-contentapp.md`
  - `Specs/SPEC-012-hash-abstraction.md`
  - `Specs/SPEC-013-ngal-adaptation-layer.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-017-content-name-correlation.md`
  - `Specs/SPEC-018-payload-sticks-on-reused-inline-response.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
- Commit `7106c4e509becbe2cdaedeb84d325ab347a78b48` — Returning previous NRNS version (link basis: the SPEC file changed in this commit)
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/REVERT.md`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
  - `Specs/SPEC-004-contentapp-performance-optimisation.md`
  - `Specs/SPEC-005-gw-shm-poll-interval-reduction.md`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-010-contentapp-hello-ipc20-bindings.md`
  - `Specs/SPEC-011-periodic-exposition-contentapp.md`
  - `Specs/SPEC-012-hash-abstraction.md`
  - `Specs/SPEC-013-ngal-adaptation-layer.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-017-content-name-correlation.md`
  - `Specs/SPEC-018-payload-sticks-on-reused-inline-response.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
  - `docs/SPEC-015-hash-mismatch-root-cause-analysis.md`
  - `docs/SPEC-016-hash-verification-post-spec015.md`
  - `docs/clang-format-report-2026-07-06.md`
- Commit `2124b8f1bd8859d992fca5cfc54c2fb4ec3fcca5` — fix(ng-042): fix use-after-free SIGSEGV + hello IPC 2.0 HT_BID + GetBlock bug (SPEC-001/002/003) (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Common/src/Action.cpp`
  - `Common/src/Block.cpp`
  - `Common/src/GW.cpp`
  - `Common/src/GWExposition02.cpp`
  - `Common/src/GWHelloIPC02.cpp`
  - `Common/src/GWHelloIPC02.h`
  - `Common/src/GWMsgCl01.cpp`
  - `Common/src/GWRunInitialization01.cpp`
  - `Common/src/MessageBuilder.cpp`
  - `Common/src/MessageBuilder.h`
  - `Common/src/Process.cpp`
  - `ContentApp/src/CoreRunDiscover01.cpp`
  - `EPGS/UnitTest.c`
  - `GIRS/src/IRStatusS01.cpp`
  - `IoTTestApp/src/CoreRunDiscover01.cpp`
  - `NBTestApp/src/CoreRunDiscover01.cpp`
  - `PGCS/src/CoreRunDiscover01.cpp`
  - `PSS/src/PSStatusS01.cpp`
  - `Scripts/Simple/run_IoTTestApp.sh`
  - `Scripts/Simple/run_NRNCS.sh`
  - `Scripts/Simple/run_PGCS.sh`
  - `Scripts/Simple/run_Repository.sh`
  - `Scripts/Simple/run_Source.sh`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`

### `AIOPT4`
- Commit `3adb1635dbaf00658afc746855df117e71109aa5` — SPEC-033 Phase B step 5: fix factually wrong SPEC-003 comment (D4). The comment claimed no Action pushes to ScheduledMessages anymore; 5 still do (CoreMsgCl01 x4 modules, IRMsgCl02). Corrected: the cleanup loop stays removed because pushed messages belong to the Process container (sole owner) — deletion here would be a double-ownership bug; disposition is the later PushToInputQueue transfer. Added NDEBUG-gated invariant: each scheduled message must still be a live Process-owned slot at end of the CL loop. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/Block.cpp`
- Commit `977856c2b9610c431e93319204a95f742c8fc62e` — SPEC-033 Phase C.2 COMPLETE: all 5 lifecycle-script questions answered by code sweep of Common/PGCS/NRNCS/ContentApp. P1: Actions use APIs only (no wire parsing). P2: no cardinality lookahead. P3: ScheduledMessages is the only intra-Run retention, dies with Run; no Message* members anywhere. P4: Type is live and semantic (0=core, 1=app), keep it. P5: no new wire fields needed. Finding: SPEC-003 comment in Block.cpp is factually wrong (5 Actions still push to ScheduledMessages) (link basis: commit subject/body names this SPEC identifier)
  - `Docs/MESSAGE-LIFECYCLE.md`
- Commit `887e9693f090bfb934447d2aafd0c5adcf1e5db7` — docs: restore Specs/ and Docs/ from git history + HT debug log (link basis: the SPEC file changed in this commit)
  - `Common/src/Block.cpp`
  - `Common/src/GWSCNSeq01.cpp`
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/REVERT.md`
  - `NRNCS/src/NRInfoPayload01.cpp`
  - `NRNCS/src/NRMsgCl01.cpp`
  - `NRNCS/src/NRSCNSeq01.cpp`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
  - `Specs/SPEC-004-contentapp-performance-optimisation.md`
  - `Specs/SPEC-005-gw-shm-poll-interval-reduction.md`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-010-contentapp-hello-ipc20-bindings.md`
  - `Specs/SPEC-011-periodic-exposition-contentapp.md`
  - `Specs/SPEC-012-hash-abstraction.md`
  - `Specs/SPEC-013-ngal-adaptation-layer.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-017-content-name-correlation.md`
  - `Specs/SPEC-018-payload-sticks-on-reused-inline-response.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
- Commit `7106c4e509becbe2cdaedeb84d325ab347a78b48` — Returning previous NRNS version (link basis: the SPEC file changed in this commit)
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/REVERT.md`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
  - `Specs/SPEC-004-contentapp-performance-optimisation.md`
  - `Specs/SPEC-005-gw-shm-poll-interval-reduction.md`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-010-contentapp-hello-ipc20-bindings.md`
  - `Specs/SPEC-011-periodic-exposition-contentapp.md`
  - `Specs/SPEC-012-hash-abstraction.md`
  - `Specs/SPEC-013-ngal-adaptation-layer.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-017-content-name-correlation.md`
  - `Specs/SPEC-018-payload-sticks-on-reused-inline-response.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
  - `docs/SPEC-015-hash-mismatch-root-cause-analysis.md`
  - `docs/SPEC-016-hash-verification-post-spec015.md`
  - `docs/clang-format-report-2026-07-06.md`
- Commit `2124b8f1bd8859d992fca5cfc54c2fb4ec3fcca5` — fix(ng-042): fix use-after-free SIGSEGV + hello IPC 2.0 HT_BID + GetBlock bug (SPEC-001/002/003) (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Common/src/Action.cpp`
  - `Common/src/Block.cpp`
  - `Common/src/GW.cpp`
  - `Common/src/GWExposition02.cpp`
  - `Common/src/GWHelloIPC02.cpp`
  - `Common/src/GWHelloIPC02.h`
  - `Common/src/GWMsgCl01.cpp`
  - `Common/src/GWRunInitialization01.cpp`
  - `Common/src/MessageBuilder.cpp`
  - `Common/src/MessageBuilder.h`
  - `Common/src/Process.cpp`
  - `ContentApp/src/CoreRunDiscover01.cpp`
  - `EPGS/UnitTest.c`
  - `GIRS/src/IRStatusS01.cpp`
  - `IoTTestApp/src/CoreRunDiscover01.cpp`
  - `NBTestApp/src/CoreRunDiscover01.cpp`
  - `PGCS/src/CoreRunDiscover01.cpp`
  - `PSS/src/PSStatusS01.cpp`
  - `Scripts/Simple/run_IoTTestApp.sh`
  - `Scripts/Simple/run_NRNCS.sh`
  - `Scripts/Simple/run_PGCS.sh`
  - `Scripts/Simple/run_Repository.sh`
  - `Scripts/Simple/run_Source.sh`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`

### `AIOPT5`
- Commit `3adb1635dbaf00658afc746855df117e71109aa5` — SPEC-033 Phase B step 5: fix factually wrong SPEC-003 comment (D4). The comment claimed no Action pushes to ScheduledMessages anymore; 5 still do (CoreMsgCl01 x4 modules, IRMsgCl02). Corrected: the cleanup loop stays removed because pushed messages belong to the Process container (sole owner) — deletion here would be a double-ownership bug; disposition is the later PushToInputQueue transfer. Added NDEBUG-gated invariant: each scheduled message must still be a live Process-owned slot at end of the CL loop. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/Block.cpp`
- Commit `977856c2b9610c431e93319204a95f742c8fc62e` — SPEC-033 Phase C.2 COMPLETE: all 5 lifecycle-script questions answered by code sweep of Common/PGCS/NRNCS/ContentApp. P1: Actions use APIs only (no wire parsing). P2: no cardinality lookahead. P3: ScheduledMessages is the only intra-Run retention, dies with Run; no Message* members anywhere. P4: Type is live and semantic (0=core, 1=app), keep it. P5: no new wire fields needed. Finding: SPEC-003 comment in Block.cpp is factually wrong (5 Actions still push to ScheduledMessages) (link basis: commit subject/body names this SPEC identifier)
  - `Docs/MESSAGE-LIFECYCLE.md`
- Commit `887e9693f090bfb934447d2aafd0c5adcf1e5db7` — docs: restore Specs/ and Docs/ from git history + HT debug log (link basis: the SPEC file changed in this commit)
  - `Common/src/Block.cpp`
  - `Common/src/GWSCNSeq01.cpp`
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/REVERT.md`
  - `NRNCS/src/NRInfoPayload01.cpp`
  - `NRNCS/src/NRMsgCl01.cpp`
  - `NRNCS/src/NRSCNSeq01.cpp`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
  - `Specs/SPEC-004-contentapp-performance-optimisation.md`
  - `Specs/SPEC-005-gw-shm-poll-interval-reduction.md`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-010-contentapp-hello-ipc20-bindings.md`
  - `Specs/SPEC-011-periodic-exposition-contentapp.md`
  - `Specs/SPEC-012-hash-abstraction.md`
  - `Specs/SPEC-013-ngal-adaptation-layer.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-017-content-name-correlation.md`
  - `Specs/SPEC-018-payload-sticks-on-reused-inline-response.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
- Commit `7106c4e509becbe2cdaedeb84d325ab347a78b48` — Returning previous NRNS version (link basis: the SPEC file changed in this commit)
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/REVERT.md`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`
  - `Specs/SPEC-004-contentapp-performance-optimisation.md`
  - `Specs/SPEC-005-gw-shm-poll-interval-reduction.md`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-010-contentapp-hello-ipc20-bindings.md`
  - `Specs/SPEC-011-periodic-exposition-contentapp.md`
  - `Specs/SPEC-012-hash-abstraction.md`
  - `Specs/SPEC-013-ngal-adaptation-layer.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-017-content-name-correlation.md`
  - `Specs/SPEC-018-payload-sticks-on-reused-inline-response.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
  - `docs/SPEC-015-hash-mismatch-root-cause-analysis.md`
  - `docs/SPEC-016-hash-verification-post-spec015.md`
  - `docs/clang-format-report-2026-07-06.md`
- Commit `2124b8f1bd8859d992fca5cfc54c2fb4ec3fcca5` — fix(ng-042): fix use-after-free SIGSEGV + hello IPC 2.0 HT_BID + GetBlock bug (SPEC-001/002/003) (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Common/src/Action.cpp`
  - `Common/src/Block.cpp`
  - `Common/src/GW.cpp`
  - `Common/src/GWExposition02.cpp`
  - `Common/src/GWHelloIPC02.cpp`
  - `Common/src/GWHelloIPC02.h`
  - `Common/src/GWMsgCl01.cpp`
  - `Common/src/GWRunInitialization01.cpp`
  - `Common/src/MessageBuilder.cpp`
  - `Common/src/MessageBuilder.h`
  - `Common/src/Process.cpp`
  - `ContentApp/src/CoreRunDiscover01.cpp`
  - `EPGS/UnitTest.c`
  - `GIRS/src/IRStatusS01.cpp`
  - `IoTTestApp/src/CoreRunDiscover01.cpp`
  - `NBTestApp/src/CoreRunDiscover01.cpp`
  - `PGCS/src/CoreRunDiscover01.cpp`
  - `PSS/src/PSStatusS01.cpp`
  - `Scripts/Simple/run_IoTTestApp.sh`
  - `Scripts/Simple/run_NRNCS.sh`
  - `Scripts/Simple/run_PGCS.sh`
  - `Scripts/Simple/run_Repository.sh`
  - `Scripts/Simple/run_Source.sh`
  - `Specs/SPEC-001-hello-ipc-ht-bid.md`
  - `Specs/SPEC-002-message-leak-fix.md`
  - `Specs/SPEC-003-fix-use-after-free-and-scheduled-messages.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
