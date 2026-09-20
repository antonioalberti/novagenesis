# Astra Review — final fixture hardening `801e0d5`

**Data:** 2026-09-20
**Modelo:** `gpt-6-astra`
**Baseline:** `bcd9e1d`
**Candidato exact-SHA:** `801e0d5`
**Estado:** `GO` limitado aos testes controller-only da SPEC-060

## Veredicto

O diff isola os três caminhos de integridade do bundle v2:

- adulteração do hash do manifest no terminal seal → `NGELC-TAMPERED`, `integrity=false`;
- adulteração de payload com seal restaurado → `NGELC-TAMPERED`, `integrity=false`;
- payload restaurado e remoção do seal → `NGELC-TERMINAL-SEAL-MISSING`.

A etapa de fixture controller-only fica encerrada neste escopo.

## Validação local reportada

- Teste focado: **1 passed**.
- Suite canónica: **266 passed, 47 subtests passed**.
- `compileall`: PASS.
- `git diff --check`: PASS.
- Nenhuma operação runtime/VM/SSH/QGA foi executada.

## Limite

A restauração do payload usa o literal conhecido da fixture; capturar/restaurar os bytes originais seria uma melhoria de robustez não bloqueante. O fixture prova preservação, manifest v2, terminal seal e verificação offline de bytes sintéticos; não prova recolha remota nem collector de produção.

O próximo passo continua exclusivamente controller-only. Este parecer não autoriza VM, SSH, QGA, cleanup, reboot, trial, push, tag ou release.
