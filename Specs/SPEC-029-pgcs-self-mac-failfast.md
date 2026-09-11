# SPEC-029: PGCS Fail-Fast Validation of Self-MAC Peer Configuration

**Version:** v1.0
**Date:** 2026-09-07
**Author:** Antonio Alberti (design per Astra/gpt-6-astra review)
**Status:** In Progress
**Branch:** AIOPT3
**Related:** SPEC-027-pgcs-multithreading.md

---

## 1. Problem / Objective

With `-p` (deterministic peers), `PGCS` binds **two** HT category-17 values for the
configured identifier when that identifier equals the local interface MAC:
`MAC → CSID` (client socket, ~line 346) and `MAC → SSID` (server socket, ~line 367).
`PG::SendToARawSocket` then finds `SIDs->size() != 1` and refuses to send — every hello
fails, `PGCSTuples` never populates, and inter-VM stress traffic never flows
(`ERROR: more than one identifier to this address <own-MAC>`).

Objective: detect the misconfiguration at start-up and fail fast with an actionable
message, instead of running in a degraded state that is hard to diagnose.

## 2. Root Cause

Configuration error (run scripts pass the wrong MAC as peer — verified during the
2026-09-07 session) silently accepted by `PGRunInitialization01::Run`. The code itself
is correct per the intended semantics: `Identifiers->at(i)` is the **remote peer MAC**.

## 3. Proposed Solution

In `PGRunInitialization01.cpp`, for `Ethernet`/`Wi-Fi` stacks, immediately after
`GetHostRawAddress()`:

1. Normalize both MACs (lowercase, same separator).
2. If `PPGCS->Identifiers->at(i) == MAC` (local): log an actionable ERROR block and
   abort initialization (`Status = ERROR; return Status;`). Do NOT create sockets or
   bindings in this state.

No changes to: bindings themselves, `PG::SendToARawSocket`, hello logic,
`PGMsgCl01`, stress scheduling, NGAL/SAR.

## 4. Files Affected

| File | Change | Reason |
|---|---|---|
| `PGCS/src/PGRunInitialization01.cpp` | Self-MAC validation after `GetHostRawAddress()` | Fail fast on misconfiguration |

## 5. Acceptance Criteria

1. Starting PGCS with `-p ... eth0 <own-MAC> 1200` aborts with the ERROR block and
   does not create sockets/bindings.
2. Starting PGCS with the peer's MAC proceeds exactly as before (behaviour identical
   to HEAD for correct configuration).
3. Both VMs start, hellos flow, `PGCSTuples` populate, stress pings flow both ways.

## 6. Rollback Plan

`git revert <commit>` on AIOPT3; rebuild on VMs.

## 7. Implementation Notes

- Astra review (2026-09-07): validation must **propagate failure**, not merely set
  `Status = ERROR` and continue.
- Soak interpretation correction: the 30-min soak (SPEC-027) is recorded as
  "survival with bounded RSS under unverified/no demonstrated stress load" — not a
  stress-validation pass.

## 8. Decisions Log

| # | Decision | Date | Reason |
|---|---|---|---|
| 1 | Config fix over code redesign | 2026-09-07 | Binding logic correct for intended `-p` semantics |
| 2 | Fail-fast abort, not warn-and-continue | 2026-09-07 | Astra: silent degraded state caused multi-hour misdiagnosis |

## 9. Pitfalls

- MAC string comparison must be case-insensitive (the same MAC in different case variants).
- Do not break the `FF:FF:FF:FF:FF:FF` broadcast configuration path (it never equals local MAC).
