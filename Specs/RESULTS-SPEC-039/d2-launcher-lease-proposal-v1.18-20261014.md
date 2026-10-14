# NG-039 D2 launcher/lease provisioning proposal — v1.18 aligned

**Status:** candidate design for owner/reviewer decision; not normative, installed, or tested.
**Purpose:** satisfy SPEC-039 v1.18 §9.4–9.6 dedicated-identity and continuous-lease prerequisite for one local D2 Q0/Q1 correctness run.
**Boundary:** this proposal does not authorize D2 code edits, configure/build, harness/finalizer execution, IPC creation/removal, Astra submission, VM/SSH, commit, or push.
**Alignment:** every field in this proposal matches SPEC-039 v1.18 §9.6 manifest alignment table exactly.

## 1. Host facts checked read-only

- Local host exposes `systemd 255.4`; `flock`, `setpriv`, `runuser`, `useradd`, and `groupadd` are available. None was used to mutate state.
- The owner's home directory is mode `0750`; a future D2 UID must not be added to the owner's group or granted access to the home tree merely to reach test files.
- `/var/lib` and `/srv` are root-owned `0755`; `/proc/sysvipc` is readable (`0555`) and `/dev/shm` is host-visible (`01777`). Candidate `<WORKSPACE_ROOT>` is `/var/lib/ng039-d2/workspace`, outside the private home and `.hermes`; §7.1 leaves `<WORKSPACE_ROOT>` to be resolved and recorded in the run manifest.
- No matching named `ng039` account, service unit, or launcher was found in the bounded preflight; this is not an exhaustive root-owned filesystem audit. No D2 test files exist in either examined checkout.

## 2. Candidate identity and files — pending approval

| Item | Proposed value | Required invariant |
|---|---|---|
| Dedicated account/group | `ng039d2` | Non-login shell `/usr/sbin/nologin`; locked password; no supplementary groups, SSH key, sudoers/polkit launch grant, or unrelated system/user service; reserved only for this D2 gate. These facts must be independently checked at install and pre-run. |
| Systemd unit | `/etc/systemd/system/ng039-d2.service` | Root-owned, fixed one-shot entry (`User=root`, `Group=root`, `Restart=no`, `KillMode=control-group`). Only root may start/modify it. Candidate hardening: `NoNewPrivileges=yes`, `ProtectSystem=strict`, `ProtectHome=yes`, `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no`, `PrivateUsers=no`, `PrivateNetwork=no`, `ProtectProc=default`, `RestrictAddressFamilies=AF_UNIX`, `CapabilityBoundingSet=CAP_SETUID CAP_SETGID CAP_KILL CAP_DAC_READ_SEARCH`, plus explicit `ReadWritePaths` for the pinned workspace/control paths and host `/dev/shm`. The exact property set must be reviewed and a no-IPC service-side preflight must prove it does not hide or replace `/proc/sysvipc` or the host `/dev/shm` mount. |
| Supervisor | `/usr/local/libexec/ng039-d2-supervisor` | Root-owned, non-writable by `ng039d2`; fixed argument contract; SHA-256 pinned in the run manifest. It holds the root-only lease, launches only the pinned harness and finalizer without a shell, and drops each controlled child to `ng039d2` using an explicit `setpriv --reuid=... --regid=... --clear-groups --no-new-privs -- ...` argv. **The harness never inherits the lease FD.** The finalizer receives a fixed `--lease-fd <n>` handle after the harness exits; it may inspect the pinned FD but must not close/unlock it. The supervisor retains its own lease FD until finalizer exit and final-record verification. The build/check phase is separate and must finish, be pinned, and be made read-only before this one-shot run service starts. The finalizer is sequential, never concurrent with the harness. |
| Workspace root | `/var/lib/ng039-d2/workspace` (`<WORKSPACE_ROOT>`) | Root-managed and not writable by `ng039d2`. Freeze fresh sibling roots beneath it: `source/<run-id>/`, `build/<run-id>/`, `results/<run-id>/`, and `scratch/<run-id>/{home,tmp}`. Source/manifest and the post-build executable become root-owned/read-only; build output is writable only during configure/build; results and the distinct HOME/TMPDIR are writable only by the D2 UID. Verify pairwise non-overlap, symlink-free parents, device/inode and ownership before use. No `.hermes` paths. |
| Control and lease | `/var/lib/ng039-d2/control/<run-id>/`; `/run/lock/ng039-d2/lease.lock` | Control parents are root-managed; `/var/lib/ng039-d2/control` and the per-run control directory are root:`ng039d2` mode `0710`, so the D2 UID can traverse but cannot list or modify them. `supervisor-termination.json` and closed logs are root:`ng039d2` mode `0440`, opened with no-follow/exclusive identity checks and fsynced/hashed by the supervisor before the finalizer receives read-only access. Lease directory/file are root-managed `0700`/`0600`; supervisor checks regular-file, owner/mode, `st_dev`/`st_ino`, opens with `O_NOFOLLOW|O_CLOEXEC`, obtains nonblocking exclusive `flock`, and continuously verifies the same held FD/identity through final-record closure. `ng039d2` cannot open, replace, or remove the lease. |
| IPC visibility | systemd `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no` | No IPC or mount namespace may hide/replace host resources. Before any Nova/IPC run, a no-Nova preflight under the exact unit must compare service/host IPC namespace identity, raw `/proc/sysvipc` readability/hash and `/dev/shm` mount/device/inode identity without creating/removing IPC. Exact `sem_t`-to-object device/inode mapping remains a required check during the separately authorized D2 reservation phase. No network input. |

**Owner trust assumption:** host root/systemd remains trusted, as allowed by §7.1. The fixed root-owned unit/launcher is the only permitted entry to `ng039d2`; account/session/service/polkit audits plus a live UID census and systemd-cgroup membership check must show no unrelated process before pre-inventory and at every boundary. Any unlisted process or process outside the unit cgroup blocks finalization and preserves evidence. The systemd cgroup is the process-tree source of truth; a process group alone is insufficient. If root-level concurrent mutation is in scope, this shared-host profile is unacceptable and D2 remains blocked.

## 3. Proposed run/supervisor protocol (aligned to §9.4–9.5)

1. A separately authorized owner action starts the one root-owned service with a root-owned, one-use authorization record that names the paired D2 experiment: **one fresh Q0 process start followed by one fresh Q1 process start** (`N=1` GW consumer per condition), with **separate run IDs, journals and final records** (cardinality = 1 for all). The supervisor atomically marks the authorization consumed before any D2 action; repeated starts fail closed, and another experiment requires another owner decision. No automatic restart or retry. The root supervisor validates the fixed unit, script/binary hashes, manifest, account/session/service/polkit state, workspace roots, result-directory identity and lease-file identity before any inventory.
2. The supervisor obtains the **root-only exclusive lease before Q0 pre-inventory and retains it continuously across Q0, its finalizer, Q1, its finalizer and final-record closure** (§9.4 `lease.fd_owner = "supervisor_root"`). It checks all processes with the D2 UID against the unit cgroup and expected process ledger before/after each phase; any unrelated or escaped process blocks the experiment.
3. Build/configure/checks are not performed by the run service: they require their own post-review authorization, explicit `env -i` commands under `<WORKSPACE_ROOT>`, and a frozen executable hash. Candidate retained environment is exactly `PATH=/usr/bin:/bin`, `HOME=<WORKSPACE_ROOT>/scratch/<run-id>/home`, `TMPDIR=<WORKSPACE_ROOT>/scratch/<run-id>/tmp`, `LANG=C.UTF-8`, and `LC_ALL=C.UTF-8`; HOME and TMPDIR are distinct. Before the run unit starts, the exact source/manifest and executable are root-owned/read-only. After the no-Nova service visibility check and Q0 pre-inventory, the supervisor starts the pinned harness as a single `ng039d2` child via fixed no-shell `setpriv` argv, with a new session/process group and bounded stdout/stderr FDs opened in the protected control directory. The harness itself creates no child process; the Gateway output worker is a thread.
4. The supervisor records the systemd unit/cgroup identity and, for every process, PID, PPID, PGID, `/proc` start time, real/effective/saved/filesystem UID/GID, executable path/device/inode/hash, cgroup membership, and raw `waitpid` status. It compares the live cgroup process set with the expected ledger (root supervisor plus at most one sequential D2 child) at each boundary; unknown members, escaped children, or incomplete reaping are `BLOCKED/INCONCLUSIVE`.
5. After each harness exits, the supervisor fsyncs/closes/hashes its log FDs and writes that run's root-owned `supervisor-termination.json` in the protected control directory. The same binary's `--finalize` mode runs sequentially as `ng039d2`, **with read-only receipt/log access and no lease FD** (§9.4 `lease.finalizer_has_fd = false`); the root supervisor retains its own lease FD. The finalizer must preserve the handle through final-record closure and must not unlock it. After finalizer exit, the supervisor verifies the lease identity/lock and final-record hash before advancing. Q1 begins only after Q0 finalization and a fresh Q1 pre-inventory under the same lease. Any Q0 failure that leaves inventory, cleanup, identity or lease uncertain stops the sequence; Q1 is recorded `NOT_RUN`, never retried. The supervisor must not invoke/re-enter a finalizer if receipt, log, process-set, identity or lease checks fail.
6. **Three distinct wait sets with manifest-frozen timeouts** (§9.4 `lease.wait_sets`):
   - `cgroup_census` — live cgroup PID census (includes supervisor); timeout 30 s
   - `harness_wait_set` — reaped harness PID only (single process); timeout 20 s outer + 5 s kill grace
   - `finalizer_wait_receipt` — finalizer acknowledgment; timeout 10 s outer + 5 s kill grace
7. **Bounded state machine** (states: `PRELAUNCH`, `RUNNING`, `DRAINING`, `FINALIZING`, `TERMINAL`) with transitions only on verified events (PID exit, finalizer ack, timeout). Each transition logs: `from`, `to`, `trigger`, `monotonic_timestamp`, `cgroup_census_snapshot`. Timeouts frozen in manifest: `launch=20s`, `drain=30s`, `finalize=10s`, `total=60s`.
8. **Lease release:** only by supervisor after `FINALIZING` → `TERMINAL`; `flock(LOCK_UN)` on owned FD.
9. **Containment:** any timeout/escalation → `SHUTDOWN_BLOCKED`; no second shutdown request; no `qm stop`/force-stop.
10. The supervisor seals and hashes each result tree after its final record. It releases the lease only after both run records are closed (or Q0 has failed closed with Q1 `NOT_RUN`) and exits the unit. `KillMode=control-group` is a last-resort containment mechanism, not proof of reaping; each receipt must prove zero unexpected cgroup members and the expected wait results before the next phase. Timeout/signal behavior is pinned in the manifest; no retry or broad cleanup.
11. Any lease/UID/process/log/identity mismatch, timeout, unexpected child, incomplete receipt or failed journal boundary preserves pending/partial evidence and stops. D3 is not part of this unit.

## 4. Partial pre-construction reservation — INCONCLUSIVE retention (aligned to §9.5)

If reservation (UID, namespace, IPC, tmpfs, RLIMITs) fails **before** `Process`/`GW` construction:
- Classification: `INCONCLUSIVE` (never `PASS`, never `FAIL`)
- `cleanup_outcome = "unknown"`
- **Retention without cleanup:** preserve all partial artifacts (staging record, IPC pre-inventory, manifests) under `D2_RESULTS/<run_id>/partial/`; do **not** attempt destructive cleanup.
- `abnormal_evidence_only = true` **only if** a valid common authority (owner-signed receipt) authorizes it; otherwise `false`.
- **Journal contract:** no `CLEANUP_INTENT` or `RUN_SHUTDOWN_COMPLETE` emitted for unconstructed run (cannot truthfully emit).
- **Run-record schema `allOf` addition:** cross-field rule enforces `status == "INCONCLUSIVE" && cleanup_outcome == "unknown" && !record_finalized` for partial-reservation paths.

## 5. Installation and test scope — not authorized or performed

Provisioning would require privileged changes to the host account database, `/etc/systemd/system`, `/usr/local/libexec`, `/etc/tmpfiles.d`, `/run/lock/ng039-d2`, and `/var/lib/ng039-d2`. Before installation, freeze exact files, contents, modes/owners, hashes, unit properties, rollback and no-run acceptance tests and obtain owner approval. A no-Nova/no-IPC service check must verify the actual UID transition, root-only unit/launcher and lease permissions, polkit/sudoers restrictions, live UID census, cgroup tracking, IPC namespace identity, raw `/proc/sysvipc` access, `/dev/shm` mount/device/inode visibility, safe control/log FDs, receipt read-only access, one-use authorization and fail-closed behavior. It must not create/remove IPC or invoke the real D2 harness. Exact semaphore backing-object mapping cannot be established by this no-IPC check; prove it only in the separately authorized D2 reservation phase before constructing `Process/GW`.

**Not done:** no user/group, file, systemd unit, lock, directory, or launcher was created; no `sudo` was run; no service was started; no Astra call was made. This design remains a proposal until owner decision and documentary review.

---

## 6. Manifest alignment certification (§9.6)

The following manifest fields **must** reflect the above contracts exactly (frozen at v1.18 SPEC SHA `2054b669da7856aded2ba459f2dfccd9a310bcb8ec3fb4268e3a7c248f9a1507`):

| Manifest field | v1.18 value (this proposal) |
|---|---|
| `execution_model` | `"single_process_two_phase"` |
| `cardinality.process_start_count` | `1` |
| `cardinality.run_id_count` | `1` |
| `cardinality.journal_count` | `1` |
| `cardinality.final_record_count` | `1` |
| `sem_failure.protocol` | `"sticky_abort_exit127"` |
| `sem_failure.fault_injection_cases` | `["ENOENT","EACCES","EMFILE"]` |
| `link_model.diagnostic_target` | `"hybrid_-Wl,-Bdynamic_-lc"` |
| `link_model.provider_evidence_schema` | `"ng039-sem-provider-evidence/v1"` |
| `lease.fd_owner` | `"supervisor_root"` |
| `lease.finalizer_has_fd` | `false` |
| `lease.wait_sets` | `["cgroup_census","harness_wait_set","finalizer_wait_receipt"]` |
| `lease.timeouts` | `{"launch":20,"drain":30,"finalize":10,"total":60}` |
| `partial_reservation.classification` | `"INCONCLUSIVE"` |
| `partial_reservation.cleanup_outcome` | `"unknown"` |
| `partial_reservation.retention_without_cleanup` | `true` |
| `partial_reservation.abnormal_evidence_only_condition` | `"valid_common_authority_receipt"` |

**No drift between SPEC §9, manifest JSON, and this launcher proposal is permitted at verification gate.**