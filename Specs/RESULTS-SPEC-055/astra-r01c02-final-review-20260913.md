# Astra final R01/C02 review — SPEC-056 / NG-ELC

**Date:** 2026-09-13  
**Reviewer:** GPT-6 Astra  
**Verdict:** HOLD — increment not accepted

> Preserved as received; it does not authorize a release or real acceptance trial.

1. **Verdict: HOLD — R01/C02 increment not accepted yet.**  
   Reviewed only the supplied sections; repository state and the reported **34/34** execution cannot be independently verified. The changes improve IPC uncertainty handling and cleanup of **already-observed** detached descendants, but do not yet establish fail-closed teardown. Keep **SPEC-056, NG-055 and G3 open**.

2. **Blocking correctness findings — process cleanup**
   - **Unreadable is treated as absent.** `process_starttime()` returns `None` for permission/read/parse failures; `local_stop_tracked_descendants()` then skips that PID and can report success. Likewise, `process_state() == None` is accepted as termination. Only confirmed disappearance should count as gone; other failures must leave cleanup incomplete.
   - **Start-time parsing is inconsistent.** `process_starttime()` uses whole-record `.split()[21]`, which breaks when `/proc/<pid>/stat`’s parenthesized `comm` contains spaces. Descendant discovery correctly splits after the closing parenthesis, so the two helpers can disagree on the same live identity.
   - **Identity is checked only before TERM.** State polling and subsequent KILL use the PID without validating its start time again. PID reuse can therefore target an unrelated process; the initial check also has a check-to-signal race. Identity-safe signaling needs an explicit solution, not merely the initial comparison.
   - **Descendant discovery is sampling, not containment.** A child can detach and become reparented before observation, or be created after the last observation. No final discovery is shown; `descendant_scan_complete` also defaults to `True` without a scan. Thus “complete” means readable sampled records, not complete descendant coverage.
   - **Teardown can abort before sealing evidence.** Permission errors from signaling, `/proc` enumeration failures, and exceptions inside the `finally` teardown are not handled locally. `local_group_members()` additionally suppresses read/parse failures and can return an apparently clean empty list.

3. **IPC: improvement is real, but closure is unproven**
   - `local_ipc_ids()` correctly returns `None` for command errors, timeouts and missing recognized headers.
   - Its parser still silently skips every non-`0x` row. A recognized header plus unsupported/malformed data rows can become a falsely empty inventory rather than unavailable.
   - Full `attribute_new_ipc()`, `local_ipc_details()` and `local_remove_new_ipc()` implementations were not supplied; end-to-end attribution, deletion safety and post-removal verification **cannot be verified**.

4. **Tests genuinely cover useful paths, not the full R01/C02 claims**
   - Covered: failed/headerless inventory, unavailable-details/no-`ipcrm`, unrelated attribution, same-group residual detection, and a live-parent detached-child happy path.
   - Missing: timeout/OSError tests; `None` baseline/current inventory through removal; recognized-header malformed rows; ENOENT versus permission/parse failures; spaced `comm`; identity mismatch/reuse and KILL escalation; no-scan/early-reparenting cases; teardown exception-to-result propagation.
   - The detached-child test asserts helper success, not independently verified child exit. Its fallback kills only the original group, so a regression can leave the escaped child alive temporarily. Fixed sleeps also make discovery timing-dependent.

5. **Residual acceptance blockers and one bounded next step**
   - Increment blockers are the cleanup/parser defects and missing adversarial coverage above.
   - Separate acceptance blockers remain: dirty worktree, unverified build linkage, and no post-increment acceptance evidence. `build_linkage` currently checks only manifest existence; an empty `{}` qualifies without binding binaries to source. The previous PASS/PASS/COMPLETE bundle with exit **21** is historical, not acceptance of this increment.
   - **Next step:** one controller-only corrective/test increment addressing these specific failure paths, followed by read-only re-review of the complete touched helpers and test results. No remote or C++ changes, commits, release action, or new real trial before increment acceptance.
