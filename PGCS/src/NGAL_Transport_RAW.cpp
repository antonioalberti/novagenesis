/*
        NovaGenesis

        Name:           NovaGenesis Adaptation Layer — Transport RAW
        Object:         NGAL_Transport_RAW
        File:           NGAL_Transport_RAW.cpp
        Author:         Antonio Marcos Alberti
        Date:           07/2026
        Version:        0.1

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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _NGAL_TRANSPORT_RAW_H
#include "NGAL_Transport_RAW.h"
#endif

#ifndef _NGAL_SAR_H
#include "NGAL_SAR.h"
#endif

#ifndef _NGAL_CS_H
#include "NGAL_CS.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef _NETINET_IN_H
#include <netinet/in.h>
#endif

#ifndef _ERRNO_H
#include <errno.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _POLL_H
#include <poll.h>
#endif

#include <memory>
#include <new>
#include <cstdlib>
#include <sys/uio.h>

// #define DEBUG

// ─────────────────────────────────────────────────────────────────────────
// SPEC-031: batched receive pool (recvmmsg)
// C = ETH_FRAME_LEN (1514). One pool per dispatcher, independent of SSID count.
// ─────────────────────────────────────────────────────────────────────────
namespace
{
struct RawReceivePool
{
  enum { BatchSize = 32, Capacity = ETH_FRAME_LEN };
  unsigned char frames[BatchSize][Capacity];
  struct iovec vectors[BatchSize];
  struct mmsghdr messages[BatchSize];
  struct sockaddr_ll addresses[BatchSize];

  void Reset()
  {
    for (unsigned int slot = 0; slot < BatchSize; ++slot)
    {
      std::memset(&addresses[slot], 0, sizeof(addresses[slot]));
      std::memset(&messages[slot], 0, sizeof(messages[slot]));
      vectors[slot].iov_base = frames[slot];
      vectors[slot].iov_len = Capacity;
      messages[slot].msg_hdr.msg_name = &addresses[slot];
      messages[slot].msg_hdr.msg_namelen = sizeof(addresses[slot]);
      messages[slot].msg_hdr.msg_iov = &vectors[slot];
      messages[slot].msg_hdr.msg_iovlen = 1;
      messages[slot].msg_hdr.msg_control = NULL;
      messages[slot].msg_hdr.msg_controllen = 0;
      messages[slot].msg_hdr.msg_flags = 0;
      messages[slot].msg_len = 0;
    }
  }
};

void RawReceiveFailure(const char* reason)
{
  std::cerr << "[FATAL] NGAL_Transport_RAW::ReceiveDispatcher: "
            << reason << "; deployment failed, rollback required" << std::endl;
  std::exit(EXIT_FAILURE);
}
}

using namespace std;

// ─────────────────────────────────────────────────────────────────────────
// Create a Raw Ethernet socket
// ─────────────────────────────────────────────────────────────────────────
int NGAL_Transport_RAW::CreateRawSocket(int& _SID)
{
  int Status = 1; // ERROR
  unsigned short proto = 0x1234;

  _SID = socket(AF_PACKET, SOCK_RAW, htons(proto));

  if (_SID > 0)
  {
    Status = 0; // OK
  }
  else
  {
#ifdef DEBUG
    cerr << "[ERROR] NGAL_Transport_RAW::CreateRawSocket: socket() failed: " 
         << strerror(errno) << " (errno=" << errno << ")" << endl;
#endif
    perror("socket:");
  }

  return Status;
}

// ─────────────────────────────────────────────────────────────────────────
// Send one NGAL-PDU fragment via raw socket
// Called as callback from NGAL_SAR::SendSegmented()
// ─────────────────────────────────────────────────────────────────────────
int NGAL_Transport_RAW::SendFragment(int SSID, int ifindex,
                                     unsigned char* SourceMAC, unsigned char* DestMAC,
                                     unsigned char* FragmentData, unsigned int FragmentSize)
{
  int Status = 1; // ERROR
  unsigned int Tentatives = 12000; // ~2 minutes at 10ms per retry
  int numbytes;
  struct sockaddr_ll saddrll;
  unsigned short proto = 0x1234;
  ngal_ethframe frame;

  while (Tentatives > 0)
  {
    // Configure the Ethernet II address struct
    memset((void*)&saddrll, 0, sizeof(saddrll));
    saddrll.sll_family = PF_PACKET;
    saddrll.sll_ifindex = ifindex;
    saddrll.sll_halen = 6; // ETH_ALEN
    memcpy((void*)(saddrll.sll_addr), (void*)DestMAC, 6);

    // Configure the Ethernet II frame
    memcpy(frame.field.header.h_dest, DestMAC, 6);
    memcpy(frame.field.header.h_source, SourceMAC, 6);
    frame.field.header.h_proto = htons(proto);
    memcpy(frame.field.data, FragmentData, FragmentSize);

    unsigned int frame_len = FragmentSize + 14; // ETH_HLEN

    numbytes = static_cast<int>(sendto(SSID, (void*)frame.buffer, frame_len, 0,
                                        (struct sockaddr*)&saddrll, sizeof(saddrll)));

    if (numbytes > 0)
    {
      Tentatives = 0;
      Status = 0; // OK
#ifdef DEBUG
      cerr << "[DEBUG] NGAL_Transport_RAW::SendFragment: SSID=" << SSID 
           << " ifindex=" << ifindex << " frag_size=" << FragmentSize 
           << " bytes_sent=" << numbytes << endl;
#endif
    }
    else
    {
      perror("sendto:");
#ifdef DEBUG
      if (Tentatives % 100 == 0 || Tentatives == 11999) // Log every 100 retries + first
      {
        cerr << "[WARN] NGAL_Transport_RAW::SendFragment: sendto() failed (attempt " 
             << (12000 - Tentatives) << "/12000) SSID=" << SSID 
             << " ifindex=" << ifindex << " errno=" << errno 
             << " (" << strerror(errno) << ")" << endl;
      }
#endif
      Tentatives--;
      // Sleep 10ms between retries
      struct timespec ts = {0, 10 * 1000000};
      nanosleep(&ts, 0);
    }
  }

#ifdef DEBUG
  if (Status != 0)
  {
    cerr << "[ERROR] NGAL_Transport_RAW::SendFragment: FAILED after 12000 retries SSID=" << SSID 
         << " ifindex=" << ifindex << " frag_size=" << FragmentSize << endl;
  }
#endif
  return Status;
}

// ─────────────────────────────────────────────────────────────────────────
// Main receive dispatcher thread (replaces SocketDispatcher3)
// Uses poll() on all SSIDs to avoid socket starvation.
// Receives frames, feeds NGAL_SAR, delivers via NGAL_CS.
// ─────────────────────────────────────────────────────────────────────────
void NGAL_Transport_RAW::ReceiveDispatcher(PG* PPG)
{
  if (PPG == 0)
    return;

  PGCS* PPGCS = (PGCS*)PPG->PP;
  if (PPGCS == 0)
    return;

  string Offset = "                    ";
  NGAL_SAR SAR;                                // Reassembly state per dispatcher
  unsigned int numbytes = 0;

  // SPEC-031: dispatcher-owned batch receive pool (RAII releases at scope exit;
  // the polling loop has no stop flag).
  std::unique_ptr<RawReceivePool> pool(new (std::nothrow) RawReceivePool);
  if (!pool)
    RawReceiveFailure("receive-pool allocation failed");

  unsigned int nextStart = 0;

  while (1)
  {
    // Use poll() to check all SSIDs for readability
    if (PPGCS->SSIDs != 0 && PPGCS->SSIDs->size() > 0)
    {
      unsigned int nfds = static_cast<unsigned int>(PPGCS->SSIDs->size());
      struct pollfd fds[32]; // Max 32 sockets per dispatcher

      if (nfds > 32)
        nfds = 32;

      for (unsigned int i = 0; i < nfds; i++)
      {
        fds[i].fd = PPGCS->SSIDs->at(i);
        fds[i].events = POLLIN;
        fds[i].revents = 0;
      }

      int poll_result = poll(fds, nfds, 100); // 100ms timeout

      if (poll_result < 0)
      {
        // Interrupted or error — retry
#ifdef DEBUG
        if (errno != EINTR)
        {
          cerr << "[WARN] NGAL_Transport_RAW::ReceiveDispatcher: poll() error: " 
               << strerror(errno) << " (errno=" << errno << ")" << endl;
        }
#endif
        continue;
      }

      if (poll_result == 0)
      {
        // Timeout — cleanup timed-out reassembly buffers and continue
        SAR.CleanupTimedOut(PPGCS->GetTime());
        continue;
      }

      // Process readable sockets — SPEC-031: rotate the first socket each cycle,
      // retaining each socket's original Sizes index for BlockSize lookup.
      const unsigned int start = nextStart % nfds;
      nextStart = (start + 1) % nfds;

      for (unsigned int turn = 0; turn < nfds; ++turn)
      {
        const unsigned int i = (start + turn) % nfds;

        if ((fds[i].revents & POLLIN) == 0)
          continue;

        // SPEC-031 drain policy: up to two recvmmsg calls (64 frames) per ready
        // socket per poll cycle; EAGAIN ends this socket's turn.
        for (unsigned int attempt = 0; attempt < 2; ++attempt)
        {
          pool->Reset();
          // NOTE: MSG_TRUNC must NOT be passed as an input flag on AF_PACKET raw
          // sockets (it is an output-only flag there; as input it causes EINVAL).
          // Oversize detection instead relies on msg_len > Capacity and the
          // MSG_TRUNC bit in msg_hdr.msg_flags set by the kernel when trimming.
          const int nrec = recvmmsg(fds[i].fd, pool->messages,
                                    RawReceivePool::BatchSize,
                                    MSG_DONTWAIT | MSG_TRUNC, NULL);

          if (nrec <= 0)
          {
            if (nrec < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
            {
              // SPEC-031: visible without DEBUG — a persistent non-EAGAIN error
              // starves the receive path (once per 5s, throttled by static var).
              static double lastErrLog = 0;
              double now = PPGCS->GetTime();
              if (now - lastErrLog >= 5.0)
              {
                lastErrLog = now;
                cerr << "[ERROR] NGAL_Transport_RAW::ReceiveDispatcher: recvmmsg() failed fd=" << fds[i].fd
                     << " errno=" << errno << " (" << strerror(errno) << ")" << endl;
              }
            }
            break; // EAGAIN/EWOULDBLOCK: drain complete; 0: nothing more this turn
          }

          for (int slot = 0; slot < nrec; ++slot)
          {
            const unsigned int receivedbytes = static_cast<unsigned int>(pool->messages[slot].msg_len);
            const struct sockaddr_ll* from = &pool->addresses[slot];

            // SPEC-031: reject truncated frames (payload longer than pool capacity)
            // and too-short frames — never parse partial data.
            if ((pool->messages[slot].msg_hdr.msg_flags & MSG_TRUNC) != 0 ||
                receivedbytes > RawReceivePool::Capacity)
            {
              NGAL_SAR::Stats().Count(NGAL_SAR::RxTruncated);
#ifdef DEBUG
              cerr << "[WARN] NGAL_Transport_RAW::ReceiveDispatcher: RX_TRUNCATED fd=" << fds[i].fd
                   << " bytes=" << receivedbytes << " (dropped)" << endl;
#endif
              continue;
            }

            if (receivedbytes < 14 + 8)
            {
              NGAL_SAR::Stats().Count(NGAL_SAR::RxTooShort);
#ifdef DEBUG
              cerr << "[WARN] NGAL_Transport_RAW::ReceiveDispatcher: FRAME_TOO_SMALL fd=" << fds[i].fd
                   << " bytes=" << receivedbytes << " (dropped)" << endl;
#endif
              continue;
            }

#ifdef DEBUG
            cerr << "[DEBUG] NGAL_Transport_RAW::ReceiveDispatcher: fd=" << fds[i].fd
                 << " received_bytes=" << receivedbytes
                 << " proto=0x" << hex << from->sll_protocol << dec << endl;
#endif
            // Only process NovaGenesis frames (ethertype 0x1234 → sll_protocol 13330)
            // Note (F6): 0x1234 on wire appears as 13330 (0x3412) in host byte order
            if (from->sll_protocol != 13330)
            {
#ifdef DEBUG
              cerr << "[DEBUG] NGAL_Transport_RAW::ReceiveDispatcher: NON_NG_FRAME fd=" << fds[i].fd
                   << " proto=0x" << hex << from->sll_protocol << dec
                   << " bytes=" << receivedbytes << " (ignored)" << endl;
#endif
              continue;
            }

            // Strip Ethernet header (14 bytes)
            numbytes = receivedbytes - 14;

            if (numbytes < 8)
            {
              NGAL_SAR::Stats().Count(NGAL_SAR::RxTooShort);
#ifdef DEBUG
              cerr << "[WARN] NGAL_Transport_RAW::ReceiveDispatcher: FRAME_TOO_SMALL fd=" << fds[i].fd
                   << " payload_bytes=" << numbytes << " (min 8 for SegHeader, dropping)" << endl;
#endif
              continue;
            }

            // Copy payload out of the pool slot — SAR and delivery must not retain
            // pointers into receive buffers (SPEC-031 ownership rule).
            unsigned char* TempBuffer = new unsigned char[numbytes];
            memcpy(TempBuffer, pool->frames[slot] + 14, numbytes);

            // Get the BlockSize for this SSID
            unsigned int BlockSize = 1400; // default
            if (PPGCS->Sizes != 0 && i < PPGCS->Sizes->size())
            {
              BlockSize = PPGCS->Sizes->at(i);
            }

            // Feed to NGAL_SAR for reassembly
            char* CompletedBuffer = 0;
            long long CompletedSize = 0;
            int sar_status = SAR.ReceiveFragment(TempBuffer, numbytes, BlockSize,
                                                 CompletedBuffer, CompletedSize);

            if (sar_status == 0 && CompletedBuffer != 0 && CompletedSize > 0)
            {
              // Message reassembled — deliver raw char buffer to GW via NGAL_CS.
              NGAL_CS::DeliverToGateway(PPG->PGW, CompletedBuffer, CompletedSize);

#ifdef DEBUG
              cerr << "[DEBUG] NGAL_Transport_RAW::ReceiveDispatcher: Delivered to GW size=" << CompletedSize << endl;
#endif
              // DeliverToGateway copies for the queue; delete our copy.
              delete[] CompletedBuffer;
            }

            delete[] TempBuffer;
          }

          // A short batch means the socket is drained for now.
          if (nrec < static_cast<int>(RawReceivePool::BatchSize))
            break;
        }
      }
    }
    else
    {
      // No sockets yet — sleep and retry
      struct timespec ts = {0, 100 * 1000000}; // 100ms
      nanosleep(&ts, 0);
    }
  }
}