/*
	NovaGenesis

	Name:		HTRunInitialization01
	Object:		HTRunInitialization01
	File:		HTRunInitialization01.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.1

 	Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _HTRUNINITIALIZATION01_H
#include "HTRunInitialization01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

HTRunInitialization01::HTRunInitialization01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTRunInitialization01::~HTRunInitialization01 ()
{
}

// Run the actions behind a received command line
// ng -run --initialization 0.1
int
HTRunInitialization01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;

  int Category;
  string Key;
  string Value;
  vector<string> Values;
  HT *PHT = 0;
  string PID = "";
  string BID = "";
  string MyBlockIndexString = "";
  string ProcessLegibleNameSCN = "";
  stringstream ss;
  unsigned int Index = 0;
  string Offset = "                    ";
  string HashBlockLegibleName;

  PB->S << Offset << this->GetLegibleName () << endl;

  // ******************************************************
  // Calculate auxiliary variables and required input data
  // ******************************************************

  PID = PB->PP->GetSelfCertifyingName ();

  //cout << "MyProcessSCN = "<< MyProcessSCN << endl;

  BID = PB->GetSelfCertifyingName ();

  //cout << "MyBlockSCN = "<< MyBlockSCN << endl;

  PB->GenerateSCNFromCharArrayBinaryPatterns ("Process", ProcessLegibleNameSCN);

  //cout << "ProcessLegibleNameSCN = "<< ProcessLegibleNameSCN << endl;

  ss << Index;

  MyBlockIndexString = ss.str ();

  //cout << "MyBlockIndexString = "<< MyBlockIndexString << endl;

  PHT = (HT *)PB;

  PB->GenerateSCNFromCharArrayBinaryPatterns (PB->GetLegibleName (), HashBlockLegibleName);

  //PB->S << Offset <<  "(HashBlockLegibleName = " << HashBlockLegibleName << ")" <<endl;

  if (PID != "" && BID != "" && ProcessLegibleNameSCN != "" && MyBlockIndexString != "" && PHT != 0)
	{
	  PB->S << Offset << "(Moving to operational state)" << endl;

	  // ******************************************************
	  // Binding Hash("GW") to "GW"
	  // ******************************************************

	  // Setting up the category
	  Category = 1;

	  // Setting up the binding key
	  Key = HashBlockLegibleName;

	  // Setting up the binding value
	  Values.push_back ("HT");

	  // Creating the ng -sr --b 0.1 command line
	  PHT->StoreBinding (Category, Key, &Values);

	  // Clearing the values container
	  Values.clear ();

	  // ******************************************************
	  // Binding Hash("HT") to BID
	  // ******************************************************

	  // Setting up the category
	  Category = 2;

	  // Setting up the binding key
	  Key = HashBlockLegibleName;

	  // Setting up the binding value
	  Values.push_back (BID);

	  // Creating the ng -sr --b 0.1 command line
	  PHT->StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding BID to Hash("HT")
	  // ******************************************************

	  // Setting up the category
	  Category = 3;

	  // Setting up the binding key
	  Key = BID;

	  // Setting up the binding value
	  Values.push_back (HashBlockLegibleName);

	  // Creating the ng -sr --b 0.1 command line
	  PHT->StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding BID to HT index on Blocks container
	  // ******************************************************

	  // Setting up the category
	  Category = 13;

	  // Setting up the binding key
	  Key = BID;

	  // Setting up the value
	  Values.push_back (MyBlockIndexString);

	  // Store the binding
	  PHT->StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding Hash("Process") to "Process"
	  // ******************************************************

	  // Setting up the category
	  Category = 1;

	  // Setting up the binding key
	  Key = ProcessLegibleNameSCN;

	  // Setting up the value
	  Values.push_back ("Process");

	  PHT->StoreBinding (Category, Key, &Values);

	  Values.clear ();

	  // ******************************************************
	  // Binding PID to Hash("Process")
	  // ******************************************************

	  // Setting up the category
	  Category = 3;

	  // Setting up the binding key
	  Key = PID;

	  // Setting up the value
	  Values.push_back (ProcessLegibleNameSCN);

	  // Store the binding
	  PHT->StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding Hash("Process") to Process SCN
	  // ******************************************************

	  // Setting up the category
	  Category = 2;

	  // Setting up the binding key
	  Key = ProcessLegibleNameSCN;

	  // Setting up the value
	  Values.push_back (PID);

	  // Store the binding
	  PHT->StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  // ******************************************************
	  // Binding PID to BID
	  // ******************************************************

	  // Setting up the category
	  Category = 5;

	  // Setting up the binding key
	  Key = PID;

	  // Setting up the value
	  Values.push_back (BID);

	  // Store the binding
	  PHT->StoreBinding (Category, Key, &Values);

	  // Clear the values
	  Values.clear ();

	  PB->State = "operational";

	  PB->S << Offset << "(State: Operational)" << endl << endl << endl;

	  PB->S << Offset << "(Done)" << endl << endl << endl;

	  Status = OK;
	}

  return Status;
}
