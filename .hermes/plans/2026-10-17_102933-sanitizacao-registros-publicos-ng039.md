# Sanitização dos Registros Públicos NG-039 Implementation Plan

> **For Hermes:** Implementar somente após aprovação explícita deste plano e da SPEC de saneamento; não reescrever o histórico sem autorização separada.

**Goal:** Reduzir o volume e a exposição desnecessária dos registros NG-039 no branch público, mantendo evidência suficiente, verificável e sem apagar silenciosamente o histórico.

**Architecture:** Escopo imediato reduzido para NG-069: remover apenas 11 ficheiros `*.last.sse` comprovadamente idênticos a um `*.raw.sse` retido e sem referências. Preservar o par r12 porque um recibo de submissão imutável referencia o path `.last.sse`. A SPEC lista os paths e hashes exactos. Não transferir evidência para arquivo privado, não mexer em fixtures/pacotes com nomes distintos e não reescrever histórico Git.

**Tech Stack:** Git e Python 3 para igualdade byte a byte, SHA-256, inventário e validação de paths.

---

## Contexto congelado

- Repositório: `/home/gandalf/workspace/novagenesis`.
- Branch: `AIOPT5`; `origin/AIOPT5` confirmado em `ac357849ed7109c0e13ea3faf809b01dc9645694`.
- Inventário read-only inicial: `Specs/RESULTS-SPEC-039/` contém 145 ficheiros e 18.164.472 bytes; a SPEC vigente tem 59.541 bytes.
- Foram encontrados 12 pares exactos `.last.sse`/`.raw.sse`; 11 não têm referências e serão removidos sob a SPEC. O par r12 é mantido devido a referência num recibo imutável. Outros grupos duplicados ficam fora deste incremento.
- Há `.sse`, `.jsonl` e pedidos/transcrições extensos. Avaliar conteúdo e valor probatório; não presumir que todo ficheiro grande é dispensável nem que o conteúdo é seguro para publicação.
- O checkout contém 19 alterações locais preexistentes após o pull de 17/10. Preservá-las byte a byte; não as incluir, restaurar, reorganizar ou misturar com este trabalho.
- O histórico Git público continuará a conter qualquer artefacto removido por commit normal. Apagá-lo da branch não equivale a apagá-lo do histórico.

## Tarefa proposta

- ID candidato: `NG-069`.
- Título: Saneamento dos registos públicos da NG-039.
- Resultado: política e alteração delimitada que reduzem duplicação/volume público sem perder a capacidade de verificar identidade, decisão, escopo e limitações de cada gate.
- Responsável: Antonio Alberti.
- Relação: tarefa filha/associada a NG-039; não altera nem fecha os gates técnicos de correctness ou performance da NG-039.
- Status inicial após aprovação formal do escopo: `planeada`.

## Limites

Incluído:
- Inventariar todos os ficheiros rastreados em `Specs/RESULTS-SPEC-039/`, tamanho, tipo, SHA-256, relações de duplicação e referências cruzadas.
- Classificar cada item como contrato vigente, recibo canónico, evidência bruta, cópia derivada/duplicada, fixture, ferramenta, ou material histórico/ambíguo.
- Avaliar privacidade, informação operacional, contexto de prompts e valor de auditoria dos registos candidatos a sair do branch público.
- Definir um formato compacto de recibo que preserve, no mínimo, data, identificador da revisão/gate, hashes de SPEC/packet/baseline relevantes, veredicto, escopo, limitações, e localização/hash do artefacto arquivado.
- Comparar opções de retenção sem reescrita: manter no branch, remover duplicados exactos, substituir bruto por recibos compactos, ou adiar itens ambíguos.
- Verificar arquivo privado e produzir manifesto de transferência com SHA-256/tamanho e read-back, caso a SPEC aprovada seleccione essa opção.
- Preparar alteração numa branch/worktree isolada, verificar a diferença e aguardar autorização explícita antes de commit/push.

Excluído sem autorização adicional:
- Force-push, filter-repo, rebase público ou qualquer reescrita de histórico.
- Remover/refazer conteúdo fora de `Specs/RESULTS-SPEC-039/`.
- Alterar SPEC-039 técnica, checker, código de produção, testes de runtime, D2/D3 ou evidência de NG-040.
- Apagar ou mover qualquer artefacto antes da sua classificação e verificação do destino.
- Publicar novos conteúdos brutos, credenciais ou informação pessoal no arquivo privado.

## Plano de execução

### Fase 1 — Abrir a tarefa e congelar a base

1. Confirmar que `NG-069` ainda não existe e criar a nota Obsidian conforme `Templates/Tarefa.md`; ligar o Dashboard com resumo curto.
2. Registar o SHA exacto de `AIOPT5`, estado do worktree e inventário inicial; separar claramente as 19 alterações preexistentes.
3. Criar SPEC própria de higiene de evidências públicas, em Draft, com allowlist de paths e decisões ainda abertas. Não editar os artefactos-alvo nesta fase.

Gate: tarefa e SPEC identificam exact scope, artefactos incluídos/excluídos e proibições; mudanças preexistentes continuam intactas.

### Fase 2 — Confirmar a allowlist fechada

1. Revalidar os 12 pares identificados por tamanho, SHA-256 e igualdade byte a byte.
2. Verificar referências no repositório e na nota NG-039. Excluir da remoção o par r12, pois `astra-review-v1.14-r12-20261013-submission.json` referencia explicitamente o path `.last.sse`.
3. Confirmar que a allowlist fechada de 11 paths corresponde à tabela de `Specs/SPEC-069-ng039-public-evidence-deduplication.md`.

Gate: todas as pré-condições passam; divergência ou referência nova interrompe a remoção.

### Fase 3 — Remover somente as cópias aprovadas

1. Apagar apenas os 11 paths allowlisted; não modificar os `.raw.sse` retidos, o par r12, recibos ou outros ficheiros.
2. Recontar os ficheiros/bytes e confirmar todos os hashes retidos.

Gate: 134 ficheiros e 13.928.653 bytes na pasta, com os 11 exemplares brutos presentes e verificáveis.

### Fase 4 — Validar e actualizar registos locais

1. Executar `git diff --check` e verificar que somente os 11 paths da allowlist aparecem como deletados.
2. Confirmar que os 19 deltas preexistentes e os novos ficheiros locais não foram staged nem alterados indevidamente.
3. Atualizar task/Dashboard com o resultado, a exceção r12 e a limitação do histórico Git.

Gate: verificações passam e não ocorre commit/push.

### Fase 5 — Decisão posterior de publicação

1. Apresentar ao proprietário o diff exacto e esclarecer que os bytes continuam acessíveis no histórico Git.
2. Não fazer commit/push nesta tarefa sem pedido separado.
3. Não propor reescrita do histórico como passo implícito.

## Critérios de aceitação propostos

- Onze ficheiros `.last.sse` redundantes removidos; o par r12 referenciado permanece intacto.
- Os 11 `.raw.sse` correspondentes permanecem com os hashes congelados.
- Contagem e bytes da pasta confirmados em 145/18.164.472 antes e 134/13.928.653 depois.
- Nenhuma alteração fora da allowlist de remoção e da SPEC; histórico Git não reescrito.
- Paths alterados ficam dentro da allowlist; worktree anterior fica preservado.
- `git diff --check`, verificações de referências/hashes e revisão final passam.
- Commit/push só ocorre com autorização separada; a tarefa não implica aprovação técnica D2/D3.

## Riscos e perguntas que a SPEC deve resolver

- Respostas brutas podem conter contexto de prompt ou informação operacional; a triagem é manual e não deve enviar esse material a outro serviço de IA.
- Arquivo privado preserva bytes, mas não torna a evidência verificável por leitores públicos sem acesso; recibos públicos devem declarar essa limitação.
- Eliminar duplicados pode quebrar referências ou reduzir proveniência se cópias tiverem funções distintas; verificar consumidores antes da decisão.
- Um novo commit reduz a árvore actual, não o histórico público. Qualquer purga histórica é separada e pode afectar clones/colaboradores.

## Validações reproduzíveis

- Inventário e pares: script Python read-only baseado na tabela exacta da SPEC, com tamanho/SHA-256 e igualdade byte a byte.
- Referências: varrer ficheiros rastreados e a nota NG-039; manter r12 por referência explícita no recibo.
- Preservação: recalcular SHA-256 dos 11 `.raw.sse` mantidos.
- Escopo: `git diff --name-status` limitado à allowlist aprovada; comparar estado inicial das alterações preexistentes.
- Integridade: `git diff --check`, validação JSON/Markdown dos artefactos tocados e scans focados pós-alteração.
- Publicação: depois de autorização, confirmar `git ls-remote origin refs/heads/AIOPT5` igual ao commit publicado e ler de volta os ficheiros alterados.

## Estado deste plano

Plano aplicado em escopo reduzido pela tarefa NG-069. A deduplicação local foi autorizada; publicação e qualquer reescrita de histórico permanecem gates separados.
