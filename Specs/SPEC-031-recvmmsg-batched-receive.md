# SPEC-031 — Batched RAW Receive with `recvmmsg()`

**Status:** Draft — approval required before implementation
**Scope:** One performance change: receive-side syscall batching.
**Sequence:** User approves → implementation → Hermes verifies → Hermes benchmarks.

## 1. Context and non-goals

- Phase 1 signed off; SPEC-028-B heartbeat and SPEC-030 verified.
- Reference: 500 msg/s bidirectional, measured zero loss, one-way delay 0.13/0.64 ms, RSS ~2.8 MB, PGCS ~1 busy core per VM.
- Current path: `NGAL_Transport_RAW::ReceiveDispatcher` polls the SSID collection, calls one `recvfrom()` per ready socket, strips the 14-byte Ethernet header, calls `SAR.ReceiveFragment`, and invokes `DeliverToGateway` on completion.
- Change only reception mechanics. Preserve framing, SAR, delivery, socket configuration, and wire format.
- No send batching, affinity changes, socket-buffer tuning, SAR changes, or NRNCS stale-cache fix in this benchmark.

## 2. Exact change design

### Placement and allocation

Replace the ready-socket `recvfrom()` operation inside `ReceiveDispatcher` with `recvmmsg()`.

- Batch size: **32 frames** per call.
- Allocate one reusable dispatcher-owned pool at initialization, shared serially across ready sockets.
- Pool contains 32 payload buffers, 32 `iovec`, 32 `mmsghdr`, and 32 `sockaddr_ll`.
- Each payload buffer has capacity **C**, equal to the current receive buffer capacity; document C before implementation approval.
- No per-packet allocation or pool growth. Initialization failure fails dispatcher startup explicitly.
- Release the pool at dispatcher teardown.
- Consume returned frames synchronously, in vector order, before pool reuse.
- Verify that SAR/delivery does not retain pointers into receive buffers; preserve existing ownership/copy semantics.

### Call and reset rules

Use:

```c
recvmmsg(fd, msgs, 32, MSG_DONTWAIT | MSG_TRUNC, NULL);
```

Before **every** call, reinitialize all 32 slots:

- Zero each `sockaddr_ll`, including `sll_family`, `sll_protocol`, `sll_ifindex`, `sll_hatype`, `sll_pkttype`, `sll_halen`, and `sll_addr`.
- Set `msg_name` to that slot's address and `msg_namelen = sizeof(struct sockaddr_ll)`.
- Restore `iov_base`, `iov_len = C`, `msg_iov`, and `msg_iovlen = 1`.
- Set `msg_control = NULL`, `msg_controllen = 0`, `msg_flags = 0`, and `msg_len = 0`.
- Consume only slots below the returned count; treat returned address fields as kernel output.
- Preserve existing address/protocol filters; validate returned address length before field access.

### Frame validation and processing

- Linux `MSG_TRUNC` may return the original packet length, exceeding C.
- Reject any slot with output `MSG_TRUNC` or `msg_len > C`; never parse its partial payload.
- Reject frames shorter than 14 bytes before Ethernet-header stripping.
- Route these rejects through existing receive-drop accounting, once per rejected frame; no new heartbeat schema.
- Otherwise pass exactly `msg_len - 14` bytes through the unchanged SAR/delivery path.
- Preserve receive ordering within each socket; no cross-socket ordering guarantee is added.

### Drain policy and fairness

- For each ready socket, attempt up to **two calls per poll cycle**: at most **64 frames**.
- Continue after a positive return, including a short batch, until `EAGAIN/EWOULDBLOCK` or the budget is exhausted.
- `EAGAIN/EWOULDBLOCK`: normal drain completion; not an error or drop.
- Zero return: stop servicing that socket for this cycle.
- `EINTR`: retry only within the two-call attempt budget.
- Other errors: preserve existing error handling; stop this socket's current turn.
- Service every ready socket once per cycle, rotating the starting position across cycles.
- Budget exhaustion returns control to the dispatcher; level-triggered `poll()` revisits unread data.
- Check existing shutdown/control conditions between socket turns.
- No blocking timeout and no `MSG_WAITFORONE`; a single frame is processed immediately.

## 3. Benchmark protocol

1. Hermes captures BEFORE sources, MTU, SSID count, socket options, payload distribution, and SAR fragmentation profile.
2. Freeze these settings, topology, generator, background load, and measurement tools for AFTER.
3. Confirm whether "500 msg/s bidirectional" means aggregate or per direction; record both per-direction offered rates.
4. Test 500 and **2000 msg/s using that same rate convention**, with unchanged direction split.
5. For each rate/build: 60 s warm-up, 300 s measurement, stop offering, then allow up to 60 s for drain.
6. Run three untraced repetitions per rate/build; use the same sequence and seeds.
7. Capture every 10 s `SPEC028_STATS` line, including all seven counters and all 14 SAR metrics.
8. Record per-direction offered/received/completed rates, one-way latency distribution, PGCS/dispatcher CPU, RSS peak/steady state, and relevant kernel/interface drops.
9. Use the same synchronized-clock latency method as BEFORE; report clock uncertainty.
10. Capture separate matched syscall-profile runs with `strace -f -c` covering `recvfrom`, `recvmmsg`, and `poll`; tracing results are not latency/throughput results.
11. Normalize receive syscall counts by successfully dequeued Ethernet frames, using identical known fragmentation or independent ingress-frame counts.
12. Include unsuccessful receive calls in the syscall total. Ordinary `/proc` process/I/O counters are not a per-syscall substitute.

## 4. Acceptance criteria

- **Syscalls:** At 2000 msg/s, at least **20% fewer receive syscalls per dequeued frame** than BEFORE. Report batch occupancy and the 500 msg/s reduction separately.
- Sparse arrivals may prevent batching; missing the reduction threshold means the optimization is not accepted on this workload.
- **Throughput:** At both rates, sustain the requested offered rate and complete all offered messages after drain, in every repetition.
- AFTER steady-state completion throughput must be no worse than BEFORE within 1% measurement tolerance, with no growing outstanding backlog.
- If BEFORE cannot sustain 2000 msg/s, retain that result; AFTER must still meet the absolute 2000 msg/s target.
- **Zero loss:** Reconcile SPEC028 counter deltas per direction using their established semantics and workload mapping.
- Require no unexplained offered/send/receive/completion deficit, no new `dropped` or `guard_reject`, and outstanding returning to its pre-run value after drain.
- Require no new SAR loss/timeout/error indications or kernel/interface drops; counter resets invalidate a run.
- **Latency:** Compare like-for-like percentiles; no p99 regression greater than `max(10% of BEFORE p99, 0.10 ms)`.
- **RSS:** Peak increase over BEFORE must not exceed the documented pool footprint plus **1 MiB** allocator allowance; no sustained growth after warm-up.
- Hermes also verifies truncation, short frames, address reset, shutdown responsiveness, and multi-socket fairness outside measured runs.

## 5. musl / Alpine compatibility

- Linux-specific API; musl on Alpine provides `recvmmsg()` with `<sys/socket.h>` and `_GNU_SOURCE` enabled before system headers.
- Use `<linux/if_packet.h>` for `sockaddr_ll`; verify the target Alpine toolchain builds the declaration.
- Verify runtime support on the actual kernel and RAW socket type; container seccomp may deny the syscall.
- `MSG_DONTWAIT` supplies per-call nonblocking behavior; retain existing descriptor flags.
- No changes to `SO_RCVBUF`, `SO_RCVLOWAT`, packet membership, promiscuous mode, or other socket options.
- No silent fallback: unsupported or denied syscall is a deployment failure requiring rollback.

## 6. Rollback and release gate

- Keep this implementation in one isolated commit; archive the BEFORE binary and configuration.
- Roll back on loss, fairness failure, crash, unsupported syscall, memory breach, or failed acceptance gate.
- Restore the BEFORE binary or revert the commit, restart using the same configuration, and repeat the 500 msg/s zero-loss check.
- Hermes publishes BEFORE/AFTER evidence and a pass/fail table. Deployment proceeds only after verification and benchmark acceptance.
