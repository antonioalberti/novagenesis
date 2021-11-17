/*
	NovaGenesis

	Name:		CoreDeliveryBind01
	Object:		CoreDeliveryBind01
	File:		CoreDeliveryBind01.cpp
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

#ifndef _COREDELIVERYBIND01_H
#include "CoreDeliveryBind01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _IOTTESTAPP_H
#include "IoTTestApp.h"
#endif

#define DEBUG

CoreDeliveryBind01::CoreDeliveryBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreDeliveryBind01::~CoreDeliveryBind01 ()
{
}

// Run the actions behind a received command line
// ng -d --b 0.1
int
CoreDeliveryBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Category;
  vector<string> Key;
  vector<string> Values;
  vector<string> StoreBindingsLimiters;
  vector<string> StoreBindingsSources;
  vector<string> StoreBindingsDestinations;
  CommandLine *PCL = 0;
  Block *PHTB = 0;
  Core *PCore = 0;
  Message *RunEvaluate = 0;

  PCore = (Core *)PB;

  PHTB = (Block *)PCore->PHT;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 3)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, Category);
		  _PCL->GetArgument (1, Key);
		  _PCL->GetArgument (2, Values);

		  if (Category.size () > 0 && Key.size () > 0 && Values.size () > 0) // VVV
			{
			  if (PCore->GenerateStoreBindingsMsgCl01 == true)
				{
				  // ****************************************************
				  // Generate the ng -m --cl header for App::HT block
				  // ****************************************************

				  // Setting up the process SCN as the space limiter
				  StoreBindingsLimiters.push_back (PB->PP->Intra_Process);

				  // Setting up the CLI block SCN as the source SCN
				  StoreBindingsSources.push_back (PB->GetSelfCertifyingName ());

				  // Setting up the HT block SCN as the destination SCN
				  StoreBindingsDestinations.push_back (PHTB->GetSelfCertifyingName ());

				  // Creating the ng -cl -m command line
				  PMB->NewConnectionLessCommandLine ("0.1", &StoreBindingsLimiters, &StoreBindingsSources, &StoreBindingsDestinations, InlineResponseMessage, PCL);

				  // Set the flag to false to avoid the generation of multiples command lines at the InlineResponseMessage
				  PCore->GenerateStoreBindingsMsgCl01 = false;

				  // Set the flag to true to generate a ng -scn --seq for the InlineResponseMessage
				  PCore->GenerateStoreBindingsSCNSeq01 = true;

				  // ***************************************************
				  // Generate the ng -message --type [ < 1 string 1 > ]
				  // ***************************************************

				  //InlineResponseMessage->NewCommandLine("-message","--type","0.1",PCL);

				  //PCL->NewArgument(1);

				  //PCL->SetArgumentElement(0,0,PB->IntToString(InlineResponseMessage->GetType()));

				  // ***************************************************
				  // Generate the ng -message --seq [ < 1 string 1 > ]
				  // ***************************************************

				  //InlineResponseMessage->NewCommandLine("-message","--seq","0.1",PCL);

				  //PCL->NewArgument(1);

				  //PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));
				}

			  // Generate the ng -sr --b for the local HT
			  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", PB->StringToInt (Category.at (0)), Key
				  .at (0), &Values, InlineResponseMessage, PCL);

			  if (PCore->GenerateRunX01 == true)
				{
				  if (ScheduledMessages.size () > 0)
					{
					  //PB->S << Offset <<  "(There is a scheduled message)" << endl;

					  RunEvaluate = ScheduledMessages.at (0);

					  if (RunEvaluate != 0)
						{
						  // Adding a ng -run --evaluation command line
						  RunEvaluate->NewCommandLine ("-run", "--evaluate", "0.1", PCL);

						  Status = OK;
						}
					}

				  PCore->GenerateRunX01 = false;
				}

			  // Update related Subscription
			  for (unsigned int i = 0; i < PCore->Subscriptions.size (); i++)
				{
				  Subscription *PS = PCore->Subscriptions[i];

				  //PB->S << Offset <<  "(Testing subscription "<<i<<")" << endl;

				  //PB->S << Offset <<  "(Subscription status is "<<PS->Status<<")" << endl;

				  if (PS->Status == "Waiting delivery")
					{
					  //PB->S << Offset <<  "(Checking the key)" << endl;

					  for (unsigned int t = 0; t < Key.size (); t++)
						{
						  if (Key.at (t) == PS->Key)
							{
							  if (PS->Category == 18)
								{
								  PS->HasContent = true;
								}

							  //PB->S << Offset <<  "(The following key matched = "<<Key.at(t)<<". Going to check the ng -info --payload command line)" << endl;

#ifdef DEBUG
							  // Open debug file
							  PCore->Debug.OpenOutputFile();

							  PCore->Debug << "A category 18 binding was received. A previous created subscription with index "<<i<<" matched the key = " << PS->Key << ". Status of the subscription is " <<PS->Status<< endl;

							  PCore->Debug.CloseFile();
#endif
							  double Time = GetTime ();

							  // Sample the time performance
							  double DeltaT = Time - PS->Timestamp;

							  //PB->S << Offset << setprecision(10) <<"(The entire subscription time was  "<<DeltaT<<" seconds)"<< endl;

							  PCore->subrtt->Sample (DeltaT);

							  PCore->subrtt->CalculateArithmetic ();

							  PCore->subrtt->SampleToFile (Time);
							}
						}
					}
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;
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

