# SPEC-002: Correção do Leak de Mensagens — ScheduledMessages não processadas

**Autor:** Antonio Marcos Alberti  
**Data:** 20/06/2026  
**Estado:** Reverted — superceded por SPEC-003

---

## 1. Problema

O `Messages in memory` cresce continuamente durante a operação. Num teste de ~54 segundos, passou de 83 para 100 mensagens, e continuaria a crescer até ao limite `MAX_MESSAGES_IN_MEMORY`.

No `Block::Run()`, existe um `vector<Message*> ScheduledMessages` local que as acções podem preencher, mas **nunca é processado nem limpo** após o loop de command lines. As mensagens nele inseridas ficam para sempre em `Process::Messages[]` sem nunca serem marcadas para delete.

### 1.1 Causa raiz

Em `GWMsgCl01::ForwardMessageInsideOS` (linhas 329-343):

```cpp
if (PB->StopProcessingMessage == false)
{
    // Cria GWStatusS01Msg — NUNCA é libertada!
    PB->PP->NewMessage(GetTime(), 0, false, GWStatusS01Msg);
    ScheduledMessages.push_back(GWStatusS01Msg);

    PMB->NewConnectionLessCommandLine("0.1",
                                      &ReceivedMessageLimiters,
                                      &ReceivedMessageDestinations,
                                      &ReceivedMessageSources,
                                      GWStatusS01Msg, GWMsgCl01);
}
```

Isto é activado para **cada mensagem hello IPC** com destinos específicos (não `FFFFFFFF`), que são todas as mensagens hello recebidas via PGCS exposition.

## 2. Solução proposta (original)

Adicionar limpeza de `ScheduledMessages` no final de `Block::Run()`, após o loop de processamento de command lines. Qualquer mensagem deixada no vector sem ser consumida por uma acção seria marcada para delete.

Adicionalmente, marcar `_ReceivedMessage->MarkToDelete()` no fim do success path de `Block::Run()`.

## 3. O que foi implementado

1. Loop de cleanup de `ScheduledMessages` em `Block::Run()` (marcar para delete)
2. `_ReceivedMessage->MarkToDelete()` no fim do success path de `Block::Run()`

## 4. Reversão

Ambas as alterações foram **revertidas** porque:

1. O `_ReceivedMessage->MarkToDelete()` no fim de `Block::Run()` causava **double-free** quando combinado com as marcações já existentes em acções como `GWMsgCl01::ForwardMessageInsideProcess` (Index==1 path). Revertido na sessão de 21/06.

2. O loop de cleanup de `ScheduledMessages` foi removido pela **SPEC-003** porque:
   - A `GWStatusS01Msg` (única mensagem push para `ScheduledMessages`) era **dead code** — criada com 1 CL apenas, nunca processada, nunca consumida por nenhuma action.
   - A solução correcta não é limpar `ScheduledMessages` no fim de `Block::Run()`, mas sim **eliminar a criação da mensagem dead code** na origem (`GWMsgCl01::ForwardMessageInsideOS`).
   - O loop de cleanup interagia mal com `DeleteMarkedMessages()`, contribuindo para o use-after-free que causou o SIGSEGV no ContentApp.

## 5. Estado actual

- `Block::Run()`: **sem** loop de cleanup de ScheduledMessages e **sem** `_ReceivedMessage->MarkToDelete()` no success path.
- `GWMsgCl01::ForwardMessageInsideOS`: **sem** criação de GWStatusS01Msg (removido por SPEC-003).
- `ScheduledMessages` vector: permanece declarado mas é sempre vazio.

Ver SPEC-003 para a solução definitiva.
