/*
	NovaGenesis

	Name:		Indirection Resolution
	Object:		IR
	File:		IR.cpp
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

#ifndef _IR_H
#include "IR.h"
#endif

#ifndef _IRRUNINITIALIZATION01_H
#include "IRRunInitialization01.h"
#endif

#ifndef _IRMSGCL01_H
#include "IRMsgCl01.h"
#endif

#ifndef _IRMSGCL02_H
#include "IRMsgCl02.h"
#endif

#ifndef _IRSTATUSS01_H
#include "IRStatusS01.h"
#endif

#ifndef _IRSCNACK01_H
#include "IRSCNAck01.h"
#endif

#ifndef _IRDELIVERYBIND01_H
#include "IRDeliveryBind01.h"
#endif

#ifndef _IRSCNSEQ01_H
#include "IRSCNSeq01.h"
#endif

#ifndef _IRSCNSEQ02_H
#include "IRSCNSeq02.h"
#endif

#ifndef _IRSTOREBIND01_H
#include "IRStoreBind01.h"
#endif

#ifndef _IRSTOREBIND02_H
#include "IRStoreBind02.h"
#endif

#ifndef _IRGETBIND01_H
#include "IRGetBind01.h"
#endif

#ifndef _IRREVOKEBIND01_H
#include "IRRevokeBind01.h"
#endif

#ifndef _IRGETBIND02_H
#include "IRGetBind02.h"
#endif

#ifndef _IRINFOPAYLOAD01_H
#include "IRInfoPayload01.h"
#endif

#ifndef _IRINFOPAYLOAD02_H
#include "IRInfoPayload02.h"
#endif

#ifndef _IRMESSAGETYPE01_H
#include "IRMessageType01.h"
#endif

#ifndef _IRMESSAGETYPE02_H
#include "IRMessageType02.h"
#endif

#ifndef _IRRUNPERIODIC01_H
#include "IRRunPeriodic01.h"
#endif

IR::IR (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
	: Block (_LN, _PP, _Index, _Path)
{
  PGW = _PGW;
  PHT = _PHT;
  State = "initialization";

  Message *PIM = 0;
  CommandLine *PCL = 0;
  Message *InlineResponseMessage = 0;
  Action *PA = 0;

  GenerateSCNSeqForOriginalSource = true;

  // Setting the delays
  DelayBeforeRunInitiatilization = 3;
  DelayBeforeDHTPSSDiscovery = 40;
  DelayBeforePIDPublishing = 20;
  DelayBeforeRunExposition = 2;

  // Creating the actions
  NewAction ("-run --initialization 0.1", PA);
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-m --cl 0.2", PA);
  NewAction ("-st --s 0.1", PA);
  NewAction ("-scn --ack 0.1", PA);
  NewAction ("-d --b 0.1", PA);
  NewAction ("-d --b 0.2", PA);
  NewAction ("-scn --seq 0.1", PA);
  NewAction ("-scn --seq 0.2", PA);
  NewAction ("-sr --b 0.1", PA);
  NewAction ("-sr --b 0.2", PA);
  NewAction ("-g --b 0.1", PA);
  NewAction ("-g --b 0.2", PA);
  NewAction ("-rvk --b 0.1", PA);
  NewAction ("-info --payload 0.1", PA);
  NewAction ("-info --payload 0.2", PA);
  NewAction ("-message --type 0.1", PA);
  NewAction ("-message --type 0.2", PA);
  NewAction ("-run --periodic 0.1", PA);

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialization", "0.1", PCL);

  // Execute the procedure
  Run (PIM, InlineResponseMessage);
}

IR::~IR ()
{
  Tuple *Temp = 0;

  vector<Tuple *>::iterator it1;

  for (it1 = PSSTuples.begin (); it1 != PSSTuples.end (); it1++)
	{
	  Temp = *it1;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}

  vector<Tuple *>::iterator it2;

  for (it2 = HTSTuples.begin (); it2 != HTSTuples.end (); it2++)
	{
	  Temp = *it2;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}

  vector<Action *>::iterator it4;

  Action *Temp2 = 0;

  for (it4 = Actions.begin (); it4 != Actions.end (); it4++)
	{
	  Temp2 = *it4;

	  if (Temp2 != 0)
		{
		  delete Temp2;
		}

	  Temp2 = 0;
	}
}

// Allocate and add an Action on Actions container
void IR::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-m --cl 0.1")
	{
	  IRMsgCl01 *P = new IRMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-m --cl 0.2")
	{
	  IRMsgCl02 *P = new IRMsgCl02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --initialization 0.1")
	{
	  IRRunInitialization01 *P = new IRRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-st --s 0.1")
	{
	  IRStatusS01 *P = new IRStatusS01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --ack 0.1")
	{
	  IRSCNAck01 *P = new IRSCNAck01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-d --b 0.1")
	{
	  IRDeliveryBind01 *P = new IRDeliveryBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --seq 0.1")
	{
	  IRSCNSeq01 *P = new IRSCNSeq01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --seq 0.2")
	{
	  IRSCNSeq02 *P = new IRSCNSeq02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-sr --b 0.1")
	{
	  IRStoreBind01 *P = new IRStoreBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-rvk --b 0.1")
	{
	  IRRevokeBind01 *P = new IRRevokeBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-sr --b 0.2")
	{
	  IRStoreBind02 *P = new IRStoreBind02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-g --b 0.1")
	{
	  IRGetBind01 *P = new IRGetBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-g --b 0.2")
	{
	  IRGetBind02 *P = new IRGetBind02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-info --payload 0.1")
	{
	  IRInfoPayload01 *P = new IRInfoPayload01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-info --payload 0.2")
	{
	  IRInfoPayload02 *P = new IRInfoPayload02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-message --type 0.1")
	{
	  IRMessageType01 *P = new IRMessageType01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-message --type 0.2")
	{
	  IRMessageType02 *P = new IRMessageType02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --periodic 0.1")
	{
	  IRRunPeriodic01 *P = new IRRunPeriodic01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int IR::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int IR::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}





