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

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

// #define DEBUG

#define LOG(msg) PB->S << endl \
                       << "[" << fixed << setprecision(3) << GetTime() << "s] " << Offset << msg << endl

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
    if (NA >= 1)
    {
      // Get the peer key and legible name (always present, argument 0)
      _PCL->GetArgument(0, PeerData);

      // Get the optional HT BID (argument 1, present in v2.0+ from PGCS)
      string PeerHTBID = "";
      if (NA >= 2)
      {
        vector<string> HTData;
        _PCL->GetArgument(1, HTData);
        if (HTData.size() > 0)
        {
          PeerHTBID = HTData.at(0);
        }
      }

      // Get the sending process PID and GW block BID from the -m --cl command line
      CommandLine* GWMsgCl01 = 0;
      _ReceivedMessage->GetCommandLine(0, GWMsgCl01);

      if (GWMsgCl01 != 0)
      {
        GWMsgCl01->GetArgument(1, ReceivedMessageSources);

        // Check if this peer is already known (Category 20: PID -> LN)
        vector<string>* ExistingLN = new vector<string>;
        bool alreadyKnown = (PGW->GetHTBindingValues(20, ReceivedMessageSources.at(0), ExistingLN) == OK && ExistingLN->size() > 0);
        delete ExistingLN;

        if (alreadyKnown)
        {
          // Peer already discovered — skip redundant store and logging
#ifdef DEBUG
          PB->S << Offset << "Already aware of the peer service: " << ReceivedMessageSources.at(0) << endl;
#endif
        }
        else
        {
          LOG("(Discovered the peer service = " << PeerData.at(1) << " via shared memory. IPC is working properly.)");

          // ******************************************************
          // Store all peer bindings in the local HT
          // ReceivedMessageSources.at(0) is the sender PID from -m --cl source field
          // ReceivedMessageSources.at(1) is the sender BID from -m --cl source field
          // PeerData.at(0) is the peer IPC key (hash), PeerData.at(1) is the legible name
          StorePeerBindings(_ReceivedMessage, _PCL, ReceivedMessageSources.at(0), ReceivedMessageSources.at(1), PeerData.at(0), PeerData.at(1), PeerHTBID);
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
// _PeerHTBID: optional peer HT block BID (v2.0+, PGCS only)
int GWHelloIPC02::StorePeerBindings(Message* _ReceivedMessage, CommandLine* _PCL, string& _PeerPID, string& _PeerBID, string& _PeerIPCKey, string& _PeerLN, string& _PeerHTBID)
{
  int Status = OK;
  unsigned int Category;
  string Key;
  vector<string> Values;
  string HashLegiblePeerProcessName;
  string Offset = "          ";
  GW* PGW = 0;

  PGW = (GW*)PB;

  HashLegiblePeerProcessName = NameGenerator::GetInstance().GenerateFromString(_PeerLN);

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

  // Always store the GW BID
  Values.push_back(_PeerBID);

  // Also store the HT BID when available (v2.0+ from PGCS)
  // This enables DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS","HT",...)
  // to find the HT BID via intersection between Cat[5] and Cat[2] Hash("HT")
  if (!_PeerHTBID.empty() && _PeerHTBID != _PeerBID)
  {
    Values.push_back(_PeerHTBID);
  }

  PGW->StoreHTBindingValues(Category, Key, &Values);
  Values.clear();

  // ******************************************************
  // Binding: Hash("HT") -> HT_BID (Category 2) [v2.0+ only]
  // ******************************************************

  if (!_PeerHTBID.empty())
  {
    string HashHT;
    HashHT = NameGenerator::GetInstance().GenerateFromString("HT");

    Category = 2;
    Key = HashHT;
    Values.push_back(_PeerHTBID);
    PGW->StoreHTBindingValues(Category, Key, &Values);
    Values.clear();

    LOG("(Stored HT BID " << _PeerHTBID << " for peer " << _PeerLN << " in Cat[2] Hash(\"HT\"))");
  }

  return Status;
}