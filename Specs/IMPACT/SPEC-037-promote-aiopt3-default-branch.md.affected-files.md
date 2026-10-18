# Affected-file record: `SPEC-037-promote-aiopt3-default-branch.md`

SPEC title: SPEC-037: Promote AIOPT3 to the Default NovaGenesis Branch

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | AIOPT3 | v1.0 | Implemented | (not declared) | `5ab91ce` baseline; final promotion result recorded in `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md` | AIOPT3-origin; historical line; status/gates apply to this snapshot only |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | v1.0 | Implemented | (not declared) | `5ab91ce` baseline; final promotion result recorded in `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md` | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | yes | AIOPT3 | v1.0 | Implemented | (not declared) | `5ab91ce` baseline; final promotion result recorded in `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md` | AIOPT3-origin copy; not authority for AIOPT5 |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT4` — source `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- No literal file paths were detected in the recognized scope/file sections.

### `AIOPT5` — source `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- No literal file paths were detected in the recognized scope/file sections.

No explicit file paths found in the recognized sections for this SPEC. This is an inventory gap, not evidence that no files were affected.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `4f83b38176bc6db3162876379fa6b61c7489ccae` — SPEC-038: anonymize public deployment data (link basis: the SPEC file changed in this commit)
  - `Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`
  - `Docs/DECISIONS/DOCUMENTATION-ARCHITECTURE.md`
  - `Docs/DECISIONS/NG-020-C1-test-only-manifest-20260910.md`
  - `Docs/DECISIONS/NG-037-BRANCH-COMPARISON-20260911.md`
  - `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
  - `Docs/DECISIONS/REVERT.md`
  - `Docs/DECISIONS/SPEC033-R3-B1-FIXTURE-REDESIGN-20260909.md`
  - `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-032-gw-input-queue-batching.md`
  - `Issues/OPEN/ISSUE-001-NRInfoPayload01-Cache-Miss.md`
  - `Make/compile-parallel.sh`
  - `Make/compile.sh`
  - `Scripts/AlpineVMs/alpine-phase4-ng.sh`
  - `Scripts/AlpineVMs/alpine-phase5-repo.sh`
  - `Scripts/AlpineVMs/alpine-phase5-source.sh`
  - `Scripts/AlpineVMs/check-run-scripts.sh`
  - `Scripts/AlpineVMs/fix-network.sh`
  - `Scripts/AlpineVMs/ng-vm.env.example`
  - `Scripts/AlpineVMs/run_NRNCS_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_NRNCS_on_Source_VM.sh`
  - `Scripts/AlpineVMs/run_PGCS_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh`
  - `Scripts/AlpineVMs/run_Repository_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/AlpineVMs/setup-ng-repo.sh`
  - `Scripts/AlpineVMs/setup-ng-source.sh`
  - `Scripts/AlpineVMs/soak-monitor.sh`
  - `Scripts/AlpineVMs/start-ng-repo.sh`
  - `Scripts/AlpineVMs/start-ng-source.sh`
  - `Scripts/AlpineVMs/stop-ng.sh`
  - `Scripts/Python/zmq_client.py`
  - `Scripts/Simple/Inter_OS_Content_Test_NRNCS.sh`
  - `Scripts/Simple/Intra_OS_NBTest.sh`
  - `Scripts/Simple/run_IoTTestApp.sh`
  - `Scripts/Simple/run_NRNCS.sh`
  - `Scripts/Simple/run_PGCS.sh`
  - `Scripts/Simple/run_Repository.sh`
  - `Scripts/Simple/run_Source.sh`
  - `Specs/PLAN-CONSOLIDATION-2026-07-16.md`
  - `Specs/RESULTS-SPEC-027/amend3v2-500-run.json`
  - `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`
  - `Specs/RESULTS-SPEC-027/soak30min-20260906.json`
  - `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`
  - `Specs/RESULTS-SPEC-027/spec031-after-2000.json`
  - `Specs/RESULTS-SPEC-027/spec031-after-500.json`
  - `Specs/RESULTS-SPEC-027/spec031-before-baseline.json`
  - `Specs/RESULTS-SPEC-027/spec033-r3-b1-controls-20260910.json`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-nrinfopayload01-separate-messages.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-027-pgcs-multithreading.md`
  - `Specs/SPEC-029-pgcs-self-mac-failfast.md`
  - `Specs/SPEC-033-R2-newmessage-exhaustion-output.md`
  - `Specs/SPEC-033-R3-B-gw-caller-safety.md`
  - `Specs/SPEC-033-R3-B2-gw-input-queue-null-safety.md`
  - `Specs/SPEC-033-R3-C-process-output-safety.md`
  - `Specs/SPEC-033-R3-caller-safety-amendment.md`
  - `Specs/SPEC-034-pgcs-low-rate-diagnostic.md`
  - `Specs/SPEC-035-nbtestapp-nrncs-intermediate.md`
  - `Specs/SPEC-036-deprecate-standalone-pss-girs-hts.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
  - `Specs/SPEC-038-public-repository-hygiene.md`
- Commit `cf7526503ddea85f077d80ff96dd9e8c7ec6eff7` — SPEC-037: record default branch promotion (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- Commit `5ab91ce0c4731575f5ffef67c834d2628edc9762` — SPEC-037: record branch comparison and accepted gates (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/NG-037-BRANCH-COMPARISON-20260911.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- Commit `b69a9ae689637163919782ff1d98082eff3692cc` — SPEC-037: define AIOPT3 default branch promotion (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`

### `AIOPT4`
- Commit `4f83b38176bc6db3162876379fa6b61c7489ccae` — SPEC-038: anonymize public deployment data (link basis: the SPEC file changed in this commit)
  - `Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`
  - `Docs/DECISIONS/DOCUMENTATION-ARCHITECTURE.md`
  - `Docs/DECISIONS/NG-020-C1-test-only-manifest-20260910.md`
  - `Docs/DECISIONS/NG-037-BRANCH-COMPARISON-20260911.md`
  - `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
  - `Docs/DECISIONS/REVERT.md`
  - `Docs/DECISIONS/SPEC033-R3-B1-FIXTURE-REDESIGN-20260909.md`
  - `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-032-gw-input-queue-batching.md`
  - `Issues/OPEN/ISSUE-001-NRInfoPayload01-Cache-Miss.md`
  - `Make/compile-parallel.sh`
  - `Make/compile.sh`
  - `Scripts/AlpineVMs/alpine-phase4-ng.sh`
  - `Scripts/AlpineVMs/alpine-phase5-repo.sh`
  - `Scripts/AlpineVMs/alpine-phase5-source.sh`
  - `Scripts/AlpineVMs/check-run-scripts.sh`
  - `Scripts/AlpineVMs/fix-network.sh`
  - `Scripts/AlpineVMs/ng-vm.env.example`
  - `Scripts/AlpineVMs/run_NRNCS_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_NRNCS_on_Source_VM.sh`
  - `Scripts/AlpineVMs/run_PGCS_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh`
  - `Scripts/AlpineVMs/run_Repository_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/AlpineVMs/setup-ng-repo.sh`
  - `Scripts/AlpineVMs/setup-ng-source.sh`
  - `Scripts/AlpineVMs/soak-monitor.sh`
  - `Scripts/AlpineVMs/start-ng-repo.sh`
  - `Scripts/AlpineVMs/start-ng-source.sh`
  - `Scripts/AlpineVMs/stop-ng.sh`
  - `Scripts/Python/zmq_client.py`
  - `Scripts/Simple/Inter_OS_Content_Test_NRNCS.sh`
  - `Scripts/Simple/Intra_OS_NBTest.sh`
  - `Scripts/Simple/run_IoTTestApp.sh`
  - `Scripts/Simple/run_NRNCS.sh`
  - `Scripts/Simple/run_PGCS.sh`
  - `Scripts/Simple/run_Repository.sh`
  - `Scripts/Simple/run_Source.sh`
  - `Specs/PLAN-CONSOLIDATION-2026-07-16.md`
  - `Specs/RESULTS-SPEC-027/amend3v2-500-run.json`
  - `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`
  - `Specs/RESULTS-SPEC-027/soak30min-20260906.json`
  - `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`
  - `Specs/RESULTS-SPEC-027/spec031-after-2000.json`
  - `Specs/RESULTS-SPEC-027/spec031-after-500.json`
  - `Specs/RESULTS-SPEC-027/spec031-before-baseline.json`
  - `Specs/RESULTS-SPEC-027/spec033-r3-b1-controls-20260910.json`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-nrinfopayload01-separate-messages.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-027-pgcs-multithreading.md`
  - `Specs/SPEC-029-pgcs-self-mac-failfast.md`
  - `Specs/SPEC-033-R2-newmessage-exhaustion-output.md`
  - `Specs/SPEC-033-R3-B-gw-caller-safety.md`
  - `Specs/SPEC-033-R3-B2-gw-input-queue-null-safety.md`
  - `Specs/SPEC-033-R3-C-process-output-safety.md`
  - `Specs/SPEC-033-R3-caller-safety-amendment.md`
  - `Specs/SPEC-034-pgcs-low-rate-diagnostic.md`
  - `Specs/SPEC-035-nbtestapp-nrncs-intermediate.md`
  - `Specs/SPEC-036-deprecate-standalone-pss-girs-hts.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
  - `Specs/SPEC-038-public-repository-hygiene.md`
- Commit `cf7526503ddea85f077d80ff96dd9e8c7ec6eff7` — SPEC-037: record default branch promotion (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- Commit `5ab91ce0c4731575f5ffef67c834d2628edc9762` — SPEC-037: record branch comparison and accepted gates (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/NG-037-BRANCH-COMPARISON-20260911.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- Commit `b69a9ae689637163919782ff1d98082eff3692cc` — SPEC-037: define AIOPT3 default branch promotion (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`

### `AIOPT5`
- Commit `4f83b38176bc6db3162876379fa6b61c7489ccae` — SPEC-038: anonymize public deployment data (link basis: the SPEC file changed in this commit)
  - `Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`
  - `Docs/DECISIONS/DOCUMENTATION-ARCHITECTURE.md`
  - `Docs/DECISIONS/NG-020-C1-test-only-manifest-20260910.md`
  - `Docs/DECISIONS/NG-037-BRANCH-COMPARISON-20260911.md`
  - `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
  - `Docs/DECISIONS/REVERT.md`
  - `Docs/DECISIONS/SPEC033-R3-B1-FIXTURE-REDESIGN-20260909.md`
  - `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md`
  - `Docs/HISTORICAL/SPEC032-033-withdrawn-attempt/Specs/SPEC-032-gw-input-queue-batching.md`
  - `Issues/OPEN/ISSUE-001-NRInfoPayload01-Cache-Miss.md`
  - `Make/compile-parallel.sh`
  - `Make/compile.sh`
  - `Scripts/AlpineVMs/alpine-phase4-ng.sh`
  - `Scripts/AlpineVMs/alpine-phase5-repo.sh`
  - `Scripts/AlpineVMs/alpine-phase5-source.sh`
  - `Scripts/AlpineVMs/check-run-scripts.sh`
  - `Scripts/AlpineVMs/fix-network.sh`
  - `Scripts/AlpineVMs/ng-vm.env.example`
  - `Scripts/AlpineVMs/run_NRNCS_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_NRNCS_on_Source_VM.sh`
  - `Scripts/AlpineVMs/run_PGCS_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh`
  - `Scripts/AlpineVMs/run_Repository_on_Repo_VM.sh`
  - `Scripts/AlpineVMs/run_Source_on_Source_VM.sh`
  - `Scripts/AlpineVMs/setup-ng-repo.sh`
  - `Scripts/AlpineVMs/setup-ng-source.sh`
  - `Scripts/AlpineVMs/soak-monitor.sh`
  - `Scripts/AlpineVMs/start-ng-repo.sh`
  - `Scripts/AlpineVMs/start-ng-source.sh`
  - `Scripts/AlpineVMs/stop-ng.sh`
  - `Scripts/Python/zmq_client.py`
  - `Scripts/Simple/Inter_OS_Content_Test_NRNCS.sh`
  - `Scripts/Simple/Intra_OS_NBTest.sh`
  - `Scripts/Simple/run_IoTTestApp.sh`
  - `Scripts/Simple/run_NRNCS.sh`
  - `Scripts/Simple/run_PGCS.sh`
  - `Scripts/Simple/run_Repository.sh`
  - `Scripts/Simple/run_Source.sh`
  - `Specs/PLAN-CONSOLIDATION-2026-07-16.md`
  - `Specs/RESULTS-SPEC-027/amend3v2-500-run.json`
  - `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`
  - `Specs/RESULTS-SPEC-027/soak30min-20260906.json`
  - `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`
  - `Specs/RESULTS-SPEC-027/spec031-after-2000.json`
  - `Specs/RESULTS-SPEC-027/spec031-after-500.json`
  - `Specs/RESULTS-SPEC-027/spec031-before-baseline.json`
  - `Specs/RESULTS-SPEC-027/spec033-r3-b1-controls-20260910.json`
  - `Specs/SPEC-008-ht-bid-raw-socket-discovery.md`
  - `Specs/SPEC-009-alpine-vm-ssh-run-scripts.md`
  - `Specs/SPEC-014-ngal-hash-mismatch-diagnosis.md`
  - `Specs/SPEC-019-payload-hash-logging.md`
  - `Specs/SPEC-020-subscription-redelivery-fix.md`
  - `Specs/SPEC-021-one-message-per-file.md`
  - `Specs/SPEC-022-nrinfopayload01-separate-messages.md`
  - `Specs/SPEC-022-one-file-per-message-nrinfopayload01.md`
  - `Specs/SPEC-027-pgcs-multithreading.md`
  - `Specs/SPEC-029-pgcs-self-mac-failfast.md`
  - `Specs/SPEC-033-R2-newmessage-exhaustion-output.md`
  - `Specs/SPEC-033-R3-B-gw-caller-safety.md`
  - `Specs/SPEC-033-R3-B2-gw-input-queue-null-safety.md`
  - `Specs/SPEC-033-R3-C-process-output-safety.md`
  - `Specs/SPEC-033-R3-caller-safety-amendment.md`
  - `Specs/SPEC-034-pgcs-low-rate-diagnostic.md`
  - `Specs/SPEC-035-nbtestapp-nrncs-intermediate.md`
  - `Specs/SPEC-036-deprecate-standalone-pss-girs-hts.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
  - `Specs/SPEC-038-public-repository-hygiene.md`
- Commit `cf7526503ddea85f077d80ff96dd9e8c7ec6eff7` — SPEC-037: record default branch promotion (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/NG-037-DEFAULT-BRANCH-PROMOTION-20260911.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- Commit `5ab91ce0c4731575f5ffef67c834d2628edc9762` — SPEC-037: record branch comparison and accepted gates (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Docs/DECISIONS/NG-037-BRANCH-COMPARISON-20260911.md`
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`
- Commit `b69a9ae689637163919782ff1d98082eff3692cc` — SPEC-037: define AIOPT3 default branch promotion (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-037-promote-aiopt3-default-branch.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
