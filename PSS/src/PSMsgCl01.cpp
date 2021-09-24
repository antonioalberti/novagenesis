/*
	NovaGenesis

	Name:		PSMsgCl01
	Object:		PSMsgCl01
	File:		PSMsgCl01.cpp
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

#ifndef _PSMSGCL01_H
#include "PSMsgCl01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _PS_H
#include "PS.h"
#endif

////#define DEBUG

PSMsgCl01::PSMsgCl01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PSMsgCl01::~PSMsgCl01 ()
{
}

// Run the actions behind a received command line
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int
PSMsgCl01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  vector<string> StoreBindingsLimiters;
  vector<string> StoreBindingsSources;
  vector<string> StoreBindingsDestinations;
  Message *StoreBindings = 0;
  CommandLine *PCL;
  string Offset = "                    ";
  PS *PPS = 0;
  Block *PHTB = 0;
  Message *ForwardBindings = 0;

  PPS = (PS *)PB;

  PHTB = (Block *)PPS->PHT;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 3)
		{
		  // Get received command line arguments
		  if (_PCL->GetArgument (0, Limiters) == OK && _PCL->GetArgument (1, Sources) == OK
			  && _PCL->GetArgument (2, Destinations) == OK)
			{
			  if (Limiters.size () > 0 && Sources.size () > 0 && Destinations.size () > 0)
				{
				  // ****************************************************
				  // IntraDomain, interhost
				  // ****************************************************
				  if (Destinations.size () == 4)
					{
					  if (PB->State == "operational")
						{
#ifdef DEBUG
						  // Prepare the header to forward the message to the GIRS(s) instance(s)
						  PB->S << Offset << "(Creating the first command line to forward a binding to a GIRS)" << endl;
#endif

						  if (PPS->GIRSTuples.size () > 0)
							{

							  InlineResponseMessage->SetTime (GetTime () + PPS->DelayBeforeSendingToGIRS);

							  // Clear the destinations vector
							  Destinations.clear ();

							  int i = rand () % PPS->GIRSTuples.size ();

#ifdef DEBUG
							  PB->S << Offset << "(The peer GIRS HID is = " << PPS->GIRSTuples[i]->Values[0] << ")"
									<< endl;
							  PB->S << Offset << "(The peer GIRS OSID is = " << PPS->GIRSTuples[i]->Values[1] << ")"
									<< endl;
							  PB->S << Offset << "(The peer GIRS PID is = " << PPS->GIRSTuples[i]->Values[2] << ")"
									<< endl;
							  PB->S << Offset << "(The peer GIRS IR BID is = " << PPS->GIRSTuples[i]->Values[3] << ")"
									<< endl;
#endif

							  // Add the peer GIRS HID to the destinations vector
							  Destinations.push_back (PPS->GIRSTuples[i]->Values[0]);

							  // Add the peer GIRS OSID to the destinations vector
							  Destinations.push_back (PPS->GIRSTuples[i]->Values[1]);

							  // Add the peer GIRS PID to the destinations vector
							  Destinations.push_back (PPS->GIRSTuples[i]->Values[2]);

							  // Add the peer GIRS IR BID to the destinations vector
							  Destinations.push_back (PPS->GIRSTuples[i]->Values[3]);

							  // Add a -m --cl 0.1 to the new message
							  if (PMB->NewConnectionLessCommandLine ("0.2", &Limiters, &Sources, &Destinations, InlineResponseMessage, PCL)
								  == OK)
								{
								  Status = OK;
								}
							  else
								{
								  PB->S << Offset << "(ERROR: Unable to create the -m --cl inline response message)"
										<< endl;
								}
							}
						  else
							{
							  PB->S << Offset << "(ERROR: Unable to found the GIRS tuple)" << endl;
							}

						  // Clear the data of the last publisher
						  PPS->Publisher.Values.clear ();

						  // Stores the tuple of the source in case of the notification
						  for (unsigned int i = 0; i < Sources.size (); i++)
							{
							  PPS->Publisher.Values.push_back (Sources.at (i));
							}

#ifdef DEBUG

						  PB->S << Offset << "(Deleting the previous marked messages)" << endl;
#endif
						}
					}
				}
			}
		}
	}

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
