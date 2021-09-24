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

#ifndef _PROMPT_IOSTREAM_H
#define _PROMPT_IOSTREAM_H

#ifndef _IOSTREAM_H
#include <iostream>
#endif

using namespace std;

typedef void (*OutFunct)(char);

class ConsoleStreambuf : public ::streambuf
{
	private:

	OutFunct OutputFunction;

	public:

    ConsoleStreambuf();

    ~ConsoleStreambuf();

    void SetOutputFunction(OutFunct OutputFunction_);

    OutFunct GetOutputFunction();

    virtual int overflow(int S);

    virtual int underflow(void){return 0;}
};

class ConsoleOstream : public ::ostream
{
	public:

      ConsoleStreambuf StreamBuf;

      ConsoleOstream(void (*OutputFunction_)(char)=NULL);

      OutFunct GetOutputFunction();

      void  SetOutputFunction(OutFunct OutputFunction_);
};

#endif
