/*
	NovaGenesis

	Name:		Name Resolution and Network Cache Service
	Object:		execNRNCS
	File:		execNRNCS.cpp
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

#include "NRNCS.h"

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

int main (int argc, char *argv[])
{
  int R = 0;
  bool Problem = false;

  if (argc != 0)
	{
	  if (argc == 2)
		{
		  string Path = argv[1];

		  cout << "*******************************************************************" << endl;
		  cout << "*                                                                 *" << endl;
		  cout << "*  NovaGenesis(NG) Name Resolution and Network Cache Service v0.1 *" << endl;
		  cout << "*  Copyright Antonio Marcos Alberti - Inatel - April 2021         *" << endl;
		  cout << "*                                                                 *" << endl;
		  cout << "*                                                                 *" << endl;
		  cout << "*******************************************************************" << endl << endl;

		  cout << "(The I/O path is " << Path << ")" << endl;

		  long long Temp = (long long)&R;

		  // Initialize the random generator
		  srand ((unsigned int)Temp * time (NULL));

		  // Generates a random key
		  R = 1 + (rand () % 2147483647);

		  // Set the shm key
		  key_t Key = R;

		  // Create a process instance
		  NRNCS execNRS ("NRNCS", Key, Path);
		}
	  else
		{
		  cout << "(ERROR: Wrong number of main() arguments)" << endl;

		  Problem = true;
		}
	}
  else
	{
	  cout << "(ERROR: No argument supplied)" << endl;

	  Problem = true;
	}

  if (Problem == true)
	{
	  cout << "(Usage: ./NRNCS Path)" << endl;
	  cout << "(Path example: /home/myprofile/workspace/novagenesis/IO/NRNCS/)" << endl;
	}

  return 0;
}




