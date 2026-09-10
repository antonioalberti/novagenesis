# NG-018 — Pacote de evidência para sign-off

Data: 2026-09-10
Branch: AIOPT3
HEAD: 7d6796e9993c0f0bed80b6e990dd0c647fd6072f
Árvore: 285ffaeecccb18cdd9323445f222f4b8ecae273e
Estado: revisão documental; não constitui encerramento

## Conclusão actual

NG-018 ainda não pode ser marcada como concluída. A evidência operacional da
Phase 1 está presente, mas o artefacto exacto de `SPEC-028-D — sign-off final`
não foi localizado como ficheiro independente. O repositório contém
`Specs/SPEC-028-stress-telemetry-heartbeat.md`, cujo cabeçalho ainda diz
`Draft`, embora existam commits e resultados de implementação/validação.

É necessário reconciliar o nome/estado de SPEC-028-D com a evidência e o
sign-off registado antes de fechar a tarefa.

## Matriz evidência → critério

| Critério | Evidência encontrada | Estado |
|---|---|---|
| Guard tests | `Specs/RESULTS-SPEC-027/guard-tests-20260907.json`: 6/6 PASS, incluindo self-MAC, peer válido, single SID, routing, restart recovery e frames malformados | Verificado no registo |
| Heartbeat/telemetria | `Specs/RESULTS-SPEC-027/spec028b-heartbeat-validation.json`: 20+ linhas em ~13 min, counters plausíveis/monotónicos, `dropped=0`, `guard_reject=0` | Verificado no registo |
| Carga bidireccional | `Specs/RESULTS-SPEC-027/amend3v2-500-run.json`: 37+ min, taxa reconciliada por delta de counters, 0 cores, RSS limitado | Verificado no registo; não é soak lossless |
| Integridade de 1000 fotos | `Specs/RESULTS-SPEC-027/spec030-runtime-1000-photos.json`: 1000/1000, 0 missing, 0 mismatched, comparação contra manifest | Verificado no registo |
| SPEC-029 fail-fast | `Specs/SPEC-029-pgcs-self-mac-failfast.md` + guard result: self-MAC rejeitado e peer válido aceite | Verificado no registo; SPEC ainda declara `In Progress` |
| Sign-off final SPEC-028-D | Nenhum ficheiro `SPEC-028-D` encontrado; `SPEC-028-stress-telemetry-heartbeat.md` permanece `Draft` | Pendente |
| Aplicabilidade ao HEAD actual | HEAD contém documentação posterior e alterações dos grupos A/B0 de SPEC-033; a evidência foi executada em commits anteriores | Requer reconciliação documental |

## Decisões de separação

- Os resultados normais de NG-018 não aceitam o B1 de SPEC-033.
- O crash B1 em `GW.cpp:857` permanece um workstream separado.
- Não haverá alteração de produção para resolver esta pendência documental.
- O teste de interrupção de link continua explicitamente diferido, conforme o
  registo dos guard tests.

## Próximo passo

1. Localizar no histórico/transcrições ou reconstruir a referência exacta ao
   sign-off de `SPEC-028-D`.
2. Confirmar se `SPEC-028-D` é uma subdivisão formal de SPEC-028 ou apenas o
   nome do conjunto de guard tests.
3. Reconciliar os estados dos ficheiros SPEC-028 e SPEC-029 com os commits e
   resultados verificados.
4. Só então pedir/registrar o sign-off final de NG-018.

Este documento é um pacote de revisão e não altera o estado da tarefa NG-018.
