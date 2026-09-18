# SPEC-059 preflight/build evidence — candidate 3eb46c7

Date: 2026-09-18
Branch: AIOPT3
Source candidate: `3eb46c78ff89604687147c2be23244c3140396ff`

## Scope

This bundle records build linkage, QEMU Guest Agent capability and native-profile preflight only. No cleanup command was called and no NovaGenesis role was launched.

## Source identity

- `git archive HEAD` SHA-256: `bdfb2456b24758678a98d40096fbd46531456a550086c7a1bc21072b2ec50b58`
- `Scripts/AlpineVMs/ng_remote_executor.py` SHA-256: `90252d0028666405dd5d6122d910bc0c24ca71025cc7a4e45b4ae08e7de18e21`
- `Scripts/Simple/clean.sh` SHA-256: `70e37f22ee2836ab2cb253005fd663109aafc06137d4334b73e7ef11a5246805`
- Worktree was clean when the candidate was committed.

## Builds

The canonical normal build used `Scripts/AlpineVMs/ng_observability.py build` with profile `obs-normal.json`, CMake Debug, two jobs and `NG_ENABLE_LEGACY_STANDALONE=OFF`. It returned `0` and produced `build/debug/build-manifest.json`.

Independent isolated builds also returned `0`:

- Debug: `/tmp/ng-build-3eb46c7-debug`
- Sanitizer: `/tmp/ng-build-3eb46c7-sanitizer`, `NG_ENABLE_SANITIZERS=ON`
- CMake `3.28.3`; compiler `/usr/bin/c++`, GCC `13.3.0`

Debug binary SHA-256:

- `PGCS`: `dc90e3ffb6519dbaf3e7c661517385507cec3136de9f72bfb519ec0eee03f83d`
- `NRNCS`: `ea75e521ca3e8a8ac9f3ef79e82ee3564996b08355f1295fa7c864ab7b2180b7`
- `ContentApp`: `07e05f71bd0f2aaaf3d7b8384bb5414b2b09c582c17bdb76c86227b6b07eb88a`

Sanitizer binary SHA-256:

- `PGCS`: `433f2018d73f3a8cad13f110e2a36c12d5e69e3b3e4169658df77a7cfcc96dea`
- `NRNCS`: `ba46a5e9bd89bf592ff616b917825ce10f17206def676b0866a8a0b13835c49d`
- `ContentApp`: `100203847ea24edab1821ffd68b6fd3939ea5a6ddb18b6f24224e23fa77ab19d`

The normal binaries are statically linked; sanitizer binaries are dynamically linked PIE executables. This is build/linkage evidence, not sanitizer runtime evidence.

## QGA capability and preflight

- `qga-channel.json`: `channel_ready=true`, VM `100`, guest UID `0`, transport exit `0`, guest exit `0`.
- Direct local preflight was intentionally attempted and failed closed with `native-privileged local profile requires effective UID 0`; see `preflight/nonroot-direct.json`.
- The identical preflight executed inside VM 100 via `proxmox-qga` as UID 0 and returned `ok=true`, `exit_code=0`, `local_profile=native-privileged`, contract `LOCAL`; see `qga-preflight.json`.
- Explicit paths used: repository `/home/gandalf/workspace/novagenesis`, build `/tmp/ng-observability-3eb46c7-normal`, IO `/tmp/ng-io-preflight-3eb46c7`, evidence path in this bundle.

## Limitations and next gate

This bundle does not prove a runtime trial, cleanup, payload delivery, sealed acceptance bundle, zero post-trial IPC, or release acceptance. The next review decision is whether the exact build/preflight packet permits one bounded diagnostic native trial through QGA. M0 remains blocked and M1 remains open until the release plan is reconciled and Astra reviews this packet.

`artifact-manifest.json` hashes all retained files in this bundle except the manifest itself.
