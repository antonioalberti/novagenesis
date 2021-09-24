/*
	NovaGenesis

	Name:		EPGS service acceptance
	Object:		epgs_subServiceAcceptance
	File:		epgs_subServiceAcceptance.c
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

#include "epgs_subServiceAcceptance.h"
#include "../epgs_wrapper.h"
#include "../Common/ng_epgs_hash.h"

int
ActionSubscriptionServiceAcceptance (NgNetInfo *hwInfo, NgNetInfo *pgcsSCNInfo, NgScnIDInfo *pssSCNInfo, int seqNumber, char *key, NgMessage **subServiceAcceptanceMessage)
{

  long long len = 0;

  NgMessage *message = NULL;
  ng_create_message (ng_GetTime (), 1, true, &message);

  NgCommand *CL = ng_create_command ("-m", "--cl", "0.1");
  NewArgument (CL, 1);
  SetArgumentElement (CL, 0, 0, hwInfo->LIMITER);

  NewArgument (CL, 4);
  SetArgumentElement (CL, 1, 0, hwInfo->HID);
  SetArgumentElement (CL, 1, 1, hwInfo->OSID);
  SetArgumentElement (CL, 1, 2, hwInfo->PID);
  SetArgumentElement (CL, 1, 3, "NULL");

  NewArgument (CL, 4);
  SetArgumentElement (CL, 2, 0, pssSCNInfo->HID);
  SetArgumentElement (CL, 2, 1, pssSCNInfo->OSID);
  SetArgumentElement (CL, 2, 2, pssSCNInfo->PID);
  SetArgumentElement (CL, 2, 3, pssSCNInfo->BID);

  NewCommandLine (message, CL);

  NgCommand *bindCat18 = ng_create_command ("-s", "--b", "0.1");
  NewArgument (bindCat18, 1);
  SetArgumentElement (bindCat18, 0, 0, "18"); // Category
  NewArgument (bindCat18, 1);
  SetArgumentElement (bindCat18, 1, 0, key); // Key - Hash do arquivo.
  NewCommandLine (message, bindCat18);

  NgCommand *bindCat2 = ng_create_command ("-s", "--b", "0.1");
  NewArgument (bindCat2, 1);
  SetArgumentElement (bindCat2, 0, 0, "2"); // Category
  NewArgument (bindCat2, 1);
  SetArgumentElement (bindCat2, 1, 0, key); // Key - Hash do arquivo.
  NewCommandLine (message, bindCat2);

  NgCommand *bindCat9 = ng_create_command ("-s", "--b", "0.1");
  NewArgument (bindCat9, 1);
  SetArgumentElement (bindCat9, 0, 0, "9"); // Category
  NewArgument (bindCat9, 1);
  SetArgumentElement (bindCat9, 1, 0, key); // Key - Hash do arquivo.
  NewCommandLine (message, bindCat9);

  char number[4];
  NgCommand *msgTypeCL = ng_create_command ("-message", "--type", "0.1");
  NewArgument (msgTypeCL, 1);
  ng_sprintf (number, "%d", message->Type);
  SetArgumentElement (msgTypeCL, 0, 0, number);
  NewCommandLine (message, msgTypeCL);

  NgCommand *seqNumberCL = ng_create_command ("-message", "--seq", "0.1");
  NewArgument (seqNumberCL, 1);

  ng_sprintf (number, "%d", seqNumber);
  SetArgumentElement (seqNumberCL, 0, 0, number);  //Incrementar
  NewCommandLine (message, seqNumberCL);

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

  *subServiceAcceptanceMessage = message;

  return NG_OK;
}
