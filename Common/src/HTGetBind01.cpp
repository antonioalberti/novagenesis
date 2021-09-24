/*
	NovaGenesis

	Name:		HTGetBind01
	Object:		HTGetBind01
	File:		HTGetBind01.cpp
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

#ifndef _HTGETBIND01_H
#include "HTGetBind01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

HTGetBind01::HTGetBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTGetBind01::~HTGetBind01 ()
{
}

// Run the actions behind a received command line
// ng -g --b 0.1 [ < 1 string _Category > < 1 string _Key > ]
int
HTGetBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  unsigned int NA = 0;
  unsigned int *NE = new unsigned int[100];
  CommandLine *NewHTDeliveryBind01 = 0;
  vector<string> PArguments;
  unsigned int Category = 0;
  string Key = " ";
  string Offset = "                    ";

  //PB->S << Offset <<  this->GetLegibleName() << endl;

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

			  PArguments.clear ();

			  // Check valid category value
			  if (Category > 0 || Category < 18)
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

						  PArguments.clear ();

						  // Check for invalid key
						  if (Key != " ")
							{
							  // Check for HT block. The get bind command line only makes sense in an HT block
							  if (PB->GetLegibleName () == "HT")
								{
								  // Cast the owners block to a HT block
								  HT *PHT = (HT *)PB;

								  vector<string> *_Values = new vector<string>;

								  const string _Key = Key;

								  // Get the binding from the hash_multimap container
								  PHT->GetBinding (Category, _Key, _Values);

								  // Add a delivery command line to the in line response message
								  // ng -d --b 0.1 [ < 1 string Category > < 1 string Key > < n string S_1 ... S_n > ]
								  PMB->NewCommonCommandLine ("-d", "--b", _PCL
									  ->Version, Category, _Key, _Values, InlineResponseMessage, NewHTDeliveryBind01);

								  // Note: The following code is an exception to the general storage rule. It adds a ng -info --payload cl
								  //		 to the message and carries the content associated to the subscription of the message.
								  //       In future versions, this will be revisited.

								  if (Category == 18)
									{
									  if (_Values != 0)
										{
										  if (_Values->size () > 0)
											{
											  string ThePath;

											  ThePath = PB->GetPath ();

											  InlineResponseMessage
												  ->SetMessage (GetTime (), 0, true, "Temp.txt", _Values
													  ->at (0), "Message.ngs", ThePath);

											  InlineResponseMessage->ConvertPayloadFromFileToCharArray ();

											  // Adding only the ng -info --payload 01 command line
											  PMB->NewInfoPayloadCommandLine ("0.1", _Values, InlineResponseMessage, NewHTDeliveryBind01);
											}
										  else
											{
											  PB->S << Offset << "(ERROR: The value under the key " << _Key
													<< " is not here. Look somewhere else.)" << endl;
											}
										}
									  else
										{
										  PB->S << Offset
												<< "(ERROR: Unable to get the value stored under the informed key)"
												<< endl;
										}
									}

								  // Delete temporary values
								  _Values->clear ();

								  delete _Values;

								  Status = OK;
								}
							  else
								{
								  PB->S << Offset
										<< "(ERROR: Unable to perfom the store. Wrong block! An HT block is required)"
										<< endl;

								  Status = ERROR;
								}
							}
						  else
							{
							  PB->S << Offset
									<< "(ERROR: Unable to read the binding key from the received command line)" << endl;

							  Status = ERROR;
							}
						}
					  else
						{
						  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;

						  Status = ERROR;
						}
					}
				  else
					{
					  PB->S << Offset << "(ERROR: Wrong number of elements on second argument)" << endl;

					  Status = ERROR;
					}
				}
			  else
				{
				  PB->S << Offset << "(ERROR: Invalid category)" << endl;

				  Status = ERROR;
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Wrong number of elements on first argument)" << endl;

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

	  Status = ERROR;
	}

  delete[] NE;

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
