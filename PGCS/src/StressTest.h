/*
	NovaGenesis

	Name:		StressTest counter block for PGCS
	Object:		StressTest
	File:		StressTest.h
	Version:	0.1
*/

#ifndef _STRESSTEST_H
#define _STRESSTEST_H

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#define ERROR 1
#define OK 0

using namespace std;

// ============================================================================
// StressTestPing01 — counts incoming stress-test messages
// ============================================================================
class StressTestPing01 : public Action {
 public:
  StressTestPing01 (string _LN, Block *_PB, MessageBuilder *_PMB);
  ~StressTestPing01 ();
  int Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage);
};

// ============================================================================
// StressTest — message counter block
// ============================================================================
class StressTest : public Block {
 private:
  GW *PGW;
  HT *PHT;

  unsigned long long Sent;
  unsigned long long Received;
  unsigned long long Dropped;

  string MyHostSCN;

 public:
  File Stats;
  bool Enabled;

  StressTest (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path);
  ~StressTest ();

  void NewAction (const string _LN, Action *&_PA);
  int  GetAction (string _LN, Action *&_PA);
  int  DeleteAction (string _LN);

  unsigned long long GetSent ()     { return Sent; }
  unsigned long long GetReceived () { return Received; }
  unsigned long long GetDropped ()  { return Dropped; }
  void IncSent ()     { Sent++; }
  void IncReceived () { Received++; }
  void IncDropped ()  { Dropped++; }
};

#endif