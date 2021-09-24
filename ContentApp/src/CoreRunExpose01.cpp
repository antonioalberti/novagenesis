/*
	NovaGenesis

	Name:		CoreRunExpose01
	Object:		CoreRunExpose01
	File:		CoreRunExpose01.cpp
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

#ifndef _CORERUNEXPOSE01_H
#include "CoreRunExpose01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

CoreRunExpose01::CoreRunExpose01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreRunExpose01::~CoreRunExpose01 ()
{
}

// Run the actions behind a received command line
// ng -run --expose 0.1 [ < 1 string Limiter > < 3 string Category1 Key1 Value1 >  < 3 string Category2 Key2 Value2 > ... < 3 string CategoryN KeyN ValueN> ]
int
CoreRunExpose01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Limiter;
  vector<string> Terna;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Core *PCore = 0;
  Message *Publish = 0;
  CommandLine *PCL = 0;
  vector<string> *Values = new vector<string>;

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA >= 2)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, Limiter);

		  // Pairs=_PCL->Arguments.at(1);
		  if (Limiter.size () > 0)
			{
			  if (Limiter.at (0) == PB->PP->Intra_Domain)
				{
				  PB->S << Offset << "(Generating a message to publish in the domain scope some App bindings )" << endl;

				  // Setting up the OSID as the space limiter
				  Limiters.push_back (PB->PP->Intra_Domain);

				  // Setting up the this OS as the 1st source SCN
				  Sources.push_back (PB->PP->GetHostSelfCertifyingName ());

				  // Setting up the this OS as the 2nd source SCN
				  Sources.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

				  // Setting up the this process as the 3rd source SCN
				  Sources.push_back (PB->PP->GetSelfCertifyingName ());

				  // Setting up the PS block SCN as the 4th source SCN
				  Sources.push_back (PB->GetSelfCertifyingName ());

				  // Setting up the destination 1st source
				  Destinations.push_back (PCore->PSTuples[0]->Values[0]);

				  // Setting up the destination 2nd source
				  Destinations.push_back (PCore->PSTuples[0]->Values[1]);

				  // Setting up the destination 3rd source
				  Destinations.push_back (PCore->PSTuples[0]->Values[2]);

				  // Setting up the destination 4th source
				  Destinations.push_back (PCore->PSTuples[0]->Values[3]);

				  // Creating a new message
				  PB->PP->NewMessage (GetTime (), 1, false, Publish);

				  // Creating the ng -cl -m command line
				  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Publish, PCL);

				  // ***************************************************
				  // Generate the get bindings to be published
				  // ***************************************************

				  for (unsigned int j = 1; j < NA; j++)
					{
					  // Get received command line arguments
					  _PCL->GetArgument (j, Terna);

					  // Pairs=_PCL->Arguments.at(1);
					  if (Terna.size () > 0)
						{
						  Values->push_back (Terna.at (2));

						  // Generate the ng -p --b
						  PMB->NewCommonCommandLine ("-p", "--b", "0.1", PB->StringToInt (Terna.at (0)), Terna
							  .at (1), Values, Publish, PCL);

						  Values->clear ();
						}

					  Terna.clear ();
					}

				  // ***************************************************
				  // Generate the ng -message --type [ < 1 string 1 > ]
				  // ***************************************************

				  //Publish->NewCommandLine("-message","--type","0.1",PCL);

				  //PCL->NewArgument(1);

				  //PCL->SetArgumentElement(0,0,PB->IntToString(Publish->GetType()));

				  // ***************************************************
				  // Generate the ng -message --seq [ < 1 string 1 > ]
				  // ***************************************************

				  //Publish->NewCommandLine("-message","--seq","0.1",PCL);

				  //PCL->NewArgument(1);

				  //PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

				  // ******************************************************
				  // Generate the SCN
				  // ******************************************************
				  PB->GenerateSCNFromMessageBinaryPatterns (Publish, SCN);

				  // Creating the ng -scn --s command line
				  PMB->NewSCNCommandLine ("0.1", SCN, Publish, PCL);

				  // ******************************************************
				  // Finish
				  // ******************************************************

				  // Push the message to the GW input queue
				  PCore->PGW->PushToInputQueue (Publish);

				  if (ScheduledMessages.size () > 0)
					{
					  Message *Temp = ScheduledMessages.at (0);

					  Temp->MarkToDelete ();

					  // Make the scheduled messages vector empty
					  ScheduledMessages.clear ();
					}

				  Status = OK;
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

  delete Values;

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

