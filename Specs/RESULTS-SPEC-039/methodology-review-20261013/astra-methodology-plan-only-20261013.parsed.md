## A. Executive diagnosis

**Facts from the supplied records**

- NG-039/NG-040 have stalled at the **design and evidence-contract gate**, not through repeated failures of executed Gateway tests. D1-H was compile/link only; no D2 correctness run, semantic-finaliser execution or D3 measurement is reported. The dedicated UID/launcher/lease remains `NOT_PROVISIONED`.
- Several findings were genuinely closed. The bounded r07 documentary approval covered B1/B2/B3 only. Subsequent host-controlled lifecycle requirements introduced a different review surface.
- r09–r12 expose contradictions between permissions, phase prerequisites, historical resource state and reported outcomes. The supplied r12 counterexample demonstrates why individually schema-valid events do not establish a valid transaction sequence.
- The current request supplies post-freeze consent for this one plan-only consultation. Earlier frozen “consent=false” fields remain historical facts, not a current refusal.

**Inference**

The principal failure is **incremental correction of duplicated prose without a single, testable lifecycle model**. Each amendment addressed the latest finding but did not systematically check neighbouring transitions and failure paths. Small example-based fixture sets and inconsistent projections then allowed related defects to survive.

**Recommendation**

Use a small, finite combination: **same-SPEC corrections, one bounded methodology task, a focused existing process/skill update, and a read-only contract-checking utility**. A separate broad methodology SPEC or general verification platform is not justified now.

**Limitations:** this plan uses supplied text only. No tools, tests, hash recomputation or filesystem inspection were performed. Early-round evidence is partly task-note summary; reported local reproduction is not independently reproduced here.

## B. Round-by-round recurrence table

“Recurrence” below distinguishes a repeated defect from a new defect sharing an earlier root cause.

| Round | Classification and finding | Recurrence/disposition |
|---|---|---|
| r03 | Compiler-checker/acceptance concerns: concrete defects reported in the **task-note summary**. | Original response not supplied; individual findings cannot reliably be reconstructed. |
| r04 | **Summary:** some design findings closed; finding 4a remained open. | Explicit continuation of an unresolved finding, not evidence that all earlier findings recurred. |
| r05 | **Summary:** forwarded/function-like macro bypasses, `-ansi` override and optimisation-disabled link assertions. Optional target/link/schema hardening. | Concrete manifestations within the continuing checker gap. r06 explicitly records the three named must-fixes as closed at design level. |
| r06 | Distinct defects: missing trusted-root enforcement/exact `-c`; absent environment contract; link invocation/file-binding and forwarding gaps; insufficient status/evidence constraints. | Same broader provenance/validation weakness, but not simply repetition of the three closed r05 cases. Artifact uniqueness, glob rejection, fixture provenance and editorial matters were advisories. |
| r07 initial | Distinct invocation defect: unconsumed `--`; joined forced-include/macro-file bypass. | Invocation-binding and macro-bypass **families recur through different concrete cases**. Failure-phase vocabulary and bookkeeping were advisory. |
| r07 identity correction | Stale SPEC digest in manifest/schema: process/provenance error with concrete acceptance consequences. | Not a compiler defect or mere sender typo. |
| r07 closure | B1/B2/B3 documentary approval in the supplied derived response. | Did not approve D2, verify reported synthetic execution, implement the finaliser or establish runtime correctness. |
| r08 | B1: sender-envelope typo, withdrawn. B2/B3: distinct host-profile safety and evidence-lifecycle defects—continuous exclusion, unchecked unlink/TOCTOU, retained journal/raw inventory/mapping evidence. | New host-controlled scope; r07 checker closure was not reopened. Later records retain these safeguards at documentary level. |
| r09 | Distinct contradictory IPC boundary: finaliser forbidden to touch IPC yet assigned inspection/removal. | New responsibility contradiction; closed in r10. Mapping representation, fixture bookkeeping and fsync-evidence limits were advisory. |
| r10 | Distinct abnormal-path contradiction: final record required where unconditional joins/destruction prerequisites cannot hold. | Same incomplete lifecycle-model root cause, not recurrence of r09’s IPC wording defect. |
| r11 | B1: lease loss could select a mode requiring continuous lease. B2: prior cleanup could be labelled `not_started`. | Two distinct state/eligibility defects. r10’s join conflict was closed. Raw-preservation versus strict-bundle gating was advisory. |
| r12 | B1: creation intent consumed by removal outcome; phase ordering incomplete. B2: normal-failure clauses disagree over historically supported `complete`. | B1 is a new concrete classifier defect in the recurring truthful-history problem; B2 is projection drift plus failure-phase conflation. r11 lease conflict closed. No new r12 verdict is offered here. |

Sources: supplied r06–r12 responses; r03–r05 task-note summary; r12 classifier, fixture matrix and validation receipt.

## C. Ranked causes and evidence

1. **Incomplete phase/resource transition modelling — strongest evidence.**  
   r09–r11 conflate operational permission, normal shutdown and abnormal finalisation eligibility. r12 `classify()` uses one `pending` set for creation and cleanup. These require distinct states, not additional outcome labels alone.

2. **Multiple independently maintained policy projections.**  
   SPEC-039 §7.1 and manifest `semantic_finalizer_contract.normal_cleanup_failure` disagree over `complete`. Repeated boundary blocks and historical policy lists increase drift. Generating a manifest from the previous manifest preserves provenance, but does not establish semantic consistency.

3. **Fixture selection tests examples rather than transition obligations.**  
   The r12 receipt lists passing cases, but omits the supplied schema-valid orphan-removal sequence. Some negative fixtures violate several rules simultaneously, so rejection does not establish which intended rule worked.

4. **Validation claims exceed the demonstrated test coverage.**  
   “Malformed/reordered history rejected” is broader than a finite list of executed examples establishes. The records correctly label PID cross-artifact validation `NOT EXECUTED`; that precision should apply throughout. Schema meta-validation, semantic model tests and runtime enforcement are different evidence classes.

5. **Packaging and evidence-retention weaknesses add avoidable rounds.**  
   r07 stale digest, r08 envelope typo, historical unretained fixtures and unavailable original D1-H bytes show avoidable provenance friction. These do not explain the substantive lifecycle defects.

**Hypotheses, not established facts:** shared assumptions between author and fixture writer, review fatigue and packet size may contribute. The records support these as plausible risks, not demonstrated causes or reviewer misconduct.

## D. Minimal plan, tooling and acceptance criteria

All work below is proposed and subject to owner decisions.

### Phase 1 — Establish one bounded contract

**Mandatory deliverables**

- One canonical, versioned transition table under SPEC-039, with stable requirement IDs.
- A pure offline executable interpretation of that table—not another operational finaliser or independent policy authority.
- One traceability ledger: finding → requirement → transition → fixture → evidence class.

Model separately:

- common authority and file-safety eligibility;
- normal versus abnormal processing;
- per-resource creation and cleanup transactions;
- shutdown, cleanup, inventory verification and record closure;
- historical cleanup outcome versus overall run status.

**Acceptance criteria**

- Every declared state/event pair has an explicit allowed, forbidden or uncertainty disposition.
- Every allowed effect identifies prerequisites and evidence obligations.
- Creation and cleanup outcomes cannot satisfy one another’s pending transactions.
- Authority loss never becomes an alternative finalisation permission.
- Destructive-cleanup failure is distinguished from later evidence/finalisation failure; historically supported `complete` does not imply PASS or verified inventory.
- Invalid history yields `unknown` without blocking otherwise permitted safe raw preservation.

This would directly address the classes behind r09–r12; it cannot guarantee discovery of every future defect.

### Phase 2 — Test transitions and projections

**Mandatory minimum**

1. Retain all supplied counterexamples as named regressions.
2. Add deterministic, bounded model-based tests:
   - orphan, wrong-kind and duplicate outcomes;
   - creation after shutdown;
   - unresolved intent and truncation at each transaction boundary;
   - two-resource interleavings and identity mismatches;
   - authority loss at each effect boundary;
   - completed removals followed by evidence/final-record failure.
3. Keep event shape and sequence valid when testing semantic ordering, so an unrelated schema failure cannot mask the target defect.
4. Check cross-artifact PID/run identity, missing/unavailable sets and mode-specific evidence rules offline; clearly distinguish these tests from execution of the actual finaliser.
5. Generate repeated normative fragments from the canonical table where practical; otherwise compare their structured predicates automatically. Do not rely on prose-string presence.
6. Use assertion-independent checker failures and verify literal entry points under normal and optimised Python.

**Acceptance criteria**

- Every named requirement has a positive case where applicable and a targeted negative or uncertainty case.
- Every declared transition and forbidden transition class is covered within an explicit finite bound.
- The r12 orphan trace returns `unknown`.
- Deliberately reintroducing the shared-pending-state error or a conflicting outcome projection makes the suite fail.
- Receipts list exact inputs, tool versions, fixture IDs, actual counts and unexecuted obligations. No claim of exhaustive correctness.

**Necessary, not overengineering:** a small transition model, deterministic generation and projection checks.  
**Optional:** broader property-based fuzzing and mutation campaigns. A general model checker, compiler grammar or repository-wide verification framework is unnecessary for this closure.

### Phase 3 — Independent local audit and freeze

**Mandatory deliverables**

- One independent local audit using the transition table and counterexamples, not merely rereading the latest amendment.
- A freeze checklist covering SPEC/manifest/schema bindings, packet bytes, sender envelope, receipts and scope.
- A separate append-only consent record binding exact packet identity, purpose and permitted call count. Frozen pre-consent fields are not rewritten to simulate current consent.
- Preserved fixture and checker bytes, not hashes alone.

**Finite review budget:** one audit, one consolidated correction pass and one verification pass. If a substantive contradiction remains, stop and return the unresolved decision to the owner; do not begin another automatic amendment cycle.

**Exit criteria:** zero unresolved contradictions within the bounded contract, exact projection agreement, complete retained evidence for claimed checks, and explicit outstanding runtime obligations.

No further Astra consultation is part of this plan.

### Phase 4 — Separate documentary closure, D2 and D3

| Gate | Required result | Stop condition |
|---|---|---|
| Documentary | Contract and local audit criteria met; existing documentary gate remains in force. | Unresolved semantics, missing evidence or inconsistent identities. |
| D2 preparation | Separate permission for implementation/build; provisioned and verified launcher/lease; actual finaliser conformance and safety tests. | Missing provisioned authority, identity uncertainty or failed safety test. |
| D2 correctness | Separately authorised single controlled Q0/Q1 run; complete evidence-derived result. | FAIL, INCONCLUSIVE, timeout, ambiguity or authority loss: preserve permitted evidence; no automatic rerun or cleanup retry. |
| D3 measurement | D2 PASS plus separate measurement design and owner permission. | No D2 PASS or no dedicated measurement authority. |

Documentary completion is not implementation readiness; build success is not correctness; correctness is not performance.

### Packaging and transfer beyond Q0/Q1

**Recommended:** one finite methodology task delivering the model, fixtures, checker and concise existing process/skill guidance. Keep Gateway invariants in SPEC-039. Close the task when these artifacts and acceptance criteria are satisfied.

**Optional later:** a separate process SPEC only if multiple subsystems demonstrate a durable shared normative need; it should replace shared guidance, not duplicate it.

Transfer the **method**, not Gateway-specific rules:

- **Serialization:** representation/version states, lengths, truncation, round-trip and malformed-input fixtures.
- **SAR/HT:** subsystem-defined identity, ownership, correlation and ordering invariants.
- **Runtime/lifecycle:** launch-handle acquisition, termination proof and teardown outcomes; unknown launch state must not become successful teardown.

Each subsystem retains its own authority gates, oracle and resource constraints. An optional paper-only transfer example is sufficient initially; no suspended task needs reopening.

## E. Owner decisions and explicit non-authorities

**Decisions reserved to the owner**

1. Whether to accept the bounded methodology task and same-SPEC correction route.
2. Who owns the canonical contract and independent local audit.
3. Whether to authorise documentary/tooling work within the finite scope and review budget.
4. Whether and when to provision runtime prerequisites and separately authorise D2 preparation, one correctness run and later D3.
5. Whether optional hardening or cross-subsystem adoption is worth pursuing after minimum closure.

**Non-authorities**

This plan neither evaluates r12 for acceptance nor authorises edits, patches, builds, tests, harness/finaliser execution, IPC, cleanup, VM/network activity, commits, pushes or performance claims. It does not weaken existing safety constraints, request a follow-up consultation or authorise another call.