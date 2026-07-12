# SPEC-MUSL-001: GCC 15 Compatibility — Fix `thread` Ambiguity in Process.cpp

**Autor:** Antonio Marcos Alberti
**Data:** 25/06/2026
**Estado:** Draft

---

## 1. Problema

Ao compilar o NovaGenesis no Alpine Linux com GCC 15.2.0, o `Common/src/Process.cpp` falha com:

```
error: reference to 'thread' is ambiguous
 1031 |    thread t1(&CLI::PromptThreadWrapper,PCLI);
```

**Causa:** A linha `thread t1(...)` é ambígua porque:

1. `tthread::thread` — da biblioteca `tinythread.h` (incluída via `Message.h` → `Process.h`)
2. `std::thread` — incluído transitivamente via `GW.h:121` → `<condition_variable>` → `<stop_token>` → `<thread>` (comportamento do GCC 15+)

GCCs anteriores (≤14) não incluíam `<thread>` transitivamente a partir de `<condition_variable>`, por isso o código compilava sem problemas.

## 2. Escopo

**Ficheiro a modificar:** `Common/src/Process.cpp` (linha 1031)

**Alteração:** Mudar `thread t1(...)` para `tthread::thread t1(...)` — usando o qualificador de namespace explícito.

**Código afectado:**

```cpp
// Process.cpp:1028-1034
CLI *PCLI=(CLI*)PCLIB;

// Create a CLI prompt thread
thread t1(&CLI::PromptThreadWrapper,PCLI);      // ← AMBIGUOUS

// Wait for the threads to finish
t1.join();
```

**Após a correcção:**

```cpp
// Create a CLI prompt thread
tthread::thread t1(&CLI::PromptThreadWrapper,PCLI);  // ← FIXED
```

## 3. Análise de Segurança

- `tthread::thread` é o mesmo tipo que `thread` resolvia antes da ambiguidade — a variável `t1` já era do tipo `tthread::thread` (herdado de `tinythread.h`)
- `CLI::PromptThreadWrapper` aceita `void*` — o `PCLI` (CLI*) é convertido correctamente
- `t1.join()` não precisa de qualificador — `t1` já é uma variável local tipada
- Nenhum outro ficheiro usa `thread` sem qualificador (verificado via grep)
- A função `RunPrompt()` só é compilada com `#ifdef DEBUG` — não afecta builds de produção

## 4. Platformas Afectadas

| Plataforma | Impacto |
|------------|---------|
| Alpine Linux (GCC 15) | ❌ Falha — precisa da correcção |
| Ubuntu 24.04 (GCC 13) | ✅ Compila com ou sem a correcção |
| Docker Ubuntu (GCC 13) | ✅ Compila com ou sem a correcção |

## 5. Etapas de Implementação

| Etapa | Descrição | Local |
|-------|-----------|-------|
| **E0** | Preflight — confirmar linha exacta e contexto | Process.cpp:1031 |
| **E1** | Aplicar patch: `thread` → `tthread::thread` | Process.cpp:1031 |
| **E2** | Recompilar PGCS + NRNCS + ContentApp na VM Source | `make -j2` na Source VM |
| **E3** | Recompilar PGCS + ContentApp na VM Repo | `make -j2` na Repo VM |
| **E4** | Executar cenário de teste | start-ng-source.sh + start-ng-repo.sh |
| **E5** | Verificar logs | PGCS.log, NRNCS.log, ContentApp.log |
| **E6** | Repetir no Docker se necessário | docker build + docker run |

## 6. Critérios de Aceitação

1. Compilação 100% bem-sucedida em ambas as VMs Alpine (PGCS, NRNCS, ContentApp)
2. PGCS executa sem erros de símbolo (`pthread_cond_clockwait`, `mcount`, etc.)
3. Serviços NG rodam em ambas as VMs
4. Logs mostram descoberta de peers via raw socket

## 7. Riscos

| Risco | Probabilidade | Mitigação |
|-------|---------------|-----------|
| Outros ficheiros com `thread` não qualificado | Baixa | `grep -rn '\bthread\b' Common/src/` — apenas esta ocorrência |
| GCC 15 outras incompatibilidades | Média | Pode haver mais erros após o primeiro — iterar |
| `pthread_cond_clockwait` residuals | Média | O erro original veio do binário antigo compilado com glibc — recompilação com musl resolve |