# MESSAGE-LIFECYCLE — Inventory and ownership contract (SPEC-033 Phase C)

**Status:** DRAFT for Astra + Antonio review
**Author:** Hermes
**Date:** 2026-09-07
**Inputs:** code reading of Common/src (Process, GW, Block, Message, NGAL_CS),
PGCS/src (Core, PG), plus SPEC-033 §2 and Astra review emendas.

---

## 1. Fluxo real (situacao atual)

```
[Caminho A — rede]
RAW socket thread
  → NGAL_CS::DeliverToInMemQueue (char* + size; SEM Message ainda)
  → GW loop Step 3: Process::NewMessage()        [OWNERSHIP NASCE NO PROCESS]
  → PM->SetMessageFromCharArray + Convert...2 (parse)
  → GW::PushToInputQueue(PM)                     [RETENCAO: InputQueue]
  → GW pop (due time) → Block::Run(PM1, PM2)
      → OkToRun(PM1)? (guard SPEC-003)
      → para cada CL: Action::Run(PM1, PCL, ScheduledMessages, PM2)
          → respostas: Process::NewMessage() + MarkToDelete imediato
          → PM1->MarkToDelete() (somente se ApplicationDeleted==DELETED_BY_CORE)
  → PP->DeleteMarkedMessages()                   [DESTRUICAO]
  → PM1 = NULL

[Caminho B — execucao local direta, sem GW]
Core::ListBindings / similar:
  → Process::NewMessage (PIM)
  → PIM->MarkToDelete()
  → Block::Run(PIM, InlineResponseMessage)
  → (destruicao so acontece no proximo DeleteMarkedMessages do GW loop)

[Caminho C — IPC/SHM entre processos]
Processo emissor → SHM → GW loop Step 4 → NewMessage + parse → igual A.
```

### Retentores de Message* (inventario)

| Retentor | Tipo | Quando adquire | Quando solta | Observacoes |
|---|---|---|---|---|
| Process::Messages[i] | slot array, dono | NewMessage | delete (DeleteMessage/DeleteMarkedMessages/DeleteMessages) | PROPRIETARIO de facto |
| GW::InputQueue | priority_queue<Message*> | PushToInputQueue | pop no loop do GW | comparador desreferencia antes do Run (ABA/C6) |
| GW::OutputQueues | map<string, priority_queue<Message*>> | PushToOutputQueue | pop no envio | mesmo padrao do InputQueue |
| Block::Run local | vector<Message*> ScheduledMessages | (nunca hoje — SPEC-003 removeu os pushes) | fim do Run | SEMPRE VAZIO (Block.cpp:357-360) |
| InlineResponseMessage | Message* por referência | Action cria via NewMessage | GW/Caller processa e marca | percorre o Run; deve ter MarkToDelete garantido |
| Block membros | NENHUM Message* membro | — | — | confirmado por grep em *.h |
| Script ciclo de vida | conforme implementado | — | — | PENDENTE: audit de C.2 (usuario) |
| NGAL_CS | nunca cria Message | — | — | entrega char* cru (evita data race em NewMessage) |

### Pontos de falha ja identificados (ligados a lifetime)

1. **C5:** mensagem com ApplicationDeleted==DELETED_BY_APP e app que nunca
   reseta a flag → nunca deletada → vazamento lento do container.
2. **Comparador da priority_queue** desreferencia mensagens para ordenar
   (DereferenceCompareNode) — se DeleteMessage ocorrer entre pop e comparacao,
   UAF antes do guard do OkToRun.
3. **Caminho B:** ListBindings roda fora do loop do GW; a destruicao depende do
   proximo ciclo do GW (spec de shutdown: fechar sem pendencias — invariante 8
   do Astra).
4. **EraseMessage** remove o ponteiro SEM destruir (astra finding 6): qualquer
   caller hoje vaza a mensagem. Grep: chamado em 0 lugares fora do Process
   (verificar antes de Fase B; manter API mas tornar explicito).

## 2. Contrato-alvo (implementado na Fase B)

1. **Process e o UNICO proprietario.** Toda outra entidade tem RETENCAO.
2. Retencao = handle `{slot, generation}` capturado enquanto a mensagem esta
   viva. Resolve para `Message*` somente se generation casar (impede ABA).
3. `Run` mantem uma retencao implicita durante a execucao (a mensagem nao pode
   ser destruida entre pop e fim do Run — mesmo que MarkToDelete ocorra).
4. `MarkToDelete` solicita; destruicao efetiva ocorre em DeleteMarkedMessages
   APENAS se nenhuma retencao ativa.
5. Filas (Input/Output) guardam handles; o comparador ordena por
   `(due_time, sequence)` armazenados NO SLOT (nunca desreferencia o Message*).
6. Cancelamento (limpar fila) libera a retencao sem executar.
7. Shutdown: destruir todas as mensagens com retencoes resolvidas; nenhuma
   dependencia de um proximo ciclo do GW.

### Invariantes testaveis (substituem as globais da SPEC-033, emendas do Astra)

- T1: toda mensagem viva tem exatamente 1 dono (slot do Process).
- T2: resolve(handle) com generation obsoleta → NULL (nunca mensagem errada).
- T3: numero de mensagens vivas == slots ocupados, apos sucesso E falha.
- T4: parse com erro preserva o objeto e nunca publica mensagem executavel.
- T5: nenhuma mensagem destruida enquanto queue/Run/Script a retém.
- T6: shutdown zera vivos e retencoes.

## 3. Auditoria C.2 — RESPONDIDA (varredura Common/PGCS/NRNCS/ContentApp, 2026-09-07)

**Nota:** não existe uma classe "Script" isolada. O "Script do ciclo de vida"
sao as classes Action/Run (CoreRunXxx01, NRRunXxx01, GWMsgCl01, etc.) que
executam o ciclo das mensagens. As respostas abaixo valem para todo o
conjunto, verificado por grep exaustivo nos 4 modulos.

### P1 — APIs de argumentos ou parse direto do wire?

**APIs.** Zero parse textual do wire fora de `CommandLineParser`:
- `GetArgumentElement` usado 26x em ContentApp, 0x parse manual de
  `<`/`[` em Actions (grep por `find("<"`, `strtok`, etc. so acha
  `rfind('.')` para extensao de arquivo).
- O unico consumidor de bytes crus e `NGAL_Transport_RAW` (framing de
  rede, antes da Message existir) — nao ve o formato de CommandLine.
- Consequencia: **Fase A e D nao quebram nenhum Action**.

### P2 — Cardinalidade usada para lookahead?

**Nao.** Nenhum Action le o numero declarado para avancar; todos usam
`GetNumberofArgumentElements`/`GetArgumentElement` por indice. A
cardinalidade no wire e usada apenas pelos parsers (alocacao + validacao).
Fase D pode remover o `s` mantendo a contagem sem risco — ja decidido.

### P3 — Retencao de Message*/Payload* alem do Run?

**Uma retencao real encontrada: o mecanismo ScheduledMessages.**
- `CoreMsgCl01.cpp:99` (e ContentApp/IoTTestApp/NBTestApp/GIRS) cria a
  mensagem "Run" e faz `ScheduledMessages.push_back(Run)`.
- O vetor e LOCAL de `Block::Run` (Block.cpp:230) e é passado por
  referencia aos Actions seguintes da MESMA mensagem:
  `Core::DiscoveryFirstStep/SecondStep` (Core.cpp:491,545) leem
  `ScheduledMessages.at(0)`, adicionam CLs e ajustam `SetTime(futuro)`;
  `CoreSCNSeq01.cpp:113` e `CoreSCNAck01.cpp:166` fazem o
  `PushToInputQueue(Run)` que a agenda de verdade.
- **Fecho do ciclo:** a retencao morre quando `Block::Run` retorna. A
  mensagem fica no container do Process (dono) ate ser processada pelo GW
  ou marcada/deletada. Nenhum ponteiro escapa do Run.
- **Payload emprestado:** `CoreRunContentPublish01.cpp:590` faz
  `GetPayloadFromCharArray(Payload)` mas o ponteiro e usado apenas dentro
  do Run para calcular hash — nunca armazenado (o que persiste e o
  `PayloadHash` string). NRNCS `NRInfoPayload01` persiste o payload em
  DISCO (`ConvertPayloadFromCharArrayToFile`), nunca retem o ponteiro.
- **Nenhuma classe (Block/Core/NR/HTS/PS) tem membro `Message*`.** Os
  unicos retentores persistentes continuam sendo `Process::Messages[]`,
  `GW::InputQueue`, `GW::OutputQueues`.
- Consequencia: **Fase B (handles) e E (RAII) liberadas** — nao ha
  retencao fora do fluxo dono; o padrão "empresta durante o Run" ja e
  respeitado de facto.

### P4 — Estado associado a Message::Type?

**Sim, dois usos reais:**
1. `CoreInfoPayload01.cpp:130`: `if (_ReceivedMessage->GetType() == 1)`
   — estatisticas de tempo de processamento so para mensagens tipo 1
   (mensagens de aplicacao; tipo 0 = internas do core).
2. Escrito nos CLs de status/sequencia: `CoreRunPublish02/03`,
   `CoreSCNSeq01` (`Run->GetType()`), `CoreDeliveryBind01`,
   `CoreRunSubscribe01` — o valor do Type e ecoado num argumento
   (`ng -message --type [ < 1 string N > ]`).
3. Convencoes de criacao: 30x `NewMessage(..., 0, ...)` (internas) vs 35x
   `NewMessage(..., 1, ...)` (aplicacao/payload).
Consequencia: **Type e vivo e semantico (0=core, 1=app). NAO remover e
documentar a convencao.** Um futuro `enum class` teria estes call sites.

### P5 — Campos novos no wire (TTL, sequencia, prioridade)?

**Nao fazem falta — ja existem por outros meios:**
- Ordenacao/deferimento: `Message::Time` + `SetTime(futuro)` +
  priority_queue do GW (DiscoveryFirstStep usa isso para atrasar a
  discovery em `DelayBeforeDiscovery`).
- Sequencia: comando proprio `ng -message --seq` com
  `PCore->GetSequenceNumber()`.
- Tipo: comando `ng -message --type`.
- TTL/prioridade: nenhum uso identificado em nenhum modulo.
Consequencia: **Fase D sai exatamente como esta na SPEC** (so remover o
`s`), sem bump extra. Nenhuma segunda migracao sera necessaria.

### Achado adicional (descoberta da varredura)

O comentario SPEC-003 em Block.cpp:357-360 ("no action pushes to
ScheduledMessages anymore. The vector is always empty") esta **ERRADO**:
5 Actions ainda fazem `push_back` (CoreMsgCl01 x4 modulos, IRMsgCl02).
O cleanup removido por SPEC-003 estava correto em nao deletar as
mensagens agendadas (elas pertencem ao Process), mas o comentario que
justificou a remocao e falso. Nao e bug hoje (o vetor local so some no
fim do Run e as mensagens ficam com o dono), mas deve ser corrigido no
comentario/documentacao para nao confundir auditorias futuras.

## 4. Plano imediato (pos-patches urgentes, commit 6c7c1e5)

1. ~~Patches urgentes P2/P3/M4/C1/C2/null-safety~~ DONE (build limpo).
2. Este documento: review Antonio + Astra (emendas de contrato antes de B).
3. C.2: Antonio responde as 5 perguntas sobre o Script.
4. Fase A (parser unico) pode avancar em paralelo — nao toca em lifetime.
