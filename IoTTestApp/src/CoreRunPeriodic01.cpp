/*
	NovaGenesis

	Name:		CoreRunPeriodic01
	Object:		CoreRunPeriodic01
	File:		CoreRunPeriodic01.cpp
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

#ifndef _CORERUNPERIODIC01_H
#include "CoreRunPeriodic01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

CoreRunPeriodic01::CoreRunPeriodic01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreRunPeriodic01::~CoreRunPeriodic01 ()
{
}

// Run the actions behind a received command line
// ng -run --periodic _Version
int
CoreRunPeriodic01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  Core *PCore = 0;
  CommandLine *PCL = 0;
  Message *RunPeriodic = 0;
  Message *RunEvaluate = 0;
  Message *RunPublishSSData = 0;

  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // *************************************************************
  // Schedule discovery message
  // *************************************************************
  vector<string> Cat2Keywords;

  // -----------------------------------------------------------------------------
  // Customize here the keywords for discovering the peers
  // -----------------------------------------------------------------------------

  Cat2Keywords.push_back ("Gateway");
  Cat2Keywords.push_back ("Controller");
  Cat2Keywords.push_back ("OS");
  Cat2Keywords.push_back ("Core");
  Cat2Keywords.push_back ("IoT");
  Cat2Keywords.push_back ("Proxy");
  Cat2Keywords.push_back ("Wi-Fi");

  vector<string> Cat9Keywords;

  Cat9Keywords.push_back ("Host");

  //PB->S << Offset <<  "(Schedule both steps discover message)"<<endl;

  // Schedule both steps on the same message
  PCore->DiscoveryFirstStep (PB->PP->GetDomainSelfCertifyingName (), &Cat2Keywords, &Cat9Keywords, ScheduledMessages);

  //PB->S << Offset <<  "(Schedule second step discover message)"<<endl;
  PCore->DiscoverySecondStep (PB->PP->GetDomainSelfCertifyingName (), &Cat2Keywords, &Cat9Keywords, ScheduledMessages);

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
  PB->PP->NewMessage (GetTime () + PCore->DelayBeforeRunPeriodic, 1, false, RunPeriodic);

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

  // Push the message to the GW input queue
  PCore->PGW->PushToInputQueue (RunPeriodic);

  // ******************************************************
  // Finish
  // ******************************************************


  // ******************************************************
  // Schedule a message to run evaluate again
  // ******************************************************

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 1, false, RunEvaluate);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunEvaluate, PCL);

  // Adding a ng -run --periodic command line
  RunEvaluate->NewCommandLine ("-run", "--evaluate", "0.1", PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (RunEvaluate, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, RunEvaluate, PCL);

  // ******************************************************
  // Finish
  // ******************************************************

  // Push the message to the GW input queue
  PCore->PGW->PushToInputQueue (RunEvaluate);

  if (PB->State == "operational")
	{
	  PB->PP->MarkMalformedMessagesPerNoCLs (2);
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
