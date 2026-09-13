# SPEC-003: Correção de Use-After-Free e Eliminação de ScheduledMessages

**Autor:** Antonio Marcos Alberti / Hermes Agent  
**Data:** 22/06/2026  
**Estado:** Implemented

---

## 1. Problema

Após a implementação das SPEC-001 e SPEC-002, o ContentApp crasha com SIGSEGV (use-after-free) em `Block::Run()`, ao tentar imprimir os CommandLines de uma mensagem já libertada.

### 1.1 Sintoma

```
Thread 1 "ContentApp" received signal SIGSEGV, Segmentation fault.
#1  Block::Run (...) at Common/src/Block.cpp:184
    _dbgCL = 0x55500036f2a7   ← ponteiro corrompido (memória libertada)
    _dbgi = 0
    _dbgNCL = 3
```

A mensagem 0x555555635620 é processada no primeiro `Block::Run` (t=406.5468718). Após `DeleteMarkedMessages()` libertá-la, o MESMO ponteiro aparece na segunda iteração (t=406.5471047). O DEBUGX acede à memória libertada → crash.

### 1.2 Causas raiz

Dois problemas contribuem para o crash:

#### A. GWStatusS01Msg é dead code (ScheduledMessages)

Em `GWMsgCl01::ForwardMessageInsideOS()`, após `ForwardMessageInsideProcess` com Index==1 (destination = GW), o código cria `GWStatusS01Msg` (mensagem com 1 CL apenas), e faz push para `ScheduledMessages`. Esta mensagem **nunca é processada** — não tem CLs suficientes para entrar no InputQueue (requer NoCL > 2), e nenhuma action a consome. A SPEC-002 cleanup marcava-a para delete e `DeleteMarkedMessages` libertava-a. É trabalho inútil que introduz complexidade no ciclo de vida das mensagens.

#### B. DEBUGX acede à mensagem ANTES de OkToRun

O bloco DEBUGX (linhas 166-194) imprime os CommandLines da mensagem no início de `Block::Run()`, **antes** da verificação `OkToRun()` (linha 206). Se uma mensagem libertada estiver no InputQueue, o DEBUGX acede à memória inválida e crasha antes que `OkToRun` pudesse rejeitá-la.

### 1.3 Mecanismo do use-after-free

A sequência exacta do crash:

1. PGCS envia hello IPC 2.0 para ContentApp via SHM
2. ContentApp lê do SHM, cria Message A (addr 0x555555635620), push para IQ
3. Gateway pop A, `Block::Run(A)`:
   - CL[0] -m --cl → GWMsgCl01 → ForwardMessageInsideProcess Index==1 → `A->MarkToDelete()`
   - GWMsgCl01 cria GWStatusS01Msg (addr B), push para ScheduledMessages
   - CL[1] -hello --ipc 2.0 → GWHelloIPC02 → "Already aware"
   - CL[2] -scn --seq → warning
   - SPEC-002 cleanup: `GWStatusS01Msg->MarkToDelete()`
4. `DeleteMarkedMessages()`: liberta A (0x555555635620) e GWStatusS01Msg (B)
5. Próxima iteração: pop do IQ obtém ponteiro 0x555555635620 (use-after-free) → SIGSEGV

---

## 2. Solução implementada

### 2.1 Eliminar GWStatusS01Msg dead code (Problema A)

**Ficheiro:** `Common/src/GWMsgCl01.cpp`

Removido o bloco que cria GWStatusS01Msg em `ForwardMessageInsideOS()` (após `ForwardMessageInsideProcess` com Index==1):

```cpp
// REMOVIDO:
if (PB->StopProcessingMessage == false)
{
    PB->PP->NewMessage(GetTime(), 0, false, GWStatusS01Msg);
    ScheduledMessages.push_back(GWStatusS01Msg);
    PMB->NewConnectionLessCommandLine("0.1", ...);
}
```

**Justificação:** A mensagem nunca é consumida. A sua criação e libertação adicionam complexidade inútil ao ciclo de vida das mensagens, e interagem mal com o `DeleteMarkedMessages`.

### 2.2 Remover SPEC-002 ScheduledMessages cleanup (Problema A, continuação)

**Ficheiro:** `Common/src/Block.cpp`

Removido o loop de cleanup de `ScheduledMessages` que tinha sido adicionado pela SPEC-002:

```cpp
// REMOVIDO:
for (Message* ScheduledMsg : ScheduledMessages)
{
    if (ScheduledMsg != 0)
    {
        ScheduledMsg->MarkToDelete();
    }
}
```

**Justificação:** Sem o push em GWMsgCl01 (2.1), nenhuma action push para `ScheduledMessages`. O vector fica sempre vazio. O loop é dead code.

**Nota:** O vector `ScheduledMessages` permanece declarado (a assinatura das actions não muda), mas fica vazio.

### 2.3 Mover DEBUGX após OkToRun (Problema B)

**Ficheiro:** `Common/src/Block.cpp`

Movido o bloco DEBUGX (impressão de CommandLines e dump da mensagem) para **dentro** do bloco `if (PP->OkToRun(_ReceivedMessage) == true)`. Se a mensagem não estiver no container `Messages[]` (foi libertada), `OkToRun` retorna false e o DEBUGX não é executado.

```cpp
// ANTES (problemático):
#ifdef DEBUG
  // DEBUGX: acede _ReceivedMessage->GetCommandLine → crash se libertada
  ...
#endif

if (PP->OkToRun(_ReceivedMessage) == true)  // ← too late
{

// DEPOIS (corrigido):
if (PP->OkToRun(_ReceivedMessage) == true)
{
  #ifdef DEBUG
    // DEBUGX: só executa se a mensagem for válida
    ...
  #endif
```

### 2.4 Fix de Process::GetBlock (descoberto durante testes)

**Ficheiro:** `Common/src/Process.cpp`

Durante os testes pós-SPEC-003, o ContentApp continuava a falhar em `DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS", "HT", ...)` porque o hello 2.0 do PGCS chegava sem o HT_BID.

**Causa:** `Process::GetBlock(string _LN, Block *&_PB)` tinha `Status=OK` DEPOIS do `break` — dead code. A função retornava sempre `ERROR`, mesmo encontrando o block. A condição `GetBlock("HT", PHTB) == OK` em `GWExposition02::ExposePeers()` Phase 2 falhava, deixando `selfHTBID` vazio.

**Fix:** Movido `Status=OK` para antes do `break`.

```cpp
// ANTES (bug):
if (PB->GetLegibleName() == _LN)
{
    _PB = PB;
    break;        // ← break primeiro
    Status = OK;  // ← dead code, nunca executado
}

// DEPOIS (corrigido):
if (PB->GetLegibleName() == _LN)
{
    _PB = PB;
    Status = OK;  // ← agora executado
    break;
}
```

**Nota:** Este bug era pré-existente (não introduzido por SPEC-001/002/003), mas só se tornou relevante porque o GWExposition02 Phase 2 foi o primeiro caller a depender do valor de retorno `OK`. O overload `GetBlock(unsigned int, Block*&)` já estava correcto.

---

## 3. Ficheiros modificados

| Ficheiro | Alteração |
|---|---|
| `Common/src/GWMsgCl01.cpp` | Removida criação de GWStatusS01Msg em ForwardMessageInsideOS (2.1) |
| `Common/src/Block.cpp` | Removido SPEC-002 cleanup loop (2.2) + movido DEBUGX após OkToRun (2.3) |
| `Common/src/Process.cpp` | Fix de GetBlock: Status=OK antes de break (2.4) |

---

## 4. Critérios de aceitação

1. ContentApp deixa de crashear com SIGSEGV após receber hello IPC 2.0 do PGCS — **OK (confirmado pelo usuário)**
2. `Messages in memory` estabiliza (não cresce indefinidamente) — **pendente verificação**
3. O DEBUGX não executa sobre mensagens libertadas — **OK**
4. O vector `ScheduledMessages` permanece vazio (nenhuma action push para ele) — **OK**
5. Hello 2.0 do PGCS inclui o HT_BID — **pendente verificação após recompilação com fix do GetBlock**
6. `DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS", "HT", ...)` retorna OK — **pendente verificação**

---

## 5. Plano de implementação

1. Remover GWStatusS01Msg dead code em GWMsgCl01.cpp ✓
2. Remover SPEC-002 cleanup em Block.cpp ✓
3. Mover DEBUGX após OkToRun em Block.cpp ✓
4. Fix de Process::GetBlock (Status=OK antes de break) ✓
5. Compilar: `sudo bash compile-parallel.sh 4 PGCS NRNCS ContentApp`
6. Testar: iniciar PGCS + NRNCS + ContentApp, verificar sem crash e Messages in memory estável

---

## 6. Impacto

| Aspecto | Impacto |
|---|---|
| Estabilidade | Elimina o SIGSEGV (use-after-free) |
| Memória | Menos mensagens criadas (sem GWStatusS01Msg dead code) |
| Complexidade | ScheduledMessages deixa de ser usado (pode ser removido completamente no futuro) |
| Retrocompatibilidade | Nenhuma action dependia de ScheduledMessages para funcionar |
| GetBlock | Correcção de bug pré-existente que afectava qualquer caller que dependesse do return value |

---

## 7. Riscos e mitigação

| Risco | Mitigação |
|---|---|
| Remover ScheduledMessages pode quebrar actions que dependam dele | Verificado: apenas GWMsgCl01 push para ScheduledMessages, e a mensagem nunca era consumida |
| Mover DEBUGX pode esconter bugs de mensagens inválidas | OkToRun já rejeita mensagens inválidas; o DEBUGX é apenas debug, não lógica de negócio |
| Fix de GetBlock pode afectar outros callers | O overload por índice já estava correcto; o overload por nome agora retorna OK correctamente, alinhando-se com o comportamento esperado |
