# NG-018 — Pacote de evidência para sign-off

Data: 2026-09-10
Branch: AIOPT3
HEAD: 7d6796e9993c0f0bed80b6e990dd0c647fd6072f
Árvore: 285ffaeecccb18cdd9323445f222f4b8ecae273e
Estado: reconciliado documentalmente; candidato a encerramento

## Conclusão actual

A referência histórica `SPEC-028-D` foi localizada e reconciliada: não é um
ficheiro independente, mas o conjunto de guard tests dirigido registado no
commit `faa6c4e`. O commit `50e351c` alterou o estado de SPEC-027 para
`TEST SIGNED OFF by Astra`, explicitamente com referência aos guard tests 6/6.
A evidência satisfaz o critério de sign-off final da NG-018, sem exigir novo
runtime ou alteração de produção.

A decisão mantém B1 de SPEC-033 separado: o crash em `GW.cpp:857` não é
aceite por estes testes nem é pré-requisito para o encerramento de NG-018.

## Matriz evidência → critério

| Critério | Evidência encontrada | Estado |
|---|---|---|
| Guard tests | `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`: 6/6 PASS, incluindo self-MAC, peer válido, single SID, routing, restart recovery e frames malformados | Verificado no registo |
| Heartbeat/telemetria | `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`: 20+ linhas em ~13 min, counters plausíveis/monotónicos, `dropped=0`, `guard_reject=0` | Verificado no registo |
| Carga bidireccional | `Specs/RESULTS-SPEC-027/amend3v2-500-run.json`: 37+ min, taxa reconciliada por delta de counters, 0 cores, RSS limitado | Verificado no registo; não é soak lossless |
| Integridade de 1000 fotos | `Specs/RESULTS-SPEC-027/spec030-runtime-1000-photos.json`: 1000/1000, 0 missing, 0 mismatched, comparação contra manifest | Verificado no registo |
| SPEC-029 fail-fast | `Specs/SPEC-029-pgcs-self-mac-failfast.md` + guard result: self-MAC rejeitado e peer válido aceite | Verificado no registo; SPEC ainda declara `In Progress` |
| Sign-off final SPEC-028-D | Commit `faa6c4e`: `SPEC-028-D: directed guard tests 6/6 PASS`; commit `50e351c`: `SPEC-027: Phase 1 TEST SIGNED OFF by Astra (guard tests 6/6, commit faa6c4e)` | Satisfeito |
| Aplicabilidade ao HEAD actual | A evidência foi executada antes da documentação posterior dos grupos A/B0 de SPEC-033; NG-018 não altera esses caminhos | Não exige novo runtime |

## Decisões de separação

- Os resultados normais de NG-018 não aceitam o B1 de SPEC-033.
- O crash B1 em `GW.cpp:857` permanece um workstream separado.
- Não haverá alteração de produção para resolver esta pendência documental.
- O teste de interrupção de link continua explicitamente diferido, conforme o
  registo dos guard tests.

## Próximo passo

1. Actualizar a nota NG-018 para `concluida`, preencher `fim` e remover o
   bloqueio `SPEC-028-D`.
2. Manter B1 de SPEC-033 como tarefa separada e bloqueada pela revisão Astra.
3. Não executar novo runtime apenas para resolver esta lacuna documental.

A reconciliação está baseada nos commits `faa6c4e` e `50e351c`, no resultado
`guard-tests-20260907.json` e no histórico das sessões. Não há alteração de
produção neste fecho.
