# NovaGenesis — Plano Mestre da Release v1.0.0

**Plano:** `RELEASE-MASTER-PLAN-v1.0.0`  
**Versão:** 4 — escopo rebaseado
**Data de criação:** 2026-09-13  
**Branch candidata:** `AIOPT3`  
**Estado:** `BLOCKED — rebaseline v1.0.0 em execução`
**Tarefa guarda-chuva:** [[NG-050-release-master-control-v1-20260913]]  
**SPEC do método:** `Specs/SPEC-054-release-master-plan-and-audit.md`

> Este documento é o índice de decisão da release. Não substitui as tarefas Obsidian, as SPECs, os resultados brutos, os manifestos ou as skills. Nenhum gate passa por narrativa, checkbox isolada, build isolado ou opinião de um agente.

## 1. Objectivo

Controlar a primeira release pública `v1.0.0` da NovaGenesis através de uma cadeia rastreável:

```text
Tarefa → SPEC → gate → hipótese → experiência → evidência → commit/configuração → veredicto
```

O plano é actualizado somente quando existe evidência verificável. Contradições são registadas como `DRIFT`; não são corrigidas silenciosamente.

## 2.1 Controlador canónico de experiências

Todos os ensaios de release, locais ou multi-VM, devem ser executados pelo:

```text
NG Experiment Lifecycle Controller (NG-ELC)
```

Entry point actual durante a Proposal da SPEC-057:

```text
Scripts/AlpineVMs/ng_remote_executor.py
```

Após a implementação e aceitação da SPEC-057, o entrypoint canónico será `Scripts/AlpineVMs/ng_elc.py`; o caminho antigo poderá permanecer apenas como shim de compatibilidade. O modo remoto usa `--mode remote`; o cenário intra-OS usa `--mode local` e o plano `Scripts/AlpineVMs/plans/local-intra-os.example.json`.

## 2. Âmbito da release

### Incluído

- baseline/proveniência e freeze do candidato;
- higiene actual e segurança pública do perfil suportado;
- deployment, runner, observabilidade e teardown bounded;
- correctness do caminho normal de subscription;
- gates PGCS/NRNCS/Repository necessários ao caminho normal;
- workload novo de cinco JPEGs com mapas independentes;

### 2.1 Perfil mínimo suportado da v1.0.0

Para evitar que a primeira release pública misture todas as linhas históricas de investigação, a v1.0.0 certificará somente:

- Linux, branch `AIOPT3`, dois guests Alpine e a topologia normal documentada;
- executor remoto actual `Scripts/AlpineVMs/ng_remote_executor.py` — o rename da SPEC-057 fica para v1.1+;
- perfil normal NRNCS, uma subscription normal e o caminho Source → PGCS → NRNCS → Repository;
- workload novo de cinco JPEGs determinísticos, com mapas independentes nome → tamanho → SHA-256;
- lifecycle bounded, provenance/build receipt, bundle durável, teardown por identidade e revisão final.

O perfil mínimo não anuncia suporte genérico ao modo local, todos os caminhos de erro de subscriptions, performance/soak, 100/1000 fotos, multiplataforma ou equivalência completa dos perfis legacy. Essas claims permanecem rastreadas no Apêndice A para v1.1+.
- revisão Astra, release notes e tag verificável.

### Excluído

- implementação de optimizações NG-041, NG-043, NG-044 e NG-045;
- alterações experimentais de performance no candidato;
- criação ou promoção de AIOPT4;
- qualquer SPEC não relacionada sem decisão explícita.

NG-040 pode produzir o baseline de performance, sem introduzir alterações no candidato da release.

## 3. Vocabulário de veredictos

- `PASS` — critério demonstrado por evidência directa e reproduzível.
- `OPEN` — critério ainda não demonstrado.
- `BLOCKED` — pré-condição, aprovação ou configuração impede execução.
- `INCONCLUSIVE` — o ensaio ocorreu, mas o oráculo não permite concluir.
- `FAIL` — o comportamento esperado foi testado e não ocorreu.
- `N/A` — fora do âmbito, com justificação explícita.
- `DRIFT` — tarefa, SPEC, código ou evidência não estão coerentes entre si.

`concluída` numa tarefa e `Implemented` numa SPEC não fecham automaticamente um gate.

## 4. Gate register — perfil v1.0.0 rebaseado

| Gate | Objectivo | Tarefas | SPECs | Pré-requisito | Evidência | Estado | Próximo passo |
|---|---|---|---|---|---|---|---|
| M0 | Contrato suportado, higiene, baseline e freeze | NG-050, NG-038 | SPEC-038, SPEC-054 | — | `Specs/RESULTS-SPEC-054/g0-freeze-v4-20260914.md`; `Specs/RESULTS-SPEC-038/g1-hygiene-validation-v4-20260914.md`; `Specs/RESULTS-SPEC-038/g1-build-v4-20260914.md` | BLOCKED | Fechar classificação pública, documentos históricos, runtime reproduzível e revisão SPEC-054; a decisão fixture-only já está registada |
| M1 | Deployment, lifecycle, observabilidade e teardown do perfil remoto suportado | NG-046, NG-049, NG-055, NG-056 | SPEC-009, SPEC-046, SPEC-055, SPEC-056 | M0 | `Specs/RESULTS-SPEC-054/g2-e7-canonical-20260914.md`; `Specs/RESULTS-SPEC-056/` | OPEN | Fechar receipt/build real, bundle durável, ownership C01 e trial local/remote retido |
| M2 | Caminho normal end-to-end: PGCS, NRNCS, Repository, subscription e cinco JPEGs | NG-047, NG-048, NG-051, NG-052, NG-053, NG-054 | SPEC-044, SPEC-045, SPEC-047, SPEC-048, SPEC-049, SPEC-050 | M1 | `Specs/RESULTS-SPEC-047/`; `Specs/RESULTS-SPEC-048/`; `Specs/RESULTS-SPEC-049/`; `Specs/RESULTS-SPEC-050/` | OPEN | Executar um trial candidato normal com os subcritérios observáveis e hashes independentes |
| M3 | Revisão final, notas, tag e leitura de volta | NG-050 | SPEC-054, release criteria | M0–M2 | tag, manifest final e release notes | BLOCKED | Astra final, auditoria sem drift e tag `v1.0.0-AIOPT3` |

M2 mantém os subcritérios técnicos de G4–G8, mas a v1.0.0 certifica apenas o caminho normal suportado. O workload de cinco JPEGs é o contrato mínimo; 100/1000 fotos, stress e matrizes completas de erro ficam para v1.1+.

## 5. Ordem de execução

```text
M0 contrato/higiene/freeze
  → M1 deployment/lifecycle/teardown
  → M2 caminho normal end-to-end
  → M3 revisão/tag
```

M2 pode preparar os seus oráculos em paralelo, mas a aceitação depende de M1. Nenhum subcritério histórico é promovido a PASS por narrativa; claims adiadas são marcadas N/A para o perfil v1.0.0 e preservadas no Apêndice A.

## Apêndice A — mapa completo G0–G10 para v1.1+

O mapa abaixo é preservado para não perder trabalho, critérios ou evidência. Ele não é o gate register operacional da v1.0.0 rebaseada.

| Gate | Objectivo | Tarefas | SPECs | Pré-requisito | Evidência | Estado legado | Próximo passo v1.1+ |
|---|---|---|---|---|---|---|---|
| G0 | Âmbito, baseline e freeze | NG-050, NG-020, NG-036, NG-037 | SPEC-036, SPEC-037, SPEC-054 | — | `Specs/RESULTS-SPEC-054/g0-freeze-20260914.md` | BLOCKED | Reconciliar freeze completo |
| G1 | Higiene, segurança e provenance pública | NG-038 | SPEC-038 | G0 | `Specs/RESULTS-SPEC-038/g1-hygiene-validation-20260913.md` | OPEN | Fechar rotação, histórico e runtime legacy |
| G2 | Scripts de deployment reproduzíveis | NG-046 | SPEC-009 | G0 | `Specs/RESULTS-SPEC-054/g2-e7-canonical-20260914.md` | PASS operacional | Reassociar ao candidato final |
| G3 | Runner, observabilidade, modo local, teardown e canonicalidade | NG-049, NG-055, NG-056, NG-057 | SPEC-046, SPEC-052, SPEC-053, SPEC-055, SPEC-056, SPEC-057 | G2 | `Specs/RESULTS-SPEC-046/`, `Specs/RESULTS-SPEC-055/`, `Specs/RESULTS-SPEC-057/` | OPEN | Hardening completo, local mode e rename |
| G4 | PGCS on-wire e activação | NG-051 | SPEC-047 | G3 | `Specs/RESULTS-SPEC-047/` | OPEN | Aceitação formal |
| G5 | Processamento receptor PGCS | NG-052 | SPEC-048 | G4 | `Specs/RESULTS-SPEC-048/` | OPEN | Binding/oracle |
| G6 | NRNCS readiness e storage invocation | NG-053 | SPEC-049 | G5 | `Specs/RESULTS-SPEC-049/` | OPEN | Readback independente |
| G7 | Discovery/readback Repository | NG-054 | SPEC-050, SPEC-051 | G6 | `Specs/RESULTS-SPEC-050/` | OPEN | Fechar discovery-only/readback |
| G8 | Correctness completa de subscriptions | NG-047, NG-048 | SPEC-044, SPEC-045 | G3 | `Specs/RESULTS-SPEC-044/` | OPEN | Matriz de erros completa |
| G9 | Payload normal de 100 fotos | NG-050 | — | G6, G7, G8 | `Specs/RESULTS-MATCHED-comparison.md` | OPEN | Trial 100 fotos |
| G10 | Revisão final e tag integral | NG-050 | release criteria | G1–G9 | — | BLOCKED | Release integral |

## 6. Registo de consistência actual

Verificação realizada em 2026-09-13:

- A branch remota/default é `AIOPT3`.
- `AIOPT1` e `AIOPT2` não existem no remoto consultado; referências a essas branches devem ser tratadas como históricas. O clone local ainda conserva refs stale de `AIOPT1`, que não são base operacional nem devem ser usadas para a release.
- O candidato actual publicado/verificado é `c7deb7d`; commits anteriores (`1af604d`, `18d9b18`, `fce8577`, `4a8916e`, `203927e`, `eeb02bb`) permanecem apenas como evidência histórica.
- A matriz v4 cobre quatro macro-gates operacionais (`M0–M3`) para o perfil mínimo; o mapa G0–G10 completo foi preservado no Apêndice A para v1.1+.
- R12–R14 de SPEC-056 estão publicados como incrementos técnicos não-aceites; Astra mantém HOLD para aceitação formal.
- O downcast alvo de `CoreRunEvaluate01` foi corrigido no commit `21a5512`.
- Builds CMake Debug e Sanitizer passaram numa árvore temporária fora do repositório.
- A suite local do runner passou `16/16`; a suite do NG-ELC passou `20/20`; `py_compile` e `bash -n` passaram.
- SPEC-055 adicionou o modo local do NG-ELC; o ensaio local está selado, mas a revisão Astra devolveu `NO-GO` para aceitação.
- O primeiro ensaio local pelo NG-ELC foi selado em `Specs/RESULTS-SPEC-055/local-intra-os-20260913-final/` com `PASS/PASS/COMPLETE`, 5 fotos, hashes Source↔Repository, manifesto íntegro e zero processos/IPC residuais; permanece evidência local reportada, não aceitação multi-VM.
- A suite de contratos/lifecycle/observabilidade passou `44/44` na baseline actual; o ciclo anterior `42/42`, o ciclo `37/37` e o registo `30/30` são snapshots históricos; `py_compile` e `bash -n` passam nas ferramentas alteradas.
- O trial real final `local-intra-os-spec056-final-20260913` (antes do último incremento de ownership) demonstrou `PASS/PASS/COMPLETE`, mas `local_acceptance_eligible=false` e `exit_code=21`.
- O fluxo normal de 100 fotos tem evidência matched, mas não fecha subscriptions, discovery-only, readback ou teardown.
- A revisão Astra de SPEC-055 foi `NO-GO`; o relatório está em `Specs/RESULTS-SPEC-055/astra-review-20260913.md`.
- Trial real pós-guard `local-intra-os-spec056-guard-20260913`: `PASS/PASS/COMPLETE`, 23 ficheiros do manifesto e 10 JPEGs preservados, elegibilidade de acceptance `false`, `exit_code=21`.
- A revisão Astra pós-teste classificou o trial anterior como diagnóstico-only e recomendou o guard; relatório em `Specs/RESULTS-SPEC-055/astra-posttest-review-20260913.md`.
- SPEC-056 está `In Progress` e NG-056 `em-andamento`; o hardening técnico avançou, mas a aceitação do perfil mínimo continua aberta.
- SPEC-038 está `In Progress`; SPEC-044/045/046/047/048/049/050 continuam abertas como subcritérios M2; SPEC-052/053/055/056 continuam abertas como parte M1; as claims completas permanecem no Apêndice A.
- Existem referências ghost a `SPEC-015` e `SPEC-016` em documentos SPEC.
- Referências de branches anteriores permanecem apenas em documentos históricos/decisões; instruções activas devem continuar em AIOPT3.
- G2 teve hardening RED→GREEN local e o trial L5 canónico `e7-l5-direct-dfb95c9` fechou preflight, os cinco roles, markers de publicação/recepção, teardown e bundle. G2 está `PASS` operacional; G9 permanece `OPEN` porque ainda requer 100 fotos e mapas SHA-256 independentes.

## 7. Regras para novos agentes

### Início da sessão

1. Ler este plano mestre.
2. Verificar `git status`, branch e commit.
3. Seleccionar o primeiro gate `OPEN` ou `BLOCKED` que possa ser desbloqueado.
4. Ler apenas a tarefa, SPEC e evidência desse gate.
5. Carregar a skill aplicável.
6. Declarar hipótese, experiência, oráculo e condição de avanço.

### Execução

- Usar a menor experiência capaz de falsificar a hipótese.
- Manter commit, configuração, workload, binários e ferramenta identificados.
- Preservar RED antes de GREEN.
- Separar runtime, correctness, payload, teardown, provenance e performance.
- Não alterar produção sem SPEC própria, revisão e autorização aplicável.

### Fecho

1. Preservar logs, manifestos, hashes e resultado bruto.
2. Executar os testes da ferramenta e da SPEC.
3. Executar a auditoria read-only.
4. Actualizar tarefa/plano somente com evidência.
5. Registar exactamente um próximo gate ou bloqueio.

## 8. AIOPT4 e pós-release

AIOPT4 não faz parte deste plano. Só deve ser criada a partir da tag verificável:

```text
v1.0.0-AIOPT3
```

AIOPT3 permanecerá como branch de release/manutenção. NG-041, NG-043, NG-044 e NG-045 devem ter SPEC, baseline comparável e plano próprio em AIOPT4.

## 9. Critério de Go/No-Go

A release só pode avançar quando:

- todos os gates obrigatórios G0–G10 estão `PASS` ou `N/A` justificado;
- não existe drift de task/SPEC/código/evidência sem decisão registada;
- o commit candidato está congelado e reproduzível;
- builds normal e Sanitizer foram repetidos a partir do candidato;
- correctness funcional e runtime distribuído têm oráculos próprios;
- teardown deixa zero resíduos do trial;
- Astra reviu o diff, resultados e limitações;
- a tag e as release notes foram verificadas após publicação.

## 10. Ferramenta de auditoria

Auditor read-only:

```text
Scripts/ProjectAudit/ng_release_audit.py
```

Execução com vault:

```bash
python3 Scripts/ProjectAudit/ng_release_audit.py \
  --repo . \
  --plan Specs/RELEASE-MASTER-PLAN-v1.0.0.md \
  --vault <obsidian-vault>
```

JSON para automação:

```bash
python3 Scripts/ProjectAudit/ng_release_audit.py \
  --repo . \
  --plan Specs/RELEASE-MASTER-PLAN-v1.0.0.md \
  --vault <obsidian-vault> \
  --json
```

A ferramenta não altera tarefas, SPECs, código, commits ou configuração.

## 11. Histórico do plano

| Versão | Data | Alteração |
|---|---|---|
| v1 | 2026-09-13 | Criação do plano mestre e do gate register para a Release 1.0.0 |
| v2 | 2026-09-13 | Registo da validação operacional do G2 e separação do hash mismatch para G9; freeze G0 continua pendente |
| v3 | 2026-09-13 | Hardening RED→GREEN dos launchers/builder e reconciliação de SPEC-009; novo build/smoke canónico ainda pendente |
