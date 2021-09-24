/*
	NovaGenesis

	Name:		CoreSCNSeq01
	Object:		CoreSCNSeq01
	File:		CoreSCNSeq01.cpp
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

#ifndef _CORESCNSEQ01_H
#include "CoreSCNSeq01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

////#define DEBUG // To follow message processing

CoreSCNSeq01::CoreSCNSeq01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreSCNSeq01::~CoreSCNSeq01 ()
{
}

// Run the actions behind a received command line
// ng -scn --seq 0.1 [ < 1 string SCN > ]
int
CoreSCNSeq01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  CommandLine *PCL = 0;
  string ReceivedSCN;
  string NewSCN;
  string Offset = "                    ";
  Message *Run = 0;
  Core *PCore = 0;
  unsigned int NoCL = 0;

  PCore = (Core *)PB;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // ************************************************************************
  // Generate the ng -scn -seq if required for the ng -run --evaluation case
  // ************************************************************************

  if (ScheduledMessages.size () > 0)
	{

#ifdef DEBUG
	  PB->S << Offset << "(The message bellow was build as a scheduled message type)" << endl;
#endif

	  Run = ScheduledMessages.at (0);

	  if (Run != 0)
		{
		  Run->GetNumberofCommandLines (NoCL);

		  if (NoCL > 1)
			{
			  // ***************************************************
			  // Generate the ng -message --type [ < 1 string 1 > ]
			  // ***************************************************

			  //Run->NewCommandLine("-message","--type","0.1",PCL);

			  //PCL->NewArgument(1);

			  //PCL->SetArgumentElement(0,0,PB->IntToString(Run->GetType()));

			  // ***************************************************
			  // Generate the ng -message --seq [ < 1 string 1 > ]
			  // ***************************************************

			  //Run->NewCommandLine("-message","--seq","0.1",PCL);

			  //PCL->NewArgument(1);

			  //PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

			  // ***************************************************
			  // Generate the ng -scn --seq
			  // ***************************************************
			  PB->GenerateSCNFromMessageBinaryPatterns (Run, SCN);

			  // Creating the ng -scn --s command line
			  PMB->NewSCNCommandLine ("0.1", SCN, Run, PCL);

#ifdef DEBUG
			  PB->S << "(" << endl << *Run << ")" << endl;
#endif

			  // Push the message to the GW input queue
			  PCore->PGW->PushToInputQueue (Run);

			  Status = OK;
			}
		}
	}

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
