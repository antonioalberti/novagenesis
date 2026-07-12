# SPEC-018: SetPayloadFromCharArray One-Shot Guard — Payload Sticks on Reused InlineResponseMessage

**Data:** 2026-07-10
**Estado:** Fix proposta
**Autor:** Hermes Agent
**Relacionada:** SPEC-015 (RC2 — corrupção binária), SPEC-017 (RC3 — content↔name correlation)

---

## 1. Problema

Após SPEC-015 e SPEC-017, o hash mismatch persiste em 17/100 ficheiros. Todos os 17 ficheiros recebidos no ContentApp Repository contêm bytes idênticos ao ficheiro `00019-alpine-ng-source.jpg`, independentemente do nome do ficheiro. O NRNCS tem todos os 100 ficheiros armazenados correctamente (verificado por hash), eliminando o caminho de armazenamento como causa.

## 2. Diagnóstico Experimental

### 2.1 Hash verification

A função hash NG (`MurmurHash3_x86_32`, seed=3571, output big-endian 8 hex chars) foi portada para Python e verificada:

- Source1 (100 ficheiros originais): 100/100 hashes coincidem com os hashes do publisher no log
- NRNCS (100 ficheiros no NRNCS): 100/100 conteúdo idêntico a Source1, 100/100 hashes correctos
- Repository1 (99 ficheiros recebidos): 82 correctos, 17 com conteúdo idêntico a 00019

### 2.2 Padrão de falha

Os 17 ficheiros errados no Repository1 são todos byte-for-byte idênticos a `00019-alpine-ng-source.jpg` (289342 bytes). Os ficheiros originais correspondentes têm tamanhos diferentes (288871B a 289443B), confirmando que não são truncamentos ou corrupções — são conteúdo completamente errado.

### 2.3 Eliminação de causas anteriores

| Root Cause | SPEC | Estado | Aplicada? |
|---|---|---|---|
| RC1 — ReceiveFragment data race | SPEC-014 | FIXED | Sim |
| RC2 — ExtractPayloadCharArrayFromMessageCharArray | SPEC-015 | FIXED | Sim (chamada comentada) |
| RC3 — Subscription loop sem break | SPEC-017 | FIXED | Sim (break adicionado) |
| **RC4 — SetPayloadFromCharArray one-shot guard** | **SPEC-018** | **Nova** | **Não** |

## 3. Causa Raiz (RC4)

### 3.1 O guard one-shot

`Message::SetPayloadFromCharArray()` (Message.cpp:817-843) tem um guard que impede o payload de ser substituído:

```cpp
int Message::SetPayloadFromCharArray(char* _Value, long long _Size)
{
  int Status = ERROR;

  if (PayloadSize == 0 && DeletePayloadArray == false)  // <-- GUARD
  {
    if (_Size > 0)
    {
      PayloadSize = _Size;
      Payload = new char[PayloadSize];
      // ... copy ...
      Status = OK;
      DeletePayloadArray = true;
      HasPayloadFlag = true;
    }
  }

  return Status;
}
```

O mesmo guard existe em `ConvertPayloadFromFileToCharArray()` (Message.cpp:1171):
```cpp
if (PayloadSize == 0 && DeletePayloadArray == false && Status == OK && HasPayloadFlag == true)
```

### 3.2 Como o InlineResponseMessage é reutilizado

O fluxo de subscrição de conteúdo é:

1. ContentApp Repository cria subscrições: `ng -s --b 0.1 [ < 1 s 18 > < N s key1 key2 ... keyN > ]`
2. PSS recebe a mensagem. `PSSubBind01::Run()` converte cada key num `ng -g --b` command line **no mesmo InlineResponseMessage** (loop na linha 77-81)
3. O InlineResponseMessage vai para o PGCS HT. `Block::Run()` processa todos os `ng -g --b` CLs sequencialmente, passando o **mesmo** `_InlineResponseMessage` a cada chamada de `HTGetBind01::Run()`
4. `HTGetBind01::Run()` para Category 18 (linha 121-137):
   - `InlineResponseMessage->SetMessage(..., _Values->at(0), ...)` — define o nome do ficheiro
   - `InlineResponseMessage->ConvertPayloadFromFileToCharArray()` — carrega o ficheiro para o payload
   - `PMB->NewInfoPayloadCommandLine(...)` — adiciona `-info --payload`

### 3.3 Cenário de falha

Com 19 keys numa única subscrição:

1. `HTGetBind01` processa o 1º `ng -g --b` (ex: key=A0638CC0, file=00003):
   - `SetMessage` define PayloadFile = 00003-alpine-ng-source.jpg
   - `ConvertPayloadFromFileToCharArray` carrega 00003 → PayloadSize > 0, DeletePayloadArray = true
   - Adiciona `-info --payload 0.1 [ < 1 s 00003-alpine-ng-source.jpg > ]`
   - **Mas este CL fica na mesma mensagem que os outros 18 gets ainda não processados**

2. `HTGetBind01` processa o 2º `ng -g --b` (ex: key=4AF7B897, file=00019):
   - `SetMessage` define PayloadFile = 00019-alpine-ng-source.jpg
   - `ConvertPayloadFromFileToCharArray` **FALHA SILENCIOSAMENTE** porque `PayloadSize != 0` (guard: `PayloadSize == 0 && DeletePayloadArray == false`)
   - O payload continua a ser o conteúdo de 00003 (ou do primeiro ficheiro processado)
   - Adiciona `-info --payload 0.1 [ < 1 s 00019-alpine-ng-source.jpg > ]`

3. O mesmo acontece para os restantes 17 keys: o nome do ficheiro é correcto no `-info --payload` CL, mas o Payload char array contém sempre o conteúdo do primeiro ficheiro processado.

4. Quando o InlineResponseMessage é serializado por `ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray()`, o Payload único é anexado após todos os CLs. No receptor, `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` extrai todos os CLs e depois o payload (um único bloco). Todos os `-info --payload` CLs referenciam o mesmo payload.

5. O ContentApp processa cada `-info --payload` sequencialmente. Cada um define o filename e escreve o mesmo Payload para esse ficheiro. Resultado: todos os ficheiros têm o mesmo conteúdo (o do primeiro entregue).

### 3.4 Porque é que 00019 é o conteúdo "vencedor"

A ordem de processamento das keys no HT depende da ordem dos `ng -g --b` CLs, que por sua vez depende da ordem no vector `Key` em `PSSubBind01`. A primeira key processada cujo ficheiro existe no HT determina o payload que "cola". O log mostra que 00006 e 00019 foram resolvidos nas primeiras rondas, sugerindo que foram dos primeiros a ser processados. O conteúdo de 00019 é o que aparece em todos os 17 ficheiros errados, indicando que 00019 foi o primeiro ficheiro cujo payload foi carregado com sucesso.

## 4. Correcção

### 4.1 Reset do payload antes de cada SetPayloadFromCharArray / ConvertPayloadFromFileToCharArray

A solução correcta é garantir que o payload é limpo antes de carregar um novo ficheiro. Adicionar um método `ResetPayload()` que liberta o payload existente e reseta as flags, e chamá-lo antes de cada carregamento.

**Alternativa mais simples:** Remover o guard one-shot de `SetPayloadFromCharArray` e `ConvertPayloadFromFileToCharArray`, permitindo que o payload seja substituído. Mas isto pode causar memory leak se o Payload anterior não for libertado.

**Solução adoptada:** Adicionar `ResetPayload()` e chamar explicitamente antes de cada `SetPayloadFromCharArray` / `ConvertPayloadFromFileToCharArray` nos sítios onde o InlineResponseMessage é reutilizado.

### 4.2 Ficheiros afectados

| Ficheiro | Mudança |
|---|---|
| `Common/src/Message.h` | Declarar `void ResetPayload();` |
| `Common/src/Message.cpp` | Implementar `ResetPayload()`; modificar guards em `SetPayloadFromCharArray` e `ConvertPayloadFromFileToCharArray` |
| `Common/src/HTGetBind01.cpp` | Chamar `ResetPayload()` antes de `ConvertPayloadFromFileToCharArray()` |
| `NRNCS/src/NRInfoPayload01.cpp` | Chamar `ResetPayload()` antes de `SetPayloadFromCharArray()` |
| `PSS/src/PSInfoPayload01.cpp` | Chamar `ResetPayload()` antes de `SetPayloadFromCharArray()` |

## 5. Verificação

1. Aplicar a correcção
2. Compilar: `cd cmake-build-debug && make -j$(nproc)`
3. Deploy para VMs 101/102
4. Testar com `--publish 0.1`
5. Verificar ZERO erros "hash of the file ... is not the same"
6. Verificar que todos os 100 ficheiros recebidos têm conteúdo idêntico aos originais
