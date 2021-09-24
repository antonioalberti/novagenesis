/*
	NovaGenesis

	Name:		PSSCNAck01
	Object:		PSSCNAck01
	File:		PSSCNAck01.cpp
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

#ifndef _PSSCNACK01_H
#include "PSSCNAck01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _PS_H
#include "PS.h"
#endif

////#define DEBUG

PSSCNAck01::PSSCNAck01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PSSCNAck01::~PSSCNAck01 ()
{
}

// Run the actions behind a received command line
// ng -scn --ack 0.1 [ < 2 string SCN AckSCN > ]
int
PSSCNAck01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string ReceivedSCN;
  string AckSCN;
  string Offset = "                    ";
  PS *PPS = 0;
  Message *StoreBindings = 0;
  CommandLine *PCL = 0;

  return Status;
}
