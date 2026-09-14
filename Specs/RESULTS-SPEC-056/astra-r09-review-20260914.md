# Astra R09 Review — SPEC-056

**Date:** 2026-09-14
**Commit reviewed:** `a4dfbcc` (parent `3c9dc9d`)
**Scope:** read-only source/spec review; no runtime sign-off.
**Model:** `gpt-6-astra`
**Verdict:** `HOLD / NO-GO` for the one clean local candidate-trial step.

## Findings

1. **HIGH — Build-identity semantics remain permissive; index linkage improves.** Nested numeric/boolean values can satisfy meaningful-identity checks; e.g. `{"id": False}` can count as runtime identity. Per-role presence is enforced, but semantic runtime-library identity is not.
2. **HIGH — Secret-safe publication and protected-input blocking remain incomplete.** Quoted assignments are redacted, but text rewrites do not consistently create protected-input blockers; unreadable/rewrite failures can leave secrets; secret-bearing filenames are not scanned; manifest-time blockers do not necessarily downgrade/abort sealing.
3. **HIGH — Descendant-safe rollback is not established.** Descendants are sampled once before killing the leader; later descendants can escape, and rollback scan/stop exceptions are not fully guarded.
4. **MEDIUM — Lifecycle positive control remains insufficient.** The four-role fixture requires launch/readiness events but expects diagnostic exit 11 and does not prove readiness success, observation, reverse stop, final inventories or accepted offline bundle.
5. **MEDIUM — Remote compatibility and sealed-bundle secret testing remain unproven.** Tests cover an unsealed helper tree and schema/default shape, not the controller-produced terminally sealed bundle or full remote validation/lifecycle.

R09 does not close R08’s remaining blockers. SPEC-056 and all release gates remain open.
