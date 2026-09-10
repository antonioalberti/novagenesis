# SPEC-033-R3-C: Process::NewMessage failure-output contract

**Author:** Antonio Alberti / Hermes Agent  
**Date:** 2026-09-10  
**Status:** Proposal — production blocked pending caller audit and Astra review  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** `SPEC-033-R2-newmessage-exhaustion-output.md`, `SPEC-033-R3-caller-safety-amendment.md`, `SPEC-033-R3-B2-gw-input-queue-null-safety.md`

## 1. Purpose and boundary

Define the failure-output contract of the first overload only:

`Process::NewMessage(double, short, bool, Message*&)`

The current implementation returns `ERROR` on capacity exhaustion but does not initialise `M` before searching for a free slot. A non-null incoming output therefore remains unchanged on failure, unlike the other overloads, which initialise their output to `NULL`.

This SPEC does not authorise production changes. B1 and B2 remain accepted in their independent scopes. B3 was audited without a demonstrated normal-path defect. Caller migration, queue redesign, allocation policy, exception policy, parser, scheduler and wire-format changes are excluded.

## 2. Observed baseline

The production-linked fixture `process_exhaustion.cpp`, compiled against the current AIOPT3 source, was executed three times:

```text
RESULT initial=3 capacity=30000 status=1 nonnull_output_cleared=0 output_still_aliases_live=1 null_input_remains_null=1 simple_overload_clears=1 complete_overload_clears=1 copy_overload_clears=1 counters_unchanged=1 recovery_success=1
```

All trials returned exit code `10`, the expected RED for the proposed output contract. The fixture also proved that public reclamation frees one slot and a subsequent allocation succeeds while the retained live message remains valid.

Evidence files:

- `/home/gandalf/workspace/ng-spec033-characterization-20260909/process_exhaustion.cpp`
- `/home/gandalf/workspace/ng-spec033-characterization-20260909/process-c-current-1.log`
- `/home/gandalf/workspace/ng-spec033-characterization-20260909/process-c-current-2.log`
- `/home/gandalf/workspace/ng-spec033-characterization-20260909/process-c-current-3.log`

## 3. Proposed contract

For the first overload:

- Success: return `OK`, assign one newly allocated message, occupy one free slot, increment `NoM` and `MessageCounter` exactly once, and preserve current metadata/slot behaviour.
- Capacity exhaustion: return `ERROR`, set `M == NULL`, leave `NoM`, `MessageCounter`, slot controls and existing message objects unchanged.
- The prior referent, if any, must not be deleted, released, marked or otherwise modified.
- Allocation/construction exceptions remain outside this proposal.

The proposed change would be limited to initialising the output reference at function entry. No caller changes are included.

## 4. Compatibility risks

The first overload has many callers across Common, PGCS, NRNCS and ContentApp. Some ignore the return status or use the output before/after the call. A caller may rely accidentally on pointer preservation after failure, or may dereference a null output before reaching a queue boundary.

The caller audit must classify each actual first-overload call as `failure-safe`, `invariant-protected`, `blocker` or `unresolved`, with exact provenance, status handling, preceding/following uses and ownership. A production change is blocked if a reachable caller requires an edit outside this SPEC.

Known focused findings:

- `Common/src/CLI.cpp:69–81`: status ignored; `PIM` starts null and is dereferenced before queue at line 72 and after at lines 78/81. Upstream blocker, outside C.
- `Common/src/GW.cpp:614–622`: status checked; `PM` starts null and is assigned before `OK`; queue boundary is invariant-protected in the current path. No C caller edit justified.
- `PGCS/src/PG.cpp:260`: helper rejects null before publication and uses no pointer after; failure-safe at the boundary.

## 6. Current exact-overload audit state

The corrected mechanical filter excludes comments, overload definitions and non-target overloads. At the pinned AIOPT3 checkout it reports **76 actual four-argument call sites** for the first overload. The prior count of 77 included the definition at `Process.cpp:542` and was corrected before any production decision.

The complete evidence-only workset is:

`/home/gandalf/workspace/ng-spec033-characterization-20260909/SPEC-033-R3-C-exact-overload-callers.md`

The first three focused classifications are:

- `Common/src/CLI.cpp:69`: upstream blocker on capacity failure; status ignored and output dereferenced before/after publication.
- `Common/src/GW.cpp:615`: invariant-protected at the shown queue boundary; `PM` starts null, status is checked and assignment precedes `OK`.
- `PGCS/src/PG.cpp:234`: unresolved upstream caller; the separately analysed `PushStressMessage()` helper at line 260 does not protect this earlier path.

GPT-6 Astra's current verdict is `NO-GO` for C production. The current provisional triage of the 76-row inventory reports 4 callers as `SAFE`, 8 as `UPSTREAM-DEFECT-OUTSIDE-C` and 64 as `UNRESOLVED`; no pointer-preservation compatibility risk was demonstrated, but compatibility is not thereby proven. Subsequent batch review found that some supplied blocks were nested excerpts rather than complete functions, so these counts are not semantic closure. The consolidated audit is:

`/home/gandalf/workspace/ng-spec033-characterization-20260909/SPEC-033-R3-C-semantic-caller-audit-final.md`

The 64 unresolved rows require a revision-stamped evidence supplement with complete declarations, loops, aliases, cleanup, exception handlers and transitive helper contracts. The eight upstream defects cannot be repaired by this first-overload-only amendment. No separate test seam is currently necessary.

## 7. Required gates

1. Complete and revision-pin the first-overload caller matrix.
2. Confirm the proposed output-only change does not invalidate any supported caller contract.
3. Preserve and rerun the production-linked RED fixture, including non-null incoming output, null incoming output, all other overloads, unchanged counters/slots, recovery and cleanup.
4. Obtain a fresh Astra conditional GO for this exact first-overload-only scope.
5. Obtain explicit user approval before editing `Common/src/Process.cpp`.
6. Implement only output initialisation in the first overload.
7. Run identical RED→GREEN tests, ASAN and full build.
8. Reconcile caller regressions and obtain post-implementation Astra acceptance.

No runtime Alpine requirement is assumed by this SPEC unless the final approved acceptance criteria add one. B1/B2 runtime evidence is not transferred automatically.

## 6. Current disposition

`NO-GO` for production implementation until the caller matrix and contract compatibility review are complete. The current fixture establishes a real, reproducible contract inconsistency, not permission to fix it.
