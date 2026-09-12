# SPEC-044: Corrigir downcast inválido em CoreRunEvaluate01

**Author:** Antonio Alberti / Hermes
**Date:** 2026-09-12
**Status:** In Progress
**Branch:** AIOPT3
**Implementation commit:** —
**Related:** SPEC-041-gw-deadline-aware-wait.md, SPEC-040-common-performance-build-profiles.md

## 1. Problema e evidência

O build Sanitizer do `PGCS` alcança o fluxo de avaliação e o UBSan reporta:

```text
PGCS/src/CoreRunEvaluate01.cpp:425:10:
runtime error: downcast of address ... which does not point to an object of type 'PG'
note: object is of type 'Core'
```

O código actual é:

```cpp
PG* PPGB = 0;
PPGB = (PG*)PB;
```

A função `CheckSubscriptions()` recebe a acção criada para um objecto `Core`; `PB` é o owner da acção e o diagnóstico confirma que o objecto dinâmico é `Core`, não `PG`. O cast viola o contrato de tipos C++ e torna o acesso subsequente a `PPGB->PGCSTuples` comportamento indefinido.

O relatório foi observado durante o smoke Sanitizer de SPEC-041. O defeito é independente da alteração de espera do Gateway e deve ser tratado separadamente.

## 2. Objectivo

Eliminar o downcast inválido em `CoreRunEvaluate01::CheckSubscriptions()` e preservar o comportamento funcional da verificação/armazenamento de publishers, sem ampliar a alteração para outros callers ou para a arquitectura NovaGenesis.

## 3. Hipótese a validar

A análise estática confirmou que `PGCSTuples` é membro de `PG`; `Core` é um `Block` separado e não declara esse estado; `PB->PP` aponta para `PGCS`, que também não declara `PGCSTuples`. O armazenamento deve ser obtido através do owner `PG` real, se existir e puder ser referenciado legitimamente, ou o acesso deve ser removido/reprojectado se for redundante. A solução correcta depende da confirmação do contrato entre `Core`, `PG`, `PGCS`, `PB->PP` e `PGCSTuples`; não se assume que outro cast seja seguro.

## 4. Allowlist de produção

Incluído:

- `PGCS/src/CoreRunEvaluate01.cpp`
- `CoreRunEvaluate01::CheckSubscriptions()`
- região actual aproximada `408–523`, a confirmar contra a fonte imediatamente antes da implementação
- testes específicos do caminho de subscriptions e do arranque Sanitizer

Explicitamente excluído:

- todos os outros padrões `(PG*)PB` no repositório;
- `PGRun*`, `PGHello*` e outros callers de `PG`;
- alteração da hierarquia `Core`/`PG`/`PGCS`;
- Gateway, filas, SHM, wire format, inverted pub/sub, lifecycle ou ownership;
- correcção de outros diagnósticos UBSan, incluindo downcasts fora deste caller;
- alteração de `CoreRunEvaluate01.h` ou APIs públicas, salvo necessidade demonstrada e nova revisão da SPEC.

## 5. Critérios de aceitação

- [ ] O contrato de ownership de `PGCSTuples` e a origem correcta do estado estão documentados a partir do código real.
- [ ] Fixture/runner test-only reproduz o caminho que origina o UBSan antes da alteração (RED), ou a limitação de observabilidade é registada.
- [x] A solução não executa downcast de um `Core*` para `PG*` nesse caller.
- [x] Builds CMake Debug, RelWithDebInfo e Sanitizer dos alvos aplicáveis passam para `PGCS` e `ContentApp`.
- [x] Smoke Sanitizer alcança e atravessa `CheckSubscriptions()` sem este diagnóstico UBSan, confirmado por marcador DEBUG isolado.
- [ ] O comportamento nominal de subscriptions é preservado com marcador/estado observável e resultado exacto; o build actual não possui testes registados (`ctest`: nenhum teste encontrado).
- [ ] Não há alteração de wire bytes, ordering, ownership, lifecycle, filas, SHM ou inverted pub/sub.
- [x] O diagnóstico UBSan alvo desaparece no smoke pós-correção; diagnósticos posteriores permanecem classificados separadamente.
- [x] `git diff --check`, hashes de fonte/binário e evidência de build/smoke foram actualizados.
- [ ] Revisão Astra do diff exacto e aceitação pós-alteração são obtidas antes de encerrar.

## 6. Plano de trabalho

1. Congelar branch, commit, estado dirty e evidência Sanitizer actual.
2. Inventariar semanticamente os usos de `PGCSTuples`, `PB`, `PB->PP`, `Core` e `PG` neste caminho.
3. Definir a observabilidade pública do teste; não criar friend/API apenas para forçar uma fixture.
4. Executar o baseline RED com a configuração Sanitizer actual e preservar o log.
5. Submeter a análise e a allowlist exacta a revisão Astra.
6. Após aprovação Astra e autorização do utilizador, implementar a menor alteração possível.
7. Reexecutar o mesmo estímulo RED→GREEN, builds completos e controlos nominais.
8. Classificar qualquer diagnóstico UBSan restante fora do escopo.
9. Actualizar tarefa, manifesto e evidência; manter a SPEC aberta se algum gate falhar.

## 7. Rollback

Reverter apenas a alteração em `CoreRunEvaluate01.cpp`, preservando a fixture, o log RED e a evidência de revisão. Não reverter nem misturar as alterações de SPEC-040/041.

## 8. Riscos

| Risco | Impacto | Mitigação |
|---|---|---|
| Substituir o cast por outro cast incorrecto | UBSan ou corrupção silenciosa | Confirmar dynamic type e owner real antes da alteração |
| Remover acesso considerado necessário | Alteração funcional em subscriptions | Controlos nominais e estado observável |
| Fixture não alcançar o caller real | Falso GREEN | Smoke Sanitizer completo e rastreio de call path |
| Diagnóstico posterior confundido com este | Escopo e aceitação inválidos | Classificar cada trace por ficheiro/call path |

## 9. Evidência inicial

- Branch: `AIOPT3`.
- Commit base observado: `e8faaf3`.
- Fonte: `PGCS/src/CoreRunEvaluate01.cpp:408–523`.
- `/tmp/ng039-pgcs-sanitizer-smoke.log`.
- Resultado: timeout controlado após 5 s; Core/Gateway alcançados; UBSan downcast em `CoreRunEvaluate01.cpp:425`; nenhum ASan SEGV observado.
- `/tmp/spec044-pgcs-sanitizer-postfix.log` — smoke pós-correção; timeout controlado, Core/Gateway alcançados, diagnóstico alvo ausente e ASan errors = 0.
- `Specs/RESULTS-SPEC-044/preliminary-red-20260912.md` — baseline RED e análise de ownership.
- `Specs/RESULTS-SPEC-044/post-fix-20260912.md` — qualificação pós-correção e hashes.
- SPEC relacionada: `Specs/SPEC-041-gw-deadline-aware-wait.md`.

## 10. Estado

Esta SPEC separa formalmente o diagnóstico UBSan de SPEC-041. A alteração allowlisted foi implementada após revisão Astra e autorização do utilizador. Builds e smoke Sanitizer passaram; a aceitação funcional permanece aberta até à execução da matriz da SPEC-045 e à revisão Astra pós-correção.

## 11. Gate histórico de release

Após todos os critérios desta SPEC serem demonstrados e a branch AIOPT3 passar a revisão final de higiene/provenance, preparar a primeira release pública do NovaGenesis, `v1.0.0`, com notas de release, commit/tag verificáveis e links para artefactos reproduzíveis. A release não deve ser criada enquanto qualquer gate funcional, sanitizer, revisão Astra, documentação ou higiene permanecer aberto.
