/*
        NovaGenesis

        Name:		CoreInfoPayload01
        Object:		CoreInfoPayload01
        File:		CoreInfoPayload01.cpp
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

#ifndef _COREINFOPAYLOAD01_H
#include "CoreInfoPayload01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "../../Common/src/NameGenerator.h"
#endif

////#define DEBUG // To follow message processing

CoreInfoPayload01::CoreInfoPayload01(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

CoreInfoPayload01::~CoreInfoPayload01()
{
}

// Run the actions behind a received command line
// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
int CoreInfoPayload01::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Values;
  Core* PCore = 0;

  PCore = (Core*)PB;

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
      _PCL->GetArgument(0, Values);

      if (Values.size() > 0)
      {
        // The additional information is related to the message payload
        if (_PCL->Alternative == "--payload")
        {
          if (_ReceivedMessage->GetHasPayloadFlag() == true)
          {
            string PayloadPath = PB->GetPath();

#ifdef DEBUG
            PB->S << Offset << "(The received message has a payload whose file is named " << Values.at(0) << ")" << endl;
            PB->S << Offset << "(Saving the payload on the path " << PayloadPath << ")" << endl;
#endif
            _ReceivedMessage->SetPayloadFileName(Values.at(0));
            _ReceivedMessage->SetPayloadFilePath(PayloadPath);
            _ReceivedMessage->SetPayloadFileOption("BINARY");
            // SPEC-015: Removed ExtractPayloadCharArrayFromMessageCharArray() — redundant call that
            // re-parses Msg using stringstream::getline(), corrupting binary payloads (e.g. JPG).
            // The GW already correctly extracted Payload via ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2().
            //_ReceivedMessage->ExtractPayloadCharArrayFromMessageCharArray();
            _ReceivedMessage->ConvertPayloadFromCharArrayToFile();

            // SPEC-019: Log payload hash at ContentApp for traceability
            {
              string PayloadHash;
              // Read the file we just saved to compute hash
              File F1;
              F1.OpenInputFile(Values.at(0), PayloadPath, "BINARY");
              F1.seekg(0, ios::end);
              long long PayloadSize = F1.tellg();
              F1.seekg(0);
              if (PayloadSize > 0) {
                char* payload_bytes = new char[PayloadSize];
                F1.read(payload_bytes, PayloadSize);
                // Use NameGenerator which wraps MurmurHash3_x86_32 with seed 3571
                PayloadHash = NameGenerator::GetInstance().GenerateFromCharArray(payload_bytes, PayloadSize);
                delete[] payload_bytes;
              }
              F1.CloseFile();

              PB->S << Offset << "(ContentApp received payload: file=" << Values.at(0) << ", size=" << PayloadSize << " bytes, hash=" << PayloadHash << ")" << endl;
            }

            // Update related Subscription
            for (unsigned int i = 0; i < PCore->Subscriptions.size(); i++)
            {
              Subscription* PS = PCore->Subscriptions[i];

#ifdef DEBUG
              PB->S << Offset << "(Testing subscription " << i << ")" << endl;

              PB->S << Offset << "(Subscription status is " << PS->Status << ")" << endl;
#endif

              if (PS->Status == "Waiting delivery")
              {
#ifdef DEBUG
                PB->S << Offset << "(Storing the file named " << Values.at(0) << " to this subscription)" << endl;

                PB->S << Offset << "(Changing subscription status from \"Waiting delivery\" to \"Processing required\")" << endl;
#endif

                PS->Status = "Processing required";

                PS->FileName = Values.at(0);

                // SPEC-020: Do NOT set Status = "Delivered" here.
                // CoreRunEvaluate01 needs "Processing required" to trigger
                // the acceptance flow. It will set Status = "Delete" after
                // creating the Service_Accepted response.
                // CoreDeliveryBind01 already set HasContent = true, so
                // CoreRunPeriodic01 won't re-subscribe (it only re-subscribes
                // when Status == "Waiting delivery").

                // SPEC-017: break after updating the first matching subscription.
                // Each -info --payload corresponds to exactly one delivery.
                break;

#ifdef DEBUG
                PCore->Debug.OpenOutputFile();

                PCore->Debug << "Received the file " << Values.at(0) << " related to the subscription of index " << i << ". Status of the subscription is " << PS->Status << endl;

                PCore->Debug.CloseFile();
#endif
              }
            }

            // **************************************************
            // Type 1 messages statistics
            // **************************************************
            if (_ReceivedMessage->GetType() == 1)
            {
              double Time = GetTime();

              // Sample
              PCore->tsmiup1->Sample(Time - _ReceivedMessage->GetInstantiationTime());

              // Update the mean
              PCore->tsmiup1->CalculateArithmetic();

              // Sample to file
              PCore->tsmiup1->SampleToFile(Time);
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
