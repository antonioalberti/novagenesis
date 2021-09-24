/*
	NovaGenesis

	Name:		GWStatusS01
	Object:		GWStatusS01
	File:		GWStatusS01.cpp
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

#ifndef _GWSTATUSS01_H
#include "GWStatusS01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

////#define DEBUG

GWStatusS01::GWStatusS01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
  ListBindings = true;
}

GWStatusS01::~GWStatusS01 ()
{
}

// Run the actions behind a received command line
// ng -st --s _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > ]
int
GWStatusS01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  vector<string> RelatedCommand;
  vector<string> StatusCode;
  string Offset = "          ";
  GW *PGW = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  Message *GWHelloToPGS = 0;
  CommandLine *PCL = 0;
  string SCN = "FFFFFFFF";
  CommandLine *PSCNCL = 0;
  vector<string> LegibleNames;
  vector<string> GWStoreBind01MsgLimiters;
  vector<string> GWStoreBind01MsgSources;
  vector<string> GWStoreBind01MsgDestinations;
  string Version = "0.1";
  string Key;
  vector<string> Values;
  string HashLegiblePeerProcessName;
  string IPv6;
  string HID;
  sem_t *mutex;

  // ******************************************************
  // Calculate auxiliary variables and required input data
  // ******************************************************

  PGW = (GW *)PB;

  LegibleNames.push_back (PB->PP->GetLegibleName ());

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 2)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, RelatedCommand);
		  _PCL->GetArgument (1, StatusCode);

		  if (RelatedCommand.size () > 0 && StatusCode.size () > 0)
			{
			  if (RelatedCommand.at (0) == "-sr" && RelatedCommand.at (1) == "--b" && StatusCode.at (0) == "0"
				  && PB->State == "initialization")
				{
				  // Change the state from initialization to hello
				  PB->State = "hello";

				  PB->S << endl << Offset
						<< "(Moving to hello state)"
						<< endl << endl;

				  if (PB->PP->GetLegibleName () != "PGCS" && PB->StopProcessingMessage == false)
					{
					  // **************************************************************
					  // Sending a Hello message to the PGCS::GW at the same host
					  // **************************************************************

					  // Creating a new message
					  PB->PP->NewMessage (GetTime (), 0, false, GWHelloToPGS);

					  // Setting up the OS SCN as the space limiter
					  Limiters.push_back (PB->PP->Intra_OS);

					  // Setting up this process PID as the first source SCN
					  Sources.push_back (PB->PP->GetSelfCertifyingName ());

					  // Setting up the GW block SCN as the second source SCN
					  Sources.push_back (PB->GetSelfCertifyingName ());

					  // Setting up the destination process PID as empty
					  Destinations.push_back ("FFFFFFFF");

					  // Setting up the destination block BID as empty
					  Destinations.push_back ("FFFFFFFF");

					  // ******************************************************
					  // Create the first command line
					  // ******************************************************

					  // Creating the ng -cl -m command line
					  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, GWHelloToPGS, PCL);

					  // ******************************************************
					  // Create the second command line
					  // ******************************************************

					  PMB->NewIPCHelloCommandLine ("--ipc", "0.1", PB->PP->Key, PB->PP
						  ->GetLegibleName (), GWHelloToPGS, PCL);

					  // ******************************************************
					  // Setting up the SCN command line
					  // ******************************************************

					  // Generate the SCN
					  PB->GenerateSCNFromMessageBinaryPatterns (GWHelloToPGS, SCN);

					  // Creating the ng -scn --s command line
					  PMB->NewSCNCommandLine ("0.1", SCN, GWHelloToPGS, PSCNCL);

					  // ******************************************************
					  // Setting up the message IPC output key flag
					  // ******************************************************

					  PB->S << Offset << "(The PGCS IPC Key is 11)" << endl;

					  // Note: The 11 key is a well known number of all process in an OS
					  // More investigation is required concerning access collision

					  // ******************************************************
					  // Converting the message to char array
					  // ******************************************************

					  GWHelloToPGS->ConvertMessageFromCommandLinesandPayloadFileToCharArray ();

					  // ******************************************************
					  // Finish
					  // ******************************************************

					  // Modified in 11th April 2021 to deal with parallel shared memories.
					  // Set the semaphore name
					  string SemaphoreName = "Output_Queue";

					  while (1)
						{
						  mutex = sem_open (SemaphoreName.c_str (), O_CREAT, 0666, 1);

						  // Check for error on semaphore open
						  if (mutex != SEM_FAILED)
							{
#ifdef DEBUG
							  PB->S << Offset << "(Writing Output Queue)" << endl;
#endif
							  if (sem_trywait (mutex) == 0)
								{
#ifdef DEBUG
								  PB->S << Offset << "(Locked the semaphore " << SemaphoreName << ")" << endl;
#endif
								  // Push the message to the GW output queue
								  PGW->PushToOutputQueue ("11", GWHelloToPGS);

								  Status = OK;

								  if (sem_post (mutex) != 0) // Successfully locked the semaphore, so unlock it
									{
									  perror ("Writing Output Queue : sem_post");
									}
								} // (sem_trywait(mutex) != 0) means UNABLE TO LOCK. If someone locked, it is normal to get this condition

							  if (sem_close (mutex) != 0)
								{
								  perror ("Writing Output Queue : sem_close");
								}
							} // (mutex == SEM_FAILED) means it was unable to create/open the semaphore
						  else
							{
							  perror ("Writing Output Queue : unable to create/open semaphore");

							  sem_unlink (SemaphoreName.c_str ());
							}

						  if (Status == OK)
							{
							  break;
							}
						}
					}
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;

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

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
