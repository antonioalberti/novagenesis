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

#ifndef _GWSTATUSS01_H
#include "GWStatusS01.h"
#endif

#ifndef _GWSTATUSSCNS01_H
#include "GWStatusSCNS01.h"
#endif

#ifndef _GWHELLOIPC01_H
#include "GWHelloIPC01.h"
#endif

#ifndef _GWSCNSEQ01_H
#include "GWSCNSeq01.h"
#endif

#ifndef _GWSCNACK01_H
#include "GWSCNAck01.h"
#endif


//#define STATISTICS
#define DEBUG // To follow message processing
//#define DEBUG1  // To follow shared memory access
//#define DEBUG2  // More on shm access
//#define DEBUG3 // Even more on shm access

union semun {
  int val; /* used for SETVAL only */
  struct semid_ds *buf; /* for IPC_STAT and IPC_SET, not discussed here */
  ushort *array; /* used for GETALL and SETALL */
};

GW::GW (string _LN, Process *_PP, unsigned int _Index, string _Path) : Block (_LN, _PP, _Index, _Path)
{
  InputQueueTag = 0;
  OutputQueueTag = 0;
  StopGateway = false;
  State = "initialization";
  ScheduleStatusFlag = false;
  PreviousDiscreteTime = 0;
  MaxSegmentSize = MAX_MESSAGE_SIZE;                            // The size of the shared memory segment

  // Setting the delays
  DelayBeforeStatusIPC = 2;

  Action *PA = 0;
  CommandLine *PCL = 0;
  Block *PHTB = 0;
  Message *PIM = 0;
  Message *InlineResponseMessage = NULL;

#ifdef STATISTICS

  // Allocating and configuring the statistics variables

  // **************************************************
  // All messages statistics
  // **************************************************
  twi=new OutputVariable(this);
  wi=new OutputVariable(this);
  tsi=new OutputVariable(this);
  si=new OutputVariable(this);
  mii=new OutputVariable(this);
  ri=new OutputVariable(this);
  li=new OutputVariable(this);
  two=new OutputVariable(this);
  wo=new OutputVariable(this);
  tso=new OutputVariable(this);
  so=new OutputVariable(this);
  mio=new OutputVariable(this);
  ro=new OutputVariable(this);
  lo=new OutputVariable(this);
  tss=new OutputVariable(this);
  tshm=new OutputVariable(this);

  // Setting the statistics variables
  twi->Initialization("twi","MEAN_ARITHMETIC","Input queue waiting time (seconds)",1);
  wi->Initialization("wi","MEAN_WEIGHTED","Input queue occupation (messages)",1);
  tsi->Initialization("tsi","MEAN_ARITHMETIC","Input server service time (seconds/message)",1);
  si->Initialization("si","MEAN_WEIGHTED","Input server occupation (messages)",1);
  mii->Initialization("mii","MEAN_ARITHMETIC","Input server service rate (messages/second)",1);
  ri->Initialization("ri","MEAN_ARITHMETIC","Input server service rate (bytes/second)",1);
  li->Initialization("li","MEAN_ARITHMETIC","Transit messages size (bytes)",1);

  two->Initialization("two","MEAN_ARITHMETIC","Output queue waiting time (seconds)",1);
  wo->Initialization("wo","MEAN_WEIGHTED","Output queue occupation (messages)",1);
  tso->Initialization("tso","MEAN_ARITHMETIC","Output server service time (seconds/message)",1);
  so->Initialization("so","MEAN_WEIGHTED","Output server occupation (messages)",1);
  mio->Initialization("mio","MEAN_ARITHMETIC","Output server service rate (messages/second)",1);
  ro->Initialization("ro","MEAN_ARITHMETIC","Output server service rate (bytes/second)",1);
  lo->Initialization("lo","MEAN_ARITHMETIC","Transit messages size (bytes)",1);

  tshm->Initialization("tshm","MEAN_ARITHMETIC","Time expend betweem write and read from shared memory (seconds)",1);

  tss->Initialization("tss","MEAN_ARITHMETIC","Delay to read a message from the shared memory to a message object (seconds)",1);

  // **************************************************
  // Type 1 messages statistics
  // **************************************************

  twi1=new OutputVariable(this);
  wi1=new OutputVariable(this);
  tsi1=new OutputVariable(this);
  si1=new OutputVariable(this);
  mii1=new OutputVariable(this);
  ri1=new OutputVariable(this);
  li1=new OutputVariable(this);
  two1=new OutputVariable(this);
  wo1=new OutputVariable(this);
  tso1=new OutputVariable(this);
  so1=new OutputVariable(this);
  mio1=new OutputVariable(this);
  ro1=new OutputVariable(this);
  lo1=new OutputVariable(this);
  tss1=new OutputVariable(this);
  tshm1=new OutputVariable(this);

  // Setting the statistics variables
  twi1->Initialization("twi1","MEAN_ARITHMETIC","Input queue waiting time (seconds)",1);
  wi1->Initialization("wi1","MEAN_WEIGHTED","Input queue occupation (messages)",1);
  tsi1->Initialization("tsi1","MEAN_ARITHMETIC","Input server service time (seconds/message)",1);
  si1->Initialization("si1","MEAN_WEIGHTED","Input server occupation (messages)",1);
  mii1->Initialization("mii1","MEAN_ARITHMETIC","Input server service rate (messages/second)",1);
  ri1->Initialization("ri1","MEAN_ARITHMETIC","Input server service rate (bytes/second)",1);
  li1->Initialization("li1","MEAN_ARITHMETIC","Transit messages size (bytes)",1);

  two1->Initialization("two1","MEAN_ARITHMETIC","Output queue waiting time (seconds)",1);
  wo1->Initialization("wo1","MEAN_WEIGHTED","Output queue occupation (messages)",1);
  tso1->Initialization("tso1","MEAN_ARITHMETIC","Output server service time (seconds/message)",1);
  so1->Initialization("so1","MEAN_WEIGHTED","Output server occupation (messages)",1);
  mio1->Initialization("mio1","MEAN_ARITHMETIC","Output server service rate (messages/second)",1);
  ro1->Initialization("ro1","MEAN_ARITHMETIC","Output server service rate (bytes/second)",1);
  lo1->Initialization("lo1","MEAN_ARITHMETIC","Transit messages size (bytes)",1);

  tshm1->Initialization("tshm1","MEAN_ARITHMETIC","Time expend betweem write and read from shared memory (seconds)",1);

  tss1->Initialization("tss1","MEAN_ARITHMETIC","Delay to read a message from the shared memory to a message object (seconds)",1);

#endif

  PP->GetBlock ("HT", PHTB);

  PHT = (HT *)PHTB;

  // Creating the actions
  NewAction ("-run --initialization 0.1", PA);
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-st --s 0.1", PA);
  NewAction ("-st --scns 0.1", PA);
  NewAction ("-hello --ipc 0.1", PA);
  NewAction ("-scn --seq 0.1", PA);
  NewAction ("-scn --ack 0.1", PA);

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialization", "0.1", PCL);

  // Run
  Run (PIM, InlineResponseMessage);

  // Mark to delete
  PIM->MarkToDelete ();
}

GW::~GW ()
{

#ifdef STATISTICS

  // Deallocating the statistics variables
  delete twi;
  delete wi;
  delete tsi;
  delete si;
  delete mii;
  delete ri;
  delete li;
  delete two;
  delete wo;
  delete tso;
  delete so;
  delete mio;
  delete ro;
  delete lo;
  delete tss;
  delete tshm;

  delete twi1;
  delete wi1;
  delete tsi1;
  delete si1;
  delete mii1;
  delete ri1;
  delete li1;
  delete two1;
  delete wo1;
  delete tso1;
  delete so1;
  delete mio1;
  delete ro1;
  delete lo1;
  delete tss1;
  delete tshm1;

#endif

  vector<Action *>::iterator it4;

  Action *Temp;

  for (it4 = Actions.begin (); it4 != Actions.end (); it4++)
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
void GW::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-m --cl 0.1")
	{
	  GWMsgCl01 *P = new GWMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --initialization 0.1")
	{
	  GWRunInitialization01 *P = new GWRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-st --s 0.1")
	{
	  GWStatusS01 *P = new GWStatusS01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-st --scns 0.1")
	{
	  GWStatusSCNS01 *P = new GWStatusSCNS01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-hello --ipc 0.1")
	{
	  GWHelloIPC01 *P = new GWHelloIPC01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --seq 0.1")
	{
	  GWSCNSeq01 *P = new GWSCNSeq01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --ack 0.1")
	{
	  GWSCNAck01 *P = new GWSCNAck01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int GW::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int GW::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}

// Push a Message into the input message priority queue
// Be aware to lock Input queue mutex (IQM) before accessing this function
void GW::PushToInputQueue (Message *M)
{
  unsigned int NoCL = 0;

#ifdef DEBUG

  S << "[1]       (Pushing a message to the InputQueue. Size = " << InputQueue.size ()
	<< ". Message instantiation number is " << M->InstantiationNumber << ")" << endl;

#endif

  if (M != 0)
	{
	  if (M->GetNumberofCommandLines (NoCL) == OK)
		{
		  if (NoCL > 2)
			{
			  // Avoid deleting the message
			  M->UnmarkToDelete ();

			  // Set the message tag
			  M->SetTag (InputQueueTag);

#ifdef STATISTICS

			  SamplingBeforePushingToInputQueue(M);

#endif

			  // Push the message to the queue
			  InputQueue.push (M);

			  // Increases the tag counter
			  InputQueueTag++;
			}
		  else
			{
			  //S <<"          (ERROR: The message has less than 3 command lines at input queue)" << endl;

			  // Mark to delete the message
			  M->MarkToDelete ();
			}
		}
	  else
		{
		  S << "          (ERROR: Unable to read the number of command lines at input queue)" << endl;

		  // Mark to delete the message
		  M->MarkToDelete ();
		}
	}
  else
	{
	  S << "          (ERROR: The message being store in input queue is corrupted at input queue)" << endl;

	  // Mark to delete the message
	  M->MarkToDelete ();
	}
}

// Push a Message into the output message priority queue
void GW::PushToOutputQueue (std::string OQS, Message *M)
{

#ifdef DEBUG

  S << "[6]       (Pushing the following message to the OutputQueue " << OQS << ". Size = " << OutputQueues[OQS].size ()
	<< ")" << endl;

  S << "(" << endl << *M << ")"
	<< endl;

#endif

  unsigned int NoCL = 0;
  size_t MaxSegmentSize = MAX_MESSAGE_SIZE;                            // The size of the shared memory segment
  long long MessageSize = 0;
  string Offset = "          ";

  if (M != 0)
	{
	  M->ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray ();

	  // Get the Message size
	  if (M->GetMessageSize (MessageSize) == OK)
		{
		  // Test to check if the Message is not empty and if it fits on the shm segment size
		  if (MessageSize > 0 && MessageSize <= (MaxSegmentSize - 8))
			{
			  if (M->GetNumberofCommandLines (NoCL) == OK)
				{
				  if (NoCL > 2)
					{
					  // Avoid deleting the message
					  M->UnmarkToDelete ();

					  // Set the message tag
					  M->SetTag (OutputQueueTag);

#ifdef STATISTICS

					  SamplingBeforePushingToOutputQueue(M);

#endif

					  //S << "          (The size of the OutputQueue is = "<<OutputQueue.size()<<")"<<endl;

					  // Push the message to the queue
					  OutputQueues[OQS].push (M);

					  //S << "          (The size of the OutputQueue is = "<<OutputQueue.size()<<")"<<endl;

					  // Increases the tag counter
					  OutputQueueTag++;

					}
				  else
					{
					  S << "          (ERROR: The message has less than 3 command lines at output queue)" << endl;

					  // Mark to delete the message
					  M->MarkToDelete ();
					}
				}
			  else
				{
				  S << "          (ERROR: Unable to read the number of command lines at output queue)" << endl;

				  // Mark to delete the message
				  M->MarkToDelete ();
				}

			}
		  else
			{
			  S << Offset << "(ERROR: Invalid message size at output queue)" << endl;

			  // Mark to delete the message
			  M->MarkToDelete ();
			}
		}
	  else
		{
		  S << Offset << "(ERROR: Unable to get the message size at output queue)" << endl;

		  // Mark to delete the message
		  M->MarkToDelete ();
		}
	}
  else
	{
	  S << "          (ERROR: The message being store in output queue is corrupted)" << endl;

	  // Mark to delete the message
	  M->MarkToDelete ();
	}
}

// Read a Message from output message priority queues. Only the GW can forward messages to shared memory instances
void GW::ReadFromOutputQueue ()
{
  Message *PM1 = NULL;
  sem_t *mutex;

  // ****************************************************************************
  // Step 3 : Read OutputQueues and Dispatch
  // ****************************************************************************

  // Modified in 11th April 2021 to deal with parallel shared memories.

  // Set the semaphore name
  string SemaphoreName = "Output_Queue";

  while (1)
	{
	  mutex = sem_open (SemaphoreName.c_str (), O_CREAT, 0666, 1);

	  // Check for error on semaphore open
	  if (mutex != SEM_FAILED)
		{
		  if (sem_trywait (mutex) == 0)
			{
			  map<std::string, priority_queue<Message *, vector<Message *>, DereferenceCompareNode> >::iterator it;

			  for (it = OutputQueues.begin (); it != OutputQueues.end (); it++)
				{
				  // Verify if there is a message on the queue
				  if (!it->second.empty ())
					{
					  PM1 = it->second.top ();

#ifdef STATISTICS
					  SamplingBeforeWritingToSHM(PM1);
#endif

					  // Shared memory IPC
					  if (WriteToSharedMemory3 (it->first, PM1) == OK)
						{
						  it->second.pop ();

						  // Mark the message to be deleted
						  PM1->MarkToDelete ();
						}
					  else
						{
#ifdef STATISTICS
						  // Note: Considers that the message entered the queue again. But in fact it never left. Just the statistics are reset
						  SamplingBeforePushingToOutputQueue(PM1);
#endif
						}

#ifdef STATISTICS
					  SamplingAfterSHMService(PM1);
#endif
					}
				}

			  if (sem_post (mutex) != 0) // Successfully locked the semaphore, so unlock it
				{
				  perror ("Writing Output Queue : sem_post");
				}
			}

		  if (sem_close (mutex) != 0)
			{
			  perror ("Writing Output Queue : sem_close");
			}
		}
	  else
		{
		  perror ("Writing Output Queue : unable to create/open semaphore");

		  sem_unlink (SemaphoreName.c_str ());
		}

	  // Sleep to avoid CPU overheating
	  tthread::this_thread::sleep_for (tthread::chrono::microseconds (10));
	}
}

// Read Messages from the queues and forward to other ShowMessages()processes via IPC/IHC
void GW::Gateway ()
{
  Message *PM1 = NULL;
  Message *PM2 = NULL;
  bool RunFlag = false;
  unsigned int NumberOfCycles = 0;
  string Offset = "          ";
  double ScheduledTime = 0;
  double Time = 0;
  long long int MessageSize = 0;

  // Added in April 11th, 2021 to provide a separated thread for output queues serving
  tthread::thread *T = new thread (&GW::ReadFromOutputQueueThreadWrapper, this);

  while (StopGateway == false) // If true, the gateway will exit
	{
	  // ****************************************************************************
	  // Step 1 : Read InputQueue
	  // ****************************************************************************

	  // Verify if there is a message on the queue
	  if (!InputQueue.empty ())
		{
		  // Check the message in the input queue
		  PM1 = InputQueue.top ();

		  // Get the message time
		  ScheduledTime = PM1->GetTime ();

		  // Get the current time
		  Time = GetTime ();

		  // If the message scheduled time is smaller than the current time, remove the message from the queue
		  // Otherwise, keep the message on the queue
		  if (ScheduledTime < Time)
			{
#ifdef DEBUG
			  S << endl << setprecision (10) << "          (t = " << Time << ")" << endl;
#endif

			  if (NumberOfCycles % 500 == 0)
				{
				  S << endl << setprecision (10) << "          (t = " << Time << ")" << endl;
				  S << "[1]       (IQ Size = " << InputQueue.size () << ")" << endl;

				  map<std::string, priority_queue<Message *, vector<Message *>, DereferenceCompareNode> >::iterator it;

				  unsigned int Counter = 0;

				  for (it = OutputQueues.begin (); it != OutputQueues.end (); it++)
					{
					  Counter++;

					  S << "[6]       (OQ[" << Counter << "] Size = " << it->second.size () << ")" << endl;
					}

				  S << Offset << "(Messages in memory = " << PP->GetNumberOfMessages () << ")" << endl;
				}

#ifdef STATISTICS

			  SamplingAfterRemovingFromInputQueue(PM1);

#endif

			  //S << "          (Reading the message from the InputQueue)"<< endl;

			  InputQueue.pop ();

			  //S << Offset << "(The popping was a success. Size = " << InputQueue.size() << ")"<<endl;

			  //S << Offset << "(Calling the run to process the message)" << endl;

			  RunFlag = true;
			}
		  else
			{
			  // Set the discrete time
			  DiscreteTime = floor (Time);

#ifdef DEBUG
			  if (DiscreteTime != PreviousDiscreteTime)
				{
				  S << setprecision (10) << "          (Waiting " << (ScheduledTime - Time) << " sec)" << endl;
				}
#endif

			  // Stores the previous discrete time for the next loop
			  PreviousDiscreteTime = DiscreteTime;

			  RunFlag = false;
			}
		}

	  // ****************************************************************************
	  // Step 2 : Run procedure to interpret and run the received message
	  // ****************************************************************************

	  if (PM1 != NULL)
		{
		  if (RunFlag == true)
			{

#ifdef STATISTICS
			  SamplingBeforeRun(PM1);
#endif

#ifdef DEBUG
			  S << "[2]       (Going to process the following message with instantiation number "
				<< PM1->InstantiationNumber << ")" << endl;
			  //S << "[2]       (Processing)" << endl;

			  S << "(" << endl << *PM1 << ")" << endl;
#endif

			  // *************************
			  // Run
			  // *************************
			  Run (PM1, PM2);

#ifdef DEBUG
			  S << "[3]       (Finished processing.)" << endl;
#endif

#ifdef STATISTICS
			  SamplingAfterRun(PM1);
#endif

#ifdef DEBUG
			  S << "[3]       (Deleting messages previously marked to delete.)" << endl;
#endif

#ifdef DEBUG2

			  PP->ShowMessages ();

#endif

			  PP->DeleteMarkedMessages ();

#ifdef DEBUG

			  S << "[3]       (Finished deleting)" << endl;

#endif

			  // Make the pointer null
			  PM1 = NULL;

			  RunFlag = false;
			}
		}

	  // ****************************************************************************
	  // Step 4 : Read OS IPC
	  // ****************************************************************************

	  // Read from shared memory
	  ReadFromSharedMemory3 ();

	  NumberOfCycles++;

	  // Sleep to avoid CPU overheating
	  tthread::this_thread::sleep_for (tthread::chrono::microseconds (10));
	}

  // Added in April 11th, 2021 to provide a separated thread for output queues serving
  T->join ();

  delete T;
}

int GW::ReadFromSharedMemory3 ()
{
  int Status = ERROR;
  string Offset = "          ";
  double StartingTime;
  double EndingTime = 0;

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
	   z++) // Modified in 9th April 2021 to deal with parallel shared memories.
	{
	  sem_t *mutex;
	  void *shm_address;
	  unsigned char *data;    // Manipulate the content on shared memory
	  long long TotalSize = 0;     // The total size of the message being transferred
	  // The shared memory segment address
	  string SemaphoreName;

	  // Set the semaphore name
	  SemaphoreName = IntToString (
		  PP->Key + z); // Modified in 9th April 2021 to deal with parallel shared memories.

	  mutex = sem_open (SemaphoreName.c_str (), O_CREAT, 0666, 1);

	  // Check for error on semaphore open
	  if (mutex != SEM_FAILED)
		{

#ifdef DEBUG3

		  S << Offset << "(Opened the semaphore " << SemaphoreName << ")" << endl;

#endif

		  if (PP->shmid[z] == -1)
			{ // Modified in 9th April 2021 to deal with parallel shared memories.
			  // Create and connect to the segment
			  if ((PP->shmid[z] = shmget (PP->Key + z, MaxSegmentSize, IPC_CREAT | 0666 )) < 0)
				{ // Modified in 9th April 2021 to deal with parallel shared memories.
				  perror ("shmget");
				  exit(1);
				}

				//(shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)

			  S << "          (Created an input shared memory segment with key = " << (PP->Key + z)
				<< " and identifier = "
				<< PP->shmid[z] << ")"
				<< endl; // Modified in 9th April 2021 to deal with parallel shared memories.
			}
		  else
			{
			  // Test to see if it worked
			  // Modified in 9th April 2021 to deal with parallel shared memories.
			  if ((shm_address = shmat (PP->shmid[z], NULL, 0)) != NULL)
				{

				  if (sem_trywait (mutex) == 0)
					{
#ifdef DEBUG3
					  S << Offset << "(Locked the semaphore " << SemaphoreName << ")" << endl;
#endif

					  // *****************************
					  // Start receiving a new message
					  // *****************************

					  data = (unsigned char *)shm_address;

					  // Test to see if it worked
					  if (data != (unsigned char *)(-1))
						{
						  if (data[0] == (unsigned char)'w')
							{
							  StartingTime = GetTime ();

#ifdef DEBUG1
							  S << endl << endl << "          (Time = " << StartingTime << ")" << endl;

							  S << "[0]       (Reading from shared memory)" << endl;

							  S << "          (The value of the first byte in shared memory is " << (char)data[0]
								<< " (f = Free to write, w = still waiting for peer reading)" << endl;

							  S << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

							  unsigned char *HeaderSizeField = new unsigned char[sizeof (TotalSize)];

							  for (int i = 0; i < 8; i++)
								{
								  HeaderSizeField[i] = data[i + 1];

								  //printf("%i %d %c \n",i,data[i],data[i]);
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
								  //S << Offset << "(TotalSize = "<<TotalSize<<")"<< endl;

								  //S << Offset << "A = " <<A<<endl;
								  //S << Offset << "B = " <<B<<endl;
								  //S << Offset << "C = " <<C<<endl;
								  //S << Offset << "D = " <<D<<endl;
								  //S << Offset << "E = " <<E<<endl;
								  //S << Offset << "G = " <<G<<endl;
								  //S << Offset << "H = " <<H<<endl;
#endif

								  char *Payload = new char[TotalSize - 8];

								  // Read all the message
								  for (int j = 8; j < TotalSize; j++)
									{
									  Payload[j - 8] = (char)data[j + 1];
									}

								  double TimeStamp = 0;

								  // Read the timestamp header
								  unsigned char *HeaderTimeStampField = new unsigned char[8];

								  for (int n = 0; n < 8; n++)
									{
									  HeaderTimeStampField[n] = data[TotalSize + 1 + n];

									  //printf("%i %d %c \n",i,data[i],data[i]);
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
								  Message *PM = 0;

								  PP->NewMessage (GetTime (), 0, false, PM);

								  PM->SetMessageFromCharArray (Payload, (TotalSize - 8));

								  PM->ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2 ();

#ifdef DEBUG1
								  long long SS;
								  long long SP;
								  PM->GetMessageSize(SS);
								  PM->GetPayloadSize(SP);

								  S << Offset <<"(The total size of the message is = "<<TotalSize-8<<")"<< endl;
								  S << Offset <<"(The total size of the message is = "<<SS<<")"<< endl;
								  S << Offset <<"(The total size of the payload content in message is = "<<SP<<")"<< endl;

								  if (TotalSize < 3000 && SP > 1)
									{
									  S << Offset << "(Showing what is on the payload array. Limited to messages smaller than 3000 bytes)"<< endl;

									  for (unsigned int l=0; l<(TotalSize-8); l++)
										{
										  printf("%i %d %c \n",l,Payload[l],Payload[l]);
										}

									  S << Offset << "(Shown)"<< endl;
									}
#endif

#ifdef DEBUG2
								  S << Offset << "(MessageSize = " << (TotalSize - 8) << ")" << endl;

								  S << Offset
									<< "(The following message was received from another process in the same OS)"
									<< endl;

								  S << "(" << endl << *PM << endl << ")" << endl;
#endif

								  CommandLine *PCL = 0;

								  // The special case of message's object type
								  PM->GetCommandLine ("-message", "--type", PCL);

								  if (PCL != 0)
									{
									  string MessageType;

									  // Set the new message field properly
									  if (PCL->GetArgumentElement (0, 0, MessageType) == OK)
										{
										  PM->SetType (StringToInt (MessageType));

										  //S << Offset <<"(The message is from type = "<<MessageType<<")"<< endl;
										}
									}

								  // Pushing message to the queue
								  PushToInputQueue (PM);

#ifdef STATISTICS

								  // Set the service ending time
								  EndingTime=GetTime();

								  // Sample
								  tss->Sample(EndingTime-StartingTime);

								  // Update the mean
								  tss->CalculateArithmetic();

								  // Sample to file
								  tss->SampleToFile(EndingTime);

								  // **************************************************
								  // Type 1 messages statistics
								  // **************************************************
								  if (PM->GetType() == 1)
									  {
										  // Sample
										  tss1->Sample(EndingTime-StartingTime);

										  // Update the mean
										  tss1->CalculateArithmetic();

										  // Sample to file
										  tss1->SampleToFile(EndingTime);
									  }

								  // Measuring the delay between final write and beggining to read

								  // Sample
								  tshm->Sample(StartingTime-TimeStamp);

								  // Update the mean
								  SN							tshm->CalculateArithmetic();

								  // Sample to file
								  tshm->SampleToFile(EndingTime);

								  // **************************************************
								  // Type 1 messages statistics
								  // **************************************************
								  if (PM->GetType() == 1)
									  {
										  // Sample
										  tshm->Sample(EndingTime-StartingTime);

										  // Update the mean
										  tshm->CalculateArithmetic();

										  // Sample to file
										  tshm->SampleToFile(EndingTime);
									  }

#endif

								  delete[] HeaderTimeStampField;

								  MessagesReceived++;

								  delete[] Payload;

#ifdef DEBUG3
								  S << Offset << "(Freeing the shared memory to the peer)" << endl;
#endif

								  // Clean the memory segment
								  memset (data, (unsigned char)'f', 1);

								  //S << Offset << "(Finished reading)"<< endl;

								  //S << Offset << "(Time = "<<GetTime()<<")"<<endl;

								  Status = OK;

								} // (TotalSize < 0 && TotalSize > 1073741816) means that the message is with error or misconfigured
							  else
								{
								  // Total size is negative or zero
								  // Clean the memory segment
								  memset (data, (unsigned char)'f', MaxSegmentSize);
								}

							  delete[] HeaderSizeField;
							}

						} // (data == (unsigned char *)(-1)) means that the data at the SHM was not set properly
					  else
						{
						  S << Offset << "(Warning: unable to read the data on the shared memory)" << endl;

						  perror ("reading SHM : shmat");
						}

					  if (sem_post (mutex) == 0)
						{
#ifdef DEBUG3
						  S << Offset << "(Unlocked the semaphore " << SemaphoreName << ")" << endl;
#endif
						}
					  else
						{
						  perror ("reading SHM : sem_post");
						}

					} // (sem_trywait(mutex) != 0) means that the SHM is not free to lock

				  // Detach from shared memory
				  if (shmdt (shm_address) == -1)
					{
					  perror ("reading SHM : shmdt");
					}

				} // (shm_address = shmat(PP->shmid, NULL, 0)) == NULL) means it is unable to attach to the shared memory segment
			  else
				{
				  perror ("reading SHM : shmat");
				}
			}

		  if (sem_close (mutex) == 0)
			{
#ifdef DEBUG3
			  S << Offset << "(Closed the semaphore " << SemaphoreName << ")" << endl;
#endif
			}
		  else
			{
			  perror ("reading SHM : sem_close");
			}

		} // (mutex == SEM_FAILED) means it was unable to create/open the semaphore
	  else
		{
		  perror ("reading SHM : unable to create/open semaphore");

		  sem_unlink (SemaphoreName.c_str ());
		}
	}

  return Status;
}

// Write to the shared memory
int GW::WriteToSharedMemory3 (std::string OQS, Message *M)
{
  int Status = ERROR;
  unsigned int Category = 17;

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
	   z++) // Modified in 9th April 2021 to deal with parallel shared memories.
	{
	  long long MessageSize = 0;    // The size of the Message (in bytes) being transferred
	  long long TotalSize = 0;        // The total size of the message being transferred
	  unsigned int NoCL = 0;
	  unsigned char *data;            // Manipulate the content on shared memory
	  int shmid;                    // Shared memory ID on OS for a certain key_t
	  void *shm_address;            // The shared memory segment address
	  string Offset = "          ";
	  sem_t *mutex;
	  vector<string> *Values = new vector<string>;

	  int _shm_key = StringToInt (OQS) + z;

	  string _oqs = IntToString (_shm_key);

	  if (GetHTBindingValues (Category, _oqs, Values) == OK)
		{
		  // Assert the Values vector
		  if (Values != 0)
			{
			  if (!Values->empty ())
				{
				  // Carry the destination pointer
				  shmid = StringToInt (Values->at (0));

#ifdef DEBUG2
				  S << Offset << "(Shared memory ID (shmid) already on HT = " << shmid << ")" << endl;
#endif
				}
			}
		}
	  else
		{
		  if (ReturnIPCSHMID ((key_t)_shm_key, shmid) == OK)
			{
			  Values->push_back (IntToString (shmid));

			  StoreHTBindingValues (Category, _oqs, Values);

			  S << Offset << "(Storing the shared memory ID (shmid) = " << shmid << " on HT. The related key is "
				<< _oqs << ")" << endl;
			}
		}

	  if (shmid > 0)
		{
		  mutex = sem_open (_oqs.c_str (), O_CREAT, 0666, 1);

		  // Check for error on semaphore open
		  if (mutex != SEM_FAILED)
			{

			  // ***************************************************************************************
			  // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
			  // ***************************************************************************************

#ifdef DEBUG3
			  S << Offset << "(Opened the semaphore " << _oqs << ")" << endl;
#endif

			  if ((shm_address = shmat (shmid, NULL, 0)) != NULL)
				{

				  if (sem_trywait (mutex) == 0)
					{

#ifdef DEBUG3
					  S << Offset << "(Locked the semaphore " << _oqs << ")" << endl;
#endif

					  data = (unsigned char *)shm_address;

					  if (data != (unsigned char *)(-1))
						{
						  if (data[0] != (unsigned char)'f' && data[0] != (unsigned char)'w')
							{
							  data[0] = (unsigned char)'f';

							  S << Offset << "(Initialized the shared memory with key " << _oqs
								<< " regarding the r/w control flag)" << endl;
							}
						}

#ifdef DEBUG3
					  //S << "          (The value of the first byte in shared memory is "<<(char)data[0]<<" (f = Free to write, w = still waiting for peer reading)" << endl;
					  S << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

					  if (data[0] == (unsigned char)'f')
						{

#ifdef DEBUG2
						  S << "[5]       (Writing a message to the shared memory segment with key = " << _oqs
							<< " identified by " << shmid << ")" << endl;
#endif

						  // Get the Message size
						  if (M->GetMessageSize (MessageSize) == OK)
							{
							  unsigned char *Payload;

							  // Get the message in a char array format
							  if (M->GetMessageFromCharArray (Payload) == OK)
								{
								  M->GetNumberofCommandLines (NoCL);

								  TotalSize = MessageSize + 8;

								  unsigned char *HeaderSizeField = new unsigned char[TotalSize];
								  unsigned char *PDU = new unsigned char[TotalSize];

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
										  //printf("%i %d %c \n",j,PDU[j],PDU[j]);
										}

									  if (j >= 8 && j < (MessageSize + 8))
										{
										  PDU[j] = (unsigned char)Payload[(j - 8)];
										  //printf("%i %d %c \n",j,PDU[j],PDU[j]);
										}
									}

								  if (NoCL > 2 && TotalSize <= (long long)MaxSegmentSize)
									{

#ifdef DEBUG2
									  S << Offset << "(Sending the following message)" << endl;

									  S << "(" << endl << *M << ")" << endl;

									  S << Offset << "(The size of the payload is = " << MessageSize << ")" << endl;

									  S << Offset << "(The size of the total message is = " << TotalSize << ")" << endl;

									  S << Offset << "(The size of the memory segment is = " << MaxSegmentSize << ")"
										<< endl;

									  //S << Offset << "(Make the shared memory unavailable while not read by the peer)"<< endl;
#endif

									  // Clean the memory segment before writing
									  // March 6th, 2021: changed from MaxSegmentSize to 1, since there is not need to clean more than this
									  memset (data, (unsigned char)'w', 1);

									  for (long long k = 1; k < TotalSize + 1; k++)
										{
										  data[k] = PDU[k - 1];
										}

									  // Send the timestap of this operation after the message

									  unsigned long long Time = (unsigned long long)(GetTime () * 10e9);

									  unsigned char *HeaderTimeStampField = new unsigned char[8];

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

									  MessagesSent++;

									  Status = OK;
									}
								  else
									{
									  S << Offset
										<< "(ERROR: The message is too big for the memory segment or it has less than 3 command lines)"
										<< endl;

									  // Mark to delete the message. Added in March 7th, 2021
									  M->MarkToDelete ();
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
						  tthread::this_thread::sleep_for (tthread::chrono::microseconds (5));
						}

					  if (sem_post (mutex) == 0) // Successfully locked the semaphore, so unlock it
						{
#ifdef DEBUG3
						  S << Offset << "(Unlocked the semaphore " << _oqs << ")" << endl;
#endif
						}
					  else
						{
						  perror ("writing SHM : sem_post");
						}

					} // (sem_trywait(mutex) != 0) means UNABLE TO LOCK. If someone locked, it is normal to get this condition
				  else
					{
#ifdef DEBUG1
					  perror ("writing SHM : unable to lock the semaphore");
#endif
					}

				  // Sleep while wait for shared memory semaphore
				  tthread::this_thread::sleep_for (tthread::chrono::microseconds (5));

				  // Detach from shared memory, if attached
				  if (shmdt (shm_address) == -1)
					{
					  perror ("writing SHM : shmdt");
					}

				} // ((shm_address = shmat(shmid, NULL, 0)) == NULL) means UNABLE TO ATTACH
			  else
				{
				  perror ("writing SHM : semop");
				}

			  if (sem_close (mutex) == 0)
				{
#ifdef DEBUG3
				  S << Offset << "(Closed the semaphore " << _oqs << ")" << endl;
#endif
				}
			  else
				{
				  perror ("writing SHM : sem_close");
				}

			} //(mutex != SEM_FAILED)
		  else
			{
			  perror ("writing SHM : unable to open/create the semaphore");

			  sem_unlink (_oqs.c_str ());
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

  //S << Offset << "(Finished)"<< endl;

  return Status;
}

// First attachment to learned shared memory
int GW::ReturnIPCSHMID (key_t _Key, int &_shmid)
{
  //GW			*PGW=0;
  string Offset = "          ";
  string SemaphoreName;
  sem_t *mutex;
  int Status = ERROR;


  // Set the semaphore name
  SemaphoreName = IntToString (_Key);

  mutex = sem_open (SemaphoreName.c_str (), O_CREAT, 0666, 1);

  // Check for error on semaphore open
  if (mutex != SEM_FAILED)
	{
	  // Connect to the segment
	  if ((_shmid = shmget (_Key, MaxSegmentSize, IPC_CREAT | 0666 )) != -1)
		{
		  Status = OK;
		}
	  else
		{
		  perror ("shmat");

		  S << Offset << "(ERROR: Unable to get the shared memory segment identified by the key = " << _Key << ")"
			<< endl;
		}
	}
  else
	{
	  perror ("While initiating the SHM: unable to create semaphore");

	  sem_unlink (SemaphoreName.c_str ());

	  S << Offset << "(ERROR: Unable to create the semaphore while discovering the peer shmid)" << endl;
	}

  return Status;
}

// Auxiliary functions
void GW::SetStopGatewayFlag (bool _F)
{
  StopGateway = _F;
}

// Set a value behind a key
int GW::StoreHTBindingValues (unsigned int _Category, string _Key, vector<string> *_Values)
{
  int Status = ERROR;

  if (PHT->StoreBinding (_Category, _Key, _Values) == OK)
	{
	  Status = OK;
	}

  return Status;
}

// Discover the values behind a key
int GW::GetHTBindingValues (unsigned int _Category, string _Key, vector<string> *&_Values)
{
  int Status = ERROR;

  if (PHT->GetBinding (_Category, _Key, _Values) == OK)
	{
	  Status = OK;
	}

  return Status;
}

// Wrapper function for ReadFromOutputQueue() thread
void GW::ReadFromOutputQueueThreadWrapper (void *_PGW)
{
  GW *P = static_cast<GW *>(_PGW);

  P->ReadFromOutputQueue ();
}

// Reset all statistics
void GW::ResetStatistics ()
{

#ifdef STATISTICS

  twi->Reset();
  wi->Reset();
  tsi->Reset();
  si->ResetButKeepLastValue();
  mii->Reset();
  ri->Reset();
  li->Reset();
  two->Reset();
  wo->Reset();
  tso->Reset();
  so->ResetButKeepLastValue();
  mio->Reset();
  ro->Reset();
  lo->Reset();
  tss->Reset();

#endif
}

void GW::SamplingBeforePushingToInputQueue (Message *PM1)
{
  double Time = GetTime ();

  // Set the time the message entered the input queue
  PM1->TimeStamp = Time;

  // **************************************************
  // All messages statistics
  // **************************************************

  // Calculates mean occupation
  wi->CalculateWeighted (Time);

  // Sample occupation to file
  wi->SampleToFile (Time);

  // Increments occupation
  wi->SampleAsAnIncrement ();

  // Sample occupation to file
  wi->SampleToFile (Time);

  // **************************************************
  // Type 1 messages statistics
  // **************************************************

  if (PM1->GetType () == 1)
	{
	  // Calculates mean occupation
	  wi1->CalculateWeighted (Time);

	  // Sample occupation to file
	  wi1->SampleToFile (Time);

	  // Increments occupation
	  wi1->SampleAsAnIncrement ();

	  // Sample occupation to file
	  wi1->SampleToFile (Time);
	}
}

void GW::SamplingAfterRemovingFromInputQueue (Message *PM1)
{
  double EntranceTime = 0;
  double Time = GetTime ();
  double DeltaT = 0;

  EntranceTime = PM1->TimeStamp;

  // Calculate queue delay
  DeltaT = Time - EntranceTime;

  if (DeltaT < 1e-50)
	{ DeltaT = 1e-50; }

  // **************************************************
  // All messages statistics
  // **************************************************

  // Sample
  twi->Sample (DeltaT);

  // Update the mean
  twi->CalculateArithmetic ();

  // Sample to file
  twi->SampleToFile (Time);

  // Update the mean
  wi->CalculateWeighted (Time);

  // Sample occupation to file
  wi->SampleToFile (Time);

  //S << setprecision(10) << "          (Input queue occupation before = " << wi->GetLastSample() << " message(s))" << endl;

  // Decrements occupation
  wi->SampleAsADecrement ();

  // Sample occupation to file
  wi->SampleToFile (Time);

  //S << setprecision(10) << "          (Input queue occupation now = " << wi->GetLastSample() << " message(s))" << endl;
  //S << setprecision(10) << "          (Input queue occupation mean = " << wi->GetMean() << " message(s))" << endl;
  //S << setprecision(10) << "          (Input queue delay = " << DeltaT << " seconds)" << endl;

  // **************************************************
  // Type 1 messages statistics
  // **************************************************

  if (PM1->GetType () == 1)
	{
	  // Sample
	  twi1->Sample (DeltaT);

	  // Update the mean
	  twi1->CalculateArithmetic ();

	  // Sample to file
	  twi1->SampleToFile (Time);

	  // Update the mean
	  wi1->CalculateWeighted (Time);

	  // Sample occupation to file
	  wi1->SampleToFile (Time);

	  // Decrements occupation
	  wi1->SampleAsADecrement ();

	  // Sample occupation to file
	  wi1->SampleToFile (Time);
	}
}

void GW::SamplingBeforeWritingToSHM (Message *PM1)
{
  double EntranceTime = 0;
  double Time = GetTime ();
  double DeltaT = 0;
  long long int Size = 0;

  EntranceTime = PM1->TimeStamp;

  // Calculate queue delay
  DeltaT = Time - EntranceTime;

  if (DeltaT < 1e-50)
	{ DeltaT = 1e-50; }

  // **************************************************
  // All messages statistics
  // **************************************************

  // Sample
  two->Sample (DeltaT);

  // Update the mean
  two->CalculateArithmetic ();

  // Sample to file
  two->SampleToFile (Time);

  // Update the mean
  wo->CalculateWeighted (Time);

  // Sample occupation to file
  wo->SampleToFile (Time);

  //S << setprecision(10) << "          (Output queue occupation before = " << wo->GetLastSample() << " message(s))" << endl;

  // Decrements occupation
  wo->SampleAsADecrement ();

  // Sample occupation to file
  wo->SampleToFile (Time);

  //S << setprecision(10) << "          (Output queue occupation now = " << wo->GetLastSample() << " message(s))" << endl;
  //S << setprecision(10) << "          (Output queue delay = " << DeltaT << " seconds)" << endl;

  // Update the mean
  so->CalculateWeighted (Time);

  // Sample occupation to file
  so->SampleToFile (Time);

  //S << setprecision(10) << "          (Output server occupation before = " << so->GetLastSample() << " message(s))" << endl;

  // Decrements occupation
  so->SampleAsAnIncrement ();

  // Sample occupation to file
  so->SampleToFile (Time);

  //S << setprecision(10) << "          (Output server occupation now = " << so->GetLastSample() << " message(s))" << endl;

  PM1->GetMessageSize (Size);

  //S << setprecision(10) << "          (Size = " << Size << ")" << endl;

  if (Size > 0)
	{
	  // Sample
	  lo->Sample (Size);

	  // Update the mean
	  lo->CalculateArithmetic ();

	  // Sample to file
	  lo->SampleToFile (Time);
	}

  // **************************************************
  // Type 1 messages statistics
  // **************************************************
  if (PM1->GetType () == 1)
	{
	  // Sample
	  two1->Sample (DeltaT);

	  // Update the mean
	  two1->CalculateArithmetic ();

	  // Sample to file
	  two1->SampleToFile (Time);

	  // Update the mean
	  wo1->CalculateWeighted (Time);

	  // Sample occupation to file
	  wo1->SampleToFile (Time);

	  // Decrements occupation
	  wo1->SampleAsADecrement ();

	  // Sample occupation to file
	  wo1->SampleToFile (Time);

	  // Update the mean
	  so1->CalculateWeighted (Time);

	  // Sample occupation to file
	  so1->SampleToFile (Time);

	  // Decrements occupation
	  so1->SampleAsAnIncrement ();

	  // Sample occupation to file
	  so1->SampleToFile (Time);

	  if (Size > 0)
		{
		  // Sample
		  lo1->Sample (Size);

		  // Update the mean
		  lo1->CalculateArithmetic ();

		  // Sample to file
		  lo1->SampleToFile (Time);
		}
	}

  // Set the time the message is send to the SHM
  PM1->TimeStamp = Time;
}

void GW::SamplingBeforePushingToOutputQueue (Message *PM1)
{
  double Time = GetTime ();

  PM1->TimeStamp = Time;

  // **************************************************
  // All messages statistics
  // **************************************************

  // Calculates mean occupation
  wo->CalculateWeighted (Time);

  // Sample occupation to file
  wo->SampleToFile (Time);

  // Increments occupation
  wo->SampleAsAnIncrement ();

  // Sample occupation to file
  wo->SampleToFile (Time);

  // **************************************************
  // Type 1 messages statistics
  // **************************************************
  if (PM1->GetType () == 1)
	{
	  // Calculates mean occupation
	  wo1->CalculateWeighted (Time);

	  // Sample occupation to file
	  wo1->SampleToFile (Time);

	  // Increments occupation
	  wo1->SampleAsAnIncrement ();

	  // Sample occupation to file
	  wo1->SampleToFile (Time);
	}
}

void GW::SamplingAfterSHMService (Message *PM1)
{
  double EntranceTime = 0;
  double Time = GetTime ();
  double DeltaT = 0;

  // Update the entrance time
  EntranceTime = PM1->TimeStamp;

  // Calculate queue delay
  DeltaT = Time - EntranceTime;

  if (DeltaT < 1e-50)
	{ DeltaT = 1e-50; }

  // **************************************************
  // All messages statistics
  // **************************************************

  // Update the mean
  so->CalculateWeighted (Time);

  // Sample occupation to file
  so->SampleToFile (Time);

  //S << setprecision(10) << "          (Output server occupation before = " << so->GetLastSample() << " message(s))" << endl;

  // Decrements occupation
  so->SampleAsADecrement ();

  //S << setprecision(10) << "          (Output server occupation now = " << so->GetLastSample() << " message(s))" << endl;

  // Sample occupation to file
  so->SampleToFile (Time);

  // Sample
  tso->Sample (DeltaT);

  // Update the mean
  tso->CalculateArithmetic ();

  // Sample to file
  tso->SampleToFile (Time);

  //S << setprecision(10) << "          (Output server service delay = " << DeltaT << " seconds)" << endl;

  // Sample
  mio->Sample (1 / DeltaT);

  // Update the mean
  mio->CalculateArithmetic ();

  // Sample to file
  mio->SampleToFile (Time);

  // Sample
  ro->Sample (lo->GetMean () / DeltaT);

  // Update the mean
  ro->CalculateArithmetic ();

  // Sample to file
  ro->SampleToFile (Time);

  // **************************************************
  // Type 1 messages statistics
  // **************************************************
  if (PM1->GetType () == 1)
	{
	  // Update the mean
	  so1->CalculateWeighted (Time);

	  // Sample occupation to file
	  so1->SampleToFile (Time);

	  // Decrements occupation
	  so1->SampleAsADecrement ();

	  // Sample occupation to file
	  so1->SampleToFile (Time);

	  // Sample
	  tso1->Sample (DeltaT);

	  // Update the mean
	  tso1->CalculateArithmetic ();

	  // Sample to file
	  tso1->SampleToFile (Time);

	  // Sample
	  mio1->Sample (1 / DeltaT);

	  // Update the mean
	  mio1->CalculateArithmetic ();

	  // Sample to file
	  mio1->SampleToFile (Time);

	  // Sample
	  ro1->Sample (lo1->GetMean () / DeltaT);

	  // Update the mean
	  ro1->CalculateArithmetic ();

	  // Sample to file
	  ro1->SampleToFile (Time);
	}
}

void GW::SamplingBeforeRun (Message *PM1)
{
  double Time = GetTime ();
  long long int Size = 0;

  // **************************************************
  // All messages statistics
  // **************************************************

  // Update the mean
  si->CalculateWeighted (Time);

  // Sample occupation to file
  si->SampleToFile (Time);

  //S << setprecision(10) << "          (Input server occupation before = " << si->GetLastSample() << " message(s))" << endl;

  // Decrements occupation
  si->SampleAsAnIncrement ();

  // Sample occupation to file
  si->SampleToFile (Time);

  PM1->GetMessageSize (Size);

  //S << setprecision(10) << "          (Size = " << Size << ")" << endl;

  if (Size > 0)
	{
	  // Sample
	  li->Sample (Size);

	  // Update the mean
	  li->CalculateArithmetic ();

	  // Sample to file
	  li->SampleToFile (Time);
	}

  // **************************************************
  // Type 1 messages statistics
  // **************************************************
  if (PM1->GetType () == 1)
	{
	  // Update the mean
	  si1->CalculateWeighted (Time);

	  // Sample occupation to file
	  si1->SampleToFile (Time);

	  // Decrements occupation
	  si1->SampleAsAnIncrement ();

	  // Sample occupation to file
	  si1->SampleToFile (Time);

	  if (Size > 0)
		{
		  // Sample
		  li1->Sample (Size);

		  // Update the mean
		  li1->CalculateArithmetic ();

		  // Sample to file
		  li1->SampleToFile (Time);
		}
	}

  // Update the time the message entered the service
  PM1->TimeStamp = Time;

  //S << setprecision(10) << "          (Input server occupation now = " << si->GetLastSample() << " message(s))" << endl;

}

void GW::SamplingAfterRun (Message *PM1)
{
  double EntranceTime = 0;
  double Time = GetTime ();
  double DeltaT = 0;

  // Get the entrance time
  EntranceTime = PM1->TimeStamp;

  // Calculate queue delay
  DeltaT = Time - EntranceTime;

  if (DeltaT < 1e-50)
	{ DeltaT = 1e-50; }

  // **************************************************
  // All messages statistics
  // **************************************************

  // Update the mean
  si->CalculateWeighted (Time);

  // Sample occupation to file
  si->SampleToFile (Time);

  //S << setprecision(10) << "          (Input server occupation before = " << si->GetLastSample() << " message(s))" << endl;

  // Decrements occupation
  si->SampleAsADecrement ();

  //S << setprecision(10) << "          (Input server occupation now = " << si->GetLastSample() << " message(s))" << endl;

  // Sample occupation to file
  si->SampleToFile (Time);

  // Sample
  tsi->Sample (DeltaT);

  // Update the mean
  tsi->CalculateArithmetic ();

  // Sample to file
  tsi->SampleToFile (Time);

  //S << setprecision(10) << "          (Input server service delay = " << DeltaT << " seconds)" << endl;

  // Sample
  mii->Sample (1 / DeltaT);

  // Update the mean
  mii->CalculateArithmetic ();

  // Sample to file
  mii->SampleToFile (Time);

  // Sample
  ri->Sample (li->GetMean () / DeltaT);

  // Update the mean
  ri->CalculateArithmetic ();

  // Sample to file
  ri->SampleToFile (Time);

  //S << setprecision(10) << "          (Input server service rate = " << 1/DeltaT << " messages/second)" <<endl;

  // **************************************************
  // Type 1 messages statistics
  // **************************************************
  if (PM1->GetType () == 1)
	{
	  // Update the mean
	  si1->CalculateWeighted (Time);

	  // Sample occupation to file
	  si1->SampleToFile (Time);

	  // Decrements occupation
	  si1->SampleAsADecrement ();

	  // Sample occupation to file
	  si1->SampleToFile (Time);

	  // Sample
	  tsi1->Sample (DeltaT);

	  // Update the mean
	  tsi1->CalculateArithmetic ();

	  // Sample to file
	  tsi1->SampleToFile (Time);

	  // Sample
	  mii1->Sample (1 / DeltaT);

	  // Update the mean
	  mii1->CalculateArithmetic ();

	  // Sample to file
	  mii1->SampleToFile (Time);

	  // Sample
	  ri1->Sample (li1->GetMean () / DeltaT);

	  // Update the mean
	  ri1->CalculateArithmetic ();

	  // Sample to file
	  ri1->SampleToFile (Time);
	}
}
