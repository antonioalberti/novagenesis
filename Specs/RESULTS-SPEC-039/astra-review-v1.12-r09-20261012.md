## 1. VERDICT: AMEND — documentary contract only

Reviewed only the supplied packet identified as SHA-256 `560ee5654d9a8859e3e21aa916a1d7a6` **[correction: exact identity below]**:

`560ee5654d9a8859e3e21aa916a1dca9f0e564fe1f8eac41fb4ee8f4bd3397a6`

The r09 amendment substantially addresses r08 B2/B3, but contains one blocking contradiction about finalizer responsibilities. No unchanged r07 checker findings are reopened.

- **Dedicated UID/launcher/lease:** adequate as a mandatory, fail-closed prerequisite, not proof of an existing enforcement mechanism. The explicit `NOT_PROVISIONED`/`BLOCKED` states, prohibition on ordinary-host-UID execution, rejection of census-only evidence and trusted-root boundary appropriately prevent a readiness claim.
- **Diagnostic unlink suppression:** properly bounded to the three pinned branches, the opt-in isolated diagnostic build and targeted zero-unlink tests. It complements, rather than replaces, continuous exclusion.
- **Journal and inventories:** the required retained bytes, intent/outcome ordering, fsync obligations, ambiguous-crash handling, no-retry rule and final hash binding substantially close the earlier evidence gaps. Raw inventory digests and summaries must be independently recomputed.
- **Semaphore mapping:** the retained matching maps-line route can support pointer-range/shared-mapping/device/inode checks against the recorded semaphore and libc identity. This is an untested evidence procedure; missing or contradictory linkage must remain `INCONCLUSIVE`.
- **Classification:** schemas alone do not establish safety. The combined semantic contract appropriately requires evidence-derived status and excludes unsafe or incomplete runs from both PASS and correctness FAIL, subject to resolving the blocker below.

## 2. Must-fix blocker

### B1 — Contradictory finalizer IPC boundary

**Exact locations**

- **“Proposed runtime artifacts”**, D2 proposed allowlist row: `--finalize` performs finalisation “without constructing Process/GW **or touching IPC**”.
- **§7.1**, paragraphs beginning “The three pinned…” and “After Gateway/output threads…”: only the separate finalizer may clean resources; it verifies current identities and performs removal.
- **`collection_responsibility.outer_finalizer`**: the finalizer logs cleanup intent/outcome and performs exact identity-bound cleanup.
- **`sandbox.runtime_finalizer_cleanup`**: cleanup occurs after the run process exits.

**Concrete risk:** these requirements cannot all be satisfied. An implementation following the no-IPC clause cannot perform the assigned cleanup; an implementation following the cleanup clauses breaches that explicit boundary. It also leaves current-identity inspection and cleanup ownership contradictory at the point where safety depends on them.

**Required amendment:** establish one consistent owner and sequence. If `--finalize` owns cleanup, replace “or touching IPC” with a narrowly defined allowance for inventory/current-identity inspection and journal-authorised exact removal under the continuously verified lease. Continue to prohibit Process/GW construction, resource creation, production IPC use and speculative or broad cleanup. Align the cited clauses.

## 3. Optional advisories — not blockers

- Make the matching `mapping_line` mandatory for successful semaphore evidence, or add explicit mapping permissions and offset fields. The current structured alternative lacks those fields; the semantic contract must reject projections that cannot prove a shared mapping.
- Separate historical fixture counts from newly required negative classes and remove duplicated policy entries. These are not evidence that r09 semantic fixtures ran.
- State explicitly that retained journal bytes support replay but do not independently prove historical fsync calls; durability also depends on the subsequently verified collector implementation.

## 4. Authority boundary

The launcher remains **unprovisioned**, and execution remains **BLOCKED**. No tools, code execution, hash recomputation or runtime validation were performed.

**No operational authority is granted:** no implementation, configure/build, harness/finalizer execution, IPC, namespace, VM/SSH, D3, cleanup, commit, push or release. Documentary approval, if subsequently obtained, would not replace the separate provisioning, verification and owner-authorisation gates.