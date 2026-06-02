/*
        NovaGenesis

        Name:		GWHelloIPC02
        Object:		GWHelloIPC02
        File:		GWHelloIPC02.cpp
        Author:		Antonio Marcos Alberti
        Date:		05/2026
        Version:	0.2

   Copyright (C) 2026  Antonio Marcos Alberti

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

#ifndef _GWHelloIPC02_H
#include "GWHelloIPC02.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

// #define DEBUG

GWHelloIPC02::GWHelloIPC02(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

GWHelloIPC02::~GWHelloIPC02()
{
}

// Run the actions behind a received command line
// ng -hello --ipc 0.2 [ < 2 string Peer_Key Peer_LN > ]
int GWHelloIPC02::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = OK;
  unsigned int NA = 0;
  vector<string> PeerData;
  vector<string> ReceivedMessageSources;
  GW* PGW = 0;
  string Offset = "          ";

  PGW = (GW*)PB;

  // Get the number of arguments
  if (_PCL->GetNumberofArguments(NA) == OK)
  {
    if (NA == 1)
    {
      // Get the peer key and legible name
      _PCL->GetArgument(0, PeerData);

      // Get the sending process PID and GW block BID from the -m --cl command line
      CommandLine* GWMsgCl01 = 0;
      _ReceivedMessage->GetCommandLine(0, GWMsgCl01);

      if (GWMsgCl01 != 0)
      {
        GWMsgCl01->GetArgument(1, ReceivedMessageSources);

        PB->S << Offset << "(Discovered the peer service = " << PeerData.at(1) << " via shared memory. IPC is working properly.)" << endl;

        // ******************************************************
        // Store all peer bindings in the local HT
        // ReceivedMessageSources.at(0) is the sender PID from -m --cl source field
        // PeerData.at(0) is the peer IPC key (hash), PeerData.at(1) is the legible name
        StorePeerBindings(_ReceivedMessage, _PCL, ReceivedMessageSources.at(0), PeerData.at(0), PeerData.at(1));

        // ******************************************************
        // If this is the PGCS, send a hello reply to the sender
        // and forward the hello to all other known peers
        // ******************************************************

        if (PB->PP->GetLegibleName() == "PGCS")
        {
          // Get the sender's IPC key from Category 19 (PeerPID -> IPCKey)
          string senderIPCKey = "";
          vector<string>* SenderKeys = new vector<string>;
          if (PGW->GetHTBindingValues(19, ReceivedMessageSources.at(0), SenderKeys) == OK && SenderKeys->size() > 0)
          {
            senderIPCKey = SenderKeys->at(0);
          }
          delete SenderKeys;

          if (!senderIPCKey.empty())
          {
            // Build a hello IPC reply message with PGCS's own key and name
            Message* HelloReply = 0;
            CommandLine* ReplyPCL = 0;
            vector<string> Limiters;
            vector<string> Sources;
            vector<string> Destinations;
            string ReplyVersion = "0.2";

            PB->PP->NewMessage(GetTime(), 0, false, HelloReply);

            Limiters.push_back(PB->PP->Intra_OS);
            Sources.push_back(PB->PP->GetSelfCertifyingName());
            Sources.push_back(PB->GetSelfCertifyingName());
            Destinations.push_back("FFFFFFFF");
            Destinations.push_back("FFFFFFFF");

            PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, HelloReply, ReplyPCL);
            PMB->NewIPCHelloCommandLine("--ipc", ReplyVersion, PB->PP->Key, PB->PP->GetLegibleName(), HelloReply, ReplyPCL);

            string SCN = "FFFFFFFF";
            PB->GenerateSCNFromMessageBinaryPatterns(HelloReply, SCN);
            PMB->NewSCNCommandLine("0.1", SCN, HelloReply, ReplyPCL);

            PB->S << Offset << "(Sending hello IPC reply to peer " << PeerData.at(1) << " with key = " << senderIPCKey << ")" << endl;
            PGW->PushToOutputQueue(senderIPCKey, HelloReply);
          }
          else
          {
            PB->S << Offset << "(ERROR: Unable to get IPC key for sender PID " << ReceivedMessageSources.at(0) << ")" << endl;
          }

          // Forward the hello to all other known peers (not the sender)
          ForwardToPeers(_ReceivedMessage, _PCL, ReceivedMessageSources.at(0));
        }
      }
      else
      {
        PB->S << Offset << "(ERROR: Unable to get the MsgCl command line)" << endl;
        Status = ERROR;
      }
    }
    else
    {
      PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;
      Status = ERROR;
    }
  }
  else
  {
    PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
    Status = ERROR;
  }

  return Status;
}

// Store peer bindings in local HT
// _PeerPID: sender PID from -m --cl (used as key for cat 1, 3, 5)
// _PeerIPCKey: peer IPC key (hash from hello command, used as value for cat 19)
// _PeerLN: peer legible name
int GWHelloIPC02::StorePeerBindings(Message* _ReceivedMessage, CommandLine* _PCL, string& _PeerPID, string& _PeerIPCKey, string& _PeerLN)
{
  int Status = OK;
  unsigned int Category;
  string Key;
  vector<string> Values;
  string HashLegiblePeerProcessName;
  string Offset = "          ";
  GW* PGW = 0;

  PGW = (GW*)PB;

  PB->GenerateSCNFromCharArrayBinaryPatterns(_PeerLN, HashLegiblePeerProcessName);

  // ******************************************************
  // Binding: PeerPID -> LegibleName (Category 20)
  // ******************************************************

  Category = 20;
  Key = _PeerPID;
  Values.push_back(_PeerLN);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding: PeerPID -> IPC Key (Category 19)
  // ******************************************************

  Category = 19;
  Key = _PeerPID;
  Values.push_back(_PeerIPCKey);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding Hash("Legible Process Name") -> PeerPID (Category 2)
  // ******************************************************

  Category = 2;
  Key = HashLegiblePeerProcessName;
  Values.push_back(_PeerPID);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding: PeerPID -> Hash("Legible Process Name") (Category 3)
  // ******************************************************

  Category = 3;
  Key = _PeerPID;
  Values.push_back(HashLegiblePeerProcessName);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding Hash("Legible Process Name") -> HID (Category 9)
  // ******************************************************

  Category = 9;
  Key = HashLegiblePeerProcessName;
  Values.push_back(PB->PP->GetHostSelfCertifyingName());
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding: PeerPID -> BID (Category 5)
  // ******************************************************

  Category = 5;
  Key = _PeerPID;
  Values.push_back(_PeerPID);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  return Status;
}

// Forward hello to all other known peers (only on PGCS)
// Skips peers whose IPC input key is one of this process's own SHM segment keys
// (PP->Key + z for z in 0..NUMBER_OF_PARALLEL_SHARED_MEMORIES-1), avoiding wasted
// writes to segments with no reader.
int GWHelloIPC02::ForwardToPeers(Message* _ReceivedMessage, CommandLine* _PCL, string& _SenderPID)
{
  int Status = OK;
  string Offset = "          ";
  GW* PGW = 0;
  vector<string> KnownPIDs;

  PGW = (GW*)PB;

  long selfBaseKey = PB->PP->Key;

  // Build the set of this process's own SHM input segment keys
  // so we never forward hellos to our own segments (no reader there)
  vector<string> selfInputKeys;
  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES; z++)
  {
    selfInputKeys.push_back(std::to_string(selfBaseKey + z));
  }

  // Get all PIDs from category 19 (hello IPC discovered peers only)
  // Category 19 only has peers discovered via hello IPC.
  // This prevents forwarding hellos to segments with no reader.
  if (PGW->PHT->GetBindingKeys(19, KnownPIDs) == OK)
  {
    PB->S << Offset << "(Forwarding hello IPC to " << KnownPIDs.size() << " known peers)" << endl;

    for (unsigned int i = 0; i < KnownPIDs.size(); i++)
    {
      string peerPID = KnownPIDs.at(i);

      // Do not forward back to the sender
      if (peerPID == _SenderPID)
      {
        continue;
      }

      // Get the IPC key for this peer (category 19: PeerKey -> IPCKey)
      string peerIPCKey = "";
      vector<string>* PeerKeys = new vector<string>;
      if (PGW->GetHTBindingValues(19, peerPID, PeerKeys) == OK && PeerKeys->size() > 0)
      {
        peerIPCKey = PeerKeys->at(0);
      }
      delete PeerKeys;

      if (peerIPCKey.empty())
      {
        continue;
      }

      // Get peer legible name (category 1: PeerKey -> LegibleName)
      string peerLN = "";
      vector<string>* PeerNames = new vector<string>;
      if (PGW->GetHTBindingValues(1, peerPID, PeerNames) == OK && PeerNames->size() > 0)
      {
        peerLN = PeerNames->at(0);
      }
      delete PeerNames;

      // Do not forward to any of this process's own SHM input segment keys
      // This prevents writing hellos to segments that have no reader
      bool isSelfSegment = false;
      for (unsigned int z = 0; z < selfInputKeys.size(); z++)
      {
        if (peerIPCKey == selfInputKeys.at(z))
        {
          isSelfSegment = true;
          break;
        }
      }

      if (isSelfSegment)
      {
        PB->S << Offset << "(Skipping peer " << peerLN << " with key = " << peerIPCKey << ": self input segment)" << endl;
        continue;
      }

      // Create a copy of the hello message for this peer
      Message* HelloCopy = NULL;
      CommandLine* CopyPCL = NULL;

      if (CreateHelloCopy(_ReceivedMessage, _PCL, HelloCopy, CopyPCL) == OK)
      {
        PB->S << Offset << "(Forwarding hello to peer " << peerLN << " with key = " << peerIPCKey << ")" << endl;
        PGW->PushToOutputQueue(peerIPCKey, HelloCopy);
      }
      else
      {
        PB->S << Offset << "(ERROR: Failed to create hello copy for peer " << peerLN << ")" << endl;
      }
    }
  }
  else
  {
    PB->S << Offset << "(No known peers to forward to)" << endl;
  }

  return Status;
}

// Create a copy of the hello message for forwarding to another peer
int GWHelloIPC02::CreateHelloCopy(Message* _OriginalMessage, CommandLine* _OriginalPCL, Message*& _CopyMessage, CommandLine*& _CopyPCL)
{
  int Status = OK;
  vector<string> PeerData;
  CommandLine* GWMsgCl01 = 0;
  GW* PGW = 0;
  string Offset = "          ";

  PGW = (GW*)PB;

  // Get the peer key and legible name from the original hello command line
  _OriginalPCL->GetArgument(0, PeerData);

  if (PeerData.size() < 2)
  {
    return ERROR;
  }

  // Get the MsgCl command line from the original message
  _OriginalMessage->GetCommandLine(0, GWMsgCl01);

  if (GWMsgCl01 == 0)
  {
    return ERROR;
  }

  // Create a new message
  PB->PP->NewMessage(GetTime(), 0, false, _CopyMessage);

  // Copy the -m --cl command line
  _CopyMessage->ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2();

  // Create new message by copying the original
  // We'll rebuild the message from scratch with the same content
  Message* NewMsg = NULL;
  PB->PP->NewMessage(GetTime(), 0, false, NewMsg);
  CommandLine* NewPCL = NULL;

  // Get the limiter/sources/destinations from the -m --cl header
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string Version = "0.2";

  GWMsgCl01->GetArgument(0, Limiters);
  GWMsgCl01->GetArgument(1, Sources);
  GWMsgCl01->GetArgument(2, Destinations);

  // Rebuild the message with the same -m --cl header
  string CLVersion = "0.1";
  PMB->NewConnectionLessCommandLine(CLVersion, &Limiters, &Sources, &Destinations, NewMsg, NewPCL);

  // Add the -hello --ipc command line with same peer data
  PMB->NewIPCHelloCommandLine("--ipc", Version, PB->PP->Key, PB->PP->GetLegibleName(), NewMsg, NewPCL);

  // Generate the SCN
  string SCN = "FFFFFFFF";
  PB->GenerateSCNFromMessageBinaryPatterns(NewMsg, SCN);
  string SCNVersion = "0.1";
  PMB->NewSCNCommandLine(SCNVersion, SCN, NewMsg, NewPCL);

  // Copy the relevant data
  _CopyMessage = NewMsg;
  _CopyPCL = NewPCL;

  return Status;
}
