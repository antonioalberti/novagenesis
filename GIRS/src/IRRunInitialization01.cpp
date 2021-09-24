/*
	NovaGenesis

	Name:		IRRunInitialization01
	Object:		IRRunInitialization01
	File:		IRRunInitialization01.cpp
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

#ifndef _IRRUNINITIALIZATION01_H
#include "IRRunInitialization01.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

IRRunInitialization01::IRRunInitialization01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRRunInitialization01::~IRRunInitialization01 ()
{
}

// Run the actions behind a received command line
// ng -run --initialization 0.1
int
IRRunInitialization01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  Message *StoringInitialBinds = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Block *PHTB = 0;
  CommandLine *PCL = 0;
  IR *PIR = 0;
  string Offset = "                    ";
  string Parameter;
  string Value;
  double Temp;
  Message *RunPeriodic = 0;

  PIR = (IR *)PB;

  PHTB = (Block *)PIR->PHT;

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the IR block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the HT block SCN as the destination SCN
  Destinations.push_back (PHTB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime () + PIR->DelayBeforeRunInitiatilization, 0, false, StoringInitialBinds);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBLNToHashBLN ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBLN ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashBLNToBID ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToHashBLN ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBIDToBlocksIndex ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromBlocksIndexToBID ("0.1", PB, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPLNToHashPLN ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashPLNToPLN ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashPLNToPID ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPIDToHashPLN ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromPIDToBID ("0.1", PB, StoringInitialBinds, PCL);

  //PMB->NewStoreBindingCommandLineFromOSLNToHashOSLN("0.1",StoringInitialBinds,PCL);

  PMB->NewStoreBindingCommandLineFromHashOSLNToOSLN ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromOSIDToPID ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHLNToHashHLN ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashHLNToHLN ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromOSIDToHID ("0.1", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHIDToOSID ("0.1", StoringInitialBinds, PCL);

  //PMB->NewStoreBindingCommandLineFromHIDToPID("0.1",StoringInitialBinds,PCL);

  //PMB->NewStoreBindingCommandLineFromPIDToHID("0.1",StoringInitialBinds,PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (StoringInitialBinds, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, StoringInitialBinds, PCL);

  // ******************************************************
  // Finish
  // ******************************************************

  // Push the message to the GW input queue
  PIR->PGW->PushToInputQueue (StoringInitialBinds);

  // Clear the temporary containers
  Limiters.clear ();
  Sources.clear ();
  Destinations.clear ();

  // ******************************************************
  // Load customized parameters if available
  // ******************************************************

  PB->S << Offset << "(Loading customized parameters (if available) at " << PB->GetPath () << "GIRS.ini)" << endl;

  File F3;

  F3.OpenInputFile ("GIRS.ini", PB->GetPath (), "DEFAULT");

  F3.seekg (0);

  char Line[512];

  while (F3.getline (Line, sizeof (Line), '\n'))
	{
	  istringstream ins (Line);

	  ins >> Parameter;
	  ins >> Value;

	  Temp = PB->StringToDouble (Value);

	  if (Temp > 0)
		{
		  if (Parameter == "DelayBeforeRunInitiatilization")
			{
			  PIR->DelayBeforeRunInitiatilization = Temp;

			  PB->S << Offset << "DelayBeforeRunInitiatilization is " << Temp << endl;
			}

		  if (Parameter == "DelayBeforeDHTPSSDiscovery")
			{
			  PIR->DelayBeforeDHTPSSDiscovery = Temp;

			  PB->S << Offset << "DelayBeforeDHTPSSDiscovery is " << Temp << endl;
			}

		  if (Parameter == "DelayBeforePIDPublishing")
			{
			  PIR->DelayBeforePIDPublishing = Temp;

			  PB->S << Offset << "DelayBeforePIDPublishing is " << Temp << endl;
			}

		  if (Parameter == "DelayBeforeRunExposition")
			{
			  PIR->DelayBeforeRunExposition = Temp;

			  PB->S << Offset << "DelayBeforeRunExposition is " << Temp << endl;
			}

		  if (Parameter == "DelayBeforeRunPeriodic")
			{
			  PIR->DelayBeforeRunPeriodic = Temp;

			  PB->S << Offset << "DelayBeforeRunPeriodic is " << Temp << endl;

			  break;
			}
		}
	}

  F3.CloseFile ();

  // Clear the temporary containers
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
  Destinations.push_back (PB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime () + 10, 1, false, RunPeriodic);

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
  PIR->PGW->PushToInputQueue (RunPeriodic);

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

