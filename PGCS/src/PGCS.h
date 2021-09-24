/*
	NovaGenesis

	Name:		Proxy, Gateway and Controller Service
	Object:		PGCS
	File:		PGCS.h
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.3

 	Copyright (C) 2021  Antonio Marcos Alberti

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

#ifndef _PGCS_H
#define _PGCS_H

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#ifndef _STDIO_H
#include "stdio.h"
#endif

#define ERROR 1
#define OK 0

class PGCS : public Process {
 public:

  // Threads container
  tthread::thread *Threads[MAX_NUMBER_OF_THREADS];

  // Number of threads
  unsigned int NoT;

  bool AlreadyCreatedPeerPGCSFrameReceivingIEEEThread;

  bool AlreadyCreatedPeerPGCSFrameReceivingUDPThread;

  bool HasCore;

  // A port to be used in only the case a TCP/IP connection is required, e.g. when TCP/IP is the Stack or other configuration
  string Port;

  // The role of this PGCS in domain. Options are Intra_Domain or Inter_Domain
  string Role;

  // Container to store the peer substrate technologies stack
  vector<string> *Stacks;

  // Container to store the peer role in domain
  vector<string> *Roles;

  // Container to store the peer substrate OS interfaces
  vector<string> *Interfaces;

  // Container to store the peer PGCS substrate identifiers
  vector<string> *Identifiers;

  // Container to store the maximum size of the data blocks for every interface
  vector<unsigned int> *Sizes;

  // TODO: FIXP/Update - We removed all related to this vector that is redundant while controlling peer PGCSes. Its main usage was in avoiding exposition messages.
  // However, in Docker containers such feature requires further investigation.
  // Container to store the peer NovaGenesis HIDs
  //vector<string> *Host_Self_Verifying_Names;

  // Container to store the SSID of the serving sockets
  vector<int> *SSIDs;

  // TODO: FIXP/Update - Added to deal with the case of PGCS -de initialization
  // Container to store the SSID of the serving sockets
  vector<int> *CSIDs;

  // Auxiliary variable to store the MAC address a certain interface
  string MyMACAddress;

  // Constructor
  PGCS (string _LN, key_t _Key, string _flag, string _Port, string _Role, vector<string> *_Stacks, vector<string> *_Roles, vector<
	  string> *_Interfaces, vector<string> *_Identifiers, vector<unsigned int> *_Sizes, string _Path);

  // Destructor
  ~PGCS ();        // Auxiliary function to select a valid Linux interface at local Linux instance

  // Allocate a new block based on a name and add a Block on Blocks container
  int NewBlock (string _LN, Block *&_PB);

  // Auxiliary

  friend class PGRunInitialization01;
  friend class PGRunHello01;
  friend class PGMsgCl01;
  friend class PG;
  friend class Core;
};

#endif
