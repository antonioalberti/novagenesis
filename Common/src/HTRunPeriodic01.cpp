/*
	NovaGenesis

	Name:		HTRunPeriodic01
	Object:		HTRunPeriodic01
	File:		HTRunPeriodic01.cpp
	Author:		Antonio Marcos Alberti and students
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

#ifndef _HTRUNPERIODIC01_H
#include "HTRunPeriodic01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

HTRunPeriodic01::HTRunPeriodic01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTRunPeriodic01::~HTRunPeriodic01 ()
{
}

// Run the actions behind a received command line
// ng -run --periodic _Version
int
HTRunPeriodic01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  HT *PHT = 0;
  CommandLine *PCL = 0;
  Message *RunPeriodic = 0;
  Message *ListBindings = 0;
  Message *BindReport = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Block *PGWB = 0;
  GW *PGW = 0;
  string GWLN = "GW";

  PB->PP->GetBlock (GWLN, PGWB);
  PGW = (GW *)PGWB;

  PHT = (HT *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // ******************************************************
  // Schedule a message to bind report at the HT
  // ******************************************************

  //if (PB->PP->GetLegibleName() == "HTS")
  //	{
  // Setting up the process SCN as the space limiter
  //		Limiters.push_back(PB->PP->Intra_Process);

  // Setting up the block SCN as the source SCN
  //		Sources.push_back(PB->GetSelfCertifyingName());

  // Setting up the block SCN as the destination SCN
  //		Destinations.push_back(PHT->GetSelfCertifyingName());

  // Creating a new message
  //		PB->PP->NewMessage(GetTime(),1,false,BindReport);

  // Creating the ng -cl -m command line
  //		PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,BindReport,PCL);

  // Adding a ng -run --periodic command line
  //		BindReport->NewCommandLine("-bind","--report","0.1",PCL);

  // Generate the SCN
  //		PB->GenerateSCNFromMessageBinaryPatterns(BindReport,SCN);

  // Creating the ng -scn --s command line
  //		PMB->NewSCNCommandLine("0.1",SCN,BindReport,PCL);

  //		Limiters.clear();
  //		Sources.clear();
  //		Destinations.clear();
  //	}

  // ******************************************************
  // Schedule a message to list bindings at the HT
  // ******************************************************

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the block SCN as the destination SCN
  Destinations.push_back (PHT->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 1, false, ListBindings);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, ListBindings, PCL);

  // Adding a ng -run --periodic command line
  ListBindings->NewCommandLine ("-list", "--b", "0.1", PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (ListBindings, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, ListBindings, PCL);

  Limiters.clear ();
  Sources.clear ();
  Destinations.clear ();

  // ******************************************************
  // Schedule a message to run periodic again
  // ******************************************************

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the block SCN as the destination SCN
  Destinations.push_back (PB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime () + PHT->DelayBeforeRunPeriodic, 1, false, RunPeriodic);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunPeriodic, PCL);

  // Adding a ng -run --periodic command line
  RunPeriodic->NewCommandLine ("-run", "--periodic", "0.1", PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (RunPeriodic, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, RunPeriodic, PCL);

  // ******************************************************
  // Finish
  // ******************************************************

  //if (PB->PP->GetLegibleName() == "HTS")
  //	{
  // Push the message to the GW input queue
  //		PGW->PushToInputQueue(BindReport);
  //	}

  // Push the message to the GW input queue
  PGW->PushToInputQueue (ListBindings);

  // Push the message to the GW input queue
  PGW->PushToInputQueue (RunPeriodic);

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
