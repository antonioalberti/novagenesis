/*
	NovaGenesis

	Name:		Action
	Object:		Action
	File:		Action.cpp
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

#ifndef _ACTION_H
#include "Action.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

Action::Action (string _LN, Block *_PB, MessageBuilder *_PMB)
{
  LN = _LN;
  PB = _PB;
  PMB = _PMB;

  Process *PR = NULL;
}

Action::~Action ()
{
}

// Set block legible name
void Action::SetLegibleName (string _LN)
{
  LN = _LN;
}

// Set block self-certifying name
void Action::SetSelfCertifyingName (string _SCN)
{
  SCN = _SCN;
}

// Get block legible name
string Action::GetLegibleName ()
{
  return LN;
}

// Get block self-certifying name
string Action::GetSelfCertifyingName ()
{
  return SCN;
}

// Run the actions behind a received command line
int
Action::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  return ERROR;
}

double Action::GetTime ()
{
  struct timespec t;

  clock_gettime (CLOCK_MONOTONIC, &t);

  return ((t.tv_sec) + (double)(t.tv_nsec / 1e9));
}
