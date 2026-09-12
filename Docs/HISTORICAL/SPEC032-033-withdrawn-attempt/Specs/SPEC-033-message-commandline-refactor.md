# SPEC-033 — Message & CommandLine lifecycle refactor

**Status:** DRAFT (for Astra review)
**Author:** Hermes, on behalf of Antonio
**Date:** 2026-09-07
**Depends on:** Astra review of `Docs/REVIEW-Astra-Message-CommandLine-2026-09-07.md`
**Branch target:** AIOPT3 (or new branch `msg-refactor` off 8e92e29)

---

## 0. Motivação

O ciclo de vida das mensagens hoje depende de:

1. **Dois parsers independentes de CommandLine** (`operator>>` e
   `ConvertCommandLineFromCharArray`) com gramáticas divergentes e ~10 bugs
   confirmados (ver §2.1).
2. **Container de mensagens do Process** com capacidade fixa de 30000 slots
   (`Process.h:140`, `MAX_MESSAGES_IN_MEMORY`), varredura linear em TODAS as
   operações, e indexação inconsistente (`GetMessage` usa `NoM` como limite
   sobre um array com buracos).
3. **Message** com 3 representações simultâneas (CommandLines, char arrays,
   Files) e lifetime manual com 4 flags + contador anti-double-delete.
4. **Wire format verboso** (`[ < 3 s A B C > ]`) onde o campo de tipo `s` é
   gravado e lido mas ignorado semanticamente (tudo é string).

Além disso, o novo **Script de ciclo de vida** (implementado pelo Antonio no
ciclo Run das mensagens) adiciona mais um retentor de `Message*` cujo contrato
ainda não está documentado — este SPEC inclui sua auditoria.

## 1. Escopo

**IN:**
- F1: Parser único de CommandLine com contrato de erro formal
- F2: Refactor do container `Process::Messages[]`
- F3: RAII gradual em `Message` (payload/message buffers)
- F4: Wire format sem o campo de tipo `s`, com discriminador de codificação
- F5: Auditoria e documentação do ciclo de vida (quem retém `Message*`, ordem
  enqueue → pop → Run → mark → delete)

**OUT:**
- Remoção do campo de cardinalidade (REJEITADO pelo Astra nesta revisão)
- Remoção de `Type`, `Delete`, `ApplicationDeleted`, `InstantiationNumber`
  (só depois da auditoria F5)
- Refactor estrutural do PG.cpp (SPEC separada, fila NG-019 item 6)
- Migração de File I/O para fora de Message (fase futura)

## 2. Achados que motivam cada fase

### 2.1 Parser (motiva F1)

| # | Bug | Local |
|---|---|---|
| P1 | `Status = OK` setado antes de validar os vetores | CommandLine.cpp:490 |
| P2 | `Words[l+1]` acessado sem bounds check quando `<` está no fim dos tokens | CommandLine.cpp:497 |
| P3 | `WhiteSpacePositions[4096]` fixo; 4097º espaço corrompe memória (stack) | CommandLine.cpp:383 |
| P4 | Contagem declarada nunca validada contra elementos reais | CommandLine.cpp:500-515 |
| P5 | Parser array ignora o token de tipo por posição; parser stream aceita s/h/i → comportamentos divergentes | CommandLine.cpp:327 vs 485 |
| P6 | `operator>>` lê tokens sem validar fechamento `>` e `]` | CommandLine.cpp:350,370 |
| P7 | Elementos podem conter `<`, `>`, `[`, `]` (SetArgumentElement aceita qualquer string) → round-trip quebrado | CommandLine.cpp:198-214 + writer 266-276 |
| P8 | Parser stream aloca vetor de qualquer tamanho positivo (sem limite) | CommandLine.cpp:320 |
| P9 | `operator<<` vs parser array: NoA==0 emitido sem `[ ]` mas parser exige os marcadores | CommandLine.cpp:282 vs 438 |
| P10 | Conversão numérica aceita prefixo decimal + lixo | ambos |

### 2.2 Container do Process (motiva F2)

| # | Bug | Local |
|---|---|---|
| C1 | `GetMessage(idx)` testa `idx < NoM` sobre array com buracos → retorna NULL/pulsa | Process.cpp:704 |
| C2 | `NewMessage` (1ª sobrecarga) não zera `M` quando container cheio → caller usa lixo | Process.cpp:542 |
| C3 | Varredura linear de 30000 slots em NewMessage/DeleteMessage/HasMessage/OkToRun/EraseMessage a cada chamada | Process.cpp:546,719,745,767,988 |
| C4 | `Messages[30000]` + `Controls[30000]` fixos: ~525 KB estáticos por processo, sem bound dinâmico | Process.h:140,143 |
| C5 | Mensagem nunca deletada se app esquece de resetar `ApplicationDeleted` (comentário do próprio código admite) | Message.cpp:449-452 |
| C6 | SPEC-003 workaround: `OkToRun` checa use-after-free de ponteiro em fila — o design não garante lifetime | Block.cpp:178 |

### 2.3 Message RAII (motiva F3)

| # | Achado | Local |
|---|---|---|
| M1 | `Payload`/`Msg` públicos + getters de ponteiro emprestado ("Don't delete[]") → realocação quebra todos os borrowers | Message.h:122,125 |
| M2 | Copy ctor copia payload byte-a-byte; não copia `Msg` deliberadamente (comentário l.221) → semântica de cópia parcial não documentada | Message.cpp:221 |
| M3 | `ResetPayload()` (SPEC-018) existe só porque o estado é manual (4 campos) | Message.cpp:818 |
| M4 | `delete Temp` (sem `[]`) na conversão não-2 | Message.cpp:1849 |
| M5 | `Line[4096]` corta command lines longas silenciosamente em 3 pontos (operator>> Message, operator>> fstream, ConvertMessage...2) | Message.cpp:2366,2441,1273 |

### 2.4 Wire format (motiva F4)

Formato atual por argumento: `< NoE s E1 ... EN >`. O `s` é 2 bytes mortos
todos os parsers o ignoram semanticamente. Risco da remoção: parsers antigos
podem ACEITAR mensagens novas com parse silenciosamente errado (vetor vazio +
OK). Por isso F4 exige discriminador + reader-first (ver plano do Astra, §5
dele).

## 3. Fases de implementação

### FASE A — Parser único (F1) — sem mudança de wire

**A.1** Extrair `ParseCommandLine(std::string_view input, CommandLine& out) →
ParseResult` em arquivo novo (`CommandLineParser.h/.cpp`):

- Limites: 64 KiB por linha, 256 argumentos, 4096 elementos/argumento,
  4096 bytes/elemento (todos como constantes nomeadas, revogáveis).
- Contagem declarada DEVE casar com elementos lidos (fecha P4).
- Fechamento `>` e `]` obrigatórios, tokens restantes → erro (fecha P6).
- Números validados integralmente (fecha P10).
- `out` NÃO é modificado em caso de erro (constrói em temp, commit no fim).
- Elementos com marcadores/whitespace: REJEITAR com erro explícito nesta fase
  (comportamento atual é corrupção silenciosa; decidir escaping na Fase D).
- Erro retorna posição + motivo (enum + offset).

**A.2** Adaptadores: `operator>>` e `ConvertCommandLineFromCharArray` passam a
chamar o parser único (assinaturas preservadas — o Astra pediu para não
remover `operator>>` da API ainda).

**A.3** `ParseResult` parse no objeto pré-preenchido: limpa argumentos antes
(fecha a mutação parcial).

**GATE A:** fuzzing ASan/UBSan (truncamento, overflow, contagem errada,
delimitadores faltantes, valores s/h/i/vazios/marcadores); round-trip byte-exact
de fixtures legados; gate 500 msg/s nas VMs 101/102; SCN/hash byte-exact.

### FASE B — Container do Process (F2) — sem mudança de Message

**B.1** `Message* Messages[30000]` → `std::vector<Message*> Messages_` +
free-list, ou `std::unordered_map<uint32_t, Message*>`. Capacidade configurável
(env var ou construtor), default 30000 para compat.

**B.2** Corrigir de imediato os bugs C1/C2 independente da estrutura (patch
pequeno e segurável antes do refactor maior):
- `GetMessage(idx)`: iterar apenas slots BUSY (ou índice = handle direto).
- `NewMessage` 1ª sobrecarga: `M = NULL` no início.

**B.3** Handle-based lookup: `OkToRun`/`HasMessage`/`DeleteMessage` passam a
usar o handle (índice no map) em vez de comparação de ponteiro — elimina a
classe SPEC-003 de use-after-free por ponteiro órfão (fecha C6 parcialmente:
a fila guarda handles, não ponteiros).

**GATE B:** gate 2000 msg/s bidirecional (o container está no hot path);
`GetNumberOfMessages` idempotente; soak 30 min RSS bounded.

### FASE C — Auditoria de lifetime + Script (F5) — zero código de produção

**C.1** Documentar em `Docs/MESSAGE-LIFECYCLE.md` o contrato atual:
```
GW receive → Process.NewMessage (slot BUSY, NoM++)
  → deserialise → GW.PushToInputQueue (priority_queue<Message*>)
  → GW pop (due time) → Block::Run(OkToRun? → Actions Run)
  → Actions criam respostas via NewMessage + ScheduledMessages
  → marcar MarkToDelete (só se ApplicationDeleted==DELETED_BY_CORE)
  → PP->DeleteMarkedMessages() (GW loop, por ciclo)
```
Inventariar TODO retentor de `Message*`: InputQueue, OutputQueues,
ScheduledMessages (vetor por Run), ponteiros locais nos Actions, o novo
Script, referências em Blobs/HTS/PSS (filter com grep em todos os módulos,
tabela: quem / quando / quanto tempo / como solta).

**C.2** AUDITAR O SCRIPT DO CICLO DE VIDA (o novo): responder formalmente:
- usa APIs de argumentos ou interpreta wire diretamente?
- usa cardinalidade para lookahead/avanço? (bloqueia F4 se sim)
- retém mensagens além do Run corrente? (bloqueia RAII se sim)
- associa estado ao `Message::Type`? (bloqueia mudança de Type — fora de escopo, mas documentar)

**C.3** Definir ownership: uma única entidade é DONA de cada mensagem em cada
estágio (GW até pop; Block/Action durante Run; Script se retiver). Escrever
isso como invariantes verificáveis.

**GATE C:** documento revisado pelo Antonio + Astra. Nenhum código muda.

### FASE D — Wire format sem `s` (F4) — reader-first

**D.1** Novo formato: `[ < 3 E1 E2 E3 > ]` (sem tipo; cardinalidade fica).
Discriminador de codificação **separado da Version semântica** — proposta:
prefixo do envelope da linha: `ng -cl 0.1` (legado) vs `ng -cl 0.1+enc2` OU
marker novo `n2` (decidir na revisão do Astra; ele preferiu discriminador
fora do corpo).

**D.2** Ordem: (1) leitor dual em todas as VMs; (2) confirmar capacidade;
(3) trocar escritor PGCS→legado→novo; (4) consolidar. Rollback = desligar
escritor novo antes de rebaixar leitor.

**D.3** Consumidores externos da sintaxe: **verificado em 2026-09-07 — não
existem.** O usuário confirmou que nenhuma fixture/script externa usa o campo
`s`; grep em Scripts/, Docs/, Specs/, Make/ não encontrou nenhum parser textual
do wire (`[ < 1 s ...`) fora do código C++. A única superfície consumidora são
os parsers C++ (Fase A) e os comentários de documentação nos headers. O risco
de fixture externo quebra é, portanto, nulo; a migração fica limitada ao
reader-first entre nossas VMs.

**GATE D:** fixtures byte-exact legados; round-trip dual; VM101→102 e
102→101 em todos os estados mistos; SCN/hash byte-exact; Script cycle
completo (retenção, conclusão, reset, exclusão); gate 500 msg/s.

### FASE E — RAII gradual (F3) — depois do C (ownership mapeado)

**E.1** `Payload`/`Msg` → `std::vector<char>` PRIVADO com getters
`const char* data() + size()` (borrowed, documento: válido até próxima
mutação). `DeletePayloadArray`/`DeleteMessageArray` removidos.
`ResetPayload()` mantém assinatura e semântica (implementação trivial).

**E.2** `CommandLines` → `std::vector<std::unique_ptr<CommandLine>>` privado;
`GetCommandLine` continua devolvendo `CommandLine*` (estável: unique_ptr
endereços estáveis). Copy ctor preservado com cópia profunda explícita
(documentar M2: Msg NÃO é copiado — manter esse comportamento e documentar).

**E.3** `Delete`/`ApplicationDeleted`/`InstantiationNumber`: só removível
depois de C.3; nesta fase apenas documentar quem lê/escreve.

**GATE E:** guard tests 6/6; fotos 1000/1000 byte-exact; soak 30 min @500.

## 4. Invariantes globais (verificáveis)

1. Nenhuma mensagem é deletada enquanto retida por fila/Script/Action.
2. Todo `Message*` fora do Process container é emprestado, exceto os das
   filas GW (donos até pop).
3. ParseCommandLine nunca deixa objeto parcialmente modificado em erro.
4. Um leitor dual nunca produz parse silenciosamente diferente entre
   gramáticas para a mesma entrada válida.
5. Wire novo e legado nunca coexistem no mesmo buffer de reassembly SAR
   (framing por linha é compartilhado).

## 5. Testes e verificação

- Testes unitários novos: `tests/` para parser (matriz §GATE A).
- Fuzzing: libFuzzer ou corpus aleatório com ASan/UBSan.
- Byte-compat: fixture de mensagens legadas commitado; round-trip byte-exact.
- Gates de carga existentes (500/2000 msg/s, soak 30min) — os mesmos da
  SPEC-031/028-B.
- Critério de aceite por fase: os GATEs acima.

## 6. Riscos

| Risco | Mitigação |
|---|---|
| Parsers antigos em outras VMs aceitarem novo formato com parse errado | Reader-first + discriminador (Fase D) |
| Realocação de vector invalidar ponteiros emprestados | Fase E só depois do inventário de borrowers (Fase C) |
| Script do ciclo de vida depender de cardinalidade/wire | Fase C.2 bloqueia F4 se confirmar dependência |
| Container no hot path regredir com map/hash | Benchmark A/B no gate B (2000 msg/s) |
| Rejeitar mensagens legadas toleradas pelo parser velho | Fase A mantém comportamento: mesmos inputs válidos passam; só adiciona erros para os que já corrompiam |

## 7. Adenda — Runtime scheduler wait e aceitação da Fase B

### Estado actualizado

| Campo | Estado |
|---|---|
| Estado documental | Em revisão — adenda de aceitação runtime |
| Implementação da Fase B | Parcialmente implementada |
| Harnesses | 14/14 reportados; reexecutar após a correcção |
| Runtime cross-VM | Bloqueado por busy-loop com fila futura |
| Gate 1000 fotos | Não concluído |
| Aceitação da Fase B | Não concedida |
| Referência | `AIOPT3@05ffc35` |

### Requisitos normativos

- A mera existência de uma entrada na `InputQueue` não pode tornar o gateway imediatamente executável quando o deadline ainda é futuro.
- A espera deve distinguir fila não vazia de trabalho devido, usando a mesma comparação temporal estrita (`deadline < agora`) da extracção actual, e reavaliar o estado após timeout, notificação ou wake-up espúrio.
- O cálculo da espera e a observação da fila devem ocorrer sob `InputQueueMutex`.
- Quando não houver stop nem cabeça dispatchable, o timeout solicitado deve ser positivo e não superior a 1 ms; este é o limite da espera solicitada, não uma garantia de latência end-to-end.
- A correcção não pode truncar uma duração positiva para timeout zero.
- A espera deve preservar o polling SHM configurado e não introduzir um ciclo de reespera que adie esse serviço.
- Inserções que alterem o próximo trabalho relevante devem permitir recalcular a espera.
- O pedido de shutdown deve sincronizar-se com os waiters de input e output e notificá-los sem depender da expiração do polling.
- Devem ser preservados o ordering da fila, o limite de batch 32, a transferência de retenção e a arquitectura SHM.

### Critérios de aceitação

1. Um reproducer com uma mensagem válida a pelo menos 5 segundos no futuro deve falhar em `05ffc35` e passar após a correcção: durante 2 segundos, zero pop/Run, entrada preservada e CPU inferior a 10% de um core no harness controlado.
2. O deadline deve ser respeitado sem execução antecipada e com exactamente uma execução.
3. Devem ser testados deadline vencido, igualdade, intervalo sub-ms, inserção de entrada anterior, wake-up espúrio e shutdown com fila vazia/futura.
4. A recepção SHM deve continuar funcional durante uma espera por mensagem futura.
5. Os harnesses 14/14 devem ser reexecutados após a alteração.
6. Deve passar um smoke test cross-VM antes do gate de 1000 fotos.
7. O gate 1000/1000 permanece bloqueado até todos os critérios acima e os gates originais da SPEC estarem comprovados.

### Implementação e próximos passos

A correcção inicial fica limitada ao scheduler de `GW.cpp`: espera com deadline bounded e sincronização de shutdown. A optimização/substituição do polling SHM e qualquer refactor arquitectural do scheduler ficam fora desta adenda e exigem SPEC separada.

Ordem obrigatória: aplicar patch → reproducer pre/post → testes temporais/shutdown/SHM → harnesses 14/14 → rebuild limpo nas VMs → smoke test cross-VM → gate 1000 fotos → revisão de aceitação da Fase B.

## 8. Perguntas para o Astra

1. Discriminador de codificação: sufixo na Version (`0.1+enc2`), marker novo
   (`n2`), ou campo no envelope? (ele pediu "fora do corpo"; falta escolher.)
2. Fase B: `unordered_map<handle, Message*>` vs vector+free-list — preferência
   considerando o hot path de 2000 msg/s?
3. Fase E.2: `unique_ptr<CommandLine>` quebra quem copia `Message*` sem
   deep-copy — auditamos copy do Message no C.1; confirmar aceitação.
4. O Script do ciclo de vida: após C.2, definir se o wire da Fase D precisa de
   algum campo adicional (ex.: TTL, ordem determinística) que valha a pena
   incluir no mesmo bump de codificação para evitar duas migrações.
