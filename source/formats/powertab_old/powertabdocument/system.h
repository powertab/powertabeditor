/////////////////////////////////////////////////////////////////////////////
// Name:            system.h
// Purpose:         Stores and renders a system
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEM_H
#define SYSTEM_H

#include "rect.h"
#include "powertabobject.h"

#include <memory>
#include <vector>

namespace PowerTabDocument {

class Direction;
class ChordText;
class RhythmSlash;
class Staff;
class Barline;

/// Stores and renders a system
class System : public PowerTabObject
{
    friend class Score;

public:
    // Default Constants
    static const Rect DEFAULT_RECT;
    static const uint8_t DEFAULT_POSITION_SPACING;
    static const uint8_t DEFAULT_RHYTHM_SLASH_SPACING_ABOVE;
    static const uint8_t DEFAULT_RHYTHM_SLASH_SPACING_BELOW;
    static const uint8_t DEFAULT_EXTRA_SPACING;
    static const uint8_t SYSTEM_SYMBOL_SPACING;
    static const uint8_t RHYTHM_SLASH_SPACING;
    static const uint8_t CLEF_PADDING;
    static const uint8_t CLEF_WIDTH;

    // Position Spacing Constants
    static const uint8_t MIN_POSITION_SPACING;

    typedef std::shared_ptr<Direction> DirectionPtr;
    typedef std::shared_ptr<ChordText> ChordTextPtr;
    typedef std::shared_ptr<RhythmSlash> RhythmSlashPtr;
    typedef std::shared_ptr<Staff> StaffPtr;
    typedef std::shared_ptr<const Staff> StaffConstPtr;
    typedef std::shared_ptr<Barline> BarlinePtr;
    typedef std::shared_ptr<const Barline> BarlineConstPtr;

    // Member Variables
private:
    Rect  m_rect;				///< Bounding rect for the system
    uint8_t  m_positionSpacing;			///< Spacing between each position in the system
    uint8_t  m_rhythmSlashSpacingAbove;		///< Spacing above the rhythm slashes
    uint8_t  m_rhythmSlashSpacingBelow;		///< Spacing below the rhythm slashes
    uint8_t  m_extraSpacing;			///< Extra spacing used within the system (for rehearsal signs + tempo markers)

    BarlinePtr                      m_startBar;         ///< Barline at the start of the system (uses shared_ptr for consistency with barlines from m_barlineArray)
    std::vector<DirectionPtr>       m_directionArray;   ///< List of directions used within the system
    std::vector<ChordTextPtr>       m_chordTextArray;   ///< List of chord text items used within the system
    std::vector<RhythmSlashPtr>     m_rhythmSlashArray; ///< List of rhythm slashes used within the system
    std::vector<StaffPtr>           m_staffArray;       ///< List of staves used within the system
    std::vector<BarlinePtr>         m_barlineArray;     ///< List of barlines (not including start and end bars) used within the system
    BarlinePtr                      m_endBar;           ///< Barline at the end of the system (time and key signature are not used in this barline)

    // Constructor/Destructor
public:
    System();

    // Serialzation Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {return "CSection";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {return ((uint16_t)1);}

    BarlinePtr GetStartBar() const;
    BarlinePtr GetEndBar() const;

    // Direction Functions
    bool IsValidDirectionIndex(size_t index) const;
    size_t GetDirectionCount() const;
    DirectionPtr GetDirection(size_t index) const;

    // Chord Text Functions
    bool IsValidChordTextIndex(size_t index) const;
    size_t GetChordTextCount() const;
    ChordTextPtr GetChordText(size_t index) const;

    // Rhythm Slash Functions
    bool IsValidRhythmSlashIndex(size_t index) const;
    size_t GetRhythmSlashCount() const;
    RhythmSlashPtr GetRhythmSlash(size_t index) const;

    // Staff Functions
    bool IsValidStaffIndex(size_t index) const;
    size_t GetStaffCount() const;
    StaffPtr GetStaff(size_t index) const;

    // Barline Functions
    bool IsValidBarlineIndex(size_t index) const;
    size_t GetBarlineCount() const;
    BarlinePtr GetBarline(size_t index) const;

    // Layout Functions.
    const Rect &GetRect() const;
    int CalculatePositionCount(int positionSpacing) const;
    int GetCumulativeInternalKeyAndTimeSignatureWidth(int position = -1) const;
    int GetPositionCount() const;
    int GetFirstPositionX() const;
    int GetPositionX(int position) const;
    size_t GetPositionFromX(int x) const;
};

}

#endif // SYSTEM_H
