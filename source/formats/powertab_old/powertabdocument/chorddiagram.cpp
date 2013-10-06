/////////////////////////////////////////////////////////////////////////////
// Name:            chorddiagram.cpp
// Purpose:         Stores and renders a chord diagram
// Author:          Brad Larsen
// Modified by:     
// Created:         Dec 16, 2004
// RCS-ID:          
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "chorddiagram.h"
#include "tuning.h"
#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default Constants
const uint8_t ChordDiagram::DEFAULT_TOP_FRET     = 0;

// Top Fret Constants
const uint8_t ChordDiagram::MIN_TOP_FRET         = 0;
const uint8_t ChordDiagram::MAX_TOP_FRET         = 20;

// Fret Number Constants
const uint8_t ChordDiagram::MIN_FRET_NUMBER      = 0;
const uint8_t ChordDiagram::MAX_FRET_NUMBER      = 24;

/// Default Constructor
ChordDiagram::ChordDiagram() :
    m_topFret(DEFAULT_TOP_FRET)
{
}

/// Primary Constructor
/// @param chordName Chord name represented in the chord diagram
/// @param topFret The fret represented at the top of the chord diagram (zero =
/// the nut)
/// @param fretNumbers Fret numbers for each string in the chord diagram (from high to low)
ChordDiagram::ChordDiagram(const ChordName& chordName, uint8_t topFret,
                           const std::vector<uint8_t>& fretNumbers)
    : m_topFret(topFret)
{
    assert(IsValidTopFret(topFret));
    SetChordName(chordName);
    AddFretNumbers(fretNumbers);
}

/// Secondary Constructor
/// @param topFret The fret represented at the top of the chord diagram (zero =
/// the nut)
/// @param fretNumbers Fret numbers for each string in the chord diagram (from high to low)
ChordDiagram::ChordDiagram(uint8_t topFret, const std::vector<uint8_t>& fretNumbers) :
    m_topFret(topFret)
{
    assert(IsValidTopFret(topFret));
    AddFretNumbers(fretNumbers);
}

/// Copy Constructor
ChordDiagram::ChordDiagram(const ChordDiagram& chordDiagram) :
    PowerTabObject(), m_topFret(DEFAULT_TOP_FRET)
{
    *this = chordDiagram;
}

// Operators
/// Assignment Operator
const ChordDiagram& ChordDiagram::operator=(const ChordDiagram& chordDiagram)
{
    m_chordName = chordDiagram.m_chordName;
    m_topFret = chordDiagram.m_topFret;
    m_fretNumberArray = chordDiagram.m_fretNumberArray;
    return *this;
}

/// Equality Operator
bool ChordDiagram::operator==(const ChordDiagram& chordDiagram) const
{
    return (m_chordName == chordDiagram.m_chordName &&
            m_topFret == chordDiagram.m_topFret &&
            m_fretNumberArray == chordDiagram.m_fretNumberArray);
}

/// Inequality Operator
bool ChordDiagram::operator!=(const ChordDiagram& chordDiagram) const
{
    return (!operator==(chordDiagram));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool ChordDiagram::Serialize(PowerTabOutputStream& stream) const
{
    m_chordName.Serialize(stream);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream << m_topFret;
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteSmallVector(m_fretNumberArray);
    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool ChordDiagram::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    m_chordName.Deserialize(stream, version);

    stream >> m_topFret;

    stream.ReadSmallVector(m_fretNumberArray);
    return true;
}

// Chord Diagram Functions
/// Sets the contents of the ChordDiagram object
/// @param chordName Chord name represented in the chord diagram
/// @param topFret The fret represented at the top of the chord diagram (zero =
/// the nut)
/// @param fretNumbers Fret numbers for each string in the chord diagram (from high to low)
/// @return True if the chord diagram was set, false if not
bool ChordDiagram::SetChordDiagram(const ChordName& chordName, uint8_t topFret,
                                   const std::vector<uint8_t>& fretNumbers)
{
    SetChordName(chordName);
    if (!SetTopFret(topFret))
        return false;
    return AddFretNumbers(fretNumbers);
}

// Fret Number Functions
/// Determines if a fret number is valid
/// @param fretNumber Fret number to validate
/// @return True if the fret number is valid, false if not
bool ChordDiagram::IsValidFretNumber(uint8_t fretNumber)
{
    return (fretNumber <= MAX_FRET_NUMBER) ||
           (fretNumber == stringMuted) ||
           (fretNumber == notUsed);
}

/// Sets the fret number for an existing string in the chordDiagram
/// @param string String to set the fret number for
/// @param fretNumber Fret number to set
/// @return True if the fret number was set, false if not
bool ChordDiagram::SetFretNumber(uint32_t string, uint8_t fretNumber)
{
    PTB_CHECK_THAT(IsValidString(string), false);
    PTB_CHECK_THAT(IsValidFretNumber(fretNumber), false);
    m_fretNumberArray[string] = fretNumber;
    return (true);
}

/// Gets the fret number on a given string
/// @param string String to get the fret number for
/// @return The fret number on the string
uint8_t ChordDiagram::GetFretNumber(uint32_t string) const
{
    PTB_CHECK_THAT(IsValidString(string), 0);
    return m_fretNumberArray[string];
}

/// Sets the fret numbers for the chord diagram
/// @param fretNumbers Fret numbers for each string in the chord diagram (from high to low)
/// @return True if the chord diagram fret numbers were set, false if not
bool ChordDiagram::AddFretNumbers(const std::vector<uint8_t>& fretNumbers)
{
    PTB_CHECK_THAT(Tuning::IsValidStringCount(fretNumbers.size()), false);

    for (auto &fretNumber : fretNumbers)
    {
        PTB_CHECK_THAT(IsValidFretNumber(fretNumber), false);
    }

    m_fretNumberArray = fretNumbers;
    return true;
}

// Voicing Functions
/// Determines if the chord diagram's voicing is the same as that of another
/// ChordDiagram object
/// @param chordDiagram ChordDiagram object to compare with
/// @return True if the chord diagrams have the same voicing, false if not
bool ChordDiagram::IsSameVoicing(const ChordDiagram& chordDiagram) const
{
    return chordDiagram.m_fretNumberArray == this->m_fretNumberArray;
}

/// Determines if the chord diagram's voicing is the same
/// @param fretNumbers Fret numbers for each string in the chord diagram (from high to low)
/// @return True if all of the chord diagram fret numbers match, false if not
bool ChordDiagram::IsSameVoicing(const std::vector<uint8_t>& fretNumbers) const
{
    ChordDiagram temp;
    temp.AddFretNumbers(fretNumbers);
    return IsSameVoicing(temp);
}

// Operations
/// Gets the spelling for the chord diagram (i.e. 0 2 2 1 0 0)
/// @return The spelling for the chord diagram
std::string ChordDiagram::GetSpelling() const
{
    std::stringstream text;

    for (size_t i = GetStringCount(); i > 0; i--)
    {
        if (m_fretNumberArray[i - 1] == stringMuted)
        {
            text << "x";
        }
        else
        {
            text << static_cast<int>(m_fretNumberArray[i-1]);
        }

        if (i > 1)
        {
            text << " ";
        }
    }

    return text.str();
}

void ChordDiagram::SetChordName(const ChordName& chordName)
{
    m_chordName = chordName;
}

const ChordName& ChordDiagram::GetChordName() const
{
    return m_chordName;
}

ChordName& ChordDiagram::GetChordName()
{
    return m_chordName;
}

/// Determines if a string is valid
/// @param string String to validate
/// @return True if the string is valid, false if not
bool ChordDiagram::IsValidString(uint32_t string) const
{
    return string < GetStringCount();
}

/// Gets the number of strings used in the chord diagram
/// @return The number of strings used in the chord diagram
size_t ChordDiagram::GetStringCount() const
{
    return m_fretNumberArray.size();
}

/// Determines if a top fret value is valid
/// @param topFret Top fret to validate
/// @return True if the top fret value is valid, false if not
bool ChordDiagram::IsValidTopFret(uint8_t topFret)
{
    return topFret <= MAX_TOP_FRET;
}

/// Sets the top fret
/// @param topFret Top fret to set
/// @return True if the top fret was set, false if not
bool ChordDiagram::SetTopFret(uint8_t topFret)
{
    PTB_CHECK_THAT(IsValidTopFret(topFret), false);
    m_topFret = topFret;
    return true;
}

/// Gets the top fret value
/// @return The top fret
uint8_t ChordDiagram::GetTopFret() const
{
    return m_topFret;
}

}
