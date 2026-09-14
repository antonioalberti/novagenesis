# SPEC-052: Executor SSH multi-VM e observação remota

**Author:** Antonio Alberti / Astra  
**Date:** 2026-09-12  
**Status:** Proposal  
**Branch:** AIOPT3  
**Implementation commit:**  
**Related:** SPEC-046-multi-vm-runner-safe-teardown.md, SPEC-045-core-evaluate-functional-harness.md, SPEC-051-cross-vm-nrncs-discovery-diagnosis.md, SPEC-055-ng-elc-local-mode.md

## 1. Problema e base verificada

1. `supervise_process.py` cria e supervisiona um grupo **local**. Não regista identidade, readiness ou término de processos guest. Terminar esse supervisor ou o seu `ssh` não demonstra término remoto.
2. Os cinco launchers `run_*.sh` usam sessões SSH interactivas com GDB, verificam branch/commit/receipt antes do arranque e não executam cleanup destrutivo. A propagação do status do processo depende de `-return-child-result`; o teardown supervisionado continua fora do escopo dos launchers.
3. A SPEC-046 documenta remoção de IPC com consumidores/wrappers ainda activos e interferência `shmat/shmdt: Invalid argument`.
4. Os assessments fornecidos demonstram observação remota por GDB, mas distinguem processamento receptor, invocação de armazenamento e readback. Esses gates não são equivalentes.
5. A comparação matched reporta 100/100 byte-exact nos dois commits testados. Não resolve a falha discovery-only da SPEC-050/051.
6. A árvore de trabalho fornecida contém alterações de produção e ficheiros não rastreados. Esta SPEC não autoriza revertê-los, incorporá-los ou atribuir-lhes proveniência de um commit limpo.

As secções seguintes são **requisitos propostos**, não descrição de funcionalidades já implementadas.

## 2. Objectivo, âmbito e exclusões

O controlador canónico é o **NG Experiment Lifecycle Controller (NG-ELC)**, com entry point `Scripts/AlpineVMs/ng_remote_executor.py`. Esta SPEC define o modo `remote`; o modo intra-OS do mesmo controlador é definido em `SPEC-055-ng-elc-local-mode.md`.

### 2.1 Incluído

Executor bounded para VM101 Repository e VM102 Source, com:

- reinício completo antes de cada novo trial;
- proveniência verificável e exclusão de execuções concorrentes;
- lançamento PGCS Source → PGCS Repository → NRNCS Source → Repository → Source;
- perfis diagnósticos que terminem antes dos papéis posteriores;
- supervisão remota de aplicações, GDB, capturas e respectivos descendentes;
- gates observáveis, workload opcional, hashes e evidência durável;
- término remoto, inventário/limpeza selectiva de IPC e paragem final das VMs;
- resultados separados para runtime, teardown e integridade da evidência.

### 2.2 Excluído

- Alterações C++ de produção, protocolo, wire format ou modelo inverted pub/sub.
- Resolver ou contornar silenciosamente a SPEC-051.
- Builds/deploys automáticos durante um trial.
- Serviços pagos, agentes permanentes, daemon de execução genérico ou credenciais no repositório.
- `killall`, `pkill` global, `ipcrm -a`, limpeza por UID ou remoção recursiva de caminhos não comprovadamente pertencentes ao trial.
- Claims funcionais por CPU, existência de PID, frames Ethernet ou contagem de mensagens.
- Contenção de processos maliciosos, processos que escapem deliberadamente da sessão/grupo ou recuperação garantida após falha do kernel/host.

### 2.3 Compatibilidade

Os `run_*.sh` e o supervisor local permanecem com interfaces e comportamento existentes. O novo executor **não os chama**, pois os seus cleanups e SSH foreground não satisfazem este contrato.

O README distingue modo manual legado e modo supervisionado. Não se podem misturar na mesma VM durante um trial. A compatibilidade manual não constitui certificação do seu teardown.

## 3. Arquitectura escolhida

### 3.1 Componentes

| Componente | Responsabilidade |
|---|---|
| Controlador local Python 3, stdlib | Plano, estados, deadlines, SSH/SCP, gates, manifest e resultado |
| Helper remoto Python 3, stdlib | Lock guest, identidade, wrappers, lease, status e término |
| OpenSSH CLI | Comandos curtos, autenticação e controlo remoto |
| SCP | Helper, contrato e artefactos, com verificação SHA-256 |
| Ferramentas existentes | GDB, tcpdump, `/proc`, `ipcs`/`ipcrm`, Git, `sha256sum`, ferramentas de VM |
| Adaptador Proxmox | `qm status/stop/start` no host configurado, restrito a VM101/102 |

Python 3 no Repository guest é um pré-requisito a verificar; a documentação fornecida só o exige explicitamente para a geração Source. Justifica-se para JSON, `/proc`, sinais e subprocessos sem acrescentar bibliotecas externas.

### 3.2 Decisão de transporte e supervisão

Usar **helper copiado por SCP e iniciado por SSH**, não uma sessão SSH longa por papel.

1. Criar exclusivamente um directório remoto do trial em armazenamento persistente.
2. Copiar helper e contrato; verificar hashes antes de os executar.
3. Iniciar monitor remoto desacoplado da sessão SSH: nova sessão, stdin fechado e stdout/stderr em ficheiros.
4. Confirmar handshake persistido com `trial_id`, boot ID, PID/starttime do monitor e hash do helper.
5. Usar ligações SSH curtas para `status`, renovação de lease, `stop` e inventários.
6. Cada papel tem wrapper/grupo próprios; GDB e o inferior ficam nesse grupo. Capturas têm grupos separados.
7. Um wrapper-âncora conserva a identidade do grupo enquanto existem descendentes. Não terminar apenas porque GDB ou o filho inicial terminou.
8. O monitor permanece fora dos grupos que termina e regista/recolhe os seus estados.

Matar o `ssh` local nunca é operação de cleanup. Perda do controlador expira a lease remota e inicia término dos grupos, **sem apagar IPC nem parar a VM**.

O suporte inicial exige aplicações foreground que não façam `setsid`/daemonização. Escapes detectados produzem `UNSUPPORTED_DESCENDANT`, bloqueiam limpeza e exigem recuperação verificada; não autorizar sinais por simples nome do executável.

### 3.3 Lease e idempotência

- Renovação normal: cada 5 s; expiração: 30 s, medida pelo relógio monotónico remoto.
- Cada renovação contém sequência crescente; duplicados/replays não prolongam a lease.
- Deadline absoluto monotónico do trial no helper limita renovações indefinidas.
- `launch(role)` duplicado devolve a identidade existente; nunca lança uma segunda instância.
- `stop` repetido é seguro; reconexão nunca reinicia automaticamente um papel.
- Journals e estados são escritos atomicamente; transições críticas recebem `fsync`.
- Locks locais por par de VMs e remotos por guest. Lock antigo não é apagado apenas pela idade.

## 4. Configuração e contrato do trial

### 4.1 Configuração local obrigatória

Extender `ng-vm.env.example` apenas com placeholders neutros. O ficheiro real é ignorado pelo Git e tem permissões restritas.

| Variáveis | Conteúdo |
|---|---|
| `NG_SSH_KEY`, `NG_SSH_USER` | Chave local existente e utilizador guest, sem defaults implícitos |
| `SOURCE_VM_IP`, `REPO_VM_IP` | Destinos SSH guest distintos |
| `SOURCE_VM_MAC`, `REPO_VM_MAC` | MACs locais reais, verificadas no guest |
| `SOURCE_VM_IFACE`, `REPO_VM_IFACE` | Interfaces Ethernet |
| `NG_REPO_PATH` | Caminho absoluto do repositório nos guests |
| `NG_EVIDENCE_PATH` | Raiz local persistente de evidência |
| `NG_REMOTE_EVIDENCE_PATH` | Raiz persistente remota, fora de `/tmp` |
| `NG_SSH_KNOWN_HOSTS` | Ficheiro local com chaves guest/host previamente verificadas |
| `NG_VM_HOST`, `NG_VM_USER`, `NG_VM_SSH_KEY` | Acesso ao host que executa `qm` |
| `NG_REPO_VM_ID`, `NG_SOURCE_VM_ID` | Respectivamente 101 e 102 |

Se houver captura no bridge do host, exigir também a interface de captura. Captura guest é permitida quando suficiente para o oráculo declarado.

Valores ausentes, placeholders, destinos ambíguos, caminhos relativos, MAC inválida ou VM ID fora da allowlist causam falha antes de mutações. Nunca interpretar os valores como fragmentos shell.

O controlador recebe o ambiente exportado; **não faz `source` de ficheiros**. O carregamento do env pelo operador é operação local de confiança.

### 4.2 Plano JSON versionado

`schema_version: 1`, com:

- perfil e objectivo/oráculos do trial;
- commit completo esperado e política de árvore dirty;
- caminhos relativos dos executáveis, SHA-256 esperados e recibo de build;
- papéis, `argv` como arrays, `cwd`, configurações e ambiente explicitamente permitido;
- directórios exclusivos de IO/cache/output e regras de inventário;
- delays efectivos, probes, deadlines e dependências dos gates;
- modo `direct` ou `gdb`, ficheiros de comandos GDB e respectivos hashes;
- capturas, quotas, limites de disco e workload;
- política IPC e critérios de conclusão.

Não aceitar comandos shell arbitrários no plano. Comandos GDB são artefactos executáveis de confiança, sujeitos a revisão e hash.

Plano efectivo, configurações e argumentos ficam congelados antes do primeiro lançamento. Alterações posteriores exigem novo trial.

### 4.3 Proveniência

Registar separadamente controlador e guests:

- branch, HEAD completo, estado dirty, submódulos aplicáveis;
- hashes dos ficheiros modificados/não rastreados relevantes;
- caminhos reais, tamanho, formato e SHA-256 dos binários;
- recibo que ligue build, fonte/configuração e binários;
- versões do helper, SSH, Python, GDB e ferramentas usadas.

Git HEAD não prova que um binário foi compilado desse commit.

Por omissão, fontes guest dirty, hash inesperado ou ausência de proveniência de build bloqueiam lançamento. Um perfil **diagnóstico** pode declarar previamente proveniência parcial/dirty, preservá-la e produzir resultado limitado; nunca PASS de release. A árvore local dirty conhecida não pode ser limpa automaticamente.

## 5. Interface, ficheiros e limites

### 5.1 CLI proposta

Entrada: `python3 Scripts/AlpineVMs/ng_remote_executor.py`

| Subcomando | Interface |
|---|---|
| `dry-run` | `--mode <remote|local> --plan <json> --scenario <L0..L5|local-intra-os> --debug-profile <id>` |
| `preflight` | `--mode <remote|local> --plan <json> --scenario <L0..L5|local-intra-os> --debug-profile <id>` |
| `run` | `--mode <remote|local> --plan <json> --scenario <L0..L5|local-intra-os> --debug-profile <id>` |
| `status` | `--trial <id>` |
| `collect` | `--trial <id>` |
| `cleanup` | `--trial <id>` |
| `finalize` | `--trial <id>` |

- `dry-run`: valida localmente, apresenta plano redigido; sem SSH/SCP ou mutações.
- `preflight`: consultas remotas read-only; não reinicia nem limpa.
- `run`: gera ID UTC+UUID exclusivo e executa todo o lifecycle.
- `cleanup`: termina apenas recursos verificados do trial, inventaria e faz limpeza IPC autorizada; não lança workload.
- `collect`: recupera evidência sem iniciar processos de aplicação.
- `finalize`: repete verificações/evidência e só então tenta a paragem final das VMs.
- Não existe `--force` para ignorar identidade, readiness ou cleanup.

Interface interna do helper: `prepare`, `start-monitor`, `launch`, `renew`, `status`, `stop`, `inventory`, `cleanup-ipc`, `seal`. Pedidos/respostas JSON têm versão, ID de operação e trial. Endpoints não são interfaces de execução shell genérica.

### 5.2 Layout proposto

- `Scripts/AlpineVMs/ng_remote_executor.py`
- `Scripts/AlpineVMs/remote/ng_trial_helper.py`
- `Scripts/AlpineVMs/plans/*.example.json`
- `Scripts/AlpineVMs/schemas/trial-v1.schema.json`
- `Scripts/AlpineVMs/schemas/manifest-v1.schema.json`
- `Scripts/AlpineVMs/tests/` — unitários, fake SSH/SCP/helper e fixtures
- `Scripts/AlpineVMs/ng-vm.env.example` — extensão compatível
- `Specs/SPEC-052-remote-ssh-multi-vm-executor.md`
- `Specs/RESULTS-SPEC-052/` — assessments e referências redigidas

Validar esquemas por stdlib; não exigir pacote JSON Schema. Acrescentar regras `.gitignore` específicas para configuração/estado local, sem ocultar ficheiros já modificados nem ignorar genericamente scripts.

### 5.3 SSH/SCP

Usar:

- `BatchMode=yes`, `IdentitiesOnly=yes`;
- `StrictHostKeyChecking=yes`, `UserKnownHostsFile` configurado;
- `ConnectTimeout=10`, `ConnectionAttempts=1`;
- `ServerAliveInterval=5`, `ServerAliveCountMax=3`;
- `ForwardAgent=no`, `ClearAllForwardings=yes`, sem X11;
- SSH sem PTY (`-T`), sem multiplexação implícita.

Usar política equivalente no SCP. Verificar suporte real SCP/SFTP nos guests em preflight; não degradar silenciosamente autenticação ou host-key checking.

O protocolo de controlo via stdin evita interpolação de JSON no comando remoto. Os poucos caminhos usados na invocação SSH exigem quoting testado; rejeitar caracteres de controlo e destinos começados por opções.

### 5.4 Timeouts e sinais

Defaults, todos finitos e registados:

| Operação | Limite |
|---|---:|
| Comando de controlo SSH | 30 s |
| Operação `qm` | 120 s |
| Boot e SSH guest pronto | 180 s |
| Handshake de lançamento | 15 s |
| Gate de readiness | 240 s |
| Observação diagnóstica após gate | 30 s |
| Workload 100 fotos | 600 s |
| SIGTERM grace | 10 s |
| Verificação após SIGKILL | 10 s |
| Recolha por guest | 180 s |
| Trial até início obrigatório do teardown | 1800 s |

Timeouts de gate têm de exceder os delays efectivos e a janela de observação exigida. Se o orçamento global não comportar o plano, rejeitar antes do lançamento. Overrides só no plano validado; nenhum valor infinito.

SIGINT/SIGTERM local: parar admissões, persistir `ABORTED`, solicitar término remoto e recolher evidência. Segundo sinal pode abreviar esperas locais, mas não autoriza IPC deletion ou VM stop. SIGKILL/falha do controlador é coberto apenas pela lease remota e recuperação posterior.

## 6. Lifecycle fail-closed

| Estado | Gate/efeito obrigatório |
|---|---|
| `VALIDATED` | Configuração, plano, orçamento e armazenamento válidos |
| `LOCKED` | Locks exclusivos locais e remotos; nenhum trial activo concorrente |
| `PRE_RESTART_AUDIT` | Inventário inicial e recuperação de evidência anterior |
| `RESTARTED` | `qm stop` + `qm start` de ambas; novos boot IDs confirmados |
| `PREFLIGHT_OK` | Alvos, MACs, ferramentas, privilégios, disco, processos e IPC verificados |
| `PROVENANCE_OK` | Commit/binários/configuração congelados e política satisfeita |
| `PREPARED` | Helper verificado, IO exclusivo, inventários baseline e lease activos |
| `LAUNCHING` | Papéis lançados uma vez, identidade confirmada |
| `READY` | Gates exigidos pelo perfil satisfeitos |
| `OBSERVING` / `WORKLOAD` | Probes ou fotos; progresso e deadlines supervisionados |
| `RUNTIME_SEALED` | Resultado runtime e snapshots iniciais fixados |
| `TERMINATING` | Término remoto em ordem inversa, capturas por último |
| `PROCESSES_ZERO` | Ausência confirmada de aplicações, GDB, wrappers e capturas do trial |
| `IPC_VERIFIED` | Inventários, limpeza selectiva permitida e inventário final |
| `EVIDENCE_VERIFIED` | Artefactos recolhidos, hashes confirmados, helper encerrado e auditado |
| `VM_STOPPED` | `qm stop` e confirmação de ambas as VMs paradas |
| `FINALIZED` | Manifest final local e códigos de resultado |

Regras:

1. `PRE_RESTART_AUDIT` não permite usar reboot como mecanismo de limpeza cega. Recursos antigos conhecidos exigem recuperação/cleanup/evidência primeiro; recursos desconhecidos bloqueiam e deixam VMs ligadas.
2. Um novo teste real exige novo stop/start completo. Não reutilizar trial, boot, staging ou cache.
3. Antes do lançamento: zero processos NG/GDB antigos. Após cada lançamento: exactamente um processo de aplicação por papel previsto; wrappers/GDB são contados separadamente.
4. Falha de gate impede papéis dependentes/workload e transita para preservação e teardown.
5. Falha de identidade, conectividade ou inventário significa estado **desconhecido**, não ausência.
6. Se evidência ou cleanup falhar, não parar a VM. Registar `QUARANTINED`, mantendo instruções de recuperação.
7. O monitor termina depois de selar o journal. Uma consulta SSH final read-only confirma ausência do monitor e demais processos do trial antes da paragem.

## 7. Identidade remota e prevenção de resíduos

Cada processo controlado recebe registo com:

- `trial_id`, role, VM ID, destino SSH e fingerprint da host key;
- boot ID Linux e identidade guest;
- PID, PPID, PGID, SID e `/proc/<pid>/stat` starttime;
- tipo: monitor, wrapper, GDB, aplicação ou captura;
- caminho resolvido de `/proc/<pid>/exe`, dispositivo/inode e hash;
- `argv` exacto, `cwd`, commit declarado, hash binário e hash de configuração;
- relação parent/child observada e sequência de lançamento.

Distinguir PID Linux de PID/BID/HID/OSID do protocolo NG.

Antes de cada sinal, validar boot, PID/starttime, grupo/sessão e pertença ao trial. Nunca adoptar um processo porque o nome, PID ou comando coincide. Um PID reutilizado gera recusa de sinal e falha de identidade.

O wrapper-âncora mantém o grupo reconhecível até ao término; o monitor serializa launch/stop/reap. A implementação deve testar a janela entre validação e sinalização e não reutilizar um PGID registado depois de perder a sua âncora. Handles estáveis de PID, quando disponíveis, protegem operações individuais; não tornam `killpg` atomicamente identity-safe.

Não se promete isolamento de segurança contra processos hostis. Se não for possível preservar a identidade do grupo, falhar fechado, conservar evidência e não fazer cleanup IPC.

## 8. Readiness e perfis

### 8.1 Contrato dos probes

Cada gate define:

- produtor e identidade esperada;
- ficheiro/offset de log do trial ou probe GDB;
- matcher explícito e campos de identidade;
- pré-condições, delay efectivo e deadline;
- artefacto bruto e resultado estruturado.

Markers anteriores ao lançamento, de outro boot ou papel não satisfazem gates. Delay configurado determina quando esperar activação; **decorrer o delay não prova readiness**. Polling bounded substitui sleeps cegos. Logs buffered podem requerer GDB, não uma promoção de `ps` a oráculo.

### 8.2 Gates por papel

| Papel | Gate mínimo |
|---|---|
| PGCS, ambos | Processo único, sockets/interface/peer MAC correctos; processamento receptor bidireccional e identidade peer observados |
| NRNCS Source | `OPERATIONAL: Everything ok!`, configuração efectiva e identidade NG; descoberta local SHM pelo PGCS Source |
| Repository | Core/GW operacional e App.ini efectivo; para gate control-plane, descoberta/readback NRNCS com identidade correspondente |
| Source | Staging exclusivo validado; Core/GW operacional; descoberta/subscrição e progresso de publicação conforme probes previamente definidos |

A SPEC-048 fornece markers `RECEIVER_HANDLER_ENTRY` e `RECEIVER_HANDLER_STATUS_OK`; frames sozinhos não demonstram processamento nem binding. A SPEC-049 fornece `OPERATIONAL: Everything ok!`; `HT_STORE_CALL` prova invocação, não persistência.

`Discovered a NRNCS!` sem identidade correlacionada não basta para readback. Os matchers exactos Core/GW, Source e lookup têm de ser extraídos dos logs/configurações/GDB reais e revistos antes de habilitar os respectivos perfis; não foram integralmente fornecidos nesta revisão.

### 8.3 Perfis e limites

- **`pgcs-only`**: dois PGCS, captura e probes receptores; nenhum NRNCS/ContentApp.
- **`nrncs-only`**: anterior + NRNCS Source; readiness e publicação→storage invocation. Readback permanece separado.
- **`repository-control`**: anterior + Repository, sem Source; observa descoberta e lookup. Ausência dentro da janela é resultado diagnóstico negativo, não defeito do executor por si só.
- **`photos-100`**: todos os papéis e oráculo byte-exact.

Para `photos-100`, gates PGCS/NRNCS e Core/GW Repository são obrigatórios antes de Source. O plano deve escolher explicitamente:

1. `strict-control`: readback Repository exigido antes de Source; ou
2. `matched-normal`: Source lançado após readiness local Repository, deixando descoberta/subscrição como gates conjuntos durante a execução bounded.

Não mudar de política após timeout. `matched-normal` não declara que Source é causa ou requisito arquitectural da descoberta e não fecha a SPEC-051. O início Source pode iniciar publicações imediatamente; sem barreira existente verificada, não prometer separar readiness Source de emissão de workload.

## 9. Teardown e IPC seguro

1. Recolher snapshot inicial e fixar resultado runtime antes dos sinais.
2. Parar Source → Repository → NRNCS → PGCS; capturas permanecem até registar os términos.
3. Para cada grupo com identidade confirmada: SIGTERM, grace, SIGKILL se necessário e verificação bounded de todos os membros.
4. Não considerar término do launcher/GDB suficiente. Confirmar aplicações e descendentes; zombies contam como resíduos até reap/ausência confirmada.
5. Encerrar capturas e registar contadores de drops/truncação.
6. Fazer inventário completo de processos, SHM, semáforos e outros IPC usados antes/depois. SHM inclui ID, key, owner, criador, tempos e `nattch`, quando disponíveis.
7. Não remover IPC enquanto existir qualquer consumidor NG correspondente, wrapper/GDB activo ou identidade desconhecida.

**Propriedade IPC:** diferença de inventários, UID ou key isolados não demonstram pertença. Exigir correlação com criador/processo do trial, metadados e baseline. Semáforos sem associação verificável permanecem preservados e causam cleanup incompleto. Para SHM, exigir ainda `nattch=0`.

Só então executar remoção por ID verificado, revalidando metadados imediatamente antes da operação. IDs reutilizados ou inventário incompleto bloqueiam remoção.

Critério final: zero processos e IPC **do trial**; recursos externos permanecem iguais ao baseline. Nos guests dedicados, exigir baseline de IPC relevante vazio e zero SHM/semáforos NG final. Nunca apagar IPC legítimo do sistema para satisfazer um contador global zero.

Directórios de evidência e staging não são apagados automaticamente. VM stop não pode mascarar resíduos.

## 10. Evidência e resultados

### 10.1 Organização

Raiz: `<NG_EVIDENCE_PATH>/<trial_id>/`, criada exclusivamente.

- `plan.effective.json`, `manifest.json`, `manifest.sha256`;
- `controller/events.jsonl`, `controller/transport.jsonl`;
- `vm-101/` e `vm-102/`:
  - `preflight.json`, `provenance.json`, `processes.jsonl`;
  - `<role>/stdout.log`, `stderr.log`, `wrapper.jsonl`, `exit.json`;
  - `gates.jsonl`, configurações e probes GDB;
  - `ipc.before.json`, `ipc.after-processes.json`, `ipc.final.json`;
  - capturas e estatísticas;
- `workload/publish-manifest.sha256`, mapas Source/NRNCS/Repository e comparação;
- `teardown.json`, `vm-lifecycle.json`, `assessment.md`.

Usar armazenamento guest persistente, nunca logs extensivos em `/tmp` tmpfs. Preflight verifica espaço e inodes; monitorização contínua aborta antes da reserva mínima configurada. Quotas e rotação segmentada não podem descartar logs sem marcar evidência incompleta.

O manifest lista schema, plano/hash, identidades, estados, gates, deadlines, exits brutos, sinais, caminhos relativos, tamanhos e SHA-256 de artefactos. Não inclui a própria hash; `manifest.sha256` sela o ficheiro final. Eventos usam UTC para correlação humana e tempo monotónico local para deadlines, sem subtrair relógios monotónicos de máquinas diferentes.

Recolha SCP usa destino temporário, verifica SHA-256 contra o inventário remoto selado e só então renomeia. Logs live são provisórios; recolher versão final após término.

### 10.2 Classificação independente

- `runtime_result`: `PASS`, `FAIL`, `INCONCLUSIVE`, `NOT_RUN`, `ABORTED`.
- `teardown_result`: `PASS`, `FAIL`, `UNKNOWN`.
- `evidence_result`: `COMPLETE`, `INCOMPLETE`.
- Registar `failure_stage`, `failure_class` e sinais/exits originais.

Classes mínimas: configuração, proveniência, identidade, transporte, readiness-timeout, aplicação, workload-mismatch, disco, escalada, IPC residual, recolha e VM-stop.

Serviços long-lived terminados planeadamente após o oráculo não são runtime crash. Preservar sinais/exits; não converter timeout `124/125` de wrappers antigos em sucesso implícito. SIGKILL inesperado mantém anotação de escalada e falha de teardown limpo, mesmo que resíduos sejam posteriormente removidos.

### 10.3 Exit status do controlador

| Código | Significado |
|---:|---|
| 0 | Objectivo do perfil satisfeito, teardown PASS, evidência COMPLETE |
| 2 | Configuração/CLI/plano inválido, antes de lançamento |
| 10 | Runtime FAIL com teardown/evidência completos |
| 11 | Runtime INCONCLUSIVE/NOT_RUN por infra-estrutura, com recuperação completa |
| 20 | Teardown FAIL/UNKNOWN ou evidência INCOMPLETE; precedência sobre runtime |
| 130 / 143 | Interrupção SIGINT/SIGTERM com recuperação completa |

Com interrupção e recuperação incompleta, devolver 20 e preservar o sinal no manifest. `preflight`/`dry-run` usam 0 somente para o seu próprio âmbito, nunca como trial PASS.

## 11. Plano de testes e aceitação

### 11.1 Offline

Python `unittest`, sem VMs ou credenciais reais:

1. Validação de env/JSON, quoting, caminhos, placeholders e segredos redigidos.
2. Fake SSH/SCP/helper: ordem de estados, deadlines, reconexão e hashes.
3. Duplicação de launch, perda de ACK, lock concorrente e boot ID alterado.
4. PID reutilizado, PGID perdido, descendente sobrevivente, zombie e escape de sessão.
5. Filho normal, crash antes de readiness, SIGTERM ignorado e escalada.
6. Morte de SSH/controlador e expiração de lease.
7. Marker antigo, errado, sem identidade, buffered ou recebido depois do deadline.
8. IPC alheio, IPC reutilizado, `nattch>0` e inventário parcial: nenhuma remoção.
9. Falha de SCP, hash divergente, disco cheio, recolha interrompida e VM-stop falho.
10. Sinais locais, recuperação idempotente e precedência dos exit codes.

Executar compilação sintáctica Python e `bash -n`/`sh -n` conforme shebang dos ficheiros shell alterados. Testes offline não provam semântica real de SSH, GDB, `/proc`, IPC ou `qm`.

### 11.2 Ensaios reais, sempre com novo stop/start

1. **Harness remoto com filhos sintéticos:** ambos os guests; desconectar/matar SSH e controlador; demonstrar que lease termina grupos remotos. Incluir filho que ignora SIGTERM. Cada cenário real constitui novo trial.
2. **PGCS-only cross-VM:** proveniência, exactamente um PGCS por guest, peer MAC correcta, processamento receptor em ambos os sentidos e teardown/evidência completos. Não prova bindings, NRNCS ou payload.
3. **NRNCS-only:** operacional, identidade, descoberta local e publicação→invocação storage correlacionadas. Não prova readback.
4. **Repository control-plane:** sem Source; Core/GW, descoberta e lookup por identidade, ou ausência localizada com timeout explícito. Um runtime diagnóstico negativo pode validar o comportamento do executor, mas não vale PASS control-plane.
5. **Posterior `photos-100`:** 100 JPEGs novos, staging passado directamente como IO Source, sem alterar `IO/Source1` ou symlink partilhado; exactamente 100 publicações JPEG e mapas nome→SHA-256 iguais em Source, cache NRNCS e Repository, sem extras/faltas. Comparar com manifest publish-time selado e revalidado, não com Source mutável.

Antes do último ensaio, Hermes confirma geração/decodificação/dimensões, política stdlib do gerador, aceitação de staging read-only pelo ContentApp e exclusão de `manifest.sha256` da publicação. `BuildPhotos.py` não foi fornecido; não presumir essas propriedades.

Adicionar teste negativo de comparação: corromper exactamente uma cópia recebida numa fixture preservada e obter exactamente um mismatch. Não modificar a evidência original.

Liveness é progresso de contadores concluídos, não CPU. Quando houver telemetria de perdas, calcular deltas entre duas amostras compatíveis, nunca usar campos elapsed/timestamps como denominador substituto.

### 11.3 Matriz de aceitação

| Critério | Artefacto reproduzível |
|---|---|
| Sintaxe e contrato CLI/configuração | Relatório unitário + comandos/versões |
| Sem launch duplicado, PID stale rejeitado | Fixtures identity/idempotency + journal |
| Morte de SSH não abandona aplicações | Trial sintético, lease events, inventário remoto final |
| SIGTERM/grace/SIGKILL bounded | Eventos monotónicos e exits sintéticos |
| Nenhuma remoção IPC com consumidores/alheios | Testes negativos + três inventários IPC |
| Reinício limpo e exclusividade | Boot IDs, locks, `vm-lifecycle.json` |
| Proveniência exacta | `provenance.json`, recibos e hashes |
| PGCS receptor bidireccional | Logs GDB, identidades, captura e gate assessment |
| NRNCS operacional/storage invocation | Markers e tuplos categoria/chave/valor |
| Repository readback ou ausência explícita | Probe lookup e assessment sem Source |
| 100/100 byte-exact posterior | Manifest publish-time e três mapas SHA-256 |
| Detector encontra corrupção única | Relatório negativo com um mismatch |
| Evidência completa antes de VM stop | Manifest verificado e sequência lifecycle |
| Runtime separado do teardown | Casos combinados de resultado e exit status |
| Compatibilidade e ausência de produção/segredos | Diff revisto, README e auditoria de ficheiros |

## 12. README operacional

Actualizar secções 7–9, sem endereços, MACs, caminhos pessoais, passwords ou chaves reais:

1. Pré-requisitos local/guest/host, permissões raw socket/tcpdump/GDB/IPC e `qm`.
2. Configuração ignorada, host keys verificadas e peer MAC como MAC **do outro guest**.
3. `dry-run`, `preflight` e interpretação dos seus limites.
4. Comandos por perfil e reinício obrigatório por novo trial.
5. Readiness versus liveness; diferença entre perfis diagnósticos e funcional.
6. Estrutura de evidência, códigos de saída e preservação externa.
7. Recuperação com `status`, `collect`, `cleanup`, `finalize`.
8. Troubleshooting: SSH 255, lease expirada, MAC errada, buffering, delay periódico, símbolos GDB, ENOSPC, IPC ocupado e identidade divergente.
9. Limitações: Linux/SSH disponível, foreground groups, ausência de contenção hostil, GDB perturbando timing e descoberta SPEC-051 ainda aberta.
10. Advertência explícita: não executar `clean.sh` nem launchers manuais durante um trial supervisionado.

O README não deve recomendar limpeza global nem “parar a VM para resolver” antes de recuperar evidência e verificar cleanup.

## 13. Migração, rollback e segurança

1. Implementar de forma aditiva; preservar alterações existentes da árvore de trabalho. Rever apenas ficheiros desta SPEC.
2. Primeiro testes offline/sintéticos, depois PGCS, NRNCS e Repository; habilitar `photos-100` apenas após os gates aplicáveis.
3. Não integrar no fluxo de release da SPEC-045 antes de revisão dos resultados Astra. Esta SPEC concretiza o executor remoto pendente da SPEC-046, não fecha automaticamente os seus critérios.
4. Rollback: deixar de usar o executor e regressar ao modo manual **somente após** cleanup/evidência final ou quarentena resolvida. Não reverter produção.
5. Helper por trial, sem serviço instalado; conservar artefactos. Remoção futura apenas por retenção explícita e ownership verificado.
6. Não copiar chaves para guests, encaminhar agente, activar shell tracing com segredos ou registar ambiente completo.
7. Configurações/artefactos remotos com permissões restritas; rejeitar symlinks e path traversal em raízes de trial e destinos SCP.
8. Acesso `qm` restrito a VM101/102; privilégios guest mínimos compatíveis com ferramentas. Não alterar `sudoers` automaticamente.
9. Evidência bruta pode conter identidade de implantação, payloads, memória GDB e core dumps: armazenar fora do Git com acesso restrito. Publicar apenas resumo redigido e hashes.
10. Root pode contornar chmod; hashes externos e proveniência são tripwires, não fronteira de segurança.

## 14. Emenda de integração com SPEC-053

O executor deve aceitar `scenario`, `debug_profile`, `build_manifest` e `observation_contract` no plano efectivo. A topologia do cenário e o perfil de debug são seletores independentes; combinações incompatíveis falham antes do lançamento. O executor não chama os launchers manuais `run_*.sh`, e um build targeted só pode ser usado quando o manifesto comprovar a selecção por unidade de compilação definida na SPEC-053.

