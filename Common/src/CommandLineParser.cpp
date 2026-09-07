/*
        NovaGenesis

        Name:           CommandLineParser
        Object:         CommandLineParser
        File:           CommandLineParser.cpp
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

// SPEC-033 Phase A: single CommandLine parser. See header for the contract.

#ifndef _COMMANDLINEPARSER_H
#include "CommandLineParser.h"
#endif

#include <cctype>

namespace
{

// Tokenizer: returns the next whitespace-delimited token starting at io_pos.
// Returns false at end of input.
bool NextToken(const std::string& _In, size_t& _Pos, std::string& _Tok)
{
  while (_Pos < _In.size() && std::isspace(static_cast<unsigned char>(_In[_Pos])) != 0)
  {
    _Pos++;
  }

  if (_Pos >= _In.size())
  {
    return false;
  }

  size_t Begin = _Pos;

  while (_Pos < _In.size() && std::isspace(static_cast<unsigned char>(_In[_Pos])) == 0)
  {
    _Pos++;
  }

  _Tok = _In.substr(Begin, _Pos - Begin);

  return true;
}

// Peek: next token without consuming; "" at end of input.
std::string PeekToken(const std::string& _In, size_t _Pos)
{
  std::string Tok;
  size_t P = _Pos;

  NextToken(_In, P, Tok);

  return Tok;
}

// Strict decimal cardinality: all chars digits, fits int, no sign/garbage.
bool ParseCount(const std::string& _Tok, int& _Value)
{
  if (_Tok.empty() || _Tok.size() > 9)
  {
    return false;
  }

  for (char C : _Tok)
  {
    if (std::isdigit(static_cast<unsigned char>(C)) == 0)
    {
      return false;
    }
  }

  _Value = std::stoi(_Tok);

  return true;
}

bool ContainsMarkerChar(const std::string& _Tok)
{
  return _Tok.find('<') != std::string::npos ||
         _Tok.find('>') != std::string::npos ||
         _Tok.find('[') != std::string::npos ||
         _Tok.find(']') != std::string::npos;
}

} // namespace

int CommandLineParser::Parse(const std::string& _Input, CommandLine& _Out, int& _ErrorCode, size_t& _ErrorOffset)
{
  _ErrorCode = CLP_OK;
  _ErrorOffset = 0;

  // Global limits first (fail before any allocation).
  if (_Input.size() > CLP_MAX_LINE)
  {
    _ErrorCode = CLP_ERR_LIMIT;
    _ErrorOffset = CLP_MAX_LINE;

    return ERROR;
  }

  if (_Input.empty())
  {
    _ErrorCode = CLP_ERR_EMPTY;

    return ERROR;
  }

  // Temporary target: `out` is only replaced on full success.
  CommandLine Temp;
  size_t Pos = 0;
  std::string Tok;

  // Header: ng Name Alternative Version
  if (NextToken(_Input, Pos, Tok) == false || Tok != "ng")
  {
    _ErrorCode = CLP_ERR_PREFIX;

    return ERROR;
  }

  if (NextToken(_Input, Pos, Tok) == false || Tok.empty() || ContainsMarkerChar(Tok))
  {
    _ErrorCode = CLP_ERR_HEADER;

    return ERROR;
  }

  Temp.Name = Tok;

  if (NextToken(_Input, Pos, Tok) == false || Tok.empty() || Tok[0] != '-' || ContainsMarkerChar(Tok))
  {
    _ErrorCode = CLP_ERR_HEADER;

    return ERROR;
  }

  Temp.Alternative = Tok;

  if (NextToken(_Input, Pos, Tok) == false || Tok.empty() || ContainsMarkerChar(Tok))
  {
    _ErrorCode = CLP_ERR_HEADER;

    return ERROR;
  }

  Temp.Version = Tok;

  // Argument block: optional [ ... ] (legacy writer emits no [] when NoA==0,
  // and the legacy array parser required them — here both forms are accepted
  // as valid legacy inputs, closing divergence P9).
  std::string Peek = PeekToken(_Input, Pos);

  if (Peek == "[")
  {
    NextToken(_Input, Pos, Tok); // consume [

    unsigned int ArgIndex = 0;

    while (true)
    {
      Peek = PeekToken(_Input, Pos);

      if (Peek == "]")
      {
        NextToken(_Input, Pos, Tok); // consume ]

        break;
      }

      if (Peek != "<")
      {
        _ErrorCode = CLP_ERR_MARKER;
        _ErrorOffset = Pos;

        return ERROR;
      }

      if (ArgIndex >= CLP_MAX_ARGS)
      {
        _ErrorCode = CLP_ERR_LIMIT;
        _ErrorOffset = Pos;

        return ERROR;
      }

      NextToken(_Input, Pos, Tok); // consume <

      // Cardinality (strict: no sign, no garbage).
      if (NextToken(_Input, Pos, Tok) == false)
      {
        _ErrorCode = CLP_ERR_UNCLOSED;
        _ErrorOffset = Pos;

        return ERROR;
      }

      int Count = 0;

      if (ParseCount(Tok, Count) == false || Count <= 0 || Count > CLP_MAX_ELEMENTS)
      {
        _ErrorCode = CLP_ERR_SIZE;
        _ErrorOffset = Pos;

        return ERROR;
      }

      // Optional legacy type token: s/h/i accepted as pure syntax.
      // SPEC-033: under the NEW format (no type) elements follow directly;
      // if the peek is not s/h/i we simply do not consume a type token.
      // (The legacy writer ALWAYS emits the type, so this is unambiguous.)
      Peek = PeekToken(_Input, Pos);

      if (Peek == "s" || Peek == "h" || Peek == "i")
      {
        NextToken(_Input, Pos, Tok); // consume type
      }

      Temp.NewArgument(Count);

      // Exactly Count elements, then >.
      for (int E = 0; E < Count; E++)
      {
        if (NextToken(_Input, Pos, Tok) == false || Tok == ">")
        {
          _ErrorCode = CLP_ERR_ELEMENTS;
          _ErrorOffset = Pos;

          return ERROR;
        }

        if (Tok == "<" || Tok == "[" || Tok == "]" || ContainsMarkerChar(Tok))
        {
          _ErrorCode = CLP_ERR_ELEMENT_CHARS;
          _ErrorOffset = Pos;

          return ERROR;
        }

        if (Tok.size() > CLP_MAX_ELEMENT)
        {
          _ErrorCode = CLP_ERR_LIMIT;
          _ErrorOffset = Pos;

          return ERROR;
        }

        Temp.SetArgumentElement(ArgIndex, E, Tok);
      }

      // Closing > required.
      if (NextToken(_Input, Pos, Tok) == false || Tok != ">")
      {
        _ErrorCode = CLP_ERR_UNCLOSED;
        _ErrorOffset = Pos;

        return ERROR;
      }

      ArgIndex++;
    }
  }

  // Trailing garbage check: only whitespace may remain.
  while (Pos < _Input.size())
  {
    if (std::isspace(static_cast<unsigned char>(_Input[Pos])) == 0)
    {
      _ErrorCode = CLP_ERR_TRAILING;
      _ErrorOffset = Pos;

      return ERROR;
    }

    Pos++;
  }

  // Commit: replace out only now (exception safety: CommandLine copy may
  // allocate; if it throws, out is still untouched).
  _Out = Temp;

  return OK;
}

const char* CommandLineParser::ErrorString(int _ErrorCode)
{
  switch (_ErrorCode)
  {
    case CLP_OK:
      return "ok";
    case CLP_ERR_EMPTY:
      return "empty input";
    case CLP_ERR_PREFIX:
      return "missing ng prefix";
    case CLP_ERR_HEADER:
      return "malformed Name/Alternative/Version";
    case CLP_ERR_MARKER:
      return "expected < or ]";
    case CLP_ERR_SIZE:
      return "malformed or out-of-range cardinality";
    case CLP_ERR_TYPE:
      return "unknown type token";
    case CLP_ERR_ELEMENTS:
      return "fewer elements than declared";
    case CLP_ERR_UNCLOSED:
      return "missing > or ]";
    case CLP_ERR_TRAILING:
      return "trailing garbage";
    case CLP_ERR_ELEMENT_CHARS:
      return "element contains reserved marker char";
    case CLP_ERR_LIMIT:
      return "limit exceeded";
    default:
      return "unknown error";
  }
}
