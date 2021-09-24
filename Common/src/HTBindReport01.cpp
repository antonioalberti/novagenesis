/*
	NovaGenesis

	Name:		HTBindReport01
	Object:		HTBindReport01
	File:		HTBindReport01.cpp
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

#ifndef _HTBINDREPORT01_H
#include "HTBindReport01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

////#define DEBUG

HTBindReport01::HTBindReport01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{

#ifdef DEBUG

  if (PB->PP->GetLegibleName() == "HTS")
	  {
		  // Set the file name
		  string FileName="Bindings_Report.txt";

		  // Open the file to write
		  F1.OpenOutputFile(FileName,PB->GetPath(),"DEFAULT");

		  F1.CloseFile();
	  }
#endif
}

HTBindReport01::~HTBindReport01 ()
{
}

// Run the actions behind a received command line
// ng -bind --report 0.1
int
HTBindReport01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;

  HT *PHT = 0;
  Iterator it1;
  string PreviousKey = "";
  string Offset = "                    ";
  //unsigned int	StatusCode=0;
  //CommandLine 	*NewStatusS01=0;

  F1.OpenOutputFile ();

  PHT = (HT *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  F1 << endl << Offset << "(Hash Tables Size)" << endl;

  for (unsigned int j = 0; j < 19; j++)
	{
	  if (PHT->Bindings != 0)
		{
		  if (PHT->Bindings[j].size () > 0)
			{
			  F1 << Offset << "Cat [" << j << "] = " << PHT->Bindings[j].size () << endl;

#ifdef DEBUG

			  for (it1=PHT->Bindings[j].begin(); it1!= PHT->Bindings[j].end(); it1++ )
				  {
					  if ((*it1).first != PreviousKey)
						  {
							  F1 << endl << Offset << (*it1).first << "    " << '\t' << (*it1).second;
						  }
					  else
						  {
							  F1 << "    " <<  '\t' << (*it1).second;
						  }

					  PreviousKey=(*it1).first;
				  }

			  F1<<endl<<endl;
#endif

			}
		  else
			{
			  F1 << Offset << "Empty container..." << endl;
			}

		  Status = OK;
		}
	  else
		{
		  F1 << Offset << "(ERROR: Invalid Bindings pointer)";

		  Status = ERROR;

		  break;
		}
	}

  F1.CloseFile ();

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

