# SPEC-033-R3: Caller-safe allocation failure handling

**Status:** A e B0 implementados e aceites pelo Astra nos commits `8b4aeaf` e `1f89c31`; B1/B2/B3 e C continuam bloqueados por gates próprios.
**Branch:** AIOPT3
**Related:** SPEC-033 Restart 1, SPEC-033-R2, `Docs/DECISIONS/SPEC033-R2-CALLER-MATRIX.md`

## 1. Trigger and decision

The R2 candidate proposed setting `M = NULL` at entry of the first
`Process::NewMessage(double, short, bool, Message*&)` overload. The production-
linked RED is real and repeated, but the caller matrix found unchecked immediate
uses in active runtime paths. Returning NULL without caller handling could turn
existing stale-output misuse into a deterministic null dereference.

GPT-6 Astra's final gate therefore returned:

```text
NO-GO — redesign/split amendment
```

The R2 production patch is blocked. The next work item is a separate caller-
safety amendment; no production source changes are authorised by this document.

## 2.1 Confirmed MessageBuilder blocker

A production-linked external fixture was added at
`/home/gandalf/workspace/ng-spec033-characterization-20260909/wrapper_exhaustion.cpp`.
It fills the real Process to 30,000 messages and invokes each of the five
MessageBuilder wrappers with an independently retained live output pointer.
Against the restored baseline, compilation succeeded and execution returned:

```text
RESULT wrappers=1,1,1,1,1 out0_null=0 out0_marked=1 out1_null=0 out1_marked=1 out2_null=0 out2_marked=1 out3_null=0 out3_marked=1 out4_null=0 out4_marked=1 count=30000
```

This is a confirmed failure-handling blocker: every wrapper returns `ERROR`, but
all five output references remain non-null and marked for deletion. The wrappers
therefore preserve/consume a prior live object on allocation failure. If R2
later clears `_M` before the wrapper's unconditional `_M->MarkToDelete()`, the
same path can become a null dereference. The run also exposes the separate
`NewConnectionLessRunMessage(..., Message* _M)` by-value output contract, which
must not be silently repaired in R3.

Evidence files: `wrapper_exhaustion.cpp`, `wrapper-baseline-run.log`, and
`wrapper-build.log` in the external characterization directory. This fixture is
characterization only; no production source was modified.


`NewMessage()` already exposes capacity exhaustion as `ERROR`. A caller must not
consume, enqueue, mutate, serialize, execute or delete an invalid output after
that failure. If the output variable held an old pointer, failure handling must
not delete or release it accidentally; ownership must remain explicit.

The eventual API correction may still clear `M` on entry, but it is not safe to
apply it until all affected callers are either failure-safe or protected by an
explicit, supported and enforced capacity/lifecycle invariant.

## 3. Scope of the split amendment

The amendment will be divided into independently reviewable caller groups:

1. Common framework/GW/CLI and MessageBuilder wrappers;
2. PGCS startup, periodic, hello, exposition, publishing and stress paths;
3. NRNCS startup, periodic, binding delivery and notification paths;
4. ContentApp startup, periodic, discovery, subscription, expose and photo paths.

Each group must have its own exact file list, tests, commit and rollback boundary.
No group may silently include parser, allocator, queue, scheduler, wire-format,
RAII, batching, throughput or unrelated warning changes.

## 4. Required per-call evidence

For every first-overload call in the reconciled matrix:

- exact overload and output storage location;
- initial value and whether it can be non-null/reused;
- status handling and all failure exits;
- subsequent dereference, builder call, enqueue, scheduling, execution,
  serialization, deletion or propagation;
- ownership/alias relationship to Process bookkeeping;
- supported capacity/lifecycle invariant, if claimed;
- test demonstrating failure-safe behavior;
- final disposition: `failure-safe`, `invariant-protected`, `blocker` or
  `unresolved`.

The existing matrix is conservative triage. Ordinary successful runtime and low
observed occupancy do not clear an `ERROR` path.

## 5. Test-only work allowed now

Allowed before production approval:

- strengthen `/home/gandalf/workspace/ng-spec033-characterization-20260909/process_exhaustion.cpp`;
- add test-only fixtures/runners in that external characterization directory;
- add/update decision documents under `novagenesis/Docs/DECISIONS/`;
- update this SPEC and the R2 matrix.

Tests must cover non-null and null outputs on exhaustion, preservation of the
previous object without accidental deletion, unchanged counters/slots/controls,
success after supported reclamation, wrapper propagation, and every caller group
that is proposed for implementation.

## 6. Forbidden changes at this stage

No production edits to `Common/src/Process.cpp`, callers, headers, build files,
allocator/container/queue/lifetime mechanisms, parser, scheduler, wire format,
GW batching, throughput code, NRNCS cache or SSID handling.

Do not approve the R2 `M = NULL` patch merely because the external harness turns
GREEN. Integration safety requires caller evidence first.

## 7. Review gates

Before any production edit:

1. complete the per-call semantic matrix and exact group boundaries;
2. specify caller failure behavior without changing unrelated semantics;
3. run test-only exhaustion and wrapper fixtures;
4. send the complete amended scope and evidence to GPT-6 Astra;
5. obtain a conditional GO for one caller group and its production files;
6. implement one group only, then run RED→GREEN and matched runtime regression;
7. repeat fresh-boot Alpine build/runtime gates before the next group.

Further Astra review is mandatory before production changes. If a caller needs a
If a caller needs a new API or broad ownership change, stop and create a separate SPEC instead of
expanding R3.

## 8. Astra-reviewed wrapper amendment design

GPT-6 Astra reviewed the actual five wrapper bodies and the production-linked
exhaustion fixture. Verdict: **GO — wrapper amendment design**, not production
implementation.

For the four `Message*&` wrappers, `_M` is output-only. The approved design is:

1. Create a separate working pointer initialised to `NULL`.
2. Call `Process::NewMessage()` into that working pointer.
3. Check allocation status and pointer before any builder/name-generation use.
4. Build exclusively through the working pointer.
5. On allocation failure: return `ERROR`, expose `_M == NULL`, do not call
   builders, name generation or cleanup on `_M`, and leave any incoming referent
   untouched.
6. On construction failure: mark only the successfully allocated working message,
   expose `_M == NULL`, and return `ERROR`.
7. On complete success: publish the working pointer to `_M` once and return `OK`.

Clearing/replacing `_M` must never delete, release or mark its previous referent.
`MarkToDelete()` is deferred cleanup and must apply only to the new working
message.

`NewConnectionLessRunMessage(..., Message* _M)` is a separate by-value output
contract defect. Amendment A may isolate its local allocation/failure behavior
while preserving its signature and successful-path command semantics. Signature
changes and caller migration require a separate amendment.

### Amendment A boundary

Permitted production files: only `Common/src/MessageBuilder.cpp`, limited to the
five named wrapper bodies. No `Process.cpp`, headers, callers, allocation,
counters, occupancy, queues, batching, command contents, successful construction
order, direct deletion or broad exception policy changes.

Before implementation, test-only fixtures must cover each wrapper with null and
live incoming output, allocation exhaustion, construction-stage failure where a
controlled fixture is available, successful construction, old-object liveness
and marking, and no downstream construction after allocation failure. Corrected
contract assertions are expected to be RED on the unchanged baseline. After A,
wrapper tests must be GREEN while the independent Process stale-output test stays
RED until the later Process amendment C.

Astra review is required again after the test gate and before implementing A.

## 9. Test-gate result before implementation A

The wrapper fixture now asserts the proposed output-only/publish-on-success
contract. It was compiled against the unchanged restored baseline and executed
three times:

```text
build_exit=0
repeat-0 exit=10 ... expected_contract=0
repeat-1 exit=10 ... expected_contract=0
repeat-2 exit=10 ... expected_contract=0
```

All five wrappers returned `ERROR`; the four reference outputs were not cleared,
the by-value Run output remained non-null, and every retained old object was
marked. The expected corrected-contract assertion is therefore RED on the
baseline, with no crash. This is the required pre-implementation gate; the
fixture does not authorise production edits.

Evidence: `wrapper-contract-repeat-0.log`, `wrapper-contract-repeat-1.log`,
`wrapper-contract-repeat-2.log`, and `wrapper-build-v2.log` in the external
characterization directory.

## 10. Post-implementation test results for A

After applying only the five wrapper-body changes in `Common/src/MessageBuilder.cpp`:

- Full CMake build on VM100 passed.
- Wrapper exhaustion contract fixture passed 3/3:

```text
repeat-0 exit=0 ... out0_null=1 out0_marked=0 out1_null=1 out1_marked=0 out2_null=1 out2_marked=0 out3_null=1 out3_marked=0 out4_null=0 out4_marked=0 count=30000 expected_contract=1
repeat-1 exit=0 ... expected_contract=1
repeat-2 exit=0 ... expected_contract=1
```

- Wrapper success fixture passed 3/3:

```text
repeat-0 exit=0 success_statuses=0,0,0,0,0 refs_ok=1 run_caller_unchanged=1 count=8 expected_contract=1
repeat-1 exit=0 ... expected_contract=1
repeat-2 exit=0 ... expected_contract=1
```

- The independent Process exhaustion fixture remains RED 3/3 with
  `nonnull_output_cleared=0`, proving A did not silently implement C.

The five wrappers now satisfy the reviewed output-isolation behavior in the
available allocation-exhaustion and success tests. Construction-stage failure
injection after successful allocation remains a required gate before claiming A
complete if a controlled production-linked mechanism can be provided.

Evidence files are preserved in
`/home/gandalf/workspace/ng-spec033-characterization-20260909/`.
Post-implementation Astra review is mandatory before B or C.

## 11. Construction-failure gate

A production-linked fixture was added at
`/home/gandalf/workspace/ng-spec033-characterization-20260909/wrapper_construction_failure.cpp`.
It keeps allocation available, forces the first construction builder to return
`ERROR` with empty routing vectors, and then uses public reclamation to verify
that only the newly allocated working message is reclaimed.

Compiled against the modified wrappers and executed three times. Every run
returned `expected_contract=1` and exit 0:

```text
case0_status=1 output_ok=1 old_ok=1 reclaimed=1
case1_status=1 output_ok=1 old_ok=1 reclaimed=1
case2_status=1 output_ok=1 old_ok=1 reclaimed=1
case3_status=1 output_ok=1 old_ok=1 reclaimed=1
case4_status=1 output_ok=1 old_ok=1 reclaimed=1
```

For cases 0–3 the reference output became NULL; for case 4 the by-value caller
output remained unchanged. All retained old referents remained unmarked. The
fixture therefore closes the previously open construction-failure gate for the
reachable first builder failure, while preserving the separate need for further
stage-specific injection if materially different failure paths are introduced.

## 12. Conditional acceptance of Amendment A

GPT-6 Astra reviewed the exact post-implementation diff and all reported build
and fixture results. Verdict: **ACCEPT A — conditional**.

Commit `8b4aeaf` contains only `Common/src/MessageBuilder.cpp` and the five
permitted function bodies. Full build, wrapper exhaustion, wrapper success and
first-stage construction-failure tests passed; the independent Process.cpp
exhaustion defect remains RED as intentionally deferred.

The later-stage fault-injection limitation is recorded and must not be claimed as
exhaustively covered. Amendments B (direct callers, starting with `GW.cpp:850`)
and C (Process first-overload output) remain blocked and require separate scopes,
tests and Astra review. No further Astra review is required before retaining or
merging this unchanged A commit; any material patch change requires renewed review.
