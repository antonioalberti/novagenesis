# G2 canonical NG-ELC trial — 2026-09-14

**Branch:** AIOPT3
**Candidate commit:** `fce8577caab1ff2be053a23d2a8a7ba1d029ca78`
**Controller:** `Scripts/AlpineVMs/ng_remote_executor.py`
**Plan:** `Scripts/AlpineVMs/plans/L2-pgcs-only.example.json`
**Trial:** `g2-l2-fce8577`
**Evidence bundle:** `Specs/RESULTS-SPEC-054/g2-evidence-20260913/g2-l2-fce8577/`

## Scope and execution

- Guest pre-existing untracked IO/manifests were copied to `guest-prebuild-20260914/` and preserved in persistent guest backups before the builder ran.
- Both guests were fast-forwarded by the canonical builder to the same candidate SHA.
- Normal guest build completed with `rc=0` on both guests; receipts, `git fsck` and binary paths were verified independently.
- The controller restarted both VMs, retried the transient post-boot SSH preflight within the readiness deadline, launched both PGCS roles, observed both client/server socket markers, sealed evidence and completed teardown.
- VMs 101/102 were stopped after the trial and verified `stopped`.

## Separated verdicts

| Claim | Verdict | Evidence |
|---|---|---|
| Candidate SHA and build receipt on both guests | PASS | guest receipts and preflight result; SHA `fce8577` |
| Guest tools, interface/MAC and no stale NG processes | PASS | preflight result in controller events |
| PGCS Source/Repository socket readiness | PASS | `controller-events.jsonl`, both roles `OBSERVED` |
| Controller reboot-to-SSH lifecycle | PASS | focused RED→GREEN test plus trial events |
| Teardown and sealed evidence | PASS | result `teardown_result=PASS`, manifest `COMPLETE` |
| L2 functional runtime oracle | INCONCLUSIVE | plan is diagnostic-only and has no functional runtime oracle |
| Five-launcher E7/photo smoke | OPEN | not promoted from the two-PGCS L2 diagnostic trial |
| Payload integrity/G9 | OPEN | requires the matched 100-photo workload and independent hash oracle |

The first run at `466777b` exposed a controller-generated inline Python `SyntaxError`; it was fixed and tested in `466777b`. The corrected controller then exposed the post-reboot SSH readiness gap; the retry was added and committed in `fce8577`.

## Validation after the fix

- Focused lifecycle RED→GREEN: **PASS**
- Full Alpine/controller suite: **64/64 PASS**
- Python compilation: **21/21 PASS**
- Shell syntax: **25/25 PASS**
- Remote canonical build: **PASS** on both guests
- Remote canonical L2 trial: **build/preflight/readiness/teardown PASS**, runtime **INCONCLUSIVE by declared scope**

This result does not close G3, subscriptions, Repository readback, payload delivery or G9.
