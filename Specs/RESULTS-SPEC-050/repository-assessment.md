# SPEC-050 — Resultado discovery-only Repository

**Commit:** `1af604d`  
**Topology:** PGCS VM101/VM102; NRNCS VM102; Repository VM101; sem Source ContentApp

## Resultados

| Gate | Estado | Evidência |
|---|---|---|
| Proveniência/configuração | PASS | Preflights exactos; App.ini preservado |
| PGCS peer | PASS | VM102 regista peer PGCS VM101 |
| NRNCS readiness | PASS | Source NRNCS em `OPERATIONAL: Everything ok!` |
| Source PGCS → NRNCS | PASS limitado | Source regista `Discovered the peer service = NRNCS via shared memory` |
| Repository readiness | PASS | Core/GW operacionais; App.ini carregado |
| Repository → NRNCS discovery/readback | OPEN/FAIL | Repository regista `NRNCS unknown` e `GetHTBinding warning`; não surgiu marker `Discovered a NRNCS!` |
| Payload/files | NOT TESTED | Source ContentApp não foi iniciado |
| Cleanup | PASS | Zero processos, SHM e semáforos; VMs paradas |

## Interpretação

O topology gate demonstra NRNCS operacional e descoberta local pelo PGCS Source, mas não demonstra exposição/propagação suficiente para o Repository remoto. O Repository descobriu o PGCS peer, mas não o NRNCS. A causa exacta continua aberta entre exposição no PGCS Source, propagação através do NRNCS/PGCS, armazenamento remoto ou lookup no Repository.

Este resultado não autoriza Source ContentApp nem claims de readback/payload. Próxima etapa: SPEC-051 para localizar o primeiro edge de descoberta NRNCS cross-VM.
