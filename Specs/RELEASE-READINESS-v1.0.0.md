# NovaGenesis v1.0.0 — Mapa de readiness

**Data:** 2026-09-12  
**Branch candidata:** `AIOPT3`  
**Último commit remoto verificado:** `1af604d`  
**Estado:** RELEASE BLOCKED

## Gates

| Gate | Estado | Evidência / próximo passo |
|---|---|---|
| Branch e proveniência | ⚠️ aberto | AIOPT3 está alinhada no repositório principal; alterações locais de performance ainda não classificadas |
| Build normal Alpine | ✅ demonstrado | VMs 101/102 reconstruídas no commit `1af604d`; binários coincidentes |
| Runtime PGCS on-wire | ✅ limitado | SPEC-047: 36 hellos `0x1234`, 18 por direcção, MACs correctos |
| Processamento receptor PGCS | ✅ fechado no escopo | SPEC-048: GDB demonstrou 18 entradas e 18 retornos `STATUS_OK` em cada guest |
| NRNCS/discovery/binding | ⚠️ parcial | SPEC-049 fechou readiness e storage invocation; readback/persistência continuam abertos |
| Repository cross-VM discovery | ❌ aberto | SPEC-050: Source descobriu NRNCS; Repository remoto não descobriu; SPEC-051 é o próximo diagnóstico |
| Correctness de subscriptions | ⛔ aberto | SPEC-044/045; harness funcional e matriz RED/GREEN ainda não fechados |
| Runner multi-VM | ⚠️ aberto | SPEC-046; supervisor local funciona, cleanup normal requer aceitação automatizada e readiness completo |
| Higiene/provenance pública | ⚠️ aberto | SPEC-038 ainda consta como `Proposal` e requer reconciliação formal dos critérios |
| Performance Common | ⏸ separado | SPEC-039–043 não deve ser misturada silenciosamente no commit de release |
| Revisão Astra | 🔄 contínua | Necessária antes de cada alteração material e antes da aceitação final |
| Tag/release pública | ❌ bloqueada | Só depois de todos os gates funcionais, sanitizer, documentação, hygiene e provenance |

## Critério de release

A tag `v1.0.0` só pode ser criada quando:

1. SPEC-044/045 demonstrarem correctness funcional observável.
2. SPEC-046 tiver runner remoto, readiness e teardown automatizados aceites.
3. SPEC-048 fechar o processamento receptor e o trial seguinte justificar a introdução de NRNCS.
4. A higiene pública/provenance estiver formalmente encerrada.
5. O commit candidato estiver congelado, limpo ou com todas as alterações explicitamente incluídas.
6. Builds e runtime forem repetidos a partir desse commit.
7. Astra fizer revisão final do diff, evidências e limitações.
8. As release notes distinguirem runtime normal, correctness, sanitizer, performance e teardown.

## Regra de não regressão

Resultados `100/100` de entrega normal não fecham automaticamente subscriptions, caminhos de erro, lifecycle, receiver handling ou teardown. Cada claim deve manter o seu próprio gate e artefacto de evidência.
