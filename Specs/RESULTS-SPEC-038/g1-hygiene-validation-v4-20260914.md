# G1 Hygiene Validation — v1.0 profile v4

**Date:** 2026-09-14
**Branch:** `AIOPT3`
**Tree checked:** `4974fcf82f66472e067785080ccd266cc0cba7e7`
**Functional candidate boundary:** `c7deb7d` (documentation-only descendants)
**Scope:** active v1.0 supported path only

## Scan boundary

The scan covered the active v1.0 path under `Scripts/AlpineVMs` and `Scripts/ProjectAudit`, excluding tests, plans, retained evidence/results and generated build trees, plus the active release master plan. It checked for host-private paths, RFC1918 addresses and literal MAC addresses. Guest-internal paths such as `/mnt` and documented operator-provided environment variables are not private host identities.

## Result

| Pattern | Files scanned | Matches |
|---|---:|---:|
| Host-private absolute path | 54 | 0 |
| RFC1918 address literal | 54 | 0 |
| Literal MAC address | 54 | 0 |

The active public-data scan for the supported v1.0 profile passes. The audit does not claim that historical records, retained trial evidence, legacy diagnostic scripts or generated/vendor material contain no identifiers; those are classified separately and are not active v1.0 deployment instructions.

## Dispositions

- `Scripts/AlpineVMs`: active supported deployment/controller path; no private host identity found by this scan.
- `Scripts/ProjectAudit`: active tooling uses `<obsidian-vault>` rather than a host-specific path.
- `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`: active commands use `<obsidian-vault>`.
- `Docs/DECISIONS/BASELINE-RESTORATION-2026-09-09.md`: explicitly labelled historical; guest recovery paths are retained as provenance, not instructions.
- `Specs/RESULTS-*`, matched trials and generated build material: retained evidence or generated artefacts; not used as active deployment instructions.
- Legacy `Scripts/Simple/*` examples and fixtures: outside the supported v1.0 profile; retained for historical/diagnostic work and tracked for v1.1+ classification.

## Remaining open criteria

- Historical files need final classification evidence.
- Normal runtime, legacy build profile and documented procedures need current-candidate reproducibility evidence.
- SPEC-054 still requires the bounded review of the v4 plan, auditor and first current report.

This report closes only the active public-data scan for the supported v1.0 profile. It does not close M0, SPEC-038 as a whole, M1 or M2.
