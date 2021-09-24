/*
	NovaGenesis

	Name:		Publish/Subscribe System
	Object:		NRNCS
	File:		NRNCS.h
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

#ifndef _NRNCS_H
#include "NRNCS.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

NRNCS::NRNCS (string _LN, key_t _Key, string _Path) : Process (_LN, _Key, _Path)
{
  Block *PNRB = 0;
  string NRLN = "NR";

  NewBlock (NRLN, PNRB);

  // Run the base class GW
  RunGateway ();
}

NRNCS::~NRNCS ()
{
}

// Allocate a new block based on a name and add a Block on Blocks container
int NRNCS::NewBlock (string _LN, Block *&_PB)
{
  if (_LN == "NR")
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

	  NR *PNR = new NR (_LN, this, Index, PGW, PHT, GetPath ());

	  _PB = (Block *)PNR;

	  InsertBlock (_PB);

	  return OK;
	}

  return ERROR;
}



