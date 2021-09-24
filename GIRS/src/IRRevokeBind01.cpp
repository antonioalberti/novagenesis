/*
	NovaGenesis

	Name:		IRRevokeBind01
	Object:		IRRevokeBind01
	File:		IRRevokeBind01.cpp
	Author:		Antonio Marcos Alberti
	Date:		5/2016
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

#ifndef _IRREVOKEBIND01_H
#include "IRRevokeBind01.h"
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

////#define DEBUG

IRRevokeBind01::IRRevokeBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRRevokeBind01::~IRRevokeBind01 ()
{
}

// Run the actions behind a received command line
// ng -rvk --b _Version [ < 1 string _Category > < 1 string _Key > ]
int
IRRevokeBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Category;
  vector<string> Key;
  CommandLine *PTemp;
  IR *PIR = 0;
  int Index = 0;

  PIR = (IR *)PB;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

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
				  // Select the appropriated message index of the ScheduledMessages to whose the store CL needs to be copied
				  if (SelectHTS (Key.at (0), PIR->HTSTuples.size (), Index) == OK)
					{
#ifdef DEBUG
					  PB->S << endl << Offset << "Index = " << Index << endl;

					  PB->S << Offset << "HTSTuples.size() = " << PIR->HTSTuples.size () << endl;

					  PB->S << Offset << "ScheduledMessages.size() = " << ScheduledMessages.size () << endl;
#endif

					  if (Index <= (int)ScheduledMessages.size ())
						{
						  // Copy the received command line to the InlineResponseMessage
						  ScheduledMessages.at (Index)->NewCommandLine (_PCL, PTemp);

						  // Return to 0.1 version
						  PTemp->Version = "0.1";
						}
					}
				  else
					{
					  PB->S << Offset << "(ERROR: Unable to select an HTS instance)" << endl;
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

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

int IRRevokeBind01::SelectHTS (string _Key, int _NumberOfInstances, int &_Index)
{
  unsigned long long Total = 0;
  unsigned int X = 0;
  unsigned long long Partial = 0;
  int Status = ERROR;

  if (_NumberOfInstances >= 1)
	{
	  char Word[_Key.size ()];

	  //cout <<"SCN = "<<_Key<<endl;

	  for (unsigned int k = 0; k < 8; k++)
		{
		  Word[k] = _Key[k];
		}

	  for (unsigned int i = 0; i < 8; i++)
		{
		  stringstream Temp1;

		  Temp1 << hex << Word[i];

		  Temp1 >> X;

		  //cout <<"X = "<<setprecision(0)<<X<<endl;

		  //cout <<"Exp = "<< setprecision(0) <<pow(16,(31-i))<<endl;

		  Partial = X * ((unsigned long long)pow (4, (7 - i)));

		  Total = Total + Partial;

		  //cout <<"Partial = "<< setprecision(0)<<Partial<<endl;

		  //cout <<"Total = "<< setprecision(0)<<Total<<endl;
		}

	  _Index = Total % _NumberOfInstances;

	  Status = OK;
	}

  return Status;
}

