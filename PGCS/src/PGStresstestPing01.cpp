/*
	NovaGenesis

	Name:		PGStresstestPing01
	Object:		PGStresstestPing01
	File:		PGStresstestPing01.cpp
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

#ifndef _PGSTRESSTESTPING01_H
#include "PGStresstestPing01.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#include <iostream>

#define DEBUG

PGStresstestPing01::PGStresstestPing01 (string _LN, Block *_PB, MessageBuilder *_PMB)
	: Action (_LN, _PB, _PMB)
{
}

PGStresstestPing01::~PGStresstestPing01 () {}

int PGStresstestPing01::Run (Message *_ReceivedMessage, CommandLine *_PCL,
							 vector<Message *> &ScheduledMessages,
							 Message *&InlineResponseMessage)
{
  PG *PPG = (PG *)PB;
  string Offset = "                    ";

  std::cerr << ">>> PGStresstestPing01::Run CALLED <<<" << std::endl;

#ifdef DEBUG
  PB->S << Offset << this->GetLegibleName() << endl;
#endif

  if (!PPG->StressEnabled)
    return OK;

  PPG->StressReceived++;

#ifdef DEBUG
  PB->S << Offset << "(Stress ping received. Total Received=" << PPG->StressReceived << ")" << endl;
#endif

  if (PPG->StressReceived % 100 == 0)
    {
      PPG->StressStats << PPG->GetTime () << " Sent=" << PPG->StressSent
		       << " Recv=" << PPG->StressReceived
		       << " Drop=" << PPG->StressDropped << endl;
    }

  return OK;
}
