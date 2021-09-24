/*
	NovaGenesis
	
	Name:		Publish/Subscribe
	Object:		PS
	File:		PS.h
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

#ifndef _PS_H
#define _PS_H

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

class PS : public Block {
 private:

  // Gateway pointer
  GW *PGW;

  // HT pointer
  HT *PHT;

 public:

  // Auxiliary flags
  bool MarkForDeletion;

  // Auxiliary container
  vector<Tuple *> GIRSTuples;

  // Auxiliary publisher tuple
  Tuple Publisher;

  // Constructor
  PS (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path);

  // Destructor
  ~PS ();

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
  double DelayBeforeGIRSDiscovery;
  double DelayBeforeRunExposition;
  double DelayBeforePIDPublishing;
  double DelayBeforeSendingToGIRS;
  double DelayBeforeSendingANotification;
  double DelayBeforeRunPeriodic;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Friend classes
  // ------------------------------------------------------------------------------------------------------------------------------

  friend class PSRunInitialization01;
  friend class PSStatusS01;
  friend class PSDeliveryBind01;
  friend class PSSCNAck01;
  friend class PSMsgCl01;
  friend class PSPubNotify01;
  friend class PSDeliveryBind01;
  friend class PSRunPeriodic01;
  friend class PSRevokeBind01;
};

#endif






