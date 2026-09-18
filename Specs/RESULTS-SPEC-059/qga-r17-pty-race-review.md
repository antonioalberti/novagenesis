# SPEC-059 controller-only closeout: PTY process-group identity

Base diagnostic: `qga-r17-20260918`
Base evidence commit: `243416dbc1889710d87a192a60141a780c08e444`
Candidate runtime build: `68f0678694ed6a79fdb9b5818d211932c9e88e96`

## Root cause

The native PTY launcher invoked `/usr/bin/setsid --wait --ctty` through `Popen(start_new_session=False)`. `Popen` returned the wrapper PID before the external `setsid` wrapper had established its session. Registration immediately captured the inherited controller PGID. A local reproduction showed the inherited PGID immediately after `Popen`, changing to the wrapper PID after 0.2 seconds. The r17 PGCS/NRNCS launch records show the same inherited PGID `1145756`; Repository/Source non-PTY launches had `pgid == pid`. This explains the observed registration race and strongly explains the later NRNCS `identity-mismatch`.

The QGA error `Agent error: PID ld does not exist` remains a separate unresolved transport/completion issue; this patch does not claim to explain it.

## Patch

`launch_local_role_pty` now:

- passes the role argv directly to `Popen`;
- uses `start_new_session=True`, making the registered child the session/process-group leader before `Popen` returns;
- applies `TIOCSCTTY` in a child callback after session creation;
- starts `_PtyCapture` only after `Popen` returns, so no controller capture thread exists while `preexec_fn` runs;
- preserves merged PTY output and argument boundaries.

## Verification

- Before patch, regression test failed: actual PGID `1270023` differed from wrapper PID `1270026`.
- After patch, real PTY regression passed.
- Real PTY contract passed: `TTY=True FG=True`.
- SPEC-059 focused suite: `20 passed in 1.57s`.
- Complete AlpineVM suite: `190 passed, 7 subtests passed in 19.32s`.
- `py_compile`: passed for changed Python files.
- `git diff --check`: passed.
- No NovaGenesis trial was launched during this investigation.

## Astra classification

Astra review: `CONDITIONAL_READY` for the patch, subject to the threading inspection recorded above.

- Patch readiness: `READY_FOR_REVIEW_COMMIT`.
- SPEC-059: `DIAGNOSTIC_VALIDATED / ACCEPTANCE_PENDING`.
- M1: `NOT_ACCEPTED`.
- Release 1.0.0: `NOT_RELEASE_READY`.

## Next gate

`CONTROLLER_ONLY_REVIEW_CLOSEOUT` is complete for the PTY registration race. The QGA completion/teardown issue and Source→Repository delivery remain open. No new NG trial is started automatically; a future trial requires explicit operator scope and a fresh Astra review of the resulting evidence.
