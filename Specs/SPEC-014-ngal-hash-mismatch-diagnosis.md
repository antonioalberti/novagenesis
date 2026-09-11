# SPEC-014: NGAL Hash Mismatch — Diagnosis and Correction

**Author:** Hermes Agent (consolidation of SPEC-014 v1 + own analysis)  
**Date:** 09/07/2026  
**Revision:** 3  
**Status:** Implemented — Pending E2E Test  
**Branch:** AIOPT3  
**Fix commit:** Pending (after test)

---

## 1. Problema

Após a implementação da **SPEC-013 (NGAL)**, o ContentApp Repository falha a verificação de integridade em **TODAS** as fotos recebidas. O log mostra:

```
(ERROR: The hash of the file 00000-alpine-ng-source.jpg is not the same
than the one generated on the publisher. i.e. 581077ED)
```

Antes da SPEC-013, o pipeline SHM funcionava sem este erro.

---

## 2. Causa Raiz — Diagnóstico Definitivo

### 2.1 O bug: dupla deserialização + NewMessage em thread errada

A SPEC-013 introduziu um pipeline NGAL onde `NGAL_SAR::ReceiveFragment()` chama `PP->NewMessage()` na thread do `ReceiveDispatcher`, criando um objecto `Message*` completo (com `SetMessageFromCharArray` + `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2`). Depois, no GW thread, o mesmo buffer é deserializado **outra vez**.

```
PIPELINE ANTIGO (pré-NGAL) — 1 deserialização:
  SocketDispatcher3 → char buffer → SHM →
  GW::ReadFromSharedMemory3 → NewMessage + SetMessageFromCharArray + ConvertMessage → PushToInputQueue

PIPELINE COM BUG (SPEC-013) — 2 deserializações + data race:
  ReceiveDispatcher thread:
    NGAL_SAR::ReceiveFragment → PP->NewMessage() ← RACE! ←
      + SetMessageFromCharArray + ConvertMessage → Message* completo
    GetMessageFromCharArray → DeliverToGateway (queue de char buffers)
  GW thread:
    NewMessage + SetMessageFromCharArray + ConvertMessage → PushToInputQueue
```

### 2.2 Porque causa hash mismatch

A dupla deserialização por si NÃO corrompe os bytes (a função `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` é read-only em `Msg`). O problema real é:

1. **`PP->NewMessage()` é chamado na thread errada** — o `ReceiveDispatcher` corre numa thread separada e chama `PP->NewMessage()` no Process do PG. O GW thread também chama `NewMessage()` no mesmo Process. Isto é um **data race** não-determinista que pode corromper a estrutura interna do Process (lista de Messages, contadores, etc.).

2. **`MarkToDelete()` no Message criado no SAR** — Este Message é marcado para deleção pelo GW thread via `DeleteMarkedMessages()`, mas o GW thread não tem referência directa a este Message. O timing de deleção é imprevisível e pode libertar memória enquanto outro thread ainda a referencia.

3. **Violação do Finding F2 da SPEC-013** — A própria SPEC-013 já documentava: "Process::NewMessage() is NOT thread-safe — DeliverToGateway() sends char buffer, GW thread does NewMessage". A implementação VIOLOU este finding.

### 2.3 Por que falha em TODOS os ficheiros (não intermitente)

Embora o data race seja teoricamente não-determinista, o hash falha de forma consistente porque:
- O `NewMessage` na thread do receiver afecta sempre o estado do Process
- A corruptibilidade sistemática do Message/Process durante a deserialização no GW produz sempre payloads incorrectos
- O `MarkToDelete` + `DeleteMarkedMessages` pode libertar o Message do SAR antes de o buffer ser copiado para a queue

---

## 3. Correcção Implementada

### 3.1 Princípio: SAR devolve buffer bruto, GW faz a deserialização

O `NGAL_SAR::ReceiveFragment()` passa a devolver o **buffer bruto** (`char*` + `size`) em vez de criar um `Message*`. Isto elimina:
- A dupla deserialização
- O `NewMessage()` na thread errada (data race)
- O `MarkToDelete()` órfão
- A violação do Finding F2

### 3.2 Mudanças detalhadas

| Ficheiro | Mudança | Razão |
|----------|---------|-------|
| `Common/src/NGAL_SAR.h` | `ReceiveFragment(Process*, Message*&)` → `ReceiveFragment(char*&, long long&)`; forward declaration `class Message`; remoção de `#include "Process.h"` e `"Message.h"` | Interface limpa, sem dependência de Message/Process no header |
| `Common/src/NGAL_SAR.cpp` | Body de ReceiveFragment devolve `FB->Buffer` + `FB->MessageSize` em vez de `NewMessage + SetMessageFromCharArray + ConvertMessage + MarkToDelete`; adicionado `#include "Message.h"` e `"Process.h"` (necessários para SendSegmented) | Elimina dupla deserialização e data race |
| `PGCS/src/NGAL_Transport_RAW.cpp` | Chama `DeliverToGateway(CompletedBuffer, CompletedSize)` directamente; `delete[] CompletedBuffer` após entrega; remoção de includes desnecessários | Pipeline simplificado, buffer bruto entregue ao GW |
| `Common/src/GW.cpp` | **Sem mudança** — Step 3 continua igual (NewMessage + SetMessageFromCharArray + ConvertMessage — **uma única vez**, no GW thread) | Este é o caminho correcto (Finding F2) |

### 3.3 Pipeline corrigido

```
PIPELINE CORRIGIDO — 1 deserialização, sem data race:
  ReceiveDispatcher thread:
    NGAL_SAR::ReceiveFragment → devolve char* buffer + size (sem NewMessage!)
    NGAL_CS::DeliverToGateway(buffer, size) → queue de char buffers
    delete[] buffer
  GW thread:
    NewMessage + SetMessageFromCharArray + ConvertMessage → PushToInputQueue
```

Idêntico ao pipeline antigo em estrutura — o buffer bruto chega ao GW exactamente como chegava via SHM.

---

## 4. Bug Secundário: CleanupTimedOut não funciona

`CleanupTimedOut(0)` é chamado com `timeout_threshold=0` no `ReceiveDispatcher`, e `FB->Timestamp` nunca é actualizado (inicializado a 0, nunca modificado). A condição de timeout nunca se verifica. Isto NÃO causa o hash mismatch, mas deve ser corrigido.

**Correcção pendente:** Usar `GetTime()` como threshold e actualizar `FB->Timestamp = GetTime()` em cada fragmento recebido.

---

## 5. Crítica à SPEC-014 v1 (Draft)

A SPEC-014 v1 propunha adicionar logging hex temporário (`DEBUG_NGAL_HEX`) em 6 pontos (E1-E6) e executar o cenário para comparar bytes. Isto é uma abordagem de **diagnóstico empírico** que:

1. **Não identifica a causa raiz** — A análise estática mostrou que os bytes são uma cópia fiel (send side = receive side), logo os dumps hex seriam idênticos e a causa raiz ficaria por explicar.
2. **É demorada** — Requer 10 etapas (E0-E10), cada uma com modificações em código, compilação, execução e análise.
3. **Mascara o bug real** — O `printf`/`cerr` adicionado para logging altera o timing do ReceiveDispatcher, podendo eliminar ou criar race conditions, tornando o diagnóstico não-reprodutível.
4. **Concluiu erroneamente que a dupla deserialização é segura** (secção 3.5) — A análise estática diz que `ConvertMessage...` é read-only, mas ignora o data race em `PP->NewMessage()`.

A abordagem correcta era: analisar a diferença **estrutural** entre o pipeline antigo e novo (secção 3.5 da v1 já a documentava!), e reconhecer que `PP->NewMessage()` na thread errada é o bug.

---

## 6. Plano de Execução

| # | Etapa | Estado | Detalhe |
|---|-------|--------|---------|
| P1 | Correcção do `NGAL_SAR::ReceiveFragment` | **DONE** | Devolve buffer bruto em vez de Message* |
| P2 | Correcção do `NGAL_Transport_RAW::ReceiveDispatcher` | **DONE** | Usa buffer bruto para DeliverToGateway |
| P3 | Compilação | **DONE** | `cmake-build-ngal/` — PGCS, ContentApp, NRNCS OK |
| P4 | Teste: cenário intra-OS Content | **PENDENTE** | Executar com binários corrigidos |
| P5 | Corrigir CleanupTimedOut | **DONE** | Timestamp actualizado com time(0) em cada fragmento; threshold usa PPGCS->GetTime() |
| P6 | Actualizar SPEC-013 (referência cruzada) | **DONE** | Secção E9 adicionada com F2 fix |
| P7 | Commit | **PENDENTE** | Após teste passar |

---

## 7. Como Testar (P4)

Os binários corrigidos estão em `<local-repository-path>/cmake-build-ngal/`.

Copiar para o path de produção ou executar directamente:

```bash
# Parar processos antigos
./Scripts/Simple/clean.sh

# Executar com binários corrigidos (ajustar paths conforme script do cenário)
sudo <local-repository-path>/cmake-build-ngal/PGCS <local-repository-path>/IO/PGCS/ 0 Intra_Domain -lc
# + ContentApp, NRNCS, etc.
```

**Critério de sucesso:** ContentApp Repository verifica hash com sucesso (0 erros "hash of the file ... is not the same").

---

## 8. Decisões

| # | Decisão | Data | Razão |
|---|---------|------|-------|
| D1 | Eliminar dupla deserialização em vez de adicionar logging | 09/07/2026 | A análise estrutural identificou a causa raiz; logging empírico é mais lento e pode mascarar o bug |
| D2 | SAR devolve buffer bruto (char* + size) | 09/07/2026 | Alinha com Finding F2 da SPEC-013; elimina data race; simplifica o pipeline |
| D3 | Não adicionar DEBUG_NGAL_HEX | 09/07/2026 | logging temporário altera timing e pode criar/mascarar race conditions |
| D4 | Correcção do CleanupTimedOut é separada (P5) | 09/07/2026 | Não afecta o hash mismatch; prioridade inferior |

---

## 9. Pitfalls

1. **Ownership do buffer** — `ReceiveFragment` transfere ownership do `FB->Buffer` para o caller. O caller DEVE fazer `delete[]` após `DeliverToGateway` (que faz uma cópia para a queue).
2. **Forward declaration** — `NGAL_SAR.h` usa `class Message;` (forward declaration). O `.cpp` inclui `"Message.h"` para definição completa. Isto é correcto e intencional.
3. **DeleteMarkedMessages** — Com a correcção, o SAR já não cria Messages. O `DeleteMarkedMessages` no GW só afecta Messages criados no GW thread — correcto.
4. **SHM path** — O caminho intra-processo (ContentApp→PGCS e PGCS→ContentApp) continua via SHM e **não foi alterado**. Se o hash mismatch persistir APENAS intra-VM, o problema é outro.
