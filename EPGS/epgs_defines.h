/*
	NovaGenesis

	Name:		Embedded Proxy/Gateway Service defines
	Object:		epgs_defines
	File:		epgs_defines.h
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

#ifndef EPGS_DEFINES_H_
#define EPGS_DEFINES_H_

// Boolean define
typedef int bool;
enum { false, true };

// ERROR TYPES
#define NG_PROCESSING 2
#define NG_ERROR 1
#define NG_OK 0

// NG CONSTANTS
#define NG_TYPE_MSB 0x12
#define NG_TYPE_LSB 0x34

#define SVN_SIZE 8

static const char SCN_NG_DOMAIN[] = "Intra_Domain";

static const char NG_STACK_ETHERNET[] = "Ethernet";
static const char NG_STACK_WIFI[] = "Wifi";
static const char NG_STACK_BLUETOOTH[] = "Bluetooth";

//VBOX UBUNTU
//static const char NG_INTERFACE_NAME[] = "enp0s3";

//LAB COMPUTERS
//static const char NG_INTERFACE_NAME[] = "enp2s0";
//static char NG_INTERFACE_NAME[]="";

//char NG_INTERFACE_NAME[]="";

#define MILLI_SECONDS_PER_CYCLE 1000 // 1 second per cycle

#endif /* EPGS_DEFINES_H_ */
