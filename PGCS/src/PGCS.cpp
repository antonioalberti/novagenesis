/*
	NovaGenesis

	Name:		Proxy, Gateway and Controller System
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
#include "PGCS.h"
#endif

PGCS::PGCS (string _LN, key_t _Key, string _flag, string _Port, string _Role, vector<string> *_Stacks, vector<string> *_Roles,
			vector<string> *_Interfaces, vector<string> *_Identifiers, vector<unsigned int> *_Sizes,
			string _Path) : Process (_LN, _Key, _Path)
{
  // For PG
  Block *PPGB = 0;

  // For Core
  Block *PCoreB = 0;

  for (unsigned int f = 0; f < MAX_NUMBER_OF_THREADS; f++)
	{
	  Threads[f] = NULL;
	}

  NoT = 0;

  Port = _Port;
  Role = _Role;
  Stacks = _Stacks;
  Roles = _Roles;
  Interfaces = _Interfaces;
  Identifiers = _Identifiers;
  Sizes = _Sizes;

  SSIDs = new vector<int>;

  CSIDs = new vector<int>; // TODO: FIXP/Update - adding new to this vector.

  MyMACAddress = "";

  AlreadyCreatedPeerPGCSFrameReceivingUDPThread = false;
  AlreadyCreatedPeerPGCSFrameReceivingIEEEThread = false;
  HasCore = false;

  // TODO: FIXP/Update - The following call brings problem in Docker Containers. Commenting it. Please, provide the run of this script clean.sh before running PGCS
  //string _Command = "sh clean.sh";
  //FILE *f1;
  //char OUTPUT[2048];

  //cout << endl << "Running the following command to clean previous NG runs = " << _Command << endl << endl;

  //f1 = popen (_Command.c_str (), "r");

  //if (fgets (OUTPUT, sizeof (OUTPUT) - 1, f1) != NULL)
//	{
//	  cout << OUTPUT;

//	  cout << "Cleaned shared memory buffers from previous NG run in this system..." << endl << endl;
//	}
//  else
//	{
//	  cout << "Nothing to clean or file not found..." << endl << endl;
//	}

  // Close the file
  //pclose (f1);

  NewBlock ("PG", PPGB);

  // Cast to PG pointer
  PG *PPG = (PG *)PPGB;

  if (_flag == "Create_Core")
	{
	  HasCore = true;

	  NewBlock ("Core", PCoreB);

	  // Cast to PG pointer
	  Core *PCore = (Core *)PCoreB;
	}

  Stacks->size ();

  // Run the base class GW
  RunGateway ();

  delete SSIDs;

  // Wait for the threads to finish
  for (unsigned int j = 0; j < MAX_NUMBER_OF_THREADS; j++)
	{
	  if (Threads[j] != 0)
		{
		  Threads[j]->join ();

		  delete Threads[j];
		}
	}
}

PGCS::~PGCS ()
{
  delete SSIDs; // TODO: FIXP/Update - adding delete to these vectors.

  delete CSIDs;
}

// Allocate a new block based on a name and add a Block on Blocks container
int PGCS::NewBlock (string _LN, Block *&_PB)
{
  if (_LN == "PG")
	{
	  Block *PGWB = 0;
	  Block *PHTB = 0;
	  GW *PGW = 0;
	  HT *PHT = 0;
	  string GWLN = "GW";
	  string HTLN = "HT";

	  GetBlock (GWLN, PGWB);

	  PGW = (GW *)PGWB;

	  GetBlock (HTLN, PHTB);

	  PHT = (HT *)PHTB;

	  unsigned int Index = 0;

	  Index = GetBlocksSize ();

	  PG *PPG = new PG (_LN, this, Index, PGW, PHT, GetPath ());

	  _PB = (Block *)PPG;

	  InsertBlock (_PB);

	  return OK;
	}

  if (_LN == "Core")
	{
	  Block *PGWB = 0;
	  Block *PHTB = 0;
	  GW *PGW = 0;
	  HT *PHT = 0;
	  string GWLN = "GW";
	  string HTLN = "HT";

	  GetBlock (GWLN, PGWB);

	  PGW = (GW *)PGWB;

	  GetBlock (HTLN, PHTB);

	  PHT = (HT *)PHTB;

	  unsigned int Index = 0;

	  Index = GetBlocksSize ();

	  Core *PCore = new Core (_LN, "Core", this, Index, PGW, PHT, GetPath ());

	  _PB = (Block *)PCore;

	  InsertBlock (_PB);

	  return OK;
	}

  return ERROR;
}






