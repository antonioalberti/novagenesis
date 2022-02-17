/*
	NovaGenesis

	Name:		Simple application process
	Object:		App
	File:		App.h
	Author:		Antonio M. Alberti
	Date:		11/2012
	Version:	0.1
*/

#ifndef _APP_H
#include "App.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

AppBrowser::AppBrowser(string _LN, string _Role, key_t _Key, string _Path):Process(_LN,_Key,_Path)
{
	Block	*PCoreB=0;
	string 	CoreLN="Core";
	Role=_Role;

	NewBlock(CoreLN,PCoreB);

	// Run the base class GW
	RunGateway();
}

AppBrowser::~AppBrowser()
{
}

// Allocate a new block based on a name and add a Block on Blocks container
int AppBrowser::NewBlock(string _LN, Block *&_PB)
{
	if (_LN == "Core" )
		{
			Block 	*PGWB=0;
			Block 	*PHTB=0;
			GW		*PGW=0;
			HT		*PHT=0;
			string 	GWLN="GW";
			string 	HTLN="HT";

			GetBlock(GWLN,PGWB);

			PGW=(GW*)PGWB;

			GetBlock(HTLN,PHTB);

			PHT=(HT*)PHTB;

			unsigned int Index=0;

			Index=GetBlocksSize();

			Core *PCore=new Core(_LN,this,Index,PGW,PHT,GetPath());

			_PB=(Block*)PCore;

			InsertBlock(_PB);

			return OK;
		}

	return ERROR;
}



