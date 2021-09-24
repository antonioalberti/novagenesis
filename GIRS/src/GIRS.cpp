/*
	NovaGenesis

	Name:		Generic Indirecteion Resolution System
	Object:		GIRS
	File:		GIRS.h
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

//Comentário teste
//
#ifndef _GIRS_H
#include "GIRS.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

GIRS::GIRS (string _LN, key_t _Key, string _Path) : Process (_LN, _Key, _Path)
{
  Block *PIRB = 0;
  string IRLN = "IR";

  NewBlock (IRLN, PIRB);

  // Run the base class GW
  RunGateway ();
}

GIRS::~GIRS ()
{
}

// Allocate a new block based on a name and add a Block on Blocks container
int GIRS::NewBlock (string _LN, Block *&_PB)
{
  if (_LN == "IR")
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

	  IR *PIR = new IR (_LN, this, Index, PGW, PHT, GetPath ());

	  _PB = (Block *)PIR;

	  InsertBlock (_PB);

	  return OK;
	}

  return ERROR;
}




