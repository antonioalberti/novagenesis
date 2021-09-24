/*
	NovaGenesis

	Name:		Embedded Proxy/Gateway Service
	Object:		epgs
	File:		epgs.c
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

#include "epgs.h"
#include "epgs_wrapper.h"
#include "Common/ng_epgs_hash.h"
#include "Controller/epgs_controller.h"
#include "Network/PG.h"

int initEPGS (NgEPGS **ngEPGS)
{

  if (*ngEPGS)
	{
	  return NG_ERROR;
	}

  (*ngEPGS) = (NgEPGS *)ng_malloc (sizeof (NgEPGS));

  (*ngEPGS)->MyNetInfo = (NgNetInfo *)ng_malloc (sizeof (NgNetInfo));

  (*ngEPGS)->PeersNetInfoCount = 0;
  (*ngEPGS)->PeersNetInfo = NULL;

  (*ngEPGS)->PGCSInfo = NULL;

  (*ngEPGS)->PSSScnIDInfo = NULL;

  (*ngEPGS)->APPScnIDInfo = NULL;

  (*ngEPGS)->ReceivedMsg = NULL;

  (*ngEPGS)->HwDescriptor = (NgHwDescriptor *)ng_malloc (sizeof (NgHwDescriptor));
  (*ngEPGS)->HwDescriptor->keyWords = NULL;
  (*ngEPGS)->HwDescriptor->keyWordsCounter = 0;

  (*ngEPGS)->HwDescriptor->sensorFeatureName = NULL;
  (*ngEPGS)->HwDescriptor->sensorFeatureValue = NULL;
  (*ngEPGS)->HwDescriptor->featureCounter = 0;

  addKeyWords (ngEPGS, "EPGS");
  addKeyWords (ngEPGS, "Embedded_Proxy_Gateway_Service");

  (*ngEPGS)->MessageCounter = 0;
  (*ngEPGS)->ngState = HELLO;
  (*ngEPGS)->enableHello = true;

  (*ngEPGS)->pubDataFileName = NULL;
  (*ngEPGS)->pubDataSize = 0;
  (*ngEPGS)->pubData = NULL;

  (*ngEPGS)->key = NULL;

  return NG_OK;
}

int destroyEPGS (NgEPGS **ngEPGS)
{
  destroy_NgEPGS (ngEPGS);
  return 1;
}

int
setHwConfigurations (NgEPGS **ngEPGS, const char *hid, const char *soid, const char *pid, const char *Stack, const char *Interface, const char *Identifier)
{
  if (!(*ngEPGS)->MyNetInfo)
	{
	  return NG_ERROR;
	}

  int len = 0;

  len = ng_strlen (hid);
  char *hidSCN;
  GenerateSCNFromCharArrayBinaryPatterns4Bytes (hid, len, &hidSCN);
  ng_strcpy ((*ngEPGS)->MyNetInfo->HID, hidSCN);
  ng_free (hidSCN);

  len = ng_strlen (soid);
  char *soidSCN;
  GenerateSCNFromCharArrayBinaryPatterns4Bytes (soid, len, &soidSCN);
  ng_strcpy ((*ngEPGS)->MyNetInfo->OSID, soidSCN);
  ng_free (soidSCN);

  len = ng_strlen (pid);
  char *pidSCN;
  GenerateSCNFromCharArrayBinaryPatterns4Bytes (pid, len, &pidSCN);
  ng_strcpy ((*ngEPGS)->MyNetInfo->PID, pidSCN);
  ng_free (pidSCN);

  len = ng_strlen (SCN_NG_DOMAIN);
  char *limiterSCN;
  GenerateSCNFromCharArrayBinaryPatterns4Bytes (SCN_NG_DOMAIN, len, &limiterSCN);
  ng_strcpy ((*ngEPGS)->MyNetInfo->LIMITER, limiterSCN);
  ng_free (limiterSCN);

  (*ngEPGS)->MyNetInfo->Stack = (char *)ng_malloc (sizeof (char) * (ng_strlen (Stack) + 1));
  ng_strcpy ((*ngEPGS)->MyNetInfo->Stack, Stack);

  (*ngEPGS)->MyNetInfo->Interface = (char *)ng_malloc (sizeof (char) * (ng_strlen (Interface) + 1));
  ng_strcpy ((*ngEPGS)->MyNetInfo->Interface, Interface);

  (*ngEPGS)->MyNetInfo->Identifier = (char *)ng_malloc (sizeof (char) * (ng_strlen (Identifier) + 1));
  ng_strcpy ((*ngEPGS)->MyNetInfo->Identifier, Identifier);

  return NG_OK;
}

int enablePeriodicHello (NgEPGS **ngEPGS, bool enable)
{
  (*ngEPGS)->enableHello = enable;
  return NG_OK;
}

int processLoop (NgEPGS **ngEPGS)
{

  if ((*ngEPGS)->enableHello == true)
	{
	  RunHello ((*ngEPGS));
	}

  if ((*ngEPGS)->ngState == PUB_DATA)
	{
	  if ((*ngEPGS)->pubData)
		{
		  RunPublishData ((*ngEPGS));
		}
	}

  return NG_OK;
}

int newEthernetReceivedMessage (NgEPGS **ngEPGS, const char *message, int msgSize)
{
  int result = newMessageReceived (ngEPGS, message, msgSize);

  if (result != NG_PROCESSING)
	{
	  destroy_NgReceivedMsg (&((*ngEPGS)->ReceivedMsg));
	}

  return result;
}

int newBLEReceivedMessage (NgEPGS **ngEPGS, const char *message, int msgSize)
{
  ng_printf ("newBLEReceivedMessage begin - size: %d", msgSize);

  int result = NG_ERROR;
  result = newMessageReceivedThroughBLE (ngEPGS, message, msgSize);

  ng_printf ("newBLEReceivedMessage end - result: %d", result);
  if (result != NG_PROCESSING)
	{
	  destroy_NgReceivedMsg (&((*ngEPGS)->ReceivedMsg));
	}

  return result;
}

int addKeyWords (NgEPGS **ngEPGS, const char *key)
{
  (*ngEPGS)->HwDescriptor->keyWords = (char **)ng_realloc ((*ngEPGS)->HwDescriptor->keyWords,
														   ((*ngEPGS)->HwDescriptor->keyWordsCounter + 1)
														   * sizeof (char *));
  (*ngEPGS)->HwDescriptor->keyWords[(*ngEPGS)->HwDescriptor->keyWordsCounter] = (char *)ng_malloc (
	  sizeof (char) * (ng_strlen (key) + 1));
  ng_strcpy ((*ngEPGS)->HwDescriptor->keyWords[(*ngEPGS)->HwDescriptor->keyWordsCounter], key);

  (*ngEPGS)->HwDescriptor->keyWordsCounter++;

  return NG_OK;
}

int addHwSensorFeature (NgEPGS **ngEPGS, const char *name, const char *value)
{
  (*ngEPGS)->HwDescriptor->sensorFeatureName = (char **)ng_realloc ((*ngEPGS)->HwDescriptor->sensorFeatureName,
																	((*ngEPGS)->HwDescriptor->featureCounter + 1)
																	* sizeof (char *));
  (*ngEPGS)->HwDescriptor->sensorFeatureName[(*ngEPGS)->HwDescriptor->featureCounter] = (char *)ng_malloc (
	  sizeof (char) * (ng_strlen (name) + 1));
  ng_strcpy ((*ngEPGS)->HwDescriptor->sensorFeatureName[(*ngEPGS)->HwDescriptor->featureCounter], name);

  (*ngEPGS)->HwDescriptor->sensorFeatureValue = (char **)ng_realloc ((*ngEPGS)->HwDescriptor->sensorFeatureValue,
																	 ((*ngEPGS)->HwDescriptor->featureCounter + 1)
																	 * sizeof (char *));
  (*ngEPGS)->HwDescriptor->sensorFeatureValue[(*ngEPGS)->HwDescriptor->featureCounter] = (char *)ng_malloc (
	  sizeof (char) * (ng_strlen (value) + 1));
  ng_strcpy ((*ngEPGS)->HwDescriptor->sensorFeatureValue[(*ngEPGS)->HwDescriptor->featureCounter], value);

  (*ngEPGS)->HwDescriptor->featureCounter++;

  return NG_OK;
}

int setDataToPub (NgEPGS **ngEPGS, const char *fileName, const char *data, int dataLength)
{

  if (!fileName || !data || dataLength <= 0)
	{
	  return NG_ERROR;
	}

  (*ngEPGS)->pubDataFileName = (char *)ng_realloc ((*ngEPGS)->pubDataFileName,
												   (ng_strlen (fileName) + 1) * sizeof (char));

  (*ngEPGS)->pubData = (char *)ng_realloc ((*ngEPGS)->pubData, (dataLength) * sizeof (char));

  ng_strcpy ((*ngEPGS)->pubDataFileName, fileName);

  int i = 0;
  for (i = 0; i < dataLength; i++)
	{
	  (*ngEPGS)->pubData[i] = data[i];
	}

  (*ngEPGS)->pubDataSize = dataLength;

  return NG_OK;
}
