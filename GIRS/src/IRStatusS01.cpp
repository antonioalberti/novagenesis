/*
	NovaGenesis

	Name:		IRStatusS01
	Object:		IRStatusS01
	File:		IRStatusS01.cpp
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

#ifndef _IRSTATUSS01_H
#include "IRStatusS01.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

IRStatusS01::IRStatusS01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRStatusS01::~IRStatusS01 ()
{
}

// Run the actions behind a received command line
// ng -st --s _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > ]
int
IRStatusS01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;

  /*
  unsigned int 	NA=0;
  vector<string>	RelatedCommand;
  vector<string>	StatusCode;
  Message 		*ExposingInitialBinds=0;
  vector<string>	Limiters;
  vector<string>	Sources;
  vector<string>	Destinations;
  Block			*PHTB=0;
  //Block			*PGWB=0;
  CommandLine		*PCL=0;
  IR				*PIR=0;
  vector<string>	GetBind01MsgLimiters;
  vector<string>	GetBind01MsgSources;
  vector<string>	GetBind01MsgDestinations;
  string 			Version="0.1";
  string 			Key="";
  string			PGCSPID;
  vector<string> 	*PGCSBIDs=new vector<string>;
  string			Offset = "                    ";
  Message 		*Discovery=0;
  Message 		*Publish=0;
  vector<string>	Values;
  string			HashProcessLegibleName;
  Message			*IPCUpdate=NULL;

  PIR=(IR*)PB;

  PHTB=(Block*)PIR->PHT;

  //PGWB=(Block*)PIR->PGW;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments(NA) == OK)
	  {
		  // Check the number of argument
		  if (NA == 2)
			  {
				  // Get received command line arguments
				  if (_PCL->GetArgument(0,RelatedCommand) == OK && _PCL->GetArgument(1,StatusCode) == OK)
					  {
						  if (RelatedCommand.size() > 0 && StatusCode.size() > 0)
							  {
								  if (RelatedCommand.at(0) == "-sr" && RelatedCommand.at(1) == "--b" && StatusCode.at(0) == "0" && PB->State == "initialization")
									  {
										  if (PIR->ChangeState == true)
											  {
												  // Change the state from initialization to exposition
												  PB->State = "exposition";

												  PB->S << endl << Offset <<  "(Moving to exposition state --------------------------------------------------------------------------------------------------)"<<endl << endl;

												  Message *PListBindingsMessage=0;
												  CommandLine *PListBindingsCL;

												  // Creating a -run --initialization message
												  PB->PP->NewMessage(GetTime(),0,false,PListBindingsMessage);

												  // Adding only the list bindings command line
												  PListBindingsMessage->NewCommandLine("-list","--b","0.1",PListBindingsCL);

												  // Execute the procedure
												  PIR->PHT->Run(PListBindingsMessage,InlineResponseMessage);

												  // Tag to delete the temporary message
												  PListBindingsMessage->MarkToDelete();

												  PIR->ChangeState=false;

												  // ***************************************************
												  // Get the PGCS::HT BID from the PGCS LN
												  // ***************************************************

												  PB->PP->DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS","HT",PGCSPID,PGCSBIDs,PB);

												  if (PGCSBIDs->size() > 0)
													  {
														  // ***************************************************
														  // Setup the PGCS::HT table as the destination
														  // ***************************************************

														  // Setting up the OSID as the space limiter
														  Limiters.push_back(PB->PP->Intra_OS);

														  // Setting up the this process as the first source SCN
														  Sources.push_back(PB->PP->GetSelfCertifyingName());

														  // Setting up the IR block SCN as the source SCN
														  Sources.push_back(PB->GetSelfCertifyingName());

														  // Setting up the PGCS PID as the destination SCN
														  Destinations.push_back(PGCSPID);

														  // Setting up the PGCS::HT BID as the destination SCN
														  Destinations.push_back(PGCSBIDs->at(0));

														  // Creating a new message
														  PB->PP->NewMessage(GetTime()+PIR->DelayBeforeRunExposition,0,false,ExposingInitialBinds);

														  // Creating the ng -cl -m command line
														  PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,ExposingInitialBinds,PCL);

														  // ***************************************************
														  // Generate the bindings to be store on PGCS
														  // ***************************************************

														  PMB->NewStoreBindingCommandLineFromOSIDToPID("0.1",ExposingInitialBinds,PCL);

														  PMB->NewStoreBindingCommandLineFromPIDToBID("0.1",PB,ExposingInitialBinds,PCL);

														  PMB->NewStoreBindingCommandLineFromPIDToBID("0.1",PHTB,ExposingInitialBinds,PCL);

														  //PMB->NewStoreBindingCommandLineFromPIDToBID("0.1",PGWB,ExposingInitialBinds,PCL);

														  PMB->NewStoreBindingCommandLineFromHashLNToSCN("0.1",2,"IR",PB->GetSelfCertifyingName(),ExposingInitialBinds,PCL);

														  PMB->NewStoreBindingCommandLineSCNToHashLN("0.1",3,PB->GetSelfCertifyingName(),"IR",ExposingInitialBinds,PCL);

														  // Generate the SCN
														  PB->GenerateSCNFromMessageBinaryPatterns(ExposingInitialBinds,SCN);

														  // Creating the ng -scn --s command line
														  PMB->NewSCNCommandLine("0.1",SCN,ExposingInitialBinds,PCL);

														  // ******************************************************
														  // Finish
														  // ******************************************************

														  // Push the message to the GW input queue
														  PIR->PGW->PushToInputQueue(ExposingInitialBinds);

														  Status=OK;

														  //PB->S << Offset <<  "(Done)" << endl << endl << endl;
													  }
												  else
													  {
														  PB->S << Offset <<  "(ERROR: Failed to discover the PGS_PID and its HT_BID)" << endl;
													  }
											  }
									  }
								  else if (RelatedCommand.at(0) == "-sr" && RelatedCommand.at(1) == "--b" && StatusCode.at(0) == "0" && PB->State == "exposition")
									  {
										  // Change the state from exposition to discovery
										  if (PIR->ChangeState == true)
											  {
												  // Change the state from initialization to exposition
												  PB->State = "discovery";

												  PB->S << endl << Offset <<  "(Moving to discovery state --------------------------------------------------------------------------------------------------)"<<endl << endl;

												  PIR->ChangeState=false;

												  // ***************************************************
												  // Get the PGCS::HT BID from the PGCS LN
												  // ***************************************************

												  PB->PP->DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS","HT",PGCSPID,PGCSBIDs,PB);

												  if (PGCSBIDs->size() > 0)
													  {
														  // ***************************************************
														  // Set up the keywords for the discovery state
														  // ***************************************************

														  string LN1="Host";
														  string LN2="OS";
														  string LN3="PSS";
														  string LN4="PS";
														  string LN5="HTS";
														  string LN6="DHT";

														  string Key1;
														  string Key2;
														  string Key3;
														  string Key4;
														  string Key5;
														  string Key6;

														  PMB->GenerateSCNFromCharArrayBinaryPatterns(LN1,Key1);
														  PMB->GenerateSCNFromCharArrayBinaryPatterns(LN2,Key2);
														  PMB->GenerateSCNFromCharArrayBinaryPatterns(LN3,Key3);
														  PMB->GenerateSCNFromCharArrayBinaryPatterns(LN4,Key4);
														  PMB->GenerateSCNFromCharArrayBinaryPatterns(LN5,Key5);
														  PMB->GenerateSCNFromCharArrayBinaryPatterns(LN6,Key6);

														  // ***************************************************
														  // Prepare the first command line
														  // ***************************************************

														  // Setting up the OSID as the space limiter
														  Limiters.push_back(PB->PP->Intra_OS);

														  // Setting up the this process as the first source SCN
														  Sources.push_back(PB->PP->GetSelfCertifyingName());

														  // Setting up the PS block SCN as the source SCN
														  Sources.push_back(PB->GetSelfCertifyingName());

														  // Setting up the PGCS PID as the destination SCN
														  Destinations.push_back(PGCSPID);

														  // Setting up the PGCS::HT BID as the destination SCN
														  Destinations.push_back(PGCSBIDs->at(0));

														  // Creating a new message
														  PB->PP->NewMessage(GetTime()+PIR->DelayBeforeDHTPSSDiscovery,0,false,Discovery);

														  // Creating the ng -cl -m command line
														  PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,Discovery,PCL);

														  // ***************************************************
														  // Generate the get bindings to be ask on PGCS
														  // ***************************************************
														  PMB->NewGetCommandLine("0.1",9,Key1,Discovery,PCL);
														  PMB->NewGetCommandLine("0.1",2,Key2,Discovery,PCL);
														  PMB->NewGetCommandLine("0.1",2,Key3,Discovery,PCL);
														  PMB->NewGetCommandLine("0.1",2,Key4,Discovery,PCL);
														  PMB->NewGetCommandLine("0.1",2,Key5,Discovery,PCL);
														  PMB->NewGetCommandLine("0.1",2,Key6,Discovery,PCL);

														  // ******************************************************
														  // Setting up the SCN command line
														  // ******************************************************

														  // Generate the SCN
														  PB->GenerateSCNFromMessageBinaryPatterns(Discovery,SCN);

														  // Creating the ng -scn --s command line
														  PMB->NewSCNCommandLine("0.1",SCN,Discovery,PCL);

														  // ******************************************************
														  // Finish
														  // ******************************************************

														  // Push the message to the GW input queue
														  PIR->PGW->PushToInputQueue(Discovery);


														  // Disable generating the -scn --seq header
														  PIR->GenerateSCNSeqForGetBind = false;

														  PB->S << Offset <<"(Sending a discovery message to the PGCS process on the same operating system)"<< endl;
													  }
												  else
													  {
														  PB->S << Offset <<  "(ERROR: Failed to discover the PGS_PID and its HT_BID)" << endl;
													  }
											  }
									  }
								  else if (RelatedCommand.at(0) == "-sr" && RelatedCommand.at(1) == "--b" && StatusCode.at(0) == "0" && PB->State == "discovery")
									  {
										  // Change the state from exposition to discovery
										  if (PIR->ChangeState == true)
											  {
												  bool GoToOperational=true;

												  PB->S << Offset <<  "(Discovering the existent NRNCS(s) and HTS(s))"<<endl;

												  if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("PSS","PS",PIR->PSTuples,PB) == OK)
													  {
														  for (unsigned int i=0; i<PIR->PSTuples.size(); i++)
															  {
																  PB->S << Offset << "(Tuple = " << endl;

																  PB->S << Offset << "(HID = " << PIR->PSTuples[i]->Values[0] << ")" << endl;

																  PB->S << Offset << "(OSID = " << PIR->PSTuples[i]->Values[1] << ")" << endl;

																  PB->S << Offset << "(PID = " << PIR->PSTuples[i]->Values[2] << ")" << endl;

																  PB->S << Offset << "(BID = " << PIR->PSTuples[i]->Values[3] << ")" << endl;
															  }

														  // ******************************************************************
														  // Reset gateway statistics
														  // ******************************************************************

														  PIR->PGW->ResetStatistics();
													  }
												  else
													  {
														  GoToOperational=false;

														  PB->S << Offset <<  "(Warning: Empty NRNCS tuples vector)" << endl;
													  }

												  if (PB->PP->DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames("HTS","DHT",PIR->HTSTuples,PB) == OK)
													  {
														  for (unsigned int i=0; i<PIR->HTSTuples.size(); i++)
															  {
																  PB->S << Offset << "(Tuple = " << endl;

																  PB->S << Offset << "(HID = " << PIR->HTSTuples[i]->Values[0] << ")" << endl;

																  PB->S << Offset << "(OSID = " << PIR->HTSTuples[i]->Values[1] << ")" << endl;

																  PB->S << Offset << "(PID = " << PIR->HTSTuples[i]->Values[2] << ")" << endl;

																  PB->S << Offset << "(BID = " << PIR->HTSTuples[i]->Values[3] << ")" << endl;
															  }
													  }
												  else
													  {
														  GoToOperational=false;

														  PB->S << Offset <<  "(Warning: Empty HTS tuples vector)" << endl;
													  }

												  if (GoToOperational == true)
													  {
														  // Change the state from initialization to exposition
														  PB->State = "operational";

														  PB->S << endl << Offset <<  "(Moving to operational state --------------------------------------------------------------------------------------------------)"<<endl << endl;

														  PIR->ChangeState=false;

														  //PB->S << Offset <<  "(Generating a message to publish the binding <hash(\"GIRS\"), GIRS PID> )"<<endl;

														  PB->S << Offset <<  "(Everything ok!)"<<endl;

														  // Setting up the OSID as the space limiter
														  Limiters.push_back(PB->PP->Intra_Domain);

														  // Setting up the this OS as the 1st source SCN
														  Sources.push_back(PB->PP->GetHostSelfCertifyingName());

														  // Setting up the this OS as the 2nd source SCN
														  Sources.push_back(PB->PP->GetOperatingSystemSelfCertifyingName());

														  // Setting up the this process as the 3rd source SCN
														  Sources.push_back(PB->PP->GetSelfCertifyingName());

														  // Setting up the PS block SCN as the 4th source SCN
														  Sources.push_back(PB->GetSelfCertifyingName());

														  // Setting up the this OS as the 1st source SCN
														  Destinations.push_back(PIR->PSTuples[0]->Values[0]);

														  // Setting up the this OS as the 2nd source SCN
														  Destinations.push_back(PIR->PSTuples[0]->Values[1]);

														  // Setting up the this process as the 3rd source SCN
														  Destinations.push_back(PIR->PSTuples[0]->Values[2]);

														  // Setting up the PS block SCN as the 4th source SCN
														  Destinations.push_back(PIR->PSTuples[0]->Values[3]);

														  // Creating a new message
														  PB->PP->NewMessage(GetTime()+PIR->DelayBeforePIDPublishing,0,false,Publish);

														  // Creating the ng -cl -m command line
														  PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,Publish,PCL);

														  // ***************************************************
														  // Generate the bindings to be published
														  // ***************************************************
														  Values.push_back(PB->PP->GetSelfCertifyingName());

														  PMB->GenerateSCNFromCharArrayBinaryPatterns("GIRS",HashProcessLegibleName);

														  PMB->NewCommonCommandLine("-p","--b","0.1",2,HashProcessLegibleName,&Values,Publish,PCL);

														  // Generate the SCN
														  PB->GenerateSCNFromMessageBinaryPatterns(Publish,SCN);

														  // Creating the ng -scn --s command line
														  PMB->NewSCNCommandLine("0.1",SCN,Publish,PCL);

														  // ******************************************************
														  // Finish
														  // ******************************************************

														  // Push the message to the GW input queue
														  PIR->PGW->PushToInputQueue(Publish);

														  Limiters.clear();
														  Sources.clear();
														  Destinations.clear();

														  // ***************************************************************************************
														  // Creating a message to discover the PGCS aiming to discover the shared memory of peers
														  // ***************************************************************************************

														  PB->PP->DiscoverHomonymsBlocksBIDsFromProcessLegibleName("PGCS","HT",PGCSPID,PGCSBIDs,PB);

														  if (PGCSBIDs->size() > 0)
															  {
																  // ***************************************************
																  // Setup the PGCS::HT table as the destination
																  // ***************************************************

																  // Setting up the OSID as the space limiter
																  Limiters.push_back(PB->PP->Intra_OS);

																  // Setting up the this process as the first source SCN
																  Sources.push_back(PB->PP->GetSelfCertifyingName());

																  // Setting up the IR block SCN as the source SCN
																  Sources.push_back(PB->GetSelfCertifyingName());

																  // Setting up the PGCS PID as the destination SCN
																  Destinations.push_back(PGCSPID);

																  // Setting up the PGCS::HT BID as the destination SCN
																  Destinations.push_back(PGCSBIDs->at(0));

																  // Creating a new message
																  PB->PP->NewMessage(GetTime(),0,false,IPCUpdate);

																  // Creating the ng -cl -m command line
																  PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,IPCUpdate,PCL);

																  // ***************************************************
																  // Generate the get to discover peer SHM Key on PGCS
																  // ***************************************************

																  PMB->NewGetCommandLine("0.1",13,PIR->PSTuples[0]->Values[2],IPCUpdate,PCL);

																  PMB->NewGetCommandLine("0.1",13,PIR->HTSTuples[0]->Values[2],IPCUpdate,PCL);

																  // Generate the SCN
																  PB->GenerateSCNFromMessageBinaryPatterns(IPCUpdate,SCN);

																  // Creating the ng -scn --s command line
																  PMB->NewSCNCommandLine("0.1",SCN,IPCUpdate,PCL);

																  // ******************************************************
																  // Finish
																  // ******************************************************

																  // Push the message to the GW input queue
																  PIR->PGW->PushToInputQueue(IPCUpdate);

																  Status=OK;

																  //PB->S << Offset <<  "(Done)" << endl << endl << endl;
															  }
														  else
															  {
																  PB->S << Offset <<  "(ERROR: Failed to discover the PGS_PID and its HT_BID)" << endl;
															  }

														  Status=OK;
													  }
											  }
									  }
								  else if (RelatedCommand.at(0) == "-sr" && RelatedCommand.at(1) == "--b" && StatusCode.at(0) == "0" && PB->State == "operational")
									  {
										  //PB->S << Offset <<  "(There is no need to provide an inline message)" << endl;

										  // Stop processing the other command lines of the message.
										  PB->StopProcessingMessage=true;
									  }
							  }
						  else
							  {
								  PB->S << Offset <<  "(ERROR: One or more argument is empty)" << endl;
							  }
					  }
				  else
					  {
						  PB->S << Offset <<  "(ERROR: Unable to read the arguments)" << endl;
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

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  delete PGCSBIDs;

  */

  return Status;
}


