/*
        NovaGenesis

        Name:		HashFunction
        Object:		HashFunction
        File:		HashFunction.h
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

#ifndef _HASHFUNCTION_H
#define _HASHFUNCTION_H

#include <string>
#include <cstdint>

using namespace std;

/// Abstract base for hash function backends.
/// Each subclass wraps a specific hash algorithm into a uniform interface.
class HashFunction
{
public:
    virtual ~HashFunction() = default;

    /// Compute the hash of the given input bytes.
    /// @param input   Pointer to the input data
    /// @param size    Number of bytes in the input
    /// @param seed    Seed value for the hash function
    /// @param output  [out] Raw hash bytes (size depends on the function)
    virtual void Compute(const void* input, size_t size, uint32_t seed, void* output) = 0;

    /// Human-readable name of this hash function (e.g. "murmur3_x86_32").
    virtual string Name() const = 0;

    /// Default output size in bytes for this hash function.
    virtual size_t DefaultOutputSize() const = 0;
};

#endif // _HASHFUNCTION_H