## 1. VERDICT: AMEND

r07 substantially resolves the four r06 findings at documentary level: trusted-root containment and exact `-c` cardinality are now explicit; the minimal five-variable `env -i` contract is frozen; link validation reads the named file and rejects forwarding; and local schema constraints are complemented by a sufficiently explicit, fail-closed semantic-finalizer contract. That bounded division is proportionate and does not require a generic compiler/linker grammar, exhaustive environment denylisting, or duplicated cross-artifact enforcement. However, the frozen checker invocations still disagree with their argument handling, and the compile checker admits joined forced-include forms that can bypass observer-macro inspection. These prevent full closure while preserving the r05 protections. This conclusion is textual only: supplied identity strings are consistent where repeated, but hashes have not been independently recomputed; the sidecar receipt’s contents are not supplied. Reported synthetic checks and D1-H compile/link evidence establish neither D2 execution nor implemented finalizer behaviour.

## 2. Must-fix blockers

### B1. Both planned checker commands shift the positional arguments

**Locations:** Manifest `cmake.compile_commands_verification.planned_command` and `.checker_logic`; `cmake.link_command_verification.planned_command` and `.checker_logic`; SPEC §7 item 4.

Both commands use:

```text
/usr/bin/python3 -c <checker> -- <inputs...>
```

Both snippets nevertheless treat `sys.argv[1]` as their first input. With this invocation, that argument is the literal `--`, not `$DB` or `$LINK_TXT`. The remaining inputs are also shifted.

**Acceptance risk:** The frozen entry points do not validate the intended database or named link file. Function-level synthetic positives do not establish that these complete invocations are correct. This is a documentary invocation mismatch, not an observed execution failure.

**Required amendment:** Remove the separator following the `-c` programme, or explicitly consume and validate it before reading the inputs. Freeze a positive and negative invocation-level check for each exact command shape, including the trusted roots and expected diagnostic target.

### B2. Joined forced-include options can bypass observer-macro validation

**Locations:** Manifest `cmake.compile_commands_verification.checker_logic`, specifically the compiler-mode rejection condition and `parse_macro_switches`; SPEC §7 item 4’s requirement to reject forced-include forms that bypass the critical checks.

The rejection condition tests exact tokens `-include` and `-imacros`, but not their joined operand forms. An otherwise conforming diagnostic argument vector can therefore retain:

```text
-DNG_GW_QUEUE_TEST_OBSERVATION=1
```

and append, for example:

```text
-include/path/to/override.h
```

where the header undefines the observer macro. The joined token is neither rejected nor examined by `parse_macro_switches`; a `.h` operand also does not disturb the checker’s source-suffix test.

**Acceptance risk:** The checker can certify the required command-line definition while an uninspected forced include changes the macro state seen by the translation unit. This reopens an acceptance-critical observer bypass; it is not a request to validate unrelated compiler options.

**Required amendment:** Reject joined as well as separate forced-include and macro-file forms supported by the pinned compiler. Add targeted negative fixtures for those forms under normal Python and `PYTHONOPTIMIZE=1`. No generic argument grammar is necessary.

## 3. Optional advisories — non-blocking

- **Failure-phase vocabulary:** Manifest `semantic_finalizer_contract.assertion_to_phase` uses `bootstrap`, `post_inventory` and `final_inventory`, whereas `evidence_schemas.run_record.properties.failure` does not permit those values in `phase`. Align the mapping and enum before implementing finalization. The existing no-valid-final-record → INCONCLUSIVE rule prevents false acceptance, but this mismatch can unnecessarily prevent informative, schema-valid failure records.
- **Evidence and authority bookkeeping:** Preserve the distinction between packet-reported checker/schema probes and the unimplemented semantic finalizer. Record this review separately against the frozen identity; the packet’s “not submitted” and preparation-only statements are historical snapshot text, superseded for this single documentary review by the explicit authorisation in the request—not by operational permission.

## 4. Gate statement

**Documentary outcome: AMEND; closure is not yet achieved. D2 implementation/execution authority remains NO.** This review authorises no configure/build, harness or finalizer execution, IPC/namespace/VM/SSH activity, D3 measurement, commit or push.