/*
	NovaGenesis

	Name:		GWSCNSeq01
	Object:		GWSCNSeq01
	File:		GWSCNSeq01.cpp
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

#ifndef _GWSCNSEQ01_H
#include "GWSCNSeq01.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

GWSCNSeq01::GWSCNSeq01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

GWSCNSeq01::~GWSCNSeq01 ()
{
}

// Run the actions behind a received command line
// ng -scn --ack 0.1 [ < 1 string SCN > ]
int
GWSCNSeq01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  CommandLine *PCL = 0;
  unsigned int NA = 0;
  string ReceivedSCN;
  vector<string> PA;
  string NewSCN;
  Message *GWStatusS01Msg = 0;
  GW *PGW = 0;
  string Offset = "          ";

  PGW = (GW *)PB;

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
			  //PB->S << Offset <<  "(ScheduledMessages size is "<<ScheduledMessages.size()<<")" << endl;

			  if (ScheduledMessages.size () == 1)
				{
				  GWStatusS01Msg = ScheduledMessages[0];

				  if (GWStatusS01Msg != 0)
					{
					  // Generate the SCN for the GWStatusS01Msg
					  PB->GenerateSCNFromMessageBinaryPatterns (GWStatusS01Msg, NewSCN);

					  PMB->NewSCNCommandLine ("0.1", NewSCN, ReceivedSCN, GWStatusS01Msg, PCL);

					  // Push the message to the GW input queue
					  PGW->PushToInputQueue (GWStatusS01Msg);

					  Status = OK;
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
