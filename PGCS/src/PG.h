/*
        NovaGenesis

        Name:		Proxy and Gateway for underlying resources
        Object:		PG
        File:		PG.h
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.1

        Copyright (C) 2021  Antonio Marcos Alberti

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

#ifndef _PG_H
#define _PG_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef __OUTPUTVARIABLE_H
#include "OutputVariable.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _STRING_H
#include <string>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif

#ifndef _SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef _NETINET_IN_H
#include <netinet/in.h>
#endif

#ifndef _ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef _INTTYPES_H
#include <inttypes.h>
#endif

#ifndef _MATH_H
#include <math.h>
#endif

#ifndef _ERRNO_H
#include <errno.h>
#endif

#ifndef _ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef _NETDB_H
#include <netdb.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#ifndef _TUPLE_H
#include "Tuple.h"
#endif

#ifndef _MESSAGERECEIVING_H
#include "MessageReceiving.h"
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _IOCTL_H
#include <sys/ioctl.h>
#endif

#ifndef _IF_H
#include <linux/if.h>
#endif

#ifndef _NETDB_H
#include <netdb.h>
#endif

#ifndef _IF_ETHER_H
#include <linux/if_ether.h>
#endif

#ifndef _IF_PACKET_H
#include <linux/if_packet.h>
#endif

#ifndef _IOCTL_H
#include <sys/ioctl.h>
#endif

#define ERROR 1
#define OK 0

using namespace std;

class PG : public Block
{
public:
  // Gateway pointer (public for NGAL access)
  GW* PGW;

  // HT pointer
  HT* PHT;

  // Define the maximum segment size on shared memory
  size_t MaxSegmentSize;

  // Constructor
  PG(string _LN, Process* _PP, unsigned int _Index, GW* _PGW, HT* _PHT, string _Path);

  // Destructor
  ~PG();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary containers
  // ------------------------------------------------------------------------------------------------------------------------------

  // Stores the discovered NRNCS tuples
  vector<Tuple*> PSTuples;

  // Stores the informed peer PGCS tuples
  vector<Tuple*> PGCSTuples;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Main functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // ************************** Adaptation Layer **************************

  // Get the host IPv4 or IPv6 address
  void GetHostIPAddress(string _Stack, string _Interface, string& _Address);

  // Get the host MAC address
  void GetHostRawAddress(string _Interface, string& _Address);

  // Create a Raw socket
  int CreateRawSocket(int& _SID);

  // Send a message to another machine using a Raw socket
  // Delegates to NGAL_SAR + NGAL_Transport_RAW internally
  int SendToARawSocket(string _Interface, string _Identifier, unsigned int _Size, Message* M);

  // Socket dispatcher — replaced by NGAL_Transport_RAW::ReceiveDispatcher
  // Kept as a thin wrapper for backward compatibility
  void SocketDispatcher3();

  // ********************************************************************** // Adaptation

  // Finish the reception of a message via SHM (inter-process IPC)
  int WriteToSharedMemory3(File* _PF, char* _MessageCharArray, long long _MessageSize);

  long long int OpenHeaderMessageSizeField(unsigned char* _Buffer);

  void
  OpenHeaderSegmentationField(unsigned char* _Buffer, unsigned int& _MessageNumber, unsigned int& _SequenceNumber);

  // The message sequence number
  unsigned int Number_Of_Threads_At_Socket_Dispatcher;

  // The message sequence number
  unsigned int MessageNumber;

  // The fragment sequence number
  unsigned int SequenceNumber;

  // A counter to the number of messages already sent
  unsigned int MessageCounter;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Action related functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Allocate and add an Action on Actions container
  void NewAction(const string _LN, Action*& _PA);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary
  // ------------------------------------------------------------------------------------------------------------------------------

  // Auxiliary RTT measuring variable
  double TimeStamp;

  // Auxiliary variable to stop publish PGCS data as soon as possible
  bool AwareOfAPS;

  // Auxiliary variable to avoid publishing more than once PGCS basic bindings at NRNCS
  bool AlreadyPublishedBasicBindings;

  // Wrapper function for ReceiveDispatcher() thread (NGAL)
  static void ReceiveDispatcherWrapper(void* _PPG);

  // Wrapper function for EthernetWiFiSocketDispatcher() thread
  static void EthernetWiFiSocketDispatcherThreadWrapper(void* _PPG);

  // Get IP address function
  void* get_in_addr(struct sockaddr* sa);

  // Auxiliary to convert from 'e8' to 232
  void Hex2Char(char* szHex, unsigned char& rch);

  // The name of my domain
  string MyDomainName;

  // Hash of my domain name
  string HashOfMyDomainName;

  // The name of my upper level domain
  string MyUpperLevelDomainName;

  // Hash of the name of my upper level domain
  string HashOfMyUpperLevelDomainName;

  // ------------------------------------------------------------------------------------------------------------------------------
  // .ini file parameters
  // ------------------------------------------------------------------------------------------------------------------------------
  double DelayBeforeRunPeriodic;
  double DelayBetweenMessageEmissions;
  double DelayBetweenHellos01;
  double DelayBetweenHellos02;
  double DelayBetweenExpositions;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Stress test counters and configuration
  // ------------------------------------------------------------------------------------------------------------------------------

  bool StressEnabled;
  double StressInterval;
  std::atomic<unsigned long long> StressOffered{0};
  std::atomic<unsigned long long> StressSent{0};
  std::atomic<unsigned long long> StressReceived{0};
  std::atomic<unsigned long long> StressDropped{0};
  int StressDelayCount;
  File StressStats;
  OutputVariable* DelayStats;
  OutputVariable* Loss;

  // Validate the same admission predicate as GW before calling its void API. (SPEC-028-B)
  bool PushStressMessage(Message* M);

private:
  std::thread HeartbeatThread;
  std::mutex HeartbeatMutex;
  std::condition_variable HeartbeatCV;
  bool HeartbeatStop = false; // Protected by HeartbeatMutex.
  std::chrono::steady_clock::time_point HeartbeatStart;
  std::string HeartbeatRun;
  void HeartbeatLoop();
  void EmitHeartbeat(const char* prefix);

public:

  // ------------------------------------------------------------------------------------------------------------------------------
  // Friend classes
  // ------------------------------------------------------------------------------------------------------------------------------

  friend class PGRunInitialization01;
  friend class PGMsgCl01;
  friend class PGHelloIHC01;
  friend class PGHelloIHC02;
  friend class PGHelloIHC03;
  friend class PGRunHello01;
  friend class PGRunHello02;
  friend class PGRunHello03;
  friend class PGRunPublishing01;
  friend class PGRunPeriodic01;
  friend class PGRunExposition01;
  friend class PGRunStresstest01;
  friend class PGStresstestPing01;
};

struct PARAMETERS1
{
  PG* _PPG;
  unsigned int _Index;
};

#endif