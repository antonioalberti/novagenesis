/*
	NovaGenesis

	Name:		EPGS novagenesis message
	Object:		ng_message
	File:		ng_message.h
	Author:		Vâner José Magalhães
	Date:		05/2021
	Version:	0.1

  	Copyright (C) 2021  Vâner José Magalhães

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

#ifndef MESSAGE_NG_MESSAGE_H_
#define MESSAGE_NG_MESSAGE_H_

#include "ng_command.h"
#include "../epgs_defines.h"

#define DELETED_BY_APP true
#define DELETED_BY_CORE false

struct _ng_message {
  // Type
  short Type;

  // CommandLine container
  NgCommand **CommandLines;

  // Number of command lines
  unsigned int NoCL;

  // Has payload control flag. True means that the message has a payload.
  bool HasPayloadFlag;

  // Payload char array. Can be used to carry a payload in memory instead of saving it to a file
  char *Payload;

  // Message char array. Can be used to carry the message to memory instead of using a file
  char *Msg;

  // The size of the payload in bytes
  int PayloadSize;

  // The size of the message in bytes
  int MessageSize;

  // Time for priority queue comparison
  double Time;
};

typedef struct _ng_message NgMessage;

void ng_create_message (double _Time, short _Type, bool _HasPayload, NgMessage **ngMessage);

void ng_destroy_message (struct _ng_message **ngMessage);

int NewCommandLine (struct _ng_message *ngMessage, NgCommand *CL);

// Get a CommandLine by its index
int GetCommandLine (struct _ng_message *ngMessage, unsigned int _Index, NgCommand **CL);

// Set *Payload from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
int SetPayloadFromCharArray (struct _ng_message *ngMessage, char *_Value, int _Size);

// Set *Msg from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
int SetMessageFromCharArray (struct _ng_message *ngMessage, char *_Value, int _Size);

int ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray (struct _ng_message **ngMessage);

void MessageToString (struct _ng_message *ngMessage, char **out);

void MessageFromString (const char *in, int msgSize, struct _ng_message **ngMessage);

#endif /* MESSAGE_NG_MESSAGE_H_ */
