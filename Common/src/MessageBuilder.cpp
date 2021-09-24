/*
	NovaGenesis

	Name:		Message Builder
	Object:		MessageBuilder
	File:		MessageBuilder.cpp
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
#include "MessageBuilder.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _MURMURHASH3_H_
#include "MurmurHash3.h"
#endif

MessageBuilder::MessageBuilder(Process *_PP)
{
	PP=_PP;

	// Generate the required prime numbers
    unsigned int n_primes = 0;

    for (unsigned int z = 2; n_primes < 16; z++)
    	{
        	for (unsigned int w = 0; w < n_primes; w++)
        		{
        			if (z % primes[w] == 0)
        				{
        					goto not_prime;
        				}
        		}

        	primes[n_primes] = z;

        	n_primes++;

        	not_prime:
            	;
    	}
}

MessageBuilder::~MessageBuilder()
{
}

// ------------------------------------------------------------------------------------------------------------------------------
// Functions to create customized command lines
// ------------------------------------------------------------------------------------------------------------------------------

// Creates a message connection less command line header
// ng -m --cl _Version [ < _LimitersSize string S_1 ... S_LimitersSize > < _SourcesSize string S_1 ... S_SourcesSize > < _DestinationsSize string S_1 ... S_Destinations > ]
int MessageBuilder::NewConnectionLessCommandLine(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
		Message *_M, CommandLine *& _PCL)
{
	int	Status=ERROR;

	if (_Limiters != 0 && _Sources !=0 && _Destinations != 0)
		{
			unsigned int _LimitersSize=_Limiters->size();
			unsigned int _SourcesSize=_Sources->size();
			unsigned int _DestinationsSize=_Destinations->size();

			if (_LimitersSize > 0 && _SourcesSize > 0 && _DestinationsSize > 0)
				{
					_M->NewCommandLine("-m","--cl",_Version,_PCL);

					// Creates the first argument
					_PCL->NewArgument(_LimitersSize);

					// Configures the first argument
					for (unsigned int i=0; i<_LimitersSize; i++)
						{
							// Set the value on the first argument
							_PCL->SetArgumentElement(0,i,_Limiters->at(i));
						}

					// Creates the second argument
					_PCL->NewArgument(_SourcesSize);

					// Configures the second argument
					for (unsigned int j=0; j<_SourcesSize; j++)
						{
							// Set the value on the second argument
							_PCL->SetArgumentElement(1,j,_Sources->at(j));
						}

					// Creates the third argument
					_PCL->NewArgument(_DestinationsSize);

					// Configures the third argument
					for (unsigned int k=0; k<_DestinationsSize; k++)
						{
							// Set the value on the third argument
							_PCL->SetArgumentElement(2,k,_Destinations->at(k));
						}

					Status=OK;
				}
		}

	return Status;
}

// Creates a self-certyfing name command line header
// ng -scn --seq _Version [ < 1 string SEQ_SCN > ]
int MessageBuilder::NewSCNCommandLine(string _Version, string _SCN, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-scn","--seq",_Version,_PCL);

	_PCL->NewArgument(1);

	_PCL->SetArgumentElement(0,0,_SCN);

	return Status;
}

// Creates a self-certyfing name command line header
// ng -scn --ack _Version [ < 2 string SEQ_SCN string ACK_SCN > ]
int MessageBuilder::NewSCNCommandLine(string _Version, string _SCN, string _AckSCN, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-scn","--ack",_Version,_PCL);

	_PCL->NewArgument(2);

	_PCL->SetArgumentElement(0,0,_SCN);

	_PCL->SetArgumentElement(0,1,_AckSCN);

	return Status;
}

// Creates a self-certifying name command line header with multiple ack
// ng -scn --ack _Version [ < (N+1) string SEQ_SCN ACK_SCN_1 ... ACK_SCN_N > ]
int NewSCNCommandLine(string _Version, string _SCN, vector<string> *_AckSCNs, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-scn","--ack",_Version,_PCL);

	_PCL->NewArgument(_AckSCNs->size()+1);

	_PCL->SetArgumentElement(0,0,_SCN);

	for (unsigned int i=1; i<_AckSCNs->size(); i++)
		{
			_PCL->SetArgumentElement(0,i,_AckSCNs->at(i));
		}

	return Status;
}

// Creates a hello command line header
// ng -hello --ipc _Version [ < 2 Peer_Key Peer_LN > ]
int MessageBuilder::NewIPCHelloCommandLine(string _Alternative, string _Version, key_t _Key, string _LN, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-hello",_Alternative,_Version,_PCL);

	_PCL->NewArgument(2);

	_PCL->SetArgumentElement(0,0,IntToString(_Key));

	_PCL->SetArgumentElement(0,1,_LN);

	return Status;
}

// Creates a hello command line header
// ng -hello --ihc _Version GW_SCN HT_SCN _Stack _Interface _Identifier
int MessageBuilder::NewIHCHelloCommandLine(string _Alternative, string _Version, string _GW_SCN, string _HT_SCN, string _Stack, string _Interface, string _Identifier, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-hello",_Alternative,_Version,_PCL);

	_PCL->NewArgument(5);

	_PCL->SetArgumentElement(0,0,_GW_SCN);

	_PCL->SetArgumentElement(0,1,_HT_SCN);

	_PCL->SetArgumentElement(0,2,_Stack);

	_PCL->SetArgumentElement(0,3,_Interface);

	_PCL->SetArgumentElement(0,4,_Identifier);

	return Status;
}

// Creates a hello command line header
// ng -hello --ihc _Version GW_SCN HT_SCN _Stack _Interface _Identifier
int MessageBuilder::NewIHCHelloCommandLine(string _Alternative, string _Version, string _GW_SCN, string _HT_SCN, string _Core_SCN, string _Stack, string _Interface, string _Identifier, string _PSS_HID, string _PSS_OSID, string _PSS_PID, string _PS_BID, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-hello",_Alternative,_Version,_PCL);

	_PCL->NewArgument(6);

	_PCL->SetArgumentElement(0,0,_GW_SCN);

	_PCL->SetArgumentElement(0,1,_HT_SCN);

	_PCL->SetArgumentElement(0,2,_Core_SCN);

	_PCL->SetArgumentElement(0,3,_Stack);

	_PCL->SetArgumentElement(0,4,_Interface);

	_PCL->SetArgumentElement(0,5,_Identifier);

	_PCL->NewArgument(4);

	_PCL->SetArgumentElement(1,0,_PSS_HID);

	_PCL->SetArgumentElement(1,1,_PSS_OSID);

	_PCL->SetArgumentElement(1,2,_PSS_PID);

	_PCL->SetArgumentElement(1,3,_PS_BID);

	return Status;
}

// Creates a time command line header
// ng -time --s _Version [ < 1 string _Time > ]
int MessageBuilder::NewTimeCommandLine(string _Version, double _Time, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-time","--s",_Version,_PCL);

	_PCL->NewArgument(1);

	_PCL->SetArgumentElement(0,0,DoubleToString(_Time));

	return Status;
}

// Creates a publication without notification command line header
// ng -p --b _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > ]
int MessageBuilder::NewPublicationWithoutNotificationCommandLine(string _Version, unsigned int _Category, string _Key, vector<string>* _Values,
		Message *_M, CommandLine *& _PCL)
{
	int	Status=ERROR;

	if (_Values != 0)
		{
			unsigned int _ValuesSize=_Values->size();

			if (_ValuesSize > 0)
				{
					_M->NewCommandLine("-p","--b",_Version,_PCL);

					// Creates the first argument
					_PCL->NewArgument(1);

					_PCL->SetArgumentElement(0,0,IntToString(_Category));

					// Creates the second argument
					_PCL->NewArgument(1);

					// Set the value on the second argument
					_PCL->SetArgumentElement(1,0,_Key);

					// Creates the third argument
					_PCL->NewArgument(_ValuesSize);

					// Set the value on the third argument
					for (unsigned int k=0; k<_ValuesSize; k++)
						{
							_PCL->SetArgumentElement(2,k,_Values->at(k));
						}

					Status=OK;
				}
		}

	return Status;
}

// ng -p --notify _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > < _PubNotifySize string pub HID OSID PID BID > ... < _SubNotifySize string sub HID OSID PID BID > ]
int MessageBuilder::NewPublicationWithNotificationCommandLine(string _Version, unsigned int _Category, string _Key, vector<string>* _Values,
		vector<Tuple*> * _PubNotify, vector<Tuple *> * _SubNotify, Message *_M, CommandLine *& _PCL)
{
	int				Status=ERROR;
	Tuple			*PT=0;
	unsigned int	TupleSize=0;

	if (_Values != 0 && _PubNotify !=0 && _SubNotify != 0)
		{
			unsigned int ValuesSize=_Values->size();
			unsigned int PubNotifySize=_PubNotify->size();
			unsigned int SubNotifySize=_SubNotify->size();

			if (ValuesSize > 0 && (PubNotifySize > 0 || SubNotifySize > 0))
				{
					_M->NewCommandLine("-p","--notify",_Version,_PCL);

					// Creates the first argument
					_PCL->NewArgument(1);

					_PCL->SetArgumentElement(0,0,IntToString(_Category));

					// Creates the second argument
					_PCL->NewArgument(1);

					// Set the value on the second argument
					_PCL->SetArgumentElement(1,0,_Key);

					// Creates the third argument
					_PCL->NewArgument(ValuesSize);

					// Set the value on the third argument
					for (unsigned int k=0; k<ValuesSize; k++)
						{
							_PCL->SetArgumentElement(2,k,_Values->at(k));
						}

					for (unsigned int i=0; i<PubNotifySize; i++)
						{
							PT=_PubNotify->at(i);

							TupleSize=PT->Values.size();

							// Creates the next argument
							_PCL->NewArgument(TupleSize);

							for (unsigned int k=0; k<TupleSize; k++)
								{
									_PCL->SetArgumentElement(3+i,k,PT->Values[k]);
								}
						}

					for (unsigned int j=0; j<SubNotifySize; j++)
						{
							PT=_SubNotify->at(j);

							TupleSize=PT->Values.size();

							// Creates the next argument
							_PCL->NewArgument(TupleSize);

							for (unsigned int k=0; k<TupleSize; k++)
								{
									_PCL->SetArgumentElement(2+PubNotifySize+j,k,PT->Values[k]);
								}
						}

					Status=OK;
				}
		}

	return Status;
}

// Creates a publication/subscription command line header
// ng -s --s _Version [ < 1 string _Category > < _SCNsSize string S_1 ... S_SCNsSize > ]
int MessageBuilder::NewSubscriptionCommandLine(string _Version, unsigned int _Category, vector<string>* _Keys, Message *_M, CommandLine *& _PCL)
{
	int	Status=ERROR;

	if (_Keys != 0)
		{
			unsigned int _SCNsSize=_Keys->size();

			_M->NewCommandLine("-s","--b",_Version,_PCL);

			_PCL->NewArgument(1);

			_PCL->SetArgumentElement(0,0,IntToString(_Category));

			if (_SCNsSize > 0)
				{
					_PCL->NewArgument(_SCNsSize);

					for (unsigned int i=0; i<_SCNsSize; i++)
						{
							_PCL->SetArgumentElement(1,i,_Keys->at(i));
						}
				}

			Status=OK;
		}

	return Status;
}

// Creates a publication/subscription command line header
// ng -notify --s _Version [ < 1 string _Category > < _KeysSize string S_1 ... S_KeysSize > < _TupleSize string T_1 ... T_TupleSize > ]
int MessageBuilder::NewPubNotificationCommandLine(string _Version, unsigned int _Category, vector<string>* _Keys, Tuple *_Publisher, Message *_M, CommandLine *& _PCL)
{
	int	Status=ERROR;

	if (_Keys != 0)
		{
			unsigned int _KeysSize=_Keys->size();
			unsigned int _PublisherTupleSize=_Publisher->Values.size();

			_M->NewCommandLine("-notify","--s",_Version,_PCL);

			_PCL->NewArgument(1);

			_PCL->SetArgumentElement(0,0,IntToString(_Category));

			if (_KeysSize > 0)
				{
					_PCL->NewArgument(_KeysSize);

					for (unsigned int i=0; i<_KeysSize; i++)
						{
							_PCL->SetArgumentElement(1,i,_Keys->at(i));
						}
				}

			if (_PublisherTupleSize > 0)
				{
					// Creates the next argument
					_PCL->NewArgument(_PublisherTupleSize);

					for (unsigned int k=0; k<_PublisherTupleSize; k++)
						{
							_PCL->SetArgumentElement(2,k,_Publisher->Values[k]);
						}
				}

			Status=OK;
		}

	return Status;
}

// Creates a standard status command line header
// ng -st --s _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > ]
int MessageBuilder::NewStatusCommandLine(string _Version, string _AckCommandName, string _AckCommandAlt, unsigned int _StatusCode,
		Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-st","--s",_Version,_PCL);

	// First argument
	_PCL->NewArgument(2);

	_PCL->SetArgumentElement(0,0,_AckCommandName);

	_PCL->SetArgumentElement(0,1,_AckCommandAlt);

	// Second argument
	_PCL->NewArgument(1);

	_PCL->SetArgumentElement(1,0,UnsignedIntToString(_StatusCode));

	return Status;
}

// Creates a status command line header with an additional vector of related SCNs
// ng -st --scns _Version [ < 2 string _AckCommandName _AckCommandAlt > < 1 string _StatusCode > < _RelatedSCNsSize string S_1 ... S_RelatedSCNsSize > ]
int MessageBuilder::NewStatusCommandLine(string _Version, string _AckCommandName, string _AckCommandAlt, unsigned int _StatusCode, vector<string> *RelatedSCNs,
		Message *_M, CommandLine *& _PCL)
{
	int	Status=ERROR;

	_M->NewCommandLine("-st","--scns",_Version,_PCL);

	// First argument
	_PCL->NewArgument(2);

	_PCL->SetArgumentElement(0,0,_AckCommandName);

	_PCL->SetArgumentElement(0,1,_AckCommandAlt);

	// Second argument
	_PCL->NewArgument(1);

	_PCL->SetArgumentElement(1,0,UnsignedIntToString(_StatusCode));

	if (RelatedSCNs->size() > 0)
		{
			// Third argument
			_PCL->NewArgument(RelatedSCNs->size());

			for (unsigned int i=0; i<RelatedSCNs->size(); i++)
				{
					_PCL->SetArgumentElement(2,i,RelatedSCNs->at(i));
				}

			Status=OK;
		}

	return Status;
}

// Creates a common command line header for store and delivery commands
// ng _Name _Alternative _Version [ < 1 string _Category > < 1 string _Key > < _ValuesSize string S_1 ... S_ValuesSize > ]
int MessageBuilder::NewCommonCommandLine(string _Name, string _Alternative, string _Version, unsigned int _Category, string _Key, vector<string>* _Values,
		Message *_M, CommandLine *& _PCL)
{
	int	Status=ERROR;

	if (_Values != 0)
		{
			unsigned int _ValuesSize=_Values->size();

			if (_ValuesSize > 0)
				{
					_M->NewCommandLine(_Name,_Alternative,_Version,_PCL);

					// First argument
					_PCL->NewArgument(1);

					_PCL->SetArgumentElement(0,0,IntToString(_Category));

					// Second argument
					_PCL->NewArgument(1);

					_PCL->SetArgumentElement(1,0,_Key);

					// Third argument
					_PCL->NewArgument(_ValuesSize);

					// Set the value on the third argument
					for (unsigned int k=0; k<_ValuesSize; k++)
						{
							_PCL->SetArgumentElement(2,k,_Values->at(k));
						}

					Status = OK;
				}
		}

	return Status;
}

// **************************************************
// Hash(Legible Names)
// **************************************************

// ng -sr --b _Version [ < 1 string _Category > < 1 string Hash("Legible Name") > < 1 string SCN > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashLNToSCN(string _Version, unsigned int _Category, string _LN, string _SCN, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashLN;

	GenerateSCNFromCharArrayBinaryPatterns(_LN,HashLN);

	Values.push_back(_SCN);

	NewCommonCommandLine("-sr","--b",_Version,_Category,HashLN,&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 3 > < 1 string SCN > < 1 string Hash("Legible Name") > ]
int MessageBuilder::NewStoreBindingCommandLineSCNToHashLN(string _Version, unsigned int _Category, string _SCN, string _LN, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashLN;

	GenerateSCNFromCharArrayBinaryPatterns(_LN,HashLN);

	Values.push_back(HashLN);

	NewCommonCommandLine("-sr","--b",_Version,_Category,_SCN,&Values,_M, _PCL);

	return Status;
}

// **************************************************
// Block
// **************************************************

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string "Block Legible Name" > < 1 string Hash("Block Legible Name") > ]
int MessageBuilder::NewStoreBindingCommandLineFromBLNToHashBLN(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashBLN;

	GenerateSCNFromCharArrayBinaryPatterns(_PB->GetLegibleName(),HashBLN);

	Values.push_back(HashBLN);

	NewCommonCommandLine("-sr","--b",_Version,0,_PB->GetLegibleName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string Hash("Block Legible Name") > < 1 string "Block Legible Name" > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashBLNToBLN(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashBLN;

	GenerateSCNFromCharArrayBinaryPatterns(_PB->GetLegibleName(),HashBLN);

	Values.push_back(_PB->GetLegibleName());

	NewCommonCommandLine("-sr","--b",_Version,1,HashBLN,&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string Hash("Block Legible Name") > < 1 string BID > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashBLNToBID(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashBLN;

	GenerateSCNFromCharArrayBinaryPatterns(_PB->GetLegibleName(),HashBLN);

	Values.push_back(_PB->GetSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,2,HashBLN,&Values,_M, _PCL);

	return Status;
}


// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string BID > < 1 string Hash("Block Legible Name") > ]
int MessageBuilder::NewStoreBindingCommandLineFromBIDToHashBLN(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashBLN;

	GenerateSCNFromCharArrayBinaryPatterns(_PB->GetLegibleName(),HashBLN);

	Values.push_back(HashBLN);

	NewCommonCommandLine("-sr","--b",_Version,3,_PB->GetSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string BID > < 1 string Index > ]
int MessageBuilder::NewStoreBindingCommandLineFromBIDToBlocksIndex(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(UnsignedIntToString(_PB->GetIndex()));

	NewCommonCommandLine("-sr","--b",_Version,13,_PB->GetSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string Hash("Block Legible Name") > < 1 string "Block Legible Name" > ]
int MessageBuilder::NewStoreBindingCommandLineFromBlocksIndexToBID(string _Version, Block* _PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(_PB->GetSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,14,UnsignedIntToString(_PB->GetIndex()),&Values,_M, _PCL);

	return Status;
}


// **************************************************
// Process
// **************************************************

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string "Process Legible Name" > < 1 string Hash("Process Legible Name") > ]
int MessageBuilder::NewStoreBindingCommandLineFromPLNToHashPLN(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashPLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetLegibleName(),HashPLN);

	Values.push_back(HashPLN);

	NewCommonCommandLine("-sr","--b",_Version,0,PP->GetLegibleName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string Hash("Process Legible Name") > < 1 string "Process Legible Name" > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashPLNToPLN(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashPLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetLegibleName(),HashPLN);

	Values.push_back(PP->GetLegibleName());

	NewCommonCommandLine("-sr","--b",_Version,1,HashPLN,&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string Hash("Process Legible Name") > < 1 string PID > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashPLNToPID(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashPLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetLegibleName(),HashPLN);

	Values.push_back(PP->GetSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,2,HashPLN,&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string PID > < 1 string Hash("Process Legible Name") > ]
int MessageBuilder::NewStoreBindingCommandLineFromPIDToHashPLN(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashPLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetLegibleName(),HashPLN);

	Values.push_back(HashPLN);

	NewCommonCommandLine("-sr","--b",_Version,3,PP->GetSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// **************************************************
// Block and Process
// **************************************************

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string PID > < 1 string BID > ]
int MessageBuilder::NewStoreBindingCommandLineFromPIDToBID(string _Version, Block *_PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(_PB->GetSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,5,PP->GetSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string BID > < 1 string PID > ]
int MessageBuilder::NewStoreBindingCommandLineFromBIDToPID(string _Version, Block *_PB, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(PP->GetSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,5,_PB->GetSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 5 > < 1 string SCN1 > < 1 string SCN2 > ]
int MessageBuilder::NewStoreBindingCommandLineFromSCNToSCN(string _Version, unsigned int _Category, string _SCN1, string _SCN2, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(_SCN2);

	NewCommonCommandLine("-sr","--b",_Version,_Category,_SCN1,&Values,_M, _PCL);

	return Status;
}

// **************************************************
// Process, Operating System, and Host
// **************************************************

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string "Operating System Legible Name" > < 1 string Hash("OS Legible Name") > ]
int MessageBuilder::NewStoreBindingCommandLineFromOSLNToHashOSLN(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashOSLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetOperatingSystemLegibleName(),HashOSLN);

	Values.push_back(HashOSLN);

	NewCommonCommandLine("-sr","--b",_Version,0,PP->GetOperatingSystemLegibleName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string Hash"Operating System Legible Name" > < 1 string "OS Legible Name" > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashOSLNToOSLN(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashOSLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetOperatingSystemLegibleName(),HashOSLN);

	Values.push_back(PP->GetOperatingSystemLegibleName());

	NewCommonCommandLine("-sr","--b",_Version,1,HashOSLN,&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 5 > < 1 string OSID > < 1 string PID > ]
int MessageBuilder::NewStoreBindingCommandLineFromOSIDToPID(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(PP->GetSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,5,PP->GetOperatingSystemSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 5 > < 1 string OSID > < 1 string PID > ]
int MessageBuilder::NewStoreBindingCommandLineFromOSIDToPID(string _Version, string _PID, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(_PID);

	NewCommonCommandLine("-sr","--b",_Version,5,PP->GetOperatingSystemSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 5 > < 1 string PID > < 1 string OSID > ]
int MessageBuilder::NewStoreBindingCommandLineFromPIDToOSID(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(PP->GetOperatingSystemSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,5,PP->GetSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 0 > < 1 string "Host Legible Name" > < 1 string Hash("Host Legible Name") > ]
int MessageBuilder::NewStoreBindingCommandLineFromHLNToHashHLN(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashHLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetHostLegibleName(),HashHLN);

	Values.push_back(HashHLN);

	NewCommonCommandLine("-sr","--b",_Version,0,PP->GetHostLegibleName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 0 > < 1 string Hash("Host Legible Name") > < 1 string "Host Legible Name" > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashHLNToHLN(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashHLN;

	GenerateSCNFromCharArrayBinaryPatterns(PP->GetHostLegibleName(),HashHLN);

	Values.push_back(PP->GetHostLegibleName());

	NewCommonCommandLine("-sr","--b",_Version,1,HashHLN,&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 7 > < 1 string OSID > < 1 string HID > ]
int MessageBuilder::NewStoreBindingCommandLineFromOSIDToHID(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(PP->GetHostSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,7,PP->GetOperatingSystemSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 6 > < 1 string HID > < 1 string OSID > ]
int MessageBuilder::NewStoreBindingCommandLineFromHIDToOSID(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(PP->GetOperatingSystemSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,6,PP->GetHostSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string HID > < 1 string PID > ]
int MessageBuilder::NewStoreBindingCommandLineFromHIDToPID(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(PP->GetSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,6,PP->GetHostSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// Creates a  command line header to store a particular binding
// ng -sr --b _Version [ < 1 string 1 > < 1 string PID > < 1 string HID > ]
int MessageBuilder::NewStoreBindingCommandLineFromPIDToHID(string _Version, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(PP->GetHostSelfCertifyingName());

	NewCommonCommandLine("-sr","--b",_Version,7,PP->GetSelfCertifyingName(),&Values,_M, _PCL);

	return Status;
}

// **************************************************
// Limiters related
// **************************************************
// ng -sr --b _Version [ < 1 string 0 > < 1 string "Limiter" > < 1 string Hash("Limiter") > ]
int MessageBuilder::NewStoreBindingCommandLineFromLimiterToHashLimiter(string _Version, string _Limiter, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashLimiter;

	GenerateSCNFromCharArrayBinaryPatterns(_Limiter,HashLimiter);

	Values.push_back(HashLimiter);

	NewCommonCommandLine("-sr","--b",_Version,0,_Limiter,&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 1 > < 1 string Hash("Limiter") > < 1 string "Limiter" > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashLimiterToLimiter(string _Version, string _Limiter, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashLimiter;

	GenerateSCNFromCharArrayBinaryPatterns(_Limiter,HashLimiter);

	Values.push_back(_Limiter);

	NewCommonCommandLine("-sr","--b",_Version,1,HashLimiter,&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 2 > < 1 string Hash("Limiter") > < 1 string Representative SCN > ]
int MessageBuilder::NewStoreBindingCommandLineFromHashLimiterToRepresentativeSCN(string _Version, string _Limiter, string _RepresentativeSCN, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashLimiter;

	GenerateSCNFromCharArrayBinaryPatterns(_Limiter,HashLimiter);

	Values.push_back(_RepresentativeSCN);

	NewCommonCommandLine("-sr","--b",_Version,2,HashLimiter,&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 3 > < 1 string Representative SCN > < 1 string Hash("Limiter") > ]
int MessageBuilder::NewStoreBindingCommandLineFromRepresentativeSCNToHashLimiter(string _Version, string _RepresentativeSCN, string _Limiter, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;
	string			HashLimiter;

	GenerateSCNFromCharArrayBinaryPatterns(_Limiter,HashLimiter);

	Values.push_back(HashLimiter);

	NewCommonCommandLine("-sr","--b",_Version,3,_RepresentativeSCN,&Values,_M, _PCL);

	return Status;
}

// **************************************************
// Related to connectivity
// **************************************************

// ng -sr --b _Version [ < 1 string 15 > < 1 string HID > < 1 string Identifier > ]
int MessageBuilder::NewStoreBindingCommandLineFromHIDToIdentifier(string _Version, string _HID, string _Identifier, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(_Identifier);

	NewCommonCommandLine("-sr","--b",_Version,15,_HID,&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 16 > < 1 string Identifier > < 1 string HID > ]
int MessageBuilder::NewStoreBindingCommandLineFromIdentifierToHID(string _Version, string _Identifier, string _HID, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(_HID);

	NewCommonCommandLine("-sr","--b",_Version,16,_Identifier,&Values,_M, _PCL);

	return Status;
}

// ng -sr --b _Version [ < 1 string 17 > < 1 string Identifier > < 1 string SID > ]
int MessageBuilder::NewStoreBindingCommandLineFromIdentifierToSID(string _Version, string _Identifier, int _SID, Message *_M, CommandLine *& _PCL)
{
	int				Status=OK;
	vector<string> 	Values;

	Values.push_back(IntToString(_SID));

	NewCommonCommandLine("-sr","--b",_Version,17,_Identifier,&Values,_M, _PCL);

	return Status;
}

// ***************************************
// Continuing other command lines
// ***************************************

// Creates a get command line header
// ng -g --b _Version [ < 1 string _Category > < 1 string _Key > ]
int MessageBuilder::NewGetCommandLine(string _Version, unsigned int _Category, string _Key, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-g","--b",_Version,_PCL);

	// First argument
	_PCL->NewArgument(1);

	_PCL->SetArgumentElement(0,0,IntToString(_Category));

	// Second argument
	_PCL->NewArgument(1);

	_PCL->SetArgumentElement(1,0,_Key);

	return Status;
}

// Creates a run procedure message
// ng -run --proc _Version [ < 1 string _Procedure > ]
int MessageBuilder::NewRunProcedureCommandLine(string _Version, string _Procedure, Message *_M, CommandLine *& _PCL)
{
	int	Status=OK;

	_M->NewCommandLine("-run","--proc",_Version,_PCL);

	_PCL->NewArgument(1);

	_PCL->SetArgumentElement(0,0,_Procedure);

	return Status;
}

// The following informational header can be used to provide additional information regarding the message
// ng -info --payload _Version [ < n string _ValuesSize string S_1 ... S_ValuesSize > ]
int MessageBuilder::NewInfoPayloadCommandLine(string _Version, vector<string>* _Values, Message *_M, CommandLine *& _PCL)
{
	int	Status=ERROR;

	if (_Values != 0)
		{
			unsigned int _ValuesSize=_Values->size();

			if (_ValuesSize > 0)
				{
					_M->NewCommandLine("-info","--payload",_Version,_PCL);

					// Unique argument
					_PCL->NewArgument(_ValuesSize);

					// Set the value on the argument
					for (unsigned int k=0; k<_ValuesSize; k++)
						{
							_PCL->SetArgumentElement(0,k,_Values->at(k));
						}

					Status = OK;
				}
		}

	return Status;
}

// ------------------------------------------------------------------------------------------------------------------------------
// Functions to create customized messages
// ------------------------------------------------------------------------------------------------------------------------------

// Creates a new message appropriate to store a binding at some HT block
int MessageBuilder::NewConnectionLessStoreBindingMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
		unsigned int _Category, string _Key, vector<string>* _Values, Block *_PB, Message *&_M)
{
	int Status = ERROR;
	string		_SCN="empty";

	CommandLine 	*PConnectionLessCL=0;
	CommandLine		*PSCNCL=0;
	CommandLine 	*PStoreCL=0;

	PP->NewMessage(GetTime(),0,false,_M);

	if (NewConnectionLessCommandLine(_Version,_Limiters,_Sources,_Destinations,_M,PConnectionLessCL) == OK)
		{
			if (NewCommonCommandLine("-sr","--b",_Version,_Category,_Key,_Values,_M,PStoreCL) == OK)
				{
					// Generate the SCN
					_PB->GenerateSCNFromMessageBinaryPatterns(_M,_SCN);

					if (NewSCNCommandLine(_Version,_SCN,_M,PSCNCL) == OK)
						{
							Status = OK;
						}
				}
		}

	if (Status == ERROR)
		{
			_M->MarkToDelete();
		}

	return Status;
}

// Creates a get binding message
int MessageBuilder::NewConnectionLessGetBindingMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
		unsigned int _Category, string _Key, Block *_PB, Message *&_M)
{
	int Status = ERROR;
	string		_SCN="empty";

	CommandLine 	*PConnectionLessCL=0;
	CommandLine		*PSCNCL=0;
	CommandLine 	*PGetCL=0;

	PP->NewMessage(GetTime(),0,false,_M);

	if (NewConnectionLessCommandLine(_Version,_Limiters,_Sources,_Destinations,_M,PConnectionLessCL) == OK)
		{
			if (PConnectionLessCL != 0 && NewGetCommandLine(_Version,_Category,_Key,_M,PGetCL) == OK && PGetCL != 0)
				{
					// Generate the SCN
					_PB->GenerateSCNFromMessageBinaryPatterns(_M,_SCN);

					if (NewSCNCommandLine(_Version,_SCN,_M,PSCNCL) == OK)
						{
							Status = OK;
						}
				}
		}

	if (Status == ERROR)
		{
			_M->MarkToDelete();
		}

	return Status;
}

// Creates a delivery binding message
int MessageBuilder::NewConnectionLessDeliveryBindingMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
		unsigned int _Category, string _Key, vector<string>* _Values, Block *_PB, Message *&_M)
{
	int Status = ERROR;
	string		_SCN="empty";

	CommandLine 	*PConnectionLessCL=0;
	CommandLine		*PSCNCL=0;
	CommandLine 	*PDeliveryCL=0;

	PP->NewMessage(GetTime(),0,false,_M);

	if (NewConnectionLessCommandLine(_Version,_Limiters,_Sources,_Destinations,_M,PConnectionLessCL) == OK)
		{
			if (NewCommonCommandLine("-d","--b",_Version,_Category,_Key,_Values,_M,PDeliveryCL) == OK)
				{
					// Generate the SCN
					_PB->GenerateSCNFromMessageBinaryPatterns(_M,_SCN);

					if (NewSCNCommandLine(_Version,_SCN,_M,PSCNCL) == OK)
						{
							Status = OK;
						}
				}
		}

	if (Status == ERROR)
		{
			_M->MarkToDelete();
		}

	return Status;
}

// Creates a status message
int MessageBuilder::NewConnectionLessStatusMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
		string _AckCommandName, string _AckCommandAlt, unsigned int _StatusCode, Block *_PB, Message *&_M)
{
	int Status = ERROR;
	string		_SCN="empty";

	CommandLine 	*PConnectionLessCL=0;
	CommandLine		*PSCNCL=0;
	CommandLine 	*PStatusCL=0;

	PP->NewMessage(GetTime(),0,false,_M);

	if (NewConnectionLessCommandLine(_Version,_Limiters,_Sources,_Destinations,_M,PConnectionLessCL) == OK)
		{
			if (NewStatusCommandLine(_AckCommandName,_AckCommandAlt,_Version,_StatusCode,_M,PStatusCL) == OK)
				{
					// Generate the SCN
					_PB->GenerateSCNFromMessageBinaryPatterns(_M,_SCN);

					if (NewSCNCommandLine(_Version,_SCN,_M,PSCNCL) == OK)
						{
							Status = OK;
						}
				}
		}

	if (Status == ERROR)
		{
			_M->MarkToDelete();
		}

	return Status;
}

// Creates a run procedure message
int MessageBuilder::NewConnectionLessRunMessage(string _Version, vector<string>* _Limiters, vector<string>* _Sources, vector<string>* _Destinations,
		string _Procedure, Block *_PB, Message *_M)
{
	int Status = ERROR;
	string		_SCN="empty";

	CommandLine 	*PConnectionLessCL=0;
	CommandLine		*PSCNCL=0;
	CommandLine 	*PRunCL=0;

	PP->NewMessage(GetTime(),0,false,_M);

	if (NewConnectionLessCommandLine(_Version,_Limiters,_Sources,_Destinations,_M,PConnectionLessCL) == OK)
		{
			if (NewRunProcedureCommandLine(_Version,_Procedure,_M,PRunCL) == OK)
				{
					// Generate the SCN
					_PB->GenerateSCNFromMessageBinaryPatterns(_M,_SCN);

					if (NewSCNCommandLine(_Version,_SCN,_M,PSCNCL) == OK)
						{
							Status = OK;
						}
				}
		}

	if (Status == ERROR)
		{
			_M->MarkToDelete();
		}

	return Status;
}

// Auxiliary functions
int MessageBuilder::StringToInt(string _String)
{
	stringstream ss(_String);

	int Temp=0;

	ss >> Temp;

	return Temp;
}

string MessageBuilder::IntToString(int _Int)
{
	stringstream ss;

	ss << _Int;

	return ss.str();
}

string MessageBuilder::DoubleToString(double _Double)
{
	stringstream ss;

	ss << _Double;

	return ss.str();
}

string MessageBuilder::UnsignedIntToString(unsigned int _Int)
{
	string		 ret;
	stringstream ss;

	ss << _Int;

	ret=ss.str();

	return ret;
}

double MessageBuilder::GetTime()
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((t.tv_sec)+(double)(t.tv_nsec/1e9));
}

int MessageBuilder::GenerateSCNFromCharArrayBinaryPatterns4Bytes(const char *_Input, long long _Size, string &_SCN)
{
	int				 			Product=0;
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	unsigned char	 			*Output=new unsigned char[4];
	string						Strings;
	int							Status=ERROR;

	//cout<< "Input = "<<*_Input<<endl;

	if (_Input != 0)
		{
			// Initializing the input array
			if (_Size < 17)
				{
					Strings="";

					for (unsigned int b=0; b<16; b++)
						{
							for (unsigned int c=0; c<_Size; c++)
								{
									Product=((int)_Input[c])*primes[b];

									stringstream ss;

									ss << Product;

									Strings=Strings+ss.str();
								}
						}

				}

			unsigned long seed=3571;

			*Bytes=0;

			if (_Size < 17)
				{
					//cout<< "Strings = "<<Strings<<endl;

					key=(const void*)Strings.c_str();

					MurmurHash3_x86_32(key,Strings.size(),seed,Bytes);
				}
			else
				{
					key=(const void*)_Input;

					MurmurHash3_x86_32(key,_Size,seed,Bytes);
				}

			Temp[0] = (*Bytes >> (8*3)) & 0xff;
			Temp[1] = (*Bytes >> (8*2)) & 0xff;
			Temp[2] = (*Bytes >> (8*1)) & 0xff;
			Temp[3] = (*Bytes >> (8*0)) & 0xff;

			for (int x=0; x<4; x++)	{Chars[x]=' ';}

			Chars[0]=(char)(Temp[0]);
			Chars[1]=(char)(Temp[1]);
			Chars[2]=(char)(Temp[2]);
			Chars[3]=(char)(Temp[3]);

			// Copying the hash code to the _SCN string
			for (int y=0; y<4; y++)
				{
					//printf("%i %d %c \n",y,Chars[y],Chars[y]);

					Output[y]=Chars[y];
				}

			// Converting to an hexadecimal string
			ostringstream ret;

			for (unsigned int f = 0; f < 4; f++)
				{
					//cout << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

					ret << hex << std::setfill('0') << std::setw(2)<< uppercase << (int)Output[f];
				}

			_SCN= ret.str();

			//cout << endl << "SCN  = " << _SCN << endl;

			Status=OK;
		}
	else
		{
			cout << "          ERROR: null input array" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
	delete[] Output;

	return Status;
}

int MessageBuilder::GenerateSCNFromCharArrayBinaryPatterns16Bytes(const char *_Input, long long _Size, string &_SCN)
{
	int				 			Product=0;
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	unsigned int 				InputArraySize=0;
	unsigned char	 			*Output=new unsigned char[16];
	int							Index=0;
	unsigned char				*InputArray[4];
	int							*Primes[4];
	string						Strings[4];
	int							Status=ERROR;

	InputArraySize=(unsigned int)ceil((double)_Size/4);

	//cout << "Input = ";

	//for (unsigned int r=0; r<_Size; r++ )
	//	{
	//		cout<<_Input[r];
	//	}

	//cout << endl;

	//cout << "Char array size = " << (_Size) << endl;

	//cout << "InputArraySize = " << InputArraySize << endl;

	if (_Input != 0)
		{
			// Initializing the input array
			if (_Size < 17)
				{
					for (unsigned int i=0; i<4; i++)
						{
							Primes[i]=new int[16];
						}

					// Generate first 64 prime numbers
				    int max = 64;
				    int primes[max];
				    int n_primes = 0;

				    for (int z = 2; n_primes < max; z++)
				    	{
				        	for (int w = 0; w < n_primes; w++)
				        		{
				        			if (z % primes[w] == 0)
				        				{
				        					goto not_prime;
				        				}
				        		}

				        	primes[n_primes] = z;

				        	n_primes++;

				        	not_prime:
				            	;
				    	}

				    // Copy for Primes bidimensional array
					for (unsigned int c=0; c<4; c++)
						{
							for (unsigned int d=0; d<16; d++ )
								{
									Primes[c][d]=primes[c*16+d];

									//cout << "Primes ["<<c<<","<<d <<"] ="<<Primes[c][d]<< endl;
								}
						}

					// Initialize Input Array with input * primes product
					for (unsigned int a=0; a<4; a++)
						{
							Strings[a]="";

							for (unsigned int b=0; b<_Size; b++ )
								{
									Product=((int)_Input[b])*Primes[a][b];

									//cout << "Product = " << Product << endl;

									stringstream ss;

									ss << Product;

									Strings[a]=Strings[a]+ss.str();
								}

							//cout << "Strings = " << Strings[a] << endl;
						}
				}
			else
				{
					for (unsigned int i=0; i<4; i++)
						{
							InputArray[i]=new unsigned char[InputArraySize];
						}

					int u=0;

					for (unsigned int a=0; a<4; a++)
						{
							for (unsigned int b=0; b<InputArraySize; b++ )
								{
									u=b+a*InputArraySize;

									if (u < _Size)
										{
											InputArray[a][b]=_Input[u];
										}
									else
										{
											InputArray[a][b]=' ';
										}

									//printf("%i %i %d %c \n",a,b,InputArray[a][b],InputArray[a][b]);
								}
						}
				}

			// Generating 4 hash codes with 4 bytes each
			for (int j=0; j<4; j++)
				{
					unsigned long seed=3571;

					*Bytes=0;

					if (_Size < 17)
						{
							key=(const void*)Strings[j].c_str();

							MurmurHash3_x86_32(key,Strings[j].size(),seed,Bytes);
						}
					else
						{
							key=(const void*)InputArray[j];

							MurmurHash3_x86_32(key,InputArraySize,seed,Bytes);
						}

					Temp[0] = (*Bytes >> (8*3)) & 0xff;
					Temp[1] = (*Bytes >> (8*2)) & 0xff;
					Temp[2] = (*Bytes >> (8*1)) & 0xff;
					Temp[3] = (*Bytes >> (8*0)) & 0xff;

					for (int x=0; x<4; x++)	{Chars[x]=' ';}

					Chars[0]=(char)(Temp[0]);
					Chars[1]=(char)(Temp[1]);
					Chars[2]=(char)(Temp[2]);
					Chars[3]=(char)(Temp[3]);

					// Copying the hash code to the _SCN string
					for (int y=0; y<4; y++)
						{
							//printf("%i %d %c \n",y,Chars[y],Chars[y]);

							Output[(Index*4+y)]=Chars[y];
						}

					Index++;
				}

			// Converting to an hexadecimal string
			ostringstream ret;

			for (unsigned int f = 0; f < 16; f++)
				{
					//cout << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

					ret << hex << std::setfill('0') << std::setw(2)<< uppercase << (int)Output[f];
				}

			_SCN= ret.str();

			//cout << endl << "SCN  = " << _SCN << endl;

			Status=OK;
		}
	else
		{
			cout << "          ERROR: null input array" << endl;
		}

	delete Bytes;
	delete Temp;
	delete Chars;
	delete Output;

	if (_Size < 17)
		{
			for (unsigned int i=0; i<4; i++)
				{
					delete Primes[i];
				}
		}
	else
		{
			for (unsigned int i=0; i<4; i++)
				{
					delete InputArray[i];
				}
		}

	return Status;
}


int MessageBuilder::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string _Input, string &_SCN)
{
	int				 			Product=0;
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	unsigned char	 			*Output=new unsigned char[4];
	string						Strings;
	int							Status=ERROR;
	long long 					_Size=_Input.size();

	//cout<<"Input = "<<_Input<<endl;

	if (_Input != "")
		{
			// Initializing the input array
			if (_Size < 17)
				{
					Strings="";

					for (unsigned int b=0; b<16; b++)
						{
							for (unsigned int c=0; c<_Size; c++)
								{
									Product=((int)_Input[c])*primes[b];

									stringstream ss;

									ss << Product;

									Strings=Strings+ss.str();
								}
						}

				}

			unsigned long seed=3571;

			*Bytes=0;

			if (_Size < 17)
				{
					//cout<< "Strings = "<<Strings<<endl;

					key=(const void*)Strings.c_str();

					MurmurHash3_x86_32(key,Strings.size(),seed,Bytes);
				}
			else
				{
					key=(const void*)_Input.c_str();

					MurmurHash3_x86_32(key,_Size,seed,Bytes);
				}

			Temp[0] = (*Bytes >> (8*3)) & 0xff;
			Temp[1] = (*Bytes >> (8*2)) & 0xff;
			Temp[2] = (*Bytes >> (8*1)) & 0xff;
			Temp[3] = (*Bytes >> (8*0)) & 0xff;

			for (int x=0; x<4; x++)	{Chars[x]=' ';}

			Chars[0]=(char)(Temp[0]);
			Chars[1]=(char)(Temp[1]);
			Chars[2]=(char)(Temp[2]);
			Chars[3]=(char)(Temp[3]);

			// Copying the hash code to the _SCN string
			for (int y=0; y<4; y++)
				{
					//printf("%i %d %c \n",y,Chars[y],Chars[y]);

					Output[y]=Chars[y];
				}

			// Converting to an hexadecimal string
			ostringstream ret;

			for (unsigned int f = 0; f < 4; f++)
				{
					//cout << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

					ret << hex << std::setfill('0') << std::setw(2)<< uppercase << (int)Output[f];
				}

			_SCN= ret.str();

			//cout << endl << "SCN  = " << _SCN << endl;

			Status=OK;
		}
	else
		{
			cout << "          ERROR: null input array" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
	delete[] Output;

	return Status;
}

int MessageBuilder::GenerateSCNFromCharArrayBinaryPatterns16Bytes(string _Input, string &_SCN)
{
	int				 			Product=0;
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	unsigned int 				InputArraySize=0;
	unsigned char	 			*Output=new unsigned char[16];
	int							Index=0;
	unsigned char				*InputArray[4];
	int							*Primes[4];
	string						Strings[4];
	int							Status=ERROR;
	long long 					Size=_Input.size();

	InputArraySize=(unsigned int)ceil((double)Size/4);

	//cout << "Input = " << _Input << endl;

	//cout << "Char array size = " << (Size) << endl;

	//cout << "InputArraySize = " << InputArraySize << endl;

	if (_Input != "")
		{
			// Initializing the input array
			if (Size < 17)
				{
					for (unsigned int i=0; i<4; i++)
						{
							Primes[i]=new int[16];
						}

					// Generate first 64 prime numbers
				    int max = 64;
				    int primes[max];
				    int n_primes = 0;

				    for (int z = 2; n_primes < max; z++)
				    	{
				        	for (int w = 0; w < n_primes; w++)
				        		{
				        			if (z % primes[w] == 0)
				        				{
				        					goto not_prime;
				        				}
				        		}

				        	primes[n_primes] = z;

				        	n_primes++;

				        	not_prime:
				            	;
				    	}

				    // Copy for Primes bidimensional array
					for (unsigned int c=0; c<4; c++)
						{
							for (unsigned int d=0; d<16; d++ )
								{
									Primes[c][d]=primes[c*16+d];

									//cout << "Primes ["<<c<<","<<d <<"] ="<<Primes[c][d]<< endl;
								}
						}

					// Initialize Input Array with input * primes product
					for (unsigned int a=0; a<4; a++)
						{
							Strings[a]="";

							for (unsigned int b=0; b<Size; b++ )
								{
									Product=((int)_Input[b])*Primes[a][b];

									//cout << "Product = " << Product << endl;

									stringstream ss;

									ss << Product;

									Strings[a]=Strings[a]+ss.str();
								}

							//cout << "Strings = " << Strings[a] << endl;
						}
				}
			else
				{
					for (unsigned int i=0; i<4; i++)
						{
							InputArray[i]=new unsigned char[InputArraySize];
						}

					for (unsigned int a=0; a<4; a++)
						{
							for (unsigned int b=0; b<InputArraySize; b++ )
								{
									InputArray[a][b]=' ';
								}

							int u=0;

							for (unsigned int b=0; b<InputArraySize; b++ )
								{
									u=b+a*InputArraySize;

									if (u < Size)
										{
											InputArray[a][b]=_Input[u];
										}
									else
										{
											InputArray[a][b]=' ';
										}

									//printf("%i %i %d %c \n",a,b,InputArray[a][b],InputArray[a][b]);
								}
						}
				}

			// Generating 4 hash codes with 4 bytes each
			for (int j=0; j<4; j++)
				{
					unsigned long seed=3571;

					*Bytes=0;

					if (Size < 17)
						{
							key=(const void*)Strings[j].c_str();

							MurmurHash3_x86_32(key,Strings[j].size(),seed,Bytes);
						}
					else
						{
							key=(const void*)InputArray[j];

							MurmurHash3_x86_32(key,InputArraySize,seed,Bytes);
						}

					Temp[0] = (*Bytes >> (8*3)) & 0xff;
					Temp[1] = (*Bytes >> (8*2)) & 0xff;
					Temp[2] = (*Bytes >> (8*1)) & 0xff;
					Temp[3] = (*Bytes >> (8*0)) & 0xff;

					for (int x=0; x<4; x++)	{Chars[x]=' ';}

					Chars[0]=(char)(Temp[0]);
					Chars[1]=(char)(Temp[1]);
					Chars[2]=(char)(Temp[2]);
					Chars[3]=(char)(Temp[3]);

					// Copying the hash code to the _SCN string
					for (int y=0; y<4; y++)
						{
							//printf("%i %d %c \n",y,Chars[y],Chars[y]);

							Output[(Index*4+y)]=Chars[y];
						}

					Index++;
				}

			// Converting to an hexadecimal string
			ostringstream ret;

			for (unsigned int f = 0; f < 16; f++)
				{
					//cout << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

					ret << hex << std::setfill('0') << std::setw(2)<< uppercase << (int)Output[f];
				}

			_SCN= ret.str();

			//cout << endl << "SCN  = " << _SCN << endl;

			Status=OK;
		}
	else
		{
			cout << "          ERROR: null input array" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
	delete[] Output;

	if (Size < 17)
		{
			for (unsigned int i=0; i<4; i++)
				{
					delete[] Primes[i];
				}
		}
	else
		{
			for (unsigned int i=0; i<4; i++)
				{
					delete[] InputArray[i];
				}
		}

	return Status;
}

// Generate a self-certified name from a char array
int MessageBuilder::GenerateSCNFromCharArrayBinaryPatterns(const char *_Input, long long _Size, string &_SCN)
{
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(_Input,_Size,_SCN);
}

// Generate a self-certified name from a char array
int MessageBuilder::GenerateSCNFromCharArrayBinaryPatterns(string _Input, string &_SCN)
{
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(_Input,_SCN);
}

