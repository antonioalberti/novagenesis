/*
	NovaGenesis

	Name:		DHTMsgCl01
	Object:		DHTMsgCl01
	File:		DHTMsgCl01.cpp
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

#ifndef _DHTMSGCL01_H
#include "DHTMsgCl01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _DHT_H
#include "DHT.h"
#endif

DHTMsgCl01::DHTMsgCl01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

DHTMsgCl01::~DHTMsgCl01 ()
{
}

// Run the actions behind a received command line
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int
DHTMsgCl01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
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
  DHT *PDHT = 0;
  Block *PHTB = 0;
  Message *NewStore = 0;

  PDHT = (DHT *)PB;

  PHTB = (Block *)PDHT->PHT;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  //PB->S << Offset << "(Messages container size = "<<PB->GetNumberOfMessages()<<")"<< endl;

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
						  // Prepare the header to forward the message to the HT instance

						  //PB->S << Offset <<  "(Changing the first command line to forward a binding to the HT block)" << endl;

						  // Creating a new message from the received one
						  PB->PP->NewMessage (_ReceivedMessage, NewStore);

						  CommandLine *PMsgCl = 0;

						  // Get the first argument of the copied message
						  NewStore->GetCommandLine (0, PMsgCl);

						  // Change the last destination on received message
						  PMsgCl->SetArgumentElement (2, 3, PHTB->GetSelfCertifyingName ());

						  // Push the message to the GW input queue
						  PDHT->PGW->PushToInputQueue (NewStore);

						  ////PB->S << Offset <<  "(Deleting the previous marked messages)" << endl;
						}
					}
				}
			}
		}
	}


  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
