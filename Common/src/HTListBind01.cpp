/*
	NovaGenesis

	Name:		HTListBind01
	Object:		HTListBind01
	File:		HTListBind01.cpp
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

#ifndef _HTLISTBIND01_H
#include "HTListBind01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

////#define DEBUG

HTListBind01::HTListBind01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

HTListBind01::~HTListBind01 ()
{
}

// Run the actions behind a received command line
// ng -list --b 0.1
int
HTListBind01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;

  HT *PHT = 0;
  Iterator it1;
  string PreviousKey = "";
  string Offset = "                    ";
  //unsigned int	StatusCode=0;
  //CommandLine 	*NewStatusS01=0;

  PHT = (HT *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  PB->S << endl << Offset << "(Hash Tables Size)" << endl;

  for (unsigned int j = 0; j < 19; j++)
	{
	  if (PHT->Bindings != 0)
		{
		  if (PHT->Bindings[j].size () > 0)
			{
			  PB->S << Offset << "Cat [" << j << "] = " << PHT->Bindings[j].size () << endl;

#ifdef DEBUG

			  for (it1=PHT->Bindings[j].begin(); it1!= PHT->Bindings[j].end(); it1++ )
				  {
					  if ((*it1).first != PreviousKey)
						  {
							  PB->S << endl << Offset << (*it1).first << "    " << '\t' << (*it1).second;
						  }
					  else
						  {
							  PB->S << "    " <<  '\t' << (*it1).second;
						  }

					  PreviousKey=(*it1).first;
				  }

			  cout<<endl<<endl;
#endif

			}
		  else
			{
			  //PB->S << Offset <<  "Empty container..." << endl;
			}

		  Status = OK;
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Invalid Bindings pointer)";

		  Status = ERROR;

		  break;
		}
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
