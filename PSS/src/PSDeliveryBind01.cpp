/*
	NovaGenesis

	Name:		PSDeliveryBind01
	Object:		PSDeliveryBind01
	File:		PSDeliveryBind01.cpp
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

#ifndef _PSDELIVERYBIND01_H
#include "PSDeliveryBind01.h"
#endif

#ifndef _PS_H
#include "PS.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PSS_H
#include "PSS.h"
#endif

////#define DEBUG

PSDeliveryBind01::PSDeliveryBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PSDeliveryBind01::~PSDeliveryBind01 ()
{
}

// Run the actions behind a received command line
// ng -d --b 0.1
int
PSDeliveryBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Category;
  vector<string> Key;
  vector<string> Values;
  PS *PPS = 0;
  Message *StoreBindings = 0;
  CommandLine *PCL = 0;
  vector<string> StoreBindingsLimiters;
  vector<string> StoreBindingsSources;
  vector<string> StoreBindingsDestinations;
  Block *PHTB = 0;
  string SCN;

  PPS = (PS *)PB;

  PHTB = (Block *)PPS->PHT;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

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
				  if (PB->State == "operational")
					{
					  // ****************************************************
					  // Generate the ng -m --cl header for PSS::HT block
					  // ****************************************************

					  // Setting up the process SCN as the space limiter
					  StoreBindingsLimiters.push_back (PB->PP->Intra_Process);

					  // Setting up the CLI block SCN as the source SCN
					  StoreBindingsSources.push_back (PB->GetSelfCertifyingName ());

					  // Setting up the HT block SCN as the destination SCN
					  StoreBindingsDestinations.push_back (PHTB->GetSelfCertifyingName ());

					  // Creating a new message
					  PB->PP->NewMessage (GetTime (), 0, false, StoreBindings);

					  // Creating the ng -cl -m command line
					  PMB->NewConnectionLessCommandLine ("0.1", &StoreBindingsLimiters, &StoreBindingsSources, &StoreBindingsDestinations, StoreBindings, PCL);

					  // Generate the ng -sr --b for the local HT
					  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", 13, Key.at (0), &Values, StoreBindings, PCL);

					  // Generate the new SCN
					  PB->GenerateSCNFromMessageBinaryPatterns (StoreBindings, SCN);

					  // Add the SCN to the message
					  PMB->NewSCNCommandLine ("0.1", SCN, StoreBindings, PCL);

					  // ******************************************************
					  // Finish
					  // ******************************************************

					  // Push the message to the GW input queue
					  PPS->PGW->PushToInputQueue (StoreBindings);
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

