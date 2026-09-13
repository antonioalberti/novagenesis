/*
        NovaGenesis

        Name:		DefaultNameStrategy
        Object:		DefaultNameStrategy
        File:		DefaultNameStrategy.cpp
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

#include "DefaultNameStrategy.h"
#include "Process.h"
#include "Block.h"

// ---------------------------------------------------------------------------
// DefaultNameStrategy provides the input bytes that NameGenerator will hash.
// It mirrors the current Process/Block input-gathering logic.
// For the string/char* case, the raw string bytes are returned directly.
// The prime-multiplication loop lives in NameGenerator (tightly coupled with
// the hash step, matching the existing code structure).
// ---------------------------------------------------------------------------

DefaultNameStrategy::DefaultNameStrategy()
{
    GeneratePrimes();
}

void DefaultNameStrategy::GeneratePrimes()
{
    // Generate the first 16 primes — identical to Process constructor logic
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
}

vector<const void*> DefaultNameStrategy::GatherInputs(Process* _PP)
{
    vector<const void*> inputs;
    // TODO: E4.5 — implement with Process input-gathering logic
    (void)_PP;
    return inputs;
}

vector<const void*> DefaultNameStrategy::GatherInputs(Block* _PB)
{
    vector<const void*> inputs;
    // TODO: E4.6 — implement with Block input-gathering logic
    (void)_PB;
    return inputs;
}

vector<const void*> DefaultNameStrategy::GatherInputs(const string& _Input)
{
    vector<const void*> inputs;
    inputs.push_back((const void*)_Input.c_str());
    return inputs;
}

vector<const void*> DefaultNameStrategy::GatherInputs(const char* _Input, long long _Size)
{
    vector<const void*> inputs;
    inputs.push_back((const void*)_Input);
    (void)_Size;
    return inputs;
}