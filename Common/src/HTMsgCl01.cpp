/*
	NovaGenesis

	Name:		HTMsgCl01
	Object:		HTMsgCl01
	File:		HTMsgCl01.cpp
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

#ifndef _HTMSGCL01_H
#include "HTMsgCl01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

HTMsgCl01::HTMsgCl01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTMsgCl01::~HTMsgCl01 ()
{
}

// Run the actions behind a received command line
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int
HTMsgCl01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;

  unsigned int NA = 0;
  CommandLine *NewHTMsgCl01 = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  //vector<string>	*ListBindingsLimiters=new vector<string>;
  //vector<string>	*ListBindingsSources=new vector<string>;
  string Offset = "                    ";
  CommandLine *PCL = 0;
  Block *PGWB = 0;
  GW *PGW = 0;
  string GWLN = "GW";
  HT *PHT = 0;

  PHT = (HT *)PB;

  PB->PP->GetBlock (GWLN, PGWB);

  PGW = (GW *)PGWB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;


  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 3)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, Limiters);
		  _PCL->GetArgument (1, Sources);
		  _PCL->GetArgument (2, Destinations);

		  if (Limiters.size () > 0 && Sources.size () > 0 && Destinations.size () > 0)
			{
			  // Add a -m --cl 0.1 to the new message. Invert source and destinations in the message for the answer
			  if (PMB->NewConnectionLessCommandLine (_PCL->Version, &Limiters, &Destinations, &Sources, InlineResponseMessage, NewHTMsgCl01)
				  == OK)
				{
				  //if (PB->State == "operational")
				  //	{
				  //PB->S << Offset << "(Messages container size = "<<PB->GetNumberOfMessages()<<")"<< endl;

				  //PB->S << Offset << "(Deleting the previous marked messages)" << endl;

				  //	}

				  Status = OK;
				}
			  else
				{
				  PB->S << Offset << "(ERROR: Unable to create the -m --cl inline response message)" << endl;

				  Status = ERROR;
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
