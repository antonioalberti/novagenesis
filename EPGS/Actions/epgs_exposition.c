/*
	NovaGenesis

	Name:		EPGS exposition
	Object:		epgs_exposition
	File:		epgs_exposition.c
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

#include "epgs_exposition.h"
#include "../epgs_wrapper.h"
#include "../Common/ng_epgs_hash.h"

int actionExpostion (NgEPGS *ngEPGS, NgMessage **expositionMessage)
{
  int result = NG_OK;

  if (!ngEPGS || !ngEPGS->MyNetInfo || !ngEPGS->MyNetInfo->HID || !ngEPGS->MyNetInfo->OSID)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->PSSScnIDInfo || !ngEPGS->PSSScnIDInfo->HID || !ngEPGS->PSSScnIDInfo->OSID || !ngEPGS->PSSScnIDInfo->PID
	  || !ngEPGS->PSSScnIDInfo->BID)
	{
	  return NG_ERROR;
	}

  if (!ngEPGS->HwDescriptor || ngEPGS->HwDescriptor->keyWordsCounter <= 0 || !ngEPGS->HwDescriptor->keyWords)
	{
	  return NG_ERROR;
	}

  int len = 0;

  NgMessage *message = NULL;
  ng_create_message (ng_GetTime (), 1, true, &message);

  NgCommand *CL = ng_create_command ("-m", "--cl", "0.1");
  NewArgument (CL, 1);
  SetArgumentElement (CL, 0, 0, ngEPGS->MyNetInfo->LIMITER);

  NewArgument (CL, 4);
  SetArgumentElement (CL, 1, 0, ngEPGS->MyNetInfo->HID);
  SetArgumentElement (CL, 1, 1, ngEPGS->MyNetInfo->OSID);
  SetArgumentElement (CL, 1, 2, ngEPGS->MyNetInfo->PID);
  SetArgumentElement (CL, 1, 3, "NULL");

  NewArgument (CL, 4);
  SetArgumentElement (CL, 2, 0, ngEPGS->PSSScnIDInfo->HID);
  SetArgumentElement (CL, 2, 1, ngEPGS->PSSScnIDInfo->OSID);
  SetArgumentElement (CL, 2, 2, ngEPGS->PSSScnIDInfo->PID);
  SetArgumentElement (CL, 2, 3, ngEPGS->PSSScnIDInfo->BID);

  NewCommandLine (message, CL);

  int i = 0;
  for (i = 0; i < ngEPGS->HwDescriptor->keyWordsCounter; i++)
	{
	  if (!ngEPGS->HwDescriptor->keyWords[i])
		{
		  continue;
		}

	  len = ng_strlen (ngEPGS->HwDescriptor->keyWords[i]);
	  char *auxSCN = NULL;
	  GenerateSCNFromCharArrayBinaryPatterns4Bytes (ngEPGS->HwDescriptor->keyWords[i], len, &auxSCN);

	  NgCommand *bindingCL = ng_create_command ("-p", "--b", "0.1");
	  NewArgument (bindingCL, 1);
	  SetArgumentElement (bindingCL, 0, 0, "2"); // Category

	  NewArgument (bindingCL, 1);
	  SetArgumentElement (bindingCL, 1, 0, auxSCN); // Key

	  NewArgument (bindingCL, 1);
	  SetArgumentElement (bindingCL, 2, 0, ngEPGS->MyNetInfo->PID); // Process

	  NewCommandLine (message, bindingCL);

	  NgCommand *bindingNlnCL = ng_create_command ("-p", "--b", "0.1");

	  NewArgument (bindingNlnCL, 1);
	  SetArgumentElement (bindingNlnCL, 0, 0, "1"); // Category

	  NewArgument (bindingNlnCL, 1);
	  SetArgumentElement (bindingNlnCL, 1, 0, auxSCN); // Key

	  NewArgument (bindingNlnCL, 1);
	  SetArgumentElement (bindingNlnCL, 2, 0, ngEPGS->HwDescriptor->keyWords[i]); // Natural Language

	  NewCommandLine (message, bindingNlnCL);

	  ng_free (auxSCN);
	}

  NgCommand *binding_OS_P = ng_create_command ("-p", "--b", "0.1");
  NewArgument (binding_OS_P, 1);
  SetArgumentElement (binding_OS_P, 0, 0, "5"); // Category
  NewArgument (binding_OS_P, 1);
  SetArgumentElement (binding_OS_P, 1, 0, ngEPGS->MyNetInfo->OSID); // SO ID
  NewArgument (binding_OS_P, 1);
  SetArgumentElement (binding_OS_P, 2, 0, ngEPGS->MyNetInfo->PID); // Process ID
  NewCommandLine (message, binding_OS_P);

  NgCommand *binding_H_OS = ng_create_command ("-p", "--b", "0.1");
  NewArgument (binding_H_OS, 1);
  SetArgumentElement (binding_H_OS, 0, 0, "6"); // Category
  NewArgument (binding_H_OS, 1);
  SetArgumentElement (binding_H_OS, 1, 0, ngEPGS->MyNetInfo->HID); // HID ID
  NewArgument (binding_H_OS, 1);
  SetArgumentElement (binding_H_OS, 2, 0, ngEPGS->MyNetInfo->OSID); // SOID ID
  NewCommandLine (message, binding_H_OS);

  NgCommand *bindingHost = ng_create_command ("-p", "--b", "0.1");
  NewArgument (bindingHost, 1);
  SetArgumentElement (bindingHost, 0, 0, "9"); // Category
  NewArgument (bindingHost, 1);
  SetArgumentElement (bindingHost, 1, 0, "Host"); // Key
  NewArgument (bindingHost, 1);
  SetArgumentElement (bindingHost, 2, 0, ngEPGS->MyNetInfo->HID); // Hardware ID
  NewCommandLine (message, bindingHost);

  NgCommand *bindingOS = ng_create_command ("-p", "--b", "0.1");
  NewArgument (bindingOS, 1);
  SetArgumentElement (bindingOS, 0, 0, "2"); // Category
  NewArgument (bindingOS, 1);
  SetArgumentElement (bindingOS, 1, 0, "OS"); // Key
  NewArgument (bindingOS, 1);
  SetArgumentElement (bindingOS, 2, 0, ngEPGS->MyNetInfo->OSID); // SO ID
  NewCommandLine (message, bindingOS);

  char number[20];
  //TODO: Remove this command line. It is deprecated
  NgCommand *msgTypeCL = ng_create_command ("-message", "--type", "0.1");
  NewArgument (msgTypeCL, 1);
  ng_sprintf (number, "%d", message->Type);
  SetArgumentElement (msgTypeCL, 0, 0, number);
  NewCommandLine (message, msgTypeCL);

  NgCommand *seqNumberCL = ng_create_command ("-message", "--seq", "0.1");
  NewArgument (seqNumberCL, 1);

  ng_sprintf (number, "%d", ngEPGS->MessageCounter);
  SetArgumentElement (seqNumberCL, 0, 0, number);  //Incrementar
  NewCommandLine (message, seqNumberCL);
  ngEPGS->MessageCounter++;

  char *msgToString;
  MessageToString (message, &msgToString);
  len = ng_strlen (msgToString);

  char *SCN;
  GenerateSCNFromCharArrayBinaryPatterns4Bytes (msgToString, len, &SCN);
  ng_free (msgToString);
  NgCommand *scnCL = ng_create_command ("-scn", "--seq", "0.1");
  NewArgument (scnCL, 1);
  SetArgumentElement (scnCL, 0, 0, SCN);
  ng_free (SCN);
  NewCommandLine (message, scnCL);

  ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray (&message);

  *expositionMessage = message;

  return result;
}
