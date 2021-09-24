/*
	NovaGenesis

	Name:		EPGS pub notify
	Object:		epgs_pubNotify
	File:		epgs_pubNotify.h
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

#include "epgs_pubNotify.h"

#include "../Common/ng_epgs_hash.h"
#include "../epgs_wrapper.h"

int
actionPublicationAndNotification (NgEPGS *ngEPGS, bool isData, char *fileName, char *filePayload, int payloadSize, NgMessage **pubNotifyMessage)
{

  if (!ngEPGS || !ngEPGS->MyNetInfo || !ngEPGS->MyNetInfo->HID || !ngEPGS->MyNetInfo->OSID)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->PSSScnIDInfo || !ngEPGS->PSSScnIDInfo->HID || !ngEPGS->PSSScnIDInfo->OSID || !ngEPGS->PSSScnIDInfo->PID
	  || !ngEPGS->PSSScnIDInfo->BID)
	{
	  return NG_ERROR;
	}
  if (isData)
	{
	  if (!ngEPGS->APPScnIDInfo || !ngEPGS->APPScnIDInfo->HID || !ngEPGS->APPScnIDInfo->OSID
		  || !ngEPGS->APPScnIDInfo->PID || !ngEPGS->APPScnIDInfo->BID)
		{
		  return NG_ERROR;
		}
	}
  else
	{
	  if (!ngEPGS->PGCSInfo || !ngEPGS->PGCSInfo->HID || !ngEPGS->PGCSInfo->OSID || !ngEPGS->PGCSInfo->PID
		  || !ngEPGS->PGCSInfo || !ngEPGS->PGCSInfo->CORE_BID_SCN)
		{
		  return NG_ERROR;
		}
	}

  char *payloadHash;
  int len = payloadSize;
  GenerateSCNFromCharArrayBinaryPatterns4Bytes (filePayload, len, &payloadHash);

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

  NgCommand *notificationCL = ng_create_command ("-p", "--notify", "0.1");
  NewArgument (notificationCL, 1);
  SetArgumentElement (notificationCL, 0, 0, "18"); // Category

  NewArgument (notificationCL, 1);
  SetArgumentElement (notificationCL, 1, 0, payloadHash); // Key - Hash do arquivo.
  ng_free (payloadHash);

  NewArgument (notificationCL, 1);
  SetArgumentElement (notificationCL, 2, 0, fileName); // _Publisher - File Name

  NewArgument (notificationCL, 5);
  SetArgumentElement (notificationCL, 3, 0, "pub");

  if (isData)
	{
	  SetArgumentElement (notificationCL, 3, 1, ngEPGS->APPScnIDInfo->HID);
	  SetArgumentElement (notificationCL, 3, 2, ngEPGS->APPScnIDInfo->OSID);
	  SetArgumentElement (notificationCL, 3, 3, ngEPGS->APPScnIDInfo->PID);
	  SetArgumentElement (notificationCL, 3, 4, ngEPGS->APPScnIDInfo->BID);
	}
  else
	{
	  SetArgumentElement (notificationCL, 3, 1, ngEPGS->PGCSInfo->HID);
	  SetArgumentElement (notificationCL, 3, 2, ngEPGS->PGCSInfo->OSID);
	  SetArgumentElement (notificationCL, 3, 3, ngEPGS->PGCSInfo->PID);
	  SetArgumentElement (notificationCL, 3, 4, ngEPGS->PGCSInfo->CORE_BID_SCN);
	}

  NewCommandLine (message, notificationCL);

  NgCommand *infoCL = ng_create_command ("-info", "--payload", "0.1");
  NewArgument (infoCL, 1);
  SetArgumentElement (infoCL, 0, 0, fileName);
  NewCommandLine (message, infoCL);

  char number[20];
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

  SetPayloadFromCharArray (message, filePayload, payloadSize);

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

  *pubNotifyMessage = message;

  return NG_OK;
}
