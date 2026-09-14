# NovaGenesis — Plano Mestre da Release v1.0.0

**Plano:** `RELEASE-MASTER-PLAN-v1.0.0`  
**Versão:** 3  
**Data de criação:** 2026-09-13  
**Branch candidata:** `AIOPT3`  
**Estado:** `BLOCKED`  
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

- baseline/proveniência de NG-020, NG-036 e NG-037;
- higiene e segurança pública de NG-038;
- scripts Alpine de NG-046;
- runner, observabilidade e teardown de NG-049;
- correctness de subscriptions de NG-047/NG-048;
- gates PGCS de SPEC-047/048;
- gates NRNCS/Repository de SPEC-049/050/051;
- reteste normal de 100 fotos com workload novo;
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

## 4. Gate register

| Gate | Objectivo | Tarefas | SPECs | Pré-requisito | Evidência | Estado | Próximo passo |
|---|---|---|---|---|---|---|---|
| G0 | Âmbito, baseline e freeze | NG-050, NG-020, NG-036, NG-037 | SPEC-036, SPEC-037, SPEC-054 | — | `Specs/RESULTS-SPEC-054/g1-current-audit-20260913.md` | BLOCKED | Definir candidato limpo e reconciliar registos |
| G1 | Higiene, segurança e provenance pública | NG-038 | SPEC-038 | G0 | `Specs/RESULTS-SPEC-038/g1-hygiene-validation-20260913.md` | OPEN | Resolver rotação/handoff histórico, classificação documental e runtime normal pós-cleanup |
| G2 | Scripts de deployment reproduzíveis | NG-046 | SPEC-009 | G0 | `Specs/RESULTS-SPEC-054/g2-preflight-20260913.md`; `Specs/RESULTS-SPEC-054/g2-final-smoke-20260913.md`; `Specs/RESULTS-SPEC-054/g2-script-hardening-20260913.md` | BLOCKED | Congelar G0 e executar novo build/smoke canónico no commit final; hash mismatch histórico fica no G9 |
| G3 | Runner, observabilidade, modo local, teardown e canonicalidade do entrypoint | NG-049, NG-055, NG-056, NG-057 | SPEC-046, SPEC-052, SPEC-053, SPEC-055, SPEC-056, SPEC-057 | G2 | `Specs/RESULTS-SPEC-046/`, `Specs/RESULTS-SPEC-055/`, `Specs/RESULTS-SPEC-057/` | OPEN | Concluir hardening da SPEC-056; depois implementar/rever o rename da SPEC-057 e executar novo trial local/preflight SSH |
| G4 | PGCS on-wire e activação | NG-051 | SPEC-047 | G3 | `Specs/RESULTS-SPEC-047/` | OPEN | Reconciliar diagnóstico e critérios de periodic activation |
| G5 | Processamento receptor PGCS | NG-052 | SPEC-048 | G4 | `Specs/RESULTS-SPEC-048/` | OPEN | Reconciliar resultado GDB com acceptance formal |
| G6 | NRNCS readiness e storage invocation | NG-053 | SPEC-049 | G5 | `Specs/RESULTS-SPEC-049/` | OPEN | Fechar binding/readback independente |
| G7 | Discovery/readback Repository | NG-054 | SPEC-050, SPEC-051 | G6 | `Specs/RESULTS-SPEC-050/` | OPEN | Localizar o primeiro edge não demonstrado |
| G8 | Correctness de subscriptions | NG-047, NG-048 | SPEC-044, SPEC-045 | G3 | `Specs/RESULTS-SPEC-044/` | OPEN | Construir harness e executar RED/GREEN |
| G9 | Payload normal de 100 fotos segundo o modelo de cache | NG-050 (controlo) | — | G6, G7, G8 | `Specs/RESULTS-MATCHED-comparison.md` | OPEN | Repetir no commit congelado |
| G10 | Revisão final e tag | NG-050 | release criteria | G1–G9 | — | BLOCKED | Astra, manifest final e tag `v1.0.0-AIOPT3` |

G4–G7 têm agora tarefas Obsidian NG-051–NG-054 próprias. A aceitação continua aberta até os critérios das SPECs e os oráculos correspondentes serem demonstrados.

## 5. Ordem de execução

```text
G0 âmbito/freeze
  → G1 hygiene/provenance
  → G2 scripts
  → G3 runner/teardown
  → G4 PGCS on-wire/activation
  → G5 receiver processing
  → G6 NRNCS readiness/storage
  → G7 Repository discovery/readback
  → G8 subscription correctness
  → G9 100-photo payload
  → G10 final review/tag
```

G8 pode desenvolver o harness em paralelo com G4–G6 depois de G3, mas a sua aceitação continua independente. G9 não pode começar enquanto G6, G7 e G8 não tiverem os oráculos necessários.

## 6. Registo de consistência actual

Verificação realizada em 2026-09-13:

- A branch remota/default é `AIOPT3`.
- `AIOPT1` e `AIOPT2` não existem no remoto consultado; referências a essas branches devem ser tratadas como históricas. O clone local ainda conserva refs stale de `AIOPT1`, que não são base operacional nem devem ser usadas para a release.
- O commit remoto verificado é `1af604d`.
- A árvore de trabalho local contém alterações rastreadas e ficheiros não rastreados; ainda não é um candidato congelado.
- A matriz reconciliada cobre `16` tarefas e `11` gates; G3 inclui o subitem NG-057/SPEC-057 de canonicalidade do entrypoint, dependente da SPEC-056.
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
- SPEC-056 está `In Progress` e NG-056 `em-andamento`; a implementação de hardening continua incompleta.
- SPEC-038 está `In Progress`; SPEC-044/045/046/047/048/049/050/051/052/053/054/055/056 continuam abertas ou parcialmente aceites.
- Existem referências ghost a `SPEC-015` e `SPEC-016` em documentos SPEC.
- Referências de branches anteriores permanecem apenas em documentos históricos/decisões; instruções activas devem continuar em AIOPT3.
- G2 teve validação operacional diagnóstica em 2026-09-13; depois os seis scripts receberam hardening RED→GREEN local (status do filho, pinning AIOPT3, receipt, SSH estrito, logs duráveis e cancelamento). O smoke remoto anterior preservou uma divergência de hash de payload para G9 e antecede o hardening. O gate formal permanece BLOCKED pelo G0/candidato não congelado; novo build/smoke canónico é obrigatório.

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
  --vault /home/gandalf/ObsidianVault
```

JSON para automação:

```bash
python3 Scripts/ProjectAudit/ng_release_audit.py \
  --repo . \
  --plan Specs/RELEASE-MASTER-PLAN-v1.0.0.md \
  --vault /home/gandalf/ObsidianVault \
  --json
```

A ferramenta não altera tarefas, SPECs, código, commits ou configuração.

## 11. Histórico do plano

| Versão | Data | Alteração |
|---|---|---|
| v1 | 2026-09-13 | Criação do plano mestre e do gate register para a Release 1.0.0 |
| v2 | 2026-09-13 | Registo da validação operacional do G2 e separação do hash mismatch para G9; freeze G0 continua pendente |
| v3 | 2026-09-13 | Hardening RED→GREEN dos launchers/builder e reconciliação de SPEC-009; novo build/smoke canónico ainda pendente |
