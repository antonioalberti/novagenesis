/*
	NovaGenesis

	Name:		EPGS novagenesis message
	Object:		ng_message
	File:		ng_message.c
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

#include "ng_message.h"
#include "../epgs_wrapper.h"
#include "../ng_util/ng_util.h"

void ng_create_message (double _Time, short _Type, bool _HasPayload, NgMessage **ngMessage)
{
  struct _ng_message *p_message = (struct _ng_message *)ng_malloc (sizeof (struct _ng_message));

  p_message->Time = _Time;
  p_message->Type = _Type;
  p_message->HasPayloadFlag = _HasPayload;

  p_message->PayloadSize = 0;
  p_message->MessageSize = 0;

  //Header=0;
  p_message->Payload = 0;
  p_message->Msg = 0;

  p_message->CommandLines = NULL;
  p_message->NoCL = 0;

  *ngMessage = p_message;
}

void ng_destroy_message (NgMessage **ngMessage)
{

  if (!(*ngMessage))
	{
	  return;
	}

  if ((*ngMessage)->NoCL > 0)
	{
	  int i = 0;
	  for (i = 0; i < (*ngMessage)->NoCL; i++)
		{
		  ng_destroy_command (&(*ngMessage)->CommandLines[i]);
		}
	  ng_free ((*ngMessage)->CommandLines);
	  (*ngMessage)->CommandLines = NULL;
	}
  if ((*ngMessage)->Payload)
	{
	  ng_free ((*ngMessage)->Payload);
	  (*ngMessage)->Payload = NULL;
	}
  if ((*ngMessage)->Msg)
	{
	  ng_free ((*ngMessage)->Msg);
	  (*ngMessage)->Msg = NULL;
	}
  ng_free ((*ngMessage));
  (*ngMessage) = NULL;
}

int NewCommandLine (struct _ng_message *ngMessage, NgCommand *CL)
{

  if (!ngMessage || !CL)
	{
	  return NG_ERROR;
	}
  ngMessage->CommandLines = (struct _ng_command **)ng_realloc (ngMessage->CommandLines,
															   (ngMessage->NoCL + 1) * sizeof (struct _ng_command *));

  ngMessage->CommandLines[ngMessage->NoCL] = CL;

  ngMessage->NoCL++;

  return NG_OK;
}

// Get a CommandLine by its index
int GetCommandLine (struct _ng_message *ngMessage, unsigned int _Index, NgCommand **CL)
{

  if (_Index < ngMessage->NoCL)
	{
	  *CL = ngMessage->CommandLines[_Index];
	  return NG_OK;
	}
  return NG_ERROR;
}

// Set *Payload from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
int SetPayloadFromCharArray (struct _ng_message *ngMessage, char *_Value, int _Size)
{
  int Status = NG_ERROR;

  if (ngMessage->PayloadSize == 0)
	{
	  if (_Size > 0)
		{
		  ngMessage->PayloadSize = _Size;

		  ngMessage->Payload = (char *)ng_calloc (sizeof (char) * 1, ngMessage->PayloadSize);

		  int j = 0;
		  for (j = 0; j < ngMessage->PayloadSize; j++)
			{
			  ngMessage->Payload[j] = _Value[j];
			}

		  Status = NG_OK;

		  ngMessage->HasPayloadFlag = true;
		}
	}

  return Status;
}

// Set *Msg from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
int SetMessageFromCharArray (struct _ng_message *ngMessage, char *_Value, int _Size)
{
  int Status = NG_ERROR;

  if (ngMessage->MessageSize == 0)
	{
	  if (_Size > 0)
		{
		  ngMessage->MessageSize = _Size;

		  ngMessage->Msg = (char *)ng_calloc (sizeof (char) * 1, ngMessage->MessageSize);

		  int j = 0;
		  for (j = 0; j < ngMessage->MessageSize; j++)
			{
			  ngMessage->Msg[j] = _Value[j];
			}

		  Status = NG_OK;
		}
	}

  return Status;
}

int ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray (struct _ng_message **ngMessage)
{
  int Status = NG_ERROR;

  long long msgLen = (*ngMessage)->NoCL * 512 + (*ngMessage)->PayloadSize;

  char *output = (char *)ng_calloc (sizeof (char) * 1, msgLen);

  int msgIndex = 0;

  if ((*ngMessage)->MessageSize == 0)
	{
	  if ((*ngMessage)->NoCL > 0)
		{
		  unsigned int i = 0;
		  for (i = 0; i < (*ngMessage)->NoCL; i++)
			{
			  char *out = ConvertNgCommandToString ((*ngMessage)->CommandLines[i]);
			  ng_sprintf (output + msgIndex, "%s", out);
			  msgIndex += ng_strlen (out);
			  ng_free (out);
			}

		  if ((*ngMessage)->HasPayloadFlag == true && (*ngMessage)->PayloadSize > 0)
			{
			  ng_sprintf (output + msgIndex, "%s", "\n"); // Add a blank line to separate the payload from the header.
			  msgIndex++;

			  int i = 0;
			  for (i = 0; i < (*ngMessage)->PayloadSize; i++)
				{
				  output[msgIndex] = (*ngMessage)->Payload[i];
				  msgIndex++;
				}
			}

		  (*ngMessage)->MessageSize = msgIndex;

		  (*ngMessage)->Msg = (char *)ng_calloc ((*ngMessage)->MessageSize + 1, sizeof (char) * 1);

		  int j = 0;
		  for (j = 0; j < (*ngMessage)->MessageSize; j++)
			{
			  (*ngMessage)->Msg[j] = output[j];
			}

		  Status = NG_OK;
		}
	}
  ng_free (output);

  return Status;
}

void MessageToString (struct _ng_message *ngMessage, char **out)
{
  char *output = NULL;

  int msgLen = ngMessage->NoCL * 512 + ngMessage->PayloadSize;

  if (ngMessage->NoCL > 0)
	{
	  output = (char *)ng_calloc (sizeof (char) * 1, msgLen);
	  unsigned int i = 0;
	  for (i = 0; i < ngMessage->NoCL; i++)
		{
		  char *outAux = ConvertNgCommandToString (ngMessage->CommandLines[i]);
		  ng_sprintf (output + ng_strlen (output), "%s", outAux);
		  ng_free (outAux);
		}

	  if (ngMessage->HasPayloadFlag == true && ngMessage->PayloadSize > 0)
		{
		  ng_sprintf (output + ng_strlen (output), "There is a payload of %lld bytes\n", ngMessage
			  ->PayloadSize); // Add a blank line to separate the payload from the header.
		}
	}
  *out = output;
}

void MessageFromString (const char *in, int msgSize, struct _ng_message **ngMessage)
{

  int msgLen = msgSize;
  int position = 0;
  char *end_str;

  NgMessage *message = NULL;
  ng_create_message (ng_GetTime (), 0, false, &message);

  char *aux_token = (char *)ng_malloc (sizeof (char) * msgLen + 1);
  ng_strncpy (aux_token, in, msgLen);

  char *token = strtok_r (aux_token, "\n", &end_str);

  while (token)
	{
	  if (token[0] == 'n' && token[1] == 'g' && token[2] == ' ' && token[3] == '-')
		{
		  NgCommand *CL = ConvertStringToNgCommand (token);
		  NewCommandLine (message, CL);
		  position = position + ng_strlen (token) + 1;
		}
	  else
		{
		  position = position + 1;
		  break;
		}
	  token = strtok_r (NULL, "\n", &end_str);

	}

  if (token && position < msgLen)
	{
	  int payloadSize = msgLen - position;
	  message->PayloadSize = payloadSize;
	  message->Payload = (char *)ng_calloc (sizeof (char) * 1, payloadSize);
	  ng_strncpy (message->Payload, token, message->PayloadSize);
	  message->HasPayloadFlag = true;
	}
  ng_free (aux_token);

  *ngMessage = message;
}
