# Astra review — SPEC-055 / NG-ELC local

**Date:** 2026-09-13  
**Reviewer:** GPT-6 Astra  
**Scope:** read-only review of SPEC-055, NG-055, local NG-ELC source, tests and sealed local trial bundle  
**Repository:** https://github.com/antonioalberti/novagenesis.git  
**Branch:** AIOPT3  
**HEAD reviewed:** `1af604dad66148618cc0e578c28adb4a9829e607`  
**Verdict:** `NO-GO` for SPEC-055 acceptance

> The review is preserved as received. It is not a substitute for local verification of omitted helpers, full repository contents, raw manifest hashes or live host state.

1. **Verdict: NO-GO for SPEC-055 acceptance. Design direction is correct; lifecycle implementation is incomplete.** Review is grounded in the supplied code/evidence; repository contents, omitted helpers, raw manifest hashes and live host state were **not independently verified**.
   - Correct: separate local configuration, argv-based launch, canonical role order including PGCS `-lc`, marker readiness, process sessions, reverse stop order and result fields.
   - Contract gaps: `timeouts.total` is never enforced; readiness receives a fresh budget per role. Local log quota is validated but not enforced in the shown lifecycle. Observation does not check role crashes. Local SIGTERM handling lacks the remote path’s cleanup protection.
   - Local `trial_id` bypasses `trial_paths()` validation, allowing an absolute/path-traversing value to escape the validated evidence root. The plan fixes five matching filenames as a count, but does not define synthetic workload generation or establish fresh, uncontaminated IO.

2. **First-trial evidence: useful functional observation, insufficient acceptance evidence.**
   - Events report all four readiness markers, progression from Repository 3/5 to 5/5, in-memory hash equality, and termination of four tracked leaders. This is stronger than an isolated `PASS`, but not independently reproducible byte-exact evidence.
   - **The manifest preserves neither JPEG artifacts nor either name→SHA-256 map.** `local_file_oracle()` calculates maps and discards them; its event records only counts, equality and discrepancy lists. Hashing that event log seals an assertion—not the underlying payload comparison.
   - Build and IO under `/tmp` **do not themselves violate §4**; evidence must be durable. However, §8 requires evidence preservation before temporary cleanup. No artifact-copy or temporary-removal phase appears in `run_local_trial()`. The reported later absence of `/tmp/ng-*` cannot establish preservation order or controller-owned cleanup.
   - Provenance records HEAD and executable hashes, but not dirty source contents, controller identity, build recipe/toolchain/options, or a sealed plan/configuration snapshot. Those binary hashes do not establish a build from `1af604d`.

3. **Required blockers/amendments before “Implemented/accepted”:**
   - **Oracle/result:** preserve both maps and independently rehashable payload evidence; establish initial Source workload and empty Repository. Reject aliased oracle directories. Prevent crashes from yielding PASS. Define and test divergence handling: currently a complete mismatch can later be overwritten by PASS or INCONCLUSIVE; counts with extras remain INCONCLUSIVE. The marker oracle also needs accumulated unique `(role,id)` coverage, not `len(observed["matches"])`.
   - **Process cleanup:** inventory NG processes before launch; verify recorded PID/starttime/executable and surviving group members after stop. Current cleanup skips signalling groups whose leaders already exited, waits only for leaders, and logs the process snapshot without using it to determine teardown success.
   - **IPC cleanup:** `after − before` identifies newly appearing IPC, **not trial ownership**; it can delete unrelated concurrent IPC. Require attributable ownership or enforced isolation, fail closed on inventory uncertainty, and seal baseline/final inventories. The omitted snapshot helper’s error handling cannot be verified.
   - **Sealing/lifecycle:** enforce total deadline and local log quota; validate trial-path containment; handle termination and cleanup exceptions with a durable non-success result. `COMPLETE` must require the specified evidence inventory, not merely successful manifest writing. Seal artifacts before narrowly scoped temporary removal and record final verification.
   - **Tests/provenance:** add targeted RED→GREEN regressions for these failures and rerun remote regression coverage. The supplied “bounded local lifecycle” test never creates its IO directory: `local_preflight()` rejects it before launch, so its passing assertions do **not** demonstrate a lifecycle. Correct premature checked criteria and record an implementation commit.

4. **G3 local sub-scope: cannot close on this trial.** Retain it as a **reported local 5/5 observation, acceptance pending**. After the blockers and a reviewed fresh trial, the local sub-scope may close explicitly without closing G3 overall, its G2 prerequisite, or any multi-VM gate. The reported dirty tree blocks a frozen release claim and, with this incomplete provenance, prevents attributing SPEC acceptance to the named HEAD. It does not invalidate every observation, but **G0/G10 remain BLOCKED**.

5. **Bounded next action:** Hermes should complete one focused NG-055 hardening/test pass, freeze an identifiable clean implementation candidate, then run **one fresh bounded five-photo local trial**. Preserve plan/config, build provenance, both artifact sets/maps, full logs, and attributable pre/post process/IPC inventories before removing temporaries. Submit that full bundle and test output for read-only re-review. Do not tag, mark SPEC-055 Implemented, or promote this trial into multi-VM acceptance.
