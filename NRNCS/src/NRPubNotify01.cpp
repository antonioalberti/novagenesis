/*
	NovaGenesis

	Name:		NRPubNotify01
	Object:		NRPubNotify01
	File:		NRPubNotify01.cpp
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

#ifndef _NRPUBNOTIFY01_H
#include "NRPubNotify01.h"
#endif

#ifndef _NR_H
#include "NR.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _NRNCS_H
#include "NRNCS.h"
#endif

// TODO: FIXP/Update - Added this debug control
#define DEBUG

NRPubNotify01::NRPubNotify01 (string _LN, Block *_PB, MessageBuilder *_PMB) : Action (_LN, _PB, _PMB)
{
}

NRPubNotify01::~NRPubNotify01 ()
{
}

// Run the actions behind a received command line
// ng -p --notify _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > < _PubNotifySize string pub HID OSID PID BID > ...
//                             < _SubNotifySize string sub HID OSID PID BID > ]

int
NRPubNotify01::Run (Message *_ReceivedMessage, CommandLine *_PCL, vector<Message *> &ScheduledMessages, Message *&InlineResponseMessage)
{
  int Status = OK;
  string Offset = "                    ";
  unsigned int NA = 0;
  vector<string> Category;
  vector<string> Key;
  vector<string> Values;
  vector<string> Notification;
  CommandLine *PCL = 0;
  string Type;
  string HID;
  string OSID;
  string PID;
  string BID;
  Message *Notify;
  vector<string> Limiters;
  vector<string> Sources;
  vector<string> Destinations;
  NR *PNR = 0;

  PNR = (NR *)PB;

  //PB->S << Offset <<  this->GetLegibleName() << endl;

  // Load the number of arguments
  if (_PCL->GetNumberofArguments (NA) == OK)
	{
	  // Check the number of arguments
	  if (NA >= 4)
		{
		  // Get received command line arguments
		  if (_PCL->GetArgument (0, Category) == OK && _PCL->GetArgument (1, Key) == OK
			  && _PCL->GetArgument (2, Values) == OK)
			{
			  if (Category.size () > 0 && Key.size () > 0 && Values.size ())
				{
				  if (PB->State == "operational")
					{
					  // Change command line from ng -p --b to ng -sr --b
					  PMB->NewCommonCommandLine ("-sr", "--b", "0.1", PB->StringToInt (Category.at (0)), Key
						  .at (0), &Values, InlineResponseMessage, PCL);

					  // Get received pub/sub notification tuples
					  for (unsigned int i = 3; i < NA; i++)
						{
						  _PCL->GetArgument (i, Notification);

						  if (Notification.size () == 5)
							{
							  Type = Notification.at (0);
							  HID = Notification.at (1);
							  OSID = Notification.at (2);
							  PID = Notification.at (3);
							  BID = Notification.at (4);

							  if (Type == "pub")
								{
								  // **********************************************************************************
								  // Send a ng -notify 0.1 to the indicated tuple
								  // **********************************************************************************

								  //PB->S << Offset <<  "(Scheduling a notification message to the Peer entity)" << endl;

								  // Setting up the DID as the space limiter
								  Limiters.push_back (PB->PP->Intra_Domain);

								  // Setting up the this OS as the 1st source SCN
								  Sources.push_back (PB->PP->GetHostSelfCertifyingName ());

								  // Setting up the this OS as the 2nd source SCN
								  Sources.push_back (PB->PP->GetOperatingSystemSelfCertifyingName ());

								  // Setting up the this process as the 3rd source SCN
								  Sources.push_back (PB->PP->GetSelfCertifyingName ());

								  // Setting up the NR block SCN as the 4th source SCN
								  Sources.push_back (PB->GetSelfCertifyingName ());

								  // Setting up the destination
								  Destinations.push_back (HID);

								  // Setting up the destination
								  Destinations.push_back (OSID);

								  // Setting up the destination
								  Destinations.push_back (PID);

								  // Setting up the destination
								  Destinations.push_back (BID);

								  // Creating a new message
								  PB->PP->NewMessage (
									  GetTime () + PNR->DelayBeforeSendingANotification, 0, false, Notify);

								  // ***************************************************
								  // Generate the ng -m --cl command Line
								  // ***************************************************

								  PMB->NewConnectionLessCommandLine ("0.1", &Limiters, &Sources, &Destinations, Notify, PCL);

								  PMB->NewPubNotificationCommandLine ("0.1", PB->StringToInt (Category.at (0)), &Key, &PNR
									  ->Publisher, Notify, PCL);

								  // ***************************************************
								  // Generate the ng -scn --seq command Line
								  // ***************************************************

								  // Generate the SCN
								  PB->GenerateSCNFromMessageBinaryPatterns (Notify, SCN);

								  // Creating the ng -scn --s command line
								  PMB->NewSCNCommandLine ("0.1", SCN, Notify, PCL);

								  // ******************************************************
								  // Finishing
								  // ******************************************************

								  // Push the message to the GW input queue
								  PNR->PGW->PushToInputQueue (Notify);

								  // TODO: FIXP/Update - Added to check the pub notify case

#ifdef DEBUG
								  PB->S << Offset <<"(The following message was published to the peer)"<< endl;

								  PB->S << "(" << endl << *Notify << ")"<< endl;
#endif
								}
							}
						  else
							{
							  PB->S << Offset << "(ERROR: Misformatted notification. It must have 5 elements)" << endl;
							}

						  Notification.clear ();
						  Limiters.clear ();
						  Sources.clear ();
						  Destinations.clear ();
						}
					}
				  else // TODO: FIXP/Oct - Added a warning to inform when NRNCS is not operational
					{
					  PB->S << Offset << "(Warning: NRNCS is not operational)" << endl;
					}
				}
			  else
				{
				  PB->S << Offset << "(ERROR: One or more argument is empty)" << endl;
				}
			}
		  else
			{
			  PB->S << Offset << "(ERROR: Unable to read the arguments)" << endl;
			}
		}
	  else
		{
		  PB->S << Offset << "(ERROR: Wrong number of arguments)" << endl;
		}
	}
  else
	{
	  PB->S << Offset << "(ERROR: Unable to read the number of arguments)" << endl;
	}

  //PB->S << Offset <<  "(Done)" << endl << endl << endl;

  return Status;
}

