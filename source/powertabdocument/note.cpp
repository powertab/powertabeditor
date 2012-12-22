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

#include <algorithm>
#include <sstream>
#include "note.h"

#include "chordname.h"
#include "tuning.h"
#include "powertabfileheader.h"             // Needed for file version constants
#include "powertabinputstream.h"
#include "powertaboutputstream.h"
#include "complexsymbolarray.h"

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

/// Primary Constructor
/// @param string Zero-based number of the string the note is played on
/// @param fretNumber Fret number used to play the note
Note::Note(uint32_t string, uint8_t fretNumber) :
    m_stringData(DEFAULT_STRING_DATA),
    m_simpleData(DEFAULT_SIMPLE_DATA)
{
    assert(IsValidString(string));
    assert(IsValidFretNumber(fretNumber));

    SetString(string);
    SetFretNumber(fretNumber);
    ComplexSymbols::clearComplexSymbols(m_complexSymbolArray);
}

/// Copy Constructor
Note::Note(const Note& note) :
    PowerTabObject(),
    m_stringData(DEFAULT_STRING_DATA),
    m_simpleData(DEFAULT_SIMPLE_DATA)
{
    *this = note;
}

/// Assignment Operator
const Note& Note::operator=(const Note& note)
{
    if (this != &note)
    {
        m_stringData = note.m_stringData;
        m_simpleData = note.m_simpleData;
        m_complexSymbolArray = note.m_complexSymbolArray;
    }
    return *this;
}

/// Equality Operator
bool Note::operator==(const Note& note) const
{
    // avoid comparing the complex symbol arrays if possible
    if (m_stringData != note.m_stringData || m_simpleData != note.m_simpleData)
    {
        return false;
    }

    // copy the complex symbol arrays and sort them, since complex symbols aren't necessarily in the same slot in the array
    boost::array<uint32_t, MAX_NOTE_COMPLEX_SYMBOLS> thisComplexSymbolArray(m_complexSymbolArray);
    boost::array<uint32_t, MAX_NOTE_COMPLEX_SYMBOLS> thatComplexSymbolArray(note.m_complexSymbolArray);

    std::sort(thisComplexSymbolArray.begin(), thisComplexSymbolArray.end());
    std::sort(thatComplexSymbolArray.begin(), thatComplexSymbolArray.end());

    return thisComplexSymbolArray == thatComplexSymbolArray;
}

/// Inequality Operator
bool Note::operator!=(const Note& note) const
{
    return !operator==(note);
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

// String Functions
/// Determines if a string is valid
/// @param string String to validate
/// @return True if the string is valid, false if not
bool Note::IsValidString(uint32_t str)
{
    return str <= MAX_STRING;
}

/// Sets the string for the note
/// @param string String to set (zero-based)
/// @return True if the string was set, false if not
bool Note::SetString(uint32_t string)
{
    PTB_CHECK_THAT(IsValidString(string), false);

    m_stringData &= ~stringMask;
    m_stringData |= (uint8_t)(string << 5);

    return (true);
}

/// Gets the string for the note
/// @return The string for the note
uint32_t Note::GetString() const
{
    return (uint8_t)((m_stringData & stringMask) >> 5);
}

// Fret Number Functions
/// Determines if a fret number is valid
/// @param fretNumber Fret number to validate
/// @return True if the fret number is valid, false if not
bool Note::IsValidFretNumber(uint8_t fretNumber)
{
    return fretNumber <= MAX_FRET_NUMBER;
}

/// Sets the fret number for the note. Be careful when using this function, as
/// it will not update hammerons, pulloffs, etc. with adjacent notes.
/// @param fretNumber Fret number to set.
/// @return True if the fret number was set, false if not.
bool Note::SetFretNumber(uint8_t fretNumber)
{
    PTB_CHECK_THAT(IsValidFretNumber(fretNumber), false);

    m_stringData &= ~fretNumberMask;
    m_stringData |= fretNumber;

    // If the trilled fret is the same as the new fret number, clear the trill.
    uint8_t trilledFret = 0;
    if (GetTrill(trilledFret) && trilledFret == fretNumber)
    {
        ClearTrill();
    }

    return true;
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

/// Sets a simple flag
/// @param flag Flag to set
/// @return True if the flag was set, false if not
bool Note::SetSimpleFlag(uint16_t flag)
{
    PTB_CHECK_THAT(IsValidSimpleFlag(flag), false);

    // Mutually exclusive operations
    if ((flag & hammerPullMask) != 0)
        ClearSimpleFlag(hammerPullMask);

    if ((flag & octaveMask) != 0)
        ClearSimpleFlag(octaveMask);

    m_simpleData |= flag;
    return (true);
}

/// Clears a simple flag
/// @param flag Flag to clear
/// @return True if the flag was cleared, false if not
bool Note::ClearSimpleFlag(uint16_t flag)
{
    PTB_CHECK_THAT(IsValidSimpleFlag(flag), false);
    m_simpleData &= ~flag;
    return true;
}

/// Determines if a simple flag is set
/// @param flag Flag to test
/// @return True if the flag is set, false if not
bool Note::IsSimpleFlagSet(uint16_t flag) const
{
    PTB_CHECK_THAT(IsValidSimpleFlag(flag), false);
    return (m_simpleData & flag) == flag;
}

// Slide Into Functions

/// Determines if a slide into type is valid
/// @param type Type to validate
/// @return True if the slide into type is valid, false if not
bool Note::IsValidSlideIntoType(uint8_t type)
{
    return type <= slideIntoLegatoSlideDownwards;
}

/// Determines if slide into data is valid
/// @param type Type to validate
/// @return True if slide into data is valid, false if not
bool Note::IsValidSlideInto(uint8_t type)
{
    return IsValidSlideIntoType(type);
}

/// Sets (adds or updates) a slide into marker
/// @param type Slide into type to set (see slideIntoTypes enum for values)
/// @return True if the slide into marker was added or updated
bool Note::SetSlideInto(uint8_t type)
{
    PTB_CHECK_THAT(IsValidSlideInto(type), false);

    if (type == slideIntoNone)
    {
        ClearSlideInto();
        return true;
    }

    // Look for an existing slide first
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, slide);

    // Slide exists; update its slide into data
    if (index != (uint32_t)-1)
    {
        m_complexSymbolArray[index] &= ~slideIntoTypeMask;
        m_complexSymbolArray[index] |= (uint32_t)(type << 16);
        return (true);
    }

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(MAKEWORD(0, 0), MAKEWORD(type, slide));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Determines if the note has a slide (either in or out)
/// @return True if the note has a slide, false if not
bool Note::HasSlide() const
{
    return HasSlideInto() || HasSlideOutOf();
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

/// Determines if the note has a slide into
/// @return True if the note has a slide into, false if not
bool Note::HasSlideInto() const
{
    uint8_t type = slideIntoNone;
    if (!GetSlideInto(type))
        return (false);
    return (type != slideIntoNone);
}

/// Removes a slide into from the note, if possible
void Note::ClearSlideInto()
{
    // Get the index of the slide
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, slide);
    if (index == (uint32_t)-1)
        return;

    m_complexSymbolArray[index] &= ~slideIntoTypeMask;

    // Return if there is a slide out, otherwise remove the slide symbol
    if (HasSlideOutOf())
        return;

    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, slide);
}

// Slide Out Of Functions

/// Determines if a slide out of type is valid
/// @param type Type to validate
/// @return True if the slide out of type is valid, false if not
bool Note::IsValidSlideOutOfType(uint8_t type)
{
    return type <= slideOutOfUpwards;
}

/// Determines if slide out of data is valid
/// @param type Type to validate
/// @return True if the slide out of data is valid, false if not
bool Note::IsValidSlideOutOf(uint8_t type)
{
    return IsValidSlideOutOfType(type);
}

/// Sets (adds or updates) a slide out of marker
/// @param type Slide out of type to set (see slideOutOf enum for values)
/// @param steps Number of steps to slide, in half tones
/// @return True if the slide out of marker was added or updated
bool Note::SetSlideOutOf(uint8_t type, int8_t steps)
{
    PTB_CHECK_THAT(IsValidSlideOutOf(type), false);

    if (type == slideOutOfNone)
    {
        ClearSlideOutOf();
        return true;
    }

    // Look for an existing slide first
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, slide);

    // Slide exists; update its slide out of data
    if (index != (uint32_t)-1)
    {
        m_complexSymbolArray[index] &= ~(slideOutOfTypeMask | slideOutOfStepsMask);
        m_complexSymbolArray[index] |= MAKELONG(MAKEWORD(steps, type), 0);
        return true;
    }

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(MAKEWORD(steps, type), MAKEWORD(0, slide));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Determines if the note has a slide out of
/// @return True if the note has a slide out of, false if not
bool Note::HasSlideOutOf() const
{
    uint8_t type = slideOutOfNone;
    int8_t steps = 0;
    if (!GetSlideOutOf(type, steps))
        return (false);
    return (type != slideOutOfNone);
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

/// Removes a slide out of from the note, if possible
void Note::ClearSlideOutOf()
{
    // Get the index of the slide
    uint32_t index = ComplexSymbols::findComplexSymbol(m_complexSymbolArray, slide);
    if (index == (uint32_t)-1)
        return;

    m_complexSymbolArray[index] &= ~(slideOutOfTypeMask | slideOutOfStepsMask);

    // Return if there is a slide in, otherwise remove the slide symbol
    if (HasSlideInto())
        return;

    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, slide);
}

// Bend Functions
/// Determines if bend data is valid
/// @param type Type to validate
/// @param bentPitch Bent pitch to validate
/// @param releasePitch Release pitch to validate
/// @param duration Duration to validate
/// @param drawStartPoint Draw start point to validate
/// @param drawEndPoint Draw end point to validate
/// @return True if the bend data is valid, false if not
bool Note::IsValidBend(uint8_t type, uint8_t bentPitch, uint8_t releasePitch,
                       uint8_t duration, uint8_t drawStartPoint, uint8_t drawEndPoint)
{
    // 1st validate the individual pieces
    if (!IsValidBendType(type) || !IsValidBentPitch(bentPitch) ||
        !IsValidReleasePitch(releasePitch) || !IsValidBendDuration(duration) ||
        !IsValidDrawStartPoint(drawStartPoint) ||
        !IsValidDrawEndPoint(drawEndPoint))
    {
        return (false);
    }

    bool returnValue = false;

    if ((type == normalBend) || (type == bendAndHold))
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be zero
        // c) Duration may be any value
        // d) Draw start must be low or mid and end must be higher
        returnValue = ((bentPitch > 0) && (releasePitch == 0) &&
                       (drawStartPoint <= midPoint) && (drawEndPoint > drawStartPoint));
    }
    else if (type == bendAndRelease)
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be less than bent pitch
        // c) Duration must be zero
        // d) Draw start must be low or mid and drawEndPoint must be low or mid
        returnValue = ((bentPitch > 0) && (releasePitch < bentPitch) &&
                       (duration == 0) && (drawStartPoint <= midPoint) &&
                       (drawEndPoint <= midPoint));
    }
    else if ((type == preBend) || (type == preBendAndHold))
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be zero
        // c) Duration must be zero
        // d) Draw start must be low, and end must be higher
        returnValue = ((bentPitch > 0) && (releasePitch == 0) &&
                       (duration == 0) && (drawStartPoint == lowPoint) &&
                       (drawEndPoint > drawStartPoint));
    }
    else if (type == preBendAndRelease)
    {
        // a) Bent pitch must be greater than zero
        // b) Release pitch must be less than bent pitch
        // c) Duration must be zero
        // d) Draw start and end must be low
        returnValue = ((bentPitch > 0) && (releasePitch < bentPitch) &&
                       (duration == 0) && (drawStartPoint == lowPoint) &&
                       (drawEndPoint == lowPoint));
    }
    else if (type == gradualRelease)
    {
        // a) Bent pitch must be zero
        // b) Release pitch must be standard to 2 3/4
        // c) Duration must be > 0
        // d) Draw start must be high or mid point, draw end must be lower
        returnValue = ((bentPitch == 0) && (releasePitch <= 11) &&
                       (duration > 0) && (drawStartPoint >= midPoint) &&
                       (drawEndPoint < drawStartPoint));
    }
    else if (type == immediateRelease)
    {
        // a) Bent pitch must be zero
        // b) Release pitch must be zero
        // c) Duration must be zero
        // d) Draw start must be high or mid, and match draw end
        returnValue = ((bentPitch == 0) && (releasePitch == 0) &&
                       (duration == 0) && (drawStartPoint >= midPoint) &&
                       (drawStartPoint == drawEndPoint));
    }

    return (returnValue);
}

/// Determines if a bend type is valid
/// @param type Bend type to validate
/// @return True if the bend type is valid, false if not
bool Note::IsValidBendType(uint8_t type)
{
    return type <= immediateRelease;
}

/// Determines if a bent pitch is valid
/// @param bentPitch Bent pitch to validate
/// @return True if the bent pitch is valid, false if not
bool Note::IsValidBentPitch(uint8_t bentPitch)
{
    return bentPitch <= MAX_BEND_PITCH;
}

/// Determines if a release pitch is valid
/// @param releasePitch Release pitch to validate
/// @return True if the release pitch is valid, false if not
bool Note::IsValidReleasePitch(uint8_t releasePitch)
{
    return releasePitch <= MAX_BEND_PITCH;
}

/// Determines if a bend duration is valid
/// @param duration Duration to validate
/// @return True if the duration is valid, false if not
bool Note::IsValidBendDuration(uint8_t duration)
{
    return duration <= MAX_BEND_DURATION;
}

/// Determines if a draw start point is valid
/// @param drawStartPoint Draw start point to validate
/// @return True if the draw start point is valid, false if not
bool Note::IsValidDrawStartPoint(uint8_t drawStartPoint)
{
    return drawStartPoint <= highPoint;
}

/// Determines if a draw end point is valid
/// @param drawEndPoint Draw end point to validate
/// @return True if the draw end point is valid, false if not
bool Note::IsValidDrawEndPoint(uint8_t drawEndPoint)
{
    return drawEndPoint <= highPoint;
}

/// Sets (adds or updates) a bend
/// @param type Type of bend to set (see bendTypes enum for values)
/// @param bentPitch Amount of pitch to bend, in quarter steps
/// @param releasePitch Pitch to release to, in quarter steps
/// @param duration The duration of the bend (0 = default, occurs over 32nd
/// note, 1 = occurs over duration of note, 2 = occurs over duration of note +
/// next note, etc.)
/// @param drawStartPoint Vertical draw start point for the bend (see
/// bendDrawingPoints enum for values)
/// @param drawEndPoint Vertical draw end point for the bend (see
/// bendDrawingPoints enum for values)
/// @return True if the bend was added or updated
bool Note::SetBend(uint8_t type, uint8_t bentPitch, uint8_t releasePitch,
                   uint8_t duration, uint8_t drawStartPoint, uint8_t drawEndPoint)
{
    PTB_CHECK_THAT(IsValidBend(type, bentPitch, releasePitch, duration, drawStartPoint,
                           drawEndPoint), false);

    // Construct the symbol data, then add it to the array
    uint32_t symbolData = MAKELONG(0, MAKEWORD(0, bend));
    symbolData |= (uint32_t)(type << 20);
    symbolData |= (uint32_t)(drawStartPoint << 18);
    symbolData |= (uint32_t)(drawEndPoint << 16);
    symbolData |= (uint32_t)(duration << 8);
    symbolData |= (uint32_t)(bentPitch << 4);
    symbolData |= (uint32_t)(releasePitch);

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Removes a bend from the note, if possible
void Note::ClearBend()
{
   ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, bend);
}

// Tapped Harmonic Functions
/// Determines if a tapped fret number is valid
/// @param tappedFretNumber Tapped fret number to validate
/// @return True if the tapped fret number is valid, false if not
bool Note::IsValidTappedFretNumber(uint8_t tappedFretNumber)
{
    return tappedFretNumber <= MAX_FRET_NUMBER;
}

/// Determines if tapped harmonic data is valid
/// @param tappedFretNumber Tapped fret number to validate
/// @return True if the tapped harmonic data is valid, false if not
bool Note::IsValidTappedHarmonic(uint8_t tappedFretNumber)
{
    return IsValidTappedFretNumber(tappedFretNumber);
}

/// Sets (adds or updates) a tapped harmonic
/// @param tappedFretNumber Tapped fret number
/// @return True if the tapped harmonic was added or updated
bool Note::SetTappedHarmonic(uint8_t tappedFretNumber)
{
    PTB_CHECK_THAT(IsValidTappedHarmonic(tappedFretNumber), false);

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(MAKEWORD(tappedFretNumber, 0),
                                   MAKEWORD(0, tappedHarmonic));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Removes a tapped harmonic from the note, if possible
void Note::ClearTappedHarmonic()
{
    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, tappedHarmonic);
}

// Trill Functions
/// Determines if a trilled fret number is valid
/// @param trilledFretNumber Trilled fret number to validate
/// @return True if the trilled fret number is valid, false if not
bool Note::IsValidTrilledFretNumber(uint8_t trilledFretNumber)
{
    return trilledFretNumber <= MAX_FRET_NUMBER;
}

/// Determines if trill data is valid
/// @param trilledFretNumber Trilled fret number to validate
/// @return True if the trill data is valid, false if not
bool Note::IsValidTrill(uint8_t trilledFretNumber) const
{
    return IsValidTrilledFretNumber(trilledFretNumber) && trilledFretNumber != GetFretNumber();
}

/// Sets (adds or updates) a trill
/// @param trilledFretNumber Trilled fret number
/// @return True if the trill was added or updated
bool Note::SetTrill(uint8_t trilledFretNumber)
{
    PTB_CHECK_THAT(IsValidTrill(trilledFretNumber), false);

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(0, MAKEWORD(trilledFretNumber, trill));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Removes a trill from the note, if possible
void Note::ClearTrill()
{
    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, trill);
}

// Artificial Harmonic Functions
/// Determines if a artificial octave is valid
/// @param octave Octave to validate
/// @return True if the artificial harmonic octave is valid, false if not
bool Note::IsValidArtificialHarmonicOctave(uint8_t octave)
{
    return octave <= artificialHarmonicOctave15ma;
}

/// Determines if artificial harmonic data is valid
/// @param key Key to validate
/// @param keyVariation Key variation to validate
/// @param octave Octave to validate
/// @return True if the artificial harmonic data is valid, false if not
bool Note::IsValidArtificialHarmonic(uint8_t key, uint8_t keyVariation,
                                     uint8_t octave)
{
    return (ChordName::IsValidKeyAndVariation(key, keyVariation) &&
        IsValidArtificialHarmonicOctave(octave));
}

/// Sets (adds or updates) a artificial harmonic
/// @param key Key to set (see ChordName::keys enum for values)
/// @param keyVariation Key variation to set (see ChordName::keyVariation enum
/// for values)
/// @param octave Octave to set
/// @return True if the artificial harmonic was added or updated
bool Note::SetArtificialHarmonic(uint8_t key, uint8_t keyVariation, uint8_t octave)
{
    PTB_CHECK_THAT(IsValidArtificialHarmonic(key, keyVariation, octave), false);

    // Construct the symbol data, then add it to the array
    const uint32_t symbolData = MAKELONG(MAKEWORD(key, keyVariation),
                                         MAKEWORD(octave, artificialHarmonic));

    ComplexSymbols::addComplexSymbol(m_complexSymbolArray, symbolData);
    return true;
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

/// Removes a artificial harmonic from the note, if possible
void Note::ClearArtificialHarmonic()
{
    ComplexSymbols::removeComplexSymbol(m_complexSymbolArray, artificialHarmonic);
}

// Complex Symbol Array Functions
/// Determines if a complex symbol type is valid
/// @param type Symbol type to validate
/// @return True if the symbol type is valid, false if not
bool Note::IsValidComplexSymbolType(uint8_t type)
{
    return ((type == slide) || (type == bend) || (type == tappedHarmonic) ||
            (type == trill) || (type == artificialHarmonic));
}

/// Creates a text representation of the note, including brackets for ghost notes, harmonics, etc
/// @return The text representation of the note
std::string Note::GetText() const
{
    // For muted notes, display 'x'
    // For tapped harmonics, display '7(14)', where 14 is the tapped note
    // For natural harmonics, display '[12]'
    // For ghost notes, display '(12)'
    // Otherwise, just display the fret number

    if (IsMuted())
    {
        return "x";
    }

    std::stringstream output;
    uint8_t noteValue = GetFretNumber();

    // for tapped harmonics and trills, display original note first, and tapped/trilled note after
    if (HasTappedHarmonic() || HasTrill())
    {
        output << static_cast<int>(noteValue);
        if (HasTappedHarmonic())
        {
            GetTappedHarmonic(noteValue);
        }
        if (HasTrill())
        {
            GetTrill(noteValue);
        }
    }

    std::string brackets = "";
    if (HasTappedHarmonic() || IsGhostNote() || HasTrill())
    {
        brackets = "()";
    }
    else if (IsNaturalHarmonic())
    {
        brackets = "[]";
    }

    if (!brackets.empty())
    {
        output << brackets.at(0);
    }
    output << static_cast<int>(noteValue);
    if (!brackets.empty())
    {
        output << brackets.at(1);
    }

    return output.str();
}

/// Calculates the pitch of the note, for the given guitar tuning.
/// @param tuning The tuning for the guitar that is playing the note.
/// @param includeOffset Specifies whether to include the music notation offset
/// in the pitch.
/// @return The pitch of the note.
uint8_t Note::GetPitch(const Tuning& tuning, bool includeOffset) const
{
    return tuning.GetNote(GetString(), includeOffset) + GetFretNumber();
}

/// Returns the number of octaves (from -2 to +2) that the note is shifted by
int Note::GetOctaveOffset() const
{
    if (IsOctave8va())
    {
        return 1;
    }
    else if (IsOctave15ma())
    {
        return 2;
    }
    else if (IsOctave8vb())
    {
        return -1;
    }
    else if (IsOctave15mb())
    {
        return -2;
    }
    
    return 0;    
}

// 8va Octave Functions
/// Sets or clears an 8va octave marker
/// @param set True to set the 8va octave marker, false to clear it
/// @return True if the 8va octave marker was set or cleared, false if not
bool Note::SetOctave8va(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(octave8va);
    }
    return SetSimpleFlag(octave8va);
}

/// Determines if the note is an 8va
/// @return True if the note is an 8va, false if not
bool Note::IsOctave8va() const
{
    return IsSimpleFlagSet(octave8va);
}

// 15ma Octave Functions
/// Sets or clears an 15ma octave marker
/// @param set True to set the 15ma octave marker, false to clear it
/// @return True if the 15ma octave marker was set or cleared, false if not
bool Note::SetOctave15ma(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(octave15ma);
    }
    return SetSimpleFlag(octave15ma);
}

/// Determines if the note is an 15ma
/// @return True if the note is an 15ma, false if not
bool Note::IsOctave15ma() const
{
    return IsSimpleFlagSet(octave15ma);
}

// 8vb Octave Functions
/// Sets or clears an 8vb octave marker
/// @param set True to set the 8vb octave marker, false to clear it
/// @return True if the 8vb octave marker was set or cleared, false if not
bool Note::SetOctave8vb(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(octave8vb);
    }

    return SetSimpleFlag(octave8vb);
}

/// Determines if the note is an 8vb
/// @return True if the note is an 8vb, false if not
bool Note::IsOctave8vb() const
{
    return IsSimpleFlagSet(octave8vb);
}

// 15mb Octave Functions
/// Sets or clears an 15mb octave marker
/// @param set True to set the 15mb octave marker, false to clear it
/// @return True if the 15mb octave marker was set or cleared, false if not
bool Note::SetOctave15mb(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(octave15mb);
    }
    return SetSimpleFlag(octave15mb);
}

/// Determines if the note is an 15mb
/// @return True if the note is an 15mb, false if not
bool Note::IsOctave15mb() const
{
    return IsSimpleFlagSet(octave15mb);
}

/// Sets or clears a tie
/// @param set True to set the tie, false to clear it
/// @return True if the tie was set or cleared, false if not
bool Note::SetTied(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(tied);
    }
    else
    {
        return SetSimpleFlag(tied);
    }
}

/// Determines if the note is tied
/// @return True if the note is tied, false if not
bool Note::IsTied() const
{
    return IsSimpleFlagSet(tied);
}

/// Sets or clears the muted effect
/// @param set True to set the muted effect, false to clear it
/// @return True if the muted effect was set or cleared, false if not
bool Note::SetMuted(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(muted);
    }
    else
    {
        return SetSimpleFlag(muted);
    }
}

/// Determines if the note is muted
/// @return True if the note is muted, false if not
bool Note::IsMuted() const
{
    return IsSimpleFlagSet(muted);
}

/// Sets or clears a tie wrap
/// @param set True to set the tie wrap, false to clear it
/// @return True if the tie wrap was set or cleared, false if not
bool Note::SetTieWrap(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(tieWrap);
    }
    else
    {
        return SetSimpleFlag(tieWrap);
    }
}

/// Determines if the note has a tie wrap
/// @return True if the note has a tie wrap, false if not
bool Note::HasTieWrap() const
{
    return IsSimpleFlagSet(tieWrap);
}

/// Sets or clears a hammer on
/// @param set True to set the hammer on, false to clear it
/// @return True if the hammer on was set or cleared, false if not
bool Note::SetHammerOn(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(hammerPullMask);
    }
    else
    {
        return SetSimpleFlag(hammerOn) && ClearSimpleFlag(hammerPullFromToNowhere);
    }
}

/// Determines if the note has a hammer on
/// @return True if the note has a hammer on, false if not
bool Note::HasHammerOn() const
{
    return IsSimpleFlagSet(hammerOn) && !IsSimpleFlagSet(hammerPullFromToNowhere);
}

/// Sets or clears a hammer on from nowhere
/// @param set True to set the hammer on from nowhere, false to clear it
/// @return True if the hammer on from nowhere was set or cleared, false if
/// not
bool Note::SetHammerOnFromNowhere(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(hammerPullMask);
    }
    else
    {
        return SetSimpleFlag(hammerOn | hammerPullFromToNowhere);
    }
}

/// Determines if the note has a hammer on from nowhere
/// @return True if the note has a hammer on from nowhere, false if not
bool Note::HasHammerOnFromNowhere() const
{
    return IsSimpleFlagSet(hammerOn | hammerPullFromToNowhere);
}

/// Sets or clears a pull off
/// @param set True to set the pull off, false to clear it
/// @return True if the pull off was set or cleared, false if not
bool Note::SetPullOff(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(hammerPullMask);
    }
    else
    {
        return SetSimpleFlag(pullOff) && ClearSimpleFlag(hammerPullFromToNowhere);
    }
}

/// Determines if the note has a pull off
/// @return True if the note has a pull off, false if not
bool Note::HasPullOff() const
{
    return IsSimpleFlagSet(pullOff) && !IsSimpleFlagSet(hammerPullFromToNowhere);
}

/// Sets or clears a pull off to nowhere
/// @param set True to set the pull off to nowhere, false to clear it
/// @return True if the pull off to nowhere was set or cleared, false if not
bool Note::SetPullOffToNowhere(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(hammerPullMask);
    }
    else
    {
        return SetSimpleFlag(pullOff | hammerPullFromToNowhere);
    }
}

/// Determines if the note has a pull off to nowhere
/// @return True if the note has a pull off to nowhere, false if not
bool Note::HasPullOffToNowhere() const
{
    return IsSimpleFlagSet(pullOff) && IsSimpleFlagSet(hammerPullFromToNowhere);
}

/// Determines if the note has either a hammer-on or a pull-off
/// @return True if the note has either a hammer-on or a pull-off
bool Note::HasHammerOnOrPulloff() const
{
    return HasHammerOn() || HasPullOff();
}

/// Sets or clears a natural harmonic
/// @param set True to set the natural harmonic, false to clear it
/// @return True if the natural harmonic was set or cleared, false if not
bool Note::SetNaturalHarmonic(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(naturalHarmonic);
    }
    else
    {
        return SetSimpleFlag(naturalHarmonic);
    }
}

/// Determines if the note is a natural harmonic
/// @return True if the note is a natural harmonic, false if not
bool Note::IsNaturalHarmonic() const
{
    return IsSimpleFlagSet(naturalHarmonic);
}

/// Sets or clears a ghost note
/// @param set True to set the ghost note, false to clear it
/// @return True if the ghost note was set or cleared, false if not
bool Note::SetGhostNote(bool set)
{
    if (!set)
    {
        return ClearSimpleFlag(ghostNote);
    }
    else
    {
        return SetSimpleFlag(ghostNote);
    }
}

/// Determines if the note is a ghost note
/// @return True if the note is a ghost note, false if not
bool Note::IsGhostNote() const
{
    return IsSimpleFlagSet(ghostNote);
}

/// Returns a text description of the bend (e.g. "1 1/2" or "Full")
std::string Note::GetBendText(uint8_t pitch)
{
    PTB_CHECK_THAT(IsValidBentPitch(pitch), "");

    std::stringstream text;

    if (pitch == 0)
    {
        text << "Standard";
    }
    else if (pitch == 4)
    {
        text << "Full";
    }
    // display a fraction
    else
    {
        const int quotient = pitch / 4;
        const int remainder = pitch % 4;

        // whole number
        if (quotient != 0)
        {
            text << quotient;

            if (remainder != 0)
            {
                text << " ";
            }
        }

        // fractional part
        if (remainder != 0)
        {
            // reduce the fraction
            if (remainder == 1 || remainder == 3)
            {
                text << remainder << "/" << 4;
            }
            else
            {
                text << "1/2";
            }
        }
    }

    return text.str();
}
