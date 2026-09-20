# Astra Review — replaneamento TRL3→TRL4

**Data:** 2026-09-20
**Modelo:** `gpt-6-astra`
**Contexto revisto:** Plano Mestre v5, proposta A/B/C, SPEC-060/NG-058, pacote remoto, diagnóstico r17, histórico local e estado exact-SHA `446f698`
**Estado do parecer:** `CONDITIONAL GO` para replaneamento de um demonstrador local; `HOLD` para a release pública/engenharia `v1.0.0`

## 1. Diagnóstico

A circularidade resulta de misturar quatro níveis:

1. implementação controller-only já testada;
2. lacuna de evidência operacional — falta um ensaio novo ligado ao SHA;
3. pré-requisito operacional — o fluxo remoto ainda passa `payload_result=None` e não tem collector/helper produtivo;
4. rótulo de release — requisitos de engenharia remota foram tratados como bloqueios da primeira demonstração TRL3→TRL4.

Mais fixtures não produzem a observação operacional em falta.

## 2. Taxonomia recomendada

### Primeiro marco

**NovaGenesis Research Demonstrator 0.1 — Local Normal Path**
Identificador proposto: `v0.1.0-research.1-AIOPT3`

Claims possíveis, após um ensaio aceite: demonstração integrada de laboratório, configuração Linux local específica, cinco JPEGs novos, igualdade byte-exact Source↔Repository, lifecycle observado e bundle verificável.

Não demonstra produção, M2-R, multi-VM, persistência NRNCS, readback de protocolo, semântica geral de subscriptions, performance ou QGA-loss.

### Marco posterior

**NovaGenesis Engineering Release 1.0 — Remote Normal Profile**
Identificador futuro: `v1.0.0-AIOPT3`

Mantém os gates estritos M0/M1-R/M2-R/M3 e SPEC-060 remoto completo.

### Linha diferida

**NovaGenesis QGA-Loss Resilience Research Track**
QGA-loss permanece `HOLD/NO-GO`, sem tag de release agora.

## 3. Gates mínimos do demonstrador

R0 exact-SHA e plano congelado; R1 build receipt e hashes dos binários; R2 cinco JPEGs e isolamento Source/Repository; R3 runtime local bounded e completion conhecido; R4 mapas independentes expected/Source/Repository com cópias re-hashable; R5 teardown e inventário por identidade; R6 bundle durável com manifest/seal schema-v2 e verifier; R7 revisão do bundle e matriz explícita de claims/limitações.

Para este marco:

- três origens Source/NRNCS/Repository: `N/A`/diferido;
- manifest/seal schema-v2: obrigatório;
- cinco JPEGs: obrigatório;
- marker-only: não é aceitação;
- mapas Source/Repository: obrigatórios;
- readback Repository e semântica completa de subscriptions: `N/A`/diferidos;
- multi-VM e QGA-loss: `N/A`/diferidos.

`N/A` significa exclusão explícita, nunca PASS implícito.

## 4. Próximo passo recomendado

Parar novas rondas de hardening remoto e preparar apenas uma revisão exact-SHA do caminho local existente. Depois, e somente com autorização operacional separada, executar uma única experiência local bounded, sem QGA, sem retry, sem fallback e sem injecção de falha. O resultado deve ser aceite, FAIL, INCONCLUSIVE ou BLOCKED; não deve ser reinterpretado para obter GREEN.

## 5. Alterações propostas

- transformar o Plano Mestre num plano de marcos de investigação e engenharia;
- adicionar R0–R7 como caminho do demonstrador;
- preservar M0/M1-R/M2-R/M3 como gates de engenharia, sem os marcar PASS;
- renomear A para `A-RDEMO-LOCAL`, manter B-QGALOSS em HOLD e C como `C-ENGINEERING-REMOTE`;
- manter SPEC-060 estrita para a release remota de engenharia, fora do caminho crítico do demonstrador;
- marcar NG-058 como implementação/fixtures concluídos, integração operacional remota OPEN;
- criar tarefa finita para uma única experiência RDEMO;
- manter controlos históricos como históricos.

## 6. Regra anti-loop

Não acrescentar mais hardening controller-only remoto, matrizes cosméticas de tamper, metas de 100/1000 fotos, readback, subscriptions completas ou QGA-loss como pré-requisitos do primeiro demonstrador. Só reabrir implementação se existir um bloqueio concreto de R0–R7.

Este parecer não autoriza VM, SSH, QGA, `clean.sh`, reboot, trial, push, tag, publicação ou release.
