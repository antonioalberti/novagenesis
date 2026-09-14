# SPEC-051: Diagnóstico de descoberta NRNCS cross-VM

**Author:** Antonio Alberti / Hermes  
**Date:** 2026-09-12  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-049-nrncs-readiness-binding-oracles.md, SPEC-050-contentapp-repository-control-plane-readback.md, SPEC-010-contentapp-hello-ipc20-bindings.md

## 1. Problema

No trial SPEC-050, a Source VM descobriu o NRNCS local via SHM, mas o Repository remoto apenas descobriu o PGCS peer. O Repository produziu:

- `Unable to send the discovery message. The domain NRNCS is still unknown`;
- `GetHTBinding function returned warning status.`

Não há ainda evidência suficiente para atribuir a causa a exposição PGCS, propagação cross-VM, armazenamento/lookup NRNCS ou timing do Repository.

## 2. Objectivo

Localizar o primeiro edge não demonstrado na cadeia:

```text
NRNCS Source
  → bindings NRNCS/NR
  → exposição PGCS Source
  → transporte cross-VM
  → lookup/armazenamento PGCS Repository
  → descoberta Core Repository
```

## 3. Âmbito

### Incluído

- Mesmo commit/configuração/topologia da SPEC-050.
- Observação separada dos bindings NRNCS/NR, exposição Source, ingress Repository e lookup ContentApp.
- GDB/log/captura targeted, sem alterar semântica.
- PGCS + NRNCS + Repository; Source ContentApp continua excluído.
- Oráculos por identidade: NRNCS HID/OSID/PID/BID, PGCS peer, categoria/chave/valor e processo consumidor.

### Excluído

- Alteração de produção antes da causa.
- Source ContentApp, payload, fotos, cache, subscriptions completas ou performance.
- Claims de release.

## 4. Critérios de aceitação

- [ ] SPEC revista/aprovada antes de instrumentação.
- [ ] Source NRNCS operational e binding identity registados.
- [ ] Source PGCS demonstra exposição/propagação do NRNCS ou a ausência é localizada.
- [ ] Bridge capture correlaciona mensagens de exposição relevantes, se emitidas.
- [ ] Repository PGCS demonstra ingress/lookup dos bindings ou primeiro edge ausente.
- [ ] Repository Core demonstra lookup falhado ou descoberta bem-sucedida com identidade.
- [ ] O resultado distingue timing, ausência de publicação, transporte, storage e lookup.
- [ ] Cleanup deixa zero resíduos do trial.
- [ ] Qualquer alteração de produção abre SPEC de implementação separada e requer Astra.

## 5. Estado inicial e evidência

- SPEC-049: NRNCS Source operational; publicação→HTStore invocation observada.
- SPEC-050: Source PGCS descobriu NRNCS via SHM; Repository não descobriu NRNCS e emitiu os warnings acima.
- Evidência: `Specs/RESULTS-SPEC-050/repository-trial/` e `repository-assessment.md`.

## 6. Resultado da comparação normal-path

Foi executado o fluxo completo com Source ContentApp e 100 JPEGs novos nos dois commits:

- `1af604d`: Source=100, NRNCS=100, Repository=100; SHA-256 iguais.
- `21a5512`: Source=100, NRNCS=100, Repository=100; SHA-256 iguais.

Nos dois commits o Repository apresentou inicialmente `NRNCS is still unknown` e `GetHTBinding function returned warning status`, mas depois registou `Discovered a NRNCS!` e recebeu os 100 payloads. A comparação não reproduziu uma regressão no fluxo normal.

Evidência: `Specs/RESULTS-MATCHED-comparison.md`.

Interpretação revista: Source e Repository são o mesmo processo ContentApp com papéis diferentes; o papel não deve ser tratado como requisito para o Repository aprender o NRNCS. A comparação normal não demonstra regressão entre os commits, mas também não explica a falha discovery-only. O primeiro alvo deve ser a cadeia local de hello IPC/SHM e os critérios das SPECs históricas, não uma dependência presumida do Source ContentApp nem um rastreamento completo de IHC/RAW. Não fazer rollback com base nesta evidência.
