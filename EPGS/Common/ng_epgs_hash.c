/*
	NovaGenesis

	Name:		EPGS novagenesis hash
	Object:		ng_epgs_hash
	File:		ng_epgs_hash.c
	Author:		Vâner José Magalhães
	Date:		05/2021
	Version:	0.1

  	Copyright (C) 2021  Vâner José Magalhães

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

#include "ng_epgs_hash.h"
#include "../epgs_defines.h"
#include "../epgs_wrapper.h"


int GenerateSCNFromCharArrayBinaryPatterns4Bytes(const char *_Input, int _Size, char** ret)
{
	int				 			Product=0;
	const void*					key;
	unsigned int				*Bytes;
	unsigned int				*Temp;
	unsigned char 				*Chars;
	unsigned char	 			*Output;
	char*						Strings;
	int							Status=NG_ERROR;
	unsigned int 				primes[16];

	Bytes = (unsigned int*) ng_malloc(sizeof (unsigned int));
	Temp = (unsigned int*) ng_malloc(sizeof (unsigned int) * 4);
	Chars = (unsigned char*) ng_malloc(sizeof (unsigned char) * 4);
	Output = (unsigned char*) ng_malloc(sizeof (unsigned char) * 4);
	Strings = (char*) ng_malloc(1000);
	char* tempRet = (char*) ng_calloc(sizeof(char), 1000);

	// Generate the required prime numbers
	unsigned int n_primes = 0;
	unsigned int z = 2;
	for (z = 2; n_primes < 16; z++)
	{
		unsigned int w = 0;
		for (w = 0; w < n_primes; w++)
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

	if (_Input != 0)
	{
		// Initializing the input array
		if (_Size < 17)
		{
			Strings[0]='\0';

			unsigned int b=0;
			for (b=0; b<16; b++)
			{
				unsigned int c=0;
				for (c=0; c<_Size; c++)
				{
					Product=((int)_Input[c])*primes[b];
					ng_sprintf(Strings + ng_strlen(Strings), "%d", Product);
				}
			}
		}

		unsigned long seed=3571;

		*Bytes=0;

		if (_Size < 17)
		{
			//cout<< "Strings = "<<Strings<<endl;

			key=(const void*)Strings;

			MurmurHash3_x86_32(key,ng_strlen(Strings),seed,Bytes);
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

		int x=0;
		for (x=0; x<4; x++)	{Chars[x]=' ';}

		Chars[0]=(char)(Temp[0]);
		Chars[1]=(char)(Temp[1]);
		Chars[2]=(char)(Temp[2]);
		Chars[3]=(char)(Temp[3]);

		// Copying the hash code to the _SCN string
		int y=0;
		for (y=0; y<4; y++)
		{
			//ng_printf("%i %d %c \n",y,Chars[y],Chars[y]);

			Output[y]=Chars[y];
		}

		unsigned int f = 0;
		for (f = 0; f < 4; f++)
		{
			ng_sprintf(tempRet + ng_strlen(tempRet), "%02X", Output[f]);
		}


		Status=NG_OK;
	}
	*ret = tempRet;

	ng_free(Bytes);
    ng_free(Temp);
    ng_free(Chars);
    ng_free(Output);
    ng_free(Strings);

	return Status;
}
