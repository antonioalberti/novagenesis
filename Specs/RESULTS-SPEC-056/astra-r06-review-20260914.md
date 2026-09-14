# Astra R06 Review — SPEC-056

**Date:** 2026-09-14
**Commit reviewed:** `c1f5ad8` (parent `bdfacca`)
**Scope:** read-only source review of the exact SPEC-056 R06 diff; no runtime sign-off.
**Model:** `gpt-6-astra`
**Usage:** 14,582 input / 1,212 output tokens; 5h quota 39%; weekly quota 46%.
**Raw SSE:** `/tmp/astra-spec056-r06.last.sse` (session-local copy)

## Verdict

**HOLD / NO-GO** for the next acceptance-oriented candidate-trial step. SPEC-056 remains unaccepted.

## Findings

1. **HIGH — Build linkage can validate incomplete source/build identity.** `_snapshot_records()` and `validate_build_linkage()` accept a source object containing only `head`; optional tree/status fields, toolchain, options and runtime-library identities are not required.
2. **HIGH — Executable mapping can silently omit a launched binary.** `capture_local_provenance()` collapses roles by executable basename; differing hashes can leave one executable unvalidated.
3. **HIGH — Dirty-source snapshots are not proven reconstructable or stable.** Staged/unstaged versions are not separately preserved; deleted files and submodules are incomplete; copied bytes are not rehashed and source races are not checked.
4. **HIGH — Secret-safe capture is incomplete.** Key-only redaction misses secrets in argv/URLs/strings and dirty source files are copied verbatim; protected provenance is not represented.
5. **HIGH — Pre-launch rehash does not establish launched identity or source-drift safety.** The pathname is hashed before launch but execution is not bound to the checked file and source identity is not rechecked.
6. **MEDIUM — Canonical wiring is present, but completeness is optimistic.** Optional capture fields and incomplete manifest preservation can still result in unverifiable eligibility.
7. **MEDIUM — Tests do not substantiate the claimed R06 or remote guarantees.** Missing coverage includes basename collisions, staged/unstaged reconstruction, snapshot races, secret-bearing argv/source, incomplete manifests, source drift and lifecycle-level replacement; remote compatibility is not independently verified by the added tests.

## Required remediation before candidate trial

- Make build/source identity fields mandatory for linkage; reject incomplete manifests.
- Validate every distinct launched executable and reject ambiguous basename mappings.
- Make dirty snapshots byte-stable and independently rehashable, with explicit deletion/submodule handling.
- Redact secret-bearing values in all published config/argv surfaces and fail closed or use protected provenance for secret-dependent inputs.
- Bind the final launched executable to the captured identity and recheck source/build identity at the launch boundary.
- Add lifecycle-level RED→GREEN tests for the above and an explicit remote schema-v1 regression.
- Then perform one fresh clean-candidate local trial with a production build manifest; retain ownership/cleanup and remaining lifecycle gates.
