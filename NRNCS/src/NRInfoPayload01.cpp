/*
        NovaGenesis

        Name:		NRInfoPayload01
        Object:		NRInfoPayload01
        File:		NRInfoPayload01.cpp
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

#ifndef _NRINFOPAYLOAD01_H
#include "NRInfoPayload01.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

#define DEBUG

NRInfoPayload01::NRInfoPayload01(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

NRInfoPayload01::~NRInfoPayload01()
{
}

// Run the actions behind a received command line
// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
int NRInfoPayload01::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Values;
  char* Payload = 0;
  CommandLine* PCL = 0;
  long long Size = 0;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName() << endl;

#endif

  // Load the number of arguments
  if (_PCL->GetNumberofArguments(NA) == OK)
  {
    // Check the number of arguments
    if (NA == 1)
    {
      // Get received command line argument
      if (_PCL->GetArgument(0, Values) == OK)
      {
        if (Values.size() > 0)
        {
          if (PB->State == "operational")
          {
            // Get the payload size
            _ReceivedMessage->GetPayloadSize(Size);

            if (Size > 0)
            {
              // Get the pointer of the received message payload char array
              _ReceivedMessage->GetPayloadFromCharArray(Payload);

              // Check for error
              if (Payload != 0)
              {
                // SPEC-022: Create a SEPARATE message per -info --payload.
                // A single NG message can carry only ONE payload. When the NR
                // block processes multiple -info --payload in one message,
                // all -info --payload CLs share the same payload — the last
                // one loaded. Fix: create a dedicated message with its own
                // payload and push it directly to the GW input queue.

                NR* PNR = (NR*)PB;
                GW* PGW = PNR->PGW;

                // Extract routing from the received message's -m --cl
                CommandLine* RoutedCL = NULL;
                _ReceivedMessage->GetCommandLine("-m", "--cl", RoutedCL);

                // Create a new message for THIS file
                Message* PayloadMsg = NULL;
                PB->PP->NewMessage(GetTime(), 0, false, PayloadMsg);

                // Copy routing (-m --cl) — SWAP sources/destinations for return path
                if (RoutedCL != NULL)
                {
                    vector<string> Limiters;
                    vector<string> Sources;
                    vector<string> Destinations;
                    RoutedCL->GetArgument(0, Limiters);
                    RoutedCL->GetArgument(1, Sources);
                    RoutedCL->GetArgument(2, Destinations);

                    if (Limiters.size() > 0 && Sources.size() > 0 && Destinations.size() > 0)
                    {
                        CommandLine* RouteCL = NULL;
                        // SWAP: the message came FROM Sources TO Destinations (us).
                        // The response must go FROM us (Sources) TO the originator (Destinations).
                        PMB->NewConnectionLessCommandLine(RoutedCL->Version,
                                                          &Limiters, &Destinations, &Sources,
                                                          PayloadMsg, RouteCL);
                    }
                }

                // Add -d --b
                PMB->NewCommonCommandLine("-d", "--b", "0.1",
                                          PB->StringToInt("18"), Values.at(0), &Values,
                                          PayloadMsg, PCL);

                // Copy payload
                PayloadMsg->SetPayloadFromCharArray(Payload, Size);

                // Add -info --payload
                PayloadMsg->NewCommandLine(_PCL, PCL);
                PCL->Version = "0.1";

                // Add -scn --s (passes NoCL > 2 guard)
                string SCN = NameGenerator::GetInstance().GenerateFromMessage(PayloadMsg);
                PMB->NewSCNCommandLine("0.1", SCN, PayloadMsg, PCL);

                // Send to GW input queue
                PGW->PushToInputQueue(PayloadMsg);

#ifdef DEBUG

                PB->S << Offset << "(NRNCS forwarding payload: file=" << Values.at(0) << ", size=" << Size << " bytes)" << endl;

#endif
              }
              else
              {
                PB->S << Offset << "(ERROR: Unable to copy the payload)" << endl;
              }
            }
            else
            {
              PB->S << Offset << "(ERROR: The payload size is zero)" << endl;
            }
          }
        }
      }
    }
  }

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl
        << endl
        << endl;

#endif

  return Status;
}
