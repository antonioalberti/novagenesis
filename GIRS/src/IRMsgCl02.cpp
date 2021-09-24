/*
	NovaGenesis

	Name:		IRMsgCl02
	Object:		IRMsgCl02
	File:		IRMsgCl02.cpp
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

#ifndef _IRMSGCL02_H
#include "IRMsgCl02.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

////#define DEBUG

IRMsgCl02::IRMsgCl02 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRMsgCl02::~IRMsgCl02 ()
{
}

// Run the actions behind a received command line
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int
IRMsgCl02::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
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
  IR *PIR = 0;
  Block *PHTB = 0;
  string Key;
  vector<string> *Values = new vector<string>;
  Message *Forward = 0;

  PIR = (IR *)PB;

  PHTB = (Block *)PIR->PHT;

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
						  // ************************************************************************
						  // Prepare the header to forward the message to the appropriate HTS instance
						  // ************************************************************************

#ifdef DEBUG
						  PB->S << Offset << "(Creating the first command line to forward the message to a peer HTS)"
								<< endl;
#endif
						  if (PIR->HTSTuples.size () > 0)
							{
							  // Making one -m --cl header for each peer HTS
							  for (unsigned int i = 0; i < PIR->HTSTuples.size (); i++)
								{

#ifdef DEBUG
								  PB->S << Offset << "(The peer HTS is of index = " << i << ")" << endl;
								  PB->S << Offset << "(The peer HTS HID is = " << PIR->HTSTuples[i]->Values[0] << ")"
										<< endl;
								  PB->S << Offset << "(The peer HTS OSID is = " << PIR->HTSTuples[i]->Values[1] << ")"
										<< endl;
								  PB->S << Offset << "(The peer HTS PID is = " << PIR->HTSTuples[i]->Values[2] << ")"
										<< endl;
								  PB->S << Offset << "(The peer HTS IR BID is = " << PIR->HTSTuples[i]->Values[3] << ")"
										<< endl;
#endif

								  // Clear the destinations vector
								  Destinations.clear ();

								  // Add the peer GIRS HID to the destinations vector
								  Destinations.push_back (PIR->HTSTuples[i]->Values[0]);

								  // Add the peer GIRS OSID to the destinations vector
								  Destinations.push_back (PIR->HTSTuples[i]->Values[1]);

								  // Add the peer GIRS PID to the destinations vector
								  Destinations.push_back (PIR->HTSTuples[i]->Values[2]);

								  // Add the peer GIRS IR BID to the destinations vector
								  Destinations.push_back (PIR->HTSTuples[i]->Values[3]);

								  // Creating a new message
								  PB->PP->NewMessage (GetTime (), 1, false, Forward);

								  // Add a -m --cl 0.1 to the new message - Return CL version to 0.1
								  if (PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Forward, PCL)
									  == OK)
									{
									  // Store this message on the scheduled messages vector
									  ScheduledMessages.push_back (Forward);

									  Status = OK;
									}
								  else
									{
									  PB->S << Offset << "(ERROR: Unable to create the -m --cl inline response message)"
											<< endl;
									}
								}
							}
						}
					}

				  // Set the following flag independently of other actions
				  PIR->GenerateSCNSeqForOriginalSource = true;
				}
			}
		}
	}

  Values->clear ();

  delete Values;

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
