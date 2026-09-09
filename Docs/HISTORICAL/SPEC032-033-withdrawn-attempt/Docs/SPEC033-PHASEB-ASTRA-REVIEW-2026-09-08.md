Based on the description—not a review of the commits themselves—I agree with keeping exhaustion behavior stable and staging the migration. However, I would **not proceed to B-3 until mixed-API slot bookkeeping and the synchronization/retention contract are settled**. Those are more significant than the sentinel choice.

## D1. Exhaustion: keep the current contract

**Choose (a) now:** return `ERROR`, leave `H` invalid, and leave the container unchanged.

Backpressure is a separate admission-control policy, not a replacement for allocation failure handling. Even with queue-depth thresholds, allocation must safely fail: queue depth does not account for every live or retained message.

Add:

- An allocation-failure counter and occupancy high-water mark.
- Rate-limited logging/alarming, rather than one log per rejected message.
- Explicit caller handling: no resolve, enqueue, or partial publication after failure.
- A documented policy for `MessageCounter` on failure. Assuming it counts successful creations, it and `NoM` must remain unchanged.

If initialization can fail after a slot is reserved, rollback must restore the slot and counters without exposing a live handle.

## D2. Queue migration: a separate B-2 is reasonable, but retention comes first

**Yes to a distinct B-2; no to treating a narrow UAF window as a safety argument.**

Implement the retention mechanism and safe acquisition primitives before—or atomically with—the queue migration. Threading retention through `Run` alone is insufficient: **queue residence is also retention** under your target contract.

The ownership/retention sequence should be:

```text
Process owns message
    ↓
queue holds one retention
    ↓ pop
worker receives that retention, without a release/acquire gap
    ↓
Run / output processing
    ↓
release, or transfer to another queue on retry/enqueue
```

A queue can hold a retention logically through its entry even before Phase E gives that behavior an RAII wrapper. Every enqueue failure, cancellation, queue clear, pop, and retry must balance it.

For the output retry path:

- Keep the worker retention across the SHM attempt.
- On busy, transfer that retention back to the queue.
- Do not release and then try to reacquire by resolving the handle.

**An entry with an active retention should not become stale during normal operation.** Skipping stale entries is valuable defense, but it is not T5 protection. If a legitimately retained retry handle no longer resolves, that indicates a lifecycle bug, except during a specifically coordinated teardown.

Also, copied keys require a decision: queue ordering is based on the **enqueue-time snapshot**. If queued messages can change `Time` or `Tag`, either prohibit that change or require removal/reinsertion.

## D3. Retention accounting: the stated reasoning does not establish thread safety

I cannot confirm an actual race path without the code, but **“only one thread calls `DeleteMarkedMessages`” is not enough**.

The relevant question is whether any other thread can concurrently:

- Acquire or release retention.
- Mark a message.
- Allocate, erase, or resolve a slot.
- Access a message while reclamation scans the container.

Threads working on different messages still share the free-list, counters, and potentially a scanner that examines all slots. Also, “the GW Run thread never marks those messages” does not prove that it cannot inspect or reclaim them.

The fundamental race is:

```text
Thread A: Resolve(H) returns Message*
Thread B: sees Retentions == 0; destroys Message
Thread A: increments Retentions or accesses Message
```

Making `Retentions` atomic alone does not fix this.

**Recommended first implementation: one Process lifecycle mutex**, protecting slot bookkeeping, generation validation, retention acquisition/release, marking, and the reclamation decision. Provide an operation conceptually like:

```cpp
TryRetain(H) // validates handle and increments retention atomically with respect to erase
Release(H)
```

After successful retention, the caller can use the object outside that mutex until release. That protects lifetime, not arbitrary concurrent mutation of message contents.

`ResolveMessage(H)` by itself should be documented as a borrowed lookup—not a cross-thread lifetime guarantee.

An `InputQueueMutex` works only if **every** relevant lifecycle operation uses it, including output operations; a mutex used only inside `DeleteMarkedMessages` does not work. A dedicated Process mutex is clearer. Do not hold it across `Run` or queue operations; establish a lock-order policy.

A lock-free/single-thread-affine alternative is valid only if lifecycle operations really are confined to one thread and other threads submit commands to it. An assertion can enforce that architecture, but a metric cannot substitute for synchronization.

Finally:

- Check retention underflow and overflow.
- Ordinary erasure must respect active retention.
- Shutdown may override retention **only after users/workers are quiesced**. “Shutdown destroys everything” must not mean destroying objects while another thread is using them.

## D4. Fix the comment; check disposition, not necessarily enqueue

**Fix the incorrect comment now. Add a debug invariant, but broaden it.**

“Every scheduled message was pushed to the input queue” is stronger than the contract you quoted. A legitimate outcome might instead be cancellation/deletion-request plus release, or an explicitly authorized live-but-unretained state.

The useful invariant is:

> Every scheduled retention has a recorded disposition before `Run` exits: transferred to a retaining destination, or released through a defined completion/failure path.

Process ownership is always present; merely saying “left to the Process owner” does not explain how that message will eventually be reclaimed.

If the intended semantics genuinely require every scheduled message to be enqueued, assert that stronger rule—but make it explicit and cover failed enqueue paths. Checking only whether `PushToInputQueue` was called is insufficient: **the push must succeed and the retention transfer must complete**.

## D5. Changes and bugs to address before B-3

### 1. Mixed pointer/handle bookkeeping is the biggest potential integration bug

You say the legacy APIs are “untouched.” If that means their implementations still independently manipulate the slot array, the integration is unsafe even before call sites migrate.

For example:

- Legacy `NewMessage` fills slot 0.
- The new free-list still offers slot 0.
- `NewMessageHandle` allocates the already-occupied slot.

Conversely, legacy deletion can leave the free-list or generation stale, allowing handle resurrection or capacity leaks.

**Both APIs must delegate to one allocation/reclamation implementation.** Every successful destruction must invalidate the generation, update `NoM`, and return the slot exactly once, regardless of which API initiated it.

Legacy `GetMessage` can remain a lookup, but legacy allocation and deletion cannot retain independent bookkeeping. If “untouched” means only unchanged signatures and callers, with shared internals already in place, this concern is covered.

### 2. LIFO does not preserve lowest-free-index allocation generally

Descending initialization gives `0, 1, 2, ...` on initial allocation. It does **not** match a lowest-index linear scan after arbitrary frees.

Example:

```text
free slot 2
free slot 7
next LIFO allocation → 7
old lowest-free scan → 2
```

I would keep the O(1) free-list and remove the stronger equivalence claim, unless some actual behavior depends on lowest-index reuse.

### 3. Clarify ordinary erasure versus forced shutdown destruction

If `EraseMessageHandle(H)` immediately destroys a retained message, it violates the target contract.

Either make ordinary erase fail/defer when retained, or keep raw destruction private behind the reclamation policy. Forced teardown should be a separate, quiesced path. Audit legacy `DeleteMessage` and `EraseMessage` for the same bypass.

### 4. The sentinel is fine

**Keep an invalid-slot sentinel; no dedicated `bool` is needed.**

Prefer explicit-width types if portability matters:

```cpp
struct MsgHandle {
    uint32_t Slot = UINT32_MAX;
    uint32_t Generation = 0;

    bool Valid() const { return Slot != UINT32_MAX; }
};
```

`Valid()` means “not the invalid sentinel,” not “currently resolves.” Resolution must still check bounds, occupancy, and generation. Add a compile-time capacity check.

Generation 0 is fine. Generation wrap is the caveat: 32-bit generations provide ABA prevention only until wraparound. Document the limitation, use 64-bit generations for practical headroom, or retire slots before wrap if strict prevention is required.

### 5. Extend tests against the actual Process implementation

The standalone harness establishes the model, but it can miss integration drift. Before B-3, add coverage for:

- Interleaved legacy and handle allocation/deletion.
- Legacy deletion invalidating a previously issued handle.
- Duplicate/stale erase leaving counters and free-list unchanged.
- Retained erase rejection/deferment.
- Queue → worker → retry transfer, including enqueue failure.
- Every early/error exit from `Run` releasing its retentions.
- Concurrent retain-versus-reclaim, using the chosen synchronization protocol.
- Shutdown after worker quiescence.
- Exact comparator direction and tie behavior; reject or explicitly order NaN timestamps if they can occur.

**Bottom line:** keep D1 and the sentinel, stage B-2, and fix the comment. The blockers are ensuring one shared allocator/reclaimer behind both APIs, making queue residence count as retention, and making retention acquisition atomic with respect to destruction. Copied sort keys solve comparator dereferences; handles alone do not solve lifetime races.