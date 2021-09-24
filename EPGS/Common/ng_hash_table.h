/*
	NovaGenesis

	Name:		EPGS novagenesis hash table
	Object:		ng_hash_table
	File:		ng_hash_table.h
	Author:		Vâner José Magalhães
	Date:		05/2021
	Version:	0.1

  	Copyright (C) 2021  Vâner José Magalhães

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

#ifndef COMMON_NG_HASH_TABLE_H_
#define COMMON_NG_HASH_TABLE_H_

struct _ng_hash_entry {
	// Command Name
	char*	Name;  // ng -msg
	int nameSize;

	// Command Alternative
	char*	Value; // --cl
	int valueSize;
};

typedef struct _ng_hash_entry NgHashEntry;

struct _ng_hash_table {
	struct _ng_hash_entry **entries;
	int category;
	int entriesCount;
};

typedef struct _ng_hash_entry NgHashEntry;
typedef struct _ng_hash_table NgHashTable;

void ng_hash_table_create(int category, NgHashTable** ngHashTable);

int ng_hash_table_get_position(NgHashTable *ngHashTable, char* Name);

int ng_hash_table_put(NgHashTable *ngHashTable, char* Name, int nameSize, char* Value, int valueSize);

int ng_hash_table_get(NgHashTable *ngHashTable, char* Name, char** Value);

#endif /* COMMON_NG_HASH_TABLE_H_ */
