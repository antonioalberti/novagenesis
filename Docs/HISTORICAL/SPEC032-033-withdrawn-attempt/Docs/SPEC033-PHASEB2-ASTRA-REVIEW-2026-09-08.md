**B-3’s ownership model is sound, but I would make proceeding conditional on two lifecycle checks below.** This is a design review of your summary, not a source-level verification of the commits or harness results.

The allocator sentinel fix and mixed-API tests are particularly valuable. Centralizing slot mutation is the right foundation; now the key is ensuring every reclamation path obeys the same retention rules.

## Checks before queue migration

### 1. Does `EraseMessage` respect retention?

Your summary explicitly lists retention checks for `DeleteMessage`, `DeleteMessages`, and `DeleteMarkedMessages`, but not `EraseMessage`.

**Every destruction path—including pointer erase, handle erase, and any direct `FreeSlot` caller—must refuse reclamation while `Retentions > 0`.** Ideally, enforce this at the common reclaim boundary, under `LifecycleMutex`, rather than depending on each caller to check correctly.

Likewise, validation, retention testing, slot removal, generation advancement, and free-list updates must be serialized against `TryRetain`. A check under the mutex followed by unprotected reclamation is insufficient.

If this is already true, consider this check satisfied.

### 2. Is `Run` safe before retention is acquired?

You describe:

```text
OkToRun -> FindHandle(pointer) -> TryRetain(handle)
```

Two things need checking:

- If `OkToRun` dereferences the message, it must run **after** lifetime protection is acquired.
- `FindHandle(pointer)` cannot distinguish an old pointer from a new message allocated at the same address. `FindHandle` followed by `TryRetain` protects the identity that was found, not necessarily the identity originally intended by the caller.

The queued path should therefore enter execution with the **original handle and an already-owned retention**, not rediscover identity from a raw pointer. Legacy raw-pointer entry needs an explicit caller lifetime guarantee; pointer lookup alone cannot provide that guarantee.

## Q1 — Proceed with `QEntry`?

**Yes, with the following refinements.**

### (a) Enqueue acquires one retention

Agreed for a *new* enqueue:

1. `TryRetain(H)`.
2. Copy the scheduling key under the applicable metadata synchronization.
3. Insert the entry.
4. On insertion failure, release the newly acquired retention.

But change:

> If retention fails, the message is marked and NOT queued.

to:

> If retention fails, return failure and do not queue or dereference the message.

A failed retain means you have no protected message to mark. Marking is permissible only if the caller independently owns a valid retention.

Enqueue needs an explicit success/failure outcome and a precise ownership contract, including allocation failure during `priority_queue::push`.

### (b) Pop transfers ownership

Agreed. Removing the entry transfers its existing retention to the worker; there must be no intermediate `Release`.

### (c) Retry transfers ownership back

Agreed. Treat this as a distinct operation from fresh enqueue:

- **Success:** worker relinquishes its retention to the queue.
- **Failure:** worker still owns the retention and must finish or release it.

Do not run the ordinary acquire-on-enqueue path and accidentally add a second count.

### (d) Comparator uses copied keys

Agreed. Preserve the existing priority direction and tie behavior deliberately—`priority_queue` comparator direction is easy to invert. Test equal-time ordering and whether `Tag` actually provides the intended tie-break.

### (e) Stale entry is an invariant violation

Agreed: assert in debug; loudly diagnose and discard without dereferencing in production.

Do not attempt to “repair” it by resolving or releasing the current occupant of the slot. A generation mismatch must never affect that replacement message. Discarding is defensive containment, not proof that ownership accounting remains healthy.

## Q2 — Assert on `Retentions > 0` in setters?

**No—not as the queue-immutability enforcement mechanism.**

Retention means “must remain alive,” not “is queued.” A worker legitimately retains a popped message and may need to update its time before retrying. Your Discovery steps may also execute while `Run` holds a retention. Thus:

```cpp
assert(Retentions == 0);
```

would conflate lifetime protection with scheduling-state protection.

Choose one explicit model:

- **Snapshot semantics:** the copied `QEntry` key determines scheduling until removal. Later message changes do not reschedule an existing entry.
- **Queued-message immutability:** additionally track queue residence or scheduling ownership and reject key mutation while queued.

For B-3, I recommend **documented snapshot semantics plus an audit of `SetTime`/`SetTag` and synchronization of key capture**. If strict queued-message immutability is required, enforce it using queue-residence state, not total retention count.

Copied keys protect the heap ordering; they do **not** by themselves eliminate data races while copying mutable fields.

## Q3 — Batch pop → `Run`

**Yes, there is no lifetime gap if ownership is transferred as described.** All messages removed into the local batch remain retained—including those waiting while earlier batch messages execute.

However, your current `Run` implementation acquires its own retention. That gives two valid implementation choices:

| Execution model | Retention behavior |
|---|---|
| Keep current `Run` | Worker owns transferred queue retention; `Run` temporarily acquires another. Worker releases its count after `Run`. |
| Add an owned-retention execution path | `Run` consumes/adopts the worker’s existing retention and releases or transfers it exactly once. |

Both are safe. **Do not implement a hybrid that assumes `Run` released the transferred count when it actually released only its additional count.** An RAII retention owner would make these paths substantially easier to audit.

For normal completion, release the worker/execution ownership **before** the intended reclamation pass. Otherwise a marked message will be skipped by that pass and needs a guaranteed later sweep.

## Q4 — Remaining conditions and tests

Beyond the two lifecycle checks, make these part of B-3’s acceptance criteria:

- **Queue drain/shutdown:** release one retention per discarded entry.
- **All worker exits:** errors, exceptions where applicable, early returns, and unprocessed batch entries release or transfer ownership exactly once.
- **No queue-lock execution:** `Run` and actions execute outside queue mutexes.
- **Explicit lock ordering:** audit both queue → lifecycle and lifecycle → queue paths; avoid inversions.
- **Output queues:** give dequeue/send/retry/drop the same ownership accounting as input processing.
- **Duplicate enqueue policy:** reject duplicates or support one retention per entry explicitly. Retention alone does not prevent simultaneous execution of the same message.

Add focused tests for retained erase rejection, deletion while queued, batch-pop deletion attempts, retry without count growth, insertion failure rollback, and shutdown drain.

**Verdict: conditional go for B-3.** Confirm universal retention-aware reclamation and safe `Run` entry first. Then proceed with `QEntry`, explicit ownership-transfer APIs, and scheduling-key rules independent of lifetime retention.