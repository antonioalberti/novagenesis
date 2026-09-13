# NG-020 / SPEC-033 R3-B3 — Auditoria read-only do caminho de recepção de rede

## Escopo

Auditoria sem alteração de produção do caminho `GW.cpp:603–622`, com os seus owners imediatos:

- `Common/src/GW.cpp::Gateway()`;
- `Common/src/NGAL_CS.cpp::DeliverToGateway()`;
- `PGCS/src/NGAL_Transport_RAW.cpp::ReceiveDispatcher()`;
- `Common/src/NGAL_SAR.cpp::ReceiveFragment()`.

B1 permanece aceite independentemente em `e086868`; B2 permanece aceite apenas na fronteira local em `53f06ff`. B3 não está autorizado para implementação.

## Fluxo verificado

```text
RAW recvmmsg pool
    -> TempBuffer heap copy
    -> NGAL_SAR::ReceiveFragment
    -> CompletedBuffer ownership transfer
    -> NGAL_CS::DeliverToGateway copy
    -> delete[] CompletedBuffer
    -> GW::NetworkReceiveQueue
    -> GW::Gateway NewMessage + deserialize
    -> PushToInputQueue
    -> delete[] buffer
```

## Achados

### 1. Validação antes do SAR

`ReceiveDispatcher()` rejeita antes da cópia/SAR:

- frames truncados ou maiores que a capacidade do pool;
- frames menores que Ethernet + 8 bytes;
- protocolos diferentes de `13330`;
- payloads menores que 8 bytes.

`NGAL_SAR::ReceiveFragment()` inicializa `CompletedBuffer = 0` e `CompletedSize = 0`, valida headers, MN, BlockSize, tamanho, SN, payload e limites de reassembly antes de alocar/copiar.

### 2. Ownership do buffer temporário

`TempBuffer` é criado pelo dispatcher, passado ao SAR e libertado em `delete[] TempBuffer` após o retorno. O SAR copia apenas os bytes validados para o seu próprio buffer; não retém o ponteiro do pool nem `TempBuffer`.

### 3. Ownership do buffer completo

Ao completar a reassembly, o SAR:

1. atribui `FB->Buffer` a `CompletedBuffer`;
2. atribui o tamanho a `CompletedSize`;
3. define `FB->Buffer = 0`;
4. remove o `FragmentBuffer`.

Isto transfere explicitamente a ownership do buffer completo para o dispatcher.

### 4. Entrega ao GW

`NGAL_CS::DeliverToGateway()` rejeita `PGW == 0`, buffer nulo ou tamanho não positivo. Para entrada válida, cria uma cópia própria sob `NetworkReceiveQueueMutex`, insere o par `(BufferCopy, MessageSize)`, notifica e retorna `OK`.

O dispatcher ignora o retorno de `DeliverToGateway()`, mas liberta sempre a sua própria `CompletedBuffer`. Para entradas inválidas, `DeliverToGateway()` não copia; a ownership original continua no dispatcher e é libertada por ele.

### 5. Consumo pelo GW

`GW::Gateway()` remove cada entrada da `NetworkReceiveQueue`, guarda `buffer`/`size`, e apenas desserializa se `buffer != 0 && size > 0`. Cria `PM` apenas dentro de `if (PP->NewMessage(...) == OK)`. Depois do bloco, liberta sempre `buffer` em `delete[] buffer`.

No caminho de falha de `NewMessage()`, `PM` começa explicitamente nulo, não entra em `PushToInputQueue()`, e o buffer continua a ser libertado.

## Riscos potenciais não demonstrados

- `new unsigned char[numbytes]` no dispatcher não está protegido por `try/catch`; uma excepção `bad_alloc` pode terminar o processo.
- `new char[MessageSize]` em `NGAL_CS::DeliverToGateway()` também não está protegido; nesse caso a cópia não é inserida e a ownership da entrada original permanece no dispatcher apenas se a excepção for tratada, o que actualmente não ocorre.
- `new` pode falhar de modo semelhante em outras etapas do dispatcher/SAR, embora o SAR use `try/catch` para o buffer de reassembly.
- O retorno de `DeliverToGateway()` não é registado nem convertido em telemetria de drop.

Estes são riscos de excepção/telemetria e não constituem, nesta auditoria, um defeito B3 demonstrado de cleanup normal, uso-after-free ou dereference. Não foram alterados.

## Veredicto Astra — 2026-09-10

`B3 — not implemented / no defect demonstrated.`

A revisão confirmou que a ownership normal é consistente: o SAR desanexa o buffer completo antes de remover o registo, o dispatcher liberta o buffer depois da cópia para o GW e o GW liberta a cópia da queue após o processamento. A falha de `NewMessage()` não alcança `PushToInputQueue()` neste caminho porque `PM` começa nulo e o processamento está protegido pelo retorno `OK`.

Este veredicto não é uma certificação de robustez perante `bad_alloc`, nem valida os contratos internos de conversão e queue. As alocações sem `catch` e o retorno ignorado de `DeliverToGateway()` permanecem riscos separados e não autorizam alterações B3.

Nenhum teste adicional ou alteração de produção é necessário para esta disposição limitada. B1 e B2 permanecem aceites nos seus escopos próprios; C permanece bloqueada.

## Conclusão da auditoria

- não foi demonstrado uso-after-free;
- não foi demonstrado double-free;
- não foi demonstrado leak normal do `TempBuffer`, `CompletedBuffer` ou `buffer` da queue;
- falha normal de `NewMessage()` não alcança B2 e liberta o buffer;
- não foi demonstrada necessidade de alteração de produção em B3.

Classificação provisória: **B3 sem defeito de produção demonstrado; não implementar B3**.

Os riscos de `bad_alloc` e o retorno ignorado de `DeliverToGateway()` devem permanecer como questões separadas. Se forem considerados requisitos de robustez, exigem uma SPEC própria ou uma emenda B3 explicitamente autorizada; não devem ser incorporados por inferência.

## Evidência

- `Common/src/GW.cpp:599–629`
- `Common/src/NGAL_CS.cpp:42–74`
- `PGCS/src/NGAL_Transport_RAW.cpp:331–415`
- `Common/src/NGAL_SAR.cpp:239–493`
- SPEC-033 R3 B2 e aceitação Astra local em `53f06ff`
