# SPEC-033-R2: NewMessage exhaustion output contract

**Status:** Blocked — superseded as a production candidate by split SPEC-033-R3; test-only evidence retained
**Branch:** AIOPT3
**Scope:** First overload only: `Process::NewMessage(double, short, bool, Message*&)`
**Baseline:** `8e92e295033f894793fee6a5c2c12c7014f05e17`

## 1. Evidence

The production-linked exhaustion harness at
`<workspace-root>/ng-spec033-characterization-20260909/process_exhaustion.cpp`
reproduced the same result three times against the restored baseline:

```text
RESULT initial=3 capacity=30000 status=1 nonnull_output_cleared=0 output_still_aliases_live=1 null_input_remains_null=1 simple_overload_clears=1 complete_overload_clears=1 copy_overload_clears=1 counters_unchanged=1
```

When all 30,000 slots are occupied, the first overload returns `ERROR` but does
not assign `M`. A non-null input therefore remains an alias to an existing live
message. The other three overloads already initialise `M = NULL` before their
search. Failed allocation leaves counters, occupancy and existing messages
unchanged.

This establishes an inconsistent failure-output contract. It does not by itself
prove downstream corruption.

## 2. Normative proposed contract

`M` is an output parameter. Its prior pointer value is discarded on entry; the
function must not delete, release or otherwise modify the previously referenced
object.

- Success: return `OK`; assign one newly allocated message; occupy one free slot;
  increment `NoM` and `MessageCounter` exactly once; preserve current allocation,
  metadata and slot-selection behaviour.
- Capacity exhaustion: return `ERROR`; leave `M == NULL`; leave counters, slots,
  and existing messages unchanged.
- Allocation/construction exceptions are not converted into `ERROR`; no new
  exception policy is part of this amendment.

This is source/signature compatible but can change callers that rely on pointer
preservation after failure. The consistency of the other overloads supports the
contract but does not replace caller compatibility analysis.

## 3. Astra review decision (2026-09-09)

GPT-6 Astra reviewed the real SPEC, implementation, characterization results,
caller inventory and multi-VM evidence.

- **GO:** prepare this narrow amendment and complete the caller audit/tests.
- **NO-GO:** implement production code now.
- **Conditional implementation GO:** only after the caller audit is complete,
the R2 contract is explicitly approved, the preserved RED test is ready to become
GREEN, and no caller blocker requires unauthorized scope expansion.

Astra recommends changing only the first overload. Normalising all four overloads
would add unnecessary production scope.

## 4. Caller audit status

The repository inventory found first-overload uses in Common, PGCS, ContentApp,
NRNCS and other NovaGenesis components. The initial static pass shows many calls
that do not visibly check the integer return value before using or scheduling the
output. This is a compatibility risk, not evidence that every path is defective.

The final audit must record, for every first-overload call:

1. file and containing function;
2. output variable storage and initialisation;
3. overload selected;
4. return-status check;
5. failure path and subsequent dereference/enqueue/delete;
6. whether the caller depends on pointer preservation;
7. disposition under this amendment.

Priorities are CLI/input paths, startup/initialisation, GW handlers, exposition,
periodic handlers, MessageBuilder wrappers, ContentApp, NRNCS and any indirect
wrapper. `GW.cpp` has at least one explicit status check, but its failure branch
and subsequent paths still require review.

A reachable unchecked failure that would require caller edits blocks this
first-overload-only amendment. Caller changes are outside the current allowlist.

## 4.1 Initial static audit evidence

The first production-source scan covered `Common`, `PGCS`, `NRNCS` and
`ContentApp` at HEAD `71df07e` and initially identified 78 syntactic
four-argument first-overload candidates: 72 without a visibly checked status and
6 with one. After excluding comments/definitions and reconciling balanced calls,
the refined semantic workset contains 76 source candidates: 61 blocker
candidates, 8 unresolved and 7 potentially failure-safe. These are triage
classifications, not 61 proven runtime defects; capacity reachability and full
caller lifecycle still require confirmation.

Evidence aid: `Docs/DECISIONS/SPEC033-R2-CALLER-AUDIT-INITIAL.md`.
The reconciled matrix is `Docs/DECISIONS/SPEC033-R2-CALLER-MATRIX.md`.
The semantic matrix must ultimately classify every actual call as failure-safe,
unreachable under an explicit supported invariant, or blocker. “Unchecked” is
not automatically an unsupported failure because the API explicitly returns
`ERROR` on exhaustion.

GPT-6 Astra's follow-up review gives GO for completing this manual matrix and
strengthening test-only characterization, but NO-GO for R2 approval or any
production edit until unresolved callers and the exact test boundary are closed.
## 5. Test requirements before implementation

Preserve the existing harness, raw output, source revision, compiler/build
commands and repeatability evidence. Extend production-linked tests without
private-state shortcuts to cover:

- exhaustion with a non-null live alias;
- exhaustion with null output;
- unchanged `NoM`, `MessageCounter`, occupancy, slot pointers and aliased message;
- all three existing overloads retaining null-on-exhaustion behaviour;
- first-overload success with null and non-null initial output;
- output metadata, registration and exactly-one counter increments on success;
- remove one message, allocate successfully, and restore full capacity through
  supported production APIs;
- clean teardown without leaks or double deletion.

The RED assertion must fail specifically because a non-null output remains
non-null, not merely because diagnostic fields are printed. After implementation,
the identical suite must become GREEN without weakening assertions.

## 5.1 Strengthened characterization result

The external production-linked harness was extended to cover supported slot
reclamation and subsequent successful allocation with a non-null output. It was
compiled against the restored baseline and executed three times:

```text
repeat-0 exit=10 ... counters_unchanged=1 recovery_success=1
repeat-1 exit=10 ... counters_unchanged=1 recovery_success=1
repeat-2 exit=10 ... counters_unchanged=1 recovery_success=1
```

The expected baseline RED remains isolated to
`nonnull_output_cleared=0` / `output_still_aliases_live=1`. The new recovery
invariant passes in all three runs: one message was reclaimed through public
`MarkToDelete()` + `DeleteMarkedMessages()`, the first overload then allocated
successfully, restored full occupancy, and the previously retained live message
remained valid. No production source was changed.

## 6. Exact boundary

Production allowlist:

- `Common/src/Process.cpp`
- entry initialisation of `M` in the first overload only

Non-production allowlist must name exact test/amendment files before approval.
The existing external characterization harness may remain an evidence artifact
if its bytes and execution are preserved.

Forbidden collateral:

- `Process.h` signature/layout changes;
- caller edits under this amendment;
- allocator, queue, container, capacity, counter, removal or ownership redesign;
- sparse `GetMessage`, parser/lookahead, Block lifetime or RAII fixes;
- GW batching, scheduler/wire-format changes, throughput work or SPEC-032;
- NRNCS/SSID fixes, warning cleanup or build/configuration changes.

## 7. Runtime gates and rollback

After GREEN unit/production-linked tests, rebuild both Alpine guests from the
same approved source and build path. Require candidate executable hashes to
match across VMs. Repeat fresh-boot multi-VM delivery of 100 generated 800x600
JPEGs: 100/100 files, byte-exact SHA-256 sets, no missing/extra/mismatched files,
and no error/alarm/drop/fail markers. This is an ordinary-runtime non-regression
gate, not a throughput or saturation-safety claim.

If any test or matched runtime gate fails, preserve evidence and revert only the
isolated correction with a new reviewed commit. Do not restore the withdrawn
allocator/queue implementation or add unreviewed caller fixes.

## 8. Current decision

This document prepares R2 and records the Astra conditions. Production
implementation remains blocked until the complete caller matrix and exact
production-linked regression test boundary are reviewed and the amendment is
explicitly approved.
