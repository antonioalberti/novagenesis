# SPEC-021: One Message Per File — Eliminate InlineResponseMessage with Multiple Payloads

**Date:** 2026-07-11  
**Status:** Superseded — Superseded by **SPEC-022-nrinfopayload01-separate-messages.md** (cache model)  
**Author:** Hermes Agent  
**Related:** SPEC-014 (data race), SPEC-017 (break), SPEC-018 (ResetPayload), SPEC-020 (re-delivery)

---

> **NOTICE:** This SPEC was written assuming the **forwarding model** for NRInfoPayload01. The correct NovaGenesis model is the **cache model** implemented in SPEC-022 (canonical). In the cache model, NRInfoPayload01 stores payloads to disk and does not forward them. Delivery happens via HTGetBind01 (cat=18) when subscribers request content. This SPEC is kept for historical reference only.

---

## 1. Problema

Após SPEC-014/015/017/018/020, o hash mismatch persiste. O SPEC-018 introduziu `ResetPayload()` para permitir substituir o payload no InlineResponseMessage reutilizado, mas isto **não resolve** o problema fundamental: uma mensagem NG serializada contém exactamente **UM** payload. Todos os `-info --payload` CLs numa mensagem partilham esse payload único.

### 1.1 Sintoma observado

- **Sem SPEC-018 (sem ResetPayload):** O payload fica preso no 1º ficheiro. Todos os N ficheiros recebidos contêm os bytes do 1º ficheiro cujo payload foi carregado.
- **Com SPEC-018 (com ResetPayload):** Cada iteração substitui o payload. Na serialização, todos os `-info --payload` CLs partilham o payload do **último** ficheiro. Resultado: todos os ficheiros têm o conteúdo do último ficheiro processado.

### 1.2 Porquê SPEC-018 falha

`ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray()` (Message.cpp:1586-1681) serializa:
```
[CL1][CL2]...[CLn][\n][PayloadBytes]
```

Um bloco único de payload é anexado após todos os CLs. A deserialização `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` (Message.cpp:1907-2049) extrai todos os CLs e depois UM payload (`PayloadSize = MessageSize - t`). Não existe mecanismo para múltiplos payloads numa mensagem.

---

## 2. Causa Raiz (RC5)

O `NRSubBind01::Run()` (NRNCS/PSS, linha 77-81) faz loop sobre as keys de subscrição e adiciona **todas** como `ng -g --b` CLs ao **mesmo** InlineResponseMessage:

```cpp
for (unsigned int i = 0; i < Key.size(); i++)
{
    PMB->NewGetCommandLine("0.2", PB->StringToInt(Category.at(0)), Key.at(i), InlineResponseMessage, PCL);
}
```

Este InlineResponseMessage é depois encaminhado ao HT block (via `NRMsgCl01` que adiciona `-m --cl` com destino ao HT). O `Block::Run()` do HT processa todos os `ng -g --b` sequencialmente com o mesmo InlineResponseMessage. Cada `HTGetBind01::Run()` para Category 18:
1. `ResetPayload()` — limpa o payload anterior
2. `SetMessage(..., filename, ...)` — define PayloadFile
3. `ConvertPayloadFromFileToCharArray()` — carrega o ficheiro actual
4. `NewInfoPayloadCommandLine(filename)` — adiciona `-info --payload`
5. `NewCommonCommandLine("-d", "--b", ...)` — adiciona `-d --b`

Após N iterações, o InlineResponseMessage tem N pares de `[-d --b] + [-info --payload]` CLs mas UM payload — o último carregado.

---

## 3. Fluxo Completo do Problema

```
ContentApp Repo (repository guest)                     ContentApp Source (source guest)
────────────────────────                     ────────────────────────
                                             Publica 100 ficheiros (JPG)
                                             NRNCS source recebe notificação
                                             ContentApp envia subscrição:
                                             ng -s --b 0.1 [ <1 s 18> <99 s key1...key99> ]

NRNCS source:
  NRMsgCl01 → adiciona -m --cl com destino HT
  NRSubBind01 → loop: adiciona 99x ng -g --b ao InlineResponseMessage
  InlineResponseMessage volta ao GW via PushToInputQueue
  GW encaminha ao HT via -m --cl

  HT Block::Run() processa 99x ng -g --b:
    HTGetBind01 (iter 1): ResetPayload + load file 00000 + add -d --b + -info --payload
    HTGetBind01 (iter 2): ResetPayload + load file 00001 + add -d --b + -info --payload
    ...
    HTGetBind01 (iter 99): ResetPayload + load file 00098 + add -d --b + -info --payload

  InlineResponseMessage final:
    [-m --cl dest=PGCS] [99x -d --b] [99x -info --payload] [payload=conteúdo de 00098]

  Serialização: UM payload após todos os CLs ← BUG

  InlineResponseMessage via GW → SHM → PGCS (source) → NGAL → NRNCS (repo)

NRNCS repo:
  NRInfoPayload01 processa cada -info --payload:
    Para CADA um: copia o MESMO payload (00098) para InlineResponseMessage
    → SHM → PGCS (repo) → ContentApp (repo)

ContentApp Repo:
  CoreInfoPayload01 processa cada -info --payload:
    Escreve o mesmo payload (00098) em 99 ficheiros com nomes diferentes
    → Hash mismatch em 98/99 ficheiros (excepto 00098)
```

---

## 4. Correcção

### 4.1 Princípio

Cada ficheiro de conteúdo precisa da sua **própria mensagem** com o seu **próprio payload**. Em vez de acumular N `ng -g --b` CLs num único InlineResponseMessage, o `HTGetBind01` para Category 18 deve criar uma **mensagem separada** por ficheiro e enviá-la directamente para o GW output.

### 4.2 Mudança em `HTGetBind01.cpp`

**Antes (código actual com SPEC-018):**
```cpp
if (Category == 18)
{
    if (_Values != 0 && _Values->size() > 0)
    {
        string ThePath = PB->GetPath();
        // SPEC-018: Reset payload from any previous GetBind response
        InlineResponseMessage->ResetPayload();
        InlineResponseMessage->SetMessage(GetTime(), 0, true, "Temp.txt",
                                          _Values->at(0), "Message.ngs", ThePath);
        InlineResponseMessage->ConvertPayloadFromFileToCharArray();
        PMB->NewInfoPayloadCommandLine("0.1", _Values, InlineResponseMessage, NewHTDeliveryBind01);
    }
}
```

**Depois (SPEC-021):**
```cpp
if (Category == 18)
{
    if (_Values != 0 && _Values->size() > 0)
    {
        string ThePath = PB->GetPath();

        // SPEC-021: Create a SEPARATE message per file payload.
        // A single NG message can carry only ONE payload. When multiple
        // ng -g --b CLs are processed in the same InlineResponseMessage,
        // all -info --payload CLs share the same payload — the last one
        // loaded. Fix: create a dedicated message with its own payload
        // and push it directly to the GW output queue.

        // Get the GW block to push the message
        GW* PGW = PHT->PGW;

        // Create a new message for this file
        Message* FilePayloadMessage = NULL;
        PP->NewMessage(GetTime(), 1, false, FilePayloadMessage);

        // Add the connection-less routing header (-m --cl)
        // Copy the destination from the InlineResponseMessage's existing -m --cl
        // (which points to the PGCS that will forward to the subscriber)
        // Use the same limiters/sources/destinations as the subscription message.
        // The InlineResponseMessage already has a -m --cl from NRMsgCl01
        // that routes to the PGCS. We need to replicate this routing.
        CommandLine* RouteCL = NULL;
        CommandLine* PayloadInfoCL = NULL;
        CommandLine* DeliveryCL = NULL;

        // Get the -m --cl from the InlineResponseMessage (first CL)
        // and copy its routing to the new message
        unsigned int IRM_NCL = 0;
        InlineResponseMessage->GetNumberofCommandLines(IRM_NCL);
        if (IRM_NCL > 0)
        {
            CommandLine* OriginalRouteCL = NULL;
            InlineResponseMessage->GetCommandLine(0, OriginalRouteCL);
            if (OriginalRouteCL != NULL
                && OriginalRouteCL->Name == "-m"
                && OriginalRouteCL->Alternative == "--cl")
            {
                vector<string> OrigLimiters;
                vector<string> OrigSources;
                vector<string> OrigDestinations;
                OriginalRouteCL->GetArgument(0, OrigLimiters);
                OriginalRouteCL->GetArgument(1, OrigSources);
                OriginalRouteCL->GetArgument(2, OrigDestinations);

                PMB->NewConnectionLessCommandLine(
                    OriginalRouteCL->Version,
                    &OrigLimiters,
                    &OrigSources,
                    &OrigDestinations,
                    FilePayloadMessage, RouteCL);
            }
        }

        // Load the file as payload
        FilePayloadMessage->SetMessage(GetTime(), 0, true, "Temp.txt",
                                       _Values->at(0), "Message.ngs", ThePath);
        FilePayloadMessage->ConvertPayloadFromFileToCharArray();

        // Add -d --b and -info --payload to the new message
        PMB->NewCommonCommandLine("-d", "--b", _PCL->Version,
                                  Category, _Key, _Values,
                                  FilePayloadMessage, DeliveryCL);
        PMB->NewInfoPayloadCommandLine("0.1", _Values,
                                       FilePayloadMessage, PayloadInfoCL);

        // Push the new message to the GW output queue
        if (IRM_NCL > 0 && RouteCL != NULL)
        {
            // Determine the output queue name from the -m --cl destination
            // The first destination value is the PGCS SCN or OS limit
            string OQS = "Intra_Process";
            PGW->PushToOutputQueue(OQS, FilePayloadMessage);
        }
    }
}
```

### 4.3 Alternativa mais simples — não modificar HTGetBind01

A mudança acima é intrusiva: requer acesso ao PP e PGW dentro de HTGetBind01 (que é genérico e pertence ao Common/). A alternativa mais limpa é **não acumular múltiplos `ng -g --b` no mesmo InlineResponseMessage**.

### 4.4 Solução adoptada — mudar `NRSubBind01` para criar mensagens separadas por key

Em vez de adicionar todos os `ng -g --b` ao mesmo InlineResponseMessage, criar uma mensagem separada por key e enviá-la directamente ao HT via `PushToInputQueue`.

**IMPORTANTE — Guarda `NoCL > 2` no PushToInputQueue:** O GW (`Common/src/GW.cpp:229`) verifica `if (NoCL > 2)` antes de aceitar uma mensagem na input queue. Cada mensagem separada precisa de pelo menos 3 CLs. A solução abaixo adiciona `-scn --seq` como terceira CL para satisfazer esta guarda.

**Ficheiro:** `NRNCS/src/NRSubBind01.cpp`

**Antes:**
```cpp
for (unsigned int i = 0; i < Key.size(); i++)
{
    // Change command line from ng -s --b to ng -g --b
    PMB->NewGetCommandLine("0.2", PB->StringToInt(Category.at(0)), Key.at(i), InlineResponseMessage, PCL);
}
```

**Depois (SPEC-021):**
```cpp
// SPEC-021: Create a SEPARATE message per subscription key.
// A single NG message can carry only ONE payload. When the HT
// processes multiple ng -g --b in one message, all -info --payload
// CLs share the same payload — the last one loaded. Fix: each key
// gets its own message routed to the HT, so HTGetBind01 creates
// one InlineResponseMessage per file — each with its own payload.
//
// SCN guard: GW::PushToInputQueue requires NoCL > 2. Each message
// must have at least 3 CLs, so we add -scn --s as the third CL.

NR* PNR = (NR*)PB;
GW* PGW = PNR->PGW;
Block* PHTB = (Block*)PNR->PHT;

// Extract routing from the received message's -m --cl
CommandLine* RoutedCL = NULL;
_ReceivedMessage->GetCommandLine("-m", "--cl", RoutedCL);

vector<string> RouteLimiters;
vector<string> RouteSources;
vector<string> RouteDestinations;

if (RoutedCL != NULL)
{
    RoutedCL->GetArgument(0, RouteLimiters);
    RoutedCL->GetArgument(1, RouteSources);
    RoutedCL->GetArgument(2, RouteDestinations);
}

for (unsigned int i = 0; i < Key.size(); i++)
{
    Message* GetBindMessage = NULL;
    CommandLine* MsgCl = NULL;
    CommandLine* GetBindCL = NULL;

    // Create a new message for this key
    PB->PP->NewMessage(GetTime(), 1, false, GetBindMessage);

    // Add routing: -m --cl with destination = HT (copied from received message)
    if (RoutedCL != NULL && RouteDestinations.size() == 4)
    {
        vector<string> DestCopy = RouteDestinations;
        DestCopy[3] = PHTB->GetSelfCertifyingName();

        PMB->NewConnectionLessCommandLine("0.1", &RouteLimiters, &RouteSources, &DestCopy,
                                          GetBindMessage, MsgCl);
    }

    // Add ng -g --b for this key only
    PMB->NewGetCommandLine("0.2", PB->StringToInt(Category.at(0)), Key.at(i),
                           GetBindMessage, GetBindCL);

    // SPEC-021: Add -scn --s to satisfy PushToInputQueue NoCL > 2 guard.
    // Without this third CL, the message is silently discarded at GW.cpp:229.
    string SCN = NameGenerator::GetInstance().GenerateFromMessage(GetBindMessage);
    PMB->NewSCNCommandLine("0.1", SCN, GetBindMessage, GetBindCL);

    // Push to GW input queue for processing by the HT
    PGW->PushToInputQueue(GetBindMessage);
}
```

**Ficheiro:** `PSS/src/PSSubBind01.cpp` — mesma mudança (o PSS tem a mesma lógica de loop).

### 4.5 Reverter SPEC-018 em HTGetBind01

Com SPEC-021, o `HTGetBind01` nunca processa mais do que um `ng -g --b` Category 18 por InlineResponseMessage. O `ResetPayload()` torna-se desnecessário para este caso. No entanto, manter o `ResetPayload()` é seguro e previne problemas se outras situações reutilizarem o InlineResponseMessage. **Decisão: manter o ResetPayload() por segurança.**

### 4.6 Remoção do one-shot guard em NRInfoPayload01

No NRNCS repo-side, `NRInfoPayload01` (linha 87-112) copia o payload da mensagem recebida para um InlineResponseMessage. Com SPEC-021, cada mensagem recebida tem exactamente UM `-info --payload` e UM payload. O `ResetPayload()` (SPEC-018) continua seguro mas torna-se menos crítico. **Decisão: manter o ResetPayload() por segurança.**

---

## 5. Ficheiros Afectados

| Ficheiro | Mudança | Razão |
|----------|---------|-------|
| `NRNCS/src/NRSubBind01.cpp` | SPEC-021: Criar mensagem separada por key em vez de acumular no InlineResponseMessage | Cada payload precisa da sua própria mensagem; adicionar `-scn --seq` como 3ª CL para NoCL > 2 guard |
| `NRNCS/src/NRSubBind01.h` | Adicionar `#include "GW.h"`, `#include "NR.h"` e `#include "NameGenerator.h"` | Necessário para acesso ao PGW, PHT e geração de SCN |
| `PSS/src/PSSubBind01.cpp` | SPEC-021: Mesma mudança que NRNCS NRSubBind01 (já implementada) | O PSS tem a mesma lógica de loop |
| `PSS/src/PSSubBind01.h` | Adicionar includes necessários (já implementado) | Idem |
| `Common/src/HTGetBind01.cpp` | Manter ResetPayload() (SPEC-018) — não alterar | Prevenção, mesmo que já não seja necessário para múltiplos gets |
| `NRNCS/src/NRInfoPayload01.cpp` | Manter ResetPayload() (SPEC-018) — não alterar | Prevenção |

---

## 6. Porquê esta abordagem

### 6.1 Vantagens

1. **Cada ficheiro tem a sua própria mensagem** — payload e `-info --payload` estão garantidamente associados
2. **Não altera a serialização/deserialização** — o formato da mensagem não muda
3. **Não altera o HTGetBind01** — o código genérico Common/ permanece intacto
4. **Não altera o PGCS** — o PGCS relay processa cada mensagem individualmente (como já faz)
5. **Compatible with SPEC-017/018/019/020** — all previous corrections remain valid

### 6.2 Desvantagens

1. **Mais mensagens no sistema** — 100 subscrições = 100 mensagens separadas em vez de 1
2. **Overhead de routing** — cada mensagem tem o seu próprio `-m --cl` header
3. **Não resolve o problema arquitectural** — a mensagem NG continua a suportar apenas 1 payload; se no futuro se tentar acumular novamente, o bug regressa

### 6.3 Alternativas consideradas e rejeitadas

| Alternativa | Porquê rejeitada |
|-------------|-----------------|
| Modificar serialização para suportar múltiplos payloads | Demasiado intrusiva; altera o formato da mensagem NG; afecta todos os componentes |
| Modificar `HTGetBind01` para criar mensagens separadas | HTGetBind01 é código genérico (Common/); não deve ter acesso directo ao GW ou PP |
| Adicionar marker de payload offset em cada `-info --payload` | Requer mudanças na serialização e deserialização; fragiliza o parser binário |
| Concatenar payloads com delimitadores | Impraticável para dados binários (JPG contém qualquer byte) |

---

## 7. Impacto no NRNCS Source-side InlineResponseMessage

Após SPEC-021, o InlineResponseMessage gerado pelo `NRSubBind01` **já não tem** `ng -g --b` CLs. Em vez disso, cada key é enviada como mensagem separada ao HT. O InlineResponseMessage original pode ficar vazio ou com apenas o `-m --cl`.

**Problema:** O GW verifica `NoCL > 2` antes de empurrar o InlineResponseMessage para a OutputQueue (GW.cpp:293). Se o InlineResponseMessage tiver menos de 3 CLs, é descartado.

**Solução:** O `NRSubBind01` não deve mais adicionar `ng -g --b` ao InlineResponseMessage. Em vez disso, as mensagens separadas são enviadas via `PGW->PushToInputQueue()`. O InlineResponseMessage do NR block fica com apenas o `-m --cl` (1 CL), que será descartado pelo GW (NoCL < 3). Isto é correcto — as respostas reais vêm das mensagens separadas que o HT vai gerar.

---

## 8. Verificação

1. Aplicar SPEC-021 em `NRSubBind01.cpp` e `PSSubBind01.cpp`
2. Compilar: `cd cmake-build-debug && make -j$(nproc)`
3. Deploy para VMs 101/102 (ou usar pull-and-build-vms.sh)
4. Testar com `--publish 0.1`
5. Verificar ZERO erros "hash of the file ... is not the same"
6. Verificar que TODOS os 100 ficheiros recebidos têm conteúdo idêntico aos originais
7. Verificar que o SPEC-019 logging mostra hashes consistentes ao longo do pipeline

---

## 9. Decisões

| # | Decisão | Data | Razão |
|---|---------|------|-------|
| D1 | Mudar NRSubBind01 em vez de HTGetBind01 | 2026-07-11 | HTGetBind01 é código genérico Common/; NRSubBind01 é específico do NRNCS/PSS |
| D2 | Manter ResetPayload() (SPEC-018) | 2026-07-11 | Prevenção contra reutilização futura do InlineResponseMessage |
| D3 | Aplicar mesma mudança ao PSS PSSubBind01 | 2026-07-11 | O PSS tem a mesma lógica de loop e o mesmo bug |
| D4 | Não alterar serialização/deserialização | 2026-07-11 | Muito intrusivo; o formato actual funciona correctamente com 1 payload por mensagem |

---

## 10. Pitfalls

1. **InlineResponseMessage vazio** — Após SPEC-021, o NR block não gera um InlineResponseMessage útil. O GW descarta-o (NoCL < 3). Isto é correcto.
2. **Ordem de chegada** — As mensagens separadas chegam ao HT em ordem, mas o GW pode reordenar com base no tempo agendado. Isto não é problema — cada mensagem é independente.
3. **Memory** — Mais mensagens em memória N em vez de 1. O `MAX_MESSAGES_IN_MEMORY` pode ser atingido. Verificar se o limiar de congestão (`MAX_MESSAGES_IN_MEMORY - 200`) no `CoreNotifyS01` é suficiente.
4. **SCN collisions** — Cada mensagem nova precisa de um SCN único. O `NameGenerator::GenerateFromMessage` gera um hash da mensagem, que será diferente para cada uma (diferentes CLs). Isto funciona correctamente.
5. **Re-delivery timer** — O `CoreRunPeriodic01` re-submete subscrições "Waiting delivery" após timeout. Com SPEC-021, cada subscrição é servida por uma mensagem separada, pelo que o timer não dispara (a não ser que a mensagem se perca).
6. **⚠️ Guarda `NoCL > 2` no PushToInputQueue** — O GW (`Common/src/GW.cpp:229`) rejeita mensagens com ≤ 2 CLs na input queue. As mensagens separadas criadas pelo `NRSubBind01` precisam de pelo menos 3 CLs (`-m --cl` + `-g --b` + `-scn --seq`). Sem a 3ª CL, as mensagens são descartadas silenciosamente e o Repository nunca recebe conteúdo. A implementação do PSSubBind01 já inclui esta correcção via `NewSCNCommandLine`.
