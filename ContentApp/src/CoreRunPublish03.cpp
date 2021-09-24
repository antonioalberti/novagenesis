/*
	NovaGenesis

	Name:		CoreRunPublish03
	Object:		CoreRunPublish03
	File:		CoreRunPublish03.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.2

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

#ifndef _CORERUNPUBLISH03_H
#include "CoreRunPublish03.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

CoreRunPublish03::CoreRunPublish03 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreRunPublish03::~CoreRunPublish03 ()
{
}

// Run the actions behind a received command line
// ng -run --publish 0.3 [ < 1 string FileName > ]
int
CoreRunPublish03::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  Core *PCore = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  vector<string> Values;
  string PayloadHash;
  vector<Tuple *> PubNotify;
  vector<Tuple *> SubNotify;
  string PayloadString;
  unsigned int NA = 0;
  vector<string> FileName;
  Tuple *PT = 0;

  PCore = (Core *)PB;

  PB->S << endl << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 1)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, FileName);

		  if (FileName.size () == 1)
			{
			  PB->S << Offset << "(Going to publish the file named " << FileName[0] << ".)" << endl;
			  PB->S << Offset << "(Path = " << PB->GetPath() << ")" << endl;

			  for (unsigned int i = 0; i < PCore->PeerServerAppTuples.size (); i++)
				{
				  Tuple *ExtendedPeerAppTuple = new Tuple;

				  // Set this extended tuple to have a publication notification
				  ExtendedPeerAppTuple->Values.push_back ("pub");

				  // Copy the related Tuple pointer
				  PT = PCore->PeerServerAppTuples[i];

				  PB->S << Offset << "(Repository " << i << ")" << endl;
				  //PB->S << Offset << "(HID = " << PT->Values[0] << ")" << endl;
				  //PB->S << Offset << "(OSID = " << PT->Values[1] << ")" << endl;
				  //PB->S << Offset << "(PID = " << PT->Values[2] << ")" << endl;
				  //PB->S << Offset << "(BID = " << PT->Values[3] << ")" << endl;

				  // Set the remaining elements on the tuple
				  for (unsigned int j = 0; j < PT->Values.size (); j++)
					{
					  ExtendedPeerAppTuple->Values.push_back (PT->Values[j]);
					}

				  // Configure the tuple to be informed of this publication
				  PubNotify.push_back (ExtendedPeerAppTuple);
				}

			  CreatePublishMessage (FileName[0], PubNotify, SubNotify);

			  for (unsigned int m = 0; m < PubNotify.size (); m++)
				{
				  Tuple *PET = PubNotify[m];

				  //PB->S << Offset << "(Deleting the extended tuple number " << m << ")" << endl;

				  delete PET;
				}

			  if (ScheduledMessages.size () > 0)
				{
				  Message *Temp = ScheduledMessages.at (0);

				  Temp->MarkToDelete ();

				  // Make the scheduled messages vector empty
				  ScheduledMessages.clear ();
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;
			}
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
	}

  PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

void CoreRunPublish03::CreatePublishMessage (string _FileName, vector<Tuple *> &_PubNotify, vector<Tuple *> &_SubNotify)
{
  string Offset = "                    ";
  Core *PCore = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *Publish = 0;
  CommandLine *PCL = 0;
  vector<string> Values;
  string PayloadHash;
  char *Payload = 0;
  long long PayloadSize = 0;
  string PayloadString;
  vector<string> FileName;

  PCore = (Core *)PB;

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

  unsigned int HowManyPSs = PCore->PSTuples.size ();

  if (HowManyPSs > 0)
	{
	  unsigned int Target = rand () % HowManyPSs;

	  //PB->S << Offset <<"(Target = "<<Target<<")"<< endl;

	  // Setting up the destination 1st source
	  Destinations.push_back (PCore->PSTuples[Target]->Values[0]);

	  // Setting up the destination 2nd source
	  Destinations.push_back (PCore->PSTuples[Target]->Values[1]);

	  // Setting up the destination 3rd source
	  Destinations.push_back (PCore->PSTuples[Target]->Values[2]);

	  // Setting up the destination 4th source
	  Destinations.push_back (PCore->PSTuples[Target]->Values[3]);

	  // Creating a new message
	  PB->PP->NewMessage (GetTime (), 1, true, "Not_Necessary", _FileName, "Not_Necessary", PB->GetPath (), Publish);

	  Publish->ConvertPayloadFromFileToCharArray ();

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Publish, PCL);

	  // ***************************************************
	  // Generate the bindings to be published
	  // ***************************************************

	  // Get the payload to be published
	  Publish->GetPayloadFromCharArray (Payload);

	  // Get the payload size
	  Publish->GetPayloadSize (PayloadSize);

	  if (PayloadSize > 0)
		{
		  // Allocates a new subscription
		  Publication *PP;

		  PCore->NewPublication (PP);

		  PP->Timestamp = GetTime ();

		  // Calculates the offer file payload hash
		  PCore->GetFileContentHash (_FileName, PayloadHash);

		  //PCore->Debug.OpenOutputFile();

		  //PCore->Debug << "Publishing the file "<< _FileName << " with hash code equal to " << PayloadHash << endl;

		  //PCore->Debug.CloseFile();

		  //PB->S << endl << Offset <<"(The payload hash is " << PayloadHash << ")"<< endl;

		  // Put the file name on the binding value
		  Values.push_back (_FileName);

		  // Set the ng -p --notify 0.1
		  PMB->NewPublicationWithNotificationCommandLine ("0.1", 18, PayloadHash, &Values, &_PubNotify, &_SubNotify, Publish, PCL);

		  // ***************************************************
		  // Generate the -info --payload command line
		  // ***************************************************

		  // Adding a ng -info --payload 01 [ < 1 string Service_Offer.txt > ]
		  PMB->NewInfoPayloadCommandLine ("0.1", &Values, Publish, PCL);

		  // Publish the provenance information
		  Values.clear ();

		  Values.push_back (PB->GetSelfCertifyingName ());

		  // Binding < Hash(Payload), App::Core BID>
		  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 2, PayloadHash, &Values, Publish, PCL);

		  Values.clear ();

		  Values.push_back (PB->PP->GetSelfCertifyingName ());

		  // Binding < Hash(Payload), App PID>
		  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 2, PayloadHash, &Values, Publish, PCL);

		  Values.clear ();

		  Values.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

		  // Binding < Hash(Payload), App OSID>
		  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 2, PayloadHash, &Values, Publish, PCL);

		  Values.clear ();

		  Values.push_back (PB->PP->GetHostSelfCertifyingName ());

		  // Binding < Hash(Payload), App PID>
		  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 9, PayloadHash, &Values, Publish, PCL);

		  // ***************************************************
		  // Generate the ng -message --type [ < 1 string 1 > ]
		  // ***************************************************

		  //Publish->NewCommandLine("-message","--type","0.1",PCL);

		  //PCL->NewArgument(1);

		  //PCL->SetArgumentElement(0,0,PB->IntToString(Publish->GetType()));

		  // ***************************************************
		  // Generate the ng -message --seq [ < 1 string 1 > ]
		  // ***************************************************

		  //Publish->NewCommandLine("-message","--seq","0.1",PCL);

		  //PCL->NewArgument(1);

		  //PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

		  // ******************************************************
		  // Generate the SCN
		  // ******************************************************

		  // Generate the SCN
		  PB->GenerateSCNFromMessageBinaryPatterns (Publish, SCN);

		  // Creating the ng -scn --s command line
		  PMB->NewSCNCommandLine ("0.1", SCN, Publish, PCL);

		  // Stores the SCN on the Publication object for future reference
		  PP->Key = SCN;

		  PB->S << Offset <<"(The following message was published to the peer)"<< endl;

		  PB->S << "(" << endl << *Publish << ")"<< endl;

		  //long long xx=0;

		  //Publish->GetMessageSize(xx);

		  //PB->S << Offset <<"(The message has size equal to "<<xx<< " bytes.)"<< endl;

		  // ******************************************************
		  // Finish
		  // ******************************************************

		  // Push the message to the GW input queue
		  PCore->PGW->PushToInputQueue (Publish);
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Invalid size)" << endl;
		}
	}
}
