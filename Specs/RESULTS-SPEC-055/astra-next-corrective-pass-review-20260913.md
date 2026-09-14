# Astra next corrective-pass review — SPEC-056 / NG-ELC

**Date:** 2026-09-13  
**Reviewer:** GPT-6 Astra  
**Verdict:** HOLD

> Preserved as received; no release or acceptance authorization.

## HOLD

Read-only review of the supplied excerpts for AIOPT3 at stated HEAD `1af604dad66148618cc0e578c28adb4a9829e607`. No actions taken. The reported **40/40** is not independently verified; the pasted code is discontinuous, including the group-stop body, so that implementation cannot be fully reviewed.

The marker helper preserves `FAIL`, TERM-side descendant errors become residual failures, and IPC cleanup reasons are recorded. However, remaining code-grounded blockers prevent accepting this increment:

1. **Descendant KILL errors still escape cleanup.**  
   `local_stop_tracked_descendants()` catches only `ProcessLookupError` around `os.kill(pid, SIGKILL)`. A `PermissionError` or other `OSError` propagates from the trial’s `finally` block, potentially skipping remaining roles, IPC cleanup, and evidence finalization. The supplied signal-error test exercises TERM only.

2. **`/proc` enumeration containment misses lazy failures.**  
   Both scanners wrap `Path("/proc").iterdir()` creation, but iterate outside the `try`. Errors raised when advancing the iterator can escape rather than return `complete=False`. The test patches the call itself to raise, so it does not cover this failure mode.

3. **IPC parsing does not meet the stated validation claim.**  
   Headers are accepted merely because they contain `shmid` or `semid`, without validating column layout. `local_ipc_ids()` requires only three row fields; it does not validate permissions, size/count, or attachment fields. For example, a recognized header followed by `0x1 7 tester` is accepted for that owner despite missing required columns. Malformed inventory can therefore be treated as authoritative.

4. **Descendant completeness can still be falsely inferred.**  
   `descendant_scan_seen=True` proves only that a scan occurred. A child can fork and detach between scans, then be reparented when the leader exits. The final scan is skipped when `proc.poll()` reports exit; earlier `complete=True` remains sufficient, and an escaped child outside the original PGID can be missed by both checks. The supplied detached-child test keeps the parent alive while discovering the child—it does not exercise this gap.

5. **The outer exception handler can erase an observed runtime failure.**  
   `except Exception` unconditionally assigns `runtime = "INCONCLUSIVE"`. Thus the marker helper’s sticky-FAIL behavior does not protect a prior `FAIL` if a subsequent observation/runtime operation raises.

**One bounded next step:** prepare one controller-only corrective patch with regression cases for these five paths—especially lazy iterator failure, TERM-success/KILL-denied, and leader-exit/detached-child escape—and supply the complete changed functions plus test results for re-review. No release, commit, remote, or C++ action is warranted.

Keep **SPEC-056 / NG-055 / G3 open**. The dirty worktree and absence of a post-change real trial do not alone decide increment acceptance, but neither the reported tests nor the earlier diagnostic exit **21** justify closure.
