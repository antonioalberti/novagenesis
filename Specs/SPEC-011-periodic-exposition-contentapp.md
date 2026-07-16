# SPEC-011: Exposição Periódica do ContentApp ao NRNCS

**Autor:** Hermes Agent (Scalifax)
**Data:** 06/07/2026
**Revisão:** 1
**Estado:** Implementado ✓

---

## 1. Problema

O `ContentApp` publica as suas bindings de aplicação (papel Source/Repository, subject Content) ao NRNCS **apenas uma vez**, através do mecanismo `RunExpose` em `CoreRunPeriodic01.cpp` (linhas 244-267):

```cpp
if (PCore->PSTuples.size() > 0)
{
    if (PCore->RunExpose == true)          // ← Guard de execução única
    {
        PCore->Exposition(Intra_Domain, ScheduledMessages);
        PCore->Exposition(Intra_OS, ScheduledMessages);
        PCore->RunExpose = false;          // ← Nunca mais executa
        PB->State = "operational";
    }
}
```

Após `RunExpose = false`, a `Exposition()` nunca mais é invocada, mesmo que:

1. **PSTuples esteja vazio na primeira execução** — o NRNCS pode ainda não ter sido descoberto quando o primeiro `-run --periodic` corre, e a exposição é saltada para sempre.

2. **A mensagem `-p --b` perca-se no SHM** — se por qualquer razão (fila cheia, conflito de semáforo, timeout) o ContentApp não conseguir enviar a exposição ao NRNCS na única janela disponível, nunca mais tenta.

3. **PSTuples[0] tenha dados incompletos** — `CoreRunExpose01.cpp` linha 102 usa `PSTuples[0]->Values[0..3]` como destino. Se algum valor estiver vazio, a mensagem vai para o destino errado e a operação falha irremediavelmente.

**Sintoma no log:** o ContentApp descobre o NRNCS (`Aware of NRNCS on Categories 2 and 9`), mas nunca publica `Hash("Source")`/`Hash("Repository")`/`Hash("Content")` ao NRNCS. O NRNCS nunca recebe as bindings de aplicação para propagar ao outro ContentApp. Ambos os ContentApps ficam sem encontrar o peer.

---

## 2. Solução Proposta

**Remover o guard `RunExpose` e executar `Exposition()` em cada ciclo de `-run --periodic`.** Isto garante tentativas múltiplas: se o PSTuples ainda está vazio num ciclo, tenta no próximo. Se a mensagem se perdeu, reenvia. A resiliência passa a ser **intrínseca** em vez de depender de uma única janela de oportunidade.

### 2.1 Alterações

#### Ficheiro: `ContentApp/src/CoreRunPeriodic01.cpp`

**O quê:** Substituir o bloco das linhas 244-267 de execução única para execução periódica, mantendo apenas o guard `PSTuples.size() > 0` (precisamos do destino NRNCS).

```cpp
// ANTES (linhas 244-267):
if (PCore->PSTuples.size () > 0)
{
    if (PCore->RunExpose == true)
    {
        PCore->Exposition (PB->PP->Intra_Domain, ScheduledMessages);
        PCore->Exposition (PB->PP->Intra_OS, ScheduledMessages);
        PCore->RunExpose = false;
        PB->State = "operational";
    }
}

// DEPOIS:
if (PCore->PSTuples.size () > 0)
{
    // SPEC-011: Exposição periódica — corre em cada ciclo de -run --periodic
    // para garantir resiliência a PSTuples vazio, mensagem perdida, ou
    // destino incompleto no primeiro ciclo.
    PCore->Exposition (PB->PP->Intra_Domain, ScheduledMessages);
    PCore->Exposition (PB->PP->Intra_OS, ScheduledMessages);

    // State transition: apenas na primeira vez que PSTuples está disponível
    if (PB->State != "operational")
    {
        PB->State = "operational";
    }
}
```

#### Ficheiro: `ContentApp/src/Core.h`

**O quê:** Remover o campo `bool RunExpose;` (linha 101) — fica obsoleto.

#### Ficheiro: `ContentApp/src/Core.cpp`

**O quê:** Remover a inicialização `RunExpose = true;` (linha 139).

### 2.2 Impacto na `CoreRunExpose01.cpp` (sem alterações)

A função `CoreRunExpose01::Run()` já usa `PCore->PSTuples[0]` para determinar o destino. Como a `Exposition()` só é chamada quando `PSTuples.size() > 0`, o `PSTuples[0]` existe sempre que `-run --expose` é agendado.

### 2.3 Segurança: Re-publicação de bindings

A `Exposition()` adiciona um command line `-run --expose 0.1` ao `ScheduledMessages[0]`, que quando executado gera um `-p --b` para o NRNCS. O `-p --b` armazena bindings no NRNCS via `StoreHTBindingValues`. Como as bindings têm chaves determinísticas (hashes de nomes), re-publicar as mesmas bindings é **idempotente** — o NRNCS simplesmente substitui pelo mesmo valor.

---

## 3. Ficheiros Alterados

| Ficheiro | Alteração |
|----------|-----------|
| `ContentApp/src/CoreRunPeriodic01.cpp` | Remover guard `RunExpose`, executar `Exposition()` sempre que PSTuples > 0 |
| `ContentApp/src/Core.h` | Remover `bool RunExpose;` |
| `ContentApp/src/Core.cpp` | Remover `RunExpose = true;` na inicialização |

---

## 4. Critérios de Aceitação

1. **Resiliência a PSTuples vazio:** Se o NRNCS ainda não foi descoberto no primeiro ciclo, o ContentApp publica bindings no ciclo seguinte (assim que PSTuples > 0).

2. **Resiliência a perda de mensagem:** Se o `-p --b` se perder, é reenviado no próximo `-run --periodic` (tipicamente 30-60s depois).

3. **Transição de estado correcta:** `PB->State = "operational"` ocorre apenas uma vez, mesmo que a exposição corra múltiplas vezes.

4. **Idempotência:** Re-publicar bindings não causa duplicação no HT do NRNCS nem efeitos laterais indesejados.

5. **Compilação limpa:** Sem erros ou warnings novos.

---

## 5. Plano de Reversão

Se o comportamento periódico causar problemas (ex.: flood de `-p --b` no NRNCS):

```bash
git revert HEAD --no-edit
git push origin AIOPT3
```

Sintomas a vigiar nas primeiras 24h após aplicação:
- Multiplicação de bindings duplicadas no NRNCS (verificar `-st --s` logs)
- Aumento anormal de tráfego SHM entre ContentApp e PGCS
- Logs do NRNCS a mostrar `-p --b` recebidos a cada ciclo

---

## 6. Notas de Implementação

- **Não alterar** `CoreRunExpose01.cpp` — a lógica de destino via `PSTuples[0]` mantém-se.
- **Não alterar** `Core::Exposition()` — apenas o local de chamada (frequência) muda.
- O `PB->State = "operational"` continua a ser uma transição única via guard `if (PB->State != "operational")`.
