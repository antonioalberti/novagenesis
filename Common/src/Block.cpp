/*
	NovaGenesis

	Name:		Block
	Object:		Block
	File:		Block.cpp
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

#ifndef _BLOCK_H
#include "Block.h"
#endif

#ifndef _MURMURHASH3_H_
#include "MurmurHash3.h"
#endif

#ifndef _PROCESS_H
#include "Process.h"
#endif

////#define DEBUG // To follow message processing

Block::Block(string _LN, Process *_PP, unsigned int _Index, string _Path)
{
	LN=_LN;
	Index=_Index;
	SCN="";
	PP=_PP;
	StopProcessingMessage=false;
	Path=_Path;
	string			Offset = "          ";

	// Generate the required prime numbers
    unsigned int n_primes = 0;

    for (unsigned int z = 2; n_primes < 32; z++)
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

	GenerateSCNFromBlockBinaryPatterns(this,SCN);

	cout<<Offset<<"("<<LN<<" block has the SCN = "<<SCN<<")"<<endl;
}

Block::~Block()
{
}

// Set block legible name
void Block::SetLegibleName(string _LN)
{
	LN=_LN;
}

// Set block self-certifying name
void Block::SetSelfCertifyingName(string _SCN)
{
	SCN=_SCN;
}

// Set working path
void Block::SetPath(string _Path)
{
  Path=_Path;
}

// Set operational state
void Block::SetState(string _State)
{
	State=_State;
}


// Get block legible name
string Block::GetLegibleName()
{
	return LN;
}

// Get block self-certifying name
string Block::GetSelfCertifyingName()
{
	return SCN;
}

// Get working path
string Block::GetPath()
{
	return Path;
}

// Get operational state
string Block::GetState()
{
	return State;
}

// Get index
unsigned int Block::GetIndex()
{
	return Index;
}

// Run the actions behind a received message, generating a resulting message
int Block::Run(Message *_ReceivedMessage, Message *&_InlineResponseMessage)
{
	int 			Status=ERROR;			// Overall execution status
	vector<int>		CLStatus;				// Status of every command line action
	unsigned int 	NCL=0;					// Number of command lines
	CommandLine     *PCL=0;					// Pointer to the command line being executed
	Action			*PA1=0;					// Pointer to the action that is performing such execution
	Action			*PA2=0;					// Pointer to the action that is performing such execution
	ActionsIterator it;						// Iterator to find out a value
	unsigned int 	i=0;					// Command lines counter
	string			Offset="          ";	// Auxiliary variable for logging

	if (LN != "GW") {Offset = "                    ";}

#ifdef DEBUG

	S << endl << endl;

	S << Offset << "*************************************************************************************************"<<endl;
	S << Offset << "   Block = "<<LN<<", State = "<<State<<", SCN = " << SCN << endl;
	S << Offset << "*************************************************************************************************"<<endl;

	S << Offset << "*****************************************"<<endl;
	S << Offset << "   Block = "<<LN<<", State = "<<State<< endl;
	S << Offset << "*****************************************"<<endl;

	S << endl << "(" << endl << *_ReceivedMessage << ")" << endl;

#endif

	// ***********************************************************************
	// Call the corresponding actions to perform the processing
	// ***********************************************************************

	if (PP->OkToRun(_ReceivedMessage) == true)
		{
			if (_ReceivedMessage->GetNumberofCommandLines(NCL) == OK)
				{
					if (NCL != 0)
						{
#ifdef DEBUG
							S << endl << Offset <<"(NCL = "<<NCL<<")"<< endl;
#endif

							CLStatus.resize(NCL,ERROR);

							// Vector of indexes to the block Messages container
							vector<Message*> ScheduledMessages;

							while (i<NCL && StopProcessingMessage==false)
								{
									if (_ReceivedMessage->GetCommandLine(i,PCL) == OK)
										{
											if (PCL != 0)
												{
													if (PCL->Name != "" && PCL->Alternative != "")
														{
															char C1=PCL->Name[0];
															char C2=PCL->Alternative[0];
															char C3=PCL->Alternative[1];

															if (C1 == '-' && C2 == '-' && C3 == '-')
																{
																	// Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
																	string LN1(PCL->Name+" "+PCL->Alternative+" "+PCL->Version);

																	// Set the legible name of the command line to be processed, e.g. "-sr --b 0.1"
																	string LN2(PCL->Name+" "+PCL->Alternative+" 0.1");

#ifdef DEBUG

																	S << Offset << "(LN1 = "<<LN1<<", LN2 = "<<LN2<<".)"<< endl;
#endif

																	for (unsigned int j=0; j<Actions.size(); j++)
																		{
																			if (Actions[j]->GetLegibleName() == LN1)
																				{
																					PA1=Actions[j];

#ifdef DEBUG
																					S << Offset << "(Found)" << endl;
#endif

																					break;
																				}
																		}

																	if (PA1 != 0)
																		{
#ifdef DEBUG
																			S << Offset <<  "(Action is "<<PA1->GetLegibleName()<<")"<< endl;
#endif

																			// Call the action
																			CLStatus[i]=PA1->Run(_ReceivedMessage,PCL,ScheduledMessages,_InlineResponseMessage);

																			PA1=0;

																			PA2=0;
																		}
																	else
																		{

#ifdef DEBUG
																			S << Offset << "(Warning: "<<LN1<<" not implemented.)" << endl;
#endif

																			for (unsigned int j=0; j<Actions.size(); j++)
																				{
																					if (Actions[j]->GetLegibleName() == LN2)
																						{
																							PA2=Actions[j];

																							break;
																						}
																				}
																		}

																	if (PA1 == 0 && PA2 != 0)
																		{
#ifdef DEBUG
																			S << Offset <<  "(Action is "<<PA2->GetLegibleName()<<")"<< endl;
#endif

																			// Call the action
																			CLStatus[i]=PA2->Run(_ReceivedMessage,PCL,ScheduledMessages,_InlineResponseMessage);

																			PA2=0;
																		}
																}
															else
																{
																	S << Offset << "(ERROR: Corrupted command line (2)!)" << endl;

																	CLStatus[i]=ERROR;

																	StopProcessingMessage=true;

																	_ReceivedMessage->MarkToDelete();

																	break;
																}
														}
													else
														{
															S << Offset << "(ERROR: Corrupted command line (1)!)" << endl;
														}
												}
											else
												{
													S << "(ERROR: Invalid command line)" << endl;

													CLStatus[i]=ERROR;
												}
										}
									else
										{
											S << "(ERROR: Unable to get the command line)" << endl;

											CLStatus[i]=ERROR;
										}

									i++;
								}

							// Set overall status
							for (unsigned int j=0; j<NCL; j++)
								{
									if (CLStatus[j] == ERROR) {Status = ERROR;}
								}

							StopProcessingMessage=false;

						}
					else
						{
							S << "(ERROR: Invalid number of command lines)" << endl;

							Status=ERROR;
						}
				}
			else
				{
					S << "(ERROR: Unable to get the number of command lines)" << endl;

					Status=ERROR;
				}
		}
	else
		{
			S << "(ERROR: Unable to run a message that is not at the Messages container)" << endl;
		}

	return Status;
}

// Allocate and add an Action on Actions container
void Block::NewAction(const string _LN, Action *&_PA)
{
}

// Get an Action
int Block::GetAction(string _LN, Action *&_PA)
{
	return ERROR;
}

// Delete an Action
int Block::DeleteAction(string _LN)
{
	return ERROR;
}

// Auxiliary functions
int Block::StringToInt(string _String)
{
	stringstream ss(_String);

	int Temp=0;

	ss >> Temp;

	return Temp;
}

string Block::IntToString(int _Int)
{
	stringstream ss;

	ss << _Int;

	return ss.str();
}

string Block::DoubleToString(double _Double)
{
	stringstream ss;

	ss << _Double;

	return ss.str();
}

// Converting string to double
double Block::StringToDouble(string _String)
{
	return atof(_String.c_str());
}

string Block::UnsignedIntToString(unsigned int _Int)
{
	string		 ret;
	stringstream ss;

	ss << _Int;

	ret=ss.str();

	return ret;
}

// Generate block self-certified name from its binary patterns
void Block::GenerateSCNFromBlockBinaryPatterns4Bytes(Block *_PB,string &_SCN)
{
	int				 			Product=0;
	string						Strings;
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	string 						Input;

	if (_PB != 0)
		{
			std::stringstream map;
			map << _PB;
			map << _PB->State;
			map << &_PB->Actions;
			map << _PB->LN;

			// Setting the input integers for each round
			Input=map.str();

			//cout<< endl <<"Input = "<<Input<<endl;

			Strings="";

			for (unsigned int b=0; b<32; b++)
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
					// S << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

			        ret << hex << std::setfill('0') << std::setw(2) << uppercase << (int)Chars[f];
				}

			_SCN= ret.str();

			//_SCN=_SCN+"_"+LN+"_BID";

			//cout<< "SCN = "<<_SCN<<endl;
		}
	else
		{
			S << "(ERROR: Invalid block pointer)" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
}

// Generate block self-certified name from its binary patterns
void Block::GenerateSCNFromBlockBinaryPatterns16Bytes(Block *_PB,string &_SCN)
{
	vector<string> 				Strings(4);
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	long long 					*Input=new long long [4];
	unsigned char	 			*Output=new unsigned char[16];
	int							Index=0;

	if (_PB != 0)
		{
			// Setting the input integers for each round
			Input[0]=(((long long)_PB))*(110503)*_PB->Index;
			Input[1]=(((long long)&_PB->State))*(132049)*_PB->Index;
			Input[2]=(((long long)&_PB->Actions))*(216091)*_PB->Index;
			Input[3]=(((long long)&_PB->LN))*(756839)*_PB->Index;

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

					Chars[0]=(unsigned char)(Temp[0]);
					Chars[1]=(unsigned char)(Temp[1]);
					Chars[2]=(unsigned char)(Temp[2]);
					Chars[3]=(unsigned char)(Temp[3]);

					// Copying the hash code to the _SCN string
					for (unsigned int y=0; y<4; y++)
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
					// S << " f = "<<f<<" (int)Output[f] = "<<(int)Output[f];

			        ret << hex << std::setfill('0') << std::setw(2) << uppercase << (int)Output[f];
				}

			 _SCN= ret.str();

			 //_SCN=_SCN+"_"+LN+"_BID";
		}
	else
		{
			S << "(ERROR: Invalid block pointer)" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
	delete[] Input;
	delete[] Output;
}

// Generate block self-certified name from its binary patterns
void Block::GenerateSCNFromBlockBinaryPatterns32Bytes(Block *_PB,string &_SCN)
{
	vector<string> 				Strings(8);
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	long long 					*Input=new long long[8];
	unsigned char	 			*Output=new unsigned char[32];
	int							Index=0;

	if (_PB != 0)
		{
			// Setting the input integers for each round
			Input[0]=(((long long)_PB))*(110503);
			Input[1]=(((long long)&_PB->State))*(132049);
			Input[2]=(((long long)&_PB->Actions))*(216091);
			Input[3]=(((long long)&_PB->LN))*(756839);
			Input[4]=(((long long)&_PB->SCN))*(859433);
			Input[5]=(((long long)&_PB->Path))*(1257787);
			Input[6]=(((long long)_PB->PP))*(1398269);
			Input[7]=(((long long)_PB))*(2976221);

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

					Chars[0]=(unsigned char)(Temp[0]);
					Chars[1]=(unsigned char)(Temp[1]);
					Chars[2]=(unsigned char)(Temp[2]);
					Chars[3]=(unsigned char)(Temp[3]);

					// Copying the hash code to the _SCN string
					for (unsigned int y=0; y<4; y++)
						{
							//printf("%i %d %c \n",y,Chars[y],Chars[y]);

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
			S << "(ERROR: Invalid block pointer)" << endl;
		}

	delete Bytes;
	delete[] Temp;
	delete[] Chars;
	delete[] Input;
	delete[] Output;
}

int Block::GenerateSCNFromCharArrayBinaryPatterns4Bytes(const char *_Input, long long _Size, string &_SCN)
{
	int				 			Product=0;
	const void*					key;
	unsigned int				*Bytes=new unsigned int;
	unsigned int				*Temp=new unsigned int[4];
	unsigned char 				*Chars=new unsigned char[4];
	unsigned char	 			*Output=new unsigned char[4];
	string						Strings;
	int							Status=ERROR;

	//cout<<"_Input = "<<_Input<<endl;

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

			//cout << "SCN  = " << _SCN << endl;

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

int Block::GenerateSCNFromCharArrayBinaryPatterns16Bytes(const char *_Input, long long _Size, string &_SCN)
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

int Block::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string _Input, string &_SCN)
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


int Block::GenerateSCNFromCharArrayBinaryPatterns16Bytes(string _Input, string &_SCN)
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
int Block::GenerateSCNFromMessageBinaryPatterns4Bytes(Message *_M, string &_SCN)
{
	int	Status=ERROR;

	stringstream ss;

	ss << *_M;

	string Temp=ss.str();

	if (Temp.size() > 0)
		{
			Status=GenerateSCNFromCharArrayBinaryPatterns4Bytes(Temp,_SCN);
		}

	return Status;
}

// Generate block self-certified name from its binary patterns
int Block::GenerateSCNFromMessageBinaryPatterns16Bytes(Message *_M, string &_SCN)
{
	int	Status=ERROR;

	stringstream ss;

	ss << *_M;

	string Temp=ss.str();

	if (Temp.size() > 0)
		{
			Status=GenerateSCNFromCharArrayBinaryPatterns16Bytes(Temp,_SCN);
		}

	return Status;
}

// Generate block self-certified name from its binary patterns
void Block::GenerateSCNFromBlockBinaryPatterns(Block *_PB, string &_SCN)
{
	GenerateSCNFromBlockBinaryPatterns4Bytes(_PB,_SCN);

	//cout<<"(SCN = "<<_SCN<<")"<<endl;
}

// Generate a self-certified name from a char array
int Block::GenerateSCNFromCharArrayBinaryPatterns(const char *_Input, long long _Size, string &_SCN)
{
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(_Input,_Size,_SCN);

	//cout<<"(SCN = "<<_SCN<<")"<<endl;
}

// Generate a self-certified name from a char array
int Block::GenerateSCNFromCharArrayBinaryPatterns(string _Input, string &_SCN)
{
	GenerateSCNFromCharArrayBinaryPatterns4Bytes(_Input,_SCN);

	//cout<<"(SCN = "<<_SCN<<")"<<endl;
}

// Generate a self-certified name from a message object ostream
int Block::GenerateSCNFromMessageBinaryPatterns(Message *_M, string &_SCN)
{
	GenerateSCNFromMessageBinaryPatterns4Bytes(_M,_SCN);

	//cout<<"(SCN = "<<_SCN<<")"<<endl;
}

double Block::GetTime()
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);

	return ((t.tv_sec)+(double)(t.tv_nsec/1e9));
}



