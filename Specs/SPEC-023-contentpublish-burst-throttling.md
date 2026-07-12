# SPEC-023: ContentPublish Burst Throttling e PGCS Relay Congestion Control

**Data:** 2026-07-12
**Estado:** Proposta
**Autor:** Hermes Agent
**Relacionada:** Docs/PGCS-RELAY-CONGESTION-BURST-SPLIT-2026-07-12.md (diagnóstico), SPEC-021 (one message per key), SPEC-022 (NRInfoPayload01)

---

## 1. Problema

Após SPEC-021 (mensagens separadas por key em NRSubBind01/PSSubBind01), o volume de mensagens no pipeline ContentPublish aumentou significativamente. O PGCS relay (raw Ethernet socket + single-thread ReceiveDispatcher) não consegue processar bursts de 200 publicações sem congestão.

### 1.1 Sintomas observados

- Corrupção de payloads/hashes após ~278 notificações
- PGCS relay raw socket buffer overflow (socket buffer ~212KB ≈ 152 frames de 1400 bytes)
- Fragmentos NGAL_SAR perdidos → reassembly incompleto → dados corrompidos
- PCore->Content vector cresce unbounded (nunca é limpo)
- Ausência de feedback do repositório → perda silenciosa

### 1.2 Causas Raiz

| RC | Descrição | Ficheiro |
|----|-----------|----------|
| RC7 | PGCS relay raw socket congestion: single-thread dispatcher insuficiente para 200+ mensagens em burst | `NGAL_Transport_RAW.cpp` |
| RC8 | Ausência de ACK/NACK entre ContentApp Source e Repository | `CoreRunContentPublish01.cpp` |
| RC9 | PCore->Content sem limite — crescimento unbounded | `Core.h` |

---

## 2. Correcções

### 2.1 Reduzir ContentBurstSize para 20-30 (fix primário)

**Ficheiro:** `IO/Source1/App.ini` e `IO/Repository1/App.ini`

**Problema:** ContentBurstSize=200 gera ~16400+ fragmentos NGAL_SAR num curto intervalo, excedendo a capacidade do raw socket (~212KB buffer ≈ 152 fragmentos) e do single-thread ReceiveDispatcher. O threshold de congestão observado é ~278 notificações.

**Correcção:** Reduzir ContentBurstSize de 200 para **25** (valor inicial empírico):

```ini
ContentBurstSize 25
```

**Porquê 25:**
- 25 ficheiros × ~82 fragmentos JPG ≈ 2050 fragmentos Ethernet
- A 152 fragmentos por buffer-cheio, o ReceiveDispatcher processa ~13 ciclos de buffer
- Com poll() timeout de 100ms, 13 ciclos ≈ 1.3s — dentro do `DelayBeforeANewPhotoPublish` (10s)
- Versus 200 ficheiros × 82 = 16400 fragmentos = 108 ciclos ≈ 10.8s — já no limite do timeout

**Efeito:** Reduz a carga no raw socket em ~8×. Os 200 ficheiros demoram 8 bursts (25×8) = 80s em vez de 1 burst de 10s. A latência total aumenta, mas a fiabilidade é garantida.

**Nota:** Este valor é um ponto de partida. Após logging (secção 2.4), calibrar empiricamente.

### 2.2 Threshold de memória: usar 50% de MAX_MESSAGES_IN_MEMORY (referência existente)

**Ficheiro:** `ContentApp/src/CoreRunContentPublish01.cpp`

**Problema:** O break condition na linha 236 só dispara com `MAX_MESSAGES_IN_MEMORY - 200` = 29800 mensagens — praticamente sistema cheio.

**Correcção:** Adicionar throttle preventivo usando o threshold de **50%** (15000), que já é usado noutro ponto do sistema (`PGCS/src/CoreMsgCl01.cpp:71`):

```cpp
// SPEC-023: Throttle preventivo.
// Threshold: 50% de MAX_MESSAGES_IN_MEMORY (15000), mesma referência
// usada no PGCS CoreMsgCl01.cpp:71 para controlo de congestão.
//
// NOTA: Este threshold NÃO se relaciona directamente com o raw socket
// buffer (~212KB). É uma heurística de segurança: se o Process já tem
// 15000+ mensagens em memória, há mensagens acumuladas que o PGCS relay
// ainda não escoou — parar o burst evita agravar a congestão.

#define MEMORY_THROTTLE           (MAX_MESSAGES_IN_MEMORY * 0.5)  // 15000
#define MEMORY_HARD_LIMIT         (MAX_MESSAGES_IN_MEMORY - 200)  // 29800
```

**Antes (linha 236):**
```cpp
if ((Counter == PCore->ContentBurstSize) || (PB->PP->GetNumberOfMessages() >= (MAX_MESSAGES_IN_MEMORY - 200)))
{
    break;
}
```

**Depois (SPEC-023):**
```cpp
// SPEC-023: Throttle preventivo
unsigned int CurrentMem = PB->PP->GetNumberOfMessages();

if (Counter == PCore->ContentBurstSize)
{
    // Burst normal — publicou ContentBurstSize ficheiros
    break;
}

if (CurrentMem >= MEMORY_HARD_LIMIT)
{
    // Hard limit — memória crítica
    PB->S << Offset << "(WARNING: Hard memory limit. Breaking burst at Counter="
          << Counter << ". Messages=" << CurrentMem << ")" << endl;
    break;
}

if (CurrentMem >= MEMORY_THROTTLE)
{
    // Throttle preventivo — se já publicou pelo menos 1, pausa
    if (Counter > 0)
    {
        PB->S << Offset << "(THROTTLE: Memory throttle. Pausing burst at Counter="
              << Counter << ". Messages=" << CurrentMem << ")" << endl;
        break;
    }
    // Se Counter == 0, publica pelo menos 1 antes de pausar
}
```

### 2.3 ContentBurstSize dinâmico (calibração empírica)

**Ficheiro:** `ContentApp/src/CoreRunContentPublish01.cpp`

Em vez de ContentBurstSize fixo, o logging (secção 2.5) vai mostrar quantas mensagens estão em memória durante cada burst. Com esses dados, o ContentBurstSize pode ser ajustado dinamicamente numa futura iteração.

**Para já:** Manter ContentBurstSize fixo em 25 (App.ini). O logging vai medir:

```
[BURST] Starting. ContentBurstSize=25 FilesInDir=200
        MessagesInMem=XX Content.size=YY
[BURST] Stopping. Counter=25 Reason=ContentBurstSize
        MessagesInMem=XX+Δ
```

Com estes logs, saberemos:
- Quantas mensagens em memória no início do burst (baseline)
- Quantas mensagens adiciona cada burst (Δ)
- Se Δ for pequeno (< 100-200), o PGCS relay escoa entre bursts → ContentBurstSize pode subir
- Se Δ crescer entre bursts (acumulação), o ContentBurstSize precisa de descer

### 2.4 Content vector size limit

**Ficheiro:** `ContentApp/src/CoreRunContentPublish01.cpp` + `ContentApp/src/Core.h`

**Problema:** `vector<string> Content` cresce sem limite. Após milhares de publicações, a verificação `for (i=0; i<Content.size(); i++)` torna-se O(n) e o consumo de memória é desnecessário.

**Correcção:** Adicionar limite máximo e política de cleanup.

```cpp
// Em Core.h
#define MAX_CONTENT_HISTORY 10000   // Máximo de entradas no Content vector
```

```cpp
// Em CoreRunContentPublish01.cpp, após push_back (L168):
PCore->Content.push_back(PayloadHash);

// SPEC-023: Limitar tamanho do Content vector
if (PCore->Content.size() > MAX_CONTENT_HISTORY)
{
    // Remove as entradas mais antigas (metade do histórico)
    PCore->Content.erase(PCore->Content.begin(),
                         PCore->Content.begin() + (PCore->Content.size() - MAX_CONTENT_HISTORY));
}
```

### 2.5 Rate limiting no NRPubNotify01

**Ficheiro:** `NRNCS/src/NRPubNotify01.cpp`

**Problema:** Para 25 ficheiros × N publishers, o `NRPubNotify01` cria N×25 mensagens de notificação quase em simultâneo. O `DelayBeforeSendingANotification` (0.0001s) é curto demais.

**Correcção:** Aumentar o delay efectivo quando há muitas notificações simultâneas.

```cpp
// SPEC-023: Rate limiting — espaçar notificações quando há muitas
unsigned int NA = 0;
_PCL->GetNumberofArguments(NA);
unsigned int PubCount = 0;

for (unsigned int i = 3; i < NA; i++)
{
    vector<string> Notification;
    _PCL->GetArgument(i, Notification);
    if (Notification.size() == 5 && Notification.at(0) == "pub")
        PubCount++;
}

// Calcular delay adaptativo: quanto mais pubs, maior o spacing
double EffectiveDelay = PNR->DelayBeforeSendingANotification;
if (PubCount > 10)
{
    // Se houver mais de 10 pubs, espaçar para evitar congestão
    EffectiveDelay = max(EffectiveDelay, 0.001 * PubCount);  // 1ms por publisher
}
```

No loop de criação de notificações, usar `EffectiveDelay` em vez de `PNR->DelayBeforeSendingANotification`:

```cpp
PB->PP->NewMessage(
    GetTime() + EffectiveDelay, 0, false, Notify);
```

**Nota:** Este rate limiting é no lado do NRNCS Source. O PGCS relay no ContentApp Source é o bottleneck principal, mas o NRNCS também contribui para a carga.

### 2.6 Logging de diagnóstico (activado sempre)

**Ficheiro:** `ContentApp/src/CoreRunContentPublish01.cpp`

Activar logging de diagnóstico SEMPRE (não apenas com #ifdef DEBUG) para monitorizar o comportamento do burst e calibrar o ContentBurstSize:

```cpp
// SPEC-023: Logging sempre activo para diagnóstico de burst
PB->S << Offset << "[BURST] Starting. ContentBurstSize="
      << PCore->ContentBurstSize
      << " FilesInDir=" << FileNamesInThePath.size()
      << " MessagesInMem=" << PB->PP->GetNumberOfMessages()
      << " Content.size=" << PCore->Content.size()
      << endl;
```

E antes do break:

```cpp
// SPEC-023: Logging do motivo do break
PB->S << Offset << "[BURST] Stopping. Counter=" << Counter
      << " Reason=" << (Counter == PCore->ContentBurstSize ? "ContentBurstSize" :
                        CurrentMem >= MEMORY_HARD_LIMIT ? "HardLimit" :
                        CurrentMem >= MEMORY_THROTTLE ? "MemThrottle" : "Unknown")
      << " MessagesInMem=" << CurrentMem
      << " Delta=" << (CurrentMem - StartingMem)
      << endl;
```

Onde `StartingMem` é guardado no início do loop:

```cpp
unsigned int StartingMem = PB->PP->GetNumberOfMessages();

---

## 3. Ficheiros Afectados

| Ficheiro | Mudança | Razão |
|----------|---------|-------|
| `ContentApp/src/CoreRunContentPublish01.cpp` | Throttling adaptativo + ContentBurstSize dinâmico + Content cleanup + logging | RC7, RC8, RC9 |
| `ContentApp/src/Core.h` | Adicionar `MAX_CONTENT_HISTORY` | RC9 |
| `NRNCS/src/NRPubNotify01.cpp` | Rate limiting adaptativo no delay de notificações | RC7 (contribuição secundária) |
| `NRNCS/src/NRPubNotify01.h` | Sem alterações (já inclui NR.h) | — |

---

## 4. Decisões de Design

| # | Decisão | Razão |
|---|---------|-------|
| D1 | **Reduzir ContentBurstSize de 200 para 25** (App.ini) como fix primário | Empírico: ~278 congestão com 200 → ~2050 fragmentos com 25 cabem no raw socket buffer |
| D2 | **Manter break no Counter==ContentBurstSize** como condição normal de fim de burst | O burst normal não deve ser afectado |
| D3 | **Throttle preventivo a 50% de MAX_MESSAGES_IN_MEMORY (15000)** — mesma referência que CoreMsgCl01 já usa | Não resolve o raw socket, mas evita agravar congestão quando o Process está carregado |
| D4 | **Content vector limitado a 10000 entradas** — remover as mais antigas | Previne crescimento unbounded sem perder referências recentes |
| D5 | **Rate limiting no NRPubNotify01** — delay adaptativo proporcional ao número de publishers | Reduz contribuição do NRNCS para a carga total |
| D6 | **Não modificar NGAL_Transport_RAW** — single-thread ReceiveDispatcher requer alteração arquitectural profunda | O throttle no emissor é mais simples e eficaz |
| D7 | **Calibrar ContentBurstSize empiricamente via logging [BURST]** em vez de adivinhar threshold de memória | O bottleneck é o raw socket (~212KB buffer), não o array de mensagens do Process (30000 slots) |

---

## 5. Alternativas Consideradas e Rejeitadas

| Alternativa | Motivo da rejeição |
|-------------|-------------------|
| Multi-thread ReceiveDispatcher no PGCS | Mudança arquitectural profunda; NGAL_SAR partilhado precisaria de locks; risco de data race |
| Aumentar SO_RCVBUF no raw socket | Apenas adia o problema — o buffer overflow volta com burst maior |
| ACK/NACK entre Source e Repository | Requer novo mecanismo de mensagens bidireccionais; muito intrusivo para SPEC-023 |
| Eliminar PCore->Content e usar disco | Performance impact; Content é critical path |

---

## 6. Verificação

1. Definir `StartingMem` no início do loop em `CoreRunContentPublish01.cpp` e adicionar throttling preventivo (MEMORY_THROTTLE a 50%)
2. Adicionar logging [BURST] sempre activo (Starting, Stopping com Delta)
3. Reduzir ContentBurstSize para 25 em `IO/Source1/App.ini` e `IO/Repository1/App.ini`
4. Adicionar `#define MAX_CONTENT_HISTORY 10000` em `Core.h` + cleanup no push_back
5. Aplicar rate limiting em `NRPubNotify01.cpp` (delay adaptativo)
6. Compilar: `cd build && make -j$(nproc)`
7. Deploy para source e repository VMs
8. Testar com `--publish 0.1` (200+ ficheiros)
9. Verificar logs: cada [BURST] mostra counter, motivo de break, Delta de mensagens
10. Verificar que **nenhum** erro de hash mismatch aparece (0 erros)
11. Verificar que o Delta entre bursts é pequeno (< 200 mensagens) — indica que o PGCS relay escoa entre bursts
12. Com base nos logs, ajustar ContentBurstSize para cima (ex: 50) ou para baixo (ex: 15)

---

## 7. Dependências

| SPEC | Dependência | Notas |
|------|-------------|-------|
| SPEC-021 | Aplicado | Mensagens separadas por key — este problema foi exacerbado por SPEC-021 |
| SPEC-022 | Aplicado (ou não) | Rate limiting é independente da correcção do NRInfoPayload01 |
| SPEC-023 | Nenhuma | Pode ser aplicado sobre o estado actual |

---

## 8. Pitfalls

1. **Throttle com Counter=0:** Se o throttle de memória (50%) dispara com Counter=0 (primeira iteração do burst), o loop não publica nenhum ficheiro. A verificação `if (Counter > 0)` garante pelo menos 1 ficheiro.
2. **Content cleanup:** Remover entradas antigas do Content vector significa que ficheiros removidos do directório podem ser republicados se o ContentCleanup removeu as suas hashes. Isto é aceitável — o repositório deve deduplicar.
3. **ContentBurstSize=25 é conservador:** 200 ficheiros com 25/burst e `DelayBeforeANewPhotoPublish=10s` levam ~80s. Se os logs mostrarem Delta estável (< 200 mensagens entre bursts), pode subir para 50. Se houver sinais de congestão, desce para 15.
4. **Rate limiting no NRPubNotify01:** Afecta a latência de entrega de notificações, não a fiabilidade. Notificações atrasadas são aceitáveis em troca de menos congestão.