# SPEC-049 — Resultado do trial NRNCS/binding

**Date:** 2026-09-12  
**Commit:** `1af604d`  
**Topology:** PGCS em VM101/VM102; NRNCS apenas na VM102 Source; sem ContentApp  
**Production code changed:** no

## Gates

| Gate | Resultado | Evidência |
|---|---|---|
| Proveniência | PASS | Preflights exactos nos dois guests |
| PGCS peer/on-wire | PASS limitado | Frames `0x1234` bidireccionais; PGCS sockets criados |
| NRNCS readiness | PASS | Marker literal `OPERATIONAL: Everything ok!`; configuração efectiva registada |
| NRNCS initial bindings | PASS observado | Sequência de `HT_STORE_CALL` durante inicialização |
| PGCS publication ingress | PASS | `NRPUB_INGRESS`: categoria `2`, chave `18A4AAFB`, valor `00B50E23` |
| HT storage invocation | PASS observado | `HT_STORE_CALL`: categoria `2`, chave `18A4AAFB`, valor `00B50E23`, argumentos iguais ao ingress |
| Binding persistence/readback | OPEN | Este ensaio observou a chamada a `HT::StoreBinding`, mas não fez lookup posterior independente |
| ContentApp/subscriptions | NOT TESTED | Correctamente excluídos desta fase |
| Teardown | PASS | Zero processos, SHM e semáforos; VMs paradas |

## Interpretação

O gate de readiness NRNCS e o gate de publicação→invocação de armazenamento foram demonstrados. A correlação é exacta para a publicação observada: categoria `2`, chave `18A4AAFB` e valor `00B50E23` aparecem no ingresso `NRPubBind01` e na chamada `HTStoreBind01`.

O resultado não prova persistência/readback após a chamada de armazenamento. Esse limite deve permanecer explícito antes de claims de binding durável.

Evidência:

- `nrncs.log`
- `repo.log`, `source.log`
- `ng049-tcpdump.txt`
- `source-cleanup.txt`, `repo-cleanup.txt`, `vm-stop.txt`
- `trial-contract.txt`
