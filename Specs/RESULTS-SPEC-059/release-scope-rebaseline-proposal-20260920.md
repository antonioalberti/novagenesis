# Proposta de rebaseline da Release v1.0.0 — normal path versus QGA-loss

**Data:** 2026-09-20<br>
**Branch:** `AIOPT3`<br>
**HEAD de referência:** `1b21ae7`<br>
**Estado:** proposta para revisão Astra; não altera ainda o gate register normativo.

## 1. Problema de escopo

O Plano Mestre v4 declara que a v1.0.0 certifica apenas:

- Linux/AIOPT3;
- dois guests Alpine;
- executor remoto `Scripts/AlpineVMs/ng_remote_executor.py`;
- caminho normal Source → PGCS → NRNCS → Repository;
- workload novo de cinco JPEGs;
- lifecycle, provenance, bundle durável e teardown.

Contudo, o gate M1 inclui simultaneamente `NG-055`, `NG-056`, `SPEC-055`, `SPEC-056`, `SPEC-058` e `SPEC-059`, que governam o perfil local/native-privileged e o adaptador Proxmox/QGA. Isto transforma o failure path local QGA-loss numa pré-condição do caminho normal remoto, apesar de o próprio plano dizer que o modo local não é o perfil mínimo certificado.

Esse acoplamento criou um ciclo: não se executa um novo normal-path trial enquanto o failure path QGA não estiver fechado; e o failure path QGA só pode ser demonstrado com uma autoridade de recuperação que ainda não existe.

## 2. Decisão proposta

Separar o release register em três linhas independentes:

```text
M0      baseline, higiene e freeze
M1-R    deployment/lifecycle/teardown do perfil remoto v1.0
M2-R    caminho normal remoto end-to-end
M1-L    perfil local/native-privileged e QGA-loss — v1.1+
M3      revisão/tag da v1.0
```

A v1.0.0 continua sem anunciar suporte genérico ao modo local/native-privileged nem resiliência a perda QGA. O trabalho local permanece preservado e activo como backlog v1.1+, não é apagado nem declarado resolvido.

## 3. Reclassificação proposta

### Dentro do perfil v1.0.0

- `SPEC-009` — deployment suportado;
- `SPEC-046` — runner multi-VM e teardown seguro;
- `SPEC-052` — executor remoto SSH/multi-VM;
- `SPEC-054` — plano, freeze, auditoria e release;
- SPECs funcionais PGCS/NRNCS/Repository/subscription do M2;
- trial normal remoto com cinco JPEGs, mapas independentes e bundle selado.

### Fora do perfil v1.0.0; preservado para v1.1+

- `SPEC-055` — modo local;
- `SPEC-056` — hardening de evidência/provenance do modo local;
- `SPEC-058` — perfil local `native-privileged`;
- `SPEC-059` — canal Proxmox/QGA;
- `NG-055` e `NG-056` como tarefas de linha local/QGA;
- seam QGA-loss → reconciliação → teardown → seal.

A classificação é **N/A para o perfil v1.0.0, com justificação explícita**, não PASS.

## 4. Evidência histórica preservada

Os resultados não são apagados nem reescritos:

- `local-intra-os-20260913-final`: sucesso operacional local `PASS/PASS/COMPLETE`, 5 fotos e hashes Source↔Repository; aceitação formal permaneceu pendente.
- `local-intra-os-spec056-guard-20260913`: runtime/teardown/evidence observados como PASS/PASS/COMPLETE, mas `local_acceptance_eligible=false` e `exit_code=21` por provenance/build linkage.
- `qga-r17-20260918`: execução native-privileged real, quatro roles/readiness e cinco JPEGs no Source; perda QGA, Repository vazio, ausência de seal final e recuperação externa. Permanece diagnóstico do perfil local/QGA.

Esses resultados podem servir como positive controls e contexto técnico; não fecham a v1.0 nem são promovidos retroactivamente a PASS de release.

## 5. Pacote mínimo para um novo trial normal v1.0

Só depois da revisão de escopo e autorização específica:

1. congelar um SHA limpo e reproduzível;
2. confirmar build receipt/manifest ligado ao mesmo SHA e aos binários lançados;
3. executar o executor canónico em `--mode remote`, sem QGA/local-profile;
4. usar plano normal e workload novo de cinco JPEGs;
5. verificar readiness PGCS/NRNCS/Repository;
6. preservar Source, NRNCS e Repository separadamente com mapas nome/tamanho/SHA-256;
7. verificar subscription/readback declarado pelo M2;
8. executar teardown remoto por identidade e inventário final;
9. selar e reabrir o bundle para validação offline;
10. submeter o pacote exacto a Astra antes de qualquer tag/release.

Uma falha do trial continua a ser evidência diagnóstica; nenhum resultado histórico é usado para mascarar o resultado do candidato actual.

## 6. Não-goals desta rebaseline

- Não executar trial, VM, QGA, `clean.sh` ou reboot nesta etapa.
- Não alterar C++, protocolo, payload ou modo remoto.
- Não implementar o seam QGA-loss agora.
- Não fechar SPEC-055/056/058/059.
- Não declarar a release pronta.

## 7. Decisão solicitada a Astra

Rever se:

1. a separação M1-R/M2-R versus M1-L/QGA v1.1+ é coerente com o perfil mínimo já declarado no Plano Mestre;
2. a remoção de SPEC-055/056/058/059 do caminho crítico da v1.0 é uma rebaseline legítima, e não uma redução silenciosa de critérios;
3. o pacote acima é suficiente para preparar um único trial normal remoto, mantendo o perfil local/QGA explicitamente fora de escopo;
4. o Plano Mestre, NG-050, NG-056 e Dashboard devem ser actualizados em conjunto após aprovação.

Até essa revisão, o gate permanece `HOLD`; nenhuma execução real é autorizada por este documento.
