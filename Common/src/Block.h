/*
        NovaGenesis

        Name:		Block
        Object:		Block
        File:		Block.h
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

#ifndef _BLOCK_H
#define _BLOCK_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _GLIBCXX_IOMANIP
#include <iomanip>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _MSGHANDLE_H
#include "MsgHandle.h"
#endif

#ifndef _PROMPT_IOSTREAM_H
#include "Prompt_iostream.h"
#endif

#ifndef _HASHMULTIMAPS_H
#include "HashMultimaps.h"
#endif

#ifndef _MATH_H
#include <math.h>
#endif

#ifndef _INTTYPES_H
#include <inttypes.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#define ERROR 1
#define OK 0

// using namespace std;
using namespace tthread;

class Process;

class Block
{
private:
  // Legible Name
  string LN;

  // Self-certifying Name
  string SCN;

  // Working path
  string Path;

  // Block index
  unsigned int Index;

  // Auxiliary prime numbers array
  unsigned int primes[32];

public:
  // Operational state
  string State;

  // Stop processing message flag
  bool StopProcessingMessage;

  // Console ostream for prompt
  ConsoleOstream S;

  // Pointer to the process that hosts the block
  Process* PP;

  // Actions container
  vector<Action*> Actions;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Main functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Constructor
  Block(string _LN, Process* _PP, unsigned int _Index, string _Path);

  // Destructor
  virtual ~Block();

  // Run the actions behind a received message
  virtual int Run(Message* _ReceivedMessage, Message*& _InlineResponseMessage);

  // SPEC-033 Phase B gate 2 (Astra): entry with an ALREADY-OWNED retention.
  // The caller (GW pop path, B-3) holds a retention acquired from the queue
  // entry's original handle — identity guaranteed without pointer lookup.
  // This overload adopts that retention (does NOT add a second count) and
  // releases it at the single exit point. _RunHandle must be the handle the
  // retention was taken on; _ReceivedMessage must equal ResolveMessage
  // (_RunHandle).
  virtual int Run(Message* _ReceivedMessage, Message*& _InlineResponseMessage, const MsgHandle& _RunHandle);

private:
  // SPEC-033 Phase B gate 2: shared core behind both Run overloads.
  // _AdoptOwned == true -> adopt the caller's existing retention (queued
  // path, identity from the handle); false -> acquire via FindHandle +
  // TryRetain (legacy pointer path).
  int Run(Message* _ReceivedMessage, Message*& _InlineResponseMessage, const MsgHandle& _RunHandle, bool _AdoptOwned);

public:

  // ------------------------------------------------------------------------------------------------------------------------------
  // Basic functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Set block legible name
  void SetLegibleName(string _LN);

  // Set block self-certifying name
  void SetSelfCertifyingName(string _SCN);

  // Set working path
  void SetPath(string _Path);

  // Set operational state
  void SetState(string _State);

  // Get block legible name
  string GetLegibleName();

  // Get block self-certifying name
  string GetSelfCertifyingName();

  // Get working path
  string GetPath();

  // Get operational state
  string GetState();

  // Get index
  unsigned int GetIndex();

  // Allocate and add an Action on Actions container
  virtual void NewAction(const string _LN, Action*& _PA);

  // Get an Action
  virtual int GetAction(string _LN, Action*& _PA);

  // Delete an Action
  virtual int DeleteAction(string _LN);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary functions
  // ------------------------------------------------------------------------------------------------------------------------------

  int StringToInt(string _String);

  // Convert int to string
  string IntToString(int _Int);

  // Convert double to string
  string DoubleToString(double _Double);

  // Converting string to double
  double StringToDouble(string _String);

  // Convert unsigned int to string
  string UnsignedIntToString(unsigned int _Int);

  // Return the current time in seconds
  double GetTime();

  // Friends
  friend class Process;
};

#endif
