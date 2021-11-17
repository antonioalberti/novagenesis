/*
	NovaGenesis

	Name:		CoreRunInvite01
	Object:		CoreRunInvite01
	File:		CoreRunInvite01.cpp
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

#ifndef _CORERUNINVITE01_H
#include "CoreRunInvite01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

CoreRunInvite01::CoreRunInvite01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreRunInvite01::~CoreRunInvite01 ()
{
}

// Run the actions behind a received command line
// ng -run --invite 0.1 [ < 2 string Server 1 > ]
int
CoreRunInvite01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  Core *PCore = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *Publish = 0;
  CommandLine *PCL = 0;
  vector<string> Values;
  vector<Tuple *> PubNotify;
  vector<Tuple *> SubNotify;
  Tuple *ExtendedPeerAppTuple = new Tuple;
  string PayloadString;
  unsigned int NA = 0;
  vector<string> PeerData;
  Tuple *PT = 0;
  string OfferHash;
  string OfferFileName;

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 1)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, PeerData);

		  if (PeerData.size () > 0)
			{
			  PT = PCore->PeerTuples[PB->StringToInt (PeerData.at (1))];

			  PB->S << Offset << "(The peer is " << PeerData.at (0) << " " << PeerData.at (1) << ")" << endl;
			  PB->S << Offset << "(Its HID is = " << PT->Values[0] << ")" << endl;
			  PB->S << Offset << "(Its OSID is = " << PT->Values[1] << ")" << endl;
			  PB->S << Offset << "(Its PID is = " << PT->Values[2] << ")" << endl;
			  PB->S << Offset << "(Its BID is = " << PT->Values[3] << ")" << endl;

			  // Setting up the OSID as the space limiter
			  Limiters.push_back (PB->PP->Intra_Domain);

			  // Setting up this OS as the 1st source SCN
			  Sources.push_back (PB->PP->GetHostSelfCertifyingName ());

			  // Setting up this OS as the 2nd source SCN
			  Sources.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

			  // Setting up this process as the 3rd source SCN
			  Sources.push_back (PB->PP->GetSelfCertifyingName ());

			  // Setting up the PS block SCN as the 4th source SCN
			  Sources.push_back (PB->GetSelfCertifyingName ());

			  // Setting up the destination 1st source
			  Destinations.push_back (PCore->PSTuples[0]->Values[0]);

			  // Setting up the destination 2nd source
			  Destinations.push_back (PCore->PSTuples[0]->Values[1]);

			  // Setting up the destination 3rd source
			  Destinations.push_back (PCore->PSTuples[0]->Values[2]);

			  // Setting up the destination 4th source
			  Destinations.push_back (PCore->PSTuples[0]->Values[3]);

			  CreateServiceOffer (OfferFileName);

			  PCore->Debug.OpenOutputFile ();

			  PCore->Debug << "Inviting the peer " << PeerData.at (0) << " " << PeerData.at (1) << ". Creating the "
						   << OfferFileName << endl;

			  PCore->Debug.CloseFile ();

			  // Creating a new message
			  PB->PP->NewMessage (GetTime () + PCore->DelayBeforePublishingServiceOffer, 1, true, "Empty.txt",
								  OfferFileName, "Message.ngs", PB->GetPath (), Publish);

			  Publish->ConvertPayloadFromFileToCharArray ();

			  // Creating the ng -cl -m command line
			  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Publish, PCL);

			  // ***************************************************
			  // Generate the bindings to be published
			  // ***************************************************

			  // Put the file name on the binding value
			  Values.push_back (OfferFileName);

			  // First of all, set this extended tuple to have a publication notification
			  ExtendedPeerAppTuple->Values.push_back ("pub");

			  // Set the remaining elements on the tuple
			  for (unsigned int i = 0; i < PT->Values.size (); i++)
				{
				  ExtendedPeerAppTuple->Values.push_back (PT->Values[i]);
				}

			  // Configure the tuple to be informed of this publication
			  PubNotify.push_back (ExtendedPeerAppTuple);

			  // Calculates the offer file payload hash
			  PCore->GetFileContentHash (OfferFileName, OfferHash);

			  // Set the ng -p --notify 0.1
			  PMB->NewPublicationWithNotificationCommandLine ("0.1", 18, OfferHash, &Values, &PubNotify, &SubNotify, Publish, PCL);

			  // ***************************************************
			  // Generate the -info --payload command line
			  // ***************************************************

			  // Adding a ng -info --payload 01 [< 1 string Service_Offer.txt > ]
			  PMB->NewInfoPayloadCommandLine ("0.1", &Values, Publish, PCL);

			  // Publish the provenance information
			  Values.clear ();

			  Values.push_back (PB->GetSelfCertifyingName ());

			  // Binding < Hash(Payload), App::Core BID>
			  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 2, OfferHash, &Values, Publish, PCL);

			  Values.clear ();

			  Values.push_back (PB->PP->GetSelfCertifyingName ());

			  // Binding < Hash(Payload), App PID>
			  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 2, OfferHash, &Values, Publish, PCL);

			  Values.clear ();

			  Values.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

			  // Binding < Hash(Payload), App OSID>
			  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 2, OfferHash, &Values, Publish, PCL);

			  Values.clear ();

			  Values.push_back (PB->PP->GetHostSelfCertifyingName ());

			  // Binding < Hash(Payload), App PID>
			  PMB->NewCommonCommandLine ("-p", "--b", "0.1", 9, OfferHash, &Values, Publish, PCL);

			  // ***************************************************
			  // Generate the ng -message --type [ < 1 string 1 > ]
			  // ***************************************************

			  Publish->NewCommandLine ("-message", "--type", "0.1", PCL);

			  PCL->NewArgument (1);

			  PCL->SetArgumentElement (0, 0, PB->IntToString (Publish->GetType ()));

			  // ***************************************************
			  // Generate the ng -message --seq [ < 1 string 1 > ]
			  // ***************************************************

			  Publish->NewCommandLine ("-message", "--seq", "0.1", PCL);

			  PCL->NewArgument (1);

			  PCL->SetArgumentElement (0, 0, PB->IntToString (PCore->GetSequenceNumber ()));

			  // ******************************************************
			  // Generate the SCN
			  // ******************************************************

			  // Generate the SCN
			  PB->GenerateSCNFromMessageBinaryPatterns (Publish, SCN);

			  // Creating the ng -scn --s command line
			  PMB->NewSCNCommandLine ("0.1", SCN, Publish, PCL);

			  PB->S << Offset << "(The following message is to invite the peer)" << endl;

			  PB->S << "(" << endl << *Publish << ")" << endl;

			  // ******************************************************
			  // Finish
			  // ******************************************************

			  // Push the message to the GW input queue
			  PCore->PGW->PushToInputQueue (Publish);

			  delete ExtendedPeerAppTuple;

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

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

// This function enables you to change the content of the service offer.
int CoreRunInvite01::CreateServiceOffer (string &_OfferFileName)
{
  int Status = ERROR;
  File F1;
  Core *PCore = 0;
  vector<string> *Values = new vector<string>;
  string Offset = "                    ";

  PCore = (Core *)PB;

  unsigned int Complement = rand () % 10000000000000;

  // Set the file name
  _OfferFileName = "Service_Offer_" + PB->UnsignedIntToString (Complement) + ".txt";

  // Open the file to write
  F1.OpenOutputFile (_OfferFileName, PB->GetPath (), "DEFAULT");

  if (Values->size () == 14)
	{
	  F1 << "Offer from IoTTestApp" << endl;
	}
  else
	{
	  PB->S << Offset << "(ERROR: Wrong number of SLA parameters)" << endl;
	}

  F1.CloseFile ();

  delete Values;

  return Status;
}

