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

#include <vector>
#include <boost/shared_ptr.hpp>

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

    typedef boost::shared_ptr<Direction> DirectionPtr;
    typedef boost::shared_ptr<ChordText> ChordTextPtr;
    typedef boost::shared_ptr<RhythmSlash> RhythmSlashPtr;
    typedef boost::shared_ptr<Staff> StaffPtr;
    typedef boost::shared_ptr<const Staff> StaffConstPtr;
    typedef boost::shared_ptr<Barline> BarlinePtr;
    typedef boost::shared_ptr<const Barline> BarlineConstPtr;

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
    System(const System& system);

    void Init(const std::vector<uint8_t>& staffSizes);

    // Operators
    const System& operator=(const System& system);
    bool operator==(const System& system) const;
    bool operator!=(const System& system) const;

    // Serialzation Functions
    bool Serialize(PowerTabOutputStream& stream) const;
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    // MFC Class Functions
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const
    {return "CSection";}
    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const
    {return ((uint16_t)1);}

    // Rect Functions
    /// Gets the bounding rect for the system
    /// @return The bounding rect for the system
    Rect GetRect() const
    {
        return m_rect;
    }
    void SetRect(const Rect& rect)
    {
        m_rect = rect;
    }

    // Position Spacing Functions
    bool IsValidPositionSpacing(int positionSpacing) const;

    /// Gets the amount of spacing between positions
    /// @return The amount of spacing between positions
    uint32_t GetPositionSpacing() const
    {return (m_positionSpacing);}

    bool SetPositionSpacing(uint8_t positionSpacing);

    // Rhythm Slash Spacing Above Functions
    /// Gets the amount of spacing above the rhythm slashes (when slashes are
    /// used)
    /// @return The amount of spacing above the rhythm slashes
    uint32_t GetRhythmSlashSpacingAbove() const
    {return (m_rhythmSlashSpacingAbove);}

    // Rhythm Slash Spacing Below Functions
    /// Gets the amount of spacing below the rhythm slashes (when slashes are
    /// used)
    /// @return The amount of spacing below the rhythm slashes
    uint32_t GetRhythmSlashSpacingBelow() const
    {return (m_rhythmSlashSpacingBelow);}

    // Extra Spacing Functions
    /// Gets the extra spacing (used by rehearsal signs, tempo markers, etc.)
    /// @return The amount of extra spacing
    uint32_t GetExtraSpacing() const
    {return (m_extraSpacing);}

    void SetExtraSpacing(uint32_t spacing)
    {
        m_extraSpacing = spacing;
        CalculateHeight();
    }

    // Start Bar Functions
    BarlinePtr GetStartBar() const;
    void SetStartBar(BarlinePtr barline);

    // Direction Functions
    bool IsValidDirectionIndex(uint32_t index) const;
    size_t GetDirectionCount() const;
    DirectionPtr GetDirection(uint32_t index) const;

    size_t MaxDirectionSymbolCount() const;

    // Chord Text Functions
    bool IsValidChordTextIndex(uint32_t index) const;
    size_t GetChordTextCount() const;
    ChordTextPtr GetChordText(uint32_t index) const;

    bool HasChordText(uint32_t position) const;
    int FindChordText(uint32_t position) const;
    bool InsertChordText(ChordTextPtr chordText, uint32_t index);
    bool RemoveChordText(uint32_t index);

    // Rhythm Slash Functions
    bool IsValidRhythmSlashIndex(uint32_t index) const;
    size_t GetRhythmSlashCount() const;
    RhythmSlashPtr GetRhythmSlash(uint32_t index) const;

    // Staff Functions
    bool IsValidStaffIndex(uint32_t index) const;
    size_t GetStaffCount() const;
    StaffPtr GetStaff(uint32_t index) const;

    size_t FindStaffIndex(StaffConstPtr staff) const;

    // Barline Functions
    bool IsValidBarlineIndex(uint32_t index) const;
    size_t GetBarlineCount() const;
    BarlinePtr GetBarline(uint32_t index) const;

    BarlinePtr GetBarlineAtPosition(uint32_t position) const;
    BarlinePtr GetPrecedingBarline(uint32_t position) const;
    BarlinePtr GetNextBarline(uint32_t position) const;
    void GetBarlines(std::vector<BarlineConstPtr>& barlineArray) const;
    void GetBarlines(std::vector<BarlinePtr>& barlineArray);

    bool InsertBarline(BarlinePtr barline);
    bool RemoveBarline(uint32_t position);

    bool HasRehearsalSign() const;

    // End Bar Functions
    BarlinePtr GetEndBar() const;

    // Position Functions
    bool IsValidPosition(int position) const;
    int CalculatePositionCount(int nPositionSpacing) const;
    int GetPositionCount() const;
    int GetFirstPositionX() const;
    int GetPositionX(int position) const;
    size_t GetPositionFromX(int x) const;

    // Operations
    int GetCumulativeInternalKeyAndTimeSignatureWidth(int position = -1) const;

    void CalculateHeight();
    uint32_t GetStaffHeightOffset(uint32_t staff, bool absolutePos = false) const;

    void CalculateBeamingForStaves();

    void ShiftForward(uint32_t positionIndex);
    void ShiftBackward(uint32_t positionIndex);

    bool HasMultiBarRest(BarlineConstPtr startBar, uint8_t& measureCount) const;

private:
    void PerformPositionShift(uint32_t positionIndex, int offset);
};

#endif // SYSTEM_H
