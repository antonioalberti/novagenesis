/*
	NovaGenesis

	Name:		NRSubBind01
	Object:		NRSubBind01
	File:		NRSubBind01.cpp
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

#ifndef _NRSUBBIND01_H
#include "NRSubBind01.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _NRNCS_H
#include "NRNCS.h"
#endif

NRSubBind01::NRSubBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

NRSubBind01::~NRSubBind01 ()
{
}

// Run the actions behind a received command line
// ng -s --b _Version [ < 1 string _Category > < _SCNsSize string S_1 ... S_SCNsSize > ]
int
NRSubBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Category;
  vector<string> Key;
  CommandLine *PCL = 0;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 2)
		{
		  // Get received command line arguments
		  if (_PCL->GetArgument (0, Category) == OK && _PCL->GetArgument (1, Key) == OK)
			{
			  if (Category.size () > 0 && Key.size () > 0)
				{
				  for (unsigned int i = 0; i < Key.size (); i++)
					{
					  // Change command line from ng -s --b to ng -g --b
					  PMB->NewGetCommandLine ("0.1", PB->StringToInt (Category.at (0)), Key
						  .at (i), InlineResponseMessage, PCL);
					}
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

