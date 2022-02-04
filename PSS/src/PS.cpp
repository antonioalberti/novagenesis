/*
	NovaGenesis

	Name:		Publish/Subscribe
	Object:		PS
	File:		PS.cpp
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
#include "PS.h"
#endif

#ifndef _PSRUNINITIALIZATION01_H
#include "PSRunInitialization01.h"
#endif

#ifndef _PSMSGCL01_H
#include "PSMsgCl01.h"
#endif

#ifndef _PSSTATUSS01_H
#include "PSStatusS01.h"
#endif

#ifndef _PSSCNACK01_H
#include "PSSCNAck01.h"
#endif

#ifndef _PSDELIVERYBIND01_H
#include "PSDeliveryBind01.h"
#endif

#ifndef _PSPUBBIND01_H
#include "PSPubBind01.h"
#endif

#ifndef _PSSCNSEQ01_H
#include "PSSCNSeq01.h"
#endif

#ifndef _PSSUBBIND01_H
#include "PSSubBind01.h"
#endif

#ifndef _PSREVOKEBIND01_H
#include "PSRevokeBind01.h"
#endif

#ifndef _PSINFOPAYLOAD01_H
#include "PSInfoPayload01.h"
#endif

#ifndef _PSPUBNOTIFY01_H
#include "PSPubNotify01.h"
#endif

#ifndef _PSRUNPERIODIC01_H
#include "PSRunPeriodic01.h"
#endif

PS::PS (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
	: Block (_LN, _PP, _Index, _Path)
{
  PGW = _PGW;
  PHT = _PHT;
  State = "initialization";

  Message *PIM = 0;
  CommandLine *PCL = 0;
  Message *InlineResponseMessage = 0;
  Action *PA = 0;

  // Setting the delays
  DelayBeforeRunInitiatilization = 3;
  DelayBeforeGIRSDiscovery = 60;
  DelayBeforePIDPublishing = 20;
  DelayBeforeRunExposition = 2;
  DelayBeforeSendingToGIRS = 0.001;
  DelayBeforeSendingANotification = 0.03;

  MarkForDeletion = false;

  // Creating the actions
  NewAction ("-run --initialization 0.1", PA);
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-st --s 0.1", PA);
  NewAction ("-scn --ack 0.1", PA);
  NewAction ("-d --b 0.1", PA);
  NewAction ("-p --b 0.1", PA);
  NewAction ("-s --b 0.1", PA);
  NewAction ("-rvk --b 0.1", PA);
  NewAction ("-scn --seq 0.1", PA);
  NewAction ("-info --payload 0.1", PA);
  NewAction ("-p --notify 0.1", PA);
  NewAction ("-message --type 0.1", PA);
  NewAction ("-run --periodic 0.1", PA);

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialization", "0.1", PCL);

  // Execute the procedure
  Run (PIM, InlineResponseMessage);
}

PS::~PS ()
{
  Tuple *Temp = 0;

  vector<Tuple *>::iterator it1;

  for (it1 = GIRSTuples.begin (); it1 != GIRSTuples.end (); it1++)
	{
	  Temp = *it1;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}

  vector<Action *>::iterator it4;

  Action *Temp1;

  for (it4 = Actions.begin (); it4 != Actions.end (); it4++)
	{
	  Temp1 = *it4;

	  if (Temp1 != 0)
		{
		  delete Temp1;
		}

	  Temp1 = 0;
	}
}

// Allocate and add an Action on Actions container
void PS::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-m --cl 0.1")
	{
	  PSMsgCl01 *P = new PSMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --initialization 0.1")
	{
	  PSRunInitialization01 *P = new PSRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-st --s 0.1")
	{
	  PSStatusS01 *P = new PSStatusS01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --ack 0.1")
	{
	  PSSCNAck01 *P = new PSSCNAck01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-d --b 0.1")
	{
	  PSDeliveryBind01 *P = new PSDeliveryBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-p --b 0.1")
	{
	  PSPubBind01 *P = new PSPubBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --seq 0.1")
	{
	  PSSCNSeq01 *P = new PSSCNSeq01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-s --b 0.1")
	{
	  PSSubBind01 *P = new PSSubBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-rvk --b 0.1")
	{
	  PSRevokeBind01 *P = new PSRevokeBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-info --payload 0.1")
	{
	  PSInfoPayload01 *P = new PSInfoPayload01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-p --notify 0.1")
	{
	  PSPubNotify01 *P = new PSPubNotify01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --periodic 0.1")
	{
	  PSRunPeriodic01 *P = new PSRunPeriodic01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int PS::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int PS::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}



