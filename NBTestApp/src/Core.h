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

#define ERROR 1
#define OK 0

using namespace std;

class Core : public Block {
 private:

  // Gateway pointer
  GW *PGW;

  // HT pointer
  HT *PHT;

 public:

  // Auxiliary NRNCS tuples container
  vector<Tuple *> PSTuples;

  // Stores the category to be subscribed after a ng -notify -s command line among other things
  vector<Subscription *> Subscriptions;

  // Stores the ng -scn --seq and a timestamp for publishing messages
  vector<Publication *> Publications;

  // Constructor
  Core (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path);

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

  // Auxiliary flags
  bool GenerateStoreBindingsMsgCl01;
  bool GenerateRunX01;
  bool GenerateStoreBindingsSCNSeq01;
  bool GenerateRunXSCNSeq01;
  bool ScheduledMessagesCreation;

  // Auxiliary counter
  unsigned int Counter;

  // Delays/Bursts
  double DelayBeforeDiscovery;
  double DelayBeforeRunPeriodic;
  int NumberOfPublications;
  int NumberOfSubscriptions;
  int NumberOfMessagesPerBurst;
  int NumberOfPubsPerMessage;

  // Output naming variables
  string NumberOfHTSs;
  string Trial;

  // ------------------------------------------------------------------------------------------------------------------------------
  // Statistic variables
  // ------------------------------------------------------------------------------------------------------------------------------
  OutputVariable *pubrtt;
  OutputVariable *subrtt;
  //OutputVariable	*tsmiup1; // Delay since instantiation at arriving on PGS up to the final -run --evaluate for message type 1 (seconds)

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

  // Add a limiter on the ng -run --discovery command line
  void NewLimiterCommandLineArgument (string _Limiter, CommandLine *&_PCL);

  // Add a pair <category, key> on the ng -run --x command line
  void NewPairCommandLineArgument (unsigned int _Category, string _Key, CommandLine *&_PCL);

  // Add a terna <category, key, value> on the ng -run --x command line
  void NewTernaCommandLineArgument (unsigned int _Category, string _Key, string _Value, CommandLine *&_PCL);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Friend classes
  // ------------------------------------------------------------------------------------------------------------------------------

  friend class CoreRunEvaluate01;
  friend class CoreRunInitialize01;
  friend class CoreRunSubscribe01;
  friend class CoreRunPublish01;
  friend class CoreMsgCl01;
  friend class CoreStatusS01;
  friend class CoreSCNAck01;
  friend class CoreSCNSeq01;
  friend class CoreRunPeriodic01;
  friend class CoreDeliveryBind01;
  friend class CoreRunDiscover01;
  friend class CoreNotifyS01;
};

#endif






