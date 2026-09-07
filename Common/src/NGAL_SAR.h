/*
        NovaGenesis

        Name:           NovaGenesis Adaptation Layer — Segmentation & Reassembly
        Object:         NGAL_SAR
        File:           NGAL_SAR.h
        Author:         Antonio Marcos Alberti
        Date:           07/2026
        Version:        0.1

        Copyright (C) 2026 Antonio Marcos Alberti

    This work is available under the GNU General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _NGAL_SAR_H
#define _NGAL_SAR_H

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Message;

class NGAL_SAR
{
public:
  NGAL_SAR();
  ~NGAL_SAR();

  NGAL_SAR(const NGAL_SAR&) = delete;
  NGAL_SAR& operator=(const NGAL_SAR&) = delete;

  // Callback receives (fragment data, fragment size, fragment index).
  int SendSegmented(Message* M,
                    unsigned int BlockSize,
                    unsigned int& MessageNumber,
                    unsigned int& SequenceNumber,
                    unsigned int& MessageCounter,
                    std::function<int(char*, unsigned int, unsigned int)> transport_callback);

  // Returns 0 only on completion; otherwise returns 1.
  // Outputs are reset on entry. On completion the caller owns CompletedBuffer
  // and must eventually delete[] it. No NewMessage/ConvertMessage is performed.
  //
  // Synchronization is unchanged: no internal mutex is acquired. When shared,
  // ReceiveFragment and CleanupTimedOut must use the SAME external SAR mutex.
  // Destruction requires all users to have stopped.
  int ReceiveFragment(unsigned char* TempBuffer,
                      unsigned int numbytes,
                      unsigned int BlockSize,
                      char*& CompletedBuffer,
                      long long& CompletedSize);

  // Helpers require a valid pointer to at least eight bytes.
  static long long OpenHeaderMessageSizeField(unsigned char* _Buffer);
  static void OpenHeaderSegmentationField(unsigned char* _Buffer,
                                          unsigned int& _MessageNumber,
                                          unsigned int& _SequenceNumber);
  static void BuildHeaderSizeField(unsigned char* Header, long long TotalSize);
  static void BuildHeaderSegmentationField(unsigned char* Header,
                                           unsigned int MessageNumber,
                                           unsigned int SequenceNumber);

  // timeout_threshold is the current absolute time in seconds, not an age.
  void CleanupTimedOut(double timeout_threshold);

private:
  struct FragmentBuffer
  {
    unsigned int MessageNumber;
    unsigned int NoS;
    long long MessageSize;
    long long ReceivedSoFar;
    unsigned int SegmentsSoFar;
    char* Buffer;
    double Timestamp;
    bool ContinueReceiving;
    unsigned int BlockSize;
    std::vector<bool> ReceivedSN;
    size_t ChargedBytes;

    FragmentBuffer()
      : MessageNumber(0), NoS(0), MessageSize(0), ReceivedSoFar(0),
        SegmentsSoFar(0), Buffer(0), Timestamp(0), ContinueReceiving(true),
        BlockSize(0), ChargedBytes(0)
    {
    }

    ~FragmentBuffer()
    {
      delete[] Buffer;
    }

    FragmentBuffer(const FragmentBuffer&) = delete;
    FragmentBuffer& operator=(const FragmentBuffer&) = delete;
  };

  std::vector<FragmentBuffer*> ReassemblyBuffers;
  size_t ReassemblyBytes;

  static const size_t MAX_REASSEMBLY_BUFFERS = 256;
  // Charge payload, one byte per bitmap entry (conservative for vector<bool>),
  // object size and 256 bytes of per-buffer allocation/rounding allowance.
  static const size_t MAX_REASSEMBLY_BYTES = 384ULL * 1024 * 1024;

  void RemoveBuffer(size_t index);
  static void LogProgress(const FragmentBuffer* FB);
};

#endif
