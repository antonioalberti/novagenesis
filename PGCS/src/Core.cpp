/*
	NovaGenesis

	Name:		PGCS Application's core
	Object:		Core
	File:		Core.cpp
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

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _PGS_H
#include "PGCS.h"
#endif

#ifndef _CORERUNEVALUTE01_H
#include "CoreRunEvaluate01.h"
#endif

#ifndef _CORERUNINITIALIZE01_H
#include "CoreRunInitialize01.h"
#endif

#ifndef _CORERUNEXPOSE01_H
#include "CoreRunExpose01.h"
#endif

#ifndef _CORERUNDISCOVER01_H
#include "CoreRunDiscover01.h"
#endif

#ifndef _CORERUNINVITE01_H
#include "CoreRunInvite01.h"
#endif

#ifndef _CORERUNPUBLISH01_H
#include "CoreRunPublish01.h"
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

#ifndef _CORENOTIFYS01_H
#include "CoreNotifyS01.h"
#endif

#ifndef _COREINFOPAYLOAD01_H
#include "CoreInfoPayload01.h"
#endif

#ifndef _CORERUNPUBLISH02_H
#include "CoreRunPublish02.h"
#endif

#ifndef _CORERUNPUBLISH03_H
#include "CoreRunPublish03.h"
#endif

Core::Core (string _LN, string _ULN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
	: Block (_LN, _PP, _Index, _Path)
{
  PGW = _PGW;
  PHT = _PHT;
  State = "initialization";
  ULN = _ULN;

  // Setting the delays
  DelayBeforePublishingServiceOffer = 1;
  DelayBeforeDiscovery = 1;
  DelayBeforeRunPeriodic = 15;
  DelayBeforeRunPublishSSData = 45;
  DelayBeforeANewPeerEvaluation = 1;
  DelayBeforeFirstPeriodic = 120;

  Message *PIM = 0;
  CommandLine *PCL = 0;
  Message *InlineResponseMessage = 0;
  Action *PA = 0;

  // Initialize auxiliary flags
  GenerateStoreBindingsMsgCl01 = true;
  GenerateStoreBindingsSCNSeq01 = false;
  GenerateRunX01 = true;
  GenerateRunXSCNSeq01 = true;
  RunExpose = true;

  // Auxiliary counter
  Counter = 0;

  // Auxiliary timer
  NextPeerEvaluationTime = 0;

  // More ingenious task actions
  NewAction ("-run --evaluate 0.1", PA);

  // Basic task actions
  NewAction ("-run --initialize 0.1", PA);
  NewAction ("-run --expose 0.1", PA);
  NewAction ("-run --discover 0.1", PA);
  NewAction ("-run --invite 0.1", PA);
  NewAction ("-run --publish 0.1", PA);
  NewAction ("-run --publish 0.2", PA);
  NewAction ("-run --publish 0.3", PA);
  NewAction ("-run --subscribe 0.1", PA);
  NewAction ("-run --periodic 0.1", PA);

  // Basic message related actions
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-st --s 0.1", PA);
  NewAction ("-scn --ack 0.1", PA);
  NewAction ("-scn --seq 0.1", PA);
  NewAction ("-d --b 0.1", PA);
  NewAction ("-notify --s 0.1", PA);
  NewAction ("-info --payload 0.1", PA);

  // Statistic variables
  pubrtt = new OutputVariable (this);
  subrtt = new OutputVariable (this);
  tsmiup1 = new OutputVariable (this);

  pubrtt
	  ->Initialization ("pubrtt", "MEAN_ARITHMETIC", "Binding publishing round trip time (seconds)", 1); // Sync reference is the time at the machine where the App process is running
  subrtt->Initialization ("subrtt", "MEAN_ARITHMETIC", "Binding subscribing round trip time (seconds)", 1);
  tsmiup1
	  ->Initialization ("tsmiup1", "MEAN_ARITHMETIC", "BDelay since instantiation at arriving on PGCS up to the final -run --evaluate for message type 1 (seconds)", 1);

  // Setup of the keywords and their hashes
  SetKeywordsAndTheirHashes ();

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 1, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialize", "0.1", PCL);

  // Execute the procedure
  Run (PIM, InlineResponseMessage);
}

Core::~Core ()
{
  delete pubrtt;
  delete subrtt;
  delete tsmiup1;

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

  for (it2 = PeerTuples.begin (); it2 != PeerTuples.end (); it2++)
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

  if (_LN == "-run --expose 0.1")
	{
	  CoreRunExpose01 *P = new CoreRunExpose01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --discover 0.1")
	{
	  CoreRunDiscover01 *P = new CoreRunDiscover01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --invite 0.1")
	{
	  CoreRunInvite01 *P = new CoreRunInvite01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --publish 0.1")
	{
	  CoreRunPublish01 *P = new CoreRunPublish01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --publish 0.2")
	{
	  CoreRunPublish02 *P = new CoreRunPublish02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
  if (_LN == "-run --publish 0.3")
	{
	  CoreRunPublish03 *P = new CoreRunPublish03 (_LN, this, PP->PMB);

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

  if (_LN == "-notify --s 0.1")
	{
	  CoreNotifyS01 *P = new CoreNotifyS01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-info --payload 0.1")
	{
	  CoreInfoPayload01 *P = new CoreInfoPayload01 (_LN, this, PP->PMB);

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
	  S << Offset << "(There is a scheduled message)" << endl;

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
	  S << Offset << "(There is a scheduled message)" << endl;

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

int Core::Exposition (string _Limiter, vector<Message *> ScheduledMessages)
{
  int Status = ERROR;
  string Offset = "                    ";
  Message *Run = 0;
  CommandLine *PCL = 0;
  string HashHint1;
  string HashHint2;
  string HashHint3;
  string HashProcessLegibleName;
  string HashBlockLegibleName;

  // *****************************************************************************************************************************************************************
  // ng -run --exposition 0.1 [ < 1 string Limiter > < 3 string Category1 Key1 Value1 >  < 3 string Category2 Key2 Value2 > ... < 3 string CategoryN KeyN ValueN> ]
  // *****************************************************************************************************************************************************************
  if (ScheduledMessages.size () > 0)
	{
	  S << Offset << "(There is a scheduled message)" << endl;

	  Run = ScheduledMessages.at (0);

	  if (Run != 0)
		{
		  // Adding a ng -run --discovery command line
		  Run->NewCommandLine ("-run", "--expose", "0.1", PCL);

		  NewLimiterCommandLineArgument (_Limiter, PCL);

		  if (Keywords.size () == KeywordHashes.size () && Keywords.size () > 1)
			{
			  for (int i = 0; i < Keywords.size (); i++)
				{
				  // Publish binding < Keyword, Hash(Keyword) >
				  NewTernaCommandLineArgument (2, KeywordHashes[i], PP->GetSelfCertifyingName (), PCL);

				  // Publish binding < Hash(Keyword), Keyword >
				  NewTernaCommandLineArgument (1, KeywordHashes[i], Keywords[i], PCL);
				}

			  // Publish binding < Hash("RMS"), App PID >
			  NewTernaCommandLineArgument (2, KeywordHashes[0], PP->GetSelfCertifyingName (), PCL);

			  // Publish binding < Hash("Core"), Core BID >
			  NewTernaCommandLineArgument (2, KeywordHashes[1], GetSelfCertifyingName (), PCL);
			}

		  // Publish binding < App PID, App::Core BID >
		  NewTernaCommandLineArgument (5, PP->GetSelfCertifyingName (), GetSelfCertifyingName (), PCL);

		  // Publish binding < This OSID, App PID >
		  NewTernaCommandLineArgument (5, PP->GetOperatingSystemSelfCertifyingName (), PP
			  ->GetSelfCertifyingName (), PCL);

		  // Publish binding < This HID, This OSID >
		  NewTernaCommandLineArgument (6, PP->GetHostSelfCertifyingName (), PP
			  ->GetOperatingSystemSelfCertifyingName (), PCL);
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

// Get Index of a Peer Application Tuple
int Core::GetPeerAppTupleIndex (string PID, unsigned int &_Index)
{
  int Status = ERROR;

  _Index = 0;

  for (unsigned int k = 0; k < PeerTuples.size (); k++)
	{
	  Tuple *PT = PeerTuples[k];

	  if (PT->Values[2] == PID)
		{
		  _Index = k;

		  Status = OK;

		  break;
		}
	}

  return Status;
}

// Get file content hash
int Core::GetFileContentHash (string FileName, string &_SCN)
{
  int Status = ERROR;
  File F1;
  long long PayloadSize;
  string PayloadString;
  string PayloadHash;
  string Offset = "                              ";

  F1.OpenInputFile (FileName, GetPath (), "BINARY");

  F1.seekg (0, ios::end);

  // Read the number of characters in the file.
  PayloadSize = F1.tellg ();

  //S << Offset <<  "(The payload size is = "<<PayloadSize<<")"<<endl;

  if (PayloadSize > 0)
	{
	  char *Payload = new char[PayloadSize];

	  F1.seekg (0);

	  F1.read (Payload, PayloadSize);

	  GenerateSCNFromCharArrayBinaryPatterns (Payload, PayloadSize, _SCN);

	  delete[] Payload;

	  Status = OK;
	}

  F1.CloseFile ();

  return Status;
}

// ------------------------------------------------------------------------------------------------------------------------------
// Functions to customize the application
// ------------------------------------------------------------------------------------------------------------------------------
void Core::SetKeywordsAndTheirHashes ()
{
  // ***************************************************************
  // Set the keywords that will be used to found out this app
  // ***************************************************************

  Keywords.push_back ("PGCS");    // The process legible name must the first name on the vector
  Keywords.push_back ("Core");        // The core block legible name must the first name on the vector
  Keywords.push_back ("Gateway");
  Keywords.push_back ("Proxy");
  Keywords.push_back ("Controller");
  Keywords.push_back ("IoT");
  Keywords.push_back ("Wi-Fi");
  Keywords.push_back ("Agent");
  Keywords.push_back (ULN);

  for (int i = 0; i < Keywords.size (); i++)
	{
	  string Hash;

	  GenerateSCNFromCharArrayBinaryPatterns (Keywords[i], Hash);

	  KeywordHashes.push_back (Hash);
	}
}

