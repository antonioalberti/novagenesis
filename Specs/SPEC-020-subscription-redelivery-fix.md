# SPEC-020: Subscription Re-Delivery Bug — Root Cause e Correcção

**Data:** 2026-07-11
**Estado:** Consolidação — fix implementado no ContentApp, pendente PGCS
**Autor:** Hermes Agent
**Relacionada:** SPEC-014 (data race), SPEC-015 (getline corruption), SPEC-017 (loop sem break), SPEC-018 (ResetPayload), SPEC-019 (hash logging)

---

## 1. Problema

O ContentApp Repository recebe correctamente o payload (hash confere) mas a subscription nunca é actualizada para "Delivered", causando re-subscrição infinita a cada ~60s.

**Log do repo61:**
```
(RTT from NRNCS was 0.0102803020 seconds for the key 789703CA)
(ContentApp received payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
(Subscription 0 has Status Waiting delivery, Key = 789703CA, HasContent = 1, Time from subscription = 27.3s)
--- 60s depois ---
(RTT from NRNCS was 87.3206969120 seconds for the key 789703CA)
(ContentApp received payload: file=Service_Offer_1046744630.txt, size=36 bytes, hash=789703CA)
(Subscription 0 has Status Waiting delivery, Key = 789703CA, HasContent = 1, Time from subscription = 147.3s)
```

O RTT aumenta 60s de cada vez — a subscription está a ser re-submetida num timer periódico.

## 2. Causa Raiz

### 2.1 Três actores no ciclo de vida da subscription

| Actor | Ficheiro | Função |
|-------|----------|--------|
| **CoreDeliveryBind01** | `ContentApp/src/CoreDeliveryBind01.cpp` | Processa `-d --b` → marca `HasContent = true` |
| **CoreInfoPayload01** | `ContentApp/src/CoreInfoPayload01.cpp` | Processa `-info --payload` → salva payload + marca "Delivered" |
| **CoreRunPeriodic01** | `ContentApp/src/CoreRunPeriodic01.cpp` | Timer periódico: re-submete subscrições ainda "Waiting delivery" |

### 2.2 Ciclo de falha

```
1. Inicialização:
   Subscription[0].Status = "Waiting delivery"
   Subscription[0].HasContent = false

2. CoreDeliveryBind01 processa "-d --b" (entrega anunciada):
   Subscription[0].HasContent = true    ← ANTES do payload chegar

3. CoreInfoPayload01 processa "-info --payload" (payload recebido):
   if (PS->Status == "Waiting delivery" && !PS->HasContent)  ← CONDIÇÃO ANTIGA
   → FALHA porque HasContent já é true
   → Subscription[0].Status continua "Waiting delivery"

4. CoreRunPeriodic01 (timer ~60s):
   if ((GetTime() - PS->Timestamp) > TIMEOUT && PS->Status == "Waiting delivery")
   → Re-submete subscription para NRNCS

5. NRNCS re-entrega o mesmo payload
   → CoreInfoPayload01 recebe outra vez
   → Mesma condição falha
   → Loop infinito
```

### 2.3 Porque o RTT aumenta 60s de cada vez

Cada re-subscrição passa pelo NRNCS que mantém o payload em cache. O RTT reportado é o tempo desde a subscrição original, não o tempo desde a re-subscrição. O incremento de 60s é o intervalo do `TIMEOUT` em `CoreRunPeriodic01` (linha 295).

## 3. Correcção (SPEC-020)

### 3.1 Remover o guard `!PS->HasContent`

O `CoreDeliveryBind01` marca `HasContent = true` antes do `CoreInfoPayload01` processar o payload. Portanto, `!PS->HasContent` é sempre `false` quando o `CoreInfoPayload01` corre, bloqueando a actualização.

**Condição antiga (BUG):**
```cpp
if (PS->Status == "Waiting delivery" && !PS->HasContent)
```

**Condição correcta (SPEC-020):**
```cpp
if (PS->Status == "Waiting delivery")
```

### 3.2 Manter "Processing required" — não forçar "Delivered"

O `CoreRunEvaluate01` (linha 501) depende de `Status == "Processing required"` para processar o acceptance. Se o SPEC-020 forçar `Status = "Delivered"` imediatamente, o acceptance nunca é gerado.

**Correcção correcta:**
```cpp
if (PS->Status == "Waiting delivery")
{
    PS->Status = "Processing required";   // CoreRunEvaluate01 precisa disto
    PS->FileName = Values.at(0);
    break;  // SPEC-017: apenas uma subscription por mensagem
}
```

**Não fazer:**
```cpp
PS->HasContent = true;    // CoreDeliveryBind01 já o fez
PS->Status = "Delivered"; // ❌ Mata o acceptance!
```

### 3.3 Porque não há re-delivery sem "Delivered"

O `CoreRunPeriodic01` (linha 295) só re-submete se:
```cpp
if ((GetTime() - PS->Timestamp) > TIMEOUT && PS->Status == "Waiting delivery")
```

Com `Status == "Processing required"`, a condição **não** é satisfeita. O `CoreRunPeriodic01` nunca re-submete uma subscription que já está a ser processada. O "Delivered" é desnecessário para prevenir re-delivery.

### 3.4 Ciclo completo

```
Waiting delivery ──CoreInfoPayload01──→ Processing required ──CoreRunEvaluate01──→ Delete
    ↑                    (condição fix)        │                                      │
CoreDeliveryBind01                              │ (cria acceptance,                     │
(arruma HasContent)                             │  publica resposta)                    │
                                                └── acceptance → Source ──→ contentpublish

## 4. Ficheiros Afectados

| Ficheiro | Código actual | Acção |
|----------|--------------|-------|
| `ContentApp/src/CoreInfoPayload01.cpp` | `if (PS->Status == "Waiting delivery")` ✅ | Fix já aplicado localmente |

## 5. Verificação

1. Compilar: `cd build && make -j$(nproc)`
2. Deploy para repo61 e source36
3. Iniciar NRNCS + PGCS + ContentApp em ambas as VMs
4. Verificar log do Repo: **ZERO** ocorrências de `(The following message contains a subscription of delayed deliveries)`
5. Verificar que cada subscription mostra `Status = Delivered` após a primeira entrega
6. Testar com `--publish 0.1` — verificar 100/100 ficheiros sem re-delivery

## 6. Nota: PGCS

O `PGCS/src/CoreInfoPayload01.cpp` tem a condição `PS->Status == "Waiting delivery" && PS->HasContent == true` — diferente do bug do ContentApp. O PGCS **não** tem o re-delivery porque:
- O `CoreRunPeriodic01` que re-submete subscrições é do ContentApp, não do PGCS
- O PGCS é um relay; as subscriptions de conteúdo são geridas pelo ContentApp

Portanto, PGCS não precisa de correcção SPEC-020.

## 7. Decisões

| # | Decisão | Data | Razão |
|---|---------|------|-------|
| D1 | Remover `!PS->HasContent` em vez de reordenar CoreDeliveryBind01 | 2026-07-11 | `HasContent` é semanticamente correcto como sinal de "entrega anunciada"; a condição correcta é verificar só o Status |
| D2 | Manter "Processing required" + "Delivered" (dupla atribuição) | 2026-07-11 | "Processing required" é usado por outro código que verifica o estado da subscription; "Delivered" é o estado terminal oficial |