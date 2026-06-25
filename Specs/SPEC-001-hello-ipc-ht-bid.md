# SPEC-001: Hello IPC 2.0 — Carregar o BID do HT nos processos PGCS

**Autor:** Antonio Marcos Alberti  
**Data:** 20/06/2026  
**Estado:** Implemented

---

## 1. Problema

O `CoreRunDiscover01` do ContentApp (e de outros processos não-PGCS) falha ao executar:

```
DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS", "HT", PGCSPID, PGCSBIDs, PB)
```

A função faz três consultas ao HT local:

| Passo | Categoria | Chave | Objectivo |
|---|---|---|---|
| 1 | Cat[2] | Hash("PGCS") | Obter o PID do PGCS |
| 2 | Cat[5] | PID obtido | Obter todos os BIDs do PGCS |
| 3 | Cat[2] | Hash("HT") | Obter todos os BIDs do bloco HT |
| Final | — | — | Intersecção entre (2) e (3) = BID do HT do PGCS |

**Causa da falha:** O `GWHelloIPC02::StorePeerBindings` armazena Cat[2] Hash("PGCS") → PID e Cat[5] PID → GW_BID, mas **nunca armazena Cat[2] Hash("HT") → HT_BID**. Portanto, a 3ª consulta retorna vazia e a intersecção falha.

## 2. Restrição de segurança

Nem todos os processos devem expor os seus BIDs de blocos internos (HT, Core, etc.) a outros processos. Apenas o **PGCS** — por ser o orquestrador do sistema — deve anunciar o BID do seu HT. Isto evita que processos arbitrários possam endereçar directamente o HT de outro processo, o que seria uma exposição indevida.

## 3. Solução implementada

Evolução do protocolo hello IPC para versão **2.0**, com um campo opcional contendo o BID do HT do emissor. Apenas o PGCS preenche este campo.

### 3.1 Formato da mensagem hello IPC 2.0

```
ng -m --cl 0.1 [ <1 s Intra_OS > <2 s Sender_PID Sender_GW_BID > <2 s Dest_PID Dest_GW_BID > ]
ng -hello --ipc 2.0 [ <2 s Sender_IPC_Key Sender_LN > <1 s Sender_HT_BID > ]
ng -scn --s 0.1 [ <1 s SCN > ]
```

Para processos **não-PGCS**, o argumento `Sender_HT_BID` é omitido (formato 0.2 com 2 argumentos). Para o **PGCS**, é incluído (formato 2.0 com 3 argumentos).

### 3.2 Ficheiros modificados

#### 3.2.1 `GWExposition02.cpp` (emissor do HT_BID)

O HT_BID é incluído na **Phase 2** (auto-exposição do PGCS), não no `GWRunHelloIPC02`. Isto porque `GWRunHelloIPC02` é desactivado no PGCS (verifica `Key == 11` e retorna early). A Phase 2 foi adicionada a `GWExposition02::ExposePeers()` para que o PGCS envie um hello 2.0 a cada peer conhecido, anunciando o seu próprio PID, BID, LN, SHM key e HT_BID.

```cpp
// GWExposition02.cpp, Phase 2 (linhas 270-393)
string selfHTBID = "";
{
    Block* PHTB = 0;
    if (PB->PP->GetBlock("HT", PHTB) == OK && PHTB != 0)
    {
        selfHTBID = PHTB->GetSelfCertifyingName();
    }
}
PMB->NewIPCHelloCommandLine("--ipc", "2.0", selfKey, selfLN, selfHTBID, SelfHello, SelfPCL);
```

#### 3.2.2 `MessageBuilder.h` / `MessageBuilder.cpp`

Adicionado overload de `NewIPCHelloCommandLine` que aceita `_HTBID`:

```cpp
int NewIPCHelloCommandLine(string _Alternative, string _Version, key_t _Key,
                           string _LN, string _HTBID,
                           Message *_M, CommandLine *& _PCL);
```

Quando `_HTBID` é não-vazio, cria a command line com 2 argumentos:
```
ng -hello --ipc 2.0 [ < 2 s Key LN > < 1 s HT_BID > ]
```

Quando `_HTBID` é vazio, usa o formato original (1 argumento, compatível com 0.2).

#### 3.2.3 `GWHelloIPC02.cpp` (receptor)

Modificado in-place (não renomeado para GWHelloIPC20). O `Run()` verifica se o 2º argumento (HT_BID) está presente:

```cpp
string PeerHTBID = "";
if (NA >= 2)
{
    vector<string> HTData;
    _PCL->GetArgument(1, HTData);
    if (HTData.size() > 0)
        PeerHTBID = HTData.at(0);
}
```

E `StorePeerBindings` foi estendido para armazenar `Cat[2] Hash("HT") → HT_BID` e `Cat[5] PID → HT_BID` quando `_PeerHTBID` não é vazio.

#### 3.2.4 `GW.cpp` (registo das acções)

Registada a acção `-hello --ipc 2.0` no GW, usando a mesma classe `GWHelloIPC02` (que agora processa tanto 0.2 como 2.0).

### 3.3 Bug adicional corrigido: `Process::GetBlock`

Durante os testes, descobriu-se que o hello 2.0 chegava sem o HT_BID:
```
ng -hello --ipc 2.0 [ < 2 s 11 PGCS > ]   ← faltava < 1 s HT_BID >
```

**Causa:** `Process::GetBlock(string, Block*&)` tinha um bug onde `Status=OK` estava DEPOIS do `break` (dead code). A função retornava sempre `ERROR`, mesmo encontrando o block. A condição `GetBlock("HT", PHTB) == OK` em GWExposition02 falhava, deixando `selfHTBID` vazio.

**Fix:** Movido `Status=OK` para antes do `break` (`Common/src/Process.cpp` linha 417-423).

### 3.4 Diagrama de fluxo

```
PGCS                                        ContentApp
┌─────────────┐                            ┌─────────────────┐
│ GW          │                            │ GW              │
│ Exposition02│                            │  HelloIPC02     │
│  Phase 2    │                            │  → lê args      │
│  → GetBlock │                            │  → se HT_BID    │
│    ("HT")   │                            │    presente:    │
│  → obtém HT │── SHM key ────────────────→│    Cat[2]       │
│    BID      │                            │    Hash("HT")   │
│  → constrói │                            │    → HT_BID ✓   │
│    hello    │                            │                  │
│    com HT   │                            │ Depois:          │
│    BID      │                            │ CoreRunDiscover01│
└─────────────┘                            │ → Cat[2] Hash    │
                                           │   ("HT") existe ✓│
                                           │ → DISCOVER OK!   │
                                           └─────────────────┘
```

### 3.5 Critérios de aceitação

1. PGCS envia hello com HT BID → ContentApp armazena Cat[2] Hash("HT") → HT_BID
2. NRNCS envia hello SEM HT BID → ContentApp não armazena nada adicional
3. `DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS", "HT", ...)` retorna OK
4. `CoreRunDiscover01` Intra_OS consegue enviar discovery para PGCS::HT
5. As mensagens hello 0.2 continuam a ser processadas correctamente (retrocompatibilidade)

**Estado:** Critérios 1-2 verificados. Critérios 3-5 pendentes de validação após fix do GetBlock.

**Update 26/06/2026:** O fix do GetBlock (commit 2124b8f) foi verificado por análise estática — `Status=OK` está antes do `break` em Process.cpp:420. A spec está implementada e consistente com o código. No entanto, a validação empirica dos critérios 3-5 está bloqueada pelo acesso SSH às VMs 101/102 (chave rotacionada, pendente deploy). Adicionalmente, o SPEC-008 (inter-PGCS exposition via raw socket) revelou que o `DiscoverHomonymsBlocksBIDsFromPID` ainda falha nesse cenário — mas por uma causa diferente da ausência de Cat[2] (ver SPEC-008).

---

## 4. Notas de implementação

- O `GWRunHelloIPC02` NÃO foi modificado para incluir HT_BID, porque é desactivado no PGCS (`Key == 11`). A inclusão do HT_BID é feita exclusivamente em `GWExposition02::ExposePeers()` Phase 2.
- O `GWHelloIPC02` foi modificado in-place para processar ambas as versões (0.2 e 2.0), em vez de criar uma classe nova `GWHelloIPC20`.
- O bug de `Process::GetBlock` era pré-existente (não foi introduzido por esta spec), mas só se tornou relevante porque o GWExposition02 Phase 2 foi o primeiro caller a depender do valor de retorno `OK`.
