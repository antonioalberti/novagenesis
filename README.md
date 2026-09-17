# NovaGenesis

NovaGenesis (NG) is a convergent information-processing, storage and exchange architecture developed at INATEL - Instituto Nacional de Telecomunicações, Santa Rita do Sapucaí, Minas Gerais, Brazil.

The project currently continues at the University of Leeds under the leadership of [Prof. Antônio Marcos Alberti](https://eps.leeds.ac.uk/computing/staff/15735/dr-antonio-alberti). Its foundations were selected in 2008, first specified in 2011, and prototyped in C/C++ during 2012–2013.

NovaGenesis is an event-driven Linux prototype in which services organize themselves through names, name bindings and contracts. Communication uses the NovaGenesis message model over raw Ethernet sockets. This prototype does not support other operating systems and does not provide a complete security layer. The software is provided as-is under its applicable open-source licences.

Current project branch

- Principal/default branch: `AIOPT3`
- `master`: preserved as a secondary historical/compatibility branch
- Current development path: NRNCS-based normal runtime
- Standalone PSS, GIRS and HTS: deprecated; available only through an explicit legacy profile

Research references

- NovaGenesis design paper: https://www.sciencedirect.com/science/article/abs/pii/S0167739X16302643
- Publications: https://www.researchgate.net/profile/Antonio-Alberti-2

# 1. Architecture overview

NovaGenesis services exchange self-certifying names, bindings and messages. The main components are:

- `Common` — shared message, command-line, process, queue, gateway and naming infrastructure.
- `PGCS` — proxy/gateway/controller service responsible for local IPC, bootstrapping, discovery and raw Ethernet forwarding.
- `NRNCS` — normal domain service integrating the former PSS, GIRS and HTS functions. It stores domain bindings and cached payload files.
- `ContentApp` — named-content publisher/subscriber with Source and Repository instances.
- `NBTestApp` — name-binding publication and subscription workload for the NRNCS path.
- `IoTTestApp` — contract-based client for externally maintained IoT/I4.0 devices.

The normal content path is:

```text
ContentApp Source
    -> local PGCS
        -> raw Ethernet PGCS-to-PGCS transport
            -> PGCS on the peer VM
                -> NRNCS
                    -> Repository ContentApp
```

NRNCS follows the inverted pub/sub model: published payloads are cached locally first; later subscription requests retrieve the cached content through the binding service. `NRInfoPayload01` must not forward payloads prematurely.

# 2. Repository layout

| Directory | Purpose |
|---|---|
| `Common/` | Shared C++ library and NovaGenesis message infrastructure |
| `PGCS/` | Proxy/Gateway/Controller service |
| `NRNCS/` | Normal Name Resolution and Network Cache Service |
| `ContentApp/` | Named-content Source and Repository application |
| `NBTestApp/` | NRNCS name-binding workload |
| `IoTTestApp/` | External IoT/I4.0 contract client |
| `PSS/`, `GIRS/`, `HTS/` | Deprecated standalone implementations, retained for legacy builds |
| `Docker/` | Docker build contexts and service images |
| `Scripts/AlpineVMs/` | Alpine VM build and process launch helpers |
| `Scripts/Docker/` | Docker scenarios and log/stop helpers |
| `Scripts/Simple/` | Local host scenarios, cleanup and validation helpers |
| `IO/` | Runtime configuration and generated output; test evidence is kept outside Git |
| `Docs/` | Architecture, decisions, diagnostics and historical records |
| `Specs/` | SPEC-driven design and acceptance documents |
| `Issues/` | Open and closed issue records |
| `Plots/` | Plotting helpers for generated service statistics |
| `Make/` | Alternative direct compilation scripts |

# 3. Requirements

NovaGenesis currently targets Linux. For compilation and normal execution, install:

- CMake;
- GCC/G++ with C++20 support;
- POSIX shared memory and semaphore support;
- raw Ethernet socket capability for inter-VM PGCS tests;
- Python 3 for photo-generation helpers;
- Docker, if using the Docker scenarios;
- SSH and the configured key, if using the Alpine VM scenarios.

The Alpine guests use Alpine Linux with musl libc and GCC 15. The supported VM roles are documented in Section 7. The concrete VM numbers, hostnames, addresses and hardware addresses are deployment-specific and must be replaced in local launchers.

# 4. Build profiles

## 4.1 Normal profile

The normal profile is the default. It builds the recommended NRNCS path and does not create standalone PSS, GIRS or HTS executables.

From the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DNG_ENABLE_LEGACY_STANDALONE=OFF
cmake --build build -j"$(nproc)"
```

Expected normal executables include:

```text
build/PGCS
build/NRNCS
build/ContentApp
build/NBTestApp
build/IoTTestApp
```

`build/PSS`, `build/GIRS` and `build/HTS` must not be produced by the normal profile.

## 4.2 Explicit legacy build

The deprecated standalone services can be compiled only by opting in explicitly:

```bash
cmake -S . -B build-legacy -DCMAKE_BUILD_TYPE=Debug \
  -DNG_ENABLE_LEGACY_STANDALONE=ON
cmake --build build-legacy -j"$(nproc)"
```

The legacy build emits a deprecation warning. Legacy runtime selection is also explicit:

```bash
export NG_RUNTIME_PROFILE=legacy
```

Do not use the legacy profile as evidence for normal NRNCS operation. Functional legacy runtime tests are compatibility checks performed on demand.

## 4.3 Direct compilation scripts

`Make/compile.sh` and related scripts remain available for environments where the direct GCC build is useful. For Alpine VM deployments, prefer the profile-aware workflow in `Scripts/AlpineVMs/pull-and-build-vms.sh` and verify the resulting executables independently.

# 5. Runtime cleanup and local scenarios

Before a new local or VM test, stop stale NovaGenesis processes and remove stale IPC resources:

```bash
bash Scripts/Simple/clean.sh
```

Do not delete configuration files when only payload/cache data must be removed. Preserve test evidence outside the active runtime directories.

The local scripts include:

- `Scripts/Simple/run_PGCS.sh`
- `Scripts/Simple/run_NRNCS.sh`
- `Scripts/Simple/run_Repository.sh`
- `Scripts/Simple/run_Source.sh`
- `Scripts/Simple/run_IoTTestApp.sh`
- `Scripts/Simple/stop_test.sh`

Inspect a launcher before using it. Confirm its executable path, configuration path and runtime profile; stale binaries and incorrect IO paths can produce misleading results.

# 6. Docker scenarios

Docker scenarios are available under `Scripts/Docker/`. The main scenario families are:

- `run-multiple-content-distribution-applications.sh` — multiple Source and Repository ContentApp instances;
- `run-name-binding-resolution-application.sh` — NBTestApp/NRNCS name-binding workload;
- `run-PGCS-NRNCS.sh` and related helpers — core service scenarios;
- `run-PGCS-ContentApp-Source.sh` and `run-PGCS-ContentApp-Repository.sh` — content distribution components;
- `run-PGCS-NBTestApp.sh` — binding workload;
- `stop-*` and `log-*` scripts — teardown and evidence collection.

Build or update images with the repository-level scripts only after inspecting their current arguments:

```bash
bash make-all-docker-images.sh
```

The Docker path uses shared memory for intra-container IPC and raw Ethernet framing for PGCS communication. The NG wire protocol uses EtherType `0x1234`; HTTP, TCP and UDP are not substitutes for the NG data path.

# 7. Alpine VM cross-process scenarios

The cross-process scenario uses two Alpine Linux guests and a separate control host. Use deployment-local names and addresses; the table below intentionally contains no site-specific network identity:

| Role | Hostname | Address | Ethernet MAC |
|---|---|---|---|---|
| Repository guest | `<repository-host>` | `<repository-ip>` | `<repository-mac>` |
| Source guest | `<source-host>` | `<source-ip>` | `<source-mac>` |

Each PGCS must receive the other guest's actual Ethernet MAC as its peer argument. Preserve the literal representation through SSH and launcher layers, including lowercase hexadecimal letters where the local parser requires them. Before testing, verify the actual values with `ip link show <interface>` or the equivalent platform tool. Never copy the example placeholders into a real deployment.

## 7.1 Build and deploy

From the control host, prepare a local ignored configuration from `Scripts/AlpineVMs/ng-vm.env.example`, fill in the two guest addresses/MACs, guest repository/build paths, SSH key/user/known-hosts and a durable local evidence path, then source it:

```bash
cp Scripts/AlpineVMs/ng-vm.env.example Scripts/AlpineVMs/ng-vm.env
$EDITOR Scripts/AlpineVMs/ng-vm.env
. Scripts/AlpineVMs/ng-vm.env
```

The profile-aware deployment helper is:

```bash
NG_BUILD_PROFILE=normal bash Scripts/AlpineVMs/pull-and-build-vms.sh
```

Use `NG_BUILD_PROFILE=legacy` only when explicitly testing the deprecated standalone profile.

### 7.1.1 Privileged local execution through Proxmox

When Hermes operates from VM 100 without an interactive Linux terminal, do not use `sudo -v` or request a password in chat. On a Proxmox deployment with QEMU Guest Agent enabled, verify the privileged channel from VM 100:

```bash
python3 Scripts/AlpineVMs/ng_remote_executor.py channel-check \
  --channel proxmox-qga \
  --host "$NG_PROXMOX_HOST" \
  --vmid "$NG_PROXMOX_VM_ID" \
  --ssh-user "$NG_PROXMOX_SSH_USER" \
  --ssh-key "$NG_PROXMOX_SSH_KEY" \
  --known-hosts "$NG_PROXMOX_KNOWN_HOSTS"
```

The command must report `channel_ready: true` and `uid: 0`. To invoke the canonical NG-ELC as root inside the guest, use the QGA adapter; do not replace the controller with a manual launcher:

```bash
python3 Scripts/AlpineVMs/proxmox_qga_channel.py \
  --host "$NG_PROXMOX_HOST" \
  --vmid "$NG_PROXMOX_VM_ID" \
  --ssh-user "$NG_PROXMOX_SSH_USER" \
  --ssh-key "$NG_PROXMOX_SSH_KEY" \
  --known-hosts "$NG_PROXMOX_KNOWN_HOSTS" \
  exec /usr/bin/python3 "$NG_REPO_PATH/Scripts/AlpineVMs/ng_remote_executor.py" \
  preflight --plan "$NG_REPO_PATH/Scripts/AlpineVMs/plans/local-intra-os.example.json" \
  --scenario local-intra-os --mode local --local-profile native-privileged
```

The example paths above are guest paths and must be replaced by the frozen candidate paths in the local configuration. `preflight` is non-destructive; it must pass before any explicit authorization to invoke `Scripts/Simple/clean.sh`. Record the backend, Proxmox host, VM ID, UID proof, guest exit code, stdout/stderr and evidence path in the trial bundle. A transport success is not a guest success: use the `exitcode` returned by `qm guest exec`.

This channel is a Proxmox-specific optional adapter. The NG-ELC core and remote SSH mode must remain usable on other hypervisors; if no supported channel proves UID 0, stop with `BLOCKED` rather than falling back to interactive sudo, passwords or unprivileged acceptance.

The local configuration template includes the `NG_LOCAL_*` paths and optional `NG_PROXMOX_*` channel settings. Use a fresh IO directory and a durable evidence directory inside the repository for each acceptance candidate.

The guests must independently verify:

- the expected Git commit;
- healthy Git objects;
- executable format and nonzero size;
- executable SHA-256 values;
- the NRNCS cache marker (`strings NRNCS | grep 'cached payload'`).

## 7.2 Normal launch order

Before every run:

1. Stop and start both VMs cleanly.
2. Verify that the previous trial has no identity-attributed processes or IPC resources; the five launchers do not invoke `Scripts/Simple/clean.sh`.
3. Verify exactly one intended process per role.
4. Start PGCS on both VMs with the peer MACs above.
5. Wait for bidirectional PGCS peer registration.
6. Start NRNCS on the Source guest and wait for operational/shared-memory discovery markers.
7. Start Repository ContentApp on the Repository guest.
8. Generate fresh unique JPEGs and start Source ContentApp on the Source guest.

The corresponding helpers are:

```text
Scripts/AlpineVMs/run_PGCS_on_Source_VM.sh
Scripts/AlpineVMs/run_PGCS_on_Repo_VM.sh
Scripts/AlpineVMs/run_NRNCS_on_Source_VM.sh
Scripts/AlpineVMs/run_Repository_on_Repo_VM.sh
Scripts/AlpineVMs/run_Source_on_Source_VM.sh
```

## 7.3 Acceptance evidence

A photo-transfer run is accepted only when:

- Source, NRNCS and Repository contain the expected number of fresh JPEGs;
- filenames match at all three locations;
- programmatic SHA-256 maps satisfy `Source == NRNCS == Repository`;
- there are no missing or extra files;
- logs contain no unexplained `ERROR`, `ALARM` or `FATAL` entries;
- the tested commit and configuration are recorded;
- logs, manifests and hash comparisons are preserved in a durable evidence directory.

For a 100-photo gate, exactly 100 fresh JPEGs must be present and byte-exact at all three locations. File counts or textual delivery messages alone are not sufficient evidence.

For asymmetric discovery, inspect the actual frames on the hypervisor or host bridge before changing NovaGenesis code. Substitute the interface that carries traffic for `<bridge-interface>`:

```bash
tcpdump -eni <bridge-interface> ether proto 0x1234
```

First verify that each transmitted Ethernet destination matches the peer VM MAC. A wrong destination is a test/deployment configuration error, not evidence of a NovaGenesis runtime defect.

After evidence collection, stop all NG processes and leave both test guests stopped unless another test is actively running.

## 7.4 Supervised executor and observability

The manual `run_*.sh` helpers above remain available for interactive diagnosis only. They verify build provenance and propagate child exit status, but do not provide the supervised teardown contract of SPEC-046/G3. For reproducible acceptance trials use the additive tooling under `Scripts/AlpineVMs/`:

```bash
cp Scripts/AlpineVMs/ng-vm.env.example Scripts/AlpineVMs/ng-vm.env
$EDITOR Scripts/AlpineVMs/ng-vm.env   # local values only; never commit this file
. Scripts/AlpineVMs/ng-vm.env

# Inventory the actual source tree and create inventory.json, inventory.md and coverage.json.
python3 Scripts/AlpineVMs/ng_observability.py \\
  inventory --source . --output "$NG_EVIDENCE_PATH/observability-inventory"

# Validate a profile against that inventory.
python3 Scripts/AlpineVMs/ng_observability.py \\
  validate --inventory "$NG_EVIDENCE_PATH/observability-inventory/inventory.json" \\
  --profile Scripts/AlpineVMs/observability/profiles/obs-hello.json

# Configure an isolated normal or targeted-debug build.
python3 Scripts/AlpineVMs/ng_observability.py build \\
  --source . --profile Scripts/AlpineVMs/observability/profiles/obs-normal.json \\
  --variant normal --output "$NG_EVIDENCE_PATH/build-normal" --jobs 4

# Validate the plan locally; this command does not open SSH.
python3 Scripts/AlpineVMs/ng_remote_executor.py dry-run \\
  --plan Scripts/AlpineVMs/plans/L2-pgcs-only.example.json \\
  --scenario L2 --debug-profile obs-normal

# Run read-only guest preflight, then use the same plan only after reviewing it.
python3 Scripts/AlpineVMs/ng_remote_executor.py preflight \\
  --plan Scripts/AlpineVMs/plans/L2-pgcs-only.example.json \\
  --scenario L2 --debug-profile obs-normal
python3 Scripts/AlpineVMs/ng_remote_executor.py run \\
  --plan Scripts/AlpineVMs/plans/L2-pgcs-only.example.json \\
  --scenario L2 --debug-profile obs-normal
```

The staged scenarios are `L0` (preflight), `L1` (local PGCS/SHM), `L2` (two-VM PGCS and receiver), `L3` (NRNCS readiness/binding), `L4` (Repository control plane) and `L5` (100-photo path). Each level has its own gates and does not promote a lower-level result into a higher-level claim.

`obs-normal` is the acceptance baseline. Targeted profiles such as `obs-hello`, `obs-raw-sar`, `obs-nrncs-binding`, `obs-core-control` and `obs-payload-cache` are diagnostic variants and produce separate manifests. Broad `PGCS/src/PG.cpp` debug, `Common/src/GW.cpp:DEBUG`, `DEBUG2`, `DEBUG3`, `DEBUG5`, `DEBUG6` and `DEBUG_NETWORK_QUEUE` are prohibited by default because they can alter timing or flood Alpine `/tmp`.

The executor uses short non-PTY SSH calls and a per-trial remote helper. It records remote PID/PGID/start time, executable, command, commit, configuration and hashes; it distinguishes runtime, teardown and evidence results. Use `status`, `collect` and `cleanup` for recovery. Do not run manual `run_*.sh` helpers concurrently with a supervised trial, do not use global `killall`/`ipcrm -a`, and do not stop a VM before evidence and process/IPC verification.

The helper requires Python 3 and persistent guest evidence storage. Logs from the guest are kept outside `/tmp`; raw logs, captures and manifests may contain private deployment or payload data and must remain outside Git. A missing or buffered marker is reported as `INCONCLUSIVE`, not silently promoted to `FAIL` or `PASS`.

# 8. Documentation and reproducibility

The repository contains the durable technical record of the project:

- `Docs/ARCHITECTURE/` — architecture and the inverted pub/sub model;
- `Docs/DECISIONS/` — implementation decisions, baselines, reviews and acceptance packets;
- `Docs/DIAGNOSTICS/` — root-cause analyses and runtime diagnostics;
- `Docs/HISTORICAL/` — historical attempts and evidence that must not be treated as the current baseline;
- `Specs/` — SPEC-driven design, implementation scope and acceptance criteria;
- `Issues/` — open and closed issue records;
- `Scripts/AlpineVMs/` — reproducible multi-VM build and launch procedures;
- `Scripts/Simple/` and `Scripts/Docker/` — local and containerized scenarios.

Runtime evidence from the control host and Alpine guests is kept in external evidence directories and linked from the relevant task, SPEC or decision record. Evidence should include the tested commit, configuration, process logs, manifests and programmatic hash comparisons. Local VM paths, credentials, private addresses and temporary runtime state are deliberately not embedded in this public README.

The current operational baseline is the `AIOPT3` branch. The remote default branch is also `AIOPT3`; `master` is preserved as a secondary historical/compatibility branch.

# 9. Troubleshooting and limitations

- If PGCS discovery is asymmetric, verify literal MAC arguments and capture EtherType `0x1234` on the bridge before inspecting application code.
- If NRNCS is unknown, confirm PGCS and NRNCS readiness markers before starting ContentApp.
- If files arrive with mismatching hashes, inspect cache state and filename reuse before attributing the issue to transport.
- Always compare against a publish-time manifest; do not compare a received file against a mutable live Source directory.
- `NG_RUNTIME_PROFILE=legacy` is not the normal deployment path.
- The prototype is Linux-only and does not provide a complete security layer.
- Historical Docker, local and standalone-service instructions may remain in `Docs/HISTORICAL`; they do not override the normal NRNCS policy documented here.

# 10. Licence and historical material

Read the licence files included in the repository and in the `Common` library before redistribution or use. Historical source, scripts and documents are retained for reproducibility; active normal deployment follows the NRNCS profile and the current AIOPT3 branch.
