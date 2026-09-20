# Astra Review — bounded QGA-loss reconciliation seam

**Data:** 2026-09-20  \  
**Modelo:** `gpt-6-astra`  \  
**Branch:** `AIOPT3`  \  
**HEAD reviewed:** `6c03bc380ee5c596b7c2c7a3e282e98c4a049fb7`  \  
**Scope:** definição arquitectural antes de qualquer implementação ou trial real.

## Veredicto: **NO-GO para implementação do seam de produção no estado actual**

**Não autoriza trial real.** Esta revisão baseia-se no estado fornecido; não pressupõe inspecção adicional do repositório nem execução de comandos.

A lacuna não é apenas encaminhar `QGAChannelError` para um `finally`: falta uma **autoridade de recuperação durável, local ao guest e independente da sessão QGA**, capaz de determinar ownership e concluir o teardown quando o transporte desaparece.

**Recomendo A como desenho-alvo**, sujeito à alteração mínima de SPEC descrita abaixo. Sem essa capacidade, A não cumpre o contrato; B não a substitui. A ausência dessa autoridade deve produzir **RED**, não activar um fallback baseado em inferência.

### Opção recomendada e opções rejeitadas

| Opção | Decisão | Fundamentação |
|---|---|---|
| **A — `QGAExecutionBoundary` host-side** | **Único desenho-alvo recomendado, actualmente bloqueado** | Corresponde ao papel documentado de QGA: invocar o canonical controller dentro do guest. Mantém execução e teardown dos recursos sob autoridade local ao guest. Exige recovery independente da sessão e handoff durável. |
| **B — evento interno em `run_local_trial`** | Rejeitada como seam de produção | Um sink resolve encaminhamento de evidência, mas não torna um evento host-side acessível ao controller guest depois de perder QGA. Injectar QGA na observação local reproduz a limitação do probe revertido, salvo existência de outro backend real explicitamente autorizado. |
| **C — QGA por role** | Rejeitada | Fragmenta ownership, acopla roles ao transporte e deixa o host sem autoridade local verificável para reconciliar processos/IPC guest. |
| **D — melhorar except + monkeypatch** | Rejeitada | Pode ser teste unitário de robustez, nunca prova do caminho de produção. |

A implementação de A pode usar tipos internos de observação, mas **isso não transforma B num segundo seam**: a fronteira de execução continua a ser host → canonical controller guest.

## Contrato mínimo exacto

Os nomes seguintes são **proposta de API**, não funcionalidades existentes.

### 1. Módulos e entradas

**Criar `Scripts/AlpineVMs/ng_qga_execution_boundary.py`**

```text
QGAExecutionBoundary.execute(request) -> BoundaryResult
QGAExecutionBoundary.recover(operation_id, trial_id) -> BoundaryResult
normalize_qga_observation(request, returned_value=None, error=None)
persist_boundary_observation(observation)
```

Responsabilidades:

- invocar exclusivamente o canonical controller;
- consumir **todos** os retornos e erros de `QGAChannel.exec`;
- persistir observações host-side;
- consultar/solicitar recuperação da mesma operação, nunca relançar silenciosamente;
- produzir um bundle host-side que referencia o bundle guest sem o modificar.

**Criar `Scripts/AlpineVMs/ng_controller_recovery.py`**

```text
OperationJournal
OwnershipLease
RecoveryAuthority
reconcile_operation(operation_id, trial_id, budget) -> RecoveryReport
```

Responsabilidades:

- journal e lease duráveis;
- exclusão mútua e fencing;
- recuperação local do finalizer;
- checkpoints idempotentes.

**Alterar `Scripts/AlpineVMs/ng_remote_executor.py`**

```text
run_local_trial(..., operation_context)
finalize_local_operation(operation_context, cause) -> FinalizationReport
```

Adicionar uma entrada **local**, proposta:

```text
recover --operation-id ... --trial-id ... --budget-seconds ...
```

Esta entrada não altera o modo remoto, o payload Source→Repository ou o protocolo C++. Contudo, **não basta existir uma CLI `recover` invocável apenas por QGA**: tem de existir uma autoridade local que a execute quando QGA não estiver disponível.

**Alterar `proxmox_qga_channel.py` apenas no necessário para normalização/persistência uniforme**, preservando a distinção entre transporte e resultado guest. O wrapper deve também tratar as duas formas actuais: envelope retornado e excepção.

### 2. Pedido de execução

Campos obrigatórios:

```text
schema_version
operation_id
trial_id
action                    # preflight | run | recover | status
request_sha256
backend                   # proxmox-qga
controller_identity       # versão/hash esperado
recovery_authority_id
execution_deadline
recovery_budget_seconds
```

`operation_id` é criado e persistido antes da primeira chamada. Repetir o mesmo ID com `request_sha256` diferente é conflito fatal.

### 3. Observação normalizada

Todas as chaves abaixo são obrigatórias; valores desconhecidos são `null`, nunca estimativas:

```text
schema_version
operation_id
trial_id
phase
sequence
backend
producer_id
provenance                # host-transport | guest-controller
transport_exit_code
guest_exit_code
exit_code
guest_outcome             # unknown | success | failure
failure_class
raw_transport
raw_artifact
raw_sha256
```

Regras:

- `sequence` é monotónico **por producer e operação**, não uma falsa ordenação global host/guest.
- `exit_code` neste envelope é o código guest confirmado, não o exit code do wrapper.
- `transport_exit_code == 0` **não implica** `guest_exit_code == 0`.
- Em perda de transporte:

```text
guest_exit_code = null
exit_code = null
guest_outcome = unknown
failure_class = transport-loss
```

- Timeout sem código de transporte confirmado mantém `transport_exit_code = null`.
- `raw_transport` preserva o material capturado, incluindo output parcial ou ausência explícita de output.
- `raw_artifact` identifica caminho relativo, tamanho, encoding e hash do artefacto persistido.
- Output parcial nunca é interpretado como completion, mesmo que contenha texto semelhante a um resultado válido.

O `BoundaryResult` usa **`boundary_exit_code` separado**, evitando sobrecarga de `exit_code`.

### 4. Convergência de partial-output e no-output

O caminho obrigatório do wrapper é:

```text
QGAChannel.exec
    → retorno OU QGAChannelError
    → normalize_qga_observation
    → persist raw
    → persist envelope
    → classificar transporte
    → solicitar/aguardar recovery da operação existente
    → finalizar resultado host-side
```

Não pode existir um caminho “envelope retornado → retorno directo ao chamador”.

**Distinção essencial:** ambas as formas chegam à mesma decisão de recuperação no host. Não se pode prometer que chegam imediatamente ao `finally` guest durante uma interrupção de transporte. O finalizer guest é alcançado pela autoridade local, por cancelamento durável ou por expiração de lease/deadline.

### 5. Handoff, lease e ownership

Antes de lançar qualquer role, o guest tem de persistir:

- IDs da operação e hash do pedido;
- identidade do host/guest e `boot_id`;
- identidade do controller, incluindo PID e start time;
- identidade da autoridade de recuperação;
- geração/fencing token;
- deadline local baseada em relógio monotónico;
- estado do lifecycle;
- escopo exclusivo de recursos da operação;
- identidade dos processos e IPC criados;
- checkpoints de teardown e de evidência.

**Há uma janela crítica entre criar um processo e registar o PID.** Um journal de PIDs, sozinho, não chega. É necessário um escopo exclusivo recuperável, por exemplo um cgroup por operação, ou mecanismo equivalente cuja completude seja demonstrada. O mesmo requisito aplica-se ao ownership de IPC.

Sem esse mecanismo, a operação não pode lançar: **RED antes de launch**.

Estados de arranque distintos:

```text
NOT_STARTED_CONFIRMED
STARTED
START_STATUS_UNKNOWN
```

Timeout, ausência de resposta ou journal inacessível implicam `START_STATUS_UNKNOWN`. Só a autoridade guest, com registo durável e exclusão de arranque tardio, pode confirmar `NOT_STARTED_CONFIRMED`.

### 6. Recovery bounded e idempotente

Proposta normativa de limites, a validar na SPEC:

- execução com deadline finita obrigatória;
- após perda detectada: budget de recuperação de **60 s**;
- máximo de **3** tentativas de `status/recover`;
- cada tentativa: timeout até **10 s**, limitado pelo budget restante;
- backoff de **1 s e 2 s**;
- **zero retries automáticos de `run`**;
- recuperação local inicia até **5 s** após deadline/lease expirada;
- finalizer local com budget próprio de **60 s**.

A indisponibilidade de QGA impede recolha pelo host, mas não deve impedir a recuperação local.

Idempotência exige:

- lock exclusivo e fencing antes de efeitos destrutivos;
- mesmo pedido → mesma operação;
- checkpoints duráveis por etapa;
- recursos já ausentes → verificar ausência, não repetir comandos cegamente;
- identidade divergente → não matar/remover; marcar teardown não verificado;
- seal existente válido → devolver o mesmo seal;
- seal existente incompatível → falha de integridade, sem sobrescrita.

Se controller e recovery disputarem ownership, apenas um pode actuar. **Expirar um ficheiro lease sem impedir o owner antigo de continuar a produzir efeitos não constitui fencing.**

## Invariantes de segurança

### Resultados

| Campo | Condições |
|---|---|
| `runtime_result=PASS` | Completion guest explícita, validação funcional completa e nenhuma perda de transporte nesta operação. |
| `runtime_result=INCONCLUSIVE` | Perda de transporte ou completion desconhecida, salvo FAIL já demonstrado. |
| `runtime_result=FAIL` | Falha funcional/runtime confirmada; permanece sticky. |
| `teardown_result=PASS` | Ausência verificada de processos/IPC pertencentes à operação e inventário completo. |
| `teardown_result=INCONCLUSIVE` | Estado guest inacessível, ownership insuficiente ou inventário incompleto. |
| `teardown_result=FAIL` | Resíduos confirmados ou falha confirmada de teardown. |
| `evidence_result=PASS` | Bundle completo, provenance válida, hashes, manifest e seal verificáveis. Pode documentar um runtime inconclusivo. |
| `evidence_result=INCONCLUSIVE/FAIL` | Incompleto/inacessível, ou corrupção/incompatibilidade confirmada, respectivamente. |

Proposta para o **wrapper**, sem redefinir códigos existentes do canonical controller:

```text
boundary_exit_code = 0  # aceitação integral
boundary_exit_code = 1  # falha confirmada
boundary_exit_code = 2  # inconclusivo/autoridade ausente/recuperação não verificada
```

Perda de transporte nunca produz `boundary_exit_code=0`.

Um resultado guest anterior, mesmo `PASS`, pode ser preservado como declaração guest verificada. **Não promove a operação host afectada por perda a PASS ou aceite.**

Source/Repository só podem aparecer como evidência explicitamente observada. Não se inferem execução, entrega, completion ou aceitação a partir de launch, readiness, output parcial ou transporte bem-sucedido.

### Ordem exacta de persistência e sealing

Em cada produtor, sem inventar atomicidade distribuída:

1. Persistir journal/pedido antes dos efeitos correspondentes.
2. Ao observar perda, persistir raw atomicamente, com sincronização durável.
3. Persistir envelope referenciando esse raw e actualizar o estado sticky.
4. Executar `stop`.
5. Reconciliar e inventariar processos/IPC.
6. Preservar artefactos e escrever `preservation.json` como checkpoint durável.
7. Fazer temporary cleanup **apenas** dos recursos temporários autorizados; nunca da única cópia da evidência.
8. Executar inventário/verificação final após cleanup.
9. Escrever o conteúdo definitivo de `result.json`, ainda não aceite.
10. Escrever manifest dos artefactos finais.
11. Executar offline verification e persistir relatório.
12. Escrever terminal seal que vincula hashes de manifest, resultado e relatório de verificação.
13. Validar o seal; só então publicar o estado terminal/decisão de aceitação.

Para evitar ciclos de hash, o manifest não inclui o seu próprio hash, o relatório que o verifica ou o seal; o seal vincula esses objectos explicitamente. Nada coberto é reescrito depois do seal.

O host recolhe o bundle guest imutável e cria **um bundle host separado**, com o seu raw QGA e referência ao seal guest. Não acrescenta ficheiros a um bundle guest já selado.

## Matriz de falhas por fase

| Fase da perda | Tratamento obrigatório | Limite de conclusão |
|---|---|---|
| Antes de launch | Registar perda; consultar journal/autoridade; impedir reenvio de `run`. | Sem confirmação guest: arranque desconhecido, não “nunca iniciou”. |
| Durante launch | Recuperar escopo de recursos, incluindo janela anterior ao registo de PIDs. | Sem ownership completo: teardown não PASS. |
| Readiness/observe | Runtime desconhecido/sticky; autoridade local termina operação dentro da deadline. | Não inferir completion de output parcial. |
| Durante stop | Retomar finalizer por checkpoint e verificar identidade antes de cada efeito. | Pedido de stop não prova ausência. |
| Inventory/preservation/cleanup | Retomar apenas etapas pendentes; preservar evidência já durável. | Inventário incompleto ou checkpoint ausente bloqueia aceitação. |
| Manifest/verification/seal | Não regenerar objectos incompatíveis; validar artefactos existentes. | Seal ausente/inválido bloqueia aceitação. |
| Depois de seal guest | Preservar seal guest; registar perda em bundle host separado. | Seal guest não apaga perda host nem autoriza PASS da operação host. |
| QGA permanece indisponível | Autoridade local actua independentemente; host termina no budget. | Host reporta recuperação não verificada até obter evidência; nunca espera indefinidamente. |

## Testes mínimos RED→GREEN, controller-only

Os testes devem chamar **`QGAExecutionBoundary.execute/recover`**, com runner QGA fake e autoridade local fixture. Não monkeypatchar `local_observe` como prova de produção.

1. **Partial-output:** retorno realista do runner normalizado, raw persistido, recovery chamado, aceitação bloqueada.
2. **No-output:** `QGAChannelError(raw_transport=...)`; mesmos passos e preservação integral do raw.
3. **Transporte zero:** completion guest ausente mantém códigos guest `null`.
4. **Perda por fase:** antes de launch, janela launch/journal, readiness, observe, stop e pós-seal.
5. **Arranque ambíguo:** timeout após pedido aceite não causa segundo `run`.
6. **Owner continua vivo:** fixture mantém controller activo após morte da sessão; fencing impede dois finalizers concorrentes.
7. **Crash checkpoints:** interrupções entre raw/envelope, preservation/cleanup e manifest/seal; retomada sem perda nem seal divergente.
8. **Identidade divergente:** PID reutilizado ou IPC sem ownership não é removido; teardown fica RED.
9. **Retry bounded:** relógio fake prova limites de tentativas e tempo.
10. **Provenance:** bundle guest adulterado, operação divergente ou hash incompatível bloqueiam verificação.
11. **Sticky:** completion tardia ou seal guest PASS não eliminam o estado de perda host.
12. **Autoridade ausente:** falha antes de qualquer launch e sem fallback para B/D.
13. **Source/Repository:** output parcial contendo marcadores plausíveis não produz aceitação.

Podem usar processos locais descartáveis de teste, sem VM, QGA real, `clean.sh` ou quaisquer operações proibidas. Fixtures demonstram a máquina de estados; **não demonstram por si a independência real da autoridade guest**.

## Bloqueios e condições de saída

### Alteração mínima de SPEC necessária

Acrescentar a SPEC-059, com referência normativa à SPEC-056:

> Execuções do canonical controller iniciadas por transporte desacoplável só podem lançar recursos depois de estabelecer journal durável, ownership recuperável, deadline finita e autoridade local de finalização independente desse transporte. Repetições usam identidade idempotente, nunca relançamento implícito. Ausência dessa autoridade bloqueia launch. Perda de transporte permanece não aceitável, mesmo quando um bundle guest válido é posteriormente recuperado.

A mesma alteração deve fixar:

- mecanismo concreto de recuperação independente e fencing;
- cobertura da janela spawn/journal e ownership de IPC;
- budgets e códigos do wrapper;
- relação entre bundles host/guest;
- convenção acíclica de manifest/verificação/seal.

**Este é o menor requisito novo indispensável; um sink ou mais um `except` não o substitui.**

### Condições para sair deste NO-GO

1. Aprovar a alteração normativa.
2. Escolher e rever o mecanismo concreto de autoridade local e ownership.
3. Confirmar que a solução permanece controller-only, sem alterar os componentes proibidos.
4. Rever o desenho exacto actualizado antes de implementação.
5. Depois de autorizada a implementação, passar os testes controller-only e a verificação offline dos bundles.

Só então pode ser solicitada **nova revisão para eventual trial real**, com plano e autorização próprios. **Esta revisão não autoriza esse trial.**
