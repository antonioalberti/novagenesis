# Astra Review — SPEC-060 v3 final proposal

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**SPEC:** `SPEC-060-ng-elc-remote-payload-evidence.md`<br>
**Task:** `NG-058`<br>

## Parecer Astra — SPEC-060 v3 / NG-058

**Veredicto: GO para a SPEC como Proposal, limitado ao primeiro incremento controller-only definido na secção 4.**

A revisão é exclusivamente documental e incide sobre a versão exacta transcrita nesta mensagem. Não constitui aprovação de implementação existente, de resultados de testes, de um candidato operacional ou de fecho de M2-R. Os documentos referenciados não foram verificados independentemente.

### 1. Bloqueantes restantes

**Não identifico bloqueantes documentais restantes para iniciar o incremento controller-only delimitado.**

O texto resolve os bloqueantes anteriores de forma suficiente:

- **Classes de plano fechadas:** distingue legado marker-only, diagnóstico explícito e candidato `normal-payload`, com rejeição pré-launch de combinações parciais ou contraditórias.
- **Contrato fail-closed:** define versão, estrutura, cardinalidade, nomes, digests, bounds, ownership e binding NRNCS; rejeita duplicados e campos desconhecidos.
- **Independência das origens:** exige Source, NRNCS e Repository separados, com identidade demonstrável e ligação da área NRNCS à configuração efectiva do serviço.
- **Preservação verificável:** fixa namespaces, mapas sobre bytes preservados, metadados e checkpoint anterior a qualquer remoção de temporários.
- **Resultados sem promoção indevida:** separa runtime, completude da evidência e elegibilidade; preserva `FAIL` sticky e distingue ausência inconclusiva de divergência funcional comprovada.
- **Integridade sem ciclo:** estabelece manifest → verificação offline → seal, com reabertura do seal e sem aceitação baseada apenas numa verificação anterior.
- **Gates separados:** distingue aprovação da SPEC antes da implementação, revisão posterior do diff/testes e autorização operacional independente.

### 2. Leituras normativas a preservar na implementação

Estas observações **não bloqueiam o GO**, mas não podem transformar-se em flexibilizações do contrato:

1. **R00/R01 prevalecem na admissão do plano.** Um candidato normal sem `payload_evidence` completo é rejeitado antes do launch. O `INCONCLUSIVE` descrito em R07 é uma salvaguarda de resultado, não uma autorização para lançar esse plano.

2. **Integridade não equivale a aceitação funcional.** Um bundle pode estar íntegro e correctamente selado, mas continuar com `accepted=false`. Selar uma recolha incompleta não a torna completa nem elegível.

3. **O rollback não cria fallback marker-only de aceitação.** Remover `payload_evidence` de um candidato normal torna-o inválido para launch nessa classe; não o converte automaticamente num diagnóstico legado.

4. **As fixtures provam o comportamento do controller, não a realidade remota.** Não demonstram, por si, identidade efectiva de guests, quiescência real ou binding operacional NRNCS.

### 3. Menor incremento posterior permitido

Após este parecer, o menor incremento permitido é:

1. Criar testes RED de validação e classificação dos planos, sem contactar guests.
2. Implementar o validador fechado de `payload_evidence` e o freeze do plano efectivo.
3. Acrescentar helpers puros de mapas, comparação e decisão conforme R05.
4. Exercitar três origens exclusivamente por fake SSH/helper, com namespaces independentes, bounds e falhas explícitas.
5. Integrar, em fixture, preservação, manifest, re-hash offline e seal.
6. Executar posteriormente os testes controller-only previstos, preservando os resultados para revisão.

**Não é necessário nem permitido introduzir SSH real ou executar um trial para demonstrar este incremento.**

### 4. Condições de implementação

- Respeitar integralmente o âmbito controller-only, sem C++, protocolo, comportamento das VMs, QGA ou fallback local.
- Manter a compatibilidade runtime dos planos marker-only históricos, sem lhes atribuir elegibilidade M2-R.
- Não substituir origens ausentes por mapas vazios, cópias de outra origem ou `local_observe`.
- Testar precedência de falhas, recolha incompleta, identidade não demonstrável, tamper e verificação final do seal.
- Preservar evidência RED→GREEN, suite completa no intérprete canónico, `compileall` e `git diff --check`, conforme NG-058.
- Submeter o **diff exacto e os resultados dos testes** a nova revisão Astra antes de qualquer preparação operacional.
- Registar o presente GO apenas como aprovação documental da SPEC: os critérios de implementação e testes continuam pendentes.

### 5. Estado dos gates e autorização

| Objecto | Decisão |
|---|---|
| SPEC-060 v3 como Proposal | **GO** |
| Primeiro incremento controller-only fake/fixture | **Permitido posteriormente, nos limites acima** |
| Implementação e testes existentes | **Não avaliados nesta revisão** |
| Fecho de M2-R / aceitação de payload real | **Não aprovado** |
| Preparação do candidato operacional | **Bloqueada por M0/M1-R e revisão posterior do diff/testes** |
| Trial e operação real | **NÃO AUTORIZADOS** |

**Confirmação explícita:** este parecer não autoriza executar VM, SSH real, QGA, cleanup, reboot, trial, push, tag ou release. A execução operacional exige autorização posterior e separada.

Nesta revisão não foi implementado código nem executado qualquer teste ou operação.
