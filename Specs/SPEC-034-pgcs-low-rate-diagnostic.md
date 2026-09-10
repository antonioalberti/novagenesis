# SPEC-034: Diagnóstico PGCS ↔ PGCS com tráfego interno baixo

**Data:** 2026-09-10  
**Estado:** Implemented — diagnóstico concluído; macros temporárias revertidas  
**Autor:** Hermes Agent + Antonio Alberti  
**Branch:** AIOPT3  
**Related:** SPEC-027, SPEC-028-B, SPEC-033, NG-022

## 1. Objectivo

Isolar a descoberta mútua e o transporte RAW entre os PGCS das VMs 101 (repo61) e 102 (source36), sem NRNCS ou ContentApp, usando o gerador interno de stress a aproximadamente 1 mensagem/s por direcção.

## 2. Hipótese

Se ambos os PGCS se descobrirem e o tráfego interno for aceite, os logs deverão mostrar:

- dispatcher RAW criado em cada PGCS;
- descoberta mútua em `PGCSTuples`;
- mensagens de stress agendadas após a descoberta;
- `sendto()` bem-sucedido e recepção correspondente;
- ausência de retries, timeouts ou crescimento anómalo de filas.

Se a descoberta falhar, o teste deverá distinguir configuração/initialização, discovery, transporte RAW ou recepção.

## 3. Âmbito

### Incluído

- Apenas dois processos PGCS: um em cada VM.
- Paths canónicos de `IO/PGCS/` e configuração `PGCS.ini`.
- `StressTest 1`, `StressInterval 1`, `DelayBeforeRunPeriodic 5`, resultando em 5 mensagens por período de 5 s (aprox. 1 mensagem/s após truncamento inteiro).
- Debug selectivo nos módulos de descoberta, scheduling e transporte RAW.
- Stop/start limpo das VMs, POSIX semaphores e SysV SHM.
- Logs separados por VM e captura de PIDs, MACs, sockets, discovery e tráfego.

### Excluído

- NRNCS, ContentApp, publicação de fotos e cache.
- Alterações funcionais ao protocolo, GW, SAR, filas ou ownership.
- `DEBUG2`, `DEBUG3`, `DEBUG5` e `DEBUG6` na primeira passagem devido ao volume de SHM/segmentos.
- Aceitação de NG-022 ou B1 de SPEC-033.

## 4. Debug temporário autorizado

Activar apenas durante a compilação diagnóstica:

- `PGCS/src/PGHelloIHC01.cpp`: `#define DEBUG` — decisões de discovery.
- `PGCS/src/PGRunPeriodic01.cpp`: `#define DEBUG` — hello/exposition/stress scheduling e `PGCSTuples`.
- `PGCS/src/NGAL_Transport_RAW.cpp`: `#define DEBUG` — sucesso/falha de `sendto()` e fragmentos.

Não activar inicialmente o debug amplo em `PG.cpp`; os seus níveis `DEBUG2/3` produzem detalhe de semáforos/SHM desnecessário para este teste.

As macros são temporárias, não constituem alteração funcional e devem ser revertidas após a caracterização.

## 5. Critérios de aceitação diagnóstica

- [ ] PGCS correcto compilado nas duas VMs, com commit/proveniência e binários identificados.
- [ ] Cada PGCS cria dispatcher RAW e usa o MAC remoto correcto.
- [ ] Cada PGCS descobre o outro e `PGCSTuples.size() > 0`.
- [ ] O scheduler deixa de reportar “No peers discovered yet”.
- [ ] O scheduler reporta 5 mensagens por período de 5 s (aprox. 1/s por direcção).
- [ ] Os logs de transporte mostram envios/recepções sem erro persistente ou retry anómalo durante a janela completa de 30 minutos.
- [ ] O teste termina sem crash e com cleanup verificável de processos e IPC.

Um resultado positivo é evidência de discovery/transporte normal em baixa taxa; não prova entrega de fotos, cache, ausência de forwarding ou segurança B1.

## 6. Configuração

O código de `PGRunInitialization01` lê explicitamente `<path>/PGCS.ini`. Para este teste, `PGCS_pgcs1.ini` e `PGCS_pgcs2.ini` não serão tratados como configuração carregada automaticamente.

Configuração diagnóstica:

```text
StressTest 1
StressInterval 1
DelayBeforeRunPeriodic 5
```

## 7. Execução

1. Preservar o estado Git e confirmar a branch AIOPT3.
2. Aplicar as macros apenas no checkout diagnóstico.
3. Compilar PGCS nas duas VMs; nunca copiar binários.
4. Reiniciar as VMs e remover processos/IPC residual.
5. Iniciar um PGCS por VM com `PGCS.ini` canónico e MAC do peer.
6. Observar durante uma janela de 30 minutos, recolhendo logs de discovery e transporte.
7. Parar processos/VMs e verificar cleanup.
8. Reverter as macros temporárias sem alterar o código funcional.

## 8. Resultado obtido — 2026-09-10

### Métricas observadas

- [x] `StressTest 1` e `StressInterval 1` foram carregados nos dois PGCS.
- [x] Ambos os PGCS reconheceram o peer: `PGCSTuples.size=1`.
- [x] Cada PGCS criou o dispatcher RAW e trocou mensagens HELLO.
- [x] O scheduler reportou 5 mensagens por período de 5 s (`StressInterval=1`), equivalente a aproximadamente 1 mensagem/s por direcção.
- [x] Repository-side log: 60 períodos de stress, 300 mensagens agendadas, 330 fragmentos enviados e 325 recebidos.
- [x] Source-side log: 59 períodos de stress, 295 mensagens agendadas, 325 fragmentos enviados e 330 recebidos.
- [x] Após descontar os 30 fragmentos HELLO de cada lado, as 300 mensagens stress enviadas pelo Repository foram recebidas pelo Source e as 295 enviadas pelo Source foram recebidas pelo Repository.
- [x] Não foram observados `ERROR`, `FATAL`, `sendto` failures ou warnings no resumo recolhido.
- [x] O teste terminou com cleanup e as VMs ficaram `stopped`.

A diferença observada nessa execução curta não foi suficiente para classificar perda ou duplicação. O resultado prova discovery e transporte RAW normal a baixa taxa, mas não prova transferência de fotos, cache NRNCS, segurança B1 ou contabilidade exacta de mensagens.

### Execução estendida — 30 minutos

- [x] Ambos os PGCS permaneceram activos durante a janela completa.
- [x] `PGCSTuples.size=1` foi observado em 359 períodos de scheduling em cada lado.
- [x] Cada lado registou 359 períodos × 5 mensagens = 1.795 mensagens de stress agendadas.
- [x] Repository: 1.975 fragmentos enviados e 1.975 recebidos; descontando 180 HELLOs, 1.795 mensagens de stress enviadas e 1.795 recebidas.
- [x] Source: 1.975 fragmentos enviados e 1.980 recebidos; descontando 180 HELLOs, 1.795 mensagens de stress enviadas e 1.800 recebidas.
- [x] Não foram observados `ERROR`, `FATAL`, falhas `sendto` ou warnings nos resumos.
- [x] O teste terminou com cleanup e as VMs ficaram `stopped`.

A discrepância de cinco recepções extra no Source não pode ser declarada como perda nem como duplicação sem reconciliação adicional dos logs/counters. O resultado é aceite como evidência forte de estabilidade do discovery e do transporte RAW a baixa taxa, mas a contabilidade exacta de mensagens permanece aberta.


## 9. Classificação esperada

O relatório deve classificar a primeira edge não comprovada como:

- inicialização/configuração;
- discovery PGCS;
- scheduling do gerador;
- transporte RAW/envio;
- recepção/dispatcher;
- ou processamento posterior.

## 10. Rollback

Reverter apenas as alterações temporárias de `#define DEBUG`. Não reverter o baseline AIOPT3 nem alterar SPEC-022, B1 ou NG-022.
