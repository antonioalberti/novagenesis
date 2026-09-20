# Proposta de rebaseline A/B/C — normal path, QGA-loss e release

**Data:** 2026-09-20<br>
**Branch:** `AIOPT3`<br>
**Baseline documental da revisão Astra:** `b6fbcef`<br>
**Base declarada da proposta:** `03a9e14`<br>
**Candidato documental:** `0f410ca`<br>
**Candidato funcional:** `f1bf3bc`<br>
**Última revisão Astra:** `CONDITIONAL GO` controller-only; proposta documental condicional<br>
**Estado:** `PROPOSTA — não aprovada para trial`

## 1. Objetivo

Separar explicitamente três claims que continuam misturados nos registos da release:

- **A — A-LNORMAL:** um candidato local/native-privileged de caminho normal;
- **B — B-QGALOSS:** resiliência à perda QGA e recuperação do failure path;
- **C — C-RELEASE:** gates da release pública e do perfil distribuído remoto.

A proposta preserva os trials reais históricos como positive controls, sem os promover retroactivamente a acceptance e sem tornar B uma pré-condição automática para A.

Nenhuma parte deste documento autoriza VM, QGA, SSH real, `clean.sh`, reboot, trial, push, tag ou release.

## 2. Gate A — A-LNORMAL

**Nome:** `local-native-normal candidate`<br>
**Estado:** `OPEN / BLOCKED`<br>
**Escopo:** exactamente um candidato local com `mode=local`, `local_profile=native-privileged`, plano e CLI concordantes, quatro roles, workload determinístico de cinco JPEGs, oráculo independente Source/Repository, provenance/build linkage, teardown identity-safe e bundle schema-v2 selado.

### Governing material

- SPEC-055: lifecycle local e readiness;
- SPEC-056: provenance, build linkage, oracle, ownership, bundle e teardown aplicáveis ao candidato local;
- SPEC-058: EUID 0, paths, cleanup e perfil native-privileged;
- SPEC-059: capability/preflight e completion normal confirmado; critérios de perda QGA pertencem a B.

### Claims permitidos

A pode demonstrar apenas o caminho normal local definido pelo plano e pelo oráculo. Um mapa independente Source/Repository prova apenas presença, nome, tamanho e bytes nos roots declarados; não prova por si só readback através da interface/protocolo do Repository, persistência independente no NRNCS ou subscription semantics. Cada uma dessas claims exige oráculo e gate próprios. A não demonstra automaticamente:

- QGA-loss resilience;
- multi-VM deployment ou remote-v1;
- persistência independente de NRNCS, subscription semantics ou Repository readback, mesmo que seja acrescentado um terceiro mapa: cada claim continua a exigir a evidência semântica correspondente;
- M1-R, M2-R, M3 ou a release pública `v1.0.0`.

### Falha durante A

Perda QGA, guest completion desconhecido, teardown ambíguo ou bundle não selado tornam A `INCONCLUSIVE`/`FAIL`. Não existe fallback não aprovado e não há retry automático.

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

## 4. Gate C — C-RELEASE

**Nome:** `broader public release gates`<br>
**Estado:** `BLOCKED / OPEN`<br>
**Escopo:** M0 → M1-R → M2-R → M3, incluindo deployment remoto, multi-VM lifecycle/teardown, PGCS/NRNCS/Repository, subscription/readback, hygiene, release review e tag.

Para um candidato A isolado, estes gates podem ser `N/A por perfil`, nunca `PASS`, apenas depois de uma matriz de aplicabilidade rastreável ligar cada row ao perfil, ao plano, ao SPEC governante e à claim explicitamente fora do escopo. `N/A` não dispensa requisitos de C nem converte uma ausência de evidência em aceitação. Continuam requisitos da release pública C:

- SPEC-009, SPEC-044–054 e NG-046/047/048/049/050/051/052/053/054;
- deployment SSH multi-VM, teardown remoto e payload/readback distribuído;
- provenance/build linkage e auditoria final;
- release notes, revisão final e tag.

A não pode ser chamado `v1.0.0` sem decisão explícita que redefina C e sem fechar os gates retidos.

## 5. Preparação, execução e saída de um A futuro

### 5.1 Aprovação e autorização — antes de qualquer operação

Antes de qualquer preparação, launch ou cleanup, devem existir:

- decisão A/B/C commitada e reconciliada em Plano Mestre, NG-050, NG-056, Dashboard, handoff e SPECs afectadas;
- matriz de aplicabilidade que identifica `PASS`, `OPEN`, `BLOCKED` e `N/A por perfil`, sem tratar `N/A` como aceitação;
- revisão Astra do candidato funcional exact-SHA;
- autorização operacional explícita e separada para a preparação, sem autorização implícita de trial ou release.

### 5.2 Preparação autorizada — antes do launch

Depois da autorização, e ainda antes do launch, devem ser verificados:

- checkout limpo no SHA funcional imutável; o commit documental da proposta permanece separado;
- build fora da source tree com receipt, manifest bruto/normalizado e hashes dos quatro aliases/roles lançados;
- `mode=local`, `local_profile=native-privileged` no plano e CLI, EUID 0 e paths absolutos confiáveis;
- inventário baseline de processos, SysV IPC e POSIX semáforos; `clean.sh` só pode ser executado se a baseline não estiver zero, com novo inventário obrigatório e scope fail-closed;
- IO novo e workload novo de cinco JPEGs com mapa expected nome/tamanho/SHA-256;
- Repository vazio ou namespace de Repository exclusivamente isolado, com condição e inventário equivalentes explicitamente ligados à claim;
- oráculo file-map Source/Repository obrigatório para A; readback do Repository, persistence NRNCS e subscription semantics só são obrigatórios quando reclamados, e devem aparecer na matriz quando excluídos.

### 5.3 Acções da execução — não são pré-condições

- launch PGCS → NRNCS → Repository → Source, PTY/stream/captura bounded e markers de readiness;
- recolha das origens declaradas após stop/quiescence, preservação independente e checkpoint antes de cleanup;
- teardown reverse-order identity-safe, com reconciliação de processos/IPC e sem fallback não aprovado;
- construção do manifest, verificação offline e seal terminal.

### 5.4 Critérios de saída

- completion normal confirmado; qualquer perda de transporte ou completion desconhecido permanece não aceite;
- mapas esperados e observados completos, comparáveis e coerentes com o oráculo aplicável;
- divergência comprovada com evidência completa: `runtime=FAIL`, `evidence=COMPLETE`, `accepted=false`;
- recolha ausente, instável, truncada ou insuficiente: `runtime=INCONCLUSIVE`, `evidence=INCOMPLETE`, `accepted=false`, salvo `FAIL` sticky anterior;
- teardown ambíguo, ausência de seal ou falha de cobertura/hash: nunca `accepted=true`;
- manifest/seal schema-v2, `verify_bundle` com `NGELC-VALID`, `integrity=true` e `accepted=true` apenas quando ligados às claims do perfil A;
- resultado final, inventories e artefactos preservados lidos de volta e ligados ao SHA funcional exacto.

A packetização anterior baseada em `1b21ae7` é apenas desenho histórico: não deve ser executada sem novo freeze, porque a branch contém commits posteriores de documentação e hardening.

## 6. Positive controls preservados

- `local-intra-os-20260913-final`: sucesso operacional local histórico;
- `local-intra-os-spec056-guard-20260913`: runtime/oracle positivo com acceptance veto explícito;
- `local-intra-os-spec056-final-20260913`: repetição operacional com blockers de provenance;
- `qga-r17-20260918`: roles/readiness/Source observados, mas perda QGA, Repository vazio e ausência de seal;
- `g2-e7-canonical-20260914`: lifecycle remoto marker-only, sem prova de payload independente.

## 7. Decisão solicitada a Astra

Rever esta proposta exacta e responder:

1. se A-LNORMAL, B-QGALOSS e C-RELEASE devem ser gates separados;
2. se B pode permanecer `HOLD/NO-GO` sem bloquear um único A normal-path;
3. se as claims e não-claims de A estão suficientemente fechadas;
4. se o packet A deve ser reconstruído a partir de um novo SHA exacto, e não de `1b21ae7`;
5. quais pré-condições devem ser obrigatórias antes de qualquer autorização operacional.

**Veredicto actual:** proposta não aprovada; release continua `NO-GO`; trial continua `HOLD`.
