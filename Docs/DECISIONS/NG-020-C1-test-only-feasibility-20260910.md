# NG-020 / SPEC-033 R3-C1 — Test-only feasibility assessment

**Source revision:** `0dc2b38e1d95e09877a4b3e7c90cf2d3bce282b9` (AIOPT3)  
**Status:** Test-only feasibility blocked; no production changes

## Assessment

The three target `Run()` functions are public virtual action methods, but their valid execution requires a populated PGCS/PG object graph, received command lines, peer tuples, stack metadata, Core/GW/HT blocks and MessageBuilder/transport state.

The current public APIs expose Process message count and message inspection, but do not expose deterministic event counts for:

- MessageBuilder calls;
- SCN/name generation;
- raw-socket sends;
- GW queue publication;
- `MarkToDelete()` calls attributable to one iteration;
- ownership transfer of messages already queued/sent.

The existing repository contains no C1-specific public-lifecycle harness that supplies the required object graph and event trace. A test that only fills Process capacity and invokes a caller would not prove that the failed iteration skipped builders, sends, queue publication and marking.

## Exact C1 targets

- `PGCS/src/PGRunPublishing01.cpp::PGRunPublishing01::Run()` — `Publish` at line 122;
- `PGCS/src/PGRunHello02.cpp::PGRunHello02::Run()` — `PGIHCHello` at line 104;
- `PGCS/src/PGHelloIHC03.cpp::PGHelloIHC03::Run()` — `StoreBind01Msg` at line 209.

## Current decision

The existing public interfaces are sufficient to reproduce the producer output defect, but not sufficient to prove the full C1 downstream-event contract. Adding a friend, production callback, public counter, header change or instrumentation hook would exceed C1 and requires a separate seam SPEC/review.

No test-only C1 fixture was fabricated because it would either:

- omit the required event assertions; or
- use private state/unauthorized production instrumentation.

## Astra disposition

Astra granted `CONDITIONAL GO` for C1 test-only work but required the exact test manifest and event observability to be demonstrated first. Astra selected immediate `return ERROR` for all three callers and `NO-GO` for production.

This feasibility assessment shows that the event-observability gate is not currently closed. C1 and producer C remain blocked. B1/B2 remain accepted; B3 remains closed as no defect demonstrated.

## Next safe action

Do not create a seam implicitly. Present this feasibility gap for a separate scope decision. If a seam is approved, create a standalone SPEC defining its test-only build boundary, instrumentation, proof that production behavior is unchanged, and rollback. If no seam is approved, retain the evidence-only C1 finding and defer the caller amendment.
