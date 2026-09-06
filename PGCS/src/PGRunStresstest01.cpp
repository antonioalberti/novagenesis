/*
        NovaGenesis

        Name:		PGRunStresstest01
        Object:		PGRunStresstest01
        File:		PGRunStresstest01.cpp
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

#ifndef _PGRUNSTRESSTEST01_H
#include "PGRunStresstest01.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

#define DEBUG
// #define DEBUG

PGRunStresstest01::PGRunStresstest01(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

PGRunStresstest01::~PGRunStresstest01()
{
}

// Run the actions behind a received command line
// ng -run --stresstest 0.1
int PGRunStresstest01::Run(Message* _ReceivedMessage, CommandLine* _PCL,
                           vector<Message*>& ScheduledMessages,
                           Message*& InlineResponseMessage)
{
  int Status = OK;
  PG* PPG = 0;
  PGCS* PPGCS = 0;
  Message* StressPing = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  CommandLine* PCL = 0;
  string PeerHID;
  string PeerOSID;
  string PeerPID;
  string PeerBID;
  string Offset = "                    ";

  PPG = (PG*)PB;
  PPGCS = (PGCS*)PB->PP;

#ifdef DEBUG
  PB->S << Offset << this->GetLegibleName() << endl;
  PB->S << Offset << "(StressEnabled = " << PPG->StressEnabled << ")" << endl;
  PB->S << Offset << "(PGCSTuples.size = " << PPG->PGCSTuples.size() << ")" << endl;
#endif

  if (!PPG->StressEnabled)
  {
#ifdef DEBUG
    PB->S << Offset << "(StressTest is DISABLED — skipping)" << endl;
#endif
    return OK;
  }

  // Loop over DISCOVERED peers (PGCSTuples, populated by PGHelloIHC01)
  for (unsigned int p = 0; p < PPG->PGCSTuples.size(); p++)
  {
    PeerHID = PPG->PGCSTuples[p]->Values[0];
    PeerOSID = PPG->PGCSTuples[p]->Values[1];
    PeerPID = PPG->PGCSTuples[p]->Values[2];
    PeerBID = PPG->PGCSTuples[p]->Values[3];

#ifdef DEBUG
    PB->S << Offset << "(Peer " << p << ": HID=" << PeerHID
          << " OSID=" << PeerOSID << " PID=" << PeerPID
          << " BID=" << PeerBID << ")" << endl;
#endif

    // Create IHC message addressed to peer's PG block
    PB->PP->NewMessage(GetTime(), 0, false, StressPing);

    Limiters.clear();
    Sources.clear();
    Destinations.clear();

    // Space limiter: Intra_Domain
    Limiters.push_back(PB->PP->Intra_Domain);

    // Sources: our full tuple (HID, OSID, PID, BID)
    Sources.push_back(PB->PP->GetHostSelfCertifyingName());
    Sources.push_back(PB->PP->GetOperatingSystemSelfCertifyingName());
    Sources.push_back(PB->PP->GetSelfCertifyingName());
    Sources.push_back(PB->GetSelfCertifyingName());

    // Destinations: peer's full tuple (HID, OSID, PID, BID)
    Destinations.push_back(PeerHID);
    Destinations.push_back(PeerOSID);
    Destinations.push_back(PeerPID);
    Destinations.push_back(PeerBID);

    // Create connectionless header
    PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, StressPing, PCL);

    // Add stress ping command line
    StressPing->NewCommandLine("-stresstest", "--ping", "0.1", PCL);

    // Add counter as argument (for testing serialization)
    PCL->NewArgument(1);
    PCL->SetArgumentElement(0, 0, PB->IntToString((int)PPG->StressSent));

    // Embed send timestamp in payload for delay measurement
    char TimeStr[64];
    snprintf(TimeStr, sizeof(TimeStr), "%.6f", PB->GetTime());
    StressPing->SetPayloadFromCharArray(TimeStr, strlen(TimeStr));

    // Generate SCN
    SCN = NameGenerator::GetInstance().GenerateFromMessage(StressPing);

    // Create SCN command line
    PMB->NewSCNCommandLine("0.1", SCN, StressPing, PCL);

#ifdef DEBUG
    PB->S << Offset << "(Pushing stress ping to GW InputQueue for peer BID=" << PeerBID << ")" << endl;
#endif

    // Push to GW InputQueue — GW routes to PG, PG sends via raw socket
    PPG->PGW->PushToInputQueue(StressPing);
    PPG->StressSent++;

#ifdef DEBUG
    PB->S << Offset << "(Pushed OK. Total Sent=" << PPG->StressSent << ")" << endl;
#endif
  }

#ifdef DEBUG
  PB->S << Offset << "(Done)" << endl
        << endl
        << endl;
#endif

  return Status;
}