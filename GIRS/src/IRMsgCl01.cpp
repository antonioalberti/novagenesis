/*
	NovaGenesis

	Name:		IRMsgCl01
	Object:		IRMsgCl01
	File:		IRMsgCl01.cpp
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

#ifndef _IRMSGCL01_H
#include "IRMsgCl01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

////#define DEBUG

IRMsgCl01::IRMsgCl01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRMsgCl01::~IRMsgCl01 ()
{
}

// Run the actions behind a received command line
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int
IRMsgCl01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  vector<string> StoreBindingsLimiters;
  vector<string> StoreBindingsSources;
  vector<string> StoreBindingsDestinations;
  Message *StoreBindings = 0;
  CommandLine *PCL;
  string Offset = "                    ";
  IR *PIR = 0;
  Block *PHTB = 0;
  string Key;
  vector<string> *Values = new vector<string>;

  PIR = (IR *)PB;

  PHTB = (Block *)PIR->PHT;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // Set the following flag independently of other actions
  PIR->GenerateSCNSeqForOriginalSource = true;

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
