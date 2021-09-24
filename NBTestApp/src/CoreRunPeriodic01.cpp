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
  PublishCounter = 0;
  SubscribeCounter = 0;
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
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string Offset1 = "                              ";
  Message *Publish = 0;
  Message *Subscribe = 0;
  string Key;
  vector<string> Value;
  vector<string> Keys;

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // ******************************************************
  // Schedule a message to publish name bindings
  // ******************************************************

  //PB->S << Offset <<  "(Publishing name bindings to test the NRNCS/GIRS/HTS performance.)"<<endl;
  PB->S << Offset << "(Number of messages in memory = " << PB->PP->GetNumberOfMessages () << ".)" << endl;

  // *************************************************************
  // Publishing name bindings
  // *************************************************************
  if (PCore->PSTuples.size () > 0)
	{
	  if (PublishCounter < PCore->NumberOfPublications)
		{
		  PB->S << Offset << "(Number of publications done = " << PublishCounter << ".)" << endl;

		  // Calculate the interval between pub messages
		  double Interval = PCore->DelayBeforeRunPeriodic / (PCore->NumberOfMessagesPerBurst + 1);

		  // ***********************************************************
		  //	Prepare the message header
		  // ***********************************************************
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

		  for (unsigned int q = 0; q < PCore->NumberOfMessagesPerBurst; q++)
			{
			  int i = rand () % PCore->PSTuples.size ();

			  // Setting up the destination 1st source
			  Destinations.push_back (PCore->PSTuples[i]->Values[0]);

			  // Setting up the destination 2nd source
			  Destinations.push_back (PCore->PSTuples[i]->Values[1]);

			  // Setting up the destination 3rd source
			  Destinations.push_back (PCore->PSTuples[i]->Values[2]);

			  // Setting up the destination 4th source
			  Destinations.push_back (PCore->PSTuples[i]->Values[3]);

			  // Allocates a new subscription
			  Publication *PP;

			  PCore->NewPublication (PP);

			  PP->Timestamp = GetTime () + q * Interval;

			  // Creating a new message
			  PB->PP->NewMessage (GetTime () + q * Interval, 1, false, Publish);

			  // Creating the ng -cl -m command line
			  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Publish, PCL);

			  // ***************************************************
			  //	Create the incremental binding
			  // ***************************************************
			  for (unsigned int r = 0; r < PCore->NumberOfPubsPerMessage; r++)
				{
				  // Set the key as the hash(PublishCounter)
				  string Input = PB->IntToString (PublishCounter);

				  // Set the name binding value
				  Value.push_back (Input);

				  PublishCounter++;

				  //PB->S << Offset <<"(Counter = "<<Counter<<".)"<< endl;

				  PMB->GenerateSCNFromCharArrayBinaryPatterns (Input, Key);

				  // ***************************************************
				  //	Create the Publish message
				  // ***************************************************
				  // Binding Key, Values
				  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 8, Key, &Value, Publish, PCL);

				  Value.clear ();
				}

			  Publish->NewCommandLine ("-message", "--type", "0.1", PCL);

			  PCL->NewArgument (1);

			  PCL->SetArgumentElement (0, 0, PB->IntToString (Publish->GetType ()));

			  // ***************************************************
			  // Generate the ng -message --seq [ < 1 string 1 > ]
			  // ***************************************************

			  Publish->NewCommandLine ("-message", "--seq", "0.1", PCL);

			  PCL->NewArgument (1);

			  PCL->SetArgumentElement (0, 0, PB->IntToString (PCore->GetSequenceNumber ()));

			  // ******************************************************
			  // Generate the SCN
			  // ******************************************************

			  // Generate the SCN
			  PB->GenerateSCNFromMessageBinaryPatterns (Publish, SCN);

			  // Creating the ng -scn --s command line
			  PMB->NewSCNCommandLine ("0.1", SCN, Publish, PCL);

			  //PB->S << Offset <<"(The following message is going to be published)"<< endl;

			  //PB->S << "(" << endl << *Publish << ")"<< endl;

			  // Stores the SCN on the Publication object for future reference
			  PP->Key = SCN;

			  // ******************************************************
			  // Finish
			  // ******************************************************

			  // Push the message to the GW input queue
			  PCore->PGW->PushToInputQueue (Publish);

			  // Clean the destinations vector
			  Destinations.clear ();
			}
		}
	  else
		{
		  if (SubscribeCounter == 0)
			{
			  // Change the interval between subscriptions
			  PCore->DelayBeforeRunPeriodic = 20;

			  SubscribeCounter++;
			}
		  else if (SubscribeCounter < (PCore->NumberOfSubscriptions + 1))
			{
			  PB->S << Offset << "(Number of subscriptions done = " << SubscribeCounter << ".)" << endl;

			  // Allocates a new subscription
			  Subscription *PS;

			  PCore->NewSubscription (PS);

			  // Set the timestamp
			  PS->Timestamp = GetTime ();

			  PS->Category = 8;

			  srand (time (NULL));

			  /* generate secret number between 1 and NumberOfPublications: */
			  int Number = rand () % PCore->NumberOfPublications + 1;

			  // Set the key as the hash(Counter)
			  string Input = PB->IntToString (Number);

			  // Calculate the hash(Input)
			  PMB->GenerateSCNFromCharArrayBinaryPatterns (Input, Key);

			  // Copy the key to the Subscription container
			  PS->Key = Key;

			  Keys.push_back (Key);

			  // Set the status to schedule a ng -run --subscribe
			  PS->Status = "Waiting delivery";

			  PS->HasContent = false;

			  // Increment the number of subscribed keys
			  SubscribeCounter++;

			  // Change the interval between subscriptions
			  PCore->DelayBeforeRunPeriodic = 0.5;

			  // April 2021, not scalable at all. Would be much better whether the keys were used to determine to which PSS/NRNCS the content has been submitted.
			  for (unsigned int u = 0; u < PCore->PSTuples.size (); u++)
				{
				  Subscribe = NULL;
				  Limiters.clear ();
				  Sources.clear ();
				  Destinations.clear ();

				  // ***********************************************************
				  //	Prepare the message header
				  // ***********************************************************
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

				  // Setting up the destination 1st source
				  Destinations.push_back (PCore->PSTuples[u]->Values[0]);

				  // Setting up the destination 2nd source
				  Destinations.push_back (PCore->PSTuples[u]->Values[1]);

				  // Setting up the destination 3rd source
				  Destinations.push_back (PCore->PSTuples[u]->Values[2]);

				  // Setting up the destination 4th source
				  Destinations.push_back (PCore->PSTuples[u]->Values[3]);

				  // Creating a new message
				  PB->PP->NewMessage (PS->Timestamp, 1, false, Subscribe);

				  // Creating the ng -cl -m command line
				  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Subscribe, PCL);

				  // ***************************************************
				  //	Create the Publish message
				  // ***************************************************
				  // Binding Key, Values
				  PMB->NewSubscriptionCommandLine ("0.1", 8, &Keys, Subscribe, PCL);

				  // ***************************************************
				  // Generate the ng -message --seq [ < 1 string 1 > ]
				  // ***************************************************

				  Subscribe->NewCommandLine ("-message", "--seq", "0.1", PCL);

				  PCL->NewArgument (1);

				  PCL->SetArgumentElement (0, 0, PB->IntToString (PCore->GetSequenceNumber ()));

				  // ******************************************************
				  // Generate the SCN
				  // ******************************************************

				  // Generate the SCN
				  PB->GenerateSCNFromMessageBinaryPatterns (Subscribe, SCN);

				  // Creating the ng -scn --s command line
				  PMB->NewSCNCommandLine ("0.1", SCN, Subscribe, PCL);

				  //PB->S << Offset <<"(Counter = "<<SubscribeCounter<<", Number = "<<Number<< ", Key = "<<Key<<".)"<<endl;

				  // ******************************************************
				  // Finish
				  // ******************************************************

				  // Push the message to the GW input queue
				  PCore->PGW->PushToInputQueue (Subscribe);
				}
			}
		  else
			{
			  PB->S << endl << endl << Offset << "(-------------------------- Finished --------------------------)"
					<< endl;

			  PB->S << Offset << "(NumberOfPublications = " << PCore->NumberOfPublications << ".)" << endl;
			  PB->S << Offset << "(NumberOfSubscriptions = " << PCore->NumberOfSubscriptions << ".)" << endl;
			  PB->S << Offset << "(PublishCounter = " << PublishCounter << ".)" << endl;
			  PB->S << Offset << "(SubscribeCounter = " << SubscribeCounter << ".)" << endl;

			  exit (0);
			}
		}
	}
  else
	{
	  //PB->S << Offset1 << "(Waiting for NRNCS discovery)"<<endl;
	}

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
  // Clean bugged messages
  // ******************************************************

  if (PB->State == "operational")
	{
	  PB->PP->MarkMalformedMessagesPerNoCLs (2);
	}

  //PB->PP->ShowMessages();

  return Status;
}
