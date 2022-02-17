/*
	NovaGenesis

	Name:		CoreStatusS01
	Object:		CoreStatusS01
	File:		CoreStatusS01.cpp
	Author:		Antonio M. Alberti
	Date:		11/2012
	Version:	0.1
*/

#ifndef _CORESTATUS01_H
#include "CoreStatusS01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _TUPLE_H
#include "Tuple.h"
#endif

#ifndef _APP_H
#include "App.h"
#endif

CoreStatusS01::CoreStatusS01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CoreStatusS01::~CoreStatusS01()
{
}

// Run the actions behind a received command line
// ng -status --s _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > ]
int CoreStatusS01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 			Status=ERROR;
	string			Offset = "                    ";
	unsigned int 	NA=0;
	vector<string>	RelatedCommand;
	vector<string>	StatusCode;
	vector<string>	Limiters;
	vector<string>	Sources;
	vector<string>	Destinations;
	Message			*RunEvaluate=0;
	CommandLine		*PCL=0;
	Core			*PCore=0;

	PCore=(Core*)PB;

	PB->S << Offset <<  this->GetLegibleName() << endl;

	// Load the number of arguments
	if (_PCL->GetNumberofArguments(NA) == OK)
		{
			// Check the number of arguments
			if (NA == 2)
				{
					// Get received command line arguments
					if (_PCL->GetArgument(0,RelatedCommand) == OK && _PCL->GetArgument(1,StatusCode) == OK)
						{
							if (RelatedCommand.size() > 0 && StatusCode.size() > 0)
								{
									if (RelatedCommand.at(0) == "-store" && RelatedCommand.at(1) == "--bind" && StatusCode.at(0) == "200" && PCore->GenerateRunX01 == true)
										{
											if (ScheduledMessages.size() > 0)
												{
													//PB->S << Offset <<  "(There is a scheduled message)" << endl;

													RunEvaluate=ScheduledMessages.at(0);

													if (RunEvaluate != 0)
														{
															// Adding a ng -run --evaluation command line
															RunEvaluate->NewCommandLine("-run","--evaluate","0.1",PCL);

															Status=OK;
														}
												}

											PCore->GenerateRunX01 = false;
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

	return Status;
}
