/////////////////////////////////////////////////////////////////////////////
// Name:            staff.h
// Purpose:         Stores and renders a staff
// Author:          Brad Larsen
// Modified by:
// Created:         Dec 16, 2004
// RCS-ID:
// Copyright:       (c) Brad Larsen
// License:         wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef STAFF_H
#define STAFF_H

#include <array>
#include "powertabobject.h"
#include <vector>

namespace PowerTabDocument {

class Position;

/// Stores and renders a staff
class Staff : public PowerTabObject
{
    // Constants
public:
    // Default Constants
    static const uint8_t DEFAULT_DATA;                                   ///< Default value for the data member variable
    static const uint8_t DEFAULT_CLEF;                                   ///< Default clef type
    static const uint8_t DEFAULT_TABLATURE_STAFF_TYPE;                   ///< Default tablature staff type
    static const uint8_t DEFAULT_STANDARD_NOTATION_STAFF_ABOVE_SPACING;  ///< Default value for the standard notation staff above spacing member variable
    static const uint8_t DEFAULT_STANDARD_NOTATION_STAFF_BELOW_SPACING;  ///< Default value for the standard notation staff below spacing member variable
    static const uint8_t DEFAULT_SYMBOL_SPACING;                         ///< Default value for the symbol spacing member variable
    static const uint8_t DEFAULT_TABLATURE_STAFF_BELOW_SPACING;          ///< Default value for the tablature staff below spacing member variable
    static const uint8_t STD_NOTATION_LINE_SPACING;
    static const uint8_t STD_NOTATION_STAFF_TYPE;
    static const uint8_t STAFF_BORDER_SPACING; /// padding around top and bottom of staves
    static const uint8_t TAB_SYMBOL_HEIGHT; /// height allocated for drawing a tab symbol 

    // Clef Constants
    static const uint8_t TREBLE_CLEF;                                ///< Treble clef
    static const uint8_t BASS_CLEF;                                  ///< Bass clef

    // Tablature Staff Type Constants
    static const uint8_t MIN_TABULATURE_STAFF_TYPE;                   ///< Minimum allowed value for tablature staff type
    static const uint8_t MAX_TABULATURE_STAFF_TYPE;                   ///< Maximum allowed value for tablature staff type

    enum flags
    {
        clefMask                    = 0xf0,                 ///< Mask used to retrieve the clef type
        tablatureStaffTypeMask      = 0xf                   ///< Mask used to retrieve the tablature type (3 - 7 string)
    };

    enum { NUM_STAFF_VOICES = 2 }; ///< Number of voices in a staff

    // Member Variables
private:
    uint8_t m_data;                                 ///< Top 4 bits = clef type, bottom 4 bits = tablature type
    uint8_t m_standardNotationStaffAboveSpacing;    ///< Amount of space alloted from the top line of the standard notation staff
    uint8_t m_standardNotationStaffBelowSpacing;    ///< Amount of space alloted from the last line of the standard notation staff
    uint8_t m_symbolSpacing;                        ///< Amount of space alloted for symbols located between the standard notation and tablature staff
    uint8_t m_tablatureStaffBelowSpacing;           ///< Amount of space alloted from the last line of the tablature staff
    bool m_isShown;

public:
    std::array<std::vector<Position*>, NUM_STAFF_VOICES> positionArrays; ///< collection of position arrays, one per voice

    // Constructor/Destructor
public:
    Staff();
    Staff(uint8_t tablatureStaffType, uint8_t clef);
    ~Staff();

    // Serialize Functions
    bool Serialize(PowerTabOutputStream &stream) const override;
    bool Deserialize(PowerTabInputStream &stream, uint16_t version) override;

public:
    /// Gets the MFC Class Name for the object
    /// @return The MFC Class Name
    std::string GetMFCClassName() const override
    {
        return "CStaff";
    }

    /// Gets the MFC Class Schema for the object
    /// @return The MFC Class Schema
    uint16_t GetMFCClassSchema() const override
    {
        return 1;
    }

    bool IsShown() const;

    // Clef Functions
    static bool IsValidClef(uint8_t clef);
    bool SetClef(uint8_t type);
    uint8_t GetClef() const;

    bool IsValidTablatureStaffType(uint8_t type) const;
    bool SetTablatureStaffType(uint8_t type);
    uint8_t GetTablatureStaffType() const;

    static bool IsValidVoice(size_t voice);

    // Position Functions
    bool IsValidPositionIndex(size_t voice, size_t index) const;
    size_t GetPositionCount(size_t voice) const;
    Position* GetPosition(size_t voice, size_t index) const;
};

}

#endif // STAFF_H
