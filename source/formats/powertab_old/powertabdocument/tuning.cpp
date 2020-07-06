/////////////////////////////////////////////////////////////////////////////
// Name:            tuning.cpp
// Purpose:         Stores a tuning used by a stringed instrument
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 14, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "tuning.h"
#include "generalmidi.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

const char*     Tuning::DEFAULT_NAME                    = "";
const uint8_t   Tuning::DEFAULT_DATA                    = 0;
const int8_t    Tuning::MIN_MUSIC_NOTATION_OFFSET       = -12;
const int8_t    Tuning::MAX_MUSIC_NOTATION_OFFSET       = 12;
const uint32_t  Tuning::MIN_STRING_COUNT                = 3;
const uint32_t  Tuning::MAX_STRING_COUNT                = 8;

// Constructors/Destructors
/// Default Constructor
Tuning::Tuning() :
    m_name(DEFAULT_NAME), m_data(DEFAULT_DATA)
{
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Tuning::Serialize(PowerTabOutputStream& stream) const
{
    stream.WriteMFCString(m_name);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream << m_data;
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteSmallVector(m_noteArray);

    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Tuning::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream.ReadMFCString(m_name);
    stream >> m_data;
    stream.ReadSmallVector(m_noteArray);
    return true;
}

/// Gets the music notation offset
/// @return The music notation offset, in semi-tones
int8_t Tuning::GetMusicNotationOffset() const
{
    int8_t returnValue = (int8_t)((m_data & musicNotationOffsetValueMask) >> 1);

    // If the sign is set, the value is negative
    if ((m_data & musicNotationOffsetSignMask) == musicNotationOffsetSignMask)
        returnValue = -returnValue;

    return (returnValue);
}

std::vector<uint8_t> Tuning::GetTuningNotes() const
{
    return m_noteArray;
}

}
