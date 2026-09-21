---
id: NG-060-R0-R2-EXACT-SHA-REVIEW-20260921
title: NG-060 R0–R2 exact-SHA review packet — Local Normal Path
project: NovaGenesis
task: NG-060
status: blocked
review_scope: documentation-only
review_date: 2026-09-21
proposed_demonstrator_id: v0.1.0-research.1-AIOPT3
functional_sha: NO-SHA
---

# NG-060 — Pacote de revisão exact-SHA R0–R2

## 1. Finalidade e limite

Este documento prepara a revisão documental dos gates **R0–R2** do NG-060 — `RDEMO local normal path v0.1`. Não é um freeze funcional, não é autorização de trial e não é uma declaração de release.

O identificador `v0.1.0-research.1-AIOPT3` é apenas o identificador proposto para o demonstrador. O RDEMO não é chamado `v1.0.0`.

### Limites preservados

- Sem VM, SSH, QGA, `clean.sh`, reboot, trial, push, tag, publicação ou release.
- Sem preparação/execução do trial local.
- Sem collector remoto.
- Sem reabrir QGA-loss, hardening remoto, readback, subscriptions completas ou metas 100/1000.
- NG-055 e NG-056 permanecem dependências parciais; não são fechadas integralmente.
- Resultados históricos continuam históricos/positive controls; não são PASS actual.
- Nenhum resultado runtime, mapa, inventário, `result.json` ou seal foi fabricado.

## 2. Decisão estratégica Astra

A revisão foi enviada ao **GPT-6 Astra** com o contexto completo necessário para a decisão: plano mestre, tarefa NG-060, rebaseline A/B/C, follow-up Astra, SPEC-055/SPEC-056/SPEC-060 relevantes, plano/cenário local, receipt de build, hashes dos binários, estado Git, configuração efectiva conhecida e classificações dos bundles históricos.

- Modelo: `gpt-6-astra`
- Label: `NG-060 R0-R2 strategic exact-SHA review 20260921`
- Primeira revisão do tracker: `2026-09-21T08:02:02Z`; resposta em `~/CodeRepository/tool-llm-usage-accounting/data/last_response.md`.
- Follow-up documental do pacote: `2026-09-21T08:14:27Z`; resposta em `~/CodeRepository/tool-llm-usage-accounting/data/last_response.md`.
- Revisão final do pacote corrigido: `2026-09-21T08:19:53Z`; resposta em `~/CodeRepository/tool-llm-usage-accounting/data/last_response.md`.
- Uso registado: primeira revisão `input_tokens=19823`, `output_tokens=1576`, `total_tokens=21399`; follow-up `input_tokens=5806`, `output_tokens=747`, `total_tokens=6553`; revisão final `input_tokens=6344`, `output_tokens=207`, `total_tokens=6551`.

### Veredicto Astra aplicado

1. **R0 — BLOCKED; functional SHA: `NO-SHA`.** Não existe freeze demonstrado.
2. **R1 — OPEN.** Os seis hashes actuais coincidem com o receipt histórico de `4b01e5e`, mas isso não demonstra build do candidato NG-060 actual nem identidade do executável que seria lançado; R1 depende de R0.
3. **R2 — BLOCKED.** Configuração efectiva, workload qualificado e isolamento dos roots não estão demonstrados. O gerador actual produz bytes “JPEG-shaped”; a validade JPEG decodificável não foi estabelecida.
4. **Escritas permitidas:** somente documentação, sem activar plano, alterar ambiente/código ou preparar staging.
5. **Histórico do follow-up:** a revisão intermédia foi `AMEND`; a revisão final do pacote corrigido é `APPROVED-DOC`, somente quanto à fidelidade documental e segurança de escopo. Não há aceite dos gates nem autorização operacional.
6. **Próximo gate:** revisão documental read-only do candidato funcional exact-SHA, da fronteira de provenance e do freeze; não é gate de autorização operacional.

## 3. Ledger de identidade exact-SHA

> As identidades, hashes, verificações e acções abaixo são observações reportadas no contexto fornecido, não verificações independentes desta revisão textual. A coincidência de hashes é limitada aos ficheiros observados no snapshot e não prova identidade de um executável lançado.

| Boundary | Identidade verificada | Interpretação |
|---|---|---|
| Branch | `AIOPT3` | branch activo |
| Current `HEAD` | `b1dd1764ed1d658fa93a1ed151d73ef5ed6455ef` | commit documental: `docs(release): record Astra rebaseline approval`; altera apenas o follow-up Astra |
| Worktree (snapshot pré-escrita) | limpo | `git diff --check` PASS; nenhum caminho alterado antes da criação deste pacote |
| `origin/AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | ref local e servidor coincidem; HEAD local está 37 commits à frente e 0 atrás; nenhum push |
| Archive SHA de `HEAD` | `d9a33673faa3fd5a0f4f6851e3392e7ace71f3d283097c5008322730d29884a7` | não equivale ao archive do receipt histórico |
| Controller SHA em `HEAD` | `0da46b21b9c0e1cb801ec62dd9e60a797b1be1dc75fd25005e1f69893e405670` | difere do controller do receipt histórico |
| Receipt-backed binary source | `4b01e5e258b7228958e027eb1df963048f6629c3` | candidato de origem dos binários do receipt `g0-build-linkage-v5`; não é automaticamente o candidato NG-060 |
| Archive SHA de `4b01e5e` | `fe0f96b9a922e4bced86b5107e940fc70515490ea7ca16f025fd9e62291c4d97` | identidade histórica do tree usado no receipt |
| Controller SHA em `4b01e5e` | `dd1bf82d110c9c6e7e2432d78a113ddf664e7c3c440d59497fa408984e1b6fb5` | identidade histórica do controller usado no receipt |
| Receipt-publication commit | `b6c423e949f726d6835df2680f82b0374e050b26` | confirmado: `SPEC-054: Record current build linkage`; adiciona apenas o receipt; não é a origem dos binários |
| Later local boundary | `ce5a7869ff61733362553b3a8d5bb3a0a627cc3e` | PTY process-group identity; altera controller, testes e review |
| Later local boundary | `bc72435b859e49cbb364e1116ffe1718977b0edd` | QGA-loss/local finalizer; altera controller e teste |
| A/B/C remote candidate | `f1bf3bc94db4857c3b94966f9183372850948992` | payload evidence remoto; não é receipt de binários NG-060 |
| NG-060 functional SHA | **`NO-SHA`** | não seleccionado/congelado; não deve ser inferido a partir de `HEAD` ou `4b01e5e` |

### Conclusão de identidade

O `HEAD` documental não pode ser promovido a SHA funcional. O receipt antigo não pode ser promovido a candidato actual sem uma equivalência bounded de inputs de build e uma proveniência actual do controller. O pacote permanece explicitamente **sem SHA funcional**.

## 4. R0 — exact-SHA, plano e configuração

**Estado: BLOCKED.**

### Evidência documental existente

- Plano Mestre: `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`.
- Plano local declarado: `Scripts/AlpineVMs/plans/local-intra-os.example.json`.
- Cenário declarado: `Scripts/AlpineVMs/observability/scenarios/LOCAL.json`.
- Controller: `Scripts/AlpineVMs/ng_remote_executor.py`.
- Tarefa: `/home/gandalf/ObsidianVault/Tarefas/NG-060-rdemo-local-normal-path-v0-1-20260920.md`.

O plano declarado contém, entre outros, `native-privileged`, ordem `PGCS → NRNCS → Repository → Source`, readiness `240s`, observation `120s`, total `600s`, oracle Source/Repository e workload declarada de cinco itens com seed `56002`, tamanho `4096` e prefixo `spec056-photo`.

### Gaps que impedem R0

- `NG_LOCAL_REPO_PATH`, `NG_LOCAL_BUILD_PATH`, `NG_LOCAL_IO_PATH`, `NG_LOCAL_EVIDENCE_PATH` e `NG_LOCAL_BUILD_MANIFEST` estão unset no ambiente actual.
- O `ng-vm.env` ignorado existe, mas contém configuração remota; não fornece a configuração local efectiva.
- O EUID actual é `1000`, enquanto `native-privileged` exige a verificação de EUID 0 para controller e roles.
- Não existe freeze receipt NG-060 que ligue um SHA único a plano expandido, configuração efectiva, workload, oracle e evidence root.
- Não existe boundary de provenance actual que demonstre que o plano e a configuração pertencem à árvore que produziu os binários.

### R0 necessário, ainda não produzido

- exact-SHA funcional escolhido e imutável;
- plano expandido e configuração efectiva preservada;
- concordância entre perfil declarado, CLI e EUID;
- roots absolutos e exclusivos para Source, Repository, IO e evidence;
- freeze receipt específico do NG-060.

## 5. R1 — build receipt e ligação código/configuração/executável

**Estado: OPEN, bloqueado por R0.**

### Receipt histórico disponível

`Specs/RESULTS-SPEC-054/g0-build-linkage-v5-20260916.md` identifica:

- Candidate HEAD: `4b01e5e258b7228958e027eb1df963048f6629c3`;
- Documentation commit: `b6c423e949f726d6835df2680f82b0374e050b26`;
- árvore limpa no momento do build;
- seis binários Debug/Sanitizer e hashes correspondentes;
- receipt como evidência histórica, sem fechar G0.

### Hashes actuais que coincidem com o receipt

| Perfil | Executável | SHA-256 |
|---|---|---|
| Debug | PGCS | `ec3e9bdd309973d31340faabe001802c977fdca12c8d7f95ffe7eac0ef4e9a69` |
| Debug | NRNCS | `ad9ec118bd8374b9d06fc327c4f5fa850949cd93339827e84a5e529ea2b2579c` |
| Debug | ContentApp | `910a3c038f72d539b013c01d9dcda319e346f91b3c6fc9d9e28b527eaf879cc6` |
| Sanitizer | PGCS | `96d9ae8ca0c4a95e3f5a94b9360556b5e752638b7f924625caeb9917ec99c094` |
| Sanitizer | NRNCS | `76621ae691f030ff6e0eac601f059f5f2d4be40b38c56910367b4cd08326eb08` |
| Sanitizer | ContentApp | `966296fb35c33b24a3b31af7a2e767f6ebc69149f9abe100a87e4073ed2ce9bc` |

A coincidência reportada sustenta somente a correspondência de hashes nos ficheiros observados no snapshot. Não fecha `candidate SHA → configuration → build → launched executable`.

### R1 necessário, ainda não produzido

- receipt novo ou reconciliado para um candidato NG-060 seleccionado;
- build inputs/configuração ligados ao candidato;
- controller e helpers importados ligados ao mesmo boundary;
- manifest de build e caminhos absolutos dos executáveis;
- ligação explícita `SHA → configuração → build → executável → role`.

Nenhum build, deployment ou launch foi autorizado por este pacote.

## 6. R2 — workload novo e roots isolados

**Estado: BLOCKED.**

### Contrato declarado

- cinco ficheiros novos;
- mapa expected: nome, tamanho e SHA-256;
- roots Source e Repository exclusivos, não aninhados e sem alias;
- Repository inicialmente vazio;
- cópias preservadas e re-hasháveis.

### Gaps actuais

- Não foram gerados JPEGs novos nesta sessão.
- Não foi criado staging de trial.
- Não existe mapa expected actual específico do NG-060.
- Não existe prova actual de Repository vazio nem inventário de isolamento dos roots.
- O gerador do controller produz bytes com forma/cabeçalho/terminador de JPEG; a validade de imagem JPEG decodificável não foi demonstrada. O requisito deve permanecer qualificado como gap, não ser silenciosamente redefinido.
- Bundles anteriores não fecham R2: um bundle histórico possui Source/Repository mas é inelegível por provenance/build linkage; outro possui Repository vazio; o bundle QGA possui apenas Source staging.

### R2 necessário, ainda não produzido

- cinco payloads novos após a selecção do candidato/configuração;
- mapa expected actual;
- roots absolutos e exclusivos;
- prova de baseline vazio do Repository;
- manifest de isolamento e cópias preservadas.

A produção desses ficheiros não foi realizada nesta sessão porque seria preparação operacional/staging, além do que é necessário para o pacote documental actual.

## 7. Matriz de gates R0–R2

| Gate | Estado | Evidência verificada | Blocker/gap principal |
|---|---|---|---|
| R0 | **BLOCKED** | branch, HEAD, worktree, origin, plano/cenário declarados (snapshot pré-escrita) | exact-SHA não seleccionado; configuração efectiva local ausente; freeze não produzido |
| R1 | **OPEN** | receipt histórico; correspondência de hashes reportada no snapshot | depende de R0; receipt pertence a `4b01e5e`; controller actual difere; ligação candidata não demonstrada |
| R2 | **BLOCKED** | contrato de workload e oracle no plano; históricos classificados | sem cinco payloads novos, expected map, roots isolados e prova Repository vazio; JPEG validity não qualificada |

A auditoria read-only **pré-escrita** `Scripts/ProjectAudit/ng_release_audit.py` confirmou:

- R0–R7: `OPEN`;
- `HEAD`: `b1dd1764ed1d658fa93a1ed151d73ef5ed6455ef`;
- worktree limpo no snapshot pré-escrita;
- veredicto geral: `BLOCKED`;
- issues: SPEC-054, SPEC-055 e SPEC-056 permanecem `In Progress`.

Estado pós-escrita fornecido: branch `AIOPT3`, HEAD `b1dd1764ed1d658fa93a1ed151d73ef5ed6455ef`; pacote documental untracked em `Specs/RESULTS-SPEC-060`; sem commit ou push. Auditoria read-only: R0–R7 `OPEN`, geral `BLOCKED`, `WORKTREE_DIRTY` apenas devido ao pacote. Os estados da auditoria não substituem a classificação estratégica R0 `BLOCKED`, R1 `OPEN`, R2 `BLOCKED`.

## 8. Applicability matrix

| Área | NG-060 R0–R2 | NG-058/SPEC-060 remoto | QGA-loss/failure path |
|---|---|---|---|
| exact-SHA/plano local | aplicável, aberto | N/A nesta revisão | N/A nesta revisão |
| build/linkage local | aplicável, receipt histórico insuficiente | N/A nesta revisão | N/A nesta revisão |
| cinco payloads/roots locais | aplicável, não produzido | N/A nesta revisão | N/A nesta revisão |
| collector remoto | fora de escopo | diferido ao escopo remoto | fora de escopo |
| QGA, SSH, multi-VM | fora de escopo | diferido/fora do RDEMO | fora de escopo |
| metas 100/1000/performance | fora de escopo | diferido | fora de escopo |

## 9. Pacote mínimo e próximo gate

Este ficheiro é o pacote documental mínimo actual, **preparado e com revisão final Astra `APPROVED-DOC`**, somente quanto à fidelidade documental e segurança de escopo. Deve ser enviado como **review packet R0 `BLOCKED` / R1 `OPEN` / R2 `BLOCKED` — `NO-SHA`**, não como aceite.

Os artefactos R1/R2 são requisitos pendentes, não acções autorizadas. O próximo passo limita-se à revisão documental de evidência existente; não autoriza build, alterações de ambiente, geração de payloads, staging local ou runtime.

Referências anexas:

- `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`;
- `Specs/RESULTS-SPEC-059/astra-trl3-trl4-rebaseline-amend-verification-20260920.md`;
- `Specs/RESULTS-SPEC-059/release-scope-abc-rebaseline-proposal-20260920.md`;
- `Specs/RESULTS-SPEC-054/g0-build-linkage-v5-20260916.md`;
- `Scripts/AlpineVMs/plans/local-intra-os.example.json`;
- `Scripts/AlpineVMs/observability/scenarios/LOCAL.json`;
- `Scripts/AlpineVMs/ng_remote_executor.py`;
- SPEC-055/SPEC-056/SPEC-060 e tarefa NG-060.

**Próximo gate único:** revisão read-only da fronteira funcional completa e do freeze documental. Essa revisão deve classificar o delta entre `4b01e5e`, `ce5a786`, `bc72435`, `b1dd176` e qualquer candidato seleccionado, sem executar build/trial.

## 10. Declaração de execução

**Nenhum runtime foi executado nesta sessão.**

Não foram executados VM, SSH, QGA, NG-ELC, build, trial, `clean.sh`, reboot, cleanup, staging local, collector remoto, push, tag, publicação ou release.

As acções realizadas foram: leitura de documentação, consulta read-only do histórico/contexto, verificação Git, auditoria read-only do plano, enumeração/hash read-only de artefactos existentes, criação deste documento e revisão estratégica Astra.
