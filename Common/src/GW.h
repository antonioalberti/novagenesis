/*
	NovaGenesis

	Name:		Gateway
	Object:		GW
	File:		GW.h
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.1

   	Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _GW_H
#define _GW_H

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _GLIBCXX_IOMANIP
#include <iomanip>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _QUEUE_H
#include <queue>
#endif

#ifndef _SYS_MSG_H
#include <sys/msg.h>
#endif

#ifndef _SYS_STAT_H
#include <sys/stat.h>
#endif

#ifndef _SYS_SHM_H
#include <sys/shm.h>
#endif

#ifndef _SYS_IPC_H
#include <sys/ipc.h>
#endif

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _GLIBCXX_FUNCTIONAL
#include <functional>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_SEM_H
#include <sys/sem.h>
#endif

#ifndef _SEMAPHORE_H
#include <semaphore.h>
#endif

#ifndef __OUTPUTVARIABLE_H
#include "OutputVariable.h"
#endif

#ifndef __FCNTL_H
#include <fcntl.h>
#endif

#ifndef __MAP_H
#include <map>
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#define ERROR 1
#define OK 0

using namespace std;
using namespace tthread;

struct DereferenceCompareNode : public binary_function<const Message *, const Message *, bool> {
  bool operator() (const Message *M1, const Message *M2) const
  {
	if (M1->GetTime () != M2->GetTime ())
	  {
		return M1->GetTime () > M2->GetTime ();
	  }
	else
	  {
		return M1->GetTag () > M2->GetTag ();
	  }
  }
};

class HT;

class GW : public Block {
 private:

  // ------------------------------------------------------------------------------------------------------------------------------
  // Priority queues
  // ------------------------------------------------------------------------------------------------------------------------------

  // Input message queue
  priority_queue<Message *, vector<Message *>, DereferenceCompareNode> InputQueue;

  // Per key output message queue
  map<std::string, priority_queue<Message *, vector<Message *>, DereferenceCompareNode> > OutputQueues;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary variables
  // ------------------------------------------------------------------------------------------------------------------------------

  // Auxiliary tag for messages stored on input queue
  unsigned int InputQueueTag;

  // Auxiliary tag for messages stored on output queue
  unsigned int OutputQueueTag;

  // Stop gateway flag
  bool StopGateway;

  // Pointer to the HT block
  HT *PHT;

  // ------------------------------------------------------------------------------------------------------------------------------
  // OS Shared Memory IPC functions
  // ------------------------------------------------------------------------------------------------------------------------------

  int ReadFromSharedMemory3 ();

  // Write to the shared memory
  int WriteToSharedMemory3 (std::string OQS, Message *M);

  // First attachment to learned shared memory
  int ReturnIPCSHMID (key_t _Key, int &_shmid);

  // Sent messages counter
  unsigned long MessagesSent;

  // Received messages counter
  unsigned long MessagesReceived;

 public:

  // Define the maximum segment size on shared memory
  size_t MaxSegmentSize;

  // Schedule status flag
  bool ScheduleStatusFlag;

  // Constructor
  GW (string _LN, Process *_PP, unsigned int _Index, string _Path);

  // Destructor
  ~GW ();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Action related functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Allocate and add an Action on Actions container
  void NewAction (const string _LN, Action *&_PA);

  // Get an Action
  int GetAction (string _LN, Action *&_PA);

  // Delete an Action
  int DeleteAction (string _LN);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Core public functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Push a Message into the input message priority queue
  void PushToInputQueue (Message *M);

  // Monitor IPC, read Messages from other processes and pushes to the InputQueue
  // Read Messages from the InputQueue and call the GW::Run(Message *M)
  // Read Messages from the OutputQueue and forward to other processes via IPC
  void Gateway ();

  // Push a Message into the output message priority queue. Only the GW can push messages at the output queue
  void PushToOutputQueue (std::string OQS, Message *M);

  // Read a Message from output message priority queues. Only the GW can forward messages to shared memory instances
  void ReadFromOutputQueue ();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary functions
  // ------------------------------------------------------------------------------------------------------------------------------

  void SetStopGatewayFlag (bool _F);

  // Get the values behind a key
  int GetHTBindingValues (unsigned int _Category, string _Key, vector<string> *&_Values);

  // Set a value behind a key
  int StoreHTBindingValues (unsigned int _Category, string _Key, vector<string> *_Values);

  // Wrapper function for ReadFromOutputQueue() thread
  static void ReadFromOutputQueueThreadWrapper (void *_PGW);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Delay parameters
  // ------------------------------------------------------------------------------------------------------------------------------
  double DelayBeforeStatusIPC;

  // Discrete timers
  double DiscreteTime;
  double PreviousDiscreteTime;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Statistic variables and functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // **************************************************
  // All messages statistics
  // **************************************************

  // Input system
  OutputVariable *twi;        // Waiting time (seconds/message)
  OutputVariable *wi;        // Queue occupation (messages)
  OutputVariable *tsi;        // Service time (seconds)
  OutputVariable *si;        // Service occupation (messages)
  OutputVariable *mii;        // Service rate (messages/second)
  OutputVariable *ri;        // Service rate (bytes/second)
  OutputVariable *li;        // Message length (bytes/message)

  // Output system
  OutputVariable *two;        // Waiting time (seconds/message)
  OutputVariable *wo;        // Queue occupation (messages)
  OutputVariable *tso;        // Service time (seconds)
  OutputVariable *so;        // Service occupation (messages)
  OutputVariable *mio;        // Service rate (messages/second)
  OutputVariable *ro;        // Service rate (bytes/second)
  OutputVariable *lo;        // Message length (bytes/message)

  // Transit in the shared memory
  OutputVariable *tshm;        // Service time (seconds/message)

  // Shared memory
  OutputVariable *tss;        // Service time (seconds/message)

  // **************************************************
  // Type 1 messages statistics
  // **************************************************

  // Input system
  OutputVariable *twi1;        // Waiting time (seconds/message)
  OutputVariable *wi1;        // Queue occupation (messages)
  OutputVariable *tsi1;        // Service time (seconds)
  OutputVariable *si1;        // Service occupation (messages)
  OutputVariable *mii1;        // Service rate (messages/second)
  OutputVariable *ri1;        // Service rate (bytes/second)
  OutputVariable *li1;        // Message length (bytes/message)

  // Output system
  OutputVariable *two1;        // Waiting time (seconds/message)
  OutputVariable *wo1;        // Queue occupation (messages)
  OutputVariable *tso1;        // Service time (seconds)
  OutputVariable *so1;        // Service occupation (messages)
  OutputVariable *mio1;        // Service rate (messages/second)
  OutputVariable *ro1;        // Service rate (bytes/second)
  OutputVariable *lo1;        // Message length (bytes/message)

  // Transit in the shared memory
  OutputVariable *tshm1;        // Service time (seconds/message)

  // Shared memory
  OutputVariable *tss1;        // Service time (seconds/message)

  void ResetStatistics ();

  void SamplingBeforePushingToInputQueue (Message *PM1);
  void SamplingAfterRemovingFromInputQueue (Message *PM1);
  void SamplingBeforePushingToOutputQueue (Message *PM1);
  void SamplingBeforeWritingToSHM (Message *PM1);
  void SamplingAfterSHMService (Message *PM1);
  void SamplingBeforeRun (Message *PM1);
  void SamplingAfterRun (Message *PM1);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Friend classes
  // ------------------------------------------------------------------------------------------------------------------------------
  friend class GWMsgCl01;
  friend class GWRunInitialization01;
  friend class GWHelloIPC01;
  friend class GWSCNAck01;
  friend class GWStatusS01;
  friend class GWStatusSCNS01;
  friend class Message;
};

#endif
