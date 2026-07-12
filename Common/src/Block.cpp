/*
        NovaGenesis

        Name:		Block
        Object:		Block
        File:		Block.cpp
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

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _MURMURHASH3_H_
#include "MurmurHash3.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _NAMEGENERATOR_H
#include "NameGenerator.h"
#endif

// #define DEBUG // To follow message processing
#define DEBUG1 // To follow inlineresponse message generation only

Block::Block(string _LN, Process* _PP, unsigned int _Index, string _Path)
{
  LN = _LN;
  Index = _Index;
  SCN = "";
  PP = _PP;
  StopProcessingMessage = false;
  Path = _Path;
  string Offset = "          ";

  // Generate the required prime numbers
  unsigned int n_primes = 0;

  for (unsigned int z = 2; n_primes < 32; z++)
  {
    for (unsigned int w = 0; w < n_primes; w++)
    {
      if (z % primes[w] == 0)
      {
        goto not_prime;
      }
    }

    primes[n_primes] = z;

    n_primes++;

  not_prime:;
  }

  SCN = NameGenerator::GetInstance().GenerateFromString(LN);

  cout << Offset << "(" << LN << " block has the SCN = " << SCN << ")" << endl;
}

Block::~Block()
{
}

// Set block legible name
void Block::SetLegibleName(string _LN)
{
  LN = _LN;
}

// Set block self-certifying name
void Block::SetSelfCertifyingName(string _SCN)
{
  SCN = _SCN;
}

// Set working path
void Block::SetPath(string _Path)
{
  Path = _Path;
}

// Set operational state
void Block::SetState(string _State)
{
  State = _State;
}

// Get block legible name
string Block::GetLegibleName()
{
  return LN;
}

// Get block self-certifying name
string Block::GetSelfCertifyingName()
{
  return SCN;
}

// Get working path
string Block::GetPath()
{
  return Path;
}

// Get operational state
string Block::GetState()
{
  return State;
}

// Get index
unsigned int Block::GetIndex()
{
  return Index;
}

// Run the actions behind a received message, generating a resulting message
int Block::Run(Message* _ReceivedMessage, Message*& _InlineResponseMessage)
{
  int Status = ERROR;           // Overall execution status
  vector<int> CLStatus;         // Status of every command line action
  unsigned int NCL = 0;         // Number of command lines
  CommandLine* PCL = 0;         // Pointer to the command line being executed
  Action* PA1 = 0;              // Pointer to the action that is performing such execution
  Action* PA2 = 0;              // Pointer to the action that is performing such execution
  ActionsIterator it;           // Iterator to find out a value
  unsigned int i = 0;           // Command lines counter
  string Offset = "          "; // Auxiliary variable for logging

  if (LN != "GW")
  {
    Offset = "                    ";
  }

#ifdef DEBUG

  S << endl
    << endl;

  S << Offset << "*************************************************************************************************" << endl;
  S << Offset << "   Block = " << LN << ", State = " << State << ", SCN = " << SCN << endl;
  S << Offset << "*************************************************************************************************" << endl;

  S << Offset << "*****************************************" << endl;
  S << Offset << "   Block = " << LN << ", State = " << State << endl;
  S << Offset << "*****************************************" << endl;

#endif

  // ***********************************************************************
  // Call the corresponding actions to perform the processing
  // ***********************************************************************

  if (PP->OkToRun(_ReceivedMessage) == true)
  {
    // SPEC-003: Moved DEBUGX and message dump AFTER OkToRun check.
    // Previously, these accessed _ReceivedMessage before verifying it
    // is still in Process::Messages[], causing use-after-free SIGSEGV
    // when a freed message pointer was dequeued from the InputQueue.
#ifdef DEBUG
    {
      unsigned int _dbgNCL = 0;
      _ReceivedMessage->GetNumberofCommandLines(_dbgNCL);
      S << Offset << "(DEBUGX: _RcvMsg=" << _ReceivedMessage
        << " NoCL=" << _dbgNCL
        << " HasPayload=" << _ReceivedMessage->HasPayloadFlag
        << " PayloadSize=" << _ReceivedMessage->PayloadSize
        << ")" << endl;
      for (unsigned int _dbgi = 0; _dbgi < _dbgNCL && _dbgi < 10; _dbgi++)
      {
        CommandLine* _dbgCL = 0;
        if (_ReceivedMessage->GetCommandLine(_dbgi, _dbgCL) == OK && _dbgCL != 0)
        {
          S << Offset << "(DEBUGX: CL[" << _dbgi << "] ptr=" << _dbgCL
            << " Name=\"" << _dbgCL->Name
            << "\" Alt=\"" << _dbgCL->Alternative
            << "\" Ver=\"" << _dbgCL->Version
            << "\" NoA=" << _dbgCL->NoA
            << " Arguments=" << _dbgCL->Arguments
            << ")" << endl;
        }
        else
        {
          S << Offset << "(DEBUGX: CL[" << _dbgi << "] = NULL)" << endl;
        }
      }
    }

    S << endl
      << "(" << endl
      << *_ReceivedMessage << ")" << endl;
#endif

    if (_ReceivedMessage->GetNumberofCommandLines(NCL) == OK)
    {
      if (NCL != 0)
      {
#ifdef DEBUG
        S << endl
          << Offset << "(NCL = " << NCL << ")" << endl;
#endif

        CLStatus.resize(NCL, ERROR);

        // Vector of indexes to the block Messages container
        vector<Message*> ScheduledMessages;

        while (i < NCL && StopProcessingMessage == false)
        {
          if (_ReceivedMessage->GetCommandLine(i, PCL) == OK)
          {
            if (PCL != 0)
            {
              if (PCL->Name != "" && PCL->Alternative != "")
              {
                char C1 = PCL->Name[0];
                char C2 = PCL->Alternative[0];
                char C3 = PCL->Alternative[1];

                if (C1 == '-' && C2 == '-' && C3 == '-')
                {
                  // Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
                  string LN1(PCL->Name + " " + PCL->Alternative + " " + PCL->Version);

                  // Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
                  string LN2(PCL->Name + " " + PCL->Alternative + " 0.1");

#ifdef DEBUG

                  S << Offset << "(LN1 = " << LN1 << ", LN2 = " << LN2 << ".)" << endl;
#endif

                  for (unsigned int j = 0; j < Actions.size(); j++)
                  {
                    if (Actions[j]->GetLegibleName() == LN1)
                    {
                      PA1 = Actions[j];

#ifdef DEBUG
                      S << Offset << "(Found)" << endl;
#endif

                      break;
                    }
                  }

                  if (PA1 != 0)
                  {
#ifdef DEBUG
                    S << Offset << "(Action is " << PA1->GetLegibleName() << ")" << endl;
#endif

                    // Call the action
                    CLStatus[i] = PA1->Run(_ReceivedMessage, PCL, ScheduledMessages, _InlineResponseMessage);

                    PA1 = 0;

                    PA2 = 0;
                  }
                  else
                  {

#ifdef DEBUG
                    S << Offset << "(Warning: " << LN1 << " not implemented.)" << endl;
#endif

                    for (unsigned int j = 0; j < Actions.size(); j++)
                    {
                      if (Actions[j]->GetLegibleName() == LN2)
                      {
                        PA2 = Actions[j];

                        break;
                      }
                    }
                  }

                  if (PA1 == 0 && PA2 != 0)
                  {
#ifdef DEBUG
                    S << Offset << "(Action is " << PA2->GetLegibleName() << ")" << endl;
#endif

                    // Call the action
                    CLStatus[i] = PA2->Run(_ReceivedMessage, PCL, ScheduledMessages, _InlineResponseMessage);

                    PA2 = 0;
                  }
                }
                else
                {
                  S << Offset << "(ERROR: Corrupted command line (2)!)" << endl;

                  CLStatus[i] = ERROR;

                  StopProcessingMessage = true;

                  _ReceivedMessage->MarkToDelete();

                  break;
                }
              }
              else
              {
                S << Offset << "(ERROR: Corrupted command line (1)!)" << endl;
              }
            }
            else
            {
              S << "(ERROR: Invalid command line)" << endl;

              CLStatus[i] = ERROR;
            }
          }
          else
          {
            S << "(ERROR: Unable to get the command line)" << endl;

            CLStatus[i] = ERROR;
          }

          i++;
        }

        // Set overall status
        for (unsigned int j = 0; j < NCL; j++)
        {
          if (CLStatus[j] == ERROR)
          {
            Status = ERROR;
          }
        }

        // SPEC-003: Removed ScheduledMessages cleanup loop.
        // With the GWStatusS01Msg dead code eliminated from GWMsgCl01,
        // no action pushes to ScheduledMessages anymore. The vector is
        // always empty, making the cleanup loop unnecessary.

#ifdef DEBUG1

        // SPEC-DEBUG-HT: Log the InlineResponseMessage after processing ALL command lines
        // so we can see the accumulated -d --b and -info --payload CLs with a single payload.
        if (LN == "HT" && _InlineResponseMessage != 0)
        {
          unsigned int _dbgIRM_NCL = 0;
          _InlineResponseMessage->GetNumberofCommandLines(_dbgIRM_NCL);
          S << Offset << "(SPEC-DEBUG-HT: InlineResponseMessage after processing "
            << NCL << " CL(s) — total CLs = " << _dbgIRM_NCL
            << ", HasPayload = " << _InlineResponseMessage->HasPayloadFlag
            << ", PayloadSize = " << _InlineResponseMessage->PayloadSize
            << ")" << endl;
          S << Offset << "(BEGIN InlineResponseMessage)" << endl;
          S << *_InlineResponseMessage;
          S << Offset << "(END InlineResponseMessage)" << endl;
        }
#endif

        StopProcessingMessage = false;
      }
      else
      {
        S << "(ERROR: Invalid number of command lines)" << endl;

        Status = ERROR;
      }
    }
    else
    {
      S << "(ERROR: Unable to get the number of command lines)" << endl;

      Status = ERROR;
    }
  }
  else
  {
    S << "(ERROR: Unable to run a message that is not at the Messages container)" << endl;
  }

  return Status;
}

// Allocate and add an Action on Actions container
void Block::NewAction(const string _LN, Action*& _PA)
{
}

// Get an Action
int Block::GetAction(string _LN, Action*& _PA)
{
  return ERROR;
}

// Delete an Action
int Block::DeleteAction(string _LN)
{
  return ERROR;
}

// Auxiliary functions
int Block::StringToInt(string _String)
{
  stringstream ss(_String);

  int Temp = 0;

  ss >> Temp;

  return Temp;
}

string Block::IntToString(int _Int)
{
  stringstream ss;

  ss << _Int;

  return ss.str();
}

string Block::DoubleToString(double _Double)
{
  stringstream ss;

  ss << _Double;

  return ss.str();
}

// Converting string to double
double Block::StringToDouble(string _String)
{
  return atof(_String.c_str());
}

string Block::UnsignedIntToString(unsigned int _Int)
{
  string ret;
  stringstream ss;

  ss << _Int;

  ret = ss.str();

  return ret;
}

double Block::GetTime()
{
  struct timespec t;

  clock_gettime(CLOCK_MONOTONIC, &t);

  return ((t.tv_sec) + (double)(t.tv_nsec / 1e9));
}
