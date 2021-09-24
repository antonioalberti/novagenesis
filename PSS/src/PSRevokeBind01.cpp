/*
	NovaGenesis

	Name:		PSRevokeBind01
	Object:		PSRevokeBind01
	File:		PSRevokeBind01.cpp
	Author:		Antonio Marcos Alberti
	Date:		5/2016
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

#ifndef _PSREVOKEBIND01_H
#include "PSRevokeBind01.h"
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

PSRevokeBind01::PSRevokeBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PSRevokeBind01::~PSRevokeBind01 ()
{
}

// Run the actions behind a received command line
// ng -rvk --b _Version [ < 1 string _Category > < 1 string _key > ]
int
PSRevokeBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  CommandLine *PCL = 0;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  InlineResponseMessage->NewCommandLine (_PCL, PCL);

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}

