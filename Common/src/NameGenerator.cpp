/*
        NovaGenesis

        Name:		NameGenerator
        Object:		NameGenerator
        File:		NameGenerator.cpp
        Author:		Antonio Marcos Alberti
        Date:		07/2026
        Version:	0.1

        Copyright (C) 2026  Antonio Marcos Alberti

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

#include "NameGenerator.h"
#include "HashFunction.h"
#include "NameStrategy.h"
#include "MurmurHash3Strategy.h"
#include "DefaultNameStrategy.h"
#include "MurmurHash3.h"

#include <sstream>
#include <iomanip>
#include <iostream>

#define ERROR 1
#define OK 0

// ---------------------------------------------------------------------------
// The hash logic below is an EXACT copy of:
//   - Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string, string&)
//   - Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(char*, long long, string&)
//   - Block::GenerateSCNFromCharArrayBinaryPatterns4Bytes(const char*, long long, string&)
//   - Block::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string, string&)
//
// NO functional changes. Same seed (3571), same prime-multiplication loop,
// same hex encoding. The output is byte-for-byte identical.
// ---------------------------------------------------------------------------

NameGenerator::NameGenerator()
    : HashFn(make_shared<MurmurHash3Strategy>()),
      Strat(make_shared<DefaultNameStrategy>())
{
}

NameGenerator& NameGenerator::GetInstance()
{
    static NameGenerator instance;
    return instance;
}

void NameGenerator::SetHashFunction(shared_ptr<HashFunction> _HF)
{
    if (_HF)
        HashFn = _HF;
}

void NameGenerator::SetNameStrategy(shared_ptr<NameStrategy> _NS)
{
    if (_NS)
        Strat = _NS;
}

// ============================================================================
// GenerateFromString
//
// EXACT copy of:
//   Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string _Input, string& _SCN)
//   (which is itself identical to Block's GenerateSCNFromCharArrayBinaryPatterns4Bytes(string, string&))
// ============================================================================
string NameGenerator::GenerateFromString(const string& _Input)
{
    int Product = 0;
    const void* key;
    unsigned int* Bytes = new unsigned int;
    unsigned int* Temp = new unsigned int[4];
    unsigned char* Chars = new unsigned char[4];
    unsigned char* Output = new unsigned char[4];
    string Strings;
    long long _Size = _Input.size();
    unsigned int max = 16;

    if (_Input != "")
    {
        // Initializing the input array
        if (_Size < 17)
        {
            Strings = "";

            // Get the first 16 prime numbers (same as Process::primes)
            unsigned int primes[16];
            unsigned int n_primes = 0;

            for (unsigned int z = 2; n_primes < 16; z++)
            {
                bool is_prime = true;

                for (unsigned int w = 0; w < n_primes; w++)
                {
                    if (z % primes[w] == 0)
                    {
                        is_prime = false;
                        break;
                    }
                }

                if (is_prime)
                {
                    primes[n_primes] = z;
                    n_primes++;
                }
            }

            for (unsigned int b = 0; b < max; b++)
            {
                for (unsigned int c = 0; c < _Size; c++)
                {
                    Product = ((int)_Input[c]) * primes[b];

                    stringstream ss;

                    ss << Product;

                    Strings = Strings + ss.str();
                }
            }
        }

        unsigned long seed = 3571;

        *Bytes = 0;

        if (_Size < 17)
        {
            key = (const void*)Strings.c_str();

            MurmurHash3_x86_32(key, Strings.size(), seed, Bytes);
        }
        else
        {
            key = (const void*)_Input.c_str();

            MurmurHash3_x86_32(key, _Size, seed, Bytes);
        }

        Temp[0] = (*Bytes >> (8 * 3)) & 0xff;
        Temp[1] = (*Bytes >> (8 * 2)) & 0xff;
        Temp[2] = (*Bytes >> (8 * 1)) & 0xff;
        Temp[3] = (*Bytes >> (8 * 0)) & 0xff;

        for (int x = 0; x < 4; x++)
        {
            Chars[x] = ' ';
        }

        Chars[0] = (char)(Temp[0]);
        Chars[1] = (char)(Temp[1]);
        Chars[2] = (char)(Temp[2]);
        Chars[3] = (char)(Temp[3]);

        // Copying the hash code to the _SCN string
        for (int y = 0; y < 4; y++)
        {
            Output[y] = Chars[y];
        }

        // Converting to an hexadecimal string
        ostringstream ret;

        for (unsigned int f = 0; f < 4; f++)
        {
            ret << hex << std::setfill('0') << std::setw(2) << uppercase << (int)Output[f];
        }

        delete Bytes;
        delete[] Temp;
        delete[] Chars;
        delete[] Output;

        return ret.str();
    }
    else
    {
        cout << "          ERROR: null input array" << endl;

        delete Bytes;
        delete[] Temp;
        delete[] Chars;
        delete[] Output;

        return "";
    }
}

// ============================================================================
// GenerateFromCharArray
//
// EXACT copy of:
//   Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(char* _Input, long long _Size, string& _SCN)
// ============================================================================
string NameGenerator::GenerateFromCharArray(const char* _Input, long long _Size)
{
    int Product = 0;
    const void* key;
    unsigned int* Bytes = new unsigned int;
    unsigned int* Temp = new unsigned int[4];
    unsigned char* Chars = new unsigned char[4];
    unsigned char* Output = new unsigned char[4];
    string Strings;

    if (_Input != 0)
    {
        // Initializing the input array
        if (_Size < 17)
        {
            Strings = "";

            // Get the first 16 prime numbers
            unsigned int primes[16];
            unsigned int n_primes = 0;

            for (unsigned int z = 2; n_primes < 16; z++)
            {
                bool is_prime = true;

                for (unsigned int w = 0; w < n_primes; w++)
                {
                    if (z % primes[w] == 0)
                    {
                        is_prime = false;
                        break;
                    }
                }

                if (is_prime)
                {
                    primes[n_primes] = z;
                    n_primes++;
                }
            }

            for (unsigned int b = 0; b < 16; b++)
            {
                for (unsigned int c = 0; c < _Size; c++)
                {
                    Product = ((int)_Input[c]) * primes[b];

                    stringstream ss;

                    ss << Product;

                    Strings = Strings + ss.str();
                }
            }
        }

        unsigned long seed = 3571;

        *Bytes = 0;

        if (_Size < 17)
        {
            key = (const void*)Strings.c_str();

            MurmurHash3_x86_32(key, Strings.size(), seed, Bytes);
        }
        else
        {
            key = (const void*)_Input;

            MurmurHash3_x86_32(key, _Size, seed, Bytes);
        }

        Temp[0] = (*Bytes >> (8 * 3)) & 0xff;
        Temp[1] = (*Bytes >> (8 * 2)) & 0xff;
        Temp[2] = (*Bytes >> (8 * 1)) & 0xff;
        Temp[3] = (*Bytes >> (8 * 0)) & 0xff;

        for (int x = 0; x < 4; x++)
        {
            Chars[x] = ' ';
        }

        Chars[0] = (char)(Temp[0]);
        Chars[1] = (char)(Temp[1]);
        Chars[2] = (char)(Temp[2]);
        Chars[3] = (char)(Temp[3]);

        // Copying the hash code to the _SCN string
        for (int y = 0; y < 4; y++)
        {
            Output[y] = Chars[y];
        }

        // Converting to an hexadecimal string
        ostringstream ret;

        for (unsigned int f = 0; f < 4; f++)
        {
            ret << hex << std::setfill('0') << std::setw(2) << uppercase << (int)Output[f];
        }

        delete Bytes;
        delete[] Temp;
        delete[] Chars;
        delete[] Output;

        return ret.str();
    }
    else
    {
        cout << "          ERROR: null input array" << endl;

        delete Bytes;
        delete[] Temp;
        delete[] Chars;
        delete[] Output;

        return "";
    }
}

// ============================================================================
// GenerateFromProcess (stub for E4.5)
// ============================================================================
string NameGenerator::GenerateFromProcess(Process* _PP)
{
    // TODO: E4.5 — migrate Process::GenerateSCNFromProcessBinaryPatterns4Bytes
    (void)_PP;
    return "";
}

// ============================================================================
// GenerateFromBlock (stub for E4.6)
// ============================================================================
string NameGenerator::GenerateFromBlock(Block* _PB)
{
    // TODO: E4.6 — migrate Block::GenerateSCNFromBlockBinaryPatterns4Bytes
    (void)_PB;
    return "";
}