/*
	NovaGenesis

	Name:		EPGS novagenesis command
	Object:		ng_command
	File:		ng_command.c
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

#include "ng_command.h"
#include "../epgs_wrapper.h"
#include "../epgs_defines.h"
#include "../ng_util/ng_util.h"

NgCommand *ng_create_command (char *_Name, char *_Alternative, char *_Version)
{
  struct _ng_command *p_command = (struct _ng_command *)ng_malloc (sizeof (NgCommand) * 1);

  p_command->Name = (char *)ng_malloc (sizeof (char) * (ng_strlen (_Name) + 1));
  ng_strcpy (p_command->Name, _Name);

  p_command->Alternative = (char *)ng_malloc (sizeof (char) * (ng_strlen (_Alternative) + 1));
  ng_strcpy (p_command->Alternative, _Alternative);

  ng_strcpy (p_command->Version, _Version);

  p_command->NoA = 0;
  //p_command->Arguments = (struct _ng_arguments**)ng_malloc(sizeof(struct _ng_arguments*));
  p_command->Arguments = NULL;

  return p_command;
}

void SetVersion (struct _ng_command *ngCommand, char *_Version)
{
  ng_strcpy (ngCommand->Version, _Version);
}

void ng_destroy_command (struct _ng_command **ngCommand)
{

  if ((*ngCommand)->NoA > 0)
	{
	  unsigned int i = 0;
	  unsigned int j = 0;
	  for (i = 0; i < (*ngCommand)->NoA; i++)
		{
		  if ((*ngCommand)->Arguments[i])
			{
			  for (j = 0; j < (*ngCommand)->Arguments[i]->NoE; j++)
				{
				  if ((*ngCommand)->Arguments[i]->Elements[j])
					{
					  ng_free ((*ngCommand)->Arguments[i]->Elements[j]);
					  (*ngCommand)->Arguments[i]->Elements[j] = NULL;
					}
				}
			  ng_free ((*ngCommand)->Arguments[i]->Elements);
			  (*ngCommand)->Arguments[i]->Elements = NULL;
			  ng_free ((*ngCommand)->Arguments[i]);
			  (*ngCommand)->Arguments[i] = NULL;
			}
		}

	  ng_free ((*ngCommand)->Arguments);
	  (*ngCommand)->Arguments = NULL;
	}
  ng_free ((*ngCommand)->Name);
  (*ngCommand)->Name = NULL;
  ng_free ((*ngCommand)->Alternative);
  (*ngCommand)->Alternative = NULL;

  ng_free ((*ngCommand));
}

// Allocates and add a string vector on Arguments container
void NewArgument (struct _ng_command *ngCommand, int _Size)
{

  ngCommand->Arguments = (struct _ng_arguments **)ng_realloc (ngCommand->Arguments,
															  (ngCommand->NoA + 1) * sizeof (struct _ng_arguments *));

  ngCommand->Arguments[ngCommand->NoA] = (struct _ng_arguments *)ng_malloc (sizeof (struct _ng_arguments) * 1);
  ngCommand->Arguments[ngCommand->NoA]->NoE = _Size;
  ngCommand->Arguments[ngCommand->NoA]->Elements = (char **)ng_malloc (sizeof (char *) * _Size);

  ngCommand->NoA++;
}

// Get argument
int GetArgument (struct _ng_command *ngCommand, unsigned int _Index, struct _ng_arguments **_Argument)
{

  if (_Index < ngCommand->NoA)
	{
	  unsigned int i = 0;
	  struct _ng_arguments *Argument = (struct _ng_arguments *)ng_malloc (sizeof (struct _ng_arguments) * 1);
	  Argument->Elements = (char **)ng_malloc (sizeof (char *) * ngCommand->Arguments[_Index]->NoE);
	  Argument->NoE = ngCommand->Arguments[_Index]->NoE;

	  for (i = 0; i < ngCommand->Arguments[_Index]->NoE; i++)
		{
		  Argument->Elements[i] = (char *)ng_malloc (
			  sizeof (char) * (ng_strlen (ngCommand->Arguments[_Index]->Elements[i] + 1)));
		  ng_strcpy (Argument->Elements[i], ngCommand->Arguments[_Index]->Elements[i]);
		}

	  *_Argument = Argument;
	  return NG_OK;
	}

  return NG_ERROR;
}

// Get number of arguments
int GetNumberofArguments (struct _ng_command *ngCommand, unsigned int *_Number)
{

  *_Number = ngCommand->NoA;
  return NG_OK;
}

// Get number of elements in a certain argument
int GetNumberofArgumentElements (struct _ng_command *ngCommand, unsigned int _Index, unsigned int *_Number)
{
  if (_Index < ngCommand->NoA)
	{
	  *_Number = ngCommand->Arguments[_Index]->NoE;

	  return NG_OK;
	}

  return NG_ERROR;
}

// Set an element at an Argument
int SetArgumentElement (struct _ng_command *ngCommand, unsigned int _Index, unsigned int _Element, char *_Value)
{

  if (_Index < ngCommand->NoA)
	{
	  if (_Element < ngCommand->Arguments[_Index]->NoE)
		{
		  ngCommand->Arguments[_Index]->Elements[_Element] = (char *)ng_malloc (
			  sizeof (char) * (ng_strlen (_Value) + 1));
		  ng_strcpy (ngCommand->Arguments[_Index]->Elements[_Element], _Value);

		  return NG_OK;
		}
	  else
		{
		  return NG_ERROR;
		}
	}

  return NG_ERROR;
}

// Get an element at an Argument
int GetArgumentElement (struct _ng_command *ngCommand, unsigned int _Index, unsigned int _Element, char **_Value)
{
  if (_Index < ngCommand->NoA)
	{
	  if (_Element < ngCommand->Arguments[_Index]->NoE)
		{
		  *_Value = (char *)ng_malloc (
			  sizeof (char) * (ng_strlen (ngCommand->Arguments[_Index]->Elements[_Element]) + 1));
		  ng_strcpy (*_Value, ngCommand->Arguments[_Index]->Elements[_Element]);

		  return NG_OK;
		}

	  return NG_ERROR;
	}

  return NG_ERROR;
}

char *ConvertNgCommandToString (struct _ng_command *CL)
{
  char NG[4] = "ng ";
  char SP[2] = " ";
  char NL[2] = "\n";
  char OA[3] = "[ ";
  char CA[2] = "]";
  char OV[3] = "< ";
  char CV[2] = ">";
  unsigned int _Size;

  char *output = (char *)ng_calloc (sizeof (char) * 1, 512);

  ng_sprintf (output + ng_strlen (output), "%s%s%s%s%s%s", NG, CL->Name, SP, CL->Alternative, SP, CL->Version);

  if (CL->NoA > 0)
	{
	  ng_sprintf (output + ng_strlen (output), "%s%s", SP, OA);

	  // Run over the number of arguments
	  unsigned int i = 0;
	  for (i = 0; i < CL->NoA; i++)
		{

		  ng_sprintf (output + ng_strlen (output), "%s", OV);

		  _Size = CL->Arguments[i]->NoE;

		  ng_sprintf (output + ng_strlen (output), "%u%ss ", _Size, SP);

		  unsigned int j = 0;
		  for (j = 0; j < CL->Arguments[i]->NoE; j++)
			{
			  ng_sprintf (output + ng_strlen (output), "%s%s", CL->Arguments[i]->Elements[j], SP);
			}

		  ng_sprintf (output + ng_strlen (output), "%s%s", CV, SP);

		}

	  ng_sprintf (output + ng_strlen (output), "%s%s", CA, NL);
	}

  if (CL->NoA == 0)
	{
	  ng_sprintf (output + ng_strlen (output), "%s", NL);
	}

  return output;
}

NgCommand *ConvertStringToNgCommand (char *inputStr)
{
  int Size;
  unsigned int Argument_Number = 0;
  NgCommand *CL = NULL;

  char *auxStr = (char *)ng_malloc (sizeof (char) * (ng_strlen (inputStr) + 1));
  ng_strcpy (auxStr, inputStr);

  char *end_str;
  char *token = strtok_r (auxStr, " ", &end_str);

  if (ng_strcmp (token, "ng") == 0)
	{
	  token = strtok_r (NULL, " ", &end_str);
	  char *name = (char *)ng_malloc (sizeof (char) * (ng_strlen (token) + 1));
	  ng_strcpy (name, token);
	  token = strtok_r (NULL, " ", &end_str);
	  char *alternative = (char *)ng_malloc (sizeof (char) * (ng_strlen (token) + 1));
	  ng_strcpy (alternative, token);
	  token = strtok_r (NULL, " ", &end_str);
	  char *version = (char *)ng_malloc (sizeof (char) * (ng_strlen (token) + 1));
	  ng_strcpy (version, token);

	  CL = ng_create_command (name, alternative, version);

	  ng_free (name);
	  ng_free (alternative);
	  ng_free (version);

	  // Reads the [
	  token = strtok_r (NULL, " ", &end_str);

	  if (token && ng_strcmp (token, "[") == 0 && ng_strcmp (CL->Name, "") != 0 && ng_strcmp (CL->Alternative, "") != 0
		  && ng_strcmp (CL->Version, "") != 0 && ng_strcmp (token, "[<") != 0)
		{
		  while (token)
			{
			  token = strtok_r (NULL, " ", &end_str); // Reads the <

			  if (token && ng_strcmp (token, "<") == 0 && ng_strcmp (token, "<1") != 0 && ng_strcmp (token, "<2") != 0
				  && ng_strcmp (token, "<3") != 0 && ng_strcmp (token, "]") != 0)
				{
				  token = strtok_r (NULL, " ", &end_str);  // Reads the number

				  if (token)
					{

					  Size = ng_atoi (token);

					  if (Size > 0)
						{
						  // New argument
						  NewArgument (CL, Size);

						  token = strtok_r (NULL, " ", &end_str);  // Reads the type

						  if (token && (ng_strcmp (token, "s") == 0 || ng_strcmp (token, "h") == 0
										|| ng_strcmp (token, "i") == 0)
							  && (ng_strcmp (token, "1s") != 0 && ng_strcmp (token, "2s") != 0
								  && ng_strcmp (token, "3s") != 0))
							{
							  int i = 0;
							  for (i = 0; i < Size; i++)
								{
								  token = strtok_r (NULL, " ", &end_str);  // Reads the value

								  if (token && ng_strcmp (token, "") != 0 && ng_strcmp (token, ">") != 0
									  && ng_strcmp (token, "<") != 0 && ng_strcmp (token, "]") != 0
									  && ng_strcmp (token, " ") != 0)
									{
									  // New element
									  SetArgumentElement (CL, Argument_Number, i, token);
									}
								  else
									{
									  break;
									}
								}

							  token = strtok_r (NULL, " ", &end_str); // Reads the >
							}
						  else
							{
							  break;
							}
						}
					}
				}
			  else
				{
				  break;
				}

			  Argument_Number++;
			}
		}
	}

  token = strtok_r (NULL, " ", &end_str); // Reads the ]

  ng_free (auxStr);

  return CL;
}
