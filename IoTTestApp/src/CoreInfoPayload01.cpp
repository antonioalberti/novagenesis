/*
	NovaGenesis

	Name:		CoreInfoPayload01
	Object:		CoreInfoPayload01
	File:		CoreInfoPayload01.cpp
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

#ifndef _COREINFOPAYLOAD01_H
#include "CoreInfoPayload01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

CoreInfoPayload01::CoreInfoPayload01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreInfoPayload01::~CoreInfoPayload01 ()
{
}

// Run the actions behind a received command line
// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
int
CoreInfoPayload01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Values;
  Core *PCore = 0;

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 1)
		{
		  // Get received command line argument
		  _PCL->GetArgument (0, Values);

		  if (Values.size () > 0)
			{
			  // The additional information is related to the message payload
			  if (_PCL->Alternative == "--payload")
				{
				  if (_ReceivedMessage->GetHasPayloadFlag () == true)
					{
					  string PayloadPath = PB->GetPath ();

					  PB->S << Offset << "(The received message has a payload whose file is named " << Values.at (0)
							<< ")" << endl;
					  //PB->S << Offset <<  "(Saving the payload on the path "<<PayloadPath<<")" << endl;

					  _ReceivedMessage->SetPayloadFileName (Values.at (0));
					  _ReceivedMessage->SetPayloadFilePath (PayloadPath);
					  _ReceivedMessage->SetPayloadFileOption ("BINARY");
					  _ReceivedMessage->ExtractPayloadCharArrayFromMessageCharArray ();
					  _ReceivedMessage->ConvertPayloadFromCharArrayToFile ();

					  // Update related Subscription
					  for (unsigned int i = 0; i < PCore->Subscriptions.size (); i++)
						{
						  Subscription *PS = PCore->Subscriptions[i];

						  PB->S << Offset << "(Testing subscription " << i << ")" << endl;

						  PB->S << Offset << "(Subscription status is " << PS->Status << ")" << endl;

						  if (PS->Status == "Waiting delivery" && PS->HasContent == true)
							{
							  //PB->S << Offset <<  "(Storing the file named "<<Values.at(0)<<" to this subscription)" << endl;

							  //PB->S << Offset << "(Changing subscription status from \"Waiting delivery\" to \"Processing required\")" << endl;

							  PS->Status = "Processing required";

							  PS->FileName = Values.at (0);

#ifdef DEBUG

							  PCore->Debug.OpenOutputFile();

							  PCore->Debug << "Received the file " << Values.at(0) << " related to the subscription of index " << i << ". Status of the subscription is " <<PS->Status<< endl;

							  PCore->Debug.CloseFile();

#endif
							}
						}

					  // **************************************************
					  // Type 1 messages statistics
					  // **************************************************
					  if (_ReceivedMessage->GetType () == 1)
						{
						  double Time = GetTime ();

						  // Sample
						  PCore->tsmiup1->Sample (Time - _ReceivedMessage->GetInstantiationTime ());

						  // Update the mean
						  PCore->tsmiup1->CalculateArithmetic ();

						  // Sample to file
						  PCore->tsmiup1->SampleToFile (Time);
						}
					}
				}
			}
		}
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
