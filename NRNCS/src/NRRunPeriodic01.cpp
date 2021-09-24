/*
	NovaGenesis

	Name:		NRRunPeriodic01
	Object:		NRRunPeriodic01
	File:		NRRunPeriodic01.cpp
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

#ifndef _NRRUNPERIODIC01_H
#include "NRRunPeriodic01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

#ifndef _NRNCS_H
#include "NRNCS.h"
#endif

//#define DEBUG // This debug is important to follow NRNCS running

NRRunPeriodic01::NRRunPeriodic01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
  PGCSPID = "unknown";

  PGCSHT = "unknown";
}

NRRunPeriodic01::~NRRunPeriodic01 ()
{
}

// Run the actions behind a received command line
// ng -run --periodic _Version
int
NRRunPeriodic01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  NR *PNR = 0;
  CommandLine *PCL = 0;
  Message *RunPeriodic = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;

  PNR = (NR *)PB;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // ******************************************************
  // Put here the function to be performed every period
  // ******************************************************

  if (PGCSPID == "unknown" && PGCSHT == "unknown")
	{
	  GetPGCSNames ();
	}

  if (PGCSPID != "unknown" && PGCSHT != "unknown")
	{
	  Exposition ();
	}

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
  PB->PP->NewMessage (GetTime () + PNR->DelayBeforeRunPeriodic, 1, false, RunPeriodic);

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
  PNR->PGW->PushToInputQueue (RunPeriodic);

  // ******************************************************
  // Clean the messages container
  // ******************************************************

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

int NRRunPeriodic01::GetPGCSNames ()
{
  int Status = ERROR;
  vector<string> *PGCSBIDs = new vector<string>;

  if (PB->PP->DiscoverHomonymsBlocksBIDsFromProcessLegibleName ("PGCS", "HT", PGCSPID, PGCSBIDs, PB) == OK)
	{
	  if (PGCSBIDs->size () > 0)
		{
		  PGCSHT = PGCSBIDs->at (0);
		}
	}

  return Status;
}

int NRRunPeriodic01::Exposition ()
{
  int Status = ERROR;
  string Offset = "                    ";
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *ExposingInitialBinds = NULL;
  NR *PNR = 0;
  Block *PHTB = 0;
  CommandLine *PCL = 0;

  PNR = (NR *)PB;

  PHTB = (Block *)PNR->PHT;

#ifdef DEBUG

  PB->S << Offset << "(1. Expose NRNCS names.)" << endl;

#endif

  // ***************************************************
  // Setup the PGCS::HT table as the destination
  // ***************************************************

  // Setting up the OSID as the space limiter
  Limiters.push_back (PB->PP->Intra_OS);

  // Setting up the this process as the first source SCN
  Sources.push_back (PB->PP->GetSelfCertifyingName ());

  // Setting up the IR block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the PGCS PID as the destination SCN
  Destinations.push_back (PGCSPID);

  // Setting up the PGCS::HT BID as the destination SCN
  Destinations.push_back (PGCSHT);

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 0, false, ExposingInitialBinds);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, ExposingInitialBinds, PCL);

  // ***************************************************
  // Generate the bindings to be store on PGCS
  // ***************************************************

  PMB->NewStoreBindingCommandLineFromOSIDToPID ("0.1", ExposingInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPIDToBID ("0.1", PB, ExposingInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPIDToBID ("0.1", PHTB, ExposingInitialBinds, PCL);

  //PMB->NewStoreBindingCommandLineFromPIDToBID("0.1",PGWB,ExposingInitialBinds,PCL);

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "NR", PB
	  ->GetSelfCertifyingName (), ExposingInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PB->GetSelfCertifyingName (), "NR", ExposingInitialBinds, PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (ExposingInitialBinds, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, ExposingInitialBinds, PCL);

  // ******************************************************
  // Finish
  // ******************************************************

#ifdef DEBUG

  PB->S << Offset << "(The following message was scheduled.)" << endl;

  PB->S << *ExposingInitialBinds << endl;

#endif

  // Push the message to the GW input queue
  PNR->PGW->PushToInputQueue (ExposingInitialBinds);

  Status = OK;

  return Status;
}