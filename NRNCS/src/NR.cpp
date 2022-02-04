/*
	NovaGenesis

	Name:		Publish/Subscribe
	Object:		NR
	File:		NR.cpp
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

#ifndef _NR_H
#include "NR.h"
#endif

#ifndef _NRRUNINITIALIZATION01_H
#include "NRRunInitialization01.h"
#endif

#ifndef _NRMSGCL01_H
#include "NRMsgCl01.h"
#endif

#ifndef _NRDELIVERYBIND01_H
#include "NRDeliveryBind01.h"
#endif

#ifndef _NRPUBBIND01_H
#include "NRPubBind01.h"
#endif

#ifndef _NRSCNSEQ01_H
#include "NRSCNSeq01.h"
#endif

#ifndef _NRSUBBIND01_H
#include "NRSubBind01.h"
#endif

#ifndef _NRREVOKEBIND01_H
#include "NRRevokeBind01.h"
#endif

#ifndef _NRINFOPAYLOAD01_H
#include "NRInfoPayload01.h"
#endif

#ifndef _NRPUBNOTIFY01_H
#include "NRPubNotify01.h"
#endif

#ifndef _NRRUNPERIODIC01_H
#include "NRRunPeriodic01.h"
#endif

NR::NR (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
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

NR::~NR ()
{
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
void NR::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-m --cl 0.1")
	{
	  NRMsgCl01 *P = new NRMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --initialization 0.1")
	{
	  NRRunInitialization01 *P = new NRRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-d --b 0.1")
	{
	  NRDeliveryBind01 *P = new NRDeliveryBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-p --b 0.1")
	{
	  NRPubBind01 *P = new NRPubBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --seq 0.1")
	{
	  NRSCNSeq01 *P = new NRSCNSeq01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-s --b 0.1")
	{
	  NRSubBind01 *P = new NRSubBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-rvk --b 0.1")
	{
	  NRRevokeBind01 *P = new NRRevokeBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-info --payload 0.1")
	{
	  NRInfoPayload01 *P = new NRInfoPayload01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-p --notify 0.1")
	{
	  NRPubNotify01 *P = new NRPubNotify01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --periodic 0.1")
	{
	  NRRunPeriodic01 *P = new NRRunPeriodic01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int NR::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int NR::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}