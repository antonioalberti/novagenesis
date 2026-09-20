# Astra Review — SPEC-060 `9670e53` e rebaseline A/B/C

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**Commit de implementação revisto:** `9670e53`<br>
**Documento de escopo revisto:** `release-scope-abc-rebaseline-proposal-20260920.md`<br>
**Estado:** `SPEC-060 HOLD`; `A/B/C CONDITIONAL GO documental`

## A. SPEC-060 — implementação `9670e53`

### Veredicto

**HOLD.** A remediação resolve parte dos findings, mas ainda não fecha o confinamento de escrita nem o contrato de execução bounded.

Os resultados reportados de 19 testes focados, 223 testes, 47 subtests, `compileall` e `git diff --check` são evidência reportada do incremento, não uma execução independente desta revisão estática.

### Findings melhorados

- `compare_payload_maps` rejeita expected vazio e valida registos dos dois mapas.
- Chaves JSON duplicadas são rejeitadas.
- `schema_version`, `count` e paths normalizados têm validação mais estrita.
- Divergência com evidência completa pode ser classificada como `FAIL / COMPLETE`.
- Enumeração limitada a `max_files + 1` entradas.
- Cópia rejeita destino final preexistente, incluindo symlink pendente.
- A não elegibilidade do plano legacy está expressa por helper e teste.

### Blockers remanescentes

1. `_write_json` ainda usa `Path.write_text` e pode seguir symlinks preexistentes nos JSON de expected, maps, metadata e checkpoint.
2. A validação de diretórios é check-then-use e não resiste à troca de ancestrais; é necessária abertura ancorada em descritores confiáveis, com no-follow por componente, ou boundary equivalente demonstrada.
3. O deadline não cobre toda a operação: faltam checks na enumeração, no caminho vazio, na abertura, leitura, escrita, `fsync` e nos JSON; chamadas síncronas não têm interrupção end-to-end demonstrada.
4. Ficheiro JPEG de tamanho zero pode gerar um caminho inconsistente, com erro inesperado no fluxo de preservação em vez de classificação bounded.
5. O orçamento de leitura não contabiliza todos os bytes consumidos em erros como `read-race`; deve separar bytes consumidos de bytes aceites ou documentar outra garantia agregada.
6. A ligação de `payload_acceptance_eligible` ao ponto que concede acceptance não estava demonstrada; o fluxo marker-only não podia depender apenas de uma chamada directa ao helper.

### Próximo passo admissível

Apenas outro incremento **controller-only**, com fixtures e testes negativos, seguido de nova revisão do diff exacto. Este HOLD não autoriza VM, SSH, QGA, cleanup, reboot, push, tag ou release.

## B. Rebaseline A/B/C

### Veredicto

**CONDITIONAL GO**, exclusivamente para separação de escopo e rebaseline documental.

A separação entre `A-LNORMAL`, `B-QGALOSS` e `C-RELEASE` é adequada e preserva os controlos históricos sem os promover retroactivamente a acceptance.

### Adjudicações

- Os três gates devem ser separados.
- `B-QGALOSS` pode permanecer `HOLD/NO-GO` sem bloquear um único A normal-path, condicionado à reconciliação normativa; A mantém os seus próprios requisitos de ownership, teardown, completude e seal.
- O packet A deve ser reconstruído a partir de um novo SHA exacto; `1b21ae7` é desenho histórico e `9670e53` não é automaticamente candidato autorizado.
- `N/A por perfil` só é válido após matriz de aplicabilidade rastreável; não equivale a PASS nem remove requisitos de C.
- Claims de Source/Repository file-map, Repository readback, NRNCS persistence e subscription semantics devem ser separadas; um terceiro mapa não prova automaticamente as últimas duas.
- Pré-condições de entrada devem ser separadas de acções operacionais e critérios de saída.
- Divergência completa deve ser `FAIL / COMPLETE`; completion desconhecido ou evidência insuficiente permanece não aceite; teardown ambíguo ou seal ausente nunca fecha acceptance.
- `NGELC-VALID`, `integrity=true` e `accepted=true` devem estar ligados às claims e oráculos do perfil, não usados como prova universal de escopo.

### Estado permitido

| Gate | Estado |
|---|---|
| A-LNORMAL | `OPEN / BLOCKED` — packet novo e revisão exact-SHA pendentes |
| B-QGALOSS | `HOLD / NO-GO` — independente no escopo, sem dispensa dos requisitos de A |
| C-RELEASE | `BLOCKED / NO-GO` |

Não há aprovação de candidato nem autorização de trial, M2-R ou release pública. Os controlos históricos permanecem históricos.
