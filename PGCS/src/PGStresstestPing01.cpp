/*
	NovaGenesis

	Name:		PGStresstestPing01
	Object:		PGStresstestPing01
	File:		PGStresstestPing01.cpp
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

#ifndef _PGSTRESSTESTPING01_H
#include "PGStresstestPing01.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

#include <iostream>
#include <stdexcept>

//#define DEBUG

PGStresstestPing01::PGStresstestPing01 (string _LN, Block *_PB, MessageBuilder *_PMB)
	: Action (_LN, _PB, _PMB)
{
}

PGStresstestPing01::~PGStresstestPing01 () {}

int PGStresstestPing01::Run (Message *_ReceivedMessage, CommandLine *_PCL,
							 vector<Message *> &ScheduledMessages,
							 Message *&InlineResponseMessage)
{
  PG *PPG = (PG *)PB;
  string Offset = "                    ";

  #ifdef DEBUG
  std::cerr << ">>> PGStresstestPing01::Run CALLED <<<" << std::endl;
#endif

#ifdef DEBUG
  PB->S << Offset << this->GetLegibleName() << endl;
#endif

  if (!PPG->StressEnabled)
    return OK;

  PPG->StressReceived++;

  // Calculate one-way delay from payload timestamp
  double Delay = 0;
  long long PayloadSize = 0;
  if (_ReceivedMessage && _ReceivedMessage->GetPayloadSize(PayloadSize) == OK && PayloadSize > 0)
    {
      char *PayloadPtr = nullptr;
      _ReceivedMessage->GetPayloadFromCharArray(PayloadPtr);
      if (PayloadPtr)
        {
          string PayloadStr(PayloadPtr, PayloadSize);
          try {
            double SendTime = stod(PayloadStr);
            Delay = PB->GetTime() - SendTime;
            if (Delay < 0) Delay = 0;
            PPG->DelayStats->Sample(Delay);
            PPG->DelayStats->CalculateArithmetic();
          } catch (...) {
            Delay = 0;
          }
        }
    }

  if (PPG->StressReceived % 100 == 0)
    {
      double LossRate = 100.0;
      unsigned long long Expected = 0;
      int NPeers = (int)PPG->PGCSTuples.size();

      if (NPeers > 0 && PPG->StressInterval > 0)
        {
          double Elapsed = PB->GetTime() - PB->PP->InstantiationTime;
          if (Elapsed > 0)
            Expected = (unsigned long long)(NPeers * Elapsed / PPG->StressInterval);
        }

      if (Expected > 0)
        LossRate = 100.0 * (1.0 - (double)PPG->StressReceived / (double)Expected);

      // Sample loss rate to OutputVariable (exported to .dat)
      PPG->Loss->Sample(LossRate);
      PPG->Loss->CalculateArithmetic();
      PPG->Loss->SampleToFile(PB->GetTime());

      // Also write delay stats
      PPG->DelayStats->SampleToFile(PB->GetTime());

      // Summary line to StressStats
      PPG->StressStats << PPG->GetTime()
                       << " NPeers=" << NPeers
                       << " Sent=" << PPG->StressSent
                       << " Recv=" << PPG->StressReceived
                       << " Expected=" << Expected
                       << " Loss=" << LossRate << "%"
                       << " DelayAvg=" << PPG->DelayStats->GetMean()
                       << " DelaySigma=" << PPG->DelayStats->GetSigma()
                       << " DelayME=" << PPG->DelayStats->GetME()
                       << " DelayLower=" << PPG->DelayStats->GetLower()
                       << " DelayUp=" << PPG->DelayStats->GetUp()
                       << endl;
    }

  return OK;
}
