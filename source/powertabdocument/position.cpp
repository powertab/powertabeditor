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
#include <stdexcept>
#include <sstream>
#include "position.h"

#include "dynamic.h"
#include "note.h"
#include "powertabfileheader.h"     // Needed for file version constants
#include "tuning.h"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/rational.hpp>

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

#include "complexsymbolarray.h"

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

/// Primary Constructor
/// @param position Zero-based index within the system where the position is
/// anchored
/// @param durationType Duration type to set (1 = whole, 2 = half, 4 = quarter,
/// 8 = 8th, 16 = 16th, 32 = 32nd, 64 = 64th)
/// @param dotCount Number of duration dots to set
Position::Position(uint32_t position, uint8_t durationType, uint8_t dotCount) :
    m_position(position), m_beaming(DEFAULT_BEAMING), m_data(DEFAULT_DATA)
{
    assert(IsValidPosition(position));
    assert(IsValidDurationType(durationType));

    SetDurationType(durationType);
    if (dotCount == 1)
        SetDotted();
    else if (dotCount == 2)
        SetDoubleDotted();

    ComplexSymbols::clearComplexSymbols(m_complexSymbolArray);
}

/// Copy Constructor
Position::Position(const Position& position) :
    m_position(DEFAULT_POSITION), m_beaming(DEFAULT_BEAMING),
    m_data(DEFAULT_DATA)
{
    *this = position;
}

/// Destructor
Position::~Position()
{
    for (uint32_t i = 0; i < m_noteArray.size(); i++)
    {
        delete m_noteArray.at(i);
    }
}

/// Assignment Operator
const Position& Position::operator=(const Position& position)
{
    // Check for assignment to self
    if (this != &position)
    {
        m_position = position.m_position;
        m_beaming = position.m_beaming;
        m_data = position.m_data;

        // copy the complex symbols
        std::copy(position.m_complexSymbolArray.begin(), position.m_complexSymbolArray.end(),
                  m_complexSymbolArray.begin());

        // clean out the current note array
        for (auto i = m_noteArray.begin(); i != m_noteArray.end(); ++i)
            delete *i;

        m_noteArray.clear();

        // clone the note array
        std::transform(position.m_noteArray.begin(), position.m_noteArray.end(),
                       std::back_inserter(m_noteArray),
                       std::mem_fun(&Note::CloneObject));
    }
    return (*this);
}

Position* Position::CloneObject() const
{
    return new Position(*this);
}

struct CompareNotePointers
{
    bool operator()(Note* note1, Note* note2)
    {
        return *note1 == *note2;
    }
};

/// Equality Operator
bool Position::operator==(const Position& position) const
{
    // Compare complex symbols (regardless of the order they appear in)
    std::vector<uint32_t> thisComplexSymbolArray(m_complexSymbolArray.begin(), m_complexSymbolArray.end());
    std::vector<uint32_t> thatComplexSymbolArray(position.m_complexSymbolArray.begin(),
                                                 position.m_complexSymbolArray.end());

    std::sort(thisComplexSymbolArray.begin(), thisComplexSymbolArray.end());
    std::sort(thatComplexSymbolArray.begin(), thatComplexSymbolArray.end());

    if (!(thisComplexSymbolArray == thatComplexSymbolArray))
    {
        return false;
    }

    // compare notes (by value, not by their pointers)
    if (m_noteArray.size() != position.m_noteArray.size())
    {
        return false;
    }

    if (!std::equal(m_noteArray.begin(), m_noteArray.end(),
                    position.m_noteArray.begin(), CompareNotePointers()))
    {
        return false;
    }

    // compare all other data
    return (
        (m_position == position.m_position) &&
        (m_beaming == position.m_beaming) &&
        (m_data == position.m_data)
    );
}

/// Inequality Operator
bool Position::operator!=(const Position& position) const
{
    return (!operator==(position));
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Position::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_position << m_beaming << m_data;
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteSmallVector(m_complexSymbolArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_noteArray);
    CHECK_THAT(stream.CheckState(), false);

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

/// Returns the note duration, including dots, irregular groupings, etc
/// This does not include tempo, and the durations are relative to a quarter note
/// (i.e. A quarter note is 1.0, eighth note is 0.5, etc)
double Position::GetDuration() const
{
    double duration = 4.0 / GetDurationType();

    duration += IsDotted() * 0.5 * duration;
    duration += IsDoubleDotted() * 0.75 * duration;

    // adjust for irregular groupings (triplets, etc)
    if (HasIrregularGroupingTiming())
    {
        uint8_t notesPlayed = 0, notesPlayedOver = 0;
        GetIrregularGroupingTiming(notesPlayed, notesPlayedOver);

        // for example, with triplets we have 3 notes played in the time of 2,
        // so each note is 2/3 of its normal duration
        duration *= static_cast<double>(notesPlayedOver) / static_cast<double>(notesPlayed);
    }

    return duration;
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

/// Returns a text representation of the irregular grouping text (i.e. "3" or "7:5")
std::string Position::GetIrregularGroupingText() const
{
    if (!HasIrregularGroupingTiming())
    {
        return "";
    }

    uint8_t notesPlayed = 0, notesPlayedOver = 0;
    GetIrregularGroupingTiming(notesPlayed, notesPlayedOver);

    std::ostringstream out;
    out << static_cast<int>(notesPlayed);

    const bool isNormalGrouping = (notesPlayedOver == 2 || notesPlayedOver == 4 ||
                                   notesPlayedOver == 8 || notesPlayedOver == 16 ||
                                   notesPlayedOver == 32 || notesPlayedOver == 64);

    // display the ratio if there is an irregular or non-standard ratio
    if (!isNormalGrouping || notesPlayed < notesPlayedOver)
    {
        out << ":" << static_cast<int>(notesPlayedOver);
    }

    return out.str();
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

/// Returns whether the position can be beamed with other notes
/// - The note duration must be an eighth note or less, and the position cannot be a rest
bool Position::IsBeamable() const
{
    return (GetDurationType() >= 8 && !IsRest());
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

bool Position::IsValidVolumeSwell(uint8_t startVolume, uint8_t endVolume, uint8_t duration)
{
    return (
            (Dynamic::IsValidVolume(startVolume) && startVolume != Dynamic::notSet) &&
            (Dynamic::IsValidVolume(endVolume) && endVolume != Dynamic::notSet) &&
            (startVolume != endVolume) && (duration <= MAX_VOLUME_SWELL_DURATION)
            );
}

/// Sets (adds or updates) a volume swell
/// @param startVolume Starting volume of the swell
/// @param endVolume Ending volume of the swell
/// @param duration Duration of the swell  (0 = occurs over position, 1 and up
/// = occurs over next n positions)
/// @return True if the volume swell was added or updated
bool Position::SetVolumeSwell(uint8_t startVolume, uint8_t endVolume,
    uint8_t duration)
{
    CHECK_THAT(IsValidVolumeSwell(startVolume, endVolume, duration), false);

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(MAKEWORD(endVolume, startVolume),
        MAKEWORD(duration, volumeSwell));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Removes a volume swell from the position, if possible
void Position::ClearVolumeSwell()
{
    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, volumeSwell);
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
    CHECK_THAT(IsValidTremoloBar(type, duration, pitch), false);

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(MAKEWORD(pitch, duration),
        MAKEWORD(type, tremoloBar));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Removes a tremolo bar from the position, if possible
void Position::ClearTremoloBar()
{
    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, tremoloBar);
}

/// Returns a text representation of the tremolo bar pitch - e.g. "1 3/4"
std::string Position::GetTremoloBarText() const
{
    using boost::lexical_cast;
    using boost::rational_cast;

    uint8_t type = 0, duration = 0, pitch = 0;
    GetTremoloBar(type, duration, pitch);

    const boost::rational<int> fraction(pitch, 4);

    // whole number
    if (fraction.numerator() % fraction.denominator() == 0)
    {
        return lexical_cast<std::string>(rational_cast<int>(fraction));
    }
    // proper fraction
    else if (fraction.numerator() < fraction.denominator())
    {
        return lexical_cast<std::string>(fraction);
    }
    // convert improper fraction to mixed number
    else
    {
        const int wholeNum = rational_cast<int>(fraction);
        return boost::str(boost::format("%1% %2%") % wholeNum % (fraction - wholeNum));
    }
}

// Multibar Rest Functions
/// Sets (adds or updates) a multibar rest
/// @param measureCount Number of measures to rest for
/// @return True if the multibar rest was added or updated
bool Position::SetMultibarRest(uint8_t measureCount)
{
    CHECK_THAT(IsValidMultibarRest(measureCount), false);

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(MAKEWORD(measureCount, 0),
        MAKEWORD(0, multibarRest));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Removes a multibar rest from the position, if possbile
void Position::ClearMultibarRest()
{
    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, multibarRest);
}

/// Inserts the given note
/// @throw std::logic_error If there is already a note at the same string
void Position::InsertNote(Note* note)
{
    if (GetNoteByString(note->GetString()))
    {
        throw std::logic_error("Cannot insert note - there is already a note at the same string");
    }

    m_noteArray.push_back(note);
}

/// Removes the note at the given string
/// @throw std::logic_error If there is not a note at the given string
void Position::RemoveNote(uint8_t string)
{
    Note* note = GetNoteByString(string);
    if (note == NULL)
    {
        throw std::logic_error("There is no note to remove at the given string");
    }

    delete note;
    // get the iterator to the note, and erase it
    m_noteArray.erase(std::find(m_noteArray.begin(), m_noteArray.end(), note));
}

// functor for the GetNoteByString function
// using c++0x lambdas would be nice, but mingw doesn't seem to like them ...
struct NoteComparison
{
    uint8_t string;

    bool operator()(Note* note)
    {
        return note->GetString() == string;
    }
};

/// Finds the note located at a specific string.
/// @param string The string to search for a note at.
/// @return A pointer to the Note object if it is found, otherwise NULL.
Note* Position::GetNoteByString(uint8_t string) const
{
    NoteComparison compare;
    compare.string = string;

    auto note = std::find_if(m_noteArray.begin(), m_noteArray.end(), compare);
    if (note == m_noteArray.end())
    {
        return NULL;
    }
    else
    {
        return *note;
    }
}

/// Determines if the tab number of a note can be shifted up or down by a string
/// @param note The note that will be shifted
/// @param type Type of shift (up or down)
/// @param numStringsInStaff The number of strings in the current staff
/// @param tuning The tuning of the guitar in the current staff
/// @return True if the note can be shifted, false otherwise.
bool Position::CanShiftTabNumber(Note* note, ShiftType type, uint8_t numStringsInStaff, const Tuning& tuning) const
{
    // check that the note is actually in this position
    CHECK_THAT(std::find(m_noteArray.begin(), m_noteArray.end(), note) != m_noteArray.end(), false);

    const int newStringNum = GetShiftedStringNumber(note, type);

    // check that the string number is valid
    if (!Note::IsValidString(newStringNum) || newStringNum >= numStringsInStaff)
    {
        return false;
    }

    // check if there is already a note at this string
    Note* noteAtString = GetNoteByString(newStringNum);
    if (noteAtString != NULL)
    {
        return false;
    }

    const int newFretNum = GetShiftedFretNumber(note, note->GetString(), newStringNum, tuning);
    return (newFretNum >=0 && Note::IsValidFretNumber(newFretNum));
}

/// Shifts the tab number of a note up or down by a string
/// @param note The note that will be shifted
/// @param type Type of shift (up or down)
/// @param numStringsInStaff The number of strings in the current staff
/// @param tuning The tuning of the guitar in the current staff
/// @return True if the note was successfully shifted, false otherwise.
bool Position::ShiftTabNumber(Note* note, ShiftType type, uint8_t numStringsInStaff, const Tuning& tuning)
{
    CHECK_THAT(CanShiftTabNumber(note, type, numStringsInStaff, tuning), false);

    const int newString = GetShiftedStringNumber(note, type);
    note->SetFretNumber(GetShiftedFretNumber(note, note->GetString(), newString, tuning));
    note->SetString(newString);

    return true;
}

/// Finds the string that the note will be shifted to
/// @param note The note that will be shifted
/// @param type Type of shift
/// @return The new string number that the note will have after applying the shift
int Position::GetShiftedStringNumber(Note* note, ShiftType type) const
{
    const int stringNum = note->GetString();
    int newStringNum = (type == SHIFT_UP) ? stringNum - 1 : stringNum + 1;
    return newStringNum;
}

/// Finds the fret number that the note will be shifted to after changing strings
/// @param note The note that will be shifted
/// @param originalString The original string that the note was on
/// @param newString The string that the note will shift to
/// @param tuning The tuning of the guitar in the current staff
/// @return The new fret number that the note will have after applying the shift
int Position::GetShiftedFretNumber(Note* note, int originalString, int newString, const Tuning& tuning) const
{
    const int originalFret = note->GetFretNumber();

    const int pitchDiff = tuning.GetNote(originalString) - tuning.GetNote(newString);

    return originalFret + pitchDiff;
}

// Checks if at least one of the notes at this position satisfies the given predicate
bool Position::HasNoteWithProperty(bool (Note::*notePropertyPredicate)() const) const
{
    return std::find_if(m_noteArray.begin(), m_noteArray.end(),
                        std::mem_fun(notePropertyPredicate)) != m_noteArray.end();
}

bool Position::HasNoteWithTrill() const
{
    return HasNoteWithProperty(&Note::HasTrill);
}

bool Position::HasNoteWithNaturalHarmonic() const
{
    return HasNoteWithProperty(&Note::IsNaturalHarmonic);
}

bool Position::HasNoteWithArtificialHarmonic() const
{
    return HasNoteWithProperty(&Note::HasArtificialHarmonic);
}

bool Position::HasNoteWithHammeronOrPulloff() const
{
    return HasNoteWithProperty(&Note::HasHammerOnOrPulloff);
}

bool Position::HasNoteWithHammeron() const
{
    return HasNoteWithProperty(&Note::HasHammerOn);
}

bool Position::HasNoteWithHammeronFromNowhere() const
{
    return HasNoteWithProperty(&Note::HasHammerOnFromNowhere);
}

bool Position::HasNoteWithPulloff() const
{
    return HasNoteWithProperty(&Note::HasPullOff);
}

bool Position::HasNoteWithPulloffToNowhere() const
{
    return HasNoteWithProperty(&Note::HasPullOffToNowhere);
}

bool Position::HasNoteWithSlide() const
{
    return HasNoteWithProperty(&Note::HasSlide);
}

bool Position::HasNoteWithTappedHarmonic() const
{
    return HasNoteWithProperty(&Note::HasTappedHarmonic);
}

/// Returns the highest and lowest strings that are used by notes in this position
std::pair<uint8_t, uint8_t> Position::GetStringBounds() const
{
    assert(m_noteArray.size() != 0);

    // create list of string numbers for each position
    std::vector<uint8_t> strings(m_noteArray.size());
    std::transform(m_noteArray.begin(), m_noteArray.end(), strings.begin(),
                   std::mem_fun(&Note::GetString));

    auto lowest = std::min_element(strings.begin(), strings.end());
    auto highest = std::max_element(strings.begin(), strings.end());

    return std::make_pair(*lowest, *highest);
}

// Functor used for sorting notes by their string index
// - derives from std::binary_function so that we can make use of std::not2
//   for sorting in the opposite direction
struct CompareStrings : public std::binary_function<Note*, Note*, bool>
{
    bool operator()(Note* note1, Note* note2) const
    {
        return note1->GetString() < note2->GetString();
    }
};

// Sorts notes from top to bottom
void Position::SortNotesDown()
{
    std::sort(m_noteArray.begin(), m_noteArray.end(), CompareStrings());
}

// Sorts notes from bottom to top
void Position::SortNotesUp()
{
    std::sort(m_noteArray.begin(), m_noteArray.end(), std::not2(CompareStrings()));
}

/// Determines if a complex symbol type is valid
/// @param type Symbol type to validate
/// @return True if the symbol type is valid, false if not
bool Position::IsValidComplexSymbolType(uint8_t type)
{
    return ((type == volumeSwell) || (type == tremoloBar) ||
            (type == multibarRest));
}
