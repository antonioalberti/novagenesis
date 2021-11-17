/*
	NovaGenesis

	Name:		PGHelloIHC02
	Object:		PGHelloIHC02
	File:		PGHelloIHC02.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.2

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

#ifndef _PGHELLOIHC02_H
#include "PGHelloIHC02.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGS_H
#include "PGCS.h"
#endif

//#define DEBUG

PGHelloIHC02::PGHelloIHC02 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGHelloIHC02::~PGHelloIHC02 ()
{
}

// Run the actions behind a received command line
// ng -hello --ihc _Version [ < HID OSID PGCS_PID PG_BID GW_SCN HT_SCN _Stack _Interface _Identifier > ... < HID OSID PGCS_PID PG_BID GW_SCN HT_SCN _Stack _Interface _Identifier > ]
int
PGHelloIHC02::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  PG *PPGB = 0;
  string HID;
  string PID;
  string BID;
  vector<string> ReceivedElements;
  string Offset = "                    ";
  string MyStack;            // The local stack
  string MyInterface;        // The local interface
  string MyPeerIdentifier;    // The local address
  string PeerStack;            // The peer stack
  string PeerInterface;        // The peer interface
  string PeerIdentifier;    // The peer address
  string Peer;
  PGCS *PPGCS = 0;
  bool StoreNewPGCS1 = true; //TODO: FIXP/Oct2021 - Changed these two variable names.
  bool StoreNewPGCS2 = true;
  //vector<string>	Limiters;
  //vector<string>	Sources;
  //vector<string>	Destinations;
  Block *PHTB = 0;
  CommandLine *PCL = 0;
  CommandLine *PSCNCL = 0;
  unsigned int NoA = 0;
  string FirstPeerStack;
  string FirstPeerRole;
  string FirstPeerInterface;
  string FirstPeerIdentifier;
  string FirstPeerSize;

  PPGCS = (PGCS *)PB->PP;
  PPGB = (PG *)PB;
  PHTB = (Block *)PPGB->PHT;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  if (_PCL->GetNumberofArguments (NoA) == OK)
	{

#ifdef DEBUG

	  PB->S << Offset << "NoA = " << NoA << endl;

#endif
	  if (NoA == 1)
		{
		  _PCL->GetArgument (0, ReceivedElements);

		  // *****************************************************************************
		  // Check if there is a compatible local tuple <stack, interface, identifier>
		  // *****************************************************************************
		  for (unsigned int i = 0; i < PPGCS->Stacks->size (); i++)
			{
			  MyStack = PPGCS->Stacks->at (i);                // Stack of the peer as informed when initialized

			  MyInterface = PPGCS->Interfaces
				  ->at (i);        // Local interface to talk with the peer informed when initialized

			  MyPeerIdentifier = PPGCS->Identifiers->at (i); // Identifier of the peer as informed when initialized

			  PeerStack = ReceivedElements.at (6);            // Stack as informed by the peer now

			  PeerInterface = ReceivedElements.at (7);        // Interface as informed by the peer now

			  PeerIdentifier = ReceivedElements.at (8);        // Identifier as informed by the peer now

#ifdef DEBUG
			  PB->S << endl << Offset << "Index = " << i << endl;
			  PB->S << Offset << "(Check if this stack " << MyStack << " is equal to the hello stack " << PeerStack
					<< ")" << endl;
			  PB->S << Offset << "(Check if this identifier " << MyPeerIdentifier
					<< " is equal to the hello identifier " << PeerIdentifier << ")" << endl;
			  PB->S << Offset << "MyInterface = " << MyInterface << endl;
			  PB->S << Offset << "PeerInterface = " << PeerInterface << endl;
#endif
			  if (MyStack == PeerStack)
				{
				  if (PeerIdentifier == MyPeerIdentifier)
					{
#ifdef DEBUG
					  PB->S << Offset << "Going to check this previous informed PGCS" << endl;
#endif
					  // **************************************************************************
					  // Record the peer data in the case another PEER is discovered dynamically
					  // **************************************************************************

					  FirstPeerStack = MyStack;
					  FirstPeerRole = PPGCS->Roles->at (i);
					  FirstPeerInterface = MyInterface;
					  FirstPeerIdentifier = MyPeerIdentifier;
					  FirstPeerSize = PPGCS->Sizes->at (i);

					  // ******************************************************
					  // Check to record the new peer data
					  // ******************************************************

					  for (unsigned int j = 0; j < PPGB->PGCSTuples.size (); j++)
						{
						  if (ReceivedElements.at (2) == PPGB->PGCSTuples[j]->Values[2])
							{
							  StoreNewPGCS1 = false;

							  break;
							}
						}

					  if (StoreNewPGCS1 == true
						  && ReceivedElements.at (0) != PB->PP->GetHostSelfCertifyingName ()) // Just one PGCS per host
						{
						  PB->S << Offset << "(A new peer PGCS was registered via point to point: "
								<< ReceivedElements.at (2) << " at the node identified by " << PeerIdentifier << ")"
								<< endl;

						  Tuple *PeerPGCS = new Tuple;

						  PeerPGCS->Values.push_back (ReceivedElements.at (0));
						  PeerPGCS->Values.push_back (ReceivedElements.at (1));
						  PeerPGCS->Values.push_back (ReceivedElements.at (2));
						  PeerPGCS->Values.push_back (ReceivedElements.at (3));

						  PB->S << Offset << "(HID = " << ReceivedElements.at (0) << ")" << endl;
						  PB->S << Offset << "(OSID = " << ReceivedElements.at (1) << ")" << endl;
						  PB->S << Offset << "(PID = " << ReceivedElements.at (2) << ")" << endl;
						  PB->S << Offset << "(BID = " << ReceivedElements.at (3) << ")" << endl;

						  // Store the peer PGCS tuple for future use
						  PPGB->PGCSTuples.push_back (PeerPGCS);

						  StoreNewPGCS1 = false;

						  // TODO: FIXP/Oct2021- This call has been modified
						  ScheduleStoreBindings ("-p", ReceivedElements, PeerIdentifier, PeerStack);
						}
					}
				else // TODO: FIXP/Oct2021- This case is required to register EPGS on this PGCS
				  {

					// ****************************************************************************************
					// Added in April 22th, 2016; Updated in 26th August 2021
					// ****************************************************************************************

					// ******************************************************
					// Record the new peer data
					// ******************************************************

					for (unsigned int j = 0; j < PPGB->PGCSTuples.size (); j++)
					  {
						if (ReceivedElements.at (2) == PPGB->PGCSTuples[j]->Values[2])
						  {
							StoreNewPGCS2 = false;

							break;
						  }
					  }

#ifdef DEBUG
					PB->S << Offset << "Trying to store the discovered peer = " <<StoreNewPGCS2<< endl;
#endif

					if (StoreNewPGCS2 == true
						&& ReceivedElements.at (0) != PB->PP->GetHostSelfCertifyingName ()) // Just one PGCS per host
					  {
						PB->S << endl << endl << Offset << "(A new peer EPGS was discovered without previous knowledge: "
							  << ReceivedElements.at (2) << " at the node identified by " << PeerIdentifier << ")"
							  << endl << endl << endl ;

						Tuple *PeerPGCS = new Tuple;

						PeerPGCS->Values.push_back (ReceivedElements.at (0));
						PeerPGCS->Values.push_back (ReceivedElements.at (1));
						PeerPGCS->Values.push_back (ReceivedElements.at (2));
						PeerPGCS->Values.push_back (ReceivedElements.at (3));

						PB->S << Offset << "(HID = " << ReceivedElements.at (0) << ")" << endl;
						PB->S << Offset << "(OSID = " << ReceivedElements.at (1) << ")" << endl;
						PB->S << Offset << "(PID = " << ReceivedElements.at (2) << ")" << endl;
						PB->S << Offset << "(BID = " << ReceivedElements.at (3) << ")" << endl;

						// Store the peer PGCS tuple for future use
						PPGB->PGCSTuples.push_back (PeerPGCS);

						ScheduleStoreBindings ("-de", ReceivedElements, PeerIdentifier, PeerStack);
					  }
					else
					  {
						//PB->S << Offset << "(Warning: Peer already registered or it is this PGCS itself)"<<endl;

						StoreNewPGCS2 = true;
					  }
				  }
				}
			}

		  ReceivedElements.clear ();
		}
	  else
		{
		  PB->S << Offset << "(Warning: wrong number of arguments. Waiting for a hello with 1 argument.)" << endl;
		}
	}

  Status = OK;

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

int PGHelloIHC02::ScheduleStoreBindings (string _Case, vector<string> &_ReceivedElements, string _PeerIdentifier, string _PeerStack)
{
  int Status = OK;

  Message *StoreBind01Msg = 0;
  CommandLine *StatusS01 = 0;
  CommandLine *MsgCl01 = 0;
  CommandLine *StoreBind01 = 0;
  PG *PPGB = 0;
  string HID;
  string PID;
  string BID;
  vector<string> StoreBind01MsgLimiters;
  vector<string> StoreBind01MsgSources;
  vector<string> StoreBind01MsgDestinations;
  string Version = "0.1";
  unsigned int Category;
  string Key;
  vector<string> Values;
  string Offset = "                    ";
  string GW_SCN;
  string HT_SCN;
  PGCS *PPGCS = 0;
  Block *PHTB = 0;
  CommandLine *PCL = 0;
  CommandLine *PSCNCL = 0;

  PPGCS = (PGCS *)PB->PP;
  PPGB = (PG *)PB;
  PHTB = (Block *)PPGB->PHT;

  // Generate the MsgCl header for the store message
  StoreBind01MsgLimiters.push_back (PB->PP->Intra_Process);
  StoreBind01MsgSources.push_back (PB->GetSelfCertifyingName ());
  StoreBind01MsgDestinations.push_back (PPGB->PHT->GetSelfCertifyingName ());

  // ******************************************************
  // Schedule a message to store the learned name bindings
  // ******************************************************

  // Creating a new message
  PB->PP->NewMessage (GetTime (), 0, false, StoreBind01Msg);

  // ******************************************************
  // Setting up the first command line
  // ******************************************************

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &StoreBind01MsgLimiters, &StoreBind01MsgSources, &StoreBind01MsgDestinations, StoreBind01Msg, MsgCl01);

  // ******************************************************
  // Binding Received HID to Received OSID
  // ******************************************************

  // Setting up the category
  Category = 6;

  // Setting up the binding key
  Key = _ReceivedElements.at (0);

  // Setting up the values
  Values.push_back (_ReceivedElements.at (1));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

  // Clearing the values container
  Values.clear ();

  // ******************************************************
  // Binding Received OSID to Received HID
  // ******************************************************

  // Setting up the category
  Category = 7;

  // Setting up the binding key
  Key = _ReceivedElements.at (1);

  // Setting up the values
  Values.push_back (_ReceivedElements.at (0));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

  // Clearing the values container
  Values.clear ();

  // ******************************************************
  // Binding Received HID to Hash("Host") and vice-versa
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Host", _ReceivedElements
	  .at (0), StoreBind01Msg, StoreBind01);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 8, _ReceivedElements.at (0), "Host", StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Received OSID to Hash("OS") and vice-versa
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "OS", _ReceivedElements
	  .at (1), StoreBind01Msg, StoreBind01);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, _ReceivedElements.at (1), "OS", StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Received OSID to Received PGCS PID
  // ******************************************************

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _ReceivedElements.at (1);

  // Setting up the values
  Values.push_back (_ReceivedElements.at (2));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

  // Clearing the values container
  Values.clear ();

  // ******************************************************
  // Binding Received PGCS PID to Host HID
  // ******************************************************

  // Setting up the category
  Category = 7;

  // Setting up the binding key
  Key = _ReceivedElements.at (2);

  // Setting up the values
  Values.push_back (_ReceivedElements.at (0));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

  // Clearing the values container
  Values.clear ();

  // ******************************************************
  // Binding Received PGCS PID to Hash("PGCS") and vice-versa
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "PGCS", _ReceivedElements
	  .at (2), StoreBind01Msg, StoreBind01);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, _ReceivedElements.at (2), "PGCS", StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Received PGCS PID to Received PG BID
  // ******************************************************

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _ReceivedElements.at (2);

  // Setting up the values
  Values.push_back (_ReceivedElements.at (3));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

  // Clearing the values container
  Values.clear ();

  // ******************************************************
  // Binding Received PG BID to Hash("PG") and vice-versa
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "PG", _ReceivedElements
	  .at (3), StoreBind01Msg, StoreBind01);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, _ReceivedElements.at (3), "PG", StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Received PGCS PID to Received GW BID
  // ******************************************************

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _ReceivedElements.at (2);

  // Setting up the values
  Values.push_back (_ReceivedElements.at (4));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

  // Clearing the values container
  Values.clear ();

  // ******************************************************
  // Binding Received GW BID to Hash("GW") and vice-versa
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "GW", _ReceivedElements
	  .at (4), StoreBind01Msg, StoreBind01);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, _ReceivedElements.at (4), "GW", StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Received PGCS PID to Received HT BID
  // ******************************************************

  // Setting up the category
  Category = 5;

  // Setting up the binding key
  Key = _ReceivedElements.at (2);

  // Setting up the values
  Values.push_back (_ReceivedElements.at (5));

  // Creating the ng -sr --b 0.1 command line
  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

  // Clearing the values container
  Values.clear ();

  // ******************************************************
  // Binding Received HT BID to Hash("HT") and vice-versa
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "HT", _ReceivedElements
	  .at (5), StoreBind01Msg, StoreBind01);

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, _ReceivedElements.at (5), "HT", StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Peer PGCS HID to informed Peer Identifier.
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromHIDToIdentifier ("0.1", _ReceivedElements
	  .at (0), _PeerIdentifier, StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Peer PGCS identifier to HID.
  // ******************************************************

  PMB->NewStoreBindingCommandLineFromIdentifierToHID ("0.1", _PeerIdentifier, _ReceivedElements
	  .at (0), StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Binding Peer PGCS HID to Hash(Peer PGCS stack).
  // ******************************************************

  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 8, _ReceivedElements
	  .at (0), _PeerStack, StoreBind01Msg, StoreBind01);

  // TODO: FIXP/Oct2021 - Added to deal with the PGCS -de initialization in October 27th, 2021

  if (_Case == "-de" && PPGCS->CSIDs->size () > 0)
	{
	  PB->S << Offset << "(Associating the client socket with CSID " << PPGCS->CSIDs->at (0)
			<< " for the peer PGCS with MAC "
			<< _PeerIdentifier << ")" << endl;

	  // Binding Ethernet address to CSID
	  PMB->NewStoreBindingCommandLineFromIdentifierToSID ("0.1", _PeerIdentifier, PPGCS->CSIDs
		  ->at (0), StoreBind01Msg, StoreBind01);
	}

  // ******************************************************
  // Setting up the SCN command line
  // ******************************************************

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (StoreBind01Msg, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, StoreBind01Msg, StoreBind01);

  // ******************************************************
  // Finish
  // ******************************************************

  // Push the message to the GW input queue
  PPGB->PGW->PushToInputQueue (StoreBind01Msg);

  Status = OK;

  return Status;
}

