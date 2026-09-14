# Astra IPC post-increment review — SPEC-056 / NG-ELC

**Date:** 2026-09-13  
**Reviewer:** GPT-6 Astra  
**Scope:** R01/C02 IPC attribution and PGID cleanup increment  
**Repository:** https://github.com/antonioalberti/novagenesis.git  
**Branch:** AIOPT3  
**Verdict:** not acceptance-ready; SPEC-056/NG-055/G3 remain open

> The review is preserved as received. It does not authorize a release or acceptance trial.

**Verdict: not acceptance-ready. Keep SPEC-056, NG-055 and G3 open.** Review is limited to the supplied sections; dirty-tree contents, omitted helpers, host behavior and the reported 30/30 results cannot be independently verified.

1. **IPC attribution is conservative at the function boundary, but not end-to-end fail-closed.**
   - `attribute_new_ipc()` correctly rejects new objects with unknown/non-trial creators. `local_remove_new_ipc()` then performs **no deletions**, including otherwise attributed objects. Either details inventory being `None` rejects cleanup even with an empty delta.
   - However, `local_ipc_ids()` converts nonzero `ipcs` status into an empty set. An unavailable post-run or final inventory can therefore appear empty and permit success when details queries succeed. Baseline failure also loses the distinction between pre-existing and newly created objects.
   - `subprocess.run()` timeout/launch exceptions are not converted into inventory-unavailable results. In the shown `finally`, they can interrupt remaining finalization.
   - Attribution uses **numeric creator PID only**. PID reuse can misattribute unrelated IPC; only role-leader PIDs are supplied, so legitimate child-created IPC is rejected. This is not proof of exclusive resource ownership under R01.
   - The `ipcs -s -p` output contract needs target-platform verification: semaphore creator-PID availability cannot be assumed. Successful command execution with unrecognized output currently becomes `{}`, not an explicit inventory error.

   **Conclusion:** ordinary unrelated creators are protected, assuming accurate inventories and stable identities; general concurrent-IPC safety and unavailable-inventory handling are not established.

2. **Explicit reasons currently stop at the helper.**
   `local_remove_new_ipc()` discards `reason`, `owned` and `unattributed`, returning only a Boolean and the complete delta. The shown inventory event therefore cannot distinguish `inventory-unavailable` from `unattributed-ipc` or an `ipcrm` failure. The tests assert the unavailable reason, but not the unrelated reason or persisted evidence. The increment adds reason computation—not verified end-to-end reason reporting.

3. **PGID detection improves observability, but does not prove trial-process absence.**
   - The post-stop scan executes even when the leader has already exited. Visible surviving members of that PGID correctly force teardown `FAIL`.
   - It neither terminates those residuals nor catches descendants that leave the group using `setsid()`/`setpgid()`.
   - Per-process `/proc` read failures are silently skipped, allowing incomplete inventory to resemble absence. A single scan is also not an ownership boundary.
   - The only supplied PGID test checks membership of the current process; it does not exercise teardown.
   
   **Concrete testable gap:** launch a leader that forks a same-PGID child and exits; verify teardown fails and records the child’s identity. Add a detached-child variant: it must not produce teardown PASS unless escape is prevented or the child is independently tracked. Separately, `process_starttime()` splits the entire stat record, unlike the new group parser, so names containing spaces can corrupt its identity check.

4. **Remaining blockers for this increment**
   - End-to-end unavailable/ambiguous inventory propagation, including baseline/final queries and exceptions.
   - Ownership assurance beyond bare creator PIDs, with a verified semaphore inventory strategy.
   - Persisted attribution decisions/reasons and verified residual/escaped-process handling.
   - Adversarial cleanup tests—not just helper tests.
   - Clean, build-linked provenance and a **new real acceptance trial after these changes**. The earlier reported PASS/PASS/COMPLETE, 23-file/10-JPEG trial remains diagnostic evidence: exit 21 and its pre-increment timing prevent sign-off.

5. **One bounded next step**
   Have Hermes complete one controller-only hardening/test increment covering failed baseline/final IPC inventory, details timeout/unsupported format, mixed unrelated IPC with **zero `ipcrm` calls**, and surviving/detached children. Require durable reason codes and non-PASS outcomes for every unknown state. Review that increment before spending another real acceptance trial; no production-code or remote-contract changes.
