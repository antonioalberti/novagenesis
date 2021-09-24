/*
	NovaGenesis

	Name:		PGRunExposition01
	Object:		PGRunExposition01
	File:		PGRunExposition01.cpp
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

#ifndef _PGRUNEXPOSITION01_H
#include "PGRunExposition01.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PGS_H
#include "PGCS.h"
#endif

//#define DEBUG // This debug is important to follow PGCS running

PGRunExposition01::PGRunExposition01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGRunExposition01::~PGRunExposition01 ()
{
}

// Run the actions behind a received command line
// ng -run --exposition 0.1

//TODO: FIXP/Update - This function has been revisited in August 2021. Copy it for your old version of code
int
PGRunExposition01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  CommandLine *PCL;
  vector<string> *HTs = new vector<string>;
  PG *PPG = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  PGCS *PPGCS = 0;
  Message *Exposition = 0;
  vector<Tuple *> HTSs;
  vector<Tuple *> GIRSs;
  vector<Tuple *> PSSs;
  vector<Tuple *> NRNCSs;
  Tuple *PTuple = 0;
  bool HTS_OK = false;
  bool GIRS_OK = false;
  bool PSS_OK = false;
  bool NRNCS_OK = false;
  bool HTS_ERROR = false;
  bool GIRS_ERROR = false;
  bool PSS_ERROR = false;
  bool NRNCS_ERROR = false;
  unsigned int Index = 0;

  PPGCS = (PGCS *)PB->PP;

  PPG = (PG *)PB;

#ifdef DEBUG
  PB->S << endl << endl << Offset << this->GetLegibleName () << endl;
#endif

  if (PB->StopProcessingMessage == false)
	{

#ifdef DEBUG
	  PB->S << Offset << "(This PGCS has " << PPG->PGCSTuples.size () << " peer(s))" << endl;
#endif

	  // **************************************************************
	  // Sending an Exposition message to the PGCS::PG at another hosts
	  // **************************************************************
	  if (PPGCS->Stacks != NULL && PPGCS->Stacks->size () > 0)
		{
		  // Looping over the Peer PGCS identifiers
		  for (unsigned int i = 0; i < PPG->PGCSTuples.size (); i++)
			{
			  // Clear all vectors
			  HTs->clear ();
			  Limiters.clear ();
			  Sources.clear ();
			  Destinations.clear ();
			  HTSs.clear ();
			  GIRSs.clear ();
			  PSSs.clear ();
			  NRNCSs.clear ();

#ifdef DEBUG
			  PB->S << Offset << "(Checking PGCS_HID " << PPG->PGCSTuples.at (i)->Values.at (0) << ")" << endl;
			  PB->S << Offset << "(Checking PGCS_PID " << PPG->PGCSTuples.at (i)->Values.at (2) << ")" << endl;
			  PB->S << Offset << "(Roles vector size = " << PPGCS->Roles->size () << ")" << endl;
			  PB->S << Offset << "(Index = " << Index << ")" << endl;
#endif

			  // The following code was changed to avoid exposition inter domain.
			  // The original idea was to limit expositions to a domain. Then, the comparison:
			  // if (PPGCS->Roles->at(Index) == "Intra_Domain") is very dangerous!!!
			  // The Index is out of range in Roles vector. Please investigate.
			  // Limit expositions inside domain and to pre-configured Peer.
			  // Please, recode this in a error proof way.
			  // This was done in June 10th, 2018.

#ifdef DEBUG
			  PB->S << Offset << "(Exposing to the Intra_Domain peer PGCS with PID = "
					<< PPG->PGCSTuples[i]->Values[2] << ")" << endl;
#endif
			  // **************************************
			  // Destination SCNs: The peer PGCS::HT
			  // **************************************

			  if (PB->PP->DiscoverHomonymsBlocksBIDsFromPID (PPG->PGCSTuples[i]->Values[2], "HT", HTs, PB)
				  == OK)
				{
				  if (HTs->size () == 1)
					{
					  // *****************************************************************************
					  // Get the local bindings to be exposed
					  // *****************************************************************************

					  if (PB->PP
							  ->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("HTS", "DHT", HTSs, PB)
						  == OK)
						{
						  for (unsigned int f = 0; f < HTSs.size (); f++)
							{
							  PTuple = HTSs.at (f);

							  if (PTuple == 0)
								{
								  HTS_ERROR = true;
								}
							  else
								{
								  HTS_OK = true;
								}
							}
						}

					  if (PB->PP
							  ->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("GIRS", "IR", GIRSs, PB)
						  == OK)
						{
						  for (unsigned int g = 0; g < GIRSs.size (); g++)
							{
							  PTuple = GIRSs.at (g);

							  if (PTuple == 0)
								{
								  GIRS_ERROR = true;
								}
							  else
								{
								  GIRS_OK = true;
								}
							}
						}

					  if (PB->PP
							  ->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("PSS", "PS", PSSs, PB)
						  == OK)
						{
						  for (unsigned int h = 0; h < PSSs.size (); h++)
							{
							  PTuple = PSSs.at (h);

							  if (PTuple == 0)
								{
								  PSS_ERROR = true;
								}
							  else
								{
								  PSS_OK = true;
								}
							}
						}

					  if (PB->PP
							  ->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("NRNCS", "NR", NRNCSs, PB)
						  == OK)
						{
						  for (unsigned int h = 0; h < NRNCSs.size (); h++)
							{
							  PTuple = NRNCSs.at (h);

							  if (PTuple == 0)
								{
								  NRNCS_ERROR = true;
								}
							  else
								{
								  NRNCS_OK = true;
								}
							}
						}

					  if ((HTS_OK == true && HTS_ERROR == false) ||
						  (GIRS_OK == true && GIRS_ERROR == false) ||
						  (PSS_OK == true && PSS_ERROR == false) ||
						  (NRNCS_OK == true && NRNCS_ERROR == false))
						{
						  // Creating a new message
						  PB->PP->NewMessage (GetTime (), 0, false, Exposition);

						  // Setting up the OS SCN as the space limiter
						  Limiters.push_back (PB->PP->Intra_Domain);

						  // **************
						  // Source SCNs
						  // **************

						  // Setting up this HID as the 1st source SCN
						  Sources.push_back (PB->PP->GetHostSelfCertifyingName ());

						  // Setting up this OSID as the 2nd source SCN
						  Sources.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

						  // Setting up this PID as the 3rd source SCN
						  Sources.push_back (PB->PP->GetSelfCertifyingName ());

						  // Setting up the PG block SCN (this BID) as the fourth source SCN
						  Sources.push_back (PB->GetSelfCertifyingName ());

						  // Setting up the destination HID as empty
						  Destinations.push_back (PPG->PGCSTuples[i]->Values[0]);

						  // Setting up the destination OSID as empty
						  Destinations.push_back (PPG->PGCSTuples[i]->Values[1]);

						  // Setting up the destination PID as empty
						  Destinations.push_back (PPG->PGCSTuples[i]->Values[2]);

						  // Setting up the destination BID as empty
						  Destinations.push_back (HTs->at (0));

						  // Creating the ng -cl -m command line
						  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Exposition, PCL);

						  for (unsigned int f = 0; f < HTSs.size (); f++)
							{
							  PTuple = HTSs.at (f);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Host", PTuple->Values
								  .at (0), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "OS", PTuple->Values
								  .at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 6, PTuple->Values
								  .at (0), PTuple->Values.at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (1), PTuple->Values.at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (2), PTuple->Values.at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "HTS", PTuple->Values
								  .at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (2), "HTS", Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "DHT", PTuple->Values
								  .at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (3), "DHT", Exposition, PCL);
							}

						  for (unsigned int g = 0; g < GIRSs.size (); g++)
							{
							  PTuple = GIRSs.at (g);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Host", PTuple->Values
								  .at (0), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "OS", PTuple->Values
								  .at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 6, PTuple->Values
								  .at (0), PTuple->Values.at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (1), PTuple->Values.at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (2), PTuple->Values.at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "GIRS", PTuple->Values
								  .at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (2), "GIRS", Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "IR", PTuple->Values
								  .at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (3), "IR", Exposition, PCL);
							}

						  for (unsigned int h = 0; h < PSSs.size (); h++)
							{
							  PTuple = PSSs.at (h);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Host", PTuple->Values
								  .at (0), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "OS", PTuple->Values
								  .at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 6, PTuple->Values
								  .at (0), PTuple->Values.at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (1), PTuple->Values.at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (2), PTuple->Values.at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "PSS", PTuple->Values
								  .at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (2), "PSS", Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "PS", PTuple->Values
								  .at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (3), "PS", Exposition, PCL);
							}

						  for (unsigned int h = 0; h < NRNCSs.size (); h++)
							{
							  PTuple = NRNCSs.at (h);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Host", PTuple->Values
								  .at (0), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "OS", PTuple->Values
								  .at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 6, PTuple->Values
								  .at (0), PTuple->Values.at (1), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (1), PTuple->Values.at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromSCNToSCN ("0.1", 5, PTuple->Values
								  .at (2), PTuple->Values.at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "NRNCS", PTuple->Values
								  .at (2), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (2), "NRNCS", Exposition, PCL);

							  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "NR", PTuple->Values
								  .at (3), Exposition, PCL);

							  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PTuple->Values
								  .at (3), "NR", Exposition, PCL);
							}

						  // ******************************************************
						  // Setting up the SCN command line
						  // ******************************************************

						  // Generate the SCN
						  PB->GenerateSCNFromMessageBinaryPatterns (Exposition, SCN);

						  // Creating the ng -scn --s command line
						  PMB->NewSCNCommandLine ("0.1", SCN, Exposition, PCL);

						  // ******************************************************
						  // Finish
						  // ******************************************************

#ifdef DEBUG
						  PB->S << Offset << "(The scheduled message was:)" << endl;

						  PB->S << endl << endl << *Exposition << endl << endl;
#endif
						  // Push the message to the GW input queue
						  PPG->PGW->PushToInputQueue (Exposition);
						}
					}
				  else
					{
					  PB->S << Offset << "(ERROR: Unable to get peer PGCS::HT BID from local hash table)"
							<< endl;
					}
				}

			  Index = 0;

			}
		}
	}

  HTs->clear ();

  delete HTs;

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

