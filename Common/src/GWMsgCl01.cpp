/*
	NovaGenesis

	Name:		GWMsgCl01
	Object:		GWMsgCl01
	File:		GWMsgCl01.cpp
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

#ifndef _GWMSGCL01_H
#include "GWMsgCl01.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

//#define DEBUG
////#define DEBUG1

GWMsgCl01::GWMsgCl01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

GWMsgCl01::~GWMsgCl01 ()
{
}

// The forwarding is limited to a Process
int
GWMsgCl01::ForwardMessageInsideProcess (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NoCL = 0;
  vector<string> ReceivedMessageDestinations;
  vector<string> GWGetBind01MsgLimiters;
  vector<string> GWGetBind01MsgSources;
  vector<string> GWGetBind01MsgDestinations;
  unsigned int Category = 3;
  string Version = "0.1";
  string Key = "";
  Message *AutomaticInlineResponseMessage = NULL;
  GW *PGW = 0;
  vector<string> *Values = new vector<string>;
  string BindingValue = "";
  Block *CalledBlock = 0;
  unsigned int Index = 0;
  stringstream ss;
  string Offset = "          ";

  // *******************************************************************************
  // Discover the destination block pointer according to the HT stored Blocks index
  // *******************************************************************************

  PGW = (GW *)PB;

  // Prepare the fields of the GW ng -g --b 0.1 message
  GWGetBind01MsgLimiters.push_back (PB->PP->GetSelfCertifyingName ());
  GWGetBind01MsgSources.push_back (PB->GetSelfCertifyingName ());
  GWGetBind01MsgDestinations.push_back (PGW->PHT->GetSelfCertifyingName ());

  // Get received command line arguments
  _PCL->GetArgument (2, ReceivedMessageDestinations);

  // Setting up the category
  Category = 13;

  if (ReceivedMessageDestinations.size () > 0)
	{
	  // Setting up the key. Note: The destination block is the last value on the destinations argument. Some guarantee test for this is missing.
	  Key = ReceivedMessageDestinations.at ((ReceivedMessageDestinations.size () - 1));

#ifdef DEBUG

	  PB->S << Offset << "(Forwarding: The destination block has the SCN = " << Key << ")" << endl;

#endif

	  if (Key != "")
		{
		  if (PB->PP->GetHTBindingValues (Category, Key, Values) == OK)
			{
			  // Assert the Values vector
			  if (!Values->empty ())
				{
				  // Carry the destination pointer
				  BindingValue = Values->at (0);

				  // Cast to an integer
				  ss << BindingValue;
				  ss >> Index;

#ifdef DEBUG
				  PB->S << Offset << "(Forwarding: The destination block has index " << Index << ")" << endl;

#endif

				  if (Index == 0 || Index > 1)
					{
					  // Stop processing the other command lines of the message. The destination is another block
					  PB->StopProcessingMessage = true;

					  // Getting the destination block pointer
					  PB->PP->GetBlock (Index, CalledBlock);

					  if (CalledBlock != 0 && CalledBlock != PB)
						{
#ifdef DEBUG
						  PB->S << Offset << "(Forwarding: calling the block " << CalledBlock->GetLegibleName () << ")"
								<< endl;

#endif
						  // Create a new inline response message to enable called block to answer
						  PB->PP
							  ->NewMessage (GetTime (), 0, false, AutomaticInlineResponseMessage);

						  // Call the block Run message function and pass the message
						  CalledBlock->Run (_ReceivedMessage, AutomaticInlineResponseMessage);

						  if (AutomaticInlineResponseMessage != NULL)
							{
							  AutomaticInlineResponseMessage->GetNumberofCommandLines (NoCL);

							  if (NoCL > 2)
								{
#ifdef DEBUG
								  PB->S << endl << endl << Offset
										<< "(Forwarding: The following inline response message with instantiation number "
										<< AutomaticInlineResponseMessage->InstantiationNumber << " was generated:)"
										<< endl;

								  PB->S << "(" << endl << *AutomaticInlineResponseMessage << ")" << endl;
#endif

								  // Push the message to the GW input queue
								  PGW->PushToInputQueue (AutomaticInlineResponseMessage);
								}
							  else
								{
								  AutomaticInlineResponseMessage->MarkToDelete ();

								  //PB->S << Offset <<  "(ERROR: An inline message has less than 3 command lines)" << endl;
								}
							}
						  else
							{
							  PB->S << Offset << "(ERROR: An inline scheduled message is corrupted)" << endl;
							}

						  // Mark the message to be deleted
						  _ReceivedMessage->MarkToDelete ();

						  //PB->S << Offset << "("<<CalledBlock->GetLegibleName()<<" message container size = "<<CalledBlock->GetNumberOfMessages()<<")"<< endl;

						  // Set Status
						  Status = OK;
						}
					  else
						{
						  PB->S << Offset << "(ERROR: Invalid pointer to the destination Block)" << endl;
						}
					}
				  else
					{
					  // Do not stop processing the other command lines of the message. The destination is the GW
					  PB->StopProcessingMessage = false;

					  //PB->S << Offset <<  "(Forwarding: The destination is the gateway)" << endl;
					}
				}
			  else
				{
				  PB->S << Offset << "(ERROR: Unable to read the binding value at forwarding inside a process)" << endl;
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Unable to get the forwarding binding from destination block)" << endl;
			}
		}
	  else
		{
		  PB->S << Offset << "(ERROR: The key to subscribe the destination block is null)" << endl;
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: The destination argument on the received message is empty)" << endl;
	}

  Values->clear ();

  delete Values;

  return Status;
}

// The forwarding is limited to an OS
int
GWMsgCl01::ForwardMessageInsideOS (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  vector<string> ReceivedMessageLimiters;
  vector<string> ReceivedMessageSources;
  vector<string> ReceivedMessageDestinations;
  Message *GWStatusS01Msg = 0;
  CommandLine *GWMsgCl01 = 0;
  vector<string> GWGetBind01MsgLimiters;
  vector<string> GWGetBind01MsgSources;
  vector<string> GWGetBind01MsgDestinations;
  GW *PGW = 0;
  unsigned int Category = 3;
  string Key = "";
  vector<string> *Values = new vector<string>;
  string BindingValue = "";
  string Offset = "          ";
  sem_t *mutex;

  PGW = (GW *)PB;

  // Get received command line arguments
  _PCL->GetArgument (0, ReceivedMessageLimiters);
  _PCL->GetArgument (1, ReceivedMessageSources);
  _PCL->GetArgument (2, ReceivedMessageDestinations);

  if (ReceivedMessageLimiters.size () == 1 && ReceivedMessageSources.size () >= 2
	  && ReceivedMessageDestinations.size () >= 2)
	{
	  if (ReceivedMessageDestinations.at (0) != "FFFFFFFF" && ReceivedMessageDestinations.at (1) != "FFFFFFFF")
		{
		  // Test for dropping the message or to continue to other process
		  if (ReceivedMessageDestinations.at ((ReceivedMessageDestinations.size () - 2))
			  == PB->PP->GetSelfCertifyingName ())
			{
			  // **********************************************************************************
			  // The destination is a block inside this process. Use ForwardMessageInsideProcess
			  // **********************************************************************************
#ifdef DEBUG
			  PB->S << Offset << "(Forwarding: The destination is a block inside this process)" << endl;
#endif

			  Status = ForwardMessageInsideProcess (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);

			  if (PB->StopProcessingMessage == false)
				{
				  // Create a new message to answer the one being processed at GW
				  PB->PP->NewMessage (GetTime (), 0, false, GWStatusS01Msg);

				  // Store the index of this message on Messages container
				  ScheduledMessages.push_back (GWStatusS01Msg);

				  // Create a new ng -m --cl 0.1 command line
				  PMB->NewConnectionLessCommandLine ("0.1",
													 &ReceivedMessageLimiters,
													 &ReceivedMessageDestinations,
													 &ReceivedMessageSources,
													 GWStatusS01Msg,
													 GWMsgCl01);
				}
			}
		  else
			{
			  // **********************************************************************************
			  // The destination is a block outside this process. Send to the Output Queue for IPC
			  // **********************************************************************************
#ifdef DEBUG
			  PB->S << Offset << "(Forwarding: The destination is a block outside this process)" << endl;
#endif

			  // Stop processing the other command lines of the message. The destination is another block outside this process
			  PB->StopProcessingMessage = true;

			  // Resolves the adequate destination IPC output (limited to this OS)
			  // PID -> Output Key

			  // Setting up the category
			  Category = 13;

			  // Setting up the key.
			  Key = ReceivedMessageDestinations.at (ReceivedMessageDestinations.size () - 2);

#ifdef DEBUG
			  PB->S << Offset << "(Looking at Category 13 for the shared memory key behind the SCN = " << Key << ")"
					<< endl;
#endif

			  if (Key != "")
				{
				  if (PB->PP->GetHTBindingValues (Category, Key, Values) == OK)
					{
					  // Assert the Values vector
					  if (Values != 0)
						{
						  if (!Values->empty ())
							{
							  // Carry the destination pointer
							  BindingValue = Values->at (0);

#ifdef DEBUG
							  PB->S << endl << endl << Offset << "(Forwarding: The destination IPC key is "
									<< BindingValue << ")" << endl;
#endif

							  // Modified in 11th April 2021 to deal with parallel shared memories.
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
										  PGW->PushToOutputQueue (BindingValue, _ReceivedMessage);

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
					  else
						{
						  PB->S << Offset << "(ERROR: Null values vector)" << endl;
						}
					}
				  else
					{
					  // ***************************************************
					  // Alternative forwarding to the local OS PGCS
					  // ***************************************************

#ifdef DEBUG
					  PB->S << endl << endl << Offset << "(Forwarding: The destination IPC key is 11)" << endl;
#endif
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
								  PGW->PushToOutputQueue ("11", _ReceivedMessage);

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
		}
	  else
		{
		  // **********************************************************************************
		  // The special ng -hello --ipc case
		  // **********************************************************************************

#ifdef DEBUG
		  PB->S << endl << Offset << "(Forwarding: A ng -hello --ipc 0.1 message was received)" << endl << endl;
#endif

		  // Create a new message to answer the one being processed at GW
		  PB->PP->NewMessage (GetTime (), 0, false, GWStatusS01Msg);

		  // Store this message on the scheduled messages container
		  ScheduledMessages.push_back (GWStatusS01Msg);

		  // Clear the destinations vector, since it is empty
		  ReceivedMessageDestinations.clear ();

		  // Add this process SCN to the destinations vector
		  ReceivedMessageDestinations.push_back (PB->PP->GetSelfCertifyingName ());

		  // Add this block SCN to the destinations vector
		  ReceivedMessageDestinations.push_back (PB->GetSelfCertifyingName ());

		  // Create a new ng -m --cl 0.1 command line
		  PMB->NewConnectionLessCommandLine ("0.1",
											 &ReceivedMessageLimiters,
											 &ReceivedMessageDestinations,
											 &ReceivedMessageSources,
											 GWStatusS01Msg,
											 GWMsgCl01);
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Received command line differs from the expected for an Intra OS message)" << endl;
	}

  Values->clear ();

  delete Values;

  return Status;
}

// The forwarding is limited to a Domain
int
GWMsgCl01::ForwardMessageInsideDomain (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  vector<string> ReceivedMessageLimiters;
  vector<string> ReceivedMessageSources;
  vector<string> ReceivedMessageDestinations;
  vector<string> GWGetBind01MsgLimiters;
  vector<string> GWGetBind01MsgSources;
  vector<string> GWGetBind01MsgDestinations;
  string Key = "";
  string BindingValue = "";
  string Offset = "          ";
  Block *PPGB = 0;
  Message *AutomaticInlineResponseMessage = NULL;
  unsigned int NoCL = 0;
  GW *PGW = 0;

  PGW = (GW *)PB;

  // Get received command line arguments
  _PCL->GetArgument (0, ReceivedMessageLimiters);
  _PCL->GetArgument (1, ReceivedMessageSources);
  _PCL->GetArgument (2, ReceivedMessageDestinations);

  if (ReceivedMessageLimiters.size () == 1 && ReceivedMessageSources.size () >= 3
	  && ReceivedMessageDestinations.size () >= 3)
	{
	  if (ReceivedMessageDestinations.at (0) != "FFFFFFFF" && ReceivedMessageDestinations.at (1) != "FFFFFFFF" &&
		  ReceivedMessageDestinations.at (2) != "FFFFFFFF" && ReceivedMessageDestinations.at (3) != "FFFFFFFF")
		{
		  // Test for dropping the message or to continue to other OS
		  if (ReceivedMessageDestinations.at (0) == PB->PP->GetHostSelfCertifyingName () &&
			  ReceivedMessageDestinations.at (1) == PB->PP->GetOperatingSystemSelfCertifyingName ())
			{
			  // **********************************************************************************
			  // The destination is a block inside this OS. Use ForwardMessageInsideOS
			  // **********************************************************************************
#ifdef DEBUG
			  PB->S << Offset << "(Forwarding: The destination is a block inside this OS)" << endl;
#endif

			  Status = ForwardMessageInsideOS (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
			}
		  else
			{
			  // **********************************************************************************
			  // The destination is a block outside this OS. Send to the PG block for IHC
			  // **********************************************************************************
#ifdef DEBUG
			  PB->S << Offset << "(Forwarding: The destination is a block outside this OS)" << endl;
#endif

			  // Stop processing the other command lines of the message. The destination is a block outside this OS
			  PB->StopProcessingMessage = true;

			  // Call an internal PG block to deal with the IHC forwarding
			  PB->PP->GetBlock ("PG", PPGB);

			  if (PPGB != 0)
				{
				  // Create a new inline response message to enable called block to answer
				  PB->PP->NewMessage (GetTime (), 0, false, AutomaticInlineResponseMessage);

				  // Run the PG
				  Status = PPGB->Run (_ReceivedMessage, AutomaticInlineResponseMessage);

				  if (AutomaticInlineResponseMessage != NULL)
					{
					  AutomaticInlineResponseMessage->GetNumberofCommandLines (NoCL);

					  if (NoCL > 2)
						{
#ifdef DEBUG1
						  PB->S << endl << endl << Offset <<  "(Forwarding: The following inline response message with instantiation number "<<AutomaticInlineResponseMessage->InstantiationNumber<<" was generated:)" <<endl;

						  PB->S << "(" << endl << *AutomaticInlineResponseMessage << ")"<< endl;
#endif

						  // Push the message to the GW input queue
						  PGW->PushToInputQueue (AutomaticInlineResponseMessage);
						}
					  else
						{
						  AutomaticInlineResponseMessage->MarkToDelete ();

#ifdef DEBUG1
						  PB->S << Offset <<  "(Warning: An inline message has less than 3 command lines)" << endl;
#endif
						}

					}
				  else
					{
					  PB->S << Offset << "(ERROR: An inline scheduled message is corrupted)" << endl;
					}

				  // Mark the message to be deleted
				  _ReceivedMessage->MarkToDelete ();
				}
			  else
				{
#ifdef DEBUG
				  PB->S << Offset << "(Warning: Unable to call the PG block. Sending via IPC to the PGCS)" << endl;
#endif

				  Status = ForwardMessageInsideOS (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
				}
			}
		}
	  else
		{
		  // The -hello --ihc case. Do not continue to process the message
		  PB->StopProcessingMessage = false;

		  // Call an internal PG block to deal with the IHC forwarding
		  PB->PP->GetBlock ("PG", PPGB);

		  // Modify the BLock SCN to the PG SCN
		  _ReceivedMessage->SetCommandLineArgumentElement (0, 2, 3, PPGB->GetSelfCertifyingName ());

		  // Send to the PGCS::PG
		  Status = ForwardMessageInsideProcess (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: The received command line differs from the expected for an Intra OS message)"
			<< endl;
	}

  return Status;
}

// The forwarding is between two domains
int
GWMsgCl01::ForwardInterDomainMessage (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  vector<string> ReceivedMessageLimiters;
  vector<string> ReceivedMessageSources;
  vector<string> ReceivedMessageDestinations;
  vector<string> GWGetBind01MsgLimiters;
  vector<string> GWGetBind01MsgSources;
  vector<string> GWGetBind01MsgDestinations;
  string Key = "";
  string BindingValue = "";
  string Offset = "          ";
  Block *PPGB = 0;
  Message *AutomaticInlineResponseMessage = NULL;
  unsigned int NoCL = 0;
  GW *PGW = 0;

  PGW = (GW *)PB;

  // Get received command line arguments
  _PCL->GetArgument (0, ReceivedMessageLimiters);
  _PCL->GetArgument (1, ReceivedMessageSources);
  _PCL->GetArgument (2, ReceivedMessageDestinations);

  if (ReceivedMessageLimiters.size () == 1 && ReceivedMessageSources.size () >= 4
	  && ReceivedMessageDestinations.size () >= 4)
	{
	  if (ReceivedMessageDestinations.at (0) != "FFFFFFFF" && ReceivedMessageDestinations.at (1) != "FFFFFFFF"
		  && ReceivedMessageDestinations.at (2) != "FFFFFFFF" &&
		  ReceivedMessageDestinations.at (3) != "FFFFFFFF" && ReceivedMessageDestinations.at (4) != "FFFFFFFF")
		{
		  // Test for continuing to other OS
		  if (ReceivedMessageDestinations.at (1) == PB->PP->GetHostSelfCertifyingName () &&
			  ReceivedMessageDestinations.at (2) == PB->PP->GetOperatingSystemSelfCertifyingName ())
			{
			  // **********************************************************************************
			  // The destination is a block inside this OS. Use ForwardMessageInsideOS
			  // **********************************************************************************
#ifdef DEBUG
			  PB->S << Offset << "(Forwarding: The destination is a block inside this OS)" << endl;
#endif

			  Status = ForwardMessageInsideOS (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
			}
		  else
			{
			  // **********************************************************************************
			  // The destination is a block outside this OS. Send to the PG block for IHC
			  // **********************************************************************************
#ifdef DEBUG
			  PB->S << Offset << "(Forwarding: The destination is a block outside this OS)" << endl;
#endif

			  // Stop processing the other command lines of the message. The destination is a block outside this OS
			  PB->StopProcessingMessage = true;

			  // Call an internal PG block to deal with the IHC forwarding
			  PB->PP->GetBlock ("PG", PPGB);

			  if (PPGB != 0)
				{
				  // Create a new inline response message to enable called block to answer
				  PB->PP->NewMessage (GetTime (), 0, false, AutomaticInlineResponseMessage);

				  // Run the PG
				  Status = PPGB->Run (_ReceivedMessage, AutomaticInlineResponseMessage);

				  if (AutomaticInlineResponseMessage != NULL)
					{
					  AutomaticInlineResponseMessage->GetNumberofCommandLines (NoCL);

					  if (NoCL > 2)
						{
#ifdef DEBUG1
						  PB->S << endl << endl << Offset <<  "(Forwarding: The following inline response message with instantiation number "<<AutomaticInlineResponseMessage->InstantiationNumber<<" was generated:)" <<endl;

						  PB->S << "(" << endl << *AutomaticInlineResponseMessage << ")"<< endl;
#endif

						  // Push the message to the GW input queue
						  PGW->PushToInputQueue (AutomaticInlineResponseMessage);
						}
					  else
						{
						  AutomaticInlineResponseMessage->MarkToDelete ();

#ifdef DEBUG1
						  PB->S << Offset <<  "(ERROR: An inline message has less than 3 command lines)" << endl;
#endif
						}

					}
				  else
					{
					  PB->S << Offset << "(ERROR: An inline scheduled message is corrupted)" << endl;
					}

				  // Mark the message to be deleted
				  _ReceivedMessage->MarkToDelete ();
				}
			  else
				{
#ifdef DEBUG
				  PB->S << Offset << "(Warning: Unable to call the PG block. Sending via IPC to the PGCS)" << endl;
#endif

				  Status = ForwardMessageInsideOS (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
				}
			}
		}
	  else
		{
		  // The -hello --ihc case. Do not continue to process the message
		  PB->StopProcessingMessage = false;

		  // Call an internal PG block to deal with the IHC forwarding
		  PB->PP->GetBlock ("PG", PPGB);

		  // Modify the Block SCN to the PG SCN
		  _ReceivedMessage->SetCommandLineArgumentElement (0, 2, 4, PPGB->GetSelfCertifyingName ());

		  // Send to the PGCS::PG
		  Status = ForwardMessageInsideProcess (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: The received command line differs from the expected for an Intra OS message)"
			<< endl;
	}

  return Status;
}

// Run the actions behind a received command line
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int
GWMsgCl01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  // Note: 	This implementation is limited to one limiter SCN.
  // 			Also, it does not assert if source and destination are members of the space defined by this limiter SCN.
  // 			Finally, it does not support multiple operating systems in a single host machine.

  int Status = ERROR;
  unsigned int NA = 0;
  vector<string> ReceivedMessageLimiters;
  vector<string> ReceivedMessageSources;
  vector<string> ReceivedMessageDestinations;
  vector<string> GWGetBind01MsgLimiters;
  vector<string> GWGetBind01MsgSources;
  vector<string> GWGetBind01MsgDestinations;
  unsigned int Category = 3;
  string Version = "0.1";
  string Key;
  vector<string> *Values = new vector<string>;
  string BindingValue = "";
  stringstream ss;
  string Offset = "          ";

#ifdef DEBUG

  PB->S << Offset << "(Running the action " << this->GetLegibleName () << " at block " << PB->GetLegibleName () << ")"
		<< endl;

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 3)
		{
		  // Get received command line arguments
		  _PCL->GetArgument (0, ReceivedMessageLimiters);
		  _PCL->GetArgument (1, ReceivedMessageSources);
		  _PCL->GetArgument (2, ReceivedMessageDestinations);

		  if (ReceivedMessageLimiters.size () > 0 && ReceivedMessageSources.size () > 0
			  && ReceivedMessageDestinations.size () > 0)
			{
			  // ***********************************************************************************
			  // Get the legible name associated to the received limiter
			  // ***********************************************************************************

			  Key = ReceivedMessageLimiters.at (0);

#ifdef DEBUG
			  PB->S << Offset << "(Forwarding: The SCN associated to the received limiter is " << Key << ")" << endl;
#endif

			  // *************************
			  // Decide what to do
			  // *************************
			  if (Key == PB->PP->Intra_Process)
				{
				  // ***********************************************************************************
				  // The forwarding is limited to a Process
				  // ***********************************************************************************

#ifdef DEBUG
				  PB->S << Offset << "(Forwarding: The message forwarding is limited to this Process)" << endl;
#endif

				  Status = ForwardMessageInsideProcess (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
				}
			  else if (Key == PB->PP->Intra_OS)
				{
				  // ***********************************************************************************
				  // The forwarding is limited to an Operating System
				  // ***********************************************************************************
#ifdef DEBUG
				  PB->S << Offset << "(Forwarding: The message forwarding is limited to this OS)" << endl;
#endif

				  Status = ForwardMessageInsideOS (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
				}
			  else if (Key == PB->PP->Intra_Domain)
				{
				  // ***********************************************************************************
				  // The forwarding is limited to a Domain
				  // ***********************************************************************************
#ifdef DEBUG
				  PB->S << Offset << "(Forwarding: The message forwarding is limited to this Domain)" << endl;
#endif

				  Status = ForwardMessageInsideDomain (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
				}
			  else if (Key == PB->PP->Inter_Domain)
				{
				  // ***********************************************************************************
				  // The forwarding is between two domains
				  // ***********************************************************************************
#ifdef DEBUG
				  PB->S << Offset << "(Forwarding: The message forwarding is between two domains)" << endl;
#endif

				  Status = ForwardInterDomainMessage (_ReceivedMessage, _PCL, ScheduledMessages, InlineResponseMessage);
				}
			  else
				{
				  PB->S << Offset << "(ERROR: Invalid forwarding option)" << endl;

				  Status = ERROR;
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

  delete Values;

#ifdef DEBUG

  PB->S << Offset << "(Done)" << endl << endl << endl;

#endif

  return Status;
}
