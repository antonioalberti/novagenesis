# SPEC-049: Readiness NRNCS e oráculos de binding PGCS

**Author:** Antonio Alberti / Hermes  
**Date:** 2026-09-12  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-047-pgcs-readiness-periodic-activation.md, SPEC-048-pgcs-receiver-processing-evidence.md, SPEC-045-core-evaluate-functional-harness.md

## 1. Objectivo

Introduzir o NRNCS como próxima camada experimental, demonstrando separadamente:

1. readiness operacional do NRNCS;
2. descoberta e binding PGCS↔NRNCS;
3. persistência/observabilidade dos bindings esperados;
4. troca de mensagens PGCS↔NRNCS sem ainda introduzir ContentApp ou workload de fotos.

## 2. Pré-condições

- SPEC-047 fechada no escopo PGCS-only/on-wire/teardown.
- SPEC-048 fechada no gate de processamento receptor nos dois sentidos.
- Commit, binários e configuração congelados e verificados nos dois guests.
- Nenhuma alteração de produção entre os ensaios.

## 3. Âmbito

### Incluído

- PGCS nas duas VMs, depois NRNCS na Source VM, na ordem documentada.
- Readiness gates de processo, GW, SHM, PGCS peer handling, NRNCS bootstrap e binding.
- Oráculos explícitos para os bindings esperados, incluindo identidade, categoria, chave, valor e tempo limite.
- Captura `0x1234`, logs PGCS/NRNCS, supervisor bounded e cleanup.
- Teste de uma mensagem de controlo mínima, sem ContentApp.

### Excluído

- ContentApp, NBTestApp e publicação de fotos.
- Claims de subscriptions funcionais ou entrega de payload.
- Alterações de routing, wire format, ownership ou cache.
- Alterações de produção para criar observabilidade sem SPEC própria.

## 4. Oráculos derivados do código

| Gate | Oráculo concreto | Fonte |
|---|---|---|
| NRNCS readiness | Marker literal `(--------------------------------------------------------------------------OPERATIONAL: Everything ok!)` após `PB->State = "operational"` | `NRNCS/src/NRRunInitialization01.cpp:303–307` |
| NRNCS timing | Configuração efectiva: `DelayBeforeRunInitiatilization=3`, `DelayBeforeGIRSDiscovery=30`, `DelayBeforePIDPublishing=20`, `DelayBeforeRunExposition=5`, `DelayBeforeRunPeriodic=5` | `IO/NRNCS/NRNCS.ini`, `NRRunInitialization01.cpp:169–240` |
| Publication ingress | `NRPubBind01::Run` recebe `-p --b 0.1` com exactamente 3 argumentos: categoria, chave e valores | `NRNCS/src/NRPubBind01.cpp:56–95` |
| Binding storage | `HTStoreBind01::Run` lê categoria/chave/valores e chama `HT::StoreBinding(Category, Key, &PArguments)` | `Common/src/HTStoreBind01.cpp:47–114` |
| Binding result | Capturar e preservar categoria, chave, valores, bloco HT e retorno da operação; comparar com o mesmo `-p --b` que entrou no NRNCS | `NRPubBind01.cpp`, `HTStoreBind01.cpp` |
| PGCS publication trigger | PGCS publica uma vez quando `AlreadyPublishedBasicBindings == false` e `AwareOfAPS == true`, através de `-run --publishing 0.1` | `PGCS/src/PGRunPeriodic01.cpp:515–582` |

A observação GDB deve capturar `NRPubBind01::Run` e `HTStoreBind01::Run` no Source. A entrada e o armazenamento devem ser correlacionados por processo, timestamp e categoria/chave/valores; o marker de publicação sozinho não é suficiente.

## 5. Critérios de aceitação

- [ ] SPEC aprovada e revisão Astra concluída antes do trial.
- [ ] PGCS peer readiness reproduzida nos dois sentidos.
- [ ] NRNCS inicia e alcança marker de readiness definido, não apenas processo vivo.
- [ ] Binding PGCS→NRNCS esperado aparece no oráculo definido, com chave/valor/identidade correctos.
- [ ] Binding inverso ou resposta correspondente é demonstrado quando aplicável.
- [ ] Tempo de aparecimento e timeout são bounded e preservados.
- [ ] Mensagem de controlo mínima é recebida pelo NRNCS correcto, sem atribuição ambígua.
- [ ] Não há ContentApp nem workload de fotos neste gate.
- [ ] Cleanup deixa zero resíduos pertencentes ao trial nos dois guests.
- [ ] Astra aceita resultados antes da introdução de ContentApp.

## 6. Plano

1. Definir os bindings esperados a partir das APIs/configuração reais.
2. Definir os markers de readiness e os oráculos observáveis antes de executar.
3. Reiniciar limpo as VMs e validar proveniência.
4. Lançar PGCS peers e aguardar a cadeia já demonstrada.
5. Lançar NRNCS na Source e aguardar readiness.
6. Observar bindings e executar uma única mensagem de controlo bounded.
7. Recolher evidência, limpar processos/IPC e parar guests.
8. Rever com Astra; manter ContentApp bloqueado se qualquer binding ou readiness permanecer aberto.

## 8. Resultado da execução

No commit `1af604d`, com PGCS nas duas VMs e NRNCS apenas na Source VM:

- o marker `OPERATIONAL: Everything ok!` foi observado;
- `NRPUB_INGRESS` recebeu categoria `2`, chave `18A4AAFB`, valor `00B50E23`;
- `HT_STORE_CALL` recebeu os mesmos argumentos e invocou o caminho de armazenamento;
- 66 frames `0x1234` foram capturados no bridge;
- cleanup deixou zero processos, SHM e semáforos; VMs paradas.

Fechado no escopo: readiness NRNCS e correspondência publicação→invocação de storage. Aberto: lookup/readback independente, persistência, subscriptions, payload e correctness end-to-end.

Evidência: `Specs/RESULTS-SPEC-049/nrncs-binding-assessment.md` e `Specs/RESULTS-SPEC-049/nrncs-binding-trial/`.
