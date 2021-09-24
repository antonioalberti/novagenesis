/*
	NovaGenesis

	Name:		HTMessageType01
	Object:		HTMessageType01
	File:		HTMessageType01.cpp
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

#ifndef _HTMESSAGETYPE01_H
#include "HTMessageType01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

HTMessageType01::HTMessageType01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTMessageType01::~HTMessageType01 ()
{
}

// Run the actions behind a received command line
// ng -message --type _Version [ < 1 string _Type > ]
int
HTMessageType01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  CommandLine *PTemp;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Copy the received command line to the InlineResponseMessage
  InlineResponseMessage->NewCommandLine (_PCL, PTemp);

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

