# SPEC-055: NG Experiment Lifecycle Controller — modo intra-OS

**Author:** Antonio Alberti / Hermes Agent  
**Date:** 2026-09-13  
**Status:** In Progress  
**Branch:** AIOPT3  
**Implementation commit:** —  
**Related:** SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-052-remote-ssh-multi-vm-executor.md; SPEC-053-debug-log-observability-matrix.md; SPEC-054-release-master-plan-and-audit.md; SPEC-056-ng-elc-evidence-provenance-hardening.md; SPEC-057-ng-elc-canonical-entrypoint-rename.md

**Canonical tool:** `NG Experiment Lifecycle Controller (NG-ELC)` — `Scripts/AlpineVMs/ng_remote_executor.py`

## 1. Problema

O projecto possui o controlador de lifecycle `Scripts/AlpineVMs/ng_remote_executor.py`, mas a sua configuração e os seus roles estão actualmente limitados a dois guests SSH. O teste intra-OS anterior foi executado por um launcher paralelo e terminou com erro no agregador final. Mesmo tendo produzido uma observação de 5/5 ficheiros, não é uma experiência selada pelo mesmo contrato de lifecycle.

Executar cenários com ferramentas diferentes altera launch order, readiness, timeouts, cleanup, logs e classificação. Isso impede uma comparação científica consistente.

## 2. Nome canónico

O nome normativo da ferramenta passa a ser:

```text
NG Experiment Lifecycle Controller (NG-ELC)
```

O entry point actual permanece:

```text
Scripts/AlpineVMs/ng_remote_executor.py
```

`ng_remote_executor.py`, `remote executor`, `runner` e `supervisor` podem aparecer como descrições técnicas de componentes, mas o controlador integrado deve ser referido como `NG-ELC`.

## 3. Objectivo

Adicionar um modo `local` ao NG-ELC para executar cenários intra-OS com o mesmo contrato de lifecycle, evidência e veredicto do modo remoto, sem SSH, reboot de VM ou launcher manual.

## 4. Interface normativa

O modo remoto existente mantém a interface:

```bash
python3 Scripts/AlpineVMs/ng_remote_executor.py run \
  --mode remote \
  --plan Scripts/AlpineVMs/plans/L2-pgcs-only.example.json \
  --scenario L2
```

O modo local usa:

```bash
python3 Scripts/AlpineVMs/ng_remote_executor.py run \
  --mode local \
  --plan Scripts/AlpineVMs/plans/local-intra-os.example.json \
  --scenario local-intra-os
```

O modo local exige apenas configuração explícita e local:

```text
NG_LOCAL_REPO_PATH
NG_LOCAL_BUILD_PATH
NG_LOCAL_IO_PATH
NG_LOCAL_EVIDENCE_PATH
```

Todos os paths devem ser absolutos; `NG_LOCAL_EVIDENCE_PATH` não pode ficar em `/tmp`.

## 5. Contrato comum de lifecycle

Os dois modos devem produzir as mesmas fases semânticas:

```text
prepare → preflight → launch → readiness → observe → stop
→ inventory → seal → release/result
```

Diferenças permitidas:

| Aspecto | Remote | Local |
|---|---|---|
| Transporte de controlo | SSH/SCP sem PTY | subprocessos locais sem shell |
| Identidade | VM, PID, starttime, executable | PID, PGID, starttime, executable |
| Reboot | opcional, conforme plano | proibido |
| Evidência | guest persistente + cópia local | directório local durável |
| IPC | inventário/cleanup no guest | inventário/cleanup local |
| Configuração | `NG_*` remoto | `NG_LOCAL_*` explícito |

O resultado deve manter `runtime_result`, `teardown_result`, `evidence_result` e `exit_code`.

## 6. Plano local canónico

Criar:

```text
Scripts/AlpineVMs/plans/local-intra-os.example.json
Scripts/AlpineVMs/observability/scenarios/LOCAL.json
```

O plano deve fixar explicitamente:

- PGCS `-lc`;
- NRNCS;
- ContentApp Repository;
- ContentApp Source;
- ordem dos roles;
- `cwd` e paths de build/IO;
- markers de readiness;
- timeout bounded;
- workload de 5 fotos synthetic;
- oráculo Source↔Repository por nomes e SHA-256.

O modo local não deve chamar `run_*.sh`, `gnome-terminal`, `ssh`, `scp` ou `sudo`.

## 7. Oráculo de artefactos local

Além do oracle de markers, o modo local pode usar um oracle de ficheiros explicitamente declarado no plano:

```json
{
  "type": "files",
  "source": "${NG_LOCAL_IO_PATH}/Source1",
  "repository": "${NG_LOCAL_IO_PATH}/Repository1",
  "pattern": "*.jpg",
  "expected_count": 5,
  "hash": "sha256"
}
```

O oracle exige:

- contagem exacta;
- conjuntos de nomes iguais;
- mapa nome→SHA-256 igual;
- ausência de extras/faltas;
- resultado `INCONCLUSIVE` se a janela terminar sem oráculo suficiente;
- resultado `FAIL` apenas para divergência observada depois de ambos os lados estarem prontos.

## 8. Cleanup e temporários

O modo local deve:

1. inventariar processos NG e IPC antes do trial;
2. iniciar cada role em grupo de processos próprio;
3. terminar roles em ordem inversa;
4. verificar ausência por PID/PGID/executable;
5. remover apenas IPC criado pelo trial;
6. selar logs, manifestos e resultado em evidence durável;
7. remover build, IO e staging temporários apenas depois da cópia da evidência;
8. deixar uma verificação final de zero processos e zero IPC do trial.

O controlador não deve usar `killall` nem remover `/tmp` globalmente.

## 9. Critérios de aceitação

- [x] `NG-ELC` é usado no entry point, plano, SPECs, tarefa e skills pertinentes.
- [x] `--mode remote` preserva o comportamento e os testes existentes.
- [x] `--mode local` rejeita configuração incompleta ou placeholders.
- [x] O plano local é validado contra `LOCAL.json` e o perfil de observabilidade.
- [x] O modo local não executa SSH, reboot ou launcher manual.
- [x] Processos locais têm identidade e grupos bounded.
- [x] Readiness é baseada em markers, não apenas em processo vivo.
- [x] O oracle de ficheiros verifica nomes e SHA-256.
- [x] Timeout, crash e cleanup incerto produzem veredictos fail-closed.
- [x] Manifesto sela comando, commit, build, IO, config, logs, hashes e resultado.
- [x] Temporários são limpos depois do ensaio, sem apagar evidência durável.
- [x] Testes RED→GREEN cobrem o lifecycle local.
- [ ] O primeiro ensaio real é revisto por Astra antes de fechar esta SPEC.

## 10. Plano de implementação

1. Adicionar testes RED para configuração local, validação de plano, launch order, timeout, cleanup e oracle de ficheiros. **Concluído: suite NG-ELC `20/20`.**
2. Adicionar `--mode` mantendo `remote` como default explícito.
3. Implementar configuração local sem reutilizar requisitos SSH.
4. Implementar subprocess lifecycle local com grupos e logs por role.
5. Implementar inventory/cleanup local por identidade.
6. Implementar oracle local Source↔Repository.
7. Adicionar plano e cenário `local-intra-os`.
8. Executar suite existente + suite nova + dry-run real.
9. Executar primeiro ensaio local bounded com staging novo.
10. Selar evidência e obter revisão Astra. **Evidência local selada: `Specs/RESULTS-SPEC-055/local-intra-os-20260913-final/`, resultado `PASS/PASS/COMPLETE`. Auditoria posterior: `Specs/RESULTS-SPEC-054/ng-elc-audit-20260913.json`, veredicto global `BLOCKED`.**

## 11. Não-goals

- Não alterar `PGCS`, `NRNCS`, `ContentApp`, wire format ou protocolo.
- Não converter sucesso local em aceitação multi-VM.
- Não remover o modo remoto.
- Não criar um segundo supervisor paralelo.
- Não aceitar logs ou contagens sem manifest/hash.
