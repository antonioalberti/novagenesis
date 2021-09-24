/*
	Name:		Simulator Input Output Stream
	Object:		Sim_Stream
	File:		sim_stream.cpp
	Authors:	Jackson Klein and Antonio Marcos Alberti
	Date:		02/2007
	Version:	0.3

 	Copyright (C) 2021  Jackson Klein and Antonio Marcos Alberti

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

#include "Prompt_iostream.h"

//  ConsoleStreambuf Class

ConsoleStreambuf::ConsoleStreambuf() : streambuf()
{
	OutputFunction=NULL;
};

ConsoleStreambuf::~ConsoleStreambuf()
{
};

void  ConsoleStreambuf::SetOutputFunction(OutFunct OutputFunction_)
{
	OutputFunction=OutputFunction_;
};

OutFunct ConsoleStreambuf::GetOutputFunction()
{
	return OutputFunction;
};


int ConsoleStreambuf::overflow(int S)
{
	cout<<(char)S;

 	//if(OutputFunction!=NULL) OutputFunction((char)S);

    //else cout<<(char)S;

	return 0;
}

//------------------------------------------------------------------------------

// ConsoleOstream Class

ConsoleOstream::ConsoleOstream(OutFunct OutputFunction_)
{
    init ( &StreamBuf);

	SetOutputFunction(OutputFunction_);
};


void  ConsoleOstream::SetOutputFunction(OutFunct OutputFunction_)
{
	StreamBuf.SetOutputFunction(OutputFunction_);
};

OutFunct ConsoleOstream::GetOutputFunction()
{
	return 	StreamBuf.GetOutputFunction();
};

