/*
	NovaGenesis

	Name:		GWHelloIPC01
	Object:		GWHelloIPC01
	File:		GWHelloIPC01.cpp
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

#ifndef _GWHELLOIPC01_H
#include "GWHelloIPC01.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

GWHelloIPC01::GWHelloIPC01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

GWHelloIPC01::~GWHelloIPC01 ()
{
}

// Run the actions behind a received command line
// ng -hello --ipc 0.1 [ < 2 string Peer_Key Peer_LN > ]
// ng -hello --ipc 0.1 [ < 2 string 21 HTS > ]
int
GWHelloIPC01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  unsigned int NA = 0;
  Message *GWStoreBind01Msg = 0;
  Message *GWStatusS01Msg = 0;
  CommandLine *GWStatusS01 = 0;
  CommandLine *GWMsgCl01 = 0;
  CommandLine *GWStoreBind01 = 0;
  GW *PGW = 0;
  unsigned int StatusCode = 0;
  vector<string> PeerData;
  vector<string> IPCOutputKeys;
  string HID;
  string PID;
  string BID;
  vector<string> ReceivedMessageSources;
  vector<string> GWStoreBind01MsgLimiters;
  vector<string> GWStoreBind01MsgSources;
  vector<string> GWStoreBind01MsgDestinations;
  string Version = "0.1";
  unsigned int Category;
  string Key;
  vector<string> Values;
  string HashLegiblePeerProcessName;
  string Identifier;
  string Offset = "          ";
  vector<string> RelatedSCNs;

  PGW = (GW *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Get the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // PB->S << Offset <<  "(The number of arguments is = " << NA << ")" << endl;

	  // Check the number of arguments and get the number of elements in first vector
	  if (NA == 1)
		{
		  // Get the information at received command line
		  _PCL->GetArgument (0, PeerData);

		  // Generate the MsgCl header for the store message
		  GWStoreBind01MsgLimiters.push_back (PB->PP->Intra_Process);
		  GWStoreBind01MsgSources.push_back (PB->GetSelfCertifyingName ());
		  GWStoreBind01MsgDestinations.push_back (PGW->PHT->GetSelfCertifyingName ());

		  // Get the sending process PID and GW block BID
		  _ReceivedMessage->GetCommandLine (0, GWMsgCl01);

		  if (GWMsgCl01 != 0)
			{
			  GWMsgCl01->GetArgument (1, ReceivedMessageSources);

        PB->S <<endl;

        PB->S << Offset <<  "(Discovered the peer service = "<<PeerData.at (1)<<" via shared memory. IPC is working properly.)"<<endl;

			  PB->GenerateSCNFromCharArrayBinaryPatterns (PeerData.at (1), HashLegiblePeerProcessName);

			  // Creating a new message
			  PB->PP->NewMessage (GetTime (), 0, false, GWStoreBind01Msg);

			  // ******************************************************
			  // Create the first command line
			  // ******************************************************

			  // Creating the ng -cl -m command line
			  PMB->NewConnectionLessCommandLine ("0.1", &GWStoreBind01MsgLimiters, &GWStoreBind01MsgSources, &GWStoreBind01MsgDestinations, GWStoreBind01Msg, GWMsgCl01);

			  // ******************************************************
			  /// Binding Received PID to Process IPC Input Key
			  // ******************************************************

			  // Setting up the category
			  Category = 13;

			  // Setting up the binding key
			  Key = ReceivedMessageSources.at (0);

			  Values.push_back (PeerData.at (0));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, GWStoreBind01Msg, GWStoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  /// Binding Received PID to Received BID
			  // ******************************************************

			  // Setting up the category
			  Category = 5;

			  // Setting up the binding key
			  Key = ReceivedMessageSources.at (0);

			  // Setting up the values
			  Values.push_back (ReceivedMessageSources.at (1));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, GWStoreBind01Msg, GWStoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  /// Binding Hash("Legible Process Name") to Received PID
			  // ******************************************************

			  // Setting up the category
			  Category = 2;

			  // Setting up the binding key
			  Key = HashLegiblePeerProcessName;

			  // Setting up the values
			  Values.push_back (ReceivedMessageSources.at (0));

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, GWStoreBind01Msg, GWStoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  /// Binding Received PID to Hash("Legible Process Name")
			  // ******************************************************

			  // Setting up the category
			  Category = 3;

			  // Setting up the binding key
			  Key = ReceivedMessageSources.at (0);

			  // Setting up the values
			  Values.push_back (HashLegiblePeerProcessName);

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, GWStoreBind01Msg, GWStoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  /// Binding Hash("Legible Process Name") to HID
			  // ******************************************************

			  // Setting up the category
			  Category = 9;

			  // Setting up the binding key
			  Key = HashLegiblePeerProcessName;

			  // Setting up the values
			  Values.push_back (PB->PP->GetHostSelfCertifyingName ());

			  // Creating the ng -sr --b 0.1 command line
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, GWStoreBind01Msg, GWStoreBind01);

			  // Clearing the values container
			  Values.clear ();

			  // ******************************************************
			  // Setting up the SCN command line
			  // ******************************************************

			  // Generate the SCN
			  PB->GenerateSCNFromMessageBinaryPatterns (GWStoreBind01Msg, SCN);

			  // Creating the ng -scn --s command line
			  PMB->NewSCNCommandLine ("0.1", SCN, GWStoreBind01Msg, GWStoreBind01);

			  // ******************************************************
			  // Finish
			  // ******************************************************

			  // Push the message to the GW input queue
			  PGW->PushToInputQueue (GWStoreBind01Msg);
			}

		  // Complete the ng -st --s message to ack the hello

		  //PB->S << Offset <<  "(ScheduledMessages size is "<<ScheduledMessages.size()<<")" << endl;

		  if (ScheduledMessages.size () == 1)
			{
			  GWStatusS01Msg = ScheduledMessages[0];

			  if (GWStatusS01Msg != 0)
				{
				  // Configuring the PGCS::HT BID as a related SCN
				  RelatedSCNs.push_back (PGW->PHT->GetSelfCertifyingName ());

				  // Add a -st --s 0.1 to the new inline message
				  PMB->NewStatusCommandLine (_PCL->Version, "-hello", "--ipc", StatusCode, &RelatedSCNs, GWStatusS01Msg, GWStatusS01);

				  // Give time to run the store binding before
				  GWStatusS01Msg->SetTime (GetTime () + PGW->DelayBeforeStatusIPC);

				  //PB->S << *GWStatusS01Msg << endl;

				  Status = OK;

				  PB->State = "operational";
				}
			  else
				{
				  PB->S << Offset << "(ERROR: The status message is null)" << endl;
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Unable to access the scheduled message index at Run Action interface)"
					<< endl;
			}

		  //PB->S << Offset <<  "(Done)" << endl << endl << endl;
		}
	}

  return Status;
}
