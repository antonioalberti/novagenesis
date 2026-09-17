# SPEC-059: Canal privilegiado via QEMU Guest Agent para o NG-ELC

**Author:** Antonio Alberti / Hermes Agent
**Date:** 2026-09-17
**Status:** In Progress
**Branch:** AIOPT3
**Implementation commit:** `4f206eb`
**Related:** SPEC-054-release-master-plan-and-audit.md, SPEC-056-ng-elc-evidence-provenance-hardening.md, SPEC-058-ng-elc-privileged-local-profile.md
**Task linkage:** NG-056 (child of NG-050)

## 1. Decision and problem

O agente Hermes opera a partir da VM 100 através do Discord. Não é possível fornecer uma senha interativa no mesmo terminal Linux usado pelo agente, portanto `sudo -n` dentro da VM não é um canal válido para o trial `native-privileged`.

A infraestrutura atual oferece um caminho alternativo: o Proxmox host `gandalf` tem o QEMU Guest Agent ativo para a VM 100. O host pode executar um comando específico dentro do guest com `qm guest exec`; a verificação realizada em 2026-09-17 executou `/usr/bin/id -u` na VM 100 e obteve UID 0.

Esta SPEC documenta esse canal como um adaptador de execução privilegiada específico do backend Proxmox/QEMU. Ela não transforma QEMU Guest Agent em requisito do NG-ELC nem altera o contrato de virtualizações alternativas.

## 2. Scope

### Included

- Adaptador host-side Proxmox/QEMU para executar o NG-ELC dentro da VM 100.
- Verificação não destrutiva de disponibilidade do QEMU Guest Agent.
- Prova explícita de identidade privilegiada (`id -u` igual a zero).
- Captura e normalização de comando, VM ID, host, exit code, stdout, stderr e estado do guest.
- Integração do canal no método de teste e no bundle de provenance.
- Testes unitários com comandos QEMU simulados e testes de contrato do controlador.
- Documentação das pré-condições, limitações e alternativas portáveis.

### Excluded

- Alterações a C++, protocolo, pub/sub ou comportamento das VMs 101/102.
- Uso de `sudo`, password prompts, `SUDO_PASSWORD` ou bypass de autenticação.
- Execução automática de `clean.sh` nesta SPEC sem o consentimento explícito exigido pela SPEC-058.
- Dependência obrigatória de Proxmox, QEMU Guest Agent ou `qm` para outras virtualizações.
- Alteração do modo remoto SSH ou do schema-v1 existente.
- Declaração de aceitação da release; o trial continua sujeito à SPEC-058, revisão Astra e M1/G3.

## 3. Contract

### 3.1 Capability discovery

O adaptador MUST verificar, em ordem:

1. que o host Proxmox alvo é o host autorizado para a VM;
2. que `qm agent <vmid> ping` responde com sucesso;
3. que `qm guest exec <vmid> -- /usr/bin/id -u` retorna `0`;
4. que os caminhos absolutos do repositório, plano, build, IO e evidência existem ou foram criados pelo fluxo declarado.

Qualquer falha, timeout, resposta ambígua ou UID diferente de zero MUST resultar em `BLOCKED` antes de cleanup ou launch.

### 3.2 Command execution

O adaptador MUST:

- usar um vetor de argumentos, sem shell string composta;
- executar o controlador dentro do guest com o interpretador e caminhos absolutos registrados;
- preservar o `exitcode` informado pelo Guest Agent e não inferir sucesso a partir do código de transporte do SSH/QEMU;
- capturar stdout/stderr bounded e o JSON bruto de resposta do `qm guest exec`;
- persistir host Proxmox, VM ID, nome do guest, comando, timestamp, UID verificado e estado do canal;
- impedir que variáveis secretas ou conteúdo de credenciais sejam gravados na evidência.

O adaptador não deve assumir que o campo de transporte `exited` representa o código do processo; o `exitcode` do comando guest é a autoridade do resultado.

### 3.3 NG-ELC integration

A invocação de aceitação MUST continuar sendo o NG-ELC:

```text
Scripts/AlpineVMs/ng_remote_executor.py
```

Para o perfil local privilegiado, o plano e a CLI devem declarar conjuntamente:

```text
mode=local
local_profile=native-privileged
execution_channel=proxmox-qga
vmid=100
```

O controlador deve recusar a combinação `execution_channel=proxmox-qga` com modo remoto, VM não autorizada ou host não validado. O resultado deve distinguir:

- `channel_ready` — QGA responde e UID 0 foi provado;
- `preflight_pass` — configuração, perfil e paths são válidos;
- `runtime_result` — resultado do lifecycle;
- `teardown_result` — resultado do cleanup;
- `evidence_result` — selagem/verificação do bundle.

Nenhum desses estados isolados fecha M1/M2/M3.

## 4. Portability boundary

O núcleo do NG-ELC deve depender de uma interface abstrata de execução privilegiada, não de comandos `qm` embutidos no lifecycle. O adaptador Proxmox é uma implementação opcional.

Backends futuros podem fornecer a mesma interface por:

- libvirt/QEMU Guest Agent;
- VMware guest operations;
- Hyper-V PowerShell Direct ou Guest Services;
- um agente root explicitamente instalado no guest;
- uma sessão SSH Linux com autorização não-interativa previamente configurada.

Se nenhum backend suportado provar UID 0, o resultado deve ser `BLOCKED`; não é permitido cair silenciosamente em senha, sudo interativo ou execução não privilegiada para declarar o mesmo trial válido.

O bundle deve registrar `backend`, `transport`, `host`, `guest`, `vmid` quando aplicável e `capability_status`, permitindo demonstrar por que uma configuração de virtualização não é compatível sem alegar que o NG-ELC é genericamente incompatível.

## 5. Method of test

1. Congelar branch, commit, plano e paths do trial.
2. Verificar QGA ping e obter prova UID 0 com comando absoluto.
3. Executar o preflight local do NG-ELC pelo adaptador, sem cleanup.
4. Se o preflight passar, solicitar/registrar consentimento explícito para o `Scripts/Simple/clean.sh` privilegiado.
5. Executar o trial `native-privileged` bounded pelo NG-ELC.
6. Preservar logs, mapas, manifestos e respostas brutas do adaptador.
7. Verificar teardown, bundle offline e estado final de processos/IPC.
8. Submeter o diff, testes e bundle exato para revisão Astra.

O primeiro ensaio deste canal deve ser tratado como validação do adaptador e do perfil; só pode ser evidência de aceitação se satisfizer todos os critérios da SPEC-058.

## 6. Required tests

- QGA indisponível, ping timeout e VM inexistente resultam em `BLOCKED`.
- `id -u` diferente de zero resulta em `BLOCKED`.
- Exit code do guest diferente de zero não é mascarado pelo exit code do comando host.
- Respostas `qm guest exec` com stdout/stderr vazios ou truncados são registradas sem inferência.
- Shell-string injection, path relativo e VM ID não autorizado são rejeitados.
- O canal Proxmox é rejeitado em modo remoto.
- O perfil local default continua unprivileged e não requer QGA.
- O modo remoto SSH continua sem alteração.
- O preflight não chama cleanup.
- O bundle contém a prova do canal e não contém segredos.
- Uma falha do canal não inicia nenhum role nem remove IPC.
- Um teste end-to-end mockado percorre channel check → preflight → launch boundary → result recording sem chamar cleanup real.

## 7. Acceptance criteria

- [x] SPEC-059 aprovada antes da implementação.
- [x] Interface de canal privilegiado definida sem acoplar o núcleo a Proxmox.
- [x] Adaptador QGA implementado e coberto por testes RED→GREEN.
- [x] Prova UID 0, VM ID e resposta bruta persistidas com redaction segura.
- [ ] Preflight real via QGA concluído sem cleanup e com paths explícitos.
- [ ] Um trial real bounded é executado apenas após autorização explícita para cleanup.
- [ ] Bundle sealed e verificador offline passam.
- [ ] Zero processos/IPC residuais atribuíveis após teardown.
- [ ] Revisão Astra do diff e bundle exatos concluída.
- [ ] Compatibilidade e comportamento do modo remoto verificados.
- [ ] SPEC-056/058, NG-056, NG-050, Dashboard e plano mestre reconciliados sem promover o trial a release acceptance.

## 8. Risks and rollback

### Risks

- O host Proxmox torna-se uma dependência operacional do trial local.
- O Guest Agent pode retornar sucesso de transporte com falha no processo guest se `exitcode` não for preservado.
- Um comando root via QGA amplia o impacto de paths ou argumentos incorretos.
- Outras virtualizações não terão os mesmos comandos ou garantias.

### Mitigation

- Paths absolutos e validados; vetor de argumentos; VM ID allowlist.
- Prova UID 0 e interpretação exclusiva do `exitcode` guest.
- Preflight não destrutivo antes de qualquer cleanup.
- Evidência do backend e capability status.
- Fail-closed quando a capacidade não existe.

### Rollback

Desativar o adaptador QGA e manter o perfil unprivileged e o modo remoto inalterados. Preservar testes e diagnóstico do canal, mas não reutilizar um resultado sem prova de UID 0, provenance, teardown ou bundle completo.

## 9. Evidence

- Verificação de capability: `qm agent 100 ping`.
- Prova root: `qm guest exec 100 -- /usr/bin/id -u` retornou `out-data: 0`.
- Preflight inicial: bloqueado por ausência de `NG_LOCAL_REPO_PATH`, `NG_LOCAL_BUILD_PATH`, `NG_LOCAL_IO_PATH` e `NG_LOCAL_EVIDENCE_PATH`; nenhum cleanup foi executado.
- Código canónico: `Scripts/AlpineVMs/ng_remote_executor.py`.
- Plano local: `Scripts/AlpineVMs/plans/local-intra-os.example.json`.

## 10. Open decisions

- Nome e localização da interface host-side do adaptador.
- Se a configuração local será um arquivo ignorado, variáveis de ambiente controladas ou parâmetros explícitos do executor.
- Como o host Proxmox autorizado será identificado sem acoplar uma identidade pessoal ao bundle público.
- Se o primeiro trial será apenas de capability/preflight ou também de runtime após autorização de cleanup.

## 11. Recorded operator decision

Em 2026-09-17, o operador autorizou a execução de `Scripts/Simple/clean.sh` como parte do fluxo `native-privileged` da Release 1.0.0, através do canal Proxmox/QEMU Guest Agent já verificado. Esta autorização é limitada a esse fluxo de teste e não substitui as validações fail-closed, o preflight, a preservação de evidência ou a necessidade de interromper quando o contexto mudar.

## 12. Trial diagnostic finding — 2026-09-17

O trial `qga-r1-20260917` foi executado pelo NG-ELC como UID 0 e preservado em `Specs/RESULTS-SPEC-059/local-intra-os-20260917/qga-r1-20260917/`. Nenhum role foi lançado. O cleanup terminou `PASS`, mas a elegibilidade foi corretamente bloqueada por:

- `git` recusando o repositório user-owned quando executado como root (`dubious ownership`);
- build linkage ausente porque nenhum manifest/receipt foi fornecido;
- área de evidência de Repository incompleta antes do launch.

O resultado é `runtime_result=INCONCLUSIVE`, `teardown_result=UNKNOWN` e `evidence_result=INCOMPLETE`. O bundle permanece diagnóstico e não constitui aceitação. A correção do `safe.directory` e o teste de exclusão do `NG_LOCAL_EVIDENCE_PATH` foram implementados; o próximo trial deve usar um build isolado com manifest real e um bundle novo.
