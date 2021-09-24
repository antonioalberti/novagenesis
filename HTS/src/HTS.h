/*
	NovaGenesis
	
	Name:		Hash Table System
	Object:		HTS
	File:		HTS.h
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

#ifndef _HTS_H
#define _HTS_H

#include "Process.h"

#define ERROR 1
#define OK 0

class HTS : public Process {
 public:

  // Constructor
  HTS (string _LN, key_t _Key, string _Path);

  // Destructor
  ~HTS ();

  // Allocate a new block based on a name and add a Block on Blocks container
  int NewBlock (string _LN, Block *&_PB);
};

#endif






