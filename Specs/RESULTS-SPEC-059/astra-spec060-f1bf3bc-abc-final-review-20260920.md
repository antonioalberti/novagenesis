# Astra Review — SPEC-060 `f1bf3bc` e proposta A/B/C `0f410ca`

**Data:** 2026-09-20<br>
**Modelo:** `gpt-6-astra`<br>
**Baseline código:** `303acbf`<br>
**Candidato funcional:** `f1bf3bc`<br>
**Baseline documental da revisão:** `b6fbcef`<br>
**Candidato documental:** `0f410ca`<br>
**Estado:** `CONDITIONAL GO controller-only`; proposta `CONDITIONAL GO documental`

## 1. SPEC-060 — `f1bf3bc`

### Veredicto

**CONDITIONAL GO**, restrito à remediação controller-only. O incremento melhora materialmente o confinamento das leituras, mas não fecha incondicionalmente todas as afirmações normativas.

### Verificações positivas

- Inventário e leitura das origens usam travessia ancorada em descritores, `O_NOFOLLOW`, `os.scandir(root_fd)`, `os.stat(..., dir_fd=root_fd, follow_symlinks=False)` e abertura relativa dos filhos.
- A preservação reabre a raiz por travessia segura e compara `st_dev/st_ino` antes da cópia.
- `fstat` após enumeração e leitura detecta alterações observáveis de identidade, modo, `mtime_ns` e `ctime_ns`; isto não é snapshot atómico nem prova sobre o pathname original depois da abertura.
- `max_bytes_total` esgotado impede iniciar a inventariação de origens posteriores e os bytes consumidos são acumulados.
- A travessia da raiz `/` está corrigida e coberta.
- Os deadlines são cooperativos, não hard real-time, com verificações adicionais em restats e após `fsync`.

### Pendências concretas

1. A especificação deve alinhar a frase “antes e depois de cada operação” com a instrumentação realmente demonstrada. Abertura inicial de `/`, `fstat` da raiz durante cópia e `next(iterator)` não constituem pares explícitos em todos os pontos.
2. A contabilidade da cópia deve ficar explicitamente definida: `max_bytes_total` limita bytes consumidos na inventariação das origens; `copied_bytes` representa bytes preservados com sucesso. Se no futuro a quota abranger todo o I/O de cópia, será necessário campo/contrato próprio.
3. A cobertura TDD adicional ainda não demonstra directamente leitura parcial seguida de erro, mutação durante a operação, troca de entrada/ancestral ou timeout durante escrita/`fsync`. Estes continuam gaps de evidência, não autorização para runtime.

### Admissibilidade

É admissível continuar com inspeção exact-SHA e testes controller-only locais. Qualquer alteração adicional de código ou SPEC exige novo SHA e nova revisão. Não há autorização operacional.

## 2. Proposta A/B/C — `0f410ca`

### Veredicto

**CONDITIONAL GO** como proposta documental, não como decisão operacional aprovada.

A revisão confirma que a proposta:

- separa aprovação/autorização, preparação, execução e saída;
- exige autorização explícita para preparação;
- restaura Repository vazio ou namespace exclusivamente isolado com evidência equivalente;
- torna Source/Repository file-map obrigatório para A;
- condiciona readback, persistência NRNCS e subscription semantics às claims correspondentes;
- distingue candidato funcional de base documental.

### Condições de incorporação

- Registar `f1bf3bc` como revisão técnica condicional, sem fechar o gate técnico.
- Manter a proveniência documental explícita: baseline `b6fbcef`, base declarada `03a9e14`, candidato documental `0f410ca`, candidato funcional `f1bf3bc`.
- Preservar `PROPOSTA — não aprovada para trial`.
- Manter exclusões como exclusões e não promover file-map a prova de readback, persistência ou subscrição.

A proposta pode ser incorporada no Plano Mestre apenas como proposta condicional rastreável; não está aprovada como rebaseline operacional.

## 3. Limites

- Não foi demonstrada nem reivindicada integração de recolha de payload em produção.
- Marker-only permanece `local_acceptance_eligible=false`.
- Normal-payload sem recolha permanece nonzero e não elegível.
- Controlos históricos permanecem não acceptance do candidato actual.
- Nenhuma VM, SSH, QGA, cleanup, reboot, push, tag, trial ou release pública foi autorizada.
