# Pacote de preparação — trial normal remoto v1.0.0

**Data:** 2026-09-20<br>
**Branch documental:** `AIOPT3`<br>
**Baseline documental:** `462df8492ef9f0b7ec2d307950b4b36fbc1558fe`<br>
**Estado:** `PREPARADO / NÃO AUTORIZADO`

## 1. Âmbito

Este pacote prepara um único trial normal remoto para o perfil mínimo v1.0.0:

```text
M0 → M1-R → M2-R → M3
Linux/AIOPT3 / dois guests Alpine / SSH / Source → PGCS → NRNCS → Repository
```

Não inclui o perfil local/native-privileged, Proxmox/QGA, `clean.sh` via QGA ou o failure path QGA-loss. Esses itens pertencem a M1-L/v1.1+.

Nenhuma VM, SSH de trial, QGA, `clean.sh`, reboot ou processo NovaGenesis foi executado para criar este pacote.

## 2. Invocação canónica prevista

O entry point permanece:

```text
Scripts/AlpineVMs/ng_remote_executor.py
```

A forma prevista, ainda não executada, é:

```bash
python3 Scripts/AlpineVMs/ng_remote_executor.py run \
  --mode remote \
  --plan <plano-remoto-aprovado.json> \
  --scenario L5 \
  --debug-profile obs-normal \
  --trial <trial-id-novo>
```

O ambiente remoto deve ser fornecido explicitamente pelo operador, sem valores de credencial no repositório ou no pacote público:

- `NG_SSH_KEY`, `NG_SSH_USER`, `NG_SSH_KNOWN_HOSTS`;
- `SOURCE_VM_IP`, `REPO_VM_IP`;
- `SOURCE_VM_MAC`, `REPO_VM_MAC`;
- `SOURCE_VM_IFACE`, `REPO_VM_IFACE`;
- `NG_REPO_PATH`, `NG_EVIDENCE_PATH`, `NG_REMOTE_EVIDENCE_PATH`;
- `NG_REPO_VM_ID=101`, `NG_SOURCE_VM_ID=102`.

## 3. Lacuna do plano actual

`Scripts/AlpineVMs/plans/L5-e7-marker.example.json` é um plano normal não-diagnóstico, mas o seu oráculo remoto é apenas de markers:

- Source: `Publishing the content`;
- Repository: `ContentApp received payload: file=`.

O validador actual aceita `runtime_oracle.type=files` apenas em modo local; para modo remoto aceita apenas `markers`. Portanto, esse plano não fecha sozinho o requisito M2-R de mapas independentes Source/NRNCS/Repository com nome, tamanho e SHA-256.

Antes de qualquer trial de aceitação é necessária uma decisão documentada entre:

1. extensão controller-only do modo remoto para preservar e re-hashar os três mapas; ou
2. protocolo remoto aprovado de recolha dos artefactos e cálculo offline dos mapas, com o resultado ligado ao bundle/manifest/seal do trial.

Não se deve executar o trial normal e tentar resolver esta lacuna depois.

## 4. Pré-condições obrigatórias

### M0 / freeze

- [ ] SPEC-038 e SPEC-054 reconciliadas para o candidato;
- [ ] SHA candidato completo, limpo e congelado;
- [ ] build normal e Sanitizer ligados ao mesmo SHA;
- [ ] binários lançados verificados contra o build receipt;
- [ ] plano, configuração, workload e critérios congelados;
- [ ] destino de evidência durável confirmado.

### M1-R / executor remoto

- [ ] configuração privada disponível no ambiente de execução, sem ser copiada para o bundle público;
- [ ] host keys e utilizadores explicitamente fixados;
- [ ] dois guests identificados e exclusivos para o trial;
- [ ] helper remoto, leases, inventário e teardown por identidade verificados;
- [ ] baseline de processos e IPC capturada antes do launch;
- [ ] procedimento de falha SSH/timeout definido: preservar, não aceitar e não fazer fallback implícito para QGA/local.

### M2-R / caminho normal

- [ ] cinco JPEGs novos, deterministicamente identificados;
- [ ] Source, NRNCS e Repository com áreas de observação independentes;
- [ ] readiness PGCS/NRNCS/Repository definida antes da execução;
- [ ] subscription/readback definido com oráculo explícito;
- [ ] mapas independentes Source/NRNCS/Repository e cópias re-hashable preservadas;
- [ ] ausência de extras/faltas e igualdade byte-exact exigidas;
- [ ] teardown remoto e inventário final fechados antes do seal.

## 5. Critério de paragem

Parar sem retry automático quando ocorrer qualquer um destes casos:

- preflight, build linkage ou identidade de guest não comprovados;
- readiness incompleta;
- perda SSH ou ausência de completion guest;
- Source/NRNCS/Repository não observáveis independentemente;
- teardown ambíguo, residual ou inventário indisponível;
- falha de preservação, manifest, verificação offline ou seal.

O resultado deve ser preservado como `OPEN`, `INCONCLUSIVE`, `FAIL` ou `BLOCKED`, conforme o primeiro gate não provado. Nunca converter recuperação externa posterior em `teardown=PASS` do controller.

## 6. Resultado esperado do bundle

O candidato só poderá ser considerado para M2-R se o bundle exacto demonstrar:

```text
runtime_result   = PASS
teardown_result  = PASS
evidence_result   = COMPLETE
exit_code        = 0
```

E, separadamente:

- Source/NRNCS/Repository mapas completos e independentes;
- `result.json`, eventos, provenance/build receipt, inventários, cleanup, manifest e terminal seal;
- verificador offline reaberto com integridade válida;
- nenhuma aceitação inferida de markers isolados, transporte, readiness ou Source-only.

Mesmo com todos esses resultados, a aceitação final depende da revisão Astra do pacote exacto e da auditoria do Plano Mestre.

## 7. Bloqueios actuais

1. M0 permanece `BLOCKED` por SPECs/documentos abertos.
2. M1-R permanece `HOLD`; SPEC-046/SPEC-052 e o procedimento remoto ainda não estão formalmente fechados.
3. O plano remoto actual não suporta file oracle; falta decidir/implementar o caminho de mapas independentes.
4. A configuração privada e a autorização operacional de uma única tentativa ainda não foram confirmadas.
5. Nenhum trial está autorizado por este documento.

## 8. Histórico usado apenas como controlo

- `g2-l2-fce8577`: build/preflight/readiness/teardown remoto PASS, runtime INCONCLUSIVE por plano diagnóstico.
- SPEC-047/048/049: gates remotos parciais PASS com limites explícitos.
- `local-intra-os-20260913-final` e r17: resultados locais/QGA, fora do perfil remoto v1.0.

Esses resultados não substituem um novo bundle exact-SHA do perfil remoto actual.
