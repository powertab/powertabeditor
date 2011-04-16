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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "barline.h"
#include "rect.h"

#include <vector>

class Direction;
class ChordText;
class RhythmSlash;
class Staff;

/// Stores and renders a system
class System : public PowerTabObject
{
    // Constants
public:
    // Default Constants
    static const Rect DEFAULT_RECT;
    static const uint8_t DEFAULT_POSITION_SPACING;
    static const uint8_t DEFAULT_RHYTHM_SLASH_SPACING_ABOVE;
    static const uint8_t DEFAULT_RHYTHM_SLASH_SPACING_BELOW;
    static const uint8_t DEFAULT_EXTRA_SPACING;
    static const uint8_t SYSTEM_SYMBOL_SPACING;

    // Position Spacing Constants
    static const uint8_t MIN_POSITION_SPACING;

    // Member Variables
protected:
    Rect  m_rect;				///< Bounding rect for the system
    uint8_t  m_positionSpacing;			///< Spacing between each position in the system
    uint8_t  m_rhythmSlashSpacingAbove;		///< Spacing above the rhythm slashes
    uint8_t  m_rhythmSlashSpacingBelow;		///< Spacing below the rhythm slashes
    uint8_t  m_extraSpacing;			///< Extra spacing used within the system (for rehearsal signs + tempo markers)

public:
    Barline             m_startBar;                             ///< Barline at the start of the system
    std::vector<Direction*>      m_directionArray;                       ///< List of directions used within the system
    std::vector<ChordText*>      m_chordTextArray;                       ///< List of chord text items used within the system
    std::vector<RhythmSlash*>    m_rhythmSlashArray;                     ///< List of rhythm slashes used within the system
    std::vector<Staff*>          m_staffArray;                           ///< List of staves used within the system
    std::vector<Barline*>        m_barlineArray;                         ///< List of barlines (not including start and end bars) used within the system
    Barline             m_endBar;                               ///< Barline at the end of the system (time and key signature are not used in this barline)

    // Constructor/Destructor
public:
    System();
    System(const System& system);
    ~System();

    void Init(const std::vector<uint8_t>& staffSizes);

    // Operators
    const System& operator=(const System& system);
    bool operator==(const System& system) const;
    bool operator!=(const System& system) const;

    // Serialzation Functions
    bool Serialize(PowerTabOutputStream& stream);
    bool Deserialize(PowerTabInputStream& stream, uint16_t version);

    // MFC Class Functions
public:
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
    {return (m_rect);}
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
    /// Gets the bar at the start of the system
    /// @return The start bar
    Barline GetStartBar() const
    {return (m_startBar);}
    /// Gets a reference to the bar at the start of the system
    /// @return A reference to the bar at the start of the system
    Barline& GetStartBarRef()
    {return (m_startBar);}
    /// Gets a constant reference to the bar at the start of the system
    /// @return A constant reference to the bar at the start of the system
    const Barline& GetStartBarConstRef() const
    {return (m_startBar);}

    /// Gets a pointer to the bar at the start of the system
    /// @return A pointer to the bar at the start of the system
    Barline* GetStartBarPtr()
    {
        return &m_startBar;
    }

    // Direction Functions
    /// Determines if a staff index is valid
    /// @param index staff index to validate
    /// @return True if the staff index is valid, false if not
    bool IsValidDirectionIndex(uint32_t index) const
    {return (index < GetDirectionCount());}
    /// Gets the number of staffs in the system
    /// @return The number of staffs in the system
    size_t GetDirectionCount() const
    {return (m_directionArray.size());}
    /// Gets the nth staff in the system
    /// @param index Index of the staff to get
    /// @return The nth staff in the system
    Direction* GetDirection(uint32_t index) const
    {
        CHECK_THAT(IsValidDirectionIndex(index), NULL);
        return (m_directionArray[index]);
    }

    size_t MaxDirectionSymbolCount() const;

    // Chord Text Functions
    /// Determines if a chord text index is valid
    /// @param index chord text index to validate
    /// @return True if the chord text index is valid, false if not
    bool IsValidChordTextIndex(uint32_t index) const
    {return (index < GetChordTextCount());}
    /// Gets the number of chord text items in the system
    /// @return The number of chord text items in the system
    size_t GetChordTextCount() const
    {return (m_chordTextArray.size());}
    /// Gets the nth chord text item in the system
    /// @param index Index of the chord text to get
    /// @return The nth chord text item in the system
    ChordText* GetChordText(uint32_t index) const
    {
        CHECK_THAT(IsValidChordTextIndex(index), NULL);
        return (m_chordTextArray[index]);
    }

    // Returns true if a ChordText item exists at the given position
    inline bool HasChordText(uint32_t position) const
    {
        return FindChordText(position) != -1;
    }

    int FindChordText(uint32_t position) const;
    bool InsertChordText(ChordText* chordText, uint32_t index);
    bool RemoveChordText(uint32_t index);

    // Rhythm Slash Functions
    /// Determines if a rhythm slash index is valid
    /// @param index rhythm slash index to validate
    /// @return True if the rhythm slash index is valid, false if not
    bool IsValidRhythmSlashIndex(uint32_t index) const
    {return (index < GetRhythmSlashCount());}
    /// Gets the number of rhythm slashes in the system
    /// @return The number of rhythm slashes in the system
    size_t GetRhythmSlashCount() const
    {return (m_rhythmSlashArray.size());}
    /// Gets the nth rhythm slash in the system
    /// @param index Index of the rhythm slash to get
    /// @return The nth rhythm slash in the system
    RhythmSlash* GetRhythmSlash(uint32_t index) const
    {
        CHECK_THAT(IsValidRhythmSlashIndex(index), NULL);
        return (m_rhythmSlashArray[index]);
    }

    // Staff Functions
    /// Determines if a staff index is valid
    /// @param index staff index to validate
    /// @return True if the staff index is valid, false if not
    bool IsValidStaffIndex(uint32_t index) const
    {return (index < GetStaffCount());}
    /// Gets the number of staffs in the system
    /// @return The number of staffs in the system
    size_t GetStaffCount() const
    {return (m_staffArray.size());}
    /// Gets the nth staff in the system
    /// @param index Index of the staff to get
    /// @return The nth staff in the system
    Staff* GetStaff(uint32_t index) const
    {CHECK_THAT(IsValidStaffIndex(index), NULL); return (m_staffArray[index]);}

    size_t FindStaffIndex(Staff* staff) const;

    // Barline Functions
    /// Determines if a barline index is valid
    /// @param index barline index to validate
    /// @return True if the barline index is valid, false if not
    bool IsValidBarlineIndex(uint32_t index) const
    {return (index < GetBarlineCount());}
    /// Gets the number of barlines in the system
    /// @return The number of barlines in the system
    size_t GetBarlineCount() const
    {return (m_barlineArray.size());}
    /// Gets the nth barline in the system
    /// @param index Index of the barline to get
    /// @return The nth barline in the system
    Barline* GetBarline(uint32_t index) const
    {
        CHECK_THAT(IsValidBarlineIndex(index), NULL);
        return (m_barlineArray[index]);
    }
    Barline* GetBarlineAtPosition(uint32_t position) const;
    Barline* GetPrecedingBarline(uint32_t position) const;
    Barline* GetNextBarline(uint32_t position) const;
    void GetBarlines(std::vector<const Barline*>& barlineArray) const;

    bool InsertBarline(Barline* barline);
    bool RemoveBarline(uint32_t position);

    bool HasRehearsalSign() const;

    // End Bar Functions
    /// Gets the bar at the end of the system
    /// @return The end bar
    Barline GetEndBar() const
    {return (m_endBar);}
    /// Gets a reference to the bar at the end of the system
    /// @return A reference to the bar at the end of the system
    Barline& GetEndBarRef()
    {return (m_endBar);}
    /// Gets a constant reference to the bar at the end of the system
    /// @return A constant reference to the bar at the end of the system
    const Barline& GetEndBarConstRef() const
    {return (m_endBar);}

    // Position Functions
    bool IsValidPosition(int position) const;
    int CalculatePositionCount(int nPositionSpacing) const;
    int GetPositionCount() const;
    int GetFirstPositionX() const;
    int GetPositionX(int position) const;
    size_t GetPositionFromX(int x) const;

    // Returns the width of a clef
    int GetClefWidth() const
    {
        return 22;
    }
    // Returns the width of a clef
    int GetClefPadding() const
    {
        return 3;
    }

    // Operations
    int GetCumulativeInternalKeyAndTimeSignatureWidth(int position = -1) const;

    void CalculateHeight();
    uint32_t GetStaffHeightOffset(uint32_t staff, bool absolutePos = false) const;

    void CalculateBeamingForStaves();

    void ShiftForward(uint32_t positionIndex);
    void ShiftBackward(uint32_t positionIndex);

protected:
    void PerformPositionShift(uint32_t positionIndex, int offset);
};

#endif
