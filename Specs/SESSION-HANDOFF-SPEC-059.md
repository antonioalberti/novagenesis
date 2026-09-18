# Handoff — SPEC-059 / QGA local-intra-os

## Retomada

- Repositório: `/home/gandalf/workspace/novagenesis`
- Branch: `AIOPT3`
- HEAD de handoff: `ce5a7869ff61733362553b3a8d5bb3a0a627cc3e`
- Estado Git no momento do handoff: limpo, à frente de `origin/AIOPT3`.
- Host Proxmox: `192.168.0.200`; VM alvo: `100`.
- Não executar novo trial NG sem revisar este documento, a SPEC e os blockers abaixo.

## Governing material

- SPEC: `Specs/SPEC-059-ng-elc-qemu-guest-agent-channel.md`
- Plano: `Scripts/AlpineVMs/plans/local-intra-os.example.json`
- Executor: `Scripts/AlpineVMs/ng_remote_executor.py`
- Provenance: `Scripts/AlpineVMs/local_provenance.py`
- Build/manifest: `Scripts/AlpineVMs/ng_observability.py`
- QGA adapter: `Scripts/AlpineVMs/proxmox_qga_channel.py`
- Relevant tests: `Scripts/AlpineVMs/tests/test_spec059_qga_channel.py`, `test_local_provenance.py`, `test_spec056_r08_remediation.py`

## O que foi corrigido e verificado

1. Provenance root-safe: `safe.directory`, exclusão controlada da evidência, `__pycache__`/`.pyc`, aliases `Repository`/`Source` em outputs, binaries e runtime identities.
2. Builds isolados fora da árvore-fonte; builds r12/r14/r15 tiveram HEAD exato, árvore limpa, sete identidades binárias/runtime e returncodes `[0,0]`.
3. QGA:
   - `qm guest exec` usa `--synchronous 1 --timeout 0`;
   - argv remoto é serializado com `shlex.join(qm_argv)`;
   - testes e smoke real validaram `sleep 35`, `false`, argumentos com espaços/metacaracteres, timeout externo e resposta sem exitcode.
4. Suíte AlpineVM mais recente: `190 passed, 7 subtests passed`; `py_compile` passou.
5. Fixture QGA real de reconciliação sem NG passou: perda após iniciar `/bin/sleep`, reconexão, leitura de PID, `kill -0`, `SIGTERM`, verificação de ausência e remoção do pidfile.
6. Fixtures sem NG cobrem processo líder/filho, identidade divergente, resposta QGA só com PID, timeout de transporte e IPC SysV estrangeiro fail-closed.

## Resultados NG importantes

- r14/r15: quatro roles lançados, readiness/discovery via SHM, cinco JPEGs no Source, Repository vazio.
- r15: espera longa e quoting SSH já corrigidos, mas teardown terminou em `stop identity-mismatch` para `Source`; QGA externo retornou `transport_exit_code=29`, `Agent error: PID ld does not exist`.
- r14/r15 deixaram 16 SHM e 16 semáforos POSIX cada. Foram removidos individualmente via QGA root, com inventário posterior zero. Isso é recuperação externa, não prova de teardown automático.
- Bundles:
  - `Specs/RESULTS-SPEC-059/local-intra-os-20260917-r14/qga-r14-20260917/`
  - `Specs/RESULTS-SPEC-059/local-intra-os-20260918-r15/qga-r15-20260918/`
- Não há aceitação de release/runtime.

## Veredictos Astra

- Revisão pós-r14: **HOLD** — QGA/teardown/provenance/entrega incompletos.
- QGA synchronous patch: **GO diagnóstico** condicional.
- SSH argv patch: **GO diagnóstico** condicional.
- Pós-r15: **HOLD** — prioridade teardown/process identity e IPC ownership antes de payload.
- Fixtures de processo/IPC/transporte: fecham caracterizações parciais, mas não o HOLD global.
- Real QGA reconciliation fixture: passou, mas não prova teardown de árvore NG nem ownership IPC seletivo.
- r17: quatro roles/readiness observados e cinco JPEGs no Source; Repository permaneceu vazio; QGA terminou com transport exit 29 / `Agent error: PID ld does not exist`; teardown automático ficou incompleto e a recuperação externa deixou inventário zero.
- Revisão controller-only r17: a corrida PTY foi isolada; o wrapper `setsid --wait --ctty` podia ser registado antes de estabilizar o PGID. O patch `ce5a786` usa `Popen(start_new_session=True)` + `TIOCSCTTY`, com teste RED→GREEN.
- Astra pós-r17: patch `CONDITIONAL_READY`, depois `READY_FOR_REVIEW_COMMIT` após inspeção de threading; SPEC-059 `DIAGNOSTIC_VALIDATED / ACCEPTANCE_PENDING`; M1 `NOT_ACCEPTED`; Release `NOT_RELEASE_READY`.

Tracker Astra: `~/CodeRepository/tool-codex-usage-tracker/`; usar a skill `codex-delegation-loop` e `tool-codex-usage-tracker`. O Codex standalone não funciona com a autenticação Hermes (`401`); usar `codex_usage_tracker.py send --model gpt-6-astra` e sempre enviar todo o contexto, pois Astra é stateless.

## Estado actual e próximo passo obrigatório

Não iniciar outro trial NG ainda. O próximo incremento deve ser revisado por Astra e deve resolver/definir:

1. validar a correcção PTY `ce5a786` numa futura execução diagnóstica, sem assumir que ela resolve o QGA;
2. como o estado QGA `Agent error: PID ld does not exist` é classificado e reconciliado;
3. confirmar teardown automático completo, seal final e persistência de JSON bruto em falhas de transporte;
4. separar Source→Repository delivery da evidência de controlo/teardown;
5. decidir a política `unattributed-ipc` all-or-nothing versus remoção selectiva, sem remover recursos estrangeiros.

Qualquer novo trial deve ser apenas diagnóstico, com baseline zero verificada, build ligado ao HEAD exato, evidência nova e cleanup atribuído; não alterar C++ NovaGenesis sem nova SPEC/autorização.
