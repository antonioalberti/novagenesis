# G2 — Validação estática dos scripts Alpine

**Data:** 2026-09-13  
**Branch:** AIOPT3  
**HEAD observado:** `1af604dad66148618cc0e578c28adb4a9829e607`  
**Escopo:** NG-046 / SPEC-009; validação local sem iniciar VMs, SSH ou processos NovaGenesis

## Verificações executadas

1. `bash -n` nos cinco launchers correntes, em `pull-and-build-vms.sh` e em `check-run-scripts.sh`.
2. `bash Scripts/AlpineVMs/check-run-scripts.sh`.
3. Execução fail-closed de `pull-and-build-vms.sh` num ambiente sem configuração.

## Resultado observado

- Os sete scripts retornaram `rc=0` no `bash -n`.
- `check-run-scripts.sh` validou os cinco `run_*.sh` com `Errors: 0`.
- Sem `ng-vm.env`, `pull-and-build-vms.sh` recusou a execução na variável `REPO_VM_IP`, com `rc=1`.
- Não foram iniciadas VMs, ligações SSH, builds nos guests ou processos NovaGenesis.
- Não foram executados cleanup, reboot, commit, push ou tag.

## Veredicto

```text
G2 static/script checks = PASS
G2 remote preflight      = BLOCKED
G2 smoke E2E             = BLOCKED
G2 overall               = BLOCKED
```

## Bloqueio exacto

A configuração privada local e as host keys dedicadas exigidas pelo fluxo de deployment não foram encontradas nos caminhos documentados. O único ficheiro disponível é `Scripts/AlpineVMs/ng-vm.env.example`, que contém placeholders e não pode ser usado para execução.

O próximo passo permitido é o operador disponibilizar localmente, fora do Git, a configuração preenchida e as host keys verificadas. Depois disso, deve-se executar o preflight read-only, confirmar branch/build/proveniência nos dois guests e só então executar o smoke E2E na ordem canónica.

A ausência de configuração foi tratada como bloqueio; nenhum valor de rede, MAC, chave ou credencial foi inferido a partir de documentação histórica.
