# SPEC-048: Evidência de processamento receptor do hello PGCS

**Author:** Antonio Alberti / Hermes  
**Date:** 2026-09-12  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-047-pgcs-readiness-periodic-activation.md, SPEC-046-multi-vm-runner-safe-teardown.md, SPEC-013-ngal-adaptation-layer.md

## 1. Problema

A SPEC-047 demonstrou, no trial PGCS-only multi-VM, 36 frames `EtherType 0x1234` contendo `ng -hello --ihc 0.1`, com 18 frames em cada direcção e MACs correctos. Isto prova emissão e chegada ao bridge, mas não prova que o PGCS receptor:

- recebeu o frame no socket RAW;
- passou o frame ao reassembly NGAL-SAR;
- entregou a mensagem ao Gateway;
- executou o handler `PGHelloIHC01`;
- armazenou o binding do peer.

Os logs normais não têm markers suficientes porque o debug amplo está desactivado.

## 2. Objectivo

Obter evidência directa e correlacionada do processamento receptor de pelo menos um hello em cada direcção, mantendo separados os gates de emissão, ingress, reassembly, dispatch, handler e binding.

## 3. Âmbito

### Incluído

- PGCS-only nas VMs 101/102, exacto commit e configuração congelados.
- Observabilidade targeted em `NGAL_Transport_RAW`, `NGAL_SAR`, `GW` e `PGHelloIHC01`, ou observação externa equivalente sem alterar semântica.
- Correlação por PID, timestamp, MAC peer, EtherType e conteúdo/identidade do hello.
- Repetição bounded do trial já aceite no escopo de emissão/on-wire.
- Cleanup automático e evidência de zero processos/SHM/semafóros.

### Excluído

- Alteração do protocolo ou wire format.
- Alteração de routing, discovery, subscriptions ou ownership para facilitar o teste.
- Adição de NRNCS, ContentApp ou NBTestApp antes de fechar o gate receptor.
- Claims de correctness funcional da SPEC-044 ou de release `v1.0.0`.

## 4. Critérios de aceitação

- [ ] SPEC aprovada antes de qualquer alteração de instrumentação.
- [ ] Instrumentação/observação é targeted, reversível e não altera o comportamento normal.
- [ ] O trial demonstra ingress num socket PGCS receptor com PID identificado.
- [ ] O mesmo frame/hello é correlacionado com resultado de `NGAL_SAR` e entrega ao Gateway.
- [ ] `PGHelloIHC01` é observado a processar o hello, com peer correcto.
- [ ] O binding/estado resultante é demonstrado por um oráculo observável, ou a limitação é explicitamente registada.
- [ ] O resultado distingue emissão, bridge ingress, processamento receptor e binding.
- [ ] Os dois sentidos demonstram a cadeia completa: socket receptor → NGAL-SAR → Gateway → `PGHelloIHC01` → resultado do handler.
- [ ] Lacunas são reportadas explicitamente, mas não fecham o gate receptor.
- [ ] O binding/estado resultante é demonstrado por um oráculo observável, ou permanece explicitamente `unproven`.
- [ ] NRNCS permanece bloqueado até os dois sentidos passarem e qualquer pré-requisito de binding do trial seguinte ser verificado.
- [ ] Os logs, hashes, configuração, captura e manifesto são preservados.
- [ ] Cleanup deixa zero resíduos pertencentes ao trial nos dois guests: processos NovaGenesis, segmentos SHM e semáforos, comparados com o inventário inicial.
- [ ] Astra revê o escopo e o resultado antes da expansão para NRNCS.

## 5. Plano

1. Congelar commit, hashes e configuração; preservar o runner corrigido.
2. Escolher a observabilidade mínima: primeiro observar externamente; só adicionar debug targeted se necessário.
3. Se houver alteração de instrumentação, criar build/artefacto separado e obter revisão Astra antes de executar.
4. Repetir o PGCS-only trial com a mesma janela e MACs.
5. Correlacionar bridge capture, logs receptor, PID e estado de binding.
6. Limpar os guests e verificar o inventário final.
7. Só depois decidir se NRNCS pode ser introduzido no próximo ensaio.

## 6. Evidência predecessora

- SPEC-047 local: `Specs/RESULTS-SPEC-047/local-gate-assessment.md`.
- SPEC-047 multi-VM: `Specs/RESULTS-SPEC-047/network-gate-assessment.md`.
- Trial: `Specs/RESULTS-SPEC-047/network/trial/`.
- Resultado actual: 36 frames `0x1234`, 18 por direcção; receptor/application handling permanece OPEN.

## 7. Resultado da execução

No commit `1af604d`, o trial GDB externo observou em ambos os guests:

- 18 frames `EtherType 0x1234` em cada direcção;
- 18 entradas em `PGHelloIHC01::Run` no Repository e 18 no Source;
- 18 retornos `RECEIVER_HANDLER_STATUS_OK` em cada guest;
- cleanup com zero processos, SHM e semáforos; VMs paradas.

O gate de processamento receptor está fechado nos dois sentidos. O resultado não prova instalação/persistência do binding, readiness do NRNCS, subscriptions ou correctness end-to-end.

Evidência: `Specs/RESULTS-SPEC-048/receiver-gdb-assessment.md` e `Specs/RESULTS-SPEC-048/receiver-gdb-trial/`.

## 8. Relação com a primeira release

Esta SPEC fecha apenas o gate de processamento receptor PGCS. A release continua bloqueada por binding/NRNCS, subscriptions, SPEC-044/045, runner, hygiene e provenance.
