# Plano de Reversão — NG-042-04 (MarkToDelete m--cl Index==1)

## Commit a reverter
- **Hash:** `6994e84`
- **Mensagem:** `fix(gw): mark m--cl local destination message for delete (NG-042-04)`
- **Branch:** `AIOPT2`
- **Arquivo:** `Common/src/GWMsgCl01.cpp`
- **Mudança:** +10 linhas, adiciona `_ReceivedMessage->MarkToDelete()` no caso `Index == 1` do `m--cl` action

## Como reverter

### Opção 1: `git revert` (recomendado, preserva histórico)
```bash
cd /home/gandalf/workspace/novagenesis
git revert 6994e84 --no-edit
git push origin AIOPT2
```

### Opção 2: `git reset --hard` (apaga o commit, mais agressivo)
```bash
cd /home/gandalf/workspace/novagenesis
git reset --hard 0172339    # volta para o commit anterior
git push origin AIOPT2 --force-with-lease
```

### Opção 3: Patch manual (se o build/ambiente estiver quebrado)
Editar `Common/src/GWMsgCl01.cpp` linhas 260-270 e remover o bloco:
```cpp
            // FIX NG-042-04 (2026-06-03): Mark for delete.
            // ... (comentários)
            // See: https://github.com/antonioalberti/novagenesis/issues/NG-042-04
            _ReceivedMessage->MarkToDelete();
```
Depois rebuildar:
```bash
bash Make/compile-parallel.sh 4 PGCS NRNCS ContentApp
```

## Quando reverter

Reverter se QUALQUER um destes sintomas aparecer após o fix:
1. **Crash / segfault** durante processamento de mensagem
2. **Comportamento estranho** em hello--ipc, scn--seq, ou discovery
3. **Mensagens válidas somem** (log mostra "Discovered" ou SCN bindings desaparecendo)
4. **Mensagens "duplicadas"** (a mesma lógica executa duas vezes)
5. **Mensagens em memória = 0** subitamente (sinal de delete prematuro)

## Por que pode falhar

O fix assume que:
- Nenhuma action subsequente ao m--cl retém referência à `_ReceivedMessage` depois que `Block::Run()` retorna
- `MarkToDelete()` é seguro mesmo se a action for chamada novamente no mesmo loop (a flag é idempotente)

Se alguma action futura (não existente hoje) segurar uma referência async à mensagem e tentar usá-la depois do `DeleteMarkedMessages()`, haverá **use-after-free**.

## Validação esperada se o fix funcionar

- **`Messages in memory`** no log deve estabilizar em ~30-80 (não crescer linearmente)
- Sem novos crashes
- Bindings em Cat[5], Cat[6], Cat[20] continuam populando normalmente
- Discovery de peers continua funcionando

## Se Messages in memory AINDA crescer após este fix

Próximas fontes prováveis (em ordem de probabilidade):
1. `SelfReschedule` em `Common/src/GWRunHelloIPC02.cpp` e `Common/src/GWExposition02.cpp` (não marca `_ReceivedMessage` para delete)
2. `Block::Run()` end-of-loop (não marca em success path)
3. Outras actions que retêm mensagens (verificar com `grep -rn "MarkToDelete" Common/src/`)

Aplicar fix B (SelfReschedule) e fix C (Block::Run)依次 se necessário.
