/////////////////////////////////////////////////////////////////////////////
// Name:            note.cpp
// Purpose:         Stores and renders a note
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 17, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "note.h"

#include "complexsymbolarray.h"
#include "powertabinputstream.h"
#include "powertaboutputstream.h"

namespace PowerTabDocument {

// Default Constants
const uint8_t Note::DEFAULT_STRING_DATA      = 0;
const uint16_t Note::DEFAULT_SIMPLE_DATA      = 0;

// String Constants
const uint8_t Note::MIN_STRING               = 0;
const uint8_t Note::MAX_STRING               = 6;

// Fret Number Constants
const uint8_t Note::MIN_FRET_NUMBER          = 0;
const uint8_t Note::MAX_FRET_NUMBER          = 29;

// Bend Constants
const uint8_t Note::MAX_BEND_PITCH           = 12;
const uint8_t Note::MAX_BEND_DURATION        = 9;

// Constructor/Destructor
/// Default Constructor
Note::Note() :
    m_stringData(DEFAULT_STRING_DATA),
    m_simpleData(DEFAULT_SIMPLE_DATA)
{
    ComplexSymbols::clearComplexSymbols(m_complexSymbolArray);
}

// Serialization Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool Note::Serialize(PowerTabOutputStream& stream) const
{
    stream << m_stringData << m_simpleData;
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteSmallVector(m_complexSymbolArray);
    return stream.CheckState();
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool Note::Deserialize(PowerTabInputStream& stream, uint16_t)
{
    stream >> m_stringData >> m_simpleData;

    stream.ReadSmallVector(m_complexSymbolArray);
    return true;
}

/// Gets the string for the note
/// @return The string for the note
uint32_t Note::GetString() const
{
    return (uint8_t)((m_stringData & stringMask) >> 5);
}

/// Gets the fret number on a given string
/// @return The fret number on the string
uint8_t Note::GetFretNumber() const
{
    return ((uint8_t)(m_stringData & fretNumberMask));
}

// Simple Flag Functions
/// Determines if a simple flag is valid
/// @param flag Flag to validate
/// @return True if the flag is valid, false if not
bool Note::IsValidSimpleFlag(uint16_t flag)
{
    return (((flag & simpleFlagsMask) != 0) &&
            ((flag & ~simpleFlagsMask) == 0));
}

/// Determines if a simple flag is set
/// @param flag Flag to test
/// @return True if the flag is set, false if not
bool Note::IsSimpleFlagSet(uint16_t flag) const
{
    PTB_CHECK_THAT(IsValidSimpleFlag(flag), false);
    return (m_simpleData & flag) == flag;
}

bool Note::HasSlideIntoFromAbove() const
{
    uint8_t type = 0;
    GetSlideInto(type);
    return type == Note::slideIntoFromAbove;
}

bool Note::HasSlideIntoFromBelow() const
{
    uint8_t type = 0;
    GetSlideInto(type);
    return type == Note::slideIntoFromBelow;
}

/// Gets the slide into data (if any)
/// @param type Holds the slide into type return value
/// @return True if the data was returned, false if not
bool Note::GetSlideInto(uint8_t& type) const
{
    type = slideIntoNone;

    // Get the index of the slide
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, slide);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the slide into
    uint32_t symbolData = m_complexSymbolArray[index];
    type = LOBYTE(HIWORD(symbolData));

    return (true);
}

/// Determines if the note has a certain type of slide out
/// @return True if the note has that slide type, false if not
bool Note::HasSlideOutType(uint8_t slideType) const
{
    uint8_t type = 0;
    int8_t steps = 0;

    if (!GetSlideOutOf(type, steps))
    {
        return false;
    }
    return type == slideType;
}

/// Gets the slide out of data (if any)
/// @param type Holds the slide out of type return value
/// @param steps Holds the steps return value
/// @return True if the data was returned, false if not
bool Note::GetSlideOutOf(uint8_t& type, int8_t& steps) const
{
    type = slideIntoNone;
    steps = 0;

    // Get the index of the slide
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, slide);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the slide out of
    uint32_t symbolData = m_complexSymbolArray[index];
    type = HIBYTE(LOWORD(symbolData));
    steps = LOBYTE(LOWORD(symbolData));

    return (true);
}

bool Note::HasSlideOutOfDownwards() const
{
    uint8_t type = 0;
    int8_t steps = 0;
    GetSlideOutOf(type, steps);
    return type == slideOutOfDownwards;
}

bool Note::HasSlideOutOfUpwards() const
{
    uint8_t type = 0;
    int8_t steps = 0;
    GetSlideOutOf(type, steps);
    return type == slideOutOfUpwards;
}

/// Gets the bend data (if any)
/// @param type Holds the bend type return value
/// @param bentPitch Holds the bent pitch return value
/// @param releasePitch Holds the release pitch return value
/// @param duration Holds the duration return value
/// @param drawStartPoint Holds the draw start point return value
/// @param drawEndPoint Holds the draw end point return value
/// @return True if the data was returned, false if not
bool Note::GetBend(uint8_t& type, uint8_t& bentPitch, uint8_t& releasePitch,
                   uint8_t& duration, uint8_t& drawStartPoint, uint8_t& drawEndPoint) const
{
    type = 0;
    duration = 0;
    bentPitch = 0;
    releasePitch = 0;
    drawStartPoint = 0;
    drawEndPoint = 0;

    // Get the index of the bend
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, bend);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the bend
    uint32_t symbolData = m_complexSymbolArray[index];

    type = (uint8_t)((symbolData & bendTypeMask) >> 20);
    duration = HIBYTE(LOWORD(symbolData));
    bentPitch = (uint8_t)((symbolData & bentPitchMask) >> 4);
    releasePitch = (uint8_t)(symbolData & releasePitchMask);
    drawStartPoint = (uint8_t)((symbolData & drawStartMask) >> 18);
    drawEndPoint = (uint8_t)((symbolData & drawEndMask) >> 16);

    return (true);
}

/// Determines if the note has a bend
/// @return True if the note has a bend, false if not
bool Note::HasBend() const
{
    return (ComplexSymbols::findComplexSymbol(m_complexSymbolArray, bend) != (uint32_t)-1);
}

/// Gets the tapped harmonic data (if any)
/// @param tappedFretNumber Holds the tapped fret number return value
/// @return True if the data was returned, false if not
bool Note::GetTappedHarmonic(uint8_t& tappedFretNumber) const
{
    tappedFretNumber = 0;

    // Get the index of the tapped harmonic
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, tappedHarmonic);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the tapped harmonic
    uint32_t symbolData = m_complexSymbolArray[index];
    tappedFretNumber = LOBYTE(LOWORD(symbolData));

    return (true);
}

/// Determines if the note has a tapped harmonic
/// @return True if the note has a tapped harmonic, false if not
bool Note::HasTappedHarmonic() const
{
    return (ComplexSymbols::findComplexSymbol(m_complexSymbolArray, tappedHarmonic) != (uint32_t)-1);
}

/// Gets the trill data (if any)
/// @param trilledFretNumber Holds the trilled fret number return value
/// @return True if the data was returned, false if not
bool Note::GetTrill(uint8_t& trilledFretNumber) const
{
    trilledFretNumber = 0;

    // Get the index of the trill
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, trill);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the trill
    uint32_t symbolData = m_complexSymbolArray[index];
    trilledFretNumber = LOBYTE(HIWORD(symbolData));

    return (true);
}

/// Determines if the note has a trill
/// @return True if the note has a trill, false if not
bool Note::HasTrill() const
{
    return (ComplexSymbols::findComplexSymbol(m_complexSymbolArray, trill) != (uint32_t)-1);
}

/// Gets the artificial harmonic data (if any)
/// @param key Holds the key return value
/// @param keyVariation Holds the key variation return value
/// @param octave Holds the octave return value
/// @return True if the data was returned, false if not
bool Note::GetArtificialHarmonic(uint8_t& key, uint8_t& keyVariation,
                                 uint8_t& octave) const
{
    key = 0;
    keyVariation = 0;
    octave = 0;

    // Get the index of the artificial harmonic
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, artificialHarmonic);
    if (index == (uint32_t)-1)
        return (false);

    // Get the individual pieces that make up the artificial harmonic
    uint32_t symbolData = m_complexSymbolArray[index];
    key = LOBYTE(LOWORD(symbolData));
    keyVariation = HIBYTE(LOWORD(symbolData));
    octave = LOBYTE(HIWORD(symbolData));

    return (true);
}

/// Determines if the note has a artificial harmonic
/// @return True if the note has a artificial harmonic, false if not
bool Note::HasArtificialHarmonic() const
{
    return (ComplexSymbols::findComplexSymbol(m_complexSymbolArray, artificialHarmonic) != (uint32_t)-1);
}

/// Determines if the note is an 8va
/// @return True if the note is an 8va, false if not
bool Note::IsOctave8va() const
{
    return IsSimpleFlagSet(octave8va);
}

/// Determines if the note is an 15ma
/// @return True if the note is an 15ma, false if not
bool Note::IsOctave15ma() const
{
    return IsSimpleFlagSet(octave15ma);
}

/// Determines if the note is an 8vb
/// @return True if the note is an 8vb, false if not
bool Note::IsOctave8vb() const
{
    return IsSimpleFlagSet(octave8vb);
}

/// Determines if the note is an 15mb
/// @return True if the note is an 15mb, false if not
bool Note::IsOctave15mb() const
{
    return IsSimpleFlagSet(octave15mb);
}

/// Determines if the note is tied
/// @return True if the note is tied, false if not
bool Note::IsTied() const
{
    return IsSimpleFlagSet(tied);
}

/// Determines if the note is muted
/// @return True if the note is muted, false if not
bool Note::IsMuted() const
{
    return IsSimpleFlagSet(muted);
}

/// Determines if the note has a tie wrap
/// @return True if the note has a tie wrap, false if not
bool Note::HasTieWrap() const
{
    return IsSimpleFlagSet(tieWrap);
}

/// Determines if the note has a hammer on
/// @return True if the note has a hammer on, false if not
bool Note::HasHammerOn() const
{
    return IsSimpleFlagSet(hammerOn) && !IsSimpleFlagSet(hammerPullFromToNowhere);
}

/// Determines if the note has a hammer on from nowhere
/// @return True if the note has a hammer on from nowhere, false if not
bool Note::HasHammerOnFromNowhere() const
{
    return IsSimpleFlagSet(hammerOn | hammerPullFromToNowhere);
}

/// Determines if the note has a pull off
/// @return True if the note has a pull off, false if not
bool Note::HasPullOff() const
{
    return IsSimpleFlagSet(pullOff) && !IsSimpleFlagSet(hammerPullFromToNowhere);
}

/// Determines if the note has a pull off to nowhere
/// @return True if the note has a pull off to nowhere, false if not
bool Note::HasPullOffToNowhere() const
{
    return IsSimpleFlagSet(pullOff) && IsSimpleFlagSet(hammerPullFromToNowhere);
}

/// Determines if the note is a natural harmonic
/// @return True if the note is a natural harmonic, false if not
bool Note::IsNaturalHarmonic() const
{
    return IsSimpleFlagSet(naturalHarmonic);
}

/// Determines if the note is a ghost note
/// @return True if the note is a ghost note, false if not
bool Note::IsGhostNote() const
{
    return IsSimpleFlagSet(ghostNote);
}

}
