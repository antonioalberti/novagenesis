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
#ifdef DEBUG
    PB->S << Offset << "(GWExposition02: Not PGCS, skipping exposition)" << endl;
#endif

    return Status;
  }

#ifdef DEBUG
  PB->S << Offset << "(GWExposition02: Periodic exposition on PGCS)" << endl;
#endif

  // Expose all known peers to all other known peers
  ExposePeers();

  // Self-reschedule for next execution in 10 second
  SelfReschedule(_ReceivedMessage);

#ifdef DEBUG
  PB->S << Offset << "(GWExposition02: Done)" << endl;
#endif

  return Status;
}

// Build and send hello messages for all known peers to all other known peers
// and also expose the LOCAL process (PGCS) to each known peer.
//
// Phase 1: For each known peer PID, builds a hello IPC message advertising
//          that peer's identity and sends it to every other known peer's
//          SHM key. (Peer-to-peer redistribution.)
//
// Phase 2: For the LOCAL process (PGCS), builds a hello IPC message
//          advertising PGCS's own identity (self PID/BID/LN/SHM key) and
//          sends it to each known peer's SHM key. This is needed because
//          GWRunHelloIPC02 is intentionally disabled on PGCS (its SHM key
//          11 is well-known and statically configured on the peers), yet
//          some bindings that peers (e.g. ContentApp) need can only be
//          delivered via a hello 0.2 message, not via the static key
//          advertisement. With Phase 2, every known peer receives exactly
//          one hello 0.2 from PGCS per exposition cycle, regardless of how
//          many other peers are known. This also resolves NG-042-03
//          ("GWExposition02 useless with 1 peer"): with a single known
//          peer, Phase 1 generates zero messages, but Phase 2 still
//          delivers the PGCS hello to that peer.
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
#ifdef DEBUG
      PB->S << Offset << "(GWExposition02: No known peers to expose)" << endl;
#endif
      return Status;
    }

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

      // Get the exposed peer's IPC key from category 19
      string exposedBID = "";
      vector<string>* ExposedBIDs = new vector<string>;
      if (PGW->GetHTBindingValues(5, exposedPID, ExposedBIDs) == OK && ExposedBIDs->size() > 0)
      {
        exposedBID = ExposedBIDs->at(0);
      }
      delete ExposedBIDs;

      if (exposedIPCKey.empty())
      {
#ifdef DEBUG
        PB->S << Offset << "(GWExposition02: ERROR: No IPC key for peer PID " << exposedPID << ")" << endl;
#endif
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
#ifdef DEBUG
        PB->S << Offset << "(GWExposition02: ERROR: No legible name for peer PID " << exposedPID << ")" << endl;
#endif
        continue;
      }

      key_t peerKey = (key_t)std::stol(exposedIPCKey);

      // Send to all OTHER known peers (not the exposed peer itself, not self)
      for (unsigned int j = 0; j < KnownPIDs.size(); j++)
      {
        if (i == j)
        {
          continue; // Don't send back to the exposed peer
        }

#ifdef DEBUG
        PB->S << Offset << "(GWExposition02: Exposing peer " << exposedLN << " with PID " << exposedPID << " and key " << exposedIPCKey << ")" << endl;
#endif

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

        // Get target peer BID from category 5
        string targetBID = "";
        vector<string>* TargetBIDs = new vector<string>;
        if (PGW->GetHTBindingValues(5, targetPID, TargetBIDs) == OK && TargetBIDs->size() > 0)
        {
          targetBID = TargetBIDs->at(0);
        }
        delete TargetBIDs;

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

        // Setting up the OS SCN as the space limiter
        FLimiters.push_back(PB->PP->Intra_OS);

        // Setting up the originator's (exposed peer's) PID and GW BID as
        // the source SCNs. Previously this used PGCS's own SCN, which made
        // the forwarded hello look like PGCS was the originator. The receiver
        // then stored a binding like `Cat[20] PGCS_PID -> NRNCS` (mismatch)
        // and Categories 5/6 were never populated correctly, breaking
        // domain discovery.
        FSources.push_back(exposedPID);
        FSources.push_back(exposedBID);

        FDestinations.push_back(targetPID);
        FDestinations.push_back(targetBID);

        PMB->NewConnectionLessCommandLine("0.1", &FLimiters, &FSources, &FDestinations, FreshHello, FreshPCL);
        PMB->NewIPCHelloCommandLine("--ipc", Version, peerKey, exposedLN, FreshHello, FreshPCL);

        string FSCN = "FFFFFFFF";
        PB->GenerateSCNFromMessageBinaryPatterns(FreshHello, FSCN);
        PMB->NewSCNCommandLine("0.1", FSCN, FreshHello, FreshPCL);

#ifdef DEBUG
        PB->S << Offset << "(GWExposition02: Sending exposition of " << exposedLN << " to " << targetLN << " with key = " << targetIPCKey << ")" << endl;
#endif
        PGW->PushToOutputQueue(targetIPCKey, FreshHello);
      }
    }

// Phase 2: expose the LOCAL process (PGCS) to each known peer.
//
// PGCS does not run GWRunHelloIPC02 (key 11 is the well-known
// initialization key, statically configured on every peer). Without
// Phase 2, peers would never receive a hello 0.2 originated by PGCS,
// and the bindings that hello carries (peer self-identification that
// goes beyond the static key advertisement) would be missing on the
// peer side. Phase 2 also resolves NG-042-03: with a single known
// peer, Phase 1 emits zero messages (i==j filter drops the only
// iteration), but Phase 2 still delivers the PGCS hello to that
// peer.
#ifdef DEBUG
    PB->S << Offset << "(GWExposition02: Self-exposing PGCS to " << KnownPIDs.size() << " known peer(s))" << endl;
#endif

    string selfPID = PB->PP->GetSelfCertifyingName();
    string selfBID = selfPID; // GW BID of the local process == PID
    string selfLN = PB->PP->GetLegibleName();
    key_t selfKey = (key_t)selfBaseKey;

    for (unsigned int j = 0; j < KnownPIDs.size(); j++)
    {
      string targetPID = KnownPIDs.at(j);

      // Defensive: skip self (self should not be in cat 19, but the
      // isSelfSegment check below would also catch this case)
      if (targetPID == selfPID)
      {
        continue;
      }

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

      // Get target peer BID from category 5
      string targetBID = "";
      vector<string>* TargetBIDs = new vector<string>;
      if (PGW->GetHTBindingValues(5, targetPID, TargetBIDs) == OK && TargetBIDs->size() > 0)
      {
        targetBID = TargetBIDs->at(0);
      }
      delete TargetBIDs;

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

      // Build a fresh hello IPC message advertising the LOCAL process.
      // This is identical to what GWRunHelloIPC02 would build, except
      // that destinations are the specific target peer (not FFFFFFFF)
      // and the IPC key being advertised is the local process's own
      // SHM key (selfKey), not a remote peer's key.
      Message* SelfHello = 0;
      CommandLine* SelfPCL = 0;
      vector<string> SelfLimiters;
      vector<string> SelfSources;
      vector<string> SelfDestinations;
      string Version = "0.2";

      PB->PP->NewMessage(GetTime(), 0, false, SelfHello);

      // Setting up the OS SCN as the space limiter
      SelfLimiters.push_back(PB->PP->Intra_OS);

      // Source: the local process's PID and GW BID (PGCS)
      SelfSources.push_back(selfPID);
      SelfSources.push_back(selfBID);

      // Destination: the target peer (specific, not broadcast)
      SelfDestinations.push_back(targetPID);
      SelfDestinations.push_back(targetBID);

      // Get the local HT block BID for inclusion in hello v2.0
      // This enables the receiver to discover PGCS::HT and address
      // discovery messages directly to it.
      string selfHTBID = "";
      {
        Block* PHTB = 0;
        if (PB->PP->GetBlock("HT", PHTB) == OK && PHTB != 0)
        {
          selfHTBID = PHTB->GetSelfCertifyingName();
        }
      }

      PMB->NewConnectionLessCommandLine("0.1", &SelfLimiters, &SelfSources, &SelfDestinations, SelfHello, SelfPCL);
      PMB->NewIPCHelloCommandLine("--ipc", "2.0", selfKey, selfLN, selfHTBID, SelfHello, SelfPCL);

      string SelfSCN = "FFFFFFFF";
      PB->GenerateSCNFromMessageBinaryPatterns(SelfHello, SelfSCN);
      PMB->NewSCNCommandLine("0.1", SelfSCN, SelfHello, SelfPCL);

#ifdef DEBUG
      PB->S << Offset << "(GWExposition02: PGCS self-exposing to " << targetLN << " (key = " << targetIPCKey << "))" << endl;
#endif
      PGW->PushToOutputQueue(targetIPCKey, SelfHello);
    }
  }
  else
  {
#ifdef DEBUG
    PB->S << Offset << "(GWExposition02: No known peers to expose)" << endl;
#endif
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

  // Setting up the OS SCN as the space limiter
  Limiters.push_back(PB->PP->Intra_Process);

  // Setting up this process BID as the source SCN
  Sources.push_back(PB->GetSelfCertifyingName());

  // Setting up this process BID as the destination SCN
  Destinations.push_back(PB->GetSelfCertifyingName());

  // Create a new message scheduled 1 second from now
  PB->PP->NewMessage(GetTime() + 10, 1, false, SelfMsg);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine("0.1", &Limiters, &Sources, &Destinations, SelfMsg, PCL);

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
