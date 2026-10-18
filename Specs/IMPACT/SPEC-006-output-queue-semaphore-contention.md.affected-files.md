# Affected-file record: `SPEC-006-output-queue-semaphore-contention.md`

SPEC title: SPEC-006 — Output Queue Semaphore Contention Fix

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | (none) | (not declared) | Implemented | (not declared) | (not declared) | Indeterminate: no Branch header establishes applicability |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | (none) | (not declared) | Implemented | (not declared) | (not declared) | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | (none) | (not declared) | Implemented | (not declared) | (not declared) | Indeterminate: no Branch header establishes applicability |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-006-output-queue-semaphore-contention.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT4` — source `Specs/SPEC-006-output-queue-semaphore-contention.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT5` — source `Specs/SPEC-006-output-queue-semaphore-contention.md`
- No literal file paths were detected in the recognized scope/file sections.

No explicit file paths found in the recognized sections for this SPEC. This is an inventory gap, not evidence that no files were affected.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `fb87f5da0b249e61d3b38fb35eb94d4127405c1d` — Consolidate documentation: SPECs, Docs, Issues, validation scripts (link basis: the SPEC file changed in this commit)
  - `Docs/ARCHITECTURE/NG-INVERTED-PUB-SUB-MODEL.md`
  - `Docs/ARCHITECTURE/NGAL-ARCHITECTURE.md`
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/DECISIONS/DOCUMENTATION-ARCHITECTURE.md`
  - `Docs/DECISIONS/REVERT.md`
  - `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`
  - `Docs/DIAGNOSTICS/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md`
  - `Docs/DIAGNOSTICS/PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md`
  - `Docs/NG-INVERTED-PUB-SUB-MODEL.md`
  - `Docs/PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md`
  - `Docs/REVERT.md`
  - `Issues/CLOSED/ISSUE-002-NGAL-Hash-Mismatch.md`
  - `Issues/CLOSED/ISSUE-003-GCC15-Thread-Ambiguity.md`
  - `Issues/CLOSED/ISSUE-004-NRNCS-Debug-Logging.md`
  - `Issues/OPEN/ISSUE-001-NRInfoPayload01-Cache-Miss.md`
  - `Issues/TEMPLATE.md`
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/Simple/validate-skills.sh`
  - `Scripts/Simple/validate-specs.sh`
  - `Specs/PLAN-CONSOLIDATION-2026-07-16.md`
  - `Specs/SPEC-001-NRNCS-debug-logging.md`
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
  - `Specs/SPEC-023-contentpublish-burst-throttling.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
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
- Commit `c36b8ba04ae5e56a9b3de6762ebf7237663ebeaf` — docs(spec): SPEC-007 — decouple WriteToSharedMemory3 from OutputQueueMutex (link basis: commit subject/body names this SPEC identifier)
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
- Commit `3f4d6658e3644bb7db72f7bf2d371a5faae0bf0e` — fix(gw): remove redundant Output Queue named semaphore + add retry loop (SPEC-006 v2.0) (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
  - `Common/src/GWMsgCl01.cpp`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`

### `AIOPT4`
- Commit `fb87f5da0b249e61d3b38fb35eb94d4127405c1d` — Consolidate documentation: SPECs, Docs, Issues, validation scripts (link basis: the SPEC file changed in this commit)
  - `Docs/ARCHITECTURE/NG-INVERTED-PUB-SUB-MODEL.md`
  - `Docs/ARCHITECTURE/NGAL-ARCHITECTURE.md`
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/DECISIONS/DOCUMENTATION-ARCHITECTURE.md`
  - `Docs/DECISIONS/REVERT.md`
  - `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`
  - `Docs/DIAGNOSTICS/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md`
  - `Docs/DIAGNOSTICS/PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md`
  - `Docs/NG-INVERTED-PUB-SUB-MODEL.md`
  - `Docs/PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md`
  - `Docs/REVERT.md`
  - `Issues/CLOSED/ISSUE-002-NGAL-Hash-Mismatch.md`
  - `Issues/CLOSED/ISSUE-003-GCC15-Thread-Ambiguity.md`
  - `Issues/CLOSED/ISSUE-004-NRNCS-Debug-Logging.md`
  - `Issues/OPEN/ISSUE-001-NRInfoPayload01-Cache-Miss.md`
  - `Issues/TEMPLATE.md`
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/Simple/validate-skills.sh`
  - `Scripts/Simple/validate-specs.sh`
  - `Specs/PLAN-CONSOLIDATION-2026-07-16.md`
  - `Specs/SPEC-001-NRNCS-debug-logging.md`
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
  - `Specs/SPEC-023-contentpublish-burst-throttling.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
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
- Commit `c36b8ba04ae5e56a9b3de6762ebf7237663ebeaf` — docs(spec): SPEC-007 — decouple WriteToSharedMemory3 from OutputQueueMutex (link basis: commit subject/body names this SPEC identifier)
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
- Commit `3f4d6658e3644bb7db72f7bf2d371a5faae0bf0e` — fix(gw): remove redundant Output Queue named semaphore + add retry loop (SPEC-006 v2.0) (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
  - `Common/src/GWMsgCl01.cpp`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`

### `AIOPT5`
- Commit `fb87f5da0b249e61d3b38fb35eb94d4127405c1d` — Consolidate documentation: SPECs, Docs, Issues, validation scripts (link basis: the SPEC file changed in this commit)
  - `Docs/ARCHITECTURE/NG-INVERTED-PUB-SUB-MODEL.md`
  - `Docs/ARCHITECTURE/NGAL-ARCHITECTURE.md`
  - `Docs/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/DECISIONS/DOCUMENTATION-ARCHITECTURE.md`
  - `Docs/DECISIONS/REVERT.md`
  - `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`
  - `Docs/DIAGNOSTICS/CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md`
  - `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md`
  - `Docs/DIAGNOSTICS/PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md`
  - `Docs/NG-INVERTED-PUB-SUB-MODEL.md`
  - `Docs/PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md`
  - `Docs/REVERT.md`
  - `Issues/CLOSED/ISSUE-002-NGAL-Hash-Mismatch.md`
  - `Issues/CLOSED/ISSUE-003-GCC15-Thread-Ambiguity.md`
  - `Issues/CLOSED/ISSUE-004-NRNCS-Debug-Logging.md`
  - `Issues/OPEN/ISSUE-001-NRInfoPayload01-Cache-Miss.md`
  - `Issues/TEMPLATE.md`
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/Simple/validate-skills.sh`
  - `Scripts/Simple/validate-specs.sh`
  - `Specs/PLAN-CONSOLIDATION-2026-07-16.md`
  - `Specs/SPEC-001-NRNCS-debug-logging.md`
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
  - `Specs/SPEC-023-contentpublish-burst-throttling.md`
  - `Specs/SPEC-MUSL-001-gcc15-thread-ambiguity.md`
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
- Commit `c36b8ba04ae5e56a9b3de6762ebf7237663ebeaf` — docs(spec): SPEC-007 — decouple WriteToSharedMemory3 from OutputQueueMutex (link basis: commit subject/body names this SPEC identifier)
  - `Specs/SPEC-007-output-queue-lock-decoupling.md`
- Commit `3f4d6658e3644bb7db72f7bf2d371a5faae0bf0e` — fix(gw): remove redundant Output Queue named semaphore + add retry loop (SPEC-006 v2.0) (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
  - `Common/src/GWMsgCl01.cpp`
  - `Specs/SPEC-006-output-queue-semaphore-contention.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
