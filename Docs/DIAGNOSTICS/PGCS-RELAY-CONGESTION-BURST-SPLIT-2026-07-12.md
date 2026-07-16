# Diagnóstico: PGCS Relay Congestion e Burst Split no ContentPublish

**Data:** 2026-07-12
**Contexto:** SPEC-021 aplicado (mensagens separadas por key em NRSubBind01/PSSubBind01).
**Sintoma observado:** Corrupção de payloads/hashes após ~278 notificações no pipeline ContentPublish.

---

## 1. Mapa do Pipeline

```
┌─────────────────────────────────────────────────────────────────┐
│  VM Source (ContentApp + NRNCS + PGCS)                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ContentApp Source                                              │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ CoreRunContentPublish01::Run()                          │   │
│  │  1. Lê directorio (até ContentBurstSize=200)            │   │
│  │  2. Para cada ficheiro:                                 │   │
│  │     a. Verifica PCore->Content[] (já publicado?)        │   │
│  │     b. Cria mensagem com payload + -p --notify          │   │
│  │     c. push_back(PayloadHash) em PCore->Content          │   │
│  │     d. Counter++                                        │   │
│  │  3. Break se Counter==200 OU memória cheia (L236)      │   │
│  │  4. Agenda nova ronda (-run --contentpublish)           │   │
│  └─────────────────────────────────────────────────────────┘   │
│             │                                                  │
│             ▼                                                  │
│  GW InputQueue → OutputQueue                                   │
│             │                                                  │
│             ▼                                                  │
│  PGCS relay → NGAL_SAR (fragmentação)                          │
│             → NGAL_Transport_RAW (raw socket Ethernet)  ───────┤
│                                                                 │
│  PGCS → NGAL_Transport_RAW (recv dispatcher, single thread)    │
│       → NGAL_SAR (reassembly) → NGAL_CS::DeliverToGateway      │
│       → GW → NRNCS                                              │
│                                                                 │
│  NRNCS Source:                                                  │
│     NRPubNotify01 (expande -p --notify)                         │
│       → cria mensagens -notify --s para cada "pub" tuple       │
│       → cada com DelayBeforeSendingANotification (0.03s/0.0001)│
│       → PushToInputQueue                                        │
│       → GW → PGCS → raw socket → Repository VM                  │
│                                                                 │
├─────────────────────────────────────────────────────────────────┤
│  VM Repository (ContentApp + PGCS)                              │
│  PGCS → NGAL_Transport_RAW (recv) → NGAL_SAR → NGAL_CS          │
│       → GW → ContentApp Repo                                     │
│  ContentApp Repo: CoreNotifyS01 → subscreve → recebe payload    │
│  └─ CoreInfoPayload01 → escreve ficheiro → verifica hash        │
└─────────────────────────────────────────────────────────────────┘
```

## 2. O Break Condition (CoreRunContentPublish01.cpp:236)

```cpp
if ((Counter == PCore->ContentBurstSize) || (PB->PP->GetNumberOfMessages() >= (MAX_MESSAGES_IN_MEMORY - 200)))
{
    break;
}
```

| Parâmetro | Valor | Fonte |
|-----------|-------|-------|
| `ContentBurstSize` | 200 | `App.ini`, `Core.cpp:127` default=50 |
| `MAX_MESSAGES_IN_MEMORY` | 30000 | `Common/src/Process.h:114` |
| `MAX_MESSAGES_IN_MEMORY - 200` | 29800 | Guard do memory pressure |
| `GetNumberOfMessages()` | crescente | Retorna mensagens activas no Process |

**Observação crítica:** O memory guard (`GetNumberOfMessages >= 29800`) só dispara quando há ~29800 mensagens em memória. Com ContentBurstSize=200, o loop parte sempre no Counter==200 em conditions normais. **Contudo**, o ~278 observado não vem deste guard — é um limite de throughput do PGCS relay (ver secção 3).

### 2.1 O que acontece quando o loop parte

1. **Counter** é variável local (resets a 0 na próxima ronda)
2. **PCore->Content** (unbounded `vector<string>`) manteve os hashes dos ficheiros publicados
3. A ronda seguinte (`-run --contentpublish` agendado com `DelayBeforeANewPhotoPublish`) re-lê o directorio, filtra já-publicados por hash, publica os restantes
4. Mas se a primeira ronda só publicou N < 200 (por memory guard ou outro break), a segunda ronda publica 200-N

**Problema:** Se o PGCS relay perde/corrompe algumas notificações da primeira ronda, o repositório nunca recebe esses ficheiros. O ContentApp Source assume que foram publicados (hash em `PCore->Content`), mas o repositório não confirma.

## 3. PGCS Relay Congestion (~278)

### 3.1 Arquitectura do relay

O PGCS relay é implementado em três camadas:

1. **NGAL_SAR** — Segmentation & Reassembly: divide mensagens em fragmentos de ~1400 bytes (Ethernet MTU), recompõe no destino
2. **NGAL_Transport_RAW** — Raw Ethernet socket (AF_PACKET, ethertype 0x1234)
   - Send: `SendFragment()` com loop de 12000 tentativas (~2 min) com 10ms sleep
   - Receive: **single thread** `ReceiveDispatcher()` com `poll()` em até 32 SSIDs, timeout 100ms
3. **NGAL_CS::DeliverToGateway** — Fila thread-safe com mutex + condition variable para entregar ao GW

### 3.2 Gargalo identificado

| Componente | Tipo de gargalo | Capacidade |
|-----------|----------------|-----------|
| `ReceiveDispatcher` | Single thread poll() | 1 thread para todas as SSIDs |
| `NGAL_SAR::ReceiveFragment` | Reassembly incremental | Bloqueia até fragmentos completos |
| `sendto()` no send | Tentativas com sleep 10ms | ~12000 retries (~2min) |
| `NetworkReceiveQueue` | Fila unbounded | Sem limite de tamanho |
| `NGAL_CS::DeliverToGateway` | Thread-safe push | Mutex contention possível |

Com ContentBurstSize=200, o pipeline gera aproximadamente:

| Mensagem | Quantidade | Tamanho típico |
|----------|-----------|---------------|
| Publish messages (payload JPG ~100KB) | 200 | ~115 KB cada |
| NGAL_SAR fragments (1400 bytes) | 200×82 = ~16400 | ~1400 bytes cada |
| NRNCS subscription messages (SPEC-021) | 200 | ~1 KB cada |
| NRPubNotify01 notification messages | 200 × N_peers | ~500 bytes cada |
| HT response messages | 200 | ~1 KB cada |

**Total de frames Ethernet:** ~16400 + 200 + 800 + 200 = ~17600 frames num curto intervalo.

O threshold observado de ~278 não é um limite de código, mas sim o ponto onde **a congestão do raw socket + single-thread dispatcher causa perda ou corrupção** de fragmentos SAR, resultando em reassembly mal-sucedido e dados corrompidos.

### 3.3 Mecanismo de corrupção

1. Single `ReceiveDispatcher()` processa uma SSID de cada vez
2. Durante o processamento de uma frame (recvfrom + SAR reassembly), as outras SSIDs acumulam frames no socket buffer
3. Socket buffer Ethernet (típico 212992 bytes = ~152 frames de 1400 bytes) enche
4. O kernel descarta frames novas (overflow do socket buffer)
5. Fragmentos perdidos → reassembly SAR nunca completa
6. Dados parciais são corrompidos ou perdidos

## 4. NRPubNotify01 — Key Passing

### 4.1 Fluxo actual

NRPubNotify01::Run() (NRNCS/src/NRPubNotify01.cpp):

```
Recebe -p --notify com Category, Key, Values, pub/sub tuples
  → Adiciona -sr --b ao InlineResponseMessage (1 só, sem payload)
  → Para cada tuple "pub":
       Cria nova mensagem com:
         -m --cl (routing para o publisher)
         -notify --s (key = Key.at(0))
         -scn --s
       Agenda com DelayBeforeSendingANotification
       PushToInputQueue
```

**Key correctness:** Cada notificação leva `Key.at(0)` — a hash do payload publicado. Após SPEC-021, cada mensagem de subscrição tem 1 key, portanto `Key` tem size 1. O passing da key está correcto para mensagens individuais.

**Problema potencial:** Se o ContentApp publica 200 ficheiros, o NRNCS recebe 200 notificações (uma por ficheiro). Cada notificação gera N `-notify --s` mensagens para os N publishers. Com 200 ficheiros × 4 publishers = 800 mensagens num curto intervalo. O `DelayBeforeSendingANotification` (0.0001s no INI) é curto demais para espaçar estas mensagens → todas entram na GW InputQueue quase em simultâneo → congestionam.

## 5. PCore->Content e Counter — Overflow

### 5.1 Content vector

```cpp
vector<string> Content;  // Core.h:84 — sem limite
```

- Cresce monotonamente: cada ficheiro publicado faz `push_back(PayloadHash)`
- **Nunca é limpo** — não há mecanismo de `clear()` ou remoção de entradas antigas
- Sem reinício do processo, Content pode crescer até esgotar memória
- Com publicações periódicas (CoreRunContentPublish01 reaponta), cada ronda verifica Content.size() antes de publicar → O(n) por ficheiro

### 5.2 Counter

```cpp
unsigned int Counter = 0;  // Variável local em Run()
```

- Reseta a 0 em cada invocação de Run()
- Incrementa apenas por publicações bem-sucedidas
- **Comportamento correcto** — Counter é local ao burst, não há overflow

### 5.3 Interacção entre bursts

Se o loop parte antes de Counter=200:

1. Counter para em N < 200 (ex: se memory guard ou outro erro)
2. PCore->Content tem N hashes
3. Próximo `-run --contentpublish` agenda com `DelayBeforeANewPhotoPublish`
4. Nova ronda: re-lê directorio → encontra os mesmos 200 ficheiros
5. Hash check: N já publicados → saltos
6. Publica 200-N novos → Counter vai de 0 a 200-N
7. PCore->Content tem agora 200 hashes

**Isto funciona correctamente** desde que o PGCS relay não perca notificações. Se perder, o ContentApp assume publicado (hash em Content) mas o repositório nunca recebeu = perda de dados.

## 6. Causas Raiz

### RC7: PGCS relay raw socket congestion — single-thread dispatcher insuficiente para bursts de 200+ mensagens

O ReceiveDispatcher (1 thread) + NGAL_SAR reassembly não consegue processar ~17600 frames Ethernet no intervalo de tempo de um ContentBurst. O socket buffer do kernel overflowa e fragmentos são perdidos.

### RC8: Ausência de feedback/acknowledgment entre ContentApp Source e Repository

Não existe confirmação de que o repositório recebeu o conteúdo. O ContentApp assume sucesso após `PushToInputQueue`. Se o PGCS relay perde a mensagem, é perda silenciosa.

### RC9: PCore->Content sem limite — crescimento unbounded

O vector `Content` cresce sem limite. Após múltiplos bursts, a verificação `for (i=0; i<Content.size(); i++)` torna-se O(n) e pode acumular milhões de entradas.

## 7. Próximos Passos

### 7.1 Curto prazo — mitigação

1. **Reduzir ContentBurstSize** — de 200 para **25**, para diminuir a carga no PGCS relay por burst de ~16400 para ~2050 fragmentos
2. **Aumentar DelayBeforeSendingANotification** — de 0.0001 para 0.01-0.05, para espaçar notificações no NRPubNotify01
3. **Adicionar log [BURST]** — sempre activo, com StartingMem, Counter, Delta de mensagens, motivo de break
4. **Throttle preventivo a 50% de MAX_MESSAGES** (15000) — mesma referência usada no CoreMsgCl01

### 7.2 Médio prazo — SPEC-023

1. **ContentBurstSize calibrado empiricamente** — com base nos logs [BURST], ajustar para o valor máximo que não cause congestão (Delta estável entre bursts)
2. **Content vector cleanup** — remover hashes de ficheiros antigos quando excede MAX_CONTENT_HISTORY (10000)
3. **Rate limiting no NRPubNotify01** — delay adaptativo proporcional ao número de publishers

### 7.3 Longo prazo

1. **Multi-thread ReceiveDispatcher** — uma thread por SSID ou pool de threads
2. **Socket buffer tuning** — aumentar SO_RCVBUF no raw socket de 212KB para >= 2MB

---

## Apêndice A: Ficheiros Relevantes

| Ficheiro | Função |
|----------|--------|
| `ContentApp/src/CoreRunContentPublish01.cpp` | Loop de publicação com break condition (L236) |
| `ContentApp/src/Core.h` | `Content` vector (L84), `ContentBurstSize` (L160) |
| `NRNCS/src/NRPubNotify01.cpp` | Expande notificações pub/sub |
| `PGCS/src/NGAL_Transport_RAW.cpp` | Raw socket relay (send + receive dispatcher) |
| `Common/src/NGAL_CS.cpp` | Convergence sublayer — delivery ao GW |
| `Common/src/NGAL_SAR.cpp` | Segmentation & Reassembly |
| `Common/src/Process.h` | `MAX_MESSAGES_IN_MEMORY` = 30000 |
| `ContentApp/src/CoreNotifyS01.cpp` | Guard `GetNumberOfMessages() < MAX-200` (L130) |

## Apêndice B: Logging Sugerido

```cpp
// Em CoreRunContentPublish01.cpp, antes do break (L236):
PB->S << "[BURST] Counter=" << Counter
      << " Content.size=" << PCore->Content.size()
      << " MessagesInMem=" << PB->PP->GetNumberOfMessages()
      << " Breaking=" << (Counter == PCore->ContentBurstSize ? "ContentBurstSize" : "MemoryGuard")
      << endl;

// Em NGAL_Transport_RAW.cpp, ReceiveDispatcher:
// Quando poll() retorna com POLLIN e recvfrom() bem-sucedido:
//   static unsigned int totalFrames = 0;
//   if (++totalFrames % 100 == 0)
//     PB->S << "[PGCS-RELAY] totalFrames=" << totalFrames << endl;
```