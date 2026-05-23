/*
	NovaGenesis

	Name:		StressTest counter block
	Object:		StressTest
	File:		StressTest.cpp
	Version:	0.1
*/

#ifndef _STRESSTEST_H
#include "StressTest.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#include <sstream>
#include <iostream>

// ============================================================================
// StressTestPing01 — count incoming messages
// ============================================================================
StressTestPing01::StressTestPing01 (string _LN, Block *_PB, MessageBuilder *_PMB)
	: Action (_LN, _PB, _PMB)
{
}

StressTestPing01::~StressTestPing01 () {}

int StressTestPing01::Run (Message *_ReceivedMessage, CommandLine *_PCL,
						   vector<Message *> &ScheduledMessages,
						   Message *&InlineResponseMessage)
{
  StressTest *PST = (StressTest *)PB;

  if (!PST->Enabled)
	return OK;

  PST->IncReceived ();

  if (PST->GetReceived () % 100 == 0)
	{
	  PST->Stats << GetTime () << " Sent=" << PST->GetSent ()
				  << " Recv=" << PST->GetReceived ()
				  << " Drop=" << PST->GetDropped () << endl;
	}

  return OK;
}

// ============================================================================
// StressTest block
// ============================================================================
StressTest::StressTest (string _LN, Process *_PP, unsigned int _Index,
						GW *_PGW, HT *_PHT, string _Path)
	: Block (_LN, _PP, _Index, _Path)
{
  PGW = _PGW;
  PHT = _PHT;

  Sent = 0;
  Received = 0;
  Dropped = 0;
  Enabled = false;

  MyHostSCN = _PP->GetHostSelfCertifyingName ();

  Action *PA = 0;
  NewAction ("-stresstest --ping 0.1", PA);

  Stats.OpenOutputFile ("StressTest_Stats.txt", _Path, "DEFAULT");

  cout << endl << "[StressTest] Block initialized on " << MyHostSCN
	   << " (Enabled=" << Enabled << ")" << endl;
}

StressTest::~StressTest ()
{
  cout << endl << "[StressTest] Final: Sent=" << Sent
	   << " Received=" << Received << " Dropped=" << Dropped << endl;
}

void StressTest::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-stresstest --ping 0.1")
	{
	  StressTestPing01 *P = new StressTestPing01 (_LN, this, PP->PMB);
	  _PA = (Action *)P;
	  Actions.push_back ((Action *)P);
	}
}

int StressTest::GetAction (string _LN, Action *&_PA)
{
  return Block::GetAction (_LN, _PA);
}

int StressTest::DeleteAction (string _LN)
{
  return Block::DeleteAction (_LN);
}