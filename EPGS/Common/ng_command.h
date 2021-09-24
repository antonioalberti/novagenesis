/*
	NovaGenesis

	Name:		EPGS novagenesis command
	Object:		ng_command
	File:		ng_command.h
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

#ifndef COMMAND_NG_COMMAND_H_
#define COMMAND_NG_COMMAND_H_

struct _ng_arguments {
  char **Elements;
  unsigned int NoE;
};

struct _ng_command {
  // Command Name
  char *Name;  // ng -msg

  // Command Alternative
  char *Alternative; // --cl

  // Command Version
  char Version[5]; // 0.1

  // Arguments and elements container
  struct _ng_arguments **Arguments;

  // Number of arguments
  unsigned int NoA;

};

typedef struct _ng_command NgCommand;
typedef struct _ng_arguments NgArguments;

NgCommand *ng_create_command (char *_Name, char *_Alternative, char *_Version);

void SetVersion (struct _ng_command *ngCommand, char *_Version);

void ng_destroy_command (struct _ng_command **ngCommand);

// Allocates and add a string vector on Arguments container
void NewArgument (struct _ng_command *ngCommand, int _Size);

// Get argument
int GetArgument (struct _ng_command *ngCommand, unsigned int _Index, NgArguments **_Argument);

// Get number of arguments
int GetNumberofArguments (struct _ng_command *ngCommand, unsigned int *_Number);

// Get number of elements in a certain argument
int GetNumberofArgumentElements (struct _ng_command *ngCommand, unsigned int _Index, unsigned int *_Number);

// Set an element at an Argument
int SetArgumentElement (struct _ng_command *ngCommand, unsigned int _Index, unsigned int _Element, char *_Value);

// Get an element at an Argument
int GetArgumentElement (struct _ng_command *ngCommand, unsigned int _Index, unsigned int _Element, char **_Value);

char *ConvertNgCommandToString (struct _ng_command *CL);

NgCommand *ConvertStringToNgCommand (char *inputStr);

#endif /* COMMAND_NG_COMMAND_H_ */
