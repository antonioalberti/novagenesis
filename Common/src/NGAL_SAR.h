/*
        NovaGenesis

        Name:		NovaGenesis Adaptation Layer — Segmentation & Reassembly
        Object:		NGAL_SAR
        File:		NGAL_SAR.h
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

#ifndef _NGAL_SAR_H
#define _NGAL_SAR_H

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _GLIBCXX_FUNCTIONAL
#include <functional>
#endif

#ifndef _MATH_H
#include <math.h>
#endif

#ifndef _STRING_H
#include <string>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _GLIBCXX_IOMANIP
#include <iomanip>
#endif

#ifndef _CTYPE_H
#include <cstdlib>
#endif

using namespace std;

// Forward declaration — Message is fully defined in Message.h (included in .cpp)
class Message;

class NGAL_SAR
{
public:
  // ── Constructor / Destructor ──
  NGAL_SAR();
  ~NGAL_SAR();

  // ── Send SAR ──
  // Serialise Message → NGAL-PDU fragments.
  // Calls transport_callback for each fragment.
  // transport_callback receives: (char* fragment_data, unsigned int fragment_size, unsigned int fragment_index)
  int SendSegmented(Message* M,
                    unsigned int BlockSize,
                    unsigned int& MessageNumber,
                    unsigned int& SequenceNumber,
                    unsigned int& MessageCounter,
                    std::function<int(char*, unsigned int, unsigned int)> transport_callback);

  // ── Receive SAR ──
  // Process one frame from transport. Manages reassembly buffer internally.
  // Returns the reassembled char buffer and its size via output parameters
  // when reassembly finishes. The CALLER owns the returned buffer (must delete[] it).
  // Returns 0 (OK) when a message is completed, 1 (ERROR) otherwise.
  // This function does NOT call NewMessage/ConvertMessage — that is the GW's job.
  int ReceiveFragment(unsigned char* TempBuffer,
                      unsigned int numbytes,
                      unsigned int BlockSize,
                      char*& CompletedBuffer,
                      long long& CompletedSize);

  // ── Header helpers (static) ──
  static long long OpenHeaderMessageSizeField(unsigned char* _Buffer);
  static void OpenHeaderSegmentationField(unsigned char* _Buffer,
                                          unsigned int& _MessageNumber,
                                          unsigned int& _SequenceNumber);
  static void BuildHeaderSizeField(unsigned char* Header, long long TotalSize);
  static void BuildHeaderSegmentationField(unsigned char* Header,
                                           unsigned int MessageNumber,
                                           unsigned int SequenceNumber);

  // ── Timeout cleanup (public — called from transport layer) ──
  void CleanupTimedOut(double timeout_threshold);

private:
  struct FragmentBuffer
  {
    unsigned int MessageNumber;   // MN — message identifier
    unsigned int NoS;             // Expected segments count
    long long int MessageSize;    // Full message size (from header)
    long long int ReceivedSoFar;  // Bytes received
    unsigned int SegmentsSoFar;   // Segments received
    char* Buffer;                 // Reassembly buffer
    double Timestamp;             // For timeout cleanup
    bool ContinueReceiving;       // True = more segments needed
    unsigned int BlockSize;       // BlockSize used when this buffer was created (AMEND-3)
    std::vector<bool> ReceivedSN; // Per-SN received bitmap — count/copy each SN once (AMEND-3)
  };

  std::vector<FragmentBuffer*> ReassemblyBuffers;

  // Maximum simultaneous reassembly buffers (AMEND-3: unbounded growth caused
  // PGCS crashes under high load; MN collisions/timeouts leak buffers).
  static const size_t MAX_REASSEMBLY_BUFFERS = 256;
};

#endif