# SPEC-054: Plano mestre e auditoria determinística da Release v1.0.0

**Author:** Antonio Alberti / Hermes Agent  
**Date:** 2026-09-13  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-038-public-repository-hygiene.md; SPEC-044-core-run-evaluate-invalid-pg-downcast.md; SPEC-045-core-evaluate-functional-harness.md; SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-047-pgcs-readiness-periodic-activation.md; SPEC-048-pgcs-receiver-processing-evidence.md; SPEC-049-nrncs-readiness-binding-oracles.md; SPEC-050-contentapp-repository-control-plane-readback.md; SPEC-051-cross-vm-nrncs-discovery-diagnosis.md; SPEC-055-ng-elc-local-mode.md; SPEC-056-ng-elc-evidence-provenance-hardening.md; SPEC-057-ng-elc-canonical-entrypoint-rename.md

**Canonical experiment tool:** `NG Experiment Lifecycle Controller (NG-ELC)` — current implementation `Scripts/AlpineVMs/ng_remote_executor.py`; canonical rename proposed by SPEC-057: `Scripts/AlpineVMs/ng_elc.py`

**Branch history:** `AIOPT1` and `AIOPT2` are deleted from the remote. Existing mentions are historical; this clone still has stale local/tracking refs for `AIOPT1`, while active release work remains exclusively on `AIOPT3`.

## 1. Problema

O projecto contém tarefas Obsidian, SPECs, resultados de ensaios, ferramentas e decisões que avançaram em ritmos diferentes. Um novo agente precisa actualmente de reconstruir manualmente a relação entre esses registos. Isso cria risco de repetir trabalho, tratar um resultado parcial como aceitação ou misturar optimizações com o candidato da primeira release.

O documento existente `Specs/RELEASE-READINESS-v1.0.0.md` é uma base útil, mas ainda não funciona como matriz de rastreabilidade nem é validado automaticamente contra o código e os artefactos.

## 2. Objectivo

Criar um plano mestre humano e uma auditoria read-only determinística que controlem a Release `v1.0.0` através de gates verificáveis, relacionando tarefas, SPECs, experiências, ferramentas, evidências, configuração, commit e revisão.

## 3. Princípio de autoridade

A hierarquia de fontes é:

1. código e estado Git real para implementação;
2. SPEC activa para contrato e critérios técnicos;
3. manifesto/resultado bruto para o que foi observado;
4. tarefa Obsidian para ownership, escopo e próximo passo;
5. plano mestre para índice, dependências e decisão de release;
6. Dashboard para navegação, nunca como fonte única de aceitação;
7. skills para procedimento, nunca como prova do produto.

Nenhuma fonte pode transformar sozinha um estado não demonstrado em `PASS`.

## 4. Âmbito da Release

### Incluído

- baseline e proveniência relevantes de NG-020, NG-036 e NG-037;
- hygiene/provenance de NG-038;
- scripts e deployment de NG-046;
- runner, observabilidade e teardown de NG-049;
- correctness de subscriptions de NG-047/NG-048;
- gates PGCS de SPEC-047/048;
- gates NRNCS e Repository de SPEC-049/050/051;
- reteste normal de payload com workload novo e hashes correlacionados;
- revisão Astra, release notes e tag verificável.

### Excluído da Release 1.0.0

- implementação de optimizações NG-041, NG-043, NG-044 e NG-045;
- alterações experimentais de performance no candidato;
- criação ou promoção de AIOPT4;
- qualquer gate não relacionado com o âmbito acima, salvo decisão explícita.

NG-040 pode produzir o baseline de referência, sem misturar alterações de performance no commit candidato.

## 5. Vocabulário de veredictos

- `PASS`: critério demonstrado com evidência directa, reprodutível e ligada ao commit/configuração.
- `OPEN`: critério ainda não demonstrado.
- `BLOCKED`: pré-condição, aprovação ou configuração impede execução.
- `INCONCLUSIVE`: o ensaio ocorreu, mas o oráculo não permite conclusão.
- `FAIL`: o comportamento esperado foi testado e não ocorreu.
- `N/A`: fora do âmbito, com justificação explícita e aprovação quando necessário.

`concluída` numa tarefa ou `Implemented` numa SPEC não substitui o veredicto do gate.

## 6. Gates

| Gate | Objectivo | Tarefas | SPECs | Pré-requisito | Estado inicial |
|---|---|---|---|---|---|
| G0 | Âmbito, baseline e freeze | NG-050; NG-020; NG-037 | SPEC-033, SPEC-037 | nenhum | OPEN |
| G1 | Hygiene, segurança e provenance | NG-038 | SPEC-038 | G0 | OPEN |
| G2 | Scripts de deployment reproduzíveis | NG-046 | SPEC-009 | G0 | OPEN |
| G3 | Runner, observabilidade, modo local, teardown e canonicalidade do entrypoint | NG-049, NG-055, NG-056, NG-057 | SPEC-046, SPEC-052, SPEC-053, SPEC-055, SPEC-056, SPEC-057 | G2 | OPEN |
| G4 | PGCS on-wire e activação | NG-051 | SPEC-047 | G3 | parcial |
| G5 | Processamento receptor PGCS | NG-052 | SPEC-048 | G4 | parcial |
| G6 | NRNCS readiness e storage invocation | NG-053 | SPEC-049 | G5 | parcial |
| G7 | Discovery/readback Repository | NG-054 | SPEC-050, SPEC-051 | G6 | OPEN |
| G8 | Correctness de subscriptions | NG-047, NG-048 | SPEC-044, SPEC-045 | G3 | OPEN |
| G9 | Payload normal de 100 fotos segundo o modelo de cache | NG-050 (controlo) | — | G6, G7, G8 | OPEN |
| G10 | Revisão final e tag | NG-050 | release criteria | G1–G9 | BLOCKED |

Os gates G4–G7 têm agora tarefas Obsidian formais NG-051–NG-054, cada uma com SPEC e evidência próprios. A aceitação continua independente e não deve ser inferida a partir de um gate vizinho.

## 7. Matriz de rastreabilidade mínima

Cada linha de gate deve manter:

```text
Gate ID
Tarefa(s)
SPEC(s)
Objectivo/oráculo
Pré-condições
Commit exacto
Configuração e perfil
Ferramenta/skill usada
Caminho de evidência
Resultado observado
Veredicto
Revisor
Próximo passo/bloqueio
```

O plano mestre mantém apenas estes campos. Detalhes de implementação ficam na SPEC; logs, capturas e hashes ficam em `Specs/RESULTS-*`; decisões operacionais ficam nas tarefas/decisions.

## 8. Auditoria determinística

Criar `Scripts/ProjectAudit/ng_release_audit.py`, usando apenas Python standard library, com saída humana e JSON.

A auditoria deve:

1. ler o plano mestre;
2. resolver links para tarefas e SPECs;
3. validar frontmatter essencial das tarefas;
4. validar status, branch e implementation commit das SPECs;
5. detectar checkboxes abertas em critérios activos;
6. confirmar existência de commits e sua relação com `HEAD`;
7. detectar árvore dirty e ficheiros não rastreados relevantes;
8. resolver links de evidência e manifests;
9. verificar referências ghost a SPECs;
10. detectar divergência entre Dashboard, tarefa e SPEC;
11. identificar gates sem tarefa formal;
12. emitir `PASS`, `OPEN`, `BLOCKED`, `INCONCLUSIVE` ou `DRIFT` sem alterar os ficheiros.

A ferramenta não executa alterações de produção, não cria commits e não marca tarefas automaticamente.

## 9. Critérios de aceitação

- [x] O plano mestre implementa o âmbito e os gates definidos nesta SPEC.
- [x] A matriz cobre todas as tarefas/SPECs incluídas ou regista a lacuna.
- [x] A auditoria resolve links válidos e reporta links inexistentes.
- [x] A auditoria detecta status drift entre tarefa, SPEC e evidência.
- [x] A auditoria detecta implementation commit ausente, inválido ou não ancestral do candidato.
- [x] A auditoria detecta worktree dirty e separa alterações rastreadas de artefactos não rastreados.
- [x] A auditoria detecta referências ghost e referências AIOPT2 que não estejam explicitamente históricas.
- [x] A auditoria valida caminhos de evidência sem considerar existência de ficheiro como prova de conteúdo.
- [x] A auditoria produz JSON estável e resumo humano reproduzível.
- [x] Testes cobrem parsing, estados, links, commits e drift.
- [x] A execução contra o estado actual encontra pelo menos os drift já conhecidos.
- [x] A ferramenta permanece read-only por omissão.
- [x] O plano descreve o procedimento de início/fecho de sessão para novos agentes.
- [x] AIOPT4 e as optimizações permanecem fora do âmbito da release.
- [ ] Astra revê o plano, a ferramenta e o primeiro relatório antes de usar os resultados para aceitar um gate.

## 10. Resultado da implementação inicial

Em 2026-09-13 foram implementados o plano mestre e o auditor read-only. A execução real contra o repositório actual produziu:

- `21` testes locais passados (`5` do auditor e `16` do runner existente);
- `py_compile` e `bash -n` passados;
- `11` gates lidos do plano;
- veredicto inicial `BLOCKED` por `19` problemas; após correcção do parser/mapa, a reauditoria permaneceu `BLOCKED` com `18` problemas bloqueantes;
- detecção dos drift conhecidos: árvore dirty, SPECs abertas, SPEC-036 sem commit registado, NG-049 com metadata legacy, referências ghost `SPEC-015`/`SPEC-016` e referência AIOPT2 fora de documento histórico.

O resultado não é aceitação da release. A aceitação desta SPEC permanece aberta até revisão Astra e reconciliação explícita dos registos.

Após a reconciliação de tarefas e a correcção do scanner para ignorar menções históricas, a execução final encontrou `16` problemas bloqueantes: `15` SPECs abertas e uma árvore Git dirty. Não foram encontrados ciclos nas dependências NG.

## 11. Método experimental

Para cada gate futuro:

1. declarar a hipótese antes do ensaio;
2. fixar commit, configuração, workload e ferramenta;
3. executar o menor cenário capaz de falsificar a hipótese;
4. preservar saída bruta, manifestos e hashes;
5. avaliar o oráculo específico do gate;
6. classificar o resultado sem inferir gates superiores;
7. actualizar tarefa e plano mestre apenas com evidência verificável;
8. pedir revisão antes de avançar para o próximo gate.

## 12. Integração Hermes

No início de uma sessão NovaGenesis, o agente deve consultar:

1. plano mestre;
2. `git status` e commit actual;
3. primeiro gate `OPEN`/`BLOCKED`;
4. tarefa Obsidian ligada;
5. SPEC e evidência desse gate;
6. skill aplicável.

No fecho, deve executar a validação relevante e o `consistency-check`. O plano mestre não substitui `session_search`, o Dashboard ou as notas de tarefa; ele evita que tenham de ser correlacionados manualmente a cada sessão.

## 13. Rollback

As alterações desta SPEC são documentais e de auditoria. Devem ser revertíveis por commits separados. A ferramenta não pode modificar o código NovaGenesis, tarefas ou SPECs durante uma execução normal.

## 14. Riscos

| Risco | Mitigação |
|---|---|
| Plano mestre duplica todo o projecto | Manter apenas índices, gates e veredictos |
| Auditoria passa por presença de ficheiros | Exigir manifesto, hash, commit e oráculo |
| Status manual fica novamente stale | Executar auditoria no início e no fecho das sessões |
| Novo agente ignora o plano | Integrar leitura no procedimento de session-start |
| LLM transforma plausibilidade em prova | Manter evidência determinística como autoridade |
| Ferramenta torna-se específica demais | Separar parser genérico de regras NovaGenesis |
