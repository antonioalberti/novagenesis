# ANALYSIS — Message / CommandLine structure: simplification and de-verbosity

Author: Hermes (pre-Astra analysis). Date: 2026-09-07.
Scope: `Common/src/Message.h/.cpp`, `Common/src/CommandLine.h/.cpp`, `Common/src/MessageBuilder.h/.cpp`.
Goal: analyse possible simplifications of the message structure, especially the
CommandLine wire format verbosity (type field "s" inside `[ < 1 s X > ]`).

---

## 1. The wire format today

Each CommandLine serializes as:

    ng -cl 0.1 [ < 3 s E1 E2 E3 > < 1 s E1 > ]

Per argument vector we pay: `<`, size (decimal), type char (`s`), N elements, `>`.
Observations:

1. **The type field is dead weight.** Every element is stored as `string` in
   `CommandLine::Arguments` (a `string**`). There is no heterogeneous typing:
   `s` (string), `h` (hash?) and `i` (int?) are all read and stored identically
   by `operator>>` (CommandLine.cpp:327). The type char carries zero information.
2. **The size field is also (mostly) redundant.** Elements are delimited by
   whitespace, and elements cannot contain whitespace or the markers `< > [ ]`.
   So `< 3 s A B C >` is unambiguously parseable as `< A B C >`: the parser can
   just read tokens until `>`. The size adds a second source of truth that can
   (and today must be) validated against reality — a mismatch is a silent
   corruption path. `ConvertCommandLineFromCharArray` already IGNORES the type
   token entirely (it parses size then skips one token), so we have two parsers
   with different assumptions about the same field.

**Proposal A (minimal, my recommendation):** drop the type char. Format becomes
`[ < 3 E1 E2 E3 > ... ]`. Save 2 bytes per argument on the wire and one parse
step. Keep the size (cheap, useful for pre-allocation and integrity check).
Parser change is symmetric and small in both `operator>>` and
`ConvertCommandLineFromCharArray`.

**Proposal B (more aggressive):** drop both size and type: `[ < E1 E2 E3 > ]`.
Parsing is a simple token scan to `>`. Removes `StringToInt`-based size logic
and the class of size-mismatch bugs. Cost: cannot pre-allocate (minor; vectors
are small) and loses an early corruption check (but the marker scan itself is a
stronger check). Recommend only if paired with a format version bump.

**Compatibility:** both change the wire format. Mitigation: keep the `Version`
field of the CommandLine (`0.1` → `0.2`) as the format discriminator, and
accept BOTH forms in the parser during a transition window (writers emit the
new form, readers accept old + new). All nodes are our own code; there is no
external third-party parser, so the transition is a rolling-upgrade concern
only (our two VMs).

---

## 2. Two parsers, one format — a bug factory

`CommandLine` currently has **two independent parsers**:

- `operator>>(istringstream&, CommandLine&)` (CommandLine.cpp:289) — used for
  in-memory re-parsing. Full of magic-string guards (`Temp != "<1"`,
  `Temp != "[<"`...) that are classic token-confusion workarounds: they exist
  because the parser reads tokens blindly and can mistake element values for
  markers. If an element value ever equals a marker token, parsing breaks.
- `ConvertCommandLineFromCharArray` (CommandLine.cpp:376) — character-scan with
  a fixed `WhiteSpacePositions[4096]` array (silent overflow if a header line
  exceeds 4096 chars) and its own marker counting. It skips the type token by
  position, assuming exactly one type token — already inconsistent with the
  other parser.

**Recommendation:** delete `operator>>` and keep ONE parser (the char-array
one, hardened), or better: extract a small free function
`bool ParseCommandLine(const char* buf, size_t len, CommandLine& out)` used by
every path (message deserialization, file reload, tests). Single grammar,
single failure mode, unit-testable in isolation. The 4096 cap should be
replaced by dynamic growth or an explicit error return.

---

## 3. Message class: state surface is too large

`Message` carries, simultaneously:

- `CommandLine** CommandLines` + `NoC` (raw pointer array of raw pointers)
- `File PayloadFile`, `File MessageFile` (file-backed representation)
- `char* Payload`, `char* Msg` + `PayloadSize`, `MessageSize` (memory
  representation)
- THREE deletion-control flags (`DeletePayloadArray`, `DeleteMessageArray`,
  `Delete`) plus `InstantiationNumber` (double-delete detector — evidence the
  current design already needed runtime guards) plus `ApplicationDeleted`.

This is the "three representations + manual lifetime" pattern that produced the
use-after-free class of bugs fixed in AMEND-3, and `ResetPayload()` (SPEC-018)
exists precisely because reuse of a Message required hand-resetting 4 fields.

**Recommendations (no wire-format change needed):**

1. **Own the buffers with `std::vector<char>` (or `std::string`)** for
   `Payload`/`Msg`. This removes `DeletePayloadArray`/`DeleteMessageArray`,
   `InstantiationNumber`, `ResetPayload`, and the double-delete guard class —
   the compiler/destructor does it. Copy constructor becomes defaultable.
2. **`CommandLine** CommandLines` → `std::vector<std::unique_ptr<CommandLine>>`
  ** or `std::vector<CommandLine>`. Removes the manual loop-deletes and the
   partial-initialization risk when `NewCommandLine` fails mid-way.
3. **Deduplicate the File-backed path.** If the file-backed representations are
   only used at I/O boundaries (SAR staging, payload cache — SPEC-030), move
   File I/O OUT of Message into free functions
   (`LoadMessageFromFile(path) → Message`, `WritePayloadToCache(msg, dir)`).
   Message becomes a pure in-memory value; File lifecycle stops being the
   Message's problem. This also matches the AIOPT3 model (NRInfoPayload01
   caches payload on disk; message object need not own the File handle).
4. **`short Type`** — check whether it is still semantically used after the
   lifecycle scripts landed. If dispatch is now done on CommandLine
   name/alternative, `Type` may be legacy and removable (or fold into the
   first CL). If it is still needed, keep it but make the enum explicit
   (`enum class MsgType : uint16_t`), not a bare short.
5. **Timing fields** (`Time`, `TimeStamp`, `InstantiationTime`, `Tag`) — group
   into a small `struct MessageMeta` so the hot data (SCN, CLs, payload) stays
   cache-local and the queue-comparison fields are explicit.

---

## 4. MessageBuilder: the combinatorial explosion

~50 methods of the form `NewStoreBindingCommandLineFromXToY(...)` that differ
only in category number and argument order. All reduce to one primitive:

    int NewBind(Message* _M, unsigned _Category,
                const vector<string>& _Args, CommandLine*& _CL);

plus thin inline wrappers (or direct call sites with a named constant per
category). This is mechanical de-duplication with zero wire impact; it cuts the
header by ~2/3 and makes adding a new binding category a one-liner instead of a
copy-paste of 6 lines.

---

## 5. Suggested execution order (each step independently verifiable)

| Step | Change | Risk | Verification |
|---|---|---|---|
| 1 | Drop type char from writer + tolerant reader (accept with/without) | Low | Unit test: round-trip old and new format; full two-VM gate @500 msg/s |
| 2 | Single parser extraction, remove `operator>>` and the 4096 cap | Medium | Unit tests incl. adversarial elements (`<`, `>`, `]` as values → must be rejected or escaped) |
| 3 | Message buffers → vector/RAII, delete deletion flags | Medium | Guard tests 6/6 + photos 1000/1000 byte-exact |
| 4 | MessageBuilder dedup | Low | Compile + existing tests |
| 5 | (Optional, needs version bump) drop size field | Medium | Rolling upgrade across VM101/102 with mixed versions |

Item 2 raises a real design question: **can elements contain whitespace?**
Today, no escaping exists. If the answer must become "yes" someday, the right
moment to add a quoting rule is Step 2 (single parser), not later.

---

## 6. Open questions for review

1. Is any external tool or paper artifact depending on the exact `[ < 1 s X > ]`
   syntax? (Scripts, docs, test fixtures.)
2. Do the lifecycle scripts you implemented assume the size field for lookahead
   (i.e., did any consumer rely on seeking by declared size rather than
   scanning to `>`)?
3. Should the new format be `ng` marker-compatible (same `ng` prefix, `Version`
   bumped) or a new marker (`ng2`) for instant rejection of mixed fleets?
4. Confirm `short Type` usage across all blocks before Step 3 of the Message
   refactor.
