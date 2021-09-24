/*
	NovaGenesis

	Name:		CoreRunEvaluate01
	Object:		CoreRunEvaluate01
	File:		CoreRunEvaluate01.cpp
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

#ifndef _CORERUNEVALUATE01_H
#include "CoreRunEvaluate01.h"
#endif

#ifndef _NBTESTAPP_H
#include "NBTestApp.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

//#define DEBUG // To follow message processing

CoreRunEvaluate01::CoreRunEvaluate01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreRunEvaluate01::~CoreRunEvaluate01 ()
{
}

// Run the actions behind a received command line
// ng -run --evaluate 0.1
int
CoreRunEvaluate01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  string Offset1 = "                              ";
  Core *PCore = 0;
  vector<string> *Values = new vector<string>;
  vector<Tuple *> PSs;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string Key;
  vector<string> Value;
  vector<Tuple *> PubNotify;
  vector<Tuple *> SubNotify;
  vector<string> Pair;
  string PGCSPID;
  vector<string> *PGCSBIDs = new vector<string>;
  Message *IPCUpdate = NULL;
  CommandLine *PCL = NULL;

  PCore = (Core *)PB;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  if (PCore->State == "initialization")
	{

#ifdef DEBUG
	  PB->S << Offset << "(Check for NRNCS awareness.)" << endl;
#endif

	  // *************************************************************
	  // Check for NRNCS discovery first Step
	  // *************************************************************
	  if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "PSS", PB) == ERROR &&
		  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "NRNCS", PB) == ERROR)
		{
		  vector<string> Cat2Keywords;

		  Cat2Keywords.push_back ("OS");
		  Cat2Keywords.push_back ("PSS");
		  Cat2Keywords.push_back ("PS");
		  Cat2Keywords.push_back ("NRNCS");
		  Cat2Keywords.push_back ("NR");

		  vector<string> Cat9Keywords;

		  Cat9Keywords.push_back ("Host");

		  // Schedule a first step of NRNCS discovery
		  PCore->DiscoveryFirstStep (PB->PP->Intra_OS, &Cat2Keywords, &Cat9Keywords, ScheduledMessages);

#ifdef DEBUG

		  PB->S << Offset1 << "(Not aware of any NRNCS on Categories 2 and 9. Prepare discover first step)" << endl;

#endif

		  // Set the flag to generate the ng -run command line, if required.
		  PCore->GenerateRunX01 = true;

		  // Set the flag to generate the ng -scn --seq for the ng -run message
		  PCore->GenerateRunXSCNSeq01 = true;
		}
	  else
		{

#ifdef DEBUG
		  PB->S << Offset1 << "(Aware of a NRNCS on Categories 2 and 9)" << endl;
#endif
		}

	  // *************************************************************
	  // Check for NRNCS discovery second Step
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

		  PB->S << Offset1 << "(Not aware of any NRNCS on Categories 5 and 6. Prepare discover second step)" << endl;

#endif

		  // Set the flag to generate the ng -run command line, if required.
		  PCore->GenerateRunX01 = true;

		  // Set the flag to generate the ng -scn --seq for the ng -run message
		  PCore->GenerateRunXSCNSeq01 = true;
		}
	  else
		{
		  // Test storage of new NRNCS if there is some candidate
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
						  PB->S << Offset1 << "(The app is already aware of this NRNCS)" << endl;
#endif
						}
					}

				  if (StoreFlag == true)
					{
					  PCore->PSTuples.push_back (PSs[g]);

					  PB->S << Offset1 << "(Discovered a PSS/NRNCS)" << endl;

					  PCore->ScheduledMessagesCreation = true;

					  PCore->State = "operational";

					  // Clear the temporary containers
					  Limiters.clear ();
					  Sources.clear ();
					  Destinations.clear ();

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
						  PB->S << Offset << "(ERROR: Failed to discover the PGCS_PID and its HT_BID at Run Evaluate)"
								<< endl;
						}
					}
				}
			}
		  else
			{

#ifdef DEBUG
			  PB->S << Offset1 << "(There is no candidate for NRNCS)" << endl;
#endif
			}
		}
	}

  // Delete temporary vectors
  delete Values;

#ifdef DEBUG

  PB->S << Offset << "(Deleting the previous marked messages)" << endl;

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
