# Local runtime preflight — static build linkage blocker

**Date:** 2026-09-14
**Tree:** `8253ed8fcef6e16232d0f858afb1b5055c75606e`
**Plan:** `Scripts/AlpineVMs/plans/local-intra-os.example.json`
**Trial:** `m0-runtime-v4`

## Observed result

The canonical local NG-ELC invocation reached provenance capture and stopped fail-closed before launching the four roles:

```text
runtime_result=INCONCLUSIVE
teardown_result=UNKNOWN
evidence_result=INCOMPLETE
local_acceptance_eligible=false
acceptance_blockers=["build linkage missing"]
exit_code=20
```

No runtime process was launched by this attempt.

## Root cause

The current normal CMake executables are statically linked:

```text
file PGCS/NRNCS/ContentApp: ELF ... statically linked
ldd: not a dynamic executable
```

`Scripts/AlpineVMs/local_provenance.py::_validate_runtime_record()` currently requires `status == "ok"` and a successful `ldd` invocation. Therefore a truthful static-loader identity cannot satisfy the current receipt contract, and the controller correctly fails closed instead of accepting an invented dynamic-library record.

## Scope

This is a bounded M1/SPEC-056 compatibility gap between the supported build output and the provenance validator. It is not evidence of a NovaGenesis payload/runtime failure. A targeted amendment must define an explicit static-binary identity (for example, ELF/file/build-id plus the observed `ldd` static result) while preserving dynamic `ldd` validation for dynamically linked binaries. No VM trial should be started until that contract is resolved.
