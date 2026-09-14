# Astra post-test review — SPEC-056 / NG-ELC

**Date:** 2026-09-13  
**Reviewer:** GPT-6 Astra  
**Scope:** current SPEC-056 implementation, 27/27 tests, fresh local trial and sealed evidence bundle  
**Repository:** https://github.com/antonioalberti/novagenesis.git  
**Branch:** AIOPT3  
**HEAD reviewed:** `1af604dad66148618cc0e578c28adb4a9829e607`  
**Classification:** diagnostic-only; local payload evidence independently rehashable  
**SPEC-056 status recommendation:** `Proposal → In Progress` only

> The review is preserved as received. It is not a release sign-off and does not close NG-055 or G3.

1. **Demonstrated: meaningful partial implementation, not full R01–R06 compliance.**  
   Based on the supplied code, tests and trial evidence; repository/host state was not independently inspected here.
   - **R01 partial:** safe trial-ID validation, four explicit absolute local paths, rejection of evidence under `/tmp`, and rejection of existing trial directories.
   - **R03 partial:** observed Source/Repository name–size–SHA-256 maps, preserved payload copies and copy-map verification. Sticky-verdict and unique `(role, marker_id)` helpers have unit coverage.
   - **R04 partial:** monotonic readiness/observation deadlines capped by a shared deadline, process-exit polling and per-file log checks.
   - **Fresh trial:** observed Repository progress from 3/5 to matching 5/5; ten preserved JPEGs reproduce both maps. The reported independent verification validates all 22 manifest entries, including `result.json`. Four role stops, independently verified process/IPC absence, and temporary removal after evidence validation are positive **trial-specific observations**.
   - Reported **27/27 tests passed**; this does not establish all normative acceptance gates.

2. **Open requirements and severity — all below block SPEC-056 acceptance.**
   - **Critical — ownership/destructive cleanup, R01/R05/lifecycle:** `local_remove_new_ipc()` deletes same-user IPC by set difference, not trial ownership; `local_ipc_ids()` treats command failure as an empty inventory. No exclusive resource lock or conflicting-process preflight exists. Teardown tracks leaders, skips groups whose leaders exited, and does not verify descendant absence. Successful cleanup this time does not establish safe attribution.
   - **High — paths/fresh workload, R01/R02:** no filesystem durability classification, full containment/race protection, Source/Repository alias/nesting/shared-inode checks, or cleanup-target isolation. No controller-created fresh IO, preserved initial expected map, deterministic-generator record or prelaunch proof of empty Repository. Canonical plan contents were not supplied, so its full declaration requirements cannot be verified.
   - **High — false-PASS paths, R03:** `local_file_oracle()` compares current Source↔Repository, not initial workload. Partial-count mismatches and extras can remain `INCONCLUSIVE` instead of latching `FAIL`. Preservation compares copies with a newly sampled map—not the successful observation—so changed final contents can escape rejection. Stable-read/link-race protection is absent. A file-oracle plan with `diagnostic_only=True` can still return PASS.
   - **High — runtime/liveness, R04:** no teardown/evidence reserve or end-to-end operation budget; no aggregate log/artifact quota; file observation bypasses log checks. Observation stops at first oracle success without a final endpoint liveness check. Readiness crashes become `INCONCLUSIVE`, not sticky `FAIL`; marker post-processing can overwrite an observation crash verdict.
   - **High — abort/finalization, R05:** local mode lacks unified SIGTERM/SIGINT handling and repeated-signal protection. `Popen()` registration occurs after fallible identity operations. Cleanup/logging exceptions can stop remaining cleanup; unfinished-trial ownership/recovery is absent.
   - **High — provenance/schema/sealing, R06 and §§4–5:** schema remains v1. Missing dirty-input snapshots, controller/helper identities, plan/config/environment capture, build recipe/toolchain/input linkage, library identity and executable-drift enforcement. Required ownership, inventory, cleanup and preservation records and controller-gated offline validation are absent. `COMPLETE` is optimistic; exit zero ignores acceptance blockers. **HEAD plus binary hashes does not prove a build from that HEAD.**
   - **High — test validity:** `test_local_trial_uses_same_bounded_result_contract` still omits IO creation and can pass without launching roles. The bundle test prepopulates Repository, proving packaging—not fresh delivery. Failure-injection coverage for the gaps above is not demonstrated.

3. **Fresh-trial classification: diagnostic-only, with independently rehashable local payload evidence.**  
   It is **not inconclusive about the observed 5/5 equality and preservation**, but it is **not accepted SPEC-056 local evidence**. Missing workload baseline, build linkage and schema-v2 lifecycle verification prevent acceptance despite reported `PASS/PASS/COMPLETE`. `/tmp` build/IO placement is permitted; incomplete dirty-source provenance is the blocker, not that placement. External cleanup verification does not substitute for controller-owned lifecycle enforcement.

4. **Status: GO for Proposal → In Progress only.**  
   Concrete implementation and trial evidence justify that status. **Not Implemented; NG-055 acceptance and G3 overall remain open.** Record the diagnostic-only classification and acceptance blockers explicitly. No multi-VM or release gate is closed, and the historical bundle is not retroactively accepted.

5. **One bounded next action:** implement a **local-only acceptance-eligibility guard** in the existing controller/result contract. Missing required provenance/build linkage must produce explicit blockers, `local_acceptance_eligible=false`, and nonzero exit—even when component observations are `PASS/PASS/COMPLETE`. Add one regression covering this exact fresh-trial provenance shape; leave remote behavior unchanged.
