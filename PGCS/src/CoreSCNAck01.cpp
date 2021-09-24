/*
	NovaGenesis

	Name:		CoreSCNAck01
	Object:		CoreSCNAck01
	File:		CoreSCNAck01.cpp
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

#ifndef _CORESCNACK01_H
#include "CoreSCNAck01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

CoreSCNAck01::CoreSCNAck01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

CoreSCNAck01::~CoreSCNAck01 ()
{
}

// Run the actions behind a received command line
// ng -scn --ack 0.1 [ < 2 string SCN AckSCN > ]
int
CoreSCNAck01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  CommandLine *PStoredCL = 0;
  unsigned int NA = 0;
  string ReceivedSCN;
  vector<string> PReceivedA;
  vector<string> PStoredA;
  string AckSCN;
  string Offset = "                    ";
  Message *PM = 0;                                // Pointer to the message being analyzed
  unsigned int NCL = 0;                                // Number of command lines
  unsigned int NStoredA = 0;                            // Number of arguments
  Core *PCore = 0;
  Message *Run = 0;
  CommandLine *PCL = 0;
  Publication *PP = 0;

  PCore = (Core *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // ************************************************************************
  // Sample the roud trip time to publish on NRNCS
  // ************************************************************************

  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA == 1)
		{
		  // Get the fist argument
		  _PCL->GetArgument (0, PReceivedA);

		  // Get received message SCN
		  ReceivedSCN = PReceivedA.at (0);

		  // Get ack message SCN
		  AckSCN = PReceivedA.at (1);

		  //PB->S << Offset <<"(AckSCN = "<< AckSCN << ")"<< endl;

		  if (ReceivedSCN != "" && AckSCN != "")
			{
			  if (PCore->GetPublication (AckSCN, PP) == OK)
				{
				  if (PP != 0)
					{
					  double Now = GetTime ();

					  double DeltaT = Now - PP->Timestamp;

					  PB->S << Offset << setprecision (10) << "(The publication round trip time to NRNCS was " << DeltaT
							<< ")" << endl;

					  PCore->pubrtt->Sample (DeltaT);

					  PCore->pubrtt->CalculateArithmetic ();

					  PCore->pubrtt->SampleToFile (Now);
					}
				  else
					{
					  PB->S << Offset << "(ERROR: Null publication object)" << endl;

					  Status = ERROR;
					}
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Unable to read the arguments)" << endl;

			  Status = ERROR;
			}
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;

		  Status = ERROR;
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
	}

  // ************************************************************************
  // Generate the ng -scn -seq if required for the ng -run --evaluation case
  // ************************************************************************

  if (ScheduledMessages.size () > 0)
	{
	  if (PCore->GenerateRunXSCNSeq01 == true)
		{
		  //PB->S << Offset <<  "(There is a scheduled message)" << endl;

		  Run = ScheduledMessages.at (0);

		  if (Run != 0)
			{
			  // Generate the SCN
			  PB->GenerateSCNFromMessageBinaryPatterns (Run, SCN);

			  // Creating the ng -scn --s command line
			  PMB->NewSCNCommandLine ("0.1", SCN, Run, PCL);

			  //PB->S << "(" << endl << *Run << ")" << endl;

			  // Push the message to the GW input queue
			  PCore->PGW->PushToInputQueue (Run);

			  PCore->GenerateRunXSCNSeq01 = false;

			  Status = OK;
			}
		}
	}

  // ************************************************************************
  // Generate the ng -scn -seq if required for the ng -sr --b case
  // ************************************************************************

  if (PCore->GenerateStoreBindingsSCNSeq01 == true)
	{
	  //PB->S << Offset <<  "(There is an inline storage message)" << endl;

	  // Generate the ng -scn -seq
	  PB->GenerateSCNFromMessageBinaryPatterns (InlineResponseMessage, SCN);

	  // Creating the ng -scn --s command line
	  PMB->NewSCNCommandLine ("0.1", SCN, InlineResponseMessage, PCL);

	  // Set for false. Wait another ng -d --b to go to true again
	  PCore->GenerateStoreBindingsSCNSeq01 = false;

	  // Set for true. Enable next message to be processed at ng -d --b
	  PCore->GenerateStoreBindingsMsgCl01 = true;
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

/*
 *
 *
 * 	// Load the number of arguments
	if (_PCL->GetNumberofArguments(NA) == OK)
		{
			// Check the number of arguments
			if (NA == 1)
				{
					// Get the fist argument
					_PCL->GetArgument(0,PReceivedA);

					// Get received message SCN
					ReceivedSCN=PReceivedA.at(0);

					// Get ack message SCN
					AckSCN=PReceivedA.at(1);

					//PB->S << Offset <<"(AckSCN = "<< AckSCN << ")"<< endl;

					if (ReceivedSCN != "" && AckSCN != "")
						{
							// Interact over the Block Messages container
							for (unsigned int i=0; i<PB->GetNumberOfMessages(); i++)
								{
									if (PB->GetMessage(i,PM) == OK)
										{
											if (PM != 0)
												{
													if (PM->GetNumberofCommandLines(NCL) == OK)
														{
															if (NCL > 0)
																{
																	//PB->S << endl << Offset <<"(NCL = "<<NCL<<")"<< endl;

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
																																	PB->S << Offset <<"(Match with the message of index "<<i<<")"<< endl;

																																	PB->S << "(" << endl << *PM << ")"<< endl;

																																	//PB->S << Offset <<"(Checking whether it is necessary to store statistics or not)"<< endl;

																																	PM->GetCommandLine(1,PCL);

																																	PB->S << Offset <<"(Name = "<<PCL->Name<<")"<< endl;
																																	PB->S << Offset <<"(Alternative = "<<PCL->Alternative<<")"<< endl;

																																	// Check the command name
																																	if (PCL->Name == "-p" && PCL->Alternative == "--notify")
																																		{
																																			double Now=GetTime();

																																			double DeltaT=Now-PM->GetInstantiationTime();

																																			PB->S << Offset << setprecision(10) <<"(Round trip time = "<<DeltaT<<")"<< endl;

																																			PCore->pubrtt->Sample(DeltaT);

																																			PCore->pubrtt->CalculateArithmetic();

																																			PCore->pubrtt->SampleToFile(Now);
																																		}

																																	if (PCL->Name == "-s" && PCL->Alternative == "--b")
																																		{
																																			double Now=GetTime();

																																			double DeltaT=Now-PM->GetInstantiationTime();

																																			PB->S << Offset << setprecision(10) <<"(Round trip time = "<<DeltaT<<")"<< endl;

																																			PCore->subrtt->Sample(DeltaT);

																																			PCore->subrtt->CalculateArithmetic();

																																			PCore->subrtt->SampleToFile(Now);
																																		}

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
 *
 *
 */
