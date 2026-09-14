# G2 — Preflight de deployment Alpine

**Data:** 2026-09-13  
**Ferramenta:** `Scripts/AlpineVMs/ng_remote_executor.py`  
**Cenário:** `L2` / `dry-run`  
**Modo:** sem contacto com VMs

## Observações

- VM 101: `stopped`.
- VM 102: `stopped`.
- Configuração privada `ng-vm.env`: ausente.
- Host keys dedicadas: ausentes.
- O executor recusou o plano com `CONFIG_ERROR`.
- Foram exigidos: `NG_SSH_KEY`, `NG_SSH_USER`, IPs/MACs/interfaces dos guests, paths de repositório/build/evidência e `NG_SSH_KNOWN_HOSTS`.

## Veredicto

`BLOCKED`: o preflight não pode avançar até a configuração privada ser criada localmente pelo operador e as host keys serem verificadas. Nenhum processo ou VM foi iniciado.
