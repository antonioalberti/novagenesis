/*
	NovaGenesis

	Name:		GWHelloIPC00
	Object:		GWHelloIPC00
	File:		GWHelloIPC00.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2026
	Version:	0.1

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

#ifndef _GWHELLOIPC00_H
#include "GWHelloIPC00.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#define DEBUG

GWHelloIPC00::GWHelloIPC00(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

GWHelloIPC00::~GWHelloIPC00()
{
}

// Run the actions behind a received command line
// ng -hello --ipc 0.0 [ < 2 string Peer_Key Peer_LN > ]
int GWHelloIPC00::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
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
        // ******************************************************

        StorePeerBindings(_ReceivedMessage, _PCL, PeerData.at(0), PeerData.at(1));

        // ******************************************************
        // If this is the PGCS, forward the hello to all other known peers
        // ******************************************************

        if (PB->PP->GetLegibleName() == "PGCS")
        {
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

// Store peer bindings in local HT (same 5 categories as original GWHelloIPC01)
int GWHelloIPC00::StorePeerBindings(Message* _ReceivedMessage, CommandLine* _PCL, string& _PeerKey, string& _PeerLN)
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
  // Binding: PID -> IPC Input Key (Category 13)
  // ******************************************************

  Category = 13;
  Key = _PeerKey;
  Values.push_back(_PeerKey);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding Hash("Legible Process Name") -> PID (Category 2)
  // ******************************************************

  Category = 2;
  Key = HashLegiblePeerProcessName;
  Values.push_back(_PeerKey);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding: PID -> Hash("Legible Process Name") (Category 3)
  // ******************************************************

  Category = 3;
  Key = _PeerKey;
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
  // Binding: PID -> BID (Category 5)
  // ******************************************************

  Category = 5;
  Key = _PeerKey;
  Values.push_back(_PeerKey);
  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  return Status;
}

// Forward hello to all other known peers (only on PGCS)
int GWHelloIPC00::ForwardToPeers(Message* _ReceivedMessage, CommandLine* _PCL, string& _SenderPID)
{
  int Status = OK;
  string Offset = "          ";
  GW* PGW = 0;
  vector<string> KnownPIDs;

  PGW = (GW*)PB;

  // Get all PIDs from category 13 (PID -> IPC Key)
  if (PGW->PHT->GetBindingKeys(13, KnownPIDs) == OK)
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

      // Get the IPC key for this peer
      vector<string>* PeerKeys = new vector<string>;
      if (PGW->GetHTBindingValues(13, peerPID, PeerKeys) == OK && PeerKeys->size() > 0)
      {
        string peerIPCKey = PeerKeys->at(0);

        // Create a copy of the hello message for this peer
        Message* HelloCopy = NULL;
        CommandLine* CopyPCL = NULL;

        if (CreateHelloCopy(_ReceivedMessage, _PCL, HelloCopy, CopyPCL) == OK)
        {
          PB->S << Offset << "(Forwarding hello to peer with key = " << peerIPCKey << ")" << endl;
          PGW->PushToOutputQueue(peerIPCKey, HelloCopy);
        }
        else
        {
          PB->S << Offset << "(ERROR: Failed to create hello copy for peer)" << endl;
        }
      }

      delete PeerKeys;
    }
  }
  else
  {
    PB->S << Offset << "(No known peers to forward to)" << endl;
  }

  return Status;
}

// Create a copy of the hello message for forwarding to another peer
int GWHelloIPC00::CreateHelloCopy(Message* _OriginalMessage, CommandLine* _OriginalPCL, Message*& _CopyMessage, CommandLine*& _CopyPCL)
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
  string Version = "0.1";

  GWMsgCl01->GetArgument(0, Limiters);
  GWMsgCl01->GetArgument(1, Sources);
  GWMsgCl01->GetArgument(2, Destinations);

  // Rebuild the message with the same -m --cl header
  PMB->NewConnectionLessCommandLine(Version, &Limiters, &Sources, &Destinations, NewMsg, NewPCL);

  // Add the -hello --ipc command line with same peer data
  PMB->NewIPCHelloCommandLine("--ipc", Version, PB->PP->Key, PB->PP->GetLegibleName(), NewMsg, NewPCL);

  // Generate the SCN
  string SCN = "FFFFFFFF";
  PB->GenerateSCNFromMessageBinaryPatterns(NewMsg, SCN);
  PMB->NewSCNCommandLine(Version, SCN, NewMsg, NewPCL);

  // Copy the relevant data
  _CopyMessage = NewMsg;
  _CopyPCL = NewPCL;

  return Status;
}
