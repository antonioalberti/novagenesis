/*
        NovaGenesis

        Name:		NovaGenesis Adaptation Layer — Convergence Sublayer
        Object:		NGAL_CS
        File:		NGAL_CS.cpp
        Author:		Antonio Marcos Alberti
        Date:		07/2026
        Version:	0.1

        Copyright (C) 2026  Antonio Marcos Alberti

    This work is available under the GNU General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _NGAL_CS_H
#include "NGAL_CS.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _GLIBCXX_UTILITY
#include <utility>
#endif

using namespace std;

// ── DeliverToGateway ──
// Push a completed message's raw serialised buffer to the GW's
// intermediate receive queue (thread-safe).
// The GW thread will later call NewMessage + SetMessageFromCharArray +
// ConvertMessage + PushToInputQueue on the GW thread.
// This avoids the data race on Process::NewMessage().
int NGAL_CS::DeliverToGateway(GW* PGW,
                              char* MessageCharArray,
                              long long MessageSize)
{
  if (PGW == 0 || MessageCharArray == 0 || MessageSize <= 0)
  {
    return 1; // ERROR
  }

  {
    std::lock_guard<std::mutex> lock(PGW->NetworkReceiveQueueMutex);

    // Allocate a copy of the buffer — the caller owns the original
    char* BufferCopy = new char[static_cast<size_t>(MessageSize)];

    for (long long i = 0; i < MessageSize; i++)
    {
      BufferCopy[i] = MessageCharArray[i];
    }

    PGW->NetworkReceiveQueue.push(std::make_pair(BufferCopy, MessageSize));
  }

  PGW->NetworkReceiveQueueCV.notify_one();

  return 0; // OK
}

// ── DeliverToSHM ──
// Write a message to a peer process's SHM (inter-process IPC).
// This is a wrapper that preserves the existing WriteToSharedMemory3 semantics.
// The actual SHM write is handled by the calling code (PG::WriteToSharedMemory3
// or GW::WriteToSharedMemory3) — this is a placeholder for future alignment.
// For now, inter-process SHM IPC continues to use the existing code paths.
int NGAL_CS::DeliverToSHM(File* _PF, char* _MessageCharArray, long long _MessageSize,
                          GW* PGW, int shm_key, size_t MaxSegmentSize)
{
  // TODO: Implement as a wrapper for PG::WriteToSharedMemory3
  // when the NGAL-CS sublayer is fully integrated.
  // For now, inter-process SHM IPC continues via existing code paths.
  return 1; // ERROR — not yet implemented as standalone
}