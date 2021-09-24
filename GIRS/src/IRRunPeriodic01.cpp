/*
	NovaGenesis

	Name:		IRRunPeriodic01
	Object:		IRRunPeriodic01
	File:		IRRunPeriodic01.cpp
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

#ifndef _IRRUNPERIODIC01_H
#include "IRRunPeriodic01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

#ifndef _GIRS_H
#include "GIRS.h"
#endif

////#define DEBUG
////#define DEBUG1 // To see the messages stored in memory at girs periodically

IRRunPeriodic01::IRRunPeriodic01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
  PGCSPID = "unknown";

  PGCSHT = "unknown";
}

IRRunPeriodic01::~IRRunPeriodic01 ()
{
}

// Run the actions behind a received command line
// ng -run --periodic _Version
int
IRRunPeriodic01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  IR *PIR = 0;
  CommandLine *PCL = 0;
  Message *RunPeriodic = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;

  PIR = (IR *)PB;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

#ifdef DEBUG1

  PB->S << Offset << "(Show the messages in memory)" << endl;

  PB->PP->ShowMessages ();

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

	  DiscoveryFirst ();

	  DiscoverySecond ();

	  Operational ();
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
  PB->PP->NewMessage (GetTime () + PIR->DelayBeforeRunPeriodic, 1, false, RunPeriodic);

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

  // ******************************************************
  // Clean the messages container
  // ******************************************************

#ifdef DEBUG

  PB->S << Offset << "(Deleting the previous marked messages)" << endl;

#endif

  PB->PP->DeleteMarkedMessages ();

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

int IRRunPeriodic01::GetPGCSNames ()
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

int IRRunPeriodic01::Exposition ()
{
  int Status = ERROR;
  string Offset = "                    ";
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *ExposingInitialBinds = NULL;
  IR *PIR = 0;
  Block *PHTB = 0;
  CommandLine *PCL = 0;

  PIR = (IR *)PB;

  PHTB = (Block *)PIR->PHT;

#ifdef DEBUG

  PB->S << Offset << "(1. Expose GIRS names.)" << endl;

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

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "IR", PB
	  ->GetSelfCertifyingName (), ExposingInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PB->GetSelfCertifyingName (), "IR", ExposingInitialBinds, PCL);

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
  PIR->PGW->PushToInputQueue (ExposingInitialBinds);

  Status = OK;

  return Status;
}

int IRRunPeriodic01::DiscoveryFirst ()
{
  int Status = ERROR;
  string Offset = "                    ";
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *Discovery = NULL;
  IR *PIR = 0;
  Block *PHTB = 0;
  CommandLine *PCL = 0;

  PIR = (IR *)PB;

  PHTB = (Block *)PIR->PHT;

#ifdef DEBUG

  PB->S << Offset << "(2. Discovery PSS/HTS names: first step.)" << endl;

#endif


  // ***************************************************
  // Set up the keywords for the discovery state
  // ***************************************************

  string LN1 = "Host";
  string LN2 = "OS";
  string LN3 = "PSS";
  string LN4 = "PS";
  string LN5 = "HTS";
  string LN6 = "DHT";

  string Key1;
  string Key2;
  string Key3;
  string Key4;
  string Key5;
  string Key6;

  PMB->GenerateSCNFromCharArrayBinaryPatterns (LN1, Key1);
  PMB->GenerateSCNFromCharArrayBinaryPatterns (LN2, Key2);
  PMB->GenerateSCNFromCharArrayBinaryPatterns (LN3, Key3);
  PMB->GenerateSCNFromCharArrayBinaryPatterns (LN4, Key4);
  PMB->GenerateSCNFromCharArrayBinaryPatterns (LN5, Key5);
  PMB->GenerateSCNFromCharArrayBinaryPatterns (LN6, Key6);

  // ***************************************************
  // Prepare the first command line
  // ***************************************************

  // Setting up the OSID as the space limiter
  Limiters.push_back (PB->PP->Intra_OS);

  // Setting up the this process as the first source SCN
  Sources.push_back (PB->PP->GetSelfCertifyingName ());

  // Setting up the PS block SCN as the source SCN
  Sources.push_back (PHTB->GetSelfCertifyingName ());

  // Setting up the PGCS PID as the destination SCN
  Destinations.push_back (PGCSPID);

  // Setting up the PGCS::HT BID as the destination SCN
  Destinations.push_back (PGCSHT);

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 0, false, Discovery);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Discovery, PCL);

  // ***************************************************
  // Generate the get bindings to be ask on PGCS
  // ***************************************************
  PMB->NewGetCommandLine ("0.1", 9, LN1, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, LN2, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, LN3, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, LN4, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, LN5, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, LN6, Discovery, PCL);

  PMB->NewGetCommandLine ("0.1", 9, Key1, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, Key2, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, Key3, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, Key4, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, Key5, Discovery, PCL);
  PMB->NewGetCommandLine ("0.1", 2, Key6, Discovery, PCL);

  // ******************************************************
  // Setting up the SCN command line
  // ******************************************************

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (Discovery, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, Discovery, PCL);

  // ******************************************************
  // Finish
  // ******************************************************

#ifdef DEBUG
  PB->S << Offset << "(The following message was scheduled.)" << endl;

  PB->S << *Discovery << endl;
#endif

  // Push the message to the GW input queue
  PIR->PGW->PushToInputQueue (Discovery);

  Status = OK;

  return Status;
}

int IRRunPeriodic01::DiscoverySecond ()
{
  int Status = ERROR;
  string Offset = "                    ";
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *Discovery = NULL;
  IR *PIR = 0;
  Block *PHTB = 0;
  CommandLine *PCL = 0;
  vector<string> *HIDs = new vector<string>;
  vector<string> *ADIs = new vector<string>;

  PIR = (IR *)PB;

  PHTB = (Block *)PIR->PHT;

#ifdef DEBUG

  PB->S << Offset << "(3. Discovery PSS/HTS names: second step.)" << endl;

#endif


  // ***************************************************
  // Set up the keywords for the discovery state
  // ***************************************************

  string LN1 = "Host";
  string LN2 = "OS";
  string LN3 = "PSS";
  string LN4 = "PS";
  string LN5 = "HTS";
  string LN6 = "DHT";

  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (9, LN1, HIDs, PB);
  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, LN2, ADIs, PB);
  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, LN3, ADIs, PB);
  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, LN4, ADIs, PB);
  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, LN5, ADIs, PB);
  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, LN6, ADIs, PB);

#ifdef DEBUG
  PB->S << Offset << "(Size of HIDs = " << HIDs->size () << ")" << endl;
  PB->S << Offset << "(Size of ADIs = " << ADIs->size () << ")" << endl;
#endif

  if (HIDs->size () > 0 && ADIs->size () > 0)
	{
	  // ***************************************************
	  // Prepare the first command line
	  // ***************************************************

	  // Setting up the OSID as the space limiter
	  Limiters.push_back (PB->PP->Intra_OS);

	  // Setting up the this process as the first source SCN
	  Sources.push_back (PB->PP->GetSelfCertifyingName ());

	  // Setting up the PS block SCN as the source SCN
	  Sources.push_back (PHTB->GetSelfCertifyingName ());

	  // Setting up the PGCS PID as the destination SCN
	  Destinations.push_back (PGCSPID);

	  // Setting up the PGCS::HT BID as the destination SCN
	  Destinations.push_back (PGCSHT);

	  // Creating a new message
	  PB->PP->NewMessage (GetTime (), 0, false, Discovery);

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Discovery, PCL);

	  // ***************************************************
	  // Generate the get bindings to be ask on PGCS
	  // ***************************************************

	  for (unsigned int i = 0; i < HIDs->size (); i++)
		{
		  PMB->NewGetCommandLine ("0.1", 6, HIDs->at (i), Discovery, PCL);
		}

	  for (unsigned int i = 0; i < ADIs->size (); i++)
		{
		  PMB->NewGetCommandLine ("0.1", 5, ADIs->at (i), Discovery, PCL);
		}

	  // ******************************************************
	  // Setting up the SCN command line
	  // ******************************************************

	  // Generate the SCN
	  PB->GenerateSCNFromMessageBinaryPatterns (Discovery, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, Discovery, PCL);

	  // ******************************************************
	  // Finish
	  // ******************************************************

#ifdef DEBUG
	  PB->S << Offset << "(The following message was scheduled.)" << endl;

	  PB->S << *Discovery << endl;
#endif

	  // Push the message to the GW input queue
	  PIR->PGW->PushToInputQueue (Discovery);
	}
  else
	{
	  PB->S << Offset << "(ERROR: Does not found result from categories 2 and 9)" << endl;
	}

  delete HIDs;
  delete ADIs;

  return Status;
}

int IRRunPeriodic01::Operational ()
{
  int Status = ERROR;
  string Offset = "                    ";
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  IR *PIR = 0;
  CommandLine *PCL = 0;
  Message *IPCUpdate = NULL;
  vector<Tuple *> LocalPSSTuples;
  vector<Tuple *> LocalHTSTuples;
  bool StoreNewPSS;
  bool StoreNewHTS;
  bool GoToOperational1 = false;
  bool GoToOperational2 = false;

  PIR = (IR *)PB;

#ifdef DEBUG

  PB->S << Offset << "(4. Verify peer(s) and go to operational, if possible)" << endl;

#endif

  if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("PSS", "PS", LocalPSSTuples, PB) == OK)
	{
	  for (unsigned int i = 0; i < LocalPSSTuples.size (); i++)
		{
		  GoToOperational1 = true;

#ifdef DEBUG
		  PB->S << Offset << "Tuple = " << endl;

		  PB->S << Offset << "(HID = " << LocalPSSTuples[i]->Values[0] << ")" << endl;

		  PB->S << Offset << "(OSID = " << LocalPSSTuples[i]->Values[1] << ")" << endl;

		  PB->S << Offset << "(PID = " << LocalPSSTuples[i]->Values[2] << ")" << endl;

		  PB->S << Offset << "(BID = " << LocalPSSTuples[i]->Values[3] << ")" << endl;

#endif
		  StoreNewPSS = true;

		  for (unsigned int z = 0; z < PIR->PSSTuples.size (); z++)
			{
			  if (PIR->PSSTuples[z]->Values[2] == LocalPSSTuples[i]->Values[2])
				{
				  StoreNewPSS = false;
				}
			}

		  if (StoreNewPSS == true)
			{
			  PIR->PSSTuples.push_back (LocalPSSTuples[i]);

			  PB->S << Offset << "(Added a new NRNCS)" << endl;
			}
		}
	}
  else
	{
	  GoToOperational1 = false;

	  PB->S << Offset << "(Warning: Empty NRNCS tuples vector)" << endl;
	}

  if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("HTS", "DHT", LocalHTSTuples, PB) == OK)
	{
	  for (unsigned int i = 0; i < LocalHTSTuples.size (); i++)
		{
		  GoToOperational2 = true;

#ifdef DEBUG
		  PB->S << Offset << "(Tuple = " << endl;

		  PB->S << Offset << "(HID = " << LocalHTSTuples[i]->Values[0] << ")" << endl;

		  PB->S << Offset << "(OSID = " << LocalHTSTuples[i]->Values[1] << ")" << endl;

		  PB->S << Offset << "(PID = " << LocalHTSTuples[i]->Values[2] << ")" << endl;

		  PB->S << Offset << "(BID = " << LocalHTSTuples[i]->Values[3] << ")" << endl;
#endif
		  StoreNewHTS = true;

		  for (unsigned int z = 0; z < PIR->HTSTuples.size (); z++)
			{
			  if (PIR->HTSTuples[z]->Values[2] == LocalHTSTuples[i]->Values[2])
				{
				  StoreNewHTS = false;
				}
			}

		  if (StoreNewHTS == true)
			{
			  PIR->HTSTuples.push_back (LocalHTSTuples[i]);

			  PB->S << Offset << "(Added a new HTS)" << endl;
			}
		}
	}
  else
	{
	  GoToOperational2 = false;

	  PB->S << Offset << "(Warning: Empty HTS tuples vector)" << endl;
	}

  if (GoToOperational1 == true && GoToOperational2 == true && PB->State != "operational")
	{
	  // Reset Statistics
	  PIR->PGW->ResetStatistics ();

	  // Change the state from initialization to exposition
	  PB->State = "operational";

	  PB->S << Offset
			<< "(--------------------------------------------------------------------------OPERATIONAL: Everything ok!)"
			<< endl;

	  Limiters.clear ();
	  Sources.clear ();
	  Destinations.clear ();

	  // ***************************************************************************************
	  // Creating a message to discover the PGCS aiming to discover the shared memory of peers
	  // ***************************************************************************************

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
	  PB->PP->NewMessage (GetTime (), 0, false, IPCUpdate);

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, IPCUpdate, PCL);

	  // ***************************************************
	  // Generate the get to discover peer SHM Key on PGCS
	  // ***************************************************

	  PMB->NewGetCommandLine ("0.1", 13, PIR->PSSTuples[0]->Values[2], IPCUpdate, PCL);

	  PMB->NewGetCommandLine ("0.1", 13, PIR->HTSTuples[0]->Values[2], IPCUpdate, PCL);

	  // Generate the SCN
	  PB->GenerateSCNFromMessageBinaryPatterns (IPCUpdate, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, IPCUpdate, PCL);

	  // ******************************************************
	  // Finish
	  // ******************************************************
#ifdef DEBUG

	  PB->S << Offset << "(The following message was scheduled.)" << endl;

	  PB->S << *IPCUpdate << endl;

#endif

	  // Push the message to the GW input queue
	  PIR->PGW->PushToInputQueue (IPCUpdate);

	  Status = OK;

	}

  return Status;
}
