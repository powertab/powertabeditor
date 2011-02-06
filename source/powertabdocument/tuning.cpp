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

#include <algorithm> // std::min and std::max
#include <sstream>

#include "tuning.h"
#include "generalmidi.h"

using std::pair;
using std::string;

const string    Tuning::DEFAULT_NAME                    = "";
const uint8_t   Tuning::DEFAULT_DATA                    = 0;
const int8_t    Tuning::MIN_MUSIC_NOTATION_OFFSET       = -12;
const int8_t    Tuning::MAX_MUSIC_NOTATION_OFFSET       = 12;
const uint32_t  Tuning::MIN_STRING_COUNT                = 3;
const uint32_t  Tuning::MAX_STRING_COUNT                = 7;

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
/// @param note1 MIDI note value for the 1st string in the tuning (for standard
/// guitar tuning, this is the high E string)
/// @param note2 MIDI note value for the 2nd string in the tuning
/// @param note3 MIDI note value for the 3rd string in the tuning
/// @param note4 MIDI note value for the 4th string in the tuning
/// @param note5 MIDI note value for the 5th string in the tuning
/// @param note6 MIDI note value for the 6th string in the tuning
/// @param note7 MIDI note value for the 7th string in the tuning
Tuning::Tuning(const char* name, int8_t musicNotationOffset, bool sharps,
    uint8_t note1, uint8_t note2, uint8_t note3, uint8_t note4, uint8_t note5,
    uint8_t note6, uint8_t note7) : m_name(name), m_data(DEFAULT_DATA)
{
    assert(name != NULL);
    assert(IsValidMusicNotationOffset(musicNotationOffset));

    SetSharps(sharps);
    SetMusicNotationOffset(musicNotationOffset);
    AddTuningNotes(note1, note2, note3, note4, note5, note6, note7);
}

/// Copy Constructor
Tuning::Tuning(const Tuning& tuning) :
    m_name(DEFAULT_NAME), m_data(DEFAULT_DATA)
{
    *this = tuning;
}

/// Destructor
Tuning::~Tuning()
{
}

// Operators
/// Assignment operator
const Tuning& Tuning::operator=(const Tuning& tuning)
{
    // Check for assignment to self
    if (this != &tuning)
    {
        m_name = tuning.m_name;
        m_data = tuning.m_data;
        DeleteNoteArrayContents();
        size_t i = 0;
        size_t count = tuning.m_noteArray.size();
        for (; i < count; i++)
            m_noteArray.push_back(tuning.m_noteArray[i]);
    }
    return (*this);
}

/// Equality operator
bool Tuning::operator==(const Tuning& tuning) const
{
    size_t thisStringCount = GetStringCount();
    size_t otherStringCount = tuning.GetStringCount();

    // Tunings have a differing number of strings
    if (thisStringCount != otherStringCount)
        return (false);

    // Check each string for matching notes
    size_t i = 0;
    for (; i < thisStringCount; i++)
    {
        if (m_noteArray[i] != tuning.m_noteArray[i])
            return (false);
    }

    return (
        (m_name == tuning.m_name) &&
        (m_data == tuning.m_data)
    );
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
bool Tuning::Serialize(PowerTabOutputStream& stream)
{
    stream.WriteMFCString(m_name);
    CHECK_THAT(stream.CheckState(), false);

    stream << m_data;
    CHECK_THAT(stream.CheckState(), false);

    // Write the string count as a byte
    size_t stringCount = GetStringCount();
    stream << (uint8_t)stringCount;
    CHECK_THAT(stream.CheckState(), false);

    // Write each note
    size_t i = 0;
    for (; i < stringCount; i++)
    {
        stream << m_noteArray[i];
        CHECK_THAT(stream.CheckState(), false);
    }
    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Tuning::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    UNUSED(version);

    stream.ReadMFCString(m_name);
    CHECK_THAT(stream.CheckState(), false);

    stream >> m_data;
    CHECK_THAT(stream.CheckState(), false);

    DeleteNoteArrayContents();

    // Get number of notes, then the notes themselves
    uint8_t stringCount = 0;
    stream >> stringCount;
    CHECK_THAT(stream.CheckState(), false);

    size_t i = 0;
    for (; i < stringCount; i++)
    {
        uint8_t note;
        stream >> note;
        CHECK_THAT(stream.CheckState(), false);
        m_noteArray.push_back(note);
    }
    return (stream.CheckState());
}

// Tuning Functions
/// Sets the contents of the Tuning object
/// @param name Tuning name (Standard, Dropped-D, Open G, etc.)
/// @param musicNotationOffset Offset used when drawing notes on the music
/// staff, in semi-tones
/// @param sharps Display the tuning notes using sharps as opposed to flats
/// @param note1 MIDI note value for the 1st string in the tuning (for standard
/// guitar tuning, this is the high E string)
/// @param note2 MIDI note value for the 2nd string in the tuning
/// @param note3 MIDI note value for the 3rd string in the tuning
/// @param note4 MIDI note value for the 4th string in the tuning
/// @param note5 MIDI note value for the 5th string in the tuning
/// @param note6 MIDI note value for the 6th string in the tuning
/// @param note7 MIDI note value for the 7th string in the tuning
/// @return True if the tuning was set, false if not
bool Tuning::SetTuning(const char* name, int8_t musicNotationOffset,
    bool sharps, uint8_t note1, uint8_t note2, uint8_t note3, uint8_t note4,
    uint8_t note5, uint8_t note6, uint8_t note7)
{
    if (!SetName(name))
        return (false);
    SetSharps(sharps);
    if (!SetMusicNotationOffset(musicNotationOffset))
        return (false);
    return (AddTuningNotes(note1, note2, note3, note4, note5, note6, note7));
}

/// Determines if the tuning notes are the same as that of another Tuning object
/// @param tuning Tuning object to compare with
/// @return True if the tunings have the same notes, false if not
bool Tuning::IsSameTuning(const Tuning& tuning) const
{
    size_t thisStringCount = GetStringCount();
    size_t otherStringCount = tuning.GetStringCount();

    // Tunings have a different number of strings
    if (thisStringCount != otherStringCount)
        return (false);

    // Check each string for matching notes
    size_t i = 0;
    for (; i < thisStringCount; i++)
    {
        if (m_noteArray[i] != tuning.m_noteArray[i])
            return (false);
    }

    return (true);
}

/// Determines if the tuning notes are the same
/// @param note1 MIDI note value for the 1st string in the tuning (for standard
/// guitar tuning, this is the high E string)
/// @param note2 MIDI note value for the 2nd string in the tuning
/// @param note3 MIDI note value for the 3rd string in the tuning
/// @param note4 MIDI note value for the 4th string in the tuning
/// @param note5 MIDI note value for the 5th string in the tuning
/// @param note6 MIDI note value for the 6th string in the tuning
/// @param note7 MIDI note value for the 7th string in the tuning
/// @return True if all of the tuning notes match, false if not
bool Tuning::IsSameTuning(uint8_t note1, uint8_t note2, uint8_t note3,
    uint8_t note4, uint8_t note5, uint8_t note6, uint8_t note7) const
{
    Tuning temp;
    temp.AddTuningNotes(note1, note2, note3, note4, note5, note6, note7);
    return (IsSameTuning(temp));
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
    return (true);
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
    //------Last Checked------//
    // - Dec 14, 2004
    CHECK_THAT(IsValidString(string), "");
    return (midi::GetMidiNoteText(m_noteArray[string], UsesSharps()));
}

/// Determines if a note is a note that can be played as an open string
/// @param note Note to test
/// @return True if the note can be played as an open string, false if not
bool Tuning::IsOpenStringNote(uint8_t note) const
{
    //------Last Checked------//
    // - Dec 14, 2004
    int32_t i = 0;
    int32_t nCount = GetStringCount();
    for (; i < nCount; i++)
    {
        if (m_noteArray[i] != notUsed)
        {
            // Note matches a tuning note; it's an open string note
            if (m_noteArray[i] == note)
                return (true);
        }
    }
    return (false);
}

/// Gets the MIDI note range for the tuning (lowest possible playable note +
/// highest possible playable note)
/// @param capo Capo value to apply to the tuning
/// @return A pair that contains the lowest possible note and the highest
/// possible note
pair<int, int> Tuning::GetNoteRange(uint8_t capo) const
{
    pair<int, int> returnValue(notUsed, 0);

    // Loop through each tuning note and get the minimum and maximum notes for
    // the string
    size_t i = 0;
    size_t stringCount = GetStringCount();
    for (; i < stringCount; i++)
    {
        if (m_noteArray[i] != notUsed)
        {
            returnValue.first = std::min(returnValue.first, m_noteArray[i] + capo);
            returnValue.second = std::max(returnValue.second, m_noteArray[i] + capo + 24);       // Assume guitar has 24 frets
        }
    }
    return (returnValue);
}

/// Sets the tuning notes for the tuning
/// @param note1 MIDI note value for the 1st string in the tuning (for standard
/// guitar tuning, this is the high E string)
/// @param note2 MIDI note value for the 2nd string in the tuning
/// @param note3 MIDI note value for the 3rd string in the tuning
/// @param note4 MIDI note value for the 4th string in the tuning
/// @param note5 MIDI note value for the 5th string in the tuning
/// @param note6 MIDI note value for the 6th string in the tuning
/// @param note7 MIDI note value for the 7th string in the tuning
/// @return True if the tuning notes were set, false if not
bool Tuning::AddTuningNotes(uint8_t note1, uint8_t note2, uint8_t note3,
    uint8_t note4, uint8_t note5, uint8_t note6, uint8_t note7)
{
    using midi::IsValidMidiNote;

    CHECK_THAT(IsValidMidiNote(note1), false);
    CHECK_THAT(IsValidMidiNote(note2), false);
    CHECK_THAT(IsValidMidiNote(note3), false);
    CHECK_THAT(IsValidMidiNote(note4) || (note4 == notUsed), false);
    CHECK_THAT(IsValidMidiNote(note5) || (note5 == notUsed), false);
    CHECK_THAT(IsValidMidiNote(note6) || (note6 == notUsed), false);
    CHECK_THAT(IsValidMidiNote(note7) || (note7 == notUsed), false);

    DeleteNoteArrayContents();

    m_noteArray.push_back(note1);
    m_noteArray.push_back(note2);
    m_noteArray.push_back(note3);

    if (note4 == notUsed)
        return (true);
    m_noteArray.push_back(note4);

    if (note5 == notUsed)
        return (true);
    m_noteArray.push_back(note5);

    if (note6 == notUsed)
        return (true);
    m_noteArray.push_back(note6);

    if (note7 == notUsed)
        return (true);
    m_noteArray.push_back(note7);

    return (true);
}

/// Deletes the contents (and frees the memory) of the note array
void Tuning::DeleteNoteArrayContents()
{
    m_noteArray.clear();
}

// Operations
/// Gets a full string representation of the tuning from low to high
/// (i.e. E A D G B E)
/// @return A full string representation of the tuning from low to high
string Tuning::GetSpelling() const
{
    std::stringstream returnValue;

    size_t stringCount = GetStringCount();

    // Go from lowest to highest string
    size_t i = stringCount;
    for (; i > 0; i--)
    {
        if (i != stringCount)
           returnValue << " ";
        returnValue << GetNoteText(i - 1);
    }

    return returnValue.str();
}
