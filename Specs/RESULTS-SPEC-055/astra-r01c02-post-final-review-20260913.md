# Astra post-final review — SPEC-056 / NG-ELC

**Date:** 2026-09-13  
**Reviewer:** GPT-6 Astra  
**Scope:** controller-only R01/C02 corrective increment  
**Verdict:** HOLD — increment only; no release acceptance

## Verdict: HOLD — controller increment only

Read-only assessment of the supplied excerpts; no repository actions or independent test execution. The reported **37/37 passing tests** support specific cases, but do not resolve the code-grounded blockers below.

### Assessment of the four fixes

1. **Process identity and stop handling — partially correct.**  
   The unified `/proc` parser uses the correct field offsets, and the visible descendant-stop path correctly treats unknown identity after TERM as failure rather than success. The group-stop tests cover identity change before KILL and TERM signal errors.

   **Blocker:** descendant TERM/KILL calls catch only `ProcessLookupError`. A `PermissionError` or other `OSError` escapes into the caller’s `finally`, potentially skipping subsequent role cleanup, evidence preservation, IPC cleanup, and result finalization. `/proc` directory enumeration errors likewise escape the completeness-reporting helpers. Group-stop exception containment does not protect these paths.

2. **Marker finalization preserves FAIL — supported by the focused test, not fully inspectable.**  
   The test explicitly requires `FAIL` to remain `FAIL` despite satisfied markers, and the visible call site uses `marker_runtime_result`. Its implementation is absent, so full code-level verification remains outstanding.

3. **IPC parsing and persisted cleanup reasons — improved, but incomplete.**  
   Missing headers, command failures, and several malformed rows now yield unavailable inventory; unavailable attribution prevents `ipcrm`. The caller records `ipc_cleanup_reason`.

   **Blockers:**
   - Recognized-header validation remains partial: `local_ipc_ids` accepts a key such as `0xNOTHEX` and a three-column row despite the expected additional columns. `local_ipc_details` accepts a nonnumeric fourth field. Thus blanket rejection of malformed rows is not established.
   - `local_remove_new_ipc` still returns `(False, new_ids, None)` after removal errors or a residual post-cleanup delta. The failure flag persists, but its reason does not.
   - Cleanup receives only role-leader PIDs, not tracked descendant PIDs. Descendant-created IPC therefore remains unattributed: conservative, but not complete descendant cleanup.

4. **PGID completeness and detached descendants — useful but not sufficient for closure.**  
   Unreadable stat records mark scans incomplete, and the supplied test demonstrates stopping an already-discovered `setsid` child by PID/starttime.

   **Remaining gap:** a PPID snapshot cannot discover a child that detached and was reparented before discovery. Tracking integration is omitted, and `descendant_scan_complete` defaults to `True` when absent. The excerpts therefore do not establish that every role was scanned or that escaped descendants cannot produce a false-clean teardown.

### Evidence limitation

The paste is discontinuous: the descendant-stop tail and most of the group-stop implementation are missing. This is a review limitation, **not evidence that the worktree has syntax errors**. No post-change real trial exists; the earlier diagnostic exit **21** does not validate these changes.

### One bounded next step

Complete one **controller-only teardown-hardening pass** addressing the concrete gaps above, with failure-injection regressions and complete helper/tracking excerpts for re-review—no real trial, commit, release, remote, or C++ action.

Keep **SPEC-056 / NG-055 / G3 open**.
