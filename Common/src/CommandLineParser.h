/*
        NovaGenesis

        Name:           CommandLineParser
        Object:         CommandLineParser
        File:           CommandLineParser.h
        Author:         Antonio Marcos Alberti / Hermes (SPEC-033 Phase A)
        Date:           09/2026
        Version:        0.1

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

// SPEC-033 Phase A: single CommandLine parser with a formal error contract.
// Replaces the two divergent readers (operator>> and ConvertCommandLineFromCharArray).
//
// Contract (per Astra review):
//   - On SUCCESS the parsed CommandLine replaces `out` (built in a temporary,
//     committed only at the end). `out` is left UNTOUCHED on any error.
//   - On ERROR, out_error_code and out_error_offset describe the failure.
//   - Legacy type tokens s/h/i are accepted as pure syntax and ignored.
//   - Element values containing markers (<, >, [, ]) are rejected — the legacy
//     writer cannot represent them and the old parsers corrupted them silently.

#ifndef _COMMANDLINEPARSER_H
#define _COMMANDLINEPARSER_H

#include <string>

#ifndef _COMMANDLINE_H
#include "CommandLine.h"
#endif

// Error codes returned in out_error_code
#define CLP_OK 0
#define CLP_ERR_EMPTY 1          // Empty input
#define CLP_ERR_PREFIX 2         // Missing "ng " prefix
#define CLP_ERR_HEADER 3         // Malformed Name/Alternative/Version
#define CLP_ERR_MARKER 4         // Missing/mismatched [ ] or < >
#define CLP_ERR_SIZE 5           // Malformed or out-of-range cardinality
#define CLP_ERR_TYPE 6           // Unknown type token (only s/h/i accepted)
#define CLP_ERR_ELEMENTS 7       // Fewer elements than declared
#define CLP_ERR_UNCLOSED 8       // Missing > or ]
#define CLP_ERR_TRAILING 9       // Non-whitespace trailing garbage
#define CLP_ERR_ELEMENT_CHARS 10 // Element contains a reserved marker char
#define CLP_ERR_LIMIT 11         // Exceeds a configured limit

// Limits (SPEC-033 Fase A; revogaveis por constantes nomeadas)
#define CLP_MAX_LINE 65536     // Max command line bytes
#define CLP_MAX_ARGS 256       // Max argument vectors per command line
#define CLP_MAX_ELEMENTS 4096  // Max elements per argument vector
#define CLP_MAX_ELEMENT 4096   // Max bytes per element

class CommandLineParser
{
public:
  // Parse one command line. Returns CLP_OK and replaces `out` on success;
  // returns an error code and leaves `out` untouched on failure.
  static int Parse(const std::string& _Input, CommandLine& _Out, int& _ErrorCode, size_t& _ErrorOffset);

  // Human-readable error string (for logging).
  static const char* ErrorString(int _ErrorCode);
};

#endif
