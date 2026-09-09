# Baseline restoration and controlled SPEC-033 restart

Date: 2026-09-09. User-approved rollback; no new refactor implementation approved.

## Source and recovery identity

- Original baseline: `8e92e295033f894793fee6a5c2c12c7014f05e17`.
- Withdrawn attempt head: `c6ffcf7ce71f09d8124f346f94ede915093f4769`.
- Restoration commit: `77efe27fafbd7483507ed1448cbeedf04782bff4` on AIOPT3.
- Exact restored tree: `8087c27ad2fd22723a7abc603453545041c6fc94`, identical to baseline.
- Preservation tag: `recovery/pre-spec032-rollback-20260909T061553`.
- Rollback and tag pushed and verified by reading remote refs. No reset/force-push.
- Later documentation commit(s) change only Docs/ and Specs/, not production source.

Recovery artifacts (private local operational evidence, not committed wholesale):
`/home/gandalf/workspace/novagenesis-recovery-20260909T061553/`.
Includes complete Git bundle, SHA-256-verified workspace tar, two preserved
untracked files, old builds, manifests, deployment reports, scripts and test logs.
The bundle passed `git bundle verify`; archived regular files were compared with
original bytes before restoration. The existing GW.cpp trailing whitespace was
preserved rather than silently changing the exact baseline.

Historical specs/reviews/results are archived verbatim in
`../HISTORICAL/SPEC032-033-withdrawn-attempt/`. They do not describe active code.

## Guest recovery and build provenance

Both guest clones were detached at 8e92e29, with modified tracked binaries and
untracked runtime files. Stash failed. Subsequent fetch exposed an empty Git
object for c6ffcf7 on both guests. This establishes repository corruption, not
its cause; do not attribute it to the SPEC-033 source changes.

Each entire old clone is preserved at:
`/root/ng-recovery-20260909/previous-repository`.
Pre-cleanup IO/Results/binaries are additionally archived and copied to VM100.
A fresh shallow AIOPT3 clone replaced each active guest path. `git fsck --full`
passed; HEAD/tree and clean status were verified after the build.

New guest binaries: `/root/ng-recovery-20260909/build/`.
PGCS, NRNCS and ContentApp were rebuilt successfully, static-pie linked on Alpine,
with identical SHA-256 for each executable across both guests:

| Binary | SHA-256 |
|---|---|
| PGCS | 4cb3f7d7b184b47a581a3e229db23fa27750b8de2a44b7d6d33483ea45a34d94 |
| NRNCS | 36e6c0e4d71d3d6a6245b9f5a82765ec9dc68cd28483aeacce299b3954c9af8b |
| ContentApp | 0b269b937f56c8c6cf485878c19e1a399799c4038869a6f98367d2a8838f2728 |

VM100 full build: success, zero compiler errors, 137 warnings in restored source.
Tested build: `/home/gandalf/workspace/novagenesis/build-rollback-8e92e29/`.
The local `build/` symlink now resolves to this tested directory; the previous
SPEC-033 build was retired into the recovery folder. CMake build through the
symlink was exercised successfully. Do not use historical tracked executables
in cmake-build-debug as evidence of the selected source version.

## Runtime evidence

### Local intra-OS smoke — PASS

Fresh isolated IO directories, configs copied from the baseline, all processes
running as gandalf; PGCS -lc needed no raw-socket capabilities. Generated five
800x600 JPEGs into immutable staging. Delivered 5/5 with exact SHA-256 agreement,
no extra JPEGs; all four launched processes were alive at verification. Recorded
elapsed time: 120.02 seconds from first launch. Processes stopped afterwards.
Evidence: `local-smoke/result.json`, staging manifest and process logs.

### Multi-VM photos — PASS for external byte integrity

Fresh-boot guests, no previously running NG processes; launch order PGCS source,
PGCS repository, NRNCS source, repository app, source app. Correct peer MACs;
StressTest disabled; baseline NRNCS.ini and App.ini preserved and hashed.

1000 newly staged 800x600 JPEGs delivered source36 -> repo61. Final verifier:
`total / matched / missing / mismatched: 1000 / 1000 / 0 / 0`.
The controller also checked zero extra JPEGs. Final sample was 153.16 seconds
into post-launch monitoring (not total wall time including startup). All five
recorded NG PIDs remained alive at verification. Logs and received files were
copied to VM100 and the independent repository VerifyDelivery.py was run there.

Evidence: `photos1000-result.json`, `photos1000-manifest.sha256`, and
`vm-192.168.0.61/photos-1000/`, `vm-192.168.0.36/photos-1000/`.

Native hash caveat: the baseline's CoreRunEvaluate01 still checks the application
payload hash; no ERROR line was found in the captured repository log. However,
per-payload native acceptance success markers are compiled out, and the final
buffered log line is incomplete after stopping. Therefore this run certifies
external byte integrity, NOT an independently observed 1000/1000 native
MurmurHash3 acceptance count. Keep that evidence gap explicit.

### Bounded bidirectional load — completed, limited scope

After another guest reboot (boot IDs verified different from the photo run),
PGCS-only stress ran with StressInterval=0.0005, baseline executable hashes and
fresh IO. The observed post-readiness window was 305.48 seconds. Counter deltas
were divided by external monotonic observation time, not the internal elapsed_s
field. The 10-second heartbeat and burst scheduling make these rates approximate.

| Side | Offered delta | Received delta | Offered/s | Received/s | Max RSS KiB | Peak sampled queue | Final sampled queue |
|---|---:|---:|---:|---:|---:|---:|---:|
| repo61 | 590000 | 582736 | 1931.36 | 1907.58 | 29132 | 10002 | 2 |
| source36 | 610000 | 600000 | 1996.81 | 1964.08 | 7556 | 10002 | 265 |

Both processes remained alive; receive counters progressed. All sampled dropped
and guard_reject counters were zero. The final recorded SAR error counters were
also zero. These are not matched, drained end-to-end totals: heartbeat boundaries
are asynchronous and some work remained queued. Do NOT call this zero loss or a
30-minute soak. No production patch or debugger mutation was used to stop offering.

Evidence: `load2000-result.json`, `load2000-summary.json`, guest launch.json and
complete pgcs.log copies. The new sampled peak 10002 must not be conflated with
the historical peak 2762: scheduling phase, sampling and workload conditions need
controlled comparison before drawing a regression conclusion.

## Planning decision and remaining gates

SPEC-032 is deferred. SPEC-033 is a replacement Draft, with baseline qualification
and production-linked characterisation/ownership audit before any minimal safety
fix. Parser consolidation, allocator/handles, queue migration and RAII are separate
future approval boundaries, not one implementation batch. Wire-format changes and
batching are excluded. Historical fixes are not blindly cherry-picked.

Remaining before new implementation: repeated baseline gates, full native
acceptance visibility, a separate 500 msg/s trial, defined drain and long-soak
acceptance, production-linked reproducer/caller audit, and explicit user approval
of the first small amendment. Successful workload recovery does not erase known
baseline memory-safety risks. No new SPEC-033 production code was introduced.
