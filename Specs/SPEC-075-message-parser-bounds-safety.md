# SPEC-075: Message Parser Bounds Safety on AIOPT5

**Version:** v0.3-draft
**Date:** 2026-10-23
**Author:** Hermes Agent; owner selected the candidate for HERMES-006 item 2
**Status:** Draft — second documentary amendment after Astra design review; implementation is NOT approved
**Branch:** AIOPT5
**Proposed source baseline:** `6b3fe17b20b63ce670328a551f0ba8d6ad463b02`
**Owner task:** `HERMES-006-ciclo-engenharia-kanban-novagenesis-v1-20261018.md`
**Related (context only):** `SPEC-033-message-commandline-refactor.md` (historical AIOPT3 Draft; not governing), `SPEC-074-common-problem-inventory-and-ase-ranking.md` (selection/ranking only; not an implementation specification)

> **Identity boundary:** The declared source baseline is the published `origin/AIOPT5` commit above. The current primary checkout remains at `603d304b1413aa885fa405b2fe556cefc323674a` and has independent local changes that must be preserved. This local Draft does not assert that the primary checkout matches the proposed baseline and does not authorize checkout, reset, clean, merge, or transplanting files.

> **Authority boundary:** `SPEC-033` records historical AIOPT3 parser principles, including separate bounded parser amendments and wire compatibility, but transfers no approval or implementation authority to this AIOPT5 slice. `SPEC-074` selected this candidate from a read-only inventory and transfers no execution authority. `SPEC-027`, `SPEC-039`, and `SPEC-073` are not operational dependencies and grant no authorization for this slice.

## 1. Problem and objective

`Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` scans a buffer copied by `Message::SetMessageFromCharArray()`. The setter allocates exactly `MessageSize` bytes. The parser enters only when `MessageSize > 16` and `DeleteMessageArray == true`; it scans while `t < MessageSize` and recognizes a command-line prefix with:

```cpp
if (Msg[t] == 'n' && Msg[t + 1] == 'g' && Msg[t + 2] == ' ' && Msg[t + 3] == '-')
```

This is a **conditional source-level out-of-bounds-read risk**. At a command-recognition position with one remaining byte `n`, short-circuit evaluation reaches `Msg[t + 1]`; with two bytes `ng`, it reaches `Msg[t + 2]`; with three bytes `ng `, it reaches `Msg[t + 3]`. The loop guard alone does not establish those reads are in bounds. This is not a reproduced runtime failure, crash, or exploit claim.

The supplied source evidence shows network-receive and shared-memory receive callers in `Common/src/GW.cpp`; it does not claim an exhaustive repository-wide caller census. The supplied **network** caller visibly calls `PushToInputQueue(PM)` after deserialization without making queue admission conditional on the parser return value. The supplied shared-memory excerpt does not extend far enough to establish its later queue-admission policy. Therefore this slice cannot claim end-to-end rejection or disposal merely by choosing a parser return value.

The objective is one minimal correction that bounds every evaluated look-ahead at this prefix-recognition site while preserving valid-message behavior and the defined nonmatching-fallback cases below.

## 2. Scope and invariants

### Included

- The bounds condition for command-line-prefix recognition in `Common/src/Message.cpp`.
- A production-linked deterministic test plan for truncated input and valid control input.
- A precise failure contract before any code is written.

### Required invariants

- Preserve wire representation and bytes of valid messages.
- Preserve valid-message command-line and payload interpretation, ordering, ownership, lifecycle, queues, scheduling, and public APIs.
- Preserve the existing nonmatching-fallback behavior defined in §4 for the partial-prefix fixtures; this SPEC does not redefine malformed-input grammar.
- Do not infer that static caller reachability establishes runtime frequency.
- Do not treat an `ERROR` return as downstream rejection, disposal, or a `PushToInputQueue` policy change.

### Excluded

- Parser consolidation, `CommandLine` parser changes, Message ownership/container refactoring, Gateway scheduling, queue changes, serialization redesign, wire-format changes, and a Gateway policy change for parser failures.
- A complete `ng -` prefix lacking a terminating newline: that is a separate parser condition, not a partial-look-ahead fixture in this SPEC.
- Payload content that merely ends in `n`, `ng`, or `ng ` after the parser has already entered its payload branch: this SPEC must not introduce a general ban or altered interpretation for such valid payload bytes.
- `SPEC-027` SAR work, `SPEC-039` Q0/Q1 work, and `SPEC-073` performance work.
- Changes to `Common/src/Message.h` unless a later approved amendment demonstrates an unavoidable API/documentation need.
- Build, test execution, CMake edits, sanitizer execution, runtime, VM/IPC, Kanban dispatch, external Astra/JEV/Codex egress except an explicitly consented documentary review, commit, and push.

## 3. Proposed correction boundary

The proposed production allowlist is exactly:

```text
Common/src/Message.cpp
```

The future focused test-file allowlist, subject to separate approval, is:

```text
Tools/tests/NGMessageParserBoundsTest.cpp
```

No `CMakeLists.txt` change is proposed. The correction must check that at least four bytes remain before evaluating the four-byte `ng -` prefix. It must not add sentinel bytes, rely on allocator padding, broaden parser grammar, or change the treatment of complete valid input.

## 4. Future deterministic oracle

### 4.1 Common fixture setup and observability

Each future fixture uses a fresh `Message` initialized with `HasPayloadFlag=false`. It first calls `SetMessageFromCharArray()` once with the exact fixture bytes and declared byte count; that setter call must return `OK`. The fixture then calls `ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2()` once.

The test must observe only existing public state/API: parser return value; `GetNumberofCommandLines`; each available `GetCommandLine` result and its public `Name`, `Alternative`, and `Version`; `GetHasPayloadFlag`; `GetPayloadSize`; `GetPayloadFromCharArray`; `GetMessageSize`; and `GetMessageFromCharArray`. The copied serialized message bytes and declared message size must remain unchanged in every fixture.

`ng -x --y 0.1 [ ]\n` is the valid command-line prefix used below. Its exact bytes are ASCII:

```text
6e 67 20 2d 78 20 2d 2d 79 20 30 2e 31 20 5b 20 5d 0a
```

It is 18 bytes, so every fixture satisfies the parser entry condition `MessageSize > 16`. In the current parser, the newline ends the command line, then the outer scan advances once to the immediately following suffix or payload byte.

### 4.2 Partial-prefix fixtures and existing fallback contract

The correction must preserve the following observable behavior of the current nonmatching fallback branch. These fixtures establish semantic preservation; they do not by themselves prove that an out-of-bounds read did not occur in the baseline.

| ID | Exact full serialized bytes | Recognition position / remaining suffix | Expected parser return | Expected command lines | Expected public payload state |
|---|---|---|---|---|---|
| `P1` | `ng -x --y 0.1 [ ]\nn` | final byte `n`; 1 remaining | `OK` | exactly one parsed command line: name `-x`, alternative `--y`, version `0.1`, in position 0 | `GetHasPayloadFlag()==true`; `GetPayloadSize` returns `OK` with `0`; `GetPayloadFromCharArray` returns `ERROR` because size is zero |
| `P2` | `ng -x --y 0.1 [ ]\nng` | final bytes `ng`; 2 remaining | `OK` | same one command line, same position/order | payload flag true; size `1`; payload bytes exactly `g` |
| `P3` | `ng -x --y 0.1 [ ]\nng ` | final bytes `ng `; 3 remaining | `OK` | same one command line, same position/order | payload flag true; size `2`; payload bytes exactly `g ` |

For `P1`–`P3`, the serialized bytes remain unchanged and `GetMessageSize` returns the original fixture size. The documented payload behavior follows the currently supplied fallback: it increments `t` once before copying the remaining bytes. It is an explicit bounded compatibility contract for this slice, not a claim that this is the desired general malformed-input policy.

### 4.3 Valid and payload controls

The second newline in `V1` and `V2` is an explicit separator consumed by the existing nonmatching fallback before it copies payload. It is not part of the payload.

| ID | Exact full serialized bytes | Serialized size | Expected parser return | Expected command lines | Expected public payload state |
|---|---|---:|---|---|---|
| `V1` | `ng -x --y 0.1 [ ]\n\npayload` | 26 | `OK` | exactly one command line: name `-x`, alternative `--y`, version `0.1`, in position 0 | payload flag true; size `7`; bytes exactly `payload` |
| `V2` | `ng -x --y 0.1 [ ]\n\npayload ng ` | 29 | `OK` | same one command line, same position/order | payload flag true; size `10`; bytes exactly `payload ng `, including the terminal `ng ` |

`V2` is a control against treating matching suffix bytes in already-entered payload as a parser error or a command-recognition event.

### 4.4 Separate evidence roles

The future test has four separate evidence boundaries:

1. **Structural RED claim on the baseline:** a narrowly scoped source assertion must identify that the prefix condition evaluates `Msg[t+1]`, `Msg[t+2]`, and `Msg[t+3]` without an explicit remaining-byte guard. This is the deterministic baseline inadequacy signal; it is not execution evidence.
2. **Baseline behavioral boundary:** `P1`–`P3` reach an expression that is out of bounds in the baseline before the fallback is established. Their observed baseline behavior is not guaranteed, must not be required for RED, and cannot be accepted as a deterministic baseline oracle.
3. **Required corrected-parser contract:** after the separately authorized correction, `P1`–`P3` must satisfy the return/state contract in §4.2. `V1` and `V2` are safe preservation controls for the defined valid payload behavior; they do not independently prove memory safety.
4. **Complementary memory-safety claim:** AddressSanitizer or an equivalent mechanism may later test the same bounded fixtures only under separate authorization. No deterministic crash, ASan finding, or baseline runtime failure is promised by this SPEC.

## 5. Gates and rollback

1. **Document approval:** owner approves this amended Draft's exact bytes and any required documentary review result before test creation or execution.
2. **Test authorization:** owner separately authorizes the focused RED source assertion and the standalone behavioral test in an isolated worktree at a freshly pinned source identity.
3. **Implementation authorization:** after the authorized RED evidence and the same exact allowlist, owner separately authorizes one minimal source correction.
4. **Validation/review:** run only separately authorized focused checks and an independent read-only diff review. No result accepts broader Message, Gateway, Common, or NovaGenesis behavior.

Rollback is a narrowly reviewed revert of the one authorized source change. Preserve failure evidence and do not mix a rollback with unrelated worktree cleanup.

## 6. Review history and current state

- The owner approved v0.1 as a documentary Draft. Astra reviewed packet `121a2f9751f186ccfcefc5bcadb22eb7124162c09998c44136ed2416d6e12986` and returned `REVISIONS_REQUIRED`; v0.2 addressed its state contract, concrete fixtures, valid control, evidence-role separation, and qualified source/caller claims.
- Astra then reviewed v0.2 packet `0823c47c4bd3599689dbed0c118658f93681a614d95bdcd46477256a627044f8` and returned `AMEND`; receipt `~/.hermes/reviews/HERMES-006/2026-10-23T-spec075-amendment-review/submission-receipt.json` SHA-256 `bbc008a3378ddc338770721e66f2801cf1ecc7d69ff326a970cc73ae8e3c1604`, derived response SHA-256 `3420d666bafbc7cec28e0964148f10bfed2279e7200993c9e7be0225b16440ec`. This v0.3 amendment adds the control separator and payload sizes, distinguishes unsafe-baseline observations from the corrected-parser contract, and narrows the supplied caller statement. That verdict is historical evidence, not approval of these new bytes.
- A new packet and fresh exact-SHA consent are required for any follow-up Astra review.
- **Owner acceptance (2026-10-24):** accepts the local candidate guard only at detached baseline `6b3fe17b20b63ce670328a551f0ba8d6ad463b02`, production diff SHA-256 `6cdcb4f8c0bedc3aa8bc520643a5fda66093f9bb5c28f2a6484c1288bb22eb31`, and GREEN receipt `19d338644bf309f4c642575b4c518ae80a37d5f361c3d9b65dd298bf2b95ff3b`. The optional structural-checker-order and harness-comment hardenings are deliberately deferred and not applied.
- This accepts neither broader Message parsing nor legacy warnings, general memory safety, full product behavior, runtime/IPC/VM evidence, commit, or push. The isolated candidate must not be silently merged or copied into the primary checkout; commit preparation, commit, and push are separate gates.
- This Draft does not reopen NG-075, NG-039, NG-071, NG-062, or the paused NG-073 line.
