/*
	NovaGenesis

	Name:		CoreSCNSeq01
	Object:		CoreSCNSeq01
	File:		CoreSCNSeq01.cpp
	Author:		Antonio M. Alberti
	Date:		11/2012
	Version:	0.1
*/

#ifndef _CORESCNSEQ01_H
#include "CoreSCNSeq01.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

CoreSCNSeq01::CoreSCNSeq01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CoreSCNSeq01::~CoreSCNSeq01()
{
}

// Run the actions behind a received command line
// ng -scn --seq 0.1 [ < 1 string SCN > ]
int CoreSCNSeq01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 			Status=OK;
	CommandLine		*PCL=0;
	string			ReceivedSCN;
	string			NewSCN;
	string			Offset = "                    ";
	Message			*Run=0;
	Core			*PCore=0;
	unsigned int	NoCL=0;

	PCore=(Core*)PB;

	PB->S << Offset <<  this->GetLegibleName() << endl;

	// ************************************************************************
	// Generate the ng -scn -seq if required for the ng -run --evaluation case
	// ************************************************************************

	if (ScheduledMessages.size() > 0)
		{
			//PB->S << Offset <<  "(There is a scheduled message)" << endl;

			Run=ScheduledMessages.at(0);

			if (Run != 0)
				{
					Run->GetNumberofCommandLines(NoCL);

					if (NoCL > 1)
						{
							// ***************************************************
							// Generate the ng -message --type [ < 1 string 1 > ]
							// ***************************************************

							Run->NewCommandLine("-message","--type","0.1",PCL);

							PCL->NewArgument(1);

							PCL->SetArgumentElement(0,0,PB->IntToString(Run->GetType()));

							// ***************************************************
							// Generate the ng -message --seq [ < 1 string 1 > ]
							// ***************************************************

							Run->NewCommandLine("-message","--seq","0.1",PCL);

							PCL->NewArgument(1);

							PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

							// ***************************************************
							// Generate the ng -scn --seq
							// ***************************************************
							PB->GenerateSCNFromMessageBinaryPatterns16Bytes(Run,SCN);

							// Creating the ng -scn --s command line
							PMB->NewSCNCommandLine("0.1",SCN,Run,PCL);

							//PB->S << "(" << endl << *Run << ")"<< endl;

							// Push the message to the GW input queue
							PCore->PGW->PushToInputQueue(Run);

							Status=OK;
						}
				}
		}

	//PB->S << Offset <<  "(Done)" << endl << endl << endl; 

	return Status;
}
