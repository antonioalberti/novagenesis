/*
        NovaGenesis

        Name:		NameGenerator
        Object:		NameGenerator
        File:		NameGenerator.h
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

#ifndef _NAMEGENERATOR_H
#define _NAMEGENERATOR_H

#include <string>
#include <memory>

using namespace std;

class HashFunction;
class NameStrategy;
class Process;
class Block;
class Message;

/// Global singleton responsible for ALL name (SCN/BID/PID) generation.
///
/// v1 focus: structural refactor that preserves the EXACT same hash output.
/// The string/char* methods copy the prime-multiplication + MurmurHash3_x86_32
/// logic from the existing Process/Block methods without any functional change.
///
/// Strategy pattern allows future:
///   - Different hash functions (via HashFunction subclasses)
///   - Different input-gathering strategies (via NameStrategy subclasses)
///   - Configurable output sizes (32/64/128/256-bit names)
class NameGenerator
{
public:
    /// Access the global singleton.
    static NameGenerator& GetInstance();

    // -----------------------------------------------------------------------
    // Name generation — exact copies of existing Process/Block hash logic
    // -----------------------------------------------------------------------

    /// Generate a 4-byte (32-bit) SCN from a string.
    /// EXACT copy of Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(string, string&).
    /// Uses the same seed (3571), prime-multiplication loop, and hex encoding.
    string GenerateFromString(const string& _Input);

    /// Generate a 4-byte (32-bit) SCN from a char array.
    /// EXACT copy of Process::GenerateSCNFromCharArrayBinaryPatterns4Bytes(char*, long long, string&).
    string GenerateFromCharArray(const char* _Input, long long _Size);

    // -----------------------------------------------------------------------
    // Future: Process/Block overloads (stubs for E4.5/E4.6)
    // -----------------------------------------------------------------------

    /// Generate a name from a Process object. (To be implemented in E4.5)
    string GenerateFromProcess(Process* _PP);

    /// Generate a name from a Block object. (To be implemented in E4.6)
    string GenerateFromBlock(Block* _PB);

    /// Generate a 4-byte (32-bit) SCN from a Message object.
    /// EXACT copy of Block::GenerateSCNFromMessageBinaryPatterns4Bytes.
    string GenerateFromMessage(Message* _M);

    // -----------------------------------------------------------------------
    // Strategy management
    // -----------------------------------------------------------------------

    /// Set the hash function backend (default: MurmurHash3Strategy).
    void SetHashFunction(shared_ptr<HashFunction> _HF);

    /// Set the input-gathering strategy (default: DefaultNameStrategy).
    void SetNameStrategy(shared_ptr<NameStrategy> _NS);

    /// Get the current hash function backend.
    shared_ptr<HashFunction> GetHashFunction() const { return HashFn; }

    /// Get the current input-gathering strategy.
    shared_ptr<NameStrategy> GetNameStrategy() const { return Strat; }

    /// Seed value (fixed at 3571, matching existing code).
    static constexpr unsigned long DEFAULT_SEED = 3571;

private:
    // Singleton — private constructor
    NameGenerator();

    // No copying
    NameGenerator(const NameGenerator&) = delete;
    NameGenerator& operator=(const NameGenerator&) = delete;

    /// The hash function backend (wraps MurmurHash3_x86_32 by default).
    shared_ptr<HashFunction> HashFn;

    /// The input-gathering strategy (DefaultNameStrategy by default).
    shared_ptr<NameStrategy> Strat;
};

#endif // _NAMEGENERATOR_H