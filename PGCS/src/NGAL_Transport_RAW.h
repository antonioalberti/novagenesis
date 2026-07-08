/*
        NovaGenesis

        Name:		NovaGenesis Adaptation Layer — Transport RAW
        Object:		NGAL_Transport_RAW
        File:		NGAL_Transport_RAW.h
        Author:		Antonio Marcos Alberti
        Date:		07/2026
        Version:	0.1

        Copyright (C) 2026  Antonio Marcos Alberti

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

#ifndef _NGAL_TRANSPORT_RAW_H
#define _NGAL_TRANSPORT_RAW_H

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _STRING_H
#include <string>
#endif

#ifndef _NETINET_IF_ETHER_H
#include <netinet/if_ether.h>
#endif

#ifndef _LINUX_IF_PACKET_H
#include <linux/if_packet.h>
#endif

#ifndef _SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifndef _LINUX_IF_H
#include <linux/if.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

// Forward declarations
class PG;
class GW;

// Ethernet frame union for raw socket transmission
union ngal_ethframe
{
  struct
  {
    struct ethhdr header;
    unsigned char data[ETH_DATA_LEN];
  } field;
  unsigned char buffer[ETH_FRAME_LEN];
};

class NGAL_Transport_RAW
{
public:
  // Create a raw Ethernet socket
  static int CreateRawSocket(int& _SID);

  // Send one NGAL-PDU fragment via raw socket
  // Called as callback from NGAL_SAR::SendSegmented()
  static int SendFragment(int SSID, int ifindex,
                          unsigned char* SourceMAC, unsigned char* DestMAC,
                          unsigned char* FragmentData, unsigned int FragmentSize);

  // Main receive dispatcher thread (replaces SocketDispatcher3)
  // Runs in its own thread. Receives frames via poll(), feeds NGAL_SAR,
  // delivers completed messages via NGAL_CS.
  // Uses poll() on all SSIDs to avoid socket starvation.
  static void ReceiveDispatcher(PG* PPG);

private:
  // No child threads — the dispatcher does reassembly + delivery directly.
  // ChildReceiver is eliminated. The dispatcher uses non-blocking recvfrom()
  // with poll() to serve all sockets fairly.
};

#endif