/*
	NovaGenesis

	Name:		HTRevokeBind01
	Object:		HTRevokeBind01
	File:		HTRevokeBind01.cpp
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

#ifndef _HTREVOKEBIND01_H
#include "HTRevokeBind01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

////#define DEBUG

HTRevokeBind01::HTRevokeBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTRevokeBind01::~HTRevokeBind01 ()
{
}

// Run the actions behind a received command line
// ng -rvk --b 0.1 [ < 1 string Category > < 1 string Key > ]
int
HTRevokeBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  unsigned int *NE = new unsigned int[100];
  CommandLine *NewStatusS01 = 0;
  vector<string> PArguments;
  unsigned int Category = 0;
  string Key = " ";
  unsigned int StatusCode = 0;
  string Offset = "                    ";

#ifdef DEBUG
  PB->S << Offset <<  this->GetLegibleName() << endl;
#endif

  // Get the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  //PB->S << Offset <<  "(The number of arguments is = " << NA << ")";

	  // Check the number of arguments and get the number of elements in first vector
	  if (NA == 2 && _PCL->GetNumberofArgumentElements (0, NE[0]) == OK)
		{
		  //PB->S << Offset <<  "(The number of elements in first argument is = " << NE[0] << ")";

		  // Check the number of elements in the first argument
		  if (NE[0] == 1)
			{
			  _PCL->GetArgument (0, PArguments);

			  // Convert the string to a unsigned int
			  Category = PB->StringToInt (PArguments.at (0));

#ifdef DEBUG
			  PB->S << Offset <<  "(The category is = " << Category << ")";
#endif

			  PArguments.clear ();

			  // Check valid category value
			  if (Category >= 0 || Category <= 18)
				{
				  // Check the number of elements in second vector
				  if (_PCL->GetNumberofArgumentElements (1, NE[1]) == OK)
					{
					  //PB->S << Offset <<  "(The number of elements in second argument is = " << NE[1] << ")";

					  // Check the number of elements in the second argument
					  if (NE[1] == 1)
						{
						  _PCL->GetArgument (1, PArguments);

						  Key = PArguments.at (0);

#ifdef DEBUG
						  PB->S << "(The key is = "<< Key<< ")" << endl;
#endif

						  PArguments.clear ();

						  // Check for invalid key
						  if (Key != " ")
							{
							  // Check for HT block.The revoke bind command line only makes sense in a HT block
							  if (PB->GetLegibleName () == "HT")
								{
								  HT *PHT = (HT *)PB;

								  // Store the binding on the hash_multimap container
								  PHT->RevokeBinding (Category, Key);

								  // Add a -st --s 0.1 to the new message
								  PMB->NewStatusCommandLine (_PCL->Version, "-rvk", "--b", StatusCode, InlineResponseMessage, NewStatusS01);
								}
							  else
								{
								  PB->S << Offset
										<< "(ERROR: Unable to perform the revoke. Wrong block! An HT block is required)"
										<< endl;
								}
							}
						  else
							{
							  PB->S << Offset
									<< "(ERROR: Unable to read the binding key from the received command line)" << endl;
							}
						}
					  else
						{
						  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
						}
					}
				  else
					{
					  PB->S << Offset << "(ERROR: Wrong number of elements on second argument)" << endl;
					}
				}
			  else
				{
				  PB->S << Offset << "(ERROR: Invalid category)" << endl;
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Wrong number of elements on first argument)" << endl;
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

  delete[] NE;

#ifdef DEBUG

  PB->S << Offset <<  "(Done)" << endl << endl << endl;

#endif

  return Status;
}
