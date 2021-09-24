/*
	NovaGenesis

	Name:		Hash Table
	Object:		HT
	File:		HT.cpp
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
#include "HT.h"
#endif

#ifndef _HTMSGCL01_H
#include "HTMsgCl01.h"
#endif

#ifndef _HTSTOREBIND01_H
#include "HTStoreBind01.h"
#endif

#ifndef _HTDELIVERYBIND01_H
#include "HTDeliveryBind01.h"
#endif

#ifndef _HTGETBIND01_H
#include "HTGetBind01.h"
#endif

#ifndef _HTREVOKEBIND01_H
#include "HTRevokeBind01.h"
#endif

#ifndef _HTRUNINITIALIZATION01_H
#include "HTRunInitialization01.h"
#endif

#ifndef _HTLISTBIND01_H
#include "HTListBind01.h"
#endif

#ifndef _HTBINDREPORT01_H
#include "HTBindReport01.h"
#endif

#ifndef _HTSTATUSS01_H
#include "HTStatusS01.h"
#endif

#ifndef _HTSCNREQ01_H
#include "HTSCNSeq01.h"
#endif

#ifndef _HTINFOPAYLOAD01_H
#include "HTInfoPayload01.h"
#endif

#ifndef _HTMESSAGETYPE01_H
#include "HTMessageType01.h"
#endif

#ifndef _HTMESSAGESEQ01_H
#include "HTMessageSeq01.h"
#endif

#ifndef _HTRUNPERIODIC01_H
#include "HTRunPeriodic01.h"
#endif

HT::HT (string _LN, Process *_PP, unsigned int _Index, string _Path) : Block (_LN, _PP, _Index, _Path)
{
  Action *PA = NULL;
  int Category;
  string Key;
  string Value;
  vector<string> Values;
  string PID = "";
  string BID = "";
  string MyBlockIndexString = "";
  string ProcessLegibleNameSCN = "";
  stringstream ss;
  unsigned int Index = 0;
  string Offset = "                    ";
  string HashBlockLegibleName;

  HaveOneStatusCL = false;

  // Setting the delays
  DelayBeforeRunPeriodic = 60;

  State = "initialization";

  Bindings = new HashStringMultimap[MAX_CATEGORIES];

  // Creating the actions
  NewAction ("-run --initialization 0.1", PA);
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-sr --b 0.1", PA);
  NewAction ("-d --b 0.1", PA);
  NewAction ("-g --b 0.1", PA);
  NewAction ("-rvk --b 0.1", PA);
  NewAction ("-list --b 0.1", PA);
  NewAction ("-bind --report 0.1", PA);
  NewAction ("-st --s 0.1", PA);
  NewAction ("-scn --seq 0.1", PA);
  NewAction ("-info --payload 0.1", PA);
  NewAction ("-message --type 0.1", PA);
  NewAction ("-message --seq 0.1", PA);
  NewAction ("-run --periodic 0.1", PA);

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // ******************************************************
  // Calculate auxiliary variables and required input data
  // ******************************************************

  PID = PP->GetSelfCertifyingName ();

  BID = GetSelfCertifyingName ();

  GenerateSCNFromCharArrayBinaryPatterns ("Process", ProcessLegibleNameSCN);

  //cout << "ProcessLegibleNameSCN = "<< ProcessLegibleNameSCN << endl;

  ss << Index;

  MyBlockIndexString = ss.str ();

  //cout << "MyBlockIndexString = "<< MyBlockIndexString << endl;

  GenerateSCNFromCharArrayBinaryPatterns (GetLegibleName (), HashBlockLegibleName);

  S << Offset << "(Hash\"HT\" = " << HashBlockLegibleName << ")" << endl;

  if (PID != "" && BID != "" && ProcessLegibleNameSCN != "" && MyBlockIndexString != "")
	{
	  cout << Offset << "(" << GetLegibleName () << " is moving to operational state)" << endl;

	  // Setting up the category
	  Category = 0;

	  // Setting up the binding key
	  Key = "HT";

	  // Setting up the binding value
	  Values.push_back (HashBlockLegibleName);

	  // Creating the ng -sr --b 0.1 command line
	  StoreBinding (Category, Key, &Values);

	  // Clearing the values container
	  Values.clear ();

	  // ******************************************************
	  // Binding Hash("HT") to "HT"
	  // ******************************************************

	  // Setting up the category
	  Category = 1;

	  // Setting up the binding key
	  Key = HashBlockLegibleName;

	  // Setting up the binding value
	  Values.push_back ("HT");

	  // Creating the ng -sr --b 0.1 command line
	  StoreBinding (Category, Key, &Values);

	  // Clearing the values container
	  Values.clear ();

	  // ******************************************************
	  // Binding Hash("HT") to BID
	  // ******************************************************

	  // Setting up the category
	  Category = 2;

	  // Setting up the binding key
	  Key = HashBlockLegibleName;

	  // Setting up the binding value
	  Values.push_back (BID);

	  // Creating the ng -sr --b 0.1 command line
	  StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding BID to Hash("HT")
	  // ******************************************************

	  // Setting up the category
	  Category = 3;

	  // Setting up the binding key
	  Key = BID;

	  // Setting up the binding value
	  Values.push_back (HashBlockLegibleName);

	  // Creating the ng -sr --b 0.1 command line
	  StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding BID to HT index on Blocks container
	  // ******************************************************

	  // Setting up the category
	  Category = 13;

	  // Setting up the binding key
	  Key = BID;

	  // Setting up the value
	  Values.push_back (MyBlockIndexString);

	  // Store the binding
	  StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding Hash("Process") to "Process"
	  // ******************************************************

	  // Setting up the category
	  Category = 1;

	  // Setting up the binding key
	  Key = ProcessLegibleNameSCN;

	  // Setting up the value
	  Values.push_back ("Process");

	  StoreBinding (Category, Key, &Values);

	  Values.clear ();

	  // ******************************************************
	  // Binding PID to Hash("Process")
	  // ******************************************************

	  // Setting up the category
	  Category = 3;

	  // Setting up the binding key
	  Key = PID;

	  // Setting up the value
	  Values.push_back (ProcessLegibleNameSCN);

	  // Store the binding
	  StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding Hash("Process") to Process SCN
	  // ******************************************************

	  // Setting up the category
	  Category = 2;

	  // Setting up the binding key
	  Key = ProcessLegibleNameSCN;

	  // Setting up the value
	  Values.push_back (PID);

	  // Store the binding
	  StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding PID to BID
	  // ******************************************************

	  // Setting up the category
	  Category = 5;

	  // Setting up the binding key
	  Key = PID;

	  // Setting up the value
	  Values.push_back (BID);

	  // Store the binding
	  StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  State = "operational";

	  S << Offset << "(Done)" << endl << endl << endl;
	}
  else
	{
	  S << Offset << "(ERROR: Unable to store initial bindings in the HT data structure)" << endl << endl << endl;
	}
}

HT::~HT ()
{
  S << "Deleting " << GetLegibleName () << ". The hash tables size are respectively: ";

  for (unsigned int j = 0; j < MAX_CATEGORIES; j++)
	{
	  S << " " << j << "=" << Bindings[j].size () << ".";
	}

  S << endl;

  for (unsigned int k = 0; k < MAX_CATEGORIES; k++)
	{
	  Bindings[k].clear ();
	}

  delete[] Bindings;

  vector<Action *>::iterator it4;

  Action *Temp;

  for (it4 = Actions.begin (); it4 != Actions.end (); it4++)
	{
	  Temp = *it4;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}
}

// Allocate and add an Action on Actions container
void HT::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-run --initialization 0.1")
	{
	  HTRunInitialization01 *P = new HTRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-m --cl 0.1")
	{
	  HTMsgCl01 *P = new HTMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-sr --b 0.1")
	{
	  HTStoreBind01 *P = new HTStoreBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-d --b 0.1")
	{
	  HTDeliveryBind01 *P = new HTDeliveryBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-st --s 0.1")
	{
	  HTStatusS01 *P = new HTStatusS01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-g --b 0.1")
	{
	  HTGetBind01 *P = new HTGetBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-rvk --b 0.1")
	{
	  HTRevokeBind01 *P = new HTRevokeBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-list --b 0.1")
	{
	  HTListBind01 *P = new HTListBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-bind --report 0.1")
	{
	  HTBindReport01 *P = new HTBindReport01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --seq 0.1")
	{
	  HTSCNSeq01 *P = new HTSCNSeq01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-info --payload 0.1")
	{
	  HTInfoPayload01 *P = new HTInfoPayload01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-message --type 0.1")
	{
	  HTMessageType01 *P = new HTMessageType01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-message --seq 0.1")
	{
	  HTMessageSeq01 *P = new HTMessageSeq01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --periodic 0.1")
	{
	  HTRunPeriodic01 *P = new HTRunPeriodic01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int HT::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int HT::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}

// Store a binding in the Bindings container
int HT::StoreBinding (unsigned int _Cat, const string _Key, vector<string> *_Values)
{
  int Status = OK;
  string Offset = "                    ";
  vector<string> *AlreadyStoredValues = new vector<string>;
  bool StoreIt[_Values->size ()];

  GetBinding (_Cat, _Key, AlreadyStoredValues);

  for (unsigned int i = 0; i < _Values->size (); i++)
	{
	  if (AlreadyStoredValues->size () > 0)
		{
		  for (unsigned int j = 0; j < AlreadyStoredValues->size (); j++)
			{
			  if (AlreadyStoredValues->at (j) == _Values->at (i))
				{
				  StoreIt[i] = false;

				  break;
				}
			  else
				{
				  StoreIt[i] = true;
				}
			}
		}
	  else
		{
		  StoreIt[i] = true;
		}
	}

  for (unsigned int k = 0; k < _Values->size (); k++)
	{
	  if (StoreIt[k] == true)
		{
		  Bindings[_Cat].insert (HashStringMultimap::value_type (_Key, _Values->at (k)));

		  //S << Offset << "(Cat = "<<_Cat<<", Pair = [ < "<< _Key << " " << _Values->at(k) <<" > ])" << endl;
		}
	  else
		{
		  //S << Offset <<  "(Warning: The value "<<_Values->at(k)<<" at index "<<k<<" is already stored under the key "<<_Key<<" )" << endl;
		}
	}

  AlreadyStoredValues->clear ();

  delete AlreadyStoredValues;

  return Status;
}

// Get a binding from the Bindings container
int HT::GetBinding (unsigned int _Cat, const string _Key, vector<string> *&_Values)
{
  int Status = ERROR;
  string Offset = "                    ";

  string E;

  //S << Offset << "(Cat = " << _Cat << ", Key = " << _Key << ")" << endl;

  if (Bindings != 0)
	{
	  IteratorsPair itp = Bindings[_Cat].equal_range (_Key);

	  Iterator it;

	  for (it = itp.first; it != itp.second; ++it)
		{
		  E = (*it).second;

		  if (E != "")
			{
			  _Values->push_back (E);

			  Status = OK;

			  //S << Offset << "(Found a match in the table)" << endl;
			}
		}
	}
  else
	{
	  S << "(ERROR: Unable to access the required hash table)" << endl;
	}

  return Status;
}

// Revoke a binding from the Bindings container
int HT::RevokeBinding (unsigned int _Cat, const string _Key)
{
  int Status = ERROR;
  string Offset = "                    ";

  string E;

  S << Offset << "(Revoking a binding with Cat = " << _Cat << ", Key = " << _Key << ")" << endl;

  if (Bindings != 0)
	{
	  IteratorsPair itp = Bindings[_Cat].equal_range (_Key);

	  vector<string> *_Values = new vector<string>;

	  // Get the binding from the hash_multimap container
	  GetBinding (_Cat, _Key, _Values);

	  if (_Cat == 18)
		{
		  if (_Values != 0)
			{
			  if (_Values->size () > 0)
				{
				  std::remove ((GetPath () + _Values->at (0)).c_str ());
				}
			  else
				{
				  S << Offset << "(ERROR: The value under the key " << _Key << " is not here. Look somewhere else.)"
					<< endl;
				}
			}
		  else
			{
			  S << Offset << "(ERROR: Unable to get the value stored under the informed key)" << endl;
			}
		}

	  // Delete temporary values
	  _Values->clear ();

	  delete _Values;

	  Bindings[_Cat].erase (itp.first, itp.second);
	}
  else
	{
	  S << "(ERROR: Unable to access the required hash table)" << endl;
	}

  return Status;
}

