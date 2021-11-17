/*
	NovaGenesis

	Name:		Proxy, Gateway and Controller System for Internet of Things
	Object:		execPGCS
	File:		execPGCS.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.1

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
#include "PGCS.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

int main (int argc, char *argv[])
{
  bool Problem = false;

  if (argc >= 5)
	{
	  vector<string> Stacks;
	  vector<string> Roles;
	  vector<string> Interfaces;
	  vector<string> Identifiers;
	  vector<unsigned int> Sizes;

	  string Path = argv[1];
	  string Port = argv[2];
	  string Role = argv[3];
	  string Alternative = argv[4];

	  cout << "*******************************************************************" << endl;
	  cout << "*                                                                 *" << endl;
	  cout << "*  NovaGenesis(NG) Proxy/Gateway/Controller Service v0.5          *" << endl;
	  cout << "*  Copyright Antonio Marcos Alberti - Inatel - April 2021         *" << endl;
	  cout << "*                                                                 *" << endl;
	  cout << "*                                                                 *" << endl;
	  cout << "*******************************************************************" << endl << endl;

	  // ************************** Adaptation Layer **************************

	  // Bellow, are the possible alternatives for ./PGCS execution. Repair on the number of arguments and their meaning.
	  // It is recommended to map any technology to a set of one registry in each of the vectors above.
	  // Therefore, a new interface for a certain technology should detail, respectively, the type of stack, the role of this PGCS in the domain, the interface used for this stack at
	  // OS, the identifier (address) of the peer PGCS on other machines, and the size of the MTU on the network.


	  if (Alternative == "-p") // Explicitly specify the peers
		{
		  if ((argc - 5) % 5 == 0)
			{
			  for (int i = 5; i < argc; i = i + 5)
				{
				  string Temp1 = argv[i];     // Stack
				  string Temp2 = argv[i + 1]; // Role
				  string Temp3 = argv[i + 2]; // Interface
				  string Temp4 = argv[i + 3]; // Identifier
				  unsigned int Temp5 = 0;           // Size

				  stringstream ss;

				  ss << argv[i + 4];

				  ss >> Temp5;

				  Stacks.push_back (Temp1);
				  Roles.push_back (Temp2);
				  Interfaces.push_back (Temp3);
				  Identifiers.push_back (Temp4);
				  Sizes.push_back (Temp5);

				  cout << "(The I/O path is " << Path << ")" << endl;
				  cout << "(The pull socket port is " << Port << ".)" << endl;
				  cout << "(The role of this PGCS in domains is " << Role << ".)" << endl;
				  cout << "(The PGCS is creating a new interface with the " << Temp1 << " protocol stack.)" << endl;
				  cout << "(The role of the peer PGCS is " << Temp2 << ".)" << endl;
				  cout << "(The interface for the stack " << Temp1 << " will be " << Temp3 << " interface.)" << endl;
				  cout << "(The peer PGCS has the identifier " << Temp4 << ".)" << endl;
				  cout << "(The maximum data block size for NovaGenesis messages on this interface is " << Temp5
					   << " bytes.)" << endl;
				}
			}
		  else
			{
			  Problem = true;

			  cout << "(ERROR: Wrong number of arguments)" << endl;
			}

		  if (Stacks.size () == 0)
			{
			  cout << "(No peer PGCS information was provided. PGCS operation will be limited to this host.)" << endl;
			}

		  // Set the shm key
		  key_t Key = 11;

		  // Create a process instance
		  PGCS execPGS ("PGCS", Key, "No_Core", Port, Role, &Stacks, &Roles, &Interfaces, &Identifiers, &Sizes, Path);
		}

	  if (Alternative == "-pc") // Explicitly specify the peers, create a core block
		{
		  if ((argc - 5) % 5 == 0)
			{
			  for (int i = 5; i < argc; i = i + 5)
				{
				  string Temp1 = argv[i];     // Stack
				  string Temp2 = argv[i + 1]; // Role
				  string Temp3 = argv[i + 2]; // Interface
				  string Temp4 = argv[i + 3]; // Identifier
				  unsigned int Temp5;           // Size

				  stringstream ss;

				  ss << argv[i + 4];

				  ss >> Temp5;

				  Stacks.push_back (Temp1);
				  Roles.push_back (Temp2);
				  Interfaces.push_back (Temp3);
				  Identifiers.push_back (Temp4);
				  Sizes.push_back (Temp5);

				  cout << "(-pc option started)" << endl;
				  cout << "(The I/O path is " << Path << ")" << endl;
				  cout << "(The pull socket port is " << Port << ".)" << endl;
				  cout << "(The role of this PGCS in domains is " << Role << ".)" << endl;
				  cout << "(The PGCS is creating a new interface with the " << Temp1 << " protocol stack.)" << endl;
				  cout << "(The role of the peer PGCS is " << Temp2 << ".)" << endl;
				  cout << "(The interface for the stack " << Temp1 << " will be " << Temp3 << " interface.)" << endl;
				  cout << "(The peer PGCS has the identifier " << Temp4 << ".)" << endl;
				  cout << "(The maximum data block size for NovaGenesis messages on this interface is " << Temp5
					   << " bytes.)" << endl;
				}
			}
		  else
			{
			  Problem = true;

			  cout << "(ERROR: Wrong number of arguments)" << endl;
			}

		  if (Stacks.size () == 0)
			{
			  cout << "(No peer PGCS information was provided. PGCS operation will be limited to this host.)" << endl;
			}

		  // Set the shm key
		  key_t Key = 11;

		  // Create a process instance
		  PGCS execPGS ("PGCS", Key, "Create_Core", Port, Role, &Stacks, &Roles, &Interfaces, &Identifiers, &Sizes, Path);
		}

	  if (Alternative == "-ped") // Specify peers and left PGCS broadcast to other peers
		{
		  string Temp10 = argv[5];     // Interface

		  if ((argc - 6) % 5 == 0)
			{
			  for (int i = 6; i < argc; i = i + 5)
				{
				  string Temp1 = argv[i];     // Stack
				  string Temp2 = argv[i + 1]; // Role
				  string Temp3 = argv[i + 2]; // Interface
				  string Temp4 = argv[i + 3]; // Identifier
				  unsigned int Temp5 = 1000;           // Size

				  stringstream ss;

				  ss << argv[i + 4];

				  ss >> Temp5;

				  Stacks.push_back (Temp1);
				  Roles.push_back (Temp2);
				  Interfaces.push_back (Temp3);
				  Identifiers.push_back (Temp4);
				  Sizes.push_back (Temp5);

				  cout << "(The I/O path is " << Path << ")" << endl;
				  cout << "(The pull socket port is " << Port << ".)" << endl;
				  cout << "(The role of this PGCS in domains is " << Role << ".)" << endl;
				  cout << "(The PGCS is creating a new interface with the " << Temp1 << " protocol stack.)" << endl;
				  cout << "(The role of the peer PGCS is " << Temp2 << ".)" << endl;
				  cout << "(The interface for the stack " << Temp1 << " will be " << Temp3 << " interface.)" << endl;
				  cout << "(The peer PGCS has the identifier " << Temp4 << ".)" << endl;
				  cout << "(The maximum data block size for NovaGenesis messages on this interface is " << Temp5
					   << " bytes.)" << endl;
				  cout << "(The PGCS will also try to discover peers using the Interface " << Temp10 << ".)" << endl;
				}
			}
		  else
			{
			  Problem = true;

			  cout << "(ERROR: Wrong number of arguments)" << endl;
			}

		  if (Stacks.size () == 0)
			{
			  cout << "(No peer PGCS information was provided. PGCS operation will be limited to this host.)" << endl;
			}

		  Stacks.push_back ("Ethernet");
		  Roles.push_back ("Intra_Domain");
		  Interfaces.push_back (Temp10);
		  Identifiers.push_back ("FF:FF:FF:FF:FF:FF");

		  //TODO: FIXP/Update - Increased MTU to improve throughput
		  Sizes.push_back (1480);

		  // Set the shm key
		  key_t Key = 11;

		  // Create a process instance
		  PGCS execPGS ("PGCS", Key, "No_Core", Port, Role, &Stacks, &Roles, &Interfaces, &Identifiers, &Sizes, Path);
		}

	  if (Alternative == "-de") // Discover peers doing broadcast at informed interface
		{
		  cout << "(The I/O path is " << Path << ")" << endl;

		  string Temp10 = argv[5];     // Interface

		  cout << "(The PGCS will try to discover peers using the Interface " << Temp10 << ".)" << endl;

		  Stacks.push_back ("Ethernet");
		  Roles.push_back ("Intra_Domain");
		  Interfaces.push_back (Temp10);
		  Identifiers.push_back ("FF:FF:FF:FF:FF:FF");

		  //TODO: FIXP/Update - Increased MTU to improve throughput
		  Sizes.push_back (1480);

		  // Set the shm key
		  key_t Key = 11;

		  // Create a process instance
		  PGCS execPGS ("PGCS", Key, "No_Core", Port, Role, &Stacks, &Roles, &Interfaces, &Identifiers, &Sizes, Path);
		}

		// TODO FIXP/Update - Created a novel format to start PGCS with core and support peer discovery at the same time
	  if (Alternative == "-dec") // Discover peers doing broadcast at informed interface and start Core block for contract negotiation with things
		{
		  cout << "(The I/O path is " << Path << ")" << endl;

		  string Temp10 = argv[7];     // Interface

		  cout << "(The PGCS will try to discover peers using the Interface " << Temp10 << ".)" << endl;

		  Stacks.push_back ("Ethernet");
		  Roles.push_back ("Intra_Domain");
		  Interfaces.push_back (Temp10);
		  Identifiers.push_back ("FF:FF:FF:FF:FF:FF");

		  //TODO: FIXP/Update - Increased MTU to improve throughput
		  Sizes.push_back (1480);

		  // Set the shm key
		  key_t Key = 11;

		  // Create a process instance
		  PGCS execPGS ("PGCS", Key, "Create_Core", Port, Role, &Stacks, &Roles, &Interfaces, &Identifiers, &Sizes, Path);
		}

	  if (Alternative == "-l") // Run PGCS locally without core Block
		{

		  cout << "(The I/O path is " << Path << ")" << endl;
		  cout << "(The PGCS is running locally, without using network.)" << endl;

		  // Set the shm key
		  key_t Key = 11;

		  // Create a process instance
		  PGCS execPGS ("PGCS", Key, "No_Core", Port, Role, &Stacks, &Roles, &Interfaces, &Identifiers, &Sizes, Path);
		}

	  if (Alternative == "-lc") // Run PGCS locally starting the core for manage contracting
		{

		  cout << "(The I/O path is " << Path << ")" << endl;
		  cout << "(The PGCS is running locally, without using network. Starting the core to manage contracts.)"
			   << endl;

		  // Set the shm key
		  key_t Key = 11;

		  // Create a process instance
		  PGCS execPGS ("PGCS", Key, "Create_Core", Port, Role, &Stacks, &Roles, &Interfaces, &Identifiers, &Sizes, Path);
		}
	}
  else
	{
	  Problem = true;

	  cout << "(ERROR: Wrong number of arguments)" << endl;
	}

  if (Problem == true)
	{
	  cout
		  << "(Usage: ./PGCS Path Port Role -p Stack1 Role1 Interface1 Identifier1 Size1 ... StackN RoleN InterfaceN IdentifierN SizeN to specify the peers.)"
		  << endl;
	  cout
		  << "(Usage: ./PGCS Path Port Role -ped Interface Stack1 Role1 Interface1 Identifier1 Size1 ... StackN RoleN InterfaceN IdentifierN SizeN to specify the peers.)"
		  << endl;
	  cout
		  << "(Usage: ./PGCS Path Port Role -pc Stack1 Role1 Interface1 Identifier1 Size1 ... StackN RoleN InterfaceN IdentifierN SizeN to specify the peers and create a Core block.)"
		  << endl;
	  cout
		  << "(Usage: ./PGCS Path Port Role -de Interface to discover peers using informed interface and Ethernet technology.)"
		  << endl;
	  cout << "(Usage: ./PGCS Path Port Role -l to run locally without peers.)" << endl << endl;
	}

  return 0;
}




