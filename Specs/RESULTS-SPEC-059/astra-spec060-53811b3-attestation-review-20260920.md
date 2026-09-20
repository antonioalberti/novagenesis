# Astra Review — verifier attestation seam `53811b3`

**Data:** 2026-09-20
**Modelo:** `gpt-6-astra`
**Baseline:** `36425aa`
**Candidato exact-SHA:** `53811b3`
**Estado:** `GO` exclusivamente controller-only

## Veredicto

O seam `evaluate_remote_acceptance` passa a exigir uma attestation mínima do verifier para planos `normal-payload`. O parecer é limitado ao diff e aos resultados reportados; não constitui execução independente.

### Contrato verificado

- `payload_result=None` mantém a aceitação bloqueada.
- Tipos que não sejam `Mapping` são rejeitados sem `.get` inseguro.
- `offline_verification` ausente ou não-objecto é rejeitado.
- `code` tem de ser `NGELC-VALID`.
- `integrity` e `accepted` exigem estritamente `True`.
- `schema_version` exige o inteiro `2`, rejeitando booleanos, `"2"` e `2.0`.
- Código não-zero de runtime/teardown/evidence não é substituído por `21`.
- O fluxo remoto permanece com `payload_result=None`; não foi introduzido collector/helper nem produção de bundle v2.

## Validação local reportada

- Testes focados: **13 passed**.
- Suite canónica: **256 passed, 47 subtests passed**.
- `compileall`: PASS.
- `git diff --check`: PASS.

## Limites e cobertura futura

Podem ser acrescentados testes explícitos para cada campo ausente, `False`, `accepted=1` no nível superior, `schema_version=True`/inteiro diferente de `2`, e falhas de teardown/evidence com attestation válida. Não são bloqueadores deste incremento.

Este GO não autoriza VM, SSH, QGA, cleanup, reboot, trial, push, tag ou release. Collector/helper bounded, bundle v2 efectivo, M2-R e release permanecem fora do escopo.
