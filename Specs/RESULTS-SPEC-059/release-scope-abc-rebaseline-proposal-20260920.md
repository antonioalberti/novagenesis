# Proposta de rebaseline A/B/C — normal path, QGA-loss e release

**Data:** 2026-09-20<br>
**Branch:** `AIOPT3`<br>
**Baseline documental da revisão Astra:** `b6fbcef`<br>
**Base declarada da proposta:** `03a9e14`<br>
**Candidato documental:** `0f410ca`<br>
**Candidato funcional:** `f1bf3bc`<br>
**Última revisão Astra:** `CONDITIONAL GO` controller-only; proposta documental condicional<br>
**Estado:** `REBASELINE APROVADA DOCUMENTALMENTE — sem autorização operacional`

## 1. Objetivo

Separar explicitamente três claims que continuam misturados nos registos da release:

- **A — A-RDEMO-LOCAL:** um demonstrador local de caminho normal, limitado ao perfil experimental R0–R7;
- **B — B-QGALOSS:** resiliência à perda QGA e recuperação do failure path;
- **C — C-ENGINEERING-REMOTE:** gates da release de engenharia pública e do perfil distribuído remoto.

A proposta preserva os trials reais históricos como positive controls, sem os promover retroactivamente a acceptance e sem tornar B uma pré-condição automática para A.

Nenhuma parte deste documento autoriza VM, QGA, SSH real, `clean.sh`, reboot, trial, push, tag ou release.

## 2. Gate A — A-RDEMO-LOCAL

**Nome:** `NovaGenesis Research Demonstrator 0.1 — Local Normal Path`<br>
**Identificador proposto:** `v0.1.0-research.1-AIOPT3`<br>
**Estado:** `OPEN / BLOCKED`<br>
**Escopo:** exactamente uma experiência local bounded, ligada a um exact-SHA e plano congelado, com workload determinístico de cinco JPEGs, oráculo independente expected/Source/Repository, provenance/build linkage, teardown identity-safe e bundle schema-v2 selado. O demonstrador não depende de QGA, retry, fallback ou injecção de falha.

### Governing material

- SPEC-055: lifecycle local e readiness;
- SPEC-056: provenance, build linkage, oracle, ownership, bundle e teardown aplicáveis ao candidato local;
- NG-060: perfil reduzido R0–R7 do demonstrador;
- SPEC-058/059: permanecem na linha M1-L/B-QGALOSS e não são pré-requisitos do RDEMO.

### Claims permitidos

A pode demonstrar apenas uma demonstração integrada de laboratório numa configuração Linux local específica: cinco JPEGs novos, igualdade byte-exact Source↔Repository, lifecycle observado e bundle verificável. Um mapa independente Source/Repository prova apenas presença, nome, tamanho e bytes nos roots declarados. A não demonstra automaticamente:

- três origens Source/NRNCS/Repository;
- persistência independente de NRNCS, subscription semantics ou Repository readback;
- multi-VM deployment, remote-v1, performance ou QGA-loss;
- M0, M1-R, M2-R, M3 ou a release pública `v1.0.0`.

Para A-RDEMO, três origens, readback Repository, subscriptions completas, multi-VM e QGA-loss são `N/A`/diferidos. `N/A` não é `PASS` implícito.

### Falha durante A

Perda/ambiguidade de transporte local, completion desconhecido, teardown ambíguo ou bundle não selado tornam A-RDEMO `INCONCLUSIVE`/`FAIL`. Não existe fallback não aprovado e não há retry automático.

## 3. Gate B — B-QGALOSS

**Nome:** `QGA-loss resilience and failure path`<br>
**Estado:** `HOLD / NO-GO`<br>
**Escopo:** partial/no output, completion desconhecido, reconciliação bounded, autoridade guest-local, journal durável, lease/fencing, ownership recuperável, teardown automático e seal terminal.

B continua aberto e não é fechado pelos helpers de raw transport, pelo verifier ou por um catch de `QGAChannelError` dentro de `run_local_trial`.

### Condições de saída de B

1. amendment normativa de SPEC-059 para a boundary C+D;
2. journal, lease/fencing, deadlines e janela spawn→journal definidos;
3. testes controller-only RED→GREEN para partial/no output, ambiguous dispatch, no retry de `run`, stale completion, fencing, restart, phase-specific loss e seal;
4. revisão Astra do diff exacto;
5. failure-path trial separado, somente com autorização explícita posterior.

### Relação com A

B não é pré-requisito de A quando A declara continuidade normal do transporte. Porém, qualquer perda durante A é não-aceite; A não pode usar B como atalho de recuperação.

## 4. Gate C — C-ENGINEERING-REMOTE

**Nome:** `NovaGenesis Engineering Release 1.0 — Remote Normal Profile`<br>
**Estado:** `BLOCKED / OPEN`<br>
**Escopo:** M0 → M1-R → M2-R → M3, incluindo deployment remoto, multi-VM lifecycle/teardown, PGCS/NRNCS/Repository, subscription/readback, hygiene, release review e tag. C é um marco posterior e não é pré-requisito operacional do RDEMO.

Para um candidato A-RDEMO isolado, estes gates podem ser `N/A por perfil`, nunca `PASS`, apenas depois de uma matriz de aplicabilidade rastreável ligar cada row ao perfil, ao plano, ao SPEC governante e à claim explicitamente fora do escopo. `N/A` não dispensa requisitos de C nem converte uma ausência de evidência em aceitação. Continuam requisitos da release de engenharia C:

- SPEC-009, SPEC-044–054 e NG-046/047/048/049/050/051/052/053/054;
- deployment SSH multi-VM, teardown remoto e payload/readback distribuído;
- provenance/build linkage e auditoria final;
- release notes, revisão final e tag.

A-RDEMO não pode ser chamado `v1.0.0` sem decisão explícita que redefina C e sem fechar os gates retidos.

## 5. Preparação, execução e saída de um RDEMO futuro

### 5.1 Aprovação e autorização — antes de qualquer operação

Antes de qualquer preparação, launch ou cleanup, devem existir:

- decisão A/B/C commitada e reconciliada em Plano Mestre, NG-050, NG-060, Dashboard, handoff e SPECs afectadas;
- matriz de aplicabilidade que identifica `PASS`, `OPEN`, `BLOCKED` e `N/A por perfil`, sem tratar `N/A` como aceitação;
- revisão Astra do candidato funcional exact-SHA;
- autorização operacional explícita e separada para a preparação, sem autorização implícita de trial ou release.

### 5.2 Preparação autorizada — antes do launch

Depois da autorização, e ainda antes do launch, devem ser verificados:

- checkout limpo no SHA funcional imutável; o commit documental permanece separado;
- build fora da source tree com receipt, manifest bruto/normalizado e hashes dos binários lançados;
- `mode=local` no plano e CLI, sem QGA, retry, fallback ou injecção de falha;
- inventário baseline de processos/IPC e paths absolutos confiáveis;
- IO novo e workload novo de cinco JPEGs com mapa expected nome/tamanho/SHA-256;
- roots Source e Repository exclusivamente isolados, com condição e inventário equivalentes ligados à claim;
- oráculo file-map Source/Repository obrigatório; NRNCS, readback, subscriptions, multi-VM e QGA-loss entram na matriz como `N/A`/diferidos.

### 5.3 Acções da execução — não são pré-condições

- launch local pelo NG-ELC, captura bounded e completion conhecido;
- recolha Source/Repository após stop/quiescence, preservação independente e checkpoint antes de cleanup;
- teardown reverse-order identity-safe, com reconciliação de processos/IPC e sem fallback não aprovado;
- construção do manifest, verificação offline e seal terminal.

### 5.4 Critérios de saída

- completion normal confirmado; qualquer completion desconhecido permanece não aceite;
- mapas expected/Source/Repository completos, comparáveis e coerentes com o oráculo aplicável;
- divergência comprovada com evidência completa: `runtime=FAIL`, `evidence=COMPLETE`, `accepted=false`;
- recolha ausente, instável, truncada ou insuficiente: `runtime=INCONCLUSIVE`, `evidence=INCOMPLETE`, `accepted=false`, salvo `FAIL` sticky anterior;
- teardown ambíguo, ausência de seal ou falha de cobertura/hash: nunca `accepted=true`;
- manifest/seal schema-v2, `verify_bundle` com `NGELC-VALID`, `integrity=true` e `accepted=true` apenas quando ligados às claims do perfil RDEMO;
- resultado final, inventories e artefactos preservados lidos de volta e ligados ao SHA funcional exacto.

A packetização anterior baseada em `1b21ae7` é apenas desenho histórico: não deve ser executada sem novo freeze, porque a branch contém commits posteriores de documentação e hardening.

## 6. Positive controls preservados

- `local-intra-os-20260913-final`: sucesso operacional local histórico;
- `local-intra-os-spec056-guard-20260913`: runtime/oracle positivo com acceptance veto explícito;
- `local-intra-os-spec056-final-20260913`: repetição operacional com blockers de provenance;
- `qga-r17-20260918`: roles/readiness/Source observados, mas perda QGA, Repository vazio e ausência de seal;
- `g2-e7-canonical-20260914`: lifecycle remoto marker-only, sem prova de payload independente.

## 7. Decisão aplicada e regra anti-loop

O parecer Astra de 2026-09-20 é aplicado exactamente:

1. A passa a `A-RDEMO-LOCAL`, com R0–R7 e identificador experimental `v0.1.0-research.1-AIOPT3`.
2. B permanece `HOLD / NO-GO` e não bloqueia o primeiro caminho normal local; qualquer perda durante o RDEMO invalida a experiência.
3. C passa a `C-ENGINEERING-REMOTE`, preservando M0/M1-R/M2-R/M3 e a release futura `v1.0.0-AIOPT3`.
4. SPEC-060/NG-058 permanecem no caminho remoto de engenharia; controller-only/fixtures estão concluídos, mas a integração collector/helper remota continua `OPEN`.
5. Não serão acrescentados hardening remoto, matrizes cosméticas, metas 100/1000, readback, subscriptions completas ou QGA-loss como pré-requisitos do RDEMO. Só se reabre implementação perante bloqueio concreto de R0–R7.

O replaneamento não autoriza VM, SSH, QGA, `clean.sh`, reboot, trial, push, tag, publicação ou release. A única experiência futura será uma execução local bounded, sem retry, fallback ou injecção de falha, após revisão exact-SHA e autorização operacional separada.
