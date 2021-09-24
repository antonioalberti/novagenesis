/*
	NovaGenesis

	Name:		HTStatusS01
	Object:		HTStatusS01
	File:		HTStatusS01.cpp
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

#ifndef _HTSTATUSS01_H
#include "HTStatusS01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

HTStatusS01::HTStatusS01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTStatusS01::~HTStatusS01 ()
{
}

// Run the actions behind a received command line
// ng -st --s _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > ]
int
HTStatusS01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  vector<string> RelatedCommand;
  vector<string> StatusCode;
  string Offset = "                    ";

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 2)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, RelatedCommand);
		  _PCL->GetArgument (1, StatusCode);

		  if (RelatedCommand.size () > 0 && StatusCode.size () > 0)
			{
			  if (RelatedCommand.at (0) == "-sr" && RelatedCommand.at (1) == "--b" && StatusCode.at (0) == "0"
				  && PB->State == "operational")
				{
				  //PB->S << Offset <<  "(There is no need to provide an inline message)" << endl;

				  // Stop processing the other command lines of the message.
				  PB->StopProcessingMessage = true;

				  Status = OK;
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
