/*
	NovaGenesis

	Name:		Embedded Proxy/Gateway Service ethernet simulator to run in Linux
	Object:		EPGS_Simulation
	File:		EPGS_Simulation.c
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

#include <pthread.h>
#include <stdio.h>
#include "epgs_wrapper.h"
#include "epgs.h"
#include "epgs_ethernet.h"
#include "epgs_defines.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <sys/wait.h>

#ifndef _TIME_H
#include <time.h>
#endif

char NG_INTERFACE_NAME[] = "";

void *EthernetListener (void *ngInfo)
{
  int SID;
  CreateRawSocket (&SID);

  while (true)
	{

	  void *mtu = (void *)malloc (2000);
	  long long size;

	  ReceiveFromARawSocket (SID, &mtu, &size);
	  newEthernetReceivedMessage (ngInfo, mtu, size);
	  free (mtu);
	}

  return 0;
}

double GetTime ()
{
  struct timespec t;

  clock_gettime (CLOCK_MONOTONIC, &t);

  return ((t.tv_sec) + (double)(t.tv_nsec / 1e9));
}

int main (int argc, char *argv[])
{
  ng_printf ("*************************\n* FIoT Test on Ethernet *\n* EPGS version 1.2      *\n*************************\n\n");

  if (argc >= 2)
	{
	  char *Address;

	  char Alternative[] = "";

	  size_t S1 = sizeof (argv[1]);

	  strncpy (Alternative, argv[1], S1);

	  printf ("Using option %s\n", Alternative);

	  if (Alternative[1] == 'i') // Explicitly specify the interface to be used
		{
		  if (argc == 3)
			{
			  printf ("Explicitly specify the interface to be used\n");

			  size_t S2 = sizeof (argv[2]);

			  strncpy (NG_INTERFACE_NAME, argv[2], S2);
			}
		  else
			{
			  printf ("Error: no Interface provided after -i option\n");
			}
		}
	  else if (Alternative[1] == 'd') // Discover the interface to be used automatically
		{
		  printf ("Discover the interface to be used automatically. Select the first found.\n");

		  SelectInterface (&NG_INTERFACE_NAME);
		}
	  else if (Alternative[1]
			   == 's') // Discover the interface to be used automatically and uses a filter that have what is passed in second argument
		{
		  if (argc == 3)
			{
			  printf ("Discover the interface to be used automatically applying filter word %s to select the adequate one\n", argv[2]);

			  size_t S4 = sizeof (argv[2]);

			  char Filter[] = "";

			  strncpy (Filter, argv[2], S4);

			  if (SelectInterfaceWithFilter (&NG_INTERFACE_NAME, Filter) == 0)
				{
				  printf ("Error: unable to find an interface that satisfies the query. Exiting\n");

				  return 1;
				}
			}
		  else
			{
			  printf ("Error: no Filter provided after -s option\n");
			}
		}
	  else
		{
		  printf ("ERROR: command alternative does not exist.\n\n");
		  return 0;
		}

	  printf ("\nSelected Interface: %-8s\n", NG_INTERFACE_NAME);

	  if (GetHostRawAddress (NG_INTERFACE_NAME, &Address) == 1)
		{
		  printf ("EGPS initialization error! Error getting MAC address.\n");
		  return 0;
		}

	  printf ("Selected Address: %s\n", Address);

	  NgEPGS *ngInfo = NULL;

	  initEPGS (&ngInfo);

	  char sPid[8];
	  int iPid = getpid ();
	  sprintf (sPid, "%d", iPid);


	  // Set the interface to be used
	  ngInfo->Interface = NG_INTERFACE_NAME;

	  struct utsname unameData;
	  uname (&unameData);

	  char Z[sizeof (unameData.nodename) + sizeof (unameData.release) + sizeof (unameData.version)
			 + sizeof (unameData.machine)] = "";

	  strcat (Z, unameData.nodename);
	  strcat (Z, unameData.release);
	  strcat (Z, unameData.version);
	  strcat (Z, unameData.machine);

	  printf ("Nodename: %s", unameData.nodename);
	  printf ("\nRelease: %s", unameData.release);
	  printf ("\nVersion: %s", unameData.version);
	  printf ("\nMachine: %s", unameData.machine);
	  printf ("\nHostname: %s\n", Z);

	  setHwConfigurations (&ngInfo, Z, unameData.version, sPid,
						   "Ethernet", NG_INTERFACE_NAME, Address);

	  free (Address);

	  addKeyWords (&ngInfo, "Termômetro");

	  addHwSensorFeature (&ngInfo, "sensorType", "Temperature");
	  addHwSensorFeature (&ngInfo, "sensorRangeMin", "-20");
	  addHwSensorFeature (&ngInfo, "sensorRangeMax", "100");
	  addHwSensorFeature (&ngInfo, "sensorResolution", "0.1");
	  addHwSensorFeature (&ngInfo, "sensorAccuracy", "0.2");

	  enablePeriodicHello (&ngInfo, true);

	  pthread_t threads;

	  pthread_create (&threads, NULL, EthernetListener, (void *)&ngInfo);

	  int Count = 0;

	  while (true)
		{
		  int dataPSize = 19;

		  char *dataP = (char *)malloc (sizeof (char) * dataPSize);

		  int Temperature = 20 + rand () % 10;

		  char TempChar[sizeof (Temperature)];

		  sprintf (TempChar, "%d", Temperature);

		  char X[19] = "";

		  strcat (X, "{ Temperature: ");
		  strcat (X, TempChar);
		  strcat (X, " }");

		  //printf("X = %s",X);

		  strcpy (dataP, X);

		  char Temp1[] = "Temperature_";

		  char Temp2[SVN_SIZE + 1];

		  for (int i = 0; i < (SVN_SIZE + 1); i++)
			{
			  Temp2[i] = ngInfo->MyNetInfo->PID[i];
			}

		  char Temp3[sizeof (Count)];

		  sprintf (Temp3, "_%d", Count);

		  //printf("\nTemp3=%s",Temp3);

		  char Temp4[] = ".json";

		  char *A = strcat (Temp1, Temp2);

		  char *B = strcat (Temp3, Temp4);

		  char *Name = strcat (A, B);

		  //printf("\nName=%s",Name);

		  setDataToPub (&ngInfo, Name, dataP, dataPSize);

		  processLoop (&ngInfo);

		  enablePeriodicHello (&ngInfo, false);

		  if (ngInfo->ngState == PUB_DATA)
			{
			  double T = GetTime ();

			  setDataToPub (&ngInfo, Name, dataP, dataPSize);

			  printf ("Time = %f\n", GetTime () - T);

			}

		  Count++;

		  ng_free (dataP);

		  usleep (2000000);
		}

	  pthread_exit (NULL);
	  destroy_NgEPGS (&ngInfo);
	}
  else
	{
	  printf ("ERROR: no alternative provided in EPGS command line.\n\n");

	  printf ("Usage: sudo ./EPGS -i Interface    for example: sudo ./EPGS -i eth0\n");
	  printf ("Usage: sudo ./EPGS -d              for example: sudo ./EPGS -d\n");
	}

  return 1;
}
