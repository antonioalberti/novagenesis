# Matched normal-path comparison — 100 JPEGs

Date: 2026-09-12

## Scope

The same normal-path topology and launch sequence were executed at two commits:

- `1af604dad66148618cc0e578c28adb4a9829e607`
- `21a5512b0dc34df0c7f730805e55637144087073`

Each trial used fresh generated JPEGs, PGCS on both VMs, NRNCS on the Source VM, Repository ContentApp, and Source ContentApp launched last. VMs, logs, staging, caches, outputs, and cleanup were captured separately.

## Results

| Commit | Source | NRNCS cache | Repository | SHA-256 equality |
|---|---:|---:|---:|---|
| `1af604d` | 100 | 100 | 100 | PASS: all equal |
| `21a5512` | 100 | 100 | 100 | PASS: all equal |

Both Repository logs show the same transient control-plane sequence:

```text
NRNCS is still unknown
GetHTBinding function returned warning status
Discovered a NRNCS! HID=... OSID=... PID=... BID=...
```

After discovery, both trials received the service offer and all 100 JPEG payloads. Both Source ContentApps logged exactly 100 JPEG publications. All bounded service processes ended by supervisor timeout (`124`), while fetch and cleanup operations returned `0`; this is expected for the bounded runner.

## Conclusion

The complete normal path passes at both commits. The previous SPEC-050 discovery-only failure is not a demonstrated code regression between `21a5512` and `1af604d`.

The Source/Repository role is not an architectural distinction for learning the NRNCS: both are the same ContentApp process with different role labels, and `CoreRunPeriodic01` contains the common NRNCS-discovery path. The successful Source workload must therefore not be treated as a prerequisite or causal explanation. The exact failure remains unresolved in the earlier local hello-IPC/SHM control-plane path or in the trial setup/observation, and requires evidence from the governing historical SPECs rather than a full speculative route trace.

Do not roll back on the current evidence. Keep SPEC-050/051 open for a targeted local hello-IPC/SHM acceptance check. A release claim still requires the other outstanding gates.

Evidence:

- `Specs/RESULTS-MATCHED-A/normal-100-1af604d/`
- `Specs/RESULTS-MATCHED-B/normal-100-21a5512/`
- `Specs/RESULTS-MATCHED-A/run_normal_100_trial.py`
- `Specs/RESULTS-MATCHED-B-runner.py`
