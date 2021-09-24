/*
	NovaGenesis

	Name:		GWRunInitialization01
	Object:		GWRunInitialization01
	File:		GWRunInitialization01.cpp
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

#ifndef _GWRUNINITIALIZATION01_H
#include "GWRunInitialization01.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

GWRunInitialization01::GWRunInitialization01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

GWRunInitialization01::~GWRunInitialization01 ()
{
}

// Run the actions behind a received command line
// ng -run --initialization 0.1
int
GWRunInitialization01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  Message *StoringInitialBinds = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Block *PHTB = 0;
  CommandLine *PCL = 0;
  GW *PGW = 0;
  int Category;
  string Key;
  vector<string> Values;
  string OperatingSystemLegibleNameSCN;
  string Offset = "                    ";
  Message *RunPeriodic = 0;

  PGW = (GW *)PB;

  PHTB = (Block *)PGW->PHT;

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the CLI block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the HT block SCN as the destination SCN
  Destinations.push_back (PHTB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 0, false, StoringInitialBinds);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, StoringInitialBinds, PCL);

  // Block related

  PMB->NewStoreBindingCommandLineFromBLNToHashBLN ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBLN ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBID ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToHashBLN ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToBlocksIndex ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBlocksIndexToBID ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPIDToBID ("0.1", PB, StoringInitialBinds, PCL);

  // Limiters related

  PMB->NewStoreBindingCommandLineFromLimiterToHashLimiter ("0.1", "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToLimiter ("0.1", "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN ("0.1", "OS", PB->PP
	  ->OSSCN, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromRepresentativeSCNToHashLimiter ("0.1", PB->PP
	  ->OSSCN, "OS", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromLimiterToHashLimiter ("0.1", "Domain", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToLimiter ("0.1", "Domain", StoringInitialBinds, PCL);

  // **********************************************************************************
  // The next two lines store a temporary domain name, while Domain Service is booting
  // **********************************************************************************

  PMB->NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN ("0.1", "Domain", PB->PP
	  ->DSCN, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromRepresentativeSCNToHashLimiter ("0.1", PB->PP
	  ->DSCN, "Domain", StoringInitialBinds, PCL);

  // ******************************************************
  // Binding PID to Input IPC Key
  // ******************************************************

  // Setting up the category
  Category = 13;

  // Setting up the key
  Key = PB->PP->GetSelfCertifyingName ();

  Values.push_back (PB->IntToString (PB->PP->Key));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoringInitialBinds, PCL);

  Values.clear ();

  // ***************************************************************************************
  // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
  // ***************************************************************************************

  if (PB->PP->GetLegibleName () != "PGCS")
	{
	  key_t IPCKey = 11;
	  int shmid = 0;

	  if (PGW->ReturnIPCSHMID (IPCKey, shmid) == OK)
		{
		  // Setting up the category
		  Category = 17;

		  // Setting up the key
		  Key = PB->IntToString (IPCKey);

		  Values.push_back (PB->IntToString (shmid));

		  // Creating the ng -sr --b 0.1 command line
		  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoringInitialBinds, PCL);
		}
	}

  // ******************************************************
  // Finish
  // ******************************************************

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (StoringInitialBinds, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, StoringInitialBinds, PCL);

  // Clear the temporary containers
  Values.clear ();
  Limiters.clear ();
  Sources.clear ();
  Destinations.clear ();

  // ******************************************************
  // Schedule a message to run periodic first time
  // ******************************************************

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the CLI block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the HT block SCN as the destination SCN
  Destinations.push_back (PHTB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 1, false, RunPeriodic);

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
  PGW->PushToInputQueue (StoringInitialBinds);

  // Push the message to the GW input queue
  PGW->PushToInputQueue (RunPeriodic);

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

