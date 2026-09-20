# Astra Review — SPEC-060 corrected proposal (round 2)

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**SPEC:** `SPEC-060-ng-elc-remote-payload-evidence.md`<br>
**Task:** `NG-058`<br>

## Verdict — **CONDITIONAL GO** para a SPEC como Proposal

A versão corrigida incorpora as exigências centrais do parecer anterior. Contudo, ainda contém ambiguidades normativas que impedem um **GO para implementação integral**. NG-058 deve permanecer **bloqueada** até à revisão/aprovação exacta aplicável.

## Bloqueantes restantes

1. **Namespaces de preservação inconsistentes.**
   R03 exige `artifacts/source`, `artifacts/nrncs` e `artifacts/repository`; R06 exige cobertura de `payload/source/`, `payload/nrncs/` e `payload/repository/`. Definir um layout canónico e distinguir explicitamente bytes preservados, mapas e metadados. O manifest tem de cobrir os bytes efectivamente recolhidos.

2. **Esquema fechado ainda incompleto.**
   Faltam localização, tipos e regras dos campos obrigatórios de bounds, ownership, identidade efectiva e ligação NRNCS à configuração. Esclarecer também:
   - onde reside o `acceptance_profile` usado em R00 e como se relaciona com o campo dentro de `payload_evidence`;
   - classificação de planos legados com campos omitidos e rejeição de combinações contraditórias;
   - regra exacta de `pattern` e âmbito da enumeração que impede ocultar extras;
   - distinção entre expansão legítima de paths e rejeição de paths relativos.

3. **Resultados ainda admitem interpretações incompatíveis.**
   Harmonizar R00/R07: ausência do bloco num candidato normal deve causar rejeição pré-launch; `INCONCLUSIVE` deve aplicar-se à evidência runtime incompleta num plano previamente válido.
   Qualificar também R07 sobre Repository vazio com a excepção conclusiva de R05. Na última linha de R05, “não elegível para PASS” precisa de uma regra explícita para o estado runtime, preservando qualquer `FAIL` sticky.

4. **Dependência do seal e ordem das aprovações por fechar.**
   R06 torna normativo o protocolo de SPEC-056, enquanto a secção 7 coloca essa SPEC na linha M1-L/v1. Explicitar que apenas o contrato controller-only de integridade é reutilizado, sem importar dependências operacionais, e identificar os objectos excluídos para evitar ciclos.
   Separar ainda aprovação da SPEC **antes da implementação** de revisão do diff/testes **depois da implementação**. A exigência de revisão do “diff exacto” antes de implementar o collector é circular se se referir ao próprio diff do collector.

## Menor incremento controller-only autorizado

**Nesta revisão, apenas fecho documental dos pontos acima e desenho dos testes/fixtures.** Não fica autorizada implementação ou execução de testes pelo presente pedido.

Após aprovação da versão exacta fechada, o menor incremento deverá limitar-se a:

- parser/validador puro do contrato;
- mapas e comparação fail-closed;
- três origens fake independentes;
- preservação, manifest, re-hash offline e seal exclusivamente em fixtures locais;
- testes de compatibilidade marker-only e da matriz de resultados.

Sem transporte SSH real, fallback local ou promoção de M2-R.

## O texto já é suficiente para implementar testes/fixtures locais?

**Parcialmente.** É suficiente para desenhar casos RED e fixtures dos requisitos inequívocos, mas **não para fixar toda a suite normativa nem o fluxo completo de preservação/seal sem assumir decisões não especificadas**. Além disso, a própria NG-058 condiciona a implementação à aprovação exacta da SPEC.

## Condições ainda necessárias

- Resolver os bloqueantes e obter revisão Astra da versão exacta.
- Implementar apenas o incremento controller-only aprovado.
- Obter RED→GREEN, suite completa no intérprete canónico, `compileall` e `git diff --check`, numa fase posteriormente autorizada.
- Submeter o diff exacto e a evidência dos testes à revisão Astra.
- Manter preparação do candidato dependente de M0/M1-R e execução dependente de autorização operacional separada.

**Trial continua explicitamente não autorizado. Nenhuma implementação, execução, VM, SSH, QGA, cleanup, reboot, push, tag ou release foi realizada nesta revisão.**
