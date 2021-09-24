/*
	NovaGenesis

	Name:		CoreRunContentPublish01
	Object:		CoreRunContentPublish01
	File:		CoreRunContentPublish01.cpp
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

#ifndef _CORERUNCONTENTPUBLISH01_H
#include "CoreRunContentPublish01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

//#define DEBUG // To follow message processing
////#define DEBUG // To follow message processing regarding traffic profile



CoreRunContentPublish01::CoreRunContentPublish01 (string _LN, Block *_PB, MessageBuilder *_PMB)
	: Action (_LN, _PB, _PMB)
{
}

CoreRunContentPublish01::~CoreRunContentPublish01 ()
{
}

// TODO: FIXP/Update - This function has been modified in August 2021.

// Run the actions behind a received command line
// ng -run --contentpublish _Version
int
CoreRunContentPublish01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string Offset = "                    ";
  Core *PCore = 0;
  CommandLine *PCL = 0;
  Message *RunPhotoPublish = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  struct dirent *dirpent;
  DIR *dirp;
  string::size_type Pos = 0;
  string FileName;
  string Extension;
  string PayloadHash;
  bool PublicationStatus = true;
  unsigned int Counter = 0;
  Tuple *PT = 0;
  vector<Tuple *> PubNotify;
  vector<Tuple *> SubNotify;

  PCore = (Core *)PB;

#ifdef DEBUG
  PB->S << endl << Offset << this->GetLegibleName () << endl;
#endif

  // ******************************************************
  // Looping over Path folder
  // ******************************************************

#ifdef DEBUG
  PB->S << Offset << "(Looping over the files on path = " << PB->GetPath ().c_str () << endl;
#endif

  dirp = opendir (PB->GetPath ().c_str ());

  if (dirp != NULL)
	{
	  std::vector<std::string> FileNamesInThePath;

	  FileNamesInThePath = read_directory (PB->GetPath ());

	  for (int y = 0; y < FileNamesInThePath.size (); y++)
		{
		  PublicationStatus = true;

		  FileName = FileNamesInThePath[y];

		  Pos = FileName.rfind ('.');

		  if (Pos != string::npos)
			{
			  Extension = FileName.substr (Pos + 1);
			}

#ifdef DEBUG
		  PB->S << Offset << "(Testing file number " << Jumping << ". Aware of " << PCore->Content.size ()
				<< " published photos)" << endl;
#endif
		  if (Extension != "" && Extension != "ini" && Extension != "txt" && Extension != "dat" && Extension != "sh" && Extension != "py" && Extension != "out")
			{
			  if (PCore->GetFileContentHash (FileName, PayloadHash) == OK)
				{
				  for (unsigned int i = 0; i < PCore->Content.size (); i++)
					{
					  if (PayloadHash == PCore->Content[i])
						{
						  PublicationStatus = false;

#ifdef DEBUG
						  PB->S << Offset << "(The content " << FileName << " was already published. Skipping)" << endl;
#endif

						  break;
						}
					}
				}
			  else
				{
				  PB->S << Offset << "(ERROR: Unable to calculate the payload hash)" << endl;
				}

			  if (PublicationStatus == true)
				{
#ifdef DEBUG
				  PB->S << Offset << "(Going to publish the content " << FileName
						<< ". Registering this publication on Content container)" << endl;
#endif
				  // TODO: FIXP/Update - Removed in September 2021
				 //PublishAPhotoToAllPeerApps03 (FileName, ScheduledMessages);

				  PCore->NoPP++;

				  PB->S << Offset << "(Publishing the content " << FileName << ". Publisher counter  " << PCore->NoPP << ". Running time is "<< setprecision(12) << GetTime() <<". )" << endl;

#ifdef DEBUG
				  PCore->Debug.OpenOutputFile ();

				  PCore->Debug << "Publishing the content " << FileName << ". Publisher counter " << PCore->NoPP << endl;

				  PCore->Debug.CloseFile ();
#endif

				  PCore->Content.push_back (PayloadHash);

#ifdef DEBUG
				  PB->S << Offset << "(The payload hash is = " << PayloadHash << ")" << endl;
#endif

				  // TODO: FIXP/Update - Added in September 2021.
				  for (unsigned int i = 0; i < PCore->PeerServerAppTuples.size (); i++)
					{
					  Tuple *ExtendedPeerAppTuple = new Tuple;

					  // Set this extended tuple to have a publication notification
					  ExtendedPeerAppTuple->Values.push_back ("pub");

					  // Copy the related Tuple pointer
					  PT = PCore->PeerServerAppTuples[i];

					  //PB->S << Offset << "(Repository " << i << ")" << endl;
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

				  // TODO - FIXP/Update - Added this equation to uniformly distributed publication in the interval
				  double Delta = double (Counter / PCore->ContentBurstSize) * double (PCore->DelayBeforeANewPhotoPublish);

				  //PB->S << Offset << "(Counter = " << Counter << ")" << endl;

				  //PB->S << Offset << "(Counter / PCore->ContentBurstSize) = " << double (Counter / PCore->ContentBurstSize) << ")" << endl;

				  //PB->S << Offset << "(Delay between publications (Delta) = " << setprecision(12) << Delta << ")" << endl;

				  //PB->S << Offset << "(Scheduling publication time is " << GetTime () + Delta << ")" << endl;

				  CreatePublishMessage (GetTime () + Delta, FileName, PubNotify, SubNotify);

				  for (unsigned int m = 0; m < PubNotify.size (); m++)
					{
					  Tuple *PET = PubNotify[m];

					  //PB->S << Offset << "(Deleting the extended tuple number " << m << ")" << endl;

					  delete PET;
					}

				  PubNotify.clear();
				  SubNotify.clear();

				  // TODO: FIXP/Update - Up to here

				  Counter++;

#ifdef DEBUG1
				  PB->S << Offset << "(Counter = " << Counter << ")" << endl;
				  PB->S << Offset << "(Messages in memory = " << PB->PP->GetNumberOfMessages () << ")" << endl;
				  PB->S << Offset << "(Messages window = " << Allowed << ")" << endl << endl;
#endif
				  if ((Counter == PCore->ContentBurstSize)
					  || (PB->PP->GetNumberOfMessages () >= (MAX_MESSAGES_IN_MEMORY - 200)))
					{
					  break;
					}
				}
			}
		  else
			{
#ifdef DEBUG
			  PB->S << Offset << "(Skipping the file = " << FileName << "." << Extension
					<< ". Unacceptable file extension.)" << endl;
#endif
			}
		}

	  PCore->GenerateRunX01 = false;

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
	  PB->S << Offset << "(ERROR: Unable to read the directory provided)" << endl;

	  perror ("opendir");
	}

  closedir (dirp);

  //PB->S << Offset <<  "(Finished)"<<endl;

  // ******************************************************
  // Schedule a message to -run --contentpublish again
  // ******************************************************

  // Setting up the process SCN as the space limiter
  Limiters.push_back (PB->PP->Intra_Process);

  // Setting up the CLI block SCN as the source SCN
  Sources.push_back (PB->GetSelfCertifyingName ());

  // Setting up the HT block SCN as the destination SCN
  Destinations.push_back (PB->GetSelfCertifyingName ());

  // Creating a new message
  PB->PP->NewMessage (GetTime () + PCore->DelayBeforeANewPhotoPublish, 1, false, RunPhotoPublish);

  // Creating the ng -cl -m command line
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, RunPhotoPublish, PCL);

  // Adding a ng -run --periodic command line
  RunPhotoPublish->NewCommandLine ("-run", "--contentpublish", "0.1", PCL);

  // Generate the SCN
  PB->GenerateSCNFromMessageBinaryPatterns (RunPhotoPublish, SCN);

  // Creating the ng -scn --s command line
  PMB->NewSCNCommandLine ("0.1", SCN, RunPhotoPublish, PCL);

  // ******************************************************
  // Finish
  // ******************************************************

  // Push the message to the GW input queue
  PCore->PGW->PushToInputQueue (RunPhotoPublish);

  if (ScheduledMessages.size () > 0)
	{
	  Message *Temp = ScheduledMessages.at (0);

	  Temp->MarkToDelete ();

	  // Make the scheduled messages vector empty
	  ScheduledMessages.clear ();
	}

#ifdef DEBUG
  PB->S << Offset << "(Done)" << endl << endl << endl;
#endif

  return Status;
}

// Publish a content to all the peer applications - notifies just one peer
int CoreRunContentPublish01::PublishAPhotoToAllPeerApps01 (string _FileName, vector<Message *> &ScheduledMessages)
{
  int Status = ERROR;
  Message *Run = 0;
  string Offset = "                    ";
  Core *PCore = 0;
  CommandLine *PCL = 0;

  PCore = (Core *)PB;

  if (ScheduledMessages.size () > 0)
	{
	  //PB->S << Offset <<  "(There is a scheduled message)" << endl;

	  Run = ScheduledMessages.at (0);

	  if (Run != 0)
		{
		  // Publish to all peer servers
		  for (unsigned int i = 0; i < PCore->PeerServerAppTuples.size (); i++)
			{
			  // Adding a ng -run --publish command line
			  Run->NewCommandLine ("-run", "--publish", "0.1", PCL);

			  PCL->NewArgument (3);

			  // Use the stored Subscription to fulfill the command line
			  PCL->SetArgumentElement (0, 0, "Repository");

			  PCL->SetArgumentElement (0, 1, PB->IntToString (i));

			  PCL->SetArgumentElement (0, 2, _FileName);

			  Status = OK;
			}

		  // Publish to all peer clients
		  for (unsigned int j = 0; j < PCore->PeerClientAppTuples.size (); j++)
			{
			  // Adding a ng -run --publish command line
			  Run->NewCommandLine ("-run", "--publish", "0.1", PCL);

			  PCL->NewArgument (3);

			  // Use the stored Subscription to fulfill the command line
			  PCL->SetArgumentElement (0, 0, "Source");

			  PCL->SetArgumentElement (0, 1, PB->IntToString (j));

			  PCL->SetArgumentElement (0, 2, _FileName);

			  Status = OK;
			}
		}
	}

  return Status;
}

// Publish a content to all the peer applications - notifies all peers
int CoreRunContentPublish01::PublishAPhotoToAllPeerApps02 (string _FileName, vector<Message *> &ScheduledMessages)
{
  int Status = ERROR;
  Message *Run = 0;
  string Offset = "                    ";
  CommandLine *PCL = 0;

  if (ScheduledMessages.size () > 0)
	{
	  //PB->S << Offset <<  "(There is a scheduled message)" << endl;

	  Run = ScheduledMessages.at (0);

	  if (Run != 0)
		{
		  // Adding a ng -run --publish command line
		  Run->NewCommandLine ("-run", "--publish", "0.2", PCL);

		  PCL->NewArgument (1);

		  PCL->SetArgumentElement (0, 0, _FileName);

		  Status = OK;
		}
	}

  return Status;
}

// Publish a content to all server peer applications - notifies all server peers
int CoreRunContentPublish01::PublishAPhotoToAllPeerApps03 (string _FileName, vector<Message *> &ScheduledMessages)
{
  int Status = ERROR;
  Message *Run = 0;
  string Offset = "                    ";
  CommandLine *PCL = 0;

  if (ScheduledMessages.size () > 0)
	{
	  //PB->S << Offset <<  "(There is a scheduled message)" << endl;

	  Run = ScheduledMessages.at (0);

	  if (Run != 0)
		{
		  // Adding a ng -run --publish command line
		  Run->NewCommandLine ("-run", "--publish", "0.3", PCL);

		  PCL->NewArgument (1);

		  PCL->SetArgumentElement (0, 0, _FileName);

		  Status = OK;
		}
	}

  return Status;
}

std::vector<std::string> CoreRunContentPublish01::read_directory (const string _Path)
{
  std::vector<std::string> result;
  dirent *de;
  DIR *dp;
  errno = 0;
  dp = opendir (_Path.empty () ? "." : _Path.c_str ());
  if (dp)
	{
	  while (true)
		{
		  errno = 0;
		  de = readdir (dp);
		  if (de == NULL) break;
		  result.push_back (std::string (de->d_name));
		}
	  closedir (dp);
	  std::sort (result.begin (), result.end ());
	}
  return result;
}

void CoreRunContentPublish01::CreatePublishMessage (double _Time, string _FileName, vector<Tuple *> &_PubNotify, vector<Tuple *> &_SubNotify)
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

	  // TODO - FIXP/Update - Use the uniformly distributed pub scheduling

	  // Creating a new message
	  PB->PP->NewMessage (_Time, 1, true, "Not_Necessary", _FileName, "Not_Necessary", PB->GetPath (), Publish);

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

		  // TODO - FIXP/Update - Define publishing time as the time the message will be run. Not now.
		  PP->Timestamp = _Time;

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

		  //PB->S << Offset <<"(The following message was published to the peer)"<< endl;

		  //PB->S << "(" << endl << *Publish << ")"<< endl;

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