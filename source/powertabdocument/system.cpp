/////////////////////////////////////////////////////////////////////////////
// Name:            system.cpp
// Purpose:         Stores and renders a system
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "system.h"
#include <cassert>
#include <algorithm>

#include "powertabfileheader.h"                     // Needed for file version constants

// Default Constants
const Rect System::DEFAULT_RECT                                   = Rect(0,0,0,0);
const uint8_t System::DEFAULT_POSITION_SPACING                       = 20;
const uint8_t System::DEFAULT_RHYTHM_SLASH_SPACING_ABOVE             = 0;
const uint8_t System::DEFAULT_RHYTHM_SLASH_SPACING_BELOW             = 0;
const uint8_t System::DEFAULT_EXTRA_SPACING                          = 0;
const uint8_t System::SYSTEM_SYMBOL_SPACING = 18; // spacing given to a system symbol (i.e. rehearsal sign)

// Position Spacing Constants
const uint8_t System::MIN_POSITION_SPACING                           = 3;

// Constructor/Destructor
/// Default Constructor
System::System() :
    m_rect(DEFAULT_RECT), m_positionSpacing(DEFAULT_POSITION_SPACING),
    m_rhythmSlashSpacingAbove(DEFAULT_RHYTHM_SLASH_SPACING_ABOVE),
    m_rhythmSlashSpacingBelow(DEFAULT_RHYTHM_SLASH_SPACING_BELOW),
    m_extraSpacing(DEFAULT_EXTRA_SPACING)
{
    //------Last Checked------//
    // - Jan 14, 2005
}

/// Copy Constructor
System::System(const System& system) :
    m_rect(DEFAULT_RECT), m_positionSpacing(DEFAULT_POSITION_SPACING),
    m_rhythmSlashSpacingAbove(DEFAULT_RHYTHM_SLASH_SPACING_ABOVE),
    m_rhythmSlashSpacingBelow(DEFAULT_RHYTHM_SLASH_SPACING_BELOW),
    m_extraSpacing(DEFAULT_EXTRA_SPACING)
{
    //------Last Checked------//
    // - Dec 16, 2004
    *this = system;
}

/// Destructor
System::~System()
{
    //------Last Checked------//
    // - Jan 14, 2005
    for (uint32_t i = 0; i < m_directionArray.size(); i++)
    {
        delete m_directionArray.at(i);
    }
    for (uint32_t i = 0; i < m_chordTextArray.size(); i++)
    {
        delete m_chordTextArray.at(i);
    }
    for (uint32_t i = 0; i < m_rhythmSlashArray.size(); i++)
    {
        delete m_rhythmSlashArray.at(i);
    }
    for (uint32_t i = 0; i < m_staffArray.size(); i++)
    {
        delete m_staffArray.at(i);
    }
    for (uint32_t i = 0; i < m_barlineArray.size(); i++)
    {
        delete m_barlineArray.at(i);
    }
    m_directionArray.clear();
    m_chordTextArray.clear();
    m_rhythmSlashArray.clear();
    m_staffArray.clear();
    m_barlineArray.clear();
}

/// Assignment Operator
const System& System::operator=(const System& system)
{
    //------Last Checked------//
    // - Jan 14, 2005

    // Check for assignment to self
    if (this != &system)
    {
        m_rect = system.m_rect;
        m_positionSpacing = system.m_positionSpacing;
        m_rhythmSlashSpacingAbove = system.m_rhythmSlashSpacingAbove;
        m_rhythmSlashSpacingBelow = system.m_rhythmSlashSpacingBelow;
        m_extraSpacing = system.m_extraSpacing;
        m_startBar = system.m_startBar;
        m_directionArray = system.m_directionArray;
        m_chordTextArray = system.m_chordTextArray;
        m_rhythmSlashArray = system.m_rhythmSlashArray;
        m_staffArray = system.m_staffArray;
        m_barlineArray = system.m_barlineArray;
        m_endBar = system.m_endBar;
    }
    return (*this);
}

/// Equality Operator
bool System::operator==(const System& system) const
{
    //------Last Checked------//
    // - Jan 14, 2005
    return (
        (m_rect == system.m_rect) &&
        (m_positionSpacing == system.m_positionSpacing) &&
        (m_rhythmSlashSpacingAbove == system.m_rhythmSlashSpacingAbove) &&
        (m_rhythmSlashSpacingBelow == system.m_rhythmSlashSpacingBelow) &&
        (m_extraSpacing == system.m_extraSpacing) &&
        (m_startBar == system.m_startBar) &&
        (m_directionArray == system.m_directionArray) &&
        (m_chordTextArray == system.m_chordTextArray) &&
        (m_rhythmSlashArray == system.m_rhythmSlashArray) &&
        (m_staffArray == system.m_staffArray) &&
        (m_barlineArray == system.m_barlineArray) &&
        (m_endBar == system.m_endBar)
    );
}

/// Inequality Operator
bool System::operator!=(const System& system) const
{
    //------Last Checked------//
    // - Jan 5, 2005
    return (!operator==(system));
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool System::Serialize(PowerTabOutputStream& stream)
{
    //------Last Checked------//
    // - Jan 14, 2005
    stream.WriteMFCRect(m_rect);
    CHECK_THAT(stream.CheckState(), false);

    // Note: End bar is stored as a byte; we use Barline class to make it easier
    // for the user
    uint8_t endBar = (uint8_t)((m_endBar.GetType() << 5) |
        (m_endBar.GetRepeatCount()));
    stream << endBar << m_positionSpacing << m_rhythmSlashSpacingAbove <<
        m_rhythmSlashSpacingBelow << m_extraSpacing;
    CHECK_THAT(stream.CheckState(), false);

    m_startBar.Serialize(stream);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_directionArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_chordTextArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_rhythmSlashArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_staffArray);
    CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_barlineArray);
    CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool System::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    //------Last Checked------//
    // - Jan 14, 2005

    // Version 1.0 and 1.0.2
    if (version == PowerTabFileHeader::FILEVERSION_1_0 ||
        version == PowerTabFileHeader::FILEVERSION_1_0_2)
    {
        uint8_t key;
        uint16_t endBar;

        stream.ReadMFCRect(m_rect);
        CHECK_THAT(stream.CheckState(), false);

        stream >> key >> endBar >> m_positionSpacing >>
            m_rhythmSlashSpacingAbove >> m_rhythmSlashSpacingBelow >>
            m_extraSpacing;
        CHECK_THAT(stream.CheckState(), false);

        // Update the key signature at start of section (always shown)
        uint8_t keyType = (uint8_t)((key >> 4) & 0xf);
        uint8_t keyAccidentals = (uint8_t)(key & 0xf);

        m_startBar.GetKeySignatureRef().Show();

        // Cancellation
        if (keyType > 2)
            m_startBar.GetKeySignatureRef().SetCancellation();

        keyType = (uint8_t)(((keyType % 2) == 1) ? KeySignature::majorKey :
            KeySignature::minorKey);

        m_startBar.GetKeySignatureRef().SetKey(keyType, keyAccidentals);

        // Update the ending bar
        uint8_t barType = HIBYTE(endBar);
        uint8_t repeatCount = LOBYTE(endBar);

        m_endBar.SetBarlineData(barType, repeatCount);
        //SetEndBar(barType, repeatCount);

        stream.ReadVector(m_directionArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_chordTextArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_rhythmSlashArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_staffArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_barlineArray, version);
        CHECK_THAT(stream.CheckState(), false);

        // Any barline at position zero is now stored in the section m_startBar
        if (GetBarlineCount() > 0)
        {
            Barline* barline = m_barlineArray[0];
            if (barline != NULL)
            {
                if (barline->GetPosition() == 0)
                {
                    m_startBar = *barline;
                    delete barline;
                    m_barlineArray.erase(m_barlineArray.begin());
                }
            }
        }

        // Update key signs that aren't show to match active key sign
        KeySignature* activeKeySignature = m_startBar.GetKeySignaturePtr();

        size_t i = 0;
        size_t count = m_barlineArray.size();
        for (; i < count; i++)
        {
            KeySignature& keySignature = m_barlineArray[i]->GetKeySignatureRef();

            // Key on bar doesn't match active
            if (keySignature != *activeKeySignature)
            {
                // Key isn't shown, update key to match
                if (!keySignature.IsShown())
                {
                    keySignature = *activeKeySignature;
                    keySignature.Hide();
                    keySignature.SetCancellation(false);
                }

                // Update active key
                activeKeySignature = m_barlineArray[i]->GetKeySignaturePtr();
            }
        }
    }
    // Version 1.5 and up
    else
    {
        stream.ReadMFCRect(m_rect);
        CHECK_THAT(stream.CheckState(), false);

        uint8_t endBar = 0;
        stream >> endBar >> m_positionSpacing >> m_rhythmSlashSpacingAbove >>
            m_rhythmSlashSpacingBelow >> m_extraSpacing;
        CHECK_THAT(stream.CheckState(), false);

        // Update end bar (using Barline class is easier to use)
        m_endBar.SetBarlineData((uint8_t)((endBar & 0xe0) >> 5),
            (uint8_t)(endBar & 0x1f));

        m_startBar.Deserialize(stream, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_directionArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_chordTextArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_rhythmSlashArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_staffArray, version);
        CHECK_THAT(stream.CheckState(), false);

        stream.ReadVector(m_barlineArray, version);
        CHECK_THAT(stream.CheckState(), false);
    }

    return (stream.CheckState());
}

// Barline Array Functions
/// Gets the barline at a given position
/// @param position Position to get the barline for
/// @return A pointer to the barline at the position, or NULL if the barline
/// doesn't exist
Barline* System::GetBarlineAtPosition(uint32_t position) const
{
    //------Last Checked------//
    // - Jan 26, 2005
    if (position == 0)
        return ((Barline*)&m_startBar);

    // Iterate through the barlines
    size_t barlineIndex = 0;
    size_t barlineCount = GetBarlineCount();
    for (; barlineIndex < barlineCount; barlineIndex++)
    {
        Barline* barline = GetBarline(barlineIndex);

        if (barline == NULL)
        {
            assert(false);
            continue;
        }

        // Found it; return the barline
        if (barline->GetPosition() == position)
            return (barline);
    }

    // Barline not found at position
    return (NULL);
}

// Comparison functor for barline positions
struct CompareBarlineToPosition
{
    uint32_t position;
    bool operator()(Barline* barline)
    {
        return barline->GetPosition() <= position;
    }
};

// Barline Array Functions
/// Gets the barline preceding a given position
/// @param position Position to get the preceding barline for
/// @return A pointer to the barline preceding the position
Barline* System::GetPrecedingBarline(uint32_t position) const
{
    if (m_barlineArray.empty() || position < m_barlineArray.at(0)->GetPosition())
    {
        return (Barline*)&m_startBar;
    }

    CompareBarlineToPosition compareToPosition;
    compareToPosition.position = position;
    auto barline = std::find_if(m_barlineArray.rbegin(), m_barlineArray.rend(), compareToPosition);

    return *barline;
}

Barline* System::GetNextBarline(uint32_t position) const
{
    // if position is past the last barline in array return m_endBar
    if (m_barlineArray.empty() ||
        position > m_barlineArray.at(GetBarlineCount() - 1)->GetPosition())
        return const_cast<Barline *>(&m_endBar);

    // if position is before the first non-startbar barline
    if (position < m_barlineArray.at(0)->GetPosition())
        return m_barlineArray.at(0);
    
    CompareBarlineToPosition compareToPosition;
    compareToPosition.position = position;
    auto barline = 
            std::find_if(m_barlineArray.rbegin(), 
                         m_barlineArray.rend(),  
                         compareToPosition);

    --barline;
    return *barline;
}

/// Gets a list of barlines in the system
/// @param barlineArray Holds the barline return values
void System::GetBarlines(vector<Barline*>& barlineArray)
{
    //------Last Checked------//
    // - Apr 25, 2006
    barlineArray.clear();
    barlineArray.push_back(&m_startBar);

    for (int barline = 0, barlineCount = m_barlineArray.size();
        barline < barlineCount; ++barline)
    {
        barlineArray.push_back(m_barlineArray[barline]);
    }

    barlineArray.push_back(&m_endBar);
}

// Position Functions
/// Determines if a position is valid
/// @param position Zero-based index of the position to validate
/// @return True if the position is valid, false if not
bool System::IsValidPosition(int position) const
{
    //------Last Checked------//
    // - Aug 30, 2007
    return ((position >= 0) && (position <= GetPositionCount()));
}

/// Calculates the number of positions that will fit across the system based on
/// a given position spacing
/// @param positionSpacing Position spacing used to perform the calculation
/// @return The number of positions that will fit across the system
int System::CalculatePositionCount(int positionSpacing) const
{
    //------Last Checked------//
    // - Aug 30, 2007
    CHECK_THAT(IsValidPositionSpacing(positionSpacing), 0);

    int returnValue = 0;

    // The available width extends from the first position to the right side of
    // the system
    int width = m_rect.GetWidth();

    // Subtract the width of the key and time signatures on the barlines within
    // the system (does not include the starting barline)
    width -= GetCumulativeInternalKeyAndTimeSignatureWidth();

    // We need at least 1 position worth of space from the last position and the
    // end of the system
    width -= positionSpacing;

    // If we have enough width for at least 1 position, calculate the position
    // count
    if (width >= positionSpacing)
        returnValue = width / positionSpacing;

    return (returnValue);
}

/// Gets the number of positions that can fit across the system based on the
/// current position width
/// @return The number of positions that can fit across the system
int System::GetPositionCount() const
{
    //------Last Checked------//
    // - Aug 30, 2007

    // Calculate the position count using the current position spacing
    return (CalculatePositionCount(GetPositionSpacing()));
}

/// Gets the x co-ordinate of the first position in the system, relative to the left edge
/// @return The x co-ordinate of the first position in the system
int System::GetFirstPositionX() const
{
    //------Last Checked------//
    // - Aug 30, 2007

    int returnValue = 0;

    // Add the width of the clef; the symbol itself is 16 units wide, with 3
    // units of space on both sides, for a total of 22 units
    returnValue += 22;

    // Add the width of the starting key signature
    int keySignatureWidth = m_startBar.GetKeySignatureConstRef().GetWidth();
    returnValue += keySignatureWidth;

    // Add the width of the starting time signature
    int timeSignatureWidth = m_startBar.GetTimeSignatureConstRef().GetWidth();
    returnValue += timeSignatureWidth;

    // If we have both a key and time signature, they are separated by 3 units
    if ((keySignatureWidth > 0) && (timeSignatureWidth > 0))
        returnValue += 3;

    // Add the width required by the starting barline; for a standard barline,
    // this is 1 unit of space, otherwise it is the distance between positions
    int barlineWidth = ((m_startBar.IsBar()) ? 1 : GetPositionSpacing());
    returnValue += barlineWidth;

    return (returnValue);
}

/// Gets the x co-ordinate of the nth position in the system, relative to the left edge of the system
/// @param position Zero-based index of the position to retrieve the x
/// co-ordinate for
/// @return The x co-ordinate for the position, or the x co-ordinate of the
/// first position if the position is invalid
int System::GetPositionX(int position) const
{
    //------Last Checked------//
    // - Aug 30, 2007

    // Initialize to the first position
    int returnValue = GetFirstPositionX();

    // Validate the position
    CHECK_THAT(IsValidPosition(position), returnValue);

    // Get the width of all key and time signatures up to, but not
    // including, the position
    int keyAndTimeSignatureWidth =
            GetCumulativeInternalKeyAndTimeSignatureWidth(position);

    // Move "n" positions across using the position spacing, adding the
    // cumulative key and time signature widths. Add 1 since the position
    // value is zero-based
    returnValue += (((position + 1) * GetPositionSpacing()) +
                    keyAndTimeSignatureWidth);

    return (returnValue);
}

// Operations
/// Gets the total width used by all key and time signatures that reside within
/// the system (does not include the start bar)
/// @param position Zero-based index of the position to stop at. If -1, traverse
/// all the barlines
int System::GetCumulativeInternalKeyAndTimeSignatureWidth(int position) const
{
    //------Last Checked------//
    // - Sep 01, 2007
    int returnValue = 0;

    bool bAllBarlines = (position == -1);

    // Loop through barline list
    for (int barline = 0, barlineCount = m_barlineArray.size();
        barline < barlineCount; ++barline)
    {
        // Get the position where the barline resides
        int barlinePosition = m_barlineArray[barline]->GetPosition();

        // Only use bars before the index
        if (bAllBarlines || (barlinePosition < position))
        {
            // Ignore keys and time signs at position 0, they're handled in
            // GetFirstPositionX
            if (m_barlineArray[barline]->GetPosition() > 0)
            {
                // Add the width of the key and time signature, if present on
                // the barline
                returnValue +=
                    m_barlineArray[barline]->GetKeyAndTimeSignatureWidth();
            }
        }
        else
            break;
    }

    return (returnValue);
}

// Calculate the height of the entire system
void System::CalculateHeight()
{
    // get height offset to the top of the last staff
    int sum = GetStaffHeightOffset(m_staffArray.size() - 1);

    // add the height of the last staff
    sum += m_staffArray.at(m_staffArray.size() - 1)->GetHeight();

    m_rect.SetHeight(sum);
}

// Get the height offset of a staff from the top of the system
// Optionally, can return the absolute position of the top of the staff
uint32_t System::GetStaffHeightOffset(uint32_t staff, bool absolutePos)
{
    CHECK_THAT(IsValidStaffIndex(staff), 0);

    uint32_t offset = GetExtraSpacing() + GetRhythmSlashSpacingAbove() + GetRhythmSlashSpacingBelow();

    for (uint32_t i = 0; i < staff; i++)
    {
        offset += m_staffArray.at(i)->GetHeight();
    }

    if (absolutePos)
    {
        offset += GetRect().GetTop();
    }

    return offset;
}

// Searches for a ChordText object with the specified position
// Returns the index of that object if found, and returns -1 otherwise
int System::FindChordText(uint32_t position) const
{
    CHECK_THAT(IsValidPosition(position), -1);

    for (uint32_t i = 0; i < m_chordTextArray.size(); i++)
    {
        if (m_chordTextArray.at(i)->GetPosition() == position)
        {
            return i;
        }
    }

    return -1;
}

// Inserts a ChordText object at the specified index
// Returns true if the insertion succeeded
bool System::InsertChordText(ChordText* chordText, uint32_t index)
{
    CHECK_THAT(index <= GetChordTextCount(), false);

    m_chordTextArray.insert(m_chordTextArray.begin() + index, chordText);

    return true;
}

// Removes a ChordText object at the specified index
// Returns true if the removal succeeded
// NOTE: this only removes the chord text pointer from the array, and does not call 'delete'
bool System::RemoveChordText(uint32_t index)
{
    CHECK_THAT(IsValidChordTextIndex(index), false);

    m_chordTextArray.erase(m_chordTextArray.begin() + index);
    return true;
}

// Returns the index of a staff within the system
int System::FindStaffIndex(Staff *staff) const
{
    auto result = std::find(m_staffArray.begin(), m_staffArray.end(), staff);
    return std::distance(m_staffArray.begin(), result);
}

// Checks if a rehearsal sign occurs in the system
bool System::HasRehearsalSign() const
{
    if (m_startBar.GetRehearsalSignConstRef().IsSet() || m_endBar.GetRehearsalSignConstRef().IsSet())
    {
        return true;
    }

    for (auto i = m_barlineArray.begin(); i != m_barlineArray.end(); ++i)
    {
        if ((*i)->GetRehearsalSignConstRef().IsSet())
        {
            return true;
        }
    }
    return false;
}
