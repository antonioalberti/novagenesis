# Modelo NG: Pub/Sub Invertido — Princípios de Desenho

**Data:** 2026-07-15
**Autor:** Antonio Alberti

---

## 1. O Modelo NG é Diferente do MQTT

No MQTT, o broker faz **push** do conteúdo para os subscritores:
```
Publicador → Broker → [push] → Subscritor A
                         → [push] → Subscritor B
```

No NovaGenesis, o modelo é **invertido**:
```
Publicador → NRNCS (cache local)
                 ↓
Subscritor → [ng -s --b] → NRNCS → [serve do cache] → Subscritor
```

## 2. Regras de Ouro

### Regra 1: NRInfoPayload01 NUNCA reencaminha conteúdo

O `-info --payload` que chega com a publicação serve **APENAS** para:
1. Extrair o `Payload[]` da mensagem recebida
2. Guardar em disco no path do NRNCS (`IO/NRNCS/<nome_ficheiro>`)

Nunca deve criar mensagens `-d --b`. Nunca deve usar `PGW->PushToInputQueue()`.

### Regra 2: A entrega só acontece via assinatura

O conteúdo só é enviado ao subscritor quando:
1. O subscritor envia `ng -s --b` (assinatura)
2. O `NRSubBind01` faz `ng -g --b` ao HT local
3. O `HTGetBind01` (cat=18) lê o ficheiro do cache e cria `-d --b` + `-info --payload` + payload

### Regra 3: Cache é idempotente

O `NRInfoPayload01` deve verificar se o ficheiro já existe antes de guardar. Se já existe, salta (a publicação pode chegar mais que uma vez).

### Regra 4: Se o ficheiro não está em cache, é ALARME

O `HTGetBind01` (cat=18) deve verificar o retorno de `ConvertPayloadFromFileToCharArray()`. Se falhar, emite ALARME — o ficheiro não foi cacheado pelo `NRInfoPayload01`.

## 3. Fluxo Correcto (Passo a Passo)

```
┌─ ContentApp Source ─────────────────────┐
│ CoreRunPublish02:                        │
│  • Lê ficheiro do disco                  │
│  • Cria mensagem com:                    │
│    - -m --cl (routing)                   │
│    - -p --notify (cat=18, hash→nome)     │
│    - -info --payload (metadados)         │
│    - payload (conteúdo do ficheiro)      │
│    - -p --b (proveniência)               │
│    - -scn --seq                          │
│  • Envia para o NRNCS local              │
└──────────────────────────────────────────┘
                     │
                     ▼
┌─ NRNCS Source ───────────────────────────┐
│                                          │
│ ① NRPubNotify01:                         │
│    • Guarda binding no HT:               │
│      cat=18, hash(conteúdo)→nome         │
│    • Envia ng -notify ao Repo            │
│                                          │
│ ② NRInfoPayload01:                       │
│    • Extrai Payload[] e Size             │
│    • Guarda em IO/NRNCS/<nome_ficheiro>  │
│    • Se já existe, salta (idempotente)   │
│    • NÃO reencaminha                     │
│                                          │
└──────────────────────────────────────────┘
                     │ (ng -notify)
                     ▼
┌─ ContentApp Repo ────────────────────────┐
│ CoreNotifyS01:                            │
│  • Cria Subscription com:                │
│    Status="Waiting delivery"             │
│    HasContent=false                      │
│  • Envia ng -s --b ao Source             │
└──────────────────────────────────────────┘
                     │ (ng -s --b)
                     ▼
┌─ NRNCS Source ───────────────────────────┐
│                                          │
│ ③ NRSubBind01:                           │
│    • Extrai routing da msg recebida      │
│    • Cria nova msg com:                  │
│      - -m --cl (para o HT)              │
│      - -g --b (cat=18, key=hash)        │
│      - -scn --s                          │
│    • Envia para o InputQueue do GW       │
│                                          │
│ ④ HTGetBind01:                           │
│    • Lê binding → nome do ficheiro       │
│    • Lê ficheiro do disco:               │
│      IO/NRNCS/<nome_ficheiro>            │
│      → SE NÃO EXISTE: ALARME            │
│    • Cria InlineResponseMessage com:     │
│      - -d --b (delivery)                 │
│      - -info --payload + payload         │
│      - -scn --ack (acknowledgement)      │
│    • Devolve ao NRSubBind01              │
│                                          │
│ ⑤ GW reencaminha para o Repo            │
│                                          │
└──────────────────────────────────────────┘
                     │ (-d --b + -info --payload + payload)
                     ▼
┌─ ContentApp Repo ────────────────────────┐
│ CoreInfoPayload01:                        │
│  • Verifica HasPayloadFlag==true         │
│  • Guarda ficheiro em disco              │
│  • Actualiza Subscription                │
│  • Verifica hash do conteúdo             │
└──────────────────────────────────────────┘
```

## 4. O Que NÃO Fazer

- ❌ `NRInfoPayload01` NUNCA deve usar `NewConnectionLessCommandLine()`
- ❌ `NRInfoPayload01` NUNCA deve usar `PGW->PushToInputQueue()`
- ❌ `NRInfoPayload01` NUNCA deve criar mensagens com `-d --b`
- ❌ O `HTGetBind01` NUNCA deve assumir que o ficheiro existe em disco — verificar sempre
- ❌ NUNCA acumular múltiplos `-info --payload` na mesma mensagem (cada payload precisa da sua própria mensagem)

## 5. Porquê Este Modelo?

1. **Desacoplamento**: O publicador não precisa de saber quem são os subscritores
2. **Escalabilidade**: O cache local permite servir múltiplos subscritores sem repetir a publicação
3. **Fiabilidade**: O subscritor pede o conteúdo quando está pronto para o receber
4. **Auditabilidade**: O `HTGetBind01` pode verificar integridade antes de entregar