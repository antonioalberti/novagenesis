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
#define _APP_H

#include "Process.h"

#define ERROR 1
#define OK 0

class AppBrowser: public Process
{
	public:

		// Application role ("Client" or "Server")
		string	Role;
		
		// Constructor
		AppBrowser(string _LN, string _Role,key_t _Key, string _Path);

		// Destructor
		~AppBrowser();

		// Allocate a new block based on a name and add a Block on Blocks container
		int NewBlock(string _LN, Block *&_PB);
};

#endif






