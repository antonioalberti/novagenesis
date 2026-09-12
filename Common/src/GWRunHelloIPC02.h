/*
        NovaGenesis

        Name:		GWRunHelloIPC02
        Object:		GWRunHelloIPC02
        File:		GWRunHelloIPC02.h
        Author:		Antonio Marcos Alberti
        Date:		05/2026
        Version:	0.2

   Copyright (C) 2026  Antonio Marcos Alberti

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

#ifndef _GWRUNHELLOIPC02_H
#define _GWRUNHELLOIPC02_H

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

#define ERROR 1
#define OK 0

class Block;

using namespace std;

class GWRunHelloIPC02 : public Action
{
public:
  // Constructor
  GWRunHelloIPC02(string _LN, Block* _PB, MessageBuilder* _PMB);

  // Destructor
  ~GWRunHelloIPC02();

  // Run the actions behind a received command line
  // ng -run --helloIPC 0.2
  int Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage);
};

#endif
