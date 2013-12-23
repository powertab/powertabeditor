/////////////////////////////////////////////////////////////////////////////
// Name:            direction.cpp
// Purpose:         Stores and renders directions
// Author:          Brad Larsen
// Modified by:     
// Created:         Jan 12, 2005
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "direction.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default constants
const uint8_t        Direction::DEFAULT_POSITION                 = 0;

// Position Constants
const uint32_t      Direction::MIN_POSITION                     = 0;
const uint32_t      Direction::MAX_POSITION                     = 255;

// Symbol Constants
const uint8_t        Direction::MAX_SYMBOLS                      = 3;
const uint8_t        Direction::NUM_SYMBOL_TYPES                 = 19;

// Repeat Number Constants
const uint8_t        Direction::MIN_REPEAT_NUMBER                = 0;
const uint8_t        Direction::MAX_REPEAT_NUMBER                = 31;

static std::string shortDirectionText[Direction::NUM_SYMBOL_TYPES] =
{
    "Coda", "Double Coda", "Segno", "Segno Segno",
    "Fine", "D.C.", "D.S.", "D.S.S.", "To Coda",
    "To Dbl. Coda", "D.C. al Coda", "D.C. al Dbl. Coda",
    "D.S. al Coda", "D.S. al Dbl. Coda", "D.S.S. al Coda",
    "D.S.S. al Dbl. Coda", "D.C. al Fine", "D.S. al Fine",
    "D.S.S. al Fine"
};

static std::string longDirectionText[Direction::NUM_SYMBOL_TYPES] =
{
    "Coda", "Double Coda", "Segno", "Segno Segno",
    "Fine", "Da Capo", "Dal Segno", "Dal Segno Segno", "To Coda",
    "To Double Coda", "Da Capo al Coda", "Da Capo al Double Coda",
    "Dal Segno al Coda", "Dal Segno al Double Coda", "Dal Segno Segno al Coda",
    "Dal Segno Segno al Double Coda", "Da Capo al Fine", "Dal Segno al Fine",
    "Dal Segno Segno al Fine"
};

// Constructor/Destructor
/// Default Constructor
Direction::Direction() : 
    m_position(DEFAULT_POSITION)
{
}

/// Primary Constructor
/// @param position Zero-based index of the position within the system where the
/// barline is anchored
/// @param symbolType Type of symbol to add (see symbolTypes enum for values)
/// @param activeSymbol Symbol that must be active for the symbol to be
/// triggered (see activeSymbols enum for values)
/// @param repeatNumber Repeat number that must be active for the symbol to be
/// triggered (0 = none)
Direction::Direction(uint32_t position, uint8_t symbolType,
                     uint8_t activeSymbol, uint8_t repeatNumber)
    : m_position(position)
{
    assert(IsValidPosition(position));
    AddSymbol(symbolType, activeSymbol, repeatNumber);   
}

Direction::Direction(const Direction& direction) :
    PowerTabObject(), m_position(DEFAULT_POSITION)
{
    *this = direction;
}

const Direction& Direction::operator=(const Direction& direction)
{
    if (this != &direction)
    {
        m_position = direction.m_position;
        m_symbolArray = direction.m_symbolArray;
    }
    return *this;
}

bool Direction::operator==(const Direction& direction) const
{
    return (m_position == direction.m_position &&
            m_symbolArray == direction.m_symbolArray);
}

bool Direction::operator!=(const Direction& direction) const
{
    return !operator==(direction);
}

bool Direction::operator<(const Direction &direction) const
{
    return m_position < direction.m_position;
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Direction::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_position;
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteSmallVector(m_symbolArray);
    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Direction::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream >> m_position;

    stream.ReadSmallVector(m_symbolArray);
    return true;
}

/// Determines whether a position is valid
/// @param position Position to validate
/// @return True if the position is valid, false if not
bool Direction::IsValidPosition(size_t position)
{
    return position <= MAX_POSITION;
}

/// Sets the position within the system where the direction is anchored
/// @param position Zero-based index within the system where the direction
/// is anchored
/// @return True if the position was set, false if not
bool Direction::SetPosition(size_t position)
{
    PTB_CHECK_THAT(IsValidPosition(position), false);
    m_position = static_cast<uint8_t>(position);
    return true;
}

/// Gets the position within the system where the direction is anchored
/// @return The position within the system where the direction is anchored
size_t Direction::GetPosition() const                           
{
    return m_position;
}

/// Determines if a symbol type is valid
/// @param symbolType Symbol type to validate
/// @return True if the symbol is valid, false if not
bool Direction::IsValidSymbolType(uint8_t symbolType)            
{
    return symbolType <= dalSegnoSegnoAlFine;
}

/// Determines if an active symbol is valid
/// @param activeSymbol Active symbol to validate
/// @return True if the active symbol is valid, false if not
bool Direction::IsValidActiveSymbol(uint8_t activeSymbol)
{
    return activeSymbol <= activeDalSegnoSegno;
}

/// Determines if an repeat number is valid
/// @param repeatNumber Repeat number to validate
/// @return True if the repeat number is valid, false if not
bool Direction::IsValidRepeatNumber(uint8_t repeatNumber)
{
    return repeatNumber <= MAX_REPEAT_NUMBER;
}

// Symbol Functions
/// Adds a symbol to the symbol array
/// @param symbolType Type of symbol to add (see symbolTypes enum for values)
/// @param activeSymbol Symbol that must be active for the symbol to be
/// triggered (see activeSymbols enum for values)
/// @param repeatNumber Repeat number that must be active for the symbol to be
/// triggered (0 = none)
/// @return True if the symbol was added, false if not
bool Direction::AddSymbol(uint8_t symbolType, uint8_t activeSymbol,
    uint8_t repeatNumber)
{
    PTB_CHECK_THAT(IsValidSymbolType(symbolType), false);
    PTB_CHECK_THAT(IsValidActiveSymbol(activeSymbol), false);
    PTB_CHECK_THAT(IsValidRepeatNumber(repeatNumber), false);
    
    // Can't add anymore symbols
    if (GetSymbolCount() == MAX_SYMBOLS)
        return false;

    // Add a symbol to the end of the array, then set the data    
    m_symbolArray.push_back(0);
    return (SetSymbol(GetSymbolCount() - 1, symbolType, activeSymbol,
        repeatNumber));
}

/// Sets the data for an existing symbol in the symbol array
/// @param index Index of the symbol to set the data for
/// @param symbolType Type of symbol (see symbolTypes enum for values)
/// @param activeSymbol Symbol that must be active for the symbol to be
/// triggered (see activeSymbols enum for values)
/// @param repeatNumber Repeat number that must be active for the symbol to be
/// triggered (0 = none)
/// @return True if the symbol data was set, false if not
bool Direction::SetSymbol(size_t index, uint8_t symbolType,
    uint8_t activeSymbol, uint8_t repeatNumber)
{
    PTB_CHECK_THAT(IsValidSymbolIndex(index), false);
    PTB_CHECK_THAT(IsValidSymbolType(symbolType), false);
    PTB_CHECK_THAT(IsValidActiveSymbol(activeSymbol), false);
    PTB_CHECK_THAT(IsValidRepeatNumber(repeatNumber), false);
    
    uint16_t symbol = (uint16_t)(symbolType << 8);
    symbol |= (uint16_t)(activeSymbol << 6);
    symbol |= (uint16_t)repeatNumber;
   
    m_symbolArray[index] = symbol;
    
    return true;
}

/// Gets the symbol stored in the nth index of the symbol array
/// @param index Index of the symbol to get
/// @param symbolType Holds the symbol type return value
/// @param activeSymbol Holds the active symbol return value
/// @param repeatNumber Holds the repeat number return value
/// @return True if the direction data was retrieved, false if not
bool Direction::GetSymbol(size_t index, uint8_t& symbolType,
    uint8_t& activeSymbol, uint8_t& repeatNumber) const
{
    PTB_CHECK_THAT(IsValidSymbolIndex(index), false);
   
    symbolType = activeSymbol = repeatNumber = 0;
    
    symbolType = (uint8_t)((m_symbolArray[index] & symbolTypeMask) >> 8);
    activeSymbol = (uint8_t)((m_symbolArray[index] & activeSymbolMask) >> 6);
    repeatNumber = (uint8_t)(m_symbolArray[index] & repeatNumberMask);
    
    return true;
}

/// Determines if a symbol in the symbol array is a given type
/// @param index Index of the symbol
/// @param symbolType Type of symbol to test against
/// @return True if the symbol is of the type, false if not
bool Direction::IsSymbolType(size_t index, uint8_t symbolType) const
{
    PTB_CHECK_THAT(IsValidSymbolIndex(index), false);
    PTB_CHECK_THAT(IsValidSymbolType(symbolType), false);
    
    uint8_t type = 0;
    uint8_t activeSymbol = 0;
    uint8_t repeatNumber = 0;
    
    if (!GetSymbol(index, type, activeSymbol, repeatNumber))
    {
        return false;
    }
        
    return type == symbolType;
}

/// Removes a symbol from the symbol array
/// @param index Index of the symbol to remove
/// @return True if the symbol was removed, false if not
bool Direction::RemoveSymbolAtIndex(size_t index)
{
    PTB_CHECK_THAT(IsValidSymbolIndex(index), false);
 
    m_symbolArray.erase(m_symbolArray.begin() + index);
    
    return true;
}

/// Gets a text representation of a symbol, such as "D.S.S. al Fine".
/// @param index Index of the symbol to get the text for
/// @return Text representation of the symbol
std::string Direction::GetText(size_t index) const
{
    uint8_t symbolType = 0;
    uint8_t activeSymbol = 0;
    uint8_t repeatNumber = 0;
    
    if (!GetSymbol(index, symbolType, activeSymbol, repeatNumber))
    {
        return "";
    }
    
    PTB_CHECK_THAT(IsValidSymbolType(symbolType), "");
    
    return shortDirectionText[symbolType];
}

/// Returns the detailed text representation of a symbol, such as
/// "Dal Segno Segno al Fine".
std::string Direction::GetDetailedText(uint8_t symbolType)
{
    PTB_CHECK_THAT(IsValidSymbolType(symbolType), "");
    return longDirectionText[symbolType];
}

/// Determines if a symbol index is valid
/// @param index Index to validate
/// @return True if the symbol index is valid, false if not
bool Direction::IsValidSymbolIndex(size_t index) const         
{
    return index < GetSymbolCount();
}

/// Gets the number of symbols in the symbol array
/// @return The number of symbols in the symbol array
size_t Direction::GetSymbolCount() const
{
    return m_symbolArray.size();
}

}
