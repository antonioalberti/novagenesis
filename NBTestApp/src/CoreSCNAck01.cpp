/*
	NovaGenesis

	Name:		CoreSCNAck01
	Object:		CoreSCNAck01
	File:		CoreSCNAck01.cpp
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

#ifndef _CORESCNACK01_H
#include "CoreSCNAck01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

CoreSCNAck01::CoreSCNAck01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreSCNAck01::~CoreSCNAck01 ()
{
}

// Run the actions behind a received command line
// ng -scn --ack 0.1 [ < 2 string SCN AckSCN > ]
int
CoreSCNAck01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  string ReceivedSCN;
  vector<string> PReceivedA;
  vector<string> PStoredA;
  string AckSCN;
  string Offset = "                    ";
  Core *PCore = 0;
  Message *Run = 0;
  CommandLine *PCL = 0;
  Publication *PP = 0;
  unsigned int NoCL = 0;

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // ************************************************************************
  // Sample the round trip time to publish on NRNCS
  // ************************************************************************

  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 1)
		{
		  // Get the fist argument
		  _PCL->GetArgument (0, PReceivedA);

		  // Get received message SCN
		  ReceivedSCN = PReceivedA.at (0);

		  // Get ack message SCN
		  AckSCN = PReceivedA.at (1);

		  //PB->S << Offset <<"(AckSCN = "<< AckSCN << ")"<< endl;

		  if (ReceivedSCN != "" && AckSCN != "")
			{
			  if (PCore->GetPublication (AckSCN, PP) == OK)
				{
				  if (PP != 0)
					{
					  double Now = GetTime ();

					  double DeltaT = Now - PP->Timestamp;

					  //PB->S << Offset << setprecision(10) <<"(The publication round trip time to NRNCS was "<<DeltaT<<")"<< endl;

					  PCore->pubrtt->Sample (DeltaT);

					  PCore->pubrtt->CalculateArithmetic ();

					  PCore->pubrtt->SampleToFile (Now);

					  PCore->DeletePublication (PP);
					}
				  else
					{
					  PB->S << Offset << "(ERROR: Null publication object)" << endl;

					  Status = ERROR;
					}
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Unable to read the arguments)" << endl;

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
	}

  // ************************************************************************
  // Generate the ng -scn -seq if required for the ng -run --evaluation case
  // ************************************************************************

  if (ScheduledMessages.size () > 0)
	{
	  if (PCore->GenerateRunXSCNSeq01 == true)
		{
		  //PB->S << Offset <<  "(There is a scheduled message)" << endl;

		  Run = ScheduledMessages.at (0);

		  if (Run != 0)
			{
			  // Generate the SCN
			  PB->GenerateSCNFromMessageBinaryPatterns (Run, SCN);

			  // Creating the ng -scn --s command line
			  PMB->NewSCNCommandLine ("0.1", SCN, Run, PCL);

			  //PB->S << "(" << endl << *Run << ")"<< endl;
			  if (Run->GetNumberofCommandLines (NoCL) == OK)
				{
				  if (NoCL > 2)
					{
					  // Push the message to the GW input queue
					  PCore->PGW->PushToInputQueue (Run);

					  PCore->GenerateRunXSCNSeq01 = false;

					  Status = OK;
					}
				}
			}
		}
	}

  // ************************************************************************
  // Generate the ng -scn -seq if required for the ng -sr --b case
  // ************************************************************************

  if (PCore->GenerateStoreBindingsSCNSeq01 == true)
	{
	  //PB->S << Offset <<  "(There is an inline storage message)" << endl;

	  // Generate the ng -scn -seq
	  PB->GenerateSCNFromMessageBinaryPatterns (InlineResponseMessage, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, InlineResponseMessage, PCL);

	  // Set for false. Wait another ng -d --b to go to true again
	  PCore->GenerateStoreBindingsSCNSeq01 = false;

	  // Set for true. Enable next message to be processed at ng -d --b
	  PCore->GenerateStoreBindingsMsgCl01 = true;
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
