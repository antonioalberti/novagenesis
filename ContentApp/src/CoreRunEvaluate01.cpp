/*
	NovaGenesis

	Name:		CoreRunEvaluate01
	Object:		CoreRunEvaluate01
	File:		CoreRunEvaluate01.cpp
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

#ifndef _CORERUNEVALUATE01_H
#include "CoreRunEvaluate01.h"
#endif

#ifndef _CONTENTAPP_H
#include "ContentApp.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

//#define DEBUG // To follow message processing

CoreRunEvaluate01::CoreRunEvaluate01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
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
  string Offset1 = "                              ";
  Core *PCore = 0;
  ContentApp *PApp = 0;
  vector<string> *Servers = new vector<string>;
  vector<string> *Clients = new vector<string>;
  vector<string> *Subject = new vector<string>;
  string Goto;
  string LN1;
  string LN2;
  string LN3;
  string LN4;
  Message *Run = 0;
  vector<Tuple *> PSs;
  vector<Tuple *> Apps;
  CommandLine *PCL = 0;
  bool ClearScheduledMessage = true;
  double Time = GetTime ();
  string::size_type Pos = 0;
  string Extension;
  File F1;
  stringstream ss;
  string HUID;
  string Method;
  string SourcePID;
  string DestinationPID;
  string PayloadString;
  string PayloadHash;
  unsigned int Index;
  string Type;
  Subscription *PS = 0;
  string AcceptedHash;
  string AcceptedFileName;
  string AcceptedPayload;
  File F2;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  string PGCSPID;
  vector<string> *PGCSBIDs = new vector<string>;
  Message *IPCUpdate = NULL;

  PCore = (Core *)PB;

  PApp = (ContentApp *)PB->PP;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

  PB->S << endl << endl << Offset << "(Time = " << Time << ")" << endl;
  PB->S << Offset <<  "(NextPeerEvaluationTime = " << PCore->NextPeerEvaluationTime << ")"<<endl;

  PB->S << Offset << "(1. Check for new peer application tuples)" << endl;

#endif

  // *************************************************************
  // Check for new peer server app tuples
  // *************************************************************
  if (PCore->PSTuples.size () > 0)
	{
	  if (PCore->NextPeerEvaluationTime < Time)
		{

#ifdef DEBUG
		  PB->S << endl << Offset1 << "(Check for new peer server application tuples)" << endl;
#endif

		  if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Repository", Servers, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Content", Subject, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("ContentApp", "Core", Apps, PB)
			  == OK)
			{
#ifdef DEBUG
			  PB->S << endl << Offset1 << "(Servers vector size = " << Servers->size () << ")" << endl;

			  for (unsigned int u = 0; u < Servers->size (); u++)
				{
				  PB->S << Offset1 << "(Repository [" << u << "] = " << Servers->at (u) << ")" << endl;
				}

			  PB->S << Offset1 << "(Subject vector size = " << Subject->size () << ")" << endl;

			  for (unsigned int v = 0; v < Subject->size (); v++)
				{
				  PB->S << Offset1 << "(Repository [" << v << "] = " << Subject->at (v) << ")" << endl;
				}

			  PB->S << Offset1 << "(App tuples vector size = " << Apps.size () << ")" << endl;

			  for (unsigned int w = 0; w < Apps.size (); w++)
				{
				  PB->S << Offset1 << "(Repository [" << w << "] = " << Apps.at (w)->Values.at (2) << ")" << endl;
				}
#endif

			  // Check for new servers
			  for (unsigned int i = 0; i < Servers->size (); i++)
				{
				  //PB->S << Offset1 << "(Testing the Repository = " << Servers->at(i) << ")" << endl;

				  // Is the server different than this process
				  if (Servers->at (i) != PB->PP->GetSelfCertifyingName ())
					{
					  for (unsigned int j = 0; j < Subject->size (); j++)
						{
						  // Has the server the same subject?
						  if (Servers->at (i) == Subject->at (j))
							{
							  // Run over Apps
							  for (unsigned int k = 0; k < Apps.size (); k++)
								{
								  // Is the server an application with core block
								  if (Servers->at (i) == Apps[k]->Values[2])
									{
									  // Auxiliary flag
									  bool StoreFlag = true;

									  // Loop over the already stored server tuples
									  for (unsigned int h = 0; h < PCore->PeerServerAppTuples.size (); h++)
										{
										  // Is the server already stored?
										  if (Servers->at (i) == PCore->PeerServerAppTuples[h]->Values[2])
											{
											  StoreFlag = false;
											}
										  else
											{
#ifdef DEBUG
											  PB->S << Offset1 << "(The candidate is already stored)" << endl;
#endif
											}
										}

									  if (StoreFlag == true)
										{
										  // Store the learned tuple on the peer server app tuples
										  PCore->PeerServerAppTuples.push_back (Apps[k]);

										  unsigned int _I = PCore->PeerServerAppTuples.size () - 1;

#ifdef DEBUG

										  PB->S << Offset1 << "(Discovered a candidate for a Repository application)"
												<< endl;
										  PB->S << Offset1 << "(HID = " << PCore->PeerServerAppTuples[_I]->Values[0]
												<< ")" << endl;
										  PB->S << Offset1 << "(OSID = " << PCore->PeerServerAppTuples[_I]->Values[1]
												<< ")" << endl;
										  PB->S << Offset1 << "(PID = " << PCore->PeerServerAppTuples[_I]->Values[2]
												<< ")" << endl;
										  PB->S << Offset1 << "(BID = " << PCore->PeerServerAppTuples[_I]->Values[3]
												<< ")" << endl;
#endif

										  // Only clients invite servers
										  if (PApp->Role == "Source")
											{
											  if (ScheduledMessages.size () > 0)
												{

#ifdef DEBUG
												  PB->S << Offset1
														<< "(There is a scheduled message with a ng -invite command line)"
														<< endl;
#endif

												  Run = ScheduledMessages.at (0);

												  if (Run != 0)
													{
													  // Adding a ng -run --invitation command line
													  Run->NewCommandLine ("-run", "--invite", "0.1", PCL);

													  PCL->NewArgument (2);

													  PCL->SetArgumentElement (0, 0, "Repository");

													  PCL->SetArgumentElement (0, 1, PB->IntToString (_I));

													  ClearScheduledMessage = false;

													  PCore->DelayBeforeRunPeriodic = 30;

													  PB->S << Offset1
															<< "(Discovered a Repository--------------------------------------------------------------------------------------------------)"
															<< endl;
													}
												}
											}
										}
									  else
										{
#ifdef DEBUG
										  PB->S << Offset1 << "(The app is already aware of this Repository)" << endl;
#endif
										}
									}
								}
							}
						}
					}
				  else
					{
#ifdef DEBUG
					  PB->S << Offset1 << "(The candidate is the own process)" << endl;
#endif
					}
				}
			}

		  // Clear the vectors
		  Subject->clear ();
		  Apps.clear ();

#ifdef DEBUG

		  PB->S << endl << Offset1 << "(Check for new peer client application tuples)" << endl;

#endif

		  // *************************************************************
		  // Check for new peer client app tuples
		  // *************************************************************


		  if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Source", Clients, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesIDsFromLN (2, "Content", Subject, PB) == OK &&
			  PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames ("ContentApp", "Core", Apps, PB)
			  == OK)
			{

#ifdef DEBUG
			  PB->S << endl << Offset1 << "(Clients vector size = " << Clients->size () << ")" << endl;
			  PB->S << Offset1 << "(Subject vector size = " << Subject->size () << ")" << endl;
			  PB->S << Offset1 << "(App tuples vector size = " << Apps.size () << ")" << endl;

#endif

			  // Check for new clients
			  for (unsigned int i = 0; i < Clients->size (); i++)
				{
				  //PB->S << Offset1 << "(Testing the Source = " << Clients->at(i) << ")" << endl;

				  // Is the client different than this process
				  if (Clients->at (i) != PB->PP->GetSelfCertifyingName ())
					{
					  for (unsigned int j = 0; j < Subject->size (); j++)
						{
						  // Has the client the same subject?
						  if (Clients->at (i) == Subject->at (j))
							{
							  // Run over Apps
							  for (unsigned int k = 0; k < Apps.size (); k++)
								{
								  // Is the client an application with core block
								  if (Clients->at (i) == Apps[k]->Values[2])
									{
									  // Auxiliary flag
									  bool StoreFlag = true;

									  // Loop over the already stored client tuples
									  for (unsigned int h = 0; h < PCore->PeerClientAppTuples.size (); h++)
										{
										  // Is the client already stored?
										  if (Clients->at (i) == PCore->PeerClientAppTuples[h]->Values[2])
											{
											  StoreFlag = false;
											}
										  else
											{

#ifdef DEBUG
											  PB->S << Offset1 << "(The candidate is already stored)" << endl;
#endif
											}
										}

									  if (StoreFlag == true)
										{
										  if (PApp->Role == "Repository")
											{
											  // Store the learned tuple on the peer client app tuples
											  PCore->PeerClientAppTuples.push_back (Apps[k]);

											  unsigned int _I = PCore->PeerClientAppTuples.size () - 1;

#ifdef DEBUG
											  PB->S << Offset1 << "(Discovered a candidate for a Source application)"
													<< endl;
											  PB->S << Offset1 << "(HID = " << PCore->PeerClientAppTuples[_I]->Values[0]
													<< ")" << endl;
											  PB->S << Offset1 << "(OSID = "
													<< PCore->PeerClientAppTuples[_I]->Values[1] << ")" << endl;
											  PB->S << Offset1 << "(PID = " << PCore->PeerClientAppTuples[_I]->Values[2]
													<< ")" << endl;
											  PB->S << Offset1 << "(BID = " << PCore->PeerClientAppTuples[_I]->Values[3]
													<< ")" << endl;
#endif

											  PCore->DelayBeforeRunPeriodic = 60;

											  PB->S << Offset1
													<< "(Discovered a Source --------------------------------------------------------------------------------------------------)"
													<< endl;
											}
										}
									  else
										{
#ifdef DEBUG
										  PB->S << Offset1 << "(The app is already aware of this Source)" << endl;
#endif
										}
									}
								}
							}
						  else
							{

#ifdef DEBUG
							  PB->S << Offset1 << "(The candidate is not related to \"Content\" subject)" << endl;
#endif
							}
						}
					}
				  else
					{

#ifdef DEBUG
					  PB->S << Offset1 << "(The candidate is the own process)" << endl;
#endif
					}
				}
			}

		  PCore->NextPeerEvaluationTime = Time + PCore->DelayBeforeANewPeerEvaluation;
		}
	  else
		{
		  //PB->S << Offset1 << "(Too early for that. Wait next ng -run --evaluate)"<<endl;
		}
	}
  else
	{

#ifdef DEBUG
	  PB->S << Offset1 << "(Waiting for PSS/NRNCS discovery)" << endl;
#endif
	}

#ifdef DEBUG

  PB->S << Offset << "(2. Show the discovered server App(s))" << endl;


  // *************************************************************
  // Show the discovered server App(s)
  // *************************************************************
  if (PCore->PeerServerAppTuples.size () > 0)
	{
	  for (unsigned int i = 0; i < PCore->PeerServerAppTuples.size (); i++)
		{
		  PB->S << Offset1 << "(Aware of the Peer App Repository " << i << " )" << endl;
		}
	}
  else
	{
	  PB->S << Offset1 << "(Not aware of any Repository)" << endl;
	}

  PB->S << Offset << "(3. Show the discovered client App(s))" << endl;

  // *************************************************************
  // Show the discovered client App(s)
  // *************************************************************
  if (PCore->PeerClientAppTuples.size () > 0)
	{
	  for (unsigned int i = 0; i < PCore->PeerClientAppTuples.size (); i++)
		{
		  PB->S << Offset1 << "(Aware of the Peer App Source " << i << " )" << endl;
		}
	}
  else
	{
	  PB->S << Offset1 << "(Not aware of any Source)" << endl;
	}

  PB->S << Offset << "(4. Check subscriptions)" << endl;

#endif

  // *************************************************************
  // Check subscriptions
  // *************************************************************
  if (PCore->PSTuples.size () > 0)
	{
	  // Looping over existent Subscriptions
	  for (unsigned int i = 0; i < PCore->Subscriptions.size (); i++)
		{
		  PS = PCore->Subscriptions[i];

#ifdef DEBUG
		  PB->S << Offset1 << "(Testing subscription " << i << ")" << endl;

		  PB->S << Offset1 << "(Subscription status is " << PS->Status << ")" << endl;
#endif

		  PCore->GetPeerAppTupleType (PS->Publisher.Values[2], Index, Type);

		  if (Type == "Repository" || Type == "Source")
			{
#ifdef DEBUG
			  PB->S << Offset1 << "(Publisher = " << Type << " " << Index << ")" << endl;
#endif
			}
		  else
			{
			  PB->S << Offset1 << "(Warning: This application is still unknown)" << endl;
			}

#ifdef DEBUG
		  PB->S << Offset1 << "(Key = " << PS->Key << ")" << endl;
		  PB->S << Offset1 << "(HID = " << PS->Publisher.Values[0] << ")" << endl;
		  PB->S << Offset1 << "(OSID = " << PS->Publisher.Values[1] << ")" << endl;
		  PB->S << Offset1 << "(PID = " << PS->Publisher.Values[2] << ")" << endl;
		  PB->S << Offset1 << "(BID = " << PS->Publisher.Values[3] << ")" << endl;
#endif

		  // ****************************************************************************

		  // Subscription requiring processing of a delivered content
		  if (PS->Status == "Processing required")
			{
			  if (PCore->GetPeerAppTupleType (PS->Publisher.Values[2], Index, Type) == OK)
				{

#ifdef DEBUG
				  PB->S << Offset1 << "(Checking the file received from the peer with name " << PS->FileName << ")"
						<< endl;
#endif

				  Pos = PS->FileName.rfind ('.');

				  if (Pos != string::npos)
					{
					  Extension = PS->FileName.substr (Pos + 1);
					}

#ifdef DEBUG
				  PB->S << Offset1 << "(File extension = " << Extension << ")" << endl;
#endif

				  if (Extension == "txt")
					{
					  F1.OpenInputFile (PS->FileName, PB->GetPath (), "DEFAULT");

					  F1.seekg (0);

					  F1 >> HUID;
					  F1 >> Method;
					  F1 >> SourcePID;
					  F1 >> DestinationPID;

#ifdef DEBUG
					  PB->S << Offset1 << "(HUID = " << HUID << ")" << endl;
					  PB->S << Offset1 << "(Method = " << Method << ")" << endl;
					  PB->S << Offset1 << "(SourcePID = " << SourcePID << ")" << endl;
					  PB->S << Offset1 << "(DestinationPID = " << DestinationPID << ")" << endl;
#endif

					  if (HUID == "Alberti_HUID" && Method == "Offer")
						{
						  Run = ScheduledMessages.at (0);

						  if (Run != 0)
							{
							  if (PCore->GetFileContentHash (PS->FileName, PayloadHash) == OK)
								{

#ifdef DEBUG
								  PB->S << Offset1 << "(The payload hash calculated now is              " << PayloadHash
										<< ")" << endl;
								  PB->S << Offset1 << "(The payload hash calculated at the publisher is " << PS->Key
										<< ")" << endl;
#endif

								  // Integrity check
								  if (PS->Key == PayloadHash)
									{

#ifdef DEBUG
									  PB->S << Offset1
											<< "(The payload hash is the same than the one generated on the publisher)"
											<< endl;

									  PB->S << Offset1 << "(There is a scheduled message)" << endl;
#endif

									  // Adding a ng -run --publish command line
									  Run->NewCommandLine ("-run", "--publish", "0.1", PCL);

									  PCL->NewArgument (3);

									  // Use the stored Subscription to fulfill the command line
									  PCL->SetArgumentElement (0, 0, Type);

									  PCL->SetArgumentElement (0, 1, PB->IntToString (Index));

									  // Set the service offer payload
									  AcceptedPayload =
										  "Alberti_HUID Accepted " + PB->PP->GetSelfCertifyingName () + " "
										  + PS->Publisher.Values[2];

									  unsigned int Complement = rand () % 10000000000000;

									  // Set the file name
									  AcceptedFileName =
										  "Service_Accepted_" + PB->UnsignedIntToString (Complement) + ".txt";

									  // Open the file to write
									  F2.OpenOutputFile (AcceptedFileName, PB->GetPath (), "DEFAULT");

									  F2 << AcceptedPayload;

									  F2.CloseFile ();

									  PCL->SetArgumentElement (0, 2, AcceptedFileName);

									  ClearScheduledMessage = false;

									  PS->Status = "Delete";

#ifdef DEBUG
									  PCore->Debug.OpenOutputFile ();

									  PCore->Debug << "Evaluating a processing required with key " << PS->Key
												   << ". Creating the " << AcceptedFileName
												   << ". Status of the subscription is " << PS->Status << endl;

									  PCore->Debug.CloseFile ();
#endif
									}
								  else
									{
									  PB->S << Offset1
											<< "(ERROR: The received file hash differs from the publihed one)" << endl;
									}
								}
							  else
								{
								  PB->S << Offset1 << "(ERROR: Unable to calculate the payload hash)" << endl;
								}
							}
						  else
							{
							  PB->S << Offset1 << "(ERROR: There is no scheduled message)" << endl;
							}
						}

					  if (HUID == "Alberti_HUID" && Method == "Accepted")
						{
						  Run = ScheduledMessages.at (0);

						  if (Run != 0)
							{
							  if (PCore->GetFileContentHash (PS->FileName, PayloadHash) == OK)
								{
#ifdef DEBUG
								  PB->S << Offset1 << "(The payload hash calculated now is              " << PayloadHash
										<< ")" << endl;
								  PB->S << Offset1 << "(The payload hash calculated at the publisher is " << PS->Key
										<< ")" << endl;
#endif

								  // Integrity check
								  if (PS->Key == PayloadHash)
									{
#ifdef DEBUG

									  PB->S << Offset1
											<< "(The payload hash is the same than the one generated on the publisher)"
											<< endl;

									  PB->S << Offset1 << "(The published Service Offer was accepted by the peer!)"
											<< endl;

									  PB->S << Offset1 << "(There is a scheduled message)" << endl;
#endif

									  // Adding a ng -run --publish command line
									  Run->NewCommandLine ("-run", "--contentpublish", "0.1", PCL);

									  Run->SetTime (GetTime () + PCore->DelayBeforeANewPhotoPublish);

									  ClearScheduledMessage = false;

									  PS->Status = "Delete";

#ifdef DEBUG

									  PCore->Debug.OpenOutputFile ();

									  PCore->Debug << "Evaluating a processing required with key " << PS->Key
												   << ". Going to publish the photos. Status of the subscription is "
												   << PS->Status << endl;

									  PCore->Debug.CloseFile ();
#endif
									}
								  else
									{
									  PB->S << Offset1
											<< "(ERROR: The received file hash differs form the published one)" << endl;
									}
								}
							  else
								{
								  PB->S << Offset1 << "(ERROR: Unable to calculate the payload hash)" << endl;
								}
							}
						  else
							{
							  PB->S << Offset1 << "(ERROR: There is no scheduled message)" << endl;
							}
						}
					}

					// TODO: FIXP/Update - Added .py as a file type to do not transfer
				  if (Extension != "" && Extension != "ini" && Extension != "txt" && Extension != "dat" && Extension != "py")
					{
					  if (PCore->GetFileContentHash (PS->FileName, PayloadHash) == OK)
						{

#ifdef DEBUG
						  PB->S << Offset1 << "(The payload hash calculated now is              " << PayloadHash << ")"
								<< endl;
						  PB->S << Offset1 << "(The payload hash calculated at the publisher is " << PS->Key << ")"
								<< endl;
#endif

						  // Integrity check
						  if (PS->Key == PayloadHash)
							{
							  PCore->NoSP++;

#ifdef DEBUG

							  PB->S << Offset << "(Receiver counter " << PCore->NoSP << ")" << endl;

							  PB->S << Offset1
									<< "(The payload hash is the same than the one generated on the publisher)" << endl;

							  PCore->Debug.OpenOutputFile ();

							  PCore->Debug << Offset << "(" << PS->FileName << " is the received content number "
										   << PCore->NoSP << ")" << endl;

							  PCore->Debug.CloseFile ();

#endif

							  PCore->KeysOfReceivedPayloads.push_back (PS->Key);

							  PS->Status = "Delete";
							}
						  else
							{
							  // TODO - FIXP/Update - Modified this debug message to include details. Modifications up to the end of this branch
							  PB->S << Offset1
									<< "(ERROR: The hash of the file " << PS->FileName
									<< " is not the same than the one generated on the publisher. i.e. " << PS->Key
									<< ")" << endl;

							  // Put back on the subscriptions waiting delivery
							  PS->Status = "Waiting delivery";

							  // Inform that previous received payload is with error or failing in integrity
							  PS->HasContent=false;
							}
						}
					}

				  F1.CloseFile ();
				}
			  else
				{
				  PB->S << Offset1
						<< "(Warning: The received file is from an application still unknown. Waiting for discovery first)"
						<< endl;
				}
			}

		  // Deleting finished subscriptions
		  if (PS->Status == "Delete")
			{

#ifdef DEBUG
			  PB->S << Offset1 << "(Deleting the subscription with index = " << i << ")" << endl;
#endif

			  PCore->DeleteSubscription (PS);
			}
		}
	}
  else
	{

#ifdef DEBUG
	  PB->S << Offset1 << "(Waiting for PSS/NRNCS discovery)" << endl;
#endif
	}

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
  else
	{
#ifdef DEBUG

	  PB->S << Offset << "(The following messages have been scheduled)" << endl;

	  for (unsigned int d=0; d<ScheduledMessages.size(); d++)
		{
		  PB->S << endl << *ScheduledMessages.at(d) << endl;
		}

#endif
	}

  delete Servers;
  delete Clients;
  delete Subject;

#ifdef DEBUG

  PB->S << Offset << "(Deleting the previous marked messages)" << endl;

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
