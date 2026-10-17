## AMEND — advisory pre-code review only

**Packet reviewed:** `cb3cfa8cee77ba51b74070d49872d4946eac5123ddbca74506572dd3a3acd711`

The supplied text addresses the three specifically summarized r05 defects, but the frozen checker/manifest contract still has material gaps. **This exact packet is not ready for documentary closure.**

### r05 must-fix disposition

| Summarized r05 issue | Disposition from textual inspection |
|---|---|
| Forwarded/function-like observer macros | **Closed for the identified spellings at design level.** The compile checker rejects `-Wp*` and `-Xpreprocessor`, extracts function-like macro base names, and requires exactly one diagnostic object-like `=1` definition. |
| `-ansi` dialect override | **Closed at design level.** The supported grammar rejects `-ansi` and requires exactly one `-std=gnu++20`. |
| Python `assert` disabled by optimization | **Closed at design level.** The supplied link checker uses explicit exceptions/nonzero termination, not `assert`. |

These dispositions do **not** establish that the full build-verification contract is complete or implemented.

## Must-fix findings

### 1. Compile-checker path containment is specified but not enforced

**Location:** SPEC §7 item 4; `cmake.compile_commands_verification.checker_logic`.

The SPEC requires directories and outputs under the selected fresh build root and sources under the pinned worktree. The checker receives only the database and profile; it has no trusted root inputs and performs no containment checks.

**Concrete bypass:** In an otherwise accepted diagnostic database, use an external directory and external source, with an output such as:

`/outside/build/CMakeFiles/Common.dir/foreign.cpp.o`

Make `file` agree with the source operand and `output` agree with `-o`; retain the required flags and a corresponding harness entry. The checker can accept the expected target census despite neither source nor output belonging to the frozen roots.

**Required amendment:** Bind trusted build/worktree roots to validation and enforce normalized containment and nonempty metadata. Also enforce the promised exactly-one `-c`: currently repeated `-c` tokens are accepted. The output-derived target extraction is an improvement, but is not sufficient provenance validation.

### 2. The required environment contract is absent

**Location:** SPEC §7 item 4 references `environment_contract.unset`; the embedded manifest contains no such field.

Consequently, the promised pre-configure/build/checker environment sanitization is not frozen. Compiler-affecting inherited variables such as `CPATH`, `CPLUS_INCLUDE_PATH`, `COMPILER_PATH`, and `LIBRARY_PATH` are not necessarily visible in the inspected argument arrays.

**Required amendment:** Supply the exact environment contract, including removal rules and retained-value recording. Resolve how it applies consistently to configure, build, and checker invocations. Do not describe that contract as complete while its referenced field is missing.

### 3. Link validation has both an invocation mismatch and remaining override gaps

**Location:** `cmake.link_command_verification.planned_command` and `.checker_logic`.

- The planned invocation passes `link.txt` as a positional argument, but the checker reads **standard input** and never reads that argument. As frozen, it does not reliably inspect the named file; empty stdin rejects, while other stdin could validate unrelated text.
- The denylist misses forwarded controls. For example, an otherwise accepted token list with **`-Wl,-shared`** passes the checker: its `-Wl` branch does not reject `-shared`. Similarly, **`-Xlinker -B -Xlinker dynamic`** is not recognized as a dynamic-mode override.

These are textual checker counterexamples, not claims that a particular resulting link succeeds.

**Required amendment:** Bind the invocation to the actual named `link.txt`, and define a supported link-argument grammar or complete forwarding normalization with fail-closed handling of unsupported controls. Include these cases in the frozen fixture matrix. Assertion-independent validation fixes r05’s optimization defect, but does not by itself establish static-link correctness.

### 4. Schema-valid status is still insufficiently bound to observations

**Location:** `evidence_schemas.run_record.allOf`, especially the PASS branch, and the finalizer validation contract.

The new conditions substantially improve status consistency, but still admit contradictory or unavailable evidence.

**Concrete schema-level counterexamples:**

- An otherwise conforming PASS record can set Q0/Q1 measurement fields to `null`, including wait/decision identities and sequences, while retaining `correlation_status="MATCHED"` and assertion labels `PASS`.
- It can report `target_pop_count=1` or `actual_popped=true` while `target_not_popped="PASS"`.
- A schema-valid FAIL can report `failure.cleanup_outcome="unknown"` or missing required artifacts in `failure.missing_artifacts`, despite the other fields claiming complete evidence and cleanup.

The prose disallows these outcomes, but the schema does not establish them. No complete cross-artifact semantic validator is supplied.

**Required amendment:** Encode straightforward non-null/value/status implications in the schema and explicitly freeze the remaining finalizer semantic checks: sequence/identity correlation, observation-to-assertion consistency, inventory-content/hash binding, and failure-summary consistency. Contradictory or unavailable evidence must produce **INCONCLUSIVE**, not PASS or bounded FAIL.

## Optional findings

- Require artifact-name uniqueness, not merely `uniqueItems` on complete artifact objects. Different hashes for the same filename currently permit duplicate-name entries.
- Reject command-string glob syntax explicitly as promised by the SPEC. The current command check does not reject `*`, `?`, or bracket patterns.
- Reconcile synthetic-test provenance: several fields describe historical, unretained v1.8 fixtures or “not executed in r06,” while others claim this-freeze r06 validation. Distinguish historical reports from fresh checks and retained evidence.
- Correct the unterminated Markdown backtick around the manifest digest; this is editorial only.

## Evidence limits and retained design closures

- This was **textual inspection of the supplied packet only**. I did not execute checkers, validate schemas with a tool, recompute hashes, inspect Git objects, or access archived responses.
- The r05 response is available here only through its supplied non-verbatim summary and asserted archive digest.
- Synthetic outcomes are packet-reported claims, not independently reproduced results.
- D1-H remains a historical compile/link claim with unavailable binary/log rehash evidence—not runtime or correctness evidence.
- **Findings 3 and 4b–4d remain design-level closures only**, as summarized in this packet. Nothing here upgrades observer behavior, ownership safety, cleanup, isolation, or finalization into implemented or runtime-verified properties.

**Authority explicitly withheld:** This AMEND grants no implementation, file-edit, configure/build, execution, IPC/VM, D2 acceptance, D3, performance, or operational authority. No retry or additional review call is authorized by this response.
