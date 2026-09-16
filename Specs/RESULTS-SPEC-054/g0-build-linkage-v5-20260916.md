# SPEC-054 G0 Build-Linkage Receipt v5

**Date:** 2026-09-16
**Branch:** `AIOPT3`
**Candidate HEAD:** `4b01e5e258b7228958e027eb1df963048f6629c3`
**Repository state:** clean; no untracked files
**Acceptance status:** evidence receipt only; G0 remains blocked pending freeze reconciliation and release review

## Source and controller identity

- Deterministic `git archive` SHA-256 for HEAD: `fe0f96b9a922e4bced86b5107e940fc70515490ea7ca16f025fd9e62291c4d97`
- `Scripts/AlpineVMs/ng_remote_executor.py` SHA-256: `dd1bf82d110c9c6e7e2432d78a113ddf664e7c3c440d59497fa408984e1b6fb5`
- `Scripts/Simple/clean.sh` SHA-256: `70e37f22ee2836ab2cb253005fd663109aafc06137d4334b73e7ef11a5246805`

The repository was clean at capture time, so tracked source identity is represented by the candidate commit and archive hash. This receipt does not claim that a privileged runtime trial has been executed.

## Build execution

Commands executed on Linux/VM 100:

```text
cmake --build cmake-build-debug -j2
cmake --build cmake-build-sanitizer -j2
```

Both commands exited successfully. The build emitted existing compiler warnings in PGCS; no compilation or link error occurred.

Toolchain:

- CMake `3.28.3`
- C++ compiler `/usr/bin/c++`
- GCC C++ `13.3.0 (Ubuntu 13.3.0-6ubuntu2~24.04.1)`

## Linked executable hashes

| Configuration | Executable | SHA-256 |
|---|---|---|
| Debug | `cmake-build-debug/PGCS` | `ec3e9bdd309973d31340faabe001802c977fdca12c8d7f95ffe7eac0ef4e9a69` |
| Debug | `cmake-build-debug/NRNCS` | `ad9ec118bd8374b9d06fc327c4f5fa850949cd93339827e84a5e529ea2b2579c` |
| Debug | `cmake-build-debug/ContentApp` | `910a3c038f72d539b013c01d9dcda319e346f91b3c6fc9d9e28b527eaf879cc6` |
| Sanitizer | `cmake-build-sanitizer/PGCS` | `96d9ae8ca0c4a95e3f5a94b9360556b5e752638b7f924625caeb9917ec99c094` |
| Sanitizer | `cmake-build-sanitizer/NRNCS` | `76621ae691f030ff6e0eac601f059f5f2d4be40b38c56910367b4cd08326eb08` |
| Sanitizer | `cmake-build-sanitizer/ContentApp` | `966296fb35c33b24a3b31af7a2e767f6ebc69149f9abe100a87e4073ed2ce9bc` |

## Verification boundary

Verified here:

- candidate commit is present locally and on `origin/AIOPT3`;
- repository is clean;
- Debug build completes;
- Sanitizer build completes;
- executable hashes are recorded;
- controller and repository-owned cleanup script identities are recorded.

Not verified by this receipt:

- effective UID 0 native trial;
- five-photo end-to-end runtime result;
- post-trial process/IPC absence;
- sealed runtime evidence bundle;
- Astra approval;
- G0/M1/M2/M3 release acceptance.
