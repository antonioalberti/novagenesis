# SPEC-050: Integração control-plane com Repository e readback de bindings

**Author:** Antonio Alberti / Hermes  
**Date:** 2026-09-12  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-049-nrncs-readiness-binding-oracles.md, SPEC-045-core-evaluate-functional-harness.md, SPEC-044-core-run-evaluate-invalid-pg-downcast.md

## 1. Objectivo

Introduzir apenas um `ContentApp` no papel Repository depois dos gates PGCS e NRNCS, para demonstrar o caminho de control-plane e obter um readback independente de bindings. Não executar Source ContentApp, publicação de fotos ou workload de payload nesta fase.

## 2. Pré-condições

- SPEC-047: PGCS on-wire e teardown aceites no escopo declarado.
- SPEC-048: processamento receptor PGCS fechado nos dois sentidos.
- SPEC-049: NRNCS readiness e publicação→invocação de storage demonstradas.
- Commit/binários/configuração congelados; sem alterações de produção.

## 3. Âmbito

### Incluído

- PGCS nas VMs 101/102 e NRNCS na Source VM.
- Um `ContentApp Repository` na VM101.
- Readiness do ContentApp, descoberta do NRNCS e leitura de bindings/control-plane.
- Um único caminho de consulta/subscription suportado pelo `App.ini`, sem fotos.
- GDB/observers apenas se os markers públicos não demonstrarem o readback.
- Captura, logs, manifests, cleanup e hashes.

### Excluído

- ContentApp Source.
- JPEGs, `NRInfoPayload01`, cache de payload e entrega de ficheiros.
- Claims de performance, subscriptions completas ou correctness da SPEC-044.
- Alteração de APIs, routing, wire format ou ownership.

## 4. Oráculo derivado do código

| Gate | Oráculo concreto | Fonte |
|---|---|---|
| Repository configuration | `IO/Repository1/App.ini`: `DelayBeforePublishingServiceOffer=30`, `DelayBeforeDiscovery=3`, `DelayBeforeRunPeriodic=10`, `DelayBeforeANewPeerEvaluation=5`, `DelayBeforeANewPhotoPublish=1`, `ContentBurstSize=25` | `IO/Repository1/App.ini` |
| NRNCS discovery | ContentApp queries `DiscoverHomonymsEntitiesIDsFromLN(2, "NRNCS", ...)` and then `DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("NRNCS", "NR", ...)` | `ContentApp/src/CoreRunPeriodic01.cpp:162–215` |
| Repository readback | Marker literal `Discovered a NRNCS! HID=... OSID=... PID=... BID=...` when a new tuple is inserted into `Core::PSTuples` | `ContentApp/src/CoreRunPeriodic01.cpp:223–252` |
| Readback identity | Capture HID, OSID, PID and BID from the marker and correlate to the NRNCS/PGCS identities from the same trial; absence, mismatch or duplicate ambiguity fails the readback gate | `CoreRunPeriodic01.cpp:233–252` |
| No payload | Do not start Source ContentApp; verify no JPEG publication or payload-cache artefact is created | launch scope and `IO` inventory |

The marker is a retrieval/readback oracle because it follows the Repository's real discovery lookup and records the tuple in `PSTuples`; process liveness, request emission or NRNCS storage invocation alone do not satisfy it.

## 5. Critérios de aceitação

- [ ] SPEC aprovada e revisão Astra concluída.
- [ ] PGCS peer readiness e NRNCS operational marker observados antes de iniciar ContentApp.
- [ ] ContentApp Repository inicia com o `App.ini` exacto e alcança marker de readiness.
- [ ] O Repository observa/consulta o NRNCS esperado através do caminho real.
- [ ] Pelo menos um binding/control-plane objecto é lido de volta com identidade, categoria, chave e valor correlacionados.
- [ ] O resultado distingue publicação/storage invocation de readback bem-sucedido.
- [ ] Não são publicados ficheiros nem iniciado ContentApp Source.
- [ ] Cleanup deixa zero resíduos pertencentes ao trial.
- [ ] Astra revê o resultado antes de introduzir Source ContentApp.

## 6. Plano

1. Definir os markers e o objecto exacto de readback a partir do código/App.ini.
2. Reiniciar as VMs e reproduzir PGCS/NRNCS com os gates anteriores.
3. Iniciar Repository apenas após readiness observável.
4. Executar um único caminho de consulta control-plane bounded.
5. Recolher logs, GDB/captura se necessário e comparar identidade/categoria/chave/valor.
6. Limpar e parar VMs.
7. Manter Source ContentApp bloqueado se readback ou readiness não forem demonstrados.

## 7. Resultado da execução

O trial discovery-only demonstrou:

- PGCS peer readiness;
- NRNCS operacional na Source;
- descoberta local do NRNCS pelo Source PGCS;
- Repository operacional e descoberta do PGCS peer.

Não demonstrou o gate principal: Repository remoto não descobriu o NRNCS. O log preservou `NRNCS is still unknown`, `GetHTBinding function returned warning status` e não apresentou o marker `Discovered a NRNCS!`. Cleanup e paragem das VMs passaram.

Evidência: `Specs/RESULTS-SPEC-050/repository-assessment.md` e `Specs/RESULTS-SPEC-050/repository-trial/`.

## 8. Limites explícitos

Este gate não prova entrega de payload, persistência de ficheiros, subscriptions funcionais completas, correctness da SPEC-044 ou readiness da release `v1.0.0`.
