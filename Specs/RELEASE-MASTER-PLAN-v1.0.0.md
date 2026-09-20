# NovaGenesis — Plano Mestre de Investigação TRL3→TRL4 e Engenharia v1.0.0

**Plano:** `RELEASE-MASTER-PLAN-v1.0.0`  
**Versão:** 6 — demonstrador local TRL4 separado da engenharia remota/QGA
**Data de criação:** 2026-09-13; rebaseline documental 2026-09-20<br>
**Branch candidata:** `AIOPT3`  
**Estado:** `RDEMO em preparação documental; engenharia v1.0.0 e QGA em HOLD`
**Tarefa guarda-chuva:** [[NG-050-release-master-control-v1-20260913]]  
**SPEC do método:** `Specs/SPEC-054-release-master-plan-and-audit.md`

> Este documento é o índice de decisão da release. Não substitui as tarefas Obsidian, as SPECs, os resultados brutos, os manifestos ou as skills. Nenhum gate passa por narrativa, checkbox isolada, build isolado ou opinião de um agente.

## 1. Objectivo

Controlar dois marcos separados da NovaGenesis através de uma cadeia rastreável:

```text
Tarefa → SPEC → gate → hipótese → experiência → evidência → commit/configuração → veredicto
```

1. **RDEMO:** `NovaGenesis Research Demonstrator 0.1 — Local Normal Path`, primeira demonstração TRL3→TRL4, com o caminho local mínimo R0–R7.
2. **Engenharia:** `NovaGenesis Engineering Release 1.0 — Remote Normal Profile`, mantendo os gates M0–M3 e SPEC-060 remotos estritos.

QGA-loss permanece uma linha de investigação separada, em `HOLD/NO-GO`. O RDEMO não fecha, não substitui e não promove os gates de engenharia remota.

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

## 2. Âmbito dos marcos

### 2.1 Incluído no RDEMO local

- exact-SHA e plano congelado;
- build receipt e hashes dos binários efectivamente usados;
- configuração Linux local específica e NG-ELC local bounded;
- workload novo de cinco JPEGs determinísticos;
- mapas independentes expected/Source/Repository, sem substituir a observação por markers;
- cópias preservadas, re-hash independente, teardown por identidade e inventário final;
- bundle durável com manifest/seal schema-v2 e verificador offline;
- revisão do bundle e matriz explícita de claims e limitações.

O RDEMO não reclama produção, M2-R, multi-VM, persistência NRNCS, readback de protocolo, semântica geral de subscriptions, performance ou QGA-loss.

### 2.2 Mantido como engenharia remota v1.0.0

- baseline/proveniência e freeze do candidato;
- deployment, runner, observabilidade e teardown bounded multi-VM;
- correctness do caminho normal Source → PGCS → NRNCS → Repository;
- workload de cinco JPEGs com mapas independentes das três origens;
- SPEC-060 completa, collector/helper remoto, manifest/seal e verificação offline;
- revisão Astra, release notes e tag verificável.

### 2.3 Diferido ou fora do primeiro marco

- três origens Source/NRNCS/Repository no RDEMO;
- readback do Repository, semântica completa de subscriptions e multi-VM no RDEMO;
- Proxmox/QGA, QGA-loss, performance/soak, 100/1000 fotos e claims de produção;
- implementação de optimizações NG-041, NG-043, NG-044 e NG-045;
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

## 4. Gate register — RDEMO local e engenharia remota v1.0

### 4.1 Caminho RDEMO local

| Gate | Objectivo | Tarefas | SPECs | Pré-requisito | Evidência | Estado | Próximo passo |
|---|---|---|---|---|---|---|---|
| R0 | Exact-SHA e plano congelado | NG-060 | SPEC-054, SPEC-055 | — | freeze/plan receipt | OPEN | revisão exact-SHA |
| R1 | Build receipt e identidade dos binários | NG-060 | SPEC-054, SPEC-056 | R0 | receipt e hashes | OPEN | preparar build ligado |
| R2 | Cinco JPEGs e isolamento Source/Repository | NG-060 | SPEC-055, SPEC-056 | R0, R1 | mapa expected + roots isolados | OPEN | preparar workload |
| R3 | Runtime local bounded e completion conhecido | NG-060 | SPEC-055 | R0–R2 | eventos NG-ELC e result | OPEN | autorização para uma experiência |
| R4 | Mapas independentes e cópias re-hashable | NG-060 | SPEC-056 | R3 | mapas/cópias Source/Repository | OPEN | recolha durante o trial |
| R5 | Teardown e inventário por identidade | NG-060 | SPEC-055, SPEC-056 | R3, R4 | inventário baseline/final | OPEN | verificar zero resíduos atribuíveis |
| R6 | Bundle durável, manifest/seal schema-v2 e verifier | NG-060 | SPEC-056 | R4, R5 | bundle selado + verificação | OPEN | verificar offline |
| R7 | Revisão e claims/limitações | NG-060, NG-050 | SPEC-054, SPEC-056 | R0–R6 | revisão Astra e matriz | OPEN | decisão RDEMO |

O RDEMO é um demonstrador experimental local. `N/A` significa exclusão explícita; nunca é `PASS` implícito. Uma perda de transporte, completion desconhecido, bundle não selado ou evidência incompleta impede aceitação.

### 4.2 Caminho de engenharia remota v1.0

| Gate | Objectivo | Tarefas | SPECs | Pré-requisito | Evidência | Estado | Próximo passo |
|---|---|---|---|---|---|---|---|
| M0 | Contrato suportado, higiene, baseline e freeze | NG-050, NG-038 | SPEC-038, SPEC-054 | — | `Specs/RESULTS-SPEC-054/g0-freeze-v4-20260914.md`; `Specs/RESULTS-SPEC-038/g1-hygiene-validation-v4-20260914.md`; `Specs/RESULTS-SPEC-038/g1-build-v4-20260914.md`; `Specs/RESULTS-SPEC-038/g1-historical-classification-v4-20260914.md` | BLOCKED | Fechar runtime normal e revisão SPEC-054; a decisão fixture-only e a higiene documental do perfil já estão registadas |
| M1-R | Deployment, lifecycle, observabilidade e teardown do perfil remoto suportado | NG-046, NG-049 | SPEC-009, SPEC-046, SPEC-052, SPEC-054 | M0 | `Specs/RESULTS-SPEC-054/g2-e7-canonical-20260914.md`; receipt/build remoto e bundle candidato | HOLD | Fixar SHA, receipt ligado aos binários e procedimento remoto fail-closed |
| M2-R | Caminho normal end-to-end: PGCS, NRNCS, Repository, subscription e cinco JPEGs | NG-047, NG-048, NG-051, NG-052, NG-053, NG-054, NG-058 | SPEC-044, SPEC-045, SPEC-047, SPEC-048, SPEC-049, SPEC-050, SPEC-060 | M1-R | `Specs/RESULTS-SPEC-047/`; `Specs/RESULTS-SPEC-048/`; `Specs/RESULTS-SPEC-049/`; `Specs/RESULTS-SPEC-050/`; SPEC-060 payload contract | OPEN | Implementar collector/helper e preparar um único trial candidato normal remoto |
| M1-L | Perfil local/native-privileged, QGA e QGA-loss → reconciliação → teardown → seal | NG-055, NG-056 | SPEC-055, SPEC-056, SPEC-058, SPEC-059 | linha QGA diferida | resultados locais históricos e `Specs/RESULTS-SPEC-059/` | DEFERRED / OPEN | Implementar seam C+D e rever separadamente; não bloqueia RDEMO nem v1.0 remoto |
| M3 | Revisão final, notas, tag e leitura de volta do perfil remoto | NG-050 | SPEC-054, release criteria | M0, M1-R, M2-R | tag, manifest final e release notes | BLOCKED | Astra final, auditoria sem drift e tag `v1.0.0-AIOPT3` |

M2-R mantém os subcritérios técnicos de G4–G8 para a engenharia remota. O RDEMO não fecha M0, M1-R, M2-R ou M3. SPEC-060 permanece estrita para o perfil remoto: o incremento controller-only está concluído, mas collector/helper e integração operacional continuam abertos.

## 5. Ordem dos marcos

```text
R0 exact-SHA/plano
 → R1 build receipt
 → R2 workload/isolamento
 → R3 runtime local bounded
 → R4 mapas/cópias
 → R5 teardown/inventário
 → R6 manifest/seal/verifier
 → R7 revisão/claims

M0 contrato/higiene/freeze
 → M1-R deployment/lifecycle/teardown remoto
 → M2-R caminho normal remoto end-to-end
 → M3 revisão/tag
```

O primeiro marco deve parar depois de uma única experiência local bounded e da revisão R7. Não há retry automático, fallback ou injecção de falha no RDEMO. Só se reabre implementação se existir um bloqueio concreto de R0–R7. A engenharia remota segue separadamente; nenhum subcritério histórico é promovido a PASS por narrativa.

### 5.1 Matriz de aplicabilidade da rebaseline

| Item diferido | Aplicabilidade v1.0.0 | Obrigações transversais mantidas | Destino |
|---|---|---|---|
| SPEC-055 / NG-055 | `N/A — perfil local fora do mínimo certificado` | O executor remoto continua canónico e deve ter lifecycle/teardown próprios | M1-L / v1.1+ |
| SPEC-056 / NG-056 | `N/A — contrato local de evidência fora do perfil remoto` | Provenance, build linkage, bundle durável, manifest, seal e verificação exigidos pelo remoto permanecem em M1-R | M1-L / v1.1+ |
| SPEC-058 | `N/A — native-privileged local não certificado na v1.0` | Nenhum fallback local/QGA pode ser usado para certificar o perfil remoto | M1-L / v1.1+ |
| SPEC-059 | `N/A — Proxmox/QGA não é dependência operacional do perfil remoto` | Perda SSH/teardown remoto deve ser tratada fail-closed no procedimento remoto declarado | M1-L / v1.1+ |
| SPEC-060 | `N/A no RDEMO; aplicável à engenharia remota M2-R` | No remoto: mapas independentes Source/NRNCS/Repository, cópias re-hashable, manifest, seal e verificação offline | M2-R / v1.0 |

`N/A` é relativo ao perfil explicitamente seleccionado; não fecha nem abandona as SPECs/tarefas diferidas.

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
- O candidato histórico publicado/verificado em 2026-09-14 era `c7deb7d`; a reconciliação documental actual da release foi publicada nos commits `20657b2` e `19d1c44`. Commits anteriores (`1af604d`, `18d9b18`, `fce8577`, `4a8916e`, `203927e`, `eeb02bb`) permanecem apenas como evidência histórica.
- A matriz v4 cobre quatro macro-gates operacionais (`M0–M3`) para o perfil mínimo; o mapa G0–G10 completo foi preservado no Apêndice A para v1.1+.
- R12–R14 de SPEC-056 estão publicados como incrementos técnicos não-aceites; Astra mantém HOLD para aceitação formal.
- O downcast alvo de `CoreRunEvaluate01` foi corrigido no commit `21a5512`.
- Builds CMake Debug e Sanitizer passaram numa árvore temporária fora do repositório.
- A suite local do runner passou `16/16`; a suite do NG-ELC passou `20/20`; `py_compile` e `bash -n` passaram.
- SPEC-055 adicionou o modo local do NG-ELC; o ensaio local está selado, mas a revisão Astra devolveu `NO-GO` para aceitação.
- SPEC-058 adiciona o perfil opt-in `native-privileged`, com root boundary, clean repository-owned e PTY; implementação/testes estão em curso, sem trial privilegiado retido.
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

### Reconciliação de 2026-09-16

- O candidato de código usado para os builds Debug/Sanitizer e para o receipt `Specs/RESULTS-SPEC-054/g0-build-linkage-v5-20260916.md` é `4b01e5e258b7228958e027eb1df963048f6629c3`.
- O commit `b6c423e949f726d6835df2680f82b0374e050b26` contém apenas o receipt documental; não é reivindicado como a origem dos binários.
- A árvore de `4b01e5e` estava limpa, os seis binários foram recompilados no Linux/VM 100 e os seus hashes foram verificados contra o receipt.
- A suite do NG-ELC passou `85/85`; isso não substitui o ensaio `native-privileged`, o bundle runtime selado ou a revisão Astra de aceitação.
- A integração SPEC-059 adicionou o adaptador Proxmox/QEMU Guest Agent, `channel-check` no NG-ELC e persistência de prova; a suite Alpine actual passou `172/172` com 7 subtestes.
- M0/G0 permanece `BLOCKED` até a reconciliação formal do freeze. M1-R/M2-R continuam em HOLD/OPEN; M3 continua bloqueado. SPEC-058 pertence à linha M1-L/v1.1+ e não é requisito do perfil remoto v1.0.
- O mapa G0–G10 é histórico/apêndice; os gates operacionais da release mínima são M0, M1-R, M2-R e M3, com M1-L diferido.
- Rebaseline documental de 2026-09-20: o primeiro marco passa a ser o RDEMO local R0–R7; M1-R/M2-R/M3 continuam o caminho separado da engenharia remota v1.0; QGA-loss permanece `HOLD/NO-GO`.
- Astra deu `CONDITIONAL GO` ao replaneamento TRL3→TRL4, não à operação: controller-only e fixtures de SPEC-060 ficam concluídos como evidência de componente; collector remoto, trial remoto, VM, SSH, QGA, `clean.sh`, reboot, push, tag e release continuam não autorizados.
- A regra anti-loop fica activa: não acrescentar novo hardening remoto, matrizes cosméticas, metas 100/1000, readback, subscriptions completas ou QGA-loss como pré-requisitos do RDEMO; só reabrir implementação perante blocker concreto de R0–R7.

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

A release v1.0 remoto só pode avançar quando:

- M0, M1-R, M2-R e M3 estão `PASS`, e quaisquer critérios fora do perfil estão `N/A` com justificação explícita;
- não existe drift de task/SPEC/código/evidência sem decisão registada;
- o commit candidato está congelado e reproduzível;
- builds normal e Sanitizer foram repetidos a partir do candidato;
- correctness funcional e runtime distribuído têm oráculos próprios;
- teardown deixa zero resíduos do trial;
- Astra reviu o diff, resultados e limitações;
- a tag e as release notes foram verificadas após publicação.

## 9.1 Critério de Go/No-Go do RDEMO

O RDEMO só pode ser aceite como demonstrador experimental quando R0–R7 estiverem satisfeitos por evidência ligada ao exact-SHA, com claims limitadas a: configuração Linux local específica, cinco JPEGs novos, igualdade byte-exact Source↔Repository, lifecycle observado e bundle verificável. Os itens explicitamente fora do perfil — NRNCS independente, readback de protocolo, subscriptions gerais, multi-VM, performance e QGA-loss — devem aparecer como `N/A`/diferidos, nunca como `PASS`.

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
| v4 | 2026-09-14 | Rebaseline do perfil mínimo Linux/AIOPT3, dois guests, cinco JPEGs e macro-gates M0–M3; G0–G10 preservados como Apêndice A |
| v5 | 2026-09-20 | Separação documental M1-R/M2-R (perfil remoto v1.0) de M1-L (local/native-privileged/QGA v1.1+), com matriz de aplicabilidade e Astra `CONDITIONAL GO` |
| v6 | 2026-09-20 | Replaneamento Astra TRL3→TRL4: RDEMO local R0–R7 como primeiro marco; engenharia remota v1.0 e QGA-loss mantidos em linhas separadas, sem autorização operacional |
