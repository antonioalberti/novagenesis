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

#ifndef _FILE_H
#include "File.h"
#endif

#include <cerrno>
#include <cctype>
#include <sys/stat.h>

#define DEBUG

static string BoundedCacheLogField(const string& Value)
{
  const size_t BodyLimit = 93; // leave room for the truncation marker
  const char Hex[] = "0123456789ABCDEF";
  string Result;
  Result.reserve(96);
  size_t i = 0;
  for (; i < Value.size() && Result.size() < BodyLimit; ++i)
  {
    unsigned char c = (unsigned char)Value[i];
    string Token;
    if (c >= 0x20 && c <= 0x7E && c != '\\')
      Token.assign(1, (char)c);
    else if (c == '\\')
      Token = "\\\\\\\\";
    else
    {
      Token = "\\\\x";
      Token += Hex[c >> 4];
      Token += Hex[c & 0x0F];
    }

    if (Result.size() + Token.size() > BodyLimit)
      break;
    Result += Token;
  }
  if (i < Value.size())
    Result += "...";
  return Result;
}

NRInfoPayload01::NRInfoPayload01(string _LN, Block* _PB, MessageBuilder* _PMB)
    : Action(_LN, _PB, _PMB)
{
}

NRInfoPayload01::~NRInfoPayload01()
{
}

// Run the actions behind a received command line
// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
//
// NG inverted pub/sub model:
//   This action does NOT forward the content. It ONLY caches the payload
//   to disk in the NRNCS cache path. The actual delivery (ng -d --b) is
//   triggered later when the subscriber sends ng -s --b and the HT serves
//   the cached file via HTGetBind01 (category 18).
int NRInfoPayload01::Run(Message* _ReceivedMessage, CommandLine* _PCL, vector<Message*>& ScheduledMessages, Message*& InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Values;
  char* Payload = 0;
  long long Size = 0;
  string CachePath;

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
                // ============================================================
                // Cache the payload to disk in the NRNCS path.
                // The HTGetBind01 (category 18) will read this file later
                // when the subscriber's ng -s --b triggers a ng -g --b.
                // ============================================================

                CachePath = PB->GetPath();

                // Check if file already exists (idempotent cache)
                {
                  File F;
                  if (F.OpenInputFile(Values.at(0), CachePath, "BINARY") == OK)
                  {
                    // File already cached — skip
                    F.CloseFile();
                    Status = OK;

#ifdef DEBUG
                    PB->S << Offset << "(Cache hit: " << Values.at(0) << " already exists at " << CachePath << ")" << endl;
#endif

                    return Status;
                  }
                }

                _ReceivedMessage->SetPayloadFileName(Values.at(0));
                _ReceivedMessage->SetPayloadFilePath(CachePath);
                _ReceivedMessage->SetPayloadFileOption("BINARY");
                int CacheWriteStatus = _ReceivedMessage->ConvertPayloadFromCharArrayToFile();

                // Bounded, behavior-neutral postcondition trace. The payload
                // hash is the NG content identity; the probe observes the
                // exact path used by File::OpenOutputFile (Path + Name).
                string PayloadHash;
                if (Payload != 0 && Size > 0)
                {
                  PayloadHash = NameGenerator::GetInstance().GenerateFromCharArray(
                      (const char*)Payload, Size);
                }
                string CachedFilePath = CachePath + Values.at(0);
                struct stat CachedStat;
                int CacheProbeStatus = stat(CachedFilePath.c_str(), &CachedStat);
                long long CachedSize = CacheProbeStatus == 0
                                           ? (long long)CachedStat.st_size
                                           : -1;
                int CacheProbeError = CacheProbeStatus == 0 ? 0 : errno;
                PB->S << Offset << "[SPEC033CACHE] key=" << BoundedCacheLogField(PayloadHash)
                      << " file=" << BoundedCacheLogField(Values.at(0))
                      << " payload_size=" << Size
                      << " cache_path=" << BoundedCacheLogField(CachedFilePath)
                      << " write_status=" << CacheWriteStatus
                      << " probe_status=" << CacheProbeStatus
                      << " probe_errno=" << CacheProbeError
                      << " cached_size=" << CachedSize << endl;

                // SPEC-019: Log hash for traceability. Reuse the hash computed
                // above so instrumentation adds no second payload traversal.
                {
                  PB->S << Offset << "(NRNCS cached payload: file="
                        << BoundedCacheLogField(Values.at(0))
                        << ", size=" << Size << " bytes, hash=" << PayloadHash << ")" << endl;
                }

#ifdef DEBUG
                PB->S << Offset << "(Cached file: " << Values.at(0) << ", size=" << Size << " bytes, at " << CachePath << ")" << endl;
#endif

                Status = OK;
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