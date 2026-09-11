# Plano de Reversão — NG-042-04 e NG-042-05 (Memory Leak Fixes)

Dois commits relacionados, ambos na chain de fixes para o memory leak observado como `Messages in memory` crescendo linearmente no log.

---

## NG-042-05 — MarkToDelete no Block::Run() end-of-loop

**Commit a reverter:**
- **Hash:** `64b7ef7`
- **Mensagem:** `fix(gw): mark _ReceivedMessage for delete at end of Block::Run (NG-042-05)`
- **Branch:** `AIOPT2`
- **Arquivo:** `Common/src/Block.cpp`
- **Mudança:** +23 linhas, adiciona `_ReceivedMessage->MarkToDelete()` no fim do loop de sucesso em `Block::Run()`

### Como reverter NG-042-05

**Opção 1: `git revert` (recomendado, preserva histórico)**
```bash
cd <local-repository-path>
git revert 64b7ef7 --no-edit
git push origin AIOPT2
```

**Opção 2: `git reset --hard` (apaga o commit, mais agressivo)**
```bash
cd <local-repository-path>
git reset --hard 6994e84    # volta para o commit NG-042-04
git push origin AIOPT2 --force-with-lease
```

**Opção 3: Patch manual (se o build/ambiente estiver quebrado)**
Editar `Common/src/Block.cpp` após `StopProcessingMessage=false;` no fim do loop (linhas ~310-330) e remover o bloco:
```cpp
// FIX NG-042-05 (2026-06-03): Mark for delete at the end of the
// ... (comentários completos)
// Revert plan: Docs/REVERT.md
_ReceivedMessage->MarkToDelete();
```
Depois rebuildar:
```bash
bash Make/compile-parallel.sh 4 PGCS NRNCS ContentApp
```

### Quando reverter NG-042-05

Reverter se QUALQUER um destes sintomas aparecer após o fix:
1. **Crash / segfault** durante processamento de mensagem
2. **Use-after-free**: ação tentar acessar mensagem que já foi deletada
3. **Comportamento estranho** em periodic actions (HTRunPeriodic01, GWRunHelloIPC02, GWExposition02)
4. **Mensagens válidas somem** (log mostra "Discovered" ou SCN bindings desaparecendo)
5. **Mensagens "duplicadas"** (a mesma lógica executa duas vezes)

### Por que NG-042-05 pode falhar

O fix assume que:
- Nenhuma action retém referência async à `_ReceivedMessage` depois que `Block::Run()` retorna
- Nenhuma action armazena `_ReceivedMessage` em uma estrutura de dados que sobrevive ao `Block::Run()`
- `MarkToDelete()` é idempotente (chamar 2x é no-op) — verdadeiro: o gate é `if (ApplicationDeleted == DELETED_BY_CORE) Delete = true;` e `ApplicationDeleted` só muda se `SetApplicationDeletedFlag(true)` for chamado, o que **não acontece em nenhum lugar do código**

**Risco residual:** se uma action futura (não existente hoje) armazenar `_ReceivedMessage*` em uma estrutura de dados e tentar usá-la depois do `DeleteMarkedMessages()` rodar, haverá **use-after-free**.

---

## NG-042-04 — MarkToDelete no m--cl Index==1

**Commit a reverter:**
- **Hash:** `6994e84`
- **Mensagem:** `fix(gw): mark m--cl local destination message for delete (NG-042-04)`
- **Branch:** `AIOPT2`
- **Arquivo:** `Common/src/GWMsgCl01.cpp`
- **Mudança:** +10 linhas, adiciona `_ReceivedMessage->MarkToDelete()` no caso `Index == 1` do `m--cl` action

### Como reverter NG-042-04

**Opção 1: `git revert`**
```bash
cd <local-repository-path>
git revert 6994e84 --no-edit
git push origin AIOPT2
```

**Opção 2: `git reset --hard`**
```bash
cd <local-repository-path>
git reset --hard 0172339    # volta para o commit anterior (após o log LN)
git push origin AIOPT2 --force-with-lease
```

**Opção 3: Patch manual**
Editar `Common/src/GWMsgCl01.cpp` no `else` (linhas ~258-270) e remover o bloco:
```cpp
// FIX NG-042-04 (2026-06-03): Mark for delete.
// ... (comentários completos)
// See: https://github.com/antonioalberti/novagenesis/issues/NG-042-04
_ReceivedMessage->MarkToDelete();
```

---

## Reverter AMBOS

**Reverter NG-042-05 e NG-042-04 em uma operação:**
```bash
cd <local-repository-path>
git revert 64b7ef7 6994e84 --no-edit
git push origin AIOPT2
```

**Voltar para o estado pré-leak-fixes (apenas log LN ativo):**
```bash
cd <local-repository-path>
git reset --hard 0172339
git push origin AIOPT2 --force-with-lease
```

**Voltar para o estado pré-leak-investigation (último commit estável):**
```bash
cd <local-repository-path>
git reset --hard 9b411e1    # último build antes do log LN
git push origin AIOPT2 --force-with-lease
```

---

## Validação esperada (fix A + C funcionando)

- **`Messages in memory`** no log deve **estabilizar** em ~30-80 (não crescer linearmente)
- Sem crashes
- Bindings em Cat[5], Cat[6], Cat[20] continuam populando normalmente
- Discovery de peers continua funcionando
- GWExposition02 envia forwarded hellos para todos os peers (sem auto-exposição)
- HTRunPeriodic01, GWRunHelloIPC02, GWExposition02 rodam periodicamente sem leak

## Histórico de commits relacionados

```
64b7ef7 fix(gw): mark _ReceivedMessage for delete at end of Block::Run (NG-042-05)  ← este
6994e84 fix(gw): mark m--cl local destination message for delete (NG-042-04)         ← anterior
01ef005 docs: add revert plan for NG-042-04 m--cl MarkToDelete fix
0172339 diag(gw): show target process LN in m--cl forward log
9b411e1 build: rebuild with GWExposition02 + GWMsgCl01 fixes (NG-042)
d477eaf fix(gw): two follow-ups for NG-042
46b0252 build: rebuild with deterministic Process SCN (NG-042-01 fix)
6d822a9 fix(process): make Process SCN deterministic by removing pointer inputs
8ca4804 fix(hello-ipc): skip re-discovery when peer already known in Category 20
270a540 Fixing hello IPC
```

## Investigação relacionada (informações de contexto)

Causa raiz do leak: `Message::MarkToDelete()` é gateado por `ApplicationDeleted == DELETED_BY_CORE (false)`. Por design, app pode "travar" uma mensagem via `SetApplicationDeletedFlag(true)`. **Esse setter nunca é chamado em nenhum lugar do código** (verificado via `grep -rn SetApplicationDeletedFlag`), então a gate está sempre aberta. Foot-gun para futuro, mas não é causa do leak atual.

Causa real do leak: ações que produzem novas mensagens (`HTRunPeriodic01`, `GWRunHelloIPC02::SelfReschedule`, `GWExposition02::SelfReschedule`, `m--cl` Index==1) não marcavam `_ReceivedMessage` para delete, e `Block::Run()` também não marcava no fim do success path. Mensagens órfãs acumulavam em `Process::Messages[]`.

Padrão de fix:
- NG-042-04: cobre leak do `m--cl` Index==1 (forwarded hellos do PGCS)
- NG-042-05: cobre TODOS os outros leaks (periodic actions e qualquer futura action que esqueça)
