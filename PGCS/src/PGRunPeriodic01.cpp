/*
	NovaGenesis

	Name:		PGRunPeriodic01
	Object:		PGRunPeriodic01
	File:		PGRunPeriodic01.cpp
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

#ifndef _PGRUNPERIODIC01_H
#include "PGRunPeriodic01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

//#define DEBUG

PGRunPeriodic01::PGRunPeriodic01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGRunPeriodic01::~PGRunPeriodic01 ()
{
}

// Run the actions behind a received command line
// ng -run --periodic _Version
int
PGRunPeriodic01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  PG *PPG = 0;
  CommandLine *PCL = 0;
  Message *RunPeriodic = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;

  PPG = (PG *)PB;

#ifdef DEBUG

  PB->S << endl << endl << Offset << this->GetLegibleName () << endl;
  PB->S << Offset << "This PGCS has the SCN "<< PB->PP->GetSelfCertifyingName() << " and is running at host " << PB->PP->GetHostSelfCertifyingName() << endl;

#endif

  // ******************************************************
  // Put here the function to be performed every period
  // ******************************************************

  // Check for NRNCS awareness
  PSAwareness ();

  // Schedule a hello to the peer PGSs
  HelloScheduling ();

  // Expose learned core SCNs to the peer PGSs
  ExpositionScheduling ();

  // Publish PGCS data to PSS/NRNCS
  PGCSPublishingScheduling ();

  // Check for domain and upper level peer domain names
  //DiscoverDomainNames();

  // Check for domain names awareness
  //DomainNamesAwareness();

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
  PB->PP->NewMessage (GetTime () + PPG->DelayBeforeRunPeriodic, 1, false, RunPeriodic);

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
  PPG->PGW->PushToInputQueue (RunPeriodic);

  // ******************************************************
  // Clean the messages container
  // ******************************************************

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

int PGRunPeriodic01::PSAwareness ()
{
  int Status = ERROR;
  string Offset = "                    ";
  string Offset1 = "                              ";
  vector<Tuple *> ProcessesTuples;
  PG *PPG = 0;
  bool NewPSDetected = true;
  PGCS *PPGCS = 0;

  PPG = (PG *)PB;
  PPGCS = (PGCS *)PB->PP;

#ifdef DEBUG

  PB->S << Offset << "(1. Check for core processes awareness.)" << endl;

#endif

  if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("PSS", "PS", ProcessesTuples, PB) == ERROR)
	{

#ifdef DEBUG
	  PB->S << Offset1 << "(Not aware of any PSS.)" << endl;
#endif
	}
  else
	{
	  if (ProcessesTuples.size () > 0)
		{
		  for (unsigned int i = 0; i < ProcessesTuples.size (); i++)
			{
			  PPG->AwareOfAPS = true;

#ifdef DEBUG
			  PB->S << Offset1 << "(Aware of the PSS " << i << " )" << endl;
			  PB->S << Offset1 << "(HID = " << ProcessesTuples[i]->Values[0] << ")" << endl;
			  PB->S << Offset1 << "(OSID = " << ProcessesTuples[i]->Values[1] << ")" << endl;
			  PB->S << Offset1 << "(PID = " << ProcessesTuples[i]->Values[2] << ")" << endl;
			  PB->S << Offset1 << "(BID = " << ProcessesTuples[i]->Values[3] << ")" << endl;
#endif

			  for (unsigned int j = 0; j < PPG->PSTuples.size (); j++)
				{
				  if (ProcessesTuples[i]->Values[2] == PPG->PSTuples[j]->Values[2])
					{
					  // Change the flag
					  NewPSDetected = false;
					}
				}

			  if (NewPSDetected == true)
				{

#ifdef DEBUG
				  PB->S << Offset1 << "(A new PSS was discovered.)" << endl;
#endif

				  PPG->PSTuples.push_back (ProcessesTuples[i]);

				  if (PPGCS->HasCore == true)
					{
					  Block *PCoreB = NULL;

					  PPGCS->GetBlock ("Core", PCoreB);

					  Core *PCore = (Core *)PCoreB;

					  PCore->PSTuples.push_back (ProcessesTuples[i]);
					}
				}
			}
		}
	}

  if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("NRNCS", "NR", ProcessesTuples, PB)
	  == ERROR)
	{

#ifdef DEBUG
	  PB->S << Offset1 << "(Not aware of any PSS/NRNCS.)" << endl;
#endif
	}
  else
	{
	  if (ProcessesTuples.size () > 0)
		{
		  for (unsigned int i = 0; i < ProcessesTuples.size (); i++)
			{
			  PPG->AwareOfAPS = true;

#ifdef DEBUG
			  PB->S << Offset1 << "(Aware of the PSS/NRNCS " << i << " )" << endl;
			  PB->S << Offset1 << "(HID = " << ProcessesTuples[i]->Values[0] << ")" << endl;
			  PB->S << Offset1 << "(OSID = " << ProcessesTuples[i]->Values[1] << ")" << endl;
			  PB->S << Offset1 << "(PID = " << ProcessesTuples[i]->Values[2] << ")" << endl;
			  PB->S << Offset1 << "(BID = " << ProcessesTuples[i]->Values[3] << ")" << endl;
#endif

			  for (unsigned int j = 0; j < PPG->PSTuples.size (); j++)
				{
				  if (ProcessesTuples[i]->Values[2] == PPG->PSTuples[j]->Values[2])
					{
					  // Change the flag
					  NewPSDetected = false;
					}
				}

			  if (NewPSDetected == true)
				{

#ifdef DEBUG
				  PB->S << Offset1 << "(A new PSS/NRNCS was discovered.)" << endl;
#endif

				  PPG->PSTuples.push_back (ProcessesTuples[i]);

				  if (PPGCS->HasCore == true)
					{
					  Block *PCoreB = NULL;

					  PPGCS->GetBlock ("Core", PCoreB);

					  Core *PCore = (Core *)PCoreB;

					  PCore->PSTuples.push_back (ProcessesTuples[i]);
					}
				}
			}
		}
	}

  return Status;
}

// Schedule a hello to the peer PGSs
int PGRunPeriodic01::HelloScheduling ()
{
  int Status = ERROR;
  string Offset = "                    ";
  string Offset1 = "                              ";
  PG *PPG = 0;
  Message *RunHello01 = 0;
  Message *RunHello02 = 0;
  Message *RunHello03 = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  CommandLine *PCL = 0;
  PGCS *PPGCS = 0;

  PPG = (PG *)PB;
  PPGCS = (PGCS *)PB->PP;

  // ******************************************************
  // Schedule a message to run hello 0.1 for other NG PGCSs
  // ******************************************************

#ifdef DEBUG

  PB->S << Offset << "(2. Scheduling a hello 0.1 message to the peer PGCS(s).)" << endl;

#endif


  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the block SCN as the destination SCN
  Destinations.push_back (PB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 1, false, RunHello01);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunHello01, PCL);

  // Adding a ng -run --periodic command line
  RunHello01->NewCommandLine ("-run", "--hello", "0.1", PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (RunHello01, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, RunHello01, PCL);


  // Push the message to the GW input queue
  PPG->PGW->PushToInputQueue (RunHello01);

  if (PPGCS->HasCore == true)
	{
	  // ******************************************************
	  // Schedule a message to run hello 0.2 for NG EPGSs
	  // ******************************************************

#ifdef DEBUG

	  PB->S << Offset << "(2. Scheduling a hello 0.2 message to the peer PGCS(s).)" << endl;

#endif


	  // Creating a new message
	  PB->PP->NewMessage (GetTime (), 1, false, RunHello02);

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunHello02, PCL);

	  // Adding a ng -run --periodic command line
	  RunHello02->NewCommandLine ("-run", "--hello", "0.2", PCL);

	  // Generate the SCN
	  PB->GenerateSCNFromMessageBinaryPatterns (RunHello02, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, RunHello02, PCL);

	  // Push the message to the GW input queue
	  PPG->PGW->PushToInputQueue (RunHello02);
	}

  if (PPGCS->Role == "Inter_Domain")
	{

	  // ******************************************************
	  // Schedule a message to run hello 0.3 for NG EPGSs
	  // ******************************************************

#ifdef DEBUG

	  PB->S << Offset << "(2. Scheduling a hello 0.3 message to the peer PGCS(s).)" << endl;

#endif

	  // Creating a new message
	  PB->PP->NewMessage (GetTime (), 1, false, RunHello03);

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunHello03, PCL);

	  // Adding a ng -run --periodic command line
	  RunHello03->NewCommandLine ("-run", "--hello", "0.3", PCL);

	  // Generate the SCN
	  PB->GenerateSCNFromMessageBinaryPatterns (RunHello03, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, RunHello03, PCL);

	  // Push the message to the GW input queue
	  PPG->PGW->PushToInputQueue (RunHello03);

	}

  return Status;
}

// Expose learned core SCNs to the peer PGSs
int PGRunPeriodic01::ExpositionScheduling ()
{
  int Status = ERROR;
  string Offset = "                    ";
  string Offset1 = "                              ";
  Message *RunExposition = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  CommandLine *PCL = 0;
  PG *PPG = 0;

  PPG = (PG *)PB;

#ifdef DEBUG

  PB->S << Offset << "(3. Exposing learned core processes name bindings.)" << endl;

#endif

  // ******************************************************
  // Schedule a message to run exposition
  // ******************************************************

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the block SCN as the destination SCN
  Destinations.push_back (PB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 1, false, RunExposition);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunExposition, PCL);

  // Adding a ng -run --periodic command line
  RunExposition->NewCommandLine ("-run", "--exposition", "0.1", PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (RunExposition, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, RunExposition, PCL);

  // Push the message to the GW input queue
  PPG->PGW->PushToInputQueue (RunExposition);

  return Status;
}

// Publish PGCS data to PSS/NRNCS
int PGRunPeriodic01::PGCSPublishingScheduling ()
{
  int Status = ERROR;
  string Offset = "                    ";
  string Offset1 = "                              ";
  PG *PPG = 0;
  Message *RunPublishing = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  CommandLine *PCL = 0;

  PPG = (PG *)PB;

  if (PPG->AlreadyPublishedBasicBindings == false && PPG->AwareOfAPS == true)
	{
	  // ******************************************************
	  // Schedule a message to run publishing names to PSS/NRNCS
	  // ******************************************************

#ifdef DEBUG

	  PB->S << Offset << "(5. Publishing PGCS name bindings to the PSS/NRNCS.)" << endl;

#endif

	  // Setting up the process SCN as the space limiter
	  Limiters.push_back (PB->PP->Intra_Process);

	  // Setting up the block SCN as the source SCN
	  Sources.push_back (PB->GetSelfCertifyingName ());

	  // Setting up the block SCN as the destination SCN
	  Destinations.push_back (PB->GetSelfCertifyingName ());

	  // Creating a new message
	  PB->PP->NewMessage (GetTime (), 1, false, RunPublishing);

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunPublishing, PCL);

	  // Adding a ng -run --periodic command line
	  RunPublishing->NewCommandLine ("-run", "--publishing", "0.1", PCL);

	  // Generate the SCN
	  PB->GenerateSCNFromMessageBinaryPatterns (RunPublishing, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, RunPublishing, PCL);

	  // Push the message to the GW input queue
	  PPG->PGW->PushToInputQueue (RunPublishing);

	  PPG->AlreadyPublishedBasicBindings = true;
	}

  return Status;
}

