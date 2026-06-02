/*
	NovaGenesis

	Name:		GWExposition02
	Object:		GWExposition02
	File:		GWExposition02.cpp
	Author:		Antonio Marcos Alberti
	Date:		06/2026
	Version:	0.2

   Copyright (C) 2026  Antonio Marcos Alberti

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

#ifndef _GWEXPOSITION02_H
#include "GWExposition02.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

// #define DEBUG

GWExposition02::GWExposition02(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

GWExposition02::~GWExposition02()
{
}

// Run the actions behind a received command line
// ng -exposition 0.2
int GWExposition02::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = OK;
  string Offset = "          ";

  // Only PGCS runs the exposition
  if (PB->PP->GetLegibleName() != "PGCS")
  {
    PB->S << Offset << "(GWExposition02: Not PGCS, skipping exposition)" << endl;
    SelfReschedule(_ReceivedMessage);
    return Status;
  }

  PB->S << Offset << "(GWExposition02: Periodic exposition on PGCS)" << endl;

  // Expose all known peers to all other known peers
  ExposePeers();

  // Self-reschedule for next execution in 1 second
  SelfReschedule(_ReceivedMessage);

  PB->S << Offset << "(GWExposition02: Done)" << endl;

  return Status;
}

// Build and send hello messages for all known peers to all other known peers
// For each known peer PID, builds a hello IPC message advertising that peer's
// identity and sends it to every other known peer's SHM key.
int GWExposition02::ExposePeers()
{
  int Status = OK;
  string Offset = "          ";
  GW* PGW = 0;
  vector<string> KnownPIDs;

  PGW = (GW*)PB;

  long selfBaseKey = PB->PP->Key;

  // Build the set of this process's own SHM input segment keys to avoid self-writes
  vector<string> selfInputKeys;
  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES; z++)
  {
    selfInputKeys.push_back(std::to_string(selfBaseKey + z));
  }

  // Get all known peer PIDs from category 19
  if (PGW->PHT->GetBindingKeys(19, KnownPIDs) == OK)
  {
    if (KnownPIDs.size() == 0)
    {
      PB->S << Offset << "(GWExposition02: No known peers to expose)" << endl;
      return Status;
    }

    PB->S << Offset << "(GWExposition02: Exposing " << KnownPIDs.size() << " known peers to each other)" << endl;

    // For each known peer, build a hello and send to all other peers
    for (unsigned int i = 0; i < KnownPIDs.size(); i++)
    {
      string exposedPID = KnownPIDs.at(i);

      // Get the exposed peer's IPC key from category 19
      string exposedIPCKey = "";
      vector<string>* ExposedKeys = new vector<string>;
      if (PGW->GetHTBindingValues(19, exposedPID, ExposedKeys) == OK && ExposedKeys->size() > 0)
      {
        exposedIPCKey = ExposedKeys->at(0);
      }
      delete ExposedKeys;

      if (exposedIPCKey.empty())
      {
        PB->S << Offset << "(GWExposition02: ERROR: No IPC key for peer PID " << exposedPID << ")" << endl;
        continue;
      }

      // Get the exposed peer's legible name from category 20
      string exposedLN = "";
      vector<string>* ExposedNames = new vector<string>;
      if (PGW->GetHTBindingValues(20, exposedPID, ExposedNames) == OK && ExposedNames->size() > 0)
      {
        exposedLN = ExposedNames->at(0);
      }
      delete ExposedNames;

      if (exposedLN.empty())
      {
        PB->S << Offset << "(GWExposition02: ERROR: No legible name for peer PID " << exposedPID << ")" << endl;
        continue;
      }

      PB->S << Offset << "(GWExposition02: Exposing peer " << exposedLN << " with PID " << exposedPID << " and key " << exposedIPCKey << ")" << endl;

      key_t peerKey = (key_t)std::stol(exposedIPCKey);

      // Send to all OTHER known peers (not the exposed peer itself, not self)
      for (unsigned int j = 0; j < KnownPIDs.size(); j++)
      {
        if (i == j)
        {
          continue; // Don't send back to the exposed peer
        }

        string targetPID = KnownPIDs.at(j);

        // Get the target peer's IPC key from category 19
        string targetIPCKey = "";
        vector<string>* TargetKeys = new vector<string>;
        if (PGW->GetHTBindingValues(19, targetPID, TargetKeys) == OK && TargetKeys->size() > 0)
        {
          targetIPCKey = TargetKeys->at(0);
        }
        delete TargetKeys;

        if (targetIPCKey.empty())
        {
          continue;
        }

        // Get target peer legible name for logging
        string targetLN = "";
        vector<string>* TargetNames = new vector<string>;
        if (PGW->GetHTBindingValues(20, targetPID, TargetNames) == OK && TargetNames->size() > 0)
        {
          targetLN = TargetNames->at(0);
        }
        delete TargetNames;

        // Do not forward to any of this process's own SHM input segment keys
        bool isSelfSegment = false;
        for (unsigned int z = 0; z < selfInputKeys.size(); z++)
        {
          if (targetIPCKey == selfInputKeys.at(z))
          {
            isSelfSegment = true;
            break;
          }
        }

        if (isSelfSegment)
        {
          continue;
        }

        // Build a fresh hello IPC message for this target
        // This is identical to what the exposed peer would send itself
        Message* FreshHello = 0;
        CommandLine* FreshPCL = 0;
        vector<string> FLimiters;
        vector<string> FSources;
        vector<string> FDestinations;
        string Version = "0.2";

        PB->PP->NewMessage(GetTime(), 0, false, FreshHello);

        FLimiters.push_back(PB->PP->Intra_OS);

        // Use the exposed peer's identity as sender (so receiver learns about the exposed peer)
        FSources.push_back(exposedPID);
        FSources.push_back(exposedPID);

        FDestinations.push_back("FFFFFFFF");
        FDestinations.push_back("FFFFFFFF");

        PMB->NewConnectionLessCommandLine("0.1", &FLimiters, &FSources, &FDestinations, FreshHello, FreshPCL);
        PMB->NewIPCHelloCommandLine("--ipc", Version, peerKey, exposedLN, FreshHello, FreshPCL);

        string FSCN = "FFFFFFFF";
        PB->GenerateSCNFromMessageBinaryPatterns(FreshHello, FSCN);
        PMB->NewSCNCommandLine("0.1", FSCN, FreshHello, FreshPCL);

        PB->S << Offset << "(GWExposition02: Sending exposition of " << exposedLN << " to " << targetLN << " with key = " << targetIPCKey << ")" << endl;
        PGW->PushToOutputQueue(targetIPCKey, FreshHello);
      }
    }
  }
  else
  {
    PB->S << Offset << "(GWExposition02: No known peers to expose)" << endl;
  }

  return Status;
}

// Self-reschedule: create a copy of this command with time +1s
void GWExposition02::SelfReschedule(Message* _ReceivedMessage)
{
  Message* SelfMsg = 0;
  CommandLine* PCL = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;

  // Create a new message scheduled 1 second from now
  PB->PP->NewMessage(GetTime() + 1.0, 1, false, SelfMsg);

// Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, SelfMsg, PCL);

  // Copy the command lines from the received message
  SelfMsg->NewCommandLine("-run", "--exposition", "0.2", PCL);

  // Generate the SCN
  string SCN = "FFFFFFFF";
  PB->GenerateSCNFromMessageBinaryPatterns(SelfMsg, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine("0.1", SCN, SelfMsg, PCL);

  // Push to the GW input queue
  GW* PGW = (GW*)PB;
  PGW->PushToInputQueue(SelfMsg);
}
