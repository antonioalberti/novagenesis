/*
	NovaGenesis
	
	Name:		CommandLine
	Object:		CommandLine
	File:		CommandLine.h
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
#define _COMMANDLINE_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _SSTREAM_H
#include <sstream>
#endif

#ifndef _CTYPE_H
#include <ctype.h>
#endif

#define ERROR 1
#define OK 0

using namespace std;

// Example
// ng -sub --bind _Version [ < 1 string _Category > < _SCNsSize string S_1 ... S_SCNsSize > ]
class CommandLine
{
	public:

		// Command Name
		string 							Name;  // ng -msg

		// Command Alternative
		string 							Alternative; // --cl

		// Command Version
		string 							Version; // 0.1

		// Arguments and elements container
		string							**Arguments;
		
		// Number of arguments
		unsigned int					NoA;

		// Array of the number of elements per argument
		unsigned int 					*NoE;

		// Empty Constructor
        CommandLine();

        // Constructor
        CommandLine(string _Name, string _Alternative, string _Version);

        // Copy constructor
        CommandLine(const CommandLine &CL);

		// Destructor
		~CommandLine();

		// Allocates and add a string vector on Arguments container
		void NewArgument(int _Size);

		// Get argument
		int GetArgument(unsigned int _Index, vector<string> & _Argument);

		// Get number of arguments
		int GetNumberofArguments(unsigned int &_Number);

		// Get number of elements in a certain argument
		int GetNumberofArgumentElements(unsigned int _Index, unsigned int &_Number);

		// Set an element at an Argument
		int SetArgumentElement(unsigned int _Index, unsigned int _Element, string _Value);

		// Get an element at an Argument
		int GetArgumentElement(unsigned int _Index, unsigned int _Element, string &_Value) const;

		// Overloading ostream operator
		friend ostream&  operator<< (ostream& os, const CommandLine& CL);

		// Overloading istringstream operator
		friend istringstream&  operator>> (istringstream& iss, CommandLine& CL);

		// New function added to avoid the istringstream operator above
		int ConvertCommandLineFromCharArray(char *_CL, int _Size);

		// ------------------------------------------------------------------------------------------------------------------------------
		// Auxiliary functions
		// ------------------------------------------------------------------------------------------------------------------------------

		int StringToInt(string _String);
};

#endif






