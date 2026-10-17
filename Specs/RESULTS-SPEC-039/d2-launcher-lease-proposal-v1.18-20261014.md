# NG-039 D2 launcher/lease provisioning proposal — v1.18 aligned

**Status:** candidate design for owner/reviewer decision; not normative, installed, or tested.
**Purpose:** satisfy SPEC-039 v1.18 §§9.1–9.3 dedicated-identity and continuous-lease prerequisite for one local D2 Q0/Q1 correctness run.
**Boundary:** this proposal does not authorize D2 code edits, configure/build, harness/finalizer execution, IPC creation/removal, Astra submission, VM/SSH, commit, or push.
**Alignment:** the table projects SPEC §9.2. Full pre-code validation remains HOLD; this is not a certification of executable readiness.

## 1. Host facts checked read-only

- Local host exposes `systemd 255.4`; `flock`, `setpriv`, `runuser`, `useradd`, and `groupadd` are available. None was used to mutate state.
- The owner's home directory is mode `0750`; a future D2 UID must not be added to the owner's group or granted access to the home tree merely to reach test files.
- `/var/lib` and `/srv` are root-owned `0755`; `/proc/sysvipc` is readable (`0555`) and `/dev/shm` is host-visible (`01777`). Candidate `<WORKSPACE_ROOT>` is `/var/lib/ng039-d2/workspace`, outside the private home and `.hermes`; SPEC §9.1 leaves `<WORKSPACE_ROOT>` to be resolved and recorded in the run manifest.
- No matching named `ng039` account, service unit, or launcher was found in the bounded preflight; this is not an exhaustive root-owned filesystem audit. No D2 test files exist in either examined checkout.

## 2. Candidate identity and files — pending approval

| Item | Proposed value | Required invariant |
|---|---|---|
| Dedicated account/group | `ng039d2` | Non-login shell `/usr/sbin/nologin`; locked password; no supplementary groups, SSH key, sudoers/polkit launch grant, or unrelated system/user service; reserved only for this D2 gate. These facts must be independently checked at install and pre-run. |
| Systemd unit | `/etc/systemd/system/ng039-d2.service` | Root-owned, fixed one-shot entry (`User=root`, `Group=root`, `Restart=no`, `KillMode=control-group`). Only root may start/modify it. Candidate hardening: `NoNewPrivileges=yes`, `ProtectSystem=strict`, `ProtectHome=yes`, `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no`, `PrivateUsers=no`, `PrivateNetwork=no`, `ProtectProc=default`, `RestrictAddressFamilies=AF_UNIX`, `CapabilityBoundingSet=CAP_SETUID CAP_SETGID CAP_KILL CAP_DAC_READ_SEARCH`, plus explicit `ReadWritePaths` for the pinned workspace/control paths and host `/dev/shm`. The exact property set must be reviewed and a no-IPC service-side preflight must prove it does not hide or replace `/proc/sysvipc` or the host `/dev/shm` mount. |
| Supervisor | `/usr/local/libexec/ng039-d2-supervisor` | Root-owned, non-writable by `ng039d2`; fixed argument contract; SHA-256 pinned in the run manifest. It holds the root-only lease, launches only the pinned harness and finalizer without a shell, and drops each controlled child to `ng039d2` using an explicit `setpriv --reuid=... --regid=... --clear-groups --no-new-privs -- ...` argv. **The harness never inherits the lease FD.** The finalizer receives no lease FD; only the supervisor owns the lock-capable descriptor. The supervisor retains its own lease FD until finalizer exit and final-record verification. The build/check phase is separate and must finish, be pinned, and be made read-only before this one-shot run service starts. The finalizer is sequential, never concurrent with the harness. |
| Workspace root | `/var/lib/ng039-d2/workspace` (`<WORKSPACE_ROOT>`) | Root-managed and not writable by `ng039d2`. Freeze fresh sibling roots beneath it: `source/<run-id>/`, `build/<run-id>/`, `results/<run-id>/`, and `scratch/<run-id>/{home,tmp}`. Source/manifest and the post-build executable become root-owned/read-only; build output is writable only during configure/build; results and the distinct HOME/TMPDIR are writable only by the D2 UID. Verify pairwise non-overlap, symlink-free parents, device/inode and ownership before use. No `.hermes` paths. |
| Control and lease | `/var/lib/ng039-d2/control/<run-id>/`; `/run/lock/ng039-d2/lease.lock` | Control parents are root-managed; `/var/lib/ng039-d2/control` and the per-run control directory are root:`ng039d2` mode `0710`, so the D2 UID can traverse but cannot list or modify them. `supervisor-termination.json` and closed logs are root:`ng039d2` mode `0440`, opened with no-follow/exclusive identity checks and fsynced/hashed by the supervisor before the finalizer receives read-only access. Lease directory/file are root-managed `0700`/`0600`; supervisor checks regular-file, owner/mode, `st_dev`/`st_ino`, opens with `O_NOFOLLOW|O_CLOEXEC`, obtains nonblocking exclusive `flock`, and continuously verifies the same held FD/identity through final-record closure. `ng039d2` cannot open, replace, or remove the lease. |
| IPC visibility | systemd `PrivateIPC=no`, `PrivateTmp=no`, `PrivateDevices=no` | No IPC or mount namespace may hide/replace host resources. Before any Nova/IPC run, a no-Nova preflight under the exact unit must compare service/host IPC namespace identity, raw `/proc/sysvipc` readability/hash and `/dev/shm` mount/device/inode identity without creating/removing IPC. Exact `sem_t`-to-object device/inode mapping remains a required check during the separately authorized D2 reservation phase. No network input. |

**Owner trust assumption:** host root/systemd remains trusted, as allowed by SPEC §9.1. The fixed root-owned unit/launcher is the only permitted entry to `ng039d2`; account/session/service/polkit audits plus a live UID census and systemd-cgroup membership check must show no unrelated process before pre-inventory and at every boundary. Any unlisted process or process outside the unit cgroup blocks finalization and preserves evidence. The systemd cgroup is the process-tree source of truth; a process group alone is insufficient. If root-level concurrent mutation is in scope, this shared-host profile is unacceptable and D2 remains blocked.

## 3. Proposed single-run supervisor protocol (SPEC §9.1)

1. Consume one root-owned one-use owner authorisation for one harness process, one run ID and one journal. The harness performs Q0 then Q1 in-process; Q0 uncertainty makes Q1 NOT_RUN. No intermediate finalizer, new constructor or separate Q1 pre-inventory is permitted. No retry/restart.
2. The root supervisor obtains the exclusive lease before any inventory or reservation and retains its FD through the single finalization sequence. Neither child receives or opens it. Verify the fixed unit, UID, source/binary/manifest identities, protected paths and live UID/cgroup census before launch. Any unknown or escaped process blocks the run.
3. Configure/build/checks are separate authorised steps, never part of this service. Freeze the executable hash and root-owned read-only source/binary before launch. Candidate environment is exactly PATH=/usr/bin:/bin, HOME=<WORKSPACE_ROOT>/scratch/<run-id>/home, TMPDIR=<WORKSPACE_ROOT>/scratch/<run-id>/tmp, LANG=C.UTF-8 and LC_ALL=C.UTF-8. Paths must be distinct and identity-checked.
4. Use host-visible IPC, not unshare or a private tmpfs. Capture attributable host inventories; preserve all foreign objects. Prove exclusive reservations and backing-object identities before Process/GW construction. An empty host inventory is not required. Exact reservation and cleanup implementation contracts remain OPEN under SPEC §9.1; no host IPC is authorised by this proposal.
5. Launch the pinned harness once through fixed no-shell setpriv argv. It creates no child process; Gateway/output workers are threads. Record PID/PPID/PGID, start time, UID/GID tuple, executable identity/hash and cgroup membership. Record raw waitpid results independently of wrapper exit codes.
6. Distinguish cgroup_census (live set including supervisor), harness_wait_set (the reaped harness child), and finalizer_wait_receipt (the sequential post-processing child). These sets are not interchangeable. After all required waits and closed/fsynced log receipts, invoke the finalizer at most once, only if every common authority/identity/lease gate remains valid. It receives read-only receipts/logs, no lease FD. Receipt/liveness protocol and its negative fixtures remain OPEN; receipt existence alone is insufficient proof.
7. States are PRELAUNCH, RUNNING, DRAINING, FINALIZING, TERMINAL, plus SHUTDOWN_BLOCKED on uncertainty. Record each transition with monotonic time, trigger and census. Budgets launch=20s, drain=30s, finalize=10s are maxima within total=60s; all kill grace counts inside that total. Timeout is not a successful TERMINAL transition and never authorises automatic retry, cleanup or lease release. Durable containment/recovery on supervisor death or blocked shutdown remains OPEN; flock cannot survive its last owning FD closing.
8. After successful finalizer exit and lease/result identity verification, seal the single result tree. Only then may the supervisor release its FD. Missing final record is INCONCLUSIVE, not a fabricated record to satisfy cardinality. Release/recovery for abnormal retained-resource outcomes requires an explicit reviewed policy and owner decision; no automatic broad cleanup.

## 4. Partial reservation and evidence-only finalization (SPEC §9.1)

If pre-construction reservation fails, retain partial evidence/resources without destructive cleanup or retry. Classify INCONCLUSIVE with cleanup_outcome=unknown; never emit RUN_SHUTDOWN_COMPLETE or CLEANUP_INTENT for an unconstructed run. Invalid or missing common authority forbids finalizer invocation and leaves pending evidence. Valid common authority may permit one abnormal evidence-only final record, still INCONCLUSIVE. Therefore record_finalized is false for pending evidence and true only after authorised successful evidence finalisation, not universally false. The corresponding strict schemas and semantic fixtures remain OPEN, not implemented by this paragraph.

## 5. Installation and test scope — not authorized or performed

Provisioning would require privileged changes to the host account database, `/etc/systemd/system`, `/usr/local/libexec`, `/etc/tmpfiles.d`, `/run/lock/ng039-d2`, and `/var/lib/ng039-d2`. Before installation, freeze exact files, contents, modes/owners, hashes, unit properties, rollback and no-run acceptance tests and obtain owner approval. A no-Nova/no-IPC service check must verify the actual UID transition, root-only unit/launcher and lease permissions, polkit/sudoers restrictions, live UID census, cgroup tracking, IPC namespace identity, raw `/proc/sysvipc` access, `/dev/shm` mount/device/inode visibility, safe control/log FDs, receipt read-only access, one-use authorization and fail-closed behavior. It must not create/remove IPC or invoke the real D2 harness. Exact semaphore backing-object mapping cannot be established by this no-IPC check; prove it only in the separately authorized D2 reservation phase before constructing `Process/GW`.

**Not done:** no user/group, file, systemd unit, lock, directory, or launcher was created; no `sudo` was run; no service was started; no Astra call was made. This design remains a proposal until owner decision and documentary review.

---

## 6. Documentary field projection (SPEC §9.2; not acceptance)

The following manifest fields **must** reflect the above contracts exactly (frozen at v1.18 SPEC SHA `80713abc89f0b945f0c752f56f58cbfaa2c847c71b79fbb453fa219450c701c1`):

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