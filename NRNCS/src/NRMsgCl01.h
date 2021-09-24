/*
	NovaGenesis
	
	Name:		NRMsgCl01
	Object:		NRMsgCl01
	File:		NRMsgCl01.h
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

#ifndef _NRMSGCL01_H
#define _NRMSGCL01_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _ACTION_H
#include "Action.h"
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _MESSAGEBUILDER_H
#include "MessageBuilder.h"
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#define ERROR 1
#define OK 0

class Block;

class NRMsgCl01 : public Action {
 public:

  // Constructor
  NRMsgCl01 (string _LN, Block *_PB, MessageBuilder *_PMB);

  // Destructor
  virtual ~NRMsgCl01 ();

  // Run the actions behind a received message
  virtual int
  Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage);
};

#endif






