/////////////////////////////////////////////////////////////////////////////
// Name:            chordtext.cpp
// Purpose:         Stores and renders chord text
// Author:          Brad Larsen
// Modified by:
// Created:         Jan 3, 2005
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "chordtext.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default Constants
const uint8_t ChordText::DEFAULT_POSITION = 0;

// Constructor/Destructor
/// Default Constructor
ChordText::ChordText() :
    m_position(DEFAULT_POSITION)
{
    //------Last Checked------//
    // - Jan 3, 2005
}

/// Primary Constructor
/// @param position Zero-based index of the position within the system where the
/// chord text is anchored
/// @param chordName Chord name represented in the chord text
ChordText::ChordText(uint32_t position, const ChordName& chordName) :
    m_position((uint8_t)position), m_chordName(chordName)
{
    //------Last Checked------//
    // - Jan 3, 2005
    assert(IsValidPosition(position));
}

/// Copy Constructor
ChordText::ChordText(const ChordText& chordText) :
    PowerTabObject(), m_position(DEFAULT_POSITION)
{
    //------Last Checked------//
    // - Jan 3, 2005
    *this = chordText;
}

/// Destructor
ChordText::~ChordText()
{
    //------Last Checked------//
    // - Jan 3, 2005
}

// Operators
/// Assignment Operator
const ChordText& ChordText::operator=(const ChordText& chordText)
{
    //------Last Checked------//
    // - Jan 3, 2005

    // Check for assignment to self
    if (this != &chordText)
    {
        m_position = chordText.m_position;
        m_chordName = chordText.m_chordName;
    }
    return (*this);
}

/// Equality Operator
bool ChordText::operator==(const ChordText& chordText) const
{
    //------Last Checked------//
    // - Jan 3, 2005
    return (
        (m_position == chordText.m_position) &&
        (m_chordName == chordText.m_chordName)
    );
}

/// Inequality Operator
bool ChordText::operator!=(const ChordText& chordText) const
{
    //------Last Checked------//
    // - Jan 3, 2005
    return (!operator==(chordText));
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool ChordText::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Jan 3, 2005
    stream << m_position;
    PTB_CHECK_THAT(stream.CheckState(), false);

    m_chordName.Serialize(stream);
    PTB_CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool ChordText::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    stream >> m_position;

    m_chordName.Deserialize(stream, version);

    return true;
}

}
