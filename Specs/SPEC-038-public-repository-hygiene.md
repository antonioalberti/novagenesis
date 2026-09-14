# SPEC-038: Public Repository Security, Privacy and Reproducibility Hygiene

**Version:** v1.0
**Date:** 2026-09-11
**Author:** Antonio Alberti + Hermes Agent
**Status:** In Progress
**Branch:** AIOPT3
**Implementation commit:** `7b430a3`, `4f83b38`
**Related:** SPEC-036-deprecate-standalone-pss-girs-hts.md; SPEC-037-promote-aiopt3-default-branch.md

## 1. Objective

Audit and clean the public NovaGenesis repository so that the active default branch contains no live credentials, unnecessary private deployment identities, misleading local paths, obsolete generated artefacts or inappropriate public operational instructions, while preserving legitimate attribution, licences, scientific history and reproducibility.

`master` remains an old secondary branch as requested, but retaining it does not remove sensitive values from public Git history. Any credential found in history is treated as compromised; it must be rotated/revoked independently unless the governing release profile records a bounded fixture-only exception. History rewriting is a separate decision and must not be assumed to follow from this SPEC.

## 2. Initial Audit Findings

### Critical

- A literal default root password appears in public Alpine setup/diagnostic scripts, including `alpine-install-repo.sh`, `alpine-install-source.sh`, `alpine-phase2-packages.sh` and `check-vm-state.sh`.
- Password authentication is enabled by setup scripts. This must be removed or made explicitly local-only; normal deployment must use SSH keys and an operator-provided credential flow.
- The password must be treated as compromised and must not be reused.

### High privacy/reproducibility concern

- Initial audit found private deployment identities, local filesystem references and generated runtime material in active scripts and historical records.
- These values describe one laboratory deployment and cannot be assumed by another user.
- Active operational scripts should use required environment variables or a local ignored configuration file. Historical records may retain exact values only when clearly labelled as historical and intentionally excluded from active instructions.

### Repository hygiene

- Tracked compiled binaries exist under `cmake-build-debug/` (`PGCS`, `NRNCS`, `ContentApp`).
- Temporary/debug artefacts exist under `Temp/`, including test binaries and one-off scripts.
- Backup/source artefacts such as `PGCS/src/PG.cpp.bak` and generated `Make/out.txt` are tracked.
- Runtime/configuration outputs under `IO/` require classification: immutable configuration may remain, generated output and experiment state should not be tracked.

### Legitimate content

- Author names, copyright notices, institutional affiliations, paper links and the public University of Leeds profile are legitimate attribution and must not be removed merely because they identify the author.
- No private-key, GitHub-token or common cloud-API-key pattern was found in the initial current-tree scan.
- No obviously unethical content was identified in the initial scan; the ethical review gate remains a classification check, not a claim that every historical statement is current or endorsed.

## 3. Scope

### Included

- Current-tree secret and credential removal.
- Credential rotation/revocation checklist and explicit warning that public history may contain the old value.
- Parameterisation of active Alpine deployment scripts.
- Replacement of private paths, VM identities and network literals in active documentation with roles/placeholders.
- Classification of SPECs and Docs as active, historical, or deployment-specific.
- Removal or Git-ignore treatment of tracked build/temp/generated artefacts when they are not required source assets.
- Public README and active documentation review for privacy, security, misleading claims and reproducibility.
- Evidence packet containing the audit patterns, dispositions and post-cleanup scans.

### Excluded

- Rewriting `master` or deleting it.
- Automatic history rewrite or force-push.
- Removing legitimate author attribution, licences, citations or historical evidence solely because it is old.
- Removing source code required for the normal or explicit legacy build without a separate deprecation decision.
- Publishing any private VM credentials, local paths or untracked runtime evidence.

## 4. Proposed Policy

1. Never commit passwords, private keys, access tokens or reusable credentials.
2. Active scripts must obtain deployment identities from explicit environment variables or an ignored local configuration file and fail closed when required values are absent.
3. Public examples use placeholders such as `<repository-ip>`, `<source-ip>`, `<repository-mac>` and `<ssh-key>`.
4. Historical documents are retained only when labelled historical and when retention serves reproducibility; active README/instructions must not depend on them.
5. Generated binaries, temporary test executables and runtime outputs are not tracked unless they are deliberately distributed source assets with a documented reason.
6. A current-tree scan is not a history scan. Credential rotation is mandatory for production or shared-purpose systems even if a value is removed from AIOPT3; a dedicated test-fixture exception requires explicit scope, containment and a no-reuse condition.
7. Security/privacy findings are classified as: critical secret, private deployment identity, obsolete artefact, legitimate attribution, historical evidence, or acceptable public documentation.

## 5. Acceptance Criteria

- [x] The literal Alpine password is removed from all active files and no replacement credential is embedded.
- [x] SSH-key/operator-credential setup is documented without exposing a secret.
- [x] The password is recorded as compromised in the task handoff. External rotation is explicitly N/A for the v1.0 profile because the referenced guests are dedicated NovaGenesis test fixtures only; this is not a production-security claim.
- [x] Active deployment scripts no longer require this laboratory's IPs, MACs, hostnames or absolute paths; they accept local configuration.
- [x] Active documentation uses roles/placeholders and does not expose private deployment identities.
- [x] Tracked binaries, temporary artefacts and generated outputs have a documented disposition and are removed or deliberately retained.
- [x] Historical files retained in AIOPT3 are labelled and do not override active instructions for the v1.0 supported profile; the bounded classification is recorded in `Specs/RESULTS-SPEC-038/g1-historical-classification-v4-20260914.md`, while the broader legacy backlog remains v1.1+.
- [x] Current-tree secret scan finds no credentials or private keys.
- [x] Public-data scan finds no unintended personal, infrastructure or local-path exposure in the active v1.0 supported profile; the bounded scan and its exclusions are recorded in `Specs/RESULTS-SPEC-038/g1-hygiene-validation-v4-20260914.md`.
- [ ] Normal runtime and documented v1.0 test procedures remain reproducible after cleanup; the legacy build profile is N/A for v1.0 and remains tracked for v1.1+.
- [x] A final evidence report maps every finding to a disposition.
- [x] No history rewrite or force-push occurs without separate explicit approval.

## 6. Work Packages

- **A — Credential containment:** remove literal password, disable password-based assumptions, document rotation and scan current tree/history.
- **B — Deployment parameterisation:** refactor active Alpine scripts and examples around roles and local configuration.
- **C — Repository hygiene:** classify/remove tracked binaries, temp files, generated outputs and backup files.
- **D — Documentation/privacy review:** classify active versus historical docs and remove unintended local identities.
- **E — Verification:** build/profile checks, secret scan, public-data scan, link checks and evidence report.
- **F — Optional history remediation:** prepare a separate plan for Git history purge only if explicitly approved; do not execute as part of the initial cleanup.

## 6.1 Current validation

G1 validation is recorded in `Specs/RESULTS-SPEC-038/g1-hygiene-validation-20260913.md`, the current-profile scan in `Specs/RESULTS-SPEC-038/g1-hygiene-validation-v4-20260914.md`, the current normal build in `Specs/RESULTS-SPEC-038/g1-build-v4-20260914.md`, and historical classification in `Specs/RESULTS-SPEC-038/g1-historical-classification-v4-20260914.md`. The current-tree and normal CMake build gates passed. On 2026-09-14, the owner classified external rotation as N/A for the v1.0 profile because the referenced guests are dedicated NovaGenesis test fixtures, not production systems and not shared-purpose machines. The historical value remains compromised, must not be published again, and must never be reused outside those disposable test fixtures. The active v1.0 public-data and bounded historical classification scans now pass; legacy build parity is N/A for v1.0 and remains tracked for v1.1+, while normal runtime remains open.

## 6.2 Release-profile decision — 2026-09-14

- The Proxmox guests used by the v1.0 profile are dedicated to NovaGenesis testing and have no production or unrelated workload.
- A well-known fixture credential may be retained in the operator's ignored local configuration when required by the test harness; it must not be committed, copied into active public instructions, or presented as a secure deployment default.
- This decision accepts the bounded test-fixture risk only. It does not waive production hardening: production deployments must use operator-managed credentials, preferably SSH keys, and must not reuse the fixture credential.
- If a guest is repurposed, exposed to an unrelated network, or used for production data, the fixture credential must be replaced before that use.

## 7. Rollback

- Keep cleanup changes in separable `SPEC-038:` commits.
- Revert current-tree cleanup commits if build or deployment behaviour regresses.
- Do not restore the exposed password.
- History remediation, if approved later, must have a full remote backup, contributor coordination and an explicit force-push decision.
