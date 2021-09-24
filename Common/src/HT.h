/*
	NovaGenesis
	
	Name:		Hash Table
	Object:		HT
	File:		HT.h
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

#ifndef _HT_H
#define _HT_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _CSTRING_H
#include <cstring>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _FSTREAM_H
#include <fstream>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _HASHMULTIMAPS_H
#include "HashMultimaps.h"
#endif

#define ERROR 1
#define OK 0
#define MAX_CATEGORIES 19

using namespace std;

class Process;

class HT : public Block {
 private:

  // Hash multimap container pointer
  HashStringMultimap *Bindings;

 public:

  // Constructor
  HT (string _LN, Process *_PP, unsigned int _Index, string _Path);

  // Destructor
  ~HT ();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary flag (to reduce size of status messages, 6th November 2017)
  // ------------------------------------------------------------------------------------------------------------------------------
  bool HaveOneStatusCL;


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

  // Store a binding in the Bindings container at category _Cat
  int StoreBinding (unsigned int _Cat, const string _Key, vector<string> *_Values);

  // Get a binding from the Bindings container at category _Cat
  int GetBinding (unsigned int _Cat, const string _Key, vector<string> *&_Values);

  // Revoke (remove) a binding from the Bindings container at category _Cat
  int RevokeBinding (unsigned int _Cat, const string _Key);

  // Delays
  double DelayBeforeRunPeriodic;

  // Friend classes
  friend class HTListBind01;

  friend class HTRunPeriodic01;

  friend class HTDeliveryBind01;

  friend class HTBindReport01;

  friend class HTRevokeBinding01;
};

#endif






