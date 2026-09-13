# Investigação ContentApp — Timers, Discovery e Fluxo até Service Offer/Acceptance

**Sessão:** 2026-06-04 (sessão tarde, após NG-042-03)
**Status:** Investigação (sem código alterado ainda)
**Applicability:** As referências a PSS/GIRS/HTS neste diagnóstico descrevem a topologia legacy. O caminho normal actual usa NRNCS; reproduções da topologia antiga exigem `NG_RUNTIME_PROFILE=legacy`.
**Pedido do usuário:**
- Simplificar a fase de descoberta (INTERNO, sem remover etapas)
- Usar o que temos para avançar até Service Offer e Acceptance
- Rever timers (demora muito)
- **REGRA EXPLÍCITA DO USUÁRIO:** "no remova etapas, mantenha do hello ao publish" — preservar as 6 etapas: hello → discovery → exposition → service offer → acceptance → photo publish
- Fazer com muito cuidado (app complexa)

## 0. Princípio da Investigação (regra de ouro)

> **As 6 etapas do ciclo são SAGRADAS.** Nenhuma será removida, fundida em outra, ou tornada opcional. A "simplificação" significa:
> 1. Reduzir esperas (timers longos demais)
> 2. Identificar bloqueios escondidos (loops, lookups redundantes)
> 3. Usar o que já existe (não criar infra nova — ex: NG-042-03 hello do PGCS, NG-005 fixes de busy-wait)
> 4. Não tocar no protocolo (categorias HT, formato de mensagens) sem discussão explícita

Se uma otimização exigir mexer no protocolo, eu paro e pergunto antes de aplicar.

---

## 1. Mapa do Fluxo Atual (6 etapas — TODAS PRESERVADAS)

**Princípio:** nenhuma das 6 etapas é opcional ou removível. A "simplificação" é sobre como cada etapa é EXECUTADA, não sobre pular etapas.

```
[t=0]    Etapa 1: HELLO
         ├── PGCS envia hello 0.2 (NG-042-03 Phase 2) para cada peer conhecido
         └── NRNCS e ContentApp recebem, armazenam cat[19]/cat[20]
         │
[t=10]   Etapa 2: DISCOVERY (loop periódico)
         ├── CoreRunPeriodic01.Run() (a cada DelayBeforeRunPeriodic = 10s)
         │   ├── DiscoveryFirstStep(Intra_Domain, hash OS/ContentApp/Core/Repo/Source/Content) [+DelayBeforeDiscovery=10s]
         │   ├── DiscoverySecondStep(Intra_Domain, mesmo hash set) [+DelayBeforeDiscovery=10s]
         │   ├── DiscoveryFirstStep(Intra_OS, hash OS/PSS/PS/NRNCS/NR) [+DelayBeforeDiscovery=10s]
         │   ├── DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("PSS","PS") — get
         │   ├── Se ERROR → DiscoverySecondStep(Intra_OS) [+DelayBeforeDiscovery=10s]
         │   └── Se OK → store PSTuples, transition to "operational"
         │
[t=??]   Etapa 3: EXPOSITION (1x no lifecycle)
         ├── Core::Exposition(Intra_Domain) [RunExpose=true → false]
         ├── Publish cat 5/6 bindings via -p --b
         └── State = "operational" → habilita Etapa 4
         │
[t=+60]  Etapa 4: SERVICE OFFER (Source) [DelayBeforePublishingServiceOffer = 60s]
         ├── CoreRunExpose01.Run() (publica o que Source está oferecendo)
         └── Push to NRNCS via -p --b
         │
[t=??]   Etapa 5: ACCEPTANCE (Repository) [triggered by Service Offer arrival]
         ├── CoreRunInvite01.Run() (Repository aceita o offer)
         ├── Subscribe to Source's content category
         └── Notify Source: "aceito"
         │
[t=+10]  Etapa 6: PHOTO PUBLISH (Source) [DelayBeforeANewPhotoPublish = 10s entre cada foto]
         ├── CoreRunContentPublish01.Run() (gera e envia ContentBurstSize = 200 fotos)
         ├── CoreNotifyS01 (handler de notificação de publicação, usado para confirmar)
         └── Repository recebe, armazena
```

**Observação crítica:** o fluxo acima mostra a chamada A CADA 10s do periodic. Cada periodic dispara ~5-7 mensagens com delays adicionais de `DelayBeforeDiscovery=10s` cada. **O ciclo NÃO é "uma passada a cada 10s" — é "uma passada a cada ~30-50s"** porque cada periodic dispara múltiplas mensagens assíncronas que precisam completar antes do próximo progresso acontecer.

**As 6 etapas SAGRADAS:**
1. ✅ HELLO (PGCS → peer via SHM) — fix NG-042-03 já feito
2. ✅ DISCOVERY (ContentApp → NRNCS via PGCS) — código atual, timers a revisar
3. ✅ EXPOSITION (ContentApp anuncia seu papel) — código atual, RunExpose 1x
4. ✅ SERVICE OFFER (Source anuncia fotos) — código atual, timer de 60s
5. ✅ ACCEPTANCE (Repository aceita offer) — código atual, triggered
6. ✅ PHOTO PUBLISH (Source publica fotos) — código atual, timer 10s/foto

**O que "simplificar" pode significar (e o que NÃO pode):**
- ✅ Pode: reduzir `DelayBeforePublishingServiceOffer` de 60s para menos
- ✅ Pode: reduzir `DelayBeforeANewPhotoPublish` de 10s para menos
- ✅ Pode: fundir 5 mensagens de discover em 1-2 (mantendo a semântica)
- ✅ Pode: eliminar o `+ DelayBeforeDiscovery` redundante em passos assíncronos
- ❌ NÃO pode: pular a Etapa 2 (discovery) mesmo se o peer já foi visto antes
- ❌ NÃO pode: pular a Etapa 3 (exposition) — outros processos precisam saber
- ❌ NÃO pode: assumir Service Offer sem Etapa 5 (acceptance) explícita
- ❌ NÃO pode: mudar formato de mensagens / categorias HT sem discussão

---

## 2. Timers em Uso (defaults vs INI atual)

Lidos de `ContentApp/src/Core.cpp` (construtor, linhas 117-123) e `ContentApp/src/CoreRunInitialize01.cpp` (parser, linhas 199-252).

| Parâmetro | Default (Core.cpp) | App.ini (Source1/Repository1) | Skill `contentapp-nrncs-discovery.md` diz | Efeito |
|---|---|---|---|---|
| `DelayBeforePublishingServiceOffer` | **1 s** | **60 s** | 20 s | Quando publicar o service offer após "operational" |
| `DelayBeforeDiscovery` | **3 s** | **10 s** | (não mencionado) | Delay entre cada step de discover (1ª e 2ª) |
| `DelayBeforeRunPeriodic` | **40 s** | **10 s** | 5 s | Intervalo do loop periódico |
| `DelayBeforeANewPeerEvaluation` | **5 s** | **5 s** | 5 s | Intervalo de evaluate de peers |
| `DelayBeforeANewPhotoPublish` | **0.5 s** | **10 s** | 5 s | Intervalo entre cada foto publicada |
| `ContentBurstSize` | 50 | 200 | (não mencionado) | Tamanho do burst de conteúdo |

**Inconsistências:**
- Skill desatualizada (5s vs 10s real para RunPeriodic)
- INI tem `DelayBeforePublishingServiceOffer = 60s` — EXTREMAMENTE conservador. Default no código é 1s. Esse é provavelmente o gargalo #1.
- `DelayBeforeANewPhotoPublish = 10s` para 200 fotos = ~33 minutos só para a fase de publish. Default código é 0.5s.

---

## 3. Bottlenecks Identificados

### 🔴 B1. `DelayBeforePublishingServiceOffer = 60s` (INI)
- **Sintoma:** após "operational", espera 60s antes de publicar service offer.
- **Análise:** O default no `Core.cpp` é 1s. O INI subiu para 60s — provavelmente de uma época em que havia race conditions. Hoje (2026-06-04) com NG-042-03 e NG-042-05 aplicados, talvez o conservadorismo não seja mais necessário.
- **Ciclo:** +60s no tempo total até Service Offer ser publicado.
- **Risco de mudar:** se houver race condition de HT bindings não propagados, Repository recebe offer antes de ter seus bindings criados. Mas isso é exatamente o que NG-005 (Phase 1+2) e NG-042-05 (MarkToDelete) tentaram resolver.

### 🔴 B2. `DelayBeforeANewPhotoPublish = 10s` (INI)
- **Sintoma:** 200 fotos × 10s entre cada = 2000s = **33 minutos** só para a fase de publish.
- **Análise:** O default no código é 0.5s. 0.5s × 200 = 100s = ~2 minutos. Mesmo conservador (2s) daria ~7 minutos.
- **Risco de mudar:** se o `WriteToSharedMemory3` retry loop não acompanhar, mensagens podem ser silenciosamente descartadas. Mas com NG-005 Phase 1+2 aplicado, isso deve estar OK.

### 🟡 B3. `DelayBeforeDiscovery = 10s` aplicado em AMBOS os steps
- **Sintoma:** cada periodic agenda **5-6 mensagens de discover** com +10s cada. Então o `DelayBeforeDiscovery` é aplicado 5-6× por ciclo de 10s.
- **Análise:** O `Core::DiscoveryFirstStep` e `Core::DiscoverySecondStep` ambos chamam `Run->SetTime(GetTime() + DelayBeforeDiscovery)`. Isso adiciona 10s DE CADA step, sequencialmente.
- **Cálculo:** 5 steps × 10s = 50s para completar UMA rodada de discovery. Aí o próximo periodic roda, faz outros 5 steps, etc.
- **Risco de mudar:** se as mensagens de discover chegarem antes dos bindings serem propagados, falham com "Not aware on Cat 5/6". Mas isso é exatamente o problema que Phase 2 (NG-042-03) tenta resolver: o peer (PGCS) já tem os bindings necessários quando o discover é processado.

### 🟡 B4. `DelayBeforeRunPeriodic = 10s` × discovery steps (acumulado)
- **Sintoma:** entre cada `RunPeriodic`, há 10s. Mas CADA periodic dispara 5+ discovers com +10s cada. Resultado: o conteúdo só progride a cada ~30-50s, não a cada 10s.
- **Análise:** olhando o log do ContentApp do usuário (turn anterior), o `RunPeriodic` aparece a cada 10s (`t=4997.846, t=5007.75, t=5017.75, t=5027.79` — confirmado 10s entre periodic). Mas o discovery só completa a cada 5-6 ciclos.
- **Cálculo:** periodic(10s) + 5 discover(50s) = ~60s para uma "rodada completa" de discovery.

### 🟢 B5. Intra_Domain vs Intra_OS — duas passadas de discovery
- **Sintoma:** o periodic faz discovery em DOIS escopos: Intra_Domain (procurando NRNCS em todo domínio) e Intra_OS (procurando PGCS no mesmo OS).
- **Análise:** o Intra_Domain é mais lento (precisa do NRNCS responder). O Intra_OS é local e rápido. Manter ambos é correto, mas podem ser otimizados.
- **Otimização possível:** priorizar Intra_OS primeiro (é local, mais rápido), depois Intra_Domain. Ou fundir em uma única mensagem com ambos os lookups.

### 🟢 B6. `RunExpose = true` no Core e set `false` após primeira exposition
- **Sintoma:** a exposition roda apenas UMA vez no lifecycle do app. Não é periódico.
- **Análise:** isso é correto — a exposition é para anunciar SEU serviço, não para re-anunciar a cada N segundos. Mas: se o peer for reiniciado, perdemos a oferta. Talvez precise de re-exposição periódica.
- **Pergunta para o usuário:** "Re-exposição periódica faz sentido? Ou 'operational' é estável o suficiente?"

---

## 4. Proposta de Otimização (3 níveis — SEM remover etapas)

> **REGRA:** Todos os 3 níveis MANTÊM as 6 etapas (hello → discovery → exposition → service offer → acceptance → photo publish). A diferença entre níveis é QUANTO DE CÓDIGO é tocado e QUANTO TEMPO economizado.

### 🅰️ Nível Conservador (mínimo disruption — só INI)
**Princípio:** mudar APENAS o INI, sem mexer no código. As 6 etapas continuam idênticas, só com timers menores.

- `DelayBeforePublishingServiceOffer`: 60 → **5** (default código é 1, mas 5 dá margem)
- `DelayBeforeANewPhotoPublish`: 10 → **1** (10× mais rápido; ainda conservador)
- Manter todos os outros timers como estão
- **Ciclo esperado:** ~30-50s para Service Offer (vs 90-120s atual)
- **Ciclo esperado para 200 fotos:** 200 × 1s = 200s = 3.3 min (vs 33 min)
- **Risco:** baixo — se aparecer race, sobe os valores de volta
- **Arquivo a alterar:** `IO/Source1/App.ini` e `IO/Repository1/App.ini`

### 🅱️ Nível Moderado (mexer em código + INI)
**Princípio:** além dos timers, fundir as 5-6 mensagens de discover em 1-2 (mantendo a semântica das 6 etapas).

- Tudo do nível A
- **Modificar `CoreRunPeriodic01.cpp`:** fundir as 5-6 mensagens de discover em **1-2 mensagens** (uma Intra_Domain, uma Intra_OS), cada uma fazendo múltiplos lookups em paralelo. Mantém 100% da semântica.
- **Modificar `Core.cpp`:** remover o `+ DelayBeforeDiscovery` em `DiscoverySecondStep` (já é assíncrono, não precisa de delay extra). Mantém o delay no `DiscoveryFirstStep` para o caso de cold-start.
- **Ciclo esperado:** ~10-20s para Service Offer
- **Risco:** médio — precisa testar bindings cruzados, mensagens fora de ordem
- **Arquivos:** `IO/*/App.ini`, `ContentApp/src/CoreRunPeriodic01.cpp`, `ContentApp/src/Core.cpp`

### 🅲 Nível Agressivo (refator)
**Princípio:** refator mais profundo, ainda mantendo as 6 etapas.

- Tudo do nível B
- **Refatorar o sistema de timers:** usar um único `AppPeriodicTimer` no Core e calcular todos os outros a partir dele (em vez de cada action ter seu próprio delay)
- **Eliminar `DiscoveryFirstStep` / `DiscoverySecondStep` separados** — fazer um único `RunDiscovery` que decide qual passo baseado no estado. Mantém Etapa 2.
- **Re-exposição periódica** se o peer for perdido (refator RunExpose). Mantém Etapa 3.
- **Ciclo esperado:** ~5-10s para Service Offer
- **Risco:** alto — refator grande, requer re-testar tudo
- **Arquivos:** além dos do nível B, `ContentApp/src/Core.h`, `ContentApp/src/CoreRunDiscover01.cpp`

---

## 5. Plano de Implementação Recomendado

**Recomendação:** começar com **Nível A** (apenas INI), validar que funciona, depois avançar para Nível B se necessário.

### Passo 1: Validar premissas (sem mudança)
- [ ] Confirmar com o usuário se ele quer re-exposição periódica (B6) ou manter "uma vez na vida"
- [ ] Confirmar se NG-005 Phase 1+2 e NG-042-05 MarkToDelete estão estáveis o suficiente para baixar os timers (B1, B2)

### Passo 2: Nível A (mínimo)
- [ ] Editar `IO/Source1/App.ini` e `IO/Repository1/App.ini`
- [ ] Documentar os novos valores em wiki `concepts/contentapp-timers.md` (criar)
- [ ] Atualizar skill `contentapp-nrncs-discovery.md` com valores atualizados
- [ ] Atualizar Obsidian `NG-042-07-contentapp-timers.md` com rationale
- [ ] Compilar (não precisa — só INI)
- [ ] Testar cross-process (4 processos) e medir tempo até primeira foto
- [ ] Se OK: commit. Se não: aumentar valores de volta

### Passo 3 (condicional): Nível B
- [ ] Só avançar se o Nível A ainda for lento
- [ ] Fazer refator de `CoreRunPeriodic01` e `Core.cpp`
- [ ] Re-testar

### Passo 4 (não recomendado para esta sessão): Nível C
- [ ] Requer mais investigação; deixar para sessão futura

---

## 6. Próximos Passos Imediatos

1. **Aguardar resposta do usuário** sobre:
   - B6: Re-exposição periódica? (sim/não)
   - B1: Pode baixar DelayBeforePublishingServiceOffer de 60 para 5?
   - B2: Pode baixar DelayBeforeANewPhotoPublish de 10 para 1?
   - Nível A, B ou C?

2. **Enquanto aguarda:** nenhum código alterado. Investigação documentada.

---

## 7. Anexos (referências consultadas)

- `ObsidianVault/Gandalf-Wiki/concepts/contract-communication.md` (a verificar)
- `ObsidianVault/Gandalf-Wiki/entities/content-app.md` (overview)
- `ObsidianVault/Gandalf-Wiki/concepts/service-discovery.md` (a verificar)
- Skill `novagenesis-dev`:
  - `references/contentapp-nrncs-discovery.md` (timing parameters table)
  - `references/contentapp-photo-transfer-failure.md` (Docker)
  - `references/contentapp-forwarding-bindings.md` (forwarding failure)
  - `references/adding-pgcs-actions.md` (action pattern)
- Código:
  - `ContentApp/src/CoreRunPeriodic01.cpp` (linhas 1-448) — main loop
  - `ContentApp/src/CoreRunInitialize01.cpp` (linhas 1-265) — ini parser
  - `ContentApp/src/CoreRunExpose01.cpp` (linhas 1-212) — service offer
  - `ContentApp/src/Core.cpp` (linhas 117-123 default timers, 514-700 discovery steps)
  - `ContentApp/src/CoreRunEvaluate01.cpp` (linhas 1-812) — peer evaluation
  - `ContentApp/src/CoreRunInvite01.cpp` (linhas 1-295) — invite
  - `ContentApp/src/CoreNotifyS01.cpp` (linhas 1-327) — notification handler
  - `IO/Source1/App.ini` e `IO/Repository1/App.ini` (timers configurados)

---

**TL;DR para o usuário:**
- Service Offer está esperando 60s (conservador demais, default código é 1s)
- Photo publish 10s entre cada foto = 33 min para 200 fotos (default código é 0.5s = 100s)
- Periodic dispara 5+ discovers com +10s cada = 30-50s por "rodada" não 10s
- Recomendação: começar mudando só o INI (Nível A, baixo risco), medir, depois decidir se precisa mexer no código
- Decisões pendentes: B6 (re-exposição periódica), valores de B1 e B2
