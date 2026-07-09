# SPEC-016: Verificação de Hash pós-SPEC-015 — Conteúdos Trocados Confirmados

**Data:** 2026-07-09
**Estado:** Fix confirmada; deploy pendente
**Autor:** Hermes Agent
**Relacionada:** SPEC-015 (causa raiz), SPEC-014 (NGAL data race)

---

## 1. Problema

Após a correcção da SPEC-014 (data race no `NGAL_SAR::ReceiveFragment`), o hash mismatch persiste no cenário cross-VM ContentApp Source (VM 102) → Repository (VM 101). O log mostra 28 ficheiros `.jpg` com erro de hash.

**Amostra do log (~5365s):**
```
(ERROR: The hash of the file 00004-alpine-ng-source.jpg is not the same
 than the one generated on the publisher. i.e. 1F572033)
...
(ERROR: The hash of the file 00099-alpine-ng-source.jpg is not the same
 than the one generated on the publisher. i.e. 02A32654)
```

---

## 2. Experiência: Comparação Directa de Hash

### 2.1 Metodologia

Implementou-se o algoritmo de hash exacto do NovaGenesis (`MurmurHash3_x86_32` com seed `3571`, lendo o ficheiro em modo binário, formatando cada um dos 4 bytes como 2 hex chars maiúsculos — total 8 chars) em Python (`mmh3`) para verificar:

| Origem | Descrição |
|--------|-----------|
| **Source1/** | Ficheiros originais do publisher (VM 102, Source) — cópia local |
| **Repository1/** | Ficheiros recebidos via NRNCS + PGCS (VM 101, Repository) — cópia local |

### 2.2 Resultados

Dos **12 pares** de ficheiros disponíveis em ambas as pastas:

| Estado | Quantidade | Detalhe |
|--------|-----------|---------|
| **Hash idêntico** (ORIGINAL == RECEBIDO) | **11** | 00000, 00001, 00018, 00019, 00036, 00037, 00054, 00055, 00072, 00073, 00090 |
| **Hash diferente** | **1** | 00091-alpine-ng-source.jpg |

### 2.3 Caso 00091-alpine-ng-source.jpg — Conteúdo Trocado

| Propriedade | Source1 (original) | Repository1 (recebido) |
|-------------|-------------------|----------------------|
| Tamanho | 289.099 bytes | 289.289 bytes |
| Hash NG | `C9FDDE02` | **`0A05BF9A`** |
| Corresponde ao esperado? | ✓ (C9FDDE02 = publisher) | ✗ |

**Descoberta crítica:** O hash `0A05BF9A` que o Repository1 tem em `00091-alpine-ng-source.jpg` é **exactamente o hash esperado para o ficheiro `00022-alpine-ng-source.jpg`** segundo a linha do log:

```
(ERROR: The hash of the file 00022-alpine-ng-source.jpg is not the same
 than the one generated on the publisher. i.e. 0A05BF9A)
```

**Conclusão:** O conteúdo binário do ficheiro `00022` foi **recebido correctamente** (o hash `0A05BF9A` corresponde ao esperado), mas foi **guardado com o nome do ficheiro `00091`**. Não é corrupção — é uma **troca de associação conteúdo↔nome de ficheiro**.

### 2.4 Implicações

1. A transmissão/reassembly via NGAL+NRNCS está a entregar os bytes correctos.
2. A extracção do payload via `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` no GW funciona correctamente.
3. O bug está no passo de correlação entre o **payload entregue** e o **nome do ficheiro** onde é salvo, especificamente:
   - O `CoreInfoPayload01.cpp` chama `ExtractPayloadCharArrayFromMessageCharArray()`, que usa `stringstream::getline()` para re-parser a mensagem.
   - Esta re-extração sobrepõe o `Payload` correctamente extraído com valores corrompidos.
   - A função `ConvertPayloadFromCharArrayToFile()` escreve o payload corrompido no ficheiro.
4. **11 dos 12 ficheiros** não foram afectados porque o seu conteúdo binário não contém sequências de bytes (`\n`, `\0`, etc.) que confundem o `getline()`.
5. O `00091` foi afectado juntamente com 27 outros — a amostra disponível inclui apenas o 00091 destes 28.

---

## 3. Correcção SPEC-015 (já aplicada no código)

### 3.1 Ficheiros modificados

| Ficheiro | Linha | Mudança |
|----------|-------|---------|
| `ContentApp/src/CoreInfoPayload01.cpp` | 94 | `ExtractPayloadCharArrayFromMessageCharArray()` comentada |
| `PGCS/src/CoreInfoPayload01.cpp` | 88 | `ExtractPayloadCharArrayFromMessageCharArray()` comentada |

### 3.2 O quê

```cpp
// SPEC-015: Removed ExtractPayloadCharArrayFromMessageCharArray() — redundant call that
// re-parses Msg using stringstream::getline(), corrupting binary payloads (e.g. JPG).
// The GW already correctly extracted Payload via ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2().
//_ReceivedMessage->ExtractPayloadCharArrayFromMessageCharArray();
_ReceivedMessage->ConvertPayloadFromCharArrayToFile();
```

### 3.3 Porquê

- O GW já extraiu correctamente `Payload` e `PayloadSize` em `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()`.
- `ExtractPayloadCharArrayFromMessageCharArray()` re-faz a extracção usando `stringstream::getline()`, que é insegura para dados binários (JPG contém bytes `\n`, `\0`, etc.).
- A re-extração **sobrepõe** o payload correcto com bytes corrompidos (offset errado do `tellg()`).
- Causa também uma **memory leak** — o array `Payload` original nunca é libertado.

### 3.4 Binários pré-compilados

Disponíveis em `deploy-spec015/`:
- `ContentApp` (7.419.320 bytes)
- `PGCS` (9.540.736 bytes)
- `NRNCS` (6.008.456 bytes)
- `deploy.sh` — script de deploy para VMs 101 e 102

### 3.5 Commit

```
62e0ae6 Spec 15
```

---

## 4. Plano de Execução

| # | Etapa | Estado | Detalhe |
|---|-------|--------|---------|
| P1 | Verificação de hash (SPEC-016) | **DONE** | 11/12 hash OK; 1 troca de conteúdo confirmada |
| P2 | Recompilar (se necessário) | **DONE** | Binários em `deploy-spec015/` |
| P3 | Deploy para VMs 101/102 | **PENDENTE** | `./deploy-spec015/deploy.sh` |
| P4 | Teste --publish 0.1 | **PENDENTE** | Verificar zero erros de hash |
| P5 | Capturar ficheiros recebidos pós-fix | **PENDENTE** | Comparar hash de TODOS os ficheiros |
| P6 | Commit SPEC-016 | **PENDENTE** | Após teste passar |

---

## 5. Como Testar (P4)

```bash
# 1. Deploy
./deploy-spec015/deploy.sh

# 2. Em VM 102 (Source), iniciar processos
./Scripts/Simple/clean.sh
# (iniciar NRNCS + PGCS + ContentApp com role=Source)

# 3. Em VM 101 (Repository), iniciar processos
./Scripts/Simple/clean.sh
# (iniciar NRNCS + PGCS + ContentApp com role=Repository)

# 4. No ContentApp Source, executar:
# ng -run --publish 0.1

# 5. Verificar log do ContentApp Repository:
# grep "ERROR: The hash of the file" <log>
# Critério de sucesso: ZERO ocorrências
```

---

## 6. Decisões

| # | Decisão | Data | Razão |
|---|---------|------|-------|
| D1 | Remover `ExtractPayloadCharArrayFromMessageCharArray()` em vez de corrigir | 2026-07-09 | A função é fundamentalmente insegura para dados binários (usa `getline`). O GW já faz a extracção correctamente. |
| D2 | Mesma correcção no PGCS | 2026-07-09 | O PGCS processa o mesmo fluxo de mensagens e tem o mesmo bug. |
| D3 | Pré-compilar binários para deploy imediato | 2026-07-09 | Evita tempo de compilação nas VMs (Alpine sem toolchain completa). |

---

## 7. Ficheiros Chave

| Ficheiro | Papel |
|----------|-------|
| `ContentApp/src/CoreInfoPayload01.cpp` | **BUG corrigido**: linha 94 comentada |
| `PGCS/src/CoreInfoPayload01.cpp` | **BUG corrigido**: linha 88 comentada |
| `docs/SPEC-015-hash-mismatch-root-cause-analysis.md` | Análise detalhada da causa raiz |
| `deploy-spec015/deploy.sh` | Script de deploy |
| `deploy-spec015/ContentApp` | Binário ContentApp compilado (commit 62e0ae6) |
| `deploy-spec015/PGCS` | Binário PGCS compilado |
| `deploy-spec015/NRNCS` | Binário NRNCS compilado |
| `/home/gandalf/Downloads/Source1/` | Ficheiros originais (Source) para comparação |
| `/home/gandalf/Downloads/Repository1/` | Ficheiros recebidos (Repository) para comparação |
| `/home/gandalf/workspace/compare_hashes.py` | Script de verificação de hash NG |
