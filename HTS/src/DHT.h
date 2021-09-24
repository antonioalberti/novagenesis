/*
	NovaGenesis
	
	Name:		Distributed Hash Table
	Object:		DHT
	File:		DHT.h
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

#ifndef _DHT_H
#define _DHT_H

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _TUPLE_H
#include "Tuple.h"
#endif

#define ERROR 1
#define OK 0

using namespace std;

class DHT : public Block {
 private:

  // Gateway pointer
  GW *PGW;

  // HT pointer
  HT *PHT;

 public:

  // Auxiliary flags
  bool GenerateGetBindHeader;
  bool GenerateSCNSeqForStoreBind;
  bool GenerateSCNSeqForGetBind;
  bool ChangeState;
  int AuxCounter;

  // Auxiliary container
  vector<Tuple *> PSSTuples;

  // Auxiliary container
  vector<Tuple *> GIRSTuples;

  // Constructor
  DHT (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path);

  // Destructor
  ~DHT ();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Action related functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Allocate and add an Action on Actions container
  void NewAction (const string _LN, Action *&_PA);

  // Get an Action
  int GetAction (string _LN, Action *&_PA);

  // Delete an Action
  int DeleteAction (string _LN);

  // Delays
  double DelayBeforeRunInitiatilization;
  double DelayBeforeGIRSPSSDiscovery;
  double DelayBeforeRunExposition;
  double DelayBeforePIDPublishing;
  double DelayBeforeRunPeriodic;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Friend classes
  // ------------------------------------------------------------------------------------------------------------------------------
  friend class DHTRunInitialization01;
  friend class DHTSCNAck01;
  friend class DHTMsgCl01;
  friend class DHTDeliveryBind01;
  friend class DHTRunPeriodic01;
};

#endif






