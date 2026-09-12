/*
        NovaGenesis

        Name:		NRSubBind01
        Object:		NRSubBind01
        File:		NRSubBind01.cpp
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

#ifndef _NRSUBBIND01_H
#include "NRSubBind01.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _NRNCS_H
#include "NRNCS.h"
#endif

//#define DEBUG

NRSubBind01::NRSubBind01(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

NRSubBind01::~NRSubBind01()
{
}

// Run the actions behind a received command line
// ng -s --b _Version [ < 1 string _Category > < _SCNsSize string S_1 ... S_SCNsSize > ]
int NRSubBind01::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Category;
  vector<string> Key;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName() << endl;

#endif

  // Load the number of arguments
  if (_PCL->GetNumberofArguments(NA) == OK)
  {
    // Check the number of arguments
    if (NA == 2)
    {
      // Get received command line arguments
      if (_PCL->GetArgument(0, Category) == OK && _PCL->GetArgument(1, Key) == OK)
      {
        if (Category.size() > 0 && Key.size() > 0)
        {
#ifdef DEBUG

          PB->S << Offset << "(Processing " << Key.size() << " subscription key(s))" << endl;

#endif

          // SPEC-021: Create a SEPARATE message per subscription key.
          // A single NG message can carry only ONE payload. When the HT
          // processes multiple ng -g --b in one message, all -info --payload
          // CLs share the same payload — the last one loaded. Fix: each key
          // gets its own message routed to the HT, so HTGetBind01 creates
          // one InlineResponseMessage per file — each with its own payload.
          //
          // SCN guard: GW::PushToInputQueue requires NoCL > 2. Each message
          // must have at least 3 CLs, so we add -scn --s as the third CL.

          NR* PNR = (NR*)PB;
          GW* PGW = PNR->PGW;
          Block* PHTB = (Block*)PNR->PHT;

          // Extract routing from the received message's -m --cl
          CommandLine* RoutedCL = NULL;
          _ReceivedMessage->GetCommandLine("-m", "--cl", RoutedCL);

          vector<string> RouteLimiters;
          vector<string> RouteSources;
          vector<string> RouteDestinations;

          if (RoutedCL != NULL)
          {
            RoutedCL->GetArgument(0, RouteLimiters);
            RoutedCL->GetArgument(1, RouteSources);
            RoutedCL->GetArgument(2, RouteDestinations);
          }

          for (unsigned int i = 0; i < Key.size(); i++)
          {
#ifdef DEBUG

            PB->S << Offset << "(Key[" << i << "]=" << Key.at(i) << ")" << endl;

#endif

            Message* GetBindMessage = NULL;
            CommandLine* MsgCl = NULL;
            CommandLine* GetBindCL = NULL;

            // Create a new message for this key
            PB->PP->NewMessage(GetTime(), 1, false, GetBindMessage);

            // Add routing: -m --cl with destination = HT (copied from received message)
            if (RoutedCL != NULL && RouteDestinations.size() == 4)
            {
              vector<string> DestCopy = RouteDestinations;
              DestCopy[3] = PHTB->GetSelfCertifyingName();

              PMB->NewConnectionLessCommandLine("0.1", &RouteLimiters, &RouteSources, &DestCopy,
                                                GetBindMessage, MsgCl);
            }

            // Add ng -g --b for this key only
            PMB->NewGetCommandLine("0.1", PB->StringToInt(Category.at(0)), Key.at(i),
                                   GetBindMessage, GetBindCL);

            // SPEC-021: Add -scn --s to satisfy PushToInputQueue NoCL > 2 guard.
            // Without this third CL, the message is silently discarded at GW.cpp:229.
            string SCN = NameGenerator::GetInstance().GenerateFromMessage(GetBindMessage);
            PMB->NewSCNCommandLine("0.1", SCN, GetBindMessage, GetBindCL);

            // Push to GW input queue for processing by the HT
            PGW->PushToInputQueue(GetBindMessage);
          }
        }
        else
        {
          PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;
        }
      }
      else
      {
        PB->S << Offset << "(ERROR: Unable to read the arguments)" << endl;
      }
    }
    else
    {
      PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;
    }
  }
  else
  {
    PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
  }

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl
        << endl
        << endl;

#endif

  return Status;
}
