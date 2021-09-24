/*
	NovaGenesis

	Name:		EPGS novagenesis data structures
	Object:		epgs_structures
	File:		epgs_structures.c
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

#include "epgs_structures.h"
#include "../epgs_wrapper.h"

void destroy_NgNetInfo (struct _ng_net_info **ngHWInfo)
{
  if ((*ngHWInfo) != NULL)
	{
	  if ((*ngHWInfo)->Stack != NULL)
		{
		  ng_free ((*ngHWInfo)->Stack);
		  (*ngHWInfo)->Stack = NULL;
		}
	  if ((*ngHWInfo)->Interface != NULL)
		{
		  ng_free ((*ngHWInfo)->Interface);
		  (*ngHWInfo)->Interface = NULL;
		}
	  if ((*ngHWInfo)->Identifier != NULL)
		{
		  ng_free ((*ngHWInfo)->Identifier);
		  (*ngHWInfo)->Identifier = NULL;
		}
	  ng_free ((*ngHWInfo));
	  (*ngHWInfo) = NULL;
	}
}

void destroy_NgHwDescriptor (struct _ng_hw_descriptor **ngHwDescriptor)
{
  if ((*ngHwDescriptor) != NULL)
	{
	  int i = 0;

	  if ((*ngHwDescriptor)->keyWords)
		{
		  for (i = 0; i < (*ngHwDescriptor)->keyWordsCounter; i++)
			{
			  if ((*ngHwDescriptor)->keyWords[i])
				{
				  ng_free ((*ngHwDescriptor)->keyWords[i]);
				  (*ngHwDescriptor)->keyWords[i] = NULL;
				}
			}
		  ng_free ((*ngHwDescriptor)->keyWords);
		  (*ngHwDescriptor)->keyWords = NULL;
		}

	  if ((*ngHwDescriptor)->sensorFeatureName)
		{
		  for (i = 0; i < (*ngHwDescriptor)->featureCounter; i++)
			{
			  if ((*ngHwDescriptor)->sensorFeatureName[i])
				{
				  ng_free ((*ngHwDescriptor)->sensorFeatureName[i]);
				  (*ngHwDescriptor)->sensorFeatureName[i] = NULL;
				}
			}
		  ng_free ((*ngHwDescriptor)->sensorFeatureName);
		  (*ngHwDescriptor)->sensorFeatureName = NULL;
		}
	  if ((*ngHwDescriptor)->sensorFeatureValue)
		{
		  for (i = 0; i < (*ngHwDescriptor)->featureCounter; i++)
			{
			  if ((*ngHwDescriptor)->sensorFeatureValue[i])
				{
				  ng_free ((*ngHwDescriptor)->sensorFeatureValue[i]);
				  (*ngHwDescriptor)->sensorFeatureValue[i] = NULL;
				}
			}
		  ng_free ((*ngHwDescriptor)->sensorFeatureValue);
		  (*ngHwDescriptor)->sensorFeatureValue = NULL;
		}
	  (*ngHwDescriptor)->featureCounter = 0;
	  ng_free ((*ngHwDescriptor));
	  (*ngHwDescriptor) = NULL;
	}
}

void destroy_NgPGCSInfo (struct _ng_pgcs_info **ngPeerInfo)
{

  if ((*ngPeerInfo) != NULL)
	{
	  if ((*ngPeerInfo)->Stack != NULL)
		{
		  ng_free ((*ngPeerInfo)->Stack);
		  (*ngPeerInfo)->Stack = NULL;
		}
	  if ((*ngPeerInfo)->Interface != NULL)
		{
		  ng_free ((*ngPeerInfo)->Interface);
		  (*ngPeerInfo)->Interface = NULL;
		}
	  if ((*ngPeerInfo)->Identifier != NULL)
		{
		  ng_free ((*ngPeerInfo)->Identifier);
		  (*ngPeerInfo)->Identifier = NULL;
		}
	  ng_free ((*ngPeerInfo));
	  (*ngPeerInfo) = NULL;
	}
}

void destroy_NgScnIDInfo (struct _ng_scn_id_info **ngScnIDInfo)
{
  if ((*ngScnIDInfo) != NULL)
	{
	  ng_free ((*ngScnIDInfo));
	  (*ngScnIDInfo) = NULL;
	}
}

void destroy_NgReceivedMsg (struct _ng_received_msg **ngReceivedMsg)
{
  if (*ngReceivedMsg != NULL)
	{
	  if ((*ngReceivedMsg)->buffer != NULL)
		{
		  ng_free ((*ngReceivedMsg)->buffer);
		  (*ngReceivedMsg)->buffer = NULL;
		}
	  ng_free (*ngReceivedMsg);
	  (*ngReceivedMsg) = NULL;
	}
}

void destroy_NgEPGS (struct _ng_epgs **ngEPGS)
{
  if ((*ngEPGS) != NULL)
	{
	  destroy_NgNetInfo (&(*ngEPGS)->MyNetInfo);
	  destroy_NgNetInfo (&(*ngEPGS)->PGCSInfo);
	  destroy_NgScnIDInfo (&(*ngEPGS)->PSSScnIDInfo);
	  destroy_NgScnIDInfo (&(*ngEPGS)->APPScnIDInfo);
	  destroy_NgReceivedMsg (&(*ngEPGS)->ReceivedMsg);
	  destroy_NgHwDescriptor (&(*ngEPGS)->HwDescriptor);

	  if ((*ngEPGS)->PeersNetInfoCount > 0)
		{
		  int i = 0;
		  for (i = 0; i < (*ngEPGS)->PeersNetInfoCount; i++)
			{
			  destroy_NgNetInfo (&(*ngEPGS)->PeersNetInfo[i]);
			}
		  ng_free ((*ngEPGS)->PeersNetInfo);
		  (*ngEPGS)->PeersNetInfo = NULL;
		  (*ngEPGS)->PeersNetInfoCount = 0;
		}

	  if ((*ngEPGS)->key)
		{
		  ng_free ((*ngEPGS)->key);
		  (*ngEPGS)->key = NULL;
		}

	  if ((*ngEPGS)->pubData)
		{
		  ng_free ((*ngEPGS)->pubData);
		  (*ngEPGS)->pubData = NULL;
		}

	  if ((*ngEPGS)->pubDataFileName)
		{
		  ng_free ((*ngEPGS)->pubDataFileName);
		  (*ngEPGS)->pubDataFileName = NULL;
		}

	  ng_free ((*ngEPGS));
	  (*ngEPGS) = NULL;
	}
}
