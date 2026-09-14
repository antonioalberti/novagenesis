# SPEC-048 — Resultado do trial GDB receptor

**Date:** 2026-09-12  
**Commit:** `1af604d`  
**Guests:** VM101 repo61 / VM102 source36  
**Window:** 180 s  
**Production code changed:** no

## Provenance and setup

- Both guests passed preflight at the exact commit.
- GDB symbols for `PGHelloIHC01::Run` and line 240 were found on both binaries.
- The first GDB attempt is excluded as setup-invalid: the generated command file contained literal `\\n` separators and no PGCS started.
- This result uses the corrected command file with real newline separators.

## Gate results

| Gate | Result | Evidence |
|---|---|---|
| PGCS startup/socket | PASS | Both logs show client/server socket creation and correct peer MAC |
| On-wire hello | PASS | 36 `0x1234` frames: 18 in each direction |
| Receiver handler entry | PASS | `RECEIVER_HANDLER_ENTRY`: 18 repo + 18 source |
| Receiver handler return | PASS | `RECEIVER_HANDLER_STATUS_OK`: 18 repo + 18 source |
| Directional correlation | PASS limited | Matching frame directions and repeated handler executions; GDB backtraces identify the handler and process |
| Binding state oracle | OPEN | This trial did not directly assert the resulting HT binding state |
| NRNCS readiness | NOT TESTED | Correctly held back until this gate |
| Teardown | PASS | Both cleanup logs show zero final processes, SHM and semaphores; VMs stopped |

## Interpretation

The receiver-processing gate is demonstrated for both directions through the application handler and successful return. This closes the SPEC-048 receiver-processing prerequisite for the next staged experiment, but does not by itself prove the binding state transition or NRNCS discovery. The next trial may introduce NRNCS only with its own readiness and binding oracles.

Evidence:

- `trial-contract.txt`
- `remote-repo-pgcs.log`, `remote-source-pgcs.log`
- `repo-supervisor.txt`, `source-supervisor.txt`
- `ng048-tcpdump.txt`
- `repo-cleanup.txt`, `source-cleanup.txt`, `vm-stop.txt`
