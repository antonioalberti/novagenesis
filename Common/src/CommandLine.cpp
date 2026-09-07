/*
        NovaGenesis

        Name:		CommandLine
        Object:		CommandLine
        File:		CommandLine.cpp
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

#ifndef _COMMANDLINE_H
#include "CommandLine.h"
#endif

#ifndef _COMMANDLINEPARSER_H
#include "CommandLineParser.h"
#endif

CommandLine::CommandLine()
{
  Name = "";
  Alternative = "";
  Version = "";

  NoA = 0;
  Arguments = 0;
  NoE = 0;
}

CommandLine::CommandLine(string _Name, string _Alternative, string _Version)
{
  Name = _Name;
  Alternative = _Alternative;
  Version = _Version;

  NoA = 0;
  Arguments = 0;
  NoE = 0;
}

CommandLine::CommandLine(const CommandLine& CL)
{
  Name = CL.Name;
  Alternative = CL.Alternative;
  Version = CL.Version;

  NoA = CL.NoA;

  // Allocating the arguments
  Arguments = new string*[NoA];

  NoE = new unsigned int[NoA];

  // Copy the previous NoE array
  for (unsigned int m = 0; m < NoA; m++)
  {
    NoE[m] = CL.NoE[m];

    Arguments[m] = new string[NoE[m]];
  }

  // Copying the elements
  for (unsigned int n = 0; n < CL.NoA; n++)
  {
    for (unsigned int o = 0; o < CL.NoE[n]; o++)
    {
      Arguments[n][o] = CL.Arguments[n][o];
    }
  }
}

// SPEC-033 Phase A: deep copy assignment operator.
CommandLine& CommandLine::operator=(const CommandLine& CL)
{
  if (this == &CL)
  {
    return *this;
  }

  // Release current resources first.
  if (NoA > 0)
  {
    for (unsigned int i = 0; i < NoA; i++)
    {
      delete[] Arguments[i];
    }

    delete[] Arguments;

    delete[] NoE;

    Arguments = 0;

    NoE = 0;
  }

  Name = CL.Name;
  Alternative = CL.Alternative;
  Version = CL.Version;

  NoA = CL.NoA;

  Arguments = new string*[NoA];

  NoE = new unsigned int[NoA];

  for (unsigned int m = 0; m < NoA; m++)
  {
    NoE[m] = CL.NoE[m];

    Arguments[m] = new string[NoE[m]];
  }

  for (unsigned int n = 0; n < CL.NoA; n++)
  {
    for (unsigned int o = 0; o < CL.NoE[n]; o++)
    {
      Arguments[n][o] = CL.Arguments[n][o];
    }
  }

  return *this;
}

CommandLine::~CommandLine()
{
  // cout << "It has "<<NoA<<" arguments)"<<endl;

  if (NoA > 0)
  {
    for (unsigned int i = 0; i < NoA; i++)
    {
      // cout << "(Going to delete the argument number "<<i<<")"<<endl;

      delete[] Arguments[i];
    }

    // cout << "(Going to delete the arguments array)"<<endl;

    delete[] Arguments;

    // cout << "(Going to delete the number of elements array)"<<endl;

    delete[] NoE;
  }
}

void CommandLine::NewArgument(int _Size)
{
  string** A = new string*[NoA + 1];

  unsigned int* NE = new unsigned int[NoA + 1];

  // Copy the previous NoE array
  for (unsigned int m = 0; m < NoA; m++)
  {
    NE[m] = NoE[m];

    A[m] = new string[NE[m]];
  }

  // Copy the previous arguments elements
  for (unsigned int n = 0; n < NoA; n++)
  {
    for (unsigned int o = 0; o < NE[n]; o++)
    {
      A[n][o] = Arguments[n][o];
    }
  }

  // Set the new argument number of elements
  NE[NoA] = _Size;

  // Allocate the new argument
  A[NoA] = new string[NE[NoA]];

  if (NoA > 0)
  {
    // Delete the previous allocation
    for (unsigned int q = 0; q < NoA; ++q)
    {
      delete[] Arguments[q];
    }

    delete[] Arguments;

    delete[] NoE;
  }

  // Set the temporary array as the new arguments
  Arguments = A;

  // Set the temporary array of number of elements as the new array
  NoE = NE;

  // Update the NoA
  NoA++;

  // cout << "NoA = "<<NoA<<endl;
}

int CommandLine::GetArgument(unsigned int _Index, vector<string>& _Argument)
{
  if (_Index < NoA)
  {
    for (unsigned int i = 0; i < NoE[_Index]; i++)
    {
      _Argument.push_back(Arguments[_Index][i]);
    }

    return OK;
  }

  return ERROR;
}

int CommandLine::GetNumberofArguments(unsigned int& _Number)
{
  _Number = NoA;

  return OK;
}

int CommandLine::GetNumberofArgumentElements(unsigned int _Index, unsigned int& _Number)
{
  if (_Index < NoA)
  {
    _Number = NoE[_Index];

    return OK;
  }

  return ERROR;
}

int CommandLine::SetArgumentElement(unsigned int _Index, unsigned int _Element, string _Value)
{
  if (_Index < NoA)
  {
    if (_Element < NoE[_Index])
    {
      Arguments[_Index][_Element] = _Value;

      return OK;
    }
    else
    {
      return ERROR;
    }
  }

  return ERROR;
}

int CommandLine::GetArgumentElement(unsigned int _Index, unsigned int _Element, string& _Value) const
{
  if (_Index < NoA)
  {
    if (_Element < NoE[_Index])
    {
      _Value = Arguments[_Index][_Element];

      return OK;
    }

    return ERROR;
  }

  return ERROR;
}

ostream& operator<<(ostream& os, const CommandLine& CL)
{
  string NG = "ng ";
  string SP = " ";
  string NL = "\n";
  string OA = "[ ";
  string CA = "]";
  string OV = "< ";
  string CV = ">";
  unsigned int _Size;

  os << NG << CL.Name << SP << CL.Alternative << SP << CL.Version;

  if (CL.NoA > 0)
  {
    os << SP << OA;

    // Run over the number of arguments
    for (unsigned int i = 0; i < CL.NoA; i++)
    {
      os << OV;

      stringstream out;

      _Size = CL.NoE[i];

      // cout << endl <<"(The command line has "<<_Size<<" arguments)"<< endl;

      out << _Size;

      os << out.str() << SP << "s ";

      // cout << endl <<"(The argument has "<<PV1->size()<<" elements)"<< endl;

      for (unsigned int j = 0; j < CL.NoE[i]; j++)
      {
        string E;

        E = CL.Arguments[i][j];

        os << E << SP;
      }

      os << CV << SP;
    }

    os << CA << NL;
  }

  if (CL.NoA == 0)
  {
    os << NL;
  }

  return os;
}

// SPEC-033 Phase A: this operator now delegates to the single parser.
// Kept as an API adapter (Astra review: do not remove operator>> yet).
// Parse failures set failbit on the stream.
istringstream& operator>>(istringstream& iss, CommandLine& CL)
{
  std::string Line((std::istreambuf_iterator<char>(iss)), std::istreambuf_iterator<char>());

  int ErrorCode = CLP_OK;
  size_t ErrorOffset = 0;

  if (CommandLineParser::Parse(Line, CL, ErrorCode, ErrorOffset) != OK)
  {
    iss.setstate(std::ios::failbit);
  }

  return iss;
}

// Example of command line
// ng -p --notify _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > < _PubNotifySize string pub HID OSID PID BID > ... < _SubNotifySize string sub HID OSID PID BID > ]
// SPEC-033 Phase A: this method now delegates to the single parser.
// Kept for API compatibility (callers check the return value).
int CommandLine::ConvertCommandLineFromCharArray(char* _CL, int _Size)
{
  if (_CL == NULL || _Size <= 0)
  {
    return ERROR;
  }

  std::string Input(_CL, _Size);

  int ErrorCode = CLP_OK;
  size_t ErrorOffset = 0;

  return CommandLineParser::Parse(Input, *this, ErrorCode, ErrorOffset) == OK ? OK : ERROR;
}

// Auxiliary functions
int CommandLine::StringToInt(string _String)
{
  stringstream ss(_String);

  int Temp = 0;

  ss >> Temp;

  return Temp;
}
