/*
	NovaGenesis

	Name:		Name binding testing application. Publishes a large amount of NBs and then subscribes back only one per second.
	Object:		App
	File:		App.h
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

#include "NBTestApp.h"

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
		  cout << "*  NovaGenesis(NG) Name Binding Simple Test Application v0.1      *" << endl;
		  cout << "*  Copyright Antonio Marcos Alberti - Inatel - July 2015              *" << endl;
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
		  NBTestApp execApp ("NBTestApp", Key, Path);
		}
	  else
		{
		  Problem = true;

		  cout << "(ERROR: Wrong number of main() arguments)" << endl;
		}
	}
  else
	{
	  Problem = true;

	  cout << "(ERROR: No argument supplied)" << endl;
	}

  if (Problem == true)
	{
	  cout << "(Usage: ./App Path)" << endl;
	  cout << "(Path example: /home/myprofile/workspace/novagenesis/IO/App/)" << endl;
	}

  return 0;
}




