/*
	NovaGenesis

	Name:		EPGS novagenesis proxy gateway
	Object:		PG
	File:		PG.h
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

#ifndef NG_PG_H_
#define NG_PG_H_

#include "../DataStructures/epgs_structures.h"
#include "../Common/ng_message.h"

#define DEFAULT_MTU 1500

#define HEADER_SIZE_FIELD_SIZE 8
#define HEADER_SEGMENTATION_FIELD_SIZE 8
#define ETHERNET_MAC_ADDR_FIELD_SIZE 6
#define ETHERNET_TYPE_FIELD_SIZE 2

//char NG_INTERFACE_NAME[]="";

void convertStrToMAC (char *macSTR, char **macBytes);
int getNumberOfMessages (int msgSize);

int sendNGMessage (NgEPGS *ngEPGS, NgMessage *message, bool isBroadcast);
int forwardNGMessageToPeer (NgEPGS *ngEPGS, int peerID, NgMessage *message);
int newMessageReceived (struct _ng_epgs **ngEPGS, const char *message, int rcvdMsgSize);

int sendNGMessageThroughBLE (NgEPGS *ngEPGS, NgMessage *message);
int forwardNGMessageToPeerThroughBLE (NgEPGS *ngEPGS, int peerID, NgMessage *message);
int newMessageReceivedThroughBLE (struct _ng_epgs **ngEPGS, const char *message, int rcvdMsgSize);

#endif /* NG_PG_H_ */
