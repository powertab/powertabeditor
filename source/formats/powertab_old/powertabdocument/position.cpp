/////////////////////////////////////////////////////////////////////////////
// Name:            position.cpp
// Purpose:         Stores and renders a position (a group of notes, or a rest)
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 17, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "position.h"

#include "complexsymbolarray.h"
#include "note.h"
#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Constants
// Default Constants
const uint8_t    Position::DEFAULT_POSITION                              = 0;
const uint8_t    Position::DEFAULT_BEAMING                               = 0;
const uint32_t  Position::DEFAULT_DATA                                  = (uint32_t)(DEFAULT_DURATION_TYPE << 24);
const uint8_t    Position::DEFAULT_DURATION_TYPE                         = 8;

// Position Constants
const uint32_t  Position::MIN_POSITION                                  = 0;
const uint32_t  Position::MAX_POSITION                                  = 255;

// Irregular Grouping Constants
const uint8_t    Position::MIN_IRREGULAR_GROUPING_NOTES_PLAYED           = 2;
const uint8_t    Position::MAX_IRREGULAR_GROUPING_NOTES_PLAYED           = 16;
const uint8_t    Position::MIN_IRREGULAR_GROUPING_NOTES_PLAYED_OVER      = 2;
const uint8_t    Position::MAX_IRREGULAR_GROUPING_NOTES_PLAYED_OVER      = 8;

const uint8_t    Position::MAX_VOLUME_SWELL_DURATION                     = 8;

const uint8_t    Position::MAX_TREMOLO_BAR_DURATION                      = 8;
const uint8_t    Position::MAX_TREMOLO_BAR_PITCH                         = 28;

// Multibar Rest Constants
const uint8_t    Position::MIN_MULTIBAR_REST_MEASURE_COUNT               = 2;
const uint8_t    Position::MAX_MULTIBAR_REST_MEASURE_COUNT               = 255;

/// Default Constructor
Position::Position() :
    m_position(DEFAULT_POSITION), m_beaming(DEFAULT_BEAMING),
    m_data(DEFAULT_DATA)
{
    ComplexSymbols::clearComplexSymbols(m_complexSymbolArray);
}

/// Destructor
Position::~Position()
{
    for (auto &note : m_noteArray)
    {
        delete note;
    }
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Position::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_position << m_beaming << m_data;
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteSmallVector(m_complexSymbolArray);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_noteArray);
    PTB_CHECK_THAT(stream.CheckState(), false);

    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Position::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    stream >> m_position >> m_beaming >> m_data;

    stream.ReadSmallVector(m_complexSymbolArray);
    stream.ReadVector(m_noteArray, version);
    return true;
}

/// Gets the duration type (1 = whole, 2 = half, 4 = quarter, 8 = 8th,
/// 16 = 16th)
/// @return The duration type
uint8_t Position::GetDurationType() const
{
    //------Last Checked------//
    // - Jan 18, 2005
    return ((uint8_t)((m_data & durationTypeMask) >> 24));
}

/// Gets the irregular grouping timing
/// @param notesPlayed Top value for the irregular grouping timing
/// @param notesPlayedOver Bottom value for the irregular grouping timing
/// @return True if the irregular grouping was set, false if not
void Position::GetIrregularGroupingTiming(uint8_t& notesPlayed,
    uint8_t& notesPlayedOver) const
{
    //------Last Checked------//
    // - Jan 18, 2005

    // Values are stored as 1-15 and 1-7, but there is no 1 value
    notesPlayed =
        (uint8_t)(((m_beaming & irregularGroupingNotesPlayedMask) >> 3) + 1);
    notesPlayedOver =
        (uint8_t)((m_beaming & irregularGroupingNotesPlayedOverMask) + 1);
}

/// Determines if the position has an irregular grouping timing
/// @return True if the position has an irregular grouping timing, false if not
bool Position::HasIrregularGroupingTiming() const
{
    //------Last Checked------//
    // - Jan 20, 2005
    uint8_t notesPlayed = 0;
    uint8_t notesPlayedOver = 0;
    GetIrregularGroupingTiming(notesPlayed, notesPlayedOver);
    return (!((notesPlayed == 1) && (notesPlayedOver == 1)));
}

/// Gets the volume swell data (if any)
/// @param startVolume Holds the start volume return value
/// @param endVolume Holds the end volume return value
/// @param duration Holds the duration return value
/// @return True if the data was returned, false if not
bool Position::GetVolumeSwell(uint8_t& startVolume, uint8_t& endVolume,
    uint8_t& duration) const
{
    //------Last Checked------//
    // - Jan 19, 2005

    startVolume = 0;
    endVolume = 0;
    duration = 0;

    // Get the index of the volume swell
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, volumeSwell);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the volume swell
    uint32_t symbolData = m_complexSymbolArray[index];
    startVolume = HIBYTE(LOWORD(symbolData));
    endVolume = LOBYTE(LOWORD(symbolData));
    duration = LOBYTE(HIWORD(symbolData));

    return (true);
}

/// Determines if the position has a volume swell
/// @return True if the position has a volume swell, false if not
bool Position::HasVolumeSwell() const
{
    return (ComplexSymbols::findComplexSymbol(m_complexSymbolArray, volumeSwell) != (uint32_t)-1);
}

/// Gets the tremolo bar data (if any)
/// @param type Holds the type return value
/// @param duration Holds the duration return value
/// @param pitch Holds the pitch return value
/// @return True if the data was returned, false if not
bool Position::GetTremoloBar(uint8_t& type, uint8_t& duration,
    uint8_t& pitch) const
{
    //------Last Checked------//
    // - Jan 19, 2005

    type = 0;
    duration = 0;
    pitch = 0;

    // Get the index of the tremolo bar
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, tremoloBar);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the tremolo bar
    uint32_t symbolData = m_complexSymbolArray[index];
    type = LOBYTE(HIWORD(symbolData));
    duration = HIBYTE(LOWORD(symbolData));
    pitch = LOBYTE(LOWORD(symbolData));

    return (true);
}

/// Determines if the position has a tremolo bar
/// @return True if the position has a tremolo bar, false if not
bool Position::HasTremoloBar() const
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (ComplexSymbols::findComplexSymbol(m_complexSymbolArray, tremoloBar) != (uint32_t)-1);
}

/// Gets the multibar rest data (if any)
/// @param measureCount Holds the measure count return value
/// @return True if the data was returned, false if not
bool Position::GetMultibarRest(uint8_t& measureCount) const
{
    measureCount = 0;

    // Get the index of the multibar rest
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, multibarRest);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the multibar rest
    uint32_t symbolData = m_complexSymbolArray[index];
    measureCount = LOBYTE(LOWORD(symbolData));

    return (true);
}

/// Determines if the position has a multibar rest
/// @return True if the position has a multibar rest, false if not
bool Position::HasMultibarRest() const
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (ComplexSymbols::findComplexSymbol(m_complexSymbolArray, multibarRest) != (uint32_t)-1);
}

}
