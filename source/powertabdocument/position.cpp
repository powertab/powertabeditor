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

#include <algorithm>
#include "position.h"

#include "powertabfileheader.h"     // Needed for file version constants

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
    //------Last Checked------//
    // - Jan 18, 2005
    ClearComplexSymbolArrayContents();
}

/// Primary Constructor
/// @param position Zero-based index within the system where the position is
/// anchored
/// @param durationType Duration type to set (1 = whole, 2 = half, 4 = quarter,
/// 8 = 8th, 16 = 16th, 32 = 32nd, 64 = 64th)
/// @param dotCount Number of duration dots to set
Position::Position(uint32_t position, uint8_t durationType, uint8_t dotCount) :
    m_position(position), m_beaming(DEFAULT_BEAMING), m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Jan 18, 2005
    assert(IsValidPosition(position));
    assert(IsValidDurationType(durationType));

    SetDurationType(durationType);
    if (dotCount == 1)
        SetDotted();
    else if (dotCount == 2)
        SetDoubleDotted();

    ClearComplexSymbolArrayContents();
}

/// Copy Constructor
Position::Position(const Position& position) :
    m_position(DEFAULT_POSITION), m_beaming(DEFAULT_BEAMING),
    m_data(DEFAULT_DATA)
{
    //------Last Checked------//
    // - Dec 17, 2004
    *this = position;
}

/// Destructor
Position::~Position()
{
    //------Last Checked------//
    // - Jan 18, 2005
    ClearComplexSymbolArrayContents();
    for (uint32_t i = 0; i < m_noteArray.size(); i++)
    {
        delete m_noteArray.at(i);
    }
    m_noteArray.clear();
}

/// Assignment Operator
const Position& Position::operator=(const Position& position)
{
    //------Last Checked------//
    // - Jan 18, 2005

    // Check for assignment to self
    if (this != &position)
    {
        m_position = position.m_position;
        m_beaming = position.m_beaming;
        m_data = position.m_data;

        size_t i = 0;
        for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
            m_complexSymbolArray[i] = position.m_complexSymbolArray[i];

        m_noteArray = position.m_noteArray;
    }
    return (*this);
}

Position* Position::CloneObject() const
{
    Position* newPosition = new Position;
    newPosition->m_position = m_position;
    newPosition->m_beaming = m_beaming;
    newPosition->m_data = m_data;

    size_t i = 0;
    for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
    {
        newPosition->m_complexSymbolArray[i] = m_complexSymbolArray[i];
    }

    newPosition->m_noteArray.clear();

    for (uint32_t i=0; i < m_noteArray.size(); i++)
    {
        newPosition->m_noteArray.push_back(m_noteArray.at(i)->CloneObject());
    }

    return newPosition;
}

/// Equality Operator
bool Position::operator==(const Position& position) const
{
    //------Last Checked------//
    // - Jan 19, 2005

    vector<uint32_t> thisComplexSymbolArray;
    vector<uint32_t> thatComplexSymbolArray;

    uint32_t i = 0;
    for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
    {
        thisComplexSymbolArray.push_back(m_complexSymbolArray[i]);
        thatComplexSymbolArray.push_back(position.m_complexSymbolArray[i]);
    }

    std::sort(thisComplexSymbolArray.begin(), thisComplexSymbolArray.end());
    std::sort(thatComplexSymbolArray.begin(), thatComplexSymbolArray.end());

    i = 0;
    for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
    {
        if (thisComplexSymbolArray[i] != thatComplexSymbolArray[i])
            return (false);
    }

    return (
        (m_position == position.m_position) &&
        (m_beaming == position.m_beaming) &&
        (m_data == position.m_data) &&
        (m_noteArray == position.m_noteArray)
    );
}

/// Inequality Operator
bool Position::operator!=(const Position& position) const
{
    //------Last Checked------//
    // - Dec 17, 2004
    return (!operator==(position));
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Position::Serialize(PowerTabOutputStream& stream)
{
    //------Last Checked------//
    // - Jan 17, 2005
    stream << m_position << m_beaming << m_data;
    CHECK_THAT(stream.CheckState(), false);

    uint8_t complexCount = GetComplexSymbolCount();
    stream << complexCount;
    CHECK_THAT(stream.CheckState(), false);

    size_t i = 0;
    for (; i < complexCount; i++)
    {
        stream << m_complexSymbolArray[i];
        CHECK_THAT(stream.CheckState(), false);
    }

    stream.WriteVector(m_noteArray);
    CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Position::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    //------Last Checked------//
    // - Jan 17, 2005

    // Version 1.0/1.0.2 beaming updated
    if (version == PowerTabFileHeader::FILEVERSION_1_0 ||
        version == PowerTabFileHeader::FILEVERSION_1_0_2)
    {
        stream >> m_position >> m_beaming >> m_data;
        CHECK_THAT(stream.CheckState(), false);

        // All we have to do is move the irregular flags to the duration
        // variable, the document will be rebeamed in the CDocument::Serialize()
        if ((m_beaming & 0x2000) == 0x2000)
            m_data |= irregularGroupingStart;
        if ((m_beaming & 0x4000) == 0x4000)
            m_data |= irregularGroupingMiddle;
        if ((m_beaming & 0x8000) == 0x8000)
            m_data |= irregularGroupingEnd;

        uint8_t complexCount;
        stream >> complexCount;
        CHECK_THAT(stream.CheckState(), false);

        // Read the symbols
        size_t i = 0;
        for (; i < complexCount; i++)
        {
            stream >> m_complexSymbolArray[i];
            CHECK_THAT(stream.CheckState(), false);
        }

        stream.ReadVector(m_noteArray, version);
        CHECK_THAT(stream.CheckState(), false);
    }
    // Version 1.5 and up
    else
    {
        stream >> m_position >> m_beaming >> m_data;
        CHECK_THAT(stream.CheckState(), false);

        uint8_t complexCount;
        stream >> complexCount;
        CHECK_THAT(stream.CheckState(), false);

        // Read the symbols
        size_t i = 0;
        for (; i < complexCount; i++)
        {
            stream >> m_complexSymbolArray[i];
            CHECK_THAT(stream.CheckState(), false);
        }

        stream.ReadVector(m_noteArray, version);
        CHECK_THAT(stream.CheckState(), false);
    }

    return (stream.CheckState());
}

// Duration Type Functions
/// Sets the duration type
/// @param durationType Duration type to set (1 = whole, 2 = half, 4 = quarter,
/// 8 = 8th, 16 = 16th, 32 = 32nd, 64 = 64th)
/// @return True if the duration type was set, false if not
bool Position::SetDurationType(uint8_t durationType)
{
    //------Last Checked------//
    // - Jan 18, 2005
    CHECK_THAT(IsValidDurationType(durationType), false);

    // Duration type is stored in power of two format
    m_data &= ~durationTypeMask;
    m_data |= (uint32_t)(durationType << 24);

    return (true);
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

// Irregular Grouping Functions
/// Sets the irregular grouping timing
/// @param notesPlayed Number of notes played
/// @param notesPlayedOver Number of notes played over
/// @return True if the irregular grouping timing was set, false if not
bool Position::SetIrregularGroupingTiming(uint8_t notesPlayed,
    uint8_t notesPlayedOver)
{
    //------Last Checked------//
    // - Jan 18, 2005
    CHECK_THAT(IsValidIrregularGroupingTiming(notesPlayed, notesPlayedOver),
        false);

    // Values are stored as 1-15 and 1-7
    notesPlayed--;
    notesPlayedOver--;

    m_beaming &= ~irregularGroupingTimingMask;

    m_beaming |= (uint16_t)(notesPlayed << 3);
    m_beaming |= (uint16_t)notesPlayedOver;

    return (true);
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

/// Clears the irregular grouping timing
/// @return True if the irregular grouping was cleared, false if not
bool Position::ClearIrregularGroupingTiming()
{
    //------Last Checked------//
    // - Jan 20, 2005
    m_beaming &= ~irregularGroupingTimingMask;
    return (true);
}

// Previous Beam Duration Functions
/// Sets the duration type of the previous rhythm slash in the beam group (cache
/// only)
/// @param durationType Duration type to set (0 = not beamed, 8 = 8th,
/// 16 = 16th, 32 = 32nd, 64 = 64th)
/// @return True if the duration type was set, false if not
bool Position::SetPreviousBeamDurationType(uint8_t durationType)
{
    //------Last Checked------//
    // - Jan 18, 2005
    CHECK_THAT(IsValidPreviousBeamDurationType(durationType), false);

    // Clear the current duration type
    m_beaming &= ~previousBeamDurationTypeMask;

    uint16_t flag = 0;

    if (durationType == 8)
        flag = previousBeamDuration8th;
    else if (durationType == 16)
        flag = previousBeamDuration16th;
    else if (durationType == 32)
        flag = previousBeamDuration32nd;
    else if (durationType == 64)
        flag = previousBeamDuration64th;

    m_beaming |= (uint16_t)(flag << 7);

    return (true);
}

/// Gets the duration type of the previous rhythm slash in the beam group
/// @return The duration type of the previous rhythm slash in the beam group
/// (0 = not beamed, 8 = 8th, 16 = 16th)
uint8_t Position::GetPreviousBeamDurationType() const
{
    //------Last Checked------//
    // - Jan 18, 2005

    uint8_t flag = (uint8_t)((m_beaming & previousBeamDurationTypeMask) >> 7);
    if (flag == previousBeamDuration8th)
        return (8);
    else if (flag == previousBeamDuration16th)
        return (16);
    else if (flag == previousBeamDuration32nd)
        return (32);
    else if (flag == previousBeamDuration64th)
        return (64);

    return (0);
}

// Beaming Functions
/// Sets a beaming flag
/// @param flag Flag to set
/// @return True if teh beaming flag was set, false if not
bool Position::SetBeamingFlag(uint16_t flag)
{
    //------Last Checked------//
    // - Jan 7, 2005
    CHECK_THAT(IsValidBeamingFlag(flag), false);

    // Mutually exclusive operations
    if ((flag & beamStart) == beamStart)
    {
        ClearBeamingFlag(beamEnd);
        flag &= ~beamEnd;
    }
    else if ((flag & beamEnd) == beamEnd)
        ClearBeamingFlag(beamStart);

    m_beaming |= flag;

    return (true);
}

// Data Flag Functions
/// Sets a data flag
/// @param flag Flag to set
/// @return True if the flag was set, false if not
bool Position::SetDataFlag(uint32_t flag)
{
    //------Last Checked------//
    // - Jan 7, 2005
    CHECK_THAT(IsValidDataFlag(flag), false);

    // Mutually exclusive operations
    if ((flag & dottedMask) != 0)
        ClearDataFlag(dottedMask);

    if ((flag & vibratoMask) != 0)
        ClearDataFlag(vibratoMask);

    if ((flag & arpeggioMask) != 0)
        ClearDataFlag(arpeggioMask);

    if ((flag & pickStrokeMask) != 0)
        ClearDataFlag(pickStrokeMask);

    if ((flag & accentMask) != 0)
        ClearDataFlag(accentMask);

    if ((flag & tripletFeelMask) != 0)
        ClearDataFlag(tripletFeelMask);

    m_data |= flag;

    return (true);
}

// Volume Swell Functions
/// Sets (adds or updates) a volume swell
/// @param startVolume Starting volume of the swell
/// @param endVolume Ending volume of the swell
/// @param duration Duration of the swell  (0 = occurs over position, 1 and up
/// = occurs over next n positions)
/// @return True if the volume swell was added or updated
bool Position::SetVolumeSwell(uint8_t startVolume, uint8_t endVolume,
    uint8_t duration)
{
    //------Last Checked------//
    // - Jan 19, 2005
    CHECK_THAT(IsValidVolumeSwell(startVolume, endVolume, duration), false);

    // Construct the symbol data, then add it to the array
    uint32_t symbolData = MAKELONG(MAKEWORD(endVolume, startVolume),
        MAKEWORD(duration, volumeSwell));
    return (AddComplexSymbol(symbolData));
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
    uint32_t index = FindComplexSymbol(volumeSwell);
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
    //------Last Checked------//
    // - Jan 19, 2005
    return (FindComplexSymbol(volumeSwell) != (uint32_t)-1);
}

/// Removes a volume swell from the position
/// @return True if the volume swell was removed, false if not
bool Position::ClearVolumeSwell()
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (RemoveComplexSymbol(volumeSwell));
}

// Tremolo Bar Functions
/// Sets (adds or updates) a tremolo bar
/// @param type Type of tremolo bar (see tremoloBarTypes enum for values)
/// @param duration Duration of the tremolo bar (0 = occurs over position, 1
/// and up = occurs over next n positions)
/// @param pitch Pitch of the tremolo bar
/// @return True if the tremolo bar was added or updated
bool Position::SetTremoloBar(uint8_t type, uint8_t duration, uint8_t pitch)
{
    //------Last Checked------//
    // - Jan 19, 2005
    CHECK_THAT(IsValidTremoloBar(type, duration, pitch), false);

    // Construct the symbol data, then add it to the array
    uint32_t symbolData = MAKELONG(MAKEWORD(pitch, duration),
        MAKEWORD(type, tremoloBar));
    return (AddComplexSymbol(symbolData));
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
    uint32_t index = FindComplexSymbol(tremoloBar);
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
    return (FindComplexSymbol(tremoloBar) != (uint32_t)-1);
}

/// Removes a tremolo bar from the position
/// @return True if the tremolo bar was removed, false if not
bool Position::ClearTremoloBar()
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (RemoveComplexSymbol(tremoloBar));
}

// Multibar Rest Functions
/// Sets (adds or updates) a multibar rest
/// @param measureCount Number of measures to rest for
/// @return True if the multibar rest was added or updated
bool Position::SetMultibarRest(uint8_t measureCount)
{
    //------Last Checked------//
    // - Jan 19, 2005
    CHECK_THAT(IsValidMultibarRest(measureCount), false);

    // Construct the symbol data, then add it to the array
    uint32_t symbolData = MAKELONG(MAKEWORD(measureCount, 0),
        MAKEWORD(0, multibarRest));
    return (AddComplexSymbol(symbolData));
}

/// Gets the multibar rest data (if any)
/// @param measureCount Holds the measure count return value
/// @return True if the data was returned, false if not
bool Position::GetMultibarRest(uint8_t& measureCount) const
{
    //------Last Checked------//
    // - Jan 19, 2005

    measureCount = 0;

    // Get the index of the multibar rest
    uint32_t index = FindComplexSymbol(multibarRest);
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
    return (FindComplexSymbol(multibarRest) != (uint32_t)-1);
}

/// Removes a multibar rest from the position
/// @return True if the multibar rest was removed, false if not
bool Position::ClearMultibarRest()
{
    //------Last Checked------//
    // - Jan 19, 2005
    return (RemoveComplexSymbol(multibarRest));
}

// Complex Symbol Array Functions
/// Adds a complex symbol to the complex symbol array
/// @param symbolData Data that makes up the symbol
/// @return True if the symbol was added or updated, false if not
bool Position::AddComplexSymbol(uint32_t symbolData)
{
    //------Last Checked------//
    // - Jan 19, 2005

    // Get and validate the symbol type
    uint8_t type = HIBYTE(HIWORD(symbolData));
    CHECK_THAT(IsValidComplexSymbolType(type), false);

    bool returnValue = false;

    // Get the index in the complex array where the symbol is stored
    uint32_t index = FindComplexSymbol(type);

    // Found symbol in the array, update the symbol data
    if (index != (uint32_t)-1)
    {
        m_complexSymbolArray[index] = symbolData;
        returnValue = true;
    }
    // Symbol was not found in the array, find the first free array slot and
    // insert there
    else
    {
        uint32_t i = 0;
        for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
        {
            if (m_complexSymbolArray[i] == notUsed)
            {
                m_complexSymbolArray[i] = symbolData;
                returnValue = true;
                break;
            }
        }
    }

    return (returnValue);
}

/// Gets the number of complex symbols used by the position
/// @return The number of complex symbols used by the position
size_t Position::GetComplexSymbolCount() const
{
    //------Last Checked------//
    // - Jan 19, 2005
    size_t returnValue = 0;
    size_t i = 0;
    for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
    {
        // Slot is not used; break out
        if (m_complexSymbolArray[i] == notUsed)
            break;
        returnValue++;
    }
    return (returnValue);
}

/// Gets the index of a given complex symbol type in the complex symbol array
/// @param type Type of symbol to find
/// @return Index within the array where the symbol was found, or -1 if not
/// found
uint32_t Position::FindComplexSymbol(uint8_t type) const
{
    //------Last Checked------//
    // - Jan 19, 2005
    uint32_t returnValue = (uint32_t)-1;

    uint32_t i = 0;
    for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
    {
        // Found the symbol type; break out
        if (HIBYTE(HIWORD(m_complexSymbolArray[i])) == type)
        {
            returnValue = i;
            break;
        }
    }

    return (returnValue);
}

/// Removes a complex symbol from the complex symbol array
/// @param type Type of symbol to remove
/// @return True if the symbol was removed, false if not
bool Position::RemoveComplexSymbol(uint8_t type)
{
    //------Last Checked------//
    // - Jan 19, 2005
    bool returnValue = false;

    uint32_t i = 0;
    for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
    {
        if (HIBYTE(HIWORD(m_complexSymbolArray[i])) == type)
        {
            m_complexSymbolArray[i] = notUsed;
            returnValue = true;
            break;
        }
    }

    return (returnValue);
}

/// Clears the contents of the symbol array (sets all elements to "not used")
void Position::ClearComplexSymbolArrayContents()
{
    //------Last Checked------//
    // - Jan 19, 2005
    uint32_t i = 0;
    for (; i < MAX_POSITION_COMPLEX_SYMBOLS; i++)
        m_complexSymbolArray[i] = notUsed;
}
