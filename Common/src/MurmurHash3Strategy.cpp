/*
        NovaGenesis

        Name:		MurmurHash3Strategy
        Object:		MurmurHash3Strategy
        File:		MurmurHash3Strategy.cpp
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

#include "MurmurHash3Strategy.h"
#include "MurmurHash3.h"

void MurmurHash3Strategy::Compute(const void* input, size_t size, uint32_t seed, void* output)
{
    // Delegates to the existing MurmurHash3_x86_32 function.
    // This is the exact same call used throughout Process.cpp and Block.cpp.
    MurmurHash3_x86_32(input, (int)size, seed, output);
}