/*
	NovaGenesis

	Name:		PGMsgCl01
	Object:		PGMsgCl01
	File:		PGMsgCl01.cpp
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

#ifndef _PGMSGCL01_H
#include "PGMsgCl01.h"
#endif

#ifndef _PGS_H
#include "PGCS.h"
#endif

#ifndef _PG_H
#include "PG.h"
#endif

//#define DEBUG

PGMsgCl01::PGMsgCl01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
  ForwardingCase = " ";

  // ************************** Adaptation Layer **************************

  // Self-verifying names (SVNes) used to classify supported adaptation layers

  // Set auxiliary hash of natural language keywords
  PB->GenerateSCNFromCharArrayBinaryPatterns ("Ethernet", HashEthernet);

  PB->GenerateSCNFromCharArrayBinaryPatterns ("Wi-Fi", HashWiFi);

  PB->GenerateSCNFromCharArrayBinaryPatterns ("IPv4_UDP", HashIPv4_UDP);

  // **********************************************************************
}

PGMsgCl01::~PGMsgCl01 ()
{
}

// Run the actions behind a received command line
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int
PGMsgCl01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = ERROR;
  unsigned int NA = 0;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  vector<string> NewDestinations;
  CommandLine *PCL;
  string Offset = "                    ";
  string MessageHID;
  string MyHID;
  PG *PPG = 0;
  string Identifier;
  string HashStack;
  vector<string> GetBind01MsgLimiters;
  vector<string> GetBind01MsgSources;
  vector<string> GetBind01MsgDestinations;
  vector<string> *PeerPGSHTBID = new vector<string>;
  vector<string> *Identifiers = new vector<string>;
  vector<string> *HashStacks = new vector<string>;
  PGCS *PPGCS = 0;

  PPG = (PG *)PB;

  PPGCS = (PGCS *)PB->PP;

#ifdef DEBUG

  PB->S << Offset << this->GetLegibleName () << endl;

#endif

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of argument
	  if (NA == 3)
		{
		  // Get received command line arguments
		  if (_PCL->GetArgument (0, Limiters) == OK && _PCL->GetArgument (1, Sources) == OK
			  && _PCL->GetArgument (2, Destinations) == OK)
			{
			  if (Limiters.size () > 0 && Sources.size () > 0 && Destinations.size () > 0)
				{
				  // ****************************************************
				  // Intradomain, interhost
				  // ****************************************************
				  if (Destinations.size () == 4)
					{
					  // ***********************************************
					  // Check the IHC case
					  // ***********************************************
					  if (Destinations.at (0) != "FFFFFFFF" && Destinations.at (1) != "FFFFFFFF" &&
						  Destinations.at (2) != "FFFFFFFF")
						{
						  // ***********************************************
						  // The typical case
						  // ***********************************************
						  MessageHID = Destinations.at (0);

						  MyHID = PB->PP->GetHostSelfCertifyingName ();

#ifdef DEBUG

						  PB->S << Offset << "(The Message HID is " << MessageHID << ")" << endl;

						  PB->S << Offset << "(This host HID is " << MyHID << ")" << endl;

#endif

						  if (MessageHID == MyHID)
							{
							  // ***********************************************
							  // The message is for the PG itself
							  // ***********************************************

#ifdef DEBUG
							  // Set the forwarding case
							  ForwardingCase = "Intraprocess: destination is a complete tuple at this PGCS";

							  PB->S << endl << Offset << "(      " << ForwardingCase << "         )" << endl << endl;
#endif

							  // Set the message execution time
							  InlineResponseMessage->SetTime (GetTime ());

							  // Clear the sources vector
							  Limiters.clear ();

							  // Add this process PDI as the limiter
							  Limiters.push_back (PB->PP->Intra_Process);

							  // Cleaning the Destinations vector
							  Destinations.clear ();

							  Destinations.push_back (PB->GetSelfCertifyingName ());

							  // Do not stop processing the other command lines of the message.
							  PB->StopProcessingMessage = false;

							  // Add a -m --cl 0.1 to the new message
							  if (PMB->NewConnectionLessCommandLine (_PCL->Version, &Limiters, &Destinations, &Destinations, InlineResponseMessage, PCL)
								  == OK)
								{
								  Status = OK;
								}
							  else
								{
								  PB->S << Offset << "(ERROR: Unable to create the -m --cl inline response message)"
										<< endl;

								  Status = ERROR;
								}
							}
						  else
							{
							  // ***********************************************
							  // The message is in transit to another host
							  // ***********************************************
#ifdef DEBUG
							  // Set the forwarding case
							  ForwardingCase = "Interhost: destination is a complete tuple outside this host";

							  PB->S << endl << Offset << "(      " << ForwardingCase << "         )" << endl << endl;
#endif

							  // Stop processing the other command lines of the message. The destination is another block
							  PB->StopProcessingMessage = true;

#ifdef DEBUG

							  PB->S << Offset << "(The destination HID is " << MessageHID << ")" << endl;

#endif
							  // ******************************************************************************************
							  // Check for a peer PGCS already acknowledged by hello
							  // ******************************************************************************************

							  // Get binding between Peer PGCS HID and its identifier (legacy or not) on Category 15
							  if (PB->PP->GetHTBindingValues (15, MessageHID, Identifiers) == OK)
								{
								  // Get binding between Peer PGCS HID and the Hash of the name of the peer PGCS stack (legacy or not)
								  if (PB->PP->GetHTBindingValues (8, MessageHID, HashStacks) == OK)
									{

#ifdef DEBUG
									  PB->S << Offset << "(Using next peer forwarding)" << endl;

#endif

									  // Assert the Values vector
									  if (Identifiers->size () > 0 && HashStacks->size () > 0)
										{
										  for (unsigned int q = 0; q < Identifiers->size (); q++)
											{
#ifdef DEBUG
											  PB->S << Offset << "(The Identifier " << q << " is "
													<< Identifiers->at (q) << ".)" << endl;
#endif
											}

										  // Carry the destination identifier (legacy or not)
										  Identifier = Identifiers->at (0);

										  for (unsigned int w = 0; w < HashStacks->size (); w++)
											{
											  // Carry the destination stack (legacy or not)
											  HashStack = HashStacks->at (w);

#ifdef DEBUG
											  PB->S << Offset << "(Hash of the selected stack = " << HashStack << ")"
													<< endl;
#endif

											  for (unsigned int y = 0; y < PPGCS->Identifiers->size (); y++)
												{
#ifdef DEBUG
												  PB->S << Offset << "(Identifiers[" << y << "] = "
														<< PPGCS->Identifiers->at (y) << ")" << endl;
#endif

												  // TODO: FIXP/Update - Modified here to allow the case that the PPGCS->Identifiers->at (y) is FF:FF:FF:FF:FF:FF
												  if (PPGCS->Identifiers->at (y) == Identifier || PPGCS->Identifiers->at (y) == "FF:FF:FF:FF:FF:FF")
													{
													  // ************************** Adaptation Layer **************************

													  // Here are the calls for the function on PG that effectively send the packets!!
													  // Check for the technologies accordingly to the peer.

													  if (HashStack == HashIPv4_UDP)
														{
#ifdef DEBUG
														  PB->S << Offset << "(The \"HashIPv4_UDP\" hash is = "
																<< HashIPv4_UDP << ")" << endl;
#endif
														  // Send the message using the UDP socket
														  PPG->SendToAUDPSocket (Identifier, PPGCS->Sizes
															  ->at (y), _ReceivedMessage);

														  // The message was already sent, so mark it to be deleted
														  _ReceivedMessage->MarkToDelete ();
														}

													  if (HashStack == HashEthernet || HashStack == HashWiFi)
														{
#ifdef DEBUG
														  PB->S << Offset << "(The \"Ethernet\" hash is = "
																<< HashEthernet << ")" << endl;
#endif

														  // Send the message
														  PPG->SendToARawSocket (PPGCS->Interfaces
																					 ->at (y), Identifier, PPGCS->Sizes
																					 ->at (y), _ReceivedMessage);

														  // The message was already sent, so mark it to be deleted
														  _ReceivedMessage->MarkToDelete ();
														}

													  // **********************************************************************
													}
												}
											}
										}
									  else
										{
										  PB->S << Offset << "(ERROR: the obtained vector of identifiers is empty)"
												<< endl;
										}
									}
								  else
									{
									  PB->S << Offset << "(ERROR: Unable to obtain the peer PGCS stack)" << endl;
									}
								}
							  else
								{
								  // *****************************************************************************************************************
								  // This code was added to allow a PGCS forward to another PGCS that it does not know using a relay PGCS in between
								  // *****************************************************************************************************************

								  // *****************************************************************************************************************
								  // Note: This code bellow create a snow ball that propagates messages to all outputs, generating lots of traffic
								  // *****************************************************************************************************************
								  /*

								  for (unsigned int w=0; w<PPGCS->Stacks->size(); w++)
									  {
										  // Calculate the hash of the stack
										  PB->GenerateSCNFromCharArrayBinaryPatterns(PPGCS->Stacks->at(w),HashStack);



#ifdef DEBUG
										  PB->S << Offset <<  "(Using broadcast forwarding to all peers)" << endl;

										  PB->S << Offset <<  "(Identifiers of stack ["<<w<<"] = "<<PPGCS->Identifiers->at(w)<<")"<<endl;
#endif

										  if (HashStack == HashZMQTCP)
											  {
#ifdef DEBUG
												  PB->S << Offset <<  "(The \"ZMQ_TCP\" hash is = "<<HashZMQTCP<<")"<< endl;
#endif
												  // Send the message using the ZMQ socket
												  PPG->SendToAZMQSocket(PPGCS->Identifiers->at(w),PPGCS->Sizes->at(w),_ReceivedMessage);

												  // The message was already sent, so mark it to be deleted
												  _ReceivedMessage->MarkToDelete();
											  }

										  if (HashStack == HashEthernet || HashStack == HashWiFi)
											  {
#ifdef DEBUG
												  PB->S << Offset <<  "(The \"Ethernet\" hash is = "<<HashEthernet<<")"<< endl;
#endif

												  // Send the message
												  PPG->SendToARawSocket(PPGCS->Interfaces->at(w),PPGCS->Identifiers->at(w),PPGCS->Sizes->at(w),_ReceivedMessage);

												  // The message was already sent, so mark it to be deleted
												  _ReceivedMessage->MarkToDelete();
											  }
									  }

									  */

								  PB->S << Offset << "(Warning: Unable to send the message. The Peer PGCS HID "
										<< MessageHID << " is unknown on PGCS:HT Category 15)" << endl;

								}
							}
						}
					  else
						{
						  // ***********************************************
						  // The ng -hello --ihc 0.1 case
						  // ***********************************************

						  // Do not stop processing the other command lines of the message.
						  PB->StopProcessingMessage = false;
						}
					}

				  // ****************************************************
				  // Interdomain, interhost
				  // ****************************************************
				  if (Destinations.size () == 5)
					{
					  // ***********************************************
					  // Check the IHC case
					  // ***********************************************
					  if (Destinations.at (0) != "FFFFFFFF" && Destinations.at (1) != "FFFFFFFF" &&
						  Destinations.at (2) != "FFFFFFFF")
						{
						  // ***********************************************
						  // The typical case
						  // ***********************************************
						  MessageHID = Destinations.at (1);

						  MyHID = PB->PP->GetHostSelfCertifyingName ();

#ifdef DEBUG

						  PB->S << Offset << "(The Message HID is " << MessageHID << ")" << endl;

						  PB->S << Offset << "(This host HID is " << MyHID << ")" << endl;

#endif

						  if (MessageHID == MyHID)
							{
							  // ***********************************************
							  // The message is for the PG itself
							  // ***********************************************

#ifdef DEBUG
							  // Set the forwarding case
							  ForwardingCase = "Intraprocess: destination is a complete tuple at this PGCS";

							  PB->S << endl << Offset << "(      " << ForwardingCase << "         )" << endl << endl;
#endif

							  // Set the message execution time
							  InlineResponseMessage->SetTime (GetTime ());

							  // Clear the sources vector
							  Limiters.clear ();

							  // Add this process PDI as the limiter
							  Limiters.push_back (PB->PP->Intra_Process);

							  // Cleaning the Destinations vector
							  Destinations.clear ();

							  Destinations.push_back (PB->GetSelfCertifyingName ());

							  // Do not stop processing the other command lines of the message.
							  PB->StopProcessingMessage = false;

							  // Add a -m --cl 0.1 to the new message
							  if (PMB->NewConnectionLessCommandLine (_PCL->Version, &Limiters, &Destinations, &Destinations, InlineResponseMessage, PCL)
								  == OK)
								{
								  Status = OK;
								}
							  else
								{
								  PB->S << Offset << "(ERROR: Unable to create the -m --cl inline response message)"
										<< endl;

								  Status = ERROR;
								}
							}
						  else
							{
							  // ***********************************************
							  // The message is in transit to another host outside this domain
							  // ***********************************************
#ifdef DEBUG
							  // Set the forwarding case
							  ForwardingCase = "Interdomain: destination is a complete tuple outside this host and domain";

							  PB->S << endl << Offset << "(      " << ForwardingCase << "         )" << endl << endl;
#endif

							  // Stop processing the other command lines of the message. The destination is another block
							  PB->StopProcessingMessage = true;

#ifdef DEBUG

							  PB->S << Offset << "(The destination HID is " << MessageHID << " and DID is "
									<< Destinations.at (0) << ")" << endl;

#endif

							  // Get binding between Peer PGCS HID and its identifier (legacy or not) on Category 15
							  if (PB->PP->GetHTBindingValues (15, MessageHID, Identifiers) == OK)
								{
								  // Get binding between Peer PGCS HID and the Hash of the name of the peer PGCS stack (legacy or not)
								  if (PB->PP->GetHTBindingValues (8, MessageHID, HashStacks) == OK)
									{
									  // Assert the Values vector
									  if (Identifiers->size () > 0 && HashStacks->size () > 0)
										{
										  for (unsigned int q = 0; q < Identifiers->size (); q++)
											{
											  //PB->S << Offset <<  "(The Identifier "<<q<<" is "<<Identifiers->at(q)<<".)" << endl;
											}

										  // Carry the destination identifier (legacy or not)
										  Identifier = Identifiers->at (0);

										  //cout << "Identifier = "<<Identifier<<endl;

										  for (unsigned int w = 0; w < HashStacks->size (); w++)
											{
											  // Carry the destination stack (legacy or not)
											  HashStack = HashStacks->at (w);

											  //PB->S << Offset <<  "(The HashStack "<<w<<" is "<<HashStack[w]<<")" << endl;

#ifdef DEBUG
											  cout << "Hash of the selected stack = " << HashStack << endl;
#endif

											  for (unsigned int y = 0; y < PPGCS->Identifiers->size (); y++)
												{
#ifdef DEBUG
												  cout << "Identifiers[" << y << "] = " << PPGCS->Identifiers->at (y)
													   << endl;
#endif

												  if (PPGCS->Identifiers->at (y) == Identifier)
													{
													  if (HashStack == HashIPv4_UDP)
														{
#ifdef DEBUG
														  PB->S << Offset << "(The \"IPv4_UDP\" hash is = "
																<< HashIPv4_UDP << ")" << endl;
#endif
														  // Send the message using the ZMQ socket
														  PPG->SendToAUDPSocket (Identifier, PPGCS->Sizes
															  ->at (y), _ReceivedMessage);

														  // The message was already sent, so mark it to be deleted
														  _ReceivedMessage->MarkToDelete ();
														}

													  if (HashStack == HashEthernet || HashStack == HashWiFi)
														{
#ifdef DEBUG
														  PB->S << Offset << "(The \"Ethernet\" hash is = "
																<< HashEthernet << ")" << endl;
#endif

														  // Send the message
														  PPG->SendToARawSocket (PPGCS->Interfaces
																					 ->at (y), Identifier, PPGCS->Sizes
																					 ->at (y), _ReceivedMessage);

														  // The message was already sent, so mark it to be deleted
														  _ReceivedMessage->MarkToDelete ();
														}
													}
												}
											}
										}
									  else
										{
										  PB->S << Offset << "(ERROR: the obtained vector of identifiers is empty)"
												<< endl;
										}
									}
								  else
									{
									  PB->S << Offset << "(ERROR: Unable to obtain the peer PGCS stack)" << endl;
									}
								}
							  else
								{
								  PB->S << Offset << "(ERROR: Unable to obtain the peer PGCS identifier)" << endl;
								}
							}
						}
					  else
						{
						  // ***********************************************
						  // The ng -hello --ihc 0.1 case
						  // ***********************************************

						  // Do not stop processing the other command lines of the message.
						  PB->StopProcessingMessage = false;
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

  delete PeerPGSHTBID;
  delete Identifiers;
  delete HashStacks;

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}
