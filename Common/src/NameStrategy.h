/*
        NovaGenesis

        Name:		NameStrategy
        Object:		NameStrategy
        File:		NameStrategy.h
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

#ifndef _NAMESTRATEGY_H
#define _NAMESTRATEGY_H

#include <string>
#include <vector>

using namespace std;

/// Forward declarations to avoid circular includes.
/// NameStrategy implementations may include the full headers as needed.
class Process;
class Block;

/// Abstract base for input-gathering strategies.
/// Each subclass defines how input data is assembled before hashing.
class NameStrategy
{
public:
    virtual ~NameStrategy() = default;

    /// Gather input bytes from a Process object for ID generation.
    virtual vector<const void*> GatherInputs(Process* _PP) = 0;

    /// Gather input bytes from a Block object for ID generation.
    virtual vector<const void*> GatherInputs(Block* _PB) = 0;

    /// Gather input bytes from a raw string.
    /// Returns a vector containing the string's c_str pointer.
    virtual vector<const void*> GatherInputs(const string& _Input) = 0;

    /// Gather input bytes from a raw char array.
    /// Returns a vector containing the array pointer.
    virtual vector<const void*> GatherInputs(const char* _Input, long long _Size) = 0;

    /// Human-readable name of this strategy.
    virtual string StrategyName() const = 0;
};

#endif // _NAMESTRATEGY_H