/*
	NovaGenesis

	Name:		CoreRunDiscover01
	Object:		CoreRunDiscover01
	File:		CoreRunDiscover01.cpp
	Author:		Antonio M. Alberti
	Date:		12/2012
	Version:	0.1
*/

#ifndef _CORERUNDISCOVER01_H
#include "CoreRunDiscover01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

CoreRunDiscover01::CoreRunDiscover01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CoreRunDiscover01::~CoreRunDiscover01()
{
}

// Run the actions behind a received command line
// ng -run --discover 0.1 [ < 1 string Limiter > < 2 string Category1 Key1 >  < 2 string Category2 Key2 > ... < 2 string CategoryN KeyN > ]
int CoreRunDiscover01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 			Status=OK;
	string			Offset = "                    ";
	unsigned int 	NA=0;
	vector<string>	Limiter;
	vector<string>	Pair;
	string			PGSPID;
	vector<string> 	*PGSBIDs=new vector<string>;
	vector<string>	Limiters;
	vector<string>	Sources;
	vector<string>	Destinations;
	Message 		*Discovery=0;
	CommandLine		*PCL=0;
	Core			*PCore=0;
	vector<string>	*Keys=new vector<string>;
	bool			Veredict=false;

	PCore=(Core*)PB;

	PB->S << Offset <<  this->GetLegibleName() << endl;

	// Load the number of arguments
	if (_PCL->GetNumberofArguments(NA) == OK)
		{
			// Check the number of arguments
			if (NA >= 2)
				{
					// Get received command line arguments
					_PCL->GetArgument(0,Limiter);

							if (Limiter.size() > 0)
								{
									if (Limiter.at(0) == PB->PP->GetOperatingSystemSelfCertifyingName())
										{
											// ***************************************************
											// The discovery is restricted to the OS
											// ***************************************************

											PB->PP->DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGS","HT",PGSPID,PGSBIDs,PB);

											if (PGSBIDs->size() > 0)
												{
													PB->S << Offset <<"(Sending a discovery message to the PGS process on the same operating system)"<< endl;

													// ***************************************************
													// Prepare the first command line
													// ***************************************************

													// Setting up the OSID as the space limiter
													Limiters.push_back(Limiter.at(0));

													// Setting up the this process as the first source SCN
													Sources.push_back(PB->PP->GetSelfCertifyingName());

													// Setting up the PS block SCN as the source SCN
													Sources.push_back(PB->GetSelfCertifyingName());

													// Setting up the PGS PID as the destination SCN
													Destinations.push_back(PGSPID);

													// Setting up the PGS::HT BID as the destination SCN
													Destinations.push_back(PGSBIDs->at(0));

													// Creating a new message
													PB->PP->NewMessage(GetTime(),1,false,Discovery);

													// Creating the ng -cl -msg command line
													PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,Discovery,PCL);

													// ***************************************************
													// Generate the get bindings to be ask on PGS
													// ***************************************************

													//PB->S << Offset << "(NA = " << NA << ")"<< endl;

													for (unsigned int j=1; j<NA; j++)
														{
															// Get received command line arguments
															_PCL->GetArgument(j,Pair);

															//PB->S << Offset << "(j = " << j << ")"<< endl;

															// Pairs=_PCL->Arguments.at(1);
															if (Pair.size() > 0)
																{
																	//PB->S << Offset << "(Category = "<<Pair.at(0)<<")"<< endl;
																	//PB->S << Offset << "(Key = "<<Pair.at(1)<<")"<< endl;

																	//PB->S << Offset <<"(Cheching if this binding already exists on the discovery message)"<< endl;

																	Discovery->DoAllTheseValuesExistInSomeCommandLine(&Pair,Veredict);

																	if (Veredict == false)
																		{
																			// Generate the ng -get --bind
																			PMB->NewGetCommandLine("0.1",PB->StringToInt(Pair.at(0)),Pair.at(1),Discovery,PCL);

																			//PB->S << Offset <<"(The binding does not exists)"<< endl;
																		}
																	else
																		{
																			//PB->S << Offset <<"(The binding already exists. Skipping it)"<< endl;
																		}
																}

															Pair.clear();
														}

													// ***************************************************
													// Generate the ng -message --type [ < 1 string 1 > ]
													// ***************************************************

													Discovery->NewCommandLine("-message","--type","0.1",PCL);

													PCL->NewArgument(1);

													PCL->SetArgumentElement(0,0,PB->IntToString(Discovery->GetType()));

													// ***************************************************
													// Generate the ng -message --seq [ < 1 string 1 > ]
													// ***************************************************

													Discovery->NewCommandLine("-message","--seq","0.1",PCL);

													PCL->NewArgument(1);

													PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

													// ******************************************************
													// Setting up the SCN command line
													// ******************************************************

													// Generate the SCN
													PB->GenerateSCNFromMessageBinaryPatterns16Bytes(Discovery,SCN);

													// Creating the ng -scn --s command line
													PMB->NewSCNCommandLine("0.1",SCN,Discovery,PCL);

													// ******************************************************
													// Finish
													// ******************************************************

													// Push the message to the GW input queue
													PCore->PGW->PushToInputQueue(Discovery);

													if (ScheduledMessages.size() > 0)
														{
															Message *Temp=ScheduledMessages.at(0);

															Temp->Delete=true;

															// Make the scheduled messages vector empty
															ScheduledMessages.clear();
														}

													Status=OK;
												}
											else
												{
													PB->S << Offset <<  "(ERROR: Failed to discover the PGS_PID and its HT_BID)" << endl;
												}
										}

									Pair.clear();

									Discovery=0;

									if (Limiter.at(0) == PB->PP->GetDomainSelfCertifyingName())
										{
											PB->S << Offset <<"(Sending a discovery message to the domain's PSS)"<< endl;

											if (PCore->PSSTuples.size() > 0)
												{
													// ***************************************************
													// The discovery is restricted to the domain
													// ***************************************************

													// Setting up the OSID as the space limiter
													Limiters.push_back(PB->PP->GetDomainSelfCertifyingName());

													// Setting up the this OS as the 1st source SCN
													Sources.push_back(PB->PP->GetHostSelfCertifyingName());

													// Setting up the this OS as the 2nd source SCN
													Sources.push_back(PB->PP->GetOperatingSystemSelfCertifyingName());

													// Setting up the this process as the 3rd source SCN
													Sources.push_back(PB->PP->GetSelfCertifyingName());

													// Setting up the PS block SCN as the 4th source SCN
													Sources.push_back(PB->GetSelfCertifyingName());

													// Setting up the destination 1st source
													Destinations.push_back(PCore->PSSTuples[0]->Values[0]);

													// Setting up the destination 2nd source
													Destinations.push_back(PCore->PSSTuples[0]->Values[1]);

													// Setting up the destination 3rd source
													Destinations.push_back(PCore->PSSTuples[0]->Values[2]);

													// Setting up the destination 4th source
													Destinations.push_back(PCore->PSSTuples[0]->Values[3]);

													// Creating a new message
													PB->PP->NewMessage(GetTime(),1,false,Discovery);

													// Creating the ng -cl -msg command line
													PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,Discovery,PCL);

													// ***************************************************
													// Generate the get bindings to be ask on PGS
													// ***************************************************

													//PB->S << Offset << "(NA = " << NA << ")"<< endl;

													for (unsigned int j=1; j<NA; j++)
														{
															// Get received command line arguments
															_PCL->GetArgument(j,Pair);

															// Pairs=_PCL->Arguments.at(1);
															if (Pair.size() > 0)
																{
																	//PB->S << Offset << "(Category = "<<Pair.at(0)<<")"<< endl;
																	//PB->S << Offset << "(Key = "<<Pair.at(1)<<")"<< endl;

																	//PB->S << Offset <<"(Cheching if this binding already exists on the discovery message)"<< endl;

																	Discovery->DoAllTheseValuesExistInSomeCommandLine(&Pair,Veredict);

																	if (Veredict == false)
																		{
																			// Set the key
																			Keys->push_back(Pair.at(1));

																			// Generate the ng -sub --bind
																			PMB->NewSubscriptionCommandLine("0.1",PB->StringToInt(Pair.at(0)),Keys,Discovery,PCL);

																			//PB->S << Offset <<"(The binding does not exists)"<< endl;

																			Keys->clear();
																		}
																	else
																		{
																			//PB->S << Offset <<"(The binding already exists. Skipping it)"<< endl;
																		}
																}

															Pair.clear();
														}

													// ***************************************************
													// Generate the ng -message --type [ < 1 string 1 > ]
													// ***************************************************

													Discovery->NewCommandLine("-message","--type","0.1",PCL);

													PCL->NewArgument(1);

													PCL->SetArgumentElement(0,0,PB->IntToString(Discovery->GetType()));

													// ***************************************************
													// Generate the ng -message --seq [ < 1 string 1 > ]
													// ***************************************************

													Discovery->NewCommandLine("-message","--seq","0.1",PCL);

													PCL->NewArgument(1);

													PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

													// Generate the SCN
													PB->GenerateSCNFromMessageBinaryPatterns16Bytes(Discovery,SCN);

													// Creating the ng -scn --s command line
													PMB->NewSCNCommandLine("0.1",SCN,Discovery,PCL);

													// ******************************************************
													// Finish
													// ******************************************************

													// Push the message to the GW input queue
													PCore->PGW->PushToInputQueue(Discovery);

													if (ScheduledMessages.size() > 0)
														{
															Message *Temp=ScheduledMessages.at(0);

															Temp->Delete=true;

															// Make the scheduled messages vector empty
															ScheduledMessages.clear();
														}

													Status=OK;
												}
											else
												{
													PB->S << Offset <<  "(Warning: Unable to send the discovery message. The domain PSS is still unkwon)" << endl;
												}
										}
								}
							else
								{
									PB->S << Offset <<  "(ERROR: One or more argument is empty)" << endl;
								}
				}
			else
				{
					PB->S << Offset <<  "(ERROR: Wrong number of arguments)" << endl;
				}
		}
	else
		{
			PB->S << Offset <<  "(ERROR: Unable to read the number of arguments)" << endl;
		}

	delete PGSBIDs;
	delete Keys;

	//PB->S << Offset <<  "(Done)" << endl << endl << endl; 

	return Status;
}

