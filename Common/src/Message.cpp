/*
	NovaGenesis

	Name:		Message
	Object:		Message
	File:		Message.cpp
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

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#include <string.h>

////#define DEBUG // To follow message processing
////#define DEBUG1 // To follow message deletion

Message::Message (double _Time, short _Type, bool _HasPayload)
{
  //HeaderFile.SetName("");
  PayloadFile.SetName ("");
  //MessageFile.SetName("");

  InstantiationTime = GetSystemTime ();

  SCN = "";
  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;

  //HeaderSize=0;
  PayloadSize = 0;
  MessageSize = 0;

  //DeleteHeaderArray=false;
  DeletePayloadArray = false;
  DeleteMessageArray = false;

  //Header=0;
  Payload = 0;
  Msg = 0;

  Delete = false;
  ApplicationDeleted = DELETED_BY_CORE;

  InstantiationNumber = 0;

  NoCL = 0;
  CommandLines = NULL;
}

Message::Message (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path)
{
  //HeaderFile.SetName(_HeaderFileName);
  //HeaderFile.SetPath(_Path);
  //HeaderFile.SetOption("BINARY");

  PayloadFile.SetName ("");
  //MessageFile.SetName("");

  InstantiationTime = GetSystemTime ();

  SCN = "";
  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;

  //HeaderSize=0;
  PayloadSize = 0;
  MessageSize = 0;

  //DeleteHeaderArray=false;
  DeletePayloadArray = false;
  DeleteMessageArray = false;

  //Header=0;
  Payload = 0;
  Msg = 0;

  Delete = false;
  ApplicationDeleted = DELETED_BY_CORE;

  InstantiationNumber = 0;

  NoCL = 0;
  CommandLines = NULL;
}

Message::Message (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path)
{
  //HeaderFile.SetName(_HeaderFileName);
  //HeaderFile.SetPath(_Path);
  //HeaderFile.SetOption("BINARY");

  PayloadFile.SetName (_PayloadFileName);
  PayloadFile.SetPath (_Path);
  PayloadFile.SetOption ("BINARY");

  //MessageFile.SetName(_MessageFileName);
  //MessageFile.SetPath(_Path);
  //MessageFile.SetOption("BINARY");

  SCN = "";

  InstantiationTime = GetSystemTime ();

  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;

  //HeaderSize=0;
  PayloadSize = 0;
  MessageSize = 0;

  //DeleteHeaderArray=false;
  DeletePayloadArray = false;
  DeleteMessageArray = false;

  //Header=0;
  Payload = 0;
  Msg = 0;

  Delete = false;
  ApplicationDeleted = DELETED_BY_CORE;

  InstantiationNumber = 0;

  NoCL = 0;
  CommandLines = NULL;
}

// Message copy constructor
Message::Message (const Message &M)
{
  //Header=0;
  Payload = 0;
  Msg = 0;

  string Option = "BINARY";

  //HeaderFile.SetName(M.HeaderFile.GetName());
  //HeaderFile.SetPath(M.HeaderFile.GetPath());
  //HeaderFile.SetOption(Option);

  PayloadFile.SetName (M.PayloadFile.GetName ());
  PayloadFile.SetPath (M.PayloadFile.GetPath ());
  PayloadFile.SetOption (Option);

  //MessageFile.SetName(M.MessageFile.GetName());
  //MessageFile.SetPath(M.MessageFile.GetPath());
  //MessageFile.SetOption(Option);

  SetHasPayloadFlag (M.GetHasPayloadFlag ());

  InstantiationTime = GetSystemTime ();

  NoCL = 0;
  CommandLines = NULL;

  for (unsigned int i = 0; i < M.NoCL; i++)
	{
	  CommandLine *PCL1 = 0;
	  CommandLine *PCL2 = 0;

	  PCL1 = M.CommandLines[i];

	  if (PCL1 != 0)
		{
		  NewCommandLine (PCL1, PCL2);
		}
	}

  /*

  if (M.HeaderSize > 0)
	  {
		  HeaderSize=M.HeaderSize;

		  Header=new char[HeaderSize];

		  for (int j=0; j<HeaderSize ;j++)
			  {
				  Header[j]=M.Header[j];
			  }
	  }*/

  if (M.PayloadSize > 0)
	{
	  PayloadSize = M.PayloadSize;

	  Payload = new char[PayloadSize];

	  for (int k = 0; k < PayloadSize; k++)
		{
		  Payload[k] = M.Payload[k];
		}
	}

  // Do not initialize those values, because after that it is not possible to change the array
  MessageSize = 0;
  DeleteMessageArray = false;

  SCN = M.GetSelfCertifyingName ();
  Time = M.GetTime ();
  Type = M.GetType ();
  Tag = M.GetTag ();

  Delete = M.Delete;
  ApplicationDeleted = M.ApplicationDeleted;

  //DeleteHeaderArray=M.DeleteHeaderArray;
  DeletePayloadArray = M.DeletePayloadArray;

  InstantiationNumber = 0;
}

Message::~Message ()
{
  //cout << "(" << endl << *this << ")"<< endl << endl << endl;

  //cout << "(The message instantion number is "<<InstantiationNumber<<")"<<endl;
  //cout << "(The size of the header is "<<HeaderSize<<" bytes)"<<endl;
  //cout << "(The size of the payload is "<<PayloadSize<<" bytes)"<<endl;
  //cout << "(The size of the message is "<<MessageSize<<" bytes)"<<endl;
  //cout << "(The delete header flag is "<<DeleteHeaderArray<<" (0=false, 1=true))"<<endl;
  //cout << "(The delete payload flag is "<<DeletePayloadArray<<" (0=false, 1=true))"<<endl;
  //cout << "(The delete message flag is "<<DeleteMessageArray<<" (0=false, 1=true))"<<endl;

  /*

  if (DeleteHeaderArray == true && Header != 0 && HeaderSize > 0)
	  {
		  delete[] Header;
	  }
  else
	  {
		  //cout << "(Warning: The header array pointer is null)"<<endl;
	  } */

  if (DeletePayloadArray == true && Payload != 0 && PayloadSize > 0)
	{
	  delete[] Payload;
	}
  else
	{
	  //cout << "(Warning: The payload array pointer is null)"<<endl;
	}

  if (DeleteMessageArray == true && Msg != 0 && MessageSize > 0)
	{
	  delete[] Msg;
	}
  else
	{
	  //cout << "(Warning: The message array pointer is null)"<<endl;
	}

  if (CommandLines != NULL)
	{
	  for (unsigned int i = 0; i < NoCL; i++)
		{
		  delete CommandLines[i];
		}

	  delete[] CommandLines;
	}
}

void
Message::SetMessage (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path)
{
  //HeaderFile.SetName(_HeaderFileName);
  //HeaderFile.SetPath(_Path);
  //HeaderFile.SetOption("BINARY");

  PayloadFile.SetName (_PayloadFileName);
  PayloadFile.SetPath (_Path);
  PayloadFile.SetOption ("BINARY");

  //MessageFile.SetName(_MessageFileName);
  //MessageFile.SetPath(_Path);
  //MessageFile.SetOption("BINARY");

  Time = _Time;
  Type = _Type;
  Tag = 0;
  HasPayloadFlag = _HasPayload;
}

// Set message's SCN
void Message::SetSelfCertifyingName (string _SCN)
{
  SCN = _SCN;
}

// Set message's type
void Message::SetType (short _Type)
{
  Type = _Type;
}

void Message::SetHasPayloadFlag (bool _Flag)
{
  HasPayloadFlag = _Flag;
}

// Set time
void Message::SetTime (double _Time)
{
  Time = _Time;
}

// Set tag
void Message::SetTag (unsigned int _Tag)
{
  Tag = _Tag;
}

/*

// Set header file name
void Message::SetHeaderFileName(string _Name)
{
	HeaderFile.SetName(_Name);
} */

// Set payload file name
void Message::SetPayloadFileName (string _Name)
{
  PayloadFile.SetName (_Name);
}

/*
// Set payload file name
void Message::SetMessageFileName(string _Name)
{
	MessageFile.SetName(_Name);
}*/

/*
// Get header file name
string Message::GetHeaderFileName()
{
	string Temp;

	HeaderFile.GetName(Temp);

	return Temp;
}*/

// Get payload file name
string Message::GetPayloadFileName ()
{
  string Temp;

  PayloadFile.GetName (Temp);

  return Temp;
}

/*

// Get message file name
string Message::GetMessageFileName()
{
	string Temp;

	MessageFile.GetName(Temp);

	return Temp;
}*/

/*
// Set header file path
void Message::SetHeaderFilePath(string _Path)
{
	HeaderFile.SetPath(_Path);
}*/

// Set payload file path
void Message::SetPayloadFilePath (string _Path)
{
  PayloadFile.SetPath (_Path);
}

/*
// Set payload file path
void Message::SetMessageFilePath(string _Path)
{
	MessageFile.SetPath(_Path);
}*/

/*
// Get header file path
string Message::GetHeaderFilePath()
{
	string Temp;

	HeaderFile.GetPath(Temp);

	return Temp;
}*/

// Get payload file path
string Message::GetPayloadFilePath ()
{
  string Temp;

  PayloadFile.GetPath (Temp);

  return Temp;
}

/*
// Get message file path
string Message::GetMessageFilePath()
{
	string Temp;

	MessageFile.GetPath(Temp);

	return Temp;
}*/

// Set message to be deleted
void Message::MarkToDelete ()
{
  // Only mark to delete if the Application Deleted flag was false.
  // If true, the application should make the Application Deleted flag false first to enable automatic deletion.
  // If the application does not turn the Application Deleted flag false the message will be never deleted.
  // SetApplicationDeletedFlag(DELETED_BY_CORE)
  if (ApplicationDeleted == DELETED_BY_CORE)
	{
	  Delete = true;

#ifdef DEBUG1
	  cout<<"Marking the following message to delete"<<endl;

	  cout << "(" << endl << *this << ")"<< endl;
#endif
	}
}

// Avoid a message to be deleted
void Message::UnmarkToDelete ()
{
  Delete = false;
}

// Set application driven deletion
void Message::SetApplicationDeletedFlag (bool _Flag)
{
  ApplicationDeleted = _Flag;
}

/*
// Set header file option
void Message::SetHeaderFileOption(string _Option)
{
	HeaderFile.SetOption(_Option);
}*/

// Set payload file option
void Message::SetPayloadFileOption (string _Option)
{
  PayloadFile.SetOption (_Option);
}

/*
// Set payload file option
void Message::SetMessageFileOption(string _Option)
{
	MessageFile.SetOption(_Option);
}*/

/*
// Get header file option
string Message::GetHeaderFileOption()
{
	string Temp;

	HeaderFile.GetOption(Temp);

	return Temp;
}*/

// Get payload file option
string Message::GetPayloadFileOption ()
{
  string Temp;

  PayloadFile.GetOption (Temp);

  return Temp;
}

/*
// Get message file option
string Message::GetMessageFileOption()
{
	string Temp;

	MessageFile.GetOption(Temp);

	return Temp;
}*/

// Get message's SCN
string Message::GetSelfCertifyingName () const
{
  return SCN;
}

// Get message's type
short Message::GetType () const
{
  return Type;
}

bool Message::GetHasPayloadFlag () const
{
  return HasPayloadFlag;
}

// Get time
double Message::GetTime () const
{
  return Time;
}

// Get tag
unsigned int Message::GetTag () const
{
  return Tag;
}

// Set message to be deleted
bool Message::GetDeleteFlag ()
{
  return Delete;
}

// Set application driven deletion
bool Message::GetApplicationDeletedFlag ()
{
  return ApplicationDeleted;
}

int Message::NewCommandLine (CommandLine *&CL)
{
  CommandLine *PCL = new CommandLine;

  CL = PCL;

  CommandLine **Temp = new CommandLine *[NoCL + 1];

  for (unsigned int i = 0; i < NoCL; i++)
	{
	  Temp[i] = CommandLines[i];
	}

  Temp[NoCL] = PCL;

  if (CommandLines != NULL)
	{
	  delete[] CommandLines;
	}

  CommandLines = Temp;

  NoCL++;

  return OK;
}

int Message::NewCommandLine (CommandLine *_PCL, CommandLine *&CL)
{
  CommandLine *PCL = new CommandLine (*_PCL);

  CL = PCL;

  CommandLine **Temp = new CommandLine *[NoCL + 1];

  for (unsigned int i = 0; i < NoCL; i++)
	{
	  Temp[i] = CommandLines[i];
	}

  Temp[NoCL] = PCL;

  if (CommandLines != NULL)
	{
	  delete[] CommandLines;
	}

  CommandLines = Temp;

  NoCL++;

  return OK;
}

int Message::NewCommandLine (string _Name, string _Alternative, string _Version, CommandLine *&C)
{
  CommandLine *PCL = new CommandLine (_Name, _Alternative, _Version);

  C = PCL;

  CommandLine **Temp = new CommandLine *[NoCL + 1];

  for (unsigned int i = 0; i < NoCL; i++)
	{
	  Temp[i] = CommandLines[i];
	}

  Temp[NoCL] = PCL;

  if (CommandLines != NULL)
	{
	  delete[] CommandLines;
	}

  CommandLines = Temp;

  NoCL++;

  return OK;
}

int Message::GetCommandLine (unsigned int _Index, CommandLine *&CL)
{
  if (_Index < NoCL)
	{
	  CL = CommandLines[_Index];
	}

  return OK;
}

// Get a CommandLine by its name and alternative
int Message::GetCommandLine (string _Name, string _Alternative, CommandLine *&CL)
{
  int Status = ERROR;
  CommandLine *PCL = 0;

  //cout << "NCL = "<< NCL <<endl;

  for (unsigned int i = 0; i < NoCL; i++)
	{
	  // Select a command line
	  GetCommandLine (i, PCL);

	  if (PCL->Name == _Name && PCL->Alternative == _Alternative)
		{
		  CL = PCL;

		  Status = OK;

		  break;
		}
	}

  return Status;
}

// Get a CommandLine Index
int Message::GetCommandLineIndex (CommandLine *CL, unsigned int &_Index)
{
  int Status = ERROR;
  vector<CommandLine *>::iterator it;

  for (unsigned int i = 0; i < NoCL; i++)
	{
	  if (CommandLines[i] == CL)
		{
		  _Index = i;

		  Status = OK;

		  break;
		}

	  i++;
	}

  return Status;
}

int Message::GetNumberofCommandLines (unsigned int &_Number)
{
  _Number = NoCL;

  return OK;
}

int Message::GetNumberofCommandLineArguments (unsigned int _CommandLine, unsigned int &_Number)
{
  if (_CommandLine < NoCL)
	{
	  return CommandLines[_CommandLine]->GetNumberofArguments (_Number);
	}

  return ERROR;
}

int
Message::GetNumberofCommandLineArgumentElements (unsigned int _CommandLine, unsigned int _Argument, unsigned int &_Number)
{
  if (_CommandLine < NoCL)
	{
	  return CommandLines[_CommandLine]->GetNumberofArgumentElements (_Argument, _Number);
	}

  return ERROR;
}

int
Message::SetCommandLineArgumentElement (unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string _Value)
{
  int Status = ERROR;

  if (_CommandLine < NoCL)
	{
	  CommandLines[_CommandLine]->SetArgumentElement (_Argument, _Element, _Value);

	  Status = OK;
	}

  return Status;
}

int
Message::GetCommandLineArgumentElement (unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string &_Value)
{
  if (_CommandLine < NoCL)
	{
	  CommandLines[_CommandLine]->GetArgumentElement (_Argument, _Element, _Value);

	  return OK;
	}

  return ERROR;
}

/*
 int Message::GetHeaderFile(File& _F)
{
	_F=HeaderFile;

	return OK;
}*/

int Message::GetPayloadFile (File &_F)
{
  _F = PayloadFile;

  return OK;
}

/*
int Message::GetMessageFile(File& _F)
{
	_F=MessageFile;

	return OK;
}*/

/*
// Set *Header from char array. A copy of the char array is done.
int Message::SetHeaderFromCharArray(char* _Value, long _Size)
{
	int Status=ERROR;

	if (HeaderSize == 0 && DeleteHeaderArray == false)
		{
			if (_Size > 0)
				{
					HeaderSize=_Size;

					Header=new char[HeaderSize];

					for (long j=0; j<HeaderSize ; j++)
						{
							Header[j]=_Value[j];
						}

					Status=OK;

					DeleteHeaderArray=true;
				}
		}

	return Status;
}
*/

// Set *Payload from char array. A copy of the char array is done.
int Message::SetPayloadFromCharArray (char *_Value, long long _Size)
{
  int Status = ERROR;

  if (PayloadSize == 0 && DeletePayloadArray == false)
	{
	  if (_Size > 0)
		{
		  PayloadSize = _Size;

		  Payload = new char[PayloadSize];

		  for (long long j = 0; j < PayloadSize; j++)
			{
			  Payload[j] = _Value[j];
			}

		  Status = OK;

		  DeletePayloadArray = true;

		  HasPayloadFlag = true;
		}
	}

  return Status;
}

// Set *Msg from char array. A copy of the char array is done.
int Message::SetMessageFromCharArray (char *_Value, long long _Size)
{
  int Status = ERROR;

  if (MessageSize == 0 && DeleteMessageArray == false)
	{
	  if (_Size > 0)
		{
		  MessageSize = _Size;

		  Msg = new char[MessageSize];

		  for (long long j = 0; j < MessageSize; j++)
			{
			  Msg[j] = (char)_Value[j];

			  //printf("%i %d %c \n",j,Msg[j],Msg[j]);
			}

		  Status = OK;

		  DeleteMessageArray = true;
		}
	}

  return Status;
}

/*
// Get *Header from char array. Don't delete[] the array pointer
int Message::GetHeaderFromCharArray(char*& _Value)
{
	int Status=ERROR;

	if (HeaderSize > 0 && DeleteHeaderArray == true)
		{
			_Value=Header;

			Status=OK;
		}

	return Status;
}*/

// Get *Payload from char array. Don't delete[] the array pointer
int Message::GetPayloadFromCharArray (char *&_Value)
{
  int Status = ERROR;

  if (PayloadSize > 0 && DeletePayloadArray == true)
	{
	  _Value = Payload;

	  Status = OK;
	}

  return Status;
}

// Get *Msg from char array. Don't delete[] the array pointer
int Message::GetMessageFromCharArray (char *&_Value)
{
  int Status = ERROR;

  if (MessageSize > 0 && DeleteMessageArray == true)
	{
	  _Value = Msg;

	  Status = OK;
	}

  return Status;
}

// Get *Msg from char array. Don't delete[] the array pointer
int Message::GetMessageFromCharArray (unsigned char *&_Value)
{
  int Status = ERROR;

  if (MessageSize > 0 && DeleteMessageArray == true)
	{
	  _Value = (unsigned char *)Msg;

	  Status = OK;
	}

  return Status;
}

double Message::GetSystemTime ()
{
  struct timespec t;

  clock_gettime (CLOCK_MONOTONIC, &t);

  return ((t.tv_sec) + (double)(t.tv_nsec / 1e9));
}

// Get the object instantiation time in seconds
double Message::GetInstantiationTime ()
{
  return InstantiationTime;
}


// ******************************************************
// Header format convertions
// ******************************************************

/*
// Convert the Header from file to a char array pointed by *Header. The HeaderFile handler needs to be properly configured.
int Message::ConvertHeaderFromFileToCharArray()
{
	int Status=ERROR;

	if (HeaderFile.GetName() != "")
		{
			Status=HeaderFile.OpenInputFile();

			if (HeaderSize == 0)
				{
					// Read the number of characters in the MessageFile.
					HeaderSize=HeaderFile.tellg();

					if (HeaderSize > 0)
						{
							// Change the position to read from the beginning
							HeaderFile.seekg(0);

							Header=new char[HeaderSize];

							HeaderFile.read(Header,HeaderSize);

							//for (int i=0; i< _Size; i++)
							//	{
							//		printf("%i %d %c \n",i,_Value[i],_Value[i]);
							//	}
						}

					HeaderFile.CloseFile();

					DeleteHeaderArray=true;

					Status=OK;
				}
		}

	return Status;
}
*/

/*
// Convert the Header from CommandLines to a char array pointed by *Header. The CommandLines need to be properly configured.
int Message::ConvertHeaderFromCommandLinesToCharArray()
{
	int Status=ERROR;

	stringstream ss;

	if (NoCL > 0)
		{
			if (HeaderSize == 0 && DeleteHeaderArray == false)
				{
					//cout << "CommandLines.size() = "<< CommandLines.size() << endl;

					for (unsigned int i=0; i< NoCL; i++)
						{
							ss << *CommandLines[i];

							//cout << "Command Line:"<< endl<<*CommandLines[i] << endl;
						}

					string tmp(ss.str());

					//cout <<"Header = "<<tmp<< endl;

					HeaderSize=(long)tmp.size();

					//cout <<"Header size = "<<HeaderSize<< endl;

					Header=new char[HeaderSize];

					for (long k=0; k<HeaderSize; k++)
						{
							Header[k]=tmp[k];
						}

					DeleteHeaderArray=true;

					//for (int i=0; i< HeaderSize; i++)
					//	{
					//		printf("%i %d %c \n",i,Header[i],Header[i]);
					//	}

					Status=OK;
				}
			else
				{
					cout << "(ERROR: Header convertion error)" << endl;
				}
		}
	else
		{
			cout << "(ERROR: Empty header)" << endl;
		}

	return Status;
}*/

/*

// Convert the Header from char array to CommandLines. The char array pointed by *Header needs to be properly configured.
int Message::ConvertHeaderFromCharArrayToCommandLines()
{
	int 		Status=ERROR;

	if (HeaderSize > 0 && DeleteHeaderArray == true && Header != 0)
		{
			stringstream ss;

			for (int t=0; t<HeaderSize; t++)
				{
					ss << Header[t];
				}

			ss >> *this;

			Status=OK;
		}

	return Status;
}

*/

/*

// Convert the Header from file to CommandLines. The HeaderFile handler needs to be properly configured.
int Message::ConvertHeaderFromFileToCommandLines()
{
	int Status=0;

	Status=HeaderFile.OpenInputFile();

	HeaderFile >> *this;

	return Status;
}

*/

/*
// Convert the Header from char array to file. The char array pointed by *Header needs to be properly configured as well as the HeaderFile handler
int Message::ConvertHeaderFromCharArrayToFile()
{
	int Status=ERROR;

	if (HeaderFile.GetName() != "")
		{
			Status=HeaderFile.OverwriteOutputFile();

			if (HeaderSize > 0 && DeleteHeaderArray == true)
				{
					// Change the position to read from the beginning
					HeaderFile.seekg(0);

					// Write char array content to the MessageFile
					HeaderFile.write(Header,HeaderSize);

					//for (int i=0; i< _Size; i++)
					//	{
					//		printf("%i %d %c \n",i,_Value[i],_Value[i]);
					//	}

					HeaderFile.CloseFile();
				}
		}

	return Status;
}

*/

/*
// Convert the Header from CommandLines to file. The CommandLines need to be properly configured as well as the HeaderFile handler
int Message::ConvertHeaderFromCommandLinesToFile()
{
	int Status=OK;CommandLines

	if (NoCL > 0)
		{
			if (HeaderFile.GetName() != "" && DeleteHeaderArray == false)
				{
					Status=HeaderFile.OverwriteOutputFile();

					for (unsigned int i=0; i< NoCL; i++)
						{
							HeaderFile << *CommandLines[i];
						}

					DeleteHeaderArray=true;

					Status=OK;

					HeaderFile.CloseFile();
				}
		}

	return Status;
}
*/

// ******************************************************
// Payload format convertions
// ******************************************************

// Convert the Payload from file to a char array pointed by *Payload. The PayloadFile handler needs to be properly configured.
int Message::ConvertPayloadFromFileToCharArray ()
{
  int Status = OK;
  long long Length = 0;

  if (PayloadFile.GetName () != "")
	{
	  Status = PayloadFile.OpenInputFile ();

	  if (PayloadSize == 0 && DeletePayloadArray == false && Status == OK && HasPayloadFlag == true)
		{
		  // Stores current position
		  Length = PayloadFile.tellg ();

		  // Change position to the beginning
		  PayloadFile.seekg (0);

		  if (Length > 0)
			{
			  PayloadSize = Length;

			  Payload = new char[PayloadSize];

			  PayloadFile.read (Payload, PayloadSize);

			  DeletePayloadArray = true;

			  PayloadFile.CloseFile ();
			}
		}
	}

  return Status;
}

// Convert the Payload from char array to file. The char array pointed by *Payload needs to be properly configured as well as the PayloadFile handler
int Message::ConvertPayloadFromCharArrayToFile ()
{
  int Status = OK;

  if (PayloadFile.GetName () != "")
	{
	  PayloadFile.OpenOutputFile (PayloadFile.GetName (), PayloadFile.GetPath (), PayloadFile.GetOption ());

	  Status = PayloadFile.OverwriteOutputFile ();

	  if (PayloadSize > 0 && DeletePayloadArray == true)
		{
		  // Change the position to read from the beginning
		  PayloadFile.seekg (0);

		  // Write char array content to the PayloadFile
		  PayloadFile.write (Payload, PayloadSize);

		  for (long long i = 0; i < PayloadSize; i++)
			{
			  //printf("%i %d %c \n",i,Payload[i],Payload[i]);
			}

		  PayloadFile.CloseFile ();
		}
	  else
		{
		  cout << "ERROR: Payload size equals to zero or delete payload arrays flag is false" << endl;
		}
	}
  else
	{
	  cout << "ERROR: Empty file name" << endl;
	}

  return Status;
}

// Extract Payload char array from message char array. The Msg char array needs to be properly configured.
int Message::ExtractPayloadCharArrayFromMessageCharArray ()
{
  int Status = OK;
  string Option;
  string ng = "ng";
  long long Position = 0;
  long long Size = 0;
  long long length = 0;
  stringstream ss;

  //cout << "          (Message size = " <<MessageSize<< endl;

  for (long long i = 0; i < MessageSize; i++)
	{
	  ss << Msg[i];

	  //printf("%i %d %c \n",i,Msg[i],Msg[i]);
	}

  ss.seekg (0);

  char Line[4096];

  while (ss.getline (Line, sizeof (Line), '\n'))
	{
	  if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
		{
		}
	  else
		{
		  break;
		}
	}

  // Stores current position
  Position = ss.tellg ();

  // Get the total size
  Size = ss.str ().length ();

  //cout << "Size = " << Size << endl;

  // Calculates the length of the payload portion
  length = Size - Position;

  //cout << "length = " << length << endl;

  //cout << "Reading from position = " << Position << endl;

  if (length > 1 && Position > 0 && Size > 0)
	{
	  // There is a payload
	  HasPayloadFlag = true;

	  // Change the position to write at the beginning
	  PayloadFile.seekp (0, ios::beg);

	  Payload = new char[length];

	  ss.read (Payload, length);

	  PayloadSize = length;

	  //cout << "Payload size = " << PayloadSize << endl;

	  DeletePayloadArray = true;
	}

  //cout << "          (The extraction of the payload was a sucess)" << endl;

  return Status;
}

// ******************************************************
// Entire message format conversions
// ******************************************************

/*
// Convert the Message from file to CommandLines and Payload file. The MessageFile handler needs to be properly configured
int Message::ConvertMessageFromFileToCommandLinesandPayloadFile()
{
	int Status=ERROR;

	if (MessageFile.GetName() != "")
		{
			Status=MessageFile.OpenInputFile();

			MessageFile >> *this;

			MessageFile.CloseFile();
		}

	return Status;
}

*/


// Convert the Message from file to a char array pointed by *Message. The MessageFile handler needs to be properly configured.
int Message::ConvertMessageFromFileToCharArray ()
{
  int Status = ERROR;

  if (MessageFile.GetName () != "")
	{
	  //cout << "> Message Name = " << MessageFile.GetName() << endl;

	  Status = MessageFile.OpenInputFile ();

	  // Read the number of characters in the MessageFile.
	  MessageSize = MessageFile.tellg ();

	  //cout << "> Message Size = " << MessageSize << endl;

	  if (MessageSize > 0 && DeleteMessageArray == false)
		{
		  // Change the position to read from the beginning
		  MessageFile.seekg (0);

		  Msg = new char[MessageSize];

		  MessageFile.read (Msg, MessageSize);

		  //for (int i=0; i< MessageSize; i++)
		  //	{
		  //		printf("%i %d %c \n",i,Msg[i],Msg[i]);
		  //	}

		  DeleteMessageArray = true;
		}
	  else
		{
		  cout << "> ERROR: The Msg char array have been previously created. It cannot be recreated in this version."
			   << endl;
		}

	  MessageFile.CloseFile ();
	}

  return Status;
}


/*
// Convert the Message from char array to a complete Message file. The char array pointed by *Message needs to be properly configured as well as the MessageFile handler
int Message::ConvertMessageFromCharArrayToFile()
{
	int Status=ERROR;

	if (MessageFile.GetName() != "")
		{
			Status=MessageFile.OverwriteOutputFile();

			if (MessageSize > 0 && DeleteMessageArray == true)
				{
					// Change the position to read from the beginning
					MessageFile.seekg(0);

					// Write char array content to the MessageFile
					MessageFile.write(Msg,MessageSize);

					//for (int i=0; i< _Size; i++)
					//	{
					//		printf("%i %d %c \n",i,_Value[i],_Value[i]);
					//	}

					MessageFile.CloseFile();
				}
		}

	return Status;
}

*/

/*
// Convert the Message from CommandLines and Payload file to a complete Message file. The CommandLines need to be properly configured as well as the MessageFile handler
int Message::ConvertMessageFromCommandLinesandPayloadFileToFile()
{
	int Status=ERROR;

	if (MessageFile.GetName() != "")
		{
			Status=MessageFile.OverwriteOutputFile();

			char			*buffer;
			long		    length=0;

			for (unsigned int i=0; i<NoCL; i++)
				{
					MessageFile << *CommandLines[i];
				}

			if (HasPayloadFlag == true && PayloadSize > 0)
				{
					MessageFile << endl; // Add a blank line to separate the payload from the header.

					if (PayloadFile.OpenInputFile() == OK)
						{
							//cout << "Loading payload from file" << endl;

							// Read the number of characters in the Payload file.
							length=PayloadFile.tellg();

							// Change the position to read from the beginning
							PayloadFile.seekg(0, ios::beg);

							if (length > 0)
								{
									buffer=new char[length];

									PayloadFile.read(buffer,length);

									MessageFile.write(buffer,length);

									delete[] buffer;
								}

							PayloadFile.CloseFile();
						}
					else
						{
							//cout << "Loading payload from char array" << endl;

							if (PayloadSize > 0)
								{
									buffer=new char[PayloadSize];

									for (long long int i=0; i<PayloadSize; i++)
										{
											buffer[i]=(char)Payload[i];
										}

									MessageFile.write(buffer,PayloadSize);

									delete[] buffer;
								}
						}
				}
		}

	return Status;
}

*/

// Convert the Message from CommandLines and Payload file to a char array. The CommandLines need to be properly configured as well as the PayloadFile handler
int Message::ConvertMessageFromCommandLinesandPayloadFileToCharArray ()
{
  int Status = ERROR;

  if (MessageSize == 0 && DeleteMessageArray == false)
	{
	  ostringstream oss;

	  if (NoCL > 0)
		{
		  char *buffer;
		  long length = 0;

		  for (unsigned int i = 0; i < NoCL; i++)
			{
			  oss << *CommandLines[i];
			}

		  if (HasPayloadFlag == true && PayloadSize > 0)
			{
			  oss
				  << endl; // Add a blank line to separate the payload ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2from the header.

			  if (PayloadFile.OpenInputFile () == OK)
				{
				  //cout << "Loading payload from file" << endl;

				  // Read the number of characters in the Payload file.
				  length = PayloadFile.tellg ();

				  // Change the position to read from the beginning
				  PayloadFile.seekg (0, ios::beg);

				  if (length > 0)
					{
					  buffer = new char[length];

					  PayloadFile.read (buffer, length);

					  oss.write (buffer, length);

					  delete[] buffer;
					}

				  PayloadFile.CloseFile ();
				}
			  else
				{
				  //cout << "Loading payload from char array" << endl;

				  if (PayloadSize > 0)
					{
					  buffer = new char[PayloadSize];

					  for (long long int i = 0; i < PayloadSize; i++)
						{
						  buffer[i] = (char)Payload[i];
						}

					  oss.write (buffer, PayloadSize);

					  delete[] buffer;
					}
				}
			}

		  const std::string tmp = oss.str ();

		  MessageSize = tmp.size ();

		  Msg = new char[MessageSize];

		  for (int i = 0; i < MessageSize; i++)
			{
			  Msg[i] = tmp[i];

			  //printf("%i %d %c \n",i,Msg[i],Msg[i]);
			}

		  DeleteMessageArray = true;

		  Status = OK;
		}
	}

  return Status;
}

// Convert the Message from CommandLines and Payload char array to a message char array. The CommandLines need to be properly configured as well as the Payload char array
int
Message::ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray ()
{
  int Status = ERROR;

  if (MessageSize == 0 && DeleteMessageArray == false)
	{
	  ostringstream oss;

	  if (NoCL > 0)
		{
		  char *buffer;
		  long long length = 0;

		  for (unsigned int i = 0; i < NoCL; i++)
			{
			  oss << *CommandLines[i];
			}

		  if (HasPayloadFlag == true && PayloadSize > 0)
			{
			  oss << endl; // Add a blank line to separate the payload from the header.

			  if (PayloadFile.OpenInputFile () == OK)
				{
#ifdef DEBUG
				  cout << "Loading payload from file" << endl;
#endif
				  // Read the number of characters in the Payload file.
				  length = (long long)PayloadFile.tellg ();

				  // Change the position to read from the beginning
				  PayloadFile.seekg (0, ios::beg);

				  if (length > 0)
					{
					  buffer = new char[length];

					  PayloadFile.read (buffer, length);

					  oss.write (buffer, length);

					  delete[] buffer;
					}

				  PayloadFile.CloseFile ();
				}
			  else
				{
#ifdef DEBUG
				  cout << "Loading payload from char array" << endl;
#endif

				  if (PayloadSize > 0)
					{
					  buffer = new char[PayloadSize];

#ifdef DEBUG
					  cout << "(The size of the payload of the message with instantiation number "<<InstantiationNumber<<" is "<<PayloadSize<<" bytes)"<<endl;
#endif
					  for (long long i = 0; i < PayloadSize; i++)
						{
						  buffer[i] = (char)Payload[i];
						}

					  oss.write (buffer, PayloadSize);

					  delete[] buffer;
					}
				}
			}

		  const std::string tmp = oss.str ();

		  MessageSize = tmp.size ();

#ifdef DEBUG
		  cout << "(The size of the message with instantiation number "<<InstantiationNumber<<" is "<<MessageSize<<" bytes)"<<endl;
#endif
		  Msg = new char[MessageSize];

		  for (long long j = 0; j < MessageSize; j++)
			{
			  Msg[j] = tmp[j];
#ifdef DEBUG
			  printf("%i %d %c \n",j,Msg[j],Msg[j]);
#endif
			}

		  DeleteMessageArray = true;

		  Status = OK;
		}
	}

  return Status;
}

// Convert the complete message char array into command lines and payload file

/*
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadFile()
{
	int Status=ERROR;

	if (MessageSize > 0 && DeleteMessageArray == true)
		{
			stringstream ss;

			for (int t=0; t<MessageSize; t++)
				{
					ss << Msg[t];

					//printf("%i %d %c \n",t,Msg[t],Msg[t]);
				}

			ss >> *this;

			Status=OK;
		}

	return Status;
}

*/

// Convert the complete message char array into command lines and payload char array
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray ()
{

  int Status = ERROR;
  CommandLine *PCL = 0;
  string Option;
  string ng = "ng";
  long long int Position = 0;
  long int Size = 0;
  int length = 0;
  unsigned int i = 0;

  //cout<< "Message size = "<<MessageSize<<endl;

  if (MessageSize > 0 && DeleteMessageArray == true)
	{
	  stringstream ss;

	  for (int t = 0; t < MessageSize; t++)
		{
		  ss << Msg[t];

		  //printf("%i %d %c \n",t,Msg[t],Msg[t]);
		}

	  char Line[4096];

	  while (ss.getline (Line, sizeof (Line), '\n'))
		{
		  if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
			{
			  istringstream ins (Line);

			  NewCommandLine (PCL);

			  ins >> *CommandLines[i];

			  i++;
			}
		  else
			{
			  break;
			}
		}

	  // Stores current position
	  Position = ss.tellg ();

	  // Get the total size
	  Size = ss.str ().length ();

	  //cout << "Size = " << Size << endl;

	  // Calculates the length of the payload portion
	  length = Size - Position;

	  //cout << "length = " << length << endl;

	  //cout << "Reading from position = " << Position << endl;

	  if (length > 1 && Position > 0 && Size > 0)
		{
		  // There is a payload
		  HasPayloadFlag = true;

		  // Change the position to write at the beggining
		  PayloadFile.seekp (0);

		  Payload = new char[length];

		  ss.read (Payload, length);

		  PayloadSize = length;

		  DeletePayloadArray = true;
		}

	  Status = OK;
	}

  return Status;
}

// Convert the complete message char array into command lines and payload char array
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray (File *_PF)
{
  int Status = ERROR;
  CommandLine *PCL = 0;
  string Option;
  long long t = 0;                // Pointer to scan the buffer
  char *Temp;

  if (MessageSize > 16 && DeleteMessageArray == true)
	{
	  //*_PF<<"MessageSize = "<< MessageSize <<endl;

	  // While over the received buffer to delineate command lines and payload
	  while (t < MessageSize)
		{
		  //*_PF<<endl<<"Jumping to position = "<<t<<endl;

		  // Search for a ng command line
		  if (Msg[t] == 'n' && Msg[t + 1] == 'g' && Msg[t + 2] == ' ' && Msg[t + 3] == '-')
			{
			  //*_PF<<"A command line was found"<<endl;

			  // Command line detected
			  // Continues up to the end of this line
			  for (long long u = t; u < MessageSize; u++)
				{
				  if (Msg[u] == '\n')
					{
					  int CommandLineSize = (int)(u - t + 1);

					  //*_PF<<"Command line size = "<<CommandLineSize<<endl;

					  Temp = new char[CommandLineSize];

					  //*_PF<<"Showing the discovered command line:"<<endl;

					  for (int v = 0; v < (u - t); v++)
						{
						  Temp[v] = Msg[t + v];

						  //*_PF<<Temp[v];
						}

					  //*_PF<<endl<<"Creating the new command line object"<<endl;

					  NewCommandLine (PCL);

					  //*_PF<<"Copying the temporary array content to command line object"<<endl;

					  PCL->ConvertCommandLineFromCharArray (Temp, CommandLineSize);

					  //*_PF<<"Deleting the temporary array"<<endl;

					  delete Temp;

					  //*_PF<<endl<<"Jumping to position = "<<u<<endl;

					  t = u;

					  break;
					}
				}
			}
		  else
			{
			  //*_PF<<"Jumping the blank line."<<endl;

			  t++; // Jumps the blank line

			  //*_PF<<endl<<"Jumping to position = "<<t<<endl;

			  //*_PF<<"Start copying the payload to array and file"<<endl;

			  // There is a payload
			  HasPayloadFlag = true;

			  PayloadSize = MessageSize - t;

			  //*_PF<<"Payload size = "<<PayloadSize<<endl;

			  Payload = new char[PayloadSize];

			  long long r = 0;

			  // Continues up	 to the end of the buffer
			  for (int u = t; u < MessageSize; u++)
				{
				  Payload[r] = Msg[u];

				  //*_PF<<Payload[r]<<endl;

				  r++;
				}

			  DeletePayloadArray = true;

			  t = MessageSize
				  - 1;

			  //*_PF<<"Passei tudo"<<endl;
			}

		  t++;
		}

	  Status = OK;
	}

  return Status;
}

// Convert the complete message char array into command lines and payload char array
int Message::ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2 ()
{

  int Status = OK;
  CommandLine *PCL = 0;
  string Option;
  long long t = 0;                // Pointer to scan the buffer
  char *Temp;
  int Aux = OK;

  if (MessageSize > 16 && DeleteMessageArray == true)
	{

#ifdef DEBUG
	  cout<<"MessageSize = "<< MessageSize <<endl;
#endif

	  // While over the received buffer to delineate command lines and payload
	  while (t < MessageSize)
		{
#ifdef DEBUG
		  cout << endl << "Jumping to position = "<< t << endl;
#endif


		  // Search for a ng command line
		  if (Msg[t] == 'n' && Msg[t + 1] == 'g' && Msg[t + 2] == ' ' && Msg[t + 3] == '-')
			{
#ifdef DEBUG
			  cout<<"A command line was found"<<endl;
#endif

			  // Command line detected
			  // Continues up to the end of this line
			  for (long long u = t; u < MessageSize; u++)
				{
				  if (Msg[u] == '\n')
					{
					  int CommandLineSize = (int)(u - t + 1);

#ifdef DEBUG
					  cout<<"Command line size = "<<CommandLineSize<<endl;
#endif

					  Temp = new char[CommandLineSize];

#ifdef DEBUG
					  cout<<"Showing the discovered command line:"<<endl;
#endif

					  for (int v = 0; v < (u - t); v++)
						{
						  Temp[v] = Msg[t + v];
#ifdef DEBUG
						  cout<<Temp[v];
#endif
						}

#ifdef DEBUG
					  cout<<endl<<"Creating the new command line object"<<endl;
#endif
					  NewCommandLine (PCL);

#ifdef DEBUG
					  cout<<"Copying the temporary array content to command line object"<<endl;
#endif

					  Aux = PCL->ConvertCommandLineFromCharArray (Temp, CommandLineSize);

					  if (Aux == ERROR)
						{
						  Status = ERROR;
						}

#ifdef DEBUG
					  cout<<"Deleting the temporary array"<<endl;
#endif
					  delete[] Temp;

#ifdef DEBUG
					  cout<<endl<<"Jumping to position = "<<u<<endl;
#endif
					  t = u;

					  break;
					}
				}
			}
		  else
			{

#ifdef DEBUG
			  cout<<"Jumping the blank line."<<endl;
#endif
			  t++; // Jumps the blank line

#ifdef DEBUG
			  cout<<endl<<"Jumping to position = "<<t<<endl;

			  cout<<"Start copying the payload to array and file"<<endl;
#endif
			  // There is a payload
			  HasPayloadFlag = true;

			  PayloadSize = MessageSize - t;

#ifdef DEBUG
			  cout<<"Payload size = "<<PayloadSize<<endl;
#endif

			  Payload = new char[PayloadSize];

			  long long r = 0;

			  // Continues up to the end of the buffer
			  for (int u = t; u < MessageSize; u++)
				{
				  Payload[r] = Msg[u];

#ifdef DEBUG
				  cout<<Payload[r]<<endl;
#endif
				  r++;
				}

			  DeletePayloadArray = true;

			  t = MessageSize - 1;

#ifdef DEBUG
			  cout<<"Finished"<<endl;
#endif
			}

		  t++;
		}
	}

  return Status;
}

// ******************************************************
// Message size functions
// ******************************************************


/*
// Get header size in bytes.
int Message::GetHeaderSize(long& _Size)
{
	int Status=ERROR;

	if (DeleteHeaderArray == true)
		{
			_Size=HeaderSize;

			Status=OK;
		}

	return Status;
}

*/

// Get payload size in bytes
int Message::GetPayloadSize (long long &_Size)
{
  int Status = ERROR;

  if (DeletePayloadArray == true)
	{
	  _Size = PayloadSize;

	  Status = OK;
	}

  return Status;
}

// Get message size in bytes
int Message::GetMessageSize (long long &_Size)
{
  int Status = ERROR;

  if (MessageSize > 0)
	{
	  _Size = MessageSize;

	  Status = OK;
	}
  else
	{
	  ostringstream oss;

	  if (NoCL > 0)
		{
		  char *buffer;
		  long length = 0;

		  for (unsigned int i = 0; i < NoCL; i++)
			{
			  oss << *CommandLines[i];
			}

		  if (HasPayloadFlag == true)
			{
			  oss << endl; // Add a blank line to separate the payload from the header.

			  if (PayloadFile.OpenInputFile () == OK)
				{
				  //cout << "Loading payload from file" << endl;

				  // Read the number of characters in the Payload file.
				  length = PayloadFile.tellg ();

				  // Change the position to read from the beginning
				  PayloadFile.seekg (0, ios::beg);

				  if (length > 0)
					{
					  buffer = new char[length];

					  PayloadFile.read (buffer, length);

					  oss.write (buffer, length);

					  delete[] buffer;
					}

				  PayloadFile.CloseFile ();
				}
			  else
				{
				  //cout << "Loading payload from char array" << endl;

				  if (PayloadSize > 0)
					{
					  buffer = new char[PayloadSize];

					  for (long long int i = 0; i < PayloadSize; i++)
						{
						  buffer[i] = (char)Payload[i];
						}

					  oss.write (buffer, PayloadSize);

					  delete[] buffer;
					}
				}
			}

		  const std::string tmp = oss.str ();

		  _Size = tmp.size ();

#ifdef DEBUG
		  cout << "Message Size = "<< _Size <<endl;
#endif

		  Status = OK;
		}
	}

  return Status;
}

// ******************************************************
// Checking for command lines
// ******************************************************
int Message::DoesThisCommandLineExistsInMessage (string _Name, string _Alternative, bool &_Verdict)
{
  int Status = ERROR;
  CommandLine *PCL = 0;

  for (unsigned int i = 0; i < NoCL; i++)
	{
	  // Select a command line
	  GetCommandLine (i, PCL);

	  if (PCL->Name == _Name && PCL->Alternative == _Alternative)
		{
		  _Verdict = true;

		  Status = OK;

		  break;
		}
	}

  return Status;
}

// ******************************************************
// Checking for arguments
// ******************************************************
int Message::DoAllTheseValuesExistInSomeCommandLine (vector<string> *_Values, bool &_Verdict)
{
  int Status = ERROR;
  CommandLine *PCL = 0;
  unsigned int NA = 0;
  vector<string> Argument;

  for (unsigned int i = 0; i < NoCL; i++)
	{
	  // Select a command line
	  GetCommandLine (i, PCL);

	  // Get the number of arguments in this command line
	  GetNumberofCommandLineArguments (i, NA);

	  //cout << endl << "i = "<<i<<endl;

	  vector<bool> Matches;

	  for (unsigned int m = 0; m < _Values->size (); m++)
		{
		  Matches.push_back (false);
		}

	  //cout << "NA = "<< NA <<endl;

	  for (unsigned int j = 0; j < NA; j++)
		{
		  //cout << "j = "<<j<<endl;

		  // Select an argument
		  PCL->GetArgument (j, Argument);

		  // Select the values in this argument
		  for (unsigned int k = 0; k < Argument.size (); k++)
			{
			  //cout << "k = "<<k<<endl;

			  // Select a value at the input list
			  for (unsigned int l = 0; l < _Values->size (); l++)
				{
				  //cout << "_Values-at("<<l<<") = "<<_Values->at(l) << "         Argument->at("<<k<<") = "<<Argument.at(k)<<endl;

				  // Check whether this value exists on the selected argument
				  if (Argument.at (k) == _Values->at (l))
					{
					  Matches[l] = true;

					  //cout << "Match!"<<endl;
					}
				  else
					{
					  //cout << "Do not match."<<endl;
					}
				}
			}

		  Argument.clear ();
		}

	  _Verdict = true;

	  for (unsigned int m = 0; m < _Values->size (); m++)
		{
		  //cout << "Matches["<<m<<"] = "<<Matches[m]<<endl;

		  _Verdict = _Verdict && Matches[m];
		}

	  //cout << "The Verdict was (0 = false, 1 = true) = "<<_Verdict<<endl;

	  if (_Verdict == true)
		{
		  break;
		}
	}

  return Status;
}

// Convert the entire message object to an ostream
// Converte o objeto mensagem inteiro para um ostream
ostream &operator<< (ostream &os, Message &M)
{
  for (unsigned int i = 0; i < M.NoCL; i++)
	{
	  os << *M.CommandLines[i];
	}

  if (M.HasPayloadFlag == true && M.PayloadSize > 0)
	{
	  os << endl << "There is a payload of " << M.PayloadSize << " bytes" << endl;

	  //os<<endl;

	  //for (long long j=0;j<M.PayloadSize;j++)
	  //	{
	  //		os<<M.Payload[j];
	  //	}
	}

  return os;
}

// Convert the entire message object to a file
// Converte o objeto mensagem inteiro para um arquivo
fstream &operator<< (fstream &fs, Message &M)
{

  char *buffer;
  long length = 0;

  for (unsigned int i = 0; i < M.NoCL; i++)
	{
	  fs << *M.CommandLines[i];
	}

  if (M.HasPayloadFlag == 1)
	{
	  fs << endl; // Add a blank line to separate the payload from the header.

	  M.PayloadFile.OpenInputFile ();

	  // Read the number of characters in the Payload file.
	  length = M.PayloadFile.tellg ();

	  // Change the position to read from the beginning
	  M.PayloadFile.seekg (0);

	  if (length > 0)
		{
		  buffer = new char[length];

		  M.PayloadFile.read (buffer, length);

		  fs.write (buffer, length);

		  delete[] buffer;
		}

	  M.PayloadFile.CloseFile ();
	}

  return fs;
}

// Convert an stringstream to an objext
// Converte um stringstream da mensagem para um objeto
stringstream &operator>> (stringstream &ss, Message &M)
{
  unsigned int i = 0;
  CommandLine *PCL = 0;
  string Option;
  string ng = "ng";
  long long int Position = 0;
  long int Size = 0;
  int length = 0;
  char *buffer;

  ss.seekg (0);

  char Line[4096];

  while (ss.getline (Line, sizeof (Line), '\n'))
	{
	  if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
		{
		  istringstream ins (Line);

		  M.NewCommandLine (PCL);

		  ins >> *M.CommandLines[i];

		  i++;
		}
	  else
		{
		  break;
		}
	}

  // Stores current position
  Position = ss.tellg ();

  // Get the total size
  Size = ss.str ().length ();

  //cout << "Size = " << Size << endl;

  // Calculates the length of the payload portion
  length = Size - Position;

  //cout << "length = " << length << endl;

  //cout << "Reading from position = " << Position << endl;

  if (length > 1 && Position > 0 && Size > 0)
	{
	  // There is a payload
	  M.HasPayloadFlag = true;

	  // Open the output file
	  M.PayloadFile.OverwriteOutputFile ();

	  // Change the position to write at the begining
	  M.PayloadFile.seekp (0, ios::beg);

	  buffer = new char[length];

	  ss.read (buffer, length);

	  M.PayloadFile.write (buffer, length);

	  M.PayloadFile.CloseFile ();

	  delete[] buffer;
	}

  return ss;
}

// Convert the message file to an objext
// Converte o arquivo da mensagem para um objeto
fstream &operator>> (fstream &fs, Message &M)
{
  unsigned int i = 0;
  CommandLine *PCL = 0;
  string Option;
  char *buffer;
  int length = 0;
  long long int Position = 0;
  long int Size = 0;
  string ng = "ng";

  fs.seekg (0);

  char Line[4096];

  while (fs.getline (Line, sizeof (Line), '\n'))
	{
	  if (Line[0] == 'n' && Line[1] == 'g' && Line[2] == ' ' && Line[3] == '-')
		{
		  istringstream ins (Line);

		  M.NewCommandLine (PCL);

		  ins >> *M.CommandLines[i];

		  i++;
		}
	  else
		{
		  break;
		}
	}

  // Stores current position
  Position = fs.tellg ();

  // Change position to the end
  fs.seekg (ios::cur, ios::end);

  // Get the total size
  Size = fs.tellg ();

  // Calculates the length of the payload portion
  length = Size - Position;

  // Returns the reading pointer to the correct position
  fs.seekg (Position, ios::beg);

  // cout << "Reading from position = " << Position << endl;

  if (length > 0 && Position > 0 && Size > 0)
	{
	  // There is a payload
	  M.HasPayloadFlag = true;

	  // Open the output file
	  M.PayloadFile.OverwriteOutputFile ();

	  // Change the position to write at the beginning
	  M.PayloadFile.seekp (0, ios::beg);

	  buffer = new char[length];

	  fs.read (buffer, length);

	  M.PayloadFile.write (buffer, length);

	  M.PayloadFile.CloseFile ();

	  delete[] buffer;
	}

  return fs;
}
