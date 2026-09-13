# SPEC-035: NBTestApp — diagnóstico intermédio de bindings NRNCS

**Data:** 2026-09-10  
**Estado:** Proposal — teste intermédio aprovado para execução  
**Autor:** Hermes Agent + Antonio Alberti  
**Branch:** AIOPT3  
**Related:** SPEC-022, SPEC-034, NG-022

## 1. Objectivo

Testar o caminho de bindings, discovery, notifications, subscriptions e HT delivery através do NRNCS sem introduzir a complexidade de payloads de ficheiros/JPEG.

## 2. Hipótese

Se o caminho NRNCS estiver funcional, o `NBTestApp` deverá publicar bindings, receber/usar notificações, subscrever bindings e obter respostas do HT sem falhas de discovery, IPC ou routing.

## 3. Âmbito

### Incluído

- PGCS nas VMs 101/102.
- NRNCS no Source (source guest).
- `NBTestApp` como aplicação de teste, substituindo ContentApp.
- Bindings sem payload de ficheiro.
- Discovery, exposição, publicação, subscrição e entrega de bindings.
- Configuração curta e observável, sem stress massivo.
- Reinício limpo das VMs, processos e IPC.

### Excluído

- JPEGs, `NRInfoPayload01` com payload e manifests SHA-256.
- Alterações funcionais ao NRNCS, HT, routing ou GW.
- Política de colisão de filenames/payloads.
- Stress original de 200000 publicações.
- Aceitação final de NG-022.

## 4. Configuração controlada

```text
DelayBeforeDiscovery 3
DelayBeforeRunPeriodic 5
NumberOfPublications 20
NumberOfSubscriptions 20
NumberOfMessagesPerBurst 1
NumberOfPubsPerMessage 1
NumberOfHTSs 1nrncs
Trial ng022-intermediate-1
```

A configuração original do repositório (`200000/1440/200/250`) não será usada neste diagnóstico.

## 5. Critérios de aceitação diagnóstica

- [ ] `NBTestApp` compila nas VMs a partir do HEAD AIOPT3.
- [ ] PGCS e NRNCS atingem discovery/operational sem erros persistentes.
- [ ] Bindings são publicados e armazenados no NRNCS/HT.
- [ ] Notifications e subscriptions são observadas nos logs.
- [ ] Deliveries de bindings retornam sem erros de routing/IPC.
- [ ] Não há crash, falha persistente, SHM residual ou processo zombie.
- [ ] Counts de publicações/subscrições/deliveries e primeira edge não comprovada são registados.

Resultado positivo valida apenas o caminho de controlo/bindings; não valida payload de ficheiro ou `NRInfoPayload01`.

## 6. Execução

1. Aguardar/encerrar qualquer run anterior e preservar os seus logs.
2. Reiniciar VMs 101/102.
3. Limpar processos, POSIX semaphores e SysV SHM.
4. Compilar `NBTestApp` localmente em cada VM; não copiar binários.
5. Iniciar PGCS → NRNCS → NBTestApp, usando paths canónicos e configuração acima.
6. Observar durante uma janela limitada, com logs capturados por processo.
7. Recolher counts e marcadores antes do teardown.
8. Parar processos/VMs e verificar cleanup.

## 7. Classificação de falhas

Classificar a primeira edge não comprovada como:

- inicialização/configuração;
- discovery PGCS/NRNCS;
- exposição/notification;
- publicação/armazenamento de binding;
- subscription/HT lookup;
- delivery/routing;
- IPC/GW;
- ou processamento do `NBTestApp`.

## 8. Rollback

Não há alteração funcional prevista. Remover apenas a configuração temporária e artefactos/logs de teste; preservar a SPEC e a evidência.
