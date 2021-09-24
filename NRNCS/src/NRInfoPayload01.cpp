/*
	NovaGenesis

	Name:		NRInfoPayload01
	Object:		NRInfoPayload01
	File:		NRInfoPayload01.cpp
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

#ifndef _NRINFOPAYLOAD01_H
#include "NRInfoPayload01.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

NRInfoPayload01::NRInfoPayload01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

NRInfoPayload01::~NRInfoPayload01 ()
{
}

// Run the actions behind a received command line
// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
int
NRInfoPayload01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Values;
  char *Payload = 0;
  CommandLine *PCL = 0;
  long long Size = 0;

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
					  // Get the payload size
					  _ReceivedMessage->GetPayloadSize (Size);

					  if (Size > 0)
						{
						  // Get the pointer of the received message payload char array
						  _ReceivedMessage->GetPayloadFromCharArray (Payload);

						  //PB->S <<"Plotting the message payload recovered by NRNCS"<<endl;

						  // Check for error
						  if (Payload != 0)
							{
							  // Copy the payload from received message *Payload array to the new message
							  InlineResponseMessage->SetPayloadFromCharArray (Payload, Size);

							  // Copy the ng -info --payload to the new message
							  InlineResponseMessage->NewCommandLine (_PCL, PCL);

							  // Change to 0.2 version
							  PCL->Version = "0.1";

							  //PB->S <<"A new pub for the file "<<Values.at(0)<<" was received with size "<<Size<<" bytes"<<endl; // TODO: FIXP/Update - Added this line to follow files being published
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
