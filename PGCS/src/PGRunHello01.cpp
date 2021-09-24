/*
	NovaGenesis

	Name:		PGRunHello01
	Object:		PGRunHello01
	File:		PGRunHello01.cpp
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

#ifndef _PGRUNHELLO01_H
#include "PGRunHello01.h"
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

////#define DEBUG

PGRunHello01::PGRunHello01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGRunHello01::~PGRunHello01 ()
{
}

// Run the actions behind a received command line
// ng -run --hello 0.1
int
PGRunHello01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  PG *PPG = 0;
  PGCS *PPGCS = 0;
  Message *PGIHCHello = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  CommandLine *PCL;
  string MyStack;        // The local stack
  string MyIdentifier;    // The local address
  string MyInterface;    // The local interface

  PPG = (PG *)PB;
  PPGCS = (PGCS *)PB->PP;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  if (PB->StopProcessingMessage == false)
	{
	  // **************************************************************
	  // Sending a Hello message to the PGCS::PG at another hosts
	  // **************************************************************
	  if (PPGCS->Stacks != NULL && PPGCS->Stacks->size () > 0 && PPGCS->Roles->size () > 0)
		{

#ifdef DEBUG
		  PB->S << Offset << "(This PGCS has " << PPGCS->Stacks->size () << " network interface(s).)" << endl;
#endif

		  // Looping over the Peer PGCS identifiers
		  for (unsigned int i = 0; i < PPGCS->Stacks->size (); i++)
			{
			  // ****************************************************************
			  // Check for the peer role
			  // ****************************************************************

			  if (PPGCS->Roles->at (i) == "Intra_Domain")
				{
				  // Creating a new message
				  PB->PP->NewMessage (GetTime (), 0, false, PGIHCHello);

				  // Clean the vectors first
				  Limiters.clear ();

				  Sources.clear ();

				  Destinations.clear ();

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

				  // **************
				  // Destination SCNs
				  // **************

				  // Setting up the destination HID as empty
				  Destinations.push_back ("FFFFFFFF");

				  // Setting up the destination OSID as empty
				  Destinations.push_back ("FFFFFFFF");

				  // Setting up the destination PID as empty
				  Destinations.push_back ("FFFFFFFF");

				  // Setting up the destination BID as empty
				  Destinations.push_back ("FFFFFFFF");

				  // ******************************************************
				  // Create the first command line
				  // ******************************************************

				  // Creating the ng -cl -m command line
				  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, PGIHCHello, PCL);

				  // ******************************************************
				  // Create the second command line
				  // ******************************************************

				  MyStack = PPGCS->Stacks->at (i);

				  MyInterface = PPGCS->Interfaces->at (i);

				  if (MyStack == "IPv4_UDP")
					{
					  PPG->GetHostIPAddress (MyStack, MyInterface, MyIdentifier);

					  //PMB->NewIHCHelloCommandLine("--ihc",
					  //							"0.1",
					  //							PPG->PGW->GetSelfCertifyingName(),
					  //							PPG->PHT->GetSelfCertifyingName(),
					  //							MyStack,
					  //							MyInterface,
					  //							(MyIdentifier+":"+PPGCS->Port),
					  //							PGIHCHello,PCL);

					  PGIHCHello->NewCommandLine ("-hello", "--ihc", "0.1", PCL);

					  PCL->NewArgument (9);

					  PCL->SetArgumentElement (0, 0, PB->PP->GetHostSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 1, PB->PP->GetOperatingSystemSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 2, PB->PP->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 3, PB->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 4, PPG->PGW->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 5, PPG->PHT->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 6, MyStack);

					  PCL->SetArgumentElement (0, 7, MyInterface);

					  PCL->SetArgumentElement (0, 8, (MyIdentifier + ":" + PPGCS->Port));
					}

				  if (MyStack == "Ethernet" || MyStack == "Wi-Fi")
					{
#ifdef DEBUG
					  PB->S << Offset << "(My MAC address is " << PPGCS->MyMACAddress << ")"
							<< endl;
					  PB->S << Offset << "(My peer MAC identifier is " << PPGCS->Identifiers->at (i)
							<< ")" << endl;
#endif
					  PGIHCHello->NewCommandLine ("-hello", "--ihc", "0.1", PCL);

					  PCL->NewArgument (9);

					  PCL->SetArgumentElement (0, 0, PB->PP->GetHostSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 1, PB->PP->GetOperatingSystemSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 2, PB->PP->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 3, PB->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 4, PPG->PGW->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 5, PPG->PHT->GetSelfCertifyingName ());

					  PCL->SetArgumentElement (0, 6, MyStack);

					  PCL->SetArgumentElement (0, 7, MyInterface);

					  PCL->SetArgumentElement (0, 8, PPGCS->MyMACAddress);

					  //AddLearnedPeersPhysical(PCL);
					}

				  // ******************************************************
				  // Setting up the SCN command line
				  // ******************************************************

				  // Generate the SCN
				  PB->GenerateSCNFromMessageBinaryPatterns (PGIHCHello, SCN);

				  // Creating the ng -scn --s command line
				  PMB->NewSCNCommandLine ("0.1", SCN, PGIHCHello, PCL);

#ifdef DEBUG
				  PB->S << Offset << "(Sending a ng -hello --ihc to the peer at the interface "
						<< PPGCS->Interfaces->at (i) << " " << PPGCS->Identifiers->at (i) << ".)" << endl;

				  PB->S << "(" << endl << *PGIHCHello << ")" << endl;
#endif

				  if (PPGCS->Stacks->at (i) == "IPv4_UDP")
					{
					  try
						{
						  // Put here the code to send the message to the other PGSs
						  PPG->SendToAUDPSocket (PPGCS->Identifiers->at (i), PPGCS->Sizes->at (i), PGIHCHello);
						}
					  catch (std::exception &e)
						{
						  std::cerr << "exception caught: " << e.what () << "\n";
						}
					}

				  if (PPGCS->Stacks->at (i) == "Ethernet" || PPGCS->Stacks->at (i) == "Wi-Fi")
					{
					  try
						{
						  // Send the message to the other PGSs
						  PPG->SendToARawSocket (PPGCS->Interfaces->at (i), PPGCS->Identifiers->at (i), PPGCS->Sizes
							  ->at (i), PGIHCHello);
						}
					  catch (std::exception &e)
						{
						  std::cerr << "exception caught: " << e.what () << "\n";
						}
					}

				  PGIHCHello->MarkToDelete ();

				  // Stop processing the other command lines of the message.
				  PB->StopProcessingMessage = true;

				  MyIdentifier = "";
				}

			  Status = OK;
			}
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: The stop message processing flag is true)" << endl;

	  Status = ERROR;
	}

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

void PGRunHello01::AddLearnedPeersPhysical (CommandLine *_PCL)
{
  vector<string> *PGCSs = new vector<string>;
  vector<string> *BIDs = new vector<string>;
  vector<string> *What = new vector<string>;
  vector<string> *HIDs = new vector<string>;
  vector<string> *OSIDs = new vector<string>;
  vector<string> *Identifiers = new vector<string>;
  PGCS *PPGCS = 0;
  string HashPGCS;
  string HashGW;
  string HashHT;
  string HashPG;

  PPGCS = (PGCS *)PB->PP;

  PB->GenerateSCNFromCharArrayBinaryPatterns ("PGCS", HashPGCS);
  PB->GenerateSCNFromCharArrayBinaryPatterns ("GW", HashGW);
  PB->GenerateSCNFromCharArrayBinaryPatterns ("HT", HashHT);
  PB->GenerateSCNFromCharArrayBinaryPatterns ("PG", HashPG);

#ifdef DEBUG
  PB->S << "(Reading data from already learned peers)" << endl;
#endif

  if (PB->PP->GetHTBindingValues (2, HashPGCS, PGCSs) == OK)
	{
	  for (unsigned int i = 0; i < PGCSs->size (); i++)
		{
		  string HID = " ";
		  string OSID = " ";
		  string PGCS_PID = PGCSs->at (i);
		  string PG_BID = " ";
		  string GW_BID = " ";
		  string HT_BID = " ";
		  string Identifier = " ";

		  if (PGCS_PID != PB->PP->GetSelfCertifyingName ())
			{
#ifdef DEBUG
			  PB->S << "(Checking the peer " << PGCSs->at (i) << ")" << endl;
#endif
			  if (PB->PP->GetHTBindingValues (5, PGCSs->at (i), BIDs) == OK)
				{
				  for (unsigned int j = 0; j < BIDs->size (); j++)
					{
					  if (PB->PP->GetHTBindingValues (3, BIDs->at (j), What) == OK)
						{
						  if (What->size () == 1)
							{
							  if (What->at (0) == HashGW)
								{
								  GW_BID = BIDs->at (j);

#ifdef DEBUG
								  PB->S << "(Discovered the peer GW BID = " << GW_BID << ")" << endl;
#endif
								}

							  if (What->at (0) == HashHT)
								{
								  HT_BID = BIDs->at (j);

#ifdef DEBUG
								  PB->S << "(Discovered the peer HT BID = " << HT_BID << ")" << endl;
#endif
								}

							  if (What->at (0) == HashPG)
								{
								  PG_BID = BIDs->at (j);

#ifdef DEBUG
								  PB->S << "(Discovered the peer PG BID = " << PG_BID << ")" << endl;
#endif
								}
							}
						  else
							{
							  PB->S << "(Warning: Unable to properly select a block)" << endl;
							}

						}
					  else
						{
						  PB->S << "(Warning: Unable to find PGCS BIDs legible names on Category 3)" << endl;
						}

					  What->clear ();
					}
				}
			  else
				{
				  PB->S << "(Warning: Unable to find PGCS BIDs on Category 5)" << endl;
				}

			  if (PB->PP->GetHTBindingValues (7, PGCSs->at (i), HIDs) == OK)
				{
				  if (HIDs->size () == 1)
					{
					  HID = HIDs->at (0);

#ifdef DEBUG
					  PB->S << "(Discovered the peer HID = " << HID << ")" << endl;
#endif

					  if (PB->PP->GetHTBindingValues (15, HID, Identifiers) == OK)
						{
						  if (Identifiers->size () == 1)
							{
							  Identifier = Identifiers->at (0);

#ifdef DEBUG
							  PB->S << "(Discovered the peer identifier = " << Identifier << ")" << endl;
#endif
							}
						}
					  else
						{
						  PB->S << "(Warning: Unable to find HID on Category 15)" << endl;
						}

					  if (PB->PP->GetHTBindingValues (6, HID, OSIDs) == OK)
						{
						  if (OSIDs->size () == 1)
							{
							  OSID = OSIDs->at (0);

#ifdef DEBUG
							  PB->S << "(Discovered the peer OSID = " << OSID << ")" << endl;
#endif
							}
						}
					  else
						{
						  PB->S << "(Warning: Unable to find OSIDs on Category 6)" << endl;
						}
					}
				  else
					{
					  PB->S << "(Warning: Unable to discover the HID at run hello)" << endl;
					}
				}
			  else
				{
				  //PB->S << "(Warning: Unable to find HID on Category 7)"<< endl;
				}

			  for (unsigned int k = 0; k < PPGCS->Identifiers->size (); k++)
				{
				  if (PPGCS->Identifiers->at (k) == Identifier && GW_BID != " " && HT_BID != " " && PG_BID != " ")
					{
#ifdef DEBUG
					  PB->S << "(Everything OK! Generating the hello argument for the peer " << PGCSs->at (i) << ")"
							<< endl;
#endif

					  _PCL->NewArgument (9);

					  unsigned int NoA = 0;

					  _PCL->GetNumberofArguments (NoA);

					  _PCL->SetArgumentElement (NoA - 1, 0, HID);

					  _PCL->SetArgumentElement (NoA - 1, 1, OSID);

					  _PCL->SetArgumentElement (NoA - 1, 2, PGCS_PID);

					  _PCL->SetArgumentElement (NoA - 1, 3, PG_BID);

					  _PCL->SetArgumentElement (NoA - 1, 4, GW_BID);

					  _PCL->SetArgumentElement (NoA - 1, 5, HT_BID);

					  _PCL->SetArgumentElement (NoA - 1, 6, PPGCS->Stacks->at (k));

					  _PCL->SetArgumentElement (NoA - 1, 7, PPGCS->Interfaces->at (k));

					  _PCL->SetArgumentElement (NoA - 1, 8, Identifier);

#ifdef DEBUG
					  PB->S << *_PCL << endl;
#endif
					}
				}

			  BIDs->clear ();

			  HIDs->clear ();

			  Identifiers->clear ();

			  What->clear ();

			  OSIDs->clear ();
			}
		}
	}
  else
	{
	  PB->S << "(Warning: Unable to find PGCS_PIDs on Category 2)" << endl;
	}

  delete PGCSs;
  delete BIDs;
  delete What;
  delete HIDs;
  delete OSIDs;
  delete Identifiers;
}
