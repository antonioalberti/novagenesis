# SPEC-033 R3-C — Compatibility blockers found in complete callers

**Source revision:** `0dc2b38e1d95e09877a4b3e7c90cf2d3bce282b9` (AIOPT3)  
**Status:** Evidence-only finding; C production remains unauthorized

## Finding

The first-overload-only proposal `M = NULL` is not yet compatible with at least three complete caller loops. These callers reuse the output pointer across iterations without resetting it before the next allocation. The current first overload preserves the previous pointer when capacity is exhausted; entry-clearing C would replace that observed value with `NULL`.

This does not mean the current reuse is correct. It means C cannot be accepted without explicitly resolving the caller contract. A nulling producer-only change could convert an existing stale/reuse path into a null dereference in a builder or sender.

## 1. `PGCS/src/PGRunPublishing01.cpp:122`

- `Message* Publish = 0` is declared once before the `PSTuples` loop.
- Each iteration calls the first overload without resetting `Publish`.
- On success, builders construct the message and line 149 publishes it to the GW input queue.
- The vector fields are cleared, but `Publish` is not cleared or reclaimed locally.
- On a later capacity failure, current behavior retains the previous queued message pointer.
- Subsequent builders and `PushToInputQueue(Publish)` reuse that pointer.
- C would set `Publish = NULL` before the failed allocation; subsequent builders would receive null.

Disposition: `POINTER-PRESERVATION-COMPATIBILITY-RISK`, requiring caller-specific contract resolution outside the current C allowlist.

## 2. `PGCS/src/PGRunHello02.cpp:104`

- `Message* PGIHCHello = 0` is declared once before the stack loop.
- Each eligible iteration calls the first overload without resetting it.
- On success, the message is built, sent through the raw-socket path and marked for deletion at line 223.
- The output pointer is not reset after marking.
- On a later capacity failure, current behavior retains the previous message pointer, which may already be marked.
- Subsequent builders, socket sends and `MarkToDelete()` operate on that retained pointer.
- C would clear the output and cause later builder/sender use to see null.

Disposition: `POINTER-PRESERVATION-COMPATIBILITY-RISK`, requiring caller-specific loop/ownership resolution outside C.

## 3. `PGCS/src/PGHelloIHC03.cpp:209`

- `Message* StoreBind01Msg = 0` is declared once before the compatible-stack/peer loop.
- Each matching peer iteration calls the first overload without resetting it.
- On success, multiple binding builders populate the message and line 524 submits it to the GW input queue.
- The output pointer is not cleared after publication.
- On a later capacity failure, current behavior retains the previously submitted pointer and the builders execute against it.
- C would clear the output and alter the subsequent builder path to receive null.
- Peer tuple state is also updated independently; no rollback is shown.

Disposition: `POINTER-PRESERVATION-COMPATIBILITY-RISK`, requiring caller-specific loop/ownership resolution outside C.

## Consequence for C

These are not safe candidates for a producer-only output-clearing change. The current C proposal must remain `NO-GO` until each loop is explicitly resolved. Possible resolutions include caller resets/early returns or a supported invariant, but those are caller changes and are not authorized by C.

Do not infer that preserving the old pointer is intended or correct; the evidence establishes only that it is observable under the current implementation and may be consumed by later code.

## Validator status

These findings are submitted to GPT-6 Astra for confirmation. No production code was changed.
