# SPEC-044 preliminary RED result

Date: 2026-09-12
Branch: AIOPT3
Source commit observed: e8faaf3

## Stimulus

```text
timeout 5s ./cmake-build-sanitizer/PGCS ./IO/PGCS/ 0 Intra_Domain -lc
```

## Result

- Exit status: 124 (controlled timeout after the process remained alive).
- Core/Gateway startup reached.
- UBSan diagnostic reproduced:

```text
PGCS/src/CoreRunEvaluate01.cpp:425:10:
runtime error: downcast of address ... which does not point to an object of type 'PG'
note: object is of type 'Core'
```

- No ASan SEGV occurred in this run.
- Full log: `/tmp/ng039-pgcs-sanitizer-smoke.log`.

## Contract observations

- `PGCSTuples` is declared as a member of `PG` in `PGCS/src/PG.h`.
- `Core` is a separate `Block` subclass in `PGCS/src/Core.h`; it has `PGW` and `PHT`, but no `PGCSTuples` member.
- `CoreRunEvaluate01` stores its owner in `Action::PB`, and `Core::NewAction()` constructs this action with `this`, so `PB` is a `Core*` in this path.
- `PB->PP` is a `Process*` pointing to `PGCS`; `PGCS` does not declare `PGCSTuples`.
- Therefore the current access through `PG* PPGB = (PG*)PB` has no demonstrated valid object provenance.

## Decision boundary

This is a valid RED characterization, not permission to replace the cast. A production fix requires a separately reviewed design for obtaining the owning `PG` state or removing/reworking the dependency while preserving subscription behavior.
