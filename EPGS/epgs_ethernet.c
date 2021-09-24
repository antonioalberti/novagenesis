/*
	NovaGenesis

	Name:		Embedded Proxy/Gateway Service ethernet support to run in Linux
	Object:		epgs_ethernet
	File:		epgs_ethernet.c
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

#include "epgs_ethernet.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <ifaddrs.h>

union ethframe {
  struct {
	struct ethhdr header;
	unsigned char data[ETH_DATA_LEN];
  } field;
  unsigned char buffer[ETH_FRAME_LEN];
};

int GetHostRawAddress (char *_Interface, char **_Address)
{
  int result = 0;
  int Size = 18;
  char ret[Size];
  struct ifreq s;
  int fd = socket (PF_INET, SOCK_DGRAM, IPPROTO_IP);

  *_Address = (char *)malloc (sizeof (char) * 18);

  strcpy (s.ifr_name, _Interface);

  if (fd >= 0)
	{
	  if (0 == ioctl (fd, SIOCGIFHWADDR, &s))
		{
		  int i;
		  for (i = 0; i < 6; ++i)
			{
			  if (i < 5)
				{
				  sprintf (ret + i * 3, "%02x:",
						   (unsigned char)s.ifr_addr.sa_data[i]);
				}
			  else
				{
				  sprintf (ret + i * 3, "%02x",
						   (unsigned char)s.ifr_addr.sa_data[i]);
				}
			}
		}
	  else
		{
		  printf ("Error. Please, check the interface provided!\n");
		  result = 1;
		}
	}
  else
	{
	  printf ("Error. Please, make sure you have sudo privilege·\n");
	  result = 1;
	}

  strcpy (*_Address, ret);

  close (fd);
  return result;
}

// Create a Raw Ethernet socket
int CreateRawSocket (int *SID)
{
  int _SID;
  int Status = 0;
  unsigned short proto = 0x1234;

  _SID = socket (AF_PACKET, SOCK_RAW, htons (proto));

  if (_SID > 0)
	{
	  Status = 1;
	  *SID = _SID;
	}
  else
	{
	  perror ("socket:");
	}

  return Status;
}

int SendToARawSocket (int SID, char *Interface, void *mtu, long long size)
{
  struct sockaddr_ll saddrll;
  int ifindex;
  struct ifreq buffer;
  unsigned char Source[ETH_ALEN];
  unsigned char Destination[ETH_ALEN];
  unsigned short proto = 0x1234;
  union ethframe frame;

  memset (&buffer, 0x00, sizeof (buffer));
  strncpy (buffer.ifr_name, Interface, IFNAMSIZ);

  memcpy ((void *)Destination, (void *)mtu, ETH_ALEN);
  memcpy ((void *)Source, (void *)(mtu + ETH_ALEN), ETH_ALEN);

  if (ioctl (SID, SIOCGIFINDEX, &buffer) >= 0)
	{
	  ifindex = buffer.ifr_ifindex;

	  // Configure the Ethernet II address struct
	  memset ((void *)&saddrll, 0, sizeof (saddrll));
	  saddrll.sll_family = PF_PACKET;
	  saddrll.sll_ifindex = ifindex;
	  saddrll.sll_halen = ETH_ALEN;
	  memcpy ((void *)(saddrll.sll_addr), (void *)Destination, ETH_ALEN);

	  // Configure the Ethernet II frame
	  memcpy (frame.field.header.h_dest, Destination, ETH_ALEN);
	  memcpy (frame.field.header.h_source, Source, ETH_ALEN);
	  frame.field.header.h_proto = htons (proto);
	  memcpy (frame.field.data, (void *)(mtu + ETH_HLEN), size - ETH_HLEN);

	  unsigned int frame_len = size;

	  int numbytes = sendto (SID, (void *)frame.buffer, frame_len, 0,
							 (struct sockaddr *)&saddrll, sizeof (saddrll));

	  if (numbytes > 0)
		{
		  //printf("Data sent successfully!\n");
		}
	  else
		{
		  //printf("Error, could not send data.\n");
		}
	  close (SID);

	}
  return 1;
}

void ReceiveFromARawSocket (int SID, void **mtu, long long *size)
{

  unsigned char frame_buffer[ETH_FRAME_LEN];
  unsigned int receivedbytes;
  struct sockaddr_ll saddrll;

  //printf("\nListening...\n");

  memset ((void *)&saddrll, 0, sizeof (saddrll));
  socklen_t sll_len = (socklen_t)sizeof (saddrll);
  receivedbytes = recvfrom (SID, frame_buffer, ETH_FRAME_LEN, 0,
							(struct sockaddr *)&saddrll, &sll_len);

  *size = receivedbytes;
  *mtu = (void *)malloc (sizeof (void) * receivedbytes);
  memcpy ((void *)*mtu, (void *)frame_buffer, receivedbytes);

  // Printing received data
  //printf("\nData received: %s\n", frame_buffer + sizeof(struct ethhdr));

  //close(SID);

}

//*******************************************************
// Add by Alberti, March 2018, for auto config interface
// ******************************************************
int SelectInterface (char *_Interface)
{
  struct ifaddrs *ifaddr, *ifa;
  int family;
  int n;
  int Status = 0;

  if (getifaddrs (&ifaddr) != -1)
	{
	  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
		{
		  family = ifa->ifa_addr->sa_family;

		  if (family == AF_INET)
			{
			  if (*ifa->ifa_name != 'l' && *ifa->ifa_name != 'v' && *ifa->ifa_name != 'd')
				{
				  Status = 1;

				  //printf("%-8s \n",ifa->ifa_name);

				  size_t S = sizeof (ifa->ifa_name);

				  //printf("Size: %d \n",S);

				  strncpy (_Interface, ifa->ifa_name, S);

				  //printf("Interface: %-8s \n",_Interface);

				  break;
				}
			}
		}
	}
  else
	{
	  perror ("getifaddrs");
	}

  freeifaddrs (ifaddr);

  return Status;
}

//*******************************************************
// Add by Alberti, June 2018, for auto config interface with restrictions.
// Put in the filter what are the CHARACTERS that the interface must have.
// ******************************************************
int SelectInterfaceWithFilter (char *_Interface, char *_Filter)
{
  struct ifaddrs *ifaddr, *ifa;
  int family;
  int n;
  int Status = 0;

  if (getifaddrs (&ifaddr) != -1)
	{
	  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
		{
		  family = ifa->ifa_addr->sa_family;

		  if (family == AF_INET)
			{
			  const char *Temp = NULL;

			  size_t S = sizeof (ifa->ifa_name);

			  strncpy (_Interface, ifa->ifa_name, S);

			  Temp = strstr (_Interface, _Filter);

			  //printf("Filter = %s\n",_Filter);

			  //printf("Temp = %s\n",Temp);

			  //printf("Interface = %s\n",_Interface);

			  if (Temp != NULL)
				{
				  Status = 1;

				  break;
				}
			  else
				{
				  Status = 0;
				}
			}
		}
	}
  else
	{
	  perror ("getifaddrs");
	}

  freeifaddrs (ifaddr);

  return Status;
}
