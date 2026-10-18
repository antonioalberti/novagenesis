# Pinned Git-object evidence for SPEC-071 v0.13 design follow-up

This bundle contains exact excerpts from the NG-070 pinned Git objects. It is documentary design evidence only; it does not prove acceptance or performance. Worktree overlays are separate and are identified in the packet manifest.

## SPEC-031 branch-object identity

- `AIOPT3` `24a3851e1d5a90f039b9e148a1af38dd03c4d925`: `Specs/SPEC-031-recvmmsg-batched-receive.md`, 8103 bytes, SHA-256 `b4984442dbb866d5fcf1be7be1ae011a9249be9984e56a6677ea9b95a9b9606e`.
- `AIOPT4` `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`: `Specs/SPEC-031-recvmmsg-batched-receive.md`, 8103 bytes, SHA-256 `b4984442dbb866d5fcf1be7be1ae011a9249be9984e56a6677ea9b95a9b9606e`.
- `AIOPT5` `8d2386dcb10d2b37f133d4b24c2cb96761753d82`: `Specs/SPEC-031-recvmmsg-batched-receive.md`, 8103 bytes, SHA-256 `b4984442dbb866d5fcf1be7be1ae011a9249be9984e56a6677ea9b95a9b9606e`.

### SPEC-031 shared committed clauses (AIOPT3/AIOPT4/AIOPT5 object content is byte-identical)

Snapshot: `AIOPT3` HEAD `24a3851e1d5a90f039b9e148a1af38dd03c4d925`; Git-object path `Specs/SPEC-031-recvmmsg-batched-receive.md`; bytes `8103`; SHA-256 `b4984442dbb866d5fcf1be7be1ae011a9249be9984e56a6677ea9b95a9b9606e`.

Lines 1–5:

```text
1: # SPEC-031 — Batched RAW Receive with `recvmmsg()`
2: 
3: **Status:** Draft — approval required before implementation
4: **Scope:** One performance change: receive-side syscall batching.
5: **Sequence:** User approves → implementation → Hermes verifies → Hermes benchmarks.
```

Lines 9–13:

```text
9: - Phase 1 signed off; SPEC-028-B heartbeat and SPEC-030 verified.
10: - Reference: 500 msg/s bidirectional, measured zero loss, one-way delay 0.13/0.64 ms, RSS ~2.8 MB, PGCS ~1 busy core per VM.
11: - Current path: `NGAL_Transport_RAW::ReceiveDispatcher` polls the SSID collection, calls one `recvfrom()` per ready socket, strips the 14-byte Ethernet header, calls `SAR.ReceiveFragment`, and invokes `DeliverToGateway` on completion.
12: - Change only reception mechanics. Preserve framing, SAR, delivery, socket configuration, and wire format.
13: - No send batching, affinity changes, socket-buffer tuning, SAR changes, or NRNCS stale-cache fix in this benchmark.
```

Lines 17–21:

```text
17: ### Placement and allocation
18: 
19: Replace the ready-socket `recvfrom()` operation inside `ReceiveDispatcher` with `recvmmsg()`.
20: 
21: - Batch size: **32 frames** per call.
```

### AIOPT3 ReceiveDispatcher recvmmsg source excerpts; call lines [310, 319]

Snapshot: `AIOPT3` HEAD `24a3851e1d5a90f039b9e148a1af38dd03c4d925`; Git-object path `PGCS/src/NGAL_Transport_RAW.cpp`; bytes `15010`; SHA-256 `3a718ffc759eaaaf131affd203eee3d4449607408f60f1cd1ffb2e7292e37682`.

Lines 307–315:

```text
307:         for (unsigned int attempt = 0; attempt < 2; ++attempt)
308:         {
309:           pool->Reset();
310:           const int nrec = recvmmsg(fds[i].fd, pool->messages,
311:                                     RawReceivePool::BatchSize,
312:                                     MSG_DONTWAIT | MSG_TRUNC, NULL);
313: 
314:           if (nrec <= 0)
315:           {
```

Lines 316–324:

```text
316: #ifdef DEBUG
317:             if (nrec < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
318:             {
319:               cerr << "[WARN] NGAL_Transport_RAW::ReceiveDispatcher: recvmmsg() error fd=" << fds[i].fd
320:                    << " errno=" << errno << " (" << strerror(errno) << ")" << endl;
321:             }
322: #endif
323:             break; // EAGAIN/EWOULDBLOCK: drain complete; 0: nothing more this turn
324:           }
```

### AIOPT4 ReceiveDispatcher recvmmsg source excerpts; call lines [310, 319]

Snapshot: `AIOPT4` HEAD `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`; Git-object path `PGCS/src/NGAL_Transport_RAW.cpp`; bytes `15010`; SHA-256 `3a718ffc759eaaaf131affd203eee3d4449607408f60f1cd1ffb2e7292e37682`.

Lines 307–315:

```text
307:         for (unsigned int attempt = 0; attempt < 2; ++attempt)
308:         {
309:           pool->Reset();
310:           const int nrec = recvmmsg(fds[i].fd, pool->messages,
311:                                     RawReceivePool::BatchSize,
312:                                     MSG_DONTWAIT | MSG_TRUNC, NULL);
313: 
314:           if (nrec <= 0)
315:           {
```

Lines 316–324:

```text
316: #ifdef DEBUG
317:             if (nrec < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
318:             {
319:               cerr << "[WARN] NGAL_Transport_RAW::ReceiveDispatcher: recvmmsg() error fd=" << fds[i].fd
320:                    << " errno=" << errno << " (" << strerror(errno) << ")" << endl;
321:             }
322: #endif
323:             break; // EAGAIN/EWOULDBLOCK: drain complete; 0: nothing more this turn
324:           }
```

### AIOPT5 ReceiveDispatcher recvmmsg source excerpts; call lines [319, 331]

Snapshot: `AIOPT5` HEAD `8d2386dcb10d2b37f133d4b24c2cb96761753d82`; Git-object path `PGCS/src/NGAL_Transport_RAW.cpp`; bytes `16532`; SHA-256 `bf1e4f96d8bc0d612aad3978e9b6f37c9f909c6511eddf66114006c5a8df727a`.

Lines 316–324:

```text
316:           NGAL_STAGE_TIMER_START(rawReceiveTimer,
317:                                  NGALStageTelemetry::Timing::RawReceiveCall);
318: #endif
319:           const int nrec = recvmmsg(fds[i].fd, pool->messages,
320:                                     RawReceivePool::BatchSize,
321:                                     MSG_DONTWAIT | MSG_TRUNC, NULL);
322: #ifdef SPEC027_STAGE_DIAGNOSTICS
323:           NGAL_STAGE_TIMER_STOP(rawReceiveTimer);
324: #endif
```

Lines 328–336:

```text
328: #ifdef DEBUG
329:             if (nrec < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
330:             {
331:               cerr << "[WARN] NGAL_Transport_RAW::ReceiveDispatcher: recvmmsg() error fd=" << fds[i].fd
332:                    << " errno=" << errno << " (" << strerror(errno) << ")" << endl;
333:             }
334: #endif
335:             break; // EAGAIN/EWOULDBLOCK: drain complete; 0: nothing more this turn
336:           }
```

### SPEC-069 committed scope, deletion contract, and verification instruction

Snapshot: `AIOPT5` HEAD `8d2386dcb10d2b37f133d4b24c2cb96761753d82`; Git-object path `Specs/SPEC-069-ng039-public-evidence-deduplication.md`; bytes `7915`; SHA-256 `c97621ff5ac76f58fa24f305d239f55318a57e911642bd74d9c1240d82e42866`.

Lines 1–5:

```text
1: # SPEC-069: NG-039 Public Evidence Deduplication
2: 
3: **Version:** v1.0
4: **Date:** 2026-10-17
5: **Author:** Antonio Alberti / Hermes Agent
```

Lines 12–24:

```text
12: ## 1. Problem and objective
13: 
14: `Specs/RESULTS-SPEC-039/` contains identical copies of raw Astra response streams under both `*.raw.sse` and session-local `*.last.sse` names. The duplicate files increase the public working-tree footprint without adding distinct bytes of evidence. This bounded change removes only the 11 unreferenced session-local copies listed below; it retains every canonical `*.raw.sse` byte-for-byte.
15: 
16: ## 2. Scope and authority
17: 
18: The owner requested creation and implementation of this task on 2026-10-17, then explicitly authorised proceeding with publication. This authorises one normal commit/push of the exact allowlist and this SPEC to AIOPT5. It does not authorise public history rewriting, deletion of any other records, or changes to D2/D3 evidence.
19: 
20: Included: deletion of the 11 listed `*.last.sse` paths after rechecking exact byte equality and references; retain the matching `*.raw.sse` files and all other files.
21: 
22: Excluded: all other duplicate-content groups, including named fixtures and submitted packet copies; archival/migration; compression or rewriting of raw responses; history rewriting/force-push; unrelated pre-existing working-tree changes.
23: 
24: Exception retained: `Specs/RESULTS-SPEC-039/astra-session-v1.14-r12-20261013/ng039-v114-r12-documentary-review.last.sse` is byte-identical to its `.raw.sse` counterpart but is explicitly named by the immutable `raw_sse_path_in_session` field in `astra-review-v1.14-r12-20261013-submission.json`. Keep both paths; editing that submission receipt is outside this bounded cleanup.
```

Lines 26–31:

```text
26: ## 3. Exact deletion allowlist
27: 
28: For each row, the retained raw file and deletion candidate must have the listed SHA-256 and byte count before deletion.
29: 
30: | Delete this exact path | Retain this exact path | SHA-256 | Bytes |
31: |---|---|---|---:|
```

Lines 52–66:

```text
52: 
53: 1. Before deletion, every listed pair exists, matches the exact size and SHA-256 in §3, and is byte-identical.
54: 2. No pre-existing repository file or NG-039 task note references any deletion path. The path references in this SPEC §3 are the approved allowlist; retained raw paths remain referenced wherever provenance requires it.
55: 3. Exactly the 11 allowlisted paths are deleted from `Specs/RESULTS-SPEC-039/`; the 11 retained raw files remain byte-identical. The r12 `.last.sse` remains because an immutable submission receipt references that exact path.
56: 4. The results directory changes from 145 files / 18,164,472 bytes to 134 files / 13,928,653 bytes: a reduction of 11 files / 4,235,819 bytes.
57: 5. `git diff --check` passes. No non-allowlisted pre-existing change is altered or staged.
58: 6. Publish the exact allowlist and this SPEC using a normal fast-forward commit/push to AIOPT5; verify the remote commit SHA and affected paths. Do not rewrite history; state that prior copies remain accessible in Git history.
59: 
60: ## 6. Verification and rollback
61: 
62: Use a read-only Python probe to verify the pair hashes, sizes, and byte equality immediately before removal. After removal, rerun the same allowlist/hash check, scan for path references, count the results directory, inspect `git diff --name-status`, and run `git diff --check`.
63: 
64: Rollback is limited to restoring a deleted duplicate from its retained byte-identical raw counterpart if an acceptance check fails. Do not use `git restore` on unrelated dirty files. Stop if any pair differs or a reference is discovered.
65: 
66: ## 7. Review and publication gates
```

### AIOPT5 committed reference/applicability example: Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md

Snapshot: `AIOPT5` HEAD `8d2386dcb10d2b37f133d4b24c2cb96761753d82`; Git-object path `Specs/SPEC-044-core-run-evaluate-invalid-pg-downcast.md`; bytes `7128`; SHA-256 `bc4748916db0bbeef1ede3fa4463b9c5a5138af5495ae8add2617897c59c108e`.

Lines 1–12:

```text
1: # SPEC-044: Corrigir downcast inválido em CoreRunEvaluate01
2: 
3: **Author:** Antonio Alberti / Hermes
4: **Date:** 2026-09-12
5: **Status:** In Progress
6: **Branch:** AIOPT3
7: **Implementation commit:** —
8: **Related:** SPEC-041-gw-deadline-aware-wait.md, SPEC-040-common-performance-build-profiles.md
9: 
10: ## 1. Problema e evidência
11: 
12: O build Sanitizer do `PGCS` alcança o fluxo de avaliação e o UBSan reporta:
```

Lines 28–30:

```text
28: 
29: O relatório foi observado durante o smoke Sanitizer de SPEC-041. O defeito é independente da alteração de espera do Gateway e deve ser tratado separadamente.
30: 
```

Lines 83–85:

```text
83: 
84: Reverter apenas a alteração em `CoreRunEvaluate01.cpp`, preservando a fixture, o log RED e a evidência de revisão. Não reverter nem misturar as alterações de SPEC-040/041.
85: 
```

Lines 105–107:

```text
105: - `Specs/RESULTS-SPEC-044/vm-101-102-100-photo-20260912.md` — runtime multi-VM normal-path no commit rebuilt; não substitui a matriz de subscriptions.
106: - SPEC relacionada: `Specs/SPEC-041-gw-deadline-aware-wait.md`.
107: 
```

Lines 109–111:

```text
109: 
110: Esta SPEC separa formalmente o diagnóstico UBSan de SPEC-041. A alteração allowlisted foi implementada após revisão Astra e autorização do utilizador. Builds e smoke Sanitizer passaram; a aceitação funcional permanece aberta até à execução da matriz da SPEC-045 e à revisão Astra pós-correção.
111: 
```

### AIOPT5 committed reference/applicability example: Specs/SPEC-045-core-evaluate-functional-harness.md

Snapshot: `AIOPT5` HEAD `8d2386dcb10d2b37f133d4b24c2cb96761753d82`; Git-object path `Specs/SPEC-045-core-evaluate-functional-harness.md`; bytes `8319`; SHA-256 `28946c341d78e81e1d8edfe06c83d5c37275f07b698ac6d9d3208f2cce96bcf4`.

Lines 1–12:

```text
1: # SPEC-045: Harness funcional para validação de CoreRunEvaluate01
2: 
3: **Author:** Antonio Alberti / Hermes
4: **Date:** 2026-09-12
5: **Status:** In Progress
6: **Branch:** AIOPT3
7: **Implementation commit:** —
8: **Related:** SPEC-044-core-run-evaluate-invalid-pg-downcast.md, SPEC-041-gw-deadline-aware-wait.md
9: 
10: ## 1. Problema
11: 
12: A SPEC-044 removeu um downcast UBSan inválido e passou os smoke tests de build e arranque. Contudo, o repositório não possui testes `ctest` registados nem um harness que consiga observar deterministicamente as branches funcionais de `CoreRunEvaluate01::CheckSubscriptions()`.
```

### AIOPT3 committed dated register claim; inventory/history context

Snapshot: `AIOPT3` HEAD `24a3851e1d5a90f039b9e148a1af38dd03c4d925`; Git-object path `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`; bytes `5721`; SHA-256 `0b002488b4e659a7e69f20079c4363b419f112da75d39e4dd47dd02885d01df3`.

Lines 1–5:

```text
1: # SPEC-STATUS-REGISTER.md — Mestra de todas as SPECs NovaGenesis
2: 
3: **Base:** Commit `cd804a7` (branch AIOPT3) — 2026-07-16  
4: **Total:** 24 SPECs em `Specs/`
5: 
```

### AIOPT4 committed dated register claim; inventory/history context

Snapshot: `AIOPT4` HEAD `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`; Git-object path `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`; bytes `7094`; SHA-256 `dbb167a14dedccd8804bdf141ede5ba3eccf7416baf059e06e61865ab1815b2c`.

Lines 1–5:

```text
1: # SPEC-STATUS-REGISTER.md — Mestra de todas as SPECs NovaGenesis
2: 
3: **Base:** Commit `cd804a7` (branch AIOPT3) — 2026-07-16  
4: **Total:** 24 SPECs em `Specs/`
5: 
```

### AIOPT5 committed dated register claim; inventory/history context

Snapshot: `AIOPT5` HEAD `8d2386dcb10d2b37f133d4b24c2cb96761753d82`; Git-object path `Docs/DECISIONS/SPEC-STATUS-REGISTER.md`; bytes `5721`; SHA-256 `0b002488b4e659a7e69f20079c4363b419f112da75d39e4dd47dd02885d01df3`.

Lines 1–5:

```text
1: # SPEC-STATUS-REGISTER.md — Mestra de todas as SPECs NovaGenesis
2: 
3: **Base:** Commit `cd804a7` (branch AIOPT3) — 2026-07-16  
4: **Total:** 24 SPECs em `Specs/`
5: 
```

### AIOPT3 committed AIOPT3-origin branch-scope example

Snapshot: `AIOPT3` HEAD `24a3851e1d5a90f039b9e148a1af38dd03c4d925`; Git-object path `Specs/SPEC-054-release-master-plan-and-audit.md`; bytes `10992`; SHA-256 `bf2a85323a7994eb7bac6395881b6e7b4ed3d01c3d4382660f147881f1b3b7fe`.

Lines 1–12:

```text
1: # SPEC-054: Plano mestre e auditoria determinística da Release v1.0.0
2: 
3: **Author:** Antonio Alberti / Hermes Agent  
4: **Date:** 2026-09-13  
5: **Status:** In Progress  
6: **Branch:** AIOPT3  
7: **Implementation commit:** —  
8: **Related:** SPEC-038-public-repository-hygiene.md; SPEC-044-core-run-evaluate-invalid-pg-downcast.md; SPEC-045-core-evaluate-functional-harness.md; SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-047-pgcs-readiness-periodic-activation.md; SPEC-048-pgcs-receiver-processing-evidence.md; SPEC-049-nrncs-readiness-binding-oracles.md; SPEC-050-contentapp-repository-control-plane-readback.md; SPEC-051-cross-vm-nrncs-discovery-diagnosis.md; SPEC-055-ng-elc-local-mode.md; SPEC-056-ng-elc-evidence-provenance-hardening.md; SPEC-057-ng-elc-canonical-entrypoint-rename.md
9: 
10: **Canonical experiment tool:** `NG Experiment Lifecycle Controller (NG-ELC)` — current implementation `Scripts/AlpineVMs/ng_remote_executor.py`; canonical rename proposed by SPEC-057: `Scripts/AlpineVMs/ng_elc.py`
11: 
12: **Branch history:** `AIOPT1` and `AIOPT2` are deleted from the remote. Existing mentions are historical; this clone still has stale local/tracking refs for `AIOPT1`, while active release work remains exclusively on `AIOPT3`.
```

### AIOPT4 committed AIOPT3-origin branch-scope example

Snapshot: `AIOPT4` HEAD `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6`; Git-object path `Specs/SPEC-054-release-master-plan-and-audit.md`; bytes `10992`; SHA-256 `bf2a85323a7994eb7bac6395881b6e7b4ed3d01c3d4382660f147881f1b3b7fe`.

Lines 1–12:

```text
1: # SPEC-054: Plano mestre e auditoria determinística da Release v1.0.0
2: 
3: **Author:** Antonio Alberti / Hermes Agent  
4: **Date:** 2026-09-13  
5: **Status:** In Progress  
6: **Branch:** AIOPT3  
7: **Implementation commit:** —  
8: **Related:** SPEC-038-public-repository-hygiene.md; SPEC-044-core-run-evaluate-invalid-pg-downcast.md; SPEC-045-core-evaluate-functional-harness.md; SPEC-046-multi-vm-runner-safe-teardown.md; SPEC-047-pgcs-readiness-periodic-activation.md; SPEC-048-pgcs-receiver-processing-evidence.md; SPEC-049-nrncs-readiness-binding-oracles.md; SPEC-050-contentapp-repository-control-plane-readback.md; SPEC-051-cross-vm-nrncs-discovery-diagnosis.md; SPEC-055-ng-elc-local-mode.md; SPEC-056-ng-elc-evidence-provenance-hardening.md; SPEC-057-ng-elc-canonical-entrypoint-rename.md
9: 
10: **Canonical experiment tool:** `NG Experiment Lifecycle Controller (NG-ELC)` — current implementation `Scripts/AlpineVMs/ng_remote_executor.py`; canonical rename proposed by SPEC-057: `Scripts/AlpineVMs/ng_elc.py`
11: 
12: **Branch history:** `AIOPT1` and `AIOPT2` are deleted from the remote. Existing mentions are historical; this clone still has stale local/tracking refs for `AIOPT1`, while active release work remains exclusively on `AIOPT3`.
```

## AIOPT5 SPEC-054 absence

`Specs/SPEC-054-release-master-plan-and-audit.md` is absent from pinned AIOPT5 tree `8d2386dcb10d2b37f133d4b24c2cb96761753d82` (git cat-file exit 128). This is branch-tree absence only; it does not prove the document never existed historically.

## Evidence boundary

These excerpts supply the original Git-object clauses/source lines missing from the v0.12 Astra packet. The complete branch inventory and conservative attribution rules are in `Specs/IMPACT/BRANCH-APPLICABILITY-REVIEW.md`; per-SPEC mapping is in the attached SPEC-031/044/045/069 sidecars. No implementation, runtime, benchmark, or acceptance claim is established by these excerpts.
