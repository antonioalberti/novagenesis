# Astra Review — descriptor ownership `d2b9add`

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**Baseline:** `8e47c78`<br>
**Candidato exact-SHA:** `d2b9add24773b199e6b502af912ed1f6aa0ba609`<br>
**Estado:** `CONDITIONAL GO controller-only`

## Veredicto

O incremento resolve o HOLD anterior relativo ao ownership do descriptor `child` quando o deadline expira depois da abertura de um componente de directório. Não foi identificado novo bloqueador no fluxo corrigido.

### Verificações

- `create=False`: `child` é fechado no ramo de timeout e `current` é fechado pelo handler exterior.
- `create=True`: o mesmo ownership é aplicado depois de `mkdir` e abertura do componente.
- Sucesso: ownership é transferido explicitamente com `current = child; child = None` antes de fechar o descriptor anterior.
- Escritas JSON e cópia têm testes dirigidos para expiração imediatamente depois da abertura do destino.
- Deadlines permanecem cooperativos, sem garantia hard real-time.
- Nenhuma integração de payload em produção foi introduzida.

## Validação local exact-SHA

- Commit confirmado: `d2b9add24773b199e6b502af912ed1f6aa0ba609`.
- SPEC-060 focada: **40 passed**.
- Suite canónica: **244 passed, 47 subtests passed**.
- `compileall`: PASS.
- `git diff --check`: PASS.
- Worktree: limpo.

## Gaps não bloqueantes identificados por Astra

1. Os testes aceitam qualquer `OSError` ao verificar fecho; `errno.EBADF` seria uma asserção mais específica.
2. O teste cobre directamente o fecho de `child`; poderia também proteger explicitamente o fecho do descriptor pai.
3. A contagem anterior reportada de 36 testes foi reconciliada: o candidato exacto tem 40 testes SPEC-060 após os quatro casos de ownership JSON/cópia/directório.

## Limites

O parecer não autoriza VM, SSH, QGA, cleanup, reboot, push, tag, trial, integração produtiva ou release. A aceitação de payload, M2-R e a release pública permanecem abertas.
