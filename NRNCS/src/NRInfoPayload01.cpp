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

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

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
  long long Size = 0;

  // PB->S << Offset <<  this->GetLegibleName() << endl;

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

              // PB->S <<"Plotting the message payload recovered by NRNCS"<<endl;

              // Check for error
              if (Payload != 0)
              {
                // SPEC-018: Reset payload from any previous response on this reused InlineResponseMessage
                InlineResponseMessage->ResetPayload();

                // SPEC-019: Log payload hash at NRNCS for traceability
                {
                  string PayloadHash;
                  unsigned char* payload_bytes = (unsigned char*)Payload;
                  // Use NameGenerator which wraps MurmurHash3_x86_32 with seed 3571
                  PayloadHash = NameGenerator::GetInstance().GenerateFromCharArray((const char*)payload_bytes, Size);

                  PB->S << Offset << "(NRNCS forwarding payload: file=" << Values.at(0) << ", size=" << Size << " bytes, hash=" << PayloadHash << ")" << endl;
                }

                // SPEC-022: Create a SEPARATE message per file payload.
                // Do NOT accumulate multiple -info --payload CLs in the same
                // InlineResponseMessage — NG serialisation produces only ONE
                // payload block. All -info --payload CLs in one message share
                // that single payload. Fix: each file gets its own message
                // with its own payload, pushed directly to the GW InputQueue.

                {
                  NR* PNR = (NR*)PB;
                  GW* PGW = PNR->PGW;

                  // Create a new message for this file
                  Message* PayloadMsg = NULL;
                  PB->PP->NewMessage(GetTime(), 0, false, PayloadMsg);

                  // Extract routing from the received message's -m --cl
                  CommandLine* RoutedCL = NULL;
                  _ReceivedMessage->GetCommandLine("-m", "--cl", RoutedCL);

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
                      PMB->NewConnectionLessCommandLine(RoutedCL->Version,
                                                        &Limiters, &Sources, &Destinations,
                                                        PayloadMsg, RouteCL);
                    }
                  }

                  // Add -d --b delivery binding
                  CommandLine* DeliveryCL = NULL;
                  PMB->NewCommonCommandLine("-d", "--b", "0.1",
                                            PB->StringToInt("18"), Values.at(0), &Values,
                                            PayloadMsg, DeliveryCL);

                  // Copy the payload
                  PayloadMsg->SetPayloadFromCharArray(Payload, Size);

                  // Copy the ng -info --payload CL
                  CommandLine* InfoCL = NULL;
                  PayloadMsg->NewCommandLine(_PCL, InfoCL);
                  InfoCL->Version = "0.1";

                  // Add -scn --s to satisfy PushToInputQueue NoCL > 2 guard
                  string SCN = NameGenerator::GetInstance().GenerateFromMessage(PayloadMsg);
                  CommandLine* SCNCL = NULL;
                  PMB->NewSCNCommandLine("0.1", SCN, PayloadMsg, SCNCL);

                  // Push to GW input queue — the GW handles routing via -m --cl
                  PGW->PushToInputQueue(PayloadMsg);
                }

                // PB->S <<"A new pub for the file "<<Values.at(0)<<" was received with size "<<Size<<" bytes"<<endl; // TODO: FIXP/Update - Added this line to follow files being published
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

  // PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
