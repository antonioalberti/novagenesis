# PLAN-CONSOLIDATION-2026-07-16: Documentação NovaGenesis Consistente com HEAD (AIOPT3)

**Data:** 2026-07-16  
**Base:** Commit `cd804a7` (branch AIOPT3) — "Disable all DEBUG defines and remove verbose MAX_MESSAGES_IN_MEMORY log"  
**Objetivo:** Toda a documentação (Specs/, Docs/, Issues/, Skills/) consistente com a realidade do código em HEAD.

---

## 1. CONTEXTO ACTUAL (Snapshot do HEAD)

| Item | Estado em `cd804a7` (AIOPT3) |
|------|-------------------------------|
| **Branch activo** | `AIOPT3` (up-to-date com origin) |
| **DEBUG** | Desactivado em todos os 39 ficheiros `.cpp` (`//#define DEBUG`) |
| **NRInfoPayload01** | Cache em disco (SPEC-022 canonical) — NÃO reencaminha |
| **NRSubBind01 / PSSubBind01** | Uma mensagem por key (SPEC-021 implementado) |
| **ContentApp Expoição Periódica** | Implementado (SPEC-011, commit 164cc49) |
| **NGAL (SPEC-013/014)** | Implementado — hash mismatch corrigido |
| **GCC 15 thread fix** | Aplicado em `Process.cpp:1031` (`tthread::thread`) — SPEC-MUSL-001 |
| **Alpine VM SSH scripts** | Implementados (E1-E6) |
| **ContentPublish Throttling** | SPEC-023 proposta (pendente aplicação) |

---

## 2. PROBLEMAS IDENTIFICADOS NAS SPECs (24 ficheiros)

### 2.1 Referências AIOPT2 desactualizadas (CRÍTICO)

| SPEC | Linhas | Ação |
|------|--------|------|
| SPEC-001-NRNCS-debug-logging.md | L7 | `Branch: AIOPT2` → `Branch: AIOPT3` |
| SPEC-009-alpine-vm-ssh-run-scripts.md | L6, L65, L233 | `AIOPT2` → `AIOPT3` (3 ocorrências) |
| SPEC-011-periodic-exposition-contentapp.md | L127 | `git push origin AIOPT2` → `git push origin AIOPT3` |

### 2.2 Status inconsistente com realidade (CRÍTICO)

| SPEC | Status actual | Realidade em HEAD | Nova status |
|------|---------------|-------------------|-------------|
| SPEC-001-NRNCS-debug-logging.md | Draft | Implementado (DEBUG off em todos ficheiros) | **Implementado** |
| SPEC-013-ngal-adaptation-layer.md | Proposal | Código implementado (commit 164cc49 + NGAL) | **Implementado** |
| SPEC-014-ngal-hash-mismatch-diagnosis.md | In Progress | Fix aplicado, compilado, pendente teste E2E | **Implementado — Pendente Teste** |
| SPEC-MUSL-001-gcc15-thread-ambiguity.md | Draft | Fix aplicado em AIOPT3 (`tthread::thread`) | **Implementado** |

### 2.3 Modelo de forwarding desactualizado (ALTO)

| SPEC | Problema | Ação |
|------|----------|------|
| SPEC-018-payload-sticks-on-reused-inline-response.md | Descreve forwarding do NRInfoPayload01 — modelo antigo | **Marcar SUPERSEDED** → ver SPEC-022 canonical |
| SPEC-019-payload-hash-logging.md | Diagrama L31: "NRNCS forwarding payload" — já não existe | Actualizar diagrama para cache model |
| SPEC-021-one-message-per-file.md | Premissa errada para AIOPT3 (assume forwarding) | **Marcar SUPERSEDED** |
| SPEC-023-contentpublish-burst-throttling.md | L299: referência ambígua a "SPEC-022" (qual dos dois?) | Especificar `SPEC-022-nrinfopayload01-separate-messages.md` |

### 2.4 Referências dangling a SPEC-015/016 (ALTO — 6 SPECs)

SPEC-015 e SPEC-016 **não existem**. Remover referências:

| SPEC | Linhas |
|------|--------|
| SPEC-014-ngal-hash-mismatch-diagnosis.md | L6-7 |
| SPEC-017-content-name-correlation.md | L6 |
| SPEC-018-payload-sticks-on-reused-inline-response.md | L6 |
| SPEC-019-payload-hash-logging.md | L6 |
| SPEC-020-subscription-redelivery-fix.md | L6 |
| SPEC-021-one-message-per-file.md | L6 |

### 2.5 Outros problemas (MÉDIO)

| SPEC | Problema | Ação |
|------|----------|------|
| SPEC-010-content-name-correlation.md | Status Draft (nunca implementado), ref ambígua a SPEC-001 (2 ficheiros) | Marcar **Abandonado** ou **Superseded** |
| SPEC-008-ht-bid-raw-socket-discovery.md | Status Draft — Rev2 (nunca fechado) | Revisar status |
| SPEC-012-hash-abstraction.md | E5 e E11 duplicados; código exemplo usa PGIDGenerator em vez de NameGenerator | Corrigir duplicados e exemplo |
| SPEC-017-content-name-correlation.md | Fix proposta nunca implementada, possivelmente subsumida por SPEC-021 | Marcar **Superseded by SPEC-021** |

---

## 3. PLANO DE ACÇÃO — FASES

### FASE 1: Limpeza de SPECs (Prioridade: CRÍTICA)

#### 1.1 Actualizar referências AIOPT2 → AIOPT3
- [ ] SPEC-001-NRNCS-debug-logging.md: L7
- [ ] SPEC-009-alpine-vm-ssh-run-scripts.md: L6, L65, L233
- [ ] SPEC-011-periodic-exposition-contentapp.md: L127

#### 1.2 Corrigir status vs realidade
- [ ] SPEC-001-NRNCS-debug-logging.md: Draft → **Implementado** + nota "DEBUG off em produção"
- [ ] SPEC-013-ngal-adaptation-layer.md: Proposal → **Implementado** + commit ref
- [ ] SPEC-014-ngal-hash-mismatch-diagnosis.md: In Progress → **Implementado — Pendente Teste E2E**
- [ ] SPEC-MUSL-001-gcc15-thread-ambiguity.md: Draft → **Implementado** + commit ref

#### 1.3 Marcar SPECs supersedidas/abandonadas
- [ ] SPEC-018-payload-sticks-on-reused-inline-response.md → **SUPERSEDED** (ver SPEC-022 canonical)
- [ ] SPEC-021-one-message-per-file.md → **SUPERSEDED** (modelo forwarding obsoleto)
- [ ] SPEC-010-content-name-correlation.md → **ABANDONADO** (nunca implementado)
- [ ] SPEC-017-content-name-correlation.md → **SUPERSEDED BY SPEC-021**

#### 1.4 Remover referências dangling SPEC-015/016
- [ ] SPEC-014: L6-7
- [ ] SPEC-017: L6
- [ ] SPEC-018: L6
- [ ] SPEC-019: L6
- [ ] SPEC-020: L6
- [ ] SPEC-021: L6

#### 1.5 Corrigir SPEC-023 referência ambígua
- [ ] L299: "SPEC-022" → `SPEC-022-nrinfopayload01-separate-messages.md`

#### 1.6 Corrigir SPEC-012 duplicados e exemplo
- [ ] Remover E5/E11 duplicados
- [ ] Substituir `PGIDGenerator` por `NameGenerator` no exemplo

#### 1.7 Actualizar SPEC-019 diagrama
- [ ] L31: "NRNCS forwarding payload" → "NRNCS cached payload"

#### 1.8 Revisar SPEC-008 status
- [ ] Decidir: Implementado? Abandonado? Mover para Issues/ se for bug histórico

---

### FASE 2: Reorganizar Pastas Docs/ e Issues/ (Prioridade: ALTA)

#### 2.1 Docs/ — Estrutura alvo
```
/home/gandalf/workspace/novagenesis/Docs/
├── ARCHITECTURE/
│   ├── NG-INVERTED-PUB-SUB-MODEL.md          (existente — manter)
│   └── NGAL-ARCHITECTURE.md                  (NOVO — extrair de SPEC-013 E1-E3)
├── DIAGNOSTICS/
│   ├── PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md  (existente)
│   ├── CONTENTAPP-TIMERS-INVESTIGATION-2026-06-04.md    (existente)
│   └── NGAL-HASH-MISMATCH-ROOT-CAUSE.md    (NOVO — extrair de SPEC-014 secção 2)
├── DECISIONS/
│   ├── REVERT.md                             (existente — manter)
│   └── SPEC-STATUS-REGISTER.md               (NOVO — tabela mestra de todas SPECs)
└── HISTORICAL/
    └── (mover docs antigos/obsoletos para aqui)
```

#### 2.2 Issues/ — Estrutura alvo
```
/home/gandalf/workspace/novagenesis/Issues/
├── OPEN/
│   └── ISSUE-001-nrinfopayload01-cache-miss.md  (existente)
├── CLOSED/
│   ├── ISSUE-002-ngal-hash-mismatch.md          (NOVO — fechar com SPEC-014)
│   ├── ISSUE-003-gcc15-thread-ambiguity.md      (NOVO — fechar com SPEC-MUSL-001)
│   └── ISSUE-004-nrncs-debug-logging.md         (NOVO — fechar com SPEC-001)
└── TEMPLATE.md                                  (NOVO — template padrão)
```

#### 2.3 Ações
- [ ] Criar `Docs/ARCHITECTURE/NGAL-ARCHITECTURE.md` (extrair SPEC-013 E1-E3)
- [ ] Criar `Docs/DIAGNOSTICS/NGAL-HASH-MISMATCH-ROOT-CAUSE.md` (extrair SPEC-014 secção 2)
- [ ] Criar `Docs/DECISIONS/SPEC-STATUS-REGISTER.md` (tabela mestra de 24 SPECs)
- [ ] Criar Issues fechados para bugs resolvidos
- [ ] Mover docs obsoletos para `Docs/HISTORICAL/`
- [ ] Criar `Issues/TEMPLATE.md`

---

### FASE 3: Alinhar Skills NovaGenesis (Prioridade: ALTA)

#### 3.1 Skills existentes em `~/.hermes/skills/novagenesis*/`
| Skill | Estado | Ação |
|-------|--------|------|
| `novagenesis/ng-inverted-pub-sub` | Existe | Verificar vs Docs/ARCHITECTURE/NG-INVERTED-PUB-SUB-MODEL.md |
| `novagenesis-dev` | Existe (SKILL.md 100KB) | **Refactoring necessário** — muito grande, mistura dev workflow + docs |
| `novagenesis-debug` | Existe | Verificar se ainda relevante pós-NGAL |
| `novagenesis-intra-domain-routing` | Existe | Verificar |

#### 3.2 Nova estrutura proposta
```
~/.hermes/skills/
├── novagenesis-architecture/       # NGAL, inverted pub/sub, NGAL-SAR/CS/T
├── novagenesis-build-deploy/       # compile, pull-and-build-vms, Alpine VM scripts
├── novagenesis-debug/              # logging, debug defines, gdb patterns
├── novagenesis-spec-workflow/      # como escrever/atualizar SPECs, status transitions
└── novagenesis-git-workflow/       # branches (AIOPT3), commits (EN), tags, PRs
```

#### 3.3 Ações
- [ ] Ler `novagenesis-dev/SKILL.md` e `novagenesis-debug/SKILL.md` na íntegra
- [ ] Extrair secções de arquitetura → `novagenesis-architecture`
- [ ] Extrair build/deploy → `novagenesis-build-deploy`
- [ ] Extrair debug → `novagenesis-debug` (limpo)
- [ ] Criar `novagenesis-spec-workflow` (novo)
- [ ] Criar `novagenesis-git-workflow` (novo)
- [ ] Arquivar `novagenesis-dev` (muito grande) → mover conteúdo relevante

---

### FASE 4: Git Workflow & Convenções (Prioridade: MÉDIA)

#### 4.1 Regras obrigatórias (já no memory, reforçar)
- [ ] **Branch**: `AIOPT3` (actual) — não mais `AIOPT2`
- [ ] **Commits**: SEMPRE em inglês (user correction 2026-07-15)
- [ ] **Tags**: `v<major>.<minor>.<patch>-<branch>` ex: `v0.3.0-AIOPT3`
- [ ] **SPECs**: Status em inglês (Draft, Proposal, In Progress, Implemented, Superseded, Abandoned)

#### 4.2 Scripts de validação
- [ ] Criar `Scripts/Simple/validate-specs.sh` — verifica status vs git log, refs dangling
- [ ] Criar `Scripts/Simple/validate-skills.sh` — verifica skills vs docs

---

### FASE 5: Limpeza de Ficheiros Órfãos / Duplicados (Prioridade: BAIXA)

- [ ] `SPEC-022-one-file-per-message-nrinfopayload01.md` → mover para `Docs/HISTORICAL/` (já marcado SUPERSEDED)
- [ ] Verificar se há SPECs duplicados com nomes ligeiramente diferentes
- [ ] Limpar `Temp/`, `Logs/`, `Plots/` se não necessários no repo

---

## 4. CRONOGRAMA SUGERIDO

| Fase | Esforço estimado | Prioridade |
|------|------------------|------------|
| 1. Limpeza SPECs | 2-3h | CRÍTICA |
| 2. Reorganizar Docs/Issues | 2h | ALTA |
| 3. Refactoring Skills | 3-4h | ALTA |
| 4. Git Workflow/Scripts | 1h | MÉDIA |
| 5. Limpeza órfãos | 30min | BAIXA |
| **TOTAL** | **~8-10h** | |

---

## 5. ORDEM DE EXECUÇÃO RECOMENDADA

1. **Fase 1.1-1.5** (editar 8 ficheiros SPEC — referências AIOPT2, status, superseded, dangling refs)
2. **Fase 1.6-1.8** (corrigir SPEC-012, SPEC-019, SPEC-008)
3. **Fase 2** (criar Docs/ e Issues/ estrutura, extrair conteúdo)
4. **Fase 3** (ler skills atuais, criar novas, arquivar velha)
5. **Fase 4** (scripts validação, convenções)
6. **Fase 5** (limpeza final)

---

## 6. COMANDOS ÚTEIS PARA EXECUÇÃO

```bash
# Ver todas as SPECs
ls -1 Specs/*.md | sort

# Grepar referências AIOPT2
grep -rn "AIOPT2" Specs/

# Grepar referências SPEC-015/016
grep -rn "SPEC-01[56]" Specs/

# Ver status atuais
grep -h "^Status:\|^Estado:" Specs/*.md | sort | uniq -c

# Ver branch actual
git branch --show-current

# Ver commits recentes
git log --oneline -10
```

---

## 7. CRITÉRIOS DE SUCESSO (Definition of Done)

- [ ] **0 referências a AIOPT2** em Specs/
- [ ] **0 referências a SPEC-015/016** em Specs/
- [ ] **Todas as 24 SPECs** com status consistente com HEAD (Implementado/Superseded/Abandoned)
- [ ] **SPEC-STATUS-REGISTER.md** existe e cobre todas as 24 SPECs
- [ ] **Docs/** reorganizado em ARCHITECTURE/, DIAGNOSTICS/, DECISIONS/, HISTORICAL/
- [ ] **Issues/** reorganizado em OPEN/, CLOSED/, TEMPLATE.md
- [ ] **Skills** refactored em 5 skills focadas (architecture, build-deploy, debug, spec-workflow, git-workflow)
- [ ] **validate-specs.sh** passa sem erros
- [ ] **Commits em inglês** a partir de agora

---

## 8. NOTAS PARA O EXECUTOR

1. **Não alterar código C++** — apenas documentação e skills
2. **Um commit por fase** (ou sub-fase lógica) com mensagens em inglês
3. **Testar validate-specs.sh** após Fase 1
4. **Ler skills existentes** antes de refactoring — há conhecimento valioso nelas
5. **Manter SPEC-022-one-file-per-message...** em HISTORICAL para rastreabilidade

---

*Documento criado em 2026-07-16 como guia de consolidação pós-análise das 24 SPECs.*