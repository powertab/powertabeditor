/////////////////////////////////////////////////////////////////////////////
// Name:            rehearsalsign.cpp
// Purpose:         Stores and renders Rehearsal Sign symbols
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 10, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "rehearsalsign.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

#include <sstream>

namespace PowerTabDocument {

// Default constants
const int8_t      RehearsalSign::DEFAULT_LETTER           = RehearsalSign::notSet;
const std::string RehearsalSign::DEFAULT_DESCRIPTION      = "";

/// Default Constructor
RehearsalSign::RehearsalSign() :
    m_letter(DEFAULT_LETTER), m_description(DEFAULT_DESCRIPTION)
{
}

/// Primary Constructor
/// @param letter Unique letter identifying the rehearsal sign
/// @param description Description for the rehearsal sign (i.e. Intro)
RehearsalSign::RehearsalSign(int8_t letter, const std::string& description) :
    m_letter(letter), m_description(description)
{
    assert(IsValidLetter(letter));
    assert(!description.empty());
}

/// Equality Operator
bool RehearsalSign::operator==(const RehearsalSign& rehearsalSign) const
{
    return ((m_letter == rehearsalSign.m_letter) &&
            (m_description == rehearsalSign.m_description));
}

/// Inequality Operator
bool RehearsalSign::operator!=(const RehearsalSign& rehearsalSign) const
{
    return (!operator==(rehearsalSign));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool RehearsalSign::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_letter;
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteMFCString(m_description);
    PTB_CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool RehearsalSign::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream >> m_letter;
    stream.ReadMFCString(m_description);
    return true;
}

// Letter Functions
/// Sets the letter used by the rehearsal sign
/// @param letter Letter to set
bool RehearsalSign::SetLetter(int8_t letter)
{
    //------Last Checked------//
    // - Dec 10, 2004
    PTB_CHECK_THAT(IsValidLetter(letter), false);
    m_letter = letter;

    // If the rehearsal sign is being unset, clear the description
    if (letter == notSet)
        m_description.clear();
    return (true);
}

// Operations
/// Gets a formatted text representation of the rehearsal sign (combines the
/// letter and the description)
/// @return The formatted text representation of the rehearsal sign
std::string RehearsalSign::GetFormattedText() const
{
    std::stringstream sstream;
    sstream << GetLetter() << " -- " << GetDescription();
    return sstream.str();
}

/// Determines if a letter is valid
/// @return True if the letter is valid, false if not
bool RehearsalSign::IsValidLetter(int8_t letter)
{
    return (letter >= 'A' && letter <= 'Z') || (letter == notSet);
}

/// Gets the letter used to uniquely identify the rehearsal sign
/// @return The letter used to uniquely identify the rehearsal sign
int8_t RehearsalSign::GetLetter() const
{
    return m_letter;
}

/// Sets the rehearsal sign description
/// @return True if the description was set, false if not
bool RehearsalSign::SetDescription(const std::string& description)
{
    PTB_CHECK_THAT(!description.empty(), false);
    m_description = description;
    return true;
}

/// Gets the rehearsal sign description
std::string RehearsalSign::GetDescription() const
{
    return m_description;
}

/// Determines if a RehearsalSign object is set (in use)
/// @return True if the RehearsalSign object is set, false if not
bool RehearsalSign::IsSet() const
{
    return GetLetter() != notSet;
}

/// Clears the rehearsal sign letter and description, and sets to not set
void RehearsalSign::Clear()
{
    SetLetter(notSet);
    m_description.clear();
}

}
