# SPEC-010: ContentApp Bindings Alignment After Hello IPC 2.0

**Autor:** Hermes Agent (Scalifax)
**Data:** 05/07/2026
**Revisão:** 1
**Estado:** Draft

---

## 1. Problema

Após a introdução do **Hello IPC 2.0** (SPEC-001, alterações em `GWHelloIPC02.cpp`, `GWMsgCl01.cpp`, `GWRunInitialization01.cpp`), as categorias de bindings usadas para roteamento inter-processo foram alteradas:

- **Cat 13** — agora usada para lookup BID→BlockIndex (dentro do processo)
- **Cat 19** — nova: PeerPID→IPC Key (roteamento SHM entre processos)
- **Cat 20** — nova: PeerPID→LegibleName

O `ContentApp/src/` nunca foi revisto após estas mudanças. Embora o ContentApp não use directamente Cat 13, 19 ou 20 (são internas ao GW), ele **depende** do correcto funcionamento do roteamento via GW para:

1. Publicar bindings de exposição (`Core::Exposition`) através do NRNCS
2. Descobrir pares (`CoreRunEvaluate01`) via Cat 2 e Cat 5
3. Obter IPC key do NRNCS (`CoreRunPeriodic01` linha 294) via Cat 19

Sintoma actual nos logs:

```
(1. Check for NRNCS awareness.)
(Aware of a NRNCS on Categories 2 and 9)
...
(Not aware of any Repository)
(Not aware of any Source)
```

O NRNCS é descoberto, mas os pares ContentApp (Source↔Repository) **não se descobrem mutuamente**.

---

## 2. Contexto: O que mudou no Hello IPC 2.0

### 2.1 GWHelloIPC02::StorePeerBindings (Common/src/GWHelloIPC02.cpp)

Após receber um `-hello --ipc 0.2` de um peer PGCS, armazena no HT local:

| Categoria | Binding | Descrição |
|-----------|---------|----------|
| **20** | PeerPID → LegibleName | Novo — identifica textual do peer |
| **19** | PeerPID → IPC Key | Novo — chave SHM para roteamento |
| **2** | Hash("LegibleName") → PeerPID | Existente |
| **3** | PeerPID → Hash("LegibleName") | Existente |
| **9** | Hash("LegibleName") → HID | Existente |
| **5** | PeerPID → BID (+ HT BID v2.0) | Existente, agora inclui HT BID |
| **2** | Hash("HT") → HT_BID | Novo v2.0 — armazenado no PGCS peer |

### 2.2 GWMsgCl01 (Common/src/GWMsgCl01.cpp)

- `ForwardMessageInsideProcess`: usa **Cat 13** para BID→BlockIndex
- `ForwardMessageInsideOS`: usa **Cat 19** para PID→IPC Key (roteamento SHM)

### 2.3 GWRunInitialization01 (Common/src/GWRunInitialization01.cpp)

- Armazena **Cat 13** (BID→BlocksIndex) e **Cat 14** (Index→BID)
- Armazena **Cat 20** (PID→LegibleName) inicial
- Armazena **Cat 17** (IPC Key→shmid) para PGCS local

---

## 3. Análise do ContentApp — Mapa de Bindings Actual

### 3.1 CoreRunInitialize01 — bindings iniciais (linhas 92-127)

| Função MessageBuilder | Categoria | Propósito |
|----------------------|-----------|-----------|
| `NewStoreBindingCommandLineFromBLNToHashBLN` | 1 | Hash("BlockLN") → "BlockLN" |
| `NewStoreBindingCommandLineFromHashBLNToBLN` | 2 | Hash("BlockLN") → Block BID |
| `NewStoreBindingCommandLineFromHashBLNToBID` | 2 | Hash("BlockLN") → Block BID |
| `NewStoreBindingCommandLineFromBIDToHashBLN` | 3 | Block BID → Hash("BlockLN") |
| `NewStoreBindingCommandLineFromBIDToBlocksIndex` | **13** | BID → Block Index ✅ |
| `NewStoreBindingCommandLineFromBlocksIndexToBID` | 14 | Index → BID ✅ |
| `NewStoreBindingCommandLineFromPIDToBID` | 5 | PID → BID ✅ |
| `NewStoreBindingCommandLineFromPLNToHashPLN` | 1 | Hash("ProcessLN") → "ProcessLN" |
| `NewStoreBindingCommandLineFromHashPLNToPLN` | 2 | Hash("ProcessLN") → PID |
| `NewStoreBindingCommandLineFromHashPLNToPID` | 2 | Hash("ProcessLN") → PID |
| `NewStoreBindingCommandLineFromPIDToHashPLN` | 3 | PID → Hash("ProcessLN") |
| `NewStoreBindingCommandLineFromOSIDToPID` | 5 | OSID → PID |
| `NewStoreBindingCommandLineFromHLNToHashHLN` | 1 | Hash("HostLN") → "HostLN" |
| `NewStoreBindingCommandLineFromHashHLNToHLN` | 2 | Hash("HostLN") → HID |
| `NewStoreBindingCommandLineFromOSIDToHID` | 6 | OSID → HID |
| `NewStoreBindingCommandLineFromHIDToOSID` | 6 | HID → OSID |

**Conclusão:** `NewStoreBindingCommandLineFromBIDToBlocksIndex` usa Cat 13, alinhado com Hello IPC 2.0 ✅

### 3.2 Core::Exposition — publicação de bindings via NRNCS (Core.cpp linhas 687-734)

O ContentApp publica bindings no domínio (Intra_Domain) via NRNCS:

| Categoria | Binding | Quando |
|-----------|---------|--------|
| **2** | Hash("ContentApp") → App PID | Source e Repository |
| **2** | Hash("Core") → Core BID | Source e Repository |
| **2** | Hash("Content") → App PID | Source e Repository |
| **2** | Hash("Source"/"Repository") → App PID | Conforme role |
| **1** | Hash("ContentApp") → "ContentApp" | Source e Repository |
| **1** | Hash("Content") → "Content" | Source e Repository |
| **1** | Hash("Source"/"Repository") → "Source"/"Repository" | Conforme role |
| **5** | App PID → Core BID | Source e Repository |
| **5** | OSID → App PID | Source e Repository |
| **6** | HID → OSID | Source e Repository |

**Conclusão:** Categorias correctas para exposição via NRNCS ✅

### 3.3 CoreRunEvaluate01 — descoberta de pares (linhas 129-131, 287-289)

```cpp
DiscoverHomonymsEntitiesIDsFromLN(2, "Repository", Servers, PB)
DiscoverHomonymsEntitiesIDsFromLN(2, "Content", Subject, PB)
DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("ContentApp","Core", Apps, PB)
```

Depende de Cat 2: Hash("Repository")→PID e Cat 2: Hash("Content")→PID publicadas pelo peer.

### 3.4 CoreRunPeriodic01 — descoberta NRNCS + IPC key (linha 294)

```cpp
PMB->NewGetCommandLine("0.1", 19, PCore->PSTuples[0]->Values[2], IPCUpdate, PCL);
```

Já usa **Cat 19** para obter IPC key do NRNCS ✅

### 3.5 CoreRunSubscribe01 — subscrição (linhas 132-138)

Subscreve Cat 18, Cat 2 e Cat 9 para chaves de conteúdo ✅

### 3.6 CoreRunPublish01/02/03 — publicação de conteúdo

Publica Cat 18 (conteúdo), Cat 2 (proveniência), Cat 9 (proveniência) ✅

### 3.7 CoreDeliveryBind01 — entrega (linha 180)

Verifica `PS->Category == 18` para conteúdo recebido ✅

---

## 4. Discrepâncias e Problemas Identificados

### 4.1 Descoberta de IPC key — apenas uma vez (⚠️ Médio)

Em `CoreRunPeriodic01.cpp`, a query de Cat 19 para NRNCS está dentro do bloco `if (PCore->RunExpose == true)` (linhas 246-313). Após a primeira execução, `RunExpose = false` e nunca mais é repetida. Se a binding Cat 19 expirar ou for perdida (e.g., reinício do PGCS), o ContentApp não consegue re-descobrir a IPC key.

### 4.2 Ausência de verificação de Cat 5 no peer discovery (⚠️ Baixo)

`CoreRunEvaluate01` descobre pares apenas via Cat 2 (`DiscoverHomonymsEntitiesIDsFromLN`). Não verifica se o peer também tem Cat 5 (PID→BID) consistente. Isto pode levar a tuplos incompletos.

### 4.3 ContentApp não publica bindings para descoberta local (Intra_OS) (⚠️ Médio)

O ContentApp publica bindings apenas via NRNCS (Intra_Domain — linha 251 do CoreRunPeriodic01). Para descoberta **local** (Intra_OS, mesma VM), deveria também publicar bindings directamente no PGCS. Sem bindings locais, `DiscoverHomonymsEntitiesIDsFromLN(2, ...)` no `CoreRunEvaluate01` só encontra peers via NRNCS (que requer que o outro ContentApp já tenha publicado).

Isto pode causar um **deadlock de arranque**:
- Source precisa de descobrir Repository → pergunta ao NRNCS
- NRNCS só tem bindings se Repository já publicou
- Repository publica bindings via NRNCS → mas precisa que NRNCS esteja pronto
- Se ambos arrancam ao mesmo tempo, nenhum descobre o outro

### 4.4 NRNCS IPC key — Key vs PID mismatch (⚠️ Crítico — causa provável)

Na linha 294 do `CoreRunPeriodic01.cpp`:
```cpp
PMB->NewGetCommandLine("0.1", 19, PCore->PSTuples[0]->Values[2], IPCUpdate, PCL);
```

O `PSTuples[0]->Values[2]` é a **PID do NRNCS** (ex: `09EB3120`).

O `GWHelloIPC02::StorePeerBindings` armazena **Cat 19** com:
```cpp
Category = 19;
Key = _PeerPID;       // ← a PID do peer PGCS, não do NRNCS!
Values.push_back(_PeerIPCKey);
```

**Problema:** O ContentApp pede Cat 19 com key = **NRNCS PID**, mas o GWHelloIPC02 armazena Cat 19 com key = **PGCS PID** (o processo que enviou o hello). A menos que o NRNCS partilhe a PID do PGCS (não partilha — o NRNCS é um processo separado), o lookup **falha silenciosamente** e a mensagem vai para o fallback (IPC key 11 = PGCS).

**Evidência nos logs:**
```
(Looking at Category 19 for the shared memory key behind the SCN = 09EB3120)
...
(Forwarding: The destination IPC key is 11)
```

O SCN `09EB3120` é a PID do NRNCS, mas a IPC key devolvida é `11` (PGCS, o fallback). Isto confirma que o lookup Cat 19 → NRNCS PID falhou, e o GW caiu no fallback para o PGCS.

### 4.5 ContentApp não armazena Cat 20 (PeerPID→LegibleName) (⚠️ Baixo)

O `GWRunInitialization01` armazena Cat 20 para o próprio processo. O `GWHelloIPC02` armazena Cat 20 para peers. O ContentApp não precisa de fazer isto — é da responsabilidade do GW ✅

---

## 5. Solução Proposta

### 5.1 Corrigir NRNCS IPC key lookup (4.4) — Priority: 🔥 Crítico

A raiz do problema: o ContentApp pede Cat 19 com **NRNCS PID**, mas quem armazena Cat 19 é o **PGCS peer** (durante hello IPC). A Cat 19 tem key = PGCS PID, não NRNCS PID.

**Opção A (recomendada):** O `CoreRunPeriodic01` deve pedir a IPC key do **PGCS** (não do NRNCS) e enviar mensagens para o domínio através do PGCS local.

Na prática, a mensagem para domínio já passa pelo PGCS local (IPC key 11). O lookup de Cat 19 para NRNCS PID é desnecessário porque:
- Mensagens Intra_Domain são enviadas com limiter = `Intra_Domain`
- O GW vê que o destino está fora do OS e encaminha para o PG block
- O PG block (PGCS) encaminha via raw socket para o PGCS peer
- O PGCS peer entrega ao NRNCS local

**Opção B:** Fazer o NRNCS também enviar um hello IPC 2.0 e armazenar a sua própria IPC key em Cat 19. Isto requer alterações no NRNCS (fora do escopo — não modificar PG.cpp/GW.cpp).

**Decisão:** Opção A — remover o lookup de Cat 19 para NRNCS PID. A mensagem de exposição já é enviada com Intra_Domain e o GW sabe como encaminhar.

### 5.2 Publicar bindings também Intra_OS (4.3) — Priority: 🔥 Alta

Adicionar uma exposição local (Intra_OS) para que peers na mesma VM possam descobrir-se sem depender do NRNCS.

### 5.3 Mover IPC key query para fora do bloco RunExpose (4.1) — Priority: ⚠️ Médio

Fazer a query Cat 19 repetidamente (no ciclo periódico) para garantir robustez a reinícios.

### 5.4 Adicionar verificação cruzada Cat 5 na descoberta de pares (4.2) — Priority: ⚠️ Baixo

Validar que o PID descoberto em Cat 2 também tem Cat 5 (PID→BID) antes de criar o tuplo de par.

---

## 6. Etapas de Implementação

| Etapa | Descrição | Ficheiro | Prio |
|-------|-----------|----------|------|
| **E0** | Adicionar logging em `CoreRunPeriodic01` para confirmar o lookup Cat 19 (imprimir key, resultado) | CoreRunPeriodic01.cpp | 🔥 |
| **E1** | Remover lookup de Cat 19 para NRNCS PID — a mensagem Intra_Domain já é encaminhada pelo PGCS local. A exposição via NRNCS não precisa de IPC key directa | CoreRunPeriodic01.cpp | 🔥 |
| **E2** | Adicionar exposição Intra_OS no `Core::Exposition`: publicar bindings também para `PB->PP->Intra_OS` (além do `Intra_Domain`) | Core.cpp (Exposition) | 🔥 |
| **E3** | Mover query de IPC key (se mantida) para fora do bloco `RunExpose`, ou eliminar | CoreRunPeriodic01.cpp | ⚠️ |
| **E4** | Adicionar validação Cat 5 no `CoreRunEvaluate01` antes de armazenar tuplo de par | CoreRunEvaluate01.cpp | ⚠️ |
| **E5** | Compilar, testar localmente (VM 100), depois nas VMs Alpine 101/102 | cmake-build-debug/ | 🔥 |
| **E6** | Executar cenário 1core-1repo-1source, confirmar descoberta Source↔Repository | Scripts/AlpineVMs/ | 🔥 |

---

## 7. Alterações Detalhadas

### E0 — Logging do lookup Cat 19

Em `CoreRunPeriodic01.cpp`, após linha 294, adicionar:

```cpp
#ifdef DEBUG
PB->S << Offset << "(DIAG: Requesting Cat[19] binding for NRNCS PID = "
      << PCore->PSTuples[0]->Values[2] << ")" << endl;
#endif
```

### E1 — Remover lookup Cat 19 (recomendado)

Em `CoreRunPeriodic01.cpp`, remover ou comentar linhas 258-308 (todo o bloco IPCUpdate), mantendo apenas a exposição:

```cpp
if (PCore->RunExpose == true)
{
    PCore->Exposition(PB->PP->Intra_Domain, ScheduledMessages);
    PCore->RunExpose = false;
    PB->State = "operational";

    // NOTE: IPC key lookup via Cat[19] removed.
    // Intra_Domain messages are routed through the local PGCS,
    // which handles cross-OS forwarding natively. The NRNCS PID
    // is NOT the same as the PGCS PID, so Cat[19] (stored with
    // PGCS PID as key) would never match NRNCS PID.
}
```

### E2 — Exposição Intra_OS

Em `Core.cpp`, função `Core::Exposition`, duplicar as publicações para `Intra_OS`:

Após a linha 735 (`}`), adicionar um segundo ciclo de exposição com limiter `PB->PP->Intra_OS`:

```cpp
// Also expose bindings locally (Intra_OS) for same-VM peer discovery
// without depending on NRNCS propagation
if (ScheduledMessages.size() > 0)
{
    Run = ScheduledMessages.at(0);
    if (Run != 0)
    {
        // Same bindings as Intra_Domain but with OS limiter
        Run->NewCommandLine("-run", "--expose", "0.1", PCL);
        NewLimiterCommandLineArgument(PB->PP->Intra_OS, PCL);

        // Reuse the same Hint1/Hint2 logic
        // ... same NewTernaCommandLineArgument calls as above ...
    }
}
```

### E3 — IPC key query periódica

Se a Opção A for rejeitada e a query mantida, mover as linhas 258-308 para o topo do `CoreRunPeriodic01::Run()`, fora dos blocos condicionais `RunExpose`.

### E4 — Validação Cat 5

Em `CoreRunEvaluate01.cpp`, antes de armazenar um tuplo de par (linhas 198/344), verificar:

```cpp
vector<string> *BIDs = new vector<string>;
if (PP->DiscoverHomonymsEntitiesIDsFromLN(5, Servers->at(i), BIDs, PB) == OK && BIDs->size() > 0)
{
    // Cat 5 binding exists — proceed with store
    PCore->PeerServerAppTuples.push_back(Apps[k]);
}
else
{
    PB->S << Offset1 << "(Warning: Candidate PID " << Servers->at(i)
          << " has no Cat[5] binding — incomplete tuple, skipping)" << endl;
}
delete BIDs;
```

---

## 8. Ficheiros a Modificar

| Ficheiro | Alteração | Prio |
|----------|-----------|------|
| `ContentApp/src/CoreRunPeriodic01.cpp` | E0 + E1: logging + remover IPC key lookup | 🔥 |
| `ContentApp/src/Core.cpp` (Exposition) | E2: exposição Intra_OS | 🔥 |
| `ContentApp/src/CoreRunEvaluate01.cpp` | E4: validação Cat 5 | ⚠️ |

**Não modificar:**
- `GW.cpp`, `PG.cpp` (regra do utilizador)
- `GWHelloIPC02.cpp`, `GWMsgCl01.cpp` (Common/src — Hello IPC 2.0 está correcto)
- `PG*.cpp` (fora do escopo)

---

## 9. Critérios de Aceitação

1. **Descoberta Source↔Repository** — ContentApp na Source VM descobre o Repository e vice-versa
2. **Logs sem erros** — `"Unable to get peer PGCS::HT BID"` não aparece
3. **Logs sem fallback** — IPC key 11 (PGCS fallback) não é usada para NRNCS
4. **Exposição Intra_OS** — Bindings são publicadas localmente para peers na mesma VM
5. **Nenhum crash novo** — Teste de 5 min sem crash
6. **Transferência de fotos** — Source → NRNCS → Repository funciona (10/10 fotos)

---

## 10. Decisões

| # | Decisão | Data | Razão |
|---|---------|------|-------|
| D1 | Remover lookup Cat 19 para NRNCS PID | 05/07/2026 | Cat 19 é armazenado por PGCS PID, não NRNCS PID. Mensagens Intra_Domain já são encaminhadas pelo PGCS local |
| D2 | Adicionar exposição Intra_OS | 05/07/2026 | Peer discovery intra-VM não deve depender do NRNCS |
| D3 | Não modificar GW.cpp nem PG.cpp | 05/07/2026 | Regra do utilizador |
| D4 | Não modificar Common/src (Hello IPC 2.0) | 05/07/2026 | GWMsgCl01 e GWHelloIPC02 já estão correctos |

---

## 11. Pitfalls

1. **Cat 19 é por PGCS, não por NRNCS**: O erro mais crítico. Assumir que Cat 19(PID→IPC Key) funciona para qualquer PID é incorrecto — só funciona para PIDs que enviaram hello IPC 2.0 (neste caso, o PGCS peer).
2. **Exposição duplicada**: Adicionar Intra_OS sem filtrar pode duplicar bindings no destino se o NRNCS também as propagar. O NRNCS deve ignorar bindings que já existem localmente.
3. **DelayBeforeRunPeriodic**: Após exposição Intra_OS, os pares na mesma VM podem ser descobertos mais cedo. Ajustar timers se necessário.
4. **Compatibilidade com Versão 0.1 do Hello IPC**: Se algum processo ainda usa hello IPC 0.1 (sem Cat 19), o lookup Cat 19 falha. O fallback para IPC key 11 é o comportamento esperado para processos legacy.
5. **Source e Repository na mesma VM**: Se ambos estiverem na mesma VM, a exposição Intra_OS permite descoberta directa via PGCS, sem NRNCS. Isto é desejável e mais rápido.