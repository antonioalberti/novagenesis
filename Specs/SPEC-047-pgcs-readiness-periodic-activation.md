# SPEC-047: Diagnóstico de readiness e activação periódica do PGCS

**Author:** Antonio Alberti / Hermes  
**Date:** 2026-09-12  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-044-core-run-evaluate-invalid-pg-downcast.md, SPEC-045-core-evaluate-functional-harness.md, SPEC-046-multi-vm-runner-safe-teardown.md, SPEC-029-pgcs-self-mac-failfast.md

## 1. Problema e evidência

Os diagnósticos supervisionados da SPEC-046 demonstraram que os PGCS das VMs 101/102:

- iniciam e identificam correctamente a interface, MAC local e MAC peer;
- criam o socket cliente e o socket servidor;
- iniciam o `NGAL ReceiveDispatcher`;
- criam o segmento de memória partilhada de entrada;
- não produzem, nos artefactos preservados, marcadores de `RunPeriodic`, `HelloScheduling`, envio de hello ou discovery peer.

A configuração carregada regista `DelayBeforeRunPeriodic = 5`, `DelayBetweenHellos01 = 2`, `DelayBetweenHellos02 = 4` e `DelayBeforeFirstPeriodic = 120`. Mesmo o diagnóstico estendido não demonstrou a activação observável do ciclo periódico.

A evidência actual não permite atribuir a ausência de discovery ao transporte RAW. O primeiro edge não demonstrado está entre a inicialização do PG/GW, a criação/execução do primeiro `RunPeriodic` e a observabilidade dos hellos.

## 2. Objectivo

Localizar, com um experimento bounded e reproduzível, o primeiro ponto em que a inicialização/execução periódica do PGCS deixa de progredir, sem alterar o comportamento de produção ou inferir uma falha de transporte a partir de logs incompletos.

## 3. Âmbito

### Incluído

- Sequência real de `PGRunInitialization01`, `GW::Gateway`, `CoreRunInitialize01`/`PGRunPeriodic01` e `HelloScheduling`.
- Readiness gates distintos para processo, sockets, GW operacional, SHM operacional, primeiro `RunPeriodic`, primeiro hello emitido e primeiro hello recebido.
- Instrumentação exclusivamente test-only ou logging temporário explicitamente allowlisted.
- Execução local equivalente na VM 100 antes de novo trial multi-VM.
- Runner bounded, identidade de processo, logs completos e cleanup verificado.
- Comparação entre build normal e, se necessário, Sanitizer.

### Excluído

- Correcção de produção antes de localizar a causa e aprovar uma SPEC própria.
- Alterações no wire format, NGAL-SAR, transporte RAW, discovery, subscriptions ou inverted pub/sub sem evidência causal.
- Alteração de `DelayBeforeFirstPeriodic` apenas para fazer o teste passar.
- Claims de correctness funcional, performance ou release a partir de um smoke de startup.

## 4. Hipóteses a distinguir

1. **H1 — GW/SHM:** o PGCS não alcança o loop de Gateway após criar a SHM.
2. **H2 — agendamento:** o primeiro `RunPeriodic` é criado, mas não é executado ou não é reencaminhado.
3. **H3 — configuração/owner:** o PG lê parâmetros diferentes dos esperados ou existe um problema de ownership/estado antes de `HelloScheduling`.
4. **H4 — hello:** o ciclo periódico executa, mas `HelloScheduling` não agenda/emite o hello.
5. **H5 — transporte/recepção:** o hello é emitido, mas não chega ou não é processado pelo peer.

A experiência deve distinguir H1–H5; não deve escolher H5 por ausência de markers de alto nível.

## 5. Critérios de aceitação

- [ ] SPEC revista e aprovada antes de qualquer alteração de código.
- [ ] A sequência de inicialização e os pontos de observação estão mapeados para ficheiro/linha.
- [ ] O trial local bounded identifica o primeiro gate PASS e o primeiro gate OPEN/FAIL.
- [ ] Cada gate regista timestamp, PID, commit, binário, configuração e identidade da VM/processo.
- [ ] O resultado separa processo vivo, GW/SHM readiness, periodic activation, hello emitido, hello recebido e cleanup.
- [ ] Nenhuma conclusão de transporte é feita sem demonstrar primeiro emissão do hello.
- [ ] Cleanup deixa 0 processos NG, 0 segmentos SHM e 0 arrays de semáforos.
- [ ] O resultado bruto e o manifesto são preservados em `Specs/RESULTS-SPEC-047/`.
- [ ] Qualquer correcção necessária é aberta numa SPEC de implementação separada; esta SPEC não autoriza alteração de produção.
- [ ] Astra revê o diagnóstico antes de integrar o resultado no caminho das SPECs 045/046.

## 6. Plano experimental

1. Congelar a árvore de trabalho sem misturar as alterações de performance existentes.
2. Preservar o estado actual e confirmar os artefactos do diagnóstico SPEC-046.
3. Executar o cenário local equivalente na VM 100, com configuração mínima e duração bounded.
4. Adicionar apenas observabilidade allowlisted, se os markers existentes não distinguirem os gates.
5. Repetir em build normal; usar Sanitizer apenas como gate separado.
6. Reexecutar o primeiro cenário local que produza uma hipótese causal clara.
7. Só depois repetir nos guests 101/102 com o supervisor remoto.
8. Actualizar SPEC-046/045 com o primeiro edge demonstrado e manter OPEN tudo o que continuar sem oráculo.

## 7. Rollback e segurança

Este draft não autoriza alterações de produção. Qualquer instrumentação deve ser reversível, isolada e commitada separadamente. Nenhum processo deve ser terminado por identidade não verificada; IPC só pode ser removido depois de os processos correspondentes estarem ausentes.

## 8. Estado e evidência inicial

- Branch/remote: `AIOPT3`, ambos em `1af604d` no início da análise.
- Evidência: `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md` e logs `vm101-long.log`/`vm102-long.log`.
- Os logs terminam após a criação da SHM de entrada, antes de qualquer marker preservado de periodic/hello.
- A árvore de trabalho contém alterações não commitadas de performance e artefactos de SPEC-039–043; estes devem ser classificados antes de qualquer freeze de release.

## 9. Resultado da execução

O trial local `PGCS -lc` demonstrou GW/SHM e periodicidade no build AIOPT3 `1af604d`. O trial PGCS-only multi-VM seguinte demonstrou sockets, hellos bidireccionais e tráfego `EtherType 0x1234` com MACs correctos: 36 frames, 18 por direcção. Ambos os guests foram reconstruídos a partir do mesmo commit e terminaram com processos e IPC limpos.

A causa da observação anterior sem markers de hello permanece **não reproduzida e não resolvida**; não deve ser chamada de buffering sem prova. O processamento do frame pelo PGCS receptor continua OPEN porque a captura de bridge não demonstra parsing, dispatch ou armazenamento de bindings no processo.

Evidência:

- `Specs/RESULTS-SPEC-047/local-gate-assessment.md`
- `Specs/RESULTS-SPEC-047/network-gate-assessment.md`
- `Specs/RESULTS-SPEC-047/network/trial/`
- Revisões Astra: preflight, resultado local e resultado multi-VM registadas no tracker Codex.

Estado de release: esta SPEC não fecha SPEC-044/045 nem autoriza `v1.0.0`.

## 10. Relação com a primeira release

SPEC-047 é um diagnóstico prerequisite para tornar o runner multi-VM e o harness funcional interpretáveis. Não é, por si só, prova de readiness nem autorização para criar `v1.0.0`. A release continua bloqueada enquanto SPEC-044/045, os gates necessários da SPEC-046, revisão Astra e higiene/provenance não estiverem fechados.
