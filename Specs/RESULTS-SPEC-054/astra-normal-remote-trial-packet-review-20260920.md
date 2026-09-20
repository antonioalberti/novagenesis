# Astra Review — pacote de trial normal remoto v1.0.0

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**Branch:** `AIOPT3`<br>
**Pacote:** `Specs/RESULTS-SPEC-054/normal-remote-trial-packet-20260920.md`<br>

## Veredito Astra: **HOLD**

**Prontidão do pacote:** adequado como documento de preparação, **não pronto para autorizar execução ou aceitação**. O estado `PREPARADO / NÃO AUTORIZADO` é coerente com os bloqueios declarados.

Esta revisão limita-se ao texto exacto fornecido. Não verifica o repositório, o baseline, as SPECs ou bundles históricos e não autoriza trial, VM, SSH, QGA, `clean.sh`, reboot, push, tag ou release.

### 1. Avaliação do pacote

O pacote delimita correctamente:

- o percurso remoto **M0 → M1-R → M2-R → M3**, sem importar resultados local/QGA;
- a diferença entre markers de progresso e prova byte-exact;
- a necessidade de observação independente em Source, NRNCS e Repository;
- a preservação de falhas, sem retry automático nem fallback implícito;
- o fecho de teardown e evidência antes da aceitação;
- o carácter apenas histórico dos resultados anteriores.

Os bloqueios apresentados impedem um **GO**, mesmo condicional para execução: M0 não está fechado, M1-R está em `HOLD`, o oráculo necessário não está disponível no plano remoto actual e falta autorização operacional.

### 2. A lacuna do file oracle bloqueia o trial?

**Sim: bloqueia a tentativa normal de aceitação M2-R proposta.**

Não é obrigatório que a solução se chame `runtime_oracle.type=files`; é obrigatório que exista, **antes da tentativa**, um mecanismo aprovado que preserve e permita verificar:

- mapas independentes Source/NRNCS/Repository;
- nome, tamanho e SHA-256 de cada um dos cinco JPEGs;
- cópias re-hashable provenientes de cada área de observação;
- ausência de extras e faltas, e igualdade byte-exact;
- ligação inequívoca desses artefactos ao trial, SHA candidato, manifest e seal.

Markers isolados não satisfazem esse requisito. Executar agora e construir depois o protocolo de prova contrariaria a secção 3 do próprio pacote.

### 3. Menor incremento seguinte: controller-only

Recomendo **fechar primeiro o contrato de evidência remota e implementá-lo no controller**, sem alterar processos NovaGenesis, sem acrescentar QGA e sem executar infraestrutura remota nesta etapa.

O incremento mínimo deve:

1. **Definir o contrato do oráculo remoto:** origens independentes, conjunto esperado dos cinco ficheiros, esquema dos mapas e regras de completude/igualdade.
2. **Estender validação e preservação no controller:** aceitar esse contrato apenas quando completo; recolher, na futura execução autorizada, os artefactos das três origens para namespaces separados no bundle, sem usar uma cópia Source como substituto de NRNCS ou Repository.
3. **Re-hashar as cópias preservadas e ligar a prova ao bundle:** registar origem e identidade, incluir mapas e artefactos no manifest e exigir a sua verificação offline antes do seal terminal.
4. **Testar apenas com fixtures/mocks:** cobrir sucesso, ficheiro ausente/extra, divergência de tamanho/hash, origem não independente, recolha incompleta e adulteração do bundle.

**Não basta desbloquear `files` no validador remoto.** É preciso implementar a semântica de recolha, independência, preservação e decisão correspondente.

A alternativa de recolha aprovada com cálculo offline também é admissível, mas apenas com esse contrato congelado e integrado no ciclo de evidência **antes** da tentativa; não como regularização posterior.

### 4. Condições para uma única tentativa posterior

Uma autorização futura exige, cumulativamente:

| Gate | Condição |
|---|---|
| **M0** | SPEC-038/054 reconciliadas; candidato completo, limpo e congelado; build normal e Sanitizer ligados ao mesmo SHA; verificação dos binários contra o receipt definida e exigida antes do launch. |
| **M1-R** | SPEC-046/052 e procedimento remoto formalmente fechados; configuração privada, host keys, identidades, exclusividade, leases, inventários e teardown por identidade aprovados. |
| **Oráculo M2-R** | Uma das duas soluções aprovada, implementada quando aplicável e testada; plano, workload, readiness, subscription/readback e critérios congelados. |
| **Evidência** | Destino durável, três origens observáveis, artefactos re-hashable, manifest, verificador offline e terminal seal definidos. |
| **Falhas** | Regras inequívocas para atribuir `OPEN`, `INCONCLUSIVE`, `FAIL` ou `BLOCKED`, distinguindo gates não provados de violações observadas; sem retry automático ou recuperação externa contabilizada como teardown do controller. |
| **Autorização** | Aprovação operacional explícita para **uma tentativa**, vinculada ao SHA candidato, plano/configuração congelados e novo trial ID. |

O baseline documental `462df8492ef9f0b7ec2d307950b4b36fbc1558fe` não deve ser tratado automaticamente como SHA candidato de execução.

### Disposição final

**Manter o pacote como `PREPARADO / NÃO AUTORIZADO`.** O próximo passo é o incremento controller-only e o fecho documental dos gates, não um trial.

Mesmo posteriormente, `runtime_result=PASS`, `teardown_result=PASS`, `evidence_result=COMPLETE` e `exit_code=0` serão **necessários, mas não suficientes**: a aceitação continuará dependente da prova independente dos três mapas, da revisão Astra do bundle exacto e da auditoria do Plano Mestre.
