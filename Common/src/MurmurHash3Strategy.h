/*
        NovaGenesis

        Name:		MurmurHash3Strategy
        Object:		MurmurHash3Strategy
        File:		MurmurHash3Strategy.h
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

#ifndef _MURMURHASH3STRATEGY_H
#define _MURMURHASH3STRATEGY_H

#include "HashFunction.h"
#include <cstdint>

/// Wraps the existing MurmurHash3_x86_32 into the HashFunction interface.
/// Preserves the exact same algorithm, seed handling, and 32-bit (4-byte) output.
class MurmurHash3Strategy : public HashFunction
{
public:
    void Compute(const void* input, size_t size, uint32_t seed, void* output) override;

    string Name() const override { return "murmur3_x86_32"; }

    size_t DefaultOutputSize() const override { return 4; }  // 32 bits
};

#endif // _MURMURHASH3STRATEGY_H