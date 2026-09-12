# SPEC-045: Harness funcional para validação de CoreRunEvaluate01

**Author:** Antonio Alberti / Hermes
**Date:** 2026-09-12
**Status:** Proposal
**Branch:** AIOPT3
**Implementation commit:** —
**Related:** SPEC-044-core-run-evaluate-invalid-pg-downcast.md, SPEC-041-gw-deadline-aware-wait.md

## 1. Problema

A SPEC-044 removeu um downcast UBSan inválido e passou os smoke tests de build e arranque. Contudo, o repositório não possui testes `ctest` registados nem um harness que consiga observar deterministicamente as branches funcionais de `CoreRunEvaluate01::CheckSubscriptions()`.

Sem esse harness não é possível demonstrar, com o método científico, a preservação dos comportamentos de publishers conhecidos/desconhecidos, PID overlap, BID não-NULL, lookup inválido, deduplicação e deleção.

## 2. Objectivo

Criar um mecanismo de teste reproduzível que exercite o caminho real de avaliação e produza oráculos observáveis para a SPEC-044, sem transformar estado interno privado em API pública apenas para facilitar o teste.

## 3. Estratégia e alternativas

### Opção preferida: E2E pelo protocolo real

- Iniciar o `PGCS` com `PG` e `Core` reais.
- Injectar mensagens serializadas através do Gateway/IPC usando o protocolo suportado.
- Criar subscriptions e publishers através das command lines reais.
- Observar respostas, logs estruturados já existentes, estado de processo e efeitos persistentes permitidos pelo protocolo.
- Usar subprocessos isolados, timeout bounded, cleanup por identidade e manifests de configuração/hash.

### Opção secundária: seam test-only mínimo

Só pode ser considerada se a opção E2E não conseguir observar uma condição necessária. O seam deve ser:

- compilado apenas para o alvo de teste;
- explicitamente separado de headers/APIs de produção, se possível;
- aprovado por SPEC e revisão Astra próprias;
- incapaz de alterar wire format, ownership, lifecycle ou comportamento normal;
- removível sem alterar a arquitectura runtime.

Explicitamente proibido:

- `#define private public`;
- friends adicionados apenas para contornar observabilidade;
- alterar produção para emitir um resultado artificial;
- validar por contadores agregados sem identidade do objecto/evento;
- fixtures que chamam directamente métodos privados ou substituem o caminho real.

## 4. Âmbito

### Incluído

- Inventário das interfaces reais de criação de subscription, publisher e avaliação.
- Runner fail-closed com fases `baseline`, `post-fix` e `functional`.
- Casos known publisher, unknown `BID == "NULL"`, PID overlap, unknown non-NULL BID, lookup ausente/tipo inesperado, duplicate e delete.
- Oráculos de identidade, estado, labels, índices, inserção, deduplicação, processamento e deleção.
- Debug, RelWithDebInfo e Sanitizer.
- Isolamento de processo/IPC e cleanup verificável.
- Integração com `ctest` apenas depois de o runner ser determinístico.

### Excluído

- Alterações adicionais em `CoreRunEvaluate01.cpp` sem revisão própria.
- Alteração da hierarquia `Core`/`PG`/`PGCS`.
- Alteração de wire format, inverted pub/sub, Gateway, SHM ou ownership.
- Claims de performance.
- Uso de estado privado não observável como se fosse evidência.

## 5. Critérios de aceitação

- [ ] Caminho de entrada e criação de subscriptions documentado a partir do código real.
- [ ] Interface E2E escolhida ou seam test-only justificado com evidence gap explícito.
- [ ] Runner reproduz o baseline RED da SPEC-044 ou documenta por que o diagnóstico só é reproduzível no smoke completo.
- [ ] Runner falha em timeout, cleanup incerto, identidade ausente, exit inesperado ou marcador ausente.
- [ ] Known publisher preserva labels e não insere duplicate.
- [ ] Unknown `BID == "NULL"` insere exactamente um peer e repetições não duplicam.
- [ ] PID overlap com `PGCSTuples` respeita a política de exclusão.
- [ ] Unknown non-NULL BID permanece unresolved sem atribuição falsa.
- [ ] Lookup ausente/null/wrong-type falha fechado.
- [ ] Empty peer vector e peer no índice zero não produzem falsa resolução.
- [ ] `Processing required` e `Delete` têm resultados observáveis e correctos.
- [ ] Deleção de entradas adjacentes é validada sem saltos silenciosos.
- [ ] Casos passam em Debug, RelWithDebInfo e Sanitizer.
- [ ] Dados brutos, logs, hashes, configuração e manifestos são preservados.
- [ ] Runner é registado no `ctest` somente depois dos testes manuais passarem.
- [ ] Astra revê SPEC, runner, oráculos e resultados; a SPEC-044 só fecha depois da aceitação final.

## 6. Plano de trabalho

1. Mapear `PGCS::NewBlock`, criação de `PG/Core`, command lines e ciclo de avaliação.
2. Identificar o caminho E2E mínimo e os marcadores observáveis existentes.
3. Definir unidade experimental, pré-condições, oráculos e cleanup.
4. Executar baseline sem a correção em subprocesso isolado quando possível.
5. Implementar apenas runner/fixtures test-only.
6. Executar a matriz funcional no estado pré e pós-correção.
7. Rever resultados com Astra e corrigir o runner, não enfraquecer os oráculos.
8. Integrar no `ctest`, se a determinabilidade for demonstrada.
9. Actualizar SPEC-044 e só então avaliar encerramento/release.

## 7. Riscos

| Risco | Impacto | Mitigação | Estado |
|---|---|---|---|
| Protocolo E2E não expõe estado suficiente | Falso GREEN | Registar evidence gap e avaliar seam separado | aberto |
| Logs confundidos com oráculo | Aceitação inválida | Identidade/evento no mesmo objecto ou resposta | aberto |
| IPC residual entre trials | Resultados contaminados | subprocesso isolado, cleanup por identidade e manifest | aberto |
| Runner dependente de timing | Flakiness | barreiras/protocolo, não sleeps arbitrários | aberto |
| Seam altera comportamento | Teste não representativo | alvo test-only e revisão Astra | aberto |

## 8. Evidência inicial

- `ctest --test-dir cmake-build-sanitizer --output-on-failure`: nenhum teste encontrado.
- `PGCS/src/PGCS.cpp:91–100`: criação de `PG` e `Core` no mesmo processo.
- `Common/src/Process.h:254–255`: `GetBlock()` público por nome.
- `PGCS/src/CoreRunEvaluate01.cpp:408–525`: alvo da SPEC-044.
- `PGCS/src/execPGCS.cpp:132–133, 179–180`: modos locais `-l`/`-lc` e criação do processo PGCS.
- `PGCS/src/PGCS.cpp:91–100`: criação dos blocos irmãos `PG` e `Core` no mesmo processo.
- `Scripts/Simple/Intra_OS_Complete_Test.sh` e `Scripts/Simple/Intra_OS_Content_Test_NRNCS.sh`: candidatos a fluxo E2E local com PGCS, NRNCS e ContentApp.
- `Specs/RESULTS-SPEC-044/preliminary-red-20260912.md`.
- `Specs/RESULTS-SPEC-044/post-fix-20260912.md`.

## 10. Estado da investigação

O caminho E2E candidato foi localizado, mas ainda não foi convertido em harness determinístico. Os scripts existentes usam terminais interactivos e delays fixos; isso não é suficiente como runner científico. A próxima etapa é extrair os comandos, substituir delays por gates observáveis, isolar IO/IPC e definir oráculos ligados à mesma subscription/publisher.

## 11. Relação com SPEC-044

SPEC-044 pode permanecer implementada em `In Progress`, mas não pode ser marcada concluída enquanto os critérios funcionais desta SPEC-045 não produzirem evidência suficiente. Esta SPEC não autoriza alterações adicionais de produção por si só.
