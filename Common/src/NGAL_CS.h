/*
        NovaGenesis

        Name:		NovaGenesis Adaptation Layer — Convergence Sublayer
        Object:		NGAL_CS
        File:		NGAL_CS.h
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
#define _NGAL_CS_H

#ifndef _QUEUE_H
#include <queue>
#endif

#ifndef _MUTEX
#include <mutex>
#endif

#ifndef _CONDITION_VARIABLE
#include <condition_variable>
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _FILE_H
#include "File.h"
#endif

using namespace std;

class NGAL_CS
{
public:
  // Push a completed message's raw serialised buffer to the GW's
  // intermediate receive queue (thread-safe). The GW thread will
  // later call NewMessage + SetMessageFromCharArray + ConvertMessage + PushToInputQueue.
  // This avoids the data race on Process::NewMessage() that would occur
  // if the receiver thread called NewMessage() directly.
  static int DeliverToGateway(GW* PGW,
                              char* MessageCharArray,
                              long long MessageSize);

  // Write a message to a peer process's SHM (inter-process IPC).
  // Keeps existing WriteToSharedMemory3 semantics.
  // _PF is a File* for log output, wrapped for compatibility.
  static int DeliverToSHM(File* _PF, char* _MessageCharArray, long long _MessageSize,
                          GW* PGW, int shm_key, size_t MaxSegmentSize);
};

#endif