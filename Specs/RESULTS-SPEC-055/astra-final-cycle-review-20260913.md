# Astra final-cycle review — SPEC-056 / NG-ELC

**Date:** 2026-09-13  
**Reviewer:** GPT-6 Astra  
**Scope:** current SPEC-056 implementation, 30/30 tests, final local trial and evidence bundle  
**Repository:** https://github.com/antonioalberti/novagenesis.git  
**Branch:** AIOPT3  
**HEAD reviewed:** `1af604dad66148618cc0e578c28adb4a9829e607`  
**Verdict:** partial hardening; SPEC-056 remains `In Progress`

> The review is preserved as received. It is not a release sign-off and does not close NG-055 or G3.

1. **Demonstrated: useful partial hardening, not full implementation.**
   - Supplied code/tests demonstrate safe-component trial-ID validation, explicit local paths, observed SHA-256 maps, independently copied payloads, sticky-failure and unique-marker helpers, process-group residual detection, and nonzero acceptance blocking.
   - The reported final trial demonstrates five matching Source/Repository files, ten preserved JPEGs, 23 independently hash-verified manifest entries, and successful post-cleanup absence observations.
   - The reported 29/29 tests and Debug build exit 0 are positive results, not proof of all normative gates. Repository state, complete test output and omitted utilities **cannot be independently verified here**.

2. **Remaining blockers prevent SPEC-056 becoming Implemented.**
   - **Critical — ownership safety, R01/C01/C02:** `local_remove_new_ipc()` deletes current-user IPC using only `after − before`; inventory failure becomes an empty set. No exclusive isolation or positive attribution is established. Stopping remains leader-dependent; surviving groups are detected but not cleaned when the leader already exited.
   - **High — oracle correctness, R02/R03:** no controller-created fresh workload, initial expected map or Repository-empty proof. Equality is only Source↔Repository; extras/missing-count mismatches can remain `INCONCLUSIVE`. Preservation compares against a newly observed snapshot, not the successful observation and initial workload. A diagnostic file-oracle plan can still return runtime PASS.
   - **High — bounds/finalization, R04/R05:** no teardown reserve, aggregate/artifact quota or bounded hashing/copying; file observation bypasses log checks. Early role death is `INCONCLUSIVE`, observation can end at first equality, and marker-result assignment can overwrite failure. Local SIGTERM handling, immediate post-spawn registration and exception-resilient cleanup are missing.
   - **High — provenance/evidence, R06/§5/C03:** dirty input contents, helper/environment/toolchain/library identities and verified build linkage are absent; merely supplying a manifest file sets linkage true. Schema remains **v1**, without required workload/ownership/cleanup/preservation inventories, transactional terminal seal or offline eligibility verification. No controller-owned temporary-removal phase exists. Manifest failure incorrectly changes teardown to `UNKNOWN` and retries.
   - **High — regression coverage, §7/§8.1:** the original lifecycle fixture **still omits IO**, permitting success after preflight rejection without launch assertions. The additional bundle fixture starts with Repository populated and accepts an empty build manifest. Required destructive/fault-injection coverage and implementation commit are not demonstrated.
   - **Separate acceptance prerequisites:** dirty candidate and absent build linkage independently block acceptance. Cleaning and rebuilding alone would not resolve the implementation defects above.

3. **Final trial: diagnostic-only evidence, not accepted SPEC-056 evidence.**  
   Preserve it as a reported **5/5 byte-equality observation with verified copies and observed cleanup**. Exit **21** and `local_acceptance_eligible=false` appropriately prevent acceptance. Verified hashes establish integrity of listed files—not required-artifact coverage, initial-workload correctness, ownership-safe cleanup or schema-v2 sealing.

4. **Status is correct; records need qualification.**  
   Keep **SPEC-056 In Progress** and **NG-056 em-andamento under NG-050**; no implementation or acceptance approval is granted. Preserve the original result unchanged, but annotate externally that `PASS/PASS/COMPLETE` is the existing controller’s classification, **not SPEC-056-compliant completeness**. Keep SPEC-055 historical acceptance pending. “No C++ changes in this cycle” may be recorded as reported; provenance lists dirty C++ files, so the broader candidate’s scope cannot be established from HEAD/status alone.

5. **One bounded next action:** complete an **implementation-only R01/C02 ownership-safety increment**, using isolated fake IPC fixtures to prove unrelated concurrent IPC survives and inventory ambiguity prevents deletion and PASS. Do not run another acceptance trial yet. After all implementation gates are demonstrably complete, the clean linked candidate, fresh five-photo trial and explicit Astra approval remain separate local acceptance gates; NG-050, release and multi-VM gates remain untouched.
