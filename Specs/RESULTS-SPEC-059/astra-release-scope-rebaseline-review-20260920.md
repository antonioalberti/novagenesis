# Astra Review — release-scope rebaseline normal path versus QGA

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**Branch:** `AIOPT3`<br>
**Baseline:** `1b21ae7`<br>
**Scope:** rebaseline documental antes de qualquer trial.

## Verdict: **CONDITIONAL GO — apenas para a rebaseline documental**

A separação proposta é legítima **se o perfil mínimo do Plano Mestre v4 for exactamente o descrito e se a análise de dependências confirmar que os requisitos retirados não são necessários ao perfil remoto**.

Este parecer incide sobre o texto apresentado: não verifica o repositório, o conteúdo integral das SPECs nem o estado de `AIOPT3` em `1b21ae7`.

**O gate operacional permanece `HOLD`.** Não fica autorizado qualquer trial, operação de VM/QGA, `clean.sh`, reboot, push, tag ou release.

## 1. Legitimidade da separação

**Sim, é coerente separar:**

- **M1-R:** deployment, lifecycle e teardown do perfil remoto;
- **M2-R:** caminho normal remoto end-to-end;
- **M1-L:** perfil local/native-privileged e QGA-loss, preservado para v1.1+.

Se a v1.0 certifica exclusivamente o executor remoto SSH, não é necessário exigir que um failure path específico do adaptador local/QGA esteja resolvido para certificar esse perfil.

Contudo, a exclusão deve ser feita **por aplicabilidade dos requisitos, não apenas pelo título ou número da SPEC**. Em particular, retirar `SPEC-056` do perfil v1.0 não pode retirar as obrigações transversais de provenance, ligação entre SHA/build/binários e integridade da evidência. O mesmo vale para requisitos de teardown ou recovery que também sejam necessários no modo remoto.

### Redução explícita de escopo versus contorno de critérios

A proposta constitui uma **redução explícita do escopo do gate actualmente registado**, ainda que alinhe esse gate com o perfil mínimo declarado no Plano Mestre. É uma rebaseline legítima quando:

- é aprovada e registada antes do novo trial;
- limita expressamente as alegações de suporte da release;
- preserva requisitos e resultados locais como pendentes;
- mantém integralmente os critérios aplicáveis ao remoto;
- não transforma falhas históricas em PASS.

Seria **contorno de critérios** se a exclusão servisse para ocultar uma dependência real do remoto, dispensar provenance/teardown/seal, ou mudar os critérios depois de conhecido o resultado do candidato.

## 2. Correcções necessárias

### A. Corrigir “três linhas independentes”

O bloco apresenta cinco linhas, não três. Além disso, os gates da release não são independentes entre si.

Sugestão:

> “Separar o register em gates do perfil remoto v1.0 e numa linha local/QGA diferida para v1.1+. M3 depende de M0, M1-R e M2-R aceites; M1-L não bloqueia a v1.0, desde que não exista dependência aplicável ao perfil remoto.”

### B. Acrescentar uma matriz de aplicabilidade

Para cada SPEC/NG retirado do caminho crítico, registar:

- requisito e âmbito;
- justificação para `N/A — perfil v1.0 remoto`;
- eventuais obrigações transversais que continuam exigíveis e onde ficam cobertas;
- destino no backlog local/QGA;
- aprovação da reclassificação.

**`N/A` deve ser relativo ao perfil v1.0, não um estado global de conclusão da SPEC ou tarefa.** `NG-055` e `NG-056` não ficam fechados por esta decisão.

### C. Fixar critérios verificáveis antes do trial

O pacote proposto é uma boa base, mas ainda não basta como contrato de aceitação. Deve fixar previamente:

- plano e configuração exactos do candidato;
- definição de workload “novo”;
- readiness exigida e respectivos limites de espera;
- conteúdo e regras de comparação dos três mapas;
- resultado esperado de subscription/readback;
- critérios de teardown por identidade e inventário final;
- regras de completude, integridade e reabertura offline do bundle.

“Verificar” não deve deixar a definição de PASS para depois da execução.

### D. Tornar explícita a falha segura do remoto

Não é necessário implementar agora o seam QGA-loss. É necessário definir o que acontece se o trial remoto perder SSH, expirar ou não conseguir comprovar teardown:

- interromper segundo limites previamente definidos;
- preservar evidência parcial;
- não declarar teardown concluído sem prova;
- colocar o candidato em `HOLD`;
- solicitar autorização separada para qualquer recuperação fora do procedimento aprovado.

**Sem fallback implícito para QGA/local-profile.**

### E. Qualificar os controlos históricos

Preservar os três resultados exactamente como descritos. A expressão “positive controls” deve limitar-se às propriedades efectivamente demonstradas.

Em particular, `qga-r17-20260918` pode sustentar observações de arranque/readiness e diagnóstico de falha, **não** controlo positivo de entrega ao Repository, teardown completo ou seal. Nenhum dos históricos fecha gates do candidato remoto actual.

## 3. Actualização conjunta dos documentos

**Sim — parecer favorável condicionado à actualização conjunta do Plano Mestre, NG-050, NG-056 e Dashboard**, incorporando as correcções acima e a aprovação formal de escopo.

A actualização deve ser coerente entre os quatro documentos e incluir:

- perfil certificado e exclusões explícitas;
- nova estrutura e dependências dos gates;
- matriz de aplicabilidade;
- preservação do backlog local/QGA e dos resultados históricos;
- critérios do próximo candidato;
- estado operacional ainda em `HOLD`;
- referência comum à decisão de rebaseline.

Se o gate register normativo ou `NG-055` forem mantidos noutro local, terão também de reflectir a decisão, sem estados contraditórios.

Isto é autorização condicionada **para a alteração documental**, não confirmação de que foi executada nem autorização de push.

## 4. Condições para um único trial normal remoto posterior

O pacote da secção 5 é **suficiente como base de preparação**, depois das correcções. A execução só poderá ocorrer com autorização específica posterior que identifique:

1. **Rebaseline aprovada e documentos sincronizados.**
2. **SHA candidato completo, limpo e congelado**, distinto do mero HEAD de referência quando aplicável, com build receipt/manifest ligado aos binários efectivamente lançados.
3. **Ambiente Linux/AIOPT3, dois guests Alpine identificados e executor canónico em `--mode remote`**, sem dependência operacional de QGA.
4. **Plano normal, configuração e cinco JPEGs novos identificados**, sem reutilização de outputs históricos como evidência do candidato.
5. **Critérios de aceitação fixados antes da execução**, incluindo readiness, mapas independentes Source/NRNCS/Repository e subscription/readback.
6. **Procedimento de teardown remoto por identidade**, inventários e tratamento autorizado de falhas.
7. **Destino durável da evidência**, seal e validação offline por reabertura.
8. **Autorização limitada a uma única tentativa**, sem retries automáticos ou nova tentativa sem autorização.
9. **Submissão do pacote exacto a Astra**, sem tag/release automática mesmo que o trial reporte PASS.

**Conclusão:** aprovar condicionalmente a separação de escopo e a sua tradução documental; manter `HOLD` para execução. O objectivo legítimo é certificar com rigor um perfil mais estreito e declarado — não usar esse perfil para dispensar critérios que continuam a ser seus.
