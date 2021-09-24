/*
	NovaGenesis
	
	Name:		Message Builder
	Object:		MessageBuilder
	File:		MessageBuilder.h
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

#ifndef _MESSAGEBUILDER_H
#define _MESSAGEBUILDER_H

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _MATH_H
#include <math.h>
#endif

#ifndef _INTTYPES_H
#include <inttypes.h>
#endif

#ifndef _TUPLE_H
#include "Tuple.h"
#endif

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

#define ERROR 1
#define OK 0

using namespace std;

class Process;
class Block;

class MessageBuilder
{
	private:

		Process *PP;

		// Auxiliary prime numbers array
		unsigned int 			primes[16];

	public:

		// Constructor
		MessageBuilder(Process *_PP);

		// Destructor
		~MessageBuilder();

		// ------------------------------------------------------------------------------------------------------------------------------
		// Functions to create customized command lines
		// ------------------------------------------------------------------------------------------------------------------------------

		// Creates a message connection less command line header
		// ng -msg --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
		int NewConnectionLessCommandLine(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
				Message *_M, CommandLine *& _PCL);

		// Creates a self-certifying name command line header without ack
		// ng -scn --seq _Version [ < 1 string SEQ_SCN > ]
		int NewSCNCommandLine(string _Version, string _SCN,
				Message *_M, CommandLine *& _PCL);

		// Creates a self-certifying name command line header with ack
		// ng -scn --ack _Version [ < 2 string SEQ_SCN ACK_SCN > ]
		int NewSCNCommandLine(string _Version, string _SCN, string _AckSCN,
				Message *_M, CommandLine *& _PCL);

		// Creates a self-certifying name command line header with multiple ack
		// ng -scn --ack _Version [ < (N+1) string SEQ_SCN ACK_SCN_1 ... ACK_SCN_N > ]
		int NewSCNCommandLine(string _Version, string _SCN, vector<string> *_AckSCNs,
				Message *_M, CommandLine *& _PCL);

		// Creates an IPC hello command line header
		// ng -hello --ipc _Version [ < 2 Peer_Key Peer_LN > ]
		int NewIPCHelloCommandLine(string _Alternative, string _Version, key_t _Key, string _LN, Message *_M, CommandLine *& _PCL);

		// Creates an IHC hello command line header
		// ng -hello --ihc _Version GW_SCN HT_SCN Legacy_Stack Legacy_Address
		int NewIHCHelloCommandLine(string _Alternative, string _Version, string _GW_SCN, string _HT_SCN, string _Stack, string _Interface, string _Identifier, Message *_M, CommandLine *& _PCL);

		// Creates an IHC hello command line header
		// ng -hello --ihc _Version GW_SCN HT_SCN Legacy_Stack Legacy_Address
		int NewIHCHelloCommandLine(string _Alternative, string _Version, string _GW_SCN, string _HT_SCN, string _Core_SCN, string _Stack, string _Interface, string _Identifier, string _PSS_HID, string _PSS_OSID, string _PSS_PID, string _PS_BID, Message *_M, CommandLine *& _PCL);

		// Creates a time command line header
		// ng -time --s _Version [ < 1 string _Time > ]
		int NewTimeCommandLine(string _Version, double _Time,
				Message *_M, CommandLine *& _PCL);

		// Creates a publication without notification command line header
		// ng -pub --bind _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > ]
		int NewPublicationWithoutNotificationCommandLine(string _Version, unsigned int _Category, string _Key, vector<string>* _Values,
				Message *_M, CommandLine *& _PCL);

		// Creates a publication with notification command line header
		// ng -pub --notify _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > < _PubNotifySize string pub HID OSID PID BID > ... < _SubNotifySize string sub HID OSID PID BID > ]
		int NewPublicationWithNotificationCommandLine(string _Version, unsigned int _Category, string _Key, vector<string>* _Values,
				vector<Tuple*> * _PubNotify, vector<Tuple *> * _SubNotify, Message *_M, CommandLine *& _PCL);

		// Creates a subscription command line header
		// ng -sub --bind _Version [ < 1 string _Category > < _SCNsSize string S_1 ... S_SCNsSize > ]
		int NewSubscriptionCommandLine(string _Version, unsigned int _Category, vector<string>* _Keys,
				Message *_M, CommandLine *& _PCL);

		// TODO: FIXP/Update - The name of this function was changed to better represent what it does
		// Creates a publication/subscription command line header
		// ng -notify --s _Version [ < 1 string _Category > < _KeysSize string S_1 ... S_KeysSize > ]
		int NewPubNotificationCommandLine(string _Version, unsigned int _Category, vector<string>* _Keys, Tuple * _Publisher,
				Message *_M, CommandLine *& _PCL);

		// Creates a standard status command line header
		// ng -status --s _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > ]
		int NewStatusCommandLine(string _Version, string _AckCommandName, string _AckCommandAlt, unsigned int _StatusCode,
				Message *_M, CommandLine *& _PCL);

		// Creates a status command line header with an additional vector of related SCNs
		// ng -status --scns _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > < _RelatedSCNsSize string S_1 ... S_RelatedSCNsSize > ]
		int NewStatusCommandLine(string _Version, string _AckCommandName, string _AckCommandAlt, unsigned int _StatusCode, vector<string> *RelatedSCNs,
				Message *_M, CommandLine *& _PCL);

		// Creates a common command line header for store and delivery commands
		// ng _Name _Alternative _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > ]
		int NewCommonCommandLine(string _Name, string _Alternative, string _Version, unsigned int _Category, string _Key, vector<string>* _Values,
				Message *_M, CommandLine *& _PCL);

		// **************************************************
		// Hash(Legible Names)
		// **************************************************

		// ng -store --bind _Version [ < 1 string _Category > < 1 string Hash("Legible Name") > < 1 string SCN > ]
		int NewStoreBindingCommandLineFromHashLNToSCN(string _Version, unsigned int _Category, string _LN, string _SCN, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string _Category > < 1 string SCN > < 1 string Hash("Legible Name") > ]
		int NewStoreBindingCommandLineSCNToHashLN(string _Version, unsigned int _Category, string _SCN, string _LN, Message *_M, CommandLine *& _PCL);

		// **************************************************
		// Block
		// **************************************************

		// ng -store --bind _Version [ < 1 string 1 > < 1 string "Block Legible Name" > < 1 string Hash("Block Legible Name") > ]
		int NewStoreBindingCommandLineFromBLNToHashBLN(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string Hash("Block Legible Name") > < 1 string "Block Legible Name" > ]
		int NewStoreBindingCommandLineFromHashBLNToBLN(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string Hash("Block Legible Name") > < 1 string BID > ]
		int NewStoreBindingCommandLineFromHashBLNToBID(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string BID > < 1 string Hash("Block Legible Name") > ]
		int NewStoreBindingCommandLineFromBIDToHashBLN(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string BID > < 1 string Index > ]
		int NewStoreBindingCommandLineFromBIDToBlocksIndex(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string Hash("Block Legible Name") > < 1 string "Block Legible Name" > ]
		int NewStoreBindingCommandLineFromBlocksIndexToBID(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL);

		// **************************************************
		// Process
		// **************************************************

		// ng -store --bind _Version [ < 1 string 1 > < 1 string "Process Legible Name" > < 1 string Hash("Process Legible Name") > ]
		int NewStoreBindingCommandLineFromPLNToHashPLN(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string Hash("Process Legible Name") > < 1 string "Process Legible Name" > ]
		int NewStoreBindingCommandLineFromHashPLNToPLN(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string Hash("Process Legible Name") > < 1 string PID > ]
		int NewStoreBindingCommandLineFromHashPLNToPID(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string PID > < 1 string Hash("Process Legible Name") > ]
		int NewStoreBindingCommandLineFromPIDToHashPLN(string _Version, Message *_M, CommandLine *& _PCL);

		// **************************************************
		// Block and Process
		// **************************************************

		// ng -store --bind _Version [ < 1 string 5 > < 1 string PID > < 1 string BID > ]
		int NewStoreBindingCommandLineFromPIDToBID(string _Version, Block *_PB, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 5 > < 1 string BID > < 1 string PID > ]
		int NewStoreBindingCommandLineFromBIDToPID(string _Version, Block *_PB, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string _Category > < 1 string SCN1 > < 1 string SCN2 > ]
		int NewStoreBindingCommandLineFromSCNToSCN(string _Version, unsigned int _Category, string _SCN1, string _SCN2, Message *_M, CommandLine *& _PCL);

		// **************************************************
		// Process, Operating System, and Host
		// **************************************************

		// ng -store --bind _Version [ < 1 string 1 > < 1 string "Operating System Legible Name" > < 1 string Hash("OS Legible Name") > ]
		int NewStoreBindingCommandLineFromOSLNToHashOSLN(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string Hash"Operating System Legible Name" > < 1 string "OS Legible Name" > ]
		int NewStoreBindingCommandLineFromHashOSLNToOSLN(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 5 > < 1 string OSID > < 1 string PID > ]
		int NewStoreBindingCommandLineFromOSIDToPID(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 5 > < 1 string OSID > < 1 string PID > ]
		int NewStoreBindingCommandLineFromOSIDToPID(string _Version, string _PID, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 5 > < 1 string PID > < 1 string OSID > ]
		int NewStoreBindingCommandLineFromPIDToOSID(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 0 > < 1 string "Host Legible Name" > < 1 string Hash("Host Legible Name") > ]
		int NewStoreBindingCommandLineFromHLNToHashHLN(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 0 > < 1 string Hash("Host Legible Name") > < 1 string "Host Legible Name" > ]
		int NewStoreBindingCommandLineFromHashHLNToHLN(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 7 > < 1 string OSID > < 1 string HID > ]
		int NewStoreBindingCommandLineFromOSIDToHID(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 6 > < 1 string HID > < 1 string OSID > ]
		int NewStoreBindingCommandLineFromHIDToOSID(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 6 > < 1 string HID > < 1 string PID > ]
		int NewStoreBindingCommandLineFromHIDToPID(string _Version, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 7 > < 1 string PID > < 1 string HID > ]
		int NewStoreBindingCommandLineFromPIDToHID(string _Version, Message *_M, CommandLine *& _PCL);

		// **************************************************
		// Limiters related
		// **************************************************

		// Note: The representative SCN is the SCN of the entity that represents a certain limiter (i.e. _Limiter)

		// ng -store --bind _Version [ < 1 string 0 > < 1 string "Limiter" > < 1 string Hash("Limiter") > ]
		int NewStoreBindingCommandLineFromLimiterToHashLimiter(string _Version, string _Limiter, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 1 > < 1 string Hash("Limiter") > < 1 string "Limiter" > ]
		int NewStoreBindingCommandLineFromHashLimiterToLimiter(string _Version, string _Limiter, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 2 > < 1 string Hash("Limiter") > < 1 string Representative SCN > ]
		int NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN(string _Version, string _Limiter, string _RepresentativeSCN, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 3 > < 1 string Hash("Limiter") > < 1 string Representative SCN > ]
		int NewStoreBindingCommandLineFromRepresentativeSCNToHashLimiter(string _Version, string _RepresentativeSCN, string _Limiter, Message *_M, CommandLine *& _PCL);

		// **************************************************
		// Related to connectivity
		// **************************************************

		// ng -store --bind _Version [ < 1 string 15 > < 1 string HID > < 1 string Identifier > ]
		int NewStoreBindingCommandLineFromHIDToIdentifier(string _Version, string _HID, string _Identifier, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 16 > < 1 string _Identifier > < 1 string HID > ]
		int NewStoreBindingCommandLineFromIdentifierToHID(string _Version, string _Identifier, string _HID, Message *_M, CommandLine *& _PCL);

		// ng -store --bind _Version [ < 1 string 17 > < 1 string _Identifier > < 1 string SID > ]
		int NewStoreBindingCommandLineFromIdentifierToSID(string _Version, string _Identifier, int _SID, Message *_M, CommandLine *& _PCL);

		// **********************
		// Other command lines
		// **********************

		// ng -get --bind _Version [ < 1 string _Category > < 1 string _Key > ]
		int NewGetCommandLine(string _Version, unsigned int _Category, string _Key, Message *_M, CommandLine *& _PCL);

		// ng -run --proc _Version [ < 1 string _Procedure > ]
		int NewRunProcedureCommandLine(string _Version, string _Procedure, Message *_M, CommandLine *& _PCL);

		// The following informational header can be used to provide additional information regarding the message
		// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
		int NewInfoPayloadCommandLine(string _Version, vector<string>* _Values, Message *_M, CommandLine *& _PCL);

		// ------------------------------------------------------------------------------------------------------------------------------
		// Functions to create customized messages
		// ------------------------------------------------------------------------------------------------------------------------------

		// Creates a store binding message
		int NewConnectionLessStoreBindingMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
				unsigned int _Category, string _Key, vector<string>* _Values,
				Block *_PB, Message *&_M);

		// Creates a get binding message
		int NewConnectionLessGetBindingMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
				unsigned int _Category, string _Key,
				Block *_PB, Message *&_M);

		// Creates a delivery binding message
		int NewConnectionLessDeliveryBindingMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
				unsigned int _Category, string _Key, vector<string>* _Values,
				Block *_PB, Message *&_M);

		// Creates a status message
		int NewConnectionLessStatusMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
				string _AckCommandName, string _AckCommandAlt, unsigned int _StatusCode,
				Block *_PB, Message *&_M);

		// Creates a run procedure message
		int NewConnectionLessRunMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
				string _Procedure,
				Block *_PB, Message *_M);

		// ------------------------------------------------------------------------------------------------------------------------------
		// Auxiliary functions and variables
		// ------------------------------------------------------------------------------------------------------------------------------

		int StringToInt(string _String);

		// Convert int to string
		string IntToString(int _Int);

		// Convert double to string
		string DoubleToString(double _Double);

		// Convert unsigned int to string
		string UnsignedIntToString(unsigned int _Int);

		// Return the current time in seconds
		double GetTime();

		// Generate a self-certified name from a char array
		int GenerateSCNFromCharArrayBinaryPatterns4Bytes(const char *_Input, long long _Size, string &_SCN);

		// Generate a self-certified name from a char array
		int GenerateSCNFromCharArrayBinaryPatterns16Bytes(const char *_Input, long long _Size, string &_SCN);

		// Generate a self-certified name from a char array
		int GenerateSCNFromCharArrayBinaryPatterns4Bytes(string _Input, string &_SCN);

		// Generate a self-certified name from a char array
		int GenerateSCNFromCharArrayBinaryPatterns16Bytes(string _Input, string &_SCN);


		// Generate a self-certified name from a char array
		int GenerateSCNFromCharArrayBinaryPatterns(const char *_Input, long long _Size, string &_SCN);

		// Generate a self-certified name from a char array
		int GenerateSCNFromCharArrayBinaryPatterns(string _Input, string &_SCN);
};

#endif






