/*
	NovaGenesis

	Name:		CoreRunSubscribe01
	Object:		CoreRunSubscribe01
	File:		CoreRunSubscribe01.cpp
	Author:		Antonio M. Alberti
	Date:		12/2012
	Version:	0.1
*/

#ifndef _CORERUNSUBSCRIBE01_H
#include "CoreRunSubscribe01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

CoreRunSubscribe01::CoreRunSubscribe01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CoreRunSubscribe01::~CoreRunSubscribe01()
{
}

// Run the actions behind a received command line
// ng -run --subscribe 0.1 [ < 1 string Category > < N string Key1 .. KeyN > ]
int CoreRunSubscribe01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 			Status=OK;
	string			Offset = "                    ";
	unsigned int 	NA=0;
	vector<string>	Category;
	vector<string>	Keys;
	vector<string>	Limiters;
	vector<string>	Sources;
	vector<string>	Destinations;
	Core			*PCore=0;
	Message 		*Subcription=0;
	CommandLine		*PCL=0;
	vector<string>	*Values=new vector<string>;

	PCore=(Core*)PB;

	PB->S << Offset <<  this->GetLegibleName() << endl;

	// Load the number of arguments
	if (_PCL->GetNumberofArguments(NA) == OK)
		{
			// Check the number of arguments
			if (NA == 2)
				{
					// Get received command line arguments
					_PCL->GetArgument(0,Category);
					_PCL->GetArgument(1,Keys);

					// Pairs=_PCL->Arguments.at(1);
							if (Category.size() > 0 && Keys.size())
								{
									PB->S << Offset <<  "(Generating a message to subscribe some keys at domain level)"<<endl;

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

									// Setting up the destination
									Destinations.push_back(PCore->PSSTuples[0]->Values[0]);

									// Setting up the destination 2nd source
									Destinations.push_back(PCore->PSSTuples[0]->Values[1]);

									// Setting up the destination 3rd source
									Destinations.push_back(PCore->PSSTuples[0]->Values[2]);

									// Setting up the destination 4th source
									Destinations.push_back(PCore->PSSTuples[0]->Values[3]);

									// Creating a new message
									PB->PP->NewMessage(GetTime(),1,false,Subcription);

									// Creating the ng -cl -msg command line
									PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,Subcription,PCL);

									// ***************************************************
									// Generate the ng -sub -bind
									// ***************************************************

									// Subscribe the key on the Category 18
									PMB->NewSubscriptionCommandLine("0.1",PB->StringToInt(Category.at(0)),&Keys,Subcription,PCL);

									// Subscribe the key on the Category 2
									PMB->NewSubscriptionCommandLine("0.1",2,&Keys,Subcription,PCL);

									// Subscribe the key on the Category 9
									PMB->NewSubscriptionCommandLine("0.1",9,&Keys,Subcription,PCL);

									// ***************************************************
									// Generate the ng -message --type [ < 1 string 1 > ]
									// ***************************************************

									Subcription->NewCommandLine("-message","--type","0.1",PCL);

									PCL->NewArgument(1);

									PCL->SetArgumentElement(0,0,PB->IntToString(Subcription->GetType()));

									// ***************************************************
									// Generate the ng -message --seq [ < 1 string 1 > ]
									// ***************************************************

									Subcription->NewCommandLine("-message","--seq","0.1",PCL);

									PCL->NewArgument(1);

									PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

									// ******************************************************
									// Generate the SCN
									// ******************************************************

									PB->GenerateSCNFromMessageBinaryPatterns16Bytes(Subcription,SCN);

									// Creating the ng -scn --s command line
									PMB->NewSCNCommandLine("0.1",SCN,Subcription,PCL);

									PB->S << Offset <<"(The following message constains a subscription to the peer)"<< endl;

									PB->S << "(" << endl << *Subcription << ")"<< endl;

									// ******************************************************
									// Finish
									// ******************************************************

									// Push the message to the GW input queue
									PCore->PGW->PushToInputQueue(Subcription);

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

	delete Values;

	//PB->S << Offset <<  "(Done)" << endl << endl << endl; 

	return Status;
}

