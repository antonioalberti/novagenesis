# SPEC-038 — Validação G1 de higiene e proveniência

**Data:** 2026-09-13  
**Branch/HEAD:** `AIOPT3` / `1af604d`  
**Modo:** auditoria local sem alterações de produção

## Hipóteses testadas

1. O tree actual não contém credenciais activas ou chaves privadas.
2. Artefactos compilados/temporários identificados pela SPEC-038 não estão rastreados.
3. Os perfis normal e legacy continuam compiláveis após a limpeza.
4. Os commits de implementação de SPEC-038 estão no candidato actual.

## Evidência observada

- Scan actual de padrões de password/token/chave privada: `0` ficheiros com credencial activa.
- Scan de chaves privadas: `0` ficheiros.
- Scan de artefactos gerados rastreados: `0` ficheiros nos padrões verificados.
- Scan de identidades de laboratório nos ficheiros activos verificados: `0` ficheiros.
- Scan histórico limitado aos padrões de credencial/chave: `0` correspondências.
- Build CMake Debug normal em `/tmp/ng-release-audit-build`: `PASS`, alvos Common, ContentApp, IoTTestApp, NBTestApp, PGCS e NRNCS.
- Build CMake Debug legacy em `/tmp/ng-release-audit-legacy`: `PASS`, alvos GIRS, HTS e PSS presentes juntamente com os alvos normais.
- Commits de limpeza ancestrais de `HEAD`: `7b430a3` e `4f83b38`.
- `Common/src/MurmurHash3.cpp.bak` foi confirmado como backup não referenciado, preservado em `/home/gandalf/workspace/novagenesis-hygiene-backup-20260913/` e removido da árvore candidata.
- `git diff --check`: `PASS` no estado observado.

## Limitações

Os paths `/home/ng/...` encontrados em `Scripts/Docker` são paths internos documentados dos containers, não paths do host; permanecem fora do scan de identidade privada. Esta evidência não prova rotação externa de uma password histórica, purge de histórico Git, nem a ausência de toda a exposição pessoal/documental possível. Esses pontos permanecem gates abertos da SPEC-038.

## Veredicto

`OPEN`: G1 avançou materialmente, mas SPEC-038 não deve ser encerrada até rotação/handoff do segredo histórico, revisão final de documentação e aceitação formal.
