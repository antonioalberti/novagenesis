# SPEC-009 — Alpine VM SSH Run Scripts (per-process in dedicated terminal)

**Status:** Implemented (E1-E6 scripts created)  
**Date:** 2026-06-26  
**Scope:** NovaGenesis ContentApp — 1core-1repo-1source scenario on Alpine VMs  
**Stack:** Bash, SSH, gdb, PGCS/NRNCS/ContentApp (AIOPT3 branch)
**Autor:** Hermes Agent
**Revisão:** v1.1

---

## 1. Goal

Criar novos scripts em `Scripts/AlpineVMs/` que permitam ao utilizador iniciar **cada processo NovaGenesis** (PGCS ×2, NRNCS ×1, ContentApp Source, ContentApp Repository) num **terminal SSH exclusivo** a partir da VM 100 (1stagent), cada qual correndo em foreground com output visível em tempo real.

Apenas NRNCS na Source VM — a Repo VM não executa NRNCS.

Cada PGCS usa a interface `eth0` entre VMs no bridge ProxMox com MAC alvo específico (`-p` deterministic mode).

---

## 2. Scope (IN / OUT / MINIMAL TOUCH)

### IN

Novos scripts em `Scripts/AlpineVMs/`:

| Script | Processo | VM alvo | 
|--------|----------|---------|
| `run_PGCS_on_Source_VM.sh` | PGCS | 102 Source (<source-guest-ip>) |
| `run_PGCS_on_Repo_VM.sh` | PGCS | 101 Repo (<repository-guest-ip>) |
| `run_NRNCS_on_Source_VM.sh` | NRNCS | 102 Source (<source-guest-ip>) |
| `run_Source_on_Source_VM.sh` | ContentApp Source | 102 Source (<source-guest-ip>) |
| `run_Repository_on_Repo_VM.sh` | ContentApp Repository | 101 Repo (<repository-guest-ip>) |
| `README.md` §§ 7.1–7.3 | Documentação | — |

### OUT (DO NOT TOUCH)

- Todos os scripts existentes em `Scripts/Simple/run_*.sh` — permanecem intactos
- Os antigos wrappers `start-ng-*.sh` e `stop-ng.sh` foram removidos pela NG-046; o setup não os gera mais
- `README.md` §§ 7.1–7.3 — documentação operacional canónica da branch AIOPT3
- Código fonte C++ do NovaGenesis (PGCS, NRNCS, ContentApp, Common, GW) — NENHUMA alteração
- CMakeLists.txt, Make/compile-*.sh — NENHUMA alteração
- IO directories e ficheiros de configuração — NENHUMA alteração
- ProxMox config, firewall, VMs — NENHUMA alteração

### MINIMAL TOUCH

- O controlo de paragem fica a cargo dos terminais `run_*.sh` e de PIDs verificados; não há wrapper global de `killall` no fluxo AIOPT3

---

## 3. Codebase Facts (verified)

| Fact | Detail |
|------|--------|
| **PGCS deterministic mode** | `./PGCS <Path> <Port> <Role> -p Ethernet <Peer_Role> <Interface> <Peer_MAC> <MTU>` — verificado nos scripts `run_PGCS_on_Source_VM.sh` e `run_PGCS_on_Repo_VM.sh` |
| **Source VM MAC** | `<repository-peer-mac>` (eth0) — verificado `ip addr` 2026-06-26 |
| **Repo VM MAC** | `<source-peer-mac>` (eth0) — verificado `ip addr` 2026-06-26 |
| **Source VM IP** | `<source-guest-ip>` — verificado |
| **Repo VM IP** | `<repository-guest-ip>` — verificado |
| **PGCS Source → Repo** | `-p Ethernet Intra_Domain eth0 <source-peer-mac> 1200` |
| **PGCS Repo → Source** | `-p Ethernet Intra_Domain eth0 <repository-peer-mac> 1200` |
| **gdb disponível** | `/usr/bin/gdb` em ambas as VMs — verificado 2026-06-26 |
|| **Branch AIOPT3** | Commit `cd804a7` em ambas as VMs — verificado 2026-07-16 |
| **NRNCS presente na source guest** | `cmake-build-debug/NRNCS` (5.5 MB) — verificado 2026-06-26. Apenas Source VM precisa de NRNCS. |
| **SSH key** | `<operator-ssh-key>` em `~/.ssh/`, deployada em ambas as VMs — verificado |
| **PGCS precisa de sudo** | PGCS usa SHM + raw sockets — necessita de root em ambas as VMs Alpine |
| **NRNCS/ContentApp sem sudo** | Correm como root via SSH por simplicidade (já são SSH como root) |

---

## 4. Architecture

### Fluxo de execução (utilizador abre N terminais na VM 100)

```
Terminal 1         Terminal 2         Terminal 3
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ ssh -t      │    │ ssh -t      │    │ ssh -t      │
│ source guest      │    │ repository guest      │    │ source guest      │
│ PGCS -p     │    │ PGCS -p     │    │ NRNCS       │
│ Source MAC  │    │ Repo MAC    │    │ foreground  │
└──────┬──────┘    └──────┬──────┘    └──────┬──────┘
       │                  │                  │
       ▼                  ▼                  ▼
   eth0 bridge ProxMox ─────────────────────────
   (<private-test-subnet>)

Terminal 4         Terminal 5
┌─────────────┐    ┌─────────────┐
│ ssh -t      │    │ ssh -t      │
│ source guest      │    │ repository guest      │
│ ContentApp  │    │ ContentApp  │
│ Source      │    │ Repository  │
└─────────────┘    └─────────────┘
```

### Ordem de arranque recomendada

```
1. run_PGCS_on_Source_VM.sh     (esperar ~2s)
2. run_PGCS_on_Repo_VM.sh       (esperar ~2s)
3. run_NRNCS_on_Source_VM.sh    (esperar ~2s)
4. run_Repository_on_Repo_VM.sh (esperar ~2s)
5. run_Source_on_Source_VM.sh
```

### GDB wrapper pattern

Cada script usa gdb em batch mode, herdado dos scripts `run_*.sh` existentes:
```
gdb -batch -ex "run" -ex "bt" -ex "quit" --args ./<binary> <args>
```

Isto captura backtrace automático em caso de crash sem bloquear — o processo corre normalmente e o gdb só intervém no segfault.

---

## 5. Implementation

### 5.1 `run_PGCS_on_Source_VM.sh`

```bash
#!/bin/bash
# run_PGCS_on_Source_VM.sh — Start PGCS on Source VM (<source-guest-ip>)
# Opens a dedicated SSH terminal. PGCS uses deterministic mode (-p)
# targeting the Repo VM's MAC address for inter-VM raw socket discovery.
#
# Usage: bash run_PGCS_on_Source_VM.sh
# Requires: ssh key ~/.ssh/<operator-ssh-key>

SSH_KEY=~/.ssh/<operator-ssh-key>
VM_IP=<source-guest-ip>
PEER_MAC=<source-peer-mac>  # Repo VM MAC
BASE=<guest-repository-path>

echo "=== PGCS on Source VM (${VM_IP}) ==="
echo "Peer MAC: ${PEER_MAC}"
echo "Opening SSH terminal... (Ctrl+C to stop)"

ssh -t -i ${SSH_KEY} root@${VM_IP} \
  "cd ${BASE}/cmake-build-debug && \
   sudo gdb -batch -ex \"run\" -ex \"bt\" -ex \"quit\" --args \
   ./PGCS ${BASE}/IO/PGCS/ 0 Intra_Domain -p Ethernet Intra_Domain eth0 ${PEER_MAC} 1200"
```

### 5.2 `run_PGCS_on_Repo_VM.sh`

Same pattern, but targets repository guest with Source MAC as peer:

```bash
VM_IP=<repository-guest-ip>
PEER_MAC=<repository-peer-mac>  # Source VM MAC
```

### 5.3 `run_NRNCS_on_Source_VM.sh`

```bash
#!/bin/bash
# run_NRNCS_on_Source_VM.sh — Start NRNCS on Source VM (<source-guest-ip>)
# Opens a dedicated SSH terminal. Must start after PGCS.
#
# Usage: bash run_NRNCS_on_Source_VM.sh

SSH_KEY=~/.ssh/<operator-ssh-key>
VM_IP=<source-guest-ip>
BASE=<guest-repository-path>

ssh -t -i ${SSH_KEY} root@${VM_IP} \
  "cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt\" -ex \"quit\" --args \
   ./NRNCS ${BASE}/IO/NRNCS/"
```

### 5.4 `run_Source_on_Source_VM.sh`

```bash
#!/bin/bash
# run_Source_on_Source_VM.sh — Start ContentApp Source on Source VM (<source-guest-ip>)
# Opens a dedicated SSH terminal. Requires PGCS already running.
# Generates photos, then starts ContentApp.
#
# Usage: bash run_Source_on_Source_VM.sh [num_photos] [width] [height]

SSH_KEY=~/.ssh/<operator-ssh-key>
VM_IP=<source-guest-ip>
PHOTOS=${1:-100}
WIDTH=${2:-800}
HEIGHT=${3:-600}
BASE=<guest-repository-path>
IO_DIR=${BASE}/IO/Source1

ssh -t -i ${SSH_KEY} root@${VM_IP} \
  "cd ${BASE} && \
   python3 Scripts/Python/BuildPhotos.py different ${PHOTOS} ${WIDTH} ${HEIGHT} && \
   mkdir -p ${IO_DIR} && \
   mv *.jpg ${IO_DIR}/ 2>/dev/null; \
   echo 'Photos generated. Starting ContentApp...' && \
   cd ${BASE}/cmake-build-debug && \
   gdb -batch -ex \"run\" -ex \"bt full\" -ex \"info registers\" -ex \"thread apply all bt full\" -ex \"quit\" --args \
   ./ContentApp ${IO_DIR}/ Source"
```

### 5.5 `run_Repository_on_Repo_VM.sh`

Similar to Source but on repository guest, Repository role, no photo generation.

### 5.6 Documentação operacional no README raiz

Documentation covering:
- Pré-requisitos (VMs ligadas, SSH key)
- Ordem de arranque
- Uso de cada script (número de terminais)
- Como parar (Ctrl+C em cada terminal)
- Verificação de que os PGCS se descobriram (log `PGCS has N peer(s)`)

---

## 6. Etapas

### Etapa E0: Preflight — Verificar estado das VMs e branch

**Comando:**
```bash
# Verificar VMs ligadas
ssh root@<control-host> "qm status 101 && qm status 102"
# Verificar branch
ssh -i ~/.ssh/<operator-ssh-key> root@<source-guest-ip> "cd <guest-repository-path> && git log --oneline -1"
ssh -i ~/.ssh/<operator-ssh-key> root@<repository-guest-ip> "cd <guest-repository-path> && git log --oneline -1"
```

**Done when:** Both VMs report AIOPT3 branch at commit `cd804a7`.

### Etapa E1: Criar `run_PGCS_on_Source_VM.sh`

**Done when:** Script escrito em `Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh`, permissões de execução (`chmod +x`), testado com `bash -n` (syntax check).

### Etapa E2: Criar `run_PGCS_on_Repo_VM.sh`

**Done when:** Script escrito e syntax-checked.

### Etapa E3: Criar `run_NRNCS_on_Source_VM.sh`

**Done when:** Script escrito e syntax-checked.

### Etapa E4: Criar `run_Source_on_Source_VM.sh`

**Done when:** Script escrito e syntax-checked.

### Etapa E5: Criar `run_Repository_on_Repo_VM.sh`

**Done when:** Script escrito e syntax-checked.

### Etapa E6: Documentar o fluxo no README raiz

**Done when:** `README.md` §§ 7.1–7.3 contém pré-requisitos, ordem, uso dos scripts e evidência de aceitação.

### Etapa E7: Smoke test E2E (utilizador executa)

O utilizador abre 5 terminais na VM 100 e executa os scripts na ordem correcta:

1. Terminal 1 → `bash Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh`
2. Terminal 2 → `bash Scripts/AlpineVMs/run_PGCS_on_Repo_VM.sh`
3. Terminal 3 → `bash Scripts/AlpineVMs/run_NRNCS_on_Source_VM.sh`
4. Terminal 4 → `bash Scripts/AlpineVMs/run_Repository_on_Repo_VM.sh`
5. Terminal 5 → `bash Scripts/AlpineVMs/run_Source_on_Source_VM.sh`

**Done when:** ContentApp Source mostra mensagens de descoberta de NRNCS/PGCS e inicia publicação de fotos. ContentApp Repository mostra recepção.

---

## 7. Testing Plan

| Teste | Descrição | Critério |
|-------|-----------|----------|
| T1 | Syntax check | `bash -n script.sh` retorna 0 |
| T2 | SSH connectivity | `ssh -i key -o ConnectTimeout=5 root@VM_IP "echo OK"` retorna "OK" |
| T3 | Binary exists on VM | SSH check de `ls` no cmake-build-debug/ |
| T4 | E2E smoke | Utilizador abre 5 terminais, corre script por script, verifica logs |

---

## 8. Risks

| Risco | Prob. | Impacto | Mitigação |
|-------|-------|---------|-----------|
| SSH pede password | Baixa | Médio | Chave já deployada; verificar authorized_keys |
| VM não tem Python/Pillow | Baixa | Médio | Alpine pode ter Python mas sem Pillow — verificar com `python3 -c "from PIL import Image"` |
| gdb não encontra binário | Baixa | Alto | BASE path é `<guest-repository-path>` — verificado |
| Ordem de arranque errada | Média | Médio | Documentado no README com `sleep 2` entre passos |
| PGCS precisa de raw socket | Baixa | Alto | `-p` deterministic mode funciona em bridge ProxMox — confirmado nos scripts existentes |

---

## 9. Open Questions

1. **GDB wrapper ou foreground simples?** — Os scripts `run_*.sh` existentes usam gdb batch. Preferem manter gdb ou versão mais simples sem gdb? **Presunção:** manter gdb para consistência.

2. **Número de terminais** — 5 terminais é o cenário completo. O utilizador prefere scripts individuais ou um script mestre que abre os N terminais em separado (ex: usando `x-terminal-emulator` ou `tmux`)? **Presunção:** scripts individuais, o utilizador gere os terminais manualmente.

---

## 10. Decisões Aprovadas

- **Apenas NRNCS na Source VM** (2026-06-26) — A Repo VM (101) não precisa de NRNCS. Apenas o Source VM corre NRNCS. Elimina a necessidade de copiar o binário NRNCS para a repository guest.

---

## 11. Pitfalls Discovered During Design

- **SSH `-t` vs `-tt`**: `-t` força pseudo-TTY allocation. Para comandos que correm muito tempo (PGCS), `-t` é suficiente. `-tt` pode forçar duas alocações que quebram o redireccionamento de saída.
- **GDB com sudo via SSH**: `sudo gdb -batch -ex "run" ...` funciona porque o utilizador na VM Alpine (root) tem NOPASSWD para os binários PGCS. Confirmado nos scripts existentes.
- **Photos no Source VM via SSH**: O `mv *.jpg` pode falhar se Python não gerar fotos (Pillow não instalado). Incluir fallback: `mv *.jpg ... 2>/dev/null || true`.
- **Ordem de arranque é crítica**: PGCS em ambas as VMs tem de arrancar PRIMEIRO (raw socket discovery). NRNCS depois. ContentApp por último. Documentar explicitamente.
- **Ctrl+C num terminal SSH** fecha a sessão SSH e o processo remoto recebe SIGHUP → morre. Isto é o comportamento desejado para "parar o processo".

---

## 12. Acceptance Criteria

- [ ] E0-E7 executáveis e documentados
- [ ] `Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh` — executa PGCS na Source VM via SSH, foreground, gdb wrapper
- [ ] `Scripts/AlpineVMs/run_PGCS_on_Repo_VM.sh` — executa PGCS na Repo VM via SSH, foreground, gdb wrapper
- [ ] `Scripts/AlpineVMs/run_NRNCS_on_Source_VM.sh` — executa NRNCS na Source VM via SSH, foreground
- [ ] `Scripts/AlpineVMs/run_Source_on_Source_VM.sh` — gera fotos + ContentApp Source via SSH
- [ ] `Scripts/AlpineVMs/run_Repository_on_Repo_VM.sh` — ContentApp Repository via SSH
- [x] `README.md` §§ 7.1–7.3 documenta ordem, pré-requisitos e evidência
- [ ] Smoke test E2E: utilizador consegue abrir 5 terminais, arrancar tudo, ver fotos a serem publicadas