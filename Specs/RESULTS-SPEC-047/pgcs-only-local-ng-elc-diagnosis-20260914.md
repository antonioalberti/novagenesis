# Local PGCS-only diagnostic — NG-ECL interaction hypothesis

**Date:** 2026-09-14
**Branch:** `AIOPT3`
**Build:** `/tmp/novagenesis-release-v4-build-d303b05`
**Binary:** `PGCS` SHA-256 `c7c4a62e46eee2fd8d5dc17b109725f54ec7c485a774d0d0d67dd2d8c69515fd`
**Mode:** `-lc`

## Question

Determine whether the PGCS crash seen in the four-role NG-ELC local trial occurs when PGCS runs alone or only after the controller launches NRNCS.

## Diagnostic

The exact PGCS executable and equivalent staged `PGCS.ini` were run without NRNCS, Repository or Source:

```text
PGCS <staged-io>/PGCS/ 0 Intra_Domain -lc
```

The process was supervised for 45 seconds. The command returned `124`, meaning the bounded supervisor terminated a still-running PGCS; it did not return a crash status. No stderr was produced. The output reached `State: Operational` and the Core block marker. No PGCS process remained afterwards.

The four IPC segments created by this diagnostic were removed by their exact IDs after process termination and verified absent. No semaphore arrays or PGCS process remained.

## Comparison

The four-role NG-ELC trial recorded:

```text
GW readiness marker → NRNCS launch → PGCS returncode -11
```

The isolated run recorded:

```text
PGCS alone → alive for 45 seconds → controlled timeout 124
```

## Status — invalidated diagnostic

This diagnostic was run without the mandatory privileged project cleanup:

```bash
sudo bash Scripts/Simple/clean.sh
```

The later manual PGCS run demonstrated that stale/shared-memory state can change the result. The 45-second standalone observation therefore cannot distinguish an NG crash from contaminated IPC state and must not be used to implicate the multi-process launch interaction.

The only valid conclusion from this artefact is procedural: repeat the diagnostic after the official privileged clean, verify zero NG processes, zero System V SHM/semaphores/message queues and zero named POSIX semaphores, then compare PGCS-only with the four-role NG-ELC run.

No release acceptance or Astra approval is claimed.
