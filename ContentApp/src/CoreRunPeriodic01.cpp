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

//#define DEBUG // To follow message processing

CoreRunPeriodic01::CoreRunPeriodic01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreRunPeriodic01::~CoreRunPeriodic01 ()
{
}

// TODO: FIXP/Update - This function has been revisited in September 2021 to reduce the mount of events in ContentApp

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
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  vector<Tuple *> PSs;
  string PGCSPID;
  vector<string> *PGCSBIDs = new vector<string>;
  Message *IPCUpdate = NULL;
  Message *SubscriptionM = NULL;
  vector<string> SubscribingKeys;

  PCore = (Core *)PB;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // *************************************************************
  // Schedule discovery message
  // *************************************************************
  vector<string> Cat2Keywords;

  Cat2Keywords.push_back ("OS");
  Cat2Keywords.push_back ("ContentApp");
  Cat2Keywords.push_back ("Core");
  Cat2Keywords.push_back ("Repository");
  Cat2Keywords.push_back ("Source");
  Cat2Keywords.push_back ("Content");

  vector<string> Cat9Keywords;

  Cat9Keywords.push_back ("Host");

#ifdef DEBUG

  PB->S << Offset << "(Schedule both steps discover message)" << endl;

#endif

  // Schedule both steps on the same message
  PCore->DiscoveryFirstStep (PB->PP->Intra_Domain, &Cat2Keywords, &Cat9Keywords, ScheduledMessages);

  //PB->S << Offset <<  "(Schedule second step discover message)"<<endl;
  PCore->DiscoverySecondStep (PB->PP->Intra_Domain, &Cat2Keywords, &Cat9Keywords, ScheduledMessages);

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


#ifdef DEBUG
  PB->S << Offset << "(1. Check for PSS/NRNCS awareness.)" << endl;
#endif

  // *************************************************************
  // Check for PSS/NRNCS discovery first Step
  // *************************************************************
  //TODO: FIXP/Update - Changed this function just to show whether PSS/NRNCS is already known
  if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "PSS", PB) == OK ||
	  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "NRNCS", PB) == OK)
	{
#ifdef DEBUG
	  PB->S << Offset << "(Aware of a PSS/NRNCS on Categories 2 and 9)" << endl;
#endif
	}

  //TODO: FIXP/Update - Decided to make this procedure independently of previous knowledge on PSS/NRNCS.

  Cat2Keywords.clear();

  Cat2Keywords.push_back ("OS");
  Cat2Keywords.push_back ("PSS");
  Cat2Keywords.push_back ("PS");
  Cat2Keywords.push_back ("NRNCS");
  Cat2Keywords.push_back ("NR");

  Cat9Keywords.clear();

  Cat9Keywords.push_back ("Host");

  // Schedule a first step of NRNCS discovery
  PCore->DiscoveryFirstStep (PB->PP->Intra_OS, &Cat2Keywords, &Cat9Keywords, ScheduledMessages);

  // *************************************************************
  // Check for PSS/NRNCS discovery second Step
  // *************************************************************
  if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("PSS", "PS", PSs, PB) == ERROR &&
	  PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("NRNCS", "NR", PSs, PB) == ERROR)
	{
	  vector<string> Cat2Keywords;

	  Cat2Keywords.push_back ("OS");
	  Cat2Keywords.push_back ("PSS");
	  Cat2Keywords.push_back ("PS");
	  Cat2Keywords.push_back ("NRNCS");
	  Cat2Keywords.push_back ("NR");

	  vector<string> Cat9Keywords;

	  Cat9Keywords.push_back ("Host");

	  // Schedule a second step of NRNCS discovery
	  PCore->DiscoverySecondStep (PB->PP->Intra_OS, &Cat2Keywords, &Cat9Keywords, ScheduledMessages);

#ifdef DEBUG
	  PB->S << Offset << "(Not aware of any PSS/NRNCS on Categories 5 and 6. Prepare discover second step)" << endl;
#endif

	}
  else
	{
	  // Testing storage of new PSS/NRNCS if there is some candidate
	  if (PSs.size () > 0)
		{
		  // Loop over the discovered candidates
		  for (unsigned int g = 0; g < PSs.size (); g++)
			{
			  // Auxiliary flag
			  bool StoreFlag = true;

			  // Loop over the already stored tuples
			  for (unsigned int h = 0; h < PCore->PSTuples.size (); h++)
				{
				  if (PSs[g]->Values[2] == PCore->PSTuples[h]->Values[2])
					{
					  StoreFlag = false;

#ifdef DEBUG
					  PB->S << Offset << "(The app is already aware of the PSS/NRNCS with PID = " << PSs[g]->Values[2] << ")" << endl;
#endif
					}
				}

			  if (StoreFlag == true)
				{
				  PCore->PSTuples.push_back (PSs[g]);

				  PB->S << Offset << "(Discovered a PSS/NRNCS!)" << endl;
				}
			}
		}
	  else
		{
#ifdef DEBUG
		  PB->S << Offset << "(There is no candidate for PSS/NRNCS)" << endl;
#endif
		}

	  // Run other procedures if it is aware of at least one PSS/NRNCS
	  if (PCore->PSTuples.size () > 0)
		{
		  if (PCore->RunExpose == true)
			{
			  //PB->S << Offset1 << "(Run expose once in entire life)"<<endl;

			  PCore->Exposition (PB->PP->Intra_Domain, ScheduledMessages);

			  PCore->RunExpose = false;

			  PB->State = "operational";

			  // *************************************************************************
			  // Discover NRNCS IPC key, if in the local OS
			  // *************************************************************************

			  PB->PP->DiscoverHomonymsBlocksBIDsFromProcessLegibleName ("PGCS", "HT", PGCSPID, PGCSBIDs, PB);

			  if (PGCSBIDs->size () > 0)
				{
				  // ***************************************************
				  // Prepare the first command line
				  // ***************************************************

				  // Setting up the OSID as the space limiter
				  Limiters.push_back (PB->PP->Intra_OS);

				  // Setting up the this process as the first source SCN
				  Sources.push_back (PB->PP->GetSelfCertifyingName ());

				  // Setting up the PS block SCN as the source SCN
				  Sources.push_back (PB->GetSelfCertifyingName ());

				  // Setting up the PGCS PID as the destination SCN
				  Destinations.push_back (PGCSPID);

				  // Setting up the PGCS::HT BID as the destination SCN
				  Destinations.push_back (PGCSBIDs->at (0));

				  // Creating a new message
				  PB->PP->NewMessage (GetTime (), 1, false, IPCUpdate);

				  // Creating the ng -cl -m command line
				  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, IPCUpdate, PCL);

				  // ***************************************************
				  // Generate the get to discover peer SHM Key on PGCS
				  // ***************************************************

				  PMB->NewGetCommandLine ("0.1", 13, PCore->PSTuples[0]->Values[2], IPCUpdate, PCL);

				  // Generate the SCN
				  PB->GenerateSCNFromMessageBinaryPatterns (IPCUpdate, SCN);

				  // Creating the ng -scn --s command line
				  PMB->NewSCNCommandLine ("0.1", SCN, IPCUpdate, PCL);

				  // ******************************************************
				  // Finish
				  // ******************************************************

				  // Push the message to the GW input queue
				  PCore->PGW->PushToInputQueue (IPCUpdate);
				}
			  else
				{
				  PB->S << Offset << "(ERROR: Failed to discover the PGCS_PID and its HT_BID)" << endl;
				}
			}
		}
	}

  // *************************************************************
  // Check pending subscriptions
  // *************************************************************
  // TODO: FIXP/Update - Reschedule subscriptions that are delaying more than 10 second

  PB->S << Offset << "(Looping over existent Subscriptions at this Content App)" <<endl;

  for (unsigned int i = 0; i < PCore->Subscriptions.size (); i++)
	{
	  Subscription *PS = PCore->Subscriptions[i];

	  PB->S << Offset << "(Subscription " << i << " has Status " << PS->Status << ", Key = " << PS->Key
			<< ", HasContent = " << PS->HasContent << ", Time from subscription = " << GetTime () - PS->Timestamp
			<< ")"<< endl;

	  if ((GetTime () - PS->Timestamp) > TIMEOUT && PS->Status == "Waiting delivery")
		{
		  SubscribingKeys.push_back(PS->Key);
		}
	}

  if (PCore->PSTuples.size () > 0 && SubscribingKeys.size() > 0)
	{
	  // April 2021, not scalable at all. Would be much better whether the keys were used to determine to which PSS/NRNCS the content has been submitted.
	  for (unsigned int u = 0; u < PCore->PSTuples.size (); u++)
		{
		  SubscriptionM = NULL;
		  Limiters.clear ();
		  Sources.clear ();
		  Destinations.clear ();

		  // Setting up the OSID as the space limiter
		  Limiters.push_back (PB->PP->Intra_Domain);

		  // Setting up the this OS as the 1st source SCN
		  Sources.push_back (PB->PP->GetHostSelfCertifyingName ());

		  // Setting up the this OS as the 2nd source SCN
		  Sources.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

		  // Setting up the this process as the 3rd source SCN
		  Sources.push_back (PB->PP->GetSelfCertifyingName ());

		  // Setting up the PS block SCN as the 4th source SCN
		  Sources.push_back (PB->GetSelfCertifyingName ());

		  // Setting up the destination
		  Destinations.push_back (PCore->PSTuples[u]->Values[0]);

		  // Setting up the destination 2nd source
		  Destinations.push_back (PCore->PSTuples[u]->Values[1]);

		  // Setting up the destination 3rd source
		  Destinations.push_back (PCore->PSTuples[u]->Values[2]);

		  // Setting up the destination 4th source
		  Destinations.push_back (PCore->PSTuples[u]->Values[3]);

		  // Creating a new message
		  PB->PP->NewMessage (GetTime (), 1, false, SubscriptionM);

		  // Creating the ng -cl -m command line
		  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, SubscriptionM, PCL);

		  // ***************************************************
		  // Generate the ng -s -bind
		  // ***************************************************

		  // Subscribe the key on the Category 18
		  PMB->NewSubscriptionCommandLine ("0.1", PB
			  ->StringToInt ("18"), &SubscribingKeys, SubscriptionM, PCL);

		  // Subscribe the key on the Category 2
		  //PMB->NewSubscriptionCommandLine ("0.1", 2, &SubscribingKeys, SubscriptionM, PCL);

		  // Subscribe the key on the Category 9
		  //PMB->NewSubscriptionCommandLine ("0.1", 9, &SubscribingKeys, SubscriptionM, PCL);

		  // ***************************************************
		  // Generate the ng -message --type [ < 1 string 1 > ]
		  // ***************************************************

		  //SubscriptionM->NewCommandLine("-message","--type","0.1",PCL);

		  //PCL->NewArgument(1);

		  //PCL->SetArgumentElement(0,0,PB->IntToString(SubscriptionM->GetType()));

		  // ***************************************************
		  // Generate the ng -message --seq [ < 1 string 1 > ]
		  // ***************************************************

		  //SubscriptionM->NewCommandLine("-message","--seq","0.1",PCL);

		  //PCL->NewArgument(1);

		  //PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

		  // ******************************************************
		  // Generate the SCN
		  // ******************************************************

		  PB->GenerateSCNFromMessageBinaryPatterns (SubscriptionM, SCN);

		  // Creating the ng -scn --s command line
		  PMB->NewSCNCommandLine ("0.1", SCN, SubscriptionM, PCL);

		  PB->S << Offset << "(The following message contains a subscription of delayed deliveries to the peer)" << endl;

		  PB->S << "(" << endl << *SubscriptionM << ")" << endl;

		  // ******************************************************
		  // Finish
		  // ******************************************************

		  // Push the message to the GW input queue
		  PCore->PGW->PushToInputQueue (SubscriptionM);
		}
	}

  if (PB->State == "operational")
	{
	  PB->PP->MarkMalformedMessagesPerNoCLs (2);
	}

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
