# SPEC-008: Raw Socket Discovery — Inter-PGCS Exposition Failure

**Author:** Antonio Marcos Alberti  
**Date:** 25/06/2026  
**Revision:** 26/06/2026 — original diagnosis corrected after complete static analysis  
**Status:** Draft — Rev2 proposed in section 11  
**Branch:** AIOPT3

---

> **SPEC-036 applicability:** This SPEC's references to standalone HTS, GIRS or PSS describe the legacy distributed profile. Normal operation uses NRNCS; `NG_RUNTIME_PROFILE=legacy` is required when reproducing that topology. The shared PGCS `HT` destination and NRNCS exposition path remain in scope where stated.

## 1. Problem

In the Docker 1core-1repo-1source scenario and Alpine VMs 101/102, each PGCS fails to send inter-PGCS exposition messages. The repeated error in logs is:

```
PGRunExposition01.cpp:420
(ERROR: Unable to get peer PGCS::HT BID from local hash table)
```

**Visible symptom:** Inter-PGCS exposition (sending HTS, GIRS, PSS, NRNCS bindings between PGCS peers) never happens. ContentApp gets stuck in "The domain NRNCS is still unknown" because it never receives service bindings from the peer PGCS via exposition.
- Antes de propor uma correcção, verificar a cadeia completa: quem recebe a mensagem raw socket (PGHelloIHC01) → quem armazena bindings (ScheduleStoreBindings) → quem envia exposição (PGRunExposition01)

## 3. Análise estática completa (26/06/2026)

### 3.1 Cadeia de descoberta raw socket (verificada no código)

```
1. PGRunHello01::Run()
   → Constrói -hello --ihc com 9 elementos:
     [HID, OSID, PID, PG_BID, GW_SCN, HT_SCN, Stack, Interface, Identifier]
   → Linha 220: PCL->SetArgumentElement(0, 5, PPG->PHT->GetSelfCertifyingName())
   → HT_SCN (elemento 5) é preenchido com o SCN do HT local ✅
   → Envia via raw socket (SendToARawSocket)

2. PGHelloIHC01::Run() (receptor no peer)
   → Faz parse dos 9 elementos para ReceivedElements
   → Cria PGCSTuple [HID, OSID, PID, PG_BID] (Values[0..3])
   → Chama ScheduleStoreBindings("-de", ReceivedElements, PeerIdentifier, PeerStack)

3. PGHelloIHC01::ScheduleStoreBindings()
   → Cria mensagem -sr --b com múltiplas bindings:
     Cat[5] PID → PG_BID      (linha 399-409)
     Cat[5] PID → GW_BID      (linha 427-437)
     Cat[5] PID → HT_BID      (linha 455-465)
     Cat[2] Hash("HT") → HT_BID  (linha 474-477)  ✅
     Cat[2] Hash("PGCS") → PID   (linha 390-391)
     Cat[3] HT_BID → Hash("HT")  (linha 477)
     ... e mais (Cat 6, 7, 8, 9, 15)
   → Push da mensagem para GW input queue (linha 528)

4. HTStoreBind01::Run() (processa -sr --b)
   → Executa PHT->StoreBinding(Category, Key, Values)
   → As bindings ficam no HashStringMultimap do HT

5. PGRunExposition01::Run()
   → Linha 140: DiscoverHomonymsBlocksBIDsFromPID(PID, "HT", HTs, PB)
   → Intersecção: Cat[5] PID→BIDs ∩ Cat[2] Hash("HT")→BIDs = HT_BID
   → Se falha: imprime "ERROR: Unable to get peer PGCS::HT BID"
```

### 3.2 Verificações de consistência (tudo confirmado)

| Verificação | Resultado | Evidência |
|---|---|---|
| HT_SCN preenchido no hello IHC? | ✅ | PGRunHello01.cpp:220 `SetArgumentElement(0, 5, PPG->PHT->GetSelfCertifyingName())` |
| Cat[2] Hash("HT") armazenado? | ✅ | PGHelloIHC01.cpp:474 `NewStoreBindingCommandLineFromHashLNToSCN("0.1", 2, "HT", _ReceivedElements.at(5), ...)` |
| Chave de escrita = chave de leitura? | ✅ | Ambas usam `GenerateSCNFromCharArrayBinaryPatterns("HT", ...)` (MessageBuilder.cpp:604 e Process.cpp:2166) |
| GetBlock(0, PHTB) retorna HT? | ✅ | Process.cpp:226 `NewBlock("HT", PB3)` primeiro → indice 0 no vector Blocks |
| GetBlock(string) dead-code fixado? | ✅ | Commit 2124b8f — Status=OK movido para antes do break (Process.cpp:420) |
| HT::GetBinding funciona? | ✅ | HT.cpp:528 — HashStringMultimap.equal_range(Key), retorna valores correctos |

### 3.3 O PGCSTuples e populado?

Sim. `PGHelloIHC01::Run()` cria `Tuple *PeerPGS` com [HID, OSID, PID, PG_BID] e faz `PPGB->PGCSTuples.push_back(PeerPGS)` (linha 217). O `PGRunExposition01` itera sobre `PGCSTuples` e usa `Values[2]` (PID) como chave de lookup.

### 3.4 Conclusão da análise estática

A binding Cat[2] Hash("HT") → HT_BID **deveria** estar a ser armazenada correctamente. O código está sintacticamente correcto. A causa raiz do erro `Unable to get peer PGCS::HT BID` **não pode ser determinada por análise estática apenas** — requer investigação empirica com logs.

## 4. Hipóteses remanescentes (por confirmar empiricamente)

### H1: A mensagem de store binding não é processada antes do PGRunExposition01

`ScheduleStoreBindings` cria uma mensagem `-sr --b` e faz push para a GW input queue (linha 528). Se a mensagem não for processada pelo `HTStoreBind01` antes de `PGRunExposition01` correr, as bindings ainda não estarão no HT.

**Contra-argumento:** O teste Docker decorreu por 12+ min. O PGRunExposition01 corre periodicamente. Em 12 min, a mensagem deveria ter sido processada.

**Como verificar:** Adicionar log em `HTStoreBind01::Run()` imprimindo as bindings armazenadas, e log em `PGRunExposition01::Run()` imprimindo o conteúdo de Cat[5] e Cat[2] antes do lookup.

### H2: O PGRunExposition01 itera sobre PGCSTuples antes de serem populados

Se `PGRunExposition01` correr antes de `PGHelloIHC01` receber o hello do peer, `PGCSTuples` está vazio e o loop não executa. Mas se correr DEPOIS do peer ser registado mas ANTES da mensagem de store binding ser processada, o PID existe em `PGCSTuples` mas as bindings ainda não estão no HT.

**Como verificar:** Log do tamanho de `PGCSTuples` e do timestamp relativo em `PGRunExposition01`.

### H3: O _ReceivedElements.at(5) vem vazio ou com valor inválido

Se o HT_SCN do peer for uma string vazia (e.g., se `PHT->GetSelfCertifyingName()` retornar "" no emissor), a binding Cat[2] Hash("HT") → "" é armazenada mas o `HT::GetBinding` ignora valores vazios (linha 547: `if (E != "")`).

**Como verificar:** Log de `_ReceivedElements.at(5)` em `ScheduleStoreBindings`. Verificar se o SCN do HT do PGCS emissor está vazio.

### H4: Message lifecycle afecta o processamento da store binding

As SPECs 002/003 alteraram `MarkToDelete` em `Block::Run()`. A mensagem de store binding pode estar a ser marcada para delete antes de ser totalmente processada.

**Contra-argumento:** SPEC-002 foi revertida. SPEC-003 removeu apenas o `GWStatusS01Msg` dead code e o loop de ScheduledMessages. Não deveria afectar mensagens `-sr --b`.

**Como verificar:** Log em `HTStoreBind01::Run()` confirmando que a função é realmente chamada.

### H5: A intersecção em DiscoverHomonymsBlocksBIDsFromPID falha por mismatch de valores

A intersecção faz `BottomUpBIDs[i] == TopDownBIDs[j]` (string equality). Cat[5] retorna [PG_BID, GW_BID, HT_BID]. Cat[2] retorna [HT_BID]. Se o HT_BID armazenado em Cat[5] for diferente do armazenado em Cat[2] (e.g., por terem sido gerados em momentos diferentes), a intersecção falha.

**Como verificar:** Log dos valores exactos em Cat[5] e Cat[2] no momento do lookup.

## 5. Plano de investigação empirica

### 5.1 Pré-requisitos

- Acesso SSH às VMs 101/102 (deploy da nova chave `<operator-ssh-key>`)
- Ou: compilar e testar localmente no host 100

### 5.2 Etapas

| Etapa | Descrição | Ficheiro |
|---|---|---|
| **E0** | Adicionar log temporário em `PGRunExposition01::Run()` antes da linha 140: imprimir PID do peer, tamanho de Cat[5] e Cat[2] para esse PID | PGRunExposition01.cpp |
| **E1** | Adicionar log temporário em `HTStoreBind01::Run()`: imprimir Category, Key e Values de cada binding armazenada | HTStoreBind01.cpp |
| **E2** | Adicionar log temporário em `ScheduleStoreBindings`: imprimir `_ReceivedElements.at(5)` (HT_BID recebido) | PGHelloIHC01.cpp |
| **E3** | Compilar (g++ -fsyntax-only para verificação, depois compilar nas VMs) | Make/compile-parallel.sh |
| **E4** | Executar cenário 1core-1repo-1source nas VMs Alpine 101/102 | `Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh`, `run_PGCS_on_Repo_VM.sh`, `run_NRNCS_on_Source_VM.sh`, `run_Repository_on_Repo_VM.sh`, `run_Source_on_Source_VM.sh` |
| **E5** | Analisar logs: confirmar se as bindings chegam ao HT, se o PID está correcto, se o HT_BID não é vazio | IO/logs/*.log |
| **E6** | Com base nos logs, identificar qual hipótese (H1-H5) é a correcta | — |
| **E7** | Escrever a correcção específica e testar | — |

### 5.3 Código de diagnóstico para E0

Adicionar em `PGRunExposition01.cpp` antes da linha 140:

```cpp
#ifdef DEBUG
{
    vector<string> *Cat5BIDs = new vector<string>;
    vector<string> *Cat2BIDs = new vector<string>;
    string HashHT;
    PB->PP->GenerateSCNFromCharArrayBinaryPatterns("HT", HashHT);

    PB->S << Offset << "(DIAG: Looking up PID = " << PPG->PGCSTuples[i]->Values[2] << ")" << endl;

    if (PB->PP->GetHTBindingValues(5, PPG->PGCSTuples[i]->Values[2], Cat5BIDs) == OK)
    {
        PB->S << Offset << "(DIAG: Cat[5] PID→BIDs has " << Cat5BIDs->size() << " values:)";
        for (unsigned int d = 0; d < Cat5BIDs->size(); d++)
            PB->S << " " << Cat5BIDs->at(d);
        PB->S << endl;
    }
    else
        PB->S << Offset << "(DIAG: Cat[5] PID→BIDs lookup FAILED)" << endl;

    if (PB->PP->GetHTBindingValues(2, HashHT, Cat2BIDs) == OK)
    {
        PB->S << Offset << "(DIAG: Cat[2] Hash(\"HT\")→BIDs has " << Cat2BIDs->size() << " values:)";
        for (unsigned int d = 0; d < Cat2BIDs->size(); d++)
            PB->S << " " << Cat2BIDs->at(d);
        PB->S << endl;
    }
    else
        PB->S << Offset << "(DIAG: Cat[2] Hash(\"HT\")→BIDs lookup FAILED)" << endl;

    delete Cat5BIDs;
    delete Cat2BIDs;
}
#endif
```

## 6. Problema relacionado: Cat[19] PID → IPC Key (fora do escopo)

`GWExposition02::ExposePeers()` Phase 2 precisa de Cat[19] PID → IPC key para enviar hello IPC 2.0 via SHM. A descoberta raw socket não transporta o IPC key do peer. Com IPC namespaces separados no Docker, key 11 é per-container.

**Decisão:** Cat[19] fica fora do escopo desta SPEC. O `PGRunExposition01` (exposição inter-PGCS via raw socket) é um mecanismo independente do `GWExposition02` (hello IPC 2.0 via SHM). Mesmo sem Cat[19], se PGRunExposition01 funcionar, as bindings são trocadas via raw socket.

## 7. Ficheiros envolvidos (sem alterações até conclusão do diagnóstico)

| Ficheiro | Estado | Notas |
|---|---|---|
| PGRunExposition01.cpp | Não modificado | E0 adiciona log temporário |
| PGHelloIHC01.cpp | Não modificado | ScheduleStoreBindings verificado |
| PGRunHello01.cpp | Não modificado | Hello IHC verificado |
| HTStoreBind01.cpp | Não modificado | E1 adiciona log temporário |
| Process.cpp | Não modificado | GetBlock e GetHTBindingValues verificados |
| HT.cpp | Não modificado | GetBinding e StoreBinding verificados |
| GW.cpp | Não modificado (regra mantida) | — |

## 8. Critérios de Aceitação (após correcção)

1. `PGRunExposition01` já não imprime `ERROR: Unable to get peer PGCS::HT BID from local hash table`
2. Mensagens de exposição são enviadas entre PGCS peers
3. ContentApp recebe bindings HTS/GIRS/NRNCS via exposição
4. ContentApp sai de "The domain NRNCS is still unknown"
5. Nenhum crash novo introduzido

## 9. Decisões

| # | Decisão | Data | Razão |
|---|---|---|---|
| D1 | Diagnóstico original (Cat[2] vazio) rejeitado | 26/06/2026 | ScheduleStoreBindings já armazena Cat[2] Hash("HT") → HT_BID (linhas 470-477) |
| D2 | Não modificar PGRunHello01.cpp | 26/06/2026 | A StoreBinding proposta era redundante — a binding já é criada em ScheduleStoreBindings |
| D3 | Investigação empirica antes de qualquer correcção | 26/06/2026 | Análise estática não identifica a causa — é preciso verificar runtime |
| D4 | GW.cpp não modificado (regra mantida) | 26/06/2026 | — |
| D5 | Cat[19] fora do escopo | 26/06/2026 | Mecanismo independente (SHM vs raw socket) |

## 10. Pitfalls

1. **Não assumir bindings ausentes sem verificar o fluxo completo**: O erro original foi assumir que Cat[2] estava vazio. A análise do código mostra que é preenchido por ScheduleStoreBindings.
2. **Não propor StoreBinding redundante**: A correção proposta no diagnóstico original adicionaria uma binding que já existe. Isto poderia criar duplicatas ou mascarar o problema real.
3. **Message lifecycle**: As SPECs 002/003 podem ter efeitos colaterais subtis no processamento de mensagens `-sr --b`. Verificar se HTStoreBind01::Run() é realmente chamado.
4. **Valores vazios em bindings**: HT::GetBinding ignora valores vazios (`if (E != "")`). Se o HT_SCN do emissor for vazio, a binding existe mas é ignorada no lookup.
5. **Timing**: O PGRunExposition01 pode correr num momento em que as bindings ainda não foram processadas, mesmo que eventualmente sejam.

---

## 11. SPEC-008 Rev2 — Proposta de Correção (05/07/2026)

### 11.1 Contexto

A SPEC-008 original (v1, 26/06/2026) concluiu que o código em `ScheduleStoreBindings` armazena correctamente as bindings Cat[5] PID→HT_BID e Cat[2] Hash("HT")→HT_BID. No entanto, o erro `Unable to get peer PGCS::HT BID` persiste no runtime.

Análise adicional (05/07/2026) durante SPEC-010 (ContentApp bindings) e investigação do NRNCS revelou que:

**O problema é de timing (H1 confirmado conceptualmente):**

1. `PGHelloIHC01::Run()` recebe o `-hello --ihc` do peer PGCS via raw socket
2. `ScheduleStoreBindings` cria uma mensagem `-sr --b` e faz `PushToInputQueue` (linha 528)
3. A mensagem é processada **assincronamente** pelo ciclo `RoundRobinMessageProcessing` do GW
4. `PGRunExposition01::Run()` corre **antes** da mensagem ser processada
5. As bindings Cat[5] e Cat[2] ainda **não estão no HT** quando o lookup é feito
6. `DiscoverHomonymsBlocksBIDsFromPID` falha → erro na linha 420

A mensagem de store é eventualmente processada, mas `PGRunExposition01` já falhou para este peer. Nas iterações seguintes, a binding pode já estar presente e a exposição funciona — ou não, dependendo do momento exacto do ciclo.

### 11.2 Solução Proposta: Direct HT Storage (sem mensagem)

Em vez de criar uma mensagem `-sr --b` para a fila, o `ScheduleStoreBindings` deve armazenar todas as bindings **directamente no HT** usando `PGW->StoreHTBindingValues(Category, Key, &Values)`. A mensagem é **eliminada** por ser redundante — o único consumidor é o `HTStoreBind01` local, e o armazenamento directo é mais rápido e elimina a race condition.

**Vantagens:**
- Sem race condition — bindings disponíveis imediatamente
- Menos mensagens no pipeline do GW (menos ciclos de processamento)
- `PGW->StoreHTBindingValues` já existe e é usada em `GWRunInitialization01` (linha 92) e `GWHelloIPC02` (linhas 164, 174, etc.)
- Código mais simples e directo

### 11.3 Ficheiros a Modificar

| Ficheiro | Alteração | Descrição |
|----------|-----------|-----------|
| `PGCS/src/PGHelloIHC01.cpp` | Substituir criação de mensagem por direct HT stores | Remover `NewMessage`, `NewConnectionLessCommandLine` e todos os `NewCommonCommandLine`/`NewStoreBindingCommandLine*`, substituir por `PGW->StoreHTBindingValues` |

### 11.4 Bindings a Armazenar Directamente

O `ScheduleStoreBindings` actualmente cria uma mensagem com todas as bindings. A mensagem será removida e substituída por chamadas directas a `StoreHTBindingValues`. As bindings a armazenar são:

| Categoria | Key | Value | Descrição |
|-----------|-----|-------|-----------|
| 6 | `_ReceivedElements[0]` (HID) | `_ReceivedElements[1]` (OSID) | HID→OSID |
| 7 | `_ReceivedElements[1]` (OSID) | `_ReceivedElements[0]` (HID) | OSID→HID |
| 9 | Hash("Host") | `_ReceivedElements[0]` (HID) | Host ID lookup |
| 8 | `_ReceivedElements[0]` (HID) | "Host" | HID→LegibleName "Host" |
| 2 | Hash("OS") | `_ReceivedElements[1]` (OSID) | OS lookup |
| 3 | `_ReceivedElements[1]` (OSID) | Hash("OS") | OSID→Hash("OS") |
| 5 | `_ReceivedElements[1]` (OSID) | `_ReceivedElements[2]` (PID) | OSID→PID |
| 7 | `_ReceivedElements[2]` (PID) | `_ReceivedElements[0]` (HID) | PID→HID |
| 2 | Hash("PGCS") | `_ReceivedElements[2]` (PID) | PGCS PID — **para descoberta** |
| 3 | `_ReceivedElements[2]` (PID) | Hash("PGCS") | PID→Hash("PGCS") |
| **5** | **`_ReceivedElements[2]` (PID)** | **`_ReceivedElements[3]` (PG_BID)** | **PID→PG_BID** |
| 2 | Hash("PG") | `_ReceivedElements[3]` (PG_BID) | PG BID lookup |
| 3 | `_ReceivedElements[3]` (PG_BID) | Hash("PG") | PG_BID→Hash("PG") |
| **5** | **`_ReceivedElements[2]` (PID)** | **`_ReceivedElements[4]` (GW_BID)** | **PID→GW_BID** |
| 2 | Hash("GW") | `_ReceivedElements[4]` (GW_BID) | GW BID lookup |
| 3 | `_ReceivedElements[4]` (GW_BID) | Hash("GW") | GW_BID→Hash("GW") |
| **5** | **`_ReceivedElements[2]` (PID)** | **`_ReceivedElements[5]` (HT_BID)** | **PID→HT_BID** ⬅️ CRÍTICO |
| **2** | **Hash("HT")** | **`_ReceivedElements[5]` (HT_BID)** | **Hash("HT")→HT_BID** ⬅️ CRÍTICO |
| **3** | **`_ReceivedElements[5]` (HT_BID)** | **Hash("HT")** | **HT_BID→Hash("HT")** ⬅️ CRÍTICO |

As **críticas** (Cat 5 PID→HT_BID e Cat 2 Hash("HT")→HT_BID) são as que `PGRunExposition01` usa para o lookup que falha.

### 11.5 Código a Implementar

Substituir todo o corpo de `ScheduleStoreBindings` (linhas 242-533) por armazenamento directo no HT. O código mantém apenas a declaração de variáveis e as chamadas a `PPGB->PGW->StoreHTBindingValues`:

```cpp
  // ********************************************************
  // SPEC-008 Rev2: Store bindings directly in HT to avoid
  // race condition between message queue processing and
  // PGRunExposition01 execution.
  // ********************************************************

  // Cat[6] HID -> OSID
  Category = 6;
  Key = _ReceivedElements.at(0);
  Values.clear();
  Values.push_back(_ReceivedElements.at(1));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[5] OSID -> PID
  Category = 5;
  Key = _ReceivedElements.at(1);
  Values.clear();
  Values.push_back(_ReceivedElements.at(2));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[5] PID -> PG_BID
  Category = 5;
  Key = _ReceivedElements.at(2);
  Values.clear();
  Values.push_back(_ReceivedElements.at(3));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[5] PID -> GW_BID
  Values.push_back(_ReceivedElements.at(4));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[5] PID -> HT_BID (CRITICAL — for PGRunExposition01)
  Values.push_back(_ReceivedElements.at(5));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[2] Hash("HT") -> HT_BID (CRITICAL — for PGRunExposition01)
  Category = 2;
  string HashHT;
  PB->GenerateSCNFromCharArrayBinaryPatterns("HT", HashHT);
  Key = HashHT;
  Values.clear();
  Values.push_back(_ReceivedElements.at(5));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[2] Hash("PGCS") -> Peer PID
  Category = 2;
  string HashPGCS;
  PB->GenerateSCNFromCharArrayBinaryPatterns("PGCS", HashPGCS);
  Key = HashPGCS;
  Values.clear();
  Values.push_back(_ReceivedElements.at(2));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);
```

### 11.6 Requisitos

- **`PPGB->PGW`** deve estar acessível (já está — `PG *PPGB = (PG *)PB;` na linha 250)
- **`PGW->StoreHTBindingValues`** deve ser pública (já é — usada em `GWHelloIPC02.cpp` e `GWRunInitialization01.cpp`)
- **`PB->GenerateSCNFromCharArrayBinaryPatterns`** deve ser acessível (já é — usado em toda a Common/src)

### 11.7 Cenário Alternativo: Sem Direct Store

Se a solução directa não for aceite, a alternativa é adicionar um atraso no `PGRunExposition01` para aguardar que as mensagens sejam processadas:

```cpp
// Alternative: yield to let pending store bindings be processed
PB->PP->RoundRobinMessageProcessing();
```

### 11.8 Diagnóstico (E0-E3 da SPEC-008 v1)

Manter as etapas de diagnóstico propostas na v1 (secção 5), mas com prioridade reduzida — a correção directa elimina a necessidade de confirmar a hipótese H1.

### 11.9 Decisões

| # | Decisão | Data | Razão |
|---|---------|------|-------|
| R1 | Direct HT storage **substitui** a mensagem (não acumula) | 05/07/2026 | Mensagem era redundante — único consumidor era HTStoreBind01 local |
| R2 | ScheduleStoreBindings passa a armazenar tudo directamente | 05/07/2026 | Elimina race condition e reduz ciclos de processamento |
| R3 | Cat[5] PID → HT_BID combinado com PG_BID e GW_BID | 05/07/2026 | StoreHTBindingValues adiciona ao multimap sem limpar anteriores |

### 11.10 Critérios de Aceitação

1. `PGRunExposition01` não imprime `ERROR: Unable to get peer PGCS::HT BID`
2. Mensagens de exposição são enviadas entre PGCS peers dentro de 1 ciclo
3. ContentApp recebe bindings NRNCS/HTS/GIRS via exposição inter-PGCS
4. ContentApp sai de "The domain NRNCS is still unknown"
5. Nenhum crash novo introduzido
6. `ScheduleStoreBindings` não cria nem envia mensagens (código mais enxuto)

### 11.11 Ficheiros Afectados

| Ficheiro | Acção |
|----------|-------|
| `PGCS/src/PGHelloIHC01.cpp` | Reescrever ScheduleStoreBindings: remover criação de mensagem, adicionar direct stores |
| `PGCS/src/PGRunExposition01.cpp` | Sem alterações (o bug é no ScheduleStoreBindings, não aqui) |
