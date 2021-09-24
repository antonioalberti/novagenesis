/*
	NovaGenesis

	Name:		CLISCNAck01
	Object:		CLISCNAck01
	File:		CLISCNAck01.cpp
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

#ifndef _CLISCNACK01_H
#include "CLISCNAck01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

CLISCNAck01::CLISCNAck01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CLISCNAck01::~CLISCNAck01()
{
}

// Run the actions behind a received command line
// ng -scn --ack 0.1 SCN AckSCN
int CLISCNAck01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 			Status=ERROR;
	CommandLine		*PStoredCL=0;
	unsigned int 	NReceivedA=0;
	string			ReceivedSCN;
	vector<string>	PReceivedA;
	vector<string>	PStoredA;
	string			AckSCN;
	string			Offset = "                    ";
	Message 		*PM=0;								// Pointer to the message being analyzed
	unsigned int 	NCL=0;								// Number of command lines
	unsigned int 	NStoredA=0;							// Number of arguments

	//PB->S << Offset <<  this->GetLegibleName() << endl;

	// Load the number of arguments
	if (_PCL->GetNumberofArguments(NReceivedA) == OK)
		{
			// Check the number of arguments
			if (NReceivedA == 1)
				{
					// Get the fist argument
					_PCL->GetArgument(0,PReceivedA);

					// Get received message SCN
					ReceivedSCN=PReceivedA.at(0);

					// Get ack message SCN
					AckSCN=PReceivedA.at(1);

					PB->S << Offset <<"(AckSCN = "<< AckSCN << ")" << endl;

					if (ReceivedSCN != "" && AckSCN != "")
						{
							for (unsigned int i=0; i<PB->PP->GetNumberOfMessages(); i++)
								{
									if (PB->PP->GetMessage(i,PM) == OK)
										{
											if (PM->GetNumberofCommandLines(NCL) == OK)
												{
													if (NCL > 0)
														{
															PB->S << endl << Offset <<"(NCL = "<<NCL<<")"<< endl;

															for (unsigned int j=0; j<NCL; j++)
																{
																	if (PM->GetCommandLine(j,PStoredCL) == OK)
																		{
																			if (PStoredCL != 0)
																				{
																					// Check the command name
																					if (PStoredCL->Name == "-scn" && PStoredCL->Alternative == "--seq")
																						{
																							// Load the number of arguments
																							if (PStoredCL->GetNumberofArguments(NStoredA) == OK)
																								{
																									// Check the number of argument
																									if (NStoredA == 1)
																										{
																											// Check the first argument
																											if (PStoredCL->GetArgument(0,PStoredA) == OK)
																												{
																													if (PStoredA.at(0) == AckSCN)
																														{
																															PB->S << Offset <<"(Messages container size = "<<PB->PP->GetNumberOfMessages()<<")"<< endl;

																															//PB->S << Offset <<"(Match with the message of index "<<i<<")"<< endl;

																															//PB->S << "(" << endl << *PM << ")"<< endl;

																															break;
																														}
																												}
																										}
																								}
																						}
																				}
																		}

																	PStoredA.clear();
																}
														}
												}
										}
								}
						}
					else
						{
							PB->S << Offset <<  "(ERROR: Unable to read the arguments)" << endl;

							Status=ERROR;
						}
				}
			else
				{
					PB->S << Offset <<  "(ERROR: Wrong number of arguments)" << endl;

					Status=ERROR;
				}
		}
	else
		{
			PB->S << Offset <<  "(ERROR: Unable to read the number of arguments)" << endl;
		}

	//PB->S << Offset <<  "(Done)" << endl << endl << endl; 

	return Status;
}
