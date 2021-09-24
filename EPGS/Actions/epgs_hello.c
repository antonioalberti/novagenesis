/*
	NovaGenesis

	Name:		EPGS hello
	Object:		epgs_hello
	File:		epgs_hello.c
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


#include "epgs_hello.h"

#include "../epgs_wrapper.h"
#include "../Common/ng_epgs_hash.h"

int ActionRunHello (NgEPGS *ngEPGS, NgMessage **helloMessage)
{

  long long len = 0;

  NgMessage *message = NULL;
  ng_create_message (ng_GetTime (), 0, false, &message);

  NgCommand *CL = ng_create_command ("-m", "--cl", "0.1");
  NewArgument (CL, 1);
  SetArgumentElement (CL, 0, 0, ngEPGS->MyNetInfo->LIMITER);

  NewArgument (CL, 4);
  SetArgumentElement (CL, 1, 0, ngEPGS->MyNetInfo->HID);
  SetArgumentElement (CL, 1, 1, ngEPGS->MyNetInfo->OSID);
  SetArgumentElement (CL, 1, 2, ngEPGS->MyNetInfo->PID);
  SetArgumentElement (CL, 1, 3, "NULL");

  NewArgument (CL, 4);
  SetArgumentElement (CL, 2, 0, "FFFFFFFF");
  SetArgumentElement (CL, 2, 1, "FFFFFFFF");
  SetArgumentElement (CL, 2, 2, "FFFFFFFF");
  SetArgumentElement (CL, 2, 3, "FFFFFFFF");

  NewCommandLine (message, CL);

  NgCommand *helloCL = ng_create_command ("-hello", "--ihc", "0.2");
  NewArgument (helloCL, 9);
  SetArgumentElement (helloCL, 0, 0, ngEPGS->MyNetInfo->HID);
  SetArgumentElement (helloCL, 0, 1, ngEPGS->MyNetInfo->OSID);
  SetArgumentElement (helloCL, 0, 2, ngEPGS->MyNetInfo->PID);
  SetArgumentElement (helloCL, 0, 3, "NULL");
  SetArgumentElement (helloCL, 0, 4, "NULL"); //
  SetArgumentElement (helloCL, 0, 5, "NULL"); //
  SetArgumentElement (helloCL, 0, 6, ngEPGS->MyNetInfo->Stack);
  SetArgumentElement (helloCL, 0, 7, ngEPGS->MyNetInfo->Interface);
  SetArgumentElement (helloCL, 0, 8, ngEPGS->MyNetInfo->Identifier);

  //***********************************************************************************************
  // As linhas seguintes não são necessarias que o EPGS envie para o PGCS. Somente ao contrario.
  //***********************************************************************************************

  //if(ngEPGS->PSSScnIDInfo) {
  //	SetVersion(helloCL, "0.2");
  //	NewArgument(helloCL, 4);
  //	SetArgumentElement(helloCL, 1, 0, ngEPGS->PSSScnIDInfo->HID);
  //	SetArgumentElement(helloCL, 1, 1, ngEPGS->PSSScnIDInfo->OSID);
  //	SetArgumentElement(helloCL, 1, 2, ngEPGS->PSSScnIDInfo->PID);
  //	SetArgumentElement(helloCL, 1, 3, ngEPGS->PSSScnIDInfo->BID);
  //}

  NewCommandLine (message, helloCL);

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

  *helloMessage = message;

  return NG_OK;
}
