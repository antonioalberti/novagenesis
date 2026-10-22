# NG-075 — Local SAR expiry ASE pilot run record

**Date:** 2026-10-21 (Europe/London)  
**Outcome:** Focused local tests/builds and the per-sweep comparison pass, but the second read-only Astra review of the refreshed candidate returned `AMEND` for irregular cleanup cadence and clean-build reproducibility. The local slice is not owner-accepted. This is not acceptance of Phase 0/3, the complete SPEC-027, or NG-062.

## Frozen identities

- AIOPT5 baseline: `603d304b1413aa885fa405b2fe556cefc323674a`.
- Governing SPEC: `Specs/SPEC-027-pgcs-multithreading.md`, SHA-256 `acac719db2b6c986e77c94a52e0a8f22d8d9c2ec4d4f3731d81615f3320adaaa`.
- Baseline SAR files:
  - `Common/src/NGAL_SAR.h` — `4953500da2f81d37a032056e3abef8ae6d889e86ffd34fe344a07f399e8bf13d`
  - `Common/src/NGAL_SAR.cpp` — `906aaa9f68af274e10dae21ad70149f33a28ef8f987dd35a1d920c8f5ed95553`
  - `PGCS/src/NGAL_Transport_RAW.cpp` — `bf1e4f96d8bc0d612aad3978e9b6f37c9f909c6511eddf66114006c5a8df727a`
- Candidate working-tree files at Astra round-2 review (before the resumed correction):
  - `Common/src/NGAL_SAR.h` — `10eaf7f776b483bf83748aa44d5a83e2acf145ad54eed5eaa8de93dc514448d6`
  - `Common/src/NGAL_SAR.cpp` — `f20b14c7c253b37082a91789dc49989239fb130e69408c2e2db97284bc13098b`
  - `PGCS/src/NGAL_Transport_RAW.cpp` — `71d86a676c7002a4089ad7d0ec6524ed1f7d5a5a32046338d283d989561dbcfa`
  - `Tools/tests/NGALSARExpirationTest.cpp` — `3fd4a31353e0fecbe042528ce6b811ccbcd7537ce4efa28a5ddaa96bcf68b53b`

## Test-first correctness

The pinned baseline reproducer is `baseline-red-reproducer.cpp` (SHA-256 `150dffccd6d6cbaee8f18d7146e533552308654412d0236ef980fb994879a2bb`). It builds against the clean baseline `Common` library and exits `1` for the expected defect: a partial message with a wall-clock timestamp survives a monotonic cleanup threshold, and its continuation completes the assembly. The observed output was `RED: wall-clock timestamp survived monotonic cleanup threshold`.

The candidate harness prints `PASS: SAR idle/max expiry, refresh semantics, continuation guard, and scheduled cleanup` in both Debug and Release. It covers:

- idle expiry at 29.999 s, exactly 30 s, and 30.001 s;
- the 150 s maximum age at 149.999 s, exactly 150 s, and 150.001 s, despite accepted progress;
- accepted progress refreshing idle age, while duplicate and rejected fragments do not;
- rejecting continuation of an expired assembly before it can complete;
- a simulated continuously active dispatcher checking cleanup every 500 ms, requiring at least 30 sweeps over 30 s and one abandoned assembly;
- a deterministic 600 ms call cadence that checks no scheduled-sweep interval exceeds one second, including the initial deadline.

The exact Debug/Release build, harness, and focused PGCS translation-unit commands and results from the authorized post-AMEND iteration are recorded below. `ctest -N` reports **0 registered tests** in the isolated build, so the standalone harness is the focused test.

For the initial pre-AMEND candidate, read-only analyzer comparison against the pinned baseline found no new diagnostics: cppcheck reported the same 17 diagnostic lines on both snapshots; clang-tidy reported the same 10 diagnostic identities. Those analyzers were not rerun after the 500 ms cadence correction. A prior cppcheck message about the existing `unique_ptr`-to-raw-pointer ownership transfer was present in both initial snapshots.

## Initial pre-AMEND cleanup-sweep comparison

The durable harness is `cleanup-benchmark.cpp` (SHA-256 `3ddfcf02195a25ae1e7ae712c778d4a8ff84d2e8b1a1f9c4a00ac077f3ec8f0e`). It creates 256 incomplete assemblies, then times 1,000,000 cleanup sweeps per fresh process. The baseline arm reads `steady_clock` for each sweep and passes the resulting threshold; the candidate calls its default steady-clock-backed `CleanupTimedOut()`. Both arms include a clock read per sweep and retain all buffers during the sub-second timing window. The candidate and baseline use the same compiler, Release `Common` profile, harness source, flags, buffer count, sweep count, and measurement boundary.

Environment: Ubuntu 24.04 host, x86_64, AMD Ryzen 7 8845HS, GCC 13.3.0, CMake 3.28.3. `Common` compile commands use `-O0 -g3 ... -O3 -DNDEBUG -std=gnu++20` (final optimization flag is `-O3`); harness commands use `g++ -std=c++20 -O3 -DNDEBUG -Wall -pthread ... -lpthread -lrt`.

20 independent baseline/candidate process pairs were run in alternating order, after one warm-up process per arm:

```text
baseline ns/sweep: 285.667, 286.274, 285.319, 286.883, 287.986, 287.045, 288.188, 285.496, 287.007, 286.603, 287.904, 287.557, 285.857, 286.540, 285.861, 287.656, 284.828, 288.129, 286.406, 308.221
candidate ns/sweep: 241.477, 240.959, 244.493, 241.761, 240.941, 242.924, 241.621, 240.680, 244.467, 241.622, 241.314, 241.644, 244.226, 241.948, 240.828, 243.148, 240.842, 241.614, 241.090, 256.786
```

- Baseline median: **286.743 ns/sweep**.
- Candidate median: **241.6215 ns/sweep**.
- Median paired candidate/baseline ratio: **0.84287022**; paired-bootstrap 95% interval **0.84102093–0.84529146** (10,000 resamples, fixed seed `20261021`).
- Candidate sweep was faster in **20/20** pairs; median paired difference **−45.007 ns/sweep**.

This measures only local cleanup-sweep cost. It excludes per-frame dispatcher scheduling overhead, network behavior, and end-to-end throughput; do not infer system-level performance from it. The baseline arm intentionally retains the current wall/monotonic mismatch; the comparison is not evidence that the baseline expiry contract is correct.

The static `Common` archive hashes used were baseline `8137b9fcffcf1e4bdbbfc4b4d039bf09dd5286663e55224649b39e12ef4840c7` and candidate `b1f4e987f99fb6d971ee482518585886a5dca43810b194e28b6c239804b71f06`.

## Post-AMEND authorized closure iteration (2026-10-21)

The owner authorized one local iteration limited to SAR cleanup cadence, its deterministic regression, and this evidence record. No SPEC, wire format, CMake, DEBUG definition, dispatcher call-site, VM/IPC/runtime, full-build, commit, or push change was authorized or made.

### Cadence regression: RED then GREEN

`TestScheduledCleanupWithUnalignedCalls` advances the injected monotonic clock in 600 ms steps and checks the first sweep and every following sweep against a one-second maximum interval. Before the implementation change, the new Debug-linked test produced the expected RED:

```text
FAIL: scheduled cleanup interval exceeded one second at 600 ms call cadence
RED_EXIT=1
```

The minimal correction uses `SAR_CLEANUP_INTERVAL = 500 ms` for both the initial deadline and the re-armed deadline. The header comment now describes the sub-second target. The existing 500 ms continuous-traffic test accepts an earlier schedule while still requiring at least 30 sweeps in 30 seconds and exactly one timed-out assembly.

After the correction, the focused harness passed in both Debug and Release:

```text
PASS: SAR idle/max expiry, refresh semantics, continuation guard, and scheduled cleanup
```

### Exact focused build and test commands

The configured build caches identify the candidate worktree and `CMAKE_BUILD_TYPE=Debug` / `Release`. These exact build commands passed:

```sh
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-603d304 --target Common -j2
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-candidate-release-603d304 --target Common -j2
```

The exact standalone harness commands were:

```sh
ROOT=/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304
DBG=/home/gandalf/workspace/novagenesis-ng075-build-603d304
REL=/home/gandalf/workspace/novagenesis-ng075-build-candidate-release-603d304

g++ -std=c++20 -O0 -g3 -Wall -Wextra -pthread -I"$ROOT/Common/src" "$ROOT/Tools/tests/NGALSARExpirationTest.cpp" "$DBG/libCommon.a" -lpthread -lrt -o "$DBG/NGALSARExpirationTest"
"$DBG/NGALSARExpirationTest"

g++ -std=c++20 -O3 -DNDEBUG -Wall -Wextra -pthread -I"$ROOT/Common/src" "$ROOT/Tools/tests/NGALSARExpirationTest.cpp" "$REL/libCommon.a" -lpthread -lrt -o "$REL/NGALSARExpirationTest"
"$REL/NGALSARExpirationTest"
```

The two PGCS translation units were compiled with their exact candidate-Debug `compile_commands.json` entries; only the output object names were redirected to dedicated focused artifacts. The command used was:

```python
import json, pathlib, shlex, subprocess
build = pathlib.Path("/home/gandalf/workspace/novagenesis-ng075-build-603d304")
rows = json.load(open(build / "compile_commands.json"))
targets = {
    "PGCS/src/NGAL_Transport_RAW.cpp": "NGAL_Transport_RAW.focussed.o",
    "PGCS/src/PG.cpp": "PG.focussed.o",
}
for suffix, output in targets.items():
    row = next(r for r in rows if r["file"].endswith("/" + suffix))
    args = shlex.split(row["command"])
    args[args.index("-o") + 1] = str(build / output)
    subprocess.run(args, cwd=row["directory"], check=True)
```

Both focused translation-unit compilations exited 0. The compile database supplies the complete include paths, warning flags, and `-O0 -g3 -std=gnu++20` options.

### Re-run of the bounded cleanup-sweep comparison

This run binds the measurement to the corrected candidate archive. The baseline remains the clean pinned worktree at `603d304b1413aa885fa405b2fe556cefc323674a`. Both build caches are Release. The static archives were baseline `8137b9fcffcf1e4bdbbfc4b4d039bf09dd5286663e55224649b39e12ef4840c7` and corrected candidate `46fddaf4aa6d6edd67c854bb571b2f2323f5ee12dcc519e74c2ecfb58a454dd4`. The unchanged harness source SHA-256 is `3ddfcf02195a25ae1e7ae712c778d4a8ff84d2e8b1a1f9c4a00ac077f3ec8f0e`.

Environment was Ubuntu 24.04 x86_64, AMD Ryzen 7 8845HS, GCC `13.3.0-6ubuntu2~24.04.1`, CMake `3.28.3`. The exact compiler commands were:

```sh
g++ -std=c++20 -O3 -DNDEBUG -Wall -pthread -I/home/gandalf/workspace/novagenesis-ng075-sar-baseline-603d304/Common/src /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Specs/RESULTS-SPEC-027/ase-sar-expiry-pilot/cleanup-benchmark.cpp /home/gandalf/workspace/novagenesis-ng075-build-baseline-release-603d304/libCommon.a -lpthread -lrt -o /home/gandalf/workspace/novagenesis-ng075-build-baseline-release-603d304/NGALSARCleanupBenchmark
g++ -std=c++20 -O3 -DNDEBUG -Wall -pthread -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src -DNGAL_SAR_BENCH_CURRENT /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Specs/RESULTS-SPEC-027/ase-sar-expiry-pilot/cleanup-benchmark.cpp /home/gandalf/workspace/novagenesis-ng075-build-candidate-release-603d304/libCommon.a -lpthread -lrt -o /home/gandalf/workspace/novagenesis-ng075-build-candidate-release-603d304/NGALSARCleanupBenchmark
```

One warm-up process per arm was discarded. Then 20 paired processes were run, alternating baseline/candidate order. Each process constructed 256 incomplete assemblies and measured 1,000,000 sweeps. The following Python driver was used for the campaign and paired bootstrap:

```python
from pathlib import Path
import json, random, statistics, subprocess

base_build = Path("/home/gandalf/workspace/novagenesis-ng075-build-baseline-release-603d304")
cand_build = Path("/home/gandalf/workspace/novagenesis-ng075-build-candidate-release-603d304")
bins = {"baseline": base_build / "NGALSARCleanupBenchmark",
        "candidate": cand_build / "NGALSARCleanupBenchmark"}

def run(binary):
    p = subprocess.run([str(binary)], check=True, text=True, capture_output=True)
    return float(p.stdout.strip())

for binary in bins.values():
    run(binary)  # one warm-up per arm
samples = {"baseline": [], "candidate": []}
for pair in range(20):
    order = ("baseline", "candidate") if pair % 2 == 0 else ("candidate", "baseline")
    for arm in order:
        samples[arm].append(run(bins[arm]))
ratios = [c / b for b, c in zip(samples["baseline"], samples["candidate"])]
rng = random.Random(20261021)
bootstrap = sorted(statistics.median(ratios[rng.randrange(20)] for _ in range(20))
                   for _ in range(10000))
print(json.dumps({
    "pairs": 20, "sweeps_per_process": 1000000, "incomplete_assemblies": 256,
    "baseline_ns_per_sweep": samples["baseline"],
    "candidate_ns_per_sweep": samples["candidate"],
    "baseline_median_ns_per_sweep": statistics.median(samples["baseline"]),
    "candidate_median_ns_per_sweep": statistics.median(samples["candidate"]),
    "paired_median_ratio_candidate_over_baseline": statistics.median(ratios),
    "bootstrap_seed": 20261021, "bootstrap_resamples": 10000,
    "bootstrap_percentile_indices": [249, 9749],
    "bootstrap_95pct_median_ratio": [bootstrap[249], bootstrap[9749]],
    "candidate_faster_pairs": sum(c < b for b, c in zip(samples["baseline"], samples["candidate"]))
}, separators=(",", ":")))
```

Observed raw per-process samples and summary:

```text
baseline ns/sweep: 286.380, 286.545, 286.177, 286.477, 284.978, 287.181, 286.183, 288.605, 286.632, 286.065, 286.877, 285.805, 286.259, 285.002, 285.022, 286.885, 286.376, 286.158, 287.953, 286.632
candidate ns/sweep: 241.075, 240.878, 242.076, 241.403, 240.793, 240.440, 241.303, 240.560, 240.641, 244.685, 242.666, 240.663, 241.298, 241.104, 241.005, 241.441, 241.783, 241.003, 244.448, 240.421
baseline median: 286.378 ns/sweep
candidate median: 241.0895 ns/sweep
paired median candidate/baseline ratio: 0.84279848
paired bootstrap 95% interval: 0.84159506–0.84525963
candidate faster pairs: 20/20
```

The 500 ms scheduler deadline itself is not timed by this harness: it directly measures `CleanupTimedOut()` with 256 incomplete buffers. The comparison therefore remains a per-sweep microbenchmark, not dispatcher scheduling overhead, network behavior, or system throughput.

## Scope and review

The candidate changes are limited to `Common/src/NGAL_SAR.h`, `Common/src/NGAL_SAR.cpp`, `PGCS/src/NGAL_Transport_RAW.cpp`, the focused test, and pilot evidence. CMake, DEBUG macros, wire format, shared-worker assumptions, and unrelated tasks were not changed. Refreshed diff SHA-256 is `8dab625024b6f34c9de0c188476775d415d369f9abb5008797e902a5e23f70d4`; `git apply --check` passed against the clean pinned baseline. The primary-agent read-only review found no additional blocker within its bounded scope, but the second Astra review of packet `98d8b7bb2de577c16ea718fcacd844c27e6bcde7899bfc338e670910084fee66` returned `AMEND`; no code or SPEC changed after that call. No further Astra/Codex/JEV call, VM/IPC/runtime, full application build, commit, or push was performed.

## Second read-only Astra review (2026-10-21)

The current candidate was reviewed once, read-only, under packet SHA-256 `98d8b7bb2de577c16ea718fcacd844c27e6bcde7899bfc338e670910084fee66`. At submission, the reviewed run-record SHA was `399977722824e1d26b11fb79fbc60c7d44dd05f6243af976591315dc875fb9e8`; diff SHA was `8dab625024b6f34c9de0c188476775d415d369f9abb5008797e902a5e23f70d4`; SPEC SHA remained `acac719db2b6c986e77c94a52e0a8f22d8d9c2ec4d4f3731d81615f3320adaaa`.

**Verdict: `AMEND` — REQ-027-015 local single-dispatcher slice only.** The reviewer confirmed no demonstrated wire-format, ownership, or lifecycle regression in the supplied changes and agreed the microbenchmark is limited to direct retained-buffer sweep cost. The previous `AMEND` was not carried forward automatically; this is a new verdict on the refreshed packet.

### Mandatory findings

1. **Cadence remains noncompliant for irregular sub-second calls.** A 500 ms target fixes the regular 600 ms test but not all call phasing. Astra supplied the counterexample: call at 0.600 s sweeps and sets deadline 1.100 s; call at 1.090 s does not sweep; call at 1.690 s sweeps. Call gaps are at most 600 ms, yet the sweep gap is 1.090 s. The current test covers only a regular 600 ms sequence (`Tools/tests/NGALSARExpirationTest.cpp:236–265`). Fix the scheduling contract/implementation and add an irregular sequence that crosses deadlines. This does not imply hard real-time guarantees during arbitrary thread suspension.

2. **Clean-build reproducibility remains incomplete.** The receipt records measurement commands and statistics, but starts from preconfigured build directories. It lacks the configure/bootstrap commands and relevant options, the exact baseline Release `Common` build procedure, and embedded PGCS `compile_commands.json` entries. Document the missing build provenance or narrow the reproducibility claim; a full application build or reviewer reexecution is not required by this finding.

### Optional hardening, not blockers in this verdict

The reviewer suggested stronger distinction of accepted versus rejected assembly states, same-MN reuse/multiple-removal cases, explicit benchmark population validation, and wording that distinguishes “no SPEC change during the correction iteration” from the §12 addition relative to baseline. Do not widen the current correction scope without owner approval.

### Archived call evidence

- Derived response: `astra-review-post-amend-response.md`, SHA-256 `f6ed1349e21925d04ad284e5cf810cefc660a3fa13886978cdd97c998bb62589`.
- Call record: `astra-review-post-amend-call-record.json`, SHA-256 `d42e8bcddac4ef5dfe0c86acac3f3e6ab31a5702f04da90a80d04a1b900ea8ad`.
- Normalization record: `astra-review-post-amend-response-normalization.md`, SHA-256 `1fff49d9a8421fe0eaba38ca7a9a09c07c9da6bdd32d5bf611e468bb843dc4bd`.
- Raw SSE: `astra-review-post-amend.raw.sse`, SHA-256 `87ae3803e6731461e3400b969013411c9d80758f855bdc1cc2af80a227e8b288`; archived session transcript SHA-256 `9abe7e1fa3678a59d73f0b728f3ff5954599be957cb8a20a90e2dbd3e639dc76`.
- Usage: 19,481 input / 1,894 output / 70 reasoning tokens; response latency 64.399 s; response headers showed 14% primary / 16% secondary and zero paid-credit balance. No monetary charge amount was reported.
- The session parser concatenated duplicate terminal text (17,816 chars); 1,818 delta events yielded the single 8,908-character derived response and matched each terminal representation. Raw SSE and transcript remain byte-preserved.

## Short ASE retrospective

- The gate sequence caught the clock-domain defect with a baseline RED before the production edit; injecting a monotonic function pointer made the boundary cases deterministic without changing the wire format.
- The first candidate test draft depended on a not-yet-existing clock API and did not compile on baseline; it was replaced by a real baseline reproducer before implementation, then expanded into the final deterministic suite.
- The final benchmark uses the same durable harness and per-sweep steady-clock read in both arms; it measures only the bounded cleanup path. No CMake change was needed, so the focused harness remains standalone.
- Two Astra rounds both returned `AMEND`, each scoped to its own immutable packet; the second identifies the current blockers above. On 2026-10-21 the owner authorized one second bounded local iteration in the same REQ-027-015. The Kanban review was reopened to `ready`; no code/SPEC changes for this second iteration have been made yet. A review verdict alone does not authorize further changes.

**Next gate after Astra round 2 (historical):** add the irregular-call RED case, correct phase drift without widening the single-dispatcher scope, complete clean focused-build provenance, and rerun only the authorized focused tests/builds/measurement. The owner resumed this work on 2026-10-22; details follow.

## Resumed bounded local correction — 2026-10-22

**Status:** the owner-authorised second local correction addressed the two Astra round-2 findings in bounded local evidence: the tested irregular cadence sequences are GREEN, clean focused build provenance is recorded, and the paired per-sweep comparison was rerun. P1 remains conditional on cooperative calls no more than 600 ms apart; the dispatcher's maximum per-slot interval was not measured. No Astra/Codex/JEV call followed. This does **not** constitute owner acceptance, full SPEC-027 acceptance, or the first complete ASE cycle under HERMES-006.

### Test-first cadence correction

- Added `TestScheduledCleanupWithIrregularCalls()` to the focused standalone test. It contains the Astra counterexample call intervals `600, 490, 600 ms` and a deadline-straddling case `400, 390, 600 ms`; both assert that no observed sweep gap exceeds one second. The additional worst-phase case is only executed after the first case passes.
- Before changing the scheduler, rebuilt the candidate Debug `Common` library from the current pre-fix source, compiled the harness, and ran it. It returned exit 1 with the expected `FAIL: irregular 600/490/600 ms calls allowed a sweep gap over one second`.
- Changed the target interval from 500 ms to 400 ms and re-armed from the prior scheduled deadline, not from `now`. When one or more scheduled deadlines have elapsed, arithmetic skips them in O(1) rather than running catch-up sweeps. This keeps the cadence phase stable and avoids an unbounded catch-up loop after a long delay.
- The proof is conditional: with `MaybeCleanup()` invocations no more than 600 ms apart, a phase-anchored 400 ms target keeps observed sweep gaps at or below one second. The dispatcher calls it at the loop top and before each received slot (`PGCS/src/NGAL_Transport_RAW.cpp:259,340`); `poll()` has a 100 ms timeout (`:276`). No live runtime measurement established a strict maximum interval for the per-slot processing path. This is a cooperative-progress assumption, not a hard-real-time guarantee during a slow handler or arbitrary dispatcher stall; keep that limitation open for owner review against §12.2.
- After the fix, the standalone contract harness printed `PASS: SAR idle/max expiry, refresh semantics, continuation guard, and scheduled cleanup` in both Debug and Release.

### Clean focused build provenance

Pinned source roots:

- Candidate: `/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304`, HEAD `603d304b1413aa885fa405b2fe556cefc323674a`.
- Baseline: `/home/gandalf/workspace/novagenesis-ng075-sar-baseline-603d304`, same HEAD and clean `git status` before build.
- Governing SPEC: `Specs/SPEC-027-pgcs-multithreading.md`, SHA-256 `acac719db2b6c986e77c94a52e0a8f22d8d9c2ec4d4f3731d81615f3320adaaa`.

Environment: Ubuntu 24.04 x86_64, AMD Ryzen 7 8845HS, CMake 3.28.3, `/usr/bin/c++` GCC `13.3.0-6ubuntu2~24.04.1`; generator `Unix Makefiles`; C and C++ compilers `/usr/bin/cc` and `/usr/bin/c++`; `CMAKE_EXPORT_COMPILE_COMMANDS=ON`; base C/C++ flags empty. Observed cache values in all three configurations: `NG_ENABLE_LEGACY_STANDALONE=OFF`, `NG_ENABLE_SPEC027_STAGE_DIAGNOSTICS=OFF`, `NG_SPEC027_PERF_PROFILE=OFF`.

The three clean out-of-tree configuration commands (each exited 0) were:

```sh
cmake -S /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304 -B /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-debug-603d304 -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/cc -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DNG_ENABLE_LEGACY_STANDALONE=OFF -DNG_ENABLE_SPEC027_STAGE_DIAGNOSTICS=OFF -DNG_SPEC027_PERF_PROFILE=OFF
cmake -S /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304 -B /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-release-603d304 -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/cc -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DNG_ENABLE_LEGACY_STANDALONE=OFF -DNG_ENABLE_SPEC027_STAGE_DIAGNOSTICS=OFF -DNG_SPEC027_PERF_PROFILE=OFF
cmake -S /home/gandalf/workspace/novagenesis-ng075-sar-baseline-603d304 -B /home/gandalf/workspace/novagenesis-ng075-build-r2-baseline-release-603d304 -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/cc -DCMAKE_CXX_COMPILER=/usr/bin/c++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DNG_ENABLE_LEGACY_STANDALONE=OFF -DNG_ENABLE_SPEC027_STAGE_DIAGNOSTICS=OFF -DNG_SPEC027_PERF_PROFILE=OFF
```

Only the `Common` library target was built; no PGCS/application/full-product link was attempted:

```sh
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-debug-603d304 --target Common -j2
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-release-603d304 --target Common -j2
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-r2-baseline-release-603d304 --target Common -j2
```

All three commands exited 0. The Release builds emitted warnings in unrelated existing Common translation units in both baseline and candidate; no `NGAL_SAR.cpp` warning was reported. The focused test compile commands were:

```sh
g++ -std=c++20 -O0 -g3 -Wall -Wextra -pthread -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Tools/tests/NGALSARExpirationTest.cpp /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-debug-603d304/libCommon.a -lpthread -lrt -o /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-debug-603d304/NGALSARExpirationTest
g++ -std=c++20 -O3 -DNDEBUG -Wall -Wextra -pthread -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Tools/tests/NGALSARExpirationTest.cpp /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-release-603d304/libCommon.a -lpthread -lrt -o /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-release-603d304/NGALSARExpirationTest
```

Both harness compiles and both executions exited 0. Exact output in each profile:

```text
PASS: SAR idle/max expiry, refresh semantics, continuation guard, and scheduled cleanup
```

The two PGCS translation units were compiled individually from the candidate Debug `compile_commands.json`; no PGCS target was linked. The JSON `directory` was `/home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-debug-603d304` for both entries. The original `command` fields were:

```text
/usr/bin/c++  -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/ContentApp/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src/rapidjson -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src/rapidjson/error -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src/rapidjson/internal -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/IoTTestApp/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/NBTestApp/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/PGCS/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/NRNCS/src  -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer -g -std=gnu++20 -o CMakeFiles/PGCS.dir/PGCS/src/NGAL_Transport_RAW.cpp.o -c /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/PGCS/src/NGAL_Transport_RAW.cpp
/usr/bin/c++  -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/ContentApp/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src/rapidjson -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src/rapidjson/error -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src/rapidjson/internal -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/IoTTestApp/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/NBTestApp/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/PGCS/src -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/NRNCS/src  -O0 -g3 -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt -fno-omit-frame-pointer -g -std=gnu++20 -o CMakeFiles/PGCS.dir/PGCS/src/PG.cpp.o -c /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/PGCS/src/PG.cpp
```

Each command exited 0. For the focused executions, only the `-o` path was changed to `/home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-debug-603d304/NGAL_Transport_RAW.ng075-focussed.o` and `.../PG.ng075-focussed.o`, respectively. No CMake source file or link target changed.

### Paired local sweep comparison, resumed candidate

The unchanged harness source SHA-256 is `3ddfcf02195a25ae1e7ae712c778d4a8ff84d2e8b1a1f9c4a00ac077f3ec8f0e`. It creates 256 incomplete assemblies and times 1,000,000 direct cleanup sweeps per process. One warm-up per arm was discarded; then 20 paired baseline/candidate processes were run in alternating order. The exact Release harness commands were:

```sh
g++ -std=c++20 -O3 -DNDEBUG -Wall -pthread -I/home/gandalf/workspace/novagenesis-ng075-sar-baseline-603d304/Common/src /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Specs/RESULTS-SPEC-027/ase-sar-expiry-pilot/cleanup-benchmark.cpp /home/gandalf/workspace/novagenesis-ng075-build-r2-baseline-release-603d304/libCommon.a -lpthread -lrt -o /home/gandalf/workspace/novagenesis-ng075-build-r2-baseline-release-603d304/NGALSARCleanupBenchmark
g++ -std=c++20 -O3 -DNDEBUG -Wall -pthread -I/home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Common/src -DNGAL_SAR_BENCH_CURRENT /home/gandalf/workspace/novagenesis-ng075-sar-expiry-603d304/Specs/RESULTS-SPEC-027/ase-sar-expiry-pilot/cleanup-benchmark.cpp /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-release-603d304/libCommon.a -lpthread -lrt -o /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-release-603d304/NGALSARCleanupBenchmark
```

Paired bootstrap used seed `20261021`, 10,000 resamples of 20 paired ratios, percentile indices `[249, 9749]`:

```text
baseline ns/sweep: 289.175, 285.782, 287.269, 285.847, 290.654, 286.813, 289.295, 285.635, 286.154, 288.159, 286.326, 287.347, 285.391, 288.600, 285.962, 285.993, 285.838, 286.467, 288.249, 285.864
candidate ns/sweep: 240.985, 240.331, 243.244, 244.239, 240.578, 241.057, 242.064, 246.926, 241.574, 240.981, 240.938, 240.892, 240.839, 240.726, 240.807, 240.483, 241.269, 240.675, 242.014, 241.730
baseline median: 286.3965 ns/sweep
candidate median: 240.983 ns/sweep
paired median candidate/baseline ratio: 0.8409147119
paired bootstrap 95% interval: 0.8389659199–0.8439836891
candidate faster pairs: 20/20
```

Archive/harness identities: baseline Release `libCommon.a` SHA-256 `f4de1738e22762af157129eb7a3fb6d9db9a87ff906bba0e9361bd43ef41b225`; candidate Release `libCommon.a` `3b3ec8165a1fc89f37e32e571c47aebaa22a29e140e75a98e2e1a69b3f390263`; harness binaries baseline `9e191548bdc9f2f6e6740c1e92c07d3b391a329fa81ab70b74023929834adc19`, candidate `65393ba1fd448e10bfa022cc0f9df90f91b05b9e3d585b03cdf9ccc2d074a5a6`.

This measures direct per-sweep work only. The scheduler target changed from 500 ms to 400 ms; the target sweep frequency therefore rises from 2.0 to 2.5 sweeps/s. The benchmark does not measure scheduled frequency or total dispatcher CPU, so it cannot establish a net system-level performance gain.

### Snapshot and process status

Current candidate file hashes: `Common/src/NGAL_SAR.cpp` `7f42d5dbf40a0e49ebca99df185ada072f2cf068874a1cce87e4f69a35634b0e`; `Common/src/NGAL_SAR.h` `d82c6fb4e363ac4bb96e8133bd7019a39fe0197d647c205b007e7498db359ee0`; `PGCS/src/NGAL_Transport_RAW.cpp` `71d86a676c7002a4089ad7d0ec6524ed1f7d5a5a32046338d283d989561dbcfa`; SPEC-027 `acac719db2b6c986e77c94a52e0a8f22d8d9c2ec4d4f3731d81615f3320adaaa`; focused test `5ee3f878795d522f0d5881b2b1bb116ecf50a1c9c3d3218bcaec0779193bbf73`. Candidate Debug `CMakeCache.txt` `16e642d6510cd471cfeecfccedf4c22b35367c472dde3aee2aad2a15cd3f5e53`, `compile_commands.json` `083017b40b8d9a5afe5713b4e9022783092fced0e2e0de92ce2df8ee95538993`; candidate Release cache `ad7517660e8a8c1137f9291ea12b000efe108733a0adb2d77060ea7eab3931e8`, compile DB `de6498ab105a36daf9164849c4c17c376bc823266d7a6ccd374e0afc5a0f4071`; baseline Release cache `ef749e53f7e7feadc1bfd20f32b7d6525c345d1c376a1625ada88ed8cd6458c3`, compile DB `88cff8461d35bf3997cea10a40bad239a91d53c039caa51a9b91f723f571d114`.

The refreshed candidate diff is `candidate-diff.patch`, SHA-256 `32368d7a91bd86a22d8aa5ae555eb0f502d8b1fb5eb82e3335268a2f7e03c1b7`; `git diff --check` and `git apply --check` against the clean pinned baseline both exited 0. No commit or push was made. No SPEC change occurred in this correction iteration; §12 was already present relative to the original HEAD.

**ASE status:** this manual single-agent SAR iteration is preserved as a technical spike and evidence for the eventual case study. HERMES-006 implementation Slices 2–4 (reusable ASE Skills/templates/profiles, safe dispatch controls, and validated stage handoffs) are still incomplete. Therefore this work must not be counted as the planned first complete ASE cycle or used to mark HERMES-006 criterion 70 complete. The next full cycle should start from a clean pinned baseline only after the approved ASE implementation and read-back gates are complete. The spike and its unaccepted candidate remain preserved separately; full SPEC-027 and NG-062 remain open/suspended.

**Current next gate:** implement the approved HERMES-006 ASE assets and safety controls, then launch a fresh, pinned end-to-end cycle using the implemented process. Keep the dispatch card blocked until its auto-decomposition/wake-up/concurrency settings are reviewed; no new Astra review is authorized by this record.


### Verificação 2026-10-22 — build e cadência (registro corretivo)

**Comandos verificados nesta sessão:**
```sh
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-debug-603d304 --target Common -j2
# exit 0, saída: [100%] Built target Common — alvo considerado já construído, não prova recompilação limpa
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-r2-candidate-release-603d304 --target Common -j2
# exit 0, saída: [100%] Built target Common
cmake --build /home/gandalf/workspace/novagenesis-ng075-build-r2-baseline-release-603d304 --target Common -j2
# exit 0, saída: [100%] Built target Common
```

**Teste de cadência atual:** compilação e execução do harness NGALSARExpirationTest em Debug e Release retornaram `PASS` com `EXIT:0` (hashes: NGAL_SAR.cpp 7f42d5db..., NGAL_SAR.h d82c6fb4..., test 5ee3f878...). Correção anterior que afirmava `RED` com `FAIL: irregular 600/490/600 ms calls allowed a sweep gap` não se reproduziu nesta verificação — o teste atual passou nos cenários cobertos (`600/490/600` e `400/390/600`).

**Limitação preservada:** `AMEND` da Astra (2026-10-21, packet 98d8b7bb...) permanece histórico. O `PASS` cobre apenas cadência em relógio injetado (até 30 s, intervalos 600/490/600), não mede o dispatcher real. A hipótese `≤600 ms` entre `MaybeCleanup` continua pendente; `t_c073c4d4` permanece bloqueado; ciclo ASE não aceito.

