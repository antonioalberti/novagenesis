# SPEC-057: NG-ELC canonical entrypoint rename

**Author:** Antonio Alberti / Hermes Agent  
**Date:** 2026-09-13  
**Status:** Proposal  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-052-remote-ssh-multi-vm-executor.md; SPEC-053-debug-log-observability-matrix.md; SPEC-054-release-master-plan-and-audit.md; SPEC-055-ng-elc-local-mode.md; SPEC-056-ng-elc-evidence-provenance-hardening.md  
**Task linkage:** NG-057 (child of NG-050; depends on NG-056)  
**Current entrypoint:** `Scripts/AlpineVMs/ng_remote_executor.py`  
**Proposed canonical entrypoint:** `Scripts/AlpineVMs/ng_elc.py`

## 1. Problema e autoridade

O componente deixou de ser apenas um executor remoto. O mesmo controlador coordena preflight, launch, readiness, observação, oracle, teardown, ownership e evidência nos modos local e remoto. O nome `ng_remote_executor.py` já não descreve o contrato actual e induz a interpretação de que o modo remoto é o caminho canónico.

A nomenclatura conceptual autorizada permanece:

```text
NG Experiment Lifecycle Controller (NG-ELC)
```

A sigla `NG-EPC` não será introduzida. O rename é de nomenclatura e organização do entrypoint; não altera o comportamento do controller, o protocolo, o wire format ou os modos de execução.

A migração afecta referências normativas em SPEC-046, SPEC-052 e SPEC-053, além de SPEC-054/055/056, README e tarefas. Bundles históricos e provenance já selada permanecem imutáveis; a auditoria deve separar essas referências históricas do novo entrypoint normativo.

## 2. Resultado pretendido

Adoptar `Scripts/AlpineVMs/ng_elc.py` como único entrypoint canónico do NG-ELC, mantendo uma migração compatível e verificável para consumidores legados durante o período definido nesta SPEC.

## 3. Âmbito e não-goals

### Incluído

- Renomear a implementação Python para `Scripts/AlpineVMs/ng_elc.py`.
- Actualizar imports, comandos, README, SPECs, tarefas, skills e referências de evidência.
- Manter temporariamente `ng_remote_executor.py` apenas como shim de compatibilidade, sem lógica duplicada, se consumidores existentes ainda o exigirem.
- Fazer o shim delegar para o entrypoint canónico e preservar argumentos, exit codes e stdout/stderr.
- Adicionar testes de equivalência do entrypoint canónico e do shim.
- Actualizar provenance/manifests para identificar o caminho canónico e o estado de compatibilidade.
- Executar auditoria de referências para garantir que o nome antigo não permanece como entrypoint normativo.

### Excluído

- Alterações C++, protocolo, wire format, pub/sub ou comportamento de runtime.
- Alterações ao contrato remoto ou ao modo local.
- Alterações ao hardening funcional da SPEC-056, excepto o necessário para imports e provenance do caminho.
- Commit, tag, push ou criação de AIOPT4 nesta SPEC.
- Remoção do shim sem evidência de que não existem consumidores activos.

## 4. Decisões de nomenclatura

| Item | Decisão |
|---|---|
| Nome conceptual | `NG Experiment Lifecycle Controller (NG-ELC)` |
| Módulo Python canónico | `ng_elc.py` |
| Nome legado | `ng_remote_executor.py`, apenas shim durante migração |
| `NG-EPC` | Não adoptar; não corresponde ao nome conceptual aprovado |
| Entry point normativo em documentação nova | `Scripts/AlpineVMs/ng_elc.py` |
| Compatibilidade | Shim sem lógica própria, com teste de equivalência |

`ng_elc.py` usa underscore porque é importável como módulo Python. Um nome executável com hífen não é necessário e dificultaria imports directos nos testes.

## 5. Requisitos normativos

### R01 — Canonicalidade

1. A implementação do controller deve existir em `Scripts/AlpineVMs/ng_elc.py`.
2. Documentação e comandos novos devem referir `ng_elc.py`.
3. `ng_remote_executor.py`, se mantido, deve conter apenas delegação para `ng_elc.py` e aviso de compatibilidade.
4. Não pode existir uma segunda cópia funcional do controller.

### R02 — Compatibilidade

1. O shim deve aceitar os mesmos argumentos do entrypoint canónico.
2. O shim deve preservar exit codes, stdout, stderr e códigos de erro de configuração.
3. O shim não deve alterar ambiente, paths, timeouts, sinais ou teardown.
4. A compatibilidade não se aplica a imports internos do módulo antigo depois da migração dos testes.

### R03 — Integridade de referências

1. Imports dos testes devem usar `ng_elc`.
2. README, SPECs activas, plano mestre, tarefas e skills devem usar o caminho canónico.
3. Referências ao nome antigo devem estar limitadas ao próprio shim, ao histórico e à documentação de migração.
4. Auditoria deve detectar referências normativas residuais ao nome antigo.

### R04 — Provenance

1. O controller deve registar o caminho canónico e o SHA-256 do ficheiro executado.
2. O shim deve ser distinguível do controller canónico no provenance.
3. O rename não pode criar ambiguidade sobre qual ficheiro produziu uma evidência.
4. Bundles históricos não devem ser reescritos; referências novas devem apontar para o caminho canónico.

### R05 — Verificação

A aceitação exige, no mínimo:

- teste canónico de import e `--help`;
- teste de equivalência canónico/shim;
- suite completa do NG-ELC sem regressões;
- `py_compile` dos dois caminhos;
- auditoria de referências sem entrypoint normativo antigo fora da migração;
- `git diff --check`;
- revisão Astra do rename e das referências;
- confirmação de que SPEC-056 continua semanticamente inalterada.

## 6. Plano de migração

1. Manter esta SPEC como `Proposal` até a decisão de implementação.
2. Criar teste RED para localizar e validar o entrypoint canónico e o shim.
3. Mover a implementação para `ng_elc.py` sem alterações funcionais.
4. Criar o shim mínimo `ng_remote_executor.py`.
5. Migrar imports e referências normativas.
6. Executar testes, compilação e auditoria de referências.
7. Obter revisão Astra contra os ficheiros completos e resultados reais.
8. Só então considerar a transição da SPEC para `Implemented`, condicionada a commit identificável e evidência completa.

## 7. Critérios de não aceitação

A SPEC permanece aberta se ocorrer qualquer uma destas condições:

- comportamento divergente entre shim e entrypoint canónico;
- referências normativas novas ao nome antigo;
- duas implementações funcionais;
- provenance incapaz de distinguir shim e controller;
- alterações acidentais ao modo local/remoto;
- regressão na suite ou nos exit codes;
- worktree/release hygiene incompatível com o plano mestre.

## 8. Evidência esperada

```text
Specs/RESULTS-SPEC-057/
├── entrypoint-equivalence.json
├── reference-audit.json
├── test-results.txt
├── provenance-rename.json
└── astra-review.md
```

Nenhum resultado histórico será sobrescrito. A evidência nova deve usar caminho absoluto dentro da árvore do repositório e ser ligada à tarefa NG-057.

## 9. Estado inicial

- O nome conceptual `NG-ELC` já é usado nas SPECs e tarefas activas.
- O caminho antigo aparece em comandos, imports, documentação e provenance existentes.
- A implementação actual continua sob `Scripts/AlpineVMs/ng_remote_executor.py`.
- A SPEC-056 permanece `In Progress` e a Release 1.0.0 permanece `BLOCKED`.
- A implementação desta SPEC deve começar apenas após a estabilização/revisão da SPEC-056, para não misturar rename com hardening funcional.
