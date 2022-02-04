/*
	NovaGenesis

	Name:		Proxy and Gateway for underlying resources
	Object:		PG
	File:		PG.cpp
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

	*********************************************************************************************
	Some of the functions to receive and send messages using sockets were adapted from beej.us
	*********************************************************************************************

	Beej's Guide to Network Programming is Copyright © 2012 Brian "Beej Jorgensen" Hall.
	With specific exceptions for source code and translations, below, this work is licensed under the Creative Commons Attribution- Noncommercial- No Derivative Works 3.0 License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/3.0/ or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
	One specific exception to the "No Derivative Works" portion of the license is as follows: this guide may be freely translated into any language, provided the translation is accurate, and the guide is reprinted in its entirety. The same license restrictions apply to the translation as to the original guide. The translation may also include the name and contact information for the translator.
	The C source code presented in this document is hereby granted to the public domain, and is completely free of any license restriction.
	Educators are freely encouraged to recommend or supply copies of this guide to their students.

	Contact beej@beej.us for more information.

	*********************************************************************************************
	Additional functions to discover Linux Interface names, i.e. em1, eth0, etc.
	*********************************************************************************************

	* showif.c : PUBLIC DOMAIN - Jon Mayo - August 22, 2006
 	* - You may remove any comments you wish, modify this code any way you wish,
 	*   and distribute any way you wish.
	* finds all network interfaces and shows a little information about them.
	* some operating systems list interfaces multiple times because of different
	* flags, modes, etc. if you want to use this code you should be aware that
	* duplicate interfaces is a possibility

*/

#ifndef _PG_H
#include "PG.h"
#endif

#ifndef _PGCS_H
#include "PGCS.h"
#endif

#ifndef _PGRUNINITIALIZATION01_H
#include "PGRunInitialization01.h"
#endif

#ifndef _PGMSGCL01_H
#include "PGMsgCl01.h"
#endif

#ifndef _PGHELLOIHC01_H
#include "PGHelloIHC01.h"
#endif

#ifndef _PGHELLOIHC02_H
#include "PGHelloIHC02.h"
#endif

#ifndef _PGHELLOIHC03_H
#include "PGHelloIHC03.h"
#endif

#ifndef _PGRUNEXPOSITION01_H
#include "PGRunExposition01.h"
#endif

#ifndef _PGRUNHELLO01_H
#include "PGRunHello01.h"
#endif

#ifndef _PGRUNHELLO02_H
#include "PGRunHello02.h"
#endif

#ifndef _PGRUNHELLO03_H
#include "PGRunHello03.h"
#endif

#ifndef _PGRUNPUBLISHING01_H
#include "PGRunPublishing01.h"
#endif

#ifndef _PGRUNPERIODIC01_H
#include "PGRunPeriodic01.h"
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

//#define DEBUG
//#define DEBUG2
//#define DEBUG1
//#define DEBUG3  // DEBUG6 or 3 are mutually exclusive
//#define DEBUG5  // Details of each segment sent
//#define DEBUG6 // TODO - FIXP/Update - Created a new debug only for child thread transfer
//#define STATISTICS


union ethframe {
  struct {
	struct ethhdr header;
	unsigned char data[ETH_DATA_LEN];
  } field;
  unsigned char buffer[ETH_FRAME_LEN];
};

PG::PG (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
	: Block (_LN, _PP, _Index, _Path)
{
  PGW = _PGW;
  PHT = _PHT;
  State = "initialization";

  Message *PIM = 0;
  CommandLine *PCL = 0;
  Message *InlineResponseMessage = NULL;
  Action *PA = 0;
  TimeStamp = 0;
  MaxSegmentSize = MAX_MESSAGE_SIZE;                            // The size of the shared memory segment
  Number_Of_Threads_At_Socket_Dispatcher = NUMBER_OF_THREADS_AT_SOCKET_DISPATCHER;  /* This is the number of simultaneous threads that are employed at a certain socket to treat received protocol data units of all technologies */

  // Set the delays
  DelayBeforeRunPeriodic = 5;
  DelayBetweenMessageEmissions = 10;
  DelayBetweenHellos01=1;
  DelayBetweenHellos02=1;

  // Set auxiliary variables
  AwareOfAPS = false;
  AlreadyPublishedBasicBindings = false;

  // The name of my domain
  MyDomainName = "Undefined";

  // Hash of my domain name
  HashOfMyDomainName = "Undefined";

  // The name of my upper level domain
  MyUpperLevelDomainName = "Undefined";

  // Hash of the name of my upper level domain
  HashOfMyUpperLevelDomainName = "Undefined";

#ifdef STATISTICS

  // Allocating the statistics variables
	rtt=new OutputVariable(this);
	tsmidown=new OutputVariable(this);
	tsmidown1=new OutputVariable(this);

	// Setting the statistics variables
	rtt->Initialization("rtt","MEAN_ARITHMETIC","Round trip time for neighbor PGCS (seconds)",1);
	tsmidown->Initialization("tsmidown","MEAN_ARITHMETIC","Delay since message object instantiation (seconds) in the NG processes",1);
	tsmidown1->Initialization("tsmidown1","MEAN_ARITHMETIC","Delay since message object instantiation (seconds) in the NG processes for type 1 messages",1);

#endif

  MessageNumber = 0;
  MessageCounter = 0;
  SequenceNumber = 0;

  srand (static_cast<unsigned int>(time (NULL)));

  // Creating the actions
  NewAction ("-run --initialization 0.1", PA);
  NewAction ("-m --cl 0.1", PA);
  NewAction ("-hello --ihc 0.1", PA);
  NewAction ("-hello --ihc 0.2", PA);
  NewAction ("-hello --ihc 0.3", PA);
  NewAction ("-run --exposition 0.1", PA);
  NewAction ("-run --hello 0.1", PA);
  NewAction ("-run --hello 0.2", PA);
  NewAction ("-run --hello 0.3", PA);
  NewAction ("-run --publishing 0.1", PA);
  NewAction ("-run --periodic 0.1", PA);

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialization", "0.1", PCL);

  // Push the message to the GW input queue
  //PGW->PushToInputQueue(PIM);

  // Run
  Run (PIM, InlineResponseMessage);

  // Mark to delete
  PIM->MarkToDelete ();
}

PG::~PG ()
{
  delete rtt;
  delete tsmidown;
  delete tsmidown1;

  Tuple *Temp = 0;

  vector<Tuple *>::iterator it1;

  for (it1 = PSTuples.begin (); it1 != PSTuples.end (); it1++)
	{
	  Temp = *it1;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}

  vector<Tuple *>::iterator it2;

  for (it2 = PGCSTuples.begin (); it2 != PGCSTuples.end (); it2++)
	{
	  Temp = *it2;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}

  vector<Action *>::iterator it4;

  Action *Temp2 = 0;

  for (it4 = Actions.begin (); it4 != Actions.end (); it4++)
	{
	  Temp2 = *it4;

	  if (Temp2 != 0)
		{
		  delete Temp2;
		}

	  Temp2 = 0;
	}
}

// ************************** Adaptation Layer **************************

// The following code is related to the TCP/IP stack


void PG::GetHostIPAddress (string _Stack, string _Interface, string &_Address)
{
  struct ifaddrs *ifAddrStruct = NULL;
  struct ifaddrs *ifa = NULL;
  void *tmpAddrPtr = NULL;
  int Control = 0;

  getifaddrs (&ifAddrStruct);

  for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
	{
	  if (ifa != 0 && ifa->ifa_addr != 0)
		{
		  if (ifa->ifa_addr->sa_family == AF_INET && (_Stack == "IPv4_UDP"))
			{
			  tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

			  char addressBuffer[INET_ADDRSTRLEN];

			  //cout << "INET_ADDRSTRLEN = " << INET_ADDRSTRLEN << endl;

			  inet_ntop (AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

			  //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);

			  if (ifa->ifa_name == _Interface && Control == 0)
				{
				  char Temp[INET_ADDRSTRLEN];

				  for (int j = 0; j < INET_ADDRSTRLEN; j++)
					{
					  Temp[j] = addressBuffer[j];

					  //printf("%i %c\n",j,Temp[j]);
					}

				  Control = 1;

				  _Address = Temp;
				}
			}
		  else if (ifa->ifa_addr->sa_family == AF_INET6 && (_Stack == "TCPv6" || _Stack == "UDPv6"))
			{
			  tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;

			  char addressBuffer[INET6_ADDRSTRLEN];

			  inet_ntop (AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);

			  //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);

			  if (ifa->ifa_name == _Interface && Control == 0)
				{
				  char Temp[INET6_ADDRSTRLEN];

				  for (int j = 0; j < INET6_ADDRSTRLEN; j++)
					{
					  Temp[j] = addressBuffer[j];

					  //printf("%i %c\n",j,Temp[j]);
					}

				  Control = 1;

				  _Address = Temp;
				}
			}
		}
	}

  if (ifAddrStruct != NULL)
	{
	  freeifaddrs (ifAddrStruct);
	}
}

// ************************** Adaptation Layer **************************

// The following code is related to the Ethernet/Wi-Fi stack

void PG::GetHostRawAddress (string _Interface, string &_Address)
{
  int Size = 13;
  char *ret = new char[Size];
  struct ifreq s;

  int fd = socket (PF_INET, SOCK_DGRAM, IPPROTO_IP);

  strcpy (s.ifr_name, _Interface.c_str ());

  if (fd >= 0 && ret && 0 == ioctl (fd, SIOCGIFHWADDR, &s))
	{
	  int i;
	  for (i = 0; i < 6; ++i)
		snprintf (ret + i * 2, static_cast<size_t>(Size - i * 2), "%02x", (unsigned char)s.ifr_addr.sa_data[i]);
	}

  for (int k = 0; k < 12; k = k + 2)
	{
	  if (k == 10)
		{
		  _Address = _Address + ret[k] + ret[k + 1];
		}
	  else
		{
		  _Address = _Address + ret[k] + ret[k + 1] + ":";
		}
	}

  close (fd);

  delete[] ret;
}

// Create a Raw Ethernet socket
int PG::CreateRawSocket (int &_SID)
{
  int Status = ERROR;
  string Offset = "                    ";
  unsigned short proto = 0x1234;

  // ************************** Adaptation Layer **************************

  _SID = socket (AF_PACKET, SOCK_RAW, htons (proto));

  if (_SID > 0)
	{
	  Status = OK;
	}
  else
	{
	  S << Offset << "(ERROR: Unable to create a new socket. Check if you have root permission.)" << endl;

	  perror ("socket:");
	}

  // **********************************************************************

  return Status;
}

// Write to other OS
int PG::SendToARawSocket (string _Interface, string _Identifier, unsigned int _Size, Message *M)
{
  int Status = ERROR;
  long long MessageSize = 0;
  string Offset = "                    ";
  unsigned int Tentatives = 30;
  unsigned int NoCL = 0;
  bool Verdict = false;
  double Time = 0;
  int numbytes;
  struct sockaddr_ll saddrll;
  vector<string> *SIDs = new vector<string>;
  bool Problem = false;
  unsigned int EffectiveSize;
  unsigned int BlockSize = _Size;
  unsigned int NoS;
  struct ifreq buffer;
  int ifindex;
  unsigned char Source[ETH_ALEN];
  unsigned char Destination[ETH_ALEN];
  unsigned short proto = 0x1234;
  union ethframe frame;
  char TempSource[6][2];
  char TempDestination[6][2];
  unsigned int z = 0;
  PGCS *PPGCS = 0;

  PPGCS = (PGCS *)PP;

  // ************************** Adaptation Layer **************************

  // Address formatting translation

  for (unsigned int x = 0; x < 6; x++)
	{
	  for (unsigned int y = 0; y < 2; y++)
		{
		  TempSource[x][y] = PPGCS->MyMACAddress[y + z];
		}

	  z = z + 3;

	  Hex2Char (TempSource[x], Source[x]);
	}

  z = 0;

  for (unsigned int x = 0; x < 6; x++)
	{
	  for (unsigned int y = 0; y < 2; y++)
		{
		  TempDestination[x][y] = _Identifier[y + z];
		}

	  z = z + 3;

	  Hex2Char (TempDestination[x], Destination[x]);
	}

  memset (&buffer, 0x00, sizeof (buffer));

  strncpy (buffer.ifr_name, _Interface.c_str (), IFNAMSIZ);

  // **********************************************************************

  // Get binding between this PGCS Ethernet Address to the CSID on Category 17
  if (PP->GetHTBindingValues (17, _Identifier, SIDs) == OK)
	{
	  if (SIDs->size () == 1)
		{

		  stringstream Hexadecimal;
		  Hexadecimal << std::hex << MessageNumber;

#ifdef DEBUG
		  S << endl << endl << "[7]                 (Sending the message number " << MessageNumber
			<< " (" << Hexadecimal.str () << ") to the peer PGCS at " << _Identifier
			<< " using the socket identified by " << SIDs->at (0) << ")"
			<< endl;

		  S << "(" << endl << *M << endl << ")" << endl;
#endif

		  if (MessageCounter % 100 == 0)
			{
			  S << endl << Offset << "(Already sent " << MessageCounter << " messages to other peers.)" << endl;
			}

		  if (ioctl (StringToInt (SIDs->at (0)), SIOCGIFINDEX, &buffer) >= 0)
			{
			  ifindex = buffer.ifr_ifindex;

#ifdef DEBUG

			  S << endl << Offset << "(The interface index is = " << ifindex << ")" << endl;

#endif

			  M->ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray ();

			  if (M->GetMessageSize (MessageSize) == OK)
				{
				  if (MessageSize > 0)
					{
					  M->GetNumberofCommandLines (NoCL);

					  unsigned char *HeaderSizeField = new unsigned char[8];
					  char *Payload;
					  unsigned char *SDU = new unsigned char[8 + MessageSize];

					  M->GetMessageFromCharArray (Payload);

					  HeaderSizeField[0] = static_cast<unsigned char>((MessageSize >> (8 * 7)) & 0xff);
					  HeaderSizeField[1] = static_cast<unsigned char>((MessageSize >> (8 * 6)) & 0xff);
					  HeaderSizeField[2] = static_cast<unsigned char>((MessageSize >> (8 * 5)) & 0xff);
					  HeaderSizeField[3] = static_cast<unsigned char>((MessageSize >> (8 * 4)) & 0xff);
					  HeaderSizeField[4] = static_cast<unsigned char>((MessageSize >> (8 * 3)) & 0xff);
					  HeaderSizeField[5] = static_cast<unsigned char>((MessageSize >> (8 * 2)) & 0xff);
					  HeaderSizeField[6] = static_cast<unsigned char>((MessageSize >> (8 * 1)) & 0xff);
					  HeaderSizeField[7] = static_cast<unsigned char>((MessageSize >> (8 * 0)) & 0xff);

					  if (NoCL > 2)
						{

						  for (int j = 0; j < (8 + MessageSize); j++)
							{
							  if (j < 8)
								{
								  SDU[j] = HeaderSizeField[j];
								  //printf("%i %d %c \n",j,SDU[j],SDU[j]);
								}

							  if (j >= 8 && j < (MessageSize + 8))
								{
								  SDU[j] = (unsigned char)Payload[(j - 8)];

								  //printf("%i %d %c \n",j,SDU[j],SDU[j]);
								}
							}

#ifdef DEBUG

						  S << Offset << "(The size of the payload is = " << MessageSize << ")" << endl;
#endif

						  NoS = (unsigned int)ceil ((double)(8 + MessageSize) / (double)BlockSize);

						  //S << Offset << "(The required number of Eth/Wi-Fi frames is = "<<NoS<<")" << endl;

						  // Important: Message need to have random numbers to avoid
						  // conflict with messages numbered on other processes

						  MessageNumber = static_cast<unsigned int>(rand () % 4294967289)
										  + 1; // It never will be zero, therefore zero is problem

						  for (unsigned int i = 0; i < NoS; i++)
							{
							  unsigned char *DataBlock;

							  if (i < (NoS - 1))
								{
								  // Preparing the segment payload for the first segments
								  DataBlock = new unsigned char[BlockSize + 8];

								  unsigned char HeaderSegmentationField[8];

								  HeaderSegmentationField[0] = static_cast<unsigned char>((MessageNumber >> (8 * 3))
																						  & 0xff);
								  HeaderSegmentationField[1] = static_cast<unsigned char>((MessageNumber >> (8 * 2))
																						  & 0xff);
								  HeaderSegmentationField[2] = static_cast<unsigned char>((MessageNumber >> (8 * 1))
																						  & 0xff);
								  HeaderSegmentationField[3] = static_cast<unsigned char>((MessageNumber >> (8 * 0))
																						  & 0xff);
								  HeaderSegmentationField[4] = static_cast<unsigned char>((SequenceNumber >> (8 * 3))
																						  & 0xff);
								  HeaderSegmentationField[5] = static_cast<unsigned char>((SequenceNumber >> (8 * 2))
																						  & 0xff);
								  HeaderSegmentationField[6] = static_cast<unsigned char>((SequenceNumber >> (8 * 1))
																						  & 0xff);
								  HeaderSegmentationField[7] = static_cast<unsigned char>((SequenceNumber >> (8 * 0))
																						  & 0xff);

								  //S << Offset << "(The segmentation header is)"<<endl;

								  for (unsigned int j = 0; j < 8; j++)
									{
									  DataBlock[j] = HeaderSegmentationField[j];

									  //printf("%i %d %c \n",j,DataBlock[j],DataBlock[j]);
									}

								  //S << Offset << "(The remainder of the segment is)"<<endl;

								  for (unsigned int k = 8; k < BlockSize + 8; k++)
									{
									  DataBlock[k] = SDU[i * BlockSize + k - 8];

									  //printf("%i %d %c \n",k,DataBlock[k],DataBlock[k]);
									}

								  EffectiveSize = BlockSize + 8;
								}
							  else
								{
								  // Preparing the segment payload for the last segment
								  unsigned int Remaining = static_cast<unsigned int>((8 + MessageSize)
																					 - (NoS - 1) * BlockSize);

								  DataBlock = new unsigned char[Remaining + 8];

								  unsigned char HeaderSegmentationField[8];

								  HeaderSegmentationField[0] = static_cast<unsigned char>((MessageNumber >> (8 * 3))
																						  & 0xff);
								  HeaderSegmentationField[1] = static_cast<unsigned char>((MessageNumber >> (8 * 2))
																						  & 0xff);
								  HeaderSegmentationField[2] = static_cast<unsigned char>((MessageNumber >> (8 * 1))
																						  & 0xff);
								  HeaderSegmentationField[3] = static_cast<unsigned char>((MessageNumber >> (8 * 0))
																						  & 0xff);
								  HeaderSegmentationField[4] = static_cast<unsigned char>((SequenceNumber >> (8 * 3))
																						  & 0xff);
								  HeaderSegmentationField[5] = static_cast<unsigned char>((SequenceNumber >> (8 * 2))
																						  & 0xff);
								  HeaderSegmentationField[6] = static_cast<unsigned char>((SequenceNumber >> (8 * 1))
																						  & 0xff);
								  HeaderSegmentationField[7] = static_cast<unsigned char>((SequenceNumber >> (8 * 0))
																						  & 0xff);

								  //S << Offset << "(The segmentation header is)"<<endl;

								  for (unsigned int j = 0; j < 8; j++)
									{
									  DataBlock[j] = HeaderSegmentationField[j];

									  //printf("%i %d %c \n",j,DataBlock[j],DataBlock[j]);
									}

								  //S << Offset << "(The remainder of the segment is)"<<endl;

								  for (unsigned int k = 8; k < Remaining + 8; k++)
									{
									  DataBlock[k] = SDU[(NoS - 1) * BlockSize + k - 8];

									  //printf("%i %d %c \n",k,DataBlock[k],DataBlock[k]);
									}

								  EffectiveSize = Remaining + 8;
								}

							  // ************************** Adaptation Layer **************************

							  // The following code is related to Ethernet

							  Tentatives = 12000;    // 2 minutes

							  while (Tentatives > 0)
								{
								  // Configure the Ethernet II address struct
								  memset ((void *)&saddrll, 0, sizeof (saddrll));
								  saddrll.sll_family = PF_PACKET;
								  saddrll.sll_ifindex = ifindex;
								  saddrll.sll_halen = ETH_ALEN;
								  memcpy ((void *)(saddrll.sll_addr), (void *)Destination, ETH_ALEN);

								  // Configure the Ethernet II frame
								  memcpy (frame.field.header.h_dest, Destination, ETH_ALEN);
								  memcpy (frame.field.header.h_source, Source, ETH_ALEN);
								  frame.field.header.h_proto = htons (proto);
								  memcpy (frame.field.data, DataBlock, EffectiveSize);

								  unsigned int frame_len = EffectiveSize + ETH_HLEN;

								  //S << Offset <<"(Message size = "<<MessageSize<<" bytes)"<< endl;

								  //S << Offset <<"(EffectiveSize = "<<EffectiveSize<<" bytes)"<< endl;

								  //S << Offset <<"(Frame length = "<<frame_len<<" bytes)"<< endl;


								  numbytes = static_cast<int>(sendto (StringToInt (SIDs->at (0)), (void *)frame
									  .buffer, frame_len, 0, (struct sockaddr *)&saddrll, sizeof (saddrll)));

								  if (numbytes > 0)
									{

#ifdef DEBUG5
									  S << Offset <<"(The frame with sequence number "<<SequenceNumber<<" was sent with size "<<numbytes<<" bytes at instant " << setprecision (14) << GetTime() << ")"<< endl;
#endif

									  Tentatives = 0;

									  Problem = false;

									  SequenceNumber++;


									}
								  else
									{
									  S << Offset << "(Warning: Unable to send segment. Trying again in 10 ms.)"
										<< endl;

									  perror ("sendto:");

									  Tentatives--;

									  Problem = true;

									  // Sleep between an unsuccessful tentative of sending a segment
									  tthread::this_thread::sleep_for (tthread::chrono::milliseconds (DelayBetweenMessageEmissions));
									}

								  tthread::this_thread::sleep_for (tthread::chrono::microseconds (DelayBetweenMessageEmissions));
								}

							  // ********************************************************************** // Adaptation

							  if (Problem)
								{
								  Status = ERROR;

								  S << Offset << "(ERROR: All the attempts to transfer the message were failed)"
									<< endl;

								  break;
								}
							  else
								{
								  Status = OK;
								}

							  // Sleep between sending segments
							  tthread::this_thread::sleep_for (tthread::chrono::microseconds (DelayBetweenMessageEmissions));

							  // TODO - FIXP/Update - This periodic delay has been added to allow receiving socket buffer reading. In future, should be removed.
							  if (i % 50 == 0)
								{
								  tthread::this_thread::sleep_for (tthread::chrono::milliseconds (15));

								  //S << Offset << "(Adding an extra delay of 50 ms)"
									//<< endl;
								}

							  delete[] DataBlock;
							}

#ifdef DEBUG

						  S << Offset << "(Adding a delay of " << ProportionalDelay << " milliseconds to allow message processing on receiver)" << endl;
#endif

						  if (Status == OK)
							{
							  //S << endl << Offset << "(Message sent)" << endl;

							  MessageCounter++;

							  SequenceNumber = 0;


#ifdef STATISTICS

							  Time = GetTime ();

							  // Sample
															tsmidown->Sample(Time-M->GetInstantiationTime());

															// Update the mean
															tsmidown->CalculateArithmetic();

															// Sample to file
															tsmidown->SampleToFile(Time);

															// **************************************************
															// Type 1 messages statistics
															// **************************************************
															if (M->GetType() == 1)
																{
																	// Sample
																	tsmidown1->Sample(Time-M->GetInstantiationTime());

																	// Update the mean
																	tsmidown1->CalculateArithmetic();

																	// Sample to file
																	tsmidown1->SampleToFile(Time);
																}

#endif

							}
						  else
							{
							  S << Offset << "(ERROR: The message was not send properly)" << endl;
							}
						}
					  else
						{
						  S << Offset << "(ERROR: Any message needs to have at least 3 command lines for IHC)" << endl;
						}

					  delete[] HeaderSizeField;
					  delete[] SDU;

					}
				  else
					{
					  S << Offset << "(ERROR: Message size is zero or negative)" << endl;
					}
				}
			  else
				{
				  S << Offset << "(ERROR: Unable to read message size)" << endl;
				}
			}
		  else
			{
			  S << Offset << "(ERROR: could not get the interface index on the OS)" << endl;

			  perror ("writing socket : ioctl");
			}
		}
	  else
		{
		  S << Offset << "(ERROR: more than one identifier to this address " << _Identifier << ". They are:)" << endl;

		  for (int u = 0; u < SIDs->size (); u++)
			{
			  S << Offset << "(SID [" << u << "] = " << SIDs->at (static_cast<unsigned long>(u)) << ")" << endl;
			}
		}
	}
  else
	{
	  S << Offset << "(ERROR: unable to recover the client socket ID from the PGCS::HT block)" << endl;
	}

  delete SIDs;

  return Status;
}

// TODO: FIXP/Update - This function has suffered many changes
void PG::SocketDispatcher3 ()
{
  struct sockaddr_in addr; // Address structure for legacy link layer
  socklen_t fromlen; // Length of link layer PDU
  PGCS *PPGCS = (PGCS *)PP; // Pointer to the PGCS object
  int SSID = 0; // Server socket ID for any supported socket
  unsigned int receivedbytes = 0; // Number of bytes read from any socket
  unsigned int numbytes = 0; // NG Adaptation Layer PDU. Includes adaptation layer PCI and NovaGenesis layer SDU
  unsigned int BlockSize = 0; // The size of the data block used in NG fragmentation/reassembly. MUST BE smaller than network including all headers
  char Frame[ETH_FRAME_LEN]; // The Ethernet/Wi-Fi frames at link layer
  unsigned int MN; // NG adaptation layer: Message Number or Identifier for unique messages treatment in the network
  unsigned int SN; // NG adaptation layer: Message Fragments Sequence Number for fragmentation and reassembly in the network
  int port; // Employed to determine legacy technology port
  struct sockaddr_in si_me, si_other; // Employed to determine legacy technology addresses
  sem_t *mutex[Number_Of_Threads_At_Socket_Dispatcher]; // Mutex to control access in temporary buffers while dispatching to multiple threads
  string Stack; // Technology employed to transport NG messages
  File F1; // Log file
  string Offset = "                    "; // To format log output
  string EthernetWiFiSemaphoreName[Number_Of_Threads_At_Socket_Dispatcher]; // Name of Ethernet/Wi-Fi semaphores for a number of threads
  long long MessageSize = 0;
  bool NewMessage = true;
  unsigned int BufferIndex = 0;
  vector<MessageReceiving *> Buffer;
  bool Stop = false;

#ifdef DEBUG3

  F1.OpenOutputFile ("EthernetWiFiSocketDispatcher_Thread_Output_.txt", GetPath (), "DEFAULT");

  F1 << endl << Offset << "(Starting running the Ethernet or Wi-Fi socket dispatcher thread)" << endl;

#endif

#ifdef DEBUG6

  F1.OpenOutputFile ("EthernetWiFiSocketDispatcher_Thread_Output_.txt", GetPath (), "DEFAULT");

  F1 << endl << Offset << "(Starting running the Ethernet or Wi-Fi socket dispatcher thread)" << endl;

#endif

  fromlen = sizeof addr; // Get the size of Ethernet/Wi-Fi link layer addresses

  struct sockaddr_ll saddrll; // Initialize the struct for Ethernet/Wi-Fi link layer addresses

  memset ((void *)&saddrll, 0, sizeof (saddrll)); // Initialize the values in the struct for Ethernet/Wi-Fi link layer addresses

  socklen_t sll_len = (socklen_t)sizeof (saddrll); // Get the size of Ethernet/Wi-Fi link layer addresses

  // Initialize Ethernet/Wi-Fi semaphores to temporary buffers
  for (unsigned int f = 0; f < Number_Of_Threads_At_Socket_Dispatcher; f++)
	{
	  EthernetWiFiSemaphoreName[f] = "EthernetWiFi_" + IntToString (f); // Set semaphore names only once
	}

  while (1)
	{
	  for (unsigned int x = 0; x < PPGCS->SSIDs->size (); x++)
		{
		  if (PPGCS->Sizes->size () > 0)
			{
			  BlockSize = PPGCS->Sizes->at (x);
			  Stack = PPGCS->Stacks->at (x);

#ifdef DEBUG3
			  F1 << endl << endl << Offset << "(The socket "<<x<<" for stack "<<Stack<<" has block size = "<<BlockSize<<")" << endl;
#endif

			  if (Stack == "Ethernet" || Stack == "Wi-Fi") // Ethernet or Wi-Fi socket
				{
				  if (PPGCS->MyMACAddress != "") // The Ethernet case
					{
					  // Discover the socket ID
					  SSID = PPGCS->SSIDs->at (x);

#ifdef DEBUG3
					  F1 << endl << Offset << "(The server socket ID (SSID) is "<<SSID<<")" << endl;
#endif

					  receivedbytes = static_cast<unsigned int>(recvfrom (SSID, Frame, ETH_FRAME_LEN, 0, (struct sockaddr *)&saddrll,
																		  &sll_len));

#ifdef DEBUG3
					  // TODO: FIXP/Update - Improving debug
					  F1 << endl << Offset << "(I do received "<<receivedbytes<<" in this socket at instant " << setprecision (12) << GetTime() << ")" << endl;
#endif

					  if (receivedbytes > 0 && saddrll.sll_protocol
											   == 13330) // It is required to check the type of protocol at link layer. It is equivalent to Eth.type 00x1234
						{
						  numbytes = receivedbytes - 14; // Discounts the Ethernet type II header size

						  unsigned char *TempBuffer = new unsigned char[numbytes];

#ifdef DEBUG3
						  F1<< Offset << "(Creating a temporary array of "<<numbytes<<" bytes)" << endl;
#endif

						  for (unsigned int v = 14; v < numbytes + 14; v++)
							{
							  TempBuffer[v - 14] = static_cast<unsigned char>(Frame[v]);
							}

						  MN = 0;
						  SN = 0;

						  // Open the HeaderSegmentationField
						  OpenHeaderSegmentationField (TempBuffer, MN, SN);

						  if (MN > 0 && SN >= 0)
							{
#ifdef DEBUG3
							  F1 << Offset << "(There is a frame from message number " << MN << ")" << endl;
							  F1 << Offset << "(This frame has the sequence number " << SN << ")" << endl;
							  F1 << Offset << "(Number of bytes read from Ethernet/WiFi Dispatcher is " << numbytes << ")" << endl;
#endif

							  NewMessage = true;

							  bool messagePartsInBuffer = false;

							  unsigned int c;

							  for (c = 0; c < Buffer.size (); c++)
								{
								  if (Buffer[c]->MessageNumber == MN)
									{
									  messagePartsInBuffer = true;

									  break;
									}
								}

							  if (messagePartsInBuffer && SN > 0)
								{
								  NewMessage = false;

								  BufferIndex = c;
								}

							  if (!messagePartsInBuffer && SN > 0)
								{
								  // message part not found in buffer
								  // SN > 0
								  // drop message:

#ifdef DEBUG3
								  F1 << Offset << "(ERROR: Previous message fragment(s) is(are) missing)" << endl;
#endif

								  continue;
								}

							  MessageReceiving *PMR = NULL;

							  if (NewMessage) // Threat the case of a new message
								{
								  MessageSize = OpenHeaderMessageSizeField (TempBuffer);

								  if (MessageSize > 0 && MessageSize < MAX_MESSAGE_SIZE)
									{
#ifdef DEBUG3
									  F1 << endl << endl
										 << "[8]                 (Start receiving a new message with message number "
										 << MN << " and size = " << MessageSize
										 << " bytes)" << endl;
#endif

									  // Create a message receiving object to handle the reception
									  PMR = new MessageReceiving;

									  // Allocate an array to receive the NG message
									  char *TheMessage1 = new char[MessageSize];

									  // Initialize the object
									  PMR->Number = SSID;
									  PMR->MessageNumber = MN;
									  PMR->NoS = (unsigned int)ceil (
										  (double)(8 + MessageSize) / (double)BlockSize);
									  PMR->MessageBeingReceived = TheMessage1;
									  PMR->MessageSize = MessageSize;
									  PMR->ReceivedSoFar = (numbytes - 16);
									  PMR->SegmentsSoFar = 1;

									  // TODO - FIXP/Update added this timestamp to implement a TIMEOUT in problematic partially received messages
									  PMR->Timestamp = GetTime ();

									  Buffer.push_back (PMR);

									  BufferIndex = static_cast<unsigned int>(Buffer.size () - 1);

#ifdef DEBUG3

									  F1 << Offset << "(Buffer index " << BufferIndex << ")" << endl;
									  F1 << Offset << "(Receiving " << numbytes << " bytes)" << endl;
									  F1 << Offset << "(Message number " << MN << ")" << endl;
									  F1 << Offset << "(Sequence number " << SN << ")" << endl;
									  F1 << Offset << "(Message size = " << MessageSize << ")" << endl;
									  F1 << Offset << "(Expected number of segments = " << PMR->NoS << ")"
										 << endl;
									  F1 << Offset << "(Received so far = " << PMR->ReceivedSoFar << ")" << endl;
									  F1 << Offset << "(Segments so far = " << PMR->SegmentsSoFar << ")" << endl;
#endif

									  for (unsigned int r = 0; r < (numbytes - 16); r++)
										{
										  TheMessage1[r] = (char)TempBuffer[r + 16];
										}

									  // Stop criteria
									  if (PMR->ReceivedSoFar == MessageSize && PMR->SegmentsSoFar == PMR->NoS)
										{
										  PMR->ContinueReceiving = false;

										  Stop = false;

#ifdef DEBUG3
										  F1 << Offset << "(Stop receiving)" << endl;
#endif
										}
									  else
										{
										  PMR->ContinueReceiving = true;

										  Stop = true;

#ifdef DEBUG3
										  F1 << Offset << "(Continue receiving)" << endl;
#endif
										}
									}
								  else
									{
									  F1 << Offset << "(ERROR: Invalid message size)" << endl;

									  continue;
									}
								}
							  else
								{ // Continuing a previous message!!!
#ifdef DEBUG3
								  F1 << endl << endl
									 << "[9]                 (Continue receiving a message with number " << MN << ")" << endl;

								  F1 << Offset << "(Buffer index " << BufferIndex << ")" << endl;
#endif
								  // *******************************************************************************************
								  // Critical burst control. Take care to change the Stop transferring to child threads control
								  // *******************************************************************************************
								  //TODO: FIXP/Update - Removed the following control that is no longer necessary
								  //if (SN > 1)
								  //	{
								  //	  Stop = true;
								  //	}
								  //else
								  //{
								  //Stop = false;
								  //}

								  PMR = Buffer[BufferIndex];

								  int rsf = static_cast<int>(PMR->ReceivedSoFar);

								  long long Pointer = SN * BlockSize - 8;

								  char *TheMessage2 = PMR->MessageBeingReceived;

								  unsigned int h = 0; // Very important counter because of minimum size Ethernet frames (64bytes)

								  // Recover the total message size to check for finishing reception
								  MessageSize = PMR->MessageSize;

								  for (unsigned int q = 0; q < (numbytes - 8); q++)
									{
									  TheMessage2[Pointer + q] = (char)TempBuffer[q + 8];

#ifdef DEBUG7
									  //printf("%i %d %c \n",q,TempBuffer[q],TempBuffer[q]);
#endif

									  h++;

									  if ((rsf + h) == MessageSize)
										{
										  break;
										}
									}

								  PMR->ReceivedSoFar = rsf + h;
								  PMR->SegmentsSoFar++;

#ifdef DEBUG3
								  F1 << endl << "[9]                 (Message = " << MN
									 << ", # of expected seg. = " << PMR->NoS << ", Segment = "
									 << PMR->SegmentsSoFar << ")" << endl;

								  F1 << Offset << "(Received " << h << " bytes)" << endl;
								  F1 << Offset << "(Message number " << MN << ")" << endl;
								  F1 << Offset << "(Sequence number " << SN << ")" << endl;
								  F1 << Offset << "(Expected number of segments = " << PMR->NoS
									 << ")" << endl;
								  F1 << Offset << "(Segments so far = " << PMR->SegmentsSoFar
									 << ")" << endl;
								  F1 << Offset << "(Received so far = " << PMR->ReceivedSoFar
									 << ")" << endl;
								  F1 << Offset << "(Original message size = " << MessageSize << ")" << endl;
#endif

								  // Stop criteria
								  if (PMR->ReceivedSoFar == PMR->MessageSize &&
									  PMR->SegmentsSoFar == PMR->NoS)
									{
									  PMR->ContinueReceiving = false;

									  // TODO: FIXP/Update - Override stop control to deal with this finished message
									  Stop = false;

#ifdef DEBUG3
									  F1 << Offset << "(Stop receiving)" << endl;
#endif
									}
								  else
									{
									  PMR->ContinueReceiving = true;

									  Stop = true;

#ifdef DEBUG3
									  F1 << Offset << "(Continue receiving)" << endl;
#endif
									}
								}
							}
						}

#ifdef DEBUG
					  F1 << Offset << "(Stop transferring to child threads (0 = false; 1 = true) = " << Stop << ")" << endl;
#endif

					  if (Stop == false)
						{
						  double Beginning_Time = GetTime();

						  // TODO - FIXP/Update - This portion of code inside the loop have been optimized accordingly to new Stop logic
						  for (unsigned int u = 0; u < Buffer.size (); u++)
							{
							  MessageReceiving *PMR3 = Buffer[u];

							  if (PMR3 != NULL)
								{
								  if (!PMR3->ContinueReceiving)
									{
									  TBuffer1 *A = new TBuffer1;

									  A->MessageSize = PMR3->MessageSize;
									  A->TheMessage = PMR3->MessageBeingReceived;

#ifdef DEBUG6
									  F1 << endl << Offset << "(Transferring the message " << PMR3->MessageNumber
										 << " with size "<<PMR3->MessageSize<<" bytes and with "<<PMR3->NoS<<" segments to a child thread. Buffer size is " << Buffer.size ()
										 << ". This message is in buffer with index "<<u<<")" << endl;
#endif

									  // Try up to achieve transferring
									  while(1)
										{
										  unsigned int v = rand () % Number_Of_Threads_At_Socket_Dispatcher;

										  mutex[v] = sem_open (EthernetWiFiSemaphoreName[v].c_str (), O_CREAT, 0666, 1);

										  // Check for error on semaphore open
										  if (mutex[v] == SEM_FAILED)
											{
											  perror ("Reading Ethernet/Wi-Fi Socket : unable to create/open semaphore");

											  sem_unlink (EthernetWiFiSemaphoreName[v].c_str ());
#ifdef DEBUG6
											  F1 << Offset << "(Error on sem_open "<<EthernetWiFiSemaphoreName[v]<<")" << endl;
#endif
											}
										  else
											{
#ifdef DEBUG6
											  F1 << Offset << "(The semaphore "<<EthernetWiFiSemaphoreName[v]<<" has been opened successfully for the transfer)" << endl;
#endif
											  if (sem_trywait (mutex[v]) == 0)
												{

												  // Copy the data finally to the thread that will process it
												  TemporaryBuffers1[EthernetWiFiSemaphoreName[v]].push_back (A);

#ifdef DEBUG6
												  // TODO: FIXP/Update - Improving debug
												  F1 << Offset << "(Deleting Message Receiving Buffer at index = " << u << ")" << endl;
#endif

												  Buffer.erase (Buffer.begin () + u);

												  delete PMR3;

												  if (sem_post (mutex[v])
													  != 0) // Successfully locked the semaphore, so unlock it
													{
													  perror ("Reading Ethernet/Wi-Fi Socket : sem_post");
													}
												  else
													{
#ifdef DEBUG6
													  F1 << Offset << "(The semaphore "<<EthernetWiFiSemaphoreName[v]<<" is unlocked)" << endl;
#endif
													}

												  break;
												}
											  else
												{
#ifdef DEBUG6
												  F1 << Offset << "(The semaphore "<<EthernetWiFiSemaphoreName[v]<<" presented sem_trywait fail)" << endl;
#endif
												}

											  if (sem_close (mutex[v]) != 0)
												{
												  perror ("Reading Ethernet/Wi-Fi Socket : sem_close");
												}
											  else
												{
#ifdef DEBUG6
												  F1 << Offset << "(The semaphore "<<EthernetWiFiSemaphoreName[v]<<" is closed)" << endl;
#endif

												}
											}
										}
									}
									else
									{
									  // TODO: FIXP/Update - Added this case to delete incomplete messages after TIMEOUT
									  if ((GetTime () - PMR3->Timestamp) > TIMEOUT)
										{

#ifdef DEBUG6
										  // TODO: FIXP/Update - Improving debug
										  F1 << Offset << "(Deleting incomplete Message " << PMR3->MessageNumber
											   << " due to TIMEOUT at index = " << u << ")" << endl;
#endif

										  Buffer.erase (Buffer.begin () + u);

										  delete PMR3;
										}
									}
								}
							}
#ifdef DEBUG6
						  F1 << Offset << "(Time spent  "<< setprecision(12) << GetTime()-Beginning_Time<<" seconds)" << endl;
#endif
						}
					}
				}
			}
		}
	}
}

void PG::FinishReceivingThread (unsigned int Index)
{
  string Offset = "                    ";

  long long MessageSize = 0;
  bool NewMessage = true;
  unsigned int BufferIndex = 0;
  char Frame[ETH_FRAME_LEN];
  PGCS *PPGCS = (PGCS *)PP;
  File F1;
  vector<MessageReceiving *> Buffer;
  sem_t *mutex;
  vector<TBuffer1 *> T;

#ifdef DEBUG3

  F1.OpenOutputFile ("EthernetWiFiSocket_Thread_"+IntToString(Index)+"_Output_.txt", GetPath (), "DEFAULT");

  F1 << endl << Offset << "(Starting running the thread " <<Index <<" to receive Ethernet or Wi-Fi frames)" << endl;

#endif

  string SemaphoreName = "EthernetWiFi_" + IntToString (Index);

  tthread::this_thread::sleep_for (tthread::chrono::seconds (1));

  while (1)
	{
	  mutex = sem_open (SemaphoreName.c_str (), O_CREAT, 0666, 1);

	  T.clear (); // Clear the vector of received frames

	  // Check for error on semaphore open
	  if (mutex != SEM_FAILED)
		{
		  if (sem_trywait (mutex) == 0)
			{
			  // ******************************************************************************
			  // Here is what will be done when the semaphore is open for this thread
			  // ******************************************************************************

			  unsigned int VS = TemporaryBuffers1[SemaphoreName].size ();

			  if (VS > 0)
				{
				  for (unsigned x = 0; x < TemporaryBuffers1[SemaphoreName].size (); x++)
					{
					  T.push_back (TemporaryBuffers1[SemaphoreName].at (x));
					}

				  TemporaryBuffers1[SemaphoreName].clear ();
#ifdef DEBUG3
				  F1 << endl << endl << Offset << "(Copied frames to temporary vector)" << endl;
#endif
				}

			  if (sem_post (mutex) != 0) // Successfully locked the semaphore, so unlock it
				{
				  perror ("Reading Ethernet/Wi-Fi Socket : sem_post");
				}
			}

		  if (sem_close (mutex) != 0)
			{
			  perror ("Reading Ethernet/Wi-Fi Socket : sem_close");
			}
		}
	  else
		{
		  perror ("Reading Ethernet/Wi-Fi Socket : unable to create/open semaphore");

		  sem_unlink (SemaphoreName.c_str ());
		}

	  for (unsigned int y = 0; y < T.size (); y++) // Loop over all received messages
		{
#ifdef DEBUG3
		  F1 << endl << Offset << "(--- Delivering message " << y << " to the SMH ---)" << endl;
#endif

		  // Modified in 9th April 2021 to deal with parallel shared memories.
		  while (WriteToSharedMemory3 (&F1, T[y]->TheMessage, T[y]->MessageSize) != OK);
		}

	  for (unsigned int z = 0; z < T.size (); z++) // Loop over all received frames
		{
#ifdef DEBUG3
		  F1 << Offset << "(Deleting temporary buffer " << z << ")" << endl;
#endif
		  delete[] T[z]->TheMessage; // Delete TheMessage from dispatcher thread

		  delete T[z]; // Delete TBuffer1 from dispatcher thread
		}
	}
}

int PG::CreateUDPSocket (string _Type, string _URI)
{
  string Offset = "                    ";
  PGCS *PPGCS = 0;
  int Index = 0;

  PPGCS = (PGCS *)PP;

  // ************************** Adaptation Layer **************************

  // Creates Traditional UDP Sockets

  if (_Type == "PULL")
	{
	  Index = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

  if (_Type == "PUSH")
	{
	  Index = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

  // **********************************************************************

  return Index;
}

// Write to other OS
int PG::SendToAUDPSocket (string _Identifier, unsigned int _Size, Message *M)
{
  int Status = ERROR;    // Negativist approach ;-)
  long long MessageSize = 0;
  string Offset = "                    ";
  unsigned int NoCL = 0;
  bool Verdict = false;
  double Time = 0;
  int numbytes;
  struct sockaddr_ll saddrll;
  vector<string> *SIDs = new vector<string>;
  bool Attempt = false;
  ssize_t Result = 0;
  unsigned int EffectiveSize;
  unsigned int BlockSize = _Size;
  unsigned int NoS;
  struct ifreq buffer;
  int ifindex;
  unsigned int z = 0;
  PGCS *PPGCS = 0;
  int Index = 0;

  int port;
  struct sockaddr_in si_other;
  int slen = sizeof (si_other);

  PPGCS = (PGCS *)PP;

  // Get binding between this PGCS IP:Port to the CSID on Category 17
  if (PP->GetHTBindingValues (17, _Identifier, SIDs) == OK)
	{
	  if (SIDs->size () == 1)
		{

		  Index = StringToInt (SIDs->at (0));

#ifdef DEBUG
		  S << endl << endl << "[7]                 (Sending the message number " << MessageNumber
			<< " to the peer PGCS at " << _Identifier << " using the socket identified by " << SIDs->at (0) << ")"
			<< endl;

		  S << "(" << endl << *M << endl << ")" << endl;
#endif

		  if (MessageCounter % 1000 == 0)
			{
			  S << endl << Offset << "(Already sent " << MessageCounter << " messages to other peers.)" << endl;
			}

		  M->ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray ();

		  if (M->GetMessageSize (MessageSize) == OK)
			{
			  if (MessageSize > 0)
				{
				  M->GetNumberofCommandLines (NoCL);

				  unsigned char *HeaderSizeField = new unsigned char[8];
				  char *Payload;
				  unsigned char *SDU = new unsigned char[8 + MessageSize];

				  M->GetMessageFromCharArray (Payload);

				  HeaderSizeField[0] = static_cast<unsigned char>((MessageSize >> (8 * 7)) & 0xff);
				  HeaderSizeField[1] = static_cast<unsigned char>((MessageSize >> (8 * 6)) & 0xff);
				  HeaderSizeField[2] = static_cast<unsigned char>((MessageSize >> (8 * 5)) & 0xff);
				  HeaderSizeField[3] = static_cast<unsigned char>((MessageSize >> (8 * 4)) & 0xff);
				  HeaderSizeField[4] = static_cast<unsigned char>((MessageSize >> (8 * 3)) & 0xff);
				  HeaderSizeField[5] = static_cast<unsigned char>((MessageSize >> (8 * 2)) & 0xff);
				  HeaderSizeField[6] = static_cast<unsigned char>((MessageSize >> (8 * 1)) & 0xff);
				  HeaderSizeField[7] = static_cast<unsigned char>((MessageSize >> (8 * 0)) & 0xff);

				  if (NoCL > 2)
					{
					  //S << Offset << "(The SDU is)"<<endl;

					  for (int j = 0; j < (8 + MessageSize); j++)
						{
						  if (j < 8)
							{
							  SDU[j] = HeaderSizeField[j];
							  //printf("%i %d %c \n",j,SDU[j],SDU[j]);
							}

						  if (j >= 8 && j < (MessageSize + 8))
							{
							  SDU[j] = (unsigned char)Payload[(j - 8)];

							  //printf("%i %d %c \n",j,SDU[j],SDU[j]);
							}
						}

					  //S << Offset << "(The size of the payload is = "<<MessageSize<<")" << endl;

					  NoS = (unsigned int)ceil ((double)(8 + MessageSize) / (double)BlockSize);

					  //S << Offset << "(The required number of Eth/Wi-Fi frames is = "<<NoS<<")" << endl;

					  // Important Note: Message need to have random numbers to avoid
					  // conflict with messages numbered on other processes

					  MessageNumber = static_cast<unsigned int>(rand () % 4294967290);

					  for (unsigned int i = 0; i < NoS; i++)
						{
						  unsigned char *DataBlock;

						  if (i < (NoS - 1))
							{
							  // Preparing the segment payload for the first segments
							  DataBlock = new unsigned char[BlockSize + 8];

							  unsigned char HeaderSegmentationField[8];

							  HeaderSegmentationField[0] = static_cast<unsigned char>((MessageNumber >> (8 * 3))
																					  & 0xff);
							  HeaderSegmentationField[1] = static_cast<unsigned char>((MessageNumber >> (8 * 2))
																					  & 0xff);
							  HeaderSegmentationField[2] = static_cast<unsigned char>((MessageNumber >> (8 * 1))
																					  & 0xff);
							  HeaderSegmentationField[3] = static_cast<unsigned char>((MessageNumber >> (8 * 0))
																					  & 0xff);
							  HeaderSegmentationField[4] = static_cast<unsigned char>((SequenceNumber >> (8 * 3))
																					  & 0xff);
							  HeaderSegmentationField[5] = static_cast<unsigned char>((SequenceNumber >> (8 * 2))
																					  & 0xff);
							  HeaderSegmentationField[6] = static_cast<unsigned char>((SequenceNumber >> (8 * 1))
																					  & 0xff);
							  HeaderSegmentationField[7] = static_cast<unsigned char>((SequenceNumber >> (8 * 0))
																					  & 0xff);

							  //S << Offset << "(The segmentation header is)"<<endl;

							  for (unsigned int j = 0; j < 8; j++)
								{
								  DataBlock[j] = HeaderSegmentationField[j];

								  //printf("%i %d %c \n",j,DataBlock[j],DataBlock[j]);
								}

							  //S << Offset << "(The remainder of the segment is)"<<endl;

							  for (unsigned int k = 8; k < BlockSize + 8; k++)
								{
								  DataBlock[k] = SDU[i * BlockSize + k - 8];

								  //printf("%i %d %c \n",k,DataBlock[k],DataBlock[k]);
								}

							  EffectiveSize = BlockSize + 8;
							}
						  else
							{
							  // Preparing the segment payload for the last segment
							  unsigned int Remaining = static_cast<unsigned int>((8 + MessageSize)
																				 - (NoS - 1) * BlockSize);

							  DataBlock = new unsigned char[Remaining + 8];

							  unsigned char HeaderSegmentationField[8];

							  HeaderSegmentationField[0] = static_cast<unsigned char>((MessageNumber >> (8 * 3))
																					  & 0xff);
							  HeaderSegmentationField[1] = static_cast<unsigned char>((MessageNumber >> (8 * 2))
																					  & 0xff);
							  HeaderSegmentationField[2] = static_cast<unsigned char>((MessageNumber >> (8 * 1))
																					  & 0xff);
							  HeaderSegmentationField[3] = static_cast<unsigned char>((MessageNumber >> (8 * 0))
																					  & 0xff);
							  HeaderSegmentationField[4] = static_cast<unsigned char>((SequenceNumber >> (8 * 3))
																					  & 0xff);
							  HeaderSegmentationField[5] = static_cast<unsigned char>((SequenceNumber >> (8 * 2))
																					  & 0xff);
							  HeaderSegmentationField[6] = static_cast<unsigned char>((SequenceNumber >> (8 * 1))
																					  & 0xff);
							  HeaderSegmentationField[7] = static_cast<unsigned char>((SequenceNumber >> (8 * 0))
																					  & 0xff);

							  //S << Offset << "(The segmentation header is)"<<endl;

							  for (unsigned int j = 0; j < 8; j++)
								{
								  DataBlock[j] = HeaderSegmentationField[j];

								  //printf("%i %d %c \n",j,DataBlock[j],DataBlock[j]);
								}

							  //S << Offset << "(The remainder of the segment is)"<<endl;

							  for (unsigned int k = 8; k < Remaining + 8; k++)
								{
								  DataBlock[k] = SDU[(NoS - 1) * BlockSize + k - 8];

								  //printf("%i %d %c \n",k,DataBlock[k],DataBlock[k]);
								}

							  EffectiveSize = Remaining + 8;
							}

						  // ************************** Adaptation Layer **************************

						  // Send a NG message as payload of a UDP segment(s) using UDP push.
						  // Added by Alberti in August 2018 for NovaGenesis over GFDM 5G link

						  memset ((char *)&si_other, 0, static_cast<size_t>(slen));

						  si_other.sin_family = AF_INET;

						  string Separation (":");

						  size_t found = _Identifier.find (Separation);

						  string Port = _Identifier.substr (found + 1, _Identifier.size ());

						  port = StringToInt (Port);

						  char *ip = new char[found + 1];

						  string IP = _Identifier.substr (0, found);

						  strcpy (ip, IP.c_str ());

						  si_other.sin_port = htons (static_cast<uint16_t>(port));

#ifdef DEBUG

						  S << Offset << "(Destination IP = " << IP << ")" << endl;
						  S << Offset << "(Destination ip = " << ip << ")" << endl;
						  S << Offset << "(Destination Port = " << Port << ")" << endl;
						  S << Offset << "(Destination port = " << port << ")" << endl;

						  S << Offset << "(Sending fragment number " << SequenceNumber << ")" << endl;
#endif

						  SequenceNumber++;

						  if (inet_aton (ip, &si_other.sin_addr) != 0)
							{
							  try
								{
								  Result = sendto (Index, DataBlock, EffectiveSize, 0, (struct sockaddr *)&si_other, static_cast<socklen_t>(slen));

#ifdef DEBUG

								  S << Offset << "(Result = " << Result << ")" << endl;

#endif

								  if (Result == -1)
									{
									  Attempt = false;

									  Result = 0;
									}
								  else
									{
									  Attempt = true;

									  Result = 0;
									}
								}
							  catch (std::exception &e)
								{
								  perror ("sendto()");

								  std::cerr << "exception caught: " << e.what () << "\n";
								}

							  if (!Attempt)
								{
								  Status = ERROR;

								  S << Offset << "(ERROR: The transfer of the message have failed)" << endl;
								}
							  else
								{
								  Status = OK;
#ifdef DEBUG
								  S << Offset << "(OK!)" << endl;
#endif
								}
							}
						  else
							{
							  S << Offset << "(ERROR: Unable to access UDP push socket using index = " << Index << ")"
								<< endl;

							  perror ("inet_aton()");
							}

						  // ********************************************************************** // Adaptation

						  // Sleep between sending segments
						  tthread::this_thread::sleep_for (tthread::chrono::microseconds (DelayBetweenMessageEmissions));

						  delete[] DataBlock;
						}

					  if (Status == OK)
						{
						  //S << endl << Offset << "(Message sent)" << endl;

						  MessageCounter++;

						  SequenceNumber = 0;

						  Time = GetTime ();

#ifdef STATISTICS

						  // Sample
													tsmidown->Sample(Time-M->GetInstantiationTime());

													// Update the mean
													tsmidown->CalculateArithmetic();

													// Sample to file
													tsmidown->SampleToFile(Time);

													// **************************************************
													// Type 1 messages statistics
													// **************************************************
													if (M->GetType() == 1)
														{
															// Sample
															tsmidown1->Sample(Time-M->GetInstantiationTime());

															// Update the mean
															tsmidown1->CalculateArithmetic();

															// Sample to file
															tsmidown1->SampleToFile(Time);
														}

#endif
						}
					  else
						{
						  S << Offset << "(ERROR: The message was not send properly)" << endl;
						}
					}
				  else
					{
					  S << Offset << "(ERROR: Any message needs to have at least 3 command lines for IHC)" << endl;
					}

				  delete[] HeaderSizeField;
				  delete[] SDU;

				}
			  else
				{
				  S << Offset << "(ERROR: Message size is zero or negative)" << endl;
				}
			}
		  else
			{
			  S << Offset << "(ERROR: Unable to read message size)" << endl;
			}

		}
	  else
		{
		  S << Offset << "(ERROR: Unexpected number of socket IDs related to the IP address " << _Identifier << ")"
			<< endl;
		}
	}
  else
	{
	  S << Offset << "(ERROR: unable to recover the client socket ID from the PGCS::HT block)" << endl;
	}

  delete SIDs;

  return Status;
}

void PG::ReceiveFromAUDPSocket ()
{
  string Offset = "                    ";
  unsigned int receivedbytes;
  struct sockaddr_in addr;
  int SSID = 0;
  long long MessageSize = 0;
  unsigned int BlockSize = 0;
  socklen_t fromlen;
  unsigned int MN;
  unsigned int SN;
  bool NewMessage = true;
  unsigned int BufferIndex;
  PGCS *PPGCS = (PGCS *)PP;
  File F1;
  int shmid = 0;
  vector<MessageReceiving *> Buffer;

  unsigned int numbytes = 0;
  int port;
  struct sockaddr_in si_me, si_other;

#ifdef DEBUG

  F1.OpenOutputFile ("PG_ReceiveFromAUDPSocket_Thread_Output_.txt", GetPath (), "DEFAULT");

  F1 << endl << Offset << "(Starting running the thread to receive UDP segments)" << endl;

#endif

  fromlen = sizeof addr;

  tthread::this_thread::sleep_for (tthread::chrono::seconds (5));

  port = StringToInt (PPGCS->Port.c_str ());

  for (unsigned int x = 0; x < PPGCS->SSIDs->size (); x++)
	{

	  BlockSize = PPGCS->Sizes->at (x);

	  // Discover the socket ID
	  SSID = PPGCS->SSIDs->at (x);

#ifdef DEBUG

	  F1 << Offset << "(SSID = " << SSID << ")" << endl;

#endif

	  memset ((char *)&si_me, 0, sizeof (si_me));

	  si_me.sin_family = AF_INET;
	  si_me.sin_port = htons (static_cast<uint16_t>(port));
	  si_me.sin_addr.s_addr = htonl (INADDR_ANY);

	  //bind socket to port
	  if (bind (SSID, (struct sockaddr *)&si_me, sizeof (si_me)) != -1)
		{
		  F1 << Offset << "(Successfully bound to socket= " << SSID << ")" << endl;
		}
	  else
		{
		  F1 << Offset << "(ERROR: Unable to bind socket to UDP port using index = " << x << ")" << endl;

		  perror ("Bind()");
		}
	}

  while (1)
	{
	  for (unsigned int x = 0; x < PPGCS->SSIDs->size (); x++)
		{

		  BlockSize = PPGCS->Sizes->at (x);

		  // Discover the socket ID
		  SSID = PPGCS->SSIDs->at (x);

#ifdef DEBUG

		  F1 << Offset << "(SSID = " << SSID << ")" << endl;

#endif

		  // ************************** Adaptation Layer **************************

		  // Receives from a UDP pull socket using traditional UDP Socket

		  memset ((char *)&si_me, 0, sizeof (si_me));

		  si_me.sin_family = AF_INET;
		  si_me.sin_port = htons (static_cast<uint16_t>(port));
		  si_me.sin_addr.s_addr = htonl (INADDR_ANY);

		  socklen_t sll_len = (socklen_t)sizeof (si_other);

		  unsigned char TempBuffer[MaxSegmentSize];

		  try
			{
			  numbytes = static_cast<unsigned int>(recvfrom (SSID, TempBuffer, MaxSegmentSize, 0, (struct sockaddr *)&si_other, &sll_len));

#ifdef DEBUG

			  F1 << Offset << "(numbytes = " << numbytes << ")" << endl;

#endif
			}
		  catch (std::exception &e)
			{
			  perror ("recvfrom()");

			  std::cerr << "exception caught: " << e.what () << "\n";
			}

		  if (numbytes > 0)
			{
			  // **********************************************************************



			  //string Payload = string(static_cast<char*>(MESSAGE.data()), MESSAGE.size());

			  //numbytes=MESSAGE.size();

#ifdef DEBUG
			  F1 << endl << endl << "[7]                 (I do received a packet with " << numbytes
				 << " bytes from socket identified as " << SSID << " with block size of " << BlockSize << " bytes)"
				 << endl;

			  F1 << endl;

			  for (unsigned int v = 0; v < numbytes; v++)
				{
				  F1 << TempBuffer[v];
				}

			  F1 << endl;

#endif

			  // Open the HeaderSegmentationField
			  OpenHeaderSegmentationField (TempBuffer, MN, SN);

			  if (MN >= 0 && SN >= 0)
				{

#ifdef DEBUG
				  F1 << Offset << "(There is a frame from message number " << MN << ")" << endl;
				  F1 << Offset << "(This frame has the sequence number " << SN << ")" << endl;
#endif

				  NewMessage = true;
				  MessageSize = 0;
				  bool messagePartsInBuffer = false;
				  unsigned int c;

				  for (c = 0; c < Buffer.size (); c++)
					{
					  if (Buffer[c]->MessageNumber == MN)
						{
						  messagePartsInBuffer = true;
						  break;
						}
					}

				  if (MN >= 0 && SN > 0)
					{
					  NewMessage = false;
					  BufferIndex = c;
					}

				  if (!messagePartsInBuffer && SN > 0)
					{
					  // message part not found in buffer
					  // SN > 0
					  // drop message:

#ifdef DEBUG
					  F1 << Offset
						 << "(ERROR: Message fragment(s) already received, but message is not a receiving buffer array)"
						 << endl;
#endif

					  continue;
					}

				  MessageReceiving *PMR2 = NULL;

				  if (NewMessage)
					{
					  MessageSize = OpenHeaderMessageSizeField (TempBuffer);

					  if (MessageSize > 0 && MessageSize < MAX_MESSAGE_SIZE)
						{

#ifdef DEBUG
						  F1 << endl << endl
							 << "[8]                 (Start receiving a new message with message number " << MN
							 << " at socket " << SSID << " with size = " << MessageSize << " bytes)" << endl;
#endif

						  // Create a message receiving object to handle the reception
						  MessageReceiving *PMR1 = new MessageReceiving;

						  // Allocate an array to receive the NG message
						  char *TheMessage1 = new char[MessageSize];

						  // Initialize the object
						  PMR1->Number = SSID;
						  PMR1->MessageNumber = MN;
						  PMR1->NoS = (unsigned int)ceil ((double)(8 + MessageSize) / (double)BlockSize);
						  PMR1->MessageBeingReceived = TheMessage1;
						  PMR1->MessageSize = MessageSize;
						  PMR1->ReceivedSoFar = (numbytes - 16);
						  PMR1->SegmentsSoFar = 1;

						  Buffer.push_back (PMR1);

						  BufferIndex = static_cast<unsigned int>(Buffer.size () - 1);
#ifdef DEBUG
						  F1 << Offset << "(Buffer index " << BufferIndex << ")" << endl;
						  F1 << Offset << "(Receiving " << numbytes << " bytes)" << endl;
						  F1 << Offset << "(Message number " << MN << ")" << endl;
						  F1 << Offset << "(Sequence number " << SN << ")" << endl;
						  F1 << Offset << "(Message size = " << MessageSize << ")" << endl;
						  F1 << Offset << "(Expected number of segments = " << PMR1->NoS << ")" << endl;
						  F1 << Offset << "(Received so far = " << PMR1->ReceivedSoFar << ")" << endl;
						  F1 << Offset << "(Segments so far = " << PMR1->SegmentsSoFar << ")" << endl;
#endif

						  for (unsigned int r = 0; r < (numbytes - 16); r++)
							{
							  TheMessage1[r] = (char)TempBuffer[r + 16];
							}

						  // Stop criteria
						  if (PMR1->ReceivedSoFar == MessageSize && PMR1->SegmentsSoFar == PMR1->NoS)
							{
							  PMR1->ContinueReceiving = false;

#ifdef DEBUG
							  F1 << Offset << "(Stop receiving)" << endl;
#endif
							}
						  else
							{
							  PMR1->ContinueReceiving = true;

#ifdef DEBUG
							  F1 << Offset << "(Continue receiving)" << endl;
#endif
							}
						}
					  else
						{
						  F1 << Offset << "(ERROR: Invalid message size)" << endl;

						  continue;
						}
					}
				  else
					{ // Continue receiving a message!!

#ifdef DEBUG
					  F1 << endl << endl << "[9]                 (Continue receiving a message with number " << MN
						 << " at socket " << SSID << ")" << endl;

					  F1 << Offset << "(Buffer index " << BufferIndex << ")" << endl;
#endif

					  int rsf = static_cast<int>(Buffer[BufferIndex]->ReceivedSoFar);

					  long long Pointer = SN * BlockSize - 8;

					  PMR2 = Buffer[BufferIndex];

					  char *TheMessage2 = PMR2->MessageBeingReceived;

					  unsigned int h = 0; // Very important counter because of minimum size Ethernet frames (64bytes)

					  // Recover the total message size to check for finishing reception
					  MessageSize = PMR2->MessageSize;

					  for (unsigned int q = 0; q < (numbytes - 8); q++)
						{
						  TheMessage2[Pointer + q] = (char)TempBuffer[q + 8];

#ifdef DEBUG
						  //printf("%i %d %c \n",q,TempBuffer[q],TempBuffer[q]);
#endif

						  h++;

						  if ((rsf + h) == MessageSize)
							{
							  break;
							}
						}

					  PMR2->ReceivedSoFar = rsf + h;
					  PMR2->SegmentsSoFar++;

#ifdef DEBUG
					  F1 << endl << "[9]                 (Message = " << MN << ", # of expected seg. = "
						 << Buffer[BufferIndex]->NoS << ", Segment = " << Buffer[BufferIndex]->SegmentsSoFar << ")"
						 << endl;

					  F1 << Offset << "(Received " << h << " bytes)" << endl;
					  F1 << Offset << "(Message number " << MN << ")" << endl;
					  F1 << Offset << "(Sequence number " << SN << ")" << endl;
					  F1 << Offset << "(Expected number of segments = " << Buffer[BufferIndex]->NoS << ")" << endl;
					  F1 << Offset << "(Segments so far = " << Buffer[BufferIndex]->SegmentsSoFar << ")" << endl;
					  F1 << Offset << "(Received so far = " << Buffer[BufferIndex]->ReceivedSoFar << ")" << endl;
					  F1 << Offset << "(Original message size = " << MessageSize << ")" << endl;
#endif

					  // Stop criteria
					  if (PMR2->ReceivedSoFar == PMR2->MessageSize
						  && PMR2->SegmentsSoFar == PMR2->NoS)
						{
						  PMR2->ContinueReceiving = false;

#ifdef DEBUG
						  F1 << Offset << "(Stop receiving)" << endl;
#endif
						}
					  else
						{
						  PMR2->ContinueReceiving = true;

#ifdef DEBUG
						  F1 << Offset << "(Continue receiving)" << endl;
#endif
						}
					}

				  if (!PMR2->ContinueReceiving)
					{
#ifdef DEBUG
					  F1 << Offset << "(Going to write to the shared memory identified by " << shmid << ")" << endl;
#endif
					  char *TheMessage3 = PMR2->MessageBeingReceived;

					  // Modified in 9th April 2021 to deal with parallel shared memories.
					  while (WriteToSharedMemory3 (&F1, TheMessage3, MessageSize) != OK);

#ifdef DEBUG

					  F1 << Offset << "(Going to delete the temporary message char array of index " << BufferIndex
						 << ")" << endl;
#endif

					  delete[] TheMessage3;

#ifdef DEBUG
					  F1 << Offset << "(Deleted)" << endl;
#endif

					  Buffer.erase (Buffer.begin () + BufferIndex);

					  delete PMR2;
					}

				  MN = 0;

				  SN = 0;
				}
			  else
				{
				  F1 << Offset << "(ERROR: Unable to read the message number and sequence number)" << endl;
				}
			}
		  else
			{
			  F1 << Offset << "(ERROR: unable to pull from UDP socket)" << endl;
			}
		}
	}
}

// ************************** Adaptation Layer **************************

// The following code was necessary to avoid concurrency problems in different threads.
// Instead of writing to the GW input queue directly, write to shared memory instead. The GW reads from shared avoid concurrency.
// Obviously, this should be replaced in future.


// Modified in 9th April 2021 to deal with parallel shared memories.
int PG::WriteToSharedMemory3 (File *_PF, char *_MessageCharArray, long long _MessageSize)
{
  int Status = ERROR;
  string Offset = "          ";

  for (int z = 0; z < NUMBER_OF_PARALLEL_SHARED_MEMORIES;
	   z++) // Modified in 9th April 2021 to deal with parallel shared memories.
	{
	  long long TotalSize = 0;    // The total size of the message being transferred
	  unsigned char *data;        // Manipulate the content on shared memory
	  void *shm_address;        // The shared memory segment address
	  string SemaphoreName;
	  sem_t *mutex;
	  vector<string> *Values = new vector<string>;

	  if (PP->shmid[z] > 0)
		{
		  int _shm_key = 11 + z;

#ifdef DEBUG3

		  *_PF << "Writing to shared memory with key "<< _shm_key<<" and id = " << PP->shmid[z] << " at PG)" << endl;

#endif

		  // Set the semaphore name
		  SemaphoreName = IntToString (_shm_key);

		  mutex = sem_open (SemaphoreName.c_str (), O_CREAT, 0666, 1);

		  // Check for error on semaphore open
		  if (mutex != SEM_FAILED)
			{

			  // ***************************************************************************************
			  // Create a local process HT binding relating PGCS shared memory IPC key to its shmid
			  // ***************************************************************************************

#ifdef DEBUG2
			  *_PF << Offset << "(Opened the semaphore " << SemaphoreName << ")" << endl;
#endif

			  if ((shm_address = shmat (PP->shmid[z], NULL, 0)) != NULL)
				{

				  if (sem_trywait (mutex) == 0)
					{

#ifdef DEBUG2
					  *_PF << Offset << "(Locked the semaphore " << SemaphoreName << ")" << endl;
#endif

					  data = (unsigned char *)shm_address;

					  if (data != (unsigned char *)(-1))
						{
						  if (data[0] != (unsigned char)'f' && data[0] != (unsigned char)'w')
							{
							  data[0] = (unsigned char)'f';
#ifdef DEBUG2
							  *_PF << Offset
								   << "(Initialized the shared memory with key 11 regarding the r/w control flag)"
								   << endl;
#endif
							}
						}

#ifdef DEBUG2
					  *_PF << "          (The value of the first byte in shared memory is " << (char)data[0]
						   << " (f = Free to write, w = still waiting for peer reading)" << endl;
					  *_PF << "          (First byte is " << (char)data[0] << ")" << endl;
#endif

					  if (data[0] == (unsigned char)'f')
						{

#ifdef DEBUG2
						  *_PF << "[5]       (Writing a message to the shared memory segment with key "<<_shm_key<<" identified by "
							   << PP->shmid[z] << ")" << endl;
#endif

						  TotalSize = _MessageSize + 8;

						  unsigned char *HeaderSizeField = new unsigned char[TotalSize];
						  unsigned char *PDU = new unsigned char[TotalSize];

						  HeaderSizeField[0] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 7)) & 0xff);
						  HeaderSizeField[1] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 6)) & 0xff);
						  HeaderSizeField[2] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 5)) & 0xff);
						  HeaderSizeField[3] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 4)) & 0xff);
						  HeaderSizeField[4] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 3)) & 0xff);
						  HeaderSizeField[5] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 2)) & 0xff);
						  HeaderSizeField[6] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 1)) & 0xff);
						  HeaderSizeField[7] = static_cast<unsigned char>((unsigned char)(TotalSize >> (8 * 0)) & 0xff);

						  for (long long j = 0; j < TotalSize; j++)
							{
							  if (j < 8)
								{
								  PDU[j] = HeaderSizeField[j];
								  //printf("%i %d %c \n",j,PDU[j],PDU[j]);
								}

							  if (j >= 8 && j < (_MessageSize + 8))
								{
								  PDU[j] = (unsigned char)_MessageCharArray[(j - 8)];
								  //printf("%i %d %c \n",j,PDU[j],PDU[j]);
								}
							}

						  if (TotalSize <= (long long)MaxSegmentSize)
							{
#ifdef DEBUG3
							  *_PF << Offset << "(The size of the payload is = " << _MessageSize << ")" << endl;

							  *_PF << Offset << "(The size of the total message is = " << TotalSize << ")" << endl;

							  *_PF << Offset << "(The size of the memory segment is = " << MaxSegmentSize << ")"
								   << endl;

							  //S << Offset << "(Make the shared memory unavailable while not read by the peer)"<< endl;
#endif

							  // Clean the memory segment before writing
							  // April 2nd, 2021: changed from MaxSegmentSize to 1, since there is not need to clean more than this
							  memset (data, (unsigned char)'w',
									  1);

							  for (long long k = 1; k < TotalSize + 1; k++)
								{
								  data[k] = PDU[k - 1];
								}

							  // Send the timestamp of this operation after the message

							  unsigned long long Time = (unsigned long long)(GetTime () * 10e9);

							  unsigned char *HeaderTimeStampField = new unsigned char[8];

							  HeaderTimeStampField[0] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 7))
																				   & 0xff);
							  HeaderTimeStampField[1] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 6))
																				   & 0xff);
							  HeaderTimeStampField[2] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 5))
																				   & 0xff);
							  HeaderTimeStampField[3] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 4))
																				   & 0xff);
							  HeaderTimeStampField[4] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 3))
																				   & 0xff);
							  HeaderTimeStampField[5] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 2))
																				   & 0xff);
							  HeaderTimeStampField[6] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 1))
																				   & 0xff);
							  HeaderTimeStampField[7] = static_cast<unsigned char>((unsigned char)(Time >> (8 * 0))
																				   & 0xff);

							  for (unsigned int n = 0; n < 8; n++)
								{
								  data[TotalSize + 1 + n] = HeaderTimeStampField[n];
								}

							  delete[] HeaderTimeStampField;

							  Status = OK;
							}
						  else
							{
							  *_PF << Offset
								   << "(ERROR: The message is too big for the memory segment or it has less than 3 command lines)"
								   << endl;
							}

						  delete[] PDU;
						  delete[] HeaderSizeField;

						} // (data[0] != (unsigned char)'f') means the memory is not free to write, despite of locking the semaphore.
					  else
						{
#ifdef DEBUG
						  *_PF << Offset
							   << "(Warning: The peer still does not read the shared memory. Trying again later)"
							   << endl;
#endif

						  Status = ERROR;

						  // Sleep before trying to send in the shared memory again
						  tthread::this_thread::sleep_for (tthread::chrono::microseconds (5));
						}

					  if (sem_post (mutex) == 0) // Successfully locked the semaphore, so unlock it
						{
#ifdef DEBUG2
						  *_PF << Offset << "(Unlocked the semaphore " << SemaphoreName << ")" << endl;
#endif
						}
					  else
						{
						  perror ("writing SHM : sem_post");
						}

					} // (sem_trywait(mutex) != 0) means UNABLE TO LOCK. If someone locked, it is normal to get this condition
				  else
					{
#ifdef DEBUG1
					  //perror("writing SHM : unable to lock the semaphore");
#endif
					}

				  // Sleep while wait for shared memory semaphore
				  tthread::this_thread::sleep_for (tthread::chrono::microseconds (5));


				  // Detach from shared memory, if attached
				  if (shmdt (shm_address) == -1)
					{
					  perror ("writing SHM : shmdt");
					}

				} // ((shm_address = shmat(shmid, NULL, 0)) == NULL) means UNABLE TO ATTACH
			  else
				{
				  perror ("writing SHM : semop");
				}

			  if (sem_close (mutex) == 0)
				{
#ifdef DEBUG2
				  *_PF << Offset << "(Closed the semaphore " << SemaphoreName << ")" << endl;
#endif
				}
			  else
				{
				  perror ("writing SHM : sem_close");
				}

			} //(mutex != SEM_FAILED)
		  else
			{
			  perror ("writing SHM : unable to open/create the semaphore");

			  sem_unlink (SemaphoreName.c_str ());
			}

		} //(shmid <= 0)
	  else
		{
		  *_PF << Offset << "(ERROR: Unable to determine shared memory ID in operating system)" << endl;
		}

	  delete Values;

	  if (Status == OK)
		{
		  break;
		}
	}

  //S << Offset << "(Finished)"<< endl;

  return Status;
}

long long int PG::OpenHeaderMessageSizeField (unsigned char *_Buffer)
{
  long long MessageSize = 0;

  unsigned long long A = _Buffer[8];
  unsigned long long B = _Buffer[9];
  unsigned long long C = _Buffer[10];
  unsigned long long D = _Buffer[11];
  unsigned long long E = _Buffer[12];
  unsigned long long F = _Buffer[13];
  unsigned long long G = _Buffer[14];
  unsigned long long H = _Buffer[15];

  //cout << "A = " <<A<<endl;
  //cout << "B = " <<B<<endl;
  //cout << "C = " <<C<<endl;
  //cout << "D = " <<D<<endl;
  //cout << "E = " <<E<<endl;
  //cout << "F = " <<F<<endl;
  //cout << "G = " <<G<<endl;
  //cout << "H = " <<H<<endl;

  MessageSize = (A << 56) |
				(B << 48) |
				(C << 40) |
				(D << 32) |
				(E << 24) |
				(F << 16) |
				(G << 8) |
				H;

  return MessageSize;
}

void
PG::OpenHeaderSegmentationField (unsigned char *_Buffer, unsigned int &_MessageNumber, unsigned int &_SequenceNumber)
{
  unsigned int A = _Buffer[0];
  unsigned int B = _Buffer[1];
  unsigned int C = _Buffer[2];
  unsigned int D = _Buffer[3];
  unsigned int E = _Buffer[4];
  unsigned int F = _Buffer[5];
  unsigned int G = _Buffer[6];
  unsigned int H = _Buffer[7];

  //cout << "A = " <<A<<endl;
  //cout << "B = " <<B<<endl;
  //cout << "C = " <<C<<endl;
  //cout << "D = " <<D<<endl;
  //cout << "E = " <<E<<endl;
  //cout << "F = " <<F<<endl;
  //cout << "G = " <<G<<endl;
  //cout << "H = " <<H<<endl;

  _MessageNumber = (A << 24) |
				   (B << 16) |
				   (C << 8) |
				   D;

  _SequenceNumber = (E << 24) |
					(F << 16) |
					(G << 8) |
					H;
}

// Allocate and add an Action on Actions container
void PG::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-m --cl 0.1")
	{
	  PGMsgCl01 *P = new PGMsgCl01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --initialization 0.1")
	{
	  PGRunInitialization01 *P = new PGRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-hello --ihc 0.1")
	{
	  PGHelloIHC01 *P = new PGHelloIHC01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-hello --ihc 0.2")
	{
	  PGHelloIHC02 *P = new PGHelloIHC02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-hello --ihc 0.3")
	{
	  PGHelloIHC03 *P = new PGHelloIHC03 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --exposition 0.1")
	{
	  PGRunExposition01 *P = new PGRunExposition01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --hello 0.1")
	{
	  PGRunHello01 *P = new PGRunHello01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --hello 0.2")
	{
	  PGRunHello02 *P = new PGRunHello02 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --hello 0.3")
	{
	  PGRunHello03 *P = new PGRunHello03 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --publishing 0.1")
	{
	  PGRunPublishing01 *P = new PGRunPublishing01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-run --periodic 0.1")
	{
	  PGRunPeriodic01 *P = new PGRunPeriodic01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int PG::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int PG::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}

// Wrapper function for ReceiveFromAnIPv4TCPSocket() thread
void PG::ReceiveFromAUDPSocketThreadWrapper (void *_PPG)
{
  PG *PPG = static_cast<PG *>(_PPG);

  PPG->ReceiveFromAUDPSocket ();
}

// Wrapper function for EthernetWiFiSocketDispatcher() thread
void PG::EthernetWiFiSocketDispatcherThreadWrapper (void *_PPG)
{
  PG *PPG = static_cast<PG *>(_PPG);

  PPG->SocketDispatcher3 ();
}

// Wrapper function for ReceiveFromARawSocketThread() thread
void PG::FinishReceivingThreadWrapper (void *_Param)
{
  PARAMETERS1 *params = (PARAMETERS1 *)_Param;

  PG *PPG = static_cast<PG *>(params->_PPG);

  PPG->FinishReceivingThread (params->_Index);
}

// get sockaddr, IPv4 or IPv6:
void *PG::get_in_addr (struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
	{
	  return &(((struct sockaddr_in *)sa)->sin_addr);
	}

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Reset all statistics
void PG::ResetStatistics ()
{
  rtt->Reset ();
}

void PG::Hex2Char (char *szHex, unsigned char &rch)
{
  rch = 0;
  for (int i = 0; i < 2; i++)
	{
	  if (*(szHex + i) >= '0' && *(szHex + i) <= '9')
		rch = (rch << 4) + (*(szHex + i) - '0');
	  else if (*(szHex + i) >= 'a' && *(szHex + i) <= 'f')
		rch = static_cast<unsigned char>((rch << 4) + (*(szHex + i) - 'a' + 10));
	  else
		break;
	}
}

