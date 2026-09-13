# SPEC-046: Runner multi-VM e teardown seguro

**Author:** Antonio Alberti / Hermes
**Date:** 2026-09-12
**Status:** In Progress
**Branch:** AIOPT3
**Implementation commit:** c132e23
**Related:** SPEC-045-core-evaluate-functional-harness.md, SPEC-044-core-run-evaluate-invalid-pg-downcast.md

## 1. Problema

O teste multi-VM de 2026-09-12 produziu entrega normal de 100/100 JPEGs, mas os wrappers SSH/GDB que mantinham PGCS, NRNCS e ContentApp em foreground permaneceram activos quando o cleanup removeu os segmentos SHM. Os wrappers terminaram com exit 255 e mensagens `shmat/shmdt: Invalid argument`; o launcher Source terminou com SIGTERM.

Esses exits são interferência de teardown, mas tornam o runner ambíguo e podem contaminar a classificação de uma experiência futura.

## 2. Objectivo

Criar um runner bounded para testes multi-VM que controle explicitamente o ciclo de vida dos wrappers, processos guest, IPC e VMs, garantindo que teardown só ocorre depois de os processos consumidores terem terminado.

## 3. Âmbito

### Incluído

- Preparação e verificação de VM 101/102.
- Proveniência de branch, commit, binários, configuração e MACs.
- Lançamento na ordem PGCS→PGCS→NRNCS→Repository→Source.
- Readiness gates observáveis e captura de logs/tráfego.
- Watchdog externo com SIGTERM→espera bounded→SIGKILL apenas para identidade confirmada.
- Teardown ordenado: parar aplicações, confirmar ausência, limpar IPC, depois parar VMs.
- Preservação de exit status, logs e artefactos mesmo quando o teardown falha.

### Excluído

- Alteração do protocolo NovaGenesis.
- Alteração de `PGCS`, `NRNCS`, `ContentApp` ou wire format para facilitar testes.
- Ignorar ou normalizar exits não-zero.
- Cleanup global em hosts não pertencentes ao trial.
- Claims de performance ou correctness funcional sem oráculos próprios.

## 4. Critérios de aceitação

- [ ] Runner passa `bash -n` e `set -euo pipefail`.
- [ ] Cada processo/wrapper tem PID, VM, comando e identidade registados.
- [ ] O runner não remove IPC enquanto qualquer processo guest correspondente está vivo.
- [x] SIGTERM, espera bounded e SIGKILL são registados separadamente.
- [x] Cleanup falho produz exit não-zero e preserva evidência.
- [ ] Cleanup normal deixa 0 processos NG, 0 SHM e 0 semáforos nos guests.
- [ ] VMs só são paradas após o cleanup das aplicações e verificação final.
- [ ] Readiness distingue processo vivo de PGCS discovery, NRNCS bootstrap e ContentApp readiness.
- [x] O runner preserva os logs completos fora de `/tmp` quando necessário.
- [x] O runner é testado com child falso que ignora SIGTERM, child que termina normalmente e child que falha antes do readiness.
- [ ] Um trial real com 100 fotos mantém Source/NRNCS/Repository SHA-256 exactos.
- [x] O runner não classifica teardown interference como runtime PASS ou FAIL da SPEC testada.
- [ ] Astra revê o runner e os resultados antes de integração no fluxo de release.

## 5. Plano de trabalho

1. Congelar o contrato de lifecycle e o inventário das sessões actualmente usadas.
2. Implementar primeiro um supervisor local com children falsos.
3. Validar parsing de PID/identidade e escalada de sinais.
4. Adaptar a execução SSH/guest sem `gdb` persistente não supervisionado.
5. Executar trial multi-VM controlado e recolher evidence manifest.
6. Repetir um trial de 100 fotos com teardown ordenado.
7. Rever resultados com Astra e integrar no caminho da SPEC-045.

## 6. Decisões

| Data | Decisão | Motivo | Impacto |
|---|---|---|---|
| 2026-09-12 | Wrappers SSH devem terminar antes da remoção de IPC | Evita `shmat/shmdt Invalid argument` durante teardown | Requer supervisor externo explícito |
| 2026-09-12 | Cleanup e classificação de resultado são gates distintos | Uma entrega válida pode ter teardown inválido | Preserva evidência sem falsificar PASS |

## 7. Riscos

| Item | Impacto | Mitigação | Estado |
|---|---|---|---|
| SSH/GDB mantém sessão após processo terminar | Cleanup prematuro | Controlar wrapper e child separadamente | aberto |
| SIGKILL deixa IPC residual | Contaminação do trial seguinte | Verificação e cleanup por identidade | aberto |
| Readiness baseada apenas em `ps` | Injectar workload cedo demais | Discovery/Bootstrap gates | aberto |
| VM parada antes de recolha | Perda de logs/artefactos | Recolha antes do teardown | aberto |

## 8. Evidência inicial

- `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md`
- `Specs/RESULTS-SPEC-046/pgcs-only-20260912.md`.
- Commit publicado: `c71c191`.
- PGCS VM101/VM102: exit 255 com `shmat/shmdt Invalid argument` após cleanup.
- NRNCS/ContentApp Source/Repository: warnings equivalentes; Source launcher SIGTERM.
- Resultado normal independente: 100/100 JPEGs byte-exact Source = NRNCS = Repository.

## 9. Relação com release

A SPEC-046 é um gate de qualidade do caminho de teste, não um requisito para declarar que a entrega 100/100 ocorreu. Antes da primeira `v1.0.0`, qualquer resultado usado nas release notes deve separar explicitamente runtime normal, correctness funcional, sanitizer e teardown.
