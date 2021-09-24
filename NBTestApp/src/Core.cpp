/*
	NovaGenesis

	Name:		Application's core
	Object:		Core
	File:		Core.cpp
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

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _APP_H
#include "NBTestApp.h"
#endif

#ifndef _CORERUNEVALUTE01_H
#include "CoreRunEvaluate01.h"
#endif

#ifndef _CORERUNINITIALIZE01_H
#include "CoreRunInitialize01.h"
#endif

#ifndef _CORERUNDISCOVER01_H
#include "CoreRunDiscover01.h"
#endif

#ifndef _CORERUNSUBSCRIBE01_H
#include "CoreRunSubscribe01.h"
#endif

#ifndef _CORERUNPERIODIC01_H
#include "CoreRunPeriodic01.h"
#endif

#ifndef _COREMSGCL01_H
#include "CoreMsgCl01.h"
#endif

#ifndef _CORESTATUSS01_H
#include "CoreStatusS01.h"
#endif

#ifndef _CORESNCNACK01_H
#include "CoreSCNAck01.h"
#endif

#ifndef _CORESNCNSEQ01_H
#include "CoreSCNSeq01.h"
#endif

#ifndef _COREDELIVERYBIND01_H
#include "CoreDeliveryBind01.h"
#endif

Core::Core (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
	: Block (_LN, _PP, _Index, _Path)
{
  PGW = _PGW;
  PHT = _PHT;
  State = "initialization";

  // Initialize auxiliary flags
  GenerateStoreBindingsMsgCl01 = true;
  GenerateRunX01 = true;
  GenerateStoreBindingsSCNSeq01 = false;
  GenerateRunXSCNSeq01 = true;
  ScheduledMessagesCreation = true;

  // Setting the delays
  DelayBeforeDiscovery = 5;
  DelayBeforeRunPeriodic = 10;
  NumberOfPublications = 1000;
  NumberOfSubscriptions = 100;
  NumberOfMessagesPerBurst = 100;
  NumberOfPubsPerMessage = 100;

  Message *PIM = 0;
  CommandLine *PCL = 0;
  Message *InlineResponseMessage = 0;
  Action *PA = 0;

  // More ingenious task actions
  NewAction ("-run --evaluate 0.1", PA);

  // Basic task actions
  NewAction ("-run --initialize 0.1", PA);
  NewAction ("-run --discover 0.1", PA);
  NewAction ("-run --subscribe 0.1", PA);
  NewAction ("-run --periodic 0.1", PA);

  // Basic message related actions
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-st --s 0.1", PA);
  NewAction ("-scn --ack 0.1", PA);
  NewAction ("-scn --seq 0.1", PA);
  NewAction ("-d --b 0.1", PA);

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialize", "0.1", PCL);

  // Run
  Run (PIM, InlineResponseMessage);

  // Mark to delete
  PIM->MarkToDelete ();
}

Core::~Core ()
{
  delete pubrtt;
  delete subrtt;
  //delete tsmiup1;

  Tuple *Temp = 0;

  vector<Tuple *>::iterator it1;

  for (it1 = PSTuples.begin (); it1 != PSTuples.end (); it1++)
	{
	  Temp = *it1;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}

  vector<Tuple *>::iterator it2;

  vector<Action *>::iterator it4;

  Action *Temp2 = 0;

  for (it4 = Actions.begin (); it4 != Actions.end (); it4++)
	{
	  Temp2 = *it4;

	  if (Temp != 0)
		{
		  delete Temp2;
		}

	  Temp2 = 0;
	}
}

// Allocate and add an Action on Actions container
void Core::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-run --evaluate 0.1")
	{
	  CoreRunEvaluate01 *P = new CoreRunEvaluate01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --initialize 0.1")
	{
	  CoreRunInitialize01 *P = new CoreRunInitialize01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --discover 0.1")
	{
	  CoreRunDiscover01 *P = new CoreRunDiscover01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --subscribe 0.1")
	{
	  CoreRunSubscribe01 *P = new CoreRunSubscribe01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --periodic 0.1")
	{
	  CoreRunPeriodic01 *P = new CoreRunPeriodic01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-m --cl 0.1")
	{
	  CoreMsgCl01 *P = new CoreMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-st --s 0.1")
	{
	  CoreStatusS01 *P = new CoreStatusS01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --ack 0.1")
	{
	  CoreSCNAck01 *P = new CoreSCNAck01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-d --b 0.1")
	{
	  CoreDeliveryBind01 *P = new CoreDeliveryBind01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --seq 0.1")
	{
	  CoreSCNSeq01 *P = new CoreSCNSeq01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int Core::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int Core::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}

// Allocate and add a Subscription on Subscriptions container
void Core::NewSubscription (Subscription *&_PS)
{
  Subscription *PS = new Subscription;

  Subscriptions.push_back (PS);

  _PS = PS;
}

// Delete a Subscription
int Core::DeleteSubscription (Subscription *_PS)
{
  Subscription *PS = 0;
  vector<Subscription *>::iterator it;
  int i = 0;
  int Status = ERROR;

  for (it = Subscriptions.begin (); it != Subscriptions.end (); it++)
	{
	  PS = Subscriptions[i];

	  if (PS == _PS)
		{
		  Subscriptions.erase (it);

		  delete PS;

		  Status = OK;

		  break;
		}

	  i++;
	}

  return Status;
}

// Allocate and add a Publication on Publications container
void Core::NewPublication (Publication *&_PP)
{
  Publication *PP = new Publication;

  Publications.push_back (PP);

  _PP = PP;
}

// Get a Publication
int Core::GetPublication (string _Key, Publication *&_PP)
{
  Publication *PP = 0;
  vector<Publication *>::iterator it;
  int i = 0;
  int Status = ERROR;

  for (it = Publications.begin (); it != Publications.end (); it++)
	{
	  PP = Publications[i];

	  if (PP->Key == _Key)
		{
		  _PP = PP;

		  Status = OK;

		  break;
		}

	  i++;
	}

  return Status;
}

// Delete a Subscription
int Core::DeletePublication (Publication *_PP)
{
  Publication *PP = 0;
  vector<Publication *>::iterator it;
  int i = 0;
  int Status = ERROR;

  for (it = Publications.begin (); it != Publications.end (); it++)
	{
	  PP = Publications[i];

	  if (PP == _PP)
		{
		  Publications.erase (it);

		  delete PP;

		  Status = OK;

		  break;
		}

	  i++;
	}

  return Status;
}

unsigned int Core::GetSequenceNumber ()
{
  Counter++;

  return Counter;
}

// Discover a 4-tuple on PGCS: first step
void
Core::DiscoveryFirstStep (string _Limiter, vector<string> *_Cat2Keywords, vector<string> *_Cat9Keywords, vector<Message *> ScheduledMessages)
{
  string Offset = "                    ";
  Message *Run = 0;
  CommandLine *PCL = 0;
  string Hash;

  // **********************************************************************************************************************************************************
  // Creates the ng -run --discovery 0.1 [ < 1 string Limiter > < 2 string Category1 Key1 >  < 2 string Category2 Key2 > ... < 2 string CategoryN KeyN > ]
  // **********************************************************************************************************************************************************
  if (ScheduledMessages.size () > 0)
	{

#ifdef DEBUG
	  S << Offset <<  "(There is a scheduled message)" << endl;
#endif

	  Run = ScheduledMessages.at (0);

	  if (Run != 0)
		{
		  // Adding a ng -run --discovery command line
		  Run->NewCommandLine ("-run", "--discover", "0.1", PCL);

		  // Set the time to run
		  Run->SetTime (GetTime () + DelayBeforeDiscovery);

		  NewLimiterCommandLineArgument (_Limiter, PCL);

		  if (_Cat2Keywords != 0)
			{
			  for (unsigned int i = 0; i < _Cat2Keywords->size (); i++)
				{
				  GenerateSCNFromCharArrayBinaryPatterns (_Cat2Keywords->at (i), Hash);

				  NewPairCommandLineArgument (2, Hash, PCL);

				  Hash = "";
				}
			}

		  if (_Cat9Keywords != 0)
			{
			  for (unsigned int j = 0; j < _Cat9Keywords->size (); j++)
				{
				  GenerateSCNFromCharArrayBinaryPatterns (_Cat9Keywords->at (j), Hash);

				  NewPairCommandLineArgument (9, Hash, PCL);

				  Hash = "";
				}
			}
		}
	}
}

// Discover a 4-tuple on PGCS: second step
int
Core::DiscoverySecondStep (string _Limiter, vector<string> *_Cat2Keywords, vector<string> *_Cat9Keywords, vector<Message *> ScheduledMessages)
{
  int Status = ERROR;
  string Offset = "                    ";
  Message *Run = 0;
  CommandLine *PCL = 0;
  vector<string> *HIDs = new vector<string>;
  vector<string> *ADIs = new vector<string>;

  // **********************************************************************************************************************************************************
  // Creates the ng -run --discovery 0.1 [ < 1 string Limiter > < 2 string Category1 Key1 >  < 2 string Category2 Key2 > ... < 2 string CategoryN KeyN > ]
  // **********************************************************************************************************************************************************
  if (ScheduledMessages.size () > 0)
	{

#ifdef DEBUG
	  S << Offset <<  "(There is a scheduled message)" << endl;
#endif

	  Run = ScheduledMessages.at (0);

	  if (Run != 0)
		{
		  // Adding a ng -run --discovery command line
		  Run->NewCommandLine ("-run", "--discover", "0.1", PCL);

		  // Set the time to run
		  Run->SetTime (GetTime () + DelayBeforeDiscovery);

		  NewLimiterCommandLineArgument (_Limiter, PCL);

		  if (_Cat9Keywords != 0)
			{
			  for (unsigned int m = 0; m < _Cat9Keywords->size (); m++)
				{
				  if (PP->DiscoverHomonymsEntitiesIDsFromLN (9, _Cat9Keywords->at (m), HIDs, this) == OK)
					{
					  for (unsigned int i = 0; i < HIDs->size (); i++)
						{
						  NewPairCommandLineArgument (6, HIDs->at (i), PCL);

						  Status = OK;
						}
					}
				}
			}

		  if (_Cat2Keywords != 0)
			{
			  for (unsigned int n = 0; n < _Cat2Keywords->size (); n++)
				{
				  if (PP->DiscoverHomonymsEntitiesIDsFromLN (2, _Cat2Keywords->at (n), ADIs, this) == OK)
					{
					  for (unsigned int i = 0; i < ADIs->size (); i++)
						{
						  NewPairCommandLineArgument (5, ADIs->at (i), PCL);

						  Status = OK;
						}
					}
				}
			}
		}
	  else
		{
		  S << Offset << "(ERROR: Unable to obtain the scheduled message at index zero)" << endl;
		}
	}
  else
	{
	  S << Offset << "(ERROR: Unable to obtain the size of the scheduled messages vector)" << endl;
	}

  delete HIDs;
  delete ADIs;

  return Status;
}

// Add a limiter on the ng -run --x command line
void Core::NewLimiterCommandLineArgument (string _Limiter, CommandLine *&_PCL)
{
  _PCL->NewArgument (1);

  _PCL->SetArgumentElement (0, 0, _Limiter);
}

// Add a pair <category, key> on the ng -run --x command line
void Core::NewPairCommandLineArgument (unsigned int _Category, string _Key, CommandLine *&_PCL)
{
  unsigned int Number = 0;

  _PCL->NewArgument (2);

  _PCL->GetNumberofArguments (Number);

  _PCL->SetArgumentElement ((Number - 1), 0, IntToString (_Category));

  _PCL->SetArgumentElement ((Number - 1), 1, _Key);
}

// Add a terna <category, key, value> on the ng -run --x command line
void Core::NewTernaCommandLineArgument (unsigned int _Category, string _Key, string _Value, CommandLine *&_PCL)
{
  unsigned int Number = 0;

  _PCL->NewArgument (3);

  _PCL->GetNumberofArguments (Number);

  _PCL->SetArgumentElement ((Number - 1), 0, IntToString (_Category));

  _PCL->SetArgumentElement ((Number - 1), 1, _Key);

  _PCL->SetArgumentElement ((Number - 1), 2, _Value);
}



