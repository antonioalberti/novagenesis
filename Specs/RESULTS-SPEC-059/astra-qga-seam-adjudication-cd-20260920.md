# Astra Adjudication — refined QGA-loss seam C+D

**Data:** 2026-09-20  \  
**Modelo:** `gpt-6-astra`  \  
**Branch:** `AIOPT3`  \  
**Baseline reviewed:** `9dba393`  \  
**Scope:** adjudicação da arquitectura antes de implementação.

## 1. Veredicto: **CONDITIONAL GO**

**GO apenas para um primeiro incremento controller-only de contratos, normalização, fixtures e wrapper fake. HOLD para ligação de produção e para alteração do ciclo de vida do controller. NO-GO para qualquer trial real nesta fase.**

Esta adjudicação baseia-se nos resultados das auditorias descritos na pergunta; não constitui uma nova inspecção do HEAD `9dba393` nem aprovação de um diff ainda inexistente.

O refinamento resolve a ambiguidade principal da opção A: **persistir evidência e decidir aceitação no host não equivale a ter autoridade para finalizar o guest**. Continua, porém, por demonstrar o mecanismo que permite ao controller guest cumprir a sua obrigação terminal quando a sessão QGA desaparece.

## 2. Baseline: **C+D**, com autoridade de teardown exclusivamente guest-local

C+D deve substituir A como baseline de implementação. A pode permanecer no histórico como motivação — fronteira host-side e evidência durável — mas não como segunda formulação normativa concorrente.

A separação normativa deve ser:

| Responsabilidade | Autoridade |
|---|---|
| Executar, interromper e finalizar o trabalho guest | Controller guest, com journal/lease/fencing e deadline próprios |
| Preservar evidência de transporte | Wrapper host-side |
| Consultar completion já publicado | Wrapper host-side, read-only e bounded |
| Decidir se as evidências permitem aceitação | Verificador host-side |
| Executar cleanup guest por inferência do host | **Não autorizado** |

**C** é a fronteira host-side de execução limitada e reconciliação. **D** é o contrato tipado que atravessa essa fronteira; não é um callback nem uma nova autoridade de lifecycle.

A obrigação durável reparte-se em duas:

- **Guest:** garantir responsabilidade local pelo lifecycle e pela finalização.
- **Host:** preservar a evidência recebida e nunca aceitar resultados sem justificação verificável.

Se A significava “o host consegue recuperar/finalizar o guest”, essa parte deve ser explicitamente retirada. Se significava apenas “o host mantém um registo durável de operação e decide aceitação”, essa propriedade é conservada em C+D.

**Ressalva importante:** invocar o canonical controller via QGA não prova, por si só, independência da sessão QGA. Essa independência é um requisito de implementação e teste, não uma consequência do desenho do wrapper.

## 3. Contrato mínimo por módulo

Os nomes abaixo são uma proposta de API normativa; não pressupõem interfaces já existentes no repositório.

### `ng_remote_executor.py`: transporte, não adjudicação

Manter aqui:

- `QGAChannel.exec()` e os tipos/erros de transporte associados;
- captura dos dados que o transporte realmente conhece;
- preservação das distinções entre resposta completa, resposta parcial e falha sem resposta.

Não colocar aqui:

- decisão de aceitação;
- interpretação de manifest/seal;
- retries de execução;
- lógica de teardown;
- descoberta do controller;
- hooks em `run_local_trial`, `local_observe`, readiness ou oracle.

**Não é necessário alterar a assinatura existente de `exec()` no primeiro incremento.** O wrapper deve adaptar o `Mapping` e `QGAChannelError` actuais, sem exigir que os callers existentes adoptem imediatamente um novo envelope.

Se faltarem campos no transporte, representá-los como desconhecidos; não os reconstruir a partir de heurísticas.

### Novo `ng_qga_execution_boundary.py`: wrapper e política bounded

API pública mínima:

```python
class QGAExecutionBoundary:
    def execute_once(
        self, request: ControllerRequest
    ) -> BoundaryResult: ...

    def reconcile_once(
        self, request: ReconciliationRequest
    ) -> BoundaryResult: ...
```

Semântica obrigatória:

- `execute_once` permite **uma tentativa de dispatch**, sem retry de `run`;
- usa apenas o canonical controller e uma identidade previamente fixada;
- não aceita um comando shell arbitrário;
- normaliza retorno e excepção antes de interpretar observações;
- persiste a evidência normalizada/raw bounded-redacted antes de reconciliar ou aceitar;
- uma tentativa de dispatch que termina em perda continua a contar como tentativa;
- `reconcile_once` consulta apenas a operação exacta e nunca chama `run`, stop, cleanup ou seal.

As duas funções devem partilhar o orçamento da operação: **no máximo uma tentativa de reconciliação**, inclusive se chamada explicitamente depois de uma tentativa automática. Não devem existir dois caminhos com contadores independentes.

A unicidade precisa de estado durável para ser garantida entre processos/restarts. No primeiro incremento esse estado pode ser fake; uma variável em memória não satisfaz o contrato de produção.

### Novo módulo de contrato, por exemplo `ng_qga_completion_contract.py`

Manter a lógica pura de schema, normalização e validação separada do I/O:

```python
def normalize_exec_outcome(
    request: ControllerRequest,
    outcome: Mapping[str, object] | QGAChannelError,
) -> ExecutionEnvelope: ...

def validate_completion(
    expected: CompletionExpectation,
    record: GuestCompletionRecord,
    evidence: CompletionEvidence,
) -> CompletionValidation: ...
```

O `ExecutionEnvelope` deve conter, no mínimo:

- versão do schema;
- `operation_id`, `trial_id`, nonce e request hash;
- controller identity e boot identity esperadas, quando estabelecidas;
- operação, phase/sequence e deadline;
- classificação do transporte;
- `transport_returncode: int | None`;
- `guest_outcome`;
- `guest_exit_code: int | None`;
- `exit_code: int | None`, se necessário por compatibilidade;
- stdout/stderr bounded;
- evidência raw redacted, indicação de truncagem e diagnósticos de normalização.

**Regra para perda de transporte:**

```text
guest_outcome = unknown
guest_exit_code = null
exit_code = null
```

Isto aplica-se ao envelope do evento de perda, mesmo que exista stdout parcial ou um número de retorno do transporte. Uma reconciliação posterior pode estabelecer um resultado guest verificado **num registo de resolução separado**, sem reescrever a perda como sucesso de transporte.

O `GuestCompletionRecord` deve incluir os campos propostos na pergunta e uma referência inequívoca ao result, por exemplo `result_hash`, além de manifest/seal hashes. Deve preservar os verdicts de runtime, teardown e evidence como dimensões distintas.

Pontos normativos adicionais:

- versão desconhecida, campos críticos ausentes ou tipos inválidos: **não aceite**;
- `guest_exit_code = 0` não implica teardown PASS nem aceitação;
- completion terminal pode documentar falha; “terminal” não significa “sucesso”;
- frescura exige correspondência de identidade, operação, nonce, request e geração/boot; timestamp isolado não basta;
- não criar dependência circular de hashes entre completion, manifest e seal;
- a publicação atómica pressupõe leitura limitada, consistente e verificação posterior dos artefactos referenciados.

O resultado de validação deve distinguir **completion válido** de **resultado aceite**. Um completion que confirma runtime FAIL pode ser perfeitamente válido e ainda assim não permitir aceitação.

### Controller guest: produtor do completion, único finalizador

Contrato externo:

1. assumir a operação sob autoridade local durável;
2. executar sob deadline guest-local;
3. realizar:

   `stop → inventory → evidence-preserve → temporary-cleanup → final-verification → terminal-seal reaberto/verificado`;

4. publicar o completion atomicamente apenas após concluir essa cadeia.

Se a cadeia não chegar ao terminal verificável, o journal pode fornecer estado diagnóstico, mas **não pode fazer-se passar por completion aceite**.

Lease/journal/fencing devem proteger também contra duas instâncias guest que tentem finalizar a mesma operação. Um retry host-side não é substituto desse mecanismo.

### Persistência de evidência

`persist_channel_evidence` continua a ser um sink, não um trigger de finalização.

O wrapper precisa de uma operação equivalente a:

```python
persist_envelope(envelope: ExecutionEnvelope) -> EvidenceReceipt
```

O receipt só confirma sucesso depois da persistência exigida pelo contrato, fora de `/tmp`. Falha de persistência bloqueia aceitação e reconciliação subsequente nesse caminho; **não autoriza cleanup alternativo**.

“Raw” significa aqui a representação sanitizada preservada, acompanhada de limites e metadados de redacção/truncagem. Não deve prometer captura integral quando houve limites.

### Vocabulário: substituir `recover`

Usar:

- `reconcile_completion`;
- `read_terminal_completion`;
- `validate_completion`.

Reservar `resume_guest_finalization` ou equivalente para eventual código **interno ao guest**, sob fencing e autoridade local.

Evitar nomes como `recover_guest`, `recover_and_cleanup` ou `recover_trial` no wrapper. A documentação deve dizer expressamente:

> Reconciliação resolve incerteza de observação; não transfere autoridade de execução ou teardown para o host.

## 4. Primeiro incremento bounded autorizado

**Implementar apenas schema/normalizer, terminal completion fixtures, verificador puro e wrapper fake. Não extrair ainda o finalizer inline.**

Escopo autorizado:

1. Contratos versionados e regras explícitas de null/unknown.
2. Normalização comum para `Mapping` e `QGAChannelError`.
3. Fixtures sintéticas de completions válidos e inválidos.
4. Wrapper com dependências fake de transporte, persistência e consulta read-only.
5. Testes RED→GREEN, sem processos reais, VM ou QGA.
6. Documentação normativa mínima e revisão Astra do diff exacto.

A fixture terminal deve representar a pós-condição esperada; **não demonstra que o controller real consegue produzi-la**.

Extrair o finalizer agora aumentaria o escopo e poderia institucionalizar dependências de `Popen/PID/PGID` antes de definir autoridade durável. Uma função reutilizável, por si só, não torna essas dependências recuperáveis.

Essa extracção deve ser um incremento posterior, precedido por contrato explícito dos inputs, estado persistido, fencing, idempotência e tratamento de estado processual indisponível.

### Menor alteração normativa a SPEC-059

Adicionar uma única secção normativa, com precedência expressa sobre formulações anteriores incompatíveis:

> **Fronteira QGA e completion guest — implementação preparatória, sem autorização operacional.**  
> A integração segue C+D: adapter/wrapper host-side bounded e handoff tipado versionado. QGA não integra `run_local_trial`, `local_observe`, readiness, oracle ou teardown. A autoridade de execução e finalização permanece exclusivamente no canonical controller guest, com lifecycle independente da sessão QGA.  
> Retornos `Mapping` e `QGAChannelError` convergem num envelope canónico antes da interpretação de observações. A evidência bounded/redacted é persistida duravelmente fora de `/tmp` antes de reconciliação. Perda implica outcome e exitcodes guest desconhecidos; não permite retry de `run`, cleanup host-side nem inferência de teardown PASS.  
> É permitida no máximo uma reconciliação read-only bounded por operação. A aceitação exige completion fresco e matching, publicado após a cadeia terminal, com result/manifest/seal/verificação concordantes e evidência de Source e Repository independente. Perda não reconciliada impede aceitação.  
> Nesta fase autorizam-se somente contratos, fixtures, fakes e testes controller-only. Qualquer ligação de produção ou trial exige revisão Astra exacta e autorização posterior explícita.

Não é necessário reabrir o restante desenho nem introduzir C++, protocolo, payload Source→Repository ou modo remoto.

## 5. Riscos residuais e testes mínimos

| Risco | Teste mínimo |
|---|---|
| Partial-output interpretado antes da normalização | `Mapping` parcial e erro equivalente produzem a mesma decisão conservadora |
| No-output segue um caminho diferente | Ambos convergem antes de qualquer leitura de campos de observação |
| Confusão de exitcodes | Transporte `0` com guest desconhecido permanece não aceite |
| Perda seguida de retry involuntário | Fake verifica um único dispatch, incluindo falha após dispatch |
| Reconciliação duplicada | Caminhos automático e explícito partilham uma única tentativa |
| Reconciliação antes de persistência | Trace fake exige persistência concluída antes de consulta |
| Persistência falha | Nenhuma aceitação nem consulta posterior |
| Completion stale ou de outra operação | Rejeitar nonce/request/controller/boot/phase incompatíveis |
| Completion inconsistente | Rejeitar divergências entre record, result, manifest, seal e verifier |
| Truncagem ou input malformado | Limites aplicados sem converter conteúdo incompleto em sucesso |
| Zero IPC usado como prova de teardown | Zero IPC externo nunca promove teardown para PASS |
| Evidências colapsadas | Source e Repository continuam avaliados separadamente |
| Sucesso terminal confundido com execução bem-sucedida | Completion válido com runtime FAIL continua não aceite |

Também são necessários testes de estados UNKNOWN/INCONCLUSIVE/INCOMPLETE com critérios documentados, não escolhidos conforme o caminho de excepção.

**Ainda não demonstrado por este incremento:** independência real da sessão, deadline guest-local, fencing entre instâncias, continuidade após perda e durabilidade efectiva da publicação. Esses pontos permanecem gates de implementação posteriores.

## 6. Condições para eventual trial real

Um eventual trial exige, cumulativamente:

1. Primeiro incremento controller-only RED→GREEN e revisão Astra exacta do diff/HEAD.
2. Desenho e implementação revistos da autoridade guest-local, incluindo journal, lease/fencing e deadline independente de QGA.
3. Finalizer desacoplado de pressupostos exclusivamente em memória, sem inventar PID/PGID nem reconstruir autoridade a partir de stdout.
4. Testes controller-only de perda de sessão, restart do wrapper, dispatch ambíguo, instância concorrente, completion stale e falhas em cada fase terminal.
5. Evidência de que o wrapper não cria retries, callbacks de observação, cleanup host-side ou outro caminho de aceitação.
6. Plano separado para validar a cadeia real host→guest, com limites e critérios de paragem, previamente revisto.
7. **Autorização explícita posterior para o trial concreto.**

Até essa autorização permanecem vedados trial, VM, QGA, `clean.sh`, reboot, push, tag e release. **Este CONDITIONAL GO não autoriza nenhuma dessas acções.**
