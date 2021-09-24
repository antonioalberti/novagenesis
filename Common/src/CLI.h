/*
	NovaGenesis
	
	Name:		Command Line Interface
	Object:		CLI
	File:		CLI.h
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

#ifndef _CLI_H
#define _CLI_H

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _SSTREAM_H
#include <sstream>
#endif

#ifndef _IOMANIP_H
#include <iomanip>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _QUEUE_H
#include <queue>
#endif

#ifndef _SYSMSG_H
#include <sys/msg.h>
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#define ERROR 1
#define OK 0

using namespace std;
using namespace tthread;

class GW;
class HT;

class CLI : public Block {
 private:

  string Version;

  // Index at Blocks container
  unsigned int Index;

  // Gateway pointer
  GW *PGW;

  // HT pointer
  HT *PHT;

 public:

  // Constructor
  CLI (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path);

  // Destructor
  ~CLI ();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Action related functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Allocate and add an Action on Actions container
  void NewAction (const string _LN, Action *&_PA);

  // Get an Action
  int GetAction (string _LN, Action *&_PA);

  // Delete an Action
  int DeleteAction (string _LN);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Core functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Command line interface prompt
  void Prompt ();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Wrapper function for Prompt() thread
  static void PromptThreadWrapper (void *aArg);

  // Friend classes
  friend class CLIRunInitialization01;
};

#endif






