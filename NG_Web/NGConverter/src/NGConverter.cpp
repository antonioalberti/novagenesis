//============================================================================
// Name        : NGConverter.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>

#include "MurmurHash3.h"
#include "File.h"

using namespace std;

int GetFileContentHash(string FileName, string &_SCN);
int GenerateSCNFromCharArrayBinaryPatterns16Bytes(const char *_Input, long long _Size, string &_SCN);

int main(int argc, char *argv[])
{
	if( argc < 2 ) {
		printf("Usage: %s <filename>\n",argv[0]);
		return 0;
	}

	string output;
	GetFileContentHash(argv[1],output);

	printf("%s\n",output.c_str());

	return 0;
}

// Get file content hash
int GetFileContentHash(string FileName, string &_SCN)
{
	int 					Status=1;
	File 					F1;
	long long	 			PayloadSize;
	string					PayloadString;
	string					PayloadHash;
	string					Offset = "                              ";

	F1.OpenInputFile(FileName,"BINARY");

	F1.seekg (0, ios::end);

	// Read the number of characters in the file.
	PayloadSize=F1.tellg();

	//S << Offset <<  "(The payload size is = "<<PayloadSize<<")"<<endl;

	if (PayloadSize > 0)
		{
			char *Payload=new char[PayloadSize];

			F1.seekg (0);

			F1.read(Payload,PayloadSize);

			GenerateSCNFromCharArrayBinaryPatterns16Bytes(Payload,PayloadSize,_SCN);

			delete[] Payload;

			Status=0;
		}

	F1.CloseFile();

	return Status;
}

int GenerateSCNFromCharArrayBinaryPatterns16Bytes(const char *_Input, long long _Size, string &_SCN)
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

					ret << hex << setfill('0') << setw(2)<< uppercase << (int)Output[f];
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
