/*
        NovaGenesis

        Name:		DefaultNameStrategy
        Object:		DefaultNameStrategy
        File:		DefaultNameStrategy.h
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

#ifndef _DEFAULTNAMESTRATEGY_H
#define _DEFAULTNAMESTRATEGY_H

#include "NameStrategy.h"

/// Preserves the current input-gathering logic exactly as it exists
/// in Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes and
/// Block::GenerateSCNFromCharArrayBinaryPatterns4Bytes.
///
/// The string-based methods (GatherInputs from string/char*) are fully
/// implemented with the exact same prime-multiplication logic.
/// Process* and Block* overloads are stubs to be filled during migration
/// (E4.5-E4.6) when we replace the old methods in Process.cpp and Block.cpp.
class DefaultNameStrategy : public NameStrategy
{
public:
    /// Prime number array (first 16 primes, same as Process::primes[16])
    unsigned int primes[16];

    DefaultNameStrategy();

    vector<const void*> GatherInputs(Process* _PP) override;
    vector<const void*> GatherInputs(Block* _PB) override;
    vector<const void*> GatherInputs(const string& _Input) override;
    vector<const void*> GatherInputs(const char* _Input, long long _Size) override;

    string StrategyName() const override { return "default"; }

private:
    void GeneratePrimes();
};

#endif // _DEFAULTNAMESTRATEGY_H