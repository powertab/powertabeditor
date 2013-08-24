/////////////////////////////////////////////////////////////////////////////
// Name:            alternateending.cpp
// Purpose:         Stores and renders alternate ending symbols
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 3, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "alternateending.h"

#include <cmath>

namespace PowerTabDocument {

AlternateEnding::AlternateEnding()
{
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool AlternateEnding::Serialize(PowerTabOutputStream& stream) const
{
    return (SystemSymbol::Serialize(stream));
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool AlternateEnding::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    return (SystemSymbol::Deserialize(stream, version));
}

/// Get a list of all numbers that are set.
std::vector<uint8_t> AlternateEnding::GetListOfNumbers() const
{
    std::vector<uint8_t> numbers;

    for (uint8_t i = 1; i < daCapo; i++)
    {
        if (IsNumberSet(i))
        {
            numbers.push_back(i);
        }
    }

    return numbers;
}
		
/// Determines if a number is set
/// @param number A one-based index of the number to test
/// @return True if the number is set, false if not
bool AlternateEnding::IsNumberSet(uint32_t number) const
{
    PTB_CHECK_THAT(number >= 1 && number <= dalSegnoSegno, false);

    // Number is one based, so subtract one
    number--;

    // Determine if bit is set
    uint16_t numbers = HIWORD(m_data);
    uint16_t power = (uint16_t)pow((double)2, (double)number);
    return ((numbers & power) == power);
}

}
