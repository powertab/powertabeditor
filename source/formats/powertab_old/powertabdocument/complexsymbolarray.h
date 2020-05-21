/*
  * Copyright (C) 2011 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#ifndef COMPLEXSYMBOLARRAY_H
#define COMPLEXSYMBOLARRAY_H

#include <algorithm>
#include <array>
#include <cstdint>
#include "macros.h"

/// Provides general functions for dealing with arrays of complex symbols
namespace ComplexSymbols
{
    enum
    {
        notUsed = 0
    };

    struct CompareComplexSymbolType
    {
        CompareComplexSymbolType(uint8_t symbolType) : symbolType(symbolType) {}

        /// Extracts the type of the symbol given, and compares it to the supplied symbol type
        inline bool operator()(uint32_t symbol) const
        {
            return HIBYTE(HIWORD(symbol)) == symbolType;
        }

    private:
        const uint8_t symbolType;
    };

    /// Gets the index of a given complex symbol type in the complex symbol array
    /// @param type Type of symbol to find
    /// @return Index within the array where the symbol was found, or -1 if not
    /// found
    template <size_t N>
    uint32_t findComplexSymbol(const std::array<uint32_t, N>& symbolArray, uint8_t type)
    {
        typename std::array<uint32_t, N>::const_iterator symbol =
                std::find_if(symbolArray.begin(), symbolArray.end(), CompareComplexSymbolType(type));

        if (symbol != symbolArray.end())
        {
            return static_cast<uint32_t>(symbol - symbolArray.begin());
        }
        else
        {
            return -1;
        }
    }

    /// Gets the number of complex symbols used by the note
    /// @return The number of complex symbols used by the note
    template <size_t N>
    size_t getComplexSymbolCount(const std::array<uint32_t, N>& symbolArray)
    {
        return  symbolArray.size() -
                std::count(symbolArray.begin(), symbolArray.end(), static_cast<uint32_t>(notUsed));
    }

    /// Clears the contents of the symbol array (sets all elements to "not used")
    template <size_t N>
    void clearComplexSymbols(std::array<uint32_t, N>& symbolArray)
    {
        std::fill(symbolArray.begin(), symbolArray.end(), static_cast<uint32_t>(notUsed));
    }

    /// Removes a complex symbol from the complex symbol array
    /// @param type Type of symbol to remove
    /// @return True if the symbol was removed, false if not
    template <size_t N>
    void removeComplexSymbol(std::array<uint32_t, N>& symbolArray, uint8_t type)
    {
        std::replace_if(symbolArray.begin(), symbolArray.end(),
                        CompareComplexSymbolType(type), static_cast<uint32_t>(notUsed));
    }

    /// Adds a complex symbol to the complex symbol array
    /// @param symbolData Data that makes up the symbol
    template <size_t N>
    void addComplexSymbol(std::array<uint32_t, N>& symbolArray, uint32_t symbolData)
    {
        const uint8_t type = HIBYTE(HIWORD(symbolData));

        // search for the symbol in the array
        typename std::array<uint32_t, N>::iterator symbol =
                std::find_if(symbolArray.begin(), symbolArray.end(), CompareComplexSymbolType(type));

        // if the symbol was already in the array, update it
        if (symbol != symbolArray.end())
        {
            *symbol = symbolData;
        }
        // otherwise, find the first free array slot and insert it there
        else
        {
            typename std::array<uint32_t, N>::iterator firstUnused =
                    std::find(symbolArray.begin(), symbolArray.end(), static_cast<uint32_t>(notUsed));

            if (firstUnused != symbolArray.end())
            {
                *firstUnused = symbolData;
            }
        }
    }
}

#endif // COMPLEXSYMBOLARRAY_H
