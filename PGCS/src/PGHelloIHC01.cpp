/*
        NovaGenesis

        Name:		PGHelloIHC01
        Object:		PGHelloIHC01
        File:		PGHelloIHC01.cpp
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

#ifndef _PGHELLOIHC01_H
#include "PGHelloIHC01.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGS_H
#include "PGCS.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

// #define DEBUG

#define LOG(msg) PB->S << endl \
                       << "[" << fixed << setprecision(3) << GetTime() << "s] " << Offset << msg << endl

PGHelloIHC01::PGHelloIHC01(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

PGHelloIHC01::~PGHelloIHC01()
{
}

// Run the actions behind a received command line
// ng -hello --ihc _Version [ < HID OSID PGCS_PID PG_BID GW_SCN HT_SCN _Stack _Interface _Identifier > ... < HID OSID PGCS_PID PG_BID GW_SCN HT_SCN _Stack _Interface _Identifier > ]
int PGHelloIHC01::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
// TODO: FIXP/Update - You should replace all the implementation of this function
{
  int Status = OK;
  PG* PPGB = 0;
  string HID;
  string PID;
  string BID;
  vector<string> ReceivedElements;
  string Offset = "                    ";
  string MyStack;          // The local stack
  string MyInterface;      // The local interface
  string MyPeerIdentifier; // The local address
  string PeerStack;        // The peer stack
  string PeerInterface;    // The peer interface
  string PeerIdentifier;   // The peer address
  string Peer;
  PGCS* PPGCS = 0;
  bool StoreNewPGCS1 = true;
  bool StoreNewPGCS2 = true;
  Block* PHTB = 0;
  CommandLine* PCL = 0;
  CommandLine* PSCNCL = 0;
  unsigned int NoA = 0;

  PPGCS = (PGCS*)PB->PP;
  PPGB = (PG*)PB;
  PHTB = (Block*)PPGB->PHT;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName() << endl;

#endif

  if (_PCL->GetNumberofArguments(NoA) == OK)
  {
    for (unsigned int k = 0; k < NoA; k++)
    {
      _PCL->GetArgument(k, ReceivedElements);

      // *****************************************************************************
      // Check if there is a compatible local tuple <stack, interface, identifier>
      // *****************************************************************************
      for (unsigned int i = 0; i < PPGCS->Stacks->size(); i++)
      {
        MyStack = PPGCS->Stacks->at(i);

        MyInterface = PPGCS->Interfaces->at(i);

        MyPeerIdentifier = PPGCS->Identifiers->at(i);

        PeerStack = ReceivedElements.at(6);

        PeerInterface = ReceivedElements.at(7);

        PeerIdentifier = ReceivedElements.at(8);

#ifdef DEBUG
        PB->S << endl
              << Offset << "Index = " << i << endl;
        PB->S << Offset << "(Check if this stack " << MyStack << " is equal to the hello stack " << PeerStack
              << ")" << endl;
        PB->S << Offset << "(Check if this identifier " << MyPeerIdentifier
              << " is equal to the hello identifier " << PeerIdentifier << ")" << endl;
        PB->S << Offset << "MyInterface = " << MyInterface << endl;
        PB->S << Offset << "PeerInterface = " << PeerInterface << endl;
#endif
        if (MyStack == PeerStack)
        {
          if (PeerIdentifier == MyPeerIdentifier)
          {
#ifdef DEBUG
            PB->S << Offset << "Going to check this previous informed PGCS" << endl;
#endif
            // ******************************************************
            // Check to record the new peer data
            // ******************************************************

            for (unsigned int j = 0; j < PPGB->PGCSTuples.size(); j++)
            {
              if (ReceivedElements.at(2) == PPGB->PGCSTuples[j]->Values[2])
              {
                StoreNewPGCS1 = false;

                break;
              }
            }

            if (StoreNewPGCS1 == true && ReceivedElements.at(0) != PB->PP->GetHostSelfCertifyingName()) // Just one PGCS per host
            {
              LOG("(A new peer PGCS was registered via point to point: "
                  << ReceivedElements.at(2) << " at the node identified by " << PeerIdentifier << ")");

              Tuple* PeerPGCS = new Tuple;

              PeerPGCS->Values.push_back(ReceivedElements.at(0));
              PeerPGCS->Values.push_back(ReceivedElements.at(1));
              PeerPGCS->Values.push_back(ReceivedElements.at(2));
              PeerPGCS->Values.push_back(ReceivedElements.at(3));

              PB->S << Offset << "(HID = " << ReceivedElements.at(0) << ")" << endl;
              PB->S << Offset << "(OSID = " << ReceivedElements.at(1) << ")" << endl;
              PB->S << Offset << "(PID = " << ReceivedElements.at(2) << ")" << endl;
              PB->S << Offset << "(BID = " << ReceivedElements.at(3) << ")" << endl;

              // Store the peer PGCS tuple for future use
              PPGB->PGCSTuples.push_back(PeerPGCS);

              StoreNewPGCS1 = false;

              ScheduleStoreBindings("-p", ReceivedElements, PeerIdentifier, PeerStack);
            }
          }
          else
          {

            // ****************************************************************************************
            // Added in April 22th, 2016; Updated in 26th August 2021
            // ****************************************************************************************

#ifdef DEBUG
            PB->S << Offset << "This PGCS is unknown" << endl;
#endif

            // ******************************************************
            // Record the new peer data
            // ******************************************************

            for (unsigned int j = 0; j < PPGB->PGCSTuples.size(); j++)
            {
              if (ReceivedElements.at(2) == PPGB->PGCSTuples[j]->Values[2])
              {
                StoreNewPGCS2 = false;

                break;
              }
            }

#ifdef DEBUG
            PB->S << Offset << "Trying to store the discovered peer = " << StoreNewPGCS2 << endl;
#endif

            if (StoreNewPGCS2 == true && ReceivedElements.at(0) != PB->PP->GetHostSelfCertifyingName()) // Just one PGCS per host
            {
              PB->S << Offset << "(A new peer PGCS was discovered without previous knowledge: "
                    << ReceivedElements.at(2) << " at the node identified by " << PeerIdentifier << ")"
                    << endl;

              Tuple* PeerPGS = new Tuple;

              PeerPGS->Values.push_back(ReceivedElements.at(0));
              PeerPGS->Values.push_back(ReceivedElements.at(1));
              PeerPGS->Values.push_back(ReceivedElements.at(2));
              PeerPGS->Values.push_back(ReceivedElements.at(3));

              PB->S << Offset << "(HID = " << ReceivedElements.at(0) << ")" << endl;
              PB->S << Offset << "(OSID = " << ReceivedElements.at(1) << ")" << endl;
              PB->S << Offset << "(PID = " << ReceivedElements.at(2) << ")" << endl;
              PB->S << Offset << "(BID = " << ReceivedElements.at(3) << ")" << endl;

              // Store the peer PGCS tuple for future use
              PPGB->PGCSTuples.push_back(PeerPGS);

              ScheduleStoreBindings("-de", ReceivedElements, PeerIdentifier, PeerStack);
            }
            else
            {
              // PB->S << Offset << "(Warning: Peer already registered or it is this PGCS itself)"<<endl;

              StoreNewPGCS2 = true;
            }
          }
        }
      }

      ReceivedElements.clear();
    }
  }

  Status = OK;

  // PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

int PGHelloIHC01::ScheduleStoreBindings(string _Case, vector<string>& _ReceivedElements, string _PeerIdentifier, string _PeerStack)
{
  int Status = OK;

  PG* PPGB = 0;
  unsigned int Category;
  string Key;
  vector<string> Values;
  string Offset = "                    ";
  string HashOS;
  string HashHost;
  string HashPGCS;
  string HashPG;
  string HashGW;
  string HashHT;
  PGCS* PPGCS = 0;

  PPGCS = (PGCS*)PB->PP;
  PPGB = (PG*)PB;

  // SPEC-008 Rev2: Store bindings directly in the HT to eliminate
  // race condition between message queue processing and
  // PGRunExposition01 execution. The message -sr --b was removed
  // because it was redundant — the only consumer was HTStoreBind01
  // which stored in the same HT.

  // Generate hash keys
  HashHost = NameGenerator::GetInstance().GenerateFromString("Host");
  HashOS = NameGenerator::GetInstance().GenerateFromString("OS");
  HashPGCS = NameGenerator::GetInstance().GenerateFromString("PGCS");
  HashPG = NameGenerator::GetInstance().GenerateFromString("PG");
  HashGW = NameGenerator::GetInstance().GenerateFromString("GW");
  HashHT = NameGenerator::GetInstance().GenerateFromString("HT");

  // ******************************************************
  // Cat[6] HID -> OSID
  // ******************************************************
  Category = 6;
  Key = _ReceivedElements.at(0);
  Values.clear();
  Values.push_back(_ReceivedElements.at(1));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[7] OSID -> HID
  // ******************************************************
  Category = 7;
  Key = _ReceivedElements.at(1);
  Values.clear();
  Values.push_back(_ReceivedElements.at(0));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[9] Hash("Host") -> HID
  // ******************************************************
  Category = 9;
  Key = HashHost;
  Values.clear();
  Values.push_back(_ReceivedElements.at(0));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[8] HID -> "Host"
  // ******************************************************
  Category = 8;
  Key = _ReceivedElements.at(0);
  Values.clear();
  Values.push_back("Host");
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[2] Hash("OS") -> OSID
  // ******************************************************
  Category = 2;
  Key = HashOS;
  Values.clear();
  Values.push_back(_ReceivedElements.at(1));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[3] OSID -> Hash("OS")
  // ******************************************************
  Category = 3;
  Key = _ReceivedElements.at(1);
  Values.clear();
  Values.push_back(HashOS);
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[5] OSID -> PID
  // ******************************************************
  Category = 5;
  Key = _ReceivedElements.at(1);
  Values.clear();
  Values.push_back(_ReceivedElements.at(2));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[7] PID -> HID
  // ******************************************************
  Category = 7;
  Key = _ReceivedElements.at(2);
  Values.clear();
  Values.push_back(_ReceivedElements.at(0));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[2] Hash("PGCS") -> PID
  // ******************************************************
  Category = 2;
  Key = HashPGCS;
  Values.clear();
  Values.push_back(_ReceivedElements.at(2));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[3] PID -> Hash("PGCS")
  // ******************************************************
  Category = 3;
  Key = _ReceivedElements.at(2);
  Values.clear();
  Values.push_back(HashPGCS);
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[5] PID -> PG_BID (CRITICAL — for DiscoverHomonymsBlocksBIDsFromPID)
  // ******************************************************
  Category = 5;
  Key = _ReceivedElements.at(2);
  Values.clear();
  Values.push_back(_ReceivedElements.at(3));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[2] Hash("PG") -> PG_BID
  // ******************************************************
  Category = 2;
  Key = HashPG;
  Values.clear();
  Values.push_back(_ReceivedElements.at(3));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[3] PG_BID -> Hash("PG")
  // ******************************************************
  Category = 3;
  Key = _ReceivedElements.at(3);
  Values.clear();
  Values.push_back(HashPG);
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[5] PID -> GW_BID (CRITICAL — for DiscoverHomonymsBlocksBIDsFromPID)
  // ******************************************************
  Category = 5;
  Key = _ReceivedElements.at(2);
  Values.clear();
  Values.push_back(_ReceivedElements.at(4));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[2] Hash("GW") -> GW_BID
  // ******************************************************
  Category = 2;
  Key = HashGW;
  Values.clear();
  Values.push_back(_ReceivedElements.at(4));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[3] GW_BID -> Hash("GW")
  // ******************************************************
  Category = 3;
  Key = _ReceivedElements.at(4);
  Values.clear();
  Values.push_back(HashGW);
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[5] PID -> HT_BID (CRITICAL — for DiscoverHomonymsBlocksBIDsFromPID)
  // ******************************************************
  Category = 5;
  Key = _ReceivedElements.at(2);
  Values.clear();
  Values.push_back(_ReceivedElements.at(5));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[2] Hash("HT") -> HT_BID (CRITICAL — for DiscoverHomonymsBlocksBIDsFromPID)
  // ******************************************************
  Category = 2;
  Key = HashHT;
  Values.clear();
  Values.push_back(_ReceivedElements.at(5));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Cat[3] HT_BID -> Hash("HT")
  // ******************************************************
  Category = 3;
  Key = _ReceivedElements.at(5);
  Values.clear();
  Values.push_back(HashHT);
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // Peer address bindings (Cat[8] / Cat[15])
  // ******************************************************

  // Cat[8] HID -> Peer Stack Hash
  string HashPeerStack;
  HashPeerStack = NameGenerator::GetInstance().GenerateFromString(_PeerStack);
  Category = 8;
  Key = _ReceivedElements.at(0);
  Values.clear();
  Values.push_back(HashPeerStack);
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[15] HID -> Peer Identifier
  Category = 15;
  Key = _ReceivedElements.at(0);
  Values.clear();
  Values.push_back(_PeerIdentifier);
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // Cat[15] Peer Identifier -> HID
  Category = 15;
  Key = _PeerIdentifier;
  Values.clear();
  Values.push_back(_ReceivedElements.at(0));
  PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);

  // ******************************************************
  // -de mode: associate client socket with CSID
  // ******************************************************
  if (_Case == "-de" && PPGCS->CSIDs->size() > 0)
  {
    PB->S << Offset << "(Associating the client socket with CSID " << PPGCS->CSIDs->at(0)
          << " for the peer PGCS with MAC "
          << _PeerIdentifier << ")" << endl;

    // Cat[15] Peer Identifier -> CSID
    Category = 15;
    Key = _PeerIdentifier;
    Values.clear();
    Values.push_back(PB->IntToString(PPGCS->CSIDs->at(0)));
    PPGB->PGW->StoreHTBindingValues(Category, Key, &Values);
  }

  Status = OK;

  return Status;
}
