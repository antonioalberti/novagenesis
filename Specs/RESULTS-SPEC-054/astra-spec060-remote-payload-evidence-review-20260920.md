# Astra Review — SPEC-060 remote payload evidence

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**SPEC:** `SPEC-060-ng-elc-remote-payload-evidence.md`<br>
**Task:** `NG-058`<br>

## Veredicto: **CONDITIONAL GO — apenas para a SPEC como Proposal**

A direcção é correcta: substituir a suficiência de markers por evidência preservada de **Source, NRNCS e Repository**, mantendo a separação entre M2-R remoto v1.0 e M1-L/QGA v1.1+.

**O texto exacto ainda não está suficientemente fechado para implementação.** Recomendo aprovação conceptual condicionada às correcções abaixo, seguida de revisão Astra da versão exacta. NG-058 deve continuar bloqueada até essa aprovação.

**O trial permanece em HOLD.** Este parecer não autoriza preparação operacional, VM, SSH real, QGA, cleanup, reboot, push, tag ou release. A análise limita-se aos textos fornecidos; não valida os ficheiros referenciados, código ou evidências históricas.

## 1. Erros e ambiguidades de contrato

### 1.1. Bloco obrigatório versus compatibilidade marker-only — bloqueante

Há conflito entre:

- R05: markers completos sem payload completo implicam runtime `INCONCLUSIVE`;
- §4: planos marker-only existentes mantêm a semântica;
- §5: bloco ausente implica rejeição antes do launch.

É necessário distinguir explicitamente:

| Classe de plano | Sem `payload_evidence` |
|---|---|
| Diagnóstico marker-only legado | Mantém a semântica anterior; não comprova aceitação normal M2-R |
| Candidato normal de payload | Rejeitado na validação pré-launch |
| Plano de payload válido, mas recolha incompleta | Não-PASS, com evidência da incompletude |

A presença de `payload_evidence` não deve redefinir implicitamente todos os planos remotos. Falta também especificar como o bloco se articula com `runtime_oracle.type=markers`: **oráculo composto, requisito adicional ou novo tipo versionado**. O contrato deve escolher uma opção.

### 1.2. O exemplo não é um esquema normativo suficiente — bloqueante

“Bloco versionado semelhante a” deixa comportamentos essenciais indefinidos. Devem ser fixados:

- exactamente as chaves de origem `source`, `nrncs`, `repository`;
- exactamente cinco entradas no mapa, coerentes com `count`;
- `hash` limitado a `sha256` nesta versão;
- tamanho inteiro com domínio definido e digest hexadecimal de 64 caracteres;
- política de campos desconhecidos e de chaves JSON duplicadas;
- regras de nomes: basename ou path relativo, extensão, case sensitivity e normalização;
- rejeição de paths absolutos nos nomes, `..`, separadores indevidos, NUL e colisões;
- relação entre `pattern` e o conjunto inventariado.

**Extras não podem desaparecer por filtragem de `*.jpg`.** É preciso definir se cada raiz é uma área exclusiva de payload ou se existe uma regra explícita para separar payload de ficheiros auxiliares.

O exemplo de mapa com uma entrada e `"..."` deve ser identificado como **abreviado e inválido para execução**, ou substituído por um exemplo completo.

### 1.3. Independência não equivale apenas a paths diferentes — bloqueante

O exemplo coloca Source e NRNCS no mesmo host. Isso não é, por si só, um erro: a independência exigida é de **origem e função**, não necessariamente de máquina.

Mas o contrato precisa distinguir:

1. validação estática de paths e identidades declaradas;
2. verificação posterior da origem efectiva pelo mecanismo de recolha;
3. preservação local em namespaces distintos.

Paths lexicalmente diferentes não excluem symlinks, hardlinks, bind mounts ou dois aliases para o mesmo endpoint. É necessário definir a evidência de identidade/canonicalização suportada pelo helper e tratar identidade não demonstrável como **não elegível**, sem prometer que o controller prova tudo sem contactar guests.

Também falta estabelecer como se demonstra que a área NRNCS é efectivamente a área observada pelo serviço, e não uma pasta preenchida para satisfazer o oráculo. Fixtures podem testar este contrato, mas não demonstrar essa propriedade numa instalação real.

### 1.4. Resultado funcional, evidência e aceitação — bloqueante

R03 permite `evidence_result=INCOMPLETE` **ou** `runtime_result=INCONCLUSIVE`. Isto pode permitir combinações ambíguas, incluindo runtime `PASS` com evidência incompleta.

Deve existir uma tabela normativa, pelo menos:

| Situação | Runtime de payload | Evidência | Aceitação normal |
|---|---|---|---|
| Três origens completas, iguais ao esperado, restantes gates satisfeitos | `PASS` | Completa e verificável | Elegível, sujeita aos gates externos |
| Recolha ausente, instável, truncada ou interrompida | `INCONCLUSIVE`, salvo `FAIL` sticky anterior | `INCOMPLETE` | `false` |
| Divergência conclusiva numa observação válida e completa | `FAIL` sticky | Pode estar completa e íntegra | `false` |
| Manifest/hash/cobertura/seal inválido | Não elegível para PASS de aceitação | `INCOMPLETE` | `false` |

O resultado `FAIL` já observado não pode ser apagado por um erro posterior de recolha.

Há ainda tensão entre “faltas” serem não-PASS e Repository vazio ser `UNPROVEN/INCONCLUSIVE`. Deve esclarecer-se quando vazio significa ausência de prova e quando constitui falha funcional conclusiva. **Readiness do serviço não basta para provar que a recolha foi completa.**

### 1.5. Manifest, verificação e seal — bloqueante

A sequência termina em:

`result → manifest → offline verification → terminal seal`

Mas R05 exige verificar também o seal. Falta definir:

- o que é verificado antes do seal;
- o conteúdo exacto coberto pelo seal;
- a verificação final que inclui o seal;
- onde fica a decisão final de aceitação, sem ciclo de hashes.

Após fechar manifest/seal, nenhuma alteração a ficheiros cobertos pode ser silenciosamente permitida. `accepted=true` não deve apoiar-se apenas numa verificação anterior à existência do seal.

## 2. Dependências e autorização em NG-058

### Separar bloqueios de desenvolvimento e operacionais

`blocked_by` agrega M0/M1-R e revisão da Proposal. Contudo, o texto apresenta o incremento controller-only como próximo passo.

Deve ficar explícito:

- **implementação controller-only:** depende da aprovação Astra da SPEC exacta e das dependências técnicas pertinentes;
- **preparação de candidato normal:** depende da revisão do diff/testes e da conclusão de M0/M1-R;
- **execução:** depende de autorização operacional posterior e separada.

Se M0/M1-R também bloqueiam desenvolvimento, isso deve ser dito expressamente.

### Não há elementos para validar NG-049 e NG-054

Os seus conteúdos não foram fornecidos. Não é possível confirmar se são dependências duras, contextuais ou já satisfeitas. Convém indicar, para cada uma, **qual entrega bloqueia qual fase**, sem presumir equivalência entre IDs de tarefa e de SPEC.

### Corrigir duas formulações

- “O próximo incremento autorizado” deve tornar-se “o próximo incremento proposto, condicionado à aprovação da SPEC exacta”.
- “Actualizar M2-R” deve significar **registar progresso/evidência, sem promover nem fechar o gate**.

A tarefa deve também alinhar o plano de trabalho com a exigência explícita de concluir M0/M1-R antes de preparar o candidato.

## 3. Critérios de aceitação em falta

Além dos testes já previstos, acrescentaria:

- **Bounds definidos:** timeout total e por origem, limites de ficheiros/bytes, política de retry e comportamento de quota.
- **Quiescência:** confirmação de paragem de todos os writers relevantes; falha de stop ou leitura instável impede PASS.
- **Checkpoint de preservação:** condições objectivas para o considerar concluído e política de retenção perante falha; cleanup nunca apaga a única evidência recuperável.
- **Ownership:** recolha/cleanup restritos ao trial, com regras efectivas, não apenas declaradas.
- **Proveniência:** mapa esperado e workload congelados antes do launch, ligação ao trial e rejeição de artefactos stale ou de outro trial.
- **Re-hash independente:** verifier offline lê novamente os bytes preservados, sem confiar nos mapas ou digests produzidos pelo collector.
- **Cobertura do bundle:** expected, plano efectivo, proveniência, mapas, cópias e estados/erros de recolha cobertos; ficheiros inesperados detectados.
- **JPEGs “novos”:** definir se isto exige apenas geração associada ao trial ou também validação de formato. Extensão e hash, isoladamente, não demonstram formato JPEG.
- **Isolamento dos testes:** qualquer tentativa de SSH real, acesso a guests ou uso de `local_observe` como substituto deve falhar explicitamente.
- **Intérprete canónico:** identificar o intérprete e os comandos de teste aprovados; harmonizar SPEC e tarefa quanto a `compileall` e `git diff --check`.

## 4. Menor incremento controller-only admissível

**Após aprovação Astra do contrato corrigido**, o menor incremento deve limitar-se a:

1. esquema e validação pura, incluindo distinção entre diagnóstico legado e candidato de payload;
2. helpers puros para inventário, comparação e decisão fail-closed/sticky;
3. collector injectável exclusivamente fake/fixture nesta fase, com três origens identificadas;
4. preservação local em namespaces separados;
5. manifest, re-hash offline e seal em fixture;
6. testes RED→GREEN, regressão marker-only e verificações estáticas previstas.

A ordem stop → collect → preservation → cleanup deve ser demonstrada por eventos fake. A aprovação deste incremento **não autoriza conectar o collector a SSH real nem certificar a independência operacional das origens**.

## 5. Condições para implementação

Antes de iniciar:

- resolver os bloqueantes de contrato acima;
- alinhar SPEC-060 e NG-058 quanto a fases, dependências e autorização;
- obter revisão Astra da **versão exacta corrigida**;
- registar o perímetro controller-only e os comandos locais permitidos.

Depois da implementação, exigir revisão Astra do **diff exacto e dos resultados dos testes**, sem promoção automática de M2-R.

**Conclusão:** CONDITIONAL GO para a proposta arquitectural; implementação ainda condicionada; trial e aceitação M2-R continuam em HOLD.
