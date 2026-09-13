# Review Astra — SPEC-033

## Veredicto: **APROVO COM AMENDAS**

A direção está correta, mas **a SPEC ainda não está pronta para execução na ordem A → B → C → D → E**. As principais emendas são:

1. Antecipar a auditoria C para antes da mudança de identidade/lifetime em B.
2. Usar **handles com geração**, não apenas índices, e distinguir validação de identidade de retenção de lifetime.
3. Estabelecer **Process como proprietário único** das mensagens; filas, Actions e Script mantêm retenções, não ownership concorrente.
4. Corrigir a contradição de atomicidade em A.3 e trazer os leitores com `Line[4096]` para o escopo de A.
5. Escolher **`n2` como marcador de codificação**, preservando a `Version` semântica.
6. Separar compatibilidade byte-exact legada de identidade/hash entre codificações.
7. Completar os contratos de falha, cópia, buffers e liberação — os gates de throughput não demonstram segurança de lifetime.

**Limite desta revisão:** o review anterior e o Script não foram fornecidos integralmente. O trecho de Process também não contém os corpos de `NewMessage`. Portanto, separo abaixo fatos visíveis no código de achados reportados pela SPEC e de verificações ainda necessárias.

---

## 1. Problema central: B já muda lifetime, mas C vem depois

B.3 não é uma troca interna de container. Migrar filas de `Message*` para handles altera:

- identidade de mensagens;
- acesso nos comparadores de prioridade;
- passagem para `Block::Run`;
- retenção por Script e Actions;
- cancelamento, descarte e exclusão;
- comportamento perante reutilização de slots.

Isso exige o inventário de C **antes**, não depois.

### Ordem aprovada

```text
Patches urgentes de segurança
          ↓
C — auditoria e contrato-alvo
          ↓
A — parser e todos os adaptadores de entrada
          ↓
B — container + identidade + retenções
          ↓
E — RAII interno
          ↓
D — ativação do wire novo
```

A pode avançar em paralelo com C. A implementação do leitor dual pode ser preparada antes de E; **não há dependência técnica obrigatória entre D e E**. Coloco a ativação do escritor por último para isolar regressões de wire das de memória.

Patches urgentes não devem esperar o refactor: P2/P3, M4, correções locais de C1/C2 e contratos de ponteiros nulos.

---

## 2. Fase A — aprovada com correções de contrato e cobertura

### 2.1 A.3 contradiz A.1

> “parse no objeto pré-preenchido: limpa argumentos antes”

Se isso limpar `out`, viola a promessa de preservá-lo em erro.

**Substituir por:**

- criar um temporário vazio;
- validar integralmente;
- substituir `out` apenas no sucesso;
- em erro, preservar **todos os campos**, não somente argumentos.

O commit precisa ser seguro também diante de falha de alocação. Testar parse válido sobre objeto preenchido e parse inválido sobre objeto preenchido.

### 2.2 M5 deve ser resolvido em A, não ficar perdido na lista de achados

O parser aceitar 64 KiB não resolve nada se os três leitores de `Message` continuam entregando fragmentos de 4096 bytes.

**Emenda obrigatória:** migrar os leitores externos ao parser para leitura limitada consistente. Linha acima do limite deve causar erro explícito, sem:

- truncar e aceitar um prefixo;
- interpretar o restante como outra command line;
- desalinhar a leitura do payload.

O limite deve ser aplicado **durante a leitura**, não somente após uma alocação sem limite.

### 2.3 Falta fechar a gramática legada

Especificar:

- comportamento de `NoA == 0`, incluindo a forma emitida atualmente sem `[ ]`;
- se argumento com zero elementos é válido;
- tratamento de `s/h/i`, tipo desconhecido e tipo ausente;
- sinal, overflow e zeros à esquerda nas contagens;
- whitespace permitido, CRLF/LF, NUL embutido e terminação da linha;
- validação de `Name`, `Alternative` e `Version`;
- limite agregado de command lines, bytes e payload por mensagem.

**Decisão para tipos legados:** aceitar `s/h/i` como tokens sintáticos legados, sem atribuir semântica nova; rejeitar outros tipos. Fixar isso em testes.

Os limites propostos são razoáveis como ponto inicial, mas precisam ser validados com mensagens reais. Não representam “mesmos inputs válidos”: introduzem restrições novas e intencionais.

### 2.4 P7 não se resolve somente no parser

Uma string com whitespace pode já ter sido dividida em vários tokens. O parser nem sempre consegue recuperar que a intenção original era um único elemento.

**Validar também a produção do wire.** Nesta SPEC:

- não introduzir escaping;
- rejeitar elementos não representáveis, incluindo string vazia se a gramática não a representa;
- aplicar a validação no writer e, quando compatível com o contrato da API, nos setters;
- não emitir parcialmente uma linha antes de descobrir o erro.

### 2.5 Adaptadores precisam de semântica de erro

- `operator>>`: falha de parse deve refletir-se em `failbit`.
- Parser de array: retornar `ERROR`, sem publicar objeto parcial.
- Framing: definir se a unidade consumida é exatamente uma linha.
- Falha ao desserializar uma mensagem: liberar a mensagem criada e impedir enqueue.

**Gate A adicional:** testes de limites exatos e limite + 1, todos os adaptadores, falha de alocação onde viável e erro de parse após `NewMessage`.

---

## 3. Fase B — escolher slots, mas corrigir a proposta de handles

### 3.1 Estrutura aprovada

**`vector<Slot>` + free-list, com handle `{slot, generation}`.**

Cada slot contém, conceitualmente:

```cpp
generation
message          // ownership do Process
retention_count  // ou mecanismo equivalente de retenção explícita
state
```

Prefiro `unique_ptr<Message>` no slot. Isso não exige alterar os buffers internos de `Message` e explicita o dono.

Capacidade:

- configurada na construção;
- default 30000;
- limite duro;
- erro explícito em capacidade esgotada;
- sem mudança dinâmica de política por variável de ambiente durante a execução.

A free-list fornece criação/liberação O(1). A geração impede que um handle antigo identifique outra mensagem após reutilização do slot. Definir também a política contra wraparound; não basta ignorá-lo.

### 3.2 Índice sozinho não fecha C6

Mesmo com lookup O(1):

```text
mensagem A ocupa slot 17
A é destruída
mensagem B ocupa slot 17
fila entrega handle antigo 17
B é executada como se fosse A
```

Esse é um problema de **ABA**. O código atual também pode sofrê-lo se o alocador reutilizar um endereço: `OkToRun` encontra o mesmo ponteiro e considera outra instância válida.

**Não obter a geração desreferenciando um `Message*` possivelmente morto.** O handle deve ser capturado quando a mensagem ainda está viva e armazenado pelo retentor.

### 3.3 Handle válido não é retenção

`resolve(handle)` seguido de uso não protege contra exclusão entre as duas operações.

O contrato aprovado é:

- **Process é o proprietário único.**
- Fila/Script mantêm uma retenção explícita enquanto precisarem da mensagem.
- `Run` mantém uma retenção durante toda a execução.
- `MarkToDelete` solicita exclusão; a destruição ocorre apenas quando não houver retenções.
- Cancelamento de fila libera a retenção sem executar.
- Ponteiros passados às Actions são empréstimos válidos durante o `Run`.

A auditoria deve escolher e documentar a disciplina de execução: confinamento a thread ou sincronização. O código fornecido mostra suporte a threads, mas não demonstra quais operações concorrem.

### 3.4 Não esquecer comparadores e reentrância

Uma `priority_queue<Message*>` pode desreferenciar uma mensagem **antes de chegar a `Block::Run`**, no comparador. Mover o guard em `Block::Run` não protege isso.

A entrada de fila deve carregar chave estável de ordenação, por exemplo:

```text
due_time + sequence + retained_handle
```

Não alterar a chave de um item já enfileirado sem removê-lo/reinseri-lo.

Auditar também Actions que:

- marcam ou tentam deletar a mensagem recebida;
- alteram/removem command lines durante a iteração;
- chamam outro `Run`;
- retornam a mesma mensagem como resposta inline ou agendada.

A retenção de `Message` não garante, por si só, que `PCL` continue vivo.

### 3.5 C1 precisa de correção precisa, não de duas semânticas possíveis

Há duas sobrecargas:

- a de saída por referência, aparentemente com índice `int`, testa `_Index < NoM`;
- a que retorna ponteiro usa `_Index < MAX_MESSAGES_IN_MEMORY`.

**Preservar índice físico de slot nas APIs existentes.** Não reinterpretá-lo como “enésimo slot ocupado”.

Na primeira:

- inicializar `M = nullptr`;
- rejeitar índice negativo;
- testar capacidade e ocupação;
- retornar `ERROR` para slot inválido/vazio.

A outra já faz o teste de capacidade, embora deva acompanhar o contrato documentado. Criar uma API separada de iteração de mensagens vivas.

### 3.6 Free-list não elimina todas as varreduras

`DeleteMarkedMessages`, marcação por tempo e inspeção continuam potencialmente lineares.

A SPEC deve distinguir:

- lookup/alocação/liberação O(1);
- varreduras de manutenção;
- eventual fila de candidatos à exclusão.

Para a exclusão marcada, recomendo fila de candidatos deduplicada, com revalidação das retenções. A marcação por tempo pode continuar por varredura inicialmente, desde que medida.

**Gate B adicional:** reutilização de slot/endereço, handle obsoleto, cancelamento, exclusão durante `Run`, enqueue duplicado, capacidade cheia, falhas de criação e teardown com filas pendentes. Medir latência e ocupação, não só throughput.

---

## 4. Fase C — precisa descrever o real e o alvo separadamente

### 4.1 O fluxo proposto não cobre os caminhos reais

`ListBindings()` cria uma mensagem e chama `Block::Run` diretamente, sem GW enqueue/pop.

Logo, o documento deve incluir pelo menos:

- entrada de rede;
- execução local direta;
- resposta inline;
- mensagens agendadas;
- erro antes de enqueue;
- cancelamento;
- shutdown.

**Adicionar explicitamente `InlineResponseMessage` ao inventário.** O parâmetro por referência permite publicar/substituir ponteiros; documentar quem retém o valor anterior e o novo, se podem ser aliases da entrada e quem os libera.

### 4.2 C.3 e invariantes 1–2 são incompatíveis

A SPEC atribui ownership a GW, Block/Action e Script, mas também mantém o container responsável pela destruição.

Isso cria uma transferência de propriedade que o código não implementa.

**Decisão:** propriedade centralizada no Process; transferem-se retenções, não ownership. O documento de C deve separar “situação atual” de “contrato-alvo implementado em B”.

### 4.3 Corrigir os bloqueios de C.2

- **Usar cardinalidade não bloqueia D:** a cardinalidade será preservada.
- Interpretar wire diretamente exige adaptar esse consumidor interno antes de ativar o escritor novo.
- Reter `Message*` além do `Run` bloqueia a declaração de segurança de lifetime em B até migrar a retenção.
- Isso **não bloqueia automaticamente RAII dos buffers internos**. O bloqueio de E depende de retenção de `Payload*`, `Msg*`, `CommandLine*` e de operações que os invalidem.
- Dependência de `Type` é documentada; não se muda `Type` nesta SPEC.

A assinatura Actions não prova que o Script não retém ponteiros após retornar.

---

## 5. Fase D — simplificar, sem remover a identificação de codificação

### Decisão: **marcador `n2`, mantendo `Version` intacta**

Exemplo:

```text
ng -cl 0.1 [ < 3 s A B C > ]
n2 -cl 0.1 [ < 3 A B C > ]
```

Não aprovo `0.1+enc2`: o código de `Block::Run` usa `Version` para construir o nome da Action. Isso mistura codificação com despacho semântico e pode impedir encontrar a Action.

Também não vejo necessidade de um campo adicional de envelope: o marcador já distingue a gramática.

### Regras obrigatórias

- Despacho pela identificação exata `ng`/`n2`.
- Nunca “tentar novo e, se falhar, tentar legado”.
- Marcador desconhecido gera erro.
- Uma mensagem serializada usa uma só codificação em todas as suas command lines.
- Nenhuma conversão implícita de codificação durante fragmentação/reassembly.

**O marcador não prova que leitores antigos o rejeitam.** Testar os binários antigos reais. Se ignorarem o marcador, reader-first continua sendo a proteção efetiva.

### O que a ausência de consumidores externos permite simplificar

Pode-se remover:

- investigação adicional de consumidores externos do campo `s`;
- negociação sofisticada por peer;
- uma camada genérica de codificações extensíveis;
- escaping e campos semânticos novos desta migração.

**Migração aprovada:** leitor dual distribuído integralmente, inventário verificável de versões, habilitação do escritor novo por configuração de implantação e observação. Não é necessário inventar um protocolo de negociação.

Não remover leitor dual imediatamente: arquivos, mensagens persistidas, retransmissões e binários internos antigos continuam sendo superfícies possíveis.

### Corrigir o gate de estados mistos

Não é esperado que leitor antigo processe wire novo.

A matriz deve exigir:

| Emissor | Receptor | Resultado |
|---|---|---|
| Legado | Dual | Sucesso |
| Novo | Dual | Sucesso |
| Dual escrevendo legado | Antigo | Sucesso |
| Novo | Antigo | Tráfego impedido pela implantação; rejeição segura, se suportada |

Rollback exige desligar escritores novos **e tratar mensagens novas já enfileiradas, persistidas ou em trânsito** antes de rebaixar leitores.

### SCN/hash: o gate atual é ambíguo

Remover bytes muda um hash calculado sobre esses bytes.

**Decisão nesta SPEC: não introduzir nova canonicalização de hash para tentar manter SCNs entre codificações.**

Exigir:

- fixtures legadas com bytes e hashes antigos preservados;
- fixtures novas determinísticas, com hashes esperados próprios;
- validação ponta a ponta conforme o preimage real do protocolo;
- nenhuma recodificação após o cálculo de identidade sem uma política explícita de reconstrução das referências afetadas.

O preimage de SCN precisa ser auditado antes da ativação. “SCN/hash byte-exact” não pode significar igualdade universal entre wire legado e novo.

---

## 6. Fase E — aprovada, mas RAII não corrige borrowers sozinho

### E.1 — buffers

`vector<char>` elimina gestão manual de alocação; **não elimina invalidação de ponteiros**.

Adicionar:

- inventário de leitores e escritores dos campos públicos;
- APIs explícitas para preencher/redimensionar buffers;
- política para ponteiro em buffer vazio e terminação NUL;
- tamanhos derivados do container, evitando campos duplicados inconsistentes;
- atualização de todos os callers de `DeletePayloadArray`/`DeleteMessageArray`;
- regra de invalidação do `Msg` serializado quando payload ou command lines mudam.

“Válido até próxima mutação” deve incluir destruição, atribuição, reset e operações relevantes de movimentação.

### E.2 — `unique_ptr<CommandLine>`

**Aceito.** Copiar um `Message*` copia um endereço e não é afetado pelo `unique_ptr` interno. A premissa da pergunta 3 está incorreta.

O que precisa de auditoria é copiar **o objeto `Message`**:

- construtor de cópia;
- atribuição por cópia;
- movimentos;
- exceções durante cópia profunda.

Preservar cópia profunda do conteúdo lógico. Se `Msg` é representação derivada, a cópia deve deixá-lo ausente **com tamanho/flags coerentes e regeneração possível**. Não preservar estado que diga “serializado” sem os bytes.

O endereço de uma command line sobrevive à realocação do vector de `unique_ptr`, mas não à remoção/substituição da command line nem à destruição da mensagem.

### E.3 — manter flags

Aprovado mantê-las nesta SPEC. C.3 documentado, sozinho, não autoriza removê-las: é preciso demonstrar que seus comportamentos foram substituídos e testar os callers.

---

## 7. Respostas concretas às quatro perguntas

| Pergunta | Decisão |
|---|---|
| **1. Discriminador** | **`n2` no lugar do marcador `ng`**, sem alterar `Version`; despacho exato, sem fallback por tentativa. |
| **2. Container** | **`vector<Slot>` + free-list + geração por slot**, propriedade do Process e retenções explícitas. Não usar map como estrutura principal. |
| **3. `unique_ptr<CommandLine>`** | **Aprovado**, com cópia profunda explícita do objeto `Message`, política completa de operações especiais e auditoria de invalidação de `PCL`. Copiar `Message*` não exige deep-copy. |
| **4. Campos para o Script** | **Não adicionar TTL, ordenação ou campos de lifecycle ao wire em SPEC-033.** Ordenação local usa metadados de fila; retenção é contrato local. Necessidade distribuída comprovada pelo Script gera proposta semântica separada, não um campo preventivo nesta migração. |

---

## 8. Reclassificação dos achados e bugs adicionais

### Reclassificações

| Achado | Avaliação |
|---|---|
| **P2/P3** | **Críticos para correção imediata:** acesso fora de limites e corrupção de stack em caminho de parser. Exposição remota depende de alcançabilidade, ainda a confirmar. |
| **P8** | **Alto:** exaustão de memória/DoS. A alocação deve ser limitada antes de usar cardinalidade não confiável. |
| **P4/P6/P10** | Não são só “tolerância sintática”: podem produzir interpretações divergentes de comandos. Prioridade alta no parser. |
| **C1** | **Mais grave e mais específico:** na sobrecarga com `int`, um índice negativo pode passar no teste e acessar fora do array; saídas podem ficar antigas enquanto retorna `OK`. Não atribuir o mesmo defeito à outra sobrecarga. |
| **C2** | **Alto:** zerar `M` é necessário, mas callers precisam verificar falha. `ListBindings()` já demonstra desreferência sem teste após `NewMessage`. |
| **C3** | **Problema de desempenho**, não corrupção por si só. Não afirmar que toda operação sempre percorre 30000 slots: algumas terminam antes. |
| **C4** | **Menor gravidade:** footprint/previsibilidade, não bug de segurança. Há limite fixo; o que falta é configurabilidade. “525 KB estáticos” depende dos tipos/ABI e da forma de alocação do Process. |
| **C5** | **Alto operacionalmente:** leak persistente pode esgotar o container. O gatilho exato ainda depende dos métodos de flags não fornecidos. |
| **C6** | **Mais grave:** workaround não estabelece lifetime, não impede ABA e não cobre acessos anteriores pelo comparador da fila. |
| **M1** | **Alto quando há borrower atravessando mutação:** risco de UAF. RAII sozinho não fecha. |
| **M2** | **Condicional:** omitir cache serializado pode ser correto; inconsistência de flags/tamanho é o bug a excluir. |
| **M3** | **Rebaixar para dívida de representação:** existir `ResetPayload()` não é bug e continua fazendo sentido com RAII. |
| **M4** | **Correção imediata:** mismatch `new[]`/`delete` é comportamento indefinido, ainda que o caminho seja pouco usado. |
| **M5** | **Alto:** truncamento/framing pode mudar comandos e desalinhar mensagem; corrigir em A. |

### Bugs adicionais visíveis no trecho de Process

1. **`HasMessage` não inicializa `_Answer = false`.** Um miss pode preservar `true` do caller.
2. **`HasMessage(nullptr)` pode retornar `true`** ao encontrar slot vazio.
3. **`EraseMessage(nullptr)` pode decrementar `NoM` em slot já vazio.**
4. **`DeleteMessage(nullptr)` pode desreferenciar nulo** após encontrar slot vazio.
5. `EraseMessage` e `DeleteMessage` retornam `OK` mesmo sem realizar a operação. Definir resultados distintos para inválido, não encontrado e exclusão adiada.
6. **`EraseMessage` remove sem destruir.** Auditar todos os callers antes de migrar para slots proprietários: remoção precisa significar liberação ou transferência explícita, nunca abandono silencioso.
7. `ListBindings()` não verifica criação e declara `InlineResponseMessage` sem inicialização. Inicializar a saída e definir seu destino após `Run`.

---

## 9. Emendas finais exigidas para aceite

Além das correções por fase, substituir os invariantes globais por propriedades testáveis:

- Toda mensagem viva tem exatamente um proprietário: Process.
- Toda retenção assíncrona usa identidade com geração e impede destruição enquanto necessária.
- Todo empréstimo de `Message*`/`CommandLine*` tem escopo e regras de invalidação definidos.
- Parse com erro preserva o objeto e não publica mensagem executável.
- Codificação é selecionada exclusivamente pelo marcador.
- Fragmentos de reassembly pertencem à mesma instância de mensagem e codificação; duplicatas, conflito e timeout têm tratamento definido.
- Contagem de vivos corresponde aos slots ocupados após sucesso **e após falha**.
- Shutdown libera mensagens e retenções sem depender de um próximo ciclo do GW.

**Os gates de carga, fotos e soak permanecem, mas são complementares.** Acrescentar testes determinísticos de lifetime, ABA, falhas, cancelamento e rollback; RSS limitado durante 30 minutos não demonstra ausência de leak ou UAF.

**Conclusão:** aprovo o refactor e a retirada de `s`. Não aprovo a alegação de que lookup por handle simples resolve SPEC-003, nem a auditoria de lifetime depois dessa mudança. Com as emendas acima, a SPEC fica executável em incrementos verificáveis, sem transformar uma simplificação de wire numa migração simultânea e pouco controlada de identidade, propriedade e semântica.