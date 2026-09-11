# NG-037 Default Branch Promotion Result

**Date:** 2026-09-11
**Repository:** `antonioalberti/novagenesis`
**Candidate baseline:** `5ab91ce`
**Default branch before:** `master`
**Default branch after:** `AIOPT3`

## Runtime gate

The first cross-process attempt was invalidated because the test command used the Source peer MAC in uppercase and the transmitted Ethernet destination became `08:00:27:79:00:15`. Proxmox `tcpdump` confirmed the malformed destination. No NovaGenesis code defect was inferred from that attempt.

The corrected 10-photo smoke test used the canonical literal `08:00:27:79:bb:15` and passed bidirectional PGCS discovery, NRNCS/Source discovery and 10/10 byte-exact delivery.

The required 100-photo retest then used:

- commit `5ab91ce` on both guests;
- normal runtime profile with no standalone PSS/GIRS/HTS processes;
- clean VM/process/IPC preparation;
- peer MACs `08:00:27:65:00:08` and `08:00:27:79:bb:15`;
- `StressTest=1`, `StressInterval=1`;
- 100 fresh unique JPEGs.

Result:

- Source: 100 JPEGs;
- NRNCS cache: 100 JPEGs;
- Repository: 100 JPEGs;
- Source SHA-256 map = NRNCS SHA-256 map = Repository SHA-256 map;
- no missing or extra files;
- zero `ERROR`, `ALARM` or `FATAL` matches;
- evidence checksum verification passed.

Evidence:

`IO/NG-037-cross-process-20260911-100-photos/`

## Branch administration gate

The remote API initially reported `default_branch: master`. The default branch was changed through the repository API to `AIOPT3` and read back as `AIOPT3`.

Independent verification:

- `origin/HEAD` refreshed to `origin/AIOPT3`;
- `git ls-remote --symref origin HEAD` points to `refs/heads/AIOPT3`;
- `origin/AIOPT3` points to the promoted baseline and subsequent documentation commit;
- `origin/master` remains present at `e305782`;
- a clean shallow clone selected `AIOPT3` by default at `5ab91ce` before the final documentation commit.

`master` was not deleted, rewritten or force-pushed.

## Decision

AIOPT3 is now the principal/default NovaGenesis branch. `master` remains a secondary historical/compatibility branch. Legacy PSS/GIRS/HTS functional testing remains on-demand under SPEC-036; this does not block the branch promotion.
