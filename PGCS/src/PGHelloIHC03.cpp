/*
	NovaGenesis

	Name:		PGHelloIHC03
	Object:		PGHelloIHC03
	File:		PGHelloIHC03.cpp
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

#ifndef _PGHELLOIHC03_H
#include "PGHelloIHC03.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGS_H
#include "PGCS.h"
#endif

////#define DEBUG

PGHelloIHC03::PGHelloIHC03 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGHelloIHC03::~PGHelloIHC03 ()
{
}

// Run the actions behind a received command line
// ng -hello --ihc 0.3 GW_SCN HT_SCN _Stack _Interface _Identifier
int
PGHelloIHC03::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
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
  vector<string> PeerTuple;
  vector<string> PeerNames;
  vector<string> DomainNames;
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
  string MyStack;            // The local stack
  string MyInterface;        // The local interface
  string MyPeerIdentifier;    // The local informed peer address
  string PeerStack;            // The peer stack
  string PeerInterface;        // The peer interface
  string PeerIdentifier;    // The peer address
  string Peer;
  PGCS *PPGCS = 0;
  bool StoreNewPGS = true;
  Block *PHTB = 0;

  PPGCS = (PGCS *)PB->PP;
  PPGB = (PG *)PB;
  PHTB = (Block *)PPGB->PHT;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // Generate the MsgCl header for the store message
  StoreBind01MsgLimiters.push_back (PB->PP->Intra_Process);
  StoreBind01MsgSources.push_back (PB->GetSelfCertifyingName ());
  StoreBind01MsgDestinations.push_back (PPGB->PHT->GetSelfCertifyingName ());

  // Get the first command line of the received message
  _ReceivedMessage->GetCommandLine (0, MsgCl01);

  if (MsgCl01 != 0)
	{
	  MsgCl01->GetArgument (1, PeerTuple);

	  _PCL->GetArgument (0, PeerNames);

	  //	PPG->PGW->GetSelfCertifyingName(),
	  // 	PPG->PHT->GetSelfCertifyingName(),
	  //	CoreSCN,
	  //	MyStack,
	  //	MyInterface,
	  //	MyIdentifier,

	  _PCL->GetArgument (1, DomainNames);

	  //	PPG->MyDomainName,
	  //	PPG->HashOfMyDomainName,
	  //	PPG->MyUpperLevelDomainName,
	  //	PPG->HashOfMyUpperLevelDomainName,

	  // *****************************************************************************
	  // Check if there is a compatible local tuple <stack, interface, identifier>
	  // *****************************************************************************
	  for (unsigned int i = 0; i < PPGCS->Stacks->size (); i++)
		{
		  MyStack = PPGCS->Stacks->at (i);

		  MyInterface = PPGCS->Interfaces->at (i);

		  MyPeerIdentifier = PPGCS->Identifiers->at (i);

		  PeerStack = PeerNames.at (3);

		  PeerInterface = PeerNames.at (4);

		  PeerIdentifier = PeerNames.at (5);

#ifdef DEBUG

		  PB->S << endl << Offset << "Index = " << i << endl;
		  PB->S << endl << Offset << "MyStack = " << MyStack << endl;
		  PB->S << Offset << "MyInterface = " << MyInterface << endl;
		  PB->S << Offset << "MyInterfaceIdentifier = " << MyPeerIdentifier << endl;
		  PB->S << Offset << "PeerStack = " << PeerStack << endl;
		  PB->S << Offset << "PeerInterface = " << PeerInterface << endl;
		  PB->S << Offset << "PeerIdentifier = " << PeerIdentifier << endl;

#endif
		  if (MyStack == PeerStack && PeerIdentifier == MyPeerIdentifier)
			{
			  // ******************************************************
			  // Record the new peer data
			  // ******************************************************

			  for (unsigned int j = 0; j < PPGB->PGCSTuples.size (); j++)
				{
				  if (PeerTuple.at (2) == PPGB->PGCSTuples[j]->Values[2])
					{
					  StoreNewPGS = false;

					  break;
					}
				}

			  if (StoreNewPGS == true
				  && PeerTuple.at (0) != PB->PP->GetHostSelfCertifyingName ()) // Just one PGCS per host
				{
				  PB->S << Offset << "(A new peer PGCS was registered via point to point: " << PeerTuple.at (2)
						<< " at the node identified by " << PeerIdentifier << ")" << endl;

				  Tuple *PeerPGS = new Tuple;

				  PeerPGS->Values.push_back (PeerTuple.at (0));
				  PeerPGS->Values.push_back (PeerTuple.at (1));
				  PeerPGS->Values.push_back (PeerTuple.at (2));
				  PeerPGS->Values.push_back (PeerTuple.at (3));
				  PeerPGS->Values.push_back (PeerTuple.at (4));

				  PB->S << Offset << "(DID = " << PeerTuple.at (0) << ")" << endl;
				  PB->S << Offset << "(HID = " << PeerTuple.at (1) << ")" << endl;
				  PB->S << Offset << "(OSID = " << PeerTuple.at (2) << ")" << endl;
				  PB->S << Offset << "(PID = " << PeerTuple.at (3) << ")" << endl;
				  PB->S << Offset << "(BID = " << PeerTuple.at (4) << ")" << endl;

				  // Store the peer PGCS tuple for future use
				  PPGB->PGCSTuples.push_back (PeerPGS);

				  StoreNewPGS = false;
				}

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
			  Key = PeerTuple.at (0);

			  // Setting up the values
			  Values.push_back (PeerTuple.at (1));

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
			  Key = PeerTuple.at (1);

			  // Setting up the values
			  Values.push_back (PeerTuple.at (0));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  // Binding Received DID to Hash("Domain") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Domain", PeerTuple
				  .at (0), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 8, PeerTuple
				  .at (0), "Domain", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Received HID to Hash("Host") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 9, "Host", PeerTuple
				  .at (1), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 8, PeerTuple
				  .at (1), "Host", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Received OSID to Hash("OS") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "OS", PeerTuple
				  .at (2), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PeerTuple
				  .at (2), "OS", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Received OSID to Received PGCS PID
			  // ******************************************************

			  // Setting up the category
			  Category = 5;

			  // Setting up the binding key
			  Key = PeerTuple.at (2);

			  // Setting up the values
			  Values.push_back (PeerTuple.at (3));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  // Binding Received PGCS BID to Hash("PGCS") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "PGCS", PeerTuple
				  .at (3), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PeerTuple
				  .at (3), "PGCS", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Received PGCS PID to Received PG BID
			  // ******************************************************

			  // Setting up the category
			  Category = 5;

			  // Setting up the binding key
			  Key = PeerTuple.at (3);

			  // Setting up the values
			  Values.push_back (PeerTuple.at (4));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ****************************************************************
			  // Binding Received "DomainName" to Received Hash(DomainName)
			  // ****************************************************************

			  // Setting up the category
			  Category = 0;

			  // Setting up the binding key
			  Key = DomainNames.at (0);

			  // Setting up the values
			  Values.push_back (DomainNames.at (1));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ****************************************************************
			  // Binding Received Received Hash(DomainName) to "DomainName"
			  // ****************************************************************

			  // Setting up the category
			  Category = 1;

			  // Setting up the binding key
			  Key = DomainNames.at (1);

			  // Setting up the values
			  Values.push_back (DomainNames.at (0));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ****************************************************************
			  // Binding "MyPeerDomainNames" to "DomainName"
			  // ****************************************************************

			  // Setting up the category
			  Category = 17;

			  // Setting up the binding key
			  Key = "MyPeerDomainNames";

			  // Setting up the values
			  Values.push_back (DomainNames.at (0));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ****************************************************************
			  // Binding "MyPeerUpperLevelDomainNames" to "UpperLevelDomainName"
			  // ****************************************************************

			  // Setting up the category
			  Category = 17;

			  // Setting up the binding key
			  Key = "MyPeerUpperLevelDomainNames";

			  // Setting up the values
			  Values.push_back (DomainNames.at (2));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  // Binding Received PG BID to Hash("PG") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "PG", PeerTuple
				  .at (4), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PeerTuple
				  .at (4), "PG", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Received PGCS PID to Received GW BID
			  // ******************************************************

			  // Setting up the category
			  Category = 5;

			  // Setting up the binding key
			  Key = PeerTuple.at (3);

			  // Setting up the values
			  Values.push_back (PeerTuple.at (1));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  // Binding Received GW BID to Hash("GW") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "GW", PeerTuple
				  .at (1), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PeerTuple
				  .at (1), "GW", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Received PGCS PID to Received HT BID
			  // ******************************************************

			  // Setting up the category
			  Category = 5;

			  // Setting up the binding key
			  Key = PeerTuple.at (3);

			  // Setting up the values
			  Values.push_back (PeerNames.at (2));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  // Binding Received HT BID to Hash("HT") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "HT", PeerNames
				  .at (2), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PeerNames
				  .at (2), "HT", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Received PGCS PID to Received Core BID
			  // ******************************************************

			  // Setting up the category
			  Category = 5;

			  // Setting up the binding key
			  Key = PeerTuple.at (3);

			  // Setting up the values
			  Values.push_back (PeerNames.at (3));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, StoreBind01Msg, StoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  // Binding Received Core BID to Hash("Core") and vice-versa
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "Core", PeerNames
				  .at (3), StoreBind01Msg, StoreBind01);

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, PeerNames
				  .at (3), "Core", StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Peer PGCS HID to informed Peer Identifier.
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromHIDToIdentifier ("0.1", PeerNames
				  .at (1), PeerIdentifier, StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Peer PGCS identifier to HID.
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineFromIdentifierToHID ("0.1", PeerIdentifier, PeerNames
				  .at (1), StoreBind01Msg, StoreBind01);

			  // ******************************************************
			  // Binding Peer PGCS HID to Hash(Peer PGCS stack).
			  // ******************************************************

			  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 8, PeerNames
				  .at (1), PeerStack, StoreBind01Msg, StoreBind01);

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
			}
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Unable to read the first command line of the received message)" << endl;
	}

  Status = OK;

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

