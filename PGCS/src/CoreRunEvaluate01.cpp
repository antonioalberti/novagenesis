/*
	NovaGenesis

	Name:		CoreRunEvaluate01
	Object:		CoreRunEvaluate01
	File:		CoreRunEvaluate01.cpp
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

#ifndef _CORERUNEVALUATE01_H
#include "CoreRunEvaluate01.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

////#define DEBUG

CoreRunEvaluate01::CoreRunEvaluate01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
  TimeToExpose = 0;
}

CoreRunEvaluate01::~CoreRunEvaluate01 ()
{
}

// Run the actions behind a received command line
// ng -run --evaluate 0.1
int
CoreRunEvaluate01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;

  string Offset = "                    ";
  Core *PCore = 0;
  bool ClearScheduledMessage = true;
  double Time = GetTime ();

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  PB->S << endl << Offset << "(Time = " << Time << ")" << endl;

  PB->S << Offset << "(NextPeerEvaluationTime = " << PCore->NextPeerEvaluationTime << ")" << endl;

  CheckForPSAwareness (ScheduledMessages, ClearScheduledMessage);

  CheckForNewPeerApplication (ScheduledMessages, ClearScheduledMessage);

  ShowTheDiscoveredPeers ();

  CheckSubscriptions (ScheduledMessages, ClearScheduledMessage);

  if (ClearScheduledMessage == true)
	{
	  if (ScheduledMessages.size () > 0)
		{
		  Message *Temp = ScheduledMessages.at (0);

		  Temp->MarkToDelete ();

		  // Make the scheduled messages vector empty
		  ScheduledMessages.clear ();
		}
	}

  //PB->S << Offset <<  "(Deleting the previous marked messages)" << endl;

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

int CoreRunEvaluate01::CheckForPSAwareness (vector<Message *> &_ScheduledMessages, bool &_ClearScheduledMessage)
{
  int Status = OK;

  string Offset = "                    ";
  string Offset1 = "                              ";
  Core *PCore = 0;
  vector<string> *Values = new vector<string>;
  vector<Tuple *> PSSs;

  PCore = (Core *)PB;

  PB->S << Offset << "(1. Check for PSS/NRNCS awareness.)" << endl;

  // Run other procedures if it is aware of at least one NRNCS
  if (PCore->PSTuples.size () > 0)
	{
	  PB->S << Offset1 << "(The PGCS is already aware of this PSS/NRNCS)" << endl;

	  if (PCore->RunExpose == true && TimeToExpose < GetTime ())
		{
		  PB->S << Offset1 << "(Run expose until find the peer)" << endl;

		  PCore->Exposition (PB->PP->Intra_Domain, _ScheduledMessages);

		  _ClearScheduledMessage = false;

		  PB->State = "operational";

		  TimeToExpose = GetTime () + 10;
		}
	}

  return Status;
}

int CoreRunEvaluate01::CheckForNewPeerApplication (vector<Message *> &_ScheduledMessages, bool &_ClearScheduledMessage)
{
  int Status = ERROR;

  string Offset = "                    ";
  string Offset1 = "                              ";
  Core *PCore = 0;
  PGCS *PPGCS = 0;
  vector<string> *Candidates = new vector<string>;
  vector<string> *Subject = new vector<string>;
  Message *Run = 0;
  vector<Tuple *> Apps;
  CommandLine *PCL = 0;
  double Time = GetTime ();

  PCore = (Core *)PB;

  PPGCS = (PGCS *)PB->PP;

  PB->S << Offset << "(2. Check for new peer application tuples)" << endl;

  // *************************************************************
  // Check for new peer app tuples
  // *************************************************************
  if (PCore->PSTuples.size () > 0)
	{
	  if (PCore->NextPeerEvaluationTime < Time)
		{
		  // *******************
		  // Check for an IoT test application
		  // *******************
		  if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "IoT", Candidates, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Test", Subject, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Application", Subject, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("IoTTestApp", "Core", Apps, PB)
			  == OK)
			{
			  // Check for new servers
			  for (unsigned int i = 0; i < Candidates->size (); i++)
				{
				  PB->S << Offset1 << "(Testing the candidate = " << Candidates->at (i) << ")" << endl;

				  // Is the server different than this process
				  if (Candidates->at (i) != PB->PP->GetSelfCertifyingName ())
					{
					  for (unsigned int j = 0; j < Subject->size (); j++)
						{
						  // Has the server the same subject?
						  if (Candidates->at (i) == Subject->at (j))
							{
							  // Run over Apps
							  for (unsigned int k = 0; k < Apps.size (); k++)
								{
								  // Is the server an application with core block
								  if (Candidates->at (i) == Apps[k]->Values[2])
									{
									  // Auxiliary flag
									  bool StoreFlag = true;

									  // Loop over the already stored peer tuples
									  for (unsigned int h = 0; h < PCore->PeerAppTuples.size (); h++)
										{
										  // Is the server already stored?
										  if (Candidates->at (i) == PCore->PeerAppTuples[h]->Values[2])
											{
											  StoreFlag = false;
											}
										  else
											{
											  PB->S << Offset1 << "(The candidate peer is already stored)" << endl;
											}
										}

									  if (StoreFlag == true)
										{
										  // Store the learned tuple on the peer server app tuples
										  PCore->PeerAppTuples.push_back (Apps[k]);

										  unsigned int _I = PCore->PeerAppTuples.size () - 1;

										  PB->S << Offset1 << "(Discovered an IoT Test application)" << endl;

										  PCore->PeerAppTuples[_I]->LN = "IoTTestApp";
										  PCore->PeerAppTuples[_I]->ULN = PB->IntToString (_I);

										  PB->S << Offset1 << "(HID = " << PCore->PeerAppTuples[_I]->Values[0] << ")"
												<< endl;
										  PB->S << Offset1 << "(OSID = " << PCore->PeerAppTuples[_I]->Values[1] << ")"
												<< endl;
										  PB->S << Offset1 << "(PID = " << PCore->PeerAppTuples[_I]->Values[2] << ")"
												<< endl;
										  PB->S << Offset1 << "(BID = " << PCore->PeerAppTuples[_I]->Values[3] << ")"
												<< endl;

										  PCore->DelayBeforeRunPeriodic = 60;

										  PCore->RunExpose = false;
										}
									  else
										{
										  PB->S << Offset1 << "(The PGCS is already aware of this candidate)" << endl;
										}
									}
								}
							}
						}
					}
				  else
					{
					  PB->S << Offset1 << "(The candidate is the own process)" << endl;
					}
				}
			}

		  // Clear the vectors
		  Subject->clear ();
		  Apps.clear ();

		  // *******************
		  // Check for a RMS application
		  // *******************
		  if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "RMS", Candidates, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Core", Subject, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Controller", Subject, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("RMS", "Core", Apps, PB) == OK)
			{
			  // Check for new servers
			  for (unsigned int i = 0; i < Candidates->size (); i++)
				{
				  PB->S << Offset1 << "(Testing the candidate = " << Candidates->at (i) << ")" << endl;

				  // Is the server different than this process
				  if (Candidates->at (i) != PB->PP->GetSelfCertifyingName ())
					{
					  for (unsigned int j = 0; j < Subject->size (); j++)
						{
						  // Has the server the same subject?
						  if (Candidates->at (i) == Subject->at (j))
							{
							  // Run over Apps
							  for (unsigned int k = 0; k < Apps.size (); k++)
								{
								  // Is the server an application with core block
								  if (Candidates->at (i) == Apps[k]->Values[2])
									{
									  // Auxiliary flag
									  bool StoreFlag = true;

									  // Loop over the already stored peer tuples
									  for (unsigned int h = 0; h < PCore->PeerAppTuples.size (); h++)
										{
										  // Is the server already stored?
										  if (Candidates->at (i) == PCore->PeerAppTuples[h]->Values[2])
											{
											  StoreFlag = false;
											}
										  else
											{
											  PB->S << Offset1 << "(The candidate peer is already stored)" << endl;
											}
										}

									  if (StoreFlag == true)
										{
										  // Store the learned tuple on the peer server app tuples
										  PCore->PeerAppTuples.push_back (Apps[k]);

										  unsigned int _I = PCore->PeerAppTuples.size () - 1;

										  PB->S << Offset1 << "(Discovered a RMS application)" << endl;

										  PCore->PeerAppTuples[_I]->LN = "RMS";
										  PCore->PeerAppTuples[_I]->ULN = PB->IntToString (_I);

										  PB->S << Offset1 << "(HID = " << PCore->PeerAppTuples[_I]->Values[0] << ")"
												<< endl;
										  PB->S << Offset1 << "(OSID = " << PCore->PeerAppTuples[_I]->Values[1] << ")"
												<< endl;
										  PB->S << Offset1 << "(PID = " << PCore->PeerAppTuples[_I]->Values[2] << ")"
												<< endl;
										  PB->S << Offset1 << "(BID = " << PCore->PeerAppTuples[_I]->Values[3] << ")"
												<< endl;

										  PCore->DelayBeforeRunPeriodic = 60;

										  PCore->RunExpose = false;
										}
									  else
										{
										  PB->S << Offset1 << "(The PGCS is already aware of this candidate)" << endl;
										}
									}
								}
							}
						}
					}
				  else
					{
					  PB->S << Offset1 << "(The candidate is the own process)" << endl;
					}
				}
			}

		  // Clear the vectors
		  Subject->clear ();
		  Apps.clear ();

		  PCore->NextPeerEvaluationTime = Time + PCore->DelayBeforeANewPeerEvaluation;
		}
	  else
		{
		  PB->S << Offset1 << "(Too early for that. Wait next ng -run --evaluate)" << endl;
		}
	}
  else
	{
	  PB->S << Offset1 << "(Waiting for PSS/NRNCS discovery)" << endl;
	}

  delete Candidates;
  delete Subject;

  return Status;
}

int CoreRunEvaluate01::ShowTheDiscoveredPeers ()
{
  int Status = ERROR;

  string Offset = "                    ";
  string Offset1 = "                              ";
  Core *PCore = 0;
  PGCS *PPGCS = 0;

  PCore = (Core *)PB;

  PPGCS = (PGCS *)PB->PP;

  PB->S << Offset << "(3. Show the discovered peer App(s))" << endl;

  // *************************************************************
  // Show the discovered server App(s)
  // *************************************************************
  if (PCore->PeerAppTuples.size () > 0)
	{
	  for (unsigned int i = 0; i < PCore->PeerAppTuples.size (); i++)
		{
		  PB->S << Offset1 << "(Aware of the Application " << i << " )" << endl;

		  PB->S << Offset1 << "(LN = " << PCore->PeerAppTuples[i]->LN << ")" << endl;
		  PB->S << Offset1 << "(ULN = " << PCore->PeerAppTuples[i]->ULN << ")" << endl;
		  PB->S << Offset1 << "(HID = " << PCore->PeerAppTuples[i]->Values[0] << ")" << endl;
		  PB->S << Offset1 << "(OSID = " << PCore->PeerAppTuples[i]->Values[1] << ")" << endl;
		  PB->S << Offset1 << "(PID = " << PCore->PeerAppTuples[i]->Values[2] << ")" << endl;
		  PB->S << Offset1 << "(BID = " << PCore->PeerAppTuples[i]->Values[3] << ")" << endl;
		}
	}
  else
	{
	  PB->S << Offset1 << "(Not aware of any peer application)" << endl;
	}

  return Status;
}

int CoreRunEvaluate01::CheckSubscriptions (vector<Message *> &_ScheduledMessages, bool _ClearScheduledMessage)
{
  int Status = ERROR;

  string Offset = "                    ";
  string Offset1 = "                              ";
  Core *PCore = 0;
  PGCS *PPGCS = 0;
  unsigned int Index;
  Subscription *PS = 0;

  PCore = (Core *)PB;

  PPGCS = (PGCS *)PB->PP;

  PB->S << Offset << "(4. Check subscriptions)" << endl;

  // *************************************************************
  // Check subscriptions
  // *************************************************************
  if (PCore->PSTuples.size () > 0)
	{
	  // Looping over existent Subscriptions
	  for (unsigned int i = 0; i < PCore->Subscriptions.size (); i++)
		{
		  PS = PCore->Subscriptions[i];

		  //PB->S << Offset1 << "(Testing subscription " << i << ")" << endl;

		  //PB->S << Offset1 << "(Subscription status is " << PS->Status << ")" << endl;

		  if (PCore->GetPeerAppTupleIndex (PS->Publisher.Values[2], Index) != OK)
			{
			  PB->S << Offset1 << "(Warning: The publisher is unknown)" << endl;

			  // **********************************************************************************
			  //
			  // Check if this source was already discovered as a candidate peer
			  // (This code need to be improved to only accept peers that fit on keywords)
			  //
			  // **********************************************************************************

			  bool StoreFlag = true;

			  // Loop over the already stored peer tuples
			  for (unsigned int h = 0; h < PCore->PeerAppTuples.size (); h++)
				{
				  // Is the application already stored?
				  if (PS->Publisher.Values.at (2) == PCore->PeerAppTuples[h]->Values[2])
					{
					  StoreFlag = false;
					}
				  else
					{
					  PB->S << Offset << "(The candidate peer is already stored)" << endl;
					}
				}

			  if (StoreFlag == true && PS->Publisher.Values.at (3) == "NULL")
				{
				  Tuple *PPeer = new Tuple;

				  PPeer->Values.push_back (PS->Publisher.Values.at (0));
				  PPeer->Values.push_back (PS->Publisher.Values.at (1));
				  PPeer->Values.push_back (PS->Publisher.Values.at (2));
				  PPeer->Values.push_back (PS->Publisher.Values.at (3));

				  // Store the learned tuple on the peer app tuples
				  PCore->PeerAppTuples.push_back (PPeer);

				  int _I = PCore->PeerAppTuples.size () - 1;

				  PB->S << Offset << "(Discovered a candidate peer application)" << endl;

				  PCore->PeerAppTuples[_I]->LN = "EPGS";

				  PCore->PeerAppTuples[_I]->ULN = PB->IntToString (_I);

				  PCore->DelayBeforeRunPeriodic = 60;

				  PCore->RunExpose = false;
				}
			  else
				{
				  PB->S << Offset << "(The PGCS is already aware of this candidate)" << endl;
				}
			}

		  PS->LN = PCore->PeerAppTuples[Index]->LN;
		  PS->ULN = PCore->PeerAppTuples[Index]->ULN;

		  //PB->S << Offset1 << "(The publisher is a " << PS->LN << " and have the index " << Index << ")" << endl;
		  //PB->S << Offset1 << "(HID = " << PS->Publisher.Values[0] << ")" << endl;
		  //PB->S << Offset1 << "(OSID = " << PS->Publisher.Values[1] << ")" << endl;
		  //PB->S << Offset1 << "(PID = " << PS->Publisher.Values[2] << ")" << endl;
		  //PB->S << Offset1 << "(BID = " << PS->Publisher.Values[3] << ")" << endl;

		  // Subscription requiring a scheduling
		  if (PS->Status == "Scheduling required")
			{
			  ScheduleASubscripion (PS, _ScheduledMessages, _ClearScheduledMessage);
			}

		  // Subscription requiring processing of a delivered content
		  if (PS->Status == "Processing required")
			{
			  ProcessASubscribedPayload (PS->LN, Index, PS, _ScheduledMessages, _ClearScheduledMessage);
			}

		  // Deleting finished subscriptions
		  if (PS->Status == "Delete")
			{
			  PB->S << Offset1 << "(Deleting the subscription with index = " << i << ")" << endl;

			  PCore->DeleteSubscription (PS);
			}
		}
	}
  else
	{
	  PB->S << Offset1 << "(Waiting for NRNCS discovery)" << endl;
	}

  return Status;
}

int
CoreRunEvaluate01::ScheduleASubscripion (Subscription *_PS, vector<Message *> &_ScheduledMessages, bool _ClearScheduledMessage)
{
  Message *Run = 0;
  CommandLine *PCL = 0;
  Core *PCore = 0;
  string Offset1 = "                              ";
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string Offset = " ";
  Message *SubData = 0;

  PCore = (Core *)PB;

  //Set limiter, sources and destinations to it self
  Limiters.push_back (PB->PP->Intra_Process);
  Sources.push_back (PB->GetSelfCertifyingName ());
  Destinations.push_back (PB->GetSelfCertifyingName ());

  //Schedule and create header
  PB->PP->NewMessage (GetTime (), 1, true, SubData);
  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, SubData, PCL);
  SubData->NewCommandLine ("-run", "--subscribe", "0.1", PCL);

  //Put arguments into message
  PCL->NewArgument (1);
  PCL->SetArgumentElement (0, 0, PB->IntToString (_PS->Category));
  PCL->NewArgument (1);
  PCL->SetArgumentElement (1, 0, _PS->Key);

  //Create SCN
  PB->GenerateSCNFromMessageBinaryPatterns (SubData, SCN);
  PMB->NewSCNCommandLine ("0.1", SCN, SubData, PCL);

  //Push to input queue
  PCore->PGW->PushToInputQueue (SubData);

  PB->S << Offset1 << "(Going to subscribe the key = " << _PS->Key << ")" << endl;

  _PS->Status = "Waiting delivery";

  PB->S << Offset1 << "(Changing subscription status from \"Scheduling required\" to \"Waiting delivery\")" << endl;

  // Storing a timestamp for performance evaluation
  _PS->Timestamp = GetTime ();

  _ClearScheduledMessage = false;

  PCore->Debug.OpenOutputFile ();

  PCore->Debug << "Evaluating a scheduling required with key " << _PS->Key
			   << ". Creating a ng -run --subscribe message. Status of the subscription is " << _PS->Status << endl;

  PCore->Debug.CloseFile ();

  return OK;
}

int CoreRunEvaluate01::ProcessASubscribedPayload (string _Publisher_LN, unsigned int _Index, Subscription *_PS, vector<
	Message *> &_ScheduledMessages, bool _ClearScheduledMessage)
{
  Core *PCore = 0;
  string Offset1 = "                              ";
  string::size_type Pos = 0;
  string Extension;

  PCore = (Core *)PB;

  PB->S << Offset1 << "(Checking the file received from the peer with name " << _PS->FileName << ")" << endl;

  Pos = _PS->FileName.rfind ('.');

  if (Pos != string::npos)
	{
	  Extension = _PS->FileName.substr (Pos + 1);
	}

  PB->S << Offset1 << "(File extension = " << Extension << ")" << endl;

  if (Extension == "txt")
	{
	  ProcessTXTFile (_Publisher_LN, _Index, _PS, _PS->FileName);
	}

  _PS->Status = "Delete";

  return OK;
}

int CoreRunEvaluate01::ProcessTXTFile (string _Publisher_LN, unsigned int _Index, Subscription *_PS, string _FileName)
{
  File F1;
  Core *PCore = NULL;
  string Offset1 = "                              ";
  Block *PHTB = NULL;
  Message *StoringLearnedBinds = NULL;
  Message *PublishAcceptance = NULL;
  Message *PublishAcceptanceCopy1 = NULL;
  CommandLine *PCL = NULL;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string AcceptedHash;
  string AcceptedFileName;
  string AcceptedPayload;
  File F2;
  string Type;
  string SCN1;
  Message *Revoke = 0;
  CommandLine *PCL2 = 0;
  CommandLine *PRVKCL = 0;

  // Cast to PG pointer
  PG *PPG = (PG *)PB;

  PCore = (Core *)PB;

  PHTB = (Block *)PCore->PHT;

  F1.OpenInputFile (_FileName, PB->GetPath (), "DEFAULT");

  F1.seekg (0);

  // ------------------------------------------------------------------------------------------------------------------------------
  // Functions to customize the application. Put here the code to treat your TXT file as you wish
  // ------------------------------------------------------------------------------------------------------------------------------
  PB->S << Offset1 << "(Going to parse the file " << _FileName << " from " << _Publisher_LN << ")" << endl;

  string Temp = _FileName.substr (0, 13);

  if (Temp == "Service_Offer")
	{
	  // ******************************************************
	  // Schedule a message accepting the proposed contract
	  // ******************************************************

	  // Clear the vectors
	  Limiters.clear ();
	  Sources.clear ();
	  Destinations.clear ();

	  // Setting up the process SCN as the space limiter
	  Limiters.push_back (PB->PP->Intra_Process);

	  // Setting up the Core block SCN as the source SCN
	  Sources.push_back (PB->GetSelfCertifyingName ());

	  // Setting up the HT block SCN as the destination SCN
	  Destinations.push_back (PB->GetSelfCertifyingName ());

	  // Creating a new message
	  PB->PP->NewMessage (GetTime (), 1, false, PublishAcceptance);

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, PublishAcceptance, PCL);

	  // Adding a ng -run --publish command line
	  PublishAcceptance->NewCommandLine ("-run", "--publish", "0.1", PCL);

	  PCL->NewArgument (3);

	  // Use the stored Subscription to fulfill the command line
	  PCL->SetArgumentElement (0, 0, _Publisher_LN);

	  PCL->SetArgumentElement (0, 1, PB->IntToString (_Index));

	  if (_Publisher_LN == "EPGS")
		{
		  // Loop over the peer stored tuples
		  for (unsigned int h = 0; h < PCore->PeerAppTuples.size (); h++)
			{
			  // Is the server already stored?
			  if (PCore->PeerAppTuples[h]->LN == "IoTTestApp")
				{
				  // Set the service offer payload
				  AcceptedPayload = PCore->PeerAppTuples[h]->Values.at (0) + " " +
									PCore->PeerAppTuples[h]->Values.at (1) + " " +
									PCore->PeerAppTuples[h]->Values.at (2) + " " +
									PCore->PeerAppTuples[h]->Values.at (3);
				}
			}

		  PPG->DelayBetweenMessageEmissions = 10;
		}

	  if (_Publisher_LN == "IoTTestApp")
		{
		  // Set the service offer payload
		  AcceptedPayload = "The offer send by the IoTTestApp was accepted";
		}

	  unsigned int Complement = rand () % 10000000000000;

	  // Set the file name
	  AcceptedFileName = "Service_Accepted_" + PB->UnsignedIntToString (Complement) + ".txt";

	  // Open the file to write
	  F2.OpenOutputFile (AcceptedFileName, PB->GetPath (), "DEFAULT");

	  F2 << AcceptedPayload;

	  F2.CloseFile ();

	  PCL->SetArgumentElement (0, 2, AcceptedFileName);

	  // Generate the SCN
	  PB->GenerateSCNFromMessageBinaryPatterns (PublishAcceptance, SCN1);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN1, PublishAcceptance, PCL);

	  // ******************************************************
	  // Finish
	  // ******************************************************

	  // Push the message to the GW input queue
	  PCore->PGW->PushToInputQueue (PublishAcceptance);

	  PB->S << Offset1 << "The scheduled message was:" << endl;

	  PB->S << *PublishAcceptance << endl;

	  PCore->Debug.OpenOutputFile ();

	  PCore->Debug << "Accepted a contract from peer " << _Publisher_LN << ". Creating the " << AcceptedFileName << ")"
				   << endl;

	  PCore->Debug.CloseFile ();

	}

  string Temp1 = _FileName.substr (0, 8);

  if (Temp1 == "PGCSFile")
	{
	  string Command;
	  string Value;
	  string Counter;

	  F1 >> Command;
	  F1 >> Value;
	  F1 >> Counter;

	  std::remove ((PB->GetPath () + _FileName).c_str ());

#ifdef DEBUG
	  PB->S << Offset1 << "(There is a received file with command from RMS: " << Command << " " << Value << " "
			<< Counter << ")" << endl;
#endif

	  // Perform the required control in the hardware APS is representing

	  FILE *f1; // Result from first command

	  string Command1 =
		  "sudo /usr/bin/python coapclient.py -o POST -p coap://[aaaa::212:4b00:7ba:7607]:5683/dev/chan -P \"ch="
		  + Value
		  + "\" \& sudo /usr/bin/python coapclient.py -o POST -p coap://[aaaa::212:4b00:6e3:660c]:5683/com/chan -P \"ch="
		  + Value + "\"";

	  //string Command1="sudo /usr/bin/python coapclient.py -o POST -p coap://[aaaa::212:4b00:7ba:7607]:5683/dev/chan -P \"ch="+Value+"\"";

	  //string Command1="sudo /usr/bin/python coapclient.py -o POST -p coap://[aaaa::212:4b00:6e3:660c]:5683/com/chan -P \"ch="+Value+"\"";

	  // Colocar um print do comando.

	  Run (Command1, f1);

	  // Close the files
	  pclose (f1);


	  // ********************************************************************************
	  //
	  // Added in July 2018 by Alberti. A revoke on the received file.
	  //
	  // ********************************************************************************

	  PB->S << Offset1 << "(Generating a message to revoke the sample file key at domain level)" << endl;

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

	  // Setting up the destination
	  Destinations.push_back (PCore->PSTuples[0]->Values[0]);

	  // Setting up the destination 2nd source
	  Destinations.push_back (PCore->PSTuples[0]->Values[1]);

	  // Setting up the destination 3rd source
	  Destinations.push_back (PCore->PSTuples[0]->Values[2]);

	  // Setting up the destination 4th source
	  Destinations.push_back (PCore->PSTuples[0]->Values[3]);

	  // Creating a new message
	  PB->PP->NewMessage (GetTime (), 1, false, Revoke);

	  // Creating the ng -cl -m command line
	  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Revoke, PCL2);

	  // ***************************************************
	  // Generate the ng -rvk --binding
	  // ***************************************************

	  Revoke->NewCommandLine ("-rvk", "--b", "0.1", PRVKCL);

	  // First argument
	  PRVKCL->NewArgument (1);

	  PRVKCL->SetArgumentElement (0, 0, "18");

	  // Second argument
	  PRVKCL->NewArgument (1);

	  PRVKCL->SetArgumentElement (1, 0, _PS->Key);

	  // ******************************************************
	  // Generate the SCN
	  // ******************************************************

	  PB->GenerateSCNFromMessageBinaryPatterns (Revoke, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, Revoke, PCL2);

	  PB->S << Offset1 << "(The following message contains a revoke binding message to the NRNCS/GIRS/HTS)" << endl;

	  PB->S << "(" << endl << *Revoke << ")" << endl;

	  // ******************************************************
	  // Finish
	  // ******************************************************

	  // Push the message to the GW input queue
	  PCore->PGW->PushToInputQueue (Revoke);
	}

  F1.CloseFile ();

  return OK;
}

bool CoreRunEvaluate01::Run (string _Command, FILE *&_f)
{
  bool Status = OK;

  _f = popen (_Command.c_str (), "r");

  if (_f == NULL)
	{
	  printf ("Failed to run command\n");

	  Status = ERROR;
	}

  return Status;
}
