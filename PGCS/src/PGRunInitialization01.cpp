/*
	NovaGenesis

	Name:		PGRunInitialization01
	Object:		PGRunInitialization01
	File:		PGRunInitialization01.cpp
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

#ifndef _PGRUNINITIALIZATION01_H
#include "PGRunInitialization01.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

////#define DEBUG

PGRunInitialization01::PGRunInitialization01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGRunInitialization01::~PGRunInitialization01 ()
{
}

// Run the actions behind a received command line
// ng -run --initialization 0.1
int
PGRunInitialization01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  Message *StoringInitialBinds = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Block *PHTB = 0;
  CommandLine *PCL = 0;
  CommandLine *PSCNCL = 0;
  PG *PPG = 0;
  string Offset = "                    ";
  PGCS *PPGCS = 0;
  string PeerPGSHID;
  string Parameter;
  string Value;
  double Temp;
  string MyIPAddress;
  Message *RunPeriodic = 0;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // ******************************************************
  // Load customized parameters if available
  // ******************************************************

  PPG = (PG *)PB;

  PHTB = (Block *)PPG->PHT;

  PPGCS = (PGCS *)PPG->PP;

  PB->S << Offset << "(Loading customized parameters (if available) at " << PB->GetPath () << "PGCS.ini)" << endl;

  File F3;

  F3.OpenInputFile ("PGCS.ini", PB->GetPath (), "DEFAULT");

  F3.seekg (0);

  char Line[512];

  while (F3.getline (Line, sizeof (Line), '\n'))
	{
	  istringstream ins (Line);

	  ins >> Parameter;
	  ins >> Value;

	  //PB->S << Offset <<  "(Parameter = "<<Parameter<<" )" << endl;
	  //PB->S << Offset <<  "(Value = "<<Value<<" )" << endl;

	  if (Parameter == "DelayBeforeRunPeriodic")
		{
		  Temp = PB->StringToDouble (Value);

		  if (Temp > 0)
			{
			  PPG->DelayBeforeRunPeriodic = Temp;

			  PB->S << Offset << "(DelayBeforeRunPeriodic is " << Temp << ")" << endl;
			}
		}

	  if (Parameter == "DelayBetweenMessageEmissions")
		{
		  Temp = PB->StringToDouble (Value);

		  if (Temp > 0)
			{
			  PPG->DelayBetweenMessageEmissions = Temp;

			  PB->S << Offset << "(DelayBetweenMessageEmissions is " << Temp << ")" << endl;
			}

		  break;
		}
	}

  F3.CloseFile ();

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the HT block SCN as the destination SCN
  Destinations.push_back (PHTB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 0, false, StoringInitialBinds);

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

  // Legible names related
  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Host", PB->PP
	  ->GetHostSelfCertifyingName (), StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 8, PB->PP
	  ->GetHostSelfCertifyingName (), "Host", StoringInitialBinds, PCL);

  // Limiters related

  PMB->NewStoreBindingCommandLineFromLimiterToHashLimiter ("0.1", "Domain", StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromHashLimiterToLimiter ("0.1", "Domain", StoringInitialBinds, PCL);

  // **********************************************************************************
  //  The next two lines store a temporary domain name, while Domain Service is booting
  // **********************************************************************************

  PMB->NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN ("0.1", "Domain", PB->PP
	  ->DSCN, StoringInitialBinds, PCL);

  PMB->NewStoreBindingCommandLineFromRepresentativeSCNToHashLimiter ("0.1", PB->PP
	  ->DSCN, "Domain", StoringInitialBinds, PCL);

  if (PPGCS->Stacks != NULL)
	{
	  // Legacy networking-related
	  for (unsigned int i = 0; i < PPGCS->Stacks->size (); i++)
		{
		  // ************************** Adaptation Layer **************************

		  // First, get the host IP address
		  // Second, creates the sockets as required
		  // Third, creates a thread to deal with receptions

		  PB->S << Offset << "(My stack is " << PPGCS->Stacks->at (i) << ")" << endl;

		  // If the Peer is UDP IP
		  if (PPGCS->Stacks->at (i) == "IPv4_UDP")
			{
			  int CSID = 0;

			  // ################################
			  // PUSH
			  // ################################

			  // Get IP address
			  PPG->GetHostIPAddress (PPGCS->Stacks->at (i), PPGCS->Interfaces->at (i), MyIPAddress);

			  // Create a socket to sent datagram to a peer PGCS
			  CSID = PPG->CreateUDPSocket ("PUSH", PPGCS->Identifiers->at (i));

			  PB->S << Offset << "(Created the push socket with CSID " << CSID << " to the address "
					<< PPGCS->Identifiers->at (i) << ")" << endl;

			  // Binding TCP/IP address to SSID
			  PMB->NewStoreBindingCommandLineFromIdentifierToSID ("0.1", PPGCS->Identifiers
				  ->at (i), CSID, StoringInitialBinds, PCL);

			  // TODO: FIXP/Update - Added to deal with the case of PGCS -de initialization
			  PPGCS->CSIDs->push_back (CSID);

			  if (PPGCS->AlreadyCreatedPeerPGCSFrameReceivingUDPThread == false)
				{
				  int SSID = 0;

				  // ################################
				  // PULL
				  // ################################

				  // Create a socket to receive datagram from a peer PGCS
				  SSID = PPG->CreateUDPSocket ("PULL", MyIPAddress);

				  PB->S << Offset << "(Created the pull socket with SSID " << SSID << " with address " << MyIPAddress
						<< ")" << endl;

				  // Binding TCP/IP socket to CSID
				  PMB->NewStoreBindingCommandLineFromIdentifierToSID ("0.1", MyIPAddress, SSID, StoringInitialBinds, PCL);

				  PPGCS->SSIDs->push_back (SSID);

				  PPGCS->Threads[i] = new tthread::thread (&PG::ReceiveFromAUDPSocketThreadWrapper, PPG);

				  PB->S << Offset << "(Created a thread to pull messages to socket with SSID " << SSID << ")" << endl;

				  PPGCS->NoT++;

				  PPGCS->AlreadyCreatedPeerPGCSFrameReceivingUDPThread = true;
				}
			}

		  // **********************************************************************

		  // ************************** Adaptation Layer **************************

		  // The following code is to support Ethernet and Wi-Fi interface.
		  // First, verifies allowed interfaces for what MyMACAddress can be attributed by GetHostRawAddress
		  // Second, creates the sockets as required
		  // Third, creates a thread to deal with receptions

		  // If the Peer is Ethernet or Wi-Fi
		  if (PPGCS->Stacks->at (i) == "Ethernet" || PPGCS->Stacks->at (i) == "Wi-Fi")
			{
			  int CSID = 0;
			  int SSID = 0;
			  string MAC;

			  string _Interface = PPGCS->Interfaces->at (i);

			  PPG->GetHostRawAddress (PPGCS->Interfaces->at (i), MAC);

			  PPGCS->MyMACAddress = MAC;

			  PB->S << Offset << "MyMACAddress = " << MAC << endl;
			  PB->S << Offset << "Interface = " << _Interface << endl;

			  // Create a socket to send frames to a peer PGCS
			  if (PPG->CreateRawSocket (CSID) == OK)
				{
				  PB->S << Offset << "(Created the client socket with CSID " << CSID << " for the peer PGCS with MAC "
						<< PPGCS->Identifiers->at (i) << ")" << endl;

				  // Binding Ethernet address to CSID
				  PMB->NewStoreBindingCommandLineFromIdentifierToSID ("0.1", PPGCS->Identifiers
					  ->at (i), CSID, StoringInitialBinds, PCL);

				  // TODO: FIXP/Update - Added to deal with the case of PGCS -de initialization
				  PPGCS->CSIDs->push_back (CSID);
				}
			  else
				{
				  Status = ERROR;
				}

			  if (PPG->CreateRawSocket (SSID) == OK)
				{
				  // Create a socket to receive frames from a peer PGCS
				  if (PPGCS->AlreadyCreatedPeerPGCSFrameReceivingIEEEThread == false)
					{
					  PB->S << Offset << "(Created the server socket with SSID " << SSID << " at my MAC address "
							<< PPGCS->MyMACAddress << ")" << endl;

					  PPGCS->SSIDs->push_back (SSID);

					  // Binding Ethernet address to SSID
					  PMB->NewStoreBindingCommandLineFromIdentifierToSID ("0.1", PPGCS
						  ->MyMACAddress, SSID, StoringInitialBinds, PCL);

					  PPGCS->Threads[i * PPG
						  ->Number_Of_Threads_At_Socket_Dispatcher] = new tthread::thread (&PG::EthernetWiFiSocketDispatcherThreadWrapper, PPG);

					  PB->S << Offset << "(Created a socket dispatcher thread to read messages from socket with SSID "
							<< SSID << ")"
							<< endl;

					  // Sleep to allow thread creation without overlapping
					  tthread::this_thread::sleep_for (tthread::chrono::milliseconds (100));

					  for (unsigned int j = 0; j < PPG->Number_Of_Threads_At_Socket_Dispatcher; j++)
						{
						  PARAMETERS1 params;

						  params._PPG = PPG;
						  params._Index = j;

						  PPGCS->Threads[i * PPG->Number_Of_Threads_At_Socket_Dispatcher + j
										 + 1] = new tthread::thread (&PG::FinishReceivingThreadWrapper, &params);

						  PB->S << Offset << "(Created thread number " << j << " to serve the dispatcher)"
								<< endl;

						  // Sleep to allow thread creation without overlapping
						  tthread::this_thread::sleep_for (tthread::chrono::milliseconds (100));
						}

					  PPGCS->NoT++;

					  PPGCS->AlreadyCreatedPeerPGCSFrameReceivingIEEEThread = true;
					}
				}
			  else
				{
				  Status = ERROR;
				}
			}

		  // **********************************************************************
		}
	}

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (StoringInitialBinds, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, StoringInitialBinds, PSCNCL);

  // Push the message to the GW input queue
  PPG->PGW->PushToInputQueue (StoringInitialBinds);

  // ******************************************************
  // Finish
  // ******************************************************

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

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  if (Status == ERROR)
	{
	  PB->S << Offset
			<< "(ERROR: Unable to create the sockets. Please verify your operating system, specially for the adequate privileges.)"
			<< endl;

	  exit (1);
	}

  return Status;
}
