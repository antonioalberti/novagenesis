/*
	NovaGenesis

	Name:		IRInfoPayload02
	Object:		IRInfoPayload02
	File:		IRInfoPayload02.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.2

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

#ifndef _IRINFOPAYLOAD02_H
#include "IRInfoPayload02.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

IRInfoPayload02::IRInfoPayload02 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRInfoPayload02::~IRInfoPayload02 ()
{
}

// Run the actions behind a received command line
// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
int
IRInfoPayload02::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Values;
  char *Payload = 0;
  CommandLine *PCL = 0;
  long long int Size = 0;
  IR *PIR = 0;
  unsigned int Index = 0;
  bool HaveStoreBind = false;
  bool HaveCategory18 = false;
  vector<string> Temp;

  Temp.push_back ("18"); // Bind category with message payload content

  PIR = (IR *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 1)
		{
		  // Get received command line argument
		  if (_PCL->GetArgument (0, Values) == OK)
			{
			  if (Values.size () > 0)
				{
				  if (PB->State == "operational")
					{
					  // Extract the payload from the received msg char array
					  //_ReceivedMessage->ExtractPayloadCharArrayFromMessageCharArray();

					  // Get the payload size
					  _ReceivedMessage->GetPayloadSize (Size);

					  if (Size > 0)
						{
						  // Get the pointer of the received message payload char array
						  _ReceivedMessage->GetPayloadFromCharArray (Payload);

						  // Check for error
						  if (Payload != 0)
							{
							  if (ScheduledMessages.size () > 0)
								{
								  // Searching for category 18 at ng -sr --b CLs
								  for (unsigned int i = 0; i < ScheduledMessages.size (); i++)
									{
									  ScheduledMessages.at (i)
										  ->DoesThisCommandLineExistsInMessage ("-sr", "--b", HaveStoreBind);

									  if (HaveStoreBind == true)
										{
										  ScheduledMessages.at (i)
											  ->DoAllTheseValuesExistInSomeCommandLine (&Temp, HaveCategory18);

										  if (HaveCategory18 == true)
											{
											  Index = i;

											  break;
											}
										}
									}

								  if (HaveCategory18 == true)
									{
									  // Copy the payload from received message *Payload array to the new message with index i
									  ScheduledMessages.at (Index)->SetPayloadFromCharArray (Payload, Size);

									  // Copy the ng -info --payload to the new message
									  ScheduledMessages.at (Index)->NewCommandLine (_PCL, PCL);

									  // Return to 0.1 version
									  PCL->Version = "0.1";
									}
								}
							}
						  else
							{
							  PB->S << Offset << "(ERROR: Unable to copy the payload)" << endl;
							}
						}
					  else
						{
						  PB->S << Offset << "(ERROR: The payload size is zero)" << endl;
						}
					}
				}
			}
		}
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
