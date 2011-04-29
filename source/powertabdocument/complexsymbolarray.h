#ifndef COMPLEXSYMBOLARRAY_H
#define COMPLEXSYMBOLARRAY_H

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
        bool operator()(uint32_t symbol) const
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
        auto symbol = std::find_if(symbolArray.begin(), symbolArray.end(),
                                   CompareComplexSymbolType(type));

        if (symbol != symbolArray.end())
        {
            return symbol - symbolArray.begin();
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
                std::count(symbolArray.begin(), symbolArray.end(), notUsed);
    }

    /// Clears the contents of the symbol array (sets all elements to "not used")
    template <size_t N>
    void clearComplexSymbols(std::array<uint32_t, N>& symbolArray)
    {
        std::fill(symbolArray.begin(), symbolArray.end(), notUsed);
    }

    /// Removes a complex symbol from the complex symbol array
    /// @param type Type of symbol to remove
    /// @return True if the symbol was removed, false if not
    template <size_t N>
    void removeComplexSymbol(std::array<uint32_t, N>& symbolArray, uint8_t type)
    {
        std::replace_if(symbolArray.begin(), symbolArray.end(),
                        CompareComplexSymbolType(type), notUsed);
    }

    /// Adds a complex symbol to the complex symbol array
    /// @param symbolData Data that makes up the symbol
    template <size_t N>
    void addComplexSymbol(std::array<uint32_t, N>& symbolArray, uint32_t symbolData)
    {
        const uint8_t type = HIBYTE(HIWORD(symbolData));

        // search for the symbol in the array
        auto symbol = std::find_if(symbolArray.begin(), symbolArray.end(),
                                   CompareComplexSymbolType(type));

        // if the symbol was already in the array, update it
        if (symbol != symbolArray.end())
        {
            *symbol = symbolData;
        }
        // otherwise, find the first free array slot and insert it there
        else
        {
            auto firstUnused = std::find(symbolArray.begin(), symbolArray.end(), notUsed);

            if (firstUnused != symbolArray.end())
            {
                *firstUnused = symbolData;
            }
        }
    }
}

#endif // COMPLEXSYMBOLARRAY_H
