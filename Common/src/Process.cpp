/*
	NovaGenesis

	Name:		Process
	Object:		Process
	File:		Process.cpp
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

#ifndef _PROCESS_H
#include "Process.h"
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CLI_H
#include "CLI.h"
#endif

#ifndef _MURMURHASH3_H_
#include "MurmurHash3.h"
#endif

////#define DEBUG
//#define DEBUG1
//#define DEBUG3

Process::Process(string _LN, key_t _Key, string _Path)
{
	LN=_LN;
	SCN="";
	Path="";
	Key=_Key;
	Path=_Path;
	MessageCounter=0;

	// Modified in 9th April 2021 to deal with parallel shared memories.

    for (int z=0; z<NUMBER_OF_PARALLEL_SHARED_MEMORIES; z++)
    {
        shmid[z]=-1;
    }

    NoM=0;

	InstantiationTime=GetTime();

	PMB=new MessageBuilder(this);

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

    string HASH_SCN="";

	// Generate an SCN for the process
	GenerateSCNFromProcessBinaryPatterns(this,SCN);

	// Generate the hash of the LN for the process
	GenerateSCNFromCharArrayBinaryPatterns(LN,HASH_SCN);

	cout << "(The process legible name is "<<LN<<")" << endl;

	cout << "(The hash of process legible name is "<<HASH_SCN<<")" << endl;

	cout << "(The process self-certifying name is "<<SCN<<")" << endl;

	cout << "(The process shared memory key is = " << Key << ")" << endl;

	// Auxiliary pointers
	Block			*PB2=0;
	Block			*PB3=0;

	// *****************************************
	// Generate	HID and OSID
	// *****************************************
	struct utsname uts;

	if (uname(&uts) != -1)
	    {
			stringstream osname;
			stringstream hostname;

			osname << uts.sysname <<" ";
			osname << uts.release <<" ";
			osname << uts.version <<" ";
			osname << uts.nodename;

			hostname << uts.nodename <<" ";
			hostname << uts.machine;

			OSLN=osname.str();

			HLN=hostname.str();

			cout << "(The operating system legible name is "<<OSLN<<")" << endl;

			cout << "(The host legible name is "<<HLN<<")" << endl;

			GenerateSCNFromCharArrayBinaryPatterns(OSLN,OSSCN);

			//OSSCN=OSSCN+"_OSID";

			cout << "(The operating system self-certifying name is = " << OSSCN << ")" << endl;

			GenerateSCNFromCharArrayBinaryPatterns(HLN,HSCN);

			//HSCN=HSCN+"_HID";

			cout << "(The host self-certifying name is = " << HSCN << ")" << endl;
	    }

	// **********************************************************************************
	// Generate	DID - Deprecated in March 2016 - Replaced by Domain Service.
	// **********************************************************************************

	//DLN="NovaGenesis Domain @ INATEL, Brazil";

	//GenerateSCNFromCharArrayBinaryPatterns(DLN,DSCN);

	//DSCN=DSCN+"_DID";

	//cout << "(The domain self-certifying name is = " << DSCN << ")" << endl<< endl << endl;

	// **********************************************************************************
	// Default domain name - March 2016
	// Temporary name until Domain Service publishes the final name
	// Requires update o domain name on hash tables after DS publishing
	// the official domain name
	// **********************************************************************************

	DLN="Unnamed";

	GenerateSCNFromCharArrayBinaryPatterns(DLN,DSCN);

    // ************************************************************
    // New limiters scheme. March 2016
    // ************************************************************

	// Generate a SCN from the "Process" word
	GenerateSCNFromCharArrayBinaryPatterns("Intra_Process",Intra_Process);

	cout << "(Intra_Process = " << Intra_Process << ")" << endl;

	// Generate a SCN from the "Operating System" word
	GenerateSCNFromCharArrayBinaryPatterns("Intra_OS",Intra_OS);

	cout << "(Intra_OS = " << Intra_OS << ")" << endl;

	// Generate a SCN from the "Domain" word
	GenerateSCNFromCharArrayBinaryPatterns("Intra_Node",Intra_Node);

	cout << "(Intra_Node = " << Intra_Node << ")" << endl;

	// Generate a SCN from the "Domain" word
	GenerateSCNFromCharArrayBinaryPatterns("Intra_Domain",Intra_Domain);

	cout << "(Intra_Domain = " << Intra_Domain << ")" << endl;

	// Generate a SCN from the "Domain" word
	GenerateSCNFromCharArrayBinaryPatterns("Inter_Domain",Inter_Domain);

	cout << "(Inter_Domain = " << Inter_Domain << ")" << endl;

	// **********************************************************************************
	// Initializes the messages container
	// **********************************************************************************

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			Messages[i]=NULL;

			Controls[i]=true; // Can I write to some positions
		}

	// Create the process blocks
	NewBlock("HT",PB3);
	NewBlock("GW",PB2);
	//NewBlock("CLI",PB1);
}

Process::~Process()
{
	cout << "Deleting " << LN << endl;

	cout << "Deleting Messages container" << endl;

	DeleteMessages();

	cout << "Done" << endl;


	vector<Block*>::iterator 		it;
	unsigned int					i=0;

	for (it=Blocks.begin(); it!=Blocks.end(); it++)
		{
			delete Blocks[i];

			i++;
		}

	Blocks.clear();

	delete PMB;
}

// Set block legible name
void Process::SetLegibleName(string _LN)
{
	LN=_LN;
}

// Set block self-certyfing name
void Process::SetSelfCertifyingName(string _SCN)
{
	SCN=_SCN;
}

// Set working path
void Process::SetPath(string _Path)
{
	Path=_Path;
	
	Block *PB=0;

	vector<Block*>::iterator 		it;

	for (it=Blocks.begin(); it!=Blocks.end(); it++)
		{
			PB=*it;

			PB->SetPath(_Path);
		}
}

// Get block legible name
string Process::GetLegibleName()
{
	return LN;
}

// Get process self-certifying name
string Process::GetSelfCertifyingName()
{
	return SCN;
}


// Get working path
string Process::GetPath()
{
	return Path;
}

// Get Blocks container size
unsigned int Process::GetBlocksSize()
{
	return Blocks.size();
}

// Get Host Legible Name
string Process::GetHostLegibleName()
{
	return HLN;
}

// Get Host Self-certifying Name
string Process::GetHostSelfCertifyingName()
{
	return HSCN;
}

// Get Operating System Legible Name
string Process::GetOperatingSystemLegibleName()
{
	return OSLN;
}

// Get Operating System Self-certifying Name
string Process::GetOperatingSystemSelfCertifyingName()
{
	return OSSCN;
}

// Get Domain Self-certifying Name
string Process::GetDomainSelfCertifyingName()
{
	return DSCN;
}

// Insert an allocated Block on Blocks container
 void Process::Process::InsertBlock(Block *_PB)
{
	 Blocks.push_back(_PB);
}

// Allocate a new block based on a name and add a Block on Blocks container
int Process::NewBlock(string _LN, Block *&_PB)
{
	if (_LN == "CLI" )
		{
			Block 	*PGWB=0;
			Block 	*PHTB=0;
			GW		*PGW=0;
			HT		*PHT=0;
			string 	GWLN="GW";
			string 	HTLN="HT";

			GetBlock(GWLN,PGWB);

			PGW=(GW*)PGWB;

			GetBlock(HTLN,PHTB);

			PHT=(HT*)PHTB;

			unsigned int Index=0;

			Index=GetBlocksSize();

			CLI *PCLI=new CLI(_LN,this,Index,PGW,PHT,Path);

			_PB=(Block*)PCLI;

			InsertBlock(_PB);

			return OK;
		}

	if (_LN == "GW" )
		{
			GW *PGW=new GW(_LN,this,1,Path);

			_PB=(Block*)PGW;

			Blocks.push_back(_PB);

			return OK;
		}

	if (_LN == "HT" )
		{
			HT *PDM=new HT(_LN,this,0,Path);

			_PB=(Block*)PDM;

			Blocks.push_back(_PB);

			return OK;
		}

	return ERROR;
}

// Get a Block by its legible name
int Process::GetBlock(string _LN, Block *&_PB)
{
	Block 						*PB=0;
	vector<Block*>::iterator 	it;
	int 						Status=ERROR;

	for (it=Blocks.begin(); it!=Blocks.end(); it++)
		{
			PB=*it;

			if (PB->GetLegibleName() == _LN)
				{
					_PB=PB;

					break;

					Status=OK;
				}
		}

	return Status;
}

// Get a Block by its index on Blocks container
int Process::GetBlock(unsigned int _Index, Block *&_PB)
{
	Block 						*PB=0;
	vector<Block*>::iterator 	it;
	int 						Status=ERROR;
	unsigned int				i=0;

	for (it=Blocks.begin(); it!=Blocks.end(); it++)
		{
			PB=*it;

			if (i == _Index)
				{
					_PB=PB;

					Status=OK;

					break;
				}

			i++;
		}

	return Status;
}

// Get a Block index by its legible name
int Process::GetBlockIndex(string _LN, unsigned int &_Index)
{
	int Status=ERROR;

	for (unsigned int i=0; i<Blocks.size(); i++)
		{
			if (Blocks[i]->GetLegibleName() == _LN)
				{
					_Index=i;

					Status=OK;

					break;
				}
		}

	return Status;
}

// Delete a Block by its legible name
int Process::DeleteBlock(string _LN)
{
	return 0;
}

// Delete a Block by its index on Blocks container
int Process::DeleteBlock(unsigned int _Index)
{
	Block 						*PB=0;
	vector<Block*>::iterator 	it;
	unsigned int 				i=0;
	int 						Status=ERROR;

	for (it=Blocks.begin(); it!=Blocks.end(); it++)
		{
			if (_Index == i)
				{
					PB=Blocks[i];

					Blocks.erase(it);

					delete PB;

					Status=OK;

					break;
				}

			i++;
		}

	return Status;
}

// Delete a Block by its pointer 
int Process::DeleteBlock(Block *B)
{
	Block 						*PB=0;
	vector<Block*>::iterator 	it;
	int 						i=0;
	int 						Status=ERROR;

	for (it=Blocks.begin(); it!=Blocks.end(); it++)
		{
			PB=Blocks[i];

			if (B == PB)
				{
					Blocks.erase(it);

					delete PB;

					Status=OK;

					break;
				}

			i++;
		}

	return Status;	
}

// Allocate and add a Message on Messages container
int Process::NewMessage(double _Time, short _Type, bool _HasPayload, Message *&M)
{
	int Status=ERROR;

	for(unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == FREE)
				{
					Message *PM=new Message(_Time,_Type,_HasPayload);

					M=PM;

					PM->InstantiationNumber=MessageCounter;

					MessageCounter++;

					NoM++;

					Messages[i]=PM;

					Controls[i]=BUSY;

					Status=OK;


#ifdef DEBUG
                    cout <<"          (Creating a message at "<<GetLegibleName()<<" with instantiation number " <<PM->InstantiationNumber<< ". Number of messages is "<<NoM<<")" << endl;

					cout << "(Allocated the message with index = "<<i<<". Number of messages is "<<NoM<<".)"<<endl;
#endif

					break;
				}
		}

	return Status;
}

// Allocate and add a simple Message on Messages container
int Process::NewMessage(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _Path, Message *&M)
{
	int Status=ERROR;

	M=NULL;

	for(unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == FREE)
				{
					Message *PM=new Message(_Time,_Type,_HasPayload,_HeaderFileName,_Path);

					M=PM;

					PM->InstantiationNumber=MessageCounter;

					MessageCounter++;

					NoM++;

					Messages[i]=PM;

					Controls[i]=BUSY;

#ifdef DEBUG

					cout << "(Allocated the message with index = "<<i<<". Number of messages is "<<NoM<<".)"<<endl;

#endif

					Status=OK;

					break;
				}
		}

	return Status;
}

// Allocate and add a complete Message on Messages container
int Process::NewMessage(double _Time, short _Type, bool _HasPayload, string _HeaderFileName, string _PayloadFileName, string _MessageFileName, string _Path, Message *&M)
{
	int Status=ERROR;

	M=NULL;

	for(unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == FREE)
				{
					Message *PM=new Message(_Time,_Type, _HasPayload, _HeaderFileName,  _PayloadFileName,  _MessageFileName,  _Path);

					M=PM;

					PM->InstantiationNumber=MessageCounter;

					MessageCounter++;

					NoM++;

					Messages[i]=PM;

					Controls[i]=BUSY;

#ifdef DEBUG

					cout << "(Allocated the message with index = "<<i<<". Number of messages is "<<NoM<<".)"<<endl;

#endif

					Status=OK;

					break;
				}
		}

	return Status;
}

int Process::NewMessage(Message *_Original, Message *&M)
{
	int Status=ERROR; // Pessimistic approach ;-)

	M=NULL;

	for(unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == FREE)
				{
					Message *PM=new Message(*_Original);

					M=PM;

					PM->InstantiationNumber=MessageCounter;

					MessageCounter++;

					NoM++;

					Messages[i]=PM;

					Controls[i]=BUSY;

#ifdef DEBUG

					cout << "(Allocated the message with index = "<<i<<". Number of messages is "<<NoM<<".)"<<endl;

#endif

					Status=OK;

					break;
				}
		}

	//S <<"          (Creating a message at block "<<GetLegibleName()<<" with instantiation number " <<PM->InstantiationNumber<< ". Number of messages is "<<NoM<<")" << endl;

	return Status;
}

// Get a Message
int Process::GetMessage(unsigned int _Index, Message *&M)
{

	if (_Index < NoM)
		{
			M=Messages[_Index];
		}

	return OK;
}

// Erase a Message from the container
int Process::EraseMessage(Message *M)
{
	int Status=OK;

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Messages[i] == M)
				{
					Messages[i]=NULL;

					Controls[i]=FREE;

					NoM--;

#ifdef DEBUG

					cout << "(Erased the message index = "<<i<<". The number of messages is "<<NoM<<">)"<<endl;

#endif

					break;
				}
		}

	return Status;
}

// Tests if a message is ok
int Process::HasMessage(Message *M, bool &_Answer)
{
	int Status=OK;

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Messages[i] == M)
				{
					_Answer=true;

					break;
				}
		}

	return Status;
}

// Delete a Message by its pointer
int Process::DeleteMessage(Message *M)
{
	int Status=OK;

	bool Deleted=false;

	int Index=0;

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Messages[i] == M)
				{
					if (M->GetDeleteFlag() == true)
						{
							delete M;

							Messages[i]=NULL;

							Controls[i]=FREE;

							NoM--;

							Deleted=true;

							Index = i;

							break;
						}
				}
		}

#ifdef DEBUG

	if (Deleted == true)
		{
			cout << "(Deleted the message with index = "<<Index<<". Number of messages is "<<NoM<<".)"<<endl;
		}

#endif

	return Status;
}

// Get number of Message object on Messages container
unsigned int Process::GetNumberOfMessages()
{
	return NoM;
}

// Get message at Message position _Index
Message* Process::GetMessage(unsigned int _Index)
{
	Message* Temp=0;

	if (_Index < MAX_MESSAGES_IN_MEMORY)
		{
			Temp=Messages[_Index];
		}

	return Temp;
}

void Process::ShowMessages()
{
	unsigned int	NoCL;

	cout << endl << "(------------------------- Showing the "<<NoM<<" messages in memory -------------------------)"<< endl;

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == BUSY) // Only the occupied positions
				{
					Messages[i]->GetNumberofCommandLines(NoCL);

					cout << "(Message with index "<<i<<", "<<NoCL<<" command lines, delete flag (true (1) - false (0)) = "<<Messages[i]->GetDeleteFlag()<<", application deleted flag = "<<Messages[i]->GetApplicationDeletedFlag()<<".)"<< endl;

					cout << "(" << endl << *Messages[i] << ")"<< endl << endl;
				}
			else
				{

					cout << "(Empty index "<<i<<")"<< endl;

				}
		}
}

void Process::DeleteMessages()
{
#ifdef DEBUG
    cout << endl << "(------------------------- Deleting messages in memory -------------------------)"<< endl;
#endif

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == BUSY) // Only the occupied positions
				{
					Message *Temp=Messages[i];

					if (Temp->GetDeleteFlag() == true)
						{

#ifdef DEBUG
							cout << "(The following message with index "<<i<<" will be deleted.)"<< endl;

							cout << "(" << endl << *Messages[i] << ")"<< endl << endl;
#endif

							delete Temp;

							Messages[i]=NULL;

							Controls[i]=FREE;

							NoM--;
						}
				}
		}


}

// Delete the messages marked to be deleted
void Process::DeleteMarkedMessages()
{
#ifdef DEBUG
    cout << endl << "(------------------------- Deleting marked messages in memory -------------------------)"<< endl;
#endif

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == BUSY)
				{
					Message *Temp=Messages[i];

					if (Temp->GetDeleteFlag() == true)
						{

#ifdef DEBUG
							cout << "(The following message with index "<<i<<" will be deleted.)"<< endl;

							cout << "(" << endl << *Messages[i] << ")"<< endl << endl;

#endif

							delete Temp;

							Messages[i]=NULL;

							Controls[i]=FREE; // Frees the position

							NoM--;
						}
#ifdef DEBUG
					else
						{

							cout <<i<<" busy, but not marked to be deleted."<<endl;

							cout << "(" << endl << *Messages[i] << ")"<< endl << endl;


						}
#endif
				}
#ifdef DEBUG
			else
				{
					cout <<i<<" is free to new messages."<<endl;
				}
#endif
		}

}

void Process::MarkUnmarkedMessagesPerTime(double _Threshold)
{

	//S << "( Marking messages according to time Threshold)"<< endl;

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == BUSY)
				{
					Message *Temp=Messages[i];

					if (Temp->GetDeleteFlag() == false && Temp->GetApplicationDeletedFlag() == DELETED_BY_CORE && Temp->GetTime() < _Threshold)
						{
							Temp->MarkToDelete();
						}
				}
		}

}

void Process::MarkMalformedMessagesPerNoCLs(unsigned int _NoCL)
{
	//S << "( Marking messages according to number of command lines)"<< endl;

	unsigned int NoCL=0;

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Controls[i] == BUSY)
				{
					Message *Temp=Messages[i];

					if (Temp->GetDeleteFlag() == false && Temp->GetApplicationDeletedFlag() == DELETED_BY_CORE)
						{
							Temp->GetNumberofCommandLines(NoCL);

							if (NoCL <= _NoCL)
								{
									Temp->MarkToDelete();
								}
						}
				}
		}
}

// Test if a message is OK to run it
bool Process::OkToRun(Message *_M)
{
	bool Status=false;

	for (unsigned int i=0; i<MAX_MESSAGES_IN_MEMORY; i++)
		{
			if (Messages[i] == _M)
				{
					if (Controls[i] == BUSY)
						{
							Status=true;

							break;
						}
				}
		}

	return Status;
}

// Run the Gateway
void Process::RunGateway()
{
	GW		*PGW=0;
	Block	*PGWB=0;

	// Get the GW pointer
	GetBlock("GW",PGWB);

	// Cast to GW pointer
	PGW=(GW*)PGWB;

	// Run the gateway
	// Block main thread execution on this function
	PGW->Gateway();
}

// Initiate a Command Line Prompt for users
void Process::RunPrompt()
{
	Block *PCLIB=0;

	GetBlock("CLI",PCLIB);

	// Cast to CLI pointer
	CLI *PCLI=(CLI*)PCLIB;

	// Create a CLI prompt thread
	thread t1(&CLI::PromptThreadWrapper,PCLI);

	// Wait for the threads to finish
	t1.join();
}

// Generate process self-certified name from its binary patterns
void Process::GenerateSCNFromProcessBinaryPatterns4Bytes(Process *_PP, string &_SCN)
{
	int				 			Product=0;
	string						Strings;
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	string 						Input;

	if (_PP != 0)
		{
			std::stringstream Data;

			Data << _PP;
			Data << &_PP->Blocks;
			Data << &_PP->LN;
			Data << &_PP->SCN;
			Data << &_PP->Path;
			Data << LN;
			Data << Path;

			// Setting the input integers for each round
			Input=Data.str();

			//cout<<endl<<"Input = "<<Input<<endl;

			Strings="";

			for (unsigned int b=0; b<16; b++)
				{
					for (unsigned int c=0; c<Input.size(); c++)
						{
							Product=((int)Input[c])*primes[b];

							stringstream ss;

							ss << Product;

							Strings=Strings+ss.str();
						}
				}

			//cout<< "Strings = "<<Strings<<endl;

			// Set the seed
			unsigned long seed=3571;

			*Bytes=0;

			key=(const void*)Strings.c_str();

			MurmurHash3_x86_32(key,Strings.size(),seed,Bytes);

			Temp[0] = (*Bytes >> (8*3)) & 0xff;
			Temp[1] = (*Bytes >> (8*2)) & 0xff;
			Temp[2] = (*Bytes >> (8*1)) & 0xff;
			Temp[3] = (*Bytes >> (8*0)) & 0xff;

			for (int x=0; x<4; x++)	{Chars[x]='a';}

			Chars[0]=(unsigned char)(Temp[0]);
			Chars[1]=(unsigned char)(Temp[1]);
			Chars[2]=(unsigned char)(Temp[2]);
			Chars[3]=(unsigned char)(Temp[3]);

			// Converting to an hexadecimal string
			ostringstream ret;

			for (unsigned int f = 0; f < 4; f++)
				{
					// cout << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

			        ret << hex << std::setfill('0') << std::setw(2) << uppercase << (int)Chars[f];
				}

			_SCN= ret.str();

			//_SCN=_SCN+"_"+LN+"_BID";

			//cout<< "SCN = "<<_SCN<<endl;
		}
	else
		{
			cout << "(ERROR: Invalid block pointer)" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
}

// Generate process self-certified name from its binary patterns
void Process::GenerateSCNFromProcessBinaryPatterns16Bytes(Process *_PP, string &_SCN)
{
	vector<string> 				Strings(4);
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	char 						*Chars=new char[4];
	int 						*Input=new int[4];
	unsigned char	 			*Output=new unsigned char[16];
	int							Index=0;

	if (_PP != 0)
		{
			// Setting the input integers for each round
			Input[0]=(((long long)_PP))*(110503);
			Input[1]=(((long long)&_PP->Blocks))*(132049);
			Input[2]=(((long long)&_PP->DLN))*(216091);
			Input[3]=(((long long)&_PP->LN))*(756839);

			_SCN="";

			// Generating 4 hash codes with 4 bytes each
			for (int i=0; i<4; i++)
				{
					unsigned long seed=3571;

					*Bytes=0;

					stringstream ss;

					ss << Input[i];

					Strings[i]=ss.str();

					//cout << "Strings["<<i<<"] = " << Strings[i] << endl;

					//cout << Strings[i] << endl;

					key=(const void*)Strings[i].c_str();

					MurmurHash3_x86_32(key,4,seed,Bytes);

					Temp[0] = (*Bytes >> (8*3)) & 0xff;
					Temp[1] = (*Bytes >> (8*2)) & 0xff;
					Temp[2] = (*Bytes >> (8*1)) & 0xff;
					Temp[3] = (*Bytes >> (8*0)) & 0xff;

					for (int x=0; x<4; x++)	{Chars[x]='a';}

					Chars[0]=(char)(Temp[0]);
					Chars[1]=(char)(Temp[1]);
					Chars[2]=(char)(Temp[2]);
					Chars[3]=(char)(Temp[3]);

					// Copying the hash code to the _SCN string
					for (int y=0; y<4; y++)
						{
							//	printf("%i %d %c \n",y,Chars[y],Chars[y]);

							Output[(Index*4+y)]=Chars[y];
						}

					Index++;
				}

			// Converting to an hexadecimal string
			ostringstream ret;

			for (unsigned int f = 0; f < 16; f++)
				{
					// S << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

			        ret << hex << std::setfill('0') << std::setw(2) << uppercase << (int)Output[f];
				}

			_SCN= ret.str();

			//_SCN=_SCN+"_"+LN+"_PID";
		}
	else
		{
			cout << "(ERROR: Invalid process pointer)" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
	delete[] Input;
	delete[] Output;
}


// Generate process self-certified name from its binary patterns
void Process::GenerateSCNFromProcessBinaryPatterns32Bytes(Process *_PP, string &_SCN)
{
	vector<string> 				Strings(8);
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	char 						*Chars=new char[4];
	long long 					*Input=new long long [8];
	unsigned char	 			*Output=new unsigned char[32];
	int							Index=0;

	if (_PP != 0)
		{
			// Setting the input integers for each round
			Input[0]=(((long long)_PP))*(110503);
			Input[1]=(((long long)&_PP->Blocks))*(132049);
			Input[2]=(((long long)&_PP->DLN))*(216091);
			Input[3]=(((long long)&_PP->LN))*(756839);
			Input[4]=(((long long)&_PP->SCN))*(859433);
			Input[5]=(((long long)&_PP->Path))*(1257787);
			Input[6]=(((long long)_PP))*(1398269);
			Input[7]=(((long long)_PP))*(2976221);

			_SCN="";

			// Generating 8 hash codes with 4 bytes each
			for (int i=0; i<8; i++)
				{
					unsigned long seed=3571;

					*Bytes=0;

					stringstream ss;

					ss << Input[i];

					Strings[i]=ss.str();

					//cout << "Strings["<<i<<"] = " << Strings[i] << endl;

					//cout << Strings[i] << endl;

					key=(const void*)Strings[i].c_str();

					MurmurHash3_x86_32(key,4,seed,Bytes);

					Temp[0] = (*Bytes >> (8*3)) & 0xff;
					Temp[1] = (*Bytes >> (8*2)) & 0xff;
					Temp[2] = (*Bytes >> (8*1)) & 0xff;
					Temp[3] = (*Bytes >> (8*0)) & 0xff;

					for (int x=0; x<4; x++)	{Chars[x]='a';}

					Chars[0]=(char)(Temp[0]);
					Chars[1]=(char)(Temp[1]);
					Chars[2]=(char)(Temp[2]);
					Chars[3]=(char)(Temp[3]);

					// Copying the hash code to the _SCN string
					for (int y=0; y<4; y++)
						{
							//	printf("%i %d %c \n",y,Chars[y],Chars[y]);

							Output[(Index*4+y)]=Chars[y];
						}

					Index++;
				}

			// Converting to an hexadecimal string
			ostringstream ret;

			for (unsigned int f = 0; f < 32; f++)
				{
					// S << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

			        ret << hex << std::setfill('0') << std::setw(2) << uppercase << (int)Output[f];
				}

			_SCN= ret.str();
		}
	else
		{
			cout << "(ERROR: Invalid process pointer)" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
	delete[] Input;
	delete[] Output;
}


int Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(char *_Input, long long _Size, string &_SCN)
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

int Process::GenerateSCNFromCharArrayBinaryPatterns16Bytes(char *_Input, long long _Size, string &_SCN)
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
	int 						Status=ERROR;

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
	delete[] Temp;
	delete[] Chars;
	delete[] Output;

	if (_Size < 17)
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

int Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string _Input, string &_SCN)
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
    unsigned int 				max = 16;

	//cout<<"Input = "<<_Input<<endl;

	if (_Input != "")
		{
			// Initializing the input array
			if (_Size < 17)
				{
					Strings="";

					for (unsigned int b=0; b<max; b++)
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

int Process::GenerateSCNFromCharArrayBinaryPatterns16Bytes(string _Input, string &_SCN)
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

					int u=0;

					for (unsigned int a=0; a<4; a++)
						{
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

// Generate block self-certified name from its binary patterns
void Process::GenerateSCNFromProcessBinaryPatterns(Process *_PP, string &_SCN)
{
	GenerateSCNFromProcessBinaryPatterns4Bytes(_PP,_SCN);
}

// Generate a self-certified name from a char array
int Process::GenerateSCNFromCharArrayBinaryPatterns(char *_Input, long long _Size, string &_SCN)
{
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(_Input,_Size,_SCN);
}

// Generate a self-certified name from a char array
int Process::GenerateSCNFromCharArrayBinaryPatterns(string _Input, string &_SCN)
{
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(_Input,_SCN);
}

// Set a value behind a key
int Process::StoreHTBindingValues(unsigned int _Category, string _Key, vector<string> *_Values, Block *_PB)
{
	int 			Status=ERROR;
	Block			*PHTB=0;
	HT				*PHT=0;
	string			Offset = "                                        ";

	GetBlock(0,PHTB);

	if (PHTB != 0)
		{
			PHT=(HT*)PHTB;

			if (PHT->StoreBinding(_Category,_Key,_Values) == OK)
				{
					Status=OK;
				}
		}

	return Status;
}

// Discover the values behind a key
int Process::GetHTBindingValues(unsigned int _Category, string _Key, vector<string> *&_Values)
{
	int 			Status=ERROR;
	Block			*PHTB=0;
	HT				*PHT=0;
	string			Offset = "                                        ";

	GetBlock(0,PHTB);

	if (PHTB != 0)
		{
			PHT=(HT*)PHTB;

			if (PHT->GetBinding(_Category,_Key,_Values) == OK)
				{
					Status=OK;
				}
			else
				{
					//cout << Offset <<  "(Warning: The key "<<_Key<<" does not exist on the HT Block at category "<<_Category<<")" << endl;
				}
		}

	return Status;
}

void Process::ListBindings()
{
	Message 		*PListBindingsMessage=0;
	CommandLine 	*PListBindingsCL;
	Block			*PHTB=0;
	Message			*InlineResponseMessage;

	GetBlock("HT",PHTB);

	if (PHTB != 0)
		{
			// Creating a -run --initialization message
			NewMessage(GetTime(),0,false,PListBindingsMessage);

			// Adding only the list bindings command line
			PListBindingsMessage->NewCommandLine("-list","--b","0.1",PListBindingsCL);

			// Execute the procedure
			PHTB->Run(PListBindingsMessage,InlineResponseMessage);

			// Delete the temporary message
			PListBindingsMessage->MarkToDelete();
		}
}

// Discover the values behind a legible name on category _Cat
int Process::DiscoverHomonymsEntitiesIDsFromLN(unsigned int _Cat, string _LN, vector<string> *&_Values, Block *_PB)
{
	int 			Status=ERROR;
	Block			*PHTB=0;
	HT				*PHT=0;
	string			Offset = "                                        ";
	string			HashLN;

	GetBlock("HT",PHTB);

	if (PHTB != 0)
		{
			PHT=(HT*)PHTB;

			GenerateSCNFromCharArrayBinaryPatterns(_LN,HashLN);

			if (PHT->GetBinding(_Cat,HashLN,_Values) == OK)
				{
					Status=OK;
				}
			else
				{
					//cout << Offset <<  "(Warning: The key "<<HashLN<<" does not exist on the HT Block at category "<<_Cat<<")" << endl;
				}
		}

	return Status;
}

// Discover the values behind a legible name on category _Cat
int Process::DiscoverHomonymsEntitiesIDsFromLN(unsigned int _Cat, string _LN, Block *_PB)
{
  int 			Status=ERROR;
  Block			*PHTB=0;
  HT				*PHT=0;
  string			Offset = "                                        ";
  string			HashLN;
  vector<string> *Values=new vector<string>;

  GetBlock("HT",PHTB);

  if (PHTB != 0)
	{
	  PHT=(HT*)PHTB;

	  GenerateSCNFromCharArrayBinaryPatterns(_LN,HashLN);

	  if (PHT->GetBinding(_Cat,HashLN,Values) == OK)
		{
		  Status=OK;
		}
	  else
		{
		  //cout << Offset <<  "(Warning: The key "<<HashLN<<" does not exist on the HT Block at category "<<_Cat<<")" << endl;
		}
	}

	delete Values;

  return Status;
}

// Discover inside a Process the Blocks that have the same legible names.
int Process::DiscoverHomonymsBlocksBIDsFromPID(string _PID, string _BlockLN, vector<string> *& _BIDs, Block *_PB)
{
	int 			Status=ERROR;
	string			HashProcessLegibleName;
	string			HashBlockLegibleName;
	unsigned int	Category=0;
	string 			Version="0.1";
	string 			Key="";
	vector<string>	*BottomUpBIDs=new vector<string>;
	vector<string>	*TopDownBIDs=new vector<string>;
	string			Offset = "                                        ";

	// Setting up the category
	Category=5;

	// Setting up the binding key
	Key=_PID;

	//cout << Offset <<  "(DiscoverHomonymsBlocksBIDsFromPID: Doing the first consultation for the PID = "<<_PID<<" and Block LN = "<<_BlockLN<<")" << endl;

	//cout << Offset <<  "(PID = " <<_PID<< ")" << endl;

	// Getting the bind values on the HT block
	if(GetHTBindingValues(Category,Key,BottomUpBIDs) == OK)
		{
			if (BottomUpBIDs != 0)
				{
					if (BottomUpBIDs->size() > 0)
						{
							// Generate a SCN from the received block name
							GenerateSCNFromCharArrayBinaryPatterns(_BlockLN,HashBlockLegibleName);

							// Setting up the category
							Category=2;

							// Setting up the binding key
							Key=HashBlockLegibleName;

							//cout << Offset <<  "(Doing the second consultation)" << endl;

							// Getting the bind values on the HT block
							if(GetHTBindingValues(Category,Key,TopDownBIDs) == OK)
								{
									if (TopDownBIDs != 0)
										{
											if (TopDownBIDs->size() > 0)
												{
													//cout << Offset <<  "(Preparing the results vector)" << endl;

													//cout << Offset <<  "(BottomUpBIDs size is "<<BottomUpBIDs->size()<<")" << endl;

													//cout << Offset <<  "(TopDownBIDs size is "<<TopDownBIDs->size()<<")" << endl;

													for (unsigned int i=0; i<BottomUpBIDs->size(); i++)
														{
															//_PB->S << Offset <<  "(BottomUpBIDs["<<i<<"] = " <<BottomUpBIDs->at(i)<< ")" << endl;

															for (unsigned int j=0; j<TopDownBIDs->size(); j++)
																{
																	//_PB->S << Offset <<  "( TopDownBIDs["<<j<<"] = " <<TopDownBIDs->at(j)<< ")" << endl;

																	if (BottomUpBIDs->at(i) == TopDownBIDs->at(j))
																		{
																			_BIDs->push_back(TopDownBIDs->at(j));

																			//_PB->S << Offset <<  "(Selected BID = " <<TopDownBIDs->at(j)<< ")" << endl;

																			Status=OK;
																		}
																}
														}
												}
											else
												{
													cout << Offset <<  "(Warning: Top down BIDs vector is empty)" << endl;
												}
										}
									else
										{
											cout << Offset <<  "(Warning: Top down BIDs vector is null)" << endl;
										}
								}
							else
								{
									//cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
								}
						}
					else
						{
							//cout << Offset <<  "(Warning: Bottom up BIDs vector is empty)" << endl;
						}
				}
			else
				{
					cout << Offset <<  "(Warning: Bottom up BIDs vector is null)" << endl;
				}
		}
	else
		{
			//cout << Offset <<  "(Warning: First consultation returned an warning status)" << endl;
		}

	delete BottomUpBIDs;
	delete TopDownBIDs;

	return Status;
}

// Discover inside an OS the Processes that have the same legible names.
int Process::DiscoverHomonymsProcessesPIDsFromOSID(string _OSID, string _ProcessLN, vector<string> *& _PIDs, Block *_PB)
{
	int 			Status=ERROR;
	string			HashProcessLegibleName;
	unsigned int	Category=0;
	string 			Version="0.1";
	string 			Key="";
	vector<string>	*BottomUpPIDs=new vector<string>;
	vector<string>	*TopDownPIDs=new vector<string>;
	string			Offset = "                                        ";

	// Setting up the category
	Category=5;

	// Setting up the binding key
	Key=_OSID;

	//cout << Offset <<  "(DiscoverHomonymsProcessesPIDsFromOSID = "<<_OSID<<" and Process LN = "<<_ProcessLN<<")" << endl;

	// Getting the bind values on the HT block
	if(GetHTBindingValues(Category,Key,BottomUpPIDs) == OK)
		{
			if (BottomUpPIDs != 0)
				{
					if (BottomUpPIDs->size() > 0)
						{
							// Generate a SCN from the received block name
							GenerateSCNFromCharArrayBinaryPatterns(_ProcessLN,HashProcessLegibleName);

							// Setting up the category
							Category=2;

							// Setting up the binding key
							Key=HashProcessLegibleName;

							//cout << Offset <<  "(Doing the second consultation)" << endl;

							// Getting the bind values on the HT block
							if(GetHTBindingValues(Category,Key,TopDownPIDs) == OK)
								{
									if (TopDownPIDs != 0)
										{
											if (TopDownPIDs->size() > 0)
												{
													//cout << Offset <<  "(Preparing the results vector)" << endl;

													//cout << Offset <<  "(BottomUpPIDs size is "<<BottomUpPIDs->size()<<")" << endl;

													//cout << Offset <<  "(TopDownPIDs size is "<<TopDownPIDs->size()<<")" << endl;

													for (unsigned int i=0; i<BottomUpPIDs->size(); i++)
														{
															//_PB->S << Offset <<  "(BottomUpPIDs["<<i<<"] = " <<BottomUpPIDs->at(i)<< ")" << endl;

															for (unsigned int j=0; j<TopDownPIDs->size(); j++)
																{
																	//_PB->S << Offset <<  "( TopDownPIDs["<<j<<"] = " <<TopDownPIDs->at(j)<< ")" << endl;

																	if (BottomUpPIDs->at(i) == TopDownPIDs->at(j))
																		{
																			_PIDs->push_back(TopDownPIDs->at(j));

																			//_PB->S << Offset <<  "(Selected PID = " <<TopDownPIDs->at(j)<< ")" << endl;

																			Status=OK;
																		}
																}
														}
												}
											else
												{
													cout << Offset <<  "(Warning: Top down PIDs vector is empty)" << endl;
												}
										}
									else
										{
											cout << Offset <<  "(Warning: Top down PIDs vector is null)" << endl;
										}
								}
							else
								{
									//cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
								}
						}
					else
						{
							cout << Offset <<  "(Warning: Bottom up PIDs vector is empty)" << endl;
						}
				}
			else
				{
					//cout << Offset <<  "(Warning: Bottom up PIDs vector is null)" << endl;
				}
		}
	else
		{
			//cout << Offset <<  "(Warning: First consultation returned an warning status)" << endl;
		}

	delete BottomUpPIDs;
	delete TopDownPIDs;

	return Status;
}


// Discover inside a Process the Blocks that have the same legible names.
int Process::DiscoverHomonymsBlocksBIDsFromProcessLegibleName(string _ProcessLN, string _BlockLN, string &_PID, vector<string> *& _BIDs, Block *_PB)
{
	int 			Status=ERROR;
	string			HashProcessLegibleName;
	string			HashBlockLegibleName;
	unsigned int	Category=3;
	string 			Version="0.1";
	string 			Key="";
	vector<string>	*PIDs=new vector<string>;
	vector<string>	*BottomUpBIDs=new vector<string>;
	vector<string>	*TopDownBIDs=new vector<string>;
	string			Offset = "                                        ";

	// ***************************************************
	// Discovering the BID of the related PID
	// ***************************************************

	// Generate a SCN from the received process name
	GenerateSCNFromCharArrayBinaryPatterns(_ProcessLN,HashProcessLegibleName);

	// Setting up the category
	Category=2;

	// Setting up the binding key
	Key=HashProcessLegibleName;

#ifdef DEBUG1

	cout<< Offset << "HashProcessLegibleName = "<< HashProcessLegibleName << endl;

	cout << Offset <<  "(DiscoverHomonymsBlocksBIDsFromProcessLegibleName = "<<_ProcessLN<<" and Block LN = "<<_BlockLN<<")" << endl;

#endif

	// Getting the process PID
	if(GetHTBindingValues(Category,Key,PIDs) == OK)
		{
			if (PIDs != 0)
				{
					if (PIDs->size() > 0)
						{
							// Setting up the category
							Category=5;

							// Setting up the binding key
							Key=PIDs->at(0);

							_PID=Key;

#ifdef DEBUG1
							cout << Offset <<  "(PID = " <<Key<< ")" << endl;

							cout << Offset <<  "(Doing the second consultation)" << endl;
#endif

							// Getting the bind values on the HT block
							if(GetHTBindingValues(Category,Key,BottomUpBIDs) == OK)
								{
									if (BottomUpBIDs != 0)
										{
											if (BottomUpBIDs->size() > 0)
												{
													// Generate a SCN from the received block name
													GenerateSCNFromCharArrayBinaryPatterns(_BlockLN,HashBlockLegibleName);

													// Setting up the category
													Category=2;

													// Setting up the binding key
													Key=HashBlockLegibleName;

#ifdef DEBUG1
													cout << Offset <<  "(Doing the third consultation)" << endl;
#endif

													// Getting the bind values on the HT block
													if(GetHTBindingValues(Category,Key,TopDownBIDs) == OK)
														{
															if (TopDownBIDs != 0)
																{
																	if (TopDownBIDs->size() > 0)
																		{

#ifdef DEBUG1
																			cout << Offset <<  "(Preparing the results vector)" << endl;

																			cout << Offset <<  "(BottomUpBIDs size is "<<BottomUpBIDs->size()<<")" << endl;

																			cout << Offset <<  "(TopDownBIDs size is "<<TopDownBIDs->size()<<")" << endl;
#endif

																			for (unsigned int i=0; i<BottomUpBIDs->size(); i++)
																				{
#ifdef DEBUG1
																					_PB->S << Offset <<  "(BottomUpBIDs["<<i<<"] = " <<BottomUpBIDs->at(i)<< ")" << endl;
#endif
																					for (unsigned int j=0; j<TopDownBIDs->size(); j++)
																						{
#ifdef DEBUG1
																							_PB->S << Offset <<  "( TopDownBIDs["<<j<<"] = " <<TopDownBIDs->at(j)<< ")" << endl;
#endif

																							if (BottomUpBIDs->at(i) == TopDownBIDs->at(j))
																								{
																									_BIDs->push_back(TopDownBIDs->at(j));
#ifdef DEBUG1
																									_PB->S << Offset <<  "(BID = " <<TopDownBIDs->at(j)<< ")" << endl;
#endif

																									Status=OK;
																								}
																						}
																				}
																		}
																	else
																		{
																			cout << Offset <<  "(Warning: Top down BIDs vector is empty)" << endl;
																		}
																}
															else
																{
																	cout << Offset <<  "(Warning: Top down BIDs vector is null)" << endl;
																}
														}
													else
														{
															//cout << Offset <<  "(Warning: Third consultation returned an warning status)" << endl;
														}
												}
											else
												{
													cout << Offset <<  "(Warning: Bottom up BIDs vector is empty)" << endl;
												}

										}
									else
										{
											//cout << Offset <<  "(Warning: Bottom up BIDs vector is null)" << endl;
										}
								}
							else
								{
									//cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
								}
						}
					else
						{
							cout << Offset <<  "(Warning: PIDs vector is empty)" << endl;
						}
				}
			else
				{
					cout << Offset <<  "(Warning: PIDs vector is null)" << endl;
				}
		}
	else
		{
			cout << Offset <<  "(Warning: First consultation returned an warning status)" << endl;
		}

	delete PIDs;
	delete BottomUpBIDs;
	delete TopDownBIDs;

	return Status;
}

// Discover inside a Domain entities' 4 level-tuples that have the same legible names.
int	Process::DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames(string _ProcessLN, string _BlockLN, vector<Tuple*> &_4LTuple, Block *_PB)
{
	int 			Status=ERROR;
	string			Offset = "                                        ";

	vector<string>  *HIDs=new vector<string>;
	vector<string>  *OSIDs=new vector<string>;
	vector<string>  *PIDs=new vector<string>;
	vector<string>  *BIDs=new vector<string>;

	//cout << Offset <<  "(DiscoverHomonymsEntitiesTuplesFromProcessAndBlockLegibleNames = "<<_ProcessLN<<" and Block LN = "<<_BlockLN<<")" << endl;

	if (DiscoverHomonymsEntitiesIDsFromLN(9,"Host",HIDs,_PB) == OK)
		{
#ifdef DEBUG3
			cout << Offset <<  "(HIDs.size() = "<<HIDs->size()<<")" << endl;
#endif

			for (unsigned int i=0; i<HIDs->size(); i++)
				{
#ifdef DEBUG3
				cout << endl << Offset <<  "(HID ["<<i<<"] = "<<HIDs->at(i)<<")" << endl;
#endif
					if (DiscoverOSIDsFromHID(HIDs->at(i),OSIDs,_PB) == OK)
						{

#ifdef DEBUG3
							cout << Offset <<  "(OSIDs.size() = "<<OSIDs->size()<<")" << endl;
#endif

						for (unsigned int j=0; j<OSIDs->size(); j++)
						  {
#ifdef DEBUG3
							cout << endl << Offset <<  "(OSID ["<<j<<"] = "<<OSIDs->at(j)<<")" << endl;
#endif

							if (DiscoverHomonymsProcessesPIDsFromOSID(OSIDs->at(j),_ProcessLN,PIDs,_PB) == OK)
							  {
#ifdef DEBUG3
								cout << Offset <<  "(PIDs.size() = "<<PIDs->size()<<")" << endl;
#endif

								for (unsigned int k=0; k<PIDs->size(); k++)
								  {
#ifdef DEBUG3
									cout << endl << Offset <<  "(PID ["<<k<<"] = "<<PIDs->at(k)<<")" << endl;
#endif

									if (DiscoverHomonymsBlocksBIDsFromPID(PIDs->at(k),_BlockLN,BIDs,_PB) == OK)
									  {
#ifdef DEBUG3
										cout << Offset <<  "(BIDs.size() = "<<BIDs->size()<<")" << endl;
#endif

										if (BIDs->size() == 1)
										  {
											Tuple *Temp=new Tuple;

											Temp->Values.push_back(HIDs->at(i));

											Temp->Values.push_back(OSIDs->at(j));

											Temp->Values.push_back(PIDs->at(k));

											Temp->Values.push_back(BIDs->at(0));

											_4LTuple.push_back(Temp);

#ifdef DEBUG3
											cout << Offset <<  "(Saving a four level tuple!)" << endl;
#endif

											Status=OK;
										  }
										else
										  {
											cout << Offset <<  "(Warning: Unexpected number of homonyms blocks)" << endl;
										  }

										BIDs->clear();
									  }
									else
									  {
										//cout << Offset <<  "(Warning: Fourth consultation returned an warning status)" << endl;
									  }
								  }

								PIDs->clear();
							  }
							else
							  {
								//cout << Offset <<  "(Warning: Third consultation returned an warning status)" << endl;
							  }
						  }

						OSIDs->clear();
					  }
					else
					  {
						//cout << Offset <<  "(Warning: Second consultation returned an warning status)" << endl;
					  }
			  }
	  }
	else
	  {
		//cout << Offset <<  "(Warning: First consultation returned an warning status)" << endl;
	  }

  HIDs->clear();
  OSIDs->clear();
  PIDs->clear();
  BIDs->clear();

  delete HIDs;
  delete OSIDs;
  delete PIDs;
  delete BIDs;

  return Status;
}

// Discover inside a Process the existent Blocks
int Process::DiscoverBIDsFromPID(string _PID, vector<string> *& _BIDs, Block *_PB)
{
	int 			Status=ERROR;
	unsigned int	Category=0;
	string 			Version="0.1";
	string 			Key="";
	string			Offset = "                                        ";

	// Setting up the category
	Category=5;

	// Setting up the binding key
	Key=_PID;

	//cout << Offset <<  "(DiscoverBIDsFromPID = "<<_PID<<")" << endl;

	// Getting the bind values on the HT block
	if(GetHTBindingValues(Category,Key,_BIDs) == OK)
		{
			Status=OK;
		}
	else
		{
			cout << Offset <<  "(Warning: The GetHTBinding function returned warning status.)" << endl;
		}

	return Status;
}

// Discover inside an OS the existent Processes
int Process::DiscoverPIDsFromOSID(string _OSID, vector<string> *& _PIDs, Block *_PB)
{
	int 			Status=ERROR;
	unsigned int	Category=0;
	string 			Version="0.1";
	string 			Key="";
	string			Offset = "                                        ";

	// Setting up the category
	Category=5;

	// Setting up the binding key
	Key=_OSID;

	cout << Offset <<  "(DiscoverPIDsFromOSID = "<<_OSID<<")" << endl;

	// Getting the bind values on the HT block
	if(GetHTBindingValues(Category,Key,_PIDs) == OK)
		{
			Status=OK;
		}
	else
		{
			cout << Offset <<  "(Warning: The GetHTBinding function returned warning status.)" << endl;
		}

	return Status;
}

// Discover inside a Host the existent OSs
int Process::DiscoverOSIDsFromHID(string _HID, vector<string> *& _OSIDs, Block *_PB)
{
	int 			Status=ERROR;
	unsigned int	Category=0;
	string 			Version="0.1";
	string 			Key="";
	string			Offset = "                                        ";

	// Setting up the category
	Category=6;

	// Setting up the binding key
	Key=_HID;

	//cout << Offset <<  "(DiscoverOSIDsFromHID = "<<_HID<<")" << endl;

	// Getting the bind values on the HT block
	if(GetHTBindingValues(Category,Key,_OSIDs) == OK)
		{
			Status=OK;
		}
	else
		{
			cout << Offset <<  "(Warning: The GetHTBinding function returned warning status.)" << endl;
		}

	return Status;
}

// Discover inside a Domain the existent Hosts
int Process::DiscoverHIDsFromDID(string _DID, vector<string> *& _HIDs, Block *_PB)
{
	int 			Status=ERROR;
	unsigned int	Category=0;
	string 			Version="0.1";
	string 			Key="";
	string			Offset = "                                        ";

	// Setting up the category
	Category=7;

	// Setting up the binding key
	Key=_DID;

	//cout << Offset <<  "(DiscoverHIDsFromDID = "<<_DID<<")" << endl;

	// Getting the bind values on the HT block
	if(GetHTBindingValues(Category,Key,_HIDs) == OK)
		{
			Status=OK;
		}
	else
		{
			cout << Offset <<  "(Warning: The GetHTBinding function returned warning status.)" << endl;
		}

	return Status;
}

double Process::GetTime()
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((t.tv_sec)+(double)(t.tv_nsec/1e9));
}

string Process::DoubleToString(double _Double)
{
	stringstream ss;

	ss << _Double;

	return ss.str();
}

// Converting string to double
double Process::StringToDouble(string _String)
{
	return atof(_String.c_str());
}
