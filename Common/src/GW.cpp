/*
        NovaGenesis

        Name:		Gateway
        Object:		GW
        File:		GW.cpp
        Author:		Antonio Marcos Alberti
        Date:		05/2021
        Version:	0.2

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
#include "GW.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _GWMSGCL01_H
#include "GWMsgCl01.h"
#endif

#ifndef _GWRUNINITIALIZATION01_H
#include "GWRunInitialization01.h"
#endif

#ifndef _GWHelloIPC02_H
#include "GWHelloIPC02.h"
#endif

#ifndef _GWRUNHELLOIPC02_H
#include "GWRunHelloIPC02.h"
#endif

#ifndef _GWEXPOSITION02_H
#include "GWExposition02.h"
#endif

#define DEBUG // To follow message processing

#define LG(msg) S << endl \
                  << "[" << fixed << setprecision(3) << GetTime() << "s]        " << msg << endl
// #define DEBUG1  // To follow shared memory access
// #define DEBUG2  // More on shm access
// #define DEBUG3 // Even more on shm access

union semun
{
  int val;              /* used for SETVAL only */
  struct semid_ds* buf; /* for IPC_STAT and IPC_SET, not discussed here */
  ushort* array;        /* used for GETALL and SETALL */
};

GW::GW(string _LN, Process* _PP, unsigned int _Index, string _Path)
    : Block(_LN, _PP, _Index, _Path)
{
  InputQueueTag = 0;
  OutputQueueTag = 0;
  StopGateway = false;
  State = "initialization";
  ScheduleStatusFlag = false;
  MaxSegmentSize = MAX_MESSAGE_SIZE;

  // Setting the delays
  DelayBeforeStatusIPC = 5;

  // Initialize output message notification flag
  NewOutputMessage = false;

  // SPEC-006: Named semaphore "Output_Queue" removed from constructor.
  // OutputQueueMutex alone protects OutputQueues.

  Action* PA = 0;
  CommandLine* PCL = 0;
  Block* PHTB = 0;
  Message* PIM = 0;
  Message* InlineResponseMessage = NULL;

  PP->GetBlock("HT", PHTB);

  PHT = (HT*)PHTB;

  // Creating the actions
  NewAction("-run --initialization 0.1", PA);
  NewAction("-m --cl 0.1", PA);
  NewAction("-hello --ipc 0.2", PA);      // GWHelloIPC02 - receiver (replaces GWHelloIPC01)
  NewAction("-hello --ipc 2.0", PA);      // GWHelloIPC02 - receiver with HT BID support
  NewAction("-run --helloIPC 0.2", PA);   // GWRunHelloIPC02 - periodic emitter
  NewAction("-exposition 0.2", PA);       // GWExposition02 - periodic peer exposition on PGCS
  NewAction("-run --exposition 0.2", PA); // GWExposition02 - periodic trigger

  // Creating a -run --initialization message
  PP->NewMessage(GetTime(), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine("-run", "--initialization", "0.1", PCL);

  // Run
  Run(PIM, InlineResponseMessage);

  // Mark to delete
  PIM->MarkToDelete();
}

GW::~GW()
{

  vector<Action*>::iterator it4;

  Action* Temp;

  for (it4 = Actions.begin(); it4 != Actions.end(); it4++)
  {
    Temp = *it4;

    if (Temp != 0)
    {
      delete Temp;
    }

    Temp = 0;
  }
}

// Allocate and add an Action on Actions container
void GW::NewAction(const string _LN, Action*& _PA)
{
  if (_LN == "-m --cl 0.1")
  {
    GWMsgCl01* P = new GWMsgCl01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --initialization 0.1")
  {
    GWRunInitialization01* P = new GWRunInitialization01(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-hello --ipc 0.2")
  {
    GWHelloIPC02* P = new GWHelloIPC02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-hello --ipc 2.0")
  {
    GWHelloIPC02* P = new GWHelloIPC02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --helloIPC 0.2")
  {
    GWRunHelloIPC02* P = new GWRunHelloIPC02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-exposition 0.2")
  {
    GWExposition02* P = new GWExposition02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }

  if (_LN == "-run --exposition 0.2")
  {
    GWExposition02* P = new GWExposition02(_LN, this, PP->PMB);

    Actions.push_back((Action*)P);
  }
}

// Get an Action
int GW::GetAction(string _LN, Action*& _PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int GW::DeleteAction(string _LN)
{
  int Status = ERROR;
  return Status;
}

// Push a Message into the input message priority queue
// Be aware to lock Input queue mutex (IQM) before accessing this function
void GW::PushToInputQueue(Message* M)
{
  unsigned int NoCL = 0;

#ifdef DEBUG

  S << "[1]       (Pushing the following message to InputQueue. Size = " << InputQueue.size()
    << ". Message instantiation number is " << M->InstantiationNumber << ")" << endl;

  S << "(" << endl
    << *M << ")" << endl;

#endif

  if (M != 0)
  {
    if (M->GetNumberofCommandLines(NoCL) == OK)
    {
      if (NoCL > 2)
      {
        // Avoid deleting the message
        M->UnmarkToDelete();

        // Set the message tag
        M->SetTag(InputQueueTag);

        // Lock for push + notify (thread-safe w.r.t. Gateway() pop)
        {
          std::lock_guard<std::mutex> lock(InputQueueMutex);

          // Push the message to the queue
          InputQueue.push(M);

          // Increases the tag counter
          InputQueueTag++;
        }
        // Notify after unlock to minimize time input thread is blocked
        InputQueueCV.notify_one();
      }
      else
      {
        // Mark to delete the message
        M->MarkToDelete();
      }
    }
    else
    {
      S << "          (ERROR: Unable to read the number of command lines at input queue)" << endl;
      M->MarkToDelete();
    }
  }
  else
  {
    S << "          (ERROR: The message being store in input queue is corrupted at input queue)" << endl;
    M->MarkToDelete();
  }
}

// Push a Message into the output message priority queue
void GW::PushToOutputQueue(std::string OQS, Message* M)
{
  unsigned int NoCL = 0;
  size_t MaxSegmentSize = MAX_MESSAGE_SIZE;
  long long MessageSize = 0;
  string Offset = "          ";

#ifdef DEBUG
  S << "[6]       (Pushing the following message to the OutputQueue " << OQS << ". Size = " << OutputQueues[OQS].size() << ")" << endl;
  S << "(" << endl
    << *M << ")" << endl;
#endif

  if (M != 0)
  {
    M->ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray();

    if (M->GetMessageSize(MessageSize) == OK)
    {
      if (MessageSize > 0 && MessageSize <= (MaxSegmentSize - 8))
      {
        if (M->GetNumberofCommandLines(NoCL) == OK)
        {
          if (NoCL > 2)
          {
            M->UnmarkToDelete();
            M->SetTag(OutputQueueTag);

            // Lock for push + notify (thread-safe w.r.t. ReadFromOutputQueue)
            {
              std::lock_guard<std::mutex> lock(OutputQueueMutex);
              OutputQueues[OQS].push(M);
              OutputQueueTag++;
              NewOutputMessage = true;
            }
            OutputQueueCV.notify_one();
          }
          else
          {
            S << Offset << "(ERROR: The message has less than 3 command lines at output queue)" << endl;
            M->MarkToDelete();
          }
        }
        else
        {
          S << Offset << "(ERROR: Unable to read the number of command lines at output queue)" << endl;
          M->MarkToDelete();
        }
      }
      else
      {
        S << Offset << "(ERROR: Invalid message size at output queue)" << endl;
        M->MarkToDelete();
      }
    }
    else
    {
      S << Offset << "(ERROR: Unable to get the message size at output queue)" << endl;
      M->MarkToDelete();
    }
  }
  else
  {
    S << "          (ERROR: The message being store in output queue is corrupted)" << endl;
    M->MarkToDelete();
  }
}

// Read a Message from output message priority queues. Only the GW can forward messages to shared memory instances
// SPEC-007: Decouple WriteToSharedMemory3 from OutputQueueMutex. Pop at most
// one message per queue into a local batch, write each to SHM outside the lock,
// re-push failures under lock. Re-evaluate NewOutputMessage after partial drain
// to prevent the output thread from sleeping with pending messages.
void GW::ReadFromOutputQueue()
{
  Message* PM1 = NULL;
  std::string currentOQS;

  while (StopGateway == false)
  {
    // Wait for output messages or stop flag (blocking wait — zero CPU when idle)
    {
      std::unique_lock<std::mutex> lock(OutputQueueMutex);
      OutputQueueCV.wait(lock, [this]()
                         { return NewOutputMessage || StopGateway; });
      if (StopGateway)
        break;
      NewOutputMessage = false;
    }

    // Phase 1: Pop at most one message per queue (under lock — microseconds)
    std::vector<std::pair<std::string, Message*>> batch;
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);

      for (auto it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        if (!it->second.empty())
        {
          PM1 = it->second.top();
          it->second.pop();
          batch.emplace_back(it->first, PM1);
        }
      }
    } // Lock released

    if (batch.empty())
      continue;

    // Phase 2: Write each message to SHM (OUTSIDE the lock)
    bool anyFailed = false;

    for (auto& kv : batch)
    {
      if (WriteToSharedMemory3(kv.first, kv.second) == OK)
      {
        kv.second->MarkToDelete();
      }
      else
      {
        // SHM busy — re-push under lock for retry on next cycle
        {
          std::lock_guard<std::mutex> qlock(OutputQueueMutex);
          OutputQueues[kv.first].push(kv.second);
        }
        anyFailed = true;
      }
    }

    // Re-evaluate NewOutputMessage: check if queues still have work
    // (critical: without this, the output thread could sleep with pending
    // messages because NewOutputMessage was cleared before the cycle started)
    {
      std::lock_guard<std::mutex> qlock(OutputQueueMutex);
      bool hasMoreWork = false;
      for (auto it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        if (!it->second.empty())
        {
          hasMoreWork = true;
          break;
        }
      }
      NewOutputMessage = hasMoreWork;
    }

    // Backpressure: brief sleep if any message failed (peer SHM still busy)
    if (anyFailed)
    {
      tthread::this_thread::sleep_for(tthread::chrono::milliseconds(1));
    }
  }
}

// Read Messages from the queues and forward to other processes via IPC/IHC
void GW::Gateway()
{
  Message* PM1 = NULL;
  Message* PM2 = NULL;
  bool RunFlag = false;
  unsigned int NumberOfCycles = 0;
  string Offset = "          ";
  double ScheduledTime = 0;
  double Time = 0;
  long long int MessageSize = 0;
  std::chrono::milliseconds waitTimeout;
  constexpr long long SHM_POLL_INTERVAL_MS = 1; // SHM poll every 1ms (SPEC-007a)
  double secondsUntilNext = 1.0;                // default 1s when queue empty

  // Start output queue thread
  tthread::thread* T = new tthread::thread(&GW::ReadFromOutputQueueThreadWrapper, this);

  while (StopGateway == false)
  {
    // Calculate how long to wait: time until next scheduled message
    {
      std::lock_guard<std::mutex> lock(InputQueueMutex);
      if (!InputQueue.empty())
      {
        double nextTime = InputQueue.top()->GetTime();
        double now = GetTime();
        secondsUntilNext = nextTime - now;
        if (secondsUntilNext < 0)
          secondsUntilNext = 0;
      }
      else
      {
        secondsUntilNext = 1.0; // no messages, check SHM every 1s max
      }
    }

    // Wait for input queue or stop flag (timer-aware blocking wait)
    {
      std::unique_lock<std::mutex> lock(InputQueueMutex);
      waitTimeout = std::chrono::milliseconds(
          std::min((long long)(secondsUntilNext * 1000), SHM_POLL_INTERVAL_MS));
      InputQueueCV.wait_for(lock, waitTimeout,
                            [this]()
                            { return !InputQueue.empty() || StopGateway; });
      if (StopGateway)
        break;
    }

    // Lock, pop all due messages, unlock — minimize critical section
    PM1 = NULL;
    RunFlag = false;
    Time = GetTime();

    {
      std::lock_guard<std::mutex> lock(InputQueueMutex);
      if (!InputQueue.empty())
      {
        PM1 = InputQueue.top();
        ScheduledTime = PM1->GetTime();
        if (ScheduledTime < Time)
        {
          InputQueue.pop();
          RunFlag = true;
        }
        else
        {
          PM1 = NULL; // not yet due, leave in queue
        }
      }
    }

    // Step 2 : Run procedure to interpret and run the received message
    if (PM1 != NULL && RunFlag == true)
    {

#ifdef DEBUG

      S << endl
        << setprecision(10) << "          (t = " << Time << ")" << endl;
      S << "[1]       (IQ Size = " << InputQueue.size() << ")" << endl;

      map<std::string, priority_queue<Message*, vector<Message*>, DereferenceCompareNode>>::iterator it;
      unsigned int Counter = 0;
      for (it = OutputQueues.begin(); it != OutputQueues.end(); it++)
      {
        Counter++;
        S << "[6]       (OQ[" << Counter << "] Size = " << it->second.size() << ")" << endl;
      }
      S << Offset << "(Messages in memory = " << PP->GetNumberOfMessages() << ")" << endl;

      S << "[7]       Current peer processes on this OS are:" << endl;

      // List peers from category 20 (hello IPC discovered peers)

      vector<string> Cat20Keys;
      if (PHT->GetBindingKeys(20, Cat20Keys) == OK)
      {
        for (unsigned int i = 0; i < Cat20Keys.size(); i++)
        {
          vector<string>* Cat20Vals = new vector<string>;
          if (PHT->GetBinding(20, Cat20Keys.at(i), Cat20Vals) == OK)
          {
            S << "            Cat[20] " << Cat20Keys.at(i);
            for (unsigned int j = 0; j < Cat20Vals->size(); j++)
            {
              S << " -> " << Cat20Vals->at(j);
            }
            S << endl;
          }
          delete Cat20Vals;
        }
      }

      // List peers from category 19 (hello IPC discovered peers)

      vector<string> Cat19Keys;
      if (PHT->GetBindingKeys(19, Cat19Keys) == OK)
      {
        for (unsigned int j = 0; j < Cat19Keys.size(); j++)
        {
          vector<string>* Cat19Vals = new vector<string>;
          if (PHT->GetBinding(19, Cat19Keys.at(j), Cat19Vals) == OK)
          {
            S << "            Cat[19] " << Cat19Keys.at(j);
            for (unsigned int k = 0; k < Cat19Vals->size(); k++)
            {
              S << " -> " << Cat19Vals->at(k);
            }
            S << endl;
          }
          delete Cat19Vals;
        }
      }

      // List peers from category 15 (hello IHC discovered peers)

      vector<string> Cat15Keys;
      if (PHT->GetBindingKeys(15, Cat15Keys) == OK)
      {
        for (unsigned int j = 0; j < Cat15Keys.size(); j++)
        {
          vector<string>* Cat15Vals = new vector<string>;
          if (PHT->GetBinding(15, Cat15Keys.at(j), Cat15Vals) == OK)
          {
            S << "            Cat[15] " << Cat15Keys.at(j);
            for (unsigned int k = 0; k < Cat15Vals->size(); k++)
            {
              S << " -> " << Cat15Vals->at(k);
            }
            S << endl;
          }
          delete Cat15Vals;
        }
      }

#endif

      Run(PM1, PM2);

      PP->DeleteMarkedMessages();

      PM1 = NULL;
      RunFlag = false;
    }

    // Step 3 : NGAL: drain the NetworkReceiveQueue
    // The ReceiveDispatcher pushes raw char buffers here. The GW thread
    // does NewMessage + deserialisation + PushToInputQueue.
    {
      std::lock_guard<std::mutex> lock(NetworkReceiveQueueMutex);
      while (!NetworkReceiveQueue.empty())
      {
        auto entry = NetworkReceiveQueue.front();
        NetworkReceiveQueue.pop();

        char* buffer = entry.first;
        long long size = entry.second;

        if (buffer != 0 && size > 0)
        {
          Message* PM = NULL;
          if (PP->NewMessage(0, 0, false, PM) == OK)
          {
            PM->SetMessageFromCharArray(buffer, size);
            PM->ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();
            PushToInputQueue(PM);
          }
        }

        delete[] buffer;
      }
    }

    // Step 4 : Read OS IPC — poll shared memory for messages from other processes
    // Phase 2: rate limit to 100ms when no due messages to process (idle or waiting for future messages)
    {
      static auto lastSHMPoll = std::chrono::steady_clock::now();
      auto now = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSHMPoll).count();
      bool hasDueMessage = false;
      {
        std::lock_guard<std::mutex> lock(InputQueueMutex);
        if (!InputQueue.empty())
        {
          double nextTime = InputQueue.top()->GetTime();
          double currentTime = GetTime();
          if (nextTime <= currentTime)
            hasDueMessage = true;
        }
      }
      if (!hasDueMessage && elapsed >= SHM_POLL_INTERVAL_MS)
      {
        ReadFromSharedMemory3();
        lastSHMPoll = std::chrono::steady_clock::now();
      }
    }

    NumberOfCycles++;
  }

  // Stop the output thread
  if (StopGateway == false)
  {
    StopGateway = true;
    OutputQueueCV.notify_all();
  }

  T->join();
  delete T;
}

int GW::ReadFromSharedMemory3()
{
  int Status = ERROR;
  string Offset = "          ";
  double StartingTime;

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
       z++) // Modified in 9th April 2021 to deal with parallel shared memories.
  {
    sem_t* mutex;
    void* shm_address;
    unsigned char* data;     // Manipulate the content on shared memory
    long long TotalSize = 0; // The total size of the message being transferred
    // The shared memory segment address
    string SemaphoreName;

    // Set the semaphore name
    SemaphoreName = IntToString(
        PP->Key + z); // Modified in 9th April 2021 to deal with parallel shared memories.

    // Cache semaphore to avoid sem_open/sem_close per iteration (Phase 2)
    mutex = CachedSemaphores[SemaphoreName];
    if (mutex == NULL)
    {
      mutex = sem_open(SemaphoreName.c_str(), O_CREAT, 0666, 1);
      if (mutex != SEM_FAILED)
      {
        CachedSemaphores[SemaphoreName] = mutex;
      }
    }

    // Check for error on semaphore open
    if (mutex != NULL && mutex != SEM_FAILED)
    {

#ifdef DEBUG3

      S << Offset << "(Opened the semaphore " << SemaphoreName << ")" << endl;

#endif

      if (PP->shmid[z] == -1)
      { // Modified in 9th April 2021 to deal with parallel shared memories.
        // Create and connect to the segment
        if ((PP->shmid[z] = shmget(PP->Key + z, MaxSegmentSize, IPC_CREAT | 0666)) < 0)
        { // Modified in 9th April 2021 to deal with parallel shared memories.
          perror("shmget");
          exit(1);
        }

        //(shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)

        LG("(Created an input shared memory segment with key = " << (PP->Key + z)
                                                                 << " and identifier = " << PP->shmid[z] << ")");
      }
      else
      {
        // Test to see if it worked
        // Modified in 9th April 2021 to deal with parallel shared memories.
        if ((shm_address = shmat(PP->shmid[z], NULL, 0)) != NULL)
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
#ifdef DEBUG3
            S << Offset << "(Locked the semaphore " << SemaphoreName << ")" << endl;
#endif

            // *****************************
            // Start receiving a new message
            // *****************************

            data = (unsigned char*)shm_address;

            // Test to see if it worked
            if (data != (unsigned char*)(-1))
            {
              if (data[0] == (unsigned char)'w')
              {
                StartingTime = GetTime();

#ifdef DEBUG1
                S << endl
                  << endl
                  << "          (Time = " << StartingTime << ")" << endl;

                S << "[0]       (Reading from shared memory)" << endl;

                S << "          (The value of the first byte in shared memory is " << (char)data[0]
                  << " (f = Free to write, w = still waiting for peer reading)" << endl;

                S << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

                unsigned char* HeaderSizeField = new unsigned char[sizeof(TotalSize)];

                for (int i = 0; i < 8; i++)
                {
                  HeaderSizeField[i] = data[i + 1];

                  // printf("%i %d %c \n",i,data[i],data[i]);
                }

                unsigned long long A = (unsigned long long)HeaderSizeField[0];
                unsigned long long B = (unsigned long long)HeaderSizeField[1];
                unsigned long long C = (unsigned long long)HeaderSizeField[2];
                unsigned long long D = (unsigned long long)HeaderSizeField[3];
                unsigned long long E = (unsigned long long)HeaderSizeField[4];
                unsigned long long F = (unsigned long long)HeaderSizeField[5];
                unsigned long long G = (unsigned long long)HeaderSizeField[6];
                unsigned long long H = (unsigned long long)HeaderSizeField[7];

                TotalSize = (A << 56) |
                            (B << 48) |
                            (C << 40) |
                            (D << 32) |
                            (E << 24) |
                            (F << 16) |
                            (G << 8) |
                            H;

                if (TotalSize > 0 && TotalSize < MAX_MESSAGE_SIZE)
                {
#ifdef DEBUG1
                  // S << Offset << "(TotalSize = "<<TotalSize<<")"<< endl;

                  // S << Offset << "A = " <<A<<endl;
                  // S << Offset << "B = " <<B<<endl;
                  // S << Offset << "C = " <<C<<endl;
                  // S << Offset << "D = " <<D<<endl;
                  // S << Offset << "E = " <<E<<endl;
                  // S << Offset << "G = " <<G<<endl;
                  // S << Offset << "H = " <<H<<endl;
#endif

                  char* Payload = new char[TotalSize - 8];

                  // Read all the message
                  for (int j = 8; j < TotalSize; j++)
                  {
                    Payload[j - 8] = (char)data[j + 1];
                  }

                  double TimeStamp = 0;

                  // Read the timestamp header
                  unsigned char* HeaderTimeStampField = new unsigned char[8];

                  for (int n = 0; n < 8; n++)
                  {
                    HeaderTimeStampField[n] = data[TotalSize + 1 + n];

                    // printf("%i %d %c \n",i,data[i],data[i]);
                  }

                  unsigned long long I = (unsigned long long)HeaderTimeStampField[0];
                  unsigned long long J = (unsigned long long)HeaderTimeStampField[1];
                  unsigned long long K = (unsigned long long)HeaderTimeStampField[2];
                  unsigned long long L = (unsigned long long)HeaderTimeStampField[3];
                  unsigned long long M = (unsigned long long)HeaderTimeStampField[4];
                  unsigned long long N = (unsigned long long)HeaderTimeStampField[5];
                  unsigned long long O = (unsigned long long)HeaderTimeStampField[6];
                  unsigned long long P = (unsigned long long)HeaderTimeStampField[7];

                  unsigned long long TempTimeStamp;

                  TempTimeStamp = (I << 56) |
                                  (J << 48) |
                                  (K << 40) |
                                  (L << 32) |
                                  (M << 24) |
                                  (N << 16) |
                                  (O << 8) |
                                  P;

                  TimeStamp = ((double)(TempTimeStamp)) / 10e9;

                  // Allocate a new Message object
                  Message* PM = 0;

                  PP->NewMessage(GetTime(), 0, false, PM);

                  PM->SetMessageFromCharArray(Payload, (TotalSize - 8));

                  PM->ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();

#ifdef DEBUG1
                  long long SS;
                  long long SP;
                  PM->GetMessageSize(SS);
                  PM->GetPayloadSize(SP);

                  S << Offset << "(The total size of the message is = " << TotalSize - 8 << ")" << endl;
                  S << Offset << "(The total size of the message is = " << SS << ")" << endl;
                  S << Offset << "(The total size of the payload content in message is = " << SP << ")" << endl;

                  if (TotalSize < 3000 && SP > 1)
                  {
                    S << Offset << "(Showing what is on the payload array. Limited to messages smaller than 3000 bytes)" << endl;

                    for (unsigned int l = 0; l < (TotalSize - 8); l++)
                    {
                      printf("%i %d %c \n", l, Payload[l], Payload[l]);
                    }

                    S << Offset << "(Shown)" << endl;
                  }
#endif

#ifdef DEBUG2
                  S << Offset << "(MessageSize = " << (TotalSize - 8) << ")" << endl;

                  S << Offset
                    << "(The following message was received from another process in the same OS)"
                    << endl;

                  S << "(" << endl
                    << *PM << endl
                    << ")" << endl;
#endif

                  // Pushing message to the queue
                  PushToInputQueue(PM);

                  delete[] HeaderTimeStampField;

                  delete[] Payload;

#ifdef DEBUG3
                  S << Offset << "(Freeing the shared memory to the peer)" << endl;
#endif

                  // Clean the memory segment
                  memset(data, (unsigned char)'f', 1);

                  // S << Offset << "(Finished reading)"<< endl;

                  // S << Offset << "(Time = "<<GetTime()<<")"<<endl;

                  Status = OK;

                } // (TotalSize < 0 && TotalSize > 1073741816) means that the message is with error or misconfigured
                else
                {
                  // Total size is negative or zero
                  // Clean the memory segment
                  memset(data, (unsigned char)'f', MaxSegmentSize);
                }

                delete[] HeaderSizeField;
              }

            } // (data == (unsigned char *)(-1)) means that the data at the SHM was not set properly
            else
            {
              S << Offset << "(Warning: unable to read the data on the shared memory)" << endl;

              perror("reading SHM : shmat");
            }

            if (sem_post(mutex) == 0)
            {
#ifdef DEBUG3
              S << Offset << "(Unlocked the semaphore " << SemaphoreName << ")" << endl;
#endif
            }
            else
            {
              perror("reading SHM : sem_post");
            }

          } // (LockAttempts < 100) successfully locked

          // Detach from shared memory
          if (shmdt(shm_address) == -1)
          {
            perror("reading SHM : shmdt");
          }

        } // (shm_address = shmat(PP->shmid, NULL, 0)) == NULL) means it is unable to attach to the shared memory segment
        else
        {
          perror("reading SHM : shmat");
        }
      }

      // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)

    } // (mutex == SEM_FAILED) means it was unable to create/open the semaphore
    else
    {
      perror("reading SHM : unable to create/open semaphore");

      sem_unlink(SemaphoreName.c_str());
    }
  }

  return Status;
}

// Write to the shared memory
int GW::WriteToSharedMemory3(std::string OQS, Message* M)
{
  int Status = ERROR;
  unsigned int Category = 17;

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
       z++) // Modified in 9th April 2021 to deal with parallel shared memories.
  {
    long long MessageSize = 0; // The size of the Message (in bytes) being transferred
    long long TotalSize = 0;   // The total size of the message being transferred
    unsigned int NoCL = 0;
    unsigned char* data; // Manipulate the content on shared memory
    int shmid = -1;      // Shared memory ID on OS for a certain key_t (init to -1 for safety)
    void* shm_address;   // The shared memory segment address
    string Offset = "          ";
    sem_t* mutex;
    vector<string>* Values = new vector<string>;

    int _shm_key = StringToInt(OQS) + z;

    string _oqs = IntToString(_shm_key);

    if (GetHTBindingValues(Category, _oqs, Values) == OK)
    {
      // Assert the Values vector
      if (Values != 0)
      {
        if (!Values->empty())
        {
          // Carry the destination pointer
          shmid = StringToInt(Values->at(0));

#ifdef DEBUG2
          S << Offset << "(Shared memory ID (shmid) already on HT = " << shmid << ")" << endl;
#endif
        }
      }
    }
    else
    {
      if (ReturnIPCSHMID((key_t)_shm_key, shmid) == OK)
      {
        Values->push_back(IntToString(shmid));

        StoreHTBindingValues(Category, _oqs, Values);

        LG("(Storing the shared memory ID (shmid) = " << shmid << " on HT. The related key is "
                                                      << _oqs << ")");
      }
    }

    if (shmid != -1)
    {
      // Cache semaphore to avoid sem_open/sem_close per iteration (Phase 2)
      mutex = CachedSemaphores[_oqs];
      if (mutex == NULL)
      {
        mutex = sem_open(_oqs.c_str(), O_CREAT, 0666, 1);
        if (mutex != SEM_FAILED)
        {
          CachedSemaphores[_oqs] = mutex;
        }
      }

      // Check for error on semaphore open
      if (mutex != NULL && mutex != SEM_FAILED)
      {

        // ***************************************************************************************
        // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
        // ***************************************************************************************

#ifdef DEBUG3
        S << Offset << "(Opened the semaphore " << _oqs << ")" << endl;
#endif

        if ((shm_address = shmat(shmid, NULL, 0)) != NULL)
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

#ifdef DEBUG3
            S << Offset << "(Locked the semaphore " << _oqs << ")" << endl;
#endif

            data = (unsigned char*)shm_address;

            if (data != (unsigned char*)(-1))
            {
              if (data[0] != (unsigned char)'f' && data[0] != (unsigned char)'w')
              {
                data[0] = (unsigned char)'f';

                LG("(Initialized the shared memory with key " << _oqs
                                                              << " regarding the r/w control flag)");
              }
            }

#ifdef DEBUG3
            // S << "          (The value of the first byte in shared memory is "<<(char)data[0]<<" (f = Free to write, w = still waiting for peer reading)" << endl;
            S << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

            if (data[0] == (unsigned char)'f')
            {

#ifdef DEBUG2
              S << "[5]       (Writing a message to the shared memory segment with key = " << _oqs
                << " identified by " << shmid << ")" << endl;
#endif

              // Get the Message size
              if (M->GetMessageSize(MessageSize) == OK)
              {
                unsigned char* Payload;

                // Get the message in a char array format
                if (M->GetMessageFromCharArray(Payload) == OK)
                {
                  M->GetNumberofCommandLines(NoCL);

                  TotalSize = MessageSize + 8;

                  unsigned char* HeaderSizeField = new unsigned char[TotalSize];
                  unsigned char* PDU = new unsigned char[TotalSize];

                  HeaderSizeField[0] = (unsigned char)(TotalSize >> (8 * 7)) & 0xff;
                  HeaderSizeField[1] = (unsigned char)(TotalSize >> (8 * 6)) & 0xff;
                  HeaderSizeField[2] = (unsigned char)(TotalSize >> (8 * 5)) & 0xff;
                  HeaderSizeField[3] = (unsigned char)(TotalSize >> (8 * 4)) & 0xff;
                  HeaderSizeField[4] = (unsigned char)(TotalSize >> (8 * 3)) & 0xff;
                  HeaderSizeField[5] = (unsigned char)(TotalSize >> (8 * 2)) & 0xff;
                  HeaderSizeField[6] = (unsigned char)(TotalSize >> (8 * 1)) & 0xff;
                  HeaderSizeField[7] = (unsigned char)(TotalSize >> (8 * 0)) & 0xff;

                  for (long long j = 0; j < TotalSize; j++)
                  {
                    if (j < 8)
                    {
                      PDU[j] = HeaderSizeField[j];
                      // printf("%i %d %c \n",j,PDU[j],PDU[j]);
                    }

                    if (j >= 8 && j < (MessageSize + 8))
                    {
                      PDU[j] = (unsigned char)Payload[(j - 8)];
                      // printf("%i %d %c \n",j,PDU[j],PDU[j]);
                    }
                  }

                  if (NoCL > 2 && TotalSize <= (long long)MaxSegmentSize)
                  {

#ifdef DEBUG2
                    S << Offset << "(Sending the following message)" << endl;

                    S << "(" << endl
                      << *M << ")" << endl;

                    S << Offset << "(The size of the payload is = " << MessageSize << ")" << endl;

                    S << Offset << "(The size of the total message is = " << TotalSize << ")" << endl;

                    S << Offset << "(The size of the memory segment is = " << MaxSegmentSize << ")"
                      << endl;

                    // S << Offset << "(Make the shared memory unavailable while not read by the peer)"<< endl;
#endif

                    // Clean the memory segment before writing
                    // March 6th, 2021: changed from MaxSegmentSize to 1, since there is not need to clean more than this
                    memset(data, (unsigned char)'w', 1);

                    for (long long k = 1; k < TotalSize + 1; k++)
                    {
                      data[k] = PDU[k - 1];
                    }

                    // Send the timestap of this operation after the message

                    unsigned long long Time = (unsigned long long)(GetTime() * 10e9);

                    unsigned char* HeaderTimeStampField = new unsigned char[8];

                    HeaderTimeStampField[0] = (unsigned char)(Time >> (8 * 7)) & 0xff;
                    HeaderTimeStampField[1] = (unsigned char)(Time >> (8 * 6)) & 0xff;
                    HeaderTimeStampField[2] = (unsigned char)(Time >> (8 * 5)) & 0xff;
                    HeaderTimeStampField[3] = (unsigned char)(Time >> (8 * 4)) & 0xff;
                    HeaderTimeStampField[4] = (unsigned char)(Time >> (8 * 3)) & 0xff;
                    HeaderTimeStampField[5] = (unsigned char)(Time >> (8 * 2)) & 0xff;
                    HeaderTimeStampField[6] = (unsigned char)(Time >> (8 * 1)) & 0xff;
                    HeaderTimeStampField[7] = (unsigned char)(Time >> (8 * 0)) & 0xff;

                    for (unsigned int n = 0; n < 8; n++)
                    {
                      data[TotalSize + 1 + n] = HeaderTimeStampField[n];
                    }

                    delete[] HeaderTimeStampField;

                    Status = OK;
                  }
                  else
                  {
                    S << Offset
                      << "(ERROR: The message is too big for the memory segment or it has less than 3 command lines)"
                      << endl;

                    // Mark to delete the message. Added in March 7th, 2021
                    M->MarkToDelete();
                  }

                  delete[] PDU;
                  delete[] HeaderSizeField;
                }
                else
                {
                  S << Offset << "(ERROR: Unable to get message from char array)" << endl;
                }
              }

            } // (data[0] != (unsigned char)'f') means the memory is not free to write, despite of locking the semaphore.
            else
            {
#ifdef DEBUG1
              S << Offset << "(Warning: The peer behind shmid " << shmid << " and key " << _oqs
                << " still does not read the shared memory. Trying again later)" << endl;
#endif

              Status = ERROR;

              // Sleep before trying to send in the shared memory again
              tthread::this_thread::sleep_for(tthread::chrono::microseconds(5));
            }

            if (sem_post(mutex) == 0) // Successfully locked the semaphore, so unlock it
            {
#ifdef DEBUG3
              S << Offset << "(Unlocked the semaphore " << _oqs << ")" << endl;
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
            perror("writing SHM : unable to lock the semaphore");
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

        // Semaphore kept open in CachedSemaphores (Phase 2: no sem_close per iteration)

      } //(mutex != SEM_FAILED)
      else
      {
        perror("writing SHM : unable to open/create the semaphore");

        sem_unlink(_oqs.c_str());
      }

    } //(shmid <= 0)
    else
    {
      S << Offset << "(ERROR: Unable to determine shared memory ID in operating system)" << endl;
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

// First attachment to learned shared memory
int GW::ReturnIPCSHMID(key_t _Key, int& _shmid)
{
  // Initialize to error value to prevent undefined behavior (AMD vs Intel divergence)
  _shmid = -1;

  // GW		*PGW=0;
  string Offset = "          ";
  string SemaphoreName;
  sem_t* mutex;
  int Status = ERROR;

  // Set the semaphore name
  SemaphoreName = IntToString(_Key);

  mutex = sem_open(SemaphoreName.c_str(), O_CREAT, 0666, 1);

  // Check for error on semaphore open
  if (mutex != SEM_FAILED)
  {
    // Connect to the segment
    if ((_shmid = shmget(_Key, MaxSegmentSize, IPC_CREAT | 0666)) != -1)
    {
      Status = OK;
    }
    else
    {
      perror("shmat");

      S << Offset << "(ERROR: Unable to get the shared memory segment identified by the key = " << _Key << ")"
        << endl;
    }
  }
  else
  {
    perror("While initiating the SHM: unable to create semaphore");

    sem_unlink(SemaphoreName.c_str());

    S << Offset << "(ERROR: Unable to create the semaphore while discovering the peer shmid)" << endl;
  }

  return Status;
}

// Auxiliary functions
void GW::SetStopGatewayFlag(bool _F)
{
  StopGateway = _F;
  // Wake up both threads blocked on wait()
  InputQueueCV.notify_all();
  OutputQueueCV.notify_all();
}

// Set a value behind a key
int GW::StoreHTBindingValues(unsigned int _Category, string _Key, vector<string>* _Values)
{
  int Status = ERROR;

  if (PHT->StoreBinding(_Category, _Key, _Values) == OK)
  {
    Status = OK;
  }

  return Status;
}

// Discover the values behind a key
int GW::GetHTBindingValues(unsigned int _Category, string _Key, vector<string>*& _Values)
{
  int Status = ERROR;

  if (PHT->GetBinding(_Category, _Key, _Values) == OK)
  {
    Status = OK;
  }

  return Status;
}

// Wrapper function for ReadFromOutputQueue() thread
void GW::ReadFromOutputQueueThreadWrapper(void* _PGW)
{
  GW* P = static_cast<GW*>(_PGW);

  P->ReadFromOutputQueue();
}
