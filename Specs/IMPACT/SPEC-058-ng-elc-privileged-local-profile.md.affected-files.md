# Affected-file record: `SPEC-058-ng-elc-privileged-local-profile.md`

SPEC title: SPEC-058: NG-ELC Native Privileged Local Profile

## Snapshot coverage and applicability

| Snapshot | Pinned HEAD | Present | Declared Branch | Version | Status | Baseline | Implementation commit | Applicability |
|---|---|---|---|---|---|---|---|---|
| `AIOPT3` | `24a3851e1d5a90f039b9e148a1af38dd03c4d925` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | `6a28988`, `c12692a`, `5a5877a`, `da7a859`, `3268365`, `651708e` (profile validation, foreign-inventory gate, descriptor-bound cleanup identity, safe native `clean.sh`, idempotent PTY close, `setsid --ctty` launch/capture, RED→GREEN tests; runtime acceptance remains open) | AIOPT3-origin; historical line; status/gates apply to this snapshot only |
| `AIOPT4` | `0dd0dd4b2fd0a9d416aff232ad07672fe39746b6` | yes | AIOPT3 | (not declared) | In Progress | (not declared) | `6a28988`, `c12692a`, `5a5877a`, `da7a859`, `3268365`, `651708e` (profile validation, foreign-inventory gate, descriptor-bound cleanup identity, safe native `clean.sh`, idempotent PTY close, `setsid --ctty` launch/capture, RED→GREEN tests; runtime acceptance remains open) | Archived snapshot; no current AIOPT4 authority |
| `AIOPT5` | `8d2386dcb10d2b37f133d4b24c2cb96761753d82` | no | — | — | — | — | — | — |

## Files named in explicit file/scope sections

These are literal file/path strings found in file-, scope-, or allowlist-related headings/tables. They represent declared or proposed scope, not proof of a change.

### `AIOPT3` — source `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- pattern: `run_*.sh` — source SPEC line 39.

### `AIOPT4` — source `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- pattern: `run_*.sh` — source SPEC line 39.

## Files changed in linked Git commits

Paths below come from commits that either changed this SPEC file itself or explicitly named its identifier in the subject/body. This is evidence of co-change or a declared commit link, not proof that every path was caused solely by this SPEC.

### `AIOPT3`
- Commit `4f206eb30ea8cfc34a4ac009e553900b88b5e83e` — SPEC-059: add Proxmox QGA execution channel (link basis: the SPEC file changed in this commit)
  - `README.md`
  - `Scripts/AlpineVMs/ng-vm.env.example`
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/proxmox_qga_channel.py`
  - `Scripts/AlpineVMs/tests/test_spec059_qga_channel.py`
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/RESULTS-SPEC-059/qga-channel-check-20260917.json`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
  - `Specs/SPEC-059-ng-elc-qemu-guest-agent-channel.md`
- Commit `4b01e5e258b7228958e027eb1df963048f6629c3` — SPEC-058: Enforce all-owner native teardown (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
- Commit `a94ba63f50c05e3f9fc8be89dc438a7bc42c6e82` — SPEC-058: Enforce protected environment and role EUID (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
- Commit `81b275b72a4b53cf15c85b10fcbf4833379809dc` — SPEC-058: Cover complete IPC teardown (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_executor_contract.py`
  - `Scripts/AlpineVMs/tests/test_spec056_red_gaps.py`
- Commit `3b7f787941733a7debeac9d00cb93fff79464f5b` — SPEC-058: Fix PTY observation and regressions (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
- Commit `4f27cc70f744aae966b56214a3f15cf4717653b5` — SPEC-058: Record descriptor safety commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `651708ec1e120299e7ab22e1455a491e0ee7aa37` — SPEC-058: Enforce descriptor identity and PTY close safety (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
- Commit `6e2b4b2a0e931f09fcd551fe03b624bc66f02deb` — SPEC-058: Record safe cleanup commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `3268365b5eb323e0ff272180da4cb376a68b71bf` — SPEC-058: Make native cleanup non-destructive (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Scripts/Simple/clean.sh`
- Commit `54e28322d015d2395da79114d160a0b160d8fa69` — SPEC-058: Record setsid launcher commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `da7a85940991f656a4933824380ac87e17c16cb0` — SPEC-058: Use setsid for PTY role launch (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
- Commit `8d8c9547a61989fb763eb40cf01b0a06f9b621e8` — SPEC-058: Record foreign inventory hardening commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `5a5877aba2287724a28852ce9128c0644e0822b3` — SPEC-058: Harden foreign inventory and PTY sessions (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `f6de569a03c12ba14ee0e7ebb1e067f49115f87c` — SPEC-058: Record hardening commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `c12692a9e5993b2146105fadab43e036da69f71b` — SPEC-058: Harden privileged cleanup and PTY capture (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `43cc42b6b86dd91c898edf82f9dad54192f811c7` — SPEC-058: Record implementation commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `6a28988647910e3a00cf01d94e99ea138f0c955e` — SPEC-058: Add native privileged local profile (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/plans/local-intra-os.example.json`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/SPEC-055-ng-elc-local-mode.md`
  - `Specs/SPEC-056-ng-elc-evidence-provenance-hardening.md`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`

### `AIOPT4`
- Commit `4f206eb30ea8cfc34a4ac009e553900b88b5e83e` — SPEC-059: add Proxmox QGA execution channel (link basis: the SPEC file changed in this commit)
  - `README.md`
  - `Scripts/AlpineVMs/ng-vm.env.example`
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/proxmox_qga_channel.py`
  - `Scripts/AlpineVMs/tests/test_spec059_qga_channel.py`
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/RESULTS-SPEC-059/qga-channel-check-20260917.json`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
  - `Specs/SPEC-059-ng-elc-qemu-guest-agent-channel.md`
- Commit `4b01e5e258b7228958e027eb1df963048f6629c3` — SPEC-058: Enforce all-owner native teardown (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
- Commit `a94ba63f50c05e3f9fc8be89dc438a7bc42c6e82` — SPEC-058: Enforce protected environment and role EUID (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
- Commit `81b275b72a4b53cf15c85b10fcbf4833379809dc` — SPEC-058: Cover complete IPC teardown (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_executor_contract.py`
  - `Scripts/AlpineVMs/tests/test_spec056_red_gaps.py`
- Commit `3b7f787941733a7debeac9d00cb93fff79464f5b` — SPEC-058: Fix PTY observation and regressions (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
- Commit `4f27cc70f744aae966b56214a3f15cf4717653b5` — SPEC-058: Record descriptor safety commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `651708ec1e120299e7ab22e1455a491e0ee7aa37` — SPEC-058: Enforce descriptor identity and PTY close safety (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
- Commit `6e2b4b2a0e931f09fcd551fe03b624bc66f02deb` — SPEC-058: Record safe cleanup commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `3268365b5eb323e0ff272180da4cb376a68b71bf` — SPEC-058: Make native cleanup non-destructive (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Scripts/Simple/clean.sh`
- Commit `54e28322d015d2395da79114d160a0b160d8fa69` — SPEC-058: Record setsid launcher commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `da7a85940991f656a4933824380ac87e17c16cb0` — SPEC-058: Use setsid for PTY role launch (link basis: commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
- Commit `8d8c9547a61989fb763eb40cf01b0a06f9b621e8` — SPEC-058: Record foreign inventory hardening commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `5a5877aba2287724a28852ce9128c0644e0822b3` — SPEC-058: Harden foreign inventory and PTY sessions (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `f6de569a03c12ba14ee0e7ebb1e067f49115f87c` — SPEC-058: Record hardening commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `c12692a9e5993b2146105fadab43e036da69f71b` — SPEC-058: Harden privileged cleanup and PTY capture (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `43cc42b6b86dd91c898edf82f9dad54192f811c7` — SPEC-058: Record implementation commit (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`
- Commit `6a28988647910e3a00cf01d94e99ea138f0c955e` — SPEC-058: Add native privileged local profile (link basis: the SPEC file changed in this commit; commit subject/body names this SPEC identifier)
  - `Scripts/AlpineVMs/ng_remote_executor.py`
  - `Scripts/AlpineVMs/plans/local-intra-os.example.json`
  - `Scripts/AlpineVMs/tests/test_spec058_red_profile.py`
  - `Specs/RELEASE-MASTER-PLAN-v1.0.0.md`
  - `Specs/SPEC-055-ng-elc-local-mode.md`
  - `Specs/SPEC-056-ng-elc-evidence-provenance-hardening.md`
  - `Specs/SPEC-058-ng-elc-privileged-local-profile.md`

## Interpretation and limits

- A declared path, a path present in the tree, a source citation, and a path changed in a SPEC-linked commit are different evidence types; this record keeps declared scope separate from observed commit diffs.
- `git ls-tree`/`git show` at the pinned HEAD can establish presence and exact contents, but not that a SPEC caused a change or that its acceptance criteria passed.
- Commit-message linkage is incomplete by nature. Absence of a linked commit does not prove that no files were affected; ambiguous duplicate SPEC IDs are left unattributed.
- Branch-specific absence means the SPEC file is not in that pinned branch tree; it does not imply the branch never contained it historically.

Generated from pinned Git objects; do not treat this sidecar as implementation authorization or acceptance evidence.
