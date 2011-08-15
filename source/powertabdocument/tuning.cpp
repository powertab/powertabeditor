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

#include <algorithm>
#include <sstream>

#include "tuning.h"
#include "generalmidi.h"

#include "powertabinputstream.h"
#include "powertaboutputstream.h"

using std::pair;
using std::string;

const string    Tuning::DEFAULT_NAME                    = "";
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

/// Primary Constructor
/// @param name Tuning name (Standard, Dropped-D, Open G, etc.)
/// @param musicNotationOffset Offset used when drawing notes on the music
/// staff, in semi-tones
/// @param sharps Display the tuning notes using sharps as opposed to flats
/// @param tuningNotes A list of the MIDI note values for each string (high to low)
Tuning::Tuning(const std::string& name, int8_t musicNotationOffset, bool sharps,
               const std::vector<uint8_t> &tuningNotes) :
    m_name(name)
{
    assert(IsValidMusicNotationOffset(musicNotationOffset));

    SetMusicNotationOffset(musicNotationOffset);
    SetSharps(sharps);
    SetTuningNotes(tuningNotes);
}

Tuning::Tuning(const Tuning& tuning) :
    m_name(DEFAULT_NAME), m_data(DEFAULT_DATA)
{
    *this = tuning;
}

/// Equality operator
bool Tuning::operator==(const Tuning& tuning) const
{
    return (m_noteArray == tuning.m_noteArray &&
            m_name == tuning.m_name &&
            m_data == tuning.m_data);
}

/// Inequality Operator
bool Tuning::operator!=(const Tuning& tuning) const
{
    return (!operator==(tuning));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Tuning::Serialize(PowerTabOutputStream& stream) const
{
    stream.WriteMFCString(m_name);
    CHECK_THAT(stream.CheckState(), false);

    stream << m_data;
    CHECK_THAT(stream.CheckState(), false);

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

/// Determines if the tuning notes are the same as that of another Tuning object
/// @param tuning Tuning object to compare with
/// @return True if the tunings have the same notes, false if not
bool Tuning::IsSameTuning(const Tuning& tuning) const
{
    return m_noteArray == tuning.m_noteArray;
}

// Sharps Functions
/// Sets the tuning to use sharps when displaying the tuning notes (i.e. C# vs
/// Db)
/// @param set True uses use sharps, false uses flats
void Tuning::SetSharps(bool set)
{
    m_data &= ~sharpsMask;
    if (set)
        m_data |= sharpsMask;
}

// Music Notation Offset Functions
/// Sets the music notation offset
/// @param musicNotationOffset Music notation offset to set, in semi-tones
/// @return True if the music notation offset was set, false if not
bool Tuning::SetMusicNotationOffset(int8_t musicNotationOffset)
{
    CHECK_THAT(IsValidMusicNotationOffset(musicNotationOffset), false);

    // Clear the current music notation offset
    m_data &= ~musicNotationOffsetMask;

    // Set the sign bit if the offset is negative
    if (musicNotationOffset < 0)
        m_data |= musicNotationOffsetSignMask;

    // Set the music notation offset value
    m_data |= (uint8_t)(abs(musicNotationOffset) << 1);

    return (true);
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

// Note Functions
/// Sets the MIDI note value for an existing string in the tuning
/// @param string String to set the note for
/// @param note MIDI note to set
/// @return True if the note was set, false if not
bool Tuning::SetNote(uint32_t string, uint8_t note)
{
    CHECK_THAT(IsValidString(string), false);
    CHECK_THAT(midi::IsValidMidiNote(note), false);
    m_noteArray[string] = note;
    return true;
}

/// Gets the note for assigned to a string
/// @param string String to get the note for
/// @param includeMusicNotationOffset Include the music notation offset in the
/// note pitch
/// @return The MIDI note assigned to the string
uint8_t Tuning::GetNote(uint32_t string, bool includeMusicNotationOffset) const
{
    CHECK_THAT(IsValidString(string), midi::MIDI_NOTE_MIDDLE_C);

    uint8_t returnValue = m_noteArray[string];

    // Include the music notation offset
    if (includeMusicNotationOffset)
        returnValue = midi::OffsetMidiNote(returnValue, GetMusicNotationOffset());

    return (returnValue);
}

/// Gets the text representation of a note (i.e. C#, Eb)
/// @param string String of the note to get the text representation of
/// @return A text representation of the note
string Tuning::GetNoteText(uint32_t string) const
{
    CHECK_THAT(IsValidString(string), "");
    return (midi::GetMidiNoteText(m_noteArray[string], UsesSharps()));
}

/// Determines if a note is a note that can be played as an open string
/// @param note Note to test
/// @return True if the note can be played as an open string, false if not
bool Tuning::IsOpenStringNote(uint8_t note) const
{
    return (std::find(m_noteArray.begin(), m_noteArray.end(), note) != m_noteArray.end());
}

/// Gets the MIDI note range for the tuning (lowest possible playable note +
/// highest possible playable note)
/// @param capo Capo value to apply to the tuning
/// @return A pair that contains the lowest possible note and the highest
/// possible note
pair<uint8_t, uint8_t> Tuning::GetNoteRange(uint8_t capo) const
{
    // find iterators to min & max elements
    std::vector<uint8_t>::const_iterator max = std::max_element(m_noteArray.begin(), m_noteArray.end());
    std::vector<uint8_t>::const_iterator min = std::min_element(m_noteArray.begin(), m_noteArray.end());

    // add capo, and 24 frets for the highest note
    pair<uint8_t, uint8_t> returnValue = std::make_pair(*min + capo, *max + capo + 24);
    return returnValue;
}

/// Sets the tuning notes, from high to low
/// @return False if the notes were invalid (invalid MIDI notes, or the number of strings was invalid)
bool Tuning::SetTuningNotes(const std::vector<uint8_t>& tuningNotes)
{
    if (!IsValidStringCount(tuningNotes.size()))
    {
        return false;
    }

    // check for invalid midi notes
    if (std::find_if(tuningNotes.begin(), tuningNotes.end(),
                     std::not1(std::ptr_fun(&midi::IsValidMidiNote))) != tuningNotes.end())
    {
        return false;
    }

    m_noteArray = tuningNotes;

    return true;
}

/// Gets a full string representation of the tuning from low to high
/// (i.e. E A D G B E)
string Tuning::GetSpelling() const
{
    std::stringstream returnValue;

    const size_t stringCount = GetStringCount();

    // Go from lowest to highest string
    for (size_t i = stringCount; i > 0; i--)
    {
        if (i != stringCount)
           returnValue << " ";

        returnValue << GetNoteText(i - 1);
    }

    return returnValue.str();
}

/// Sets the tuning to standard EADGBE tuning
void Tuning::SetToStandard()
{
    using namespace midi;

    std::vector<uint8_t> notes;
    notes.push_back(MIDI_NOTE_E4);
    notes.push_back(MIDI_NOTE_B3);
    notes.push_back(MIDI_NOTE_G3);
    notes.push_back(MIDI_NOTE_D3);
    notes.push_back(MIDI_NOTE_A2);
    notes.push_back(MIDI_NOTE_E2);

    SetTuningNotes(notes);
}

/// Determines if a tuning is valid (has a valid number of strings)
/// @return True if the tuning is valid, false if not
bool Tuning::IsValid() const
{
    return IsValidStringCount(GetStringCount());
}

