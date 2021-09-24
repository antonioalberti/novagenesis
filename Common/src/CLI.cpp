/*
	NovaGenesis

	Name:		Command Line Interface
	Object:		CLI
	File:		CLI.cpp
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

#ifndef _CLI_H
#include "CLI.h"
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CLIRUNINITIALIZATION01_H
#include "CLIRunInitialization01.h"
#endif

#ifndef _CLISCNACK01_H
#include "CLISCNAck01.h"
#endif

CLI::CLI (string _LN, Process *_PP, unsigned int _Index, GW *_PGW, HT *_PHT, string _Path)
	: Block (_LN, _PP, _Index, _Path)
{
  Version = "0.1";
  PGW = _PGW;
  PHT = _PHT;
  State = "initialization";

  Message *PIM = 0;
  CommandLine *PCL = 0;
  Action *PA = 0;
  Message *InlineResponseMessage = NULL;

  NewAction ("-run --initialization 0.1", PA);
  NewAction ("-scn --ack 0.1", PA);

  // Creating a -run --initialization message
  PP->NewMessage (GetTime (), 0, false, PIM);

  // Adding only the run initialization command line
  PIM->NewCommandLine ("-run", "--initialization", "0.1", PCL);

  // Push the message to the GW input queue
  PGW->PushToInputQueue (PIM);

  // Run
  Run (PIM, InlineResponseMessage);

  // Mark to delete
  PIM->MarkToDelete ();
}

CLI::~CLI ()
{
  vector<Action *>::iterator it4;

  Action *Temp;

  for (it4 = Actions.begin (); it4 != Actions.end (); it4++)
	{
	  Temp = *it4;

	  if (Temp != 0)
		{
		  delete Temp;
		}

	  Temp = 0;
	}
}

// Allocate and add an Action on Actions container
void CLI::NewAction (const string _LN, Action *&_PA)
{
  if (_LN == "-run --initialization 0.1")
	{
	  CLIRunInitialization01 *P = new CLIRunInitialization01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}

  if (_LN == "-scn --ack 0.1")
	{
	  CLISCNAck01 *P = new CLISCNAck01 (_LN, this, PP->PMB);

	  Actions.push_back ((Action *)P);
	}
}

// Get an Action
int CLI::GetAction (string _LN, Action *&_PA)
{
  int Status = ERROR;
  return Status;
}

// Delete an Action
int CLI::DeleteAction (string _LN)
{
  int Status = ERROR;
  return Status;
}

// Command line interface prompt
void CLI::Prompt ()
{
  S << "*******************************************************************" << endl;
  S << "*  NovaGenesis(NG) Command Line Interface v0.1                    *" << endl;
  S << "*  Copyright Antonio Marcos Alberti - Inatel - September 2012         *" << endl;
  S << "*******************************************************************" << endl;

  char Line[1024];
  istringstream ins;
  char FirstWord[512];
  char SecondWord[512];
  char ThirdWord[512];
  char FourthWord[512];
  string ng;
  string CommandName;
  string CommandAlternative;
  string CommandVersion;
  int NewMessageFromScreen = 0;
  Message *PM = 0;
  unsigned int NewMessageNumberofLines = 0;
  CommandLine *PCL = 0;
  string Temp;
  int Size = 0;
  string HeaderFileName;
  string FilePath;
  string PayloadFileName;
  string MessageFileName;
  time_t seconds;

  string GWLN = "GW";

  Block *PB = 0;

  PP->GetBlock (GWLN, PB);

  GW *PGW = (GW *)PB;

  while (1)
	{
	  S << "> ";

	  cin.getline (Line, sizeof (Line), '\n');

	  if (!strcmp (Line, "quit") || !strcmp (Line, "exit"))
		{
		  break;
		}
	  else if (strcmp (Line, ""))
		{
		  istringstream iss (Line);

		  if (NewMessageFromScreen == 1)
			{
			  // *************************************************************
			  // New message from screen (look bellow first).
			  // *************************************************************

			  // Send the typed line directly to a message object
			  if (NewMessageNumberofLines == 0)
				{
				  // Create the new message
				  PB->PP->NewMessage (GetTime (), 0, false, PM);
				}

			  // Creates an auxiliary istringstream
			  istringstream temp (Line);

			  temp >> FirstWord;

			  ng = string (FirstWord);

			  // Tests to see if the user wants to terminate the message that is being typed
			  if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
				{
				  temp >> SecondWord;

				  CommandName = string (SecondWord);

				  //cout<<"CommandName = "<<CommandName<<endl;

				  // Send
				  if (CommandName == "-send")
					{
					  temp >> ThirdWord;

					  CommandAlternative = string (ThirdWord);

					  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

					  // Message from screen
					  if (CommandAlternative == "--mfs")
						{
						  temp >> FourthWord;

						  CommandVersion = string (FourthWord);

						  //cout<<"CommandVersion = "<<CommandVersion<<endl;

						  if (CommandVersion == "0.1")
							{
							  // This command concludes a new message typed on the screen and sends it to the gateway block.
							  NewMessageFromScreen = 0;

							  // The message is send to the gateway
							  // cout << "NewMessageFromScreen = "<<NewMessageFromScreen<<endl;

							  S << "> The typed message was:" << endl << endl << *PM << endl << endl;

							  // Sending the message to the Gateway
							  // MISSING

							  // Reseting the counter
							  NewMessageNumberofLines = 0;
							}
						}
					}
				  else
					{
					  // Creates the new CommandLine object
					  PM->NewCommandLine (PCL);

					  //iss >> *PM->CommandLines[NewMessageNumberofLines];

					  //cout<<"The command: "<<*PM->CommandLines[NewMessageNumberofLines]<<" was created on the message."<<endl;

					  NewMessageNumberofLines++;
					}
				}
			}
		  else
			{
			  // *************************************************************
			  // New message from a file and other commands.
			  // *************************************************************
			  while (!iss.eof ())
				{
				  iss >> FirstWord;

				  // Skip comments and new lines
				  if (FirstWord[0] == '#')continue;
				  if (FirstWord[0] == '\0')continue;

				  ng = string (FirstWord);

				  //cout<<"ng = "<<ng<<endl;

				  // Implements the CLI commands that do not require a message, e.g. -new --m or -list -p
				  if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
					{
					  iss >> SecondWord;

					  CommandName = string (SecondWord);

					  //cout<<"CommandName = "<<CommandName<<endl;

					  // New
					  if (CommandName == "-new")
						{
						  iss >> ThirdWord;

						  CommandAlternative = string (ThirdWord);

						  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

						  // Message
						  if (CommandAlternative == "--mfs")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{
								  // The next commands until a -send --mfs is received are going to be accommodated in a new MessageFile object
								  NewMessageFromScreen = 1;

								  S
									  << "> Please type your message on the following command lines. Finish with a ng -send --mfs version"
									  << endl << endl;

								  break;
								}
							}
						}

					  // Send
					  if (CommandName == "-send")
						{
						  iss >> ThirdWord;

						  CommandAlternative = string (ThirdWord);

						  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

						  // Message from file
						  if (CommandAlternative == "--mff")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;
							  if (CommandVersion == "0.1")
								{
								  // Take the [
								  iss >> Temp;

								  //cout<< Temp << endl;

								  if (Temp == "[" && Temp != "<" && Temp != "<1" && Temp != "<2" && Temp != "]")
									{
									  // Take the <
									  iss >> Temp;

									  //cout<< Temp << endl;

									  if (Temp == "<" && Temp != "[" && Temp != "<1" && Temp != "<2" && Temp != "]")
										{
										  // Take the size of the argument
										  iss >> Temp;

										  //cout<< Temp << endl;

										  stringstream ssout (Temp.c_str ());

										  ssout >> Size;

										  // Message without payload
										  if (Size == 2)
											{
											  // Take the string type
											  iss >> Temp;

											  if ((Temp == "s") && (Temp != "1s" && Temp != "2s" && Temp != "3s"))
												{
												  iss >> HeaderFileName;
												  iss >> FilePath;

												  iss >> Temp; // Reads the >
												  iss >> Temp; // Reads the ]

												  PM = 0;

												  seconds = time (NULL);

												  S << "Time = " << seconds << endl;

												  // Creating a new message from the header file
												  PB->PP->NewMessage (seconds, 0, false, HeaderFileName, FilePath, PM);
												}
											  else
												{
												  // ERROR
												}
											}
										  else if (Size == 4)
											{
											  // Take the string type
											  iss >> Temp;

											  if ((Temp == "s") && (Temp != "1s" && Temp != "2s" && Temp != "3s"))
												{
												  iss >> HeaderFileName;
												  iss >> PayloadFileName;
												  iss >> MessageFileName;
												  iss >> FilePath;

												  iss >> Temp; // Reads the >
												  iss >> Temp; // Reads the ]

												  PM = 0;

												  seconds = time (NULL);

												  //S << "Time = "<<seconds<<endl;

												  // Creating a new message from the header file
												  PB->PP
													  ->NewMessage (seconds, 0, true, HeaderFileName, PayloadFileName, MessageFileName, FilePath, PM);

												}
											  else
												{
												  // ERROR
												}
											}
										  else
											{
											  //ERROR;
											}
										}
									  else
										{
										  // ERROR
										}
									}
								  else
									{
									  // ERROR
									}
								}
							}
						}

					  // List
					  if (CommandName == "-list")
						{
						  iss >> ThirdWord;

						  CommandAlternative = string (ThirdWord);

						  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

						  // Blocks
						  if (CommandAlternative == "--b")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{
								  //
								}
							}

						  // Processes
						  if (CommandAlternative == "--p")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{
								  //
								}
							}

						  // Bindings
						  if (CommandAlternative == "--b")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{

								}
							}
						}

					  // Version
					  if (CommandName == "-version")
						{
						  iss >> ThirdWord;

						  CommandAlternative = string (ThirdWord);

						  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

						  // Standard
						  if (CommandAlternative == "--s")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{
								  S << endl << "> The current version is " << Version << endl << endl;
								}
							}
						}

					  // Set
					  if (CommandName == "-set")
						{
						  iss >> ThirdWord;

						  CommandAlternative = string (ThirdWord);

						  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

						  // Path
						  if (CommandAlternative == "--path")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{
								  // Take the [
								  iss >> Temp;

								  //cout<< Temp << endl;

								  if (Temp == "[" && Temp != "<" && Temp != "<1" && Temp != "<2" && Temp != "]")
									{
									  // Take the <
									  iss >> Temp;

									  //cout<< Temp << endl;

									  if (Temp == "<" && Temp != "[" && Temp != "<1" && Temp != "<2" && Temp != "]")
										{
										  // Take the 1
										  iss >> Temp;

										  //cout<< Temp << endl;

										  stringstream ssout (Temp.c_str ());

										  ssout >> Size;

										  if (Size == 1)
											{
											  // Take the string type
											  iss >> Temp;

											  if ((Temp == "s") && (Temp != "1s"))
												{
												  iss >> FilePath;
												  iss >> Temp; // Reads the >
												  iss >> Temp; // Reads the ]

												  if (FilePath != "")
													{
													  // Set the process working path
													  PP->SetPath (FilePath);

													  S << endl << "> Path was set to " << FilePath << endl << endl;
													}
												}
											  else
												{
												  // ERROR
												}
											}
										  else
											{
											  //ERROR;
											}
										}
									  else
										{
										  // ERROR
										}
									}

								}
							}
						}

					  // Show
					  if (CommandName == "-show")
						{
						  iss >> ThirdWord;

						  CommandAlternative = string (ThirdWord);

						  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

						  // Path
						  if (CommandAlternative == "--path")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{
								  if (PP->GetPath () != "")
									{
									  // Get the process working path
									  S << endl << "> The current path is " << PP->GetPath () << endl;
									}
								  else
									{
									  S << endl
										<< "> The path was not defined yet. Use ng -set --path version [ < 1 string path > ]"
										<< endl << endl;
									}
								}
							}
						}

					  // Help
					  if (CommandName == "-help")
						{
						  iss >> ThirdWord;

						  CommandAlternative = string (ThirdWord);

						  //cout<<"CommandAlternative = "<<CommandAlternative<<endl;

						  // Standard
						  if (CommandAlternative == "--s")
							{
							  iss >> FourthWord;

							  CommandVersion = string (FourthWord);

							  //cout<<"CommandVersion = "<<CommandVersion<<endl;

							  if (CommandVersion == "0.1")
								{
								  S << endl << " The general format of NovaGenesis messages is:" << endl;
								  S << endl << " ng -command --alternative version [ vectorial arguments ]" << endl;
								  S << endl << " Where:" << endl << endl;
								  S << right << setw (30) << "-command:" << "  It is the action to be done." << endl;
								  S << right << setw (30) << "--alternative:"
									<< "  It selects among alternative implementations. An alternative is always required. --s is the standard one."
									<< endl;
								  S << right << setw (30) << "version:"
									<< "  It selects the desired version of implementation." << endl;
								  S << right << setw (30) << "[ vectorial arguments ]:"
									<< "  They are the arguments of the command." << endl << endl;
								}
							}
						  else
							{
							  S << endl << "> Usage: ng -help --s version. Example: ng -help --s 0.1." << endl << endl;
							}
						}
					}
				}
			}
		}

	  //tthread::this_thread::sleep_for(tthread::chrono::milliseconds(2000));
	}

  PGW->SetStopGatewayFlag (true);

  S << "Closing prompt. ";
}

void CLI::PromptThreadWrapper (void *aArg)
{
  CLI *PCLI = static_cast<CLI *>(aArg);
  PCLI->Prompt ();
}

