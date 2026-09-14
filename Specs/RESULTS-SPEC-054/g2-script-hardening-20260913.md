# G2 — Hardening dos scripts de build/deployment

**Data:** 2026-09-13  
**Tarefa:** NG-046  
**SPEC:** SPEC-009  
**Estado:** implementação e verificação local concluídas; build/smoke remoto no candidato congelado ainda pendente.

## Alterações verificadas

- Os cinco launchers usam `gdb -batch -return-child-result`, para que crash/exit não seja mascarado por `rc=0` do GDB.
- Cada launcher verifica no guest a branch `AIOPT3`, `git rev-parse HEAD` e o receipt `$NG_BUILD_PATH/.ng-build-receipt` antes de iniciar o binário.
- `pull-and-build-vms.sh` exige explicitamente `NG_SSH_KEY`, `NG_SSH_USER`, `NG_SSH_KNOWN_HOSTS` e `NG_EVIDENCE_PATH`.
- O builder resolve um único SHA de `origin/AIOPT3` via `git ls-remote`, faz `fetch` e fast-forward explícito para esse SHA nos dois guests e verifica a igualdade antes/depois do build.
- O builder grava `repository-build.log`, `source-build.log`, `build-metadata.txt` e o receipt de build; não usa `git stash`, `rm -rf` ou `clean.sh` implícito.
- Os dois builds correm supervisionados com `wait -n -p`; falha/interrupção cancela o outro PID.
- README, SPEC-009, plano mestre, Dashboard, NG-046 e NG-050 foram reconciliados para separar smoke diagnóstico, aceitação canónica e G9.

## TDD e verificação

1. Teste RED novo: `Scripts/AlpineVMs/tests/test_g2_script_contract.py` — **4 falhas esperadas** antes da implementação:
   - ausência de `-return-child-result`;
   - ausência de receipt/proveniência nos launchers;
   - ausência de SHA único no builder;
   - defaults de SSH e falta de evidência/cancelamento.
2. GREEN focalizado: **4 passed**.
3. Suíte completa: **76 passed, 7 subtests passed** em 8,16 s.
4. `check-run-scripts.sh`: **Errors: 0**.
5. `bash -n`: os seis scripts passaram.
6. `git diff --check`: passou.
7. `py_compile`: passou para `ng_remote_executor.py`, `evidence_verifier.py` e `local_provenance.py`.

## Harness local sem rede

Foi usado um `ssh` falso; nenhuma VM ou guest foi alterada.

- Cinco launchers executados com paths contendo espaços e apóstrofos:
  - `run_PGCS_on_Source_VM.sh` — rc 0;
  - `run_PGCS_on_Repo_VM.sh` — rc 0;
  - `run_NRNCS_on_Source_VM.sh` — rc 0;
  - `run_Repository_on_Repo_VM.sh` — rc 0;
  - `run_Source_on_Source_VM.sh` — rc 0.
- O harness confirmou cinco comandos com `-return-child-result` e cinco verificações de provenance.
- Builder em sucesso simulado:
  - SHA fixado: `0123456789abcdef0123456789abcdef01234567`;
  - dois scripts remotos receberam o mesmo SHA;
  - logs duráveis e metadata foram criados;
  - rc 0.
- Builder com Source a falhar (`rc=7`):
  - Repository foi cancelado (`rc=143`);
  - duração observada: 0,03 s, sem esperar o sleep de 20 s do mock;
  - rc final 1.

## Veredicto

```text
Hardening local dos scripts      = PASS
Contrato de propagação GDB      = PASS
Pinning/logging/cancelamento    = PASS no harness local
Novo build remoto pós-hardening = PENDENTE
Novo smoke remoto pós-hardening = PENDENTE
G2 formal de release             = BLOCKED por G0/freeze
G9                               = OPEN; hash mismatch histórico + smoke de uma foto
```

O smoke remoto longo anterior continua válido como diagnóstico da cadeia operacional anterior, mas não fecha o hardening actual: foi executado antes destas alterações e por launchers manuais. O próximo ensaio deve ocorrer pelo fluxo canónico, com commit congelado, receipt verificado e teardown do NG-ELC.
