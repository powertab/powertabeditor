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

#include "barline.h"
#include "chordtext.h"
#include "direction.h"
#include "powertabfileheader.h"
#include "powertabinputstream.h"
#include "powertaboutputstream.h"
#include "rhythmslash.h"
#include "staff.h"

namespace PowerTabDocument {

// Default Constants
const Rect System::DEFAULT_RECT                                   = Rect(50, 20, 750, 0);
const uint8_t System::DEFAULT_POSITION_SPACING                       = 20;
const uint8_t System::DEFAULT_RHYTHM_SLASH_SPACING_ABOVE             = 0;
const uint8_t System::DEFAULT_RHYTHM_SLASH_SPACING_BELOW             = 0;
const uint8_t System::DEFAULT_EXTRA_SPACING                          = 0;
const uint8_t System::SYSTEM_SYMBOL_SPACING = 18; ///< spacing given to a system symbol (i.e. rehearsal sign)
const uint8_t System::RHYTHM_SLASH_SPACING = 2 * System::SYSTEM_SYMBOL_SPACING;
const uint8_t System::CLEF_PADDING = 3; ///< padding surrounding a clef
const uint8_t System::CLEF_WIDTH = 22; ///< width of a clef

// Position Spacing Constants
const uint8_t System::MIN_POSITION_SPACING                           = 3;

// Constructor/Destructor
/// Default Constructor
System::System() :
    m_rect(DEFAULT_RECT), m_positionSpacing(DEFAULT_POSITION_SPACING),
    m_rhythmSlashSpacingAbove(DEFAULT_RHYTHM_SLASH_SPACING_ABOVE),
    m_rhythmSlashSpacingBelow(DEFAULT_RHYTHM_SLASH_SPACING_BELOW),
    m_extraSpacing(DEFAULT_EXTRA_SPACING),
    m_startBar(new Barline),
    m_endBar(new Barline)
{
}

// Serialize Functions
/// Performs serialization for the class
/// @param stream Power Tab output stream to serialize to
/// @return True if the object was serialized, false if not
bool System::Serialize(PowerTabOutputStream& stream) const
{
    //------Last Checked------//
    // - Jan 14, 2005
    stream.WriteMFCRect(m_rect);
    PTB_CHECK_THAT(stream.CheckState(), false);

    // Note: End bar is stored as a byte; we use Barline class to make it easier
    // for the user
    uint8_t endBar = (uint8_t)((m_endBar->GetType() << 5) |
        (m_endBar->GetRepeatCount()));
    stream << endBar << m_positionSpacing << m_rhythmSlashSpacingAbove <<
        m_rhythmSlashSpacingBelow << m_extraSpacing;
    PTB_CHECK_THAT(stream.CheckState(), false);

    m_startBar->Serialize(stream);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_directionArray);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_chordTextArray);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_rhythmSlashArray);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_staffArray);
    PTB_CHECK_THAT(stream.CheckState(), false);

    stream.WriteVector(m_barlineArray);
    PTB_CHECK_THAT(stream.CheckState(), false);

    return (stream.CheckState());
}

/// Performs deserialization for the class
/// @param stream Power Tab input stream to load from
/// @param version File version
/// @return True if the object was deserialized, false if not
bool System::Deserialize(PowerTabInputStream& stream, uint16_t version)
{
    // Version 1.0 and 1.0.2
    if (version == PowerTabFileHeader::Version_1_0 ||
        version == PowerTabFileHeader::Version_1_0_2)
    {
        uint8_t key;
        uint16_t endBar;

        stream.ReadMFCRect(m_rect);

        stream >> key >> endBar >> m_positionSpacing >>
            m_rhythmSlashSpacingAbove >> m_rhythmSlashSpacingBelow >>
            m_extraSpacing;

        // Update the key signature at start of section (always shown)
        uint8_t keyType = (uint8_t)((key >> 4) & 0xf);
        uint8_t keyAccidentals = (uint8_t)(key & 0xf);

        m_startBar->GetKeySignature().Show();

        // Cancellation
        if (keyType > 2)
        {
            m_startBar->GetKeySignature().SetCancellation();
        }

        keyType = (uint8_t)(((keyType % 2) == 1) ? KeySignature::majorKey :
            KeySignature::minorKey);

        m_startBar->GetKeySignature().SetKey(keyType, keyAccidentals);

        // Update the ending bar
        uint8_t barType = HIBYTE(endBar);
        uint8_t repeatCount = LOBYTE(endBar);

        m_endBar->SetBarlineData(barType, repeatCount);
        //SetEndBar(barType, repeatCount);

        stream.ReadVector(m_directionArray, version);
        stream.ReadVector(m_chordTextArray, version);
        stream.ReadVector(m_rhythmSlashArray, version);
        stream.ReadVector(m_staffArray, version);
        stream.ReadVector(m_barlineArray, version);

        // Any barline at position zero is now stored in the section m_startBar
        if (GetBarlineCount() > 0)
        {
            BarlinePtr barline = m_barlineArray[0];
            if (barline)
            {
                if (barline->GetPosition() == 0)
                {
                    *m_startBar = *barline;
                    m_barlineArray.erase(m_barlineArray.begin());
                }
            }
        }

        // Update key signs that aren't show to match active key sign
        KeySignature& activeKeySignature = m_startBar->GetKeySignature();

        for (auto &bar : m_barlineArray)
        {
            KeySignature &keySignature = bar->GetKeySignature();

            // Key on bar doesn't match active
            if (keySignature != activeKeySignature)
            {
                // Key isn't shown, update key to match
                if (!keySignature.IsShown())
                {
                    keySignature = activeKeySignature;
                    keySignature.Hide();
                    keySignature.SetCancellation(false);
                }

                // Update active key
                activeKeySignature = bar->GetKeySignature();
            }
        }
    }
    // Version 1.5 and up
    else
    {
        stream.ReadMFCRect(m_rect);

        uint8_t endBar = 0;
        stream >> endBar >> m_positionSpacing >> m_rhythmSlashSpacingAbove >>
            m_rhythmSlashSpacingBelow >> m_extraSpacing;

        // Update end bar (using Barline class is easier to use)
        m_endBar->SetBarlineData((uint8_t)((endBar & 0xe0) >> 5),
            (uint8_t)(endBar & 0x1f));

        m_startBar->Deserialize(stream, version);
        stream.ReadVector(m_directionArray, version);
        stream.ReadVector(m_chordTextArray, version);
        stream.ReadVector(m_rhythmSlashArray, version);
        stream.ReadVector(m_staffArray, version);
        stream.ReadVector(m_barlineArray, version);
    }

    return true;
}

const Rect &System::GetRect() const
{
    return m_rect;
}

/// Gets the bar at the start of the system
/// @return The start bar
System::BarlinePtr System::GetStartBar() const
{
    return m_startBar;
}

/// Determines if a barline index is valid
/// @param index barline index to validate
/// @return True if the barline index is valid, false if not
bool System::IsValidBarlineIndex(size_t index) const
{
    return index < GetBarlineCount();
}

/// Gets the number of barlines in the system
/// @return The number of barlines in the system
size_t System::GetBarlineCount() const
{
    return m_barlineArray.size();
}

/// Gets the nth barline in the system
/// @param index Index of the barline to get
/// @return The nth barline in the system
System::BarlinePtr System::GetBarline(size_t index) const
{
    PTB_CHECK_THAT(IsValidBarlineIndex(index), BarlinePtr());
    return m_barlineArray[index];
}

/// Gets the bar at the end of the system
/// @return The end bar
System::BarlinePtr System::GetEndBar() const
{
    return m_endBar;
}

/// Determines if a chord text index is valid
/// @param index chord text index to validate
/// @return True if the chord text index is valid, false if not
bool System::IsValidChordTextIndex(size_t index) const
{
    return index < GetChordTextCount();
}

/// Gets the number of chord text items in the system
/// @return The number of chord text items in the system
size_t System::GetChordTextCount() const
{
    return m_chordTextArray.size();
}

/// Gets the nth chord text item in the system
/// @param index Index of the chord text to get
/// @return The nth chord text item in the system
System::ChordTextPtr System::GetChordText(size_t index) const
{
    PTB_CHECK_THAT(IsValidChordTextIndex(index), ChordTextPtr());
    return m_chordTextArray[index];
}

// Staff Functions
/// Determines if a staff index is valid
/// @param index staff index to validate
/// @return True if the staff index is valid, false if not
bool System::IsValidStaffIndex(size_t index) const
{
    return index < GetStaffCount();
}

/// Gets the number of staffs in the system
/// @return The number of staffs in the system
size_t System::GetStaffCount() const
{
    return m_staffArray.size();
}

/// Gets the nth staff in the system
/// @param index Index of the staff to get
/// @return The nth staff in the system
System::StaffPtr System::GetStaff(size_t index) const
{
    PTB_CHECK_THAT(IsValidStaffIndex(index), StaffPtr());
    return m_staffArray[index];
}

// Direction Functions
/// Determines if a staff index is valid
/// @param index staff index to validate
/// @return True if the staff index is valid, false if not
bool System::IsValidDirectionIndex(size_t index) const
{
    return index < GetDirectionCount();
}

/// Gets the number of staffs in the system
/// @return The number of staffs in the system
size_t System::GetDirectionCount() const
{
    return m_directionArray.size();
}

/// Gets the nth direction in the system.
/// @param index Index of the direction to get.
/// @return The nth direction in the system.
System::DirectionPtr System::GetDirection(size_t index) const
{
    PTB_CHECK_THAT(IsValidDirectionIndex(index), DirectionPtr());
    return m_directionArray[index];
}

// Rhythm Slash Functions
/// Determines if a rhythm slash index is valid
/// @param index rhythm slash index to validate
/// @return True if the rhythm slash index is valid, false if not
bool System::IsValidRhythmSlashIndex(size_t index) const
{
    return index < GetRhythmSlashCount();
}

/// Gets the number of rhythm slashes in the system
/// @return The number of rhythm slashes in the system
size_t System::GetRhythmSlashCount() const
{
    return m_rhythmSlashArray.size();
}

/// Gets the nth rhythm slash in the system
/// @param index Index of the rhythm slash to get
/// @return The nth rhythm slash in the system
System::RhythmSlashPtr System::GetRhythmSlash(size_t index) const
{
    PTB_CHECK_THAT(IsValidRhythmSlashIndex(index), RhythmSlashPtr());
    return m_rhythmSlashArray[index];
}

/// Calculates the number of positions that will fit across the system based on
/// a given position spacing
/// @param positionSpacing Position spacing used to perform the calculation
/// @return The number of positions that will fit across the system
int System::CalculatePositionCount(int positionSpacing) const
{
    if (positionSpacing < MIN_POSITION_SPACING)
        return 0;

    int returnValue = 0;

    // The available width extends from the first position to the right side of
    // the system
    int width = m_rect.GetWidth();

    // Subtract spacing for the clef, etc.
    width -= GetFirstPositionX();

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
/// current position width.
/// @return The number of positions that can fit across the system.
int System::GetPositionCount() const
{
    // Calculate the position count using the current position spacing.
    return CalculatePositionCount(m_positionSpacing);
}

/// Gets the x co-ordinate of the first position in the system, relative to the
/// left edge
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
    int keySignatureWidth = m_startBar->GetKeySignature().GetWidth();
    returnValue += keySignatureWidth;

    // Add the width of the starting time signature
    int timeSignatureWidth = m_startBar->GetTimeSignature().GetWidth();
    returnValue += timeSignatureWidth;

    // If we have both a key and time signature, they are separated by 3 units
    if ((keySignatureWidth > 0) && (timeSignatureWidth > 0))
        returnValue += 3;

    // Add the width required by the starting barline; for a standard barline,
    // this is 1 unit of space, otherwise it is the distance between positions
    int barlineWidth = ((m_startBar->IsBar()) ? 1 : m_positionSpacing);
    returnValue += barlineWidth;

    return (returnValue);
}

/// Gets the x co-ordinate of the nth position in the system, relative to the
/// left edge of the system
/// @param position Zero-based index of the position to retrieve the x
/// co-ordinate for
/// @return The x co-ordinate for the position, or the x co-ordinate of the
/// first position if the position is invalid
int System::GetPositionX(int position) const
{
    // Initialize to the first position
    int returnValue = GetFirstPositionX();

    // Get the width of all key and time signatures up to, but not
    // including, the position
    int keyAndTimeSignatureWidth =
        GetCumulativeInternalKeyAndTimeSignatureWidth(position);

    // Move "n" positions across using the position spacing, adding the
    // cumulative key and time signature widths. Add 1 since the position
    // value is zero-based
    returnValue +=
        (((position + 1) * m_positionSpacing) + keyAndTimeSignatureWidth);

    return (returnValue);
}

/// Gets the position index for an x-coordinate in the system
/// @return The closest position to the given x-coordinate, or the first/last
/// position if it is out of range
size_t System::GetPositionFromX(int x) const
{
    if (GetPositionX(0) >= x)
    {
        return 0;
    }

    for (int i = 1; i < GetPositionCount(); i++)
    {
        if (GetPositionX(i) >= x)
        {
            return i - 1;
        }
    }

    // if the x-coordinate is past the last position, just return the last
    // position index
    return GetPositionCount() - 1;
}

/// Gets the total width used by all key and time signatures that reside within
/// the system (does not include the start bar)
/// @param position Zero-based index of the position to stop at. If -1, traverse
/// all the barlines
int System::GetCumulativeInternalKeyAndTimeSignatureWidth(int position) const
{
    int returnValue = 0;

    const bool bAllBarlines = (position == -1);

    // Loop through barline list
    const size_t numBarlines = m_barlineArray.size();
    for (size_t i = 0; i < numBarlines; ++i)
    {
        const Barline &barline = *(m_barlineArray[i]);
        // Get the position where the barline resides
        const int barlinePosition = barline.GetPosition();

        // Only use bars before the index
        if (bAllBarlines || (barlinePosition < position))
        {
            // Ignore keys and time signs at position 0, they're handled in
            // GetFirstPositionX
            if (barlinePosition > 0)
            {
                // Add the width of the key and time signature, if present on
                // the barline
                returnValue += barline.GetKeyAndTimeSignatureWidth();
            }
        }
        else
        {
            break;
        }
    }

    return (returnValue);
}
}
