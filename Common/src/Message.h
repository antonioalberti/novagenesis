/*
	NovaGenesis

	Name:		Message
	Object:		Message
	File:		Message.h
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
#define _MESSAGE_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _FSTREAM_H
#include <fstream>
#endif

#ifndef _VECTOR_H
#include <vector>
#endif

#ifndef _COMMANDLINE_H
#include "CommandLine.h"
#endif

#ifndef _COMMANDLINE1_H
#include "CommandLine.h"
#endif

#ifndef _FILE_H
#include "File.h"
#endif

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

#ifndef _TINYTHREAD_H
#include "tinythread.h"
#endif

#define ERROR 1
#define OK 0

#define DELETED_BY_APP true
#define DELETED_BY_CORE false

using namespace tthread;

class Message {
 private:

  // Tag to allow deleting
  bool Delete;

 public:

  // ******************************************************
  // Attributes
  // ******************************************************

  // Self-certifying name
  string SCN;

  // Type
  short Type;

  // CommandLine container
  CommandLine **CommandLines;

  // Number of command lines
  unsigned int NoCL;

  // Header file handler. Allows to handle a file with the message command lines
  //File 					HeaderFile;

  // Payload file handler. Allows to handle a file with the message payload
  File PayloadFile;

  // Message file handler. Allows to handle a file with the entire message
  File MessageFile;

  // Has payload control flag. True means that the message has a payload.
  bool HasPayloadFlag;

  // Header char array. Can be used to carry a header to memory instead of using a file
  //char 					*Header;

  // Payload char array. Can be used to carry a payload in memory instead of saving it to a file
  char *Payload;

  // Message char array. Can be used to carry the message to memory instead of using a file
  char *Msg;

  // The size of the header in bytes
  //long					HeaderSize;

  // The size of the payload in bytes
  long long PayloadSize;

  // The size of the message in bytes
  long long MessageSize;

  // Time for priority queue comparison
  double Time;

  // Timestamp for performance analysis
  double TimeStamp;

  // Object creation time for performance analysis
  double InstantiationTime;

  // Tag for message tie on priority queue ordering
  unsigned int Tag;

  // Auxiliary flag to enable deletion of *Header array.
  //bool 					DeleteHeaderArray;

  // Auxiliary flag to enable deletion of *Payload array.
  bool DeletePayloadArray;

  // Auxiliary flag to enable deletion of *Message array.
  bool DeleteMessageArray;

  // Instantiation number to check for double deletion
  unsigned int InstantiationNumber;

  // An application avoid deletion of message by turning this flag true. False means that the core will delete the message automatically
  bool ApplicationDeleted;

  // ******************************************************
  // Constructor/destructor
  // ******************************************************

  // Empty message constructor.
  Message (double _Time, short _Type, bool _HasPayload);

  // Simple constructor.
  Message (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path);

  // Complete constructor.
  Message (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path);

  // Copy constructor
  Message (const Message &M);

  // Destructor
  ~Message ();

  // ******************************************************
  // Basic set/get functions
  // ******************************************************

  void
  SetMessage (double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path);

  // Set message's SCN
  void SetSelfCertifyingName (string _SCN);

  // Set message's type
  void SetType (short _Type);

  // Set has payload flag
  void SetHasPayloadFlag (bool _Flag);

  // Set time
  void SetTime (double _Time);

  // Set tag
  void SetTag (unsigned int _Tag);

  // Set header file name
  //void SetHeaderFileName(string _Name);

  // Set payload file name
  void SetPayloadFileName (string _Name);

  // Set payload file name
  void SetMessageFileName (string _Name);

  // Set header file name
  //string GetHeaderFileName();

  // Set payload file name
  string GetPayloadFileName ();

  // Set payload file name
  string GetMessageFileName ();

  // Set header file path
  //void SetHeaderFilePath(string _Path);

  // Set payload file path
  void SetPayloadFilePath (string _Path);

  // Set message file path
  void SetMessageFilePath (string _Path);

  // Set message to be deleted
  void MarkToDelete ();

  // Set message to be deleted
  void UnmarkToDelete ();

  // Set application driven deletion
  void SetApplicationDeletedFlag (bool _Flag);

  // Set header file path
  //string GetHeaderFilePath();

  // Set payload file path
  string GetPayloadFilePath ();

  // Set message file path
  string GetMessageFilePath ();

  // Set header file option
  //void SetHeaderFileOption(string _Option);

  // Set payload file option
  void SetPayloadFileOption (string _Option);

  // Set message file option
  //void SetMessageFileOption(string _Option);

  // Set header file option
  //string GetHeaderFileOption();

  // Set payload file option
  string GetPayloadFileOption ();

  // Set message file option
  //string GetMessageFileOption();

  // Get has payload flag
  bool GetHasPayloadFlag () const;

  // Get message's SCN
  string GetSelfCertifyingName () const;

  // Get message's type
  short GetType () const;

  // Get time
  double GetTime () const;

  // Get tag
  unsigned int GetTag () const;

  // Get application delete flag status
  bool GetDeleteFlag ();

  // Get application delete flag status
  bool GetApplicationDeletedFlag ();

  // Allocate an empty command line and add it on CommandLines container
  int NewCommandLine (CommandLine *&CL);

  // Allocate a copy command line and add it on CommandLines container
  int NewCommandLine (CommandLine *_PCL, CommandLine *&CL);

  // Allocate and add a CommandLine on CommandLines container
  int NewCommandLine (string _Name, string _Alternative, string _Version, CommandLine *&C);

  // Get a CommandLine by its index
  int GetCommandLine (unsigned int _Index, CommandLine *&CL);

  // Get a CommandLine by its name and alternative
  int GetCommandLine (string _Name, string _Alternative, CommandLine *&CL);

  // Get a CommandLine Index
  int GetCommandLineIndex (CommandLine *CL, unsigned int &_Index);

  // Get number of command lines
  int GetNumberofCommandLines (unsigned int &_Number);

  // Get number of arguments
  int GetNumberofCommandLineArguments (unsigned int _CommandLine, unsigned int &_Number);

  // Get number of elements in a certain argument
  int GetNumberofCommandLineArgumentElements (unsigned int _CommandLine, unsigned int _Argument, unsigned int &_Number);

  // Set an element at an Argument
  int
  SetCommandLineArgumentElement (unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string _Value);

  // Get an element at an Argument
  int
  GetCommandLineArgumentElement (unsigned int _CommandLine, unsigned int _Argument, unsigned int _Element, string &_Value);

  // Get header file
  //int GetHeaderFile(File& F);

  // Get payload file
  int GetPayloadFile (File &F);

  // Get message file
  int GetMessageFile (File &F);

  // Set *Header from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
  //int SetHeaderFromCharArray(char* _Value, long _Size);

  // Set *Payload from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
  int SetPayloadFromCharArray (char *_Value, long long _Size);

  // Set *Msg from char array. A copy of the char array is done. If a previous array was being used, it will be deleted.
  int SetMessageFromCharArray (char *_Value, long long _Size);

  // Get *Header from char array. Don't delete[] the array pointer
  //int GetHeaderFromCharArray(char*& _Value);

  // Get *Payload from char array. Don't delete[] the array pointer
  int GetPayloadFromCharArray (char *&_Value);

  // Get *Msg from char array. Don't delete[] the array pointer
  int GetMessageFromCharArray (char *&_Value);

  // Get *Msg from char array. Don't delete[] the array pointer
  int GetMessageFromCharArray (unsigned char *&_Value);

  // Return the current system time in seconds
  double GetSystemTime ();

  // Get the object instantiation time in seconds
  double GetInstantiationTime ();

  // ******************************************************
  // Header format convertions
  // ******************************************************

  // Convert the Header from file to a char array pointed by *Header. The HeaderFile handler needs to be properly configured.
  //int ConvertHeaderFromFileToCharArray();

  // Convert the Header from CommandLines to a char array pointed by *Header. The CommandLines need to be properly configured.
  int ConvertHeaderFromCommandLinesToCharArray ();

  // Convert the Header from char array to CommandLines. The char array pointed by *Header needs to be properly configured.
  //int ConvertHeaderFromCharArrayToCommandLines();

  // Convert the Header from file to CommandLines. The HeaderFile handler needs to be properly configured.
  int ConvertHeaderFromFileToCommandLines ();

  // Convert the Header from char array to file. The char array pointed by *Header needs to be properly configured as well as the HeaderFile handler
  //int ConvertHeaderFromCharArrayToFile();

  // Convert the Header from CommandLines to file. The CommandLines need to be properly configured as well as the HeaderFile handler
  //int ConvertHeaderFromCommandLinesToFile();

  // ******************************************************
  // Payload format convertions
  // ******************************************************

  // Convert the Payload from file to a char array pointed by *Payload. The PayloadFile handler needs to be properly configured.
  int ConvertPayloadFromFileToCharArray ();

  // Convert the Payload from char array to file. The char array pointed by *Payload needs to be properly configured as well as the PayloadFile handler
  int ConvertPayloadFromCharArrayToFile ();

  // Extract Payload char array from message char array. The Msg char array needs to be properly configured.
  int ExtractPayloadCharArrayFromMessageCharArray ();

  // ******************************************************
  // Entire message format convertions
  // ******************************************************

  // Convert the Message from file to CommandLines and Payload file. The MessageFile handler needs to be properly configured
  //int ConvertMessageFromFileToCommandLinesandPayloadFile();

  // Convert the Message from file to a char array pointed by *Msg. The MessageFile handler needs to be properly configured.
  int ConvertMessageFromFileToCharArray ();

  // Convert the Message from char array to a complete Message file. The char array pointed by *Message needs to be properly configured as well as the MessageFile handler
  //int ConvertMessageFromCharArrayToFile();

  // Convert the Message from CommandLines and Payload file to a complete Message file. The CommandLines need to be properly configured as well as the MessageFile handler
  //int ConvertMessageFromCommandLinesandPayloadFileToFile();

  // Convert the Message from CommandLines and Payload file to a char array. The CommandLines need to be properly configured as well as the PayloadFile handler
  int ConvertMessageFromCommandLinesandPayloadFileToCharArray ();

  // Convert the Message from CommandLines and Payload char array to a message char array. The CommandLines need to be properly configured as well as the Payload char array
  int ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray ();

  // Convert the complete message char array into command lines and payload file (if any was set up in advance)
  //int ConvertMessageFromCharArrayToCommandLinesandPayloadFile();

  // Convert the complete message char array into command lines and payload char array
  int ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray ();

  // Convert the complete message char array into command lines and payload char array. Added in May 2015
  int ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray (File *_PF);

  // Convert the complete message char array into command lines and payload char array. Rebuild in May 2015
  int ConvertMessageFromCharArrayToCommandLinesandPayloadCharArray2 ();

  // ******************************************************
  // Message size functions
  // ******************************************************

  // Get header size in bytes. A previous conversion from file or CommandLines to *Header char array is required
  //int GetHeaderSize(long& _Size);

  // Get payload size in bytes. A previous conversion from file to *Payload char array is required
  int GetPayloadSize (long long &_Size);

  // Get message size in bytes. A previous conversion from file and/or CommandLines to *Message char array is required
  int GetMessageSize (long long &_Size);

  // ******************************************************
  // Checking for command lines
  // ******************************************************
  int DoesThisCommandLineExistsInMessage (string _Name, string _Alternative, bool &_Verdict);

  // ******************************************************
  // Checking for arguments
  // ******************************************************

  // Check if the values on the vector exist on some command line. They all need to exist in the same command line
  int DoAllTheseValuesExistInSomeCommandLine (vector<string> *_Values, bool &_Verdict);

  // ******************************************************
  // Operators overloading
  // ******************************************************

  // Overloading ostream operator
  friend ostream &operator<< (ostream &os, Message &M);

  // Overloading fstream operator
  friend fstream &operator<< (fstream &fs, Message &M);

  // Overloading stringstream operator
  friend stringstream &operator>> (stringstream &fs, Message &M);

  // Overloading fstream operator
  friend fstream &operator>> (fstream &fs, Message &M);
};

#endif
