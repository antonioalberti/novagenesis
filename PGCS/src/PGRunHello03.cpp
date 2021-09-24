/*
	NovaGenesis

	Name:		Run hello Inter_Domain
	Object:		PGRunHello03
	File:		PGRunHello03.cpp
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

#ifndef _PGRUNHELLO03_H
#include "PGRunHello03.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

////#define DEBUG

PGRunHello03::PGRunHello03 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGRunHello03::~PGRunHello03 ()
{
}

// Run the actions behind a received command line
// ng -run --hello 0.3
int
PGRunHello03::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
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
	  if (PPGCS->Stacks != NULL && PPGCS->Stacks->size () > 0)
		{
		  if (PPG->MyDomainName != "Undefined" && PPG->MyUpperLevelDomainName != "Undefined")
			{

#ifdef DEBUG
			  PB->S << Offset << "(This PGCS has " << PPGCS->Stacks->size () << " network interfaces.)" << endl;
#endif

			  // Looping over the Peer PGCS identifiers
			  for (unsigned int i = 0; i < PPGCS->Stacks->size (); i++)
				{
				  // ****************************************************************
				  // Check for the peer role
				  // ****************************************************************

				  if (PPGCS->Roles->at (i) == "Inter_Domain")
					{
					  // Creating a new message
					  PB->PP->NewMessage (GetTime (), 0, false, PGIHCHello);

					  // Clean the vectors first
					  Limiters.clear ();

					  Sources.clear ();

					  Destinations.clear ();

					  // Set up the domain name hash
					  PB->GenerateSCNFromCharArrayBinaryPatterns (PPG->MyDomainName, PPG->HashOfMyDomainName);

					  PB->GenerateSCNFromCharArrayBinaryPatterns (PPG->MyUpperLevelDomainName, PPG
						  ->HashOfMyUpperLevelDomainName);

					  // Setting up the OS SCN as the space limiter
					  Limiters.push_back (PB->PP->Inter_Domain);

					  // **************
					  // Source SCNs
					  // **************

					  // Setting up this DID as the 1st source SCN
					  Sources.push_back (PPG->HashOfMyDomainName);

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

					  // Setting up the destination DID as empty
					  Destinations.push_back ("FFFFFFFF");

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

					  if (MyStack == "Ethernet" || MyStack == "Wi-Fi")
						{
						  MyIdentifier = PPGCS->MyMACAddress;
						}

					  string CoreSCN = "NULL";

					  Block *PCoreB = NULL;

					  if (PPGCS->GetBlock ("Core", PCoreB) == OK)
						{
						  Core *PCore = (Core *)PCoreB;

						  if (PCore != NULL)
							{
							  CoreSCN = PCore->GetSelfCertifyingName ();
							}
						}

					  PMB->NewIHCHelloCommandLine ("--ihc",
												   "0.3",
												   PPG->PGW->GetSelfCertifyingName (),
												   PPG->PHT->GetSelfCertifyingName (),
												   CoreSCN,
												   MyStack,
												   MyInterface,
												   MyIdentifier,
												   PPG->MyDomainName,
												   PPG->HashOfMyDomainName,
												   PPG->MyUpperLevelDomainName,
												   PPG->HashOfMyUpperLevelDomainName,
												   PGIHCHello, PCL);

					  // ******************************************************
					  // Setting up the SCN command line
					  // ******************************************************

					  // Generate the SCN
					  PB->GenerateSCNFromMessageBinaryPatterns (PGIHCHello, SCN);

					  // Creating the ng -scn --s command line
					  PMB->NewSCNCommandLine ("0.1", SCN, PGIHCHello, PCL);

#ifdef DEBUG

					  PB->S << Offset << "(Sending a ng -hello --ihc to a peer at other domain using the interface "
							<< PPGCS->Interfaces->at (i) << " " << PPGCS->Identifiers->at (i) << ".)" << endl;

					  PB->S << "(" << endl << *PGIHCHello << ")" << endl;
#endif

					  if (PPGCS->Stacks->at (i) == "Ethernet" || PPGCS->Stacks->at (i) == "Wi-Fi")
						{

						  PB->S << Offset << "(Interface = " << PPGCS->Interfaces->at (i) << ".)" << endl;
						  PB->S << Offset << "(Identifier = " << PPGCS->Identifiers->at (i) << ".)" << endl;
						  PB->S << Offset << "(Size = " << PPGCS->Sizes->at (i) << ".)" << endl;

						  // Send the message to the other PGSs
						  PPG->SendToARawSocket (PPGCS->Interfaces->at (i), PPGCS->Identifiers->at (i), PPGCS->Sizes
							  ->at (i), PGIHCHello);
						}

					  PGIHCHello->MarkToDelete ();

					  // Stop processing the other command lines of the message.
					  PB->StopProcessingMessage = true;

					  MyIdentifier = "";

					}

				  Status = OK;
				}
			}
		  else
			{
			  PB->S << Offset
					<< "(Warning: The PGCS is not aware of its own domain name or its upper level domain. Skipping hello.)"
					<< endl;
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

