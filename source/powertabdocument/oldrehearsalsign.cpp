/////////////////////////////////////////////////////////////////////////////
// Name:            oldrehearsalsign.cpp
// Purpose:         Used to retrieve rehearsal signs in older file versions
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 27, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "oldrehearsalsign.h"

#include "rehearsalsign.h"      // Needed for ConstructRehearsalSign

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

// Default Constants
const uint16_t        OldRehearsalSign::DEFAULT_SYSTEM            = 0;
const uint8_t        OldRehearsalSign::DEFAULT_POSITION          = 0;
const uint32_t      OldRehearsalSign::DEFAULT_DATA              = 0;
const int8_t        OldRehearsalSign::DEFAULT_LETTER            = 'A';
const char*       OldRehearsalSign::DEFAULT_DESCRIPTION       = "";
    
/// Default Constructor
OldRehearsalSign::OldRehearsalSign() :
    m_system(DEFAULT_SYSTEM), m_position(DEFAULT_POSITION),
    m_data(DEFAULT_DATA), m_letter(DEFAULT_LETTER),
    m_description(DEFAULT_DESCRIPTION)
{
    //------Last Checked------//
    // - Jan 3, 2005
}

/// Destructor
OldRehearsalSign::~OldRehearsalSign()
{
    //------Last Checked------//
    // - Jan 3, 2005
}

// Construction Functions
/// Constructs a RehearsalSign object using OldRehearsalSign data
/// @param rehearsalSign RehearsalSign object to construct
/// @return True if the object was constructed, false if not
bool OldRehearsalSign::ConstructRehearsalSign(
    RehearsalSign& rehearsalSign) const
{
    //------Last Checked------//
    // - Jan 3, 2005
    if (!rehearsalSign.SetLetter(m_letter))
        return (false);
    if (!rehearsalSign.SetDescription(m_description.c_str()))
        return (false);
    return (true);
}

// Operators
/// Assignment Operator
const OldRehearsalSign& OldRehearsalSign::operator=(
    const OldRehearsalSign& oldRehearsalSign)
{
    //------Last Checked------//
    // - Jan 5, 2005
    
    // Check for assignment to self
    if (this != &oldRehearsalSign)
    {
        m_system = oldRehearsalSign.m_system;
        m_position = oldRehearsalSign.m_position;
        m_data = oldRehearsalSign.m_data;
        m_letter = oldRehearsalSign.m_letter;
        m_description = oldRehearsalSign.m_description;
    }
    return (*this);
}

/// Equality Operator
bool OldRehearsalSign::operator==(
    const OldRehearsalSign& oldRehearsalSign) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (
        (m_system == oldRehearsalSign.m_system) &&
        (m_position == oldRehearsalSign.m_position) &&
        (m_data == oldRehearsalSign.m_data) &&
        (m_letter == oldRehearsalSign.m_letter) &&
        (m_description == oldRehearsalSign.m_description)
    );
}

/// Inequality Operator
bool OldRehearsalSign::operator!=(
    const OldRehearsalSign& oldRehearsalSign) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (!operator==(oldRehearsalSign));
}
        
// Serialization Functions
/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool OldRehearsalSign::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    UNUSED(version);
    
    stream >> m_system >> m_position >> m_data >> m_letter;
    CHECK_THAT(stream.CheckState(), false);
    
    stream.ReadMFCString(m_description);
    CHECK_THAT(stream.CheckState(), false);
    
    return (stream.CheckState());
}
