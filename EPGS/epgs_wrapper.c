/*
	NovaGenesis

	Name:		EPGS wrapper to run an embedded/simulated version on Linux
	Object:		epgs_wrapper
	File:		epgs_wrapper.c
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

#include "epgs_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

//#include "Ethernet.h"

//#include "PhysicalLayer/WiFi/WiFi.h"
#include "epgs_ethernet.h"
#include "epgs_defines.h"

int ng_rand (void)
{
  srand (time (NULL));
  return rand ();
}

int ng_atoi (const char *str)
{
  return atoi (str);
}

unsigned long ng_strtoul (const char *str, char **endptr, int base)
{
  return strtoul (str, endptr, base);
}

void *ng_calloc (long unsigned int nitems, long unsigned int size)
{
  return calloc (nitems, size);
}

void *ng_malloc (long unsigned int size)
{
  return malloc (size);
}

void *ng_realloc (void *ptr, long unsigned int size)
{
  return realloc (ptr, size);
}

void ng_free (void *ptr)
{
  free (ptr);
}

void *ng_memcpy (void *ptrDst, const void *ptrSrc, long unsigned int size)
{
  return memcpy (ptrDst, ptrSrc, size);
}

int ng_strcmp (const char *ptrA, const char *ptrB)
{
  return strcmp (ptrA, ptrB);
}

char *ng_strcpy (char *ptrDst, const char *ptrSrc)
{
  return strcpy (ptrDst, ptrSrc);
}

long unsigned int ng_strlen (const char *str)
{
  return strlen (str);
}

char *ng_strncpy (char *ptrDst, const char *ptrSrc, long unsigned int n)
{
  return strncpy (ptrDst, ptrSrc, n);
}

char *ng_strcat (char *ptrDst, const char *ptrSrc)
{
  return strcat (ptrDst, ptrSrc);
}

long unsigned int ng_strspn (const char *str1, const char *str2)
{
  return strspn (str1, str2);
}

long unsigned int ng_strcspn (const char *str1, const char *str2)
{
  return strcspn (str1, str2);
}

int ng_printf (const char *format, ...)
{
  register int retval;
  __builtin_va_list args;
  __builtin_va_start(args, format);
  retval = vprintf (format, args);
  __builtin_va_end(args);
  return retval;
  //return 1;
}

int ng_sprintf (char *str, const char *format, ...)
{
  register int retval;
  __builtin_va_list args;
  __builtin_va_start(args, format);
  retval = vsprintf (str, format, args);
  __builtin_va_end(args);
  return retval;
}

double ng_GetTime ()
{
  return 1000;
}

void ng_EthernetSendData (void *addr, long long size, char *_Interface)
{

  //********************************************************************************
  //
  // The following method is used in the Linux to send in raw socket
  //
  //********************************************************************************

  int SID;

  CreateRawSocket (&SID);

  printf ("\nSelected Interface: %-8s\n", _Interface);

  SendToARawSocket (SID, _Interface, addr, size);

  //********************************************************************************
  //
  // The following method is used in the EventOS to send in NXP board
  //
  //********************************************************************************

  //Ethernet_sendData(addr, size);

  //********************************************************************************
  //
  // The following method is used in the EventOS to send in NXP board using Wi-Fi shield
  //
  //********************************************************************************

  //WiFi_writePacket(addr, size);
}

void ng_BLESendData (char *addr, long long size)
{


  //EPGSMessage(addr, size);
}
