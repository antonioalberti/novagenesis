/*
	NovaGenesis

	Name:		GWRunHelloIPC02
	Object:		GWRunHelloIPC02
	File:		GWRunHelloIPC02.cpp
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

#ifndef _GWRUNHELLOIPC02_H
#include "GWRunHelloIPC02.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

GWRunHelloIPC02::GWRunHelloIPC02(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

GWRunHelloIPC02::~GWRunHelloIPC02()
{
}

// Run the actions behind a received command line
// ng -run --helloIPC 0.2
int GWRunHelloIPC02::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = OK;
  string Offset = "          ";
  GW* PGW = 0;
  Message* IPCHello = 0;
  CommandLine* PCL = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string Version = "0.2";

  PGW = (GW*)PB;

  // Do not send hello if this is the PGCS (key 11 is PGCS's well-known key)
  if (PB->PP->Key == 11)
  {
    // PGCS does not need to announce itself via hello IPC
    return Status;
  }

  PB->S << Offset << "(Periodic hello IPC 0.2 emission for " << PB->PP->GetLegibleName() << ")" << endl;

  // ******************************************************
  // Creating a hello IPC message to send to PGCS via SHM key 11
  // ******************************************************

  // Creating a new message
  PB->PP->NewMessage(GetTime(), 0, false, IPCHello);

  // Setting up the OS SCN as the space limiter
  Limiters.push_back(PB->PP->Intra_OS);

  // Setting up this process PID as the first source SCN
  Sources.push_back(PB->PP->GetSelfCertifyingName());

  // Setting up the GW block SCN as the second source SCN
  Sources.push_back(PB->GetSelfCertifyingName());

  // Setting up the destination process PID as empty (broadcast to any PGCS)
  Destinations.push_back("FFFFFFFF");

  // Setting up the destination block BID as empty
  Destinations.push_back("FFFFFFFF");

  // ******************************************************
  // Create the first command line: ng -m --cl 0.2
  // ******************************************************

  PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, IPCHello, PCL);

  // ******************************************************
  // Create the second command line: ng -hello --ipc 0.2
  // ******************************************************

  PMB->NewIPCHelloCommandLine("--ipc", Version, PB->PP->Key, PB->PP->GetLegibleName(), IPCHello, PCL);

  // ******************************************************
  // Setting up the SCN command line
  // ******************************************************

  // Generate the SCN
  string SCN = "FFFFFFFF";
  PB->GenerateSCNFromMessageBinaryPatterns(IPCHello, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine("0.1", SCN, IPCHello, PCL);

  // ******************************************************
  // Push to output queue targeting PGCS SHM key 11
  // ******************************************************

  PB->S << Offset << "(Sending hello IPC to PGCS via SHM key 11)" << endl;

  PGW->PushToOutputQueue("11", IPCHello);

  PB->S << Offset << "(Done)" << endl;

  return Status;
}