/*
	NovaGenesis

	Name:		CoreRunDiscover01
	Object:		CoreRunDiscover01
	File:		CoreRunDiscover01.cpp
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

#ifndef _CORERUNDISCOVER01_H
#include "CoreRunDiscover01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

//#define DEBUG // To follow message processing

CoreRunDiscover01::CoreRunDiscover01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreRunDiscover01::~CoreRunDiscover01 ()
{
}

// Run the actions behind a received command line
// ng -run --discover 0.1 [ < 1 string Limiter > < 2 string Category1 Key1 >  < 2 string Category2 Key2 > ... < 2 string CategoryN KeyN > ]
int
CoreRunDiscover01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Limiter;
  vector<string> Pair;
  string PGCSPID;
  vector<string> *PGCSBIDs = new vector<string>;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *Discovery = 0;
  CommandLine *PCL = 0;
  Core *PCore = 0;
  vector<string> *Keys = new vector<string>;
  bool Verdict = false;

  PCore = (Core *)PB;

#ifdef DEBUG

  PB->S << Offset <<  this->GetLegibleName() << endl;

#endif

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA >= 2)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, Limiter);

		  if (Limiter.size () > 0)
			{
#ifdef DEBUG
			  PB->S << Offset <<"(The limiter of the Discovery message is "<<Limiter.at(0)<<")"<< endl;
#endif

			  if (Limiter.at (0) == PB->PP->Intra_OS)
				{
				  // ***************************************************
				  // The discovery is restricted to the OS
				  // ***************************************************

				  PB->PP->DiscoverHomonymsBlocksBIDsFromProcessLegibleName ("PGCS", "HT", PGCSPID, PGCSBIDs, PB);

				  if (PGCSBIDs->size () > 0)
					{
#ifdef DEBUG
					  PB->S << Offset <<"(Sending a discovery message to the PGCS process on the same operating system)"<< endl;
#endif

					  // ***************************************************
					  // Prepare the first command line
					  // ***************************************************

					  // Setting up the OSID as the space limiter
					  Limiters.push_back (Limiter.at (0));

					  // Setting up the this process as the first source SCN
					  Sources.push_back (PB->PP->GetSelfCertifyingName ());

					  // Setting up the PS block SCN as the source SCN
					  Sources.push_back (PB->GetSelfCertifyingName ());

					  // Setting up the PGCS PID as the destination SCN
					  Destinations.push_back (PGCSPID);

					  // Setting up the PGCS::HT BID as the destination SCN
					  Destinations.push_back (PGCSBIDs->at (0));

					  // Creating a new message
					  PB->PP->NewMessage (GetTime (), 1, false, Discovery);

					  // Creating the ng -cl -m command line
					  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Discovery, PCL);

					  // ***************************************************
					  // Generate the get bindings to be ask on PGCS
					  // ***************************************************

#ifdef DEBUG
					  PB->S << Offset << "(NA = " << NA << ")"<< endl;
#endif

					  for (unsigned int j = 1; j < NA; j++)
						{
						  // Get received command line arguments
						  _PCL->GetArgument (j, Pair);

						  //PB->S << Offset << "(j = " << j << ")"<< endl;

						  // Pairs=_PCL->Arguments.at(1);
						  if (Pair.size () > 0)
							{
#ifdef DEBUG
							  PB->S << Offset << "(Category = "<<Pair.at(0)<<")"<< endl;
							  PB->S << Offset << "(Key = "<<Pair.at(1)<<")"<< endl;
							  PB->S << Offset << "(Checking if this binding already exists on the discovery message)"<< endl;
#endif

							  Discovery->DoAllTheseValuesExistInSomeCommandLine (&Pair, Verdict);

							  if (Verdict == false)
								{
								  // Generate the ng -g --b
								  PMB->NewGetCommandLine ("0.1", PB->StringToInt (Pair.at (0)), Pair
									  .at (1), Discovery, PCL);
#ifdef DEBUG
								  PB->S << Offset <<"(The binding does not exist)"<< endl;
#endif
								}
							  else
								{
#ifdef DEBUG
								  PB->S << Offset <<"(The binding already exists. Skipping it)"<< endl;
#endif
								}
							}

						  Pair.clear ();
						}

					  // ***************************************************
					  // Generate the ng -message --type [ < 1 string 1 > ]
					  // ***************************************************

					  Discovery->NewCommandLine ("-message", "--type", "0.1", PCL);

					  PCL->NewArgument (1);

					  PCL->SetArgumentElement (0, 0, PB->IntToString (Discovery->GetType ()));

					  // ***************************************************
					  // Generate the ng -message --seq [ < 1 string 1 > ]
					  // ***************************************************

					  Discovery->NewCommandLine ("-message", "--seq", "0.1", PCL);

					  PCL->NewArgument (1);

					  PCL->SetArgumentElement (0, 0, PB->IntToString (PCore->GetSequenceNumber ()));

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

					  // Push the message to the GW input queue
					  PCore->PGW->PushToInputQueue (Discovery);

					  if (ScheduledMessages.size () > 0)
						{
						  Message *Temp = ScheduledMessages.at (0);

						  Temp->MarkToDelete ();

						  // Make the scheduled messages vector empty
						  ScheduledMessages.clear ();
						}

					  Status = OK;
					}
				  else
					{
					  PB->S << Offset << "(ERROR: Failed to discover the PGS_PID and its HT_BID)" << endl;
					}
				}

			  Pair.clear ();

			  Discovery = 0;

			  if (Limiter.at (0) == PB->PP->GetDomainSelfCertifyingName ())
				{
#ifdef DEBUG
				  PB->S << Offset <<"(Sending a discovery message to the domain's PSS/NRNCS)"<< endl;
#endif

				  for (unsigned int u = 0; u < PCore->PSTuples.size (); u++)
					{

					  Discovery = 0;

					  Limiters.clear ();
					  Sources.clear ();
					  Destinations.clear ();
					  Pair.clear ();

					  // ***************************************************
					  // The discovery is restricted to the domain
					  // ***************************************************

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
					  PB->PP->NewMessage (GetTime (), 1, false, Discovery);

					  // Creating the ng -cl -m command line
					  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Discovery, PCL);

					  // ***************************************************
					  // Generate the get bindings to be ask on PGCS
					  // ***************************************************

					  //PB->S << Offset << "(NA = " << NA << ")"<< endl;

					  for (unsigned int j = 1; j < NA; j++)
						{
						  // Get received command line arguments
						  _PCL->GetArgument (j, Pair);

						  // Pairs=_PCL->Arguments.at(1);
						  if (Pair.size () > 0)
							{
							  //PB->S << Offset << "(Category = "<<Pair.at(0)<<")"<< endl;
							  //PB->S << Offset << "(Key = "<<Pair.at(1)<<")"<< endl;

							  //PB->S << Offset <<"(Checking if this binding already exists on the discovery message)"<< endl;

							  Discovery->DoAllTheseValuesExistInSomeCommandLine (&Pair, Verdict);

							  if (Verdict == false)
								{
								  // Set the key
								  Keys->push_back (Pair.at (1));

								  // Generate the ng -s --b
								  PMB->NewSubscriptionCommandLine ("0.1", PB
									  ->StringToInt (Pair.at (0)), Keys, Discovery, PCL);

								  //PB->S << Offset <<"(The binding does not exist)"<< endl;

								  Keys->clear ();
								}
							  else
								{
								  //PB->S << Offset <<"(The binding already exists. Skipping it)"<< endl;
								}
							}

						  Pair.clear ();
						}

					  // ***************************************************
					  // Generate the ng -message --type [ < 1 string 1 > ]
					  // ***************************************************

					  Discovery->NewCommandLine ("-message", "--type", "0.1", PCL);

					  PCL->NewArgument (1);

					  PCL->SetArgumentElement (0, 0, PB->IntToString (Discovery->GetType ()));

					  // ***************************************************
					  // Generate the ng -message --seq [ < 1 string 1 > ]
					  // ***************************************************

					  Discovery->NewCommandLine ("-message", "--seq", "0.1", PCL);

					  PCL->NewArgument (1);

					  PCL->SetArgumentElement (0, 0, PB->IntToString (PCore->GetSequenceNumber ()));

					  // Generate the SCN
					  PB->GenerateSCNFromMessageBinaryPatterns (Discovery, SCN);

					  // Creating the ng -scn --s command line
					  PMB->NewSCNCommandLine ("0.1", SCN, Discovery, PCL);

					  // ******************************************************
					  // Finish
					  // ******************************************************

					  // Push the message to the GW input queue
					  PCore->PGW->PushToInputQueue (Discovery);

					  if (ScheduledMessages.size () > 0)
						{
						  Message *Temp = ScheduledMessages.at (0);

						  Temp->MarkToDelete ();

						  // Make the scheduled messages vector empty
						  ScheduledMessages.clear ();
						}

					  Status = OK;
					}

				  if (PCore->PSTuples.size () == 0)
					{
					  PB->S << Offset
							<< "(Warning: Unable to send the discovery message. The domain PSS/NRNCS is still unknown)"
							<< endl;
					}
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;
			}
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
	}

  delete PGCSBIDs;
  delete Keys;

#ifdef DEBUG

  PB->S << Offset <<  "(Done)" << endl << endl << endl;

#endif

  return Status;
}

