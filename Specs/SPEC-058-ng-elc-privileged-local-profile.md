# SPEC-058: NG-ELC Native Privileged Local Profile

**Author:** Antonio Alberti / Hermes Agent
**Date:** 2026-09-16
**Status:** In Progress
**Branch:** AIOPT3
**Implementation commit:** `6a28988`, `c12692a`, `5a5877a`, `da7a859` (profile validation, foreign-inventory gate, bounded privileged cleanup, `setsid --ctty` PTY launch/capture, RED→GREEN tests; runtime acceptance remains open)
**Related:** SPEC-055-ng-elc-local-mode.md; SPEC-056-ng-elc-evidence-provenance-hardening.md; SPEC-054-release-master-plan-and-audit.md
**Task linkage:** NG-056 (child of NG-050)
**Canonical tool:** `NG Experiment Lifecycle Controller (NG-ELC)` — `Scripts/AlpineVMs/ng_remote_executor.py`

## 1. Decision and problem

The existing SPEC-055/SPEC-056 local contract describes unprivileged subprocess execution without a PTY. The historical supported NovaGenesis native execution contract requires privileged cleanup, raw-socket/IPC-compatible ownership and a terminal-backed launch. The first local trials that did not reproduce that contract were invalidated as setup failures rather than runtime results.

This SPEC resolves that contradiction without silently changing the existing fixture contract or the remote executor:

- the existing unprivileged local subprocess profile remains the default fixture/contract profile;
- this SPEC adds an explicit, opt-in `native-privileged` profile for the real native NovaGenesis local scenario;
- this SPEC takes precedence over the unconditional local prohibitions in SPEC-055 §5/§6 and SPEC-056 §2.2 only for the explicitly selected `native-privileged` profile;
- no local trial, whatever its result, closes a multi-VM or release gate by itself.

## 2. Scope

### Included

- local plan/profile selection and fail-closed evidence;
- root-boundary and repository-owned privileged cleanup preflight;
- four native roles: PGCS `-lc`, NRNCS, Repository ContentApp and Source ContentApp;
- one PTY topology per role with bounded durable capture;
- preservation of SPEC-056 R01–R06, lifecycle, C01, C02 and C03 rules;
- RED→GREEN tests and one fresh bounded five-photo trial after implementation.

### Excluded

- changes to C++, wire format, protocol, pub/sub model or payload semantics;
- changes to remote schema-v1 behavior, SSH transport or remote CLI defaults;
- internal `sudo`, password prompts, privilege escalation or `sudoers` changes;
- graphical terminals, `run_*.sh`, arbitrary shell command strings or a second supervisor;
- blanket `killall`, unowned IPC deletion or weakening of evidence/cleanup guards;
- release acceptance, tag creation or promotion of local PASS to M1/M2/M3.

## 3. Profile contract

### 3.1 Explicit selection

The native profile MUST be selected by both:

1. a canonical local plan declaration, for example:

```json
"local_profile": "native-privileged"
```

2. an explicit CLI/profile selection accepted by the controller.

The exact CLI spelling is an implementation detail to be fixed by the tests and implementation, but the plan and CLI MUST agree. Unknown, conflicting, omitted or remote-mode profile declarations MUST fail closed. Effective UID 0 alone MUST NOT select this profile.

The selected profile, plan value, CLI value, explicitness of both selections and effective UID MUST be persisted in the evidence bundle before any role launch.

### 3.2 Root boundary

The controller MUST be invoked with effective UID 0 before cleanup or role launch. It MUST NOT invoke `sudo`, prompt for a password or perform internal privilege escalation. A non-root invocation MUST fail before destructive or role-launch side effects.

The profile MUST validate trusted repository and evidence paths, reject symlink/path substitution and preserve the existing protected-input and provenance rules. Running the controller as root does not make arbitrary plans, environment variables, executables or evidence paths trusted.

### 3.3 Privileged cleanup

After non-destructive preflight and before the accepted baseline inventory, the controller MUST invoke only the repository-owned cleanup entry point:

```text
Scripts/Simple/clean.sh
```

The invocation MUST use an argument vector equivalent to `bash <absolute-repository-path>/Scripts/Simple/clean.sh`; it MUST NOT construct a shell command string or invoke an arbitrary wrapper.

The controller MUST persist:

- cleanup command identity and hash;
- exit status and bounded stdout/stderr;
- pre-clean inventory and ownership classification;
- post-clean process and IPC inventory.

Because the repository cleanup script contains legacy global process/IPC removal, the pre-clean inventory MUST already be complete and empty. Any foreign process, any System V shared-memory/semaphore/message-queue object regardless of owner, named POSIX semaphore, unreadable inventory or ambiguous ownership MUST block invocation; the controller MUST never invoke the script merely to make the inventory empty. Inventory commands MUST use trusted absolute tool paths and a sanitized environment. The script MUST be opened without symlink following and executed through a retained file descriptor, with a sanitized environment and bounded output capture. The recorded command and pre-execution script identity MUST be retained.

Nonzero cleanup status, incomplete inventory, ambiguous ownership or failure to establish the declared zero-process/zero-IPC baseline MUST block role launch. The controller MUST not delete foreign resources to obtain a zero baseline. The exact baseline scope MUST be explicit in the plan and evidence.

The cleanup script itself is a privileged dependency. Its repository content and behavior MUST be included in provenance; a filename alone is not proof of ownership-safe cleanup.

### 3.4 Role identity and groups

PGCS, NRNCS, Repository and Source MUST each run with effective UID 0. Each role MUST retain its own session/process group, PID, PGID, starttime and executable identity. A shared PGID across roles is forbidden by this SPEC because it would weaken the existing reverse-order identity-scoped teardown model.

All SPEC-056 C01/C02/C03 protections remain binding. Privilege MUST NOT relax PID reuse checks, descendant tracking, IPC attribution, path containment, evidence preservation or cleanup verdicts.

### 3.5 PTY and capture

Each role MUST use one dedicated PTY pair. The implementation MUST define and test:

- controlling-terminal setup, not merely a PTY file descriptor attached to standard streams;
- a role-specific session/process group compatible with identity-scoped teardown;
- an authoritative durable capture stream with bounded concurrent master draining;
- partial reads, decoding boundaries, PTY EOF/closure, output backpressure and final drain;
- output/log quota enforcement, truncation/error recording and bounded drain finalization.

The implementation MUST use a PTY-native child/session boundary that establishes the controlling terminal without Python `preexec_fn` execution after capture threads exist. After the child boundary is established, the controller MUST drain concurrently, enforce the declared byte quota, preserve final output before closing the master, and record any timeout or write/EOF ambiguity.

The initial implementation SHOULD attach the PTY to role stdin/stdout and retain stderr as a separate durable file, unless a reviewed alternative preserves equivalent provenance. The evidence schema MUST state which stream is authoritative; stdout/stderr MUST NOT be presented as independently captured when they were merged.

Readiness MUST consume persisted captured output. Capture failure, incomplete drain, write failure, quota exhaustion or ambiguous PTY closure MUST prevent an accepted PASS.

## 4. Compatibility and non-regression

- The unprivileged local fixture profile remains available and remains the default for unit tests unless a test explicitly selects `native-privileged`.
- Existing local lifecycle tests MUST remain green.
- Remote schema-v1, remote CLI behavior and remote launch/teardown MUST remain unchanged.
- The native profile MUST be rejected in remote mode.
- No C++ or protocol files may change in the implementation commit.

## 5. Required RED→GREEN tests

The new behavior tests MUST demonstrate the expected failure against the current implementation before the implementation change, then pass afterwards. Existing invariant tests remain green throughout.

### Profile and privilege

- default local fixture remains unprivileged;
- explicit native profile is recorded in plan/CLI/evidence;
- unknown, conflicting, omitted and remote-inapplicable profiles fail closed;
- root without explicit native selection does not select native;
- non-root native invocation fails before cleanup, launch or destructive side effects;
- no internal sudo/password path is used.

### Cleanup and ownership

- repository-owned cleanup is invoked exactly once before the accepted baseline;
- cleanup status/output are retained;
- cleanup failure, inventory failure, foreign resources and ambiguous ownership block launch;
- prohibited cleanup behavior and unsafe script/path substitution fail closed;
- zero baseline is verified without deleting unrelated resources.

### Native launch and identity

- all four roles run with EUID 0 in the privileged integration fixture;
- roles have distinct tracked sessions/groups and identity records;
- shell-string execution is rejected;
- partial launch and identity-registration failure clean only verified owned roles/resources;
- PID reuse, executable drift and symlink substitution fail closed.

### PTY capture

- controlling-terminal semantics are verified;
- concurrent high-volume output is drained without deadlock;
- split readiness markers and UTF-8 decoding boundaries are handled;
- final output on normal exit and abnormal PTY closure is retained;
- disk/write failure, quota overflow, drain timeout and early role death cannot yield accepted PASS.

### Compatibility

- all existing executor tests remain green;
- remote regression and CLI tests remain green;
- unprivileged fixtures do not require root and do not change host IPC;
- native integration tests are isolated and record their privilege/terminal preconditions.

## 6. Acceptance boundary

Implementation readiness requires the targeted RED→GREEN tests, local and remote regression suites, syntax checks, no production C++/protocol changes and a review of the exact implementation diff.

SPEC-058 acceptance additionally requires:

1. a clean candidate with verified build linkage;
2. one fresh bounded native-privileged five-photo local trial;
3. a complete sealed evidence bundle with offline verification;
4. zero attributable processes/IPC after teardown;
5. Astra review of the exact candidate, tests and retained bundle.

This does not close SPEC-056 automatically, and it does not close M1, M2 or M3. Any runtime failure or setup invalidation remains recorded as evidence, not rewritten as PASS.

## 7. Rollback

If privilege, PTY, cleanup ownership, remote regression or evidence safety fails, disable the native profile and retain the unprivileged fixture and remote behavior. Do not weaken fail-closed guards or restore historical local PASS claims. Preserve all failed bundles and record the exact candidate and unresolved resources.

## 8. Implementation order

1. Add the SPEC and explicit precedence references in SPEC-055/SPEC-056.
2. Add RED tests for profile, root boundary, cleanup contract, PTY topology and capture failure.
3. Implement profile validation and evidence recording without changing remote mode.
4. Implement privileged cleanup preflight and fail-closed baseline verification.
5. Implement PTY-backed role launch/capture with identity-safe teardown.
6. Run focused and complete suites independently, plus syntax checks and a real-interface fixture.
7. Freeze a clean candidate and run one fresh native-privileged local trial.
8. Submit the exact diff and retained bundle for Astra review; keep M1/G3 open until review permits advancement.
