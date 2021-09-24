/*
	NovaGenesis
	
	Name:		HashMultimaps
	Object:		HashMultimaps
	File:		HashMultimaps.h
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

#ifndef _HASHMULTIMAPS_H
#define _HASHMULTIMAPS_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _HASH_MAP_H
#include <hash_map>
#endif

#ifndef _ACTION_H
#include "Action.h"
#endif

#ifndef _HASH_BYTES_H
#include <bits/hash_bytes.h>
#endif

using namespace std;
using namespace __gnu_cxx;

// *****************************
// For string
// *****************************

class HashString {
 public:

  int operator() (std::string const &str) const
  {
	return __gnu_cxx::hash<char const *> () (str.c_str ());
  }
};

// This class' function operator() tests if two keys are equal
class HashStringCompare {
 public:

  bool operator() (string s1, string s2) const
  {
	return s1 == s2;
  }
};

// Define the hash_multimap
typedef hash_multimap <string, string, HashString, HashStringCompare> HashStringMultimap;

// Define an iterator for search the values behind a key
typedef HashStringMultimap::iterator Iterator;

// Define a pair of iterators
typedef pair<HashStringMultimap::iterator, HashStringMultimap::iterator> IteratorsPair;

// *****************************
// For Action
// *****************************

class Action;

// Define the hash_multimap
typedef hash_multimap<string, Action *, HashString, HashStringCompare> ActionsMultimap;

// Define an iterator for search the values behind a key
typedef ActionsMultimap::iterator ActionsIterator;

// Define a pair of iterators
typedef pair<ActionsMultimap::iterator, ActionsMultimap::iterator> ActionsIteratorsPair;

#endif






