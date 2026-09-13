# Revisão Astra — simplificação de mensagens do NovaGenesis

## Parecer executivo

**A direção de simplificação é boa, mas a ordem de execução e a classificação de risco do Hermes precisam mudar.** Há três problemas centrais:

1. **Remover `s` não é uma mudança de baixo risco:** leitores antigos podem aceitar a mensagem nova e produzir argumentos incompletos, em vez de rejeitá-la.
2. **Armazenar tudo como `string` não prova que o tipo seja semanticamente inútil para todos os consumidores.** Prova apenas que estes parsers não fazem conversão tipada.
3. **RAII dos buffers não resolve o ciclo de vida de `Message`.** Principalmente com o novo Script, não se pode eliminar controles de retenção, identidade ou exclusão sem examinar seus usuários.

**Recomendo:** corrigir e unificar a leitura primeiro, mantendo a escrita legada; migrar ownership separadamente; só depois habilitar um formato explicitamente discriminado, sem `s` e com contagem.

> **Referências:** as linhas abaixo contam a partir do `/*` inicial de cada arquivo fornecido; intervalos identificam os trechos relevantes. Não foram fornecidos `Message.cpp`, `MessageBuilder.cpp`, `CommandLine.h`, consumidores nem o novo Script. Portanto, não considero demonstradas as alegações sobre AMEND-3, execução dos testes, dispatch ou ownership real.

---

## 1. Verificação da análise contra o código

### 1.1 Wire format: o que está demonstrado

Em `CommandLine.cpp`:

- **L. 235–287:** o serializer escreve `ng`, `Name`, `Alternative`, `Version`, argumentos e newline.
- **L. 266:** escreve literalmente `out.str() << SP << "s "`.
- **L. 270–276:** escreve cada elemento **sem escaping nem validação**.
- **L. 327–347:** o parser de stream aceita `s`, `h` e `i`, mas grava os valores pelo mesmo `SetArgumentElement`.

Logo:

- **Confirmado:** eliminar `s ` economiza dois bytes por vetor de argumento.
- **Confirmado:** não há conversão diferenciada para `h` ou `i` nesse parser.
- **Não demonstrado:** `h` e `i` não têm significado para consumidores externos ou scripts.
- **Correção importante:** “elementos não podem conter whitespace ou marcadores” **não é uma invariável imposta pelo modelo**. `SetArgumentElement` aceita qualquer `string` — l. 198–214 — e o writer a emite literalmente.

Hoje é possível construir em memória uma linha que não sobreviva a round-trip. Isso inclui strings vazias, espaços e valores que conflitem com delimitadores.

O exemplo inicial de Hermes também é apenas esquemático: o writer emite **nome e alternativa**, não apenas `ng -cl 0.1`.

### 1.2 Dois parsers: diagnóstico correto, detalhes incorretos

**Confirmado:** existem duas implementações independentes, a partir das l. 289 e 376.

**Correções:**

- `WhiteSpacePositions[4096]`, l. 383, limita posições de **espaços**, não caracteres.
- A escrita sem checagem, l. 404–407, estoura o array ao registrar o **4097º espaço**. Uma linha de 5000 caracteres sem muitos espaços não provoca esse overflow específico.
- O parser por array reconhece apenas `' '` como separador; `operator>>` usa a classificação de whitespace do stream. Portanto, tabs, quebras e espaços repetidos têm comportamentos diferentes.
- As condições `Temp == "<" && Temp != "<1"...`, l. 311, são redundantes. Elas não demonstram por si só um mecanismo sofisticado de recuperação.
- A contagem de `<` e `>` é coletada — l. 425–432 — mas **não participa da condição de aceitação**, l. 436–441.
- `StringToInt` não é chamado por esses parsers: eles usam `stringstream` diretamente.

### 1.3 Falhas adicionais relevantes

| Falha | Evidência e consequência |
|---|---|
| Fechamento não validado no stream | L. 350 e 370 leem tokens, mas não verificam efetivamente `>` e `]`. |
| Sucesso prematuro no parser por array | `Status = OK`, l. 490, ocorre antes de validar os vetores. Uma linha malformada pode retornar sucesso. |
| Contagem declarada não validada contra conteúdo | L. 504–526: aloca o vetor, ignora valores inválidos e pode deixá-lo parcialmente vazio. |
| Acesso antecipado sem bounds check | `Words[l + 1]`, l. 497, pode sair do array se um `<` aparecer perto do fim dos tokens. |
| Alocação sem limite no stream | L. 320–323: qualquer tamanho positivo representável pode chegar a `NewArgument`. |
| Número parcialmente convertido | A extração numérica não exige consumo integral: por exemplo, um prefixo decimal seguido de lixo pode ser aceito. |
| Mutação parcial do destino | Ambos modificam `CL` durante a leitura; não há rollback nem limpeza explícita do estado anterior. No stream, o índice começa em zero mesmo se o objeto já tiver argumentos. |
| Linha sem argumentos inconsistente | O writer suporta `NoA == 0`, l. 282–284; o parser por array exige `[` e `]`, l. 438–439. |
| Custo de construção potencialmente quadrático | `NewArgument`, l. 109–160, realoca e copia os argumentos anteriores a cada inclusão. |

**Conclusão:** não basta endurecer o array de 4096 posições. É necessário definir uma gramática e um contrato de erro.

### 1.4 `Message`: estrutura confirmada, conclusões excessivas

Em `Message.h`, l. 88–169, estão presentes os buffers, arquivos e controles descritos. Porém:

- O contador é **`NoCL`**, não `NoC`.
- `Delete` é um controle privado; `ApplicationDeleted` tem semântica ligada à aplicação/core. Não são necessariamente flags equivalentes aos controles de liberação dos arrays.
- O comentário de `InstantiationNumber` fala em dupla exclusão, mas sua implementação não está disponível.
- A declaração e o comentário de `ResetPayload()` confirmam que ele restaura **estado**, não apenas libera memória.
- Não há base para concluir que o copy constructor possa ser defaultado depois de trocar somente os buffers.

### 1.5 `MessageBuilder`: há redundância aparente, não comprovada

`MessageBuilder.h`, l. 164–167, já declara:

```cpp
NewCommonCommandLine(Name, Alternative, Version, Category, Key, Values, ...);
```

Essa é uma abstração muito próxima do objetivo de Hermes.

Além disso:

- O header fornecido tem **19 declarações `NewStoreBinding...`**, não cerca de 50 dessa família.
- Há métodos que aparentemente calculam hashes ou consultam `Process`/`Block`, não apenas mudam categoria e ordem.
- Sem o `.cpp`, não sabemos quanto já delega a `NewCommonCommandLine`.
- Alguns comentários usam `string`; outros omitem o tipo. **Comentários não são especificação confiável do wire atual.**

---

## 2. Veredicto por proposta

### Proposta 1 — remover `s`, com leitor compatível

**Veredicto: ACEITO COM RESALVAS — risco alto durante a migração.**

**Prós**

- Remove metadado repetitivo no perfil em que todos os elementos são strings.
- Economiza dois bytes por vetor.
- Simplifica a gramática nova.

**Contras e riscos**

- A economia é pequena diante do custo de uma mudança de protocolo.
- O leitor por array antigo **pula o primeiro valor como se fosse o tipo**.
- O leitor de stream antigo pode interpretar um primeiro valor `s`, `h` ou `i` como tipo.
- “Aceitar com/sem” não deve significar adivinhar a gramática pelo conteúdo: esses mesmos tokens são valores legítimos.
- Nenhum dos parsers fornecidos usa `Version` para selecionar ou rejeitar gramáticas.

Exemplo:

```text
ng -x --y 0.2 [ < 1 A > ]
```

No parser por array antigo, `A` ocupa a posição ignorada do tipo; `>` é tratado como candidato a valor e descartado. O resultado pode ser **um vetor com string vazia e retorno `OK`**.

**Condições para aprovação**

1. Discriminador explícito e inequívoco.
2. Leitores atualizados antes dos escritores.
3. Emissão legada por padrão até confirmar capacidade do destinatário.
4. Auditoria do uso de `Version`: ele pode versionar comandos/ações, não a codificação.
5. Auditoria de SCNs, hashes, caches e assinaturas: se dependem dos bytes serializados, a mudança altera identidade.

---

### Proposta 2 — parser único

**Veredicto: ACEITO COM RESALVAS — prioridade máxima.**

**Prós**

- Elimina divergências.
- Centraliza limites, gramática, diagnóstico e testes.
- Corrige vulnerabilidades reais de memória e aceitação parcial.

**Ressalvas**

- **Não remover imediatamente `operator>>` da API.** Mantê-lo como adaptador para o parser compartilhado evita quebra desnecessária de compilação e comportamento de streams.
- Não tomar o parser por array atual como referência de correção.
- Crescimento dinâmico não substitui limites contra consumo excessivo de memória/CPU.
- Tornar a leitura estrita pode rejeitar arquivos históricos que eram tolerados. O corpus existente precisa ser examinado.

Contrato recomendado:

```cpp
ParseResult ParseCommandLine(
    std::string_view input,
    WireFormat format,
    CommandLine& out);
```

Com:

- limites de bytes, vetores, elementos e tamanho de elemento;
- números integralmente validados;
- fechamento e consumo total obrigatórios;
- erro com posição e motivo;
- destino inalterado em erro;
- construção temporária com ownership seguro, seguida de commit.

**Atenção:** não implementar o commit com atribuição rasa de uma classe dona de ponteiros. É necessário verificar a regra dos cinco de `CommandLine`.

---

### Proposta 3 — buffers RAII e remoção de flags

**Veredicto: ACEITO COM RESALVAS — aprovo RAII, não a eliminação indiscriminada dos controles.**

**Prós**

- Ownership dos buffers mais claro.
- Menos caminhos de liberação manual.
- Melhor segurança diante de exceções e inicialização parcial.

**Riscos não previstos**

- `Payload`, `Msg` e `CommandLines` são públicos: consumidores podem atribuir ou reter ponteiros diretamente.
- Getters devolvem ponteiros emprestados; realocações do `vector` invalidam esses ponteiros.
- Operações assíncronas podem continuar usando o buffer depois de reset, move ou destruição.
- Buffers originalmente emprestados não podem virar owned sem definir cópia ou transferência.
- RAII dos membros **não impede dupla exclusão do próprio `Message`**.
- `vector<unique_ptr<CommandLine>>` não é copiável por padrão.
- `vector<CommandLine>` exige operações de cópia/movimentação seguras e pode invalidar os `CommandLine*` retornados pela API.

**Decisões concretas**

- Preservar a operação semântica de `ResetPayload()`, ainda que sua implementação fique simples.
- Não remover `Delete`, `ApplicationDeleted` ou `InstantiationNumber` sem examinar o ciclo de vida.
- Não remover `Type`: o novo Script torna essa investigação ainda mais necessária.
- Preferir inicialmente `vector<unique_ptr<CommandLine>>` se estabilidade dos endereços for requisito; definir explicitamente a política de cópia.
- Separar a extração de I/O para outra mudança. Forçar tudo para memória pode aumentar pico de RAM e eliminar staging útil.

Agrupar metadados pode melhorar organização, mas **não demonstra ganho de cache** sem layout e medições.

---

### Proposta 4 — deduplicar `MessageBuilder`

**Veredicto: ACEITO COM RESALVAS.**

**Prós**

- Centraliza montagem e validação.
- Reduz divergência entre categorias.
- Facilita testes.

**Riscos**

- Um único `vector<string> Args` não descreve claramente os limites entre vetores de argumentos.
- Apagar wrappers perde nomes que documentam direção do binding e transformações.
- Reordenação, hash diferente ou cardinalidade diferente podem alterar o wire e SCNs, mesmo sem intenção.

**Recomendação**

Auditar e reutilizar `NewCommonCommandLine` antes de inventar outro primitive. Preservar wrappers semânticos e categorias nomeadas.

**Compile + testes existentes é insuficiente:** comparar bytes e estrutura gerados antes/depois para cada wrapper migrado.

---

### Proposta 5 — remover também a contagem

**Veredicto: REJEITO nesta revisão.**

**Prós**

- Menor verbosidade.
- Remove a necessidade de converter a contagem.
- Pode ser robusto com uma gramática bem definida.

**Por que rejeito agora**

- Não sabemos se o Script ou outros consumidores dependem da cardinalidade declarada.
- Trocar a contagem por busca de `>` não é, por si só, uma verificação “mais forte”.
- A ausência do delimitador ainda exige limite de leitura.
- A contagem pode detectar elementos faltantes/excedentes quando **efetivamente validada**.
- A economia incremental não justifica ampliar a migração antes de medir o ganho.

Não rejeito formatos sem contagem em princípio. Rejeito incluí-los agora sem necessidade demonstrada e sem contrato de framing/escaping.

---

## 3. Respostas às quatro open questions

### 1. Existem ferramentas ou artefatos dependentes da sintaxe exata?

**Não é possível confirmar nem negar com estes arquivos.** A afirmação “todos os nós são nossos, logo só existe rolling upgrade” é insuficiente.

Auditar:

- scripts com regex, `split`, `awk`, offsets ou substituições literais;
- fixtures, documentação executável e arquivos persistidos;
- replay de logs, caches, bridges e ferramentas de diagnóstico;
- cálculo e verificação de SCN sobre bytes.

Um artefato de artigo pode exigir apenas atualização documental; um fixture ou script pode ser consumidor real.

### 2. O Script depende do tamanho para lookahead?

**Desconhecido: o Script não foi incluído.** Isso bloqueia a remoção da contagem, não a correção do parser.

Verificar se ele:

- usa APIs de argumentos ou interpreta o wire diretamente;
- mantém offsets ou ponteiros para buffers;
- usa cardinalidade para avançar entre vetores;
- retém mensagens durante execução assíncrona;
- associa o novo tipo a `Message::Type`.

Cardinalidade não equivale a tamanho em bytes: mesmo com `N`, avançar exige interpretar os elementos.

### 3. Manter `ng` e aumentar `Version`, ou usar `ng2`?

**Não aprovo usar o `Version` existente como discriminador sem auditar dispatch.**

Há ainda uma armadilha: **`ng2` não garante rejeição instantânea pelo parser por array antigo**. Ele testa somente `_CL[0]=='n'`, `_CL[1]=='g'` e `_CL[3]=='-'`, l. 396. Na forma usual `ng2 -...` ele rejeitaria, mas isso decorre da posição do espaço/hífen, não de validação correta do marcador completo.

Minha preferência é um **discriminador de codificação separado da versão semântica**, no envelope, se disponível. Sem envelope adequado, um marcador novo rigorosamente definido é mais seguro que sobrecarregar `Version`.

Qualquer opção exige reader-first; marcador novo não cria compatibilidade.

### 4. Confirmar `short Type` antes do refactor

**Não confirmado; deve permanecer.**

`Message.h` declara `Type`, construtores que o recebem, `SetType` e `GetType`. Isso prova uma superfície pública, não a semântica dos consumidores.

Auditar leituras/escritas diretas, switches, filas, Script, serialização e valores persistidos. Só então considerar `enum class`, preservando valores e avaliando signedness/ABI. Trocar `short` por `uint16_t` pode alterar o significado de valores negativos.

---

## 4. Wire format recomendado

### Escolha: remover o tipo, manter a cardinalidade

Gramática conceitual do corpo:

```text
[ < 3 E1 E2 E3 > < 1 X > ]
```

Com discriminação explícita da codificação fora desse corpo. **Não emitir essa forma sob o contrato legado.**

Regras necessárias:

1. **Strings simples permanecem sem aspas**, para manter compacidade.
2. Strings vazias ou com whitespace, delimitadores, aspas ou barras invertidas usam uma regra única de quoting/escaping — por exemplo, strings entre aspas com escapes JSON.
3. Contagem significa número de elementos **decodificados**.
4. Ler exatamente `N` elementos e exigir `>`.
5. Definir se vetores vazios são permitidos; não deixar isso ao acaso.
6. Definir limites e representação textual canônica.
7. Escapar newline em valores para não romper o framing por linha.
8. Manter payload binário fora dessa gramática textual.

Exemplo conceitual:

```text
[ < 3 A "nome com espaço" ">" > < 1 "" > ]
```

“Afinal tudo é string” elimina a necessidade de um rótulo de tipo por vetor; **não elimina a necessidade de fronteiras inequívocas entre strings**.

---

## 5. Plano seguro para VM101/VM102

| Etapa | VM101 | VM102 | Escrita |
|---|---|---|---|
| Inicial | Leitor legado | Leitor legado | Legada |
| Reader-first parcial | Leitor dual | Leitor legado | Legada |
| Reader-first completo | Leitor dual | Leitor dual | Legada |
| Habilitação controlada | Leitor dual | Leitor dual | Nova somente com capacidade confirmada |
| Consolidação | Leitor dual | Leitor dual | Política nova, com suporte aos dados históricos necessário |

**Rollback:** desabilitar a escrita nova antes de voltar um nó para leitor antigo; filas, caches e arquivos com mensagens novas também precisam de tratamento.

### Gates mínimos

- Fixtures legadas byte-exact.
- Round-trip estrutural das duas gramáticas.
- Truncamento, overflow numérico, contagem incorreta e delimitadores faltantes.
- Valores `s`, `h`, `i`, vazios, whitespace e marcadores.
- Parsing em objeto já preenchido e garantia de não mutação em erro.
- Fuzzing com ASan/UBSan.
- VM101→102 e VM102→101, nos estados mistos suportados.
- SCN/hash e payload byte-exact.
- Ciclo completo do novo Script: retenção, conclusão, cancelamento, reset e exclusão.
- Carga e memória, além do gate de 500 msg/s.

**Ordem final recomendada:** especificação e testes → parser único seguro com escrita legada → deduplicação interna → RAII em mudanças pequenas → migração discriminada sem `s`. **Contagem e `Type` permanecem.**