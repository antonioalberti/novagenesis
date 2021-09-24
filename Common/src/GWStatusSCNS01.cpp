/*
	NovaGenesis

	Name:		GWStatusSCNS01
	Object:		GWStatusSCNS01
	File:		GWStatusSCNS01.cpp
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

#ifndef _GWSTATUSSCNS01_H
#include "GWStatusSCNS01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

GWStatusSCNS01::GWStatusSCNS01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

GWStatusSCNS01::~GWStatusSCNS01 ()
{
}

// Run the actions behind a received command line
// ng -st --scns _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > < _RelatedSCNsSize string S_1 ... S_RelatedSCNsSize > ]
int
GWStatusSCNS01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  vector<string> RelatedCommand;
  vector<string> StatusCode;
  string Offset = "          ";
  GW *PGW = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string SCN = "FFFFFFFF";
  vector<string> LegibleNames;
  Message *GWStoreBind01Msg = 0;
  CommandLine *GWStoreBind01 = 0;
  CommandLine *GWMsgCl01 = 0;
  vector<string> GWStoreBind01MsgLimiters;
  vector<string> GWStoreBind01MsgSources;
  vector<string> GWStoreBind01MsgDestinations;
  vector<string> ReceivedMessageSources;
  string Version = "0.1";
  unsigned int Category;
  string Key;
  vector<string> Values;
  string HashLegiblePeerProcessName;
  vector<string> RelatedSCNs;
  string PGSHTSCN;

  // ******************************************************
  // Calculate auxiliary variables and required input data
  // ******************************************************

  PGW = (GW *)PB;

  LegibleNames.push_back (PB->PP->GetLegibleName ());

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 3)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, RelatedCommand);
		  _PCL->GetArgument (1, StatusCode);

		  if (RelatedCommand.size () > 0 && StatusCode.size () > 0)
			{
			  if (RelatedCommand.at (0) == "-hello" && RelatedCommand.at (1) == "--ipc" && StatusCode.at (0) == "0"
				  && PB->State == "hello")
				{
				  //PB->S << Offset << "("<<LN<<" is moving to operational state)"<<endl;

				  PB->State = "operational";

				  _PCL->GetArgument (2, RelatedSCNs);

				  // Get the sending process PID and GW block BID
				  _ReceivedMessage->GetCommandLine (0, GWMsgCl01);

				  if (PB->PP->GetLegibleName () != "PGCS" && PB->StopProcessingMessage == false && GWMsgCl01 != 0
					  && RelatedSCNs.size () > 0)
					{
					  // Generate a SCN from the received process name
					  PB->GenerateSCNFromCharArrayBinaryPatterns ("PGCS", HashLegiblePeerProcessName);

					  // Creating a new message
					  PB->PP->NewMessage (GetTime (), 0, false, GWStoreBind01Msg);

					  // Generate the MsgCl header for the store message
					  GWStoreBind01MsgLimiters.push_back (PB->PP->Intra_Process);
					  GWStoreBind01MsgSources.push_back (PB->GetSelfCertifyingName ());
					  GWStoreBind01MsgDestinations.push_back (PGW->PHT->GetSelfCertifyingName ());

					  GWMsgCl01->GetArgument (1, ReceivedMessageSources);

					  // ******************************************************
					  // Create the first command line
					  // ******************************************************

					  // Creating the ng -cl -m command line
					  PMB->NewConnectionLessCommandLine (Version,
														 &GWStoreBind01MsgLimiters,
														 &GWStoreBind01MsgSources,
														 &GWStoreBind01MsgDestinations,
														 GWStoreBind01Msg,
														 GWMsgCl01);

					  // ******************************************************
					  //  Binding Received PID to Received BID
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
					  //  Binding Received BID to Hash("GW") and vice versa
					  // ******************************************************
					  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, ReceivedMessageSources
						  .at (1), "GW", GWStoreBind01Msg, GWStoreBind01);

					  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "GW", ReceivedMessageSources
						  .at (1), GWStoreBind01Msg, GWStoreBind01);

					  // ******************************************************
					  //  Binding Received PID to Received HT BID
					  // ******************************************************

					  // Note: The HT BID is transported as a related SCN on ng -st --scns command line

					  // Setting up the category
					  Category = 5;

					  // Setting up the binding key
					  Key = ReceivedMessageSources.at (0);

					  // Setting up the values
					  Values.push_back (RelatedSCNs.at (0));

					  // Creating the ng -sr --b 0.1 command line
					  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", Category, Key, &Values, GWStoreBind01Msg, GWStoreBind01);

					  // Clearing the values container
					  Values.clear ();

					  // ******************************************************
					  //  Binding HT BID to Hash("HT") and vice versa
					  // ******************************************************
					  PMB->NewStoreBindingCommandLineSCNToHashLN ("0.1", 3, RelatedSCNs
						  .at (0), "HT", GWStoreBind01Msg, GWStoreBind01);

					  PMB->NewStoreBindingCommandLineFromHashLNToSCN ("0.1", 2, "HT", RelatedSCNs
						  .at (0), GWStoreBind01Msg, GWStoreBind01);

					  // ******************************************************
					  // Binding Received PID to Hash("PGCS")
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
					  //  Binding Hash("PGCS") to Received PID
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

					  Status = OK;

					  //PB->S << Offset <<  "(Done)" << endl << endl << endl;
					}
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;

			  Status = ERROR;
			}
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;

		  Status = ERROR;
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
