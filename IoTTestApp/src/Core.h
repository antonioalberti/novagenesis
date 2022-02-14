/*
	NovaGenesis
	
	Name:		Application's core
	Object:		Core
	File:		Core.h
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
#define _CORE_H

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

#ifndef __OUTPUTVARIABLE_H
#include "OutputVariable.h"
#endif

#ifndef _SUBSCRIPTION_H
#include "Subscription.h"
#endif

#ifndef _PUBLICATION_H
#include "Publication.h"
#endif

#ifndef _DOCUMENT_H
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#endif

#ifndef _WRITER_H
#include "rapidjson/writer.h"        // for stringify JSON
#endif

#define ERROR 1
#define OK 0

using namespace std;

using namespace rapidjson;

class Core : public Block {
 private:

  // Gateway pointer
  GW *PGW;

  // HT pointer
  HT *PHT;

 public:

  // Auxiliary PSS/NRNCS tuples container
  vector<Tuple *> PSTuples;

  // Auxiliary peer tuples container. Stores peer PGCSes and EPGSes
  vector<Tuple *> PeerTuples;

  // Store the hash of available content
  vector<string> Content;

  // Store the keywords related to the application
  vector<string> Keywords;

  // Store the hash of the keywords
  vector<string> KeywordHashes;

  // Stores the category to be subscribed after a ng -notify -s command line among other things
  vector<Subscription *> Subscriptions;

  // Stores the ng -scn --seq and a timestamp for publishing messages
  vector<Publication *> Publications;

  // Stores the Keys successfully subscribed. Added during Carnival 2017
  vector<string> KeysOfReceivedPayloads;

  // Unique legible name
  string ULN;

  // Auxiliary flags
  bool GenerateStoreBindingsMsgCl01;
  bool GenerateStoreBindingsSCNSeq01;
  bool GenerateRunX01;
  bool GenerateRunXSCNSeq01;
  bool RunExpose;

  // Auxiliary counter
  unsigned int Counter;

  // Auxiliary counter for number of peer EPGSes
  int NoEPGSes;

  // Auxiliary timer
  double NextPeerEvaluationTime;

  // Debug file
  File Debug;

  // Constructor
  Core (string _LN, string _ULN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path);

  // Destructor
  ~Core ();

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
  // Subscription related functions
  // ------------------------------------------------------------------------------------------------------------------------------

  // Allocate and add a Subscription on Subscriptions container
  void NewSubscription (Subscription *&_PS);

  // Delete a Subscription
  int DeleteSubscription (Subscription *_PS);

  // Allocate and add a Publication on Publications container
  void NewPublication (Publication *&_PP);

  // Get a Publication
  int GetPublication (string _Key, Publication *&_PP);

  // Delete a Subscription
  int DeletePublication (Publication *_PP);

  // Delays
  double DelayBeforePublishingServiceOffer;
  double DelayBeforeDiscovery;
  double DelayBeforeRunPeriodic;
  double DelayBeforeRunPublishSSData;
  double DelayBeforeANewPeerEvaluation;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Statistic variables
  // ------------------------------------------------------------------------------------------------------------------------------
  OutputVariable *pubrtt;
  OutputVariable *subrtt;
  OutputVariable *tsmiup1; // Delay since instantiation at arriving on PGS up to the final -run --evaluate for message type 1 (seconds)

  // ------------------------------------------------------------------------------------------------------------------------------
  // Auxiliary functions
  // ------------------------------------------------------------------------------------------------------------------------------
  unsigned int GetSequenceNumber ();

  // Discover a 4-tuple inside a space defined the limiter: first step
  void
  DiscoveryFirstStep (string _Limiter, vector<string> *_Cat2Keywords, vector<string> *_Cat9Keywords, vector<Message *> ScheduledMessages);

  // Discover a 4-tuple inside a space defined the limiter: second step
  int
  DiscoverySecondStep (string _Limiter, vector<string> *_Cat2Keywords, vector<string> *_Cat9Keywords, vector<Message *> ScheduledMessages);

  int Exposition (string _Limiter, vector<Message *> ScheduledMessages);

  // Add a limiter on the ng -run --discovery command line
  void NewLimiterCommandLineArgument (string _Limiter, CommandLine *&_PCL);

  // Add a pair <category, key> on the ng -run --x command line
  void NewPairCommandLineArgument (unsigned int _Category, string _Key, CommandLine *&_PCL);

  // Add a terna <category, key, value> on the ng -run --x command line
  void NewTernaCommandLineArgument (unsigned int _Category, string _Key, string _Value, CommandLine *&_PCL);

  // Get Index of a Peer Application Tuple
  int GetPeerAppTupleIndex (string PID, unsigned int &_Index);

  // Get file content hash
  int GetFileContentHash (string FileName, string &_SCN);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Functions to customize the application
  // ------------------------------------------------------------------------------------------------------------------------------
  void SetKeywordsAndTheirHashes ();

  // ------------------------------------------------------------------------------------------------------------------------------
  // Friend classes
  // ------------------------------------------------------------------------------------------------------------------------------

  friend class CoreRunEvaluate01;
  friend class CoreRunInitialize01;
  friend class CoreRunExpose01;
  friend class CoreRunDiscover01;
  friend class CoreRunInvite01;
  friend class CoreRunSubscribe01;
  friend class CoreRunPublish01;
  friend class CoreRunPublish02;
  friend class CoreRunPublish03;
  friend class CoreMsgCl01;
  friend class CoreStatusS01;
  friend class CoreSCNAck01;
  friend class CoreSCNSeq01;
  friend class CoreDeliveryBind01;
  friend class CoreRunPeriodic01;
  friend class CoreRunPublishSSData01;
  friend class CoreRunGetInfo01;
  friend class CoreRunSetConfig01;
  friend class CoreNotifyS01;
};

#endif






