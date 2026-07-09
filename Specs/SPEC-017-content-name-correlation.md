# SPEC-017: Content-Name Correlation Bug — CoreInfoPayload01 Subscription Update Loop

**Data:** 2026-07-09
**Estado:** Fix proposta
**Autor:** Hermes Agent
**Relacionada:** SPEC-015 (RC2 — corrupção binária), SPEC-016 (verificação de hash)

---

## 1. Problema

Após a SPEC-015 (remoção do `ExtractPayloadCharArrayFromMessageCharArray()`), o hash mismatch persiste em ~28/100 ficheiros. A SPEC-016 confirmou que em pelo menos 1 caso (`00091-alpine-ng-source.jpg`) o conteúdo binário recebido está correcto mas foi guardado com o nome errado. Isto é o **RC3 (Root Cause 3)**: bug de correlação conteúdo↔nome de ficheiro.

## 2. Causa Raiz

### 2.1 O loop sem `break`

Em `CoreInfoPayload01::Run()` (ContentApp e PGCS), o código que actualiza as subscrições percorre **todas** as subscrições e atribui o mesmo filename a todas que tenham `HasContent=true`:

```cpp
// ContentApp/src/CoreInfoPayload01.cpp:100-131
for (unsigned int i = 0; i < PCore->Subscriptions.size(); i++)
{
    Subscription* PS = PCore->Subscriptions[i];

    if (PS->Status == "Waiting delivery" && PS->HasContent)
    {
        PS->Status = "Processing required";
        PS->FileName = Values.at(0);  // <-- BUG: mesmo filename para TODAS
        // <-- falta break aqui!
    }
}
```

### 2.2 Cenário de falha

1. Duas entregas (`-info --payload`) chegam em mensagens separadas, uma para o ficheiro `00022` e outra para `00091`
2. `CoreDeliveryBind01` processa dois `-d --b` e coloca `HasContent=true` nas subscrições de **ambos** os ficheiros
3. `CoreInfoPayload01` processa o `-info --payload` de `00022`:
   - Guarda o payload em `00022-alpine-ng-source.jpg` ✓
   - Percorre subscrições → encontra AMBAS com `HasContent=true`
   - Atribui `FileName = "00022-alpine-ng-source.jpg"` a **ambas** as subscrições ✗
4. `CoreInfoPayload01` processa o `-info --payload` de `00091`:
   - Guarda o payload em `00091-alpine-ng-source.jpg` (sobrepõe `00022`!)
   - Atribui `FileName = "00091-alpine-ng-source.jpg"` a **ambas** ✗
5. `CoreRunEvaluate01` verifica hash de `00022` → lê `00091-alpine-ng-source.jpg` (que tem conteúdo de `00091`) → hash mismatch
6. O mesmo para `00091` → se leu o mesmo ficheiro, o hash também não corresponde

### 2.3 Porque só afecta alguns ficheiros

O bug só ocorre quando duas ou mais entregas têm o `HasContent` definido antes do `CoreInfoPayload01` processar o `-info --payload` correspondente. Isto depende da ordem de chegada das mensagens `-info --payload` vs `-d --b`, que varia com o timing da rede NRNCS.

---

## 3. Correcção Proposta

### 3.1 Adicionar `break` após actualizar a primeira subscrição

Cada mensagem `-info --payload` corresponde a **exactamente uma** entrega de ficheiro. O loop deve actualizar apenas **uma** subscrição e sair.

**Ficheiro:** `ContentApp/src/CoreInfoPayload01.cpp` (linha 121)
**Ficheiro:** `PGCS/src/CoreInfoPayload01.cpp` (linha 111)

```cpp
if (PS->Status == "Waiting delivery" && PS->HasContent)
{
    PS->Status = "Processing required";
    PS->FileName = Values.at(0);
    break;  // SPEC-017: Apenas uma subscricao por mensagem -info --payload
}
```

### 3.2 Justificação

- A ordem de subscrições no vector é a ordem de criação (primeiro a subscrever = primeiro no vector)
- `CoreDeliveryBind01` processa os `-d --b` na ordem de chegada e marca `HasContent=true` na subscrição correspondente
- A primeira subscrição com `HasContent=true` é a que corresponde à mensagem `-info --payload` actual
- O `break` garante que cada `-info --payload` actualiza exactamente uma subscrição

### 3.3 Segurança

Mesmo que a ordem de subscrições não corresponda à ordem de entregas (caso raro), o pior caso é uma subscrição ficar em "Waiting delivery" sem nunca ser actualizada — em vez de DUAS subscrições ficarem com o filename errado. O timeout do NRNCS irá re-entregar o conteúdo perdido.

---

## 4. Ficheiros Afectados

| Ficheiro | Linha | Mudança |
|----------|-------|---------|
| `ContentApp/src/CoreInfoPayload01.cpp` | 121 | Adicionar `break;` após `PS->FileName = Values.at(0);` |
| `PGCS/src/CoreInfoPayload01.cpp` | 111 | Adicionar `break;` após `PS->FileName = Values.at(0);` |

---

## 5. Verificação

1. Aplicar a correcção (adicionar `break` em ambos os ficheiros)
2. Compilar: `cd cmake-build-debug && make -j$(nproc)`
3. Testar com `--publish 0.1`
4. Verificar ZERO erros "hash of the file ... is not the same"
5. Opcional: copiar ficheiros recebidos e comparar com originais usando o script de hash NG