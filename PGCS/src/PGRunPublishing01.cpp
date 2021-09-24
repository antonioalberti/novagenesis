/*
	NovaGenesis

	Name:		PGRunPublishing01
	Object:		PGRunPublishing01
	File:		PGRunPublishing01.cpp
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

#ifndef _PGRUNPUBLISHING01_H
#include "PGRunPublishing01.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

PGRunPublishing01::PGRunPublishing01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

PGRunPublishing01::~PGRunPublishing01 ()
{
}

// Run the actions behind a received command line
// ng -run --publishing 0.1
int
PGRunPublishing01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  PG *PPG = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  CommandLine *PCL;
  Message *Publish = 0;
  vector<string> Values;
  string HashProcessLegibleName;

  PPG = (PG *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // ******************************************************************
  // Generating a message to publish the binding hash("PGCS"), PGCS PID
  // ******************************************************************

  if (PPG->PSTuples.size () > 0)
	{
	  for (unsigned int t = 0; t < PPG->PSTuples.size (); t++)
		{
		  //PB->S << Offset <<  "(Generating a message to publish the binding <hash(\"PGCS\"), PGCS PID> )"<<endl;

		  PB->S << Offset << "(Everything ok!)" << endl;

		  // Setting up the OSID as the space limiter
		  Limiters.push_back (PB->PP->Intra_Domain);

		  // Setting up the this OS as the 1st source SCN
		  Sources.push_back (PB->PP->GetHostSelfCertifyingName ());

		  // Setting up the this OS as the 2nd source SCN
		  Sources.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

		  // Setting up the this process as the 3rd source SCN
		  Sources.push_back (PB->PP->GetSelfCertifyingName ());

		  // Setting up the PS block SCN as the 4th source SCN
		  Sources.push_back (PB->GetSelfCertifyingName ());

		  // Setting up the this OS as the 1st source SCN
		  Destinations.push_back (PPG->PSTuples[t]->Values[0]);

		  // Setting up the this OS as the 2nd source SCN
		  Destinations.push_back (PPG->PSTuples[t]->Values[1]);

		  // Setting up the this process as the 3rd source SCN
		  Destinations.push_back (PPG->PSTuples[t]->Values[2]);

		  // Setting up the PS block SCN as the 4th source SCN
		  Destinations.push_back (PPG->PSTuples[t]->Values[3]);

		  // Creating a new message
		  PB->PP->NewMessage (GetTime (), 0, false, Publish);

		  // Creating the ng -cl -m command line
		  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Publish, PCL);

		  // ***************************************************
		  // Generate the bindings to be published
		  // ***************************************************
		  Values.push_back (PB->PP->GetSelfCertifyingName ());

		  PMB->GenerateSCNFromCharArrayBinaryPatterns ("PGCS", HashProcessLegibleName);

		  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 2, HashProcessLegibleName, &Values, Publish, PCL);

		  // ***************************************************
		  // Generate the SCN
		  // ***************************************************
		  PB->GenerateSCNFromMessageBinaryPatterns (Publish, SCN);

		  // Creating the ng -scn --s command line
		  PMB->NewSCNCommandLine ("0.1", SCN, Publish, PCL);

		  // ******************************************************
		  // Finish
		  // ******************************************************

		  // Push the message to the GW input queue
		  PPG->PGW->PushToInputQueue (Publish);

		  Limiters.clear ();
		  Sources.clear ();
		  Destinations.clear ();
		  Values.clear ();
		}

	}
  else
	{
	  PB->S << Offset << "(ERROR: Not aware of any NRNCS)" << endl;
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
