/*
	NovaGenesis
	
	Name:		Action
	Object:		Action
	File:		Action.h
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
#define _ACTION_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _MESSAGEBUILDER_H
#include "MessageBuilder.h"
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

#define ERROR 1
#define OK 0

using namespace std;

class Block;

class Action {
 public:

  // Legible Name
  string LN;

  // Self-certifying Name
  string SCN;

  // Pointer to the block where the action is instantiated
  Block *PB;

  // Pointer to the process message builder
  MessageBuilder *PMB;

  // Constructor
  Action (string _LN, Block *_PB, MessageBuilder *_PMB);

  // Destructor
  virtual ~Action ();

  // Set block legible name
  void SetLegibleName (string _LN);

  // Set block self-certifying name
  void SetSelfCertifyingName (string _SCN);

  // Get block legible name
  string GetLegibleName ();

  // Get block self-certifying name
  string GetSelfCertifyingName ();

  // Run the actions behind a received command line
  virtual int
  Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage);

  // Return the current time in seconds
  double GetTime ();
};

#endif






