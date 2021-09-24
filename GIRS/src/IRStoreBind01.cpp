/*
	NovaGenesis

	Name:		IRStoreBind01
	Object:		IRStoreBind01
	File:		IRStoreBind01.cpp
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

#ifndef _IRSTOREBIND01_H
#include "IRStoreBind01.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _GIRS_H
#include "GIRS.h"
#endif

IRStoreBind01::IRStoreBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRStoreBind01::~IRStoreBind01 ()
{
}

// Run the actions behind a received command line
// // ng -sr --b _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > ]
int
IRStoreBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Category;
  vector<string> Key;
  vector<string> Values;
  CommandLine *PTemp;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 3)
		{
		  // Get received command line arguments
		  if (_PCL->GetArgument (0, Category) == OK && _PCL->GetArgument (1, Key) == OK
			  && _PCL->GetArgument (2, Values) == OK)
			{
			  if (Category.size () > 0 && Key.size () > 0 && Values.size () > 0)
				{
				  // Copy the received command line to the InlineResponseMessage
				  InlineResponseMessage->NewCommandLine (_PCL, PTemp);
				}
			  else
				{
				  PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Unable to read the arguments)" << endl;
			}
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

