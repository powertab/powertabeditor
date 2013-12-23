/////////////////////////////////////////////////////////////////////////////
// Name:            staff.cpp
// Purpose:         Stores and renders a staff
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "staff.h"

#include "note.h"
#include "position.h"
#include "powertabinputstream.h"
#include "powertaboutputstream.h"
#include "tuning.h"

namespace PowerTabDocument {

// Default Constants
const uint8_t Staff::DEFAULT_DATA                                        = (uint8_t)((DEFAULT_CLEF << 4) | DEFAULT_TABLATURE_STAFF_TYPE);
const uint8_t Staff::DEFAULT_CLEF                                        = TREBLE_CLEF;
const uint8_t Staff::DEFAULT_TABLATURE_STAFF_TYPE                        = 6;
const uint8_t Staff::DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING       = 9;
const uint8_t Staff::DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING       = 9;
const uint8_t Staff::DEFAULT_SYMBOL_SPACING                              = 0;
const uint8_t Staff::DEFAULT_TABLATURE_STAFF_BELOW_SPACING               = 0;
const uint8_t Staff::STD_NOTATION_LINE_SPACING = 7;
const uint8_t Staff::STD_NOTATION_STAFF_TYPE = 5;
const uint8_t Staff::STAFF_BORDER_SPACING = 10;
const uint8_t Staff::TAB_SYMBOL_HEIGHT = 10;

// Clef Constants
const uint8_t Staff::TREBLE_CLEF                         = 0;
const uint8_t Staff::BASS_CLEF                           = 1;

// Tablature Staff Type Constants
const uint8_t Staff::MIN_TABULATURE_STAFF_TYPE = Tuning::MIN_STRING_COUNT;
const uint8_t Staff::MAX_TABULATURE_STAFF_TYPE = Tuning::MAX_STRING_COUNT;

/// Default Constructor
Staff::Staff() :
    m_data(DEFAULT_DATA),
    m_standardNotationStaffAboveSpacing(DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING),
    m_standardNotationStaffBelowSpacing(DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING),
    m_symbolSpacing(DEFAULT_SYMBOL_SPACING),
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING),
    m_isShown(true)
{
}

/// Primary Constructor
/// @param tablatureStaffType The type of tablature staff to set (number of strings)
/// @param clef Type of clef to set (see CLEF constants)
Staff::Staff(uint8_t tablatureStaffType, uint8_t clef) :
    m_data(DEFAULT_DATA),
    m_standardNotationStaffAboveSpacing(DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING),
    m_standardNotationStaffBelowSpacing(DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING),
    m_symbolSpacing(DEFAULT_SYMBOL_SPACING),
    m_tablatureStaffBelowSpacing(DEFAULT_TABLATURE_STAFF_BELOW_SPACING),
    m_isShown(true)
{
    SetClef(clef);
    SetTablatureStaffType(tablatureStaffType);
}

Staff::~Staff()
{
    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        std::vector<Position*>& positionArray = positionArrays[i];
        for (size_t j = 0; j < positionArray.size(); j++)
        {
            delete positionArray[j];
        }
    }
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Staff::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    stream << m_data << m_standardNotationStaffAboveSpacing <<
        m_standardNotationStaffBelowSpacing << m_symbolSpacing <<
        m_tablatureStaffBelowSpacing;
    PTB_CHECK_THAT(stream.CheckState(), false);

    // TODO - should we serialize the number of voices??
    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        stream.WriteVector(positionArrays[i]);
        PTB_CHECK_THAT(stream.CheckState(), false);
    }

    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Staff::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    stream >> m_data >> m_standardNotationStaffAboveSpacing >>
              m_standardNotationStaffBelowSpacing >> m_symbolSpacing >>
              m_tablatureStaffBelowSpacing;

    for (size_t i = 0; i < positionArrays.size(); i++)
    {
        stream.ReadVector(positionArrays[i], version);
    }

    return true;
}

/// Sets the clef used on the standard notation staff
/// @param clef Clef to set
/// @return True if the clef was set, false if not
bool Staff::SetClef(uint8_t clef)
{
    //------Last Checked------//
    // - Jan 5, 2005
    PTB_CHECK_THAT(clef <= 1, false);

    m_data &= ~clefMask;
    m_data |= (uint8_t)(clef << 4);

    return (true);
}

/// Sets the tablature staff type
/// @param type Tablature staff type to set
/// @return True if the tablature staff type was set, false if not
bool Staff::SetTablatureStaffType(uint8_t type)
{
    PTB_CHECK_THAT(Tuning::IsValidStringCount(type), false);
    PTB_CHECK_THAT(IsValidTablatureStaffType(type), false);

    m_data &= ~tablatureStaffTypeMask;
    m_data |= type;

    return (true);
}

/// Determines if a Clef is valid
/// @param clef Clef to validate
/// @return True if the clef is valid, false if not
bool Staff::IsValidClef(uint8_t clef)
{
    return clef == TREBLE_CLEF || clef == BASS_CLEF;
}

/// Gets the clef used on the standard notation staff
/// @return The clef used on the standard notation staff
uint8_t Staff::GetClef() const
{
    return (m_data & clefMask) >> 4;
}

/// Determines if a Tablature Staff Type is valid. In addition to checking
/// whether the number of strings is in range, it will check if all notes in
/// the staff are within the range.
/// @param type Tablature staff type to validate.
/// @return True if the tablature staff type is valid, false if not.
bool Staff::IsValidTablatureStaffType(uint8_t type) const
{
    if (type < MIN_TABULATURE_STAFF_TYPE || type > MAX_TABULATURE_STAFF_TYPE)
    {
        return false;
    }

    for (size_t i = 0; i < positionArrays.size(); ++i)
    {
        const std::vector<Position*>& voice = positionArrays[i];
        for (size_t j = 0; j < voice.size(); ++j)
        {
            const Position* pos = voice.at(j);
            for (size_t m = 0; m < pos->GetNoteCount(); ++m)
            {
                const Note* note = pos->GetNote(m);
                if (note->GetString() >= type)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

/// Gets the tablature staff type (3-7 strings)
/// @return The tablature staff type
uint8_t Staff::GetTablatureStaffType() const
{
    return m_data & tablatureStaffTypeMask;
}

/// Determines if a voice is valid
/// @param voice Voice to validate
/// @return True if the voice is valid, false if not
bool Staff::IsValidVoice(size_t voice)
{
    return voice < NUM_STAFF_VOICES;
}

/// Determines if a position index is valid
/// @param voice Voice the position belongs to
/// @param index position index to validate
/// @return True if the position index is valid, false if not
bool Staff::IsValidPositionIndex(size_t voice, size_t index) const
{
    PTB_CHECK_THAT(IsValidVoice(voice), false);
    return (index < GetPositionCount(voice));
}

/// Gets the number of positions in the staff
/// @param voice Voice of the positions to get the count of
/// @throw std::out_of_range if the voice is invalid
size_t Staff::GetPositionCount(size_t voice) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    return positionArrays[voice].size();
}

/// Gets the nth position in the staff
/// @param voice Voice the position belongs to
/// @param index Index of the position to get
/// @throw std::out_of_range if the voice or index are invalid
Position* Staff::GetPosition(size_t voice, size_t index) const
{
    if (!IsValidVoice(voice))
        throw std::out_of_range("Invalid voice");

    return positionArrays[voice].at(index);
}

/// Returns whether or not the staff is visible
bool Staff::IsShown() const
{
    return m_isShown;
}

}
