/*
	NovaGenesis

	Name:		Embedded Proxy/Gateway Service ethernet support to run in Linux
	Object:		epgs_ethernet
	File:		epgs_ethernet.h
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

#ifndef EPGS_ETHERNET_H_
#define EPGS_ETHERNET_H_

int GetHostRawAddress (char *_Interface, char **_Address);
int CreateRawSocket (int *SID);
int SendToARawSocket (int SID, char *Interface, void *mtu, long long size);
void ReceiveFromARawSocket (int SID, void **mtu, long long *size);
int SelectInterface (char *_Interface);
int SelectInterfaceWithFilter (char *_Interface, char *_Filter);

#endif /* EPGS_ETHERNET_H_ */
