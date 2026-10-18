# SPEC-071 v0.13 design-contract validation receipt

Date: 2026-10-17
Scope: local validation of the unapproved SPEC-071 v0.13 draft and its machine-readable design contract. This receipt is not an implementation test, Astra approval, owner acceptance, or authorization to write the auditor.

Repository identity at validation: AIOPT5 HEAD `8d2386dcb10d2b37f133d4b24c2cb96761753d82`.
SPEC-071 SHA-256: `87439b9ba4007b6fd9a5404f96087d0d377b9eb0560ca511ff983274f7788c46`.
Report-schema SHA-256: `13c3259f59a7fed69b4da85761b9596d96455e49020f25ccfe8cdf26cfd78e25`.

Executed checks and observed results:

- `python3 Scripts/Python/generate_spec_impact_sidecars.py --write`: `WROTE/REFRESHED 1 sidecars; unchanged=64`; a subsequent inventory check confirmed 65 sidecars.
- `git diff --check`: passed (exit 0).
- Draft 2020-12 meta-validation: the report schema and installed Hermes `decision_query.schema.json` each passed schema meta-validation; this does not mean the handler query conforms to the strict profile schema.
- Synthetic instance validation: full report, input manifest, output manifest, handler-specific handoff wrapper, and progress ledger passed the report schema.
- The handler-specific query with one `claims` item passed the installed local `contracts.validate_query` and adapter `_bounded_request`; the separate installed `decision_query.schema.json` rejects that extension because it is closed and omits `claims`. This mismatch is now explicitly documented in SPEC-071; remote dispatch remains blocked pending owner resolution. A prior `question`-only fixture passed the strict schema but is not accepted by the active direct adapter, so it is not used as the v1 handoff.
- The local `typesafe_jev.build_choice_payload` serializer produced 1,069 bytes; inspection asserted that the repository path, `evidence_refs`, and local `candidate_reason` were absent. No provider request was sent.
- `assess_progress` received a synthetic valid ledger with no trust callbacks; it returned no percentage and downgraded the declared `met` gate to `unknown`.
- Synthetic semantic reconciliation passed for report counts and branch/SPEC uniqueness.
- Markdown table column counts (5 blocks) and trailing-whitespace checks passed.

Negative probes rejected: report exit/status mismatch; missing required branch; absolute handoff path; traversal path; oversized TypeSafe summary; included progress gate without owner-weight approval reference. Contract discrepancy reproduced: strict profile schema rejects the single-`claims` direct-handler query, while the handler validator/adapter accepts it; this is a blocking integration limitation, not a claimed pass.

Important evidence boundary: these are design/schema/consumer-contract fixtures only. The auditor CLI, implementation tests, generated reports, live JEV/TypeSafe call, Astra follow-up, runtime behavior, and acceptance criteria have not been implemented or validated. The initial validation wrapper had a formatting-only `TypeError` after its assertions while printing a byte count; the corrected final run completed with exit 0 and the results above. The wrapper summary's stale sidecar count was not used; the generator stdout and verified 65-file count are authoritative.

No source code, auditor tests, commits, pushes, runtime state changes, Astra follow-up, or provider dispatch occurred. Only design/review documentation, schema, evidence artifacts, impact sidecar, task note, and Dashboard records changed.