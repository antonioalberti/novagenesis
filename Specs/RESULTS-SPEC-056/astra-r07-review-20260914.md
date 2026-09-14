# Astra R07 Review — SPEC-056

**Date:** 2026-09-14
**Commit reviewed:** `f4ecc30` (parent `9ae03cd`)
**Scope:** read-only source/spec review of the R07 remediation; no runtime sign-off.
**Model:** `gpt-6-astra`
**Verdict:** `HOLD / NO-GO` for candidate-trial preparation.

## Findings

1. **HIGH — Launch is still not bound to the hashed executable.** Hashing/resolving a pathname before `Popen(argv)` does not prevent replacement between the check and execution; `checked_path` comparison is self-referential.
2. **HIGH — Build identity remains incomplete.** Nonempty placeholders can satisfy toolchain/options/runtime-library fields; compiler selection/version and per-executable library coverage are not required; dirty status can coexist with an empty captured snapshot; submodule identities are not compared to current state.
3. **HIGH — Staged-source drift remains unchecked.** Current index blobs are not compared against captured staged hashes, so index-only changes can escape the recheck.
4. **HIGH — Secret-safe capture remains incomplete.** URL userinfo, source assignments such as `API_TOKEN=...`, and expanded launch secrets can remain published; redaction does not consistently create a protected-input blocker.
5. **RESOLVED — Basename-collapse defect is closed at source-review level; runtime confirmation remains absent.
6. **HIGH — Process registration is after fallible identity calls.** A failure in `getpgid()` or `process_starttime()` after `Popen` can leave a process outside cleanup tracking.
7. **MEDIUM — Tests do not establish lifecycle RED→GREEN or complete remote compatibility.** Missing coverage includes replacement between final hash and Popen, index-only identity changes, sealed-bundle secret absence, unchanged-source positive control, and full remote lifecycle compatibility.

## Required before candidate trial

- Bind launch to an immutable checked executable representation or eliminate the check-to-launch pathname race.
- Require complete, semantically validated toolchain/options/runtime-library and source/index identity.
- Recheck staged index blobs and source snapshot identity.
- Detect/redact secret-bearing URLs, source assignments and expanded argv; add a protected-input blocker.
- Register every spawned process before fallible identity operations and clean up safely on identity failure.
- Add lifecycle-level positive/negative tests and remote compatibility coverage.

SPEC-056 remains unaccepted; no candidate trial or release approval is authorized by this review.
