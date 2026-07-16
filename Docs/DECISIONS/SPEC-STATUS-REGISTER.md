# SPEC-STATUS-REGISTER.md — Mestra de todas as SPECs NovaGenesis

**Base:** Commit `cd804a7` (branch AIOPT3) — 2026-07-16  
**Total:** 24 SPECs em `Specs/`

---

## Legenda de Status

| Status | Significado |
|--------|-------------|
| **Implemented** | Código em HEAD, testado (ou compilado e deployado) |
| **Implemented — Pending Test** | Código em HEAD, compila, teste E2E pendente |
| **Proposal** | Apenas design, sem implementação |
| **Draft** | Rascunho inicial, não validado |
| **Superseded** | Substituída por SPEC posterior (referência dada) |
| **Abandoned** | Nunca implementada, sem substituta |

---

## Tabela Mestra

| SPEC | Título | Status | Branch | Commit ref | Notas |
|------|--------|--------|--------|------------|-------|
| SPEC-001 | Hello IPC HT Bid | ✅ **Implemented** | AIOPT3 | cd804a7 | Base functionality |
| SPEC-001-NRNCS-debug-logging | Comprehensive Debug Logging NRNCS | ✅ **Implemented** | AIOPT3 | cd804a7 | DEBUG off in all 39 files |
| SPEC-002 | Message Leak Fix | ✅ **Implemented** | AIOPT3 | — | |
| SPEC-003 | Fix Use-After-Free & Scheduled Messages | ✅ **Implemented** | AIOPT3 | — | |
| SPEC-004 | ContentApp Performance Optimisation | ✅ **Implemented** | AIOPT3 | — | |
| SPEC-005 | GW SHM Poll Interval Reduction | ✅ **Implemented** | AIOPT3 | — | |
| SPEC-006 | Output Queue Semaphore Contention | ✅ **Implemented** | AIOPT3 | — | |
| SPEC-007 | Output Queue Lock Decoupling | ✅ **Implemented** | AIOPT3 | — | |
| SPEC-008 | HT Bid Raw Socket Discovery | 📝 **Draft** | — | — | Never closed (Rev2) |
| SPEC-009 | Alpine VM SSH Run Scripts | ✅ **Implemented** | AIOPT3 | — | E1-E6 scripts created |
| SPEC-010 | Content Name Correlation | ❌ **Abandoned** | — | — | Never implemented, ambiguous SPEC-001 ref |
| SPEC-011 | Periodic Exposition ContentApp | ✅ **Implemented** | AIOPT3 | 164cc49 | Periodic exposition in CoreRunPeriodic01 |
| SPEC-012 | Hash Abstraction | 📝 **Draft** | — | — | Duplicate E5/E11, PGIDGenerator example |
| SPEC-013 | NGAL Adaptation Layer | ✅ **Implemented** | AIOPT3 | 164cc49+ | NGAL-SAR/CS/T implemented |
| SPEC-014 | NGAL Hash Mismatch Diagnosis | ✅ **Implemented — Pending Test** | AIOPT3 | — | Root cause fixed (double deserialization + data race) |
| SPEC-015 | (Does not exist) | — | — | — | **DANGLING REF** |
| SPEC-016 | (Does not exist) | — | — | — | **DANGLING REF** |
| SPEC-017 | Content Name Correlation | 🔄 **Superseded by SPEC-021** | — | — | Fix proposed, never implemented |
| SPEC-018 | Payload Sticks on Reused InlineResponse | 🔄 **Superseded** | — | — | Forwarding model obsolete → SPEC-022 canonical |
| SPEC-019 | Payload Hash Logging | ✅ **Implemented** | AIOPT3 | — | ContentApp + NRNCS logging active |
| SPEC-020 | Subscription Re-Delivery Fix | ✅ **Implemented** | AIOPT3 | — | ContentApp fix applied |
| SPEC-021 | One Message Per File | 🔄 **Superseded** | — | — | Forwarding model obsolete → SPEC-022 canonical |
| SPEC-022 | NRInfoPayload01 Separate Messages (CACHE MODEL) | ✅ **Implemented** | AIOPT3 | 7dad2b6 | **Canonical** — cache to disk, no forwarding |
| SPEC-022-one-file-per-message | One File Per Message (OLD FORWARDING) | 🔄 **Superseded** | — | — | Moved to HISTORICAL |
| SPEC-023 | ContentPublish Burst Throttling | 📝 **Proposal** | — | — | SPEC-021 dependency applied |
| SPEC-MUSL-001 | GCC 15 thread Ambiguity Fix | ✅ **Implemented** | AIOPT3 | cd804a7 | `tthread::thread` in Process.cpp:1031 |

---

## Referências Cruzadas — Dangling (REMOVER)

As seguintes SPECs referenciam **SPEC-015** e **SPEC-016** que **não existem**:

| SPEC | Linhas a corrigir |
|------|-------------------|
| SPEC-014 | L6-7 |
| SPEC-017 | L6 |
| SPEC-018 | L6 |
| SPEC-019 | L6 |
| SPEC-020 | L6 |
| SPEC-021 | L6 |

---

## Referências AIOPT2 (ACTUALIZAR → AIOPT3)

| SPEC | Linhas |
|------|--------|
| SPEC-001-NRNCS-debug-logging.md | L7 |
| SPEC-009-alpine-vm-ssh-run-scripts.md | L6, L65, L233 |
| SPEC-011-periodic-exposition-contentapp.md | L127 |

---

## Status a Actualizar

| SPEC | De | Para |
|------|-----|------|
| SPEC-001-NRNCS-debug-logging | Draft | **Implemented** |
| SPEC-013-ngal-adaptation-layer | Proposal | **Implemented** |
| SPEC-014-ngal-hash-mismatch-diagnosis | In Progress | **Implemented — Pending Test** |
| SPEC-MUSL-001 | Draft | **Implemented** |
| SPEC-018-payload-sticks... | Fix proposta | **Superseded** |
| SPEC-021-one-message-per-file | Proposal | **Superseded** |
| SPEC-010-content-name-correlation | Draft | **Abandoned** |
| SPEC-017-content-name-correlation | Fix proposta | **Superseded by SPEC-021** |
| SPEC-022-one-file-per-message... | (marcado) | **Superseded** (já está) |

---

## SPEC-023 Dependência

| SPEC | Dependência | Estado |
|------|-------------|--------|
| SPEC-023 | SPEC-021 | ✅ Applied |
| SPEC-023 | SPEC-022 | ✅ Applied (ou não — independente) |

---

## Acções Pendentes por SPEC

### SPEC-008
- [ ] Decidir: Implementado? Abandonado? Mover para Issues/ se bug histórico

### SPEC-012
- [ ] Remover E5/E11 duplicados
- [ ] Corrigir exemplo: `PGIDGenerator` → `NameGenerator`

### SPEC-019
- [ ] L31: "NRNCS forwarding payload" → "NRNCS cached payload"

### SPEC-023
- [ ] L299: "SPEC-022" → `SPEC-022-nrinfopayload01-separate-messages.md`

---

## Histórico de Versões deste Registo

| Data | Versão | Autor | Mudança |
|------|--------|-------|---------|
| 2026-07-16 | 1.0 | Hermes Agent | Criação baseada em auditoria 24 SPECs |

---

*Este ficheiro deve ser actualizado a cada mudança de status de SPEC. É a fonte de verdade para `validate-specs.sh`.*