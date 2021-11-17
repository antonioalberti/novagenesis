/*
	NovaGenesis

	Name:		EPGS novagenesis controller
	Object:		epgs_controller
	File:		epgs_controller.c
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

#include "epgs_controller.h"
#include "../Common/ng_message.h"
#include "../epgs_wrapper.h"
#include "../Actions/epgs_hello.h"
#include "../Network/PG.h"
#include "../Actions/epgs_exposition.h"
#include "../Actions/epgs_pubNotify.h"
#include "../Actions/epgs_subServiceAcceptance.h"
#include "../ng_util/ng_util.h"
#include <stdio.h>
#include <string.h>

/**
 * Checks the destination of the message
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param destPeer		SCN of the destination Peer
 *
 * @return:			   -1, if the destination is unknown
 * 						0, if the destination is the current EPGS
 *						X, where X is the ID+1 of the peer.
 *
 */

int checkDestination (NgEPGS **ngEPGS, NgNetInfo destPeer)
{

  NgNetInfo *my = (*ngEPGS)->MyNetInfo;
  NgNetInfo **peers = (*ngEPGS)->PeersNetInfo;
  int peersCount = (*ngEPGS)->PeersNetInfoCount;

  if (ng_strcmp (destPeer.LIMITER, my->LIMITER) == 0)
	{
	  if (ng_strcmp (destPeer.HID, my->HID) == 0
		  || ng_strcmp (destPeer.HID, "FFFFFFFF") == 0)
		{
		  ng_printf ("\nEPGS is the destination!\n");
		  return 0;
		}
	  else
		{
		  if (peersCount > 0)
			{
			  int i = 0;

			  for (i = 0; i < peersCount; i++)
				{
				  if (ng_strcmp (peers[i]->HID, destPeer.HID) == 0
					  && ng_strcmp (peers[i]->OSID, destPeer.OSID) == 0
					  && ng_strcmp (peers[i]->PID, destPeer.PID) == 0
					  && ng_strcmp (peers[i]->BID, destPeer.BID) == 0)
					{
					  ng_printf ("\nThe peer number %d is the destination!\n",
								 i + 1);
					  return i + 1;
					}
				}
			}
		}
	}

  ng_printf ("\nNo destination peer was found!\n");
  return -1;
}

/**
 * Adds a new peer
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param peer			SCN and Net information of the Peer.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int addPeer (NgEPGS **ngEPGS, NgNetInfo peer)
{

  int i = 0;
  for (i = 0; i < (*ngEPGS)->PeersNetInfoCount; i++)
	{
	  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->LIMITER, peer.LIMITER) == 0)
		{
		  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->HID, peer.HID) == 0)
			{
			  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->OSID, peer.OSID)
				  == 0)
				{
				  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->PID, peer.PID)
					  == 0)
					{
					  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->BID, peer.BID)
						  == 0)
						{
						  ng_printf ("\nPeer already exists!\n");
						  return NG_OK;
						}
					}
				}
			}
		}
	}

  ng_printf ("\nNew peer discovered!\n");

  int peerIndex = (*ngEPGS)->PeersNetInfoCount;
  (*ngEPGS)->PeersNetInfo = (NgNetInfo **)ng_realloc ((*ngEPGS)->PeersNetInfo,
													  (peerIndex + 1) * sizeof (NgNetInfo *));
  (*ngEPGS)->PeersNetInfo[peerIndex] = (NgNetInfo *)ng_malloc (
	  sizeof (NgNetInfo) * 1);

  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->LIMITER, peer.LIMITER);

  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->HID, peer.HID);
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->OSID, peer.OSID);
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->PID, peer.PID);
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->BID, peer.BID);

  ng_printf ("\nThe peer HID is %s\n", peer.HID);

  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->GW_SCN, peer.GW_SCN);
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->HT_SCN, peer.HT_SCN);
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->CORE_BID_SCN,
			 peer.CORE_BID_SCN);

  (*ngEPGS)->PeersNetInfo[peerIndex]->Stack = (char *)ng_malloc (
	  sizeof (char) * (ng_strlen (peer.Stack) + 1));
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->Stack, peer.Stack);

  (*ngEPGS)->PeersNetInfo[peerIndex]->Interface = (char *)ng_malloc (
	  sizeof (char) * (ng_strlen (peer.Interface) + 1));
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->Interface, peer.Interface);

  (*ngEPGS)->PeersNetInfo[peerIndex]->Identifier = (char *)ng_malloc (
	  sizeof (char) * (ng_strlen (peer.Identifier) + 1));
  ng_strcpy ((*ngEPGS)->PeersNetInfo[peerIndex]->Identifier, peer.Identifier);

  (*ngEPGS)->PeersNetInfoCount++;

  RunHello ((*ngEPGS));

  return NG_OK;
}

int getPeerIndex (NgEPGS **ngEPGS, NgNetInfo peer)
{

  int i = -1;
  int result = -1;
  for (i = 0; i < (*ngEPGS)->PeersNetInfoCount; i++)
	{
	  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->LIMITER, peer.LIMITER) == 0)
		{
		  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->HID, peer.HID) == 0)
			{
			  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->OSID, peer.OSID)
				  == 0)
				{
				  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->PID, peer.PID)
					  == 0)
					{
					  if (ng_strcmp ((*ngEPGS)->PeersNetInfo[i]->BID, peer.BID)
						  == 0)
						{
						  result = i;
						}
					}
				}
			}
		}
	}

  return result;
}

int RunHello (NgEPGS *ngEPGS)
{

  if (!ngEPGS)
	{
	  return NG_ERROR;
	}
  if (!(ngEPGS->MyNetInfo))
	{
	  return NG_ERROR;
	}

  NgMessage *helloMessage = NULL;
  ActionRunHello (ngEPGS, &helloMessage);

  //ng_printf("%s", helloMessage->Msg);

  if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_ETHERNET) == 0
	  || ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_WIFI) == 0)
	{
	  sendNGMessage (ngEPGS, helloMessage, true);
	}
  else if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_BLUETOOTH) == 0)
	{
	  sendNGMessageThroughBLE (ngEPGS, helloMessage);
	}
  else
	{
	  ng_printf ("Stack not found: %s", ngEPGS->MyNetInfo->Stack);
	}

  ng_destroy_message (&helloMessage);

  return NG_OK;
}

int RunExposition (NgEPGS *ngEPGS)
{
  int result = NG_ERROR;

  if (!ngEPGS)
	{
	  return result;
	}
  if (!ngEPGS->MyNetInfo)
	{
	  return result;
	}
  if (!ngEPGS->PSSScnIDInfo)
	{
	  return result;
	}

  NgMessage *expositioneMessage = NULL;
  result = actionExpostion (ngEPGS, &expositioneMessage);

  //ng_printf("%s", expositioneMessage->Msg);

  if (result == NG_OK)
	{
	  if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_ETHERNET) == 0
		  || ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_WIFI) == 0)
		{
		  sendNGMessage (ngEPGS, expositioneMessage, false);
		}
	  else if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_BLUETOOTH)
			   == 0)
		{
		  sendNGMessageThroughBLE (ngEPGS, expositioneMessage);
		}
	  else
		{
		  ng_printf ("Stack not found: %s", ngEPGS->MyNetInfo->Stack);
		}
	}
  ng_destroy_message (&expositioneMessage);

  return result;
}

int RunPubServiceOffer (NgEPGS *ngEPGS)
{
  if (!ngEPGS)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->MyNetInfo)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->PGCSInfo)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->PGCSInfo)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->PSSScnIDInfo)
	{
	  return NG_ERROR;
	}

  char *filePayload = (char *)ng_calloc (sizeof (char), 600);
  ng_strcpy (filePayload, "ng -sr --b 0.1 [ < 1 s 17 > < 1 s EPGS_Sensor > ");

  ng_sprintf (filePayload + ng_strlen (filePayload), "< %i s ", ngEPGS->HwDescriptor->featureCounter);
  int i = 0;
  for (i = 0; i < ngEPGS->HwDescriptor->featureCounter; i++)
	{
	  ng_sprintf (filePayload + ng_strlen (filePayload), "%s ", ngEPGS->HwDescriptor->sensorFeatureName[i]);
	}
  ng_sprintf (filePayload + ng_strlen (filePayload), "> ]\n");

  for (i = 0; i < ngEPGS->HwDescriptor->featureCounter; i++)
	{
	  ng_sprintf (filePayload + ng_strlen (filePayload), "ng -sr --b 0.1 [ < 1 s 17 > < 1 s %s > < 1 s %s > ]\n", ngEPGS
		  ->HwDescriptor->sensorFeatureName[i], ngEPGS->HwDescriptor->sensorFeatureValue[i]);
	}

  NgMessage *pubServiceOfferMessage = NULL;
  actionPublicationAndNotification (ngEPGS, false, "Service_Offer.txt", filePayload, (unsigned long long)ng_strlen (filePayload), &pubServiceOfferMessage);
  ng_free (filePayload);

//	ng_printf("%s", pubServiceOfferMessage->Msg);
  sendNGMessage (ngEPGS, pubServiceOfferMessage, false);

  ng_destroy_message (&pubServiceOfferMessage);

  return NG_OK;

}

int RunSubscribeServiceAcceptance (NgEPGS *ngEPGS)
{
  int result = NG_ERROR;

  if (!ngEPGS)
	{
	  return result;
	}
  if (!ngEPGS->MyNetInfo)
	{
	  return result;
	}
  if (!ngEPGS->PGCSInfo)
	{
	  return result;
	}
  if (!ngEPGS->PSSScnIDInfo)
	{
	  return result;
	}

  NgMessage *subServiceAcceptanceMessage = NULL;
  result = ActionSubscriptionServiceAcceptance (ngEPGS->MyNetInfo,
												ngEPGS->PGCSInfo, ngEPGS->PSSScnIDInfo, ngEPGS->MessageCounter,
												ngEPGS->key, &subServiceAcceptanceMessage);

  //ng_printf("%s", subServiceAcceptanceMessage->Msg);
  if (result == NG_OK)
	{
	  if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_ETHERNET) == 0
		  || ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_WIFI) == 0)
		{
		  sendNGMessage (ngEPGS, subServiceAcceptanceMessage, false);
		}
	  else if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_BLUETOOTH)
			   == 0)
		{
		  sendNGMessageThroughBLE (ngEPGS, subServiceAcceptanceMessage);
		}
	  else
		{
		  ng_printf ("Stack not found: %s", ngEPGS->MyNetInfo->Stack);
		}
	}
  ng_destroy_message (&subServiceAcceptanceMessage);

  return result;
}

int RunPublishData (NgEPGS *ngEPGS)
{
  if (!ngEPGS)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->PGCSInfo)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->PSSScnIDInfo)
	{
	  return NG_ERROR;
	}
  if (!ngEPGS->pubData && ngEPGS->pubDataSize > 0)
	{
	  return NG_ERROR;
	}
  if (ngEPGS->pubDataSize <= 0)
	{
	  return NG_OK;
	}

  NgMessage *pubDataMessage = NULL;
  actionPublicationAndNotification (ngEPGS, true, ngEPGS->pubDataFileName,
									ngEPGS->pubData, ngEPGS->pubDataSize, &pubDataMessage);

  //ng_printf("%s", pubDataMessage->Msg);

  /*	int z = 0;
   for(z =0; z< pubDataMessage->MessageSize; z++) {
   ng_printf("%02X ", pubDataMessage->Msg[z]);
   }
   */

  if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_ETHERNET) == 0
	  || ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_WIFI) == 0)
	{
	  sendNGMessage (ngEPGS, pubDataMessage, false);
	}
  else if (ng_strcmp (ngEPGS->MyNetInfo->Stack, NG_STACK_BLUETOOTH) == 0)
	{
	  sendNGMessageThroughBLE (ngEPGS, pubDataMessage);
	}
  else
	{
	  ng_printf ("Stack not found: %s", ngEPGS->MyNetInfo->Stack);
	}
  ng_destroy_message (&pubDataMessage);

  if (ngEPGS->pubDataSize > 0)
	{
	  ngEPGS->pubDataSize = 0;
	}

  return NG_OK;

}

int ParseReceivedMessage2 (NgEPGS **ngEPGS)
{

  int result = NG_OK;

  NgScnIDInfo *pssSCNInfo = NULL;

  ng_printf ("\n%s\n", (*ngEPGS)->ReceivedMsg->buffer);

  NgMessage *ngMessage;
  MessageFromString ((*ngEPGS)->ReceivedMsg->buffer,
					 (*ngEPGS)->ReceivedMsg->mgs_size, &ngMessage);

  NgNetInfo destPeer;
  NgNetInfo senderPeer;

  int i = 0;

  for (i = 0; i < ngMessage->NoCL; i++)
	{
	  NgCommand *CL = NULL;
	  GetCommandLine (ngMessage, i, &CL);

	  //printf("Command Line = %d",i);

	  if (ng_strcmp ("-m", CL->Name) == 0 && ng_strcmp ("--cl", CL->Alternative) == 0)
		{
		  if (CL->NoA == 3)
			{
			  unsigned int nEle;

			  GetNumberofArgumentElements (CL, 0, &nEle);

			  if (nEle == 1)
				{
				  char *value;

				  GetArgumentElement (CL, 0, 0, &value);
				  ng_strcpy (senderPeer.LIMITER, value);
				  ng_strcpy (destPeer.LIMITER, value);
				  ng_free (value);
				}
			  else
				{
				  result = NG_ERROR;
				}

			  GetNumberofArgumentElements (CL, 1, &nEle);

			  if (nEle == 4)
				{
				  char *value;

				  GetArgumentElement (CL, 1, 0, &value);
				  ng_strcpy (senderPeer.HID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 1, 1, &value);
				  ng_strcpy (senderPeer.OSID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 1, 2, &value);
				  ng_strcpy (senderPeer.PID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 1, 3, &value);
				  ng_strcpy (senderPeer.BID, value);
				  ng_free (value);
				}
			  else
				{
				  result = NG_ERROR;
				}

			  GetNumberofArgumentElements (CL, 2, &nEle);

			  if (nEle == 4)
				{
				  char *value;

				  GetArgumentElement (CL, 2, 0, &value);
				  ng_strcpy (destPeer.HID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 2, 1, &value);
				  ng_strcpy (destPeer.OSID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 2, 2, &value);
				  ng_strcpy (destPeer.PID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 2, 3, &value);
				  ng_strcpy (destPeer.BID, value);
				  ng_free (value);

				  int destId = checkDestination (ngEPGS, destPeer);

				  if (destId == -1)
					{
					  break;
					}
				  else if (destId > 0)
					{

					  if (ng_strcmp ((*ngEPGS)->MyNetInfo->Stack,
									 NG_STACK_ETHERNET) == 0
						  || ng_strcmp ((*ngEPGS)->MyNetInfo->Stack,
										NG_STACK_WIFI) == 0)
						{
						  forwardNGMessageToPeer ((*ngEPGS), destId - 1,
												  ngMessage);
						}
					  else if (ng_strcmp ((*ngEPGS)->MyNetInfo->Stack,
										  NG_STACK_BLUETOOTH) == 0)
						{
						  forwardNGMessageToPeerThroughBLE ((*ngEPGS),
															destId - 1, ngMessage);
						}
					  else
						{
						  ng_printf ("Stack not found: %s",
									 (*ngEPGS)->MyNetInfo->Stack);
						}

					  break;
					}

				}
			  else
				{
				  result = NG_ERROR;
				}
			}
		  else
			{
			  result = NG_ERROR;
			}
		}
	  else if (ng_strcmp ("-hello", CL->Name) == 0
			   && ng_strcmp ("--ihc", CL->Alternative) == 0
			   && ng_strcmp ("0.2", CL->Version) == 0)
		{

		  if (CL->NoA == 2)
			{
			  unsigned int nEle;
			  GetNumberofArgumentElements (CL, 0, &nEle);

			  if (nEle == 6)
				{

				  char *value;

				  GetArgumentElement (CL, 0, 0, &value);
				  ng_strcpy (senderPeer.GW_SCN, value);
				  ng_free (value);

				  GetArgumentElement (CL, 0, 1, &value);
				  ng_strcpy (senderPeer.HT_SCN, value);
				  ng_free (value);

				  GetArgumentElement (CL, 0, 2, &value);
				  ng_strcpy (senderPeer.CORE_BID_SCN, value);
				  ng_free (value);

				  GetArgumentElement (CL, 0, 3, &value);
				  senderPeer.Stack = (char *)ng_calloc (sizeof (char) * 1,
														ng_strlen (value) + 1);
				  ng_strcpy (senderPeer.Stack, value);

				  ng_free (value);

				  GetArgumentElement (CL, 0, 4, &value);
				  senderPeer.Interface = (char *)ng_calloc (sizeof (char) * 1,
															ng_strlen (value) + 1);
				  ng_strcpy (senderPeer.Interface, value);
				  ng_free (value);

				  GetArgumentElement (CL, 0, 5, &value);
				  senderPeer.Identifier = (char *)ng_calloc (sizeof (char) * 1,
															 ng_strlen (value) + 1);
				  ng_strcpy (senderPeer.Identifier, value);
				  ng_free (value);

				  addPeer (ngEPGS, senderPeer);

				  if ((*ngEPGS)->PGCSInfo)
					{
					  destroy_NgNetInfo (&(*ngEPGS)->PGCSInfo);
					}

				  (*ngEPGS)->PGCSInfo = (NgNetInfo *)ng_malloc (sizeof (NgNetInfo) * 1);
				  ng_memcpy ((*ngEPGS)->PGCSInfo, &senderPeer, sizeof (NgNetInfo) * 1);

				}
			  else
				{
				  result = NG_ERROR;
				}

			  GetNumberofArgumentElements (CL, 1, &nEle);

			  if (nEle == 4)
				{
				  pssSCNInfo = (NgScnIDInfo *)ng_malloc (
					  sizeof (NgScnIDInfo) * 1);

				  char *value;

				  GetArgumentElement (CL, 1, 0, &value);
				  ng_strcpy (pssSCNInfo->HID, value);
				  ng_strcpy (senderPeer.HID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 1, 1, &value);
				  ng_strcpy (pssSCNInfo->OSID, value);
				  ng_strcpy (senderPeer.OSID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 1, 2, &value);
				  ng_strcpy (pssSCNInfo->PID, value);
				  ng_strcpy (senderPeer.PID, value);
				  ng_free (value);

				  GetArgumentElement (CL, 1, 3, &value);
				  ng_strcpy (pssSCNInfo->BID, value);
				  ng_strcpy (senderPeer.BID, value);
				  ng_free (value);

				  if ((*ngEPGS)->PSSScnIDInfo)
					{
					  continue;
					}

				  (*ngEPGS)->PSSScnIDInfo = pssSCNInfo;

				  addPeer (ngEPGS, senderPeer);

				  // Expoe os recursos para o par!
				  RunExposition ((*ngEPGS));

				  printf ("EXPOSITION: Waiting for PGCS...");

				  usleep (20000000);

				  RunPubServiceOffer ((*ngEPGS));

				  printf ("SERVICE OFFER: Waiting for PGCS...");

				  usleep (20000000);

				  (*ngEPGS)->ngState = WAIT_SERVICE_ACCEPTANCE_NOTIFY;
				}
			}
		  else
			{
			  result = NG_ERROR;
			}
		}
	  else if (ng_strcmp ("-notify", CL->Name) == 0
			   && ng_strcmp ("--s", CL->Alternative) == 0)
		{

		  if (CL->NoA == 3)
			{
			  unsigned int nEle;
			  GetNumberofArgumentElements (CL, 1, &nEle);

			  if (nEle == 1)
				{

				  if ((*ngEPGS)->key)
					{
					  ng_free ((*ngEPGS)->key);
					  (*ngEPGS)->key = NULL;
					}
				  GetArgumentElement (CL, 1, 0, &(*ngEPGS)->key);

				  (*ngEPGS)->ngState = WAIT_SERVICE_ACCEPTANCE_DELIVERY;
				  RunSubscribeServiceAcceptance ((*ngEPGS));

				}
			  else
				{
				  result = NG_ERROR;
				}

			}
		  else
			{
			  result = NG_ERROR;
			}
		}

	  else if (ng_strcmp ("-d", CL->Name) == 0 && ng_strcmp ("--b", CL->Alternative) == 0)
		{
		  if (CL->NoA == 3)
			{
			  //TODO: Added to avoid interpreting .json file as .txt. Modifications from this point to the end pof the function.
			  unsigned int nEle0;
			  GetNumberofArgumentElements (CL, 0, &nEle0);

			  unsigned int nEle1;
			  GetNumberofArgumentElements (CL, 1, &nEle1);

			  unsigned int nEle2;
			  GetNumberofArgumentElements (CL, 2, &nEle2);

			  if (nEle0 == 1 && nEle1 == 1 && nEle2 == 1)
				{
				  char *category;
				  GetArgumentElement (CL, 0, 0, &category);

				  char *key;
				  GetArgumentElement (CL, 1, 0, &key);

				  char *file_name;
				  GetArgumentElement (CL, 2, 0, &file_name);

				  ng_printf ("\nCategory: %s\n",category);
				  ng_printf ("Key: %s\n",key);
				  ng_printf ("Filename: %s\n",file_name);

				  if (strcmp(category, "18") == 0)
					{
					  if (ng_strcmp ((*ngEPGS)->key, key) == 0)
						{
						  char *extension = NULL;

						  char *remaining = strtok_r (file_name, ".", &extension);

						  ng_printf ("Extension: %s\n",extension );
						  ng_printf ("Remaining: %s\n",remaining);

						  if (strcmp(extension, "txt") == 0)
							{
							  ng_printf ("Processing the TXT file received\n");

							  if ((*ngEPGS)->APPScnIDInfo)
								{
								  destroy_NgScnIDInfo (&(*ngEPGS)->APPScnIDInfo);
								}
							  (*ngEPGS)->APPScnIDInfo = (NgScnIDInfo *)ng_malloc (
								  sizeof (NgScnIDInfo) * 1);

							  char *end_str = NULL;
							  char *auxStr = (char *)ng_calloc (sizeof (char) * 1,
																(ngMessage->PayloadSize + 1));
							  ng_memcpy (auxStr, ngMessage->Payload,
										 ngMessage->PayloadSize);

							  char *token = strtok_r (auxStr, " ", &end_str);

							  if (token)
								{
								  ng_strcpy ((*ngEPGS)->APPScnIDInfo->HID, token);
								  token = strtok_r (NULL, " ", &end_str);
								}
							  else
								{
								  result = NG_ERROR;
								}

							  if (token)
								{
								  ng_strcpy ((*ngEPGS)->APPScnIDInfo->OSID, token);
								  token = strtok_r (NULL, " ", &end_str);
								}
							  else
								{
								  result = NG_ERROR;
								}

							  if (token)
								{
								  ng_strcpy ((*ngEPGS)->APPScnIDInfo->PID, token);
								  token = strtok_r (NULL, " ", &end_str);
								}
							  else
								{
								  result = NG_ERROR;
								}

							  if (token)
								{
								  ng_strcpy ((*ngEPGS)->APPScnIDInfo->BID, token);
								  //	ng_free(token);
								}
							  else
								{
								  result = NG_ERROR;
								}

							  ng_printf ("HID: %s\n",(*ngEPGS)->APPScnIDInfo->HID);
							  ng_printf ("OSID: %s\n",(*ngEPGS)->APPScnIDInfo->OSID);
							  ng_printf ("PID: %s\n",(*ngEPGS)->APPScnIDInfo->PID);
							  ng_printf ("BID: %s\n",(*ngEPGS)->APPScnIDInfo->BID);

							  if (result == NG_OK)
								{
								  (*ngEPGS)->ngState = PUB_DATA;
								}
							}
						  else if (strcmp(extension, "json") == 0)
							{
							  // TODO: Add here the code to treat a json file.
							  ng_printf ("Processing the JSON file received\n");
							}
						}

					  ng_free (key);

					  (*ngEPGS)->ngState = PUB_DATA;
					}
				}
			  else
				{
				  result = NG_ERROR;
				}

			}
		  else
			{
			  result = NG_ERROR;
			}
		}
/*
		else if (ng_strcmp("-p", CL->Name) == 0
				&& ng_strcmp("--b", CL->Alternative) == 0) {

			if (CL->NoA == 3) {
				char *pgcsExpo1;
				char *pgcsExpo2;
				GetArgumentElement(CL, 1, 0, &pgcsExpo1);
				GetArgumentElement(CL, 2, 0, &pgcsExpo2);

				if (ng_strcmp(pgcsExpo1, "PGCS") == 0
						|| ng_strcmp(pgcsExpo2, "PGCS") == 0) {

					ng_printf("PGCS FOUND");

					int index = getPeerIndex(ngEPGS, senderPeer);

					if (index > -1) {
						if ((*ngEPGS)->PGCSInfo) {
							destroy_NgNetInfo(&(*ngEPGS)->PGCSInfo);
						}

						(*ngEPGS)->PGCSInfo = (NgNetInfo*) ng_malloc(
								sizeof(NgNetInfo) * 1);
						ng_memcpy((*ngEPGS)->PGCSInfo,
								(*ngEPGS)->PeersNetInfo[index],
								sizeof(NgNetInfo) * 1);

						// Exp�e os recursos para o par!
						RunExposition((*ngEPGS));

						RunPubServiceOffer((*ngEPGS));

						(*ngEPGS)->ngState = WAIT_SERVICE_ACCEPTANCE_NOTIFY;
					}

				} else if (ng_strcmp(pgcsExpo1, "EPGS") == 0
						|| ng_strcmp(pgcsExpo2, "EPGS") == 0) {

					ng_printf("EPGS FOUND");

					int index = getPeerIndex(ngEPGS, senderPeer);

					if (index > -1) {
						if ((*ngEPGS)->PGCSInfo == NULL) {
							(*ngEPGS)->PGCSInfo = (NgNetInfo*) ng_malloc(
									sizeof(NgNetInfo) * 1);
							ng_memcpy((*ngEPGS)->PGCSInfo,
									(*ngEPGS)->PeersNetInfo[index],
									sizeof(NgNetInfo) * 1);

							// Exp�e os recursos para o par!
							RunExposition((*ngEPGS));

							RunPubServiceOffer((*ngEPGS));

							(*ngEPGS)->ngState = WAIT_SERVICE_ACCEPTANCE_NOTIFY;
						}
					}
				}

				ng_free(pgcsExpo1);
				ng_free(pgcsExpo2);

			} else {
				result = NG_ERROR;
			}

		}*/

	  CL = NULL;
	}

  ng_destroy_message (&ngMessage);

  return result;
}

