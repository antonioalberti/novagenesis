# G2/E7 canonical five-role trial — 2026-09-14

**Candidate commit:** `dfb95c9c1555549f19b36bf7eb42f83a16765ac9`
**Plan:** `Scripts/AlpineVMs/plans/L5-e7-marker.example.json`
**Trial:** `e7-l5-direct-dfb95c9`
**Bundle:** `Specs/RESULTS-SPEC-054/g2-evidence-20260913/e7-l5-direct-dfb95c9/`

## Verdict

```text
L5 preflight/provenance       PASS
PGCS Source/Repository       PASS
NRNCS operational             PASS
Repository/Source readiness   PASS
Source publication marker     PASS
Repository reception marker   PASS
Teardown                      PASS
Evidence bundle               COMPLETE
Trial exit                    0
Payload integrity/G9          OPEN
```

The NG-ELC trial used five roles: `PGCS-Source`, `PGCS-Repository`, `NRNCS-Source`, `Repository` and `Source`. The controller observed publication markers for the five JPEGs and reception markers in the Repository log. It also sealed the bundle and completed teardown. VMs 101/102 were stopped after the trial.

This closes the operational E7 claim for G2. It does **not** close G9: the trial used marker oracles and a five-photo workload, not the 100-photo independent Source↔NRNCS↔Repository SHA-256 acceptance oracle.

## Important limitation

The controller stopped after the declared publication/reception marker requirements were satisfied. Marker evidence proves the declared operational claim only; it is not a file-count or byte-integrity oracle.
