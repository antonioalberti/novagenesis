# Auditoria da Release NovaGenesis

`ng_release_audit.py` verifica a rastreabilidade da Release `v1.0.0` sem modificar o repositório, as tarefas ou as SPECs.

## Execução

A partir da raiz do repositório:

```bash
python3 Scripts/ProjectAudit/ng_release_audit.py \
  --repo . \
  --plan Specs/RELEASE-MASTER-PLAN-v1.0.0.md \
  --vault <obsidian-vault>
```

Para automação:

```bash
python3 Scripts/ProjectAudit/ng_release_audit.py \
  --repo . \
  --plan Specs/RELEASE-MASTER-PLAN-v1.0.0.md \
  --vault <obsidian-vault> \
  --json
```

O comando devolve código `0` somente quando o plano tem gates e não existem problemas bloqueantes. Um resultado `BLOCKED` é esperado enquanto a release não estiver congelada.

## O que verifica

- gates e referências do plano mestre;
- links e frontmatter das tarefas Obsidian;
- status, branch e implementation commit das SPECs;
- commits existentes e ancestrais do candidato;
- árvore Git dirty;
- caminhos de evidência declarados;
- referências ghost a SPECs;
- referências AIOPT2 fora de documentos históricos;
- lacunas de rastreabilidade.

A presença de um ficheiro de resultado não prova o seu conteúdo. A aceitação continua a exigir o oráculo específico, manifesto, hashes, configuração e revisão previstos no plano mestre.
