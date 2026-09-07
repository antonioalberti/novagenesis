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

## 3. Auditoria C.2 — Script do ciclo de vida (PENDENTE DO USUARIO)

Perguntas que bloqueiam Fase D (wire novo) e afetam Fase B:

1. O Script usa as APIs de argumentos (GetCommandLineArgumentElement etc.) ou
   interpreta o wire/texto diretamente?
2. Usa a cardinalidade declarada para lookahead ou avanco?
3. Retem Message* (ou payload/CommandLine*) alem do Run corrente?
4. Associa estado ao Message::Type?
5. Precisa de novos campos no wire (TTL, ordem)? (Astra: NAO adicionar nesta
   SPEC; necessidade distribuida comprovada vira proposta semantica separada.)

## 4. Plano imediato (pos-patches urgentes, commit 6c7c1e5)

1. ~~Patches urgentes P2/P3/M4/C1/C2/null-safety~~ DONE (build limpo).
2. Este documento: review Antonio + Astra (emendas de contrato antes de B).
3. C.2: Antonio responde as 5 perguntas sobre o Script.
4. Fase A (parser unico) pode avancar em paralelo — nao toca em lifetime.
