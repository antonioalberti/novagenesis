/*
	NovaGenesis

	Name:		IRSCNSeq01
	Object:		IRSCNSeq01
	File:		IRSCNSeq01.cpp
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

#ifndef _IRSCNSEQ01_H
#include "IRSCNSeq01.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

IRSCNSeq01::IRSCNSeq01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRSCNSeq01::~IRSCNSeq01 ()
{
}

// Run the actions behind a received command line
// ng -scn --seq 0.1 [ < 1 string SCN > ]
int
IRSCNSeq01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string ReceivedSCN;
  string NewSCN;
  unsigned int NA = 0;
  vector<string> PA;
  string Offset = "                    ";
  CommandLine *PCL = 0;
  IR *PIR = 0;

  PIR = (IR *)PB;

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
			  if (PB->State == "operational" && PIR->GenerateSCNSeqForOriginalSource == true)
				{
				  PMB->NewSCNCommandLine ("0.1", ReceivedSCN, InlineResponseMessage, PCL);

				  PIR->GenerateSCNSeqForOriginalSource = false;
				}

			  // Generate the new SCN
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
