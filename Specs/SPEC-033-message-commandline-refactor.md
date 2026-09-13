# SPEC-033: Message and CommandLine — controlled restart

**Status:** Draft — planning only; implementation is NOT approved
**Revision:** Restart 1
**Date:** 2026-09-09
**Branch:** AIOPT3
**Baseline:** 8e92e295033f894793fee6a5c2c12c7014f05e17
**Restoration commit:** 77efe27fafbd7483507ed1448cbeedf04782bff4
**Related:** SPEC-031 (retained), SPEC-032 (deferred), SPEC-030 (delivery verification)

## 1. Decision and boundaries

Restore the successful baseline first; investigate and improve message safety in
small, independently reversible increments. This draft supersedes the previous
SPEC-033 execution plan, not the evidence retained in its historical archive.
The user approved the rollback and continued validation/planning, not this new
implementation. No part of the withdrawn implementation is automatically approved.

Non-goals for this restart:
- GW batching, changing the number of messages executed per scheduler iteration;
- changing scheduler deadline comparisons, waits or action ordering as a side effect;
- changing the wire format, removing type/cardinality fields or changing SCN bytes;
- combined parser/container/retention/RAII refactoring;
- throughput optimisation, PG structural refactoring or Phase 2 concurrency work;
- fixing unrelated NRNCS cache or SSIDs issues inside a message refactor commit.

SPEC-032 remains deferred until explicitly reopened, after this effort.

## 2. What is known, and what is not

The restored source retains concrete safety risks, notwithstanding successful
photo tests. Direct source inspection on the baseline shows:

| Finding | Source | What is established | Next evidence required |
|---|---|---|---|
| NewMessage exhaustion output | Common/src/Process.cpp:542-576 | First overload does not initialise output M before scanning slots; other overloads do | Real Process exhaustion test and caller error-handling inventory |
| Sparse slot access | Common/src/Process.cpp:701-709 | GetMessage bounds by NoM and returns OK even when output was not assigned | Establish whether each caller expects slot identity or ordinal enumeration |
| Parser fixed storage | Common/src/CommandLine.cpp:383,402-407 | Space positions stored in a fixed 4096 array without an insertion bound | Isolated ASan/UBSan reproducer with controlled input; never inject this into the operational baseline |
| Parser lookahead | Common/src/CommandLine.cpp:469-487 | Loop permits last Words element and reads Words[l+1] | Minimal malformed input and both parser entry paths |
| Run lifetime | Common/src/Block.cpp:178 onward | Pointer membership check precedes execution; this alone is not lifetime ownership | Enumerate retaining threads/queues and all delete/erase paths |
| Scheduler semantics | Common/src/GW.cpp:432-590 | Baseline pops at most one due input message per iteration | Contract test against production scheduler before any future queue migration |

Line numbers are for the baseline, not the withdrawn implementation. Historical
reviews provide hypotheses and test candidates, not proof of current behaviour.
The baseline is suitable for controlled testing, not certified safe for hostile inputs.

The previous attempt mixed several moving parts. A harness passing while the real
bootstrap or photo pipeline fails is an integration gap, not a runtime sign-off.
The later FreeSlot dereference fix and per-batch RunFlag fix must not be replayed:
the corresponding new allocator/batch mechanisms are absent from this baseline.
The NRNCS missing-configuration issue remains a separate robustness candidate;
normal baseline tests must preserve and hash NRNCS.ini, never delete it as cache.

## 3. Stage R0 — freeze and qualify the baseline

No production edits. Record source commit/tree, build directory/options, binary
SHA-256, config hashes, payload manifest, boot identity and exact launch commands.
Use clean run directories and immutable staged payloads. Never reuse a previous
publisher's mutable staging directory or wipe a live NRNCS cache.

Required gates before the first implementation:
1. Full local build and repeated clean local bootstrap/photo delivery.
2. Fresh guest boots and 1000/1000 multi-VM delivery, zero SHA-256 mismatches,
   zero missing/extra JPEGs; distinguish file arrival from native application acceptance.
3. Explicit native MurmurHash3 acceptance evidence for every payload, or label
   this coverage gap and approve a narrowly scoped observation/test amendment.
4. Paired 500 and 2000 msg/s workloads with observed rate, progress and resource
   measurements; then a 30-minute soak with acceptance criteria fixed before launch.
5. Loss certification requires a valid stop-offering/drain method. The current
   StressEnabled flag also gates reception; simply setting it false is NOT a
   valid sender-only drain mechanism. Do not silently patch this into production.

Current measurements and gaps live in
`../Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`. A bounded smoke is not a soak.
A previous session's 2000 msg/s pass is historical evidence, not a new measurement.

## 4. Stage R1 — characterisation and ownership audit (no production edits)

Do this BEFORE selecting a new container or integrating a parser:
- Build a corpus from actual generated/received command lines and serialization
  entry points, preserving wire bytes, cardinalities, type tokens and SCN/hash inputs.
- Map Process allocation, lookup, removal and destruction call sites, including
  ScheduledMessages, inline responses, input/output queues, retry paths and shutdown.
- For each reference: owner, borrower, thread, acquisition, transfer, release and
  last legal dereference. Treat pointer membership as distinct from identity/lifetime.
- Reconcile contradictory comments against callers, not against historical prose.
- Identify scheduler invariants: due-time rule, one input pop/Run, output retry,
  housekeeping order, StopProcessingMessage lifetime, and startup action sequence.
- Add tests linked to the real implementation. Standalone models may supplement
  these tests but can never be their sole acceptance evidence.

Deliverable: an evidence table with reproduced failures, unresolved hypotheses,
proposed minimal fixes and explicit call-site references. Review directly in this
Astra session; no redundant second Astra invocation is needed.

## 5. Stage R2 — approve exactly one minimal safety correction

Recommended first candidate to investigate: NewMessage output on exhaustion,
because the source delta can be narrow and independent of queues/parser structure.
This is a candidate, not permission to modify it or a claim of reproduced failure.

Before implementation submit a short amendment specifying:
- reproduced failing test against the actual baseline;
- exact pre/post API contract and every affected caller;
- allowed files and expressly forbidden collateral changes;
- regression tests, matched runtime gates and exact rollback boundary.

User approval is required. Then execute RED -> minimal fix -> GREEN -> runtime
regression -> evidence review. Do not start a second correction while the first
has any failed or unclassified gate. Record one correction per commit.

Parser memory bounds, sparse lookup semantics and delete/delete[] defects must
be separate amendments if reproduced. Reject malformed input safely; do not
invent compatibility-breaking limits without measuring real input and agreeing
the protocol contract.

## 6. Later stage R3 — parser consolidation, only after compatibility proof

Prerequisite: R0-R2 gates satisfied and a separately approved parser amendment.
Keep old writer/wire representation unchanged. Specify valid legacy input corpus,
invalid-input rejection, object state on error, ownership/copy semantics and
numeric limits. Error means transactional failure, not partial mutation.

Integrate one actual reader path at a time, with an explicitly temporary two-path
compatibility state and paired tests. Prove wire round-trip and SCN byte identity
for each real caller. End-to-end photos and bootstrap are mandatory after EACH
adapter change, before moving on. No Process/container changes in this stage.

## 7. Later stage R4 — ownership/container, separate design decision

Do not presume generational handles, free-list, mutexes or shared ownership are
the chosen solution. R1 must first establish the required identities and lifetimes.
If handles are chosen, review at least:
- one allocator/reclaimer for legacy and new APIs, invalid sentinel disjoint from
  valid slots and all return codes, generation reuse/wrap policy;
- identity validation atomic with retention acquisition versus destruction;
- enqueue/pop/Run/retry/drop/shutdown ownership accounting without gaps or double retain;
- no dereference after delete in reclamation bookkeeping;
- copied ordering keys versus mutable message time/tag semantics;
- defined lock ordering, no action execution under queue/lifecycle locks, no deadlock;
- all error paths, saturation, duplicate release, stale handles and null inputs.

Queue migration is a separate gated amendment, not bundled with allocator
introduction. It must preserve one-pop/one-Run and the baseline scheduler contract.
Repeated clean bootstrap and full photo delivery take precedence over microbenchmark
speed. RAII/payload representation changes require yet another design/approval.

## 8. Common acceptance, stop rules and rollback

Each increment must have production-linked tests, a clean full build, repeated
local bootstrap/photo tests and fresh-boot multi-VM photo gates. Any change touching
queues, lifetime, parsing or the hot path also repeats the matched load workload.
Use the same manifest/configuration for baseline and candidate, with isolated cache
state and separate durable logs. Sanitizers are diagnostic builds, not performance
comparators. Specify seeds/durations and archive crashing inputs before rerunning.

STOP on a crash, sanitizer finding, native hash rejection, file mismatch, duplicate
process, no-progress interval with offered traffic, missing telemetry, unresolved
runtime regression or exceeded storage budget. Do not layer a fix onto an unexplained
failure. Capture evidence, revert the single candidate, re-run the same workload on
the previous accepted commit and classify baseline versus candidate behaviour.

Before each candidate, record the accepted commit/tag. Roll back by a new reviewed
revert commit on AIOPT3, not a force-push. Preserve diagnostic evidence outside active
code. If an invariant or acceptance criterion changes, amend the spec and request
approval before resuming. Partial success is never "Implemented".

## 9. Immediate next decision

Finish R0 evidence gaps and R1 characterisation. Then present ONE minimal correction
amendment for approval. Do not implement parser consolidation, handles, batching,
scheduler rewrites or wire-format changes while that decision is pending.
