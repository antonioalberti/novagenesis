/*
        NovaGenesis

        Name:           NovaGenesis Adaptation Layer — Segmentation & Reassembly
        Object:         NGAL_SAR
        File:           NGAL_SAR.cpp
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

#include "NGAL_SAR.h"
#include "Message.h"
#include "Process.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <limits>
#include <memory>
#include <new>

// #define DEBUG

using namespace std;

const size_t NGAL_SAR::MAX_REASSEMBLY_BUFFERS;
const size_t NGAL_SAR::MAX_REASSEMBLY_BYTES;

NGAL_SAR::Telemetry& NGAL_SAR::Stats()
{
  // C++11 initialization is thread-safe; no PG-owned pointer escapes to SAR.
  static Telemetry counters;
  return counters;
}

NGAL_SAR::NGAL_SAR() : ReassemblyBytes(0)
{
}

NGAL_SAR::~NGAL_SAR()
{
  for (size_t i = 0; i < ReassemblyBuffers.size(); ++i)
    delete ReassemblyBuffers[i];
}

void NGAL_SAR::RemoveBuffer(size_t index)
{
  FragmentBuffer* FB = ReassemblyBuffers[index];
  ReassemblyBytes -= FB->ChargedBytes;
  delete FB;
  ReassemblyBuffers.erase(ReassemblyBuffers.begin() + index);
}

long long NGAL_SAR::OpenHeaderMessageSizeField(unsigned char* _Buffer)
{
  unsigned long long Size = 0;
  for (unsigned int i = 0; i < 8; ++i)
    Size = (Size << 8) | static_cast<unsigned long long>(_Buffer[i]);

  // Avoid signed-shift overflow and implementation-defined unsigned conversion.
  if (Size > static_cast<unsigned long long>(
                 std::numeric_limits<long long>::max()))
    return -1;
  return static_cast<long long>(Size);
}

void NGAL_SAR::OpenHeaderSegmentationField(unsigned char* _Buffer,
                                          unsigned int& _MessageNumber,
                                          unsigned int& _SequenceNumber)
{
  _MessageNumber = 0;
  _SequenceNumber = 0;
  for (unsigned int i = 0; i < 4; ++i)
  {
    _MessageNumber = (_MessageNumber << 8) |
                     static_cast<unsigned int>(_Buffer[i]);
    _SequenceNumber = (_SequenceNumber << 8) |
                      static_cast<unsigned int>(_Buffer[i + 4]);
  }
}

void NGAL_SAR::BuildHeaderSizeField(unsigned char* Header, long long TotalSize)
{
  unsigned long long Size = static_cast<unsigned long long>(TotalSize);
  for (unsigned int i = 0; i < 8; ++i)
    Header[i] = static_cast<unsigned char>((Size >> (8 * (7 - i))) & 0xff);
}

void NGAL_SAR::BuildHeaderSegmentationField(unsigned char* Header,
                                           unsigned int MessageNumber,
                                           unsigned int SequenceNumber)
{
  for (unsigned int i = 0; i < 4; ++i)
  {
    Header[i] = static_cast<unsigned char>(
        (MessageNumber >> (8 * (3 - i))) & 0xff);
    Header[i + 4] = static_cast<unsigned char>(
        (SequenceNumber >> (8 * (3 - i))) & 0xff);
  }
}

int NGAL_SAR::SendSegmented(
    Message* M,
    unsigned int BlockSize,
    unsigned int& MessageNumber,
    unsigned int& SequenceNumber,
    unsigned int& MessageCounter,
    std::function<int(char*, unsigned int, unsigned int)> transport_callback)
{
  int Status = 1;
  long long MessageSize = 0;
  unsigned int NoCL = 0;

  // SN=0 must accommodate the size header and a nonempty message payload.
  // Also ensure that fragment size (BlockSize + SegHeader) fits unsigned int.
  if (!M || !transport_callback || BlockSize <= 8 ||
      BlockSize > std::numeric_limits<unsigned int>::max() - 8)
    return 1;

  M->ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray();
  if (M->GetMessageSize(MessageSize) != 0 || MessageSize <= 0)
    return 1;

  M->GetNumberofCommandLines(NoCL);
  if (NoCL <= 2)
    return 1;

  const unsigned long long TotalSize =
      static_cast<unsigned long long>(MessageSize) + 8;
  if (TotalSize > std::numeric_limits<size_t>::max())
    return 1;

  const unsigned long long SegmentCount =
      TotalSize / BlockSize + (TotalSize % BlockSize != 0);
  if (SegmentCount == 0 ||
      SegmentCount > std::numeric_limits<unsigned int>::max())
    return 1;
  const unsigned int NoS = static_cast<unsigned int>(SegmentCount);

  char* Payload = 0;
  M->GetMessageFromCharArray(Payload);
  if (!Payload)
    return 1;

  std::unique_ptr<unsigned char[]> SDU(
      new unsigned char[static_cast<size_t>(TotalSize)]);
  BuildHeaderSizeField(SDU.get(), MessageSize);
  std::memcpy(SDU.get() + 8, Payload, static_cast<size_t>(MessageSize));

  // Preserve the existing MN generation and caller-owned SN/counter semantics.
  MessageNumber = static_cast<unsigned int>(rand() % 4294967289) + 1;
#ifdef DEBUG
  cerr << "[DEBUG] NGAL_SAR::SendSegmented: MN=" << MessageNumber
       << " NoS=" << NoS << " MessageSize=" << MessageSize
       << " BlockSize=" << BlockSize << endl;
#endif

  for (unsigned int i = 0; i < NoS; ++i)
  {
    const unsigned long long Offset =
        static_cast<unsigned long long>(i) * BlockSize;
    const unsigned int SegmentPayloadSize = static_cast<unsigned int>(
        std::min<unsigned long long>(BlockSize, TotalSize - Offset));
    const unsigned int EffectiveSize = SegmentPayloadSize + 8;
    std::unique_ptr<unsigned char[]> DataBlock(
        new unsigned char[EffectiveSize]);

    BuildHeaderSegmentationField(DataBlock.get(), MessageNumber, SequenceNumber);
    std::memcpy(DataBlock.get() + 8,
                SDU.get() + static_cast<size_t>(Offset), SegmentPayloadSize);

    int send_status = transport_callback(
        reinterpret_cast<char*>(DataBlock.get()), EffectiveSize, i);
    if (send_status != 0)
    {
      Status = 1;
      break;
    }

    Status = 0;
    ++SequenceNumber;
#ifdef DEBUG
    cerr << "[DEBUG] NGAL_SAR::SendSegmented: MN=" << MessageNumber
         << " SN=" << SequenceNumber - 1 << "/" << NoS - 1
         << " frag_size=" << EffectiveSize << endl;
#endif
  }

  if (Status == 0)
  {
    ++MessageCounter;
#ifdef DEBUG
    cerr << "[DEBUG] NGAL_SAR::SendSegmented: COMPLETE MN=" << MessageNumber
         << " segments_sent=" << NoS << endl;
    cerr << endl;
#endif
    SequenceNumber = 0;
  }
  return Status;
}

void NGAL_SAR::LogProgress(const FragmentBuffer* FB)
{
#ifdef DEBUG
  unsigned int progress_pct =
      (FB->NoS > 0) ? (FB->SegmentsSoFar * 100 / FB->NoS) : 0;
  bool log_progress = (FB->SegmentsSoFar % 10 == 0) ||
                      (progress_pct == 25) || (progress_pct == 50) ||
                      (progress_pct == 75);
  if (log_progress || FB->SegmentsSoFar == FB->NoS)
  {
    cerr << "[DEBUG] NGAL_SAR::ReceiveFragment: PROGRESS MN=" << FB->MessageNumber
         << " seg=" << FB->SegmentsSoFar << "/" << FB->NoS
         << " (" << progress_pct << "%) bytes=" << FB->ReceivedSoFar
         << "/" << FB->MessageSize << endl;
  }
#else
  (void)FB;
#endif
}

int NGAL_SAR::ReceiveFragment(unsigned char* TempBuffer,
                              unsigned int numbytes,
                              unsigned int BlockSize,
                              char*& CompletedBuffer,
                              long long& CompletedSize)
{
  CompletedBuffer = 0;
  CompletedSize = 0;

  // No header access is permitted before this guard.
  if (numbytes < 8 || TempBuffer == 0)
  {
    // One event contributes to both dropped and guard_reject.
    Stats().Count(FrameTooShort);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: FRAME_TOO_SHORT_OR_NULL numbytes="
         << numbytes << " (dropped)" << endl;
#endif
    return 1;
  }

  unsigned int MN = 0;
  unsigned int SN = 0;
  OpenHeaderSegmentationField(TempBuffer, MN, SN);

  if (SN == 0 && numbytes < 16)
  {
    Stats().Count(FrameTooShort);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: FRAME_TOO_SHORT numbytes="
         << numbytes << " (dropped)" << endl;
#endif
    return 1;
  }
  if (MN == 0 || BlockSize <= 8 ||
      BlockSize > std::numeric_limits<unsigned int>::max() - 8)
  {
    Stats().Count(InvalidMNOrBlockSize);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: INVALID_MN_OR_BLOCKSIZE MN="
         << MN << " BlockSize=" << BlockSize << " (dropped)" << endl;
#endif
    return 1;
  }

  FragmentBuffer* FB = 0;
  size_t BufferIndex = 0;
  for (size_t c = 0; c < ReassemblyBuffers.size(); ++c)
  {
    if (ReassemblyBuffers[c]->MessageNumber == MN)
    {
      FB = ReassemblyBuffers[c];
      BufferIndex = c;
      break;
    }
  }

  if (!FB && SN != 0)
  {
    Stats().Count(OutOfOrder);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: OUT_OF_ORDER MN=" << MN
         << " SN=" << SN << " (no buffer yet, dropping)" << endl;
#endif
    return 1;
  }

  long long MessageSize = FB ? FB->MessageSize : 0;
  if (SN == 0)
  {
    const long long AdvertisedSize = OpenHeaderMessageSizeField(TempBuffer + 8);
    if (AdvertisedSize <= 0 || AdvertisedSize >= 10 * 1024 * 1024)
    {
      Stats().Count(InvalidSize);
#ifdef DEBUG
      cerr << "[WARN] NGAL_SAR::ReceiveFragment: INVALID_SIZE MN=" << MN
           << " MessageSize=" << AdvertisedSize << " (dropping)" << endl;
#endif
      return 1;
    }
    if (FB && AdvertisedSize != FB->MessageSize)
    {
      Stats().Count(SizeMismatch);
#ifdef DEBUG
      cerr << "[WARN] NGAL_SAR::ReceiveFragment: SIZE_MISMATCH MN=" << MN
           << " (existing buffer retained)" << endl;
#endif
      return 1;
    }
    MessageSize = AdvertisedSize;
  }

  if (FB && BlockSize != FB->BlockSize)
  {
    Stats().Count(BlockSizeMismatch);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: BLOCKSIZE_MISMATCH MN=" << MN
         << " got=" << BlockSize << " expected=" << FB->BlockSize
         << " (dropping)" << endl;
#endif
    return 1;
  }

  // All inputs to this integer ceiling division are now validated.
  const unsigned long long TotalSize =
      static_cast<unsigned long long>(MessageSize) + 8;
  const unsigned long long LocalNoS =
      TotalSize / BlockSize + (TotalSize % BlockSize != 0);
  if (LocalNoS == 0 ||
      LocalNoS > std::numeric_limits<unsigned int>::max() ||
      SN >= LocalNoS)
  {
    Stats().Count(SNOutOfRange);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: SN_OUT_OF_RANGE MN=" << MN
         << " SN=" << SN << " NoS=" << LocalNoS << " (dropping)" << endl;
#endif
    return 1;
  }

  // Unsigned wide multiplication avoids overflow even for hostile wire SNs.
  const unsigned long long Offset = SN == 0 ? 0 :
      static_cast<unsigned long long>(SN) * BlockSize - 8;
  const unsigned long long Capacity = SN == 0 ? BlockSize - 8 : BlockSize;
  if (Offset >= static_cast<unsigned long long>(MessageSize))
  {
    Stats().Count(InvalidPayload);
    return 1;
  }

  const unsigned long long Expected = std::min<unsigned long long>(
      Capacity, static_cast<unsigned long long>(MessageSize) - Offset);
  const unsigned int HeaderBytes = SN == 0 ? 16 : 8;
  const unsigned int PayloadBytes = numbytes - HeaderBytes;

  // Ignore trailing bytes (e.g. Ethernet padding); reject short fragments.
  // Copy and account for only the geometry-derived Expected bytes.
  // This guard precedes all allocation, eviction, copying and bitmap changes.
  if (Expected == 0 || PayloadBytes < Expected)
  {
    Stats().Count(InvalidPayload);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: INVALID_PAYLOAD MN=" << MN
         << " SN=" << SN << " payload=" << PayloadBytes
         << " expected=" << Expected << " (dropping)" << endl;
#endif
    return 1;
  }

  if (FB && FB->ReceivedSN[SN])
  {
    // Includes SN=0: retain the original buffer, do not copy or refresh time.
    Stats().Count(DuplicateSN);
#ifdef DEBUG
    cerr << "[WARN] NGAL_SAR::ReceiveFragment: DUPLICATE SN MN=" << MN
         << " SN=" << SN << " (ignored, no timestamp refresh)" << endl;
#endif
    return 1;
  }

  if (!FB)
  {
    const size_t Charge = static_cast<size_t>(MessageSize) +
                          static_cast<size_t>(LocalNoS) +
                          sizeof(FragmentBuffer) + 256;
    if (Charge > MAX_REASSEMBLY_BYTES)
    {
      Stats().Count(BufferLimitReached);
      return 1;
    }

    // Vector order is creation order, preserving oldest-created eviction.
    // Invalid incoming geometry can never reach this loop.
    while (!ReassemblyBuffers.empty() &&
           (ReassemblyBuffers.size() >= MAX_REASSEMBLY_BUFFERS ||
            ReassemblyBytes > MAX_REASSEMBLY_BYTES - Charge))
    {
      // Count each validated eviction, not each arriving fragment.
      Stats().Count(BufferLimitReached);
      Stats().Count(ValidatedEvictions);
#ifdef DEBUG
      const FragmentBuffer* Oldest = ReassemblyBuffers.front();
      cerr << "[WARN] NGAL_SAR::ReceiveFragment: BUFFER_LIMIT_REACHED count="
           << ReassemblyBuffers.size() << " bytes=" << ReassemblyBytes
           << " dropping OLDEST MN=" << Oldest->MessageNumber
           << " seg=" << Oldest->SegmentsSoFar << "/" << Oldest->NoS << endl;
#endif
      RemoveBuffer(0);
    }

    // Allocate only after making room, avoiding a transient cap overrun.
    // RAII owns all partially constructed state until insertion succeeds.
    try
    {
      std::unique_ptr<FragmentBuffer> NewBuffer(new FragmentBuffer);
      NewBuffer->MessageNumber = MN;
      NewBuffer->NoS = static_cast<unsigned int>(LocalNoS);
      NewBuffer->MessageSize = MessageSize;
      NewBuffer->BlockSize = BlockSize;
      NewBuffer->ChargedBytes = Charge;
      NewBuffer->ReceivedSN.assign(static_cast<size_t>(LocalNoS), false);
      NewBuffer->Buffer = new char[static_cast<size_t>(MessageSize)];

      ReassemblyBuffers.push_back(NewBuffer.get());
      FB = NewBuffer.release();
      ReassemblyBytes += Charge;
      BufferIndex = ReassemblyBuffers.size() - 1;
    }
    catch (const std::bad_alloc&)
    {
      Stats().Count(AllocationFailed);
#ifdef DEBUG
      cerr << "[ERROR] NGAL_SAR::ReceiveFragment: ALLOCATION_FAILED MN="
           << MN << " (dropping)" << endl;
#endif
      return 1;
    }
  }

  std::memcpy(FB->Buffer + static_cast<size_t>(Offset),
              TempBuffer + HeaderBytes, static_cast<size_t>(Expected));
  FB->ReceivedSoFar += Expected;
  ++FB->SegmentsSoFar;
  FB->ReceivedSN[SN] = true;
  FB->Timestamp = static_cast<double>(time(0));

  // Preserve continuing-fragment and incomplete-message progress logging.
  if (SN > 0)
    LogProgress(FB);

  if (FB->ReceivedSoFar >= FB->MessageSize &&
      FB->SegmentsSoFar >= FB->NoS)
  {
    FB->ContinueReceiving = false;
#ifdef DEBUG
    const unsigned int DoneSegments = FB->SegmentsSoFar;
    const long long DoneSize = FB->MessageSize;
#endif
    CompletedBuffer = FB->Buffer;
    CompletedSize = FB->MessageSize;
    FB->Buffer = 0; // Detach before the owning FragmentBuffer destructor runs.
    RemoveBuffer(BufferIndex);
    Stats().Count(Completed); // Only the completed-buffer handoff counts.
#ifdef DEBUG
    cerr << "[DEBUG] NGAL_SAR::ReceiveFragment: COMPLETE MN=" << MN
         << " segments=" << DoneSegments << " size=" << DoneSize << endl;
    cerr << endl;
#endif
    return 0;
  }

  if (SN == 0)
    LogProgress(FB);
  return 1;
}

void NGAL_SAR::CleanupTimedOut(double timeout_threshold)
{
  for (size_t i = 0; i < ReassemblyBuffers.size();)
  {
    FragmentBuffer* FB = ReassemblyBuffers[i];
    if (!FB->ContinueReceiving)
    {
#ifdef DEBUG
      cerr << "[WARN] NGAL_SAR::CleanupTimedOut: STALE buffer MN="
           << FB->MessageNumber
           << " (completed but not removed) - cleaning up" << endl;
#endif
      RemoveBuffer(i);
    }
    else if (FB->Timestamp > 0 &&
             (timeout_threshold - FB->Timestamp) > 30.0)
    {
      if (FB->SegmentsSoFar < FB->NoS)
        Stats().Count(TimeoutAbandoned);
#ifdef DEBUG
      cerr << "[ERROR] NGAL_SAR::CleanupTimedOut: TIMEOUT MN=" << FB->MessageNumber
           << " seg_received=" << FB->SegmentsSoFar << "/" << FB->NoS
           << " bytes=" << FB->ReceivedSoFar << "/" << FB->MessageSize
           << " age=" << (timeout_threshold - FB->Timestamp)
           << "s - discarding" << endl;
#endif
      RemoveBuffer(i);
    }
    else
    {
      ++i;
    }
  }
}
