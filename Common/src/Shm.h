/*
	NovaGenesis
	
	Name:		Auxiliary shared memory access control object
	Object:		Shm
	File:		Shm.h
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

#ifndef _SHM_H
#define _SHM_H

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

class Shm {
 public:

  // The key to get a shmid
  key_t Key;

  // The keys being published
  int shmid;

  // Data initialization flag
  bool DataInitFlag;

  // shmget status
  bool ShmGetFlag;
};

#endif






