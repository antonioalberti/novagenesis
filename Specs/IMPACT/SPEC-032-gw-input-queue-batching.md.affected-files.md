# Affected-file record: `SPEC-032-gw-input-queue-batching.md`

SPEC title: SPEC-032: GW input queue batching

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | AIOPT3 | (not declared) | Deferred — previous implementation withdrawn 2026-09-09 | (not declared) | (not declared) | AIOPT3-origin; historical line; status/gates apply to this snapshot only |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | (not declared) | Deferred — previous implementation withdrawn 2026-09-09 | (not declared) | (not declared) | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | AIOPT3 | (not declared) | Deferred — previous implementation withdrawn 2026-09-09 | (not declared) | (not declared) | AIOPT3-origin copy; not authority for AIOPT5 |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-032-gw-input-queue-batching.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT4` — source `Specs/SPEC-032-gw-input-queue-batching.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT5` — source `Specs/SPEC-032-gw-input-queue-batching.md`
- No literal file paths were detected in the recognized scope/file sections.

No explicit file paths found in the recognized sections for this SPEC. This is an inventory gap, not evidence that no files were affected.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `71df07ea1ebd63e6506d2959d70302e6c9e1573f` — SPEC-033: archive withdrawn attempt and draft controlled restart (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/ANALYSIS-Message-CommandLine-Simplification-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/MESSAGE-LIFECYCLE.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/REVIEW-Astra-Message-CommandLine-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/REVIEW-Astra-SPEC-033-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/SPEC033-PHASEB-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/SPEC033-PHASEB2-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/README.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/RESULTS-SPEC-027/spec032-bisect.json`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-032-gw-input-queue-batching.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-033-message-commandline-refactor.md`
  - `Specs/SPEC-032-gw-input-queue-batching.md`
  - `Specs/SPEC-033-message-commandline-refactor.md`
- Commit `77efe27fafbd7483507ed1448cbeedf04782bff4` — SPEC-032/033: restore exact pre-SPEC-032 baseline tree (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `.gitignore`
  - `CMakeLists.txt`
  - `Common/src/Block.cpp`
  - `Common/src/Block.h`
  - `Common/src/CommandLine.cpp`
  - `Common/src/CommandLine.h`
  - `Common/src/CommandLineParser.cpp`
  - `Common/src/CommandLineParser.h`
  - `Common/src/GW.cpp`
  - `Common/src/GW.h`
  - `Common/src/GWMsgCl01.cpp`
  - `Common/src/HTStoreBind01.cpp`
  - `Common/src/Message.cpp`
  - `Common/src/Message.h`
  - `Common/src/MsgHandle.h`
  - `Common/src/Process.cpp`
  - `Common/src/Process.h`
  - `Common/tests/test_mixed_api.cpp`
  - `Common/tests/test_slot_container.cpp`
  - `Docs/ANALYSIS-Message-CommandLine-Simplification-2026-09-07.md`
  - `Docs/MESSAGE-LIFECYCLE.md`
  - `Docs/REVIEW-Astra-Message-CommandLine-2026-09-07.md`
  - `Docs/REVIEW-Astra-SPEC-033-2026-09-07.md`
  - `Docs/SPEC033-PHASEB-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/SPEC033-PHASEB2-ASTRA-REVIEW-2026-09-08.md`
  - `NRNCS/src/NR.cpp`
  - `NRNCS/src/NRInfoPayload01.cpp`
  - `NRNCS/src/NRRunPeriodic01.cpp`
  - `PGCS/src/NGAL_Transport_RAW.cpp`
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PGRunHello01.cpp`
  - `PGCS/src/PGRunPeriodic01.cpp`
  - `Specs/RESULTS-SPEC-027/spec032-bisect.json`
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
  - `Specs/SPEC-032-gw-input-queue-batching.md`
  - `Specs/SPEC-033-message-commandline-refactor.md`
  - `Tests/clp_roundtrip_test.cpp`
- Commit `44032be033c27fdb5dd2796b818e4348fa8188e2` — SPEC-033 B-3 FIX: remove per-batch-element RunFlag reset that silently skipped batch elements after the first. Pre-existing bug from the SPEC-032 diagnostic batching era, exposed by the clean-VM restart procedure: when two bootstrap messages (GW + PG StoringInitialBinds) landed due in the same batch, only batch[0] ran; PG-block StoringInitialBinds never executed, cat13/PG-SCN binding was never stored, RunPeriodic routing failed, and the whole periodic/hello/discovery/photo chain died at startup. Found via PRELOOP/POP instrumentation per Astra method. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `244f8b2352727c6a484b490b4256d53542f39a39` — SPEC-033 Phase B step 6 (B-3): GW queues migrate to QEntry with retention ownership. InputQueue and OutputQueues are now priority_queue<QEntry> with QEntryCompare (copied Time/Tag keys — comparator never dereferences the Message, closing the SPEC-032 cross-thread sort UAF class). Ownership: PushToInput/OutputQueue FindHandle+TryRetain before queue lock (failure = not queueable, mark, return); pop transfers the retention to the consumer; Gateway Run enters via the adopt overload (Run(msg,irm,handle)) — count unchanged through pop->Run; output path WriteToSharedMemory3 success = Release + MarkToDelete, SHM-busy retry TRANSFERS the retention back to the queue without release/re-acquire; stale entry while retained = contract violation, logged and discarded without dereference. nextTime reads use copied entry keys. TryRetain/Release are public on Process (queue API), LifecycleMutex private. Build clean 9 targets, harnesses 14/14 PASS. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
  - `Common/src/GW.h`
  - `Common/src/Process.h`
- Commit `c1b998305c4317f6cc80b6b385021c7f38f6ac72` — SPEC-032 bisect: regression isolated to e8692dd GW batch; 8e92e29 restored and stable at 2000 msg/s (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-bisect.json`
- Commit `c64c6d353d71aac0133eff36492bdaa17eba14c8` — SPEC-032 diag: add inMem (GetNumberOfMessages) to throttle line (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `482879998b8ad7e360de20078f16244671bee530` — SPEC-032 diag: make admission rejections visible (unbuffered cerr) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `ed632e5ce4aa2ecb4397c2324cb92079330959f8` — SPEC-032 diag fix: use time(nullptr) throttle (PB not in scope) (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
- Commit `9ddd4fc723397f1b2f721b8fa4efb93304aaf817` — SPEC-032 diag: unbuffered cerr diagnostics in SendToARawSocket (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
- Commit `d77b5bcc747799c89b7d31241ed04ceed35be9c2` — SPEC-032 diag: report top() scheduled time (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `a0c47a31370ca7c87b307cf254bc494d3c0c1ee0` — SPEC-031 fix: remove MSG_TRUNC as INPUT flag on recvmmsg (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/NGAL_Transport_RAW.cpp`
- Commit `ccab074b619a2c493308db413af26297f6d89c92` — SPEC-032 debug build: throttled diagnostics in GW batch loop (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `aa300c317ffaa7e38bca7cb149c389d28550c1ef` — SPEC-032 diagnostic: regression isolated to GW batching implementation (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
- Commit `b0968898beef63b206f70eec3a3b3580ea7e7eb4` — DIAGNOSTIC: revert SPEC-032 GW batching to isolate receive-path regression (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `e8692dd1a96ab17d2a7f28513efdd200c5f0b789` — SPEC-032: GW input queue batching (implemented by Hermes per Astra SPEC) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `1cb1059ba2a624647ebd63886ff0de07cebccde6` — SPEC-032 (Draft): GW input queue batching - Astra-authored (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-032-gw-input-queue-batching.md`

### `AIOPT4`
- Commit `71df07ea1ebd63e6506d2959d70302e6c9e1573f` — SPEC-033: archive withdrawn attempt and draft controlled restart (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/ANALYSIS-Message-CommandLine-Simplification-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/MESSAGE-LIFECYCLE.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/REVIEW-Astra-Message-CommandLine-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/REVIEW-Astra-SPEC-033-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/SPEC033-PHASEB-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/SPEC033-PHASEB2-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/README.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/RESULTS-SPEC-027/spec032-bisect.json`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-032-gw-input-queue-batching.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-033-message-commandline-refactor.md`
  - `Specs/SPEC-032-gw-input-queue-batching.md`
  - `Specs/SPEC-033-message-commandline-refactor.md`
- Commit `77efe27fafbd7483507ed1448cbeedf04782bff4` — SPEC-032/033: restore exact pre-SPEC-032 baseline tree (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `.gitignore`
  - `CMakeLists.txt`
  - `Common/src/Block.cpp`
  - `Common/src/Block.h`
  - `Common/src/CommandLine.cpp`
  - `Common/src/CommandLine.h`
  - `Common/src/CommandLineParser.cpp`
  - `Common/src/CommandLineParser.h`
  - `Common/src/GW.cpp`
  - `Common/src/GW.h`
  - `Common/src/GWMsgCl01.cpp`
  - `Common/src/HTStoreBind01.cpp`
  - `Common/src/Message.cpp`
  - `Common/src/Message.h`
  - `Common/src/MsgHandle.h`
  - `Common/src/Process.cpp`
  - `Common/src/Process.h`
  - `Common/tests/test_mixed_api.cpp`
  - `Common/tests/test_slot_container.cpp`
  - `Docs/ANALYSIS-Message-CommandLine-Simplification-2026-09-07.md`
  - `Docs/MESSAGE-LIFECYCLE.md`
  - `Docs/REVIEW-Astra-Message-CommandLine-2026-09-07.md`
  - `Docs/REVIEW-Astra-SPEC-033-2026-09-07.md`
  - `Docs/SPEC033-PHASEB-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/SPEC033-PHASEB2-ASTRA-REVIEW-2026-09-08.md`
  - `NRNCS/src/NR.cpp`
  - `NRNCS/src/NRInfoPayload01.cpp`
  - `NRNCS/src/NRRunPeriodic01.cpp`
  - `PGCS/src/NGAL_Transport_RAW.cpp`
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PGRunHello01.cpp`
  - `PGCS/src/PGRunPeriodic01.cpp`
  - `Specs/RESULTS-SPEC-027/spec032-bisect.json`
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
  - `Specs/SPEC-032-gw-input-queue-batching.md`
  - `Specs/SPEC-033-message-commandline-refactor.md`
  - `Tests/clp_roundtrip_test.cpp`
- Commit `44032be033c27fdb5dd2796b818e4348fa8188e2` — SPEC-033 B-3 FIX: remove per-batch-element RunFlag reset that silently skipped batch elements after the first. Pre-existing bug from the SPEC-032 diagnostic batching era, exposed by the clean-VM restart procedure: when two bootstrap messages (GW + PG StoringInitialBinds) landed due in the same batch, only batch[0] ran; PG-block StoringInitialBinds never executed, cat13/PG-SCN binding was never stored, RunPeriodic routing failed, and the whole periodic/hello/discovery/photo chain died at startup. Found via PRELOOP/POP instrumentation per Astra method. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `244f8b2352727c6a484b490b4256d53542f39a39` — SPEC-033 Phase B step 6 (B-3): GW queues migrate to QEntry with retention ownership. InputQueue and OutputQueues are now priority_queue<QEntry> with QEntryCompare (copied Time/Tag keys — comparator never dereferences the Message, closing the SPEC-032 cross-thread sort UAF class). Ownership: PushToInput/OutputQueue FindHandle+TryRetain before queue lock (failure = not queueable, mark, return); pop transfers the retention to the consumer; Gateway Run enters via the adopt overload (Run(msg,irm,handle)) — count unchanged through pop->Run; output path WriteToSharedMemory3 success = Release + MarkToDelete, SHM-busy retry TRANSFERS the retention back to the queue without release/re-acquire; stale entry while retained = contract violation, logged and discarded without dereference. nextTime reads use copied entry keys. TryRetain/Release are public on Process (queue API), LifecycleMutex private. Build clean 9 targets, harnesses 14/14 PASS. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
  - `Common/src/GW.h`
  - `Common/src/Process.h`
- Commit `c1b998305c4317f6cc80b6b385021c7f38f6ac72` — SPEC-032 bisect: regression isolated to e8692dd GW batch; 8e92e29 restored and stable at 2000 msg/s (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-bisect.json`
- Commit `c64c6d353d71aac0133eff36492bdaa17eba14c8` — SPEC-032 diag: add inMem (GetNumberOfMessages) to throttle line (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `482879998b8ad7e360de20078f16244671bee530` — SPEC-032 diag: make admission rejections visible (unbuffered cerr) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `ed632e5ce4aa2ecb4397c2324cb92079330959f8` — SPEC-032 diag fix: use time(nullptr) throttle (PB not in scope) (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
- Commit `9ddd4fc723397f1b2f721b8fa4efb93304aaf817` — SPEC-032 diag: unbuffered cerr diagnostics in SendToARawSocket (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
- Commit `d77b5bcc747799c89b7d31241ed04ceed35be9c2` — SPEC-032 diag: report top() scheduled time (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `a0c47a31370ca7c87b307cf254bc494d3c0c1ee0` — SPEC-031 fix: remove MSG_TRUNC as INPUT flag on recvmmsg (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/NGAL_Transport_RAW.cpp`
- Commit `ccab074b619a2c493308db413af26297f6d89c92` — SPEC-032 debug build: throttled diagnostics in GW batch loop (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `aa300c317ffaa7e38bca7cb149c389d28550c1ef` — SPEC-032 diagnostic: regression isolated to GW batching implementation (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
- Commit `b0968898beef63b206f70eec3a3b3580ea7e7eb4` — DIAGNOSTIC: revert SPEC-032 GW batching to isolate receive-path regression (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `e8692dd1a96ab17d2a7f28513efdd200c5f0b789` — SPEC-032: GW input queue batching (implemented by Hermes per Astra SPEC) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `1cb1059ba2a624647ebd63886ff0de07cebccde6` — SPEC-032 (Draft): GW input queue batching - Astra-authored (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-032-gw-input-queue-batching.md`

### `AIOPT5`
- Commit `71df07ea1ebd63e6506d2959d70302e6c9e1573f` — SPEC-033: archive withdrawn attempt and draft controlled restart (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/ANALYSIS-Message-CommandLine-Simplification-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/MESSAGE-LIFECYCLE.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/REVIEW-Astra-Message-CommandLine-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/REVIEW-Astra-SPEC-033-2026-09-07.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/SPEC033-PHASEB-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Docs/SPEC033-PHASEB2-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/README.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/RESULTS-SPEC-027/spec032-bisect.json`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-032-gw-input-queue-batching.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-033-message-commandline-refactor.md`
  - `Specs/SPEC-032-gw-input-queue-batching.md`
  - `Specs/SPEC-033-message-commandline-refactor.md`
- Commit `77efe27fafbd7483507ed1448cbeedf04782bff4` — SPEC-032/033: restore exact pre-SPEC-032 baseline tree (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `.gitignore`
  - `CMakeLists.txt`
  - `Common/src/Block.cpp`
  - `Common/src/Block.h`
  - `Common/src/CommandLine.cpp`
  - `Common/src/CommandLine.h`
  - `Common/src/CommandLineParser.cpp`
  - `Common/src/CommandLineParser.h`
  - `Common/src/GW.cpp`
  - `Common/src/GW.h`
  - `Common/src/GWMsgCl01.cpp`
  - `Common/src/HTStoreBind01.cpp`
  - `Common/src/Message.cpp`
  - `Common/src/Message.h`
  - `Common/src/MsgHandle.h`
  - `Common/src/Process.cpp`
  - `Common/src/Process.h`
  - `Common/tests/test_mixed_api.cpp`
  - `Common/tests/test_slot_container.cpp`
  - `Docs/ANALYSIS-Message-CommandLine-Simplification-2026-09-07.md`
  - `Docs/MESSAGE-LIFECYCLE.md`
  - `Docs/REVIEW-Astra-Message-CommandLine-2026-09-07.md`
  - `Docs/REVIEW-Astra-SPEC-033-2026-09-07.md`
  - `Docs/SPEC033-PHASEB-ASTRA-REVIEW-2026-09-08.md`
  - `Docs/SPEC033-PHASEB2-ASTRA-REVIEW-2026-09-08.md`
  - `NRNCS/src/NR.cpp`
  - `NRNCS/src/NRInfoPayload01.cpp`
  - `NRNCS/src/NRRunPeriodic01.cpp`
  - `PGCS/src/NGAL_Transport_RAW.cpp`
  - `PGCS/src/PG.cpp`
  - `PGCS/src/PGRunHello01.cpp`
  - `PGCS/src/PGRunPeriodic01.cpp`
  - `Specs/RESULTS-SPEC-027/spec032-bisect.json`
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
  - `Specs/SPEC-032-gw-input-queue-batching.md`
  - `Specs/SPEC-033-message-commandline-refactor.md`
  - `Tests/clp_roundtrip_test.cpp`
- Commit `44032be033c27fdb5dd2796b818e4348fa8188e2` — SPEC-033 B-3 FIX: remove per-batch-element RunFlag reset that silently skipped batch elements after the first. Pre-existing bug from the SPEC-032 diagnostic batching era, exposed by the clean-VM restart procedure: when two bootstrap messages (GW + PG StoringInitialBinds) landed due in the same batch, only batch[0] ran; PG-block StoringInitialBinds never executed, cat13/PG-SCN binding was never stored, RunPeriodic routing failed, and the whole periodic/hello/discovery/photo chain died at startup. Found via PRELOOP/POP instrumentation per Astra method. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `244f8b2352727c6a484b490b4256d53542f39a39` — SPEC-033 Phase B step 6 (B-3): GW queues migrate to QEntry with retention ownership. InputQueue and OutputQueues are now priority_queue<QEntry> with QEntryCompare (copied Time/Tag keys — comparator never dereferences the Message, closing the SPEC-032 cross-thread sort UAF class). Ownership: PushToInput/OutputQueue FindHandle+TryRetain before queue lock (failure = not queueable, mark, return); pop transfers the retention to the consumer; Gateway Run enters via the adopt overload (Run(msg,irm,handle)) — count unchanged through pop->Run; output path WriteToSharedMemory3 success = Release + MarkToDelete, SHM-busy retry TRANSFERS the retention back to the queue without release/re-acquire; stale entry while retained = contract violation, logged and discarded without dereference. nextTime reads use copied entry keys. TryRetain/Release are public on Process (queue API), LifecycleMutex private. Build clean 9 targets, harnesses 14/14 PASS. (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
  - `Common/src/GW.h`
  - `Common/src/Process.h`
- Commit `c1b998305c4317f6cc80b6b385021c7f38f6ac72` — SPEC-032 bisect: regression isolated to e8692dd GW batch; 8e92e29 restored and stable at 2000 msg/s (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-bisect.json`
- Commit `c64c6d353d71aac0133eff36492bdaa17eba14c8` — SPEC-032 diag: add inMem (GetNumberOfMessages) to throttle line (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `482879998b8ad7e360de20078f16244671bee530` — SPEC-032 diag: make admission rejections visible (unbuffered cerr) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `ed632e5ce4aa2ecb4397c2324cb92079330959f8` — SPEC-032 diag fix: use time(nullptr) throttle (PB not in scope) (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
- Commit `9ddd4fc723397f1b2f721b8fa4efb93304aaf817` — SPEC-032 diag: unbuffered cerr diagnostics in SendToARawSocket (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/PG.cpp`
- Commit `d77b5bcc747799c89b7d31241ed04ceed35be9c2` — SPEC-032 diag: report top() scheduled time (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `a0c47a31370ca7c87b307cf254bc494d3c0c1ee0` — SPEC-031 fix: remove MSG_TRUNC as INPUT flag on recvmmsg (link basis: commit subject/body names this SPEC identifier)
  - `PGCS/src/NGAL_Transport_RAW.cpp`
- Commit `ccab074b619a2c493308db413af26297f6d89c92` — SPEC-032 debug build: throttled diagnostics in GW batch loop (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `aa300c317ffaa7e38bca7cb149c389d28550c1ef` — SPEC-032 diagnostic: regression isolated to GW batching implementation (link basis: commit subject/body names this SPEC identifier)
  - `Specs/RESULTS-SPEC-027/spec032-diagnostic-revert.json`
- Commit `b0968898beef63b206f70eec3a3b3580ea7e7eb4` — DIAGNOSTIC: revert SPEC-032 GW batching to isolate receive-path regression (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `e8692dd1a96ab17d2a7f28513efdd200c5f0b789` — SPEC-032: GW input queue batching (implemented by Hermes per Astra SPEC) (link basis: commit subject/body names this SPEC identifier)
  - `Common/src/GW.cpp`
- Commit `1cb1059ba2a624647ebd63886ff0de07cebccde6` — SPEC-032 (Draft): GW input queue batching - Astra-authored (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-032-gw-input-queue-batching.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
