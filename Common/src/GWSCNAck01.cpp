/*
	NovaGenesis

	Name:		GWSCNAck01
	Object:		GWSCNAck01
	File:		GWSCNAck01.cpp
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

#ifndef _GWSCNACK01_H
#include "GWSCNAck01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

GWSCNAck01::GWSCNAck01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

GWSCNAck01::~GWSCNAck01 ()
{
}

// Run the actions behind a received command line
// ng -scn --ack 0.1 SCN AckSCN
int
GWSCNAck01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  CommandLine *PStoredCL = 0;
  unsigned int NReceivedA = 0;
  string ReceivedSCN;
  vector<string> PReceivedA;
  vector<string> PStoredA;
  string AckSCN;
  string Offset = "                    ";
  Message *PM = 0;                                // Pointer to the message being analyzed
  unsigned int NCL = 0;                                // Number of command lines
  unsigned int NStoredA = 0;                            // Number of arguments
  //GW				*PGW=0;

  //PGW=(GW*)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
