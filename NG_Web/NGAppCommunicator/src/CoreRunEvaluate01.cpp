/*
	NovaGenesis

	Name:		CoreRunEvaluate01
	Object:		CoreRunEvaluate01
	File:		CoreRunEvaluate01.cpp
	Author:		Antonio M. Alberti
	Date:		12/2012
	Version:	0.1
*/

#ifndef _CORERUNEVALUATE01_H
#include "CoreRunEvaluate01.h"
#endif

#ifndef _APP_H
#include "App.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif


#include "NGFilesSubscribed.h"
#include "NGManager.h"


CoreRunEvaluate01::CoreRunEvaluate01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
	arrBuffer.clear();
}

CoreRunEvaluate01::~CoreRunEvaluate01()
{
}

// Run the actions behind a received command line
// ng -run --evaluate 0.1
int CoreRunEvaluate01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 					Status=OK;
	string					Offset = "                    ";
	string					Offset1 = "                              ";
	Core					*PCore=0;
//	AppBrowser						*PApp=0;
	vector<string> 			*Values=new vector<string>;
	vector<string> 			*Servers=new vector<string>;
	vector<string> 			*Clients=new vector<string>;
	vector<string> 			*Subject=new vector<string>;
	string					Goto;
	string 					LN1;
	string 					LN2;
	string 					LN3;
	string 					LN4;
	Message 				*Run=0;
	vector<Tuple*>  		PSSs;
	vector<Tuple*>  		Apps;
	CommandLine				*PCL=0;
	bool					ClearScheduledMessage=true;
//	double					Time=GetTime();
	string::size_type		Pos=0;
	string 					Extension;
	File 					F1;
	stringstream 			ss;
	string 					HUID;
	string					Method;
	string 					SourcePID;
	string 					DestinationPID;
	string					PayloadString;
	string					PayloadHash;
//	unsigned int 			Index;
	string					Type;
	Subscription 			*PS=0;
	string					AcceptedHash;
	string 					AcceptedFileName;
	string 					AcceptedPayload;
	File 					F2;

	PCore=(Core*)PB;

//	PApp=(AppBrowser*)PB->PP;

	PB->S << Offset <<  this->GetLegibleName() << endl;

	//PB->S << Offset <<  "(Time = "<<Time<<")"<<endl;
	//PB->S << Offset <<  "(NextPeerEvaluationTime = " << PCore->NextPeerEvaluationTime << ")"<<endl;

	//PB->S << Offset <<  "(1. Check for PSS awareness.)"<<endl;

	// *************************************************************
	// Check for PSS discovery first Step
	// *************************************************************
	if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"PSS",Values,PB) == ERROR)
		{
			vector<string> Cat2Keywords;

			Cat2Keywords.push_back("OS");
			Cat2Keywords.push_back("PSS");
			Cat2Keywords.push_back("PS");

			vector<string> Cat9Keywords;

			Cat9Keywords.push_back("Host");

			// Schedule a first step of PSS discovery
			PCore->DiscoveryFirstStep(PB->PP->GetOperatingSystemSelfCertifyingName(),&Cat2Keywords,&Cat9Keywords,ScheduledMessages);

			PB->S << Offset1 << "(Not aware of any PSS on Categories 2 and 9. Prepare discover first step)"<<endl;

			ClearScheduledMessage=false;
		}
	else
		{
			//PB->S << Offset1 << "(Aware of a PSS on Categories 2 and 9)"<<endl;
		}

	// *************************************************************
	// Check for PSS discovery second Step
	// *************************************************************
	if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("PSS","PS",PSSs,PB) == ERROR)
		{
			vector<string> Cat2Keywords;

			Cat2Keywords.push_back("OS");
			Cat2Keywords.push_back("PSS");
			Cat2Keywords.push_back("PS");

			vector<string> Cat9Keywords;

			Cat9Keywords.push_back("Host");

			// Schedule a second step of PSS discovery
			PCore->DiscoverySecondStep(PB->PP->GetOperatingSystemSelfCertifyingName(),&Cat2Keywords,&Cat9Keywords,ScheduledMessages);

			PB->S << Offset1 << "(Not aware of any PSS on Categories 5 and 6. Prepare discover second step)"<<endl;

			ClearScheduledMessage=false;
		}
	else
		{
			// Test storage of new PSS if there is some candidate
			if (PSSs.size() > 0)
				{
					// Loop over the discovered candidates
					for (unsigned int g=0; g<PSSs.size(); g++)
						{
							// Auxiliar flag
							bool StoreFlag=true;

							// Loop over the already stored tuples
							for (unsigned int h=0; h<PCore->PSSTuples.size(); h++)
								{
									if(PSSs[g]->Values[2] == PCore->PSSTuples[h]->Values[2])
										{
											StoreFlag=false;

											PB->S << Offset1 << "(The app is already aware of this PSS)"<<endl;
										}
								}

							if (StoreFlag == true)
								{
									PCore->PSSTuples.push_back(PSSs[g]);

									PB->S << Offset1 << "(Discovered a PSS)"<<endl;
								}
						}
				}
			else
				{
					PB->S << Offset1 << "(There is no candidate for PSS)"<<endl;
				}

			// Run other procedures if it is aware of at least one PSS
			if (PCore->PSSTuples.size() > 0)
				{
					PB->S << Offset1 << "(Try to Run expose files)"<<endl;
					if (PCore->RunExpose == true)
						{
							PB->S << Offset1 << "(Run expose once in entire life)"<<endl;
							vector<string> arrKeywords;
							arrKeywords.push_back("imagens2");
							PCore->SetKeywordsAndTheirHashes(arrKeywords);

							PCore->Exposition(PB->PP->GetDomainSelfCertifyingName(),ScheduledMessages);

							PCore->RunExpose = false;

							ClearScheduledMessage=false;

							PB->State="operational";


							PB->S << Offset1 <<"(The payload hash is the same than the one generated on the publisher)"<< endl;

							//PB->S << Offset1 << "(The published Service Offer was accepted by the peer!)" << endl;

							//PB->S << Offset1 << "(There is a scheduled message)" << endl;

						}
				if (ScheduledMessages.size() > 0 )
					{
						Run=ScheduledMessages.at(0);

						if( Run != 0 ){
								NGManager* ngc = NGManager::getInstance();
								if( ngc->hasLiteral() ){
#if 0
									// Adding a ng -run --publish command line
									Run->NewCommandLine("-run","--photopublish","0.1",PCL);

									Run->SetTime(GetTime()+PCore->DelayBeforeANewPhotoPublish);

									ClearScheduledMessage=false;

									//PS->Status = "Delete";
#endif
									PB->S << Offset1 << "(Subscribing category[2] )"<<endl;

									// Adding a ng -run --subscribe command line
									Run->NewCommandLine("-run","--subscribe","0.1",PCL);

									PCL->NewArgument(1);

									PCL->SetArgumentElement(0,0,"2");

									ngc->pullArrLiteral(&arrBuffer);

									//for(unsigned int i = 0; i < arrBuffer.size(); ++i) {
										PCL->NewArgument(1);
										PCL->SetArgumentElement(1,0,arrBuffer.at(arrBuffer.size()-1));
										//PCL->SetArgumentElement(i+1,0,arrBuffer.at(i));
									//}

									PB->S << Offset1 << "(Subscribing:"<< arrBuffer.at(arrBuffer.size()-1) << " )"<<endl;

									ClearScheduledMessage=false;

									PCore->IsPublished = true;

									//Status=OK;
									// Enable publication of all the photos again to notify the new peer
									//PCore->PublishAllPhotos = true;

								} else if( arrBuffer.size() > 0 ) {
									PB->S << Offset1 << "(WORD was subscribed)"<<endl;
									vector<string> *_Values=new vector<string>;
									//for (unsigned int i = 0; i < arrBuffer.size(); ++i) {
										PCore->PHT->GetBinding(2,arrBuffer.at(0),_Values);
									//}
									if( _Values->size() > 0 ){
										arrBuffer.erase(arrBuffer.begin());
										vector<string> arrListToSubscribe;
										NGFilesSubscribed* ngfs = NGFilesSubscribed::getInstance();
										ngfs->getListToSubscribe(_Values,&arrListToSubscribe);
										if( !arrListToSubscribe.empty() ){
											PB->S << Offset1 << "(Subscribing category[18] )"<<endl;

											for (unsigned int i = 0; i < arrListToSubscribe.size(); ++i) {
												// Adding a ng -run --subscribe command line
												Run->NewCommandLine("-run","--subscribe","0.1",PCL);

												PCL->NewArgument(1);

												PCL->SetArgumentElement(0,0,"18");

												PCL->NewArgument(1);

												PCL->SetArgumentElement(1,0,arrListToSubscribe.at(i));

												PB->S << Offset1 << "(Sub["<< (i+1) << "]) " << arrListToSubscribe.at(i) <<endl;
											}

											ClearScheduledMessage=false;

											/* Notify NGBrowser about hash's subscribed */
											ngfs->addFilesSubscribed(arrListToSubscribe);

										} else {
											ClearScheduledMessage = true;
										}
										ngc->ifaceComplete(_Values);
									} else {
										PB->S << Offset1 << "(Waiting for receive Buffer subscribed)"<<endl;
										for (unsigned int i = 0; i < arrBuffer.size(); ++i) {
											PB->S << Offset1 << "-> " << arrBuffer.at(i) <<endl;
										}
									}
									delete _Values;
								} /* !PCore->IsPublished  */
								else {
									PB->S << Offset1 << "(Buffer EMPTY, nothing to subscribe)"<<endl;
								}
						} /*  Run != 0  */
					} /* ScheduledMessages */
				}
		}

	//PB->S << Offset << "(2. Check for new peer application tuples)"<<endl;
#if 0
	// *************************************************************
	// Check for new peer server app tuples
	// *************************************************************
	if (PCore->PSSTuples.size() > 0)
		{
			if (PCore->NextPeerEvaluationTime < Time)
				{
					//PB->S << endl << Offset1 << "(Check for new peer server application tuples)"<<endl;

					if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Server",Servers,PB) == OK &&
						PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Photo",Subject,PB) == OK &&
						PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("App","Core",Apps,PB) == OK)
						{
							/*

							PB->S << endl << Offset1 << "(Servers vector size = " << Servers->size() << ")" << endl;

							for (unsigned int u=0; u< Servers->size(); u++)
								{
									PB->S << Offset1 << "(Server ["<<u<<"] = " << Servers->at(u) << ")" << endl;
								}

							PB->S << Offset1 << "(Subject vector size = " << Subject->size() << ")" << endl;

							for (unsigned int v=0; v< Subject->size(); v++)
								{
									PB->S << Offset1 << "(Server ["<<v<<"] = " << Subject->at(v) << ")" << endl;
								}

							PB->S << Offset1 << "(App tuples vector size = " << Apps.size() << ")" << endl;

							for (unsigned int w=0; w< Apps.size(); w++)
								{
									PB->S << Offset1 << "(Server ["<<w<<"] = " << Apps.at(w)->Values.at(2) << ")" << endl;
								}

								*/

							// Check for new servers
							for (unsigned int i=0; i<Servers->size(); i++)
								{
									//PB->S << Offset1 << "(Testing the Server = " << Servers->at(i) << ")" << endl;

									// Is the server different than this process
									if (Servers->at(i) != PB->PP->GetSelfCertifyingName())
										{
											for (unsigned int j=0; j<Subject->size(); j++)
												{
													// Has the server the same subject?
													if (Servers->at(i) == Subject->at(j))
														{
															// Run over Apps
															for (unsigned int k=0; k<Apps.size(); k++)
																{
																	// Is the server an application with core block
																	if (Servers->at(i) == Apps[k]->Values[2])
																		{
																			// Auxiliar flag
																			bool StoreFlag=true;

																			// Loop over the already stored server tuples
																			for (unsigned int h=0; h<PCore->PeerServerAppTuples.size(); h++)
																				{
																					// Is the server already stored?
																					if(Servers->at(i) == PCore->PeerServerAppTuples[h]->Values[2])
																						{
																							StoreFlag=false;
																						}
																					else
																						{
																							//PB->S << Offset1 << "(The candidate is already stored)"<<endl;
																						}
																				}

																			if (StoreFlag == true)
																				{
																					// Store the learned tuple on the peer server app tuples
																					PCore->PeerServerAppTuples.push_back(Apps[k]);

																					unsigned int _I=PCore->PeerServerAppTuples.size()-1;

																					//PB->S << Offset1 << "(Discovered a candidate for a Server application)" << endl;
																					//PB->S << Offset1 << "(HID = " << PCore->PeerServerAppTuples[_I]->Values[0] << ")" << endl;
																					//PB->S << Offset1 << "(OSID = " << PCore->PeerServerAppTuples[_I]->Values[1] << ")" << endl;
																					//PB->S << Offset1 << "(PID = " << PCore->PeerServerAppTuples[_I]->Values[2] << ")" << endl;
																					//PB->S << Offset1 << "(BID = " << PCore->PeerServerAppTuples[_I]->Values[3] << ")" << endl;

																					// Only clients invite servers
																					if (PApp->Role == "Client")
																						{
																							if (ScheduledMessages.size() > 0)
																								{
																									//PB->S << Offset1 << "(There is a scheduled message with a ng -invite command line)" << endl;

																									Run=ScheduledMessages.at(0);

																									if (Run != 0)
																										{
																											// Adding a ng -run --invitation command line
																											Run->NewCommandLine("-run","--invite","0.1",PCL);

																											PCL->NewArgument(2);

																											PCL->SetArgumentElement(0,0,"Server");

																											PCL->SetArgumentElement(0,1,PB->IntToString(_I));

																											ClearScheduledMessage=false;

																											PCore->DelayBeforeRunPeriodic=30;
																										}
																								}
																						}
																				}
																			else
																				{
																					//PB->S << Offset1 << "(The app is already aware of this Server)"<<endl;
																				}
																		}
																}
														}
												}
										}
									else
										{
											//PB->S << Offset1 << "(The candidate is the own process)"<<endl;
										}
								}
						}

					// Clear the vectors
					Subject->clear();
					Apps.clear();

					//PB->S << endl << Offset1 << "(Check for new peer client application tuples)"<<endl;

					// *************************************************************
					// Check for new peer client app tuples
					// *************************************************************
					if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Client",Clients,PB) == OK &&
						PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Photo",Subject,PB) == OK &&
						PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("App","Core",Apps,PB) == OK)
						{
							//PB->S << endl << Offset1 << "(Clients vector size = " << Clients->size() << ")" << endl;
							//PB->S << Offset1 << "(Subject vector size = " << Subject->size() << ")" << endl;
							//PB->S << Offset1 << "(App tuples vector size = " << Apps.size() << ")" << endl;

							// Check for new clients
							for (unsigned int i=0; i<Clients->size(); i++)
								{
									//PB->S << Offset1 << "(Testing the Client = " << Clients->at(i) << ")" << endl;

									// Is the client different than this process
									if (Clients->at(i) != PB->PP->GetSelfCertifyingName())
										{
											for (unsigned int j=0; j<Subject->size(); j++)
												{
													// Has the client the same subject?
													if (Clients->at(i) == Subject->at(j))
														{
															// Run over Apps
															for (unsigned int k=0; k<Apps.size(); k++)
																{
																	// Is the client an application with core block
																	if (Clients->at(i) == Apps[k]->Values[2])
																		{
																			// Auxiliar flag
																			bool StoreFlag=true;

																			// Loop over the already stored client tuples
																			for (unsigned int h=0; h<PCore->PeerClientAppTuples.size(); h++)
																				{
																					// Is the client already stored?
																					if(Clients->at(i) == PCore->PeerClientAppTuples[h]->Values[2])
																						{
																							StoreFlag=false;
																						}
																					else
																						{
																							//PB->S << Offset1 << "(The candidate is already stored)"<<endl;
																						}
																				}

																			if (StoreFlag == true)
																				{
																					if (PApp->Role == "Server")
																						{
																							// Store the learned tuple on the peer client app tuples
																							PCore->PeerClientAppTuples.push_back(Apps[k]);

																							unsigned int _I=PCore->PeerClientAppTuples.size()-1;

																							//PB->S << Offset1 << "(Discovered a candidate for a Client application)" << endl;
																							//PB->S << Offset1 << "(HID = " << PCore->PeerClientAppTuples[_I]->Values[0] << ")" << endl;
																							//PB->S << Offset1 << "(OSID = " << PCore->PeerClientAppTuples[_I]->Values[1] << ")" << endl;
																							//PB->S << Offset1 << "(PID = " << PCore->PeerClientAppTuples[_I]->Values[2] << ")" << endl;
																							//PB->S << Offset1 << "(BID = " << PCore->PeerClientAppTuples[_I]->Values[3] << ")" << endl;

																							PCore->DelayBeforeRunPeriodic=60;
																						}

																					// A client inviting another client
																					if (PApp->Role == "Client")
																						{
																							if (ScheduledMessages.size() > 0)
																								{
																									//PB->S << Offset1 << "(There is a scheduled message with a ng -invite command line)" << endl;

																									Run=ScheduledMessages.at(0);

																									if (Run != 0)
																										{
																											// Store the learned tuple on the peer client app tuples
																											PCore->PeerClientAppTuples.push_back(Apps[k]);

																											unsigned int _I=PCore->PeerClientAppTuples.size()-1;

																											//PB->S << Offset1 << "(Discovered a candidate for a Client application)" << endl;
																											//PB->S << Offset1 << "(HID = " << PCore->PeerClientAppTuples[_I]->Values[0] << ")" << endl;
																											//PB->S << Offset1 << "(OSID = " << PCore->PeerClientAppTuples[_I]->Values[1] << ")" << endl;
																											//PB->S << Offset1 << "(PID = " << PCore->PeerClientAppTuples[_I]->Values[2] << ")" << endl;
																											//PB->S << Offset1 << "(BID = " << PCore->PeerClientAppTuples[_I]->Values[3] << ")" << endl;

																											// Adding a ng -run --invitation command line
																											Run->NewCommandLine("-run","--invite","0.1",PCL);

																											PCL->NewArgument(2);

																											PCL->SetArgumentElement(0,0,"Client");

																											PCL->SetArgumentElement(0,1,PB->IntToString(_I));

																											ClearScheduledMessage=false;

																											PCore->DelayBeforeRunPeriodic=60;
																										}
																								}
																						}
																				}
																			else
																				{
																					//PB->S << Offset1 << "(The app is already aware of this Client)"<<endl;
																				}
																		}
																}
														}
													else
														{
															//PB->S << Offset1 << "(The candidate is not related to \"Photo\" subject)"<<endl;
														}
												}
										}
									else
										{
											//PB->S << Offset1 << "(The candidate is the own process)"<<endl;
										}
								}
						}

					PCore->NextPeerEvaluationTime=Time+PCore->DelayBeforeANewPeerEvaluation;
				}
			else
				{
					PB->S << Offset1 << "(Too early for that. Wait next ng -run --evaluate)"<<endl;
				}
		}
	else
		{
			PB->S << Offset1 << "(Waiting for PSS discovery)"<<endl;
		}

	//PB->S << Offset <<  "(3. Show the discovered server App(s))"<<endl;

	// *************************************************************
	// Show the discovered server App(s)
	// *************************************************************
	if (PCore->PeerServerAppTuples.size() > 0)
		{
			for (unsigned int i=0; i<PCore->PeerServerAppTuples.size(); i++)
				{
					PB->S << Offset1 << "(Aware of the Peer App Server " << i << " )" << endl;
					//PB->S << Offset1 << "(HID = " << PCore->PeerServerAppTuples[i]->Values[0] << ")" << endl;
					//PB->S << Offset1 << "(OSID = " << PCore->PeerServerAppTuples[i]->Values[1] << ")" << endl;
					//PB->S << Offset1 << "(PID = " << PCore->PeerServerAppTuples[i]->Values[2] << ")" << endl;
					//PB->S << Offset1 << "(BID = " << PCore->PeerServerAppTuples[i]->Values[3] << ")" << endl;
				}
		}
	else
		{
			PB->S << Offset1 << "(Not aware of any Server)" << endl;
		}

	//PB->S << Offset <<  "(4. Show the discovered client App(s))"<<endl;

	// *************************************************************
	// Show the discovered client App(s)
	// *************************************************************
	if (PCore->PeerClientAppTuples.size() > 0)
		{
			for (unsigned int i=0; i<PCore->PeerClientAppTuples.size(); i++)
				{
					PB->S << Offset1 << "(Aware of the Peer App Client " << i << " )" << endl;
					//PB->S << Offset1 << "(HID = " << PCore->PeerClientAppTuples[i]->Values[0] << ")" << endl;
					//PB->S << Offset1 << "(OSID = " << PCore->PeerClientAppTuples[i]->Values[1] << ")" << endl;
					//PB->S << Offset1 << "(PID = " << PCore->PeerClientAppTuples[i]->Values[2] << ")" << endl;
					//PB->S << Offset1 << "(BID = " << PCore->PeerClientAppTuples[i]->Values[3] << ")" << endl;
				}
		}
	else
		{
			PB->S << Offset1 << "(Not aware of any Client)" << endl;
		}

	//PB->S << Offset <<  "(5. Check subscriptions)"<<endl;
//#endif

	// *************************************************************
	// Check subscriptions
	// *************************************************************
	if (PCore->PSSTuples.size() > 0)
		{
			if( PCore->Subscriptions.size() > 0 ){
//#if 0
			// Looping over existent Subscriptions
			for (unsigned int i=0; i<PCore->Subscriptions.size(); i++)
				{
					PS=PCore->Subscriptions[i];

					//PB->S << Offset1 << "(Testing subscription "<<i<<")" << endl;

					//PB->S << Offset1 << "(Subscription status is "<<PS->Status<<")" << endl;

					PCore->GetPeerAppTupleType(PS->Publisher.Values[2],Index,Type);

					if (Type == "Server" || Type == "Client")
						{
							//PB->S << Offset1 << "(Publisher = " << Type << " "<< Index << ")" << endl;
						}
					else
						{
							PB->S << Offset1 << "(Warning: This application is still unknown)" << endl;
						}

					//PB->S << Offset1 << "(HID = " << PS->Publisher.Values[0] << ")" << endl;
					//PB->S << Offset1 << "(OSID = " << PS->Publisher.Values[1] << ")" << endl;
				//	PB->S << Offset1 << "(PID = " << PS->Publisher.Values[2] << ")" << endl;
				//	PB->S << Offset1 << "(BID = " << PS->Publisher.Values[3] << ")" << endl;

					// Subscription requiring a scheduling
					if (PS->Status == "Scheduling required")
						{
							if (ScheduledMessages.size() > 0)
								{
									//PB->S << Offset1 << "(There is a scheduled message)" << endl;

									Run=ScheduledMessages.at(0);

									if (Run != 0)
										{
											// Adding a ng -run --subscribe command line
											Run->NewCommandLine("-run","--subscribe","0.1",PCL);

											PCL->NewArgument(1);

											PCL->SetArgumentElement(0,0,PB->IntToString(PS->Category));

											PCL->NewArgument(1);

											PCL->SetArgumentElement(1,0,PS->Key);

											PB->S << Offset1 << "(Going to subscribe the key = "<<PS->Key<<")" << endl;

											PS->Status = "Waiting delivery";

											//PB->S << Offset1 << "(Changing subscription status from \"Scheduling required\" to \"Waiting delivery\")" << endl;

											// Storing a timestamp for performance evaluation
											PS->Timestamp=GetTime();

											ClearScheduledMessage=false;

											PCore->Debug.OpenOutputFile();

											PCore->Debug << "Evaluating a scheduling required with key " << PS->Key << ". Creating a ng -run --subscribe message. Status of the subscription is " <<PS->Status<< endl;

											PCore->Debug.CloseFile();
										}
								}
						}

					// Subscription requiring processing of a delivered content
					if (PS->Status == "Processing required")
						{
							if (PCore->GetPeerAppTupleType(PS->Publisher.Values[2],Index,Type) == OK)
								{
									PB->S << Offset1 << "(Checking the file received from the peer with name " << PS->FileName << ")" << endl;

									Pos=PS->FileName.rfind('.');

									if(Pos != string::npos)
										{
											Extension = PS->FileName.substr(Pos+1);
										}

									PB->S << Offset1 << "(File extension = " << Extension << ")" << endl;

									if (Extension == "txt")
										{
											F1.OpenInputFile(PS->FileName,PB->GetPath(),"DEFAULT");

											F1.seekg(0);

											F1 >> HUID;
											F1 >> Method;
											F1 >> SourcePID;
											F1 >> DestinationPID;

											//PB->S << Offset1 << "(HUID = " << HUID << ")" << endl;
											//PB->S << Offset1 << "(Method = " << Method << ")" << endl;
											//PB->S << Offset1 << "(SourcePID = " << SourcePID << ")" << endl;
											//PB->S << Offset1 << "(DestinationPID = " << DestinationPID << ")" << endl;

											if (HUID == "Alberti_HUID" && Method == "Offer")
												{
													Run=ScheduledMessages.at(0);

													if (Run != 0)
														{
															if (PCore->GetFileContentHash(PS->FileName,PayloadHash) == OK)
																{
																	PB->S << Offset1 <<"(The payload hash calculated now is              " << PayloadHash << ")"<< endl;
																	PB->S << Offset1 <<"(The payload hash calculated at the publisher is " << PS->Key << ")"<< endl;

																	// Integrity check
																	if (PS->Key == PayloadHash)
																		{
																			PB->S << Offset1 <<"(The payload hash is the same than the one generated on the publisher)"<< endl;

																			//PB->S << Offset1 << "(There is a scheduled message)" << endl;

																			// Adding a ng -run --publish command line
																			Run->NewCommandLine("-run","--publish","0.1",PCL);

																			PCL->NewArgument(3);

																			// Use the stored Subscription to fulfill the command line
																			PCL->SetArgumentElement(0,0,Type);

																			PCL->SetArgumentElement(0,1,PB->IntToString(Index));

																			// Set the service offer payload
																			AcceptedPayload="Alberti_HUID Accepted "+PB->PP->GetSelfCertifyingName()+" "+PS->Publisher.Values[2];

																			unsigned int Complement=rand()%10000000000000;

																			// Set the file name
																			AcceptedFileName="Service_Accepted_"+PB->UnsignedIntToString(Complement)+".txt";

																			// Open the file to write
																			F2.OpenOutputFile(AcceptedFileName,PB->GetPath(),"DEFAULT");

																			F2 << AcceptedPayload;

																			F2.CloseFile();

																			PCL->SetArgumentElement(0,2,AcceptedFileName);

																			ClearScheduledMessage=false;

																			PS->Status = "Delete";

																			PCore->Debug.OpenOutputFile();

																			PCore->Debug << "Evaluating a processing required with key " << PS->Key << ". Creating the "<<AcceptedFileName<<". Status of the subscription is " <<PS->Status<< endl;

																			PCore->Debug.CloseFile();
																		}
																	else
																		{
																			PB->S << Offset1 << "(ERROR: The received file hash differs from the publihed one)" << endl;
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

											if(HUID == "Alberti_HUID" && Method == "Accepted")
												{
													Run=ScheduledMessages.at(0);

													if (Run != 0)
														{
															if (PCore->GetFileContentHash(PS->FileName,PayloadHash) == OK)
																{
																	PB->S << Offset1 <<"(The payload hash calculated now is              " << PayloadHash << ")"<< endl;
																	PB->S << Offset1 <<"(The payload hash calculated at the publisher is " << PS->Key << ")"<< endl;

																	// Integrity check
																	if (PS->Key == PayloadHash)
																		{
																			PB->S << Offset1 <<"(The payload hash is the same than the one generated on the publisher)"<< endl;

																			//PB->S << Offset1 << "(The published Service Offer was accepted by the peer!)" << endl;

																			//PB->S << Offset1 << "(There is a scheduled message)" << endl;

																			// Adding a ng -run --publish command line
																			Run->NewCommandLine("-run","--photopublish","0.1",PCL);

																			Run->SetTime(GetTime()+PCore->DelayBeforeANewPhotoPublish);

																			ClearScheduledMessage=false;

																			PS->Status = "Delete";

																			// Enable publication of all the photos again to notify the new peer
																			PCore->PublishAllPhotos = false;

																			PCore->Debug.OpenOutputFile();

																			PCore->Debug << "Evaluating a processing required with key " << PS->Key << ". Going to publish the photos. Status of the subscription is " <<PS->Status<< endl;

																			PCore->Debug.CloseFile();
																		}
																	else
																		{
																			PB->S << Offset1 << "(ERROR: The received file hash differs form the publihed one)" << endl;
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

									if (Extension == "jpg")
										{
											if (PCore->GetFileContentHash(PS->FileName,PayloadHash) == OK)
												{
													PB->S << Offset1 <<"(The payload hash calculated now is              " << PayloadHash << ")"<< endl;
													PB->S << Offset1 <<"(The payload hash calculated at the publisher is " << PS->Key << ")"<< endl;

													// Integrity check
													if (PS->Key == PayloadHash)
														{
															PB->S << Offset1 <<"(The payload hash is the same than the one generated on the publisher)"<< endl;

															bool NewPhoto=true;

															for (unsigned int i=0; i<PCore->Content.size(); i++)
																{
																	if (PayloadHash == PCore->Content[i])
																		{
																			NewPhoto=false;

																			PB->S << Offset <<  "(The photo "<<PS->FileName<<" is already on the available content control)"<<endl;
																		}
																}

															if (NewPhoto == true)
																{
																	//PB->S << Offset1 <<"(Updating the available content control)"<< endl;

																	PCore->Content.push_back(PayloadHash);
																}

															PS->Status = "Delete";
														}
												}
										}

									F1.CloseFile();
								}
							else
								{
									PB->S << Offset1 << "(Warning: The received file is from an application still unknown. Waiting for discovery first)" << endl;
								}
						}

					// Deleting finished subscriptions
					if (PS->Status == "Delete")
						{
							PB->S << Offset1 <<  "(Deleting the subscription with index = "<<i<<")" << endl;

							PCore->DeleteSubscription(PS);
						}
				}

			} else {
				if (ScheduledMessages.size() > 0)
				{
					Run=ScheduledMessages.at(0);
					if( Run != 0 ){
						PB->S << Offset1 <<"(The payload hash is the same than the one generated on the publisher)"<< endl;

						//PB->S << Offset1 << "(The published Service Offer was accepted by the peer!)" << endl;

						//PB->S << Offset1 << "(There is a scheduled message)" << endl;

						// Adding a ng -run --publish command line
						Run->NewCommandLine("-run","--photopublish","0.1",PCL);

						Run->SetTime(GetTime()+PCore->DelayBeforeANewPhotoPublish);

						//ClearScheduledMessage=false;

						//PS->Status = "Delete";

						// Enable publication of all the photos again to notify the new peer
						PCore->PublishAllPhotos = true;

						//PCore->Debug.OpenOutputFile();

						//PCore->Debug << "Evaluating a processing required with key " << PS->Key << ". Going to publish the photos. Status of the subscription is " <<PS->Status<< endl;

						//PCore->Debug.CloseFile();
					}
				}
			}
		}
	else
		{
			PB->S << Offset1 << "(Waiting for PSS discovery)"<<endl;
		}
#endif
/*
#else
	if (PCore->PSSTuples.size() > 0)
	{
			PB->S << Offset1 << "(Publicando: PHOTOPUBLISH)" << endl;
			// Adding a ng -run --publish command line
			Run->NewCommandLine("-run","--photopublish","0.1",PCL);

			Run->SetTime(GetTime()+PCore->DelayBeforeANewPhotoPublish);

			ClearScheduledMessage=false;
	}

#endif
*/

	if (PCore->PSSTuples.size() > 0)
	{

		if( PCore->Subscriptions.size() > 0 )
		{
			// Looping over existent Subscriptions
			for (unsigned int i=0; i<PCore->Subscriptions.size(); i++)
			{
					PS=PCore->Subscriptions[i];
					if (PS->Status == "Processing required")
					{
						PB->S << Offset1 << "(Subscribing Checking: Processing required.)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: Processing required.)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: Processing required.)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: Processing required.)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: Processing required.)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: Processing required.)"<<endl;
					} else {
						PB->S << Offset1 << "(Subscribing Checking: "<<PS->Status<<" .)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: "<<PS->Status<<" .)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: "<<PS->Status<<" .)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: "<<PS->Status<<" .)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: "<<PS->Status<<" .)"<<endl;
						PB->S << Offset1 << "(Subscribing Checking: "<<PS->Status<<" .)"<<endl;
					}
			}
		} else {
			PB->S << Offset1 << "(Subscribing Checking: Phase 2)"<<endl;
		}
	} else {
		PB->S << Offset1 << "(Subscribing Checking: Phase 1)"<<endl;
	}

	if (ClearScheduledMessage == true)
		{
			if (ScheduledMessages.size() > 0)
				{
					Message *Temp=ScheduledMessages.at(0);

					Temp->Delete=true;

					// Make the scheduled messages vector empty
					ScheduledMessages.clear();
				}
		}

	delete Values;
	delete Servers;
	delete Clients;
	delete Subject;

	//PB->S << Offset <<  "(Deleting the previous marked messages)" << endl;

  	PB->PP->DeleteMarkedMessages();

	//PB->S << Offset <<  "(Done)" << endl << endl << endl;

	return Status;
}



	/*
	 *

	 Verificacao de integridade do arquivo recebido

	 	PB->S << Offset <<  "(6. Check whether a payload arrived.)"<<endl;

	// *************************************************************
	// Check if a delivered message arrived.
	// *************************************************************
	if (PCore->LastExternalReceivedMessageMainAction == "-payload --info 0.1")
		{
			PB->S << Offset1 << "(The received file has the name "<<PCore->LastExternalReceivedMessageFileName<<endl;

			Pos=PCore->LastExternalReceivedMessageFileName.rfind('.');

			if(Pos != string::npos)
				{
					Extension = PCore->LastExternalReceivedMessageFileName.substr(Pos+1);
				}

			PB->S << Offset1 << "(File extension = " << Extension << ")" << endl;

			if (Extension == "txt")
				{
					F1.OpenInputFile(PCore->LastExternalReceivedMessageFileName,PB->GetPath(),"DEFAULT");

					F1.seekg(0);

					F1 >> HUID;
					F1 >> Method;
					F1 >> SourcePID;

					PB->S << Offset1 << "(HUID = " << HUID << ")" << endl;
					PB->S << Offset1 << "(Method = " << Method << ")" << endl;
					PB->S << Offset1 << "(SourcePID = " << SourcePID << ")" << endl;

					if (HUID == "Alberti_HUID" && Method == "Offer")
						{
							PB->S << Offset1 << "(There is a scheduled message)" << endl;

							Run=ScheduledMessages.at(0);

							if (Run != 0)
								{
									F1.seekg (0, ios::end);

									// Read the number of characters in the file.
									PayloadSize=F1.tellg();

									PB->S << Offset1 <<"(The payload size is " << PayloadSize << " bytes)" << endl;

									if (PayloadSize > 0)
										{
											char *Payload=new char[PayloadSize];

											F1.seekg (0);

											F1.read(Payload,PayloadSize);

											for (unsigned int h=0; h<PayloadSize; h++)
												{
													PayloadString=PayloadString+Payload[h];
												}

											PB->S << Offset1 <<"(The payload is:" << endl << Offset1 << PayloadString << endl << Offset1 << ")"<< endl;

											// Generate the SCN for the payload
											PMB->GenerateSCNFromCharArrayBinaryPatterns16Bytes(PayloadString,PayloadHash);

											PB->S << Offset1 <<"(The payload hash is " << PayloadHash << ")"<< endl;

											Subscription *PS=0;

											// Get the publisher information based on the obtained key
											if (PCore->GetSubscription(PayloadHash,PS) == OK)
												{
													if (PS->Status == "Process")
														{
															if (PCore->GetPeerAppTupleType(PS->Publisher.Values[2],Index,Type) == OK)
																{
																	// Adding a ng -run --publish command line
																	Run->NewCommandLine("-run","--publish","0.1",PCL);

																	PCL->NewArgument(3);

																	// Use the stored Subscription to fulfill the command line
																	PCL->SetArgumentElement(0,0,Type);

																	PB->S << Offset1 <<"(The peer is a " << Type << ")"<< endl;

																	PCL->SetArgumentElement(0,1,PB->IntToString(Index));

																	PB->S << Offset1 <<"(The peer has the index " << Index << ")"<< endl;

																	PCL->SetArgumentElement(0,2,"Service_Accepted_"+PB->PP->GetSelfCertifyingName()+".txt");

																	ClearScheduledMessage=false;

																	PS->Status = "Delete";
																}
														}
												}
											else
												{
													PB->S << Offset1 << "(ERROR: Unable to find a subscription that matches the offered key)" << endl;
												}
										}
									else
										{
											PB->S << Offset1 << "(ERROR: Payload size is zero or negative)" << endl;
										}
								}
						}

					if(HUID == "Alberti_HUID" && Method == "Accepted")
						{
							// Reduce the time between peer evaluations
							PCore->DelayBeforeANewPeerEvaluation=60;

							PB->S << Offset1 << "(Service Accepted!!)" << endl;

							PB->S << Offset1 << "(There is a scheduled message)" << endl;

							Run=ScheduledMessages.at(0);

							if (Run != 0)
								{
									F1.seekg (0, ios::end);

									// Read the number of characters in the file.
									PayloadSize=F1.tellg();

									PB->S << Offset1 <<"(The payload size is " << PayloadSize << " bytes)" << endl;

									if (PayloadSize > 0)
										{
											char *Payload=new char[PayloadSize];

											F1.seekg (0);

											F1.read(Payload,PayloadSize);

											for (unsigned int h=0; h<PayloadSize; h++)
												{
													PayloadString=PayloadString+Payload[h];
												}

											PB->S << Offset1 <<"(The payload is:" << endl << Offset1 << PayloadString << endl << Offset1 << ")"<< endl;

											// Generate the SCN for the payload
											PMB->GenerateSCNFromCharArrayBinaryPatterns16Bytes(PayloadString,PayloadHash);

											PB->S << Offset1 <<"(The payload hash is " << PayloadHash << ")"<< endl;

											Subscription *PS=0;

											// Get the publisher information based on the obtained key
											if (PCore->GetSubscription(PayloadHash,PS) == OK)
												{
													if (PS->Status == "Process")
														{
															// Adding a ng -run --subscribe command line
															// Run->NewCommandLine("-run","--publish","0.1",PCL);

															// PCL->NewArgument(3);

															// Use the stored Subscription to fulfill the command line
															//PCL->SetArgumentElement(0,0,PS->Type);

															PB->S << Offset1 <<"(The peer is a " << PS->Type << ")"<< endl;

															//PCL->SetArgumentElement(0,1,PB->IntToString(PS->Index));

															PB->S << Offset1 <<"(The peer has the index " << PS->Index << ")"<< endl;

															//PCL->SetArgumentElement(0,2,"Service_Accepted_"+PB->PP->GetSelfCertifyingName()+".txt");

															ClearScheduledMessage=true;

															PS->Status = "Delete";
														}
												}
											else
												{
													PB->S << Offset1 << "(ERROR: Unable to find a subscription that matches the offered key)" << endl;
												}
										}
									else
										{
											PB->S << Offset1 << "(ERROR: Payload size is zero or negative)" << endl;
										}
								}


						}

					F1.CloseFile();
				}

			if (Extension == "jpg")
				{

				}
		}

	 */
/*


 	if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Server",Role,PB) == OK && PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Photo",Subject,PB) == OK)
		{
			if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("App","Core",CandidateTuples,PB) == OK)
				{
					if (CandidateTuples.size() > 1)
						{
							Tuple *PT=0;

							PB->S << Offset << "(Role vector size = " << Role->size() << ")" << endl;
							PB->S << Offset << "(Subject vector size = " << Subject->size() << ")" << endl;
							PB->S << Offset << "(Candidate tuples vector size = " << CandidateTuples.size() << ")" << endl;

							for (unsigned int i=0; i<CandidateTuples.size(); i++)
								{
									PT=CandidateTuples[i];

									PB->S << Offset << "(Testing the candidate = " << PT->Values[2] << ")" << endl;

									if (CandidateTuples->Values[2] != PB->PP->GetSelfCertifyingName())
										{
											// Regarding role and own process ID
											for (unsigned int j=0; j<Role->size(); j++)
												{
													if (PT->Values[2] == Role->at(j))
														{
															// Regarding the subject
															for (unsigned int k=0; k<Subject->size(); k++)
																{
																	if (PT->Values[2] == Subject->at(k))
																		{
																			// Auxiliar flag
																			bool StoreFlag=true;

																			// Loop over the already stored server tuples
																			for (unsigned int h=0; h<PCore->PeerServerAppTuples.size(); h++)
																				{
																					if(PT->Values[2] == PCore->PeerServerAppTuples[h]->Values[2])
																						{
																							StoreFlag=false;

																							PB->S << Offset <<  "(The app is already aware of this Server)"<<endl;
																						}
																				}

																			if (StoreFlag == true)
																				{

																					// Store the learned tuple on the peer server app tuples
																					PCore->PeerServerAppTuples.push_back(PT);

																					unsigned int Index=PCore->PeerServerAppTuples.size()-1;

																					PB->S << Offset << "(Discovered a candidate for a Server application)" << endl;
																					PB->S << Offset << "(HID = " << PCore->PeerServerAppTuples[Index]->Values[0] << ")" << endl;
																					PB->S << Offset << "(OSID = " << PCore->PeerServerAppTuples[Index]->Values[1] << ")" << endl;
																					PB->S << Offset << "(PID = " << PCore->PeerServerAppTuples[Index]->Values[2] << ")" << endl;
																					PB->S << Offset << "(BID = " << PCore->PeerServerAppTuples[Index]->Values[3] << ")" << endl;

																					if (ScheduledMessages.size() > 0)
																						{
																							PB->S << Offset <<  "(There is a scheduled message)" << endl;

																							Run=ScheduledMessages.at(0);

																							if (Run != 0)
																								{
																									// Adding a ng -run --invitation command line
																									Run->NewCommandLine("-run","--invite","0.1",PCL);

																									PCL->NewArgument(2);

																									PCL->SetArgumentElement(0,0,"Server");

																									PCL->SetArgumentElement(0,1,PB->IntToString(Index));
																								}
																						}
																				}
																		}
																	else
																		{
																			PB->S << Offset <<  "(This is not related to Photo)"<<endl;
																		}
																}
														}
													else
														{
															PB->S << Offset <<  "(This is not a Server)"<<endl;
														}
												}
										}
									else
										{
											PB->S << Offset <<  "(This is me)"<<endl;
										}
								}

							CandidateTuples.clear();
						}
				}
		}

	PB->S << Offset <<  "(Check for client peer app tuples)"<<endl;

	// *************************************************************
	// Check for client peer app tuples
	// *************************************************************
	if (PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Client",Role,PB) == OK && PB->PP->DiscoverHomonymsEntitiesIDsFromLN(2,"Photo",Subject,PB) == OK)
		{
			if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("App","Core",CandidateTuples,PB) == OK)
				{
					if (CandidateTuples.size() > 1)
						{
							Tuple *PT=0;

							PB->S << Offset << "(Role vector size = " << Role->size() << ")" << endl;
							PB->S << Offset << "(Subject vector size = " << Subject->size() << ")" << endl;
							PB->S << Offset << "(Candidate tuples vector size = " << CandidateTuples.size() << ")" << endl;

							for (unsigned int i=0; i<CandidateTuples.size(); i++)
								{
									PT=CandidateTuples[i];

									PB->S << Offset << "(Testing the candidate = " << PT->Values[2] << ")" << endl;

									if (PT->Values[2] != PB->PP->GetSelfCertifyingName())
										{
											// Regarding role and own process ID
											for (unsigned int j=0; j<Role->size(); j++)
												{
													if (PT->Values[2] == Role->at(j))
														{
															// Regarding the subject
															for (unsigned int k=0; k<Subject->size(); k++)
																{
																	if (PT->Values[2] == Subject->at(k))
																		{
																			// Auxiliar flag
																			bool StoreFlag=true;

																			// Loop over the already stored client tuples
																			for (unsigned int h=0; h<PCore->PeerClientAppTuples.size(); h++)
																				{
																					if(PT->Values[2] == PCore->PeerClientAppTuples[h]->Values[2])
																						{
																							StoreFlag=false;

																							PB->S << Offset <<  "(The app is already aware of this Client)"<<endl;
																						}
																				}

																			if (StoreFlag == true)
																				{

																					// Store the learned tuple on the peer client app tuples
																					PCore->PeerClientAppTuples.push_back(PT);

																					unsigned int Index=PCore->PeerClientAppTuples.size()-1;

																					PB->S << Offset << "(Discovered a candidate for a Client application)" << endl;
																					PB->S << Offset << "(HID = " << PCore->PeerClientAppTuples[Index]->Values[0] << ")" << endl;
																					PB->S << Offset << "(OSID = " << PCore->PeerClientAppTuples[Index]->Values[1] << ")" << endl;
																					PB->S << Offset << "(PID = " << PCore->PeerClientAppTuples[Index]->Values[2] << ")" << endl;
																					PB->S << Offset << "(BID = " << PCore->PeerClientAppTuples[Index]->Values[3] << ")" << endl;

																					if (ScheduledMessages.size() > 0)
																						{
																							PB->S << Offset <<  "(There is a scheduled message)" << endl;

																							Run=ScheduledMessages.at(0);

																							if (Run != 0)
																								{
																									// Adding a ng -run --invitation command line
																									Run->NewCommandLine("-run","--invite","0.1",PCL);

																									PCL->NewArgument(2);

																									PCL->SetArgumentElement(0,0,"Client");

																									PCL->SetArgumentElement(0,1,PB->IntToString(Index));
																								}
																						}
																				}
																		}
																	else
																		{
																			PB->S << Offset <<  "(This is not related to Photo)"<<endl;
																		}
																}
														}
													else
														{
															PB->S << Offset <<  "(This is not a Client)"<<endl;
														}
												}
										}
									else
										{
											PB->S << Offset <<  "(This is me)"<<endl;
										}
								}

							CandidateTuples.clear();
						}
				}
		}

 */
