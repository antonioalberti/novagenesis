/*
	NovaGenesis

	Name:		Distributed Hash Table
	Object:		DHT
	File:		DHT.cpp
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
#include "DHT.h"
#endif

#ifndef _DHTRUNINITIALIZATION01_H
#include "DHTRunInitialization01.h"
#endif

#ifndef _DHTMSGCL01_H
#include "DHTMsgCl01.h"
#endif

#ifndef _DHTDELIVERYBIND01_H
#include "DHTDeliveryBind01.h"
#endif

#ifndef _DHTRUNPERIODIC01_H
#include "DHTRunPeriodic01.h"
#endif

DHT::DHT (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
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
  DelayBeforeGIRSPSSDiscovery = 40;
  DelayBeforePIDPublishing = 20;
  DelayBeforeRunExposition = 2;

  GenerateGetBindHeader = true;
  GenerateSCNSeqForStoreBind = false;
  GenerateSCNSeqForGetBind = false;
  ChangeState = true;
  AuxCounter = 0;

  // Creating the actions
  NewAction ("-run --initialization 0.1", PA);
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-st --s 0.1", PA);
  NewAction ("-scn --ack 0.1", PA);
  NewAction ("-d --b 0.1", PA);
  NewAction ("-run --periodic 0.1", PA);

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialization", "0.1", PCL);

  // Execute the procedure
  Run (PIM, InlineResponseMessage);
}

DHT::~DHT ()
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

  for (it2 = GIRSTuples.begin (); it2 != GIRSTuples.end (); it2++)
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
		  delete Temp;
		}

	  Temp2 = 0;
	}
}

// Allocate and add an Action on Actions container
void DHT::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-m --cl 0.1")
	{
	  DHTMsgCl01 *P = new DHTMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --initialization 0.1")
	{
	  DHTRunInitialization01 *P = new DHTRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-d --b 0.1")
	{
	  DHTDeliveryBind01 *P = new DHTDeliveryBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --periodic 0.1")
	{
	  DHTRunPeriodic01 *P = new DHTRunPeriodic01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int DHT::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int DHT::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}



