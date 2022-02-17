/*
	NovaGenesis

	Name:		CoreNotifyS01
	Object:		CoreNotifyS01
	File:		CoreNotifyS01.cpp
	Author:		Antonio M. Alberti
	Date:		11/2012
	Version:	0.1
*/

#ifndef _CORENOTIFYS01_H
#include "CoreNotifyS01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

CoreNotifyS01::CoreNotifyS01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CoreNotifyS01::~CoreNotifyS01()
{
}

/*
 *
 * ng -notify --s 0.1 [ < 1 string 18 > < 1 string 2BF32B9052D5480AD7858C6EB835B1FC > ]
 *
 */

// Run the actions behind a received command line
// ng -notify --s _Version [ < 1 string _Category > < _KeysSize string S_1 ... S_KeysSize > < _TupleSize string T_1 ... T_TupleSize > ]
int CoreNotifyS01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 			Status=ERROR;
	string			Offset = "                    ";
	unsigned int 	NA=0;
	vector<string>	Limiters;
	vector<string>	Sources;
	vector<string>	Destinations;
	Message			*RunEvaluation=0;
	CommandLine		*PCL=0;
	Core			*PCore=0;
	vector<string>	Arg0;
	vector<string>	Arg1;
	vector<string>	Arg2;

	PCore=(Core*)PB;

	PB->S << Offset <<  this->GetLegibleName() << endl;

	// Load the number of arguments
	if (_PCL->GetNumberofArguments(NA) == OK)
		{
			// Check the number of arguments
			if (NA == 3)
				{
					_PCL->GetArgument(0,Arg0);	// Category
					_PCL->GetArgument(1,Arg1);	// Keys
					_PCL->GetArgument(2,Arg2);	// Publisher's tuple

					// Allocates a new subscription
					Subscription	*PS;

					PCore->NewSubscription(PS);

					if (Arg0.size() > 0 && Arg1.size() > 0 && Arg2.size() > 0)
						{
							PS->Category=PB->StringToInt(Arg0.at(0));

							//PB->S << Offset <<  "(Notification category = "<<PS->Category<<")" << endl;

							// Copy the key to the Subscription container
							PS->Key=Arg1.at(0);

							//PB->S << Offset <<  "(Notification key = "<<PS->Key<<")" << endl;

							// Copy the publisher's tuple to the Subscription container
							for (unsigned int j=0; j<Arg2.size(); j++)
								{
									PS->Publisher.Values.push_back(Arg2.at(j));

									//PB->S << Offset <<  "(Notification publisher ID ["<<j<<"] = "<< PS->Publisher.Values[j] <<")" << endl;
								}

							// Set the status to schedule a ng -run --subscribe
							PS->Status="Scheduling required";

							PCore->Debug.OpenOutputFile();

							PCore->Debug << endl << "Received a notification of the key " << PS->Key << ". Creating the subscrition of index " << (PCore->Subscriptions.size()-1) << ". Status of the subscription is " <<PS->Status<< endl;

							PCore->Debug.CloseFile();

							PS->HasContent=false;

							if (ScheduledMessages.size() > 0)
								{
									//PB->S << Offset <<  "(There is a scheduled message with command line ng -run --evaluate)" << endl;

									RunEvaluation=ScheduledMessages.at(0);

									if (RunEvaluation != 0)
										{
											// Adding a ng -run --evaluation command line
											RunEvaluation->NewCommandLine("-run","--evaluate","0.1",PCL);

											Status=OK;
										}

									PCore->GenerateRunX01 = false;
								}
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

	return Status;
}

