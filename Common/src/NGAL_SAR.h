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

#ifndef _PROCESS_H
#include "Process.h"
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

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _MESSAGERECEIVING_H
#include "MessageReceiving.h"
#endif

using namespace std;

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
  // Returns completed Message* via output parameter when reassembly finishes.
  // Returns nullptr if more fragments needed.
  // The caller owns the returned Message* if non-null.
  int ReceiveFragment(unsigned char* TempBuffer,
                      unsigned int numbytes,
                      unsigned int BlockSize,
                      Process* PP,
                      Message*& CompletedMessage);

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
  };

  std::vector<FragmentBuffer*> ReassemblyBuffers;
};

#endif