/*
	NovaGenesis

	Name:		IRSCNSeq02
	Object:		IRSCNSeq02
	File:		IRSCNSeq02.cpp
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.2

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

#ifndef _IRSCNSEQ02_H
#include "IRSCNSeq02.h"
#endif

#ifndef _IR_H
#include "IR.h"
#endif

//]////#define DEBUG

IRSCNSeq02::IRSCNSeq02 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

IRSCNSeq02::~IRSCNSeq02 ()
{
}

// Run the actions behind a received command line
// ng -scn --seq 0.2 [ < 1 string SCN > ]
int
IRSCNSeq02::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  string ReceivedSCN;
  string NewSCN;
  unsigned int NA = 0;
  vector<string> PA;
  string Offset = "                    ";
  CommandLine *PCL = 0;
  IR *PIR = 0;
  unsigned int NoCL;
  Message *PM = NULL;
  bool HaveStoreBind = false;
  bool HaveGetBind = false;
  bool HaveRevokeBind = false;

  PIR = (IR *)PB;

#ifdef DEBUG

  PB->S << Offset <<  this->GetLegibleName() << endl;

#endif

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 1)
		{
		  // Get the fist argument
		  _PCL->GetArgument (0, PA);

		  // Get received message SCN
		  ReceivedSCN = PA.at (0);

		  if (ReceivedSCN != "")
			{
			  if (PB->State == "operational" && PIR->GenerateSCNSeqForOriginalSource == true)
				{
				  // Check if ScheduledMessages is not empty
				  if (ScheduledMessages.size () > 0)
					{
					  // Copy the received command line to each ScheduledMessages
					  for (unsigned int i = 0; i < ScheduledMessages.size (); i++)
						{
						  //PMB->NewSCNCommandLine("0.1",ReceivedSCN,InlineResponseMessage,PCL);
						  PMB->NewSCNCommandLine ("0.1", ReceivedSCN, ScheduledMessages.at (i), PCL);
						}
					}

				  PIR->GenerateSCNSeqForOriginalSource = false;
				}

			  // Check if ScheduledMessages is not empty
			  if (ScheduledMessages.size () > 0)
				{
				  // Copy the received command line to each ScheduledMessages
				  for (unsigned int i = 0; i < ScheduledMessages.size (); i++)
					{
#ifdef DEBUG
					  PB->S << Offset <<  "(index = "<<i<<")"<<endl;
#endif

					  PM = ScheduledMessages.at (i);

					  // Generate the new SCN
					  //PB->GenerateSCNFromMessageBinaryPatterns(InlineResponseMessage,NewSCN);
					  PB->GenerateSCNFromMessageBinaryPatterns (PM, NewSCN);

					  // Add the SCN to the message
					  //PMB->NewSCNCommandLine("0.1",NewSCN,InlineResponseMessage,PCL);
					  PMB->NewSCNCommandLine ("0.1", NewSCN, PM, PCL);

					  if (PM->GetNumberofCommandLines (NoCL) == OK)
						{
						  if (NoCL > 3)
							{
							  PM->DoesThisCommandLineExistsInMessage ("-sr", "--b", HaveStoreBind);

							  PM->DoesThisCommandLineExistsInMessage ("-g", "--b", HaveGetBind);

							  PM->DoesThisCommandLineExistsInMessage ("-rvk", "--b", HaveRevokeBind);

							  if (HaveStoreBind == true || HaveGetBind == true || HaveRevokeBind == true)
								{

#ifdef DEBUG
								  PB->S << Offset <<  "(Scheduling the following message.)" << endl;

								  PB->S << "(" << endl << *PM << ")"<< endl;

#endif

								  // Push the message to the GW input queue
								  PIR->PGW->PushToInputQueue (PM);

								  HaveStoreBind = false;

								  HaveGetBind = false;

								  HaveRevokeBind = false;
								}
							}
						  else
							{

#ifdef DEBUG
							  PB->S << Offset <<  "(Warning: No need to schedule the following message.)" << endl;

							  PB->S << "(" << endl << *PM << ")"<< endl;

							  PB->S << Offset <<  "(Marking the message to be deleted.)" << endl;
#endif
							  PM->MarkToDelete ();
							}
						}
					}

				  ScheduledMessages.clear ();
				}

			  Status = OK;
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

#ifdef DEBUG
  PB->S << Offset <<  "(Marking to delete the message bellow)" << endl;

  PB->S << "(" << endl << *_ReceivedMessage << ")"<< endl;
#endif

  _ReceivedMessage->MarkToDelete ();

#ifdef DEBUG
  PB->S << Offset <<  "(Done)" << endl << endl << endl;
#endif

  return Status;
}
