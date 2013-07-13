/////////////////////////////////////////////////////////////////////////////
// Name:            systemsymbol.cpp
// Purpose:         Base class for symbols residing on a system scope
// Author:          Brad Larsen
// Modified by:     
// Created:         Jan 13, 2005
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "systemsymbol.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default constants
const uint16_t        SystemSymbol::DEFAULT_SYSTEM            = 0;
const uint8_t        SystemSymbol::DEFAULT_POSITION          = 0;
const uint32_t      SystemSymbol::DEFAULT_DATA              = 0;

// System Constants
const uint32_t      SystemSymbol::MIN_SYSTEM                = 0;
const uint32_t      SystemSymbol::MAX_SYSTEM                = 65535;

// Position Constants
const uint32_t      SystemSymbol::MIN_POSITION              = 0;
const uint32_t      SystemSymbol::MAX_POSITION              = 255;

// Constructor/Destructor
/// Default Constructor
SystemSymbol::SystemSymbol() : 
    m_system(DEFAULT_SYSTEM), m_position(DEFAULT_POSITION), m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Jan 13, 2005
}

/// Primary Constructor
/// @param system Zero-based index of the system where the system symbol is
/// anchored
/// @param position Zero-based index of the position within the system where the
/// system symbol is anchored
/// @param data Data used by the symbol (the data format is dependant upon the
/// derived class)
SystemSymbol::SystemSymbol(uint32_t system, uint32_t position, uint32_t data) :
    m_system(DEFAULT_SYSTEM), m_position(DEFAULT_POSITION), m_data(data)
{
    //------Last Checked------//
    // - Jan 13, 2005
    assert(IsValidSystem(system));
    assert(IsValidPosition(position));
    SetSystem(system);
    SetPosition(position);    
}

/// Copy Constructor
SystemSymbol::SystemSymbol(const SystemSymbol& systemSymbol) :
    PowerTabObject(), m_system(DEFAULT_SYSTEM), m_position(DEFAULT_POSITION), 
    m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Jan 13, 2005
    *this = systemSymbol;
}

/// Destructor
SystemSymbol::~SystemSymbol()
{
    //------Last Checked------//
    // - Jan 13, 2005
}

/// Assignment Operator
const SystemSymbol& SystemSymbol::operator=(const SystemSymbol& systemSymbol)
{
    //------Last Checked------//
    // - Jan 13, 2005

    // Check for assignment to self
    if (this != &systemSymbol)
    {
        m_system = systemSymbol.m_system;
        m_position = systemSymbol.m_position;
        m_data = systemSymbol.m_data;
    }
    return (*this);
}

/// Equality Operator
bool SystemSymbol::operator==(const SystemSymbol& systemSymbol) const
{
    //------Last Checked------//
    // - Jan 12, 2005
    return (
        (m_system == systemSymbol.m_system) &&
        (m_position == systemSymbol.m_position) &&
        (m_data == systemSymbol.m_data)
    );
}

/// Inequality Operator
bool SystemSymbol::operator!=(const SystemSymbol& systemSymbol) const
{
    //------Last Checked------//
    // - Jan 12, 2005
    return (!operator==(systemSymbol));
}

/// Order symbols by their system, then position index
bool SystemSymbol::operator<(const SystemSymbol& systemSymbol) const
{
    if (m_system == systemSymbol.m_system)
    {
        return m_position < systemSymbol.m_position;
    }
    else
    {
        return m_system < systemSymbol.m_system;
    }
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool SystemSymbol::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Jan 13, 2005
    stream << m_system << m_position << m_data;
    PTB_CHECK_THAT(stream.CheckState(), false);
    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool SystemSymbol::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream >> m_system >> m_position >> m_data;
    return true;
}

}
