/*
        NovaGenesis

        Name:		Proxy and Gateway for underlying resources
        Object:		PG
        File:		PG.cpp
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

        *********************************************************************************************
        Some of the functions to receive and send messages using sockets were adapted from beej.us
        *********************************************************************************************

        Beej's Guide to Network Programming is Copyright © 2012 Brian "Beej Jorgensen" Hall.
        With specific exceptions for source code and translations, below, this work is licensed under the Creative Commons Attribution- Noncommercial- No Derivative Works 3.0 License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/3.0/ or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
        One specific exception to the "No Derivative Works" portion of the license is as follows: this guide may be freely translated into any language, provided the translation is accurate, and the guide is reprinted in its entirety. The same license restrictions apply to the translation as to the original guide. The translation may also include the name and contact information for the translator.
        The C source code presented in this document is hereby granted to the public domain, and is completely free of any license restriction.
        Educators are freely encouraged to recommend or supply copies of this guide to their students.

        Contact beej@beej.us for more information.

        *********************************************************************************************
        Additional functions to discover Linux Interface names, i.e. em1, eth0, etc.
        *********************************************************************************************

        * showif.c : PUBLIC DOMAIN - Jon Mayo - August 22, 2006
        * - You may remove any comments you wish, modify this code any way you wish,
        *   and distribute any way you wish.
        * finds all network interfaces and shows a little information about them.
        * some operating systems list interfaces multiple times because of different
        * flags, modes, etc. if you want to use this code you should be aware that
        * duplicate interfaces is a possibility

*/

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

#ifndef _PGRUNINITIALIZATION01_H
#include "PGRunInitialization01.h"
#endif

#ifndef _PGMSGCL01_H
#include "PGMsgCl01.h"
#endif

#ifndef _PGHELLOIHC01_H
#include "PGHelloIHC01.h"
#endif

#ifndef _PGHELLOIHC02_H
#include "PGHelloIHC02.h"
#endif

#ifndef _PGHELLOIHC03_H
#include "PGHelloIHC03.h"
#endif

#ifndef _PGRUNEXPOSITION01_H
#include "PGRunExposition01.h"
#endif

#ifndef _PGRUNHELLO01_H
#include "PGRunHello01.h"
#endif

#ifndef _PGRUNHELLO02_H
#include "PGRunHello02.h"
#endif

#ifndef _PGRUNHELLO03_H
#include "PGRunHello03.h"
#endif

#ifndef _PGRUNPUBLISHING01_H
#include "PGRunPublishing01.h"
#endif

#ifndef _PGRUNPERIODIC01_H
#include "PGRunPeriodic01.h"
#endif

#ifndef _PGRUNSTRESSTEST01_H
#include "PGRunStresstest01.h"
#endif

#ifndef _PGSTRESSTESTPING01_H
#include "PGStresstestPing01.h"
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#include <fstream>
#endif

// #define DEBUG
// #define DEBUG2
// #define DEBUG1
// #define DEBUG3  // DEBUG6 or 3 are mutually exclusive
// #define DEBUG5  // Details of each segment sent
// #define DEBUG6  

#define PG_LOG(msg) S << endl \
                      << "[" << fixed << setprecision(3) << GetTime() << "s] " << Offset << msg << endl
#define STATISTICS


// ── NGAL headers ──
#ifndef _NGAL_SAR_H
#include "NGAL_SAR.h"
#endif

#ifndef _NGAL_CS_H
#include "NGAL_CS.h"
#endif

#ifndef _NGAL_TRANSPORT_RAW_H
#include "NGAL_Transport_RAW.h"
#endif

PG::PG(string _LN, Process* _PP, unsigned int _Index, GW* _PGW, HT* _PHT, string _Path)
    : Block(_LN, _PP, _Index, _Path)
{
  PGW = _PGW;
  PHT = _PHT;
  State = "initialization";

  Message* PIM = 0;
  CommandLine* PCL = 0;
  Message* InlineResponseMessage = NULL;
  Action* PA = 0;
  TimeStamp = 0;
  MaxSegmentSize = MAX_MESSAGE_SIZE;                                               // The size of the shared memory segment
  Number_Of_Threads_At_Socket_Dispatcher = NUMBER_OF_THREADS_AT_SOCKET_DISPATCHER; /* This is the number of simultaneous threads that are employed at a certain socket to treat received protocol data units of all technologies */

  // Set the delays
  DelayBeforeRunPeriodic = 5;
  DelayBetweenMessageEmissions = 10;
  DelayBetweenHellos01 = 1;
  DelayBetweenHellos02 = 1;
  DelayBetweenExpositions = 1;

  // Set stress test defaults
  StressEnabled = false;
  StressInterval = 5;
  StressSent = 0;
  StressReceived = 0;
  StressDropped = 0;
  StressDelayCount = 0;

  // Set auxiliary variables
  AwareOfAPS = false;
  AlreadyPublishedBasicBindings = false;

  // The name of my domain
  MyDomainName = "Undefined";

  // Hash of my domain name
  HashOfMyDomainName = "Undefined";

  // The name of my upper level domain
  MyUpperLevelDomainName = "Undefined";

  // Hash of the name of my upper level domain
  HashOfMyUpperLevelDomainName = "Undefined";

#ifdef STATISTICS

  // Allocating the statistics variables
  DelayStats = new OutputVariable(this);
  DelayStats->Initialization("delay", "MEAN_ARITHMETIC", "One-way delay for stress test messages (seconds)", 0);
  DelayStats->SetFileName("StressDelay_Results.txt");

  Loss = new OutputVariable(this);
  Loss->Initialization("loss", "MEAN_ARITHMETIC", "Packet loss rate for stress test (percent)", 0);
  Loss->SetFileName("StressLoss_Results.txt");

#endif

  MessageNumber = 0;
  MessageCounter = 0;
  SequenceNumber = 0;

  srand(static_cast<unsigned int>(time(NULL)));

  // Creating the actions
  NewAction("-run --initialization 0.1", PA);
  NewAction("-m --cl 0.1", PA);
  NewAction("-hello --ihc 0.1", PA);
  NewAction("-hello --ihc 0.2", PA);
  NewAction("-hello --ihc 0.3", PA);
  NewAction("-run --exposition 0.1", PA);
  NewAction("-run --hello 0.1", PA);
  NewAction("-run --hello 0.2", PA);
  NewAction("-run --hello 0.3", PA);
  NewAction("-run --publishing 0.1", PA);
  NewAction("-run --periodic 0.1", PA);
  NewAction("-run --stresstest 0.1", PA);
  NewAction("-stresstest --ping 0.1", PA);

  // Open stress test stats file
  StressStats.OpenOutputFile("StressTest_Stats.txt", _Path, "DEFAULT");
  DelayStats->SetFilePath(_Path);

  // Creating a -run --initialization message
  PP->NewMessage(GetTime(), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine("-run", "--initialization", "0.1", PCL);

  // Push the message to the GW input queue
  // PGW->PushToInputQueue(PIM);

  // Run
  Run(PIM, InlineResponseMessage);

  // Mark to delete
  PIM->MarkToDelete();
}

PG::~PG()
{
  cout << endl
       << "[StressTest] Final: Sent=" << StressSent
       << " Received=" << StressReceived << " Dropped=" << StressDropped << endl;

  delete DelayStats;
  delete Loss;

  Tuple* Temp = 0;

  vector<Tuple*>::iterator it1;

  for (it1 = PSTuples.begin(); it1 != PSTuples.end(); it1++)
  {
    Temp = *it1;

    if (Temp != 0)
    {
      delete Temp;
    }

    Temp = 0;
  }

  vector<Tuple*>::iterator it2;

  for (it2 = PGCSTuples.begin(); it2 != PGCSTuples.end(); it2++)
  {
    Temp = *it2;

    if (Temp != 0)
    {
      delete Temp;
    }

    Temp = 0;
  }

  vector<Action*>::iterator it4;

  Action* Temp2 = 0;

  for (it4 = Actions.begin(); it4 != Actions.end(); it4++)
  {
    Temp2 = *it4;

    if (Temp2 != 0)
    {
      delete Temp2;
    }

    Temp2 = 0;
  }
}
void PG::GetHostIPAddress(string _Stack, string _Interface, string& _Address)
{
  struct ifaddrs* ifAddrStruct = NULL;
  struct ifaddrs* ifa = NULL;
  void* tmpAddrPtr = NULL;
  int Control = 0;

  getifaddrs(&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
  {
    if (ifa != 0 && ifa->ifa_addr != 0)
    {
      if (ifa->ifa_addr->sa_family == AF_INET && (_Stack == "IPv4_UDP"))
      {
        tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;

        char addressBuffer[INET_ADDRSTRLEN];

        // cout << "INET_ADDRSTRLEN = " << INET_ADDRSTRLEN << endl;

        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

        // printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);

        if (ifa->ifa_name == _Interface && Control == 0)
        {
          char Temp[INET_ADDRSTRLEN];

          for (int j = 0; j < INET_ADDRSTRLEN; j++)
          {
            Temp[j] = addressBuffer[j];

            // printf("%i %c\n",j,Temp[j]);
          }

          Control = 1;

          _Address = Temp;
        }
      }
      else if (ifa->ifa_addr->sa_family == AF_INET6 && (_Stack == "TCPv6" || _Stack == "UDPv6"))
      {
        tmpAddrPtr = &((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr;

        char addressBuffer[INET6_ADDRSTRLEN];

        inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);

        // printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);

        if (ifa->ifa_name == _Interface && Control == 0)
        {
          char Temp[INET6_ADDRSTRLEN];

          for (int j = 0; j < INET6_ADDRSTRLEN; j++)
          {
            Temp[j] = addressBuffer[j];

            // printf("%i %c\n",j,Temp[j]);
          }

          Control = 1;

          _Address = Temp;
        }
      }
    }
  }

  if (ifAddrStruct != NULL)
  {
    freeifaddrs(ifAddrStruct);
  }
}

// ************************** Adaptation Layer **************************

// The following code is related to the Ethernet/Wi-Fi stack

void PG::GetHostRawAddress(string _Interface, string& _Address)
{
  int Size = 13;
  char* ret = new char[Size]();
  struct ifreq s;

  _Address = "";

  // Try sysfs first (works without root, more reliable in containers/VMs)
  string sysfs_path = "/sys/class/net/" + _Interface + "/address";
  ifstream sysfs_file(sysfs_path.c_str());
  if (sysfs_file.is_open())
  {
    string mac_str;
    getline(sysfs_file, mac_str);
    sysfs_file.close();

    // Remove whitespace and validate length (17 chars = "xx:xx:xx:xx:xx:xx")
    mac_str.erase(0, mac_str.find_first_not_of(" \t\n\r\f\v"));
    mac_str.erase(mac_str.find_last_not_of(" \t\n\r\f\v") + 1);

    if (mac_str.length() >= 17)
    {
      _Address = mac_str;
      delete[] ret;
      return;
    }
  }

  // Fallback to ioctl
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  strcpy(s.ifr_name, _Interface.c_str());

  if (fd >= 0 && ret && 0 == ioctl(fd, SIOCGIFHWADDR, &s))
  {
    int i;
    for (i = 0; i < 6; ++i)
      snprintf(ret + i * 2, static_cast<size_t>(Size - i * 2), "%02x", (unsigned char)s.ifr_addr.sa_data[i]);

    for (int k = 0; k < 12; k = k + 2)
    {
      if (k == 10)
      {
        _Address = _Address + ret[k] + ret[k + 1];
      }
      else
      {
        _Address = _Address + ret[k] + ret[k + 1] + ":";
      }
    }
  }

  close(fd);

  delete[] ret;
}

// Create a Raw Ethernet socket
int PG::CreateRawSocket(int& _SID)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned short proto = 0x1234;

  // ************************** Adaptation Layer **************************

  _SID = socket(AF_PACKET, SOCK_RAW, htons(proto));

  if (_SID > 0)
  {
    Status = OK;
  }
  else
  {
    S << Offset << "(ERROR: Unable to create a new socket. Check if you have root permission.)" << endl;

    perror("socket:");
  }

  // **********************************************************************

  return Status;
}
// Write to other OS
int PG::SendToARawSocket(string _Interface, string _Identifier, unsigned int _Size, Message* M)
{
  int Status = ERROR;
  string Offset = "                    ";
  vector<string>* SIDs = new vector<string>;
  PGCS* PPGCS = (PGCS*)PP;
  unsigned char Source[ETH_ALEN];
  unsigned char Destination[ETH_ALEN];
  unsigned int z = 0;
  char TempSource[6][2];
  char TempDestination[6][2];

  // Address formatting translation
  for (unsigned int x = 0; x < 6; x++)
  {
    for (unsigned int y = 0; y < 2; y++)
    {
      TempSource[x][y] = PPGCS->MyMACAddress[y + z];
    }
    z = z + 3;
    Hex2Char(TempSource[x], Source[x]);
  }

  z = 0;

  for (unsigned int x = 0; x < 6; x++)
  {
    for (unsigned int y = 0; y < 2; y++)
    {
      TempDestination[x][y] = _Identifier[y + z];
    }
    z = z + 3;
    Hex2Char(TempDestination[x], Destination[x]);
  }

  // Get the SSID from HT binding on Category 17
  if (PP->GetHTBindingValues(17, _Identifier, SIDs) == OK)
  {
    if (SIDs->size() == 1)
    {
      int SSID = StringToInt(SIDs->at(0));
      struct ifreq buffer;
      int ifindex = 0;

      memset(&buffer, 0x00, sizeof(buffer));
      strncpy(buffer.ifr_name, _Interface.c_str(), IFNAMSIZ);

      if (ioctl(SSID, SIOCGIFINDEX, &buffer) >= 0)
      {
        ifindex = buffer.ifr_ifindex;

        // Delegate to NGAL_SAR + NGAL_Transport_RAW
        NGAL_SAR SAR;

        auto transport_callback = [SSID, ifindex, &Source, &Destination]
          (char* fragment_data, unsigned int fragment_size, unsigned int) -> int
        {
          return NGAL_Transport_RAW::SendFragment(SSID, ifindex,
                                                   Source, Destination,
                                                   (unsigned char*)fragment_data, fragment_size);
        };

        Status = SAR.SendSegmented(M, _Size,
                                    MessageNumber, SequenceNumber, MessageCounter,
                                    transport_callback);
      }
      else
      {
        S << Offset << "(ERROR: could not get the interface index on the OS)" << endl;
        perror("writing socket : ioctl");
      }
    }
    else
    {
      S << Offset << "(ERROR: more than one identifier to this address " << _Identifier << ")" << endl;
    }
  }
  else
  {
    S << Offset << "(ERROR: unable to recover the client socket ID from the PGCS::HT block)" << endl;
  }

  delete SIDs;

  return Status;
}

// SocketDispatcher3 — delegates to NGAL_Transport_RAW::ReceiveDispatcher
void PG::SocketDispatcher3()
{
  NGAL_Transport_RAW::ReceiveDispatcher(this);
}

// Modified in 9th April 2021 to deal with parallel shared memories.
int PG::WriteToSharedMemory3(File* _PF, char* _MessageCharArray, long long _MessageSize)
{
  int Status = ERROR;
  string Offset = "          ";

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
       z++) // Modified in 9th April 2021 to deal with parallel shared memories.
  {
    long long TotalSize = 0; // The total size of the message being transferred
    unsigned char* data;     // Manipulate the content on shared memory
    void* shm_address;       // The shared memory segment address
    string SemaphoreName;
    sem_t* mutex;
    vector<string>* Values = new vector<string>;

    // Get the correct shmid for PGCS's shared memory (key = 11 + z)
    // PP->shmid[z] is the process's own shmid for key PP->Key + z, not PGCS's shmid
    int shmid_z = -1;
    int _shm_key = 11 + z;

    if (PGW != 0)
    {
      PGW->ReturnIPCSHMID((key_t)_shm_key, shmid_z);
    }

    if (shmid_z != -1)
    {

#ifdef DEBUG3

      *_PF << "Writing to shared memory with key " << _shm_key << " and id = " << shmid_z << " at PG)" << endl;

#endif

      // Set the semaphore name
      SemaphoreName = IntToString(_shm_key);

      mutex = sem_open(SemaphoreName.c_str(), O_CREAT, 0666, 1);

      // Check for error on semaphore open
      if (mutex != SEM_FAILED)
      {

        // ***************************************************************************************
        // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
        // ***************************************************************************************

#ifdef DEBUG2
        *_PF << Offset << "(Opened the semaphore " << SemaphoreName << ")" << endl;
#endif

        if ((shm_address = shmat(shmid_z, NULL, 0)) != NULL)
        {

          // Retry sem_trywait with backoff to avoid CPU spin
          int LockAttempts = 0;
          while (sem_trywait(mutex) != 0 && LockAttempts < 100)
          {
            tthread::this_thread::sleep_for(tthread::chrono::microseconds(100));
            LockAttempts++;
          }
          if (LockAttempts < 100)
          {

#ifdef DEBUG2
            *_PF << Offset << "(Locked the semaphore " << SemaphoreName << ")" << endl;
#endif

            data = (unsigned char*)shm_address;

            if (data != (unsigned char*)(-1))
            {
              if (data[0] != (unsigned char)'f' && data[0] != (unsigned char)'w')
              {
                data[0] = (unsigned char)'f';
#ifdef DEBUG2
                *_PF << Offset
                     << "(Initialized the shared memory with key 11 regarding the r/w control flag)"
                     << endl;
#endif
              }
            }

#ifdef DEBUG2
            *_PF << "          (The value of the first byte in shared memory is " << (char)data[0]
                 << " (f = Free to write, w = still waiting for peer reading)" << endl;
            *_PF << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

            if (data[0] == (unsigned char)'f')
            {

#ifdef DEBUG2
              *_PF << "[5]       (Writing a message to the shared memory segment with key " << _shm_key << " identified by "
                   << shmid_z << ")" << endl;
#endif

              TotalSize = _MessageSize + 8;

              unsigned char* HeaderSizeField = new unsigned char[TotalSize];
              unsigned char* PDU = new unsigned char[TotalSize];

              HeaderSizeField[0] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 7)) & 0xff);
              HeaderSizeField[1] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 6)) & 0xff);
              HeaderSizeField[2] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 5)) & 0xff);
              HeaderSizeField[3] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 4)) & 0xff);
              HeaderSizeField[4] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 3)) & 0xff);
              HeaderSizeField[5] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 2)) & 0xff);
              HeaderSizeField[6] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 1)) & 0xff);
              HeaderSizeField[7] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 0)) & 0xff);

              for (long long j = 0; j < TotalSize; j++)
              {
                if (j < 8)
                {
                  PDU[j] = HeaderSizeField[j];
                  // printf("%i %d %c \n",j,PDU[j],PDU[j]);
                }

                if (j >= 8 && j < (_MessageSize + 8))
                {
                  PDU[j] = (unsigned char)_MessageCharArray[(j - 8)];
                  // printf("%i %d %c \n",j,PDU[j],PDU[j]);
                }
              }

              if (TotalSize <= (long long)MaxSegmentSize)
              {
#ifdef DEBUG3
                *_PF << Offset << "(The size of the payload is = " << _MessageSize << ")" << endl;

                *_PF << Offset << "(The size of the total message is = " << TotalSize << ")" << endl;

                *_PF << Offset << "(The size of the memory segment is = " << MaxSegmentSize << ")"
                     << endl;

                // S << Offset << "(Make the shared memory unavailable while not read by the peer)"<< endl;
#endif

                // Clean the memory segment before writing
                // April 2nd, 2021: changed from MaxSegmentSize to 1, since there is not need to clean more than this
                memset(data, (unsigned char)'w',
                       1);

                for (long long k = 1; k < TotalSize + 1; k++)
                {
                  data[k] = PDU[k - 1];
                }

                // Send the timestamp of this operation after the message

                unsigned long long Time = (unsigned long long)(GetTime() * 10e9);

                unsigned char* HeaderTimeStampField = new unsigned char[8];

                HeaderTimeStampField[0] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 7)) & 0xff);
                HeaderTimeStampField[1] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 6)) & 0xff);
                HeaderTimeStampField[2] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 5)) & 0xff);
                HeaderTimeStampField[3] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 4)) & 0xff);
                HeaderTimeStampField[4] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 3)) & 0xff);
                HeaderTimeStampField[5] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 2)) & 0xff);
                HeaderTimeStampField[6] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 1)) & 0xff);
                HeaderTimeStampField[7] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 0)) & 0xff);

                for (unsigned int n = 0; n < 8; n++)
                {
                  data[TotalSize + 1 + n] = HeaderTimeStampField[n];
                }

                delete[] HeaderTimeStampField;

                Status = OK;
              }
              else
              {
                *_PF << Offset
                     << "(ERROR: The message is too big for the memory segment or it has less than 3 command lines)"
                     << endl;
              }

              delete[] PDU;
              delete[] HeaderSizeField;

            } // (data[0] != (unsigned char)'f') means the memory is not free to write, despite of locking the semaphore.
            else
            {
#ifdef DEBUG
              *_PF << Offset
                   << "(Warning: The peer still does not read the shared memory. Trying again later)"
                   << endl;
#endif

              Status = ERROR;

              // Sleep before trying to send in the shared memory again
              tthread::this_thread::sleep_for(tthread::chrono::microseconds(5));
            }

            if (sem_post(mutex) == 0) // Successfully locked the semaphore, so unlock it
            {
#ifdef DEBUG2
              *_PF << Offset << "(Unlocked the semaphore " << SemaphoreName << ")" << endl;
#endif
            }
            else
            {
              perror("writing SHM : sem_post");
            }

          } // (LockAttempts < 100) successfully locked
          else
          {
            // Failed to lock after 100 attempts (10ms)
#ifdef DEBUG1
            // perror("writing SHM : unable to lock the semaphore");
#endif
          }

          // Sleep while wait for shared memory semaphore
          tthread::this_thread::sleep_for(tthread::chrono::microseconds(5));

          // Detach from shared memory, if attached
          if (shmdt(shm_address) == -1)
          {
            perror("writing SHM : shmdt");
          }

        } // ((shm_address = shmat(shmid, NULL, 0)) == NULL) means UNABLE TO ATTACH
        else
        {
          perror("writing SHM : semop");
        }

        if (sem_close(mutex) == 0)
        {
#ifdef DEBUG2
          *_PF << Offset << "(Closed the semaphore " << SemaphoreName << ")" << endl;
#endif
        }
        else
        {
          perror("writing SHM : sem_close");
        }

      } //(mutex != SEM_FAILED)
      else
      {
        perror("writing SHM : unable to open/create the semaphore");

        sem_unlink(SemaphoreName.c_str());
      }

    } //(shmid <= 0)
    else
    {
      *_PF << Offset << "(ERROR: Unable to determine shared memory ID in operating system)" << endl;
    }

    delete Values;

    if (Status == OK)
    {
      break;
    }
  }

  // S << Offset << "(Finished)"<< endl;
  return Status;
}

long long int PG::OpenHeaderMessageSizeField(unsigned char* _Buffer)
{
  long long MessageSize = 0;

  unsigned long long A = _Buffer[8];
  unsigned long long B = _Buffer[9];
  unsigned long long C = _Buffer[10];
  unsigned long long D = _Buffer[11];
  unsigned long long E = _Buffer[12];
  unsigned long long F = _Buffer[13];
  unsigned long long G = _Buffer[14];
  unsigned long long H = _Buffer[15];

  // cout << "A = " <<A<<endl;
  // cout << "B = " <<B<<endl;
  // cout << "C = " <<C<<endl;
  // cout << "D = " <<D<<endl;
  // cout << "E = " <<E<<endl;
  // cout << "F = " <<F<<endl;
  // cout << "G = " <<G<<endl;
  // cout << "H = " <<H<<endl;

  MessageSize = (A << 56) |
                (B << 48) |
                (C << 40) |
                (D << 32) |
                (E << 24) |
                (F << 16) |
                (G << 8) |
                H;

  return MessageSize;
}

void PG::OpenHeaderSegmentationField(unsigned char* _Buffer, unsigned int& _MessageNumber, unsigned int& _SequenceNumber)
{
  unsigned int A = _Buffer[0];
  unsigned int B = _Buffer[1];
  unsigned int C = _Buffer[2];
  unsigned int D = _Buffer[3];
  unsigned int E = _Buffer[4];
  unsigned int F = _Buffer[5];
  unsigned int G = _Buffer[6];
  unsigned int H = _Buffer[7];

  // cout << "A = " <<A<<endl;
  // cout << "B = " <<B<<endl;
  // cout << "C = " <<C<<endl;
  // cout << "D = " <<D<<endl;
  // cout << "E = " <<E<<endl;
  // cout << "F = " <<F<<endl;
  // cout << "G = " <<G<<endl;
  // cout << "H = " <<H<<endl;

  _MessageNumber = (A << 24) |
                   (B << 16) |
                   (C << 8) |
                   D;

  _SequenceNumber = (E << 24) |
                    (F << 16) |
                    (G << 8) |
                    H;
}

// Allocate and add an Action on Actions container
void PG::NewAction(const string _LN, Action*& _PA)
{
  if (_LN == "-m --cl 0.1")
  {
    PGMsgCl01* P = new PGMsgCl01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --initialization 0.1")
  {
    PGRunInitialization01* P = new PGRunInitialization01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-hello --ihc 0.1")
  {
    PGHelloIHC01* P = new PGHelloIHC01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-hello --ihc 0.2")
  {
    PGHelloIHC02* P = new PGHelloIHC02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-hello --ihc 0.3")
  {
    PGHelloIHC03* P = new PGHelloIHC03(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --exposition 0.1")
  {
    PGRunExposition01* P = new PGRunExposition01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --hello 0.1")
  {
    PGRunHello01* P = new PGRunHello01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --hello 0.2")
  {
    PGRunHello02* P = new PGRunHello02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --hello 0.3")
  {
    PGRunHello03* P = new PGRunHello03(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --publishing 0.1")
  {
    PGRunPublishing01* P = new PGRunPublishing01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --periodic 0.1")
  {
    PGRunPeriodic01* P = new PGRunPeriodic01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --stresstest 0.1")
  {
    PGRunStresstest01* P = new PGRunStresstest01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-stresstest --ping 0.1")
  {
    PGStresstestPing01* P = new PGStresstestPing01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }
}

// Wrapper function for EthernetWiFiSocketDispatcher() thread
void PG::EthernetWiFiSocketDispatcherThreadWrapper(void* _PPG)
{
  PG* PPG = static_cast<PG*>(_PPG);
  PPG->SocketDispatcher3();
}

void PG::Hex2Char(char* szHex, unsigned char& rch)
{
  rch = 0;
  for (int i = 0; i < 2; i++)
  {
    if (*(szHex + i) >= '0' && *(szHex + i) <= '9')
      rch = (rch << 4) + (*(szHex + i) - '0');
    else if (*(szHex + i) >= 'a' && *(szHex + i) <= 'f')
      rch = static_cast<unsigned char>((rch << 4) + (*(szHex + i) - 'a' + 10));
    else
      break;
  }
}


// Wrapper function for NGAL_Transport_RAW::ReceiveDispatcher() thread
void PG::ReceiveDispatcherWrapper(void* _PPG)
{
  PG* PPG = static_cast<PG*>(_PPG);
  NGAL_Transport_RAW::ReceiveDispatcher(PPG);
}
