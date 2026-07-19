/*
        NovaGenesis

        Name:		NovaGenesis Adaptation Layer — Segmentation & Reassembly
        Object:		NGAL_SAR
        File:		NGAL_SAR.cpp
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
#include "NGAL_SAR.h"
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _GLIBCXX_CMATH
#include <cmath>
#endif

#ifndef _CTIME_H
#include <ctime>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#define DEBUG

using namespace std;

// ── Constructor / Destructor ──

NGAL_SAR::NGAL_SAR()
{
}

NGAL_SAR::~NGAL_SAR()
{
  for (size_t i = 0; i < ReassemblyBuffers.size(); i++)
  {
    if (ReassemblyBuffers[i] != 0)
    {
      if (ReassemblyBuffers[i]->Buffer != 0)
      {
        delete[] ReassemblyBuffers[i]->Buffer;
      }
      delete ReassemblyBuffers[i];
    }
  }
  ReassemblyBuffers.clear();
}

// ── Static Header Helpers ──

long long NGAL_SAR::OpenHeaderMessageSizeField(unsigned char* _Buffer)
{
  // _Buffer should point to the SizeHeader field (at offset 8 in the PDU)
  // SizeHeader is 8 bytes, big-endian
  long long Size = 0;

  Size = (static_cast<long long>(_Buffer[0]) << (8 * 7)) |
         (static_cast<long long>(_Buffer[1]) << (8 * 6)) |
         (static_cast<long long>(_Buffer[2]) << (8 * 5)) |
         (static_cast<long long>(_Buffer[3]) << (8 * 4)) |
         (static_cast<long long>(_Buffer[4]) << (8 * 3)) |
         (static_cast<long long>(_Buffer[5]) << (8 * 2)) |
         (static_cast<long long>(_Buffer[6]) << (8 * 1)) |
         (static_cast<long long>(_Buffer[7]) << (8 * 0));

  return Size;
}

void NGAL_SAR::OpenHeaderSegmentationField(unsigned char* _Buffer,
                                           unsigned int& _MessageNumber,
                                           unsigned int& _SequenceNumber)
{
  // SegHeader is 8 bytes: MN (4B big-endian) + SN (4B big-endian)
  _MessageNumber = (static_cast<unsigned int>(_Buffer[0]) << (8 * 3)) |
                   (static_cast<unsigned int>(_Buffer[1]) << (8 * 2)) |
                   (static_cast<unsigned int>(_Buffer[2]) << (8 * 1)) |
                   (static_cast<unsigned int>(_Buffer[3]) << (8 * 0));

  _SequenceNumber = (static_cast<unsigned int>(_Buffer[4]) << (8 * 3)) |
                    (static_cast<unsigned int>(_Buffer[5]) << (8 * 2)) |
                    (static_cast<unsigned int>(_Buffer[6]) << (8 * 1)) |
                    (static_cast<unsigned int>(_Buffer[7]) << (8 * 0));
}

void NGAL_SAR::BuildHeaderSizeField(unsigned char* Header, long long TotalSize)
{
  Header[0] = static_cast<unsigned char>((TotalSize >> (8 * 7)) & 0xff);
  Header[1] = static_cast<unsigned char>((TotalSize >> (8 * 6)) & 0xff);
  Header[2] = static_cast<unsigned char>((TotalSize >> (8 * 5)) & 0xff);
  Header[3] = static_cast<unsigned char>((TotalSize >> (8 * 4)) & 0xff);
  Header[4] = static_cast<unsigned char>((TotalSize >> (8 * 3)) & 0xff);
  Header[5] = static_cast<unsigned char>((TotalSize >> (8 * 2)) & 0xff);
  Header[6] = static_cast<unsigned char>((TotalSize >> (8 * 1)) & 0xff);
  Header[7] = static_cast<unsigned char>((TotalSize >> (8 * 0)) & 0xff);
}

void NGAL_SAR::BuildHeaderSegmentationField(unsigned char* Header,
                                            unsigned int MessageNumber,
                                            unsigned int SequenceNumber)
{
  Header[0] = static_cast<unsigned char>((MessageNumber >> (8 * 3)) & 0xff);
  Header[1] = static_cast<unsigned char>((MessageNumber >> (8 * 2)) & 0xff);
  Header[2] = static_cast<unsigned char>((MessageNumber >> (8 * 1)) & 0xff);
  Header[3] = static_cast<unsigned char>((MessageNumber >> (8 * 0)) & 0xff);
  Header[4] = static_cast<unsigned char>((SequenceNumber >> (8 * 3)) & 0xff);
  Header[5] = static_cast<unsigned char>((SequenceNumber >> (8 * 2)) & 0xff);
  Header[6] = static_cast<unsigned char>((SequenceNumber >> (8 * 1)) & 0xff);
  Header[7] = static_cast<unsigned char>((SequenceNumber >> (8 * 0)) & 0xff);
}

// ── Send SAR ──
// Serialise Message → NGAL-PDU fragments.
// Calls transport_callback(data, size, index) for each fragment.
int NGAL_SAR::SendSegmented(Message* M,
                            unsigned int BlockSize,
                            unsigned int& MessageNumber,
                            unsigned int& SequenceNumber,
                            unsigned int& MessageCounter,
                            std::function<int(char*, unsigned int, unsigned int)> transport_callback)
{
  int Status = 1; // ERROR
  long long MessageSize = 0;
  unsigned int NoCL = 0;
  unsigned int EffectiveSize = 0;

  // Serialise the message to its char array representation
  M->ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray();

  if (M->GetMessageSize(MessageSize) == 0) // 0 = OK
  {
    if (MessageSize > 0)
    {
      M->GetNumberofCommandLines(NoCL);

      if (NoCL > 2)
      {
        // Build SDU = SizeHeader(8B) + Payload(MessageSize bytes)
        unsigned char* SDU = new unsigned char[8 + static_cast<size_t>(MessageSize)];
        char* Payload = 0;

        M->GetMessageFromCharArray(Payload);

        // Write the Size Header (MessageSize only, not MessageSize+8)
        // Note (F5): The code writes MessageSize into the Size Header.
        // The receiver reads this value and allocates MessageSize+8 for the full SDU.
        BuildHeaderSizeField(SDU, MessageSize);

        for (int j = 8; j < (8 + static_cast<int>(MessageSize)); j++)
        {
          SDU[j] = static_cast<unsigned char>(Payload[j - 8]);
        }

        unsigned int NoS = static_cast<unsigned int>(ceil(static_cast<double>(8 + MessageSize) / static_cast<double>(BlockSize)));

        // Generate a random MessageNumber for this message
        // Important: needs to be random to avoid conflict with messages numbered on other processes
        MessageNumber = static_cast<unsigned int>(rand() % 4294967289) + 1;
#ifdef DEBUG
        cerr << "[DEBUG] NGAL_SAR::SendSegmented: MN=" << MessageNumber 
             << " NoS=" << NoS << " MessageSize=" << MessageSize 
             << " BlockSize=" << BlockSize << endl;
#endif

        for (unsigned int i = 0; i < NoS; i++)
        {
          unsigned char* DataBlock;
          unsigned int SegmentPayloadSize;

          if (i < (NoS - 1))
          {
            SegmentPayloadSize = BlockSize;
            DataBlock = new unsigned char[SegmentPayloadSize + 8];
          }
          else
          {
            unsigned int Remaining = static_cast<unsigned int>((8 + MessageSize) - (NoS - 1) * BlockSize);
            SegmentPayloadSize = Remaining;
            DataBlock = new unsigned char[SegmentPayloadSize + 8];
          }

          // Build Segmentation Header (8 bytes): MN + SN
          unsigned char HeaderSegmentationField[8];
          BuildHeaderSegmentationField(HeaderSegmentationField, MessageNumber, SequenceNumber);

          for (unsigned int j = 0; j < 8; j++)
          {
            DataBlock[j] = HeaderSegmentationField[j];
          }

          for (unsigned int k = 8; k < SegmentPayloadSize + 8; k++)
          {
            DataBlock[k] = SDU[i * BlockSize + k - 8];
          }

          EffectiveSize = SegmentPayloadSize + 8;

          // Call the transport callback to send this fragment
          int send_status = transport_callback(reinterpret_cast<char*>(DataBlock), EffectiveSize, i);

          if (send_status != 0) // 0 = OK
          {
            Status = 1; // ERROR
            delete[] DataBlock;
            break;
          }

          Status = 0; // OK so far

          SequenceNumber++;
          #ifdef DEBUG
          cerr << "[DEBUG] NGAL_SAR::SendSegmented: MN=" << MessageNumber 
               << " SN=" << SequenceNumber - 1 << "/" << NoS - 1 
               << " frag_size=" << EffectiveSize << endl;
          #endif

          delete[] DataBlock;
          }

        if (Status == 0) // OK
        {
            MessageCounter++;
#ifdef DEBUG
            cerr << "[DEBUG] NGAL_SAR::SendSegmented: COMPLETE MN=" << MessageNumber 
                << " segments_sent=" << NoS << endl;
            cerr << endl; // blank line to separate completed messages in log
#endif
            SequenceNumber = 0;
        }

        delete[] SDU;
      }
    }
  }

  return Status;
}
// ── Receive SAR ──
// Process one frame from transport.
// Manages reassembly buffer internally.
// Returns 0 (OK) when a message is completed, 1 (ERROR) otherwise.
// On completion, CompletedBuffer/CompletedSize are set — CALLER must delete[] CompletedBuffer.
// Does NOT call NewMessage/ConvertMessage — that is the GW's job (Finding F2).
int NGAL_SAR::ReceiveFragment(unsigned char* TempBuffer,
                              unsigned int numbytes,
                              unsigned int BlockSize,
                              char*& CompletedBuffer,
                              long long& CompletedSize)
{
  CompletedBuffer = 0;
  CompletedSize = 0;

  unsigned int MN = 0;
  unsigned int SN = 0;

  // Parse the Segmentation Header (first 8 bytes)
  OpenHeaderSegmentationField(TempBuffer, MN, SN);

  if (MN == 0)
  {
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: INVALID_MN=0 (dropped) numbytes=" << numbytes << endl;
#endif
    return 1; // ERROR — invalid message number
  }

  // Check if this MN already has a fragment buffer
  FragmentBuffer* FB = 0;
  unsigned int BufferIndex = 0;
  bool found = false;

  for (unsigned int c = 0; c < ReassemblyBuffers.size(); c++)
  {
    if (ReassemblyBuffers[c]->MessageNumber == MN)
    {
      found = true;
      FB = ReassemblyBuffers[c];
      BufferIndex = c;
      break;
    }
  }

  if (!found && SN > 0)
  {
    // Fragments received before the first one — drop
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: OUT_OF_ORDER MN=" << MN 
         << " SN=" << SN << " (no buffer yet, dropping)" << endl;
#endif
    return 1;
  }

  if (!found && SN == 0)
  {
    // First fragment of a new message
    // Read the Size Header (bytes 8..15 of the PDU)
    long long MessageSize = OpenHeaderMessageSizeField(TempBuffer + 8);

    if (MessageSize <= 0 || MessageSize >= 10 * 1024 * 1024) // sanity: 10MB max
    {
#ifdef DEBUG
      cerr << "[WARN] NGAL_SAR::ReceiveFragment: INVALID_SIZE MN=" << MN 
           << " MessageSize=" << MessageSize << " (dropping)" << endl;
#endif
      return 1; // Invalid message size
    }

    FB = new FragmentBuffer;
    FB->MessageNumber = MN;
    FB->NoS = static_cast<unsigned int>(ceil(static_cast<double>(8 + MessageSize) / static_cast<double>(BlockSize)));
    FB->MessageSize = MessageSize;
    FB->Buffer = new char[static_cast<size_t>(MessageSize)];
    FB->ReceivedSoFar = 0;
    FB->SegmentsSoFar = 0;
    FB->ContinueReceiving = true;
    FB->Timestamp = static_cast<double>(time(0));

    // Copy payload from this segment (skip SegHeader(8) + SizeHeader(8) = 16 bytes)
    unsigned int payload_bytes = (numbytes > 16) ? (numbytes - 16) : 0;

    for (unsigned int r = 0; r < payload_bytes; r++)
    {
      FB->Buffer[r] = static_cast<char>(TempBuffer[r + 16]);
    }

    FB->ReceivedSoFar = static_cast<long long>(payload_bytes);
    FB->SegmentsSoFar = 1;
    FB->Timestamp = static_cast<double>(time(0));

    ReassemblyBuffers.push_back(FB);
    BufferIndex = static_cast<unsigned int>(ReassemblyBuffers.size() - 1);
#ifdef DEBUG
    cerr << "[DEBUG] NGAL_SAR::ReceiveFragment: NEW MN=" << MN 
         << " NoS=" << FB->NoS << " MessageSize=" << FB->MessageSize 
         << " BlockSize=" << BlockSize << endl;
#endif
  }
  else if (found && SN > 0)
  {
    // Continuing fragment for an existing message
    long long Pointer = static_cast<long long>(SN) * static_cast<long long>(BlockSize) - 8;

    if (Pointer < 0 || Pointer >= FB->MessageSize)
    {
#ifdef DEBUG
      cerr << "[WARN] NGAL_SAR::ReceiveFragment: OUT_OF_BOUNDS MN=" << MN 
           << " SN=" << SN << " Pointer=" << Pointer 
           << " MessageSize=" << FB->MessageSize << " (dropping)" << endl;
#endif
      return 1; // Out of bounds
    }

    unsigned int h = 0;

    for (unsigned int q = 0; q < (numbytes - 8); q++)
    {
      FB->Buffer[Pointer + static_cast<long long>(q)] = static_cast<char>(TempBuffer[q + 8]);

      h++;

      if ((FB->ReceivedSoFar + static_cast<long long>(h)) >= FB->MessageSize)
      {
        break;
      }
    }

    FB->ReceivedSoFar += static_cast<long long>(h);
    FB->SegmentsSoFar++;
    FB->Timestamp = static_cast<double>(time(0));
#ifdef DEBUG
    cerr << "[DEBUG] NGAL_SAR::ReceiveFragment: MN=" << MN 
         << " SN=" << SN << " seg_received=" << FB->SegmentsSoFar 
         << "/" << FB->NoS << " bytes=" << FB->ReceivedSoFar << "/" << FB->MessageSize << endl;
#endif
  }

  // Check stop criteria
  if (found || !found) // check the buffer we just created or updated
  {
    FB = ReassemblyBuffers[BufferIndex];

    if (FB->ReceivedSoFar >= FB->MessageSize && FB->SegmentsSoFar >= FB->NoS)
    {
      FB->ContinueReceiving = false;

      // Transfer ownership of the reassembly buffer to the caller.
      // The caller (NGAL_Transport_RAW::ReceiveDispatcher) will pass it
      // to NGAL_CS::DeliverToGateway, and the GW thread will do
      // NewMessage + SetMessageFromCharArray + ConvertMessage.
      CompletedBuffer = FB->Buffer;
      CompletedSize = FB->MessageSize;

      // Don't delete FB->Buffer — it's now owned by the caller.
      // Only delete the FragmentBuffer struct itself.
      FB->Buffer = 0; // prevent double-free in destructor
      delete FB;
      ReassemblyBuffers.erase(ReassemblyBuffers.begin() + BufferIndex);
      #ifdef DEBUG
      cerr << "[DEBUG] NGAL_SAR::ReceiveFragment: COMPLETE MN=" << MN 
          << " segments=" << FB->SegmentsSoFar << " size=" << FB->MessageSize << endl;
      cerr << endl; // blank line to separate completed messages in log
      #endif

      return 0; // OK — completed message
    }
    else
    {
#ifdef DEBUG
      // Log progress at milestones to reduce noise: every 10 segments or 25%/50%/75%
      unsigned int progress_pct = (FB->NoS > 0) ? (FB->SegmentsSoFar * 100 / FB->NoS) : 0;
      bool log_progress = (FB->SegmentsSoFar % 10 == 0) || 
                          (progress_pct == 25) || (progress_pct == 50) || (progress_pct == 75);
      if (log_progress || FB->SegmentsSoFar == FB->NoS)
      {
        cerr << "[DEBUG] NGAL_SAR::ReceiveFragment: PROGRESS MN=" << MN 
             << " seg=" << FB->SegmentsSoFar << "/" << FB->NoS 
             << " (" << progress_pct << "%) bytes=" << FB->ReceivedSoFar << "/" << FB->MessageSize << endl;
      }
#endif
      return 1; // ERROR — more fragments needed
    }
  }

  return 1; // ERROR
}

// ── Timeout Cleanup ──
void NGAL_SAR::CleanupTimedOut(double timeout_threshold)
{
  for (size_t i = 0; i < ReassemblyBuffers.size();)
  {
    FragmentBuffer* FB = ReassemblyBuffers[i];

    if (FB != 0 && !FB->ContinueReceiving)
    {
      // Already completed — should have been removed. Clean up.
#ifdef DEBUG
      cerr << "[WARN] NGAL_SAR::CleanupTimedOut: STALE buffer MN=" << FB->MessageNumber 
           << " (completed but not removed) - cleaning up" << endl;
#endif
      delete[] FB->Buffer;
      delete FB;
      ReassemblyBuffers.erase(ReassemblyBuffers.begin() + static_cast<long>(i));
    }
    else if (FB != 0 && FB->Timestamp > 0 && (timeout_threshold - FB->Timestamp) > 30.0)
    {
      // Timeout after 30 seconds
#ifdef DEBUG
      cerr << "[ERROR] NGAL_SAR::CleanupTimedOut: TIMEOUT MN=" << FB->MessageNumber 
           << " seg_received=" << FB->SegmentsSoFar << "/" << FB->NoS 
           << " bytes=" << FB->ReceivedSoFar << "/" << FB->MessageSize 
           << " age=" << (timeout_threshold - FB->Timestamp) << "s - discarding" << endl;
#endif
      delete[] FB->Buffer;
      delete FB;
      ReassemblyBuffers.erase(ReassemblyBuffers.begin() + static_cast<long>(i));
    }
    else
    {
      i++;
    }
  }
}