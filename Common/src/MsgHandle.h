/*
        NovaGenesis — SPEC-033 Phase B

        Name:           MsgHandle
        Object:         Generational message handle + queue entry (shared)
        File:           MsgHandle.h
        Author:         Hermes Agent
        Date:           2026-09-08

        Lives outside Process to break the Process.h <-> Block.h include
        cycle (Process.h -> GW.h -> Block.h). Process, Block and GW all use
        these types.
*/

#ifndef _MSGHANDLE_H
#define _MSGHANDLE_H

#include <cstdint>

// Generational handle to a Process-owned message slot. Slot 0xFFFFFFFF is
// the invalid sentinel; Valid() means "not the sentinel", NOT "resolves" —
// resolution must still check bounds, occupancy and generation.
struct MsgHandle
{
  uint32_t Slot = UINT32_MAX;
  uint32_t Generation = 0;

  bool Valid() const { return Slot != UINT32_MAX; }
};

// Queue entry with COPIED sort keys: GW priority queues sort on (Time, Tag)
// from the entry — the comparator never dereferences the Message.
struct QEntry
{
  double Time = 0.0;
  uint32_t Tag = 0;
  MsgHandle H;
};

#endif
