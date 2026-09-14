# SPEC-053: Matriz de observabilidade, logs e perfis debug

**Author:** Antonio Alberti / Astra  
**Date:** 2026-09-12  
**Status:** Proposal  
**Branch:** AIOPT3  
**Implementation commit:**  
**Related:** SPEC-052-remote-ssh-multi-vm-executor.md; SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-047-pgcs-readiness-periodic-activation.md; SPEC-048-pgcs-receiver-processing-evidence.md; SPEC-049-nrncs-readiness-binding-oracles.md; SPEC-050-contentapp-repository-control-plane-readback.md; SPEC-051-cross-vm-nrncs-discovery-diagnosis.md; SPEC-045-core-evaluate-functional-harness.md; SPEC-030, SPEC-028, SPEC-022, SPEC-019; Docs/NG-INVERTED-PUB-SUB-MODEL.md.

## 1. Objectivo e separação de responsabilidades

Esta SPEC define o inventário verificável de observabilidade existente, perfis de compilação debug limitados e uma escada de cenários L0–L5. Não implementa um segundo executor.

- **SPEC-052:** ciclo de vida remoto, identidade, lease, readiness, workload, teardown e evidência.
- **SPEC-053:** quais observações recolher, de que código/configuração provêm, como compilar variantes e quais conclusões permitem.

Mantém-se a arquitectura da SPEC-052: helper remoto por trial, monitor desacoplado de SSH, lease e cleanup por identidade. Acrescentam-se ao seu contrato `scenario`, `debug_profile`, `build_manifest` e `observation_contract`.

### 1.1 Incluído

1. Inventariar todos os pontos de emissão runtime e controlos `DEBUG*` da árvore corrente.
2. Distinguir presença textual, definição efectiva, ramo compilado e emissão observada.
3. Compilar variantes isoladas sem editar produção C++.
4. Seleccionar logs/probes por cenário e impedir debug amplo por omissão.
5. Preservar bytes brutos, limites, buffering, proveniência e resultados estruturados.
6. Testes offline e ensaios remotos progressivos com VM101/102.

### 1.2 Não-objectivos

- Novos logs/markers C++, alterações de protocolo ou comportamento de produção.
- Explicar a causa aberta da SPEC-051.
- Inferir persistência de binding a partir de chamada de armazenamento.
- Inferir entrega correcta de liveness, frames ou mensagens de publicação.
- Garantir equivalência temporal entre execução normal e GDB/debug.
- Alterar o gerador JPEG ou aceitar alteração de workload implicitamente.
- Desactivar macros definidas no próprio código através de edição automática.
- Activar logging global, serviços pagos ou dependências Python não-stdlib.

Se a observabilidade existente não permitir um gate, declarar a lacuna. Novos markers exigem SPEC de implementação separada.

## 2. Inventário fundamentado em código

### 2.1 Estados obrigatórios

Cada macro/ponto de log terá estados independentes:

| Campo | Significado |
|---|---|
| `declared_active` | Directiva activa no texto fornecido; não prova configuração efectiva |
| `declared_commented` | Declaração comentada; não activa compilação |
| `guarded` | Usada em condição do pré-processador |
| `effective` | Valor/estado no ponto concreto, com flags/includes reais |
| `compiled` | Ramo sobrevivente ao pré-processamento da unidade |
| `observed` | Emissão realmente recolhida, ligada a trial/processo/perfil |
| `unknown_reason` | Razão explícita quando algum estado não puder ser demonstrado |

`#define DEBUG 0` continua verdadeiro para `#ifdef DEBUG`. `-UDEBUG` não neutraliza um `#define DEBUG` posterior no ficheiro. O resultado final de `-dM` não demonstra, isoladamente, o estado da macro em todos os pontos anteriores.

### 2.2 Método de descoberta

O inventariador deve:

1. Fixar snapshot de fonte: HEAD, dirty state, hashes e lista de ficheiros incluídos/excluídos. Não limpar a árvore.
2. Percorrer fontes/headers runtime e configuração de build, excluindo apenas dependências/builds gerados explicitamente listados.
3. Localizar:
   - `#define`, `#undef`, `#if`, `#ifdef`, `#ifndef`, `defined(...)`;
   - variantes `DEBUG*`, includes e flags `-D`/`-U`;
   - sinks existentes: streams C++, stdio, descritores, syslog, ficheiros e wrappers próprios identificados na primeira auditoria;
   - warnings/erros e mensagens incondicionais, não apenas blocos debug.
4. Registar ficheiro, linha, símbolo/função, hash do excerto, literal ou construção dinâmica, sink, guardas e possíveis caminhos de alta frequência.
5. Expandir wrappers de logging e rever manualmente emissões indirectas/dinâmicas que a pesquisa textual não resolver.
6. Reexecutar o pré-processador com a invocação real de cada unidade seleccionada; guardar comandos, dependências, trace de definições e ramos relevantes.
7. Confrontar inventário estático com logs dos ensaios. “Não observado” não significa inexistente.
8. Produzir inventário completo e lista de cobertura/lacunas. Enquanto houver sinks ou ficheiros não auditados, não rotular o inventário como completo.

Regex pode descobrir candidatos, mas não é prova suficiente de actividade de macro ou cobertura de logs.

### 2.3 Inventário inicial verificado

Legenda: **A** = definição activa no texto; **C** = definição comentada; **G** = guardas. `—` significa nenhuma ocorrência dessa classe reportada, não ausência de logs incondicionais.

Caminhos abaixo são relativos à raiz. Este é o baseline fornecido; flags/includes efectivos e literais completos ainda precisam de auditoria.

| Ficheiro(s) | A | C | G |
|---|---|---|---|
| `Common/src/GW.cpp` | — | DEBUG, DEBUG1, DEBUG3, DEBUG_NETWORK_QUEUE | DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG_NETWORK_QUEUE |
| `Common/src/Process.cpp` | — | DEBUG, DEBUG1, DEBUG3 | DEBUG, DEBUG1, DEBUG3 |
| `Common/src/NGAL_SAR.cpp` | — | DEBUG | DEBUG |
| `Common/src/Block.cpp` | — | DEBUG, DEBUG1, DEBUG2 | DEBUG, DEBUG1, DEBUG2 |
| `Common/src/GWExposition02.cpp`, `GWHelloIPC02.cpp`, `GWRunHelloIPC02.cpp`, `GWSCNSeq01.cpp`, `HTDeliveryBind01.cpp` | — | DEBUG | DEBUG |
| `Common/src/GWMsgCl01.cpp` | — | DEBUG, DEBUG1 | DEBUG, DEBUG1 |
| `Common/src/HTBindReport01.cpp`, `HTListBind01.cpp`, `HTRevokeBind01.cpp` | — | — | DEBUG |
| `Common/src/HTInfoPayload01.cpp` | — | DEBUG | — |
| `Common/src/Message.cpp` | — | — | DEBUG, DEBUG1 |
| `PGCS/src/PG.cpp` | — | DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG5, DEBUG6 | DEBUG, DEBUG1, DEBUG2, DEBUG3 |
| `PGCS/src/NGAL_Transport_RAW.cpp`, `PGHelloIHC01.cpp`, `PGHelloIHC02.cpp`, `PGHelloIHC03.cpp`, `PGRunPeriodic01.cpp` | — | DEBUG | DEBUG |
| `PGCS/src/CoreNotifyS01.cpp` | — | DEBUG, DEBUG1 | DEBUG, DEBUG1 |
| `PGCS/src/CoreRunEvaluate01.cpp`, `PGMsgCl01.cpp`, `PGRunExposition01.cpp`, `PGRunPublishing01.cpp`, `PGRunStresstest01.cpp`, `PGStresstestPing01.cpp` | — | DEBUG | DEBUG |
| `PGCS/src/PGRunHello01.cpp`, `PGRunHello03.cpp` | — | — | DEBUG |
| `PGCS/src/PGRunInitialization01.cpp` | — | DEBUG | — |
| `NRNCS/src/NRPubNotify01.cpp`, `NRInfoPayload01.cpp` | DEBUG | — | DEBUG |
| `NRNCS/src/NRRunInitialization01.cpp`, `NRRunPeriodic01.cpp`, `NRPubBind01.cpp` | — | DEBUG | DEBUG |
| `NRNCS/src/NRDeliveryBind01.cpp`, `NRMessageSeq01.cpp`, `NRMsgCl01.cpp`, `NRNCS.cpp`, `NRRevokeBind01.cpp`, `NRSCNSeq01.cpp`, `NRSubBind01.cpp`, `execNRNCS.cpp` | — | DEBUG | DEBUG |
| `NRNCS/src/NR.cpp` | — | DEBUG | — |
| `ContentApp/src/CoreRunPeriodic01.cpp`, `CoreRunDiscover01.cpp`, `CoreRunEvaluate01.cpp` | — | DEBUG | DEBUG |
| `ContentApp/src/CoreRunContentPublish01.cpp` | — | DEBUG | DEBUG, DEBUG1 |
| `ContentApp/src/CoreNotifyS01.cpp` | — | DEBUG, DEBUG1 | DEBUG, DEBUG1 |
| `ContentApp/src/Core.cpp`, `CoreDeliveryBind01.cpp`, `CoreInfoPayload01.cpp`, `CoreMsgCl01.cpp` | — | DEBUG | DEBUG |
| `ContentApp/src/CoreRunExpose01.cpp`, `CoreRunInitialize01.cpp`, `CoreRunInvite01.cpp`, `CoreRunPublish01.cpp`, `CoreRunPublish02.cpp`, `CoreRunPublish03.cpp`, `CoreRunSubscribe01.cpp`, `CoreSCNSeq01.cpp` | — | DEBUG | DEBUG |
| `ContentApp/src/ContentApp.cpp`, `CoreSCNAck01.cpp`, `CoreStatusS01.cpp`, `execContentApp.cpp` | — | DEBUG | — |

Dentro de uma célula, nomes sem prefixo herdam o directório do primeiro ficheiro.

Não se infere comportamento observável de macros sem guardas reportadas. Não activar `DEBUG5/6` de `PG.cpp` só porque existem declarações comentadas.

### 2.4 Artefactos do inventário

- `inventory.json`: macros, guardas, sinks, origem, cobertura e lacunas.
- `logs.jsonl`: um registo por ponto de emissão.
- `macro-effective.jsonl`: actividade por unidade, ponto, perfil e invocação.
- `inventory.md`: resumo legível, agrupado por cenário.
- `coverage.json`: ficheiros auditados, exclusões, emissões dinâmicas pendentes.

IDs de log devem combinar caminho, função e hash do excerto; linha é localização auxiliar, não identidade estável.

## 3. Perfis debug direccionados

Todos os perfis partem de `obs-normal`. A tabela define **acréscimos por unidade de compilação**, não `-DDEBUG` global.

| Perfil | Ficheiros e macros seleccionados | Uso e restrição |
|---|---|---|
| `obs-normal` | Nenhum define adicional | Baseline real; auditar DEBUG já activo em NRNCS |
| `obs-startup` | `NRNCS/src/NRRunInitialization01.cpp:DEBUG`; `ContentApp/src/CoreRunInitialize01.cpp:DEBUG` | Inicialização dos papéis presentes; PGCS usa mensagens existentes/probes, não define sem guarda |
| `obs-gw-shm` | `Common/src/GW.cpp:DEBUG`; `GWHelloIPC02.cpp:DEBUG`; `GWRunHelloIPC02.cpp:DEBUG` | Caminho local GW/SHM; GW pode ser volumoso, usar apenas janela limitada |
| `obs-hello` | `PGCS/src/PGHelloIHC01.cpp:DEBUG`; `PGHelloIHC02.cpp:DEBUG`; `PGHelloIHC03.cpp:DEBUG`; `PGRunPeriodic01.cpp:DEBUG`; `PGRunHello01.cpp:DEBUG`; `PGRunHello03.cpp:DEBUG`; `PGRunExposition01.cpp:DEBUG`; `Common/src/GWExposition02.cpp:DEBUG` | Hello, periodicidade e exposição; não prova persistência de binding |
| `obs-raw-sar` | `PGCS/src/NGAL_Transport_RAW.cpp:DEBUG`; `Common/src/NGAL_SAR.cpp:DEBUG` | Transporte/segmentação sem workload de fotos por omissão |
| `obs-nrncs-binding` | `NRNCS/src/NRRunInitialization01.cpp:DEBUG`; `NRRunPeriodic01.cpp:DEBUG`; `NRPubBind01.cpp:DEBUG`; `NRDeliveryBind01.cpp:DEBUG`; `NRSubBind01.cpp:DEBUG`; `Common/src/HTBindReport01.cpp:DEBUG`; `HTDeliveryBind01.cpp:DEBUG`; `HTListBind01.cpp:DEBUG` | Binding/report/delivery; não presumir que estes hooks oferecem lookup independente |
| `obs-core-control` | `ContentApp/src/CoreRunPeriodic01.cpp:DEBUG`; `CoreRunDiscover01.cpp:DEBUG`; `CoreRunEvaluate01.cpp:DEBUG`; `CoreRunSubscribe01.cpp:DEBUG`; `CoreNotifyS01.cpp:DEBUG`; `CoreDeliveryBind01.cpp:DEBUG` | Descoberta, subscrição e avaliação ContentApp; não confundir com `PGCS/src/CoreRunEvaluate01.cpp` |
| `obs-payload-cache` | `ContentApp/src/CoreRunContentPublish01.cpp:DEBUG`; `CoreInfoPayload01.cpp:DEBUG` | Publicação/recepção; inclui auditoria dos DEBUG activos em `NRInfoPayload01.cpp` e `NRPubNotify01.cpp`, sem os redefinir |
| `obs-teardown-ipc` | Nenhum define adicional | Journals do helper, `/proc`, inventários IPC e sinais são a fonte autoritativa |
| `obs-verbose-window` | União explícita de no máximo dois perfis direccionados | Somente diagnóstico, janela limitada e aprovação do plano; nunca união de todos os perfis |

Regras obrigatórias:

1. Não activar inicialmente nenhum DEBUG adicional em `PGCS/src/PG.cpp`.
2. Proibir inicialmente `DEBUG2`, `DEBUG3`, `DEBUG5`, `DEBUG6` em todos os perfis e `DEBUG_NETWORK_QUEUE` em GW.
3. Não activar broad DEBUG em `Common/src/Process.cpp`, `Block.cpp` ou `Message.cpp` nos perfis iniciais.
4. `DEBUG1` não é necessário nesta versão; não o activar implicitamente.
5. Uma flag global que torne efectivos esses níveis viola o perfil, mesmo sem constar na receita.
6. Se o ramo controlado alterar estado/fluxo, avaliar argumentos com efeitos laterais ou não for estritamente observacional, rejeitar a activação. O perfil fica bloqueado até revisão; não editar C++ para o tornar aceitável.
7. Macros em headers podem afectar outras entidades compiladas na unidade. O manifest regista **efeito transitivo**, não apenas o ficheiro seleccionado.
8. Perfil nenhum garante ausência de flood. Medir volume antes de subir de nível.

## 4. Contrato de build reproduzível

### 4.1 Isolamento

Gerar builds fora da árvore de produção, sob raiz local configurada, separados por snapshot, variante e hash do perfil:

- `normal`: flags normais verificadas, sem acréscimos DEBUG.
- `targeted-debug`: mesmas optimizações/configuração da normal, acrescidas apenas das selecções auditadas e símbolos quando necessários.
- `verbose-diagnostic`: variante com `obs-verbose-window`, nunca candidata automática a aceitação funcional.

Usar CMake e compilador existentes. Um adaptador de build do tooling aplica flags **por unidade**, em build isolado, sem editar fontes C++ ou o `CMakeLists.txt` existente. A compatibilidade deste mecanismo com a configuração real deve ser testada antes de qualquer trial.

Não sobrescrever `cmake-build-debug` ou binários usados pelo modo manual. O executor lança o caminho exacto do artefacto seleccionado.

### 4.2 Manifest obrigatório

`build-manifest.json` contém:

- versão do schema, ID/hash do perfil e inventário;
- commit completo, branch, estado dirty e hash do snapshot;
- compilador/linker/versões, alvo, flags completas, includes e configuração;
- `compile_commands.json`, link commands e hashes das dependências relevantes;
- macros solicitadas versus efectivas, incluindo definições internas activas;
- ramos auditados e efeitos transitivos;
- caminho, formato, tamanho e SHA-256 de cada binário;
- hashes de configurações de build e probes GDB;
- limitações e estatuto `verified`, `partial` ou `rejected`.

O contrato runtime acrescenta hash da configuração efectiva da aplicação. Não confundir configuração de build com `App.ini`/IO efectivo.

A árvore corrente é dirty: commit sozinho não identifica o executável. Builds diagnósticos de snapshot dirty exigem opt-in explícito e proveniência completa; não se tornam evidência de release por comparação de HEAD.

### 4.3 Verificações

- Rebuild sem reutilização de objectos incompatíveis.
- Confirmar flags/macros reais, não apenas a receita.
- Se símbolos forem necessários, verificar funções/linhas no binário antes de GDB.
- Repetir build do mesmo snapshot/perfil; explicar diferenças de hash, se existirem. Não prometer determinismo byte-a-byte sem o demonstrar.
- Registar hashes guest após cópia e antes de launch.
- Não interpretar `strings` como prova de emissão runtime ou de ramo executado.

Uma execução debug pode demonstrar um ponto causal observado. Não substitui o ensaio funcional com `obs-normal`.

## 5. Escada de cenários e gates

Um nível é um contrato de topologia/oráculos, não permissão para lançar os papéis de níveis superiores. Cada novo trial real exige stop/start completo de ambas as VMs segundo SPEC-052.

Limites abaixo são por trial: **deadline readiness + observação/workload**, excluindo teardown/recolha bounded da SPEC-052. Os delays efectivos devem caber no orçamento; se não couberem, rejeitar o plano, não reduzir silenciosamente a observação.

| Nível | Componentes permitidos | Gates e fontes úteis | Perfil exigido | Limites padrão | Não prova |
|---|---|---|---|---|---|
| **L0** | Nenhum serviço NG | Identidade VM/SSH, boot, proveniência, macros, configuração, disco, ferramentas, baseline processos/IPC | `obs-normal` | 180 s; 16 MiB/VM | Readiness ou funcionamento NG |
| **L1** | Um PGCS na VM seleccionada; outra VM sem NG | Processo único; inicialização GW; identificação/criação/attach dos recursos SHM próprios; execução do caminho IPC local quando efectivamente emitido. Logs GW e inventários `/proc`/IPC | `obs-gw-shm` | 240 + 30 s; 128 MiB/VM | Peer remoto, troca IPC com serviço ausente ou descoberta NRNCS |
| **L2** | PGCS em ambas | Sockets e peer MAC correctos; frames nos dois sentidos; entrada e retorno OK do handler receptor em ambos, ligados aos processos do trial. Hello logs, GDB e pcap | `obs-hello`; `obs-raw-sar` apenas em trial alternativo | 240 + 30 s; 128 MiB logs/VM; 64 MiB captura/VM | Estado persistido dos bindings ou payload |
| **L3** | L2 + NRNCS Source | `OPERATIONAL: Everything ok!`; identidade NG; descoberta local SHM; publicação e storage invocation com categoria/chave/valor correlacionados. NRNCS, PGCS, probes HT | `obs-nrncs-binding` | 240 por gate + 30 s; 128 MiB logs/VM; 64 MiB captura/VM | Readback independente, subscrições ou entrega |
| **L4** | L3 + Repository; sem Source ContentApp | Core/GW operacional, App.ini efectivo, descoberta NRNCS por HID/OSID/PID/BID e lookup independente do binding esperado. Logs Core e probes direccionados | `obs-core-control` | 240 por gate + 60 s; 128 MiB logs/VM; 64 MiB captura/VM | Payload ou causa da ausência discovery-only |
| **L5** | PGCS ambos, NRNCS Source, Repository, Source | Readiness conforme política congelada; staging novo selado; 100 JPEGs publicados; conjuntos de nomes e SHA-256 Source=NRNCS=Repository, sem extras/faltas; manifest publish-time revalidado | `obs-normal` para aceitação; `obs-payload-cache` só em repetição diagnóstica | 240 por gate + 600 s workload; 256 MiB logs/VM; captura desactivada por omissão | Performance, release completa ou solução da SPEC-051 |

Em todos os níveis são proibidos broad PG/debugs enumerados na §3. O orçamento total até teardown permanece bounded pelo contrato SPEC-052; plano que exceda 1800 s exige orçamento explícito revisto antes do launch.

### 5.1 Precisões dos gates

1. **L1:** PGCS isolado pode não produzir troca local IPC suficiente. Não inventar peer nem iniciar NRNCS para “fechar” L1. Criação/attach e troca observada são subgates separados; troca não exercitada fica `NOT_EXERCISED`.
2. **L2:** usar `RECEIVER_HANDLER_ENTRY` e `RECEIVER_HANDLER_STATUS_OK` quando os probes GDB da SPEC-048 forem verificados para o binário actual. Captura isolada não fecha processamento receptor.
3. **L3:** `NRPUB_INGRESS` e `HT_STORE_CALL` são observações de instrumentação cuja disponibilidade precisa de verificação. Chamada `StoreBinding` não é leitura posterior.
4. **L4:** `Discovered a NRNCS!` exige identidade correlacionada. Os matchers exactos Core/GW e lookup devem ser inventariados antes de habilitar o gate. Não declarar readback só pelo texto de descoberta.
5. **L5:** conservar escolha explícita SPEC-052 entre `strict-control` e `matched-normal`. Na segunda, Source pode iniciar após readiness local Repository; descoberta/subscrição são gates conjuntos bounded, sem alegar que Source causa descoberta NRNCS.
6. O modelo permanece: `NRInfoPayload01` guarda em disco, não encaminha payload; `HTGetBind01`, categoria 18, serve cache. O inventário não fornece aqui macro verificada para `HTGetBind01`; não inventar uma.
7. Um atraso configurado, incluindo activação periódica tardia, ajusta a janela de observação; tempo decorrido não constitui readiness.

## 6. Contrato de logs e observações

### 6.1 Armazenamento e identidade

Usar a raiz persistente remota configurada na SPEC-052, nunca `/tmp` tmpfs. Estrutura adicional:

- `<trial>/observability/profile.json`
- `<trial>/observability/inventory-ref.json`
- `<trial>/<vm>/<role>/stdout.NNN.log`
- `<trial>/<vm>/<role>/stderr.NNN.log`
- `<trial>/<vm>/<role>/observations.jsonl`
- `<trial>/<vm>/<role>/collector.jsonl`

Cada stream tem sidecar com trial, VM, boot ID, role, PID/starttime, commit/snapshot, hash binário/configuração/perfil e origem do stream. Não modificar bytes originais para lhes acrescentar prefixos.

Separar stdout/stderr. Como a aplicação pode escolher os seus próprios sinks, logs internos em ficheiro também são inventariados e recolhidos. Saída inferior misturada pelo GDB deve ser rotulada `gdb-combined`; não alegar separação que não existe.

### 6.2 Observação estruturada

O collector/parser gera JSONL, sem mudanças C++, com:

`schema_version`, `trial_id`, `vm_id`, `boot_id`, `role`, `linux_pid`, `starttime`, `profile_hash`, `gate_id`, `source_kind`, `stream`, `segment`, `byte_start`, `byte_end`, `observed_utc`, `observed_monotonic`, `marker`, `fields`, `result`, `limitations`.

- Timestamp do collector é instante de observação, não necessariamente de emissão.
- PID Linux e PID de identidade NG são campos diferentes.
- Regex e parsers são versionados; cada resultado aponta para bytes brutos.
- GDB-generated markers têm `source_kind=gdb`, não `application`.
- Parser não promove mensagens ambíguas a observações mais fortes.
- Não usar relógios monotónicos de máquinas diferentes como se fossem sincronizados.

### 6.3 Buffering e ausência

Não exigir marker visível imediatamente. Evitar pseudo-TTY para forçar flushing, pois muda o ambiente. `stdbuf` ou equivalente só se existente, aplicável e registado; não resolve necessariamente streams C++ ou buffers próprios.

Resultados de observação:

- `OBSERVED`: evidência positiva válida.
- `NOT_OBSERVED`: janela válida, mas evento não visto.
- `INCONCLUSIVE`: buffering, truncação, identidade, parser ou transporte impedem conclusão.
- `NOT_EXERCISED`: caminho não exercitado pela topologia.
- `NEGATIVE_OBSERVED`: retorno/erro explícito com contexto válido.

Marker ausente, por si só, não é runtime FAIL. O executor bloqueia o próximo papel e termina com runtime `INCONCLUSIVE` quando esse marker era gate obrigatório. Um erro explícito ou hash funcional incorrecto continua a poder produzir `FAIL`, independentemente de outros logs.

### 6.4 Volume e backpressure

- Segmentos de 16 MiB; rotação pelo collector, sem `copytruncate` nem sobrescrita.
- Retenção de todos os segmentos dentro da quota; não descartar os antigos.
- Aviso a 75%; iniciar aborto controlado a 90%.
- Reservar espaço adicional para grace/kill/journals: pelo menos 64 MiB/VM fora da quota de logs e capacidade livre inicial de duas vezes o orçamento planeado.
- Preflight exige pelo menos 1 GiB e inodes suficientes na filesystem de evidência, além de payloads/artifacts previstos.
- Se a reserva for ameaçada ou a quota atingida: fechar workload/admissões, solicitar término, preservar prefixo e contabilizar bytes perdidos; `evidence_result=INCOMPLETE`.
- Collector deve drenar pipes independentemente dos parsers; falha de parser não pode bloquear aplicação.
- `obs-verbose-window`: observação útil máxima 30 s após activação do alvo, quota 64 MiB/VM, sem L5. Como macros não se desligam em runtime, contabilizar também a fase anterior e terminar o trial ao fechar a janela.
- Capturas têm quota própria, estatísticas de drops e inventário de segmentos.

Quotas não são garantia de captura integral sob flood. Overflow torna o diagnóstico limitado, nunca silenciosamente completo.

Raw logs, pcaps e dumps podem expor identidades, payloads e memória. Guardar fora do Git com permissões restritas; publicar resumo redigido, hashes e referências.

## 7. Integração e interface operacional

### 7.1 Layout aditivo

Sob `Scripts/AlpineVMs/`:

- `ng_observability.py` — inventário, validação, builds isolados e relatórios;
- `observability/profiles/*.json` — perfis da §3;
- `observability/scenarios/L0.json` … `L5.json`;
- `observability/matchers/*.json` — matchers e requisitos de probes;
- `observability/schemas/` — contratos versionados;
- `tests/observability/` — fixtures e testes.

Inventários expandidos/builds/logs reais ficam na raiz local ignorada configurada. Só receitas neutras, fixtures sintéticas e assessments redigidos são rastreados.

### 7.2 CLI proposta

Interface, não implementação:

| Operação | Comando |
|---|---|
| Inventariar | `python3 Scripts/AlpineVMs/ng_observability.py inventory --source <tree> --compile-commands <json> --output <dir>` |
| Validar | `python3 Scripts/AlpineVMs/ng_observability.py validate --inventory <json> --profile <id> --scenario <L0..L5>` |
| Build | `python3 Scripts/AlpineVMs/ng_observability.py build --source <tree> --profile <id> --variant <normal|targeted-debug|verbose-diagnostic> --output <dir>` |
| Avaliar logs | `python3 Scripts/AlpineVMs/ng_observability.py assess --trial <dir>` |
| Planear NG-ELC | `python3 Scripts/AlpineVMs/ng_remote_executor.py dry-run --mode remote --plan <json> --scenario <L0..L5> --debug-profile <id>` ou `--mode local --scenario local-intra-os` |
| Verificar NG-ELC | `python3 Scripts/AlpineVMs/ng_remote_executor.py preflight --mode remote --plan <json> --scenario <L0..L5> --debug-profile <id>` ou `--mode local --plan Scripts/AlpineVMs/plans/local-intra-os.example.json --scenario local-intra-os --debug-profile <id>` |
| Executar NG-ELC | `python3 Scripts/AlpineVMs/ng_remote_executor.py run --mode remote --plan <json> --scenario <L0..L5> --debug-profile <id>` ou `--mode local --plan Scripts/AlpineVMs/plans/local-intra-os.example.json --scenario local-intra-os --debug-profile <id>` |

Selectors CLI são congelados no plano efectivo. Divergência com build manifest bloqueia launch.

O `--profile` da SPEC-052 continua a representar topologia, não debug. Mapear L2→`pgcs-only`, L3→`nrncs-only`, L4→`repository-control`, L5→`photos-100`; L0/L1 acrescentam contratos próprios. Se ambos os selectores forem fornecidos e incompatíveis, rejeitar.

### 7.3 Responsabilidades do executor

1. Validar cenário/perfil/build e lançar somente papéis autorizados.
2. Registar normal e targeted builds como trials diferentes.
3. Confirmar logger/captura/probes e quotas antes do serviço correspondente.
4. Aplicar matchers por identidade e offsets exclusivos do trial.
5. Não invocar `run_*.sh` manuais.
6. Não encerrar monitor/colecção antes de recolher sinais, exits e inventários finais.
7. Preservar classificação runtime/teardown/evidência separada da SPEC-052.
8. Interrupção, disco cheio ou marker inconclusivo seguem cleanup remoto seguro; nunca autorizam apagar IPC com consumidores vivos.

## 8. Testes

### 8.1 Offline, sem VMs

1. Fixtures com `#define` activo/comentado, `#ifdef`, `#if DEBUG`, `#undef`, redefinição em header e `-D`/`-U`.
2. Garantir que `DEBUG=0` não é tratado como desligado em `#ifdef`.
3. Detectar `-DDEBUG` global, PG amplo, DEBUG2/3/5/6 e efeito transitivo fora da allowlist.
4. Detectar perfil inexistente, manifest ausente, fonte/binário/configuração divergentes.
5. Verificar cobertura de sinks, emissões dinâmicas pendentes e logs incondicionais.
6. Fake logs com marker stale, PID reutilizado, VM/boot/role errados, linha dividida entre segmentos e stderr separado.
7. Marker ausente/buffered → `INCONCLUSIVE`, sem launch dependente.
8. Hash incorrecto ou erro explícito → resultado negativo adequado, sem esconder por buffering.
9. Disco cheio, quota, overflow, parser lento e collector interrompido: sem deadlock nem evidência falsamente completa.
10. Sinais/cleanup: raw preservado, nenhum IPC removal enquanto consumidor existe.
11. Sintaxe Python e shell conforme os ficheiros efectivamente adicionados/alterados.

### 8.2 Reais

Executar L0→L4 progressivamente, mas não reutilizar boot, cache, staging ou trial. Confirmar exactamente um PID de aplicação por papel activo.

- L0: provar configuração e proveniência, não readiness.
- L1: observar SHM local e reportar explicitamente caminhos não exercitados.
- L2: correlacionar MACs, frames e handler receptor.
- L3: readiness e tuplos de publicação/storage invocation.
- L4: readback positivo ou ausência/inconclusão correctamente classificada, sem Source.
- L5 posterior: normal 100-photo com manifest publish-time e igualdade byte-exact; repetição debug só se necessária, com JPEGs novos.

Um L4 negativo pode validar o executor/parser, mas não satisfaz o gate de descoberta. Não bloquear o diagnóstico por falta de PASS causal nem converter esse diagnóstico em PASS de produto.

Fazer primeiro a injecção de flood/ENOSPC com produtores sintéticos, não com serviços NG. Cada cenário real de falha também exige reinício completo.

Para L5, Hermes deve verificar os pressupostos pendentes do gerador/staging da SPEC-052. Incluir fixture de recepção com corrupção única e exactamente um mismatch, preservando os originais.

Telemetria: progresso por contadores concluídos; perdas por deltas entre amostras compatíveis, nunca CPU ou elapsed textual.

## 9. Matriz de aceitação

| ID | Critério | Artefacto reproduzível |
|---|---|---|
| A01 | Inventário cobre árvore/sinks/guardas e declara lacunas | `inventory.json`, `logs.jsonl`, `coverage.json`, snapshot hash |
| A02 | Presença textual separada de actividade efectiva | Fixtures pré-processador e `macro-effective.jsonl` |
| A03 | DEBUG activos NRNCS auditados | Entradas de `NRInfoPayload01.cpp`/`NRPubNotify01.cpp`, comandos reais |
| A04 | Perfis exactos sem debug global/broad | Receitas, allowlist, testes negativos e compile commands |
| A05 | Builds isolados e rastreáveis | Manifests normal/targeted/verbose, hashes e relatório rebuild |
| A06 | Escada limita papéis e probes | Planos efectivos L0–L5 e testes de combinação inválida |
| A07 | Marker pertence ao processo/trial correcto | Fixtures stale/PID/VM/boot e referências de bytes |
| A08 | Buffering não vira FAIL sem evidência negativa | Testes `INCONCLUSIVE` e bloqueio de dependentes |
| A09 | Sem perda silenciosa nem flood em `/tmp` | Relatório quotas, filesystem, segmentos e teste overflow |
| A10 | Teardown continua seguro sob falha do logger | Trial sintético, journal e inventários IPC |
| A11 | L1 SHM delimitado; L2 receptor demonstrado | Assessments, identidades, logs/probes e pcap |
| A12 | L3 distingue invocation de readback | Tuplos correlacionados e subgates separados |
| A13 | L4 documenta descoberta/readback ou lacuna | Assessment sem Source, probe e limitações |
| A14 | L5 normal byte-exact posterior | Manifest publish-time, três mapas e comparação 100/100 |
| A15 | Diagnóstico não substitui aceitação | Assessments com classe de evidência e perfil explícitos |
| A16 | Operação neutra e produção intacta | README revisto, auditoria de segredos e diff restrito a tooling/docs |

A aceitação da infraestrutura exige A01–A13 e A15–A16, incluindo tratamento correcto de diagnóstico negativo. A14 é gate adicional antes de usar esta infraestrutura como harness funcional aceite. Nenhum destes critérios fecha automaticamente a SPEC-051 ou a release.

## 10. README, migração e revisão

Acrescentar documentação operacional site-neutral:

1. Dependências existentes, Python stdlib, compilador, CMake, GDB/tcpdump opcionais conforme gates.
2. Configuração local ignorada, raízes persistentes de build/evidência e permissões.
3. Inventariar → validar → build → dry-run → preflight → run.
4. Tabela L0–L5 e perfis, com comandos da §7 e placeholders.
5. Diferença entre `normal`, `targeted-debug`, `verbose-diagnostic` e DEBUG activos já no código.
6. Recuperação com `status`, `collect`, `cleanup`, `finalize` da SPEC-052.
7. Troubleshooting: buffer, delay periódico, perfil errado, símbolos ausentes, ENOSPC, captura truncada e marker stale.
8. Proibição de debug global, `/tmp` para logs extensivos e uso simultâneo dos launchers manuais.
9. Limitações de cada gate e separação entre evidência diagnóstica e aceitação.

Implementação aditiva por Hermes após revisão. Preservar alterações existentes da árvore; não modificar C++ nem ficheiros de configuração operacionais partilhados.

Rollback consiste em deixar de seleccionar os perfis/tooling novos após cleanup seguro. Não sobrescrever binários normais, apagar evidência ou reverter produção. Qualquer necessidade de nova emissão C++ ou de remoção de DEBUG interno abre SPEC separada.

## 11. Emendas source-grounded obrigatórias

A revisão do código corrente corrigiu as propostas preliminares desta SPEC. Estas regras têm precedência sobre qualquer tabela ou perfil anterior que as contradiga.

### 11.1 Estatuto do inventário

O inventário inicial é **parcial**, fundamentado apenas nos ficheiros auditados. Não pode declarar cobertura completa da árvore enquanto todos os fontes/headers incluídos no CMake, sinks próprios, wrappers de logging, `STATISTICS`, includes e ramos indirectos não forem auditados. Cada entrada deve distinguir `declared_active`, `declared_commented`, `guarded`, `effective`, `compiled`, `runtime_reachable` e `observed`. Código depois de `break` fica `unreachable`; ausência de observação fica `NOT_OBSERVED`, nunca “inexistente”.

A auditoria deve incluir, como fontes adicionais mínimas, `Common/src/{Block,Process,Message,File,Prompt_iostream,OutputVariable,NameGenerator,NGAL_CS}.{h,cpp}`, `Common/src/NGAL_SAR.h`, `Common/src/MessageBuilder.{h,cpp}`, `Common/src/NGRuntimeProfile.h`, `Common/src/GWRunInitialization01.{h,cpp}`, `Common/src/HT.{h,cpp}`, `Common/src/HTStoreBind01.cpp`, `HTGetBind01.cpp`, `HTBindReport01.cpp`, `HTDeliveryBind01.cpp`, `HTListBind01.cpp`, além de `ContentApp/src/CoreRunInitialize01.cpp`, `CoreNotifyS01.cpp`, `ContentApp.cpp`, `execContentApp.cpp`, `Core.h`, `PGCS/src/PGRunExposition01.cpp`, `PGRunPublishing01.cpp`, `PGCS.cpp`, `execPGCS.cpp`, `PG.h`, `PGCS.h`, `NRNCS/src/NR.cpp`, `NR.h` e `execNRNCS.cpp`, ou uma exclusão explícita e justificada.

### 11.2 Perfis corrigidos

- `obs-normal`: nenhum define adicional; pode ainda conter `DEBUG` activo no código e logs incondicionais.
- `obs-gw-shm`: **não activa `Common/src/GW.cpp:DEBUG`**. Usa logs incondicionais auditados, `/proc`, inventários IPC e probes externos.
- `obs-hello`: separa hello IHC de exposition IPC e selecciona apenas a unidade realmente exercitada.
- `obs-raw-sar`: diagnóstico curto, sem stress/fotos e nunca aceitação.
- `obs-nrncs-binding` e `obs-core-control`: ficam bloqueados até os sinks e efeitos transitivos serem auditados.
- `obs-payload-cache`: não transforma logs de cache em prova de integridade; exige mapas externos de SHA-256.
- `obs-verbose-window`: não pode incluir `Common/src/GW.cpp:DEBUG`, `PGCS/src/PG.cpp` amplo, `DEBUG2/DEBUG3/DEBUG5/DEBUG6`, `DEBUG_NETWORK_QUEUE` ou qualquer sink proibido.

É proibido inicialmente activar broad debug em `PGCS/src/PG.cpp`, `Common/src/Process.cpp`, `Block.cpp`, `Message.cpp`, e os níveis `DEBUG2`, `DEBUG3`, `DEBUG5`, `DEBUG6`. `DEBUG1` não é activado implicitamente. `Common/src/GW.cpp:DEBUG` é proibido por efeitos de estado e acessos concorrentes fora dos locks.

`NRNCS/src/NRInfoPayload01.cpp` e `NRPubNotify01.cpp` têm definições `DEBUG` activas no snapshot analisado; o baseline normal deve auditá-las e medir volume, não assumir que o logging está desligado.

### 11.3 Força dos markers

Os textos `RECEIVER_HANDLER_ENTRY`, `RECEIVER_HANDLER_STATUS_OK`, `NRPUB_INGRESS` e `HT_STORE_CALL` não são markers da aplicação nos fontes auditados; só podem ser usados se os probes GDB/externos que os produzem forem preservados como artefactos `source_kind=gdb`/`external`.

- `sendto` positivo, `SAR COMPLETE`, `Delivered to GW` e `NETWORK_QUEUE_DELIVERED` demonstram apenas o estágio correspondente, não aceitação final.
- `OPERATIONAL: Everything ok!` demonstra a transição local de estado, não persistência.
- `Discovered a NRNCS!` só fecha descoberta quando HID/OSID/PID/BID estão correlacionados ao trial.
- `HT_STORE_CALL` demonstra invocação, não readback/persistência.
- Logs de publicação, cache hit, payload recebido e contadores não substituem mapas externos de ficheiros e SHA-256.

### 11.4 Build targeted

O CMake corrente não fornece perfis por unidade. Um compiler launcher por caminho completo só pode ser usado em build isolado depois de validar a invocação real, a propagação para `Common` e os efeitos transitivos. Se essa validação falhar, o perfil targeted fica `REJECTED`; é proibido substituí-lo por `-DDEBUG` global.

### 11.5 L1–L5 corrigidos

L1 separa obtenção de SHMID, criação inédita, attach e troca; não exige hello IPC quando a topologia não o exercita. L2 separa socket, frame, reassembly, entrega ao GW e handler. L3 separa estado operacional, descoberta, publicação, store e readback. L4 exige lookup/readback independente. L5 usa `obs-normal` para aceitação e mapas externos para a integridade.

As quotas agregam stdout, stderr e sinks internos; logs extensivos ficam fora de `/tmp` tmpfs. Um marker ausente ou buffered gera `NOT_OBSERVED`/`INCONCLUSIVE`, não FAIL automático. Um erro explícito, hash divergente ou cleanup inseguro pode gerar FAIL independentemente.

