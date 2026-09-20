# SPEC-060: Gate de evidência remota de payload para a release de engenharia v1.0

**Author:** Antonio Alberti / Hermes Agent<br>
**Date:** 2026-09-20<br>
**Status:** In Progress — gate remoto de engenharia; controller-only concluído; integração remota OPEN
**Branch:** AIOPT3
**Implementation commit:**<br>
**Related:** SPEC-052-remote-ssh-multi-vm-executor.md, SPEC-054-release-master-plan-and-audit.md, SPEC-046-multi-vm-runner-safe-teardown.md, SPEC-050-contentapp-repository-control-plane-readback.md<br>
**Task linkage:** NG-058 / M2-R / NG-050; não aplicável ao RDEMO local

## 1. Decisão e problema

O perfil remoto v1.0 precisa demonstrar o caminho normal Source → PGCS → NRNCS → Repository com cinco JPEGs e mapas independentes nome → tamanho → SHA-256.

O executor remoto actual aceita apenas `runtime_oracle.type=markers` para o modo remoto. Markers de publicação/recepção demonstram progresso, mas não preservam nem permitem re-hashar os artefactos observados em Source, no cache/área NRNCS e no Repository. O pacote de preparação do trial normal foi, por isso, mantido em `HOLD` por Astra.

Esta SPEC define o menor contrato controller-only para recolher, preservar e verificar a evidência de payload remoto antes de qualquer novo trial normal de aceitação.

Esta SPEC governa exclusivamente a futura `NovaGenesis Engineering Release 1.0 — Remote Normal Profile`. Não governa nem bloqueia o `NovaGenesis Research Demonstrator 0.1 — Local Normal Path`; o RDEMO usa o seu perfil reduzido R0–R7 e não pode reclamar conformidade com as três origens remotas desta SPEC.

## 2. Âmbito

### Incluído

- contrato versionado de `payload_evidence` para planos remotos;
- conjunto esperado de cinco JPEGs, com nome, tamanho e SHA-256;
- três origens independentes: Source, NRNCS e Repository;
- recolha remota bounded através do helper/SSH existente;
- cópias preservadas sob namespaces separados no bundle local;
- mapas calculados sobre as cópias, re-hash independente e manifest/seal;
- integração da decisão no resultado do NG-ELC remoto;
- fixtures fake SSH/helper e testes RED→GREEN controller-only;
- compatibilidade explícita com os planos marker-only diagnósticos.

### Excluído

- alterações C++, protocolo, wire format, pub/sub ou comportamento das VMs;
- Proxmox/QGA, perfil local/native-privileged ou seam QGA-loss;
- alteração do modo remoto para fallback local;
- aceitação baseada apenas em markers, contagem de mensagens, frames ou readiness;
- workload de 100/1000 fotos, stress, performance ou matriz completa de erros;
- execução de VM, trial real, `clean.sh`, reboot, push, tag ou release durante a implementação desta SPEC.

## 3. Contrato normativo

### R00 — Classes de plano e compatibilidade

O campo `payload_evidence` não redefine implicitamente todos os planos remotos:

| Classe | Regra sem `payload_evidence` |
|---|---|
| Diagnóstico marker-only legado (`diagnostic_only=true`) | Mantém a semântica anterior; pode demonstrar progresso/liveness, mas não fecha M2-R nem payload acceptance |
| Candidato normal de payload (`diagnostic_only=false`, `acceptance_profile=normal-payload`) | Rejeitado na validação pré-launch |
| Candidato de payload válido | Requer o bloco `payload_evidence` completo e os restantes gates declarados |

Para o candidato normal, `runtime_oracle.type=markers` continua permitido como oráculo de progresso, mas é adicional: não substitui `payload_evidence`. A classificação é fechada: um plano sem `payload_evidence`, sem os campos novos e com o `runtime_oracle` marker-only existente é legado diagnóstico; um plano novo `diagnostic_only=true` usa `acceptance_profile=diagnostic-marker`; um candidato normal usa exactamente `diagnostic_only=false` e `acceptance_profile=normal-payload` com o bloco completo. Qualquer combinação contraditória ou parcial MUST ser rejeitada.

### R01 — Declaração do oráculo

Um plano remoto candidato de payload MUST declarar exactamente um bloco versionado com a forma seguinte. Os digests a zero são apenas dados sintéticos do exemplo e não são um candidato executável:

```json
{
  "diagnostic_only": false,
  "acceptance_profile": "normal-payload",
  "payload_evidence": {
    "schema_version": 1,
    "expected": {
      "count": 5,
      "pattern": "*.jpg",
      "hash": "sha256",
      "map": {
        "photo-01.jpg": {"size": 4096, "sha256": "0000000000000000000000000000000000000000000000000000000000000000"},
        "photo-02.jpg": {"size": 4096, "sha256": "0000000000000000000000000000000000000000000000000000000000000000"},
        "photo-03.jpg": {"size": 4096, "sha256": "0000000000000000000000000000000000000000000000000000000000000000"},
        "photo-04.jpg": {"size": 4096, "sha256": "0000000000000000000000000000000000000000000000000000000000000000"},
        "photo-05.jpg": {"size": 4096, "sha256": "0000000000000000000000000000000000000000000000000000000000000000"}
      }
    },
    "origins": {
      "source": {"host": "source-vm", "role": "source", "path": "/var/ng/source", "service_config_ref": "source.service"},
      "nrncs": {"host": "source-vm", "role": "nrncs", "path": "/var/ng/nrncs", "service_config_ref": "nrncs.service"},
      "repository": {"host": "repository-vm", "role": "repository", "path": "/var/ng/repository", "service_config_ref": "repository.service"}
    },
    "bounds": {
      "global_timeout_s": 300,
      "per_origin_timeout_s": 60,
      "max_files_per_origin": 100,
      "max_bytes_per_origin": 10485760,
      "max_bytes_total": 31457280
    },
    "ownership": {
      "trial_id": "TRIAL-EXACT-SHA",
      "owner": "ng-elc-controller",
      "cleanup_scope": "declared-roots-only"
    },
    "nrncs_binding": {
      "role": "nrncs",
      "service_config_ref": "nrncs.service",
      "observation_ref": "subscription-readback"
    }
  }
}
```

O esquema MUST fixar `schema_version=1`, o par de campos de plano `diagnostic_only=false` e `acceptance_profile=normal-payload`, `expected.count=5`, `expected.hash=sha256` e exactamente as três chaves de origem `source`, `nrncs`, `repository`. `payload_evidence` MUST conter exactamente as chaves `schema_version`, `expected`, `origins`, `bounds`, `ownership` e `nrncs_binding`. Cada origem MUST conter exactamente `host`, `role`, `path` e `service_config_ref`. `bounds` MUST conter inteiros positivos `global_timeout_s`, `per_origin_timeout_s`, `max_files_per_origin`, `max_bytes_per_origin` e `max_bytes_total`, com `global_timeout_s >= per_origin_timeout_s`, `max_files_per_origin >= expected.count` e `max_bytes_total >= 3 * max_bytes_per_origin`. `ownership` MUST conter apenas `trial_id`, `owner` e `cleanup_scope`, sendo `cleanup_scope=declared-roots-only`; `nrncs_binding` MUST conter apenas `role=nrncs`, `service_config_ref` e `observation_ref`. Campos desconhecidos, chaves JSON duplicadas e tipos inválidos MUST ser rejeitados.

Cada entrada do mapa MUST ter nome basename case-sensitive sem `/`, `\\`, NUL, `.`/`..` ou path traversal; tamanho inteiro `>=1`; e digest hexadecimal lowercase com exactamente 64 caracteres. O conjunto de nomes MUST ter cardinalidade cinco e ser coerente com `pattern`; extras não podem desaparecer por filtragem posterior.

O plano efectivo MUST congelar paths absolutos/expandidos, host, trial ID, workload e regras de ownership antes do launch. Paths relativos no plano são rejeitados; a expansão de uma variável para um path absoluto é legítima apenas antes do freeze e deve ser preservada no plano efectivo. O mapa esperado MUST ser produzido pelo gerador/workload associado ao trial e não inferido a partir de uma origem observada depois. A extensão `.jpg` e o hash não provam formato JPEG; se o formato for requisito, o gerador/verificador MUST declarar e testar essa propriedade separadamente.

`pattern="*.jpg"` restringe apenas o conjunto esperado; a enumeração efectiva percorre a raiz exclusiva inteira, sem filtro que esconda extras. Qualquer ficheiro não esperado, subdirectório, symlink ou entrada fora da regra de nomes bloqueia aceitação. A enumeração e a abertura das origens devem ser ancoradas em descritores de directório obtidos com `O_NOFOLLOW`; `lstat` seguido de uma abertura por path absoluto não é suficiente contra troca de ancestrais.

### R02 — Independência das origens

- Source, NRNCS e Repository MUST ser recolhidos como origens separadas.
- Aliasing, nesting, symlink escape, path partilhado ou cópia Source usada como substituto MUST bloquear aceitação.
- A ausência de uma origem MUST permanecer explícita; não pode ser convertida em mapa vazio.
- Markers podem apoiar liveness/progresso, mas não substituem as três origens.
- A independência é de origem e função, não exige hosts distintos: Source e NRNCS podem estar no mesmo guest, mas o helper MUST registar host/guest, role/configuração declarada, path resolvido, `st_dev`/`st_ino` do root quando disponível e identidade efectiva da recolha.
- Symlink, hardlink, bind mount, alias de endpoint ou identidade não demonstrável MUST resultar em `PAYLOAD_ORIGIN_UNPROVEN` e não-elegibilidade.
- A área NRNCS MUST ser ligada à configuração efectiva do serviço/role NRNCS; um directório apenas preenchido para satisfazer o oráculo não é evidência válida.

### R03 — Recolha e preservação

A recolha MUST ocorrer dentro da janela bounded de finalização, depois de parar os writers e antes da remoção de temporários:

```text
stop → remote collect por origem → validar cópia → calcular mapa
→ preservation checkpoint → temporary cleanup → inventário final
→ result → manifest → offline verification → terminal seal
```

Cada origem preservada MUST incluir:

- namespace canónico próprio (`payload/origins/source`, `payload/origins/nrncs`, `payload/origins/repository`);
- path remoto de origem, host/guest, trial ID e identidade do helper;
- cópia regular, sem symlink inseguro;
- tamanho e SHA-256 da cópia;
- mapa de nomes/tamanhos/SHA-256 calculado sobre a cópia;
- indicação de truncagem, recolha incompleta ou erro.

O layout canónico do bundle é:

```text
payload/
  expected.json
  origins/source/files/...
  origins/nrncs/files/...
  origins/repository/files/...
  maps/source.json
  maps/nrncs.json
  maps/repository.json
  metadata/source.json
  metadata/nrncs.json
  metadata/repository.json
```

`files/` contém os bytes preservados; `maps/` contém os mapas re-hasháveis; `metadata/` contém path, identidade, bounds, sequência e estado de recolha. O manifest cobre todos esses objectos e os artefactos de resultado definidos, não o seu próprio ficheiro, o relatório de verificação ou o terminal seal.

Limites obrigatórios: deadline global do trial, timeout finito por origem, máximo de ficheiros, máximo de bytes por origem e quota agregada declarada no plano. `max_bytes_total` limita estritamente os bytes consumidos pela inventariação das origens, incluindo leituras parciais que terminam em erro; quando o saldo chega a zero, nenhuma origem subsequente pode iniciar leitura. A cópia local de preservação é uma segunda operação bounded por `max_bytes_per_origin`; a métrica `copied_bytes` representa bytes preservados com sucesso e não altera a quota agregada de bytes observados. Se a quota futura tiver de abranger todo o I/O de cópia, será necessário um campo/contrato próprio. Os deadlines deste incremento são cooperativos: há checkpoints antes e depois das fases controladas de enumeração, restat, leitura, escrita e `fsync`; uma chamada de sistema pode terminar depois do prazo, mas qualquer checkpoint que observe expiração deve classificar a operação como incompleta e nunca como PASS. Não há retry automático da recolha; uma tentativa repetida exige sequência/evento explícito e permanece não-PASS quando a primeira recolha foi ambígua.

Falha de SCP/SSH, timeout, quota, leitura instável, origem ausente ou hash inconsistente MUST produzir a matriz de resultado R05, nunca PASS. A preservação MUST escrever um checkpoint objectivo antes de qualquer temporary cleanup; se o checkpoint falhar, a única cópia restante não pode ser removida.

### R04 — Decisão funcional

O payload runtime só pode ser `PASS` quando:

1. o conjunto esperado tem exactamente cinco entradas;
2. cada origem tem exactamente o mesmo conjunto de nomes;
3. cada entrada tem tamanho e SHA-256 iguais ao mapa esperado;
4. não existem extras, faltas ou links inseguros;
5. as três cópias foram preservadas e re-hashadas independentemente;
6. readiness/subscription/readback declarados pelo plano foram observados;
7. não ocorreu falha runtime sticky nem interrupção de recolha.

Source igual a Repository não basta: NRNCS deve ser uma origem independente.

### R05 — Matriz normativa de resultados

| Situação | Runtime de payload | `evidence_result` | `local_acceptance_eligible` |
|---|---|---|---|
| Três origens completas, iguais ao esperado, restantes gates satisfeitos | `PASS` | `COMPLETE` | pode ser `true`, sujeita aos gates externos |
| Recolha ausente, instável, truncada, interrompida ou identidade da origem não provada | `INCONCLUSIVE`, salvo `FAIL` sticky anterior | `INCOMPLETE` | `false` |
| Divergência conclusiva numa observação válida e completa | `FAIL` sticky | `COMPLETE` ou `INCOMPLETE` conforme a preservação | `false` |
| Manifest/hash/cobertura/seal inválido | preserva o estado anterior; nunca promove a PASS | `INCOMPLETE` | `false` |

Um `FAIL` já observado não pode ser apagado por erro posterior de recolha. Repository vazio é `UNPROVEN`/`INCONCLUSIVE` quando a recolha/readiness não permite estabelecer ausência conclusiva; só é falha funcional quando o serviço, a origem e a janela de observação estão comprovadamente válidos e completos.

### R06 — Manifest, verificação e seal

Antes do seal, o controller MUST verificar cópias, mapas, cobertura, provenance, ownership e manifest staged. O manifest MUST cobrir `payload/expected.json`, `payload/origins/source/`, `payload/origins/nrncs/`, `payload/origins/repository/`, `payload/maps/`, `payload/metadata/`, estados de recolha, result e todos os artefactos obrigatórios, excluindo apenas os objectos definidos para evitar ciclo.

Depois de escrever o manifest, o verifier offline MUST reabrir e re-hashar as cópias preservadas sem confiar nos digests do collector. O relatório de verificação referencia o hash do manifest e fica fora do manifest para evitar ciclo. O terminal seal referencia o hash do manifest, do result e do relatório de verificação conforme o contrato de integridade reutilizado de SPEC-056; apenas esse contrato controller-only é reutilizado, sem importar a dependência operacional M1-L/QGA. O seal publicado MUST ser reaberto e validado. Nenhum ficheiro coberto pode ser alterado depois do seal. `accepted=true` nunca pode apoiar-se apenas numa verificação anterior ao seal.

### R07 — Compatibilidade funcional e aceitação

- Para um candidato `normal-payload`, markers completos sem `payload_evidence` completo: runtime `INCONCLUSIVE`, não PASS.
- Para um plano `diagnostic_only` marker-only, a semântica runtime anterior é preservada, mas `local_acceptance_eligible=false` para M2-R.
- Source completo e Repository vazio: entrega `UNPROVEN`/`INCONCLUSIVE`, sem diagnóstico causal automático, quando a recolha/readiness não demonstram uma observação conclusiva; se a origem foi válida, completa e quiescente, é divergência funcional e torna-se `FAIL` sticky.
- Falha observada de conteúdo após todas as origens estarem prontas: runtime `FAIL`, sticky.
- Bundle intacto mas não elegível: integridade válida, `accepted=false`.
- Qualquer falha de manifest, hash, cobertura ou seal: `evidence_result=INCOMPLETE` e `accepted=false`.

## 4. Estado da implementação e integração remota

O incremento controller-only desta SPEC está concluído no escopo de componente e fixture:

1. valida o bloco `payload_evidence` sem contactar guests;
2. define helpers bounded para mapas e comparação fail-closed;
3. preserva três origens fake/fixture em namespaces distintos;
4. integra preservation, manifest, terminal seal e verificação offline em fixture sem VM;
5. mantém planos marker-only existentes sem alteração semântica;
6. fecha a attestation do verifier no seam `evaluate_remote_acceptance`;
7. passa a suite focada e a suite completa do controller, com `compileall` e `git diff --check`.

Este estado não demonstra recolha remota produtiva. O bloqueio efectivo que permanece é a ausência de collector/helper bounded ligado ao ciclo remoto e a produção end-to-end de um bundle v2 durante `run_trial`. Não acrescentar novo hardening controller-only como pré-requisito do RDEMO; reabrir implementação apenas perante um blocker concreto de M2-R ou da integração remota.

A implementação controller-only MUST NOT ser interpretada como autorização para executar VMs, SSH real, QGA, `clean.sh`, reboot, trial, push, tag, release, alterar C++/protocolo ou promover M2-R.

### 4.1 — Attestation do verifier no seam remoto

O seam `evaluate_remote_acceptance` está fechado apenas para impedir aceitação sem attestation válida; não inicia recolha remota nem altera o ciclo de execução/teardown.

Para um plano `normal-payload`, `payload_result` só pode satisfazer a decisão de aceitação quando contiver simultaneamente:

- `status=COMPLETE`;
- `accepted=true`;
- `offline_verification` como objecto;
- `offline_verification.code=NGELC-VALID`;
- `offline_verification.integrity=true`;
- `offline_verification.accepted=true`;
- `offline_verification.schema_version=2`.

Um resumo que contenha apenas `status=COMPLETE` e `accepted=true` MUST ser rejeitado como attestation insuficiente. A ausência, tipo inválido ou valor contraditório em qualquer campo MUST produzir `local_acceptance_eligible=false` e um blocker estável. O fluxo remoto actual continua a passar `payload_result=None` até existir collector/helper bounded e bundle v2 efectivamente produzido; este incremento não pode transformar essa ausência em PASS.

O teste deste seam deve ser feito com payloads sintéticos e relatório de verifier fake/fixture, sem VM, SSH, QGA, cleanup, reboot, trial ou alteração de protocolo.

### 4.2 — Anexo: perfil RDEMO local

O RDEMO não é candidato `normal-payload` desta SPEC. Para evitar ambiguidade documental:

- Source/NRNCS/Repository como três origens remotas: `N/A`/diferido no RDEMO;
- mapa Source/Repository, cinco JPEGs, lifecycle local bounded, teardown por identidade, manifest/seal schema-v2 e verifier: governados por `NG-060` e pelo Plano Mestre R0–R7;
- marker-only: compatível para diagnóstico, nunca aceitação do RDEMO;
- M0/M1-R/M2-R/M3 e a release `v1.0.0-AIOPT3`: não fechados pelo RDEMO;
- QGA-loss, multi-VM, readback de protocolo e subscriptions completas: `N/A`/diferidos.

Esta separação é de aplicabilidade, não uma redução silenciosa do contrato remoto.

## 5. Testes obrigatórios

- bloco ausente, versão inválida, origem repetida ou path relativo → rejeição antes de launch;
- plano `diagnostic_only` marker-only sem `payload_evidence` → compatibilidade mantida, mas não elegível para M2-R;
- plano normal sem `payload_evidence` → rejeição antes de launch;
- chaves duplicadas, campos desconhecidos, nomes inseguros, tamanho inválido ou digest não hexadecimal → rejeição;
- mapa esperado com cinco nomes e uma origem ausente → não-PASS;
- extra, missing, tamanho divergente e hash divergente em cada origem → resultado sticky não-PASS;
- Source e Repository iguais mas NRNCS ausente → `UNPROVEN`/`INCONCLUSIVE`;
- symlink, aliasing, nesting e escape → zero aceitação;
- root/ficheiro com identidade efectiva não demonstrável → `PAYLOAD_ORIGIN_UNPROVEN` e não-elegibilidade;
- falha/timeout de recolha de cada guest → cópia parcial explicitamente marcada, seal não aceite;
- timeout global, quota por origem ou quota agregada → preservação incompleta e não-PASS;
- writer vivo, stop incompleto ou leitura instável → checkpoint de preservação não concluído;
- mutação da cópia após cálculo → re-hash/manifest detecta tamper;
- nomes de ficheiro e paths malformados → rejeição fail-closed;
- bundle íntegro e não elegível → verifier `integrity=true`, `accepted=false`;
- plano marker-only remoto anterior → regressão sem alteração;
- fake SSH/helper demonstra stop → collect → preservation → cleanup → manifest → seal;
- nenhuma fixture usa `local_observe` como substituto do caminho remoto.
- parser com chaves JSON duplicadas e campos desconhecidos falha explicitamente;
- verifier re-hasha bytes preservados e detecta alteração pós-collector;

## 6. Critérios de aceitação da SPEC

- [x] Contrato `payload_evidence` versionado, fechado e documentado; classes marker-only versus candidato normal são explícitas.
- [x] Três origens independentes e namespaces separados implementados no escopo controller-only/fixture.
- [x] Mapas expected/Source/NRNCS/Repository preservados e re-hasháveis no escopo controller-only/fixture.
- [x] Falhas de recolha e divergências impedem PASS.
- [x] Manifest/seal/verificador cobrem as novas áreas no escopo controller-only/fixture.
- [x] Planos marker-only continuam compatíveis.
- [x] Testes RED→GREEN e suite completa passam no intérprete canónico.
- [x] Diff exacto do incremento controller-only revisto por Astra.
- [x] Nenhum trial real executado nesta SPEC.
- [ ] Collector/helper bounded integrado ao ciclo remoto.
- [ ] `run_trial` produz bundle v2 real das origens remotas e attestation ligada ao exact-SHA.
- [ ] Um plano candidato normal é criado/activado depois da revisão Astra e da conclusão de M0/M1-R.
- [ ] Revisão Astra do diff do collector e dos resultados end-to-end concluída antes da preparação operacional.

## 7. Relação com a release

Esta SPEC fecha uma lacuna de M2-R, mas não fecha M2-R sozinha. A release de engenharia v1.0 continua dependente de M0, M1-R, SPEC-044/045/047/048/049/050, provenance/build, teardown remoto, revisão Astra e auditoria sem drift. O RDEMO não depende desta SPEC e não pode usar os seus fixtures para reivindicar aceitação remota. A referência a SPEC-056 limita-se ao contrato controller-only de re-hash/manifest/seal; não importa a tarefa, o perfil local ou a dependência operacional M1-L/QGA.

SPEC-055/056/058/059 e o seam QGA-loss permanecem na linha M1-L/v1.1+; esta SPEC não reintroduz dependência operacional de QGA nem altera a rebaseline aprovada.

## 8. Rollback

Desactivar o bloco `payload_evidence` nos planos candidatos e preservar os fixtures/documentação. Não alterar os planos marker-only existentes, não apagar resultados históricos e não promover um bundle sem mapas independentes a aceitação.
