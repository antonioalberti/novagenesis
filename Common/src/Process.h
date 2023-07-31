/*
	NovaGenesis

	Name:		Process
	Object:		Process
	File:		Process.h
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

#ifndef _PROCESS_H
#define _PROCESS_H

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _QUEUE_H
#include <queue>
#endif

#ifndef _SYS_MSG_H_
#include <sys/msg.h>
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _CLI_H
#include "CLI.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _TUPLE_H
#include "Tuple.h"
#endif

#ifndef _SYS_UTSNAME_H
#include <sys/utsname.h>
#endif

#ifndef _INTTYPES_H
#include <inttypes.h>
#endif

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _IFADDRS_H
#include <ifaddrs.h>
#endif

#ifndef _NETINET_IN_H
#include <netinet/in.h>
#endif

#ifndef _ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifndef _MESSAGEBUILDER_H
#include "MessageBuilder.h"
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

// All NovaGenesis important defines are here!!
#define ERROR 1
#define OK 0
#define BUSY false
#define FREE true
#define DELETED_BY_APP true //
#define DELETED_BY_CORE false //
#define MAX_MESSAGES_IN_MEMORY 30000    		/* This is maximum number of messages that can be accommodated on the memory of a process. If you require more messages in memory increase this size */
#define MAX_MESSAGE_SIZE 33554532        		/* The maximum size of a message. Also, configures the maximum size at shared memory. There is no relation to the sockets MTU */
#define NUMBER_OF_PARALLEL_SHARED_MEMORIES 6    /* Regulates how many parallel shared memories are employed in a process to receive messages at IPC */
#define NUMBER_OF_THREADS_AT_SOCKET_DISPATCHER 8 /* This is the number of simultaneous threads that are employed at a certain socket to treat received protocol data units of all technologies */
#define MAX_NUMBER_OF_THREADS 1000
#define PORT "9034"   // port we're listening on, just in case we need TCP/IP for some reason!!
#define TIMEOUT 60 // TODO - FIXP/Update - This timeout (in seconds) can be used to limit some excessive delays in message transmission and reception

using namespace std;
using namespace tthread;

class Block;

class Process {
 private:

  // Legible Name
  string LN;

  // Self-certifying Name
  string SCN;

  // Message container
  vector<Block *> Blocks;

  // Message container
  Message *Messages[MAX_MESSAGES_IN_MEMORY];

  // Auxiliary container to control access to Messages container
  bool Controls[MAX_MESSAGES_IN_MEMORY];

  // Counter on the number of message stored in memory
  unsigned int NoM;

  // Number of created messages
  unsigned int MessageCounter;

  // Working path
  string Path;

  // Auxiliary prime numbers array
  unsigned int primes[16];

 public:

  // Message builder
  MessageBuilder *PMB;

  // The key used to read from process shared memory
  key_t Key;

  // The IDs used by this process shared memory
  int shmid[NUMBER_OF_PARALLEL_SHARED_MEMORIES];

  // Host Legible Name
  string HLN;

  // Host Self-certifying Name
  string HSCN;

  // Operating System Legible Name
  string OSLN;

  // Operating System Self-certifying Name
  string OSSCN;

  // Domain Legible Name
  string DLN;

  // Domain Self-certifying Name
  string DSCN;

  // ************************************************************
  // New limiters scheme. March 2016
  // ************************************************************

  string Intra_Process;

  string Intra_OS;

  string Intra_Node;

  string Intra_Domain;

  string Inter_Domain;

  // Instantiation time
  double InstantiationTime;

  // Constructor
  Process (string _LN, key_t _Key, string _Path);

  // Destructor
  virtual ~Process ();

  // Set legible name
  void SetLegibleName (string _LN);

  // Set self-certyfing name
  void SetSelfCertifyingName (string _SCN);

  // Set working path
  void SetPath (string _Path);

  // Get block legible name
  string GetLegibleName ();

  // Get process self-certifying name
  string GetSelfCertifyingName ();

  // Get working path
  string GetPath ();

  // Get Blocks container size
  unsigned int GetBlocksSize ();

  // Get Host Legible Name
  string GetHostLegibleName ();

  // Get Host Self-certifying Name
  string GetHostSelfCertifyingName ();

  // Get Operating System Legible Name
  string GetOperatingSystemLegibleName ();

  // Get Operating System Self-certifying Name
  string GetOperatingSystemSelfCertifyingName ();

  // Get Domain Self-certifying Name
  string GetDomainSelfCertifyingName ();

  // ***********************************************************
  // Block related functions
  // ***********************************************************

  // Allocate a new block and add a Block on Blocks container
  virtual int NewBlock (string _LN, Block *&_PB);

  // Insert an allocated Block on Blocks container
  void InsertBlock (Block *_PB);

  // Get a Block by its legible name
  int GetBlock (string _LN, Block *&_PB);

  // Get a Block by its index on Blocks container
  int GetBlock (unsigned int _Index, Block *&_PB);

  // Get a Block index by its legible name
  int GetBlockIndex (string _LN, unsigned int &_Index);

  // Delete a Block by its legible name
  int DeleteBlock (string _LN);

  // Delete a Block by its index on Blocks container
  int DeleteBlock (unsigned int _Index);

  // Delete a Block by its pointer
  int DeleteBlock (Block *B);

  // ***********************************************************
  // Message related functions
  // ***********************************************************

  // Allocate and add a Message on Messages container
  int NewMessage (double _Time, short _Type, bool _HasPayload, Message *&M);

  // Allocate and add a simple Message on Messages container
  int NewMessage (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path, Message *&M);

  // Allocate and add a complete Message on Messages container
  int
  NewMessage (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path, Message *&M);

  // Allocate a message by copying an original message
  int NewMessage (Message *_Original, Message *&M);

  // Get a Message
  int GetMessage (unsigned int _Index, Message *&M);

  // Delete a Message
  int HasMessage (Message *M, bool &_Answer);

  // Delete a Message
  int DeleteMessage (Message *M);

  // Erase a Message from the container
  int EraseMessage (Message *M);

  // Get number of Message object on Messages container
  unsigned int GetNumberOfMessages ();

  // Get message at Message position _Index
  Message *GetMessage (unsigned int _Index);

  // Show stored Messages
  void ShowMessages ();

  // Delete stored Messages
  void DeleteMessages ();

  // Delete the messages marked to be deleted
  void DeleteMarkedMessages ();

  // Mark unmarked messages to delete
  void MarkUnmarkedMessagesPerTime (double _Threshold);

  // Mark small messages to delete
  void MarkMalformedMessagesPerNoCLs (unsigned int _NoCL);

  // Test if a message is OK to run it
  bool OkToRun (Message *_M);

  // ***********************************************************
  // Other
  // ***********************************************************

  // Run the Gateway
  void RunGateway ();

  // Initiate Command Line Prompt for users
  void RunPrompt ();

  // Generate process self-certified name from its binary patterns
  void GenerateSCNFromProcessBinaryPatterns4Bytes (Process *_PP, string &_SCN);

  // Generate process self-certified name from its binary patterns
  void GenerateSCNFromProcessBinaryPatterns16Bytes (Process *_PP, string &_SCN);

  // Generate process self-certified name from its binary patterns
  void GenerateSCNFromProcessBinaryPatterns32Bytes (Process *_PP, string &_SCN);

  // Generate a self-certified name from a char array
  int GenerateSCNFromCharArrayBinaryPatterns4Bytes (char *_Input, long long _Size, string &_SCN);

  // Generate a self-certified name from a char array
  int GenerateSCNFromCharArrayBinaryPatterns16Bytes (char *_Input, long long _Size, string &_SCN);

  // Generate a self-certified name from a char array
  int GenerateSCNFromCharArrayBinaryPatterns4Bytes (string _Input, string &_SCN);

  // Generate a self-certified name from a char array
  int GenerateSCNFromCharArrayBinaryPatterns16Bytes (string _Input, string &_SCN);

  // Generate block self-certified name from its binary patterns
  void GenerateSCNFromProcessBinaryPatterns (Process *_PP, string &_SCN);

  // Generate a self-certified name from a char array
  int GenerateSCNFromCharArrayBinaryPatterns (char *_Input, long long _Size, string &_SCN);

  // Generate a self-certified name from a char array
  int GenerateSCNFromCharArrayBinaryPatterns (string _Input, string &_SCN);


  // *************************************************************************************
  // Functions to access HT data directly. Should be removed on future
  // *************************************************************************************

  // Get the values behind a key
  int GetHTBindingValues (unsigned int _Category, string _Key, vector<string> *&_Values);

  // Set a value behind a key
  int StoreHTBindingValues (unsigned int _Category, string _Key, vector<string> *_Values, Block *_PB);

  // List local HT block name bindings
  void ListBindings ();

  // *************************************************************************************
  // Functions to discover IDs on local process HT data.
  // *************************************************************************************

  // Note: Domain search will be implemented on SDS process

  // Discover the values behind a legible name on category 2
  int DiscoverHomonymsEntitiesIDsFromLN (unsigned int _Cat, string _LN, vector<string> *&_Values, Block *_PB);

  // Discover the values behind a legible name on category 2
  int DiscoverHomonymsEntitiesIDsFromLN (unsigned int _Cat, string _LN, Block *_PB);

  // Discover inside a Process the Blocks that have the same legible names.
  int DiscoverHomonymsBlocksBIDsFromPID (string _PID, string _BlockLN, vector<string> *&_BIDs, Block *_PB);

  // Discover inside an OS the Processes that have the same legible names.
  int DiscoverHomonymsProcessesPIDsFromOSID (string _OSID, string _ProcessLN, vector<string> *&_PIDs, Block *_PB);

  // Discover inside a Process the Blocks that have the same legible names.
  int
  DiscoverHomonymsBlocksBIDsFromProcessLegibleName (string _ProcessLN, string _BlockLN, string &_PID, vector<string> *&_BIDs, Block *_PB);

  // Discover inside a Domain entities' 4 level-tuples that have the same legible names.
  int
  DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames (string _ProcessLN, string _BlockLN, vector<Tuple *> &_4LTuple, Block *_PB);

  // Discover inside a Process the existent Blocks
  int DiscoverBIDsFromPID (string _PID, vector<string> *&_BIDs, Block *_PB);

  // Discover inside an OS the existent Processes
  int DiscoverPIDsFromOSID (string _OSID, vector<string> *&_PIDs, Block *_PB);

  // Discover inside a Host the existent OSs
  int DiscoverOSIDsFromHID (string _HID, vector<string> *&_OSIDs, Block *_PB);

  // Discover inside a Domain the existent Hosts
  int DiscoverHIDsFromDID (string _DID, vector<string> *&_HIDs, Block *_PB);

  // Return the current time in seconds
  double GetTime ();

  // Convert double to string
  string DoubleToString (double _Double);

  // Converting string to double
  double StringToDouble (string _String);

  // Auxiliary
  friend class Block;
};

#endif
