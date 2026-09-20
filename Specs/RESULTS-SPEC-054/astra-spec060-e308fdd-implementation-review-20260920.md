# Astra Review — SPEC-060 implementation e308fdd

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**Commit revisto:** `e308fdd`<br>
**Estado:** `HOLD`<br>

## Revisão Astra — `e308fdd`

**Verdict: HOLD para este incremento.**

O diff mantém-se no âmbito **controller-only fake/fixture** autorizado: não acrescenta transporte real nem chamadas de execução remota. A estrutura é adequada, mas há falhas demonstráveis nos requisitos de comparação fail-closed, inventário bounded e matriz de resultados que devem ser corrigidas antes do GO.

Revisão estática exclusivamente do diff fornecido. Os **9 testes focados, 213 testes canónicos e 47 subtests**, assim como `compileall` e `git diff --check`, são resultados reportados, não reexecutados nesta revisão. Não foi disponibilizado o texto integral da SPEC para confronto independente.

### Bugs e riscos concretos

#### 1. Comparação de mapas aceita ausência de evidência — bloqueante

Em `compare_payload_maps`:

```python
compare_payload_maps({}, {})
# status="PASS", accepted=True
```

A função apenas compara conjuntos de nomes e igualdade dos valores. Também aceita dois mapas idênticos com registos inválidos, sem validar nome, tamanho ou digest.

Isto contradiz explicitamente a promessa de não tratar ausência como sucesso. A validação anterior do plano não resolve o contrato desta função pública; `preserve_payload_origins` também não valida a estrutura completa de `expected`.

**Correção:** validar o mapa esperado e os registos observados no ponto de entrada, rejeitando mapas vazios ou malformados sem possibilidade de `PASS`.

#### 2. Divergência completa é transformada em INCONCLUSIVE — bloqueante

Em `preserve_payload_origins`, o estado da origem só é `COMPLETE` se ambos os mapas coincidirem com o esperado.

Consequência:

1. As três origens são inventariadas integralmente.
2. Uma contém cinco ficheiros válidos, mas um digest diferente.
3. Os bytes são preservados integralmente.
4. Essa origem recebe `status="INCOMPLETE"` por divergência.
5. `evaluate_payload_result(result["origins"])` devolve `INCONCLUSIVE`, não `FAIL`.

Assim, a integração dos helpers confunde **completude da evidência** com **conformidade do payload**. O ramo `FAIL / COMPLETE` existe no avaliador, mas não representa corretamente esse caso quando alimentado pelo preservador.

**Correção:** separar completude de recolha/preservação da comparação com o esperado. Evidência completa que prova divergência deve produzir **FAIL / COMPLETE / accepted=False**.

#### 3. Inventário e preservação não são efetivamente bounded — bloqueante

Em `inventory_payload_origin`:

- `sorted(root_path.iterdir())` enumera e materializa **todo** o diretório antes de aplicar `max_files`;
- leituras que excedem a quota ou falham por race não debitam os bytes já lidos em `total_bytes`, permitindo repetir leituras de até `max_bytes + 1` por ficheiro;
- os limites temporais declarados no contrato não são consumidos por estes helpers.

Em `preserve_payload_origins`, `shutil.copyfile` volta a ler a origem **sem limite de bytes**, depois do inventário. Um ficheiro que cresça nesse intervalo pode provocar cópia acima da quota; a verificação posterior não limita o trabalho já realizado.

**Correção:** enumeração com interrupção antecipada, orçamento cumulativo de leitura incluindo tentativas falhadas e cópia limitada. Tornar explícita a aplicação dos limites temporais no fluxo fixture, sem introduzir transporte real.

#### 4. Proteção de roots e cópia tem lacunas de confinamento

As verificações `is_symlink`/`is_file` e a abertura são operações separadas. Não existe abertura ancorada em descritor do root nem garantia de não seguir links durante a abertura.

Além disso, `destination_file.exists()` devolve `False` para um symlink pendente; `copyfile` pode então seguir esse link e escrever fora da árvore pretendida. Diretórios intermédios de destino também não são validados contra symlinks.

**Correção:** recusar destinos preexistentes, incluindo links pendentes, validar os componentes da árvore e usar criação exclusiva/não seguidora de links. A fixture deve demonstrar que não escreve fora do bundle.

#### 5. Contrato “fechado” ainda aceita tipos e caminhos indevidos

Exemplos aceites pelo código atual:

- `schema_version=True` ou `1.0`, por igualdade com `1`;
- `expected.count=5.0`, por igualdade com `5`;
- `/var/ng/./source`, embora `_absolute_path` prometa caminho normalizado: `PurePosixPath` elimina `.` antes da inspeção.

**Correção:** tipos inteiros estritos, excluindo booleanos, e validação da forma lexical do caminho antes da normalização.

### Pontos positivos e limites de garantia

- Planos `normal-payload` sem contrato são rejeitados na validação.
- O plano marker-only legado continua válido.
- O layout inclui as três origens, mapas, metadata e checkpoint.
- Manifest e verifier exigem as áreas payload quando os indicadores previstos estão presentes.
- A cobertura é repetida na verificação final da árvore.
- Não há novo transporte real no diff.

**Contudo, o teste do legado apenas verifica que `validate_plan` não lança exceção. Não demonstra a ausência de elegibilidade M2-R.** Essa propriedade necessita de uma asserção explícita no ponto de decisão de elegibilidade.

A cobertura por prefixos no manifest/verifier prova presença de áreas, não validade semântica dos mapas ou do checkpoint. Isso é aceitável como uma camada estrutural, mas não deve ser apresentado como prova completa de aceitação payload.

### TDD e regressões

Os testes fornecem uma boa base de happy path e duas verificações negativas de cobertura do bundle. Não cobrem os bloqueantes acima, e um diff final verde não permite confirmar a sequência RED→GREEN.

Antes do GO, acrescentar testes para:

- mapas vazios, malformados, nomes extra/ausentes e divergências de tamanho/digest;
- integração preservação→avaliação com **divergência completa → FAIL/COMPLETE**;
- matriz completa, incluindo preservação de `prior_runtime="FAIL"`;
- limites de enumeração, leitura e cópia, incluindo ficheiro alterado após inventário;
- links pendentes e componentes symlink no destino;
- tipos inválidos, campos desconhecidos e caminhos não normalizados;
- marker-only legado explicitamente **sem elegibilidade M2-R**;
- `normal-payload` sozinho a ativar cobertura no verifier e flag não booleana a ser rejeitada.

A suite canónica reportada é um sinal positivo de compatibilidade, mas não substitui estes testes negativos.

### Condições para o próximo incremento

1. Corrigir os bloqueantes ainda em âmbito **fake/fixture controller-only**.
2. Separar evidência completa de payload conforme.
3. Demonstrar limites e confinamento com testes determinísticos locais.
4. Fechar a prova de não elegibilidade M2-R do legado.
5. Repetir testes focados, suite canónica, `compileall` e `git diff --check`, submetendo o novo diff exato.

**Trial, VM, SSH e QGA continuam não autorizados.** Também permanecem fora de autorização cleanup, reboot, push, tag e release. Este parecer não amplia o GO da SPEC-060 v3 nem mistura este incremento com M1-L.
