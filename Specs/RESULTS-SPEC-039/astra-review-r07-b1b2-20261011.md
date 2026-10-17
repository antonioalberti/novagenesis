1. **VERDICT: AMEND** — B1 and B2 are closed at documentary level, but the amended manifest still binds acceptance to a different SPEC digest from the authorised frozen SPEC.

2. **Must-fix blockers**
   - **Stale SPEC identity in the manifest and run-record schema.**  
     **Locations:** embedded `Specs/RESULTS-SPEC-039/precode-manifest-v1.10-r07.json`, `$.spec.sha256` and `$.evidence_schemas.run_record.properties.spec_sha256.const`; compare the packet’s **Frozen identity and privacy** section and embedded SPEC file header.  
     Both manifest fields retain `7129b0fac7788b0b7c8cf3d2f93bae1a4e05da6873b99c2cd7b3c33247b1ff2a`, whereas the authorised SPEC digest is `910cb308acccdc31f11a0667acf914aa0443e64d630a6cc938a16ce059407bbd`. Consequently, the schema rejects a run record identifying the reviewed amendment and permits the superseded identity instead. This is a concrete acceptance/provenance inconsistency, not additional compiler hardening. Align both identity fields with the governing frozen SPEC and refresh the affected frozen identities and receipt binding.

3. **Optional advisories — non-blocking**
   - **B1 closure:** SPEC §7 item 4 and both manifest checker `planned_command`/`checker_logic` pairs now map operands directly to the intended `sys.argv` positions, without `--`. Missing operands cause caught exceptions and exit 2; these checks do not depend on Python assertions.
   - **B2 closure:** the compile checker’s `startswith(("-include", "-imacros", ...))` rejection covers joined forms as well as the explicitly rejected split forms. The manifest reports the relevant negatives and entrypoint checks under normal Python and `PYTHONOPTIMIZE=1`.
   - Those synthetic outcomes remain supplied documentary claims, not independently reproduced evidence. The sidecar’s separate hash binding is acceptable in principle; its contents were not supplied for inspection.
   - The deferred `failure.phase` vocabulary observation remains non-blocking; no concrete false PASS attributable to it is established here.

4. **Gate statement:** Documentary outcome only: B1/B2 closed, but packet acceptance requires the identity correction above. No tools or code were executed, and no build, runtime, finalizer or performance behaviour was verified. **D2 implementation/execution authority remains NO.**