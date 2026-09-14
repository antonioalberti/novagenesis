# G2 — Resultado do build e smoke multi-VM

**Data:** 2026-09-13  
**Branch guest:** `AIOPT3`  
**Commit nos guests:** `1af604dad66148618cc0e578c28adb4a9829e607`  
**Tarefa:** NG-046  
**SPEC:** SPEC-009  
**Escopo:** deployment/build e smoke operacional dos cinco launchers; não é aceitação G3, G8 ou G9.

## 1. Pré-condições e preservação

- VMs 101/102 foram iniciadas e depois paradas; estado final: `stopped` em ambas.
- Preflight SSH directo, com `BatchMode`, `IdentitiesOnly`, `StrictHostKeyChecking=yes` e `known_hosts.novagenesis`, passou nos dois guests.
- Ambos reportaram branch `AIOPT3`, o mesmo commit e `git fsck --full --no-progress` sem erros.
- Interfaces e MACs observados:
  - Source 102: `eth0` → `08:00:27:79:bb:15`;
  - Repository 101: `eth0` → `08:00:27:65:00:08`.
- Ferramentas requeridas (`python3`, `sha256sum`, `gdb`, `ipcs`) estavam presentes.
- Antes do build, IO não rastreado foi copiado para:
  - `g2-evidence-20260913/guest-source/IO/` — 15.468 KB, 1.183 ficheiros;
  - `g2-evidence-20260913/guest-repository/IO/` — 2.988 KB, 127 ficheiros.
- Após o smoke longo, o IO foi novamente preservado em `guest-source-post-smoke-long/` e `guest-repository-post-smoke-long/`.
- O cleanup final usou somente PIDs/executáveis verificados e SHM identificado por `cpid`; terminou com processos NG `0`, SHM `0`, semáforos `0` e `git status` guest com `0` linhas. As seis configurações `.ini` rastreadas permaneceram presentes.

## 2. Build normal

Comando:

```bash
NG_BUILD_PROFILE=normal bash Scripts/AlpineVMs/pull-and-build-vms.sh
```

Resultado:

- `rc=0` nos dois guests.
- Build normal produziu `PGCS`, `NRNCS`, `ContentApp`, `NBTestApp` e `IoTTestApp`.
- `PSS`, `GIRS` e `HTS` foram confirmados ausentes no perfil normal.
- Os cinco binários tinham hashes idênticos nos dois guests:

| Binário | SHA-256 |
|---|---|
| PGCS | `1bb7966fa04fba9bc8f8a940376c3a902d5e8380808ee9eb788ae3f65b210dca` |
| NRNCS | `a858376ee1ae0c97249df1be394ba9849de3fb4e93c19c6edaabaa7dac13809c` |
| ContentApp | `4c461bc22274613edad07fa167612e2d2505fa8e9587883be9bfb83df321457a` |
| NBTestApp | `e110f9134540fb65b3bfb98fd85663acd0aedc73f3a1021ac0fcae67bf64b584` |
| IoTTestApp | `9ef55a318e588937b25861cc522adc4195d760a880a340906095ba4708bb56fd` |

- Todos foram identificados como ELF `static-pie`, executáveis e com debug info.
- `NRNCS` continha o marcador `cached payload`.
- O build emitiu 128 linhas de warnings por guest. Entre elas existem avisos de `mismatched-new-delete`, `control reaches end of non-void function`, directivas de pré-processador malformadas, signedness e variáveis não usadas. Estes avisos foram preservados e permanecem uma pendência de qualidade separada; não foram ocultados nem corrigidos no G2.

Log do build:

- `pull-and-build-normal.log` — SHA-256 `987ab4c7f61d59f18d9df6f7777d507dad2f92c7eb1c181cc3bb1bfe83c665d2`;
- `guest-repository-build.log` — SHA-256 `d96265382de2dc318df2cac03f0358d179885e8a5e820fc562a23c46e4f10145`.

## 3. Smoke A — janela de 180 s

O primeiro smoke iniciou todos os componentes, mas encerrou antes da recepção da foto. Foi classificado `INCONCLUSIVE` por janela insuficiente, não como regressão.

## 4. Smoke B — matched, janela longa

Orquestrador: `run-g2-smoke-long.sh`. Workload: uma foto `100x100`, mantendo commit, build e configuração.

| Evidência | Resultado |
|---|---:|
| PGCS Source client/server socket | 1/1 |
| PGCS Repository client/server socket | 1/1 |
| Peer PGCS registado em cada guest | 1/1 |
| NRNCS `OPERATIONAL: Everything ok!` | 1 |
| Repository descobriu NRNCS | 1 |
| Repository descobriu Source | 1 |
| Source descobriu Repository | 1 |
| Source publicou `00000-alpine-ng-source.jpg` | 1 |
| Repository recebeu `00000-alpine-ng-source.jpg` | 3 |

Logs do smoke B:

- `smoke-long/01-pgcs-source.log` — SHA-256 `d9877062e18987ba806d4a554fe6e86deafe44b0a3cede3df2abeb1f13577c5e`;
- `smoke-long/02-pgcs-repository.log` — SHA-256 `755d42dde8e09ac0679a7ccd519a41f18f6bce67c9855c2b9b08c7e3b49f7166`;
- `smoke-long/03-nrncs.log` — SHA-256 `c11163215f3d4a510a1a0398f507f32573df77309033b8538d177501ed813af4`;
- `smoke-long/04-repository.log` — SHA-256 `55fa2ea41da16c7cbb5a73256c3a2996ead5ee7bd4bb1a4c3b3bb7837ef7843f`;
- `smoke-long/05-source.log` — SHA-256 `8349b4d6f63f6d249ff247c728940e4ce3496cbc4c9cd6d1eab318e820d727b8`.

O Repository também registou:

```text
ERROR: The hash of the file 00000-alpine-ng-source.jpg is not the same than the one generated on the publisher.
```

Esse erro é um resultado de integridade de payload e fica aberto para G9; o G2 não pode promovê-lo a sucesso de entrega íntegra.

## 5. Veredicto separado

```text
G2 scripts/syntax             = PASS
G2 guest preflight             = PASS
G2 normal build                = PASS, com warnings classificados
G2 five-launcher smoke         = PASS operacional
G2 teardown/cleanup observado  = PASS
G2 formal release gate         = BLOCKED por G0/freeze/proveniência
G9 payload integrity           = OPEN; hash mismatch observado
```

A prova operacional do G2 está completa para o commit/build/configuração observados. O gate formal da release não pode ser fechado porque o candidato de controlo continua dirty, as alterações dos launchers ainda não estão num commit congelado e G0 permanece `BLOCKED`. Depois do freeze, o G2 deve ser revalidado contra o commit final.

## 6. Limitações não atribuídas ao G2

- O `ng_remote_executor.py preflight` falhou antes de executar por um `SyntaxError` no código inline `try/except`; o preflight directo foi usado apenas como diagnóstico e esse defeito pertence ao G3/runner.
- O smoke dos launchers deixou processos remotos vivos quando o wrapper SSH terminou; o cleanup por identidade foi executado e documentado. A correção estrutural pertence ao hardening G3.
- O smoke usou uma foto, não o workload de 100 fotos; não fecha G9, subscriptions, discovery/readback ou persistência.
- O resultado não foi commitado, pushed ou tagged.
