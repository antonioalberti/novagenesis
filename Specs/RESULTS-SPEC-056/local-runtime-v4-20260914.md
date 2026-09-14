# Local runtime trial — PGCS crash after readiness

**Date:** 2026-09-14
**Branch:** `AIOPT3`
**Candidate:** `d303b05ad2ea379308b7dbb41445f456f595d2c3`
**Trial:** `m0-runtime-v4`
**Plan:** `Scripts/AlpineVMs/plans/local-intra-os.example.json`

## Result

The fresh canonical local NG-ELC trial passed provenance/build linkage eligibility and launched the real processes, but did not reach payload runtime acceptance:

```text
runtime_result=INCONCLUSIVE
teardown_result=UNKNOWN
 evidence_result=INCOMPLETE
exit_code=20
```

The final result recorded an evidence publication blocker because the early abort left no repository artifact area. This is a consequence of the runtime abort, not a successful trial.

## First failing edge

- PGCS launched and emitted `State: Operational`.
- NRNCS launched, but during its readiness wait the controller observed PGCS exit with return code `-11` (SIGSEGV).
- The controller stopped NRNCS; Repository and Source ContentApp roles were not launched.
- The lifecycle inventory recorded no residual trial processes or owned IPC after cleanup.

PGCS stdout ended immediately after the Core block was created. No stderr was emitted. The retained bundle is under the ignored local evidence root:

```text
cmake-build-debug/.ng-evidence/local-intra-os-v4-d303b05/m0-runtime-v4/
```

## Status — invalid execution environment

This trial was launched by the local controller without the mandatory privileged execution path. Historical NovaGenesis runbooks require `sudo` for `clean.sh` and for PGCS/NRNCS/ContentApp because raw sockets, SysV SHM and POSIX semaphore ownership are privilege-sensitive.

The trial therefore cannot establish a NovaGenesis runtime crash or a readiness race. Its provenance/build-linkage result remains useful as a controller check, but its runtime result is invalid for M0/M1 acceptance and must not be compared with the historical root-run trials.

The next valid run must use:

```bash
sudo bash Scripts/Simple/clean.sh
```

followed by zero-process/zero-IPC verification and a same-privilege launch of the complete local lifecycle.
