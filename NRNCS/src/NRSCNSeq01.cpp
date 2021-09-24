/*
	NovaGenesis

	Name:		NRSCNSeq01
	Object:		NRSCNSeq01
	File:		NRSCNSeq01.cpp
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

#ifndef _NRSCNSEQ01_H
#include "NRSCNSeq01.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

NRSCNSeq01::NRSCNSeq01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

NRSCNSeq01::~NRSCNSeq01 ()
{
}

// Run the actions behind a received command line
// ng -scn --seq 0.1 [ < 1 string SCN > ]
int
NRSCNSeq01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string ReceivedSCN;
  string NewSCN;
  unsigned int NA = 0;
  vector<string> PA;
  string Offset = "                    ";
  CommandLine *PCL = 0;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 1)
		{
		  // Get the fist argument
		  _PCL->GetArgument (0, PA);

		  // Get received message SCN
		  ReceivedSCN = PA.at (0);

		  if (ReceivedSCN != "")
			{
			  if (PB->State == "operational")
				{
				  PMB->NewSCNCommandLine ("0.1", ReceivedSCN, InlineResponseMessage, PCL);
				}

			  // Generate the SCN
			  PB->GenerateSCNFromMessageBinaryPatterns (InlineResponseMessage, NewSCN);

			  // Add the SCN to the message
			  PMB->NewSCNCommandLine ("0.1", NewSCN, InlineResponseMessage, PCL);

			  Status = OK;
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Unable to read the arguments)" << endl;

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
