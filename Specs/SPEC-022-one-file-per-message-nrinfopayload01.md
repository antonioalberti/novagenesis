# SPEC-022: One File Per Message — NRInfoPayload01 Não Acumular no InlineResponseMessage

**Data:** 2026-07-11
**Estado:** Proposta
**Autor:** Hermes Agent
**Relacionada:** SPEC-021 (one message per key em NRSubBind01), SPEC-018 (ResetPayload)

---

## 1. Problema

Após SPEC-021, o `NRSubBind01` cria mensagens separadas por key. O HT responde com uma mensagem por ficheiro. Cada resposta tem EXACTAMENTE UM `-info --payload` + UM payload. **Contudo, no NRNCS source-side, o `NRInfoPayload01` acumula todos os `-info --payload` no mesmo `InlineResponseMessage` — mesmo RC5, novo local.**

### 1.1 Log do NRNCS source (source36)

```
[11918.182s] (NRNCS forwarding payload: file=00003...)
              (NRNCS forwarding payload: file=00004...)
              (NRNCS forwarding payload: file=00005...)
[11918.206s] (Received 00003...)
```

3 `-info --payload` processados no mesmo `Block::Run()`, todos no mesmo `InlineResponseMessage`.

### 1.2 Log do ContentApp repo (repo61)

```
[11922.845s] file=00009 hash=6A17D369 ERROR: not same (got 59A88EAD)
[11922.859s] file=00010 hash=DE17CDB4 ERROR: not same (got 59A88EAD)
...
```
A partir de 00009, TODOS têm o hash 59A88EAD (conteúdo de 00008). Subscription 0 (Key=59A88EAD) tem `HasContent=0` — 00008 nunca foi entregue, mas o seu payload contaminou todos os seguintes.

### 1.3 Causa Raiz (RC6)

`NRInfoPayload01::Run()` (linhas 90-109):

```cpp
InlineResponseMessage->ResetPayload();
InlineResponseMessage->SetPayloadFromCharArray(Payload, Size);
InlineResponseMessage->NewCommandLine(_PCL, PCL);
PCL->Version = "0.1";
```

Para CADA `-info --payload` na mensagem recebida:
1. `ResetPayload()` liberta o payload anterior
2. `SetPayloadFromCharArray()` carrega o novo payload
3. `NewCommandLine()` copia o CL para o **mesmo** `InlineResponseMessage`

Após N iterações, o `InlineResponseMessage` contém N CLs `-info --payload` mas **UM** payload — o último carregado. A serialização NG produz UM bloco de payload partilhado por todos os CLs.

**Este é o exacto mesmo problema do RC5/SPEC-021, mas no `InlineResponseMessage` do NR block (não do HT).**

---

## 2. Fluxo Actual do Problema

```
SOURCE VM (source36)                          REPO VM (repo61)
────────────────────                          ────────────────
ContentApp Source publica 100 ficheiros
     │
     ▼
NRNCS Source: NRSubBind01 (SPEC-021)
  → 100 mensagens separadas ao HT
  → HT responde INDIVIDUALMENTE
  → Cada resposta chega ao NR block
     │
     ▼
NRInfoPayload01 (BUG):
  Block::Run() processa TODOS os -info --payload
  de TODAS as 100 respostas do HT?
  
  NÃO — na verdade cada resposta individual do HT
  tem APENAS 1 -info --payload. O bug é que
  quando MÚLTIPLAS respostas do HT chegam juntas
  (enfileiradas no InputQueue), cada uma é
  processada separadamente com o SEU próprio
  InlineResponseMessage.

  MAS — o que acontece é que o PGCS no source
  entrega MÚLTIPLAS respostas do HT como UMA
  só mensagem ao NRNCS → NR block processa
  todos os -info --payload no mesmo Block::Run
  → mesmo InlineResponseMessage → RC5.
     │
     ▼
PGCS source → raw socket → PGCS repo
     │
     ▼
ContentApp Repo recebe 1 mensagem com
N× -info --payload + 1 payload (último ficheiro)
→ hash mismatch em N-1 ficheiros
```

**A causa exacta do batching no PGCS ainda está por confirmar**, mas a solução é clara: o `NRInfoPayload01` NUNCA deve acumular no `InlineResponseMessage`. Cada `-info --payload` deve produzir a SUA própria mensagem, com o SEU próprio routing e o SEU próprio payload.

---

## 3. Correcção

### 3.1 Princípio

Cada `-info --payload` processado por `NRInfoPayload01` gera uma mensagem NOVA e INDEPENDENTE, com:
- Routing extraído da mensagem recebida (`-m --cl`)
- `-d --b` + `-info --payload` + payload
- `-scn --s` (para passar o guard `NoCL > 2` do `PushToInputQueue`)

A mensagem é enviada via `PGW->PushToInputQueue()` — o GW trata do routing naturalmente.

### 3.2 Código actual vs proposto

**Ficheiro:** `NRNCS/src/NRInfoPayload01.cpp`

**Antes (RC6 — acumula no InlineResponseMessage):**
```cpp
// SPEC-018: Reset payload
InlineResponseMessage->ResetPayload();

// Copy payload
InlineResponseMessage->SetPayloadFromCharArray(Payload, Size);

// Copy ng -info --payload CL
InlineResponseMessage->NewCommandLine(_PCL, PCL);
PCL->Version = "0.1";
```

**Depois (SPEC-022 — mensagem separada por ficheiro):**
```cpp
NR* PNR = (NR*)PB;
GW* PGW = PNR->PGW;

// Extrair routing da mensagem recebida
CommandLine* RoutedCL = NULL;
_ReceivedMessage->GetCommandLine("-m", "--cl", RoutedCL);

// Criar nova mensagem para ESTE ficheiro
Message* PayloadMsg = NULL;
PB->PP->NewMessage(GetTime(), 0, false, PayloadMsg);

// Copiar routing (-m --cl)
if (RoutedCL != NULL)
{
    vector<string> Limiters;
    vector<string> Sources;
    vector<string> Destinations;
    RoutedCL->GetArgument(0, Limiters);
    RoutedCL->GetArgument(1, Sources);
    RoutedCL->GetArgument(2, Destinations);
    
    if (Limiters.size() > 0 && Sources.size() > 0 && Destinations.size() > 0)
    {
        CommandLine* RouteCL = NULL;
        PMB->NewConnectionLessCommandLine(RoutedCL->Version,
                                          &Limiters, &Sources, &Destinations,
                                          PayloadMsg, RouteCL);
    }
}

// Adicionar -d --b
PMB->NewCommonCommandLine("-d", "--b", "0.1",
                          PB->StringToInt("18"), Values.at(0), &Values,
                          PayloadMsg, PCL);

// Copiar payload
PayloadMsg->SetPayloadFromCharArray(Payload, Size);

// Adicionar -info --payload
PayloadMsg->NewCommandLine(_PCL, PCL);
PCL->Version = "0.1";

// Adicionar -scn --s (passa o guard NoCL > 2)
string SCN = NameGenerator::GetInstance().GenerateFromMessage(PayloadMsg);
PMB->NewSCNCommandLine("0.1", SCN, PayloadMsg, PCL);

// Enviar para o GW
PGW->PushToInputQueue(PayloadMsg);
```

**Nota:** O `InlineResponseMessage` deixa de ser usado para transportar o payload. O NR block retorna com o InlineResponseMessage vazio (ou quase vazio), que será descartado pelo GWMsgCl01 (NoCL <= 2). Isto é intencional — as respostas reais vão directamente para o GW InputQueue.

### 3.3 Ficheiros afectados

| Ficheiro | Mudança |
|----------|---------|
| `NRNCS/src/NRInfoPayload01.cpp` | Criar mensagem separada por `-info --payload` `-info --payload` em vez de acumular no InlineResponseMessage |
| `NRNCS/src/NRInfoPayload01.h` | Adicionar `#include "GW.h"` e `#include "NR.h"` se necessário |

### 3.4 Arquivos já incluídos

O `NRInfoPayload01.cpp` já inclui:
- `NRInfoPayload01.h` → `Action.h`
- `NR.h` → contém `GW* PGW` como friend

O header `NR.h` já declara `friend class NRInfoPayload01;`? Vou verificar... Não, `NR.h` NÃO lista `NRInfoPayload01` como friend. **Precisamos de adicionar `friend class NRInfoPayload01;` em `NR.h`** para aceder a `PNR->PGW`.

---

## 4. Impacto

### 4.1 Routing preservado

A mensagem nova herda o routing da mensagem recebida (`-m --cl`). O GW processa o routing naturalmente — a mensagem vai para o destino correcto (ContentApp via PGCS).

### 4.2 Mensagens por ficheiro

Cada ficheiro gera exactamente 1 mensagem com:
- `-m --cl` (routing original)
- `-d --b` (delivery binding)
- `-info --payload` (metadados do ficheiro)
- `-scn --s` (SCN)
- payload (conteúdo do ficheiro)

Total: 4 CLs + payload > 3 → passa `NoCL > 2` no GW. ✅

### 4.3 InlineResponseMessage vazio

O NR block retorna com InlineResponseMessage quase vazio (pode ter 0-1 CLs de outras acções). O `GWMsgCl01::ForwardMessageInsideProcess` descarta-o (`NoCL <= 2`). Isto é correcto — as mensagens reais estão no InputQueue via PushToInputQueue.

### 4.4 Compatibilidade

SPEC-022 é compatível com SPEC-014/015/017/018/019/020/021. Nenhuma destas é alterada ou quebrada.

---

## 5. Verificação

1. Aplicar SPEC-022 em `NRInfoPayload01.cpp`
2. Adicionar `friend class NRInfoPayload01;` em `NR.h`
3. Compilar: `cd build && make -j$(nproc)`
4. Deploy para source36 via `git pull` + `make`
5. Testar com `--publish 0.1` (100 ficheiros)
6. Verificar: **ZERO** erros "hash of the file is not the same"
7. Verificar: cada timestamp no NRNCS log mostra APENAS 1 "forwarding payload" (não 3-4 em lote)
8. Verificar: ContentApp repo recebe 100/100 com hashes correctos

---

## 6. Decisões

| # | Decisão | Razão |
|---|---------|-------|
| D1 | Criar mensagem separada em vez de usar InlineResponseMessage | Mesmo padrão SPEC-021; NG só suporta 1 payload por mensagem |
| D2 | Manter ResetPayload() (SPEC-018) | Prevenção, mesmo que já não necessário |
| D3 | Usar PGW->PushToInputQueue em vez de OutputQueue | GW processa routing naturalmente; mensagem chega ao destino correcto |
| D4 | Adicionar friend class NRInfoPayload01 em NR.h | Necessário para aceder a PNR->PGW |